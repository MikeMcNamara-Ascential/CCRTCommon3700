//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/Core/ServicesLayer/Servers/InterCcrtCommServer/InterCcrtCommInterface.cpp 
//
// FILE DESCRIPTION:
//   The InterCcrtCommInterface handles the data comms between ccrt and ccrt / ccts systems. 
// 
//   Build data will be sent when requested and saved and stored when received.
//  
//   Test result data will be sent when requested and saved and stored when received.  
//
//   Heartbeat messages will be sent when required.
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
#include "InterCcrtCommInterface.h"
#include "ITaskMon.h"
//-----------------------------------------------------------------------------

InterCcrtCommInterface::InterCcrtCommInterface(InterCcrtCommServer *server) : 
AtomicThread(), m_isActive(false), m_useKeepAlive(false),m_partialMessageBuffer("")
{
    m_server = server;
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface: Stored pointer to %s", m_server->GetProcessName().c_str());
}

InterCcrtCommInterface::~InterCcrtCommInterface()
{   // Nothing special to be done here
    m_keepAliveTimer.Stop();
}

void InterCcrtCommInterface::Initialize(const XmlNode *config)
{   // Initialize the comm object
    m_server->Log(LOG_DEV_DATA, "Initializing serial comms...");
    m_interCcrtComm.Initialize(config);
    // Store the time span allowed between comms to endpoint
    m_server->Log(LOG_DEV_DATA, "Storing config data items...");
    // Store the comm port being used by this thread
    string commPort;
    try
    {
        commPort = config->getChild("CommPort")->getValue();
    }
    catch (XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface: Setting comm port to 9 - %s", excpt.GetReason());
        commPort = "9";
    }
    CommPortUsed(&commPort);
    // Store the port register delay
    INT32 registerDelay;
    try
    {
        registerDelay = BposReadInt(config->getChild("PortRegisterDelay")->getValue().c_str());
    }
    catch (XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface: Setting register delay to 500ms - %s", excpt.GetReason());
        registerDelay = 500;
    }
    PortRegisterDelay(&registerDelay);
    // Store the reconnect delay for the port
    INT32 reconnectDelay;
    try
    {
        reconnectDelay = BposReadInt(config->getChild("PortReconnectDelay")->getValue().c_str());
    }
    catch (XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface: Setting reconnect delay to 1000ms - %s", excpt.GetReason());
        reconnectDelay = 1000;
    }
    ReconnectDelay(&reconnectDelay);
    // Store the endpoint response time
    INT32 reponseTime;
    try
    {
        reponseTime = BposReadInt(config->getChild("InterCcrtCommResponseTime")->getValue().c_str());
    }
    catch (XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface: Setting response time to 50ms - %s", excpt.GetReason());
        reponseTime = 50;
    }
    InterCcrtCommResponseTime(&reponseTime);
    // Store the flag specifying if keep alive messages should be sent to the endpoint
    bool useKeepAlive = false;
    try
    {
        useKeepAlive = atob(config->getChild("SendKeepAlive")->getValue().c_str());
        if (useKeepAlive)
        {
            UINT64 keepAliveTimer;   // Timer pulse
            try
            {
                keepAliveTimer = atol(config->getChild("KeepAliveTime")->getValue().c_str());
            }
            catch (XmlException &excpt)
            {
                m_server->Log(LOG_ERRORS, "KeepAliveTime not specified - using 100 ms: %s", excpt.GetReason());
                keepAliveTimer = 100;
            }

            // Setup the timer
            m_keepAliveTimer.SetPulseCode(INTER_CCRT_COMM_SERVER_PULSE_CODE);
            m_server->Log(LOG_DEV_DATA, "Set keep alive timer pulse code to %d", INTER_CCRT_COMM_SERVER_PULSE_CODE);
            m_keepAliveTimer.SetPulseValue(KEEPALIVE_PULSE);
            m_server->Log(LOG_DEV_DATA, "Set keep alive timer pulse value to %d", KEEPALIVE_PULSE);
            m_keepAliveTimer.Initialize(m_server->GetProcessName().c_str(), (mSEC_nSEC(keepAliveTimer)), (mSEC_nSEC(keepAliveTimer)));
            m_keepAliveTimer.Stop();
        }

    }
    catch (XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface: Setting UseKeepAlive to false - %s", excpt.GetReason());
        useKeepAlive = false;
    }
    UseKeepAlive(&useKeepAlive);


        // Store the flag specifying if this endpoint handles results
    bool receiveResults = false;
    try
    {
        receiveResults = atob(config->getChild("IsResultConnection")->getValue().c_str());

    }
    catch (XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface: Setting IsResultConnection to false - %s", excpt.GetReason());
        receiveResults = false;
    }
    IsResultConnection(&receiveResults);

    // Store the flag specifying if this endpoint handles Build of materials
    bool transmitBom = false;
    try
    {
        transmitBom = atob(config->getChild("IsBomConnection")->getValue().c_str());

    }
    catch (XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface: Setting IsBomConnection to false - %s", excpt.GetReason());
        transmitBom = false;
    }
    IsBomConnection(&transmitBom);

    m_server->Log(LOG_FN_ENTRY, "InterCcrtCommInterface::Initialize() - complete");
}

