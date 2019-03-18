//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2003-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Brake/Include/GenericABSTCTemplate.h $
// 
// 1     11/01/07 2:11p Derickso
// Corrected comments.
// Removed m_motorController* since this same variable is defined in the
// base class.
// 
// 1     10/05/07 2:19p Derickso
// Corrected formatting for doxygen comments.
// 
// 19    5/01/07 11:07a Rwiersem
// Changes for the 07050201 core release:
// 
// Updated the Javadoc comments.
// 
// 18    1/17/07 3:21p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added ReloadConfiguration().
// - Added SetESPEquipped(), IsESPEquipped(), and m_isESPEquipped.
// 
// 17    10/25/06 3:50p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added Deactivate() and WarmInitialize().  These methods are used to 
//       keep the test component running when a new drive curve starts. 
//     - Changed m_reduxRecovIndex to a <vector>
// 
// 16    6/07/06 5:33p Cward
// Propagating the determination of whether the calibrate gsensor test
// should be run or not to the generic ABS level.
//
// 15    5/08/06 7:06p Cward
// Add ability to wait for ABS sensor data to catch up to the actual roll
// speeds. Added wait for stable sensor speeds method. Updated Evaluate
// ABS, and Analyze Sensor Cross methods
//
// 14    4/13/06 3:21a Cward
// Added definition of GetAbsReferenceForce
//
// 2     2/24/06 1:15p Bmeinke
// Added new GetAbsReferenceForce() method. Returns the brake force value
// that should be used as the base brake reference force when calculating
// ABS reduction/recovery percentages
//
// 13    11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 12    5/25/05 8:55p Cward
// Updated AnalyzeValveCross and updated the structure of the Input Server
//
// 11    4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 11     4/07/05 5:29p Gswope
// Added methods AllInletValvesOff() and AllInletValvesOn().
//
// 10    3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 9     9/29/04 10:12p Cward
// Added stop pump motor command
//
// 8     8/18/04 11:55p Cward
// cleaned up includes and changed template class includes to cpp from h
//
// 7     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 6     5/25/04 3:35p Cward
// Typecasting to remove compiler warnings. Adding of setting
// Communication Failure in Brake Switch Test.  Changing of format in
// Analyze recovery forces
//
// 5     3/04/04 4:21p Cward
// Changed the conditional logic for skipping test steps
//
// 4     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 3     1/27/04 4:49p Cward
// Changes resulting from DE's testing and RW's trip
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 4     11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
//
// 3     11/13/03 6:08p Derickso
// Added sensor cross test.
//
// 2     10/29/03 6:28a Derickso
// Added new methods to Calibrate a G-sensor, dump pressure at
// all wheels, recover pressure at all wheels and exit
// roll test mode.
// Modified SensorTest() method to be a pure helper method.
// Calling functions are now responsible for getting the
// vehicle in the appropriate speed range.
// Added hold valves option to Reduction and Recovery methods.
// Added an optional sensor cross check during the valve
// firing seuqence.
//
// 1     10/27/03 5:05p Derickso
// Updated to handle both 3-channel and 4-channel ABS configurations.
// Added test step to determine the module configuration.
//
// 1     10/24/03 3:20p Canyanwu
// Start of DCX working directory
//    Revision 1.2  2003/10/06 15:27:41  tenharmsel
//    Updated from home/dcx
//
//
// 1     8/21/03 8:59a Derickso
// 05WK CCRT Project
//
// 1     8/14/03 7:46a Khykin
// Updated with new naming convention from TestComponent to TC.
//
// 2     8/08/03 10:40a Khykin
// Updated with changes from Flatrock.
// Revision 1.3  2003/05/15 14:04:26  tenharmsel
// Getting rid of Include directory
//
// Revision 1.1  2003/05/14 20:02:35  swieton
// Pit conditionals working
//
// Revision 1.1  2003/05/14 18:13:12  swieton
// Created common directory in AppLayer/TestComponents and moved appropriate code into it.
//
// Revision 1.1  2003/05/08 22:00:10  tenharmsel
// Collapsed directory tree into one
//
// Revision 1.1.1.1  2003/05/07 14:58:11  tenharmsel
// Bringing in the test component
//
// Revision 1.16  2003/04/21 17:23:59  tenharmsel
// Updated header comments to match BepCo standard for what information needs to go in header comments
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GENERICABSTCTEMPLATE_H
#define GENERICABSTCTEMPLATE_H
//==============================================================================

#include <string>
#include <vector>
#include "BaseBrakeTool.h"
#include "GenericTCTemplate.cpp"
#include "IMotorController.h"
//-------------------------------------------------------------------------

// Create a structure to contain reduction/recovery indices for each wheel
typedef struct _ABSReductionRecoveryIndex
{
    INT32 reductionStart;
    INT32 reductionEnd;
    INT32 recoveryStart;
    INT32 recoveryEnd;
} ABSReductionRecoveryIndex;

// Create a structure to hold module wheels speeds as well as roller speeds for sensor cross testing
typedef struct _SensorCrossCheckSpeeds
{
    float rollerSpeeds[MAXWHEELS];
    WheelSpeeds_t sensorSpeeds;
} SensorCrossCheckSpeeds;

// Create a map of wheel name to a standard index. i.e: LeftFront --> LFWHEEL(0)
typedef map<string, int> RollerIndex;
typedef RollerIndex::iterator           RollerIndexItr;
typedef RollerIndex::const_iterator     RollerIndexCItr;
typedef RollerIndex::value_type         RollerIndexValue;


/**
 * Generic ABS test component.<br>
 * This test component handles all test steps that are common
 * to most ABS tests.  The firing order of the valves should be specified in the
 * config file for the test component.  You can also specify if reduction should be performed prior to recovery
 * or vice versa.  This is intended to support both DaimlerChrysler style ABS testing and Ford style ABS testing.
 *
 * @author Dan Erickson
 * @see BaseBrakeTool
 * @see GenericTCTemplate
 * @see GenericTC
 */
template <class VehicleModuleType>
class GenericABSTCTemplate : public GenericTCTemplate<VehicleModuleType>
{
    friend class GenericABSTCTemplateTest;

public:
    /** Constructor */
    GenericABSTCTemplate();

    /** Destructor */
    virtual ~GenericABSTCTemplate();

    /**
     * Sequence a test step.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SwitchOn - Tag to use to indicate the brake switch should be checked for the "ON" position. </li>
     *      <li> SwitchOff - Tag to use to indicate the brake switch should be checked for the "OFF" position. </li>
     *      <li> BrakeActive - System tag used to activate and deactivate the brake force meter. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Test Step Functions Called:</b>
     *      <ul>
     *      <li> Setup() </li>
     *      <li> DetermineSystemConfiguration() </li>
     *      <li> CalibrateGSensor() </li>
     *      <li> CheckForGSensorCross() </li>
     *      <li> CheckForGSwitch() </li>
     *      <li> CheckFluidFillSwitch() </li>
     *      <li> ProgramTireSize() </li>
     *      <li> ResetModule() </li>
     *      <li> RunPumpMotor() </li>
     *      <li> StopPumpMotor() </li>
     *      <li> BrakeSwitchTest() </li>
     *      <li> HighSpeedSensorTest() </li>
     *      <li> LowSpeedSensorTest() </li>
     *      <li> DumpAndIsolateAllWheels() </li>
     *      <li> ValveFiringTest() </li>
     *      <li> RecoverAllWheels() </li>
     *      <li> StopRollTestMode() </li>
     *      <li> EvaluateABS() </li>
     *      <li> AnalyzeSensorCross() </li>
     *      <li> UpdateShipStatus() </li>
     *      <li> Reduction() </li>
     *      <li> Recovery() </li>
     *      <li> AllInletValvesOn() </li>
     *      <li> AllInletValvesOff() </li>
     *      <li> ExcessiveBrakeForceCheck() </li>
     *      <li> TestStepInitialize() </li>
     *      <li> TestStepAccelerate() </li>
     *      <li> TestStepCoast() </li>
     *      <li> TestStepDrag() </li>
     *      <li> TestStepAnalyzeDrag() </li>
     *      <li> TestStepBaseBrake() </li>
     *      <li> TestStepAnalyzeBaseBrake() </li>
     *      <li> TestStepBalance() </li>
     *      <li> TestStepBrakeToStop() </li>
     *      <li> CheckEvacAndFill() </li>
     *      <li> DisableDiagSpeedLimit() </li>
     *      <li> FinishUp() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetOverallResult() </li>
     *      <li> SystemWrite() </li>
     *      <li> UpdateTarget() </li>
     *      <li> SendOverallResult() </li>
     *      <li> GenericTCTemplate::CommandTestStep() </li>
     *      </ul>
     *
     * @param value    This is the value that was commanded by the test sequencer.
     * @return  The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

    /**
     * Determine the number of ABS channels.
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The int indicating the number of ABS channels.
     * <br><p>
     *
     * @return Flag indicating the number of ABS channels.
     * 		<ul>
     * 		<li> Int - Number of ABS channels</li>
     * 		</ul>
     */
    const int GetNumberOfChannels(void);

