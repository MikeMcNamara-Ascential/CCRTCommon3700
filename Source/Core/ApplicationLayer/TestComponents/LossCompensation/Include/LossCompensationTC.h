//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/LossCompensation/Include/LossCompensationTC.h 1     11/21/07 8:19a Derickso $
//
// FILE DESCRIPTION:
// This file contains the functions necessary to calibrate the rolls
// tester.
//
//===========================================================================
// Copyright (c) 1999-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/LossCompensation/Include/LossCompensationTC.h $
// 
// 1     11/21/07 8:19a Derickso
// Updated doxygen comments.
// 
// 7     10/30/07 1:55p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 6     2/22/07 9:02a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Updated and added Javadoc commments.
// 
// 5     1/17/07 3:38p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added WarmInitialize().
// 
// 4     7/05/06 10:00p Cward
// Fix sequencing errors caused by Vehicle Present, lowering retainers, or
// turning off the loss comp switch during a Loss Comp test
//
// 3     2/15/06 7:21p Cward
// Added Low torque startup. Added warmup speed tolerance. Updated
// Staggered Accelerate Test Step.
//
// 4     12/16/05 1:46p Bmeinke
// Added a new LowTorqueStartup() method to perform a low torque startup
// on an individual roller. This is used by the
// TestStepStaggeredAccelerate() method to perform a soft startup on each
// roller before commanding speed.
//
// 3     12/12/05 2:52p Bmeinke
// Added TestStepLowTorqueStartup() to apply a small torque to the rollers
// before the acceleartion test step which would easily allow the rollers
// to be manually stopped in case somebody is working on the machine when
// the LC is started
//
// 2     11/22/05 10:49a Bmeinke
// Moved all the "private" members to protected
// Made many methods virtual in order to override for Tata
//
// 1     11/17/05 10:35a Bmeinke
// Updating Tata to latest core
//
// 2     4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 5     4/04/05 12:42a Bmeinke
// Added test step StaggeredAccelerate
//
// 4     4/04/05 12:42a Gpattiso
// Updated to ignore vehicle present becoming active during the
// acceleration phase and also to not error out if the retainers
// are lowered before zero speed is reached
//
// 1     12/21/03 6:33p Cward
//
// 2     9/22/03 5:35p Derickso
// Updated to use GenericTC instead of TestComponent.
//
// 1     8/21/03 9:00a Derickso
// 05WK CCRT Project
//
// 1     8/14/03 7:52a Khykin
// Updated with new naming convention from TestComponent to TC.
//
// 3     8/12/03 2:44p Khykin
// Updated with new directory update changes.
//
// 2     8/06/03 10:02a Khykin
// Updated with changes from Flatrock.
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
// 1     4/22/03 10:25a Khykin
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
// 10    8/23/02 8:01a Kh
// Synchronizing the pit machine with VSS.
//
// 8     5/24/02 1:47p Kh
// Updating due to addition of system tags.
//
// 7     3/25/02 6:17p Kh
// Updating due to changes in the base classes.
//
// 6     3/06/02 1:53p Kh
// Adding the ability to tag an ICM array and handle component status.
//
// 5     2/06/02 12:17a Kh
// Updating.
//
// 4     2/01/02 4:40p Kh
// Compiles now time to debug.
//
// 3     1/16/02 3:56p Russ.ross
// Checking in compiling LossCompensation files...
//
// 2     1/15/02 2:05p Russ.ross
// Compiling and running copy...still needs some more work.
//
// 1     1/03/02 8:32a Kh
//
// 5     12/20/01 1:41p Admin
// Checking in.
//
// 4     12/10/01 3:57p Russ.ross
// Updating with latest version...
//
// 3     12/02/01 7:10p Russ.ross
// Checking in the latest version...
//
// 2     11/20/01 8:38a Russ.ross
//
// 1     10/16/01 9:13a Russ.ross
// Checking in version of calibrate.cpp...
//
//*************************************************************************
#ifndef LOSSCOMPENSATION_H
#define LOSSCOMPENSATION_H

#include <strstream.h>
#include <math.h>
#include <time.h>
#include "GenericTC.h"

