//******************************************************************************
// Description:
//  Vehicle GenericTC base class that defines the functionality that each
//  vehicle component needs.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Common/GenericTC.cpp $
// 
// 4     1/21/08 11:17a Derickso
// Updated DisplayTimedPrompt to take optional values to display as part
// of the prompt.
// 
// 3     1/09/08 2:13p Derickso
// Added DisplayTimedPrompt that accepts an int prompt box parameter.
// Updated to continue initialization even if the ICM is not running.
// 
// 2     12/14/07 7:49a Derickso
// Removed tandem axle support from WHEELINFO.  Roller speed reads do not
// work correctly for two axle machines.
// 
// 1     11/01/07 2:40p Derickso
// Refactored to add tandem wheels in wheelspeed functions.
// Updated to utilize STL better.
// General refactoring to make use of existing functions instead of
// duplicating code.
// 
// 1     10/25/07 1:49p Derickso
// Refactored to make use of the STL and other common functions.
// 
// 23    2/22/07 8:54a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added AccelerateVehicleToSpeed().
// - Removed #if 0 blocks.
// - Added a separator between the methods.
// 
// 22    1/17/07 3:25p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added m_configCopy = NULL to the constructor.
// - If the configuration has changed, delete m_configCopy and create a
// new copy in Initialize().
// - If m_testResult is not NULL in Initialize(), delete it and create a
// new object.
// - Set the member variables to their default values in Activate().
// - Close the speed and wheel distance files in Deactivate().
// - Added ReloadConfiguration().
// - Initalize the member variables to their default values in
// WarmInitialize().
// - Added a test step attribute "AltValue" that can be passed to
// CommandTestStep() in CommandObjective().
// - Added TestStepReadMemStat() and UpdateTaskStats().
// 
// 21    10/25/06 3:52p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added m_regName and m_regAlias to hold the IPC names
//     - Added AddIPCLink() and RemoveIPCLink() 
//     - Added Activate(), Deactivate(), ShouldReloadConfigFile(), and 
//       WarmInitialize().  These methods are used to keep the test
// components
//       running when a new drive curve starts.
//     - Fixed the version of SendSubtestResultWithDetail() that was
// prepending the component name twice.
// 
// 20    5/24/06 2:53p Cward
// Added Release Park Brake Prompt at the end of the electric park
// brake/park pawl combo test.
//
// 5     5/16/06 4:54p Bmeinke
// Added ReleaseParkBrake prompt at the end of the electric park
// brake/parl pawl combo test
//
// 4     5/16/06 3:40p Bmeinke
// Core version 06050801 (unmodified)
//
// 19    4/26/06 6:59p Cward
// Removed old commented out code,fixed ParkBrakeDirection logic, changed
// printfs to Logs, added Default values for many Paramters moved atob to
// GeneralUtilites
//
// 18    4/13/06 3:24a Cward
// Added several 'init' methods that get called at the beginning of the
// park brake and aprk pawl tests to allow the Ford brake component to
// start monitoring the brake switch AFTER the "Shift" and "Apply park
// brake" prompts have been displayed. Changed hard coded unit strings
// with new units strings from GenericTC.h Added method
// GetAbsReferenceForce() to allow FordTRW ABS to use 'n' samples just
// prior to sending the reduction command as the reference force for ABS
// calculations Added SendTestResultWithDetail() and
// SendSubtestResultWithDetail() method that take a TestResultDetails
// object Added GetElapsedTime() to return the elapsed time for a test
// step AccelerateInReverse() now calls SendTestResultWithDetail()  to
// report the reverse test arm speed and achieved speed
//
// 3     4/01/06 4:56p Bmeinke
// Added several 'init' methods that get called at the beginning of the
// park brake and aprk pawl tests to allow the Ford brake component to
// start monitoring the brake switch AFTER the "Shift" and "Apply park
// brake" prompts have been displayed.
//
// 2     3/20/06 10:47a Bmeinke
// Changed hard coded unit strings with new units strings from GenericTC.h
// Added method GetAbsReferenceForce() to allow FordTRW ABS to use 'n'
// samples just prior to sending the reduction command as the reference
// force for ABS calculations
// Added SendTestResultWithDetail() and SendSubtestResultWithDetail()
// method that take a TestResultDetails object
// Added GetElapsedTime() to return the elapsed time for a test step
// AccelerateInReverse() now calls SendTestResultWithDetail()  to report
// the reverse test arm speed and achieved speed
//
// 1     3/20/06 9:10a Bmeinke
// Updating Ford to latest Core
//
// 17    2/15/06 7:13p Cward
// Make SpeedforTome a callible test step. Add setup operator responses
// Added MotorController initialization commands to some test steps. Added
// atob method to convert ascii to boolean. Added default value parameter
// to GetTestStepParameter methods.
//
// 16    11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 2     9/20/05 10:10a Bmeinke
// At the start of the park brake and park pawl teests, set all motor
// controller speed and torque values to 0 before sending any other
// commands to the motor controller (i saw some issues at AAI where the
// front wheels would accel to 10MPH when running the park brake test
// after having just completed the park pawl test).
//
// 1     9/16/05 10:50a Bmeinke
// Updating to Tata Core
//
// 2     8/23/05 3:00p Bmeinke
// Beautified
//
// 15    7/20/05 8:39p Cward
// Added support for new TaskMon
//
// 14    5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 13    5/09/05 5:00p Gpattison
// Updates Initialize(filename) to call base class instead of
// duplicating the code that the base class was already performing.
//
// 13    4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 12    4/29/05 8:00a Bmeinke
// Added/Changed some parameters in the for Hydraulic Park Brake
// and Park Pawl Tests
//
// 12    4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 11    3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 5     3/07/05 10:33a Bmeinke
// Changed AnalyzeParkBrakeTest() to call ValidateParkBrakeTestDistances()
// instead of ValidateParkPawlTestDistances().
//
// 10    3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 9     2/16/05 9:17p Cward
// use BposReadInt intead of atoi so that hex values can be read.
//
// 4     1/31/05 9:11a Bmeinke
// Inside ReadDataArray(): Make sure we do not try to read more data than
// the array contains
// If reading the data array fails, return an error
//
// 3     12/22/04 8:11p Bmeinke
// Added all park brake and park pawl methods (and supporting routines)
// Modified AccelerateToTestSpeed() to pass prompt values to the
// DisplayPrompt() method (needed in order to pass target speeds to eCATS)
// Added TestStepEnableMotorLoading() and TestStepDisableMotorLoading()
// Modified getDataTag() such that, if the desired tag is not in
// m_dataTags, the tag passed in is returned
//
// 2     12/14/04 8:49p Bmeinke
// Modified the AccelerateToTestSpeed() method so that it looks for
// Prompt1Val/Prompt2Val before calling DisplayPrompt()
//
// 8     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 7     9/01/04 9:03p Cward
// Added paramter to allow test status to remain displayed after the
// component is terminated. Added overloaded functions for Get Parameter,
// Get Test Step Info, and Get Test Step Parameter, to allow transparent
// getting of values from either the Test step info or the paramter list.
//
// 6     6/24/04 12:41a Cward
// Removed pound if 0 block.
//
// 5     6/09/04 11:06p Cward
//
// 4     5/25/04 3:37p Cward
// Add a check for zero speed in engage machine
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 7     11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
//
// 6     11/13/03 7:07p Derickso
// Commented out default variable value.
//
// 5     10/27/03 5:10p Derickso
// Updated TimeRemaining methods to check for NULL
// parameter prior to attempting to get the
// TestStepInfo.
//
// 4     10/20/03 9:44a Derickso
// Debugged and tested against a vehicle.
//
// 3     9/24/03 6:58a Derickso
// Updated for latest ServicesLayer which has different approach to adding
// details to prompts and test results.
//
// 2     9/22/03 4:45p Derickso
// Latest version with updated architecture.
// TestComponent no longer exists.
//
// 3     9/08/03 7:33a Derickso
// Corrected compile warning.
//
// 2     8/26/03 10:25a Derickso
// Added secondary timeout methods to allow a test step
// to have multiple timeout timers.
//
// 1     8/21/03 8:59a Derickso
// 05WK CCRT Project
//
// 3     8/19/03 8:23a Derickso
// Updated TimeRemaining to take an optional argument to be used for
// determining if time is remaining.  If the optional argument is not
// provided, the timeout attribute for the test step will be used.
//
// 2     8/08/03 10:10a Khykin
// Updated with changes from Flatrock.
//    Revision 1.4  2003/06/24 14:27:01  tenharmsel
//    Updating from FordChicago
//
//    Revision 1.2  2003/06/18 13:11:48  tenharmsel
//    Syncing directory trees with the FordChicago system
//
//
// 3     5/19/03 7:54a Khykin
// Updated the Display/Remove prompt methods to increase efficiency.
//
// 2     4/22/03 12:50p Khykin
// Adding a return to the UpdatePrompts() method.
//
// 1     4/22/03 10:27a Khykin
// Initial Working Checkin.
//
// 2     4/07/03 9:19a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:02:58  tbochene
//
//    PooBag
//
//
// 1     1/27/03 11:11a Khykin
// Initial Checkin
//
// 1     1/18/03 7:19a Khykin
// Initial Checkin
//
// 20    11/22/02 3:12p Khykin
// Added the previous overall status.
//
// 19    11/11/02 3:04p Khykin
// Added prompt locking capabilities.
//
// 18    11/04/02 6:29a Khykin
// Updating fault processing to include software fail.
//
// 17    10/21/02 1:39p Khykin
// Updated system tags, logging information, and removed obsolete code.
//
// 16    10/07/02 10:41a Khykin
// Checked in the wrong copy.  Updating for prompt priorities.
//
// 13    9/10/02 11:56a Khykin
// Updating the SendSubtestResultWithDetail methods.
//
// 11    8/23/02 8:02a Kh
// Synchronizing the pit machine with VSS.
//
// 9     5/24/02 1:48p Kh
// Updating due to changes in the response definitions.
//
// 8     3/25/02 6:18p Kh
// Updating due to changes in the base classes.
//
// 7     2/06/02 12:14a Kh
// Updating to add a wrapper around the named data broker interfac
//
// 6     2/01/02 4:41p Kh
// Updating to add a wrapper around the named data broker interfac
//
// 5     1/25/02 5:31p Kh
// Adding methods to retrieve the component information and hide the user
// from the user from communication related methods.
//
// 4     1/10/02 8:58a Kh
// Updating the interface initialization to take a configuration.
//
// 2     1/03/02 8:31a Kh
// Debugging complete.
//
// 1     12/26/01 10:52a Kh
// Initial Checkin.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <stdlib.h>     // max()
#include <sys/pathmgr.h>    // pathmgr_symlink
#include <sys/utsname.h>    // uname()
#include <sys/dir.h>        // alphasort
#include <algorithm>
#include "GenericTC.h"

const uint32_t  GenericTC::m_maxIcmCountValue = 0x0FFFFFFFL;

//=============================================================================
GenericTC::GenericTC() : BepServer(), m_foregroundTest(true), m_semInitialized(0), 
m_semCommand(0), m_semReady(1), m_semResponse(0), m_fatalFailure(false), 
m_forceFile(-1), m_forceFileOpen(false), m_speedFile(-1), 
m_speedFileOpen(false), m_wheelDistanceFile(-1), m_wheelDistanceFileOpen(false), m_faultFile(""),
m_faultFileTime( 0), m_regName( ""), m_regAlias( "")
{
    m_ndb = NULL;               // set the ndb interface to null
    m_prompt = NULL;            // set the prompt interface to null
    m_fault = NULL;             // set the fault interface to null
    m_testResult = NULL;        // set the test result interface to null
    m_configCopy = NULL;        // set copy of configuration to null
    m_retest = false;           // set the retest flag to false (initial test)

    m_command = "";             // reset the command
    m_value = "";               // reset the value

    m_overallResult = BEP_TESTING_RESPONSE; // set the overall result to testing
    m_previousResult = BEP_UNTESTED_RESPONSE; // set at end of test

    SetStatus(BEP_INITIALIZED_STATUS);      // set status to initialized
}

//=============================================================================
GenericTC::~GenericTC()
{
    if(m_ndb != NULL)           // delete the ndb interface to null
        delete m_ndb;
    if(m_prompt != NULL)        // delete the prompt interface to null
        delete m_prompt;
    if(m_fault != NULL)         // delete the fault interface to null
        delete m_fault;
    if(m_testResult != NULL)    // delete the test result interface to null
    {
        // write the status to TestNotStarted
        if(!GetParameterBool("KeepTestStatusOnExit"))
            UpdateTestStatus(TEST_STATUS_NOT_STARTED);
        delete m_testResult;
    }
    if(m_statusCheckItems.size() > 0)
    {   // Clear the status check item map
        m_statusCheckItems.clear(true);
    }
    // Close the speed file if it was opened
    CloseSpeedFile();
    // Close the speed file if it was opened
    CloseWheelDistanceFile();

    // Remove our existing IPC channel link
    if( false == m_regAlias.empty())
    {
        pathmgr_unlink( m_regAlias.c_str());
        m_regAlias.clear();
    }
}

//=============================================================================
void GenericTC::Initialize(const std::string &fileName)
{
    BepServer::Initialize(fileName);
}

//=============================================================================
void GenericTC::Initialize(const XmlNode *config)
{
    std::string cfgFile( GetConfigFile());
    std::string regName( "");
    std::string regAlias( "");
    std::string ipcName;

    // Remove our existing IPC channel link
    RemoveIPCLink();

    if( NULL != config)
    {// Dont try to copy or delete invalid pointers
        if(config != m_configCopy)
        {
            if(NULL != m_configCopy)  delete m_configCopy;
            m_configCopy = config->Copy();
        }

        try
        {
            m_regName = CreateUniqueIpcName( std::string( __progname), cfgFile);

            XmlNode *ipcNameNode = (XmlNode *)config->getChild( "Setup/Communication/Name");
            ipcName = ipcNameNode->getValue();

            // Create a symlink path using the desired IPC channel name from the config file
            //  that points to our uniquely named channel name from above
            if( ipcName[0] != '/')  m_regAlias = ipcNamePrefix + std::string( "/") + ipcName;
            else                    m_regAlias = ipcNamePrefix + ipcName;

            // Overwrite it with a unique qxe/cfg path (so IBepComm uses our unique name)
            ipcNameNode->setValue( m_regName);
        }
        catch( XmlException &err)
        {
            Log( LOG_ERRORS, "No IPC channel name found in Setup/Communication/Name\n");
            m_regName.clear();
            m_regAlias.clear();
        }
    }

    // Initialize the base component
    BepServer::Initialize(config);

    // Log method entry (Logger not initialized until after Base class initialization complete)
    Log(LOG_FN_ENTRY,"GenericTC::Initialize(called)\n");

    // retrieve a reference to the test objectives
    m_objectives = config->getChildren().getNode("Objectives")->getChildren();

    // retrieve a reference to the test steps
    m_testSteps = config->getChildren().getNode("TestSteps")->getChildren();

    // get the setup node
    const XmlNode *setup = config->getChild("Setup");

    // set up the interfaces for reporting results
    if(m_ndb == NULL)           //  the ndb interface to null
        m_ndb = new INamedDataBroker(IsDebugOn());

    // set the name of the component
    SetComponentName(setup->getChild("Name")->getValue());

    try
    {   // get the data tags list that will be used tag abstraction
        m_parameters = (XmlNodeMap)(setup->getChild("Parameters")->getChildren());
    }
    catch(...)
    {   // if not being used, no big deal
        Log(LOG_ERRORS, "No Parameters Provided\n");
    }

    // Load the faults from file
    try
    {
        XmlParser parser;
        m_faultFile = GetRootDir() + config->getChild("Setup/FaultList")->getValue();
        m_faultFileTime = GetFileModificationTime(m_faultFile);
        const XmlNode *faultList = parser.ReturnXMLDocument( m_faultFile);
        m_faults.DeepCopy(faultList->getChildren());
        Log(LOG_FN_ENTRY, "Loaded fault list from file: %s\n",
            (GetRootDir() + config->getChild("Setup/FaultList")->getValue()).c_str());
    }
    catch(BepException &err)
    {   // Error loading faults
        Log(LOG_ERRORS, "Error loading faults from file.\n\tError: %s\n", err.what());
    }

    // Get the list of valid prompts
    try
    {
        m_prompts.DeepCopy(config->getChild("Setup/Prompts")->getChildren());
    }
    catch(BepException &err)
    {       // Could not get list of valid prompts
        Log("XML Exception loading prompt list - %s\n", err.Reason().c_str());
        m_prompts.clear(true);
    }
    // Get the items to check for the StatusCheck() method
    try
    {
        m_statusCheckItems.DeepCopy(config->getChild("Setup/StatusCheckItems")->getChildren());
    }
    catch(BepException &err)
    {       // Could not get list of valid status check items
        Log("XML Exception loading status check items - %s\n", err.Reason().c_str());
        m_statusCheckItems.clear(true);
    }
    // Check if Test Component is to be run in the Foreground or Background
    try
    {
        SetTestDomain(config->getChild("Setup/TestDomain")->getValue());
    }
    catch(BepException &bepEx)
    {   // Could not get test domain
        Log(LOG_ERRORS, "%s: GenericTC::SetTestDomain(), default to Foreground - %s\n", 
            GetComponentName().c_str(), bepEx.GetReason());
        m_foregroundTest = true;
    }
    catch(...)
    {   // exception trying to get test domain
        Log(LOG_ERRORS, "%s: GenericTC::SetTestDomain() - Caught Unknown Error, default to Foreground\n", 
            GetComponentName().c_str());
        m_foregroundTest = true;
    }
    if(m_prompt == NULL)        //  the prompt interface to null
    {
        m_prompt = new IPromptServer();
        m_prompt->Initialize(PROMPT_SERVER_NAME, DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT, IsDebugOn());
        m_prompt->SetProcessName(GetProcessName());
    }
    if(m_fault == NULL)         //  the fault interface to null
    {
        m_fault = new IFaultServer();
        m_fault->Initialize(FAULT_SERVER_NAME, DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT, IsDebugOn());
    }

    if(m_testResult != NULL)    // delete the test result interface to null
    {
        // write the status to TestNotStarted
        if(!GetParameterBool("KeepTestStatusOnExit"))
            UpdateTestStatus(TEST_STATUS_NOT_STARTED);
        delete m_testResult;
        m_testResult = NULL;
    }
    if(m_testResult == NULL)    //  the test result interface to null
    {
        m_testResult = new ITestResultServer();
        m_testResult->Initialize(TEST_RESULT_SERVER_NAME, "Client", IsDebugOn());
    }

    try
    {   // reset the specified system tags Note: must happen after INamedDataBroker created
        m_systemTags = setup->getChild("SystemTagsToReset")->getChildren();
    }
    catch(...)
    {   // if not being used, no big deal
        m_systemTags.clear();
        Log(LOG_ERRORS, "No System Data Tags To Reset\n");
    }
    // Attempt to get the name of the ICM manager from the config file
    try
    {
        SetICMManagerName(config->getChild("Setup/Parameters/ICMName")->getValue());
    }
    catch(XmlException &e)
    {       // ICM name not specified in the config file
        Log(LOG_ERRORS, "ICMName not specified, using mgr/ICM  - %s\n", e.GetReason());
        SetICMManagerName("mgr/ICM");
    }
    // Create a new comm object to talk to the ICM manager
    try
    {
        m_icmMgrComm.Initialize(GetICMManagerName(), "Client");
    }
    catch(BepException &excpt)
    {
        Log(LOG_ERRORS, "ERROR: Could not connect to ICM manager: %s", excpt.GetReason());
    }

    // retrieve the number of rollers in the system
    try
    {
        SetRollerCount(SystemReadInt(ROLLER_COUNT));
        Log(LOG_DEV_DATA, "Set the roller count\n");
    }
    catch(BepException &err)
    {   // Could not read system value
        Log(LOG_ERRORS, "Unable to Get ROLLER_COUNT - %s\n", err.Reason().c_str());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "Unexpected error reading the roller count\n");
    }
    // Load the park brake directions for later comparison
    m_validParkBrakeDirections.clear();
    m_validParkBrakeDirections.push_back("Forward");
    m_validParkBrakeDirections.push_back("Reverse");
    m_validParkBrakeDirections.push_back("Both");

    Log(LOG_FN_ENTRY, "GenericTC %s Initialization Complete (%s)\n", ipcName.c_str(), m_comm->GetName().c_str());

    // Create our "friendly" IPC channel name as a symlink to our less friendly unique IPC name
    //  created above
    AddIPCLink();

    SetStatus(BEP_TESTING_STATUS);          //  set the status to testing
}

//=============================================================================
void GenericTC::InitializationComplete(void)
{   //  signal to allow component to register
    m_semInitialized.CriticalSectionExit();
}

//=============================================================================
void GenericTC::AddIPCLink(void)
{
    // If we have an IPC channel alias and a NEW unique IPC channel name
    if( (false == m_regName.empty()) && ( false == m_regAlias.empty()))
    {
        RemoveIPCLink();
        Log( LOG_DEV_DATA, "Adding IPC link <%s> -> <%s>\n", m_regAlias.c_str(), m_regName.c_str());

        // Create a symlink using the desired IPC channel name from the config file
        //  that points to the named IPC channel we created a unique name for above
        if( -1 == pathmgr_symlink( std::string( ipcNamePrefix + "/" + m_regName).c_str(), 
                                   m_regAlias.c_str()))
        {
            Log( LOG_ERRORS, "Unable to create IPC symlink: %s\n", strerror( errno));
        }
    }
}

//=============================================================================
void GenericTC::RemoveIPCLink(void)
{
    // Remove our existing IPC channel link
    if( false == m_regAlias.empty())
    {
        pathmgr_unlink( m_regAlias.c_str());
    }
}

//=============================================================================
void GenericTC::Activate(void)
{
    Log(LOG_FN_ENTRY,"Enter GenericTC::Activate()\n");
    //Moved from Deactivate
    //Initialize Member Variables
    m_fatalFailure = false;
    m_retest = false;           // set the retest flag to false (initial test)
    m_command = "";             // reset the command
    m_value = "";               // reset the value
    m_previousResult = BEP_UNTESTED_RESPONSE; // set at end of test
    m_overallResult = BEP_TESTING_RESPONSE;         // reset the overall result

    try
    {
        // If our config file has changed
        if( ShouldReloadConfigFile())
        {
            // Perform full initialization
            BepServer::Activate();
            InitializationComplete();
        }
        else
        {
            WarmInitialize();
        }
        Log(LOG_DEV_DATA, "%s: calling InitializationComplete()\n", GetComponentName().c_str());
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "XML Error Activating Component %s: %s\n", GetProcessName().c_str(), e.what());
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Error Activating Component %s: %s\n", GetProcessName().c_str(), e.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "Unknown exception Activating Component %s\n", GetProcessName().c_str());
    }

    Log(LOG_FN_ENTRY,"Exit GenericTC::Activate()\n");
}

//=============================================================================
void GenericTC::Deactivate(void)
{
    Log(LOG_FN_ENTRY,"Enter GenericTC::Deactivate()\n");

    BepServer::Deactivate();

    // Close the speed file if it was opened
    CloseSpeedFile();
    // Close the speed file if it was opened
    CloseWheelDistanceFile();

    // Remove our existing IPC channel link
    RemoveIPCLink();

    Log(LOG_FN_ENTRY,"Exit GenericTC::Deactivate()\n");
}

//=============================================================================
void GenericTC::ReloadConfiguration(void)
{
    Log(LOG_FN_ENTRY, "GenericTC::ReloadConfiguration()\n");
    //Clear all items that were copied
    m_faults.clear(true);
    m_prompts.clear(true);
    m_statusCheckItems.clear(true);
    //call baseclass
    BepServer::ReloadConfiguration();
    Log(LOG_FN_ENTRY, "Exit GenericTC::ReloadConfig() done\n");
}

//=============================================================================
bool GenericTC::ShouldReloadConfigFile(void)
{
    return( FileHasChanged( GetConfigFile(), m_cfgModTime) || FileHasChanged( m_faultFile, m_faultFileTime));
}

//=============================================================================
void GenericTC::WarmInitialize(void)
{
    Log( LOG_FN_ENTRY, "Enter GenericTC::WarmInitialize()\n");

    //Initialize Member Variables

    // retrieve a reference to the test steps - need to reinit because we write the result value
    m_testSteps = m_configCopy->getChildren().getNode("TestSteps")->getChildren();

    // Check if Test Component is to be run in the Foreground or Background
    try
    {
        SetTestDomain(m_configCopy->getChild("Setup/TestDomain")->getValue());
    }
    catch(BepException &bepEx)
    {   // Could not get test domain
        Log(LOG_ERRORS, "%s: GenericTC::SetTestDomain(), default to Foreground - %s\n", 
            GetComponentName().c_str(), bepEx.GetReason());
        m_foregroundTest = true;
    }
    catch(...)
    {   // exception trying to get test domain
        Log(LOG_ERRORS, "%s: GenericTC::SetTestDomain() - Caught Unknown Error, default to Foreground\n", 
            GetComponentName().c_str());
        m_foregroundTest = true;
    }
    //moved from deactive in case reset pulse recieved after we deactivate
    if(m_testResult != NULL)    // delete the test result interface to null
    {
        // write the status to TestNotStarted
        if(!GetParameterBool("KeepTestStatusOnExit"))
            UpdateTestStatus(TEST_STATUS_NOT_STARTED);
        delete m_testResult;
        m_testResult = NULL;
    }
    if(m_testResult == NULL)    //  the test result interface to null
    {
        m_testResult = new ITestResultServer();
        m_testResult->Initialize(TEST_RESULT_SERVER_NAME, "Client", IsDebugOn());
    }

    // Load the park brake directions for later comparison
    m_validParkBrakeDirections.clear();
    m_validParkBrakeDirections.push_back("Forward");
    m_validParkBrakeDirections.push_back("Reverse");
    m_validParkBrakeDirections.push_back("Both");

    // Create our "friendly" IPC channel name as a symlink to our less friendly unique IPC name
    //  created above
    AddIPCLink();
    // Re-subscribe for all our tags
//    SubscribeTags();    // This is also occurring in the Register function?
    //  set the status to testing
    SetStatus(BEP_TESTING_STATUS);
    InitializationComplete();
    // Register the server so we subscribe for our tags again
    Register();

    Log( LOG_FN_ENTRY, "Exit GenericTC::WarmInitialize()\n");
}

//=============================================================================
const std::string GenericTC::Register(void)
{
    std::string status( BEP_FAILURE_RESPONSE);

    // until the component is up and running, sit here and wait
    m_semInitialized.CriticalSectionEnter();

    UpdateTestStatus(TEST_STATUS_IN_PROGRESS);

    status = BepServer::Register();

    return( status);
}

//=============================================================================
const std::string GenericTC::Command(const XmlNode *temp)
{
    std::string status(BEP_SUCCESS_RESPONSE);   // set the status to success

    // look in the objective list for the command that was sequenced
    XmlNodeMapItr iter = m_objectives.find(temp->getName());
    if((iter != m_objectives.end()) || (temp->getName() == COMPONENT_OBJECTIVE_FINISHUP))
    {
        if( BEP_DEACTIVATE_STATUS != GetStatus())
        {
            status = SetCommand (temp->getName(), temp->getValue());
        }
        else
        {
            status = BEP_UNAVAILABLE_RESPONSE;
        }
    }
    else
    {
        status = BepServer::Command( temp);
    }

    Log(LOG_FN_ENTRY, "Command %s, %s, Returning: %s\n", temp->getName().c_str(), temp->getValue().c_str(), status.c_str());

    return(status);
}

//=============================================================================
void GenericTC::Terminate(void)
{
    Log(LOG_FN_ENTRY, "GenericTC %s Received Terminate Command\n", m_comm->GetName().c_str());
    // write the status to TestNotStarted
    if(!GetParameterBool("KeepTestStatusOnExit")) UpdateTestStatus(TEST_STATUS_NOT_STARTED);

    // Call the base class Terminate method.
    BepServer::Terminate();
    m_command = BEP_TERMINATE_STATUS;   // set the command to terminate status
    m_semCommand.CriticalSectionExit(); // signal to the component to wake up
}

//=============================================================================
void GenericTC::Run(volatile bool *terminateFlag /*=NULL*/)
{
    std::string status;
    std::string tag;
    std::string value;

    Log(LOG_FN_ENTRY, "%s GenericTC Process Request\n", GetComponentName().c_str());

    // send notification to the tester
    std::string message;                // buffer for incoming messages

    while(GetStatus() != BEP_TERMINATE_STATUS)
    {   // if there is a command to sequence
        if(GetCommand (tag, value) == BEP_SUCCESS_RESPONSE)
        {   // command the objectives and the value (if any) that was commanded

            if(tag != BEP_TERMINATE_STATUS)
            {
                Log(LOG_DEV_DATA, "Processing Command Received: %s, %s\n", tag.c_str(), value.c_str());
                try
                {   // command the component objective
                    status = CommandObjective (m_objectives.getNode(tag), value);
                }
                catch(BepException &e)
                {
                    Log(LOG_ERRORS, "GenericTC %s BepException: %s\n", GetComponentName().c_str(), e.what());
                    status = BEP_SOFTWAREFAIL_RESPONSE;
                }
                catch(...)
                {
                    Log(LOG_ERRORS, "GenericTC %s Unknown Exception Commanding Objective\n", GetComponentName().c_str());
                    status = BEP_SOFTWAREFAIL_RESPONSE;
                }

                SetObjectiveResult(status);     // send back the result
            }
        }
        else
            Log(LOG_ERRORS, "%s Error: no command sequenced\n", GetComponentName().c_str());
    }

    Log(LOG_FN_ENTRY, "%s GenericTC Terminating\n", GetComponentName().c_str());
}

