//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//      Advics ABS test component.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log$
//
//*************************************************************************
#ifndef AdvicsABSTC_h
#define AdvicsABSTC_h
//-----------------------------------------------------------------------------
#include "GenericABSTCTemplate.cpp"
//-----------------------------------------------------------------------------

/**
 * Array index of the reduction speed delta data in the 
 * m_absSpeedDeltas data array
 */
#define RED_DELTA_IDX		0
/**
 * Array index of the recovery speed delta data in the 
 * m_absSpeedDeltas data array
 */
#define REC_DELTA_IDX		1


template <class ModuleType>
class AdvicsABSTC : public GenericABSTCTemplate<ModuleType>
{
public:
    /**
     * Class constructor.
     */
    AdvicsABSTC();
    /**
     * Class destructor.
     */
    virtual ~AdvicsABSTC();
    /**
     * Sequence a test step.
     * <br><b>Category:</b> Utility
     * <br><b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Test Step Functions Called:</b>
     *      <ul>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      </ul>
     *
     * @param value    This is the value that was commanded by the test sequencer.
     * @return  The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

protected:
    /**
     * Establish Communications with the module.
     * <br><b>Category:</b> Test Step
     * <br><b>Description:</b><br>
     * Sends the 5 baud initialization sequence used to command the module to
     * enter diagnostic mode.
     *
     * @return The result placing the module into diagnostic mode.
     */
    string PerformModuleLinkup(void);
    /**
     * Apply a constant torque to all rollers.
     *  
     * @param value This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */
    const string TestStepApplyTorqueToAllRollers(const std::string &value);
    /**
     * Run the low speed valve firing test.  First the rolls will be commanded 
     * to a constant speed.  Then the driver will be prompted to apply the 
     * brake.  Commands will then be sent to the module to reduce and build the 
     * brake pressure.  The motor controller will store the motor load during 
     * this time for later evaluation.
     * 
     * @param value This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */ 
    const string TestStepLowSpeedValveFiringTest(const string &value);
    /**
     * Apply zero torque to all rollers.
     *  
     * @param value This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */
    const string TestStepTorqueZero(const string &value);
    /**
     * Helper function to apply torque and send test result values.
     *  
     * @param lfTorque  Left front torque value
     * @param rfTorque  Right front torque value
     * @param lrTorque  Left rear torque value
     * @param rrTorque  Right rear torque value
     * 
     * @return The status of the test step.
     */
    const string ApplyTorqueToRollers(string lfTorque, string rfTorque, string lrTorque, string rrTorque);
    /**
     * Apply the specified torques to the rolls.
     * 
     * @param lf The left front torque value.
     * @param rf The right front torque value.
     * @param lr The left rear torque value.
     * @param rr The right rear torque value.
     */ 
    void SetMotorTorque(string lf, string rf, string lr, string rr);
    /**
     * Check if a possible runaway roll is present.
     * 
     * @param delay    The amount of time to delay before checking the rolls.
     * @param maxSpeed The maximum allowable speed.
     * 
     * @return Flag to tell if a runaway condition exists.
     */ 
    bool IsRunawayPresent(int delay, int maxSpeed);
    /**
     * Verify that the specified wheel is stopped.
     * 
     * @param wheel The wheel to check.
     * 
     * @return The result of the check.
     */ 
    const string VerifyWheelStopped(string wheel);
    /**
     * Verify the specified wheel speed is greater than the minimum speed 
     * parameter.
     * 
     * @param wheel The wheel to check.
     * 
     * @return The result of the check.
     */ 
    const string VerifyWheelSpeed(string wheel);
    /**
     * Check for a sensor cross failure.  The passed in wheel should be turning
     * and the other wheels should be stopped.
     * 
     * @param wheel The wheel to check.
     */ 
    void CheckSensorCross(string wheel);
    /**
     * Get the roller number based on the roller name.
     * 
     * @param rollerName The name of the roll.
     * 
     * @return The roll number.
     */ 
    const INT32 GetRollerNumber(string rollerName);     
    /**
     * Read the ABS wheel speed sensors from the module.
     * 
     * @return Result of reading the wheel speed sensors from the module.
     */
    string ReadSensorSpeeds(void);
    /**
     * Validate the ABS test messages.
     * 
     * @return Result of validating the ABS test messages.
     */
    string AbsTestMessages(void);
    /**
     * Validate the wheel speed sensors.
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The wheel speed sensors will be compared to the roller speeds to ensure each sensor is within the allowed
     * tolerance.  Each roller will be commanded to speed.  once the roller is at speed, the wheel speed will
     * be read from the module to ensure it is within tolerance.  Once the comparison is complete, the wheel speed
     * will be commanded to zero and the next wheel will be analyzed.
     * <br><p>
     *
     * <b>Prompts:</b>
     *      <ul>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> LowSpeedSensorTestRollerSpeed - Speed to command the rollers to for checking the sensor tolerance. </li>
     *      <li> LowSpeedSensorSpeedCheckDelay - Amount of time to wait between checks for roller speed in range. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> [Roller][TestStepName]SensorHigh - Wheel speed sensors are reading above allowed tolerance. </li>
     *      <li> [Roller][TestStepName]SensorLow - Wheel speed sensors are reading below allowed tolerance. </li>
     *      <li> [TestStepName]Fail - Wheel speed sensor test failed. </li>
     *      <li> [TestStepName]SystemCommFailure - Failure reading wheel speeds from the system. </li>
     *      <li> CommunicationFailure - Failed to read wheel speed sensors from the module. </li>
     *      <li> TargetSpeedTimeout - Operator failed to achieve the target speed range in the time allowed. </li>
     *      <li> SoftwareFailure - A module exception occurred reading wheel speed sensors. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadSensorSpeeds </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      </ul>
     *
     * @return The result of validating the wheel speed sensors.
     *      <ul>
     *      <li> Pass - All wheel speed sensors are within tolerance. </li>
     *      <li> Fail - One or more wheel speed sensors are out of tolerance or wheel speeds could not be gathered
     *                  from the vehicle or system. </li>
     *      <li> Timeout - Target speed was not achieved in the time allowed. </li>
     *      <li> Skip - The wheel speed sensors do not need to be validated. </li>
     *      <li> SoftwareFail - A module exception occurred reading the wheel speed sensors. </li>
     *      </ul>
     */
    virtual string SensorTest(void);
    /**
     * Determine the configuration of the vehicle.
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The vehicle configuration will be read from the module.  The following data items will be extracted from the
     * module and used to determine if specific test steps should be performed or skipped:
     * <ul>
     * <li> Number of channels the ABS system has.  Typically Jeeps and trucks have 3 channel systems (LF, RF, Rear).
     *      Newer jeeps and trucks are migrating to 4 channel systems (LF, RF, LR, RR). </li>
     * <li> Four-wheel drive or Two-wheel drive. </li>
     * </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> FourChannelSystemCode - Mask used to determine if the vehicle is equipped with a four channel ABS. </li>
     *      <li> FourWheelDriveCode - Mask used to determine if the vehicle is equipped with 4 wheel drive. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read the vehicle configuration from the module. </li>
     *      <li> SoftwareFailure - A module exception occurred attempting to read the vehicle configuration from the module. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadModuleConfiguration </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SetFourChannelSystem() </li>
     *      <li> SetFourWheelDrive() </li>
     *      <li> IsFourChannelSystem() </li>
     *      <li> IsFourWheelDrive() </li>
     *      <li> CompareData() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     *
     * @return Result of reading the vehicle configuration.
     *      <ul>
     *      <li> Pass - The vehicle configuration was read from the module. </li>
     *      <li> Fail - The vehicle configuration could not be read from the module. </li>
     *      <li> Skip - The vehicle configuration did not need to be read from the module. </li>
     *      <li> SoftwareFail - A module exception occurred attempting to read the vehicle configuration from the module. </li>
     *      </ul>
     */
    virtual string DetermineSystemConfiguration(void);
    /**
     * Analyze the reduction force for the specified wheel.
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The wheel force array for the specified wheel will be retrieved.  The minimum data point betweent the start and
     * end indices will be located and compared against the minimum reduction allowed.  If the value falls below the
     * minimum reduction allowed, the test passes.  The results will be written to the system for display to the operator
     * as well.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> [wheel]ABSReductionValue - System tag to write the reduction value for [wheel]. </li>
     *      <li> [wheel]ABSReductionBGColor - System tag for setting the background color for the reduction value for [wheel]. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> [wheel]MinReductionPercent - Minimum reduction percent required for [wheel]. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> [wheel]ReductionFail - Reduction percentage is not low enough for [wheel]. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetWheelForceArray() </li>
     *      <li> GetDragForceValue() </li>
     *      <li> GetBrakeForceValue() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> CreateMessage() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      </ul>
     *
     * @param roller The roller to analyze.
     * @param start  The start index for the reduction pulse.
     * @param end    The end index for the reduction pulse.
     * @return The status of checking the reduction force.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Reduction precentage was below the minimum required. </li>
     *      <li> BEP_STATUS_FAILURE - Reduction percentage was above the minimum required. </li>
     *      <li> BEP_STATUS_SKIP - Reduction forces were not sampled.
     *      </ul>
     */
    //virtual BEP_STATUS_TYPE AnalyzeReductionForces(INT32 roller, INT32 start, INT32 end);

