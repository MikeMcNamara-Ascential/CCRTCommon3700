//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ServicesLayer/Servers/PlantHostInbound/WorkCellController/WorkCellController.cpp 1     3/09/06 3:16p Gswope $
//
// FILE DESCRIPTION:
//   WorkCellController class to handle all communications between the DCX
//   broadcast system and the Burke Porter system
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
// This is HMMA specific.. we whould investigate mergeing this with core
// where possible
//
// LOG:
//    $Log: /HMMA/Source/HMMA/ServicesLayer/Servers/PlantHostInbound/WorkCellController/WorkCellController.cpp $
// 
// 1     3/09/06 3:16p Gswope
// After changes to bring HMMA up to "current core"
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
// WorkCellController server for HMMA.
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
#include "WorkCellController.h"


WorkCellController::WorkCellController() : PlantHostInbound(),
m_wccTimeInfo(NULL), m_wccDateInfo(NULL), m_wccChecksumInfo(NULL),
m_wccCommSetup(NULL), m_requestHandler(NULL), m_keepAlive(NULL), m_staticTesterComm(NULL), 
m_useSectionData(false), m_useSalesCodes(false), m_portLockedStatus(false), m_startStaticTesterComm(false)
{
    m_testResult = NULL;    // set the test result interface to null
}

WorkCellController::~WorkCellController()
{   // Get rid of the comm setup node
    if (m_wccCommSetup != NULL)   delete m_wccCommSetup;
    m_wccCommSetup = NULL;
    // Kill off t he request handler
    if (m_requestHandler != NULL) delete m_requestHandler;
    m_requestHandler = NULL;
    // Stop the keep alive thread
    if (m_keepAlive != NULL)  delete m_keepAlive;
    m_keepAlive = NULL;
    if (m_testResult != NULL)   // delete the test result interface to null
    {
        delete m_testResult;
    }
    // Delete the static tester comm object
    if (m_staticTesterComm != NULL)  delete m_staticTesterComm;
    m_staticTesterComm = NULL;
    // Clear the air pressure regulator settings
    m_airPressures.clear(true);
    // Clear the drive axle settings
    m_bodyStyleDriveAxle.clear(true);
    // Clear the body style index
    m_bodyStyleIndex.clear(true);
    // Clear the sales code data
    m_salesCodes.clear();
}

void WorkCellController::Initialize(const std::string &fileName)
{                             // Call the base class method
    PlantHostInbound::Initialize(fileName);
}

void WorkCellController::Initialize(const XmlNode *document)
{
// next two lines do not get logged, logger is not active yet
    Log(LOG_FN_ENTRY, "WorkCellController::Initialize() begin\n");
    // Call the base class initialize
    Log(LOG_DEV_DATA, "Initializing PlantHostInbound with %s", document->ToString().c_str());
    PlantHostInbound::Initialize(document);
    if (document != NULL)
    {
        Log(LOG_DEV_DATA, "Initializing WCC with %s", document->ToString().c_str());
        try
        {                          // Get the drive axle for the various body styles
            SetBodyStyleDriveAxle(document->getChild("Setup/DataTags/BodyStyleDriveAxle"));
        } catch (...)
        {                          // No drive axle list specified
            Log(LOG_ERRORS, "WARNING: No body style drive axles specified, setting list to NULL!");
            SetBodyStyleDriveAxle(NULL);
        }
        // Store the body style index
        try
        {
            m_bodyStyleIndex.DeepCopy(document->getChild("Setup/DataTags/BodyStyleIndexMap")->getChildren());
        } catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Could not find body style index: %s", excpt.GetReason());
            m_bodyStyleIndex.clear(true);
        }
        // Store the air pressure regulator settings
        try
        {
            StoreTireSizeAirPressures(document->getChild("Setup/DataTags/AdjustableAirPressures"));
        } catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "WARNING: No Air Pressure Regulator settings defined.  Setting list to NULL! - %s", excpt.GetReason());
            StoreTireSizeAirPressures(NULL);
        }
        // Store the minimum and maximum air pressure setting
        try
        {
            SetMinimumAirPressure(atof(document->getChild("Setup/DataTags/MinimumAirPressure")->getValue().c_str()));
            SetMaximumAirPressure(atof(document->getChild("Setup/DataTags/MaximumAirPressure")->getValue().c_str()));
        } catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Min/Max air pressure setting not found, using 0/80 - %s", excpt.GetReason());
            SetMinimumAirPressure(0.0);
            SetMaximumAirPressure(80.0);
        }
        try
        {                          // Get the setup data for the serial channel communications
            SetBroadcastCommSetup(document->getChild("Setup/Broadcast/BroadcastCommunication"));
            SetRequestHandlerConfig(document->getChild("Setup/Broadcast/RequestHandlerConfig"));
            SetKeepAliveConfig(document->getChild("Setup/Broadcast/KeepAliveConfig"));
        } catch (...)
        {
            Log(LOG_ERRORS, "WARNING: No broadcast comm setup or request handler setup found!");
            SetBroadcastCommSetup(NULL);
            SetRequestHandlerConfig(NULL);
            SetKeepAliveConfig(NULL);
        }
        try
        {                          // Set the number of times we should try communications with broadcast
            SetMaxAttempts(BposReadInt(GetBroadcastCommSetup()->getChild("Setup/MaxAttempts")->getValue().c_str()));
        } catch (...)
        {
            Log(LOG_ERRORS, "WARNING: No broadcast comm retries defined, setting retires to 3!");
            SetMaxAttempts(3);
        }
        try
        {                          // Set the maximum time to look for a message
            SetMaxResponseTime(BposReadInt(GetBroadcastCommSetup()->getChild("Setup/MaxResponseTime")->getValue().c_str()));
        } catch (...)
        {  // default to 2 seconds of delay for a response
            Log(LOG_ERRORS, "WARNING: No broadcast comm max response time defined, setting response time to 2000ms!");
            SetMaxResponseTime(2000);
        }

        try
        {  // Set the flag that indicates if we are suppose to send a test result to the GUI for 
           //       printing on a PFS abort vehicle
            m_printOnBlock = (!strnicmp(document->getChild("Setup/PrintOnPfsBlock")->getValue().c_str(),
                                        "Yes",3)) ? true : false;
        } catch (...)
        {
            Log(LOG_ERRORS, "Exception loading value for PrintOnPfsBlock, setting to false\n");
            m_printOnBlock = false;
        }

        if (m_testResult == NULL)
        {
            m_testResult = new ITestResultServer();
            m_testResult->Initialize(TEST_RESULT_SERVER_NAME, "Client", IsDebugOn());
        }
        // Check if the keep alive thread should be started
        bool startKeepAlive = false;
        try
        {
            string configKeepAlive = document->getChild("Setup/StartKeepAlive")->getValue().c_str();
            startKeepAlive = (!stricmp(configKeepAlive.c_str(), "Yes")  || !stricmp(configKeepAlive.c_str(), "yes")  ||
                              !stricmp(configKeepAlive.c_str(), "Y")    || !stricmp(configKeepAlive.c_str(), "y")    ||
                              !stricmp(configKeepAlive.c_str(), "True") || !stricmp(configKeepAlive.c_str(), "true") ||
                              !stricmp(configKeepAlive.c_str(), "T")    || !stricmp(configKeepAlive.c_str(), "t")    ||
                              !stricmp(configKeepAlive.c_str(), "1"));
        } catch (XmlException &excpt)
        {
            startKeepAlive = false;
            Log(LOG_ERRORS, "Not starting keep alive thread: %s", excpt.GetReason());
        }

        StartKeepAlive(&startKeepAlive);
        Log(LOG_ERRORS, "StartKeepAlive value: %d\n", StartKeepAlive());

        // Check if the request handler thread should be started
        bool startRequestHandler = false;
        try
        {
            string configRequestHandler = document->getChild("Setup/StartRequestHandler")->getValue();
            startRequestHandler = (!stricmp(configRequestHandler.c_str(), "Yes")  || !stricmp(configRequestHandler.c_str(), "yes")  ||
                                   !stricmp(configRequestHandler.c_str(), "Y")    || !stricmp(configRequestHandler.c_str(), "y")    ||
                                   !stricmp(configRequestHandler.c_str(), "True") || !stricmp(configRequestHandler.c_str(), "true") ||
                                   !stricmp(configRequestHandler.c_str(), "T")    || !stricmp(configRequestHandler.c_str(), "t")    ||
                                   !stricmp(configRequestHandler.c_str(), "1"));
        } catch (XmlException &excpt)
        {
            startRequestHandler = false;
            Log(LOG_ERRORS, "Not starting request handler thread: %s", excpt.GetReason());
        }
        StartRequestHandler(&startRequestHandler);
        // Determine if a static tester server interface should be started
        bool startStaticTesterComm;
        try
        {
            startStaticTesterComm = atob(document->getChild("Setup/StartStaticServerComm")->getValue().c_str());
        } catch (XmlException &excpt)
        {
            startStaticTesterComm = false;
            Log(LOG_ERRORS, "Not starting static tester server comm: %s", excpt.GetReason());
        }
        StartStaticTesterServerComms(&startStaticTesterComm);
        // Load the prompt tags

        LoadPromptTags(document->getChild("Setup/Prompts"));
        // Load the data indicating wha type of broadcast data we will receive
        bool useSalesCodes = false;
        try
        {
            useSalesCodes = atob(document->getChild("Setup/UseSalesCodes")->getValue().c_str());
        } catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Broadcast Data Type not specified, NOT using sales codes - %s", excpt.GetReason());
            useSalesCodes = false;
        }
        UseSalesCodes(&useSalesCodes);
        try
        {
            SetValidWccResponseLength(BposReadInt(document->getChild("Setup/Broadcast/Data/ValidWccResponseLength")->getValue().c_str()));
        } catch (XmlException &excpt)
        {
            SetValidWccResponseLength(10);
        }
        // Determine if we need to load the remaining broadcast parameters
        if (!UseSalesCodes())
        {   // Check if section data should be used from the build message
            Log(LOG_DEV_DATA, "Not using sales codes");

            bool useSectionData = false;
            try
            {
                string configSectionData = document->getChild("Setup/Broadcast/Data/UseSectionData")->getValue().c_str();
                useSectionData = (!stricmp(configSectionData.c_str(), "Yes")  || !stricmp(configSectionData.c_str(), "yes") ||
                                  !stricmp(configSectionData.c_str(), "Y")    || !stricmp(configSectionData.c_str(), "y") ||
                                  !stricmp(configSectionData.c_str(), "True") || !stricmp(configSectionData.c_str(), "true") ||
                                  !stricmp(configSectionData.c_str(), "T")    || !stricmp(configSectionData.c_str(), "t") ||
                                  !stricmp(configSectionData.c_str(), "1"));
            } catch (XmlException &excpt)
            {
                useSectionData = false;
                Log(LOG_ERRORS, "Not using section data from message: %s", excpt.GetReason());
            }
            Log(LOG_ERRORS, "Use section data: %d\n", (int) useSectionData);

            (void) UseSectionLengthData(&useSectionData);

            // Load any data that was specified in the config file
            LoadData(document->getChild("Setup/Broadcast/Data"));
            if (!UseSectionLengthData())
            {   // Set the valid message length
                XmlNode *data = const_cast<XmlNode *>(document->getChild("Setup/Broadcast/Data"));
                INT32 messageDescriptionSectionLength, testDataSectionLength, partNumberSectionLength, testCodeSectionLength;
                try
                {
                    messageDescriptionSectionLength = atoi(data->getChild("MessageDescriptionLength")->getValue().c_str());
                    testDataSectionLength = atoi(data->getChild("TestDataSectionLength")->getValue().c_str());
                    partNumberSectionLength = atoi(data->getChild("PartNumberSectionLength")->getValue().c_str());
                    testCodeSectionLength = atoi(data->getChild("TestCodeSectionLength")->getValue().c_str());
                } catch (...)
                {
                    messageDescriptionSectionLength = 50;
                    testDataSectionLength = 17;
                    partNumberSectionLength = 80;
                    testCodeSectionLength = 30;
                }
                UINT32 validMessageLength = messageDescriptionSectionLength + testDataSectionLength + partNumberSectionLength + testCodeSectionLength;
                SetValidMessageLength(validMessageLength);
            } else
            {   // Load the section map
                m_wccSectionMap.DeepCopy(document->getChild("Setup/Broadcast/Data/SectionOrder")->getChildren());
                m_wccSectionLengthMap.DeepCopy(document->getChild("Setup/Broadcast/Data/SectionDataLengths")->getChildren());
            }
        } else
        {   // Store the sales code data
            try
            {
                SetSalesCodeFieldLength(BposReadInt(document->getChild("Setup/SalesCodeLength")->getValue().c_str()));
                SetDescriptionFieldLength(BposReadInt(document->getChild("Setup/SalesCodeDescriptionLength")->getValue().c_str()));
                SetVinIndex(BposReadInt(document->getChild("Setup/VinIndex")->getValue().c_str()));
                SetVinLength(BposReadInt(document->getChild("Setup/VinLength")->getValue().c_str()));
                SetSalesCodeStartIndex(BposReadInt(document->getChild("Setup/SalesCodeStartIndex")->getValue().c_str()));
            } catch (XmlException &excpt)
            {
                Log(LOG_ERRORS, "Sales Code Data missing, using defaults: %s", excpt.GetReason());
                SetSalesCodeFieldLength(21);
                SetDescriptionFieldLength(30);
                SetVinIndex(162);
                SetVinLength(17);
                SetSalesCodeStartIndex(233);
            }
        }
        // Determine if NISP support is required
        XmlNodeMapCItr iter = document->getChild("Setup")->getChildren().find("NispSupportedVehicles");
        if (iter != document->getChild("Setup")->getChildren().end())
        {
            Log(LOG_DEV_DATA, "Loading NISP selection criteria");
            m_nispSupport.StoreNispVehicleCriteria(iter->second);
        } else
        {
            Log(LOG_DEV_DATA, "NISP is not suported, not loading NISP criteria.");
        }

        try
        {
            m_plcVehicleBuildBits.DeepCopy(document->getChild("Setup/PlcBuildData/PlcVehicleBuildBits")->getChildren());
            Log(LOG_DEV_DATA,"deepcopy worked\n");
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "Failure to find the PlcVehicleBuildBits children: %s\n", excpt.GetReason());
        }
        

        string plcDataTag = "";
        string vehicleBuildTag = "";

        for(XmlNodeMapCItr iter = m_plcVehicleBuildBits.begin();
          (iter != m_plcVehicleBuildBits.end());
          iter++)
        {
            try
            {
                // xml node something like this<AlcBuildInfo/Element001 plcDataTag="ReadAbsFromPlc">ABSEquipped</AlcBuildInfo/Element001>
                plcDataTag = iter->second->getAttribute("plcDataTag")->getValue();
                vehicleBuildTag = iter->second->getValue();

                Log(LOG_DEV_DATA, "plcDataTag: %s  vehicleBuildTag: %s", plcDataTag.c_str(), vehicleBuildTag.c_str());



            }
            catch(XmlException &excpt)
            {
               Log(LOG_ERRORS, "Issue reading PLC map: %s\n", excpt.GetReason());
            }
        }


        Log(LOG_FN_ENTRY, "WorkCellController::Initialize() complete\n");
    } else
    {
        Log(LOG_ERRORS, "WARNING: config is NULL, cannot initialize WorkCellController!");
    }
}