//=============================================================================
void GenericTC::UpdateResult(const std::string &result, std::string &overallResult)
{
    // Evaluate the test step status and update the objective status.
    Log(LOG_FN_ENTRY, "GenericTC.cpp UpdateResult #1\n");
    Log(LOG_FN_ENTRY, "%s GenericTC::UpdateResult(%s, %s)\n", 
        GetComponentName().c_str(), result.c_str(), overallResult.c_str());

    if((result == BEP_UNSUPPORTED_RESPONSE) || (result == BEP_INVALID_RESPONSE) ||
       (result == BEP_UNAVAILABLE_RESPONSE) || (result == BEP_ERROR_RESPONSE) ||
       (result == BEP_NO_IMPLEMENTED_RESPONSE))
    {
        overallResult = result;
        Log(LOG_DEV_DATA, "GenericTC.cpp overallResult = %s\n", overallResult.c_str());
    }
    else if(result == BEP_ABORT_RESPONSE)
    {
        if((overallResult != BEP_UNSUPPORTED_RESPONSE) && (overallResult != BEP_INVALID_RESPONSE) &&
           (overallResult != BEP_UNAVAILABLE_RESPONSE) && (overallResult != BEP_ERROR_RESPONSE) &&
           (overallResult != BEP_NO_IMPLEMENTED_RESPONSE))
        {
            overallResult = BEP_ABORT_RESPONSE;
            Log(LOG_DEV_DATA, "GenericTC.cpp overallResult = BEP_ABORT_RESPONSE\n");
        }
    }
    else if(result == BEP_SOFTWAREFAIL_RESPONSE)
    {
        if((overallResult != BEP_UNSUPPORTED_RESPONSE) && (overallResult != BEP_INVALID_RESPONSE) &&
           (overallResult != BEP_UNAVAILABLE_RESPONSE) && (overallResult != BEP_ERROR_RESPONSE) &&
           (overallResult != BEP_NO_IMPLEMENTED_RESPONSE) && (overallResult != BEP_ABORT_RESPONSE))
        {
            overallResult = BEP_SOFTWAREFAIL_RESPONSE;
            Log(LOG_DEV_DATA, "GenericTC.cpp overallResult = BEP_SOFTWAREFAIL_RESPONSE\n");
        }
    }
    else if(result == BEP_FATALFAIL_RESPONSE)
    {
        if((overallResult != BEP_UNSUPPORTED_RESPONSE) && (overallResult != BEP_INVALID_RESPONSE) &&
           (overallResult != BEP_UNAVAILABLE_RESPONSE) && (overallResult != BEP_ERROR_RESPONSE) &&
           (overallResult != BEP_NO_IMPLEMENTED_RESPONSE) && (overallResult != BEP_ABORT_RESPONSE) &&
           (overallResult != BEP_SOFTWAREFAIL_RESPONSE))
        {
            overallResult = BEP_FATALFAIL_RESPONSE;
            Log(LOG_DEV_DATA, "GenericTC.cpp overallResult = BEP_FATALFAIL_RESPONSE\n");
        }
    }
    else if(result == BEP_TIMEOUT_RESPONSE)
    {
        if((overallResult != BEP_UNSUPPORTED_RESPONSE) && (overallResult != BEP_INVALID_RESPONSE) &&
           (overallResult != BEP_UNAVAILABLE_RESPONSE) && (overallResult != BEP_ERROR_RESPONSE) &&
           (overallResult != BEP_NO_IMPLEMENTED_RESPONSE) && (overallResult != BEP_ABORT_RESPONSE) &&
           (overallResult != BEP_SOFTWAREFAIL_RESPONSE) && (overallResult != BEP_FATALFAIL_STATUS))
        {
            overallResult = BEP_TIMEOUT_RESPONSE;
            Log(LOG_DEV_DATA, "GenericTC.cpp overallResult = BEP_TIMEOUT_RESPONSE\n");
        }
    }
    else if(result == BEP_FAIL_RESPONSE)
    {
        if((overallResult != BEP_UNSUPPORTED_RESPONSE) && (overallResult != BEP_INVALID_RESPONSE) &&
           (overallResult != BEP_UNAVAILABLE_RESPONSE) && (overallResult != BEP_ERROR_RESPONSE) &&
           (overallResult != BEP_NO_IMPLEMENTED_RESPONSE) && (overallResult != BEP_ABORT_RESPONSE) &&
           (overallResult != BEP_SOFTWAREFAIL_RESPONSE) && (overallResult != BEP_FATALFAIL_STATUS) &&
           (overallResult != BEP_TIMEOUT_RESPONSE))
        {
            overallResult = BEP_FAIL_RESPONSE;
            Log(LOG_DEV_DATA, "GenericTC.cpp overallResult = BEP_FAIL_RESPONSE\n");
        }
    }
    else if(result == BEP_PASS_RESPONSE)
    {
        if((overallResult != BEP_UNSUPPORTED_RESPONSE) && (overallResult != BEP_INVALID_RESPONSE) &&
           (overallResult != BEP_UNAVAILABLE_RESPONSE) && (overallResult != BEP_ERROR_RESPONSE) &&
           (overallResult != BEP_NO_IMPLEMENTED_RESPONSE) && (overallResult != BEP_ABORT_RESPONSE) &&
           (overallResult != BEP_SOFTWAREFAIL_RESPONSE) && (overallResult != BEP_FATALFAIL_STATUS) &&
           (overallResult != BEP_TIMEOUT_RESPONSE) && (overallResult != BEP_FAIL_RESPONSE))
        {
            overallResult = BEP_PASS_RESPONSE;
            Log(LOG_DEV_DATA, "GenericTC.cpp overallResult = BEP_PASS_RESPONSE\n");
        }
    }
    else if(result == BEP_TESTING_RESPONSE)
    {
        if((overallResult != BEP_UNSUPPORTED_RESPONSE) && (overallResult != BEP_INVALID_RESPONSE) &&
           (overallResult != BEP_UNAVAILABLE_RESPONSE) && (overallResult != BEP_ERROR_RESPONSE) &&
           (overallResult != BEP_NO_IMPLEMENTED_RESPONSE) && (overallResult != BEP_ABORT_RESPONSE) &&
           (overallResult != BEP_SOFTWAREFAIL_RESPONSE) && (overallResult != BEP_FATALFAIL_STATUS) &&
           (overallResult != BEP_TIMEOUT_RESPONSE) && (overallResult != BEP_FAIL_RESPONSE) &&
           (overallResult != BEP_PASS_RESPONSE))
        {
            overallResult = BEP_TESTING_RESPONSE;
            Log(LOG_DEV_DATA, "GenericTC.cpp overallResult = BEP_TESTING_RESPONSE\n");
        }
    }
    else if((result != BEP_SKIP_RESPONSE) && (result != BEP_UNTESTED_RESPONSE))
    {   // if currently a pass, set to software fail to indicate a problem
        if(overallResult == BEP_PASS_RESPONSE)  overallResult = BEP_SOFTWAREFAIL_RESPONSE;
        Log(LOG_ERRORS, "Invalid test status %s, %s", result.c_str(), overallResult.c_str());
    }
    else if(overallResult.empty())
    {
        Log(LOG_ERRORS, "Overall not set so updating with: %s, %s", result.c_str(), overallResult.c_str());
        overallResult = result;
    }

    Log(LOG_FN_ENTRY, "GenericTC.cpp Test Result strings = result: %s, overall: %s\n", result.c_str(), overallResult.c_str());
}

//=============================================================================
void GenericTC::UpdateResult(INT32 result, std::string &overallResult)
{
    Log(LOG_FN_ENTRY, "GenericTC.cpp UpdateResult #3\n");

    std::string status;
    // Evaluate the test step status and update the objective status.
    switch(result)
    {
    case BEP_STATUS_ABORT:
        status = BEP_ABORT_RESPONSE;        break;
    case BEP_STATUS_ERROR:
        status = BEP_ERROR_RESPONSE;        break;
    case BEP_STATUS_HARDWARE:
        status = BEP_HARDWAREFAIL_RESPONSE; break;
    case BEP_STATUS_SOFTWARE:
        status = BEP_SOFTWAREFAIL_RESPONSE; break;
    case BEP_STATUS_TIMEOUT:
        status = BEP_TIMEOUT_RESPONSE;      break;
    case BEP_STATUS_FAILURE:
        status = BEP_FAIL_RESPONSE;         break;
    case BEP_STATUS_SUCCESS:
    case BEP_STATUS_NA:
    case BEP_STATUS_SKIP:
        status = BEP_PASS_RESPONSE;         break;
    default:
        status = BEP_INVALID_RESPONSE;      break;
        break;
    }

    Log(LOG_DEV_DATA, "GenericTC.cpp result = %d\n", result);
    // update the overall result now
    UpdateResult(status, overallResult);
}

//=============================================================================
void GenericTC::UpdateResult(INT32 result, INT32 &overallResult)
{
    Log(LOG_FN_ENTRY, "GenericTC.cpp UpdateResult #2\n");
    Log(LOG_FN_ENTRY, "GenericTC.cpp UpdateResult INT32: %d, %d\n", result, overallResult);

    std::string status;
    // Evaluate the test step status and update the objective status.
    if(result > overallResult)
    {
        overallResult = result;
        Log(LOG_DEV_DATA, "GenericTC.cpp overallResult = result\n");
    }
    else if(result == (INT32) BEP_STATUS_ERROR)
    {
        overallResult = BEP_STATUS_ERROR;
        Log(LOG_DEV_DATA, "overallResult = BEP_STATUS_ERROR\n");
    }
}

//=============================================================================
const std::string GenericTC::CommandObjective(const XmlNode *objective, const std::string &value)
{
    std::string status(BEP_UNTESTED_STATUS);            // status of the objective
    std::string testStepStatus(BEP_UNTESTED_STATUS);    // individual test step status
    std::string testStepName;                           // the name of the test step to sequence

    Log(LOG_FN_ENTRY, "%s CommandingObjective %s, TestSteps: %d\n", 
        GetComponentName().c_str(), objective->getName().c_str(), objective->getChildren().size());

    if(objective->getChildren().size() > 0)
    {
        // Execute each TestStep in the objective
        for(unsigned int ii=0; (ii < objective->getChildren().size()) && (GetStatus() != BEP_ABORT_STATUS); ++ii)
        {   // Get the next TestStep to sequence
            try
            {
                // this will override the value provided for the objective in
                // the drive curve with a value provided by the test step in 
                // the objective node. Due to the way test steps are specified 
                // in the objective node, and to maintain backward 
                // compatibility, this replacement is defined as an attribute
                // applied to the test step with the name "AltValue" 
                std::string altValue = value;  // set default
                try
                {
                    altValue = objective->getChildren().getNode(ii)->getAttribute("AltValue")->getValue();
                }
                catch(...)
                {
                    altValue = value;
                }
                if(altValue == "") altValue = value;

                // if no fatal failures
                if(GetFatalFailure() == false)
                {   // load the next TestStep
                    testStepName = objective->getChildren().getNode(ii)->getValue();
                    SetCurrentTestStep(m_testSteps.getNode(testStepName));

                    Log(LOG_DEV_DATA, "CommandTestStep: %s\n", testStepName.c_str());
                    SetStartTime();
                    // Execute the Commanded TestStep and update the result
                    testStepStatus = CommandTestStep (altValue);
                    SetTestStepResult(testStepStatus);
                    Log(LOG_DEV_DATA, "CommandTestStep: %s done: %s\n", testStepName.c_str(), testStepStatus.c_str());
                }
                // else if "FinishUp"
                else if(objective->getChildren().getNode(ii)->getValue() == "FinishUp")
                {
                    testStepName = objective->getChildren().getNode(ii)->getValue();
                    SetCurrentTestStep(m_testSteps.getNode(testStepName));
                    SetStartTime();
                    // Execute the Commanded TestStep and update the result
                    testStepStatus = CommandTestStep (altValue);
                    SetTestStepResult(testStepStatus);
                }
                // else done, skip the TestStep
                else
                    testStepStatus = testSkip;

                // update the overall result at the end of the objective
                SetOverallResult(testStepStatus);
            }
            catch(BepException &e)
            {
                testStepStatus = BEP_INVALID_RESPONSE;
                Log(LOG_DEV_DATA, "%s CommandObjective Error: %s, %s\n",
                    GetComponentName().c_str(), testStepName.c_str(), e.what());
            }

            // update the overall objective status
            UpdateResult (testStepStatus, status);
        }

        // if the test was aborted, update the stuat of the component on the GUI
        if((status == BEP_ABORT_RESPONSE) && (GetStatus() == BEP_ABORT_STATUS))
            SendOverallResult(status);
    }
    else if(GetFatalFailure() == true)
    {
        Log(LOG_ERRORS, "Fatal Failure In Component %s, Skipping Objective: %s\n",
            GetComponentName().c_str(), objective->getName().c_str());
    }
    else
        Log(LOG_ERRORS, "%s No TestSteps To Sequence In: %s\n", GetComponentName().c_str(), objective->getName().c_str());

    return(status);
}

//=============================================================================
const std::string GenericTC::CommandTestStep(const std::string &value)
{
    string status;
    try
    {   // Get the test step name to perform
        string step = GetTestStepName();
        Log(LOG_DEV_DATA, "GenericTC - %s.CommandTestStep(%s): Entering test step %s\n",
            GetComponentName().c_str(), value.c_str(), step.c_str());
        // Check the status to ensure it is alright to run this test step
        if(StatusCheck() != BEP_STATUS_SUCCESS)
        {   // Not OK to run the test step
            UpdateResult(StatusCheck(), status);
            Log(LOG_ERRORS,("GenericTC - %s.CommandTestStep: StatusCheck() failed: %s\n"),
                GetComponentName().c_str(),status.c_str());
            // Change result to skip since the test step will not be performed.
            status = testSkip;
            SendTestResult(testSkip, GetTestStepInfo("Description"));
        }
        // Prompt the operator to accelerate to the requested speed
        else if(step == "AccelerateToTestSpeed")
        {
            status = AccelerateToTestSpeed(GetParameterFloat(GetTestStepInfo("TargetSpeedParameter")),
                                           GetParameter(GetTestStepInfo("SpeedRangeParameter")),
                                           GetParameterInt(GetTestStepInfo("ScanDelayParameter")));
        }
        else if(step == "EnableMotorLoading")         status = EnableMotorLoading(value);
        else if(step == "DisableMotorLoading")        status = DisableMotorLoading(value);
        else if(step == "EnableElectricMotorBoost")   status = EnableElectricMotorBoost();
        else if(step == "DisableElectricMotorBoost")  status = DisableElectricMotorBoost();
        else if(step == "EngageMachine")              status = EngageMachine();
        else if(step == "DisengageMachine")           status = DisengageMachine();
        else if(step == "ResetDriveAxle")             status = ResetDriveAxle(value);
        else if(step == "ParkBrake")                  status = TestStepParkBrake(value);
        else if(step == "ParkPawl")                   status = TestStepParkPawl(value);
        else if(step == "ParkBrakePawl")              status = TestStepParkBrakePawl(value);
        else if(step == "HydraulicParkBrake")         status = TestStepHydraulicParkBrake(value);
        else if(step == "ElectricParkBrake")          status = TestStepElectricParkBrake(value);
        else if(step == "AnalyzeParkBrake")           status = AnalyzeParkBrakeTest(value);
        else if(step == "AnalyzeParkPawl")            status = AnalyzeParkPawlTest(value);
        else if(step == "MaintainSpeedForTime")       status = MaintainSpeedForTime(GetParameterInt("MaintainSpeedTime"),
                                                                                    GetParameterInt("MaintainSpeedMinSpeed"),
                                                                                    GetParameterInt("MaintainSpeedMaxSpeed"));
        else if(step == "ReadMemoryStatistics")       status = TestStepReadMemStats(value);
        else if(step == "EnableCenteringArrows")      status = EnableCenteringArrows();
        else if(step == "DisableCenteringArrows")     status = DisableCenteringArrows();
        // Unknown test step
        else
        {
            status = BEP_UNSUPPORTED_RESPONSE;
        }
    }
    catch(BepException &e)
    {
        Log(LOG_DEV_DATA, "GenericTC - %s CommandTestStep %s Exception: %s\n",
            GetComponentName().c_str(), GetTestStepName().c_str(), e.what());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }
    Log(LOG_DEV_DATA, "GenericTC - %s.CommandTestStep(%s): Returning %s\n",
        GetComponentName().c_str(), value.c_str(), status.c_str());
    return(status);
}

//=============================================================================
std::string GenericTC::GetCommand(std::string &command, std::string &value)
{
    std::string status = BEP_SUCCESS_RESPONSE;      // the status of the operation
    m_semCommand.CriticalSectionEnter();            // wait until a new command has been sequenced
    // if the command is blank, flag an error
    if(m_command == "")    status = BEP_FAIL_RESPONSE;
    command = m_command;    // set the current command
    value = m_value;        // set the current value
    // reset the command and value to indicate a new command can be received
    m_command = m_value = "";
    Log(LOG_DEV_DATA, "Command Received: %s, %s\n", command.c_str(), value.c_str());
    return(status);
}

//=============================================================================
std::string GenericTC::SetCommand(const std::string &command, const std::string &value)
{
    std::string status = BEP_FAIL_RESPONSE; // set the overall response to "Fail"

    try
    {
        m_semReady.CriticalSectionEnter();  // wait until component ready for new command
        m_command = command;                // set the command that is to be performed
        m_value = value;                    // set the value of the command to be performed
        m_semCommand.CriticalSectionExit(); // signal to the component "new command ready"

        // if the command is requesting the overall response
        Log(LOG_DEV_DATA, "Command Set: %s, %s\n", command.c_str(), value.c_str());
        status = GetObjectiveResult();      // return the result of the command
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s Exception in SetCommand: %s\n", GetProcessName().c_str(), e.what());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }

    return(status);
}

//=============================================================================
INT32 GenericTC::SendTestResultWithDetail(const std::string result, const std::string description, /* = "" */
                                          const std::string code, /* = "0000"*/
                                          const std::string firstDetail, /* = "" */
                                          const std::string firstDetailValue, /* = "" */
                                          const std::string firstDetailUnits, /* = "" */
                                          const std::string secondDetail, /* = "" */
                                          const std::string secondDetailValue, /* = "" */
                                          const std::string secondDetailUnits, /* = "" */
                                          const std::string thirdDetail, /* = "" */
                                          const std::string thirdDetailValue, /* = "" */
                                          const std::string thirdDetailUnits, /* = "" */
                                          const std::string fourthDetail /* = "" */ ,
                                          const std::string fourthDetailValue, /* = "" */
                                          const std::string fourthDetailUnits, /* = "" */
                                          const std::string fifthDetail, /* = "" */
                                          const std::string fifthDetailValue, /* = "" */
                                          const std::string fifthDetailUnits) /* = "" */
{
    INT32 overallResult = BEP_STATUS_ERROR; // the result of the operation
    std::string response;                   // the response of the command

    try
    {
        TestResultDetails resultDetails;
        if(firstDetail != "")
        {   // Write a test result with parameters
            resultDetails.AddDetail(firstDetail, firstDetailValue, firstDetailUnits);
        }
        if(secondDetail != "")
        {   // Write a test result with parameters
            resultDetails.AddDetail(secondDetail, secondDetailValue, secondDetailUnits);
        }
        if(thirdDetail != "")
        {   // Write a test result with parameters
            resultDetails.AddDetail(thirdDetail, thirdDetailValue, thirdDetailUnits);
        }
        if(fourthDetail != "")
        {   // Write a test result with parameters
            resultDetails.AddDetail(fourthDetail, fourthDetailValue, fourthDetailUnits);
        }
        if(fifthDetail != "")
        {   // Write a test result with parameters
            resultDetails.AddDetail(fifthDetail, fifthDetailValue, fifthDetailUnits);
        }
        // Send the results to the TestResultServer
        overallResult = SendTestResultWithDetail( result, resultDetails, description, code);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s SendTestResultWithDetail Error: %s\n", GetComponentName().c_str(), e.what());
    }

    return(overallResult);
}

//=============================================================================
INT32 GenericTC::SendTestResultWithDetail(const std::string result, const TestResultDetails &details,
                                          const std::string description, const std::string code)
{
    INT32 overallResult = BEP_STATUS_ERROR; // the result of the operation
    std::string response;                   // the response of the command

    try
    {
        std::string testResultName = CreateTestResultName();
        // Send the results to the TestResultServer
        overallResult = m_testResult->WriteTestResult(testResultName, result, GetComponentName(),
                                                      description, response,
                                                      details, true, code);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s SendTestResultWithDetail Error: %s\n", GetComponentName().c_str(), e.what());
    }

    return(overallResult);
}

//=============================================================================
INT32 GenericTC::SendSubtestResultWithDetail(const std::string &testName, const std::string result,
                                             const std::string description, /* = "" */
                                             const std::string code, /* = "0000"*/
                                             const std::string firstDetail,   /* = "" */
                                             const std::string firstDetailValue, /* = "" */
                                             const std::string firstDetailUnits, /* = "" */
                                             const std::string secondDetail, /* = "" */
                                             const std::string secondDetailValue, /* = "" */
                                             const std::string secondDetailUnits, /* = "" */
                                             const std::string thirdDetail,   /* = "" */
                                             const std::string thirdDetailValue, /* = "" */
                                             const std::string thirdDetailUnits, /* = "" */
                                             const std::string fourthDetail /* = "" */ ,
                                             const std::string fourthDetailValue, /* = "" */
                                             const std::string fourthDetailUnits, /* = "" */
                                             const std::string fifthDetail,   /* = "" */
                                             const std::string fifthDetailValue, /* = "" */
                                             const std::string fifthDetailUnits) /* = "" */
{
    INT32 overallResult = BEP_STATUS_ERROR; // the result of the operation
    std::string response;                   // the response of the command

    try
    {
        TestResultDetails resultDetails;
        if(firstDetail != "")
        {   // Write a test result with parameters
            resultDetails.AddDetail(firstDetail, firstDetailValue, firstDetailUnits);
        }
        if(secondDetail != "")
        {   // Write a test result with parameters
            resultDetails.AddDetail(secondDetail, secondDetailValue, secondDetailUnits);
        }
        if(thirdDetail != "")
        {   // Write a test result with parameters
            resultDetails.AddDetail(thirdDetail, thirdDetailValue, thirdDetailUnits);
        }
        if(fourthDetail != "")
        {   // Write a test result with parameters
            resultDetails.AddDetail(fourthDetail, fourthDetailValue, fourthDetailUnits);
        }
        if(fifthDetail != "")
        {   // Write a test result with parameters
            resultDetails.AddDetail(fifthDetail, fifthDetailValue, fifthDetailUnits);
        }
        overallResult = SendSubtestResultWithDetail( testName, result, resultDetails, description, code);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s SendSubtestResultWithDetail %s Error: %s\n", 
            GetComponentName().c_str(), testName.c_str(), e.what());
    }

    return(overallResult);
}

//=============================================================================
INT32 GenericTC::SendSubtestResultWithDetail(const std::string &testName, INT32 result,
                                             const std::string description, /* = "" */
                                             const std::string code, /* = "0000"*/
                                             const std::string firstDetail,   /* = "" */
                                             const std::string firstDetailValue, /* = "" */
                                             const std::string firstDetailUnits, /* = "" */
                                             const std::string secondDetail, /* = "" */
                                             const std::string secondDetailValue, /* = "" */
                                             const std::string secondDetailUnits, /* = "" */
                                             const std::string thirdDetail,   /* = "" */
                                             const std::string thirdDetailValue, /* = "" */
                                             const std::string thirdDetailUnits, /* = "" */
                                             const std::string fourthDetail /* = "" */ ,
                                             const std::string fourthDetailValue, /* = "" */
                                             const std::string fourthDetailUnits, /* = "" */
                                             const std::string fifthDetail,   /* = "" */
                                             const std::string fifthDetailValue, /* = "" */
                                             const std::string fifthDetailUnits) /* = "" */
{
    std::string localResult="";
    UpdateResult(result, localResult);

    return(SendSubtestResultWithDetail(testName, localResult, description, code,
                                       firstDetail, firstDetailValue, firstDetailUnits,
                                       secondDetail, secondDetailValue, secondDetailUnits ,
                                       thirdDetail, thirdDetailValue, thirdDetailUnits,
                                       fourthDetail, fourthDetailValue, fourthDetailUnits,
                                       fifthDetail, fifthDetailValue, fifthDetailUnits ));
}

//=============================================================================
INT32 GenericTC::SendSubtestResultWithDetail(const std::string &testName, const std::string result,
                                             const TestResultDetails &details, const std::string description,
                                             const std::string code)
{
    INT32 overallResult = BEP_STATUS_ERROR; // the result of the operation
    std::string response;                   // the response of the command

    try
    {
        std::string testResultName = CreateTestResultName(testName);
        overallResult = m_testResult->WriteTestResult(testResultName, result, GetComponentName(),
                                                      description, response,
                                                      details, true, code);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s SendSubtestResultWithDetail %s Error: %s\n", 
            GetComponentName().c_str(), testName.c_str(), e.what());
    }

    return(overallResult);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::Disengage(void)
{
    string tag, value, response;        // communication strings
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    // Log the entry
    Log(LOG_FN_ENTRY, "Disengaging System\n");
    try
    {   // command the motor mode to Torque Mode
        if(SystemCommand(MOTOR_MODE, string(TORQUE_MODE)) != BEP_STATUS_SUCCESS)
        {
            throw BepException("Error: Could Not Command %s, %s\n", MOTOR_MODE, TORQUE_MODE);
        }
        else
        {
            Log(LOG_DEV_DATA, "Motor Mode Is Torque Mode\n");
        }
        // command the motor to 0 Torque
        if(SystemCommand(COMMAND_TORQUE, 0) != BEP_STATUS_SUCCESS)
        {
            throw BepException("Error: Could Not Command %s, %s\n", COMMAND_TORQUE, "0");
        }
        else
        {
            Log(LOG_DEV_DATA, "Torque Commanded To 0\n");
        }
        // declutch the rollers
        if((ReadSubscribeData(MACHINE_CLUTCH) != "0") || (ReadSubscribeData(MACHINE_DECLUTCH) != "1"))
        {
            value = "0";    // disable clutch
            GetNamedDataBroker()->Write(MACHINE_CLUTCH, value, response, false);
            value = "1";    // enable declutch
            if(GetNamedDataBroker()->Write(MACHINE_DECLUTCH, value, response, true) != BEP_STATUS_SUCCESS)
                throw BepException("Could Not Declutch The Rollers\n");
            else
                Log(LOG_DEV_DATA, "Clutches disengaged\n");
        }
        // give time for the system to obtain the correct state (up to 10 seconds)
        int count = 100;
        bool disengaged = false;
        while((StatusCheck() == BEP_STATUS_SUCCESS) && (count-- > 0) && !disengaged)
        {
            disengaged = (ReadSubscribeData(MACHINE_CLUTCH) == "0") && (ReadSubscribeData(MACHINE_DECLUTCH) == "1");
            Log(LOG_DEV_DATA, "Waiting For System To Disengage\n");
            BposSleep(100);
        }
        Log(LOG_DEV_DATA, "System disengaged\n");
        if(count)   status = BEP_STATUS_SUCCESS;
        else             status = BEP_STATUS_FAILURE;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Disengage Exception: %s\n", e.what());
        status = BEP_STATUS_FAILURE;
    }
    catch(...)
    {
        Log(LOG_ERRORS, "Disengage Unknown Exception\n");
        status = BEP_STATUS_FAILURE;
    }
    // Return the status
    return(status);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::Engage(void)
{
    string tag, value, response;        // communication strings
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    // Log the entry
    Log(LOG_FN_ENTRY, "Engaging System\n");
    try
    {   // Clutch the rollers
        if((ReadSubscribeData(MACHINE_CLUTCH) != "1") || (ReadSubscribeData(MACHINE_DECLUTCH) != "0"))
        {   // make sure machine at Zerospeed
            if(CheckZeroSpeed())
            {   // then clutch
                value = "1";    // enable clutch
                GetNamedDataBroker()->Write(MACHINE_CLUTCH, value, response, false);
                value = "0";    // disable declutch
                if(GetNamedDataBroker()->Write(MACHINE_DECLUTCH, value, response, true) != BEP_STATUS_SUCCESS)
                    throw BepException("Could Not Declutch The Rollers\n");
            }
            else
            {
                Log(LOG_ERRORS, "Error, Can Not Engage, Not At Zerospeed\n");
            }
        }
        // command the motor mode to Boost Mode
        if(SystemCommand(MOTOR_MODE, string(BOOST_MODE)) != BEP_STATUS_SUCCESS)
            throw BepException("Error: Could Not Command %s, %s\n", 
                               MOTOR_MODE, BOOST_MODE);
        // set Boost to enable
        if(SystemWrite(BOOST_MODE, "1") != BEP_STATUS_SUCCESS)
            throw BepException("Error: Could Not Command %s, %s\n", BOOST_MODE, "1");
        // give time for the system to obtain the correct state
        int count = 20;
        bool systemOk = false;
        while((StatusCheck() == BEP_STATUS_SUCCESS) && (count-- > 0) && !systemOk)
        {
            systemOk = (SystemReadBool(MACHINE_CLUTCH) && !SystemReadBool(MACHINE_DECLUTCH) && 
                        !SystemRead(MOTOR_MODE).compare(BOOST_MODE));
            Log(LOG_DEV_DATA, "Waiting For System To Engage\n");
            BposSleep(100);
        }
        if(systemOk)    status = BEP_STATUS_SUCCESS;
        else            status = BEP_STATUS_FAILURE;

    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Engage Exception: %s\n", e.what());
        status = BEP_STATUS_FAILURE;
    }
    // return the status
    return(status);
}

//=============================================================================
INT32 GenericTC::ReportDTC(const std::string name, 
                           const std::string code, 
                           const std::string description,
                           string *moduleName /*= NULL*/)
{
    INT32 result = BEP_STATUS_ERROR;    // the result of the operation
    string modName((moduleName == NULL) ? GetComponentName() : *moduleName);
    // Determine if common module faults are being used
    if(GetParameterBool("UseCommonModuleFaultList"))
    {   // Read the common fault from the system
        XmlNode readFaultNode(COMMON_MODULE_FAULT_TAG, name);
        XmlNode faultNode("", "");
        Log(LOG_DEV_DATA, "Reading Common Fault from system: %s", name.c_str());
        SystemRead(&readFaultNode, faultNode);
        XmlNode *modFaultNode = const_cast<XmlNode *>(faultNode.getChildren().getNode(0));
        Log(LOG_DEV_DATA, "Common Fault: %s", modFaultNode->ToString().c_str());
        result = m_testResult->ReportDTC(modName, modFaultNode);
        Log(LOG_DEV_DATA, "Reported Fault: %s", ConvertStatusToResponse(result).c_str());
    }
    else
    {
        try
        {
            result = m_testResult->ReportDTC(modName, name, code, description);
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "%s ReportDTC %s Error: %s\n", GetComponentName().c_str(), name.c_str(), e.what());
        }
    }
    Log(LOG_DEV_DATA, "Done reporting DTCs: %s", ConvertStatusToResponse(result).c_str());
    return(result);
}

//=============================================================================
INT32 GenericTC::RemoveDTC(const std::string code) /* = CLEAR_ALL_DTC*/
{
    INT32 result = BEP_STATUS_ERROR;    // the result of the operation

    try
    {
        result = m_testResult->ClearDTC(GetComponentName(), code);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s RemoveDTC %s Error: %s\n", GetComponentName().c_str(), code.c_str(), e.what());
    }

    return(result);
}

//=============================================================================
const std::string GenericTC::SystemRead(const std::string &tag)
{
    std::string value, response;
    INamedDataBroker *ndb = GetNamedDataBroker();
    if(ndb != NULL)
    {
        INT32 status = ndb->Read(tag, response, true);
        if(status == BEP_STATUS_SUCCESS)
            status = ndb->GetByTag(tag, value, response);
        // check for errors
        if(status != BEP_STATUS_SUCCESS)
        {
            Log(LOG_ERRORS, "Error Reading: %s, %d\n", tag.c_str(), status);
            value = "ERROR";
        }
    }
    else
    {
        Log(LOG_ERRORS, "GenericTC::SystemRead() - INamedDataBroker object is NULL!");
        value = BEP_UNAVAILABLE_RESPONSE;
    }
    return(value);
}

//=============================================================================
void GenericTC::SystemRead(const XmlNode *data, XmlNode &systemData)
{
    string response;
    INamedDataBroker *ndb = GetNamedDataBroker();
    if(ndb != NULL)
    {
        INT32 status = ndb->Read(data, response, true);
        if(BEP_STATUS_SUCCESS == status)
        {
            status = ndb->GetNext(systemData, response);
        }
        else
        {
            Log(LOG_ERRORS, "Could not read %s,%s: %s", 
                data->getName().c_str(), data->getValue().c_str(), ConvertStatusToResponse(status).c_str());
        }
    }
}

//=============================================================================
const INT32 GenericTC::SystemReadInt(const std::string &tag)
{
    INT32 result;
    std::string value = SystemRead(tag);
    if(value == "ERROR")
        throw BepException("SystemReadInt Error %s, %s", tag.c_str(), value.c_str());

    result = BposReadInt(value.c_str());

    return(result);
}

//=============================================================================
const float GenericTC::SystemReadFloat(const std::string &tag)
{
    float result;
    std::string value = SystemRead(tag);
    if(value == "ERROR")
        throw BepException("SystemReadFloat Error %s, %s", tag.c_str(), value.c_str());

    result = atof(value.c_str());

    return(result);
}

//=============================================================================
const bool GenericTC::SystemReadBool(const std::string &tag)
{
    bool result;
    std::string value = SystemRead(tag);
    if(value == "ERROR")
        throw BepException("SystemReadBool Error %s, %s", tag.c_str(), value.c_str());

    if(value == "1")    result = true;
    else                result = false;

    return(result);
}

//=============================================================================
INT32 GenericTC::SystemWrite(const std::string &tag, const std::string &value)
{
    INT32 result = BEP_STATUS_ERROR;    // the result of the operation
    std::string response;

    try
    {
        result = GetNamedDataBroker()->Write(tag, value, response, true);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s SystemWrite %s Error: %s\n", GetComponentName().c_str(), tag.c_str(), e.what());
    }

    return(result);
}

//=============================================================================
INT32 GenericTC::SystemWrite(const std::string &tag, const int value)
{
    char buffer[128];
    const std::string newValue = CreateMessage(buffer, sizeof(buffer),"%d", value);
    return(SystemWrite(tag, newValue));
}

//=============================================================================
INT32 GenericTC::SystemWrite(const std::string &tag, const float value)
{
    char buffer[128];
    const std::string newValue = CreateMessage(buffer, sizeof(buffer),"%f", value);
    return(SystemWrite(tag, newValue));
}

//=============================================================================
INT32 GenericTC::SystemWrite(const std::string &tag, const bool value)
{
    char buffer[128];
    const std::string newValue = CreateMessage(buffer, sizeof(buffer),"%d", value);
    return(SystemWrite(tag, newValue));
}

