//******************************************************************************
// Description:
// This class extends the functionality of GenericABSTCTemplate for Ford
// specific ABS functionality
//
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
//    $Log: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/TestComponents/Brake/Include/FordABSTCTemplate.h $
// 
// 1     2/07/11 4:46p Mmcnamar
// backup c drive source for cross compiler
// 
// 2     7/23/10 3:25p Mmcnamar
// Multiple updates for new core, KTP, OHAP limited slip, and ABS
// security.
// 
// 1     5/10/10 2:08p Mmcnamar
// initial check-in for new core.
// 
// 20    3/29/10 4:07p Mmcnamar
// Added ResetCommHw() and EnterDiagnosticMode(). Added m_valveCrossEnd[]
// for updated valve cross analysis.
// 
// 19    3/17/10 1:20p Mmcnamar
// Updated valve cross check.
// 
// 18    1/26/09 10:59a Mmcnamar
// Added SBAParkBrakePawlInit();
// 
// 17    11/14/08 3:41p Mmcnamar
// Updated for hybrid testing: Added m_brakeApplicationFault and
// m_dragTestPassed.
// 
// 16    8/21/08 2:19p Mmcnamar
// Added prototype for EvaluateRolltest().
// 
// 15    6/16/08 1:59p Jwynia
// Updated for CD338HEV: added PedalTravelCheck(); added
// ValidateSBAReductionForces();
// 
// 14    5/05/08 2:59p Mmcnamar
// Updated for hybrids.
// 
// 13    4/09/08 4:12p Mmcnamar
// Updated for Teves Hybrid SBA brake system; Added Ford ABS security
// algorithm.
// 
// 12    3/28/07 2:49p Mmcnamar
// Updated ParkBrakePawlInit() and renamed previous version to
// "OldParkBrakePawlInit()".
// 
// 11    3/05/07 10:28a Mmcnamar
// Added prototype HSSensorTest( ).
// 
// 10    12/27/06 12:57p Mmcnamar
// Updated for Dynamic Park Brake test: Added
// TestStepAccelerateToParkBrakeSpeed(); TestStepDynamicParkBrake();
// AnalyzeDynamicParkBrake(); AnalyzeParkBrakeForces();
// TestStepParkBrakeBalance(). Overloaded TestStepAccelerate() to call
// GetRollSpeed() which returns average wheel speed of driven axle.
// 
// 9     4/01/06 5:01p Bmeinke
// Overloaded the new GeneriCTC park brake/park pawl 'init' methods to
// allow us to start monitoring the brake switch AFTER the "Shift" and
// "Apply park brake" prompts have been displayed.
// 
// 8     2/24/06 1:24p Bmeinke
// Overloaded the new GetAbsReferenceForce() method to use
// "ABSReferenceSamples" brake force samples just prior to reduction for
// calculating reduction/recovery percentages
// 
// 7     9/23/05 3:03p Bmeinke
// Added a new TestStep method (ReportBrakeData) to optionally add the ICM
// brake and speed log file data to the test result file
// 
// 6     9/20/05 10:18a Bmeinke
// Added an overloaded ComboTorqueTest() to copy parameter values into
// TestStep info before calling the base class method
// 
// 5     8/31/05 5:09p Bmeinke
// Added TestStepNeutral() method to implement a neutral check
// 
// 4     8/03/05 2:41p Bmeinke
// Removed the ValidateTorqueTestDriveAxle() method and moved the park
// brake/pawl brake switch check to the new
// AnalyzeParkBrakePawlDriverInfluence() method
// 
// 3     7/08/05 12:57p Bmeinke
// Added new UpdateParameterValue() methods to support the
// HighSpeedSensorTest() method modification
// 
// 2     6/14/05 2:58p Bmeinke
// Added support for new AWDSpeedBalance test
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 3     3/09/05 12:13p Bmeinke
// Added NoStoredCodeLogging
// 
// 2     12/22/04 8:05p Bmeinke
// Moved StopPumpMotor() test step handling to GenericABSTCTemplate
// Added new test steps to monitor the brake switch (for park brake and
// park pawl driver influence checks) called StartBrakeMonitor() and
// StopBrakeMonitor()
// Added a new teststep (IsolateAll()) to isolate all wheels prior to the
// valve firing test
// Fixed the copying of the force meter center value from the parameter
// section to the TestStepInfo section (was using the wrong test step info
// attribute name) 
// 
// 1     9/30/04 2:42p Bmeinke
// Initial full core integration for Ford
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef FordABSTcTemplate_h
#define FordABSTcTemplate_h

