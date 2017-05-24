//*************************************************************************
// Description:
// This file contains the functions necessary to perform a base brake test on
// the machine.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/Brake/Include/BaseBrakeTool.h $
// 
// 9     10/30/07 1:42p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed and updated the Javadoc comments.
// 
// 8     1/17/07 3:20p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added WarmInitialize() and Deactivate().
// - Made ForceThread() a member of the class.
// - Added m_launchForceThread.
// 
// 7     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 6     4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 6     4/12/05 5:29p Rwiersema
// Added methods for new Coast test.
//
// 5     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 4     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 3     2/18/05 11:57a Bmeinke
// Added Dan E's changes to only open the ICM binary data files once per
// component lifetime
//
// 3     8/18/04 11:11p Cward
// changed angle brackets to quotes of included non system files
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 2     10/28/03 7:04a Derickso
// Latest updates and working copy.
//
// 1     10/24/03 3:20p Canyanwu
// Start of DCX working directory
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================

#ifndef BASEBRAKETOOL_H
#define BASEBRAKETOOL_H

#include "GenericTC.h"
#include "BepDefs.h"
#include "BepSync.h"
#include <strstream.h>
#include <math.h>

using std::string;

/**
 * The list of wheel force information.
 */
typedef vector< float >             FORCELIST;

typedef enum
{
    SAMPLE,
    STOP,
    TERMINATE
} FORCETHREADSTATE;

class BaseBrakeToolTest;

typedef struct _BrakeDataStats
{
    INT32  minValueIndex;
    INT32  maxValueIndex;
    float  minValue;
    float  maxValue;
    float  averageValue;
}BrakeDataStats;


/**
 * BaseBrakeTool is a utility class for functionality related to
 * base brake tests
 *
 * @author Chris TenHarmsel
 * @since Sept. 18 2003
 */
class BaseBrakeTool
{
friend class BaseBrakeToolTest;
public:
    /**
     * BaseBrakeTool constructor.
     *
     * @param tc pointer to a Test Component object used for logging, reading
     * test step parameters, and other test component related information.
     */
    BaseBrakeTool(GenericTC *tc);

    /**
     * BaseBrakeTool destructor.
     */
    virtual ~BaseBrakeTool();
    /**
     * Get the instantaneous wheel speed for each of the four wheels.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The current wheel speeds will be read from the system and returned to the calling function.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetWheelSpeeds() </li>
     *      </ul>
     *
     * @param wheelSpeed The array in which to store the current wheel speeds read from the system.
     * @return The status of the wheel speed read.
     */
    INT32 GetISpeeds(float wheelSpeed[]);

    /**
     * Get the instantaneous wheel forces for each of the four wheels.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The current forces will be retrieved from the system and returned to the calling function.
     * The force file will be checked to verify it is open prior to attempting to read the wheel forces.
     * If the force file has not been opened, it will be opened and then the whel forces will be read.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentForces - Name of the file containing the current wheel forces. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> IsForceFileOpen() </li>
     *      <li> OpenForceFile() </li>
     *      <li> GetParameter() </li>
     *      <li> BposSleep() </li>
     *      </ul>
     *
     * @param wheelForce Array of wheel forces.
     * @return The status of reading the current wheel forces from the wheel force file.
     */
    INT32 GetIForces(float wheelForce[]);

    /**
     * Wait for a condition indicating forces can be read from the system.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * This function will wait until it receives a signal indicating forces can be read from the system.
     * Once the signal is received, this function will return.  This function mainly synchronizes the
     * force thread with the brake test component.
     */
    void StartForceReads(void);

    /**
     * Get the current status for updating the force values in the system.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The status of the force thread will be retrieved and returned to the calling function.
     * <br><p>
     *
     * @return The status of force updates on the system.
     *      <ul>
     *      <li> SAMPLE - Sample wheel forces. </li>
     *      <li> STOP - Wheel forces are not being sampled. </li>
     *      <li> TERMINATE - The force thread is scheduled to be terminated. </li>
     *      </ul>
     */
    FORCETHREADSTATE GetForceReadStatus(void);

    /**
     * Get the current wheel force.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The current maximum force value will be returned to the calling function.
     *
     * @return The current maximum force value.
     */
    float GetForce(void);

