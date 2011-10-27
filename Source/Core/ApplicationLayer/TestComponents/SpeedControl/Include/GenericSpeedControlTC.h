//******************************************************************************
// Description:
//  Base Cruise testing for all types of vehicles.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/SpeedControl/Include/GenericSpeedControlTC.h $
// 
// 1     11/02/07 8:52a Derickso
// Updated comment content.
// 
// 12    10/30/07 1:59p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed and updated Javadoc comments.
// 
// 1     10/17/07 8:10a Derickso
// Updated header comment content.
// 
// 11    10/25/06 4:05p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added WarmInitialize().  This method is used to keep the test
// component running when a new drive curve starts.
// 
// 10    2/15/06 7:24p Cward
// Added Tap Up Tap Down steps, ability to test speed control without
// module comms.
//
// 9     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 8     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 7     9/01/04 9:19p Cward
// Removed inlines from prototypes
//
// 6     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 5     5/25/04 3:56p Cward
// Added functionality to allow the operator to amnually fail any portion
// of thespeed control test.
//
// 4     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 3     1/27/04 4:49p Cward
// Changes resulting from DE's testing and RW's trip
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 2     11/13/03 6:13p Derickso
// Latest updates ready for testing.
//
// 1     10/27/03 5:35p Derickso
// Corrected compiler errors.
// Compiles, not tested.
//
// 1     5/27/03 3:30p Derickso
// Periodic backup.
//
// 1     4/25/03 11:11a Derickso
// Periodic backup.
//
// 3     2/10/03 11:44a Derickso
// Added diagnostic log statements.
// Moved the removal of the Set prompt to after the set button is detected
// in order to give drivers an indication that the set button was
// detected.
//
// 2     1/30/03 8:31a Derickso
// Filled out common methods.
// Periodic checkin.
//
// 1     12/18/02 2:40p Derickso
// Initial check-in before vacation.  Not complete.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericSpeedControlTC_h
#define GenericSpeedControlTC_h
//==============================================================================

#include "GenericTCTemplate.cpp"
#include "DataAnalysis.h"

/**
 * Status of each test step.  Can be Pass, Fail, Abort, Skip, etc.
 */
typedef struct
{
    string verifyOnSwitch;        /* Verify functionality of the ON switch */
	string verifyOffSwitch;       /* Verify functionality of the OFF switch */
    string verifyCancelSwitch;    /* Verify functionality of the CANCEL switch */
    string verifySetEnable;       /* Verify functionality of the SET switch */
    string verifyResumeEnable;    /* Verify functionality of the RESUME switch */
    string verifyBrakeCutOut;     /* Verify functionality of the BRAKE switch */
    string verifyClutchCutOut;    /* Verify functionality of the CLUTCH switch */
    string checkForFaults;        /* Check the module for faults */
} SpeedControlTestStatus;

/**
 * Representation of switch state.
 */
const string switchOn("On");
const string switchOff("Off");
const string switchSet("Set");
const string switchCancel("Cancel");
const string switchResume("Resume");
const string switchBrake("Brake");
const string switchClutch("Clutch");


/**
 * Generic Cruise test component.  This class implements all the cruise objectives that
 * are common across all platforms.  
 *
 * @author Dan Erickson
 * @see GenericTCTemplate<VehicleModuleType>
 * @see DataAnalysis
 */
template <class VehicleModuleType>
class GenericSpeedControlTC : public GenericTCTemplate<VehicleModuleType>
{
    friend class CommonSpeedControlTest;

public:
    /** Class Constructor. */
    GenericSpeedControlTC();

    /** Class Destructor. */
    virtual ~GenericSpeedControlTC();

    /**
     * Reinitialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reinitialize using the already loaded configuration file. This is intended to be used when a component is 
     * activated and there have been no modifications to any configuration files used by the component.
     * Member variables that store status during the test will also be reset to initial values.  The build data will
     * also be analyzed to determine if a valid speed control type is installed in the vehicle.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedControl - System tag to retrieve the speed control data from the vehicle build data. </li>
     *      <li> NoSpeedControl - Value that indicates the vehicle is not equipped with speed control. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckForValidSpeedControlType() </li>
     *      <li> SetCruiseEquipped() </li>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      <li> GenericTCTemplate::WarmInitialize() </li>
     *      </ul>
     */
    virtual void WarmInitialize(void);

    /**
     * Execute the specified test step.
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     *
     * <b>Test Step Functions Called:</b>
     *      <ul>
     *      <li> CheckOnEnable()</li>
     *      <li> CheckOffDisable()</li>
     *      <li> CheckBrakeCutOut()</li>
     *      <li> CheckCancelCutOut()</li>
     *      <li> CheckClutchCutOut()</li>
     *      <li> CheckResumeEngagement()</li>
     *      <li> CheckSetEngagement()</li>
     *      <li> DeterminePreviousPass()</li>
     *      <li> UpdateModuleResult()</li>
     *      <li> CheckTapSpeedChange() </li>
     *      <li> CheckSpeedLimiter() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsCruiseEquipped()</li>
     *      <li> GetTestStepName()</li>
     *      <li> GenericTCTemplate::CommandTestStep()</li>
     *      </ul>
     *
     * @param value  This is the value that was commanded by the test sequencer.
     *
     * @return The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

    /**
     * Get the target speed.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Get the speed at which the speed control system was set.
     * <br><p>
     *
     * @return Speed control system target speed.
     */
    const float &GetSpeedControlTargetSpeed(void);

    /**
     * Determine if the vehicle is equipped with Electronic Throttle Control.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * An internal flag is checked to determine if the vehicle is equipped
     * with Electronic Throttle Control. (ETC).
     * <br><p>
     *
     * @return Flag indicating if the vehicle is equipped with ETC
     *      <ul>
     *      <li> True - Vehicle is equipped with ETC. </li>
     *      <li> False - Vehicle is <b><i>not</i></b> equipped with ETC. </li>
     *      </ul>
     */
    const bool &IsETCVehicle(void);