void * InterCcrtCommInterface::Execute(void)
{   // Wait for comm port to becom active
    m_server->Log(LOG_FN_ENTRY, "InterCcrtCommInterface:%s - Enter InterCcrtCommInterface::Execute()", CommPortUsed().c_str());
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    WaitForCommPort();
    //transmit stored build files
    m_server->BroadcastStoredBuildRecords();
    if (UseKeepAlive())
    {
        //start keep alive messages
        m_keepAliveTimer.Start();
    }

    m_server->Log(LOG_DEV_DATA, "Done waiting for Comm Port");
    // Keep going until server is terminated
    while (m_server->GetStatus().compare(BEP_TERMINATE))
    {
        if (InterCcrtCommActive())
        {   //Handle incoming data
            status = WaitForIncomingData();
            m_server->Log(LOG_DETAILED_DATA, "InterCcrtCommInterface:%s - GetInterCcrtCommRequest: %s", 
                          CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
            if (status == BEP_STATUS_TIMEOUT && !UseKeepAlive())
            {//client and timeout server must be down
                ReconnectPort();
            }
        }
        else if(UseKeepAlive())
        {   // this is a server and client has disconnected
            status = ReconnectPort();
        }
        //sleep for specified time
        BposSleep(250);
    }
    m_server->Log(LOG_FN_ENTRY, "Exit InterCcrtCommInterface::Execute(void)\n");
    return NULL;
}

const BEP_STATUS_TYPE InterCcrtCommInterface::SendKeepAlive()
{
    string keepAliveMessage = string(HEARTBEAT) + string(MAIN_SEPARARTOR);
    BEP_STATUS_TYPE status = SendMessage(keepAliveMessage);
    if (status != BEP_STATUS_SUCCESS)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface:%s - Error sending keep alive to endpoint - %s",
                      CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}


const BEP_STATUS_TYPE InterCcrtCommInterface::SendBOM(string partID, string bom)
{
    string bomMessage = string(START_BOM) + string(MAIN_SEPARARTOR) + partID + 
        string(MAIN_SEPARARTOR) + bom + string(END_BOM) + string(MAIN_SEPARARTOR); 
    BEP_STATUS_TYPE status = SendMessage(bomMessage);
    if (status != BEP_STATUS_SUCCESS)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface:%s - Error sending bom to endpoint - %s",
                      CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}


const BEP_STATUS_TYPE InterCcrtCommInterface::SendTestResultRequest(string partID)
{
    string message;
    CreateRequestMsg(partID,RESULT,message);

    BEP_STATUS_TYPE status = SendMessage(message);
    if (status != BEP_STATUS_SUCCESS)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface:%s - Error sending test result request to endpoint - %s",
                      CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

const BEP_STATUS_TYPE InterCcrtCommInterface::ReconnectPort()
{   // Attempt to reconnect the port if the unit is active
    int reconnectStatus = -1;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    bool activeStatus = false;
    //set active status to false
    InterCcrtCommActive(&activeStatus);
    if (!UseKeepAlive())
    {//this is a client, wait for server connection
        while (((reconnectStatus = m_interCcrtComm.ReconnectDriver()) != EOK)  &&
               m_server->GetStatus().compare(BEP_TERMINATE))// &&
               //  reconnect until connected (attempts++ < ReconnectAttempts() || !UseKeepAlive()))
        {
            m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Waiting to reconnect the port...", CommPortUsed().c_str());
            BposSleep(ReconnectDelay());
        }
        // Determine reconnect status
        if (reconnectStatus == EOK)
        {
            activeStatus = true;
            status = BEP_STATUS_SUCCESS;
        }
        else
        {
            activeStatus = false;
            status = BEP_STATUS_HARDWARE;
        }
    }
    else
    {//TODO: open port and wait for client
    }

    // Decide if the listener should be deactivated
    InterCcrtCommActive(&activeStatus);
    //send any stored build records to client
    if (IsBomConnection())
    {//TODO: fix this - should only broadcast to this client
        //we should keep separate directories and file lists
        m_server->BroadcastStoredBuildRecords();
    }
    m_server->Log(LOG_ERRORS, "InterCcrtCommInterface:%s - Reconnected port driver - %s",
                  CommPortUsed().c_str(), ConvertStatusToResponse(status).c_str());
    // Return the reconnect status
    return status;
}

const BEP_STATUS_TYPE InterCcrtCommInterface::WaitForIncomingData()
{
    SerialString_t   request;
    BEP_STATUS_TYPE  status = BEP_STATUS_ERROR;
    INT32  bytesRead = -1;
    // Get the request from the port
    m_server->Log(LOG_DETAILED_DATA, "InterCcrtCommInterface:%s - Checking for messages...", CommPortUsed().c_str());
    //lock port
    if (m_commPortMutex.Acquire() == EOK)
    {
        try
        {
            bytesRead = m_interCcrtComm.ReadPort(request, InterCcrtCommResponseTime(), m_interCcrtComm.GetTimeoutTime());
            //release lock
            m_commPortMutex.Release();
        }
        catch (...)
        {
            m_server->Log(LOG_ERRORS,"Exception sending message");
            m_commPortMutex.Release();
        }
    }
    else
    {
        m_server->Log(LOG_ERRORS,"Error locking commport mutex");
    }

    m_server->Log(LOG_DETAILED_DATA, "InterCcrtCommInterface:%s - Received %d bytes", CommPortUsed().c_str(), bytesRead);
    // If a request was received, process it
    if (bytesRead > 0)
    {
        //handle incoming data
        status = ProcessMessagesFromRaw(m_partialMessageBuffer + string((char *)&request[0], bytesRead));
    }
    else
    {
        status = BEP_STATUS_TIMEOUT;
    }
    return status;

}
const BEP_STATUS_TYPE InterCcrtCommInterface::BuildDataToString(XmlNode inputBuild, string &message)
{
    //clear message
    message = "";
    try
    {
        //const XmlNodeMap *buildMap = &inputBuild.getChild(m_server->GetDataTag("VehicleBuild"))->getChildren();
        const XmlNodeMap *buildMap = &inputBuild.getChildren();
        for (XmlNodeMapCItr iter = buildMap->begin(); iter != buildMap->end(); iter++)
        {
            message += (iter->second->getName() + DETAIL_SEPARARTOR + iter->second->getValue() + MAIN_SEPARARTOR);
        }
        return BEP_STATUS_SUCCESS;
    }
    catch (XmlException &excpt)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface:%s - Build Data not in correct format: %s", 
                      CommPortUsed().c_str(), excpt.GetReason());
        return BEP_STATUS_ERROR;
    }
    catch (BepException &excpt)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface:%s - Build Data not in correct format: %s", 
                      CommPortUsed().c_str(), excpt.GetReason());
        return BEP_STATUS_ERROR;
    }
    catch (...)
    {
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface:%s - Build Data not in correct format", 
                      CommPortUsed().c_str());
        return BEP_STATUS_ERROR;
    }
}
const BEP_STATUS_TYPE InterCcrtCommInterface::RequestBuildData(string &partId, string &message)
{
    XmlNode buildData(m_server->GetDataTag("BuildData"), "");
    message.reserve(2048);  // Reserve a 2k area for the response

    string status = m_server->ReadBuildData(partId, buildData);

    // Check if we got a decent response
    if (!status.compare(BEP_SUCCESS_RESPONSE))
    {   // Good data, format for endpoint
        return BuildDataToString(*buildData.getChild(m_server->GetDataTag("VehicleBuild")),message);
    }
    else
    {//error obtaining build data
        m_server->Log(LOG_ERRORS, "InterCcrtCommInterface:%s - Error obtaining build data from Ccrt System", 
                      CommPortUsed().c_str());

    }

    return BEP_STATUS_ERROR;
}

