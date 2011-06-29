//******************************************************************************
// Description:
//   This defines the PlantHostOutbound object.
//
//===========================================================================
// Copyright (c) 2002-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/PlantHostOutbound/Include/PlantHostOutbound.h $
// 
// 1     11/01/07 3:11p Derickso
// Added capability to print a report if the overall test was passed, but
// there were flagged failures.
// 
// 2     10/01/07 2:04p Derickso
// Added option to only print flagged failures if the overall test result
// is pass.
// 
// 11    5/01/07 1:05p Rwiersem
// Changes for the 07050201 core release:
// 
// Updated to use the HostInterface class.
// 
// 10    1/18/07 2:57p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added a version of TranslateBuildRecord() that takes an additional 
//   XmlNodeMap parameter.
// - Added TranslateBuildRecordHook().
// 
// 9     7/05/06 10:06p Cward
// Finished data formatting,  Also fixed problems associated with Send
// Test Result String.
//
// 1     6/29/06 2:44p Rwiersem
// Added ApplyDataFormatting(), AdjustDataValueLength(),
// GetFormatAttribute(), GetDataTypeAttribute(), and
// GetDataLengthAttribute().  These are used to apply data formatting
// consistently across all data types.
//
// 8     5/08/06 7:44p Cward
// Fixed reload capability
//
// 1     4/27/06 1:10p Rwiersem
// Removed passing of INamedDataBroker pointer to the constructor.  Added
// SetNameDataBroker() to allow a MockINamedDataBroker to be used for
// testing.
//
// 7     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/18/06 2:16p Rwiersem
// Made the testing class a friend.  The constructor now takes an
// INamedDataBroker object.  This allows us to use a mock INamedDataBroker
// for testing.  Added SerialChannels for DAQ and DAQFault communication.
// These are now passed in when the DAQ objects are created.  Removed the
// old ReloadConfiguration() method.  Added HandlePulse() and
// LoadAdditionalConfigurationItems().  These are used to reload the
// configuration without restarting the process.
//
// 6     3/30/06 1:41a Cward
// Updated for sending machine faults to the DAQ. PE #80
//
// 5     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 4     2/16/05 9:34p Cward
// Added option to report aborted test results
//
// 3     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 2     8/18/04 11:47p Cward
// Added changes needed to use format tag for defining floating point
// value formats instead of length tag
//
// 1     7/21/04 10:18a Cward
//
// 3     5/25/04 4:37p Cward
// Added defines for ACK and NAK. Removed friend class references
//
// 2     3/04/04 5:26p Cward
// Commented out interface class forward definitions, and friend class
// definitions. Make Build Test Results String  method virtual Moved
// con_var and m_test public
//
// 1     1/27/04 4:53p Cward
//
// 1     8/21/03 9:09a Derickso
// 05WK CCRT Project
//
// 3     8/12/03 3:00p Khykin
// Updating with latest directory structure changes.
//
// 2     8/07/03 8:04a Khykin
// Updating with latest changes from Flatrock.
//
// 2     6/18/03 9:38a Derickso
// Made m_externalHostInterfaces Protected (was Private).
// Derived classes should have access so they can add their external host
// interface objects to the list.
//
// 1     6/12/03 4:19p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//
// 5     8/14/02 3:44p Skypig13
// Removed compiler warnings
//
// 4     7/29/02 8:21a Dan.erickson
// Reworked to spin up threads for each external host that needs
// test results.
//
// 3     5/20/02 4:36p Dan.erickson
// Made UpdateTestResultStringHeader() virtual.
// Modify prototypes with XmlNodeMap variables to take a reference.
// Added serialNumberLength constant.
//
// 2     4/08/02 2:53p Dan.erickson
// Switch Set methods to use DeepCopy instead of = for maps.
// Made destructor virtual.
//
// 1     4/03/02 2:05p Dan.erickson
// Created.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef PlantHostOutbound_h
#define PlantHostOutbound_h
//==============================================================================
// forward reference of class for use by DAQInterface.
class PlantHostOutbound;
//-----------------------------------------------------------------------------
#include <math.h>
#include "BepServer.h"
#include "BepSync.h"
#include "DAQInterface.h"
#include "GeneralUtilities.h"
#include "SerialChannel.h"
#include "IFaultServer.h"
#include "INamedDataBroker.h"
#include "PlantHostOutboundDefs.h"
//-----------------------------------------------------------------------------

