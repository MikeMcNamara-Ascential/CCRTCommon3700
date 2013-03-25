//******************************************************************************
// Description:
//  Generic Test Component.
//
//==============================================================================
// Copyright (c) 2004-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/Common/GenericTCTemplate.cpp $
// 
// 14    10/30/07 1:44p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added a try-catch block around the static test result server
// intialization.
// 
// 13    5/01/07 11:10a Rwiersem
// Changes for the 07050201 core release:
// 
// Removed #if 0 blocks.  Broke single lines that were longer than the
// viewable screen area into multiple lines.
// 
// 12    2/22/07 8:55a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added m_staticTesterServerComm.
// - Added clear of m_moduleDataItems, m_validPartNumbers, and m_ignoreFaults 
//   in the destructor.
// - Added a LoadStaticTestResults configuration parameter.
// - Added LoadStaticTestResults().
// 
// 11    1/17/07 3:27p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added m_abortCalled.
// - Added Activate().
// - Added ReloadConfiguration().
// - Only call Uninitialize() in ShouldReloadConfigFile() if the configuration 
//   file has changed.
// - Set the member variables to their default values in WarmInitialize().
// - Only call OpenCom() in WarmInitialize() if the parameter 
//   DelayModuleInitialization is false.
// - Use m_abortCalled in Publish() to make sure the abort flag is only set 
//   once.
// - If the module interface is already initialized, just call OpenCom() in 
//   InitializeModuleInterface().
// - Added StartModuleKeepAlive() to add the ability to send different keep 
//   alive messages.
// - Use m_abortCalled in Abort() to make sure the abort flag is only set once.
// 
// 10    10/25/06 3:54p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added m_protocolFile and m_protocolFileTime to hold the protocol file 
//       name and last time it was updated.
//     - Added Deactivate(), ShouldReloadConfigFile(), and WarmInitialize().  
//       These methods are used to keep the test components running when a new 
//       drive curve starts.
// 
// 9     7/05/06 9:54p Cward
// Added support for reading, recording, and reporting of module data.
//
// 8     5/08/06 7:08p Cward
// Added wait for engine off key on method
//
// 7     3/30/06 1:46a Cward
// Updated to add the capability to enter diagnostic mode without a
// response
//
// 6     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 5     7/20/05 8:37p Cward
// Added call to base class method in Command Test Step
//
// 4     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 2     12/22/04 8:12p Bmeinke
// Added test step handlers for EnableMotorLoading, DisableMotorLoading,
// ParkBrake, ParkPawl, ParkBrakePawl, HydraulicParkBrake,
// ElectricParkBrake, AnalyzeParkBrake and AnalyzeParkPawl
//
// 2     9/01/04 9:05p Cward
// Added parameter to allow keep alive to always be started.  Added
// parameter to allow diagnostic mode to always be entered.
//
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <string>
#include <algorithm>
#include "IGenericModule.h"
#include "GenericTC.h"
#include "GenericTCTemplate.h"
#include "ModuleException.h"

//-----------------------------------------------------------------------------
template <class ModuleType>
GenericTCTemplate<ModuleType>::GenericTCTemplate(void) : GenericTC(),
    m_communicationFailure(false), m_initializeHookCalled(false), m_initializeCalled(false),
    m_protocolFile(""), m_protocolFileTime( 0), m_abortCalled(false), m_faultsInModule(false), 
    m_staticTesterServerComm(NULL)
{
}