void InterCcrtCommInterface::StoreTestResults(string partId, vector<string> msgDetails)
{
    XmlNode results(m_server->GetDataTag("TestResult"), partId);

    vector<string>::iterator iter;

    // Process each of the fields
    for (iter = msgDetails.begin() + PART_ID_INDEX + 1; iter != msgDetails.end()-1; iter++)
    {
        XmlNode *resultNode;
        m_server->Log(LOG_DEV_DATA, "Processing test result field: %s", (*iter).c_str());
        resultNode = ProcessResultField((*iter));
        // Add the current test result node to the overall results
        m_server->Log(LOG_DEV_DATA, "Adding new test result node: %s", resultNode->ToString().c_str());
        results.addChild(resultNode);
    }

    m_server->Log(LOG_DEV_DATA, "Finished adding new test result nodes: %s", results.ToString().c_str());
    // Command the server to store the test result
    m_server->Write(&results);
    // Clean up the node to prevent a memory leak
    results.clear();
}


XmlNode* InterCcrtCommInterface::ProcessResultField(string &resultField)
{
    XmlNode resultNode("", "");
    string testResultName;
    string testResultValue;
    // Get the test name
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Processing test result field: %s", 
                  CommPortUsed().c_str(), resultField.c_str());
    vector<string> resultItem;
    SplitString(resultField, resultItem, DETAIL_SEPARARTOR, false);

    //set main name and result

    //get name
    //use get data tag translation on test result name if exists
    testResultName = TranslateTestResultTag(resultItem[TEST_RESULT_NAME_INDEX]);
    resultNode.setName(testResultName);
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Set test result name: %s",
                  CommPortUsed().c_str(), testResultName.c_str());

    //get result
    resultNode.addChild(BEP_RESULT, resultItem[TEST_RESULT_RESULT_INDEX], ATTRIBUTE_NODE);
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Set test result attribute: %s",
                  CommPortUsed().c_str(), resultItem[TEST_RESULT_RESULT_INDEX].c_str());

    int numDetails = resultItem.size() / NUMBER_OF_RESULT_DETAIL_FIELDS;       
    //add details if they exist
    for (int x = 0; x < numDetails; x++)
    {
        resultNode.addChild(GetResultDetailNode(resultItem).Copy());
    }
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Processing test result field complete", 
                  CommPortUsed().c_str());
    return resultNode.Copy();
}

XmlNode InterCcrtCommInterface::GetResultDetailNode(vector<string> &resultItem)
{
    XmlNode resultNode("", "");
    string testResultName = "";
    string detailValue;

    //get name
    //use get data tag translation on test result name if exists
    testResultName = TranslateTestResultTag(resultItem[TEST_RESULT_NAME_INDEX]);
    resultNode.setName(testResultName);
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Set test result name: %s",
                  CommPortUsed().c_str(), testResultName.c_str());

    //get result
    resultNode.addChild(BEP_RESULT, resultItem[TEST_RESULT_RESULT_INDEX], ATTRIBUTE_NODE);
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Set test result attribute: %s",
                  CommPortUsed().c_str(), resultItem[TEST_RESULT_RESULT_INDEX].c_str());
    //min
    resultNode.addChild("Min", resultItem[TEST_RESULT_MIN_INDEX], ATTRIBUTE_NODE);
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Set test Min attribute: %s",
                  CommPortUsed().c_str(), resultItem[TEST_RESULT_MIN_INDEX].c_str());
    //max
    resultNode.addChild("Max", resultItem[TEST_RESULT_MAX_INDEX], ATTRIBUTE_NODE);
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Set test Max attribute: %s",
                  CommPortUsed().c_str(), resultItem[TEST_RESULT_MAX_INDEX].c_str());
    //value
    detailValue = resultItem[TEST_RESULT_VALUE_INDEX];
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Set test Value: %s",
                  CommPortUsed().c_str(), detailValue.c_str());
    //units
    resultNode.addChild("Units", resultItem[TEST_RESULT_UNITS_INDEX], ATTRIBUTE_NODE);
    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Set test Units attribute: %s",
                  CommPortUsed().c_str(), resultItem[TEST_RESULT_UNITS_INDEX].c_str());

    resultNode.setName(testResultName);
    resultNode.setValue(detailValue);

    m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Added test result detail: %s - %s",
                  CommPortUsed().c_str(), testResultName.c_str(), detailValue.c_str());

    return resultNode;
}

