//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/TestComponents/Emissions/Include/BoschEmissionsTC.h 1     2/22/05 2:21p Derickso $
//
// FILE DESCRIPTION:
//  Generic Emissions Test Component.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Emissions/Include/BoschEmissionsTC.h $
// 
// 1     2/22/05 2:21p Derickso
// New emissions test component.
// 

// 1     1/20/05 3:36p BMiedlar
// Converting to template.
// 
//*************************************************************************
#ifndef DcbrBoschEmissionsTC_h
#define DcbrBoschEmissionsTC_h
//*************************************************************************
#include "GenericEmissionsTCTemplate.cpp"
#include "IMotorController.h"
#include "DataAnalysis.h"
//-------------------------------------------------------------------------

/**
 * Bosch Emissions test component.
 * This test component will test DaimlerChrysler NGC-III engine controllers.
 * Specifically, this test complies with the Emissions Compliance
 * Document and the appliocable Process Standards.
 * 
 * @see GenericEmissionsTCTemplate
 * @see GenericTestComponentTemplate
 * @see GenericEmissionsModule
 * @see BoschEmissionsModule
 */
template <class VehicleModuleType>
class DcbrBoschEmissionsTC : public GenericEmissionsTCTemplate<VehicleModuleType>
{
public:
    /**
     * Class constructor.
     */
    DcbrBoschEmissionsTC();
    /**
     * Class Destructor.
     */
    virtual ~DcbrBoschEmissionsTC();
    /**
     * This is the method that will handle the execution of each
     * test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded
     * to execute the component tests.
     *
     * @param value This is the value that was commanded by
     * the test sequencer.
     * @return The status of the overall objective.
     */
    virtual const string CommandTestStep(const string &value);
    /**
     * @brief Child-class specific hook into Initialize()
     *
     * By default this method simply initializes the the
     * Vehicle Module (it checks to see that it has been
     * built first).  This method can be used by
     * child nodes to insert code into the Initialize()
     * block.  Initialize() calls this
     * method after calling TestComponent::Initialize()
     * and before calling InitializationComplete().
     * If GenericTestComponentTemplate children have specific initialization
     * needs, they should implement this method and make sure to
     * call their parent's InitializeHook after they are finished.
     * A good idea is to AND the the parent's InitializeHook
     * return value with your own to determine the total
     * return value.
     *
     * @param config the test component's configuration node
     */
    virtual void InitializeHook(const XmlNode *config);

protected:
    /**
     * Send timing signals to wake up communication
     * 
     * @return The status of the wake up call
     */
    string ModuleWakeUp(void);
    /**
     * Create a TestResult name that includes the component name prepended.
     *
     * @return The name of the Tcurrent test step.
     */
    const string CreateTestResultName(const std::string name = "");
    /**
     * Apply a load "torque" to the vehicle at speed then check engine RPM and Temperature. </li>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Prompt and wait for the vehicle to reach target speed then apply a load to the drive wheels. 
     * Monitor Temperature and RPM from the engine module for set amount of time. Default stop and exit
     * if RPM or Temperature is out of range. Report all readings. </li>
     * <br><p>
     *
     * <b>Test Component Parameters:</b>
     * 		<ul>
     * 		<li> EngineTorqueMinSpeed - Float Minimum speed for Torque test to start. </li> 
     * 		<li> EngineTorqueMaxSpeed - Float Maximum speed to run the Torque test. </li>
     * 		<li> MotorLoadingTestTime - INT Amount of time to apply torque. Zero time = one read. </li>
     * 		<li> EngineTorqueMaxTemperature - INT Maximum allowed engine temperature. </li>
     * 		<li> EngineTorqueMinRpm - INT Minimum allowed RPM for this test. </li> 
     * 		<li> EngineTorqueMaxRpm -  INT Maximum allowed RPM for this test. </li>
     * 		<li> EngineTorqueIgnoreRange - Bool Set to Yes to force loading and monitoring to run for full time. </li> 
     *		<li> UpdateTorqueResultInModule - Bool Set to Yes if required to update EOL byte torque test bit. </li>  
     * 		</ul>
     *
     * <b>Data Tags:</b>
     * 		<ul>
     * 		<li> EOLTorqueTestResultRegister - Register with torque test bit. </li> 
     * 		</ul>
     *
     * <b>Test Step Info:</b>
     * 		<ul>
     * 		<li> Description - Test step description. </li>
     * 		<li> ScanDelay - Amount of time to wait between commands. </li>
     * 		<li> TimeOut - Amount of time to wait for opertor input. </li>
     * 		</ul>
     * 
     * <b>Prompts:</b>
     * 		<ul>
     * 		<li> EngineTorqueTest - Let the operator know to monitor the vehicle during this test. </li>
     * 		<li> MaintainSpeed - Instruct operator to maintain speed during test. </li>
     * 		</ul>
     * 
     * <b>Fault Codes:</b>
     * 		<ul>
     * 		<li> EngineTorqueRpmFailure - Engine RPM out of range during test. </li>
     * 		<li> EngineTorqueTemperatureFailure - Engine Temperature to high during test. </li>
     * 		<li> CommunicationFailure - Failed to get module data. </li>
     * 		<li> EngineTorqueSpeedFailure - Failed to reach target speed. </li>
     * 		<li> SoftwareFailure - Missing configuration or bad function call. </li>
     * 		<li> ModuleUpdateCommunicationFailure -  Could not update the EOL Torque test status bit. </li>
     * 		</ul>
     * 
     * <b>Functions Called:</b>
     * 		<ul>
     * 		<li> ShortCircuitTestStep() </li>
     * 		<li> GetComponentName() </li>
     * 		<li> GetTestStepName() </li>
     * 		<li> Log() </li>
     * 		<li> DisplayPrompt() </li>
     * 		<li> RemovePrompt() </li>
     * 		<li> GetPromptBox() </li>
     * 		<li> GetPrompt() </li>
     * 		<li> GetPromptPriority() </li>
     * 		<li> SetStartTime() </li>
     * 		<li> TimeRemaining() </li>
     * 		<li> SetSecondaryStartTime() </li>
     *		<li> SecondaryTimeRemaining() </li>
     * 		<li> BposSleep() </li>
     * 		<li> GetTestStepInfo() </li>
     * 		<li> GetParameterBool() </li>
     * 		<li> GetParameterInt() </li>
     * 		<li> GetParameterFloat() </li>
     * 		<li> StatusCheck() </li>
     * 		<li> GetFaultCode() </li>
     * 		<li> GetFaultDescription() </li>
     * 		<li> ConvertStatusToResponse() </li>
     * 		<li> SendTestResultWithDetail() </li>
     * 		<li> WaitAndPromptForSpeed() </li> 
     *      <li> EnableMotorLoading() </li>
     * 		<li> DisableMotorLoading() </li>
     * 		<li> ReadModuleData() </li>
     * 		<li> UpdateEOLStatusRegister() </li>
     * 		</ul>
     * 
     * @return Test result for shifting the transmission to neutral.
     * 		<ul>
     * 		<li> Pass - The Rpm, Temperature, Vibration and Noise are in range.. </li>
     * 		<li> Skip - The test step was skipped based on the result of ShortCircuitTestStep(). </li>
     * 		<li> Fail - If bad system status, or RPM or Temperature were out of acceptable limits. </li>
     * 		</ul>
     */
    string EngineTorqueTest(void);
    /**
     * Request the operator verify Noise and Vibration were acceptable with load applied. </li>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Prompt and wait for the operator to Pass or Fail vehicle performace during Torque test. </li>
     * <br><p>
     *
     * <b>Data Tags:</b>
     * 		<ul>
     * 		<li> CheckTorquePrompt - Prompt the operator to pass or fail vibration and noise levels. </li>
     * 		</ul>
     * 
     * <b>Test Step Info:</b>
     * 		<ul>
     * 		<li> Description - Test step description. </li>
     * 		<li> TimeOut - Amount of time to wait for opertor input. </li>
     * 		</ul>
     * 
     * <b>Prompts:</b>
     * 		<ul>
     * 		<li> CheckVibrationAndNoise - Instruct operator to predd Pass or Fail for vibration and noise. </li>
     * 		</ul>
     * 
     * <b>Fault Codes:</b>
     * 		<ul>
     * 		<li> EngineTorqueOperatorFailure - Operator failed test for Vibration and Noise. </li>
     * 		<li> SoftwareFailure - Missing configuration or bad function call. </li>
     * 		</ul>
     * 
     * <b>Functions Called:</b>
     * 		<ul>
     * 		<li> ShortCircuitTestStep() </li>
     * 		<li> GetComponentName() </li>
     * 		<li> GetTestStepName() </li>
     * 		<li> Log() </li>
     * 		<li> SetStartTime() </li>
     * 		<li> TimeRemaining() </li>
     * 		<li> SetSecondaryStartTime() </li>
     *		<li> SecondaryTimeRemaining() </li>
     * 		<li> GetTestStepInfoInt() </li>
     * 		<li> GetFaultCode() </li>
     * 		<li> GetFaultDescription() </li>
     * 		<li> ConvertStatusToResponse() </li>
     * 		<li> SendTestResult() </li>
     * 		<li> OperatorPassFail() </li>
     * 		</ul>
     * 
     * @return Test result for shifting the transmission to neutral.
     * 		<ul>
     * 		<li> Pass - Operator indicated the Vibration and Noise levels were acceptable. </li>
     * 		<li> Skip - The test step was skipped based on the result of ShortCircuitTestStep(). </li>
     * 		<li> Fail - Operator failed the Test. </li>
     * 		</ul>
     */
    string CheckEngineTorquePerformace(void);

