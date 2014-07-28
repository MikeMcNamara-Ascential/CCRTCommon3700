//*************************************************************************
// Description:
//   Test Result Server to handle all test results
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/TestResultServer/TestResultServer.cpp $
// 
// 1     11/01/07 3:04p Derickso
// Updated WRite function to remove race condition that was causing the
// server to crash periodically.
// Updated SetDTC to handle common module faults.
// 
// 17    10/26/06 1:20p Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// Added a flag to tell if the test results should be cleared when the
// test is finished.
// 
// Added conditionally setting the fault and accept PLC lamps.
// 
// 16    9/22/06 11:15a Rwiersem
// Changes for the 06092201 core release:
//     - Added OpenAndReadInResultsFile() for unit testing
//     - Set the KeyPress tag name based on the configuration file
// 
// 15    4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 2     4/26/06 7:39a Gswope
// c.267, 344, 398 installed and used atob() for more lenient configuration
//
// A) Changes in Initialize() to fix the cleanup of old test results
//   c.416 increased the size of the read buffer, to allow more variablility
//   c.438-444 changed method of reading record from file and "cleaning it up"
//   c.448 changed function call to send all valid characters.
//   c.459 added call to write file after modifying it, in case we crash
//         (note that this was already being done if a real test result record
//          was being added, but it was not being done during initialization.)
//   added/modified several Log() calls
//
// B) Changes in AddTestResult() to fix the cleanup of old test results
//   c.564 changed method of determining where sequence number starts in the
//         filenames read from the index.
//   c.570 commented out drug-induced logic.
//   c.607 changed method of removing file from BposSystemCommand() to unlink()
//         for speed, standardization, and clarity. Note that this also
//         significantly simplified the surrounding code as it properly
//         handles spaces and other special characters in the filename,
//         removing several lines of string manipulation.
//   added/modified several Log() calls
//
// 1     4/11/06 2:08p Rwiersem
// Added LoadAdditionalConfigurationItems() and HandlePulse() methods.
// These are used to reload the configuration without restarting the
// process.
//
// 14    4/13/06 4:29a Cward
// Added code  to allow the ICM speed and force arrays to be saved to the
// test result file as CDATA nodes
//
// 2     3/20/06 11:01a Bmeinke
// Added code  to allow the ICM speed and force arrays to be saved to the
// test result file as CDATA nodes
//
// 13    2/15/06 8:06p Cward
// Cleaned up statistic logic error in calculating the total number of
// tests.
//
// 12    11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 11    5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 10     5/09/05 5:00p Gpattison
// Re-arranged the Initialize() method to call the base class initialize
// as soon as possible
//
// 10    3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 9     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     3/02/05 1:38p Gswope
// modified to conditionally clear build data, when OverallTest is
// published
//
// 8     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 7     5/12/04 9:53p Cward
// Changes for the May 12 iteration
//
// 6     4/28/04 10:54p Cward
// Changes associated with build 040402801
//
// 5     3/04/04 5:28p Cward
// Added all failures Node in Publish methhod.
//
// 4     2/26/04 6:35p Cward
// Changes made for the Feb 26, 2004 iteration
//
// 3     1/28/04 3:49p Cward
// Placed m data Lock and Unlock around all uses of m data
//
// 2     1/27/04 4:49p Cward
// Changes resulting from DE's testing and RW's trip
//
// 1     12/21/03 6:29p Cward
//
// 3     10/24/03 1:51p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 3     10/10/03 3:46p Khykin
// Updated with Brians latest changes.
//
// 2     10/10/03 10:22a Khykin
// Updated to use TestInProgress as the trigger for starting and binding a
// testresult file.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/07/03 3:14p Khykin
// Updated with changes from Flatrock.
//
// 1     6/12/03 3:58p Khykin
// Initial checkin.
//
// 1     4/22/03 9:51a Khykin
// Kevins Working Checkin
//
// 3     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:37  tbochene
//
//    PooBag
//
//
// 2     2/08/03 10:00a Khykin
// Updated for the publishing of the TestResult tag.
//
// 1     1/27/03 7:18a Khykin
// Initial Checkin
//
// 1     1/18/03 8:13a Khykin
// Initial Checkin From DCTC
//
// 40    11/26/02 5:08p Derickso
// Clear vin value after test results have been reported.
//
// 39    10/24/02 12:54p Derickso
// Updated to remove test results from allFailures if they have
// passed on a retest.  The allFailures node will also be published
// whenever a node is added or removed.
//
// 38    10/23/02 12:38p Derickso
// Removed seperate FTP support.  Now localizing on FTP_ROOT.
//
// 37    10/17/02 1:49p Derickso
// Clear test results on loss compensation cycle start.
//
// 36    10/16/02 10:25a Derickso
// Changed Loss compensation tag to use macro instead of config file.
//
// 35    10/15/02 3:10p Derickso
// Added try catch block in ReportREsults to correct for SigSegs
//
// 34    9/26/02 2:02p Tbochene
// Removed compiler warnings
//
// 33    8/23/02 10:44a Dan.erickson
// Added DTC reporting capabilities.  Added methods to allow updating of
// vehicle build information.
//
// 32    8/05/02 1:15p Dan.erickson
// Corrected Notify calls.
//
// 31    7/29/02 8:19a Dan.erickson
// Added support for LossCompensation results.
//
// 30    6/07/02 11:59a Dan.erickson
// Updated with new style of getChild method.
//
// 29    5/20/02 4:42p Dan.erickson
// Added publishing of AllFailures tag with a value of
// ClearScrollBox at the beginning of each test.
// Added logic to properly clean up XmlNode and XmlNodeMap variables.
//
// 28    4/25/02 4:46p Skypig13
// Updated with new changes
//
// 26    4/16/02 9:01a Dan.erickson
// Modified ReportResults() to not include the StartTest and
// VehicleBuild tags in the test result file.
//
// 25    4/15/02 2:52p Dan.erickson
// Updated CheckTestResult method.  No longer need to create
// an ITestResult Comm object in order to notify properly.
//
// 24    3/26/02 7:09a Dan.erickson
// In AddTestResult method, modified format for building the
// test result filename.  Removed the hardcoded sequence length.
//
// 23    3/25/02 10:37a Dan.erickson
// Need to pass com object to Notify method so the attributes
// are added to the node.
//
// 22    3/22/02 1:29p Dan.erickson
// Removed warning.
//
// 21    3/20/02 11:01a Dan.erickson
// Changed m_allFailures from XmlNode* to XmlNode.
// Restored GetSpecialInfo() method so attributes will be populated
// on nodes when they are returned.
// Changed OverallPassFail to OverallTest.
//
// 20    3/05/02 5:07p Dan.erickson
// Added support for an AllFailures tag.  This tag will keep track of all
// current test failures and report to subscribers.
//
// 19    2/18/02 3:36p Skypig13
// Updated for new BepServer
//
// 18    2/12/02 5:18p Dan.erickson
// Modified to get the value of the node for DataTags.
//
// 17    2/07/02 10:17a Admin
// Updating for system testing.
//
// 16    1/16/02 4:22p Dan.erickson
// Modified Initialize method to get the name of the node instead
// of the value.
//
// 15    1/15/02 5:04p Dan.erickson
// Modified ReportResults method to return the file name where
// the test result was stored.
//
// 14    1/11/02 4:48p Dan.erickson
// Changed StartTest tag to StartTestFromPLC.
// Added handling for OverallPassFail tag being published to us.
//
// 13    1/04/02 11:24a Dan.erickson
// Modified Initialize method to reflect new location of
// configuration information.
// Streamlined SpecialProcessing method.
//
// 12    12/12/01 9:01a Dan.erickson
// Modified to use accessor methods for member variables where
// applicable.
// Modified to subscribe for VehicleBuild and include vehicle
// build in the test result file.
// Removed subscription for VIN and handling of vin. Getting
// vin from vehicle build now.
// Modified SpecialProcessing method to handle write messages.
//
// 11    12/03/01 2:44p Dan.erickson
// Replaced hard coded name with macro.
//
// 10    11/15/01 2:17p Dan.erickson
// Debugged UpdatedResultFile method.
//
// 9     11/15/01 9:03a Dan.erickson
// Use macro BEP_TEST_RESULT instead of hard coded value.
// Streamlined SpecialProcessing method.
//
// 8     11/13/01 7:53a Dan.erickson
// Defaulted name of server to TestResultServer.
// Debugged Publis and ReportResults methods.
//
// 7     11/08/01 12:11p Dan.erickson
// Added Terminate method.
// Corrected file handling operations.
//
// 5     11/07/01 4:58p Dan.erickson
// Testing.
//
// 4     11/07/01 10:20a Dan.erickson
// Change inheritance from IInfoCommunication to ITestResultServer.
// Removed processing for Read messages from SpecialProcessing method.
// Added GetName method.
//
// 3     11/05/01 4:27p Dan.erickson
// Testing
//
// 2     11/01/01 2:56p Dan.erickson
// Compiles, not tested.
//
// 1     10/24/01 1:22p Dan.erickson
// Created.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================

#include <fstream.h>
#include <fcntl.h>      // O_RDONLY
#include <dirent.h>
#include <time.h>
#include <algorithm>
#include "INamedDataBroker.h"
#include "TestResultServer.h"
#include "ITestResultThreadPool.h"
#include "GeneralUtilities.h"

