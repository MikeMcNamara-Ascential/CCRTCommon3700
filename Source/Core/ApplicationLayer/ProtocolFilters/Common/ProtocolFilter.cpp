//******************************************************************************
// Description:
//  Base ProtocolFilter for communicating with vehicle modules.
//
//==============================================================================
// Copyright (c) 1999-2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/Common/ProtocolFilter.cpp $
// 
// 12    2/22/07 8:46a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Now clear m_negativeResponseCodes in the destructor and CloseCom().
// 
// 11    1/17/07 3:04p Rwiersem
// Changes for the 07011901 core release:
// 
// - If the port is already open, don't call OpenPort() again in
// OpenCom().
// 
// 10    10/25/06 3:13p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a timer to tell when the last transmit message was sent.
// This
//       allows the keep alive message to be sent when needed instead of
// on a
//       specific time interval.
//     - Added OpenCom() and CloseCom()
// 
// 9     4/13/06 3:16a Cward
// Beautified code
//
// 2     2/24/06 1:14p Bmeinke
// Beautified
//
// 8     11/11/05 4:54p Cward
// Changes for 0511101 iteration
//
// 7     5/11/05 10:03p Cward
// Updated for 05051101 iteration
//
// 6     5/09/05 10:50p Gpattison
// Added support for validation of checksum and also addition
// array bounds checking to prevent faults on incomplete messages
//
// 6     6/09/04 10:50p Cward
// Refactored app layer includes to remove implementation. Also updated
// header comment block to new standard
//
// 5     4/28/04 10:54p Cward
// Changes associated with build 040402801
//
// 4     3/04/04 4:19p Cward
// Removed pound if 0 code
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 2     8/08/03 5:10p Khykin
// Updated with changes for Flatrock.
//    Revision 1.2  2003/05/07 19:29:08  swieton
//    Fixed a handful of syntax errors.
//
//    Revision 1.1  2003/05/07 19:16:48  swieton
//    Moved ProtocolFilters/KeyWord2000/ProtocolFilter.cpp to ProtocolFilters/ProtocolFilter.cpp
//
//    Revision 1.1.1.1  2003/04/30 20:10:25  tenharmsel
//    Source code for CCRT Server
//
//
// 1     4/22/03 10:26a Khykin
// Initial Working Checkin.
//
// 1     4/11/03 7:33a Khykin
// Initial checkin.
//    Revision 1.1  2003/02/23 16:51:52  tbochene
//    New protocol filter stuff
//
//
// 1     2/10/03 2:38p Derickso
// Split functionality into seperate file.
//
// 2     1/30/03 8:40a Derickso
// Added constructor and destructor.
// Numerous updates from debugging and testing.
//
// 1     12/18/02 10:09a Derickso
// Created.  Not tested or compiled.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "ProtocolFilter.h"

ProtocolFilter::ProtocolFilter(KeepAliveTimer_t &lastTxTime,
							   StopCommsBepCondVar *stopCommsBepCondVar,
							   BepMutex *commsInUse	/*=NULL*/)  : 
IProtocolFilter(), SerialChannel(), m_numberOfRetries( 1),
m_checksumValidationRequired(false), m_lastTxTime(lastTxTime)
{	// Save the pointer to the semaphore for the comm port
	m_commsInUse = commsInUse;
	m_stopCommsBepCondVar = stopCommsBepCondVar;
}

ProtocolFilter::~ProtocolFilter()
{
	m_negativeResponseCodes.clear(true);
}

