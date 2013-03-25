//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Include/KoreaAbsTcTemplate.h 1     3/09/06 3:15p Gswope $
//
// FILE DESCRIPTION:
//    This class extends the functionality of the TestComponent.
//    @author Ross Wiersema
//    @class KoreaABSTCTemplate
//
//    @since 15 December 2003
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Include/KoreaAbsTcTemplate.h $
// 
// 1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// 
// 3     3/16/05 10:49a Gswope
// Added RecalculateDragBeforeEsp option
// Removed call to AnalyzeESPValveCross at customer (Oscar) request
// Re-installed publication of ESP forces to front panel
// 
// 2     3/02/05 11:18a Gswope
// Emergency HMMA request to remove rear valves from ESP test
// 2005.2.28 ews changed per emergency HMMA request
// made ESP valve firing and evaluation use only the front two wheels.
// EvaluateESP()  and  AnalyzeESPValveCross()
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 5     11/24/04 11:04a Bmeinke
// Fixed a bug in the EvaluateESP() method where a failed ESP test would
// be reported as a pass
// 
// 3     11/15/04 9:23a Bmeinke
// Changed AnalyzeESPBuildForces() and AnalyzeESPReductionForces() to
// check the IncludeDragInBrakeForce parameter so we optionally remove
// drag force value from the ESP calculations
// 
// 2     10/27/04 8:57p Bmeinke
// AnalyzeESPReductionForces() and AnalyzeESPRecoveryForces() methods need
// to use the IncludeDragInBrake parameter when calculating the ESP forces
// 
// 1     10/27/04 5:40p Bmeinke
// 
// 2     10/26/04 1:58p Bmeinke
// In the SensorTest methods, get a float parameter for SensortestSpeed to
// avoid repetative atoi() calls and initialize the rollSpeed[xx] elements
// before testing to see if it is greater than the sensor test target
// speed
// 
// 1     8/30/04 7:24p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
//*************************************************************************
#ifndef KoreaAbsTcTemplate_h
#define KoreaAbsTcTemplate_h

#include "GenericABSTCTemplate.cpp"
#include "GenericABSModuleTemplate.cpp"

// Create a structure to contain build/reduction indices for each wheel
typedef struct _BuildReductionIndex
{
   INT32 buildStart;
   INT32 buildEnd;
   INT32 reductionStart;
   INT32 reductionEnd;
} BuildReductionIndex;

/**
 * Korea ABS test component.
 * This test component handles all test steps that are common
 * to most Korea ABS tests.  
 * 
 * @author Ross Wiersema
 * @see GenericABSTCTemplate
 * @see GenericTCTemplate
 * @see GenericTC
 * @since 15 December 2003
 */
template <class VehicleModuleType>
class KoreaAbsTcTemplate : public GenericABSTCTemplate<VehicleModuleType>
{
   friend class KoreaABSTCTest;

public:

   /**
    * Constructor
    * Nothing special to do here.
    */
   KoreaAbsTcTemplate();

   /**
    * Destructor
    * Nothing special to do here.
    */
   virtual ~KoreaAbsTcTemplate();

   /**
    * This is the method that will handle the execution of each
    * test step that has been specified by the system for each
    * test objective that is passed to it.  This must be overloaded
    * to execute the component tests.
    * 
    * @param value This is the value that was commanded by
    * the test sequencer.
    * @return The status of the overall objective.
    * @since 15 December 2003
    */
   virtual const string CommandTestStep(const string &value);

protected:

   /**
    * Check part number for validity
    *
    * @return status string
    * @note see GenericTC for status tags
    */
   virtual string CheckPartNumber(void);

   /**
    * This test step will send a command to the motor to bring the left
    * front roll to the sensor test speed.  Once this speed is reached, it
    * will check for a front sensor cross side to side.
    * 
    * @return The status of the test step.
    * @since 16 December 2003
    */
   virtual string LFSensorTest(void);

   /**
    * This test step will send a command to the motor to bring the right
    * front roll to the sensor test speed.  Once this speed is reached, it
    * will check for a front to rear sensor cross.
    * 
    * @return The status of the test step.
    * @since 16 December 2003
    */
   virtual string RFSensorTest(void);

