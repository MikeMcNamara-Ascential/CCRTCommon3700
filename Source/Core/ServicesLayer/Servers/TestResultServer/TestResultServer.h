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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/TestResultServer/TestResultServer.h $
// 
// 12    10/31/07 10:08a Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 11    10/26/06 1:21p Rwiersem
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
// 10    9/22/06 11:17a Rwiersem
// Changes for the 06092201 core release:
//     - Added GetKeyPressTag()
//     - Added OpenAndReadInResultsFile()
//     - Added SetKeyPressTag()
//     - Added m_keyPressTag
// 
// 9     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/11/06 2:08p Rwiersem
// Added LoadAdditionalConfigurationItems() and HandlePulse() methods.
// These are used to reload the configuration without restarting the
// process.
//
// 8     4/13/06 4:30a Cward
// Added code  to allow the ICM speed and force arrays to be saved to the
// test result file as CDATA nodes
//
// 2     3/20/06 11:01a Bmeinke
// Added code  to allow the ICM speed and force arrays to be saved to the
// test result file as CDATA nodes
// 7     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 6     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     3/02/05 1:39p Gswope
// added m_clearBuildOnOverall
//
// 5     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 4     4/28/04 10:54p Cward
// Changes associated with build 040402801
//
// 3     1/27/04 4:49p Cward
// Changes resulting from DE's testing and RW's trip
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:29p Cward
//
// 3     10/24/03 1:51p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/07/03 8:09a Khykin
// Updated with changes from Flatrock.
//
// 1     6/12/03 4:25p Khykin
// Initial checkin.
//
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
//
// 3     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:34  tbochene
//
//    PooBag
//
//
// 2     2/08/03 9:55a Khykin
// Updated for new TestResult publishing to the GUI.
//
// 1     1/27/03 7:10a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 21    11/26/02 5:08p Derickso
// Added use of NO VIN if a vin was not provided.
//
// 20    10/23/02 12:40p Derickso
// Removed remote path items.  No longer needed.  Using FTP_ROOT instead.
//
// 19    10/15/02 3:19p Derickso
// Added NULL check in GetVehicleBuild() to prevent Sig Segs
//
// 18    8/23/02 9:08a Dan.erickson
// Allow addition of vehicle build information.
// Added DTC reporting capabilities.
//
// 17    7/29/02 8:19a Dan.erickson
// Added support for LossCompensation results.
//
// 16    6/07/02 12:01p Dan.erickson
// Made Terminate method virtual.
//
// 15    5/20/02 4:43p Dan.erickson
// Added methods to set and get the ClearScrollBox tag.
// Modified methods to takes references.
//
// 14    4/25/02 4:42p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:03  erickson
//    started
//
//
// 12    3/26/02 7:12a Dan.erickson
// Increased the maximum number of test results file allowed
// from 1000 to 10000.
//
// 11    3/20/02 11:07a Dan.erickson
// Changed m_allFailures from XmlNode* to XmlNode.
// Changed OverallPassFail to OverallTest.
// Overloaded Initialize(std::string &fileName) method.
// Removed GetName() method.  It is a base class method and
// does not need to be overloaded.
// Restored GetSpecialInfo() method so nodes have appropriate
// attribute values when they are returned from a Read().
//
// 10    3/05/02 5:04p Dan.erickson
// Added support for an AllFailures tag.  This tag will keep track of all
// current test failures and report to subscribers.
//
// 9     2/18/02 3:36p Skypig13
// Updated for new BepServer
//
// 8     12/12/01 9:05a Dan.erickson
// Added accessor and modifier methods for member variables.
// Added comments for constant variables.
// Added variables and methods for handling vehicle build.
//
// 7     11/13/01 7:51a Dan.erickson
// Changed vehicleTestStatus to overallPassFail.
// Removed SetTestResultsTag method.
// Added SetNumberOfTests method.
//
// 6     11/08/01 12:11p Dan.erickson
// Added Terminate method.
//
// 5     11/07/01 4:55p Dan.erickson
// Made TestResultServerTest class a friend.
//
// 4     11/07/01 10:22a Dan.erickson
// Changed inheritance from IInfoCommunication to ITestResultServer.
// Added GetName method.
//
// 3     11/06/01 7:40a Dan.erickson
// Testing.
//
// 2     11/01/01 2:56p Dan.erickson
// Compiles, not tested.
//
// 1     10/24/01 1:23p Dan.erickson
// Created.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef TestResultServer_h
#define TestResultServer_h
//==============================================================================

