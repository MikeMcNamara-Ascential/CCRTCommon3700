//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/TestComponents/TransferCase/Include/TcaseTool.h 3     10/30/07 3:23p Rwiersem $
//
// FILE DESCRIPTION:
//  TcaseTool.h  Generic transfer case test methods.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/TransferCase/Include/TcaseTool.h $
// 
// 3     10/30/07 3:23p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added Javadoc comments.
// 
// 2     2/26/04 6:32p Cward
// Changes for Feb 26, 2004. 
//
// 1     12/21/03 6:33p Cward
//
// 1     10/28/03 7:55a Derickso
// Created.
//
// 9     9/30/03 7:19p Bbarrett
// Change and update methods
//
// 8     9/29/03 6:03p Bbarrett
// Continue development
//
// 7     9/17/03 2:59p Bbarrett
// Add get ratio (samples / time).
//
// 6     9/11/03 3:44p Bbarrett
// Clean up code. Change JavaDoc comments.  Version 1.0 done.  Not
// compiled yet.
//
// 5     9/10/03 1:18p Bbarrett
// Add comments.
//
// 4     9/09/03 4:41p Bbarrett
// Add axle ratio methods. Not complete yet
//
// 3     8/29/03 2:58p Bbarrett
// Add more shift test. reconfigure test.  Not complete yet.
//
// 2     8/25/03 5:21p Bbarrett
// Add in ShiftToLow method.
//
// 1     8/22/03 5:31p Bbarrett
// Initial work on Tcase
//
//
//*************************************************************************
#ifndef TcaseTool_h
#define TcaseTool_h
//*************************************************************************

#include "GenericTC.h"
#include "BepDefs.h"
#include <strstream.h>
#include <math.h>

using std::string;
/**
 * Transfer Case test component tool kit.  This test component will test Manual 
 * Transfer Case.  The Tcase test Steps are performed in 3 stages:
 * <ol>
 * <li> Preconditions - checks to ensure the following items are in the correct state/range:
 *      <ul>
 *      <li> vehicle speed </li>
 *      <li> transmission gear </li> 
 *      <li> Engine On/Off </li>
 *      <li> Key On/Off </li>
 *      <li> Throttle Open/Closed </li>  
 *      </ul>
 * <li> Shift Tcase - shift the Tcase to the desired gear.  
 * <li> Verify Shift - checks the Tcase shift was ok.  
 * </ol>
 * The test paramters can be set to "Skip" vehicle conditions and "Skip the Driver verification.
 *
 * @author Bill Barrett
 * @since August 22, 2003
 * @see GenericTC
 */
class TcaseTool
{
public:
    /** 
     * Class constructor. 
     * 
     * @param tc Pointer to the test component that instantiated this object.
     */
    TcaseTool(GenericTC *tc);

    /** 
     * Class Destructor.
     */
    virtual ~TcaseTool();

    /**
     * Validate a range shift in the transfer case.
     * <p><b>Description</b><br>
     * Checks for vehicle conditions, shifts Tcase to desired postion, and verifies Tcase shifted to the desired range.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> <TestStepName>VerifyMethod - Method of verifying the t-case shifted to the desired range. </li>
     *      </ul>
     *
     * <b>Data Tags:</b>
     *   <ul>
     *      <li> Transmission - System tag to use to retrieve the transmission type from the vehicle build data. </li>
     *      <li> ManualTransType - Value that represents the vehicle is equipped with a manual transmission. </li>
     *   </ul>
     * 
     * <b>Prompt Tags:</b>
     *   <ul>
     *      <li> ApplyClutch - Apply the clutch pedal. </li>
     *   </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> <TestStepName>VerifyShiftFailure - Transfercase did not shift to the desired range. </li>
     *      <li> <TestStepName>ShiftTcaseFailure - Transfercase failed to shift. </li>
     *      <li> <TestStepName>VehicleConditionsFailure - Vehicle is not in the correct state to perform the shift. </li>
     *      </ul>
     * 
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameter() </li>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> PreconditionsForShift() </li>
     *      <li> ShiftTcase() </li>
     *      <li> VerifyTcaseShift() </li>
     *      </ul>
     * 
     * @return Result of validating the transfercase shift to the desired range.
     */
    virtual string CheckTcaseShift ( void );