#include <string>
#include "XmlNode.h"
#include "GenericABSTCTemplate.h"

/**
 * @version 2.0
 * @since July 29, 2003
 */
template <class ModuleType>
class FordABSTCTemplate : public GenericABSTCTemplate<ModuleType>
{
	friend class FordABSTCTest;
public:
    /** Enum used for waiting for target speed. */
	enum Direction
	{
		UP = 1,
		DOWN = 2
	};

    /** Starting and ending distance readings for the valve cycle test. */
	WHEELINFO   m_valveCycleDistances[ 2];
    /**
     * Constructor
     */
	FordABSTCTemplate();

    /**
     * Destructor
     */
    virtual ~FordABSTCTemplate();

    void InitializeHook(const XmlNode *config);

	/**
	 * This is the method that will handle the execution of each
	 * test step that has been specified by the system for each
	 * test objective that is passed to it.  This must be overloaded
	 * to execute the component tests.
	 * 
	 * @param value This is the value that was commanded by
	 * the test sequencer.
	 * @return The status of the overall objective.
	 * @since Version 1.0
	 */
	virtual const string CommandTestStep(const string &value);    

protected:


    /**
     * Used to test if the brake is applied or not
     * 
     * @return status string
     */
    virtual string PedalTravelCheck(void);

	/**
	 * Setup method. Overriden to read the number of ABS valves from
	 * our config file parameters section
	 * 
	 * @return status string
	 */
	inline string Setup(void);
	
	/**
	 * Execute the reduction/recovery test built into the
	 * ABS module
	 * 
	 * @return A String indicating the test status
	 * @since 10 February 2004
	 */
	virtual std::string ExecuteAbsRollTestSequence(void);
	
	/**
	 * Automatically determines if we need to run a high speed or a low
	 * speed sensor test based on whether or not we are at zerospeed when
	 * we are told to perform the sensor test
	 * 
	 * @return A String indicating the test status
	 */
	virtual std::string TestStepAutoSensor(void);

	/**
	 * Starts monitoring the brake switch status. Used during park brake and
	 * park pawl to determine driver influence faiilures
	 * 
	 * @return  A String indicating the test status
	 */
	virtual std::string StartBrakeSwitchMonitor(void);

	/**
	 * Stops monitoring the brake switch status. Used during park brake and
	 * park pawl to determine driver influence faiilures
	 * 
	 * @return  A String indicating the test status
	 */
	virtual std::string StopBrakeSwitchMonitor(void);

#if 0
	/**
	 * Validates the distances of the "restrained" axle for a park pawl or
	 * park brake test
	 * 
	 * @param leftDistance
	 *                 The distance travelled by the left wheel
	 * @param rightDistance
	 *                 The distance travelled by the right wheel
	 * @param maxDistance
	 *                 The maximum allowed distance for either of the restrained
	 *                 wheels to travel
	 * @param faultTag Fault tag to report if the validation fails
	 * 
	 * @return The status of the test
	 */
	virtual const std::string ValidateTorqueTestDriveAxle( float leftDistance, float rightDistance, float maxDistance, const std::string &faultTag);
#endif

	/**
	 * Final evaluation steps, may need to be
	 * tweaked in child classes, or have child
	 * classes call this and then do their own
	 * steps.
	 * @note Found in Mando BTS T.15
	 */
	virtual std::string EvaluateABS(void);
    /**
	 * Final evaluation steps, may need to be
	 * tweaked in child classes, or have child
	 * classes call this and then do their own
	 * steps.
	 * @note Found in Mando BTS T.15
	 */
	virtual std::string EvaluateABS(std::string axle);

    /**
	 * TRW ABS RollTest evaluation steps.	 
	 */
	virtual std::string EvaluateRolltest(void);

    //MAM 3/13/08
    /**
	 * Evaluate Hybrid brake test.
     * SBA stands for Simulated Brake Actuator
	 */
	virtual std::string EvaluateSBA(void);

	/**
	 * Attempt to identify the ABS reduction indices in the given data array.
	 * 
	 * @param brakeData Brake force data array for a single wheel
	 * @param startIdx  Array index to start the analysis from
	 * @param endIdx    Array index at which to stop the analysis
	 * 
	 * @return The reduction start index if found, -1 if not found
	 */
	INT32 FindReductionPoint( const DATAARRAY &brakeData, INT32 &startIdx, INT32 &endIdx);