#include "BepSemaphore.h"
#include "BepServer.h"
#include "BepTimer.h"
#include "BepSync.h"
#include "IQnxDataServer.h"
#include "ITestResultServer.h"
#include <list>

/** Root tag to use in the vehicle test result file. */
#define BEP_VEHICLE_TEST_FILE "BepVehicleTestResult"
/** Tag to use for the Vin */
#define VIN_TAG               "VIN"

/** Length of the Vin */
const INT16 defaultVINLength = 17;
/** Length of the sequence number. */
const INT16 sequenceLength = 4;
/** Maximum sequence number to use before rolling over to 0. */
const INT16 maxSequenceNumber = 9999;
/** Name to use if a VIN was not specified. */
const string noVIN("NoVIN");
/** Condvar value to signal start of result processing. */
const string processTestResults("ProcessTestResults");
/** Condvar value to signal result processing has been completed */
const string resultProcessingComplete("ResultProcessingComplete");

/**
 * Server to keep track of vehicle component test results.
 * Publishes all results to subscribers at the end of the test.
 *
 * @author Dan Erickson
 * @version Version 2.7
 * @see IInfoServer
 * @since Version 1.0
 */
class TestResultServer : public BepServer
{
    friend class TestResultServerTest;

public:
    /**
     * Default constructor
     */
    TestResultServer(void);
    /**
     * Destructor
     */
    virtual ~TestResultServer(void);
    /**
     * Method for initialization passing in the file name
     *
     * @param fileName Name of the XML configuration file to be used to initialize
     */
    virtual void Initialize(const string &fileName);
    /**
     * Set up the test result server.
     *
     * @param document Configuration file for the test result server.
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * <b>Description:</b>
     * Method that is called to command the component to perform any system 
     * registration functions necessary.
     *
     * @return BEP_SUCCESS_RESPONSE if the registration was successful
     *         BEP_FAILURE_RESPONSE if the registration was not successful
     */
    virtual const string Register(void);
    /**
     * Method used to terminate the component.  This performs
     * any cleanup that the destructor would not and sets
     * the state of the component to terminate.
     */
    virtual void Terminate(void);
    /**
     * <b>Description:</b>
     * Main run method.  This method runs until the status is BEP_TERMINATE.
     *
     * @param terminateFlag Flag used to terminate the system.  (this is unused)
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     */
    virtual const string Read(const XmlNode *node, const INT32 rate = 0);
    /**
     * Request to write a data tag
     *
     * @param node  Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     */
    virtual const string Write(const XmlNode *node);
	/**
	 * Process the reload configuration pulse.  If any other pulses
     * are received, send them on to BepComponent.
	 *
	 * @param code   Pulse code
	 * @param value  Pulse value
	 * @return If pulse is handled.
	 *         Handled     == BEP_STATUS_SUCCESS
	 *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
	 */
	virtual const INT32 HandlePulse(const INT32 code, const INT32 value);
    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     */
    virtual const string Publish(const XmlNode *node);
    /**
     * Return the tag being used to signify start of test.
     *
     * @return Start test tag.
     */
    inline const string GetStartTestTag()
    {
        return(m_startTestTag);
    };
    /**
     * Return the tag being used for the vin.
     *
     * @return Vin tag.
     */
    inline const string GetVinTag()
    {
        return(m_vinTag);
    };
    /**
     * Return the vin of the vehicle being tested.
     *
     * @return Vin
     */
    inline string GetCurrentVin()
    {
        return(m_currentVIN);
    };
    /**
     * Return the tag being used for vehicle build
     *
     * @return Vehicle build tag.
     */
    inline const string GetVehicleBuildTag()
    {
        return(m_vehicleBuildTag);
    };
    /**
     * Return the tag being used for all failures
     *
     * @return AllFailures tag.
     */
    inline const string GetAllFailuresTag()
    {
        return(m_allFailuresTag);
    };
    /**
     * Return the tag being used to clear the GUI scroll box.
     *
     * @return ClearScrollBox tag.
     */
    inline const string GetClearScrollBoxTag()
    {
        return(m_clearScrollBoxTag);
    };
    /**
     * Return the vehicle build record.
     *
     * @return String representation of the build record.
     */
    inline const string GetVehicleBuild()
    {
        string buildInfo;
        m_semVehicleBuild.CriticalSectionEnter();   // wait till can update vehicle build
        // If the node is NULL, create a new one
        if (m_vehicleBuild == NULL) m_vehicleBuild = new XmlNode(VEHICLE_BUILD_TAG, "");
        buildInfo = m_vehicleBuild->ToString();
        m_semVehicleBuild.CriticalSectionExit();
        // Return a string representation of the node
        return (buildInfo);
    };
    /**
     * Return the tag being used for vehicle test status.
     *
     * @return Vehicle test status tag
     */
    inline const string GetOverallTestTag()
    {
        return(m_overallTestTag);
    };
    /**
     * Provide the name of the file containing the list of
     * vehicles tested.
     *
     * @return m_testResultFile
     */
    inline const string GetTestResultFile()
    {
        return(m_testResultFile);
    };
    /**
     * Provide the path to the test results file.
     *
     * @return m_testResultPath
     */
    inline const string GetTestResultPath()
    {
        return(m_testResultPath);
    };
    /**
     * Provide the number of tests currently being archived.
     *
     * @return m_numberOfTests
     * @since Version 1.0
     */
    inline UINT16 GetNumberTests()
    {
        return(m_numberOfTests);
    };
    /**
     * Get the name of the driver number tag.
     *
     * @return Name of the tag.
     * @since Version 2.3
     */
    inline const string GetDriverNumberTag(void)
    {
        return(m_driverNumberTag);
    };
    /**
     * Get the name of the drive curve tag.
     *
     * @return Name of the tag.
     * @since Version 2.3
     */
    inline const string GetDriveCurveTag(void)
    {
        return(m_driveCurveTag);
    };
    /**
     * Get the name of the key press tag.
     *
     * @return Name of the tag.
     * @since AUG-15-2006
     */
    inline const string GetKeyPressTag(void)
    {
        return(m_keyPressTag);
    };
    /**
     * Store the state of the testing process.
     * Valid states are:
     *    - True, vehicle test is currently in progress.
     *    - False, no vehicle test in progress.
     *
     * @param testInProgress
     *               Testing state.
     * @since Version 2.3
     */
    inline void SetTestInProgress(const bool &testInProgress)
    {
        m_testInProgress = testInProgress;
    };
    /**
     * Return the test in progress state.
     *
     * @return True, test is in progress.
     *         False, no test currently in progress.
     * @since Version 2.3
     */
    inline const bool IsTestInProgress(void)
    {
        return(m_testInProgress);
    };
    /**
     * Return the vin value for loss compensation testing.
     *
     * @return Vin value.
     * @since Version 2.3
     */
    inline const string GetLossCompensationVin(void)
    {
        return(m_lossCompensationVin);
    };
    /**
     * Get the length of the VIN being used.
     *
     * @return Length of the VIN string.
     * @since Version 2.6
     */
    inline const INT32 &GetVINLength(void)
    {
        return(m_vinLength);
    };
    /**
     * Get the index for the test result sequence number.
     *
     * @return Indes for the sequence number.
     * @since Version 2.6
     */
    inline const INT32 &GetSequenceStartIndex(void)
    {
        return(m_sequenceStartIndex);
    };
    /**
     * Finds all failed tests and builds the error code node from the 
     * description attributes.
     *
     * @param outFile file to write error codes to 
     * @param testResultNode The test results that were reported during the 
     *                       test.
     */
    virtual void BuildErrorCodeTestsNode(FILE* &outFile, XmlNodeMap *testResultNode);
    /**
     * Return the tag being used for the base errors reported (Error0, Error1).
     *
     * @return Base error tag.
     */
    const string GetErrorTag(void);
    /**
     * Return the tag being used to report errors.
     *
     * @return Error code tag.
     */
    const string GetErrorCodeTestTag(void);

protected:
    /**
     * Load the additional configuration items needed by the test result
     * server.  The "standard" items will be handled by the call to
     * BepServer::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    /**
     * Opens the Results Index File and reads into the m_vehicleTestResults
     * string list
     * 
     * This use to be in the Initialize function, broke out so it can be
     * easily tested
     * 
     * @return bool if operation successful
     * @version AUG-15-2006
     */
    const bool OpenAndReadInResultsFile(void);
    /**
     * This method will check the Result attribute of the test result.
     * If the test failed, it will be added to the AllFailures node for subscribers.
     *
     * @param node   The test result to be analyzed.
     * @since Version 2.0
     */
    void CheckTestResult(const XmlNode *node);
    /**
     * Clear out all results from previous test.
     * Should only be called at the start of a test so the
     * results are available between tests.
     * @since Version 1.0
     */
    void ResetTestResults(void);
    /**
     * Add a new test result file to the list.
     *
     * @param vin    Vehicle test result to add to list.
     * @param readFromFile
     *               true  - extract sequence number from vin.
     *               false - increment sequence number before adding to the list
     * @since Version 1.0
     */
    const bool AddTestResult(string vin, const bool readFromFile = false);
    /**
     * Add a vehicle build item to the Vehicle Build data.
     *
     * @param buildItem Item to be added to the build data.
     * @since Version 2.3
     */
    void AddVehicleBuildItem(const XmlNode *buildItem);
    /**
     * Update the test result archive list.
     * @since Version 1.0
     */
    void UpdateTestResultFile(void);
    /**
     * Report the results of the vehicle test to all subscribers.
     *
     * @return The filename that the data was reported to.
     *         Ex. 1234567891W000000.0001
     * @since Version 1.0
     */
    virtual const string ReportResults(void);
    /**
     * Store a Diagnostic Trouble Code (DTC) to be reported with the test results.
     *
     * @param dtcNode The DTC information to report.
     * @since Version 2.4
     */
    void SetDTC(const XmlNode *dtcNode);
    /**
     * Clear a DTC from the list of reported DTCs.  Also provides the capability of clearing all
     * DTCs for a specific module.
     *
     * @param dtcNode DTC information to clear.
     * @since Version 2.4
     */
    void ClearDTC(const XmlNode *dtcNode);
    /**
     * Check if retest is starting and if so, clear reported DTCs.
     */
    void CheckClearAllDTCs(const string &retestNumber);
    /**
     * Clear all reported DTCs from the list.
     */
    void ClearAllReportedDTCs(void);
    /**
     * Calculate test statistics.
     * Current supported stats are:
     * 
     *   Number of tests performed
     *   Number of tests passed
     *   Percent of tests passed
     *
     * @param statsNode XmlNode to save the calculated statistics in.
     */
    void CalculateTestStats(XmlNode &statsNode);
    /**
     * Adds ICM log file data to the current test result file as a CDATA node under
     * a parent node with a tag of 'nodeTag'
     *
     * @param outFile  Handle to the vehicle test result file to write the speed data to
     * @param nodeTag  Tag to use as the CDATA node's parent. The CDATA node will contain the actual
     *                 ICM data.
     * @param fileName The name of the ICM log file to read the CDATA node's data values from
     */
    void AddIcmDataToResults( FILE* &outFile, const string &nodeTag, const string &fileName);
    /**
     * Get any special info from the node
     *
     * @param tag    Node to get information from.
     * @param info   Type of information to get.
     * @return Information that was part of the specified node.
     * @since Version 2.1
     */
    virtual const string GetSpecialInfo(const string &tag, const string info);
    /**
     * Set the tag being used to signify start of test.
     *
     * @param tag    Start of test tag.
     * @since Version 1.0
     */
    inline void SetStartTestTag(const string &tag)
    {
        m_startTestTag = tag;
    };
    /**
     * Set the tag being used for the VIN.
     *
     * @param tag    Vin tag.
     * @since Version 1.0
     */
    inline void SetVinTag(const string &tag)
    {
        m_vinTag = tag;
    };
    /**
     * Set the vin for the vehicle being tested
     *
     * @param vin    Vin of vehicle being tested.
     * @since Version 1.1
     */
    inline void SetCurrentVin(const string &vin)
    {   // Check to see if a vin was actually entered
        if (vin.empty()) m_currentVIN = noVIN;
        else             m_currentVIN = vin;
    };
    /**
     * Set the tag being used for vehicle build.
     *
     * @param tag    Vehicle build tag.
     * @since Version 1.1
     */
    inline void SetVehicleBuildTag(const string &tag)
    {
        m_vehicleBuildTag = tag;
    };
    /**
     * Set the tag being used for AllFailures.
     *
     * @param tag    AllFailures tag.
     * @since Version 1.1
     */
    inline void SetAllFailuresTag(const string &tag)
    {
        m_allFailuresTag = tag;
    };
    /**
     * Save the tag being used to clear the GUI scroll box.
     *
     * @param tag ClearScrollBox tag.
     * @since Version 2.2
     */
    inline void SetClearScrollBoxTag(const string &tag)
    {
        m_clearScrollBoxTag = tag;
    };
    /**
     * Save the tag being used to subscribe for a key presss
     *
     * @param tag KeyPress tag.
     * @since AUG-15-2006
     */
    inline void SetKeyPressTag(const string &tag)
    {
        m_keyPressTag = tag;
    };
    /**
     * Save the vehicle build record.
     *
     * @param node   Vehicle Build Record.
     * @since Version 1.1
     */
    inline void SetVehicleBuild(const XmlNode *node)
    {
        Log(LOG_FN_ENTRY, "Setting The Vehicle Build\n");
        m_semVehicleBuild.CriticalSectionEnter();   // wait till can update vehicle build
        if (m_vehicleBuild != NULL)
        {   // Clear the old information
            m_vehicleBuild->clear();
            delete m_vehicleBuild;
            m_vehicleBuild = NULL;
        }
        // Save the new information
        m_vehicleBuild = node->Copy();
        m_semVehicleBuild.CriticalSectionExit();
        Log(LOG_FN_ENTRY, "Vehicle Build Set\n");
    };
    /**
     * Set the tag being used for vehicle test status.
     *
     * @param tag    Vehicle test status tag.
     * @since Version 1.0
     */
    inline void SetOverallTestTag(const string &tag)
    {
        m_overallTestTag = tag;
    };
    /**
     * Set the number of tests the server will keep track of.
     *
     * @param numberOfTests
     *               number of test results to store.
     * @since Version 1.0
     */
    inline void SetNumberOfTests(const INT16 &numberOfTests)
    {
        m_numberOfTests = numberOfTests;
    }
    /**
     * Set the name of the driver number tag.
     *
     * @param tag    Driver number tag.
     * @since Version 2.3
     */
    inline void SetDriverNumberTag(const string &tag)
    {
        m_driverNumberTag = tag;
    };
    /**
     * Set the name of the drive curve tag.
     *
     * @param tag    Drive curve tag.
     * @since Version 2.3
     */
    inline void SetDriveCurveTag(const string &tag)
    {
        this->m_driveCurveTag = tag;
    };
    /**
     * Store the value being used for Loss Compensation testing.
     *
     * @param value  Loss compensation value.
     * @since Version 2.3
     */
    inline void SetLossCompensationVin(const string &value)
    {
        m_lossCompensationVin = value;
    };
    /**
     * Store the length of the VIN being used.
     *
     * @param length Length of the VIn string.
     * @since Version 2.6
     */
    inline void SetVINLength(const INT32 &length)
    {
        m_vinLength = length;
        m_sequenceStartIndex = length + 1;
    };

private:
#define TEST_RESULT_SERVER_PULSE_CODE   SYSTEM_PULSE+18
#define FAIL_ALARM_PULSE                10
#define TEST_COMPLETE_SIGNAL_PULSE      11
    /**
     * Cleanup the result directory.
     * Any file not listed in the test result file list will be
     * removed from the directory.
     */
    void CleanupResultDirectory();
	/**
     * Get/Set the flag indicating if the QNX data server shold be connected to.
     * 
     * @param connect Flag indicating if the QNX data server should be connected to.
     * 
     * @return Flag indicating if the QNX data server should be connected to.
     */
    const bool& ConnectToQnxDataServer(const bool *connect = NULL);
    /**
     * Get/Set the flag indicating if the failure alarm is enabled for use.
     * 
     * @param enabled The flag indicating if the failure alarm is enabled for use.
     * 
     * @return The flag indicating if the failure alarm is enabled for use.
     */
    const bool& FailAlarmEnabled(const bool *enabled = NULL);
    /**
     * Setup the timer for turning off the fault alarm.
     * 
     * @param timerSettings
     *               Settings for the fault alarm timer.
     */
    void SetupFaultAlarmTimer(const XmlNode *timerSettings);
    /**
     * Setup the timer for turning off the test complete signal to the PLC.
     * 
     * @param timerSettings
     *               Settings for the test complete signal timer.
     */
    void SetupTestCompleteSignalTimer(const XmlNode *timerSettings);
    /**
     * Get/Set the flag indicating if test complete should be signaled to the PLC.
     * 
     * @param sigTestComplete
     *               Flag indicating if test complete should be signaled to the PLC.
     * 
     * @return Flag indicating if test complete should be signaled to the PLC.
     */
    const bool& ShouldSignalTestComplete(const bool *sigTestComplete = NULL);
    /**
     * Toggle the state of the alarm.
     * The alarm will either be turned on or off based on the value passed into the function.
     * 
     * @param alarmOn State of the alarm:
     *                - True - turn alarm on
     *                - False - turn alarm off
     */
    void ToggleFaultAlarmState(const bool alarmOn);
    /**
     * Toggle the state of the test complete signal.
     * The signal will either be turned on or off based on the value passed into the function.
     * 
     * @param signalTestComplete
     *               State of the test complete signal:
     *               - True - turn test complete signal on
     *               - False - turn test complete signal off
     */
    void ToggleTestCompleteSignal(const bool signalTestComplete);
    /**
     * Set the tag being used as the base for each error reported 
     * (Error0, Error1, etc.).
     *
     * @param tag Base error name.
     */
    inline void SetErrorTag(const string &tag);
    /**
     * Set the tag to be used for the error code node.
     *
     * @param tag Tag to use for the error code node.
     */
    inline void SetErrorCodeTestTag(const string &tag);
    /**
     * Adds results from inter ccrt comm server
     *      
     * @param outFile  Handle to the vehicle test result file to write the speed data to
     */
    void AddInterCcrtCommServerResults( FILE* &outFile);

private:
	/**
     * Process the test results ince we have received the overall test result.
     * 
     * @return Result of processing the test results.
     */
    string ProcessTestResults(void);