   /**
    * This test step will send a command to the motor to bring the left
    * rear roll to the sensor test speed.  Once this speed is reached, it
    * will check for a rear sensor cross side to side.
    * 
    * @return The status of the test step.
    * @since 16 December 2003
    */
   virtual string LRSensorTest(void);

   /**
    * This test step will send a command to the motor to bring the right
    * rear roll to the sensor test speed.
    * 
    * @return The status of the test step.
    * @since 16 December 2003
    */
   virtual string RRSensorTest(void);

   /**
    * This test step runs the sensor quality test.
    * 
    * @return The status of the operation.
    * @since 12 Feb 2004
    */ 
   virtual string SensorQualityTest(void);

   /**
    * This function performs a hydraulic sensor test. It passes a flag to the 
    * PLC to start the ABS SENSOR MOTORS.  The PLC then times its way through 
    * a sequence that first starts the fronts, then the rears, then declutches 
    * the left side.  The sensors are thus checked for front-to-rear and 
    * side-to-side cross.
    * 
    * @return The status of the operation
    * @since 13 Feb 2004
    */ 
   virtual string HydraulicSensorTest(void);

   /**
    * This test step is responsible for prompting the driver to coast
    * until the end coast speed is reached.
    * 
    * @return The status of the test step.
    * @since Version 1.0
    */
   virtual const string TestStepCoast(void);

   /**
    * This commands the driver to coast.
    * 
    * @return The status of the test.
    * @since Version 1.0
    */
   virtual INT32 Coast(void);

   /**
    * Evaluate the ABS data and determine if the 
    * reductions and recoveries are sufficient.
    * 
    * @return Result of the test.
    * @since Version 1.0
    */
   virtual string EvaluateABS(void);

   /**
    * The test step will evaluate the TCS build and reduction forces and also
    * check for a TCS valve cross.
    * 
    * @return The status of the test step.
    * @since 12 Feb 2004
    */ 
   virtual string EvaluateTCS(void);

   /**
    * The test step will evaluate the ESP build and reduction forces and also
    * check for an ESP valve cross.
    * 
    * @return The status of the test step.
    * @since 16 December 2003
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
    * Index to the start of the TCS valve firing test.
    */
   INT32 m_TCSStartIndex;

   /**
    * Index to the end of the TCS valve firing test.
    */
   INT32 m_TCSEndIndex;

   /**
    * TCS Build and Reduction start and end indices for each wheel.
    * @since 12 Feb 2004
    */
   BuildReductionIndex *m_TCSIndex;

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

   bool m_espValveCrossPassed;

   float m_espBuildValue[4];

   string& OriginalDriveAxle(const string *driveAxle = NULL);

   string m_originalDriveAxle;

private:

   /**
    * Analyze the TCS build force for the specified wheel and report the 
    * results.
    * 
    * @param roller The roller to analyze.
    * @param start  The start index for the build pulse.
    * @param end    The end index for the build pulse.
    * @return The result of the check.
    *         BEP_STATUS_SUCCESS or BEP_STATUS_FAILURE
    */
   virtual BEP_STATUS_TYPE AnalyzeTCSBuildForces(INT32 roller, INT32 start, INT32 end);

   /**
    * Analyze the TCS reduction force for the specified wheel and report the 
    * results.
    * 
    * @param roller The roller to analyze.
    * @param start  The start index for the build pulse.
    * @param end    The end index for the build pulse.
    * @return The result of the check.
    *         BEP_STATUS_SUCCESS or BEP_STATUS_FAILURE
    */
   virtual BEP_STATUS_TYPE AnalyzeTCSReductionForces(INT32 roller, INT32 start, INT32 end);

   /**
    * Analyze the brake force test data to determine if any of the TCS valves 
    * are crossed.
    * 
    * @return The status of the operation.
    * @since 14 Feb 2004
    */
   virtual BEP_STATUS_TYPE AnalyzeTCSValveCross(void);

   /**
    * Analyze the brake force test data to determine if any of the ESP valves 
    * are crossed.
    * 
    * @return The status of the operation.
    * @since 16 December 2003
    */
   virtual BEP_STATUS_TYPE AnalyzeESPValveCross(void);

   /**
    * Analyze the brake force test data to determine if any of the ABS valves 
    * are crossed.
    * 
    * @return The status of the operation.
    * @since 16 December 2003
    */
   virtual BEP_STATUS_TYPE AnalyzeValveCross(void);

};

#endif