    /**
	 * Attempt to identify the Hybrid SBA max force & indices in the given data array.
	 * 
	 * @param brakeData Brake force data array for a single wheel
	 * @param startIdx  Array index to start the analysis from
	 * @param endIdx    Array index at which to stop the analysis
	 * 
	 * @return The max force index if found, -1 if not found
	 */
    INT32 FordABSTCTemplate<ModuleType>::FindMaxForcePoint( INT32 wheelIndex, const DATAARRAY &forceData, 
                                                        INT32 &startIdx, INT32 &endIdx, float *maxForce);

    /**
	 * Attempt to identify the Hybrid SBA min force & indices in the given data array.
	 * 
	 * @param brakeData Brake force data array for a single wheel
	 * @param startIdx  Array index to start the analysis from
	 * @param endIdx    Array index at which to stop the analysis
	 * 
	 * @return The min force index if found, -1 if not found
	 */	
    INT32 FordABSTCTemplate<ModuleType>::FindMinForcePoint( INT32 wheelIndex, const DATAARRAY &forceData, 
                                                        INT32 &startIdx, INT32 &endIdx, float *minForce);


	/**
	 * Return the reference brake force to use for calculating ABS recovery and reduction percentages.
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * Returns the reference brake force to use for calculating ABS recovery and reduction percentages.
	 * <br><p>
	 * 
	 * <b>Data Tags:</b>
	 *      <ul>
	 *      <li> None
	 *      </ul>
	 * 
	 * <b>Test Component Parameters:</b>
	 *      <ul>
	 *      <li> None
	 *      </ul>
	 * 
	 * <b>Fault Codes:</b>
	 *      <ul>
	 *      <li> None
	 *      </ul>
	 * 
	 * <b>Functions Called:</b>
	 *      <ul>
	 * 		<li> GetWheelForceArray() </li>
	 *      </ul>
	 * 
	 * @param roller The roller to return the reference brake force for
	 * 
	 * @return The reference brake force to use for calculating ABS recovery and reduction percentages.
	 */
	virtual float GetAbsReferenceForce( INT32 roller);
	
	/**
	 * Analyze the brake force test data to determine if any
	 * of the valves are crossed.
	 *
	 * @return The status of the operation.
	 */
	virtual BEP_STATUS_TYPE AnalyzeValveCross(void);

	/**
	 * Analyze the brake force test data to determine if the LF valve fired
	 * when we told it to
	 * 
	 * @return true if the LF valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeLFValveCross();

    /**
	 * Analyze the brake force test data to determine if the LF valve fired
	 * when we told it to
	 * 
	 * @return true if the LF valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeLFValveCrossNew(INT32 *minIndex);
    virtual bool AnalyzeLRValveCrossRearFirst(INT32 *minIndex); //MAM 3/8/11
    virtual bool AnalyzeLFValveCrossFrontFirst(INT32 *minIndex); //MAM 2/18/12

        /**
	 * Analyze the brake force test data to determine if the LF valve fired
	 * when we told it to
	 * 
	 * @return true if the LF valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeLFValveCrossLimSlip(INT32 *minIndex);
	
	/**
	 * Analyze the brake force test data to determine if the RF valve fired
	 * when we told it to
	 * 
	 * @return true if the valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeRFValveCross();

    /**
	 * Analyze the brake force test data to determine if the LF valve fired
	 * when we told it to
	 * 
	 * @return true if the LF valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeRFValveCrossNew(INT32 *minIndex);
    virtual bool AnalyzeRRValveCrossRearFirst(INT32 *minIndex);  //MAM 3/8/11
    virtual bool AnalyzeRFValveCrossFrontFirst(INT32 *minIndex);  //MAM 2/18/12

    /**
	 * Analyze the brake force test data to determine if the RF valve fired
	 * when we told it to
	 * 
	 * @return true if the RF valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeRFValveCrossLimSlip(INT32 *minIndex);

	/**
	 * Analyze the brake force test data to determine if the LR valve fired
	 * when we told it to
	 * 
	 * @return true if the valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeLRValveCross();

    /**
	 * Analyze the brake force test data to determine if the LF valve fired
	 * when we told it to
	 * 
	 * @return true if the LF valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeLRValveCrossNew(INT32 *minIndex);    
    virtual bool AnalyzeLFValveCrossRearFirst(INT32 *minIndex);  //MAM 3/8/11
    virtual bool AnalyzeLRValveCrossFrontFirst(INT32 *minIndex);  //MAM 2/18/12

    /**
	 * Analyze the brake force test data to determine if the LF valve fired
	 * when we told it to
	 * 
	 * @return true if the LF valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeLFValveCrossSBA();
	
	/**
	 * Analyze the brake force test data to determine if the RF valve fired
	 * when we told it to
	 * 
	 * @return true if the valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeRFValveCrossSBA();
	
	/**
	 * Analyze the brake force test data to determine if the LR valve fired
	 * when we told it to
	 * 
	 * @return true if the valve fired when we told it to, false otherwise
	 */
	virtual bool AnalyzeLRValveCrossSBA();

