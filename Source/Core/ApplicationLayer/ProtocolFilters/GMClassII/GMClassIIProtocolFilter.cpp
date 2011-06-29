//*************************************************************************
// FILE:
//    $Header: ApplicationLayer/ProtocolFilters/GMClassII/GMClassIIProtocolFilter.cpp 
//
// FILE DESCRIPTION:
//  GMClassII Protocol Filter for communicating with vehicle modules.
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
// Created.  Initial files from J2190.
//
//*************************************************************************
#include "GMClassIIProtocolFilter.h"

GMClassIIProtocolFilter::GMClassIIProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar, BepMutex *commsInUse) /*= NULL*/ : ProtocolFilter(lastTxTime, stopCommsBepCondVar, commsInUse),
m_enterDiagModeCode(0x80), m_responsePendingCode(0x78), m_responsePendingReads(3),
m_moduleRequestID(0x00), m_moduleRequestCode(0x24), m_moduleResponseCode(0x26),
m_autoEnterDiagMode(false), m_enterDiagnosticModeMessageTag("EnterDiagnosticMode"), 
m_j1850PriorityCode(0x0C), m_testerRequestID(0xF1), m_dataByteCountIndex(3)
{   // Nothing special to do here
}

GMClassIIProtocolFilter::~GMClassIIProtocolFilter()
{   // Nothing special to do here
}

