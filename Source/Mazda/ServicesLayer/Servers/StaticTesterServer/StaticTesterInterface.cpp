//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/StaticTesterServer/StaticTesterInterface.cpp 2     1/16/07 5:26a Derickso $
//
// FILE DESCRIPTION:
//   The StaticTesterInterface handles the data comms for the static tester.  
//   Build data will be retrieved for the static tester when requested.  Test
//   result data will also be saved and stored from the static tester.  Keep 
//   alive messages will also be sent when required.
//
//===========================================================================
// Copyright (c) 2006- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/StaticTesterServer/StaticTesterInterface.cpp $
// 
// 2     1/16/07 5:26a Derickso
// Added newline character to work with C# app.
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 2     10/16/06 3:08p Derickso
// Updated reconnect logic and added keep alive/heartbeat messages.
// 
// 1     8/30/06 4:42p Derickso
// New static tester server.
// 
//===========================================================================
#include "StaticTesterInterface.h"
#include "ITaskMon.h"
//-----------------------------------------------------------------------------

StaticTesterInterface::StaticTesterInterface(StaticTesterServer *server) : 
AtomicThread(), m_isActive(false), m_useKeepAlive(false)
{
    m_server = server;
    m_server->Log(LOG_DEV_DATA, "StaticTesterInterface: Stored pointer to %s", m_server->GetProcessName().c_str());
}

StaticTesterInterface::~StaticTesterInterface()
{   // Nothing special to be done here
}