    /**
	 * Validate SBA brake forces against min and max params
	 * 
	 * @return The test result
	 * @since 22 April 2008
	 */
    virtual INT32 ValidateSBABrakeForces(float *forceVal);

    /**
	 * Validate SBA reduction forces
	 * 
	 * @return The test result
	 * @since 11 June 2008
	 */
    virtual INT32 ValidateSBAReductionForces(float *forceMin, float *forceMax);

	/**
	 * Isolates all ABS valves prior to an ABS valve firing sequence
	 * 
	 * @return The test result
	 * @since 10 July 2003
	 */
	virtual string IsolateAll();

	/**
	 * Tell the ABS module to stop storing fault codes. Added for
	 * Bosch ABS at Chicago due to wheelspeed plausability faults
	 * being set during the trans test.
	 * 
	 * @return The test result
	 */
	virtual string EnableCodeLogging();
	
	/**
	 * Tell the ABS module to start storing fault codes. Added for
	 * Bosch ABS at Chicago due to wheelspeed plausability faults
	 * being set during the trans test.
	 * 
	 * @return The test result
	 */
	virtual string DisableCodeLogging();

    //MAM 1/8/15
    virtual string DisableTractionControl();
    virtual string EnableTractionControl();

	/**
	 * Checks for front to rear axle speed mismatches on AWD vehicles
	 * 
	 * @return The test result
	 */
	virtual string AWDSpeedBalance();

	/**
	 * Detects a shift to neutral by comparing the current decel rate against
	 * a parameter
	 * 
	 * @return The test result
	 */
	virtual string TestStepNeutral();

	/**
	 * Test step usedto add brake force and speed arrays to the test result file
	 * 
	 * @return testPass
	 */
	virtual string TestStepReportBrakeData();

    //MAM 3/13/08
    /**
	 * Test step used to request security access from the ABS module.
	 * 
	 * @return
	 */
	virtual string RequestAbsSecurity();

    //MAM 8/1/14
    /**
	 * Test step used to request security access from the Bendix air brake module.
	 * 
	 * @return
	 */
	virtual string RequestBendixSecurity1();
    virtual string RequestBendixSecurity3();
    virtual string RequestBendixSecurity7(); //MAM 2/9/15 - for EC80 module

    /**
	 * Test steps used to calculate the security access key.
	 */
    void CalculateSecurityKey(short level, SerialString_t &seed );
    unsigned char ReadBitLong( unsigned long ulDataValue,unsigned char bBitPosition);
    unsigned long AdjustBitLong(unsigned long ulDataValue,unsigned char bBitPosition,unsigned char bBitValue);
    //MAM 8/1/14
    void CalculateBendixKey(short level, SerialString_t &seed );

    /**
	 * Reset the SBA Test Status DID
	 * 
	 * @return The test result
	 */
	virtual string ClearStatusDID();

    //MAM 12/23/14
    virtual string CheckPartNumber();

	/**
	 * Reads an ICM data log file from the given file name into the 'fileData'
	 * argument
	 * 
	 * @param fileName name of the ICM data log file to read
	 * @param fileData Buffer to read the data log file into
	 * 
	 * @return number of bytes read
	 */
	int ReadIcmDataLogFile( const std::string &fileName, std::string &fileData);

	/**
	 * Perform pre-hydraulic park brake test initialization
	 * 
	 * @return The status of the test step
	 */
	virtual INT32 HydraulicParkBrakeInit();