    /**
     * Return the number of driven wheels for the vehicle.
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Get the value indicating the number of driven wheels.
     * <br><p>
     *
     * @return  value indicating the number of driven wheels.
     */
    const int GetNumberOfDrivenWheels(void);

    /**
     * Check if the vehicle has a four channel ABS.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The flag indicating if the vehicle is equipped with a four channel ABS will be returned to the caller.
     * <br><p>
     *
     * @return Flag indicating if the vehicle is equipped with a four channel ABS.
     *      <ul>
     *      <li> True - Vehicle is equipped with 4-channel ABS. </li>
     *      <li> False - Vehicle is equipped with 3-channel ABS. </li>
     *      </ul>
     */
    const bool IsFourChannelSystem(void);

    /**
     * Check if the vehicle is four wheel drive equipped.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Get the flag indicating if the vehicle is four wheel dirve.
     * <br><p>
     *
     * @return  Flag indicating if the vehicle is equipped with four wheel drive.
     *      <ul>
     *      <li> True - Vehicle is four wheel drive. </li>
     *      <li> False - Vehicle is two wheel drive. </li>
     */
    const bool IsFourWheelDrive(void);

    /**
     * Deactivate the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Indicate that the component should de-activate itself. This should
     * happen at the end of a test when the component is no longer needed.
     * The base brake tool will also be commanded to deactivate.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::Deactivate() </li>
     *      <li> GenericTCTemplate::Deactivate() </li>
     *      </ul>
     */
    virtual void Deactivate(void);

protected:
    /**
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Perform any test component specific initialization.  The force thread will be launched during this initialization
     * phase.  The valve firing sequences for three and four channel systems will be stored at this time.  The build
     * data will also be checked to determine if the vehicle has a G-sensor that will need calibrating.
     * <br><p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ABS3ChannelFiringSequence - Firing order to use for 3-channel systems. </li>
     *      <li> ABS4ChannelFiringSequence - Firing order to use for 4-channel systems. </li>
     *      <li> Valid4x4GSensorTypes - List of 4x4 systems that have a G-sensor. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TransferCaseTestCode - System tag to use for retrieving the transfer case type in the build data. </li>
     *      <li> NoTransferCaseTestCode - Build data value indicating the vehicle is not equipped with a transfer case. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTCTemplate::InitializeHook() </li>
     *      <li> BaseBrakeTool::LaunchForceThread() </li>
     *      <li> GetRollerCount() </li>
     *      <li> RollerIndexValue() </li>
     *      <li> CheckForValidGSensorType() </li>
     *      <li> SetGSensorEquipped() </li>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @param config Configuration file to use for initialization.
     */
    virtual void InitializeHook(const XmlNode *config);

    /**
     * Reload the configuration data.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reloads the configuration file and resets the base components objects.  The valve firing orders will be cleared
     * as well as the list of valid 4x4 types.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTCTemplate::ReloadConfiguration() </li>
     *      </ul>
     */
    virtual void ReloadConfiguration(void);

    /**
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reinitialize using the already loaded configuration file. This is intended to be used when a component is 
     * activated and there have been no modifications to any configuration files used by the component.
     * The vehicle build data will be analyzed to determine if the ABS module has a g-sensor that must be calibrated.
     * The reduction/recovery start and end indices will also be reset to ensure we analyze the correct brake data.
     * <p>
     * <b>Data Tags:</b>
     *     <ul>
     *      <li> TransferCaseTestCode - System tag to use for retrieving the transfer case type in the build data. </li>
     *      <li> NoTransferCaseTestCode - Build data value indicating the vehicle is not equipped with a transfer case. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTCTemplate::WarmInitialize() </li>
     *      <li> BaseBrakeTool::WarmInitialize() </li>
     *      <li> CheckForValidGSensorType() </li>
     *      <li> SetGSensorEquipped() </li>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     */
    virtual void WarmInitialize(void);
    
    /**
     * Command the module to reset.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This method resets the module, ensuring that the module setup and clearing of faults is completed.
     * <p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AlwaysResetModule - Flag to determine if the module should always be reset. </li>
     *      <li> ModuleResetDelay - Amount of time to wait for the module to complete its reset sequence. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> PerformPowerOnReset() </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - Communication failure commanding the module to reset. </li>
     *      <li> SoftwareFailure - A module exception occurred while commanding the module to reset. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> BposSleep() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return The result of resetting the module.
     *      <ul>
     *      <li> Pass - The module was properly reset. </li>
     *      <li> Fail - An error occurred attempting to reset the module. </li>
     *      <li> Skip - The test step was not performed. </li>
     *      <li> SoftwareFail - A module exception was thrown attempting to reset the module. </li>
     *      </ul>
     */
    virtual string ResetModule(void);

    /**
     * Command the pump motor to run.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This method ensures that the hydraulic pump is operating and that brake fluid is available for testing.
     * A parameter can be set to ignore the status of commanding the module in the event it is not desired to know if
     * the command was unsuccessful.
     * <p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> RunPumpMotorSkipModuleValidation - Flag to indiccate if the module result should be verified for
     *                                              successful completion of the command. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> RunPumpMotor() </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - A communication failure occurred commanding the pump motor to run. </li>
     *      <li> SoftwareFailure -  A module exception occurred while attempting to command the module to run the pump motor </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameterBool() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return The result of commanding the pump motor to run.
     *      <ul>
     *      <li> Pass - The pump motor was commanded to run. </li>
     *      <li> Fail - An error occurred attempting to run the pump motor. </li>
     *      <li> Skip - The pump motor was not commanded to run. </li>
     *      <li> SoftwareFail - A module exception was thrown attempting to run the pump motor. </li>
     *      </ul>
     */
    virtual string RunPumpMotor(void);

    /**
     * Stop running the pump motor.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The module will be commanded to stop running the pump motor.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StopPumpMotor() </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - A communication failure occurred commanding the pump motor to stop running. </li>
     *      <li> SoftwareFailure -  A module exception occurred while attempting to command the module to stop running
     *                              the pump motor </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return The result of commanding the module to stop running the pump motor.
     *      <ul>
     *      <li> Pass - The module was commanded to stop running the pump motor. </li>
     *      <li> Fail - An error occurred commanding the pump motor to stop running. </li>
     *      <li> Skip - The pump motor was not commanded to stop running. </li>
     *      <li> SoftwareFail - An exception occurred commanding the pump motor to stop running. </li>
     *      </ul>
     */
    virtual string StopPumpMotor(void);

