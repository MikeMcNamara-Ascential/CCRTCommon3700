//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Include/Bosch8TC.h 3     5/23/06 11:28a Gswope $
//
// FILE DESCRIPTION:
//		ABS test component for the Bosch8 system.
//
//===========================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Include/Bosch8TC.h $
// 
// 3     5/23/06 11:28a Gswope
// 1) Added CMABSValveFiringTest test step and associated function. this test
//  fires the rear valves first. It is NOT being used at present, as there was
//  no improvement over the previous method.
// 2) Modified default values for initial speed deltas to a realistic value
//  This allows the read to be commented out to simplify debug of valve firing
// 
// 2     5/15/06 8:32a Gswope
// fixed cascade of fail logic in ABSValveFiringTest()
// 
// 1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// 
// 3     1/30/06 3:41p Gswope
// in EvaluateSensorCross(), modified second comparisons (two places) to 
// improve quality of evaluation
// 
// 2     3/02/05 11:17a Gswope
// Emergency request from HMMA to remove rear valve firing from ESP test
// 2005.02.28 ews removed at HMMA emergency request
// changed ESP valve firing to use only front wheels in method
// ESPValveFiringTest()
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 5     11/24/04 11:03a Bmeinke
// Updated the individual ABS test methods to report inlet/outlet valve
// actuation results as required per the HMMA QMS spec
// Updated the EvaluateSensorCross() method to report front sensor cross,
// rear sensor cross and front-to-rear sensor cross results as required
// per the HMMA QMS spec
// General beautification
// 
// 3     11/15/04 9:21a Bmeinke
// Added ESPSteps_t enum to define the steps required during an ESP valve
// firing test
// Allocate the m_ESPIndex array in the constructor and delete it in the
// destructor
// Added handlers for EvaluateSensorCross, ReadSpeedDeltas and
// StopESPPumpMotor inside CommandTestStep
// Added ReadSpeedDeltas() method
// Call UpdatePrompts()/RemovePrompts() at the beginning/end of the
// ESPValveFiringTest() method
// Changed the **ESPTest() methods into state machines using the new
// ESPTestSteps_t
// Removed the local redSpeeds and recSpeeds variables inside the
// **ABSTest() methods. Use the class variable m_absSpeedDeltas instead
// Added new EvaluateSensorCross() method to perform an integrated sensor
// cross check using the speed delta values returned during ABS valve
// actuation.
// Override StopPumpMotor() and BrakeSwitchTest() to call
// UpdatePrompts()/RemovePrompts() at the beginning.end of the test step.
// Added m_initSpeedDelta data member to record the initial decel rate at
// the end of base brake. Used as a reference for the sensor cross
// evaluation
// Added m_absSpeedDeltas array to hold the decel rates during ABS valve
// actuation. Used to evaluate sensor cross.
// 
// 3     11/10/04 1:35a Bmeinke
// Added defines (RED_DELTA_IDX and REC_DELTA_IDX) for the wheel speed
// delta array indices
// Need to allocate the m_ESPIndex array in the constructor and delete it
// in the destructor
// Fixed the m_ESPIndex[LFWHEEL} typo in RFESPTest()
// Changed the integrated sensor cross algorithm to use the speed deltas
// obtained during the recovery cycles instead of the ones received during
// the reduction cycle
// 
// 2     10/27/04 8:59p Bmeinke
// Added EvaluateSensorCross test step and method.
// 
// 1     10/27/04 5:40p Bmeinke
// 
// 2     10/26/04 1:54p Bmeinke
// Fixed the names of the parameters used for the SensorQuality test
// Declutch the machine at the end of the SensorQuality test (instead of
// commanding the motors to zero speed) to avoid huge force spikes
// In LFAbsTest(), RFAbsTest(), LRAbsTest() and RRAbsTest(), we need to do
// a signed comparison of the wheel speed deltas to properly evaluate
// sensor cross
// ALWAYS tag the end of the recovery- even if testResult is not testPass
// 
// 1     8/30/04 7:24p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
//
//*************************************************************************
#ifndef Bosch8TC_h
#define Bosch8TC_h

#include "KoreaAbsTcTemplate.cpp"
#include "ModuleException.h"


/**
 * Possible steps for each wheel in the ESP test
 */
typedef enum
{
    ESP_INIT,
    ESP_PUMP_ON,
    ESP_PRIMARY_ON,
    ESP_START,
    ESP_PRIMARY_OFF,
    ESP_END,
    ESP_FINALIZE,
    ESP_DONE
} ESPSteps_t;

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

/**
 * Test component for the Bosch8 ABS.
 * This test component provides the specialized test steps
 * that are specific to the Bosch8 ABS.
 * 
 * @author Ross Wiersema
 * @see KoreaABSTCTemplate
 * @see GenericABSTCTemplate
 * @see GenericABSModuleTemplate
 * @see Bosch8Module
 * @since 15 December 2003
 */      