//-------------------------------------------------------------------------------------------------
TestResultServer::TestResultServer() : BepServer(), m_testResultFile(""),
m_numberOfTests(0), m_sequenceNumber(0), m_semVehicleBuild(1), m_vehicleBuild(NULL),
    m_allFailures("temp",""), m_reportedDTC(DTC_TAG,""), m_testInProgress(false),
    m_forceArrayInResults( false), m_forceArrayFile(""), m_speedArrayInResults( false),
    m_speedArrayFile(""), m_updatePlcLamps(false),m_clearResultsOnOverall(true),m_inCycleTestNumber(0),
    m_interCcrtServerComm(NULL)
{
}

//-------------------------------------------------------------------------------------------------
TestResultServer::~TestResultServer()
{
    // Disconnect from the QNX data server
    if(ConnectToQnxDataServer())  m_qnxDataServer.DisconnectFromQnxDataServer();
    Log(LOG_FN_ENTRY, "TestResultServer::~TestResultServer() done");
}

//-------------------------------------------------------------------------------------------------
void TestResultServer::Initialize(const std::string &fileName)
{
    BepServer::Initialize(fileName);
}

//-------------------------------------------------------------------------------------------------
void TestResultServer::Initialize(const XmlNode *document)
{
    // Initialize the base server
    m_comm = new ITestResultServerThreadPool(this);
    BepServer::Initialize(document);

    Log(LOG_FN_ENTRY, "TestResultServer::Initialize(%s)", document->ToString().c_str());

    // Load the additional configuration items needed by the test result server
    LoadAdditionalConfigurationItems(document);

    // Set the vin being used for Loss Compensation
    SetLossCompensationVin(LOSS_COMPENSATION_VIN);
    try
    {
        // Reset test results so we start with a clean slate
        ResetTestResults();

        // Open the Result file and read in the previous results
        if (m_numberOfTests > 0)
        {
            OpenAndReadInResultsFile();
            // Clean up the directory
            CleanupResultDirectory();
        }

        Log(LOG_FN_ENTRY, "TestResultServer::Initialize(%s) done", document->ToString().c_str());
        Log(LOG_FN_ENTRY, "TestResultServer::Initialize(Configuration) done");
    }
    catch (BepException &e)
    {
        Log(LOG_ERRORS, "TestResultServer Exception: %s\n", e.what());
    }
    catch(std::exception &e)
    {
        Log(LOG_ERRORS, "TestResultServer Exception (std) %s.\n", e.what());
    }

    if(m_data.Lock() == EOK)
    {
        try
        {   // Add the all failures node to m_data so subscribers do not get Unavailable
            m_allFailures.clear();
            m_allFailures.setName(GetAllFailuresTag());
            m_allFailures.setValue("");
            m_data.addNode(m_allFailures.Copy());
            // Start with no vin data
            SetCurrentVin(noVIN);
        }
        catch (BepException &e)
        {
            Log(LOG_ERRORS, "TestResultServer Exception [part ii]: %s\n", e.what());
        }
        catch(std::exception &e)
        {
            Log(LOG_ERRORS, "TestResultServer Exception [part ii] (std) %s.\n", e.what());
        }
        m_data.Unlock();
    }
    else
    {
        // unable to lock mutex
        Log(LOG_ERRORS, "Could not lock mutex in TestResultServer::Initialize()!\n");
    }
    bool connect = false;
    try
    {
        Log(LOG_FN_ENTRY, "Checkpoint 1");
        connect = atob(document->getChild("Setup/ConnectToQnxDataServer")->getValue().c_str());
        Log(LOG_FN_ENTRY, "Checkpoint 2a");
        Log(LOG_DEV_DATA, "Connect to qnx data server: %s", (connect)?"yes":"no");
    }
    catch(XmlException &excpt)
    {
        Log(LOG_FN_ENTRY, "Checkpoint 2b");
        Log(LOG_ERRORS, "Connect to qnx data server not specified, not connecting: %s", excpt.GetReason());
        connect = false;
    }
    Log(LOG_FN_ENTRY, "Checkpoint 3");
    ConnectToQnxDataServer(&connect);
	// Determine if a pass confirmation file should be generated
	bool createPassConfirmation = false;
	try
	{
		createPassConfirmation = atob(document->getChild("Setup/CreatePassConfirmationFile")->getValue().c_str());
		Log(LOG_DEV_DATA, "Create pass confirmation file: %s", (createPassConfirmation)?"yes":"no");
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Create pass confirmation file not configured, not generating pass confirmation files: %s",
			excpt.GetReason());
		createPassConfirmation = false;
	}
	GeneratePassConfirmationFile(&createPassConfirmation);
	if(GeneratePassConfirmationFile())
	{   // Need to store the location to place the file
		string path;
		try
		{
			path = document->getChild("Setup/TestResults/PassConfirmationFilePath")->getValue();
            Log(LOG_DEV_DATA, "Pass confirmation file path: %s", path.c_str());
		}
		catch(XmlException &excpt)
		{
			path = string(getenv("USR_ROOT")) + string("/TestResults/PassConfirmation/");
			Log(LOG_ERRORS, "Pass confirmation file path not specified, using %s - %s", path.c_str(), excpt.GetReason());
		}
		PassConfirmationFilePath(&path);
	}

	// Determine if results should be imported from InterCcrtCommServer
	bool importInterCcrtCommServerResults = false;
	try
	{
		importInterCcrtCommServerResults = atob(document->getChild("Setup/ImportCcrtCommServerResults")->getValue().c_str());
		Log(LOG_DEV_DATA, "Import Inter Ccrt Comm Server Results: %s", (importInterCcrtCommServerResults)?"yes":"no");
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Import Inter Ccrt Comm Server Results not configured: %s",
			excpt.GetReason());
		importInterCcrtCommServerResults = false;
	}
	ImportInterCcrtResults(&importInterCcrtCommServerResults);

}

//-----------------------------------------------------------------------------
const string TestResultServer::Register(void)
{   // Call the base class to begin the registration
    string result = BepServer::Register();
    // Determine if the data server should be connected.
    if(ConnectToQnxDataServer())
    {
        Log(LOG_DEV_DATA, "Connected to QNX Data Server: %s", 
            ConvertStatusToResponse(m_qnxDataServer.ConnectToQnxDataServer()).c_str());
    }

    if(ImportInterCcrtResults())
    {
        try
        {
            // Create a new comm object to the static tester server
            Log(LOG_DEV_DATA, "Creating comm object for Inter Ccrt Comm Server");
            if (m_interCcrtServerComm == NULL)
            {
                Log(LOG_DEV_DATA, "Creating comm object for core/InterCcrtCommServer...");
                m_interCcrtServerComm = new IBepCommunication();
                m_interCcrtServerComm->Initialize("core/InterCcrtCommServer", "Client", IsDebugOn(), 5000);
            }
        }
        catch (BepException &e)
        {
            Log(LOG_ERRORS, "Error creating inter ccrt server comm object %s: %s\n", GetProcessName().c_str(), e.what());
        }
    }
    return result;
}

//-----------------------------------------------------------------------------
const bool TestResultServer::OpenAndReadInResultsFile()
{
    FILE *infile;
    string vin;
    string path;
    bool operationSuccess = true;
    bool addResult = false;
    char tempVin[defaultVINLength+sequenceLength+2];
    // Make sure tempVin array is clear and ready to use
    memset(tempVin, 0, sizeof(tempVin));
    // Make sure the results file exists before we try to open it and read from it
    string filename(m_testResultPath + m_testResultFile);
    if (!access(filename.c_str(), F_OK))
    {
        if ((infile = fopen(filename.c_str(), "r")) != NULL)
        {   // Read the records from the file and add to the list
            rewind(infile);
            while (fgets(tempVin, defaultVINLength+sequenceLength+5, infile) != NULL)
            {   // Get the newline character and any straglers
                Log(LOG_DEV_DATA, "Result file name: %s", tempVin);
                char *fn = strtok(tempVin, "\n");
                vin = (fn != NULL) ? string(fn) : "";
                Log(LOG_DEV_DATA, "Adding %s to the test result list", vin.c_str());
                // Add the data to the list
                addResult = AddTestResult(vin, true);
                if(addResult)
                {
                    Log(LOG_DEV_DATA, "Added %s to the test result list", vin.c_str());
                }
                else
                {
                    Log(LOG_DEV_DATA, "Did not add %s to the test result list", vin.c_str());
                }
                // Clear the buffer for the next vin
                memset(tempVin, 0, sizeof(tempVin));
            }
            // Close the file since we have read it in
            fclose(infile);
        }
        else
        {
            Log(LOG_ERRORS, "Could not open: %s\n", (m_testResultPath + m_testResultFile).c_str());
            operationSuccess = false;
        }
    }
    else
    {
        Log(LOG_ERRORS, "Could not find: %s\n", filename.c_str());
        operationSuccess = false;
    }

    return operationSuccess;
}