    /**
     * Set the current brake force.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The current maximum force value will be set in the system.
     *
     * @param force  Maximum wheel force from the vehicle.
     */
    void SetForce(float force);
    /**
     * Tag the data arrays for the drag test.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Once the vehicle speed has dropped below the drag test start speed, the machine will be disengaged.
     * There will be a delay while the machine disengages.  The data array will then be tagged with the
     * drag start index.  After measuring the specified number of drag samples, the drag end index will be
     * calculated.
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> DragSpeed - Speed at which to start sampling drag forces. </li>
     *      <li> DragDelay - Amount of time to wait for machine to disengage. </li>
     *      <li> DragSamples - Number of drag samples to use for drag analysis. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetRollerCount() </li>
     *      <li> GetSpeed() </li>
     *      <li> Disengage() </li>
     *      <li> StatusCheck() </li>
     *      <li> BposSleep() </li>
     *      <li> TimeRemaining() </li>
     *      <li> TagArray() </li>
     *      </ul>
     *
     * @param dragStart The starting sample for the drag check.
     * @param dragEnd   The ending sample number for the drag check.
     * @return The status of disengaging the machine and waiting for the required speed range to be achieved.
     */
    virtual INT32 Drag(INT32 &dragStart, INT32 &dragEnd);
    /**
     * Tag the force array with the brake start and end tags.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The vehicle speed will be monitored to determine if conditions are still correct for gathtering brake samples.
     * The force array will be tagged with the brake start tag either after the minimum brake force has been achieved
     * on the front wheels or the vehicle speed has dropped below the brake start speed.  After the required number of
     * samples are retrieved, the force array will be tagged with the brake end tag.
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> StartSamplingAtMinimumForce - Flag indicating if a minimum brake force should be aachieved before
     *                                         beginning to sample the brake force. </li>
     *      <li> MinimumStartForce - Minimum force at which to start sampling brake force. </li>
     *      <li> BaseBrakeSampleCount - Number of brake samples to collect. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> StartBrakeSampleSpeed - Vehicle speed at which to start sampling brake force. </li>
     *      <li> EndBrakeSampleSpeed - Vehicle speed at which to stop sampling brake force. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> GetParameterBool() </li>
     *      <li> TagArray() </li>
     *      <li> GetSpeed() </li>
     *      <li> GetIForces() </li>
     *      <li> BposSleep() </li>
     *      <li> BrakeTestStatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     *
     * @param brakeStart The starting sample of the brake check.
     * @param brakeEnd   The ending sample of the brake check.
     * @return The status of sampling the brake force.
     */
    virtual INT32 Brake(INT32 &brakeStart, INT32 &brakeEnd);
    virtual INT32 WaitForBrakeThreshhold();
    /**
     * Check for excessive brake force.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The force array for the entire test period will be checked to determine if excessive brake force
     * was applied at any time.  Excessive force can cause damage to certain tires to the point it becomes
     * a safety concern.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ExcessiveBrakeForceLimit - Maximum force limit allowed. Anything over this limit will be flagged as fail. </li>
     *      <li> NumberOfSamplesAboveLimit - Number of consecutive samples above the limit before a failure is triggered. </li>
     *      <li> IcmForceArray - Name of the file containing the wheel force data. </li>
     *      <li> FailForExcessiveBrakeForce - Flag indicating if the test should fail if excessive brake force was detected.
     *                                        Sometimes just a flag may need to be sent to the plant host, but the overall
     *                                        test should not fail. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> ExcessiveBrakeForce - Excessive brake force was detected during the test sequence. </li>
     *      <li> WheelForceDataReadError - Could not read wheel force. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> TagArray() </li>
     *      <li> GetBrakeStart() </li>
     *      <li> ReadDataForWheelForceArray() </li>
     *      <li> GetRollerCount() </li>
     *      <li> GetWheelForceValue() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of checking the force array for excessive brake force.
     *      <ul>
     *      <li> Pass - No excessive brake force was detected during the test sequence. </li>
     *      <li> Fail - Excessive brake force was detected during the test sequence. </li>
     *      </ul>
     */
    virtual string ExcessiveBrakeForceCheck(void);
    /**
     * Check the system status.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Check the current status of the machine and the component to determine if the current conditions are valid for
     * continuing a test.  If the system conditions are not correct, a failure or abort status will be returned.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> IgnoreMachineConditions - Flag indicating if machine conditions should be ignored.  Some machines will
     *                                     lose vehicle present above a certain speed, causing the test to abort.  </li>
     *      </ul>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Clutch - System tag for determining if the rollers are clutched. </li>
     *      <li> Declutch - System tag for determining if the rollers are de-clutched. </li>
     *      <li> RetainersUp - System flag indicating the status of the retaining rolls - up or down. </li>
     *      <li> VehiclePresent - System flag indicating if there is a vehicle present on the rollers. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::StatusCheck() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      </ul>
     *
     * @return The status of the system for testing.
     */
    virtual INT32 StatusCheck(void);
    /**
     * Check system status during brake test.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Check the current status of the machine and the component during the brake test to determine if the current conditions
     * are valid for continuing a test.  If the system conditions are not correct, a failure or abort status will be returned.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Clutch - System tag for determining if the rollers are clutched. </li>
     *      <li> Declutch - System tag for determining if the rollers are de-clutched. </li>
     *      <li> RetainersUp - System flag indicating the status of the retaining rolls - up or down. </li>
     *      <li> VehiclePresent - System flag indicating if there is a vehicle present on the rollers. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::StatusCheck() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      </ul>
     *
     * @param checkTestStatus Flag to indicate if TestStatus should be checked.
     * @return The status of the system for testing.
     */
    virtual INT32 BrakeTestStatusCheck(bool checkTestStatus = true);
    /**
     * Check if the brake values should be calculated.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The value of the brake test flag will be checked to determine if the brake values should be calculated.
     * <br><p>
     *
     * @return The status of performing the brake test.
     *      <ul>
     *      <li> Test - Perform the test. </li>
     *      <li> Skip - Skip the test. </li>
     *      <li> Pass - Previous pass. </li>
     *      <li> Fail - Previous fail. </li>
     *      </ul>
     */
    const std::string GetBrakeTestingStatus(void);