template <class ModuleType>
class Bosch8TC : public KoreaAbsTcTemplate<ModuleType>
{
    friend class Bosch8TCTest;       // for the tests

public:
    /**
     * Constructor
     * Nothing special to do here.
     */
    Bosch8TC();

    /**
     * Destructor
     * Nothing special to do here.
     */
    virtual ~Bosch8TC();

    /**
     * This is the method that will handle the execution of each
     * test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded
     * to execute the component tests.
     * 
     * @param value  This is the value that was commanded by
     *               the test sequencer.
     * 
     * @return The status of the overall objective.
     * @since 15 December 2003
     */
    virtual const string CommandTestStep(const string &value);

protected:

    /**
     * Enter diagnostic mode.
     *
     * @return status string
     * @note see GenericTC for status tags
     */
    virtual string EnterDiagnosticMode(void);

    /**
     * This test step checks the state of the valve and motor relays.  The
     * motor relay should be off and the valve relay should be on.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckRelayState(void);

    /**
     * This test step checks the power supply voltage.  The voltage should be
     * between 9.4V and 16V.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckSupplyVoltage(void);

    /**
     * This test step checks the state of the pump motor.  The pump motor
     * should be off.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckPumpMotor(void);

    /**
     * This test step checks the park brake signal.  First, the off state
     * will be checked.  Then we prompt the driver to apply the parking brake
     * and check the on state.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckParkBrakeSignal(void);

    /**
     * This test step checks the engine speed.  If the vehicle is at zero
     * speed, the engine speed should be zero.  If the vehicle is moving, the
     * engine speed should be non-zero.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckEngineSpeed(void);

    /**
     * This test step monitors the engine speed to detect if the ignition is off. If we get
     * no response (with cable connect) or if the engine speed falls below a certain RPM value, 
     * we will consider the ignition to be off.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string IgnitionOff();

    /**
     * This test step monitors the engine speed to detect if the ignition is on. If the
     * engine speed is above a certain RPM value, we will consider the ignition to be on.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string IgnitionOn();

    /**
     * This test step checks the throttle position.  In the idle position the
     * throttle position should be less than 4%.  At full throttle the throttle
     * position should be greater than 96%.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckThrottlePosition(void);

    /**
     * This test step checks the shift lever position sensor.  We prompt the
     * driver to shift to Park,Reverse, and Drive.  We read the shift lever
     * position is all the states and make sure they correspond.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckShiftLeverPosition(void);

    /**
     * This test step checks the ESP/TCS passive switch.  First we check the
     * switch in the on state.  Then we prompt the driver to turn the switch
     * off and check the off state.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckPassiveSwitch(void);

    /**
     * This test step checks the steering wheel angle.  The steering wheel
     * should be straight and the steering wheel angle should be between -15
     * and 15 degrees.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckSteeringWheelAngle(void);

    /**
     * This test step checks the lateral acceleration.  The vehicle should be
     * at rest and the lateral acceleration should be between -0.09g and 0.09g.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckLateralAcceleration(void);

    /**
     * This test step checks the yaw rate.  The vehicle should be at rest and
     * the yaw rate should be between -4 degrees and 4 degrees.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckYawRate(void);

    /**
     * This test step checks the pressure sensor.  With the brake pedal not
     * applied, the pressure should be between 0 and 15 bar.  With the brake
     * pedal applied, the pressure should be greater than 15 bar.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckPressureSensor(void);

    /**
     * This test step checks the AY sensor test status.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CheckAYSensorStatus(void);

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
     * Reads an initial decel value for the 4 wheel speeds. We will use this 
     * initial decel reading to check for sensor cross
     * 
     * @return The status of the operation.
     */
    virtual string ReadSpeedDeltas(void);

    /**
     * This test step runs the sensor quality test.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string SensorQualityTest(void);

    /**
     * This test step runs the ESP Valve firing test.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string ESPValveFiringTest(void);

    /**
     * This test step runs the ABS Valve firing test.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string ABSValveFiringTest(void);

    /**
     * This test step runs the ABS Valve firing test.
     * firing order changed for the CM 4WD vehicle.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */
    virtual string CMABSValveFiringTest(void);

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
     * This test step checks the Upline Test status byte (used fro evac/fill status)
     * 
     * @return The status of the operation.
     * @since 20 December 2004
     */
    virtual string CheckUplineProcessByte(void);