//-------------------------------------------------------------------------------------------------
void TestResultServer::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    // Make sure the communication channel is set correctly
    std::string srvr(TEST_RESULT_SERVER_NAME);
    XmlNode *name = const_cast<XmlNode *>(document->getChild("Setup/Communication/Name"));
    if (name->getValue() != srvr)
    {   // Set the name properly
        Log(LOG_DEV_DATA, "Attempt to use non-standard name: %s, using: %s instead\n", name->getValue().c_str(), srvr.c_str());
        name->setValue(srvr);
    }
    try
    {   // Get the test result file name from the document
        m_testResultPath = getenv("FTP_ROOT") + document->getChild("Setup/TestResults/TestResultPath")->getValue();
        m_testResultPublshedPath = document->getChild("Setup/TestResults/TestResultPath")->getValue();
        m_testResultFile = document->getChild("Setup/TestResults/TestResultFile")->getValue();
        m_numberOfTests = abs(atoi(document->getChild("Setup/TestResults/NumberOfTests")->getValue().c_str()));
    }
    catch (...)
    {
        Log(LOG_ERRORS, "Warning: TestResultFile node not found.  Test results will not be saved to file\n");
        m_testResultFile.erase();
        m_testResultPath.erase();
        m_numberOfTests = 0;
    }
    try
    {
        // modified to use atob() 4/19/5 - ews
        m_clearBuildOnOverall = atob(document->getChild(
              "Setup/TestResults/ClearBuildOnOverall")->getValue().c_str());
    }
    catch (...)
    {
        m_clearBuildOnOverall = true; // for backward compatibility

    }
    try
    {   // Get the list of tags we will need to subscribe for
        XmlNode *node = const_cast<XmlNode *>(document->getChild("Setup/DataTags"));
        SetVinTag(node->getChild("Vin")->getValue());
        SetStartTestTag(node->getChild("StartTest")->getValue());
        SetAllFailuresTag(node->getChild("AllFailures")->getValue());
        SetOverallTestTag(node->getChild("OverallTest")->getValue());
        SetVehicleBuildTag(node->getChild("VehicleBuild")->getValue());
        SetClearScrollBoxTag(node->getChild("ClearScrollBox")->getValue());
        SetDriveCurveTag(node->getChild("DriveCurve")->getValue());
        SetDriverNumberTag(node->getChild("DriverNumber")->getValue());
        try
        {
            SetKeyPressTag(node->getChild("KeyPress")->getValue());
        }
        catch (XmlException &e)
        {
            //setting to a backwards compatible standard if this node doesn't exist
            SetKeyPressTag(PENDANT_KEY_PRESS);
        }
        try
        {
            SetErrorCodeTestTag(node->getChild("ErrorCodeTest")->getValue());
            SetErrorTag(node->getChild("Error")->getValue());
        }
        catch (...)
        {
            SetErrorCodeTestTag("ErrorCodeTest");
            SetErrorTag("Error");
        }
    }
    catch (...)
    {   // Default the names of the tags
        SetVinTag(VIN_TAG);
        SetStartTestTag(START_VEHICLE_TEST_DATA_TAG);
        SetAllFailuresTag("AllFailures");
        SetOverallTestTag("OverallTest");
        SetVehicleBuildTag(VEHICLE_BUILD_TAG);
        SetClearScrollBoxTag("ClearScrollBox");
        SetDriveCurveTag("DriveCurve");
        SetDriverNumberTag(DRIVER_NUMBER_DATA_TAG);
        SetKeyPressTag(PENDANT_KEY_PRESS);
    }
    try
    {   // Get the items that need to be added to the vehicle build list
        m_vehiclebuildItems.DeepCopy(document->getChild("Setup/VehicleBuildItems")->getChildren());
    }
    catch (XmlException &e)
    {
        Log(LOG_ERRORS, "Error getting vehicle build items - %s\n", e.GetReason());
        m_vehiclebuildItems.clear(true);
    }
    try
    {   // Get the length of the VIN string
        SetVINLength(BposReadInt(document->getChild("Setup/VINLength")->getValue().c_str()));
    }
    catch(XmlException &e)
    {   // No VIN Length specified
        Log(LOG_ERRORS, "Error getting VINLength, using 17 - %s\n", e.GetReason());
        SetVINLength(defaultVINLength);
    }
    // Determine if test results of 'Abort' should be included in the AllFailures node
    try
    {   // Set the flag that indicates if we are suppose add test results of 'Abort' to
        //         the AllFailures node
        // modified to use atob() 4/19/5 - ews
        m_includeAbortInAllFailures = atob(document->getChild(
              "Setup/IncludeAbortInAllFailures")->getValue().c_str());
    }
    catch (...)
    {
        Log(LOG_ERRORS, "Exception loading value for IncludeAbortInAllFailures, setting to true\n");
        m_includeAbortInAllFailures = true;
    }

    try
    {
        const XmlNode *forceNode = document->getChild("Setup/AddForceArray");
        m_forceArrayInResults = (!strnicmp(forceNode->getValue().c_str(), "Yes", 3)) ? true : false;
        if( true == m_forceArrayInResults)
        {
            try
            {
                m_forceArrayFile = getenv("FTP_ROOT") + document->getChild("Setup/ForceArrayPath")->getValue();
            }
            catch( BepException &err)
            {
                m_forceArrayFile = getenv("FTP_ROOT") + std::string("/TestResults/Machine/BrakeForceData/BrakeForce.log");
            }
        }
    }
    catch( BepException &err)
    {
        m_forceArrayInResults = false;
    }

    try
    {
        const XmlNode *speedNode = document->getChild("Setup/AddSpeedArray");
        m_speedArrayInResults = (!strnicmp(speedNode->getValue().c_str(), "Yes", 3)) ? true : false;
        if( true == m_forceArrayInResults)
        {
            try
            {
                m_speedArrayFile = getenv("FTP_ROOT") + document->getChild("Setup/SpeedArrayPath")->getValue();
            }
            catch( BepException &err)
            {
                m_speedArrayFile = getenv("FTP_ROOT") + std::string("/TestResults/Machine/BrakeForceData/Speed.log");
            }
        }
    }
    catch( BepException &err)
    {
        m_speedArrayInResults = false;
    }

    try
    {
        const XmlNode *lampNode = document->getChild("Setup/UpdateTestLampStatus");
        m_updatePlcLamps = atob(lampNode->getValue().c_str());
    }
    catch( BepException &err)
    {
        m_updatePlcLamps = false;
    }
    
    // Determine if the failure alarm is enabled
    bool alarm = false;
    try
    {
        alarm = atob(document->getChild("Setup/FaultAlarm/EnableFaultAlarm")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Fault alarm option not specified, not enabling use of fault alarm - %s", excpt.GetReason());
        alarm = false;
    }
    FailAlarmEnabled(&alarm);
    // If the test fail alarm is being used, setup the timer so the alarm can be disabled
    if(FailAlarmEnabled())
    {
        SetupFaultAlarmTimer(document->getChild("Setup/FaultAlarm"));
    }

    // Determine if the PLC should be signaled at the end of a test sequence
    bool sig = false;
    try
    {
        sig = atob(document->getChild("Setup/TestCompleteSignal/SignalTestComplete")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Test complete signaling not enabled - %s", excpt.GetReason());
        sig = false;
    }
    ShouldSignalTestComplete(&sig);
    // If test complete signaling is enabled, setup the timer
    if(ShouldSignalTestComplete())
    {
        SetupTestCompleteSignalTimer(document->getChild("Setup/TestCompleteSignal"));
    }

    Log(LOG_FN_ENTRY, "TestResultServer::Initialize(%s) done", document->ToString().c_str());
}

//-------------------------------------------------------------------------------------------------
void TestResultServer::Terminate(void)
{   // Update the test result file
    UpdateTestResultFile();
    // Call the base class terminate
    BepServer::Terminate();
}

//-------------------------------------------------------------------------------------------------
void TestResultServer::Run(volatile bool *terminateFlag/*=NULL*/)
{
    while(GetStatus() != BEP_TERMINATE)
    {   // Wait for a test result to be received
        m_processTestResults.Acquire();
        if(m_processTestResults.Wait(processTestResults, mSEC_nSEC(500)) == EOK)
        {   // Time to process the test results
            ProcessTestResults();
            // Signal result processing is complete
            m_processTestResults.Signal(resultProcessingComplete);
            m_processTestResults.Release();
        }
    }
}

//-------------------------------------------------------------------------------------------------
const bool TestResultServer::AddTestResult(std::string vin, const bool readFromFile) /*= false*/
{
    bool resultAdded = false;
    char tempVin[defaultVINLength+sequenceLength+1];
    Log(LOG_FN_ENTRY, "TestResultServer::AddTestResult(vin: %s)\n", vin.c_str());
    // Only add the result if we are archiving results
    if (m_numberOfTests)
    {   // Get the sequence number of this test
        if (readFromFile)
        {   // Possible Loss Compensation vin

            INT32 sequenceStartIndex = vin.find_last_of('.') + 1;
            Log(LOG_DETAILED_DATA, "Filename length: %d, Expected length: %d\n",
                  vin.length(),
                (GetVINLength() + sequenceLength + 1));

            // removed ridiculous logic:
            // if the filename length is longer than expected
            //      look for the sequence (extension) at offset 18 (hardcoded);
            // otherwise look for it at configured vinLength + 1.

            //            if((INT32)vin.length() > (INT32)(GetVINLength() + sequenceLength + 1))
            //                sequenceStartIndex = defaultVINLength + 1;
            //            else
            //                sequenceStartIndex = GetSequenceStartIndex();

            Log(LOG_DETAILED_DATA, "Sequence Start Index: %d\n", sequenceStartIndex);
            // Set the index from the filename
            m_sequenceNumber = atoi((vin.substr(sequenceStartIndex, sequenceLength)).c_str());
            Log(LOG_DEV_DATA, "Data from file -- VIN: %s, Sequence number: %d\n",
                  vin.c_str(), m_sequenceNumber);
        }
        else
        {
            if (++m_sequenceNumber > maxSequenceNumber) m_sequenceNumber = 0;
            std::string fmt = std::string("%s.%0") + itoa(sequenceLength, tempVin, 10) + "d";
            sprintf(tempVin, fmt.c_str(), vin.c_str(), m_sequenceNumber);
            vin = tempVin;
            Log(LOG_DEV_DATA, "Built Vin file name: %s\n", vin.c_str());
        }
        // Make sure this is a valid result
        if(!vin.empty() && (vin.length() > 1))
        {   // Add the test result to the list
        m_vehicleTestResults.push_back(vin);
        Log(LOG_DEV_DATA, "Added: %s to back of list: -->%s<--\n",
              vin.c_str(), m_vehicleTestResults.back().c_str());
        // Remove the test result file and the vin record for the oldest test if we have too many results
        std::string filename;
        int retval;
        Log(LOG_DEV_DATA, "Checking number of saved files: have %d;  max %d\n",
              m_vehicleTestResults.size(),GetNumberTests());
        while (m_vehicleTestResults.size() > GetNumberTests())
        {
            filename = GetTestResultPath() + m_vehicleTestResults.front();
            Log(LOG_DEV_DATA, "removing: '%s'\n", filename.c_str());
            if (!access(filename.c_str(), F_OK))
            {
                retval = unlink(filename.c_str());
                Log(LOG_DEV_DATA,"Return code: 0x%08X\n",retval);
            }
            else
            {
                Log(LOG_ERRORS,
                      "Unable to access file \"%s\%; skipping\n",filename.c_str());
            }
            m_vehicleTestResults.pop_front();
        }
        resultAdded = true;
    }
    }
    return(resultAdded);
}

inline void TestResultServer::AddVehicleBuildItem(const XmlNode *buildItem)
{   // Add the item to the build data
    Log(LOG_FN_ENTRY, "TestResultServer::AddVehicleBuildItem(%s)\n", buildItem->ToString().c_str());
    m_semVehicleBuild.CriticalSectionEnter();    // wait until we can updagte vehicle build data
    if(m_vehicleBuild != NULL) m_vehicleBuild->addChild(buildItem->Copy());
    m_semVehicleBuild.CriticalSectionExit();
}

void TestResultServer::UpdateTestResultFile(void)
{   // Only update the results if we are archiving results
    if (GetNumberTests() && !m_vehicleTestResults.empty())
    {   // Open the results file and write the current results
        FILE *outFile;
        if ((outFile = fopen((GetTestResultPath() + GetTestResultFile()).c_str(), "w")) != NULL)
        {   // Write all the results out to the file
            for (list<std::string>::iterator iter = m_vehicleTestResults.begin(); iter != m_vehicleTestResults.end(); iter++)
            {
                fprintf(outFile, "%s\n", (*iter).c_str());
            }
            // Close the file
            fclose(outFile);
        }
        else
            Log(LOG_ERRORS, "Could not open: %s\n", (GetTestResultPath() + GetTestResultFile()).c_str());
    }
}

const std::string TestResultServer::Read(const XmlNode *node, const INT32 rate) /*= 0*/
{
    // Check if this is a read of all failures
    std::string result;
    if (GetAllFailuresTag() == node->getName())
    {   // Build the result
        result.erase();
        // Get all the test failures
        XmlNodeMap failNodes;
        failNodes.DeepCopy(m_allFailures.getChildren());
        // Build the result string
        for (XmlNodeMapItr iter = failNodes.begin(); iter != failNodes.end(); iter++)
            iter->second->writeNode(result);
        // Clean up our mess
        failNodes.clear(true);
    }
    else
    {   // Call the base class read
        result = BepServer::Read(node, rate);
    }
    // Return the result
    return(result);
}

const std::string TestResultServer::Write(const XmlNode *node)
{   
    std::string result;
    if(m_data.Lock() == EOK)
    {
        result = BepServer::Write(node);      // Add the node to the test result list and notify subscribers
        m_data.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "Could not lock mutex in TestResultServer::Write()\n");
    }
    if (node->getName() == SET_DTC_TAG)           SetDTC(node); // Store the DTC
    else if (node->getName() == CLEAR_DTC_TAG)    ClearDTC(node);   // Clear a DTC
    else if (node->getName() != BEP_TEST_RESULT &&
             node->getName() != BEP_INTERMEDIATE_TEST_RESULT &&
             node->getName() != "InCycleTestNumber")  CheckTestResult(node);
    // Return the result of the write operation
    Log(LOG_FN_ENTRY, "TestResultServer::Write() - Added Node: %s\n", node->ToString().c_str());
    return(result);
}

