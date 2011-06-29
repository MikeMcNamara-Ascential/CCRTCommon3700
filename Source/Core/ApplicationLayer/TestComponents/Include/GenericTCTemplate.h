//******************************************************************************
// Description:
//  Generic Test Component.
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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Include/GenericTCTemplate.h $
// 
// 2     2/07/08 10:02a Derickso
// Updated comment for CheckPartNumber to add in the details of checking
// the system tag UseBypassPartNumberList.
// 
// 1     11/02/07 8:18a Derickso
// Updated comments.
// 
// 1     10/29/07 11:06a Derickso
// Updated header comments to doxygen format.
// 
// 13    5/01/07 11:19a Rwiersem
// Changes for the 07050201 core release:
// 
// Updated the Javadoc comments.
// 
// 12    2/22/07 8:59a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added LoadStaticTestResults().
// - Added m_staticTesterServerComm.
// 
// 11    1/17/07 3:34p Rwiersem
// Changes for the 07011901 core release:
// 
// - Made the test class a friend.
// - Added Activate().
// - Added ReloadConfiguration().
// - Added StartSpecialKeepAlive().
// - Added m_abortCalled.
// 
// 10    10/25/06 3:59p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added m_protocolFile and m_protocolFileTime to hold the protocol
//       file name and last time it was updated.
//
//     - Added Deactivate(), ShouldReloadConfigFile(), and 
//       WarmInitialize().  These methods are used to keep the test 
//       components running when a new drive curve starts.
// 
// 9     7/05/06 9:55p Cward
// Added support for reading recording, and reporting of module data.
//
// 8     5/08/06 7:10p Cward
// Added wait for Engine off key on method
//
// 7     3/30/06 1:46a Cward
// Updated to add the capability to enter diagnostic mode without a
// response
//
// 6     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 5     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 4     5/25/04 3:50p Cward
// Added Set Communication Failure and Get Communication  Failure methods.
// Updated Stop Module Keep Alive function to always stop the keep alive
// thead. Add Set Comunication Failure calls. Make Abort function virtual.
// Add class variable
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 10    11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
//
// 9     11/13/03 7:11p Derickso
// Minor updates from testing at JTE.
//
// 8     10/27/03 5:32p Derickso
// Updated ReadFaults to check if faults were recorded
// in the module prior to attempting to process the faults.
//
// 7     10/20/03 9:46a Derickso
// Debugged and tested against a vehicle.
//
// 6     9/22/03 4:45p Derickso
// Latest version with updated architecture.
// TestComponent no longer exists.
//
// 5     9/16/03 8:10p Derickso
// Latest changes and version.
// Numerous changes and additions.
//
// 4     9/08/03 7:18a Derickso
// Moved IsEngineRunning() and CheckEngineRunning() to
// GenericTCTemplate class.
//
// 3     8/29/03 3:22p Derickso
// Updated GetTestStepInfo calls to GetParameter calls.
// Updated AccelerateToTestSpeed to take a parameter.
// Updated call to AccelerateToTestSpeed to read the
// test step for the parameters to use.
//
// 2     8/26/03 10:24a Derickso
// Corrected compiler errors
//
// 1     8/21/03 9:00a Derickso
// 05WK CCRT Project
//
// 11    8/20/03 7:17a Derickso
// Recombined cpp and h files.  Compiler does not support implementation
// being seperate for template classes.
//
// 6     8/13/03 4:14p Derickso
// Updated class name to match filename.
//
// 5     8/12/03 2:36p Derickso
// Merged DataArrayTestComponent functionality into this test component.
// Split implementation out into cpp file.
//
// 4     5/28/03 2:57p Derickso
// Do not need to templatize on ProtocolFilterType.
// TestComponent does not care.
// Use constructor like:
//      TestComponentType<ModuleType<ProtocolFilterType> > myTC;
//
// 3     5/28/03 7:37a Derickso
// Added protocol filter type to the template.
// Reverted test steps back to using GetInfo methods where
// appropriate.
//
// 1     5/27/03 3:32p Derickso
// Periodic backup.
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericTCTemplate_H
#define GenericTCTemplate_H
//==============================================================================

#include <string>
#include "GenericTC.h"