    /**
     * Determine the configuration of the vehicle.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The vehicle configuration will be read from the module.  The following data items will be extracted from the
     * module and used to determine if specific test steps should be performed or skipped:
     * <ul>
     * <li> Number of channels the ABS system has.  Typically Jeeps and trucks have 3 channel systems (LF, RF, Rear).
     *      Newer jeeps and trucks are migrating to 4 channel systems (LF, RF, LR, RR). </li>
     * <li> Four-wheel drive or Two-wheel drive. </li>
     * </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> FourChannelSystemCode - Mask used to determine if the vehicle is equipped with a four channel ABS. </li>
     *      <li> FourWheelDriveCode - Mask used to determine if the vehicle is equipped with 4 wheel drive. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read the vehicle configuration from the module. </li>
     *      <li> SoftwareFailure - A module exception occurred attempting to read the vehicle configuration from the module. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadModuleConfiguration </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SetFourChannelSystem() </li>
     *      <li> SetFourWheelDrive() </li>
     *      <li> IsFourChannelSystem() </li>
     *      <li> IsFourWheelDrive() </li>
     *      <li> CompareData() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     *
     * @return Result of reading the vehicle configuration.
     *      <ul>
     *      <li> Pass - The vehicle configuration was read from the module. </li>
     *      <li> Fail - The vehicle configuration could not be read from the module. </li>
     *      <li> Skip - The vehicle configuration did not need to be read from the module. </li>
     *      <li> SoftwareFail - A module exception occurred attempting to read the vehicle configuration from the module. </li>
     *      </ul>
     */
    virtual string DetermineSystemConfiguration(void);

    /**
     * Update the ship status in the module.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The overall brake test result will be written to the module.  The module will then either stop blinking the
     * ABS lamp on the cluster if the test passed, or keep blinking the ABS lamp if the test failed.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SendInitMessageBeforeUpdate - Flag to indicate if an initialization message should be sent to the module
     *                                         before writing the overall result to the module. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not update the result in the module. </li>
     *      <li> SoftwareFailure - A module exception occurred attempting to update the result in the module. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> CommandModule() - Message Tag: SendInitMessage[result], where [result] is the result to write to the module. </li>
     *      <li> UpdateRollsStatusByte() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return The result of updating the ship status.
     *      <ul>
     *      <li> Pass - The result was updated in the module. </li>
     *      <li> Fail - A communication failure occurred attempting to update the result in the module. </li>
     *      <li> Skip - The result does not need to be updated in the module. </li>
     *      <li> SoftwareFail - A module exception occurred attempting to update the result in the module. </li>
     *      </ul>
     */
    virtual string UpdateShipStatus(const string &result);

    /**
     * Check to ensure the G-sensor is not crossed with the wheel sensors.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The vehicle will be checked to ensure the G-Sensor is not crossed with the wheel speed sensors.  This test
     * <i><b>must</b></i> be performed at zero speed.  The machine will wait for the operator to achieve zero speed.
     * Once zero speed is verified, the wheel speed sensors will be read from the module.  If any of the wheel speed
     * sensors do not indicate zero speed, it has been crossed with the G-sensor.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadSensorSpeeds - Message tag to use to read wheel speeds from the module. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> GSensorCrossedWith[wheel] - G-Sensor is crossed with [wheel] wheel speed sensor. </li>
     *      <li> CommunicationFailure - Communication failure reading wheel speed sensors from the module. </li>
     *      <li> ZeroSpeedTimeout - Timeout waiting for the operator to bring the vehicle to zero speed. </li>
     *      <li> SoftwareFailure - A module excpetion occurred reading wheel speed sensors from the module. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadSensorSpeeds</li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetRollerCount() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of checking the vehicle for G-sensor crossed with wheel speed sensors.
     *      <ul>
     *      <li> Pass - The g-sensor is not crossed with any wheel speed sensor. </li>
     *      <li> Fail - The g-sensor is crossed with a wheel speed sensor or a communication failure occurred reading
     *                  the wheel speed sensors from the module. </li>
     *      <li> Skip - The g-sensor was not checked. </li>
     *      <li> SoftwareFail - A module excpetion occurred attempting to read the wheel speed sensors. </li>
     *      </ul>
     */
    virtual string CheckForGSensorCross(void);

    /**
     * Calibrate the G-sensor.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The G-sensor will be calibrated to ensure the ABS functions properly.  The calibration only needs to be performed
     * successfully one time.<br>
     * <i><b>NOTE:</b></i>  This step must be performed when the vehicle is at rest and zero speed.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CalibrateGSensorFail - A failure occurred calibrating the G-Sensor. </li>
     *      <li> SoftwareFailure - A module exception occurred while attempting to calibrate the g-sensor. </li>
     *      <li> ZeroSpeedTimeout - Timeout waiting for the operator to achieve zero speed. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> CalibrateGSensor() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> IsFourWheelDrive() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of calibrating the g-sensor.
     *      <ul>
     *      <li> Pass - The g-sensor was successfully calibrated. </li>
     *      <li> Fail - A failure occurred while attempting to calibrate the g-sensor. </li>
     *      <li> Skip - The g-sensor does not need to be calibrated. </li>
     *      <li> SoftwareFail - A module exception occurred attempting to calibrate the g-sensor. </li>
     *      </ul>
     */
    virtual string CalibrateGSensor(void);

    /**
     * Check the module for the presence of a G-switch.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The module will be checked to ensure it is equipped with a g-sensor.  The presence of a g-switch is critical to
     * the proper functionality of the ABS on 4wd vehicles.  If a 4wd vehicle is not equipped with a g-sensor, the ABS
     * will potentially not function under certain conditions.<br>
     * <i><b>Note:</b></i>  This is only applicable for vehicles equipped with 4-wheel drive.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> IsGSwitchPresent - Message tag used to read from the module if it is equipped with a g-sensor. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> GSwitchMissing - The g-switch was not found in the module. </li>
     *      <li> CommunicationFailure - The g-switch configuration could not be read from the module. </li>
     *      <li> ZeroSpeedTimeout - Timeout waiting for the operator to achieve zero speed. </li>
     *      <li> SoftwareFailure - A module exception occurred reading the g-switch configuration from the module. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: IsGSwitchPresent </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of checking the vehicle for the presence of a g-switch.
     *      <ul>
     *      <li> Pass - A g-switch was found in the module. </li>
     *      <li> Fail - A g-switch was not found in the module or the g-swtich configuration could not be read from the module. </li>
     *      <li> Skip - The g-switch configuration does not need to be verified. </li>
     *      <li> SoftwareFail - A module exception occurred verifying the presence of a g-switch. </li>
     *      </ul>
     */
    virtual string CheckForGSwitch(void);
    /**
     * Check to ensure the fluid fill switch is <i><b>not</b></i> on.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The fluid fill switch will be checked to ensure there is enough brake fluid in the system prior to testing.
     * If the fluid fill switch is on, more brake fluid must be added to the vehicle.  This check must be performed
     * with the vehicle at zero speed.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> FluidFillSwitchOn - The fluid fill switch is on, indicating there is not enough brake fluid in the system. </li>
     *      <li> CommunicationFailure - An error occurred reading the fluid fill switch status from the module. </li>
     *      <li> ZeroSpeedTimeout - Timeout waiting for the operator to achieve zero speed.
     *      <li> SoftwareFailure - A module exception occurred while attempting to read the fluid filol switch status. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: IsFluidFillSwitchOn </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of checking the fluid fill switch.
     *      <ul>
     *      <li> Pass - The fluid fill switch is off. </li>
     *      <li> Fail - The fluid fill switch is on or there was a communication failure reading the switch status. </li>
     *      <li> Skip - The fluuid fill switch does not need to be checked. </li>
     *      <li> SoftwareFail - A module exception occurred attempting to verify the status of the fluid fill switch. </li>
     *      </ul>
     */
    virtual string CheckFluidFillSwitch(void);