//-------------------------------------------------------------------------------------------------
const INT32 TestResultServer::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 status = BEP_STATUS_ERROR;
    switch(code)
    {
    case TEST_RESULT_SERVER_PULSE_CODE:
        switch(value)
        {
        case FAIL_ALARM_PULSE:
            // Turn off the alarm
            ToggleFaultAlarmState(false);
            m_faultAlarmTimer.Stop();
            Log(LOG_DEV_DATA, "Turned off failure alarm and stopped the timer");
            status = BEP_STATUS_SUCCESS;
            break;

        case TEST_COMPLETE_SIGNAL_PULSE:
            // Turn off the test complete signal
            ToggleTestCompleteSignal(false);
            m_testCompleteSignalTimer.Stop();
            Log(LOG_DEV_DATA, "Turned off test complete signal and stopped the timer");
            status = BEP_STATUS_SUCCESS;
            break;

        default:   // Unknown pulse type
            Log(LOG_ERRORS, "Unknown pulse value: %d", value);
            status = BEP_STATUS_ERROR;
            break;
        }
        break;

    default:    // Let the base class handle this pulse
        status = BepServer::HandlePulse(code, value);
        break;
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
const std::string TestResultServer::Publish(const XmlNode* node)
{   
    // accept and fault lamp states  
    const XmlNode acceptLampOff(GetDataTag("AcceptLamp"),"0");
    const XmlNode faultLampOff(GetDataTag("FaultLamp"),"0");
    const XmlNode acceptLampOn(GetDataTag("AcceptLamp"),"1");
    const XmlNode faultLampOn(GetDataTag("FaultLamp"),"1");

    Log(LOG_FN_ENTRY, "TestResultServer::Publish(%s)\n", node->ToString().c_str());

    // Look for StartTest tag so we can reset the test results from the previous test
    if (((node->getName() == GetStartTestTag()) || (node->getName() == START_LOSSCOMPENSATION_DATA_TAG)) && (node->getValue() == "1"))
    {   // Clear all test failures from previous test
        Log(LOG_DEV_DATA, "Writing %s:%s\n", GetAllFailuresTag().c_str(), GetClearScrollBoxTag().c_str());
        if (m_nList != NULL) m_nList->Notify(GetAllFailuresTag(), GetClearScrollBoxTag());
        Log(LOG_DEV_DATA, "Done processing %s\n", node->ToString().c_str());
        // Set the test in progress flag
        SetTestInProgress(true);

        // Clear the ACCEPT and REJECT lamp bits
        if( m_updatePlcLamps)
        {
            BepServer::Write(&acceptLampOff);
            BepServer::Write(&faultLampOff);
        }
    }
    else if ((node->getName() == GetOverallTestTag()) && (node->getValue() != BEP_TESTING_RESPONSE))
    {//write in cycle value to test result
        m_inCycleTestNumber++;
        char inCycleRetestVal[11];
        sprintf(inCycleRetestVal,"%d",m_inCycleTestNumber);
        BepServer::Write("InCycleTestNumber", inCycleRetestVal);

        // Update the PLC lamps if desired
        if( m_updatePlcLamps)
        {
            if( node->getValue() == BEP_PASS_RESPONSE)
            {
                BepServer::Write(&acceptLampOn);
                BepServer::Write(&faultLampOff);
            }
            else
            {
                BepServer::Write(&acceptLampOff);
                BepServer::Write(&faultLampOn);
            }
        }
		if(ShouldSignalTestComplete())
		{
			ToggleTestCompleteSignal(true);
			m_testCompleteSignalTimer.Start();
		}
        // Build the test result message
        XmlNode passFailNode(GetOverallTestTag(), "");
        passFailNode.addChild("Description", "TestResult", ATTRIBUTE_NODE);
        passFailNode.addChild("Process", m_comm->GetName(), ATTRIBUTE_NODE);
        passFailNode.addChild("Result", node->getValue(), ATTRIBUTE_NODE);
        if(m_data.Lock() == EOK)
        {// Add the overall result to the test results
            m_data.addNode(passFailNode.Copy());
            //delete intermediate node if exists
            XmlNode intResultNode(BEP_INTERMEDIATE_OVERALL_RESULT, "");
            m_data.delNode(intResultNode.getName());
            m_data.Unlock();            
        }
        else
        {
            Log(LOG_ERRORS, "Could not lock mutex in TestResultServer::Publish()\n");
        }
        // Signal to start processing result
        m_processTestResults.Signal(processTestResults);
        m_inCycleTestNumber = 0;
    }
    else if (node->getName()== BEP_INTERMEDIATE_OVERALL_RESULT)
    {//received in cycle retest overall result value
        //write in cycle value to test result
        m_inCycleTestNumber++;
        char inCycleTestNumVal[11];
        sprintf(inCycleTestNumVal,"%d",m_inCycleTestNumber);
        BepServer::Write("InCycleTestNumber", inCycleTestNumVal);

        XmlNode passFailNode(GetOverallTestTag(), "");
        passFailNode.addChild("Description", "TestResult", ATTRIBUTE_NODE);
        passFailNode.addChild("Process", m_comm->GetName(), ATTRIBUTE_NODE);
        passFailNode.addChild("Result", node->getValue(), ATTRIBUTE_NODE);

        XmlNode intResultNode(BEP_INTERMEDIATE_OVERALL_RESULT, "");
        intResultNode.addChild("Description", "Flag indicating this is a result prior to rerun", ATTRIBUTE_NODE);
        intResultNode.addChild("Process", m_comm->GetName(), ATTRIBUTE_NODE);
        intResultNode.addChild("Result", node->getValue(), ATTRIBUTE_NODE);
        if(m_data.Lock() == EOK)
        {   // Add the overall result to the test results
            //dataCopy.DeepCopy(m_data);
            m_data.addNode(passFailNode.Copy());
            m_data.addNode(intResultNode.Copy());
            m_data.Unlock();
        }
        else
        {
            Log(LOG_ERRORS, "Could not lock mutex in TestResultServer::Publish()\n");
        }
        //process results
        ProcessIntermediateTestResults();
    }
    else if (node->getName() == GetVehicleBuildTag())
    {   // Save vehicle build so we can write it to test result file
        SetVehicleBuild(node);
        std::string vin;
        try
        {   // VIN was specified in the build information
            vin = node->getChild(GetVinTag())->getValue();
        }
        catch (BepException &err)
        {   // The VIN was not specified in the build information
            vin.erase();
        }
        SetCurrentVin(vin);
    }
    else if (node->getName() == GetVinTag())
    {   // Save the current vin
        SetCurrentVin(node->getValue());
    }
    else if (m_vehiclebuildItems.find(node->getName()) != m_vehiclebuildItems.end())
    {   // Add the data to the build data
        AddVehicleBuildItem(node);
    }
    else if (node->getName() == GetDriverNumberTag())
    {   // Only add the driver number if the test is in progress
        Log(LOG_DEV_DATA, "Processing %s - TestInProgress: %s\n", node->getName().c_str(),
            IsTestInProgress() ? "TRUE" : "FALSE");
        if (IsTestInProgress())
        {
            AddVehicleBuildItem(node);
            Log(LOG_DEV_DATA, "Added driver vehicle number to vehicle build data - %s\n", node->getValue().c_str());
        }
        else
        {
            Log(LOG_DEV_DATA, "Drive number NOT ADDED to vehicle build data - %s\n", node->getValue().c_str());
        }
    }
    else if ( (node->getName() == GetKeyPressTag()) && (node->getValue() == REPRINT_RESULTS))
    {
        Log(LOG_DEV_DATA, "Reprinting Results:\n");

        XmlNode node(REPRINT_RESULTS, (m_testResultPublshedPath + m_vehicleTestResults.back()));

        BepServer::Write(&node);
    }
    else if(node->getName() == std::string("RelayFile"))
    {
        // for debugging, this functions like OverallTest, but does not process
        // anything, just relays the file name supplied as the value.
        Log(LOG_FN_ENTRY, "Relaying results file: %s\n",node->getValue().c_str());
        // Publish the test results
        BepServer::Write(TEST_RESULT_TAG, node->getValue());
    }
    else if(node->getName() == std::string("StartRetest"))
    {
        // Retest is starting, so clear all reported DTCs
        CheckClearAllDTCs(node->getValue());
        Log(LOG_FN_ENTRY, "Received StartRetest: %s\n",node->getValue().c_str());
    }

    Log(LOG_FN_ENTRY, "TestResultServer::Publish() done\n");

    return(BEP_SUCCESS_RESPONSE);
}

void TestResultServer::CalculateTestStats(XmlNode &statsNode)
{
    XmlNode testCount("NumberOfTests", "");
    XmlNode passPercent("TestPassPercent", "");
    XmlNode passCount("TestPassCount", "");
    INT32 totalTests = 0;
    INT32 totalPass  = 0;
    // Count the number of passed tests
    Log(LOG_DEV_DATA, "Analyzing %d test results...", m_data.size());
    for(XmlNodeMapItr iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        string testStepName = iter->second->getName();
        if(testStepName.compare("AllFailures") &&
           testStepName.compare("DTC") &&
           (testStepName.find("OverallTest") == testStepName.npos))
        {
            try
            {   // Check the result.  If pass count it
                string result = iter->second->getAttribute("Result")->getValue();
                if(!result.compare("Pass")) totalPass++;
                // Increment the number of actual tests performed
                if (!result.compare("Pass")  || !result.compare("Fail") ||
                    !result.compare("Abort") || !result.compare("Timeout"))
                {   // Only count if result is from a test that was performed.
                    // Do not count Skip or TestNotRequired or similar results
                    totalTests++;
                }
            }
            catch(XmlException &excpt)
            {
                Log(LOG_ERRORS, "%s is not a valid test result, not counting", testStepName.c_str());
            }
        }
    }
    // Calculate the percent pass
    float percentPass = ((float)totalPass / (float)totalTests) * 100.0;
    Log(LOG_DEV_DATA, "Current test stats:\n\tTotal Tests: %d\n\tTests Passed: %d\n\tPercent Pass: %6.2f",totalTests, totalPass, percentPass);
    // Set the stats into the proper nodes
    char buff[32];
    testCount.setValue(itoa(totalTests, buff, 10));
    passCount.setValue(itoa(totalPass, buff, 10));
    passPercent.setValue(CreateMessage(buff, sizeof(buff), "%6.2f", percentPass));
    Log(LOG_DEV_DATA, "Set individual stat values");
    // Add the nodes to the stats node
    statsNode.addChild(testCount.Copy());
    statsNode.addChild(passCount.Copy());
    statsNode.addChild(passPercent.Copy());
    Log(LOG_DEV_DATA, "Added stats to node");
}

const std::string TestResultServer::GetSpecialInfo(const std::string &tag, const std::string info)
{
    std::string data;

    if(m_data.Lock() == EOK)
    {
        try
        {   // Find the node requested
            data = m_data.getNode(tag)->getAttribute(info)->getValue();
        }
        catch (...)
        {   // The node does not exist
            data = BEP_UNAVAILABLE_RESPONSE;
        }
        m_data.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "Could not lock mutex in TestResultServer::GetSpecialInfo()\n");
        data = BEP_UNAVAILABLE_RESPONSE;
    }
    return(data);
}

