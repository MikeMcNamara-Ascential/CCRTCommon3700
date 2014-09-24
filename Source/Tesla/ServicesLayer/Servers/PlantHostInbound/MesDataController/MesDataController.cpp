//*************************************************************************
// FILE:
//    $Header: /Common3700/Source/Tesla/ServicesLayer/Servers/PlantHostInbound/MesDataController/MesDataController.cpp 1     3/09/06 3:16p Gswope $
//
// FILE DESCRIPTION:
//   MesDataController class to handle all communications between the DCX
//	 broadcast system and the Burke Porter system
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// Changes to log statements only. 
// This is Tesla specific.. we whould investigate mergeing this with core
// where possible
//
// LOG:
//    $Log: /Common3700/Source/Tesla/ServicesLayer/Servers/PlantHostInbound/MesDataController/MesDataController.cpp $
// 
// 1     3/09/06 3:16p Gswope
// After changes to bring Tesla up to "current core"
// 
// 3     3/18/05 11:53a Gswope
// Changes to allow testing with or without build
// stubbed code so WCC does not attempt to set QNX system time
// 
// 2     3/02/05 1:33p Gswope
// added some comments
// 
// 1     1/18/05 10:47a Gswope
// Initial Entry in this project in SourceSafe
// 
// 3     11/24/04 12:01p Bmeinke
// RetrieveBuildRecord() now uses the new
// PlantHostInbound::GetInputVinLength() method to determine if the VIN
// length is valid
// ReadBuildRecordFromBroadcast() pads the scanned VIN with leading spaces
// if the scanned VIN is shorter than the 17 characters required by the
// work cell
// 
// 1     10/19/04 2:35p Bmeinke
// MesDataController server for Tesla.
// Copied from DaimlerChrysler
// 
// 1     8/04/04 1:07p Derickso
// 
// 1     1/27/04 9:49a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 2     9/24/03 7:00a Derickso
// Updated for new approach to adding details to prompts.
// 
// 1     8/21/03 9:08a Derickso
// 05WK CCRT Project
// 
// 1     7/09/03 2:31p Rwiersem
// Initial version.
// 
// 1     7/03/03 5:20p Khykin
// Initial checkin
// 
// 1     6/12/03 3:56p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:52a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:43  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:20a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:15a Khykin
// Initial Checkin From DCTC
// 
// 11    8/14/02 5:16p Skypig13
// Removed compiler warnings
// 
// 10    8/14/02 12:56p Skypig13
// Removed compiler warnings
// 
// 9     6/13/02 3:04p Dan.erickson
// Clear prompt boxes on new scan if the retaining rolls are down.
// Added WccNoResponse prompt.
// 
// 8     6/13/02 7:25a Dan.erickson
// Added logic to spin up the unsolicited request handler thread.
// Switched to the new form of getChild().
// 
// 7     6/03/02 1:03p Dan.erickson
// Added Diagnostic statements.
// Modified the way the wheelbases are determined.  Now the current
// wheelbase is read from the PLC and if no matching body style is
// found in the wheelbase config file, the current wheelbase is
// used.
// 
// 6     5/20/02 4:45p Dan.erickson
// Pass an unsigned char to Send method.
// 
// 5     5/06/02 3:36p Dan.erickson
// Completed adding Serial Channel communications.
// 
// 4     4/24/02 3:17p Dan.erickson
// Completed writing and testing.
// Added Serial Communications so this can talk to the 
// DaimlerChrysler broadcast system.
// 
// 3     4/08/02 12:03p Dan.erickson
// Added diagnostic statements.
// Setup Wheel Base selection to use a default value specified in 
// the config file if a match is not found.
// Retrieving build record from file has been tested and works.
// Retrieving build record from Broadcast still needs to be tested.
// 
// 2     4/05/02 7:48a Dan.erickson
// Still developing.
// 
// 1     1/21/02 12:32p Dan.erickson
// Created.
// 
//*************************************************************************

#include <time.h>
#include "MesDataController.h"
#include "ITaskMon.h"


MesDataController::MesDataController() : PlantHostInbound(), m_wccCommSetup(NULL)
{
    m_testResult = NULL;    // set the test result interface to null
}

MesDataController::~MesDataController()
{
    if (m_wccCommSetup != NULL)   delete m_wccCommSetup;
    m_wccCommSetup = NULL;
    if (m_testResult != NULL)   // delete the test result interface to null
    {
        delete m_testResult;
    }
}

void MesDataController::Initialize(const std::string &fileName)
{                             // Call the base class method
    PlantHostInbound::Initialize(fileName);
}

void MesDataController::Initialize(const XmlNode *document)
{
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::Initialize() enter\n");

    // Call the base class initialize
    PlantHostInbound::Initialize(document);

    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::Initialize() begin\n");
    try
    {                          // Get the drive axle for the various body styles
        SetBodyStyleDriveAxle(document->getChild("Setup/DataTags/BodyStyleDriveAxle"));
    }
    catch (...)
    {                          // No drive axle list specified
        SetBodyStyleDriveAxle(NULL);
    }
    try
    {                          // Get the setup data for the serial channel communications
        SetBroadcastCommSetup(document->getChild("Setup/Broadcast/BroadcastCommunication"));
    }
    catch (...)
    {
        SetBroadcastCommSetup(NULL);
    }
    // Get the reconnect time delay
    try
    {
        SetReconnectDelay(BposReadInt(document->getChild("Setup/Broadcast/ReconnectDelay")->getValue().c_str()));
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "ReconnectDelay not specified, using 1s: %s", excpt.GetReason());
        SetReconnectDelay(1000);
    }
    try
    {                          // Set the number of times we should try communications with broadcast
        SetMaxAttempts(GetBroadcastCommSetup()->getChild("Setup/MaxAttempts")->getValue());
    }
    catch (...)
    {
        SetMaxAttempts(3);
    }
    try
    {                          // Set the maximum time to look for a message
        SetMaxResponseTime(GetBroadcastCommSetup()->getChild("Setup/MaxResponseTime")->getValue());
    }
    catch (...)
    {  // default to 2 seconds of delay for a response
        SetMaxResponseTime(2000);
    }

    try
    {  // Set the flag that indicates if we are suppose to send a test result to the GUI for 
       //       printing on a PFS abort vehicle
        m_printOnPfsBlock = (!strnicmp(document->getChild("Setup/PrintOnPfsBlock")->getValue().c_str(),
                                       "Yes",3)) ? true : false;
    }
    catch (...)
    {
        Log(LOG_ERRORS, "Exception loading value for PrintOnPfsBlock, setting to false\n");
        m_printOnPfsBlock = false;
    }

    if (m_testResult == NULL)   //  the test result interface to null
    {
        m_testResult = new ITestResultServer();
        m_testResult->Initialize(TEST_RESULT_SERVER_NAME, "Client", IsDebugOn());
    }

    // Load the prompt tags
    LoadPromptTags(document->getChild("Setup/Prompts"));

    // Determine if we should use section length data
    bool useSectionData = false;
    try
    {
        string configSectionData = document->getChild("Setup/Broadcast/Data/UseSectionData")->getValue().c_str();
        useSectionData = (!stricmp(configSectionData.c_str(), "Yes")  || !stricmp(configSectionData.c_str(), "yes") ||
                          !stricmp(configSectionData.c_str(), "Y")    || !stricmp(configSectionData.c_str(), "y") ||
                          !stricmp(configSectionData.c_str(), "True") || !stricmp(configSectionData.c_str(), "true") ||
                          !stricmp(configSectionData.c_str(), "T")    || !stricmp(configSectionData.c_str(), "t") ||
                          !stricmp(configSectionData.c_str(), "1"));
    }
    catch (XmlException &excpt)
    {
        useSectionData = false;
        Log(LOG_ERRORS, "Not using section data from message: %s", excpt.GetReason());
    }
    Log(LOG_ERRORS, "Use section data: %d\n", (int) useSectionData);

    (void) UseSectionLengthData(&useSectionData);
    // Set the valid message length
    XmlNode *data = const_cast<XmlNode *>(document->getChild("Setup/Broadcast/Data"));
    m_wccDataNodeMap.DeepCopy(data->getChildren());
    INT32 validWccResponseLength;
    try
    {
        validWccResponseLength = atoi(data->getChild("ValidWccResponseLength")->getValue().c_str());
    }
    catch (...)
    {
        validWccResponseLength = 46;
    }
    SetValidMessageLength((UINT32)validWccResponseLength);
    SetValidWccResponseLength((UINT32)validWccResponseLength);
    if (!UseSectionLengthData())
    {   // Set the valid message length
        INT32 messageDescriptionSectionLength, partNumberSectionLength, testCodeSectionLength;
        try
        {
            messageDescriptionSectionLength = atoi(data->getChild("MessageDescriptionLength")->getValue().c_str());
            partNumberSectionLength = atoi(data->getChild("PartNumberSectionLength")->getValue().c_str());
            testCodeSectionLength = atoi(data->getChild("TestCodeSectionLength")->getValue().c_str());
        }
        catch (...)
        {
            messageDescriptionSectionLength = 50;
            partNumberSectionLength = 80;
            testCodeSectionLength = 30;
        }

        UINT32 validMessageLength = messageDescriptionSectionLength + partNumberSectionLength + testCodeSectionLength;
        SetValidMessageLength(validMessageLength);
    }
    else
    {
        m_wccSectionMap.DeepCopy(document->getChild("Setup/Broadcast/Data/SectionOrder")->getChildren());
        m_wccSectionLengthMap.DeepCopy(document->getChild("Setup/Broadcast/Data/SectionDataLengths")->getChildren());
    }

    // Load any data that was specified in the config file
    LoadData(data);

    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::Initialize() complete\n");
}

