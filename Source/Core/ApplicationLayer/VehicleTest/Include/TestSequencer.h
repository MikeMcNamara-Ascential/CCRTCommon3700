//*************************************************************************
// Description:
//  The Sequencer is responsible for sequencing a test.  It will launch
//  a set of ComponentManagers and then use them to command test objectives
//  in the system.  At the end of each Objective the sequencer will evaluate the
//  oveall Pass or Failure of the test and either sequence the phase again or
//  continue.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/TestSequencer.h $
// 
// 7     10/30/07 3:27p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 6     10/25/06 4:20p Rwiersem
// Changes for the 06102701 core release:
// 
// - Added m_reuseComponents.  This is used to keep the test components
// running when a new drive curve starts.
// 
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 4     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 3     9/01/04 9:28p Cward
// Added configurable delay between commanding test objectives in a test
// phase
//
// 2     6/09/04 11:28p Cward
// Refactored App  layer includes to remove implementation Updated header
// comment block to the new standard.
//
// 1     12/21/03 6:34p Cward
//
// 4     10/24/03 11:07a Khykin
// Removed Flatrock specific code.
//
// 1     10/07/03 11:21a Khykin
// Initial checkin
//
// 3     8/12/03 2:46p Khykin
// Updated with new directory update changes.
//
// 2     8/06/03 10:43a Khykin
// Updated with changes from Flatrock.
//
// 1     6/20/03 9:44a Khykin
// Initial checkin
//
// 1     6/20/03 9:19a Khykin
// Initial checkin.
//
// 3     5/28/03 5:29p Khykin
// Adding parameter for retest timeout.
//
// 2     4/30/03 7:44a Khykin
// Added a status check to determine if the machine conditions are correct
// to be able to perform a retest.
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
// 11    11/11/02 2:58p Khykin
// Updated fault handling.
//
// 10    10/31/02 1:46p Khykin
// Added the background objective status member variable.
//
// 9     10/21/02 11:09a Khykin
// Adding the ability to turn retesting on/off based on the type of test
// that is
// being sequenced by the tester.
//
// 8     8/23/02 8:01a Kh
// Synchronizing the pit machine with VSS.
//
// 6     3/25/02 6:17p Kh
// Updating due to changes in the base classes.
//
// 5     3/06/02 1:50p Kh
// Updating comments.
//
// 4     11/20/01 10:46a Kh
// Updating sequencer for new tests.
//
// 3     8/28/01 11:20a Kh
// Updating for NTO 6.1
//
// 2     8/17/01 2:35p Kh
// Checking in for weekly backup.
//
// 1     7/06/01 5:35p Kh
// Just checking in for the weekend.  Will debug next week.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef TESTSEQUENCER_H
#define TESTSEQUENCER_H
//==============================================================================
using namespace std;

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <errno.h>
#include <fstream>

//#include "GeneralUtilities.h" // general system utilities
#include "Debuggable.h"
#include "BepException.h"       // base exception definition
#include "BepDefs.h"
#include "Bepos.h"
#include "XmlParser.h"
#include "XmlNode.h"
#include "ILogger.h"
#include "IBepCommunication.h"
#include "BepComponent.h"
#include "BackgroundComponentManager.h"
#include "ComponentManager.h"
#include "INamedDataBroker.h"
#include "IPromptServer.h"

/**
 * The TestSequencer is responsible for setting up and
 * performing vehicle tests.  It will receive a drive
 * curve, launch the necessary components, and then
 * allow the use to sequence a test.
 *
 * @author Kevin Hykin
 * @version 1.4
 * @since 26 July 2003
 */
class TestSequencer : public BepComponent
{
public:
    /**
     * Constructor for the Test Sequencer.
     * @since Version 1.0
     */
    TestSequencer();

    /**
     * Destructor for the Test Sequencer.
     * @since Version 1.0
     */
    virtual ~TestSequencer();

    /**
     * Initialize the sequencer for running.
     *
     * @param config The configuration for initialization.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *config);

    /**
     * Load the specified drive curve and set up the sequencer
     * to execute it.
     *
     * @param driveCurve The drive curve that specifies the components for
     *                   the test and the test objectives that should be used.
     * @since Version 1.0
     */
    void LoadDriveCurve(const XmlNode *driveCurve, string driveCurveRootDir);