using namespace std;

/** The list of wheel losses. */
typedef vector< float >             LOSSLIST;

/**
 * LossCompensationTC component that is responsible for performing a Loss 
 * Compensation test on the machine.
 *
 * @author Russ Ross
 */
class LossCompensationTC : public GenericTC
{
    friend class LossCompensationTest;

public:

    /**
     * <b>Description:</b><br>
     * LossCompensationTC constructor.
     */
    LossCompensationTC();

    /**
     * <b>Description:</b><br>
     * LossCompensationTC destructor.
     */
    virtual ~LossCompensationTC();

    /**
     * Initialize the Loss Compensation component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The configuration data will be used to initialize the test component.  System variables updated by the Loss
     * Compensation test will be reset to initial values.  The current losses will be read from the system as will
     * the date and time of the last loss compensation sequence.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::Initialize() </li>
     *      <li> SetupLossData() </li>
     *      <li> InitializationComplete() </li>
     *      </ul>
     * 
     * @param config The configuration data to use for initialization.
     */
    void Initialize(const XmlNode *config);

    /**
     * Reinitialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reinitialize the test component using the already loaded configuration file. This is intended to be used 
     * when a component is activated and there have been no modifications to any configuration files used by the 
     * component.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetupLossData() </li>
     *      <li> GenericTC::WarmInitialize() </li>
     *      </ul>
     */
    virtual void WarmInitialize(void);
    
protected:
    /**
     * Execute a test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the system 
     * for each test objective that is passed to it.
     * <p>
     * <b>Supported Test Steps:</b>
     *      <ul>
     *      <li> Initialize </li>
     *      <li> LowTorqueStartup </li>
     *      <li> Accelerate </li>
     *      <li> StaggeredAccelerate </li>
     *      <li> LossCompensation </li>
     *      <li> Stop </li>
     *      <li> FinishUp </li>
     *      </ul>
     * 
     * <b>Test Step Functions Called:</b>
     *      <ul>
     *      <li> TestStepInitialize() </li>
     *      <li> TestStepLowTorqueStartup() </li>
     *      <li> TestStepAccelerate() </li>
     *      <li> TestStepStaggeredAccelerate() </li>
     *      <li> TestStepLossCompensation() </li>
     *      <li> TestStepStop() </li>
     *      <li> TestStepFinishUp() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> SetLCStatus() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     * 
     * @return The result of the test step.
     */
    virtual const string CommandTestStep(const string &value);

    /**
     * Ensure the system is ready to perform a loss compensation sequence.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check the current status of the machine and the component to determine if the current conditions are valid 
     * for continuing a test.  If the current conditions are not valid, the machine will be disengaged.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> VehiclePresent - System tag to use to determine if a vehicle is currently on the machine. </li>
     *      <li> LossCompensationSwitchStatus - System tag to use for reading the state of the loss 
     *                                          compensation switch. </li>
     *      <li> RaiseRetainers - System tag used to determine if the retaining devices are in position. </li>
     *      <li> FaultRetainers - Fault tag to use to indicate the retaining devices are not in position. </li>
     *      <li> FaultSwitch - Fault tag to use to indicate the loss compensation switch is not in the 
     *                         correct position.</li>
     *      <li> FaultVehiclePresent - Fault tag to use to indicate a vehicle is currently on the machine. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SkipVehiclePresentDuringAccelerate - Flag indicating if vehicle presence should not be checked while
     *                                                the rollers are accelerating. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameterBool() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetTestStepName() </li>
     *      <li> Disengage() </li>
     *      </ul>
     * 
     * @return The current machine status.
     */
    virtual INT32 LossCompStatusCheck(void);