    /**
     * Check for a valid sped control system.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The vehicle build data will be analyzed to determine if the vehicle is equipped with a valid speed control system.
     * The data from the vehicle build will be compared to the list of valid speed control types found in the config
     * file during initialization.  The data will be stored to determine if any further speed control testing should occur.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> BroadcastSpeedControlType - System tag to use for retrieving the speed control option from the build data. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetCruiseEquipped() </li>
     *      </ul>
     */
    void CheckForValidSpeedControlType(void);

    /**
     * Abort current test sequence.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Method used to abort the current processing based on the needs of the component.
     * If the vehicle is not equipped with Speed Control, the abort flag will not be set.
     * <br><p>
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> IsCruiseEquipped() </li>
     *      <li> GenericTC::Abort() </li>
     *      </ul>
     */
    void Abort(void);

protected:
    /**
     * Perform GenericSpeedControl test component intitialization.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Generic speed control specific initialization will be performed.  A list of valid speed control types will be
     * stored.  The individual test step results will also be set to a status of testing.  The vehicle build data will
     * be analyzed to determine if the vehicle is equipped with a valid speed control system.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ValidSpeedControlTypes - List of valid speed control types. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedControl - System tag for retrieving the speed control option from the vehicle build data. </li>
     *      <li> NoSpeedControl - Value indicating the vehicle is not equipped with speed control. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> CheckForValidSpeedControlType() </li>
     *      <li> SetCruiseEquipped() </li>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      <li> GenericTCTemplate::InitializeHook() </li>
     *      </ul>
     *
     * @param config The configuration data to use for initializing the test component.
     */
    virtual void InitializeHook(const XmlNode *config);
    /**
     * Register the component with the system.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Method that is called to command the component to perform any
     * system registration functions necessary.  The component status will only be set to <i>"Test In Progress"</i> if
     * the vehicle is equipped with a valid speed control system.
     * <p>
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> IsCruiseEquipped() </li>
     *      <li> UpdateTestStatus() </li>
     *      <li> BepServer::Register() </li>
     *      </ul>
     *
     * @return Status of the registration process.
     *      <ul>
     *      <li> BEP_FAILURE_RESPONSE == Could not successfully perform registration </li>
     *      <li> BEP_SUCCESS_RESPONSE == Registered </li>
     *      </ul>
     */
    virtual const string Register(void);
    /**
     * Reset the overall result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reset the overall result.  If the vehicle is not equipped with speed control, the overall result
     * will not be updated.
     * <p>
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> IsCruiseEquipped() </li>
     *      <li> GenericTC::ResetOverallResult() </li>
     *      </ul>
     */
    void ResetOverallResult(void);

    /**
     * Verify that the ON switch is functioning properly.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Wait for driver to turn speed control on.  If the speed control does not turn on, the operator can press the
     * fail button instead of waiting for the test to time out.
     * <br><p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b><br>
     *      <ul>
     *      <li> SpeedControlOnTimeout - Speed Control not on in the allowed time. </li>
     *      <li> SystemStatus - System status prevents further testing. </li>
     *      <li> CommunicationFailure - Communication failure verifying on switch. </li>
     *      <li> OperatorFailOn - Operator pressed Fail button during test. </li>
     *      <li> CruiseOnNotSeen - Did not detect cruise in the On state. </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> SetStartTime() </li>
     *      <li> SetupForOperatorInput() </li>
     *      <li> WaitForSpeedControlOn() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of checking the speed control on functionality.
     *      <ul>
     *      <li> Pass - Speed Control on was verified. </li>
     *      <li> Fail - Speed Control on was not verified. </li>
     *      <li> Timeout - Speed Control not verified in the time allotted. </li>
     *      </ul>
     */
    virtual const string CheckOnEnable(void);

    /**
     * Verify that the OFF switch is functioning properly.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Wait for driver to turn speed control off.  If the speed control does not turn off, the operator can press the
     * fail button instead of waiting for the test to time out.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b><br>
     *      <ul>
     *      <li> SpeedControlOffTimeout - Speed Control not off in the allowed time. </li>
     *      <li> SystemStatus - System status prevents further testing. </li>
     *      <li> CommunicationFailure - Communication failure verifying off switch. </li>
     *      <li> OperatorFailOff - Operator pressed Fail button during test. </li>
     *      <li> CruiseOffNotSeen - Failed to detect speed control in the Off mode. </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> SetStartTime() </li>
     *      <li> SetupForOperatorInput() </li>
     *      <li> WaitForSpeedControlOff() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of checking the speed control off functionality.
     *      <ul>
     *      <li> Pass - Speed Control off was verified. </li>
     *      <li> Fail - Speed Control off was not verified. </li>
     *      <li> Timeout - Speed Control not verified in the time allotted. </li>
     *      </ul>
     */
    virtual const string CheckOffDisable(void);