//=============================================================================
INT32 GenericTC::SystemCommand(const std::string &tag, const std::string &value)
{
    INT32 result = BEP_STATUS_ERROR;    // the result of the operation
    std::string response;

    try
    {
        result = GetNamedDataBroker()->Command(tag, value, response, true);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s SystemCommand %s Error: %s\n", GetComponentName().c_str(), tag.c_str(), e.what());
    }

    return(result);
}

//=============================================================================
INT32 GenericTC::SystemCommand(const std::string &tag, const int value)
{
    char buffer[128];
    const std::string newValue = CreateMessage(buffer, sizeof(buffer),"%d", value);
    return(SystemCommand(tag, newValue));
}

//=============================================================================
INT32 GenericTC::SystemCommand(const std::string &tag, const float value)
{
    char buffer[128];
    const std::string newValue = CreateMessage(buffer, sizeof(buffer),"%f", value);
    return(SystemCommand(tag, newValue));
}

//=============================================================================
INT32 GenericTC::SystemCommand(const std::string &tag, const bool value)
{
    char buffer[128];
    const std::string newValue = CreateMessage(buffer, sizeof(buffer),"%d", value);
    return(SystemCommand(tag, newValue));
}

//=============================================================================
bool GenericTC::TimeRemaining(const INT32 *timeout) /* = NULL*/
{
    bool timeRemaining; // set up to return the value of the system
    double timeoutTime = -1;
    // Deterimine the timeout time to use
    if(timeout != NULL)
    {   // Use the specified timeout value
        timeoutTime = *timeout;
    }
    else if(GetTestStepInfo("Timeout") != "")
    {   // Use the default test step timeout
        timeoutTime = atol(GetTestStepInfo("Timeout").c_str());
    }
    else
    {
        timeoutTime = 0;
    }
    // if a timeout is set determine if there is time remaining
    if(timeoutTime > 0)
    {   // get the current time and the timeout time
        struct timespec currentTime;
        // Get the current time
        if(clock_gettime( CLOCK_REALTIME, &currentTime) == -1)
            throw BepException("TimeRemaining Clock Gettime Error: %s", strerror(errno));
        // determine the amount of time that has elapsed in milli seconds
        double elapsedTime = ( currentTime.tv_sec - m_startTime.tv_sec ) * 1000
                             + ( currentTime.tv_nsec - m_startTime.tv_nsec ) / 1000000;
        // Log the remaining time
        Log(LOG_DEV_DATA, "TimeRemaining: %f > %f\n", timeoutTime, elapsedTime);
        timeRemaining = timeoutTime > elapsedTime;
    }
    else    // if timeout not set then never time out
        timeRemaining = true;
    // Return the result
    return(timeRemaining);
}

//=============================================================================
bool GenericTC::SecondaryTimeRemaining(const INT32 *timeout) /* = NULL*/
{
    bool timeRemaining; // set up to return the value of the system
    // get the current time and the timeout time
    struct timespec currentTime;
    double timeoutTime = -1;
    // Deterimine the timeout time to use
    if(timeout != NULL)
    {   // Use the specified timeout value
        timeoutTime = *timeout;
    }
    else if(GetTestStepInfo("SecondaryTimeout") != "")
    {   // Use the default test step timeout
        timeoutTime = atol(GetTestStepInfo("SecondaryTimeout").c_str());
    }
    else
    {
        timeoutTime = 0;
    }
    // if a timeout is set determine if there is time remaining
    if(timeoutTime >= 0)
    {
        if(clock_gettime( CLOCK_REALTIME, &currentTime) == -1)
            throw BepException("SecondaryTimeRemaining Clock Gettime Error: %s", strerror(errno));

        // determine the amount of time that has elapsed in milli seconds
        double elapsedTime = ( currentTime.tv_sec - m_secondaryStartTime.tv_sec ) * 1000
                             + ( currentTime.tv_nsec - m_secondaryStartTime.tv_nsec ) / 1000000;

        Log(LOG_DEV_DATA, "SecondaryTimeRemaining: %f > %f\n", timeoutTime, elapsedTime);
        timeRemaining = timeoutTime > elapsedTime;
    }
    else    // if timeout not set then never time out
        timeRemaining = true;

    return(timeRemaining);
}

//=============================================================================
double GenericTC::GetElapsedTime(void) const
{
    double elapsedTime;

    // get the current time and the timeout time
    struct timespec currentTime;
    // Get the current time
    if(clock_gettime( CLOCK_REALTIME, &currentTime) == -1)
        throw BepException("TimeRemaining Clock Gettime Error: %s", strerror(errno));
    // determine the amount of time that has elapsed in milli seconds
    elapsedTime = ( currentTime.tv_sec - m_startTime.tv_sec ) * 1000
                  + ( currentTime.tv_nsec - m_startTime.tv_nsec ) / 1000000;

    return( elapsedTime);
}

//=============================================================================
const std::string GenericTC::GetSystemValue(const std::string &tag)
{
    std::string value, response;
    INT32 status = m_ndb->GetByTag(tag, value, response);
    if(status != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "Error Getting System Value: %s, %d\n", tag.c_str(), status);
        value = "ERROR";
    }

    return(value);
}

//=============================================================================
INT32 GenericTC::StatusCheck(void)
{
    Log(LOG_FN_ENTRY, "Entering GenericTC Status Check\n");
    // Get the current status
    INT32 testStatus = BEP_STATUS_SUCCESS;
    std::string status = GetStatus();
    // Check the status items if any were specified in the config file
    if(m_statusCheckItems.size() > 0)
    {   // Check all of the status items
        for(XmlNodeMapItr iter = m_statusCheckItems.begin(); iter != m_statusCheckItems.end(); iter++)
        {   // Read and verify the status item
            if(SystemRead(iter->second->getName()) != iter->second->getValue())
            {   // System status is not correct
                testStatus = BEP_STATUS_FAILURE;
                Log(LOG_DEV_DATA, "System status item %s not correct - %s [%s]\n", iter->second->getName().c_str(),
                    SystemRead(iter->second->getName()).c_str(), iter->second->getValue().c_str());
            }
        }
    }
    // Determine if the specified items were good
    if(BEP_STATUS_SUCCESS == testStatus)
    {   // if the test is aborted
        if(CheckAbort())
            testStatus = BEP_STATUS_ABORT;
        // else if running or initialized
        else if((status == BEP_RUNNING_STATUS) || (status == BEP_INITIALIZED_STATUS) ||
                (status == BEP_COMPLETE_STATUS) || (status == BEP_TESTING_STATUS))
            testStatus = BEP_STATUS_SUCCESS;
        else    // else there is another problem
            testStatus = BEP_STATUS_ERROR;
    }
    // Log the exit and return the status
    Log(LOG_FN_ENTRY, "%s, Component Status (%s) %d\n", GetComponentName().c_str(), status.c_str(), testStatus);
    return(testStatus);
}

//=============================================================================
bool GenericTC::CheckAbort(void)
{
    return(atob(SystemRead(ABORT_DATA_TAG).c_str()) || (GetStatus() == BEP_ABORT_STATUS));
}

//=============================================================================
bool GenericTC::ForegroundComplete(void)
{
    return(atob(ReadSubscribeData(GetDataTag("ForegroundTestInProgress")).c_str()));
}

//=============================================================================
const INT32 GenericTC::UpdatePrompts(void)
{   // if a prompt has been specified for the first prompt box
    std::string prompt;
    INT32 box, priority;
    INT32 status = BEP_STATUS_SUCCESS;          // the status of updating the prompts
    string valueOne, valueTwo, valueThree;      // prompt values
    string bgColor;                             // the background color to use

    try
    {   // get the prompt, if it exists
        prompt = GetTestStepInfo("Prompt1");
        if(prompt != "")
        {
            // get the priority
            try
            {
                priority = GetTestStepInfoInt("Prompt1Priority");
            }
            catch(...)
            {
                priority = 0;
            }

            try
            {   // get the background color
                bgColor = GetTestStepInfo("Prompt1BGColor");
            }
            catch(...)
            {
                bgColor = "white";
            }

            // if a value was added
            valueOne = GetTestStepInfo("Prompt1Value1");
            if(!valueOne.empty())
            {
                valueTwo = GetTestStepInfo("Prompt1Value2");
                if(!valueTwo.empty())
                {
                    valueThree = GetTestStepInfo("Prompt1Value3");
                }
            }

            // display the prompt
            box = 1;
            Log(LOG_DEV_DATA, "1 Prompting Driver To %s, %d\n", prompt.c_str(), priority);
            int retry = 3;
            do
            {
                status = DisplayPrompt(box , prompt, priority, bgColor, valueOne, valueTwo, valueThree);
            } while(retry-- && (status != BEP_STATUS_SUCCESS));

            if(status != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Unable to DisplayPrompt(1, %s, %d)\n", prompt.c_str(), priority);
        }
    }
    catch(...)
    {
    }

    // if a prompt has been specified for the second prompt box
    try
    {
        prompt = GetTestStepInfo("Prompt2");
        if(prompt != "")
        {
            // get the priority
            try
            {
                priority = GetTestStepInfoInt("Prompt2Priority");
            }
            catch(...)
            {
                priority = 0;
            }

            try
            {   // get the background color
                bgColor = GetTestStepInfo("Prompt2BGColor");
            }
            catch(...)
            {
                bgColor = "white";
            }

            // if a value was added
            valueOne = GetTestStepInfo("Prompt2Value1");
            if(!valueOne.empty())
            {
                valueTwo = GetTestStepInfo("Prompt2Value2");
                if(!valueTwo.empty())
                {
                    valueThree = GetTestStepInfo("Prompt2Value3");
                }
            }

            // display the prompt
            box = 2;
            Log(LOG_DEV_DATA, "2 Prompting Driver To %s, %d\n", prompt.c_str(), priority);
            int retry = 3;
            do
            {
                status = DisplayPrompt(box , prompt, priority, bgColor, valueOne, valueTwo, valueThree);
            } while(retry-- && (status != BEP_STATUS_SUCCESS));

            if(status != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Unable to DisplayPrompt(2, %s, %d)\n", prompt.c_str(), priority);
        }
    }
    catch(...)
    {
    }

    return(status);
}

//=============================================================================
void GenericTC::RemovePrompts(void)
{   // if a prompt has been specified for the first prompt box
    std::string prompt;
    INT32 box, priority;

    try
    {   // get the prompt, if it exists
        prompt = GetTestStepInfo("Prompt1");
        if(prompt != "")
        {
            // get the priority
            try
            {
                priority = GetTestStepInfoInt("Prompt1Priority");
            }
            catch(...)
            {
                priority = 0;
            }
            box = 1;
            Log(LOG_DEV_DATA, "1 Removing Prompt %s, %d\n", prompt.c_str(), priority);
            RemovePrompt(box , prompt, priority);
        }
    }
    catch(...)
    {
        Log(LOG_ERRORS, "EXCEPTION Error Caught in GenericTC::RemovePrompts Trying to Remove Prompt 1\n");
    }

    // if a prompt has been specified for the second prompt box
    try
    {
        prompt = GetTestStepInfo("Prompt2");
        if(prompt != "")
        {
            // get the priority
            try
            {
                priority = GetTestStepInfoInt("Prompt2Priority");
            }
            catch(...)
            {
                priority = 0;
            }
            box = 2;
            Log(LOG_DEV_DATA, "2 Removing Prompt %s, %d\n", prompt.c_str(), priority);
            RemovePrompt(box , prompt, priority);
        }
    }
    catch(...)
    {
        Log(LOG_ERRORS, "EXCEPTION Error Caught in GenericTC::RemovePrompts Trying to Remove Prompt 2\n");
    }
}

//=============================================================================
std::string GenericTC::GetFaultName(const std::string fault)
{
    std::string name;
    try
    {
        name = m_faults.getNode(fault)->getValue();
    }
    catch(...)
    {
        char buffer[128];
        name = CreateMessage(buffer, sizeof(buffer),"%s Name Unavailable", fault.c_str());
    }

    return(name);
}

//=============================================================================
std::string GenericTC::GetFaultCode(const std::string fault)
{
    std::string code;
    try
    {
        code = m_faults.getNode(fault)->getAttributes().getNode("Code")->getValue();
    }
    catch(...)
    {
        char buffer[128];
        code = CreateMessage(buffer, sizeof(buffer),"%s Code Unavailable", fault.c_str());
    }

    return(code);
}

//=============================================================================
std::string GenericTC::GetFaultDescription(const std::string fault)
{
    std::string description;
    try
    {
        description = m_faults.getNode(fault)->getAttributes().getNode("Description")->getValue();
    }
    catch(...)
    {
        char buffer[128];
        description = CreateMessage(buffer, sizeof(buffer),"%s Description Unavailable", fault.c_str());
    }

    return(description);
}

//=============================================================================
std::string GenericTC::GetFaultFailureStatusMask(const std::string fault)
{
    std::string failureStatusMask;
    try
    {
        failureStatusMask = m_faults.getNode(fault)->getAttributes().getNode("FailureStatusMask")->getValue();
    }
    catch(...)
    {
        char buffer[128];
        failureStatusMask = CreateMessage(buffer, sizeof(buffer),"%s Description Unavailable", fault.c_str());
    }

    return(failureStatusMask);
}

//=============================================================================
INT32 GenericTC::Clutch(bool type)
{
    INT32 status = BEP_STATUS_FAILURE;
    std::string response;

    // command the motor controller to clutch the system
    status = GetNamedDataBroker()->Command(COMMAND_CLUTCH, type ? "1":"0", response, true);
    if(status != BEP_STATUS_SUCCESS)
        Log(LOG_ERRORS, "Unable To Command MotorController to Clutch: %d\n", status);

    return(status);
}

//=============================================================================
void GenericTC::ResetSystemTags(void)
{
    // if an update needs to be performed
    Log(LOG_FN_ENTRY, "%s Setting the system tags\n", GetComponentName().c_str());
    if(m_systemTags.size())
    {
        try
        {
            // create an interface that can handle all of the tags that need to be sent
            INamedDataBroker ndb(IsDebugOn(), (m_systemTags.size() * 200));
            std::string response;
            bool listEnd;
            for(UINT32 ii = 0; ii < m_systemTags.size(); ii++)
            {
                listEnd = (ii == m_systemTags.size() - 1);
                Log(LOG_DEV_DATA, "Sending %d, %s: %s, end: %d\n", ii, m_systemTags.getNode(ii)->getName().c_str(),
                    m_systemTags.getNode(ii)->getValue().c_str(), listEnd);
                if(ndb.Write(m_systemTags.getNode(ii)->getName(), m_systemTags.getNode(ii)->getValue(),
                             response, true) != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_ERRORS, "Error writing the System Tag: %s, %s\n",
                        m_systemTags.getNode(ii)->getName().c_str(), m_systemTags.getNode(ii)->getValue().c_str());
                }
            }
            Log(LOG_DEV_DATA, "%s System Tags Updated\n", GetComponentName().c_str());
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "There Was An Exceptional Condition in %s::ResetSystemTags: %s\n",
                GetComponentName().c_str(), e.what());
        }
    }
}

//=============================================================================
void GenericTC::UpdateTestStatus(const std::string status)
{
    // create the description for the test result
    std::string buffer = GetComponentName() + " Overall Test Result";
    std::string response;
    // send the result
    m_testResult->WriteTestResult(CreateTestResultName("OverallTest"), status, GetComponentName(), buffer, response, true);
    Log(LOG_DEV_DATA, "TestResult Sending: %s, %s, %s, %s, %s, %d\n", 
        CreateTestResultName("OverallTest").c_str(), status.c_str(),
        GetComponentName().c_str(), buffer.c_str(), response.c_str(), true);
}

//=============================================================================
void GenericTC::SendOverallResult(const std::string result) /*= BEP_PASS_RESPONSE*/
{
    try
    {
        Log(LOG_DEV_DATA, "Attempting to SetOverallResult(%s)\n", result.c_str());
        SetOverallResult(result);               // update the overall result
        Log(LOG_DEV_DATA, "Setting status to %s\n", BEP_COMPLETE_STATUS);
        SetStatus(BEP_COMPLETE_STATUS);         // set the status to complete
        Log(LOG_DEV_DATA, "Setting retest status to TRUE\n");
        SetRetestStatus(true);                  // since overall called for, any more testing is a retest
        Log(LOG_DEV_DATA, "Calling SetPreviousOverallResult(%s)\n", GetOverallResult().c_str());
        SetPreviousOverallResult(GetOverallResult());   // update previous for checking for retest
        Log(LOG_DEV_DATA, "Calling UpdateTestStatus(%s)\n", GetOverallResult().c_str());
        UpdateTestStatus(GetOverallResult());   // write the overall test result to the test result server
        Log(LOG_FN_ENTRY, "Sending overall result: %s\n", GetOverallResult().c_str());
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Error in SendOverallResult: %s\n", e.what());
    }
}

//=============================================================================
void GenericTC::Reset(void)
{
    Log(LOG_FN_ENTRY, "Resetting Component %s\n", GetComponentName().c_str());
    ResetOverallResult();           // reset the overall result
}

//=============================================================================
void GenericTC::Abort(void)
{
    // if the test is not a previous pass, abort the test
    if((GetStatus() != BEP_COMPLETE_STATUS) && (GetPreviousOverallResult() != testPass))
        SendOverallResult(testAbort);
    SetStatus(BEP_ABORT_STATUS);
}

//=============================================================================
INT32 GenericTC::GetPromptBox(const string &promptTag)
{
    INT32 promptBox = 0;
    // Attempt to get the prompt box to use
    try
    {
        promptBox = atoi(m_prompts.getNode(promptTag)->getAttribute("PromptBox")->getValue().c_str());
    }
    catch(XmlException &e)
    {       // Error getting prompt box for requested prompt
        Log(LOG_ERRORS, "Error getting prompt box for prompt %s, using prompt box 1: %s\n",
            promptTag.c_str(), e.GetReason());
        promptBox = 1;
    }
    // Return the prompt box
    return(promptBox);
}

//=============================================================================
string GenericTC::GetPrompt(const string &promptTag)
{
    string prompt = "Unknown";
    // Attempt to get the prompt tag to display
    try
    {
        prompt = m_prompts.getNode(promptTag)->getValue();
    }
    catch(XmlException &e)
    {       // Error getting prompt tag for display
        Log(LOG_ERRORS, "Error getting prompt tag for prompt %s: %s\n", promptTag.c_str(), e.GetReason());
        prompt = promptTag;
    }
    // Return the prompt to display
    return(prompt);
}

//=============================================================================
INT32 GenericTC::GetPromptPriority(const string &promptTag)
{
    INT32 priority = 0;
    // Attempt to get the prompt box to use
    try
    {
        priority = atoi(m_prompts.getNode(promptTag)->getAttribute(BEP_PRIORITY)->getValue().c_str());
    }
    catch(XmlException &e)
    {       // Error getting prompt box for requested prompt
        Log(LOG_ERRORS, "Error getting priority for prompt %s, using priority 0: %s\n",
            promptTag.c_str(), e.GetReason());
        priority = 0;
    }
    // Return the prompt priority
    return(priority);
}

//=============================================================================
INT32 GenericTC::GetPromptDuration(const string &promptTag)
{
    INT32 duration = 0;
    // Attempt to get display time for timed prompts
    try
    {
        duration = atoi(m_prompts.getNode(promptTag)->getAttribute("Duration")->getValue().c_str());
    }
    catch(XmlException &e)
    {       // Error getting prompt box for requested prompt
        Log(LOG_ERRORS, "Error getting Duration for prompt %s, using priority 0: %s\n",
            promptTag.c_str(), e.GetReason());
        duration = 0;
    }
    // Return the display time
    return(duration);
}

//=============================================================================
INT32 GenericTC::TagArray(const string arrayTag)
{
    INT32 sampleNumber=0, status = BEP_STATUS_ERROR;
    UINT8 attempt = 2;
    string response, indexTag, indexValue;
    // Attempt to tag the data array
    do 
    {   // Tell the ICM manager to tag the array
        status = m_icmMgrComm.Command(GetDataTag("TagIndex"), arrayTag, response, true);
        // Check the status of the command
        if(BEP_STATUS_SUCCESS == status)
        {       // Extract the data from the response
            m_icmMgrComm.GetNext(indexTag, indexValue, response);
            // Convert the string value to a number
            sampleNumber = atoi(indexValue.c_str());
        }
        else
        {       // Error tagging the array
            Log(LOG_ERRORS, "Error tagging the data array - status: %s, attempt: %d", ConvertStatusToResponse(status).c_str(), attempt);
        }
    } while((attempt-- > 0) && (BEP_STATUS_SUCCESS != status));
    // If not a good status, throw the exception
    if(BEP_STATUS_SUCCESS != status)
    {
        string message("Could Not Tag Array with: ");
        message += arrayTag;
        throw BepException(message);
    }
    // return the value
    return(sampleNumber);
}

//=============================================================================
INT32 GenericTC::ReadDataArrays(const string &array, int startIdx, int endIdx, WHEELDATAARRAY &dataArray)
{
    INT32 status = BEP_STATUS_SUCCESS;          // start with success
    UINT32 rollerCount = GetRollerCount();      // get the number of machine roller

    // clear out the dataArray from the previously sampled data
    dataArray.clear();

    // create force arrays for each roller
    DATAARRAY wheel[rollerCount];

    Log(LOG_FN_ENTRY, "ReadDataArray(%s, %d, %d)\n", array.c_str(), startIdx, endIdx);

    // Open the force array pseudo-file
    int arrayFd;        // array file descriptor
    if((arrayFd = open(array.c_str(), O_RDONLY)) != -1)
    {
        Log(LOG_DEV_DATA, "File Open: %s\n", array.c_str());
        ssize_t bytes;      // the number of bytes read
        // Move to the first relevant sample
        UINT32 seekPoint = (rollerCount * startIdx) * sizeof(float);

        struct stat arrayStat;
        memset( &arrayStat, 0, sizeof( arrayStat));
        fstat( arrayFd, &arrayStat);
        Log( LOG_DEV_DATA, "Seek Point: %d < Array size: %d bytes\n", seekPoint, arrayStat.st_size);
        if( (seekPoint != 0) && (seekPoint < (UINT32)arrayStat.st_size))
        {
            Log(LOG_DEV_DATA, "Seeking To: %d\n", seekPoint);
            // if seek successful read out the all of the information and the process it into the correct arrays
            if(lseek(arrayFd, seekPoint, SEEK_SET) != -1)
            {
                // determine how many samples to read
                int samples = endIdx - startIdx;
                // create a wheel information array to store the values
                WHEELINFO wheelInfoArray[samples+1];
                Log(LOG_DEV_DATA, "Reading %d Samples\n", samples);
                if(4 < GetRollerCount())
                {
                    bytes = read(arrayFd, (void *) wheelInfoArray, (sizeof(float) * GetRollerCount() * samples));
                }
                else
                {
                    WHEELINFO2AXLE wheelInfoArray2Axle[samples+1];
                    bytes = read(arrayFd, (void *) wheelInfoArray2Axle, (sizeof(float) * GetRollerCount() * samples));
                    for(int x = 0; x < samples+1; x++)
                    {
                        wheelInfoArray[x].lfWheel = wheelInfoArray2Axle[x].lfWheel;
                        wheelInfoArray[x].rfWheel = wheelInfoArray2Axle[x].rfWheel;
                        wheelInfoArray[x].lrWheel = wheelInfoArray2Axle[x].lrWheel;
                        wheelInfoArray[x].rrWheel = wheelInfoArray2Axle[x].rrWheel;
                    }
                }
                if(bytes == (ssize_t)(sizeof(float) * GetRollerCount() * samples))
                {
                    for(int index=0;(index < samples) && (status == BEP_STATUS_SUCCESS); index++)
                    {       // read in each roll respectively
                        Log(LOG_DETAILED_DATA, "Walking Through The Sample: %d, %.2f, %.2f, %.2f, %.2f\n", index,
                            wheelInfoArray[index].lfWheel, wheelInfoArray[index].rfWheel,
                            wheelInfoArray[index].lrWheel, wheelInfoArray[index].rrWheel);
                        wheel[LFWHEEL].push_back(wheelInfoArray[index].lfWheel);
                        wheel[RFWHEEL].push_back(wheelInfoArray[index].rfWheel);
                        wheel[LRWHEEL].push_back(wheelInfoArray[index].lrWheel);
                        wheel[RRWHEEL].push_back(wheelInfoArray[index].rrWheel);
                        if(4 < GetRollerCount())
                        {
                            wheel[LTWHEEL].push_back(wheelInfoArray[index].ltWheel);
                            wheel[RTWHEEL].push_back(wheelInfoArray[index].rtWheel);
                            Log(LOG_DETAILED_DATA, ", lt: %.2f, rt: %.2f", 
                                wheelInfoArray[index].ltWheel, wheelInfoArray[index].rtWheel);
                        }
                    }
                }
                else
                {
                    Log(LOG_ERRORS, "Error Reading The File Information bytes: %d, size of roller info: %d, number of rollers: %d, %s\n", bytes, ((ssize_t)(sizeof(float)) * rollerCount * samples), rollerCount, strerror(errno));
                    status = BEP_STATUS_FAILURE;
                }
                Log(LOG_DEV_DATA, "Reading The Samples complete\n");
            }
            else
            {
                Log(LOG_ERRORS, "Could Not Seek To: %d\n", seekPoint);
                status = BEP_STATUS_FAILURE;
            }

        }
        else if( seekPoint >= (UINT32)arrayStat.st_size)
        {
            Log( LOG_ERRORS, "ERROR: attempt to seek past end of array %s\n", array.c_str());
            status = BEP_STATUS_FAILURE;
        }
        else
        {
            Log( LOG_ERRORS, "ERROR: attempt to read 0 bytes from array %s\n", array.c_str());
            status = BEP_STATUS_FAILURE;
        }
        // Close the array file
        close(arrayFd);

        // if a successful load
        if(status != BEP_STATUS_FAILURE)
        {       // store the wheel information
            for(int index = 0; index < m_rollerCount; index++)
                dataArray.push_back(wheel[index]);
            status = BEP_STATUS_SUCCESS;        // set the status to success
        }
    }
    else
    {
        Log(LOG_ERRORS, "Could Not Open: %s: %s\n", array.c_str(), strerror(errno));
        status = BEP_STATUS_SOFTWARE;
    }
    return(status);
}

//=============================================================================
const UINT32 &GenericTC::GetRollerCount(void)
{
    return(m_rollerCount);
}

//=============================================================================
void GenericTC::SetRollerCount(UINT32 rollerCount)
{
    if(rollerCount >= 2)
        m_rollerCount = rollerCount;
    else
        Log(LOG_ERRORS, "Attempting To Set Invalid Roller Count: %d\n", rollerCount);
}

//=============================================================================
const UINT32 GenericTC::GetWheelCount(void)
{   // DCBR
    const string junk( ReadSubscribeData( GetDataTag( "VehicleAxleCount")));
    INT32   axleCount;
    UINT32  wheelCount = 0;

    axleCount = atoi(junk.c_str());
    if( axleCount <= 0) axleCount = 2;
    wheelCount = 2*axleCount;
    return( wheelCount);
}

//=============================================================================
const string &GenericTC::GetICMManagerName(void)
{
    return(m_icmManagerName);
}

//=============================================================================
string GenericTC::DisengageMachine(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    // Log the entry
    Log(LOG_FN_ENTRY, "GenericTC::DisengageMachine() - Enter\n");
    // Disengage the machine
    testResult = BEP_STATUS_SUCCESS == Disengage() ? testPass : testAbort;
    // Determine the reporting items
    testResultCode = testPass == testResult ? testResultCode : GetFaultCode("FailedToDisengage");
    testDescription = testPass == testDescription ? testDescription : GetFaultDescription("FailedToDisengage");
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "GenericTC::DisengageMachine() - Exit\n");
    return(testResult);
}

//=============================================================================
string GenericTC::DisableElectricMotorBoost(void)
{
    string testResult(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "GenericTC::DisableElectricMotorBoost() - Enter");
    // Ensure this is an electric machine
    if(!SystemRead(GetDataTag("MachineType")).compare(GetDataTag("ElectricMachineType")))
    {   // Set the motors to Torque 0 mode
        try
        {   // command the motor mode to Torque Mode
            if(SystemCommand(MOTOR_MODE, string(TORQUE_MODE)) != BEP_STATUS_SUCCESS)
            {
                throw BepException("Error: Could Not Command %s, %s", 
                                   MOTOR_MODE, TORQUE_MODE);
            }
            else
            {
                Log(LOG_DEV_DATA, "Motor Mode Is Torque Mode");
            }
            // command the motor to 0 Torque
            if(SystemCommand(COMMAND_TORQUE, 0) != BEP_STATUS_SUCCESS)
            {
                throw BepException("Error: Could Not Command %s, 0", COMMAND_TORQUE);
            }
            else
            {
                Log(LOG_DEV_DATA, "Torque Commanded To 0");
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "Disengage Exception: %s", e.what());
            testResult = testFail;
        }
        catch(...)
        {
            Log(LOG_ERRORS, "Disengage Unknown Exception");
            testResult = testFail;
        }
        // Determine the test result
        testResult = !testResult.compare(BEP_TESTING_STATUS) ? testPass : testFail;
    }
    else
    {   // This is not an electric machine, call disengage machine instead
        testResult = DisengageMachine();
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "GenericTC::DisableElectricMotorBoost() - Exit");
    return testResult;
}

//=============================================================================
string GenericTC::EngageMachine(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    // Log the entry
    Log(LOG_FN_ENTRY, "GenericTC::EngageMachine() - Enter\n");
    // Need to wait for zero speed
    while(!CheckZeroSpeed() && TimeRemaining())  BposSleep(150);
    // Engage the machine
    testResult = BEP_STATUS_SUCCESS == Engage() ? testPass : testAbort;
    // Determine the reporting items
    testResultCode = testPass == testResult ? testResultCode : GetFaultCode("FailedToEngage");
    testDescription = testPass == testDescription ? testDescription : GetFaultDescription("FailedToEngage");
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "GenericTC::EngageMachine() - Exit\n");
    return(testResult);
}

//=============================================================================
string GenericTC::ResetDriveAxle(const std::string &value)
{
    string testResult = BEP_PASS_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    // Log the entry
    Log(LOG_FN_ENTRY, "GenericTC::ResetDriveAxle() - Enter\n");
    // Reset the drive axle
    if(!value.compare("Front") || !value.compare("Rear"))
        testResult = (BEP_STATUS_SUCCESS == SystemWrite(GetDataTag("DriveAxle"), value)) ? testPass : testSoftwareFail;
    else
        Log(LOG_DEV_DATA, "Invalid Drive Axle value - %s. Leaving Drive Axle value unchanged.",value.c_str());
    // Determine the reporting items
    testResultCode = testPass == testResult ? testResultCode : GetFaultCode("DriveAxleFailedToSet");
    testDescription = testPass == testDescription ? testDescription : GetFaultDescription("DriveAxleFailedToSet");
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "GenericTC::ResetDriveAxle() - Exit\n");
    return(testResult);
}

//=============================================================================
string GenericTC::EnableCenteringArrows(void)
{
    string testResult = BEP_PASS_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    // Log the entry
    Log(LOG_FN_ENTRY, "GenericTC::EnableCenteringArrows() - Enter\n");
    // Enable Centering Arrows
    SystemWrite(GetDataTag("EnableCenteringArrows"),true);
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "GenericTC::EnableCenteringArrows() - Exit\n");
    return(testResult);
}

//=============================================================================
string GenericTC::DisableCenteringArrows(void)
{
    string testResult = BEP_PASS_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    // Log the entry
    Log(LOG_FN_ENTRY, "GenericTC::DisableCenteringArrows() - Enter\n");
    // Enable Centering Arrows
    SystemWrite(GetDataTag("EnableCenteringArrows"),false);
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "GenericTC::DisableCenteringArrows() - Exit\n");
    return(testResult);
}

//=============================================================================
string GenericTC::EnableElectricMotorBoost(void)
{
    string testResult(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "GenericTC::EnableElectricMotorBoost() - Enter");
    // If this is not an electric machine, call engage machine
    if(!SystemRead(GetDataTag("MachineType")).compare(GetDataTag("ElectricMachineType")))
    {   // Set the motors to boost mode
        try
        {   // command the motor mode to Boost Mode
            if(SystemCommand(MOTOR_MODE, string(BOOST_MODE)) != BEP_STATUS_SUCCESS)
                throw BepException("Error: Could Not Command %s, %s", 
                                   MOTOR_MODE, BOOST_MODE);
            // set Boost to enable
            if(SystemWrite(BOOST_MODE, "1") != BEP_STATUS_SUCCESS)
                throw BepException("Error: Could Not Command %s, 1", BOOST_MODE);
            // Enable boost successful
            testResult = testPass;
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "Enable Motor Boost Exception: %s", e.what());
            testResult = testFail;
        }
    }
    else
    {
        testResult = EngageMachine();
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "GenericTC::EnableElectricMotorBoost() - Exit");
    return testResult;
}