const std::string MesDataController::Register(void)
{  // Initialize the PromptServer interface object
    m_promptComm.Initialize(PROMPT_SERVER_NAME, DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT,false); 
    Log(LOG_DEV_DATA, "Initialized prompt server communications\n");
    // Broadcast comm object should always be started.  This server allows dynamically changing of the
    // build record source.  If the server comes up with File as the source, you should still be able to
    // dynamically change to broadcast.
    m_wccComm.Initialize(GetBroadcastCommSetup());
    Log(LOG_DEV_DATA, "Initialized broadcast communications\n");
    return(PlantHostInbound::Register());
}

const std::string MesDataController::Publish(const XmlNode *node)
{
    std::string result = BEP_ERROR_RESPONSE, response, loadResult;
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::Publish(%s)\n", node->ToString().c_str());
    result = BepServer::Publish(node);
    // Only do special stuff if publish was successful
    if (result == BEP_SUCCESS_RESPONSE)
    {
        if (node->getName() == GetVehiclePresentTag())
        {  // Update the DataInputServer state 
            UpdateInputServerState();
        }
// the next section is triggered by a VIN scan.
        else if (node->getName() == GetNextVinTag())
        {   // Store the vin to be retrieved
            Log(LOG_DEV_DATA, "Storing VIN to load - %s\n", node->getValue().c_str());
            SetVINToLoad(node->getValue());
            // Signal to get data from broadcast
            Log(LOG_DETAILED_DATA, "Signaling to retrieve broadcast data\n");
            m_getBuildRecord.Broadcast(getBuildRecord);
            BposSleep(100);   // Make sure signal is received
            // Clear the signal for the next scan
            m_getBuildRecord.Broadcast(doNotGetBuildRecord);
            Log(LOG_DETAILED_DATA, "Clearing signal to retrieve broadcast data\n");
        }
        else if ((node->getName() == START_VEHICLE_TEST_DATA_TAG) && (node->getValue() == "1"))
        {   // Invalidate the vehicle build record and update traffic light
            SetVehicleBuildRecordStatus(BEP_INVALID_RESPONSE);
            m_broker->Write(GetVinReadStatusTag(), NO_VIN, response, true);
        }
    }
    return(result);
}

void MesDataController::Run(volatile bool *terminateFlag) /*=NULL*/
{
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::Run() - begin\n");
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::Run() - waiting for data connection\n");
    WaitForMesHost();
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::Run() - data connection found. Starting Run loop\n");
    while (GetStatus() != BEP_TERMINATE)
    {   // Acquire the semaphore
        m_getBuildRecord.Acquire();
        // Wait until we are signaled(indirectly, this happens after a scan)
        m_getBuildRecord.Wait(getBuildRecord);
        // Release the semaphore
        m_getBuildRecord.Release();
        // Determine where to retrieve the build data from
        XmlParser parser;
        const XmlNode *vehicleBuildSource = parser.ReturnXMLDocument(GetConfigFile())->getChild("Setup/VehicleBuildRecordSource");
        SetVehicleBuildSource(vehicleBuildSource->getValue());
        // Process the request
        Log(LOG_DEV_DATA, "Retrieving build record data from %s\n", GetVehicleBuildSource().c_str());
        std::string loadResult = RetrieveBuildRecord();
        if (loadResult != BEP_SUCCESS_RESPONSE)  // Error loading the build record
            Log(LOG_DEV_DATA, "Error retrieving build record data - loadResult:%s\n", loadResult.c_str());
        else                                     // Build record was loaded succesfully
            Log(LOG_DEV_DATA, "Retrieved build record data - loadResult:%s\n", loadResult.c_str());
    }
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::Run() - complete\n");
}

const std::string MesDataController::RetrieveBuildRecord(void)         
{
    string response;
    string loadResult = BEP_FAILURE_RESPONSE;
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::RetrieveBuildRecord() - begin\n");
    // Only attempt to load vehicle build record if valid vin
//	if (GetVINToLoad().length() >= vinLength)
    if (GetVINToLoad().length() >= (uint32_t)GetInputVinLength())
    {  // Update the traffic light widget to let driver know vin is being processed
        m_broker->Write(GetVinReadStatusTag(), PROCESSING_VIN, response, true);
        // Load the vehicle build record for the specified vin
        Log(LOG_DEV_DATA, "Retrieving build record for <%s>\n", GetVINToLoad().c_str());
        loadResult = LoadVehicleBuildRecord(GetVINToLoad(), m_vehicleBuild, true);
        if (loadResult == BEP_SUCCESS_RESPONSE)
        {  // Publish the vehicle build to any subscribers
            XmlNode nextVehicleBuild(GetVehicleBuildTag(), "");
            for (XmlNodeMapItr iter = m_vehicleBuild.begin(); iter != m_vehicleBuild.end(); iter++)
            {   // Add the build item to the node
                nextVehicleBuild.addChild(iter->second->Copy());
            }
            // Write the next build data
            Write(&nextVehicleBuild);
            // Get rid of the node
            nextVehicleBuild.clear();
        }
    }
    else
    {   // Invalid VIN 
        Log(LOG_ERRORS, "Invalid VIN <%s>, can not retrieve build record.  Returning %s\n", 
            GetVINToLoad().c_str(), loadResult.c_str());
    }
    return(loadResult);
}

