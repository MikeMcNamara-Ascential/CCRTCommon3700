//******************************************************************************
// Description:
//  Generic Emissions Test Component.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/TransferCase/Include/ElectronicTcaseTC.h $
// 
// 1     11/02/07 10:09a Derickso
// Added content to comments.
// 
// 8     10/30/07 3:21p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added Javadoc comments.
// 
// 7     1/17/07 3:45p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added WarmInitialize().
// 
// 6     9/01/04 9:24p Cward
// Changed includes to cpp files
//
// 5     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 4     5/25/04 4:09p Cward
// Updated Command Test Step to report abort if ignition is not on after a
// specific time. Added foot on brake prompt to Command Shift Tcase
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 1     10/28/03 7:55a Derickso
// Created.
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef ELECTRONICTCASETC_H
#define ELECTRONICTCASETC_H
//==============================================================================

#include "GenericTCTemplate.cpp"
#include "ElectronicTcaseTool.cpp"

/**
 * Electronic Tcase Transfer Case test component.
 *
 * @author Bill Barrett
 * @see GenericTcaseTC
 * @see GenericTestComponentTemplate
 * @see TcaseModuleInterface
 */
template <class TcaseModuleType>
class ElectronicTcaseTC : public GenericTCTemplate<TcaseModuleType>
{
public:
    /**
     * <b>Description</b>
     * Class constructor.
     */
    ElectronicTcaseTC();
    
    /**
     * <b>Description</b>
     * Class Destructor.
     */
    virtual ~ElectronicTcaseTC();
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
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
	 * <b>Test Step Functions Called:</b>
	 * 		<ul>
     *      <li> CheckIgnitionOn() </li>
     *      <li> ReturnSwitchControl() </li>
     *      <li> ElectronicTcaseTool::CheckTcaseShift() </li>
     *      <li> ElectronicTcaseTool::CheckAxleRatio() </li>
     *      <li> ElectronicTcaseTool::CheckAxleBalance() </li>
	 * 		</ul>
	 * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GenericTCTemplate::CommandTestStep() </li>
     *      </ul>
     * 
	 * @param value    The value that was commanded by the test sequencer.
	 * @return  The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

    /**
     * Return switch control to the module.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Switch control will be returned to the module so that the t-case returns to normal operation.
     * <p>
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> TcaseCommunicationError - Could not return switch control to the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to return switch control to the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ReturnSwitchInputControl() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     * 
     * @return Result of returning switch control to the module.
     */
    virtual string ReturnSwitchControl(void);

    /**
     * Check Tcase shifts to desired postion.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The transfer case will be checked to determine if the shift to the desired position has been achieved.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTool::CheckTcaseShift() </li>
     *      </ul>
     * 
     * @return Result of checking for the t-case in the desired state.
     */
    virtual string CheckTcaseShift ( void );

    /**
     * Prompt shift Transmission.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Prompt the operator to shift the transmission to the specified state so the t-case shift can be completed.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTool::ShiftTransmission() </li>
     *      </ul>
     * 
     * @param transGear  Shift lever position to shift the transmission to. </li>
     *
     * @return Result of shifting the transmission to the specified gear.
     */
    virtual string ShiftTransmission (const string transGear);

    /**
     * Prompt key On/Off.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Instruct the operator to place the ignition key in the specified position.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTool::TurnKeyOnOff() </li>
     *      </ul>
     *
     * @param keyPosition  Position to place the ignition key in.
     *
     * @return Result of placing the ignition key in the specified position.
     */
    virtual string TurnKeyOnOff (const string keyPosition);

    /**
     * Prompt Engine On/Off.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Prompt the operator to either start or stop the engine.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTool::EngineOnOff() </li>
     *      </ul>
     *
     * @param engineRun  State in which to place the engine - "On" or "Off"
     *
     * @return Result of placing the engine in the desired state. 
     */
    virtual string EngineOnOff (const string engineRun);

    /**
     * Check Throttle position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check the throttle position to ensure it is in the correct state to achieve the transfer case shift.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTool::CheckThrottlePosition() </li>
     *      </ul>
     *
     * @param throttlePosition Position the throttle should be in for the t-case shift - "Open"/"Closed"
     *
     * @return Result of checking the throttle position.
     */
    virtual string CheckThrottlePosition ( const string throttlePosition );

    /**
     * Shift the t-case to the desired position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The t-case will be shifted to the desired position.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTool::ShiftTcase() </li>
     *      </ul>
     * 
     * @return Result of shifting the t-case to the desired position.
     */
    virtual string ShiftTcase ( void );

