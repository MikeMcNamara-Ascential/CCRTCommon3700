//*************************************************************************
// Description:
//   Rolls test machine component for performing machine based testing.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Machine/Include/MachineTC.h $
// 
// 1     11/21/07 1:41p Derickso
// Updated comment content.
// 
// 13    10/30/07 1:56p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 12    2/22/07 10:51a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Removed ResetVehicleTestStatus().
// - Added TestStepMultiDriverQuestion().
// - Updated and added Javadoc commments.
// 
// 11    1/17/07 3:41p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added WarmInitialize().
// 
// 10    9/05/06 3:55p Rwiersem
// Changes for the 06090601 core release:
// 
// - Made CommonMachineTest a friend class
// - Added TestStepReverse()
// 
// 9     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 8     4/29/05 8:00a GPattison
// Added functionality to allow the Machine Component to Set the
// System Max Boost Pressure to a value specified in the configuration
// file.  If no parameter exists in the configuration file to set the
// pressure, then nothing is done.
//
// 8     4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 8     4/07/05 1:01a GPattiso
// Added tests for CheckSpeedometer and AxleRatio for compatiblity with Hmma
// software needs.  Also removed the GetWheelSpeeds method because that was
// promoted to GenericTC
//
// 7     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 6     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 5     2/16/05 9:18p Cward
// Make test step methods protected instead of private, and also make them
// virtual.
//
// 4     6/24/04 12:44a Cward
// Removed pound if 0 block. Updated header comment block to the new
// standard.
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 2     9/22/03 5:35p Derickso
// Updated to use GenericTC instead of TestComponent.
//
// 1     8/21/03 9:00a Derickso
// 05WK CCRT Project
//
// 1     8/14/03 7:55a Khykin
// Updated with new naming convention from TestComponent to TC.
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/20/03 9:43a Khykin
// Initial checkin
//
// 1     6/20/03 9:19a Khykin
// Initial checkin.
//
// 1     4/22/03 10:26a Khykin
// Initial Working Checkin.
//
// 2     4/07/03 9:19a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:02:54  tbochene
//
//    PooBag
//
//
// 1     1/27/03 11:10a Khykin
// Initial Checkin
//
// 1     1/18/03 7:17a Khykin
// Initial Checkin
//
// 8     11/11/02 2:57p Khykin
// Updated wheel reads to use direct access.
//
// 7     11/04/02 6:50a Khykin
// Updated to add ability to reset serial hardware resets.
//
// 6     10/21/02 11:05a Khykin
//
// 5     8/23/02 8:01a Kh
// Synchronizing the pit machine with VSS.
//
// 3     5/24/02 1:47p Kh
// Updating due to addition of system tags.
//
// 2     3/25/02 6:17p Kh
// Updating due to changes in the base classes.
//
// 1     2/06/02 12:16a Kh
// Initial Checkin
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MACHINECOMPONENT_H
#define MACHINECOMPONENT_H

#include "GenericTC.h"

#define ARRAY_SIZE                    1024    /* array size for brake force */
#define LB_DEV                        3       /* lbs deviation during cal */
#define DEFAULT_MACHINE_TIMEOUT       10000   /* default timeout time of 10 seconds */
#define HYDRAULIC_PUMP_MOTOR_OFF      "1"
#define HYDRAULIC_PUMP_MOTOR_ON       "0"

/**
 * MachineTC class that is responsible for performing machine based 
 * responsibilities during a vehicle test.
 *
 * @author Kevin Hykin
 */
class MachineTC : public GenericTC
{
friend class CommonMachineTest;

public:
    /**
     * <b>Description</b><br>
     * Machine component constructor.
     */
    MachineTC ();
    /**
     * <b>Description</b><br>
     * Machine component destructor.
     */
    virtual ~MachineTC ();
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
    void Initialize(const XmlNode *config);
    /**
     * Reinitialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Reinitialize using the already loaded configuration file. This is intended to be used when a component is 
     * activated and there have been no modifications to any configuration files used by the component.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> MaxHydraulicBoostPressure - Maximum hydraulic boost pressure to allow. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetMaxHydraulicBoostPressure() </li>
     *      <li> GenericTC::WarmInitialize() </li>
     *      </ul>
     */
    virtual void WarmInitialize(void);
    /**
     * Register the component with the system.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * The base class will be called to register the component with the system.  If a communication hardware reset
     * is required, the communication hardware will be reset.  This will ensure the system is ready to go for the
     * vehicle test.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ResetCommHardware - Flag indicating if the communication hardware should be reset. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::Register() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     *
     * @return Result of registering the test component.
     *         <ul>
     *         <li> BEP_FAILURE_RESPONSE if the registration was not successful. </li>
     *         <li> BEP_SUCCESS_RESPONSE if the registration was successful. </li>
     *         </ul>
     */
    virtual const std::string Register(void);

protected:

