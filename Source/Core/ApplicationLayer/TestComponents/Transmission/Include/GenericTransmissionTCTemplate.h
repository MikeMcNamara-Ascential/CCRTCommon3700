//******************************************************************************
// Description:
//  Generic Transmission Test Component.
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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Transmission/Include/GenericTransmissionTCTemplate.h $
// 
// 2     11/08/07 10:40a Derickso
// Updated doxygen comments.
// 
// 1     11/02/07 10:25a Derickso
// Added WaitForFullLockWithThrottleGraph.
// 
// 2     10/01/07 2:02p Derickso
// Added WaitForFullLockWithThrottleGraph.
// 
// 14    2/22/07 10:54a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added KeyOffEngineOffKeyOn().
// - Added AccelerateInReverse().
// 
// 13    1/17/07 3:49p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added WarmInitialize().
// - Removed KeyOffEngineOffKeyOn().
// 
// 12    9/05/06 3:59p Rwiersem
// Changes for the 06090601 core release:
// 
// - Made CommonTransmissionTest a friend class
// - Removed QuickLearnAuthentication()
// - Added KeyOffEngineOffKeyOn()
// - Added WaitForEngineOffIgnitionOff()
// 
// 11    5/08/06 7:15p Cward
// correct bugs in Evaluate Quick Learn, and Monitor Quick Learn
// Procedure, and add authentication start stop method
//
// 10    2/15/06 7:37p Cward
// Added MonitorKickDown and StopQuickLearnProcesure.
//
// 9     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 8     9/01/04 9:25p Cward
// removed unnecessary include.
//
// 7     8/18/04 11:18p Cward
// updated includes using cpp files for template classes
//
// 6     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 5     5/25/04 4:10p Cward
// Added new test steps
//
// 4     3/04/04 5:05p Cward
// Removed ShortCircuitTestStep method. Changed conditional logic for
// skipping test steps. Removed old unused code.
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 7     11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
//
// 6     11/13/03 6:12p Derickso
// Latest updates from testing at JTE.
//
// 5     10/20/03 9:50a Derickso
// Periodic update.
//
// 4     9/22/03 4:49p Derickso
// Corrected copy/paste errors.
//
// 3     9/18/03 8:39a Derickso
// Updated member variable name analyze to m_analyze
//
// 2     8/27/03 7:31a Derickso
// Corrected compiler errors.
// Compiles, not tested.
//
// 1     8/21/03 9:00a Derickso
// 05WK CCRT Project
//
// 6     8/20/03 7:18a Derickso
// Recombined cpp and h files.  Compiler does not support implementation
// being seperate for template classes.
//
// 5     8/19/03 9:58a Derickso
// Updated class names adding Template to end of Generic name. This will
// avoid confilicts with earlier software.
//
// 1     8/13/03 3:12p Derickso
// Split implementation into cpp file.
// Renamed to shorten TestComponent to TC
//
// 1     5/27/03 3:32p Derickso
// Periodic backup.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericTransmissionTCTemplate_h
#define GenericTransmissionTCTemplate_h
//==============================================================================

#include "GenericTCTemplate.cpp"
#include "DataAnalysis.h"


/**
 * Generic transmission test component.
 * This class contains all the test steps that are common to all
 * transmission testing.
 *
 * @author Dan Erickson
 * @see GenericTCTemplate<VehicleModuleType>
 * @see GenericModuleTemplate
 * @see GenericTransmissionModuleTemplate
 * @since May 25, 2003
 */
template <class VehicleModuleType>
class GenericTransmissionTCTemplate : public GenericTCTemplate<VehicleModuleType>
{
    friend class CommonTransmissionTest;

public:
    /**
     * Class constructor.
     */
    GenericTransmissionTCTemplate();

    /**
     * Class Destructor
     */
    virtual ~GenericTransmissionTCTemplate();

    /**
     * Reinitialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reinitialize using the already loaded configuration file. This is intended to be used when a component is 
     * activated and there have been no modifications to any configuration files used by the component.
     * Member variables that store status during the test will also be reset to initial values.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTCTemplate::WarmInitialize() </li>
     *      </ul>
     */
    virtual void WarmInitialize();
    
    /**
	 * Sequence a test step.
	 * <p><b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * This is the method that will handle the execution of each test step that has been specified by the system for each
	 * test objective that is passed to it.  
	 * <br><p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> SpeedRange - Target speed range for wiating for third gear. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ReverseSpeedMinimum - Minimum reverse speed required for the reverse speed test. </li>
     *      <li> ReverseSpeedMaximum - Maximum reverse speed allowed for the reverse speed test. </li>
     *      </ul>
     * 
	 * <b>Test Step Functions Called:</b>
	 * 		<ul>
     *      <li> CheckLimpInStatus() </li>
     *      <li> ReadEOLStatusBytes() </li>
     *      <li> CheckUplineStatus() </li>
     *      <li> ClearDiagnosticTellTale() </li>
     *      <li> ClearOverallStatus() </li>
     *      <li> EvaluateQuickLearn() </li>
     *      <li> EvaluateSpeedRationality() </li>
     *      <li> MonitorOverdriveLockout() </li>
     *      <li> MonitorKickDown() </li>
     *      <li> WaitForGears() </li> 
     *      <li> WaitForGearsAndLockup() </li>
     *      <li> WaitForThirdGear() </li>
     *      <li> ResetTestStatus() </li>
     *      <li> StartFaultLimpInMonitor() </li>
     *      <li> StartGearLockUpMonitor() </li>
     *      <li> StartGearMonitor() </li>
     *      <li> StartLockUpMonitor() </li>
     *      <li> StopFaultLimpInMonitor() </li>
     *      <li> StopGearLockUpMonitor() </li>
     *      <li> StopGearMonitor() </li>
     *      <li> StopLockUpMonitor() </li>
     *      <li> CheckAutostickBits() </li>
     *      <li> CheckShiftLever() </li>
     *      <li> AccelerateInReverse() </li>
     *      <li> StartShiftLeverMonitor() </li>
     *      <li> StopShiftLeverMonitor() </li>
     *      <li> StopQuickLearnProcedure() </li>
	 * 		</ul>
	 * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GenericTCTemplate::CommandTestStep() </li>
     *      </ul>
     * 
	 * @param value    This is the value that was commanded by the test sequencer.
	 * @return  The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);


protected:
    /**
     * Stop any background monitors that have been started.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * All background monitors that have been started will be commanded to stop.  This is typically done when 
     * communication timing to the module is critical such as during ABS valve firing.  The following monitors will be
     * commanded to stop:
     * <ul>
     * <li> Gear Monitor </li>
     * <li> TCC Lock Up Monitor </li>
     * <li> Limp-in and Fault Monitor </li>
     * <li> Shift Lever Monitor </li>
     * </ul>
     * <b><i>Note:</i></b>  No checking is performed to ensure the background monitor has observed all states it 
     *                      has monitored. This method is intended to stop any monitors in the event a test abort
     *                      was received. This will prevent monitors from attempting communications after the 
     *                      Vehicle has left.
     * <p>
     * @return Result of stopping the background monitors.
     */
    virtual inline string StopBackgroundMonitors(void);

    /**
     * Restart any background monitors that were stopped with the StopBackgroundMonitors method.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Any background monitor commanded to stop will be resumed.  The following monitors will be restarted:
     * <ul>
     * <li> TCC Lock Up Monitor </li>
     * <li> Limp-in and Fault Monitor </li>
     * <li> Shift Lever Monitor </li>
     * </ul>
     * <b><i>Note:</i></b>  If any background monitors perform any special setup or store initial values, care should be 
     *                      taken to not lose the initial values that were store when the monitor was initially started.
     * <p>
     * @return Result of restarting the background monitors.
     */
    virtual inline string ResumeBackgroundMonitors(void);

    /**
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Class specific initialization will be performed.  The base class will first be called to begin the initialization.
     * Status byte information will be stored as well as the list of
     * upline tests to be checked.  The clutch volume tags will also be stored.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ResetEOLStatusBytes - EOL status bytes to be reset at the beginning of the test sequence so test status
     *                                 can be tracked. </li>
     *      <li> WriteEOLStatusByteValues - EOL status bytes to force to a completed status during development testing. </li>
     *      <li> UplineTestsToCheck - List of upline tests to check to ensure upline testing has been completed. </li>
     *      <li> ClutchVolumeTags - List of tags to use for reporting clutch volumes. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTCTemplate::InitializeHook() </li>
     *      </ul>
     *
     * @param config the test component's configuration data.
     */
    virtual void InitializeHook(const XmlNode *config);

    /**
     * Check to ensure the transmission does not have a Limp-In condition.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The limp-in status will be read from the module.  If the transmission is in a limp-in condition, the test 
     * sequence will be aborted and a prompt displayed to the operator.  Aborting the test will limit the possibility
     * of damaging the transmission.  The test sequence will also be aborted if the limp-in status cannot be read from 
     * the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadLimpInStatus - Message tag to use for reading limp-in status from the module. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> LimpInMode - Transmission is in limp-in mode. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> LimpInMode - Transmission is in limp-in mode. </li>
     *      <li> CommunicationFailure - Could not read limp-in status from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to read the limp-in status from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data Item: ReadLimpInStatus </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of checking the transmission for limp-in status.
     */
    virtual string CheckLimpInStatus(void);

    /**
     * Reset the rolls test affected bits in the EOL Status registers.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The EOL status bytes will be read from the module.  The roll test affected bits in each status byte will then
     * be reset to a "Not Complete" state and written back to the module.  This will aid in tracking test status and 
     * tests performed.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> FirstEOLStatusRegister - Index to the first EOL status register. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadEOLStatusBytes - Message tag to use for reading the EOL status bytes from the module. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read/write status bytes from/to the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to update the EOL status bytes in the
     *                             module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data Item: ReadEOLStatusBytes </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameter() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of resetting the rolls affected EOL status byte to a "Not Complete" state.
     */
    virtual string ResetTestStatus(void);