    /**
     * Program the tire size into the module.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This test step will first check the tire size already programmed in the module.  If the tire size from the build
     * data is different than the module, the build data tire size will be programmed into the module.  This test step
     * will be skipped if the tire size from the build data is zero.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TireSizeDataTag - System tag used for subscribing for the tire size from the vehicle build record. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> FailIfNoTireSize - Flag indicating if the test step should fail if no tire size was provided in the build data. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - A communication failure occurred programming the tire size into the module or
     *                                  reading the tire size from the module. </li>
     *      <li> SoftwareFailure - A module exception occurred attempting to read or write the tire size in the module. </li>
     *      <li> NoTireSize - No tire size was provided in the build data. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ReadCurrentTireSize() </li>
     *      <li> ProgramTireSize() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameterBool() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of programming the tire size.
     *      <ul>
     *      <li> Pass - The tire size in the module was correct or updated to the correct value. </li>
     *      <li> Fail - No tire size was provided in the build data or the tire size could not be programmed. </li>
     *      <li> Skip - The tire size does not need to be validated. </li>
     *      <li> SoftwareFail - A module exception occurred whlie validating the module tire size. </li>
     *      </ul>
     */
    virtual string ProgramTireSize(void);

    /**
     * Verify the brake switch in both positions (off and on).
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Verify the brake switch is observed in both the on and off positions.  The operator will be prompted to either
     * press the brake pedal or release the brake pedal depending on the initial state of the brake switch.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadBrakeSwitch - Message tag used to read the brake switch status from the module. </li>
     *      </ul>
     *
     * <b>Prompts:</b>
     *      <ul>
     *      <li> RemoveFootFromBrake - Remove foot from the brake pedal. </li>
     *      <li> DepressBrake - Press the brake pedal. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> ScanDelay - Time in milliseconds to wait between checks for brake switch status. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> TestTimeout - Timeout waiting to see brake switch in on and off positions. </li>
     *      <li> SystemStatus - System status is preventing further testing. </li>
     *      <li> BrakeSwitchStatesNotSeen - Brake switch not seen in on and off states. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadBrakeSwitch </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> SetStartTime() </li>
     *      <li> GetDataTag() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SetCommunicationFailure() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of verifying the brake switch in both positions.
     *      <ul>
     *      <li> Pass - The brake switch was observed in the on and off positions. </li>
     *      <li> Fail - Both brake switch positions were not observed. </li>
     *      <li> Timeout - Timeout waiting to observe the brake switch in both positions. </li>
     *      <li> Abort - System status is preventing further testing. </li>
     *      <li> Skip - The brake switch states do not need to be verified. </li>
     *      </ul>
     */
    virtual string BrakeSwitchTest(void);

    /**
     * Verify the brake switch is in the desired position.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Verify the brake switch is in the specified position.  This step looks for only one position and will wait for
     * a period of time for the operator to place the brake switch in the specified position.  The operator will not be
     * prompted to press or release the brake pedal, so this function will need to be sequenced at a time when the
     * brake pedal is expected to be in the specified position.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SwitchOn - Data tag used to specify the brake switch should be checked in the on position. </li>
     *      <li> ReadBrakeSwitch - Message tag used to read the brake switch status from the module. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> ScanDelay - Time in milliseconds to wait between checks for brake switch status. </li>
     *      </ul>
     *
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> BrakeSwitchTimeout - Timeout waiting for the brake switch to be in the specified position. </li>
     *      <li> SystemStatus - System status is preventing further testing. </li>
     *      <li> CommunicationFailure - Communication failure reading brake switch status from the module. </li>
     *      <li> SoftwareFailure - A module exception occurred attempting to read the brake switch status. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadBrakeSwitch </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetStartTime() </li>
     *      <li> BposSleep() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of verifying the brake switch in both positions.
     *      <ul>
     *      <li> Pass - The brake switch was observed in the specified position. </li>
     *      <li> Fail - The specified brake switch position was not observed, a communication failure occurred or
     *                  system status is preventing further testing. </li>
     *      <li> Timeout - Timeout waiting to observe the brake switch in the specified position. </li>
     *      <li> Skip - The brake switch state does not need to be verified. </li>
     *      <li> SoftwareFail - A module exception occurred attempting to verify the brake switch status. </li>
     *      </ul>
     */
    virtual string BrakeSwitchTest(const string& position);

    /**
     * Verify wheel speed sensors at low vehicle speed.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Verify the wheel speed sensors are functioning correctly at low vehicle speed.  The wheel speeds must be within
     * a given tolerance of the roller speeds.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SensorTest() </li>
     *      </ul>
     *
     * @return The result of verifying the wheel speed sensors at low speed.
     *      <ul>
     *      <li> Pass - All wheel speeds sensors are within tolerance. </li>
     *      <li> Fail - One or more wheel speed sensors are out of tolerance. </li>
     *      </ul>
     */
    virtual string LowSpeedSensorTest(void);

    /**
     * Verify wheel speed sensors at high vehicle speed.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Verify the wheel speed sensors are functioning correctly at high vehicle speed.  The wheel speeds must be within
     * a given tolerance of the roller speeds.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SensorTest() </li>
     *      </ul>
     *
     * @return The result of verifying the wheel speed sensors at high speed.
     *      <ul>
     *      <li> Pass - All wheel speeds sensors are within tolerance. </li>
     *      <li> Fail - One or more wheel speed sensors are out of tolerance. </li>
     *      </ul>
     */
    virtual string HighSpeedSensorTest(void);

    /**
     * Validate the wheel speed sensors.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The wheel speed sensors will be compared to the roller speeds to ensure each sensor is within the allowed
     * tolerance.  In order to ensure accurate comparisons if the vehicle is accelerating, the roller speeds will be
     * read from the system.  Next the wheel speed sensors will be read from the module.  The roller speeds will be read
     * again from the system.  The initial and final roller speeds will be averaged together and used for validating
     * the wheel speed sensors.
     * <br><p>
     *
     * <b>Prompts:</b>
     *      <ul>
     *      <li> MaintainSpeed - Prompt maitain speed. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> [TestStepName]TargetSpeed - Target speed to achieve before testing the wheel speed sensors. </li>
     *      <li> [TestStepName]TargetSpeedRange - Target speed range for the operator to achieve before testing the
     *                                            wheel speed sensors. </li>
     *      <li> [TestStepName]TargetSpeedScanDelay - Amount of time in milliseconds to wait between checks for target
     *                                                speed schieved. </li>
     *      <li> [TestStepName]SteadySpeedSamples - Number of wheel speeds that must be within a given range before
     *                                              steady vehicle speed can be confirmed. </li>
     *      <li> [TestStepName]SteadySpeedTimeout - Maximum time to wait for steady speed to be achieved. </li>
     *      <li> [TestStepName]MinSteadyWheelSpeed - Minimum wheel speed for steady speed determination. </li>
     *      <li> [TestStepName]MaxSteadyWheelSpeed - Maximum wheel speed for steady speed determination. </li>
     *      <li> [TestStepName]Tolerance - Tolerance the wheel speed sensors must fall within.  </li>
     *      <li> [TestStepName]WaitForStableSensors - Bool wait for sensors to be within tolerance.  </li>
     *      <li> [TestStepName]StableSensorSamples - Number of wheel speeds that must be within test tolerance before
     *                                               continuing vehicle sensor speed test. </li>
     *      <li> [TestStepName]StableSensorTimeout - Allowed time to check for stable sensor readings before continueing
     *                                               tolerance test.  </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> [Roller][TestStepName]SensorHigh - Wheel speed sensors are reading above allowed tolerance. </li>
     *      <li> [Roller][TestStepName]SensorLow - Wheel speed sensors are reading below allowed tolerance. </li>
     *      <li> [TestStepName]Fail - Wheel speed sensor test failed. </li>
     *      <li> [TestStepName]SystemCommFailure - Failure reading wheel speeds from the system. </li>
     *      <li> CommunicationFailure - Failed to read wheel speed sensors from the module. </li>
     *      <li> TargetSpeedTimeout - Operator failed to achieve the target speed range in the time allowed. </li>
     *      <li> SoftwareFailure - A module exception occurred reading wheel speed sensors. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadSensorSpeeds </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetRollerCount() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> AccelerateToTestSpeed() </li>
     *      <li> WaitForSteadySpeed() </li>
     *      <li> GetISpeeds() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      </ul>
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
     * Dump brake force and isolate all wheels.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The brake force will be dumped at all wheels and each wheel will be isolated from the others.
     * This will essentially put the vehicle into free-wheel mode with only drag forces acting on the vehicle.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DisableMeterForValveFiring - Flag to indicate if the force meter should be updated during the valve
     *                                        firing sequence.  The operator would notice the meter bouncing around if
     *                                        it is left enabled during valve firing. </li>
     *      <li> DumpAndHoldDelay - Amount of time to wait for brke force to dump at all wheels. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> DumpAllValvesFail - A failure occurred commanding the module to dump brake force at all wheels. </li>
     *      <li> HoldAllValvesFail - A failure occurred commanding module to hold all valves. </li>
     *      <li> SoftwareFailure - A module exception occurred while attempting to dump and hold all wheels. </li>
     *      </ul>
     *
     * <b>Module Interface Fcuntions:</b>
     *      <ul>
     *      <li> DumpValves() - Parameter: All </li>
     *      <li> HoldAllValves() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetParameterInt() </li>
     *      <li> DisableForceUpdates() </li>
     *      <li> EnableForceUpdates() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of dumping and isolating all wheels.
     *      <ul>
     *      <li> Pass - All brake pressure was dumped and all wheels isolated. </li>
     *      <li> Fail - Could not dump or hold all valves. </li>
     *      <li> Skip - The valves do not need to be dumped or held. </li>
     *      <li> SoftwareFail - A module excpetion occurred attempting to dump and hold all valves. </li>
     *      </ul>
     */
    virtual string DumpAndIsolateAllWheels(void);