	/**
	 * Perform pre-electirc park brake test initialization
	 * 
	 * @return The status of the test step
	 */
	virtual INT32 ElectricParkBrakeInit();

	/**
	 * Perform pre-hydraulic park pawl test initialization
	 * 
	 * @return The status of the test step
	 */
	virtual INT32 HydraulicParkPawlInit();

	/**
	 * Perform pre-electric park pawl test initialization
	 * 
	 * @return The status of the test step
	 */
	virtual INT32 ElectricParkPawlInit();

	/**
	 * Perform pre-hydraulic park brake/pawl test initialization
	 * 
	 * @return The status of the test step
	 */
	virtual INT32 HydraulicParkBrakePawlInit();

	/**
	 * Perform pre-electric park brake/pawl test initialization
	 * 
	 * @return The status of the test step
	 */
	virtual INT32 ElectricParkBrakePawlInit();

	/**
	 * Method used to start monitoring the brake switch during park brake and/or
	 * park pawl tests
	 * 
	 * @return The status of the test step
	 */
	//virtual INT32 ParkBrakePawlInit();
    virtual INT32 ParkBrakePawlPedalCheck();

    //MAM 06/03/10
    /**
	 * This method is used by the Park Pawl Test to do the actual
	 * torque testing. It sends a subtest result.
	 *
	 * @param direction Direction to turn the rollers: "Forward", "Reverse" or "Both"
	 *
	 * @return The status of the test step
	 */
	virtual const std::string ParkPawlTorqueTest(const std::string &direction);

    /**
     * Test the park brake
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Torque will be applied to the rear axle and the front axle will be commanded to speed.  After a set amount of time,
     * torque and speed will be removed from the applicable axles.  The wheel distances will then be analyzed to ensure
     * the park brake is set correctly and the operator was not applying the brake pedal.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> ParkBrakeApplyForce - Amount of force to aply to the rear rollers. </li>
     *     <li> ParkBrakeSpeedValue - Speed to command the front rollers to. </li>
     *     </ul>
     *
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> TestTime - Amount of time(ms) to apply torque to the drive axle rollers. </li>
     *     <li> Description - Description of the test step. </li>
     *     </ul>
     * 
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> Zerospeed - System tag to use to check if the vehicle is at zero speed. </li>
     *     </ul>
     * 
     * <b>Fault Tags:</b>
     *     <ul>
     *     <li> ParkBrakeFault - Park brake is not set properly. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetWheelDistances() </li>
     *      <li> SetStartTime() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> SystemCommand() </li>
     *      <li> TimeRemaining() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> ValidateParkBrakeTestDistances() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param direction The direction to turn the rollers.  Valid values are "Forward", "Reverse" or "Both".
     *
     * @return The result of testing the park brake.
     */
    virtual const std::string PerformPBTorqueTest(const std::string &direction);

    virtual const std::string ValveCycleTorqueTest(const std::string &direction);    

    //MAM 12/10/08
    /**
	 * Method used to start monitoring the brake switch during park brake and/or
	 * park pawl tests on hybrids
	 * 
	 * @return The status of the test step
	 */
	virtual INT32 SBAParkBrakePawlInit();

    virtual INT32 OldParkBrakePawlInit(); //MAM 3/21/07 - development
	
	/**
	 * Checks to see if the driver applied the brakes during a park brake/pawl test
	 * 
	 * @return The test result
	 */
	virtual string AnalyzeParkBrakePawlDriverInfluence();

    /**
     * Analyze the park pawl test data.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Analyzes the park pawl test distance results.  The start and end distances will be checked to ensure the park pawl 
     * engages correctly.  The distance on the drive axle should be small since the park pawl is preventing the wheels
     * from rotating.  
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineType - System tag used for getting the current machine type. </li>
     *      <li> MachineType3600 - Value indicating this is a 3600 style machine. </li>
     *      <li> ParkingPawlTestPass - System tag for operator input indicating if the park pawl test passed. </li>
     *      <li> ParkingPawlTestFail - System tag for operator input indicating if the park pawl test failed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> ValidateParkPawlTestDistances() </li>
     *      </ul>
     *
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of validating the wheel distances.
     */
    //virtual const std::string AnalyzeParkPawlTest(const std::string &value);
    virtual const std::string AnalyzeParkPawlTest();