//=============================================================================
bool GenericTC::ShortCircuitTestStep(void)
{
    bool skip = false;
    // Log the function entry
    Log(LOG_FN_ENTRY, "ShortCircuitTestStep: OverallResult == %s, Retest == %d, "
        "PreviousOverallResult == %s, TestStepResult ==%s\n", GetOverallResult().c_str(), IsRetest(),
        GetPreviousOverallResult().c_str(), GetTestStepResult().c_str());
    // test completely passed
    if(IsRetest() && GetPreviousOverallResult() == testPass)
    {   // Skip this test step
        skip = true;
    }
    // return the result
    Log(LOG_FN_ENTRY, "ShortCircuitTestStep() returning %s\n", skip ? "Skip" : "Perform");
    return(skip);
}

//=============================================================================
const string GenericTC::SlowDownToTestSpeed(const float &targetSpeed, const string &speedRange, const INT32 &scanDelay,
                                            const bool &reportResults, /* = true */
                                            string promptName) /* = "SlowDownToTargetSpeed" */
{
    string testResult = testFail;
    string testDescription = GetTestStepInfo("Description");
    string testResultCode = "0000";
    string eraseSpeedRange = "0 0";
    // Log the function entry
    Log(LOG_FN_ENTRY, "GenericTC::SlowToTestSpeed() Enter\n");
    // Determine if this test step should be performed
    if(ShortCircuitTestStep())
    {
        testResult = testSkip;
    }
    else
    {
        BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
        // Display Prompt
        DisplayPrompt(GetPromptBox(promptName),GetPrompt(promptName),GetPromptPriority(promptName));
        // update the target green bands
        SystemWrite(GetDataTag("SpeedTarget"), speedRange);
        //ensure proper speed range before continuing
        status = WaitForTargetSpeed(targetSpeed, DOWN, scanDelay);
        // remove the target green bands
        SystemWrite(GetDataTag("SpeedTarget"), eraseSpeedRange);
        // remove the prompts
        RemovePrompt(GetPromptBox(promptName),GetPrompt(promptName),GetPromptPriority(promptName));
        // Convert the status into a test result
        testResult = status == BEP_STATUS_SUCCESS ? testPass : ConvertStatusToResponse(status);
        if(reportResults)
        {   // report the result
            SendTestResult(testResult,testDescription,testResultCode);
        }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "GenericTC::SlowDownToTestSpeed() Exit: %s\n", testResult.c_str());
    // Return the results
    return(testResult);
}

//=============================================================================
const string GenericTC::AccelerateToTestSpeed(const float &targetSpeed, const string &speedRange, const INT32 &scanDelay,
                                              const bool &reportResults, /* = true */
                                              string promptName, /* = "AccelerateToTargetSpeed" */
                                              const bool &waitForBoostedAxle) /* = false*/
{
    string testResult = testFail;
    string testDescription = GetTestStepInfo("Description");
    string testResultCode = "0000";
    string eraseSpeedRange = "0 0";
    bool   promptsDisplayed = false;
    // Log the function entry
    Log(LOG_FN_ENTRY, "GenericTC::AccelerateToTestSpeed()\n");
    // Determine if this test step should be performed
    if(ShortCircuitTestStep())
    {
        testResult = testSkip;
    }
    else
    {
        BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
        // Only update prompts if vehicle is not already in speed range - could cause flicker
        if(GetRollSpeed() < targetSpeed)
        {   // Display Prompts
            INT32   promptBox = GetPromptBox(promptName);
            const std::string promptTag(GetPrompt(promptName));
            INT32   promptPriority = GetPromptPriority(promptName);
            std::string promptVal;

            if( promptBox == 1)     promptVal = GetTestStepInfo( "Prompt1Value1");
            else                     promptVal = GetTestStepInfo( "Prompt2Value1");

            // Display Prompts
            DisplayPrompt( promptBox, promptTag, promptPriority, "", promptVal);
            // update the target green bands
            SystemWrite(GetDataTag("SpeedTarget"), speedRange);
            promptsDisplayed = true;
        }
        //ensure proper speed range before continuing
        status = WaitForTargetSpeed(targetSpeed, UP, scanDelay, waitForBoostedAxle);
        // Check if prompts need to be removed
        if(promptsDisplayed)
        {   // remove the target green bands
            SystemWrite(GetDataTag("SpeedTarget"), eraseSpeedRange);
            // remove the prompts
            RemovePrompt(GetPromptBox(promptName),GetPrompt(promptName),GetPromptPriority(promptName));
        }
        // Convert the status into a test result
        testResult = status == BEP_STATUS_SUCCESS ? testPass : ConvertStatusToResponse(status);
        if(reportResults)
        {   // report the result
            SendTestResult(testResult,testDescription,testResultCode);
        }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "GenericTC::AccelerateToTestSpeed() done: %s\n", testResult.c_str());
    // Return the results
    return(testResult);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::WaitForTargetSpeed(const float targetSpeed, Direction direction, 
                                              UINT32 wait, /*= 100 */ 
                                              const bool &waitForBoostedAxle) /* = false */
{
    BEP_STATUS_TYPE rtnStat = BEP_STATUS_ERROR;     // return status of the test
    bool proceed = false;               // flag to indicate that conditions have been met
    float currentSpeed;                 // the current speed of the vehicle (driven axle)
    float secondaryCurrentSpeed;        // the current speed of the non-driven axle
    // Log the function entry
    Log(LOG_FN_ENTRY, "WaitForTargetSpeed(targetSpeed = %5.2f, Direction = %s, Delay = %d)\n", 
        targetSpeed, direction == UP ? "UP" : "DOWN", wait);
    if(direction != UP && direction != DOWN)
    {
        Log(LOG_ERRORS, "ERROR - GenericTC::WaitForTargetSpeed - Invalid Direction Pararmeter\n");
        rtnStat = BEP_STATUS_SOFTWARE;
    }
    else
    {   // Loop to check for target speed
        do
        {   // Get the current roller speeds
            currentSpeed = GetRollSpeed();
            if(waitForBoostedAxle)
            {
                secondaryCurrentSpeed = GetBoostedRollSpeed();
            }
            // Check if the target speed has been achieved
            if(!waitForBoostedAxle && (((direction == UP) && (currentSpeed >= targetSpeed)) ||
                ((direction == DOWN) && (currentSpeed <= targetSpeed))))
            {   // target speed achieved
                proceed = true;
                Log(LOG_DEV_DATA, "WaitForTargetSpeed() reached <%f> : %f mph\n", targetSpeed, currentSpeed);
            }
            else if(waitForBoostedAxle && (((direction == UP) && (currentSpeed >= targetSpeed) && (secondaryCurrentSpeed >= targetSpeed)) ||
                     ((direction == DOWN) && (currentSpeed <= targetSpeed) && (secondaryCurrentSpeed <= targetSpeed))))
            {   // target speed achieved on both axles
                proceed = true;
                Log(LOG_DEV_DATA, "WaitForTargetSpeed() reached <%f> : %f mph on driven axle, %f on boosted axle\n", targetSpeed, currentSpeed, secondaryCurrentSpeed);
            }
            else
            {   // Target speed has not been achieved
                BposSleep(wait);
            }
            // Keep checking until target speed achieved, timeout or bad status
        } while((!proceed) && TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS));
        // Check the exit conditions
        if(proceed && (StatusCheck() == BEP_STATUS_SUCCESS))
        {   // Target speed achieved and good status
            rtnStat = BEP_STATUS_SUCCESS;
            // Allow a wait period
            BposSleep(GetParameterInt("PostAccelerationDelayPeriod"));
        }
        else if(StatusCheck() == BEP_STATUS_ABORT)
        {   // Target speed not achieved and bad status
            rtnStat = BEP_STATUS_ABORT;
            Log(LOG_ERRORS, "Bad status waiting for target speed - %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
        }
        else
        {   // timeout waiting for target speed
            rtnStat = BEP_STATUS_TIMEOUT;
            Log(LOG_ERRORS, "Timeout waiting for target speed - current speed: %.2f, target: %.2f\n",
                currentSpeed, targetSpeed);
        }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "WaitForTargetSpeed() done: %d\n", rtnStat);
    // Return the status
    return(rtnStat);
}

//=============================================================================
string GenericTC::WaitAndPromptForSpeed(float minSpeedTarget, float maxSpeedTarget, int speedScanDelay,
                                        string promptAccelerateName, /* = "AccelerateToTargetSpeed" */
                                        string promptSlowDownName) /* = "SlowDownToTargetSpeed" */
{
    float currentRollSpeed = 0.0;   // Used to store the current vehicle speed
    char tempCharArray[20];         // Used for converting integers to a characters
    string speedRange("0 0");       // Used for storing the speed range which creates the GUI green bands
    string methodResult(testFail);  // Used to return method's ending status

    // Log Method Entry
    Log(LOG_FN_ENTRY, "GenericTC::WaitAndPromptForSpeed - Enter\n");

    // Get Current Vehicle Speed
    currentRollSpeed = GetRollSpeed();
    // If the speeds are outside the range, wait for the car to get to it
    if(minSpeedTarget >= maxSpeedTarget)
    {
        Log(LOG_ERRORS, "ERROR GenericTC::WaitAndPromptForSpeed - minSpeed = %d is Greater Than maxSpeed = %d\n",
            minSpeedTarget, maxSpeedTarget);
        methodResult = testFail;
    }
    else
    {
        speedRange = CreateMessage(tempCharArray,sizeof(tempCharArray),"%d %d", (int) minSpeedTarget, (int) maxSpeedTarget);
        if((currentRollSpeed < minSpeedTarget) || (currentRollSpeed > maxSpeedTarget))
        {
            if(currentRollSpeed < minSpeedTarget)
            {
                methodResult = AccelerateToTestSpeed(minSpeedTarget, speedRange, speedScanDelay, false, promptAccelerateName);
            }
            else
            {
                methodResult = SlowDownToTestSpeed(maxSpeedTarget, speedRange, speedScanDelay, false, promptSlowDownName);
            }
            if(methodResult != testPass)
            {
                Log(LOG_ERRORS, "ERROR GenericTC::WaitAndPromptForSpeed did not pass - methodResult = %s\n",
                    methodResult.c_str());
            }
        }
        else
        {
            methodResult = testPass;
        }
    }
    // Log Method Exit and Return Value
    Log(LOG_FN_ENTRY, "GenericTC::WaitAndPromptForSpeed - Exit, Result = %s\n", methodResult.c_str());

    return(methodResult);
}

//=============================================================================
string GenericTC::AccelerateVehicleToSpeed(const float targetSpeed, 
                                           const bool startAtZeroSpeed, 
                                           const bool disengageAtSpeed)
{
    string testResult(BEP_TESTING_STATUS);
    // Log the entry and make sure this step should be performed
    Log(LOG_FN_ENTRY, "GenericTC::AccelerateVehicleToSpeed() - enter");
    if(!ShortCircuitTestStep())
    {
        string testDescription(GetTestStepInfo("Description"));
        string testResultCode("0000");
        // Wait for zero speed
        bool vehicleAtZeroSpeed = false;
        if(startAtZeroSpeed)
        {
            vehicleAtZeroSpeed = CheckZeroSpeed();
        }
        else
        {
            vehicleAtZeroSpeed = true;
        }
        // Continue with the test if conditions correct
        if(vehicleAtZeroSpeed)
        {   // Prompt the operator to shift to neutral and remove foot from brake
            DisplayPrompt(1, GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
            DisplayPrompt(2, GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
            // Wait for the operator to complete the actions
            BposSleep(GetParameterInt("PreForcedAccelerateWait"));
            // Reset the test step timer
            SetStartTime();
            // Set the motor mode to speed mode
            if(SystemCommand(MOTOR_MODE, string(LOSSCOMPENSATION_MODE)) == BEP_STATUS_SUCCESS)
            {   // Get the current wheel speed
                float currentWheelSpeed = GetRollSpeed();
                bool speedError = false;
                // Force the vehicle to speed
                do
                {   // Accelerate to speed step and hold for time
                    currentWheelSpeed += GetParameterFloat("ForcedAccelerateSpeedStep");
                    BposSleep(GetParameterInt("ForcedAccelerateSpeedStepDelay"));
                    // Verify the motor speeds are keeping up
                    speedError = (GetRollSpeed() < (currentWheelSpeed - GetParameterFloat("ForcedAccelerateSpeedStep")));
                    // Keep looping until required speed is met, or something bad happens
                } while((targetSpeed > currentWheelSpeed) &&
                        TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && !speedError);
                // Check the exit conditions
                if(GetRollSpeed() >= targetSpeed)
                {   // We are at the correct speed
                    testResult = testPass;
                    Log(LOG_DEV_DATA, "Wheel speed at or above target");
                }
                else if(!TimeRemaining() || speedError)
                {   // Timeout waiting for wheel speeds
                    testResult = testTimeout;
                    testResultCode = GetFaultCode("TargetSpeedTimeout");
                    testDescription = GetFaultDescription("TargetSpeedTimeout");
                    Log(LOG_ERRORS, "Timeout waiting for target speed: %5.2f {target:%5.2f} -- speedError: %s", 
                        GetRollSpeed(), targetSpeed, speedError ? "True" : "False");
                }
                else if(BEP_STATUS_SUCCESS != StatusCheck())
                {   // Bad system status
                    testResult = testAbort;
                    testResultCode = GetFaultCode("SystemStatus");
                    testDescription = GetFaultDescription("SystemStatus");
                    Log(LOG_ERRORS, "Bad system status whlie boosting vehicle to speed: %s", 
                        ConvertStatusToResponse(StatusCheck()).c_str());
                }
                else
                {   // Unhandled exit condition
                    testResult = testSoftwareFail;
                    testResultCode = GetFaultCode("SoftwareFailure");
                    testDescription = GetFaultDescription("SoftwareFailure");
                    Log(LOG_ERRORS, "Unknown exit condition!");
                }
                // Disengage the rollers
                if(disengageAtSpeed)
                {
                    Disengage();
                }
                else
                {   // Return the boost mode
                    SystemCommand(MOTOR_MODE, string(BOOST_MODE));
                }
            }
            else
            {   // Could not place motor into desired mode
                testResult = testFail;
                testResultCode = GetFaultCode("MotorMode");
                testDescription = GetFaultDescription("MotorMode");
                Log(LOG_ERRORS, "Could not set motor mode to %s:%s", 
                    MOTOR_MODE, LOSSCOMPENSATION_MODE);
            }
        }
        else
        {   // Timeout waiting for zero speed
            testResult = testTimeout;
            testResultCode = GetFaultCode("ZeroSpeedTimeout");
            testDescription = GetFaultDescription("ZeroSpeedTimeout");
            Log(LOG_ERRORS, "Timeout waiting for zero speed");
        }
        // Report the result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - GenericTrwABSTCTemplate::AccelerateVehicleToSpeed()");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "GenericTrwABSTCTemplate::AccelerateVehicleToSpeed() - exit");
    return testResult;
}

//=============================================================================
const bool GenericTC::CheckZeroSpeed(void)
{
    bool zeroSpeed = false;     // Used to store result from ReadSubscribeData()
    bool promptDisplayed = false;   // Used to determine if prompts have been displayed
    UINT32 delay = 100;     // Used to store amount of delay from TestStep info in config file

    // Set delay in do-while loop using BposSleep() function call
    delay = GetTestStepInfoInt("ScanDelay");
    // Start the timer
    SetStartTime();
    // Wait for the vehicle to be at zero speed
    do
    {   // Check if the vehicle is at zero speed and prompt if neccessary
        zeroSpeed = ReadSubscribeData(ZEROSPEED_TAG) == "1";

        if(!zeroSpeed)
        {   // Check if bkground test
            if(!IsForegroundTest())
            {   // Resets the step start time since the step should not prompt until
                //     its TestDomain is set to foreground
                SetStartTime();
            }
            // if TestDomain is foreground, display prompt if not already done
            else if(!promptDisplayed)
            {   // Not at zero speed, prompt the operator
                DisplayPrompt(GetPromptBox("ZeroSpeed"), GetPrompt("ZeroSpeed"), GetPromptPriority("ZeroSpeed"));
                promptDisplayed = true;
            }
            // Wait before checking again - tight loops are bad
            BposSleep(delay);
        }
    } while(TimeRemaining() && !zeroSpeed && (BEP_STATUS_SUCCESS == StatusCheck()));

    // Check if prompt needs to be removed
    if(promptDisplayed)
    {   // Remove the prompt from the display
        RemovePrompt(GetPromptBox("ZeroSpeed"), GetPrompt("ZeroSpeed"), GetPromptPriority("ZeroSpeed"));
    }

    // Log the status
    if(!zeroSpeed) Log(LOG_ERRORS, "Timeout waiting for zero speed!\n");
    else            Log(LOG_DEV_DATA, "Vehicle at zero speed\n");
    // Return the status of zero speed
    return(zeroSpeed);
}

//=============================================================================
const bool GenericTC::CheckCableConnect(void)
{
    bool cableConnected = false;  // Used to store return result from IsCableConnected()
    bool promptDisplayed = false; // Used to determine if prompts have been displayed
    UINT32 delay = 100;  // Used to store amount of delay from TestStep info in config file

    // Set delay in do-while loop using BposSleep() function call
    delay = GetTestStepInfoInt("ScanDelay");
    // Start the timer
    SetStartTime();
    // Wait for the cable to be connected
    do
    {   // Check if the cable is connected and prompt if neccessary
        cableConnected = IsCableConnected();
        Log(LOG_DEV_DATA, "CheckCableConnect - IsCableConnected: %s -- {ReadSubscribeData(CBL_CONNECT_DATA_TAG): %s}\n",
            cableConnected ? "Yes" : "No", ReadSubscribeData(CBL_CONNECT_DATA_TAG).c_str());
        if(!cableConnected)
        {   // Check if bkground test
            if( !IsForegroundTest())
            {   // Resets the step start time since the step should not prompt until
                //     its TestDomain is set to foreground
                SetStartTime();
            }
            // if TestDomain is foreground, display prompt if not already done
            else if(!promptDisplayed)
            {   // Cable not connected, prompt the operator
                DisplayPrompt(GetPromptBox("CableConnect"), GetPrompt("CableConnect"), GetPromptPriority("CableConnect"));
                promptDisplayed = true;
            }
            // Wait before checking again
            BposSleep(delay);
        }
    } while(TimeRemaining() && !cableConnected && (BEP_STATUS_SUCCESS == StatusCheck()));

    // Check if the prompt needs to be removed
    if(promptDisplayed)
    {   // Remove the prompt from the display
        RemovePrompt(GetPromptBox("CableConnect"), GetPrompt("CableConnect"), GetPromptPriority("CableConnect"));
    }
    // Log the status
    if(!cableConnected) Log(LOG_ERRORS, "Timeout waiting for cable connect!\n");
    else                 Log(LOG_DEV_DATA, "Cable Connected\n");
    // Return the status of cable connect
    return(cableConnected);
}

//=============================================================================
const bool GenericTC::IsCableConnected(void)
{
    return(ReadSubscribeData(CBL_CONNECT_DATA_TAG) == "1");
}

//=============================================================================
const bool GenericTC::IsForegroundTest(void)
{
    return(m_foregroundTest);
}

//=============================================================================
float GenericTC::GetRollSpeed(void)
{
    float currentSpeed = 0.0;
    // determine the vehicle drive wheel
    m_driveAxle = (SystemReadBool(GetDataTag("FourWheelDriveEquipped"))
                   || ReadSubscribeData(GetDataTag("DriveAxle")) == GetDataTag("Rear")) ? RWD : FWD;
    // Get the average speed for the drive axle
    if(m_driveAxle == RWD)
    {
        currentSpeed = ((SystemReadFloat(GetDataTag("LRSpeed")) +
                         SystemReadFloat(GetDataTag("RRSpeed"))) / 2.0);
    }
    else if(m_driveAxle == TWD)
    {
        currentSpeed = ((SystemReadFloat(GetDataTag("LTSpeed")) +
                         SystemReadFloat(GetDataTag("RTSpeed"))) / 2.0);
    }
    else
    {
        currentSpeed = ((SystemReadFloat(GetDataTag("LFSpeed")) +
                         SystemReadFloat(GetDataTag("RFSpeed"))) / 2.0);
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "GetRollSpeed() value: %.2f\n", currentSpeed);
    // Return the current speed
    return(currentSpeed);
}

//=============================================================================
float GenericTC::GetBoostedRollSpeed(void)
{
    float currentSpeed = 0.0;
    // determine the vehicle drive wheel
    m_driveAxle = (SystemReadBool(GetDataTag("FourWheelDriveEquipped"))
                   || ReadSubscribeData(GetDataTag("DriveAxle")) == GetDataTag("Rear")) ? RWD : FWD;
    // Get the average speed for the drive axle
    if(m_driveAxle == RWD)
    {
        currentSpeed = ((SystemReadFloat(GetDataTag("LFSpeed")) +
                         SystemReadFloat(GetDataTag("RFSpeed"))) / 2.0);
    }
    else if(m_driveAxle == TWD)
    {
        currentSpeed = ((SystemReadFloat(GetDataTag("LFSpeed")) +
                         SystemReadFloat(GetDataTag("RFSpeed"))) / 2.0);
    }
    else
    {
        currentSpeed = ((SystemReadFloat(GetDataTag("LRSpeed")) +
                         SystemReadFloat(GetDataTag("RRSpeed"))) / 2.0);
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "GetBoostedRollSpeed() value: %.2f\n", currentSpeed);
    // Return the current speed
    return(currentSpeed);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::GetWheelSpeeds(WHEELINFO &wheelSpeed)
{
    BEP_STATUS_TYPE testStatus = BEP_STATUS_FAILURE;
    const UINT32 rollerCount = GetRollerCount();
    float fltWheelSpeeds[ rollerCount];  // used to store the wheel speeds

    // Log the function entry
    Log(LOG_FN_ENTRY, "GenericTC::GetWheelSpeeds():\n");

    memset( &wheelSpeed, 0, sizeof( wheelSpeed));
    testStatus = (BEP_STATUS_TYPE)GetWheelSpeeds(fltWheelSpeeds);
    if( BEP_STATUS_SUCCESS == testStatus)
    {
        if( rollerCount > LFWHEEL)  wheelSpeed.lfWheel = fltWheelSpeeds[LFWHEEL];
        if( rollerCount > RFWHEEL)  wheelSpeed.rfWheel = fltWheelSpeeds[RFWHEEL];
        if( rollerCount > LRWHEEL)  wheelSpeed.lrWheel = fltWheelSpeeds[LRWHEEL];
        if( rollerCount > RRWHEEL)  wheelSpeed.rrWheel = fltWheelSpeeds[RRWHEEL];
        if( rollerCount > LTWHEEL)  wheelSpeed.ltWheel = fltWheelSpeeds[LTWHEEL];
        if( rollerCount > RTWHEEL)  wheelSpeed.rtWheel = fltWheelSpeeds[RTWHEEL];
    }
    // Return the status
    return(testStatus);
}

//=============================================================================
INT32 GenericTC::GetWheelSpeeds(float wheelSpeed[])
{
    INT32 testStatus = BEP_STATUS_FAILURE;  // overall status
    BEP_STATUS_TYPE openStatus = BEP_STATUS_ERROR;
    int retry = 3;  // the number of retrys to attempt

    Log(LOG_FN_ENTRY, "GetWheelSpeeds():Roller Count = %d\n", GetRollerCount());

    // Allow only one thread to access SpeedFile at a time
    if(EOK == m_speedFileMutex.Acquire())
    {
        try
        {   // clear out the wheel speeds
            memset(wheelSpeed, 0, (sizeof(float) * GetRollerCount()));
            if(!IsSpeedFileOpen())
            {   // Open the speed file
                openStatus = OpenSpeedFile();
                if(BEP_STATUS_SUCCESS != openStatus)
                {   // Error opening the speed file
                    Log(LOG_ERRORS, "Error opening speed file, not reading wheel speeds");
                }
            }
            // Ensure the file is open before we attempt to read
            if(IsSpeedFileOpen())
            {
                do
                {   // Read the speed data
                    lseek(m_speedFile, 0, SEEK_SET);
                    UINT32 bytes = read(m_speedFile, (void *) wheelSpeed,
                                        (sizeof(float) * GetRollerCount()));
                    if(bytes != (sizeof(float) * GetRollerCount()))
                    {   // if interrupted during a read, read again
                        if(errno == EINTR)
                        {
                            Log(LOG_ERRORS, "Interrupted Trying To Read %s\n",
                                GetParameter("CurrentSpeeds").c_str());
                        }
                        // else display the error report
                        else
                        {
                            Log(LOG_ERRORS, "Error Reading The File Information: %d, %s\n",
                                bytes, strerror(errno));
                        }
                    }
                    else
                    {
                        testStatus = BEP_STATUS_SUCCESS;
                        if(GetRollerCount() == 4)
                            Log(LOG_DEV_DATA, "Read speeds: %.2f, %.2f, %.2f, %.2f\n",
                                wheelSpeed[0], wheelSpeed[1], wheelSpeed[2], wheelSpeed[3]);
                        else
                            Log(LOG_DEV_DATA, "Read speeds: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
                                wheelSpeed[0], wheelSpeed[1], wheelSpeed[2], wheelSpeed[3],
                                wheelSpeed[4], wheelSpeed[5]);
                    }
                    // if the file could not be read, give up the time slice
                    if(testStatus != BEP_STATUS_SUCCESS)    BposSleep(1);
                }while((testStatus != BEP_STATUS_SUCCESS) && (retry--));
                // release the file mutex
                m_speedFileMutex.Release();
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "GetWheelSpeeds Exception: %s\n", e.what());
            testStatus = BEP_STATUS_ERROR;
            // release the file mutex
            m_speedFileMutex.Release();
        }
    }
    else
    {
        Log(LOG_ERRORS, "ERROR: Unable to aquire speed file mutex in GetWheelSpeeds: %s\n", errno);
        testStatus = BEP_STATUS_ERROR;
    }
    return(testStatus);
}

//=============================================================================
bool GenericTC::CheckForegroundLockPrompts(void)
{
    bool promptLocked = false;

    // Log method entry
    Log(LOG_FN_ENTRY, "GenericTC::CheckForegroundLockPrompts - Enter\n");

    if(ForegroundComplete())
    {
        promptLocked = (LockPrompts() == BEP_STATUS_SUCCESS);
    }

    // Log method exit
    Log(LOG_FN_ENTRY, "GenericTC::CheckForegroundLockPrompts - Exit\n");

    return(promptLocked);
}

//=============================================================================
void GenericTC::SetTestDomain(const string &testDomain)
{
    // Check if testDomain is foreground or background
    if(testDomain == "Foreground" || testDomain == "Background")
    {   // set m_foregroundTest to true for foreground test and false for background test
        m_foregroundTest = (testDomain == "Foreground") ? true : false;
    }
    else
    {   // testDomain does not match valid responses
        Log(LOG_ERRORS, "%s: Configuration File Element - TestDomain: %s - Has No Match, Default to Foreground\n",
            GetComponentName().c_str(), testDomain.c_str());
        m_foregroundTest = true;
    }
    // Log test domain
    Log(LOG_DEV_DATA, "Test Domain: %s" , IsForegroundTest() ? "Foreground" : "Background");
}

//=============================================================================
string GenericTC::OperatorYesNo(const string &operatorPrompt /*= "EnterYesOrNo"*/, const INT32 &timeout)
{
    vector<string> validResponses;

    // Log method entry
    Log(LOG_FN_ENTRY, "GenericTC::OperatorYesNo - Enter\n");

    validResponses.push_back("Yes");
    validResponses.push_back("No");

    // Log method exit
    Log(LOG_FN_ENTRY, "GenericTC::OperatorYesNo - Exit\n");

    return(GetOperatorInput(INPUT_SERVER_YES_NO_STATE, operatorPrompt, validResponses, timeout));
}

//=============================================================================
string GenericTC::OperatorPassFail(const string &operatorPrompt /*= "PassOrFail"*/, const INT32 &timeout)
{
    vector<string> validResponses;

    // Log method entry
    Log(LOG_FN_ENTRY, "GenericTC::OperatorPassFail - Enter\n");

    validResponses.push_back("Pass");
    validResponses.push_back("Fail");
    validResponses.push_back("Abort");

    // Log method exit
    Log(LOG_FN_ENTRY, "GenericTC::OperatorPassFail - Exit\n");

    return(GetOperatorInput(INPUT_SERVER_TEST_RESULT_STATE, operatorPrompt, validResponses, timeout));
}

//=============================================================================
string GenericTC::GetOperatorInput(const string &inputServerState, const string &operatorPrompt,
                                   const vector<string> &validResponses, const INT32 &timeout)
{
    bool checkKeyPress = false; // Check if a valid keypress was entered by driver, result used for controlling do-while loop
    string driverResponse(testFail);  // Used to store the Key Pressed by the driver or Error type if problem occurs

    // Log method entry
    Log(LOG_FN_ENTRY, "GenericTC::GetOperatorInput - Enter\n");
    // Set the start time for this method to control timeout
    SetStartTime();
    // Ensure we have valid responses before we start
    if(validResponses.size() > 0)
    {   // Change the mode of the InputServer
        if(SystemWrite(INPUT_SERVER_STATE, inputServerState) == BEP_STATUS_SUCCESS)
        {   // Clear the Last Pendant Key Press
            if(SystemWrite(PENDANT_KEY_PRESS, PENDANT_CLEAR_KEY_PRESS) == BEP_STATUS_SUCCESS)
            {   // Prompt the driver to Enter Response
                DisplayPrompt(GetPromptBox(operatorPrompt), GetPrompt(operatorPrompt),GetPromptPriority(operatorPrompt));
                // Wait for the operator to press the button
                do
                {   // Read the Pendant Key Press
                    if(!GetParameterBool("UsePlcResultButtons"))
                    {
                        driverResponse = SystemRead(PENDANT_KEY_PRESS);
                    }
                    else
                    {
                        driverResponse = (SystemReadBool(GetDataTag("PassButton")) ? "Pass" : 
                                          SystemReadBool(GetDataTag("FailButton")) ? "Fail" : "No Entry");
                    }
                    Log(LOG_DEV_DATA, "GetOperatorInput - Driver response: %s\n", driverResponse.c_str());
                    // Check the response
                    if(driverResponse == "ERROR")
                    {
                        Log(LOG_ERRORS, "GenericTC::GetOperatorInput - Unable to get the KeyPress information.");
                        driverResponse = testFail;
                        checkKeyPress = true;
                    }
                    else
                    {   // Make sure the response is valid
                        checkKeyPress = (find(validResponses.begin(), validResponses.end(), driverResponse) != 
                                         validResponses.end());
                    }
                    // Wait a bit for the operator
                    if(!checkKeyPress) BposSleep(250);
                } while(TimeRemaining(&timeout) && !checkKeyPress && (StatusCheck() == BEP_STATUS_SUCCESS));
                // Check the exit status
                if(StatusCheck() != BEP_STATUS_SUCCESS)
                {   // System status is not good
                    Log(LOG_ERRORS, "GenericTC::GetOperatorInput - Status Check Failure: %s", 
                        ConvertStatusToResponse(StatusCheck()).c_str());
                    driverResponse = testFail;
                }
                else if(!TimeRemaining(&timeout) && !checkKeyPress)
                {   // Timeout waiting for the operator
                    Log(LOG_ERRORS, "GenericTC::GetOperatorInput - Timeout waiting for Pendant Key Press\n");
                    driverResponse = testTimeout;
                }
                else
                {
                    Log(LOG_DEV_DATA, "GenericTC::GetOperatorInput - Button Press Detected = %s\n", 
                        checkKeyPress ? "True" : "False");
                }
                // Remove the prompt
                RemovePrompt(GetPromptBox(operatorPrompt), GetPrompt(operatorPrompt), GetPromptPriority(operatorPrompt));
            }
            else
            {   // Errors clearing the pendant key press
                Log(LOG_ERRORS, "GenericTC::GetOperatorInput - Error Unable to clear Pendant Key Press\n");
                driverResponse = testFail;
            }
            // change the mode of the InputServer back to Normal
            SystemWrite(INPUT_SERVER_STATE, INPUT_SERVER_NORMAL_MODE);
        }
        else
        {   // Error checnging pendant mode
            Log(LOG_ERRORS, "GenericTC::GetOperatorInput - Error Unable to command Pendant Mode\n");
            driverResponse = testFail;
        }
    }
    else
    {   // No valid responses provided by caller
        Log(LOG_ERRORS, "GenericTC::GetOperatorInput - No Valid Responses Passed to Method\n");
        driverResponse = testFail;
    }
    // Log method exit
    Log(LOG_FN_ENTRY, "GenericTC::GetOperatorInput - Exit\n");
    // Return the result
    return(driverResponse);
}

//=============================================================================
void GenericTC::SetupOperatorResponses(const string &inputServerState, vector<string> &responses)
{
    responses.clear();
    if(inputServerState.compare(INPUT_SERVER_TEST_RESULT_STATE) == 0)
    {   // Setup valid test result inputs
        responses.push_back(testPass);
        responses.push_back(testFail);
        responses.push_back(testAbort);
    }
    else if(inputServerState.compare(INPUT_SERVER_YES_NO_STATE) == 0)
    {
        responses.push_back("Yes");
        responses.push_back("No");
    }
}

//=============================================================================
string GenericTC::MaintainSpeedForTime(const INT32 &requiredTime,
                                       const INT32 &speedMinimum,
                                       const INT32 &speedMaximum /*=100*/,
                                       const string &maintainSpeedPrompt /*="MaintainSpeed"*/,
                                       const bool &promptIfOutOfSpeedRange /*=true*/,
                                       const bool &reportResults /*=true*/,
                                       INT32 scanDelay/*= 250*/)
{
    float currentSpeed = 0;     // Used to store current speed of vehicle
    bool promptDisplayed = false; // Used to determine if prompts have been displayed
    char tempCharArray[32];     // Used to create the needed SpeedRange for setting the green band
    string speedRange(BEP_NO_DATA);     // Used to set the green band
    string status = testPass;   // Used to store return results of methods called from within
    string eraseSpeedRange("0 0");  // Used to remove the green band
    // Variables for Sending test results
    string testResult(testPass);
    string testDescription(GetTestStepInfo("Description"));
    string testResultCode("0000");
    // Log Method Entry
    Log(LOG_FN_ENTRY, "GenericTC::MaintainSpeedForTime(RequiredTime: %d, MinSpeed: %d, MaxSpeed: %d - Enter\n",
        requiredTime, speedMinimum, speedMaximum);
    // set up string for passing proper speed range
    speedRange = CreateMessage(tempCharArray,sizeof(tempCharArray), "%d %d", speedMinimum, speedMaximum);
    // Start the timer for speed maintained
    SetSecondaryStartTime();

    do
    {   // Get the vehicle's current speed
        currentSpeed = GetRollSpeed();
        // Check if the prompts are controlled
        if(!IsForegroundTest())
        {   // Reset Test Step Time Since it is in the background and should not prompt
            SetStartTime();
        }
        // Determine if we need to prompt the operator or just wait
        // Need to maintain speed
        else if(!promptDisplayed && (speedMinimum < currentSpeed) && (currentSpeed < speedMaximum))
        {   // Display Prompt for driver to maintain speed
            DisplayPrompt(GetPromptBox(maintainSpeedPrompt), GetPrompt(maintainSpeedPrompt), 
                          GetPromptPriority(maintainSpeedPrompt));
            promptDisplayed = true;
            // Setup the Green Speed Range Bands
            SystemWrite(GetDataTag("SpeedTarget"), speedRange);
        }
        if((currentSpeed < speedMinimum) || (currentSpeed > speedMaximum))
        {
            if(promptIfOutOfSpeedRange && IsForegroundTest())
            {
                if(promptDisplayed)
                {
                    RemovePrompt(GetPromptBox(maintainSpeedPrompt), GetPrompt(maintainSpeedPrompt), 
                                 GetPromptPriority(maintainSpeedPrompt));
                }
                promptDisplayed = false;
                status = WaitAndPromptForSpeed(speedMinimum,speedMaximum,scanDelay);
            }
            else if(!promptIfOutOfSpeedRange)
            {   // Fail test because out of range and prompt driver to fix is de-selected
                testResult = testFail;
            }
            // Reset the counter for checking if required time at speed met
            SetSecondaryStartTime();
        }

        BposSleep(scanDelay);
        // Keep checking while time remaining, good testing status, we have not failed yet and system status good
    } while(SecondaryTimeRemaining(&requiredTime) && TimeRemaining() && (status == testPass) &&
            (testResult == testPass) && (StatusCheck() == BEP_STATUS_SUCCESS));
    // Check if results need to be reported
    if(reportResults)
    {
        if(StatusCheck() != BEP_STATUS_SUCCESS)
        {
            testResult = ConvertStatusToResponse(StatusCheck());
            testResultCode = GetFaultCode("StatusCheckFailure");
            testDescription = GetFaultDescription("StatusCheckFailure");
            Log(LOG_ERRORS, "GenericTC::MaintainSpeedForTime - StatusCheck() %s\n", testResult.c_str());
        }
        else if(!TimeRemaining() || status == testTimeout)
        {
            Log(LOG_ERRORS, "GenericTC::MaintainSpeedForTime - Timeout Failure\n");
            testResult = testTimeout;
            testDescription = GetFaultDescription("MaintainSpeedTimeoutFailure");
            testResultCode = GetFaultCode("MaintainSpeedTimeoutFailure");
        }
        else if(status != testPass || testResult != testPass)
        {
            Log(LOG_ERRORS, "GenericTC::MaintainSpeedForTime - Maintain Speed Failure\n");
            testResult = status == testPass ? testResult : status;
            testDescription = GetFaultDescription("MaintainSpeedFailure");
            testResultCode = GetFaultCode("MaintainSpeedFailure");
        }
        else if(!SecondaryTimeRemaining(&requiredTime))
        {
            testResult = testPass;
        }
        else
        {
            Log(LOG_ERRORS, "ERROR %s - Unanticipated result occured\n",
                GetTestStepName().c_str());
            testResult = testSoftwareFail;
            testDescription = GetFaultDescription("SoftwareFailure");
            testResultCode = GetFaultCode("SoftwareFailure");
        }

        SendSubtestResult(testResult,testDescription,testResultCode);
    }
    // Check if the prompt needs to be removed
    if(promptDisplayed)
    {   // Remove the prompt from the display
        RemovePrompt(GetPromptBox(maintainSpeedPrompt), GetPrompt(maintainSpeedPrompt), GetPromptPriority(maintainSpeedPrompt));
        // Remove the Green Speed Range Bands
        SystemWrite(GetDataTag("SpeedTarget"), eraseSpeedRange);
        // Unlock the prompt server needs to be unlocked.
    }

    // Log Method Exit
    Log(LOG_FN_ENTRY, "GenericTC::MaintainSpeedForTime - Exit -- Result:%s\n", testResult.c_str());
    // Return result of method
    return(testResult);
}

//=============================================================================
string GenericTC::AccelerateInReverse(const float &speedMinimum, const float &speedMaximum, INT32 scanDelay /*= 100*/)
{
    string eraseSpeedRange = "0 0";         // Used to remove the green band
    BEP_STATUS_TYPE speedStatus = BEP_STATUS_FAILURE;   // Used to store return result from WaitForTargetSpeed()
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_FAILURE;  // Used to store return result for module read
    bool inReverseGear = false;     // Used to tell if the current gear is reverse
    string speedRange(BEP_NO_DATA);     // Used to make the green band
    char tempCharArray[16];         // Used to create a speed range for the green band
    string testResult = testFail;       // Used to report test result
    string testDescription = GetTestStepInfo("Description");    // Used to report test description
    string testResultCode = "0000";     // Used to report test result code
    float   reverseSpeed;
    bool prevReverseGear = false;

    // Log the function entry
    Log(LOG_FN_ENTRY, "GenericTC::AccelerateInReverse() - Enter\n");

    // Determine if this test step should be performed
    if(ShortCircuitTestStep())
    {
        testResult = testSkip;
    }
    else
    {   // Need to perfrom the test step
        try
        {   // Create the speed range to display for the operator
            speedRange = CreateMessage(tempCharArray, sizeof(tempCharArray), "%d %d", 
                                       (INT32) speedMinimum, (INT32) speedMaximum);
            if(CheckZeroSpeed())
            {   // Display Prompts
                DisplayPrompt(GetPromptBox("ShiftToReverse"),GetPrompt("ShiftToReverse"),
                              GetPromptPriority("ShiftToReverse"));
                DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"),GetPrompt("AccelerateToTargetSpeed"),
                              GetPromptPriority("AccelerateToTargetSpeed"));
                // update the target green bands
                SystemWrite(GetDataTag("SpeedTarget"), speedRange);
                SetStartTime();
                // Wait for the vehicle to achieve target speed
                bool testComplete = false;
                do
                {   // Read the current gear from the module. Stub always pass for manual.
                    moduleStatus = ReadReverseGear(inReverseGear);
                    if(moduleStatus != BEP_STATUS_SUCCESS)
                    {
                        inReverseGear = prevReverseGear;
                    }
                    prevReverseGear = inReverseGear;
                    // If vehicle is in reverse, wait for the target speed
                    if((moduleStatus == BEP_STATUS_SUCCESS)  &&  (inReverseGear == true))
                    {
                        if(GetParameterInt("AbsoluteValueSpeed"))
                        {
                            speedStatus = WaitForTargetSpeed(speedMinimum, UP, scanDelay);
                        }
                        else
                        {
                            speedStatus = WaitForTargetSpeed(-speedMinimum, DOWN, scanDelay);
                        }
                    }
                    else
                    {
                        BposSleep(scanDelay);
                    }
                    // Determine if exit conditions have been met
                    if(inReverseGear && (BEP_STATUS_SUCCESS == speedStatus)) testComplete = true;
                    reverseSpeed = GetRollSpeed();
                    // Keep checking while good system status, good comms, not at speed, time remaining and not in reverse
                } while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && 
                        (moduleStatus == BEP_STATUS_SUCCESS) && !testComplete);
                // Set the test step result
                if(StatusCheck() != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_ERRORS, "ERROR GenericTC::AccelerateInReverse() - StatusCheck() result - %s\n",
                        ConvertStatusToResponse(StatusCheck()).c_str());
                    testResult = testFail;
                    testDescription = GetFaultDescription("StatusCheckFailure");
                    testResultCode = GetFaultCode("StatusCheckFailure");
                }
                else if(moduleStatus != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_ERRORS, "ERROR GenericTC::AccelerateInReverse() - Module read result - %s\n",
                        ConvertStatusToResponse(moduleStatus).c_str());
                    testResult = testFail;
                    testDescription = GetFaultDescription("CommunicationFailure");
                    testResultCode = GetFaultCode("CommunicationFailure");
                }
                else if(!TimeRemaining() || (speedStatus != BEP_STATUS_SUCCESS))
                {
                    Log(LOG_ERRORS,"Timeout waiting for Reverse Gear or required speed\n");
                    testResult = testTimeout;
                    testDescription = GetFaultDescription("AccelerateInReverseTimeout");
                    testResultCode = GetFaultCode("AccelerateInReverseTimeout");
                }
                else if(inReverseGear)
                {
                    testResult = testPass;
                }
                else
                {
                    Log(LOG_ERRORS, "ERROR GenericTC::AccelerateInReverse() - Unexpect unknown Result\n");
                    testResult = testSoftwareFail;
                    testDescription = GetFaultDescription("UnknownFailure");
                    testResultCode = GetFaultCode("UnknownFailure");
                }
                // Remove Prompts
                RemovePrompt(GetPromptBox("ShiftToReverse"),GetPrompt("ShiftToReverse"),GetPromptPriority("ShiftToReverse"));
                RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"),GetPrompt("AccelerateToTargetSpeed"),
                             GetPromptPriority("AccelerateToTargetSpeed"));
                // Remove the target green bands
                SystemWrite(GetDataTag("SpeedTarget"), eraseSpeedRange);
            }
            else
            {   // Failed to achieve Zero Speed
                Log(LOG_ERRORS, "ERROR GenericTC::AccelerateInReverse() - Failed to Achieve Zero Speed\n");
                testResult = testTimeout;
                testDescription = GetFaultDescription("AccelerateInReverseZeroSpeedTimeout");
                testResultCode = GetFaultCode("AccelerateInReverseZeroSpeedTimeout");
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "AccelerateInReverse Exception: %s\n", e.what());
            testResult = BEP_STATUS_SOFTWARE;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }

        // Send the test Results to the test result server
        SendTestResultWithDetail(testResult,testDescription,testResultCode,
                                 "TestSpeed", CreateMessage( tempCharArray, sizeof(tempCharArray), "%.02f", speedMinimum), 
                                 unitsMPH,
                                 "ReverseSpeed", CreateMessage( tempCharArray, sizeof(tempCharArray), "%.02f", reverseSpeed), 
                                 unitsMPH);
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "GenericTC::AccelerateInReverse() exit: %s\n", testResult.c_str());
    // Return the results
    return(testResult);
}