    /**
     * Initialize the test conditions.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * This test step is responsible for validating that the machine is ready to perform a loss compensation cycle.
     * If any condition is not met, the operator will be instructed to take the appropriate actions to place the
     * machine in the correct state to perform the loss compensation sequence.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> LossCompensationSwitchStatus - System tag for reading the state of the loss compensation switch. </li>
     *      <li> VehiclePresent - System tag to use for determining if a vehicle is currently on the machine. </li>
     *      <li> Zerospeed - System tag for determining of the rollers are at zero speed. </li>
     *      <li> RaiseRetainers - System tag for determining if the retaining devices are in position. </li>
     *      <li> StartLossCompensation - System tag for determining of a loss compensation cycle has started. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> VehiclePresentPromptBox - Prompt box in which to display the vehicle present prompt. </li>
     *      <li> VehiclePresentPrompt - Prompt tag to use for informing the operator a vehicle is on the machine. </li>
     *      <li> ZerospeedPromptBox - Prompt box in which to display the zero speed prompt. </li>
     *      <li> ZerospeedPrompt - Prompt tag to use for informing the operator the rollers are not at zero speed. </li>
     *      <li> RaiseRetainersPromptBox - Prompt box in which to display the raise retainers prompt. </li>
     *      <li> RaiseRetainersPrompt - Prompt tag for instructing the operator to raise the retaining rolls. </li>
     *      <li> StartLossCompensationBox - Prompt box in which to display the start loss compensation prompt. </li>
     *      <li> StartLossCompensationPrompt - Prompt tag for instructing the operator to start the loss 
     *                                         compensation sequence. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> LossCompStatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> CheckAbort() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> Engage() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of ensuring the machine is in the correct state to perform the loss compensation sequence.
     */
    virtual const string TestStepInitialize(void);

    /**
     * Perform low torque startup on each roller.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Commands each motor to a low torque value at LC startup to ensure that there are no physical obstructions 
     * (i.e. an electricain's arm) to inhibit a LC acceleration.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineType3600 - System tag for determining if the current machine is an electric machine. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> StartupTorqueCommand - Torque to be applied to the rollers during the safety startup. </li>
     *      <li> MaxTorqueModeSpeed - Maximum roller speed during the safety startup. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> GetRollerCount() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetLCStatus() </li>
     *      <li> LossCompStatusCheck() </li>
     *      <li> GetTestStepParameter() </li>
     *      <li> SetStartTime() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> StatusSleep() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @return The result of performing the low torque safety startup.
     */
    virtual const string TestStepLowTorqueStartup(void);

    /**
     * Accelerate the rollers to the target speed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * This test step is responsible for commanding the rollers to the loss compensation arm speed and that they 
     * remain at that speed until the warmup time has expired.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> WarmUpTime - Number of seconds to hold the rollers at speed for the warmup period. </li>
     *      <li> LCArmSpeed - Loss compensation sequence arm speed. </li>
     *      <li> WarmupPromptBox - Box in which to display the warm up prompt. </li>
     *      <li> WarmupPrompt - Prompt tag to use for informaing the operator the warm up period is in progress. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> LossCompensationWarmupTime - System tag for displaying the warm up time period. </li>
     *      <li> LossCompensationWarmupTimeRemaining - System tag for displaying the number of seconds remaining in 
     *                                                 the warm up period. </li>
     *      <li> ArmSpeed - System tag for displaying the locc compensation arm speed. </li>
     *      <li> <i><roller></i>WheelDomain - Wheel domain to display for <i>roller</i>. </li>
     *      <li> <i><roller></i>WheelTarget - Target area to display for each <i>roller</i>. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> WarmUpSpeedTolerance - Wiggle room allowable from the target speed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetLCStatus() </li>
     *      <li> LossCompStatusCheck() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> GetDataTag() </li>
     *      <li> SystemWrite() </li>
     *      <li> SystemRead() </li>
     *      <li> SystemCommand() </li>
     *      <li> GetRollerCount() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> SpeedCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> RemovePrompts() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of accelerating the rollers to speed.
     */
    virtual const string TestStepAccelerate(void);