    string MaxSpeedTest(void);
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
     *       <ul>
     *       <li> IsTandemDriveEquipped() </li>
     *       <li> GenericTC::Abort() </li>
     *       </ul>
     */
    void Abort(void);


private:

    /**
     * Check if the torque test should be performed.
     * 
     * @return Flag indicating if the torque test should be performed.
     */
    bool ShouldPerformEngineTorqueTest();

    /**
     * Retrieve/Store the comm bus the module is utilizing.  If the parameter to the function is NULL,
     * the comm bus type will be returned; otherwise the value will be stored.
     *
     * @param commBus  Comm bus the module is utilizing.
     *
     * @return The comm bus the module is utilizing.
     */
    const string& WhichCommBus(const string *commBus = NULL);
    /**
     * Flag indicating if the vehicle is equipped with Motor Braking (Motor Brake, Top Brake or Turbo Brake).
     */
    bool m_motorBrakeEquipped;
    /**
     * XmlNode pointer to our config node as passed into Initialize.
     */
    const XmlNode *m_boschConfigNode;
    /**
     * Store the communcation bus the module is utilizing.
     */
    string m_commBus;
    /**
     * Store XML Node map which contains the T-Case types where torque is NOT divided.
     * for reading part number data
     */
    XmlNodeMap m_normalTorqueTypes;
    /**
     * Store XML Node map which contains the T-Case types where torque is NOT divided.
     * for reading part number data
     */
    XmlNodeMap m_maxSpeedTestEnabledTypes;
};

//------------------------------------------------------------------------------
#endif  //DcbrBoschEmissionsTC_h