/**
 * Data structure to hold the attributes of a host interface.
 * 
 * @author Dan Erickson
 */
typedef struct _HostInterface_t
{
    string interfaceName;   // Name of the host interface relative to /dev/name/local/core
    string processName;     // Name of the executable to be launched for this interface
    string registeredName;  // Name of interface will register as
    string interfacePath;   // Complete path to be adopted, ie. /dev/name/local/core/PFSInterface
    bool  reportAbortedTestResults;    // report aborted results per interface
    INT32  interfacePid;    // Process ID of the launched interface
} HostInterface_t;

//-----------------------------------------------------------------------------
typedef map<string, HostInterface_t*>     HostInterfaceMap;
typedef HostInterfaceMap::iterator        HostInterfaceMapItr;
typedef HostInterfaceMap::const_iterator  HostInterfaceMapCItr;

//-----------------------------------------------------------------------------

/**
 * Class to manage the host interfaces that get launched.
 * 
 * @author Dan Erickson
 * @see HostInterface_t
 */
class HostInterfaceProcessMap : private map<string, HostInterface_t*>
{
public:
    /**
     * Add a new host interface to the managed list of interfaces currently 
     * running.
     * 
     * @param interfaceName  Name of the host interface to add to the list.
     * @param registeredName Name the host interface will register as.
     * @param processName    Name of the host interface to use for the given 
     *                       interface name.
     * @param pid            Process ID of the spawned host interface.
     */
    void AddInterface(const string &interfaceName, 
                      const string &registeredName, 
                      const string &processName, 
                      const bool &reportAbortedTestResults, 
                      INT32 pid = -1);
    /**
     * Remove a host interface from the managed list of running interfaces.
     * 
     * @param interfaceName
     *               Name of the host interface to remove.
     */
    void RemoveInterface(const string &interfaceName);
    /**
     * Store the process ID of the spawned host interface process.
     * 
     * @param interfaceName Name of the interface to store the PID for.
     * @param pid           PID of the spawned interface.
     */
    void SetPid(const string &interfaceName, INT32 pid);
    /**
     * Return an iterator to the beginning of the managed host interface 
     * collection.
     * 
     * @return Iterator pointing to the beginning of the host interface 
     *         collection.
     */
    iterator begin();
    const_iterator begin() const;
    /**
     * Return the iterator pointing to the end of the host interface collection.
     * 
     * @return Iterator pointing to the end of the host interface collection.
     */
    iterator end();
    const_iterator end() const;
    /**
     * Find the item defined by the provided key.
     * 
     * @param key Item to be found.
     * 
     * @return Iterator to the requested item or end() if the item is not 
     *         contained in the collection.
     */
    iterator find(const key_type &key);
    const_iterator find(const key_type &key) const;
    /**
     * Return the number of items in the collection.
     * 
     * @return Number of items currently in the collection.
     */
    size_type size();
    /**
     * Remove all items from the managed collection.
     */
    void clear();
};

//-----------------------------------------------------------------------------

/**
 * PlantHostOutbound object.
 * This class is responsible for taking data from the system and translating it 
 * to a format usable by broadcast.  This class only handles data going from 
 * the Burke Porter system to the Plant Broadcast system.  The 
 * PlantHostOutbound object will subscribe for the TestResult node provided by 
 * the TestResultServer.  When the TestResult node is published, the 
 * PlantHostOutbound object will retreive the test results and parse the 
 * results for test failures and test aborts.  If any failures or aborts are 
 * found, an ErrorCodeTest node will be added to the test results.  The test 
 * results will then be passed off to any and all external host interface 
 * objects that have been spun up.  The external interface objects will build 
 * the result string and transmit it to the appropriate external host.
 *
 * @author Dan Erickson
 */
