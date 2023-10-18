//*************************************************************************
// FILE:
//    $Header:
//
// FILE DESCRIPTION:
//  Aisin transmission Test Component.
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
//*************************************************************************
#ifndef AisinTransmissionTC_h
#define AisinTransmissionTC_h
//*************************************************************************
#include "GenericTransmissionTCTemplate.cpp"

/**
 * Test component to test the Aisin Transmission..
 * 
 * @author 
 * @see GenericTransmissionTCTemplate
 */
template <class VehicleModuleType>
class AisinTransmissionTC : public GenericTransmissionTCTemplate<VehicleModuleType>
{
public:
    /**
     * Class constructor.
     */
    AisinTransmissionTC();
    /**
     * Class destructor.
     */
    virtual ~AisinTransmissionTC();
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
	 * Perform any test component specific initialization.  The market type will be evaluated to determine what type of 
	 * engine controller module interface to create.
	 * <br><p>
     * 
	 * @param config Configuration file to use for initialization.
     */
    virtual void InitializeHook(const XmlNode *config);

private:
    /**
     * Read the current fault count from the module.
     * If any faults are present, the test may be aborted depending on parameter settings.
     * 
     * @return Result of check the module for faults.
     */
    string CheckFaultCount(void);
    virtual string ClearFaults(void);

    /**
     * The torque converter clutch slip error will be rear from the module.
     * The component will wait for some amount of time for the slip error to be reduced to an acceptable level that indicates 
     * the torque converter clutch is fully enggaged.
     * 
     * @return Result of evaluating the torque converter clutch slip error.
     */
    string EvaluateTccSlipError(void);

    /**
     * Verify PRNDL display, switches, TIPM Park Neutral state and gear.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The shift lever will be tested to ensure the switches and display are consistent.
     * <p>
     * 
     * @return Result of testing the PRNDL position.
     */
    string ShiftLeverTest(void);

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



    virtual string ReadFaults(void);

    virtual string ExtraAccelerationTest(void);


    /**
     * Check the Overdrive switch. Looking for <i>any</i> state change.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The initial state of the overdrive switch will be read from the module.  The operator will then be instructed to
     * disable the overdrive by pressing the o/d cancel button.  The resultant state will be read from the module.  The
     * test will pass as long as the resultant state is different than the original state read from the module.  The
     * result of the test will be stored in the module.
     * <p>
     * 
     * @return Result of checking the functionality of the O/D Cancel switch and recording the result in the module.
     */
//    string CheckOverdriveSwitch(void);

    /**
     * Evaluate the torque converter clutch slip data.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The torque converter data will be read and analyzed to ensure the clutch is operating within tolerance.
     * The operator will be instructed to accelerate to the proper speed range prior to evaluating the torque converter
     * data.
     * <p>
     * 
     * @return Result of evaluating the torque converter slip and updating the result in the module.
     */
//    virtual string EvaluateTorqueConverterClutchSlip(void);

private:
    /** List of PRNDL positions to check for */
    XmlNodeMap m_prndlPositions;
    XmlNodeMap m_SolenoidDtcs;
    bool m_extraTest;
};
//*************************************************************************
#endif  //AisinTransmissionTC_h