bool GMClassIIProtocolFilter::Initialize(const XmlNode *config)
{   // Get the negative response code for module not in diagnostic mode
    // Store the module diagnostic request message code
    try
    {
        SetModuleRequestCode(atoh(config->getChild("Setup/DiagnosticRequestCode")->getValue().c_str()));
    }
    catch(XmlException &err)
    {

        Log(LOG_ERRORS, ": CANNOT SEND MESSAGES TO MODULE WITHOUT A DIAGNOSTIC REQUEST CODE!!!!!\n");
        throw;
    }

    // Store the module diagnostic reponse message code
    try
    {
        SetModuleResponseCode(atoh(config->getChild("Setup/DiagnosticResponseCode")->getValue().c_str()));
    }
    catch(XmlException &err)
    {
        Log(LOG_ERRORS, ": CANNOT SEND MESSAGES TO MODULE WITHOUT A DIAGNOSTIC RESPONSE CODE!!!!!\n");
        throw;
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
    try
    {
        SetJ1850PriorityCode(atoh(config->getChild("Setup/J1850PriorityCode")->getValue().c_str()));
    }
    catch(XmlException &err)
    {
        SetJ1850PriorityCode(0x0C);
    }
    try
    {
        SetTesterRequestID(atoh(config->getChild("Setup/TesterRequestID")->getValue().c_str()));
    }
    catch(XmlException &err)
    {
        SetTesterRequestID(0x0C);
    }

    // Store the data byte count index
    try
    {
        SetDataByteCountIndex(BposReadInt(config->getChild("Setup/DataByteCountIndex")->getValue().c_str()));
    }
    catch(XmlException &err)
    {
        Log(LOG_ERRORS, "Data byte count index not specified, using 6\n");
        SetDataByteCountIndex(3);
    }
    // Get the negative response code for module not in diagnostic mode
    try
    {   // Get the code being used for module not in diagnostic mode
        SetEnterDiagnosticModeCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/EnterDiagnosticMode")->getValue().c_str()));
    }
    catch(XmlException &err)
    {
        SetEnterDiagnosticModeCode(0x80);
    }
    try
    {   // Get the code being used for response pending from the module
        SetResponsePendingCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/ResponsePending")->getValue().c_str()));
    }
    catch(XmlException &err)
    {
        SetResponsePendingCode(0x78);
    }
    try
    {   // Get the code being used for module busy, repeat request
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
    // Call the base class to complete the initialization
    return ProtocolFilter::Initialize(config);
}

const BEP_STATUS_TYPE GMClassIIProtocolFilter::SendMessage(SerialString_t &message)
{
    return SendMessage(message, true);
}

const BEP_STATUS_TYPE GMClassIIProtocolFilter::SendMessage(SerialString_t &message, const bool addModuleID)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Check to ensure the binary message to be sent has enough bytes to
    //   ensure we don't sig seg when we access the array to
    //   set the SentMessageIdentifier
    if(GetSentMessageIdentifierIndex() < message.length())
    {   // Set the message ID
        SetSentMessageIdentifier(message[GetSentMessageIdentifierIndex()]);

        Log(LOG_DETAILED_DATA, "GMClassIIProtocolFilter::SendMessage() - SetSentMessageIdentifier - DataStartIndex: %d MessageIdentifier: $%02X\n", GetDataStartIndex(), GetSentMessageIdentifier());

        // Check if the ID should be added to the message
        if(addModuleID)
        {   // Add the module ID to the message
            message = GetJ1850PriorityCode() + GetModuleRequestID() + GetTesterRequestID() + message;
        }
        else
        {
            Log(LOG_DETAILED_DATA, "Not adding message ID or module ID to message.");
        }
        // Clear the Fifos so bad data is not gathered
        Log(LOG_DETAILED_DATA, "GMClassIIProtocolFilter::SendMessage() -Resetting The Connection\n");
        ResetConnection();
        Log(LOG_DETAILED_DATA, "GMClassIIProtocolFilter::SendMessage() -Reset The Connection\n");
        // Send the message and return the result
        PrintSerialString( "GMClassIIProtocolFilter::SendMessage() -ProtocolFilter Sending", message);
        INT32 bytesSent = ILogicalPort::WritePort(message, GetNumberOfRetries());
        Log(LOG_DETAILED_DATA, "GMClassIIProtocolFilter::SendMessage() -Sent the message\n");
        // Determine the result of the write
        if(bytesSent < 0)       status = BEP_STATUS_ERROR;
        else if(bytesSent == 0) status = BEP_STATUS_FAILURE;
        else if(bytesSent > 0)  status = BEP_STATUS_SUCCESS;
    }
    else
    {
        Log(LOG_ERRORS, "ERROR Module Message length shorter than Identifier Index "
            "Message Length: %d\n\tIdentifier Index: %d\n", message.length(),
            GetSentMessageIdentifierIndex());
        status = BEP_STATUS_ERROR;
    }
    // Return the status
    return status;
}

const BEP_STATUS_TYPE GMClassIIProtocolFilter::SendBusBroadcastMessage(string messageTag)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    std::string asciiMessage;
    SerialString_t xmtMessage;
    // Get the message to be sent
    if(GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
    {   // Convert the message to binary
        GetBinaryMssg(asciiMessage, xmtMessage);
        // Send the message
        status = SendMessage(xmtMessage, false);
    }
    else
    {   // No message provided for this tag
        Log(LOG_ERRORS,"No message provided for tag: %s\n", messageTag.c_str());
        status = BEP_STATUS_SOFTWARE;
    }
    // Return the status
    return status;
}

const BEP_STATUS_TYPE GMClassIIProtocolFilter::SendMessage(std::string messageTag)
{   // Call the base class
    return ProtocolFilter::SendMessage(messageTag);
}

const BEP_STATUS_TYPE GMClassIIProtocolFilter::SendMessage(std::string messageTag, SerialArgs_t &args)
{   // Call the base class
    return ProtocolFilter::SendMessage(messageTag, args);
}

const BEP_STATUS_TYPE GMClassIIProtocolFilter::GetModuleData(std::string messageTag, SerialString_t &reply, SerialArgs_t *args) /*= NULL*/
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string asciiMessage;
    SerialString_t xmtMessage;
    SerialString_t moduleResponse;
    INT32 tries = 0;
    //use message specific retries if specified
    GetResponseFailureRetryCount(messageTag, tries);
    tries = (tries < 0) ? GetNumberOfRetries() : tries;
    if(GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
    {
        do
        {   // Attempt to lock the port for our own use, allow other processes, but not other threads in our process
            if((errno = m_commsInUse->Acquire()) == EOK)
            {   // Send the message to the module
                if(args == NULL) status = SendMessage(messageTag);
                else                  status = SendMessage(messageTag, *args);
                Log(LOG_DEV_DATA, "Sent message: %s to module - status: %s\n", messageTag.c_str(),
                    ConvertStatusToResponse(status).c_str());
                if(IsResponseExpected(messageTag))
                {
                    // Clear the response area
                    moduleResponse.erase();
                    // If send was successful, get the module response
                    status = GetResponse(moduleResponse);
                    Log(LOG_DEV_DATA, "GMClassIIProtocolFilter::GetResponse() returned %s [response length: %d]",
                        ConvertStatusToResponse(status).c_str(), moduleResponse.length());
                    // Extract the module response from the message
                    if((status == BEP_STATUS_SUCCESS) && (moduleResponse.length() > 0))
                    {
                        reply = ExtractModuleData(moduleResponse);
                        // Make sure the response matches a filter before processing it
                        if(IsResponseValid(messageTag, reply))
                        {   // Check for a valid response if we got the response
                            Log(LOG_DEV_DATA, "Response for message %s is valid", messageTag.c_str());
                            if((BEP_STATUS_SUCCESS == status) && (reply.length() > 0)) status = CheckForValidResponse(reply);
                            // Check for a negative response from the module
                            if((BEP_STATUS_SUCCESS != status) && (reply.length() > 0)) status = CheckForNegativeResponse(reply);
                            // Check if it was a negaitve response
                            if(((BEP_STATUS_FAILURE == status) || (BEP_STATUS_SUCCESS != status)) &&
                               (reply.length() > GetNegativeResponseCodeIndex()))
                            {   // Check what the negative response was
                                UINT8 negativeResponseCode = reply[GetNegativeResponseCodeIndex()];
                                Log(LOG_DEV_DATA, "Current Negative Response Code: $%02X -- Module Busy Code: $%02X\n",
                                    negativeResponseCode, GetModuleBusyCode());
                                // Handle the negative response
                                if((GetEnterDiagnosticModeCode() == negativeResponseCode) && AutomaticallyEnterDiagnosticMode())
                                {   // Need to enter diagnostic mode first
                                    SendMessage(GetEnterDiagnosticModeMessageTag());
                                    GetResponse(GetEnterDiagnosticModeMessageTag(), reply);
                                }
                                else if(GetModuleBusyCode() == negativeResponseCode)
                                {   // Module busy, repeating request.
                                    // NOTE:  We should not decrement our count while the module is busy
                                    status = BEP_STATUS_NA;
                                    tries++;
                                    BposSleep(GetResponseEndDelay());   // Delay before sending again
                                }
                                else
                                {   // Negative response with no special treatment, stop all attempts
                                    status = BEP_STATUS_FAILURE;
                                    tries = 0;
                                    Log(LOG_ERRORS, "No special handling for negative response code %02X", negativeResponseCode);
                                }
                            }
                        }
                        else
                        {   // Not a response we were looking for
                            Log(LOG_DEV_DATA, "Response for %s does not match any filter - discarding\n", messageTag.c_str());
                            status = BEP_STATUS_NA;
                        }
                    }
                    else
                    {
                        Log(LOG_ERRORS, "Not extracting module data from response - status: %s, message size: %d",
                            ConvertStatusToResponse(status).c_str(), moduleResponse.length());
                    }
                }
                else
                {
                    // If response NOT expected, assume success, exit do-while loop
                    status = BEP_STATUS_SUCCESS;
                    Log(LOG_DEV_DATA, "No response expected, returning SUCCESS to exit retry loop");
                }
                // Exit the critical section since we are done with the port for now
                m_commsInUse->Release();
            }
            else
            {   // Error aquiring the semaphore
                Log(LOG_ERRORS, "Could not Acquire mutex! Message %s not sent! - %s\n", messageTag.c_str(), strerror(errno));
            }
            // Keep trying until a good response s received or we run out of attempts
        } while((BEP_STATUS_SUCCESS != status) && (tries-- > 0) && !GetStopCommsFlag());

        if(IsResponseExpected(messageTag))
        {
            // Check if valid message was retrieved
            if(status != BEP_STATUS_SUCCESS)
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
            // If response NOT expected, assume success, exit do-while loop
            status = BEP_STATUS_SUCCESS;
            Log(LOG_DEV_DATA, "No response expected, returning SUCCESS from GetModuleData()");
        }
    }
    else
    {   // No message provided for this tag
        Log(LOG_ERRORS, "No message provided for tag: %s\n", messageTag.c_str());
        status = BEP_STATUS_SOFTWARE;
    }
    Log(LOG_DEV_DATA, "GetModuleData() returning - status: %s\n", ConvertStatusToResponse(status).c_str());
    return status;
}

const BEP_STATUS_TYPE GMClassIIProtocolFilter::GetBusBroadcastMessage(string messageTag,
                                                                      const long messageWaitTime,
                                                                      SerialString_t &busMssg)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    INT32 replyBytes = 0;
    NotifyEvent_t   mssgEvent;
    struct timespec currentTime;
    struct timespec startTime;
    Log(LOG_FN_ENTRY, "Enter GetBusBroadcastMsg(%s)", messageTag.c_str());
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
    Log(LOG_FN_ENTRY, "Exiting GetBusBroadcastMsg(%s) - status: %s", messageTag.c_str(), ConvertStatusToResponse(status).c_str());
    return status;
}