    /**
     * Verify that the cruise SET feature is functioning properly.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Wait for driver to set speed control and check for stable speed.  If the speed control does not set, the operator
     * can press the fail button instead of waiting for the test to time out.  Once the vehicle speed is stable after
     * the operator presses the Set button, the vehicle speed will be stored as the target speed.
     * <br><p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> SkipOnTestResult - Flag indicating if the "On" test step result should be ignored.  Otherwise, the "On"
     *                              test step must have passed in order to perform the "Set" test step. </li>
     *      <li> SpeedControlMinSpeed - Value indicating speed control minimum speed. </li>
     *      <li> SpeedControlMaxSpeed - Value indicating speed control maximum speed. </li>
     *      <li> SpeedRangeCheckDelay - Delay between speed checks. </li>
     *      </ul>
     *
     * <b>Prompts:</b><br>
     *      <ul>
     *      <li> SetPrompt - Set speed control. </li>
     *      <li> RemoveFootPrompt - Remove foot from accelerator pedal. </li>
     *      <li> CheckStableSpeed - Checking for stable vehicle speed. </li>
     *      </ul>
     *
     * <b>Fault Tags:</b><br>
     *      <ul>
     *      <li> ConditionsNotCorrect - Speed control ON mode not seen prior to SET test. </li>
     *      <li> SpeedControlSetTimeout - Speed Control not set in the allowed time. </li>
     *      <li> SystemStatus - System status prevents further testing. </li>
     *      <li> CommunicationFailure - Communication failure checking for speed control set. </li>
     *      <li> OperatorFailSet - Operator pressed Fail button during test. </li>
     *      <li> VehicleSpeedNotStable - Vehicle speed did not stabilize after pressing the Set button. </li>
     *      <li> ThrottleApplied - Operator applied throttle after pressing the Set button. </li>
     *      <li> SpeedNotInRange - Timeout waiting for vehicle speed to be in range. </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> WaitAndPromptForSpeed() </li>
     *      <li> SetupForOperatorInput() </li>
     *      <li> WaitForSetButton() </li>
     *      <li> IsVehicleSpeedStable() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SetStartTime() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> StatusCheck() </li>
     *      <li> SetSpeedControlTargetSpeed() </li>
     *      <li> GetSpeedControlTargetSpeed() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of checking the speed control Set functionality.
     *      <ul>
     *      <li> Pass - Speed Control set was verified. </li>
     *      <li> Fail - Speed Control set was not verified. </li>
     *      <li> Timeout - Speed Control set not verified in the time allotted. </li>
     *      </ul>
     */
    virtual const string CheckSetEngagement(void);

    /**
     * Verify that the cruise RESUME feature is functioning properly.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Wait for driver to resume speed control and check for speed increasing.  If the speed control does not resume,
     * the operator can press the fail button instead of waiting for the test to time out.  In order to determine if the
     * resume feature is functioning properly, vehicle speed will be monitored to ensure it is increasing or has 
     * returned to the target speed stored when the Set button was pressed.
     * <br><p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b><br>
     *      <ul>
     *      <li> ConditionsNotCorrect - Speed control ON and Set modes not seen prior to Resume test. </li>
     *      <li> SpeedControlResumeTimeout - Speed Control not Resumed in the allowed time. </li>
     *      <li> SystemStatus - System status prevents further testing. </li>
     *      <li> CommunicationFailure - Communication failure checking for speed control resume. </li>
     *      <li> OperatorFailResume - Operator pressed Fail button during test. </li>
     *      <li> ResumeSpeedNotIncreasing - Vehicle speed is not increasing after the Resume button is pressed. </li>
     *      </ul>
     *
     * <b>Prompts:</b><br>
     *      <ul>
     *      <li> PressResumePrompt - Press and hold the cruise resume button </li>
     *      <li> PressFailPrompt -  Press the Fail button if the resume feature does not function. </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SetStartTime() </li>
     *      <li> SetupForOperatorInput() </li>
     *      <li> WaitForResumeButton() </li>
     *      <li> IsSpeedIncreasing() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of the test step.
     *      <ul>
     *      <li> Pass - Speed Control resume was verified. </li>
     *      <li> Fail - Speed Control resume was not verified. </li>
     *      <li> Timeout - Speed Control resume not verified in the time allotted. </li>
     *      </ul>
     */
    virtual const string CheckResumeEngagement(void);

    /**
     * Verify that the cruise cuts out when the brake pedal is applied.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Wait for driver to tap brakes and check for speed decreasing.  If the speed control does not cut out when the
     * brake pedal is pressed, the operator can press the fail button instead of waiting for the test to time out.
     * <br><p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SpeedControlDeltaSpeed - Minimum speed decrease required when speed control disengages. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b><br>
     *      <ul>
     *      <li> ConditionsNotCorrect - Speed control ON and Set modes not seen prior to CheckBrakeCutOut test. </li>
     *      <li> BrakeTapTimeout - Speed Control brake tap not seen in the allowed time. </li>
     *      <li> SystemStatus - System status prevents further testing. </li>
     *      <li> CommunicationFailure - Communication failure checking for brake cut out. </li>
     *      <li> OperatorFailBrake - Operator pressed Fail button during test. </li>
     *      <li> BrakeSpeedDropOff - Vehicle speed did not decrease after tapping the brake pedal. </li>
     *      </ul>
     *
     * <b>Prompts:</b><br>
     *      <ul>
     *      <li> TapBrakePrompt - Tap brakes </li>
     *      <li> PressFailPrompt - Press the fail button if the speed control system does not disengage. </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SetupForOperatorInput() </li>
     *      <li> SetStartTime() </li>
     *      <li> WaitForBrakeTap() </li>
     *      <li> IsDeltaSpeedAchieved() </li>
     *      <li> GetSpeedControlTargetSpeed() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> ReadVehicleSpeed() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Result of verifying vehicle speed drops off when speed control is disengaged for brake tap.
     */
    virtual const string CheckBrakeCutOut(void);

