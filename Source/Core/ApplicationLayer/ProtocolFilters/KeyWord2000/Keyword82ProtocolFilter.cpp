//******************************************************************************
// Description:
//  Keyword 82 Protocol Filter for communicating with vehicle modules.
//
//==============================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#include <time.h>
#include <sys/syspage.h>
#include "Keyword82ProtocolFilter.h"

//-----------------------------------------------------------------------------
Keyword82ProtocolFilter::Keyword82ProtocolFilter(KeepAliveTimer_t &lastTxTime, 
                                                 StopCommsBepCondVar *stopCommsBepCondVar, /*= NULL*/ 
                                                 BepMutex *commsInUse) /*= NULL*/ : 
ProtocolFilter(lastTxTime, stopCommsBepCondVar, commsInUse)
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
Keyword82ProtocolFilter::~Keyword82ProtocolFilter()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
bool Keyword82ProtocolFilter::Initialize(const XmlNode *config)
{   // Call the base class to begin the initialization
    bool init = ProtocolFilter::Initialize(config);
    // Store the number of attempts for detecting a message transmit window
    INT32 attempts = 0;
    try
    {
        attempts = BposReadInt(config->getChild("Setup/MessageWindowDetectAttempts")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Message window detect attempts not specified, using 0: %s", excpt.GetReason());
        attempts = 0;
    }
    Log(LOG_DEV_DATA, "Setting message window detect attempts to %d", attempts);
    MessageWindowDetectAttempts(&attempts);
    // Store the amount of time to wait in order to detect a message window
    UINT32 windowTime = 0;
    try
    {
        windowTime = BposReadInt(config->getChild("Setup/MessageSendWindowDetectTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Send message window detect time not specified, using 0: %s", excpt.GetReason());
        windowTime = 0;
    }
    Log(LOG_DEV_DATA, "Setting window detect time to %dms", windowTime);
    MessageWindowDetectTime(&windowTime);
    // Return the result
    return init;
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE Keyword82ProtocolFilter::SendMessage(SerialString_t &message)
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
        // Send the message and return the result
        PrintSerialString( "ProtocolFilter Sending", message);
        // Wait for an opportunity on the comm bus
        if(WaitForSendWindow())
        {   // Clear the Fifos so bad data is not gathered
            //ResetConnection();
            Log(LOG_DETAILED_DATA, "Transmit opportunity detected, sending message");
            // Write message
            INT32 bytesSent = ILogicalPort::WritePort(message, GetNumberOfRetries());
            ResetConnection();

            SetLastTxTime();

            Log(LOG_DETAILED_DATA, "Sent the message");
            // Determine the result of the write
            if(bytesSent < 0)       status = BEP_STATUS_ERROR;
            else if(bytesSent == 0) status = BEP_STATUS_FAILURE;
            else if(bytesSent > 0)  status = BEP_STATUS_SUCCESS;
        }
        else
        {
            Log(LOG_ERRORS, "Could not detect window for sending the message");
            status = BEP_STATUS_FAILURE;
        }
    }
    else
    {
        Log(LOG_ERRORS, "ERROR Module Message length shorter than Data Start Index "
            "Message Length: %d\n\tData Start Index: %d", message.length(),
            GetDataStartIndex());
        status = BEP_STATUS_ERROR;
    }
    // Return the status
    return(status);
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE Keyword82ProtocolFilter::SendMessage(string messageTag)
{   // Call the base class to do it's thing
    return ProtocolFilter::SendMessage(messageTag);
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE Keyword82ProtocolFilter::SendMessage(string messageTag, SerialArgs_t &args)
{   // Call the base class to do it's thing
    return ProtocolFilter::SendMessage(messageTag, args);
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE Keyword82ProtocolFilter::GetResponse(const string &messageTag, SerialString_t &reply)
{
    BEP_STATUS_TYPE status = BEP_STATUS_NA;
    // Get the response from the module
    INT32 retry = 0;        // retry counter
    INT32 bytesRead = 0;    // the number of bytes read
    do
    {   // read the data from the port
        bytesRead = WaitForFullResponse(reply);
        Log(LOG_DETAILED_DATA, "Received %d bytes for message %s", bytesRead, messageTag.c_str());
        // if data is read from the port, determine if it is done
        if(bytesRead > 0)
        {   // Log the response if enabled and flag set
            if(GetLogStatus() && (GetVerboseMask() & LOG_DEV_DATA))
            {
                string response;       // response
                char buffer[128];      // buffer for printing
                for(INT16 ii = 0; ii < bytesRead; ii++)
                    response += CreateMessage(buffer, sizeof(buffer),"$%02X ", reply[ii]);
                Log(LOG_DETAILED_DATA, "Response bytes received: %d for message: %s\tResponse: %s",
                    bytesRead, messageTag.c_str(), response.c_str());
            }
            // Validate the response against all filters
            if(IsResponseValid(messageTag, reply))
            {
                Log(LOG_DETAILED_DATA, "Full response received");
                status = BEP_STATUS_SUCCESS;
            }
            else
            {
                Log(LOG_ERRORS, "Incomplete response, clearing buffer");
                reply.clear();
            }
        }
        else
        {   // if a problem detected reading the port
            if(bytesRead < 0)
            {   // Error reading data from the port
                status = BEP_STATUS_ERROR;
            }
            // else if the reply is empty and already performed another attempt
            else if(reply.empty() && retry)
            {   // No data sent from the module
                status = BEP_STATUS_FAILURE;
            }
            // else wait the full retry attempts to see the message received
            else
            {
                Log(LOG_DETAILED_DATA, "Waiting for full message");
            }
        }
        // while the result not detemined and retries remain
    }while((status == BEP_STATUS_NA) && (retry++ < 3) && !GetStopCommsFlag());
    // if a successful read never occurred, fail
    if(retry >= 3) status = BEP_STATUS_FAILURE;
    Log(LOG_FN_ENTRY, "Exit GetResponse: %d", status);
    // Return the status
    return(status);
}

//-----------------------------------------------------------------------------
const SerialString_t Keyword82ProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
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

    if(validChecksum)
    {   // ensure the response is of proper length to prevent sig seg
        if((moduleResponse.length() > 3) && (moduleResponse.length() > GetDataStartIndex()))
        {   // Strip off the header and checksum
            dataResponse = moduleResponse.substr(GetDataStartIndex(),(moduleResponse[0]-GetDataHeaderLength()));
            PrintSerialString("ExtractModuleData() Extracted module data", dataResponse, LOG_ALL);
        }
        else
        {
            Log(LOG_ERRORS,"ERROR Module response length of %d is too short to be valid\n", moduleResponse.length());
        }
    }
    else
    {
        Log(LOG_ERRORS, "KeywordProtocolFilter::ExtractModuleData - Invalid Checksum on message, not extracting response\n");
        dataResponse.clear();
    }
    // Return the extracted data
    return dataResponse;
}

//-----------------------------------------------------------------------------
void Keyword82ProtocolFilter::AddChecksumToMessage(SerialString_t &message)
{
    UINT16 checksum = ComputeTwoByteCheckSum(message);
    PrintSerialString( "Keyword82ProtocolFilter Adding checksum to", message);
    // Add the checksum to the message
    message.push_back((checksum & 0xFF00) >> 8);
    message.push_back(checksum & 0x00FF);
    PrintSerialString( "Keyword82ProtocolFilter message with checksum", message);
}

//-----------------------------------------------------------------------------
UINT16 Keyword82ProtocolFilter::ComputeTwoByteCheckSum(const SerialString_t &message)
{
    UINT16 checksum = 0;
    if(!ChecksumCalculationMethod().compare(ChecksumCalculationXor))
    {   // XOR all message bytes
        for(UINT16 byte = 0; byte < message.length(); byte++)
            checksum = (checksum ^ message[byte]) & 0xFFFF;
    }
    else
    {   // Add up all the bytes in the message
        for(UINT16 byte = 0; byte < message.length(); byte++)
            checksum = (checksum + message[byte]) & 0xFFFF;
    }
    // Return the checksum
    Log(LOG_DEV_DATA, "Calculated checksum: %04X using %s method", checksum, ChecksumCalculationMethod().c_str());
    return(checksum);
}

//-----------------------------------------------------------------------------
bool Keyword82ProtocolFilter::WaitForSendWindow(void)
{
    bool okToSend = false;
    bool messageGapDetected = false;
    SerialString_t junkData;
    INT32 bytesRead = 0;
    INT32  windowAttempts = MessageWindowDetectTime();
    INT8 maxSendAttempts = MessageWindowDetectAttempts();
    // Wait while we detect a message in progress
    do
    {
        Log(LOG_DEV_DATA, "Resetting the connection for waiting for module auto puke");
        ResetConnection();
        bytesRead = ReadPort(junkData, GetResponseDelay(), 0);
    } while((bytesRead != -1) && maxSendAttempts--);
    // Now wait for the start of a message so we can determine the number of bytes to be received
    maxSendAttempts = MessageWindowDetectAttempts();
    Log(LOG_DEV_DATA, "Module auto puke started, Waiting for current message to be received");
    do
    {   // Check if any new data on the comm bus
        bytesRead = ReadPort(junkData, 1 ,0);
        if((-1 == bytesRead) || (0 == bytesRead))
        {   // No new data, potential window
            windowAttempts--;
        }
        else
        {   // Byte received, reset the window timer
            windowAttempts = MessageWindowDetectTime();
            Log(LOG_DEV_DATA, "Restarting message gap detect time");
        }
        messageGapDetected = (windowAttempts <= 0);
        Log(LOG_DEV_DATA, "Found %d bytes in the buffer", bytesRead);
    } while(!messageGapDetected && maxSendAttempts--);
    // Check if we detected a message gap
    maxSendAttempts = MessageWindowDetectAttempts();
    if(messageGapDetected)
    {   // Message gap detected, look for first byte of new message to get byte count
        Log(LOG_DEV_DATA, "Message gap detected, waiting for start of message to get byte count");
        bytesRead = 0;
        INT32 totalBytesRead = 0;
        INT32 expectedBytes = 9999;
        junkData.clear();   // Clear this out so we can easily find the byte count
        do
        {   // Read data from the port
            bytesRead = ReadPort(junkData, 1, 0);
            Log(LOG_DEV_DATA, "Found %d bytes in the message buffer", bytesRead);
            if((bytesRead != -1) && (expectedBytes == 9999))
            {   // Beginning of message detected, get the number of expected bytes
                expectedBytes = junkData[0] + 2;  // Need to include 2 byte checksum in expected byte count
                totalBytesRead += bytesRead;
                Log(LOG_DEV_DATA, "Start of message detected, expecting %d(0x%02X + 0x02) bytes - received %d bytes so far", 
                    expectedBytes, junkData[0], totalBytesRead);
            }
            else if(bytesRead != -1)
            {   // Total up these bytes
                totalBytesRead += bytesRead;
                Log(LOG_DEV_DATA, "Total bytes received: %d, need %d", totalBytesRead, expectedBytes);
            }
            // Check if all message bytes have been received
            okToSend = (totalBytesRead >= expectedBytes);
        } while(!okToSend && maxSendAttempts--);
        // Check the exit conditions, if ok to send clear the message buffer
        if(okToSend)
        {
            Log(LOG_DEV_DATA, "All message bytes received, resetting connection and sending message");
            ResetConnection();
        }
        else
        {
            Log(LOG_ERRORS, "Error receiving all data bytes - received: %d, expected: %d", totalBytesRead, expectedBytes);
        }
    }
    else
    {   // Could not detect a message gap
        Log(LOG_ERRORS, "Could not find the message gap, abort sending message");
    }
    return okToSend;
}

//-----------------------------------------------------------------------------
inline const UINT32& Keyword82ProtocolFilter::MessageWindowDetectTime(const UINT32 *windowTime) /*= NULL*/
{
    if(windowTime != NULL)  m_messageWindowDetectTime = *windowTime;
    return m_messageWindowDetectTime;
}

//-----------------------------------------------------------------------------
inline const INT32& Keyword82ProtocolFilter::MessageWindowDetectAttempts(const INT32 *windowDetectAttempts) /*= NULL*/
{
    if(windowDetectAttempts != NULL)  m_maxWindowDetectAttempts = *windowDetectAttempts;
    return m_maxWindowDetectAttempts;
}