    /**
     * Store the value of the brake test flag.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Set the value of the brake test flag.  This is used to indicate if the brake test calaculations should be performed.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateResult() </li>
     *      </ul>
     *
     * @param status The current status for testing.
     *      <ul>
     *      <li> Test - Perform the test. </li>
     *      <li> Skip - Skip the test. </li>
     *      <li> Pass - Previous pass. </li>
     *      <li> Fail - Previous fail. </li>
     *      </ul>
     */
    void SetBrakeTestingStatus(const std::string status);
    /**
     * Initialize the brake test.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * All system tags the brake component is responsible for will be reset to ensure we are not displaying bad values
     * or using bad data in our test.  The machine will be engaged after it is determined the vehicle is at zero speed.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> ZerospeedPrompt - Prompt tag to use for instructing the operator to achieve zero speed. </li>
     *      </ul>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Zerospeed - System flag indicating if the vehicle is at zero speed. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> IsRetest() </li>
     *      <li> GetPreviousOverallResult() </li>
     *      <li> ResetSystemTags() </li>
     *      <li> StatusCheck() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompts() </li>
     *      <li> TimeRemaining() </li>
     *      <li> BposSleep() </li>
     *      <li> Engage() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return Test result of setting the system to test base brake functionality.
     *      <ul>
     *      <li> Pass - System was properly reset to test brake functionality. </li>
     *      <li> Fail - System status is preventing resetting of the machine. </li>
     *      <li> Timeout - The machine was not engaged in the time allowed. </li>
     *      <li> SoftwareFail - An exception occurred while resetting the machine. </li>
     *      </ul>
     */
    const string TestStepInitialize(void);
    virtual const string DisplaySpeedTimeGraph(void);
    virtual const string RemoveSpeedTimeGraph(void);
    /**
     * Prompt the operator to brake test speed.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The operator will be prompted to speed for brake testing.  The vehicle speed will then be monitored until the
     * required minimum speed has been achieved or time has expired.  When the arm speed has been achieved, the brake
     * test status will be updated to testing.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to display the target speed to the operator. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> SpeedTarget - Target speed for brake testing to display to the operator. </li>
     *      <li> ArmSpeed - Minimum speed that must be achieved to begin brake testing. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetBrakeTestingStatus() </li>
     *      <li> StatusCheck() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> GetSpeed() </li>
     *      <li> StatusCheck() </li>
     *      <li> BposSleep() </li>
     *      <li> TimeRemaining() </li>
     *      <li> SetBrakeTestingStatus() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Test result for acclerating to the brake test speed.
     *      <ul>
     *      <li> Pass - The required speed has been achieved. </li>
     *      <li> Timeout - The required speed has not been achieved in the time allowed. </li>
     *      <li> SoftwareFail - An exception occurred while monitoring the acceleration. </li>
     *      </ul>
     */
    virtual const string TestStepAccelerate(void);
    /**
     * Perform the drag test.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The operator will be prompted to place the vehicle into the proper state to correctly sample the drag forces.
     * Typically, this involves shifting the transmission to neutral and removing feet from all pedals (brake and throttle).
     * The drag forces will then be sampled.  This test step will only be performed if the brake testing status has
     * been set to Testing.
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetBrakeTestingStatus() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> Drag() </li>
     *      <li> SetBrakeTestingStatus() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return The test result for sampling the drag forces.
     *      <ul>
     *      <li> Pass - The drag forces were successfully sampled. </li>
     *      <li> Fail - A failure occured sampling drag forces. </li>
     *      <li> SoftwareFail - An exception occurred whlie sampling drag forces. </li>
     *      </ul>
     */
    const string TestStepDrag (void);
    /**
     * Analyze the drag forces that were sampled.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The drag forces that were sampled during the test sequence will be analyzed.  They will also be checked against
     * parameters to determine a pass or fail result.  The drag forces will only be sampled if the brake testing status
     * is set to Testing.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TestResultBox1 - System tag representing test result box 1 on the display. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetBrakeTestingStatus() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> AnalyzeDrags() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return The test result for analyzing the drag forces sampled during the test.
     *      <ul>
     *      <li> Pass - Drag forces fell within accpetable parameters. </li>
     *      <li> Fail - Drag forces were above acceltable limits. </li>
     *      <li> Skip - The drag forces were not analyzed. </li>
     *      <li> SoftwareFail - An exception occurred while analyzing the drag forces. </li>
     *      </ul>
     */
    const string TestStepAnalyzeDrag (void);
    /**
     * Perform the base brake test.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The operator will be prompted to apply brakes until the brake force reaches the target force range.  The brake
     * brake forces will then be sampled.  The brake forces will only be sampled if the brake testing status is
     * set to Testing.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> BrakeActive - Tag used to enable and disable the brake force meter. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DisplayBrakeMeterUntilZeroSpeed - Flag to indicate if the brake force meter should continue displaying
     *                                             brake forces until the vehicle reaches zero speed. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetBrakeTestingStatus() </li>
     *      <li> BrakeTestStatusCheck() </li>
     *      <li> UpdateTarget() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> Brake() </li>
     *      <li> GetParameterBool() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return The test result for the base brake test.
     *      <ul>
     *      <li> Pass - The base brake samples were collected. </li>
     *      <li> Fail - The base brake samples could not be collected. </li>
     *      <li> Skip - The base brake test was not performed. </li>
     *      <li> SoftwareFail - An exception occurred sampling the base brake forces. </li>
     *      </ul>
     */
    const string TestStepBaseBrake(void);
    const string BrakeToThreshhold(void);
    /**
     * Analyze the base brake forces.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * Analyze the base brake forces that were sampled during the test.  The result will be displayed for the operator
     * in TestResultBox2.  The brake forces will only be analyzed if the brake testing status is set to Testing.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TestResultBox2 - System tag for Test Result Box 2 on the display. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetBrakeTestingStatus() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> AnalyzeBrakeForces() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return Test result of analyzing the base brake forces.
     *      <ul>
     *      <li> Pass - The brake forces were above the minimum required brake force. </li>
     *      <li> Fail - The brake forces were below the minimum required brake force. </li>
     *      <li> Skip - The brake forces were not analyzed. </li>
     *      <li> SoftwareFail - An exception occurred analyzing the brake forces. </li>
     *      </ul>
     */
    const string TestStepAnalyzeBaseBrake (void);
    /**
     * Validate brake balances.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The brake balances will be analyzed to determine if they are within the specified parameters.  Test result box 3
     * will be updated with the result for the operator.  The brake balances will only be validated if the brake testing
     * status is set to Testing.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TestResultBox3 - System tag for test result box 3 on the display. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetBrakeTestingStatus() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> ValidateBalances() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return The test result of analyzing the brake balances.
     *      <ul>
     *      <li> Pass - The brake balances are within the specified parameters. </li>
     *      <li> Fail - The brake balances are <b><i>not</i></b> within the specified parameters. </li>
     *      <li> Skip - The brake balances were not analyzed. </li>
     *      <li> SoftwareFail - An exception occurred analyzing the brake balances. </li>
     *      </ul>
     */
    const string TestStepBalance (void);
    /**
     * This test step is responsible for prompting the operator to
     * accelerate to the dynamic park brake arm speed. If we are testing 
     * the front axle alone (no drive axle), we will accelerate the rollers
     * via the MotorController
     * 
     * @return The status of the acceleration test step.
     */
    const string TestStepAccelerateToParkBrakeSpeed(void);
    /**
     * This test step is responsible for prompting the driver 
     * to shift to neutral and applying the park brake to measure
     * the forces generated by the park brake
     * 
     * @return The status of the test step.
     */
    const string TestStepDynamicParkBrake(void);
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
     */
    INT32 DynamicParkBrake(INT32 &brakeStart, INT32 &brakeEnd, const string &tagPrefix);
    /**
     * Analyze the forces generated during the dynamic park brake test
     * 
     * @return The status of the test step.
     */
    const string AnalyzeDynamicParkBrake(void);
    /**
     * Read the brake arrays and calculate the park brake forces for each
     * of the vehicles wheels and analyze the data.
     *
     * @param brakeStart The starting sample index of the brake force array.
     * @param brakeEnd   The ending sample index of the brake force array.
     * @return The status of the operation.
     */
    INT32 AnalyzeParkBrakeForces(INT32 brakeStart, INT32 brakeEnd);
    /**
     * Validate the park brake force of each wheel and update the display
     * with the result.
     *
     * @param roller  The roller to analyze the park brake force result for.
     * @param average The average park brake force for the specified wheel.
     * @return The status of the operation.
     * @since Version 1.0
     */
    INT32 ValidateParkBrakeForce(INT32 roller, float average);
    /**
     * Prompt the operator to brake to stop.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The operator will be prompted to brake to stop.  The vehicle speed will then be monitored until zero speed is
     * reached or until time has expired.  When zero speed has been achieved, or time has expired, the motors will
     * be placed into Boost mode.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Zerospeed - System tag indicating if the vehicle is at zero speed. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> StatusCheck() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> TimeRemaining() </li>
     *      <li> BposSleep() </li>
     *      <li> UpdateResult() </li>
     *      <li> GetDataTag() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return The test result for braking to stop.
     *      <ul>
     *      <li> Pass - The vehicle reached zero speed in the time allowed. </li>
     *      <li> Skip - The test step was skipped. </li>
     *      <li> Timeout - The vehicle did not reach zero speed in the time allowed. </li>
     *      <li> SoftwareFail - An exception occurred while waiting for the operator to brake to stop. </li>
     *      </ul>
     */
    const std::string TestStepBrakeToStop(void);
    /**
     * Finish up the brake testing.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The machine will be engaged.  The overall result of the brake test will also be determined based on the results
     * of all the test steps that have been performed.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Zerospeed - System flag indicating if the vehicle is at zero speed. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> TimeRemaining() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> BposSleep() </li>
     *      <li> Engage() </li>
     *      <li> TimeRemaining() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      <li> SendOverallResult() </li>
     *      </ul>
     *
     * @return The test result of the finish up testing.
     *      <ul>
     *      <li> Pass - The machine was engaged. </li>
     *      <li> Fail - The vehicle did not reach zero speed in the time allowed. </li>
     *      <li> Skip - The test step was not performed. </li>
     *      <li> SoftwareFail - An exception occurred during the test step. </li>
     *      </ul>
     */
    const string TestStepFinishUp (void);
    /**
     * Engage the machine.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The machine will be engaged.  
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> Engage() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return The test result of the finish up testing.
     *      <ul>
     *      <li> Pass - The machine was engaged. </li>
     *      <li> Fail - The vehicle did not reach zero speed in the time allowed. </li>
     *      <li> SoftwareFail - An exception occurred during the test step. </li>
     *      </ul>
     */
    const string TestStepEngage (void);
    /**
     * Prompt the operator to coast down to target speed.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * If the brake test status flag is set to Testing, the operator will be prompted to coast down to the indicated
     * speed range.
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use to display a speed target rnage. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> SpeedTarget - Speed target range for the operator to achieve. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetBrakeTestingStatus() </li>
     *      <li> SetBrakeTestingStatus() </li>
     *      <li> StatusCheck() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> Coast() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return The test result for coasting down to speed.
     *      <ul>
     *      <li> Pass - The vehicle coasted to the target speed in the time allowed. </li>
     *      <li> Fail - System status is preventing the test from continuing. </li>
     *      <li> Timeout - The vehicle failed to coast down to the target speed in the time allowed. </li>
     *      <li> Skip - The test step was not performed. </li>
     *      <li> SoftwareFail - An exception occurred while the vehicle was coasting down to speed. </li>
     *      </ul>
     */
    const string TestStepCoast(void);
    /**
     * Calculates the stopping distance.
     * The initial distances are read and stored when the force array is tagged with the start of the brake test.
     * 
     * @return Result of the stopping distance.
     */
    string AnalyzeStoppingDistance(void);
    /**
     * Verify the vehicle coasts to the target speed.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The wheel speeds will be monitored to ensure the vehicle costs to the target speed.  The force array will be
     * tagged with the start of the coast period.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> NeutralLamp - System data tag to turn the Neutral lamp on and off. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> CoastEndSpeed - Target speed for the vehicle to coast down to. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> TagArray() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetSpeed() </li>
     *      <li> StatusCheck() </li>
     *      <li> BposSleep() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     *
     * @return The test result for coasting to target speed.
     *      <ul>
     *      <li> Pass - The vehicle reached the target speed in the time allowed. </li>
     *      <li> Fail - System status is preventing testing from continuing. </li>
     *      <li> Timeout - The vehicle did not reach the target speed in the time allowed. </li>
     *      </ul>
     */
    INT32 Coast(void);
    /**
     * Validate the drag forces sampled during the test.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The drag forces that were sampled will be averaged for each wheel.  The average force for each wheel will then
     * be validated against parameters to ensure it is below the maximum allowed drag force.  The results will then be
     * displayed for the operator.  A parameter will also be checked to determine if the drag forces should be updated
     * in the ICM for each wheel.  If the drag values are updated in the ICM, they will not need to be subtracted from
     * the brake values later.  In order for the drag values to be removed from the brake forces, this utility must be
     * run prior to the brake forces being sampled.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> xxDragValue - System tag for the average drag value of the specified roller, where xx is the roller name. </li>
     *      <li> xxDragBGColor - System tag for the background color of the drag value for the specified roller where
     *                           xx is the roller name. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> IcmForceArray - The name of the force array kept by the ICM. </li>
     *      <li> IncludeDragInBrakeForce - Flag to optionally include the drag force in the brake force calculation.
     *                                     If this parameter is set to false, the drag values will be updated in the
     *                                     ICM and removed from the brake forces. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> FrontMaxDragForce - Maximum drag force allowed for the front wheels. </li>
     *      <li> RearMaxDragForce - Maximum drag force allowed for the rear wheels. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> xxDragFault - The specified wheel failed the drag test, where xx is the roller name. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadDataArrays() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetRollerCount() </li>
     *      <li> AverageForces() </li>
     *      <li> ValidateDrag() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> CreateMessage() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> UpdateResult() </l/i>
     *      </ul>
     *
     * @param dragStart The starting sample index of the brake force array.
     * @param dragEnd   The ending sample index of the brake force array.
     *
     * @return The result of validating the drag forces.
     *      <ul>
     *      <li> Pass - All drag forces were below the parameter limits. </li>
     *      <li> Fail - One or more wheels failed for high drag values. </li>
     *      </ul>
     */
    INT32 AnalyzeDrags(INT32 dragStart, INT32 dragEnd);
    /**
     * Validate the brake force at each wheel.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The brake force for each wheel will be averaged and verified against parameters to ensure the brake force at
     * each wheel is above minimum limits.  The results will then be updated on the display for the operator.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> xxForceValue - System tag for the brake force for the specified wheel where xx is the specified wheel. </li>
     *      <li> xxForceBGColor - System tag for the color code for the specified wheel where xx is the specified wheel. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> IcmForceArray - The name of the force array kept by the ICM. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> xxForceFault - The brake force for wheel xx was below the minimum acceptable brake force. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadDataArrays() </li>
     *      <li> GetParameter() </li>
     *      <li> GetRollerCount() </li>
     *      <li> AverageForces() </li>
     *      <li> ValidateBrakeForce() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> UpdateResult() </li>
     *      </ul>
     *
     * @param brakeStart The starting sample index of the brake force array.
     * @param brakeEnd   The ending sample index of the brake force array.
     *
     * @return The test result for validating the brake force at each wheel.
     *      <ul>
     *      <li> Pass - All brake forces were above the minimum brake force. </li>
     *      <li> Fail - One or more brake forces were below the minimum allowed brake force. </li>
     *      </ul>
     */
    INT32 AnalyzeBrakeForces(INT32 brakeStart, INT32 brakeEnd);
    /**
     * Verify the drag value for the specified wheel was below the maximum allowed drag value.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The drag value for the specified wheel will be compared to parameters limits.  Seperate limits can be set for the
     * front and rear axles.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> FrontMaxDragForce - Maximum drag force allowed for the front wheels. </li>
     *      <li> RearMaxDragForce - Maximum drag force allowed for the rear wheels. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      </ul>
     *
     * @param roller  The roller to analyze the drag result for.
     * @param average The average drag for the specified wheel.
     *
     * @return The status of validating the drag force at the specified wheel.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The drag force for the specified wheel was below the maximum allowed. </li>
     *      <li> BEP_STATUS_FAILURE - The drag force for the specified wheel was <b><i>above</i></b> the maximum allowed. </li>
     *      </ul>
     */
    INT32 ValidateDrag(INT32 roller, float average);
    /**
     * Verify the drag value for the specified wheel was above the
     * minimum allowed drag value.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The drag value for the specified wheel will be compared to parameters limits.  Seperate limits can be set for the
     * front and rear axles.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> FrontMinDragForce - Minimum drag force allowed for
     *      the front wheels. </li>
     *      <li> RearMinDragForce - Minimum drag force allowed for
     *      the rear wheels. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      </ul>
     *
     * @param roller  The roller to analyze the drag result for.
     * @param average The average drag for the specified wheel.
     *
     * @return The status of validating the drag force at the specified wheel.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The drag force for the
     *      specified wheel was above the minimum allowed. </li>
     *      <li> BEP_STATUS_FAILURE - The drag force for the
     *      specified wheel was <b><i>below</i></b> the minimum
     *      allowed.
     *      </li>
     *      </ul>
     */
    INT32 ValidateMinDrag(INT32 roller, float average);
    /**
     * Validate the brake force for the specified wheel.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The brake force for the specified wheel will be checked to ensure it is within acceptable parameters.  The brake
     * force will be compared against a lower and upper limit.  This function supports different brake force ranges for
     * the front and rear axles.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> xxForceValue - System tag used to report the average brake force for wheel xx. </li>
     *      <li> xxForceBGColor - System tag used to display the correct test status color for wheel xx. </li>
     *      </ul>
     *
     * <b>Test Step Info: </b>
     *      <ul>
     *      <li> FrontMinBrakeForce - Minimum front brake force allowed. </li>
     *      <li> FrontMaxBrakeForce - Maximum front brake force allowed. </li>
     *      <li> RearMinBrakeForce - Minimum rear brake force allowed. </li>
     *      <li> RearMaxBrakeForce - Maximum rear brake force allowed. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @param roller  The roller to analyze the brake force result for.
     * @param average The average brake force for the specified wheel.
     *
     * @return The status of validating the brake force for the specified wheel.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The brake force for the specified wheel was within an acceptable range. </li>
     *      <li> BEP_STATUS_FAILURE - The brake force for the specified wheel was out of range. </li>
     *      </ul>
     */
    INT32 ValidateBrakeForce(INT32 roller, float average);
    /**
     * Validate the brake balances are within range for the vehicle.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The side to side brake balance will be validated for both the front and rear axle.  The front to rear brake balance
     * will also be calculated and validated.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> FrontBalance() </li>
     *      <li> RearBalance() </li>
     *      <li> FrontToRearBalance() </li>
     *      <li> ValidateTandemBalances() </li>
     *      <li> UpdateResult() </li>
     *      </ul>
     *
     * @return The status of validating the brake balances.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - All brake balances are within acceptable ranges. </li>
     *      <li> BEP_STATUS_FAILURE - One or more balance calculations were out of range. </li>
     *      </ul>
     */
    INT32 ValidateBalances(void);
    /**
     * Validate the front brake balance.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The front side to side brake balance will be checked to ensure it is within the appropriate range.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> FrontBalanceValue - System tag for reporting front side to side brake balance. </li>
     *      <li> FrontBalanceBGColor - System tag for the color code of the front balance result. </li>
     *      <li> FrontBalanceTest - Tag to use for reporting the results of the front brake balance test to the TestResultServer. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> FrontSideToSideBalance - Side to side brake balance percent allowed for the front axle. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> FrontBalanceFault - Front side to side balance is out of range. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> CreateMessage() </li>
     *      </ul>
     *
     * @return The result of validating the front side to side brake balance.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Front brake balance is within range. </li>
     *      <li> BEP_STATUS_FAILURE - The front side to side brake balance is out of limits. </li>
     *      </ul>
     */
    INT32 FrontBalance(void);
    /**
     * Validate the rear brake balance.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The rear side to side brake balance will be checked to ensure it is within the appropriate range.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> RearBalanceValue - System tag for reporting rear side to side brake balance. </li>
     *      <li> RearBalanceBGColor - System tag for the color code of the rear balance result. </li>
     *      <li> RearBalanceTest - Tag to use for reporting the results of the rear brake balance test to the TestResultServer. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> RearSideToSideBalance - Side to side brake balance percent allowed for the rear axle. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> RearBalanceFault - Rear side to side balance is out of range. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> CreateMessage() </li>
     *      </ul>
     *
     * @return The result of validating the rear side to side brake balance.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Rear brake balance is within range. </li>
     *      <li> BEP_STATUS_FAILURE - The rear side to side brake balance is out of limits. </li>
     *      </ul>
     */
    INT32 RearBalance(void);
	/**
     * Validate the front to rear brake balances.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The front to rear brake balances will be validated to ensure brake force is distributed properly between the
     * front and rear axles.  A parameter controls which axle is used in the calculation (front or rear).
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> FrontToRearBalanceValue - System tag used to report the front to rear brake balance percent. </li>
     *      <li> FrontToRearBalanceBGColor - System tag for the test result color code for the front to rear brake balance. </li>
     *      <li> FrontToRearBalanceTest - Tag to use for reporting the front to rear balance results to the TestResultServer. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> FrontToRearBalanceUseFront - Flag to determine which axle sum to use in the calculation. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> BalanceFrontToRearMin - Minimum front to rear brake balance required. </li>
     *      <li> BalanceFrontToRearMax - Maximum front to rear brake balace allowed. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> FrontToRearBalanceFault - The front to rear brake balance is out of range. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> GetParameterBool() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> CreateMessage() </li>
     *      </ul>
     *
     * @return The result of the front to rear brake balance test.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The front to rear brake balance is within limits. </li>
     *      <li> BEP_STATUS_FAILURE - The front to rear brake balance is out of range. </li>
     *      </ul>
     */
    INT32 FrontToRearBalance(void);
    /**
     * Validate the tandem side to side brake balance.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The side to side brake balances will be validated on the tandem axle.
     * The results will be displayed for the operator.  This function will only be run if the roller count is
     * greater than 4.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> TandemBalance() </li>
     *      <li> TandemToRearBalance() </li>
     *      </ul>
     *
     * @return The result of the tandem brake balance validation.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The test was either not performed, or the balances are within limits. </li>
     *      <li> BEP_STATUS_FAILURE - Either the side to side balance on the tandem axle is out of range or the balance
     *                                between the tandem and rear axle is out of range. </li>
     *      </ul>
     */
    INT32 ValidateTandemBalances(void);
    /**
     * Validate the side to side brake balance on the tandem axle.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The side to side brake balance on the tandem axle will be validated against parameters to ensure the brake
     * force is distributed appropriately.  The results of the validation will be written to the system.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TandemBalanceValue - System tag for reporting the tandem brake balance result. </li>
     *      <li> TandemBalanceBGColor - Color code for the tandem brake balance result. </li>
     *      <li> TandemBalanceTest - Test result tag to use to report the results to the TestResultServer. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> TandemSideToSideBalance - Side to side balance percent limit for tandem axle. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> TandemBalanceFault - The sise to side brake balance on the tandem axle is out of limits. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> CreateMessage() </li>
     *      </ul>
     *
     * @return The result of the tandem axle brake balance validation.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The side to side brake balance on the tandem axle is within limits. </li>
     *      <li> BEP_STATUS_FAILURE - The side to side brake balance on the tandem axle is out of range. </li>
     *      </ul>
     */
    INT32 TandemBalance(void);
    /**
     * Validate the brake balance between the tandem and rear axles.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The brake balance between the tandem and rear axles will be calculated and reported to the system.  The balance
     * will be compared to the parameter limit and the result will be reported to the system.
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TandemToRearBalanceValue - System tag for reporting the tandem to rear axle brake balance percent. </li>
     *      <li> TandemToRearBalanceBGColor - Color code for the result of the balance validation. </li>
     *      <li> TandemToRearBalanceTest - Test result tag to use for reporting the the tandem to rear axle brake
     *                                     balance results. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> BalanceTandemToRearMin - Minimum brake balance percent between tandem and rear axles. </li>
     *      <li> BalanceTandemToRearMax - Maximum brake balance percent between tandem and rear axles. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> TandemToRearBalanceFault - The brake balance between the tandem and rear axles is out of limits. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> CreateMessage() </li>
     *      </ul>
     *
     * @return The result of the tandem axle to rear axle brake balance.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The brake balance between the tandem and rear axles is acceptable. </li>
     *      <li> BEP_STATUS_FAILURE - The brake balance between the tandem and rear axles is out of limits. </li>
     *      </ul>
     */
    INT32 TandemToRearBalance(void);
    /**
     * Check for wheel speeds above or below specified speed.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The current wheel speeds will be compared against the specified target speed.  The comparison will be based on the
     * argument passed in, where a value of true will check for speeds faster than that specified and false will check
     * for speeds slower than that specified.
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetISpeeds() </li>
     *      </ul>
     *
     * @param speed  The speed at which to compare the wheel speeds to.
     * @param faster Flag to determine how to compare speeds.
     *      <ul>
     *      <li> true - Check for all wheels above speed. </li>
     *      <li> false - Check for all wheels below speed. </li>
     *      </ul>
     *
     * @return The result of the check.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - All wheel speeds meet the specifed conditions. </li>
     *      <li> BEP_STATUS_FAILURE - All wheel speeds do not meet the specified conditions. </li>
     *      </ul>
     */
    INT32 SpeedCheck(float speed, bool faster);
    /**
     * Get the slowest wheel speed.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The wheel speeds will be sampled from the system.  The slowest wheel speed will then be found and returned to
     * the calling function.  If the wheel speeds can not be sampled, an exception will be thrown.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetISpeeds() </li>
     *      </ul>
     *
     * @return The slowest curent wheel speed.
     */
    float GetSpeed(void);