    /**
     * Verify that the cruise cuts out when the clutch pedal is applied.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Wait for driver to press clutch and check for speed decreasing.  If the speed control does not cut out when the
     * clutch is pressed, the operator can press the fail button instead of waiting for the test to time out.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SpeedControlDeltaSpeed - Minimum speed decrease required when speed control disengages. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b><br>
     *      <ul>
     *      <li> ConditionsNotCorrect - Speed control ON and Set modes not seen prior to CheckClutchCutOut test. </li>
     *      <li> ClutchSwitchTimeout - Speed Control clutch switch not seen in the allowed time. </li>
     *      <li> SystemStatus - System status prevents further testing. </li>
     *      <li> CommunicationFailure - Communication failure checking for clutch cut out. </li>
     *      <li> OperatorFailClutch - Operator pressed Fail button during test. </li>
     *      <li> ClutchSpeedDropOff - Vehicle speed did not drop off when the clutch pedal was pressed. </li>
     *      </ul>
     *
     * <b>Prompts:</b><br>
     *      <ul>
     *      <li> PressClutchPrompt - Press the clutch. </li>
     *      <li> PressFailPrompt - Press the fail button if the speed control does disengage when the clutch pedal 
     *                             is pressed. </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SetupForOperatorInput() </li>
     *      <li> SetStartTime() </li>
     *      <li> WaitForClutchPedal() </li>
     *      <li> IsDeltaSpeedAchieved() </li>
     *      <li> GetSpeedControlTargetSpeed() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> ReadVehicleSpeed() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Result of validating the speed control disengages when the clutch pedal is pressed.
     *      <ul>
     *      <li> Pass - Speed Control CheckClutchCutOut was verified. </li>
     *      <li> Fail - Speed Control CheckClutchCutOut was not verified. </li>
     *      <li> Timeout - Speed Control CheckClutchCutOut not verified in the time allotted. </li>
     *      </ul>
     */
    virtual const string CheckClutchCutOut(void);

    /**
     * Verify that the cruise cuts out when the CANCEL switch is pressed.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Wait for driver to press cancel and check for speed decreasing.  If the speed control does not cut out when the
     * cancel switch is pressed, the operator can press the fail button instead of waiting for the test to time out.
     * <br><p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SpeedControlDeltaSpeed - Minimum speed decrease required when speed control disengages. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b><br>
     *      <ul>
     *      <li> ConditionsNotCorrect - Speed control ON and Set modes not seen prior to CheckCancelCutOut test. </li>
     *      <li> CancelSwitchTimeout - Speed Control cancel switch not seen in the allowed time. </li>
     *      <li> SystemStatus - System status prevents further testing. </li>
     *      <li> CommunicationFailure - Communication failure checking for cancel cut out. </li>
     *      <li> OperatorFailCancel - Operator pressed Fail button during test. </li>
     *      <li> CancelSpeedDropOff - Vehicle speed did not decrease when the cancel button was pressed. </li>
     *      </ul>
     *
     * <b>Prompts:</b><br>
     *      <ul>
     *      <li> CancelPrompt - Press and release the cancel button </li>
     *      <li> PressFailPrompt -  Press the fail button if speed control does not cancel. </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SetupForOperatorInput() </li>
     *      <li> SetStartTime() </li>
     *      <li> WaitForCancelSwitch() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> StatusCheck() </li>
     *      <li> IsDeltaSpeedAchieved() </li>
     *      <li> GetSpeedControlTargetSpeed() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> ReadVehicleSpeed() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of validating the speed control disengages when the cancel button is pressed.
     *      <ul>
     *      <li> Pass - Speed Control CheckCancelCutOut was verified. </li>
     *      <li> Fail - Speed Control CheckCancelCutOut was not verified. </li>
     *      <li> Timeout - Speed Control CheckCancelCutOut not verified in the time allotted. </li>
     *      </ul>
     */
    virtual const string CheckCancelCutOut(void);

    /**
     * Verify vehicle speed changes when the Accel/Decel button is tapped.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This test step assumes the speed control is currently engaged.
     * The operator will be instructed to press the accel/decel button.  The vehicle speed will be monitored to
     * ensure it has changed.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> TapSpeedChangeMinSpeedChange - Minimum speed change required when the operator presses
     *                                          the accel/decel button. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> ScanDelay - Time to wait in ms between speed checks. </li>
     *      </ul>
     *
     * <b>Prompts:</b>
     *      <ul>
     *      <li> PressTapUpButton - Press the accel button to increase vehicle speed </li>
     *      <li> PressTapDownButton - Press the decel button to decrease vehicle speed </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> TapUpTimeout - Timeout waiting for vehicle speed to increase. </li>
     *      <li> TapDownTimeout - Timeout waiting for vehicle speed to decrease. </li>
     *      <li> SystemStatus - System status is preventing testing from continuing. </li>
     *      <li> VehicleSpeedRead - Could not read vehicle speed. </li>
     *      <li> OperatorFailTapUp - The operator pressed the fail button. </li>
     *      <li> OperatorFailTapDown - The operator pressed the fail button. </li>
     *      <li> TapUpSpeedChange - Vehicle speed did not increase when the tap up button was pressed. </li>
     *      <li> TapDownSpeedChange - Vehicle speed did not decrease when the tap down button was pressed. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetComponentName()) </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> ReadVehicleSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SetupForOperatorInput() </li>
     *      <li> CheckForOperatorFail() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameter() </li>
     *      <li> BposSleep() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> CreateMessage() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     *
     * @param direction Direction for the operator to change speed.
     *      <ul>
     *      <li> Up - Look for vehicle speed increase.</li>
     *      <li> Down - Look for vehicle speed decrease.</li>
     *      </ul>
     *
     * @return Result of verifying the vehicle speed changes when the Accel/Decel vuttons are pressed.
     *      <ul>
     *      <li> Pass - The vehicle speed changed when the operator pressed the accel/decel button. </li>
     *      <li> Fail - The vehicle speed did not change or the operator pressed the fail button. </li>
     *      <li> Abort - System status is preventing further testing. </li>
     *      <li> Timeout - Timeout waiting for vehicle speed to change as a result of the button press. </li>
     *      </ul>
     */
    virtual const string CheckTapSpeedChange(const string &direction);

