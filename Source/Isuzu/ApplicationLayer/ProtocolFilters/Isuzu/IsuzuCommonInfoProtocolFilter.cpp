//*************************************************************************
// FILE DESCRIPTION:
//	Protocol Filter for handling the Isuzu On-Board Unit Common Information
//  Interface Protocol.
//
//===========================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "IsuzuCommonInfoProtocolFilter.h"

//-------------------------------------------------------------------------------------------------
IsuzuCommonInfoProtocolFilter::IsuzuCommonInfoProtocolFilter(KeepAliveTimer_t &lastTxTime, 
															 StopCommsBepCondVar *stopCommsBepCondVar/* = NULL*/,
															 BepMutex *commPortInUse /*= NULL*/) : 
ProtocolFilter(lastTxTime, stopCommsBepCondVar, commPortInUse) 
{  
	m_msgSerialNumber = 0x01;
}

//-------------------------------------------------------------------------------------------------
IsuzuCommonInfoProtocolFilter::~IsuzuCommonInfoProtocolFilter()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void IsuzuCommonInfoProtocolFilter::AddChecksumToMessage(SerialString_t &message)
{
	UINT16 checksum = 0x0000;
	for(UINT16 index = 1; index < message.length(); index++)
	{
		CrcCalc(message[index], &checksum);
	}
	// Set the checksum in the message
	message.push_back((checksum & 0xFF00) >> 8);
	message.push_back(checksum & 0x00FF);
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE IsuzuCommonInfoProtocolFilter::CheckForValidResponse(const SerialString_t &moduleResponse)
{
    Log(LOG_DEV_DATA,"IsuzuCommonInfoProtocolFilter::CheckForValidResponse() - Enter\n");
    string fullResponse = "";

	char buff[196];
	for(UINT32 index = 0; index < moduleResponse.length(); index++)
	{
		fullResponse += CreateMessage(buff, sizeof(buff), "$%02X ", moduleResponse[index]);
	}
    Log(LOG_DEV_DATA, "Msg:  %s", fullResponse.c_str());

    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if((moduleResponse[0] == STX) && (moduleResponse[moduleResponse.length() - 3] == ETX))
	{
		status = BEP_STATUS_SUCCESS;
	}
	else
	{
		status = BEP_STATUS_FAILURE;
	}
	Log(LOG_DEV_DATA, "Check for valid response: %s", ConvertStatusToResponse(status).c_str());
    Log(LOG_DEV_DATA,"IsuzuCommonInfoProtocolFilter::CheckForValidResponse() - Exit\n");
	return status;
}

//-------------------------------------------------------------------------------------------------
void IsuzuCommonInfoProtocolFilter::CrcCalc(UINT8 dstDat, UINT16 *srcDat)
{
	UINT16 temp = *srcDat ^ dstDat;
	for(UINT8 index = 0; index < 8; index++)
	{
		if(temp & 0x0001)
		{
			temp = (temp >> 1) ^ CRC_POLY;
		}
		else 
		{
			temp >>= 1;
		}
	}
	*srcDat = temp;
}

//-------------------------------------------------------------------------------------------------
const SerialString_t IsuzuCommonInfoProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{
	Log(LOG_FN_ENTRY, "IsuzuCommonInfoProtocolFilter::ExtractModuleData() - Enter");
	string fullResponse;
	SerialString_t responseData;
	vector<SerialString_t> messages;
	char buff[4096];
	int messageLength, totalResponseBytes;

	// If multiple messages are in the buffer, extract each message for processing
	do
	{
		totalResponseBytes = moduleResponse.length();
		messageLength = ((moduleResponse[TotalDataByteIndex] & 0x07) << 8) | moduleResponse[TotalDataByteIndex+1];
		messageLength += (1 + 1 + 1 + 2);  // Header bytes
		SerialString_t temp = moduleResponse.substr(0, messageLength);
		messages.push_back(temp);
		moduleResponse = moduleResponse.substr(messageLength);
	} while(totalResponseBytes > messageLength);

	Log(LOG_DEV_DATA, "%d messages found in response", messages.size());
	for(int msgIndex = 0; msgIndex < messages.size(); msgIndex++)
	{
		fullResponse = "";
		for(UINT32 index = 0; index < messages[msgIndex].length(); index++)
		{
			fullResponse += CreateMessage(buff, sizeof(buff), "$%02X ", messages[msgIndex][index]);
		}
		Log(LOG_DEV_DATA, "Msg %d:  %s", msgIndex, fullResponse.c_str());
	}

	for(int msgIndex = 0; msgIndex < messages.size(); msgIndex++)
	{
		fullResponse = "";
		for(UINT32 index = 0; index < messages[msgIndex].length(); index++)
		{
			fullResponse += CreateMessage(buff, sizeof(buff), "$%02X ", messages[msgIndex][index]);
		}
		Log(LOG_DEV_DATA, "Extracting data from response string: %s", fullResponse.c_str());
		UINT16 dbCount = ((messages[msgIndex][TotalDataByteIndex] & 0x07) << 8) | messages[msgIndex][TotalDataByteIndex+1];
		if(messages[msgIndex].length() >= dbCount)
		{
			//m_msgSerialNumber = messages[msgIndex][MsgSerialNumberIndex];
			UINT16 dataByteCount = (messages[msgIndex][MsgDataLengthIndex] << 8) | messages[msgIndex][MsgDataLengthIndex+1];
			Log(LOG_DEV_DATA, "Message Serial Number: %02X, Data Bytes: %d", m_msgSerialNumber, dataByteCount);
			//Log(LOG_DEV_DATA, "Incremented message serial number: %02X", m_msgSerialNumber);
			responseData = messages[msgIndex].substr(DataBodyStartIndex, dataByteCount);
		}
		else
		{
			Log(LOG_ERRORS, "Invalid message received at message index %d", msgIndex);
		}
	}
	Log(LOG_FN_ENTRY, "IsuzuCommonInfoProtocolFilter::ExtractModuleData() - Exit");
	return responseData;
}

//-------------------------------------------------------------------------------------------------
bool IsuzuCommonInfoProtocolFilter::IsChecksumValid(SerialString_t &moduleResponse)
{
	Log(LOG_FN_ENTRY, "IsuzuCommonInfoProtocolFilter::IsChecksumValid() - Enter");
	UINT16 msgLength = moduleResponse.length();
	UINT16 msgChecksum = (moduleResponse[msgLength - 2] << 8) | moduleResponse[msgLength - 1];
	UINT16 dataLength = ((moduleResponse[TotalDataByteIndex] << 8) | moduleResponse[TotalDataByteIndex+1]) & 0x7FFF;
	UINT16 calcChecksum = 0x0000;
	for(UINT16 index = 1; index < dataLength; index++)
	{
		CrcCalc(moduleResponse[index], &calcChecksum);
	}
	Log(LOG_DEV_DATA, "Calculated Checksum: %04X,  Message Checksum: %04X", calcChecksum, msgChecksum);
	Log(LOG_FN_ENTRY, "IsuzuCommonInfoProtocolFilter::IsChecksumValid() - Exit");
	return calcChecksum == msgChecksum;
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE IsuzuCommonInfoProtocolFilter::SendMessage(SerialString_t &message)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	SerialString_t msg;
	UINT16 dataLen = message.length() - 2;
    Log(LOG_DEV_DATA,"IsuzuCommonInfoProtocolFilter::SendMessage - Enter\n");
    PrintSerialString( "Isuzu ProtocolFilter SendMessage:", message);
	/*
	 * Add ifxed data fields to the message length:
	 *  	4 = Device Number Length
	 * 		1 = Message Serial Number
	 *		2 = Message Length
	 *		1 = ETX
	 *		2 = CRC
	 */
	UINT16 totalBytes = message.length() + 4 + 1 + 2 + 1 + 2;
	if((message[0] == 0x31) && (message[1] == 0x0B))
	{
		totalBytes |= 0x0000;
	}
	msg.push_back(STX);
	msg.push_back(COMPRESSION_TYPE);
	msg.push_back(0x71);                                    // 3. Response/Request ID
	msg.push_back((totalBytes & 0xFF00) >> 8);              // 4. Total Data Bytes
	msg.push_back(totalBytes & 0x00FF);
	msg.push_back(0x00);                                    // 5. Device Number
	msg.push_back(0x00);
	msg.push_back(0x00);
	msg.push_back(0x00);
	msg.push_back(message[0]);                              // 6. Message ID
	msg.push_back(message[1]);
	msg.push_back(m_msgSerialNumber);                     // 7. Message Serial Number
	msg.push_back((dataLen & 0xFF00) >> 8);                 // 8. Message Length
	msg.push_back(dataLen & 0x00FF);
	for(UINT32 index = 2; index < message.length(); index++)// 9. Data Body
	{
		msg.push_back(message[index]);
	}
	msg.push_back(ETX);
	AddChecksumToMessage(msg);
	// Clear the Fifos so bad data is not gathered
	Log(LOG_DETAILED_DATA, "Resetting The Connection\n");
	ResetConnection();
	Log(LOG_DETAILED_DATA, "Reset The Connection\n");
	// Send the message and return the result
	PrintSerialString( "Isuzu Mima ProtocolFilter Sending", msg);
	// Write message
	INT32 bytesSent = ILogicalPort::WritePort(msg, GetNumberOfRetries());
	SetLastTxTime();
	Log(LOG_DETAILED_DATA, "Sent the message\n");
	//m_msgSerialNumber++;
	// Determine the result of the write
	if(bytesSent < 0)		status = BEP_STATUS_ERROR;
	else if(bytesSent == 0)	status = BEP_STATUS_FAILURE;
	else if(bytesSent > 0)	status = BEP_STATUS_SUCCESS;

	return status;
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE IsuzuCommonInfoProtocolFilter::SendMessage(string messageTag)
{
	return ProtocolFilter::SendMessage(messageTag);
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE IsuzuCommonInfoProtocolFilter::SendMessage(string messageTag, SerialArgs_t &args)
{
	return ProtocolFilter::SendMessage(messageTag, args);
}
//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE IsuzuCommonInfoProtocolFilter::GetResponse(const std::string &messageTag, SerialString_t &reply)
{
	BEP_STATUS_TYPE status = BEP_STATUS_NA;
	// Get the response from the module
	INT32 retry = 0;		// retry counter
	INT32 bytesRead = 0;	// the number of bytes read
    vector<SerialString_t> messages;
    int messageLength, totalResponseBytes;

    Log(LOG_DEV_DATA, "tk:gr1");
	do
	{	// read the data from the port
        Log(LOG_DEV_DATA, "tk:gr1 - wffr");
        bytesRead = WaitForFullResponse(reply);
		Log(LOG_DETAILED_DATA, "Received %d bytes for message %s\n", bytesRead, messageTag.c_str());
		// if data is read from the port, determine if it is done
		if(bytesRead > 0)
		{	// Log the response if enabled and flag set
			if(GetLogStatus() && (GetVerboseMask() & LOG_DEV_DATA))
			{
				std::string response;		// response
				char buffer[256];			// buffer for printing
				for(INT16 ii = 0; ii < bytesRead; ii++)
					response += CreateMessage(buffer, sizeof(buffer),"$%02X ", reply[ii]);
				Log(LOG_DETAILED_DATA, "Response bytes received: %d for message: %s\tResponse: %s\n\n",
					bytesRead, messageTag.c_str(), response.c_str());
			}

            try
            {
                messages.clear();
                // If multiple messages are in the buffer, extract each message for processing
	            do
	            {
        		    totalResponseBytes = reply.length();
        		    messageLength = ((reply[TotalDataByteIndex] & 0x07) << 8) | reply[TotalDataByteIndex+1];
        		    messageLength += (1 + 1 + 1 + 2);  // Header bytes
        		    SerialString_t temp = reply.substr(0, messageLength);
        		    messages.push_back(temp);
        		    reply = reply.substr(messageLength);
        	    } while(totalResponseBytes > messageLength);

                Log(LOG_DEV_DATA, "%d messages found in response", messages.size());
        	    for(int msgIndex = 0; msgIndex < messages.size(); msgIndex++)
        	    {
                    string response2;
                    char buffer[256];
                    for(INT16 ii = 0; ii < messages[msgIndex].length(); ii++)
                    {
                        response2 += CreateMessage(buffer, sizeof(buffer),"$%02X ", messages[msgIndex][ii]);
                        //Log(LOG_DEV_DATA, "char:%02X\n",messages[msgIndex][ii]);
                    }
                    

                    Log(LOG_DEV_DATA, "Message %d:%s - Length: %d", msgIndex, response2.c_str(), messages[msgIndex].length());
    			    // Validate the response against all filters
    			    if(IsResponseValid(messageTag, messages[msgIndex]))
    			    {
    				    Log(LOG_DETAILED_DATA, "Full response received\n");
    				    status = BEP_STATUS_SUCCESS;
                        reply = messages[msgIndex]; 
    			    }
    			    else
    				    Log(LOG_ERRORS, "Incomplete response\n");

                    retry++;
    			    //retry = 0;	// rest the retry counter, due data received 
                }
            }
            catch(exception i)
            {
                Log(LOG_DEV_DATA,"Exception in GetResponse: %s",i.what());
                retry++;
                break;
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
				Log(LOG_DETAILED_DATA, "Waiting for full message\n");
			}
			retry++;	// increment the retry counter
		}
        if (status == BEP_STATUS_NA)
        {
            delay(30);
        }

		// while the result not detemined and retries remain
	}while((status == BEP_STATUS_NA) && (retry < 5) && !GetStopCommsFlag());

	// if a successful read never occurred, fail
	if(retry >= 5) status = BEP_STATUS_FAILURE;

	Log(LOG_FN_ENTRY, "Exit IsuzuCommonInfoProtocolFilter::GetResponse: %d\n", status);

	// Return the status
	return(status);
}

const BEP_STATUS_TYPE IsuzuCommonInfoProtocolFilter::GetModuleData(std::string messageTag, SerialString_t &reply, SerialArgs_t *args)
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
			Log(LOG_DETAILED_DATA, "IsuzuCommonInfoProtocolFilter::GetModuleData Locked Port\n");
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
                    Log(LOG_DETAILED_DATA, "IsuzuCommonInfoProtocolFilter::GetModuleData clear response area");
					// Set the message ID
					// If send was successful, get the module response
					if(BEP_STATUS_SUCCESS == status)
                    { 
                        Log(LOG_DETAILED_DATA, "IsuzuCommonInfoProtocolFilter::GetModuleData getting module response");
                        status = GetResponse(messageTag, reply);
                    }
					// If this is not a valid message, wait a bit beofre trying again
					if(BEP_STATUS_SUCCESS != status)
                    {
                        Log(LOG_DETAILED_DATA, "IsuzuCommonInfoProtocolFilter::GetModuleData not valid message");
                        //BposSleep(GetResponseDelay());
                    }
				}
				else
				{

                    // If response NOT expected, exit do-while loop
					status = BEP_STATUS_SUCCESS;
                    Log(LOG_DETAILED_DATA, "IsuzuCommonInfoProtocolFilter::GetModuleData response not expected");

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
