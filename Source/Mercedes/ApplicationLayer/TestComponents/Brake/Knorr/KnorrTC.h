//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/TestComponents/Brake/Include/KnorrTC.h 2 
//
// FILE DESCRIPTION:
//		ABS test component for the Teves Mk20E ABS system.
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
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Brake/Include/KnorrC.h $
// 
//
//*************************************************************************
#ifndef KnorrTC_h
#define KnorrTC_h
//-------------------------------------------------------------------------
#include "GenericABSTCTemplate.cpp"
#include "ModuleException.h"

/**
 * Test component for the Knorr ABS.
 * This test component provides the specialized test steps
 * that are specific to the KnorrABS.
 */
template <class ModuleType>
class KnorrTC : public GenericABSTCTemplate<ModuleType>
{
public:
	/** 
	 * Constructor
	 */
	KnorrTC();
	/**
	 * Destructor
	 */
	virtual ~KnorrTC();
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
	 * @since Version 1.0
	 */
	virtual const string CommandTestStep(const string &value);

protected:
	/**
	 * Apply torque to rollers specified
	 * 
	 * @return The status of the torque commands
	 */                  
	std::string ApplyTorqueToRollers(void);
	/**
	 * Semi-static ABS test
	 * 	Commands all rollers to a torque, then fires
	 *  valves and monitors the wheels turning
	 * 
	 *  @return The status of the ABS test
	 */
	const std::string TestStepStaticAbs(void);
	/**
	 * set all rolls to zero torque
	 */
	std::string SetRollsToZeroTorque(void);
	/**
	 * Send timing signals to wake up communication
	 * 
	 * @return The status of the wake up call
	 */
	string ModuleWakeUp(void);
	/**
	 * Send a command to disable INS communication
	 * 
	 * @return the status of the command
	 */
	string DisableInsCommunication(void);
	/**
	 * Initialize the test component.
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * Perform any test component specific initialization.  The force thread will be launched during this initialization
	 * phase.  The valve firing sequences for three and four channel systems will be stored at this time.
	 * <br><p>
	 *
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GenericTCTemplate::InitializeHook() </li>
	 * 		<li> LaunchForceThread() </li>
	 * 		<li> GetRollerCount() </li>
	 * 		<li> RollerIndexValue() </li>
	 * 		</ul>
	 * 
	 * @param config Configuration file to use for initialization.
	 */
	virtual void InitializeHook(const XmlNode *config);
	/**
	 * Dump brake force and isolate all wheels.
	 * This will essentially put the vehicle into free-wheel
	 * mode with only drag forces acting on the vehicle.
	 * 
	 * @return Result of the test step.
	 * @since Version 1.0
	 */
	virtual string DumpAndIsolateAllWheels(void);
	/**
	 * Validate the wheel speed sensors.
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The wheel speed sensors will be compared to the roller speeds to ensure each sensor is within the allowed
	 * tolerance.  In order to ensure accurate comparisons if the vehicle is accelerating, the roller speeds will be
	 * read from the system.  Next the wheel speed sensors will be read from the module.  The roller speeds will be read
	 * again from the system.  The initial and final roller speeds will be averaged together and used for validating 
	 * the wheel speed sensors.
	 * <br><p>
	 *
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> [TestStepName]TargetSpeed - Target speed to achieve before testing the wheel speed sensors. </li>
	 * 		<li> [TestStepName]TargetSpeedRange - Target speed range for the operator to achieve before testing the
	 * 											  wheel speed sensors. </li>
	 * 		<li> [TestStepName]TargetSpeedScanDelay - Amount of time in milliseconds to wait between checks for target
	 * 												  speed schieved. </li>
	 * 		<li> [TestStepName]SteadySpeedSamples - Number of wheel speeds that must be within a given range before
	 * 												steady vehicle speed can be confirmed. </li>
	 * 		<li> [TestStepName]SteadySpeedTimeout - Maximum time to wait for steady speed to be achieved. </li>
	 * 		<li> [TestStepName]MinSteadyWheelSpeed - Minimum wheel speed for steady speed determination. </li>
	 * 		<li> [TestStepName]MaxSteadyWheelSpeed - Maximum wheel speed for steady speed determination. </li>
	 * 		<li> [TestStepName]Tolerance - Tolerance the wheel speed sensors must fall within.  </li>
	 * 		</ul>
	 * 
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Description of the test step. </li>
	 * 		</ul>
	 * 	
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> [Roller][TestStepName]SensorHigh - Wheel speed sensors are reading above allowed tolerance. </li>
	 * 		<li> [Roller][TestStepName]SensorLow - Wheel speed sensors are reading below allowed tolerance. </li>
	 * 		<li> [TestStepName]Fail - Wheel speed sensor test failed. </li>
	 * 		<li> [TestStepName]SystemCommFailure - Failure reading wheel speeds from the system. </li>
	 * 		<li> CommunicationFailure - Failed to read wheel speed sensors from the module. </li>
	 * 		<li> TargetSpeedTimeout - Operator failed to achieve the target speed range in the time allowed. </li>
	 * 		<li> SoftwareFailure - A module exception occurred reading wheel speed sensors. </li>
	 * 		</ul>
	 * 
	 * <b>Module Interface Functions:</b>
	 * 		<ul>
	 * 		<li> GetInfo() - Message Tag: ReadSensorSpeeds </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo() </li>
	 * 		<li> GetRollerCount() </li>
	 * 		<li> GetComponentName() </li>
	 * 		<li> GetTestStepName() </li>
	 * 		<li> ShortCircuitTestStep() </li>
	 * 		<li> GetTestStepResult() </li>
	 * 		<li> AccelerateToTestSpeed() </li>
	 * 		<li> WaitForSteadySpeed() </li>
	 * 		<li> GetISpeeds() </li>
	 * 		<li> GetParameterFloat() </li>
	 * 		<li> GetParameter() </li>
	 * 		<li> GetParameterInt() </li> 
	 * 		<li> GetFaultCode() </li>
	 * 		<li> GetFaultDescription() </li> 
	 * 		<li> SendTestResultWithDetail() </li>
	 * 		<li> SendSubtestResultWithDetail() </li> 
	 * 		<li> ConvertStatusToResponse() </li>
	 * 		</ul>
	 * 
	 * @return The result of validating the wheel speed sensors.
	 * 		<ul>
	 * 		<li> Pass - All wheel speed sensors are within tolerance. </li>
	 * 		<li> Fail - One or more wheel speed sensors are out of tolerance or wheel speeds could not be gathered
	 * 					from the vehicle or system. </li>
	 * 		<li> Timeout - Target speed was not achieved in the time allowed. </li>
	 * 		<li> Skip - The wheel speed sensors do not need to be validated. </li>
	 * 		<li> SoftwareFail - A module exception occurred reading the wheel speed sensors. </li>
	 * 		</ul>
	 */
	virtual string SensorTest(void);
	/**
	 * Perform a reduction sequence at the specified wheel.
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The module will be commanded to reduce brake pressure at the specified wheel.  After the module is commanded to 
	 * reduce pressure, the software will wait for a preset amount of time.  The module can also be optionally commanded
	 * to hold the reduction at the specified wheel in order to keep the brake pressure off the wheel.
	 * <br><p>
	 * 
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> [wheel]ReductionPulseLength - Length of time in milliseconds for module to reduce brake pressure at 
	 * 										   the specified wheel. </li>
	 * 		<li> HoldReduction - Flag to indicate if the module should be commanded to hold the brake reduction. </li>
	 * 		</ul>
	 * 
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> CommunicationFailure - Module could not be commanded to reduce brake pressure. </li>
	 * 		<li> SoftwareFailure - A module exception occurred commanding the module to reduce brake rpessure. </li>
	 * 		</ul>
	 * 
	 * <b>Module Interface Functions:</b>
	 * 		<ul>
	 * 		<li> ActuateReduction() </li>
	 * 		<li> HoldAllValves() </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> TagArray() - Tag: [wheel]ReduxStart/ReduxStop </li>
	 * 		<li> GetParameterInt() </li>
	 * 		<li> GetParameterBool() </li>
	 * 		<li> ConvertStatusToResponse() </li>
	 * 		<li> GetFaultCode() </li>
	 * 		<li> GetFaultDescription() </li>
	 * 		<li> SendSubtestResult() </li>
	 * 		</ul>
	 *
	 * @param wheel      Wheel to perform reduction on.
	 * @param startIndex Reduction start index for the specified wheel.
	 * @param stopIndex  Reduction stop index for the specified wheel.
	 *
	 * @return Result of commanding the module to reduce brake pressure at the specified wheel.
	 * 		<ul>
	 * 		<li> Pass - The module was successfully commanded to reduce brake pressure at the specified wheel. </li>
	 * 		<li> Fail - A communication failure occurred commanding the module to reduce brake pressure. </li>
	 * 		<li> SoftwareFail - A module exception ocurred commanding the module to reduce brake pressure. </li>
	 * 		</ul>
	 */
	virtual string Reduction(const string &wheel);