const std::string WorkCellController::Register(void)
{  // Initialize the PromptServer interface object
    Log(LOG_DEV_DATA, "WorkCellController::Register() - Enter\n");
    // Broadcast comm object should always be started.  This server allows dynamically changing of the
    // build record source.  If the server comes up with File as the source, you should still be able to
    // dynamically change to broadcast.
/*
    m_wccComm.Initialize(GetBroadcastCommSetup());
    Log(LOG_DEV_DATA, "WorkCellController::Register - Initialized broadcast communications\n");
    // Check if request handler thread should be started
    if (StartRequestHandler())
    {   // Initialize the wcc request handler
        m_requestHandler = new WccRequestHandler(this);
        m_requestHandler->Initialize(GetRequestHandlerConfig());
        m_requestHandler->Resume();
        Log(LOG_DEV_DATA, "WorkCellController::Register - Started request handler");
    } else
    {
        Log(LOG_DEV_DATA, "WorkCellController::Register - Not starting request handler thread.");
    }
    // Check if the keep alive thread should be started
    if (StartKeepAlive())
    {   // Get the machine number
        Log(LOG_DEV_DATA, "WorkCellController::Register - Starting keep alive thread");
        INT32 machineNumber = BposReadInt(getenv("MACHINE"));
        Log(LOG_DEV_DATA, "WorkCellController::Register - Retrieved machine number for keep alive thread: %d", machineNumber);
        m_keepAlive = new WccKeepAlive(this, machineNumber);
        Log(LOG_DEV_DATA, "WorkCellController::Register - Created keep alive thread");
        m_keepAlive->Initialize(GetKeepAliveConfig());
        Log(LOG_DEV_DATA, "WorkCellController::Register - Initialized keep alive thread");
        m_keepAlive->Resume();
        Log(LOG_DEV_DATA, "WorkCellController::Register - Keep alive thread is running");
    } else
    {
        Log(LOG_DEV_DATA, "WorkCellController::Register - Not starting keep alive thread.");
    }
    // Check if static tester comm should be started
    if (StartStaticTesterServerComms())
    {
        m_staticTesterComm = new IBepCommunication();
        m_staticTesterComm->Initialize(GetDataTag("StaticTesterServer"), "Client", IsDebugOn(), 5000);
    } else
    {
        Log(LOG_DEV_DATA, "WorkCellController::Register - Not starting StaticTesterServer comms.");
    }
*/
    Log(LOG_DEV_DATA, "WorkCellController::Register - Completing registration by calling base class");
    return(PlantHostInbound::Register());
}

const std::string WorkCellController::Publish(const XmlNode *node)
{
    std::string result = BEP_ERROR_RESPONSE, response, loadResult;
    Log(LOG_FN_ENTRY, "WorkCellController::Publish(%s)\n", node->ToString().c_str());
    result = BepServer::Publish(node);
    // Only do special stuff if publish was successful
    if (result == BEP_SUCCESS_RESPONSE)
    {
        if (node->getName() == GetVehiclePresentTag())
        {  // Update the DataInputServer state 
            UpdateInputServerState();
        } else if (node->getName() == GetNextVinTag())
        {   // Store the vin to be retrieved
            Log(LOG_DEV_DATA, "Storing VIN to load - %s\n", node->getValue().c_str());
            SetVINToLoad(node->getValue());
            // Determine where to retrieve the build data from
            XmlParser parser;
            const XmlNode *vehicleBuildSource = parser.ReturnXMLDocument(GetConfigFile())->getChild("Setup/VehicleBuildRecordSource");
            SetVehicleBuildSource(vehicleBuildSource->getValue());
            // Signal to get data from broadcast
            Log(LOG_DETAILED_DATA, "Signaling to retrieve broadcast data\n");
            m_getBuildRecord.Broadcast(getBuildRecord);
            BposSleep(10);   // Make sure signal is received
            // Clear the signal for the next scan
            m_getBuildRecord.Broadcast(doNotGetBuildRecord);
            Log(LOG_DETAILED_DATA, "Clearing signal to retrieve broadcast data\n");
        } else if ((node->getName() == START_VEHICLE_TEST_DATA_TAG) && (node->getValue() == "1"))
        {   // Invalidate the vehicle build record and update traffic light
            SetVehicleBuildRecordStatus(BEP_INVALID_RESPONSE);
            m_broker->Write(GetVinReadStatusTag(), NO_VIN, response, true);
        }
    }
    Log(LOG_FN_ENTRY, "WorkCellController::Publish(%s) - Exit\n", result.c_str());
    return(result);
}

inline const string WorkCellController::Read(const std::string &tag, const INT32 rate) /*=0*/
{
    return PlantHostInbound::Read(tag, rate);
}