    /**
     * Enable or disable the brake force meter on the display.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The brake force meter will be either enabled or disabled based on the argument passed into the function.
     * If the brake force meter is being enabled, the target range will be displayed and force updates will be enabled.
     * If the brake force meter is being disabled, the force updates will be disabled.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> BrakeDomain - System tag to use for setting the brake domain on the display. </li>
     *      <li> BrakeTarget - System tag to use for displaying the brake force target range. </li>
     *      <li> BrakeActive - System tag to use for activating and deactivating the brake force meter. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> BrakeDomain - Domain to use for setting up the brake force meter.  This is the min and max values
     *                         capable of being displayed. </li>
     *      <li> BrakeTarget - Target brake force range to display for the operator. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> EnableForceUpdates() </li>
     *      <li> DisableForceUpdates() </li>
     *      </ul>
     *
     * @param status The state to set the brake force meter to.
     *      <ul>
     *      <li> true - Enable the brake force meter. </li>
     *      <li> false - Disable the brake force meter. </li>
     *      </ul>
     *
     * @return The status of changing the state of the brake force meter.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The brake force meter was set to the specified state. </li>
     *      <li> BEP_STATUS_FAILURE - The brake force meter could not be set to the desired state. </li>
     *      </ul>
     */
    INT32 UpdateTarget(bool status = true);
#if 0 // This function could not be found in the source code file.
    /**
     * Reads the force array from the ICM.
     *
     * Each samples in the array will contain an entry for:
     * lf, rf, lr, and rr for a 4 roller machine
     * lf, rf, lr, rr, lt, and rt for a 6 roller machine
     * This will be read into the force arrays for processing.
     *
     * @param startIdx Index of first relevant sample
     * @param endIdx   Index of last relevant sample
     * @return Status of the operation.
     * @since Version 1.0
     */
    INT32 ReadForceArrays(int startIdx, int endIdx);
#endif
    /**
     * Calculate the average force for the specified wheel.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The averavge force for the specified wheel will be calculated and returned to the calling function.  The specified
     * wheel will be checked to ensure force data exists for it prior to attmepting to calculate the average force.
     * <br><p>
     *
     * @param wheel   The wheel to analyze.
     * @param average The average value of the force array.
     * @param dataStats Stats to store for the data being analyzed.
     *
     * @return Status of calculating the average force.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The average force was calculated. </li>
     *      <li> BEP_STATUS_FAILURE - No forces were sampled for the specified wheel or the specified wheelcis not
     *                                a valid wheel. </li>
     *      </ul>
     */
    INT32 AverageForces(int wheel, float &average, BrakeDataStats &dataStats);
    /**
     * Disengage the machine.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Command the motor controller to disable boost and disengage the clutches from the rollers.  The system status
     * will then be monitored until the machine has completely disengaged or time expires.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> CommandTorque - System tag used to command the motors to a specified torque output. </li>
     *      <li> Clutch - System tag used to read the clutch status. </li>
     *      <li> Declutch - System tag used to read the declutch status. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemCommand() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> INamedDataBroker::Write() </li>
     *      <li> BrakeTestStatusCheck() </li>
     *      <li> BposSleep() </li>
     *      </ul>
     *
     * @return The status of disengaging the machine.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The machine has been disengaged. </li>
     *      <li> BEP_STATUS_FAILURE - The machine did not disengage or an exception occurred. </li>
     *      </ul>
     */
    INT32 Disengage(void);