bool InterCcrtCommInterface::InterCcrtCommActive(const bool *active) /*= NULL*/
{
    bool returnActive = false;
    if (m_commsActiveMutex.Acquire() == EOK)
    {
        // If a value was supplied, update our state
        if (active != NULL)  
        {
            m_isActive = *active;
            if (!m_isActive)
            {//set not connected fault
                m_server->SetFault(CommPortUsed()+"PortNotConnected");
            }
            else
            {//clear not connected fault
                m_server->ClearFault(CommPortUsed()+"PortNotConnected");
            }
        }
        returnActive = m_isActive;
        m_commsActiveMutex.Release();
        return returnActive;
    }
    else
    {
        m_server->Log(LOG_ERRORS,"Error locking comm active mutex");
        return false;
    }
}

const INT32& InterCcrtCommInterface::ReconnectDelay(const INT32 *reconnectDelay) /*= NULL*/
{
    if (reconnectDelay != NULL)  m_reconnectDelay = *reconnectDelay;
    return m_reconnectDelay;
}

const INT32& InterCcrtCommInterface::ReconnectAttempts(const INT32 *attempts /*= NULL*/)
{
    if (attempts != NULL)  m_reconnectAttempts = *attempts;
    return m_reconnectAttempts;
}

void InterCcrtCommInterface::WaitForCommPort(void)
{
    TaskMonTask_t commPort;
    bool active = true;
    do
    {   // Wait for the endpoint comm port
        ITaskMon::ReadTaskInfo(commPort, CommPortUsed().c_str(), TASKMON_CLASS_SERIAL);
        m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Waiting for comm port - state: %d [waiting for %d]",
                      CommPortUsed().c_str(), commPort.m_taskState, TASKSTATE_RUN);
        // If the state is not run, register the port driver
        if (commPort.m_taskState != TASKSTATE_RUN)
        {   // Wait a bit before attempting to connect/reconnect
            BposSleep(PortRegisterDelay());
            // Register the port driver
            m_interCcrtComm.RegisterPortDriver();

            if (m_server->m_faultServer != NULL && active)
            {
                active = false;
                InterCcrtCommActive(&active);
            }
            // Wait a bit to allow port driver to finish registering
            BposSleep(PortRegisterDelay());
        }
        // Keep checking until state is run and not terminated
    } while ( (commPort.m_taskState != TASKSTATE_RUN) && (m_server->GetStatus().compare(BEP_TERMINATE)) );
    active = true;
    InterCcrtCommActive(&active);
}

const string& InterCcrtCommInterface::CommPortUsed(const string *commPort)
{
    if (commPort != NULL)  m_commPort = *commPort;
    return m_commPort;
}

const INT32& InterCcrtCommInterface::PortRegisterDelay(const INT32 *registerDelay) /*= NULL*/
{
    if (registerDelay != NULL)  m_portRegisterDelay = *registerDelay;
    return m_portRegisterDelay;
}

const INT32& InterCcrtCommInterface::InterCcrtCommResponseTime(const INT32 *responseTime) /*= NULL*/
{
    if (responseTime != NULL)  m_interCcrtCommResponseTime = *responseTime;
    return m_interCcrtCommResponseTime;
}

inline const string InterCcrtCommInterface::TranslateTestResultTag(string tag)
{
    return m_server->GetDataTag(tag);
}

inline const bool& InterCcrtCommInterface::UseKeepAlive(bool *useKeepAlive /*= NULL*/)
{
    if (useKeepAlive != NULL)  m_useKeepAlive = *useKeepAlive;
    return m_useKeepAlive;
}

inline const bool& InterCcrtCommInterface::IsResultConnection(bool *isResultConnection /*= NULL*/)
{
    if (isResultConnection != NULL)  m_isBomConnection = *isResultConnection;
    return m_isResultConnection;
}

inline const bool& InterCcrtCommInterface::IsBomConnection(bool *isBomConnection /*= NULL*/)
{
    if (isBomConnection != NULL)  m_isBomConnection = *isBomConnection;
    return m_isBomConnection;
}