    /**
     * This test step checks the Upline Test status byte (used fro evac/fill status)
     * 
     * @return The status of the operation.
     * @since 20 December 2004
     */
    virtual string CheckEolByte(void);
    /**
     * This test step Determines if the ECU is the expected type
     * 
     * @return The status of the operation.
     * @since 18 Feb 2010
     */
    virtual string CheckModuleType(void);
    /**
     * This test step simply waits for control status from the module
     * to return complete
     * 
     * @return The status of the test step.
     * @since 20 August 2004
     */
    virtual string WaitForControlComplete(void);

    /**
     * Command the pump motor to run.
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This method ensures that the hydraulic pump is operating and that brake fluid is available for testing.
     * A parameter can be set to ignore the status of commanding the module in the event it is not desired to know if
     * the command was unsuccessful.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> RunPumpMotorSkipModuleValidation - Flag to indiccate if the module result should be verified for
     *                                              successful completion of the command. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> RunPumpMotor() </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - A communication failure occurred commanding the pump motor to run. </li>
     *      <li> SoftwareFailure -  A module exception occurred while attempting to command the module to run the pump motor </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameterBool() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return The result of commanding the pump motor to run.
     *      <ul>
     *      <li> Pass - The pump motor was commanded to run. </li>
     *      <li> Fail - An error occurred attempting to run the pump motor. </li>
     *      <li> Skip - The pump motor was not commanded to run. </li>
     *      <li> SoftwareFail - A module exception was thrown attempting to run the pump motor. </li>
     *      </ul>
     */
    virtual string RunPumpMotor(void);


    /**
     * Open all ABS inlet valves.
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Command the module to open all ABS inlet valves.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - Failed to command the module to open all inlet valves. </li>
     *      <li> SoftwareFailure - A module exception occurred command the module to open all inlet valves. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> AllInletValvesOn() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of commanding the module to open all inlet valves.
     *      <ul>
     *      <li> Pass - The module was successfully commanded to open all inlet valves. </li>
     *      <li> Fail - A communication failrue occurred commanding the module to open all inlet valves. </li>
     *      <li> Skip - The valves do not need to be opened.
     *      <li> SoftwareFail - The module could not be commanded to open all inlet valves. </li>
     *      </ul>
     */
    virtual string AllInletValvesOn(void);
    /**
     * Close all ABS inlet valves.
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Command the module to close all ABS inlet valves.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - Failed to command the module to close all inlet valves. </li>
     *      <li> SoftwareFailure - A module exception occurred command the module to close all inlet valves. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> AllInletValvesOff() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of commanding the module to close all inlet valves.
     *      <ul>
     *      <li> Pass - The module was successfully commanded to close all inlet valves. </li>
     *      <li> Fail - A communication failrue occurred commanding the module to close all inlet valves. </li>
     *      <li> Skip - The valves do not need to be closed.
     *      <li> SoftwareFail - The module could not be commanded to close all inlet valves. </li>
     *      </ul>
     */
    virtual string AllInletValvesOff(void);
    /**
     * This test step simply read sthe wheel speed sensors
     * 
     * @return The status of the test step.
     * @since 20 August 2004
     */
    virtual string ReadSensorSpeeds(void);
    /**
     * Command the module to begin the ABS test sequence.
     * 
     * @return Result of commanding the module to begin the ABS test sequence.
     */
    string StartAbsTest(void);
    /**
     * Analyze the brake force test data to determine if any of the ABS valves 
     * are crossed.
     * 
     * @return The status of the operation.
     * @since 16 December 2003
     */
    virtual BEP_STATUS_TYPE AnalyzeValveCross(void);

private:

    /**
     * Test the left front ESP valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string LFESPTest(void);

    /**
     * Test the right front ESP valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string RFESPTest(void);

    /**
     * Test the left rear ESP valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string LRESPTest(void);

    /**
     * Test the right rear ESP valves.
     * 
     * @return The status of the operation.
     * @since 18 December 2003
     */ 
    string RRESPTest(void);

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
     * Evaluate the speed delta data returned by the ABS module 
     * during the ABS valve firing test to see if any wheel speed
     * sensors are crossed
     * 
     * @return The status of the test step.
     */
    virtual string EvaluateSensorCross(void);

    /**
    * This method ensures that the we turn off the  hydraulic pump affter 
    * fluid is available for testing.
    *
    * @return The test result
    * @since 10 July 2003
    */
    virtual string StopPumpMotor(void);

    /**
     * Verify the brake switch is on when expected.  Ideally this step
     * will be performed immediately after the base brake testing
     * and immediately prior to performing the valve firing sequence.
     * It is expected that the brake switch will be in the "On" position
     * during this time.
     *
     */
    virtual string BrakeSwitchTest(const string& position);

    /**
     * Prompt driver to verify that warning lights are off when test is
     * complete
     */
    string WarningLight(void);

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

#endif