void MesDataController::AddDerivedBuildInfo(XmlNodeMap &buildData)
{  // Determine drive axle configuration based on the body style and add to vehicle build
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::AddDerivedBuildInfo()\n");
    std::string bodyStyle = GetVehicleBuildItem(BODY_STYLE_TAG, buildData);
    std::string driveAxle = GetDriveAxle(bodyStyle);
    Log(LOG_DEV_DATA, "BodyStyle: %s\nDrive Axle: %s\n", bodyStyle.c_str(), driveAxle.c_str());
    AddVehicleBuildItem(DRIVE_AXLE_TAG, driveAxle, buildData);
    if (driveAxle == REAR_WHEEL_DRIVE_VALUE)
    {  // Set the PLC data for a rear wheel drive vehicle
        AddVehicleBuildItem(GetFrontWheelDriveTag(), "No", buildData);
        AddVehicleBuildItem(GetRearWheelDriveTag(), "Yes", buildData);
    }
    else
    {  // Set the data for a front wheel drive vehicle
        AddVehicleBuildItem(GetFrontWheelDriveTag(), "Yes", buildData);
        AddVehicleBuildItem(GetRearWheelDriveTag(), "No", buildData);
    }
    // Determine if the vehicle has four wheel drive or all wheel drive
    std::string transferCaseType = GetVehicleBuildItem(TRANSFER_CASE_TAG, buildData);
    Log(LOG_DEV_DATA, "T-case: %s\n", transferCaseType.c_str());
    AddVehicleBuildItem(ALL_WHEEL_DRIVE_TAG, HasAllWheelDrive(transferCaseType), buildData);
    AddVehicleBuildItem(GetFourWheelDriveTag(), HasFourWheelDrive(transferCaseType), buildData);
    // Determine the wheelbase for the vehicle
    AddVehicleBuildItem(GetWheelbasePositionTag(), GetWheelBasePosition(bodyStyle), buildData);
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::AddDerivedBuildInfo() complete\n");
}

const std::string MesDataController::HasAllWheelDrive(const std::string &transferCaseType)
{
    std::string result("No");
    try
    {  // The vehicle is equipped with all wheel drive
        result = m_allWheelDrive.getNode(transferCaseType)->getValue();
    }
    catch (XmlException &XmlErr)
    {  // The vehicle is not equipped with all wheel drive
        result = "No";
    }
    return(result);
}

const std::string MesDataController::HasFourWheelDrive(const std::string &transferCaseType)
{
    std::string result("Yes");
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::HasFourWheelDrive(transferCaseType:%s)\n", transferCaseType.c_str());
    try
    {  // The vehicle is not equipped with four wheel drive
        result = m_fourWheelDrive.getNode(transferCaseType)->getValue();
    }
    catch (XmlException &XmlErr)
    {  // The vehicle is equipped with four wheel drive
        Log(LOG_ERRORS, "Did not find %s in map\n", transferCaseType.c_str());
        result = "Yes";
    }
    return(result);
}

const std::string MesDataController::GetDriveAxle(const std::string &bodyStyle)
{  // Set the default to rear wheel drive
    std::string driveAxle(REAR_WHEEL_DRIVE_VALUE);
    bool foundBodyStyle = false;
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::GetDriveAxle(%s)\n", bodyStyle.c_str());
    if (!m_bodyStyleDriveAxle.empty())
    {  // Loop through the map and find the body style node
        for (XmlNodeMapItr iter = m_bodyStyleDriveAxle.begin(); 
            (iter != m_bodyStyleDriveAxle.end()) && !foundBodyStyle; iter++)
        {  // Check if the body style is listed
            std::string body = (*iter).second->getName();
            Log(LOG_DEV_DATA, "Evaluating %s from map - bodyStyle[0]:%c == body[0]:%c\n", body.c_str(), bodyStyle[0], body[0]);
            if (bodyStyle[0] == body[0])
            {  // Set flag so we know the body style was found
                Log(LOG_DEV_DATA, "Matched body Style in map, bodyStyle:%s matches body:%s\n", bodyStyle.c_str(), body.c_str());
                foundBodyStyle = true;
                driveAxle = (*iter).second->getValue();
                Log(LOG_DEV_DATA, "Drive axle selected: %s\n", driveAxle.c_str());
            }
        }
        // Check if the body style was found
        if (!foundBodyStyle)
        {  // Look for the default drive axle
            Log(LOG_ERRORS, "Did not find a match for body style: %s\n", bodyStyle.c_str());
            if (m_bodyStyleDriveAxle.find(DEFAULT_NODE) != m_bodyStyleDriveAxle.end())
            {  // Get the default drive axle
                driveAxle = m_bodyStyleDriveAxle.getNode(DEFAULT_NODE)->getValue();
                Log(LOG_ERRORS, "Using default drive axle: %s\n", driveAxle.c_str());
            }
        }
    }
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::GetDriveAxle(%s) - complete, returning drive axle: %s\n", bodyStyle.c_str(), driveAxle.c_str());
    return(driveAxle);
}

