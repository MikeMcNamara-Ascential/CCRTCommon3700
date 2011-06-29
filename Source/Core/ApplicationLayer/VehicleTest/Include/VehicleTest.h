//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/VehicleTest.h 6     10/25/06 4:21p Rwiersem $
//
// FILE DESCRIPTION:
// The Vehicle Test is responsible for execuiting a test of a vehilce.  It
// creates a Selector to choose the appropriate DriveCurve to sequence based
// on the vehicle build and then creates a Sequencer to run the test.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/VehicleTest.h $
// 
// 6     10/25/06 4:21p Rwiersem
// Changes for the 06102701 core release:
// 
// - Made VehicleTestLibraryTest a friend class
// 
// 5     4/13/06 3:30a Cward
// Added PreTestInit
//
// 2     3/20/06 10:49a Bmeinke
// Added PreTestInit() method to allow sub classes to perform tasks just
// prior to sequencing a new test
//
// 4     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     9/01/04 9:29p Cward
// Added option to allow components to be terminated at the end of the
// test sequence
//
// 1     7/21/04 10:19a Cward
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:34p Cward
//
// 3     10/24/03 10:50a Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:20a Khykin
// Initial checkin
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/20/03 9:43a Khykin
// Initial checkin
//
// 1     6/20/03 9:19a Khykin
// Initial checkin.
//
// 1     4/22/03 10:25a Khykin
// Initial Working Checkin.
//
// 2     4/07/03 9:19a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:02:55  tbochene
//
//    PooBag
//
//
// 1     1/27/03 11:10a Khykin
// Initial Checkin
//
// 1     1/18/03 7:18a Khykin
// Initial Checkin
//
// 12    10/21/02 11:10a Khykin
// Added code to handle the reporting and clearing of VehicleTest faults.
//
// 11    8/23/02 8:01a Kh
// Synchronizing the pit machine with VSS.
//
// 9     3/26/02 8:13p Kh
// Adding the incrementing of the vehicle test.
//
// 8     3/25/02 6:17p Kh
// Updating due to changes in the base classes.
//
// 7     3/06/02 1:49p Kh
// Adding the ability to command a base brake test.
//
// 6     1/24/02 1:39p Kh
// Adding a TestInProgress data tag to publish to subscribers.
//
// 5     12/20/01 10:19a Kh
// Updating to be a BepServer.
//
// 4     12/04/01 11:29a Kh
// Updating to be a BepServer.
//
// 3     8/24/01 3:24p Kh
//
// 2     8/17/01 2:36p Kh
// Checking in for weekly backup.
//
// 1     7/06/01 5:35p Kh
// Just checking in for the weekend.  Will debug next week.
//
//*************************************************************************
#ifndef VEHICLETEST_H
#define VEHICLETEST_H
//==============================================================================

#include "BepServer.h"
#include "TestSequencer.h"
#include "TestSelector.h"
#include "BepSemaphore.h"
#include "IFaultServer.h"
#include "IBepCommunication.h"


/**
 * VehicleTest class.  Responsible for managing a vehicle
 * test.
 *
 * @author Kevin Hykin
 * @version Version 1.5
 */
class VehicleTest : public BepServer
{
    friend class VehicleTestLibraryTest;
public:
    /**
     * Constructor for the Vehicle Test Class.
     * @since Version 1.0
     */
    VehicleTest();

    /**
     * Destructor for the Vehicle Test Class.
     * @since Version 1.0
     */
    virtual ~VehicleTest();

    /**
     * Initialize method that takes the vehicle test
     * configuration file and initializes it for operation.
     *
     * @param fileName The configuration file to use for initialization.
     * @since Version 1.4
     */
    virtual void Initialize(const std::string &fileName);

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
     *         BEP_SUCCESS_RESPONSE == Registered
     * @since Version 1.5
     */
    virtual const std::string Register(void);

