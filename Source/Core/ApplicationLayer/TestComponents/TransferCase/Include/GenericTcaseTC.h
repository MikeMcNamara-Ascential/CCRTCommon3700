//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/TestComponents/TransferCase/Include/GenericTcaseTC.h 4     10/30/07 3:23p Rwiersem $
//
// FILE DESCRIPTION:
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/TransferCase/Include/GenericTcaseTC.h $
// 
// 4     10/30/07 3:23p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added Javadoc comments.
// 
// 3     8/18/04 11:16p Cward
// changed angle brackets to quotes for included non system files
//
// 2     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:33p Cward
//
// 1     10/28/03 7:55a Derickso
// Created.
//
// 9     9/30/03 7:19p Bbarrett
// Change and update methods
//
// 8     9/29/03 6:03p Bbarrett
// Continue development
//
// 7     9/17/03 2:59p Bbarrett
// Add get ratio (samples / time).
//
// 6     9/11/03 3:44p Bbarrett
// Clean up code. Change JavaDoc comments.  Version 1.0 done.  Not
// compiled yet.
//
// 5     9/10/03 1:18p Bbarrett
// Add comments.
//
// 4     9/09/03 4:41p Bbarrett
// Add axle ratio methods. Not complete yet
//
// 3     8/29/03 2:58p Bbarrett
// Add more shift test. reconfigure test.  Not complete yet.
//
// 2     8/25/03 5:21p Bbarrett
// Add in ShiftToLow method.
//
// 1     8/22/03 5:31p Bbarrett
// Initial work on Tcase
//
// 6     8/13/03 4:22p Derickso
// Renamed to follow new naming convention.
//
// 5     8/12/03 3:45p Derickso
// Split implementation out into cpp file.
//
// 4     7/08/03 10:23a Derickso
// Updated parameter macros to be more readable.
// Restructured data in the config files.
// Compiles, not tested.
//
// 3     6/09/03 8:05a Derickso
// Corrected compiler errors.
// Compiles, not tested.
//
// 2     5/28/03 9:14a Derickso
// Converted to template.
//
// 1     5/27/03 3:31p Derickso
// Periodic backup.
//
// 1     5/23/03 5:02p Derickso
// Periodic backup
//
//*************************************************************************
#ifndef GenericTcaseTC_h
#define GenericTcaseTC_h
//*************************************************************************

#include "TcaseTool.h"
#include "GenericTC.h"
#include <strstream.h>
#include <math.h>

using std::string;


/**
 * Generic Transfer Case test component.<br>
 * This test component is mostly a front end for the Generic Tcase utility class, TcaseTool.
 * <p>
 * @author Bill Barrett
 * @see TcaseTool
 * @see GenericTC
 */
class GenericTcaseTC : public GenericTC
{
public:
    /**
     * <b>Description</b>
     * Class constructor.
     */
    GenericTcaseTC();
    /**
     * <b>Description</b>
     * Class destructor.
     */
    virtual ~GenericTcaseTC();
    /**
     *
     * Initialize the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The test component will be initialized.  The component will then signal that initialization is complete.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> InitializeHook() </li>
     *      <li> InitializationComplete() </li>
     *      <li> GetProcessName() </li>
     *      </ul>
     *
     * @param config The configuration data to use to initialize the test component.
     */
    virtual void Initialize(const XmlNode *config);

    /**
	 * Method that is called to command the component to perform any
	 * system registration functions necessary.
	 *
	 * @return Status of the registration process.
	 *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
	 *		   BEP_SUCCESS_RESPONSE == Registered
	 */
	virtual const string Register(void);

    /**
     * Initialize the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Class specific initialization will be completed.  Once the class specific initialization is complete, the base 
     * class will be called to compelte the initialization.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::Initialize() </li>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @param configFile Configuration data to use for initialization.
     */
    const void InitializeHook(const XmlNode *config);
    /**
     * Perform a test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The next test step in the commanded test objective will be performed.  The result will then be returned to the
     * calling function.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Test Step Functions Called:</b>
     *      <ul>
     *      <li> CheckTcaseShift() </li>
     *      <li> CheckAxleRatio() </li>
     *      <li> CheckAxleBalance() </li>
     *      <li> FinishUp() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendOverallResult() </li>
     *      <li> GenericTC::CommandTestStep() </li>
     *      </ul>
     * 
     * @param value    The value that was commanded by the test sequencer.
     * @return The result of the test step just completed.
     */
    virtual const std::string CommandTestStep(const string &value);

protected:

