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
    TevesMk70AbsTc();
    virtual ~TevesMk70AbsTc();

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
