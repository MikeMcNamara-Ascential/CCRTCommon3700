//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  The DSA Server will interface to the Multi-Function Tester (MFT) using the DSA Protocol.
//  Messages will be received from the MFT and processed.  A response will be generated to
//  transmit back to the MFT.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
// 
//*************************************************************************
#include "Gen3Interface.h"
#include "DataAnalysis.h"

//-----------------------------------------------------------------------------
Gen3Interface::Gen3Interface() : BepServer(), m_testStartIndex(0), m_testStandReady(true), m_activateMessageSchedule(false),
    m_dvtComm(NULL), m_gen3In(NULL), m_gen3Out(NULL), m_icmData(NULL), m_ndb(NULL), m_currentScheduledMessage(0),
    m_wheelbaseAdjusting(false), m_isAwdVehicle(false), m_brakeTestComplete(false), m_autoRaiseRetainers(false),
    m_autoLowerRetainers(false)
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
Gen3Interface::~Gen3Interface()
{   
    if(m_dvtComm != NULL)  delete m_dvtComm;
    if(m_gen3In != NULL)   delete m_gen3In;
    if(m_gen3Out != NULL)  delete m_gen3Out;
    if(m_icmData != NULL)  delete m_icmData;
    if(m_ndb != NULL)      delete m_ndb;
    // Clear out the gen3 message list
    for(UINT8 index = 0; index < m_mode1Messages.size(); index++)
        delete m_mode1Messages[index];
    // Clear out all XmlNodeMaps
    m_testResultTags.clear(true);
    m_testStatusTags.clear(true);
    m_brakeStatTags.clear(true);
    m_plcCodes.clear(true);
    // Clear all DVT markers
    ClearDvtMarkers();
    // Clear wheelbase positions
    m_wheelbasePositions.clear(true);
}

//-----------------------------------------------------------------------------
void Gen3Interface::Initialize(const string &fileName)
{   // Call the base class to perform the initialization
    BepServer::Initialize(fileName);
}