    /**
     * Accelerate each roller to speed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * This test step is responsible for commanding the rollers to the loss compensation arm speed and that they 
     * remain at that speed until the warmup time has expired. Motors are started in a staggered fashion in order
     * to reduce the draw from the motors and prevent an under voltage condition to occur.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> CheckSensorCross - Flag indicating if the encoders should be checked to ensure the cables are
     *                              connected to the correct ports. </li>
     *      <li> WarmUpTime - Amount of time to allow the bearings to warm up after reaching the target speed. </li>
     *      <li> LCArmSpeed - Loss compensation arm speed. </li>
     *      <li> Timeout - Amount of time to wait for the rollers to reach arm speed. </li>
     *      <li> WarmupPromptBox - Prompt box in which to display the warm up prompt. </li>
     *      <li> WarmupPrompt - Prompt tag for informing the operator that the warm up period is in progress. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineType3600 - Value indicating the machine is an electric machine. </li>
     *      <li> LossCompensationWarmupTime - System tag to display the warm up time. </li>
     *      <li> LossCompensationWarmupTimeRemaining - System tag to use for displaying the amount of time left for 
     *                                                 the warm up period. </li>
     *      <li> ArmSpeed - System tag for displaying the loss compensation arm speed. </li> </li>
     *      <li> <i><roller></i>WheelDomain - System tag for displaying the wheel speed domain for <i>roller</i>. </li>
     *      <li> <i><roller></i>WheelTarget - System tag for displaying the whel speed target for <i>roller</i>. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> StartupTorqueCommand - Amount of torque to apply to the rollers during startup. </li>
     *      <li> MaxTorqueModeSpeed - Maximum speed to allow the rollers to spin during the low torque startup. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> <i><roller></i>EncoderCross - The encoder cable for <i>roller</i> is not connected to the correct port. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetTestStepInfoBool() </li>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> GetTestStepParameter() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetLCStatus() </li>
     *      <li> LossCompStatusCheck() </li>
     *      <li> SystemWrite() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> SetStartTime() </li>
     *      <li> LowTorqueStartup() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> StatusSleep() </li>
     *      <li> SetFault() </li>
     *      <li> Disengage() </li>
     *      <li> SpeedCheck() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> TestStepAccelerate() </li>
     *      </ul>
     * 
     * @return The result of stagger accelerating the rollers.
     */
    virtual const string TestStepStaggeredAccelerate(void);
    /**
     * Accelerate the rollers.
     * <p><b>Category:</b>  Test Step
     * <p><b>Description:</b><br>
     * The rollers will be accelerated to the arm speed.  The drive axle will be changed in order to accelerate both the
     * front and rear rollers.
     * <p>
     * 
     * @return Result of accelerating the rollers.
     */
    const string TwoMotorAccelerate(void);
    /**
     * Sample the roller losses.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * This test step is responsible for performing a coastdown of the rollers and sampling the brake force to 
     * determine the average loss over the sampled interval.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> StartSampleSpeed - System tag for displaying the sample start speed. </li>
     *      <li> EndSampleSpeed - System tag for displaying the sample end speed. </li>
     *      <li> MachineLossMaxLeft - System tag for displaying the maximum allowable machine loss for the 
     *                                left rollers. </li>
     *      <li> MachineLossMinLeft - System tag for displaying the minimum allowable machine loss for the 
     *                                left rollers. </li>
     *      <li> MachineLossMaxRight - System tag for displaying the maximum allowable machine loss for the
     *                                 right rollers. </li>
     *      <li> MachineLossMinRight - System tag for displaying the minimum allowable machine loss for the
     *                                 right rollers. </li>                                                 
     *      <li> MachineLossMaxDelta - System tag for displaying the maximum amount of change allowable in 
     *                                 machine losses from the previous loss compensation sequence. </li>
     *      </ul>                                                 
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> StartSampleSpeed - Speed at which to begin sampling roller forces. </li>
     *      <li> StopSampleSpeed - Speed at which to stop sampling roller forces. </li>
     *      <li> MachineLossMaxLeft - Maximum allowable machine loss for the left rollers. </li>
     *      <li> MachineLossMinLeft - Minimum allowable machine loss for the left rollers. </li>
     *      <li> MachineLossMaxRight - Maximum allowable machine loss for the right rollers. </li>
     *      <li> MachineLossMinRight - Minimum allowable machine loss for the right rollers. </li>
     *      <li> MachineLossDiffMax - Maximum allowable change in loss values from the previous loss 
     *                                compensation sequence. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> IcmForceArray - Name of the force array to use for calculating the machine losses. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetLCStatus() </li>
     *      <li> CheckAbort() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> Disengage() </li>
     *      <li> SampleForces() </li>
     *      <li> CalculateResults() </li>
     *      <li> GetParameter() </li>
     *      <li> Timestamp() </li>
     *      <li> UpdateResult() </li>
     *      <li> Disengage() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of sampling the roller losses during the coast down period.
     */
    virtual const string TestStepLossCompensation(void);