    /**
     * Engage the machine.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The motor controller will be commanded to boost mode.  The clutches will also be pulled in to couple the wheels
     * together.  The wheels must be at zero speed in order to engage the clutches.  After the machine is commanded to
     * engage, the system status will be monitored until the machine has completely engaged or time expires.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Clutch - System tag used to read the clutch status. </li>
     *      <li> Declutch - System tag used to read the declutch status. </li>
     *      <li> Zerospeed - System tag used to determine if the rollers are at zero speed. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> INamedDataBroker::Write() </li>
     *      <li> SystemCommand() </li>
     *      <li> StatusCheck() </li>
     *      <li> BposSleep() </li>
     *      </ul>
     *
     * @return The status of engaging the machine.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The machine was engaged. </li>
     *      <li> BEP_STATUS_FAILURE - The machine did not engage or an exception occurred. </li>
     *      </ul>
     */
    INT32 Engage(void);

    /**
     * Enable the force updates to be reported to the system.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The force thread will be signaled to start sampling the wheel forces.  The wheel forces will then be written to
     * system tags so it can be updated on the display and elsewhere in the system.
     * <br><p>
     *
     * @return The status of the enabling wheel force updates.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The force thread was signaled to begin force updates.  </li>
     *      <li> BEP_STATUS_ERROR - The force thread could not be signaled to begin updating forces. </li>
     *      </ul>
     */
    INT32 EnableForceUpdates(void);