void TestResultServer::ResetTestResults(void)
{
    if(m_data.Lock() == EOK)
    {
        Log(LOG_DEV_DATA,"Clearing old test results\n");
        // When a start test is received, clear old results
        ClearData();
        Log(LOG_DEV_DATA,"Clearing old failures\n");
        // Clear all failures
        m_allFailures.clear();
        m_allFailures.setName(GetAllFailuresTag());
        m_allFailures.setValue("");
        // Clear all reported DTCs
        Log(LOG_DEV_DATA,"Clearing old DTCs\n");
        m_reportedDTC.clear();
        m_reportedDTC.setName(DTC_TAG);
        m_retestNumber = 0;
        m_prevRetestNumber = 0;
        m_data.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "TestResultServer::ResetResults was unable to reset test results!\n");
    }
}

void TestResultServer::CheckTestResult(const XmlNode *node)
{
    bool notifySubscribers = false;
    Log(LOG_FN_ENTRY, "TestResultServer::CheckTestResult(%s) begin\n", node->ToString().c_str());
    XmlNodeMapCItr iter;
    // See if the node has been reported as a failure already
    if ((iter = m_allFailures.getChildren().find(node->getName())) != m_allFailures.getChildren().end())
    {   // Check if the retested result is PASS
        Log(LOG_DEV_DATA, "Test %s was previously reported as %s\n", iter->second->getName().c_str(),
            iter->second->getAttribute(BEP_RESULT)->getValue().c_str());
        if (node->getAttribute(BEP_RESULT)->getValue() == BEP_PASS_STATUS)
        {   // Remove the node from the failure list
            m_allFailures.delChild(iter->second);
            // Flag to notify subscribers of the updated failures
            notifySubscribers = true;
            Log(LOG_DEV_DATA, "Retest of %s has been reported as %s, allFailures node updated:%s\n", node->getName().c_str(),
                node->getAttribute(BEP_RESULT)->getValue().c_str(), m_allFailures.ToString().c_str());
        }
    }
    else
    {   // This is not a retest
        Log(LOG_DEV_DATA, "Initial test of %s, %s\n", node->getName().c_str(), node->getAttribute(BEP_RESULT)->getValue().c_str());
    }
    // Check if this is a failed test result
    if ((node->getAttribute(BEP_RESULT)->getValue() == BEP_FAIL_STATUS) ||
        (node->getAttribute(BEP_RESULT)->getValue() == BEP_TIMEOUT_STATUS) ||
        (m_includeAbortInAllFailures && (node->getAttribute(BEP_RESULT)->getValue() == BEP_ABORT_STATUS)))
    {   // Add the result to the AllFailures node and Publish the node
        m_allFailures.addChild(node->Copy());
        notifySubscribers = true;
        Log(LOG_DEV_DATA, "Added test %s(%s) to allFailures node\n", node->getName().c_str(), node->getAttribute(BEP_RESULT)->getValue().c_str());
        // If fault alarm is in use, turn on the alarm
        if(FailAlarmEnabled())  ToggleFaultAlarmState(true);
    }
    else
    {   // This is a passed test
        Log(LOG_DEV_DATA, "Test %s not added to allFailures because result is %s\n", node->getName().c_str(), node->getAttribute(BEP_RESULT)->getValue().c_str());
    }
    // Determine if subscribers need notification
    if ((m_nList != NULL) && notifySubscribers) 
    {
        m_nList->Notify(&m_allFailures);
        if(ConnectToQnxDataServer())
        {   // Add all test failures to the data server
            string dataServerFailureList(",");
            for(XmlNodeMapCItr iter = m_allFailures.getChildren().begin(); iter != m_allFailures.getChildren().end(); iter++)
            {
                dataServerFailureList += (iter->second->getName() + ",");
            }
            // Update the data in the QNX data server
            Log(LOG_DEV_DATA, "Set test failure list in QNX Data Server to [%s], status: %s", dataServerFailureList.c_str(),
                ConvertStatusToResponse(m_qnxDataServer.UpdateQnxDataServerVariable(GetAllFailuresTag(), dataServerFailureList)).c_str());
        }
    }
    Log(LOG_FN_ENTRY, "TestResultServer::CheckTestResult() complete\n");
}

