//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/ProtocolFilters/KeyWord2000/DcxCANKeyword2000ProtocolFilter.cpp 3     12/06/06 1:29p Derickso $
//
// FILE DESCRIPTION:
//	Protocol Filter for handling Keyword 2000 Protocol as implemented by DaimlerChrysler
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/ProtocolFilters/KeyWord2000/DcxCANKeyword2000ProtocolFilter.cpp $
// 
// 3     12/06/06 1:29p Derickso
// Moved typedef to base class and added SetLastTxTime to Send function.
// 
// 2     11/14/06 3:38p Bbarrett
// Latest core components do not die.
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 1     12/01/04 2:36p Derickso
// Created new VSS Database.
// 
// 1     6/09/04 8:45a Derickso
// Dcx specific protocol filter for Keyword protocol 2000 on CAN bus.
// 
// 1     1/27/04 9:31a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 8     1/20/04 6:43a Derickso
// Updated Semaphore to BepSemaphore.
// 
// 7     12/16/03 12:42p Derickso
// Added handling for module busy negative response code.
// 
// 5     11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
// 
// 4     11/13/03 7:02p Derickso
// Updates to correct for response pending handling errors.
// 
// 3     10/20/03 9:40a Derickso
// Added accessor and settor methods for data byte index.
// Added loggin level masks to Log statements.
// 
// 2     9/22/03 5:30p Derickso
// Added SensMessage methods to correct comile errors.
// Updated to add module request ID to each message prior
// to sending the message.
// 
// 1     8/25/03 1:52p Derickso
// Created.  Compiles, not tested.
// 
//*************************************************************************
#include "DcxCANKeyword2000ProtocolFilter.h"

DcxCANKeyword2000ProtocolFilter::DcxCANKeyword2000ProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar, BepMutex *commPortInUse /* =NULL*/) : ProtocolFilter(lastTxTime, stopCommsBepCondVar, commPortInUse),
	m_enterDiagModeCode(0x80), m_responsePendingCode(0x78), m_responsePendingReads(3), m_dataByteCountIndex(6),
	m_autoEnterDiagMode(false), m_enterDiagnosticModeMessageTag("EnterDiagnosticMode")
{   // NOthing special to do here
}

DcxCANKeyword2000ProtocolFilter::~DcxCANKeyword2000ProtocolFilter()
{	// Clear out the buffer before we leave.
	Log(LOG_FN_ENTRY, "Resetting connection in ~DcxCANKeyword2000ProtocolFilter()");
	ResetConnection();
	BposSleep(150);
	Log(LOG_FN_ENTRY, "~DcxCANKeyword2000ProtocolFilter() complete\n");
}