const string WorkCellController::Read(const XmlNode *node, const INT32 rate) /*= 0*/
{
    string result = BEP_ERROR_RESPONSE;
    if (!node->getName().compare(GetDataTag("StaticTesterBuildRequest")))
    {   // Get the build data for the requested vin
        // Command threads to ignore incoming data
        IgnoreIncomingData(true);
        result = LoadVehicleBuildRecord(node->getValue(), m_staticTesterBuild, false);
        // Tell threads to resume processing incoming data
        IgnoreIncomingData(false);
        // If build data was retrieved, put it into string form and return
        if (!result.compare(BEP_SUCCESS_RESPONSE))
        {   // Good response
            result.erase();
            for (XmlNodeMapItr iter = m_staticTesterBuild.begin(); iter != m_staticTesterBuild.end(); iter++)
            {   // Build up the record to be passed back
                result += iter->second->ToString();
            }
        } else
        {
            Log(LOG_ERRORS, "Could not load build data for %s:%s", node->getName().c_str(), node->getValue().c_str());
        }
    } else
    {   // Call the base class to read the data
        result = PlantHostInbound::Read(node, rate);
    }
    return result;
}

void WorkCellController::Run(volatile bool *terminateFlag) /*=NULL*/
{
    Log(LOG_FN_ENTRY, "WorkCellController::Run() - begin\n");
    while (GetStatus() != BEP_TERMINATE)
    {   // Acquire the semaphore
        m_getBuildRecord.Acquire();
        // Wait until we are signaled
        m_getBuildRecord.Wait(getBuildRecord);
        // Release the semaphore
        m_getBuildRecord.Release();
        // Command threads to ignore incoming data
        IgnoreIncomingData(true);
        // Process the request
        Log(LOG_DEV_DATA, "Retrieving build record data from %s\n", GetVehicleBuildSource().c_str());
        std::string loadResult = RetrieveBuildRecord();
        if (loadResult != BEP_SUCCESS_RESPONSE)  // Error loading the build record
            Log(LOG_DEV_DATA, "Error retrieving build record data - loadResult:%s\n", loadResult.c_str());
        else                                     // Build record was loaded succesfully
            Log(LOG_DEV_DATA, "Retrieved build record data - loadResult:%s\n", loadResult.c_str());

        // Tell threads to resume processing incoming data
        IgnoreIncomingData(false);
    }
    Log(LOG_FN_ENTRY, "WorkCellController::Run() - complete\n");
}

const std::string WorkCellController::RetrieveBuildRecord(void)         
{
    string response;
    string loadResult = BEP_FAILURE_RESPONSE;
    Log(LOG_FN_ENTRY, "WorkCellController::RetrieveBuildRecord() - begin\n");
    // Only attempt to load vehicle build record if valid vin
    if (GetVINToLoad().length() >= minVinLength)
    {  // Update the traffic light widget to let driver know vin is being processed
        Log(LOG_DEV_DATA, "Updating traffic light to %s", PROCESSING_VIN);
        m_broker->Write(GetVinReadStatusTag(), PROCESSING_VIN, response, true);
        // Load the vehicle build record for the specified vin
        string vinRequest("?????????????????" + GetVINToLoad());
        vinRequest = vinRequest.substr(vinRequest.length() - vinLength);
        Log(LOG_DEV_DATA, "Retrieving build record for <%s>\n", vinRequest.c_str());
        // Check the static tester server first if it is up and running
        if (m_staticTesterComm != NULL)
        {
            XmlNode buildRequest(GetDataTag("BuildData"), GetVINToLoad());
            XmlNode *buildData = NULL;
            string response;
            if (m_staticTesterComm->Read(&buildRequest, response, true) == BEP_STATUS_SUCCESS)
            {   // Got the build data from the static tester server
                XmlNode temp("", "");
                m_staticTesterComm->GetNext(temp, response);
                buildData = temp.Copy();
                if (buildData != NULL)
                {
                    if (buildData->getValue().compare(BEP_UNAVAILABLE_RESPONSE))
                    {
                        XmlParser parser;
                        SetVehicleBuild(parser.ReturnXMLDocument(buildData->getValue())->getChild(VEHICLE_BUILD_TAG), m_vehicleBuild);
                        loadResult = BEP_SUCCESS_RESPONSE;
                        // Set the vehicle build status to valid
                        SetVehicleBuildRecordStatus(validStatus);
                        m_broker->Write(GetVinReadStatusTag(), READY_TO_TEST, response, true);
                        Log(LOG_DEV_DATA, "Retrieved build data from StaticTesterServer");
                    } else
                    {
                        loadResult = BEP_UNAVAILABLE_RESPONSE;
                    }
                    // Clean up to prevent memory leaks
                    delete buildData;
                    buildData = NULL;
                } else
                {
                    loadResult = BEP_FAILURE_RESPONSE;
                }
            } else
            {
                loadResult = BEP_FAILURE_RESPONSE;
            }
        } else
        {
            Log(LOG_DEV_DATA, "StaticTester not in use, retrieving build data from %s", GetVehicleBuildSource().c_str());
            loadResult = BEP_FAILURE_RESPONSE;
        }

        // If the vehicle build has not been loaded, do so now.
        if (loadResult.compare(BEP_SUCCESS_RESPONSE))
        {
            Log(LOG_DEV_DATA, "WCC::RetrieveBuildRecord load build record now");
            loadResult = LoadVehicleBuildRecord(vinRequest, m_vehicleBuild, true);
            Log(LOG_DEV_DATA, "WCC::RetrieveBuildRecord loadResult: %s", loadResult.c_str());
        }

        // Verify the vehicle build data was loaded
        if (loadResult == BEP_SUCCESS_RESPONSE)
        {  // Publish the vehicle build to any subscribers
            Log(LOG_DEV_DATA, "WCC::RetrieveBuildRecord successfully loaded");
            XmlNode nextVehicleBuild(GetVehicleBuildTag(), "");
            for (XmlNodeMapItr iter = m_vehicleBuild.begin(); iter != m_vehicleBuild.end(); iter++)
            {   // Add the build item to the node
                nextVehicleBuild.addChild(iter->second->Copy());
            }
            // Write the next build data
            Log(LOG_DEV_DATA, "Writing build data to the system: %s", nextVehicleBuild.ToString().c_str());
            Write(&nextVehicleBuild);
            // Get rid of the node
            nextVehicleBuild.clear();
        } else
        {
            Log(LOG_DEV_DATA, "WCC::RetrieveBuildRecord problem loading vehicle build");
        }
    } else
    {   // Invalid VIN 
        Log(LOG_ERRORS, "Invalid VIN <%s>, can not retrieve build record.  Returning %s\n", 
            GetVINToLoad().c_str(), loadResult.c_str());
        BposSleep(80); // so we dont get a bunch of them
    }

    Log(LOG_DEV_DATA, "WCC::RetrieveBuildRecord - Exit status: %s", loadResult.c_str());
    return(loadResult);
}

void WorkCellController::AddDerivedBuildInfo(XmlNodeMap &buildData)
{  // Determine drive axle configuration based on the body style and add to vehicle build
    Log(LOG_FN_ENTRY, "WorkCellController::AddDerivedBuildInfo()\n");
    if (!UseSalesCodes())
    {
        std::string bodyStyle = GetVehicleBuildItem(BODY_STYLE_TAG, buildData);
        std::string driveAxle = GetDriveAxle(bodyStyle);
        Log(LOG_DEV_DATA, "BodyStyle: %s\nDrive Axle: %s\n", bodyStyle.c_str(), driveAxle.c_str());
        AddVehicleBuildItem(DRIVE_AXLE_TAG, driveAxle, buildData);
        if (driveAxle == REAR_WHEEL_DRIVE_VALUE)
        {  // Set the PLC data for a rear wheel drive vehicle
            AddVehicleBuildItem(GetFrontWheelDriveTag(), "No", buildData);
            AddVehicleBuildItem(GetRearWheelDriveTag(), "Yes", buildData);
        } else
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
        // Determine the air pressure for the adjustable regulator
        AddVehicleBuildItem(GetAirPressureTag(), GetAirPressureSetting(bodyStyle), buildData);
        // Add flag to indicate PFS block with a value of yes or no.  This is to accomodate the
        // VehicleDataBroker sending PFS block information to the PLC.
        string pfsBlock = (GetVehicleBuildItem(PERFORM_ABORT_TEST, buildData).compare("Perform") ? "Yes" : "No");
        AddVehicleBuildItem(GetDataTag("PfsBlockTag"), pfsBlock, buildData);
        // Add body style identifier.  This is for PLC reporting to FIS.
        AddVehicleBuildItem(GetDataTag("BodyStyleIndexTag"), GetBodyStyleIndex(bodyStyle), buildData);

        //check the VINs
        string vin = GetVINToLoad();
        Log(LOG_DEV_DATA, "WCC::AddDerivedBuildInfo Setting the current VIN: %s", vin.c_str());
        AddVehicleBuildItem("VIN", vin, buildData);


    } else
    {   // Default to rear wheel drive to PLC
        AddVehicleBuildItem(DRIVE_AXLE_TAG, REAR_WHEEL_DRIVE_VALUE, buildData);
        // Set the PLC data for a rear wheel drive vehicle
        AddVehicleBuildItem(GetFrontWheelDriveTag(), "No", buildData);
        AddVehicleBuildItem(GetRearWheelDriveTag(), "Yes", buildData);
    }
    // Add the flag specifying if this is a NISP supported vehicle
    AddVehicleBuildItem(GetDataTag("NispSupportedTest"), m_nispSupport.IsNispSupported(&buildData) ? "Yes" : "No", buildData);
    Log(LOG_FN_ENTRY, "WorkCellController::AddDerivedBuildInfo() complete\n");
}

const std::string WorkCellController::HasAllWheelDrive(const std::string &transferCaseType)
{
    std::string result("No");
    try
    {  // The vehicle is equipped with all wheel drive
        result = m_allWheelDrive.getNode(transferCaseType)->getValue();
    } catch (XmlException &XmlErr)
    {  // The vehicle is not equipped with all wheel drive
        result = "No";
    }
    return(result);
}

const std::string WorkCellController::HasFourWheelDrive(const std::string &transferCaseType)
{
    std::string result("Yes");
    Log(LOG_FN_ENTRY, "WorkCellController::HasFourWheelDrive(transferCaseType:%s)\n", transferCaseType.c_str());
    try
    {  // The vehicle is not equipped with four wheel drive
        result = m_fourWheelDrive.getNode(transferCaseType)->getValue();
    } catch (XmlException &XmlErr)
    {  // The vehicle is equipped with four wheel drive
        Log(LOG_ERRORS, "Did not find %s in map\n", transferCaseType.c_str());
        result = "Yes";
    }
    return(result);
}

