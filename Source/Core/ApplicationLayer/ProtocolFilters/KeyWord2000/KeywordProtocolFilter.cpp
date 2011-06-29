//******************************************************************************
// Description:
//  Keyword 2000 protocol filter.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/KeyWord2000/KeywordProtocolFilter.cpp $
// 
// 6     10/25/06 3:37p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a timer to tell when the last transmit message was sent.
// This
//       allows the keep alive message to be sent when needed instead of
// on a
//       specific time interval.
// 
// 5     5/11/05 10:03p Cward
// Updated for 05051101 iteration
//
// 4     5/09/05 5:00p Gpattison
// Added validation of checksum and additional array bounds checking
// to prevent a fault from accessing an array out of bounds.
//
// 4     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     10/26/04 12:43p Bmeinke
// Fixed he "Busy, try again" processing insode GetModuleData()
//
// 2     10/15/04 2:05p Bmeinke
// Read the "Module Busy" negative response code in Inisialize()
// Override GetmoduleData() so we can process a "Module Busy" response and
// resend our message
// Add SetModuleBusyCode() and GetModuleBusyCode() methods
//
// 3     6/09/04 11:05p Cward
// Added default paramater to the constructor. Removed the const
// declaration from the parameter to the Extract Module Data Method.
// Updated the header comment block to the new standard.
//
// 2     5/25/04 3:19p Cward
// Extract Module Data needs to use the Header length to deterime the data
// response length.
//
// 1     12/21/03 6:33p Cward
//
// 1     7/08/03 1:05p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:28p Khykin
// Initial checkin
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "KeywordProtocolFilter.h"

/**
 * Class constructor.
 * @since Version 1.0
 */
KeywordProtocolFilter::KeywordProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar, BepMutex *commsInUse/* = NULL*/) : ProtocolFilter(lastTxTime, stopCommsBepCondVar, commsInUse)
{
};
/**
 * Class destructor.
 * @since Version 1.0
 */
KeywordProtocolFilter::~KeywordProtocolFilter()
{
};

bool KeywordProtocolFilter::Initialize(const XmlNode *config)
{
	try
	{	// Find out if we should allow "negative response" messages that indicate "Routine Not Complete"
		SetAllowRoutineNotComplete(atob(config->getChild("Setup/AllowRoutineNotComplete")->getValue().c_str()));
	}
	catch(XmlException &err)
	{
		SetAllowRoutineNotComplete(false);
	}
	try
	{	// Get the code being used for module busy, repeat request
		SetModuleBusyCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/ModuleBusy")->getValue().c_str()));
	}
	catch(XmlException &err)
	{
		SetModuleBusyCode(0x21);
	}
	// Call the base class to complete the initialization
	return ProtocolFilter::Initialize(config);
}
/**
 * Send a message to the module and read the response.
 *
 * @param messageTag Tag of message to send.
 * @param reply      Reply from the module.
 * @param args       Optional data to be written to the module.
 * @return Status of the operation.
 * @since Version 1.0
 */
const BEP_STATUS_TYPE KeywordProtocolFilter::GetModuleData(std::string messageTag, SerialString_t &reply,
														   SerialArgs_t *args/* = NULL*/)
{
	BEP_STATUS_TYPE status=BEP_STATUS_ERROR;
	int             retries=100;
	const UINT16        negCodeIdx = GetSentMessageIdentifierIndex();
	const UINT16        reasonIdx = GetNegativeResponseCodeIndex();

	Log(LOG_FN_ENTRY, "Enter KeywordProtocolFilter::GetModuleData(%s)\n",
		messageTag.c_str());

	while( retries > 0 && !GetStopCommsFlag())
	{
		// Let the base class do the work for us
		status = ProtocolFilter::GetModuleData(messageTag, reply, args);
		// If we received a negative response code of "Busy, try again"
		if(((status == BEP_STATUS_ERROR) || (status == BEP_STATUS_FAILURE)) &&
		   (reply.length() > negCodeIdx))
		{
			PrintSerialString( "Response Data", reply, LOG_ALL);
			if( reply[ negCodeIdx] == GetNegativeResponseValue())
			{
				if(reply.size() > reasonIdx)
				{
					if( reply[ reasonIdx] == GetModuleBusyCode())
					{
						Log( LOG_DEV_DATA|LOG_ERRORS, "Received a \"Busy, try again\" negative response, %d attempts remaining\n",
							 retries);
						// Loop around and try again
						retries--;
					}
					else
					{
						Log( LOG_ERRORS, "Got a negative response ($%02X), but not a code $%02X\n", reply[reasonIdx] ,GetModuleBusyCode());
						retries = 0;
					}
				}
				else
				{
					Log( LOG_ERRORS, "Got a negative response, but reply too short (%d) to indicate why\n", reply.size());
					retries = 0;
				}
			}
			else
			{
				Log( LOG_ERRORS, "Failed sending message %s, but not a negative response code\n", messageTag.c_str());
				retries = 0;
			}
		}
		else
		{
			retries = 0;
		}
	}

	Log(LOG_FN_ENTRY, "Exit KeywordProtocolFilter::GetModuleData(%s), status=%d\n",
		messageTag.c_str(), status);

	return( status);
}

