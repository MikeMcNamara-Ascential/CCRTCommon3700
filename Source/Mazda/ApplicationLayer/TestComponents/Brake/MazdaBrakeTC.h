//*************************************************************************
// Description:
// This file contains the functions necessary to test the vehicle.
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
#ifndef MazdaBrakeTC_h
#define MazdaBrakeTC_h
//-------------------------------------------------------------------------------------------------
#include "DataAnalysis.h"
#include "GenericBaseBrakeTC.h"
#include <list>
//-------------------------------------------------------------------------------------------------
class MazdaBrakeTC : public GenericBaseBrakeTC
{
public:
	MazdaBrakeTC();
	virtual ~MazdaBrakeTC();

    /**
     * Abort test processing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Abort the current test processing.  If testing is in process for this component, the overall result will be set
     * to Abort as well as the component status.  If the overall result is set to Pass, it will not be updated to Abort.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetStatus() </li>
     *      <li> GetPreviousOverallResult() </li>
     *      <li> SendOverallResult() </li>
     *      <li> SetStatus() </li>
     *      </ul>
     */
    virtual void Abort(void);

	/**
	 * Sequence a test step.
	 * <br><p>
	 * <b>Category:</b> Utility
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
	 * Initialize the base brake component.
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The test component will be initialized and inform the system when it has completed initializing.
	 * <br><p>
	 *
	 * @param config The test component configuration file to be used for initialization.
	 */
	virtual void Initialize(const XmlNode *config);





protected:

	/**
	 * Brake testing complete test step.
	 * Turn off the motors and prompt the operator to remove foot from brake pedal.
	 * 
	 * @return Result of finishing up the brake test sequence.
	 */
	string BrakeTestingComplete(void);

	/**
	 * Command the test head to run the diagnostic clear routine.
	 * 
	 * @return Result of the diagnostic clear routine.
	 */
	string ClearDiagnostics(void);

	/**
	 * Run the fault check routine from the test head.
	 * 
	 * @return Result of running the fault check routine.
	 */
	string FaultCheck(void);

	/**
	 * Run the ABS test for the specified wheel.
	 * 
	 * @param wheelTestStep
	 *               Test head test step to run.
	 * @param testingWheel
	 *               Index of the wheel being tested.
	 * @param oppositeWheel
	 *               Index of the opposite wheel on the same axle as the tested wheel.
	 * 
	 * @return Result of the ABS test for the specified wheel.
	 */
	string MazdaAbsValveTest(string wheelTestStep, INT16 testingWheel, INT16 oppositeWheel);

	/**
	 * Check the brake forces on each axle
	 * 
	 * @param axle the axle that is being checked (either "Front" or "Rear")
	 * 
	 * @return Result of the force test
	 */
	string MazdaBrakeForceTest(string axle);

	/**
	 * Perform the brake switch test.
	 * If the action sent is to start the test, the test head will be commanded to start the test.
	 * If the action sent is to stop the test, the software will monitor for the test complete and get the
	 * test result.
	 * 
	 * @param action Action to be perofmred:
	 *               Start - command the test head to start the brake switch test.
	 *               Stop - monitor for the brake switch test results.
	 * 
	 * @return Result of performing the requested action with the brake test results.
	 */
	string MazdaBrakeSwitchTest(const string &action);

	/**
	 * Check the drag on the brakes.
	 * 
	 * @return Result of the drag test.
	 */
	string MazdaDragTest(void);

	/**
	 * Command the test head to perform a test step.
	 * The test will be started and then the software will wait for the indication the test is complete.
	 * 
	 * @param testStep Test Step to be performed.
	 * @param waitForTestComplete
	 *                 Flag indicating if we should wait for the test complete flag.
	 *                 True = Wait for test complete
	 *                 False = Just start the test
	 * 
	 * @return Test head test result for the specified test step.
	 */
	string PerformTestHeadTest(const string &testStep, bool waitForTestComplete=true);

