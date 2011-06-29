//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef TevesAbsTc_h
#define TevesAbsTc_h
//-----------------------------------------------------------------------------
#include "GenericABSTCTemplate.cpp"
//-----------------------------------------------------------------------------

template<class ModuleInterface>
class TevesAbsTc : public GenericABSTCTemplate<ModuleInterface>
{
public:
    /** Class constructor */
    TevesAbsTc();
    /** Class destructor */
    virtual ~TevesAbsTc();
    /**
     * Sequence a test step.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     *
     * @param value    This is the value that was commanded by the test sequencer.
     * @return  The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

protected:
    /**
     * Determine the configuration of the vehicle.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The parameters will be checked to determine if the vehicle is equipped with a four channel system.
     * 
     * @return Result of determining the system configuration
     */
    virtual string DetermineSystemConfiguration(void);
    /**
     * Initialize the park brake test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Perform pre-electirc park brake test initialization.  System status will be monitored while waiting for the 
     * operator to place the vehicle in the correct state to test the park brake.  The base class will be called to
     * perform the waiting.  After the waiting is completed, the drive axle will be switched so the rear set of motors
     * is enabled.
     * <p>
     * 
     * @return The system status after waiting for the operator to place the vehicle in a testable state.
     */
    virtual INT32 ElectricParkBrakeInit(void);
    /**
     * Test the park brake
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Torque will be applied to the rear axle and the module will be checked to ensure the brake switch is off.  
     * After a set amount of time, torque will be removed from the applicable axles.  The wheel distances will then be 
     * analyzed to ensure the park brake is set correctly and the operator was not applying the brake pedal.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> ParkBrakeApplyForce - Amount of force to aply to the rear rollers. </li>
     *     </ul>
     *
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> TestTime - Amount of time(ms) to apply torque to the drive axle rollers. </li>
     *     <li> Description - Description of the test step. </li>
     *     </ul>
     * 
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> Zerospeed - System tag to use to check if the vehicle is at zero speed. </li>
     *     </ul>
     * 
     * <b>Fault Tags:</b>
     *     <ul>
     *     <li> ParkBrakeFault - Park brake is not set properly. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetWheelDistances() </li>
     *      <li> SetStartTime() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> SystemCommand() </li>
     *      <li> TimeRemaining() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> ValidateParkBrakeTestDistances() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param direction The direction to turn the rollers.  Valid values are "Forward", "Reverse" or "Both".
     *
     * @return The result of testing the park brake.
     */
    virtual const string PerformPBTorqueTest(const string &direction);
    /**
     * Place the module back into normal mode once testing is completed.
     * 
     * @return REsult of commanding the module to return to normal mode.
     */
    string EnterNormalMode(void);
    /**
     * Disable the brake force meter and updates.
     * 
     * @return Result of disabling the brake force meter.
     */
    string DisableForceMeter(void);
        /**
     * Put Motors Into Speed Mode for Incycle Retesting.
     * 
     * @return Result of the test.
     */
    string AccelerateToBrakeSpeed(void);
    /**
     * Perform the wheel speed sensor test and burnish the brakes.
     * 
     * @return Result of testing the wheel speed sensors and burnishing the brakes.
     */
    string IndividualSensorTest(void);
    /**
     * Perform a brake burnish cycle on the vehicle.
     * The operator will be prompted to accelerate to the target speed and then brake to a lower target
     * speed.  This cycle will be repeated multiple times.
     * 
     * @return Result of the brake burnish cycle.
     */
    string BrakeBurnishCycle(void);
    /**
     * Perform a static brake burnish test.
     * Each wheel will be accelerated to a specific speed.  Once the desired speed has been achieved, the
     * operator will be instructed to apply brake pedal until the target speed is achieved.  Once the burnish 
     * cycle is completed, the roller will be accelerated for a time period to allow the pads and rotors to 
     * cool.
     * 
     * @return Result of burnishing the brakes.
     */
    string StaticBrakeBurnishCycle(void);

private:
    /**
     * Store the original drive axle prior to switching so it can be restored.
     * 
     * @param driveAxle Drive axle value to be stored.
     * 
     * @return Drive axle value to be restored after park brake testing is completed.
     */
    string& OriginalDriveAxle(const string *driveAxle = NULL);
    /**
     * Original drive axle value.
     */
    string m_originalDriveAxle;
};

//-----------------------------------------------------------------------------
#endif //TevesAbsTc_h