    /**
     * Bring the rollers to zero speed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * This test step is responsible for bringing the machine rollers to a complete stop.  This test step is only
     * applicable on an electric machine.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineType3600 - System tag for determining if the current machine is an electric machine. </li>
     *      <li> MachineType - System tag for reading the current machine type. </li>
     *      <li> MachineType3600Value - Value indicating the machine is an electric machine. </li>
     *      <li> Zerospeed System tag for determining if the rollers are at zero speed. </li>
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
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> TimeRemaining() </li>
     *      <li> CheckAbort() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @return The result of stopping the rollers.
     */
    virtual const string TestStepStop(void);

    /**
     * Wait for the rollers to caost to a stop.
     * 
     * @return Result of coasting to stop.
     */
    const string CoastToStop(void);

    /**
     * Finish up the loss compensation test sequence.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * This test step is responsible for setting the machine configuration for normal operation.  It also determines 
     * the overall result of the test sequence.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendOverallResult() </li>
     *      <li> SetStatus() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> Disengage() </li>
     *      <li> TimeRemaining() </li>
     *      <li> SystemCommand() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     * 
     * @return The result of performing the finish up processing.
     */
    virtual const string TestStepFinishUp(void);

    /**
     * Perform a low torque startup.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Performs a low torque startup on the drive with the given index.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineType3600 - System tag for indicating the machine is an electric machine. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> GetRollerCount() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> StatusSleep() </li>
     *      </ul>
     * 
     * @param rollIdx   Index of the drive to startup.
     * @param torqueVal Amount of torque to apply to the roller to get it moving.
     * @param maxSpeed  The speed the roll must reach for the startup to be successful.
     * @param timeOut   The amount of time to wait for the roller to get up to the maxSpeed value.
     *
     * @return The status of performing the low torque startup on the specified roller.
     */
    INT32 LowTorqueStartup( INT32 rollIdx, float torqueVal, float maxSpeed, INT32 timeOut);

#if 0
    /**
     * <b>Description:</b>
     * Get the instantaneous forces for all wheels.
     *
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> LeftFrontForce </li>
     *     <li> RightFrontForce </li>
     *     <li> LeftRearForce </li>
     *     <li> RightRearForce </li>
     *     <li> LeftTandemForce </li>
     *     <li> RightTandemForce </li>
     *     </ul>
     * 
     * @param wheelForce The array for storage of the wheel forces.
     * 
     * @return The result of the operation.
     * 
     * @see BepDefs.h "Function return type" for a list of return values.
     */
    INT32 GetIForces(float wheelForce[]);

    /**
     * <b>Description:</b>
     * Get the instantaneous speed for each of the wheels.
     *
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> LeftFrontSpeed </li>
     *     <li> RightFrontSpeed </li>
     *     <li> LeftRearSpeed </li>
     *     <li> RightRearSpeed </li>
     *     <li> LeftTandemSpeed </li>
     *     <li> RightTandemSpeed </li>
     *     </ul>
     * 
     * @param wheelSpeed The array for storage of the wheel speeds.
     * 
     * @return The result of the operation.
     * 
     * @see BepDefs.h "Function return type" for a list of return values.
     */
    INT32 GetISpeeds(float wheelSpeed[]);
#endif 

    /**
     * Check if the wheel speed has reached the specified speed.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The current roller speeds will be checked to determine if the specified roller speed has been obtained.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetWheelSpeeds() </li>
     *      </ul>
     *
     * @param speed  The target roller speed.
     * @param faster Condition to check for.  A value of <i>true</i> will check for the roller speeds to be at or 
     *               above the specified roller speed.  A value of <i>false</i> will check for the roller speeds to
     *               be below the specified speed.
     * 
     * @return The result of checking the roller speeds against the specified target speed.
     */
    INT32 SpeedCheck(float speed, bool faster);