    /**
     * Validate a range shift.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Verify the T-case shifts to the desired postion.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::CheckTcaseShift() </li>
     *      </ul>
     * 
     * @return Result of validating the t-case range shift.
     */
    virtual string CheckTcaseShift ( void );

    /**
     * Validate vehicle conditions prior to shifting.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The vehicle conditions will be validated to ensure the desired range shift can be completed.  If the vehicle is
     * not in the correct state, the operator will be provided with instructions for getting the vehicle into the proper
     * state to complete the range shift.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::PreconditionsForShift() </li>
     *      </ul>
     * 
     * @return Result of verifying the vehicle is in the correct state to perform the range shift.
     */
    virtual string PreconditionsForShift ( void );

    /**
     * Verify proper vehicle speed.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The vehicle speed will be checked to ensure it is in the proper range to perform the range shift.  If the vehicle
     * speed is out of range, the operator will be prompted to achieve the desired speed range.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::CheckVehicleSpeed() </li>
     *      </ul>
     * 
     * @return Result of validating the proper speed range.
     */
    virtual string CheckVehicleSpeed ( void );

    /**
     * Place the transmission in the correct gear.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b>
     * The operator will be instructed to shift the transmission to the required gear to perform the range shift in the
     * t-case.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::ShiftTransmission() </li>
     *      </ul>
     *
     * @param transGear Gear to place the transmission into.
     * 
     * @return Result of validating the transmission shift.
     */
    virtual string ShiftTransmission ( const string transGear );

    /**
     * Switch the ignition to the indicated position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Prompt the operator to place the ingnition in the desired position so the range shift can be performed.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::TurnKeyOnOff() </li>
     *      </ul>
     *
     * @param keyPosition  Set key position to the "On" or "Off" position.
     * 
     * @return Result of placing the ignition in the desired position.
     */
    virtual string TurnKeyOnOff ( const string keyPosition );

    /**
     * Place the engine in the desired run state.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be instructed to place the engine in the specified run state.  In some instances, the engine 
     * must be off in roder to complete a range shift.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::EngineOnOff() </li>
     *      </ul>
     *
     * @param engineRun State in which to place the engine "On"/"Off"
     * 
     * @return Result of placing the engine in the desired run state.
     */
    virtual string EngineOnOff ( const string engineRun );

    /**
     * Verify the throttle is in the correct position.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * If the throttle is not in the correct position, the operator will be instructed to either apply throttle, or 
     * remove his foot from the throttle.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::CheckThrottlePosition() </li>
     *      </ul>
     *
     * @param throttlePosition Position in which to place the throttle - "Open"/"Closed".
     * 
     * @return Result of placing the throttle in the specified position.
     */
    virtual string CheckThrottlePosition ( const string throttlePosition );

    /**
     * Prompt the operator to shift the transfer case.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to shift the transfer case to the esired gear range.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::ShiftTcase() </li>
     *      </ul>
     * 
     * @return Result of shifting the transfer case.
     */
    virtual string ShiftTcase ( void );

    /**
     * Verify the range shift completed successfully.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * ONce the range shift has been completed, the t-case will be checked using the specified verification method
     * specified.  The current t-case gear will be checked to ensure the range shift completed successfully.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::VerifyTcaseShift() </li>
     *      </ul>
     *
     * @param verifyMethod  How to verify the range shift has completed successfully.
     * 
     * @return Result of verifying the range shift completed successfully.
     */
    virtual string VerifyTcaseShift ( const string &verifyMethod );

    /**
     * Prompt operator for range shift status.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to enter the result of the range shift.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::TcaseCheckDriver() </li>
     *      </ul>
     * 
     * @return Input from the operator concerning the range shift completion status.
     */
    virtual string TcaseCheckDriver( void );

    /**
     * Instruct the operator to shift the transfer case.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be instructed to shift the t-case to the desired gear range.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::TcasePromptDriver() </li>
     *      </ul>
     *
     * @return Result of the operator shifting the t-case to the desired gear range.
     */
    virtual string TcasePromptDriver ( void );