    /**
     * Check the functionality of the speed limiter.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The operator will be instructed to to engage the speed limiter and attempt to accelerate above the speed
     * limiter.  The vehicle speed will then be checked to determine if it is greater than the speed limit.  If the
     * speed limit cannot be read from the module due to no module comms provided by the customer, the operator will
     * be prompted for a pass/fail result.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadSpeedLimit - Data tag for the message to be sent to the module to read the speed limit. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CheckSpeedAgainstLimit - Flag to indicate if the vehicle speed should be checked against a set speed limit.
     *                                    If this flag is set to false, the operator will just be prompted to enter pass/fail. </li>
     *      <li> SpeedLimiterEngageDelay - Time in ms to wait for the operator to engage the speed limiter. </li>
     *      <li> ReadSpeedLimitFromModule - Flag to indicate if the sped limit should be read from the module or if a
     *                                      parameter value should be used fro the speed limit. </li>
     *      <li> SpeedLimiterSpeedLimit - Speed limit of the vehicle when the speed limiter is engaged. </li>
     *      <li> MinimumSpeedOverLimit - Minimum speed over the limit the vehicle must achieve to fail. </li>
     *      <li> SpeedLimiterRange - Speed range for the operator to attempt to achieve when speed limiter is active. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> ScanDelay - Time to wait between checks of vehicle speed. </li>
     *      </ul>
     *
     * <b>Prompts:</b>
     *      <ul>
     *      <li> EngageSpeedLimiter - Engage the speed limiter on the vehicle. </li>
     *      <li> AccelToSpeedLimiter - Prompt operator to accelerate abover the speed limiter. </li>
     *      <li> SpeedLimiterResult - Prompt operator to enter result of testing speed limiter. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> OperatorTimeoutSpeedLimiter - Operator did not enter test result in a timely manner. </li>
     *      <li> OperatorFailSpeedLimiter - Operator pressed the fail button for the speed limiter. </li>
     *      <li> OperatorAbortSpeedLimiter - Operator pressed the abort key during the speed limiter test. </li>
     *      <li> VehicleSpeedLimitRead - Could not read speed limit from the module. </li>
     *      <li> SpeedLimiterFail - Vehicle speed limiter is not functioning correctly. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> SetupForOperatorInput() </li>
     *      <li> SetupOperatorResponses() </li>
     *      <li> GetOperatorInput() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetDataTag() </li>
     *      <li> AccelerateToTestSpeed() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      </ul>
     *
     * @return Result of verifying the speed limiter is functioning correctly.
     *      <ul>
     *      <li> Pass - The vehicle did not accelerate above the speed limit. </li>
     *      <li> Fail - The vehicle was able to accelerate over the speed limit. </li>
     *      <li> Timeout - The operator did not enter pass/fail response. </li>
     *      <li> Abort - The operator pressed the abort button. </li>
     *      <li> Skip - This test does not need to be performed. </li>
     *      </ul>
     */
    virtual const string CheckSpeedLimiter(void);

    /**
     * Verify the Speed Limiter disengages when the operator acclerated under full load.
     * 
     * @return Result of speed limiter disengage.
     */
    const string CheckKickDown(void);

    /**
     * Command module security unlock.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Unlock the module security so the EOL status registers can be updated.
     * <br><p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b><br>
     *      <ul>
     *      <li> SecurityUnlockFailed - Failed to unlock module security. </li>
     *      <li> PreconditionTimeout - Timeout waiting for preconditions to be satisfied. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> SecurityUnlockAttempts - Number of attempts to unlock module security.  <b><i>Note:</i></b> Excessive
     *                                    failed attempts can lock out the module and prevent the engine from starting. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> UnlockModuleSecurity() </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> CheckIgnitionOn() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of unlocking module security.
     *      <ul>
     *      <li> Pass: Command security unlock successful. </li>
     *      <li> Abort: Timeout waiting for preconditions to be met. </li>
     *      </ul>
     */
    virtual string CommandSecurityUnlock(void);

    /**
     * Determine Previous Pass
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Check the module to determine if it is in previous pass or Ok To Ship status.
     * <br><p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b><br>
     *      <ul>
     *      <li> CommunicationFailure - Error reading previous pass status from the module. </li>
     *      <li> SoftwareFailure - Module exception determining previous pass. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message tags: IsPreviousPass </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> SetPreviousPass() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> IsPreviousPass() </li>
     *      </ul>
     *
     * @return Result of determining the previous pass/OK to Ship status of the vehicle.
     *      <ul>
     *      <li> Pass: Determine Previous Pass Successful. </li>
     *      <li> Fail: Communications error determining previous pass. </li>
     *      </ul>
     */
    virtual string DeterminePreviousPass(void);

    /**
     * Update module result.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Update the overall result of the cruise test in the module.
     * <br><p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b><br>
     *      <ul>
     *      <li> CommunicationFailure - Error updating module result. </li>
     *      <li> CableConnectTimeout - Timeout waiting for the cable to be connected. </li>
     *      <li> SoftwareFailure - Module exception updating module result. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> UnlockModuleSecurity() </li>
     *      <li> UpdateEEPROMTestBit() </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> SpeedControlStatusByte - Byte indicating status of the Speed Control </li>
     *      <li> SpeedControlTestResultBit - Bit indicating test pass or fail </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetOverallResult() </li>
     *      <li> CheckCableConnect() </li>
     *      <li> IsPreviousPass() </li>
     *      <li> GetParameter() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of recording the test result in the module.
     *      <ul>
     *      <li> Pass: Successfully updated module result </li>
     *      <li> Fail: Failed to update module result </li>
     *      <li> Timeout: Timeout updating module result </li>
     *      <li> Skip: Test was a previous pass </li>
     *      </ul>
     */
    virtual string UpdateModuleResult(void);