const std::string TestResultServer::ReportResults(void)
{   // Place all the tags into the message structure
    std::string filename("");               // the name of the test result file
    std::string reportedFilename("");       // the name of the test result file to report to the system
    Log(LOG_FN_ENTRY, "TestResultServer::ReportResults()\n");
    // Write the results to a file and add to list
    if (AddTestResult(GetCurrentVin(), false))
    {   // Open the file to contain the results
        FILE *outFile;
        filename = GetTestResultPath() + m_vehicleTestResults.back();
        reportedFilename = m_testResultPublshedPath + m_vehicleTestResults.back();
        if ((outFile = fopen(filename.c_str(), "w")) != NULL)
        {   // Write the vehicle build information to the test result file
            fprintf(outFile, "<%s>", BEP_VEHICLE_TEST_FILE);
            // Only write the vehicle build if this is not a loss compensation cycle
            if (GetCurrentVin() != GetLossCompensationVin())
            {   // Print the vehicle build information to the log file
                try
                {
                    fprintf(outFile, "%s", GetVehicleBuild().c_str());
                }
                catch(BepException &e)
                {   // Error getting the vehicle build info
                    Log(LOG_ERRORS, "Error writing vehicle build info to test result file for VIN:%s, Reason:%s\n",
                        GetCurrentVin().c_str(), e.what());
                }

            }
            // Write the results and close the file
            fprintf(outFile, "<%s>", BEP_TEST_RESULT);

            if (ImportInterCcrtResults() && GetCurrentVin() != GetLossCompensationVin())
            {//inter ccrt comm results should be imported request result by identifier
               AddInterCcrtCommServerResults(outFile);
            }
            if(m_data.Lock() == EOK)
            {
                for (XmlNodeMapItr iter = m_data.begin(); iter != m_data.end(); iter++)
                {   // Make sure the data is not part of our subscribe list
                    if ((GetStartTestTag() != iter->second->getName()) &&
                        (GetVehicleBuildTag() != iter->second->getName()) &&
                        (GetVinTag() != iter->second->getName()) &&
                        (GetKeyPressTag() != iter->second->getName()) &&
                        (SET_DTC_TAG != iter->second->getName()) &&
                        (CLEAR_DTC_TAG != iter->second->getName()))
                    {   // Write the node to the test result file
                        fprintf(outFile, "%s", iter->second->ToString().c_str());
                    }
                }
                if(GetErrorCodeTestTag() != "" && GetErrorTag() != "")
                {
                    //Build error test code node
                    BuildErrorCodeTestsNode(outFile, &m_data);
                }
                m_data.Unlock();
            }
            else
            {
                Log(LOG_ERRORS, "Could not lock mutex in TestResultServer::ReportResults()\n");
            }
            fprintf(outFile, "%s", m_reportedDTC.ToString().c_str());
            try
            {
                // Check to see if we should put the brake forces/speeds into the test result file
                if( true == m_forceArrayInResults)
                {
                    AddIcmDataToResults( outFile, "BrakeForceData", m_forceArrayFile);
                }
                if( true == m_speedArrayInResults)
                {
                    AddIcmDataToResults( outFile, "SpeedData", m_speedArrayFile);
                }
            }
            catch(...)
            {
            }
            fprintf(outFile, "</%s>", BEP_TEST_RESULT);
            fprintf(outFile, "</%s>", BEP_VEHICLE_TEST_FILE);
            fclose(outFile);
            // Update the list of vins
            UpdateTestResultFile();
        }
        else Log(LOG_ERRORS, "Could not open file: %s\n", (GetTestResultPath() + m_vehicleTestResults.back()).c_str());
    }
    Log(LOG_FN_ENTRY, "TestResultServer::ReportResults() done\n");
    // Return the filename
    return(reportedFilename);
}

void TestResultServer::SetDTC(const XmlNode *dtcNode)
{
    XmlNode *moduleNode;
    // Get the name of the vehicle module
    std::string vehicleModule = dtcNode->getAttribute(BEP_PROCESS)->getValue();
    // Get a pointer to the vehicle module node
    try
    {
        moduleNode = const_cast<XmlNode *>(m_reportedDTC.getChild(vehicleModule));
    }
    catch (...)
    {   // The node was not found, create a new one
        moduleNode = m_reportedDTC.addChild(vehicleModule, "", ELEMENT_NODE);
        Log(LOG_DEV_DATA, "Creating new node for %s DTCs\n", vehicleModule.c_str());
    }
    // Build the DTC node to be added
    XmlNode *dtc;
    if(dtcNode->getChildren().size() > 0)
    {   // ASSUMPTION: Each node has only one DTC
//        dtc = const_cast<XmlNode *>(dtcNode->getChild(COMMON_MODULE_FAULT_TAG)->getChildren().getNode(0)->Copy());
        dtc = const_cast<XmlNode *>(dtcNode->getChildren().getNode(0)->Copy());
    }
    else
    {
        dtc = new XmlNode(dtcNode->getAttribute(BEP_RESULT)->getValue(), dtcNode->getAttribute(BEP_DESCRIPTION)->getValue());
        dtc->addChild(DTC_CODE, dtcNode->getValue(), ATTRIBUTE_NODE);
    }
    // Add the DTC node to the list
    moduleNode->addChild(dtc);
    Log(LOG_DEV_DATA, "Stored DTC information for module %s: %s\n", vehicleModule.c_str(), dtc->ToString().c_str());
}

void TestResultServer::ClearDTC(const XmlNode *dtcNode)
{
    XmlNode *moduleNode;
    // Get the name of the vehicle module
    std::string vehicleModule = dtcNode->getAttribute(BEP_PROCESS)->getValue();
    // Get a pointer to the vehicle module node
    try
    {
        moduleNode = const_cast<XmlNode *>(m_reportedDTC.getChild(vehicleModule));
        // Determine the type of clear operation to perform
        if (CLEAR_ALL_DTC == dtcNode->getValue())
        {   // Clear all DTCs that were reported for this module
            moduleNode->clear();
            // Remove the module name from the list
            m_reportedDTC.delChild(moduleNode);
            Log(LOG_DEV_DATA, "Cleared all DTCs for %s module.\n", vehicleModule.c_str());
        }
        else
        {   // Clear a specific DTC
            try
            {
                moduleNode->delChild(dtcNode->getValue().c_str());
                Log(LOG_DEV_DATA, "Cleared DTC %s from %s module.\n", dtcNode->getValue().c_str(), vehicleModule.c_str());
            }
            catch (...)
            {   // DTC was not set in the first place
                Log(LOG_ERRORS, "Error clearing DTC %s from %s module.  DTC may not have been reported.\n",
                    dtcNode->getValue().c_str(), vehicleModule.c_str());
            }
        }
    }
    catch (...)
    {
        Log(LOG_ERRORS, "Could not find a DTC node for vehicle module %s\n", vehicleModule.c_str());
    }
}

void TestResultServer::CheckClearAllDTCs(const string &retestNumber)
{
    int newRetestNumber = atoi(retestNumber.c_str());
    if (newRetestNumber != 0)
    {
        if (m_prevRetestNumber != newRetestNumber)
        {
            Log(LOG_DEV_DATA, "Clearing all reported DTCs for retest number %d.\n", newRetestNumber);
            m_prevRetestNumber = newRetestNumber;
            ClearAllReportedDTCs();
        }
    }
}

void TestResultServer::ClearAllReportedDTCs()
{
    if(m_data.Lock() == EOK)
    {
        // Clear all reported DTCs
        m_reportedDTC.clear();
        m_reportedDTC.setName(DTC_TAG);
        Log(LOG_DEV_DATA,"Reported DTCs cleared\n");
        m_data.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "TestResultServer::ClearAllReportedDTCs() was unable to clear reported DTCs!\n");
    }
}

//-------------------------------------------------------------------------------------------------
void TestResultServer::AddInterCcrtCommServerResults( FILE* &outFile)
{
    Log(LOG_FN_ENTRY, "Importing Inter Ccrt Comm Server Results");
    XmlNode resultRequest(GetDataTag("TestResult"), GetCurrentVin());
    string response;
    XmlParser parser;
    INT32 status = m_interCcrtServerComm->Read(&resultRequest, response, true);
    Log(LOG_DEV_DATA, "Test result read status: %s", ConvertStatusToResponse(status).c_str());
    if(BEP_STATUS_SUCCESS == status)
    {//write to file

        string resultFile, tag;
        m_interCcrtServerComm->GetNext(tag, resultFile, response);
        XmlNodeMap *testResults = NULL;
        try
        {
            Log(LOG_DEV_DATA, "Loading test result file: %s", resultFile.c_str()); 
            testResults = const_cast<XmlNodeMap *>(&parser.ReturnXMLDocument(resultFile)->getChildren());

            for(XmlNodeMapItr iter = testResults->begin(); iter != testResults->end(); iter++)
            {
                Log(LOG_DETAILED_DATA, "Wrote node to file: %s\n",iter->second->ToString().c_str());
                fprintf(outFile, "%s",iter->second->ToString().c_str());
            }
        }
        catch(XmlException &excpt)
        {
            testResults = NULL;
            Log(LOG_ERRORS, "Could not load ccrt comm test result file, not using ccrt comm results: %s", excpt.GetReason());
        }
        catch(BepException &excpt)
        {
            testResults = NULL;
            Log(LOG_ERRORS, "Could not load ccrt comm test result file, not using ccrt comm results: %s", excpt.GetReason());
        }

    }
    else
    {
        Log( LOG_ERRORS, "Unable to obtain InterCcrtCommServerResults for %s\n", GetCurrentVin().c_str());
    }
}

