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
#ifndef ToyotaMachineTC_h
#define ToyotaMachineTC_h
//-------------------------------------------------------------------------------------------------
#include "MachineTC.h"
//-------------------------------------------------------------------------------------------------

// XYSeries and time value entries for pacesetter test
typedef vector<std::pair<int, int> > XYSeries;

struct PacesetterEntry
{
   UINT32 timeStamp;
   float highSpeed;
   float lowSpeed;
};

class ToyotaMachineTC : public MachineTC
{
public:
    ToyotaMachineTC();
    virtual ~ToyotaMachineTC();

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

protected:
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

    // Pacesetter test
    virtual const string TestStepPacesetter(void);
    // Pacesetter conversion func
    virtual BEP_STATUS_TYPE ConvertSeriesToVector(XYSeries &upper, XYSeries &lower);
};
//-------------------------------------------------------------------------------------------------
#endif //ToyotaMachineTC_h