/** GenericTCTemplate is a generic test component. */
template <class ModuleType>
class GenericTCTemplate : public GenericTC
{
    friend class GenericTcTemplateTest;

public:
    /**
     * <b>Description:</b><br>
     * Constructor that initializes the member variables to their default value.
     */
    GenericTCTemplate(void);
    /** 
     * <b>Description:</b><br>
     * Destructor that clears the XmlNodeMap member variables.
     */
    virtual ~GenericTCTemplate(void);
    /**
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The specified config file will be loaded and used to complete the test component initialization.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::Initialize() </li>
     *      </ul>
     *
     * @param fileName The name of the XML configuration file to be used for initialization.
     */
    void Initialize(const string &fileName);
    /**
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Initialize the test component using the configuration data passed in.  Once initialization has completed, any
     * system tags that need to be reset will be reset to initial values.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GenericTC::Initialize() </li>
     *      <li> InitializeHook() </li>
     *      <li> InitializationComplete() </li>
     *      <li> ResetSystemTags() </li>
     *      <li> GetProcessName() </li>
     *      </ul>
     *
     * @param config The XmlNode to use for initialization.
     */
    void Initialize(const XmlNode *config);
    /**
     * Deactivate the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Indicate that the component should de-activate itself.  This should happen at the end of a test when the component 
     * is no longer needed.  The vehicle module interface will be instructed to close communication channels.  The base
     * class will then be called to complete the deactivation.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> CloseCom() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::Deactivate() </li>
     *      </ul>
     */
    virtual void Deactivate(void);
    /**
     * Reload the configuration data.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reload the configuration file. The following lists will be cleared prior to calling the base class to complete
     * reloading the configuration data:
     * <ul>
     * <li> Ignored fault list </li>
     * <li> Valid module part number list </li>
     * <li> Module data items list </li>
     * </ul>
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::ReloadConfiguration() </li>
     *      </ul>
     */
    virtual void ReloadConfiguration(void);
    /**
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This method will handle the execution of each test step that has been specified by the system for each test 
     * objective that is passed to it.  If the test step is not supported by this class, it will be 
     * passed to GenericTC.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description fo the test step. </li>
     *      </ul>
     * 
     * <b>Supported Test Steps:</b>
     *      <ul>
     *      <li> Setup </li>
     *      <li> EnterDiagnosticMode </li>
     *      <li> ExitDiagnosticMode </li>
     *      <li> CheckPartNumber </li>
     *      <li> LockModule </li>
     *      <li> UnLockModule </li>
     *      <li> CommandSecurityUnlock </li>
     *      <li> CheckVIN </li>
     *      <li> ProgramVIN </li>
     *      <li> ClearFaults </li>
     *      <li> ReadFaults </li>
     *      <li> CollectTraceData </li>
     *      <li> StartModuleKeepAlive </li>
     *      <li> StartSpecialKeepAlive </li>
     *      <li> StopModuleKeepAlive </li>
     *      <li> StopBackgroundMonitors </li>
     *      <li> ResumeBackgroundMonitors </li>
     *      <li> MonitorFaults </li>
     *      <li> CalProgram </li>
     *      <li> ReportModuleInfo </li>
     *      <li> WaitForEngineOffKeyOn </li>
     *      <li> FinishUp </li>
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
     *      <li> SendOverallResult() </li>
     *      <li> GenericTC::CommandTestStep() </li>
     *      </ul>
     * 
     * @return The result of the overall test step.  BEP_UNSUPPORTED_RESPONSE will be returned if the test step is not supported.
     */
    virtual const string CommandTestStep(const string &value);
    /**
     * Set or clear a test component communication failure.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set or clear a test component communication failure.
     * <p>
     * @param status The communication failure status.
     */
    void SetCommunicationFailure(bool status);
    /**
     * Check the flag to determine if there was a communication failure.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the communication failure flag.
     * <p>
     * @return The communication failure status.
     */
    bool GetCommunicationFailure(void);