//-------------------------------------------------------------------------------------------------
void TestResultServer::AddIcmDataToResults( FILE* &outFile, const std::string &nodeTag, const std::string &fileName)
{
    char            *data;
    int             fd;
    struct stat     fileStat;
    int             bytesRead = 0;

    Log(LOG_FN_ENTRY, "Enter TestResultServer::AddIcmDataToResults( %s)\n", fileName.c_str());

    // Make sure we have a filename
    if( false == fileName.empty())
    {
        // See how big the file is
        memset( &fileStat, 0, sizeof( fileStat));
        if( (-1 != stat(fileName.c_str(), &fileStat)) && (fileStat.st_size > 0))
        {
            // Allocate a block of data to hold the entire file contents
            const int buffSz = fileStat.st_size+10;
            data = new char[buffSz];
            memset( data, 0, buffSz);

            // Open the data file
            if( -1 != (fd = open( fileName.c_str(), O_RDONLY)))
            {
                // Read the data file
                if((bytesRead=read( fd, data, fileStat.st_size)) > 0)
                {
                    // Add a CDATA node in the output file for the data
                    fprintf( outFile, "<%s><![CDATA[", nodeTag.c_str());
                    fprintf( outFile, "%s", data);
                    fprintf( outFile, "]]></%s>\n", nodeTag.c_str());
                }
                else
                {
                    Log( LOG_ERRORS, "Unable to read() %s\n", fileName.c_str());
                }

                close( fd);
            }
            else
            {
                Log( LOG_ERRORS, "Unable to open() %s\n", fileName.c_str());
            }
            delete[] data;
        }
        else
        {
            Log( LOG_ERRORS, "Unable to get stat() for %s\n", fileName.c_str());
        }
    }

    Log(LOG_FN_ENTRY, "Exit TestResultServer::AddIcmDataToResults( %s)\n", fileName.c_str());
}

//-----------------------------------------------------------------------------
void TestResultServer::BuildErrorCodeTestsNode(FILE* &outFile, XmlNodeMap *testResultNodeMap)
{   // Clear the error codes node
    INT32 errorNum = 0;
    bool foundErrors = false;
    char buff[5];
    string errorCode,errorNode; // Store Code and Tag name in temporary variables for failure or dtc
    Log(LOG_FN_ENTRY, "TestResultServer::BuildErrorCodeTestsNode()");
    XmlNode errorCodes(GetErrorCodeTestTag(), "");
    XmlNodeMap testResults;     // Store the AllFailures nodes from the parameter testResultNode
    XmlNodeMap dtcNodeMap;      // Store the dtc nodes from the parameter testResultNode
    XmlNodeMap errorCodeMap;    // Store reformated dtc nodes for sending to broadcast system
    XmlNodeMapItr iter;         // Iterate through the XmlNodeMap

    // Store the nodes of AllFailures in an XmlNodeMap
    testResults.DeepCopy(testResultNodeMap->getNode("AllFailures")->getChildren());
    try
    {   // Get the DTC node map to report to PFS
        dtcNodeMap.DeepCopy(m_reportedDTC.getChildren());
        // Store all subnodes for all DTC categories (ie Emissions) in one XmlNodeMap
        for(iter = dtcNodeMap.begin();iter != dtcNodeMap.end();iter++)
        {   // copy any error codes to the errorCodes map
            errorCodeMap.DeepCopy(iter->second->getChildren(), false);
        }
        // Done with DTC node map, clear it out
        dtcNodeMap.clear(true);
        // Loop through each DTC and create a map with nodes that have been reformatted for
        //      reporting to Broadcast
        for(iter = errorCodeMap.begin();iter != errorCodeMap.end();iter++)
        {   // put the error codes into the proper format and store under errorCodes XmlNodeMap
            errorCode = iter->second->getAttributes().getNode(DTC_CODE)->getValue();
            errorNode = (GetErrorTag() + itoa(errorNum++,buff,10));
            errorCodes.addChild(errorNode, errorCode, ELEMENT_NODE);
            foundErrors = true;
            Log(LOG_DEV_DATA, "Added error -- tag:%s, value:%s", errorNode.c_str(), errorCode.c_str());
        }
        // Done with error code map, clear it out
        errorCodeMap.clear(true);
    }
    catch(...)
    {   // Do nothing, DTCs may not have been reported
    }

    // Loop through the test results looking for failures
    for(iter = testResults.begin(); iter != testResults.end(); iter++)
    {   // Check if the current node has a Fail result
        Log(LOG_DEV_DATA, "Evaluating test result: %s", iter->second->ToString().c_str());
        try
        {
            if((iter->second->getAttributes().getNode(BEP_RESULT)->getValue() == BEP_FAIL_STATUS) ||
               (iter->second->getAttributes().getNode(BEP_RESULT)->getValue() == BEP_ABORT_STATUS))
            {   // Get the code and add it to our list
                errorCode = iter->second->getAttributes().getNode(BEP_TEST_RESULT_CODE)->getValue();
                bool validCode = true;
                for(UINT16 ii = 0; (ii < errorCode.length()) && validCode; ii++)
                {
                    validCode = isdigit(errorCode[ii]) || (('A' <= errorCode[ii]) && (errorCode[ii] <= 'F'));
                }
                // If this is a valid error code and it is not 0000, try to add it to the list
                if(validCode && errorCode.compare("0000"))
                {   // Only add the error if it contains all digits and is not already in the list
                    const XmlNodeMap *errors = &errorCodes.getChildren();
                    bool addError = true;
                    for(XmlNodeMapCItr errorIter = errors->begin(); (errorIter != errors->end()) && addError; errorIter++)
                    {
                        addError = errorCode.compare(errorIter->second->getValue());
                    }
                    if(addError)
                    {
                        errorNode = (GetErrorTag() + itoa(errorNum++,buff,10));
                        errorCodes.addChild(errorNode, errorCode, ELEMENT_NODE);
                        foundErrors = true;
                        Log(LOG_DEV_DATA, "Added error -- tag:%s, value:%s\n", errorNode.c_str(), errorCode.c_str());
                    }
                    else
                    {
                        Log(LOG_DEV_DATA, "Error code %s is already in the list.  Not adding it a second time.", errorCode.c_str());
                    }
                }
                else
                {   // This is not a valid error code
                    Log(LOG_ERRORS, "Invalid error code reported - code:%s, errorNode: %s\n", errorCode.c_str(), iter->second->ToString().c_str());
                }
            }
        }
        catch(XmlException &err)
        {   // Exception creating error code list
            Log(LOG_ERRORS, "XmlException creating Error Code list - %s\n", err.what());
        }
    }
    // Add the error code node to the list of test results
    if(foundErrors)
    {
        // Add error codes to test result file
        fprintf( outFile, "%s", errorCodes.ToString().c_str());
    }
    // Clear the error codes node since we are done with it
    errorCodes.clear();
    // Clear the error codes node since we are done with it
    testResults.clear( true);
    Log(LOG_FN_ENTRY, "TestResultServer::BuildErrorCodeTestsNode() done\n");
}

//-----------------------------------------------------------------------------
inline const string TestResultServer::GetErrorTag(void)
{
    return(m_errorTag);
}

//-----------------------------------------------------------------------------
inline const string TestResultServer::GetErrorCodeTestTag(void)
{
    return(m_errorCodeTag);
}


//-----------------------------------------------------------------------------
inline void TestResultServer::SetErrorTag(const string &tag)
{
    m_errorTag = tag;
}

//-----------------------------------------------------------------------------
inline void TestResultServer::SetErrorCodeTestTag(const string &tag)
{
    m_errorCodeTag = tag;
}

//-----------------------------------------------------------------------------
void TestResultServer::CleanupResultDirectory()
{
    DIR *resultDir;
    struct dirent *dirEntry;
    // Attempt to open the directory
    if((resultDir = opendir(GetTestResultPath().c_str())) != NULL)
    {   // Get each file in the directory
        while((dirEntry = readdir(resultDir)) != NULL)
        {   // Check if the current file is in the list
            string name(dirEntry->d_name);
            list<string>::iterator iter = find(m_vehicleTestResults.begin(), m_vehicleTestResults.end(), name);
            if(iter == m_vehicleTestResults.end())
            {   // Current file is not in the list, get rid of it
                if(name.compare(".") && name.compare("..") && name.compare(m_testResultFile))
                {
                    string fileName(GetTestResultPath() + name);
                    Log(LOG_DEV_DATA, "Removing orphaned file: %s", fileName.c_str());
                    unlink(fileName.c_str());
                }
            }
        }
        closedir(resultDir);
    }
    else
    {   // Error opening the directory
        Log(LOG_ERRORS, "Could not open %s - %s", GetTestResultPath().c_str(), strerror(errno));
    }
}