    /**
     * Verify vehicle is in the correct state to perform the range shift.
     * <p><b>Description</b><br>
     * The vehicle speed will be checked to ensure the vehicle is in the correct speed range to perform the range shift.
     * The engine running and ignition states will also be checked to ensure they are in the correct state.  The 
     * throttle position will be checked to ensure it is in the proper range and the transmission will be checked to
     * ensure it is in the correct gear to perform the range shift.  If any of the conditions are not correct, the
     * operator will be instructed to achieve the desired condition.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> MinimumShiftSpeed - Minimum vehicle speed required to shift the transfer case. </li>
     *      <li> <TestStepName>EngineRunning - Engine running state required to perform the requested range shift. </li>
     *      <li> <TestStepName>KeyPosition - Required key position in order to perform the requested range shift. </li>
     *      <li> <TestStepName>TransmissionGear - Transmission gear required for performing the range shift. </li>
     *      <li> <TestStepName>ThrottlePosition - Required throttle position for performing the requested range shift. </li>
     *      </ul>
     *
     * <b>Prompt Tagss:</b>
     *   <ul>
     *      <li> TcaseTesting - Testing the transfercase. </li>
     *   </ul>
     * 
     * <b>Test Component Functions:</b>
     *   <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameter() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *   </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckVehicleSpeed() </li>
     *      <li> EngineOnOff() </li>
     *      <li> TurnKeyOnOff() </li>
     *      <li> ShiftTransmission() </li>
     *      <li> CheckThrottlePosition() </li>
     *      </ul>
     * 
     * @return Result of validating the vehicle is in the correct state to perform the range shift.
     */
    virtual string PreconditionsForShift ( void );

    /**
     * Verify vehicle speed is in the correct range.
     * <p><b>Description</b><br>
     * If the vehicle speed is not in the correct range, the operator will be prompted to achieve the correct speed
     * range in order to successfully perform the requested range shift.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> MinimumSpeed - Minimum vehicle speed required to perform the desired range shift. </li>
     *      <li> MaximumSpeed - Maximum vehicle speed allowed to perform the desired range shift. </li>
     *      <li> SpeedSampleDelay - Amount of time(ms) between vehicle speed checks. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *   <ul>
     *      <li> <TestStepName>MinimumShiftSpeed - Minimum vehicle speed required to perform the range shift. </li>
     *   </ul>
     * 
     * <b>Test Component Functions:</b>
     *   <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> WaitAndPromptForSpeed() </li>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> GetTestStepInfoInt() </li>
     *   </ul>
     * 
     * @return Result of validating the vehicle speed is in the correct range.
     */
    virtual string CheckVehicleSpeed ( void );

    /**
     * Prompt the operator to shift the transmission to the desired gear.
     * <p><b>Description</b><br>
     * The operator will be prompted to shift the transmission to the required gear.  If the brake pedal needs to be 
     * applied, a prompt will be displayed for the operator.
     * <br><b><i>Note:</i></b> The transmission gear will not be checked. The prompt will only be displayed for a set
     * amount of time.
     * <p>
     * <b>Test Step Info:</b>
     *   <ul>
     *      <li> TransmissionMessageDelay - Amount of time(ms) to display the transmission shift prompt. </li>
     *   </ul>
     * 
     * <b>Test Component Parameters:</b>
     *   <ul>
     *      <li> <TestStepName>ApplyBrakeShiftTransmission - Flag indicating if the brake pedal should be applied when
     *                                                       shifting the transmission. </li>
     *   </ul>
     * 
     * <b>Prompt Tags:</b>
     *   <ul>
     *      <li> <TestStepName>ShiftTransmission - Shift the transmission to the specified gear. </li>
     *      <li> ApplyBrakePedal - Apply the brake pedal. </li>
     *      </ul>
     * 
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetParameterBool() </li>
     *      <li> StatusCheck() </li>
     *   </ul>
     * 
     * @param transGear This parameter is currently unused.
     *
     * @return string: testPass.
     */
    virtual string ShiftTransmission ( const string transGear );