    /**
     * Process intermediate test results once we have received Intermediate Overall Result.
     * 
     * @return Result of processing the test results.
     */
    string ProcessIntermediateTestResults(void);

	/**
	 * Get/Set the flag indicating if a pass confirmation file should be generated.
	 * 
	 * @param create Flag indicating if a pass confirmation file should be generated.
	 * 
	 * @return Flag indicating if a pass confirmation file should be generated.
	 */
	const bool& GeneratePassConfirmationFile(const bool *create=NULL);


	/**
	 * Get/Set the flag indicating if inter ccrt comm server results should be imported.
	 * 
	 * @param create Flag indicating inter ccrt comm server results should be imported.
	 * 
	 * @return Flag indicating inter ccrt comm server results should be imported.
	 */
	const bool& ImportInterCcrtResults(const bool *import=NULL);

	/**
	 * Get/Set the directory to use for storing pass confirmation files.
	 * 
	 * @param path   Directory to use for storing pass confirmation files.
	 * 
	 * @return Directory to use for storing pass confirmation files.
	 */
	const string& PassConfirmationFilePath(const string *path=NULL);

    /**
     * Get/Set the flag indicating if the overall result should be included in the file name.
     * 
     * @param addResult Flag indicating if the overall result should be included in the file name.
     * 
     * @return Flag indicating if the overall result should be included in the file name.
     */
    const bool& AddOverallResultToFileName(const bool *addResult = NULL);
    /**
     * Get/Set the number of characters from the overall result string to be added to the file name.
     * 
     * @param length Number of characters from the overall result string to be added to the file name.
     * 
     * @return Number of characters from the overall result string to be added to the file name.
     */
    const INT32& ResultIndicatorLength(const INT32 *length = NULL);
    /**
     * Build the result file name.
     * 
     * @param result Overall test result to use for building the test result file name.
     */
    void BuildResultFileName(const string &result);
    /**
     * Get/Set the current test result file name.
     * 
     * @param fileName The current test result file name.
     * 
     * @return The current test result file name.
     */
    const string& CurrentTestResultFileName(const string *fileName = NULL);

