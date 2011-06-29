//******************************************************************************
// Description:
//   This defines the PlantHostOutbound object.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/PlantHostOutbound/Library/PlantHostOutbound.cpp $
// 
// 1     11/01/07 3:11p Derickso
// Added capability to print a report if the overall test was passed, but
// there were flagged failures.
// 
// 2     10/01/07 2:04p Derickso
// Added option to only print flagged failures if the overall test result
// is pass.
// 
// 16    1/18/07 2:58p Rwiersem
// Changes for the 07011901 core release:
//                                        
// - Added a version of TranslateBuildRecord() that takes an additional
// XmlNodeMap parameter.
// - Added TranslateBuildRecordHook().
// 
// 15    10/26/06 1:16p Rwiersem
// Changes for the 06102701 core release:
// 
// Removed the call to LoadAdditionalConfigurationItems() in
// HandlePulse().
// This is now called from BepServer.
// 
// 14    7/05/06 10:07p Cward
// Finished data formatting,  Also fixed problems associated with Send
// Test Result String.
//
// 1     6/29/06 2:46p Rwiersem
// Added data formatting for vehicle build and value types in
// ProcessMapNode().  Updated GetTestDetailValue() to format all data
// types consistently.  Added ApplyDataFormatting(),
// AdjustDataValueLength(), GetFormatAttribute(), GetDataTypeAttribute(),
// and GetDataLengthAttribute().  These are used to apply data formatting
// consistently across all data types.
//
// 13    5/08/06 7:44p Cward
// Fixed reload capability
//
// 1     4/27/06 1:11p Rwiersem
// Removed passing of INamedDataBroker pointer to the constructor.  Added
// SetNameDataBroker() to allow a MockINamedDataBroker to be used for
// testing.
//
// 12    4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/18/06 2:21p Rwiersem
// The constructor now takes an INamedDataBroker object.  This allows us
// to use a mock INamedDataBroker for testing.  Added SerialChannels for
// DAQ and DAQFault communication.  These are now passed in when the DAQ
// objects are created.  Removed the old ReloadConfiguration() method.
// Added HandlePulse() and LoadAdditionalConfigurationItems().  These are
// used to reload the configuration without restarting the process.
//
// 11    4/13/06 4:26a Cward
// added try catch blocks
//
// 10    3/30/06 1:41a Cward
// Updated for sending machine faults to the DAQ. PE #80
//
// 9     2/15/06 8:03p Cward
// Increased time delay in publish for processing test results, and
// removed retry loop in send test result string.
//
// 8     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 7     7/20/05 9:23p Cward
// Added code to check for duplicate DTCs
//
// 6     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 5     5/09/05 5:00p Gpattison
// Re-arranged the Initialize() method to call the base class initialize
// as soon as possible
//
// 5     2/16/05 9:35p Cward
// Added option to report aborted test results.
//
// 4     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 3     8/18/04 11:50p Cward
// Removed duplicate log statement. Updated to use format tag instead of
// length tag for floating point values
//
// 2     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 1     7/21/04 10:18a Cward
//
// 3     5/25/04 4:36p Cward
// Removed processing of DTC's and module faults until bug can be
// corrected
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     1/27/04 4:53p Cward
//
// 2     10/20/03 10:11a Derickso
// Latest ServicesLayer update to correct for memory leaks and crashes.
//
// 1     8/21/03 9:09a Derickso
// 05WK CCRT Project
//
// 4     8/18/03 7:52a Khykin
// Updated with Flatrock changes.
//
// 3     8/12/03 3:00p Khykin
// Updating with latest directory structure changes.
//
// 2     8/07/03 2:42p Khykin
// Updated with changes from Flatrock.
//
// 3     6/24/03 7:59a Khykin
// Updated with Genes latest changes.
//
// 1     6/20/03 9:58a Khykin
// Initial checkin.
//
// 2     6/18/03 9:43a Derickso
// Added diagnostic statements to Initialize().
//
// 1     6/12/03 4:00p Khykin
// Initial checkin.
//
// 1     4/22/03 9:49a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//
// 16    4/03/03 11:36a Derickso
// Updated Publish method to check for test abort status before
// signaling the external interface objects to process the test
// results.  No test results will be reported if the overall test
// status was ABORT.
//
// 15    2/21/03 7:12p Derickso
// Modified GetTestResultDetail() to pad serial numbers with
// spaces instead of zero.
//
// 1     1/27/03 7:16a Khykin
// Initial Checkin
//
// 1     1/18/03 8:11a Khykin
// Initial Checkin From DCTC
//
// 14    11/21/02 10:18a Khykin
// Updated to use SerialString_t due someone deleted the SERIAL_STRING
// definition from the system.
//
// 13    11/20/02 10:00a Derickso
// Reverted to use SerialString_t macro.
//
// 11    10/16/02 10:25a Derickso
// Changed Loss compensation tag to use macro instead of config file.
//
// 10    9/26/02 2:02p Tbochene
// Removed compiler warnings
//
// 9     9/16/02 3:02p Derickso
// Modified SendTestResultString to translate the message to
// unsigned short int prior to sending the message.
//
// 7     8/23/02 10:26a Dan.erickson
// Updated BuildErrorCodeTestsNode moethod to use the AllFailures and DTC
// nodes.  Also modified method to look for error codes in TestResultCode
// attribute.
//
// 6     8/14/02 4:34p Skypig13
// Removed compiler warnings
//
// 5     8/14/02 3:45p Skypig13
// Removed compiler warnings
//
// 4     7/29/02 8:21a Dan.erickson
// Reworked to spin up threads for each external host that needs
// test results.
//
// 3     5/20/02 4:33p Dan.erickson
// Updated return value for Publish().
// Added management for XmlNodeMap variables.
// Updated logic to keep the correct count of serial numbers
// that are reported.
//
// 2     4/16/02 8:59a Dan.erickson
// Added diagnostic statements to BuildErrorCodeTestsNode().
//
// 1     4/03/02 2:04p Dan.erickson
// Created.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "PlantHostOutbound.h"
//-----------------------------------------------------------------------------
void HostInterfaceProcessMap::AddInterface(const string &interfaceName, const string &registeredName,
                                           const string &processName, const bool &reportAbortedTestResults, INT32 pid)
{
    HostInterface_t *hostInt = new HostInterface_t;
    hostInt->interfaceName = interfaceName;
    hostInt->registeredName = registeredName;
    hostInt->processName = processName;
    hostInt->reportAbortedTestResults = reportAbortedTestResults;
    hostInt->interfacePid = pid;
    insert(HostInterfaceMap::value_type(interfaceName, hostInt));
}