    /**
     * Start the background monitor to monitor for faults and limp-in status.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The fault and limp-in status monitor will be started.  The list of ignored faults will be passed to the monitor
     * so it can ignore appropriate faults and not abort the test sequence for faults that are supposed to be ignored.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommuncationFailure - Could not start the background monitor. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to start the background monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StartLimpInFaultMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return Result of starting the fault and limp-in monitor.
     */
    virtual string StartFaultLimpInMonitor(void);

    /**
     * Stop the background monitor that is monitoring faults and limp-in status.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The background monitor for faults and limp-in status will be commanded to stop monitoring data from the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not stop the background monitor. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to stop the background monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StopLimpInFaultMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of stopping the fault and limp-in status monitor.
     */
    virtual string StopFaultLimpInMonitor(void);

    /**
     * Start the background monitor to monitor for all gears and torque converter clutch lock up states.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The background monitor for transmission gears and torque converter clutch lock up states will be started.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommuncationFailure - Could not start the background monitor. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to start the background monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StartGearLockUpMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return Result of starting the background monitor.
     */
    virtual string StartGearLockUpMonitor(void);

    /**
     * Stop the background monitor that is monitoring for all gears and torque converter clutch lock up states.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The transmission gear and torque converter clutch lock up monitor will be commanded to stop monitoring data.
     * The data gathered by the monitor will be be checked to ensure all required transmission gears and torque
     * converter lock up states have been observed.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CheckForPark - Flag indicating if Park gear is a required gear for the current transmission. </li>
     *      <li> UnlockStateNotRequired - Flag indicating the TCC Unlock state is not required. </li>
     *      <li> PartialLockNotRequired - Flag indicating the TCC Partial lock state is not required. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> AllGearsLockStatesNotSeen - Did not observe all required gears or TCC lockup states. </li>
     *      <li> ParkNotSeen - Did not observe transmission in park gear. </li>
     *      <li> ReverseGearNotSeen - Did not observe transmission in reverse gear. </li>
     *      <li> NeutralGearNotSeen - Did not observe transmission in neutral gear. </li>
     *      <li> FirstGearNotSeen - Did not observe transmission in first gear. </li>
     *      <li> SecondGearNotSeen - Did not observe transmission in second gear. </li>
     *      <li> ThirdGearNotSeen - Did not observe transmission in third gear. </li>
     *      <li> FourthGearNotSeen - Did not observe transmission in fourth gear. </li>
     *      <li> FifthGearNotSeen - Did not observe transmission in fifth gear. </li>
     *      <li> SixthGearNotSeen - Did not observe transmission in sixth gear. </li>
     *      <li> TCCNoLockNotSeen - Did not observe TCC no lock state. </li>
     *      <li> TCCPartialLockNotSeen - Did not observe TCC Partial lock state. </li>
     *      <li> TCCFullLockNotSeen - Did not observe TCC full lock state. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to stop the background monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StopGearLockUpMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> CheckParkGearResults() </li>
     *      <li> CheckReverseGearResults() </li>
     *      <li> CheckNeutralGearResults() </li>
     *      <li> CheckForwardDrivingGearResults() </li>
     *      <li> CheckTCCNoLockState() </li>
     *      <li> CheckTCCPartialLockState() </li>
     *      <li> CheckTCCFullLockState() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of stopping the gear and lock up state monitor.
     */
    virtual string StopGearLockUpMonitor(void);

    /**
     * Start the background monitor to monitor for all gears.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The background data monitor for all required transmission gears will be started.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommuncationFailure - Could not start the gear monitor. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to start the gear monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions: </b>
     *      <ul>
     *      <li> StartGearMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of starting the transmission gear monitor.
     */
    virtual string StartGearMonitor(void);

    /**
     * Stop the background gear monitor.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The data gathered by the gear monitor will be analyzed to determine if all required transmission gears have been
     * observed.  The background monitor will then be commanded to stop and the results reported.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CheckForPark - Flag to determine if park is a required gear for the current transmission. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> AllGearsNotSeen - Did not detect all required transmission gears. </li>
     *      <li> ParkNotSeen - Did not observe transmission in park gear. </li>
     *      <li> ReverseGearNotSeen - Did not observe transmission in reverse gear. </li>
     *      <li> NeutralGearNotSeen - Did not observe transmission in neutral gear. </li>
     *      <li> FirstGearNotSeen - Did not observe transmission in first gear. </li>
     *      <li> SecondGearNotSeen - Did not observe transmission in second gear. </li>
     *      <li> ThirdGearNotSeen - Did not observe transmission in third gear. </li>
     *      <li> FourthGearNotSeen - Did not observe transmission in fourth gear. </li>
     *      <li> FifthGearNotSeen - Did not observe transmission in fifth gear. </li>
     *      <li> SixthGearNotSeen - Did not observe transmission in sixth gear. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to stop the gear monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StopGearMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> CheckParkGearResults() </li>
     *      <li> CheckReverseGearResults() </li>
     *      <li> CheckNeutralGearResults() </li>
     *      <li> CheckForwardDrivingGearResults() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of the test step.
     */
    virtual string StopGearMonitor(void);

    /**
     * Start the background torque converter clutch lockup monitor.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The background monitor for torque converter lockup states will be started.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommuncationFailure - Could not start the background monitor. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to start the background monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StartLockupMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return Result of starting the torque converter clutch lock up state monitor.
     */
    virtual string StartLockUpMonitor(void);

    /**
     * Stop the background torque converter clutch lockup monitor.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The data gathered by the TCC lockup monitor will be analyzed to determine if all required lockup states have been
     * observed.  The background monitor will then be stopped and the results reported.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> UnlockStateNotRequired - Flag indicating the TCC Unlock state is not required. </li>
     *      <li> PartialLockNotRequired - Flag indicating the TCC Partial lock state is not required. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> AllLockupStatesNotSeen - Failed to observe all required TCC lock states. </li>
     *      <li> TCCNoLockNotSeen - Did not observe TCC no lock state. </li>
     *      <li> TCCPartialLockNotSeen - Did not observe TCC Partial lock state. </li>
     *      <li> TCCFullLockNotSeen - Did not observe TCC full lock state. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to stop the background monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StopLockupMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> CheckTCCNoLockState() </li>
     *      <li> CheckTCCPartialLockState() </li>
     *      <li> CheckTCCFullLockState() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * @return Result of stopping the TCC lockup monitor.
     */
    virtual string StopLockUpMonitor(void);

    /**
     * Perform a speed rationality check for the transmission.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The transmission Vehicle speed will be compared to the roller speeds to ensure the proper pinion factor and 
     * other data was programmed into the Vehicle.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between Vehicle speed checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SpeedRationalityMinimumSpeed - Minimum speed required to perform the speed rationality test. </li>
     *      <li> SpeedRationalityMaximumSpeed - Maximum speed allowed to perform the speed rationality test. </li>
     *      <li> SpeedTolerance - Allowed speed deviation. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> SpeedRationalityFail - Vehicle speed is out of limits with the roller speeds. </li>
     *      <li> CommunicationError - Could not read the Vehicle speed from the module. </li>
     *      <li> SpeedRationalityTimeout - Timeout waiting for the operator to acclerate to the required speed range. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while checking the speed rationality. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> WaitAndPromptForSpeed() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetRollSpeed() </li>
     *      <li> ReadVehicleSpeed() </li>
     *      <li> GetRollSpeed() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of the speed rationality test.
     */
    virtual string EvaluateSpeedRationality(void);

    /**
     * Read the Vehicle speed from the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The Vehicle speed will be read from the module and returned to the calling function in the argument 
     * <i>vehicleSpeed</i>.  The status of reading the Vehicle speed from the module will be returned to the calling
     * function.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadVehicleSpeed - Message tag to use for reading Vehicle speed from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data item: ReadVehicleSpeed </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @param vehicleSpeed  Current Vehicle speed read from the module.
     * @return  Status of the Vehicle speed read.
     */
    virtual BEP_STATUS_TYPE ReadVehicleSpeed(float &vehicleSpeed);

    /**
     * Clear diagnostic tell tales from the module.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The diagnostic tell tales will be cleared.  Most of the information is in regards to fault recording internal to
     * the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not command the module to clear diagnostic tell tales. </li>
     *      <li> SoftwareFailure - AModuleException occurred while attempting to clear the diagnostic tell tales. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ClearDiagnosticRetention() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of clearing diagnostic tell tales from the module.
     */
    virtual string ClearDiagnosticTellTale(void);

    /**
     * Determine if all upline test have been completed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The upline status bytes will be read from the module.  If any of the upline tests listed in the config data have
     * not been completed, the test will be aborted.  The test is aborted to prevent the transmission from being damaged
     * due to improper setup.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> UplineRegisterMask - Mask to apply to the upline register contents to determine all required upline 
     *                                testing has been completed. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadUplineRegister - Message tag to use to read the upline test status from the module. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> UplinesNotComplete - Upline testing has not been completed. </li>
     *      <li> CommunicationFailure - Could not read the upline status register from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attmepting to read the upline register from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data item: ReadUplineRegister </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameter() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of checking the upline test status.
     */
    virtual string CheckUplineStatus(void);

    /**
     * Check transmission kickdown.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The operator will be prompted to accelerate to the required speed range.  Once the required speed range has been 
     * achieved, the operator will be instructed to perform the kick down test and enter the result of the test.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> KickDownSpeedMin - Minimum speed required to perform the transmission kickdown test. </li>
     *      <li> KickDownSpeedMax - Maximum speed allowed to perform the transmission kickdown test. </li>
     *      <li> KickDownSPeedCheckDelay - Amount of time(ms) between speed checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> PerformKickDown - Perform the transmission kickdown test. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> WaitAndPromptForSpeed() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameterInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> SetupOperatorResponses() </li>
     *      <li> GetOperatorInput() </li>
     *      <li> GetPrompt() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of checking the transmission kickdown feature.
     */
    virtual string MonitorKickDown(void);