    /**
     * Prompt the operator for the overall speed control result.
     * 
     * @return Operator subjective result for the speed control functionality.
     */
    string SpeedControlOperatorResult(void);

    /**
     * Check if the vehicle speed is stable.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * For vehicle speed to be stable, throttle must not be applied and the vehicle speed
     * must remain within tolerance for the required amount of time/samples.
     * A short delay is provided at the beginning of the method to allow operator time
     * to remove foot from throttle.
     * <br><p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between throttle checks. </li>
     *      </ul>
     * 
     * <b>Parameters:</b><br>
     *      <ul>
     *      <li> StableSpeedMaxVariation - Added to target speed to get maximum speed allowed. </li>
     *      <li> StableSpeedMinVariation - Subtracted from target speed to obtain minimum speed allowed. </li>
     *      <li> NoModuleComms - Flag indicating if we have communications with the module. </li>
     *      <li> CheckForClosedThrottle - Flag to determine whether or not to check for closed throttle. </li>
     *      <li> StableSpeedSampleDelay - Delay between vehicle speed checks for stable speed. </li>
     *      <li> StableSpeedPoints - Number of stable speed samples required. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameterBool() </li>
     *      <li> IsZeroThrottle() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> StatusCheck() </li>
     *      <li> ReadVehicleSpeed() </li>
     *      <li> CheckForOperatorFail() </li>
     *      </ul>
     * 
     * @param targetSpeed
     *               Target speed for the cruise module.  This is the speed at which the set button was pressed.
     * @param zeroThrottle
     *               Flag indicating if zero throttle was applied throughout the evaluation.
     * 
     * @return Flag indicating if the vehicle speed is stable.
     *         <ul>
     *         <li> True: Vehicle speed has remained within tolerance for the required amount of time/samples. </li>
     *         <li> False: Vehicle speed has either increased or decreased beyond the given tolerance. </li>
     *         </ul>
     */
    virtual const bool IsVehicleSpeedStable(const float &targetSpeed, bool &zeroThrottle, 
                                            float minSpeed = 0.0, float maxSpeed = 0.0);

    /**
     * Check if the required speed delta has been achieved.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Determines if the required speed delta has been observed.
     * <p>
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> DeltaSpeedCheckDelay - Amount of time in milliseconds to wait between checks for delta speed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> ReadVehicleSpeed() </li>
     *      <li> CheckForOperatorFail() </li>
     *      <li> GetParameterInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @param initialSpeed
     *                   Initial speed to use for calculating the delta.
     * @param deltaSpeed Change in speed required.
     * 
     * @return Flag indicating if the required delta speed change has been achieved.
     *         <ul>
     *         <li> True - The required speed change has been observed. </li>
     *         <li> False - The required speed delta has not occurred. </li>
     *         </ul>
     */
    virtual const bool IsDeltaSpeedAchieved(const float &initialSpeed, const float &deltaSpeed);

    /**
     * Check for not throttle applied.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Determines if the operator is not applying throttle.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ClosedThrottleVoltage5.7L - Closed throttle reading for ETC vehicles. </li>
     *      <li> ClosedThrottleVoltage - Closed throttle reading for non-ETC vehicles. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ThrottleClosedVoltageMessage5.7L - Message tag to read closed throttle for ETC vehicles. </li>
     *      <li> ThrottleClosedVoltageMessage - Message tag to use for reading non-ETC closed throttle. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> GetInfo() - Message tags: ThrottleClosedVoltageMessage5.7L, ThrottleClosedVoltageMessage </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> IsETCVehicle() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameterFloat() </li>
     *      </ul>
     *
     * @return  Flag indicating if the throttle is closed.
     *      <ul>
     *      <li> True - Operator is not applying throttle. </li>
     *      <li> False - The operator is applying throttle. </li>
     *      </ul>
     */
    virtual const bool IsZeroThrottle(void);

    /**
     * Check if vehicle speed is increasing.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Determines if the vehicle speed is increasing or already at the target speed.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Delay between speed samples. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> StableSpeedVariation - Added and subtracted from target speed to get a target range. </li>
     *      <li> CheckForClosedThrottle - Flag to determine whether or not to check for closed throttle. </li>
     *      <li> SpeedIncreaseSamples - Number of increasing speed samples required. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetSpeedControlTargetSpeed() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetParameterInt() </li>
     *      <li> ReadVehicleSpeed() </li>
     *      <li> CheckForOperatorFail() </li>
     *      <li> IsZeroThrottle() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return  Flag indicating if the vehicle speed is increasing.
     *      <ul>
     *      <li> True - Vehicle speed is increasing or target speed was attained. </li>
     *      <li> False - Vehicle speed is <b><i>not</i></b> increasing and has <b><i>not</i></b> reached the target speed. </li>
     *      </ul>
     */
    virtual const bool IsSpeedIncreasing(void);

    /**
     * Read the current vehicle speed.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Read the current vehicle speed from the cruise module or from the rolls depending on the setting of parameter
     * <i>UseModuleSpeed</i>.
     * 
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> UseModuleSpeed - Flag indicating if the vehicle speed should be read from the module or from the rollers. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> GetInfo() - Message Tags: ReadVehicleSpeed </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetParameterBool() </li>
     *      <li> GetRollSpeed() </li>
     *      </ul>
     * 
     * @param vehicleSpeed
     *               Current vehicle speed.
     * 
     * @return Status of reading the vehicle speed.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Vehicle speed was successfully retrieved. </li>
     *         <li> BEP_STATUS_FAILURE - Vehicle speed could not be read. </li>
     *         </ul>
     */
    virtual const BEP_STATUS_TYPE ReadVehicleSpeed(float &vehicleSpeed);