    /**
     * Analyze the machine losses.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The roller forces will be analyzed to determine the new machine losses.  The calculated losses will then
     * be analyzed to determine if the loss compensation sequence was successful.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> <i><roller></i>CurrentLossBGColor - System tag for setting the background color for the current
     *                                               machine loss for <i>roller</i>. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadDataArrays() </li>
     *      <li> GetRollerCount() </li>
     *      <li> AnalyzeForces() </li>
     *      <li> ValidateLoss() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     * 
     * @param array     The name of the array to use for calculating the results.
     * @param firstTag  The offset in the ICM data where the sampling started.
     * @param secondTag The offset in the ICM data where the sampling stopped.
     * 
     * @return The result of calculating the new machine losses.
     */
    INT32 CalculateResults(const std::string &array, int firstTag, int secondTag);

    /**
     * Verify the loss value for the specified wheel.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Validate that the wheel loss was within the acceptable limits.  This is based on the previous loss and the 
     * parameters provided in the configuration file for the test step.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> MachineLossDiffMax - Maximum change in machine loss allowed. </li>
     *      <li> MachineLossMaxRight - Maximum machine loss permitted for the right rollers. </li>
     *      <li> MachineLossMinRight - Minimum machine loss permitted for the right rollers. </li>
     *      <li> MachineLossMaxLeft - Maximum machine loss permitted for the left rollers. </li>
     *      <li> MachineLossMinLeft - Minimum machine loss permitted for the left rollers. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> <i><roller></i>MaxLossDiff - Current loss value change for <i>roller</i> is above the maximum 
     *                                        difference permitted. </li>
     *      <li> <i><roller></i>MachLossOutOfLimits - Current loss for <i>roller</i> is above/below allowable limits for 
     *                                                <i>roller</i>. </li>
     *      <li> <i><roller></i>CurrentLossBGColor - System tag for setting the background color for the current
     *                                               machine loss result for <i>roller</i>. </li>
     *      <li> <i><roller></i>MachineLossResult - Tag use to report the machine loss result for <i>roller</i>. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> SetFault() </li>
     *      <li> ClearFault() </li>
     *      <li> GetDataTag() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      </ul>
     * 
     * @param wheel        The wheel to validate.
     * @param loss         The loss that was measured.
     * @param previousLoss The previous loss that was measured.
     * 
     * @return The result of validating the loss for the specified wheel is within limits.
     */
    virtual INT32 ValidateLoss(int wheel, float loss, float previousLoss);

    /**
     * Analyze the loss data.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The loss data values will be analyzed to determine if any of the encoders have excessive runout or if the
     * encoder signal is noisy.  Either of these could indicate an issue with either the alignment of the encoder
     * or that the encoder is failing and should be replaced.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> RunoutDifference - Maximum runout difference allowed. </li>
     *      <li> RunoutErrorMultiplier - Multiplier to apply to the runout value. </li>
     *      <li> NoiseDifference - Maximum noise error allowed. </li>
     *      <li> NoiseMaxError - Maximum number of noise error counts allowed. </li>
     *      <li> RunoutSamplesPerAverage - Number of samples to include in the runout analysis. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> <i><roller></i>EncoderNoiseError - Encoder noise has been detected on the <i>roller</i> encoder. </li>
     *      <li> <i><roller></i>EncoderRunoutError - Runout error is present on the <i>roller</i> encoder. </li>
     *      <li> <i><roller></i>MachineLoss - System tag for reporting the current machine loss value for 
     *                                        <i>roller</i>. </li>
     *      <li> <i><roller></i>CurrentLoss - System tag for displaying the current machine loss value on the display
     *                                        for <i>roller</i>. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> SetFault() </li>
     *      <li> ClearFault() </li>
     *      <li> GetDataTag() </li>
     *      <li> SystemWrite() </li>
     *      <li> SystemRead() </li>
     *      </ul>
     * 
     * @param wheel   The wheel to analyze.
     * @param average The average value of the force array.
     * 
     * @return The result of analyzing the loss data for the specified roller.
     */
    virtual INT32 AnalyzeForces(int wheel, float &average);