    /**
     * Ensure the ignition is in the correct state.
     * <p><b>Description</b><br>
     * The operator will be prompted to place the ignition in the correct position.
     * <br><b><i>Note:</i></b> The actual position will not be checked.  The prompt will only be displayed for a 
     * short amount of time.
     * <p>
     * <b>Test Step Info:</b>
     *   <ul>
     *      <li> KeyOnOffMessageDelay - Amount of time (ms) to display the ignition prompt. </li>
     *   </ul>
     * 
     * <b>Prompts:</b>
     *   <ul>
     *      <li> <TestStepName>KeyOnOff - Place the ignition in the specified position. </li>
     *   </ul>
     * 
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> DisplayTimedPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      </ul>
     *
     * @param keyPosition Position in which to place the ignition.
     *
     * @return Result of instructing the operator to place the ignition in the specified position.
     */
    virtual string TurnKeyOnOff ( const string keyPosition );

    /**
     * Ensure the engine is in the correct run state.
     * <p><b>Description</b><br>
     * The operator will be instructed to place the engine in the correct run state.
     * <br><b><i>Note:</i></b> The actual run state of the engine will not be checked.  The prompt will only be displayed
     * for a set amount of time.
     * <p>
     * <b>Test Step Info:</b>
     *   <ul>
     *      <li> EngineOnOffMessageDelay - Amount of time(ms) to display the sngine state prompt. </li>
     *   </ul>
     * 
     * <b>Prompt Tags:</b>
     *   <ul>
     *      <li> <TestStepName>EngineOnOff - Place the engine in the specified run state. </li>
     *   </ul>
     * 
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      </ul>
     *
     * @param engineRun Set engine state to "On"/"Off".
     *
     * @return Result of placing the engine in the specified run state.
     */
    virtual string EngineOnOff ( const string engineRun );

    /**
     * Ensure the throttle is in the correct position.
     * <p><b>Description</b><br>
     * The operator will be instructed to place the throttle in the specified position.
     * <br><b><i>Note:</i></b> The actual throttle position will not be checked.  The prompt will only be displayed
     * for a set amount of time.
     * <p>
     * <b>Test Step Info:</b>
     *   <ul>
     *      <li> ThrottleMessageDelay - Amount of time(ms) to display the throttle prompt. </li>
     *   </ul>
     * 
     * <b>Prompt Tags:</b>
     *   <ul>
     *      <li> <TestStepName>ThrottlePosition - Apply the specified amount of throttle. </li>
     *   </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      </ul>
     *
     * @param throttlePosition Throttle position to be achieved - "Open"/"Closed".
     *
     * @return Result of prompting the operator to place the throttle in the specified position.
     */
    virtual string CheckThrottlePosition ( const string throttlePosition );

    /**
     * Instruct the operator to perform the range shift.
     * <p><b>Description</b><br>
     * Prompt the operator to perform the desired range shift.
     * <br><b><i>Note:</i></b> The actual transfercase position will not be checked.  The prompt will only be displayed
     * for a set amount of time.
     * <p>
     * <b>Test Step Info:</b>
     *   <ul>
     *      <li> CommandMessageDelay - Amount of time(ms) to display the prompt. </li>
     *   </ul>
     * 
     * <b>Prompt Tags:</b>
     *   <ul>
     *      <li> <TestStepName>ShiftTcase - Shift the t-case to the specified position. </li>
     *   </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      </ul>
     * 
     * @return Result of prompting the operator to shift the transfercase.
     */
    virtual string ShiftTcase ( void );

    /**
     * Verify the transfercase range shift was successful.
     * <p><b>Description</b><br>
     * The current transfercase gear will be checked to ensure the rnage shift compelted successfully.
     * <p>
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseCheckDriver() </li>
     *      <li> TcasePromptDriver() </li>
     *      </ul>
     *
     * @param verifyMethod
     *               Verification method to use to validate the transfercase rnage shift completed successfully.
     * 
     * @return Result of validating the range shift completed successfully.
     */
    virtual string VerifyTcaseShift ( const string &verifyMethod );