    /**
     * Start the sequencing of the vehicle test.  This
     * is called with the type of testing that should be
     * performed.
     *
     * @param type   The type of test to sequence. Start, Continue,
     *               Retest, ...
     * @return Return the overall status of the test.
     *         Pass, Fail, Abort, ...
     * @since Version 1.0
     */
    const std::string SequenceTest(const std::string type);

    /**
     * Remove all the test components from the previous vehicle test.
     * @since Version 1.4
     */
    void RemoveTestComponents(void);

    /**
     * Reloads the configuration file and resets the
     * base components objects.
     * @since Version 1.0
     */
    virtual void ReloadConfiguration(void);

    /**
     * Method used to abort the current processing based
     * on the needs of the component.
     * @since Version 1.0
     */
    virtual void Abort(void);

    /**
     * Method used to set all of the objects in the component
     * to their original states.  This would includes all
     * member variables such as buffers, lists, and stored
     * data.
     * @since Version 1.0
     */
    virtual void Reset(void);

    /**
     * Method used to terminate the component.  This performs
     * any cleanup that the destructor would not and sets
     * the state of the component to terminate.
     * @since Version 1.0
     */
    virtual void Terminate(void);

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
     *         BEP_SUCCESS_RESPONSE == Registered
     * @since Version 1.2
     */
    virtual const std::string Register(void);

    /**
     * Request to read a data tag
     *
     * @param tag    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     * @since Version 2.0
     */
    virtual const std::string Read(const std::string &tag, const INT32 rate = 0);
    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     * @since Version 2.0
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
    /**
     * Request to write a data tag
     *
     * @param tag    Tag to be read
     * @param value  Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Write(const std::string &tag, const std::string &value);
    /**
     * Request to write a data tag
     *
     * @param node  Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Write(const XmlNode *node);
    /**
     * Request to Perform an action.
     *
     * @param tag   Action to be performed
     * @param value The value of the command.
     * 
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     */
    virtual const std::string Command(const std::string &tag, const std::string &value);
    /**
     * Request to Perform an action
     *
     * @param node    Action to be performed
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Command(const XmlNode *node);
    /**
     * Method to display the test results.
     * @since Version 2.0
     */
    void DisplayResults(void);

    /**
     * Method that is used to indicate that a retest should/can
     * be performed.
     *
     * @param status The status of retesting.
     */
    void PerformRetest(bool status);

    /**
     * Method that is used to determine why the sequenced test
     * failed.
     *
     * @return The description of why the test failed.
     */
    const std::string GetFailureString(void);

    /**
     * Set the sequencer communication object.
     *
     * @param sequencerComm The sequencer communication object to use.
     */
    void SetSequencerCommObj(IBepCommunication *sequencerComm);

protected:

    /**
     * Sequences the test phase and returns the overall result
     * of the sequencing.
     *
     * @param incycleRetest Incycle retest flag.  This indicates if the background
     *                      objectives should be sequenced or not.
     * @return The number of foreground objectives sequenced.
     * @since Version 1.0
     */
    const INT32 SequencePhase(bool incycleRetest);

    /**
     * Process the results of the sequenced test objectives.
     * This method will wait until all of the objectives
     * in this test phase have reported their status.
     *
     * @return The status of the test objectives.
     * @since Version 1.0
     */
    const std::string ProcessResults(void);

    /**
     * Process the outstanding background results.
     * This method will wait until all of the objectives
     * that have been sequenced in the background have completed.
     *
     * @return The status of the test objectives.
     * @since Version 1.0
     */
    const std::string ProcessBackgroundResults(void);

    /**
     * Update Test Status will read the received message
     * and populate the correct foreground or background
     * objective.  It will validate the message to ensure that
     * it is the correct objective from the correct
     * component.
     *
     * @param commHandler
     *               The IBepCommunication handler that was used to
     *               process the incomming message.
     * @return Return the status of the result that was updated.
     * @since Version 1.0
     */
    const std::string UpdateTestStatus(IBepCommunication *commHandler);