    /**
     * Disable force updates to the system.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The force thread will be signaled to stop reporting force updates to the system.
     * <br><p>
     *
     * @return The status of disabling the force updates.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The force thread was signaled to stop reporting force updates. </li>
     *      <li> BEP_STATUS_ERROR - The force thread could not be signaled to stop reporting forces. </li>
     *      </ul>
     */
    INT32 DisableForceUpdates(void);

    /**
     * Terminate the force thread.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The force thread will be signaled to terminate.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BposSleep() </li>
     *      </ul>
     *
     * @return The status of terminating the force thread.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The force thread was signaled to terminate. </li>
     *      <li> BEP_STATUS_ERROR - The force thread could not be signaled to terminate. </li>
     *      </ul>
     */
    INT32 TerminateForceUpdates(void);

    /**
     * Launch the force thread.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Launch the force thread to allow asynchronous updates of the brake force when performing other tasks in the
     * brake component.  If the thread could not be created, an excpetion will be thrown.
     * <br><p>
     */
    void LaunchForceThread(void);
    /**
     * Report the overall test result.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The overall test result will be set and reported to the TestResultServer.  The brake testing status will also
     * be set so that subsequent tests will be flagged as a retest.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetOverallResult() </li>
     *      <li> GetOverallResult() </li>
     *      <li> SendOverallResult() </li>
     *      <li> SetBrakeTestingStatus() </li>
     *      </ul>
     *
     * @param result The test result, if any, to update the overall test result with.
     */
    virtual void SendOverallResult(const std::string result = BEP_PASS_RESPONSE);
    /**
     * Reset the test component.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The brake testing status will be checked to determine of the test component has already passed.  If the brake
     * testing status is not set to passed, the status will be reset to untested.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetBrakeTestingStatus() </li>
     *      </ul>
     */
    void Reset(void);
    /**
     * Get the wheel force data for the period specified.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Get the wheel force data for the time period between startIdx and endIdx.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadDataArrays() </li>
     *      </ul>
     *
     * @param array     Name of the array to get wheel force data from.
     * @param startIdx  Index of first requested sample.
     * @param endIdx    Index of last requested sample.
     *
     * @return Status of reading the wheel force data for the period specified.
     */
    INT32 ReadDataForWheelForceArray(const string &array, int startIdx, int endIdx);
    /**
     * Get the drag force value for the wheel specified.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Get the drag force value for the specified wheel.
     * <br><p>
     *
     * @param wheel  The wheel to get the drag force value of.
     *
     * @return The drag force value for the specified wheel.
     */
    float GetDragForceValue(const int &wheel);
    /**
     * Get the brake force value for the specified wheel.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Return the brake force for the wheel specified.
     * <br><p>
     *
     * @param wheel  The wheel to get the brake force value of.
     *
     * @return The brake force value for the specified wheel.
     */
    float GetBrakeForceValue(const int &wheel);
    /**
     * Set the brake force value for the given wheel to the given value.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The brake force value for the specified wheel will be stored in a member variable for convenient access and analysis.
     * 
     * @param wheel  Wheel to set the brake force value for
     * @param value  Brake force value for the given wheel
     */
    void SetBrakeForceValue(const int &wheel, float value);
    /**
     * Get the requested sample from the wheel force array.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Get the requested sample from the wheel force array for the specified wheel.
     * <br><p>
     *
     * @param wheel  The wheel to get the sample for.
     * @param trial  the individual sample to return from the selected wheel force array.
     *
     * @return The requested sample for the specified wheel.
     */
    float GetWheelForceValue(const int &wheel, const int &trial);
    /**
     * Get the wheel force data for the specified wheel.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The force array for the specified wheel will be returned to the calling function.
     * <br><p>
     *
     * @param wheel      The wheel to get the force data for.
     * @param wheelArray The force data for the specified wheel.
     */
    void GetWheelForceArray(const int &wheel, vector< float > &wheelArray);
    /**
     * Set the brake test start value.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Store the brake test start value.
     * <br><p>
     *
     * @param start Brake test start value.
     */
    void SetBrakeStart(const int &start);
    /**
     * Get the brake test start value.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Return the brake test start value.
     * <br><p>
     *
     * @return Brake test start value.
     */
    INT32 GetBrakeStart(void);
    /**
     * Set the brake test end value.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Store the end of the brake test.
     * <br><p>
     *
     * @param end  End of the brake test.
     */
    void SetBrakeEnd(const int &end);
    /**
     * Get the brake test end value.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Return the brake test end value.
     * <br><p>
     *
     * @return The brake test end value.
     */
    INT32 GetBrakeEnd(void);
    /**
     * Reinitialize component.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Reinitialize the component using the already loaded configuration file. This is intended to be used when a 
     * component is activated and there have been no modifications to any configuration files used by the component.
     * Force updates will be disabled and the force thread will be launched.
     * <p>
     * <b>Functions Called:</b>
     *      <ul> 
     *      <li> DisableForceUpdates() </li>
     *      <li> LaunchForceThread() </li>
     *      </ul>
     */
    virtual void WarmInitialize();
    /**
     * Deactivate the component.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Deactivate the component at the end of a test sequence.  This allows the component to enter a "stand-by" mode
     * until needed again.  Deactivating the component requires much less time and resources than killing off the 
     * process and restarting it.  The force file will be closed and the force updates will be terminated.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CloseForceFile() </li>
     *      <li> TerminateForceUpdates() </li>
     *      </ul>
     */
    virtual void Deactivate();
    /**
     * The test component pointer that will be used for logging and other
     * methods that are in Test Componenet that might be needed
     */
    GenericTC *m_component;

protected:

    /**
     * Update the system with the current force values.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The force thread periodically reads the force data and reports it to the system.  The thread will block until
     * the force read status is set to <i>SAMPLE</i>.  The force thread will then continue to sample forces and report the
     * data to the system until the force read status is set to <i>TERMINATE</i>.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> StartForceReads() </li>
     *      <li> GetForceReadStatus() </li>
     *      <li> GetIForces() </li>
     *      <li> SetForce() </li>
     *      </ul>
     * 
     * @param arg    Pointer to a BaseBrakeTool object
     * 
     * @return NULL
     */
    static void *ForceThread(void *brake);
    
    /**
     * Open the force file for reading wheel forces.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The wheel force file will be opened for reading.  If the force file is not opened, two additional attempts will
     * be made before returning a failure.  A flag will be set indicating if the file has been opened successfully.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentForces - Name of the wheel force file. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> SetForceFileOpen() </li>
     *      <li> IsForceFileOpen() </li>
     *      </ul>
     *
     * @return  The status of opening the wheel force file.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - File was opened successfully. </li>
     *      <li> BEP_STATUS_FAILURE - File was not opened. </li>
     *      </ul>
     */
    BEP_STATUS_TYPE OpenForceFile(void);
    /**
     * Close the force file.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * If the force file has been opened, it will be closed.  If an error occurrs attempting to clode the file, two
     * additional attempts will be made to clode the file before a failure is returned.  The force file open flag
     * will be set to indicate the file was closed.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsForceFileOpen() </li>
     *      <li> SetForceFileOpen() </li>
     *      </ul>
     *
     * @return The status of closing the force file.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Force file was closed succesfully. </li>
     *      <li> BEP_STATUS_FAILURE - Force file was not closed successfully. </li>
     *      </ul>
     */
    BEP_STATUS_TYPE CloseForceFile(void);
    /**
     * Check if the force file has been opened.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The flag indicating the open status of the force file will be returned.
     * <br><p>
     *
     * @return  The open status of the force file.
     *      <ul>
     *      <li> True - Force file was opened. </li>
     *      <li> False - Force file is not open. </li>
     *      </ul>
     */
    bool& IsForceFileOpen(void);
    /**
     * Store the open status of the force file.
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * The flag indicating the open status of the force file will be stored.
     * <br><p>
     *
     * @param fileOpen Flag indicating open status of force file.
     *      <ul>
     *      <li> True - force file is open. </li>
     *      <li> False - force file is closed. </li>
     *      </ul>
     */
    void SetForceFileOpen(bool fileOpen);