const std::string WorkCellController::GetDriveAxle(const std::string &bodyStyle)
{
    Log(LOG_FN_ENTRY, "WorkCellController::GetDriveAxle(%s)\n", bodyStyle.c_str());
    string driveAxle;
    driveAxle = GetDataItemFromList(bodyStyle, &m_bodyStyleDriveAxle);
    Log(LOG_FN_ENTRY, "WorkCellController::GetDriveAxle(%s) - complete, returning drive axle: %s\n", bodyStyle.c_str(), driveAxle.c_str());
    return(driveAxle);
}

const string WorkCellController::GetDataItemFromList(const string &dataItem, const XmlNodeMap *itemList)
{   // Set the default value to use
    string dataValue(BEP_NO_DATA);
    INT32  dataWeight = 0;
    bool foundItem = false;
    Log(LOG_FN_ENTRY, "WorkCellController::GetDataItemFromList(%s)\n", dataItem.c_str());
    // Make sure we are using a valid item list
    if (!itemList->empty())
    {   // Loop through the item list looking for the data item that closely matches
        for (XmlNodeMapCItr iter = itemList->begin(); (iter != itemList->end()) && !foundItem; iter++)
        {   // Evaluate the current mask against our data item
            bool   match = false;
            INT32  tempWeight = 0;
            UINT16 index = 0;
            string currentItem = iter->second->getName();
            do
            {
                match = ((currentItem[index] == 'x') || (currentItem[index] == dataItem[index]));
                if (match && (currentItem[index] == dataItem[index]))
                {   // Only update if a match and not a don't care position
                    tempWeight++;
                }
            } while (match && (index++ < currentItem.length()));
            // Update the selected item as needed
            if (match && (tempWeight > dataWeight))
            {
                dataWeight = tempWeight;
                dataValue  = iter->second->getValue();
                foundItem  = true;
            }
        }
        // Check to ensure an item was selected.  If not, use the default from the list
        if (!foundItem)
        {   // Look for the default item
            Log(LOG_ERRORS, "Could not find entry for %s, checking for default value.", dataItem.c_str());
            XmlNodeMapCItr iter = itemList->find(DEFAULT_NODE);
            if (iter != itemList->end())
            {
                dataValue = iter->second->getValue();
                Log(LOG_DEV_DATA, "Using default value %s for data item %s", dataValue.c_str(), dataItem.c_str());

            } else
            {
                Log(LOG_ERRORS, "No default specified, returning %s", dataValue.c_str());
            }
        }
    } else
    {   // Empty list, not finding anything
        Log(LOG_ERRORS, "ERROR: Item list provided for data item %s is empty!  Returning %s", 
            dataItem.c_str(), dataValue.c_str());
    }
    Log(LOG_FN_ENTRY, "WorkCellController::GetDataItemFromList(%s) - complete, returning: %s\n", 
        dataItem.c_str(), dataValue.c_str());
    return(dataValue);
}

const string WorkCellController::GetBodyStyleIndex(const string &bodyStyle)
{
    return(GetDataItemFromList(bodyStyle, &m_bodyStyleIndex));
}

bool WorkCellController::LockWccCommPort(SerialChannel *commObject)
{
    bool portLocked = false;
    // Attempt to lock the port
    if (GetPortLockedStatus() == false)
    {
        try
        {   // Set a timer so we do not hang attempting to lock the port
            sigevent event;                         // Event structure informing what to do if timed out
            event.sigev_notify = SIGEV_UNBLOCK;     // Unblock if time out
            uint64_t timeout = mSEC_nSEC(5000);     // Set timeout value - 5000ms
            TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_REPLY, &event, &timeout, NULL);
            // Block the channel so the PFS results do not interfere with us getting data
            if (!portLocked) portLocked = commObject->LockPort();
            Log(LOG_DEV_DATA, "Port locked : %s\n", (portLocked ? "TRUE" : "FALSE"));
            SetPortLockedStatus(portLocked);
        } catch (BepException &excpt)
        {
            Log(LOG_ERRORS, "BepException attempting to lock the port: %s", excpt.GetReason());
            portLocked = false;
        } catch (...)
        {
            Log(LOG_ERRORS, "Unknown exception: Could not lock the port in WorkCellController::LockWccCommPort()");
            portLocked = false;
        }
    } else Log(LOG_DEV_DATA, "Port already locked: GetPortLockedStatus: %s\n", (GetPortLockedStatus() ? "TRUE" : "FALSE"));
    return(portLocked);
}

bool WorkCellController::UnlockWccCommPort(SerialChannel *commObject)
{   // Unblock the channel for others to use
    SetPortLockedStatus(false);
    return(!(commObject->UnlockPort()));
}

void WorkCellController::IgnoreIncomingData(bool ignoreIncomingData) /*= true*/
{
    if (m_requestHandler)
    {
        if (ignoreIncomingData)
        {
            Log(LOG_DEV_DATA, "Signaling request handler to ignore incoming data");
            m_requestHandler->IgnoreIncomingData();     
        } else
        {
            Log(LOG_DEV_DATA, "Signaling request handler to resume processing incoming data");
            m_requestHandler->ClearIncomingDataBuffer();
            m_requestHandler->ProcessIncomingData();
        }
    }
}

const std::string WorkCellController::ReadBuildRecordFromBroadcast(const std::string &vin, 
                                                                   std::string &buildRecord,
                                                                   const bool &updateStatus)
{
    std::string locCmd;
    locCmd = (char)ENQ;
    return(ReadBuildRecordFromBroadcast(locCmd, vin, buildRecord, updateStatus));
}

const std::string WorkCellController::ReadBuildRecordFromBroadcast(const std::string &command, 
                                                                   const std::string &vin, 
                                                                   std::string &buildRecord,
                                                                   const bool &updateStatus)
{
    Log(LOG_FN_ENTRY, "WorkCellController::ReadBuildRecordFromBroadcast()\n");
    string result(BEP_UNAVAILABLE_RESPONSE);
    string response;
    INT32 status;
    INT32 attempts = GetMaxAttempts();
    bool portLocked = false;
    // Clear the prompt boxes in case a prompt was previously dosplayed
    if (AreRetainersDown() && updateStatus)
    {
        string response;
        m_promptComm.ClearPromptBox(1, response);
        m_promptComm.ClearPromptBox(2, response);
    }


    const short vinStringLength = 17;
    const int alcResponseTimeoutTime = 6000;
    bool failureToWriteVin = false;
    char buff[16];
    char vinChar[2];
    short vinPos = 0;

    // write the VIN to the PLC bits
    Log(LOG_DEV_DATA, "ReadBuildRecord - VIN: %s   vinLength:%d  vinMaxLength:%d", vin.c_str(), vin.length(), vinStringLength);
   

    // parse vin to place in correct plc words (software was purposefully made explicit for easy debug)
    if(vin.length() >= vinStringLength)
    {
        
        for (int i = 0; i < 9; i++)
        {
            vinChar[0] = vin[vinPos++];
            vinChar[1] = vin[vinPos++];

            string plcDataTag = "";

            switch(i)
            {
            case 0:
                plcDataTag = "VinBytes0and1";
                break;
            case 1:
                plcDataTag = "VinBytes2and3";
                break;
            case 2:
                plcDataTag = "VinBytes4and5";
                break;
            case 3:
                plcDataTag = "VinBytes6and7";
                break;
            case 4:
                plcDataTag = "VinBytes8and9";
                break;
            case 5:
                plcDataTag = "VinBytes10and11";
                break;
            case 6:
                plcDataTag = "VinBytes12and13";
                break;
            case 7:
                plcDataTag = "VinBytes14and15";
                break;
            case 8:
                plcDataTag = "VinBytes16";
                break;
            }


            Log(LOG_DEV_DATA, "CreateMessage plcDataTag: %s   vin bytes to plc: >%c%c<", plcDataTag.c_str(), vinChar[0], vinChar[1]);
            string vinStr(CreateMessage(buff, sizeof(buff), "%c%c", vinChar[0], vinChar[1]));

            // write the two characters to the plc word
            if (m_broker->Write(plcDataTag, vinStr, result, true) != BEP_STATUS_SUCCESS)
            {
                Log(LOG_ERRORS, "Error writing vin string - plcDataTag: %s   vin bytes to plc: >%c%c<", plcDataTag.c_str(), vinChar[0], vinChar[1]);
                failureToWriteVin = true;
            }
        }

    }
    else
    {
        Log(LOG_ERRORS, "Vin is to short. length: %d < vinLength: %d", vin.length(),  vinStringLength);
    }

    Log(LOG_DEV_DATA, "Trying to do a system read"); 
    string r = Read("VehicleBuildDataOk");
    Log(LOG_DEV_DATA, "System read response: %s", r.c_str());


    if (!failureToWriteVin)
    {
        Log(LOG_DEV_DATA, "Writing a bit to the system to tell it that the aon has been updated");
        // set vin set bit, if there were no failures to write the aon
        status = m_broker->Write("GetBuildDataFromSystem", "1", result, true);
    
        if (status == BEP_STATUS_SUCCESS)
        {
            int timeRemaining = GetMaxResponseTime();
    
            // wait for status bit to be cleared by plc saying all of the build information has been placed
            while(response != "1" && timeRemaining > 0)
            {
                Log(LOG_DEV_DATA, "Waiting for the build data to be set");
                m_broker->Read("VehicleBuildDataOk", response, true);
    
    
                delay(100);
                timeRemaining -= 100;

                Log(LOG_DEV_DATA, "VehicleBuildDataOk: %s (response length:%d)   %dms remaining", response.c_str(), response.length(), timeRemaining); 
    
            }


            if(response == "0")
            {
                Log(LOG_DEV_DATA, "ReadBuildRecordFromBroadcast - success");
                result = BEP_SUCCESS_RESPONSE;
            }
            else if (timeRemaining <= 0)
            {
                Log(LOG_DEV_DATA, "ReadBuildRecordFromBroadcast - timeout");
                result = BEP_TIMEOUT_RESPONSE;
            }
            else
            {
                Log(LOG_DEV_DATA, "ReadBuildRecordFromBroadcast - failure");
                result = BEP_FAILURE_RESPONSE;
            }
    
        }
        else
        {
            Log(LOG_ERRORS, "Error writing SearchForVehicleInfo plc tag to ndb");
        }
    }
    else
    {
        Log(LOG_ERRORS, "Error writing vin. Will not requstest VIN from PLC");
        // todo put up alc comm error?
    }

    Log(LOG_DEV_DATA, "REMOVE!! - Take this out when done with development. You have this in to overwrite plc bits");
    result = BEP_SUCCESS_RESPONSE;

    // Return the status
    Log(LOG_DEV_DATA, "WorkCellController::ReadBuildRecordFromBroadcast() done: %s -- buildInfo:%s\n", result.c_str(), buildRecord.c_str());
    return(result);
}