    /**
     * Initialize method that takes the vehicle test
     * configuration file and initializes it for operation.
     *
     * @param document Root node of the configuration file
     *                 to use for initialization.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Method that runs the vehicle test task until terminated.
     * It will block until started by a command to perform a
     * vehicle or machine test. If the test status is set to
     * Termiante, it will return and the test thread will
     * terminate and the process will shut down.
     * @since Version 1.0
     */
    virtual void Run(volatile bool *terminateFlag=NULL);

    /**
     * Method to process the BepCommuncation Command message.
     *
     * @param node    Action to be performed
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 1.5
     */
    virtual const std::string Command(const XmlNode *node);

    /**
     * Method that will handle all published data.  This adds the
     * functionality of launching the LossCompensation test if
     * the start event is received.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 1.5
     */
    const std::string Publish(const XmlNode *node);

    /**
     * Method called when an Abort command message is
     * received.
     * @since Version 1.0
     */
    virtual void Abort(void);

    /**
     * Method called when a Terminate command message is
     * received.
     * @since Version 1.0
     */
    virtual void Terminate(void);

    /**
     * Method called when a Reset command message is
     * received.
     * @since Version 1.0
     */
    virtual void Reset(void);

    /**
     * Method called when a Reset command message is
     * received.
     * @since Version 1.0
     */
    virtual void ReloadConfig(void);

    /**
     * Get the current vehicle build.  This will copy the build
     * and return it to the requestor and then reset the private
     * build so that the next test can be sequenced.
     *
     * @return The build to use for the commanded vehicle test.
     * @since Version 1.0
     */
    XmlNode *GetVehicleBuild(void);

    /**
     * Method used to set the vehicle build to use for the
     * vehicle test.
     *
     * @param vehicleBuild
     *               The vehicle build to update the local vehicle build
     *               with.
     * @since Version 1.0
     */
    void SetVehicleBuild( const XmlNode *vehicleBuild);

    /**
     * Get the commanded test to perform.  This method will
     * block until there is a new command to perform.  The
     * commands will be Start, Rerun, Calibrate, ...
     *
     * @return The command to perform.
     * @since Version 1.0
     */
    std::string GetCommandedTest(void);

    /**
     * Method used to command a vehicle test.  It will set
     * the test to be performed and then unblock the vehicle
     * test thread to perform the test.  Tests include: Start,
     * Retest, Calibrate, ...
     *
     * @param testType The test type to perform.
     * @return The status of the commanded test.
     * @since Version 1.0
     */
    virtual std::string CommandTest(const std::string testType);
    /**
     * Get the name of the test log index file.
     *
     * @return Filename of the test log index file.
     */
    virtual const string& GetIndexFileName(void);
    /**
     * Get the maximum log file index number to use.
     *
     * @return Maximum log file index number.
     */
    virtual const INT32& GetMaxIndexNumber(void);

protected:
    /**
     * Determines the suffix to add to the test log file.
     * @return The suffix to add to the test log file
     */
    const std::string GetLogSuffix();
    /**
     * Set a new currently active VehicleTest fault.
     *
     * @param fault  The fault to set.
     */
    void SetFault(std::string &fault);

    /**
     * Clear the currently active VehicleTest fault.
     */
    void ClearFault(void);

    /**
     * Get the currently active VehicleTest fault.
     *
     * @return The fault that is currently active.
     */
    const std::string GetFault(void);

    /**
     * Reads the latest copy of vehicle build from the system and updates our
     * vehicle build data member
     *
     * @return The status of the operation.
     */
    virtual INT32 ReadNewVehicleBuild();

    /**
     * Check the flag that determines if test components
     * are terminated at end of test sequence.
     *
     * @return  True - test components will be terminated
     *          when test sequence is complete.
     * @since July 14, 2004
     */
    const bool& TerminateComponentsOnTestComplete(void);
    /**
     * Initialize the log file index number.
     *
     * @param document  Configuration data that contains the name of the index file.
     */
    virtual void InitializeLogFileIndex(const XmlNode *document);
    /**
     * Increment the log file index.  The current log number will be read from file if
     * the initializing parameter is set to true.
     *
     * @param initializing  If set to true, read the index number from file.
     */
    virtual void IncrementLogFileIndex(bool initializing = false);
    /**
     * Store the file name of the test log index file.
     *
     * @param name  Store the name of the test log index file.
     */
    virtual void SetIndexFileName(const string &name);
    /**
     * Store the maximum index number to use for naming test log files.
     *
     * @param maxNumber  Maximum test log index number.
     */
    virtual void SetMaxIndexNumber(const INT32 &maxNumber);