//const BEP_STATUS_TYPE GetFullMessageFromString(string &testerRequest, string &returnMessage)
const BEP_STATUS_TYPE InterCcrtCommInterface::GetFullMessageFromString(string &testerRequest, string &returnMessage)
{
    string startID = "";
    size_t position;
    //get a full message from raw data

    if ((position = testerRequest.find(MAIN_SEPARARTOR)) != string::npos)
    {
        startID = testerRequest.substr(0, position);

        //m_server->Log(LOG_DEV_DATA, "GetFullMessageFromString:%s - Found request type: %s",
        //       CommPortUsed().c_str(), requestType.c_str());

        //get endTag
        string endTag = GetMessageEndIDFromStartID(startID);
        if (endTag != "")
        {//start tag is valid, continue
            if ((position = testerRequest.find(endTag)) != string::npos)
            {//end tag exists continue
                //set full message to return (include last ;)
                returnMessage = testerRequest.substr(0,position + endTag.length() + 1);
                //remove full message from testerRequest
                testerRequest = testerRequest.substr(position + endTag.length() + 1,testerRequest.length() - returnMessage.length()); 
                return BEP_STATUS_SUCCESS;
            }
            else
            {//no end tag found
                m_server->Log(LOG_ERRORS,"ERROR: end tag not found");
                //printf("\tend tag not found\n");

                //check for next start tag
                string nextRequest = testerRequest.substr(startID.length() + 1, testerRequest.length() - (startID.length() + 1));
                if ((position = FindNextStartTag(nextRequest)) == string::npos)
                {//start tag does not exist 
                    //add to partial message buffer
                    m_partialMessageBuffer = testerRequest;
                    testerRequest = "";
                    returnMessage = "";
                    return BEP_STATUS_SUCCESS;
                }
                else
                {//start tag exists
                    m_server->Log(LOG_ERRORS,"ERROR: Missing end id");
                    //printf("\tERROR: Missing end id\n");
                }
            }
        }
        else
        {//error invalid start tag
            m_server->Log(LOG_ERRORS,"ERROR: Invalid message start identifier");            
            //printf("\tERROR: Invalid message start identifier\n");            
        }
        //find next start index
        string nextRequest = testerRequest.substr(startID.length() + 1, testerRequest.length() - (startID.length() + 1));
        if ((position = FindNextStartTag(nextRequest)) != string::npos)
        {
            m_server->Log(LOG_DEV_DATA,"Setting error data and updating tester request");
            //printf("\tSetting error data and updating tester request\n");
            //set message to error data
            returnMessage = testerRequest.substr(0,startID.length() + 1 + position);
            //remove error data from tester request
            testerRequest = nextRequest.substr(position,testerRequest.length() - position); 
            //return failure
            return BEP_STATUS_ERROR;

        }
        else
        {//no other start tag found, error message is full request
            m_server->Log(LOG_ERRORS,"ERROR: next start tag not found");            
            //printf("\tERROR: next start tag not found\n");            
        }
    }
    else
    {//no main separator found
        m_server->Log(LOG_ERRORS,"ERROR: No end separator found");
        //printf("\tERROR: No end separator found\n");
    }
    m_server->Log(LOG_DEV_DATA,"Setting error data and clearing tester request");
    //printf("\tSetting error data and clearing tester request\n");
    returnMessage = testerRequest;
    testerRequest = "";
    return BEP_STATUS_ERROR;
}


//const BEP_STATUS_TYPE ProcessMessagesFromRaw(string testerRequest)//INT32 bytesRead, SerialString_t request)
const BEP_STATUS_TYPE InterCcrtCommInterface::ProcessMessagesFromRaw(string testerRequest)
{

    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    //m_server->Log(LOG_DEV_DATA, "InterCcrtCommInterface:%s - Received request: %s", 
    //              CommPortUsed().c_str(), testerRequest.c_str());
    while (testerRequest.length() > 0)
    {//get next full message
        string msg="";
        CcrtProtocolMessage ccrtMsg;
        if (GetFullMessageFromString(testerRequest,msg) != BEP_STATUS_ERROR)
        {
            if (ConvertFullMessageToCcrtProtocolMessage(msg,ccrtMsg) != BEP_STATUS_ERROR)
            {
                m_server->Log(LOG_DETAILED_DATA, "InterCcrtCommInterface:%s - Message converted, processing...", CommPortUsed().c_str());
                switch (ccrtMsg.m_messageType)
                {
                case ACK:
                    //process ack and update status
                    status = ProcessAck(ccrtMsg) == BEP_STATUS_SUCCESS ? status : BEP_STATUS_ERROR; 
                    break;          
                case TYPE_HEARTBEAT:
                    //process heartbeat and update status
                    status = ProcessHeartbeat(ccrtMsg) == BEP_STATUS_SUCCESS ? status : BEP_STATUS_ERROR; 
                    break;          
                case REQUEST:       
                    //process ack and update status
                    status = ProcessRequest(ccrtMsg) == BEP_STATUS_SUCCESS ? status : BEP_STATUS_ERROR; 
                    break;          
                case BOM:           
                    //process ack and update status
                    status = ProcessBom(ccrtMsg) == BEP_STATUS_SUCCESS ? status : BEP_STATUS_ERROR; 
                    break;          
                case RESULT:        
                    //process ack and update status
                    status = ProcessResult(ccrtMsg) == BEP_STATUS_SUCCESS ? status : BEP_STATUS_ERROR; 
                    break;
                default:
                    m_server->Log(LOG_ERRORS,"ERROR: Message Type Not Supported");
                    return BEP_STATUS_ERROR;
                }
            }
            else
            {//error converting message to ccrt protocol type
                status = BEP_STATUS_ERROR;
            }
        }
        else
        {//error in overall message format
            status = BEP_STATUS_ERROR;
        }
    }
    return status;
}