/**
 * Extract the data from the response.
 *
 * @param moduleResponse
 *               Response from the module.
 * @return The data that was included in the response.
 * @since Version 1.0
 */
const SerialString_t KeywordProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{
	SerialString_t dataResponse;
	bool validChecksum = true;

	// Log the respnse received from the module
	PrintSerialString("Extracting data from response string", moduleResponse, LOG_ALL);


	// Validate the Checksum if turned on by configuration file
	if(IsChecksumValidationRequired())
	{
		validChecksum = IsChecksumValid(moduleResponse);
	}
	Log(LOG_DEV_DATA, "Checksum valid: %s", validChecksum ? "True" : "False");

	if(validChecksum)
	{
		// ensure the response is of proper length to prevent sig seg
		if((moduleResponse.length() > 3) && (moduleResponse.length() > GetDataStartIndex()))
		{
			Log(LOG_DEV_DATA, "Message length is valid - %d", moduleResponse.length());
			if((GetDataHeaderLength() == 4) || (moduleResponse[0] == 0x80))
			{
				// Strip off the header and checksum
				dataResponse = moduleResponse.substr(4,moduleResponse[3]);
			}
			else
			{
				// Strip off the header and checksum
				dataResponse = moduleResponse.substr(GetDataStartIndex(),(moduleResponse[0]&0x3F));
			}
		}
		else
		{
			Log(LOG_ERRORS,"ERROR Module response length of %d is to short to be valid\n",
				moduleResponse.length());
		}

		PrintSerialString("ExtractModuleData() Extracted module data", dataResponse, LOG_ALL);
	}
	else
	{
		Log(LOG_ERRORS, "KeywordProtocolFilter::ExtractModuleData - Invalid Checksum on message, not extracting response\n");
		dataResponse.clear();
	}
	// Return the extracted data
	return dataResponse;
}

/**
 * Read a module response to the message with the given tag.
 *
 * @param messageTag Tag of the message response to read.
 * @param reply      Reply from the module.
 * @return Status of the operation.
 * @since Version 1.0
 */