//-----------------------------------------------------------------------------
template <class ModuleType>
GenericTCTemplate<ModuleType>::~GenericTCTemplate(void)
{   // Clear out the Xml Data Maps
    m_moduleDataItems.clear(true);
    m_partNumberPositionComparison.clear(true);
    m_partNumbersToCheck.clear(true);
    m_validPartNumbers.clear(true);
    m_ignoreFaults.clear(true);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void GenericTCTemplate<ModuleType>::Initialize(const string &fileName)
{   // Just call the base class to perfrom the initialization
    GenericTC::Initialize(fileName);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void GenericTCTemplate<ModuleType>::Initialize(const XmlNode *config)
{
    try
    {
        m_initializeCalled = true;
        Log(LOG_DEV_DATA, (GetComponentName() + ": calling GenericTC::Initialize").c_str());
        GenericTC::Initialize(config);
        // Call the child class initialize hook
        Log(LOG_DEV_DATA, (GetComponentName() + ": calling InitializeHook()").c_str());
        InitializeHook(config);
        // Call the initialize complete method
        Log(LOG_DEV_DATA, (GetComponentName() + ": calling InitializationComplete()").c_str());
        InitializationComplete();
        // Reset the data tags
        ResetSystemTags();
        Log(LOG_FN_ENTRY, (GetComponentName() + ": Initialize: EXIT").c_str());
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "XML Error Initializing Component %s: %s\n", GetProcessName().c_str(), e.what());
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Error Initializing Component %s: %s\n", GetProcessName().c_str(), e.what());
    }
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void GenericTCTemplate<ModuleType>::Deactivate(void)
{
    // Ensure the module interface has been initialized before attempting to close comms.
    if(m_vehicleModule.IsInitialized())  m_vehicleModule.CloseCom();
    GenericTC::Deactivate();

    m_communicationFailure = false;
    m_faultsInModule = false;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void GenericTCTemplate<ModuleType>::ReloadConfiguration(void)
{
    Log(LOG_FN_ENTRY, "GenericTCTemplate::ReloadConfiguration()\n");
    //Clear all items that were copied
    m_ignoreFaults.clear(true);
    m_validPartNumbers.clear(true);
    m_moduleDataItems.clear(true);
    m_partNumberPositionComparison.clear(true);
    m_partNumbersToCheck.clear(true);
    //call baseclass
    GenericTC::ReloadConfiguration();
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::ReloadConfig() done\n");
}

//-----------------------------------------------------------------------------
template <class ModuleType>
bool GenericTCTemplate<ModuleType>::ShouldReloadConfigFile(void)
{
    bool shouldReload = false;

    // See if the main config file has changed
    shouldReload = GenericTC::ShouldReloadConfigFile();
    // IF the main config file hasn't changed, check our protocol file
    if( false == shouldReload)
    {
        // Check if our protocol file has changed
        shouldReload = FileHasChanged( m_protocolFile, m_protocolFileTime);
    }

    // If any of the config files have changed
    if( true == shouldReload)
    {
        // Tell the module interface that it needs to be re-initialized
        m_vehicleModule.Uninitialize();
    }

    return( shouldReload);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void GenericTCTemplate<ModuleType>::WarmInitialize(void)
{
    Log( LOG_FN_ENTRY, "Enter GenericTCTemplate::WarmInitialize()\n");
    //initialize member variables
    m_communicationFailure = false;
    m_faultsInModule = false;
    m_abortCalled = false;
    // Call the base class to complete initialization
    GenericTC::WarmInitialize();

    if(GetParameterBool("DelayModuleInitialization") == false)
    {
        m_vehicleModule.OpenCom();
    }

    Log( LOG_FN_ENTRY, "Exit GenericTCTemplate::WarmInitialize()\n");
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const string GenericTCTemplate<ModuleType>::CommandTestStep(const string &value)
{
    string status;

    try
    {   // Get the test step name to perform
        string step = GetTestStepName();
        Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Entering test step %s\n",
            GetComponentName().c_str(), value.c_str(), step.c_str());
        // Check the status to ensure it is alright to run this test step
        if(StatusCheck() != BEP_STATUS_SUCCESS)
        {   // Not OK to run the test step
            UpdateResult(StatusCheck(), status);
            Log(LOG_ERRORS, (GetComponentName() +
                             ".CommandTestStep: StatusCheck() failed: %s\n").c_str(),status.c_str());
            // Set status to Skip since this test step will not be performed
            status = testSkip;
            SendTestResult(testSkip, GetTestStepInfo("Description"));
        }
        // Setup the module for testing
        else if(step == "Setup")                     status = Setup();
        // Place the module into diagnostic mode
        else if(step == "EnterDiagnosticMode")       status = EnterDiagnosticMode();
        // Command the module to exit diagnostic mode
        else if(step == "ExitDiagnosticMode")        status = ExitDiagnosticMode();
        // Validate the module part number
        else if(step == "CheckPartNumber")           status = CheckPartNumber();
        // Validate the module part number
        else if(step == "CheckPartNumbers")           status = CheckPartNumbers();
        // Lock the module
        else if(step == "LockModule")                status = LockModule();
        // Unlock access to the module
        else if(step == "UnLockModule")              status = UnLockModule();
        // Unlock security to the module
        else if(step == "CommandSecurityUnlock")     status = CommandSecurityUnlock();
        // Vlaidate the VIN stored in the module
        else if(step == "CheckVIN")                  status = CheckVIN();
        // Program the VIN into the module
        else if(step == "ProgramVIN")                status = ProgramVIN();
        // Clear faults from the module
        else if(step == "ClearFaults")               status = ClearFaults();
        // read faults from the module
        else if(step == "ReadFaults")                status = ReadFaults();
        // Collect traceability data from the module
        else if(step == "CollectTraceData")          status = CollectTraceData();
        // Start sending module keep alive messages
        else if(step == "StartModuleKeepAlive")      status = StartModuleKeepAlive();
        // Start sending module keep alive messages
        else if(step == "StartSpecialKeepAlive")     status = StartSpecialKeepAlive(value);
        // Stop sending module keep alive messages
        else if(step == "StopModuleKeepAlive")       status = StopModuleKeepAlive();
        // Stop all background data monitors
        else if(step == "StopBackgroundMonitors")    status = StopBackgroundMonitors();
        // Resume all background data monitors
        else if(step == "ResumeBackgroundMonitors")  status = ResumeBackgroundMonitors();
        // Monitor the module for faults
//        else if (step == "MonitorFaults")             status = MonitorFaults();
        // Run the Cal progrma on the module
//        else if (step == "CalProgram")                status = CalProgram();
        // Report info read from the module
        else if(step == "ReportModuleInfo")          status = ReportModuleInfo();
        // Wait for the Engine to be not running with in On position
        else if(step == "WaitForEngineOffKeyOn")     status = WaitForEngineOffKeyOn();
        //Engine off key locked then prompts for key on 
        else if(step == "KeyOffEngineOffKeyOn")       status = KeyOffEngineOffKeyOn();
        // Perform a power-on reset of the module
        else if (step == "ResetModule")               status = ResetModule();
        // Cleanup at the end of the test cycle
        else if(step == "FinishUp")
        {
            status = FinishUp();
            SendOverallResult(status);
        }
        // Unknown test step
        else
        {
            status = GenericTC::CommandTestStep(value);
        }
    }
    catch (ModuleException &caughtModuleException)
    {
        Log(LOG_ERRORS, "%s.%s: %s", GetComponentName().c_str(),
            GetTestStepName().c_str(), caughtModuleException.message().c_str());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }
    catch(BepException &e)
    {
        Log(LOG_DEV_DATA, "%s CommandTestStep %s Exception: %s\n",
            GetComponentName().c_str(), GetTestStepName().c_str(), e.what());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }
    Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Returning %s\n",
        GetComponentName().c_str(), value.c_str(), status.c_str());
    return(status);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void GenericTCTemplate<ModuleType>::SetCommunicationFailure(bool status)
{
    m_communicationFailure = status;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
bool GenericTCTemplate<ModuleType>::GetCommunicationFailure(void)
{
    return(m_communicationFailure);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void GenericTCTemplate<ModuleType>::Reset(void)
{
    m_vehicleModule.StopVehicleComms(false);
    GenericTC::Reset();
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const string GenericTCTemplate<ModuleType>::Publish(const XmlNode *node)
{   // Check if testing was aborted
    if (node != NULL) 
    {
        try 
        {
            if ((node->getName() == ABORT_DATA_TAG) && (node->getValue() == "1") && !m_abortCalled)
            {   //set flag indicating abort called
                m_abortCalled = true;
                // Need to stop any monitors that were started
                StopBackgroundMonitors();
                m_abortCalled = false;
            }
        } 
        catch (XmlException &excpt) 
        {
            Log(LOG_ERRORS, "XmlException during GenericTCTemplate::Publish() - %s", excpt.GetReason());
        }
    }
    // Pass the publish to the base class for further handling
    return BepServer::Publish(node);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
inline string GenericTCTemplate<ModuleType>::Setup(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if(!ShortCircuitTestStep() && (GetTestStepResult() != testPass))
    {
        if(GetParameterBool("DelayModuleInitialization"))
        {
            status = InitializeModuleInterface();
            Log(LOG_DEV_DATA, "Initialize Module Interface: status - %s", ConvertStatusToResponse(status).c_str());
        }
        else
        {
            Log(LOG_DEV_DATA, "Module Interface already initialized.");
            status = BEP_STATUS_SUCCESS;
        }
    }
    else
    {
        Log(LOG_DEV_DATA, "Skipping test step: %s", GetTestStepName().c_str());
        status = BEP_STATUS_SUCCESS;
    }
    if(m_vehicleModule.IsInitialized())
    {   // clear the stop comms flag
        m_vehicleModule.StopVehicleComms(false);
    }
    return((status == BEP_STATUS_SUCCESS) ? testPass : testFail);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
inline string GenericTCTemplate<ModuleType>::FinishUp(void)
{
    SendTestResult(testPass, GetTestStepInfo("Description"), "0000");
    return(testPass);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
inline string GenericTCTemplate<ModuleType>::StopBackgroundMonitors(void)
{   // Stop the keep alive monitor
    return BEP_STATUS_SUCCESS == m_vehicleModule.StopModuleKeepAlive() ? testPass : testFail;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
inline string GenericTCTemplate<ModuleType>::ResumeBackgroundMonitors(void)
{   // Start any background monitors that were stopped
    return BEP_STATUS_SUCCESS == m_vehicleModule.StartModuleKeepAlive() ? testPass : testFail;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
INT32 GenericTCTemplate<ModuleType>::StatusCheck(void)
{
    INT32 status = BEP_STATUS_SUCCESS;
    if(GetParameterBool("DeveloperTest") != true)
    {
        status = GenericTC::StatusCheck();
    }
    return status;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void GenericTCTemplate<ModuleType>::InitializeHook(const XmlNode *config)
{
    // Log the function entry
    Log(LOG_FN_ENTRY, "Entering GenericTCTemplate::InitializeHook\n");
    // Set the flag that we've been called
    m_initializeHookCalled = true;
    // Get the list of module faults to ignore
    try
    {
        m_ignoreFaults.DeepCopy(config->getChild("Setup/IgnoreFaults")->getChildren());
    }
    catch(BepException &err)
    {   // Could not get list of faults to ignore
        Log("XML Exception loading ignored faults list - %s\n", err.Reason().c_str());
        m_ignoreFaults.clear(true);
    }
    // Get the list of valid module part numbers
    try
    {
        m_validPartNumbers.DeepCopy(config->getChild("Setup/Parameters/ValidPartNumbers")->getChildren());
    }
    catch(BepException &err)
    {   // Could not get list of valid part numbers
        Log("XML Exception loading valid part number list - %s\n", err.Reason().c_str());
        m_validPartNumbers.clear(true);
    }
    //Get the list of Module data items to record
    try
    {
        m_moduleDataItems.DeepCopy(config->getChild("Setup/Parameters/ModuleDataItems")->getChildren());
    }
    catch(BepException &err)
    {   // Error loading Module data items
        Log("XML Exception loading module data items to record - %s\n", err.Reason().c_str());
        m_moduleDataItems.clear(true);
    }
    //Get the list of pn comparison characters
    try
    {
        m_partNumberPositionComparison.DeepCopy(config->getChild("Setup/Parameters/PartNumberPositionComparison")->getChildren());
    }
    catch(BepException &err)
    {   // Error loading Module data items
        Log("XML Exception loading part number comparison positions to check - %s\n", err.Reason().c_str());
        m_partNumberPositionComparison.clear(true);
    }

    //Get the list of pns to check
    try
    {
        m_partNumbersToCheck.DeepCopy(config->getChild("Setup/Parameters/PartNumbersToCheck")->getChildren());
    }
    catch(BepException &err)
    {   // Error loading Module data items
        Log("XML Exception loading part numbers to check - %s\n", err.Reason().c_str());
        m_partNumbersToCheck.clear(true);
    }
    // Initialize the module interface
    if(!GetParameterBool("DelayModuleInitialization"))
    {
        InitializeModuleInterface();
    }
    // Determine if static testing results should be loaded
    if(GetParameterBool("LoadStaticTestResults"))
    {
        try
        {
            // Create a new comm object to the static tester server
            Log(LOG_DEV_DATA, "Loading static test results...");
            if (m_staticTesterServerComm == NULL)
            {
                Log(LOG_DEV_DATA, "Creating comm object for %s...", GetDataTag("StaticTesterName").c_str());
                m_staticTesterServerComm = new IBepCommunication();
                m_staticTesterServerComm->Initialize(GetDataTag("StaticTesterName"), "Client", IsDebugOn(), 5000);
            }
            // Load the static tester results
            LoadStaticTestResults();
        }
        catch (BepException &e)
        {
            Log(LOG_ERRORS, "Error Loading Static Test Results %s: %s\n", GetProcessName().c_str(), e.what());
        }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exiting GenericTCTemplate::InitializeHook\n");
}

//-----------------------------------------------------------------------------
template <class ModuleType>
BEP_STATUS_TYPE GenericTCTemplate<ModuleType>::InitializeModuleInterface(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // If the module interface is already initialized
    if( m_vehicleModule.IsInitialized() == true)
    {
        // just re-open it
        m_vehicleModule.OpenCom();
    }
    // check if FTP_ROOT exists
    else if (GetRootDir() != "")
    {
        Log(LOG_DEV_DATA, "GTCT::InitializeModuleInterface: FTP_ROOT is good...");
        XmlParser parser;
        const XmlNode *document;
        string ftpRoot = GetRootDir();
        if(ftpRoot.length() > 0)
        {
            Log(LOG_DEV_DATA, ("Loading module config file: "+ ftpRoot + GetParameter("ConfigFile")).c_str());
            // initialize the vehicle module interface
            try
            {
                if( false == GetParameter("ConfigFile").empty())
                {
                    m_protocolFile = ftpRoot + GetParameter("ConfigFile");
                    document = parser.ReturnXMLDocument( m_protocolFile);
                    m_protocolFileTime = GetFileModificationTime( m_protocolFile);
                }
                else
                {
                    throw( BepException( "Empty protocol file specified in ConfigFile parameter"));
                }
            }
            catch(BepException &err)
            {
                Log(LOG_ERRORS, "BepException in InitializeModuleInterface(): %s\n", err.GetReason());
                status = BEP_STATUS_SOFTWARE;
                throw BepException ("Error opening FTP_ROOT/ConfigFile: " + err.Reason());
            }

            if(document != NULL)
            {
                Log(LOG_DEV_DATA, "GTCT::InitializeModuleInterface: About to call module init");
                try
                {
                    m_vehicleModule.Initialize(document);
                    m_vehicleModule.m_driveCurveRewriteDir = ftpRoot;
                }
                catch(XmlException &ex)
                {
                    status = BEP_STATUS_SOFTWARE;
                    Log(LOG_ERRORS, "XmlException in GenericTCTemplate::InitializeModuleInterface() - %s\n", ex.GetReason());
                }
                catch(BepException &ex)
                {
                    status = BEP_STATUS_SOFTWARE;
                    Log(LOG_ERRORS, "BepException in GenericTCTemplate::InitializeModuleInterface() - %s\n", ex.GetReason());
                }
                catch(...)
                {
                    status = BEP_STATUS_SOFTWARE;
                    Log(LOG_ERRORS, "Unknown Exception in GenericTCTemplate::InitializeModuleInterface()\n");
                }
            }
            else
            {
                status = BEP_STATUS_SOFTWARE;
                Log(LOG_ERRORS,"Parser returned null for document");
            }
        }
        else
        {
            status = BEP_STATUS_SOFTWARE;
            Log(LOG_ERRORS,"environment variable FTP_ROOT string extraction error");
        }
    }
    else
    {
        status = BEP_STATUS_SOFTWARE;
        Log(LOG_ERRORS,"!!! environment variable FTP_ROOT does not exist !!!");
    }
    return status;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::StartModuleKeepAlive(void)
{
   return(StartSpecialKeepAlive(""));
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::StartSpecialKeepAlive(std::string messageId)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if we need to skip this step
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::StartModuleKeepAlive()\n");
    if((!ShortCircuitTestStep() || GetParameterBool("AlwaysStartKeepAlive")) && CheckCableConnect())
    {   // Need to perform this test step
        try
        {   // Try to start the keep alive thread
            if(messageId == "")
                moduleStatus = m_vehicleModule.StartModuleKeepAlive();
            else
                moduleStatus = m_vehicleModule.StartSpecialKeepAlive(messageId);
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            // Set the rest of the data
            Log(LOG_DEV_DATA, "Start Module Keep Alive: %s - status: %s\n",
                testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("StartModuleKeepAliveFail"));
            testDescription = 
                (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("StartModuleKeepAliveFail"));
        }
        catch(ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::StartModuleKeepAlive()\n");
    return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::StopModuleKeepAlive(void)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::StopModuleKeepAlive()\n");
    // Always stop the keep alive thred
    try
    {   // Try to stop the keep alive thread
        moduleStatus = m_vehicleModule.StopModuleKeepAlive();
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
            GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }
    // Check if this step should be skipped
    if(!ShortCircuitTestStep())
    {   // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        // Log the data
        Log(LOG_DEV_DATA, "Stop Module Keep Alive: %s - status: %s\n",
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
        testResultCode = (testResult == testPass ? "0000" : GetFaultCode("StopModuleKeepAliveFail"));
        testDescription = 
            (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("StopModuleKeepAliveFail"));
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
        // Ensure the keep alive thread is really dead
        m_vehicleModule.StopModuleKeepAlive();
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::StopModuleKeepAlive()\n");
    return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::CheckPartNumber(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string modulePartNumber;
    string broadcastPartNumber = ReadSubscribeData(GetDataTag("ModulePartNumber"));
    bool useBypassList = SystemReadBool(GetDataTag("UseBypassPartNumberList"));
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::CheckPartNumber()\n");
    if(!ShortCircuitTestStep())
    {   // Do not need to skip
        try
        {   // Read the part number from the module
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadModulePartNumber"), modulePartNumber);
            // Check the status of the data
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good data, Check if the number should be validated
                if(GetParameterBool("BypassPartNumberValidation"))
                {   // Skip Part number verification
                    Log(LOG_DEV_DATA, "Part number verification bypassed by parameter\n");
                    testResult = testPass;
                }
                else if (GetParameterBool("ValidatePartNumberList") || useBypassList)
                {   // if using the bypass list, check broadcast first
                    if(useBypassList)
                    {
                        testResult = (modulePartNumber == broadcastPartNumber) ? testPass : testFail;
                    }
                    // Verify the module part number against a list of valid part numbers
                    if(testResult.compare(testPass))
                    {
                    XmlNodeMapItr iter = m_validPartNumbers.find("PN"+modulePartNumber);
                    testResult = iter != m_validPartNumbers.end() ? testPass : testFail;
                    broadcastPartNumber = iter != m_validPartNumbers.end() ? iter->second->getValue() : "Not Listed";
                }
                }
                else if(GetParameterBool("PartNumberCompareByPosition"))
                {//check positions individually
                    testResult = testPass;
                    Log(LOG_DEV_DATA, "Iterate through positions to check\n");
                    for(XmlNodeMapItr iter = m_partNumberPositionComparison.begin();
                       (iter != m_partNumberPositionComparison.end());
                       iter++)
                    {   // Send individual details
                        try
                        {
                            char broadcastPNChar = broadcastPartNumber[atoi(iter->second->getAttribute("BroadcastPNCharPos")->getValue().c_str())];
                            char modulePNChar = modulePartNumber[atoi(iter->second->getAttribute("ModulePNCharPos")->getValue().c_str())];
                            Log(LOG_DEV_DATA, "broadcastChar = [%c] moduleChar = [%c]\n",broadcastPNChar,modulePNChar);
                            if(broadcastPNChar != modulePNChar)
                            {
                                testResult = testFail;
                                break;
                            }
                        }
                        catch(XmlException &ex)
                        {
                            Log(LOG_ERRORS, "XML Error getting part number characters %s: %s\n", GetProcessName().c_str(), ex.what());
                        }
                        Log(LOG_DEV_DATA, "Reporting Module data: %s - Value: %s\n",
                            iter->second->getName().c_str(), iter->second->getValue().c_str());
                    }

                }
                else if(modulePartNumber == broadcastPartNumber)
                {   // Part numbers match, test passes
                    testResult = testPass;
                }
                else
                {   // Part number do not match, test fails
                    testResult = testFail;
                }
                // Log the data
                Log(LOG_DEV_DATA, "Part Number Verification: %s - Broadcast: %s, Module: %s\n",
                    testResult.c_str(), broadcastPartNumber.c_str(), modulePartNumber.c_str());
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("PartNumberMismatch"));
                testDescription = 
                    (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("PartNumberMismatch"));
            }
            else
            {   // Error getting data from the module
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                SetCommunicationFailure(true);
                Log(LOG_ERRORS, "Error reading module part number - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
            }
        }
        catch(ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "ModulePartNumber", modulePartNumber, "",
                                 "BroadcastPartNumber", broadcastPartNumber, "");
        //abort after result is sent so failure is recorded
        if(GetParameterBool("AbortOnPartMismatch") && (testResult == testFail))
        {   // Part number do not match, test aborts
            testResult = testAbort;
        }
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    if (testResult == testAbort) 
    {
        SystemWrite(ABORT_DATA_TAG, "1");
    }
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::CheckPartNumber()\n");
    // Return the status
    return(testResult);
}


//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::CheckPartNumbers(void)
{
    string testResult = BEP_PASS_STATUS;
    string subtestResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string modulePartNumber;
    string broadcastPartNumber = "";
    bool useBypassList = SystemReadBool(GetDataTag("UseBypassPartNumberList"));
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::CheckPartNumber()\n");
    if(!ShortCircuitTestStep())
    {   // Do not need to skip

        for(XmlNodeMapItr iter = m_partNumbersToCheck.begin();
                           (iter != m_partNumbersToCheck.end());
                           iter++)
        {
            try
            {
                broadcastPartNumber = ReadSubscribeData(iter->second->getName().c_str());
                string messageTag = iter->second->getValue().c_str();
                subtestResult = BEP_TESTING_STATUS;
                if(messageTag.find(string("Constant")) == messageTag.npos)
                {// Read the part number from the module
                    moduleStatus = m_vehicleModule.ReadModuleData(messageTag, modulePartNumber);
                }
                else
                {// Get the value of the specified parameter
                    modulePartNumber = GetParameter(messageTag);
                    moduleStatus = BEP_STATUS_SUCCESS;
                }

                if(GetParameterBool("PartNumberCompareByPosition"))
                {
                    m_partNumberPositionComparison.clear();
                    m_partNumberPositionComparison.DeepCopy(m_configCopy->getChild("Setup/Parameters/" + 
                                                                                   iter->second->getAttribute("comparison")->getValue())->getChildren());
                }

                //modify broadcast part number based on first character
                if(GetParameterBool("ModifyBroadCastPN"))
                {
                    if(broadcastPartNumber[0] == GetParameter("PNCheckFirstCharacterComparisonValue")[0])
                    {
                        try
                        {
                            broadcastPartNumber = broadcastPartNumber.substr(GetParameterInt("PNCheckCompareEqualStartIndex"),GetParameterInt("PNCheckCompareEqualLength"));
                        }
                        catch(...)
                        {
                            Log(LOG_ERRORS, "Broadcast PN (first char equal) modifcation Error broadcastPartNumber: %s\n",
                                broadcastPartNumber.c_str());
                        }
                    }
                    else
                    {
                        try
                        {
                            broadcastPartNumber = broadcastPartNumber.substr(GetParameterInt("PNCheckCompareNotEqualStartIndex"),GetParameterInt("PNCheckCompareNotEqualLength"));
                        }
                        catch(...)
                        {
                            Log(LOG_ERRORS, "Broadcast PN (first char not equal) modification Error broadcastPartNumber: %s\n",
                                broadcastPartNumber.c_str());
                        }
                    }
                }
                // Check the status of the data
                if(BEP_STATUS_SUCCESS == moduleStatus)
                {   // Good data, Check if the number should be validated
                    if(GetParameterBool("BypassPartNumberValidation"))
                    {   // Skip Part number verification
                        Log(LOG_DEV_DATA, "Part number verification bypassed by parameter\n");
                        subtestResult = testPass;
                    }
                    else if(GetParameterBool("ValidatePartNumberList") || useBypassList)
                    {   // if using the bypass list, check broadcast first
                        if(useBypassList)
                        {
                            subtestResult = (modulePartNumber == broadcastPartNumber) ? testPass : testFail;
                        }
                        // Verify the module part number against a list of valid part numbers
                        if(subtestResult.compare(testPass))
                        {
                            XmlNodeMapItr iter = m_validPartNumbers.find("PN"+modulePartNumber);
                            subtestResult = iter != m_validPartNumbers.end() ? testPass : testFail;
                            broadcastPartNumber = iter != m_validPartNumbers.end() ? iter->second->getValue() : "Not Listed";
                        }
                    }
                    else if(GetParameterBool("PartNumberCompareByPosition"))
                    {//check positions individually
                        subtestResult = testPass;
                        Log(LOG_DEV_DATA, "Iterate through positions to check\n");
                        for(XmlNodeMapItr iter = m_partNumberPositionComparison.begin();
                           (iter != m_partNumberPositionComparison.end());
                           iter++)
                        {   // Send individual details
                            try
                            {
                                char broadcastPNChar = broadcastPartNumber[atoi(iter->second->getAttribute("BroadcastPNCharPos")->getValue().c_str())];
                                char modulePNChar = modulePartNumber[atoi(iter->second->getAttribute("ModulePNCharPos")->getValue().c_str())];
                                Log(LOG_DEV_DATA, "broadcastChar = [%c] moduleChar = [%c]\n",broadcastPNChar,modulePNChar);
                                if(broadcastPNChar != modulePNChar)
                                {
                                    subtestResult = testFail;
                                    break;
                                }
                            }
                            catch(XmlException &ex)
                            {
                                Log(LOG_ERRORS, "XML Error getting part number characters %s: %s\n", GetProcessName().c_str(), ex.what());
                            }
                            Log(LOG_DEV_DATA, "Reporting Module data: %s - Value: %s\n",
                                iter->second->getName().c_str(), iter->second->getValue().c_str());
                        }

                    }
                    else if(modulePartNumber == broadcastPartNumber)
                    {   // Part numbers match, test passes
                        subtestResult = testPass;
                    }
                    else
                    {   // Part number do not match, test fails
                        subtestResult = testFail;
                    }
                    // Log the data
                    Log(LOG_DEV_DATA, "Part Number Verification: %s - Broadcast: %s, Module: %s\n",
                        subtestResult.c_str(), broadcastPartNumber.c_str(), modulePartNumber.c_str());
                    testResultCode = (subtestResult == testPass ? "0000" : GetFaultCode("PartNumberMismatch"));
                    testDescription = 
                    (subtestResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("PartNumberMismatch"));
                }
                else
                {   // Error getting data from the module
                    subtestResult = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                    SetCommunicationFailure(true);
                    Log(LOG_ERRORS, "Error reading module part number - status: %s\n",
                        ConvertStatusToResponse(moduleStatus).c_str());
                }
            }
            catch(ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                subtestResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
            // Send the test result
            SendSubtestResultWithDetail("Check" + iter->second->getName(),subtestResult, testDescription, testResultCode,
                                     "ModulePartNumber", modulePartNumber, "",
                                     "BroadcastPartNumber", broadcastPartNumber, "");
            testResult = (subtestResult == testPass ? testResult : subtestResult);
        }

        SendTestResult(testResult, testDescription, testResultCode);
        //abort after result is sent so failure is recorded
        if(GetParameterBool("AbortOnPartMismatch") && (testResult == testFail))
        {   // Part number do not match, test aborts
            testResult = testAbort;
            DisplayTimedPrompt(GetPrompt("PartNumberAbort"), GetPromptBox("PartNumberAbort"), 
                               GetPromptPriority("PartNumberAbort"), GetPromptDuration("PartNumberAbort"));
        }
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    if (testResult == testAbort) 
    {
        SystemWrite(ABORT_DATA_TAG, "1");
    }
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::CheckPartNumber()\n");
    // Return the status
    return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::LockModule(void)
{
    // Set up some variables
    string testStatus = testFail;
    // Default failure
    string faultTag("FaultLockModuleCommError");
    BEP_STATUS_TYPE status;

    if ((!ShortCircuitTestStep() || GetTestStepResult().compare(testPass)) && 
        (GetOverallResult() == testPass) && SystemRead("CurrentTestStatus") == BEP_PASS_STATUS)
    {
        Log(LOG_FN_ENTRY, "%s: ENTER", GetTestStepName().c_str());
        try
        {   // Try to lock the module
            if(!IsModuleLocked())
            {
                Log(LOG_DEV_DATA, "Module unlocked: locking");
            status = m_vehicleModule.LockModule();
        }
            else
            {
                Log(LOG_DEV_DATA, "Module already unlocked");
                status = BEP_STATUS_SUCCESS;
            }
        }
        catch(ModuleException& caughtModuleException)
        {
            Log(LOG_ERRORS, "%s.%s: %s", GetComponentName().c_str(), GetTestStepName().c_str(),
                caughtModuleException.message().c_str());
            testStatus = testSoftwareFail;
            status = BEP_STATUS_SOFTWARE;
            faultTag ="FaultException";
        }
        // If we were successful on the lock
        if(status == BEP_STATUS_SUCCESS)
        {   // Log the success
            Log(LOG_DEV_DATA, "%s: Successfully locked Module", GetTestStepName().c_str());
            // Mark the status as successful
            // testPass is in GenericTC
            testStatus = testPass;
        }
        // Do final checks, this is the generic layout of any test step
        if(testStatus == testPass)
        {
            SendTestResult(testPass, GetTestStepInfo("Description"));
        }
        else
        {
            SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"),
                                     GetFaultCode(faultTag), GetFaultName(faultTag),
                                     GetFaultDescription(faultTag));
        }
    }
    else if(!GetTestStepResult().compare(testPass))
    {   // This test already passed, skip it but leave it marked as pass
        Log(LOG_DEV_DATA, "Already passed LockModule, not testing again");
        testStatus = testPass;
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping LockModule");
        testStatus = testSkip;
    }    // Log the function exit
    Log(LOG_FN_ENTRY, (GetTestStepName() + ": EXIT " + testStatus).c_str());
    // return the test result
    return(testStatus);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
bool GenericTCTemplate<ModuleType>::IsModuleLocked(void)
{
    bool isLocked = true;
    // Attempt to read the A/C status fro mthe module
    try
    {   // Read the A/C status from the module
        m_vehicleModule.ReadModuleData("IsModuleLocked", isLocked);
    }
    catch(ModuleException &exception)
    {   // Exception reading data
        Log(LOG_ERRORS, "Module exception in IsModuleLocked() - %s\n", exception.message().c_str());
        isLocked = true;
    }
    // Return the result
    return isLocked;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::UnLockModule(void)
{
    // Set up some variables
    string testStatus = testFail;
    // Default failure
    string faultTag("FaultUnlockModuleCommError");
    BEP_STATUS_TYPE status;

    if(ShortCircuitTestStep())
    {
        testStatus = testPass;
    }
    else
    {   // Log the function entry
        Log(LOG_FN_ENTRY, "%s: ENTER\n", GetTestStepName().c_str());
        try
        {   // Get the module key
            string key = ReadSubscribeData( GetDataTag("ModuleKey") );
            // Try to unlock the module
            status = m_vehicleModule.UnLockModule(key);
        }
        catch(ModuleException& caughtModuleException)
        {
            Log(LOG_ERRORS, "%s.%s: %s", GetComponentName().c_str(), GetTestStepName().c_str(),
                caughtModuleException.message().c_str());
            testStatus = testSoftwareFail;
            status = BEP_STATUS_SOFTWARE;
            faultTag = "FaultException";
        }
        // If we were successful on the unlock
        if(status == BEP_STATUS_SUCCESS)
        {   // Log the success
            Log(LOG_DEV_DATA, "%s: Successfully unlocked Module\n", GetTestStepName().c_str());
            // Mark the status as successful
            // testPass is in GenericTC
            testStatus = testPass;
        }
        // Do final checks, this is the generic layout of any test step
        if(testStatus == testPass)
        {
            SendTestResult(testPass, GetTestStepInfo("Description"));
            // Log the function exit
            Log(LOG_FN_ENTRY, "%s: EXIT SUCCESS\n", GetTestStepName().c_str());
        }
        else
        {
            SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"),
                                     GetFaultCode(faultTag), GetFaultName(faultTag),
                                     GetFaultDescription(faultTag));
        }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: EXIT - %s\n ", GetTestStepName().c_str(), testStatus.c_str());
    // Return the test result
    return(testStatus);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::CommandSecurityUnlock(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Test in progress";
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::CommandSecurityUnlock()\n");
    // Vehicle is connected, unlock security
    for(INT32 unlockAttempt = 0; (unlockAttempt < GetParameterInt("SecurityUnlockAttempts")) &&
       (moduleStatus != BEP_STATUS_SUCCESS); unlockAttempt++)
    {   // Command the module to unlock security
        try
        {
            moduleStatus = m_vehicleModule.UnlockModuleSecurity();
            // If unlock was not successful, wait before attempting again
            if(BEP_STATUS_SUCCESS != moduleStatus) BposSleep(GetParameterInt("SecurityUnlockDelay"));
        }
        catch(ModuleException &exception)
        {   // Exception during security unlock
            Log(LOG_ERRORS, "Module Exception during CommandSecurityUnlock() - %s\n", exception.message().c_str());
            moduleStatus = BEP_STATUS_SOFTWARE;
        }
    }
    // Determine the test result
    testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testAbort);
    testResultCode = (testResult == testPass ? "0000" : GetFaultCode("SecurityUnlockFailed"));
    testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("SecurityUnlockFailed"));
    // Log the status
    Log(LOG_DEV_DATA, "Module Security Unlocked - %s\n", testResult.c_str());
    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::CommandSecurityUnlock()\n");
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::EnterDiagnosticMode(void)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::EnterDiagnosticMode()\n");
    if(!ShortCircuitTestStep() || GetParameterBool("AlwaysEnterDiagnosticMode"))
    {   // Need to perform this test step
        if(CheckCableConnect())
        {
            try
            {   // Try to enter diagnostic mode
                moduleStatus = m_vehicleModule.EnterDiagnosticMode();
                // Determine the test result
                if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = 
                    (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "Enter Diagnostic Mode: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            }
            catch(ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "GenericTCTemplate::EnterDiagnosticMode() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::EnterDiagnosticMode()\n");
    return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::ExitDiagnosticMode(void)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE status;
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {
        try
        {   // Try to exit diagnostic mode
            status = m_vehicleModule.ExitDiagnosticMode();
            testResult = status == BEP_STATUS_SUCCESS ? testPass : testFail;
            testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
            testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
            Log(LOG_ERRORS, "Exit Diagnostic Mode status: %s\n", ConvertStatusToResponse(status).c_str());
        }
        catch(ModuleException& caughtModuleException)
        {
            Log(LOG_ERRORS, "%s.%s: %s\n", 
                GetComponentName().c_str(), GetTestStepName().c_str(), caughtModuleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Report the result of the test
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s::%s - Exit - %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
    // return the test result
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::CheckVIN(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string moduleVIN;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if we need to skip this test step
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::CheckVIN()\n");
    if(!ShortCircuitTestStep())
    {   // Do not need to skip
        try
        {   // Read the data from the module
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadModuleVIN"), moduleVIN);
            // Check the status of the data
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good data, check if the VIN should be validated
                if(GetParameterBool("BypassVINValidation"))
                {   // Skip VIN validation
                    Log(LOG_DEV_DATA, "VIN verification bypassed by parameter\n");
                    testResult = testPass;
                }
                else if(moduleVIN == ReadSubscribeData(GetDataTag("VehicleVIN")))
                {   // VINs match, test passes
                    testResult = testPass;
                }
                else
                {   // VINs do not match, test fails
                    testResult = testFail;
                    if (GetParameterBool("PromptDriverVinMismatch"))
                    {// inform driver of failure
                        std::string prompt = "VinMismatchPrompt";
                        DisplayTimedPrompt(prompt, GetPromptBox(prompt), GetPromptPriority(prompt), GetPromptDuration(prompt));
                    }
                    // Check if testing should be aborted for a VIN mismatch
                    if(GetParameterBool("AbortIfVinMismatch"))
                    {
                        Log(LOG_ERRORS, "Module VIN does not match broadcast, aborting test sequence");
                        DisplayTimedPrompt(GetPrompt("VinAbort"), GetPromptBox("VinAbort"), GetPromptPriority("VinAbort"),
                                           GetPromptDuration("VinAbort"));
                        SystemWrite(ABORT_DATA_TAG, "1");
                        testResult = testAbort;
                    }
                }
                Log(LOG_DEV_DATA, "VIN Check: %s - Broadcast: %s, Module: %s\n",
                    testResult.c_str(), ReadSubscribeData(GetDataTag("VehicleVIN")).c_str(), moduleVIN.c_str());
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("VINMismatch"));
                testDescription = (testResult== testPass ? GetTestStepInfo("Description") : GetFaultDescription("VINMismatch"));
            }
            else
            {   // Communication error getting data
                Log(LOG_ERRORS, "Communication failure readng module VIN - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
            }
        }
        catch(ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Report the results
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "ModuleVIN", moduleVIN, "",
                                 "BroadcastVIN", ReadSubscribeData(GetDataTag("VehicleVIN")), "");
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::CheckVIN()\n");
    return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::ProgramVIN(void)
{
    string testStatus = testFail;
    string faultTag("FaultVINCommError");
    string configVIN;
    string badVINFaultTag("FaultProgramVINBadVIN");
    string badVINLengthFaultTag("FaultProgramVINBadVINLength");
    BEP_STATUS_TYPE status;

    if(ShortCircuitTestStep())
    {
        testStatus = testPass;
    }
    else
    {
        Log(LOG_FN_ENTRY, "%s: ENTER\n", GetTestStepName().c_str());
        // Get the VIN from the server
        configVIN = ReadSubscribeData(GetDataTag("VIN"));
        // Do some validation on the VIN
        unsigned int vinLength = GetParameterInt("VINLength");
        if(vinLength <= 0)
        {   // Log this
            Log(LOG_FN_ENTRY, "%s: ERROR CONFIG VIN LENGTH\n", GetTestStepName().c_str());
            // Send a failed test result with some info
            SendTestResultWithDetail(testFail, GetTestStepInfo("Description"),
                                     GetFaultCode(badVINLengthFaultTag), GetFaultName(badVINLengthFaultTag),
                                     GetFaultDescription(badVINLengthFaultTag));
            testStatus = testFail;
        }
        else
        {   // Check the length
            if(configVIN.length() != vinLength)
            {   // Log this
                Log(LOG_FN_ENTRY, "%s: ERROR VIN LENGTH MISMATCH\n", GetTestStepName().c_str());
                // Send a failed test result with some info
                SendTestResultWithDetail(testFail, GetTestStepInfo("Description"),
                                         GetFaultCode(badVINFaultTag), GetFaultName(badVINFaultTag),
                                         GetFaultDescription(badVINFaultTag));
                testStatus = testFail;
            }
            else
            {
                try
                {   // Actually program the vin
                    status = m_vehicleModule.ProgramVIN(configVIN);
                    if(status == BEP_STATUS_SUCCESS)
                    {
                        testStatus = testPass;
                        SendTestResult(testPass, GetTestStepInfo("Description"));
                    }
                    else
                    {
                        testStatus = testFail;
                        SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"),
                                                 GetFaultCode(faultTag), GetFaultName(faultTag),
                                                 GetFaultDescription(faultTag));
                    }
                }
                catch(ModuleException& caughtModuleException)
                {
                    Log(LOG_ERRORS, "%s.%s: %s\n", GetComponentName().c_str(), GetTestStepName().c_str(),
                        caughtModuleException.message().c_str());
                    testStatus = testFail;
                }
            }
        }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, (GetTestStepName() + ": EXIT " + testStatus).c_str());
    // Return the test result
    return(testStatus);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::ClearFaults(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Determine if this test step needs to be skipped
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::ClearFaults()\n");
    if(!ShortCircuitTestStep() || GetParameterBool("AlwaysClearFaults"))
    {   // Do not need to skip this step
        try
        {   // Tell the module to clear faults
            moduleStatus = m_vehicleModule.ClearFaults();
            // Determine the test results
            if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = 
                (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
            // Log the data
            Log(LOG_DEV_DATA, "Clear Faults: %s - status: %s\n",
                testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            BposSleep(GetParameterInt("ClearFaultsDelay"));
        }
        catch(ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::ClearFaults()\n");
    return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::ReadFaults(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    FaultVector_t moduleFaults;
    string foundFaults;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::ReadFaults()\n");
    if(!ShortCircuitTestStep() || GetParameterBool("AlwaysReadFaults"))
    {   // Do not need to skip this step
        try
        {   // Try to read the module faults
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadModuleFaults"), moduleFaults);
            // Check the status of the operation
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good read, evaluate the data
                bool faultsRecorded = false;
                Log(LOG_DEV_DATA, "Found %d faults recorded in the module\n", moduleFaults.size());
                // Only check for faults if faults were read from the module
                if(moduleFaults.size() > 0)
                {
                    for(UINT32 faultIndex = 0; faultIndex < moduleFaults.size(); faultIndex++)
                    {   // Determine if this fault should be ignored
                        string faultTag = "ModuleFault_" + moduleFaults[faultIndex];
                        INT32 faultCode = atoh(moduleFaults[faultIndex].c_str());
                        string faultStatus("Ignored");
                        if((faultCode != 0) && (m_ignoreFaults.find(faultTag) == m_ignoreFaults.end()))
                        {   // This is a fault to report
                            faultStatus = "Reported";
                            ReportDTC(faultTag, moduleFaults[faultIndex], GetFaultDescription(faultTag));
                            faultsRecorded = true;
                        }
                        // Log the fault read from the module
                        Log(LOG_DEV_DATA, "Module Fault %d - %s - %s\n", faultIndex+1,
                            moduleFaults[faultIndex].c_str(), faultStatus.c_str());
                    }
                    // Do special handling if faults have been recorded
                    if(faultsRecorded)
                    {   // Check if test sequence should be aborted if faults are present
                        if(GetParameterBool("AbortIfDTCPresent"))
                        {
                            testResult = testFail;
                            DisplayTimedPrompt(GetPrompt("DtcAbort"), GetPromptBox("DtcAbort"), 
                                               GetPromptPriority("DtcAbort"), GetPromptDuration("DtcAbort"));
                            SystemWrite(ABORT_DATA_TAG, "1");
                            Log(LOG_ERRORS, "Faults found in the module - ABORT all testing!\n");
                        }
                        else
                        {
                            testResult = testFail;
                            Log(LOG_ERRORS, "Faults found in the module - continue to test!\n");
                        }
                    }
                    else if(!faultsRecorded && DTCsInModule()) // If we have DTCs stored from previous run but no DTCs in module now, clear our stored DTCs
                    {
                        RemoveDTC();
                    }
                    // Store the data indicating if faults are present in the module
                    DTCsInModule(faultsRecorded);
                }
                // Set the test status
                testResult = faultsRecorded ? testResult : testPass;
                testResultCode = faultsRecorded ? GetFaultCode("ModuleFaults") : "0000";
                testDescription = faultsRecorded ? GetFaultDescription("ModuleFaults") : GetTestStepInfo("Description");
                Log(LOG_DEV_DATA, "Read Module Faults: %s\n", testResult.c_str());
            }
            else
            {   // Error reading faults from the module
                Log(LOG_ERRORS, "Error reading module faults - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                SetCommunicationFailure(true);
            }
        }
        catch(ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Report the results
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::ReadFaults()\n");
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::CollectTraceData(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string traceData;
    string serialNumber;
    BEP_STATUS_TYPE moduleStatus;
    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::CollectTraceData()\n");
    if(!ShortCircuitTestStep())
    {   // Perform the test step
        try
        {   // Try to read the trace data
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadTraceData"), traceData);
            // Set the test results
            testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = 
                (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
            // Log the data
            serialNumber = traceData;
            Log(LOG_DEV_DATA, "Trace data collected from module: %s\n", serialNumber.c_str());
        }
        catch(ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Report the test results
        SendTestResultWithDetail(testResult, testDescription, testResultCode, "SerialNumber", serialNumber, "");
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::CollectTraceData()\n");
    return(testResult);
}

#if 0
template <class ModuleType>
string GenericTCTemplate<ModuleType>::MonitorFaults(void)
{
    string testStatus = testFail;
    string faultTag;
    Log(LOG_FN_ENTRY, "%s: ENTER", GetTestStepName().c_str());
    if(ShortCircuitTestStep())
    {
        testStatus = testPass;
    }
    else
    {   /* TBD */
        if(testStatus == testPass)
        {   /* TBD */
        }
        else
        {   /* TBD */
        }
    }
    Log(LOG_FN_ENTRY, "%s: EXIT", GetTestStepName().c_str());
    return(testStatus);
}

template <class ModuleType>
string GenericTCTemplate<ModuleType>::CalProgram(void)
{
    string testStatus = testFail;
    string faultTag;
    Log(LOG_FN_ENTRY, "%s: Enter", GetTestStepName().c_str());
    if(ShortCircuitTestStep())
    {
        testStatus = testPass;
    }
    else
    {
        if(testStatus == testPass)
        {   /* TBD */
        }
        else
        {   /* TBD */
        }
    }
    Log(LOG_FN_ENTRY, "%s: EXIT", GetTestStepName().c_str());
    return(testStatus);
}
#endif

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::ReportModuleInfo(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult(testPass);    // Used to report results of the test
    string testDescription(GetTestStepInfo("Description")); // Used to report description of test
    string testResultCode("0000");  // Used to report result code from test
    // Log the method entry
    Log(LOG_FN_ENTRY, "%s:%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step needs to be performed
    if(!ShortCircuitTestStep() && (testPass != GetTestStepResult()) && (testSkip != GetTestStepResult()))
    {   // Read data from the module
        status = m_vehicleModule.ReadInfoToReport(&m_moduleDataItems, GetDataTag("ReportModuleInfoMessage"));
        testResult = (status == BEP_STATUS_SUCCESS) ? testPass : testFail;
        testDescription = 
            (testResult == testPass) ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure");
        testResultCode = (testResult == testPass) ? "0000" : GetFaultCode("CommunicationFailure");
        if(testResult == testPass)
        {//Read was good, send the test results
            for(XmlNodeMapItr iter = m_moduleDataItems.begin();
               (iter != m_moduleDataItems.end());
               iter++)
            {   // Send individual details
                Log(LOG_DEV_DATA, "Iterate through data items\n");
                try
                {
                    SendSubtestResultWithDetail(iter->second->getName(),testResult, testDescription, testResultCode,
                                                iter->second->getName(), iter->second->getValue(), "");
                }
                catch(XmlException &ex)
                {
                    Log(LOG_ERRORS, "XML Error Sending individual test result %s: %s\n", GetProcessName().c_str(), ex.what());
                }
                Log(LOG_DEV_DATA, "Reporting Module data: %s - Value: %s\n",
                    iter->second->getName().c_str(), iter->second->getValue().c_str());
            }
        }
        else
        {
            if(!GetParameterBool("AllowReportInfoFail"))
            {
                testResult = testPass;
            }
            Log(LOG_DEV_DATA, "Module Read did not pass\n");
        }
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Send test results to test server
    SendTestResult(testResult,testDescription,testResultCode);
    // Log the method exit
    Log(LOG_FN_ENTRY, "%s:%s - Exit, Result = %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
    // return method result
    return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const bool GenericTCTemplate<ModuleType>::CheckIgnitionOn()
{
    bool ignitionOn = false;    // Used to store the return result from IsIgnitionOn()
    bool promptDisplayed = false;   // Used to determine if prompts have been displayed
    bool controlPrompts = false;    // Used to determine if prompt server can be used
    UINT32 delay = 100;     // Used to store amount of delay from TestStep info in config file
    INT32 timeoutTime = GetParameterInt("IgnitionOnTimeout");
    // Set delay in do-while loop using BposSleep() function call
    delay = GetTestStepInfoInt("ScanDelay");

    // Start the timer
    SetStartTime();
    // Make sure the cable is connected first
    if(CheckCableConnect())
    {   // Wait for the ignition to be on
        do
        {   // Check if the ignition is on and prompt if neccessary
            ignitionOn = IsIgnitionOn();
            if(!ignitionOn)
            {   // Check if bkground test, if so attempt to lock prompt server
                if(!IsForegroundTest() && !controlPrompts)
                {   // Resets the step start time since the step doesn't have control
                    //    of the prompt server
                    SetStartTime();
                    // Check for Foreground test, if none, attempt to lock prompt server
                    controlPrompts = CheckForegroundLockPrompts();
                }
                // Check if foreground test and if the controlPrompts variable has been set
                else if(IsForegroundTest() && !controlPrompts)
                {
                    controlPrompts = true;
                }
                // if prompt server is accessable, display prompt if not already done
                if(controlPrompts && !promptDisplayed)
                {   // Ignition is not on, prompt the operator
                    DisplayPrompt(GetPromptBox("IgnitionOn"), GetPrompt("IgnitionOn"), GetPromptPriority("IgnitionOn"));
                    promptDisplayed = true;
                }
                else
                {
                    BposSleep(delay);
                }
            }
        } while((TimeRemaining(&timeoutTime) || !controlPrompts) && !ignitionOn && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Determine if prompt should be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("IgnitionOn"), GetPrompt("IgnitionOn"), GetPromptPriority("IgnitionOn"));
            if(controlPrompts && !IsForegroundTest())
            {
                UnlockPrompts();
            }
        }
        // Log the status
        if(!ignitionOn) Log(LOG_ERRORS, "Timeout waiting for ignition on!\n");
        else                 Log(LOG_DEV_DATA, "Ignition is On\n");
    }
    // Return the status of ignition on
    return ignitionOn;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const bool GenericTCTemplate<ModuleType>::CheckEngineRunning(void)
{
    bool engineOn = false;  // Used to store return result from IsEngineRunning()
    bool promptDisplayed = false;  // Used to determine if prompts have been displayed
    bool controlPrompts = false;  // Used to determine if prompt server can be used
    UINT32 delay = 100;  // Used to store amount of delay from TestStep info in config file
    INT32 timeoutTime = GetParameterInt("EngineRunningTimeout");
    // Set delay in do-while loop using BposSleep() function call
    delay = GetTestStepInfoInt("ScanDelay");
    // Start the timer
    SetStartTime();
    // Make sure the cable is connected first
    if(CheckCableConnect())
    {   // Wait for the engine to be running
        do
        {   // Check if the engine is running and prompt if neccessary
            engineOn = IsEngineRunning();
            if(!engineOn)
            {   // Check if bkground test, if so attempt to lock prompt server
                if(!IsForegroundTest() && !controlPrompts)
                {   // Resets the step start time since the step doesn't have control
                    //    of the prompt server
                    SetStartTime();
                    // Check for Foreground test, if none, attempt to lock prompt server
                    controlPrompts = CheckForegroundLockPrompts();
                }
                // Check if foreground test and if the controlPrompts variable has been set
                else if(IsForegroundTest() && !controlPrompts)
                {
                    controlPrompts = true;
                }
                // if prompt server is accessable, display prompt if not already done
                if(controlPrompts && !promptDisplayed)
                {   // Engine Not running, prompt the operator
                    DisplayPrompt(GetPromptBox("EngineRunning"), GetPrompt("EngineRunning"),
                                  GetPromptPriority("EngineRunning"));
                    promptDisplayed = true;
                }
                else
                {
                    BposSleep(delay);
                }
            }
        } while((TimeRemaining(&timeoutTime) || !controlPrompts) && !engineOn && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Check if prompt should be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("EngineRunning"), GetPrompt("EngineRunning"), GetPromptPriority("EngineRunning"));
            if(controlPrompts && !IsForegroundTest())
            {
                UnlockPrompts();
            }
        }
        // Log the status
        if(!engineOn) Log(LOG_ERRORS, "Timeout waiting for engine running!\n");
        else               Log(LOG_DEV_DATA, "Engine is running\n");
    }
    // Return the status of ignition on
    return engineOn;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const string GenericTCTemplate<ModuleType>::WaitForEngineOffIgnitionOn(void)
{
    string testResult(testFail);
    bool engineOffIgnitionOn = false;
    Log(LOG_FN_ENTRY, "GenericTCTemplate<ModuleType>::WaitForEngineOffIgnitionOn()  - Enter");
    do
    {   // Display prompt to put Engine off ignition on
        DisplayPrompt(GetPromptBox("EngineOffIgnitionOn"), 
                      GetPrompt("EngineOffIgnitionOn"), GetPromptPriority("EngineOffIgnitionOn"));
        if(!IsIgnitionOn())
        {   // Wait before the next check
            BposSleep(GetTestStepInfoInt("ScanDelay"));
        }
        else if(IsEngineRunning())
        {   // Wait before the next check
            BposSleep(GetTestStepInfoInt("ScanDelay"));
        }
        else
        {   // Ignition in correct position
            engineOffIgnitionOn = true;
        }
    } while(TimeRemaining() && !engineOffIgnitionOn && (BEP_STATUS_SUCCESS == StatusCheck()));
    // Remove the prompt
    RemovePrompt(GetPromptBox("EngineOffIgnitionOn"), 
                 GetPrompt("EngineOffIgnitionOn"), GetPromptPriority("EngineOffIgnitionOn"));
    // Check the exit conditions
    if(!TimeRemaining() && !engineOffIgnitionOn)
    {   // Timeout waiting engine off, ignition on
        testResult = testTimeout;
        Log(LOG_ERRORS, "Timeout waiting engine off, ignition on\n");
    }
    else if(BEP_STATUS_SUCCESS != StatusCheck())
    {   // Bad system status
        testResult = testAbort;
        Log(LOG_ERRORS, "Bad system status while waiting for engine off, ignition on - status: %s\n",
            ConvertStatusToResponse(StatusCheck()).c_str());
    }
    else
    {   // Determine the results of the test
        testResult = testPass;
        Log(LOG_ERRORS, "Engine is Off and ignition is On");
    }
    Log(LOG_FN_ENTRY, "GenericTCTemplate<ModuleType>::WaitForEngineOffIgnitionOn()  - Exit");
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const bool GenericTCTemplate<ModuleType>::IsIgnitionOn(void)
{
    bool ignitionOn = false;
    // Attempt to read data from the module. If good response, ignition is on
    try
    {   // Determine if the ignition is on
        ignitionOn = m_vehicleModule.IsIgnitionOn();
    }
    catch(ModuleException &exception)
    {   // Exception reading data
        Log(LOG_ERRORS, "Module exception in IsIgnitionOn() - %s\n", exception.message().c_str());
        ignitionOn = false;
    }
    // Return the status
    return ignitionOn;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const bool GenericTCTemplate<ModuleType>::IsEngineRunning(void)
{
    bool engineRunning = false;
    // Attempt to read the engine RPM from the module
    try
    {   // Determine if the engine is running
        engineRunning = m_vehicleModule.IsEngineRunning(GetParameterInt("MinimumEngineRunningRPM"));
    }
    catch(ModuleException &exception)
    {   // Exception reading data
        Log(LOG_ERRORS, "Module exception in IsEngineRunning() - %s\n", exception.message().c_str());
        engineRunning = false;
    }
    // Return the status
    return engineRunning;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void GenericTCTemplate<ModuleType>::Abort(void)
{   // Make sure module communications are stopped
    if(m_vehicleModule.IsInitialized())
    {   // Set the stop comms flag
        m_vehicleModule.StopVehicleComms(true);
    }
    if (!m_abortCalled)
    {
        //set flag indicating abort called
        m_abortCalled = true;
        // Need to stop all background monitors on an abort
        StopBackgroundMonitors();
        m_abortCalled = false;
    }
    // Call the base class to complete the abort processing
    GenericTC::Abort();
}

//-----------------------------------------------------------------------------
template <class ModuleType>
inline void GenericTCTemplate<ModuleType>::DTCsInModule(const bool &dtcsInModule)
{
    m_faultsInModule = dtcsInModule;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
inline const bool& GenericTCTemplate<ModuleType>::DTCsInModule(void)
{
    return m_faultsInModule;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::EnterDiagnosticModeNoResponse(void)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter GenericTCTemplate::EnterDiagnosticModeNoResponse()\n");
    if(!ShortCircuitTestStep() || GetParameterBool("AlwaysEnterDiagnosticMode"))
    {   // Need to perform this test step
        if(CheckCableConnect())
        {
            try
            {   // Try to enter diagnostic mode
                moduleStatus = m_vehicleModule.EnterDiagnosticModeNoResponse();
                // Determine the test result
                if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = 
                    (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "Enter Diagnostic Mode: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            }
            catch(ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "GenericTCTemplate::EnterDiagnosticMode() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::EnterDiagnosticMode()\n");
    return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string GenericTCTemplate<ModuleType>::WaitForEngineOffKeyOn(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    string engineStatus = BEP_FAIL_STATUS;
    Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
    // Do not need to perform this step if previous pass
    if(!ShortCircuitTestStep() || GetParameterInt("AlwaysWaitForEngineOffKeyOn"))
    {   // Wait for the engine to be off and the ignition to be on
        engineStatus = WaitForEngineOffIgnitionOn();
        if(!engineStatus.compare(testTimeout))
        {   // Timeout waiting for conditions
            testResult = testTimeout;
            testResultCode = GetFaultCode("TestStepTimeout");
            testDescription = GetFaultDescription("TestStepTimeout");
            Log(LOG_ERRORS, "Timeout waiting engine off, ignition on");
        }
        else if(!engineStatus.compare(testAbort))
        {   // Bad system status
            testResult = testAbort;
            testResultCode = GetFaultCode("StatusCheckFailure");
            testDescription = GetFaultCode("StatusCheckFailure");
            Log(LOG_ERRORS, "Bad system status while waiting for engine off, ignition on - status: %s\n",
                ConvertStatusToResponse(StatusCheck()).c_str());
        }
        else
        {   // Key On Engine Off status
            testResult = BEP_PASS_STATUS == engineStatus ? testPass : testFail;
            testResultCode = testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure");
            testDescription = 
                testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure");
            Log(LOG_ERRORS, "TurnEngineOffIgnitionOn: %s", testResult.c_str());
        }
        // Report the result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step - %s", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}

//-----------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericTCTemplate<VehicleModuleType>::ResetModule(void)
{
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    string testResult            = BEP_TESTING_STATUS;
    string testResultCode        = "0000";
    string testDescription       = GetTestStepInfo("Description");
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if (!ShortCircuitTestStep() || GetParameterBool("AlwaysResetModule"))
    {   // Attempt to command the module
        try
        {
            moduleStatus = m_vehicleModule.PerformPowerOnReset();
            // Check the status of the command
            Log(LOG_DEV_DATA, "Power on reset: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
            testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
            // Wait for module reset to complete, defaults to 0
            BposSleep(GetParameterInt("ModuleResetDelay"));
        }
        catch (ModuleException &excpt)
        {   // Something bad happened
            Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Skip the test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void GenericTCTemplate<ModuleType>::LoadStaticTestResults(void)
{
    XmlParser parser;
    // Request the static test results and load the file
    Log(LOG_FN_ENTRY, "GenericTCTemplate::LoadStaticTestResults() - Enter");
    XmlNode resultRequest(GetDataTag("StaticTestResult"), SystemRead(VIN_DATA_TAG));
    Log(LOG_DEV_DATA, "Checking for test result for: %s", SystemRead(VIN_DATA_TAG).c_str());
    string response;
    INT32 status = m_staticTesterServerComm->Read(&resultRequest, response, true);
    Log(LOG_DEV_DATA, "Test result read status: %s", ConvertStatusToResponse(status).c_str());
    if(BEP_STATUS_SUCCESS == status)
    {
        Log(LOG_DEV_DATA, "Read test results from the static tester server.");
        string resultFile, tag;
        m_staticTesterServerComm->GetNext(tag, resultFile, response);
        XmlNodeMap *testResults = NULL;
        try
        {
            Log(LOG_DEV_DATA, "Loading test result file: %s", resultFile.c_str()); 
            testResults = const_cast<XmlNodeMap *>(&parser.ReturnXMLDocument(resultFile)->getChildren());
        }
        catch(XmlException &excpt)
        {
            testResults = NULL;
            Log(LOG_ERRORS, "Could not load static test result file, not using static tester results: %s", excpt.GetReason());
        }
        catch(BepException &excpt)
        {
            testResults = NULL;
            Log(LOG_ERRORS, "Could not load static test result file, not using static tester results: %s", excpt.GetReason());
        }
        // Update our current test step results with the results from the static testing session
        for(XmlNodeMapItr iter = testResults->begin(); iter != testResults->end(); iter++)
        {   // Only process this result if it belongs to this test component
            if(iter->second->getName().find(GetComponentName()) != iter->second->getName().npos)
            {   // This result belongs to this test component
                Log(LOG_DEV_DATA, "Processing static test result: %s", iter->second->getName().c_str());
                string testStepName = iter->second->getName().substr(GetComponentName().length());
                // Load the test step and set the result
                bool testStepValid = true;
                try
                {
                    SetCurrentTestStep(m_testSteps.getNode(testStepName));
                }
                catch(XmlException &excpt)
                {
                    Log(LOG_ERRORS, "Invalid test step requested: %s", testStepName.c_str());
                    testStepValid = false;
                }
                if(testStepValid)
                {   // Update the result for this test step
                    string result;
                    bool resultValid = true;
                    try
                    {
                        result = iter->second->getAttribute("Result")->getValue();
                    }
                    catch(XmlException &excpt)
                    {
                        Log(LOG_ERRORS, "Could not find static test result for %s, using NotTested", testStepName.c_str());
                        result = "Not Tested";
                        resultValid = false;
                    }
                    Log(LOG_DEV_DATA, "Setting %s result to %s", GetTestStepName().c_str(), result.c_str());
                    SetTestStepResult(result);
                    // If result is valid, send the results to the Test Result Server
                    if(resultValid)
                    {
                        TestResultDetails details;
                        const XmlNodeMap *resultDetails = &iter->second->getChildren();
                        if(resultDetails->size() > 0)
                        {
                            for(XmlNodeMapCItr detailIter = resultDetails->begin(); 
                                 detailIter != resultDetails->end(); detailIter++)
                            {
                                details.AddDetail(detailIter->second->getName(), 
                                                  detailIter->second->getValue());
                                Log(LOG_DEV_DATA, "Adding detail %s - %s to test result %s",
                                    detailIter->second->getName().c_str(), 
                                    detailIter->second->getValue().c_str(),
                                    testStepName.c_str());
                            }
                            SendSubtestResultWithDetail(testStepName, result, details, GetTestStepInfo("Description"), "0000");
                        }
                        else
                        {
                            SendSubtestResult(testStepName, result, GetTestStepInfo("Description"), "0000");
                        }
                    }
                    else
                    {
                        Log(LOG_ERRORS, "Test result %s is not valid, not reporting to TestResultServer", result.c_str());
                    }
                }
                else
                {
                    Log(LOG_ERRORS, "Test Step %s is not a valid test step, not setting results.", testStepName.c_str());
                }
            }
        }
    }
    Log(LOG_FN_ENTRY, "GenericTCTemplate::LoadStaticTestResults() - Exit");
}

template <class ModuleType>
bool GenericTCTemplate<ModuleType>::RequestStateByPrompt(string requestedState, string promptForState, 
                                                         string currentStateTag, INT32 minimumSuccessiveReads, 
                                                         INT32 successiveReadDelay)
{
    INT32 successiveReads = 0;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_FAILURE;  // Used to store return result for module read
    string currentState(BEP_NO_DATA);                   // Used to store current state from module read

    //Do not show the prompt unless the current state does not match the request
    bool showingPrompt = false;

    //Continue checking and prompting until the current state matches the requested state
    //Also allow a minimum amount of successive reads to ensure this is not a pass-through state
    while(TimeRemaining() && (successiveReads < minimumSuccessiveReads) && (BEP_STATUS_SUCCESS == StatusCheck())) 
    {
        //Get the current state from the module
        moduleStatus = m_vehicleModule.ReadModuleData(GetDataTag(currentStateTag), currentState);

        if(moduleStatus == BEP_STATUS_SUCCESS)
        {   // Check that the requested state matches the current state
            Log(LOG_DEV_DATA, "Comparing Requested state: %s current state: %s",
                requestedState.c_str(), currentState.c_str());
            if(requestedState == currentState)
            {   //Increment the successive reads until the minimum is met
                successiveReads++;
                if(successiveReads >= minimumSuccessiveReads) break;
            }
            else
            {
                if(!showingPrompt)
                {   //Request the driver to perform action for requested state
                    DisplayPrompt(GetPromptBox(promptForState), GetPrompt(promptForState), GetPromptPriority(promptForState));

                    //Do not show the prompt more than once
                    showingPrompt = true;
                }
                //Reset the successive reads counter when request is not met
                successiveReads = 0;
            }
        }
        else
        {
            Log(LOG_ERRORS, "Failed to read the current state with data tag: %s\n", GetDataTag(currentStateTag).c_str());
        }

        //Wait between successive reads so that a quick shift through is not recorded
        BposSleep(successiveReadDelay);
    }

    if(showingPrompt)
    {   //Remove the prompt from the driver screen if it is being shown
        RemovePrompt(GetPromptBox(promptForState), GetPrompt(promptForState), GetPromptPriority(promptForState));
    }

    //Return true if the requested state was read the minimum amount of times
    bool bStateSeen = (successiveReads >= minimumSuccessiveReads);
    Log(LOG_DEV_DATA, "Requested (%i of minimum %i reads) state: '%s' state seen: %s", successiveReads, minimumSuccessiveReads,
        requestedState.c_str(), bStateSeen ? "True" : "False");
    return bStateSeen;
}

template <class ModuleType>
string GenericTCTemplate<ModuleType>::KeyOffEngineOffKeyOn(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
    // Do not need to perform this step if previous pass
    if(!ShortCircuitTestStep() || GetParameterBool("AlwaysPerformKeyOffEngineOff"))
    {
        try
        {                              //Wait for the engine to be off and ignition to be off
            testResult = WaitForEngineOffIgnitionOff();
            if(!testResult.compare(testPass))
            {                          // Reset the adaptive memory
                Log(LOG_DEV_DATA, "EngineOffIgnitionOff: %s", testResult.c_str());
                SetStartTime();
                testResult = WaitForEngineOffIgnitionOn();
                if(!testResult.compare(testPass))
                {                      // Reset the adaptive memory
                    Log(LOG_DEV_DATA, "EngineOffIgnitionOn: %s", testResult.c_str());
                    if(!GetParameterBool("DoNotPromptStartVehicle") && GetParameterInt("StartEnginePrompt") > 0)
                    {
                        DisplayPrompt(GetPromptBox("EngineRunning"), GetPrompt("EngineRunning"), GetPromptPriority("EngineRunning"));
                        BposSleep(GetParameterInt("StartEnginePrompt"));
                        RemovePrompt(GetPromptBox("EngineRunning"), GetPrompt("EngineRunning"), GetPromptPriority("EngineRunning"));
                    }
                }
                else if(!testResult.compare(testTimeout))
                {                      // Timeout waiting for conditions
                    testResult = testTimeout;
                    testResultCode = GetFaultCode("EngineOffIgnitionOnTimeoutFailure");
                    testDescription = GetFaultDescription("EngineOffIgnitionOnTimeoutFailure");
                    Log(LOG_ERRORS, "Timeout waiting engine off, ignition on");
                }
                else
                {                      // Bad system status
                    testResult = testAbort;
                    testResultCode = GetFaultCode("StatusCheckFailure");
                    testDescription = GetFaultCode("StatusCheckFailure");
                    Log(LOG_ERRORS, "Bad system status while waiting for engine off, ignition on - status: %s\n", 
                        ConvertStatusToResponse(StatusCheck()).c_str());
                }

            }
            else if(!testResult.compare(testTimeout))
            {                          // Timeout waiting for conditions
                testResult = testTimeout;
                testResultCode = GetFaultCode("EngineOffIgnitionOffTimeoutFailure");
                testDescription = GetFaultDescription("EngineOffIgnitionOffTimeoutFailure");
                Log(LOG_ERRORS, "Timeout waiting engine off, ignition off");
            }
            else
            {                          // Bad system status
                testResult = testAbort;
                testResultCode = GetFaultCode("StatusCheckFailure");
                testDescription = GetFaultCode("StatusCheckFailure");
                Log(LOG_ERRORS, "Bad system status while waiting for engine off, ignition off - status: %s\n", 
                    ConvertStatusToResponse(StatusCheck()).c_str());
            }
        }
        catch(ModuleException &e)
        {
            Log(LOG_ERRORS, "Error commanding module to reset adaptive memory - %s", e.GetReason());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }

        // Report the result
        SendTestResult(testResult, testDescription, testResultCode);    
    }
    else
    {                                  // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step - %s", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}

template <class ModuleType>
const string GenericTCTemplate<ModuleType>::WaitForEngineOffIgnitionOff(void)    
{
    string testResult(testFail);
    bool engineOffIgnitionOff = false;
    Log(LOG_FN_ENTRY, "GenericTCTemplate<ModuleType>::WaitForEngineOffIgnitionOff()  - Enter");
    SetStartTime();
    if (GetParameterBool("EngineOffPromptForPark"))
    {//Prompt driver to shift to park
        DisplayPrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
        BposSleep(GetTestStepInfoInt("ShiftToParkDisplayPrompt"));
        RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
    }
    do
    {   // Display prompt to put Engine off ignition off
        DisplayPrompt(GetPromptBox("EngineOffIgnitionOff"), GetPrompt("EngineOffIgnitionOff"), GetPromptPriority("EngineOffIgnitionOff"));
        if (IsIgnitionOn())
        {   // Wait before the next check
            BposSleep(GetTestStepInfoInt("ScanDelay"));
        }
        else
        {   // Ignition in correct position
            engineOffIgnitionOff = true;
        }
    } while (TimeRemaining() && !engineOffIgnitionOff && (BEP_STATUS_SUCCESS == StatusCheck()));
    // Remove the prompt
    RemovePrompt(GetPromptBox("EngineOffIgnitionOff"), GetPrompt("EngineOffIgnitionOff"), GetPromptPriority("EngineOffIgnitionOff"));
    // Check the exit conditions
    if (!TimeRemaining() && !engineOffIgnitionOff)
    {   // Timeout waiting engine off, ignition off
        testResult = testTimeout;
        Log(LOG_ERRORS, "Timeout waiting for ignition off engine off\n");
    }
    else
    {   // Determine the results of the test
        testResult = testPass;
        Log(LOG_ERRORS, "Ignition is Off and engine is off");
        //engine off ignition off delay
        INT32 ignitionoffWaitTime = GetParameterInt("EngineOffIgnitionOffWaitTime");
        BposSleep(ignitionoffWaitTime);
    }
    Log(LOG_FN_ENTRY, "GenericTCTemplate<ModuleType>::WaitForEngineOffIgnitionOff()  - Exit");
    return testResult;
}
