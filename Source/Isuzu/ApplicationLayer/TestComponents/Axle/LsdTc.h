//******************************************************************************
// Description:
//  Limited Slip Differential Test Component Header File
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: $
//
// 1    11/12/2010 11:40a KHarman
//  Documented code.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
//*****************************************************
#ifndef LsdTc_H
#define LsdTc_H
//*****************************************************

#include <string>
#include "GenericTC.h"
#include "IMotorController.h"

using namespace std;

/**
 * Limited Slip Differential test component.<br>
 * This test component is used to test if an axle is actually a standard or limited
 * slip differential axle.
 * <p>
 * @author Kyle Harman
 * @see GenericTC
 */
class LsdTc : public GenericTC
{
public:
    /**
     * <b>Description</b>
     * Class constructor.
     */
    LsdTc();

    /**
     * <b>Description</b>
     * Class destructor.
     */
    virtual ~LsdTc();

    /**
     * Initialize the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The test component will be initialized.  The component will then signal that initialization is complete.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::Initialize( </li>
     *      <li> InitializationComplete() </li>
     *      <li> Log() </li>
     *      </ul>
     *
     * @param config The configuration data to use to initialize the test component.
     */
    virtual void Initialize(const XmlNode *config);

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
    virtual const string CommandTestStep(const string &value);


protected:

    /**
     * Get the results of the test.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The results of the LSD test are returned.
     * <br><p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CreateMessage() </li>
     *      <li> SendTestResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> Log() </li>
     *      </ul>
     * 
     * @return The results of the LSD test.
     */
    virtual string GetTestResults(void);

    /**
     * Compare the two rear wheels and tell if they are going the same speed.
     * <p>
     * <b>Category:</b> Utility
     * <p>Description:</b><br>
     * The two rear wheels' speeds are compared. True is returned if they are equal.
     * <p>
     * <b>Functions Called:</b>
     *      N/A
     * 
     * @param wheelSpeed[] The current wheel speeds to compare.
     * @return The boolean of whether the rear wheels are moving at the same speed.
     */
    virtual bool SameSpeeds(float wheelSpeed[]);

    /**
     * Compare the two rear wheels and tell if they are going the same direction.
     * <p>
     * <b>Category:</b> Utility
     * <p>Description:</b><br>
     * The two rear wheels' directions are compared. True is returned if they are equal.
     * <p>
     * <b>Functions Called:</b>
     *      N/A
     * 
     * @param wheelSpeed[] The current wheel speeds to compare.
     * @return The boolean of whether the rear wheels are moving in the same direction.
     */
    virtual bool SameDirections(float wheelSpeed[]);

    /**
     * Start turning one of the rear wheels at a specified speed.
     * <p>
     * <b>Category:</b> Test Step
     * <p>Description: </b><br>
     * The parameters are read and one of the rear wheels is turned at the specified speed.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DisplayPrompt() </li>
     *      <li> DisplayTimedPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetPromptDuration() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameter() </li>
     *      <li> IMotorController::Write() </li>
     *      <li> StatusCheck() </li>
     *      <li> CreateMessage() </li>
     *      <li> SendTestResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     * 
     * @return The status of starting the motor.
     */
    virtual string TurnWheel(void);

    /**
     * Stop turning the wheels.
     * <p>
     * <b>Category:</b> Test Step
     * <p>Description :</b><br>
     * This turns the motors that control the wheels off.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IMotorController::Write() </li>
     *      <li> SystemCommand() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * @return The status of turning off the motors.
     */
    virtual string WheelsStop(void);

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
     * Interface object used to communicate with the motor controller.
     * Used for commanding non-standard motor modes (i.e. torque & speed 
     * simultaneously).
     */
    IMotorController m_MotorController;
    
    float fltWheelSpeeds[4];  // used to store the wheel speeds

};
#endif