//=============================================================================
BEP_STATUS_TYPE  GenericTC::ReadReverseGear( bool &inReverseGear )
{   //stub method for manual transmissions. Overload for automatics.
    Log(LOG_ERRORS, "GenericTC::ReadReverseGear() - Always pass\n");
    inReverseGear = true;
    return(BEP_STATUS_SUCCESS);
}

//=============================================================================
/*const std::string GenericTC::EnableMotorLoading(const std::string &value)
{
    std::string status = BEP_PASS_STATUS;
    std::string accelConst, speedConst;

    Log(LOG_FN_ENTRY, "GenericTC::EnableMotorLoading(%s)\n", value.c_str());

    // Read the loading parameters
    accelConst = GetParameter( "MotorLoadingAccelConst");
    speedConst = GetParameter( "MotorLoadingSpeedConst");

    Log( LOG_DEV_DATA, "Applying loading constants: Accel=%s, Speed=%s\n", accelConst.c_str(), speedConst.c_str());

    // Apply the loading constants and make sure loading is enabled
    m_MotorController.Write(std::string(MOTOR_LOADING_ACCEL_K), accelConst, false);
    m_MotorController.Write(std::string(MOTOR_LOADING_SPEED_K), speedConst, false);
    m_MotorController.Write(std::string(COMMAND_MOTOR_LOADING), "Enabled", true);

    Log(LOG_FN_ENTRY, "GenericTC::EnableMotorLoading(%s)\n", value.c_str());

    return( status);
} */

const std::string GenericTC::EnableMotorLoading(const std::string &value)
{
    Log(LOG_FN_ENTRY, "GenericTC::EnableMotorLoading(%s), MotorLoadingAccelConst = %f, MotorLoadingSpeedConst = %f, MotorLoadingTorqueConst = %f\n", value.c_str(), GetParameterFloat( "MotorLoadingAccelConst"), GetParameterFloat( "MotorLoadingSpeedConst"), GetParameterFloat( "MotorLoadingTorqueConst") );

    return( EnableMotorLoading(GetParameterFloat( "MotorLoadingAccelConst"), 
                               GetParameterFloat( "MotorLoadingSpeedConst"), 
                               GetParameterFloat( "MotorLoadingTorqueConst"), 
                               GetParameterFloat( "MotorLoadingIncreasePerSecond")));
}

//=============================================================================
const std::string GenericTC::EnableMotorLoading(float accelConst, float speedConst, float torqueConst, float increasePerSecond)
{
    std::string status = BEP_PASS_STATUS;
    char buff[30];
    std::string accel, speed, torque;
    Log(LOG_FN_ENTRY, "GenericTC::EnableMotorLoading(float accelConst, float speedConst, float torqueConst)\n");

    Log( LOG_DEV_DATA, "Applying loading constants: Accel=%f, Speed=%f, Torque=%f, Percentage Per Second=%f\n", accelConst, speedConst, torqueConst, increasePerSecond);

    int iterationCount = 1;
    // Gradually increase motor load
    // if increasePerSecond != 1, increase the motor load gradually at a rate of ${increasePerSecond} per second
    // starting with a resolution of one update per second, if insufficient, will decrease.
    while( (iterationCount * increasePerSecond < 1) && (increasePerSecond > 0) )
    {
        // Convert from float to string
        sprintf(buff,"%f",accelConst * iterationCount * increasePerSecond);
        accel = buff;
        sprintf(buff,"%f",speedConst * iterationCount * increasePerSecond);
        speed = buff;
        sprintf(buff,"%f",torqueConst * iterationCount * increasePerSecond);
        torque = buff;

        // Apply the loading constants by increments of  and make sure loading is enabled
        m_MotorController.Write(std::string(MOTOR_LOADING_ACCEL_K), accel, false);
        m_MotorController.Write(std::string(MOTOR_LOADING_SPEED_K), speed, false);
        m_MotorController.Write(std::string(MOTOR_LOADING_TORQUE_K), torque, true);
        m_MotorController.Write(std::string(COMMAND_MOTOR_LOADING), "Enabled", true);
        iterationCount++;
        // This value may be hardcoded since it is unlikely to change
        BposSleep(1000);
    }

    // Convert from float to string
    sprintf(buff,"%f",accelConst);    
    accel = buff;
    sprintf(buff,"%f",speedConst);
    speed = buff;
    sprintf(buff,"%f",torqueConst);
    torque = buff;
    // Apply the loading constants and make sure loading is enabled
    Log(LOG_FN_ENTRY, "GenericTC::EnableMotorLoading() Writing to MotorController: accel %s, speed %s, torque %s\n", accel.c_str(), speed.c_str(), torque.c_str());
    m_MotorController.Write(std::string(MOTOR_LOADING_ACCEL_K), accel, false);
    m_MotorController.Write(std::string(MOTOR_LOADING_SPEED_K), speed, false);
    m_MotorController.Write(std::string(MOTOR_LOADING_TORQUE_K), torque, true);
    m_MotorController.Write(std::string(COMMAND_MOTOR_LOADING), "Enabled", true);

    Log(LOG_FN_ENTRY, "GenericTC::EnableMotorLoading()\n");

    return( status);
}

//=============================================================================
const std::string GenericTC::DisableMotorLoading(const std::string &value)
{
    std::string status = BEP_PASS_STATUS;

    Log(LOG_FN_ENTRY, "GenericTC::DisableMotorLoading(%s)\n", value.c_str());

    // Command the motor controller to disable motor loading
    m_MotorController.Write(std::string(COMMAND_MOTOR_LOADING), "Disabled", true);

    Log(LOG_FN_ENTRY, "GenericTC::DisableMotorLoading(%s)\n", value.c_str());

    return( status);
}

//=============================================================================
const std::string GenericTC::TestStepParkBrake(const std::string &value, bool isTandemAxle /*= false*/)
{
    std::string status; // the status of the test step

    // determine what type of park brake test should be run
    if(ReadSubscribeData(GetDataTag("MachineType")) == GetDataTag("MachineType3600"))
    {
        // update the current test step base on the value
        SetCurrentTestStep(m_testSteps.getNode("ElectricParkBrake"));
        status = TestStepElectricParkBrake(value, isTandemAxle);
    }
    else
    {
        // update the current test step base on the value
        SetCurrentTestStep(m_testSteps.getNode("HydraulicParkBrake"));
        status = TestStepHydraulicParkBrake(value);
    }

    return(status);
}

//=============================================================================
const std::string GenericTC::TestStepParkPawl(const std::string &value)
{
    std::string status; // the status of the test step

    // determine what type of park brake test should be run
    if(ReadSubscribeData(GetDataTag("MachineType")) == GetDataTag("MachineType3600"))
    {
        // update the current test step base on the value
        SetCurrentTestStep(m_testSteps.getNode("ElectricParkPawl"));
        status = TestStepElectricParkPawl(value);
    }
    else
    {
        // update the current test step base on the value
        SetCurrentTestStep(m_testSteps.getNode("HydraulicParkPawl"));
        status = TestStepHydraulicParkPawl(value);
    }

    return(status);
}

//=============================================================================
const std::string GenericTC::TestStepParkBrakePawl(const std::string &value)
{
    std::string status; // the status of the test step

    // determine what type of park brake test should be run
    if(ReadSubscribeData(GetDataTag("MachineType")) == GetDataTag("MachineType3600"))
    {
        // update the current test step base on the value
        SetCurrentTestStep(m_testSteps.getNode("ElectricParkBrakePawl"));
        status = TestStepElectricParkBrakePawl(value);
    }
    else
    {
        // update the current test step base on the value
        SetCurrentTestStep(m_testSteps.getNode("HydraulicParkBrakePawl"));
        status = TestStepHydraulicParkBrakePawl(value);
    }

    return(status);
}

//=============================================================================
const std::string GenericTC::AnalyzeParkBrakeTest(const std::string &value, bool isTandemAxle /*= false*/)
{
    std::string testStatus = testFail;

    Log(LOG_FN_ENTRY, "Enter GenericTC::AnalyzeParkBrakeTest(%s)\n", value.c_str());
    // If this is an electric vehicle
    if(ReadSubscribeData(GetDataTag("MachineType")) == GetDataTag("MachineType3600"))
    {
        // Analyze the distances that the wheels travelled
        testStatus = ValidateParkBrakeTestDistances(m_parkBrakeDistances[0], m_parkBrakeDistances[1], isTandemAxle);
    }
    // Can't check for driver influence because we don't read
    // distances on a hydraulic machine
    else
    {
        if((ReadSubscribeData( GetDataTag("ParkingBrakeTestPass")) == "1") &&
           (ReadSubscribeData( GetDataTag("ParkingBrakeTestFail")) == "0"))
        {
            testStatus = testPass;  // set test step status to pass
        }
        else
        {
            testStatus = testFail;  // set test step status to fail
        }
    }
    Log(LOG_FN_ENTRY, "Exit GenericTC::AnalyzeParkBrakeTest(%s)\n", value.c_str());
    return( testStatus);
}

//=============================================================================
const std::string GenericTC::AnalyzeParkPawlTest(const std::string &value)
{
    std::string testStatus = testFail;

    Log(LOG_FN_ENTRY, "Enter GenericTC::AnalyzeParkPawlTest(%s)\n", value.c_str());
    // If this is an electric vehicle
    if(ReadSubscribeData(GetDataTag("MachineType")) == GetDataTag("MachineType3600"))
    {
        // Analyze the distances that the wheels travelled
        testStatus = ValidateParkPawlTestDistances(m_parkPawlDistances[0], m_parkPawlDistances[1]);
    }
    // Can't check for driver influence because we don't read
    // distances on a hydraulic machine
    else
    {
        if((ReadSubscribeData( GetDataTag("ParkPawlTestPass")) == "1") &&
           (ReadSubscribeData( GetDataTag("ParkPawlTestFail")) == "0"))
        {
            testStatus = testPass;  // set test step status to pass
        }
        else
        {
            testStatus = testFail;  // set test step status to fail
        }
    }
    Log(LOG_FN_ENTRY, "Exit GenericTC::AnalyzeParkPawlTest(%s)\n", value.c_str());
    return( testStatus);
}

//=============================================================================
const std::string GenericTC::TestStepHydraulicParkBrake(const std::string &value)
{
    std::string testStatus = BEP_NONE;  // set test step status to no response
    std::string result = BEP_NONE;    // set result to no response
    int         driverDelayTime, prepTime;
    INT32       status = StatusCheck();

    Log(LOG_FN_ENTRY, "GenericTC::TestStepHydraulicParkBrake(%s)\n", value.c_str());
    try
    {
        // if the conditions are correct to perform the task
        status = StatusCheck();
        if(status == BEP_STATUS_SUCCESS)
        {
            if(UpdatePrompts() != BEP_STATUS_SUCCESS) Log(LOG_ERRORS, "Unable to Update Prompts\n");
            driverDelayTime = GetParameterInt("ParkBrakeDriverDelay");

            // Allow the driver to shift to Neutral
            status = HydraulicParkBrakeInit();
            if(status == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA,"Starting Hydraulic Park Brake Test\n");

                INT32   pbForce = GetParameterInt("ParkBrakeApplyForce");
                SystemWrite(GetDataTag("UnidirectionalParkBrakeTest"), GetParameterBool("UnidirectionalParkBrakeTest"));
                SystemWrite(GetDataTag("RearBoostForce"), pbForce);
                SystemWrite(GetDataTag("FrontBoostForce"), pbForce);
                SystemWrite(GetDataTag("ParkingBrakeTest"),true);

                // Allow the PLC to initialize
                prepTime = GetParameterInt("ParkBrakePrepTime");
                Log(LOG_DEV_DATA, "Setting up to wait for pb apply: %d\n", prepTime);
                status = StatusSleep( prepTime);
                Log(LOG_DEV_DATA, "Setup complete for pb apply: %d\n", prepTime);

                SetStartTime();
                while( (TimeRemaining()) && (status == BEP_STATUS_SUCCESS) &&
                       (ReadSubscribeData("ParkingBrakeTestPass") == "0") &&
                       (ReadSubscribeData("ParkingBrakeTestFailed") == "0"))
                {
                    BposSleep(200);
                    status = StatusCheck();
                }
                Log(LOG_DEV_DATA,"Park brake Passed = %s, Park brake Failed = %s\n",
                    ReadSubscribeData("ParkingBrakeTestPass").c_str(),
                    ReadSubscribeData("ParkingBrakeTestFailed").c_str());

                if(ReadSubscribeData("ParkingBrakeTestPass") == "1") result = "PASS";
                else result = "FAIL";

                Log(LOG_DEV_DATA,"Sending parking brake test = 0 to PLC\n");
                SystemWrite(GetDataTag("ParkingBrakeTest"),false);
            }
            else
            {
                UpdateResult( status, testStatus);
            }

            // tell the driver to release the parking brake
            DisplayTimedPrompt(GetParameter("ReleaseParkBrakePrompt"), "1", 0, driverDelayTime);
        }
        else
        {
            UpdateResult( status, testStatus);
        }

        // update the status of the test
        if(status != BEP_STATUS_SUCCESS) UpdateResult(status, testStatus);
        else if(!TimeRemaining())        testStatus = BEP_TIMEOUT_STATUS;
        else if(result == "FAIL")        testStatus = BEP_FAIL_STATUS;
        else                              testStatus = BEP_PASS_STATUS;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "GenericTC::TestStepHydraulicParkBrake Exception: %s\n", e.what());
        testStatus = BEP_SOFTWAREFAIL_STATUS;
    }

    // update the test result
    if(SendTestResult(testStatus, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "GenericTC::TestStepHydraulicParkBrake Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), testStatus.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    return(testStatus);     // return the status
}

//=============================================================================
INT32 GenericTC::HydraulicParkBrakeInit(void)
{
    int         driverDelayTime = GetParameterInt("ParkBrakeDriverDelay");
    INT32       status;

    Log(LOG_FN_ENTRY, "Enter GenericTC::HydraulicParkBrakeInit()\n");

    // Allow the driver to shift to Neutral
    Log(LOG_DEV_DATA, "Waiting for driver pb apply: %d\n", driverDelayTime);
    status = StatusSleep( driverDelayTime);

    Log(LOG_FN_ENTRY, "Exit GenericTC::HydraulicParkBrakeInit(), status = %d\n", status);

    return( status);
}

//=============================================================================
const std::string GenericTC::TestStepElectricParkBrake(const std::string &value, bool isTandemAxle /*= false*/)
{
    string  testStatus;     // the overall test status
    INT32   status = StatusCheck();
    int driverDelayTime = (GetParameterInt("ParkBrakeDriverDelay") != 0) ? GetParameterInt("ParkBrakeDriverDelay") : 3000;

    Log(LOG_FN_ENTRY, "GenericTC::TestStepElectricParkBrake(%s)\n", value.c_str());
    if(!ShortCircuitTestStep())
    {
    
        try
        {
            // if the conditions are correct to perform the task
            status = StatusCheck();
            if( status == BEP_STATUS_SUCCESS)
            {
                if(UpdatePrompts() != BEP_STATUS_SUCCESS) Log(LOG_ERRORS, "Unable to Update Prompts\n");
                // Allow the driver to shift to Neutral and apply park brake
                status = ElectricParkBrakeInit();
                // if everything is still good
                if( status == BEP_STATUS_SUCCESS)
                {
                    // get the direction and perform the tests
                    string direction = GetTestStepInfo("RollerDirection");
                    if(IsParkBrakeDirectionValid(direction))
                    {
                        if((direction == "Forward") || (direction == "Both"))
                        {
                            // perform the roller torque test forwards
                            testStatus = PerformPBTorqueTest("Forward", isTandemAxle);
                        }
                        if( ((testStatus == testPass) && (direction == "Both"))
                            ||(direction == "Reverse") )
                        {
                            // perform the roller torque test in reverse
                            testStatus = PerformPBTorqueTest("Reverse", isTandemAxle);
                        }
                    }
                    else
                    {   // Requested direction is not valid, failing the test
                        Log(LOG_ERRORS, "Specified direction <%s> is not valid.  Failing test step %s.",
                            direction.c_str(), GetTestStepName().c_str());
                        testStatus = testFail;
                    }
                }
                else
                {
                    Log(LOG_DEV_DATA, (GetTestStepName() + ": Failed first StatusCheck\n").c_str());
                }
    
                // store the park brake handle force if specified in the configuration file
                ProcessParkBrakeHandleForce();
    
                // tell the driver to release the parking brake
                DisplayTimedPrompt(GetParameter("ReleaseParkBrakePrompt"), "1", 0, driverDelayTime);
    
                // command the motor controller to boost mode
                SystemCommand(MOTOR_MODE, string(BOOST_MODE));
    
                // update the status of the test
                if(!TimeRemaining())    status = BEP_STATUS_TIMEOUT;
                else                    status = StatusCheck();
            }
    
            UpdateResult(status, testStatus);
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "GenericTC::TestStepElectricParkBrake Exception: %s\n", e.what());
            testStatus = BEP_SOFTWAREFAIL_STATUS;
        }
        // Get the max force with the mutliplier applied
        float maxForceWithMultiplier = ApplyForceMultiplier(GetParameterFloat("ParkBrakeApplyForce"), GetParameterFloat("ParkBrakeForceMultiplier"));
        // update the test result
        char buff[16];
        if(SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), "0000", 
                                    "ModifiedMaxParkBrakeForce", 
                                    CreateMessage(buff, sizeof(buff), "%.2f", maxForceWithMultiplier), unitsLBF) != BEP_STATUS_SUCCESS)
        {
            Log(LOG_ERRORS, "GenericTC::TestStepElectricParkBrake Could Not Send Test Result: %s, %s\n",
                GetTestStepName().c_str(), testStatus.c_str());
        }
    
        RemovePrompts();    // remove the prompts from the screen
    }
    else
    {
        Log(LOG_FN_ENTRY, "Skipping test step GenericTC::TestStepElectricParkBrake()");
        testStatus = testSkip;
    }

    return(testStatus);     // return the status
}

//=============================================================================
INT32 GenericTC::ElectricParkBrakeInit(void)
{
    int driverDelayTime = (GetParameterInt("ParkBrakeDriverDelay") != 0) ? GetParameterInt("ParkBrakeDriverDelay") : 3000;

    Log(LOG_FN_ENTRY, "Enter GenericTC::::ElectricParkBrakeInit()\n");

    // delay the specified driver delay time as long as the component status is ok
    Log(LOG_DEV_DATA, "Setting up to wait for pb driver delay: %d\n", driverDelayTime);
    INT32 status = StatusSleep(driverDelayTime);
    Log(LOG_DEV_DATA, "Setup complete for pb driver delay: %d\n", driverDelayTime);

    Log(LOG_FN_ENTRY, "Exit GenericTC::::ElectricParkBrakeInit(), status=%d\n", status);

    return( status);
}