    /**
     * Verify the t-case was shifted to the correct position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The t-case gear will be checked to ensure the shift to the desired position has been completed successfully.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTool::VerifyTcaseShift() </li>
     *      </ul>
     *
     * @param verifyMethod  Verification method to use to validate the current t-case gear.
     *
     * @return Result of verifying the t-case shift.
     */
    virtual string VerifyTcaseShift ( const string &verifyMethod );

    /**
     * Have the operator verify t-case shift.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to verify the t-case completed the shift to the desired position.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTool::TcaseCheckDriver() </li>
     *      </ul>
     * 
     * @return Result of operator verifying t-case shift.
     */
    virtual string TcaseCheckDriver ( void );

    /**
     * Prompt Driver to shift Tcase.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to shift the t-case into the desired position.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTool::TcasePromptDriver() </li>
     *      </ul>
     *
     * @return Result of prompting the operator to shift the t-case.
     */
    virtual string TcasePromptDriver ( void );

    /**
     * Check Front to Rear axle ratio.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The operator will be prompted to the desired speed range and t-case gear.  The front to rear axle speed ratio will
     * then be calculated to ensure it falls within an acceptable range.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTool::CheckAxleRatio() </li>
     *      </ul>
     *
     * @return Axle Ratio test result.
     */
    virtual string CheckAxleRatio ( void );

    /**
     * Command the t-case to shift gears.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * A command will be sent to the t-case module to perform a range shift to the desired gear. In many cases, the same 
     * command must be sent to the module multiple times in order for the t-case to shift gears.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> TcaseShiftTime - Time(ms) allowed for the t-case to shift gears. </li>
     *      <li> TcaseGear - T-case gear to command. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CommandShiftMessageGap - Time(ms) to wait between shift commands to the t-case. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> CommandShiftTcase - Command the t-case to shift to the desired gear. </li>
     *      <li> FootOnBrake - Apply brake pedal. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> TcaseCommandShiftTimeout - Timeout waiting for the t-case to shift to the commanded gear. </li>
     *      <li> SystemFail - System status is preventing further testing. </li>
     *      <li> TcaseCommandShiftFail - T-case failed to shift to desired gear. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while commanding the t-case to shift to the desired gear. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> CommandTransferCaseShift() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetTestStepName() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SetStartTime() </li>
     *      <li> GetParameterInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of commanding the t-case to shift to the desired gear.
     */
    virtual string CommandShiftTcase (void);

    /**
     * Read Tcase shift status.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The t-case gear will be read from the module and compared to the desired gear to ensure the range shift was successful.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> TcaseGear - Transfercase gear to verify. </li>
     *      <li> TcaseMessageDelay - Time(ms) to wait between t-case gear reads. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadTcaseStatus - Message tag to use for reading the current t-case gear from the module. </li>
     *      <li> SoftwareFailure - Fault tag to use to indicate a software failure occurred. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> TcaseCheckGear - Checking for t-case gear. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> TcaseCommunicationError - Could not read t-case gear from the module. </li>
     *      <li> SystemFail - System status is preventing further testing. </li>
     *      <li> TcaseCheckGearTimeout - Timeout waiting for the t-case to shift to the desired gear. </li>
     *      <li> TcaseCheckGearFail - T-case failed to shift to the desired gear. </li>
     *      <li> SoftwareFailure - A ModuleExcpetion occurred while reading current t-case gear from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message tag: ReadTcaseStatus </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of verifying the t-case range shift.
     */
    virtual string TcaseCheckGear ( void );

    /**
     * Read Tcase switch position.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The operator will be prompted to shift the t-case to the desired position.  The t-case will then be monitored
     * to ensure the shift is successful.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> TcaseGear - Gear to shift the t-case into. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadTransferCaseSwitch - Message tag to use for reading the t-case switch. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> <i>[Test Step Name]</i>ShiftTcase - Shift the t-case to the specified gear. </li>
     *      </ul>
     * 
     * <b>Fault tags:</b>
     *      <ul>
     *      <li> TcaseCommunicationError - Could not read t-case switch info from the module. </li>
     *      <li> TcaseCheckGearTimeout - Timeout waiting for t-case gear shift to complete. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadTransferCaseSwitch </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SetStartTime() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     * 
     * @return Result of validating the t-case switch position.
     */
    virtual string TcaseCheckSwitch ( void );

    /**
     * Read Tcase shift status using cluster module message.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Check the cluster to ensure the t-case shift has completed.
     * <br><b><i>Note:</i></b> This function is not currently implemented and will return SoftwareFail if called.
     * <p>
     * 
     * @return Result of validating the cluster indicates the t-case shift is complete.
     */
    virtual string TcaseCheckCluster ( void );

    /**
     * The Generic Tcase utility object that contains most of the funtionality
     * for Tcase testing
     */
    ElectronicTcaseTool<ElectronicTcaseTC> *m_tcaseTool;
};

#endif  //ElectronicTcaseTool_h