	/**
	 * Check the wheelspeed sensors
	 * 
	 * @return Result of the wheel speed sensors
	 */
	string SpeedSensorCheck(void);



private:

	/** Structure for holding brake force data during max brake force testing and park brake testing */
	typedef struct _maxBrakeData
	{
		float currentForce;
		float maxForce;
		list<float> forceSamples;
		string currentForceTag;
		string displayTag;
		bool measurementComplete;
	} MaxBrakeData;

	// Variable result data types
	typedef enum
	{
		DRAG_RESULTS_FRONT = 1,
		DRAG_RESULTS_REAR,
		BRAKE_RESULTS_FRONT,
		BRAKE_RESULTS_REAR,
		PARK_BRAKE_RESULTS
	} ResultType_t;

	/**
	 * Analyze the forces from the test that was performed.
	 * The result data will also be written to the PLC for reporting to the Mazda system.
	 * 
	 * @param forceData  Force data to be analyzed.
	 * @param leftWheel  Index to the left wheel to be analyzed.
	 * @param rightWheel Index to the right wheel to be analyzed.
	 * @param resultType Type of force data being analyzed.
	 * @param sumLimit   Limit of the sum of the left and right avereages.
	 * @param diffLimit  Limit of the difference between the left and right averages.
	 * @param comparison Type of comparison to perform on against the sum limit.
	 * @param details    Result details to be reported to the test result server.
	 * @param axle       Which axle is being analyzed - Front or Rear.
	 * @param leftDisplayTag
	 *                   Display tag to use for displaying the result for the left wheel.
	 * @param rightDisplayTag
	 *                   Display tag to use for displaying the result for the right wheel.
	 * @param sumDisplayTag
	 *                   Tag to use to display the axle sum value.
	 * @param diffDisplayTag
	 *                   Tag to use for displaying the axle diff value.
	 * @param sumBgColorTag
	 *                   Tag to use for setting the result color for the sum.
	 * @param diffBgColorTag
	 *                   Tag to use for setting the background result color for the diff.
	 * 
	 * @return Result of the data analysis.
	 */
	string AnalyzeForceResults(MaxBrakeData *forceData, const INT16 &leftWheel, const INT16 &rightWheel, 
							   const ResultType_t &resultType, const float &sumLimit, const float &diffLimit, 
							   Comparison_t comparison, TestResultDetails &details, string &axle,
							   string leftDisplayTag, string rightDisplayTag, 
							   string sumDisplayTag, string diffDisplayTag,
							   string sumBgColorTag, string diffBgColorTag);

	/**
	 * Get/Set the average brake force for the specfied wheel.
	 * 
	 * @param wheel    Wheel to set/get the average brake force of.
	 * @param avgForce Average brake force to store.
	 * 
	 * @return Average brake force for the specified wheel.
	 */
	const float AverageBrakeForce(const INT16 &wheel, const float *avgForce = NULL);

	/**
	 * The roll motors will be disabled.
	 * The individual motor command values will be set to 0.
	 * The motor mode will also be switched back to BOOST mode.
	 * 
	 * @return Status of disabling the motors.
	 */
	BEP_STATUS_TYPE DisableRollMotors(const string motorMode);

	/**
	 * Get the axle weight for the specified axle.
	 * 
	 * @param axle   Axle to get the weight of.
	 * 
	 * @return Weight of the specified axle.
	 */
	float GetAxleWeight(string axle);

	/**
	 * Check to see if wheel has reached a specified speed
	 * 
	 * @param wheelTag - the wheel to check
	 * @param targetSpeed - the speed that should be reached
	 * @param tolerance - the percent tolerance that the wheelspeed needs to reach to be in the correct speed band
	 * 
	 * @return BEP_STATUS_TYPE
	 */ 
	bool IsWheelInSpeedRange(const string &wheelTag, const float &targetSpeed, const float &tolerance);