    /**
     * Verify the overdirve lockout switch kicks the transmission down to 3rd gear when the OD Cancel button is selected.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The operator will be prompted to accelerate to the target speed range.  Once the required speed range has been
     * achieved, the overdrive lock out status and transmission gear kickdown will be monitored to ensure the 
     * transmission gear kicks down when the overdrive is canceled.  The operator will then be instructed to enable
     * the overdirve; however, no checks are performed to ensure overdrive has been re-enabled in the transmission.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parmaeters:</b>
     *      <ul>
     *      <li> EnableOverdrivePromptDelay - Amount of time(ms) to display the enable over drive prompt. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> SelectOD - Enable overdrive. </li>
     *      <li> MaintainSpeed - Maintain current speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read overdrive status from the module. </li>
     *      <li> ODCancelButtonTimeout - Timeout waiting for the operator to press the OD Cancel button. </li>
     *      <li> ODCancelGearTimeout - Timeout waiting for the transmission gear to kick down. </li>
     *      <li> PreconditionTimeout - Timeout waiting for overdrive gear in the transmission. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while monitoring overdrive lockout in the transmission. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> WaitForOverdriveConditions() </li>
     *      <li> WaitForOverdriveCancelButton() </li>
     *      <li> WaitForOverdriveGearKickdown() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * @return Result of verifying the transmission gear kicks down when the overdrive is canceled.
     */
    virtual string MonitorOverdriveLockout(void);

    /**
     * Wait for all transmission gears to be observed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><p>
     * The operator will be prompted to accelerate to the indicated speed range in an effort to observe all required
     * forward driving gears in the transmission.  The test step will wait for all forward driving gears to be
     * observed within a specified amount of time.  If all gears have not been observed, a failure will be reported
     * only to the test result server.  The function will return a test pass, as this is mainly used in an effort to
     * prevent a re-acceleration to speed when the gear monitor is stopped at the end of the test.  Any gear not 
     * observed during this test step can potentially be observed during other portions of the test.  When the gear
     * monitor is stopped, it will prompt the operator to accelerate in an effort to detect any missed gears.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> OverdriveGear - Overdrive gear in the current transmission. </li>
     *      <li> MaxGearSpeedRange - Speed range for transmission to achieve maximum gear. </li>
     *      <li> FirstGearStatusRegister - Status register to use for storing the first gear observed result. </li>
     *      <li> FirstGearBit - Bit to use for recording the first gear observed result. </li>
     *      <li> SecondGearStatusRegister - Status register to use for recording the second gear observed result. </li>
     *      <li> SecondGearBit - Bit to use for recording the second gear observed result. </li>
     *      <li> ThirdGearStatusRegister - Status register to use for recording the third gear observed result. </li>
     *      <li> ThirdGearBit - Bit to use for recording the third gear observed result. </li>
     *      <li> FourthGearStatusRegister - Status register to use for recording the fourth gear observed result. </li>
     *      <li> FourthGearBit - Bit to use for recording the fourth gear observed result. </li>
     *      <li> FifthGearStatusRegister - Status register to use for recording the fifth gear observed result. </li>
     *      <li> FifthGearBit - Bit to use for recording the fifth gear observed result. </li>
     *      <li> SixthGearStatusRegister - Status register to use for recording the sixth gear observed result. </li>
     *      <li> SixthGearBit - Bit to use for recording the sixth gear observed result. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag used to display the target speed range on the display. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed range. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> TestStepTimeout - Timeout waiting to observe all forward driving gears. </li>
     *      <li> StatusCheckFailure - System status precludes any further testing. </li>
     *      <li> AllGearsAllLocksNotSeen - All required forward driving transmission gears not observed. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetGear5String() </li>
     *      <li> GetGear6String() </li>
     *      <li> Gear1Observed() </li>
     *      <li> Gear2Observed() </li>
     *      <li> Gear3Observed() </li>
     *      <li> Gear4Observed() </li>
     *      <li> Gear5Observed() </li>
     *      <li> Gear6Observed() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SetStartTime() </li>
     *      <li> SetFirstGearUpdateResult() </li>
     *      <li> GetFirstGearUpdateResult() </li>
     *      <li> SetSecondGearUpdateResult() </li>
     *      <li> GetSecondGearUpdateResult() </li>
     *      <li> SetThirdGearUpdateResult() </li>
     *      <li> GetThirdGearUpdateResult() </li>
     *      <li> SetFourthGearUpdateResult() </li>
     *      <li> GetFourthGearUpdateResult() </li>
     *      <li> SetFifthGearUpdateResult() </li>
     *      <li> GetFifthGearUpdateResult() </li>
     *      <li> SetSixthGearUpdateResult() </li>
     *      <li> GetSixthGearUpdateResult() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * @return Pass
     */
    virtual string WaitForGears(void);

    /**
     * Wait for the Torque Converter Clutch to fully lockup.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The operator will be prompted to accelerate to the required speed range.  The function will first wait for all
     * required forward driving gears to be observed.  It will then monitor torque converter clutch states until all
     * required lockup states have been observed or time expires.  This test step always returns "Pass" since the
     * operator will be prompted to accelerate the Vehicle if all gears and TCC lock up states have not been observed
     * by the time the gear and lock up monitors are stopped.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> UnlockStateNotRequired - Flag to indicate if the unlock state is required. </li>
     *      <li> PartialLockNotRequired - Flag to indicate if the partial lock state is required. </li>
     *      <li> MaxGearSpeedRange - Speed range to use for getting the transmission into maximum gear. </li>
     *      <li> SkipUpdateTccNoLockRegister - Flag used to indicate if the no lock state result should be recorded in
     *                                         the module. </li>
     *      <li> SkipUpdateTccPartialLockRegister - Flag used to indicate if the partial lock state result should be
     *                                              recorded in the module. </li>
     *      <li> SkipUpdateTccFullLockRegister - Flag used to indicate if the TCC full lock result should be recorded 
     *                                           in the module. </li>
     *      <li> TCCNoLockStatusRegister - Status register to use for recording the TCC no lock observed result. </li>
     *      <li> TCCNoLockTestBit - Bit to use for recording the TCC no lock result. </li>
     *      <li> TCCPartialLockStatusRegister - Status register to use for recording the TCC partial lock observed result. </li>
     *      <li> TCCPartialLockTestBit - Bit to use for recording the TCC partial lock result in the module. </li>
     *      <li> TCCFullLockStatusRegister - Status register to use for recording the TCC full lock observed result. </li>
     *      <li> TCCFullLockTestBit - Bit to use for recording the TCC full lock result. </li>
     *      <li> AttemptFullLockWithThrottleGraph - Flag to indicate if a throttle graph should be displayed to aid the
     *                                              operator in achieving TCC full lock up. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the speed range. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range on the display. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> TestStepTimeout - Timeout waiting for all gears and lock states to be observed. </li>
     *      <li> AllGearsAllLocksNotSeen - Failed to observe all TCC lock states. </li>
     *      <li> StatusCheckFailure - System status precludes further testing. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> TCCNoLockupObserved() </li> 
     *      <li> TCCPartialLockupObserved() </li>
     *      <li> TCCFullLockupObserved() </li>
     *      <li> SetAllLockStatesSeen() </li>
     *      <li> AllLockStatesObserved() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> WaitForGears() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> SetUnlockStateUpdateResult() </li>
     *      <li> GetUnlockStateUpdateResult() </li>
     *      <li> SetPartialLockStateUpdateResult() </li>
     *      <li> GetPartialLockStateUpdateResult() </li>
     *      <li> SetFullLockStateUpdateResult() </li>
     *      <li> GetFullLockStateUpdateResult() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> WaitForFullLockWithThrottleGraph() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * @return Pass
     */
    virtual string WaitForGearsAndLockup(void);

    /**
     * Wait for the transmission to shift into third gear.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * This test step simply waits for the transmission to shift into third gear after prompting the operator to 
     * accelerate to the target speed.  No other checks are performed nor are any status bytes updated.  This test step 
     * is intended to facilitate other aspects of the testing sequence.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between checks for transmission in third gear. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag used to display the target speed range for the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> Gear3Observed() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetDataTag() </li>
     *      <li> SystemWrite() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param speedRange Target speed for the operator to achieve.
     *
     * @return Pass
     */
    virtual string WaitForThirdGear(const string speedRange);