//-----------------------------------------------------------------------------
inline void HostInterfaceProcessMap::RemoveInterface(const string &interfaceName)
{
    HostInterfaceMapCItr iter = find(interfaceName);
    if(iter != end())
    {
        delete iter->second;
    }
    erase(interfaceName);
}

//-----------------------------------------------------------------------------
inline void HostInterfaceProcessMap::SetPid(const string &interfaceName, INT32 pid)
{
    HostInterfaceMapCItr iter = find(interfaceName);
    if(iter != end())
    {
        iter->second->interfacePid = pid;
    }
}

//-----------------------------------------------------------------------------
inline HostInterfaceMapItr HostInterfaceProcessMap::begin()
{
    return HostInterfaceMap::begin();
}

//-----------------------------------------------------------------------------
inline HostInterfaceMapCItr HostInterfaceProcessMap::begin() const
{
    return HostInterfaceMap::begin();
}

//-----------------------------------------------------------------------------
inline HostInterfaceMapItr HostInterfaceProcessMap::end()
{
    return HostInterfaceMap::end();
}

//-----------------------------------------------------------------------------
inline HostInterfaceMapCItr HostInterfaceProcessMap::end() const
{
    return HostInterfaceMap::end();
}

//-----------------------------------------------------------------------------
inline HostInterfaceMapItr HostInterfaceProcessMap::find(const key_type &key)
{
    return HostInterfaceMap::find(key);
}

//-----------------------------------------------------------------------------
inline HostInterfaceMapCItr HostInterfaceProcessMap::find(const key_type &key) const
{
    return HostInterfaceMap::find(key);
}

