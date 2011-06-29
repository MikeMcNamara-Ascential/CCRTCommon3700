//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/TestComponents/Axle/Include/TandemLockingDifferentialTC.h 1     6/09/05 4:46p Derickso $
//
// FILE DESCRIPTION:
//  Electronic Locking Differential Test Component.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Axle/Include/TandemLockingDifferentialTC.h $
// 
// 1     6/09/05 4:46p Derickso
// New test component.
// 
// 3     1/03/05 1:44p Mkelly
// 
// 1     11/08/04 1:17p Mkelly
// 
// 1     11/01/04 3:26p Mkelly
// 
//*************************************************************************
#ifndef TandemLockingDifferentialTC_h
#define TandemLockingDifferentialTC_h
//*************************************************************************
#include "GenericTCTemplate.cpp"
#include "IMotorController.h"
#include <cmath>

/**
 * Electronic Locking Differential Test component.
 * This class tests the locking differential on the vehicle to
 * ensure it operates properly.
 * 
 * @author Mike Kelly
 * @see GenericTCTemplate
 * @see FDCMModule
 * @see GenericABSModule
 * @since August 14, 2003
 */
class TandemLockingDifferentialTC : public GenericTC
{
public:
	/**
	 * Class constructor.
	 */
	TandemLockingDifferentialTC();
	/**
	 * Class destructor.
	 */
	virtual ~TandemLockingDifferentialTC();
	/**
	 * Execute the specified test step.
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * This is the method that will handle the execution of each
	 * test step that has been specified by the system for each
	 * test objective that is passed to it.  This must be overloaded
	 * to execute the component tests.
	 * <br><p>
	 *
	 * <b>Test Step Functions Called:</b>
	 * 		<ul>
	 *      <li> DriveRrRoll() </li>
	 *      <li> DriveLfRoll() </li>
	 *      <li> DriveRfAndLrRolls() </li>
	 *      <li> ShiftToNeutral4Low() </li>
	 *      <li> ShiftToNeutral() </li>
	 *      <li> PromptShiftTo2High() </li>
	 *      <li> SwitchDifferentialTo() </li>
	 *      <li> CheckWheelSpeedWithRearLock() </li>
	 *      <li> CheckWheelSpeedWithFrontLock() </li>
	 *      <li> SetRollsToTorqueMode() </li>
	 *      <li> SetRollsToBoostMode() </li>
	 *      <li> Delay() </li>
	 *      </ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 *      <li> StatusCheck() </li>
	 *      <li> GetTestStepName() </li>
	 *      <li> CheckZeroSpeed() </li>
	 *      <li> GenericTCTemplate::CommandTestStep() </li>
	 *      <li> ConvertStatusToResponse() </li>
	 *      </ul>
	 * 
	 * @param value  This is the value that was commanded by
	 *               the test sequencer.
	 * 
	 * @return The result of the test step that was evecuted.
	 */
	virtual const string CommandTestStep(const string &value);

	void Initialize(const string &fileName);

	void Initialize(const XmlNode *config);

