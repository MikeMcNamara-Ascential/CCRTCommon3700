//******************************************************************************
// Description:
//  Electronic version of Tool Kit for Transfer Case Test Component.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/TransferCase/Include/ElectronicTcaseTool.h $
// 
// 1     11/02/07 10:11a Derickso
// Updated comment content.
// 
// 3     10/30/07 3:22p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added Javadoc comments.
// 
// 1     10/01/07 3:51p Derickso
// Updated header comments to doxygen format.
// 
// 2     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
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
#ifndef ElectronicTcaseTool_h
#define ElectronicTcaseTool_h
//==============================================================================

#include "TcaseTool.h"

/**
 * Electronic Transfer Case test component tool kit.
 * This test component will test DaimlerChrysler Transfer Case.
 * Specifically, this test is designed to work with manual and or
 * electronic Transfer cases.
 *
 * @author Greg Pattison
 * @see TcaseTool
 * @see GenericTC
 */
template <class TestComponent>
class ElectronicTcaseTool : public TcaseTool
{
public:
    /**
     * Class constructor.<br>
     * The pointer to the instantiating test component will be saved off to a member variable.
     * 
     * @param tc     Pointer to the instantiating test component so parameters and test step info can be retrieved.
     */
    ElectronicTcaseTool(TestComponent *tc);
	
    /**
     * Class Destructor.
     */
    virtual ~ElectronicTcaseTool();

    /**
     * Shift the transmission to the desire position.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Prompt the operator to place the transmission shift lever into the desired position.
     * In some cases, the t-case will only shift with the transmission in a specific gear.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTC::ShiftTransmission() </li>
     *      </ul>
     *
     * @param transGear Gear the transmission must be in to complete the t-case shift.
     *
     * @return Result of shifting the transmission to the desired gear.
     */
    virtual string ShiftTransmission ( const string transGear );

    /**
     * Place in the ignition in the On or Off position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to place the ignition key in the On or Off position based on the value of
     * <i>keyPosition</i>.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTC::TurnKeyOnOff() </li>
     *      </ul>
     * 
     * @param keyPosition
     *               Podition to place the ignition key in.
     * 
     * @return Result of placing the ignition key inthe desired position.
     */
    virtual string TurnKeyOnOff ( const string keyPosition );

    /**
     * Prompt the operator to place the engine in the desired run state.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to place the engine in the desired run state based on the value of <i>engineRun</i>
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTC::TurnKeyOnOff() </li>
     *      </ul>
     *
     * @param engineRun Place the engine in the desired run state.
     *
     * @return Result of placing the engine in the desired run state.
     */
    virtual string EngineOnOff ( const string engineRun );

    /**
     * Verify the throttle is in the correct position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to apply the correct amount of throttle for the t-case to perform the range shift.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ElectronicTcaseTC::CheckThrottlePosition() </li>
     *      </ul>
     * 
     * @param throttlePosition
     *               Throttle position needed to achieve the desired range shift.
     * 
     * @return Result of verifying the throttle is in the correct position.
     */
    virtual string CheckThrottlePosition ( const string throttlePosition );

    /**
     * Perform the t-case range shift to the desired gear.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The t-case range shift will be performed by either commanding the t-case to shift or prompting the operator to
     * perform the range shift to the desired gear.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> <i>[Test Step Name]</i>ShiftMethod - Shifting method to use to achieve the desired range shift.
     *                                                For example, Manual or Command.
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> GetTestStepName() </li>
     *      <li> CommandShiftTcase() </li>
     *      <li> TcaseTool::ShiftTcase() </li>
     *      </ul>
     * 
     * @return Result of performing the range shift to the desired gear.
     */
    virtual string ShiftTcase ( void );

    /**
     * Verify the range shift has completed.
     * <p><b>Category:</b> Utility
     * <p><b>Descrition:</b><br>
     * The vehicle will be checked to ensure the range shift to the desired gear has been completed.  The specified
     * <i>"verifyMethod"</i> will be used to determine how to validate the t-case shift.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseCheckCluster() </li>
     *      <li> TcaseCheckDriver() </li>
     *      <li> TcaseCheckGear() </li>
     *      <li> TcaseCheckSwitch() </li>
     *      <li> TcasePromptDriver() </li>
     *      </ul>
     * 
     * @param verifyMethod
     *               Method to use to verify the shift has been completed.  Valid methods are:
     *               <ul>
     *               <li> Cluster - Check the cluster lamps. </li>
     *               <li> Selector - Read t-case switch positions. </li>
     *               <li> Gear - Read the current t-case gear from the module. </li>
     *               <li> Driver/Prompt - Prompt operator if shift completed. </li>
     *               </ul>
     * 
     * @return Result of verifying the t-case shift completed successfuly.
     */
    virtual string VerifyTcaseShift ( const string &verifyMethod );

    /**
     * Send Command message to Tcase.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * A command will be sent to the module to perform the range shift to the desired gear.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandShiftTcase() </li>
     *      </ul>
     *
     * @return Result of commanding the t-case to the desired gear.
     */
    virtual string CommandShiftTcase ( void );

    /**
     * Read Tcase shift status using cluster module message.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Once the t-case range shift has been completed, the cluster lamps will be checked to ensure the correct lamp is
     * illuminated.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseCheckCluster() </li>
     *      </ul>
     * 
     * @return Result of verifying the correct cluster lamp is illuminated.
     */
    virtual string TcaseCheckCluster ( void );

    /**
     * Read Tcase shift status using Tcase module message.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The t-case gear will be read from the module and compared against the expected gear.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseCheckGear() </li>
     *      </ul>
     *
     * @return Result of validating the current t-case gear
     */
    virtual string TcaseCheckGear ( void );

    /**
     * Read Tcase switch position using Tcase module message.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The t-case switches will be read from the module and analyzed to ensure the range shift has completed successfuly.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseCheckSwitch() </li>
     *      </ul>
     * 
     * @return Result of validating the t-case switches are in the correct position.
     */
    virtual string TcaseCheckSwitch ( void );

protected:
    /**
     * Pointer to the test component that created this object so parameters can be retrieved.
     */
    TestComponent *m_tcaseComponent;
};

#endif  //ElectronicTcaseTool_h