//-----------------------------------------------------------------------------
inline HostInterfaceMap::size_type HostInterfaceProcessMap::size()
{
    return HostInterfaceMap::size();
}

//-----------------------------------------------------------------------------
void HostInterfaceProcessMap::clear()
{   // Delete all entries prior to clearing the map
    for(HostInterfaceMapItr iter = begin(); iter != end(); iter++)
    {
        delete iter->second;
        iter->second = NULL;
    }
    HostInterfaceMap::clear();
}

//=============================================================================

//-----------------------------------------------------------------------------
PlantHostOutbound::PlantHostOutbound() : BepServer(), 
m_testResultNode(NULL), m_reportAbortResults(false)
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
PlantHostOutbound::~PlantHostOutbound()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::Initialize(const string &fileName)
{
    BepServer::Initialize(fileName);
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::Initialize(const XmlNode *document)
{
    // Initialize base server
    BepServer::Initialize(document);
    Log(LOG_FN_ENTRY, "PlantHostOutbound::Initialize() - Enter");
    // Load the additional configuration items need by plant host outbound
    LoadAdditionalConfigurationItems(document);
    Log(LOG_FN_ENTRY, "PlantHostOutbound::Initialize() - Exit");
}

//-----------------------------------------------------------------------------
const INT32 PlantHostOutbound::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 retVal = BEP_STATUS_FAILURE;

    // if this is a reload configuration pulse
    if((code == SYSTEM_PULSE) && (value == RELOAD_PULSE))
    {   // Command each host interface to terminate
        for(HostInterfaceMapItr iter = m_hostInterfaceMap.begin(); iter != m_hostInterfaceMap.end(); iter++)
        {   // Command the current interface to terminate
            TerminateInterface(iter->second);
        }
        // tell BepServer to reload the configuration
        ReloadConfiguration();         
        retVal = BEP_STATUS_SUCCESS;
    }
    else
    {   // let the BepComponent handle the other pulses
        retVal = BepComponent::HandlePulse(code, value);
    }
    // Return the result
    return(retVal);
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::LaunchHostInterfaces(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    for(HostInterfaceMapCItr iter = m_hostInterfaceMap.begin(); iter != m_hostInterfaceMap.end(); iter++)
    {
        status = LaunchHostInterface(iter->second);
        Log(LOG_DEV_DATA, "Launched host interface %s: %s", 
            iter->second->interfaceName.c_str(), ConvertStatusToResponse(status).c_str());
    }
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE PlantHostOutbound::LaunchHostInterface(HostInterface_t *hostInt)
{
    Log(LOG_FN_ENTRY, "PlantHostOutbound::LaunchHostInterface(%s)", hostInt->interfaceName.c_str());
    string parentArg("-P" + GetProcessName());
    string configArg("-f" + GetConfigFile());
    string nameArg("-p" + hostInt->interfaceName);
    // Launch the process for the current host interface
    Log(LOG_DEV_DATA, "Launching %s %s %s %s", 
        hostInt->interfaceName.c_str(), configArg.c_str(), parentArg.c_str(), nameArg.c_str());
    hostInt->interfacePid = spawnl(P_NOWAIT, hostInt->processName.c_str(), hostInt->processName.c_str(), 
                                   configArg.c_str(), parentArg.c_str(), nameArg.c_str(), NULL);
    // Determine if the host interface was launched
    BEP_STATUS_TYPE launchStatus = (hostInt->interfacePid != -1) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
    if(BEP_STATUS_SUCCESS == launchStatus)
    {   // Wait for the device to finish launching
        BposSleep(250);   // Wait for the process to start
        launchStatus = WaitForHostInterface(hostInt);
    }
    else
    {   // Error launching the host interface
        Log(LOG_ERRORS, "Could not launch %s: %s", hostInt->interfaceName.c_str(), strerror(errno));
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "PlantHostOutbound::LaunchHostInterface(%s) - host interface started: %s", 
        hostInt->interfaceName.c_str(), ConvertStatusToResponse(launchStatus).c_str());
    return launchStatus;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE PlantHostOutbound::WaitForHostInterface(const HostInterface_t *hostInt)
{
    Log(LOG_FN_ENTRY, "Waiting for %s (%s) - pid: %d", 
        hostInt->interfaceName.c_str(), hostInt->registeredName.c_str(), hostInt->interfacePid);
    // Get the time at which the host interface was launched
    struct timespec timerspec;
    clock_gettime(CLOCK_REALTIME, &timerspec);
    uint64_t startTime = timespec2nsec(&timerspec);
    // Setup a 10 second timeout waiting for the host interface we launched
    uint64_t timeout = mSEC_nSEC(10000);
    struct sigevent event;
    event.sigev_notify = SIGEV_UNBLOCK;
    TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, &event, &timeout, NULL);
    Log(LOG_DEV_DATA, "Set timer to timeout after %dms", nSEC_mSEC(timeout));
    // Wait for the device to be up and running
    struct _pulse pulse;
    uint64_t currentTime;
    bool doneWaiting = false;
    INT32 hostIntId;
    BEP_STATUS_TYPE launchStatus = BEP_STATUS_ERROR;
    while((MsgReceive(hostInt->interfacePid, &pulse, sizeof(pulse), NULL) != -1) && !doneWaiting)
    {   // Get the current time
        clock_gettime(CLOCK_REALTIME, &timerspec);
        currentTime = timespec2nsec(&timerspec);
        Log(LOG_DEV_DATA, "Host Interface %s sent pulse - %d:%d", 
            hostInt->interfaceName.c_str(), pulse.code, pulse.value.sival_int);
        // Check why we woke up
        if((currentTime - startTime) >= timeout)
        {   // Timeout waiting for host interface to register
            Log(LOG_ERRORS, "Host interface % failed to register %s in time: %s",
                hostInt->interfaceName.c_str(), hostInt->registeredName.c_str(), strerror(errno));
            doneWaiting = true;
            launchStatus = BEP_STATUS_TIMEOUT;
        }
        else if((hostIntId = name_open(hostInt->registeredName.c_str(), 0)) != -1)
        {
            Log(LOG_DEV_DATA, "Host interface %s registered name %s", 
                hostInt->interfaceName.c_str(), hostInt->registeredName.c_str());
            doneWaiting = true;
            launchStatus = BEP_STATUS_SUCCESS;
        }
        else
        {   // Some other pulse
            BepComponent::HandlePulse(pulse.code, pulse.value.sival_int);
            // Reset the timer to the mount of time remaining
            currentTime = timeout - (currentTime - startTime);
            TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, &event, &currentTime, NULL);
        }
    }
    if(!doneWaiting)
    {
        Log(LOG_ERRORS, "Something really bad just happened - strerror:%s", strerror(errno));
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "Done waiting for %s: %s", hostInt->interfaceName.c_str(), ConvertStatusToResponse(launchStatus).c_str());
    return launchStatus;
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::RegisterHostInterfaces(void)
{   // Register all enabled host interfaces
    for(HostInterfaceMapItr iter = m_hostInterfaceMap.begin(); iter != m_hostInterfaceMap.end(); iter++)
    {  
        RegisterHostInterface(iter->second);
    }
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::RegisterHostInterface(const HostInterface_t *hostInt)
{
    Log(LOG_FN_ENTRY, "PlantHostOutbound::RegisterHostInterface(%s) - Enter", hostInt->interfaceName.c_str());
    // Create a comm object to the specified interface
    IBepCommunication hostIntComm;
    hostIntComm.Initialize(hostInt->registeredName, "Client");
    hostIntComm.SetTimeout(5000);
    string registerStatus, response;
    INT32 status = hostIntComm.Register(BEP_REGISTER, registerStatus, response, true);
    Log(LOG_FN_ENTRY, "PlantHostOutbound::RegisterHostInterface(%s) - Exit: %s", 
        hostInt->interfaceName.c_str(), ConvertStatusToResponse(status).c_str());
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::Terminate(void)
{
    Log(LOG_FN_ENTRY, "PlantHostOutbound::Terminate() - Enter");
    // Command each host interface to terminate
    for(HostInterfaceMapItr iter = m_hostInterfaceMap.begin(); iter != m_hostInterfaceMap.end(); iter++)
    {   // Command the current interface to terminate
        TerminateInterface(iter->second);
    }
    // Clear the host interface map
    ClearHostInterfaceMap();
    // Call the base class to complete the terminating
    BepServer::Terminate();
    Log(LOG_FN_ENTRY, "PlantHostOutbound::Terminate() - Exit");
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::TerminateInterface(const HostInterface_t *hostInt)
{
    IBepCommunication hostIntComm;
    string response;
    Log(LOG_FN_ENTRY, "PlantHostOutbound::TerminateInterface(%s)", hostInt->interfaceName.c_str());
    // Initialize the comm object so we can talk to the host interface
    hostIntComm.Initialize(hostInt->registeredName, "Client");
    hostIntComm.SetTimeout(5000);
    // Command the host interface to terminate and log the result
    INT32 status = hostIntComm.Terminate(response);
    Log(LOG_FN_ENTRY, "Terminated %s: %s", hostInt->interfaceName.c_str(), ConvertStatusToResponse(status).c_str());
}

//-----------------------------------------------------------------------------
inline void PlantHostOutbound::ClearHostInterfaceMap(void)
{
    m_hostInterfaceMap.clear();
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::SendTestResultsToHostInterfaces(const string &resultFileName, const string overallResult)
{
    IBepCommunication hostIntComm;
    string response;
    Log(LOG_FN_ENTRY, "PlantHostOutbound::SendTestResultsToHostInterfaces() - Enter");
    // Send the test result data to each host interface
    for(HostInterfaceMapItr iter = m_hostInterfaceMap.begin(); iter != m_hostInterfaceMap.end(); iter++)
    {   //If test result is not abort or report aborted results parameter is set
        if((overallResult != BEP_ABORT_STATUS) || (iter->second->reportAbortedTestResults))
        {
            // Initialize a comm object to the current host interfce process
            hostIntComm.Initialize(iter->second->registeredName, "Client");
            hostIntComm.SetTimeout(5000);
            Log(LOG_DEV_DATA, "Sent test result to %s for procesing: %s", 
                iter->second->interfaceName.c_str(), 
                ConvertStatusToResponse(hostIntComm.Write(TEST_RESULT_TAG, resultFileName, response, true)).c_str());
        }
        else
        {
            Log(LOG_DEV_DATA, "Not sending result to host interface: ReportAbortedTestResults parameter set to false"); 
        }
    }
    Log(LOG_FN_ENTRY, "PlantHostOutbound::SendTestResultsToHostInterfaces() - Exit");
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::SendFaultToHostInterfaces(void)
{
    IBepCommunication hostIntComm;
    string response;
    Log(LOG_FN_ENTRY, "PlantHostOutbound::SendFaultToHostInterfaces() - Enter");
    // Send the data data to each host interface
    XmlNode faultData(faultCodeTag, GetFaultCode());
    for(HostInterfaceMapItr iter = m_hostInterfaceMap.begin(); iter != m_hostInterfaceMap.end(); iter++)
    {   // Initialize a comm object to the current host interfce process
        hostIntComm.Initialize(iter->second->registeredName, "Client");
        hostIntComm.SetTimeout(5000);
        Log(LOG_DEV_DATA, "Sent fault data to %s for procesing: %s", 
            iter->second->interfaceName.c_str(), 
            ConvertStatusToResponse(hostIntComm.Write(&faultData, response, true)).c_str());
    }
    Log(LOG_FN_ENTRY, "PlantHostOutbound::SendFaultToHostInterfaces() - Exit");
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::StoreHostInterfaceMap(const XmlNode *hostIntMap)
{
    Log(LOG_FN_ENTRY, "PlantHostOutbound::StoreHostInterfaceMap() - Enter");
    // Clear out the current interface map if it is not already empty
    if(m_hostInterfaceMap.size() > 0)
    {   // Remove all host interfaces
        m_hostInterfaceMap.clear();
    }
    // Store all enabled interfaces
    for(XmlNodeMapCItr iter = hostIntMap->getChildren().begin(); iter != hostIntMap->getChildren().end(); iter++)
    {   // Determine if the current interface is enabled
        if(atob(iter->second->getAttribute("Enabled")->getValue().c_str()))
        {   // Interface is enabled, store all info
            const string interfaceName(iter->second->getName());
            string interfaceProcess(iter->second->getAttribute("Process")->getValue());
            // Expand any environment varibales in the process string
            FixUpPathString(interfaceProcess);
            // Set the registered name of the host interface
            string registeredName;
            try
            {
                registeredName = iter->second->getChild("Setup/Logging/ProcessName")->getValue();
            }
            catch(XmlException &excpt)
            {
                registeredName = "core/" + interfaceName;
            }
            bool reportAbortedTestResults;
            try
            {
                reportAbortedTestResults = atob(iter->second->getChild("Setup/HIReportAbortedTestResults")->getValue().c_str());
            }
            catch(XmlException &excpt)
            {
                reportAbortedTestResults = true;
            }
            // Add the interface to the collection of managed host interfaces
            m_hostInterfaceMap.AddInterface(interfaceName, registeredName, interfaceProcess, reportAbortedTestResults);
            Log(LOG_DEV_DATA, "Added host interface: %s (%s) using process %s report aborted results: %s", 
                interfaceName.c_str(), registeredName.c_str(), interfaceProcess.c_str(), 
                reportAbortedTestResults ? "True" : "False");
        }
        else
        {   // Interface is not enabled, skip this interface
            Log(LOG_DEV_DATA, "%s host interface not enabled", iter->second->getName().c_str());
        }
    }
    Log(LOG_FN_ENTRY, "PlantHostOutbound::StoreHostInterfaceMap() - Exit");
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    // Save the List of external host interfaces
    try
    {
        StoreHostInterfaceMap(document->getChild("Setup/HostInterfaces"));
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "Error loading HostInterfaces - %s", e.GetReason());
    }
    // Set Report Abort test results flag to true or false
    bool reportAbortedResult = false;
    try
    {
        reportAbortedResult = atob(document->getChild("Setup/ReportAbortedTestResults")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Report aborted test results not defined, defaulting to not report aborted result: %s", excpt.GetReason());
    }
    ReportAbortedResults(&reportAbortedResult);
    // Set the print flagged failures flag
    bool printFlaggedFailures = false;
    try
    {
        printFlaggedFailures = atob(document->getChild("Setup/PrintForFlaggedFailures")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Print Flagged Failures option not found, not printing flagged failures: %s", excpt.GetReason());
        printFlaggedFailures = false;
    }
    // Determine if remaining flagged faliure data should be set
    if(printFlaggedFailures)
    {
        try
        {
            m_flaggedFailuresToPrint.DeepCopy(document->getChild("Setup/FlaggedFailuresToPrint")->getChildren());
            m_printServerCommands.DeepCopy(document->getChild("Setup/PrintServerCommands")->getChildren());
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "Could not find flagged failure setup data, not printing flagged failures: %s", excpt.GetReason());
            printFlaggedFailures = false;
        }
    }
    PrintFlaggedFailures(&printFlaggedFailures);
}

//-----------------------------------------------------------------------------
const string PlantHostOutbound::Register(void)
{
    string response(BepServer::Register());

    Log(LOG_DEV_DATA, "PlantHostOutbound::Register() - begin");
    // Launch all host interfaces
    LaunchHostInterfaces();
    // Register all host interfaces
    RegisterHostInterfaces();
    // Call the base class register method
    Log(LOG_DEV_DATA, "No more interfaces to configure - PlantHostOutbound::Register() - end");
    return response;
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::Run(volatile bool *terminateFlag/*=NULL*/)
{   // Run forever until told to terminate
    while(GetStatus() != BEP_TERMINATE)
    {   // Check if there is a result ready to process
        if((errno = m_testResultMutex.Acquire()) == EOK)
        {   // Make sure there is something to process
            if(m_resultsToProcess.size() > 0)
            {   // Get a local copy of the result file name and free up the mutex
                string resultFile = m_resultsToProcess.front();
                m_resultsToProcess.pop_front();
                m_testResultMutex.Release();
                // Process the test result
                ProcessTestResultFile(resultFile);
            }
            else
            {   // Just release the mutex
                m_testResultMutex.Release();
            }
        }
        else
        {
            Log(LOG_DEV_DATA, "Could not access results to process: %s", strerror(errno));
        }
        // Wait before checking again
        BposSleep(500);
    }
}

//-----------------------------------------------------------------------------
const string PlantHostOutbound::Publish(const XmlNode *node)
{
    XmlParser parser;
    Log(LOG_DEV_DATA, "PlantHostOutbound::Publish(%s)", node->ToString().c_str());
    string result = BEP_SUCCESS_RESPONSE;
    (void)BepServer::Publish(node);
    if(node->getName() == BEP_TEST_RESULT)
    {   
        // Add the test result to the list of results to process
        if((errno = m_testResultMutex.Acquire()) == EOK)
        {
            m_resultsToProcess.push_back(node->getValue().c_str());
            m_testResultMutex.Release();
            result = BEP_SUCCESS_RESPONSE;
            Log(LOG_DEV_DATA, "Added %s to processing list", node->getValue().c_str());
        }
        else
        {   // Could not add the test result to the processing list
            Log(LOG_ERRORS, "Could not add %s to the processing list: %s", node->getValue().c_str(), strerror(errno));
            result = BEP_FAIL_RESPONSE;
        }
    }
    // Need to update for fault handling to the data collector
    else if('_' == node->getName()[0])
    {   // We have a machine fault
        Log(LOG_DEV_DATA,"Received Fault from publication=> %s", node->ToString().c_str());
        // store the fault code where the DAQ can see it
        const XmlNode *locNode = node->getChildren().getNode(0);
        string faultCode = locNode->getAttribute(BEP_DESCRIPTION)->getValue();
        // validate that description has an imbedded fault code number
        if('#' == faultCode[0])
        {
            m_faultCode = faultCode.substr(1,4).c_str();
        }
        else
        {
            m_faultCode = "0";
        }
        if(locNode->getValue() == "0")
        {
            m_faultCode = locNode->getValue();
        }
        Log(LOG_DEV_DATA,"Saved Fault as=> %s", m_faultCode.c_str());
        // Send the fault to all host interfaces for processing
        SendFaultToHostInterfaces();
    }
    Log(LOG_DEV_DATA, "PlantHostOutbound::Publish() - Exited\n");
    return(BEP_SUCCESS_RESPONSE);
}

//-----------------------------------------------------------------------------
inline const string& PlantHostOutbound::GetFaultCode(void)
{
    return m_faultCode;
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::UpdateTestResultFile(const XmlNode *updatedResults, const string &fileName)
{
    FILE *resultFile = NULL;
    // Attempt to open the file for writing
    string fullName(getenv("USR_ROOT") + fileName);
    if((resultFile = fopen(fullName.c_str(), "w")) != NULL)
    {   // File has been open, write the data
        fprintf(resultFile, "%s", updatedResults->ToString().c_str());
        fclose(resultFile);
    }
    else
    {   // Could not open the file, updated results not written
        Log(LOG_ERRORS, "Could not open %s for writing updated results: %s", strerror(errno));
    }
}

//-----------------------------------------------------------------------------
const bool& PlantHostOutbound::ReportAbortedResults(const bool *reportAbortedResult /*= NULL*/)
{
    if(reportAbortedResult != NULL)  m_reportAbortResults = *reportAbortedResult;
    return m_reportAbortResults;
}

//-----------------------------------------------------------------------------
const bool& PlantHostOutbound::PrintFlaggedFailures(const bool *printFlaggedFailures /*= NULL*/)
{
    if(printFlaggedFailures != NULL)  m_printFlaggedFailures = *printFlaggedFailures;
    return m_printFlaggedFailures;
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::ForceFlaggedFailurePrint(const string &overallResult, 
                                                 const string &resultFileName, 
                                                 const XmlNode *allFailures)
{   // Check if the overall test is pass, if so, check if there are valid flagged failures
    if(!overallResult.compare(BEP_PASS_RESPONSE) && 
       PrintFlaggedFailures() && 
       (allFailures->getChildren().size() > 0))
    {   // Overall result is pass, check if any flagged failures
        bool flaggedFailuresExist = false;
        const XmlNodeMap &testFailures = allFailures->getChildren();
        for(XmlNodeMapItr iter = m_flaggedFailuresToPrint.begin(); 
             (iter != m_flaggedFailuresToPrint.end()) && !flaggedFailuresExist; 
             iter++)
        {
            flaggedFailuresExist = (testFailures.find(iter->second->getName()) != testFailures.end());
        }
        // If flagged failures, force a printout
        if(flaggedFailuresExist)
        {
            ForcePrintTestReport(resultFileName);
        }
    }
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::ForcePrintTestReport(const string &resultFileName)
{
    IBepCommunication printComm;
    Log(LOG_DEV_DATA, "Forcing print of %s", resultFileName.c_str());
    for(XmlNodeMapItr iter = m_printServerCommands.begin(); iter != m_printServerCommands.end(); iter++)
    {   // Build the command to send
        const XmlNodeMap &attribs = iter->second->getAttributes();
        // Check for the process name
        XmlNodeMapCItr currentAttrib = attribs.find("process");
        if(currentAttrib != attribs.end())
        {   // Initialize the comm object to the given process name
            printComm.Initialize(currentAttrib->second->getValue(), "Client");
            // Get the data tag and the value
            currentAttrib = attribs.find("tag");
            if(currentAttrib != attribs.end())
            {
                string tag(currentAttrib->second->getValue());
                string value("");
                currentAttrib = attribs.find("value");
                if(currentAttrib != attribs.end())
                {
                    if(!currentAttrib->second->getValue().compare("TestResultFileName"))
                    {
                        value = resultFileName;
                    }
                    else
                    {
                        value = currentAttrib->second->getValue();
                    }
                }
                // Send the data off to the server
                string response;
                INT32 status = printComm.Publish(tag, value, response, true);
                Log(LOG_DEV_DATA, "Sent data to print server (tag: %s, value: %s) - result: %s",
                    tag.c_str(), value.c_str(), ConvertStatusToResponse(status).c_str());
            }
            else
            {
                Log(LOG_ERRORS, "No data tag found, not forcing print of %s", resultFileName.c_str());
            }
        }
        else
        {
            Log(LOG_ERRORS, "No print server supplied, not forcing print of %s", resultFileName.c_str());
        }
    }
}

//-----------------------------------------------------------------------------
void PlantHostOutbound::ProcessTestResultFile(const string &resultFileName)
{
    XmlParser parser;
    // Get the test result from the file
    try
    {   // Free memory from last time
        if( m_testResultNode != NULL)
        {
            delete m_testResultNode;
            m_testResultNode = NULL;
        }
        m_testResultNode = parser.ReturnXMLDocument(getenv("FTP_ROOT") + resultFileName)->Copy();
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "PlantHostOutbound::Publish() Exception: %s", e.what());
    }
    // Check to see if this was an aborted test
    if( m_testResultNode != NULL)
    {
        string overallTestResult = m_testResultNode->getChild(BEP_TEST_RESULT)->getChild("OverallTest")->getAttribute(BEP_RESULT)->getValue();
        Log(LOG_DEV_DATA, "Received Test Result - OverallTest: %s\n", overallTestResult.c_str());
        if((overallTestResult != BEP_ABORT_STATUS) || ReportAbortedResults())
        {   // Write the test result node to all host interfaces
            SendTestResultsToHostInterfaces(resultFileName, overallTestResult);
            // Print flagged failures
            ForceFlaggedFailurePrint(overallTestResult, 
                                     resultFileName, 
                                     m_testResultNode->getChild(BEP_TEST_RESULT)->getChild("AllFailures"));
        }
        else
            Log(LOG_DEV_DATA, "Overall Test Status was %s, not reporting any results\n", BEP_ABORT_STATUS);
    }
    else
    {
        Log(LOG_ERRORS, "Test result node was NULL\n");
    }
}