	const string Register(void);


protected:
	/**
	 * Setup Rolls for the Transversal Lock test by commanding speed to the appropriate rolls
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * The machine will be engaged first.  All left side rollers will then be set to torque mode.
	 * The right rear roller will be set to speed mode and commanded to speed.  
     * The right tandem and right front rolls will be set to speed mode and commanded to speed
     * depending on the vehicle's configuration.
	 * <br><p>
	 *
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> TransversalLockWheelSpeed - Speed to command the right roller(s) to. </li>
	 * 		<li> TransversalLockWheelTorque - Torque to command the left roller(s) to. </li>
	 * 		</ul>
	 * 
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Description of the test step. </li>
	 * 		<li> ScanDelay - Amount of time to wait between checks for wheel speed in range. </li>
	 * 		</ul>
	 *
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> DriveRollsTimeoutFailure - The driven roller(s) did not achieve the required speed range in the time allowed. </li>
	 * 		<li> SoftwareFailure - Could not engage the machine or set the rollers to the desired state. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo() </li>
	 * 		<li> EngageMachine() </li>
	 * 		<li> GetParameter() </li>
	 * 		<li> DisplayPrompt() </li>
	 * 		<li> IsRearAndTandemTransEquipped() </li>
	 * 		<li> IsFrontAndRearTransEquipped() </li>
	 * 		<li> IsRearTransEquipped() </li>
	 * 		<li> SetStartTime() </li>
	 * 		<li> GetWheelSpeeds() </li>
	 * 		<li> TimeRemaining() </li>
	 * 		<li> BposSleep() </li>
	 * 		<li> GetFaultCode() </li>
	 * 		<li> GetFaultDescription() </li>
	 * 		<li> SetRollsToTorqueMode() </li>
	 * 		<li> SendSubTestResult() </li>
	 * 		</ul>
	 * 
	 * @return Test result of the test step.
	 * 		<ul>
	 * 		<li> Pass - The rollers commanded to speed reached the desired speed range in the time allowed. </li>
	 * 		<li> Timeout - Wheel speed did not reach he desired speed range in the allowed time. </li>
	 * 		<li> SoftwareFail - There was an error commanding the rollers to the desired states. </li>
	 * 		</ul>
	 */  
	string DriveRollsTransversalLock(void);
	/**
	 * Setup Rolls for the Transversal Lock test by commanding speed to the appropriate rolls
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * The machine will be engaged first.  
     * Rolls will be setup based on the vehicles longitudinal axle locking configuration
     * Rear and Tandem: LF and RF are set to torque and commanded torque value of zero
     *                  LR and RR are set to torque and commanded torque value set by parameter
     *                  LT and RT are set to speed and commanded speed value set by parameter
     * Front and rear:  LF and RF are set to torque and commanded torque value set by parameter
     *                  LR and RR are set to speed and commanded speed value set by parameter
     *                  LT and RT are set to torque and commanded torque value of zero
	 * <br><p>
	 *
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> LongitudinalLockWheelSpeed - Speed to command the driven rollers to. </li>
	 * 		<li> LongitudinalLockWheelTorque - Torque to command the non driven locked axle rollers. </li>
	 * 		</ul>
	 * 
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Description of the test step. </li>
	 * 		<li> ScanDelay - Amount of time to wait between checks for wheel speed in range. </li>
	 * 		</ul>
	 *
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> DriveRollsTimeoutFailure - The driven rollers did not achieve the required speed range in the time allowed. </li>
	 * 		<li> SoftwareFailure - Could not engage the machine or set the rollers to the desired state. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo() </li>
	 * 		<li> EngageMachine() </li>
	 * 		<li> GetParameter() </li>
	 * 		<li> DisplayPrompt() </li>
	 * 		<li> IsRearAndTandemLongEquipped() </li>
	 * 		<li> IsFrontAndRearLongEquipped() </li>
	 * 		<li> SetStartTime() </li>
	 * 		<li> GetWheelSpeeds() </li>
	 * 		<li> TimeRemaining() </li>
	 * 		<li> BposSleep() </li>
	 * 		<li> GetFaultCode() </li>
	 * 		<li> GetFaultDescription() </li>
	 * 		<li> SetRollsToTorqueMode() </li>
	 * 		<li> SendSubTestResult() </li>
	 * 		</ul>
	 * 
	 * @return Test result of the test step.
	 * 		<ul>
	 * 		<li> Pass - The rollers commanded to speed reached the desired speed range in the time allowed. </li>
	 * 		<li> Timeout - Wheel speed did not reach he desired speed range in the allowed time. </li>
	 * 		<li> SoftwareFail - There was an error commanding the rollers to the desired states. </li>
	 * 		</ul>
	 */  
	string DriveRollsLongitudinalLock(void);
	/**
	 * Verify the wheel speeds are within a certain tolerance of each other side to side.
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * Wheel speeds will be checked to ensure they are within the specified tolerance.
     * The wheel speeds to compare are determined by broadcast data obtained in the initialize() function 
	 * <br><p>
	 *
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> TransversalLockWheelSpeedTolerance - The tolerance for the left roller speed compared to the right roller speed. </li>
	 * 		</ul>
	 * 
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Test step description. </li>
	 * 		</ul>
	 * 
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> TransversalLockFailure - Failed while checking transversal axle lock </li>
	 * 		<li> SoftwareFailure - An exception occured during the test step. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetWheelSpeeds() </li>
	 * 		<li> IsRearAndTandemTransEquipped() </li>
	 * 		<li> IsFrontAndRearTransEquipped() </li>
	 * 		<li> GetParameterFloat() </li>
	 * 		<li> GetFaultCode() </li>
	 * 		<li> GetFaultDescription() </li>
	 * 		<li> SendSubTestResultWithDetail() </li>
	 * 		<li> SetRollsToTorqueMode() </li>
	 * 		</ul>
	 *
	 * @return Result of checking the rear wheel speeds for the required speed tolerance.
	 * 		<ul>
	 * 		<li> Pass - The wheel speeds are within the required tolerance of each other side to side. </li>
	 * 		<li> Fail - The wheel speeds are <i><b>not</b></i> within the required tolerance of each other side to side. </li>
	 * 		</ul>
	 */
	string CheckWheelSpeedTransversalLock(void);
	/**
	 * Verify the wheel speeds are within a certain tolerance of each other front to rear or rear to tandem.
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * Wheel speeds will be checked to ensure they are within the specified tolerance.  
     * The two axles to compare are determined by broadcast data obtained in the initialize() function 
	 * <br><p>
	 *
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> LongitudinalLockWheelSpeedTolerance - The tolerance for the front roller speed compared to the rear roller speed. </li>
	 * 		</ul>
	 * 
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Test step description. </li>
	 * 		</ul>
	 * 
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> LongitudinalLockFailureFrontToRear - Failed while checking longitudinal axle lock front to rear</li>
	 * 		<li> LongitudinalLockFailureRearToTandem - Failed while checking longitudinal axle lock rear to tandem</li>
	 * 		<li> SoftwareFailure - An exception occured during the test step. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetWheelSpeeds() </li>
	 * 		<li> IsFrontAndRearLongEquipped() </li>
	 * 		<li> IsRearAndTandemLongEquipped() </li>
	 * 		<li> GetParameterFloat() </li>
	 * 		<li> GetFaultCode() </li>
	 * 		<li> GetFaultDescription() </li>
	 * 		<li> SendSubTestResult() </li>
	 * 		<li> SetRollsToTorqueMode() </li>
	 * 		</ul>
	 *
	 * @return Result of checking the rear wheel speeds for the required speed tolerance.
	 * 		<ul>
	 * 		<li> Pass - The wheel speeds are within the required tolerance of each other front to rear. </li>
	 * 		<li> Fail - The wheel speeds are <i><b>not</b></i> within the required tolerance of each other front to rear. </li>
	 * 		</ul>
	 */
	string CheckWheelSpeedLongitudinalLock(void);
	/**
	 * Command the all roll motors to Torque 0.
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * All roll motors will be commanded to torque mode with a torque setting of zero.
	 * <br><p>
	 * 
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Test step description. </li>
	 * 		</ul>
	 * 
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> SoftwareFailure - A failure occurred commanding all roll motors to torque zero. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo() </li>
	 * 		<li> GetFaultCode() </li>
	 * 		<li> GetFaultDescription() </li>
	 * 		<li> SendTestResult() </li>
	 * 		<li> GetComponentName() </li>
	 * 		<li> GetTestStepName() </li>
	 * 		</ul>
	 *
	 * @return Result of commanding all roll motors to torque zero.
	 * 		<ul>
	 * 		<li> Pass - All roll motors were successfully commanded to torque zero. </li>
	 * 		<li> SoftwareFail - A failure occurred commanding all roll motors to torque zaero. </li>
	 * 		</ul>
	 */
	string SetRollsToTorqueMode(void);
	/**
	 * Command all motors to Boost Mode.
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * Command all roll motors to Boost mode.
	 * <br><p>
	 *
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Test step description. </li>
	 * 		</ul>
	 * 
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> SoftwareFailure - A failure occurred commanding all roll motors to boost mode. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo() </li>
	 * 		<li> GetFaultCode() </li>
	 * 		<li> GetFaultDescription() </li>
	 * 		<li> SendTestResult() </li>
	 * 		<li> GetComponentName() </li>
	 * 		<li> GetTestStepName() </li>
	 * 		<li> SystemWrite() </li>
	 * 		</ul>
	 * 
	 * @return Result of commanding all roll motors to Boost mode.
	 * 		<ul>
	 * 		<li> Pass - All roll motors were successfully commanded to boost mode. </li>
	 * 		<li> SoftwareFail - A failure occurred commanding all roll motors to boost mode. </li>
	 * 		</ul>
	 */    
	string SetRollsToBoostMode(void);
	/**
	 * Delay the test.
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * The test will be delayed for the specified amount of time.
	 * <br><p>
	 *
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Test step description. </li>
			<li> Delay - Amount of time to delay the test sequence. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo() </li>
	 * 		<li> GetTestStepInfoInt() </li>
	 * 		<li> BposSleep() </li>
	 * 		<li> SendTestResult() </li>
	 * 		<li> GetComponentName() </li>
	 * 		<li> GetTestStepName() </li>
	 * 		</ul>
	 * 
	 * @return This test step always returns pass after the required time delay has expired.
	 */ 
	string Delay(void);
	/**
	 * Display timed prompt or display prompt for verification
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * The test will either display a timed prompt or a prompt which requires 
     * driver pass/fail verification.
	 * <br><p>
	 *
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Test step description. </li>
	 * 		</ul>
	 * 
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> Verify + promptTag - True: Operator prompted to press pass / fail to continue False: time prompt displayed </li>
	 * 		<li> promptTag + MessageDelay - Amount of time to display prompt if Verify + promptTag is set to false </li>
	 * 		</ul>
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo() </li>
	 * 		<li> DisplayPrompt() </li>
	 * 		<li> RemovePrompt() </li>
	 * 		<li> OperatorPassFail() </li>
	 * 		<li> DisplayTimedPrompt() </li>
	 * 		<li> GetPrompt() </li>
	 * 		<li> GetPromptBox() </li>
	 * 		<li> GetPromptPriority() </li>
	 * 		</ul>
	 * 
	 * @param promptTag Prompt to be displayed
     * 
	 * @return Result 
     * <b>Verify + promptTag == True</b>
	 * 		<ul>
	 * 		<li> Pass - Operator pressed pass button when prompted </li>
	 * 		<li> Fail - Abort or fail pressed </li>
	 * 		</ul>
     * <b>Verify + promptTag == False</b>
	 * 		<ul>
	 * 		<li> Pass </li>
	 * 		</ul>
	 */ 
    string DisplayPromptOptionalVerify(string promptTag);
	/**
	 * Test Longitudinal (front to back) axle lock
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * Determine if test should be run based on the broadcast data gathered in initialize()
     * Prompt driver to turn on Longitudinal axle lock, call DriveRollsLongitudinalLock(), 
     * call CheckWheelSpeedLongitudinalLock().  If any failures occur place all rolls in torque mode.
	 * <br><p>
	 *
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Test step description. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> ShortCircuitTestStep() </li>
	 * 		<li> IsLongitudinalLockingUndefined() </li>
	 * 		<li> DisplayPromptOptionalVerify() </li>
	 * 		<li> DriveRollsLongitudinalLock() </li>
	 * 		<li> CheckWheelSpeedLongitudinalLock() </li>
	 * 		<li> SetRollsToTorqueMode() </li>
	 * 		<li> SendTestResult() </li>
	 * 		</ul>
	 * 
	 * @return Result of testing the longitudinal axle lock
	 * 		<ul>
	 * 		<li> Pass - The longitudinal axle lock test passed. </li>
	 * 		<li> Fail - The longitudinal axle lock test failed. </li>
	 * 		</ul>
	 */ 
    string LongitudinalLockTest(void);
	/**
	 * Test Transversal (side to side) axle lock
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * Determine if test should be run based on the broadcast data gathered in initialize()
     * Prompt driver to turn on Transversal axle lock, call DriveRollsTransversalLock(), 
     * call CheckWheelSpeedTransversalLock().  If any failures occur place all rolls in torque mode.
	 * <br><p>
	 *
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Test step description. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> ShortCircuitTestStep() </li>
	 * 		<li> IsTransversalLockingUndefined() </li>
	 * 		<li> DisplayPromptOptionalVerify() </li>
	 * 		<li> DriveRollsTransversalLock() </li>
	 * 		<li> CheckWheelSpeedTransversalLock() </li>
	 * 		<li> SetRollsToTorqueMode() </li>
	 * 		<li> SendTestResult() </li>
	 * 		</ul>
	 * 
	 * @return Result of testing the longitudinal axle lock
	 * 		<ul>
	 * 		<li> Pass - The longitudinal axle lock test passed. </li>
	 * 		<li> Fail - The longitudinal axle lock test failed. </li>
	 * 		</ul>
	 */ 
    string TransversalLockTest(void);
	/**
	 * Store the flag indicating vehicle is equipped with rear and tandem longitudinal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is equipped with rear and tandem longitudinal axle 
     * lock will be stored.  This data will be used to determine which longitudinal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @param isRearAndTandemLongEquipped
	 * 		<ul>
	 * 		<li> True - Rear and Tandem axle lock equipped. </li>
	 * 		<li> False - Rear and Tandem axle lock not equipped. </li>
	 * 		</ul>
	 */
	void SetRearAndTandemLongEquipped(bool isRearAndTandemLongEquipped);
	/**
	 * Get the flag indicating vehicle is equipped with rear and tandem longitudinal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is equipped with rear and tandem longitudinal axle 
     * lock will be retrieved.  This data will be used to determine which longitudinal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @return m_isRearAndTandemLongEquipped
	 * 		<ul>
	 * 		<li> True - Rear and Tandem axle lock equipped. </li>
	 * 		<li> False - Rear and Tandem axle lock not equipped. </li>
	 * 		</ul>
	 */
    bool IsRearAndTandemLongEquipped(void);
	/**
	 * Store the flag indicating vehicle is equipped with front and rear longitudinal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is equipped with front and rear longitudinal axle 
     * lock will be stored.  This data will be used to determine which longitudinal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @param isFrontAndRearLongEquipped
	 * 		<ul>
	 * 		<li> True - front and rear axle lock equipped. </li>
	 * 		<li> False - front and rear axle lock not equipped. </li>
	 * 		</ul>
	 */
	void SetFrontAndRearLongEquipped(bool isFrontAndRearLongEquipped);
	/**
	 * Get the flag indicating vehicle is equipped with front and rear longitudinal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is equipped with front and rear longitudinal axle 
     * lock will be retrieved.  This data will be used to determine which longitudinal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @return m_isFrontAndRearLongEquipped
	 * 		<ul>
	 * 		<li> True - front and rear axle lock equipped. </li>
	 * 		<li> False - front and rear axle lock not equipped. </li>
	 * 		</ul>
	 */
    bool IsFrontAndRearLongEquipped(void);
	/**
	 * Store the flag indicating vehicle is not equipped with a longitudinal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is not equipped with longitudinal axle 
     * lock will be stored.  This data will be used to determine if the longitudinal axle lock
     * test should be run
	 * <br><p>
	 * 
	 * @param isFrontAndRearLongEquipped
	 * 		<ul>
	 * 		<li> True - longitudinal axle lock not equipped. </li>
	 * 		<li> False - longitudinal axle lock quipped. </li>
	 * 		</ul>
	 */
	void SetLongitudinalLockingUndefined(bool isLongitudinalLockingUndefined);
	/**
	 * Get the flag indicating vehicle is not equipped with a longitudinal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is not equipped with longitudinal axle 
     * lock will be retrieved.  This data will be used to determine if the longitudinal axle lock
     * test should be run
	 * <br><p>
	 * 
	 * @return m_isFrontAndRearLongEquipped
	 * 		<ul>
	 * 		<li> True - longitudinal axle lock not equipped. </li>
	 * 		<li> False - longitudinal axle lock quipped. </li>
	 * 		</ul>
	 */
    bool IsLongitudinalLockingUndefined(void);
	/**
	 * Store the flag indicating vehicle is equipped with rear transversal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is equipped with rear transversal axle 
     * lock will be stored.  This data will be used to determine which transversal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @param isRearTransEquipped
	 * 		<ul>
	 * 		<li> True - rear axle lock equipped. </li>
	 * 		<li> False - rear axle lock not equipped. </li>
	 * 		</ul>
	 */
	void SetRearTransEquipped(bool isRearTransEquipped);
	/**
	 * Get the flag indicating vehicle is equipped with rear transversal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is equipped with rear transversal axle 
     * lock will be retrieved.  This data will be used to determine which transversal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @return m_isRearTransEquipped
	 * 		<ul>
	 * 		<li> True - rear axle lock equipped. </li>
	 * 		<li> False - rear axle lock not equipped. </li>
	 * 		</ul>
	 */
    bool IsRearTransEquipped(void);
	/**
	 * Store the flag indicating vehicle is equipped with rear and tandem transversal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is equipped with rear and tandem transversal axle 
     * lock will be stored.  This data will be used to determine which transversal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @param isRearAndTandemTransEquipped
	 * 		<ul>
	 * 		<li> True - rear and tandem axle lock equipped. </li>
	 * 		<li> False - rear and tandem axle lock not equipped. </li>
	 * 		</ul>
	 */
	void SetRearAndTandemTransEquipped(bool isRearAndTandemTransEquipped);
	/**
	 * Get the flag indicating vehicle is equipped with rear and tandem transversal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is equipped with rear and tandem transversal axle 
     * lock will be retrieved.  This data will be used to determine which transversal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @return m_isRearAndTandemTransEquipped
	 * 		<ul>
	 * 		<li> True - rear and tandem axle lock equipped. </li>
	 * 		<li> False - rear and tandem axle lock not equipped. </li>
	 * 		</ul>
	 */
    bool IsRearAndTandemTransEquipped(void);
	/**
	 * Store the flag indicating vehicle is equipped with front and rear transversal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is equipped with front and rear transversal axle 
     * lock will be stored.  This data will be used to determine which transversal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @param isFrontAndRearTransEquipped
	 * 		<ul>
	 * 		<li> True - front and rear axle lock equipped. </li>
	 * 		<li> False - front and rear axle lock not equipped. </li>
	 * 		</ul>
	 */
	void SetFrontAndRearTransEquipped(bool isFrontAndRearTransEquipped);
	/**
	 * Get the flag indicating vehicle is equipped with front and rear transversal axle lock
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the vehicle is equipped with front and rear transversal axle 
     * lock will be retrieved.  This data will be used to determine which transversal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @return m_isFrontAndRearTransEquipped
	 * 		<ul>
	 * 		<li> True - front and rear axle lock equipped. </li>
	 * 		<li> False - front and rear axle lock not equipped. </li>
	 * 		</ul>
	 */
    bool IsFrontAndRearTransEquipped(void);
	/**
	 * Store the flag indicating the transversal axle lock type is undefined
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the transversal axle lock type is undefined will be stored.
     * This data will be used to determine which transversal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @param isFrontAndRearTransEquipped
	 * 		<ul>
	 * 		<li> True - axle lock type is undefined. </li>
	 * 		<li> False - axle lock type is not undefined. </li>
	 * 		</ul>
	 */
	void SetTransversalLockingUndefined(bool isTransversalLockingUndefined);
	/**
	 * Get the flag indicating the transversal axle lock type is undefined
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The flag indicating if the transversal axle lock type is undefined will be retrieved.
     * This data will be used to determine which transversal axle lock
     * test will be required
	 * <br><p>
	 * 
	 * @return isFrontAndRearTransEquipped
	 * 		<ul>
	 * 		<li> True - axle lock type is undefined. </li>
	 * 		<li> False - axle lock type is not undefined. </li>
	 * 		</ul>
	 */
    bool IsTransversalLockingUndefined(void);