    /**
     * Validate the axle ratio.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The front to rear axle ratio will be verified to ensure the axles are within limits.  If an incorrect axle is
     * installed in the vehicle, this test will detect that situation and fail the vehicle.  This test is best performed
     * when the t-case is in 4-Lo, 4-Hi or 4-Lock, or any mode where the front to rear ratio should be 1:1.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::CheckAxleRatio() </li>
     *      </ul>
     *
     * @return Result of validating the front to rear axle ratio is within tolerance.
     */
    virtual string CheckAxleRatio ( void );

	/**
     * Validate front to rear axle balance.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The front to rear axle balance will be checked to ensure the non-driven wheels are coming up to speed within a certain
     * speed delta of the driven wheels.  This test is best performed with the t-case in an all wheel drive, part-time
     * or full-time mode or some other mode where the driven to non-driven axle ratio is not 1:1.
	 * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::CheckAxleBalance() </li>
     *      </ul>
     *
	 * @return Result of verifying the axle balance.
	 */
	virtual string CheckAxleBalance(void);

    /**
     * Get front to rear axle ratio.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The average front and rear roll speeds will be sampled.  The front to rear ratio will then be calculated.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::GetAxleRatio() </li>
     *      </ul>
     *
     * @param ratio  Calculated front to rear axle ratio to return to the calling function.
     * 
     * @return Result of calculating the axle ratio.
     */
    virtual string GetAxleRatio( float &ratio );

	/**
	 * Finish up testing.
     * <p><b>Category:</b> Test Step.
     * <p><b>Description:</b><br>
     * Perform any test component specific finish up processing.  Nothing specific is done in this function.
	 * <p>
	 * @return Pass
	 */
	virtual string FinishUp(void);

    /**
	 * Accelerate to test speed.
	 * <br><p>
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * Generic step used to get vehicle to desired speed. Displays speed target green band, prompts operator 
	 * to speed, and check the roller speeds to verify speed has been reached. Return pass or fail.
	 * <br><p>
	 *
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> (GetTestStepName()+ "TargetSpeed")- The desired test speed float value. </li>
	 * 		<li> (GetTestStepName() + "TargetSpeedRange") - The green band speed range to be displayed. </li>
	 * 		</ul>
	 * 
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo("Description") - Description of the test step. </li>
	 *		<li> GetTestStepInfoInt("ScanDelay") - How long to wait between wheel speed checks. </li>
	 * 		</ul>
	 * 
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> None - returns pass or fail. </li>
	 * 		</ul>
	 * 
	 * <b>Prompts:</b>
	 * 		<ul>
	 * 		<li> GetTestStepName()+"TargetSpeed") - Instruct operator to accelerate to speed. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> Log() </li>
	 * 		<li> ShortCircuitTestStep() </li>
	 * 		<li> AccelerateToTestSpeed() </li>
	 * 		<li> GetParameterFloat() </li>
	 * 		<li> GetTestStepName() </li>
	 * 		<li> GetParameter() </li>
	 * 		<li> GetTestStepInfoInt() </li>
	 * 		<li> SendTestResult() </li>
	 * 		<li> c_str() </li>
	 * 		<li> GetTestStepInfo() </li>
	 * 		</ul>
	 * 
	 * @return Result vehicle reached target speed.
	 * 		<ul>
	 * 		<li> Pass - The module interface successfully started monitoring switches. </li>
	 * 		<li> Fail - Switch monitor could not be started. </li>
	 * 		<li> Skip - The switches do not need to be verified. </li>
	 * 		</ul>
	 */
	virtual string TcaseAccelerateToSpeed(void);