	/**
	 * This function waits until the vehicle achieves given speed
	 * 
	 * @param targetSpeed
	 *                  Speed to be achieved
	 * @param direction Direction of speed (increasing or decreasing?)
	 * @param wait      delay in ms between loop of waiting. Default is 100ms
	 * 
	 * @return A pass/fail flag
	 * @since Version 1.6
	 */
    BEP_STATUS_TYPE WaitForTargetSpeed(float targetSpeed, Direction direction, UINT32 wait = 100);
    
	/**
	 * This function waits until the vehicle achieves given speed
	 * 
	 * @param targetSpeed
	 *                  Speed to be achieved
	 * @param direction Direction of speed (increasing or decreasing?)
	 * @param waitAll   Flag indicating if we want to wait for all wheels (true) or any
	 *                  wheel (false) to break the target speed
	 * @param wait      delay in ms between loop of waiting. Default is 100ms
	 * 
	 * @return A pass/fail flag
	 * @since Version 1.6
	 */
    BEP_STATUS_TYPE WaitForTargetSpeed(float targetSpeed, Direction direction, bool waitAll, UINT32 wait = 100);
    
	/**
	 * This overloaded StatusCheck checks for DSLStop
	 * @return A BEP_STATUS_TYPE indicating the status
	 */
	virtual INT32 StatusCheck(void);

	/**
	 * This method verifies that the wheels speed sensors work correctly at a
	 * low speed, within a tolerance. We override this method to put our
	 * parameter values into the TestStepInfo where the base class expects them.
	 * 
	 * @return The test result
	 */
	virtual string LowSpeedSensorTest(void);

    virtual string LSSensorTest(void); //MAM 5/25/11

	/**
	 * This method verifies that the wheels speed sensors work correctly at a
	 * high speed, within a tolerance. We override this method to put our
	 * parameter values into the TestStepInfo where the base class expects them.
	 * 
	 * @return The test result
	 */
	virtual string HighSpeedSensorTest(void);

    /**
	 * This method verifies that the wheels speed sensors work correctly at a
	 * high speed, within a tolerance. We override this method to put our
	 * parameter values into the TestStepInfo where the base class expects them.
	 * 
	 * @return The test result
	 */
	virtual string HSSensorTest(void);

    /**
	 * This method verifies that the wheels speed sensors work correctly at a
	 * high speed, within a tolerance. We override this method to put our
	 * parameter values into the TestStepInfo where the base class expects them.
	 * 
	 * @return The test result
	 */
	virtual string SBAHighSpeedSensorTest(void);

	/**
	 * Accelerate to  brake test speed. This is "overridden" to put our 
	 * parameters into the test step info where the base class expects them
	 * 
	 * @return Status of the test
	 */
	std::string TestStepAccelerateToBrakeSpeed(void);

    /**
	 * This test step is responsible for prompting the operator to
	 * accelerate to the dynamic park brake arm speed.
	 * 
	 * @return The status of the acceleration test step.
	 */
	virtual const string TestStepAccelerateToParkBrakeSpeed(void);

    /**
	 * This test step is responsible for prompting the operator to
	 * accelerate to the specified speed.
	 *
	 *
	 * @return Test result for acclerating to the test speed.
	 * 		<ul>
	 * 		<li> Pass - The required speed has been achieved. </li>
	 * 		<li> Timeout - The required speed has not been achieved in the time allowed. </li>
	 * 		<li> SoftwareFail - An exception occurred while monitoring the acceleration. </li>
	 * 		</ul>
	 */
	virtual const string TestStepAccelerate(void);

    /**
	 * This test step is responsible for prompting the driver 
	 * to shift to neutral and applying the park brake to measure
	 * the forces generated by the park brake
	 * 
	 * @return The status of the test step.
	 */
	virtual const string TestStepDynamicParkBrake(void);

    //MAM 3/14/12 - Added TestStepQuickStop
    /**
     * Use the motors to bring the rollers to zero speed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * This test step will prompt the driver to brake to stop and then command the motors to zerospeed if the 
     * machine is a 3600.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target sped range for the operator. </li>
     *      <li> Zerospeed - System tag to use for determining if the rollers are at zero speed. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> SpeedTarget - Speed target to diplay to the operator to achieve zero speed. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SystemWrite() </li>
     *      <li> TimeRemaining() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> UpdateResult() </li>
     *      <li> RemovePrompts() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     * 
     * @return The result of performing the quick stop.
     */
    virtual const string TestStepQuickStop (const std::string &value);

