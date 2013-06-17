//*************************************************************************
// Description:
// This file contains the functions necessary to test the vehicle.
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
#ifndef MazdaMachineTC_h
#define MazdaMachineTC_h
//-------------------------------------------------------------------------------------------------
#include "MachineTC.h"
//-------------------------------------------------------------------------------------------------
class MazdaMachineTC : public MachineTC
{
public:
    MazdaMachineTC();
    virtual ~MazdaMachineTC();

    /**
     * Execute the next test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * The next test step will be exectued.  The test step result will be returned to the calling function.
     * <p>
     * 
     * @param value The value that was commanded by the test sequencer.
     * @return The result of the test step.
     */
    virtual const string CommandTestStep(const string &value);
    /**
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Initialization method to set up the MachineTC component.  The base class will be called to begin the
     * initialization.  The maximum boost pressure will then be saved off to a member variable.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> MaxHydraulicBoostPressure - Maximum hydraulic boost pressure to allow. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::Initialize() </li>
     *      <li> SetMaxHydraulicBoostPressure() </li>
     *      <li> InitializationComplete() </li>
     *      </ul>
     * 
     * @param config Configuration data to use for initializing the test component.
     */
    virtual void Initialize(const XmlNode *config);

protected:

	/**
	 * Stop the odometer test.
	 * The current distance will be colelcted and used to determine the overall distance the vehicle traveled.
	 * The distance will be sent to the PLC in km.
	 * 
	 * @return Result of calculating the distance traveled.
	 */
	string StopOdometerTest(void);

    /**
     * Perform a speedometer verification.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * Run the speedometer test.  The operator will be prompted to accelerate to the speedometer test speed.
     * Then we wait for the driver to flash the highbeams and store the left roll speed of the driven axle
     * as the speedometer test result.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SpeedometerTestLowLimit - Lower speed limit range for the speedometer verification </li>
     *      <li> SpeedometerTestHighLimit - Upper speed limit range for the speedometer verfication. </li>
     *      <li> SpeedometerSpeedTarget - Target speed range to display to the operator. </li>
     *      <li> SpeedometerStableTime - Amount of time the vehicle speed must remain stable. </li>
     *      <li> TargetSpeed40kph - Target speed range to display for the operator. </li>
     *      </ul>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the speed target for the operator. </li>
     *      <li> DriveAxle - System tag to used to get the vehicle drive axle. </li>
     *      <li> HornSignal - System tag to use for commanding the horn. </li>
     *      <li> Start40kphTest - System tag to use for determining if the 40kph button was pressed. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetRollerCount() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> MaintainSpeedForTime() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      </ul>
     *
     * @param value The value that was commanded by the test sequencer for this test step.
     *
     * @return The result of verifying the speedometer.
     */
    virtual const string TestStepSpeedometer(const string &value);

    /**
     * Perform the reverse test.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * Run the reverse test.  We prompt the driver to shift to reverse and accelerate to the reverse test speed.  
     * If the reverse test speed is reached, the test passes.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ReverseTestSpeed - Vehicle speed required for the reverse speed test to pass. </li>
     *      <li> ReverseSpeedTarget - Speed target to display for the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToReverse - Shift the transmission into Reverse. </li>
     *      <li> AccelerateToReverseTargetSpeed - Accelerate in reverse to the target speed. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displayg the target speed range. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> StatusCheck() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetStartTime() </li>
     *      <li> WaitForTargetSpeed() </li>
     *      <li> TimeRemaining() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      <li> RemovePrompts() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     * 
     * @return The result of the reverse speed testing.
     */
    virtual const string TestStepReverse(const string &value);

    /** 
     * Prompt the operator to start the test
     * 
     */
    virtual const string StartTest(void);

	/**
	 * Instruct the operator through the disconnect sequence.
	 * 1.  Turn off ignition
	 * 2.  Disconnect cable
	 * 3.  Start engine
	 * 
	 * @return Result of performing the disconnect sequence.
	 */
	const string VehicleDisconnect();

	/**
	 * Prompt the operator throught the vehicle setup procedure.
	 * 1.  Ignition off
	 * 2.  Connect cable
	 * 3.  Apply brake pedal
	 * 4.  Start the engine
	 * 
	 * @return Result of setting up the vehicle for test.
	 */
    const string VehicleSetup(void);

    /**
     * Sit in a wait loop until the operator begins accelerating.  This step will never timeout since a test could be
     * started and it could be a long time until the operator comes to perform the test sequence.
     * 
     * @return Pass - always.
     */
    string WaitToStart(void);

    /**
     * Get the drive axle roller speed.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This function returns the maximum roller speed. 
     * <p>
     * 
     * @return The maximum roller speed.
     */
    virtual float GetRollSpeed(void);
    /**
     * Instruct the operator to accelerate to the test speed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will prompt the driver to accelerate to a provided speed and then monitor until the 
     * speed has been reached.
     * <p>
     * 
     * @param value The speed value for the operator to accelerate to.
     * 
     * @return The result of waiting for the operator to accelerate to speed.
     */
    virtual const string TestStepAccelerateToSpeed(const string &value);

    const string TestStepRainLightSensorVerification(void);
    /**
     * Check for valid RLS type.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The vehicle build data will be analyzed to determine if the vehicle is equipped with a valid RLS type.
     * A flag indicating if the vehicle is equipped with a valid RLS type will be stored in a member variable.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> BroadcastRLSEquippedType - System tag to read the ETC build option from the vehicle build data. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetRLSEquipped() </li>
     *      </ul>
     */
    void CheckForValidRLSType(void);
    /**
     * Check if the vehicle is equipped with rain light sensor.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the flag indicating if the vehicle is equipped with RLS.
     * <p>
     * @return  Flag indicating if the vehicle is equipped with RLS.
     */
    virtual const bool &IsRLSEquipped(void);

    /**
     * Store the flag indicating if the vehicle is equipped with rain light sensor.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the flag indicating if the vehicle is equipped with RLS.
     * <p>
     * @param equipped  Flag indicating if the vehicle is equipped with RLS.
     */
    virtual void SetRLSEquipped(const bool &equipped);

    /** Store the the valid rls equipped Types for this test from configuration file. */
    XmlNodeMap m_rlsEquippedTypes;

    /** Flag to indicate if the vehicle is equipped with RLS. */
    bool m_rlsEquipped;

	WHEELINFO m_odoStartDistance;
};
//-------------------------------------------------------------------------------------------------
#endif //MazdaMachineTC_h