    /**
     * Method called just prior to starting a new test to allow sub-classes to
     * install "hooks" when a test is about to begin.
     *
     * @param test       Type of test being performed (i.e. TEST_RETEST, etc)
     * @param driveCurve Drive cureve to be executed
     */
    virtual void PreTestInit(const std::string &test, const std::string &driveCurve);

    /**
     * The test sequencer that is used to sequence drive
     * curves.
     * @since Version 1.0
     */
    TestSequencer m_testSequencer;

    /**
     * The test selector that is used to read vehicle build
     * information and determine which drive curve should
     * be used.
     * @since Version 1.0
     */
    TestSelector m_testSelector;

    /**
     * Semaphore to wait on when waiting to command a vehicle test.
     * It will be cleared when a new vehicle test can be started.
     * @since Version 1.0
     */
    BepSemaphore m_semCommandTest;

    /**
     * Semaphore to wait on when waiting to update the vehicle build.
     * This will be cleared when a new vehicle build can be loaded.
     * @since Version 1.0
     */
    BepSemaphore m_semVehicleBuild;

    /**
     * Interface to the fault server for reporting test faults.
     * @since Version 1.5
     */
    IFaultServer *m_faultServer;

    /**
     * The current test number.
     * @since Version 1.3
     */
    INT32 m_testNumber;
    /**
     * The maximum test log index to allow.
     */
    INT32 m_maxTestNumber;
    /**
     * Drive curve parser for executing a commanded drive curve.
     * @since Version 1.0
     */
    XmlParser m_driveCurveParser;

    /**
     * Communication component used by the test sequencers to
     * receive test objective results.
     * @since Version 1.0
     */
    IBepCommunication *m_sequencerComm;
    INamedDataBroker m_ndb;
    /**
     * The type of test that is currently being run.
     * @since Version 1.0
     */
    std::string m_testType;

private:
    /**
     * Store the flag that determines if test components
     * are terminated at end of test sequence.
     *
     * @param terminateTestComplete  True - test components will be terminated
     *                               when test sequence is complete.
     * @since July 14, 2004
     */
    void TerminateComponentsOnTestComplete(const bool& terminateTestComplete);
    /**
     * Local parser used to retain configuration file information
     * for use by the tester. This was added because someone removed
     * if from the base class.
     * @since Version 1.4
     */
    XmlParser m_parser;
    /**
     * Reference to the current vehicle build.
     * @since Version 1.0
     */
    XmlNode *m_vehicleBuild;
    /**
     * The cable connected during a test flag.
     * @since Version 1.4
     */
    bool m_cableConnect;
    /**
     * The currently active fault (if one exists) that was
     * a result of the VehicleTest.
     * @since Version 1.3
     */
    std::string m_fault;
    /**
     * Flag to determine if components should be terminated at end of test.
     * @since July 14, 2004
     */
    bool m_terminateComponentsOnTestComplete;
    /**
     * Data file that contains the current test log number to use.
     */
    string m_dataFile;

    /**
     * Determines if the vin should be used as the test log suffix.
     */
    bool m_useVinAsTestLogSuffix;

    /**
     * Determines the length of the end of a vin to be used in a log suffix.
     */
    INT32 m_vinSuffixLength;

    /**
     * Determines the file extension used in a log suffix.
     */
    string m_suffixExtension;

    /**
     * Flag to protect commanded test from being sent twice.
     */
    bool m_testCommandInProgress;
};
#endif //VEHICLETEST_H