const std::string MesDataController::ReadBuildRecordFromBroadcast(const std::string &vin, std::string &buildRecord,
                                                                  const bool &updateStatus)
{
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::ReadBuildRecordFromBroadcast()\n");
    string result(BEP_UNAVAILABLE_RESPONSE);
    INT32 attempts = GetMaxAttempts();
    bool portLocked = false;
    std::string vinRequest( vin);

    // Clear the prompt boxes in case a prompt was previously dosplayed
    if (AreRetainersDown())
    {
        string response;
        m_promptComm.ClearPromptBox(1, response);
        m_promptComm.ClearPromptBox(2, response);
    }

    // Pad the VIN with spaces if it is too short
    if (vinRequest.length() < vinLength)
    {
        vinRequest = std::string( vinLength-vin.length(), ' ') + vin;
    }

    string requestMessage = requestBuild + (char)'|' + vinRequest;
    // Build the request message to issue
    UINT16 checkSum = CalculateCheckSum(requestMessage) & 0xFF;
//	string request = (char)ENQ + vin + (char)checkSum + (char)CR + (char)LF;
    string request = (char)ST + requestMessage + (char)checkSum + (char)ET;

    string logMessage;
    char buf[64];
    for (UINT32 ii = 0; ii < request.length(); ii++)
    {
        logMessage += CreateMessage(buf, sizeof(buf), "\t\t%02d: 0x%02X  %c\n", ii, request[ii]&0xFF, request[ii]);
    }
    Log(LOG_DEV_DATA, "Sending request to wcc [%d bytes, CheckSum=%d(0x%02X)]:\n%s", 
        request.length(), checkSum, checkSum, logMessage.c_str());
    do
    {  // Block the channel so the PFS results do not interfere with us getting data
        Log(LOG_DEV_DATA, "Attempt number %d of %d", (GetMaxAttempts()-attempts) + 1, GetMaxAttempts());
        if (!portLocked) portLocked = m_wccComm.LockPort();
        Log(LOG_DEV_DATA, "Port locked : %s\n", (portLocked ? "TRUE" : "FALSE"));
        // Clear the incoming buffer
        m_wccComm.ResetConnection();
        Log(LOG_DEV_DATA, "Cleared incoming buffer\n");
        // Send the request to the broadcast system
        INT32 status = m_wccComm.Send(request);
        Log(LOG_DEV_DATA, "Request sent, status:%d\n", status);
        // Evaluate the status of the Send operation 
        if (BEP_STATUS_SUCCESS != status)
        {  // There was an error sending the request
            Log(LOG_DEV_DATA, "Error requesting vin from broadcast - (status:%d)\n", status);
        }
        else
        {  // The request was made, get the response
            GetBroadcastResponse(buildRecord, vin);
        }
        // Unblock the channel so the PFS results can go out if needed
        if (portLocked)   portLocked = !(m_wccComm.UnlockPort());
        Log(LOG_DEV_DATA, "Port unlock status : %s(%d)\n", (!portLocked ? "TRUE" : "FALSE"), !portLocked);
        // If we received a response, evaluate it
        if (!buildRecord.empty())
        {  // Evaluate the response from the broadcast system
            result = ConvertStatusToResponse(EvaluateResponse(buildRecord, vin));
        }
        else  result = BEP_UNAVAILABLE_RESPONSE;
    } while ((result != BEP_SUCCESS_RESPONSE) && --attempts); 
    // Return the status
    Log(LOG_DEV_DATA, "(Tesla) MesDataController::ReadBuildRecordFromBroadcast() done - buildInfo:%s\n", buildRecord.c_str());
    return(result);
}

void MesDataController::GetBroadcastResponse(std::string &response, const std::string &vin)
{
    SerialString_t wccResponse;
    INT32 numBytesRead;
    // Clear the buffer
    response.erase();
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::GetBroadcastResponse()\n");
    // Clear the prompt boxes if retainers are down
    INT32 promptBox = 2;
    std::string clearResponse;
    if (AreRetainersDown()) m_promptComm.ClearPromptBox(promptBox, clearResponse);
    // Get the data from the buffer
    numBytesRead = m_wccComm.ReadPort(wccResponse, GetMaxResponseTime(), m_wccComm.GetTimeoutTime());
    Log("Got <0x%02X> from broadcast [ACK:0x%02X, NAK:0x%02X] - %d total bytes\n", wccResponse[0], ACK, NAK, numBytesRead);
    // If we got data, save it
    if (numBytesRead > 0)
    {   // Determine what type of message this is
        if (ACK == wccResponse[0])
        {  // Check for a Clock indication
            if (wccResponse[1] != CLOCK)
            {
                Log(LOG_DEV_DATA, "No clock sent from broadcast\n");
                wccResponse[1] = 0x01;        // Arbitrary so we do not get misinterpretted end of string
            }
            // Log the data we received from the workcell controller
            char buf[32768];
            string logMessage("Complete response from Broadcast:\n");
            for (UINT16 index = 0; index < wccResponse.length(); index++)
            {   // Create the log message
                logMessage += CreateMessage(buf, sizeof(buf), "\t\tindex: %03d -- %c <0x%02X>\n", 
                                            index, isprint(wccResponse[index]) ? wccResponse[index] : '?', wccResponse[index]);
            }
            Log(LOG_DEV_DATA, "%s\n", logMessage.c_str());
        }
        else if (NAK == wccResponse[0])
        {  // Display a prompt letting everyone know a NAK was received
            string resp;
            Log(LOG_DEV_DATA, "Recieved NAK from Broadcast\n");
            if (AreRetainersDown()) m_promptComm.DisplayPrompt(2, GetNAKPrompt(), resp);
        }
        else if (NO_INFO == wccResponse[0])
        {  // Broadcast has no information for the requested vin
            Log(LOG_DEV_DATA, "Broadcast has NO information for the requested Vin: %s\n", vin.c_str());
            if (AreRetainersDown())
            {  // No test in progress, display the prompt
                INT32 promptBox = 2;
                string result, detailTag("Param00");
                PromptDetails noInfoDetails;
                noInfoDetails.AddDetail(detailTag, vin);
                m_promptComm.DisplayPrompt(promptBox, GetNoInfoPrompt(), noInfoDetails, result); 
            }
        }
        // Save the response to pass back
        Log(LOG_DEV_DATA, "Preparing response for processing...\n");
        for (UINT16 index = 1; index < wccResponse.length(); index++) response += (char)wccResponse[index];
    }
    else
    {  // No response received from broadcast
        string resp;
        Log(LOG_ERRORS, "Timeout waiting for a response from broadcast\n");
        if (AreRetainersDown()) m_promptComm.DisplayPrompt(2, this->GetNoResponsePrompt(), resp);
    }
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::GetBroadcastResponse() complete\n");
}