    /**
     * Recover brake force at all wheels.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The module will be commanded to restore brake pressure at all wheels and the valves will return to normal
     * diagnostic mode operation.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DisableMeterForValveFiring - Flag to indicate if the force meter was disabled during the valve
     *                                        firing sequence.  If it was disabled, it will be re-enabled. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> RecoverAllValvesFail - A failure occurred commanding the module to recover brake force at all wheels. </b>
     *      <li> SoftwareFailure - A module excpetion occurred commanding the module to recover brake force at all wheels. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> RecoverAllValves() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> EnableForceUpdates() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of recovering brake pressure at all wheels.
     *      <ul>
     *      <li> Pass - Brake force was recovered at all wheels. </li>
     *      <li> Fail - A failure occurred commanding the module to recover brake force. </li>
     *      <li> Skip - Brake force does not need to be recovered. </li>
     *      <li> SoftwareFail - A module excpetion occurred commanding the module to recover brake force. </li>
     *      </ul>
     */
    virtual string RecoverAllWheels(void);

    /**
     * Stop roll test mode.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The mdoule will be commanded to exit the special roll test mode.  This essentially returns control to the ECU and
     * the module begins functioning as it should in a real world scenario.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> StopRollTestModeFail - A failure occurred commanding the module to exit roll test mode. </li>
     *      <li> SoftwareFailure - A module exception occurred commanding the module to exit roll test mode. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StopRollTestMode() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of commanding the module to exit roll test mode.
     *      <ul>
     *      <li> Pass - The module exited roll test mode. </li>
     *      <li> Fail - A failure occurred commanding the module to exit roll test mode. </li>
     *      <li> Skip - The module does not need to be commanded to exit roll test mode. </li>
     *      <li> SoftwareFail - A module exception occurred commanding the module to exit roll test mode. </li>
     *      </ul>
     */
    virtual string StopRollTestMode(void);

    /**
     * Perform the valve firing sequence for all wheels.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The valves at each wheel will be fired in order to collect data for the reduction/recovery performance of the
     * ABS.  Data can optionally be collected during the valve firing to determine if any wheel speed sensors are crossed
     * or if any brake valves are crossed.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DeveloperTest - Flag indicating if this is a special development test sequence. </li>
     *      <li> DisableMeterForValveFiring - Flag to indicate if the force meter should be updated during the valve
     *                                        firing sequence.  The operator would notice the meter bouncing around if
     *                                        it is left enabled during valve firing. </li>
     *      <li> PerformReductionBeforeRecovery - Flag indicating what type of valve firing sequence to perform.  If
     *                                            brake force is dumped prior to the valve firing, you would need to
     *                                            perform the recovery first.  Otherwise, you would need to dump the
     *                                            current brake force prior to trying to recover the brake force. </li>
     *      <li> CollectSensorSpeedData - Flag used to indicate if wheel speed sensor data should be collected for later
     *                                    analysis of wheel speed sensors crossed. </li>
     *      <li> BrakeForceStabilizeTime - Time to wait in milliseconds for brake force to stabilize prior to firing valves. </li>
     *      <li> InterWheelGapDelay - Time in milliseconds to wait before firing the next valve in the sequence. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> ReduxRecovSequenceFail - A failure occurred during the valve firing sequence. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadSensorSpeeds </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetRollerCount() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetBrakeTestingStatus() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetParameterInt() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> DisableForceUpdates() </li>
     *      <li> EnableForceUpdates() </li>
     *      <li> TagArray() - Tag: ABSStart/ABSEnd</li>
     *      <li> IsFourChannelSystem() </li>
     *      <li> BposSleep() </li>
     *      <li> GetISpeeds() </li>
     *      <li> Reduction() </li>
     *      <li> Recovery() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of the valve firing test.
     *      <ul>
     *      <li> Pass - The valves were fired successfully. </li>
     *      <li> Fail - An error occurred firing the valves. </li>
     *      <li> Skip - The vlaves do not need to be fired. </li>
     *      </ul>
     */
    virtual string ValveFiringTest(void);

    virtual string SendKeepAliveMessage(void);
    /**
     * Evaluate the ABS data.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The force data will be evaluated to determine if the reductions and recoveries are sufficient for each wheel.
     * The valve cross analysis will optionally be performed at this time as well.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DeveloperTest - Flag indicating if this is a special development test sequence. </li>
     *      <li> IcmForceArray - Name of the ICM force array to use for retrieving wheel forces. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> ABSFailure - ABS reduction or recovery failed at one or more wheels.
     *      <li> WheelForceDataReadError - Could not read wheel force arrays. </li>
     *      <li> NoABSData - No samples were recorded during the reduction and recovery sequences. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetRollerCount() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetBrakeTestingStatus() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterBool() </li>
     *      <li> ReadDataForWheelForceArray() </li>
     *      <li> AnalyzeReductionForces() </li>
     *      <li> AnalyzeRecoveryForces() </li>
     *      <li> AnalyzeValveCross() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of evaluating ABS reduction, recovery and valve cross.
     *      <ul>
     *      <li> Pass - Reduction, recovery and valve cross passed at all wheels. </li>
     *      <li> Fail - Reduction or recovery failed at one or more wheels or the valve cross analysis failed. </li>
     *      <li> Skip - ABS data does not need to be analyzed. </li>
     *      </ul>
     */
    virtual string EvaluateABS(void);