    /**
     * Display the throttle graph and wait for the TCC Full Lock State.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to apply a specific amount of throttle to ease TCC full lock engagement.  This is designed
     * to be called after the WaitForGears test step or the WaitForGearsAndLockup test times out.  The operator will be prompted 
     * to achieve the desired speed range if necessary.  Once the desired speed range is achieved, the throttle graph will be 
     * enabled with a TPS target for the operator.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> FullLockTpsRange - TPS meter green band range to display for the TCC full lock. </li>
     *      <li> TpsUpdateDelay - Amount of time in ms to wait between TPS reads from the module. </li>
     *      <li> SkipUpdateTccFullLockRegister - Flag indicating if the full lock state register should be updated. </li>
     *      <li> TCCFullLockStatusRegister - EOL Status register to use for indicating status of full lock state. </li>
     *      <li> TCCFullLockTestBit - Bit to use for indicating the result of the full lock state test. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TPSTarget - System tag for setting the target band on the TPS meter. </li>
     *      <li> TPSActive - System tag to use for activating and deactivating the TPS meter. </li>
     *      <li> TPSValue - Current TPS value to display for the operator. </li>
     *      <li> CurrentTPS - Message tag to use for reading the current TPS. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ThrottleInGreenBand - Apply throttle in green band. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ReadModuleData() - Data item: CurrentTPS </li>
     *      <li> TCCFullLockupObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> SetFullLockStateUpdateResult() </li>
     *      <li> RemovePrompt() </li>
     *      </ul>
     * 
     * @return Result of waiting for the TCC full lock state to be observed.
     */
    string WaitForFullLockWithThrottleGraph(void);
    /**
     * Perform the quicklearn procedure on the transmission.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The transmission oil temperature will first be analyzed to ensure it is in the correct range to successfully
     * perform the quicklearn procedure.  If the temperatur is in the correct range, the operator will be instructed to 
     * shift to Neutral and apply the brake pedal.  The module will then be commanded to perform the quicklearn 
     * procedure.  The quicklearn status will be monitored to determine if the procedure exited abnormally, or if it
     * completed successfully.  The transmission oil temperatur will be read once again at the end of the procedure to
     * verify it is still in the correct temperature range.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AlwaysPerformQuicklearn - Flag to determine if the quicklearn procedure should be performed regardless
     *                                     of previous pass results. </li>
     *      <li> MinimumOilTemperature - Minimum oil temperature required to perform the quicklearn procedure. </li>
     *      <li> MaximumOilTemperature - Maximum oil temperature allowed for performing the quicklearn procedure. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadTransaxleOilTemp - Message tag to use for reading the transmission oil temperature. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> MaintainBrake - Apply the brake pedal. </li>
     *      <li> ShiftToNeutral - Shift the transmission to Neutral. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> TransAxleOilTemperatureOutOfRange - Transmission oil temperature is out of range. </li>
     *      <li> CommunicationFailure - Could not read the transmission oil temperature. </li>
     *      <li> QuickLearnFinishupFail - Could not perform all finish up processing. </li>
     *      <li> QuickLearnMonitorFail - Quicklearn procedure was terminated abnormally. </li>
     *      <li> StartQuickLearnFail - Failed to start the quicklearn procedure. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while performing the quicklearn procedure. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data item: ReadTransaxleOilTemp </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SetQuickLearnStarted() </li>
     *      <li> StartQuickLearnProcedure() </li>
     *      <li> MonitorQuickLearnProcedure() </li>
     *      <li> FinishQuickLearnProcedure() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of performing the quicklearn procedure.
     */
    virtual string EvaluateQuickLearn(void);

    /**
     * Set the overall tranmission test result in the module.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The overall transmission test result will be written to the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> OverallStatusRegister - EOL status register to use for recording the overall transmission test result. </li>
     *      <li> EOLTestStatusBit - Bit to use for recording the transmission test result. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not record the overall transmission test result in the module. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return Result of recording the overall transmission test result in the module.
     */
    virtual string ClearOverallStatus(void);

    /**
     * Check if transmission is in reverse.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The current transmission gear will be read from the module.  The gear read from the module will then be checked
     * in order to determine if the transmission is in reverse gear.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadCurrentGear - Message tag to use for reading the current transmission gear from the module. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ReverseGearTimeout - Timeout waiting for reverse gear. </li>
     *      <li> CommunicationFailure - Could not read the current transmission gear from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while waiting for reverse gear. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data item: ReadCurrentGear </li>
     *      <li> GetGearReverseString() </li>
     *      <li> SetObservedGearReverse() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     * 
     * @param inReverseGear
     *               Flag indicating if the transmission was detected in reverse gear.
     * 
     * @return Status of reading the current transmission gear.
     */
    virtual BEP_STATUS_TYPE ReadReverseGear( bool &inReverseGear );

    /**
     * Determine if park was observed during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if reverse gear was observed during Vehicle testing.  If the gear was not observed, the operator will be 
     * prompted to achieve the required gear.  If required, the result will be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ParkStatusRegister - Status register to use for recording the park gear test result. </li>
     *      <li> ParkBit - Bit to use for recording the park gear result. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToPark - Shift the transmission to Park. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ParkTimeout - Timeout waiting for park gear. </li>
     *      <li> StatusByteUpdateFailure - Could not record the test result in the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GearParkObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckZeroSpeed() </li>
     *      <li> GetParkGearUpdateResult() </li>
     *      <li> SetParkGearUpdateResult() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      </ul>
     *
     * @return Result of checking the park gear results.
     */
    virtual string CheckParkGearResults(void);

    /**
     * Determine if reverse was observed during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if reverse gear was observed during Vehicle testing.  If the gear was not observed, the operator will be 
     * prompted to achieve the required gear.  If required, the result will be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ReverseGearStatusRegister - Status register to use for recording the reverse gear test result. </li>
     *      <li> ReverseGearBit - Bit to use for recording the reverse gear result. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToReverse - Shift the transmission to Reverse. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ReverseGearTimeout - Timeout waiting for reverse gear. </li>
     *      <li> StatusByteUpdateFailure - Could not record the test result in the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GearReverseObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckZeroSpeed() </li>
     *      <li> GetReverseGearUpdateResult() </li>
     *      <li> SetReverseGearUpdateResult() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      </ul>
     *
     * @return Result of checking the reverse gear results.
     */
    virtual string CheckReverseGearResults(void);

    /**
     * Determine if neutral was observed during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if neutral gear was observed during Vehicle testing.  If the gear was not observed, the operator will be 
     * prompted to achieve the required gear.  If required, the result will be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> NeutralGearStatusRegister - Status register to use for recording the neutral gear test result. </li>
     *      <li> NeutralGearBit - Bit to use for recording the neutral gear result. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToNeutral - Shift the transmission to Neutral. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> NeutralGearTimeout - Timeout waiting for neutral gear. </li>
     *      <li> StatusByteUpdateFailure - Could not record the test result in the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GearNeutralObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetNeutralGearUpdateResult() </li>
     *      <li> SetNeutralGearUpdateResult() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      </ul>
     *
     * @return Result of checking the neutral gear results.
     */
    virtual string CheckNeutralGearResults(void);

    /**
     * Determine if all forward driving gears were observed during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The forward driving gear results will be checked to ensure each required forward driving gear was observed.
     * <p>
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> AllGearsNotSeen - Did not observe all forward driving gears. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> CheckFirstGearResult() </li>
     *      <li> CheckSecondGearResult() </li>
     *      <li> CheckThirdGearResult() </li>
     *      <li> CheckFourthGearResult() </li>
     *      <li> CheckFifthGearResult() </li>
     *      <li> CheckSixthGearResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     *
     * @param firstGearResult
     *               Test result for first gear observed.
     * @param secondGearResult
     *               Test result for second gear observed.
     * @param thirdGearResult
     *               Test result for third gear observed.
     * @param fourthGearResult
     *               Test result for fourth gear observed.
     * @param fifthGearResult
     *               Test result for fifth gear observed.
     * @param sixthGearResult
     *               Test result for sixth gear observed.
     * @return Result of checking all forward driving gears.
     */
    virtual string CheckForwardDrivingGearResults(string &firstGearResult, string &secondGearResult,
                                                  string &thirdGearResult, string &fourthGearResult,
                                                  string &fifthGearResult, string &sixthGearResult);

    /**
     * Determine if first gear was observed during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if first gear was observed during Vehicle testing.  If the gear was not observed, the operator will be 
     * prompted to achieve the required gear.  If required, the result will be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> FirstGearSpeedRange - Vehicle speed range for first gear. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> FirstGearStatusRegister - Status register to use for recording the first gear test result. </li>
     *      <li> FirstGearBit - Bit to use for recording the first gear result. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range for the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> FirstGearTimeout - Timeout waiting for first gear. </li>
     *      <li> StatusByteUpdateFailure - Could not record the test result in the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> Gear1Observed() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetFirstGearUpdateResult() </li>
     *      <li> SetFirstGearUpdateResult() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @return Result of checking the first gear results.
     */
    virtual string CheckFirstGearResult(void);

    /**
     * Determine if second gear was observed during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if second gear was observed during Vehicle testing.  If the gear was not observed, the operator will be 
     * prompted to achieve the required gear.  If required, the result will be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> SecondGearSpeedRange - Vehicle speed range for second gear. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SecondGearStatusRegister - Status register to use for recording the second gear test result. </li>
     *      <li> SecondGearBit - Bit to use for recording the second gear result. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range for the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> SecondGearTimeout - Timeout waiting for second gear. </li>
     *      <li> StatusByteUpdateFailure - Could not record the test result in the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> Gear2Observed() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetSecondGearUpdateResult() </li>
     *      <li> SetSecondGearUpdateResult() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @return Result of checking the second gear results.
     */
    virtual string CheckSecondGearResult(void);

    /**
     * Determine if third gear was observed during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if third gear was observed during Vehicle testing.  If the gear was not observed, the operator will be 
     * prompted to achieve the required gear.  If required, the result will be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ThirdGearSpeedRange - Vehicle speed range for Third gear. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ThirdGearStatusRegister - Status register to use for recording the third gear test result. </li>
     *      <li> ThirdGearBit - Bit to use for recording the third gear result. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range for the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ThirdGearTimeout - Timeout waiting for third gear. </li>
     *      <li> StatusByteUpdateFailure - Could not record the test result in the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> Gear3Observed() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetThirdGearUpdateResult() </li>
     *      <li> SetThirdGearUpdateResult() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @return Result of checking the third gear results.
     */
    virtual string CheckThirdGearResult(void);

    /**
     * Determine if fourth gear was observed during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if fourth gear was observed during Vehicle testing.  If the gear was not observed, the operator will be 
     * prompted to achieve the required gear.  If required, the result will be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> FourthGearSpeedRange - Vehicle speed range for fourth gear. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> FourthGearStatusRegister - Status register to use for recording the fourth gear test result. </li>
     *      <li> FourthGearBit - Bit to use for recording the fourth gear result. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range for the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> FourthGearTimeout - Timeout waiting for fourth gear. </li>
     *      <li> StatusByteUpdateFailure - Could not record the test result in the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> Gear4Observed() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetFourthGearUpdateResult() </li>
     *      <li> SetFourthGearUpdateResult() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @return Result of checking the fourth gear results.
     */
    virtual string CheckFourthGearResult(void);

