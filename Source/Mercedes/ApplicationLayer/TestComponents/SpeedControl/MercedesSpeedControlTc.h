//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the Mercedes Speed Control.
//
//===========================================================================
// Copyright (c) 2011- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef MercedesSpeedControl_h
#define MercedesSpeedControl_h
//-------------------------------------------------------------------------------------------------
#include "GenericSpeedControlTC.cpp"
//-------------------------------------------------------------------------------------------------
const float MphToKph      = 1.609344;
//-------------------------------------------------------------------------------------------------
template<class ModuleType>
class MercedesSpeedControlTc : public GenericSpeedControlTC<ModuleType>
{
public:
    MercedesSpeedControlTc();
    virtual ~MercedesSpeedControlTc();

protected:
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
};

//-------------------------------------------------------------------------------------------------
#endif //MercedesSpeedControl_h