void StaticTesterInterface::Initialize(const XmlNode *config)
{   // Initialize the comm object
    m_server->Log(LOG_DEV_DATA, "Initializing serial comms...");
    m_staticTesterComms.Initialize(config);
    // Store the time span allowed between comms to static tester
    m_server->Log(LOG_DEV_DATA, "Storing config data items...");
    INT32 timeSpan;
    try
    {
        timeSpan = BposReadInt(config->getChild("ValidCommTimeSpan")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface: Setting time span to 300s - %s", excpt.GetReason());
        timeSpan = 300;  // Default to 5 minutes
    }
    CommValidTime(&timeSpan);
    // Store the comm port being used by this thread
    string commPort;
    try
    {
        commPort = config->getChild("CommPort")->getValue();
    }
    catch(XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface: Setting comm port to 9 - %s", excpt.GetReason());
        commPort = "9";
    }
    CommPortUsed(&commPort);
    // Store the port register delay
    INT32 registerDelay;
    try
    {
        registerDelay = BposReadInt(config->getChild("PortRegisterDelay")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface: Setting register delay to 500ms - %s", excpt.GetReason());
        registerDelay = 500;
    }
    PortRegisterDelay(&registerDelay);
    // Store the reconnect delay for the port
    INT32 reconnectDelay;
    try 
    {
        reconnectDelay = BposReadInt(config->getChild("PortReconnectDelay")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface: Setting reconnect delay to 1000ms - %s", excpt.GetReason());
        reconnectDelay = 1000;
    }
    ReconnectDelay(&reconnectDelay);
    // Store the static tester response time
    INT32 reponseTime;
    try
    {
        reponseTime = BposReadInt(config->getChild("StaticTesterResponseTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface: Setting response time to 50ms - %s", excpt.GetReason());
        reponseTime = 50;
    }
    StaticTesterResponseTime(&reponseTime);
    // Store the field delimiter
    string delim;
    try
    {
        delim = config->getChild("FieldDelimiter")->getValue();
    }
    catch(XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface: Setting field delimiter to ; - %s", excpt.GetReason());
        delim = ";";
    }
    FieldDelimiter(&delim);
    // Store the tag/value pair delimiter
    try
    {
        delim = config->getChild("TagValueDelimiter")->getValue();
    }
    catch(XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface: Setting tag/value delimiter to : - %s", excpt.GetReason());
        delim = ":";
    }
    TagValueDelim(&delim);
    // Store the flag specifying if keep alive messages should be sent to the static tester
    bool useKeepAlive = false;
    try
    {
        useKeepAlive = atob(config->getChild("SendKeepAlive")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface: Setting UseKeepAlive to false - %s", excpt.GetReason());
        useKeepAlive = false;
    }
    UseKeepAlive(&useKeepAlive);
    // Store the reconnect attempts
    INT32 reconnectAttempts;
    try
    {
        reconnectAttempts = BposReadInt(config->getChild("ReconnectAttempts")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface: Setting reconnect attempts to 120 - %s", excpt.GetReason());
        reconnectAttempts = 120;
    }
    // Store the amount of time to wait between checks for disconnected testers
    INT32 disconnectedTesterCheckTime;
    try
    {
        disconnectedTesterCheckTime = BposReadInt(config->getChild("DisconnectedTesterCheckTime")->getValue().c_str());
    }
    catch (XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface: Setting Disconnected Tester Check Time to 60s - %s", excpt.GetReason());
        disconnectedTesterCheckTime = 60;
    }
    DisconnectedTesterCheckTime(&disconnectedTesterCheckTime);

    m_server->Log(LOG_FN_ENTRY, "StaticTesterInterface::Initialize() - complete");
}

void * StaticTesterInterface::Execute(void)
{   // Wait for comm port to becom active
    m_server->Log(LOG_FN_ENTRY, "StaticTesterInterface:%s - Enter StaticTesterInterface::Execute()", CommPortUsed().c_str());
    WaitForCommPort();
    SetLastCommTime();   // Ensure we do not just jump into attempting to reconnect the port
    bool active = true;
    StaticTesterActive(&active);  // Default to tester active until it proves otherwise
    m_server->Log(LOG_DEV_DATA, "Done waiting for Comm Port");
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Keep going until server is terminated
    while(m_server->GetStatus().compare(BEP_TERMINATE))
    {   // Wait until told to process something
        m_server->m_checkComms.Acquire();
        m_server->m_checkComms.Wait(startCommCheck);
        // Got a signal, start processing
        m_server->m_checkComms.Release();
        if(StaticTesterActive())
        {   // Check for request from static tester
            status = GetStaticTesterRequest();
            m_server->Log(LOG_DETAILED_DATA, "StaticTesterInterface:%s - GetStaticTesterRequest: %s", 
                          CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
            // Reconnect port if time span has elapsed
            if((status == BEP_STATUS_TIMEOUT) && CheckCommsExpired())
            {
                m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Attempting to reconnect the driver", CommPortUsed().c_str());
                status = ReconnectPort();
                m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Reconnect complete: %s", 
                              CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
            }
        }
        else
        {   // Ping for the static tester
            status = ReconnectPort();
        }
    }
    m_server->Log(LOG_FN_ENTRY, "Exit StaticTesterInterface::Execute(void)\n");
    return NULL;
}

const BEP_STATUS_TYPE StaticTesterInterface::SendKeepAlive()
{
    string keepAliveMessage = m_server->GetDataTag("KeepAlive") + FieldDelimiter();
    BEP_STATUS_TYPE status = m_staticTesterComms.Send(keepAliveMessage);
    if(status != BEP_STATUS_SUCCESS)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface:%s - Error sending keep alive to static tester - %s",
                      CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
        status = ReconnectPort();
    }
    // Return the status
    return status;
}

const BEP_STATUS_TYPE StaticTesterInterface::ReconnectPort()
{   // Attempt to reconnect the port if the unit is active
    int reconnectStatus = -1;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if((StaticTesterActive() && CheckCommsExpired()) || AttemptReconnect())
    {
        INT32 attempts = 0;
        while(((reconnectStatus = m_staticTesterComms.ReconnectDriver()) != EOK)  &&
              m_server->GetStatus().compare(BEP_TERMINATE) &&
              (attempts++ < ReconnectAttempts()))
        {
            m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Waiting to reconnect the port...", CommPortUsed().c_str());
            BposSleep(ReconnectDelay());
        }
        // Determine reconnect status
        bool activeStatus = false;
        if(reconnectStatus == EOK)
        {
            activeStatus = true;
            status = BEP_STATUS_SUCCESS;
            SetLastCommTime();   // Ensure we do not attempt to reconnect
        }
        else
        {
            activeStatus = false;
            status = BEP_STATUS_HARDWARE;
        }
        // Decide if the listener should be deactivated
        StaticTesterActive(&activeStatus);
        // Set the time of the last reconnect attempt to now
        SetLastConnectAttemptTime();
        m_server->Log(LOG_ERRORS, "StaticTesterInterface:%s - Reconnected port driver - %s",
                      CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
    }
    else
    {
        status = BEP_STATUS_NA;
    }
    // Return the reconnect status
    return status;
}

inline const bool StaticTesterInterface::AttemptReconnect()
{
    return (difftime(time(NULL), GetLastConnectAttemptTime()) > DisconnectedTesterCheckTime());
}

const BEP_STATUS_TYPE StaticTesterInterface::GetStaticTesterRequest()
{
    SerialString_t   request;
    BEP_STATUS_TYPE  status = BEP_STATUS_ERROR;
    INT32  bytesRead = -1;
    // Get the request from the port
    m_server->Log(LOG_DETAILED_DATA, "StaticTesterInterface:%s - Checking for messages...", CommPortUsed().c_str());
    bytesRead = m_staticTesterComms.ReadPort(request, StaticTesterResponseTime(), m_staticTesterComms.GetTimeoutTime());
    m_server->Log(LOG_DETAILED_DATA, "StaticTesterInterface:%s - Received %d bytes", CommPortUsed().c_str(), bytesRead);
    // If a request was received, process it
    if(bytesRead > 0)
    {   // Update the last comm time
        bool staticTesterActive = true;
        string testerRequest((char *)&request[0], bytesRead);
        m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Received request: %s", 
                      CommPortUsed().c_str(), testerRequest.c_str());
        SetLastCommTime();
        StaticTesterActive(&staticTesterActive);
        // Parse the data to determine reauest type
        string requestType = testerRequest.substr(0, testerRequest.find_first_of(FieldDelimiter()));
        m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Processing request type: %s",
                      CommPortUsed().c_str(), requestType.c_str());
        // Have the server process the data
        INT32  vinStartIndex = testerRequest.find_first_of(FieldDelimiter()) + 1;
        if(!requestType.compare(m_server->GetDataTag("VinRequest")))
        {   // Process vin request
            INT32  vinLength = testerRequest.length() - vinStartIndex;
            string vin(testerRequest.substr(vinStartIndex, vinLength));
            m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Requesting build data for %s",
                          CommPortUsed().c_str(), vin.c_str());
            RequestBuildData(vin);
            status = BEP_STATUS_SUCCESS;
        }
        else if(!requestType.compare(m_server->GetDataTag("AonRequest")))
        {   // Process aon request
            m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Retrieving AON number",
                          CommPortUsed().c_str(), vin.c_str());
            ProcessAonRequest();
            status = BEP_STATUS_SUCCESS;
        }
        else if(!requestType.compare(m_server->GetDataTag("TestResult")))
        {   // Send ACK back to the tester
            string ackMessage(m_server->GetDataTag("TestResult") + FieldDelimiter() + BEP_SUCCESS_RESPONSE + char(0x00));
            status = m_staticTesterComms.Send(ackMessage);
            if(status != BEP_STATUS_SUCCESS)
            {
                m_server->Log(LOG_ERRORS, "StaticTesterInterface:%s - Error sending test result response to static tester - %s",
                              CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
            }
            // Store the test results
            INT32  vinLength = testerRequest.find_first_of(FieldDelimiter(), vinStartIndex) - vinStartIndex;
            string vin(testerRequest.substr(vinStartIndex, vinLength));
            string testResultData(testerRequest.substr(vinStartIndex + vinLength));
            m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Storing test result data: %s",
                          CommPortUsed().c_str(), testResultData.c_str());
            StoreTestResults(vin, testResultData);
        }
        else if(!requestType.compare(m_server->GetDataTag("Heartbeat")))
        {   // Send heartbeat ACK back to the tester
            string ackMessage(m_server->GetDataTag("Heartbeat") + FieldDelimiter() + BEP_SUCCESS_RESPONSE + char(0x00) + "\n");
            status = m_staticTesterComms.Send(ackMessage);
            if(status != BEP_STATUS_SUCCESS)
            {
                m_server->Log(LOG_ERRORS, "StaticTesterInterface:%s - Error sending heartbeat response to static tester - %s",
                              CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
            } 
        }
    }
    else
    {
        status = BEP_STATUS_TIMEOUT;
    }
    return status;
}

inline const bool StaticTesterInterface::CheckCommsExpired(void)
{
    return(difftime(time(NULL), GetLastCommTime()) > CommValidTime());
}

void StaticTesterInterface::RequestBuildData(const string &vin)
{
    XmlNode buildData(m_server->GetDataTag("BuildData"), "");
    string status = m_server->ReadBuildData(vin, buildData);
    string buildDataResponse;
    buildDataResponse.reserve(2048);  // Reserve a 2k area for the response
    buildDataResponse = m_server->GetDataTag("BuildData");
    // Check if we got a decent response
    if(!status.compare(BEP_SUCCESS_RESPONSE))
    {   // Good data, format for static tester
        try
        {
            const XmlNodeMap *buildMap = &buildData.getChild(m_server->GetDataTag("VehicleBuild"))->getChildren();
            for(XmlNodeMapCItr iter = buildMap->begin(); iter != buildMap->end(); iter++)
            {
                buildDataResponse += (FieldDelimiter() + iter->second->getName() + TagValueDelim() + iter->second->getValue());
            }
        }
        catch(XmlException &excpt)
        {
            m_server->Log(LOG_ERRORS, "StaticTesterInterface:%s - Build Data not in correct format: %s", 
                          CommPortUsed().c_str(), excpt.GetReason());
            buildDataResponse += (FieldDelimiter() + BEP_ERROR_RESPONSE);
        }
        catch(BepException &excpt)
        {
            m_server->Log(LOG_ERRORS, "StaticTesterInterface:%s - Build Data not in correct format: %s", 
                          CommPortUsed().c_str(), excpt.GetReason());
            buildDataResponse += (FieldDelimiter() + BEP_ERROR_RESPONSE);
        }
        catch(...)
        {
            m_server->Log(LOG_ERRORS, "StaticTesterInterface:%s - Build Data not in correct format", 
                          CommPortUsed().c_str());
            buildDataResponse += (FieldDelimiter() + BEP_ERROR_RESPONSE);
        }
    }
    else
    {   // Could not retrieve build data, construct response
        m_server->Log(LOG_ERRORS, "StaticTesterInterface:%s - Error getting build data from WCC for %s - %s", 
                      CommPortUsed().c_str(), vin.c_str(), status.c_str());
        buildDataResponse += (FieldDelimiter() + status);
    }
    // Add a NULL character to the end of the string
    buildDataResponse += char(0x00);
    // Send the response to the static tester
    m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Sending response to static tester: %s",
                  CommPortUsed().c_str(), buildDataResponse.c_str());
    BEP_STATUS_TYPE txStatus = m_staticTesterComms.Send(buildDataResponse);
    if(txStatus != BEP_STATUS_SUCCESS)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface:%s - Error sending build data to static tester - %s",
                      CommPortUsed().c_str(), ConvertStatusToResponse(txStatus).c_str());
    }
}

void StaticTesterInterface::ProcessAonRequest(void)
{
    XmlNode aonNumber(m_server->GetDataTag("AonNumber"), "");
    string aonResponse = m_server->GetDataTag("NextAon");
    aonResponse += (FieldDelimiter() + m_server->Read(aonNumber));
    // Add a NULL character to the end of the string
    aonResponse += char(0x00);
    // Send the response to the static tester
    m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Sending response to static tester: %s",
                  CommPortUsed().c_str(), aonResponse.c_str());
    BEP_STATUS_TYPE txStatus = m_staticTesterComms.Send(aonResponse);
    if(txStatus != BEP_STATUS_SUCCESS)
    {
        m_server->Log(LOG_ERRORS, "StaticTesterInterface:%s - Error sending build data to static tester - %s",
                      CommPortUsed().c_str(), ConvertStatusToResponse(txStatus).c_str());
    }
}