    /**
     * Determine if fifth gear was observed during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if fifth gear was observed during Vehicle testing.  If the gear was not observed, the operator will be 
     * prompted to achieve the required gear.  If required, the result will be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> FifthGearSpeedRange - Vehicle speed range for fifth gear. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> OverdriveGear - Overdrive gear for the current transmission. </li>
     *      <li> FifthGearStatusRegister - Status register to use for recording the fifth gear test result. </li>
     *      <li> FifthGearBit - Bit to use for recording the fifth gear result. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range for the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> FifthGearTimeout - Timeout waiting for fifth gear. </li>
     *      <li> StatusByteUpdateFailure - Could not record the test result in the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetGear5String() </li>
     *      <li> GetGear6String() </li>
     *      <li> Gear5Observed() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetFifthGearUpdateResult() </li>
     *      <li> SetFifthGearUpdateResult() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @return Result of checking the fifth gear results.
     */
    virtual string CheckFifthGearResult(void);

    /**
     * Determine if sixth gear was observed during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if sixth gear was observed during Vehicle testing.  If the gear was not observed, the operator will be 
     * prompted to achieve the required gear.  If required, the result will be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> SixthGearSpeedRange - Vehicle speed range for sixth gear. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> OverdriveGear - Overdrive gear for the current transmission. </li>
     *      <li> SixthGearStatusRegister - Status register to use for recording the sixth gear test result. </li>
     *      <li> SixthGearBit - Bit to use for recording the sixth gear result. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range for the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> SixthGearTimeout - Timeout waiting for sixth gear. </li>
     *      <li> StatusByteUpdateFailure - Could not record the test result in the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetGear6String() </li>
     *      <li> Gear6Observed() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetSixthGearUpdateResult() </li>
     *      <li> SetSixthGearUpdateResult() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @return Result of checking the sixth gear results.
     */
    virtual string CheckSixthGearResult(void);

    /**
     * Determine if the torque converter clutch was observed in the No-Lock state.  
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The TCC no-lock state results will be reviewed to determine if the no-lock state has been observed.  If the 
     * torque converter clutch was not observed to be in the No-Lock state, the operator will be prompted to achieve 
     * the required conditions to place the TCC in the no-lock state.  If required, the result will be recorded in the
     * module.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> UpdateTCCStatesInModule - Flag indicating if the test result should be recorded in the module. </li>
     *      <li> TCCNoLockStatusRegister - Status register to use for rexording the result of the TCC No-lock state. </li>
     *      <li> TCCNoLockTestBit - Bit to use for recording the TCC No-lock result. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> BrakeToStop - Brake to zero speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> TCClutchNoLockTimeout - Timeout waiting for TCC No-lock state to be observed. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> TCCNoLockupObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      </ul>
     * 
     * @return Result of checking the TCC No-lock state.
     */
    virtual string CheckTCCNoLockState(void);

    /**
     * Determine if the torque converter clutch was observed in the Partial-Lock state.  
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The TCC partial-lock state results will be reviewed to determine if the partial-lock state has been observed.  If the 
     * torque converter clutch was not observed to be in the Partial-Lock state, the operator will be prompted to achieve 
     * the required conditions to place the TCC in the partial-lock state.  If required, the result will be recorded in the
     * module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> SpeedRange - Speed range to achieve TCC partial-lock state. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> UpdateTCCStatesInModule - Flag indicating if the test result should be recorded in the module. </li>
     *      <li> TCCPartialLockStatusRegister - Status register to use for rexording the result of the TCC Partial-lock state. </li>
     *      <li> TCCPartialLockTestBit - Bit to use for recording the TCC Partial-lock result. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range to the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> TCClutchPartialLockTimeout - Timeout waiting for TCC Partial-lock state to be observed. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> TCCPartialLockupObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @return Result of checking the TCC Partial-lock state.
     */
    virtual string CheckTCCPartialLockState(void);

    /**
     * Determine if the torque converter clutch was observed in the Full-Lock state.  
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The TCC full-lock state results will be reviewed to determine if the full-lock state has been observed.  If the 
     * torque converter clutch was not observed to be in the Full-Lock state, the operator will be prompted to achieve 
     * the required conditions to place the TCC in the full-lock state.  If required, the result will be recorded in the
     * module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> SpeedRange - Speed range to achieve TCC full-lock state. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> UpdateTCCStatesInModule - Flag indicating if the test result should be recorded in the module. </li>
     *      <li> TCCFullLockStatusRegister - Status register to use for rexording the result of the TCC Full-lock state. </li>
     *      <li> TCCFullLockTestBit - Bit to use for recording the TCC Full-lock result. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range to the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> TCClutchFullLockTimeout - Timeout waiting for TCC Full-lock state to be observed. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> TCCFullLockupObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @return Result of checking the TCC Full-lock state.
     */
    virtual string CheckTCCFullLockState(void);

    /**
     * Wait for overdrive conditions to be satisfied.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to accelerate to the required speed range.  once the required speed range is 
     * achieved, the operator will be instructed to maintain current speed.  The transmission gear will be monitored
     * to detect when the overdrive gear is achieved.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> OverdriveLockoutMinimumSpeed - Minimum speed required to achieve overdrive gear. </li>
     *      <li> OverdriveLockoutMaximumSpeed - Maximum speed allowed to achieve overdrive gear. </li>
     *      <li> OverdriveGear - Overdrive gear for the current transmission. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadCurrentGear - Message tag to use to read the current transmission gear. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> MaintainSpeed - Maintain the current speed. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items: ReadCurrentGear </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> WaitAndPromptForSpeed() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameter() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetDataTag() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetRollSpeed() </li>
     *      </ul>
     *
     * @return Result of waiting for overdrive conditions to be satisfied.
     */
    virtual string WaitForOverdriveConditions(void);

    /**
     * Wait for the operator to press the OD cancel button.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be instructed to cancel overdrive.  The switch status from the module will be monitored to
     * ensure the operator has pressed the Overdrive cancel button.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadODCancelButton - Message tag for reading the overdrive cancel button. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> CancelOverdrive - Cancel overdrive. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items: ReadODCancelButton </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetDataTag() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     *
     * @return Result of checking the OD button press.
     */
    virtual string WaitForOverdriveCancelButton(void);

    /**
     * Look for the gear to kickdown when the overdrive is canceled.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The current transmission gear will be monitored until either the current gear is the same as the
     * overdrive cancel gear or time expires.
     * <p>
     * <b>Test Component Parmaeters:</b>
     *      <ul>
     *      <li> OverdriveCancelGear - Gear the transmission will shift into when overdrive is canceled. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadCurrentGear - Message tag to use for reading the current gear from the transmission. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items: ReadCurrentGear </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> GetDataTag() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     *
     * @return Result of the gear kickdown.
     */
    virtual string WaitForOverdriveGearKickdown(void);

    /**
     * Start the quicklearn procedure.  
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be instructed to bring the Vehicle to zero speed if the Vehicle is not currently at zero speed.
     * The operator will then be instructed to shift the transmission into Neutral.  The shift lever position will be
     * monitored to ensure the operator places the shift lever into Neutral.  once the shift lever is detected in the
     * Neutral position, the module will be commanded to begin the quicklearn procedure. 
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between shift lever position checks. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadShiftLeverPosition - Message tag to use for reading the current shift lever position. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToNeutral - Shift the transmission to Neutral. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> StartQuickLearnFail - Module did not begin the quicklearn procedure. </li>
     *      <li> QuickLearnNeutralTimeout - Timeout waiting for shift lever in Neutral. </li>
     *      <li> CommunicationFailure - Could not read shift lever position from the module. </li>
     *      <li> QuickLearnZeroSpeedTimeout - Timeout wiaiting for zero speed. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items: ReadShiftLeverPosition </li>
     *      <li> GetShiftLeverNeutralString() </li>
     *      <li> StartQuickLearn() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of starting the quicklearn procedure.
     */
    virtual string StartQuickLearnProcedure(void);
    /**
     * Stop the quicklearn procedure.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Quicklearn will only be commanded to stop, if the procedure was successfully running.  If the Vehicle is not 
     * already at zero speed, the operator will be prompted to bring the Vehicle to zero speed.  Once the Vehicle is
     * determined to be at zero speed, the operator will be instructed to shift the transmission to Park.  The shift
     * lever will then be monitored to ensure the operator shifts to Park.  Once the Park position has been achieved,
     * the module will be commanded to stop the quicklearn procedure.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between shift lever checks. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadShiftLeverPosition - Message tag to use for reading the current shift lever position. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToPark - Shift the transmission to Park. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> StopQuickLearnFail - Could not command the module to stop the quicklearn procedure. </li>
     *      <li> QuickLearnParkTimeout - Timeout waiting for the operator to shift to Park. </li>
     *      <li> CommunicationFailure - Could not read shift lever position from the module. </li>
     *      <li> QuickLearnZeroSpeedTimeout - Timeout waiting for zero speed. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items: ReadShiftLeverPosition </li>
     *      <li> GetShiftLeverParkString() </li>
     *      <li> StopQuickLearn() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> IsQuickLearnStarted() </li>
     *      <li> SetStartTime() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of stopping the quicklearn procedure.
     */
    virtual string StopQuickLearnProcedure(void);
    /**
     * Monitor the quicklearn procedure.  
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Monitor the quicklearn procedure status.  The quicklearn status will be monitored until the procedure completes.
     * The procedure exit status will be reported.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between quicklearn procedure status checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> QuickLearmRpmTimeOut - Amount of time to wait for the engine RPM to rise to the correct level. </li>
     *      <li> QuicklearnRetries - Number of times to restart the quicklearn procedure. </li>
     *      <li> CheckQuickLearnRPM - Flag indicating if the engine RPM should be checked. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadQuickLearnStatus - Message tag to use to read the quick learn status. </li>
     *      <li> QuickLearnOilTooCold - Transmission oil temperature too cold to perform quicklearn. </li>
     *      <li> QuickLearnOilTooHot - Transmission oil temperature too hot to perform quicklearn. </li>
     *      <li> QuickLearnFaultsPresent - Quicklearn aborted due to module faults. </li>
     *      <li> QuickLearnRPMTooLow - Engine RPM too low to perform quicklearn. </li>
     *      <li> QuickLearnBrakesNotApplied - Quicklearn procedure aborted because brake pedal is not applied. </li>
     *      <li> QuickLearnThrottleTooHigh - Quicklearn procedure aborted because throttle is not closed. </li>
     *      <li> QuickLearnNotInNeutral - Quicklearn procedure aborted because transmission is not in neutral. </li>
     *      <li> QuickLearnNotInOverdrive - Quicklearn procedure aborted because transmission is not in drive. </li>
     *      <li> QuickLearnShiftToDrive - Shift the transmission to drive. </li>
     *      <li> QuickLearnRunning - Quicklearn procedure is in progress. </li>
     *      <li> QuickLearnComplete - Quicklearn procedure has completed. </li>
     *      </ul>
     *      
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> WaitForRpmRise - Wait for engine RPM to rise. </li>
     *      <li> ShiftToDrive - Shift to Drive. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> QuickLearnWaitForParkTimeout - Timeout waiting for the operator to shift to Park. </li>
     *      <li> QuickLearnRestartFail - Failed to restart the quicklearn procedure. </li>
     *      <li> QuickLearConditions - Vehicle conditions are not correct for starting the quicklearn procedure. </li>
     *      <li> QuickLearnUnknownStatus - Unknown quicklearn status received from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items: ReadQuickLearnStatus </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> SetStartTime() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> QuickLearnWaitForPark() </li>
     *      <li> StartQuickLearnProcedure() </li>
     *      <li> SetSecondaryStartTime() </li>
     *      <li> IsQuickLearnRpmInRange() </li>
     *      <li> SecondaryTimeRemaining() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     * 
     * @return Result of monitoring the quicklearn procedure status.
     */
    virtual string MonitorQuickLearnProcedure(void);