const SerialString_t GMClassIIProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{
    string fullResponse;
    // Log the respnse received from the module
    char temp[256];
    for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
        fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
    Log(LOG_DETAILED_DATA, "GMClassIIProtocolFilter::ExtractModuleData - Extracting data from response string: %s\n", fullResponse.c_str());


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
        Log(LOG_DEV_DATA, "GMClassIIProtocolFilter::ExtractModuleData - Extracted module data: %s\n", fullResponse.c_str());
        // Check if multiple messages are in the module
        Log(LOG_DETAILED_DATA, "GMClassIIProtocolFilter::ExtractModuleData - Total Response Length: %d, Current Message Length: %d, Number of Bytes: %d\n", 
            moduleResponse.length(), (GetDataByteCountIndex() + numberOfBytes + 1), numberOfBytes);

        if((UINT16)moduleResponse.length() > (GetDataByteCountIndex() + numberOfBytes + 1))
        {   // Multiple messages in the response, update to point at next message
            SerialString_t responseMatchHeader = moduleResponse.substr(0, GetDataByteCountIndex() + 1 + GetDataHeaderLength());
            PrintSerialString("GMClassIIProtocolFilter::ExtractModuleData - ResponseMatchHeader ", responseMatchHeader);
            bool keepTakingFromQueue = true;

            while(keepTakingFromQueue)
            {
                moduleResponse = moduleResponse.substr(GetDataByteCountIndex() + numberOfBytes + 1);
                // Log the data remaining in the buffer
                fullResponse.erase();
                for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
                    fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
                Log(LOG_DETAILED_DATA, "GMClassIIProtocolFilter::ExtractModuleData - Module data remaining in buffer: %s\n", fullResponse.c_str());

                if(moduleResponse.compare(0, responseMatchHeader.length(), responseMatchHeader) == 0)
                {

                    //I found a nother match to my message
                    Log(LOG_DETAILED_DATA, "GMClassIIProtocolFilter::ExtractModuleData - Appending More Data From Multiple Messages");
                    dataResponse = dataResponse + moduleResponse.substr(dataStartIdx+1, dataRespSz-1);  //little adding and subtracting to get rid of repsonse code
                }
                else
                {
                    Log(LOG_DETAILED_DATA, "GMClassIIProtocolFilter::ExtractModuleData - No More Matching Messages");
                    keepTakingFromQueue = false;
                }

                PrintSerialString("GMClassIIProtocolFilter::ExtractModuleData - ?dataResponse ", dataResponse);
            }            


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
        Log( LOG_ERRORS, "GMClassIIProtocolFilter::ExtractModuleData - Incomplete message in ExtractModuleData: <%s>\n", fullResponse.c_str());
        fullResponse.clear();
    }
    // Return the extracted data	
    return dataResponse;       
}
/*
const SerialString_t GMClassIIProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{
    string fullResponse;
    // Log the response received from the module
    char temp[256];
    for (UINT16 ii = 0; ii < moduleResponse.size(); ii++)
    {
        fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
    }
    Log(LOG_DETAILED_DATA, "Extracting data from response string: %s\n", fullResponse.c_str());
    // Module response starts immediately after the module response ID
    SerialString_t dataResponse = moduleResponse.substr(GetDataStartIndex());
    // Log the data extracted from the module
    fullResponse.erase();
    for (UINT16 ii = 0; ii < dataResponse.length(); ii++)
        fullResponse += CreateMessage(temp, 256, "$%02X ", dataResponse[ii]);
    Log(LOG_DEV_DATA, "Extracted module data: %s\n", fullResponse.c_str());
    // Return the extracted data
    return dataResponse;
}
*/
int GMClassIIProtocolFilter::WaitForFullResponse( SerialString_t &response)
{
    int         bytesRead, retVal = 0;
    // Try to read the module response
    if( (bytesRead=ReadPort(response, (long)GetResponseDelay(), (long)GetResponseEndDelay())) > 0)
    {   // All data received
        Log(LOG_DEV_DATA, "Received %d bytes of data\n", bytesRead);
        retVal = bytesRead;
    }
    // No data Available
    else if( bytesRead == 0)
    {   // No data available..so leave
        Log(LOG_DEV_DATA, "No Data Available from module\n", bytesRead);
        retVal = 0;
    }
    // Error reading byte
    else if( errno != ETIMEDOUT)
    {
        Log(LOG_DEV_DATA, "ReadPort Error: %s, %d bytes read\n", strerror( errno), bytesRead);
        retVal = 0;
    }
    else
    {   // No more data available..so leave
        Log(LOG_DEV_DATA, "Timeout waiting for module response\n");
        retVal = 0;
    }
    // Return the results
    return( retVal);
}

