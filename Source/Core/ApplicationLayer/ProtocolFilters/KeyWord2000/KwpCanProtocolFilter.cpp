//*************************************************************************
// FILE:
//    $Header: /Nissan/Source/Nissan/ApplicationLayer/ProtocolFilters/KwpBosch/KwpCanProtocolFilter.cpp 3     1/09/08 1:28p Mmcnamar $
//
// FILE DESCRIPTION:
//	Keyword 2000 protocol filter customized for Bosch module for Nissan
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
//    $Log: /Nissan/Source/Nissan/ApplicationLayer/ProtocolFilters/KwpBosch/KwpCanProtocolFilter.cpp $
// 
// 3     1/09/08 1:28p Mmcnamar
// Updated for CAN ABS: Updated ExtractMessage( ); Overrode SendMessage()
// from ProtocolFilter.cpp to not send checksum for CAN msgs.
// 
// 2     5/29/07 8:10a Gswope
// Fixed bug in constructor
// 
// 1     2/09/07 1:50p Gswope
// Initial entry
// 
// 1     2/23/05 8:26a Gswope
// new location, code as of 1/1/2005
// 
// 1     11/22/04 11:09a Gswope
// Updates for sensor calibration.
//    Revision 1.2  2004/04/10 20:54:54  bereza
//    code complete. config complete. all uncompiled yet
//
//    Revision 1.1  2004/04/10 13:00:10  bereza
//    start of day 2 at the plant. Got the module config file done, maybe, except for interpretation. Starting on custom ProtocolFilter
// 
//*************************************************************************

#include "KwpCanProtocolFilter.h"

KwpCanProtocolFilter::KwpCanProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar, BepMutex *commPortInUse /* =NULL*/) : 
ProtocolFilter(lastTxTime, stopCommsBepCondVar, commPortInUse),
	m_enterDiagModeCode(0x80), m_responsePendingCode(0x78), m_responsePendingReads(3), m_dataByteCountIndex(6),
	m_moduleRequestID(0x0000), m_autoEnterDiagMode(false), m_enterDiagnosticModeMessageTag("EnterDiagnosticMode")
{   // NOthing special to do here
}

KwpCanProtocolFilter::~KwpCanProtocolFilter()
{	// Clear out the buffer before we leave.
	Log(LOG_FN_ENTRY, "Resetting connection in ~KwpCanProtocolFilter()");
	ResetConnection();
	BposSleep(150);
	Log(LOG_FN_ENTRY, "~KwpCanProtocolFilter() complete\n");
}

bool KwpCanProtocolFilter::Initialize(const XmlNode *config)
{	// Get the negative response code for module not in diagnostic mode
	try
	{	// Get the code being used for module not in diagnostic mode
		SetEnterDiagnosticModeCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/EnterDiagnosticMode")->getValue().c_str()));
	}
	catch(XmlException &err)
	{
		SetEnterDiagnosticModeCode(0x80);
	}
	try
	{	// Get the code being used for response pending from the module
		SetResponsePendingCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/ResponsePending")->getValue().c_str()));
	}
	catch(XmlException &err)
	{
		SetResponsePendingCode(0x78);
	}
	try
	{	// Get the code being used for module busy, repeat request
		SetModuleBusyCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/ModuleBusy")->getValue().c_str()));
	}
	catch(XmlException &err)
	{
		SetModuleBusyCode(0x21);
	}
	// Get the message tag for entering diagnostic mode
	try
	{
		SetEnterDiagnosticModeMessageTag(config->getChild("Setup/EnterDiagnosticModeMessageTag")->getValue());
	}
	catch(XmlException &err)
	{
		SetEnterDiagnosticModeMessageTag("EnterDiagnosticMode");
	}
	// Determine if diagnostic mode should be automatically entered
	try
	{
		SetAutomaticallyEnterDiagnosticMode(config->getChild("Setup/AutomaticallyEnterDiagnosticMode")->getValue() == "Yes");
	}
	catch(XmlException &err)
	{
		SetAutomaticallyEnterDiagnosticMode(false);
	}
	// Store the max number of reads to perform for a response pending response
	try
	{
		SetResponsePendingReads(BposReadInt(config->getChild("Setup/ResponsePendingReads")->getValue().c_str()));
	}
	catch(XmlException &err)
	{
		SetResponsePendingReads(3);
	}
	// Store the module request ID
	try
	{
		SetModuleRequestID(atoh(config->getChild("Setup/ModuleRequestID")->getValue().c_str()));
	}
	catch(XmlException &err)
	{
		Log(LOG_ERRORS, ": CANNOT SEND MESSAGES TO MODULE WITHOUT ID!!!!!\n");
		throw;
	}
	// Store the data byte count index
	try
	{
		SetDataByteCountIndex(BposReadInt(config->getChild("Setup/DataByteCountIndex")->getValue().c_str()));
	}
	catch(XmlException &err)
	{
		Log(LOG_ERRORS, "Data byte count index not specified, using 6\n");
		SetDataByteCountIndex(6);
	}
	// Call the base class to complete the initialization
	return ProtocolFilter::Initialize(config);
}