    /**
	 * This sets up the GUI to prompt the driver to maintain
	 * brake force in the target value.
	 * 
	 * @param brakeStart The starting sample of the park brake check.
	 * @param brakeEnd   The ending sample of the park brake check.
	 * @param tagPrefix  String added to the beginning of strings used to tag the
	 *                   start and end of the force sampling interval
	 * 
	 * @return The status of the test.
	 * @since Version 1.0
	 */
	virtual INT32 DynamicParkBrake(INT32 &brakeStart, INT32 &brakeEnd, const std::string &tagPrefix);
	
	/**
	 * Analyze the forces generated during the dynamic park brake test
	 * 
	 * @return The status of the test step.
	 */
	virtual const string AnalyzeDynamicParkBrake(void);

	/**
	 * Read the brake arrays and calculate the park brake forces for each
	 * of the vehicles wheels and analyze the data.
	 *
	 * @param brakeStart The starting sample index of the brake force array.
	 * @param brakeEnd   The ending sample index of the brake force array.
	 * @return The status of the operation.
	 * @since Version 1.0
	 */
	virtual INT32 AnalyzeParkBrakeForces(INT32 brakeStart, INT32 brakeEnd);

	/**
	 * Validate the park brake force of each wheel and update the display
	 * with the result.
	 *
	 * @param roller  The roller to analyze the park brake force result for.
	 * @param average The average park brake force for the specified wheel.
	 * @return The status of the operation.
	 * @since Version 1.0
	 */
    virtual INT32 ValidateParkBrakeForce(INT32 roller, float average);

	/**
	 * Perform a drag test. This is "overridden" to put our parameters 
	 * into the test step info where the base class expects them
	 * 
	 * @return Status of the test
	 */
	std::string TestStepDrag(void);
	
	/**
	 * Perform a base brake test. This is "overridden" to put our parameters 
	 * into the test step info where the base class expects them
	 * 
	 * @return Status of the test
	 */
	std::string TestStepBaseBrake(void);
	
	/**
	 * Perform the valve firing sequence for all wheels.
	 *
	 * @return Result of the valve firing test.
	 */
	virtual std::string ValveFiringTest();
    /**
     * Perform the valve firing sequence for all wheels.
     *
     * @return Result of the valve firing test.
     */
    virtual std::string ValveFiringTest(std::string axle);


    /**
	 * Enter diagnostic mode.
	 *
	 * @return status string
	 * @note see GenericTC for status tags
	 */
	virtual string EnterDiagnosticMode(void);

    //MAM 12/3/14 - Bendix programming
    //MAM 2/10/15 - Updated for EC80
    virtual string WriteFingerprint(void);
    virtual string ProgramTireSize(void);
    virtual string ProgramTireSizeEC80(void);
    virtual string DisableLampGndChk(void);
    virtual string DisableLampGndChkEC80(void);
    virtual string EnableHrwCcvs(void);
    virtual string EnableHrwCcvsEC80(void);
	
	/**
	 * Analyze the drag test results. This is "overridden" to put our parameters 
	 * into the test step info where the base class expects them
	 * 
	 * @return Status of the test
	 */
	std::string TestStepAnalyzeDrag(void);
    /**
	 * Analyze the drag test results. This is "overridden" to put our parameters 
	 * into the test step info where the base class expects them
	 * 
	 * @return Status of the test
	 */
	std::string TestStepAnalyzeDrag(std::string axle);
	
	/**
	 * Analyze the base brake test results. This is "overridden" to put our
	 * parameters into the test step info where the base class expects them
	 * 
	 * @return Status of the test
	 */
	std::string TestStepAnalyzeBaseBrake(void);
    /**
     * Analyze the base brake test results. This is "overridden" to put our
     * parameters into the test step info where the base class expects them
     * 
     * @return Status of the test
     */
    std::string TestStepAnalyzeBaseBrake(std::string axle);

	
	/**
	 * Analyze the brake force balance. This is "overridden" to put our
	 * parameters into the test step info where the base class expects them
	 * 
	 * @return Status of the test
	 */
	std::string TestStepBalance(void);
    /**
	 * Analyze the brake force balance. This is "overridden" to put our
	 * parameters into the test step info where the base class expects them
	 * 
	 * @return Status of the test
	 */
	std::string TestStepBalance(std::string axle);