	/** File name to store test results in. */
    string m_testResultFile;
    /** Path to store test results. */
    string m_testResultPath;
    /** Path to store test results. */
    string m_testResultPublshedPath;
	/** Directory to store the pass confirmation file in */
	string m_passConfirmationPath;
    /** Number of tests to keep in archive. */
    INT16 m_numberOfTests;
    /**
     * flag used to indicate that we need to clear the build data when
     * "OverallTest" is published to us.
     * true  => clear the build data
     * false => leave it in place.
     * ---
     * Need to investigate if this function needs to be done somewhere
     * else, like System monitor?
     */
    bool m_clearBuildOnOverall;
    /**
     * Length of the VIN string being used.
     * @since Version 2.6
     */
    INT32 m_vinLength;
    /**
     * Index for the start of the sequence number.
     * @since Version 1.0
     */
    INT16 m_sequenceStartIndex;
    /**
     * List of the vehicles currently in the archive.
     * @since Version 1.0
     */
    list<string> m_vehicleTestResults;
    /**
     * Sequence number for the vehicle tests.
     * @since Version 1.0
     */
    INT16 m_sequenceNumber;
    /**
     * Vin tag.
     * @since Version 1.0
     */
    string m_vinTag;
    /**
     * Vin of the vehicle currently being tested.
     * @since Version 1.0
     */
    string m_currentVIN;
    /**
     * Start of test tag.
     * @since Version 1.0
     */
    string m_startTestTag;
    /**
     * Vehicle Build tag.
     * @since Version 1.1
     */
    string m_vehicleBuildTag;
    /**
     * All Failures tag.
     * @since Version 1.1
     */
    string m_allFailuresTag;
    /**
     * ClearScrollBox tag.
     * @since Version 2.2
     */
    string m_clearScrollBoxTag;
    /**
     * Vehicle test status flag.
     * @since Version 1.0
     */
    string m_overallTestTag;
    /**
     * Drive Curve name tag.
     * @since Version 2.3
     */
    string m_driveCurveTag;
    /**
     * Driver Number tag.
     * @since Version 2.3
     */
    string m_driverNumberTag;
    /**
     * Key Press tag.
     * @since AUG-15-2006
     */
    string m_keyPressTag;
    /**
     * Loss compensation value.
     * @since Version 2.3
     */
    string m_lossCompensationVin;