//=============================================================================
const std::string GenericTC::TestStepHydraulicParkPawl(const std::string &value)
{
    std::string testStatus = BEP_NONE;  // set test step status to no response
    std::string result = BEP_NONE;    // set result to no response
    std::string faultTag = "ParkPawlFault";
    INT32       status = StatusCheck();

    Log(LOG_FN_ENTRY, "GenericTC::TestStepHydraulicParkPawl(%s)\n", value.c_str());
    try
    {
        // if the conditions are correct to perform the task
        status = StatusCheck();
        if(status == BEP_STATUS_SUCCESS)
        {
            if(UpdatePrompts() != BEP_STATUS_SUCCESS) Log(LOG_ERRORS, "Unable to Update Prompts\n");
            // Give the driver time to shift to park
            status = HydraulicParkPawlInit();
            if( status == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA,"Starting Park Pawl Test\n");

                INT32   ppForce = GetParameterInt("ParkPawlApplyForce");
                const std::string drvAxle = ReadSubscribeData( GetDataTag("DriveAxle"));
                if( drvAxle == "Rear")  SystemWrite(GetDataTag("RearBoostForce"), ppForce);
                else                    SystemWrite(GetDataTag("FrontBoostForce"), ppForce);
                SystemWrite(GetDataTag("UnidirectionalParkBrakeTest"), GetParameterBool("UnidirectionalParkPawlTest"));
                SystemWrite(GetDataTag("ParkPawlTest"),true);

                // Give the PLC some time to get setup
                int prepTime = GetParameterInt("ParkPawlPrepTime");
                status = StatusSleep(prepTime);

                // Reset the test step timeout
                SetStartTime();

                // Wait for PLC to say pass or fail
                result = "FAIL";
                while( (TimeRemaining()) && (status == BEP_STATUS_SUCCESS) &&
                       (ReadSubscribeData("ParkPawlTestPass") == "0") &&
                       (ReadSubscribeData("ParkPawlTestFail") == "0"))
                {
                    BposSleep(200);
                    status = StatusCheck();
                }
                Log(LOG_DEV_DATA,"Park pawl Passed = %s, Park pawl Failed = %s\n", 
                    ReadSubscribeData( GetDataTag("ParkPawlTestPass")).c_str(),
                    ReadSubscribeData( GetDataTag("ParkPawlTestFail")).c_str());

                if(ReadSubscribeData( GetDataTag("ParkPawlTestPass")) == "1")
                {
                    result = "PASS";
                }
                else
                {
                    result = "FAIL";
                }
            }

            Log(LOG_DEV_DATA,"Sending pawk pawl test = 0 to PLC\n");
            SystemWrite(GetDataTag("ParkPawlTest"),false);

            // update the status of the test
            if( result == "PASS")                           status = BEP_STATUS_SUCCESS;
            else if(!TimeRemaining())                       status = BEP_STATUS_TIMEOUT;
            else if(result == "ABORT")                      status = BEP_STATUS_ABORT;
            else if(result == "PASS")                       status = BEP_STATUS_FAILURE;
            else                                            status = StatusCheck();

            UpdateResult( status, testStatus);
        }
        // else the conditions are not correct, indicate not started
        else
        {
            testStatus = BEP_TEST_NOT_STARTED;
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "GenericTC::TestStepHydraulicParkPawl Exception: %s\n", e.what());
        testStatus = BEP_SOFTWAREFAIL_STATUS;
    }

    // update the test result
    int err = SendTestResultWithDetail( testStatus,
                                        GetTestStepInfo( "Description"),
                                        GetFaultCode( faultTag),
                                        GetFaultName(faultTag),
                                        GetFaultDescription(faultTag));
    if(err != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "GenericTC::TestStepHydraulicParkPawl Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), testStatus.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    return(testStatus);     // return the status
}

//=============================================================================
INT32 GenericTC::HydraulicParkPawlInit(void)
{
    Log(LOG_FN_ENTRY, "Enter GenericTC::HydraulicParkPawlInit()\n");

    // Give the driver time to shift to park
    int driverDelayTime = (GetParameterInt("ParkPawlDriverDelay") != 0) ? GetParameterInt("ParkPawlDriverDelay") : 3000;
    // delay the specified driver delay time
    INT32 status = StatusSleep(driverDelayTime);

    Log(LOG_FN_ENTRY, "Exit GenericTC::HydraulicParkPawlInit(), status=%d\n", status);

    return( status);
}

//=============================================================================
const std::string GenericTC::TestStepElectricParkPawl(const std::string &value)
{
    // The overall test step status
    string testStatus;
    BEP_STATUS_TYPE status;

    Log(LOG_DEV_DATA, "Enter GenericTC::TestStepElectricParkPawl()\n");

    try
    {
        // update the driver prompt
        UpdatePrompts();

        // delay the specified driver delay time
        int driverDelayTime = (GetParameterInt("ParkPawlDriverDelay") != 0) ? GetParameterInt("ParkPawlDriverDelay") : 3000;
        status = StatusSleep(driverDelayTime);
        // if everything is still good
        if( status == BEP_STATUS_SUCCESS)
        {
            // get the direction and perform the tests
            string direction = GetTestStepInfo("RollerDirection");
            if(IsParkBrakeDirectionValid(direction))
            {
                if((direction == "Forward") || (direction == "Both"))
                {
                    //turn rollers forward direction
                    testStatus = ParkPawlTorqueTest("Forward");
                }
                //if we passed the forward direction and we want to do both directions
                //or if we are just turning the rollers in the reverse direction
                if( ((testStatus == testPass) && (direction == "Both"))
                    ||(direction == "Reverse") )
                {
                    //turn rollers reverse direction
                    testStatus = ParkPawlTorqueTest("Reverse");
                }
            }
            else
            {   // Requested direction is not valid, failing the test
                Log(LOG_ERRORS, "Specified direction <%s> is not valid.  Failing test step %s.",
                    direction.c_str(), GetTestStepName().c_str());
                testStatus = testFail;
            }
        }
        else
        {
            UpdateResult( status, testStatus);
        }
    }
    catch(BepException &ex)
    {
        Log(LOG_ERRORS,
            (GetTestStepName() + ": Exception: " + ex.Reason() + "\n").c_str());
        testStatus = testSoftwareFail;
    }
    UpdateResult(StatusCheck(), testStatus);

    // command the motor controller to boost mode
    SystemCommand(MOTOR_MODE, string(BOOST_MODE));

    // Report the status of the test
    if(testStatus == testPass)
    {
        SendTestResult(testPass, GetTestStepInfo("Description"));

        // Log the function exit
        Log(LOG_FN_ENTRY, (GetTestStepName() + ": EXIT SUCCESS").c_str());
    }

    // remove the operator prompts
    RemovePrompts();

    Log(LOG_DEV_DATA, "Exit GenericTC::TestStepElectricParkPawl(), status=%s\n", testStatus.c_str());

    return(testStatus);
}

//=============================================================================
INT32 GenericTC::ElectricParkPawlInit(void)
{
    Log(LOG_FN_ENTRY, "Enter GenericTC::ElectricParkPawlInit()\n");

    // delay the specified driver delay time
    int driverDelayTime = (GetParameterInt("ParkPawlDriverDelay") != 0) ? GetParameterInt("ParkPawlDriverDelay") : 3000;
    INT32 status = StatusSleep(driverDelayTime);

    Log(LOG_FN_ENTRY, "Exit GenericTC::ElectricParkPawlInit(), status=%d\n", status);

    return( status);
}

//=============================================================================
const std::string GenericTC::TestStepHydraulicParkBrakePawl(const std::string &value)
{
    std::string testStatus = BEP_NONE;  // set test step status to no response
    std::string     pbStatus(testFail), pbFaultTag("FaultStaticParkBrake");
    std::string     ppStatus(testFail), ppFaultTag("ParkPawlFault");
    INT32       prepTime, driverDelayTime, temp1;
    bool        pbDone, ppDone;
    INT32       status;

    pbDone = false;
    ppDone = false;

    Log(LOG_DEV_DATA, "Enter GenericTC::TestStepHydraulicParkBrakePawl()\n");
    try
    {
        // if the conditions are correct to perform the task
        status = StatusCheck();
        if( status == BEP_STATUS_SUCCESS)
        {
            if(UpdatePrompts() != BEP_STATUS_SUCCESS) Log(LOG_ERRORS, "Unable to Update Prompts\n");
            driverDelayTime = GetParameterInt("ParkPawlDriverDelay");

            // Give the driver time to shift to park
            status = HydraulicParkBrakePawlInit();

            // if everything is still good
            if( status == BEP_STATUS_SUCCESS)
            {
                int parkBrakeForce = GetParameterInt("ParkBrakeApplyForce");
                int parkPawlForce = GetParameterInt("ParkPawlApplyForce");

                Log(LOG_DEV_DATA,"Starting Park Brake/Pawl Test: %d lbs/%d lbs\n",
                    parkBrakeForce, parkPawlForce);

                SystemWrite(GetDataTag("RearBoostForce"), parkBrakeForce);
                SystemWrite(GetDataTag("FrontBoostForce"), parkPawlForce);
                SystemWrite(GetDataTag("UnidirectionalParkBrakeTest"), GetParameterBool("UnidirectionalParkBrakeTest"));
                SystemWrite(GetDataTag("ParkingBrakeTest"),true);
                SystemWrite(GetDataTag("ParkPawlTest"),true);

                // Give the PLC some time to get setup
                prepTime = GetParameterInt( "ParkPawlPrepTime");
                temp1 = GetParameterInt( "ParkBrakePrepTime");
                if( temp1 > prepTime)   prepTime = temp1;
                status = StatusSleep( prepTime);

                // Reset the test step timeout
                SetStartTime();

                // Wait for PLC to say pass or fail
                while( ((pbDone == false) || (ppDone == false)) &&
                       (BEP_STATUS_SUCCESS == status) && (TimeRemaining()) )
                {
                    if( pbDone == false)
                    {
                        if((ReadSubscribeData( GetDataTag("ParkingBrakeTestPass")) != "0") ||
                           (ReadSubscribeData( GetDataTag("ParkingBrakeTestFail")) != "0"))
                        {
                            Log( LOG_DEV_DATA, "Park brake test completed\n");
                            pbDone = true;
                        }
                    }
                    if( ppDone == false)
                    {
                        if((ReadSubscribeData( GetDataTag("ParkPawlTestPass")) != "0") ||
                           (ReadSubscribeData( GetDataTag("ParkPawlTestFail")) != "0"))
                        {
                            Log( LOG_DEV_DATA, "Park pawl test completed\n");
                            ppDone = true;
                        }
                    }

                    status = StatusSleep(200);
                }

                // If no status error and no timeout yet
                if( (BEP_STATUS_SUCCESS == status) && (TimeRemaining()) )
                {
                    // Check is park brake passed
                    pbStatus = (ReadSubscribeData( GetDataTag("ParkingBrakeTestPass")) == "1") ? testPass : testFail;
                    // Check is park pawl passed
                    ppStatus = (ReadSubscribeData( GetDataTag("ParkPawlTestPass")) == "1") ? testPass : testFail;

                    Log(LOG_DEV_DATA,"Park brake: %s\n", pbStatus.c_str());
                    Log(LOG_DEV_DATA,"Park pawl: %s\n", ppStatus.c_str());

                    // If both tests passed
                    if(( testPass == pbStatus) && (testPass == ppStatus))
                    {
                        status = BEP_STATUS_SUCCESS;
                    }
                    else
                    {
                        status = BEP_STATUS_FAILURE;
                    }
                }
                else
                {
                    // If we timed out
                    if( (BEP_STATUS_SUCCESS == status) && (false == TimeRemaining()) )
                    {
                        status = BEP_STATUS_TIMEOUT;
                    }
                }
            }

            Log(LOG_DEV_DATA,"Sending park brake/pawl test = 0 to PLC\n");
            SystemWrite(GetDataTag("ParkingBrakeTest"),false);
            SystemWrite(GetDataTag("ParkPawlTest"),false);

            // tell the driver to release the parking brake
            DisplayTimedPrompt(GetParameter("ReleaseParkBrakePrompt"), "1", 0, driverDelayTime);
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "GenericTC::TestStepHydraulicParkBrakePawl Exception: %s\n", e.what());
        status = BEP_STATUS_SOFTWARE;
    }

    // update the status of the test
    UpdateResult(status, testStatus);

    SendSubtestResultWithDetail("ParkBrakeTest", pbStatus,
                                GetTestStepInfo("Description"),
                                GetFaultCode( pbFaultTag),
                                GetFaultName( pbFaultTag),
                                GetFaultDescription(pbFaultTag));
    SendSubtestResultWithDetail("ParkPawlTest", ppStatus,
                                GetTestStepInfo("Description"),
                                GetFaultCode( ppFaultTag),
                                GetFaultName( ppFaultTag),
                                GetFaultDescription( ppFaultTag));

    // update the test result
    if(SendTestResult(testStatus, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "GenericTC::TestStepHydraulicParkBrakePawl Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), testStatus.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    Log(LOG_DEV_DATA, "Exit GenericTC::TestStepHydraulicParkBrakePawl(), status=%s\n", testStatus.c_str());

    return(testStatus);     // return the status
}

//=============================================================================
INT32 GenericTC::HydraulicParkBrakePawlInit(void)
{
    INT32   driverDelayTime, temp1, status;

    Log(LOG_FN_ENTRY, "Enter GenericTC::HydraulicParkBrakePawlInit()\n");

    // delay the specified driver delay time
    // Give the driver time to shift to park
    driverDelayTime = (GetParameterInt("ParkPawlDriverDelay") != 0) ? GetParameterInt("ParkPawlDriverDelay") : 3000;
    temp1 = GetParameterInt("ParkBrakeDriverDelay");
    if( temp1 > driverDelayTime)    driverDelayTime = temp1;
    status = StatusSleep(driverDelayTime);

    Log(LOG_FN_ENTRY, "Exit GenericTC::HydraulicParkBrakePawlInit(), status=%d\n", status);

    return( status);
}

//=============================================================================
const std::string GenericTC::TestStepElectricParkBrakePawl(const std::string &value)
{
    string testStatus;  // Overall test statusn
    string faultTag("ParkBrakeParkPawlFault");    // The fault code
    BEP_STATUS_TYPE status;
    // delay the specified driver delay time
    int driverDelayTime = max( GetParameterInt("ParkPawlDriverDelay"), GetParameterInt("ParkBrakeDriverDelay"));

    Log(LOG_DEV_DATA, "Enter GenericTC::TestStepElectricParkBrakePawl()\n");

    try
    {
        UpdatePrompts();
        // Wait for the driver to shift and apply park prake
        status = (BEP_STATUS_TYPE)ElectricParkBrakePawlInit();
        // if everything is still good
        if( status == BEP_STATUS_SUCCESS)
        {
            // get the direction and perform the tests
            string direction = GetTestStepInfo("RollerDirection");
            if(IsParkBrakeDirectionValid(direction))
            {
                if((direction == "Forward") || (direction == "Both"))
                {
                    // perform the roller torque test forwards
                    testStatus = ComboTorqueTest("Forward");
                }
                if( ((testStatus == testPass) && (direction == "Both"))
                    ||(direction == "Reverse") )
                {
                    // perform the roller torque test in reverse
                    testStatus = ComboTorqueTest("Reverse");
                }
            }
            else
            {   // Requested direction is not valid, failing the test
                Log(LOG_ERRORS, "Specified direction <%s> is not valid.  Failing test step %s.",
                    direction.c_str(), GetTestStepName().c_str());
                testStatus = testFail;
            }
        }
        else
        {
            UpdateResult(status, testStatus);
        }

        // tell the driver to release the parking brake
        DisplayTimedPrompt(GetParameter("ReleaseParkBrakePrompt"), "1", 0, driverDelayTime);
    }
    catch(BepException &ex)
    {
        Log(LOG_ERRORS,
            (GetTestStepName() + ": Exception: " + ex.Reason() + "\n").c_str());
        testStatus = testSoftwareFail;
    }

    // Report the status of the test
    if(testStatus == testPass)
    {
        SendTestResult(testPass, GetTestStepInfo("Description"));

        // Log the function exit
        Log(LOG_FN_ENTRY, (GetTestStepName() + ": EXIT SUCCESS").c_str());
    }
    else
    {
        // Send the overall test result
        SendTestResultWithDetail(testStatus,
                                 GetTestStepInfo("Description"),
                                 GetFaultCode(faultTag),
                                 GetFaultName(faultTag),
                                 GetFaultDescription(faultTag));
        // Log the function exit
        Log(LOG_FN_ENTRY,
            (GetTestStepName() + ": EXIT FAIL" + testStatus).c_str());
    }

    Log(LOG_DEV_DATA, "Exit GenericTC::TestStepElectricParkBrakePawl(), status=%s\n", testStatus.c_str());

    return(testStatus);
}

//=============================================================================
INT32 GenericTC::ElectricParkBrakePawlInit(void)
{
    INT32 status;

    Log(LOG_FN_ENTRY, "Enter GenericTC::ElectricParkBrakePawlInit()\n");

    // delay the specified driver delay time
    int driverDelayTime = (GetParameterInt("ParkPawlDriverDelay") != 0) ? GetParameterInt("ParkPawlDriverDelay") : 3000;

    // if everything is still good
    status = StatusSleep(driverDelayTime);

    Log(LOG_FN_ENTRY, "Exit GenericTC::ElectricParkBrakePawlInit(), status=%d\n", status);

    return( status);
}

//=============================================================================
const std::string GenericTC::ParkPawlTorqueTest(const std::string &direction)
{
    string  status;         // the current test status
    const int rollerCount = GetRollerCount();
    string  frontMotorMode, rearMotorMode;
    string  lfMotorValTag, rfMotorValTag;
    string  lrMotorValTag, rrMotorValTag;
    float   torqueVal = ((GetParameterFloat("ParkPawlApplyForce") != 0) ? GetParameterFloat("ParkPawlApplyForce") : 350) * 0.5;
    float   speedVal = (GetParameterFloat("ParkPawlSpeedValue") != 0) ? GetParameterFloat("ParkPawlSpeedValue") : 5;
    float   wheelSpeeds[rollerCount];
    int     testTime = GetTestStepInfoInt("TestTime");
    char    temp[256];
    INT32   statusCheck;

    Log(LOG_FN_ENTRY, "Enter GenericTC::ParkPawlTorqueTest(%s)\n", direction.c_str());

    // Go to torque 0 first to clear any command history in the motor controller
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);
    BposSleep( 100);

    // If we want the wheels to turn forward, we need a negative torque
    if( ((direction == "Forward") && (torqueVal > 0)) ||
        // If we want the wheels to turn in reverse, we need a positive torque value
        ((direction == "Reverse") && (torqueVal < 0)) )
    {
        Log( LOG_DEV_DATA, "Changing torque value from %.02f to %.02f\n", torqueVal, -torqueVal);
        torqueVal *= -1.0;
    }

    // If we want the wheels to turn forward, we need a positive speed
    if( ((direction == "Forward") && (speedVal < 0)) ||
        // If we want the wheels to turn in reverse, we need a negative speed
        ((direction == "Reverse") && (speedVal > 0)) )
    {
        Log( LOG_DEV_DATA, "Changing speed value from %.02f to %.02f\n", speedVal, -speedVal);
        speedVal *= -1.0;
    }

    // read the current distance of the wheels
    GetWheelDistances( m_parkPawlDistances[ 0]);

    // If Front Wheel Drive
    if( ReadSubscribeData( "DriveAxle") == FRONT_WHEEL_DRIVE_VALUE)
    {
        // Tell the motor to turn for the park brake test
        Log( LOG_DEV_DATA, "Commanding Front Motors to %.2f LBS and Rear Motors %.2f MPH\n",
             torqueVal, speedVal);

        m_MotorController.Write("LeftFrontMotorMode", TORQUE_MODE, false);
        m_MotorController.Write("RightFrontMotorMode", TORQUE_MODE, false);
        m_MotorController.Write("LeftRearMotorMode", SPEED_MODE, false);
        m_MotorController.Write("RightRearMotorMode", SPEED_MODE, false);
        m_MotorController.Write("LeftFrontTorqueValue", CreateMessage(temp, 256, "%.2f", torqueVal), false);
        m_MotorController.Write("RightFrontTorqueValue", CreateMessage(temp, 256, "%.2f", torqueVal), false);
        m_MotorController.Write("LeftRearSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), false);
        m_MotorController.Write("RightRearSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), true);
    }
    // Rear wheel drive
    else
    {
        // Tell the motor to turn for the park brake test
        Log( LOG_DEV_DATA, "Commanding Front Motors to %.2f MPH and Rear Motors %.2f LBS\n",
             speedVal, torqueVal);

        m_MotorController.Write("LeftFrontMotorMode", SPEED_MODE, false);
        m_MotorController.Write("RightFrontMotorMode", SPEED_MODE, false);
        m_MotorController.Write("LeftRearMotorMode", TORQUE_MODE, false);
        m_MotorController.Write("RightRearMotorMode", TORQUE_MODE, false);
        m_MotorController.Write("LeftFrontSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), false);
        m_MotorController.Write("RightFrontSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), false);
        m_MotorController.Write("LeftRearTorqueValue", CreateMessage(temp, 256, "%.2f", torqueVal), false);
        m_MotorController.Write("RightRearTorqueValue", CreateMessage(temp, 256, "%.2f", torqueVal), true);
    }

    // wait the specified amount of time
    SetStartTime();
    while((GenericTC::StatusCheck() == BEP_STATUS_SUCCESS) && (testTime > 0) )
    {
        GetWheelSpeeds(wheelSpeeds);
        Log(LOG_DEV_DATA, "LF: %.02f, RF: %.02f, LR: %.02f, RR: %.02f\n", wheelSpeeds[LFWHEEL], wheelSpeeds[RFWHEEL],
            wheelSpeeds[LRWHEEL], wheelSpeeds[RRWHEEL]);
        BposSleep(100); // wait 1/10th of a second
        testTime -= 100;    // decrement the time from the total time
    }

    // Make sure we clear the speed/torque command values
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);

    // command the drives to zero torque
    Log(LOG_DEV_DATA, "Commanding torque to zero\n");
    SystemCommand(COMMAND_TORQUE, 0);

    // command the drives to zero speed
    Log(LOG_DEV_DATA, "Commanding speed to zero\n");
    SystemCommand(COMMAND_SPEED, 0);

    Log( LOG_DEV_DATA, "Waiting for zerospeed\n");
    // make sure the vehicle is at zerospeed
    while(TimeRemaining() && (ReadSubscribeData(GetDataTag("Zerospeed")) != "1"))
    {
        BposSleep (100);                // wait a tenth of a second
    }
    Log( LOG_DEV_DATA, "At zerospeed, check distances\n");

    // if and error did not occur and not aborted
    statusCheck = GenericTC::StatusCheck();
    if( statusCheck == BEP_STATUS_SUCCESS)
    {
        // read the current distance of the rear wheels
        GetWheelDistances( m_parkPawlDistances[ 1]);

        // Analyze the distances that the wheels travelled
        status = ValidateParkPawlTestDistances(m_parkPawlDistances[ 0], m_parkPawlDistances[ 1]);
    }
    else
    {
        status = ConvertStatusToResponse( statusCheck);
        Log(LOG_DEV_DATA, (GetTestStepName() + ": Failed Status Check\n").c_str());
    }

    SendTestResult( status, GetTestStepInfo("Description"));

    Log(LOG_FN_ENTRY, "Exit GenericTC::ParkPawlTorqueTest(%s), status=%s\n", direction.c_str(), status.c_str());

    return( status);
}

//=============================================================================
const string GenericTC::PerformPBTorqueTest(const std::string &direction, bool isTandemAxle) // = false)
{
    string  status;         // the current test status
    const int rollerCount = GetRollerCount();
    float   torqueVal = 
    ((GetParameterFloat("ParkBrakeApplyForce") != 0) ? GetParameterFloat("ParkBrakeApplyForce") : 350) * 0.5;
    float   speedVal = (GetParameterFloat("ParkBrakeSpeedValue") != 0) ? GetParameterFloat("ParkBrakeSpeedValue") : 5;


    string  frontMotorMode, rearMotorMode, tandemMotorMode;
    string  lfMotorValTag, rfMotorValTag;
    string  lrMotorValTag, rrMotorValTag;
    string  ltMotorValTag, rtMotorValTag;
    string  faultCode("0000");;
    float   wheelSpeeds[rollerCount];
    char    temp[256];
    int     testTime = GetTestStepInfoInt("TestTime");
    INT32   statusCheck;

    Log(LOG_FN_ENTRY, "Enter GenericTC::PerformPBTorqueTest(%s) - %s", direction.c_str(), 
        isTandemAxle ? "Tandem Test" : "Not Tandem Test");

    // Go to torque 0 first to clear any command history in the motor controller
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);
    if(isTandemAxle)
    {
        m_MotorController.Write(string("LeftTandemTorqueValue"), string("0.00"), false);
        m_MotorController.Write(string("RightTandemTorqueValue"), string("0.00"), false);
        m_MotorController.Write(string("LeftTandemSpeedValue"), string("0.00"), false);
        m_MotorController.Write(string("RightTandemSpeedValue"), string("0.00"), true);
    }

    BposSleep( 100);
    // Allow vehicle specific torque and speed values.
    torqueVal = GetVehicleParameter("ParkBrakeParameters/ParkBrakeApplyForce", torqueVal);
    speedVal  = GetVehicleParameter("ParkBrakeParameters/ParkBrakeSpeedValue", speedVal);

    // If we want the wheels to turn forward, we need a negative torque
    if( ((direction == "Forward") && (torqueVal > 0)) ||
        // If we want the wheels to turn in reverse, we need a positive torque value
        ((direction == "Reverse") && (torqueVal < 0)) )
    {
        Log( LOG_DEV_DATA, "Changing torque value from %.02f to %.02f\n", torqueVal, -torqueVal);
        torqueVal *= -1.0;
    }

    // If we want the wheels to turn forward, we need a positive speed
    if( ((direction == "Forward") && (speedVal < 0)) ||
        // If we want the wheels to turn in reverse, we need a negative speed
        ((direction == "Reverse") && (speedVal > 0)) )
    {
        Log( LOG_DEV_DATA, "Changing speed value from %.02f to %.02f\n", speedVal, -speedVal);
        speedVal *= -1.0;
    }

    // read the current distance of the wheels
    GetWheelDistances( m_parkBrakeDistances[ 0]);

    // Tell the motor to turn for the park brake test
    Log( LOG_DEV_DATA, "Commanding Front Motors to %.2f MPH and Rear Motors %.2f LBS\n",
         speedVal, torqueVal);

    m_MotorController.Write("LeftFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write("RightFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write("LeftRearMotorMode", TORQUE_MODE, false);
    m_MotorController.Write("RightRearMotorMode", TORQUE_MODE, false);
    m_MotorController.Write("LeftFrontSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), false);
    m_MotorController.Write("RightFrontSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), false);
    m_MotorController.Write("LeftRearTorqueValue", CreateMessage(temp, 256, "%.2f", torqueVal), false);
    m_MotorController.Write("RightRearTorqueValue", CreateMessage(temp, 256, "%.2f", torqueVal), true);
    if(isTandemAxle)
    {
        m_MotorController.Write(string("LeftTandemMotorMode"), string(TORQUE_MODE), false);
        m_MotorController.Write(string("RightTandemMotorMode"), string(TORQUE_MODE), false);
        m_MotorController.Write(string("LeftTandemTorqueValue"), CreateMessage(temp, 256, "%.2f", torqueVal), false);
        m_MotorController.Write(string("RightTandemTorqueValue"), CreateMessage(temp, 256, "%.2f", torqueVal), true);
    }


    // wait the specified amount of time
    SetStartTime();
    while((GenericTC::StatusCheck() == BEP_STATUS_SUCCESS) && (testTime > 0) )
    {
        GetWheelSpeeds(wheelSpeeds);
        Log(LOG_DEV_DATA, "LF: %f, RF: %f, LR: %f, RR: %f\n", wheelSpeeds[LFWHEEL], wheelSpeeds[RFWHEEL],
            wheelSpeeds[LRWHEEL], wheelSpeeds[RRWHEEL]);
        BposSleep(100);     // wait 1/10th of a second
        testTime -= 100;    // decrement the time from the total time
    }

    // Make sure we clear the speed/torque command values
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);
    if(isTandemAxle)
    {
        m_MotorController.Write(string("LeftTandemTorqueValue"), string("0.00"), false);
        m_MotorController.Write(string("RightTandemTorqueValue"), string("0.00"), false);
        m_MotorController.Write(string("LeftTandemSpeedValue"), string("0.00"), false);
        m_MotorController.Write(string("RightTandemSpeedValue"), string("0.00"), true);
    }


    // command the drives to zero torque
    Log(LOG_DEV_DATA, "commanding torque to zero\n");
    SystemCommand(COMMAND_TORQUE, 0);

    // command the drives to zero speed
    Log(LOG_DEV_DATA, "commanding speed to zero\n");
    SystemCommand(COMMAND_SPEED, 0);

    Log( LOG_DEV_DATA, "Waiting for zerospeed\n");
    // make sure the vehicle is at zerospeed
    while(TimeRemaining() && (ReadSubscribeData(GetDataTag("Zerospeed")) != "1"))
    {
        BposSleep (100);                // wait a tenth of a second
    }
    Log( LOG_DEV_DATA, "At zerospeed, check distances\n");

    // once stopped, check for pass/fail

    // if and error did not occur and not aborted
    statusCheck = GenericTC::StatusCheck();
    if( statusCheck == BEP_STATUS_SUCCESS)
    {
        // read the current distance of the rear wheels
        GetWheelDistances( m_parkBrakeDistances[ 1]);

        // Analyze the distances that the wheels travelled
        status = ValidateParkBrakeTestDistances(m_parkBrakeDistances[ 0], m_parkBrakeDistances[ 1], isTandemAxle);
    }
    else
    {
        Log(LOG_DEV_DATA, (GetTestStepName() + ": Failed Status Check\n").c_str());
        status = ConvertStatusToResponse( statusCheck);
        faultCode = GetFaultCode("ParkBrakeFault");
    }

    // Send the test result info
    SendTestResult( GetTestStepName(), status, GetTestStepInfo("Description"));

    Log(LOG_FN_ENTRY, "Exit GenericTC::PerformPBTorqueTest(%s), status=%s\n", direction.c_str(), status.c_str());

    return( status);
}

//=============================================================================
const string GenericTC::ComboTorqueTest(const std::string &direction)
{
    string      testStatus;     // the overall test status
    string      result;         // the current test status
    INT32       status = BEP_STATUS_SUCCESS;
    int         testTime, torqueValue;
    float       maxRearDistance, maxFrontDistance;
    const int   rollerCount = GetRollerCount();
    float       wheelSpeeds[rollerCount];
    WHEELINFO   totalDistance;
    float       torqueVal;

    Log(LOG_FN_ENTRY, "Enter GenericTC::ComboTorqueTest(%s)\n", direction.c_str());

    // load up the test parameters
    testTime = (GetTestStepInfoInt("TestTime") != 0) ? GetTestStepInfoInt("TestTime") : 3000;
    torqueValue = (GetTestStepInfoInt("TorqueValue") != 0) ? GetTestStepInfoInt("TorqueValue") : 350;
    maxRearDistance = GetTestStepInfoFloat("MaxRearDistance");
    maxFrontDistance = GetTestStepInfoFloat("MaxFrontDistance");

    // Go to torque 0 first to clear any command hiostory in the motor controller
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);
    BposSleep( 100);

    // read the current distance of the wheels
    GetWheelDistances(m_parkBrakeDistances[ 0]);
    m_parkPawlDistances[ 0] = m_parkBrakeDistances[ 0];
    // Make sure the we try to turn in the desired direction
    if( ((direction.compare("Reverse") == 0) && (torqueValue < 0)) ||
        ((direction.compare("Forward") == 0) && (torqueValue > 0)) )
    {
        torqueVal *= -1;
    }
    // command the drives to the desired torque
    SystemCommand(MOTOR_MODE, string(TORQUE_MODE));
    SystemCommand(COMMAND_TORQUE, torqueValue);

    // wait the specified amount of time
    SetStartTime();
    while((BEP_STATUS_SUCCESS == status) && (testTime > 0) )
    {
        GetWheelSpeeds(wheelSpeeds);
        Log(LOG_DEV_DATA, "LF: %f, RF: %f, LR: %f, RR: %f\n", wheelSpeeds[LFWHEEL], wheelSpeeds[RFWHEEL],
            wheelSpeeds[LRWHEEL], wheelSpeeds[RRWHEEL]);
        status = StatusSleep(100);      // wait 1/10th of a second
        testTime -= 100;    // decrement the time from the total time
    }

    // command the drives to zero torque
    Log(LOG_DEV_DATA, "commanding torque to zero\n");
    SystemCommand(COMMAND_TORQUE, 0);

    // command the drives to zero speed
    Log(LOG_DEV_DATA, "commanding speed to zero\n");
    SystemCommand(COMMAND_SPEED, 0);

    Log( LOG_DEV_DATA, "Waiting for zerospeed\n");
    // make sure the vehicle is at zerospeed
    while((BEP_STATUS_SUCCESS == status) && (TimeRemaining()) && (ReadSubscribeData(GetDataTag("Zerospeed")) != "1"))
    {
        status = StatusSleep(100);
    }
    Log( LOG_DEV_DATA, "At zerospeed, check distances\n");

    // if and error did not occur and not aborted
    if(status == BEP_STATUS_SUCCESS)
    {
        // read the current distance of the rear wheels
        GetWheelDistances(m_parkBrakeDistances[ 1]);
        m_parkPawlDistances[ 1] = m_parkBrakeDistances[ 1];

        // Check for roll-over / roll-under
        GetTotalDistances( totalDistance, m_parkBrakeDistances[ 0], m_parkBrakeDistances[ 1]);

        Log(LOG_DEV_DATA, "The distances traveled: %.2f, %.2f, %.2f, %.2f, Parameter MaxRear: %f, MaxFront: %f\n",
            totalDistance.lfWheel, totalDistance.rfWheel,
            totalDistance.lrWheel, totalDistance.rrWheel,
            maxRearDistance, maxFrontDistance);

        // Validate the distances for the drive axle
        result = ValidateTorqueTestDriveAxle( totalDistance.lrWheel, totalDistance.rfWheel,
                                              maxRearDistance, "StaticParkBrakeFault");
        UpdateResult( result, testStatus);

        // if the slave wheels are under the minimum allowed distance
        result = ValidateTorqueTestDriveAxle( totalDistance.lfWheel, totalDistance.rfWheel,
                                              maxFrontDistance, "ParkPawlFault");
        UpdateResult( result, testStatus);
    }
    else
    {
        UpdateResult( status, testStatus);
    }

    SendTestResult( GetTestStepName(), testStatus, GetTestStepInfo("Description"));

    // command the motor controller to boost mode
    SystemCommand(MOTOR_MODE, string(BOOST_MODE));

    Log(LOG_FN_ENTRY, "Exit GenericTC::ComboTorqueTest(%s), result=%s\n", direction.c_str(), testStatus.c_str());

    return(testStatus);
}

//=============================================================================
INT32 GenericTC::GetForces(float force[])
{
    INT32 testStatus = BEP_STATUS_FAILURE;  // overall status
    BEP_STATUS_TYPE openStatus = BEP_STATUS_ERROR;
    int retry = 3;  // the number of retrys to attempt

    Log(LOG_FN_ENTRY,(GetTestStepName() + ": GetForces()").c_str());
    // Allow only one thread to access SpeedFile at a time
    if(EOK == m_forceFileMutex.Acquire())
    {
        try
        {   // clear out the forces
            memset(force, 0, (sizeof(float) * GetRollerCount()));
            if(!IsForceFileOpen())
            {   // Open the force file
                openStatus = OpenForceFile();
                if(BEP_STATUS_SUCCESS != openStatus)
                {   // Error opening the force file
                    Log(LOG_ERRORS, "Error opening force file, not reading forces");
                }
            }
            // Ensure the file is open before we attempt to read
            if(IsForceFileOpen())
            {
                do
                {   // Read the force data
                    lseek(m_forceFile, 0, SEEK_SET);
                    UINT32 bytes = read(m_forceFile, (void *) force, (sizeof(float) * GetRollerCount()));
                    if(bytes != (sizeof(float) * GetRollerCount()))
                    {   // if interrupted during a read, read again
                        if(errno == EINTR)
                        {
                            Log(LOG_ERRORS, "Interrupted Trying To Read %s\n", GetParameter("CurrentForces").c_str());
                        }
                        else
                        {
                            Log(LOG_ERRORS, "Error Reading The File Information: %d, %s\n", bytes, strerror(errno));
                        }
                    }
                    else
                    {
                        testStatus = BEP_STATUS_SUCCESS;
                        if(GetRollerCount() == 4)
                            Log(LOG_DEV_DATA, "Read forces: %.2f, %.2f, %.2f, %.2f\n",
                                force[0], force[1], force[2], force[3]);
                        else
                            Log(LOG_DEV_DATA, "Read forces: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
                                force[0], force[1], force[2], force[3], force[4], force[5]);
                    }
                    // if the file could not be open, give up the time slice
                    if(testStatus != BEP_STATUS_SUCCESS)    BposSleep(1);
                }while( (testStatus != BEP_STATUS_SUCCESS) && (retry--));
                // release the file mutex
                m_forceFileMutex.Release();
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "GetForces Exception: %s\n", e.what());
            testStatus = BEP_STATUS_ERROR;
            // release the file mutex
            m_forceFileMutex.Release();
        }
    }
    else
    {
        Log(LOG_ERRORS, "ERROR: Unable to aquire force file mutex in GetForces: %s\n", errno);
        testStatus = BEP_STATUS_ERROR;
    }

    return(testStatus);
}

//=============================================================================
INT32 GenericTC::GetWheelDistances(float wheelDistance[])
{
    INT32 testStatus = BEP_STATUS_FAILURE;  // overall status
    BEP_STATUS_TYPE openStatus = BEP_STATUS_ERROR;
    int retry = 3;  // the number of retrys to attempt

    Log(LOG_FN_ENTRY,(GetTestStepName() + ": GetWheelDistances()").c_str());

    // Allow only one thread to access SpeedFile at a time
    if(EOK == m_wheelDistanceFileMutex.Acquire())
    {
        try
        {   // clear out the wheel distances
            memset(wheelDistance, 0, (sizeof(float) * GetRollerCount()));
            if(!IsWheelDistanceFileOpen())
            {   // Open the WheelDistance file
                openStatus = OpenWheelDistanceFile();
                if(BEP_STATUS_SUCCESS != openStatus)
                {   // Error opening the WheelDistance file
                    Log(LOG_ERRORS, "Error opening WheelDistance file, not reading wheel distances");
                }
            }
            // Ensure the file is open before we attempt to read
            if(IsWheelDistanceFileOpen())
            {
                do
                {
                    // Read the WheelDistance data
                    lseek(m_wheelDistanceFile, 0, SEEK_SET);
                    UINT32 bytes = read(m_wheelDistanceFile, (void *) wheelDistance,
                                        (sizeof(float) * GetRollerCount()));
                    if(bytes != (sizeof(float) * GetRollerCount()))
                    {   // if interrupted during a read, read again
                        if(errno == EINTR)
                        {
                            Log(LOG_ERRORS, "Interrupted Trying To Read %s\n",
                                GetParameter("CurrentDistances").c_str());
                        }
                        else
                        {
                            Log(LOG_ERRORS, "Error Reading The File Information: %d, %s\n", bytes, strerror(errno));
                        }
                    }
                    else
                    {
                        testStatus = BEP_STATUS_SUCCESS;
                        if(GetRollerCount() == 4)
                            Log(LOG_DEV_DATA, "Read distances: %.2f, %.2f, %.2f, %.2f\n",
                                wheelDistance[0], wheelDistance[1], wheelDistance[2], wheelDistance[3]);
                        else
                            Log(LOG_DEV_DATA, "Read distances: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
                                wheelDistance[0], wheelDistance[1], wheelDistance[2], 
                                wheelDistance[3], wheelDistance[4], wheelDistance[5]);
                    }
                    // if the file could not be open, give up the time slice
                    if(testStatus != BEP_STATUS_SUCCESS)    BposSleep(1);
                }while( (testStatus != BEP_STATUS_SUCCESS) && (retry--));
                // release the file mutex
                m_wheelDistanceFileMutex.Release();
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "GetWheelDistances Exception: %s\n", e.what());
            testStatus = BEP_STATUS_ERROR;
            // release the file mutex
            m_wheelDistanceFileMutex.Release();
        }
    }
    else
    {
        Log(LOG_ERRORS, "ERROR: Unable to aquire wheel distance file mutex in GetWheelDistances: %s\n", errno);
        testStatus = BEP_STATUS_ERROR;
    }

    return(testStatus);
}

//=============================================================================
INT32 GenericTC::GetWheelDistances(WHEELINFO &wheelDistance)
{
    INT32 testStatus = BEP_STATUS_FAILURE;  // overall status
    const UINT32 rollerCount = GetRollerCount();
    float distances[ rollerCount];

    Log(LOG_FN_ENTRY,
        (GetTestStepName() + ": GetWheelDistances()").c_str());

    memset( &wheelDistance, 0, sizeof( wheelDistance));
    testStatus = GetWheelDistances( distances);
    if( BEP_STATUS_SUCCESS == testStatus)
    {
        if( rollerCount > LFWHEEL)  wheelDistance.lfWheel = distances[ LFWHEEL];
        if( rollerCount > RFWHEEL)  wheelDistance.rfWheel = distances[ RFWHEEL];
        if( rollerCount > LRWHEEL)  wheelDistance.lrWheel = distances[ LRWHEEL];
        if( rollerCount > RRWHEEL)  wheelDistance.rrWheel = distances[ RRWHEEL];
        if( rollerCount > LTWHEEL)  wheelDistance.ltWheel = distances[ LTWHEEL];
        if( rollerCount > RTWHEEL)  wheelDistance.rtWheel = distances[ RTWHEEL];
    }

    return(testStatus);
}

//=============================================================================
const std::string GenericTC::ValidateParkPawlTestDistances(const WHEELINFO &startWheelDistance, 
                                                           const WHEELINFO &endWheelDistance,
                                                           bool isTandemAxle /*= false*/)
{
    string      testStatus = BEP_UNTESTED_STATUS;   // the overall test status
    string      status = BEP_PASS_STATUS;           // the current test status
    float       maxDistance, minDistance;
    WHEELINFO   totalDistance;
    float       leftMasterDist, rightMasterDist;
    float       leftSlaveDist, rightSlaveDist;
    string      leftDriveWheel, rightDriveWheel;
    char        temp[ 256];

    Log(LOG_FN_ENTRY, "Enter GenericTC::ValidateParkPawlTestDistances()\n");

    // update the total distance traveled
    GetTotalDistances( totalDistance, startWheelDistance, endWheelDistance);

    maxDistance = GetParameterFloat("ParkPawlMaxDistance");
    minDistance = GetParameterFloat("ParkPawlMinDistance");

    Log(LOG_DEV_DATA, "The distances traveled: %.2f, %.2f, %.2f, %.2f, Parameter Max: %.2f, Min: %.2f\n",
        totalDistance.lfWheel,totalDistance.rfWheel,totalDistance.lrWheel,totalDistance.rrWheel,
        maxDistance, minDistance);

    // Determine how to analyze based on drive axle
    if(ReadSubscribeData( GetDataTag("DriveAxle") )  == "Front")
    {
        leftMasterDist = totalDistance.lfWheel; rightMasterDist = totalDistance.rfWheel;
        leftSlaveDist = totalDistance.lrWheel;  rightSlaveDist = totalDistance.rrWheel;
        leftDriveWheel = "LeftFrontWheelDistance";
        rightDriveWheel = "RightFrontWheelDistance";
    }
    else
    {
        leftMasterDist = totalDistance.lrWheel; rightMasterDist = totalDistance.rrWheel;
        leftSlaveDist = totalDistance.lfWheel;  rightSlaveDist = totalDistance.rfWheel;
        leftDriveWheel = "LeftRearWheelDistance";
        rightDriveWheel = "RightRearWheelDistance";
    }

    // Validate the distances for the drive axle
    status = ValidateTorqueTestDriveAxle( leftMasterDist, rightMasterDist,
                                          maxDistance, "ParkPawlFault");
    UpdateResult( status, testStatus);

    // if the slave wheels are under the minimum allowed distance
    status = ValidateTorqueTestSlaveAxle( leftSlaveDist, rightSlaveDist,
                                          minDistance, "ParkPawlDriverInfluenceFault");
    UpdateResult( status, testStatus);

    // Send the test result info
    SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), "000",
                             "MaximumDistance", CreateMessage(temp, 256, "%.2f", maxDistance), "Pulses",
                             "MinimumDistance", CreateMessage(temp, 256, "%.2f", minDistance), "Pulses",
                             leftDriveWheel, CreateMessage(temp, 256, "%.2f", leftMasterDist), "Pulses",
                             rightDriveWheel, CreateMessage(temp, 256, "%.2f", rightMasterDist), "Pulses");

    Log(LOG_FN_ENTRY, "Exit GenericTC::ValidateParkPawlTestDistances(), status=%s\n", testStatus.c_str());

    return( testStatus);
}

//=============================================================================
const string GenericTC::ValidateParkBrakeTestDistances(const WHEELINFO &startWheelDistance, 
                                                       const WHEELINFO &endWheelDistance,
                                                       bool isTandemAxle /* = false*/)
{
    string      testStatus = BEP_UNTESTED_STATUS;   // the overall test status
    string      status = BEP_PASS_STATUS;           // the current test status
    float       maxDistance, minDistance;
    WHEELINFO   totalDistance;
    char        temp[ 256];

    Log(LOG_FN_ENTRY, "Enter GenericTC::ValidateParkBrakeTestDistances()\n");

    // update the total distance traveled
    GetTotalDistances( totalDistance, startWheelDistance, endWheelDistance);

    maxDistance = GetParameterFloat("ParkBrakeMaxDistance");
    minDistance = GetParameterFloat("ParkBrakeMinDistance");

    if(isTandemAxle)
    {
        Log(LOG_DEV_DATA, "The distances traveled: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, Parameter Max: %.2f, Min: %.2f\n",
            totalDistance.lfWheel,totalDistance.rfWheel,
            totalDistance.lrWheel,totalDistance.rrWheel,
            totalDistance.ltWheel,totalDistance.rtWheel,
            maxDistance, minDistance);   
    }
    else
    {
        Log(LOG_DEV_DATA, "The distances traveled: %.2f, %.2f, %.2f, %.2f, Parameter Max: %.2f, Min: %.2f\n",
            totalDistance.lfWheel,totalDistance.rfWheel,totalDistance.lrWheel,totalDistance.rrWheel,
            maxDistance, minDistance);
    }

    // Validate the distances for the drive axle
    status = ValidateTorqueTestDriveAxle( totalDistance.lrWheel, totalDistance.rrWheel,
                                          maxDistance, "ParkBrakeFault");
    UpdateResult( status, testStatus);

    if(isTandemAxle)
    {
        // Validate the distances for the tandem axle
        status = ValidateTorqueTestDriveAxle( totalDistance.ltWheel, totalDistance.rtWheel,
                                              maxDistance, "ParkBrakeFault");
        UpdateResult( status, testStatus);
    }

    // if the slave wheels are under the minimum allowed distance
    status = ValidateTorqueTestSlaveAxle( totalDistance.lfWheel, totalDistance.rfWheel,
                                          minDistance, "ParkBrakeDriverInfluenceFault");
    UpdateResult( status, testStatus);

    // Send the test result info
    if(isTandemAxle)
    {
        SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), "000",
                                 "MaximumDistance", CreateMessage(temp, 256, "%.2f", maxDistance), "Pulses",
                                 "LRWheelDistance", CreateMessage(temp, 256, "%.2f", totalDistance.lrWheel), "Pulses",
                                 "RRWheelDistance", CreateMessage(temp, 256, "%.2f", totalDistance.rrWheel), "Pulses",
                                 "LTWheelDistance", CreateMessage(temp, 256, "%.2f", totalDistance.ltWheel), "Pulses",
                                 "RTWheelDistance", CreateMessage(temp, 256, "%.2f", totalDistance.rtWheel), "Pulses");
    }
    else
    {
        SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), "000",
                                 "MaximumDistance", CreateMessage(temp, 256, "%.2f", maxDistance), "Pulses",
                                 "MinimumDistance", CreateMessage(temp, 256, "%.2f", minDistance), "Pulses",
                                 "LeftRearWheelDistance", CreateMessage(temp, 256, "%.2f", totalDistance.lrWheel), "Pulses",
                                 "RightRearWheelDistance", CreateMessage(temp, 256, "%.2f", totalDistance.rrWheel), "Pulses");

        Log(LOG_FN_ENTRY, "Exit GenericTC::ValidateParkBrakeTestDistances(), status=%s\n", testStatus.c_str());
    }
    return( testStatus);
}