    /**
     * Update The Foreground Objective Status will read the received
     * message and populate the correct foreground objective.
     * It will validate the message to ensure that it is the
     * correct objective from the correct component.
     *
     * @param commHandler
     *               The IBepCommunication handler that was used to
     *               process the incomming message.
     * @return Return the status of the result that was updated.
     * @since Version 1.0
     */
    const std::string UpdateForegroundObjectiveStatus(IBepCommunication *commHandler);

    /**
     * Update The Background Objective Status will read the received
     * message and populate the correct background objective.
     * It will validate the message to ensure that it is the
     * correct objective from the correct component.
     *
     * @param commHandler
     *               The IBepCommunication handler that was used to
     *               process the incomming message.
     * @return Return the status of the result that was updated.
     * @since Version 1.0
     */
    const std::string UpdateBackgroundObjectiveStatus(IBepCommunication *commHandler);

    /**
     * Return the status of the current test phase foreground
     * objective results.  This will continue to return "Testing"
     * until all of the foreground objectives have been
     * updated.
     *
     * @return The status of the test phase.
     *         Pass, Fail, Abort, or Testing
     * @since Version 1.0
     */
    const std::string TestPhaseStatus(void);

    /**
     * Method to process the result and update the current status.
     *
     * @param result The result that is to be evaluated to determine if the
     *               current status should be updated.
     * @param overallResult
     *               The current status of thethat the system is keeping.
     * @since Version 1.0
     */
    void UpdateCurrentStatus(const std::string &result, std::string &overallResult);

    /**
     * Reset all of the objectives "Results" in the phase
     * to "Sequenced" so the sequencer will know when a test has
     * been completed.
     *
     * @param incycleRetest Incycle retest flag.  This indicates if the background
     *                      objectives should be reset or not.
     * @return The number of objectives that were reset.
     * @since Version 1.0
     */
    INT32 ResetPhase(bool incycleRetest);

    /**
     * Prompt the driver to indicate if an incycle
     * retest should be performed.
     *
     * @return The results of the request.
     *         "Yes" or "No"
     */
    const std::string PromptRetest(void);

    /**
     * Determine if the vehicle test can continue.
     *
     * @param testStatus The status of the vehicle test.
     * @return The status of the ability to test.
     *         true - continue testing
     *         false - stop testing
     */
    bool CanTest(const std::string &testStatus);

    /**
     * Determine if the vehicle retest can continue.
     *
     * @param testStatus The status of the vehicle test.
     * @return The status of the ability to test.
     *         true - continue testing
     *         false - stop testing
     */
    bool CanRetest(const std::string &testStatus);
    /**
     * Check the status of the machien to determine if everything
     * is ok to test.
     *
     * @return The status of the check.
     */
    INT32 StatusCheck(void);
    /**
     * Determine if an Incycle retest should be performed.
     *
     * @param currentTestStatus The overall status of the TestPhase.
     * @param retestNumber      The number of retests that have been performed in this
     *                          TestPhase.
     * @return The status of performing a retest.
     *         true - perform retest
     *         false - do not perform retest
     */
    bool PerformIncycleRetest(const std::string &currentTestStatus, const int retestNumber);

    /**
     * Update the test status on the GUI.  This includes the
     * overall prmopt and background color of the prompt.
     *
     * @param testStatus The status to use for displaying the overall test status.
     * @version 1.2
     */
    void DisplayOverallTestPrompt(const std::string testStatus);

    /**
     * Determine if the current test is complete.  This includes
     * prompting the driver for a retest if the configuration
     * and status provide for it.
     * @version 1.2
     * @param testStatus   The status of the test.
     * @param retestNumber The number of retests that have been performed.
     * @return Flag to indicate if the test is complete.
     *         true = complete
     *         false = not complete
     */
    bool TestComplete(const std::string &testStatus, const int retestNumber);

    /**
     * Method that is used to determine why the sequenced test
     * failed.
     *
     * @return The description of why the test failed.
     */
    void SetFailureString(const std::string reason);

    /**
     * Get the amount of time to delay between commanding objectives.
     *
     * @return Time to delay between commanding objectives.
     * @since July 09, 2004
     */
    const INT32& GetObjectiveCommandDelay(void);

    /**
     * The reference to the loaded drive curve.
     * @since Version 1.0
     */
    const XmlNode *m_driveCurve;