    /**
     * Execute the next test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * The next test step will be exectued.  The test step result will be returned to the calling function.
     * <p>
     * <b>Supported Test Steps:</b>
     *      <ul>
     *      <li> Initialize </li>
     *      <li> DriverQuestion </li>
     *      <li> PromptDriver </li>
     *      <li> PromptDriverAlways </li>
     *      <li> DriverNumber </li>
     *      <li> AccelerateToSpeed </li>
     *      <li> BrakeToStop </li>
     *      <li> QuickStop </li>
     *      <li> ConnectCable </li>
     *      <li> MachineReady </li>
     *      <li> DisableHydraulicPumpMotor </li>
     *      <li> EnableHydraulicPumpMotor </li>
     *      <li> DisengageMachine </li>
     *      <li> EngageMachine </li>
     *      <li> SelectDriverSide </li>
     *      <li> FinishUp </li>
     *      <li> AxleRatio </li>
     *      <li> Speedometer </li>
     *      <li> Reverse </li>
     *      <li> MultiDriverQuestion </li>
     *      </ul>
     * 
     * <b>Test Step Functions:</b>
     *      <ul>
     *      <li> TestStepInitialize() </li>
     *      <li> TestStepDriverQuestion() </li>
     *      <li> TestStepPromptDriver() </li>
     *      <li> TestStepPromptDriverAlways() </li>
     *      <li> TestStepDriverNumber() </li>
     *      <li> TestStepAccelerateToSpeed() </li>
     *      <li> TestStepBrakeToStop() </li>
     *      <li> TestStepQuickStop() </li>
     *      <li> TestStepConnectCable() </li>
     *      <li> TestStepMachineReady() </li>
     *      <li> HydraulicPumpMotor() </li>
     *      <li> DisengageMachine() </li>
     *      <li> EngageMachine() </li>
     *      <li> TestStepSelectDriverSide() </li>
     *      <li> TestStepFinishUp() </li>
     *      <li> TestStepAxleRatio() </li>
     *      <li> TestStepSpeedometer() </li>
     *      <li> TestStepReverse() </li>
     *      <li> TestStepMultiDriverQuestion() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GenericTC::CommandTestStep() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     * @return The result of the test step.
     */
    virtual const std::string CommandTestStep(const std::string &value);
    /**
     * Handle data that was published to the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Handle published data that the component subscribed for.
     * <p>
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> KeyPress - System tag used to get the operator key press. </li>
     *     <li> DriverNumber - System tag used to get the driver number. </li>
     *     </ul>
     * 
     * <b>Supported Data Items:</b>
     *     <ul>
     *     <li> KeyPress </li>
     *     <li> DriverNumber </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetData() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetKeyPress() </li>
     *      <li> SetDriverNumber() </li>
     *      </ul>
     * 
     * @param node The information that was published to the component.
     * 
     * @return Status of updating the published tag.
     */
    virtual const std::string Publish(const XmlNode *node);
    /**
     * Validate the axle ratio is within limits.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * Run the axle ratio test.  The operator is prompted to accelerate to the test speed.  The wheel speeds will
     * be sampled for all four rollers.  The axle ratio will then be calculated.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> TargetSpeed - Axle ratio test speed. </li>
     *      <li> SpeedTarget - Target speed range for the axle ratio test. </li>
     *      <li> AxleRatioMin - Minimum acceptable axle ratio. </li>
     *      <li> AxleRatioMax - Maximum acceptable axle ratio. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Zerospeed - System tag for determining if the vehicle is at zero speed. </li>
     *      <li> FourWDSelected - System tag to use for instructing the PLC 4wd is selected. </li>
     *      <li> Start4WDTestSignalToPLC - System tag for signaling the PLC to begin the 4wd test. </li>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range to the operator. </li>
     *      <li> LFSpeed - System tag for reading the left front roller speed. </li>
     *      <li> RFSpeed - System tag for reading the right front roller speed. </li>
     *      <li> LRSpeed - System tag for reading the left rear roller speed. </li>
     *      <li> RRSpeed - System tag for reading the right rear roller speed. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> BrakeToStopPrompt - Prompt tag to use for instructing the operator to brake to stop. </li>
     *      <li> SetOperatorConsoleTo4WDPrompt - Prompt tag to use for instructing the operator to shift to 4wd. </li>
     *      <li> AccelerateToTargetSpeedPrompt - Prompt tag to use for instructing the operator to accelerate to the 
     *                                           target speed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> StatusCheck() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetParameter() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> SystemWrite() </li>
     *      <li> SystemReadFloat() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepName() </li>
     *      <li> RemovePrompts() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer for this test step.
     * 
     * @return The result of validating the axle ratio.
     */
    virtual const std::string TestStepAxleRatio(const std::string &value);
    /**
     * Perform a speedometer verification.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * Run the speedometer test.  The operator will be prompted to accelerate to the speedometer test speed.  
     * Then we wait for the driver to press the 40 KPH button and store the left roll speed of the driven axle 
     * as the speedometer test result.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SpeedometerTestLowLimit - Lower speed limit range for the speedometer verification </li>
     *      <li> SpeedometerTestHighLimit - Upper speed limit range for the speedometer verfication. </li>
     *      <li> SpeedometerSpeedTarget - Target speed range to display to the operator. </li>
     *      <li> DcxStyleSpeedoTest - Flag indicating if the speedometer verification is being performed for Chrysler. </li>
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
    virtual const std::string TestStepSpeedometer(const std::string &value);
    /**
     * Initialize the test.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will initialize the machine for operation.  Prompts will be cleared and system information 
     * will be reported for the printed report.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineNumber - System tag to use for getting the current machine number. </li>
     *      <li> SoftwareVersion - System tag to use for getting the current software revision number. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DateFormat - Format to use for reporting the test date. </li>
     *      <li> TimeFormat - Format to use for reporting the test time. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameter() </li>
     *      <li> CheckAbort() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ResetSystemTags() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer for this test step.
     *
     * @return The result of initializing the test component.
     */
    virtual const std::string TestStepInitialize(const std::string &value);
    /**
     * Display a question for the operator.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will prompt the driver with a question and wait for a Yes/No response and enter it 
     * as a test result for the prompt that was provided.
     * <p>
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> TestResultName - Name to use for reporting the result of the test step. </li>
     *     <li> Timeout - Amount of time(ms) to wait for a response from the operator. </li>
     *     <li> Description - Description of the test step. </li>
     *     </ul>
     * 
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> InputServerState - System tag for setting the state of the InputServer. </li>
     *     <li> TestResultMode - Value to send to the InputServer to place it into Test Result mode. </li>
     *     <li> YesValue - Value indicating the operator provided an affirmative answer. </li>
     *     <li> NormalMode - Value to send to the InputServer to place it into Normal mode. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetCurrentTestStep() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetTestStepName() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetKeyPress() </li>
     *      <li> CheckAbort() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.  It 
     *              will be used to determine which question to ask the driver.
     * 
     * @return The result of getting the response form the operator.
     */
    virtual const std::string TestStepDriverQuestion(const std::string &value);
    /**
     * Display a prompt for the operator.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will prompt the driver to perform a task but not look for input from the driver. On in-cycle 
     * retests, it will not display the prompt.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Timeout - Amount of time(ms) to keep the prompt displayed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetCurrentTestStep() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> CheckAbort() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.  It 
     *              will be used to determine which prompt to display to the 
     *              driver.
     * 
     * @return The result of displaying the prompt to the operator.
     */
    virtual const std::string TestStepPromptDriver(const std::string &value);
    /**
     * Display a prompt to the operator.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will prompt the driver to perform a task but not look 
     * for input from the driver.  Even on retests, it will display a prompt.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Timeout - Amount of time(ms) to keep the prompt displayed. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetCurrentTestStep() </li>
     *      <li> GetTestStepName() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> TimeRemaining() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> CheckAbort() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.  It 
     *              will be used to determine which prompt to display to the 
     *              driver.
     * 
     * @return The result of displaying the prompt to the operator.
     */
    virtual const std::string TestStepPromptDriverAlways(const std::string &value);
    /**
     * Retrieve the driver number from the operator.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will prompt the driver to input their driver ID and validate it if required.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Validate - Option indicating if the driver number should be validated or skipped. </li>
     *      <li> DriverNumberFile - File name containing the list of valid driver numbers. </li>
     *      <li> DefaultDriverNumber - Default system driver number. </li>
     *      <li> Timeout - Amount of time(ms) to wait for the operator to input the driver ID. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> DriverNumber - System tag to use to get the driver number entered. </li>
     *      <li> InputServerState - System tag for changing the mode of the InputServer. </li>
     *      <li> DriverNumberMode - Value to use to place the InputServer into Driver ID mode. </li>
     *      <li> ServerStateDriverNumber - Value to use to place the InputServer into Driver ID mode. </li>
     *      <li> ServerStateNormal - Value to use to place the InputServer into Normal mode. </li>
     *      <li> DriverNumberBGColor - System tag to use for setting the driver id background color. </li>
     *      <li> Fail - Color to use for Driver ID fail. </li>
     *      <li> NormalMode - Value to use to place the InputServer into Normal mode. </li>
     *      <li> Pass - Color to use for Driver ID pass. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> ValidateDriverNumber() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> WriteSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> DriverNumberNotComplete() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDriverNumber() </li>
     *      <li> CheckAbort() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer for this test step.
     * 
     * @return The result of collecting and possibly validating the operator ID.
     */
    virtual const std::string TestStepDriverNumber(const std::string &value);
    /**
     * Instruct the operator to accelerate to the test speed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will prompt the driver to accelerate to a provided speed and then monitor until the 
     * speed has been reached.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag used to display the target speed range to the operator. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetRollerCount() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> UpdateResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepName() </li>
     *      <li> RemovePrompts() </li>
     *      </ul>
     * 
     * @param value The speed value for the operator to accelerate to.
     * 
     * @return The result of waiting for the operator to accelerate to speed.
     */
    virtual const std::string TestStepAccelerateToSpeed(const std::string &value);
    /**
     * Instruct the operator to bring the vehicle to zero speed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will prompt the driver to brake to stop and wait until zero speed is achieved or a timeout occurs.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range to the operator. </li>
     *      <li> Zerospeed - System tag to use for determining of the vehicle is at zero speed. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> SetStartTime() </li>
     *      <li> TimeRemaining() </li>
     *      <li> UpdateResult() </li>
     *      <li> RemovePrompts() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     * 
     * @param value The value commanded by the test sequencer for this test step.
     * 
     * @return The result of the operator bringing the vehicle to zero speed.
     */
    virtual const std::string TestStepBrakeToStop(const std::string &value);
    /**
     * Use the motors to bring the rollers to zero speed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will prompt the driver to brake to stop and then command the motors to zerospeed if the 
     * machine is a 3600.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target sped range for the operator. </li>
     *      <li> Zerospeed - System tag to use for determining if the rollers are at zero speed. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> SpeedTarget - Speed target to diplay to the operator to achieve zero speed. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SystemWrite() </li>
     *      <li> TimeRemaining() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> UpdateResult() </li>
     *      <li> RemovePrompts() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     * 
     * @return The result of performing the quick stop.
     */
    virtual const string TestStepQuickStop (const std::string &value);
    /**
     * Instruct the operator to connect the diagnostic cable.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will verify that the cable is connected.  If it is not, it will prompt the driver to 
     * connect the cable and validate that it has been connected.  If the cable has not been connected, the
     * test sequence will be aborted.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> CableConnect - System tag used for determining if the diagnostci cable has been connected. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> StatusCheck() </li>
     *      <LI> UpdatePrompts() </li>
     *      <li> TimeRemaining() </li>
     *      <li> UpdateResult() </li>
     *      <li> SystemWrite() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      <li> RemovePrompts() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     * 
     * @return The result of validating the diagnostic cable has been connected.
     */
    virtual const string TestStepConnectCable (const std::string &value);
    /**
     * Verify the machine is ready to perform the test sequence.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will verify that the machine is in a valid state to start operating.  If the machine is not 
     * in a valid state, the test sequence will be aborted.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> TimeRemaining() </li>
     *      <li> CheckAbort() </li>
     *      <li> SystemWrite() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     * 
     * @return The result of validating the machine state is acceptable for testing.
     */
    virtual const string TestStepMachineReady (const std::string &value);
    /**
     * Set the state of the hydraulic pump motor.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * Set the state of the hydraulic pump motor to either On or Off.
     * <br><b><i>Note:</i></b>  The hydraulic pump motor should be off when the vehicle is being driven in reverse.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> HydraulicPumpMotor - System tag for setting the state of the hydraulic pump motor. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> PumpStatusChangeDelay - Amount of time(ms) to wait for the hydraulic pump motor to change states. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param state The new hydraulic pump motor state ("0" or "1").
     * 
     * @return The result of placing the hydraulic pump motor in the specified state.
     */
    virtual const string HydraulicPumpMotor(const string &state);
    /**
     * Perform test finish-up processing.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * Finishup test step that sends the overall result.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdatePrompts() </li>
     *      <li> SendOverallResult() </li>
     *      <li> GetOverallResult() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     * 
     * @return The result of performing the finish-up processing.
     */
    virtual const std::string TestStepFinishUp(const std::string &value);
    /**
     * Select the driver side of the vehicle.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * Test step to create a symbolic link to the gryphon box on the correct side for this vehicle.  The correct
     * side will be determined by which diagnostic cable is connected.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> RightLogicalPort - Name of the device port to connect to for a right hand drive vehicle. </li>
     *      <li> LeftLogicalPort - Name of the device port to connect to for a left hand drive vehicle. </li>
     *      <li> CommmonLogicalPort - Name of the symbolic link to create. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> RighthandDriveSelected - System tag to use to determine if the right side cable was connected. </li>
     *      </ul>
     *   
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommLinkFailure - Could not create symbolic link to the appropriate device port. </li>
     *      <li> LinkInfoMissing - Device port or symbolic link information was not specified. </li>
     *      <li> SoftwareFailure - An exception occurred attempting to create the symbolic link. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> SystemReadBool() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     * 
     * @return The result of creating the correct symbolic link.
     */
    virtual const std::string TestStepSelectDriverSide(const std::string &value);
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
    virtual const std::string TestStepReverse(const std::string &value);
    /**
     * Get a pendant key press from the operator.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Wait for the operator to press a key on the pendant.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> KeyPress - System tag to use for reading the pendant key press value. </li>
     *      <li> YesValue - Value to use to determine if the operator entered an affirmative response. </li>
     *      <li> NoValuesValue - Value to use to determine if the operator entered an negative response. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @param timeout The amount of time to wait for the driver to press a key.
     * 
     * @return The key pressed by the operator.
     */
    virtual const std::string GetKeyPress(INT32 timeout);
    /**
     * Signal a pendant key press has been received.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Method that is used to signal the main process when a new key press has been received.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> KeyPress - System tag to use for getting the pendant key press data. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     */
    virtual void SetKeyPress(void);
    /**
     * Wait for the operator to enter the drive ID.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Method that blocks until a new driver number is received.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> InputServerState - System tag to use for getting/changing the InputServer state. </li>
     *      <li> DriverNumberMode - Value to use for placing the InputServer into Driver Number mode. </li>
     *      <li> DriverNumber - System tag to read the driver number data. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @param timeout The amount of time to wait for a driver ID to be entered.
     * 
     * @return The value of the driver number that was entered.  The value "Error" is returned if a 
     *         valid driver ID is not entered.
     */
    virtual const std::string GetDriverNumber(INT32 timeout);
    /**
     * Signal the drive number has been received.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Method that is used to signal the main process when a new driver number has been entered.
     */
    virtual void SetDriverNumber(void);