//=============================================================================
void GenericTC::GetTotalDistances(WHEELINFO &totalDistance, 
                                  const WHEELINFO &startWheelDistance,
                                  const WHEELINFO &endWheelDistance)
{
    // update the total distance traveled
    for(INT32 wheel = LFWHEEL; wheel <= RTWHEEL; wheel++)
    {
        float *total = NULL;
        switch( wheel)
        {
        case LFWHEEL:
            totalDistance.lfWheel = endWheelDistance.lfWheel - startWheelDistance.lfWheel;
            total = &totalDistance.lfWheel;
            break;
        case RFWHEEL:
            totalDistance.rfWheel = endWheelDistance.rfWheel - startWheelDistance.rfWheel;
            total = &totalDistance.rfWheel;
            break;
        case LRWHEEL:
            totalDistance.lrWheel = endWheelDistance.lrWheel - startWheelDistance.lrWheel;
            total = &totalDistance.lrWheel;
            break;
        case RRWHEEL:
            totalDistance.rrWheel = endWheelDistance.rrWheel - startWheelDistance.rrWheel;
            total = &totalDistance.rrWheel;
            break;
        case LTWHEEL:
            totalDistance.ltWheel = endWheelDistance.ltWheel - startWheelDistance.ltWheel;
            total = &totalDistance.ltWheel;
            break;
        case RTWHEEL:
            totalDistance.rtWheel = endWheelDistance.rtWheel - startWheelDistance.rtWheel;
            total = &totalDistance.rtWheel;
            break;
        default:
            break;

        }

        if( total != NULL)
        {
            // Need to check for counter roll-over conditions
            if( *total < 0)
            {
                // Make delta positive for easier logic
                *total = 0 - *total;
                Log(LOG_DEV_DATA, "Negative distance detected, wheel %d\n", wheel);
            }

            // If total distance is extremely large, consider it to be a roll-under condition
            if( *total > (GenericTC::m_maxIcmCountValue /2))
            {
                // Counter rolled-under: delta = max - (end - start) + 1
                *total = GenericTC::m_maxIcmCountValue - *total + 1;
                Log(LOG_DEV_DATA, "Distance roll-under detected, wheel %d\n", wheel);
            }
        }
    }
}

//-----------------------------------------------------------------------------
float GenericTC::ConvertPulsesToMiles(const float &pulseCount)
{   // Get the circumference of the roller: C = 2*Pi*R
    float circumference = 2.0 * 3.1415927 * (GetParameterFloat("RollerDiameter")/2.0);  // This is in inches
    // Get the number of revolutions - this should be the number of pulses / pulses per revolution
    float revs = pulseCount / GetParameterFloat("PulsesPerRevolution");
    // Now should be a simple matter of multiplying the number of revolutions by the distance of 1 revolution
    float distInches = revs * circumference;
    // Now just need to convert inches into miles: 1 mile = 63360 inches
    return distInches / 63360.0;
}

//=============================================================================
const std::string 
GenericTC::ValidateTorqueTestDriveAxle(float leftDistance,
                                       float rightDistance,
                                       float maxDistance,
                                       const std::string &faultTag)
{
    string status = BEP_PASS_STATUS;        // the current test status

    Log(LOG_FN_ENTRY, "Enter GenericTC::ValidateTorqueTestDriveAxle()\n");

    // if the wheels are over the maximum allowed distance
    if((leftDistance > maxDistance) || (rightDistance > maxDistance))
    {
        //fail because the rear wheels turned beyond the max distance parameter
        Log(LOG_ERRORS, "Total distance %.2f or %.2f over max distance: %f\n",
            leftDistance, rightDistance, maxDistance);

        status = BEP_FAIL_STATUS;

        // Send the fault info
        SendSubtestResultWithDetail( GetTestStepName(),
                                     status, GetTestStepInfo("Description"), "0000",
                                     GetFaultCode(faultTag),
                                     GetFaultName(faultTag),
                                     GetFaultDescription(faultTag));
    }
    Log(LOG_FN_ENTRY, "Exit GenericTC::ValidateTorqueTestDriveAxle(), status=%s\n", status.c_str());

    return( status);
}