	/**
	 * Reset the component.
	 * <p><b>Category:</b> Utility
	 * <p><b>Description:</b><br>
	 * The overall test result for the component will be reset to a "Test in Progress" state.
	 * <p>
	 * <b>Functions Called:</b>
	 *      <ul>
	 *      <li> GetComponentName() </li>
	 *      <li> ResetOverallResult() </li>
	 *      </ul>
	 */
	virtual void Reset(void);

protected:
    /**
     * Check if the configuration file should be reloaded.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Checks if the component should reload it's config file in response to an "Activate" command.  The module config 
     * file will also be checked to determine if has been updated since the last time the file was loaded.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> Uninitialize() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::ShouldReloadConfigFile() </li>
     *      <li> FileHasChanged() </li>
     *      </ul>
     * 
     * @return Flag indicating if the config file should be reloaded.
     */
    virtual bool ShouldReloadConfigFile(void);
    /**
     * Handle data published to the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * If the system sets the abort flag, stop the background monitors.  If the node is any other value, pass it to 
     * BepServer::Publish().
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StopBackgroundMonitors() </li>
     *      <li> BepServer::Publish() </li>
     *      </ul>
     * 
     * @param node The tag and value of the subscribe data.
     * 
     * @return The result of handling the published data.
     */
    virtual const string Publish(const XmlNode *node);
    /**
     * Initialize the module interface.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The module interface will be initialized in order to retrieve data from the vehicle.  If the module has already
     * been initialized, the communication channel will be opened; otherwise, the module interface will be initialized.  
     * The module interface config file modification time will be saved to determine later if it needs to be reloaded.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> ConfigFile - Name of the configuration file to use for setting up the module interface. </li>
     *     </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> IsInitialized() </li>
     *      <li> OpenCom() </li>
     *      <li> Initialize() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> GetFileModificationTime() </li>
     *      </ul>
     * 
     * @return Status of initializing the module interface:
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the module interface was set up properly. </li>
     *          <li> BEP_STATUS_SOFTWARE if the module interface could not be setup. </li>
     *          </ul>
     */
    virtual BEP_STATUS_TYPE InitializeModuleInterface(void);
    /**
     * Perform test setup.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * If module initialization has ben delayed, it will be performed now; otherwise, there is no specifc setup to be
     * performed in this class.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DelayModuleInitialization - Flag indicating if the module initialization has been delayed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetParameterBool() </li>
     *      <li> InitializeModuleInterface() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     * 
     * @return Result of setting up the test.
     */
    virtual inline string Setup(void);
    /**
     * Finish up test processing.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Perform any finish up processing neccessary to complete the test.  There is no generic finish up processing to
     * be done.
     * <p>
     * @return  Pass
     */
    virtual inline string FinishUp(void);
    /**
     * Stop the background data monitors.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Stop any background monitors that have been started.  No checking is performed to see if all the monitored data 
     * was gathered and evaluated.  The main intent of this test step is to pause the data monitors to reduce bus 
     * traffic during critical communication timing events or if an Abort signal was received.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StopModuleKeepAlive() </li>
     *      </ul>
     * 
     * @return Result of stopping the background data monitors.
     */
    virtual inline string StopBackgroundMonitors(void);
    /**
     * Resume monitoring data.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Restart any background monitors that were stopped with the StopBackgroundMonitors method.
     * <br><b><i>Note:</i></b>  If any background monitors perform any special setup or store initial values, care 
     *                          should be taken to not lose the initial values that were stored when the monitor was 
     *                          initially started.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StartModuleKeepAlive() </li>
     *      </ul>
     * 
     * @return Result of resuming the background data monitors.
     */
    virtual inline string ResumeBackgroundMonitors(void);
    /**
     * Check system status.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check the current status of the machine and the component to determine if the current conditions are valid for 
     * continuing a test.  If the parameter <i>DeveloperTest</i> is true, this function will always return 
     * BEP_STATUS_SUCCESS.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DeveloperTest - Flag to indicate if system conditions/status should be ignored. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterBool() </li>
     *      <li> GenericTC::StatusCheck() </li>
     *      </ul>
     * 
     * @return The system status.
     */
    virtual INT32 StatusCheck(void);
    /**
     * Initialize the test component.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Initialze the configuration items for this class.  If the parameter <i>DelayModuleInitialization</i> is false, 
     * the module interface will be initialized.  Otherwise it will be initialized in Setup().  The parameter 
     * <i>LoadStaticTestResults</i> will also be checked to determine if static test results need to be loaded.  The
     * list of module faults to ignore will be saved as will the list of valid module part numbers.  The list of
     * module data items will also be saved.
     * <p>
     * <b>Configuration Items:</b>
     *      <ul>
     *      <li> Setup/IgnoreFaults - List of module faults to ignore.  The test will not fail if these are the only
     *                                faults detected in the module. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ValidPartNumbers - List of valid part numbers to use for verification if the module part number is not
     *                              available in the vehicle build data. </li>
     *      <li> ModuleDataItems - List of data items to report for module. This is mainly used for recording version 
     *                             numbers associated with the module. </li>
     *      <li> DelayModuleInitialization - Flag to determine if module initialization should be delayed.  This is
     *                                       mainly used for plants that build both left and right hand drive vehicles. </li>
     *      <li> LoadStaticTestResults - Flag indicating if the static test results should be loaded from the hand held
     *                                   testers. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> StaticTesterName - Name of the static tester server. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterBool() </li>
     *      <li> InitializeModuleInterface() </li>
     *      <li> GetDataTag() </li>
     *      <li> LoadStaticTestResults() </li>
     *      <li> GetProcessName() </li>
     *      </ul>
     * 
     * @param config The configuration data to use to initialize the component.
     */
    virtual void InitializeHook(const XmlNode *config);
    /**
     * Reinitialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reinitialize using the already loaded configuration file. This is intended to be used when a component is 
     * activated and there have been no modifications to any configuration files used by the component.  The base class
     * will be called to begin the reinitialization.  If module initialization has been delayed, the module interface
     * will be commanded to open it's communication channels.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DelayModuleInitialization - Flag indicating if modue intialization has been delayed. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> OpenCom() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::WarmInitialize() </li>
     *      <li> GetParameterBool() </li>
     *      </ul>
     */
    virtual void WarmInitialize(void);
    /**
     * Start the thread to send keep alive messages to the module.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Start the module keep alive thread.  The keep alive thread will periodically send a message to the module to
     * keep the module in diagnostic mode.
     * <p>
     * <b>Functions Called:</b>
     *     <ul>
     *     <li> StartSpecialKeepAlive() </li>
     *     </ul>
     * 
     * @return The result of starting the module keep alive thread.
     */
    virtual string StartModuleKeepAlive(void);
    /**
     * Start the thread to send keep alive messages to the module.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Start the module keep alive thread.  The keep alive thread will periodically send a message to the module to
     * keep the module in diagnostic mode.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AlwaysStartKeepAlive - Flag to indicate the module keep alive thread should be started even if this 
     *                                  component passed on a previous test cycle. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> StartModuleKeepAliveFail - Failed to start the module keep alive thread. </li>
     *      <li> SoftwareFailure - A ModuleException occurred attempting to start the module keep alive thread. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StartModuleKeepAlive() </li>
     *      <li> StartSpecialKeepAlive() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> CheckCableConnect() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of starting the module keep alive thread.
     */
    virtual string StartSpecialKeepAlive(std::string messageId);
    /**
     * Stop the keep alive thread.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Stop the module keep alive thread.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> SoftwareFailure - A ModuleException occurred attempting to stop the keep alive thread. </li>
     *      <li> StopModuleKeepAliveFail - Could not stop module keep alive. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StopModuleKeepAlive() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of stopping the keep alive thread.
     */
    virtual string StopModuleKeepAlive(void);
    /**
     * Validate the module part number.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Verify the correct module part number is programmed into the module.  Depending on parameter settings, the part
     * number can be validated against the list from the configuration file, the part number from the vehicle build data,
     * or not validated at all.  The system tag <i>UseBypassPartNumberList</i> can be checked to determine if the 
     * bypass part number list should be checked if the part number read from the module does not match broadcast.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ModulePartNumber - System tag to read the module part number from the vehicle build data. </li>
     *      <li> ReadModulePartNumber - Message tag to use to read the part number from the module. </li>
     *      <li> UseBypassPartNumberList - System tag to use for determining if the bypass part number list should be
     *                                     checked if the module part number does not match broadcast. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> BypassPartNumberValidation - Flag indicating if the part number validation should be skipped. </li>
     *      <li> ValidatePartNumberList - Flag indicating if the module part number should be validated against the
     *                                    list of valid part numbers in the list from the config file. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> PartNumberMismatch - Module part number validation failed. </li>
     *      <li> CommunicationFailure - Could not read the part number from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while reading the part number from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tag: ReadModulePartNumber </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SetCommunicationFailure() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> SystemReadBool() </li>
     *      </ul>
     * 
     * @return The result of validating the module part number.
     */
    virtual string CheckPartNumber(void);
    virtual string CheckPartNumbers(void);
    /**
     * Lock access to the module.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Lock the module.  Locking the module will prevent any process from updating data in the module.
     * <p>
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> FaultLockModuleCommError - Communication failure attempting to lock the module. </li>
     *      <li> FaultException - A ModuleException occurred while attempting to lock the module. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> LockModule() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendTestResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultName() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     * 
     * @return The result of locking the module.
     */
    virtual string LockModule(void);
    virtual bool IsModuleLocked(void);
    /**
     * Unlock access to the module.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Unlock access to the module.
     * <p>
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> FaultUnlockModuleCommError - Communication failure unlocking access to the module. </li>
     *      <li> FaultException - A ModuleException occurred unlocking access to the module. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ModuleKey - Key to use for unlocking access to the module. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> UnLockModule() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> SendTestResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultName() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     * 
     * @return The result of unlocking access to the module.
     */
    virtual string UnLockModule(void);
    /**
     * Unlock module security.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Unlock the module security so the EOL status registers can be updated.  A limited number of attempts will be made
     * to unlock the module security.  If the number of failed attempts is too great, the module could be locked out and
     * result in the inability to start the vehicle for an extended period of time.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SecurityUnlockAttempts - Maximum number of attempts to unlock module security. </li>
     *      <li> SecurityUnlockDelay - Amount of time(ms) to wait between security unlock attempts. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> SecurityUnlockFailed - Failed to unlock module security. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> UnlockModuleSecurity() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of unlocking module security.
     */
    virtual string CommandSecurityUnlock(void);
    /**
     * Enter diagnostic mode.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Command the module to begin a diagnostic session.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AlwaysEnterDiagnosticMode - Flag to indicate if the module should always be placed into diagnostic mode
     *                                       regardless of component status. </li>
     *      </ul>
     * 
     * <b>Fault Tagss:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not command the module to enter diagnostic mode. </li>
     *      <li> SoftwareFailure - A ModuleException occurred attempting to place the module into diagnostic mode. </li>
     *      <li> TimeoutFailure - Timeout waiting for the operator to connect the diagnostic cable. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> EnterDiagnosticMode() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> CheckCableConnect() </li>
     *      <li> SetCommunicationFailure() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of placing the module into diagnostic mode.
     */
    virtual string EnterDiagnosticMode(void);
    /**
     * Exit diagnostic mode.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The module will be commanded to end the current diagnostic sesion.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not command the module to exit diagnostic mode. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while exiting the current diagnostic session. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ExitDiagnosticMode() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of exiting the current diagnostic session.
     */
    virtual string ExitDiagnosticMode(void);
    /**
     * Enter diagnostic mode.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The module will be commanded to enter diagnostic mode.  This function is used for modules that do not send a 
     * response to the enter diagnostic mode command.  The module interface will not wait for a module response.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AlwaysEnterDiagnosticMode - Flag to indicate the module should always be placed into diagnostic mode
     *                                       regardless of the component status. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not command the module to enster diagnostic mode. </li>
     *      <li> SoftwareFailure - A ModuleException occurred commanding the module to enter diagnostic mode. </li>
     *      <li> TimeoutFailure - Timeout waiting for the operator to connect the diagnostic cable. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> EnterDiagnosticModeNoResponse() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> CheckCableConnect() </li>
     *      <li> SetCommunicationFailure() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of commanding the module to enter diagnostic mode.
     */
    virtual string EnterDiagnosticModeNoResponse(void);
    /**
     * Validate the VIN.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The VIN will be read from the module.  Depending on parameter settings, the VIn may be validated against the VIN
     * from the vehicle build data or not validated at all.  The test sequence may be aborted if the module VIN does
     * not match the build data or if the VIn could not be read from the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadModuleVIN - Message tag to use for reading the VIN from the module. </li>
     *      <li> VehicleVIN - System tag for getting the VIN from the vehicle build data. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> BypassVINValidation - Flag indicating if the VIn should not be validated. </li>
     *      <li> AbortIfVinMismatch - Flag indicating if the test sequence should be aborted for incorrect VINs. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> VINMismatch - Module VIN did not match the vehicle build data. </li>
     *      <li> CommunicationFailure - Could not read the VIn from the module. </li>
     *      <li> SoftwareFailure - A Module Exception occurred attempting to read the VIN from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tags: ReadModuleVIN </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameterBool() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * @return The result of validating the module VIN.
     */
    virtual string CheckVIN(void);
    /**
     * Program the VIN into the module.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The VIN will be programmed into the module.
     * <p>
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> FaultVINCommError - Could not program the VIN into the module. </li>
     *      <li> FaultProgramVINBadVIN - The VIN length is incorrect. </li>
     *      <li> FaultProgramVINBadVINLength - The VIn length is zero. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> VIN - System tag to retrieve the VIN. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> VINLength - Expected length of the VIN. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ProgramVIN() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameterInt() </li>
     *      <li> SendTestResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultName() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetComponentName() </li>
     *      </ul>
     * 
     * @return The result of programming the VIN into the module.
     */
    virtual string ProgramVIN(void);
    /**
     * Clear faults.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The module will be commanded to clear faults.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not command the module to clear faults. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while commanding the module to clear faults. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ClearFaults() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> SetCommunicationFailure() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of clearing faults from the module.
     */
    virtual string ClearFaults(void);
    /**
     * Read faults.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Faults will be read from the module.  Any faults detected in the module will be compared to the ignored fault 
     * list.  If the fault is not in the list, it will be reported.  Depending on parameter settings, the test sequence
     * can be aborted if non-ignored faults are detected in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadModuleFaults - Message tag to read faults from the module. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AbortIfDTCPresent - Flag to determine if the test sequence should be aborted if faults are detected. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ModuleFaults - Faults were detected in the module. </li>
     *      <li> CommunicationFailure - Could not read faults from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while reading faults from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message tag: ReadModuleFaults </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReportDTC() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameterBool() </li>
     *      <li> SystemWrite() </li>
     *      <li> DTCsInModule() </li>
     *      <li> SetCommunicationFailure() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of reading faults from the module.
     */
    virtual string ReadFaults(void);
    /**
     * Collect traceability data.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The traceability data will be read from the module.  No validation will be performed on the data rear from the 
     * module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadTraceData - Message tag to read the traceability data from the module. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read the traceability data from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while reading the traceability data from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message tag: ReadTraceData </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <lI> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * @return The result of reading the traceability data from the module.
     */
    virtual string CollectTraceData(void);

#if 0
    /**
     * <b>Description:</b>
     * Monitor faults.
     * 
     * This method is currently not implemented.
     *
     * @return The status of the operation.
     * 
     * @see BepDefs.h "Standard BEP Test Status" for a list of return values.
     */
    virtual string MonitorFaults(void);
    /**
     * <b>Description:</b>
     * Run calibration program.
     *
     * This method is currently not implemented.
     * 
     * @return The status of the operation.
     * 
     * @see BepDefs.h "Standard BEP Test Status" for a list of return values.
     */
    virtual string CalProgram(void);
#endif
    /**
	 * Command the module to reset.
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * This method resets the module, ensuring that the module setup and clearing of faults is completed.
	 * <br><p>
	 *
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> ModuleResetDelay - Amount of time to wait for the module to complete its reset sequence. </li>
	 * 		</ul>
	 * 
	 * <b>Test Step Info:</b>
	 * 		<ul>
	 * 		<li> Description - Description of the test step. </li>
	 * 		</ul>
	 * 
	 * <b>Module Interface Functions:</b>
	 * 		<ul>
	 * 		<li> PerformPowerOnReset() </li>
	 * 		</ul>
	 * 
	 * <b>Fault Codes:</b>
	 * 		<ul>
	 * 		<li> CommunicationFailure - Communication failure commanding the module to reset. </li>
	 * 		<li> SoftwareFailure - A module exception occurred while commanding the module to reset. </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetTestStepInfo() </li>
	 * 		<li> GetComponentName() </li>
	 * 		<li> GetTestStepName() </li>
	 * 		<li> ShortCircuitTestStep() </li>
	 * 		<li> ConvertStatusToResponse() </li>
	 * 		<li> GetFaultCode() </li>
	 * 		<li> GetFaultDescription() </li>
	 * 		<li> BposSleep() </li>
	 * 		<li> GetParameterInt() </li>
	 * 		<li> SendTestResult() </li>
	 * 		</ul>
	 * 
	 * @return The result of resetting the module.
	 * 		<ul>
	 * 		<li> Pass - The module was properly reset. </li>
	 * 		<li> Fail - An error occurred attempting to reset the module. </li>
	 * 		<li> Skip - The test step was not performed. </li>
	 * 		<li> SoftwareFail - A module exception was thrown attempting to reset the module. </li>
	 * 		</ul>
	 */
	virtual string ResetModule(void);
    /**
     * Report module configuration data.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Specific data items will be read from the module and reported.  This is mainly to track the module versions under
     * test.  The data items to read are contained in a list in the config file which gets loaded during the initialization
     * process.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReportModuleInfoMessage - Message tag to use for reading the data items from the module. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read the data items from the module. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AllowReportInfoFail - Flag to indicate if a communication failure should be ignored. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ReadInfoToReport() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SendTestResult() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> GetProcessName() </li>
     *      <li> GetParameterBool() </li>
     *      </ul>
     * 
     * @return The result of reading the data items from the module.
     */
    virtual string ReportModuleInfo(void);
    /**
     * Check if the ignition is On.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Determine if the ignition is at least keyed on.  If the engine is running, this is also acceptable.  If the 
     * ignition is not keyed on, the operator will be prompted to turn ignition to on position.
     * <br>
     * If this function is run as a background test, it will first check to see if the Ignition is on.  If the iginition
     * is not On, it then checks to see if all the foreground tests are done.  It will continue to loop until either the 
     * foreground tests are complete, StatusCheck() fails, or the Ignition is on.  If the foreground tests are complete 
     * and the Ignition is not on, the function will attempt to lock the prompt server and will continue to loop as 
     * above.  Once the prompt server has been locked, the function acts as a foreground test would.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> IgnitionOnTimeout - Amount of time(ms) to wait for the operator to turn the ignition On. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between ignition On checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> IgnitionOn - Turn the ignition to the On position. </li>
     *      </ul>
     * 
     * <b> Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> SetStartTime() </li>
     *      <li> CheckCableConnect() </li>
     *      <li> IsIgnitionOn() </li>
     *      <li> IsForegroundTest() </li>
     *      <li> CheckForegroundLockPrompts() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> UnlockPrompts() </li>
     *      </ul>
     * 
     * @return Flag indicating if the ignition is in the on position.
     */
    virtual const bool CheckIgnitionOn(void);
    /**
     * Check if the engine is running.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Determine if the engine is running.  This will attempt to read the engine RPM.  The engine RPM must be at least 
     * at the Minimum Engine Idle RPM parameter for the engine to be running.  If the engine is not running, the
     * operator will be prompted to start engine.
     * <br>
     * If this function is run as a background test, it will first check to see if the engine is running. If hte engine
     * is not running, it then checks to see if all foreground tests are done.  It will continue to loop until either the 
     * foreground tests are complete, StatusCheck() fails, or the engine is running.  If the foreground tests are 
     * complete and the engine is not running, the function will attempt to lock the prompt server and will continue to 
     * loop as above.  Once the prompt server has been locked, the function acts as a foreground test would.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> EngineRunningTimeout - Amount of time(ms) to wait for the operator to start the engine. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between engine running checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> EngineRunning - Start the engine. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> SetStartTime() </li>
     *      <li> CheckCableConnect() </li>
     *      <li> IsEngineRunning() </li>
     *      <li> IsForegroundTest() </li>
     *      <li> CheckForegroundLockPrompts() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> UnlockPrompts() </li>
     *      </ul>
     * 
     * @return Flag indicating if the engine is running.
     */
    virtual const bool CheckEngineRunning(void);
    /**
     * Wait for the engine off and iginition on.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Prompt the operator to turn off the engine and place the ignition in the "On" position.  This function will then 
     * wait for a specified amount of time for the engine to be off and the ignition to be in the on position.
     * <p>
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> EngineOffIgnitionOn - Turn off the engine and place the ignition in the On position. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between engine RPM and ignition position checks. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> IsIgnitionOn() </li>
     *      <li> IsEngineRunning() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @return Result of turning off the engine and placing the ignition in the On position.
     */
    virtual const string WaitForEngineOffIgnitionOn(void);
    /**
     * Check if the ignition is On.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Determine if the ignition is in the On position.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> IsIgnitionOn() </li>
     *      </ul>
     * 
     * @return Flag indicating if the ignition is in the on or run position.
     */
    virtual const bool IsIgnitionOn(void);
    /**
     * Check if the engine is running.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Determine if the engine is running.  Engine RPM must be higher than the minimum engine running RPM parameter for
     * the engine to be considered running.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> MinimumEngineRunningRPM - Minimum engine RPM for the engine to be considered running. </li>
     *     </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> IsEngineRunning() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      </ul>
     * 
     * @return Flag indicating if the engine is running.
     */
    virtual const bool IsEngineRunning(void);
    /**
     * Abort test processing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Abort the current test processing.  Any running background data monitors will also be commanded to stop.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StopBackgroundMonitors() </li>
     *      <li> GenericTC::Abort() </li>
     *      </ul>
     */
    virtual void Abort(void);
    /**
     * Store module faults flag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store a flag indicating the presence of module faults.
     * <p>
     * @param dtcsInModule Flag indicating if DTCs have been recorded in the module.
     */
    virtual void DTCsInModule(const bool &dtcsInModule);
    /**
     * Check if module faults have been detected.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the flag indicating if DTCs were detected in the module.
     * <p>
     * @return Flag indicating if DTCs were recorded in the module.
     */
    virtual const bool& DTCsInModule(void);
    /**
     * Wait for engine off and key on.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The operator will be instructed to turn off the engine and place the ignition in the On position.
     * <p>
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> Description - Description of the test step. </li>
     *     </ul>
     * 
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> AlwaysWaitForEngineOffKeyOn - Flag used to determine if this test step should always be performed 
     *                                        regardless of the ShortCircuitTestStep flag. </li>
     *     </ul>
     * 
     * <b>Fault Tags:</b>
     *     <ul>
     *     <li> TestStepTimeout - Timeout waiting for the operator to turn off the engine and place the ignition in the 
     *                            On position. </li>
     *     <li> StatusCheckFailure - Testing has been aborted. </li>
     *     <li> CommunicationFailure - Could not read the engine RPM or ignition status from the module. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetParameterInt() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> WaitForEngineOffIgnitionOn() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of waiting for the operator to turn off the engine and place the ignition in the On position.
     */
    virtual string WaitForEngineOffKeyOn(void);
    /**
     * Load static test results.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The test results from the static test session will be requested and loaded.  This will enable the test component 
     * to skip the test steps that were completed using the static test station.  The test results will also be sent to 
     * the TestResultServer for inclusion into the complete test result file.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> StaticTestResult - System tag used to read the static test result for a specified VIN. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> SystemRead() </li>
     *      <li> GetComponentName() </li>
     *      <li> SetCurrentTestStep() </li>
     *      <li> SetTestStepResult() </li>
     *      <li> GetTestStepName() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     */
    virtual void LoadStaticTestResults(void);
    /**
     * Prompts driver for a requested state.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to satisfy the requested state.  The state will
     * then be monitored to ensure the operator has satisfied the requested state.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      </ul>
     * 
     * @param requestedState
     *               Desired state.
     * @param promptForState
     *               Prompt box in which to display the operator instructions.
     * @param currentStateTag
     *               The message tag to read the current state from the module.
     * @param minimumSuccessiveReads
     *               The amount of successive reads where the current state matches the requested state.
     * @param successiveReadDelay
     *               The delay in milliseconds between successive reads.
     * 
     * @return Flag indicating if the driver was able to satisfy the requested state.
     */
    bool RequestStateByPrompt(string requestedState, 
                              string promptForState,
                              string currentStateTag, 
                              INT32 minimumSuccessiveReads, 
                              INT32 successiveReadDelay);
    /**
     * Cycle the ignition off and then on.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The operator will be instructed to turn off the engine and the ignition.  once the key off position is verified,
     * the operator will be instructed to switch the ignition to the On position.  The operator will finally be instructed
     * to start the engine.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> StartEnginePrompt - Amount of time(ms) to wait for the operator to start the engine. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> EngineRunning - Start the engine. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> EngineOffIgnitionOnTimeoutFailure - Timeout waiting for engine off and iginition on. </li>
     *      <li> EngineOffIgnitionOffTimeoutFailure - Timeout waiting for engine off and ignition off. </li>
     *      <li> StatusCheckFailure - System status precludes further testing. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while waiting for the ignition to be cycled. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> WaitForEngineOffIgnitionOff() </li>
     *      <li> WaitForEngineOffIgnitionOn() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return Result of keying the ignition off and then back to the On position and restarting the engine.
     */
    virtual string KeyOffEngineOffKeyOn(void);
    /**
	 * Prompt the operator to turn off the engine and place the ignition in the "Off" position.  
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator may be prompted to shift to Park if the parameter is set.  The operator will then be instructed to 
     * place the ignition in the Off position.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ShiftToParkDisplayPrompt - Amount of time(ms) to display the Shift to Park prompt. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between ignition position checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> EngineOffPromptForPark - Flag indicating if the operator should be instructed to place the transmission
     *                                    in Park prior to placing the ignition in the Off position. </li>
     *      <li> EngineOffIgnitionOffWaitTime - Amount of time(ms) to wait after the ignition is in the Off position. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToPark - Shift the transmission to the Park position. </li>
     *      <li> EngineOffIgnitionOff - Place the ignition in the Off position. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GenericTCTemplate::IsIgnitionOn() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @return Result of waiting for the operator to place the ignition in the Off position.
	 */
	virtual const string WaitForEngineOffIgnitionOff(void);
    /**
     * Flag to indicate that there is a communication failure in the TestComponent.
     */
    bool m_communicationFailure;