inline const UINT8& GMClassIIProtocolFilter::GetEnterDiagnosticModeCode(void)
{   // Return the negative response code for module not in diag mode
    return m_enterDiagModeCode;
}

inline const UINT8& GMClassIIProtocolFilter::GetResponsePendingCode(void)
{   // Return the code for response pending
    return m_responsePendingCode;
}

inline const UINT8& GMClassIIProtocolFilter::GetModuleBusyCode(void)
{   // Return the code for module busy, repeat request
    return m_moduleBusyCode;
}

inline const string& GMClassIIProtocolFilter::GetEnterDiagnosticModeMessageTag(void)
{   // Return the tag for the enter diagnostic mode message
    return m_enterDiagnosticModeMessageTag;
}

inline const INT32& GMClassIIProtocolFilter::GetNumberOfResponsePendingReads(void)
{
    return m_responsePendingReads;
}

inline const bool& GMClassIIProtocolFilter::AutomaticallyEnterDiagnosticMode(void)
{
    return m_autoEnterDiagMode;
}

inline const SerialString_t GMClassIIProtocolFilter::GetTesterRequestID(void)
{
    SerialString_t testerRequestID;
    testerRequestID.push_back((uint8_t)m_testerRequestID);
    return testerRequestID;
}
inline const SerialString_t GMClassIIProtocolFilter::GetJ1850PriorityCode(void)
{
    SerialString_t j1850PriorityCode;
    j1850PriorityCode.push_back((uint8_t)m_j1850PriorityCode);
    return j1850PriorityCode;
}
inline const SerialString_t GMClassIIProtocolFilter::GetModuleRequestCode(void)
{
    SerialString_t moduleRequestCode;
    moduleRequestCode.push_back((uint8_t)m_moduleRequestCode);
    return moduleRequestCode;
}