const BEP_STATUS_TYPE MesDataController::EvaluateResponse(std::string &response, const std::string &vin)
{
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    INT32 promptBox = 2;
    std::string result;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// line changed, since code does not work
//   static bool clockSet = false;   / / Only need to update the clock the first time through
    static bool clockSet = true;
    Log(LOG_DEV_DATA, "Message length: %d, validWccResponseLength: %d, validMessageLength: %d\n", response.length(), GetValidWccResponseLength(), GetValidMessageLength());
    if (response.length() >= GetValidWccResponseLength())
    {
        string protocolId = response.substr(1,5);
        if (!protocolId.compare("Build"))
        {
            bool goodMessage = false;
            // Check if the section lengths need to be retrieved from the message
            if (UseSectionLengthData())
            {
                StoreSectionLengths(response);
            }
            //Must store section lengths before GetValidMessageLengths()
            UINT32 valMsgLen = GetValidMessageLength();
            goodMessage = (response.length() >= valMsgLen);
            Log(LOG_DEV_DATA, "Message length: %d, validMessageLength: %d goodMessage: %d\n",
                response.length(), GetValidMessageLength(), goodMessage);

            if (goodMessage)
            {
                // Check to see if we need to set the system date/time
                bool setClock = (response[0] == (char)CLOCK);
                // Get the check sum from the message and remove the trailer
                UINT16 wccCheckSum = (unsigned char)response[GetChecksumIndex()];
                response = response.substr(0,GetChecksumIndex());
                Log(LOG_DEV_DATA, "Checksum at index %d: %d[$%02X]", GetChecksumIndex(), wccCheckSum, wccCheckSum);
                string logMessage;
                char buf[1024];
                for (UINT16 ii = 0; ii < response.length(); ii++)
                {
                    logMessage += CreateMessage(buf, sizeof(buf), "\t\tii:%03d, %c <0x%02X>\n", 
                                                ii, (isprint(response[ii]) ? response[ii] : '?'), (unsigned char)response[ii]);
                }
                Log(LOG_DEV_DATA, "Truncated response to: %s\n%s\n", response.c_str(), logMessage.c_str());
                // Make sure the checksum of the response is valid
                UINT16 checkSum = CalculateCheckSum(response) & 0xFF;
                if (checkSum != wccCheckSum)
                {  // We got a bad checksum, message is not valid
                    Log("Bad checksum from broadcast -- calculated:%d[$%02X], broadcast:%d[$%02X]\n", checkSum, checkSum, wccCheckSum, wccCheckSum);
                    if (AreRetainersDown())
                    {  // No test in progress, display the prompt
                        m_promptComm.DisplayPrompt(promptBox, GetBadChecksumPrompt(), result);
                    }
                }
                else
                {  // Received a valid message from broadcast
                    status = BEP_STATUS_SUCCESS;
                    if (setClock && !clockSet)
                    {  // Get the date and time from the record and the system
                        std::string wccDate = response.substr(GetDateIndex(), GetDateLength());
                        std::string day = wccDate.substr(0,2), month = wccDate.substr(2,2), year = wccDate.substr(4,2);
                        wccDate = year + month + day + response.substr(GetTimeIndex(), GetTimeLength());
                        std::string hour = response.substr(GetTimeIndex(), 2);
                        std::string minutes = response.substr(GetTimeIndex()+2, 2);
                        std::string sysDate = GetTimeAndDate("%y%m%d%H%M");
                        // Range check the date and time
                        bool badDate = false;
                        struct tm broadcastDate;
                        broadcastDate.tm_year = (2000 - 1900) + atoi(year.c_str());    // This is number of years since 1900
                        broadcastDate.tm_mon  = atoi(month.c_str());
                        broadcastDate.tm_mday = atoi(day.c_str());
                        broadcastDate.tm_hour = atoi(hour.c_str());
                        broadcastDate.tm_min  = atoi(minutes.c_str());
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // next line does NOT work.
                        if ((int)mktime(&broadcastDate) == -1) badDate = true;
        
                        Log(LOG_DEV_DATA, "wccDate: %s\tsystem Date: %s\n", wccDate.c_str(), sysDate.c_str());
                        if ((wccDate != sysDate) && !badDate)
                        {  // Set a timer so we do not hang attempting to set the date/time
                            sigevent event;                     // Event structure informing what to do if timed out
                            event.sigev_notify = SIGEV_UNBLOCK;    // Unblock if time out
                            uint64_t timeout = mSEC_nSEC(500);     // Set timeout value - 500ms
                            TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_SEND | _NTO_TIMEOUT_REPLY | _NTO_TIMEOUT_RECEIVE, &event, &timeout, NULL);
                            // Set the system time specified in the record by adjusting the clock time over
                            // the next 60 seconds (-S 60)
                            std::string dateCommand = std::string("date -S 60 ") + wccDate + std::string(";rtc -s hw") + " >/dev/null";
                            BposSystemCommand(dateCommand.c_str());
                            clockSet = true;
                        }
                        else if (badDate)
                        {
                            Log(LOG_ERRORS, "Bad date received from broadcast, not setting date/time -- month:%s, day:%s, year:%s\n", 
                                month.c_str(), day.c_str(), year.c_str());
                        }
                        else
                        {
                            Log(LOG_DEV_DATA, "System date and time match date and time from MesDataController");
                            clockSet = true;
                        }
                    }
                    else if (clockSet)
                    {
                        Log(LOG_DEV_DATA, "System clock previously set to match broadcast, not updating");
                    }
                    else
                    {
                        Log(LOG_DEV_DATA, "No date or time stamp from broadcast, not setting system time");
                    }
                }
            }
        }
    }
    else
    {  // Unknown message received from broadcast
        Log(LOG_ERRORS, "Unknown response from broadcast for Vin: %s, message: %s\n", vin.c_str(), response.c_str());
        if (AreRetainersDown())
        {  // Retainers are down, display prompt
            m_promptComm.DisplayPrompt(promptBox, GetUnknownMessagePrompt(), result);
        }
    }
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::EvaluateResponse() done, returning: %d\n", status);
    return(status);
}

void MesDataController::TranslateBuildRecord(const std::string &buildRecord, XmlNodeMap &buildData, const bool &updateStatus)
{
    PlantHostInbound::TranslateBuildRecord(buildRecord, buildData, updateStatus);
    PlantHostInbound::TestStatusCheck(buildData);
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::TranslateBuildRecord Returned from TestStatusCheck\n");
    std::string pfsTestStatus = GetVehicleBuildItem(PERFORM_ABORT_TEST, buildData);
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::TranslateBuildRecord Returned from get veh build item status %s\n", pfsTestStatus.c_str());
    if (PERFORM_STATUS != pfsTestStatus)
    {

        Log(LOG_FN_ENTRY, "(Tesla) clearing build\n");
        // Clear the vehicle build data since it is in PFS Block status
        ClearVehicleBuild(buildData, updateStatus);
    }

    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::TranslateBuildRecord exit\n");
}

const std::string MesDataController::GetWheelBasePosition(const std:: string &bodyStyle)
{
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::GetWheelBasePosition()\n");
    std::string bodyStyleMask, selectWheelBase, response, wheelbaseTag;
    UINT32 ii = 0, weight = 0, selectWeight = 0;
    bool charPositionMatch = false;
    // Set the selected wheel base to the current wheelbase
    Log(LOG_DEV_DATA, "Reading current wheelbase position, tag:%s\n", GetWheelbasePositionTag().c_str());
    m_broker->Read(GetWheelbasePositionTag(), response, true);
    m_broker->GetNext(wheelbaseTag, selectWheelBase, response);
    // Loop through the list of wheel bases
    for (XmlNodeMapItr iter = m_wheelBasePositions.begin(); iter != m_wheelBasePositions.end(); iter++)
    {  // Calculate the weight of the specified body style to the current wheelbase
        bodyStyleMask = iter->second->getAttributes().getNode("BodyStyle")->getValue();
        Log(LOG_DEV_DATA, "Evaluating mask: %s\n", bodyStyleMask.c_str());
        weight = 0;
        ii = 0;
        do
        {  // If the current mask character is ? or characters match, keep checking
            charPositionMatch = (bodyStyleMask[ii] == '?') || (bodyStyleMask[ii] == bodyStyle[ii]);
            // Determine if the weight needs to be updated
            if (charPositionMatch && (bodyStyleMask[ii] == bodyStyle[ii])) weight++;
        } while (charPositionMatch && (ii++ < bodyStyle.length()));
        // Evaluate the weight of this wheelbase position
        Log(LOG_DEV_DATA, "weight: %d\n", weight);
        if (charPositionMatch && (weight > selectWeight))
        {  // Save the current weight and wheelbase position
            selectWeight = weight;
            selectWheelBase = iter->second->getAttributes().getNode("Wheelbase")->getValue();
            // Convert the wheelbase to x10
            double wheelbase = atof(selectWheelBase.c_str());
            char buff[5];
            selectWheelBase = itoa((int)(wheelbase*10), buff, 10);
        }
        Log(LOG_DEV_DATA, "Select Weight: %d, Select Wheelbase(x10): %s\n", selectWeight, selectWheelBase.c_str());
    }
    Log(LOG_FN_ENTRY, "(Tesla) MesDataController::GetWheelBasePosition() done, returning: %s\n", selectWheelBase.c_str());
    return(selectWheelBase);
}