    /**
     * Wait for operator to switch the speed control system On.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Wait for the speed control to be switched on.  The speed control switch will be monitored as well
     * as the fail button in the event the speed control system is non-functional.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadSpeedControlOn - Message tag to be used to read cruise switch on.</li>
     *      </ul>
     * 
     * <b>Prompts:</b><br>
     *      <ul>
     *      <li> SpeedControlOn - Press and release cruise on button </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> SpeedControlOnMask - Mask to be applied to the data to determine if the speed control is ON. </li>
     *      <li> SpeedControlModeCheckDelay - Delay between switch status checks. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> ReadCruiseMode() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> CheckForOperatorFail() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> BposSleep() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @param cruiseOn Flag indicating if speed control on was detected.
     * @param operatorInput
     *                 Input from the operator if the speed control did not enable.
     * 
     * @return Status of reading the speed control status from the module.
     */
    virtual BEP_STATUS_TYPE WaitForSpeedControlOn(bool &cruiseOn, string &operatorInput);

    /**
     * Wait for speed control off
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Wait for the speed control to be switched off.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadSpeedControlOff - Message tag to be used to read cruise switch off.</li>
     *      </ul>
     * 
     * <b>Prompts:</b><br>
     *      <ul>
     *      <li> SpeedControlOff - Press and release cruise off button </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> SpeedControlOffMask - Mask to be applied to the data read from the module to determine if speed
     *                                 control is off. </li>
     *      <li> SpeedControlModeCheckDelay - Delay between checking switch status. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> ReadCruiseMode() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> CheckForOperatorFail() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @param cruiseOff Flag indicating if speed control off was detected.
     * @param operatorInput
     *                  Input from the operator if the speed control is non-functional.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE WaitForSpeedControlOff(bool &cruiseOff, string &operatorInput);

    /**
     * Wait for the set button to be pressed.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Wait for the operator to press the Set button.  If module communications are being utilized, the switch status
     * will be read from the module.  Otherwise, stable vehicle speed would indicate the set button has been pressed.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadSetButton - Message tag to be used to read cruise set button. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> NoModuleComms - Flag to indicate if module communications are being utilized. </li>
     *      <li> SpeedControlSetMask - Mask to be applied to the data to determine if the speed control is set. </li>
     *      <li> SpeedControlModeCheckDelay - Delay between checking switch status. </li>
     *      <li> SpeedControlMaxSpeed - Maximum speed limit for the speed control test. </li>
     *      <li> SpeedControlMinSpeed - Minimum speed limit for the speed control test. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> ReadCruiseMode() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameter() </li>
     *      <li> GetDataTag() </li>
     *      <li> CheckForOperatorFail() </li>
     *      <li> StatusCheck() </li>
     *      <li> IsVehicleSpeedStable() </li>
     *      </ul>
     * 
     * @param cruiseSet Flag to determine if the set button was pressed.
     * @param operatorInput
     *                  Input from the operator if the speed control is non-functional.
     * 
     * @return Status of determining if the Set button was pressed.
     */
    virtual BEP_STATUS_TYPE WaitForSetButton(bool &cruiseSet, string &operatorInput);

    /**
     * Wait for the resume button to be pressed.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Wait for the operator to press the Resume button.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadResumeButton - Message tag to be used to read cruise resume button.</li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> SpeedControlResumeMask - Mask to be applied to the data read from the module to determine if the
     *                                    speed control is in resume mode. </li>
     *      <li> SpeedControlModeCheckDelay - Delay between switch status checks. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> ReadCruiseMode() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> CheckForOperatorFail() </li>
     *      <li> GetParameter() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     * 
     * @param cruiseResume
     *               Flag to determine if resume button was pressed.
     * @param operatorInput
     *               Input from the operator if the speed control is non-functional.
     * 
     * @return Status of reading the resume switch status from the module.
     */
    virtual BEP_STATUS_TYPE WaitForResumeButton(bool &cruiseResume, string &operatorInput);

    /**
     * Wait for the brake pedal to be tapped.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Wait for the operator to tap the brake pedal.  If there are no communications with the module, the vehicle
     * speed will be monitored to determine if speed is decreasing.  Decreasing vehicle speed indicates the speed control
     * has been disengaged.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadBrakeSwitch - Message tag to be used to read the brake switch.</li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> NoModuleComms - Flag to indicate if module communications are being utilized. </li>
     *      <li> SpeedControlBrakeMask - Mask to be applied to the data read from the module to determine if the brake
     *                                    pedal has been pressed. </li>
     *      <li> SpeedControlModeCheckDelay - Delay between switch status checks. </li>
     *      <li> SpeedControlDeltaSpeed - Speed delta required to determine if the speed control has been disengaged. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> ReadCruiseMode() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameterBool() </li>
     *      <li> CheckForOperatorFail() </li>
     *      <li> GetDataTag() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> ReadVehicleSpeed() </li>
     *      <li> IsDeltaSpeedAchieved() </li>
     *      </ul>
     * 
     * @param brakePressed
     *               Flag to determine if the brake pedal was pressed.
     * @param operatorInput
     *               Input from the operator if the speed control is non-functional.
     * 
     * @return Status of waiting for the speed control to be disengaged by tapping the brake pedal.
     */
    virtual BEP_STATUS_TYPE WaitForBrakeTap(bool &brakePressed, string &operatorInput);