BEP_STATUS_TYPE WorkCellController::GetBroadcastResponse(std::string &response, const std::string &vin, const bool &updateStatus)
{
    SerialString_t wccResponse;
    INT32 numBytesRead;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Clear the buffer
    response.erase();
    Log(LOG_FN_ENTRY, "WorkCellController::GetBroadcastResponse()\n");
    // Clear the prompt boxes if retainers are down
    INT32 promptBox = 2;
    std::string clearResponse;
    if (AreRetainersDown() && updateStatus) m_promptComm.ClearPromptBox(promptBox, clearResponse);
    // Get the data from the buffer
    numBytesRead = m_wccComm.ReadPort(wccResponse, GetMaxResponseTime(), m_wccComm.GetTimeoutTime());
    Log("Got <0x%02X> from broadcast [ACK:0x%02X, NAK:0x%02X] - %d total bytes\n", wccResponse[0], ACK, NAK, numBytesRead);
    // If we got data, save it
    if (numBytesRead > 0)
    {   // Determine what type of message this is
        if ((ACK == wccResponse[0]) && !((numBytesRead > 1) && (NO_INFO == wccResponse[1])))
        {
            // Check for a Clock indication
            // ask dan about this
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (wccResponse[1] != CLOCK)
            {
                Log(LOG_DEV_DATA, "No clock sent from broadcast\n");
                wccResponse[1] = 0x01;         // Arbitrary so we do not get misinterpretted end of string
            }
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Log the data we received from the workcell controller
            char buf[1024];
            string logMessage("Complete response from Broadcast:\n");
            for (UINT16 index = 0; index < numBytesRead; index++)
            {   // Create the log message
                logMessage += CreateMessage(buf, sizeof(buf), "\t\tindex: %03d -- %c <0x%02X>\n", 
                                            index, isprint(wccResponse[index]) ? wccResponse[index] : '?', wccResponse[index]);
            }
            Log(LOG_DEV_DATA, "%s\n", logMessage.c_str());
            status = BEP_STATUS_SUCCESS;
        } else if (NAK == wccResponse[0])
        {  // Display a prompt letting everyone know a NAK was received
            string resp;
            Log(LOG_DEV_DATA, "Recieved NAK from Broadcast\n");
            status = BEP_STATUS_FAILURE;
            if (AreRetainersDown() && updateStatus) m_promptComm.DisplayPrompt(2, GetNAKPrompt(), resp);
        } else if (NO_INFO == wccResponse[1])
        {  // Broadcast has no information for the requested vin
            Log(LOG_DEV_DATA, "Broadcast has NO information for the requested Vin (B): %s\n", vin.c_str());

            Log(LOG_DEV_DATA, "AreRetainersDown: %s   updateStatus: %s ", (AreRetainersDown() ? "True" : "False"), (updateStatus ? "True" : "False"));
            if (AreRetainersDown() && updateStatus)
            {  // No test in progress, display the prompt
                Log(LOG_DEV_DATA, "Trying to show no broadcast message in ccrt");
                INT32 promptBox = 2;
                string result, detailTag("Param00");
                PromptDetails noInfoDetails;
                noInfoDetails.AddDetail(detailTag, vin);
                m_promptComm.DisplayPrompt(promptBox, GetNoInfoPrompt(), noInfoDetails, result); 
            }
            else
            {
                Log(LOG_DEV_DATA, "We would like to tell you we can't get data from broadcast but we think a test is in progress.");
            }
            status = BEP_STATUS_NA;
        }
        // Save the response to pass back
        Log(LOG_DEV_DATA, "Preparing response for processing...\n");
        for (UINT16 index = 1; index < numBytesRead; index++) response += (char)wccResponse[index];
    } else
    {  // No response received from broadcast
        string resp;
        Log(LOG_ERRORS, "Timeout waiting for a response from broadcast\n");
        if (AreRetainersDown() && updateStatus) m_promptComm.DisplayPrompt(2, this->GetNoResponsePrompt(), resp);
    }
    Log(LOG_FN_ENTRY, "WorkCellController::GetBroadcastResponse() complete - %s\n", ConvertStatusToResponse(status).c_str());
    return status;
}

const BEP_STATUS_TYPE WorkCellController::EvaluateResponse(std::string &response, 
                                                           const std::string &vin, 
                                                           const bool &updateStatus)
{
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    INT32 promptBox = 2;
    std::string result;
    static bool clockSet = false;   // Only need to update the clock the first time through
// setting the clock is broken in code. Some day, we should fix it or dump it.
    //static bool clockSet = true;   // Only need to update the clock the first time through
    bool goodMessage = false;

    Log(LOG_DEV_DATA,"WorkCellController::EvaluateResponse() - Enter\n");
    goodMessage = (response.length() >= GetValidWccResponseLength());
    // Check if the section lengths need to be retrieved from the message
    if (UseSectionLengthData() && goodMessage)
    {
        StoreSectionLengths(response);
    }
    //Must store section lengths before GetValidMessageLengths()
    UINT32 valMsgLen = GetValidMessageLength();
    goodMessage = goodMessage ? (response.length() >= valMsgLen) : false;
    Log(LOG_DEV_DATA, "Message length: %d, validMessageLength: %d goodMessage: %d\n",
        response.length(), GetValidMessageLength(), goodMessage);

    if (goodMessage)
    {  // Check to see if we need to set the system date/time
        Log(LOG_DEV_DATA, "Good Message Detected\n");
        bool setClock = (response[0] == (char)CLOCK);
        Log(LOG_DEV_DATA, "Good Message Detected2\n");
        // Get the check sum from the message and remove the trailer
        Log(LOG_DEV_DATA, "Checksum Index: %d", GetChecksumIndex());
        response = response.substr(1,response.length());
        UINT16 wccCheckSum = (unsigned char)response[GetChecksumIndex()];
        string logMessage;
        logMessage.reserve(response.length() + 64);
        char buf[1024];

        UINT16 howMany = response.length();
        if (howMany > 10) howMany = 10;

        for (UINT16 ii = 0; ii < howMany; ii++)
        {
            logMessage += CreateMessage(buf, sizeof(buf), "\t\tii:%03d, %c <0x%02X>\n", 
                                        ii, (isprint(response[ii]) ? response[ii] : '?'), (unsigned char)response[ii]);
        }

        Log(LOG_DEV_DATA, "Truncated response to: >%s<\n%s\n", response.c_str(), logMessage.c_str());

        // Make sure the checksum of the response is valid
        response = response.substr(0,GetChecksumIndex());

        UINT8 checkSum = CalculateCheckSum(response) & 0x00FF;

        if (checkSum != wccCheckSum)
        {  // We got a bad checksum, message is not valid
            Log("Bad checksum from broadcast -- calculated:%d[$%02X], broadcast:%d[$%02X]\n", checkSum, checkSum, wccCheckSum, wccCheckSum);
            if (AreRetainersDown() && updateStatus)
            {  // No test in progress, display the prompt
                m_promptComm.DisplayPrompt(promptBox, GetBadChecksumPrompt(), result);
            }
        } else
        {  // Received a valid message from broadcast
            Log(LOG_DEV_DATA,"Good checksum from broadcast:%d[$%02X]\n",checkSum, checkSum);
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
                if (mktime(&broadcastDate) == (time_t)-1) badDate = true;

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
                } else if (badDate)
                {
                    Log(LOG_ERRORS, "Bad date received from broadcast, not setting date/time -- month:%s, day:%s, year:%s\n", 
                        month.c_str(), day.c_str(), year.c_str());
                } else
                {
                    Log(LOG_DEV_DATA, "System date and time match date and time from WorkCellController");
                    clockSet = true;
                }
            } else if (clockSet)
            {
                Log(LOG_DEV_DATA, "System clock previously set to match broadcast, not updating");
            } else
            {
                Log(LOG_DEV_DATA, "No date or time stamp from broadcast, not setting system time");
            }
        }
    } else
    {  // Unknown message received from broadcast
        Log(LOG_ERRORS, "Unknown response from broadcast for Vin: %s, message: %s\n", vin.c_str(), response.c_str());
        if (AreRetainersDown() && updateStatus)
        {  // Retainers are down, display prompt
            // The first byte get stripped off. Example NO_INFO raw data = $06 $11 $01 $0D returned to here = $11 $01 $0D.
            // 1. Just get ACK which gets stripped so response message length = 0
            // 2. Get NAK message length = 3.
            // 3. Get NO_INFO length = 3.
            // 4. Bad data message length = varible.
            // 5. No data message length = 0 - is currently handled by the calling method (no error displayed).
            if ((response.length() > 0) && (NO_INFO == response[0]))
            {
                Log(LOG_DEV_DATA, "Broadcast has NO information for the requested Vin (A): %s\n", vin.c_str());
                string result, detailTag("Param00");
                PromptDetails noInfoDetails;
                noInfoDetails.AddDetail(detailTag, vin);
                m_promptComm.DisplayPrompt(promptBox, GetNoInfoPrompt(), noInfoDetails, result); 
            } else if ((response.length() > 0) && (NAK == response[0]))
            {
                string resp;
                Log(LOG_DEV_DATA, "Recieved NAK from Broadcast\n");
                m_promptComm.DisplayPrompt(2, GetNAKPrompt(), resp);
            } else if (response.length() <= 0)
            {
                string resp;
                Log(LOG_ERRORS, "Timeout waiting for a response from broadcast\n");
                m_promptComm.DisplayPrompt(promptBox, GetNoResponsePrompt(), resp);
            } else
            {
                Log(LOG_ERRORS, "Unknown response from broadcast for Vin: %s, message: %s\n", vin.c_str(), response.c_str());
                m_promptComm.DisplayPrompt(promptBox, GetUnknownMessagePrompt(), result);
            }
        }
    }
    Log(LOG_FN_ENTRY, "WorkCellController::EvaluateResponse() done, returning: %d\n", status);
    return(status);
}
void WorkCellController::TranslateBuildRecord(const std::string &buildRecord, 
                                              XmlNodeMap &buildData, 
                                              const bool &updateStatus)
{   // Translate the build record

    Log(LOG_FN_ENTRY, "WorkCellController::TranslateBuildRecord - Enter");

    string plcDataTag = "";
    string vehicleBuildTag = "";
    string response = "";
    string result = "";
    string value = "";
    // use the elements in the config file to check to see if bits are set and then add elements to the build record from that
    for(XmlNodeMapCItr iter = m_plcVehicleBuildBits.begin();
      (iter != m_plcVehicleBuildBits.end());
      iter++)
    {
        try
        {
            // xml node something like this<AlcBuildInfo/Element001 plcDataTag="ReadAbsFromPlc">ABSEquipped</AlcBuildInfo/Element001>
            // ABSEquipped - VDS node
            // ReadAbsFromPlc - PLC tag name
            plcDataTag = iter->second->getAttribute("plcDataTag")->getValue();
            vehicleBuildTag = iter->second->getValue();

            Log(LOG_DEV_DATA, "plcDataTag: %s  vehicleBuildTag: %s", plcDataTag.c_str(), vehicleBuildTag.c_str());

            value = ReadPlcBit(plcDataTag);
                
            Log(LOG_DEV_DATA, "ndb read - plcDataTag: %s   value: %s", plcDataTag.c_str(), value.c_str());

            // check to see if the bit was high
            if(value == "1")
            {
                AddVehicleBuildItem(vehicleBuildTag, "1", buildData);
            }


        }
        catch(XmlException &excpt)
        {
           Log(LOG_ERRORS, "Issue reading PLC map: %s\n", excpt.GetReason());
        }
    }
    
    Log(LOG_FN_ENTRY, "WorkCellController::TranslateBuildRecord - Exit");
}
string WorkCellController::ReadPlcBit(string tag)
{
    std::string value, response;
    if(m_broker != NULL)
    {
        INT32 status = m_broker->Read(tag, response, true);
        if(status == BEP_STATUS_SUCCESS)
            status = m_broker->GetByTag(tag, value, response);
        // check for errors
        if(status != BEP_STATUS_SUCCESS)
        {
            Log(LOG_ERRORS, "Error Reading: %s, %d\n", tag.c_str(), status);
            value = "ERROR";
        }
    }
    else
    {
        Log(LOG_ERRORS, "WorkCellController::ReadPlcBit() - m_broker object is NULL!");
        value = BEP_UNAVAILABLE_RESPONSE;
    }
    return(value);
}

