//*************************************************************************
// FILE DESCRIPTION:
//  Module Interface for the Bosch ABS.
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
#ifndef BoschAbsTc_h
#define BoschAbsTc_h
//-------------------------------------------------------------------------------------------------
#include "GenericABSTCTemplate.cpp"
//-------------------------------------------------------------------------------------------------
template<class AbsModuleType>
class BoschAbsTc : public GenericABSTCTemplate<AbsModuleType>
{
public:
    BoschAbsTc();
    virtual ~BoschAbsTc();

    /**
     * Sequence a test step.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     * 
     * @param value    This is the value that was commanded by the test sequencer.
     * @return  The result of the test step that was executed.
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

private:
    /**
     * Verify the steering angle sensor is aligned correctly.
     * 
     * @return Result of checking the angle of the steering sensor.
     */
    string CheckSteeringAngleSensor();

    /**
     * Disable the diagnostic speed limit in the module.
     * 
     * @return Result of commanding the module to disable the diagnostic speed limit.
     */
    string DisableDiagnosticSpeedLimit(void);

    /**
     * The sensor quality test will be performed.
     * The ABS module is commanded to start the sensor quality test.
     * The rollers are then accelerated to the test speed or a time.
     * The results are then read from the module.
     * 
     * @return Result of the sensor quality test as read from the module.
     */
    string PerformSensorQualityTest();

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
#endif //BoschAbsTc_h