	/**
	 * Perform the recovery sequence at the specified wheel.
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The module will be commanded to recover brake force at the specified wheel.  The brake force will be recovered for
	 * an amount of time specified by parameters.  Optionally, the module can be commanded to hold the valves after the
	 * recovery command has been issued.  The force array will also be tagged with the recovery start and end times.
	 * <br><p>
	 * 
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> [wheel]RecoveryPulseLength - Length of time in milliseconds for module to recover brake pressure at 
	 * 										  the specified wheel. </li>
	 * 		<li> HoldRecovery - Flag to indicate if the module should be commanded to hold the brake recovery. </li>
	 * 		</ul>
	 * 
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> CommunicationFailure - Module could not be commanded to recover brake pressure. </li>
	 * 		<li> SoftwareFailure - A module exception occurred commanding the module to recover brake rpessure. </li>
	 * 		</ul>
	 * 
	 * <b>Module Interface Functions:</b>
	 * 		<ul>
	 * 		<li> ActuateRecovery() </li>
	 * 		<li> HoldAllValves() </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> TagArray() - Tag: [wheel]RecovStart/RecovStop </li>
	 * 		<li> GetParameterInt() </li>
	 * 		<li> GetParameterBool() </li>
	 * 		<li> ConvertStatusToResponse() </li>
	 * 		<li> GetFaultCode() </li>
	 * 		<li> GetFaultDescription() </li>
	 * 		<li> SendSubtestResult() </li>
	 * 		</ul>
	 *
	 * @param wheel      Which wheel to recover.
	 * @param startIndex Recovery start index.
	 * @param stopIndex  Recovery end index.
	 *
	 * @return Result of recovering brake force at the specified wheel.
	 * 		<ul>
	 * 		<li> Pass - The module was successfully commanded to recover brake force. </li>
	 * 		<li> Fail - A communication failure occurred commanding the module to recover brake force. </li>
	 * 		<li> SoftwareFail - A module exception occurred commanding the module to recver brake force. </li>
	 * 		</ul>
	 */
	virtual string Recovery(const string &wheel);