    /**
     * Communication component used by the sequencer to
     * receive test objective results.
     * @since Version 1.0
     */
    IBepCommunication *m_sequencerComm;

    /**
     * Component manager that will manage the sending of
     * objectives to components.
     * @since Version 1.0
     */
    ComponentManager m_compManager;

    /**
     * Component manager that will manage the sending of
     * objectives to components in the background.
     * @since Version 1.0
     */
    BackgroundComponentManager m_backCompManager;

    /**
     * The type of retesting to do.
     * Incycle, ReRun, None, ...
     * @since Version 1.0
     */
    std::string m_retestType;

    bool m_setIncycleFlag;

    bool m_retestCountExceededAbort;

    /**
     * The maximum number of retests to perform.
     * @since Version 1.2
     */
    int m_maxRetests;

    /**
     * The current phase that is being run or has run.
     * This is used to walk through the drive curve.
     * @since Version 1.0
     */
    int m_currentPhaseNumber;

    /**
     * reference to the current phase that is being sequenced
     * @since Version 1.1
     */
    const XmlNode *m_currentPhase;

    /**
     * Named data broker interface which provides the ability
     * for the component to interact with the real time
     * system. (Read, Write, Command, Subscribe, Unsubscribe)
     * @since Version 1.0
     */
    INamedDataBroker *m_ndb;

    /**
     * PromptServer interface for prompting the user.
     * @since Version 1.2
     */
    IPromptServer *m_prompt;

    /**
     * Flag to indicate that a LossCompensation test is being
     * performed.
     * @since Version 1.3
     */
    bool m_retestPossible;

    /**
     * The status of the background objectives.
     * @since Version 1.4
     */
    std::string m_backgroundObjectiveStatus;
    /**
     * This is the reason that the sequencer failed to be able
     * to sequence the test.
     * @since Version 1.3
     */
    std::string m_failString;

    /**
     * The timeout time in miliseconds to wait for a driver to select to retest a
     * vehicle.
     * @since Version 1.4
     */
    INT32 m_retestTimeout;

    /**
     * The test result that the overall test result prompt will be placed in.
     * @since Version 1.4
     */
    INT32 m_overallTestResultPromptBox;

    /**
     * The prompt box that the loading parameters prompt will be placed in.
     * @since 03 Sept 2004
     */
    INT32 m_loadingParametersPromptBox;
    /**
     * The prompt box that the retest Yes or No prompt will be placed in.
     * @since 03 Sept 2004
     */
    INT32 m_retestYesOrNoPromptBox;

    /**
     * Flag indicating if we want to re-use existing test components instead of
     * always killing/relaunching them
     */
    bool m_reuseComponents;
    
    /**
     *  Debug function to return memory usage
     */
    int ReturnMemArena();

private:
   /**
     * Store the time to delay between sending objectives.
     *
     * @param delayTime Delay time between objectives.
     * @since July 09, 2004
     */
    void SetInterObjectiveDelay(const INT32& delayTime);
    /**
     * Get/Set the flag indicating if PLC result buttons are in use.
     * 
     * @param plcButtons Flag indicating if PLC result buttons are in use.
     * 
     * @return Flag indicating if PLC result buttons are in use.
     */
    const bool& UsePlcResultButtons(const bool *plcButtons = NULL);
    /**
     * Set the system tag to read for the pass button.
     * 
     * @param passButtonTag
     *               System tag to read for the pass button.
     * 
     * @return System tag to read for the pass button.
     */
    const string& PlcPassButtonTag(const string *passButtonTag = NULL);
    /**
     * Set the system tag to read for the fail button.
     * 
     * @param failButtonTag
     *               System tag to read for the fail button.
     * 
     * @return System tag to read for the fail button.
     */
    const string& PlcFailButtonTag(const string *failButtonTag = NULL);
    /**
     * Amount of time to delay between commanding of objectives.
     * @since July 09, 2004
     */
    INT32 m_interObjectiveDelay;
    /** Flag indicating if PLC result buttons are being used */
    bool m_usePlcResultButtons;
    /** Tags to use for the PLC result buttons */
    string m_plcPassButtonTag;
    string m_plcFailButtonTag;
	/** Current overall test status */
	string m_currentOverallTestStatus;
};
#endif //TESTSEQUENCER_H