void StaticTesterInterface::StoreTestResults(const string &vin, const string &testResults)
{
    XmlNode results(m_server->GetDataTag("TestResult"), vin);
    char *currentField = NULL;
    vector<string> resultFields;
    // Store the fields to process
    currentField = strtok((char *)&testResults[0], FieldDelimiter().c_str());
    while(currentField != NULL)
    {   // Store the strings for later processing
        resultFields.push_back(string(currentField));
        m_server->Log(LOG_DEV_DATA, "Stored field to process: %s", resultFields.back().c_str());
        currentField = strtok(NULL, FieldDelimiter().c_str());
    }
    // Process each of the fields
    while(!resultFields.empty())
    {
        XmlNode *resultNode;
        m_server->Log(LOG_DEV_DATA, "Processing test result field: %s", resultFields.back().c_str());
        resultNode = ProcessResultField(resultFields.back());
        // Add the current test result node to the overall results
        m_server->Log(LOG_DEV_DATA, "Adding new test result node: %s", resultNode->ToString().c_str());
        results.addChild(resultNode);
        m_server->Log(LOG_DEV_DATA, "Finished adding new test result node: %s", results.ToString().c_str());
        // Done processing this fieldremove it from the vector
        resultFields.pop_back();
    }
    // Command the server to store the test result
    m_server->Write(&results);
    // Clean up the node to prevent a memory leak
    results.clear();
}