inline const SerialString_t GMClassIIProtocolFilter::GetModuleResponseCode(void)
{
    SerialString_t moduleResponseCode;
    moduleResponseCode.push_back((uint8_t)m_moduleResponseCode);
    return moduleResponseCode;
}

inline const SerialString_t GMClassIIProtocolFilter::GetModuleRequestID(void)
{
    SerialString_t moduleRequestID;
    moduleRequestID.push_back((uint8_t)m_moduleRequestID);
    return moduleRequestID;
}


inline const INT32& GMClassIIProtocolFilter::GetDataByteCountIndex(void)
{
    return m_dataByteCountIndex;
}

inline void GMClassIIProtocolFilter::SetEnterDiagnosticModeCode(const UINT8 &enterDiagnosticModeCode)
{   // Save the enter diagnostic mode code
    m_enterDiagModeCode = enterDiagnosticModeCode;
}

inline void GMClassIIProtocolFilter::SetResponsePendingCode(const UINT8 &responsePendingCode)
{   // Save the response pending code
    m_responsePendingCode = responsePendingCode;
}

inline void GMClassIIProtocolFilter::SetModuleBusyCode(const UINT8 &moduleBusyCode)
{   // Save the module busy, repeat request code
    m_moduleBusyCode = moduleBusyCode;
}

inline void GMClassIIProtocolFilter::SetEnterDiagnosticModeMessageTag(const string& tag)
{   // Save the enter diagnostic mode message tag
    m_enterDiagnosticModeMessageTag = tag;
}

inline void GMClassIIProtocolFilter::SetAutomaticallyEnterDiagnosticMode(const bool& autoEnterDiagMode)
{
    m_autoEnterDiagMode = autoEnterDiagMode;
}

inline void GMClassIIProtocolFilter::SetResponsePendingReads(const INT32& responsePendingReads)
{
    m_responsePendingReads = responsePendingReads;
}

inline void GMClassIIProtocolFilter::SetModuleRequestCode(const UINT8& moduleRequestCode)
{
    m_moduleRequestCode = moduleRequestCode;
}

inline void GMClassIIProtocolFilter::SetModuleResponseCode(const UINT8& moduleResponseCode)
{
    m_moduleResponseCode = moduleResponseCode;
}

inline void GMClassIIProtocolFilter::SetModuleRequestID(const UINT8& moduleID)
{
    m_moduleRequestID = moduleID;
}

inline void GMClassIIProtocolFilter::SetJ1850PriorityCode(const UINT8& priorityCode)
{
    m_j1850PriorityCode = priorityCode;
}

inline void GMClassIIProtocolFilter::SetTesterRequestID(const UINT8& testerID)
{
    m_testerRequestID = testerID;
}

inline void GMClassIIProtocolFilter::SetDataByteCountIndex(const INT32& dataByteCountindex)
{
    m_dataByteCountIndex = dataByteCountindex;
}