    /**
     * Analyze the wheel speed data to determine if the wheel speed sensors on the vehicle are crossed.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The wheel speed data will be analyzed to determine if any of the sensors are crossed.  Typically the sensor data
     * is collected during the valve firing sequence.  According to this approach, the wheel being fired should have the
     * greatest speed delta.  All other wheels will be checked to ensure this is the case.  If the wheel being fired
     * does not have the greatest speed delta, then the sensors are crossed.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> [xx]SensorCrossedWith[yy] - Wheel sensor [xx] is crossed with wheel sensor [yy]. </li>
     *      <li> SensorCrossCheckFail - One or more wheel speed sensors have been crossed. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetRollerCount() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> IsFourChannelSystem() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of analyzing sensor cross data.
     *      <ul>
     *      <li> Pass - No wheel speed sensors are crossed. </li>
     *      <li> Fail - One or more wheel speed sensors are crossed. </li>
     *      <li> Skip - Sensor cross evaluation was not performed. </li>
     *      </ul>
     */
    virtual string AnalyzeSensorCross(void);

    /**
     * Perform a reduction sequence at the specified wheel.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The module will be commanded to reduce brake pressure at the specified wheel.  After the module is commanded to
     * reduce pressure, the software will wait for a preset amount of time.  The module can also be optionally commanded
     * to hold the reduction at the specified wheel in order to keep the brake pressure off the wheel.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> [wheel]ReductionPulseLength - Length of time in milliseconds for module to reduce brake pressure at
     *                                         the specified wheel. </li>
     *      <li> HoldReduction - Flag to indicate if the module should be commanded to hold the brake reduction. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - Module could not be commanded to reduce brake pressure. </li>
     *      <li> SoftwareFailure - A module exception occurred commanding the module to reduce brake rpessure. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ActuateReduction() </li>
     *      <li> HoldAllValves() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TagArray() - Tag: [wheel]ReduxStart/ReduxStop </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @param wheel      Wheel to perform reduction on.
     * @param startIndex Reduction start index for the specified wheel.
     * @param stopIndex  Reduction stop index for the specified wheel.
     *
     * @return Result of commanding the module to reduce brake pressure at the specified wheel.
     *      <ul>
     *      <li> Pass - The module was successfully commanded to reduce brake pressure at the specified wheel. </li>
     *      <li> Fail - A communication failure occurred commanding the module to reduce brake pressure. </li>
     *      <li> SoftwareFail - A module exception ocurred commanding the module to reduce brake pressure. </li>
     *      </ul>
     */
    virtual string Reduction(const string &wheel, INT32 &startIndex, INT32 &stopIndex);

    /**
     * Perform the recovery sequence at the specified wheel.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The module will be commanded to recover brake force at the specified wheel.  The brake force will be recovered for
     * an amount of time specified by parameters.  Optionally, the module can be commanded to hold the valves after the
     * recovery command has been issued.  The force array will also be tagged with the recovery start and end times.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> [wheel]RecoveryPulseLength - Length of time in milliseconds for module to recover brake pressure at
     *                                        the specified wheel. </li>
     *      <li> HoldRecovery - Flag to indicate if the module should be commanded to hold the brake recovery. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - Module could not be commanded to recover brake pressure. </li>
     *      <li> SoftwareFailure - A module exception occurred commanding the module to recover brake pressure. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ActuateRecovery() </li>
     *      <li> HoldAllValves() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TagArray() - Tag: [wheel]RecovStart/RecovStop </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @param wheel      Which wheel to recover.
     * @param startIndex Recovery start index.
     * @param stopIndex  Recovery end index.
     *
     * @return Result of recovering brake force at the specified wheel.
     *      <ul>
     *      <li> Pass - The module was successfully commanded to recover brake force. </li>
     *      <li> Fail - A communication failure occurred commanding the module to recover brake force. </li>
     *      <li> SoftwareFail - A module exception occurred commanding the module to recver brake force. </li>
     *      </ul>
     */
    virtual string Recovery(const string &wheel, INT32 &startIndex, INT32&stopIndex);

    /**
     * Open all ABS inlet valves.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Command the module to open all ABS inlet valves.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - Failed to command the module to open all inlet valves. </li>
     *      <li> SoftwareFailure - A module exception occurred command the module to open all inlet valves. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> AllInletValvesOn() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of commanding the module to open all inlet valves.
     *      <ul>
     *      <li> Pass - The module was successfully commanded to open all inlet valves. </li>
     *      <li> Fail - A communication failrue occurred commanding the module to open all inlet valves. </li>
     *      <li> Skip - The valves do not need to be opened.
     *      <li> SoftwareFail - The module could not be commanded to open all inlet valves. </li>
     *      </ul>
     */
    virtual string AllInletValvesOn(void);
    /**
     * Close all ABS inlet valves.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Command the module to close all ABS inlet valves.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - Failed to command the module to close all inlet valves. </li>
     *      <li> SoftwareFailure - A module exception occurred command the module to close all inlet valves. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> AllInletValvesOff() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of commanding the module to close all inlet valves.
     *      <ul>
     *      <li> Pass - The module was successfully commanded to close all inlet valves. </li>
     *      <li> Fail - A communication failrue occurred commanding the module to close all inlet valves. </li>
     *      <li> Skip - The valves do not need to be closed.
     *      <li> SoftwareFail - The module could not be commanded to close all inlet valves. </li>
     *      </ul>
     */
    virtual string AllInletValvesOff(void);
    /**
     * Analyze the reduction force for the specified wheel.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The wheel force array for the specified wheel will be retrieved.  The minimum data point betweent the start and
     * end indices will be located and compared against the minimum reduction allowed.  If the value falls below the
     * minimum reduction allowed, the test passes.  The results will be written to the system for display to the operator
     * as well.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> [wheel]ABSReductionValue - System tag to write the reduction value for [wheel]. </li>
     *      <li> [wheel]ABSReductionBGColor - System tag for setting the background color for the reduction value for [wheel]. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> [wheel]MinReductionPercent - Minimum reduction percent required for [wheel]. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> [wheel]ReductionFail - Reduction percentage is not low enough for [wheel]. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetWheelForceArray() </li>
     *      <li> GetDragForceValue() </li>
     *      <li> GetBrakeForceValue() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> CreateMessage() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      </ul>
     *
     * @param roller The roller to analyze.
     * @param start  The start index for the reduction pulse.
     * @param end    The end index for the reduction pulse.
     * @return The status of checking the reduction force.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Reduction precentage was below the minimum required. </li>
     *      <li> BEP_STATUS_FAILURE - Reduction percentage was above the minimum required. </li>
     *      <li> BEP_STATUS_SKIP - Reduction forces were not sampled.
     *      </ul>
     */
    virtual BEP_STATUS_TYPE AnalyzeReductionForces(INT32 roller, INT32 start, INT32 end);

    /**
     * Analyze the recovery force for the specified wheel.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The wheel force array for the specified wheel will be retrieved.  The maximum data point betweent the start and
     * end indices will be located and compared against the minimum recovery required.  If the value falls above the
     * minimum recovery required, the test passes.  The results will be written to the system for display to the operator
     * as well.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> [wheel]ABSRecoveryValue - System tag to write the recovery value for [wheel]. </li>
     *      <li> [wheel]ABSRecoveryBGColor - System tag for setting the background color for the recovery value for [wheel]. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> [wheel]MinRecoveryPercent - Minimum recovery percent required for [wheel]. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> [wheel]RecoveryFail - Recovery percentage is not high enough for [wheel]. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetWheelForceArray() </li>
     *      <li> GetBrakeForceValue() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> CreateMessage() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      </ul>
     *
     * @param roller The roller to analyze.
     * @param start  The start index for the recovery pulse.
     * @param end    The end index for the recovery pulse.
     * @return The status of checking the recovery force.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Recovery precentage was below the maximum required. </li>
     *      <li> BEP_STATUS_FAILURE - Recovery percentage was above the maximum required. </li>
     *      <li> BEP_STATUS_SKIP - Recovery forces were not sampled.
     *      </ul>
     */
    virtual BEP_STATUS_TYPE AnalyzeRecoveryForces(INT32 roller, INT32 start, INT32 end);