bool DcxCANKeyword2000ProtocolFilter::Initialize(const XmlNode *config)
{	// Get the negative response code for module not in diagnostic mode
	try
	{	// Get the code being used for module not in diagnostic mode
		SetEnterDiagnosticModeCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/EnterDiagnosticMode")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		SetEnterDiagnosticModeCode(0x80);
	}
	try
	{	// Get the code being used for response pending from the module
		SetResponsePendingCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/ResponsePending")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		SetResponsePendingCode(0x78);
	}
	try
	{	// Get the code being used for module busy, repeat request
		SetModuleBusyCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/ModuleBusy")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		SetModuleBusyCode(0x21);
	}
	// Get the message tag for entering diagnostic mode
	try
	{
		SetEnterDiagnosticModeMessageTag(config->getChild("Setup/EnterDiagnosticModeMessageTag")->getValue());
	}
	catch (XmlException &err)
	{
		SetEnterDiagnosticModeMessageTag("EnterDiagnosticMode");
	}
	// Determine if diagnostic mode should be automatically entered
	try
	{
		SetAutomaticallyEnterDiagnosticMode(config->getChild("Setup/AutomaticallyEnterDiagnosticMode")->getValue() == "Yes");
	}
	catch (XmlException &err)
	{
		SetAutomaticallyEnterDiagnosticMode(false);
	}
	// Store the max number of reads to perform for a response pending response
	try
	{
		SetResponsePendingReads(BposReadInt(config->getChild("Setup/ResponsePendingReads")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		SetResponsePendingReads(3);
	}
	// Store the module request ID
	try
	{
		SetModuleRequestID(config->getChild("Setup/ModuleRequestID")->getValue());
	}
	catch (XmlException &err)
	{
		Log(LOG_ERRORS, ": CANNOT SEND MESSAGES TO MODULE WITHOUT ID!!!!!\n");
		throw;
	}
	// Store the data byte count index
	try
	{
		SetDataByteCountIndex(BposReadInt(config->getChild("Setup/DataByteCountIndex")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		Log(LOG_ERRORS, "Data byte count index not specified, using 6\n");
		SetDataByteCountIndex(6);
	}
	// Call the base class to complete the initialization
	return ProtocolFilter::Initialize(config);
}

const BEP_STATUS_TYPE DcxCANKeyword2000ProtocolFilter::SendMessage(SerialString_t &message, bool overrideLengthCheck /*= false*/)
{	// Add the module ID to the message
	message = GetModuleRequestID() + message;
	// Clear the Fifos so bad data is not gathered
	Log(LOG_DETAILED_DATA, "Resetting The Connection\n");
	ResetConnection();
	Log(LOG_DETAILED_DATA, "Reset The Connection\n");
	// Send the message and return the result
	PrintSerialString( "ProtocolFilter Sending", message);
	INT32 bytesSent = ILogicalPort::WritePort(message, GetNumberOfRetries());

    SetLastTxTime();

	Log(LOG_DETAILED_DATA, "Sent the message\n");
	// Determine the result of the write
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	if (bytesSent < 0)		 status = BEP_STATUS_ERROR;
	else if (bytesSent == 0) status = BEP_STATUS_FAILURE;
	else if (bytesSent > 0)	 status = BEP_STATUS_SUCCESS;
	// Return the status
	return status;
}

const BEP_STATUS_TYPE DcxCANKeyword2000ProtocolFilter::SendMessage(std::string messageTag, bool overrideLengthCheck /*= false*/)
{   // Call the base class
	return ProtocolFilter::SendMessage(messageTag);
}

const BEP_STATUS_TYPE DcxCANKeyword2000ProtocolFilter::SendMessage(std::string messageTag, SerialArgs_t &args)
{   // Call the base class
	return ProtocolFilter::SendMessage(messageTag, args);
}

const BEP_STATUS_TYPE DcxCANKeyword2000ProtocolFilter::GetModuleData(string messageTag, SerialString_t &reply, SerialArgs_t *args /*= NULL*/)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string asciiMessage;
	SerialString_t xmtMessage;
	SerialString_t moduleResponse;
    INT32 tries = 0;
    //use message specific retries if specified
    GetResponseFailureRetryCount(messageTag, tries);
	tries = (tries <= 0) ? GetNumberOfRetries() : tries;
	if (GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
	{	// Convert the message to binary and attempt to send message multiple times
		GetBinaryMssg(asciiMessage, xmtMessage);
		do
		{	// Attempt to lock the port for our own use
			if ((errno = m_commsInUse->Acquire()) == EOK)
			{	// Send the message to the module
				if (args == NULL) status = SendMessage(messageTag);
				else			  status = SendMessage(messageTag, *args);
				Log(LOG_DEV_DATA, "Sent message: %s to module - status: %s\n", messageTag.c_str(), 
					ConvertStatusToResponse(status).c_str());
				// Set the message ID
				SetSentMessageIdentifier(xmtMessage[GetSentMessageIdentifierIndex()]);
				// Store the number of response pending redas to perform
                // Use message specific response pending reads if specified
				/*INT32 responsePendingReads = -1;
                GetResponsePendingReads(messageTag, responsePendingReads);
                responsePendingReads = responsePendingReads < 0 ? 
                    GetNumberOfResponsePendingReads() : responsePendingReads;*/
				// Store the number of response pending reads to perform
                // Use message specific response pending reads if specified
				INT32 responsePendingReads = -1;
                GetResponsePendingReads(messageTag, responsePendingReads);
                responsePendingReads = (responsePendingReads <= 0) ? GetNumberOfResponsePendingReads() : responsePendingReads;
				bool responsePending = false;	// Default to no response pending
				bool getResponse = true;        // Flag to signal if we need to get a response or process multiple messages
				do 
				{
					if (getResponse)
					{	// Clear the response area
						moduleResponse.erase();
						// If send was successful, get the module response
						if ((BEP_STATUS_SUCCESS == status) || (responsePending && (BEP_STATUS_NA == status)))
						{   // Get the response from the module
							status = GetResponse(moduleResponse);
						}
					}
					else
					{   // Do not get a response, just process multiple messages in the buffer we have
						status = BEP_STATUS_SUCCESS;
					}
					// Extract the module response from the message
					if ((status == BEP_STATUS_SUCCESS) && (moduleResponse.length() > 0))
					{
						reply = ExtractModuleData(moduleResponse);
					}
					// Make sure the response matches a filter before processing it
					if (IsResponseValid(messageTag, reply))
					{	// Check for a valid response if we got the response
						if ((BEP_STATUS_SUCCESS == status) && (reply.length() > 0)) status = CheckForValidResponse(reply);
						// Check for a negative response from the module
						if ((BEP_STATUS_SUCCESS != status) && (reply.length() > 0)) status = CheckForNegativeResponse(reply);
						// Check if it was a negaitve response
						if ((BEP_STATUS_FAILURE == status) || (responsePending && (BEP_STATUS_SUCCESS != status) && (reply.length() > 0)))
						{	// Check what the negative response was
							UINT8 negativeResponseCode = reply[GetNegativeResponseCodeIndex()];
							if ((GetEnterDiagnosticModeCode() == negativeResponseCode) && AutomaticallyEnterDiagnosticMode())
							{	// Need to enter diagnostic mode first
								SendMessage(GetEnterDiagnosticModeMessageTag());
								reply.erase();
#if 0
								GetResponse(GetEnterDiagnosticModeMessageTag(), reply);
#else
								if(BEP_STATUS_SUCCESS == GetResponse(reply))
								{
									reply = ExtractModuleData(reply);
									Log(LOG_DEV_DATA, "Valid response for %s from module: %s", 
										GetEnterDiagnosticModeMessageTag().c_str(), IsResponseValid(GetEnterDiagnosticModeMessageTag(), reply) ? "True" : "False");
								}
#endif 
							}
							else if (GetResponsePendingCode() == negativeResponseCode)
							{	// Check if more than one message was returned in GetResponse
								if (moduleResponse.length() > 0)
								{   // More than one response in the buffer, extract and evaluate
									getResponse = false;
								}
								else
								{   // Only decrement response pending reads if we need to look for data in the buffer
									responsePendingReads--;
									getResponse = true;
								}
								// Just need to look for the response again
								status = BEP_STATUS_NA;
								responsePending = true;
                                Log(LOG_DEV_DATA, "Handling response pending code - status: %s, responsePending: %s, getResponse: %s, responsePendingReads: %d",
                                    ConvertStatusToResponse(status).c_str(), responsePending ? "True" : "False", 
                                    getResponse ? "True" : "False", responsePendingReads);
							}
							else if (GetModuleBusyCode() != negativeResponseCode)
							{   // Negative response with no special treatment, stop all attempts
								status = BEP_STATUS_FAILURE;
								tries = 0;
							}
						}
					}
					else
					{   // Not a response we were looking for
						Log(LOG_DEV_DATA, "Response for %s does not match any filter - discarding\n", messageTag.c_str());
						// Check if more than one response in the buffer
						if (moduleResponse.length() > 0)
						{   // More than one response in the buffer, extract and evaluate
							getResponse = false;
						}
						else
						{   // Only decrement response pending reads if we need to look for data in the buffer
							responsePendingReads--;
							getResponse = true;
						}
						status = BEP_STATUS_NA;
						responsePending = true;
					}
					// If this is not a valid message, wait a bit beofre trying again
					if ((BEP_STATUS_SUCCESS != status) && getResponse) BposSleep(GetResponseDelay());
					// Keep checking until we get a good message or response pending reads run out
                    Log(LOG_DEV_DATA, "Determining if we need to loop for more data in buffer - status: %s, responsePendingReads: %d, responsePending: %s, GetStopCommFlag: %s",
                        ConvertStatusToResponse(status).c_str(), responsePendingReads, responsePending ? "True" : "False", 
                        GetStopCommsFlag() ? "True" : "False");
				} while ((BEP_STATUS_NA == status) && ((responsePendingReads > 0) && responsePending) && !GetStopCommsFlag());
				// Exit the critical section since we are done with the port for now
				m_commsInUse->Release();
			}
			else
			{   // Error aquiring the semaphore
				Log(LOG_ERRORS, "Could not Acquire mutex! Message %s not sent! - s\n", messageTag.c_str(), strerror(errno));
			}
			// Keep trying until a good response is received we run out of attempts or are signalled to stop sending messages
		} while ((BEP_STATUS_SUCCESS != status) && (tries-- > 0) && !GetStopCommsFlag());
		// Check if valid message was retrieved
        if (GetStopCommsFlag())
        {
			Log(LOG_ERRORS, "Module Signaled to stop communications:StopCommsFlag - %s, Message - %s, status: %s\n", 
				GetStopCommsFlag() ? "true" : "false", messageTag.c_str(), ConvertStatusToResponse(status).c_str());
        }
        else if (status != BEP_STATUS_SUCCESS)
		{
			Log(LOG_ERRORS, "Error getting data from module: Message - %s, status: %s\n", 
				messageTag.c_str(), ConvertStatusToResponse(status).c_str());
		}
		else if (reply.length() <= 0)
		{
			Log(LOG_ERRORS, "Did not receive a valid response from the module\n");
			status = BEP_STATUS_FAILURE;
		}
	}
	else
	{	// No message provided for this tag
		Log(LOG_ERRORS, "No message provided for tag: %s\n", messageTag.c_str());
		status = BEP_STATUS_SOFTWARE;
	}
	Log(LOG_DEV_DATA, "GetModuleData() returning - status: %s\n", ConvertStatusToResponse(status).c_str());
	return status;
}

const SerialString_t DcxCANKeyword2000ProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{
	string fullResponse;
	// Log the respnse received from the module
	char temp[256];
	for (UINT16 ii = 0; ii < moduleResponse.length(); ii++)
		fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
	Log(LOG_DETAILED_DATA, "Extracting data from response string: %s\n", fullResponse.c_str());
	// Look at the number of data bytes returned from the module
	UINT16 numberOfBytes = moduleResponse[GetDataByteCountIndex()];
    try
    {
        if (GetDataByteCountIndex() > 0) 
        {
            for (UINT8 ii = 0; ii < moduleResponse[GetDataByteCountIndex() - 1]; ii++) numberOfBytes += 256;
        }
    }
    catch (...)
    {
        Log(LOG_ERRORS, "Could not access moduleResponse[%d] to check for response size greater than 255.", GetDataByteCountIndex() - 1);
    }
	// Module response starts immediately after the module response ID
	SerialString_t dataResponse;
	const SerialString_t::size_type dataStartIdx = GetDataStartIndex();
	const SerialString_t::size_type dataRespSz = numberOfBytes-GetDataHeaderLength();
	
	dataResponse.clear();

	if( moduleResponse.size() >= (dataStartIdx + dataRespSz) )
	{
		dataResponse = moduleResponse.substr(dataStartIdx, dataRespSz);

		// Log the data extracted from the module
		fullResponse.erase();
		for (UINT16 ii = 0; ii < dataResponse.length(); ii++)
			fullResponse += CreateMessage(temp, 256, "$%02X ", dataResponse[ii]);
		Log(LOG_DEV_DATA, "Extracted module data: %s\n", fullResponse.c_str());
		// Check if multiple messages are in the module
		Log(LOG_DETAILED_DATA, "Total Response Length: %d, Current Message Length: %d, Number of Bytes: %d\n", 
			moduleResponse.length(), (GetDataByteCountIndex() + numberOfBytes + 1), numberOfBytes);
		if ((UINT16)moduleResponse.length() > (GetDataByteCountIndex() + numberOfBytes + 1))
		{   // Multiple messages in the response, update to point at next message
			moduleResponse = moduleResponse.substr(GetDataByteCountIndex() + numberOfBytes + 1);
			// Log the data remaining in the buffer
			fullResponse.erase();
			for (UINT16 ii = 0; ii < moduleResponse.length(); ii++)
				fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
			Log(LOG_DETAILED_DATA, "Module data remaining in buffer: %s\n", fullResponse.c_str());
		}
		else
		{   // No more messages to be processed, clear the buffer
			moduleResponse.erase();
		}
	}
	else
	{
		for (UINT16 ii = 0; ii < moduleResponse.length(); ii++)
			fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
		Log( LOG_ERRORS, "Incomplete message in ExtractModuleData: <%s>\n", fullResponse.c_str());
		fullResponse.clear();
	}
	// Return the extracted data	
	return dataResponse;        
}

int DcxCANKeyword2000ProtocolFilter::WaitForFullResponse( SerialString_t &response)
{
	int         retVal = 0;
	// set up the receive timeout
	//ComMssgTableEntry msgAttr;
	Log(LOG_DEV_DATA, "Waiting for response from module\n");
	Log(LOG_DEV_DATA, "\t -- Module Response Delay: %d\n", GetResponseDelay());
	Log(LOG_DEV_DATA, "\t -- Response End Delay: %d\n", GetResponseEndDelay());
#if 0
	int         bytesRead, retVal = 0;
	uint8_t     rcvByte;
	UINT16      responseDelay = GetResponseDelay();

	while (true)
	{	// Try to read next character
		if ( (bytesRead=ReadPort(&rcvByte, 1, responseDelay)) > 0)
		{
			response.push_back( rcvByte);
			retVal++;
			// Bytes are flowing in, can decrease delay time
			responseDelay = GetResponseEndDelay();

			Log(LOG_DEV_DATA, "\t Got %d (%d total) bytes...setting delay to %d ms\n", bytesRead, retVal, responseDelay);
		}
		// No more data
		else if ( bytesRead == 0)
		{	// No more data available..so leave
			Log(LOG_DEV_DATA, "All data received\n");
			break;
		}
		// Error reading byte
		else if ( errno != ETIMEDOUT)
		{
			Log(LOG_DEV_DATA, "ReadPort Error: %s, %d bytes read\n", strerror( errno), bytesRead);
			retVal = -1;
			break;
		}
		else
		{	// No more data available..so leave
			Log(LOG_DEV_DATA, "All data received (time out)\n");
			break;
		}
	}
#else
	retVal = ReadPort( response, GetResponseDelay(), GetResponseEndDelay());
#endif

	Log(LOG_DEV_DATA, "Done waiting for response.  %d bytes received\n", retVal);

	return( retVal);
}

inline const UINT8& DcxCANKeyword2000ProtocolFilter::GetEnterDiagnosticModeCode(void)
{	// Return the negative response code for module not in diag mode
	return m_enterDiagModeCode;
}

inline const UINT8& DcxCANKeyword2000ProtocolFilter::GetResponsePendingCode(void)
{	// Return the code for response pending
	return m_responsePendingCode;
}

inline const UINT8& DcxCANKeyword2000ProtocolFilter::GetModuleBusyCode(void)
{	// Return the code for module busy, repeat request
	return m_moduleBusyCode;
}

inline const string& DcxCANKeyword2000ProtocolFilter::GetEnterDiagnosticModeMessageTag(void)
{	// Return the tag for the enter diagnostic mode message
	return m_enterDiagnosticModeMessageTag;
}

inline const INT32& DcxCANKeyword2000ProtocolFilter::GetNumberOfResponsePendingReads(void)
{
	return m_responsePendingReads;
}

inline const bool& DcxCANKeyword2000ProtocolFilter::AutomaticallyEnterDiagnosticMode(void)
{
	return m_autoEnterDiagMode;
}

inline const SerialString_t& DcxCANKeyword2000ProtocolFilter::GetModuleRequestID(void)
{
	return m_moduleRequestID;
}

inline const INT32& DcxCANKeyword2000ProtocolFilter::GetDataByteCountIndex(void)
{
	return m_dataByteCountIndex;
}

inline void DcxCANKeyword2000ProtocolFilter::SetEnterDiagnosticModeCode(const UINT8 &enterDiagnosticModeCode)
{	// Save the enter diagnostic mode code
	m_enterDiagModeCode = enterDiagnosticModeCode;
}

inline void DcxCANKeyword2000ProtocolFilter::SetResponsePendingCode(const UINT8 &responsePendingCode)
{	// Save the response pending code
	m_responsePendingCode = responsePendingCode;
}

inline void DcxCANKeyword2000ProtocolFilter::SetModuleBusyCode(const UINT8 &moduleBusyCode)
{	// Save the module busy, repeat request code
	m_moduleBusyCode = moduleBusyCode;
}

inline void DcxCANKeyword2000ProtocolFilter::SetEnterDiagnosticModeMessageTag(const string& tag)
{	// Save the enter diagnostic mode message tag
	m_enterDiagnosticModeMessageTag = tag;
}

inline void DcxCANKeyword2000ProtocolFilter::SetAutomaticallyEnterDiagnosticMode(const bool& autoEnterDiagMode)
{
	m_autoEnterDiagMode = autoEnterDiagMode;
}

inline void DcxCANKeyword2000ProtocolFilter::SetResponsePendingReads(const INT32& responsePendingReads)
{
	m_responsePendingReads = responsePendingReads;
}

inline void DcxCANKeyword2000ProtocolFilter::SetModuleRequestID(string moduleID)
{
    UINT16 id = atoh(moduleID.c_str());
    m_moduleRequestID.clear();
	m_moduleRequestID.push_back((uint8_t)((id & 0xFF00) >> 8));
	m_moduleRequestID.push_back((uint8_t)(id & 0x00FF));
}

inline void DcxCANKeyword2000ProtocolFilter::SetDataByteCountIndex(const INT32& dataByteCountindex)
{
	m_dataByteCountIndex = dataByteCountindex;
}
