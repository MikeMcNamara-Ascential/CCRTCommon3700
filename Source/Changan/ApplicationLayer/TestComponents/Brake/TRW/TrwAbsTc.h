//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the TRW ABS.
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
#ifndef TrwAbsTc_h
#define TrwAbsTc_h
//-------------------------------------------------------------------------------------------------
#include "GenericABSTCTemplate.cpp"
//-------------------------------------------------------------------------------------------------


/**
 * Possible steps for each wheel in the ESP test
 */
typedef enum
{
    ESP_INIT,
    ESP_PUMP_ON,
    ESP_PRIMARY_ON,
    ESP_START,
    ESP_PRIMARY_OFF,
    ESP_END,
    ESP_FINALIZE,
    ESP_DONE
} ESPSteps_t;


// Create a structure to contain build/reduction indices for each wheel
typedef struct _BuildReductionIndex
{
   INT32 buildStart;
   INT32 buildEnd;
   INT32 reductionStart;
   INT32 reductionEnd;
} BuildReductionIndex;



template<class ModuleType>
class TrwAbsTc : public GenericABSTCTemplate<ModuleType>
{
public:
    TrwAbsTc();
    virtual ~TrwAbsTc();

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
     * This test step runs the ESP Valve firing test.
     * 
     * @return The status of the operation.
     * @since 25 April 2012
    */
    virtual string ESPValveFiringTest(void);

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
     * The test step will evaluate the ESP build and reduction forces and also
     * check for an ESP valve cross.
     * 
     * @return The status of the test step.
     * @since 4 May 2012
     */ 
    virtual string EvaluateESP(void);

    /**
     * Analyze the ESP build force for the specified wheel and report the 
     * results.
     * 
     * @param roller The roller to analyze.
     * @param start  The start index for the build pulse.
     * @param end    The end index for the build pulse.
     * @return The result of the check.
     *         BEP_STATUS_SUCCESS or BEP_STATUS_FAILURE
     */
    virtual BEP_STATUS_TYPE AnalyzeESPBuildForces(INT32 roller, INT32 start, INT32 end);

    /**
     * Analyze the ESP reduction force for the specified wheel and report the 
     * results.
     * 
     * @param roller The roller to analyze.
     * @param start  The start index for the build pulse.
     * @param end    The end index for the build pulse.
     * @return The result of the check.
     *         BEP_STATUS_SUCCESS or BEP_STATUS_FAILURE
     */
    virtual BEP_STATUS_TYPE AnalyzeESPReductionForces(INT32 roller, INT32 start, INT32 end);



    /**
    * Index to the start of the ESP valve firing test.
    */ 
   INT32 m_ESPStartIndex;

   /**
    * Index to the end of the ESP valve firing test.
    */ 
   INT32 m_ESPEndIndex;

   /**
    * ESP Build and Reduction start and end indices for each wheel.
    * @since 19 December 2003
    */
   BuildReductionIndex *m_ESPIndex;

   /**
    * this holds the recalculated drag forces, based on the
    * average of the 5 measured values prior to the ESP build command
    * on the given wheel
    * Current test design does not require an array, but it is
    * implemented this way in case the testing sequence changes
    * some time in the future. 
    */
   float m_newDrags[6];



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


	/**
     * Test the left-front and right-rear ESP valves.
     * 
     * @return The status of the operation.
     * @since 30 April 2012
     */ 
    string LFRRESPTest(void);

    /**
     * Test the right-front and left-rear ESP valves.
     * 
     * @return The status of the operation.
     * @since 30 April 2012
     */ 
    string RFLRESPTest(void);

};
//-------------------------------------------------------------------------------------------------
#endif //TrwAbsTc_h