bool ProtocolFilter::Initialize(const XmlNode *config)
{	// Get the data start index
	Log(LOG_DEV_DATA, "Getting DataStartIndex...\n");
	try
	{
		SetDataStartIndex(config->getChild("Setup/DataStartIndex")->getValue());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "DataStartIndex not provided, using 0 -- err: %s\n", err.what());
		SetDataStartIndex(0);
	}
	// Get the header length
	Log(LOG_DEV_DATA, "Getting DataHeaderLength...\n");
	try
	{
		SetDataHeaderLength(config->getChild("Setup/DataHeaderLength")->getValue());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "DataHeaderLength not provided, using 0 -- err: %s\n", err.what());
		SetDataHeaderLength(0);
	}
	// Get the check sum length
	Log(LOG_DEV_DATA, "Getting CheckSumLength...\n");
	try
	{
		SetChecksumLength(config->getChild("Setup/CheckSumLength")->getValue());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "CheckSumLength was not provided, using 1 -- err: %s\n", err.what());
		SetChecksumLength(1);
	}
	// Determine if the protocol filter should validate the checksum for each received message
	Log(LOG_DEV_DATA, "Getting ValidateChecksum...\n");
	try
	{
		m_checksumValidationRequired = atob(config->getChild("Setup/ValidateChecksum")->getValue().c_str());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "ValidateChecksum was not provided, setting to false -- err: %s\n", err.what());
		m_checksumValidationRequired = false;
	}
	// Get the number of retries
	Log(LOG_DEV_DATA, "Getting NumberOfRetries...\n");
	try
	{
		SetNumberOfRetries(config->getChild("Setup/NumberOfRetries")->getValue());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "NumberOfRetries was not provided, using 0 -- err: %s\n",err.what());
		SetNumberOfRetries(0);
	}
	// Get the valid response offset
	Log(LOG_DEV_DATA, "Getting ValidResponseOffset...\n");
	try
	{
		SetValidResponseOffset(config->getChild("Setup/ValidResponseOffset")->getValue());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "ValidResponseOffset was not provided, using 0 -- err: %s\n",err.what());
		SetValidResponseOffset(0);
	}
	// Get the index for the sent message identifier
	try
	{
		SetSentMessageIdentifierIndex(config->getChild("Setup/SentMessageIdentifierIndex")->getValue());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "SentMessageIdentifierIndex was not provided, using 3 -- err: %s\n", err.what());
		SetSentMessageIdentifierIndex(3);
	}
	// Get the negative response value
	Log(LOG_DEV_DATA, "Getting NegativeResponseValue\n");
	try
	{
		SetNegativeResponseValue(config->getChild("Setup/NegativeResponseValue")->getValue());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "NegativeResponseValue was not provided, using 0x7F -- err: %s\n",err.what());
		SetNegativeResponseValue(0x7F);
	}
	// Get the index for the negative response code
	try
	{
		SetNegativeResponseCodeIndex(config->getChild("Setup/NegativeResponseCodeIndex")->getValue());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "NegativeResponseCodeIndex was not provided, using 5 -- err: %s\n", err.what());
		SetNegativeResponseCodeIndex(5);
	}
	// Get the response delay
	Log(LOG_DEV_DATA, "Getting ModuleResponseDelay...\n");
	try
	{
		SetResponseDelay(config->getChild("Setup/ModuleResponseDelay")->getValue());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "ModuleReponseDelay was not provided, using 100ms -- err: %s\n", err.what());
		SetResponseDelay(100);
	}
	// Get the response end delay
	Log(LOG_DEV_DATA, "Getting ModuleResponseEndDelay...\n");
	try
	{
		SetResponseEndDelay(BposReadInt(config->getChild("Setup/ModuleResponseEndDelay")->getValue().c_str()));
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "ModuleReponseEndDelay was not provided, using 50ms -- err: %s\n", err.what());
		SetResponseEndDelay(50);
	}
	// Save the Negative Response codes
	try
	{
		StoreNegativeResponseCodes(config->getChild("Setup/NegativeResponseCodes"));
	}
	catch(BepException)
	{
		Log(LOG_ERRORS, "NegativeResponseCodes node not found.  Reason for negative response will not be listed\n");
	}
	// Set the checkksum calculation method
	string method;
	try
	{
		method = config->getChild("Setup/ChecksumCalculationMethod")->getValue();
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Checksum calculation method not specified, defaulting to basic sum of all bytes: %s", excpt.GetReason());
		method = ChecksumCalculationAdd;
	}
	Log(LOG_DEV_DATA, "Setting checksum calculation method to %s", method.c_str());
	ChecksumCalculationMethod(&method);
	// Complete the initialization
	Log(LOG_DEV_DATA, "Calling SerialChannel::Initialize() to complete the initialization\n");
	return(SerialChannel::Initialize(config));
}

bool ProtocolFilter::OpenCom()
{
	bool portOpen = true;

	if( -1 == GetPortFd())
	{
		// open the port
		portOpen = OpenPort(GetName().c_str());
	}

	return( portOpen);
}

void ProtocolFilter::CloseCom()
{
	SerialChannel::ClosePort();
	m_negativeResponseCodes.clear(true);
}