    /**
     * Analyze the recovery force for the specified wheel.
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The wheel force array for the specified wheel will be retrieved.  The maximum data point betweent the start and
     * end indices will be located and compared against the minimum recovery required.  If the value falls above the
     * minimum recovery required, the test passes.  The results will be written to the system for display to the operator
     * as well.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> [wheel]ABSRecoveryValue - System tag to write the recovery value for [wheel]. </li>
     *      <li> [wheel]ABSRecoveryBGColor - System tag for setting the background color for the recovery value for [wheel]. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> [wheel]MinRecoveryPercent - Minimum recovery percent required for [wheel]. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> [wheel]RecoveryFail - Recovery percentage is not high enough for [wheel]. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetWheelForceArray() </li>
     *      <li> GetBrakeForceValue() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> CreateMessage() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      </ul>
     *
     * @param roller The roller to analyze.
     * @param start  The start index for the recovery pulse.
     * @param end    The end index for the recovery pulse.
     * @return The status of checking the recovery force.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Recovery precentage was below the maximum required. </li>
     *      <li> BEP_STATUS_FAILURE - Recovery percentage was above the maximum required. </li>
     *      <li> BEP_STATUS_SKIP - Recovery forces were not sampled.
     *      </ul>
     */
    //virtual BEP_STATUS_TYPE AnalyzeRecoveryForces(INT32 roller, INT32 start, INT32 end);
    /**
     * Perform the valve firing sequence for all wheels.  Overloaded - Module LF need extra delay
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The valves at each wheel will be fired in order to collect data for the reduction/recovery performance of the
     * ABS.  Data can optionally be collected during the valve firing to determine if any wheel speed sensors are crossed
     * or if any brake valves are crossed.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DeveloperTest - Flag indicating if this is a special development test sequence. </li>
     *      <li> DisableMeterForValveFiring - Flag to indicate if the force meter should be updated during the valve
     *                                        firing sequence.  The operator would notice the meter bouncing around if
     *                                        it is left enabled during valve firing. </li>
     *      <li> PerformReductionBeforeRecovery - Flag indicating what type of valve firing sequence to perform.  If
     *                                            brake force is dumped prior to the valve firing, you would need to
     *                                            perform the recovery first.  Otherwise, you would need to dump the
     *                                            current brake force prior to trying to recover the brake force. </li>
     *      <li> CollectSensorSpeedData - Flag used to indicate if wheel speed sensor data should be collected for later
     *                                    analysis of wheel speed sensors crossed. </li>
     *      <li> BrakeForceStabilizeTime - Time to wait in milliseconds for brake force to stabilize prior to firing valves. </li>
     *      <li> InterWheelGapDelay - Time in milliseconds to wait before firing the next valve in the sequence. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> ReduxRecovSequenceFail - A failure occurred during the valve firing sequence. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadSensorSpeeds </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetRollerCount() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetBrakeTestingStatus() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetParameterInt() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> DisableForceUpdates() </li>
     *      <li> EnableForceUpdates() </li>
     *      <li> TagArray() - Tag: ABSStart/ABSEnd</li>
     *      <li> IsFourChannelSystem() </li>
     *      <li> BposSleep() </li>
     *      <li> GetISpeeds() </li>
     *      <li> Reduction() </li>
     *      <li> Recovery() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of the valve firing test.
     *      <ul>
     *      <li> Pass - The valves were fired successfully. </li>
     *      <li> Fail - An error occurred firing the valves. </li>
     *      <li> Skip - The vlaves do not need to be fired. </li>
     *      </ul>
     */
    virtual string ValveFiringTest(void);
    /**
     * <b>Description:</b>
     * Exit diagnostic mode.
     *
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> Description </li>
     *     </ul>
     * 
     * <b>Faults:</b>
     *     <ul>
     *     <li> CommunicationFailure </li>
     *     <li> SoftwareFailure </li>
     *     </ul>
     * 
     * @return The status of the operation.
     * 
     * @see BepDefs.h "Standard BEP Test Status" for a list of return values.
     */
    virtual string ExitDiagnosticMode(void);
    /**
    * This test step will send a command to the motor to bring the left
    * front roll to the sensor test speed.  Once this speed is reached, it
    * will check for a front sensor cross side to side.
    * 
    * @return The status of the test step.
    * @since 16 December 2003
    */
   virtual string LFSensorTest(void);