    /**
     * <p>
     * <b>Category:</b> Utility
     * <p>
     * <b>Description:</b><br>
     * Check the configuration file to see if the brake pedal force should be
     * added to the test results.  If the parameter is set to true, add the
     * current brake pedal force to the test results.
     * <br><p>
     */ 
    void ProcessBrakePedalForce(void);
	/**
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * Get the number of wheels to analyze.  If we are testing the front axle
	 * or a single rear axle on a tandem axle machine, the value will be two.  
	 * Otherwise, the value will be four.
	 * <br><p>
	 * 
	 * @return The number of wheels to analyze.
	 */ 
	UINT32 GetNumberOfWheelsToAnalyze(void);

	/**
	 * Report the results and details of the stopping distance test.
	 * 
	 * @param testName   Name of the test result to report
	 * @param testResult Overall result of analyzing the data.
	 * @param faultDesc  Test description.
	 * @param faultCode  Result code
	 * @param stoppingDistance
	 *                   Stopping distances for each of the wheels.
	 * @param lfResult   Result of the LF stopping distance.
	 * @param rfResult   Result of the RF stopping distance.
	 * @param lrResult   Result of the LR stopping distance.
	 * @param rrResult   Result of the RR stopping distance.
	 */
	virtual void SendBrakeStoppingDistanceResult(string testName, 
												 string testResult, string faultDesc, string faultCode,
												 WHEELINFO stoppingDistance, 
												 string lfResult, string rfResult, string lrResult, string rrResult);


#if 0
    /**
     * Open the speed file for reading wheel speeds.
     * A flag will also be set indicating of the file
     * has been opened successfully.
     *
     * @return BEP_STATUS_SUCCESS - File was opened successfully.
     *         BEP_STATUS_FAILURE - File was not opened.
     * @see SetSpeedFileOpen
     * @see IsSpeedFileOpen
     */
    BEP_STATUS_TYPE OpenSpeedFile(void);
    /**
     * Close the speed file.
     * A flag will be set indicating the speed file was closed.
     *
     * @return BEP_STATUS_SUCCESS - Speed file was closed succesfully.
     *         BEP_STATUS_FAILURE - Speed file was not closed successfully.
     * @see SetSpeedFileOpen
     * @see IsSpeedFileOpen
     */
    BEP_STATUS_TYPE CloseSpeedFile(void);
    /**
     * Returns a flag indicating if the speed file has been opened.
     *
     * @return True - Speed file was opened.
     *         False - Speed file is not open.
     */
    bool& IsSpeedFileOpen(void);
    /**
     * Set the flag indicating if the speed file is open.
     *
     * @param fileOpen Flag indicating open status of speed file.
     *                 True - speed file is open.
     *                 False - speed file is closed.
     */
    void SetSpeedFileOpen(bool fileOpen);
#endif

    /**
     * Wheel array that is used to store the force information
     * from a base brake cycle.
     */
    WHEELDATAARRAY m_wheelForceArray;
    /**
     * Brake Force for each wheel during the brake test.
     */
    FORCELIST m_brakeForce;

    /**
     * Park brake force for each wheel during the dynamic park brake test.
     */
    FORCELIST       m_parkBrakeForce;

    /**
     * Drag Force for each wheel after the vehicle was placed
     * in neutral.
     */
    FORCELIST m_dragForce;

    /**
     * Tag to indicate if the brake forces should be calculated.
     */
    std::string m_calcBrakeForce;

    /**
     * Sample number that the drag test started.
     */
    INT32 m_dragStart;

    /**
     * Sample number that the drag test ended.
     */
    INT32 m_dragEnd;

    /**
     * Sample number that the brake test was started on.
     */
    INT32 m_brakeStart;

    /**
     * Sample number that the brake test was completed on.
     */
    INT32 m_brakeEnd;

    /**
     * Dynamic park brake test start
     */
    INT32 m_dynParkBrakeStart;

    /**
     * Dynamic park brake test end
     */
    INT32 m_dynParkBrakeStop;

    /**
     * Semaphore that is used to block the update force thread
     * until the force needs to be updated on the GUI.
     */
    BepCondVar<FORCETHREADSTATE> m_updateForceSem;

    /**
     * Semaphore that is used to block the update force thread
     * until the force needs to be updated on the GUI.
     */
    BepSemaphore m_readForces;

    /**
     * Semaphore that is used to block the update force thread
     * until the force needs to be updated on the GUI.
     */
    BepSemaphore m_semReadCurrentForce;

    /**
     * The ID of the force thread.
     */
    pthread_t m_forceThreadID;

    /**
     * Flag that is used to instruct the background thread to
     *      <ul>
     *      <li> SAMPLE </li>
     *      <li> STOP </li>
     *      <li> TERMINATE </li>
     *      </ul>
     */
    FORCETHREADSTATE m_updateForceFlag;

    /**
     * The current force value.
     */
    float m_forceValue;

private:
    /**
     * File descriptor for the force file.
     */
    INT32 m_forceFile;
    /**
     * Flag indicating the open status of the force file.
     *      <ul>
     *      <li> True - File is open. </li>
     *      <li> False - File is closed. </li>
     *      </ul>
     */
    bool m_forceFileOpen;
    /**
     * Flag that tells if the force thread should be launched in 
     * WarmInitialize().  This allows the unit tests to run without launching
     * the force thread.
     */ 
    bool m_launchForceThread;
    /** Drag force test stats */
    vector<BrakeDataStats> m_dragTestStats;
    /** Brake force test stats */
    vector<BrakeDataStats> m_brakeTestStats;
    /** Variable to store the distances for measrueing the stopping distance */
    WHEELINFO m_brakeStoppingDistance[2];
};


#endif  // BASEBRAKETOOL_H