const BEP_STATUS_TYPE KwpCanProtocolFilter::SendMessage(SerialString_t &message)
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
	if(bytesSent < 0)		status = BEP_STATUS_ERROR;
	else if (bytesSent == 0) status = BEP_STATUS_FAILURE;
	else if(bytesSent > 0)	status = BEP_STATUS_SUCCESS;
	// Return the status
	return status;
}
const BEP_STATUS_TYPE KwpCanProtocolFilter::SendPGNMessage(SerialString_t &message)
{	// do not add the module ID to the message, it must be included in message built from module config
	//message = GetModuleRequestID() + message;
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
	if(bytesSent < 0)		status = BEP_STATUS_ERROR;
	else if (bytesSent == 0) status = BEP_STATUS_FAILURE;
	else if(bytesSent > 0)	status = BEP_STATUS_SUCCESS;
	// Return the status
	return status;
}
const BEP_STATUS_TYPE KwpCanProtocolFilter::SendMessage(std::string messageTag)
{   // Call the base class
	return ProtocolFilter::SendMessage(messageTag);
}

const BEP_STATUS_TYPE KwpCanProtocolFilter::SendMessage(std::string messageTag, SerialArgs_t &args)
{   // Call the base class
	return ProtocolFilter::SendMessage(messageTag, args);
}

const BEP_STATUS_TYPE KwpCanProtocolFilter::SendPGNMessage(std::string messageTag)
{	// Send the message and return the result
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	std::string asciiMessage;
	SerialString_t xmtMessage;
	// Get the message to be sent
	if(GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
	{	// Convert the message to binary
		GetBinaryMssg(asciiMessage, xmtMessage);
		// Send the message
		status = SendPGNMessage(xmtMessage);
	}
	else
	{	// No message provided for this tag
		Log(LOG_ERRORS,"No message provided for tag: %s\n", messageTag.c_str());
		status = BEP_STATUS_SOFTWARE;
	}
	// Return the status
	return(status);
}

const BEP_STATUS_TYPE KwpCanProtocolFilter::SendPGNMessage(std::string messageTag, SerialArgs_t &args)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	SerialString_t xmtMessage;
	// Get the message to be sent
	if(GetXmtMssgString(messageTag, xmtMessage, args) == true)
	{	// Send the message
		status = SendPGNMessage(xmtMessage);
	}
	else
	{	// No message provided for this tag
		Log(LOG_ERRORS,"No message provided for tag: %s\n", messageTag.c_str());
		status = BEP_STATUS_SOFTWARE;
	}
	return(status);
}