    /**
     * Set the InputServer to the desired state.
     * 
     * @param state  State to place the input server into.
     */
    string SetInputServerState(const string &state);

    /**
     * Check if a valid driver number has been entered.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Determine if a drivers number has been entered.  A valid driver number is a number that is not empty 
     * and not equal to DEFAULT_DRIVER_NUMBER.
     * <p>
     * @param driverNumber The driver number that was received.
     * 
     * @return Flag indicating if driver number entry is not complete.
     *         <ul>
     *         <li> True - A complete driver number has been entered. </li>
     *         <li> False - The driver number has not been entered. </li>
     *         </ul>
     */
    virtual bool DriverNumberNotComplete(const std::string driverNumber);
    /**
     * Verify the driver number is valid.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Validate the driver ID number.  The driver number validation list will be loaded.  The driver number entered
     * by the operator will be looked up in the list.  A flag indicating if the driver number is valid will be 
     * returned to the calling function.
     * <p>
     * @param fileName     The file that contains the list of valid IDs.
     * @param driverNumber The driver number that was entered.
     * 
     * @return Flag indicating if the driver number is valid.
     *         <ul>
     *         <li> True - The driver number is valid. </li>
     *         <li> False - The dirver number is not valid. </li>
     *         </ul>
     */
    virtual const bool ValidateDriverNumber(const std::string &fileName, const std::string &driverNumber);
    /**
     * Set the maximum allowable boost pressure for hydraulic machines.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Method to command the MotorController task to set the system MaxBoostPressure to the value passed 
     * in as a parameter.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemCommand() </li>
     *      </ul>
     * 
     * @param pressure The new maximum hydraulic boost pressure.
     *
     * @return Status of setting the maximum hydraulic boost pressure.
     */
    INT32 SetMaxHydraulicBoostPressure(const string &pressure);
    /**
     * Prompt the operator to answer multiple questions.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will prompt the driver with multiple questions and wait for a Yes/No response and 
     * enter it as a test result for the prompt that was provided.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> TestResultName - Name to use for reporting the result of the operator input. </li>
     *      <li> Timeout - Amount of time(ms) to wait for the operator to answer the question. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DoNotFailVehicleForMPBO - Flag to indicate if failures should be reported or just flagged. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> InputServerState - System tag to be used for setting the mode of the InputServer. </li>
     *      <li> TestResultMode - Value to use for placing the InputServer into Test Result mode. </li>
     *      <li> YesValue - Value to use for determining if the operator entered an affirmative response. </li>
     *      <li> NormalMode - Value to use for placing the InputServer into Normal mode. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> SetCurrentTestStep() </li>
     *      <li> GenericTC::UpdateTaskStats() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() <?li>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetKeyPress() </li>
     *      <li> CheckAbort() </li>
     *      <li> RemovePrompts() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetParameterBool() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.  It 
     *              will be used to determine which question to ask the driver.
     * 
     * @return The result of gathering data from the operator.
     */
    virtual const string TestStepMultiDriverQuestion(const string &value);