void WorkCellController::ParseSalesCodes(const string &buildRecord)
{   // Extract the sales codes from the build string
    string salesCodeSection = buildRecord.substr(GetSalesCodeStartIndex());
    ExtractSalesCodeData(salesCodeSection);
    // Loop through the vehicle build and attempt to find the required options
    for (XmlNodeMapItr iter = m_buildRecordMap.begin(); iter != m_buildRecordMap.end(); iter++)
    {   // Process the current build record item
        ProcessBuildItem(iter->second);
    }
    // Extract additional items from the build record based on offset into the string
    AddVehicleBuildItem(VIN_DATA_TAG, buildRecord.substr(GetVinIndex(), GetVinLength()), m_vehicleBuild);
}

void WorkCellController::ExtractSalesCodeData(const string &salesCodeSection)
{   // Pull out all the sales code data
    for (UINT16 index = 0; 
        index < salesCodeSection.length();   // Not end of string
        index += (GetSalesCodeFieldLength() + GetDescriptionFieldLength()))  // Increment to the next record
    {
        string currentRecord = salesCodeSection.substr(index, GetSalesCodeFieldLength() + GetDescriptionFieldLength());
        SalesCodeRecord record;
        record.salesCode   = currentRecord.substr(0, currentRecord.find_last_not_of(" ", GetSalesCodeFieldLength()));
        record.description = currentRecord.substr(GetSalesCodeFieldLength(), currentRecord.find_last_not_of(" ", GetDescriptionFieldLength()));
        m_salesCodes.insert(SalesCodeListVal(record.salesCode, record));
        Log(LOG_DEV_DATA, "Added Sales Code: %s - %s", record.salesCode.c_str(), record.description.c_str());
    }
    Log(LOG_DEV_DATA, "Extracted %d sales codes from the build record", m_salesCodes.size());
}

void WorkCellController::ProcessBuildItem(const XmlNode* buildItem)
{
    bool itemFound = false;
    string buildItemName(buildItem->getName());
    string buildValue("Unknown");
    string defaultValue("Unknown");
    // Check all of the options for this build item
    for (XmlNodeMapCItr iter = buildItem->getChildren().begin(); (iter != buildItem->getChildren().end()) && !itemFound; iter++)
    {   // If this is not the default node, get the SalesCode attribute
        if (iter->second->getName().compare("Default"))
        {   // Attempt to find the sales code itme in the build data
            try
            {
                if (m_salesCodes.find(iter->second->getAttribute("SalesCode")->getValue()) != m_salesCodes.end())
                {   // Found the desired sales code, look for any adders or deleters
                    if (iter->second->getChildren().size() > 0)
                    {   // Process the adders
                        XmlNodeMapCItr andIter = iter->second->getChildren().find("And");
                        if (andIter != iter->second->getChildren().end())
                        {   // Need to ensure all of the sales codes in this list are present
                            bool adderFound = true;
                            for (XmlNodeMapCItr addSalesCodeIter = andIter->second->getChildren().begin();
                                (addSalesCodeIter != andIter->second->getChildren().end()) && adderFound;
                                addSalesCodeIter++)
                            {   // Check if the adder item is in the list
                                adderFound = (m_salesCodes.find(addSalesCodeIter->second->getValue()) != m_salesCodes.end());
                            }
                            // If all adders were found, then the item is found, i.e good
                            itemFound = adderFound;
                        }
                        // Process any deleters
                        XmlNodeMapCItr delIter = iter->second->getChildren().find("Less");
                        if (delIter != iter->second->getChildren().end())
                        {   // Need to ensure all items are not present in the build data
                            bool delFound = false;
                            for (XmlNodeMapCItr delSalesCodeIter = delIter->second->getChildren().begin();
                                (delSalesCodeIter != delIter->second->getChildren().end()) && !delFound;
                                delSalesCodeIter++)
                            {
                                delFound = (m_salesCodes.find(delSalesCodeIter->second->getValue()) != m_salesCodes.end());
                            }
                            // If a deleter item was found, then the item has not been found and is not equipped
                            itemFound = !delFound;
                        }
                    } else
                    {   // Found the desired sales code
                        itemFound = true;
                    }
                    // If the item was found, extract the value and determine if it should be converted
                    if (itemFound)
                    {
                        try
                        {
                            buildValue = iter->second->getAttribute("TestValue")->getValue();
                            // Check if the value should be converted
                            if (iter->second->getAttributes().find("ConversionFactor") != iter->second->getAttributes().end())
                            {   // Need to convert the value
                                char buff[32];
                                double value  = atof(buildValue.c_str()) * atof(iter->second->getAttribute("ConversionFactor")->getValue().c_str());
                                buildValue = CreateMessage(buff, sizeof(buff), "%7.2f", value);
                            }
                        } catch (XmlException &excpt)
                        {
                            Log(LOG_ERRORS, "WARNING: Malformed option node for item: %s - %s",
                                buildItem->getName().c_str(), excpt.GetReason());
                        }
                    }
                }
            } catch (XmlException &excpt)
            {   // Malformed option entry
                Log(LOG_ERRORS, "WARNING: missing SalesCode attribute for %s:%s - not processing - %s", 
                    buildItem->getName().c_str(), iter->second->getName().c_str(), excpt.GetReason());
                itemFound = false;
            }
        } else
        {   // Save the default value
            defaultValue = iter->second->getValue();
        }
    }
    // If the item was not found, set the default value
    if (!itemFound)
    {
        buildValue = defaultValue;
    }
    // Create a new XmlNode for the build data
    AddVehicleBuildItem(buildItemName, buildValue, m_vehicleBuild);
}

const std::string WorkCellController::GetWheelBasePosition(const std:: string &bodyStyle)
{
    Log(LOG_FN_ENTRY, "WorkCellController::GetWheelBasePosition()\n");
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
        Log(LOG_DEV_DATA, "Evaluating mask: %s  bodyStyle: %s\n", bodyStyleMask.c_str(), bodyStyle.c_str());
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
            Log(LOG_DEV_DATA, "selectWheelBase1: %s\n", selectWheelBase.c_str());

            // Convert the wheelbase to x10
            try
            {
                double wheelbase = atof(selectWheelBase.c_str());
                Log(LOG_DEV_DATA, "wheelbase (double): %f", wheelbase);
                char buff[33];
                selectWheelBase = itoa((int)(wheelbase*10.0), buff, 10);
                Log(LOG_DEV_DATA, "selectWheelBase2: %s\n", selectWheelBase.c_str());
            }
            catch(...)
            {
                Log(LOG_DEV_DATA, "Error in the wheelbase conversions");
            }
        }
        Log(LOG_DEV_DATA, "Select Weight: %d, Select Wheelbase(x10): %s\n", selectWeight, selectWheelBase.c_str());
    }
    Log(LOG_FN_ENTRY, "WorkCellController::GetWheelBasePosition() done, returning: %s\n", selectWheelBase.c_str());
    return(selectWheelBase);
}

const string WorkCellController::GetAirPressureSetting(const string &selectData)
{
    Log(LOG_FN_ENTRY, "WorkCellController::GetAirPressureSetting() - Enter");
    string selectDataMask, selectAirPressure("0");
    selectAirPressure = GetDataItemFromList(selectData, &m_airPressures);
    Log(LOG_DEV_DATA, "Setting air presure to: %s", selectAirPressure.c_str());
    char buff[8];
    // Convert the airpressure to x10
    float airPressure = atof(selectAirPressure.c_str());
    selectAirPressure = CreateMessage(buff, sizeof(buff), "%d", (int)(airPressure*10));
    Log(LOG_FN_ENTRY, "WorkCellController::GetAirPressureSetting() - Exit");
    return(selectAirPressure);
}