	/**
	 * Initialize the gateway module
	 */
	virtual const std::string InitializeGateway(const std::string &value);
	/**
	 * Run park brake test
	 */
	const std::string TestStepParkBrake(const std::string &value);
	/**
	 * Run manual park brake test
	 */
	const std::string TestStepManualParkBrake(const std::string &value);
	/**
	 * Return true if tandem ABS equipped
	 */
	bool GetTestTandemAbs(void);
	/**
	 * Set tandem ABS equipped
	 */
	void SetTestTandemAbs(bool testTandemAbs);
	/**
	 * Verify the wheel is stopped
	 */
	const string VerifyWheelStopped(string wheel);
	/**
	 * Verify the wheel is at speed
	 */
	const string VerifyWheelSpeed(string wheel);
	/**
	 * Perform the Retarder test 
	 */
	virtual const string RetarderMultipositionalTest(void);
	/**
	 * Perform an individual retarder position test 
	 */
	virtual const string SinglePosition(int currentPosition, float *force);
	/**
	 * Perform the retarder speed limiter test 
	 */
	virtual const string RetarderLimiterTest(void);    
	/**
     * Abort the test sequence.
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Method used to abort the current processing based
     * on the needs of the component.<br>
     * <b><i>Note:</i></b>
     * 
     * <b>Functions Called:</b><br>
     * 		<ul>
     * 		<li> IsTandemDriveEquipped() </li>
     * 		<li> GenericTC::Abort() </li>
     * 		</ul>
     */
	void Abort(void);

private:
	/**
	 * Return the roller number as an int
	 */
	const int GetRollerNumber(string rollerName);
	/**
	 * Is there a tandem axle to test?
	 */
	bool m_testTandemAbs;

	/**                           
	 * A list of the types of trucks that need a tandem axle ABS test
	 */
	XmlNodeMap m_testTandemAbsTypes;

	/**                           
	 * A list of the types of trucks that need a tandem axle Park Brake test
	 */
	XmlNodeMap m_testTandemParkBrakeTypes;

	/**                           
	 * A list of the types of trucks that need a tandem axle ABS test
	 */
	XmlNodeMap m_firingSequenceMap;

	/**
	 * An array of intergers, representing the sequence of what wheels to fire.
	 */
	int * m_firingSequence;
	/**
	 * IMotorController
	 * Used to control the motors individually
	 */
	IMotorController m_motorController;
	/**
	 * Firing order for the 4-channel Tandem ABS test.
	 */
	XmlNodeMap m_abs4ChannelTandemFiringOrder;
	/**
	 * A raw comms object to allow the resetting of the reflectionCount upon a new test.
	 */
	BepComponent m_bepComponentObject;
	/**
	 * If a tandem parking brake test must be done.
	 */
	bool m_isTandemAxle;

};
//-----------------------------------------------------------------------------
#endif  //TevesMk20ETC_h
