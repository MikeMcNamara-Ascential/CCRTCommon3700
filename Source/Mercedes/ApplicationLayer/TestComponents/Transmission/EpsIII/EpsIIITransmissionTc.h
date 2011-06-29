//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the Mercedes EPS-III Transmission.
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
#ifndef EpsIIITransmissionTc_h
#define EpsIIITransmissionTc_h
//-------------------------------------------------------------------------------------------------
#include "GenericTC.h"
//-------------------------------------------------------------------------------------------------
class EpsIIITransmissionTc : public GenericTC
{
public:
    EpsIIITransmissionTc();
    virtual ~EpsIIITransmissionTc();

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
    virtual void Initialize(const XmlNode *config);

protected:
    /**
     * Prompt the operator to enter the pass/fail result for shift quality.
     * 
     * @return Operator result for shift quality.
     */
    string CheckShifting(void);

    /**
     * Select the shifting mode of the transmission
     * 
     * @return Result of instructing the operator to select the specified shifting mode.
     */
    string SelectMode(const string &mode);

    /**
     * Verify the vehicle can accelerate to the required speed in the specified gear.
     * 
     * @param gear   Transmission gear to analyze.
     * 
     * @return Result of verifying vehicle speed for the specified transmission gear.
     */
    string SpeedBasedGearTest(const string &gear);

private:
    /**
     * Check the system data to determine if the vehicle is equipped with EPS-III transmission.
     * 
     */
    void CheckIfEpsTransmissionEquipped(void);

    /**
     * Get/Set the flag indicating if the vehicle is equipped with EPS-III transmission.
     * 
     * @param equipped Flag indicating if the vehicle is equipped with EPS-III transmission.
     * 
     * @return Flag indicating if the vehicle is equipped with EPS-III transmission.
     */
    const bool& EpsTransmissionEquipped(const bool *equipped = NULL);

    /** Flag used to track if the truck is equipped with EPS-III transmission */
    bool m_isEpsTransEquipped;
};

//-------------------------------------------------------------------------------------------------
#endif //EpsIIITransmissionTc_h