class PlantHostOutbound : public BepServer
{
    friend class PlantHostOutboundTest;

public:
    /** Class constructor. */
    PlantHostOutbound();

    /** Class destructor. */
    virtual ~PlantHostOutbound();

    /**
     * Method for initialization passing in the file name.
     *
     * @param fileName Name of the XML configuration file to be used to 
     *                 initialize.
     */
    virtual void Initialize(const string &fileName);
    /**
     * Method for initialization passing in the parsed configuration file.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * Process the reload configuration pulse.  If any other pulses are 
     * received, send them on to BepComponent.
     *
     * @param code  Pulse code
     * @param value Pulse value
     * 
     * @return If pulse is handled.
     *         Handled     == BEP_STATUS_SUCCESS
     *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);
    /**
     * Method that is called to command the component to perform any system 
     * registration functions necessary.
     *
     * @return Status of the registration process.
     *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
     *         BEP_SUCCESS_RESPONSE == Registered
     */
    virtual const string Register(void);
    /**
     * Main Run method to control the process
     *
     * @param terminateFlag Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
    /**
     * Get the most recent fault code that was stored.
     * 
     * @return Most recently stored fault code.
     */
    const string& GetFaultCode(void);
    /**
     * Set the named data broker object to be used by the plant host outbound.
     * This allows the class to be tested without having the full system
     * running.
     *
     * @param namedDataBroker - pointer to the INamedDataBroker object to use
     */
    void SetNamedDataBroker(INamedDataBroker *namedDataBroker);

protected:
    /** 
     * Launch all of the enabled host interfaces. 
     */
    void LaunchHostInterfaces(void);
    /**
     * Launch the specified host interface.
     * 
     * @param hostInt Host interface information for the host interface to 
     *                launch.
     * 
     * @return Status of launching the host interface.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Host interface was sucessfully launched. </li>
     *         <li> BEP_STATUS_FAILURE - Host interface could not be launched. </li>
     *         </ul>
     */
    BEP_STATUS_TYPE LaunchHostInterface(HostInterface_t *hostInt);
    /**
     * Wait for the specified host interface to finish launching and report 
     * back to us.
     * 
     * @param hostInt Host interface to wait for.
     * 
     * @return Status of the host interface.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Host interface successfully started up. </li>
     *         <li> BEP_STATUS_TIMEOUT - The host interface did not properly start up. </li>
     *         </ul>
     */
    BEP_STATUS_TYPE WaitForHostInterface(const HostInterface_t *hostInt);
    /**
     * Command each host interface to register.
     */
    void RegisterHostInterfaces(void);
    /**
     * Command the specified host interface to register.
     * 
     * @param hostInt Host interface to register.
     */
    void RegisterHostInterface(const HostInterface_t *hostInt);
    /**
     * Do any cleanup necessary when the server is commanded to terminate.
     */
    virtual void Terminate(void);
    /**
     * Command the specified host interface to shutdown.
     * 
     * @param hostInt Host interace to be terminated.
     */
    virtual void TerminateInterface(const HostInterface_t *hostInt);
    /**
     * Clear all entries from the host interface map.
     */
    void ClearHostInterfaceMap(void);
    /**
     * Send the test result data to each host interface for reporting to the 
     * plant host systems.
     * 
     * @param resultFileName Test result file to be processed by each interface.
     * @param overallResult  Overall test result.  This will be used to determine if the results 
     *                       should be sent to a specific host interface for processing.
     */
    void SendTestResultsToHostInterfaces(const string &resultFileName, const string overallResult);
    /**
     * Send the fault code data to each host interface for reporting to 
     * the plant host systems.
     */
    void SendFaultToHostInterfaces(void);
    /**
     * Store the host interface map.  The map should contain all available host 
     * interfaces.
     * 
     * @param hostIntMap Host interface map to store.  This map should define 
     *                   all available host interfaces.
     */
    void StoreHostInterfaceMap(const XmlNode *hostIntMap);
    /**
     * Load the additional configuration items needed by plant host outbound.
     * The "standard" items will be handled by the call to 
     * BepServer::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    /**
     * Handle the subscribed for data by the process.
     *
     * @param node Tag that was subscribed for
     * 
     * @return Status of the updating of the published tag
     *         Success, Fail
     */
    virtual const string Publish(const XmlNode *node);

private:
    /**
     * Write the updated results out to the specified file.  The size of the 
     * test result node prevents it from being written to other servers/host 
     * interfaces.
     * 
     * @param updatedResults Updated test results containing the error code 
     *                       data.
     * @param fileName       Name of the file to write the updated result data 
     *                       to.
     */
    void UpdateTestResultFile(const XmlNode *updatedResults, const string &fileName);
    /**
     * Set/Get the flag that indicates if aborted test results should be 
     * reported to the host system.
     * 
     * @param reportAbortedResult Flag indicating if reported test results 
     *                            should be sent to the host system.
     * 
     * @return Flag indicating if reported test results should be sent to the 
     *         host system.
     */
    const bool& ReportAbortedResults(const bool *reportAbortedResult = NULL);
    /**
     * Store/Retrieve the flag indicating if the flagged failures should be forced to print.
     * 
     * @param printFlaggedFailures
     *               Flag indicating if the flagged failures should be forced to print.
     * 
     * @return Flag indicating if the flagged failures should be forced to print.
     */
    const bool& PrintFlaggedFailures(const bool *printFlaggedFailures = NULL);
    /**
     * Force a printout for any flagged failures.
     * The printout will only be forced if the overall test result is set to Pass and the flag from the config file indicates
     * flagged failures should be printed.
     * 
     * @param overallResult
     *               Overall vehicle test result.
     * @param resultFileName
     *               Test result file name to use for generating the printout.
     */
    void ForceFlaggedFailurePrint(const string &overallResult, const string &resultFileName, const XmlNode *allFailures);
    /**
     * Force the print server to print a test report due to flagged failures existing in the test results.
     * 
     * @param resultFileName
     *               Name of the test result file to print.
     */
    void ForcePrintTestReport(const string &resultFileName);
    /**
     * Process the provided test result file name.
     * 
     * @param resultFileName
     *               Name of the test result file to process.
     */
    void ProcessTestResultFile(const string &resultFileName);
    const bool& SkipAddingErrorCodeTestsNode(const bool *printFlaggedFailures = NULL);
    /**
     * positive setting fault
     * negative clearing fault
     */
    string  m_faultCode;

    /** The current test results to be processed. */
    XmlNode *m_testResultNode;
    /**
     * Flag used to determine if a test should report its results even if the 
     * overall test result is abort.
     */
    bool m_reportAbortResults;
    /**
     * Flag used to determine if the print server should be commanded to print flagged failures.
     */
    bool m_printFlaggedFailures;
    /**
     * Map to hold the various host interface devices that are to be started.
     * 
     * @see HostInterfaceProcessMap
     */
    HostInterfaceProcessMap  m_hostInterfaceMap;
    /**
     * List of flagged failures that need a print our.  Typically the overall test will be pass even though these
     * items have been marked as fail.
     */
    XmlNodeMap m_flaggedFailuresToPrint;
    /**
     * Commands to send to the print server in order to force a print out for the flagged failures.
     */
    XmlNodeMap m_printServerCommands;
    /** List of test results to process */
    list<string> m_resultsToProcess;
    /** Mutex to coordinate access to the result list */
    BepMutex m_testResultMutex;
};

#endif // PlantHostOutbound_h