void MesDataController::LoadDataTags(const XmlNode *setupNode)
{  // Call the base class functionality to load up the tags
    PlantHostInbound::LoadDataTags(setupNode);
    // Now load the MesDataController specific data tags
    try
    {  // Get the tags to use for data that has to go to the PLC
        XmlNode *dataTags = const_cast<XmlNode *>(setupNode->getChild("DataTags"));
        SetFrontWheelDriveTag(dataTags->getChild("FrontWheelDriveSelectedToPLC")->getValue());
        SetRearWheelDriveTag(dataTags->getChild("RearWheelDriveToPLC")->getValue());
        SetFourWheelDriveTag(dataTags->getChild("FourWDSelected")->getValue());
        if (dataTags != NULL) dataTags->clear();
        dataTags = NULL;
    }
    catch (...)
    {  // Problem getting tags, use defaults
        SetFrontWheelDriveTag("FrontWheelDriveSelectedToPLC");
        SetRearWheelDriveTag("RearWheelDriveToPLC");
        SetFourWheelDriveTag("FourWDSelected");
    }
}

void MesDataController::LoadPromptTags(const XmlNode *promptTags)
{
    try
    {   // Get the prompt tags to use
        SetNoInfoPrompt(promptTags->getChild("WccNoInfo")->getValue());
        SetBadChecksumPrompt(promptTags->getChild("WccBadChecksum")->getValue());
        SetUnknownMessagePrompt(promptTags->getChild("WccUnknownMessage")->getValue());
        SetTestBlockPrompt(promptTags->getChild("PfsBlock")->getValue());
        SetAbortPrompt(promptTags->getChild("PfsAbortStatus")->getValue());
        SetShipStatusPrompt(promptTags->getChild("PfsShipStatus")->getValue());
        SetNAKPrompt(promptTags->getChild("WccSentNAK")->getValue());
        SetNoResponsePrompt(promptTags->getChild("WccNoResponse")->getValue());
        SetAbortColor(promptTags->getChild("PfsAbortColor")->getValue());
        SetShipStatusColor(promptTags->getChild("PfsShipColor")->getValue());
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "Prompt Tags not found, setting all to defaults! - %s", excpt.GetReason());
        SetNoInfoPrompt("WccNoInfo");
        SetBadChecksumPrompt("WccBadChecksum");
        SetUnknownMessagePrompt("WccUnknownMessage");
        SetTestBlockPrompt("PfsBlock");
        SetAbortPrompt("PfsAbortStatus");
        SetShipStatusPrompt("PfsShipStatus");
        SetNAKPrompt("WccSentNAK");
        SetNoResponsePrompt("WccNoResponse");
        SetAbortColor("Red");
        SetShipStatusColor("Red");
    }
}

void MesDataController::LoadData(const XmlNode *wccDataNode)
{
    try
    {  // Get the field indices and lengths
        SetTimeIndex(BposReadInt(wccDataNode->getChild("WccTimeIndex")->getValue().c_str()));
        SetDateIndex(BposReadInt(wccDataNode->getChild("WccDateIndex")->getValue().c_str()));
        SetTimeLength(wccDataNode->getChild("WccTimeLength")->getValue());
        SetDateLength(wccDataNode->getChild("WccDateLength")->getValue());
        SetChecksumIndex(CalculateSectionStartIndex("WccInfoSection") + BposReadInt(wccDataNode->getChild("WccChecksumIndex")->getValue().c_str()));
    }
    catch (...)
    {
        SetTimeIndex(179);
        SetDateIndex(184);
        SetTimeLength(4);
        SetDateLength(6);
        SetChecksumIndex(190);
    }
}


const INT32 MesDataController::CalculateSectionStartIndex(const string &section)
{   // First find the node for the requested section
    INT32 startIndex = 0;
    bool sectionFound = false;
    XmlNodeMapItr iter;

    for (iter = m_wccSectionMap.begin(); (iter != m_wccSectionMap.end()) && !sectionFound; iter++)
    {   // Check if this is the section we are looking for
        if (section.compare(iter->second->getValue()))
        {   // This is not the section we are looking for.
            // Add the section length to the start index
            startIndex += BposReadInt(iter->second->getAttribute("SectionLength")->getValue().c_str());
        }
        else
        {   // This is the section we are looking for -- Do NOT add this section length
            sectionFound = true;
        }
    }

    // Check if the section was found
    if (sectionFound)
    {   // The requested section was found
        Log(LOG_DEV_DATA, "Calculated starting index for section: %s - %d", section.c_str(), startIndex);
    }
    else
    {   // The requested section was not found
        Log(LOG_ERRORS, "CalculateSectionStartIndex - Could not find requested section: %s", section.c_str());
        startIndex = 0;
    }
    // Return the calculated start index
    return(startIndex);
}

INT32 MesDataController::GetStartIndex(const XmlNode *node)
{   // Determine how to get the start index
    INT32 startIndex = 0;
    if (UseSectionLengthData())
    {
        try
        {
            startIndex = (CalculateSectionStartIndex(node->getAttribute("Section")->getValue()) + 
                          BposReadInt(node->getAttribute("Offset")->getValue().c_str()));
        }
        catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Could not get start index for %s - %s", node->getName().c_str(), excpt.GetReason());
            startIndex = 0;
        }
    }
    else
    {
        startIndex = PlantHostInbound::GetStartIndex(node);
    }
    return(startIndex);
}

INT32 MesDataController::GetFieldLength(const XmlNode *node)
{   // Determine how to get the start index
    INT32 fieldLength = 0;
    if (UseSectionLengthData())
    {
        try
        {
            fieldLength = atoi(node->getAttribute(FIELD_LENGTH)->getValue().c_str());
        }
        catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Could not get field length for %s - %s", node->getName().c_str(), excpt.GetReason());
            fieldLength = 0;
        }
    }
    else
    {
        fieldLength = PlantHostInbound::GetFieldLength(node);
    }
    return(fieldLength);
}

bool MesDataController::IsStartIndexValid(const XmlNode *field, const INT32 &messageLength)
{
    bool indexValid = false;
    if (UseSectionLengthData())
    {   // Get the offset into the section and the section length
        INT32 offset = 0;
        try
        {
            offset = BposReadInt(field->getAttribute("Offset")->getValue().c_str());
        }
        catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "XmlException getting Offset for %s - %s", field->getName().c_str(), excpt.GetReason());
            offset = 0;
        }
        INT32 fieldLength = 0;
        try
        {
            fieldLength = BposReadInt(field->getAttribute("Length")->getValue().c_str());
        }
        catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "XmlException getting field Length for %s - %s", field->getName().c_str(), excpt.GetReason());
            fieldLength = 0;
        }
        INT32 sectionLength = GetSectionLength(field->getAttribute("Section")->getValue());
        // Ensure the offset is within the section
        indexValid = ((offset < sectionLength) && ((offset + fieldLength) <= sectionLength));
        Log(LOG_DEV_DATA, "Index valid: %s - SectionLength: %d, Offset: %d, FieldLength: %d",
            (indexValid ? "True" : "False"), sectionLength, offset, fieldLength); 
    }
    else
    {
        indexValid = PlantHostInbound::IsStartIndexValid(field, messageLength);
    }
    return(indexValid);
}