    /** The module interface used for vehicle communication. */
    ModuleType m_vehicleModule;
    /**
     * Variable to make sure that the initialize hook method is called.
     */
    bool m_initializeHookCalled;
    /**
     * Variable to make sure that the initialize method is called.
     */
    bool m_initializeCalled;

    /** List of faults to be ignored during testing. */
    XmlNodeMap m_ignoreFaults;

    /** List of valid module part numbers. */
    XmlNodeMap m_validPartNumbers;

    /** Module data items to record. */
    XmlNodeMap m_moduleDataItems;

    /** partNumber character Positions to check. */
    XmlNodeMap m_partNumberPositionComparison;

    /** List of part numbers for CheckPartNumbers Test. */
    XmlNodeMap m_partNumbersToCheck;

    /** Path to the protocol file to use for module communication. */
    std::string m_protocolFile;

    /** Time of the last modification to the protocol file. */
    time_t  m_protocolFileTime;

    /** Flag to tell if an abort is currently being processed. */
    bool m_abortCalled;

private:
    /** Flag indicating if faults are present in the vehicle module. */
    bool m_faultsInModule;

    /**
     * Communication object to retrieve static test results from the static tester server.
     * 
     * @see IBepCommunication
     * @see StaticTesterServer
     */
    IBepCommunication *m_staticTesterServerComm;
};
#endif