const BEP_STATUS_TYPE KwpCanProtocolFilter::GetModuleData(string messageTag, SerialString_t &reply, SerialArgs_t *args /*= NULL*/)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string asciiMessage;
	SerialString_t xmtMessage;
	SerialString_t moduleResponse;
    INT32 tries = 0;
    //use message specific retries if specified
    GetResponseFailureRetryCount(messageTag, tries);
	tries = (tries <= 0) ? GetNumberOfRetries() : tries;
	if(GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
	{	// Convert the message to binary and attempt to send message multiple times
		GetBinaryMssg(asciiMessage, xmtMessage);
		do
		{	// Attempt to lock the port for our own use
			if((errno = m_commsInUse->Acquire()) == EOK)
			{	// Send the message to the module
				if(args == NULL) status = SendMessage(messageTag);
				else				  status = SendMessage(messageTag, *args);
				Log(LOG_DEV_DATA, "Sent message: %s to module - status: %s\n", messageTag.c_str(), 
					ConvertStatusToResponse(status).c_str());
				// Set the message ID
				SetSentMessageIdentifier(xmtMessage[GetSentMessageIdentifierIndex()]);
				// Store the number of response pending reads to perform
                // Use message specific response pending reads if specified
				INT32 responsePendingReads = -1;
                GetResponsePendingReads(messageTag, responsePendingReads);
                responsePendingReads = (responsePendingReads <= 0) ? 
                    GetNumberOfResponsePendingReads() : responsePendingReads;
				bool responsePending = false;	// Default to no response pending
				bool getResponse = true;        // Flag to signal if we need to get a response or process multiple messages
				do 
				{
					if(getResponse)
					{	// Clear the response area
						moduleResponse.erase();
						// If send was successful, get the module response
						if((BEP_STATUS_SUCCESS == status) || (responsePending && (BEP_STATUS_NA == status)))
						{   // Get the response from the module
							status = GetResponse(moduleResponse);
						}
					}
					else
					{   // Do not get a response, just process multiple messages in the buffer we have
						status = BEP_STATUS_SUCCESS;
					}
					// Extract the module response from the message
					if((status == BEP_STATUS_SUCCESS) && (moduleResponse.length() > 0))
					{
						reply = ExtractModuleData(moduleResponse);
					}
					// Make sure the response matches a filter before processing it
					if(IsResponseValid(messageTag, reply))
					{	// Check for a valid response if we got the response
						if((BEP_STATUS_SUCCESS == status) && (reply.length() > 0)) status = CheckForValidResponse(reply);
						// Check for a negative response from the module
						if((BEP_STATUS_SUCCESS != status) && (reply.length() > 0)) status = CheckForNegativeResponse(reply);
						// Check if it was a negaitve response
						if((BEP_STATUS_FAILURE == status) || (responsePending && (BEP_STATUS_SUCCESS != status) && (reply.length() > 0)))
						{	// Check what the negative response was
							UINT8 negativeResponseCode = reply[GetNegativeResponseCodeIndex()];
							if((GetEnterDiagnosticModeCode() == negativeResponseCode) && AutomaticallyEnterDiagnosticMode())
							{	// Need to enter diagnostic mode first
								SendMessage(GetEnterDiagnosticModeMessageTag());
								reply.erase();
								GetResponse(GetEnterDiagnosticModeMessageTag(), reply);
							}
							else if(GetResponsePendingCode() == negativeResponseCode)
							{	// Check if more than one message was returned in GetResponse
								if(moduleResponse.length() > 0)
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
							else if(GetModuleBusyCode() != negativeResponseCode)
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
						if(moduleResponse.length() > 0)
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
					if((BEP_STATUS_SUCCESS != status) && getResponse) BposSleep(GetResponseDelay());
					// Keep checking until we get a good message or response pending reads run out
                    Log(LOG_DEV_DATA, "Determining if we need to loop for more data in buffer - status: %s, responsePendingReads: %d, responsePending: %s, GetStopCommFlag: %s",
                        ConvertStatusToResponse(status).c_str(), responsePendingReads, responsePending ? "True" : "False", 
                        GetStopCommsFlag() ? "True" : "False");
				} while((BEP_STATUS_NA == status) && ((responsePendingReads > 0) && responsePending) && !GetStopCommsFlag());
				// Exit the critical section since we are done with the port for now
				m_commsInUse->Release();
			}
			else
			{   // Error aquiring the semaphore
				Log(LOG_ERRORS, "Could not Acquire mutex! Message %s not sent! - s\n", messageTag.c_str(), strerror(errno));
			}
			// Keep trying until a good response s received we run out of attempts or are signalled to stop sending messages
		} while((BEP_STATUS_SUCCESS != status) && (tries-- > 0) && !GetStopCommsFlag());
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
		else if(reply.length() <= 0)
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

const BEP_STATUS_TYPE KwpCanProtocolFilter::GetPGNModuleData(string messageTag, SerialString_t &reply, SerialArgs_t *args /*= NULL*/)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string asciiMessage;
	SerialString_t xmtMessage;
	SerialString_t moduleResponse;
    INT32 tries = 0;
	INT32 replyBytes = 0;
	NotifyEvent_t   mssgEvent;
	struct timespec currentTime;
	struct timespec startTime;
	long messageWaitTime = 500;
    //use message specific retries if specified
    GetResponseFailureRetryCount(messageTag, tries);
	tries = (tries <= 0) ? GetNumberOfRetries() : tries;
	if(GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
	{	// Convert the message to binary and attempt to send message multiple times
		GetBinaryMssg(asciiMessage, xmtMessage);
		// Bogus event (because we are going to poll)
		SIGEV_NONE_INIT( &mssgEvent);
		// Subscribe for RX data that matches the pattern defined by the messageTag
		//  message under the <ReplyMessages> node
		ILogicalPort::FilterSubscribe(mssgEvent, messageTag);
		do
		{	// Attempt to lock the port for our own use
			if((errno = m_commsInUse->Acquire()) == EOK)
			{	// Send the message to the module
				if(args == NULL) status = SendPGNMessage(messageTag);
				else				  status = SendPGNMessage(messageTag, *args);
				Log(LOG_DEV_DATA, "Sent message: %s to module - status: %s\n", messageTag.c_str(), 
					ConvertStatusToResponse(status).c_str());
					// Get the start time
					if(clock_gettime( CLOCK_REALTIME, &startTime) == -1)
					{
						throw BepException("Clock Gettime Error: %s", strerror(errno));
					}
					// Poll the comm proxy for the message
					bool validMessage = false;
					bool timeRemaining = true;
					double elapsedTime = 0;
					do
					{   // Read the port for the message
						replyBytes = ReadPort(messageTag, reply);
						// Check the message
						if(replyBytes > 0)
						{
							validMessage = IsResponseValid(messageTag, reply);
						}
						// Get the elapsed time
						clock_gettime( CLOCK_REALTIME, &currentTime);
						elapsedTime = (currentTime.tv_sec - startTime.tv_sec ) * 1000 + ( currentTime.tv_nsec - startTime.tv_nsec ) / 1000000;
						timeRemaining = messageWaitTime > elapsedTime;
						// Check if we should continue waiting for the message
					} while(!validMessage && timeRemaining && !GetStopCommsFlag());

				// Exit the critical section since we are done with the port for now
				m_commsInUse->Release();
			}
			else
			{   // Error aquiring the semaphore
				Log(LOG_ERRORS, "Could not Acquire mutex! Message %s not sent! - s\n", messageTag.c_str(), strerror(errno));
			}
			// Keep trying until a good response s received we run out of attempts or are signalled to stop sending messages
		} while((BEP_STATUS_SUCCESS != status) && (tries-- > 0) && !GetStopCommsFlag());
		// Unsubscribe from the port
		PortUnsubscribe();
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
		else if(reply.length() <= 0)
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

const BEP_STATUS_TYPE KwpCanProtocolFilter::GetModuleDataUUDTResponse(string messageTag, SerialString_t &reply, SerialArgs_t *args /*= NULL*/)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string asciiMessage;
	SerialString_t xmtMessage;
	SerialString_t moduleResponse;
	SerialString_t fullReply;
	SerialString_t intermediateReply;
	INT32 tries = 0;
	//use message specific retries if specified
	GetResponseFailureRetryCount(messageTag, tries);
	tries = (tries <= 0) ? GetNumberOfRetries() : tries;
	if(GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
	{	// Convert the message to binary and attempt to send message multiple times
		GetBinaryMssg(asciiMessage, xmtMessage);
		do
		{	// Attempt to lock the port for our own use
			if((errno = m_commsInUse->Acquire()) == EOK)
			{	// Send the message to the module
				if(args == NULL) status = SendMessage(messageTag);
				else				  status = SendMessage(messageTag, *args);
				Log(LOG_DEV_DATA, "Sent message: %s to module - status: %s\n", messageTag.c_str(), 
					ConvertStatusToResponse(status).c_str());
				// Set the message ID - reply for uudt message - pid read is first byte of response
				SetSentMessageIdentifier(xmtMessage[GetSentMessageIdentifierIndex()+1]);
				// Store the number of response pending reads to perform
				// Use message specific response pending reads if specified
				INT32 responsePendingReads = -1;
				GetResponsePendingReads(messageTag, responsePendingReads);
				responsePendingReads = (responsePendingReads <= 0) ? 
									   GetNumberOfResponsePendingReads() : responsePendingReads;
				bool fullResponseRecieved = false;
				do
				{

					bool responsePending = false;	// Default to no response pending
					bool getResponse = true;		// Flag to signal if we need to get a response or process multiple messages
					reply.erase();
					do 
					{
						if(getResponse)
						{	// Clear the response area
							moduleResponse.erase();
							// If send was successful, get the module response
							if((BEP_STATUS_SUCCESS == status) || (responsePending && (BEP_STATUS_NA == status)))
							{	// Get the response from the module
								status = GetResponseUUDT(moduleResponse);
							}
						}
						else
						{	// Do not get a response, just process multiple messages in the buffer we have
							status = BEP_STATUS_SUCCESS;
						}
						// Extract the module response from the message
						if((status == BEP_STATUS_SUCCESS) && (moduleResponse.length() > 0))
						{
							do
							{
								uint8_t     prependValue = 0x00;
								intermediateReply.erase();
								//losing prepended length (0x00) for some reason... add in here temp fix
								if(moduleResponse[0] != 0x00)
								{
									Log(LOG_DEV_DATA, "Prepending 0 0x00\n");
									moduleResponse = prependValue + moduleResponse;
								}
								if(moduleResponse[1] != 0x00)
								{
									Log(LOG_DEV_DATA, "Prepending 1 0x00\n");
									moduleResponse = prependValue + moduleResponse;
								}
								if(moduleResponse[2] != 0x00)
								{
									Log(LOG_DEV_DATA, "Prepending 2 0x00\n");
									moduleResponse = prependValue + moduleResponse;
								}
								intermediateReply = ExtractModuleData(moduleResponse);
								if(intermediateReply.length() > 0)
								{
									reply+= intermediateReply;
								}
							} while(intermediateReply.length() > 0);
						}
						// Make sure the response matches a filter before processing it
						if(IsResponseValid(messageTag, reply))
						{	// Check for a valid response if we got the response
							if((BEP_STATUS_SUCCESS == status) && (reply.length() > 0)) status = CheckForValidUUDTResponse(reply);
							// Check for a negative response from the module
							if((BEP_STATUS_SUCCESS != status) && (reply.length() > 0)) status = CheckForNegativeResponse(reply);
							// Check if it was a negaitve response
							if((BEP_STATUS_FAILURE == status) || (responsePending && (BEP_STATUS_SUCCESS != status) && (reply.length() > 0)))
							{	// Check what the negative response was
								UINT8 negativeResponseCode = reply[GetNegativeResponseCodeIndex()];
								if((GetEnterDiagnosticModeCode() == negativeResponseCode) && AutomaticallyEnterDiagnosticMode())
								{	// Need to enter diagnostic mode first
									SendMessage(GetEnterDiagnosticModeMessageTag());
									reply.erase();
									GetResponse(GetEnterDiagnosticModeMessageTag(), reply);
								}
								else if(GetResponsePendingCode() == negativeResponseCode)
								{	// Check if more than one message was returned in GetResponse
									if(moduleResponse.length() > 0)
									{	// More than one response in the buffer, extract and evaluate
										getResponse = false;
									}
									else
									{	// Only decrement response pending reads if we need to look for data in the buffer
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
								else if(GetModuleBusyCode() != negativeResponseCode)
								{	// Negative response with no special treatment, stop all attempts
									status = BEP_STATUS_FAILURE;
									tries = 0;
								}
							}
						}
						else
						{	// Not a response we were looking for
							Log(LOG_DEV_DATA, "Response for %s does not match any filter - discarding\n", messageTag.c_str());
							// Check if more than one response in the buffer
							if(moduleResponse.length() > 0)
							{	// More than one response in the buffer, extract and evaluate
								getResponse = false;
							}
							else
							{	// Only decrement response pending reads if we need to look for data in the buffer
								responsePendingReads--;
								getResponse = true;
							}
							status = BEP_STATUS_NA;
							responsePending = true;
						}
						// If this is not a valid message, wait a bit beofre trying again
						if((BEP_STATUS_SUCCESS != status) && getResponse) BposSleep(GetResponseDelay());
						// Keep checking until we get a good message or response pending reads run out
						Log(LOG_DEV_DATA, "Determining if we need to loop for more data in buffer - status: %s, responsePendingReads: %d, responsePending: %s, GetStopCommFlag: %s",
							ConvertStatusToResponse(status).c_str(), responsePendingReads, responsePending ? "True" : "False", 
							GetStopCommsFlag() ? "True" : "False");
					} while((BEP_STATUS_NA == status) && ((responsePendingReads > 0) && responsePending) && !GetStopCommsFlag());

					fullReply += reply;
					if(reply.length() < 1)
					{
						fullResponseRecieved = true;
						status = BEP_STATUS_SUCCESS;
						reply = fullReply;
					}
				} while(!GetStopCommsFlag() && !fullResponseRecieved);
				// Exit the critical section since we are done with the port for now
				m_commsInUse->Release();
			}
			else
			{	// Error aquiring the semaphore
				Log(LOG_ERRORS, "Could not Acquire mutex! Message %s not sent! - s\n", messageTag.c_str(), strerror(errno));
			}
			// Keep trying until a good response s received we run out of attempts or are signalled to stop sending messages
		} while((BEP_STATUS_SUCCESS != status) && (tries-- > 0) && !GetStopCommsFlag());
		// Check if valid message was retrieved
		if(GetStopCommsFlag())
		{
			Log(LOG_ERRORS, "Module Signaled to stop communications:StopCommsFlag - %s, Message - %s, status: %s\n", 
				GetStopCommsFlag() ? "true" : "false", messageTag.c_str(), ConvertStatusToResponse(status).c_str());
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
	{	// No message provided for this tag
		Log(LOG_ERRORS, "No message provided for tag: %s\n", messageTag.c_str());
		status = BEP_STATUS_SOFTWARE;
	}
	Log(LOG_DEV_DATA, "GetModuleData() returning - status: %s\n", ConvertStatusToResponse(status).c_str());
	return status;
}
const BEP_STATUS_TYPE KwpCanProtocolFilter::GetResponseUUDT(SerialString_t &reply)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Get the response from the port
	INT32 bytesRead = WaitForFullResponseUUDT(reply);
	// Evaluate the number of bytes
	Log(LOG_DETAILED_DATA, "GetResponse retrieved %d bytes from the module\n", bytesRead);
	if(bytesRead > 0)		 status = BEP_STATUS_SUCCESS;
	else if(bytesRead == -1) status = BEP_STATUS_ERROR;
	else					  status = BEP_STATUS_FAILURE;
	// Return the status
	return(status);
}
int KwpCanProtocolFilter::WaitForFullResponseUUDT( SerialString_t &response)
{
	int         retVal = 1;
	SerialString_t interResponse;
	// set up the receive timeout
	//ComMssgTableEntry msgAttr;
	Log(LOG_DEV_DATA, "Waiting for response from module\n");
	Log(LOG_DEV_DATA, "\t -- Module Response Delay: %d\n", GetResponseDelay());
	Log(LOG_DEV_DATA, "\t -- Response End Delay: %d\n", GetResponseEndDelay());
	while(retVal > 0)
	{
		BposSleep(10);
		interResponse.erase();
		retVal = ReadPort( interResponse, GetResponseDelay(), GetResponseEndDelay());
		Log(LOG_DEV_DATA, "inter response.  %d bytes received\n", retVal);
		// Log the respnse received from the module
		char temp[256];
		string res;
		for(UINT16 ii = 0; ii < interResponse.length(); ii++)
			res += CreateMessage(temp, 256, "$%02X ", interResponse[ii]);
		Log(LOG_DETAILED_DATA, "inter response res: %s\n", res.c_str());
		response += interResponse;
	}
	Log(LOG_DEV_DATA, "Done waiting for full response.  %d bytes received\n", response.length());
	string res;
	char temp[256];
	//char temp[256];
	for(UINT16 ii = 0; ii < response.length(); ii++)
		res += CreateMessage(temp, 256, "$%02X ", response[ii]);
	Log(LOG_DETAILED_DATA, "Full response received: %s\n", res.c_str());
	retVal = response.length();

	return( retVal);
}

//---------------------------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE KwpCanProtocolFilter::GetBusBroadcastMessage(string messageTag, 
																   const long messageWaitTime, 
																   SerialString_t &busMssg)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	INT32 replyBytes = 0;
	NotifyEvent_t   mssgEvent;
	struct timespec currentTime;
	struct timespec startTime;
	Log(LOG_FN_ENTRY, "Enter KwpCanProtocolFilter::GetBusBroadcastMsg(%s)", messageTag.c_str());
	// Bogus event (because we are going to poll)
	SIGEV_NONE_INIT( &mssgEvent);
	// Subscribe for RX data that matches the pattern defined by the messageTag
	//  message under the <ReplyMessages> node
	ILogicalPort::FilterSubscribe(mssgEvent, messageTag);
	// Get the start time
	if(clock_gettime( CLOCK_REALTIME, &startTime) == -1)
	{
		throw BepException("Clock Gettime Error: %s", strerror(errno));
	}
	// Poll the comm proxy for the message
	bool validMessage = false;
	bool timeRemaining = true;
	double elapsedTime = 0;
	do
	{   // Read the port for the message
		replyBytes = ReadPort(messageTag, busMssg);
		// Check the message
		if(replyBytes > 0)
		{
			validMessage = IsResponseValid(messageTag, busMssg);
		}
		// Get the elapsed time
		clock_gettime( CLOCK_REALTIME, &currentTime);
		elapsedTime = (currentTime.tv_sec - startTime.tv_sec ) * 1000 + ( currentTime.tv_nsec - startTime.tv_nsec ) / 1000000;
		timeRemaining = messageWaitTime > elapsedTime;
		// Check if we should continue waiting for the message
	} while(!validMessage && timeRemaining);
	// Unsubscribe from the port
	PortUnsubscribe();
	// Determine the status to return
	status = validMessage ? BEP_STATUS_SUCCESS : BEP_STATUS_TIMEOUT;
	Log(LOG_FN_ENTRY, "Exiting KwpCanProtocolFilter::GetBusBroadcastMsg(%s) - status: %s", 
		messageTag.c_str(), ConvertStatusToResponse(status).c_str());
	return status;
}

//---------------------------------------------------------------------------------------------------------------------
const SerialString_t KwpCanProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{
	string fullResponse;
	// Log the respnse received from the module
	char temp[256];
	for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
		fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
	Log(LOG_DETAILED_DATA, "Extracting data from response string: %s\n", fullResponse.c_str());
	// Look at the number of data bytes returned from the module
	UINT8 numberOfBytes = moduleResponse[GetDataByteCountIndex()];
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
		for(UINT16 ii = 0; ii < dataResponse.length(); ii++)
			fullResponse += CreateMessage(temp, 256, "$%02X ", dataResponse[ii]);
		Log(LOG_DEV_DATA, "Extracted module data: %s\n", fullResponse.c_str());
		// Check if multiple messages are in the module
		Log(LOG_DETAILED_DATA, "Total Response Length: %d, Current Message Length: %d, Number of Bytes: %d\n", 
			moduleResponse.length(), (GetDataByteCountIndex() + numberOfBytes + 1), numberOfBytes);
		if((UINT16)moduleResponse.length() > (GetDataByteCountIndex() + numberOfBytes + 1))
		{   // Multiple messages in the response, update to point at next message
			moduleResponse = moduleResponse.substr(GetDataByteCountIndex() + numberOfBytes + 1);
			// Log the data remaining in the buffer
			fullResponse.erase();
			for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
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
		for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
			fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
		Log( LOG_ERRORS, "Incomplete message in ExtractModuleData: <%s>\n", fullResponse.c_str());
		fullResponse.clear();
	}
	// Return the extracted data	
	return dataResponse;        
}

int KwpCanProtocolFilter::WaitForFullResponse( SerialString_t &response)
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

	while(true)
	{	// Try to read next character
		if( (bytesRead=ReadPort(&rcvByte, 1, responseDelay)) > 0)
		{
			response.push_back( rcvByte);
			retVal++;
			// Bytes are flowing in, can decrease delay time
			responseDelay = GetResponseEndDelay();

			Log(LOG_DEV_DATA, "\t Got %d (%d total) bytes...setting delay to %d ms\n", bytesRead, retVal, responseDelay);
		}
		// No more data
		else if( bytesRead == 0)
		{	// No more data available..so leave
			Log(LOG_DEV_DATA, "All data received\n");
			break;
		}
		// Error reading byte
		else if( errno != ETIMEDOUT)
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

inline const UINT8& KwpCanProtocolFilter::GetEnterDiagnosticModeCode(void)
{	// Return the negative response code for module not in diag mode
	return m_enterDiagModeCode;
}

inline const UINT8& KwpCanProtocolFilter::GetResponsePendingCode(void)
{	// Return the code for response pending
	return m_responsePendingCode;
}

inline const UINT8& KwpCanProtocolFilter::GetModuleBusyCode(void)
{	// Return the code for module busy, repeat request
	return m_moduleBusyCode;
}

inline const string& KwpCanProtocolFilter::GetEnterDiagnosticModeMessageTag(void)
{	// Return the tag for the enter diagnostic mode message
	return m_enterDiagnosticModeMessageTag;
}

inline const INT32& KwpCanProtocolFilter::GetNumberOfResponsePendingReads(void)
{
	return m_responsePendingReads;
}

inline const bool& KwpCanProtocolFilter::AutomaticallyEnterDiagnosticMode(void)
{
	return m_autoEnterDiagMode;
}

inline const SerialString_t KwpCanProtocolFilter::GetModuleRequestID(void)
{
	SerialString_t moduleRequestID;
	moduleRequestID.push_back((uint8_t)((m_moduleRequestID & 0xFF000000) >> 24));
	moduleRequestID.push_back((uint8_t)((m_moduleRequestID & 0x00FF0000) >> 16));
	moduleRequestID.push_back((uint8_t)((m_moduleRequestID & 0x0000FF00) >> 8));
	moduleRequestID.push_back((uint8_t)(m_moduleRequestID & 0x000000FF));
	return moduleRequestID;
}

inline const INT32& KwpCanProtocolFilter::GetDataByteCountIndex(void)
{
	return m_dataByteCountIndex;
}

inline void KwpCanProtocolFilter::SetEnterDiagnosticModeCode(const UINT8 &enterDiagnosticModeCode)
{	// Save the enter diagnostic mode code
	m_enterDiagModeCode = enterDiagnosticModeCode;
}

inline void KwpCanProtocolFilter::SetResponsePendingCode(const UINT8 &responsePendingCode)
{	// Save the response pending code
	m_responsePendingCode = responsePendingCode;
}

inline void KwpCanProtocolFilter::SetModuleBusyCode(const UINT8 &moduleBusyCode)
{	// Save the module busy, repeat request code
	m_moduleBusyCode = moduleBusyCode;
}

inline void KwpCanProtocolFilter::SetEnterDiagnosticModeMessageTag(const string& tag)
{	// Save the enter diagnostic mode message tag
	m_enterDiagnosticModeMessageTag = tag;
}

inline void KwpCanProtocolFilter::SetAutomaticallyEnterDiagnosticMode(const bool& autoEnterDiagMode)
{
	m_autoEnterDiagMode = autoEnterDiagMode;
}

inline void KwpCanProtocolFilter::SetResponsePendingReads(const INT32& responsePendingReads)
{
	m_responsePendingReads = responsePendingReads;
}

inline void KwpCanProtocolFilter::SetModuleRequestID(const UINT32& moduleID)
{
	m_moduleRequestID = moduleID;
}

inline void KwpCanProtocolFilter::SetDataByteCountIndex(const INT32& dataByteCountindex)
{
	m_dataByteCountIndex = dataByteCountindex;
}