    /**
     * Prompt the operator to enter the result of the rnage shift.
     * <p><b>Description</b><br>
     * The operator will be prompted to enter the result of the transfercase range shift.  
     * <p>Test Step Info:</b>
     *      <ul>
     *      <li> TcaseGear - Gear to shift the transfercase to. </li>
     *      </ul>
     *
     * <b>Prompt Tags:</b>
     *   <ul>
     *      <li> <TestStepName>ShiftTcase - Shift to the specified gear. </li>
     *   </ul>
     * 
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> OperatorYesNo() </li>
     *      </ul>
     *
     * @return Result of shifting the transfercase to the specified gear.
     */
    virtual string TcaseCheckDriver( void );

    /**
     * Prompt the operator to shift the transfercase.
     * <p><b>Description</b><br>
     * The operator will be instructed to shift the transfercase to the specified gear range.
     * <br><b><i>Note:</i></b> No verification will be performed to ensure the shift completed uccessfully.  A prompt
     * will only be displayed for a set amount of time.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> TcaseMessageDelay - Amount of time(ms) to display the shift prompt. </li>
     *      </ul>
     *
     * <b>Prompt Tags:</b>
     *   <ul>
     *      <li> <TestStepName>ShiftTcase - Shift the transfercase to the specified gear. </li>
     *   </ul>
     * 
     * <b>Test Component Functions:</b>
     *   <ul>
     *      <li> GetTestStepName() </li>
     *      <li> DisplayTimedPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *   </ul>
     * 
     * @return Result of prompting the operator to shift the transfercase.
     */
    virtual string TcasePromptDriver ( void );

    /**
     * Verify the front to rear axle ratio is in range.
     * <p><b>Description</b><br>
     * The operator will be prompted to accelerate to the specified speed range.  Once the vehicle speed is in the 
     * proper range, the front and rear axle speeds will be sampled.  The front to rear axle ratio will be calculated 
     * from the speed samples collected.  If the axle ratio is in an acceptable rage, the test will pass; otherwise, 
     * the oerator may be given multiple attempts to pas the test based on configuration settings.
     * <p>
     * <b>Test Step Info:</b>
     *   <ul>
     *      <li> MinimumSpeed - Minimum required vehicle speed for performing the axle ratio test. </li>
     *      <li> MaximumSpeed - Maximum allowed speed for performing the axle ratio test. </li>
     *      <li> SpeedSampleDelay - Amount of time(ms) to wait between vehicle speed samples. </li>
     *   </ul>
     * 
     * <b>Test Component Parameters:</b>
     *   <ul>
     *      <li> AxleRatioDirection - Vehicle speed direction in which to perform the axle ratio test. </li>
     *      <li> AxleRatioMinimum - Minimum axle ratio limit. </li>
     *      <li> AxleRatioMaximum - Maximum axle ratio limit. </li>
     *      <li> MaximumAxleRatioAttempts - Maximum number of attempts to pass the axle ratio test.</li>
     *   </ul>
     * 
     * <b>Data Tags:</b>
     *   <ul>
     *      <li> TargetSpeed - System tag to use for displaying the target speed range for the operator. </li>
     *   </ul>
     * 
     * <b>Prompt Tagss:</b>
     *   <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> ShiftToReverse - Shift the transmission to Reverse. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      <li> MaintainSpeed - Maintain the current speed. </li>
     *   </ul>
     * 
     * <b>Fault Tags:</b>
     *   <ul>
     *      <li> FailRatio - Axle ratio is out of limits. </li>
     *      <li> ReadRatio - Could not get the axle ratio. </li>
     *      <li> TargetSpeedTimeout - Timeout waiting for the operator to accelerate to the target speed. </li>
     *      <li> DisengageMotor - Could not disengage the motors. </li>
     *      <li> ZeroSpeed - Timeout wiating for zero speed. </li>
     *   </ul>
     * 
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> WaitForTargetSpeed() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> Disengage() </li>
     *      <li> Engage() </li>
     *      <li> GetAxleRatio() </li>
     *      </ul>
     * 
     * @return Result of validating the axle ratio is in acceptable limits.
     */
    virtual string CheckAxleRatio ( void );