    /**
     * Sample the roller forces.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This will wait for the start sample speed to be reached and store the force array index value and then wait 
     * for the end sample speed to be reached and store the force array index value.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> StartSampleSpeed - Speed at which to start sampling roller forces. </li>
     *      <li> StopSampleSpeed - Speed at which to stop sampling roller forces. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> LossCompStatusCheck() </li>
     *      <li> SpeedCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> TagArray() </li>
     *      </ul>
     * 
     * @param firstTag  The index of the force array when the start sample speed was reached.
     * @param secondTag The index of the force array when the end sample speed was reached.
     * 
     * @return The status of sampling the roller forces.
     */
    virtual INT32 SampleForces(INT32 &firstTag, INT32 &secondTag);

    /**
     * Tag the force array.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Tag the ICM data arrays and retun the index of the tag.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TagIndex - System tag to use for tagging the force array. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemCommand() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetSystemValue() </li>
     *      </ul>
     * 
     * @param arrayTag The tag to use in the data array for noting the data sample.
     * 
     * @return The index of the tag that was placed in the ICM arrays.
     */
    INT32 TagArray(const string arrayTag);

    /**
     * Get the status of the loss compensation status.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The testing status of the loss compensation sequence will be returned to the calling function.
     * <p>
     * @return The status of the loss compensation sequence.
     */
    const string GetLCStatus(void);

    /**
     * Update the testing status.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The testing status will be updated and stored in a member variable.
     * <p>
     * @param status The status to update the component overall result with.
     */
    void SetLCStatus(const string status);

    /**
     * Update the system with the loss compensation time and date.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the time and date of the calibration and update the system with it.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineLossTime - System tag for reporting the loss compensation time. </li>
     *      <li> MachineLossDate - System tag for reporting the loss compensation date. </li>
     *      <li> LossCompensationCurrentTime - System tag for reporting the loss compensation time. </li>
     *      <li> LossCompensationCurrentDate - System tag for reporting the loss compensation date. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     */
    void Timestamp(void);
    /**
     * Setup the loss compensation data.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The current and previous loss data will be read from the system.  The current values will be reset and all data
     * displayed on the screen for the user.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> <i><roller></i>MachineLoss - Current loss value for <i>roller</i>. </li>
     *      <li> <i><roller></i>PreviousLoss - Previous loss value for <i>roller</i>. </li>
     *      <li> <i><roller></i>CurrentLoss - System tag to use for displaying the current loss for <i>roller</i>. </li>
     *      <li> <i><roller></i>PreviousLossBGColor - System tag for setting the previous loss background color for
     *                                                <i>roller</i>. </li>
     *      <li> <i><roller></i>CurrentLossBGColor - System tag to use for the current loss background color for
     *                                               <i>roller</i>. </li>
     *      <li> MachineLossTime - System tag for the time of the current loss compensation sequence. </li>
     *      <li> LossCompensationPreviousTime - System tag for the time of the previous loss compensation sequence. </li>
     *      <li> MachineLossDate - System tag for the date of the current loss compensation sequence. </li>
     *      <li> LossCompensationPreviousDate - System tag for the date of the previous loss compensation sequence. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> SystemReadInt() </li>
     *      <li> SystemRead() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     */
    void SetupLossData(void);
    /**
     * Wheel array that is used to store the force information from a loss compensation cycle.
     */
    WHEELDATAARRAY m_wheelForceArray;

#if 0
    /** List of tags that are used to retrieve the machine roller factors. */
    vector< string > m_machineRollerFactor;
#endif

    /** Machine loss tags and values for the system. */
    LOSSLIST m_currentLosses;

    /** Previous machine loss tags and values in the system. */
    LOSSLIST m_previousLosses;

    /** The status of the LossCompensationTC test. */
    string m_lcStatus;
};
#endif