    /**
     * Return the reference brake force to use for calculating ABS recovery and reduction percentages.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Returns the reference brake force to use for calculating ABS recovery and reduction percentages.
     * <br><p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetBrakeForceValue() </li>
     *      </ul>
     *
     * @param roller The roller to return the reference brake force for
     *
     * @return The reference brake force to use for calculating ABS recovery and reduction percentages.
     */
    virtual float GetAbsReferenceForce( INT32 roller);

    /**
     * Analyze the brake force test data to determine if any of the valves are crossed.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The wheel force arrays will be checked to ensure that the wheel be acted on matches the overal data.
     * For example, if all wheels are isolated, and a recovery pulse is sent, this wheel should have the highest
     * brake force.  If not, the valve is crossed with the wheel that does indicate the highest brake force at that
     * time.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SkipValveCrossAnalysis - Flag indicating if the valve cross analysiss should be skipped. </li>
     *      <li> PerformReductionBeforeRecovery - Flag to indicate if the the reduction was performed prior to the recovery. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li>[wheel1]ValveCrossedWith[wheel2] - ABS valve for [wheel1] is crossed with valve for [wheel2]. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterBool() </li>
     *      <li> GetRollerCount() </li>
     *      <li> GetWheelForceValue() </li>
     *      <li> IsFourChannelSystem() </li>
     *      <li> CompareData() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetComponentName() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      </ul>
     *
     * @return The status of analyzing the valve cross.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - No valves were found to be crossed. </li>
     *      <li> BEP_STATUS_FAILURE - One or more valves were found to be crossed. </li>
     *      <li> BEP_STATUS_SKIP - The valve cross analysis was skipped. </li>
     *      </ul>
     */
    virtual BEP_STATUS_TYPE AnalyzeValveCross(void);

    /**
     * Wait for the vehicle speed to be steady.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The roller speeds will be monitored to determine if the vehicle is at a steady speed.  Steady speed is defined
     * as roller speeds between the given parameters for the given number of samples.  If the wheel speed is not in this
     * range for the given number of samples, a timeout will occur.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time to wait betwen consecutive wheel speed checks. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetWheelSpeeds() </li>
     *      <li> BposSleep() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     *
     * @param speedSamples  The number of consecutive wheel speeds that must be within range
     *                      to determine steady speed.
     * @param timeout  Amount of time to wait for steady speed.
     * @param minWheelSpeed  Minimum wheel speed allowed
     * @param maxWheelSpeed  Maximum wheel speed allowed
     */
     virtual void WaitForSteadySpeed(const INT32 &speedSamples, const INT32 &timeout,
                                     const float &minWheelSpeed, const float &maxWheelSpeed);
     virtual void WaitForSingleAxleSteadySpeed(const INT32 &speedSamples, const INT32 &timeout,
                                     const float &minWheelSpeed, const float &maxWheelSpeed, const string axel);


    /**
      * Wait for the ABS Sensor speeds to be within allowed deviation range from roller speeds.
      * <p><b>Category:</b> Utility
      * <br><p>
      * <b>Description:</b><br>
      * Check the ABS sensors against the roller speeds to determine if the ABS sensors are within allowed
      * deviation range. If the sensor speeds are not in range for the given number of samples, a timeout
      * will occur. This is to help account for a time lag in ABS module readings.
      * <br><p>
      *
      * <b>Test Step Info:</b>
      *         <ul>
      *         <li> ScanDelay - Amount of time to wait between consecutive wheel speed checks. </li>
      *         </ul>
      *
      * <b>Functions Called:</b>
      *         <ul>
      *         <li> GetRollerCount() </li>
      *         <li> Log() </li>
      *         <li> GetISpeeds() </li>
      *         <li> GetInfo() </li>
      *         <li> BposSleep() </li>
      *         <li> GetTestStepInfoInt() </li>
      *         <li> StatusCheck() </li>
      *         <li> SetSecondaryStartTime() </li>
      *         <li> SecondaryTimeRemaining() </li>
      *         </ul>
      *
      * @param speedSamples  The number of consecutive senor speeds that must be within range.
      * @param timeout  Amount of time to wait for good senor speeds.
      * @param tolerance  Allowed devaition for good senor speeds.
      */
     virtual void WaitForStableSensorSpeeds(const INT32 &speedSamples, const INT32 &timeout, const float &tolerance);

    /**
     * Check the current status of the machine and the component.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The status of the machine and the component will be checked to determine if the current conditions are valid for
     * continuing a test.  A parameter will be checked to determine if this is a developer test sequence.  If this is a
     * developer test sequence, BEP_STATUS_SUCCESS will be returned; otherwise, the actual status wille be returned.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DeveloperTest - Flag indicating if this is a special developer test sequence. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterBool() </li>
     *      <li> GenericTCTemplate::StatusCheck() </li>
     *      </ul>
     *
     * @return The status of the system for testing.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The status is good to continue testing. </li>
     *      <li> BEP_STATUS_ERROR - Testing cannot continue due to system status. </li>
     *      <li> BEP_STATUS_ABORT - The system abort flag has been set. </li>
     *      </ul>
     */
    virtual INT32 StatusCheck(void);

    /**
     * Command the module to disable the diagnostic speed limit.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The ABS module will be commanded to disable the diagnostic speed limit.  This will ensure that the ABS module
     * will not set certain faults in the module during the driving portion of the test.
     * <br><p>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> CommunicationFailure - A communication failure occurred commanding the module to disable the diagnostic
     *                                  speed limit. </li>
     *      <li> SoftwareFailure - A module exception occurred commanding the module to disable the diagnostic speed limit. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> DisableDiagnosticSpeedLimit() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return The result of commanding the module to disable the diagnostic speed limit.
     *      <ul>
     *      <li> Pass - The module was successfully commanded to disable the diagnostic speed limit. </li>
     *      <li> Fail - A communication failure occurred commanding the module to disable the diagnostic speed limt. </li>
     *      <li> SoftwareFail - A module exception occurred commanding the module to disable the diagnostic speed limit. </li>
     *      </ul>
     */
    virtual string DisableDiagSpeedLimit(void);
    /**
     * Check the Evac and Fill status of the vehicle.
     * <p><b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Verify the brake system evac and fill procedure was completed prior to the vehicle arriving for roll testing.
     * For development testing and pilot builds, the evac and fill bit in the module can be forced as complete.
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> UpdateEvacAndFill - Flag to indicate if the evac and fill result should be updated in the module. </li>
     *      </ul>
     *
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> EvacAndFillIncomplete - The evac and fill procedure was not performed on the vehicle. </li>
     *      <li> CommunicationFailure - Could not read evac and fill status from the module. </li>
     *      <li> SoftwareFailure - A module exception occurred attmepting to read or update the evac and fill status. </li>
     *      </ul>
     *
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: IsEvacAndFillComplete </li>
     *      <li> UpdateEvacAndFillResult() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameterBool() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of checking the evac and fill bit.
     *      <ul>
     *      <li> Pass - The evac and fill procedure has been completed. </li>
     *      <li> Fail - The evac and fill procedure has not been completed or the information could not be read. </li>
     *      <li> Skip - The evac and fill status does not need to be verified. </li>
     *      <li> SoftwareFail - A module exception occurred attempting to verify the evac and fill status. </li>
     *      </ul>
     */
    virtual string CheckEvacAndFill(void);
    /**
     * <b>Description:</b>
     * Method used to abort the current processing based on the needs of the 
     * component.
     */
    virtual void Abort(void);
    /**
	 * Store the value indicating the number of ABS channels.
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The value indicating if the number of ABS channels.  This information is used
	 * to determine which firing sequence to use when performing the valve firing sequence.
	 * <br><p>
	 *
	 * @param isChannelABS  value indicating the number of ABS channels.
	 * 		<ul>
	 * 		<li> 3- Vehicel is equipped with 3-channel ABS. </li>
	 * 		<li> 4- Vehicle is equipped with 4-channel ABS. </li>
     * 		<li> 6- Vehicle is equipped with 6-channel ABS. </li>
     * 		</ul>
	 */
	void SetNumberOfChannels(const int numberOfChannels);