//const BEP_STATUS_TYPE ConvertFullMessageToCcrtProtocolMessage(string fullMessage, CcrtProtocolMessage &ccrtMsg)
const BEP_STATUS_TYPE InterCcrtCommInterface::ConvertFullMessageToCcrtProtocolMessage(string fullMessage, CcrtProtocolMessage &ccrtMsg)
{
    ccrtMsg.SetMainSeparatedValues(fullMessage);
    vector<string>::iterator iter;
    bool error = false;
    bool getDetails = false;
    int detailLength = 0;
    vector<string>::iterator itrStr;
    m_server->Log(LOG_DETAILED_DATA, "InterCcrtCommInterface:%s - Converting full message to ccrt protocol message...", CommPortUsed().c_str());

    //check for min length
    if (ccrtMsg.m_mainSeparatedValues.size() > 0)
    {
        m_server->Log(LOG_DETAILED_DATA, "InterCcrtCommInterface:%s - Determining message type for: %s...", CommPortUsed().c_str(),ccrtMsg.m_mainSeparatedValues[0].c_str());
        ccrtMsg.m_messageType = StringToMessageType(ccrtMsg.m_mainSeparatedValues[0]);
        //check for specific message type min length
        switch ( ccrtMsg.m_messageType)
        {
        case ACK:
            if (ccrtMsg.m_mainSeparatedValues.size() >= MESSAGE_MIN_NUM_STRINGS)
            {
                //remove start id string
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.begin());
                //remove end id string
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.end());
                return BEP_STATUS_SUCCESS;
            }
            else
            {
                m_server->Log(LOG_ERRORS,"\tERROR: ACK invalid num strings main sep values: %d Min: %d", ccrtMsg.m_mainSeparatedValues.size(), MESSAGE_MIN_NUM_STRINGS);
                break;
            }
        case TYPE_HEARTBEAT:
            return BEP_STATUS_SUCCESS;
        case REQUEST:
            if (ccrtMsg.m_mainSeparatedValues.size() >= REQUEST_NUM_STRINGS)
            {
                ccrtMsg.m_partIdentifier = ccrtMsg.m_mainSeparatedValues[PART_ID_INDEX];
                //remove start id string
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.begin());
                //set identifier
                //itrStr = ccrtMsg.m_mainSeparatedValues.begin();
                //ccrtMsg.m_partIdentifier = (*itrStr).c_str();
                //remove part id string
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.begin());
                //remove end id string
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.end());
                return BEP_STATUS_SUCCESS;
            }
            else
            {
                m_server->Log(LOG_ERRORS,"\tERROR: REQUEST invalid num strings main sep values: %d Min: %d", ccrtMsg.m_mainSeparatedValues.size(), REQUEST_NUM_STRINGS);
                break;
            }
        case BOM:
            if (ccrtMsg.m_mainSeparatedValues.size() >= BOM_MIN_NUM_STRINGS)
            {
                ccrtMsg.m_partIdentifier = ccrtMsg.m_mainSeparatedValues[PART_ID_INDEX];
                //remove start id string
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.begin());
                //set identifier
                //itrStr = ccrtMsg.m_mainSeparatedValues.begin();
                //ccrtMsg.m_partIdentifier = (*itrStr).c_str();
                //remove part id string
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.begin());
                //remove end id string
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.end());
                getDetails = true;
                detailLength = NUMBER_OF_BOM_DETAIL_FIELDS;
                break;
            }
            else
            {
                m_server->Log(LOG_ERRORS,"\tERROR: BOM invalid num strings main sep values: %d Min: %d", ccrtMsg.m_mainSeparatedValues.size(), BOM_MIN_NUM_STRINGS);
                break;
            }
        case RESULT:
            if (ccrtMsg.m_mainSeparatedValues.size() >= RESULT_MIN_NUM_STRINGS)
            {
                ccrtMsg.m_partIdentifier = ccrtMsg.m_mainSeparatedValues[PART_ID_INDEX];
                //remove start id string
                //printf("\tErasing start tag\n");
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.begin());
                //printf("\tstart tag erased\n");
                //set identifier
                //itrStr = ccrtMsg.m_mainSeparatedValues.begin();
                //ccrtMsg.m_partIdentifier = (*itrStr).c_str();
                //remove part id string
                //printf("\tErasing part id\n");
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.begin());
                //printf("\tpart id erased\n");
                //remove end id string
                //ccrtMsg.m_mainSeparatedValues.erase(ccrtMsg.m_mainSeparatedValues.end());
                //printf("\tend id erased\n");

                getDetails = true;
                detailLength = NUMBER_OF_RESULT_DETAIL_FIELDS;
                break;
            }
            else
            {
                m_server->Log(LOG_ERRORS,"\tERROR: RESULT invalid num strings main sep values: %d Min: %d", ccrtMsg.m_mainSeparatedValues.size(), RESULT_MIN_NUM_STRINGS);
                break;
            }

        default:
            //printf("\tERROR: Invalid message type\n");
            m_server->Log(LOG_ERRORS,"\tERROR: Invalid message type\n");
            break;

        }
    }
    if (getDetails)
    {
        //check detail section is correct length
        for (iter = ccrtMsg.m_mainSeparatedValues.begin() + PART_ID_INDEX + 1; iter != (ccrtMsg.m_mainSeparatedValues.end() - 1); iter++)
        {
            //Get detail values
            if (ccrtMsg.GetDetailSeparatedValues( (*iter).c_str()).size() % detailLength != 0)
            {//incorrect number of detail fields
                //printf("\tERROR: incorrct num details %s %d %d\n", (*iter).c_str(), detailLength, ccrtMsg.GetDetailSeparatedValues( (*iter).c_str()).size());
                m_server->Log(LOG_ERRORS,"\tERROR: incorrct num details %s %d %d", (*iter).c_str(), detailLength, ccrtMsg.GetDetailSeparatedValues( (*iter).c_str()).size());
                error = true;
                break;
            }
        }
        if (!error)
        {
            return BEP_STATUS_SUCCESS;
        }
    }
    //error invalid message length
    m_server->Log(LOG_ERRORS,"\tERROR: Invalid number of strings in message %d", ccrtMsg.m_mainSeparatedValues.size());
    //printf("\tERROR: Invalid number of strings in message %d\n", ccrtMsg.m_mainSeparatedValues.size());
    return BEP_STATUS_ERROR; 
}