    /**
     * Finish the quicklearn procedure.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be instructed to shift the transmission to Park to compelte the quicklearn procedure.  The 
     * shift lever position can optionally be monitored in the module to ensure the shift lever is placed in the Park
     * position; otherwise, the operator will only be prompted and no verification will be performed.  Once the shift
     * lever is placed into the Park position, the clutch volumes will be read from the module and reported as part
     * of the test results.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> QuickLearnReadParkFromModule - Flag indicating if the shift lever position should be monitored for Park. </li>
     *      <li> ModuleBusyMaximumTime - Amount of time(ms) to wait for the operator to shift to Park. </li>
     *      <li> ReadIndividualVolumes - Flag indicating if the clutch volumes should be read separately. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToPark - Shift the transmission to Park. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - COuld not read the clutch volumes from the module. </li>
     *      <li> QuickLearnWaitForParkTimeout - Timeout waiting for the operator to shift to Park. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> QuickLearnWaitForPark() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> QuickLearnReadClutchVolumes() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     *
     * @return Result of finishing up the quicklearn procedure.
     */
    virtual string FinishQuickLearnProcedure(void);

    /**
     * Wait for the operator to shift the transmission to the Park position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be instructed to shift the transmission to Park.  The shift lever position will then be
     * monitored to ensure the shift lever is placed in the Park position.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ModuleBusyMaximumTime - Amount of time(ms) the module will return a busy signal when queried for 
     *                                   shift lever position. </li>
     *      <li> QuickLearnScanDelay - Amount of time(ms) to wait between shift lever position reads. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadShiftLeverPosition - Message tag to use for reading the shift lever position. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToPark - Shift to Park. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read the shift lever position from the module. </li>
     *      <li> QuickLearnWaitForParkTimeout - Timeout waiting for the operator to shift to Park. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items: ReadShiftLeverPosition </li>
     *      <li> GetShiftLeverParkString() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of waiting for the transmission to be placed in Park.
     */
    virtual string QuickLearnWaitForPark(void);

    /**
     * Determine if the engine RPM is in range.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The engine RPM will be checked to determine if it is in the proper range to perform the quicklearn procedure.
     * <p>
     * <b>Test Component Parmaeters:</b>
     *      <ul>
     *      <li> MinimumQuickLearnRPM - Minimum engine RPM required to perform the quicklearn procedure. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> QuickLearnRpmMessage - Message tag to read the engine RPM. </li>
     *      </ul>
     * 
     * <b>Module Interface Funcitons:</b>
     *      <ul>
     *      <li> ReadModuleData() - Data item: QuickLearnRpmMessage </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> GetParameterInt() </li>
     *      </ul>
     *
     * @return Flag indicating if the engine RPM is in the proper range to perform the quicklearn procedure.
     */
    virtual const bool IsQuickLearnRpmInRange(void);

    /**
     * Read the clutch volumes from the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The transmission clutch volumes will be read from the module and returned to the calling function in 
     * <i>clutchVolumes</i>.  The flag <i>readIndividualVolumes</i> is used to determine how clutch volumes are
     * returned from the module.  Clutch volumes can either be returned individually or in a group.
     * <p>
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ReadClutchVolumeFail - Could not read the clutch volumes from the transmission. </li>
     *      </ul>
     * 
     * <b>Funcitons Called:</b>
     *      <ul>
     *      <li> ReadIndividualClutchVolumes() </li>
     *      <li> ReadClutchVolumeGroup() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @param readIndividualVolumes  Flag indicating if the clutch volumes should be read individually.
     *      <ul>
     *      <li> True - individual volumes can be read. </li>
     *      <li> False - volumes can be read in groups. </li>
     *      </ul>
     * @param clutchVolumes
     *               Clutch volumes read from the module.
     * 
     * @return Result of reading the clutch volumes.
     */
    virtual string QuickLearnReadClutchVolumes(const bool &readIndividualVolumes, vector<UINT8> &clutchVolumes);

    /**
     * Read the inidividual clutch volumes from the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Each clutch volume listed in clutch volume list from the config data will be read from the module and stored
     * in <i>clutchVolumes</i> for return to the calling function.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> NumberOfClutchVolumes - Number of clutch volumes to read from the module. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read clutch volumes from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @param clutchVolumes
     *               Clutch volumes read from the module.
     * @return Result of reading each clutch volume from the module.
     */
    virtual string ReadIndividualClutchVolumes(vector<UINT8> &clutchVolumes);

    /**
     * Read clutch volumes by group.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The clutch volumes will be read from the module in groups.  Two groups of clutch volumes will be read from the 
     * module.  The second group will only be read if the first group read was successful.  The clutch volumes will
     * then be stored in <i>clutchVolumes</i> for return to the calling function.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadClutchVolumes1 - Message tag to read the first group of clutcj volumes from the module. </li>
     *      <li> ReadClutchVolumes2 - Message tag to read the second group of clutcj volumes from the module. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read the clutch volumes from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items: ReadClutchVolumes1, ReadClutchVolumes2 </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     *
     * @param clutchVolumes
     *               Clutch volumes read from the module.
     * @return Result of reading the clutch volumes from the module.
     */
    virtual string ReadClutchVolumeGroup(vector<UINT8> &clutchVolumes);

