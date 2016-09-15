//*************************************************************************
// FILE:
//    $Header:
//
// FILE DESCRIPTION:
//  Allison transmission Test Component.
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
#ifndef AllisonTransmissionTC_h
#define AllisonTransmissionTC_h
//*************************************************************************
#include "GenericTransmissionTCTemplate.cpp"

/**
 * Test component to test the Allison Transmission..
 * 
 * @author 
 * @see GenericTransmissionTCTemplate
 */
template <class VehicleModuleType>
class AllisonTransmissionTC : public GenericTransmissionTCTemplate<VehicleModuleType>
{
public:
    /**
     * Class constructor.
     */
    AllisonTransmissionTC();
    /**
     * Class destructor.
     */
    virtual ~AllisonTransmissionTC();
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

      virtual string ClearFaults(void);
      virtual string ReadFaults(void);
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
    virtual string ShiftLeverTest(void);
    virtual string StartGearMonitor(void);
    virtual string AccelerateInReverse(void);
    virtual string StopGearMonitor(void);
    virtual string CheckPartNumbers(void);

    /** List of PRNDL positions to check for */
    XmlNodeMap m_prndlPositions;
};
//*************************************************************************
#endif  //AllisonransmissionTC_h
