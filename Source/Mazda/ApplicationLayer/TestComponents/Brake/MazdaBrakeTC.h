//*************************************************************************
// Description:
// This file contains the functions necessary to test the vehicle.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef MazdaBrakeTC_h
#define MazdaBrakeTC_h
//-------------------------------------------------------------------------------------------------
#include "GenericBaseBrakeTC.h"
#include <list>
//-------------------------------------------------------------------------------------------------
class MazdaBrakeTC : public GenericBaseBrakeTC
{
public:
    MazdaBrakeTC();
    virtual ~MazdaBrakeTC();
    /**
     * Sequence a test step.
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
     *      <li> AccelerateVehicleToSpeed() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetVehicleParameter() </li>
     *      <li> GenericBaseBrakeTC::CommandTestStep() </li>
     *      <li> SetBrakeTestingStatus() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @param value    This is the value that was commanded by the test sequencer.
     * @return  The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

    /**
     * Boost all wheels to get the vehicle to the desired speed.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This test step will use the motors to boost all wheels to the specified target speed.
     * In certain applications, such as a mining application, the max speed limit of the vehicle may not allow
     * the vehicle to obtain normal brake test arm speed on its own.  This test step will ensure the wheel speed
     * reaches the necessary speed to properly perform the brake test.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *       <ul>
     *       <li> Description - Description of the test step </li>
     *       </ul>
     *
     * <b>Data Tags:</b>
     *       <ul>
     *       <li> EngageEbsMotors </li>
     *       </ul>
     * 
     * <b>Prompts:</b>
     *       <ul>
     *       <li> NeutralPrompt - Shift transmission to Neutral </li>
     *       <li> RemoveFootFromBrake - Remove foot from brake pedal </li>
     *       </ul>
     *
     * <b>Functions Called:</b>
     *       <ul>
     *       <li> BposSleep() </li>
     *       <li> ConvertStatusToResponse() </li>
     *       <li> DisplayPrompt() </li>
     *       <li> DisplayTimedPrompt() </li>
     *       <li> GetDataTag() </li>
     *       <li> GetPrompt() </li>
     *       <li> GetPromptBox() </li>
     *       <li> GetPromptDuration() </li>
     *       <li> GetPromptPriority() </li>
     *       <li> GetRollSpeed() </li>
     *       <li> GetTestStepInfo() </li>
     *       <li> RemovePrompt() </li>
     *       <li> SendTestResult() </li>
     *       <li> ShortCircuitTestStep() </li>
     *       <li> SystemWrite() </li>
     *       <li> TimeRemaining() </li>
     *       </ul>
     *
     * @param targetSpeed
     *               Speed to command the rollers to achieve.
     * @param startAtZeroSpeed
     *               Flag indicating if the operator must achieve zero speed before commanding the rollers to speed.
     * @param disengageAtSpeed
     *               Flag indicating if the rollers should be disengaged once the target speed is obtained.
     *
     * @return Result of boosting all wheels to the target speed.
     */
    virtual string AccelerateVehicleToSpeed(const float targetSpeed, const bool startAtZeroSpeed, const bool disengageAtSpeed);

  
    /**
     * Check the brake forces on each axle
     * 
     * @param axle the axle that is being checked (either "Front" or "Rear")
     * 
     * @return Result of the force test
     */
    string MazdaBrakeForceTest(string axle);

    /**
     * Check the brake forces on each axle
     * 
     * @param axle the axle that is being checked ("Front", "Rear", or "ParkBrake")
     * 
     * @return Result of the force test
     */
    string MazdaAnalyzeBrakeForce(string axle);

    /**
     * Check the brake forces and make sure they are in the specified ranges
     * 
     * @param axle
     *          the axle that is being tested ("Front", "Rear", or "ParkBrake")
     * @param rightWheelAvg
     *          the average force of the right wheel
     * @param leftWheelAvg
     *          the average force of the left wheel
     * 
     * @return status of the operation
     */ 
    string AnalyzeBrakeTestResults(string axle, float rightWheelAvg, float leftWheelAvg);

    /** 
     * Analyze the drage forces from the drag test
     * 
     * return Result of the analysis
     */
    string AnalyzeMazdaDragTest(void);

    /**
     * Do the comparisons and the checking of parameters in this function rather than in the parent function
     * 
     * @param string axle - The axle that is being measured, either "Front" or "Rear"
     * @param float rightWheelAvg - the average value for the right wheel
     * @param float leftWheelAvg - the average value for the left wheel
     */
    string AnalyzeDragTestResults(string axle, float rightWheelAvg, float leftWheelAvg);
    
    /**
     * Returns true if the machine is in full auto mode
     * Returns false if the machine is in half-auto mode
     */
    bool FullAutoMode(void);

    /**
     * Do the startup stuff
     */
    string InitializeBrake(void);

    /**
     * Do the standard finish up things
     */
    string FinishUp(void);

    /**
     * Check the drag on the brakes.
     * 
     * @return Result of the drag test.
     */
    string MazdaDragTest(void);

    /**
     * Each wheel can be commanded a speed and the function will return a value once every wheelspeed is obtained or it timesout.
     * 
     * Car should be shifted to N before this step runs
     * 
     * @param FLSpeed - wheelspeed
     * @param FRSpeed - wheelspeed
     * @param RLSpeed - wheelspeed
     * @param RRSpeed - wheelspeed
     * @param timeout - the maximum time that the function will check for the wheelspeeds
     * 
     * @return bep status values (BEP_STATUS_SUCCESS for pass)
     */
    BEP_STATUS_TYPE WaitForWheelSpeedsToBeReached(float FLSpeed, float FRSpeed, float RLSpeed, float RRSpeed, int timeout);