void WorkCellController::LoadDataTags(const XmlNode *setupNode)
{  // Call the base class functionality to load up the tags
    PlantHostInbound::LoadDataTags(setupNode);
    // Now load the WorkCellController specific data tags
    try
    {  // Get the tags to use for data that has to go to the PLC
        XmlNode *dataTags = const_cast<XmlNode *>(setupNode->getChild("DataTags"));
        SetFrontWheelDriveTag(dataTags->getChild("FrontWheelDriveSelectedToPLC")->getValue());
        SetRearWheelDriveTag(dataTags->getChild("RearWheelDriveToPLC")->getValue());
        SetFourWheelDriveTag(dataTags->getChild("FourWDSelected")->getValue());
        SetAirPressureTag(dataTags->getChild("AdjustableAirPressureTag")->getValue());
    } catch (XmlException &excpt)
    {  // Problem getting tags, use defaults
        Log(LOG_ERRORS, "Data Tags not found, setting all to defaults! - %s", excpt.GetReason());
        SetFrontWheelDriveTag("FrontWheelDriveSelectedToPLC");
        SetRearWheelDriveTag("RearWheelDriveToPLC");
        SetFourWheelDriveTag("FourWDSelected");
        SetAirPressureTag("ParkBrakePressure");
    }
}

void WorkCellController::LoadPromptTags(const XmlNode *promptTags)
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
    } catch (XmlException &excpt)
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

void WorkCellController::LoadData(const XmlNode *wccDataNode)
{
    if (UseSectionLengthData())
    {
        try
        {
            SetTimeIndex(wccDataNode->getChild("WccTimeIndex"));
            SetDateIndex(wccDataNode->getChild("WccDateIndex"));
            SetChecksumIndex(wccDataNode->getChild("WccChecksumIndex"));
        } catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Setup data not found, setting all to defaults! - %s", excpt.GetReason());
            SetTimeIndex(179);
            SetDateIndex(184);
            SetTimeLength(4);
            SetDateLength(6);
            SetChecksumIndex(190);
        }
    } else
    {
        try
        {  // Get the field indices and lengths
            SetTimeIndex(BposReadInt(wccDataNode->getChild("WccTimeIndex")->getValue().c_str()));
            SetDateIndex(BposReadInt(wccDataNode->getChild("WccDateIndex")->getValue().c_str()));
            SetTimeLength(BposReadInt(wccDataNode->getChild("WccTimeLength")->getValue().c_str()));
            SetDateLength(BposReadInt(wccDataNode->getChild("WccDateLength")->getValue().c_str()));
            SetChecksumIndex(BposReadInt(wccDataNode->getChild("WccChecksumIndex")->getValue().c_str()));
        } catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Setup data not found, setting all to defaults! - %s", excpt.GetReason());
            SetTimeIndex(179);
            SetDateIndex(184);
            SetTimeLength(4);
            SetDateLength(6);
            SetChecksumIndex(190);
        }
    }
}

const INT32 WorkCellController::CalculateSectionStartIndex(const string &section)
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
        } else
        {   // This is the section we are looking for -- Do NOT add this section length
            sectionFound = true;
        }
    }
    // Check if the section was found
    if (sectionFound)
    {   // The requested section was found
        Log(LOG_DEV_DATA, "Calculated starting index for section: %s - %d", section.c_str(), startIndex);
    } else
    {   // The requested section was not found
        Log(LOG_ERRORS, "CalculateSectionStartIndex - Could not find requested section: %s", section.c_str());
        startIndex = 0;
    }
    // Return the calculated start index
    return(startIndex);
}

INT32 WorkCellController::GetStartIndex(const XmlNode *node)
{   // Determine how to get the start index
    INT32 startIndex = 0;
    if (UseSectionLengthData())
    {
        startIndex = (CalculateSectionStartIndex(node->getAttribute("Section")->getValue()) + 
                      BposReadInt(node->getAttribute("Offset")->getValue().c_str()));
    } else
    {
        startIndex = PlantHostInbound::GetStartIndex(node);
    }
    return(startIndex);
}

bool WorkCellController::IsStartIndexValid(const XmlNode *field, const INT32 &messageLength)
{
    bool indexValid = false;
    if (UseSectionLengthData())
    {   // Get the offset into the section and the section length
        INT32 offset = 0;
        try
        {
            offset = BposReadInt(field->getAttribute("Offset")->getValue().c_str());
        } catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "XmlException getting Offset for %s - %s", field->getName().c_str(), excpt.GetReason());
            offset = 0;
        }
        INT32 fieldLength = 0;
        try
        {
            fieldLength = BposReadInt(field->getAttribute("Length")->getValue().c_str());
        } catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "XmlException getting field Length for %s - %s", field->getName().c_str(), excpt.GetReason());
            fieldLength = 0;
        }
        INT32 sectionLength = GetSectionLength(field->getAttribute("Section")->getValue());
        // Ensure the offset is within the section
        indexValid = ((offset < sectionLength) && ((offset + fieldLength) <= sectionLength));
        Log(LOG_DEV_DATA, "Index valid: %s - SectionLength: %d, Offset: %d, FieldLength: %d",
            (indexValid ? "True" : "False"), sectionLength, offset, fieldLength); 
    } else
    {
        indexValid = PlantHostInbound::IsStartIndexValid(field, messageLength);
    }
    return(indexValid);
}

INT32 WorkCellController::GetSectionLength(const string &section)
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
            } catch (XmlException &excpt)
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

inline const bool& WorkCellController::StartKeepAlive(const bool *startKeepAlive /*= NULL*/)
{
    if (startKeepAlive != NULL)
    {
        m_startKeepAlive = *startKeepAlive;
    }
    return(m_startKeepAlive);
}

inline const bool& WorkCellController::StartRequestHandler(const bool *startRequestHandler /*= NULL*/)
{
    if (startRequestHandler != NULL)
    {
        m_startRequestHandler = *startRequestHandler;
    }
    return(m_startRequestHandler);
}

inline const bool& WorkCellController::StartStaticTesterServerComms(const bool *startComms) /*= NULL*/
{
    if (startComms != NULL)   m_startStaticTesterComm = *startComms;
    return m_startStaticTesterComm;
}

inline const bool& WorkCellController::UseSectionLengthData(const bool *useSectionData) /*= NULL*/
{
    if (useSectionData != NULL)
    {
        m_useSectionData = *useSectionData;
    }
    return(m_useSectionData);
}

inline const bool& WorkCellController::UseSalesCodes(const bool *useSalesCodes /*= NULL*/)
{
    if (useSalesCodes != NULL)
    {
        m_useSalesCodes = *useSalesCodes;
        Log(LOG_DEV_DATA, "Storing flag for use sales code data: %s", *useSalesCodes ? "TRUE" : "FALSE");
    }
    return(m_useSalesCodes);
}

inline void WorkCellController::SetSalesCodeFieldLength(INT32 length)
{
    m_salesCodeLength = length;
}

inline const INT32& WorkCellController::GetSalesCodeFieldLength(void)
{
    return(m_salesCodeLength);
}

inline void WorkCellController::SetDescriptionFieldLength(INT32 length)
{
    m_salesCodeDescriptionLength = length;
}

inline const INT32& WorkCellController::GetDescriptionFieldLength(void)
{
    return(m_salesCodeDescriptionLength);
}

inline void WorkCellController::SetVinIndex(const INT32 index)
{
    m_vinIndex = index;
}

inline void WorkCellController::SetVinLength(const INT32 length)
{
    m_vinLength = length;
}

inline const INT32& WorkCellController::GetVinIndex(void)
{
    return(m_vinIndex);
}

inline const INT32& WorkCellController::GetVinLength(void)
{
    return(m_vinLength);
}

inline void WorkCellController::SetSalesCodeStartIndex(const INT32 &index)
{
    m_salesCodeStartIndex = index;
}

inline const INT32& WorkCellController::GetSalesCodeStartIndex(void)
{
    return(m_salesCodeStartIndex);
}

inline void WorkCellController::SetVINToLoad(const std::string vin)
{
    m_loadVIN = vin;
}

inline const std::string WorkCellController::GetRearWheelDriveTag(void)
{
    return(m_rearWheelDriveTag);
}

inline const std::string WorkCellController::GetFrontWheelDriveTag(void)
{
    return(m_frontWheelDriveTag);
}

inline const std::string WorkCellController::GetFourWheelDriveTag(void)
{
    return(m_fourWheelDriveTag);
}

inline const std::string WorkCellController::GetAutomaticTransmissionTag(void)
{
    return(m_autoTransTag);
}

inline const std::string WorkCellController::GetManualTransmissionTag(void)
{
    return(m_manualTransTag);
}

inline const string& WorkCellController::GetAirPressureTag(void)
{
    return(m_airPressureTag);
}

inline const std::string WorkCellController::GetNoInfoPrompt(void)
{
    return(m_noInfoPrompt);
}

inline const std::string WorkCellController::GetBadChecksumPrompt(void)
{
    return(m_badCheckSumPrompt);
}

inline const std::string WorkCellController::GetUnknownMessagePrompt(void)
{
    return(m_unknownMessagePrompt);
}

inline const std::string WorkCellController::GetNAKPrompt(void)
{
    return(m_wccNAKPrompt);
}

inline const std::string WorkCellController::GetNoResponsePrompt(void)
{
    return(m_wccNoResponsePrompt);
}

inline const INT32 WorkCellController::GetTimeIndex(void)
{   // Determine where the index is stored
    INT32 timeIndex = 0;
    if (UseSectionLengthData())
    {
        timeIndex = (CalculateSectionStartIndex(m_wccTimeInfo->getAttribute("Section")->getValue()) + 
                     BposReadInt(m_wccTimeInfo->getAttribute("Offset")->getValue().c_str()));
    } else
    {
        timeIndex = m_wccTimeIndex;
    }
    return(timeIndex);
}

inline const INT32 WorkCellController::GetTimeLength(void)
{
    INT32 timeLength = 0;
    if (UseSectionLengthData())
    {
        timeLength = BposReadInt(m_wccTimeInfo->getAttribute("Length")->getValue().c_str());
    } else
    {
        timeLength = m_wccTimeLength;
    }
    return(timeLength);
}