    /**
     * Get/Set the speed at which the speedometer/40kph button was pressed.
     * 
     * @param speed  Speed at which the speedometer/40kph button was pressed.
     * 
     * @return Speed at which the speedometer/40kph button was pressed.
     */
    const float SpeedometerResultSpeed(const float *speed = NULL);

private:
	/**
	 * Check the machine conditions to make sure the machine is in the correct state for testing.
	 * If one of the conditions is not correct, the sequence will be aborted and a prompt provided 
	 * to the operator stating the condition.
	 * 
	 * @param value  Value commanded with the test step.
	 * 
	 * @return Result of validating the test conditions.
	 */
	string ValidateMachineConditions(const string value);

    /**
     * Semaphore that is used to indicate a pendant input has been received.
     */
    BepSemaphore m_semKeyPress;
    /**
     * Semaphore that is used to indicate a driver number has been received.
     */
    BepSemaphore m_semDriverNumber;
    /**
     * Request pendant keypress information data flag.  This is set when the
     * MachineTC would like to be notified when pendant keypress information
     * is received.
     */
    bool m_requestKeyPressInput;
    /**
     * Request driver id data flag.  This is set when the MachineTC would
     * like to be notified when pendant keypress information is received.
     */
    bool m_requestDriverNumber;
    /**
     * Speed at which the speedometer button was pressed.
     */
    float m_speedoTestSpeedValue;
	/** List of machine conditions to be in place at the start of a test */
	XmlNodeMap m_machineConditions;
};

#endif