    /**
     * Start the background shift lever position monitor.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The shift lever monitor will be started in order to ensure all required transmission shift lever positions are
     * observed during the test sequence.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommuncationFailure - Could not start the shift lever position monitor. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to start the shift lever monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StartShiftLeverMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of starting the shift lever monitor.
     */
    virtual string StartShiftLeverMonitor(void);
    /**
     * Stop the background shift lever position monitor.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The data gathered by the monitor will be checked to ensure all required shift lever positions have been observed.
     * The shift lever monitor will then be commanded to stop and the shift lever data will be reported.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> AllShiftLeverPositionsNotSeen - Did not observe all required shift lever positions. </li>
     *      <li> ParkShiftLeverPositionNotSeen - Did not detect shift lever in Park. </li>
     *      <li> ReverseShiftLeverPositionNotSeen - Did not detect shift lever in Reverse. </li>
     *      <li> NeutralShiftLeverPositionNotSeen - Did not detect shift lever in Neutral. </li>
     *      <li> LowShiftLeverPositionNotSeen - Did not detect shift lever in Low. </li>
     *      <li> Low3ShiftLeverPositionNotSeen - Did not detect shift lever in Low-3. </li>
     *      <li> DriveShiftLeverPositionNotSeen - Did not detect shift lever in Drive. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while stopping the shift lever monitor. </li>
     *      </ul>
     * 
     * <b>Module Interace Functions:</b>
     *      <ul>
     *      <li> StopShiftLeverMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> CheckParkShiftLeverResult() </li>
     *      <li> CheckReverseShiftLeverResult() </li>
     *      <li> CheckNeutralShiftLeverResult() </li>
     *      <li> CheckDriveShiftLeverResult() </li>
     *      <li> CheckLow3ShiftLeverResult() </li>
     *      <li> CheckLowShiftLeverResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of stopping the shift lever monitor.
     */
    virtual string StopShiftLeverMonitor(void);
    /**
     * Determine if the shift lever was observed in Park during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The Park shift lever position results will be checked.  If the Park shift lever position was not observed during
     * the test sequence, the operator will be prompted to shift to Park.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between shift lever position checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToPark - Place the shift lever in the Park position. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ParkShiftLeverPositionTimeout - Timeout waiting for shift lever in Park. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ShiftLeverParkObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      </ul>
     * 
     * @return Result of checking the Park shift lever result.
     */
    virtual string CheckParkShiftLeverResult(void);
    /**
     * Determine if the shift lever was observed in Reverse during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The Reverse shift lever position results will be checked.  If the Reverse shift lever position was not observed during
     * the test sequence, the operator will be prompted to shift to Reverse.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between shift lever position checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToReverse - Place the shift lever in the Reverse position. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ReverseShiftLeverPositionTimeout - Timeout waiting for shift lever in Reverse. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ShiftLeverReverseObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      </ul>
     * 
     * @return Result of checking the Reverse shift lever result.
     */
    virtual string CheckReverseShiftLeverResult(void);
    /**
     * Determine if the shift lever was observed in Neutral during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The Neutral shift lever position results will be checked.  If the Neutral shift lever position was not observed during
     * the test sequence, the operator will be prompted to shift to Neutral.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between shift lever position checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToNeutral - Place the shift lever in the Neutral position. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> NeutralShiftLeverPositionTimeout - Timeout waiting for shift lever in Neutral. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ShiftLeverNeutralObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      </ul>
     * 
     * @return Result of checking the Neutral shift lever result.
     */
    virtual string CheckNeutralShiftLeverResult(void);
    /**
     * Determine if the shift lever was observed in Drive during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The Drive shift lever position results will be checked.  If the Drive shift lever position was not observed during
     * the test sequence, the operator will be prompted to shift to Drive.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between shift lever position checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Place the shift lever in the Drive position. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> DriveShiftLeverPositionTimeout - Timeout waiting for shift lever in Drive. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ShiftLeverDriveObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      </ul>
     * 
     * @return Result of checking the Drive shift lever result.
     */
    virtual string CheckDriveShiftLeverResult(void);
    /**
     * Determine if the shift lever was observed in Low during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The Low shift lever position results will be checked if the transmission is equipped with a Low position.  
     * If the Low shift lever position was not observed during the test sequence, the operator will be prompted 
     * to shift to Low.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between shift lever position checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToLow - Place the shift lever in the Low position. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> LowShiftLeverPositionTimeout - Timeout waiting for shift lever in Low. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ShiftLeverLowObserved() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> IsLowShiftLeverEquipped() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      </ul>
     * 
     * @return Result of checking the Low shift lever result.
     */
    virtual string CheckLowShiftLeverResult(void);
    /**
     * Determine if the shift lever was observed in Low3 during Vehicle testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The Low3 shift lever position results will be checked if the transmission is equipped with a Low3 position.  
     * If the Low3 shift lever position was not observed during the test sequence, the operator will be prompted 
     * to shift to Low3.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between shift lever position checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToLow3 - Place the shift lever in the Low3 position. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> Low3ShiftLeverPositionTimeout - Timeout waiting for shift lever in Low3. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ShiftLeverLow3Observed() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> IsLow3ShiftLeverEquipped() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      </ul>
     * 
     * @return Result of checking the Low3 shift lever result.
     */
    virtual string CheckLow3ShiftLeverResult(void);
    /**
     * Read the EOL status bytes from the module.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The EOL status bytes will be read from the module and stored in a member variable.  The status bytes will also
     * be logged to the log file for debug purposes.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read the EOL status bytes from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to read the EOL status bytes from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items: ReadEOLStatusBytes </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of reading the EOL status bytes from the module.
     */
    virtual string ReadEOLStatusBytes(void);

    /**
     * Update the EOL Status register with the test result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Many different modules handle updating EOL status bytes differently.  There is no generic implementation.  This
     * function will return "Software Fail" if called.
     * <p>
     *
     * @param statusRegister
     *                   EOL Status Register to update.
     * @param testBit    Which bit to update.
     * @param testResult Test result to write to the module.
     * @return Software Fail
     */
    virtual string UpdateEOLStatusByte(const UINT16 &statusRegister, const UINT8 &testBit, const string &testResult);

    /**
     * Static Shift Lever Test
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The operator will be prompted to shift to all required shift lever positions.  The software will then verify that
     * all required shift lever positions have been observed.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read shift lever position from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetShiftLeverParkString() </li>
     *      <li> GetShiftLeverReverseString() </li>
     *      <li> GetShiftLeverNeutralString() </li>
     *      <li> GetShiftLeverDriveString() </li>
     *      <li> GetShiftLeverAutostickString() </li>
     *      <li> GetShiftLeverOpenString() </li>
     *      <li> GetGear1String() </li>
     *      <li> GetShiftLeverDownString() </li>
     *      <li> GetShiftLeverUpString() </li>
     *      <li> GetGear2String() </li>
     *      <li> GetGear3String() </li>
     *      <li> GetShiftLeverLowString() </li>
     *      <li> GetShiftLeverThirdString() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> RequestLeverShift() </li>
     *      <li> RequestGearShift() </li>
     *      <li> CheckAutostickBits() </li>
     *      <li> IsAutostick() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of the shift lever check.
     */
    virtual string CheckShiftLever(void);

    /**
     * Wait for engine and ignition to be off.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be instructed to turn the ignition to the Off position after shifting the transmission to the
     * Park position.  The ignition status will then be monitored to determine when the ignition is in the Off position.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to ait between transmission data checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> EngineOffIgnitionOffWaitTime - Amount of time(ms) to wait after detecting the ignition in the Off
     *                                          position. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToPark - Shift the transmission into Park. </li>
     *      <li> EngineOffIgnitionOff - Switch ignition to the Off position. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetShiftLeverParkString() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> RequestLeverShift() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GenericTCTemplate::IsIgnitionOn() </li>
     *      <li> GetParameterInt() </li>
     *      </ul>
     *
     * @return Result of waiting for ignition to be in the Off position.
     */
    virtual const string WaitForEngineOffIgnitionOff(void);
    /**
     * Prompts driver for a requested state.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to place the transmission into the requested state.  The transmission state will
     * then be monitored to ensure the operator has placed the transmission in the requested state.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      </ul>
     * 
     * @param requestedState
     *               State in which the transmission is to be placed.
     * @param promptForState
     *               Prompt box in which to display the operator instructions.
     * @param currentStateTag
     *               The message tag to read the current transmission state from the module.
     * @param minimumSuccessiveReads
     *               The amount of successive reads where the current state matches the requested state.
     * @param successiveReadDelay
     *               The delay in milliseconds between successive reads.
     * 
     * @return Flag indicating if the driver was able to place the transmission in the requested state.
     */
    bool RequestStateByPrompt(string requestedState, 
                              string promptForState,
                              string currentStateTag, 
                              INT32 minimumSuccessiveReads, 
                              INT32 successiveReadDelay);

    /**
     * Prompt the operator to shift to the requested gear.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be instructed to shift the transmission to the required gear.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SuccessiveGearReadMinimum - Number of consecutive reads where the current gear must match the 
     *                                       requested gear. </li>
     *      <li> SuccessiveGearReadDelay - Amount of time(ms) to wait between gear reads. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> RequestStateByPrompt() </li>
     *      <li> GetParameterInt() </li>
     *      </ul>
     * 
     * @param requestedGear
     *               Gear to shift the transmission into.
     * @param promptForGear
     *               Prompt tag to use instructing the operator to shift to the requested gear.
     * 
     * @return Flag indicating if the driver was able to shift to the requested gear.
     */
    bool RequestGearShift(string requestedGear, string promptForGear);
    /**
     * Prompt the operator to shift the transmission.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to place the shift lever into the requested position.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SuccessiveLeverReadMinimum - Number of consecutive reads where the current shift lever position must
     *                                        match the requested shift lever position. </li>
     *      <li> SuccessiveLeverReadDelay - Amount of time(ms) to wait between shift lever reads. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> RequestStateByPrompt() </li>
     *      <li> GetParameterInt() </li>
     *      </ul>
     * 
     * @param requestedLever
     *               Requested shift lever position.
     * @param promptForLever
     *               Prompt to use for instructing the operator to place the shift lever into the requested position.
     * 
     * @return Flag indicating if the operator placed the shift lever into the desired position.
     */
    bool RequestLeverShift(string requestedLever, string promptForLever);
    /**
     * Prompt the operator to shift the transmission.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to place the shift lever into the requested position.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SuccessiveLeverReadMinimum - Number of consecutive reads where the current shift lever position must
     *                                        match the requested shift lever position. </li>
     *      <li> SuccessiveLeverReadDelay - Amount of time(ms) to wait between shift lever reads. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> RequestStateByPrompt() </li>
     *      <li> GetParameterInt() </li>
     *      </ul>
     * 
     * @param requestedLever
     *               Requested shift lever position.
     * @param promptForLever
     *               Prompt to use for instructing the operator to place the shift lever into the requested position.
     * 
     * @return Flag indicating if the operator placed the shift lever into the desired position.
	 */
	 bool RequestRawLeverShift(string requestedLever, string promptForLever);
    /**
     * Prompt the operator to shift the transmission to the requested auto-stick position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to place the shift lever into the requested auto-stick position.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SuccessiveAutostickReadMinimum - Number of consecutive reads where the current auto-stick shift lever 
     *                                            position must match the requested shift lever position. </li>
     *      <li> SuccessiveAutostickReadDelay - Amount of time(ms) to wait between shift lever reads. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> RequestStateByPrompt() </li>
     *      <li> GetParameterInt() </li>
     *      </ul>
     * 
     * @param requestedLever
     *               Requested shift lever position.
     * @param promptForLever
     *               Prompt to use for instructing the operator to place the shift lever into the requested position.
     * 
     * @return Flag indicating if the operator placed the shift lever into the desired auto-stick position.
     */
    bool RequestAutostickShift(string requestedState, string promptForLever);

    /**
     * Get the result of updating the First gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the First gear observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the First gear observed result in the module.
     */
    const string& GetFirstGearUpdateResult(void);

    /**
     * Get the result of updating the Second gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Second gear observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Second gear observed result in the module.
     */
    const string& GetSecondGearUpdateResult(void);

    /**
     * Get the result of updating the Third gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Third gear observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Third gear observed result in the module.
     */
    const string& GetThirdGearUpdateResult(void);