INT32 MesDataController::GetSectionLength(const string &section)
{
    INT32 sectionLength = 0;
    bool sectionFound = false;
    for (XmlNodeMapItr iter = m_wccSectionMap.begin(); (iter != m_wccSectionMap.end()) && !sectionFound; iter++)
    {   // Check if this is the section we are looking for
        if (!section.compare(iter->second->getValue()))
        {
            try
            {
                sectionLength = BposReadInt(iter->second->getAttribute("SectionLength")->getValue().c_str());
            }
            catch (XmlException &excpt)
            {
                Log(LOG_ERRORS, "XmlException getting SectionLengh for section: %s - %s", section.c_str(), excpt.GetReason());
                sectionLength = 0;
            }
            sectionFound = true;
        }
    }

    // Check if we found the section
    if (!sectionFound)
    {
        Log(LOG_ERRORS, "Could not find section length data for section: %s, returning section length: 0", section.c_str());
        sectionLength = 0;
    }
    // Return the length of the requested section if found
    return(sectionLength);
}

void MesDataController::StoreSectionLengths(const string &buildMessage)
{   // Loop through the section map and store the length data
    try
    {
        UINT32 validMessageLength = 0;
        for (XmlNodeMapItr iter = m_wccSectionMap.begin(); iter != m_wccSectionMap.end(); iter++)
        {   // Get the section length node to be used
            string sectionLengthTag = iter->second->getAttribute("SectionLengthTag")->getValue();
            if (!sectionLengthTag.empty())
            {   // Get the current section length for the current field
                XmlNodeMapItr sectionIter;
                if ((sectionIter = m_wccSectionLengthMap.find(sectionLengthTag)) != m_wccSectionLengthMap.end())
                {   // Found the section length data, extract the current length from the data
                    INT32 startIndex = BposReadInt(sectionIter->second->getAttribute("StartIndex")->getValue().c_str());
                    INT32 fieldLength = BposReadInt(sectionIter->second->getAttribute("Length")->getValue().c_str());
                    INT32 length = BposReadInt(buildMessage.substr(startIndex, fieldLength).c_str());
                    Log(LOG_DEV_DATA, "Store section length data for %s: StartIndex: %d, FieldLength: %d, SectionLength: %d",
                        sectionLengthTag.c_str(), startIndex, fieldLength, length);

                    validMessageLength += length;
                    // Update the node with the correct data
                    XmlNode *sectionLengthNode = const_cast<XmlNode *>(iter->second->getAttribute("SectionLength"));
                    char buff[10];
                    sectionLengthNode->setValue(itoa(length, buff, 10));
                    Log(LOG_DEV_DATA, "Updated section data node: %s", iter->second->ToString().c_str());
                }
                else
                {   // Could not find the section length node
                    Log(LOG_ERRORS, "Could not find section length data node for %s", sectionLengthTag.c_str());
                }
            }
            else
            {
                Log(LOG_ERRORS, "section length tag empty for: %s", sectionLengthTag.c_str());
            }
            Log(LOG_DEV_DATA, "Done storing section length infor for: %s", sectionLengthTag.c_str());
        }
        SetValidMessageLength(validMessageLength);
        Log(LOG_DEV_DATA, "Valid Message Length set to %d", validMessageLength);
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "Exception caught in StoreSectionLengths");
    }
}

void MesDataController::SetBodyStyleDriveAxle(const XmlNode *bodyStyleDriveAxle)
{   // Make sure the node is not NULL
    if (bodyStyleDriveAxle != NULL) m_bodyStyleDriveAxle.DeepCopy(bodyStyleDriveAxle->getChildren());
    else m_bodyStyleDriveAxle.clear(true);
}

void MesDataController::SetRearWheelDriveTag(const std::string &tag)
{
    m_rearWheelDriveTag = tag;
}

void MesDataController::SetFrontWheelDriveTag(const std::string &tag)
{
    m_frontWheelDriveTag = tag;
}

void MesDataController::SetFourWheelDriveTag(const std::string &tag)
{
    m_fourWheelDriveTag = tag;
}

void MesDataController::SetAutomaticTransmissionTag(const std::string &tag)
{
    m_autoTransTag = tag;
}

void MesDataController::SetManualTransmissionTag(const std::string &tag)
{
    m_manualTransTag = tag;
}

inline void MesDataController::SetNoInfoPrompt(const std::string &prompt)
{
    m_noInfoPrompt = prompt;
}

inline void MesDataController::SetBadChecksumPrompt(const std::string &prompt)
{
    m_badCheckSumPrompt = prompt;
}

inline void MesDataController::SetUnknownMessagePrompt(const std::string &prompt)
{
    m_unknownMessagePrompt = prompt; 
}

inline void MesDataController::SetTestBlockPrompt(const std::string &prompt)
{
    m_testBlockPrompt = prompt;
}

inline void MesDataController::SetAbortPrompt(const std::string &prompt)
{
    m_abortStatusPrompt = prompt;
}

inline void MesDataController::SetShipStatusPrompt(const std::string &prompt)
{
    m_shipStatusPrompt = prompt;
}

inline void MesDataController::SetAbortColor(const string &color)
{
    m_abortStatusColor = color;
}

inline void MesDataController::SetShipStatusColor(const string &color)
{
    m_shipStatusColor = color;
}
inline void MesDataController::SetNAKPrompt(const std::string &prompt)
{
    m_wccNAKPrompt = prompt;
}

inline void MesDataController::SetNoResponsePrompt(const std::string &prompt)
{
    m_wccNoResponsePrompt = prompt;
}

inline void MesDataController::SetTimeIndex(const std::string &index)
{
    SetTimeIndex(atoi(index.c_str()));
}

inline void MesDataController::SetTimeIndex(const INT32 &index)
{
    m_wccTimeIndex = index;
}

inline void MesDataController::SetTimeLength(const std::string &length)
{
    SetTimeLength(atoi(length.c_str()));
}
inline void MesDataController::SetTimeLength(const INT32 &length)
{
    m_wccTimeLength = length;
}
const std::string MesDataController::GetRearWheelDriveTag(void)
{
    return(m_rearWheelDriveTag);
}

const std::string MesDataController::GetFrontWheelDriveTag(void)
{
    return(m_frontWheelDriveTag);
}

const std::string MesDataController::GetFourWheelDriveTag(void)
{
    return(m_fourWheelDriveTag);
}

const std::string MesDataController::GetAutomaticTransmissionTag(void)
{
    return(m_autoTransTag);
}

const std::string MesDataController::GetManualTransmissionTag(void)
{
    return(m_manualTransTag);
}

const std::string MesDataController::GetNoInfoPrompt(void)
{
    return(m_noInfoPrompt);
}