XmlNode* StaticTesterInterface::ProcessResultField(string &resultField)
{
    XmlNode resultNode("", "");
    // Get the test name
    m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Processing test result field: %s", 
                  CommPortUsed().c_str(), resultField.c_str());
    char *resultItem = strtok((char *)&resultField[0], TagValueDelim().c_str());
    string testResultName = TranslateTestResultTag(resultItem);
    resultNode.setName(testResultName);
    m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Set test result name: %s",
                  CommPortUsed().c_str(), testResultName.c_str());
    // Add the test result
    resultItem = strtok(NULL, TagValueDelim().c_str());
    resultNode.addChild(BEP_RESULT, resultItem, ATTRIBUTE_NODE);
    m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Set test result attribute: %s",
                  CommPortUsed().c_str(), resultItem);
    // Add any details to the result
    char *detailName  = strtok(NULL, TagValueDelim().c_str());
    char *detailValue = strtok(NULL, TagValueDelim().c_str());
    while((detailName != NULL) && (detailValue != NULL))
    {   // Add the detail data to the result node
        string name = TranslateTestResultTag(detailName);
        XmlNode detailNode(name, detailValue);
        resultNode.addChild(detailNode.Copy());
        m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Added test result detail: %s - %s",
                      CommPortUsed().c_str(), name.c_str(), detailValue);
        // Get the next fields
        detailName  = strtok(NULL, TagValueDelim().c_str());
        detailValue = strtok(NULL, TagValueDelim().c_str());
    }
    m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Processing test result field complete", 
                  CommPortUsed().c_str());
    return resultNode.Copy();
}

