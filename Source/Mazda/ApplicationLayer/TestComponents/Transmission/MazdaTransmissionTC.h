//******************************************************************************
// Description:
//  Manual transmission component class
//
//==============================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Tata/Source/Tata/ApplicationLayer/TestComponents/Transmission/MazdaTransmissionTC.h $
// 
// 3     3/09/06 12:54a Bmeinke
// Modified how the acceleration test time is calculated: the start time
// is set to the point where we lose zerospeed
// Changed unit strings passed to the test result server to the new unit
// string constants from GenericTC
// Added TestStepFinishUp()
// 
// 2     3/07/06 12:36a Bmeinke
// Added an AccelerationTest
// 
// 1     3/06/06 10:25a Bmeinke
// Initial checkin
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _MAZDA_TRANSMISSIONTC_H_INCLUDED_
#define _MAZDA_TRANSMISSIONTC_H_INCLUDED_

#include "GenericTC.h"	// inherited from class

typedef struct
{
    string prompt1;
    string prompt1Value1;
    string prompt1Value2;
    string prompt2;
    string prompt2Value1;
    string prompt2Value2;
    string promptSpeedHigh;
    string promptSpeedHighValue1;
    string promptSpeedHighValue2;
    string promptSpeedLow;
    string promptSpeedLowValue1;
    string promptSpeedLowValue2;
    float targetSpeed;
    float speedTolerance;
    float endTime;
} DRIVECURVE_PARAMETERS;

            
/**
 * Manual Transmission Test Component.  This
 * Class tests the Manual Transmission to ensure it works
 * properly as require by the Process Standard
 */
class MazdaTransmissionTC : public GenericTC
{
public:
    /**
    * Class Constructor.
    * @since Version 1.0
    */
    MazdaTransmissionTC();
    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~MazdaTransmissionTC();

    /**
     * Overloaded initialization
     * 
     * @param fileName
     */
    void Initialize(const string &fileName);
    void Initialize(const XmlNode *config);

    /**
     * This is the method that will handle the execution of each
     * test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded
     * to execute the component tests.
     * 
     * @param value  This is the value that was commanded by
     *               the test sequencer.
     * 
     * @return The status of the overall objective.
     */
    virtual const string CommandTestStep(const string &value);




protected:


    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 2.1
     */
    virtual const string Publish(const XmlNode *node);

    /**
     * Finishup test step that always returns overall as pass.
     *
     * @param value    This is the value that was commanded by the test sequencer.
     * @return The result of the test.
     */
    virtual const string TestStepFinishUp(const string &value);

    void RetrieveDriveCurveParameters(void);


    const string FollowDriveCurve(void);

    const string PromptDriveCurveSection(DRIVECURVE_PARAMETERS driveCurveParams, float previousTargetSpeed, float previousEndTime);

    /**
     * This test step will prompt the driver to brake to stop and wait untill
     * zerospeed is achieved or a timeout occurs.
     * it if required.
     * 
     * @param value  This is the value that was commanded by the test sequencer.
     * 
     * @return The result of the test.
     */
    virtual const string TestStepBrakeToStop(const string &value);

    /**
     * Time at which the accel test was started
     */
    time_t      m_accelTestStartTime;
    /**
     * Vector to contain drivecurve parameters for drivecurve test step.
     */
    vector<DRIVECURVE_PARAMETERS> m_driveCurveParameters;

    /** map containing all drive curve parameters */
    XmlNodeMap m_driveCurveParametersNodeMap;


};

#endif	// _MAZDA_TRANSMISSIONTC_H_INCLUDED_