const INT32 WorkCellController::GetDateIndex(void)
{   // Determine how to get the date index
    INT32 dateIndex = 0;
    if (UseSectionLengthData())
    {
        dateIndex = (CalculateSectionStartIndex(m_wccDateInfo->getAttribute("Section")->getValue()) + 
                     BposReadInt(m_wccDateInfo->getAttribute("Offset")->getValue().c_str()));
    } else
    {
        dateIndex = m_wccDateIndex;
    }
    return(dateIndex);
}

inline const INT32 WorkCellController::GetDateLength(void)
{   // Return the date field length
    INT32 dateLength = 0;
    if (UseSectionLengthData())
    {
        dateLength = BposReadInt(m_wccDateInfo->getAttribute("Length")->getValue().c_str());
    } else
    {
        dateLength = m_wccDateLength;
    }
    return(dateLength);
}

const INT16 WorkCellController::GetChecksumIndex(void)
{   // Determine how to get the index for the checksum
    INT16 checkSumIndex = 0;
    if (UseSectionLengthData())
    {
        Log(LOG_DEV_DATA, "WorkCellController::GetChecksumIndex() using sections\n");

        checkSumIndex = (CalculateSectionStartIndex(m_wccChecksumInfo->getAttribute("Section")->getValue()) + 
                         BposReadInt(m_wccChecksumInfo->getAttribute("Offset")->getValue().c_str()));
    } else
    {
        checkSumIndex = m_wccChecksumIndex;
    }
    return(checkSumIndex);
}

inline const INT32 WorkCellController::GetMaxAttempts(void)
{
    return(m_maxAttempts);
}

inline const INT32 WorkCellController::GetMaxResponseTime(void)
{
    return(m_maxResponseTime);
}

const UINT32 WorkCellController::GetValidMessageLength(void)
{   // Check where to get the valid message length data from
    UINT32 messageLength = 0;
    if (UseSectionLengthData())
    {   // Loop through the section data and tally the section lengths
        Log(LOG_DEV_DATA, "Using Section Length\n");
        for (XmlNodeMapItr iter = m_wccSectionMap.begin(); iter != m_wccSectionMap.end(); iter++)
        {   // Get the section length from the current node and add to the tally
            messageLength += BposReadInt(iter->second->getAttribute("SectionLength")->getValue().c_str());
        }
    } else
    {
        Log(LOG_DEV_DATA, "Not Using Section Length\n");
        messageLength = m_validMessageLength;
    }
    Log(LOG_DEV_DATA, "Calculated minimum message length: %d", messageLength);
    return(messageLength);
}

inline const UINT32 WorkCellController::GetValidWccResponseLength(void)
{   // Check where to get the valid message length data from
    return(m_validWccResponseLength);
}

void WorkCellController::StoreSectionLengths(const string &buildMessage)
{   // Loop through the section map and store the length data
    try
    {
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

                    // Update the node with the correct data
                    XmlNode *sectionLengthNode = const_cast<XmlNode *>(iter->second->getAttribute("SectionLength"));
                    char buff[10];
                    sectionLengthNode->setValue(itoa(length, buff, 10));
                    Log(LOG_DEV_DATA, "Updated section data node: %s", iter->second->ToString().c_str());
                } else
                {   // Could not find the section length node
                    Log(LOG_ERRORS, "Could not find section length data node for %s", sectionLengthTag.c_str());
                }
            } else
            {
                Log(LOG_ERRORS, "section length tag empty for: %s", sectionLengthTag.c_str());
            }
            Log(LOG_DEV_DATA, "Done storing section length infor for: %s", sectionLengthTag.c_str());
        }
    } catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "Exception caught");
    }
}

inline const bool WorkCellController::AreRetainersDown(void)
{
    return(Read(m_retainersDownTag) == retainingRollsDown ? true : false);
}

inline const XmlNode* WorkCellController::GetBroadcastCommSetup(void)
{
    return(m_wccCommSetup);
}

inline const XmlNode* WorkCellController::GetRequestHandlerConfig(void)
{
    return(m_requestHandlerConfig);
}

inline const std::string WorkCellController::GetVINToLoad(void)
{
    return(m_loadVIN);
}

inline void WorkCellController::SetBodyStyleDriveAxle(const XmlNode *bodyStyleDriveAxle)
{   // Make sure the node is not NULL
    if (bodyStyleDriveAxle != NULL) m_bodyStyleDriveAxle.DeepCopy(bodyStyleDriveAxle->getChildren());
    else m_bodyStyleDriveAxle.clear(true);
}

inline void WorkCellController::StoreTireSizeAirPressures(const XmlNode *airPressures)
{   // Make sure the node is not NULL
    if (airPressures != NULL)  m_airPressures.DeepCopy(airPressures->getChildren());
    else  m_airPressures.clear(true);
}

inline void WorkCellController::SetMinimumAirPressure(const float &minPressure)
{
    m_minimumAirPressure = minPressure;
}

inline void WorkCellController::SetMaximumAirPressure(const float &maxPressure)
{
    m_maximumAirPressure = maxPressure;
}

inline void WorkCellController::SetRearWheelDriveTag(const std::string &tag)
{
    m_rearWheelDriveTag = tag;
}

inline void WorkCellController::SetFrontWheelDriveTag(const std::string &tag)
{
    m_frontWheelDriveTag = tag;
}

inline void WorkCellController::SetFourWheelDriveTag(const std::string &tag)
{
    m_fourWheelDriveTag = tag;
}

inline void WorkCellController::SetAutomaticTransmissionTag(const std::string &tag)
{
    m_autoTransTag = tag;
}

inline void WorkCellController::SetManualTransmissionTag(const std::string &tag)
{
    m_manualTransTag = tag;
}

inline void WorkCellController::SetAirPressureTag(const string &tag)
{
    m_airPressureTag = tag;
}

inline void WorkCellController::SetNoInfoPrompt(const std::string &prompt)
{
    m_noInfoPrompt = prompt;
}

inline void WorkCellController::SetBadChecksumPrompt(const std::string &prompt)
{
    m_badCheckSumPrompt = prompt;
}

inline void WorkCellController::SetUnknownMessagePrompt(const std::string &prompt)
{
    m_unknownMessagePrompt = prompt; 
}

inline void WorkCellController::SetTestBlockPrompt(const std::string &prompt)
{
    m_testBlockPrompt = prompt;
}

inline void WorkCellController::SetAbortPrompt(const std::string &prompt)
{
    m_abortStatusPrompt = prompt;
}

inline void WorkCellController::SetShipStatusPrompt(const std::string &prompt)
{
    m_shipStatusPrompt = prompt;
}

inline void WorkCellController::SetAbortColor(const string &color)
{
    m_abortStatusColor = color;
}

inline void WorkCellController::SetShipStatusColor(const string &color)
{
    m_shipStatusColor = color;
}

inline void WorkCellController::SetNAKPrompt(const std::string &prompt)
{
    m_wccNAKPrompt = prompt;
}

inline void WorkCellController::SetNoResponsePrompt(const std::string &prompt)
{
    m_wccNoResponsePrompt = prompt;
}

inline void WorkCellController::SetTimeIndex(const INT32 &index)
{
    m_wccTimeIndex = index;
}

inline void WorkCellController::SetTimeIndex(const XmlNode *timeIndex)
{   // First delete any information so we do not create a memory leak
    if (m_wccTimeInfo != NULL)  delete m_wccTimeInfo;
    m_wccTimeInfo = NULL;
    // Save the time location data
    m_wccTimeInfo = timeIndex->Copy();
}

inline void WorkCellController::SetTimeLength(const INT32 &length)
{
    m_wccTimeLength = length;
}

inline void WorkCellController::SetDateIndex(const INT32 &index)
{
    m_wccDateIndex = index;
}

inline void WorkCellController::SetDateIndex(const XmlNode* dateIndex)
{   // First delete any information so we do not create a memory leak
    if (m_wccDateInfo != NULL)  delete m_wccDateInfo;
    m_wccDateInfo = NULL;
    // Now save the date data
    m_wccDateInfo = dateIndex->Copy();
}

inline void WorkCellController::SetDateLength(const INT32 &length)
{
    m_wccDateLength = length;
}

inline void WorkCellController::SetChecksumIndex(const INT16 &index)
{
    m_wccChecksumIndex = index;
}

inline void WorkCellController::SetChecksumIndex(const XmlNode *checksumInfo)
{   // First delete any information so we do not create a memory leak
    if (m_wccChecksumInfo != NULL)  delete m_wccChecksumInfo;
    m_wccChecksumInfo = NULL;
    // Now save the location information
    m_wccChecksumInfo = checksumInfo->Copy();
}

inline void WorkCellController::SetMaxAttempts(const INT32 &attempts)
{
    m_maxAttempts = attempts;
}

inline void WorkCellController::SetMaxResponseTime(const INT32 &maxTime)
{   // Software uses a 10ms sleep time between checks.
    m_maxResponseTime = maxTime/10;
}

inline void WorkCellController::SetValidMessageLength(const UINT32 &length)
{
    m_validMessageLength = length;
}

inline void WorkCellController::SetValidWccResponseLength(const UINT32 &length)
{
    m_validWccResponseLength = length;
}

inline void WorkCellController::ClearMaps(void)
{
    m_bodyStyleDriveAxle.clear(true);
    PlantHostInbound::ClearMaps();
}

inline void WorkCellController::SetBroadcastCommSetup(const XmlNode *setup)
{
    if (setup != NULL) m_wccCommSetup = setup->Copy();
    else m_wccCommSetup = NULL;
}

inline void WorkCellController::SetRequestHandlerConfig(const XmlNode *config)
{
    if (config != NULL) m_requestHandlerConfig = config->Copy();
    else m_requestHandlerConfig = NULL;
}

inline void WorkCellController::SetKeepAliveConfig(const XmlNode *config)
{
    if (config != NULL) m_keepAliveConfig = config->Copy();
    else m_keepAliveConfig = NULL;
}

inline const XmlNode* WorkCellController::GetKeepAliveConfig(void)
{
    return(m_keepAliveConfig);
}

inline void WorkCellController::SetPortLockedStatus(const bool portStatus)
{
    m_portLockedStatus = portStatus;
}

inline const bool WorkCellController::GetPortLockedStatus(void)
{
    return(m_portLockedStatus);
}