	/**
	 * Finishup method
	 *
	 * @return status string
	 */
	virtual inline string FinishUp(void);
	/**
	 * Is vehicle equipped with a locking differential
	 *
	 * @return true if locking diff equipped
	 */
	const bool IsLockingDifferentialEquipped(void);
	/**
     * Abort the test sequence.
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Method used to abort the current processing based
     * on the needs of the component.<br>
     * <b><i>Note:</i></b>
     * 
     * <b>Functions Called:</b><br>
     * 		<ul>
     * 		<li> IsTandemDriveEquipped() </li>
     * 		<li> GenericTC::Abort() </li>
     * 		</ul>
     */
	void Abort(void);

private:
	/**
	 * IMotorController
	 * Used to control the motors individually
	 */
	IMotorController m_motorController;
	/**
	 * bool
	 * Used to determine if vehicle is equipped with rear and tandem longitudinal locking axles
	 */
	bool m_isRearAndTandemLongEquipped;
	/**
	 * bool
	 * Used to determine if vehicle is equipped with front and rear longitudinal locking axles
	 */
	bool m_isFrontAndRearLongEquipped;
	/**
	 * bool
	 * Used to determine if vehicle is not equipped with a longitudinal locking axle
	 */
	bool m_isLongitudinalLockingUndefined;
	/**
	 * bool
	 * Used to determine if vehicle is equipped with only a rear transversal locking axle
	 */
	bool m_isRearTransEquipped;
	/**
	 * bool
	 * Used to determine if vehicle is equipped with rear and tandem transversal locking axles
	 */
	bool m_isRearAndTandemTransEquipped;
	/**
	 * bool
	 * Used to determine if vehicle is equipped with front and rear transversal locking axles 
	 */
	bool m_isFrontAndRearTransEquipped;
	/**
	 * bool
	 * Used to determine if vehicle is not equipped with a transversal locking axle
	 */
	bool m_isTransversalLockingUndefined;
};

//*************************************************************************
#endif // TandemLockingDifferentialTC_h