	/**
	 * Store the value indicating the number of driven wheels.
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The value indicating the number of driven wheels.  This information is used
	 * to determine if certain test steps should be performed.
	 * <br><p>
	 *
	 * @param isFourWheelDrive  Flag indicating if the vehicle is four wheel drive.
	 * 		<ul>
	 * 		<li> 2 - Vehicle is 2 wheel drive. </li>
	 * 		<li> 4 - Vehicle is 4 wheel drive. </li>
  	 * 		<li> 6 - Vehicle is 6 wheel drive. </li>
     * 		</ul>
	 */
	void SetNumberOfDrivenWheels(const int numberOfDrivenWheels);

    /**
     * Store the flag indicating if the vehicle is equipped with a four-channel system.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The flag indicating if the vehicle is equipped with a four channel system will be stored.  This information is used
     * to determine which firing sequence to use when performing the valve firing sequence.
     * <br><p>
     *
     * @param isFourChannelABS  Flag indicating if the vehicle is equipped with a four-channel ABS.
     *      <ul>
     *      <li> True - Vehicle is equipped with 4-channel ABS. </li>
     *      <li> False - Vehicle is equipped with 3-channel ABS. </li>
     *      </ul>
     */
    void SetFourChannelSystem(const bool &isFourChannelABS);

    /**
     * Store the flag indicating if the vehicle is four wheel drive.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The flag indicating if the vehicle is equipped with four wheel drive will be stored.  This information is used
     * to determine if certain test steps should be performed.
     * <br><p>
     *
     * @param isFourWheelDrive  Flag indicating if the vehicle is four wheel drive.
     *      <ul>
     *      <li> True - Vehicle is four wheel drive. </li>
     *      <li> False - Vehicle is two wheel drive. </li>
     *      </ul>
     */
    void SetFourWheelDrive(const bool &isFourWheelDrive);

    /**
     * Get the force delta for the specified wheel.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The method will get the max force delta for each wheel from the start to the end of the period passed to the parameters.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetWheelForceArray() </li>
     *      </ul>
     *
     * @param recoveryStart
     *               The starting index for determining the force delta for each
     *               wheel
     * @param recoveryEnd
     *               The ending index for determining the force delta for each
     *               wheel
     * @param innerValveForceData
     *               the array passed in that is used to return the force delta
     *               for each wheel.  The order the data is returned in
     *               corresponds to the BEP wheel order standard
     */
    void GetValveCrossForceDelta(INT32 recoveryStart,INT32 recoveryEnd,double *innerValveForceData);

    /**
     * CheckForValidGSensorType method.<br>
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Check parameter list to see if the broadcast Tcase type matches list.
     *
     * <br><p>
     *
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> begin() </li>
     *      <li> end() </li>
     *      <li> getValue() </li>
     *      <li> compare() </li>
     *      <li> GetDataTag() </li>
     *      <li> SystemRead() </li>
     *      <li> SetGSensorEquipped() </li>
     *      </ul>
     *
     * @return Bool equipped with G-Sensor.
     *      <ul>
     *      <li> True: equipped with G-Sensor. </li>
     *      <li> False: does not have G-Sensor. </li>
     *      </ul>
     */
    void CheckForValidGSensorType(void);

    /**
     * SetGSensorEquipped method.<br>
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Set the flag indicating if the vehicle has 4x4 G-Sensor.
     * <p>
     */
    virtual void SetGSensorEquipped(const bool &equipped);

    /**
     * IsGSensorEquipped method.<br>
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Get the flag indicating if vehicle has 4x4 G-Sensor.
     * <p>
     * @return Flag indicating if the vehicle is equipped with a G-sensor.
     *      <ul>
     *      <li> True: equipped with G-Sensor. </li>
     *      <li> False: does not have G-Sensor. </li>
     *      </ul>
     */
    virtual const bool &IsGSensorEquipped(void);

    /**
     * SetESPEquipped method.<br>
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Set the flag indicating if the vehicle is equipped with ESP.
     * <p>
     */
    void SetESPEquipped(const bool &isESPEquipped);

    /**
     * IsESPEquipped method.<br>
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Get the flag indicating if vehicle is equipped with ESP.
     * <p>
     * @return Flag indicating if the vehicle is equipped with ESP.
     *      <ul>
     *      <li> True: equipped with ESP. </li>
     *      <li> False: does not have ESP. </li>
     *      </ul>
     */
    virtual const bool &IsESPEquipped(void);

    /**
	 * Value to indicate the number of ABS channels.
	 * 		<ul>
	 * 		<li> 6 - Six-channel ABS. </li>
	 * 		<li> 4 - Four-channel ABS. </li>
	 * 		<li> 3 - Three-channel ABS. </li>
	 * 		</ul>
	 */
	short  m_numberOfChannels;

	/**
	 * Value to indicate the number of driven wheels.
	 * 		<ul>
     * 	    <li> 6 - Six-wheel drive. </li>
	 * 		<li> 4 - Four-wheel drive. </li>
	 * 		<li> 2 - Two-wheel drive. </li>
	 * 		</ul>
	 */
	short  m_numberOfDrivenWheels;

    /**
     * Flag to indicate if the ABS is a four-channel system.
     *      <ul>
     *      <li> True - Four-channel ABS. </li>
     *      <li> False - Three-channel ABS. </li>
     *      </ul>
     */
    bool  m_isFourChannelSystem;
    /**
     * Flag to indicate if the vehicle is four wheel drive.
     *      <ul>
     *      <li> True - Four-wheel drive. </li>
     *      <li> False - Two-wheel drive. </li>
     *      </ul>
     */
    bool  m_isFourWheelDrive;
    /** Index of when the ABS reduction/recovery sequence began. */
    INT32 m_absStartIndex;

    /** Index of when the ABS reduction/recovery sequence ended. */
    INT32 m_absEndIndex;

    /**
     * The Base Brake utility object that contains most of the funtionality
     * for base brake testing.
     */
    BaseBrakeTool *m_baseBrakeTool;

    /** Firing order for the 3-channel ABS test. */
    XmlNodeMap m_abs3ChannelFiringOrder;

    /** Firing order for the 4-channel ABS test. */
    XmlNodeMap m_abs4ChannelFiringOrder;

    /** Firing order for the 6-channel ABS test. */
    XmlNodeMap m_abs6ChannelFiringOrder;

    /** Reduction and recovery start and end indices for each wheel. */
    std::vector<ABSReductionRecoveryIndex>  m_reduxRecovIndex;

    /**
     * Roller name translation table.
     * For example, LeftFront maps to 0 (LFWHEEL).
     */
    RollerIndex m_rollerIndex;

    /** Wheel speed data to check for wheel sensor cross. */
    SensorCrossCheckSpeeds m_sensorCrossCheckData[MAXWHEELS+2];

    /**
     * Flag indicating if the vehicle is equipped with 4 wheel drive G-Sensor.
     */
    bool m_isGSensorEquipped;

    /** Flag indicating if the vehicle is equipped with ESP. */
    bool m_isESPEquipped;

    /** Store the the valid 4 wheel drive G-Sensor Tcase types for this test from configuration file */
    XmlNodeMap m_validGSensorTypes;
    /** Store the ABS stat data for the ABS testing */
    vector<BrakeDataStats> m_absStats;
	/** Result of Evaluate ABS test.  This is used to determine if tests such as Sensor Cross analysis should be performed. */
	string m_evalAbsResult;
};

#endif  // GENERICABSTCTEMPLATE_H