const bool& StaticTesterInterface::StaticTesterActive(const bool *active) /*= NULL*/
{
    // If a value was supplied, update our state
    if(active != NULL)  m_isActive = *active;
    return m_isActive;
}

const INT32& StaticTesterInterface::ReconnectDelay(const INT32 *reconnectDelay) /*= NULL*/
{
    if(reconnectDelay != NULL)  m_reconnectDelay = *reconnectDelay;
    return m_reconnectDelay;
}

const INT32& StaticTesterInterface::ReconnectAttempts(const INT32 *attempts /*= NULL*/)
{
    if(attempts != NULL)  m_reconnectAttempts = *attempts;
    return m_reconnectAttempts;
}

inline void StaticTesterInterface::SetLastCommTime(void)
{
    m_lastCommTime = time(NULL);
}

inline const time_t& StaticTesterInterface::GetLastCommTime(void)
{
    return m_lastCommTime;
}

inline void StaticTesterInterface::SetLastConnectAttemptTime(void)
{
    m_lastConnectAttemptTime = time(NULL);
}

inline const time_t& StaticTesterInterface::GetLastConnectAttemptTime(void)
{
    return m_lastConnectAttemptTime;
}

const INT32& StaticTesterInterface::CommValidTime(const INT32 *validTime) /*= NULL*/
{
    if(validTime != NULL)  m_validCommTimeSpan = *validTime;
    return m_validCommTimeSpan;
}