    /**
     * Verify all wheels are coming up to speed within tolerance.
     * <p><b>Description</b><br>
     * The operator will be prompted to accelerate to th target speed.  As the vehicle is accelerating, the wheel speeds
     * will be sampled.  For each speed sample, the following data will be calculated:
     * <ul>
     * <li> Front to rear speed delta </li>
     * <li> Front left to right speed delta </li>
     * <li> Rear left to right speed delta </li>
     * </ul>
     * If any of the speed delta values are out of tolerance, the test will fail.  The test may be aborted if the
     * test fails based on parameter settings.
     * <p>
     * <b>Test Step Info:</b>
     *   <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between axle speed checks. </li>
     *      <li> AbortOnFail - Flag indicating the test sequence should abort if the axle speed delta is out of 
     *                         tolerance. </li>
     *   </ul>
     * 
     * <b>Test Component Parameters:</b>
     *   <ul>
     *      <li> <TestStepName>MaxFrontToRearDelta - Maximum front to rear speed delta allowed. </li>
     *      <li> <TestStepName>MaxLeftToRightDelta - Maximum left to right speed delta allowed. </li>
     *      <li> <TestStepName>TargetSpeed - Target speed for the axle balance test. </li>
     *      <li> <TestStepName>MinimumSpeed - Minimum vehicle speed required for the axle balance test. </li>
     *      <li> <TestStepName>MaximumSpeed - Maximum vehicle speed allowed for the axle balance test. </li>
     *   </ul>
     * 
     * <b>Data Tags:</b>
     *   <ul>
     *      <li> SpeedTarget - System tag to use to display the target speed range for the operator. </li>
     *      <li> DriveAxle - System tag denoting the current drive axle of the vehicle. </li>
     *      <li> Rear - Value that indicates the current vehicle is rear wheel drive. </li>
     *   </ul>
     * 
     * <b>Prompt Tags:</b>
     *   <ul>
     *      <li> ShiftToDrive - Shift the transmission into Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *   </ul>
     * 
     * <b>Fault Tags:</b>
     *   <ul>
     *      <li> SystemStatus - System status precludes further testing. </li>
     *      <li> SystemReadFail - Could not read data from the system. </li>
     *      <li> <TestStepName>Timeout - Timeout waiting for the operator to accelerate to the target speed. </li>
     *      <li> <TestStepName>Fail - Axle speed delta out of tolerance. </li>
     *      <li> <TestStepName>Abort - Axle speed delta out of tolerance, aborting the test sequence. </li>
     *   </ul>
     * 
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetTestStepInfoBool() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameterInt() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * @return Result of validating the axle speed delta is within tolerance.
     */
    virtual string CheckAxleBalance( void );
    virtual string WaitForAxleBalance( void );

    /**
     * Calculate the axle ratio.
     * <p><b>Description</b><br>
     * The wheel speed data will be sampled for a period of time.  The wheel speed data will then be used to
     * calculate the front to rear axle speed ratio.
     * <p>
     * <b>Test Step Info:</b>
     *   <ul>
     *      <li> RatioSamples - Number of wheel speed samples to use for analyzing the axle speed ratio. </li>
     *      <li> SampleTime - Amount of time to collect the wheel speed data. </li>
     *   </ul>
     * 
     * <b>Data Tags:</b>
     *   <ul>
     *      <li> IcmSpeedArray - Array to use for the wheel speed samples. </li>
     *      <li> DriveAxle - Drive axle of the current vehicle. </li>
     *      <li> Rear - Value to use to determine if the current vehicle is rear wheel drive. </li>
     *   </ul>
     * 
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetTestStepName() </li>
     *      <li> TagArray() </li>
     *      <li> ReadDataArrays() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      </ul>
     *
     * @param ratio Axle ratio calculated from the wheel speed data.
     *
     * @return Result of calculating the axle speed ratio.
     */
    virtual string GetAxleRatio( float &ratio );