//-----------------------------------------------------------------------------
void Gen3Interface::Initialize(const XmlNode *document)
{   // Call the base class to begin the initialization
    BepServer::Initialize(document);
    // Make a copy of the configuration data
    m_config = document->Copy();
    // Store the test result data tags
    try
    {
        m_testResultTags.DeepCopy(document->getChild("Setup/MessageData/TestResultTags")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No test result tags defined: %s", excpt.GetReason());
        m_testResultTags.clear(true);
    }
    // Store the test status tags
    try
    {
        m_testStatusTags.DeepCopy(document->getChild("Setup/MessageData/TestStatusTags")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No test status tags defined: %s", excpt.GetReason());
        m_testStatusTags.clear(true);
    }
    // Store the brake statistic test result tags
    try
    {
        m_brakeStatTags.DeepCopy(document->getChild("Setup/MessageData/BrakeStatData")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No brake stat tags defined: %s", excpt.GetReason());
        m_brakeStatTags.clear(true);
    }
    // Store the PLC codes
    try
    {
        m_plcCodes.DeepCopy(document->getChild("Setup/MessageData/PlcCodes")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No PLC codes defined: %s", excpt.GetReason());
        m_plcCodes.clear(true);
    }
    // Store system data items to monitor
    try
    {
        m_systemDataMonitorItems.DeepCopy(document->getChild("Setup/MonitorSystemDataItems/DataItems")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not load system data items to monitor: %s", excpt.GetReason());
        m_systemDataMonitorItems.clear(true);
    }
    // Get the incoming message timeout
    long timeout = 0;
    try
    { 
        timeout = BposReadInt(document->getChild("Setup/MessageData/Incoming/MessageTimeout")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No incoming message timeout specified, using 5 seconds: %s", excpt.GetReason());
        timeout = 5000;
    }
    DvtMessageTimeout(&timeout);
    // Store the end of message timeout
    timeout = 1;
    try
    {
        timeout = BposReadInt(document->getChild("Setup/MessageData/Incoming/MessageEndDelay")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No end of message gap delay set, using 1ms: %s", excpt.GetReason());
        timeout = 1;
    }
    DvtEndOfMessageGapTime(&timeout);
    // Store the valid wheelbase positions
    string wbFileName(getenv("USR_ROOT"));
    try
    {
        wbFileName += document->getChild("Setup/MessageData/Messages/Mode4/WheelbasePositionTable")->getValue();
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Wheelbase Position table file name not specified, using WheelbasePositionTable.xml - %s", excpt.GetReason());
        wbFileName += "WheelbasePositionTable.xml";
    }
    WheelbasePositionTableFileName(&wbFileName);
    m_wheelbasePositions.clear(true);
    LoadWheelbasePositionTable();
    // Store the wheelbase adjust start delay
    INT32 startDelay = 5000;
    try
    {
        startDelay = BposReadInt(document->getChild("Setup/MessageData/Messages/Mode4/WheelbaseAdjustStartDelay")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Wheelbase adjust start delay not specified, using 5sec: %s", excpt.GetReason());
        startDelay = 5000;
    }
    WheelbaseAdjustStartDelay(&startDelay);
    // Store the maximum number of machine faults to report
    INT32 data = 0;
    try
    {
        data = BposReadInt(document->getChild("Setup/MessageData/MaxMachineFaults")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not get maximum number of machine faults for reporting, using 10: %s", excpt.GetReason());
        data = 10;
    }
    MaxMachineFaults(&data);
    // Store the machine fault codes
    try
    {
        m_machineFaultCodes.DeepCopy(document->getChild("Setup/MessageData/Messages/Mode1/Message17/FaultCodes")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No machine fault codes defined! - %s", excpt.GetReason());
    }
    // Check if the retainers should be raised automatically
    bool autoRaise = false;
    try
    {
        autoRaise = atob(document->getChild("Setup/Parameters/AutoRaiseRetainers")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Auto raise retainers not specified, not raising retainers automatically: %s", excpt.GetReason());
        autoRaise = false;
    }
    AutoRaiseRetainers(&autoRaise);
    // Check if the retainers should be auto lowered
    bool autoLower = false;
    try
    {
        autoLower = atob(document->getChild("Setup/Parameters/AutoLowerRetainers")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Auto lower retainers not specified, not raising retainers automatically: %s", excpt.GetReason());
        autoLower = false;
    }
    AutoLowerRetainers(&autoLower);
    // Get the toggle time
    INT32 toggleTime = 0;
    try
    {
        toggleTime = BposReadInt(document->getChild("Setup/Parameters/AutoReatinersToggleDelay")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Retainer command toggle not specifed, using 500ms: %s", excpt.GetReason());
        toggleTime = 500;
    }
    AutoRetainersToggleTime(&toggleTime);
    // Setup the ICM data timer
    SetupIcmDataTimer(document->getChild("Setup/IcmData"));
    SetupScheduledMsgTimer(document->getChild("Setup/MessageData/Messages/Mode42/Outgoing"));
    SetupIncomingMessageTimer(document->getChild("Setup/MessageData/Incoming"));
    SetupSystemDataTimer(document->getChild("Setup/MonitorSystemDataItems"));
    SetupResetTimer(document->getChild("Setup/MessageData/Messages/Mode4/Incoming"));
    SetupWheelbaseTimer(document->getChild("Setup/MessageData/Messages/Mode4"));
    Setup40kmhButtonLatchTimer(document->getChild("Setup/Parameters"));
    InitializeMode1Messages(document->getChild("Setup/MessageData/Messages/Mode1"));
    InitializeMode2Message(document->getChild("Setup/MessageData/Messages/Mode2"));
    InitializeMode6Message(document->getChild("Setup/MessageData/Messages/Mode6"));
    InitializeMode7Message(document->getChild("Setup/MessageData/Messages/Mode7"));
    InitializeMode20Message(document->getChild("Setup/MessageData/Messages/Mode20/Messages"));
}

//-----------------------------------------------------------------------------
const INT32 Gen3Interface::HandlePulse(const INT32 code, const INT32 value)
{   
    INT32 retVal = BEP_STATUS_ERROR;
    Log(LOG_DETAILED_DATA, "Gen3Interface::HandlePulse(code: %d, value: %d)", code, value);
    switch(code)
    {
    case GEN3_PULSE_CODE:
        switch(value)
        {
        case ICM_DATA_PULSE:   // Update the ICM data in each message
            retVal = UpdateIcmData();
            break;

        case SCHEDULED_MSG_PULSE:
            retVal = SendScheduledMessage();
            break;

        case INCOMING_MSG_PULSE:
            retVal = CheckForIncomingMessage();
            break;

        case SYSTEM_DATA_UPDATE_PULSE:
            retVal = UpdateSystemDataItems();
            break;

        case RESET_SYSTEM_PULSE:
            retVal = ResetSystemPulseHandler();
            break;

        case WHEELBASE_TIMER_PULSE:
            retVal = WheelbaseTimerPulseHandler();
            break;

        case FORTY_KMH_BUTTON_PULSE:
            retVal = FortyKmhButtonPulseHandler();
            break;

        default:
            Log(LOG_ERRORS, "Unhandled pulse value!");
            retVal = BEP_STATUS_FAILURE;
            break;
        }
        break;

    default:
        // Call the base class to handle the pulse
        retVal = BepServer::HandlePulse(code, value);
        break;
    }
    Log(LOG_DETAILED_DATA, "Gen3Interface::HandlePulse(code: %d, value: %d) - Exit", code, value);
    return retVal;
}

//-----------------------------------------------------------------------------
const string Gen3Interface::Register(void)
{   // Call the base class to register the process
    string result = BepServer::Register();
    // Create the dvt comm object
    Log(LOG_FN_ENTRY, "Gen3Interface::Register() - Enter");
    if(m_dvtComm == NULL)
    {
        m_dvtComm = new SerialChannel();
        m_dvtComm->Initialize(m_config->getChild("Setup/DvtCommunications"));
    }
    Log(LOG_DEV_DATA, "Initialized DVT Comm Port");
    // Create a new gen3 incoming handler
    if(m_gen3In == NULL)
    {
        m_gen3In = new Gen3Incoming(this);
        m_gen3In->Initialize(m_config->getChild("Setup/MessageData"));
    }
    Log(LOG_DEV_DATA, "Setup incoming message handler");
    // Create a new gen3 outgoing handler
    if(m_gen3Out == NULL)
    {
        m_gen3Out = new Gen3Outgoing(this, m_dvtComm);
        m_gen3Out->Initialize(m_config->getChild("Setup/MessageData"));
    }
    Log(LOG_DEV_DATA, "Setup outgoing message handler");
    // Create new test component comm objects
    m_brakeComponentComm.Initialize(GetDataTag("BrakeTestComponentName"), "Client");
    m_tcaseComponentComm.Initialize(GetDataTag("TcaseTestComponentName"), "Client");
    Log(LOG_DEV_DATA, "Initialized test component communications");
    // Create a motor controller comm object
    m_motorControllerComm.Initialize(GetDataTag("MotorControllerName"), "Client");
    Log(LOG_DEV_DATA, "Initialized motor controller communications");
    // initialize NDB comms
    if(m_ndb == NULL)
    {
        m_ndb = new INamedDataBroker();
    }
    Log(LOG_DEV_DATA, "Initialized NDB comms");
    // Create a new ICM data supplier
    UINT8 rollerCount = ReadSystemData(ROLLER_COUNT);
    RollerCount(&rollerCount);
    m_icmData = new IcmDataSupplier(m_config->getChild("Setup/IcmData"), RollerCount());
    Log(LOG_DEV_DATA, "Opened ICM data files: %s", ConvertStatusToResponse(m_icmData->OpenIcmDataFiles()).c_str());
    // Start the ICM data update timer
    m_icmDataTimer.Start();
    m_scheduledMessageTimer.Start();
    m_incomingMsgTimer.Start();
    m_systemDataTimer.Start();
    // Initialize software version dates
    LoadSoftwareVersion();
    // Reset the machine
    ResetMachine(0x00, false);
    // Update all mode 1 data for items that have been subscribed for
    InitializeMode1MessageData();
    // Return the registration result
    Log(LOG_FN_ENTRY, "Gen3Interface::Register() - Exit: %s", result.c_str());
    return result;
}

//-----------------------------------------------------------------------------
void Gen3Interface::Run(volatile bool *terminateFlag)
{   // Wait for the serial channel to be created
    while(m_dvtComm == NULL)  BposSleep(1000);
    BposSleep(1000);   // Give the channel time to be opened and established
    // Loop until we are terminated
    SerialString_t message;
    while(BEP_TERMINATE != GetStatus())
    {   // Wait for a message from the DVT
        message.clear();
        INT32 byteCount = m_dvtComm->ReadPort(message, DvtMessageTimeout(), DvtEndOfMessageGapTime());
        // Clear the comm port for the next message
        m_dvtComm->ResetConnection();
        if(byteCount > 0)
        {   // Received a message, process it
            char buff[8];
            string strMessage;
            for(UINT16 index = 0; index < message.length(); index++)
            {
                strMessage += CreateMessage(buff, sizeof(buff), "%02X ", message[index]);
            }
            Log(LOG_DEV_DATA, "Received %d bytes from DVT: %s", byteCount, strMessage.c_str());
            if(m_gen3In != NULL)  
            {
                // Add the incoming message to the queue
                if(EOK == m_incomingMsgQueueMutex.Acquire())
                {
                    m_incomingMessageQueue.push_back(message);
                    m_incomingMsgQueueMutex.Release();
                }
                else
                {
                    Log(LOG_ERRORS, "Could not queue up the received message!");
                }
            }
        }
        else
        {
            Log(LOG_DETAILED_DATA, "Timeout waiting for a DVT request!");
        }
    }
}

//-----------------------------------------------------------------------------
const string Gen3Interface::Publish(const XmlNode *node)
{   // Call the base class to handle the Publish functionality
    string result(BepServer::Publish(node));
    bool updateMode1Messages = true;
    // Handle inverse logic for DVT
    if(!node->getName().compare("WheelbaseInPosition") ||       // DVT looks for wheelbase adjusting which is inverse of in position
       !node->getName().compare("HydraulicSystemIdleTimeout"))  // Hyd Pump timeout is inverse of running.
    {
        if(!node->getName().compare("WheelbaseInPosition") && !node->getValue().compare("1"))
        {   // Set the current wheelbase position
            SetCurrentWheelbasePosition();
            m_wheelbaseAdjusting = false;
            m_wheelbaseTimer.Stop();
            // Check if the retainers should be raised automatically
            if(AutoRaiseRetainers())   
            {   // Command the rollers up
                string response;
                m_ndb->Write(RAISE_ROLLS, "1", response, true);
                Log(LOG_DEV_DATA, "Commanded retaining rolls to raise");
                if(!AutoLowerRetainers())
                {
                    BposSleep(AutoRetainersToggleTime());
                    m_ndb->Write(RAISE_ROLLS, "0", response, true);
                    Log(LOG_DEV_DATA, "Cleared retaining rolls raise command");
                }
            }
        }
        XmlNode *nodeCopy = const_cast<XmlNode *>(node);
        nodeCopy->setValue(atob(node->getValue().c_str()) ? "0" : "1");
    }
    // Handle test results
    else if(m_testResultTags.find(node->getName()) != m_testResultTags.end())
    {   // Set the result as the value
        XmlNode *nodeCopy = const_cast<XmlNode *>(node);
        nodeCopy->setValue(node->getAttribute(BEP_RESULT)->getValue().compare(BEP_TESTING_STATUS) ? "1" : "0");
        Log(LOG_DEV_DATA, "Setting test result %s to state %s", node->getName().c_str(), node->getValue().c_str());
        // If this is the axle ratio test, store the axle ratio value
        if(!node->getName().compare(GetDataTag("AxleRatioResultName")))
        {   // Get the axle ratio value
            UINT8 ratio = (UINT8)(atof(node->getChild("AxleRatioValue")->getValue().c_str()) * 100.0);
            SetMode1MessageDataItem("AxleRatioValue", ratio);
            Log(LOG_DEV_DATA, "Set axle ratio to %d percent", ratio);
        }
    }
    // Handle test status items
    else if(m_testStatusTags.find(node->getName()) != m_testStatusTags.end())
    {
        XmlNode *nodeCopy = const_cast<XmlNode *>(node);
        nodeCopy->setValue(!node->getAttribute(BEP_RESULT)->getValue().compare(BEP_PASS_RESPONSE) ? "1" : "0");
        Log(LOG_DEV_DATA, "Setting test status %s to state %s", node->getName().c_str(), node->getValue().c_str());
    }
    // Handle brake stat items
    else if(m_brakeStatTags.find(node->getName()) != m_brakeStatTags.end())
    {
        m_brakeTestComplete = !node->getName().compare("BrakeRightRearBrakeTestStats");
        SetMode20BrakeStatData(node);
    }
    // Handle the drive axle
    else if(!node->getName().compare(DRIVE_AXLE_TAG))
    {   
        SetMode1MessageDataItem(string(DRIVE_AXLE_TAG) + "Front", "0");
        SetMode1MessageDataItem(string(DRIVE_AXLE_TAG) + "Rear", "0");
        SetMode1MessageDataItem(node->getName()+node->getValue(), "1");
    }
    // Handle the all faults tag
    else if(!node->getName().compare("AllFaults"))
    {   // Set the fault codes in each mode 1 message
        UpdateMachineDtcList(node);
        // Indicate there are faults
        XmlNode *nodeCopy = const_cast<XmlNode *>(node);
        nodeCopy->setValue((node->getChildren().size() > 0) ? "1" : "0");
    }
    // Handle the loss comp date
    else if(!node->getName().compare(MACHINE_LOSS_DATE))
    {
        const XmlNode *date = m_config->getChild("Setup/MessageData/Messages/Mode1/Message13/DateFormat");
        // Break out the date items
        int startIndex = BposReadInt(date->getChild("Year")->getAttribute("StartIndex")->getValue().c_str());
        int length = BposReadInt(date->getChild("Year")->getAttribute("Length")->getValue().c_str());
        string year(node->getValue().substr(startIndex, length));
        startIndex = BposReadInt(date->getChild("Month")->getAttribute("StartIndex")->getValue().c_str());
        length = BposReadInt(date->getChild("Month")->getAttribute("Length")->getValue().c_str());
        string month(node->getValue().substr(startIndex, length));
        startIndex = BposReadInt(date->getChild("Day")->getAttribute("StartIndex")->getValue().c_str());
        length = BposReadInt(date->getChild("Day")->getAttribute("Length")->getValue().c_str());
        string day(node->getValue().substr(startIndex, length));
        // Get the time
        startIndex = BposReadInt(date->getChild("Hour")->getAttribute("StartIndex")->getValue().c_str());
        length = BposReadInt(date->getChild("Hour")->getAttribute("Length")->getValue().c_str());
        string hour(ReadSubscribeData(MACHINE_LOSS_TIME).substr(startIndex, length));
        // Create the date and set
        INT16 yearMonth = (BposReadInt(year.c_str()) << 8) | BposReadInt(month.c_str());
        INT16 dayHour = (BposReadInt(day.c_str()) << 8) | BposReadInt(hour.c_str());
        // Update all messages
        SetMode1MessageDataItem("LossCompYearMonth", yearMonth);
        SetMode1MessageDataItem("LossCompDayHour", dayHour);
    }
    // Handle the E-Stop
    else if(!node->getName().compare("PanelEStopPBDepressed"))
    {
        UINT8 promptNumber = !node->getValue().compare("1") ? PromptWaitForPowerSense : PromptNoPrompt;
        SetMode1MessageDataItem("PromptNumber", promptNumber);
    }
    else if(!node->getName().compare(ROLLS_DOWN_DATA_TAG) && !node->getValue().compare("1"))
    {   // Clear the retaining roll positions
        string response;
        m_ndb->Write("RetRollHiPosition", "0", response, false);
        m_ndb->Write("RetRollLoPosition", "0", response, true);
        SetMode1MessageDataItem("RetRollHiPosition", (INT16)0);
    }
    // If this is the 40km/h button press, need to latch for some time to make sure GM can "see" it.
    else if(!node->getName().compare(GetDataTag("Start40kphTag")))
    {   // 
        if(!node->getValue().compare("1"))
        {   // Start the latch timer for the 40km/h button press
            m_40kmhButtonLatchTimer.Start();
        }
        else
        {   // Operator released the button, do not allow Mode 1 Message updating.  This will happen when the latch
            // timer expires.
            updateMode1Messages = false;
        }
    }
    // Update the data item in each of the mode 1 messages
    if(updateMode1Messages)
    {
        SetMode1MessageDataItem(node->getName(), (INT16)BposReadInt(node->getValue().c_str()));
    }
    return result;
}

//-----------------------------------------------------------------------------
const string Gen3Interface::Command(const XmlNode *node)
{
    string result(BEP_ERROR_RESPONSE);
    if(!node->getName().compare("Mode1Message"))
    {   // Send the specified mode 1 message
        SerialString_t message;
        INT32 status = m_mode1Messages[BposReadInt(node->getValue().c_str())]->GetMessage(message);
        if(BEP_STATUS_SUCCESS == status)
        {
            result = ConvertStatusToResponse(m_gen3Out->SendMessage(1, message.length(), message));
        }
        UINT8 msgType = (UINT8)BposReadInt(node->getValue().c_str());
        Log(LOG_DEV_DATA, "Calling HandleMode1Message(message: %d) to handle the request", msgType);
        HandleMode1Message(msgType, 0);
    }
    else if(!node->getName().compare("AdjustWheelbase"))
    {
        result = ConvertStatusToResponse(HandleMode4Message(SelectWheelbase, BposReadInt(node->getValue().c_str())));
        Log(LOG_DEV_DATA, "Adjusted whelbase: %s", result.c_str());
    }
    else
    {
        result = BepServer::Command(node);
    }
    return result;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::HandleMode1Message(const UINT8 &messageNumber, const INT16 &mode1Data)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "Handling Mode 1 Message %d request", messageNumber);
    SerialString_t message;
    if(22 != messageNumber)
    {
        status = m_mode1Messages[messageNumber]->GetMessage(message);
        char buff[32];
        string logMsg;
        for(UINT8 index = 0; index < message.length(); index++)
        {
            logMsg += CreateMessage(buff, sizeof(buff), "\n%03d: %02X", index, message[index]);
        }
        Log(LOG_DEV_DATA, "Retrieved raw msg to send: %s\n", logMsg.c_str());
    }
    else
    {
        status = BuildMode1Message22(mode1Data);
        if(BEP_STATUS_SUCCESS == status)
        {
            status = m_mode1Messages[messageNumber]->GetMessage(message);
        }
    }
    // Send the message
    if(BEP_STATUS_SUCCESS == status)
    {
        status = m_gen3Out->SendMessage(1, message.length(), message);
    }
    Log(LOG_FN_ENTRY, "Handled Mode 1 Message %d request - %s", messageNumber, ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::HandleMode2Message(const INT16 &markerType,
                                                  const INT16 &markerName,
                                                  const INT16 &markerIndex)
{
    Log(LOG_FN_ENTRY, "Handling Mode 2 message request - type: %d, name: %d, index: %d", markerType, markerName, markerIndex);
    INT32 markerOffset = 0;
    if((markerType != 0) || (markerName != 0))
    {
        markerOffset = FindDvtMarker(markerType, markerName);
    }
    IcmDataArray speedData, forceData;
    BEP_STATUS_TYPE speedStatus = m_icmData->ReadSpeedArray(markerOffset+markerIndex, markerOffset+markerIndex+4, speedData);
    BEP_STATUS_TYPE forceStatus = m_icmData->ReadForceArray(markerOffset+markerIndex, markerOffset+markerIndex+4, forceData);
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if((BEP_STATUS_SUCCESS == speedStatus) && (BEP_STATUS_SUCCESS == forceStatus))
    {   // Add the last ICM data point and the start point
        m_mode2Message->UpdateDataItem("TotalSamplePoints", (INT16)(markerIndex+4));
        m_mode2Message->UpdateDataItem("MarkerIndex", markerIndex);
        // Add the speeds and forces to the message
        char buff[8];
        for(INT16 index = 0; index < 4; index++)
        {
            string indexTag(CreateMessage(buff, sizeof(buff), "%d", index+1));
            m_mode2Message->UpdateDataItem("LfSpeed"+indexTag, speedData[LFWHEEL][index]);
            m_mode2Message->UpdateDataItem("RfSpeed"+indexTag, speedData[RFWHEEL][index]);
            m_mode2Message->UpdateDataItem("LrSpeed"+indexTag, speedData[LRWHEEL][index]);
            m_mode2Message->UpdateDataItem("RrSpeed"+indexTag, speedData[RRWHEEL][index]);
            m_mode2Message->UpdateDataItem("LfForce"+indexTag, forceData[LFWHEEL][index]);
            m_mode2Message->UpdateDataItem("RfForce"+indexTag, forceData[RFWHEEL][index]);
            m_mode2Message->UpdateDataItem("LrForce"+indexTag, forceData[LRWHEEL][index]);
            m_mode2Message->UpdateDataItem("RrForce"+indexTag, forceData[RRWHEEL][index]);
        }
        // Send the message to the test head
        SerialString_t message;
        status = m_mode2Message->GetMessage(message);
        if(BEP_STATUS_SUCCESS == status)
        {
            status = m_gen3Out->SendMessage(2, message.length(), message);
        }
        Log(LOG_FN_ENTRY, "Handled Mode 2 message request - %s", ConvertStatusToResponse(status).c_str());
    }
    else
    {   // Could not get speed or force data points
        Log(LOG_ERRORS, "Could not get speed or force data for Mode 2 message - speed: %s, force: %s",
            ConvertStatusToResponse(speedStatus).c_str(), ConvertStatusToResponse(forceStatus).c_str());
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::HandleMode4Message(const UINT8 &command, const UINT8 &commandValue)
{   
    BEP_STATUS_TYPE status  = BEP_STATUS_ERROR;
    SerialString_t message;
    message.push_back(command);
    string response;
    bool sendResponse = true;
    Log(LOG_FN_ENTRY, "Handling Mode 4 command: %d (%02X), value: %d (%02X)", command, command, commandValue, commandValue);
    // Handle the command only if the system is ready
    if(TestStandReady())
    {
        switch(command)
        {
        case Reset:   // Reset the machine
            // Reset the system
            sendResponse = false;
            status = ResetMachine(command);
            Log(LOG_DEV_DATA, "Reset system: %s", ConvertStatusToResponse(status).c_str());
            break;

        case TractionControlVehicle:
            Log(LOG_DEV_DATA, "Mode 4 setting traction control vehicle");
            status = BEP_STATUS_SUCCESS;
            break;

        case AbsVehicle:
            Log(LOG_ERRORS, "Mode 4 setting ABS vehicle");
            status = BEP_STATUS_SUCCESS;
            break;

        case ArmBrakeTest:
            Log(LOG_DEV_DATA, "Mode 4 Arming brake test");
            SetMode1MessageDataItem("BrakeAccelerateToTestSpeed", (INT16)1);
            SetMode1MessageDataItem("BrakeTestArmed", (INT16)1);
            SetMode1MessageDataItem("PromptNumber", PromptStartBrakeTest);
            status = BEP_STATUS_SUCCESS;
            break;

        case ExecuteDragTest:
            Log(LOG_DEV_DATA, "Mode 4 Executing Drag Test");
            SetMode1MessageDataItem("BrakeAnalyzeDragTest", (INT16)0);
            SetMode1MessageDataItem("PromptNumber", PromptDragTest);
            sendResponse = false;
            status = CommandBrakeTestObjective(GetDataTag("ObjectiveDragTest"), command);
            SetMode1MessageDataItem("PromptNumber", PromptNoPrompt);
            break;

        case TractionControlTest:
            Log(LOG_DEV_DATA, "Mode 4 Performing Traction Control Test");
            sendResponse = false;
            status = ProcessTractionControlCommand(commandValue, command);
            break;

        case BrakeTest:
            Log(LOG_DEV_DATA, "Mode 4 performing Brake Test");
            sendResponse = false;
            status = ProcessBaseBrakeTestCommand(commandValue, command);
            break;

        case AbsTest:
            Log(LOG_DEV_DATA, "Mode 4 performing ABS test");
            sendResponse = false;
            status = ProcessAbsTestCommand(commandValue, command);
            break;

        case QuickStop:
            Log(LOG_DEV_DATA, "Mode 4 performing quick stop");
            status = ConvertIntToBepStatus(m_motorControllerComm.Write(MOTOR_MODE, QUICK_BRAKE_MODE, response, true));
            break;

        case AxleRatio:
            Log(LOG_DEV_DATA, "Mode 4 performing axle ratio test");
            sendResponse = false;
            status = PerformAxleRatioTest(command);
            break;

        case FourWdVehicle:
            Log(LOG_DEV_DATA, "Mode 4 setting 4wd vehicle");
            status = ConvertIntToBepStatus(m_motorControllerComm.Write(MOTOR_MODE, TORQUE_MODE, response, true));
            break;

        case AllWdVehicle:
            Log(LOG_DEV_DATA, "Mode 4 setting AWD vehicle");
            m_isAwdVehicle = true;
            status = ConvertIntToBepStatus(m_motorControllerComm.Write(MOTOR_MODE, TORQUE_MODE, response, true));
            break;

        case MaxBrakeForceTest:
            Log(LOG_DEV_DATA, "Mode 4 setting max brake force test");
            sendResponse = false;
            if(commandValue == 1)
            {   // Clear the max brake force complete flag
                SetMode1MessageDataItem("MachineMaxBrakeForce", (INT16)0);
                SetMode1MessageDataItem("PromptNumber", PromptPeakBrakeTest);
                status = CommandBrakeTestObjective(GetDataTag("ObjectiveStartMaxBrake"), command);
            }
            else
            {   // End the max brake force test
                status = CommandBrakeTestObjective(GetDataTag("ObjectiveEndMaxBrake"), command);
                BposSleep(1000);
                SetMode1MessageDataItem("MachineMaxBrakeForce", (INT16)1);
                SetMode1MessageDataItem("PromptNumber", PromptNoPrompt);
            }
            break;

        case MaxParkBrakeForceTest:
            Log(LOG_DEV_DATA, "Mode 4 performing max park brake force test");
            sendResponse = false;
            if(commandValue == 1)
            {   // Start the park brake test
                SetMode1MessageDataItem("BrakeParkBrake", (INT16)0);
                SetMode1MessageDataItem("PromptNumber", PromptStartParkBrakeTest);
                status = CommandBrakeTestObjective(GetDataTag("ObjectiveStartParkBrake"), command);
            }
            else
            {   // End the park brake test
                status = CommandBrakeTestObjective(GetDataTag("ObjectiveStopParkBrake"), command);
                BposSleep(1000);
                SetMode1MessageDataItem("BrakeParkBrake", (INT16)1);
                SetMode1MessageDataItem("PromptNumber", PromptParkBrakeTestComplete);
            }
            break;

        case BrakeRetest:
            status = ResetMachine(command);
            sendResponse = false;
            Log(LOG_DEV_DATA, "Brake Retest - Reset system: %s", ConvertStatusToResponse(status).c_str());
            break;

        case AbsReset:
            Log(LOG_DEV_DATA, "Mode 4 reset ABS");
            status = BEP_STATUS_SUCCESS;
            break;

        case PlcSoftReset:
            Log(LOG_DEV_DATA, "Mode 4 reset PLC");
            status = BEP_STATUS_SUCCESS;
            break;

        case DeclutchMachine:
            Log(LOG_DEV_DATA, "Mode 4 declutching machine");
			status = CommandTcaseTestObjective(GetDataTag("ObjectiveDeclutchMachine"), command);
            break;

        case ClutchMachine:
            Log(LOG_DEV_DATA, "Mode 4 clutching machine");
			status = CommandTcaseTestObjective(GetDataTag("ObjectiveClutchMachine"), command);
            break;

        case LowerRolls:
            Log(LOG_DEV_DATA, "Mode 4 lowering rolls");
            status = ConvertIntToBepStatus(m_ndb->Write(RAISE_ROLLS, "0", response, true));
            break;

        case SetDriveAxle:
            Log(LOG_DEV_DATA, "Mode 4 setting drive axle");
            if(commandValue & RearWheelDriveMask)
            {
                Log(LOG_DEV_DATA, "Setting drive axle = rear");
                status = ConvertIntToBepStatus(m_ndb->Write(DRIVE_AXLE_TAG, "Rear", response, true));
            }
            else if(commandValue & FrontWheelDriveMask)
            {
                Log(LOG_DEV_DATA, "Setting drive axle = Front");
                status = ConvertIntToBepStatus(m_ndb->Write(DRIVE_AXLE_TAG, "Front", response, true));
            }
            else if(!(commandValue & FourWheelDriveMask))
            {
                Log(LOG_DEV_DATA, "Setting drive axle for 4wd vehicle");
                status = ConvertIntToBepStatus(m_motorControllerComm.Write(MOTOR_MODE, TORQUE_MODE, response, true));
            }
            else
            {
                Log(LOG_ERRORS, "Unknown drive axle type: %d", commandValue);
                status = BEP_STATUS_FAILURE;
            }
            break;

        case DriveControl:
            Log(LOG_DEV_DATA, "Mode 4 setting drive control");
            status = ProcessDriveControlRequest(commandValue);
            break;

        case ApplyRollBrake:
            Log(LOG_DEV_DATA, "Mode 4 Applying roll brakes");
            status = ConvertIntToBepStatus(m_ndb->Write("ApplyRollBrakes", "1", response, true));
            break;

        case SelectWheelbase:
            Log(LOG_DEV_DATA, "Mode 4 selecting wheelbase");
            m_ndb->Write("ResetPLC", "1", response, true);
            m_resetTimer.Start();
            status = SelectWheelbasePosition(commandValue);
            break;

        case HornControl:
            Log(LOG_DEV_DATA, "Mode 4 setting horn to %s", commandValue == 1 ? "On" : "Off");
            status = ConvertIntToBepStatus(m_ndb->Write("HornSignal", commandValue == 1 ? "1" : "0", response, true));
            break;

        case StatusLightControl:
            Log(LOG_DEV_DATA, "Mode 4 turning on 90 percent complete lamp");
            status = ConvertIntToBepStatus(m_ndb->Write("Test90PercentComplete", "1", response, true));
            break;

        case PlcAux1Control:
            Log(LOG_DEV_DATA, "Mode 4 setting Plc aux 1 output to %s", commandValue == 1 ? "On" : "Off");
            status = ConvertIntToBepStatus(m_ndb->Write("TurnOnAuxillaryOutputOne", commandValue == 1 ? "1" : "0", 
                                                        response, true));
            break;

        case PlcAux2Control:
            Log(LOG_DEV_DATA, "Mode 4 setting Plc aux 2 output to %s", commandValue == 1 ? "On" : "Off");
            status = ConvertIntToBepStatus(m_ndb->Write("TurnOnAuxillaryOutputTwo", commandValue == 1 ? "1" : "0", 
                                                        response, true));
            break;

        case BoostToSpeed:
            Log(LOG_DEV_DATA, "Mode 4 Boosting wheels to speed");
            status = BoostToArmSpeed();
            break;

        case SetFrontWheelDrive:
            Log(LOG_DEV_DATA, "Mode 4 setting front wheel drive");
            status = ConvertIntToBepStatus(m_ndb->Write(DRIVE_AXLE_TAG, "Front", response, true));
            break;

        case SetRearWheelDrive:
            Log(LOG_DEV_DATA, "Mode 4 setting rear wheel drive");
            status = ConvertIntToBepStatus(m_ndb->Write(DRIVE_AXLE_TAG, "Rear", response, true));
            break;

        case ZeroWheelDistanceCntr:
            Log(LOG_DEV_DATA, "Mode 4 zeroing wheel distance counters");
            status = BEP_STATUS_SUCCESS;
            break;

        case SetRetRollPosition:
            Log(LOG_DEV_DATA, "Mode 4 setting ret roll position");
            status = SetRetainingRollPosition(commandValue);
            break;

        default:
            Log(LOG_ERRORS, "Unknown Mode 4 command: %d (%02X)", command, command);
            status = BEP_STATUS_FAILURE;
            break;
        }
    }
    else
    {   // Machine is resetting, cannot process command
        Log(LOG_ERRORS, "Machine reset in progress, cannot process mode 4 command %d (%02X)", command, command);
        message.push_back(DvtNAK);
    }
    // Determine if the response needs to be sent
    if(sendResponse)
    {
        message.push_back(BEP_STATUS_SUCCESS == status ? DvtACK : DvtNAK);
        // Send the response to the test head
        status = m_gen3Out->SendMessage(4, message.size(), message); 
    }
    // Return the status
    Log(LOG_FN_ENTRY, "Handled Mode 4 command %d: %s", command, ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::HandleMode6Message(const INT16 &parameterNumber, const INT16 &parameterValue)
{
    Log(LOG_FN_ENTRY, "Handling Mode 6 message request - param #: %d, val: %d", parameterNumber, parameterValue);
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t message;
    if(TestStandReady() && parameterNumber && (parameterNumber < (INT16)m_keyboardParams.size()))
    {   // Update the parameter if it is different
        SetMode1MessageDataItem("PromptNumber", PromptParamDownloadInProgress);
        if(parameterValue != m_keyboardParams[parameterNumber])
        {   // New value
            Log(LOG_DEV_DATA, "Updating parameter %d from %d to %d", 
                parameterNumber, m_keyboardParams[parameterNumber], parameterValue);
            m_keyboardParams[parameterNumber] = parameterValue;
            SetMode1MessageDataItem(FindParameterName(parameterNumber), parameterValue);
        }
        message.clear();
        SetMode1MessageDataItem("PromptNumber", PromptParamDownloadComplete);
    }
    else
    {   // Invalid parameter requested
        Log(LOG_ERRORS, "Could not update parameter %d (Test stand ready: %s, Total params: %d)",
            parameterNumber, TestStandReady() ? "True" : "False", m_keyboardParams.size());
        status = BEP_STATUS_FAILURE;
        message = DvtNAK;
    }
    // Send the response
    status = m_gen3Out->SendMessage(6, message.length(), message);
    Log(LOG_FN_ENTRY, "Handled Mode 6 parameter update: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::HandleMode7Message(const INT16 &markerType, const INT16 &markerName)
{
    Log(LOG_FN_ENTRY, "Handling Mode 7 mesage request - Marker Type: %d, Marker Name: %d", markerType, markerName);
    SerialString_t message;
    message.clear();
    BEP_STATUS_TYPE status = m_gen3Out->SendMessage(7, message.length(), message);
    // Store the marker
    SetDvtMarker(markerType, markerName);
    Log(LOG_FN_ENTRY, "Handled Mode 7 DVT Marker: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::HandleMode20Message(const INT16 &testResultType)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "Handling Mode 20 message request - test result id: %d", testResultType);
    SerialString_t message;
    status = m_mode20Messages[testResultType]->GetMessage(message);
    if(BEP_STATUS_SUCCESS == status)
    {
        status = m_gen3Out->SendMessage(20, message.length(), message);
    }
    Log(LOG_FN_ENTRY, "Handled Mode 20 test results: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::HandleMode42Message(const vector<UINT8> &scheduledMessages)
{   // Start by sending the ACK to the test head
    SerialString_t message;
    message.clear();
    Log(LOG_FN_ENTRY, "Handling Mode 42 message request - scheduling %d Mode 1 messages", scheduledMessages.size());
    BEP_STATUS_TYPE status = m_gen3Out->SendMessage(42, message.length(), message);
    // Clear the message schedule
    bool msgScheduleActive = true;
    bool msgScheduleInactive = false;
    MessageScheduleActive(&msgScheduleInactive);
    if(EOK == m_msgScheduleMutex.Acquire())
    {
        m_messageSchedule.clear();
        // Assign the new message schedule and activate
        m_messageSchedule.assign(scheduledMessages.begin(), scheduledMessages.end());
        Log(LOG_DEV_DATA, "Mode 42 Scheduled %d Mode 1 messages", m_messageSchedule.size());
        m_currentScheduledMessage = 0;
        MessageScheduleActive(&msgScheduleActive);
        m_msgScheduleMutex.Release();
    }
    Log(LOG_FN_ENTRY, "Handled Mode 42 message schedule: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetupIcmDataTimer(const XmlNode *config)
{
    UINT64 icmDataUpdateRate = 0;
    bool startTimer = false;
    try
    {
        icmDataUpdateRate = atol(config->getChild("DataUpdateRate")->getValue().c_str());
        startTimer = true;
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "ICM data update rate not specified, not updating ICM data to DVT! - %s", excpt.GetReason());
        icmDataUpdateRate = 0;
        startTimer = false;
    }
    // Start the timer if update rate was specified
    if(startTimer)
    {
        m_icmDataTimer.SetPulseCode(GEN3_PULSE_CODE);
        m_icmDataTimer.SetPulseValue(ICM_DATA_PULSE);
        m_icmDataTimer.Initialize(GetProcessName(), mSEC_nSEC(icmDataUpdateRate), mSEC_nSEC(icmDataUpdateRate));
        m_icmDataTimer.Stop();
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetupScheduledMsgTimer(const XmlNode *config)
{
    UINT64 messageRate = 0;
    bool startTimer = false;
    try
    {
        messageRate = atol(config->getChild("ScheduledMessageDelay")->getValue().c_str());
        startTimer = true;
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Scheduled message delay not specified, not sending scheduled messages to test head: %s", excpt.GetReason());
        messageRate = 0;
        startTimer = false;
    }
    // Start the timer if the message delay was defined
    if(startTimer)
    {
        m_scheduledMessageTimer.SetPulseCode(GEN3_PULSE_CODE);
        m_scheduledMessageTimer.SetPulseValue(SCHEDULED_MSG_PULSE);
        m_scheduledMessageTimer.Initialize(GetProcessName(), mSEC_nSEC(messageRate), mSEC_nSEC(messageRate));
        m_scheduledMessageTimer.Stop();
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetupIncomingMessageTimer(const XmlNode *config)
{
    UINT64 timerRate = 0;
    bool startTimer = false;
    try
    {
        timerRate = atol(config->getChild("IncomingMessageCheckDelay")->getValue().c_str());
        startTimer = true;
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Incoming message check delay not defined, not starting timer: %s", excpt.GetReason());
        timerRate = 0;
        startTimer = false;
    }
    // Start the timer only if the delay has been specified
    if(startTimer)
    {
        m_incomingMsgTimer.SetPulseCode(GEN3_PULSE_CODE);
        m_incomingMsgTimer.SetPulseValue(INCOMING_MSG_PULSE);
        m_incomingMsgTimer.Initialize(GetProcessName(), mSEC_nSEC(timerRate), mSEC_nSEC(timerRate));
        m_incomingMsgTimer.Stop();
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetupSystemDataTimer(const XmlNode *config)
{
    UINT64 timerRate = 0;
    bool startTimer = false;
    try
    {
        timerRate = atol(config->getChild("SystemDataUpdateDelay")->getValue().c_str());
        startTimer = true;
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "System data update delay not defined, using 1000ms: %s", excpt.GetReason());
        timerRate = 1000;
        startTimer = true;
    }
    // Start the timer only if the delay has been specified
    if(startTimer)
    {
        m_systemDataTimer.SetPulseCode(GEN3_PULSE_CODE);
        m_systemDataTimer.SetPulseValue(SYSTEM_DATA_UPDATE_PULSE);
        m_systemDataTimer.Initialize(GetProcessName(), mSEC_nSEC(timerRate), mSEC_nSEC(timerRate));
        m_systemDataTimer.Stop();
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetupResetTimer(const XmlNode *config)
{
    UINT64 timerRate = 0;
    bool startTimer = false;
    try
    {
        timerRate = atol(config->getChild("ResetSystemDelay")->getValue().c_str());
        startTimer = true;
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Reset System delay not defined, using 1000ms: %s", excpt.GetReason());
        timerRate = 1000;
        startTimer = true;
    }
    // Start the timer only if the delay has been specified
    if(startTimer)
    {
        m_resetTimer.SetPulseCode(GEN3_PULSE_CODE);
        m_resetTimer.SetPulseValue(RESET_SYSTEM_PULSE);
        m_resetTimer.Initialize(GetProcessName(), mSEC_nSEC(timerRate), mSEC_nSEC(timerRate));
        m_resetTimer.Stop();
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetupWheelbaseTimer(const XmlNode *config)
{
    UINT64 timerRate = 0;
    bool startTimer = false;
    try
    {
        timerRate = atol(config->getChild("WheelbaseInPositionTimeout")->getValue().c_str());
        startTimer = true;
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Wheelbase position timer not defined, using 10000ms: %s", excpt.GetReason());
        timerRate = 10000;
        startTimer = true;
    }
    // Start the timer only if the delay has been specified
    if(startTimer)
    {
        m_wheelbaseTimer.SetPulseCode(GEN3_PULSE_CODE);
        m_wheelbaseTimer.SetPulseValue(WHEELBASE_TIMER_PULSE);
        m_wheelbaseTimer.Initialize(GetProcessName(), mSEC_nSEC(timerRate), mSEC_nSEC(timerRate));
        m_wheelbaseTimer.Stop();
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::Setup40kmhButtonLatchTimer(const XmlNode *config)
{
    UINT64 timerRate = 0;
    try
    {
        timerRate = atol(config->getChild("FortyKmhButtonLatchTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "40km/h button press latch timer not defined, using 2000ms: %s", excpt.GetReason());
        timerRate = 2000;
    }
    // Setup the timer information
    m_40kmhButtonLatchTimer.SetPulseCode(GEN3_PULSE_CODE);
    m_40kmhButtonLatchTimer.SetPulseValue(FORTY_KMH_BUTTON_PULSE);
    m_40kmhButtonLatchTimer.Initialize(GetProcessName(), mSEC_nSEC(timerRate), mSEC_nSEC(timerRate));
    m_40kmhButtonLatchTimer.Stop();
}

//-----------------------------------------------------------------------------
void Gen3Interface::InitializeMode1Messages(const XmlNode *msgConfig)
{
    const XmlNodeMap &messages = msgConfig->getChildren();
    UINT8 msgNumber = 0;
    Log(LOG_DEV_DATA, "Initializing %d Mode 1 messages", messages.size());
    for(XmlNodeMapCItr iter = messages.begin(); iter != messages.end(); iter++)
    {   // Create a new message object for each defined message
        Gen3Message *msg = new Gen3Message();
        msg->Initialize(iter->second);
        msg->UpdateDataItem("MessageNumber", msgNumber);
        m_mode1Messages.push_back(msg);
        msgNumber++;   // Update for the next message
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::InitializeMode1MessageData(void)
{
    for(SubscriptionListIter iter = m_subscribeTags.begin(); iter != m_subscribeTags.end(); iter++)
    {
        string data = ReadSystemDataString(*iter);
        // Need to invert if this is wheelbase in position.  GM uses this as wheelbase adjusting and should be inverse
        if(!(*iter).compare(WHEELBASE_IN_POSITION_TAG) && !data.compare("1"))  data = "0";
        // Update each message
        Log(LOG_DEV_DATA, "Setting %s:%s in Mode 1 messages", (*iter).c_str(), data.c_str());
        SetMode1MessageDataItem(*iter, data);
    }
    // Set the drive axle
    string driveAxle = ReadSystemDataString(DRIVE_AXLE_TAG);
    XmlNode axleData(DRIVE_AXLE_TAG, driveAxle);
    Log(LOG_DEV_DATA, "Setting drive axle to %s", driveAxle.c_str());
    Publish(&axleData);
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::BuildMode1Message22(const UINT16 &startIndex)
{
    IcmDataArray speedData, forceData;
    BEP_STATUS_TYPE speedStatus = m_icmData->ReadSpeedArray(startIndex-m_testStartIndex, 
                                                            (startIndex-m_testStartIndex)+10, 
                                                            speedData);
    BEP_STATUS_TYPE forceStatus = m_icmData->ReadForceArray(startIndex-m_testStartIndex, 
                                                            (startIndex-m_testStartIndex)+10, 
                                                            forceData);
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if((BEP_STATUS_SUCCESS == speedStatus) && (BEP_STATUS_SUCCESS == forceStatus))
    {
        m_mode1Messages[22]->UpdateDataItem("MemoryIndex", (INT16)startIndex);
        // Add the forces and speeds to the message
        char buff[8];
        for(INT16 index = 0; index < 10; index++)
        {
            string indexTag(CreateMessage(buff, sizeof(buff), "%d", index));
            m_mode1Messages[22]->UpdateDataItem("LfSpeed"+indexTag, speedData[LFWHEEL][index]);
            m_mode1Messages[22]->UpdateDataItem("RfSpeed"+indexTag, speedData[RFWHEEL][index]);
            m_mode1Messages[22]->UpdateDataItem("LrSpeed"+indexTag, speedData[LRWHEEL][index]);
            m_mode1Messages[22]->UpdateDataItem("RrSpeed"+indexTag, speedData[RRWHEEL][index]);
            m_mode1Messages[22]->UpdateDataItem("LfForce"+indexTag, forceData[LFWHEEL][index]);
            m_mode1Messages[22]->UpdateDataItem("RfForce"+indexTag, forceData[RFWHEEL][index]);
            m_mode1Messages[22]->UpdateDataItem("LrForce"+indexTag, forceData[LRWHEEL][index]);
            m_mode1Messages[22]->UpdateDataItem("RrForce"+indexTag, forceData[RRWHEEL][index]);
        }
        status = BEP_STATUS_SUCCESS;
    }
    else
    {
        Log(LOG_ERRORS, "Failed to read speed/force data - speed status: %s, force status: %s",
            ConvertStatusToResponse(speedStatus).c_str(), ConvertStatusToResponse(forceStatus).c_str());
    }
    return status;
}

//-----------------------------------------------------------------------------
void Gen3Interface::InitializeMode2Message(const XmlNode *msgConfig)
{
    m_mode2Message = new Gen3Message();
    m_mode2Message->Initialize(msgConfig);
}

//-----------------------------------------------------------------------------
void Gen3Interface::InitializeMode6Message(const XmlNode *msgConfig)
{
    UINT8 paramCount = 0;
    try 
    {
        paramCount = BposReadInt(msgConfig->getChild("ParameterCount")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Parameter count not specified, using 40: %s", excpt.GetReason());
        paramCount = 40;
    }
    // Set the size of the parameter list
    m_keyboardParams.reserve(paramCount);
    m_keyboardParams.resize(paramCount);
    // Load the keyboard param indices
    LoadKeyboardParamIndices(msgConfig->getChild("Parameters"));
}

//-----------------------------------------------------------------------------
void Gen3Interface::InitializeMode7Message(const XmlNode *msgConfig)
{
    try
    {
        m_markerTypeTable.DeepCopy(msgConfig->getChild("MarkerTranslationTable/TypeTranslation")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No type translation specified: %s", excpt.GetReason());
        m_markerTypeTable.clear(true);
    }
    try
    {
        m_markerNameTable.DeepCopy(msgConfig->getChild("MarkerTranslationTable/NameTranslation")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No name translation specified: %s", excpt.GetReason());
        m_markerNameTable.clear(true);
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::InitializeMode20Message(const XmlNode *msgConfig)
{
    const XmlNodeMap &messages = msgConfig->getChildren();
    UINT8 msgNumber = 0;
    Log(LOG_DEV_DATA, "Initializing %d Mode 20 messages", messages.size());
    for(XmlNodeMapCItr iter = messages.begin(); iter != messages.end(); iter++)
    {   // Create a new message object for each defined message
        Gen3Message *msg = new Gen3Message();
        msg->Initialize(iter->second);
        msg->UpdateDataItem("TestResultId", msgNumber);
        m_mode20Messages.push_back(msg);
        msgNumber++;   // Update for the next message
    }
}

//-----------------------------------------------------------------------------
UINT8 Gen3Interface::RollerCount(const UINT8 *rollerCount /*= NULL*/)
{
    if(rollerCount != NULL)  m_rollerCount = *rollerCount;
    return m_rollerCount;
}

//-----------------------------------------------------------------------------
const long& Gen3Interface::DvtMessageTimeout(const long *timeout /*= NULL*/)
{
    if(timeout != NULL)  m_dvtMsgTimeout = *timeout;
    return m_dvtMsgTimeout;
}

//-----------------------------------------------------------------------------
inline const long& Gen3Interface::DvtEndOfMessageGapTime(const long *gapTime /* = NULL*/)
{
    if(gapTime != NULL)   m_dvtMsgEndWaitTime = *gapTime;
    return m_dvtMsgEndWaitTime;
}

//-----------------------------------------------------------------------------
INT32 Gen3Interface::UpdateIcmData(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the ICM data
    float  forces[RollerCount()];
    float  speeds[RollerCount()];
    float  distances[RollerCount()];
    BEP_STATUS_TYPE forceStat = m_icmData->GetForces(forces);
    if(BEP_STATUS_SUCCESS != forceStat)  Log(LOG_ERRORS, "Could not read ICM forces: %s", ConvertStatusToResponse(forceStat).c_str());
    BEP_STATUS_TYPE speedStat = m_icmData->GetWheelSpeeds(speeds);
    if(BEP_STATUS_SUCCESS != speedStat)  Log(LOG_ERRORS, "Could not read ICM speeds: %s", ConvertStatusToResponse(speedStat).c_str());
    BEP_STATUS_TYPE distStat  = m_icmData->GetWheelDistances(distances);
    if(BEP_STATUS_SUCCESS != distStat)  Log(LOG_ERRORS, "Could not read ICM distances: %s", ConvertStatusToResponse(distStat).c_str());
    status = ((BEP_STATUS_SUCCESS == forceStat) && 
              (BEP_STATUS_SUCCESS == speedStat) && (BEP_STATUS_SUCCESS == distStat)) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
    // Get the current ICM index
    INT32 icmIndex = m_icmData->GetCurrentArrayIndex() - m_testStartIndex;
    if(BEP_STATUS_SUCCESS == status)
    {   // Update the data in the various messages
        for(UINT8 index = 0; index < m_mode1Messages.size(); index++)
        {
            m_mode1Messages[index]->UpdateIcmDataLocations(forces, speeds, distances, RollerCount(), icmIndex);
        }
        // Check for brake force high/low limits
        string dataValue = "0";
        if((forces[LFWHEEL] < m_keyboardParams[GetKeyboardParameterIndex("MinForceFaultFront")]) ||
           (forces[RFWHEEL] < m_keyboardParams[GetKeyboardParameterIndex("MinForceFaultFront")]))
        {   // Set the front min force fault
            dataValue = "1";
        }
        m_mode1Messages[0]->UpdateDataItem("FrontForceMinFault", dataValue);
        dataValue = "0";
        if((forces[LFWHEEL] > m_keyboardParams[GetKeyboardParameterIndex("MaxForceFaultFront")]) ||
           (forces[RFWHEEL] > m_keyboardParams[GetKeyboardParameterIndex("MaxForceFaultFront")]))
        {   // Set the front max force fault
            dataValue = "1";
            SetMode1MessageDataItem("PromptNumber", PromptFrontForceHighFault);
        }
        m_mode1Messages[0]->UpdateDataItem("FrontForceMaxFault", dataValue);
        dataValue = "0";
        if((forces[LRWHEEL] < m_keyboardParams[GetKeyboardParameterIndex("MinForceFaultRear")]) ||
           (forces[RRWHEEL] < m_keyboardParams[GetKeyboardParameterIndex("MinForceFaultRear")]))
        {   // Set the rear min force fault
            dataValue = "1";
        }
        m_mode1Messages[0]->UpdateDataItem("RearForceMinFault", dataValue);
        dataValue = "0";
        if((forces[LFWHEEL] > m_keyboardParams[GetKeyboardParameterIndex("MaxForceFaultRear")]) ||
           (forces[RFWHEEL] > m_keyboardParams[GetKeyboardParameterIndex("MaxForceFaultRear")]))
        {   // Set the rear max force fault
            dataValue = "1";
            SetMode1MessageDataItem("PromptNumber", PromptRearForceHighFault);
        }
        DataAnalysis analyze;
        m_mode1Messages[0]->UpdateDataItem("RearForceMaxFault", dataValue);
        float maxAbsoluteForce = analyze.FindMaxValue(forces, RollerCount());
        m_mode1Messages[0]->UpdateDataItem("AbsoluteForce", (INT16)maxAbsoluteForce);
        // Determine if the neutral speed has been achieved
        dataValue = (m_keyboardParams[GetKeyboardParameterIndex("NeutralSpeed")] < 
                     analyze.FindMaxValue(speeds, RollerCount())) ? "1" : "0";
        SetMode1MessageDataItem("AldlNeutralSpeedAchieved", dataValue);
        // check if max speed has been exceeded
        dataValue = (m_keyboardParams[GetKeyboardParameterIndex("MaximumMachineSpeed")] <
                     analyze.FindMaxValue(speeds, RollerCount())) ? "1" : "0";
        SetMode1MessageDataItem("MaxMachineSpeedFault", dataValue);
        if(!dataValue.compare("1"))  SetMode1MessageDataItem("PromptNumber", PromptOverSpeed);
    }
    else
    {
        Log(LOG_ERRORS, "Could not get ICM data: %s", ConvertStatusToResponse(status).c_str());
    }
    return status;
}

//-----------------------------------------------------------------------------
INT32 Gen3Interface::UpdateSystemDataItems(void)
{   // Read each item from the system and update in the mode 1 messages
    string value; 
    string response;
    static UINT16 wheelbaseCounter = 0;
    static UINT8 testInProgress = 0;
    bool wheelbaseInPosition = false;
    // Determine if the wheelbase counter should be incremented
    if(m_wheelbaseAdjusting)  wheelbaseCounter++;
    // Update system data items
    for(XmlNodeMapItr iter = m_systemDataMonitorItems.begin(); iter != m_systemDataMonitorItems.end(); iter++)
    {
        value = ReadSystemDataString(iter->second->getName());
        if(iter->second->getName().compare(WHEELBASE_IN_POSITION_TAG))
        {   // This is not the wheelbase in position tag, update normally
            SetMode1MessageDataItem(iter->second->getName(), value);
            Log(LOG_DETAILED_DATA, "Updating system data - %s:%s", iter->second->getName().c_str(), value.c_str());
        }
        else
        {   // Determine if we need to turn off the wheelbase start command
            if(wheelbaseCounter > WheelbaseAdjustStartDelay())
            {   // Check if we need to update the move command
                if(ReadSystemData(WHEELBASE_MOVE_TAG) == 1)   m_ndb->Write(WHEELBASE_MOVE_TAG, "0", response, true);
                // Determine if the adjusting flags need to be updated
                if(!iter->second->getName().compare(WHEELBASE_IN_POSITION_TAG) && !value.compare("1") && m_wheelbaseAdjusting)
                {
                    SetMode1MessageDataItem(WHEELBASE_IN_POSITION_TAG, "0");  // This is wheelbase adjusting flag
                    SetCurrentWheelbasePosition();
                    m_wheelbaseAdjusting = false;
                    wheelbaseInPosition = true;
                    wheelbaseCounter = 0;
                    // Make sure the PLC reset flag is cleared
                    m_ndb->Write("ResetPLC", "0", response, true);
                }
            }
        }
    }
    // Check for change in tet in progress
    UINT8 currentTestInProgressValue = ReadSystemData(TEST_IN_PROG_DATA_TAG);
    if(currentTestInProgressValue != testInProgress)
    {   // Change in test in progress status
        Log(LOG_DEV_DATA, "Updating test in progress from %d to %d", testInProgress, currentTestInProgressValue);
        testInProgress = currentTestInProgressValue;
        if(testInProgress)
        {
            m_testStartIndex = m_icmData->TagArray("StartTest");
            SetMode1MessageDataItem("PromptNumber", PromptWaitingForTestCommands);
        }
        else
        {
            SetMode1MessageDataItem("PromptNumber", PromptTestComplete);
        }
    }
    // Update Machine ready to test bit
    bool rollsUp = atob(ReadSystemDataString(ROLLS_UP_DATA_TAG).c_str());
    bool vehiclePresent = atob(ReadSystemDataString(VEH_PRESENT_DATA_TAG).c_str());
    SetMode1MessageDataItem("MachineReadyToTest", (rollsUp && vehiclePresent && !m_wheelbaseAdjusting && wheelbaseInPosition) ? "1" : "0");
    return BEP_STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
INT32 Gen3Interface::SendScheduledMessage(void)
{   // Make sure the schedule is active
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if(EOK == m_msgScheduleMutex.Acquire())
    {
        if(MessageScheduleActive() && !m_messageSchedule.empty())
        {   // Make sure the current index is valid
            if(m_currentScheduledMessage >= m_messageSchedule.size()) m_currentScheduledMessage = 0;
            // Determine if there is a message to be sent
            if(m_messageSchedule[m_currentScheduledMessage] != 0xFF)
            {   // Send the message
                SerialString_t message;
                status = m_mode1Messages[m_messageSchedule[m_currentScheduledMessage]]->GetMessage(message);
                if(BEP_STATUS_SUCCESS == status)
                {   // Send the message and update the index
                    status = m_gen3Out->SendMessage(1, message.length(), message);
                    m_currentScheduledMessage = (BEP_STATUS_SUCCESS == status) ? m_currentScheduledMessage+1 : m_currentScheduledMessage;
                }
            }
            else
            {   // No message scheduled for this period, increment to the next message
                m_currentScheduledMessage++;
                status = BEP_STATUS_SUCCESS;
            }
        }
        else
        {   // SChedule not active, so OK
            status = BEP_STATUS_SUCCESS;
        }
        m_msgScheduleMutex.Release();
    }
    return status;
}

//-----------------------------------------------------------------------------
INT32 Gen3Interface::CheckForIncomingMessage(void)
{
    INT32 retStat = BEP_STATUS_ERROR;
    // Check if there are any messages to process
    if(!m_incomingMessageQueue.empty())
    {
        SerialString_t message;
        if(EOK == m_incomingMsgQueueMutex.Acquire())
        {
            message = m_incomingMessageQueue.front();
            m_incomingMessageQueue.pop_front();
            m_incomingMsgQueueMutex.Release();
        }
        retStat = m_gen3In->ProcessMessage(message);
    }
    else
    {
        retStat = BEP_STATUS_SUCCESS;
    }
    return retStat;
}

//-----------------------------------------------------------------------------
INT32 Gen3Interface::ResetSystemPulseHandler(void)
{   // Turn off the Reset PLC bit
    string response;
    m_resetTimer.Stop();
    return m_ndb->Write("ResetPLC", "0", response, true);
}

//-----------------------------------------------------------------------------
INT32 Gen3Interface::WheelbaseTimerPulseHandler(void)
{
    string response;
    INT32 retVal = BEP_STATUS_ERROR;
    m_wheelbaseTimer.Stop();
    if(ReadSystemData(WHEELBASE_MOVE_TAG) == 0x01)
    {
        Log(LOG_DEV_DATA, "Wheelbase still moving after timer expired, forcing %s to 0", WHEELBASE_MOVE_TAG);
        retVal = m_ndb->Write(WHEELBASE_MOVE_TAG, "0", response, true);
    }
    else
    {
        retVal = BEP_STATUS_SUCCESS;
    }
    return retVal;
}

//-----------------------------------------------------------------------------
INT32 Gen3Interface::FortyKmhButtonPulseHandler(void)
{   // Stop the timer
    m_40kmhButtonLatchTimer.Stop();
    // Set the Mode 1 message items to clear the latched value
    SetMode1MessageDataItem(GetDataTag("Start40kphTag"), (INT16)0);
    Log(LOG_DEV_DATA, "Cleared 40km/h button press latch");
    return BEP_STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void Gen3Interface::LoadKeyboardParamIndices(const XmlNode *paramIndex)
{
    const XmlNodeMap &paramMap = paramIndex->getChildren();
    for(XmlNodeMapCItr iter = paramMap.begin(); iter != paramMap.end(); iter++)
    {
        m_keyboardParamsIndex.insert(KeyboardParameterMapVal(iter->second->getName(), 
                                                             BposReadInt(iter->second->getValue().c_str())));
        XmlNodeMapCItr attribIter = iter->second->getAttributes().find("Default");
        if(attribIter != iter->second->getAttributes().end())
        {
            m_keyboardParams[BposReadInt(iter->second->getValue().c_str())] = BposReadInt(attribIter->second->getValue().c_str());
            Log(LOG_DEV_DATA, "Set parameter %s to default value: %s", 
                iter->second->getName().c_str(), attribIter->second->getValue().c_str());
            SetMode1MessageDataItem(iter->second->getName(), (INT16)BposReadInt(attribIter->second->getValue().c_str()));
        }
        else
        {
            Log(LOG_DEV_DATA, "No default value for %s", iter->second->getName().c_str());
        }
    }
}

//-----------------------------------------------------------------------------
INT32 Gen3Interface::GetKeyboardParameterIndex(const string &parameter)
{
    KeyboardParameterMapIter iter = m_keyboardParamsIndex.find(parameter);
    return (iter != m_keyboardParamsIndex.end()) ? iter->second : -1;
}

//-----------------------------------------------------------------------------
string Gen3Interface::FindParameterName(const INT16 &parameterNum)
{
    string paramName;
    bool found = false;
    for(KeyboardParameterMapIter iter = m_keyboardParamsIndex.begin(); 
         (iter != m_keyboardParamsIndex.end()) && !found; 
         iter++)
    {
        if(iter->second == parameterNum)
        {
            paramName = iter->first;
            found = true;
        }
    }
    return paramName;
}

//-----------------------------------------------------------------------------
void Gen3Interface::LoadSoftwareVersion(void)
{
    string softwareId(ReadSystemDataString(SOFTWARE_ID));
    Log(LOG_DEV_DATA, "CCRT Software Version: %s", softwareId.c_str());
    // Get a pointer to the software revision information in the config
    const XmlNode *revisionData = m_config->getChild("Setup/MessageData/Messages/Mode1/Message12/SoftwareRevisionData");
    INT32 startIndex = BposReadInt(revisionData->getChild("CoreVersion")->getAttribute("StartIndex")->getValue().c_str());
    INT32 length = BposReadInt(revisionData->getChild("CoreVersion")->getAttribute("Length")->getValue().c_str());
    string coreVersion(softwareId.substr(startIndex, length));      // Core software version
    Log(LOG_DEV_DATA, "Extracted Core Version: %s", coreVersion.c_str());
    startIndex = BposReadInt(revisionData->getChild("ReleaseVersion")->getAttribute("StartIndex")->getValue().c_str());
    length = BposReadInt(revisionData->getChild("ReleaseVersion")->getAttribute("Length")->getValue().c_str());
    string releaseVerison(softwareId.substr(startIndex, length));   // Customer release revision
    Log(LOG_DEV_DATA, "Extracted Release Version: %s", releaseVerison.c_str());
    string machineType("M"+ReadSystemDataString(MACHINE_TYPE));
    Log(LOG_DEV_DATA, "Machine Type: %s", machineType.c_str());
    // Look up the code for our machine type
    const XmlNodeMap &machTypeCodes = m_config->getChild("Setup/MessageData/Messages/Mode1/Message12/MachineTypes")->getChildren();
    XmlNodeMapCItr iter = machTypeCodes.find(machineType);
    string machineCode((iter != machTypeCodes.end()) ? iter->second->getValue() : "0x00FF");
    Log(LOG_DEV_DATA, "Set machine code: %s", machineCode.c_str());
    // Store the PLC Code
    INT16 plcCode = FindPlcCode(ReadSystemDataString("PlcType"));
    Log(LOG_DEV_DATA, "Set PLC code: %d", plcCode);
    // Get the machine serial number
    UINT8 serialNumberLength = BposReadInt(m_config->getChild("Setup/MessageData/MachineSerialNumberLength")->getValue().c_str());
    string machSerialNum(getenv("MACHINE"));
    Log(LOG_DEV_DATA, "PLC Type: %s(%d), Machine Serial Number: %s", 
        ReadSystemDataString("PlcType").c_str(), plcCode, machSerialNum.c_str());
    // Determine if the serial number needs to be trimmed
    if(machSerialNum.length() > serialNumberLength)
    {   // Trim the serial number to the correct length
        machSerialNum = machSerialNum.substr(machSerialNum.length() - serialNumberLength, serialNumberLength);
        Log(LOG_DEV_DATA, "Trimmed serial number to: %s", machSerialNum.c_str());
    }
    // Update all messages with the data
    Log(LOG_DEV_DATA, "Setting base software: %s, revision number: %s, machine type code: %s",
        coreVersion.c_str(), releaseVerison.c_str(), machineCode.c_str());
    SetMode1MessageDataItem("BaseSoftwareVersion", coreVersion);
    SetMode1MessageDataItem("SoftwareRevisionLevel", releaseVerison);
    SetMode1MessageDataItem("MachineType", machineCode);
    SetMode1MessageDataItem("PlcCode", plcCode);
    SetMode1MessageDataItem("MachineSerialNumber", machSerialNum);
}

//-----------------------------------------------------------------------------
inline INT16 Gen3Interface::FindPlcCode(const string &plcType)
{
    XmlNodeMapItr iter = m_plcCodes.find(plcType);
    return (iter != m_plcCodes.end()) ? BposReadInt(iter->second->getValue().c_str()) : -1;
}

//-----------------------------------------------------------------------------
INT32 Gen3Interface::FindDvtMarker(const INT16 &markerType, const INT16 markerName)
{
    INT32 offset = 0;
    bool markerFound = false;
    // Search for the specified marker
    for(UINT16 index = 0; (index < m_dvtMarkers.size()) && !markerFound; index++)
    {   // Check if this is the marker we are looking for
        markerFound = ((markerType == m_dvtMarkers[index]->type) && (markerName == m_dvtMarkers[index]->name));
        offset = markerFound ? m_dvtMarkers[index]->index : 0;
    }
    Log(LOG_DEV_DATA, "FindDvtMarker(type: %d, name: %d) - %s, returning: %d",
        markerType, markerName, markerFound ? "Success" : "Not Found", offset);
    return offset;
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetDvtMarker(const INT16 &markerType, const INT16 &markerName)
{
    DvtMarker *marker = new DvtMarker();
    marker->type = markerType;
    marker->name = markerName;
    string icmTag(CreateIcmTag(markerType, markerName));
    marker->index = m_icmData->TagArray(icmTag);
    m_dvtMarkers.push_back(marker);
}

//-----------------------------------------------------------------------------
void Gen3Interface::ClearDvtMarkers(void)
{
    for(UINT8 index = 0; index < m_dvtMarkers.size(); index++)
    {   // Delete the current entry
        delete m_dvtMarkers[index];
    }
}

//-----------------------------------------------------------------------------
const bool& Gen3Interface::TestStandReady(const bool *ready /*= NULL*/)
{
    if(ready != NULL)  m_testStandReady = *ready;
    return m_testStandReady;
}

//-----------------------------------------------------------------------------
string Gen3Interface::CreateIcmTag(const INT16 &markerType, const INT16 &markerName)
{
    char buff[8];
    string type(CreateMessage(buff, sizeof(buff), "%02X", markerType));
    string name(CreateMessage(buff, sizeof(buff), "%02X", markerName));
    // Find the marker type translation
    XmlNodeMapItr typeIter = m_markerTypeTable.find("Type"+type);
    XmlNodeMapItr nameIter = m_markerNameTable.find("Name"+name);
    string mType(typeIter != m_markerTypeTable.end() ? typeIter->second->getValue() : "UndefType");
    string mName(nameIter != m_markerNameTable.end() ? nameIter->second->getValue() : "UndefName");
    string icmTag(mType + mName);
    return icmTag;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::ResetMachine(const UINT8 &command, bool resetBrakes)
{   // Inidicate the machine is resetting
    string response;
    bool ready(true), notReady(false);
    Log(LOG_DEV_DATA, "Resetting Machine");
    TestStandReady(&notReady);
    SetMode1MessageDataItem("ResetComplete", (INT16)0);
    SetMode1MessageDataItem("PromptNumber", PromptMachineResetting);
    // Reset the PLC
    m_ndb->Write("ResetPLC", "1", response, true);
    m_resetTimer.Start();
    // Clear out all DVT markers
    ClearDvtMarkers();
    // Clear the dvt markers
    m_dvtMarkers.clear();
    m_keyboardParams[GetKeyboardParameterIndex("FourWalTireCalibrationIndex")] = 0;  // Clear the tire parameter
    m_isAwdVehicle = false;
    // Update Mode 1 messages with correct data
    SetMode1MessageDataItem("TestCompleteStatus1", (UINT8)0x00);
    SetMode1MessageDataItem("TestCompleteStatus2", (UINT8)0x00);
    SetMode1MessageDataItem("TurnOnAuxillaryOutputOne", (UINT8)0x00);
    SetMode1MessageDataItem("TurnOnAuxillaryOutputTwo", (UINT8)0x00);
    // Initialize the brake test component
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    if(resetBrakes) status = CommandBrakeTestObjective(GetDataTag("ObjectiveInitialize"), command);
    // Clear all DVT markers
    ClearDvtMarkers();
    // Clear all Brake data in Mode 20 messages
    for(UINT8 index = 0; index < m_mode20Messages.size(); index++)
    {
        m_mode20Messages[index]->ClearMessageContents();
        m_mode20Messages[index]->UpdateDataItem("TestResultId", index);
    }
    // Turn off the 90% complete lamp
    status = ConvertIntToBepStatus(m_ndb->Write("Test90PercentComplete", "0", response, true));
    // Inidicate the machine is done resetting
    SetMode1MessageDataItem("WheelbasePositionSelected", (UINT8)0);
    TestStandReady(&ready);
    SetMode1MessageDataItem("ResetComplete", (INT16)1);
    SetMode1MessageDataItem("BrakeTestArmed", (INT16)0);
    // Enable motor boost
    m_motorControllerComm.Write(MOTOR_MODE, BOOST_MODE, response, true);
    SetMode1MessageDataItem("PromptNumber", PromptNoPrompt);
    Log(LOG_DEV_DATA, "Machine reset complete: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetMode1MessageDataItem(const string dataItem, const UINT8 &value)
{
    for(UINT8 index = 0; index < m_mode1Messages.size(); index++)
    {
        if(index == 21)   Log(LOG_DETAILED_DATA, "Updating Message 21 UINT8 item - %s:%02X", dataItem.c_str(), value);
        m_mode1Messages[index]->UpdateDataItem(dataItem, value);
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetMode1MessageDataItem(const string dataItem, const INT16 &value)
{
    for(UINT8 index = 0; index < m_mode1Messages.size(); index++)
    {
        if(index == 21)   Log(LOG_DETAILED_DATA, "Updating Message 21 INT16 item - %s:%d", dataItem.c_str(), value);
        m_mode1Messages[index]->UpdateDataItem(dataItem, value);
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetMode1MessageDataItem(const string dataItem, const string &value)
{
    for(UINT8 index = 0; index < m_mode1Messages.size(); index++)
    {
        if(index == 21)   Log(LOG_DETAILED_DATA, "Updating Message 21 string item - %s:%s", dataItem.c_str(), value.c_str());
        m_mode1Messages[index]->UpdateDataItem(dataItem, value);
    }
}

//-----------------------------------------------------------------------------
inline BEP_STATUS_TYPE Gen3Interface::CommandBrakeTestObjective(const string &objective, const UINT8 &command)
{
    string response;
    SerialString_t message;
    message.push_back(command);
    message.push_back(DvtACK);
    // Send the response to the test head
    m_gen3Out->SendMessage(4, message.size(), message); 
    BEP_STATUS_TYPE status = ConvertIntToBepStatus(m_brakeComponentComm.Command(objective, "", response, true));
    Log(LOG_FN_ENTRY, "Commanded brake test objective %s: %s", objective.c_str(), ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::CommandTcaseTestObjective(const string &objective, const UINT8& command)
{
	string response;
	SerialString_t message;
	message.push_back(command);
	message.push_back(DvtACK);
	// Send the ACK to the test head
	m_gen3Out->SendMessage(4, message.size(), message);
	// Now actually do the objective
	BEP_STATUS_TYPE status = ConvertIntToBepStatus(m_tcaseComponentComm.Command(objective, "", response, true));
	Log(LOG_FN_ENTRY, "Commanded t-case objective %s: %s", objective.c_str(), ConvertStatusToResponse(status).c_str());
	return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::ProcessTractionControlCommand(const UINT8 &commandValue, const UINT8 &command)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    switch(commandValue)
    {
    case EndTractionTest:
        Log(LOG_DEV_DATA, "Mode 4 Traction test complete");
        status = CommandBrakeTestObjective(GetDataTag("ObjectiveStopTractionTest"), command);
        // Wait a bit for the results
        BposSleep(1000);
        SetMode1MessageDataItem("BrakeTractionControlTest", (INT16)1);
        SetMode1MessageDataItem("PromptNumber", PromptNoPrompt);
        break;

    case Start2ChannelTracTest:
    case Start3ChannelTracTest:
    case Start4ChannelTracTest:
        Log(LOG_DEV_DATA, "Mode 4 Start traction test");
        SetMode1MessageDataItem("BrakeTractionControlTest", (INT16)0);
        SetMode1MessageDataItem("PromptNumber", PromptTractionTest);
        status = CommandBrakeTestObjective(GetDataTag("ObjectiveStartTractionTest"), command);
        break;

    default:
        Log(LOG_ERRORS, "Mode 4 Unknown traction command value: %d", commandValue);
        status = BEP_STATUS_FAILURE;
        break;
    }
    Log(LOG_FN_ENTRY, "Done processing traction control command: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::ProcessBaseBrakeTestCommand(const UINT8 &commandValue, const UINT8 &command)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    switch(commandValue)
    {
    case BrakeTestEnd:
        {
            Log(LOG_DEV_DATA, "Mode 4 Brake Test Complete");
            status = CommandBrakeTestObjective(GetDataTag("ObjectiveBrakeTestStop"), command);
            while(!m_brakeTestComplete) BposSleep(50);
            SetMode1MessageDataItem("BrakeStopBaseBrakeTest", (INT16)1);
            SetMode1MessageDataItem("PromptNumber", PromptNoPrompt);
        }
        break;

    case BrakeTestStart:
        Log(LOG_DEV_DATA, "Mode 4 Start brake test");
        SetMode1MessageDataItem("BrakeStopBaseBrakeTest", (INT16)0);
        SetMode1MessageDataItem("PromptNumber", m_isAwdVehicle ? PromptAwdBrakeTest : PromptBaseBrakeTest);
        m_brakeTestComplete = false;
        status = CommandBrakeTestObjective(GetDataTag("ObjectiveBrakeTestStart"), command);
        break;

    default:
        Log(LOG_ERRORS, "Unknown Mode 4 brake test command value: %d", commandValue);
        status = BEP_STATUS_FAILURE;
        break;
    }
    Log(LOG_FN_ENTRY, "Done processing base brake command: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::ProcessAbsTestCommand(const UINT8 &commandValue, const UINT8 &command)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    switch(commandValue)
    {
    case EndAbsTest:
        Log(LOG_DEV_DATA, "Mode 4 ABS test complete");
        status = CommandBrakeTestObjective(GetDataTag("ObjectiveAbsValveFiringEnd"), command);
        BposSleep(1000);
        SetMode1MessageDataItem("BrakeValveFiringTest", (INT16)1);  // Mark test complete status
        SetMode1MessageDataItem("PromptNumber", PromptNoPrompt);
        break;

    case StartClass2RwalAbs:
    case StartClass2AwdAbs:
    case StartClass23c4cAbs:
    case StartClass23chanWinAbs:
    case StartClass24chanWinAbs:
    case StartKhRwalAbs:
    case StartKh4walAbs:
        Log(LOG_DEV_DATA, "Mode 4 ABS test start");
        SetMode1MessageDataItem("BrakeValveFiringTest", (INT16)0);  // Mark test start status
        SetMode1MessageDataItem("PromptNumber", PromptAbsTest);
        status = CommandBrakeTestObjective(GetDataTag("ObjectiveAbsValveFiringStart"), command);
        break;

    default:
        Log(LOG_ERRORS, "Unknown ABS command: %d", commandValue);
        status = BEP_STATUS_FAILURE;
        break;
    }
    Log(LOG_FN_ENTRY, "Done processing ABS test command: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::PerformAxleRatioTest(const UINT8 &command)
{
    SetMode1MessageDataItem("MachineAxleRatio", (INT16)0);
    SetMode1MessageDataItem("PromptNumber", PromptAxleRatioTest);
    string response;
    SerialString_t message;
    message.push_back(command);
    message.push_back(DvtACK);
    // Send the response to the test head
    m_gen3Out->SendMessage(4, message.size(), message); 
    BEP_STATUS_TYPE status = ConvertIntToBepStatus(m_tcaseComponentComm.Command("AxleRatio", "", response, true));
    SetMode1MessageDataItem("PromptNumber", PromptAxleRatioTestComplete);
    Log(LOG_FN_ENTRY, "Commanded axle ratio objective: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::ProcessDriveControlRequest(const UINT8 &control)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string response;
    string startFront("0");
    string startRear("0");
    bool driveControlOk = true;
    switch(control & DriveControlMask)
    {
    case FrontDrivesOn:
        Log(LOG_DEV_DATA, "Setting front drives on and rear drivess off");
        startFront = "1";
        startRear  = "0";
        break;

    case RearDrivesOn:
        Log(LOG_DEV_DATA, "Setting front drives off and rear drivess on");
        startFront = "0";
        startRear  = "1";
        break;

    case FrontAndRearDrivesOn:
        Log(LOG_DEV_DATA, "Setting front drives on and rear drivess on");
        startFront = "1";
        startRear  = "1";
        break;

    default:
        Log(LOG_ERRORS, "Unknonwn drive control: %d - disabling front and rear drives", control);
        startFront = "0";
        startRear  = "0";
        status = BEP_STATUS_FAILURE;  // Ensure we do not mistakenly send an ACK
        break;
    }
    status = ConvertIntToBepStatus(m_ndb->Write("StartEBSSensorTest", startFront, response, false));
    status = ConvertIntToBepStatus(m_ndb->Write("StartRearEBSDrives", startRear, response, true));
    status = driveControlOk ? status : BEP_STATUS_FAILURE;
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::SelectWheelbasePosition(const UINT8 &wheelbase)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string positionTag("Position");
    bool clearWheelbase = false;
    char buff[8];
    // Always clear the wheelbases
    status = ClearAllWheelbaseSelections();
    switch(wheelbase)
    {
    case 0x00:   // Clear the wheelbase position
        Log(LOG_DEV_DATA, "Clearing all wheelbase selections");
        clearWheelbase = true;
        break;

    case 0x01:
        Log(LOG_DEV_DATA, "Setting wheelbase position 1", wheelbase);
        positionTag += "01";
        break;

    case 0x02:
        Log(LOG_DEV_DATA, "Setting wheelbase position 2");
        positionTag += "02";
        break;

    case 0x04:
        Log(LOG_DEV_DATA, "Setting wheelbase position 3");
        positionTag += "03";
        break;

    case 0x08:
        Log(LOG_DEV_DATA, "Setting wheelbase position 4");
        positionTag += "04";
        break;

    case 0x10:
        Log(LOG_DEV_DATA, "Setting wheelbase position 5");
        positionTag += "05";
        break;

    case 0x20:
        Log(LOG_DEV_DATA, "Setting wheelbase position 6");
        positionTag += "06";
        break;

    case 0x40:
        Log(LOG_DEV_DATA, "Setting wheelbase position 7");
        positionTag += "07";
        break;

    case 0x80:
        Log(LOG_DEV_DATA, "Setting wheelbase position 8");
        positionTag += "08";
        break;

    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
        Log(LOG_DEV_DATA, "Setting wheelbase position %d", wheelbase);
        positionTag += CreateMessage(buff, sizeof(buff), "%02d", wheelbase);
        break;

    default:
        Log(LOG_ERRORS, "Unsupported wheelbase position: %02X", wheelbase);
        status = BEP_STATUS_FAILURE;
        clearWheelbase = true;
    }
    // If wheelbases were not cleared, then 
    if(!clearWheelbase)
    {
        status = SetWheelbasePosition(positionTag);
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::ClearAllWheelbaseSelections(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    char buff[32];
    string response;
    for(UINT8 index = 1; index <= 12; index++)
    {
        string tag(CreateMessage(buff, sizeof(buff), "WheelbasePosition%02d", index));
        status = ConvertIntToBepStatus(m_ndb->Write(tag, "0", response, index == 12));
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::SetWheelbasePosition(const string &positionTag)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Make sure to use the latest wheelbase position file
    LoadWheelbasePositionTable();
    XmlNodeMapItr iter = m_wheelbasePositions.find(positionTag);
    if(iter != m_wheelbasePositions.end())
    {   // found the wheelbase, start adjusting
        SetMode1MessageDataItem(WHEELBASE_IN_POSITION_TAG, "1");
        // Clear the current wheelbase position
        SetMode1MessageDataItem("WheelbasePositionSelected", (UINT8)0x00);
        string positionVal = iter->second->getValue();
        Log(LOG_DEV_DATA, "Wheelbase position: %s mm", positionVal.c_str());
        string response;
        status = ConvertIntToBepStatus(m_ndb->Write(WHEELBASE_INCHESX10, positionVal, response, true));
        m_ndb->Write(WHEELBASE_MOVE_TAG, "1", response, true);
        m_wheelbaseAdjusting = true;
        m_wheelbaseTimer.Start();
    }
    else
    {
        status = BEP_STATUS_FAILURE;
        Log(LOG_ERRORS, "Could not find %s in wheelbase table", positionTag.c_str());
    }
    return status;
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetCurrentWheelbasePosition(void)
{
    string currentWheelbase = ReadSystemDataString(WHEELBASE_INCHESX10);
    bool found = false;
    char buff[32];
    string response;
    for(XmlNodeMapItr iter = m_wheelbasePositions.begin(); (iter != m_wheelbasePositions.end()) && !found; iter++)
    {   // Check if this is the current wheelbase position
        if(!currentWheelbase.compare(iter->second->getValue()))
        {   // Found the correct wheelbase, set the data
            UINT8 value = (UINT8)BposReadInt(iter->second->getAttribute("value")->getValue().c_str());
            SetMode1MessageDataItem("WheelbasePositionSelected", value);
            found = true;
            Log(LOG_DEV_DATA, "Found wheelbase position (%s), setting wheelbase flags to %02X", 
                iter->second->getName().c_str(), value);
            string tag(CreateMessage(buff, sizeof(buff), "WheelbasePosition%02d", value));
            m_ndb->Write(tag, "1", response, false);
            Log(LOG_DEV_DATA, "Set %s in PLC", tag.c_str());
        }
    }
    Log(LOG_DEV_DATA, "Current wheelbase: %s, position index found: %s", currentWheelbase.c_str(), found ? "True" : "False");
    // Turn off the start wheelbase command
    m_ndb->Write(WHEELBASE_MOVE_TAG, "0", response, true);
    // Write to PLC indicating that Exhaust door can be raised because WbInPos
    BposSleep(500);
    m_ndb->Write("StartLamp", "1", response, true);
    BposSleep(500);
    m_ndb->Write("StartLamp", "0", response, true);
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::BoostToArmSpeed(void)
{
    float currentSpeed[RollerCount()];
    BEP_STATUS_TYPE status = m_icmData->GetWheelSpeeds(currentSpeed);
    if((currentSpeed[LFWHEEL] > 30.0) && (currentSpeed[RFWHEEL] > 30.0) &&
       (currentSpeed[LRWHEEL] > 30.0) && (currentSpeed[RRWHEEL] > 30.0))
    {   // Set motor mode to speed mode
        string response;
        status = ConvertIntToBepStatus(m_motorControllerComm.Write("LeftFrontMotorMOde", SPEED_MODE, response, false));
        status = ConvertIntToBepStatus(m_motorControllerComm.Write("RightFrontMotorMOde", SPEED_MODE, response, false));
        status = ConvertIntToBepStatus(m_motorControllerComm.Write("LeftRearMotorMOde", SPEED_MODE, response, false));
        status = ConvertIntToBepStatus(m_motorControllerComm.Write("RightRearMotorMOde", SPEED_MODE, response, false));
        // Set the speed value for the rollers to accel to
        char buff[8];
        string speed = CreateMessage(buff, sizeof(buff), "%d", m_keyboardParams[GetKeyboardParameterIndex("BoostToArm")]);
        status = ConvertIntToBepStatus(m_motorControllerComm.Write("LeftFrontSpeedValue", speed, response, false));
        status = ConvertIntToBepStatus(m_motorControllerComm.Write("RightFrontSpeedValue", speed, response, false));
        status = ConvertIntToBepStatus(m_motorControllerComm.Write("LeftRearSpeedValue", speed, response, false));
        status = ConvertIntToBepStatus(m_motorControllerComm.Write("RightRearSpeedValue", speed, response, true));
    }
    else
    {
        Log(LOG_ERRORS, "Wheel speeds are below 30mph, cannot boost to speed");
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Interface::SetRetainingRollPosition(const UINT8 &height)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string setPosition("1");
    string unsetPosition("0");
    string highPosition, lowPosition, response;
    bool setRetRollPosition = true;
    switch(height)
    {
    case RetRollLowPosition:
        highPosition = unsetPosition;
        lowPosition = setPosition;
        break;

    case RetRollHighPosition:
        highPosition = setPosition;
        lowPosition = unsetPosition;
        break;

    default:
        Log(LOG_ERRORS, "Unknown ret roll position!");
        setRetRollPosition = false;
        status = BEP_STATUS_FAILURE;
        break;
    }
    if(setRetRollPosition)
    {
        status = ConvertIntToBepStatus(m_ndb->Write("RetRollHiPosition", highPosition, response, false));
        status = ConvertIntToBepStatus(m_ndb->Write("RetRollLoPosition", lowPosition, response, true));
        SetMode1MessageDataItem("RetRollHiPosition", (INT16)height);
    }
    return status;
}

//-----------------------------------------------------------------------------
UINT8 Gen3Interface::ReadSystemData(const string &dataTag)
{
    string response;
    UINT8 value = 0x00;
    if(BEP_STATUS_SUCCESS == m_ndb->Read(dataTag, response, true))
    {
        string strVal;
        m_ndb->GetByTag(dataTag, strVal, response);
        value = BposReadInt(strVal.c_str());
    }
    else
    {
        value = 0xFF;
        Log(LOG_ERRORS, "Could not read %s from the system, returning %02X", dataTag.c_str(), value);
    }
    return value;
}

//-----------------------------------------------------------------------------
string Gen3Interface::ReadSystemDataString(const string &dataTag)
{
    string response;
    string value("Unknown");
    if(BEP_STATUS_SUCCESS == m_ndb->Read(dataTag, response, true))
    {
        string strVal;
        m_ndb->GetByTag(dataTag, strVal, response);
        value = strVal;
    }
    else
    {
        value = "Oops";
        Log(LOG_ERRORS, "Could not read %s from the system, returning %s", dataTag.c_str(), value.c_str());
    }
    return value;
}

//-----------------------------------------------------------------------------
const bool& Gen3Interface::MessageScheduleActive(const bool *active /*= NULL*/)
{
    if(active != NULL)  m_activateMessageSchedule = *active;
    return m_activateMessageSchedule;
}

//-----------------------------------------------------------------------------
void Gen3Interface::SetMode20BrakeStatData(const XmlNode *data)
{   // Get a list of the test stat items
    const XmlNodeMap &stats = data->getChildren();
    for(XmlNodeMapCItr iter = stats.begin();  iter != stats.end(); iter++)
    {   // Create a tag
        string dataItem(data->getName() + iter->second->getName());
        INT16 dataValue = BposReadInt(iter->second->getValue().c_str());
        // Update the data item in each of the mode 20 messages
        for(UINT8 index = 0; index < m_mode20Messages.size(); index++)
        {
            m_mode20Messages[index]->UpdateDataItem(dataItem, dataValue);
        }
    }
}

//-----------------------------------------------------------------------------
void Gen3Interface::UpdateMachineDtcList(const XmlNode *faultList)
{   // Clear all stored machine faults
    char buff[32];
    for(UINT8 index = 0; index < MaxMachineFaults(); index++)
    {
        SetMode1MessageDataItem(CreateMessage(buff, sizeof(buff), "MachineDtc%d", index), (INT16)0);
    }
    // Determine if there are any faults to be set
    if(faultList->getChildren().size() > 0)
    {   // Faults need to be stored
        UINT8 index = 1;
        for(XmlNodeMapCItr iter = faultList->getChildren().begin(); iter != faultList->getChildren().end(); iter++, index++)
        {   // Look up the fault code and set the data in the mode 1 messages
            SetMode1MessageDataItem(CreateMessage(buff, sizeof(buff), "MachineDtc%d", index), 
                                    GetFaultCode(iter->second->getName()));
        }
    }
}

//-----------------------------------------------------------------------------
inline const INT32& Gen3Interface::MaxMachineFaults(const INT32 *maxFaults /*= NULL*/)
{
    if(maxFaults != NULL)  m_maxMachineFaults = *maxFaults;
    return m_maxMachineFaults;
}

//-----------------------------------------------------------------------------
inline const string Gen3Interface::GetFaultCode(const string &faultTag)
{
    XmlNodeMapItr iter = m_machineFaultCodes.find(faultTag);
    string faultCode((iter != m_machineFaultCodes.end()) ? iter->second->getValue() : "0xFFFF");
    return faultCode;
}

//-----------------------------------------------------------------------------
inline const INT32& Gen3Interface::WheelbaseAdjustStartDelay(const INT32 *startDelay /*= NULL*/)
{
    if(startDelay != NULL)  m_wheelbaseAdustStartDelay = *startDelay;
    return m_wheelbaseAdustStartDelay;
}

//-----------------------------------------------------------------------------
inline const bool& Gen3Interface::AutoRaiseRetainers(const bool *autoRaise /*= NULL*/)
{
    if(autoRaise != NULL)  m_autoRaiseRetainers = *autoRaise;
    return m_autoRaiseRetainers;
}

//-----------------------------------------------------------------------------
inline const bool& Gen3Interface::AutoLowerRetainers(const bool *autoLower /*= NULL*/)
{
    if(autoLower != NULL)  m_autoLowerRetainers = *autoLower;
    return m_autoLowerRetainers;
}

//-----------------------------------------------------------------------------
inline const INT32& Gen3Interface::AutoRetainersToggleTime(const INT32 *toggleTime /*= NULL*/)
{
    if(toggleTime != NULL)  m_autoRetainersToggleTime = *toggleTime;
    return m_autoRetainersToggleTime;
}

//-----------------------------------------------------------------------------
void Gen3Interface::LoadWheelbasePositionTable(void)
{
    XmlParser parser;
    try
    {
        const XmlNode *wheelbaseConfig = parser.ReturnXMLDocument(WheelbasePositionTableFileName());
        m_wheelbasePositions.DeepCopy(wheelbaseConfig->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Error loading wheelbase position table - %s", excpt.GetReason());
    }
    catch(BepException &excpt)
    {
        Log(LOG_ERRORS, "BepException loading wheelbase position table from file - %s", excpt.GetReason());
    }
}

//-----------------------------------------------------------------------------
inline const string& Gen3Interface::WheelbasePositionTableFileName(const string *name /*= NULL*/)
{
    if(name != NULL)  m_wheelbasePositionTableFileName = *name;
    return m_wheelbasePositionTableFileName;
}