//-------------------------------------------------------------------------------------------------
string TestResultServer::ProcessTestResults(void)
{
    Log(LOG_DEV_DATA, "Reporting results");

    if(m_data.Lock() == EOK)
    {   // Calculate the test stats and add the the results
        XmlNode statsNode("TestStats", "");
        Log(LOG_DEV_DATA, "Calculating test stats...");
        CalculateTestStats(statsNode);
        Log(LOG_DEV_DATA, "Stats calculated, adding to test result data...");
        Log(LOG_DEV_DATA, "Test Stats: %s", statsNode.ToString().c_str());
        statsNode.addChild("Description", "Test Result Statistics", ATTRIBUTE_NODE);
        statsNode.addChild("Process", "Statistics", ATTRIBUTE_NODE);
        m_data.addNode(statsNode.Copy());
        Log(LOG_DEV_DATA, "Stats added to test result data");
        // Add the all failures node to the data
        m_data.addNode(m_allFailures.Copy());
        m_data.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "Could not lock mutex in TestResultServer::Publish()\n");
    }
    // Publish the test results
    Log(LOG_DEV_DATA, "Publishing test results...");
    BepServer::Write(TEST_RESULT_TAG, ReportResults());
	// Possibly generate a pass confirmation file
	if(GeneratePassConfirmationFile())
	{   // Check if the overall result was pass and this is not a loss comp result
		XmlNodeMapItr iter = m_data.find(GetOverallTestTag());
		string result = (iter != m_data.end()) ? iter->second->getAttribute(BEP_RESULT)->getValue() : BEP_UNAVAILABLE_RESPONSE;
		Log(LOG_DEV_DATA, "Generate Pass Confirmation file - Overall Test Result: %s", result.c_str());
		if(GetCurrentVin().compare(GetLossCompensationVin()))
		{  
			FILE *passFile = NULL;
			string fileName = PassConfirmationFilePath() + GetCurrentVin() + ".DVT";
			Log(LOG_DEV_DATA, "Creating pass confirmation file: %s", fileName.c_str());
			if((passFile = fopen(fileName.c_str(), "w")) != NULL)
			{   // Get the current date to write into the file
                            // Get the current time
            time_t systemTime = time(NULL);
            struct tm *currentTime = localtime(&systemTime);
            char timeBuffer[32];
            // Convert the current time to a string
            strftime(timeBuffer, sizeof(timeBuffer), "%Y%m%d%H%M%S", currentTime);
            timeBuffer[14] = '\0';
            string result = !result.compare(BEP_PASS_RESPONSE) ? "PASS" : "FAIL";

                Log(LOG_DEV_DATA, "Writing date %s%s to pass confirmation file", timeBuffer, result.c_str());
                //Write the timestamp and the test result to the test result file
                fprintf(passFile,"%s%s", timeBuffer, result.c_str());
				fclose(passFile);
			}
			else
			{
				Log(LOG_ERRORS, "Could not open %s, not saving pass confirmation file", fileName.c_str());
			}
		}
		else
		{
			Log(LOG_DEV_DATA, "Not generating pass confirmation file for %s", GetCurrentVin().c_str());
		}
	}
    // 3/2/5 - ews - added this conditional, so we can retest.
    if(true == m_clearBuildOnOverall)
    {   // Test has been completed, set flag
        SetTestInProgress(false);
        // Clear out the current vin
        SetCurrentVin(noVIN);
        Log(LOG_DEV_DATA,"Clearing old vehicle build\n");
        // Clear the vehicle build
        m_semVehicleBuild.CriticalSectionEnter();    // wait until we can update vehicle build
        if(m_vehicleBuild)  m_vehicleBuild->clear();
        m_semVehicleBuild.CriticalSectionExit();
    }
    else
    {
        Log(LOG_DEV_DATA,"TestResultServer::Publish() NOT clearing build data\n");
    }

    // clear the test results unless the flag is false
    if(m_clearResultsOnOverall == true)
    {
        // Clear all previous test results
        Log(LOG_DEV_DATA, "Resetting test results\n");
        ResetTestResults();
        Log(LOG_DEV_DATA,"Old results cleared\n");  
    }
    Log(LOG_FN_ENTRY, "Done Reporting Results");
    return BEP_SUCCESS_RESPONSE;
}




//-------------------------------------------------------------------------------------------------
string TestResultServer::ProcessIntermediateTestResults(void)
{
    Log(LOG_DEV_DATA, "User is retesting, reporting previous cycle results");
    if(m_data.Lock() == EOK)
    {   
        // Calculate the test stats and add the the results
        XmlNode statsNode("TestStats", "");
        Log(LOG_DEV_DATA, "Calculating test stats...");
        CalculateTestStats(statsNode);
        Log(LOG_DEV_DATA, "Stats calculated, adding to test result data...");
        Log(LOG_DEV_DATA, "Test Stats: %s", statsNode.ToString().c_str());
        statsNode.addChild("Description", "Test Result Statistics", ATTRIBUTE_NODE);
        statsNode.addChild("Process", "Statistics", ATTRIBUTE_NODE);
        m_data.addNode(statsNode.Copy());
        Log(LOG_DEV_DATA, "Stats added to test result data");
        // Add the all failures node to the data
        m_data.addNode(m_allFailures.Copy());
        m_data.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "Could not lock mutex in TestResultServer::Publish()\n");
    }
    // Publish the test results
    Log(LOG_DEV_DATA, "Publishing test results...");
    BepServer::Write(BEP_INTERMEDIATE_TEST_RESULT, ReportResults());

    Log(LOG_FN_ENTRY, "Done Reporting Rerun Results");
    return BEP_SUCCESS_RESPONSE;
}






//-----------------------------------------------------------------------------
inline const bool& TestResultServer::ConnectToQnxDataServer(const bool *connect) /*= NULL*/
{
    if(connect != NULL)  m_connectToQnxDataServer = *connect;
    return m_connectToQnxDataServer;
}

//-----------------------------------------------------------------------------
inline const bool& TestResultServer::FailAlarmEnabled(const bool *enabled /*= NULL*/)
{
    if(enabled != NULL)  m_failAlarmEnabled = *enabled;
    return m_failAlarmEnabled;
}

//-----------------------------------------------------------------------------
void TestResultServer::SetupFaultAlarmTimer(const XmlNode *timerSettings)
{   // Get the timeout for the fail alarm
    UINT64 timeout = 0;
    try
    {
        timeout = atol(timerSettings->getChild("FaultAlarmDuration")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Fail alarm duration not specified, using 5s - %s", excpt.GetReason());
        timeout = 5000;
    }
    // Setup the timer
    m_faultAlarmTimer.SetPulseCode(TEST_RESULT_SERVER_PULSE_CODE);
    m_faultAlarmTimer.SetPulseValue(FAIL_ALARM_PULSE);
    m_faultAlarmTimer.Initialize(GetProcessName(), mSEC_nSEC(timeout), mSEC_nSEC(timeout));
    m_faultAlarmTimer.Stop();
}

//-----------------------------------------------------------------------------
void TestResultServer::SetupTestCompleteSignalTimer(const XmlNode *timerSettings)
{   // Get the signal duration
    UINT64 duration = 0;
    try
    {
        duration = atol(timerSettings->getChild("TestCompleteSignalDuration")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Test complete signal duration not specified, using 500ms - %s", excpt.GetReason());
        duration = 500;
    }
    // Setup the timer
    m_testCompleteSignalTimer.SetPulseCode(TEST_RESULT_SERVER_PULSE_CODE);
    m_testCompleteSignalTimer.SetPulseValue(TEST_COMPLETE_SIGNAL_PULSE);
    m_testCompleteSignalTimer.Initialize(GetProcessName(), mSEC_nSEC(duration), mSEC_nSEC(duration));
    m_testCompleteSignalTimer.Stop();
}

//-----------------------------------------------------------------------------
inline const bool& TestResultServer::ShouldSignalTestComplete(const bool *sigTestComplete /*= NULL*/)
{
    if(sigTestComplete != NULL)  m_signalTestComplete = *sigTestComplete;
    return m_signalTestComplete;
}

//-----------------------------------------------------------------------------
inline void TestResultServer::ToggleFaultAlarmState(const bool alarmOn)
{
    Log(LOG_DEV_DATA, "Turning failure alarm %s", alarmOn ? "ON" : "OFF");
    INamedDataBroker ndb;
    string response;
    ndb.Write(GetDataTag("FailureAlarmTag"), alarmOn ? "1" : "0", response, true);
    if(alarmOn)  m_faultAlarmTimer.Start();
}

//-----------------------------------------------------------------------------
inline void TestResultServer::ToggleTestCompleteSignal(const bool signalTestComplete)
{
    Log(LOG_DEV_DATA, "Turning test complete signal %s", signalTestComplete ? "ON" : "OFF");
    INamedDataBroker ndb;
    string response;
    ndb.Write(GetDataTag("TestCompleteSignalTag"), signalTestComplete ? "1" : "0", response, true);
}

//-----------------------------------------------------------------------------
const bool& TestResultServer::GeneratePassConfirmationFile(const bool *create/*=NULL*/)
{
	if(create != NULL)  m_createPassConfirmationFile = *create;
	return m_createPassConfirmationFile;
}

//-----------------------------------------------------------------------------
const bool& TestResultServer::ImportInterCcrtResults(const bool *import/*=NULL*/)
{
	if(import != NULL)  m_importInterCcrtCommServerResults = *import;
	return m_importInterCcrtCommServerResults;
}

//-----------------------------------------------------------------------------
const string& TestResultServer::PassConfirmationFilePath(const string *path/*=NULL*/)
{
	if(path != NULL)  m_passConfirmationPath = *path;
	return m_passConfirmationPath;
}