    /**
     * Wait for the clutch pedal to be applied.
     * <p><b>Category:</b> Utility
     * <br><p>
     * Wait for the operator to press the clutch pedal.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadClutchPedal - Message tag to be used to read clutch pedal.</li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> SpeedControlClutchMask - Mask to be applied to the data read from the mdoule to determine if the clutch
     *                                    pedal has been pressed. </li>
     *      <li> SpeedControlModeCheckDelay - Delay between switch status checks. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> ReadCruiseMode() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> CheckForOperatorFail() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @param clutchPressed
     *               Flag to determine if clutch pedal was pressed.
     * @param operatorInput
     *               Input from the operator if the speed control is non-functional.
     * 
     * @return Status of reading the clutch switch from the module.
     */
    virtual BEP_STATUS_TYPE WaitForClutchPedal(bool &clutchPressed, string &operatorInput);

    /**
     * Wait for the cancel switch to be pressed.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Wait for the operator to press the cancel switch.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadCancelSwitch - Message tag to be used to read cancel switch.</li>
     *      </ul>
     * 
     * <b>Parameters:</b><br>
     *      <ul>
     *      <li> SpeedControlCancelMask - Mask to be applied to the data read from the module to determine if the cancel
     *                                    button has been pressed. </li>
     *      <li> SpeedControlModeCheckDelay - Delay between switch status checks. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> ReadCruiseMode() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> CheckForOperatorFail() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @param cancelPressed
     *               Flag to determine if cancel button was pressed.
     * @param operatorInput
     *               Input from the operator if the speed control is non-functional.
     * 
     * @return Status of reading the cancel switch status from the module.
     */
    virtual BEP_STATUS_TYPE WaitForCancelSwitch(bool &cancelPressed, string &operatorInput);

    /**
     * Check if the operator pressed the fail button.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if the operator has pressed the fail button.
     * <p>
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> SystemRead() </li>
     *      </ul>
     *
     * @return  Button pressed by the operator.
     *      <ul>
     *      <li> Fail - The operator has pressed the fail button. </li>
     *      <li> Pass - The operator has pressed the pass button. </li>
     *      <li> Abort - The operator has pressed the abort button. </li>
     *      </ul>
     */
    virtual const string CheckForOperatorFail(void);

    /**
     * Setup for operator input.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Setup the InputServer so we can check if the operator pressed the fail button during the test.
     * <p>
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> INamedDataBroker::Write() </li>
     *      </ul>
     */
    virtual void SetupForOperatorInput(void);

    /**
     * Set speed control target speed.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Store the speed at which the speed control system was set.  If module communications are being utilized, the
     * vehicle speed will be read from the module; otherwise, the roller speed will be used as the vehicle speed.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> UseModuleSpeed - Flag to determine if module communications are being utilized.</li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b><br>
     *      <ul>
     *      <li> GetInfo() - Message tags: ReadCruiseTargetSpeed</li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GetRollSpeed() </li>
     *      <li> GetParameterBool() </li>
     *      </ul>
     * 
     * @return Status of storing the speed control target speed.
     */
    virtual const BEP_STATUS_TYPE SetSpeedControlTargetSpeed(void);

    /**
     * Determine if the vehicle has previously passed.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Get the flag indicating the vehicle's previous pass status.
     * <p>
     * @return  Flag indicating if the vehicle has previously passed emissions testing.
     *      <ul>
     *      <li> True - The vehicle has previously passed emissions testing. </li>
     *      <li> False - The vehicle has not previously passed emissions testing. </li>
     *      </ul>
     */
    virtual const bool &IsPreviousPass(void);
    /**
     * Check if the vehicle is equipped with speed control.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Get the flag indicating if speed control is equipped on the current vehicle.
     * <p>
     * @return  Flag indicating if the vehicle is equipped with speed control.
     *      <ul>
     *      <li> True - Vehicle is equipped with speed control. </li>
     *      <li> False - The vehicle is not equipped with speed control. </li>
     *      </ul>
     */
    virtual const bool &IsCruiseEquipped(void);
    /**
     * Store the flag indicating if the vehicle is equipped with speed control.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Set the flag indicating if the vehicle is equipped with Speed control.
     * <p>
     * @param equipped  Flag indicating if the vehicle is equipped with speed control.
     *      <ul>
     *      <li> True - The vehicle is equipped with speed control. </li>
     *      <li> False - The vehicle is not equipped with speed control. </li>
     *      </ul>
     */
    virtual void SetCruiseEquipped(const bool &equipped);

	/**
	 * Verify the vehicle speed is stable.
	 * This simply makes a test step out of IsStableSpeed() but passes in the current roller speed.
	 * 
	 * @return Result of verifying the vehicle is at steady speed.
	 */
	string VerifySteadySpeed(void);

	/**
     * Status of the individual cruise tests.
     */
    SpeedControlTestStatus m_testStepStatus;
    /**
     * Object to convert and analyze data.
     */
    DataAnalysis m_analyze;

private:
    /**
     * Store the flag indicating if emissions testing has previously passed.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Store the flag indicating the vehicle's previous pass status.
     * <p>
     * @param isPreviousPass Flag indicating the vehicle's previuous pass status.
     *      <ul>
     *      <li> True - The vehicle has previously passed emissions testing. </li>
     *      <li> False - The vehicle has not previously passed emissions testing. </li>
     *      </ul>
     */
    void SetPreviousPass(const bool &isPreviousPass);

    /**
     * Speed control target speed.
     * This is the speed at which the system was set.
     */
    float  m_targetSpeed;
    /**
     * Flag to determine if vehicle is equipped with Electronic throttle control.
     */
    bool   m_isETCVehicle;
    /**
     * Flag indicating if the vehicle has previously passed emissions testing.
     */
    bool m_previousPass;
    /**
     * Flag indicating if the vehicle is equipped with speed control.
     */
    bool m_speedControlEquipped;
    /**
     * Store the the valid Speed Control Types for this test from configuration file
     */
    XmlNodeMap m_validSpeedControlTypes;
};

#endif //GenericSpeedControlTC_h