	/**
	 * Coast to stop.
	 * <br><p>
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * Generic step for costing to stop. Displays speed target green band, prompts operator to coast
	 * down to speed, and checks the roller speeds to verify speed has been reached. Return pass or fail.
	 * <br><p>
	 *
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> CoastToStopSpeed - The desired test speed float value. </li>
	 * 		<li> CoastToStopAllowBraking - Allow using brake to completely stop vehicle bool value. </li>
	 * 		</ul>
	 *
	 * <b>Data Tags:</b><br>
	 * 		<ul>
	 * 		<li> SpeedTarget - Prompt name. </li>
	 * 		</ul>
	 * 
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo("Description") - Description of the test step. </li>
	 *		<li> GetTestStepInfoInt("ScanDelay") - How long to wait between wheel speed checks. </li>
	 * 		</ul>
	 * 
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> None - returns pass or fail. </li>
	 * 		</ul>
	 * 
	 * <b>Prompts:</b>
	 * 		<ul>
	 * 		<li> CoastToStop - Instruct operator to coast to stop. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> Log() </li>
	 * 		<li> ShortCircuitTestStep() </li>
	 * 		<li> GetParameterFloat() </li>
	 * 		<li> CreateMessage() </li>
	 * 		<li> GetWheelCount() </li>
	 * 		<li> SystemWrite() </li>
	 * 		<li> GetDataTag() </li>
	 * 		<li> SendTestResult() </li>
	 * 		<li> DisplayPrompt() </li>
	 * 		<li> GetPromptBox() </li>
	 * 		<li> GetPrompt() </li>
	 * 		<li> GetTestStepInfoInt() </li>
	 * 		<li> GetWheelSpeeds() </li>
	 * 		<li> abs() </li>
	 * 		<li> BposSleep() </li>
	 * 		<li> GetTestStepName() </li>
	 * 		<li> ConvertStatusToResponse() </li>
	 * 		<li> c_str() </li>
	 * 		<li> TimeRemaining() </li>
	 * 		<li> StatusCheck() </li>
	 * 		<li> RemovePrompt() </li>
	 * 		<li> ReadSubscribeData() </li>
	 * 		<li> GetParameterBool() </li>
	 * 		<li> SendTestResult() </li>
	 * 		<li> GetTestStepInfo() </li>
	 * 		</ul>
	 * 
	 * @return Result vehicle reached coast down speed.
	 * 		<ul>
	 * 		<li> Pass - The module interface successfully started monitoring switches. </li>
	 * 		<li> Fail - Switch monitor could not be started. </li>
	 * 		<li> Skip - The switches do not need to be verified. </li>
	 * 		</ul>
	 */
	virtual string CoastToStop(void);

	/**
	 * Check with operator if Tcase is functional.
	 * <br><p>
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * Generic step used to get pass or fail from the vehicle operator.
	 * <br><p>
	 *
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> None. 
	 * 		</ul>
	 * 
	 * <b>Data Tags:</b><br>
	 * 		<ul>
	 * 		<li> (testStepName  + "Prompt") - Prompt name. </li>
	 * 		</ul>
	 * 
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo("Description") - Description of the test step. </li>
	 *		<li> GetTestStepInfoInt("Timeout") - How long to wait for operator input. </li>
	 * 		</ul>
	 * 
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> None </li>
	 * 		</ul>
	 *
	 * <b>Prompts:</b>
	 * 		<ul>
	 * 		<li> (testStepName  + "Prompt") - Instruct operator to press Pass or Fail test. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> Log() </li>
	 * 		<li> ShortCircuitTestStep() </li>
	 * 		<li> GetComponentName() </li>
	 * 		<li> GetTestStepName() </li>
	 * 		<li> c_str() </li>
	 * 		<li> OperatorPassFail() </li>
	 * 		<li> GetDataTag() </li>
	 * 		<li> GetTestStepInfoInt() </li>
	 * 		<li> SendTestResult() </li>
	 * 		<li> GetTestStepInfo() </li>
	 * 		</ul>
	 * 
	 * @return Result of starting the switch monitor.
	 * 		<ul>
	 * 		<li> Pass - The module interface successfully started monitoring switches. </li>
	 * 		<li> Fail - Switch monitor could not be started. </li>
	 * 		<li> Skip - The switches do not need to be verified. </li>
	 * 		</ul>
	 */
	virtual string GetTcasePassFail(void);

    /**
     * Check if the vehicle is equipped with a T-Case Version 1.0
     * 
     * @return True - vehicle is equipped with a T-Case.
     * @since Version 1.0
     */
    virtual bool& IsTcaseEquipped(void);

    /**
      * The Generic Tcase utility object that contains most of the funtionality for Tcase testing.
      */
    TcaseTool *m_tcaseTool;

private:
	/**
	 * Store the flag indicating if the vehicle is equipped
	 * with a T-Case
	 * 
	 * @param isEquipped Flag indicating if the vehicle is equipped with a T-Case.
	 * @since Version 1.0
	 */
	virtual void SetTcaseEquipped(const bool &isEquipped);
	/**
	 * Flag indicating if the vehicle is equipped with a T-Case
	 * @since Version 1.0
	 */
	bool m_tcaseEquipped;
    	/**
	 * Store XML Node map which contains the T-Case types.
	 * for reading part number data
	 * @since Version 1.0
	 */
	XmlNodeMap m_tcaseTypes;
};

#endif  //GenericTcaseTC_h
