//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//      ZF ABS Component, based off Advics ABS test component.
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
#ifndef ZFABSTC_h
#define ZFABSTC_h
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
class ZFABSTC : public GenericABSTCTemplate<ModuleType>
{
public:
    /**
     * Class constructor.
     */
    ZFABSTC();
    /**
     * Class destructor.
     */
    virtual ~ZFABSTC();
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
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Perform any test component specific initialization.
     * <br><p>
     *
     * @param config Configuration file to use for initialization.
     */
    //virtual void InitializeHook(const XmlNode *config);
    /**
     * Establish Communications with the module.
     * <br><b>Category:</b> Test Step
     * <br><b>Description:</b><br>
     * Sends the 5 baud initialization sequence used to command the module to
     * enter diagnostic mode.
     *
     * @return The result placing the module into diagnostic mode.
     */
    //string PerformModuleLinkup(void);
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
     * Apply the speed ref to the rolls.
     * 
     * @param speedRef The speed reference to the rolls.
     */ 
    void SetSpeedRef(string speedRef);
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

   virtual const string TestStepDynamicParkBrake(void);
   /**
     * Analyze the forces generated during the dynamic park brake test
     * 
     * @return The status of the test step.
     */
    virtual const string AnalyzeDynamicParkBrake(void);
    /**
     * Read the brake arrays and calculate the park brake forces for each
     * of the vehicles wheels and analyze the data.
     *
     * @param brakeStart The starting sample index of the brake force array.
     * @param brakeEnd   The ending sample index of the brake force array.
     * @return The status of the operation.
     */
    virtual INT32 AnalyzeParkBrakeForces(INT32 brakeStart, INT32 brakeEnd);
    /** 
     * Validate the park brake force of each wheel and update the display
     * with the result.
     *
     * @param roller  The roller to analyze the park brake force result for.
     * @param average The average park brake force for the specified wheel.
     * @return The status of the operation.
     */
    virtual INT32 ValidateParkBrakeForce(INT32 roller, float average);
     /**
     * This sets up the GUI to prompt the driver to maintain
     * brake force in the target value.
     * 
     * @param brakeStart The starting sample of the park brake check.
     * @param brakeEnd   The ending sample of the park brake check.
     * @param tagPrefix  String added to the beginning of strings used to tag the
     *                   start and end of the force sampling interval
     * 
     * @return The status of the test.
     */
    virtual INT32 DynamicParkBrake(INT32 &brakeStart, INT32 &brakeEnd, const std::string &tagPrefix);
    /**
     * Analyze the reduction force for the specified wheel.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The wheel force array for the specified wheel will be retrieved.  The minimum data point betweent the start and
     * end indices will be located and compared against the minimum reduction allowed.  If the value falls below the
     * minimum reduction allowed, the test passes.  The results will be written to the system for display to the operator
     * as well.
     * <br><p>
     */
    virtual BEP_STATUS_TYPE AnalyzeReductionForces(INT32 roller, INT32 start, INT32 end);

    string AbsPrime(void);

    string BrakeSwitchTest(const string& position);


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
        /** Array index where the dynamic park brake test started */
    int m_dynParkBrakeStart;
    /** Array index where the dynamic park brake test ended */
    int m_dynParkBrakeStop;
    /** Park brake force for each wheel during the dynamic park brake test. */
    FORCELIST m_parkBrakeForce;

    //ModuleType m_absModule;
};

//-----------------------------------------------------------------------------
#endif  //ZFABSTC_h