    /**
     * Disengage the machine.
     * <p><b>Description</b><br>
     * The motors will be placed in torque zero mode and the clutches will be disengaged.  This essentially sets the
     * machine to a free roll mode.
     * <p>
     * <b>Test Component Functions:</b>
     *   <ul>
     *      <li> Disengage() </li>
     *   </ul>
     * 
     * @return Status of disengaging the machine.
     */
    INT32 Disengage(void);

    /**
     * Engage the machine.
     * <p><b>Description</b><br>
     * The clutches will be engaged and the motors set to Boost mode.
     * <p>
     * <b>Test Component Functions:</b>
     *   <ul>
     *      <li> Engage() </li>
     *   </ul>
     * 
     * @return Status of engaging the machine.
     */
    INT32 Engage(void);

    /**
     * Determine if the current test step should be performed or skipped.
     * <p><b>Description:</b><br>
     * The overall test result and parameters will be analyzed to determine if the current test step should be skipped.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> <TestStepName>SkipIfPreviousTCaseFail - Flag indicating if the current test step should be skipped
     *                                                   if the t-case test failed previously. </li>
     *      </ul>
     * 
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> GetParameterBool() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetOverallResult() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      </ul>
     *
     * @return Flag indicating if the current test step should be skipped.
     */
    virtual bool ShortCircuitTestStep(void);

    /**
     * Check with operator if Tcase is functional.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Generic step used to get pass or fail from the vehicle operator.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     * 		<ul>
     * 		<li> None. 
     * 		</ul>
     * 
     * <b>Data Tags:</b><br>
     * 		<ul>
     * 		<li> (testStepName  + "Prompt") - Prompt name. </li>
     * 		</ul>
     * 
     * <b>Test Step Info:</b>
     * 		<ul>
     * 		<li> GetTestStepInfo("Description") - Description of the test step. </li>
     *		<li> GetTestStepInfoInt("Timeout") - How long to wait for operator input. </li>
     * 		</ul>
     * 
     * <b>Fault Codes:</b>
     * 		<ul>
     * 		<li> None </li>
     * 		</ul>
     *
     * <b>Prompts:</b>
     * 		<ul>
     * 		<li> (testStepName  + "Prompt") - Instruct operator to press Pass or Fail test. </li>
     * 		</ul>
     * 
     * <b>Functions Called:</b>
     * 		<ul>
     * 		<li> Log() </li>
     * 		<li> ShortCircuitTestStep() </li>
     * 		<li> GetComponentName() </li>
     * 		<li> GetTestStepName() </li>
     * 		<li> c_str() </li>
     * 		<li> OperatorPassFail() </li>
     * 		<li> GetDataTag() </li>
     * 		<li> GetTestStepInfoInt() </li>
     * 		<li> SendTestResult() </li>
     * 		<li> GetTestStepInfo() </li>
     * 		</ul>
     * 
     * @return Result of starting the switch monitor.
     * 		<ul>
     * 		<li> Pass - The module interface successfully started monitoring switches. </li>
     * 		<li> Fail - Switch monitor could not be started. </li>
     * 		<li> Skip - The switches do not need to be verified. </li>
     * 		</ul>
     */
    virtual string GetTcasePassFail(void);