    int m_retestNumber;

    int m_prevRetestNumber;

    /**
     * Semaphore to wait on when waiting to update the vehicle build.
     * This will be cleared when a new vehicle build can be loaded.
     * @since Version 1.0
     */
    BepSemaphore m_semVehicleBuild;

    /**
     * Vehicle build record for the vehicle being tested.
     * @since Version 1.1
     */
    XmlNode *m_vehicleBuild;
    /**
     * All failures reported for current vehicle test.
     * @since Version 2.0
     */
    XmlNode m_allFailures;
    /**
     * All Diagnostic Trouble Codes (DTCs) reported during the test sequence.
     * @since Version 2.4
     */
    XmlNode m_reportedDTC;
    /**
     * Items to place into the vehicle build section of the
     * reported test results.
     * @since Version 2.7
     */
    XmlNodeMap m_vehiclebuildItems;
    /**
     * Keeps track of test in progress state.
     * @since Version 2.3
     */
    bool m_testInProgress;
    /**
     * Flag used to determine if test results of 'Abort'
     * should be added to the AllFailures node
     * @since 03 Sept 2004
     */
    bool m_includeAbortInAllFailures;
    /**
     * Flag indicating if we should add the brake speed array file to the test
     * result file
     */
    bool m_forceArrayInResults;
    /**
     * Path to the ICM brake force log file
     */
    string m_forceArrayFile;
    /**
     * Flag indicating if we should add the brake speed array file to the test
     * result file
     */
    bool m_speedArrayInResults;
    /**
     * Path to the ICM brake force log file
     */
    string m_speedArrayFile;