const std::string MesDataController::GetBadChecksumPrompt(void)
{
    return(m_badCheckSumPrompt);
}

const std::string MesDataController::GetUnknownMessagePrompt(void)
{
    return(m_unknownMessagePrompt);
}

const std::string MesDataController::GetNAKPrompt(void)
{
    return m_wccNAKPrompt;
}

const std::string MesDataController::GetNoResponsePrompt(void)
{
    return m_wccNoResponsePrompt;
}

const INT32 MesDataController::GetTimeIndex(void)
{
    INT32 timeIndex = 0;
    if(UseSectionLengthData())
    {
        try
        {
            timeIndex = (CalculateSectionStartIndex(m_wccDataNodeMap.getNode("WccTimeIndex")->getAttribute("Section")->getValue()) + 
                         BposReadInt(m_wccDataNodeMap.getNode("WccTimeIndex")->getAttribute("Offset")->getValue().c_str()));
        }
        catch(XmlException &e)
        {
            Log(LOG_ERRORS, "XmlException calculating TimeIndex, %s", e.what());
            timeIndex = m_wccTimeIndex;
        }
    }
    else
    {
        timeIndex = m_wccTimeIndex;
    }

    return(timeIndex);
}

const INT32 MesDataController::GetTimeLength(void)
{
    return(m_wccTimeLength);
}

const INT32 MesDataController::GetDateIndex(void)
{
    INT32 dateIndex = 0;
    if(UseSectionLengthData())
    {
        try
        {
            dateIndex = (CalculateSectionStartIndex(m_wccDataNodeMap.getNode("WccDateIndex")->getAttribute("Section")->getValue()) + 
                         BposReadInt(m_wccDataNodeMap.getNode("WccDateIndex")->getAttribute("Offset")->getValue().c_str()));
        }
        catch(XmlException &e)
        {
            Log(LOG_ERRORS, "XmlException calculating DateIndex, %s", e.what());
            dateIndex = m_wccDateIndex;
        }
    }
    else
    {
        dateIndex = m_wccDateIndex;
    }

    return(dateIndex);
}

const INT32 MesDataController::GetDateLength(void)
{
    return(m_wccDateLength);
}

const INT16 MesDataController::GetChecksumIndex(void)
{
    INT16 checksumIndex = 0;
    if(UseSectionLengthData())
    {
        try
        {
            checksumIndex = (CalculateSectionStartIndex(m_wccDataNodeMap.getNode("WccChecksumIndex")->getAttribute("Section")->getValue()) + 
                         BposReadInt(m_wccDataNodeMap.getNode("WccChecksumIndex")->getAttribute("Offset")->getValue().c_str()));
        }
        catch(XmlException &e)
        {
            Log(LOG_ERRORS, "XmlException calculating ChecksumIndex, %s", e.what());
            checksumIndex = m_wccChecksumIndex;
        }
    }
    else
    {
        checksumIndex = m_wccChecksumIndex;
    }

    return(checksumIndex);
}

const INT32 MesDataController::GetMaxAttempts(void)
{
    return m_maxAttempts;
}

const INT32 MesDataController::GetMaxResponseTime(void)
{
    return m_maxResponseTime;
}

const UINT32 MesDataController::GetValidMessageLength(void)
{
    return m_validMessageLength;
}

const UINT32 MesDataController::GetValidWccResponseLength(void)
{
    return m_validWccResponseLength;
}

const bool MesDataController::AreRetainersDown(void)
{
    return(Read(m_retainersDownTag) == retainingRollsDown ? true : false);
}

const XmlNode* MesDataController::GetBroadcastCommSetup(void)
{
    return(m_wccCommSetup);
}

const std::string MesDataController::GetVINToLoad(void)
{
    return m_loadVIN;
}


void MesDataController::SetDateIndex(const std::string &index)
{
    SetDateIndex(atoi(index.c_str()));
}

void MesDataController::SetDateIndex(const INT32 &index)
{
    m_wccDateIndex = index;
}

void MesDataController::SetDateLength(const std::string &length)
{
    SetDateLength(atoi(length.c_str()));;
}

void MesDataController::SetDateLength(const INT32 &length)
{
    m_wccDateLength = length;
}

void MesDataController::SetChecksumIndex(const std::string &index)
{
    SetChecksumIndex(atoi(index.c_str()));
}

void MesDataController::SetChecksumIndex(const INT16 &index)
{
    m_wccChecksumIndex = index;
}

void MesDataController::SetMaxAttempts(const std::string &attempts)
{
    SetMaxAttempts(atoi(attempts.c_str()));
}

void MesDataController::SetMaxAttempts(const INT32 &attempts)
{
    m_maxAttempts = attempts;
}

void MesDataController::SetMaxResponseTime(const std::string &maxTime)
{
    SetMaxResponseTime(atoi(maxTime.c_str()));
}

void MesDataController::SetMaxResponseTime(const INT32 &maxTime)
{   // Software uses a 10ms sleep time between checks.
    m_maxResponseTime = maxTime/10;
}

void MesDataController::SetValidMessageLength(const UINT32 &length)
{
    m_validMessageLength = length;
}

void MesDataController::SetValidWccResponseLength(const UINT32 &length)
{
    m_validWccResponseLength = length;
}

void MesDataController::ClearMaps(void)
{
    m_bodyStyleDriveAxle.clear(true);
    PlantHostInbound::ClearMaps();
}

void MesDataController::SetBroadcastCommSetup(const XmlNode *setup)
{
    if (setup != NULL) m_wccCommSetup = setup->Copy();
    else m_wccCommSetup = NULL;
}

void MesDataController::SetVINToLoad(const std::string &vin)
{
    m_loadVIN = vin;
}

const bool& MesDataController::UseSectionLengthData(const bool *useSectionData) /*= NULL*/
{
    if (useSectionData != NULL)
    {
        m_useSectionData = *useSectionData;
    }
    return(m_useSectionData);
}

void MesDataController::WaitForMesHost(void)
{
    TaskMonTask_t wccTask;
    do
    {   // Get the state of the WCC port
        if (ITaskMon::ReadTaskInfo(wccTask, "MesHost", TASKMON_CLASS_SERIAL) != -1)
        {
            // If the state is not run, register the port driver
            if (wccTask.m_taskState != TASKSTATE_RUN)
            {   // Delay before attempting to reconnect/register since the other PC may not be completely up.
                BposSleep(GetReconnectDelay());
                // Register the port driver
                m_wccComm.RegisterPortDriver();
            }
            else
            {
                Log(LOG_ERRORS, "Error Task already in run state\n");
            }
            // Delay after registering to allow the port driver to finish registering.
            BposSleep(GetReconnectDelay());
        }
        else
        {
            Log(LOG_ERRORS, "Unable to read task info - Install latest TaskMon / TaskMonTerm to /usr/local/bin dir\n");
        }
        // Keep looking until state is run and we are not terminated
    } while ((wccTask.m_taskState != TASKSTATE_RUN) && GetStatus().compare(BEP_TERMINATE));
}

void MesDataController::SetReconnectDelay(const INT32 &delay)
{
    m_reconnectDelay = delay;
}

const INT32& MesDataController::GetReconnectDelay(void)
{
    return m_reconnectDelay;
}

