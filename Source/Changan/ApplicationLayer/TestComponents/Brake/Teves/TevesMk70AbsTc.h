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
    
    virtual string InitWinCcrtInterface(void);
    
    virtual string CloseWinCcrtInterface(void);
    
    virtual string StartModuleKeepAlive(void);
    
    virtual string StopModuleKeepAlive(void);
    
    virtual string RequestCoding(void);
    
    virtual string SendCoding(void);
    
    virtual string StartEolSession(void);
    
    virtual string ReadEcuId(void);

    virtual const bool IsCableConnected(void);

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
    void SetCoding2Auth(bool val);
    bool GetCoding2Auth();
    
    bool m_HaveCoding2Auth;
};
//-------------------------------------------------------------------------------------------------
#endif //TevesMk70AbsTc_h