    /**
     * Flag indicating if we should update the accept/fault lamps to the PLC
     * at the beginning/end of a test
     */
    bool m_updatePlcLamps;

    /**
     * Flag indicating if we should clear the results after processing the
     * overall test result.  This should only be false for testing to validate
     * the results were correct.
     */ 
    bool m_clearResultsOnOverall;
    /** CondVar for determining if it is time to process the test results. */
    BepCondVar<string, false> m_processTestResults;
    /**
     * Tag being used as the base for each error reported (Error0, Error1, etc.)
     */
    string  m_errorTag;

    /** The tag being used for the error codes node. */
    string  m_errorCodeTag;
    /** Interface to the QNX data server */
    IQnxDataServer m_qnxDataServer;
    /** Flag indicating if we need to connect to the QNX data server */
    bool m_connectToQnxDataServer;
    /** Flag to determine if we need to send a test complete signal to the PLC */
    bool m_signalTestComplete;
    /** Flag to determine if the failure alarm should be enabled */
    bool m_failAlarmEnabled;
    /** Timer to use for turning off the fault alarm */
    BepTimer m_faultAlarmTimer;
    /** Timer to use for signaling test complete to the PLC */
    BepTimer m_testCompleteSignalTimer;
	/** Flag to determine if a pass confirmation file should be generated */
	bool m_createPassConfirmationFile;
    int m_inCycleTestNumber;
    /**
     * Communication object to retrieve static test results from the inter ccrt communication server.
     * 
     * @see IBepCommunication
     * @see InterCcrtCommServer
     */
    IBepCommunication *m_interCcrtServerComm;
    bool m_importInterCcrtCommServerResults;

	/** Flag to determine if the overall result should be indicated in the file name */
	bool m_addOverallResultToFileName;
	/** Number of characters from the result string to add to the filename */
	INT32 m_resultIndicatorLength;
	/** File name to use for storing the current test results */
	string m_currentResultFileName;
         
};
#endif  //TestResultServer_h