   /**
    * This test step will send a command to the motor to bring the right
    * front roll to the sensor test speed.  Once this speed is reached, it
    * will check for a front to rear sensor cross.
    * 
    * @return The status of the test step.
    * @since 16 December 2003
    */
   virtual string RFSensorTest(void);

   /**
    * This test step will send a command to the motor to bring the left
    * rear roll to the sensor test speed.  Once this speed is reached, it
    * will check for a rear sensor cross side to side.
    * 
    * @return The status of the test step.
    * @since 16 December 2003
    */
   virtual string LRSensorTest(void);

   /**
    * This test step will send a command to the motor to bring the right
    * rear roll to the sensor test speed.
    * 
    * @return The status of the test step.
    * @since 16 December 2003
    */
   virtual string RRSensorTest(void);
    /**
     * Test the left front ABS valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string LFABSTest(void);

    /**
     * Test the right front ABS valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string RFABSTest(void);

    /**
     * Test the left rear ABS valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string LRABSTest(void);

    /**
     * Test the right rear ABS valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string RRABSTest(void);
    /**
     * Dynamic test the left front ABS valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string LFABSTest2(void);

    /**
     * Dynamic test the right front ABS valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string RFABSTest2(void);

    /**
     * Dynamic test the left rear ABS valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string LRABSTest2(void);

    /**
     * Dynamic test the right rear ABS valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string RRABSTest2(void);
    /**
     * This test step runs the ABS Valve firing test.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string ABSValveFiringTest(void);
    /**
     * This test step runs the Dynamic ABS Valve firing test.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string DynamicABSValveFiringTest(void);
    /**
     * This test step simply waits for control status from the module
     * to return complete
     * 
     * @return The status of the test step.
     * @since 20 August 2004
     */
    virtual string WaitForControlComplete(void);
    /**
     * This test step disables the diagnostic mode speed limit.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string DisableSpeedLimit(void);
    /**
     * This test step disables the diagnostic mode valve shutdown.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string DisableValveRelayShutdown(void);
    /**
     * This test step enables the diagnostic mode speed limit.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string EnableSpeedLimit(void);

     /**
     * This test step enables the diagnostic mode valve shutdown.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string EnableValveRelayShutdown(void);
    /**
     * This test step checks the state of the valve and motor relays.  The
     * motor relay should be off and the valve relay should be on.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckRelayState(void);
    /**
     * This test step checks the state of the pump motor.
     * 
     * @return The status of the operation.
     * @since 04 November 2015
     */
    virtual string CheckPumpState(void);

    virtual string BatteryVoltageCheck(void);
    /**
     * This test step unlocks module security
     *  
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string UnlockModuleSecurity(void);

    string& OriginalDriveAxle(const string *driveAxle = NULL);

    /**
    * Check part number for validity
    *
    * @return status string
    * @note see GenericTC for status tags
    */
   virtual string CheckPartNumber(void);

   string TestStepWriteVacuumFillingEndFlag(void);


    string m_originalDriveAxle;


    /**
     * Speed delta values that we read from the ABS 
     * module during the base brake test
     */
    WheelSpeeds_t       m_initSpeedDelta;

    /**
     * Speed delta values that we receive from the ABS 
     * module in its response to our valve actuation 
     * commands
     */
    WheelSpeeds_t       m_absSpeedDeltas[ RRWHEEL+1][ REC_DELTA_IDX+1];



};

//-----------------------------------------------------------------------------
#endif  //AdvicsABSTC_h