    /**
     * Coast to stop.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Generic step for costing to stop. Displays speed target green band, prompts operator to coast
     * down to speed, and checks the roller speeds to verify speed has been reached. Return pass or fail.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     * 		<ul>
     * 		<li> CoastToStopSpeed - The desired test speed float value. </li>
     * 		<li> CoastToStopAllowBraking - Allow using brake to completely stop vehicle bool value. </li>
     * 		</ul>
     *
     * <b>Data Tags:</b><br>
     * 		<ul>
     * 		<li> SpeedTarget - Prompt name. </li>
     * 		</ul>
     * 
     * <b>Test Step Info:</b>
     * 		<ul>
     * 		<li> GetTestStepInfo("Description") - Description of the test step. </li>
     *		<li> GetTestStepInfoInt("ScanDelay") - How long to wait between wheel speed checks. </li>
     * 		</ul>
     * 
     * <b>Fault Codes:</b>
     * 		<ul>
     * 		<li> None - returns pass or fail. </li>
     * 		</ul>
     * 
     * <b>Prompts:</b>
     * 		<ul>
     * 		<li> CoastToStop - Instruct operator to coast to stop. </li>
     * 		</ul>
     * 
     * <b>Functions Called:</b>
     * 		<ul>
     * 		<li> Log() </li>
     * 		<li> ShortCircuitTestStep() </li>
     * 		<li> GetParameterFloat() </li>
     * 		<li> CreateMessage() </li>
     * 		<li> GetWheelCount() </li>
     * 		<li> SystemWrite() </li>
     * 		<li> GetDataTag() </li>
     * 		<li> SendTestResult() </li>
     * 		<li> DisplayPrompt() </li>
     * 		<li> GetPromptBox() </li>
     * 		<li> GetPrompt() </li>
     * 		<li> GetTestStepInfoInt() </li>
     * 		<li> GetWheelSpeeds() </li>
     * 		<li> abs() </li>
     * 		<li> BposSleep() </li>
     * 		<li> GetTestStepName() </li>
     * 		<li> ConvertStatusToResponse() </li>
     * 		<li> c_str() </li>
     * 		<li> TimeRemaining() </li>
     * 		<li> StatusCheck() </li>
     * 		<li> RemovePrompt() </li>
     * 		<li> ReadSubscribeData() </li>
     * 		<li> GetParameterBool() </li>
     * 		<li> SendTestResult() </li>
     * 		<li> GetTestStepInfo() </li>
     * 		</ul>
     * 
     * @return Result vehicle reached coast down speed.
     * 		<ul>
     * 		<li> Pass - The module interface successfully started monitoring switches. </li>
     * 		<li> Fail - Switch monitor could not be started. </li>
     * 		<li> Skip - The switches do not need to be verified. </li>
     * 		</ul>
     */
    virtual string CoastToStop(void);

    /**
     * Accelerate to test speed.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Generic step used to get vehicle to desired speed. Displays speed target green band, prompts operator 
     * to speed, and check the roller speeds to verify speed has been reached. Return pass or fail.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     * 		<ul>
     * 		<li> (GetTestStepName()+ "TargetSpeed")- The desired test speed float value. </li>
     * 		<li> (GetTestStepName() + "TargetSpeedRange") - The green band speed range to be displayed. </li>
     * 		</ul>
     * 
     * <b>Test Step Info:</b>
     * 		<ul>
     * 		<li> GetTestStepInfo("Description") - Description of the test step. </li>
     *		<li> GetTestStepInfoInt("ScanDelay") - How long to wait between wheel speed checks. </li>
     * 		</ul>
     * 
     * <b>Fault Codes:</b>
     * 		<ul>
     * 		<li> None - returns pass or fail. </li>
     * 		</ul>
     * 
     * <b>Prompts:</b>
     * 		<ul>
     * 		<li> GetTestStepName()+"TargetSpeed") - Instruct operator to accelerate to speed. </li>
     * 		</ul>
     * 
     * <b>Functions Called:</b>
     * 		<ul>
     * 		<li> Log() </li>
     * 		<li> ShortCircuitTestStep() </li>
     * 		<li> AccelerateToTestSpeed() </li>
     * 		<li> GetParameterFloat() </li>
     * 		<li> GetTestStepName() </li>
     * 		<li> GetParameter() </li>
     * 		<li> GetTestStepInfoInt() </li>
     * 		<li> SendTestResult() </li>
     * 		<li> c_str() </li>
     * 		<li> GetTestStepInfo() </li>
     * 		</ul>
     * 
     * @return Result vehicle reached target speed.
     * 		<ul>
     * 		<li> Pass - The module interface successfully started monitoring switches. </li>
     * 		<li> Fail - Switch monitor could not be started. </li>
     * 		<li> Skip - The switches do not need to be verified. </li>
     * 		</ul>
     */
    virtual string TcaseAccelerateToSpeed(void);

protected:
    /**
     * Wheel array that is used to store the force information from an axle 
     * ratio test.
     */
    WHEELDATAARRAY m_wheelSpeedArray;

    /**
     * The test component pointer that will be used for logging and other 
     * methods that are in test componenet that might be needed.
     */
    GenericTC *m_component;
};

#endif  //TcaseTool_h