//const BEP_STATUS_TYPE ProcessAck(CcrtProtocolMessage ccrtMsg)
const BEP_STATUS_TYPE InterCcrtCommInterface::ProcessAck(CcrtProtocolMessage ccrtMsg)
{
    //TODO:
    //add acknowledgement to queue


    //just report ack received
    //printf("\tACK received: %s\n", ccrtMsg.m_mainSeparatedValues[0].c_str());
    m_server->Log(LOG_DETAILED_DATA, "InterCcrtCommInterface:%s - ACK Received for %s...", CommPortUsed().c_str(), ccrtMsg.m_mainSeparatedValues[PART_ID_INDEX + 1].c_str());
    return BEP_STATUS_SUCCESS;
}

//const BEP_STATUS_TYPE ProcessHeartbeat(CcrtProtocolMessage ccrtMsg)
const BEP_STATUS_TYPE InterCcrtCommInterface::ProcessHeartbeat(CcrtProtocolMessage ccrtMsg)
{//ack heartbeat
    string msg;
    CreateAckMsg(TYPE_HEARTBEAT,msg);
    m_server->Log(LOG_DETAILED_DATA, "InterCcrtCommInterface:%s - Processing heartbeat...", CommPortUsed().c_str());
    if (SendMessage(msg) == BEP_STATUS_SUCCESS)
    {//update last comm time
        return BEP_STATUS_SUCCESS;

    }
    else
    {//error sending ack
        //printf("\tERROR: sending ACK\n");
        m_server->Log(LOG_ERRORS,"\tERROR: sending ACK\n");
        return BEP_STATUS_ERROR;
    }
}

//const BEP_STATUS_TYPE ProcessRequest(CcrtProtocolMessage ccrtMsg)
const BEP_STATUS_TYPE InterCcrtCommInterface::ProcessRequest(CcrtProtocolMessage ccrtMsg)
{//obtain and send requested info
    MessageType requestType =  StringToMessageType(ccrtMsg.m_mainSeparatedValues[PART_ID_INDEX + 1]);
    string requestResponse = "";
    m_server->Log(LOG_DETAILED_DATA, "InterCcrtCommInterface:%s - Processing request...", CommPortUsed().c_str());
    switch (requestType)
    {
    case BOM :
        if (CreateBomMsg(ccrtMsg.m_partIdentifier,requestResponse) == BEP_STATUS_SUCCESS)
        {
            if (SendMessage(requestResponse) == BEP_STATUS_SUCCESS)
            {
                return BEP_STATUS_SUCCESS;
            }
        }
        else break;
    case RESULT : 
        if (CreateResultMsg(ccrtMsg.m_partIdentifier,requestResponse) == BEP_STATUS_SUCCESS)
        {
            if (SendMessage(requestResponse) == BEP_STATUS_SUCCESS)
            {
                return BEP_STATUS_SUCCESS;
            }
        }
        else break;
    default:
        //request type not supported
        //printf("\tERROR: Request type not supported\n");
        m_server->Log(LOG_ERRORS,"\tERROR: Request type not supported\n");
        break;
    }
    //printf("\tERROR: processing request\n");
    m_server->Log(LOG_ERRORS,"\tERROR: processing request\n");
    return BEP_STATUS_ERROR;
}

//const BEP_STATUS_TYPE ProcessBom(CcrtProtocolMessage ccrtMsg)
const BEP_STATUS_TYPE InterCcrtCommInterface::ProcessBom(CcrtProtocolMessage ccrtMsg)
{//ack incoming bom and save bom to system 
    //throw BepException("Incoming BOM Processing not supported");
    m_server->Log(LOG_ERRORS, "InterCcrtCommInterface:%s - Processing BOM Not yet supported...", CommPortUsed().c_str());
    return BEP_STATUS_ERROR;
}

//const BEP_STATUS_TYPE ProcessResult(CcrtProtocolMessage ccrtMsg)
const BEP_STATUS_TYPE InterCcrtCommInterface::ProcessResult(CcrtProtocolMessage ccrtMsg)
{//ack incoming result and save result to system 
    string msg;
    CreateAckMsg(RESULT,msg);
    m_server->Log(LOG_DETAILED_DATA, "InterCcrtCommInterface:%s - Processing result...", CommPortUsed().c_str());

    if (SendMessage(msg) == BEP_STATUS_SUCCESS)
    {//update 
        try
        {
            StoreTestResults(ccrtMsg.m_partIdentifier, ccrtMsg.m_mainSeparatedValues);
            return BEP_STATUS_SUCCESS;
        }
        catch (...)
        {
            //error storing result
            //printf("\tERROR: storing result\n");
            m_server->Log(LOG_ERRORS,"\tERROR: storing result\n");
            return BEP_STATUS_ERROR;
        }
    }
    else
    {//error sending ack
        //printf("\tERROR: sending ACK\n");
        m_server->Log(LOG_ERRORS,"\tERROR: sending ACK\n");
        return BEP_STATUS_ERROR;
    }
}