const BEP_STATUS_TYPE KeywordProtocolFilter::GetResponse(const std::string &messageTag, SerialString_t &reply)
{
	BEP_STATUS_TYPE status = BEP_STATUS_NA;
	SerialString_t messageBuffer;	// buffer to hold the message that is received

	Log(LOG_FN_ENTRY, "Entered GetResponse\n");

	INT32 tries = GetNumberOfRetries() + 1;
	INT32 bytesRead = 0;	// reset the number of bytes read
	INT32 retry = 0;		// the retry counter

	do
	{
		// Get the response from the module
		bytesRead = WaitForFullResponse(messageBuffer);
		Log(LOG_DEV_DATA, "Received %d bytes for message %s\n", bytesRead, messageTag.c_str());
		// if a response was received
		if(bytesRead > 0)
		{	// Log the response, if development flag set
			if(GetLogStatus() && (GetVerboseMask() & LOG_DEV_DATA))
			{
				std::string response;
				char buffer[128];		// buffer to create messages with
				for(INT16 ii = 0; ii < bytesRead; ii++)
					response += CreateMessage(buffer, sizeof(buffer), "$%02X ", messageBuffer[ii]);
				Log("Response bytes received: %d for message: %s\tResponse: %s\n\n",
					bytesRead, messageTag.c_str(), response.c_str());
			}

			int attempts = 0;	// attempt number counter
			do
			{
				// Read the first full response from the message buffer
				reply = ExtractMessage(messageBuffer);
				// Validate the response against all filters
				if(IsResponseValid(messageTag, reply))
				{
					// Check to ensure the reply is long enough to contain a neg
					//    response
					if(reply.length() > 3)
					{
						// if a negative response was received, handle it
						if(reply[3] == 0x7F)
						{
							// Check to ensure the reply is long enough to contain a neg
							//    response
							if(reply.length() > 5)
							{
								// determine if it is just a processing request response
								if(reply[5] == 0x78)
								{
									Log(LOG_DEV_DATA, "Received a processing request message, resetting the retry counter\n");
									status = BEP_STATUS_NA;

									// we received a valid "response pending" message, so reset the retry counters
									attempts=0;
									retry=0;
								}
								else if((reply[5] == 0x23) && (GetAllowRoutineNotComplete()))
								{
									status = BEP_STATUS_SUCCESS;
									Log(LOG_DEV_DATA, "Routine Not Completed message recieved, but ignoring\n");
								}
								// else, it is a negative response and we are done processing
								else
								{
									status = BEP_STATUS_FAILURE;
									Log(LOG_ERRORS, "Negative Response Received: 0x%2X\n", reply[5]);
								}
							}
							else
							{
								Log(LOG_ERRORS,"ERROR Reply length of %d is to short to have neg response code\n",
									reply.length());
								status = BEP_STATUS_FAILURE;
							}
						}
						// else it is a positive response and we are done processing
						else
						{
							status = BEP_STATUS_SUCCESS;
							Log(LOG_DEV_DATA, "Valid response received\n");
						}
					}
					else
					{
						Log(LOG_ERRORS,"ERROR Reply length of %d is to short to be valid\n",
							reply.length());
						status = BEP_STATUS_FAILURE;
					}
				}
				// if the response is not valid, then we are done
				else
				{
					Log(LOG_ERRORS, "Response is NOT valid\n");
					status = BEP_STATUS_FAILURE;
				}
				// process all of the received information until the response is received, or three attempts are made
			}while((status == BEP_STATUS_NA) && (attempts++ < tries) && !GetStopCommsFlag());

			// determine if not successful
			if(status != BEP_STATUS_SUCCESS)
			{
				retry++;		// increment retries
				// if retrys have not been exhausted, reset the status
				if(retry < tries)
				{
					status = BEP_STATUS_NA;
					Log(LOG_DEV_DATA, "Partial message received, going back for more\n");
				}
				else
					Log(LOG_ERRORS, "Done Looking, only partial message received\n");

			}
		}
		// else it is not looking good
		else
		{
			// if a problem detected reading the port
			if(bytesRead < 0)
			{	// Error reading data from the port
				status = BEP_STATUS_ERROR;
			}
			// else if the reply is empty and already performed another attempt
			else if(reply.empty() && retry)
			{	// No data sent from the module
				status = BEP_STATUS_FAILURE;
			}
			// else wait the full retry attempts to see the message received
			else
			{
				Log(LOG_DEV_DATA, "Waiting for full message\n");
			}
			retry++;	// increment the retry counter
		}
		// while the result not detemined and retries remain
	}while((status == BEP_STATUS_NA) && (retry < tries) && !GetStopCommsFlag());

	// if a successful read never occurred, fail
	if(retry >= tries)	status = BEP_STATUS_FAILURE;

	Log(LOG_FN_ENTRY, "Exit GetResponse: %d\n", status);

	// Return the status
	return status;
}

/**
 * Parse a received message and return the first full message that was
 * received.  Note:  This will remove the message from the moduleResponse
 * and leave only the remaining characters.
 *
 * @param moduleResponse Response from the module.
 * @return First message from the response.
 *         Note: If a valid message is not received, a blank string is returned
 * @since Version 1.0
 */