void StaticTesterInterface::WaitForCommPort(void)
{
    TaskMonTask_t commPort;
    do
    {   // Wait for the static tester comm port
        ITaskMon::ReadTaskInfo(commPort, CommPortUsed().c_str(), TASKMON_CLASS_SERIAL);
        m_server->Log(LOG_DEV_DATA, "StaticTesterInterface:%s - Waiting for comm port - state: %d [waiting for %d]",
                      CommPortUsed().c_str(), commPort.m_taskState, TASKSTATE_RUN);
        // If the state is not run, register the port driver
        if(commPort.m_taskState != TASKSTATE_RUN)
        {   // Wait a bit before attempting to connect/reconnect
            BposSleep(PortRegisterDelay());
            // Register the port driver
            m_staticTesterComms.RegisterPortDriver();
            // Wait a bit to allow port driver to finish registering
            BposSleep(PortRegisterDelay());
        }
        // Keep checking until state is run and not terminated
    } while( (commPort.m_taskState != TASKSTATE_RUN) && (m_server->GetStatus().compare(BEP_TERMINATE)) );
}

const string& StaticTesterInterface::CommPortUsed(const string *commPort)
{
    if(commPort != NULL)  m_commPort = *commPort;
    return m_commPort;
}

const INT32& StaticTesterInterface::PortRegisterDelay(const INT32 *registerDelay) /*= NULL*/
{
    if(registerDelay != NULL)  m_portRegisterDelay = *registerDelay;
    return m_portRegisterDelay;
}

const INT32& StaticTesterInterface::StaticTesterResponseTime(const INT32 *responseTime) /*= NULL*/
{
    if(responseTime != NULL)  m_staticTesterResponseTime = *responseTime;
    return m_staticTesterResponseTime;
}

const string& StaticTesterInterface::FieldDelimiter(const string *delim) /*= NULL*/
{
    if(delim != NULL)  m_fieldDelim = *delim;
    return m_fieldDelim;
}

const string& StaticTesterInterface::TagValueDelim(const string *delim) /*= NULL*/
{
    if(delim != NULL)  m_tagValueDelim = *delim;
    return m_tagValueDelim;
}

inline const string StaticTesterInterface::TranslateTestResultTag(const char *tag)
{
    string staticTesterTag(tag);
    return m_server->GetDataTag(staticTesterTag);
}

inline const bool& StaticTesterInterface::UseKeepAlive(bool *useKeepAlive /*= NULL*/)
{
    if(useKeepAlive != NULL)  m_useKeepAlive = *useKeepAlive;
    return m_useKeepAlive;
}

inline const INT32& StaticTesterInterface::DisconnectedTesterCheckTime(INT32 *checkTime /*= NULL*/)
{
    if(checkTime != NULL)  m_disconnectedTesterCheckTimeSpan = *checkTime;
    return m_disconnectedTesterCheckTimeSpan;
}