//const BEP_STATUS_TYPE CreateAckMsg(MessageType msgType, string &message)
const BEP_STATUS_TYPE InterCcrtCommInterface::CreateAckMsg(MessageType msgType, string &message)
{
    message = string(START_ACK) + string(MAIN_SEPARARTOR) + MessageTypeToString(msgType) + 
              string(MAIN_SEPARARTOR) + string(END_ACK) + string(MAIN_SEPARARTOR);
    return BEP_STATUS_SUCCESS; 
}

//const BEP_STATUS_TYPE CreateRequestMsg(string partID, MessageType msgType, string &message)
const BEP_STATUS_TYPE InterCcrtCommInterface::CreateRequestMsg(string partID, MessageType msgType, string &message)
{
    message = string(START_REQUEST) + string(MAIN_SEPARARTOR) + string(partID) + string(MAIN_SEPARARTOR) + 
              MessageTypeToString(msgType) + string(MAIN_SEPARARTOR) + string(END_ACK) + string(MAIN_SEPARARTOR);
    return BEP_STATUS_SUCCESS; 
}

//const BEP_STATUS_TYPE CreateBomMsg(string partID, string &message)
const BEP_STATUS_TYPE InterCcrtCommInterface::CreateBomMsg(string partID, string &message)
{
    string bom;
    if (RequestBuildData(partID,bom) == BEP_STATUS_SUCCESS)
    {
        message = string(START_BOM) + string(MAIN_SEPARARTOR) + partID + string(MAIN_SEPARARTOR) + bom + string(END_BOM) + string(MAIN_SEPARARTOR); 
        return BEP_STATUS_SUCCESS; 
    }
    return BEP_STATUS_ERROR; 
}

//const BEP_STATUS_TYPE CreateResultMsg(string partID, string &message)
const BEP_STATUS_TYPE InterCcrtCommInterface::CreateResultMsg(string partID, string &message)
{
    //throw BepException("Outgoing result creation not yet supported");
    return BEP_STATUS_ERROR; 
}

//const string GetMessageEndIDFromStartID(string startID)
const string InterCcrtCommInterface::GetMessageEndIDFromStartID(string startID)
{
    if (!startID.compare(START_BOM))
    {
        return END_BOM;
    }
    else if (!startID.compare(START_RESULT))
    {
        return END_RESULT;
    }
    else if (!startID.compare(START_ACK))
    {
        return END_ACK;
    }
    else if (!startID.compare(START_REQUEST))
    {
        return END_REQUEST;
    }
    else if (!startID.compare(HEARTBEAT))
    {
        return HEARTBEAT;
    }
    else
    {
        return "";
    }
}

//size_t FindNextStartTag(string testerRequest)
size_t InterCcrtCommInterface::FindNextStartTag(string &testerRequest)
{
    size_t position = string::npos;
    size_t tempPosition = string::npos;
    if ((tempPosition = testerRequest.find(START_BOM)) != string::npos)
    {
        position = tempPosition;
    }
    if ((tempPosition = testerRequest.find(START_RESULT)) != string::npos)
    {
        position = tempPosition > position ? position : tempPosition;
    }
    if ((tempPosition = testerRequest.find(START_ACK)) != string::npos)
    {
        position = tempPosition > position ? position : tempPosition;
    }
    if ((tempPosition = testerRequest.find(START_REQUEST)) != string::npos)
    {
        position = tempPosition > position ? position : tempPosition;
    }
    if ((tempPosition = testerRequest.find(HEARTBEAT)) != string::npos)
    {
        position = tempPosition > position ? position : tempPosition;
    }
    return position;
}


//MessageType StringToMessageType(string startID)
MessageType InterCcrtCommInterface::StringToMessageType(string startID)
{
    if (startID == START_BOM)
    {
        return BOM;
    }
    else if (startID == START_ACK)
    {
        return ACK;
    }
    else if (startID == START_RESULT)
    {
        return RESULT;
    }
    else if (startID == START_REQUEST)
    {
        return REQUEST;
    }
    else if (startID == HEARTBEAT)
    {
        return TYPE_HEARTBEAT;
    }
    else
    {
        return NOT_SUPPORTED;
    }
}

//string MessageTypeToString(MessageType msgType)
string InterCcrtCommInterface::MessageTypeToString(MessageType msgType)
{
    if (msgType == BOM)
    {
        return START_BOM;
    }
    else if (msgType == ACK)
    {
        return START_ACK;
    }
    else if (msgType == RESULT)
    {
        return START_RESULT;
    }
    else if (msgType == REQUEST)
    {
        return START_REQUEST;
    }
    else if (msgType == TYPE_HEARTBEAT)
    {
        return HEARTBEAT;
    }
    else
    {
        return MESSAGE_TYPE_NOT_SUPPORTED;
    }
}
const BEP_STATUS_TYPE InterCcrtCommInterface::SendMessage(string message)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    bool statusOk = false;
    if (InterCcrtCommActive())
    {
        if (m_commPortMutex.Acquire() == EOK)
        {
            try
            {
                status = m_interCcrtComm.Send(message);
                statusOk = true;
                m_commPortMutex.Release();
            }
            catch (...)
            {
                m_server->Log(LOG_ERRORS,"Exception sending message");
                m_commPortMutex.Release();
            }
        }
        else
        {
            m_server->Log(LOG_ERRORS,"Error locking commport mutex");
        }
    }
    else
    {
        m_server->Log(LOG_ERRORS,"Error unable to send message because comms are not active");
    }
    //InterCcrtCommActive(&statusOk);
    return status;
}