    /**
	 * Analyze the Dynamic Park Brake force balance.
	 * 
	 * @return Status of the test
	 */
	std::string TestStepParkBrakeBalance(void);

	/**
	 * Sends a generic message to the ABS module. The tag of message to send
	 * is dictated by the MessageTag TestStepInfo item.
	 */
	std::string TestStepSendModuleMessage();

	/**
	 *
	 * @param direction Direction to turn the rollers: "Forward", "Reverse" or "Both"
	 *
	 * @return The status of the test step
	 */
	virtual const std::string ComboTorqueTest(const std::string &direction);

    //MAM 3/24/10
    //MAM 3/9/10
    virtual const std::string ResetCommHw(void);
    virtual const std::string DisableCommHw(void);

	/**
	 * Makes sure that the prompt1 value sent to eCATS for the brake force meter
	 * center value matches our min/max brake force values
	 */
	void SetPrompt1ForceValue();

	/**
	 * Changes the value of the given parameter
	 * 
	 * @param tag    The tag for the parameter to be changed
	 * @param value  The new parameter value
	 */
	void UpdateParameterValue(const std::string& tag, const float value);

	/**
	 * Changes the value of the given parameter
	 * 
	 * @param tag    The tag for the parameter to be changed
	 * @param value  The new parameter value
	 */
	void UpdateParameterValue(const std::string& tag, const int value);

	/**
	 * Changes the value of the given parameter
	 * 
	 * @param tag    The tag for the parameter to be changed
	 * @param value  The new parameter value
	 */
	void UpdateParameterValue(const std::string& tag, const std::string& value);
	
	/**
	 * Copies a parameter value into the TestStepInfo where the base 
	 * class routines expect the data
	 * 
	 * @param tag    The TestStepInfo tag to stick the value into
	 * @param value  The value to stick into the TestStepInfo
	 */
	void CopyParamToTestStepInfo(const std::string& tag, const float value);
	
	/**
	 * Copies a parameter value into the TestStepInfo where the base 
	 * class routines expect the data
	 * 
	 * @param tag    The TestStepInfo tag to stick the value into
	 * @param value  The value to stick into the TestStepInfo
	 */
	void CopyParamToTestStepInfo(const std::string& tag, const int value);
	
	/**
	 * Copies a parameter value into the TestStepInfo where the base 
	 * class routines expect the data
	 * 
	 * @param tag    The TestStepInfo tag to stick the value into
	 * @param value  The value to stick into the TestStepInfo
	 */
	void CopyParamToTestStepInfo(const std::string& tag, const std::string& value);

	/**
	 * "File" to read current accel values from
	 */
	std::string		m_icmAccelFile;

    /** MAM 4/24/08
	 * Pass/fail flags for sensor tests
	 */
    bool            m_sensorCrossPass[4];

    bool            m_hsSensorPass;

    //MAM 10/30/08
    /**
	 * Flag to indicate if there was a driver braking fault
	 */
	bool			m_brakeApplicationFault;

    //MAM 10/30/08
    /**
	 * Flag to indicate if the drag test passed
	 */
	bool			m_dragTestPassed;

    //MAM 3/26/10
    /**
     * End indices for valve cross check based on wheel speed
     */
    INT32   m_valveCrossEnd[4];

    //MAM 6/8/10
    //MAM 10/30/08
    /**
	 * Flag to indicate if there was a driver braking fault during the park pawl test
	 */
	bool			m_ppwlBrakeAppFault;

    //MAM 8/31/11
    /**
     * Flag to indicate if we had a communication fault during reduction/recovery
     */
    bool            m_absCommFault;

    /** Firing order for the 4-channel ABS test. */
    XmlNodeMap m_abs4ChannelFrontFiringOrder;
    /** Firing order for the 4-channel ABS test. */
    XmlNodeMap m_abs4ChannelRearFiringOrder;

    
private:
	
	/**
	 * Array index where the dynamic park brake test started
	 */
	int				m_dynParkBrakeStart;

	/**
	 * Array index where the dynamic park brake test ended
	 */
	int				m_dynParkBrakeStop;    

	/**
	 * Park brake force for each wheel during the dynamic park brake test.
	 * @since Version 1.0
	 */
	FORCELIST 		m_parkBrakeForce;    

};

#endif // FORDABSTESTCOMPONENT_H