//=============================================================================
const std::string 
GenericTC::ValidateTorqueTestSlaveAxle(float leftDistance, 
                                       float rightDistance, 
                                       float minDistance, 
                                       const std::string &faultTag)
{
    string status = BEP_PASS_STATUS;        // the current test status

    Log(LOG_FN_ENTRY, "Enter GenericTC::ValidateTorqueTestSlaveAxle()\n");

    // if the wheels are under the minimum required distance
    if((leftDistance < minDistance) || (rightDistance < minDistance))
    {
        //fail because the rear wheels turned beyond the max distance parameter
        Log(LOG_ERRORS, "Total distance %.2f or %.2f under minimum distance: %f\n",
            leftDistance, rightDistance, minDistance);

        status = BEP_FAIL_STATUS;

        // Send the fault info
        SendSubtestResultWithDetail( GetTestStepName(),
                                     status, GetTestStepInfo("Description"), "0000",
                                     GetFaultCode(faultTag),
                                     GetFaultName(faultTag),
                                     GetFaultDescription(faultTag));
    }
    Log(LOG_FN_ENTRY, "Exit GenericTC::ValidateTorqueTestSlaveAxle(), status=%s\n", status.c_str());

    return( status);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::DisplayTimedPrompt(const std::string &promptTag, 
                                              const std::string &promptBox, 
                                              int priority, 
                                              INT32 promptTime,
                                              const string valueOne /*= ""*/,
                                              const string valueTwo /*= ""*/,
                                              const string valueThree /*= ""*/)
{
    return DisplayTimedPrompt(promptTag, BposReadInt(promptBox.c_str()), priority, promptTime, valueOne, valueTwo, valueThree);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::DisplayTimedPrompt(const std::string &promptTag, 
                                              const INT32 &promptBox, 
                                              INT32 priority, 
                                              INT32 promptTime,
                                              const string valueOne /*= ""*/,
                                              const string valueTwo /*= ""*/,
                                              const string valueThree /*= ""*/)
{
    BEP_STATUS_TYPE status = (BEP_STATUS_TYPE)StatusCheck();

    // Display the prompt
    DisplayPrompt(promptBox, promptTag, priority, "", valueOne, valueTwo, valueThree);

    // Give driver time to react
    status = StatusSleep( promptTime);

    // remove "release parking brake" prompt
    RemovePrompt(promptBox, promptTag);

    return( status);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::StatusSleep( INT32 sleepTime)
{
    BEP_STATUS_TYPE status = (BEP_STATUS_TYPE)StatusCheck();

    // Give driver time to react
    while((status == BEP_STATUS_SUCCESS) && (sleepTime > 0))
    {
        if( sleepTime > 100)    BposSleep(100);     // sleep 100 ms
        else if( sleepTime > 0) BposSleep( sleepTime);
        sleepTime -= 100;   // decrement the counter by 100 ms
        status = (BEP_STATUS_TYPE)StatusCheck();
    }

    return( status);
}

//=============================================================================
void GenericTC::SetICMManagerName(const string name)
{
    m_icmManagerName = name;
}

//=============================================================================
const string GenericTC::GetPreviousOverallResult(void)
{
    return(m_previousResult);
}

//=============================================================================
void GenericTC::SetPreviousOverallResult(const string result)
{
    m_previousResult = result;
}

//=============================================================================
std::string GenericTC::GetObjectiveResult(void)
{
    std::string localResult=BEP_ERROR_RESPONSE; // set the result to an error
    m_semResponse.CriticalSectionEnter();       // wait until the response to the last command has been sent
    if(m_result != "")                          // if the result is set, update
        localResult = m_result;
    m_result = "";                              // reset the result for the next result to be added
    m_semReady.CriticalSectionExit();           // signal that component ready for new command

    return(localResult);
}

//=============================================================================
void GenericTC::SetObjectiveResult(const std::string result)
{
    m_result = result;                      // set the result
    m_semResponse.CriticalSectionExit();    // say command ready
}

//=============================================================================
const std::string GenericTC::GetParameter(const std::string dataTag)
{
    std::string value, units;
    bool convertToEnglishUnits = false;
    try
    {
        units = m_parameters.getNode(dataTag)->getAttribute(BEP_UNITS)->getValue().c_str();
        convertToEnglishUnits = atob(m_parameters.getNode(dataTag)->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
    }
    catch(XmlException &e)
    {//units do not exist
        units = "";
        convertToEnglishUnits = false;
    }
    try
    {
        value = convertToEnglishUnits ? ConvertToEnglishUnits(units, m_parameters.getNode(dataTag)->getValue()) :
                m_parameters.getNode(dataTag)->getValue();
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "Invalid Parameter Requested: %s, %s\n", dataTag.c_str(), e.what());
        value.erase();
    }

    return(value);
}

//=============================================================================
std::string GenericTC::GetParameter( const std::string &dataTag, std::string &value) throw(BepException)
{
    // Will throw an exception if the desired dataTag does not exist in the parameters
    return(m_parameters.getNode(dataTag)->getValue());
}

//=============================================================================
INT32 GenericTC::GetParameter ( const std::string &dataTag, int &value) throw(BepException)
{
    std::string     strVal, units;
    bool convertToEnglishUnits = false;

    // Will throw an exception if the desired dataTag does not exist in the parameters
    value = BposReadInt( GetParameter( dataTag, strVal).c_str());
    try
    {
        units = m_parameters.getNode(dataTag)->getAttribute(BEP_UNITS)->getValue().c_str();
        convertToEnglishUnits = atob(m_parameters.getNode(dataTag)->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
    }
    catch(XmlException &e)
    {//units do not exist
        units = "";
        convertToEnglishUnits = false;
    }
    INT32 retVal = BposReadInt( GetParameter( dataTag, strVal).c_str());
    // Will throw an exception if the desired dataTag does not exist in the parameters
    value = convertToEnglishUnits ? ConvertToEnglishUnits(units, retVal) : 
            retVal;

    return( value);
}

//=============================================================================
float GenericTC::GetParameter ( const std::string &dataTag, float &value) throw(BepException)
{
    std::string     strVal, units;
    bool convertToEnglishUnits = false;

    // Will throw an exception if the desired dataTag does not exist in the parameters
    value = atof( GetParameter( dataTag, strVal).c_str());
    try
    {
        units = m_parameters.getNode(dataTag)->getAttribute(BEP_UNITS)->getValue().c_str();
        convertToEnglishUnits = atob(m_parameters.getNode(dataTag)->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
    }
    catch(XmlException &e)
    {//units do not exist
        units = "";
        convertToEnglishUnits = false;
    }
    float fltVal = atof( GetParameter( dataTag, strVal).c_str());
    // Will throw an exception if the desired dataTag does not exist in the parameters
    value = convertToEnglishUnits ? ConvertToEnglishUnits(units, fltVal) : 
            fltVal;

    return( value);
}

//=============================================================================
bool GenericTC::GetParameter ( const std::string &dataTag, bool &value) throw(BepException)
{
    std::string     strVal;
    std::string     parameter;
    // Will throw an exception if the desired dataTag does not exist in the parameters
    parameter = GetParameter( dataTag, strVal);
    if(parameter.length() > 0)
    {
        value = atob(parameter.c_str());
    }
    else
        value = false;

    return( value);
}

//=============================================================================
const INT32 GenericTC::GetParameterInt(const std::string dataTag)
{
    INT32 value = 0;
    string units;
    bool convertToEnglishUnits = false;

    try
    {
        units = m_parameters.getNode(dataTag)->getAttribute(BEP_UNITS)->getValue().c_str();
        convertToEnglishUnits = atob(m_parameters.getNode(dataTag)->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
    }
    catch(XmlException &e)
    {//units do not exist
        units = "";
        convertToEnglishUnits = false;
    }

    try
    {
        INT32 retVal = BposReadInt(m_parameters.getNode(dataTag)->getValue().c_str());
        value = convertToEnglishUnits ? ConvertToEnglishUnits(units, retVal) : 
                retVal;
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "Invalid Parameter Requested: %s, %s\n", dataTag.c_str(), e.what());
    }

    return(value);
}

//=============================================================================
const float GenericTC::GetParameterFloat(const std::string dataTag)
{
    float value = 0;
    string units;
    bool convertToEnglishUnits = false;

    try
    {
        units = m_parameters.getNode(dataTag)->getAttribute(BEP_UNITS)->getValue().c_str();
        convertToEnglishUnits = atob(m_parameters.getNode(dataTag)->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
    }
    catch(XmlException &e)
    {//units do not exist
        units = "";
        convertToEnglishUnits = false;
    }
    try
    {
        float fltVal = atof(m_parameters.getNode(dataTag)->getValue().c_str());
        value = convertToEnglishUnits ? ConvertToEnglishUnits(units, fltVal) : 
                fltVal;
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "Invalid Parameter Requested: %s, %s\n", dataTag.c_str(), e.what());
    }

    return(value);
}

//=============================================================================
const bool GenericTC::GetParameterBool(const std::string dataTag)
{
    bool value = false;
    try
    {
        string parameter = m_parameters.getNode(dataTag)->getValue();
        if(parameter.length() > 0)
        {
            value = atob( parameter.c_str());
        }
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "Invalid Parameter Requested: %s, %s\n", dataTag.c_str(), e.what());
    }

    return(value);
}

//=============================================================================
const float GenericTC::GetVehicleParameter(const string &parameterName, const float &defValue /*= 0.0*/)
{   // Get the vehicle build data
    XmlNode vehicleBuildRequest(VEHICLE_BUILD_TAG, "");
    XmlNode buildData("", "");
    std::string strVal;
    SystemRead(&vehicleBuildRequest, buildData);
    float value = defValue;
    string units;
    bool convertToEnglishUnits = false;
    try
    {
        value = atof(buildData.getChild(parameterName)->getValue().c_str());
        try
        {
            units = buildData.getChild(parameterName)->getAttribute(BEP_UNITS)->getValue().c_str();
            convertToEnglishUnits = atob(buildData.getChild(parameterName)->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
        }
        catch(XmlException &e)
        {//units do not exist
            units = "";
            convertToEnglishUnits = false;
        }
        value = convertToEnglishUnits ? ConvertToEnglishUnits(units, value) : 
                value;
    }
    catch(XmlException &excpt)
    {   // Parameter is not in the build data, try just the normal parameter list
        int startIndex = parameterName.find_last_of("/") + 1;
        try
        {
            //check if parameter exists
            GetParameter(parameterName.substr(startIndex != (int)(parameterName.npos) ? startIndex : 0),strVal);
            value = GetParameterFloat(parameterName.substr(startIndex != (int)(parameterName.npos) ? startIndex : 0));
        }
        catch(XmlException &excpt)
        {   // Parameter is not in the normal parameter list, try test step info
            value = GetTestStepInfoFloat(parameterName.substr(startIndex != (int)(parameterName.npos) ? startIndex : 0));
        }
    }
    // Delete the build data node since we are returned a copy
    return value;
}

//=============================================================================
const int GenericTC::GetVehicleParameter(const string &parameterName, const int &defValue /*= 0*/)
{   // Get the vehicle build data
    XmlNode vehicleBuildRequest(VEHICLE_BUILD_TAG, "");
    XmlNode buildData("", "");
    std::string strVal;
    SystemRead(&vehicleBuildRequest, buildData);
    int value = defValue;
    string units;
    bool convertToEnglishUnits = false;
    try
    {
        value = BposReadInt(buildData.getChild(parameterName)->getValue().c_str());
        try
        {
            units = buildData.getChild(parameterName)->getAttribute(BEP_UNITS)->getValue().c_str();
            convertToEnglishUnits = atob(buildData.getChild(parameterName)->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
        }
        catch(XmlException &e)
        {//units do not exist
            units = "";
            convertToEnglishUnits = false;
        }
        value = convertToEnglishUnits ? ConvertToEnglishUnits(units, value) : 
                value;
    }
    catch(XmlException &excpt)
    {   // Parameter is not in the build data, try just the normal parameter list
        int startIndex = parameterName.find_last_of("/") + 1;
        try
        {
            //check if parameter exists
            GetParameter(parameterName.substr(startIndex != (int)(parameterName.npos) ? startIndex : 0),strVal);
            value = GetParameterInt(parameterName.substr(startIndex != (int)(parameterName.npos) ? startIndex : 0));
        }
        catch(XmlException &excpt)
        {   // Parameter is not in the normal parameter list, try test step info
            value = GetTestStepInfoInt(parameterName.substr(startIndex != (int)(parameterName.npos) ? startIndex : 0));
        }
    }
    // Delete the build data node since we are returned a copy
    return value;
}

//=============================================================================
const string GenericTC::GetVehicleParameter(const string &parameterName, const string &defValue) // = "")
{   // Get the vehicle build data
    XmlNode vehicleBuildRequest(VEHICLE_BUILD_TAG, "");
    XmlNode buildData("", "");
    std::string strVal;
    SystemRead(&vehicleBuildRequest, buildData);
    string value(defValue);
    string units;
    bool convertToEnglishUnits = false;
    try
    {
        value = buildData.getChild(parameterName)->getValue();
        try
        {
            units = buildData.getChild(parameterName)->getAttribute(BEP_UNITS)->getValue().c_str();
            convertToEnglishUnits = atob(buildData.getChild(parameterName)->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
        }
        catch(XmlException &e)
        {//units do not exist
            units = "";
            convertToEnglishUnits = false;
        }
        value = convertToEnglishUnits ? ConvertToEnglishUnits(units, value) : 
                value;
    }
    catch(XmlException &excpt)
    {   // Parameter is not in the build data, try just the normal parameter list
        int startIndex = parameterName.find_last_of("/") + 1;
        try
        {
            //check if parameter exists
            GetParameter(parameterName.substr(startIndex != (int)(parameterName.npos) ? startIndex : 0),strVal);
            value = GetParameter(parameterName.substr(startIndex != (int)(parameterName.npos) ? startIndex : 0));
        }
        catch(XmlException &excpt)
        {   // Parameter is not in the normal parameter list, try test step info
            value = GetTestStepInfo(parameterName.substr(startIndex != (int)(parameterName.npos) ? startIndex : 0));
        }
    }
    // Delete the build data node since we are returned a copy
    return value;
}

//=============================================================================
const std::string GenericTC::GetTestStepName(void)
{
    std::string value;
    try
    {
        value = m_currentTestStep->getName();
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "TestStep Name Does Not Exist: %s\n", e.what());
        value.erase();
    }

    return(value);
}

//=============================================================================
std::string GenericTC::GetTestStepParameter( const std::string &dataTag, std::string &value, const std::string& defVal)
{
    std::string paramVal;

    try
    {
        // Look for the data in the test step info area first. This will allow
        //  customization of parameters on a per test step basis
        GetTestStepInfo(dataTag, value);
    }
    catch( BepException &err)
    {
        // Item not located in test step info. Check for it in the parameters (using
        //  non-exception throwing version of GetTestStepInfo*()
        paramVal = GetParameter(dataTag);

        // If parameter not found, use the default value
        if( true == paramVal.empty())   value = defVal;
        else                            value = paramVal;
    }

    return( value);
}

//=============================================================================
INT32 GenericTC::GetTestStepParameter( const std::string &dataTag, int &value, const int defVal)
{
    std::string paramVal;

    try
    {
        // Look for the data in the test step info area first. This will allow
        //  customization of parameters on a per test step basis
        GetTestStepInfo(dataTag, value);
    }
    catch( BepException &err)
    {
        // Item not located in test step info. Check for it in the parameters (using
        //  non-exception throwing version of GetTestStepInfo*()
        paramVal = GetParameter(dataTag);

        // If parameter not found, use the default value
        if( true == paramVal.empty())   value = defVal;
        else                            value = BposReadInt( paramVal.c_str());
    }

    return( value);
}

//=============================================================================
float GenericTC::GetTestStepParameter( const std::string &dataTag, float &value, const float defVal)
{
    std::string paramVal;

    try
    {
        // Look for the data in the test step info area first. This will allow
        //  customization of parameters on a per test step basis
        GetTestStepInfo(dataTag, value);
    }
    catch( BepException &err)
    {
        // Item not located in test step info. Check for it in the parameters (using
        //  non-exception throwing version of GetTestStepInfo*()
        paramVal = GetParameter(dataTag);

        // If parameter not found, use the default value
        if( true == paramVal.empty())   value = defVal;
        else                            value = atof( paramVal.c_str());
    }

    return( value);
}

//=============================================================================
bool GenericTC::GetTestStepParameter( const std::string &dataTag, bool &value, const bool defVal)
{
    std::string paramVal;

    try
    {
        // Look for the data in the test step info area first. This will allow
        //  customization of parameters on a per test step basis
        GetTestStepInfo(dataTag, value);
    }
    catch( BepException &err)
    {
        // Item not located in test step info. Check for it in the parameters (using
        //  non-exception throwing version of GetTestStepInfo*()
        paramVal = GetParameter(dataTag);

        // If parameter not found, use the default value
        if( true == paramVal.empty())   value = defVal;
        else                            value = atob( paramVal.c_str());
    }

    return( value);
}

const std::string GenericTC::ConvertToEnglishUnits( std::string &units, const std::string &value) throw(BepException)
{
    string resultString = value;
    if(units != "")
    {
        UINT32 spacePos = 0;
        char buffer[128];
        if((spacePos = value.find(' ')) != value.npos)
        {//if contains space
            try
            {
                float value1 = atof(value.substr(0,spacePos).c_str());
                float value2 = atof(value.substr(spacePos+1,value.length()-(spacePos+1)).c_str());
                value1 = ConvertToEnglishUnits(units,value1);
                value2 = ConvertToEnglishUnits(units,value2);
                //reassemble string
                resultString =  CreateMessage(buffer, sizeof(buffer),"%.5f", value1) + std::string(" ") + 
                                CreateMessage(buffer, sizeof(buffer),"%.5f", value2);
            }
            catch(...)
            {
                Log(LOG_ERRORS, "Error Converting String value to English Units\n");
            }
        }
        else
        {
            float value1 = atof(value.c_str());
            value1 = ConvertToEnglishUnits(units,value1);                 
            //reassemble string
            resultString =  CreateMessage(buffer, sizeof(buffer),"%.5f", value1);
        }
    }
    return resultString;
}

//=============================================================================
INT32 GenericTC::ConvertToEnglishUnits ( std::string &units, int &value) throw(BepException)
{
    if(units != "")
    {
        float fVal = (float) value;
        fVal = ConvertToEnglishUnits(units,fVal) + .5;
        value = (INT32) fVal;
    }
    return value;
}

//=============================================================================
float GenericTC::ConvertToEnglishUnits ( std::string &units, float &value) throw(BepException)
{
    if(units != "")
    {
        if(units == unitsKPH)
        {
            value *= (KPH_MPH); 
        }
        else if(units == unitsKGF)
        {
            value *= (KGF_LBS); 
        }
		else if(units == unitsKN)
		{
			value *= (KN_LBS);
		}
    }
    return value;
}


//=============================================================================
const std::string GenericTC::CreateTestResultName(const std::string name/* = ""*/)
{
    std::string value;

    if(name.empty())    value = GetComponentName() + GetTestStepName();
    else                value = GetComponentName() + name;

    return(value);
}

//=============================================================================
const std::string GenericTC::GetTestStepInfo(const std::string tag)
{
    std::string value, units;
    bool convertToEnglishUnits = false;
    try
    {
        units = m_currentTestStep->getAttributes().getNode(tag+BEP_UNITS)->getValue();
        convertToEnglishUnits = atob(m_currentTestStep->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
    }
    catch(XmlException &e)
    {//units do not exist
        units = "";
        convertToEnglishUnits = false;
    }
    try
    {
        Log(LOG_DETAILED_DATA, "Getting Description %s\n", tag.c_str());
        value = convertToEnglishUnits ? ConvertToEnglishUnits(units, m_currentTestStep->getAttributes().getNode(tag)->getValue()) : 
                m_currentTestStep->getAttributes().getNode(tag)->getValue();
        Log(LOG_DETAILED_DATA, "Got Description %s\n", tag.c_str());
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "Unable To Retrieve String %s From The TestStep Attributes: %s\n", tag.c_str(), e.what());
        value.erase();
    }

    return(value);
}

//=============================================================================
std::string GenericTC::GetTestStepInfo( const std::string &dataTag, std::string &value) throw(BepException)
{
    // Will throw an exception if the desired dataTag does not exist in the test step info
    value = m_currentTestStep->getAttributes().getNode(dataTag)->getValue();
    return( value);
}

//=============================================================================
int GenericTC::GetTestStepInfo( const std::string &dataTag, int &value) throw(BepException)
{
    std::string     strVal, units;
    bool convertToEnglishUnits = false;
    try
    {
        units = m_currentTestStep->getAttributes().getNode(dataTag+BEP_UNITS)->getValue();
        convertToEnglishUnits = atob(m_currentTestStep->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
    }
    catch(XmlException &e)
    {//units do not exist
        units = "";
        convertToEnglishUnits = false;
    }
    int retVal = BposReadInt( GetTestStepInfo( dataTag, strVal).c_str());
    // Will throw an exception if the desired dataTag does not exist in the test step info
    value = convertToEnglishUnits ? ConvertToEnglishUnits(units, retVal) : 
            retVal;

    return( value);
}

//=============================================================================
float GenericTC::GetTestStepInfo( const std::string &dataTag, float &value) throw(BepException)
{
    std::string     strVal, units;
    bool convertToEnglishUnits = false;
    try
    {
        units = m_currentTestStep->getAttributes().getNode(dataTag+BEP_UNITS)->getValue();
        convertToEnglishUnits = atob(m_currentTestStep->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
    }
    catch(XmlException &e)
    {//units do not exist
        units = "";
        convertToEnglishUnits = false;
    }
    float fltVal = atof( GetTestStepInfo( dataTag, strVal).c_str());
    // Will throw an exception if the desired dataTag does not exist in the test step info
    value = convertToEnglishUnits ? ConvertToEnglishUnits(units, fltVal): 
            fltVal;

    return( value);
}

//=============================================================================
bool GenericTC::GetTestStepInfo( const std::string &dataTag, bool &value) throw(BepException)
{
    std::string     strVal;

    // Will throw an exception if the desired dataTag does not exist in the test step info
    value = atob(GetTestStepInfo( dataTag, strVal).c_str());

    return( value);
}

//=============================================================================
const float GenericTC::GetTestStepInfoFloat(const std::string tag)
{
    float value=0;

    std::string     units;
    bool convertToEnglishUnits = false;
    try
    {
        units = m_currentTestStep->getAttributes().getNode(tag+BEP_UNITS)->getValue();
        convertToEnglishUnits = atob(m_currentTestStep->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
    }
    catch(XmlException &e)
    {//units do not exist
        units = "";
        convertToEnglishUnits = false;
    }
    try
    {
        float fltVal = atof(m_currentTestStep->getAttributes().getNode(tag)->getValue().c_str());
        value = convertToEnglishUnits ? ConvertToEnglishUnits(units, fltVal) : 
                fltVal;
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "Error Trying To Retrieve Float %s From The TestStep Attributes: %s\n", tag.c_str(), e.what());
    }

    return(value);
}

//=============================================================================
const INT32 GenericTC::GetTestStepInfoInt(const std::string tag)
{
    INT32 value=0;
    std::string     units;
    bool convertToEnglishUnits = false;
    try
    {
        units = m_currentTestStep->getAttributes().getNode(tag+BEP_UNITS)->getValue();
        convertToEnglishUnits = atob(m_currentTestStep->getAttribute(BEP_CONVERT_TO_ENG_UNITS)->getValue().c_str());
    }
    catch(XmlException &e)
    {//units do not exist
        units = "";
        convertToEnglishUnits = false;
    }
    try
    {
        INT32 retVal =  BposReadInt(m_currentTestStep->getAttributes().getNode(tag)->getValue().c_str());
        value = convertToEnglishUnits ? ConvertToEnglishUnits(units, retVal):
                retVal;
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "Error Trying To Retrieve Integer %s From The TestStep Attributes: %s\n", tag.c_str(), e.what());
    }

    return(value);
}

//=============================================================================
const bool GenericTC::GetTestStepInfoBool(const std::string tag)
{
    bool value=false;
    try
    {
        value = atob(m_currentTestStep->getAttributes().getNode(tag)->getValue().c_str());
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "Error Trying To Retrieve Boolean %s From The TestStep Attributes: %s\n", tag.c_str(), e.what());
    }

    return(value);
}

//=============================================================================
INT32 GenericTC::DisplayPrompt(const INT32 promptBox, 
                               const std::string prompt, 
                               INT32 priorityLevel, /*= 0*/ 
                               const std::string backgroundColor, /* = "" */
                               const std::string valueOne, /* = "" */ 
                               const std::string valueTwo, /* = "" */ 
                               const std::string valueThree) /* = "" */
{
    std::string response;
    INT32 status = EOK;

    if(!prompt.empty())
    {
        PromptDetails details;
        // if a first parameter exists, add it
        if(valueOne != "")
        {
            std::string paramOne("Param00");
            details.AddDetail(paramOne, valueOne);
        }
        // if a second parameter exists, add it
        if(valueTwo != "")
        {
            std::string paramTwo("Param01");
            details.AddDetail(paramTwo, valueTwo);
        }
        // if a third parameter exists, add it
        if(valueThree != "")
        {
            std::string paramThree("Param02");
            details.AddDetail(paramThree, valueThree);
        }
        status = m_prompt->DisplayPrompt(promptBox, prompt, details, response, priorityLevel);
        if(status == BEP_STATUS_SUCCESS)
        {
            if(!backgroundColor.empty())
            {
                status = m_prompt->SetBackgroundColor(promptBox, backgroundColor, response);
                if(status != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_ERRORS, "Unable to SetBackgroundColor(%d, %s)\n", promptBox, backgroundColor.c_str());
                }
            }
        }
        else
            Log(LOG_ERRORS, "Unable to DisplayPrompt(%d, %s)\n", promptBox, prompt.c_str());
    }
    else
        Log(LOG_ERRORS, "Error, trying to DisplayPrompt with a blank prompt");

    return(status);
}

//=============================================================================
INT32 GenericTC::DisplayPrompt(const std::string promptBox, 
                               const std::string prompt, 
                               INT32 priorityLevel, /*= 0*/ 
                               const std::string backgroundColor, /* = "" */
                               const std::string valueOne, /* = "" */ 
                               const std::string valueTwo, /* = "" */ 
                               const std::string valueThree) /* = "" */
{
    return(DisplayPrompt(atoi(promptBox.c_str()), prompt, priorityLevel, backgroundColor, valueOne, valueTwo, valueThree));
}

//=============================================================================
INT32 GenericTC::RemovePrompt(const INT32 promptBox, const std::string prompt, INT32 priorityLevel) /*= 0*/
{
    std::string response;
    INT32 status = m_prompt->RemovePrompt(promptBox, prompt, response, priorityLevel);
    return(status);
}

//=============================================================================
INT32 GenericTC::RemovePrompt(std::string promptBox, const std::string prompt, INT32 priorityLevel) /*= 0*/
{
    std::string response;
    INT32 status = m_prompt->RemovePrompt(atoi(promptBox.c_str()), prompt, response, priorityLevel);
    return(status);
}

//=============================================================================
INT32 GenericTC::SendTestResult(const std::string result, 
                                std::string description, /* = "" */ 
                                std::string code) /* = "0000"*/
{
    INT32 status = BEP_STATUS_ERROR;

    try
    {
        std::string testName = GetTestStepName();
        // if description is empty, use the current test step description
        if(description.empty()) description = GetTestStepInfo("Description");
        // if code is empty, use the current test step code
        if(code.empty())        code = GetTestStepInfo("Code");
        std::string response;
        status = m_testResult->WriteTestResult(CreateTestResultName(testName), result, 
                                               GetComponentName(), description, response, true, code);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s SendTestResult Error: %s\n", GetComponentName().c_str(), e.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "%s SendTestResult Unknown Error\n", GetComponentName().c_str());
    }

    return(status);
}

//=============================================================================
INT32 GenericTC::SendSubtestResult(const std::string &testName, 
                                   const std::string &result, 
                                   const std::string description, /* = "" */ 
                                   const std::string code) /* = "0000"*/
{
    INT32 status = BEP_STATUS_ERROR;

    try
    {
        std::string response;
        status = m_testResult->WriteTestResult(CreateTestResultName(testName), result, 
                                               GetComponentName(), description, response, true, code);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s SendSubtestResult Error: %s\n", GetComponentName().c_str(), e.what());
    }

    return(status);
}

//=============================================================================
INT32 GenericTC::SendSubtestResult(const std::string &testName, 
                                   const INT32 result, 
                                   const std::string description, /*= ""*/ 
                                   const std::string code) /* = "0000"*/
{
    std::string localResult="";
    UpdateResult(result, localResult);
    return(SendSubtestResult(testName, localResult, description, code));
}

//=============================================================================
const std::string GenericTC::GetComponentName(void)
{
    return(m_componentName);
}

//=============================================================================
void GenericTC::SetComponentName(const std::string componentName)
{
    m_componentName = componentName;
}

//=============================================================================
INT32 GenericTC::SetFault(const std::string faultName, const std::string description,
                          INT32 severity) /* = -1 */
{
    INT32 status = BEP_STATUS_ERROR;

    try
    {
        std::string response;
        status = m_fault->SetFault(faultName, description, GetComponentName(), response, true, severity);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s SetFault Error: %s\n", GetComponentName().c_str(), e.what());
    }

    return(status);
}

//=============================================================================
INT32 GenericTC::ClearFault(const std::string faultName)
{
    INT32 status = BEP_STATUS_ERROR;

    try
    {
        std::string response;
        status = m_fault->ClearFault(faultName, response, true);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s SetFault Error: %s\n", GetComponentName().c_str(), e.what());
    }

    return(status);
}

//=============================================================================
void GenericTC::SetStartTime(void)
{
    if(clock_gettime( CLOCK_REALTIME, &m_startTime) == -1)
    {
        throw BepException("SetStartTime Clock Gettime Error: %s", strerror(errno) );
    }
}

//=============================================================================
void GenericTC::SetSecondaryStartTime(void)
{
    if(clock_gettime( CLOCK_REALTIME, &m_secondaryStartTime) == -1)
    {
        throw BepException("SetSecondaryStartTime Clock Gettime Error: %s", strerror(errno) );
    }
}

//=============================================================================
void GenericTC::SetTestStepResult(const std::string result)
{
    XmlNode *testStepResult = const_cast< XmlNode * >(m_currentTestStep->getAttributes().getNode("Result"));
    testStepResult->setValue(result);
}

//=============================================================================
const std::string GenericTC::GetTestStepResult(void)
{
    std::string result;

    try
    {
        result = m_currentTestStep->getAttributes().getNode("Result")->getValue();
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s GetTestStepResult Error: %s\n", GetComponentName().c_str(), e.what());
        result = BEP_UNTESTED_RESPONSE;
    }

    return(result);
}

//=============================================================================
void GenericTC::SetCurrentTestStep(const XmlNode *testStep)
{
    m_currentTestStep = testStep;
}

//=============================================================================
const std::string GenericTC::GetOverallResult(void)
{
    return(m_overallResult);
}

//=============================================================================
void GenericTC::SetOverallResult(const std::string result)
{   // update the current component overall result
    Log(LOG_FN_ENTRY, "Setting Overall Result\n");
    UpdateResult(result, m_overallResult);
}

//=============================================================================
void GenericTC::ResetOverallResult(void)
{
    if(GetOverallResult() != BEP_PASS_STATUS)       // if not a previous pass
        UpdateTestStatus(TEST_STATUS_IN_PROGRESS);  // set the test status to in progress
    m_overallResult = BEP_TESTING_RESPONSE;         // reset the overall result
    SetStatus(BEP_TESTING_STATUS);                  // set the component status to testing
}

//=============================================================================
bool GenericTC::IsRetest(void)
{
    return(m_retest);
}

//=============================================================================
void GenericTC::SetRetestStatus(bool status)
{
    m_retest = status;
}

//=============================================================================
INT32 GenericTC::LockPrompts(void)
{
    return(m_prompt->LockPromptServer());
}

//=============================================================================
INT32 GenericTC::UnlockPrompts(void)
{
    return(m_prompt->UnlockPromptServer());
}

//=============================================================================
void GenericTC::SetFatalFailure(bool status)
{
    m_fatalFailure = status;
}

//=============================================================================
bool GenericTC::GetFatalFailure(void)
{
    return(m_fatalFailure);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::OpenForceFile(void)
{
    INT32 attempts = 3;
    Log(LOG_DEV_DATA, "File Open: %s\n", GetParameter("CurrentForces").c_str());
    do
    {
        m_forceFile = open(GetParameter("CurrentForces").c_str(), O_RDONLY);
        SetForceFileOpen(m_forceFile != -1);
    } while(!IsForceFileOpen() && attempts--);

    Log(LOG_ERRORS, "Force File Opened: %s", IsForceFileOpen() ? "Success" : "Failure");
    return(IsForceFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::CloseForceFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT32 closeAttempts = 3;
    if(IsForceFileOpen())
    {   // Force file is open, close it
        do
        {
            closeStatus = close(m_forceFile) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(closeStatus == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA, "Force File closed");
                SetForceFileOpen(false);
                m_forceFile = -1;
            }
            else
            {
                Log(LOG_DEV_DATA, "Error closing Force File: %s", strerror(errno));
            }
        } while(IsForceFileOpen() && closeAttempts--);
    }
    else
    {
        Log(LOG_DEV_DATA, "Force file not open, not attempting to close");
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return(closeStatus);
}

//=============================================================================
bool& GenericTC::IsForceFileOpen(void)
{
    return m_forceFileOpen;
}

//=============================================================================
void GenericTC::SetForceFileOpen(bool fileOpen)
{
    m_forceFileOpen = fileOpen;
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::OpenSpeedFile(void)
{
    INT32 attempts = 3;
    Log(LOG_DEV_DATA, "File Open: %s\n", GetParameter("CurrentSpeeds").c_str());
    do
    {
        m_speedFile = open(GetParameter("CurrentSpeeds").c_str(), O_RDONLY);
        SetSpeedFileOpen(m_speedFile != -1);
    } while(!IsSpeedFileOpen() && attempts--);

    Log(LOG_ERRORS, "Speed File Opened: %s", IsSpeedFileOpen() ? "Success" : "Failure");
    return(IsSpeedFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::CloseSpeedFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT32 closeAttempts = 3;
    if(IsSpeedFileOpen())
    {   // Speed file is open, close it
        do
        {
            closeStatus = close(m_speedFile) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(closeStatus == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA, "Speed File closed");
                SetSpeedFileOpen(false);
                m_speedFile = -1;
            }
            else
            {
                Log(LOG_DEV_DATA, "Error closing Speed File: %s", strerror(errno));
            }
        } while(IsSpeedFileOpen() && closeAttempts--);
    }
    else
    {
        Log(LOG_DEV_DATA, "Speed file not open, not attempting to close");
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return(closeStatus);
}

//=============================================================================
bool& GenericTC::IsSpeedFileOpen(void)
{
    return(m_speedFileOpen);
}

//=============================================================================
void GenericTC::SetSpeedFileOpen(bool fileOpen)
{
    m_speedFileOpen = fileOpen;
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::OpenWheelDistanceFile(void)
{
    INT32 attempts = 3;
    Log(LOG_DEV_DATA, "File Open: %s\n", GetParameter("CurrentDistances").c_str());
    do
    {
        m_wheelDistanceFile = open(GetParameter("CurrentDistances").c_str(), O_RDONLY);
        SetWheelDistanceFileOpen(m_wheelDistanceFile != -1);
    } while(!IsWheelDistanceFileOpen() && attempts--);

    Log(LOG_ERRORS, "WheelDistance File Opened: %s", IsWheelDistanceFileOpen() ? "Success" : "Failure");
    return(IsWheelDistanceFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE);
}

//=============================================================================
BEP_STATUS_TYPE GenericTC::CloseWheelDistanceFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT32 closeAttempts = 3;
    if(IsWheelDistanceFileOpen())
    {   // WheelDistance file is open, close it
        do
        {
            closeStatus = close(m_wheelDistanceFile) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(closeStatus == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA, "WheelDistance File closed");
                SetWheelDistanceFileOpen(false);
                m_wheelDistanceFile = -1;
            }
            else
            {
                Log(LOG_DEV_DATA, "Error closing WheelDistance File: %s", strerror(errno));
            }
        } while(IsWheelDistanceFileOpen() && closeAttempts--);
    }
    else
    {
        Log(LOG_DEV_DATA, "WheelDistance file not open, not attempting to close");
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return(closeStatus);
}

//=============================================================================
bool& GenericTC::IsWheelDistanceFileOpen(void)
{
    return(m_wheelDistanceFileOpen);
}

//=============================================================================
void GenericTC::SetWheelDistanceFileOpen(bool fileOpen)
{
    m_wheelDistanceFileOpen = fileOpen;
}

//=============================================================================
TaskMonClass_t GenericTC::GetTaskClass(void) const
{
    return( TASKMON_CLASS_COMPONENT);
}

//=============================================================================
void GenericTC::SetTestStepInfoValue(const std::string& infoTag, const float value)
{
    XmlNode* infoNode = NULL;
    char    valStr[ 128];

    Log( LOG_FN_ENTRY, "Enter GenericTC::SetTestStepInfoValue(%s, %.2f)\n", infoTag.c_str(), value);

    sprintf( valStr, "%.2f", value);
    try
    {
        infoNode = (XmlNode*)m_currentTestStep->getAttribute( infoTag);
        infoNode->setValue( valStr);
    }
    catch( XmlException &err)
    {
        infoNode = ((XmlNode*)m_currentTestStep)->addChild( infoTag, valStr, ATTRIBUTE_NODE);
    }

    Log( LOG_FN_ENTRY, "Exit GenericTC::SetTestStepInfoValue(%s, %.2f)\n", infoTag.c_str(), value);
}

//=============================================================================
void GenericTC::SetTestStepInfoValue(const std::string& infoTag, const int value)
{
    XmlNode* infoNode = NULL;
    char    valStr[ 128];

    Log( LOG_FN_ENTRY, "Enter GenericTC::SetTestStepInfoValue(%s, %d)\n", infoTag.c_str(), value);

    sprintf( valStr, "%d", value);
    try
    {
        infoNode = (XmlNode*)m_currentTestStep->getAttribute( infoTag);
        infoNode->setValue( valStr);
    }
    catch( XmlException &err)
    {
        infoNode = ((XmlNode*)m_currentTestStep)->addChild( infoTag, valStr, ATTRIBUTE_NODE);
    }

    Log( LOG_FN_ENTRY, "Exit GenericTC::SetTestStepInfoValue(%s, %d)\n", infoTag.c_str(), value);
}

//=============================================================================
void GenericTC::SetTestStepInfoValue(const std::string& infoTag, const std::string& value)
{
    XmlNode* infoNode = NULL;

    Log( LOG_FN_ENTRY, "Enter GenericTC::SetTestStepInfoValue(%s, %s)\n", infoTag.c_str(), value.c_str());

    try
    {
        infoNode = (XmlNode*)m_currentTestStep->getAttribute( infoTag);
        infoNode->setValue( value);
    }
    catch( XmlException &err)
    {
        infoNode = ((XmlNode*)m_currentTestStep)->addChild( infoTag, value, ATTRIBUTE_NODE);
    }

    Log( LOG_FN_ENTRY, "Exit GenericTC::SetTestStepInfoValue(%s, %s)\n", infoTag.c_str(), value.c_str());
}

//=============================================================================
const bool GenericTC::IsParkBrakeDirectionValid(const string &direction)
{
    bool directionValid = false;
    if(!direction.empty())
    {
        directionValid = (find(m_validParkBrakeDirections.begin(), m_validParkBrakeDirections.end(), direction) !=
                          m_validParkBrakeDirections.end());
        Log(LOG_DEV_DATA, "Park Brake Direction: %s is %s", direction.c_str(), directionValid ? "Valid" : "Not Valid");
    }
    else
    {
        Log(LOG_ERRORS, "No direction specified for park brake test!");
    }
    return(directionValid);
}

//=============================================================================
const std::string GenericTC::TestStepReadMemStats(const std::string &value)
{
    struct direct   **dirEnts;
    int             dirCount;
    uint32_t        ii;
    char            dirName[_POSIX_PATH_MAX+1];
    std::string     status( BEP_PASS_STATUS);

    strcpy( dirName, "/dev/TaskMon/Tasks/Component/");

    // Read all tasks under the Components directory
    dirCount = scandir( dirName, &dirEnts, NULL, alphasort);
    // if any entries were found
    if( dirCount > 0)
    {
        for( ii=0; ii<(uint32_t)dirCount; ii++)
        {
            // Try to read task information for this task
            if( EOK != UpdateTaskStats( dirEnts[ii]->d_name))
            {
                status = BEP_FAIL_STATUS;
            }

            free( dirEnts[ii]);
        }

        // Free the memory allocated by the library scandir() routine
        free( dirEnts);
    }

    // update the test result
    if(SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "GenericTC::TestStepReadMemStats Could Not Send Test Result: %s, %s\n", 
            GetTestStepName().c_str(), status.c_str());
    }

    return( status);
}

//=============================================================================
int GenericTC::UpdateTaskStats( const std::string &taskName, const char *prefix/*=NULL*/)
{
    std::string     taskDir("/dev/TaskMon/Tasks/Component/");
    std::string     taskPath( taskDir + taskName);
    int             retries = 3;
    TaskMonTask_t   newTask;
    char            data[ 1024];
    std::string     dataFile("/home/CCRT/Rewrite/TestResults/Mem_");
    int             dataFileFd;
    int             retVal = ENODEV;

    if(GetParameterBool("LogTaskStatInfo"))
    {
        int             taskFd = open( taskPath.c_str(), O_RDONLY);
        // If task FD is valid
        if( -1 != taskFd)
        {
            // Open new/append to old data file for this component
            dataFile = std::string("/home/CCRT/Rewrite/TestResults/Mem_") + taskName + std::string(".csv");
            if( 0 != access(dataFile.c_str(), F_OK))
            {
                // Open new data file to write new stats to
                dataFileFd = open( dataFile.c_str(), O_RDWR | O_CREAT, 0666);
                strcpy(data, "PID,State,Memory,Threads,FD,Comment\n");
            }
            else
            {
                // Open existing data file to append new stats to
                dataFileFd = open( dataFile.c_str(), O_RDWR | O_APPEND);
                data[0] = 0;
            }

            if( -1 != dataFileFd)
            {
                if( data[0] != 0)   write( dataFileFd, data, strlen( data));

                // Try to read task stats
                while( (retries-- > 0) && (retVal != EOK))
                {
                    // Make sure we read all the information
                    if( sizeof(newTask) == read( taskFd, &newTask, sizeof( newTask)))
                    {
                        retVal = EOK;
                    }
                }

                // Try to read task information for this task
                if( EOK == retVal)
                {
                    sprintf( data, "%d,%d,%lld,%d,%d", newTask.m_taskPid, newTask.m_taskState, 
                             newTask.m_memoryUsage, newTask.m_threads, newTask.m_fds);
                }
                else
                {
                    sprintf( data, "--,--,--,--,--");
                }
                dataFile = data;
                if( NULL != prefix)
                {
                    dataFile += std::string(",") + std::string( prefix);
                }
                dataFile += std::string( "\n");
                Log( "%s", dataFile.c_str());
                write( dataFileFd, (void*)dataFile.c_str(), dataFile.size());

                close( dataFileFd);
            }
            else
            {
                retVal = errno;
            }
            // Close current FD
            close( taskFd);
        }
    }
    else retVal = EOK;

    return( retVal);
}

//=============================================================================
string GenericTC::GetRootDir(void)
{
    Log(LOG_DEV_DATA, "GenericTC::GetRootDir() - Enter");
    string dcRootDir = SystemRead(GetDataTag("DriveCurveRootDir"));
    Log(LOG_DEV_DATA, "DriveCurveRootDir: %s ", dcRootDir.c_str());
    if((dcRootDir == "") || !dcRootDir.compare("Unavailable"))
    {
        dcRootDir = getenv("USR_ROOT");
    }
    Log(LOG_DEV_DATA, "GenericTC::GetRootDir() - Exit - returning %s", dcRootDir.c_str());
    return dcRootDir;
}

//=============================================================================
INamedDataBroker* GenericTC::GetNamedDataBroker(void)
{
    return m_ndb;
}

//=============================================================================
void GenericTC::ProcessParkBrakeHandleForce(void)
{
    // if selected, store the park brake handle force result
    if(GetParameterBool("StoreParkBrakeHandleForce"))
    {
        // report the test result (there is no evaluation)
        SendSubtestResultWithDetail("ParkBrakeHandleForce",BEP_STATUS_SUCCESS,"Park Brake Handle Force","0000",
                                    "ParkBrakeHandleForce",ReadSubscribeData("ParkBrakeHandleForce"),"LBF");
    }
}