const BEP_STATUS_TYPE ProtocolFilter::SendMessage(SerialString_t &message)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Check to ensure the binary message to be sent has enough bytes to
	//   ensure we don't sig seg when we access the array to
	//   set the SentMessageIdentifier
	if(GetDataStartIndex() < message.length())
	{
		// Set the message identifier
		SetSentMessageIdentifier(message[GetDataStartIndex()]);
		// Add the checksum to the message
		AddChecksumToMessage(message);
		// Clear the Fifos so bad data is not gathered
		Log(LOG_DETAILED_DATA, "Resetting The Connection\n");
		ResetConnection();
		Log(LOG_DETAILED_DATA, "Reset The Connection\n");
		// Send the message and return the result
		PrintSerialString( "ProtocolFilter Sending", message);
		// Write message
		INT32 bytesSent = ILogicalPort::WritePort(message, GetNumberOfRetries());

		SetLastTxTime();

		Log(LOG_DETAILED_DATA, "Sent the message\n");
		// Determine the result of the write
		if(bytesSent < 0)		status = BEP_STATUS_ERROR;
		else if(bytesSent == 0)	status = BEP_STATUS_FAILURE;
		else if(bytesSent > 0)	status = BEP_STATUS_SUCCESS;
	}
	else
	{
		Log(LOG_ERRORS, "ERROR Module Message length shorter than Data Start Index "
			"Message Length: %d\n\tData Start Index: %d\n", message.length(),
			GetDataStartIndex());
		status = BEP_STATUS_ERROR;
	}
	// Return the status
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::SendMessage(std::string messageTag)
{	// Send the message and return the result
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	std::string asciiMessage;
	SerialString_t xmtMessage;
	// Get the message to be sent
	if(GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
	{	// Convert the message to binary
		GetBinaryMssg(asciiMessage, xmtMessage);
		// Send the message
		status = SendMessage(xmtMessage);
	}
	else
	{	// No message provided for this tag
		Log(LOG_ERRORS,"No message provided for tag: %s\n", messageTag.c_str());
		status = BEP_STATUS_SOFTWARE;
	}
	// Return the status
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::SendMessage(std::string messageTag, SerialArgs_t &args)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	SerialString_t xmtMessage;
	// Get the message to be sent
	if(GetXmtMssgString(messageTag, xmtMessage, args) == true)
	{	// Send the message
		status = SendMessage(xmtMessage);
	}
	else
	{	// No message provided for this tag
		Log(LOG_ERRORS,"No message provided for tag: %s\n", messageTag.c_str());
		status = BEP_STATUS_SOFTWARE;
	}
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::SendBusBroadcastMessage(string messageTag)
{
	Log(LOG_ERRORS, "WARNING - ProtocolFilter::SendBusBroadcastMessage() not implemented!");
	return(BEP_STATUS_SOFTWARE);
}

const BEP_STATUS_TYPE ProtocolFilter::GetResponse(const std::string &messageTag, SerialString_t &reply)
{
	BEP_STATUS_TYPE status = BEP_STATUS_NA;
	// Get the response from the module
	INT32 retry = 0;		// retry counter
	INT32 bytesRead = 0;	// the number of bytes read

	do
	{	// read the data from the port
		bytesRead = WaitForFullResponse(reply);
		Log(LOG_DETAILED_DATA, "Received %d bytes for message %s\n", bytesRead, messageTag.c_str());
		// if data is read from the port, determine if it is done
		if(bytesRead > 0)
		{	// Log the response if enabled and flag set
			if(GetLogStatus() && (GetVerboseMask() & LOG_DEV_DATA))
			{
				std::string response;		// response
				char buffer[128];			// buffer for printing
				for(INT16 ii = 0; ii < bytesRead; ii++)
					response += CreateMessage(buffer, sizeof(buffer),"$%02X ", reply[ii]);
				Log(LOG_DETAILED_DATA, "Response bytes received: %d for message: %s\tResponse: %s\n\n",
					bytesRead, messageTag.c_str(), response.c_str());
			}
			// Validate the response against all filters
			if(IsResponseValid(messageTag, reply))
			{
				Log(LOG_DETAILED_DATA, "Full response received\n");
				status = BEP_STATUS_SUCCESS;
			}
			else
				Log(LOG_ERRORS, "Incomplete response\n");

			retry = 0;	// rest the retry counter, due data received
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
				Log(LOG_DETAILED_DATA, "Waiting for full message\n");
			}
			retry++;	// increment the retry counter
		}
		// while the result not detemined and retries remain
	}while((status == BEP_STATUS_NA) && (retry < 3) && !GetStopCommsFlag());

	// if a successful read never occurred, fail
	if(retry >= 3) status = BEP_STATUS_FAILURE;

	Log(LOG_FN_ENTRY, "Exit GetResponse: %d\n", status);

	// Return the status
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::GetResponse(SerialString_t &reply)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Get the response from the port
	INT32 bytesRead = WaitForFullResponse(reply);
	// Evaluate the number of bytes
	Log(LOG_DETAILED_DATA, "GetResponse retrieved %d bytes from the module\n", bytesRead);
	if(bytesRead > 0)		 status = BEP_STATUS_SUCCESS;
	else if(bytesRead == -1) status = BEP_STATUS_ERROR;
	else					  status = BEP_STATUS_FAILURE;
	// Return the status
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::GetModuleData(std::string messageTag, SerialString_t &reply, SerialArgs_t *args)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	std::string asciiMessage;
	SerialString_t xmtMessage;
	UINT16 tries = GetNumberOfRetries() + 1;
	bool portLocked = false;
	if(GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
	{
		// Lock the port so only this thread uses it
		Log(LOG_DETAILED_DATA, "GetModuleData Locking Port\n");
		if((portLocked = LockPort()) == true)
		{	// The port was locked
			Log(LOG_DETAILED_DATA, "GetModuleData Locked Port\n");
			do
			{	// Send the message to the module
				if(args == NULL) status = SendMessage(messageTag);
				else			  status = SendMessage(messageTag, *args);
				Log(LOG_DEV_DATA, "Sent message: %s to module - status: %s\n", messageTag.c_str(),
					ConvertStatusToResponse(status).c_str());
				// If response expected, call GetResponse(
				if(IsResponseExpected(messageTag))
				{	// Clear the response area
					reply.erase();
					// Set the message ID
					// If send was successful, get the module response
					if(BEP_STATUS_SUCCESS == status) status = GetResponse(messageTag, reply);
					// If this is not a valid message, wait a bit beofre trying again
					if(BEP_STATUS_SUCCESS != status) BposSleep(GetResponseDelay());
				}
				else
				{
					// If response NOT expected, exit do-while loop
					status = BEP_STATUS_SUCCESS;
				}
			}
			while((BEP_STATUS_SUCCESS != status) && (tries-- > 0) && !GetStopCommsFlag());
			if(GetStopCommsFlag())
			{
				Log(LOG_ERRORS, "Module Signaled to stop communications:StopCommsFlag - %s, Message - %s, status: %s\n", 
					GetStopCommsFlag() ? "true" : "false", messageTag.c_str(), ConvertStatusToResponse(status).c_str());
			}
		}
		else Log(LOG_ERRORS, "Could not lock the port for message: %s because: %s\n", messageTag.c_str(), strerror(errno));
		// Finished with the port for this attempt, unlock for others
		if(portLocked)
		{	// Unlock the port
			Log(LOG_DETAILED_DATA, "GetModuleData UnLocking Port\n");
			bool portUnlocked = false;
			UINT8 attempts = 5;
			while(((portUnlocked = UnlockPort()) == false) && attempts--)
			{
				Log(LOG_ERRORS, "Error unlocking port - attempt: %d , Reason: %s\n", attempts, strerror(errno));
				BposSleep(10);
			}
			if(!portUnlocked)  Log(LOG_ERRORS, "WARNING: Port was NOT UNLOCKED!!!!!!!!!!!!!!!\n");
			else				Log(LOG_DETAILED_DATA, "GetModuleData UnLocked Port\n");
		}
		// If a response is expected, verify that response is valid
		if(IsResponseExpected(messageTag))
		{
			// Check if valid message was retrieved
			if((status == BEP_STATUS_SUCCESS) && (reply.length() > 0))
			{	// Verify a valid response was received
				status = CheckForValidResponse(reply);
				// Check for a negative response from the module
				if(status != BEP_STATUS_SUCCESS) status = CheckForNegativeResponse(reply);
				// Extract the data from the response
				if(BEP_STATUS_SUCCESS == status) reply = ExtractModuleData(reply);
			}
			else if(status != BEP_STATUS_SUCCESS)
			{
				Log(LOG_ERRORS, "Error getting data from module: Message - %s, status: %s\n",
					messageTag.c_str(), ConvertStatusToResponse(status).c_str());
			}
			else if(reply.length() <= 0)
			{
				Log(LOG_ERRORS, "Did not receive a valid response from the module\n");
				status = BEP_STATUS_FAILURE;
			}
		}
		else
		{
			// If response NOT expected, return Success
			status = BEP_STATUS_SUCCESS;
		}
	}
	else
	{	// No message provided for this tag
		Log(LOG_ERRORS, "No message provided for tag: %s\n", messageTag.c_str());
		status = BEP_STATUS_SOFTWARE;
	}
	Log(LOG_DETAILED_DATA, "GetModuleData() returning - status: %s\n", ConvertStatusToResponse(status).c_str());
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::GetBusBroadcastMessage(string messageTag,
															 const long messageWaitTime,
															 SerialString_t &busMssg)
{
	Log(LOG_ERRORS, "WARNING - ProtocolFilter::GetBusBroadcastMessage() not implemented!");
	return(BEP_STATUS_SOFTWARE);
}

void ProtocolFilter::AddChecksumToMessage(SerialString_t &message)
{	// Compute the checksum for the message
	UINT8 checkSum = ComputeCheckSum(message);
	PrintSerialString( "ProtocolFilter Adding checksum to", message);
	// Add the checksum to the message
	message.push_back(checkSum);
	PrintSerialString( "ProtocolFilter message with checksum", message);
}

UINT8 ProtocolFilter::ComputeCheckSum(const SerialString_t &message)
{
	UINT8 checksum = 0;
	if(!ChecksumCalculationMethod().compare(ChecksumCalculationXor))
	{	// XOR all message bytes
		for(UINT16 byte = 0; byte < message.length(); byte++)
			checksum = (checksum ^ message[byte]) & 0xFF;
	}
	else
	{	// Add up all the bytes in the message
		for(UINT16 byte = 0; byte < message.length(); byte++)
			checksum = (checksum + message[byte]) & 0xFF;
	}
	// Return the checksum
	return(checksum);
}

const BEP_STATUS_TYPE ProtocolFilter::FilterSubscribe(const std::string &messageTag, struct sigevent *event)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	bool foundMessage = false;
	SerialString_t moduleResponse;
	// Keep looking for the response
	do
	{	// get the response
		status = GetResponse(moduleResponse);
		// Check if it is what we are looking for
		foundMessage = IsResponseValid(messageTag, moduleResponse);
	} while((status != BEP_STATUS_SUCCESS) && !foundMessage);
	// Return the status
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::UploadFilterByte(UINT8 &filterVal, UINT32 &filterPosition)
{
	return(BEP_STATUS_FAILURE);
}

void ProtocolFilter::Close(void)
{
	ClosePort();
}

bool ProtocolFilter::LockPort(int flags) /*= 0*/
{
	ILogicalPort::LockPort(flags);
	return(true);
}


bool ProtocolFilter::UnlockPort()
{
	ILogicalPort::UnlockPort();
	return(true);
}

bool ProtocolFilter::GetXmtMssgString( const string &mssgTag, SerialString_t &mssgString,
									   const SerialArgs_t &args)
{
	bool                retVal, lastToken=false;
	string              xmtTmpl, token;
	string::size_type   off1=0, off2=0;
	uint8_t             xmtByte;
	int                 argIdx=0;
	char                junk[ 10];

	// Get the Transmit template for the given mssgTag
	if( (retVal=GetXmtTemplate( mssgTag, xmtTmpl)) == true)
	{
		while( lastToken == false)
		{	// Find the beginning of the next token
			off1 = xmtTmpl.find_first_of( '0', off2);
			if( off1 != string::npos)
			{	// Find the end of the token
				if( (off2 = xmtTmpl.find_first_of( ',', off1)) != string::npos)
				{	// Grab this token from the template
					token = xmtTmpl.substr( off1, (off2-off1));
				}
				// Last token to be parsed
				else
				{	// Grab this token from the template
					token = xmtTmpl.substr( off1);
					lastToken = true;
				}
				// We have wildcards to replace
				if( token.find_first_of( '%') != string::npos)
				{
					sprintf( junk, token.c_str(), args[ argIdx++]);
					xmtByte = atoh( junk);
				}
				else
				{
					xmtByte = atoh( token.c_str());
				}
				// Add this converted byte to the binary string
				mssgString.push_back( xmtByte);
			}
		}
	}
	return( retVal);
}

bool ProtocolFilter::GetXmtTemplate( const string &mssgTag, string &xmtTmpl)
{
	bool                retVal = false;
	ComMessageMapItr_t  tblItr;

	// See if we have an entry for this tag
	if( (tblItr=m_mssgTags.find( mssgTag)) != m_mssgTags.end())
	{	// Get the xmt template string
		ComMssgTableEntry   &tblEntry = tblItr->second;
		xmtTmpl = tblEntry.GetXmtString();
		retVal = true;
	}
	return( retVal);
}

const BEP_STATUS_TYPE ProtocolFilter::CheckForValidResponse(const SerialString_t &moduleResponse)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	UINT16 offset = GetValidResponseOffset();
	// ensure the response is not an invalid length that could cause a sig seg by dereferencing it
	if(GetSentMessageIdentifierIndex() < moduleResponse.length())
	{
		Log(LOG_DEV_DATA, "CheckForValidResponse() -- GetSentMessageIdentifier(): $%02X + offset{%02X}: $%02X == $%02X\n",
			GetSentMessageIdentifier(), offset, (GetSentMessageIdentifier() + offset), moduleResponse[GetSentMessageIdentifierIndex()]);
		if((GetSentMessageIdentifier() + offset) == moduleResponse[GetSentMessageIdentifierIndex()])
		{
			status = BEP_STATUS_SUCCESS;
		}
		else status	= BEP_STATUS_FAILURE;
	}
	else
	{
		Log(LOG_ERRORS, "ERROR Module Response length shorter than Identifier Index "
			"Response Length: %d\n\tIdentifier Index: %d\n", moduleResponse.length(),
			GetSentMessageIdentifierIndex());
		status = BEP_STATUS_ERROR;
	}
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::CheckForValidUUDTResponse(const SerialString_t &moduleResponse)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// ensure the response is not an invalid length that could cause a sig seg by dereferencing it
	if(GetSentMessageIdentifierIndex() < moduleResponse.length())
	{
		Log(LOG_DEV_DATA, "CheckForValidResponse() -- GetSentMessageIdentifier(): $%02X == $%02X\n",
			GetSentMessageIdentifier(), moduleResponse[GetSentMessageIdentifierIndex()]);
		if((GetSentMessageIdentifier()) == moduleResponse[GetSentMessageIdentifierIndex()])
		{
			status = BEP_STATUS_SUCCESS;
		}
		else status	= BEP_STATUS_FAILURE;
	}
	else
	{
		Log(LOG_ERRORS, "ERROR Module Response length shorter than Identifier Index "
			"Response Length: %d\n\tIdentifier Index: %d\n", moduleResponse.length(),
			GetSentMessageIdentifierIndex());
		status = BEP_STATUS_ERROR;
	}
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::CheckForNegativeResponse(const SerialString_t &moduleResponse)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// ensure the response is not an invalid length that could cause a sig seg by dereferencing it
	if(GetSentMessageIdentifierIndex() < moduleResponse.length() &&
	   GetNegativeResponseCodeIndex() < moduleResponse.length())
	{
		if((GetNegativeResponseValue() == moduleResponse[GetSentMessageIdentifierIndex()]))
		{	// Set status to fail
			status = BEP_STATUS_FAILURE;
			// Print out the reason for the negative response
			char buffer[32];
			std::string tag = CreateMessage(buffer, sizeof(buffer), "Code_0x%02X", moduleResponse[GetNegativeResponseCodeIndex()]);
			// Look for a text description for the code
			XmlNodeMapItr iter;
			std::string reason("No reason provided for negative response code");
			if((iter = m_negativeResponseCodes.find(tag)) != m_negativeResponseCodes.end())
			{	// Pull the reason from the node
				reason = iter->second->getValue();
			}
			Log(LOG_ERRORS, "Negative Response received -- Code: $%02X, Reason: %s\n",
				moduleResponse[GetNegativeResponseCodeIndex()], reason.c_str());
		}
		else status	= BEP_STATUS_SUCCESS;
	}
	else
	{
		Log(LOG_ERRORS, "ERROR Module Response length shorter than Identifier Index "
			"or Neg Response Code Index\n\tResponse Length: %d\n\tIdentifier Index: %d\n\t "
			"Neg Response Code Index: %d\n", moduleResponse.length(),
			GetSentMessageIdentifierIndex(),GetNegativeResponseCodeIndex());
		status = BEP_STATUS_ERROR;
	}
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::SendKeepAlive(const std::string &keepAliveMessageTag, const bool &keepAliveResponseRequired)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	SerialString_t reply;
	if(keepAliveResponseRequired)
		status = GetModuleData(keepAliveMessageTag,reply);
	else
	{	// Lock the port before sending the keep alive message
		bool portLocked = false;
		Log(LOG_DETAILED_DATA, "SendKeepAlive Locking Port\n");
		if((portLocked = LockPort()) == true)
		{
			Log(LOG_DETAILED_DATA, "SendKeepAlive Locked Port\n");
			status = SendMessage(keepAliveMessageTag);
		}
		// If the port was locked, unlock it now
		if(portLocked)
		{
			Log(LOG_DETAILED_DATA, "SendKeepAlive UnLocking Port\n");
			if(UnlockPort() != true) Log(LOG_DETAILED_DATA, "WARNING: SendKeepAlive UnlockPort() failed\n");
			else
				Log(LOG_DETAILED_DATA, "SendKeepAlive UnLocked Port\n");
		}
	}
	// Return the status
	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::GetMessage(const std::string &messageTag, std::string &message)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	ComMessageMapItr_t  itr;
	if((itr = m_mssgTags.find(messageTag)) != m_mssgTags.end())
	{	// Get the message to be sent
		ComMssgTableEntry   &mssgEntry = itr->second;
		message = mssgEntry.GetXmtString();
		status = BEP_STATUS_SUCCESS;
	}
	else status	= BEP_STATUS_FAILURE;

	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::GetResponseFailureRetryCount(const std::string &messageTag, INT32 &messageRetries)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	ComMessageMapItr_t  itr;
	if((itr = m_mssgTags.find(messageTag)) != m_mssgTags.end())
	{	// Get the message to be sent
		ComMssgTableEntry   &mssgEntry = itr->second;
		messageRetries = mssgEntry.GetResponseFailureRetryCount();
		status = BEP_STATUS_SUCCESS;
	}
	else status	= BEP_STATUS_FAILURE;

	return(status);
}

const BEP_STATUS_TYPE ProtocolFilter::GetResponsePendingReads(const std::string &messageTag, INT32 &responsePendingReads)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	ComMessageMapItr_t  itr;
	if((itr = m_mssgTags.find(messageTag)) != m_mssgTags.end())
	{	// Get the message to be sent
		ComMssgTableEntry   &mssgEntry = itr->second;
		responsePendingReads = mssgEntry.GetResponsePendingReads();
		status = BEP_STATUS_SUCCESS;
	}
	else status	= BEP_STATUS_FAILURE;

	return(status);
}

const UINT16 &ProtocolFilter::GetDataStartIndex(void)
{
	return(m_dataStartIndex);
}

const UINT16 &ProtocolFilter::GetDataHeaderLength(void)
{
	return(m_dataHeaderLength);
}

const UINT16 &ProtocolFilter::GetChecksumLength(void)
{
	return(m_checkSumLength);
}

const UINT16 &ProtocolFilter::GetNumberOfRetries(void)
{
	return(m_numberOfRetries);
}

const UINT16 &ProtocolFilter::GetSentMessageIdentifier(void)
{
	return(m_sentMessageIdentifier);
}

const UINT16 &ProtocolFilter::GetSentMessageIdentifierIndex(void)
{
	return(m_sentMessageIdentifierIndex);
}

const UINT16 &ProtocolFilter::GetValidResponseOffset(void)
{
	return(m_validResponseOffset);
}

const UINT16 &ProtocolFilter::GetNegativeResponseValue(void)
{
	return(m_negativeResponseValue);
}

const UINT16 &ProtocolFilter::GetNegativeResponseCodeIndex(void)
{
	return(m_negativeResponseCodeIndex);
}

const UINT16 &ProtocolFilter::GetResponseDelay(void)
{
	return(m_moduleResponseDelay);
}

const UINT16 &ProtocolFilter::GetResponseEndDelay(void)
{
	return(m_moduleResponseEndDelay);
}

const std::string ProtocolFilter::GetNegativeResponseReason(const UINT8 &code)
{	// Build the tag
	char buffer[128];
	std::string codeTag = CreateMessage(buffer, sizeof(buffer), "Code_0x%02X", code);
	std::string reason;
	// Find the code in the map
	XmlNodeMapItr iter;
	if((iter = m_negativeResponseCodes.find(codeTag)) != m_negativeResponseCodes.end())
	{	// Found the code in the map
		reason = iter->second->getValue();
	}
	else
	{	// Did not find a reason for this code
		reason = CreateMessage(buffer, sizeof(buffer), "No reason found for negative response code 0x%02X\n", code);
	}
	// Return the reason
	return(reason);
}

int ProtocolFilter::WaitForFullResponse( SerialString_t &response)
{
	int         retVal = 0;
	// set up the receive timeout
	ComMssgTableEntry msgAttr;
	Log(LOG_DEV_DATA, "Waiting for response from module\n");
	Log(LOG_DEV_DATA, "\t -- Module Response Delay: %d\n", GetResponseDelay());
	Log(LOG_DEV_DATA, "\t -- Response End Delay: %d\n", GetResponseEndDelay());
	retVal = ReadPort( response, GetResponseDelay(), GetResponseEndDelay());
	Log(LOG_DEV_DATA, "Done waiting for response.  %d bytes received\n", retVal);
	return( retVal);
}

bool ProtocolFilter::IsChecksumValid(SerialString_t &moduleResponse)
{
	// no base implementation for validation of check sum
	//   therefore always return true
	return(true);
}

uint64_t ProtocolFilter::GetLastTxTime()
{
	uint64_t lastTxTime = 0LL;

	if( EOK == m_lastTxTime.Acquire())
	{
		lastTxTime = m_lastTxTime.GetValue();
		m_lastTxTime.Release();
	}
	else
	{
		perror( "Error locking m_lastTxTime in ProtocolFilter::GetLastTxTime()");
	}
	return( lastTxTime);
}

bool ProtocolFilter::GetStopCommsFlag()
{
	bool stopCommsFlag = false;
	if(m_stopCommsBepCondVar != NULL)
	{
		try
		{
			if( EOK == m_stopCommsBepCondVar->Acquire())
			{
				stopCommsFlag = m_stopCommsBepCondVar->GetValue();
				m_stopCommsBepCondVar->Release();
			}
			else
			{
				Log(LOG_ERRORS, "Error locking m_stopCommsBepCondVar in ProtocolFilter::GetStopCommsFlag()");
			}
		}
		catch(...)
		{
			Log(LOG_ERRORS, "Exception trying to Acquire m_stopCommsBepCondVar");
		}
	}
	return( stopCommsFlag);
}

void ProtocolFilter::SetDataStartIndex(const UINT16 index)
{
	m_dataStartIndex = index;
}

void ProtocolFilter::SetDataStartIndex(const std::string &index)
{
	SetDataStartIndex(atoi(index.c_str()));
}

void ProtocolFilter::SetDataHeaderLength(const UINT16 length)
{
	m_dataHeaderLength = length;
}

void ProtocolFilter::SetDataHeaderLength(const std::string &length)
{
	SetDataHeaderLength(atoi(length.c_str()));
}

void ProtocolFilter::SetChecksumLength(const UINT16 length)
{
	m_checkSumLength = length;
}

void ProtocolFilter::SetChecksumLength(const std::string &length)
{
	SetChecksumLength(atoi(length.c_str()));
}

void ProtocolFilter::SetNumberOfRetries(const UINT16 retries)
{
	m_numberOfRetries = retries;
}

void ProtocolFilter::SetNumberOfRetries(const std::string &retries)
{
	SetNumberOfRetries(atoi(retries.c_str()));
}

void ProtocolFilter::SetSentMessageIdentifier(const UINT16 identifier)
{
	m_sentMessageIdentifier = identifier;
}

void ProtocolFilter::SetSentMessageIdentifierIndex(const UINT16 index)
{
	m_sentMessageIdentifierIndex = index;
}

void ProtocolFilter::SetSentMessageIdentifierIndex(const std::string &index)
{
	SetSentMessageIdentifierIndex(atoi(index.c_str()));
}

void ProtocolFilter::SetValidResponseOffset(const UINT16 offset)
{
	m_validResponseOffset = offset;
}

void ProtocolFilter::SetValidResponseOffset(const std::string &offset)
{
	SetValidResponseOffset(atoh(offset.c_str()));
}

void ProtocolFilter::SetNegativeResponseValue(const UINT16 value)
{
	m_negativeResponseValue = value;
}

void ProtocolFilter::SetNegativeResponseValue(const std::string &value)
{
	SetNegativeResponseValue(atoh(value.c_str()));
}

void ProtocolFilter::SetNegativeResponseCodeIndex(const UINT16 index)
{
	m_negativeResponseCodeIndex = index;
}

void ProtocolFilter::SetNegativeResponseCodeIndex(const std::string &index)
{
	SetNegativeResponseCodeIndex(atoi(index.c_str()));
}

void ProtocolFilter::SetResponseDelay(const UINT16 responseDelay)
{
	m_moduleResponseDelay = responseDelay;
}

void ProtocolFilter::SetResponseDelay(const std::string &responseDelay)
{
	SetResponseDelay(atoi(responseDelay.c_str()));
}

void ProtocolFilter::SetResponseEndDelay(const UINT16 responseDelay)
{
	m_moduleResponseEndDelay = responseDelay;
}

void ProtocolFilter::StoreNegativeResponseCodes(const XmlNode *codes)
{
	m_negativeResponseCodes.DeepCopy(codes->getChildren());
}

void ProtocolFilter::SetLastTxTime( const uint64_t &lastTxTime)
{
	if( EOK == m_lastTxTime.Acquire())
	{
		m_lastTxTime.SetValue(lastTxTime);
		m_lastTxTime.Release();
	}
	else
	{
		perror( "Error locking m_lastTxTime in ProtocolFilter::SetLastTxTime()");
	}
}

void ProtocolFilter::SetStopCommsFlag( const bool &stopCommsFlag)
{
	try
	{
		if(m_stopCommsBepCondVar != NULL)
		{
			if( EOK == m_stopCommsBepCondVar->Acquire())
			{
				m_stopCommsBepCondVar->SetValue(stopCommsFlag);
				m_stopCommsBepCondVar->Release();
			}
			else
			{
				Log(LOG_ERRORS, "Error locking m_stopCommsBepCondVar in ProtocolFilter::SetStopCommsFlag()");
				int retries = 5;
				bool valueSet = false;
				do
				{
					Log(LOG_ERRORS, "ERROR: StopCommsFlag was not set! retries: %d\n", retries);
					BposSleep(10);
					if( EOK == m_stopCommsBepCondVar->Acquire())
					{
						m_stopCommsBepCondVar->SetValue(stopCommsFlag);
						m_stopCommsBepCondVar->Release();
						valueSet = true;
					}
				}
				while(!valueSet && retries--);
			}
		}

	}
	catch(...)
	{
		Log(LOG_ERRORS, "Exception trying to Acquire m_stopCommsBepCondVar");
	}
}

//-----------------------------------------------------------------------------
void ProtocolFilter::SetLastTxTime()
{
	uint64_t    lastTxTime;

	ClockTime( CLOCK_REALTIME, NULL, &lastTxTime);
	SetLastTxTime( lastTxTime);
}

//-----------------------------------------------------------------------------
bool ProtocolFilter::IsChecksumValidationRequired(void)
{
	return(m_checksumValidationRequired);
}

//-----------------------------------------------------------------------------
inline const string ProtocolFilter::ChecksumCalculationMethod(const string *method)	/*= NULL*/
{
	if(method != NULL)	m_checkSumCalculation = *method;
	return m_checkSumCalculation;
}