const SerialString_t KeywordProtocolFilter::ExtractMessage(SerialString_t &moduleResponse)
{
	std::string     fullResponse;	// buffer for displaying logging information
	SerialString_t  dataResponse;	// the data that is left in the response
	char buffer[128];				// buffer to create messages with
	bool separateLengthByte=false;	// flag to tell if there is a separate length byte

	if(GetLogStatus() && (GetVerboseMask() & LOG_DEV_DATA))
	{	// Log the respnse received from the module
		for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
			fullResponse += CreateMessage(buffer, sizeof(buffer), "$%02X ", moduleResponse[ii]);
		Log("Extracting data from response string: %s\n", fullResponse.c_str());
	}

	// locate the start of the message
	bool messageFound = false;
	for(UINT16 ii = 0; (ii < moduleResponse.length()) && (!messageFound); ii++)
	{
		// if this is a message headder
		if((moduleResponse[ii] & 0x80) == 0x80)
		{
			messageFound = true;			// flag as found
			moduleResponse.erase(0, ii);	// remove the unused characters
		}
	}
	// if a message was found
	if(messageFound && (moduleResponse.length() > 3))
	{
		unsigned int messageCount=0;

		// check if there is a separate length byte
		if((GetDataHeaderLength() == 4) || (moduleResponse[0] == 0x80))	separateLengthByte = true;
		else separateLengthByte	= false;

		if(separateLengthByte == false)
		{
			// get the number of bytes in the first response (header + data + cs)
			messageCount = 3 + (moduleResponse[0] - 0x80) + 1;
		}
		else						   // separate length byte
		{
			// get the number of bytes in the first response (header + data + cs)
			messageCount = 4 + moduleResponse[3] + 1;
		}

		// if a full message has been received
		if(moduleResponse.length() >= messageCount)
		{	// retrieve the message
			dataResponse = moduleResponse.substr(0,messageCount);
			// remove the parsed data from the moduleResponse
			moduleResponse.erase(0, messageCount);

			if(GetLogStatus() && (GetVerboseMask() & LOG_DEV_DATA))
			{	// Log the data extracted from the module
				fullResponse.erase();
				for(UINT16 ii = 0; ii < dataResponse.length(); ii++)
					fullResponse += CreateMessage(buffer, sizeof(buffer), "$%02X ", dataResponse[ii]);
				Log("ExtractModuleData() Extracted module data: %s\n", fullResponse.c_str());
			}
		}
		// else, cant do anything
		else
		{
			Log(LOG_ERRORS, "Not Enough Data To Process: expected: %d, received: %d\n", messageCount, moduleResponse.length());
		}
	}
	// else a valid message was not received
	else
		Log(LOG_ERRORS, "Valid message not received\n");

	// Return the extracted data
	return dataResponse;
}

bool KeywordProtocolFilter::IsChecksumValid(SerialString_t &moduleResponse)
{
	UINT8 calculatedChecksum = 0x00;
	UINT8 responseChecksum = 0x00;
	responseChecksum = moduleResponse[(moduleResponse.length()-1)];
	// Determine how to compute the checksum
	if(!ChecksumCalculationMethod().compare(ChecksumCalculationXor))
	{
		for(UINT16 ii = 0; ii < moduleResponse.length() - 1; ii++)
		{
			calculatedChecksum ^= moduleResponse[ii];
		}
	}
	else
	{
		for(UINT16 ii = 0; ii < moduleResponse.length() - 1; ii++)
		{
			calculatedChecksum += moduleResponse[ii];
		}
	}
	// Compare the calculated vs module response checksum
	return(responseChecksum == calculatedChecksum);
}

const BEP_STATUS_TYPE KeywordProtocolFilter::CheckForValidResponse(const SerialString_t &moduleResponse)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	UINT16 offset = GetValidResponseOffset();
	UINT8 msgIndex = (moduleResponse[0] == 0x80) ? 4 : 3;
	// ensure the response is not an invalid length that could cause a sig seg by dereferencing it
	if(GetSentMessageIdentifierIndex() < moduleResponse.length())
	{
		Log(LOG_DEV_DATA, "KeywordProtocolFilter::CheckForValidResponse() -- GetSentMessageIdentifier(): $%02X + offset{%02X}: $%02X == $%02X\n",
			GetSentMessageIdentifier(), offset, (GetSentMessageIdentifier() + offset), moduleResponse[msgIndex]);
		if((GetSentMessageIdentifier() + offset) == moduleResponse[msgIndex])
		{
			status = BEP_STATUS_SUCCESS;
		}
		else status	= BEP_STATUS_FAILURE;
	}
	else
	{
		Log(LOG_ERRORS, "ERROR Module Response length shorter than Identifier Index "
			"Response Length: %d\n\tIdentifier Index: %d\n", moduleResponse.length(), msgIndex);
		status = BEP_STATUS_ERROR;
	}
	return(status);
}

inline void KeywordProtocolFilter::SetModuleBusyCode(const UINT8 &moduleBusyCode)
{
	// Save the module busy, repeat request code
	m_moduleBusyCode = moduleBusyCode;
}

inline const UINT8& KeywordProtocolFilter::GetModuleBusyCode(void)
{
	// Return the code for module busy, repeat request
	return m_moduleBusyCode;
}

inline void KeywordProtocolFilter::SetAllowRoutineNotComplete(const bool allowRoutineNotComplete)
{
	// Save the AllowRoutineNotComplete config status
	m_allowRoutineNotComplete = allowRoutineNotComplete;
}

inline const bool KeywordProtocolFilter::GetAllowRoutineNotComplete(void)
{
	// Return the AllowRoutineNotComplete config status
	return m_allowRoutineNotComplete;
}

