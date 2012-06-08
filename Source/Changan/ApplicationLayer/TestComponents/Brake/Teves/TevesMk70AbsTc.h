//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the Teves MK70 ABS.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#ifndef TevesMk70AbsTc_h
#define TevesMk70AbsTc_h
//-------------------------------------------------------------------------------------------------
#include "GenericABSTCTemplate.cpp"
//-------------------------------------------------------------------------------------------------
template<class ModuleType>
class TevesMk70AbsTc : public GenericABSTCTemplate<ModuleType>
{
public:

    /**
     * Constructor
     */     
    TevesMk70AbsTc();
    
    /**
     * Destructor
     */    
    virtual ~TevesMk70AbsTc();

    /**
     * Handle execution of each test step specified by system for each
     * test objective passed to it.  Overloaded to execute component tests.
     * 
     * @param value  value commanded by test sequencer.
     * 
     * @return The status of the overall objective.
     */
    virtual const string CommandTestStep(const string &value);
    
protected:
    /**
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Perform any test component specific initialization.
     * <br><p>
     *
     * @param config Configuration file to use for initialization.
     */
    virtual void InitializeHook(const XmlNode *config);

    /**
     * Validate the wheel speed sensors.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The wheel speed sensors will be compared to the roller speeds to ensure each sensor is within the allowed
     * tolerance.  Each roller will be rotated individually and each wheel checked to make sure the module knows the
     * correct wheel is being rotated.
     * <br><p>
     *
     * @return The result of validating the wheel speed sensors.
     *      <ul>
     *      <li> Pass - All wheel speed sensors are within tolerance. </li>
     *      <li> Fail - One or more wheel speed sensors are out of tolerance or wheel speeds could not be gathered
     *                  from the vehicle or system. </li>
     *      <li> Timeout - Target speed was not achieved in the time allowed. </li>
     *      <li> Skip - The wheel speed sensors do not need to be validated. </li>
     *      <li> SoftwareFail - A module exception occurred reading the wheel speed sensors. </li>
     *      </ul>
     */
    virtual string SensorTest(void);

	/**
	 * Interface object used to communicate with the WinCcrtInterface server.
	 */
    IWinInterfaceServer m_WinCcrtInterface;
    
    /**
     * Sends the 5 baud initialization sequence used to command the module to
     * enter diagnostic mode.
     *
     * @return The result of the method.
     */
    string PerformModuleLinkup(void);

    /**
     * Command the module to begin the EOL testing mode.
     * 
     * @param value This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */ 
    const std::string TestStepBeginEolMode(const std::string &value);

    /**
     * Send the EOL coding message to the module.
     * 
     * @param value This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */ 
    const std::string TestStepEolCoding(const std::string &value);

    /**
     * Command the module to enter the EOL testing mode.
     * 
     * @param value This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */ 
    const std::string TestStepEnterEolMode(const std::string &value);

    /**
     * Apply a constant torque to all rollers.
     *  
     * @param value This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */
    virtual const std::string TestStepApplyTorqueToAllRollers(const std::string &value);

    /**
     * Helper function to apply torque and send test result values.
     *  
     * @param lfTorque  Left front torque value
     * @param rfTorque  Right front torque value
     * @param lrTorque  Left rear torque value
     * @param rrTorque  Right rear torque value
     * 
     * @return The status of the test step.
     */
    virtual const std::string ApplyTorqueToRollers(string lfTorque, string rfTorque, string lrTorque, string rrTorque);

    /**
     * Run the low speed valve firing test.  First the rolls will be commanded 
     * to a constant speed.  Then the driver will be prompted to apply the 
     * brake.  Commands will then be sent to the module to reduce and build the 
     * brake pressure.  The motor controller will store the motor load during 
     * this time for later evaluation.
     * 
     * @param value This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */ 
    virtual const std::string TestStepLowSpeedValveFiringTest(const std::string &value);

    /**
     * Perform valve firing reduction.
     * 
     * @param wheel The wheel to reduce.
     * 
     * @return The status of the command.
     */
    virtual string Reduction(const string &wheel);

    /**
     * Perform valve firing recovery.
     * 
     * @param wheel The wheel to recovery.
     * 
     * @return The status of the command.
     */
    virtual string Recovery(const string &wheel);

    /**
     * Apply zero torque to all rollers.
     *  
     * @param value This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */
    virtual const std::string TestStepTorqueZero(const std::string &value);
    /**
     * <b>Description:</b>
     * Method used to abort the current processing based on the needs of the 
     * component.
     */
    virtual void Abort(void);    

private:
    /**
     * Rotate the specified roller at the specified speed for the specified time.
     * 
     * @param rollerIndex
     *                Roller to rotate.
     * @param rollerSpeed
     *                Speed at which to rotate the rollers.
     * @param rotationTime
     *                Length of time to rotate the rollers.
     * @param details Result details for each sensor tested.
     * 
     * @return Result of testing the specified wheel speed sensor.
     */
    string RotateRoller(const UINT8 &rollerIndex, const string &rollerSpeed, 
                        const INT32 &rotationTime, TestResultDetails &details);
};
//-------------------------------------------------------------------------------------------------
#endif //TevesMk70AbsTc_h
