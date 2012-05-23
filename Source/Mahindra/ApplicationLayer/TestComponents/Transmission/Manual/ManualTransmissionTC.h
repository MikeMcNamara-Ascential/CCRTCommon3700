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
//     $Log: /Tata/Source/Tata/ApplicationLayer/TestComponents/Transmission/Manual/ManualTransmissionTC.h $
// 
// 3     3/09/06 12:54a Bmeinke
// Modified how the acceleration test time is calculated: the start time
// is set to the point where we lose zerospeed
// Changed unit strings passedto the test result server to the new unit
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
#ifndef _MANUAL_TRANSMISSIONTC_H_INCLUDED_
#define _MANUAL_TRANSMISSIONTC_H_INCLUDED_

#include "GenericTC.h"	// inherited from class

/**
 * Manual Transmission Test Component.  This
 * Class tests the Manual Transmission to ensure it works
 * properly as require by the Process Standard
 */
class ManualTransmissionTC : public GenericTC
{
public:
    /**
    * Class Constructor.
    * @since Version 1.0
    */
    ManualTransmissionTC();
    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~ManualTransmissionTC();

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
	 * Perform a blind gear speed test.
	 * The speeds on the user display will be blanked out.
	 * The driver will then be instructed to shift to the specified gear and accelerate to the max speed in that gear.
	 * After the driver notes the maximum speed, they will press the pass button to continue.
	 * Pass/Fail determination is optional.
	 * 
	 * @param gear   Transmission gear to use for this speed check.
	 * 
	 * @return Result of the speed check.
	 */
	const string BlindGearSpeedCheck(const string &gear);

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
     * Prompt the driver to shift to a gear and accelerate until the test timeout
     * expires
     * 
     * @param value  The gear to shift to
     * 
     * @return The result of the test.
     */
    virtual const string TestStepShiftTest(const string &value);

    void SetShiftTestParameters(string &targetGear, int &scanDelay, float &targetSpeed, 
                                int &numberOfGears, int &gearFactor);

    /**
     * Prompt the driver to shift to a gear and accelerate until the test timeout
     * expires
     * 
     * @param value  The gear to shift to
     * 
     * @return The result of the test.
     */
    virtual const string TestStepSpeedBasedShiftTest(const string &value);

    void SetSpeedBasedShiftTestParameters(int &targetGear, float &targetSpeed, 
                                          float &targetSpeedTolerance, float &targetSpeedMin, 
                                          float &targetSpeedMax, int &scanDelay, 
                                          int &numberOfGears, int &maxSpeedDelay);
    /**
     * Prompt the driver to accelerate to the vehicle's defined top speed
     * 
     * @param value  This is the value that was commanded by
     *               the test sequencer
     * 
     * @return The result of the test.
     */
    virtual const string TestStepAccelerationTest(const string &value);

    void SetAccelerationTestParameters(int &scanDelay, float &startSpeed, 
                                       float &endSpeed, float &maxTime);

    /**
     * Finishup test step that always returns overall as pass.
     *
     * @param value    This is the value that was commanded by the test sequencer.
     * @return The result of the test.
     */
    virtual const string TestStepFinishUp(const string &value);

    /**
     * Prompts operator to manually buy-off idle engine RPM.
     * 
     * @param value  Value commanded by test sequencer
     * 
     * @return string testResult
     */
    virtual const string TestStepVerifyIdleRPM(const string &value);

    /**
     * Time at which the accel test was started
     */
    time_t      m_accelTestStartTime;
};

#endif	// _MANUAL_TRANSMISSIONTC_H_INCLUDED_