    /**
     * Put up prompts to have the driver place the vehicle into N and remove foot from brake pedal.
     * 
     * This function can be used whenever an acceleration event is desired.
     */
    void DisplayAccelPrompts(void);

    /**
     * Reset the motor mode and speed values on the machine.
     * 
     */
    void ZeroMotorSpeedandTorque(void);

    /**
     * Check to see if wheel has reached a specified speed
     * 
     * @param wheelTag - the wheel to check
     * @param targetSpeed - the speed that should be reached
     * @param tolerance - the percent tolerance that the wheelspeed needs to reach to be in the correct speed band
     * 
     * @return BEP_STATUS_TYPE
     */ 
    bool IsWheelInSpeedRange(string wheelTag, const float targetSpeed, float tolerance);

    /**
     * Run the vehicle for a specified distance
     * 
     */
    string DistanceValidationTest(void);

    /**
     * Check the brake stopping distance.
     * 
     * @return Result of the stopping distance test.
     */
    string BrakeDistanceTest(void);

    /**
     * Measure the maximum brake force that is generated by the vehicle.
     * 
     * @return Result of measuring the maximum brake force.
     */
    string MaxBrakeForceVerification(void);

    /**
     * Check the wheelspeed sensors
     * 
     * @return Result of the wheel speed sensors
     */
    string SpeedSensorCheck(void);

    /**
     * Run the cutomer defined wheel speed check test
     * 
     * @return Result of the test
     */
    string StartSystemSpeedCheck(void);

    /**
     * Check if the vehicle is equipped with ABS
     * 
     * @return True - if ABS is present
     *         False - if ABS is not present
     */
    bool EquippedABS(void);

    /**
     * Calibrate the max brake force readings.
     * 
     * @return Result of calibrating the max brake force readings.
     */
    string MaxBrakeForceCalibration(void);

    /**
     * Measure the maximum park brake force and ensure it is within the parameter limits.
     * 
     * @return Result of verifying the park brake force.
     */
    string ParkBrakeForceVerification(void);

    /**
     * Calibrate the park brake load cell readings.
     * 
     * @return Result of calibrating the load cells.
     */
    string ParkBrakeForceCalibration(void);

    /**
     * Run a pre-shift maintenance check.
     * This test just spins the rollers using the high torque brake motors for a few seconds.
     * 
     * @return Result of running the max brake motors.
     */
    string PreShiftCheck(void);

    /**
     * Check that the traction control system is functioning correctly.
     * The operator will be prompted to accelerate to the target speed.  If the target speed is reached or exceeded,
     * the operator will be instructed to enable the traction control system.  If vehicle speed does not fall to below
     * the target speed, the test will fail.
     * 
     * @return Result of checking the traction control system.
     */
    string TractionControlCheck(void);

    /**
     * Check the Front left valve...
     */
    string FrontLeftABSTest(void);


    /** Structure for holding brake force data during max brake force testing and park brake testing */
    typedef struct _maxBrakeData
    {
        float currentForce;
        float maxForce;
        string currentForceTag;
        string displayTag;
        bool measurementComplete;
    } MaxBrakeData;


    /**
     * Read the current load cell values.
     * 
     * @param brakeData Structure to store the current load cell readings in.
     * @param startingRoller
     *                  Roller to start monitoring.
     */
    void ReadCurrentLoadCellValues(MaxBrakeData *brakeData, UINT16 startingRoller);

protected:

    /**
     * Monitor the maximum brake force data developed on each roller.
     * The maximum observed brake force will be stored for each wheel being monitored.  When the required brake force for
     * each wheel has been detected, that wheel will be marked as completed.  After all wheels are complete or a timeout
     * occurs, the monitoring routine will exit.
     * 
     * @param startingRoller
     *                  Roller to start monitoring.
     * @param stoppingRoller
     *                  Roller to stop monitoring
     * @param minimumRequiredForceFront
     *                  Minimum required front brake force.
     * @param minimumRequiredForceRear
     *                  Minimum required rear brake force.
     * @param brakeData Structure to hold data tags and results.
     * 
     * @return Flag indicating if the minimum force has been observed on each wheel being monitored.
     */
    bool MonitorBrakeForces(INT16 startingRoller, INT16 stoppingRoller,
                            float minimumRequiredForceFront, float minimumRequiredForceRear,
                            MaxBrakeData *brakeData);



    std::list<signed int> m_flForceValue;
    std::list<signed int> m_frForceValue;
    std::list<signed int> m_rlForceValue;
    std::list<signed int> m_rrForceValue;

    /**
     * Compute the average of a List that was passed in
     * 
     * @param arr - the List<> that will be averaged
     * 
     * @return the average of the list
     */
    float computeListAverage(list<signed int> arr, float * max);

    /**
     * Analyze the dump forces
     */
    string MazdaAnalyzeABSDumpForces(MaxBrakeData *dumpForce, MaxBrakeData *startForce, UINT8 actuatedWheel);

    bool m_continueBrakeTest;
    float m_frontAxleMass;
    float m_rearAxleMass;

    float m_frontAxleAvgDrag;
    float m_rearAxleAvgDrag;
};
//-------------------------------------------------------------------------------------------------
#endif //MazdaBrakeTC_h