    /**
     * Get the result of updating the Fourth gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Fourth gear observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Fourth gear observed result in the module.
     */
    const string& GetFourthGearUpdateResult(void);

    /**
     * Get the result of updating the Fifth gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Fifth gear observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Fifth gear observed result in the module.
     */
    const string& GetFifthGearUpdateResult(void);

    /**
     * Get the result of updating the Sixth gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Sixth gear observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Sixth gear observed result in the module.
     */
    const string& GetSixthGearUpdateResult(void);

    /**
     * Get the result of updating the Park gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Park gear observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Park gear observed result in the module.
     */
    const string& GetParkGearUpdateResult(void);

    /**
     * Get the result of updating the Reverse gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Reverse gear observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Reverse gear observed result in the module.
     */
    const string& GetReverseGearUpdateResult(void);

    /**
     * Get the result of updating the Neutral gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Neutral gear observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Neutral gear observed result in the module.
     */
    const string& GetNeutralGearUpdateResult(void);

    /**
     * Get the result of updating the Tcc no-lock result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Tcc no-lock observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Tcc no-lock observed result in the module.
     */
    const string& GetUnlockStateUpdateResult(void);

    /**
     * Get the result of updating the Tcc partial-lock result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Tcc partial-lock observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Tcc partial-lock observed result in the module.
     */
    const string& GetPartialLockStateUpdateResult(void);

    /**
     * Get the result of updating the Tcc full-lock result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of recording the Tcc full-lock observed result in the EOL status bytes in the module.
     * <p>
     * @return  Result of recording the Tcc full-lock observed result in the module.
     */
    const string& GetFullLockStateUpdateResult(void);

    /**
     * Store the result of updating the first gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the first gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the first gear observed result in the module.
     */
    void SetFirstGearUpdateResult(const string &result);

    /**
     * Store the result of updating the second gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the second gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the second gear observed result in the module.
     */
    void SetSecondGearUpdateResult(const string &result);

    /**
     * Store the result of updating the third gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the third gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the third gear observed result in the module.
     */
    void SetThirdGearUpdateResult(const string &result);

    /**
     * Store the result of updating the fourth gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the fourth gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the fourth gear observed result in the module.
     */
    void SetFourthGearUpdateResult(const string &result);

    /**
     * Store the result of updating the fifth gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the fifth gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the fifth gear observed result in the module.
     */
    void SetFifthGearUpdateResult(const string &result);

    /**
     * Store the result of updating the sixth gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the sixth gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the sixth gear observed result in the module.
     */
    void SetSixthGearUpdateResult(const string &result);

    /**
     * Store the result of updating the park gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the park gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the park gear observed result in the module.
     */
    void SetParkGearUpdateResult(const string &result);

    /**
     * Store the result of updating the reverse gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the reverse gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the reverse gear observed result in the module.
     */
    void SetReverseGearUpdateResult(const string &result);

    /**
     * Store the result of updating the neutral gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the neutral gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the neutral gear observed result in the module.
     */
    void SetNeutralGearUpdateResult(const string &result);

    /**
     * Store the result of updating the TCC No-Lock gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the TCC No-Lock gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the TCC No-Lock gear observed result in the module.
     */
    void SetUnlockStateUpdateResult(const string &result);

    /**
     * Store the result of updating the TCC Partial-Lock gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the TCC Partial-Lock gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the TCC Partial-Lock gear observed result in the module.
     */
    void SetPartialLockStateUpdateResult(const string &result);

    /**
     * Store the result of updating the TCC Full-Lock gear result in the module.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of recording the TCC Full-Lock gear observed test result in the module will be stored in a member variable.
     * <p>
     * @param   result  Result of recording the TCC Full-Lock gear observed result in the module.
     */
    void SetFullLockStateUpdateResult(const string &result);

    /**
     * Check the flag indicating if the transmission shift lever is equipped with a Low position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * There is no generic method for determining if a transmission shift lever has a Low position.
     * This function always returns false.
     * <p>
     * @return False
     */
    virtual bool IsLowShiftLeverEquipped(void);
    /**
     * Check the flag indicating if the transmission shift lever is equipped with a Low-3 position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * There is no generic method for determining if a transmission shift lever has a Low-3 position.
     * This function always returns false.
     * <p>
     * @return False
     */
    virtual bool IsLow3ShiftLeverEquipped(void);

    /**
     * Store the flag indicating if quick learn is running.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The flag indicating if the quicklearn procedure is running will be stored in a member variable.
     * <p>
     * @param   quickLearnRunning The quick learn running flag.
     */
    void SetQuickLearnStarted(bool &quickLearnRunning);
    /**
     * Check the flag indicating if quick learn is running.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The flag indicating if the quicklearn procedure is running will be returned to the calling function.
     * <p>
     * @return   The quick learn running flag.
     */
    bool IsQuickLearnStarted(void);
    /**
     * Check reverse speed in the transmission.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * If the Vehicle is not at zero speed, the operator will be instructed to bring the Vehicle to zero speed.  The
     * operator will then be instructed to shift the transmission to Reverse and accelerate to the target speed.
     * Once reverse gear is detected, the Vehicle speed wil be monitored to ensure it reaches the desired target
     * speed.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AbsoluteValueSpeed - Flag to indicate if the absolute speed value should be used.  Some older machines
     *                                do not have quadrature encoders, so there is no reverse speed, only positive speed. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag for displying the target speed for the operator. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToReverse - Shift the transmission to Reverse. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> StatusCheckFailure - System status precludes further testing. </li>
     *      <li> CommunicationFailure - Could not read reverse gear from the transmission. </li>
     *      <li> AccelerateInReverseTimeout - Timeout waiting for the reverse target speed to be reached. </li>
     *      <li> UnknownFailure - An unexpected exit condition occurred. </li>
     *      <li> AccelerateInReverseZeroSpeedTimeout - Timeout waiting for the operator to brake to stop. </li>
     *      <li> SoftwareFailure - A BepException occurred while monitoring reverse speed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetStartTime() </li>
     *      <li> ReadReverseGear() </li>
     *      <li> GetParameterInt() </li>
     *      <li> WaitForTargetSpeed() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param speedMinimum
     *                  Minimum reverse speed required.
     * @param speedMaximum
     *                  The maximum reverse speed allowed.
     * @param scanDelay Amount of time(ms) to wait between Vehicle speed checks.
     * 
     * @return Result of verifying the reverse gear speed.
     */
    virtual string AccelerateInReverse(const float &speedMinimum, const float &speedMaximum, INT32 scanDelay = 100);
    /**
     * Data analysis object to assist in analyzing module data.
     */
    DataAnalysis m_analyze;
    /**
     * Bytes used to write EOL status Registers.
     */
    XmlNodeMap m_writeEOLStatusBytes;
    /**
     * EOL Status bytes to reset.
     */
    XmlNodeMap m_resetEOLStatusBytes;
    /**
     * Upline register bits to check.
     */
    XmlNodeMap m_uplineTestsToCheck;
    /**
     * List of the clutch volume tags.
     */
    XmlNodeMap m_clutchVolumeIndex;
    /**
     * EOL status bytes read from the module.
     */
    vector<UINT8> m_eolStatusBytes;
    /**
     * Result of updating the first gear observed result in the module.
     */
    string m_firstGearUpdated;
    /**
     * Result of updating the second gear observed result in the module.
     */
    string m_secondGearUpdated;
    /**
     * Result of updating the third gear observed result in the module.
     */
    string m_thirdGearUpdated;
    /**
     * Result of updating the fourth gear observed result in the module.
     */
    string m_fourthGearUpdated;
    /**
     * Result of updating the fifth gear observed result in the module.
     */
    string m_fifthGearUpdated;
    /**
     * Result of updating the sixth gear observed result in the module.
     */
    string m_sixthGearUpdated;
    /**
     * Result of updating the park gear observed result in the module.
     */
    string m_parkGearUpdated;
    /**
     * Result of updating the reverse gear observed result in the module.
     */
    string m_reverseGearUpdated;
    /**
     * Result of updating the neutral gear observed result in the module.
     */
    string m_neutralGearUpdated;
    /**
     * Result of updating the Unlocked state of the torque converter clutch in the module.
     */
    string m_noLockStateUpdated;
    /**
     * Result of updating the Partial-Lock state of the torque converter clutch in the module.
     */
    string m_partialLockStateUpdated;
    /**
     * Result of updating the Full-Lock state of the torque converter clutch in the module.
     */
    string m_fullLockStateUpdated;
    /**
     * Verify the auto-stick bits are set properly.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The auto-stick bits will be read from the module.  This data will then be compared to the transmission type from
     * the Vehicle build data to ensure the module was setup properly.  The result of the check will then be recorded
     * in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CheckAutostickBitsStatusRegister - Register to use for recording the auto-stick bit check. </li>
     *      <li> CheckAutostickTestBit - Bit to use for recording the result of the auto-stick bit check. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Transmission - System tag to get the transmission type from the Vehicle build data. </li>
     *      <li> CheckAutostickBits - Message tag to use for reading the auto-stick bits from the module. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read the auto-stick bits from the module. </i>
     *      <li> SoftwareFailure - A ModuleException occurred while reading the auto-stick bits from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items:CheckAutostickBits </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of verifying the auto-stick bits are set correctly.
     */
    virtual string CheckAutostickBits(void);
    /**
     * Check the flag indicating if the transmission is equipped with auto-stick.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The flag indicating if the transmission is equipped with auto-stick will be returned to the calling function.
     * <p>
     * 
     * @return Flag indicating if the transmission is equipped with auto-stick.
     */
    bool IsAutostick(void);
    /**
     * Flag indicating if the transmission is equipped with autostick.
     */
    bool m_autostickEquipped;
    /**
     * Flag indicating if quicklearn was started.
     */
    bool m_quickLearnRunning;
};

//*************************************************************************
#endif  //GenericTransmissionTCTemplate_h