	/**
	 * Monitor the maximum brake force data developed on each roller.
	 * The maximum observed brake force will be stored for each wheel being monitored.  When the required brake force for
	 * each wheel has been detected, that wheel will be marked as completed.  After all wheels are complete or a timeout
	 * occurs, the monitoring routine will exit.
	 * 
	 * @param startingRoller
	 *                  Roller to start monitoring.
	 * @param stoppingRoller
	 *                  Roller to stop monitoring
	 * @param minimumRequiredForceFront
	 *                  Minimum required front brake force.
	 * @param minimumRequiredForceRear
	 *                  Minimum required rear brake force.
	 * @param brakeData Structure to hold data tags and results.
	 * @param samplesToAverage
	 *                  Number of force samples consistently above the limit to indicate the brakes are applied.
	 * 
	 * @return Flag indicating if the minimum force has been observed on each wheel being monitored.
	 */
	bool MonitorBrakeForces(INT16 startingRoller, INT16 stoppingRoller,
							float minimumRequiredForceFront, float minimumRequiredForceRear,
							MaxBrakeData *brakeData, UINT16 samplesToAverage);

	/**
	 * Read the current load cell values.
	 * 
	 * @param brakeData Structure to store the current load cell readings in.
	 * @param startingRoller
	 *                  Roller to start monitoring.
	 */
	void ReadCurrentLoadCellValues(MaxBrakeData *brakeData, UINT16 startingRoller);

	/**
	 * Setup and configure the data structure.
	 * 
	 * @param dataStruct Data structure to be configured.
	 * @param loadCellTags
	 *                   System tags to use for getting the current load cell reading.
	 * @param displayTags
	 *                   System tag to use for displaying the data on the screen.
	 */
	void SetupLoadCellDataStructure(MaxBrakeData dataStruct[], string loadCellTags[], string displayTags[]);

	/**
	 * Send the start test command to the test head for the specified test step.
	 * 
	 * @param testStep Test step to be commanded.
	 * 
	 * @return Flaqg indicating if the test was commanded.
	 */
	bool StartTestHeadTest(const string &testStep);

	/**
	 * Clear the start test command to the test head for the specified test step.
	 * 
	 * @param testStep Test step command to be cleared
	 */
	void StopTestHeadTest(const string &testStep);

	/**
	 * Wait for the ABS to build pressure at the testing wheel.
	 * 
	 * @param testingWheel
	 *               Wheel being tested.
	 * @param oppositeWheel
	 *               Opposite wheel on the same axle of the wheel being tested.
	 * 
	 * @return Result of monitoring for the ABS build.
	 */
	string WaitForAbsBuild(const INT16 &testingWheel, const INT16 &oppositeWheel);

	/**
	 * Wait for the ABS to dump pressure on the specified wheel.
	 * 
	 * @param testingWheel
	 *               Index of the wheel being tested
	 * @param oppositeWheel
	 *               Index of the wheel on the opposite side of the axle from the testing wheel
	 * 
	 * @return Result of the ABS dump cycle
	 */
	string WaitForAbsDump(const INT16 &testingWheel, const INT16 &oppositeWheel);

	/**
	 * Each wheel can be commanded a speed and the function will return a value after
	 * every wheelspeed is obtained or it timesout.
	 * 
	 * Car should be shifted to N before this step runs
	 * 
	 * @param lfSpeed - wheelspeed
	 * @param rfSpeed - wheelspeed
	 * @param lrSpeed - wheelspeed
	 * @param rrSpeed - wheelspeed
	 * 
	 * @return bep status values (BEP_STATUS_SUCCESS for pass)
	 */
	BEP_STATUS_TYPE WaitForWheelSpeedsToBeReached(float lfSpeed, float rfSpeed, float lrSpeed, float rrSpeed);

	// Valid Mazda test head test steps
	XmlNodeMap m_testHeadTestSteps;

	// Array to hold the average brake force values
	float m_avgBrakeForces[4];

};
//-------------------------------------------------------------------------------------------------
#endif //MazdaBrakeTC_h
