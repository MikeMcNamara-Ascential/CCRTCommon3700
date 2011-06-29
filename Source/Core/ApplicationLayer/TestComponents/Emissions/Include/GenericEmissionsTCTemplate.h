//******************************************************************************
// Description:
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Emissions/Include/GenericEmissionsTCTemplate.h $
// 
// 1     11/01/07 2:53p Derickso
// Updated comments to doxygen format.
// 
// 1     10/15/07 10:11a Derickso
// Updated header comments to doxygen format.
// 
// 14    1/17/07 3:32p Rwiersem
// Changes for the 07011901 core release:
// 
// - Made the test class a friend.
// - Added Activate() and Deactivate().
// - Added ReloadConfiguration().
// - Added NeedToRunHydraulicCoolingFan().
// 
// 13    10/25/06 3:56p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added WarmInitialize().  This method is used to keep the test
// component running when a new drive curve starts.
// 
// 12    5/08/06 7:09p Cward
// Added ability to iterate through list of valid ETC types
//
// 11    11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 10    4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 10     4/04/05 1:01a Derickso
// Changed from using the engine size to determine if the vehicle
// is equipped with ETC (Electronic Throttle Control) to looking at the
// build data.  A member variable and system read was added to determine
// and store the build data.
//
// 9     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 8     9/01/04 9:11p Cward
// Added functions to start and stop the hydraulic cooling fans. removed
// inlines from function prototypes.
//
// 7     8/18/04 11:16p Cward
// changed template class include to cpp from h
//
// 6     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 5     5/25/04 3:39p Cward
// Various improvements and updates
//
// 4     3/04/04 4:26p Cward
// Removed Short Circuit Test Step method, Changed setting of tpsStatus in
// Resume Background Monitors. Changed Conditional logic for skipping
// tests steps.
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 7     11/13/03 6:12p Derickso
// Latest updates from testing at JTE.
//
// 6     10/28/03 7:26a Derickso
// GenericTCTemplate no longer has the ShortCircuitTestStep()
// method.  It now resides in GenericTC.
//
// 5     10/20/03 9:18a Derickso
// Latest checnges after debugging and testing with a vehicle.
//
// 4     9/22/03 4:44p Derickso
// Updated CommandTestStep to check the return status from
// StatusCheck against BEP_STATUS_SUCCESS.
//
// 3     9/08/03 7:18a Derickso
// Moved IsEngineRunning() and CheckEngineRunning() to
// GenericTCTemplate class.
//
// 2     8/26/03 8:05a Derickso
// Updated for latest directory structure.
// Compiles, not tested.
//
// 1     8/21/03 8:59a Derickso
// 05WK CCRT Project
//
// 11    8/20/03 6:47a Derickso
// Recombined cpp and h files.  Compiler does not support implementation
// being seperate for template classes.
//
// 10    8/19/03 9:58a Derickso
// Updated class names adding Template to end of Generic name. This will
// avoid confilicts with earlier software.
//
// 7     8/13/03 3:33p Derickso
// Renaming to conform to new conventions.
//
// 6     8/12/03 3:30p Derickso
// Split implementation into cpp file.
//
// 5     7/08/03 10:22a Derickso
// Updated parameter macros to be more readable.
// Latest version.  Compiles, not tested.
//
// 4     6/09/03 8:04a Derickso
// Corrected compiler errors.
// Compiles, not tested.
//
// 3     5/28/03 9:29a Derickso
// Defined IsIdleRPM method.
//
// 2     5/28/03 8:39a Derickso
// Converted to template.
//
// 1     5/27/03 3:31p Derickso
// Periodic backup.
//
// 1     5/23/03 5:02p Derickso
// Periodic backup
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericEmissionsTCTemplate_h
#define GenericEmissionsTCTemplate_h
//==============================================================================

#include "GenericTCTemplate.cpp"
#include "DataAnalysis.h"


/**
 * Generic Emissions Test Component.<br>
 * This test component will test all the generic emissions feautres
 * common to most engine controllers.  Specifics have been left out so
 * this can serve as a base emissions test component for multiple module
 * types.
 *
 * @author Dan Erickson.
 * @see GenericTCTemplate
 * @see GenericEmissionsModule
 */
template <class VehicleModuleType>
class GenericEmissionsTCTemplate : public GenericTCTemplate<VehicleModuleType>
{
    friend class CommonEmissionsTest;

public:
    /**
     * Valid engine sizes.
     */
    const string engine57LV8;
    const string engine47LV8;
    const string engine37LV6;
    /**
     * Class constructor.
     */
    GenericEmissionsTCTemplate();

    /**
     * Class destructor.
     */
    virtual ~GenericEmissionsTCTemplate();
    
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
     *      <li> GenericTCTemplate::Deactivate() </li>
     *      </ul>
     */
    virtual void Deactivate(void);
    /**
     * Execute the specified test step.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the 
     * system for each test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     * 
     * <b>Test Step Functions Called:</b><br>
     *      <ul>
     *      <li> CheckIdleSpeed() </li>
     *      <li> CheckUplineRegisters() </li>
     *      <li> CommandSecurityUnlock() </li>
     *      <li> DetermineModuleConfig() </li>
     *      <li> DeterminePreviousPass() </li>
     *      <li> EvaluateOdometerIntegrity() </li>
     *      <li> MonitorMAPSensor() </li>
     *      <li> StartHydraulicCoolingFan() </li>
     *      <li> StopHydraulicCoolingFan() </li>
     *      <li> ProgramEOLStatus() </li>
     *      <li> StartSwitchMonitor() </li>
     *      <li> StartTPSMonitor() </li>
     *      <li> StopSwitchMonitor() </li>
     *      <li> StopTPSMonitor() </li>
     *      <li> VerifyModuleType() </li>
     *      <li> ResetStatusRegisters() </li>
     *      <li> WaitForEngineRunning() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GenericTCTemplate::CommandTestStep() </li>
     *      </ul>
     * 
     * @param value  The value that was commanded by the test sequencer.
     * 
     * @return The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

    /**
     * Determine if the vehicle is at an idle state.
     * <p><b>Cateogry:</b> Utility
     * <p><b>Description:</b><br>
     * The vehicle conditions will be checked to determine if the vehicle is currently at an idle state.
     * The following conditions must be met:
     * <ul>
     * <li> Cable Connected </li>
     * <li> Zero Speed </li>
     * <li> Engine Idle </li>
     * <li> Closed Throttle </li>
     * </ul>
     * If any of the idle conditions are not satisfied, the operator will be prompted to perform the necesary actions
     * to place the vehicle in the correct state.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> CheckEngineRunning() </li>
     *      <li> CheckClosedThrottle() </li>
     *      </ul>
     *
     * @return Flag indicating if the vehicle is at idle conditions.
     */
    virtual const bool VehicleAtIdleState(void);

    /**
     * Determine if the vehicle is connected.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The ignition will be checked to determine if the diagnostic cable is connected and the ignition is in the On 
     * position.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> CheckIgnitionOn() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Flag indicating if the vehicle is connected.
     */
    virtual const bool VehicleConnected(void);

    /**
     * <b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the flag indicating if the vehicle is equipped with a redundant brake switch.
     * <p>
     * @param testRedundantBrake
     *               Flag indicating if the vehicle has a redunadant switch.
     */
    virtual void SetTestRedundantBrakeSwitch(const bool &isRedundantBrakeEquipped);

    /**
     * <b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the Redundant brake Switch Equipped flag.
     * <p>
     * @return Flag indicating if the vehicle is equipped with a redundant brake switch.
     */
    virtual const bool& IsRedundantBrakeSwitchEquipped(void);

    /**
     * <b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the flag indicating if the vehicle is equipped with a clutch Upstop switch.
     * <p>
     * @param testClutchUpstop
     *               Flag indicating if the vehicle is equipped with a clutch Upstop switch.
     */
    virtual void SetTestClutchUpstopSwitch(const bool &testClutchUpstop);

    /**
     * <b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the Clutch Upstop Switch Equipped flag.
     * <p>
     * @return Flag indicating if the vehicle is equipped with a clutch Upstop switch.
     */
    virtual const bool& IsClutchUpstopSwitchEquipped(void);
    /**
     * Check for valid ETC type.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The vehicle build data will be analyzed to determine if the vehicle is equipped with a valid ETC type.
     * A flag indicating if the vehicle is equipped with a valid ETC type will be stored in a member variable.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> BroadcastSpeedControlType - System tag to read the ETC build option from the vehicle build data. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetETCEquipped() </li>
     *      </ul>
     */
    void CheckForValidETCType(void);
    /**
     * Reinitialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reinitialize using the already loaded configuration file. This is intended to be used when a component is 
     * activated and there have been no modifications to any configuration files used by the component.
     * Member variables that store status during the test will also be reset to initial values.  The build data will
     * also be analyzed to determine if a ETC type is installed in the vehicle.  The previous pass status flag will
     * also be reset to indicate this vehicle has not passed roll testing.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ValidETCSpeedControlTypes - List of valid ETC types. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ETCBuildOption - System tag to use for retieving the ETC option from the build data. </li>
     *      <li> ETCEquippedValue - Value indicating the vehicle is equipped with ETC. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTCTemplate::WarmInitialize() </li>
     *      <li> CheckForValidETCType() </li>
     *      <li> SetETCEquipped() </li>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetPreviousPassStatus() </li>
     *      </ul>
     */
    virtual void WarmInitialize();

protected:
    /**
     * Stop any background monitors that have been started.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * All background monitors will be commanded to stop running.
     * <br><b><i>Note:</i></b>  No checking is performed to ensure the background monitor has observed all states it 
     *                          has monitored. This method is intended to stop any monitors in the event a test abort
     *                          was received. This will prevent monitors from attempting communications after the 
     *                          vehicle is disconnected.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StopTPSMonitor() </li>
     *      <li> StopSwitchMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTCTemplate::StopBackgroundMonitors() </li>
     *      </ul>
     *
     * @return Result of stopping the monitors.
     */
    virtual string StopBackgroundMonitors(void);

    /**
     * Restart any background monitors that were stopped.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Restart all background monitors that may have been suspended using the StopBackgroundMonitors function.
     * <br><b><i>Note:</i></b>  If any background monitors perform any special setup or store initial values, care 
     *                          should be taken to not lose the initial values that were store when the monitor was 
     *                          initially started.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadThrottlePositionNonETC - Message tag to use when reading the throttle position. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StartTPSMonitor() </li>
     *      <li> StartSwitchMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsPreviousPass() </li>
     *      <li> GetDataTag() </li>
     *      <li> GenericTCTemplate::ResumeBackgroundMonitors() </li>
     *      </ul>
     *
     * @return Result of restarting the background monitors.
     */
    virtual string ResumeBackgroundMonitors(void);

    /**
     * Perform GenericEmissionsTCTemplate intitialization.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This method overloads the base class to allow the GenericEmissionsTCTemplate test component to initialize any
     * data sepcific to its needs.
     * <p>
     * <b>Test Component Parameters:</b><br>
     *      <ul>
     *      <li> ResetEOLStatusBytes - List of EOL status bytes to be reset upon successful test completion. </li>
     *      <li> ResetPCMRegisters - List of PCM registers to be reset at the end of a successful test. </li>
     *      <li> DevelopmentRegisters - List of registers and values to be reset for development testing. </li>
     *      <li> SRIInitialSetting - Initial SRI register settings. </li>
     *      <li> SRIFirstTumble - SRI register settings after first tumble. </li>
     *      <li> SRISecondTumble - SRI register settings after second tumble. </li>
     *      <li> ManualTransmissionTypes - List of valid manual transmission types. </li>
     *      <li> ValidETCSpeedControlTypes - List of valid ETC types. </li>
     *      <li> SetRedundantBrakeSwitchEquipped - Flag indicating if the vehicle is equipped with a redundant brake switch. </li>
     *      <li> SetClutchUpstopSwitchEquipped - Flag indicating if the vehicle is equipped with a clutch upstop switch. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ETCBuildOption - System tag used to retrieve the ETC type from the vehicle build data. </li>
     *      <li> ETCEquippedValue - Value indicating the vehicle is equipped with a valid ETC type. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericTCTemplate::InitializeHook() </li>
     *      <li> CheckForValidETCType() </li>
     *      <li> SetETCEquipped() </li>
     *      <li> SystemRead() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameterBool() </li>
     *      <li> SetTestRedundantBrakeSwitch() </li>
     *      <li> SetTestClutchUpstopSwitch() </li>
     *      </ul>
     * 
     * @param config The configuration data to use for initializing the test component.
     */
    virtual void InitializeHook(const XmlNode *config);
    /**
     * Reload the configuration data.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The configuration data will be reloaded from file.  The data lists will first be cleared as they will
     * be reloaded from file.  The base class will then be called to complete the reload.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTCTemplate::WarmInitialize() </li>
     *      </ul> 
     */
    virtual void ReloadConfiguration(void);

    /**
     * Check if the vehicle has previously passed rolls testing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The flag indicating if th evehicle has previously passed roll testing will be returned to the calling function.
     * <p>
     * @return Flag indicating if the vehicle has previously passed roll testing.
     */
    virtual const bool IsPreviousPass(void);

    /**
     * Determine if the engine is at idle speed.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The engine RPM will be read from the module.  The RPM will then be checked against the idle limits set in the
     * parameters to determine if the engine is at idle.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> MaximumIdleRPM - Maximum allowable idle RPM. </li>
     *      <li> MinimumIdleRPM - Minimum allowable idle RPM. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> EngineRunningMessage - Message tag to use for reading the engine RPM from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Data items: EngineRunningMessage </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> GetParameterInt() </li>
     *      </ul>
     *
     * @return Flag indicating if the engine is at idle RPM.
     */
    virtual const bool IsIdleRPM(void);

    /**
     * Determine if the throttle is closed. 
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The current throttle position will be read from the module and published to the system for display purposes.
     * The TPS value will then be compared to parameters to determine if the throttle is closed.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ClosedThrottleVoltage5.7L - Maximum closed throttle voltage for 5.7L/ETC engines. </li>
     *      <li> ClosedThrottleVoltage - Maximum closed throttle voltage for all other engines. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ThrottleClosedVoltageMessage5.7L - Message tag to use for reading throttle position from 5.7L/ETC engines. </li>
     *      <li> ReadThrottlePositionNonETC - Message tag to use for reading throttle position from all other engines. </li>
     *      <li> TPSValue - System tag for reporting the current TPS value to the system. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tags: ThrottleClosedVoltageMessage5.7L, ReadThrottlePositionNonETC </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetParameterFloat() </li>
     *      </ul>
     * 
     * @return Flag indicating if the throttle is closed.
     */
    virtual const bool IsThrottleClosed(void);

    /**
     * Determine if the throttle is closed. 
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The current throttle position will be read from the module and published to the system for display purposes.
     * The TPS value will then be compared to parameters to determine if the throttle is closed.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ClosedThrottleVoltage5.7L - Maximum closed throttle voltage for 5.7L/ETC engines. </li>
     *      <li> ClosedThrottleAtoDVoltage - Maximum closed throttle voltage for all other engines. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ThrottleClosedVoltageMessage5.7L - Message tag to use for reading throttle position from 5.7L/ETC engines. </li>
     *      <li> ThrottleClosedVoltageMessage - Message tag to use for reading throttle position from all other engines. </li>
     *      <li> TPSValue - System tag for reporting the current TPS value to the system. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tags: ThrottleClosedVoltageMessage5.7L, ThrottleClosedVoltageMessage </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetParameterFloat() </li>
     *      </ul>
     * 
     * @return Flag indicating if the throttle is closed.
     */
    virtual const bool IsThrottleAtoDClosed(void);

    /**
     * Determine if the transmission is in neutral.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The neutral cell ID indicates if the transmission is in neutral.  The neutral cell ID will be read from the module
     * and compared against parameter values to determine if the transmission is in neutral.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> NeutralCellID - Neutral cell id value indicating the transmission is in neutral. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadNeutralCellID - Message tag to use for reading the Neutral Cell ID from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message tags: ReadNeutralCellID </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     *
     * @return Flag indicating the transmission is in neutral.
     */
    virtual const bool IsTransmissionInNeutral(void);

    /**
     * Determines if the Air Conditioning is Off.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The a/c status will be read from the module to determine if the a/c is switched off.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadAirConditionerStatus - Message tag to use for reading the a/c status from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tags: ReadAirConditionerStatus </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @return Flag indicating if the air conditioning is off.
     */
    virtual const bool IsAirConditioningOff(void);
    /**
     * Determine if the vehicle is equipped with a valid manual transmission.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The transmission type will be retrieved from the vehicle build data.  The transmission type will then be compared
     * against the list of valid manual transmission types loaded from the config file during initialization.  If the 
     * thransmission type is found in the list, then the vehicle is equipped with a manual transmission.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TransmissionType - System tag to use for retrieving the transmission type from the build data. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     *
     * @return Flag indicating if the vehicle is equipped with a manual transmission.
     */
    virtual const bool IsManualTransmission(void);

    /**
     * Determine if the throttle is closed.  
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The throttle position will be checked to determine if it is in a closed position. If the throttle is not closed,
     * the operator will be promted to remove foot from throttle.  TPS must read at or below the Closed Throttle Position 
     * parameter.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) between throttle reads. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ThrottleClosed - Remove foot from throttle. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> IsThrottleClosed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     *
     * @return Flag indicating if the throttle is closed.
     */
    virtual const bool CheckClosedThrottle(void);

    /**
     * Determine if all conditions are correct to measure the AIS value.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The vehicle state will be checked to ensure all idle conditions required to properly read the AIS position are met.
     * If any of the required conditions are not met, the operator will be prompted to achieve those conditions.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AISTransmissionInNeutral - Flag to indicate if the transmission should be in Neutral for the AIS test. </li>
     *      <li> SkipNeutralCheck - Flag indicating the transmission gear check should be skipped. </li>
     *      <li> SkipAirConditioningCheck - Flag indicating if the a/c status should be verified. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToNeutral - Shift the transmission to Neutral. </li>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      <li> TurnOffAC - Turn off the air conditioning. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> VehicleAtIdleState() </li>
     *      <li> SetStartTime() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetParameterInt() </li>
     *      <li> IsTransmissionInNeutral() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> IsAirConditioningOff() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     *
     * @return Flag indicating all idle conditions have been satisfied.
     */
    virtual const bool AISIdleConditions(void);

    /**
     * Ensure AIS idle conditions are satisfied at the end of the AIS test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Ensure all AIS idle conditions are still satisfied at the end of the AIS test.
     * <br><b><i>Note:</i></b>  This is different than AISIdleConditions in that we do not want to wait for each of the 
     *                          required AIS idle state items.  It should not have changed, so we just check each item 
     *                          once to ensure it remains satisfied.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AISTransmissionInNeutral - Flag indicating if the transmission is required to be in Neutral for the 
     *                                      AIS test. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsTransmissionInNeutral() </li>
     *      <li> IsIdleRPM() </li>
     *      <li> IsThrottleAtoDClosed() </li>
     *      <li> IsAirConditioningOff() </li>
     *      <li> GetParameterBool() </li>
     *      </ul>
     *
     * @return Flag indicating the AIS Idle conditions are still satisfied.
     */
     virtual bool FinalAISIdleCondtions(void);

     /**
      * Wait for the operator to start the vehicle.
      * <p><b>Category:</b> Test Step
      * <p><b>Description:</b><br>
      * The engine RPM will be checked to determine if the engine is running.  The engine RPM must fall within idle
      * parameters in order for the engine to be declared running.  After the engine is running, the idle RPM will
      * be monitored for a bit to ensure it was not a false start condition.
      * <p>
      * <b>Test Step Info:</b>
      *         <ul>
      *         <li> Description - Description of the test step. </li>
      *         </ul>
      * 
      * <b>Test Component Parameters:</b>
      *         <ul>
      *         <li> MaximumIdleBeforeRunningChecks - Maximum nuber of times to check the idle speed to determine if the
      *                                               engine is running. </li>
      *         <li> IdleBeforeRunningWaitTime - Amount of time(ms) to wait between RPM reads. </li>
      *         </ul>
      * 
      * <b>Fault Tags:</b>
      *         <ul>
      *         <li> EngineRunningTimeout - Timeout waiting for the engine to be running. </li>
      *         </ul>
      * 
      * <b>Functions Called:</b>
      *         <ul>
      *         <li> GetTestStepInfo() </li>
      *         <li> GetComponentName() </li>
      *         <li> GetTestStepName() </li>
      *         <li> CheckEngineRunning() </li>
      *         <li> GetFaultCode() </li>
      *         <li> GetFaultDescription() </li>
      *         <li> SendTestResult() </li>
      *         <li> GetParameterInt() </li>
      *         <li> TimeRemaining() </li>
      *         <li> StatusCheck() </li>
      *         <li> IsIdleRPM() </li>
      *         </ul>
      * 
      * @return Result of the waiting for the engine to be running.
      */
     virtual string WaitForEngineRunning(void);

    /**
     * Update the EOL Status register with the test result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The specified test result will be recorded in <i>testBit</i> of the <i>statusRegister</i>.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> UpdateEEPROMTestBit() </li>
     *      </ul>
     *
     * @param statusRegister
     *                   EOL Status Register to update.
     * @param testBit    Bit to be updated.
     * @param testResult Test result to write to the module.
     * @return Result of the module update.
     */
    virtual string UpdateEOLStatusRegister(const UINT16 &statusRegister,
                                           const UINT16 &testBit,
                                           const string &testResult);

    /**
     * Setup the test component and vehicle for testing.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Once the diagnostic cable is connected, the parameters will be checked to determine if module intialization was
     * delayed during test component intialization.  Module initialization is typically delayed if the plant builds both
     * left and right hand drive vehicles.  When the diagnostic cable is connected, the system establishes a connection
     * to the correct vehicle interface box.  If module initialization was delayed, the module will now be commanded
     * to open its communication path to the vehicle interface device.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> DelayModuleInitialization - Flag indicating if module initialization was delayed during test component 
     *                                       intialization. </li>
     *      <li> AbortIgnitionNotOn - Flag indicating if the test should be aborted if the ignition is not in the On position. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> PCMCommFailure - Could not establish communications with the PCM. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> SoftwareFailure - Failed to initialize the module interface. </li>
     *      <li> VehicleNotConnected - Vehicle is not connected or the ignition is not in the On position. </li>
     *      <li> PreconditionTimeout - Timeout waiting for the cable to be connected. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> CheckCableConnect() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GenericTCTemplate::Setup() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> VehicleConnected() </li>
     *      <li> SendTestResult() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return Result of setting up the test component and vehicle for testing.
     */
    virtual string Setup(void);

    /**
     * Check part number for validity.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The base class will be called to validate the module part number.  The validation result can optionally be
     * recorded in the module based on the setting of parameter <i>SkipPartNumberResultUpdate</i>.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SkipPartNumberResultUpdate - Flag indicating if the part number verification result should be recorded
     *                                        in the module. </li>
     *      <li> PartNumberResultBit - Bit to use for recording the result of the part number verification. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> EOLPartNumberResultRegister - Parameter to use for the EOL part number register. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ModuleUpdateCommunicationFailure - Could not record the part number verification result in the module. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GenericTCTemplate::CheckPartNumber() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> IsPreviousPass() </li>
     *      <li> GetDataTag() </li>
     *      <li> UpdateEOLStatusRegister() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return Result of verifying the module part number and recording the result in the module.
     */
    virtual string CheckPartNumber(void);

    /**
     * Unlock the module security.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Security will be unlocked in the module.  Security must be unlocked in order for EOL status registers/bytes to
     * be updated with test results. 
     * <br><b><i>Note:</i></b> If too many consecutive failed attempts are made to unlock security, the module will be
     *                         locked out and cause a no-start condition in the vehicle.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SecurityUnlockAttempts - Number of attempts to make for unlocking module security. </li>
     *      <li> SecurityUnlockDelay - Amount of time(ms) to wait between failed unlock attempts. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> SecurityUnlockFailed - Failed to unlock module security. </li>
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
     * @return Result of unlocking module security.
     */
    virtual string CommandSecurityUnlock(void);

    /**
     * Determine the previous pass status of the module.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The previous pass status will be read from the module.  If the vehicle has previsouly passed emissions testing,
     * typically only a subset of tests are required.  A flag will be set in a member variable to indicate the previous
     * emissions pass status of the vehicle.  The field mode status can also be optionally checked to verify the
     * vehicle was processed correctly prior to arriving at rolls.  If the module is marked as Ok To Ship, then it should
     * also be in Field mode.  If the vehicle has not previsouly passed emissions testing, the module should not be marked 
     * as Ok To Ship and should be in Plant mode.  If the module status does not match either of these scenarios, testing
     * will be aborted so the vehicle can be reprocessed.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CheckFieldModeBit - Flag indicating if the field mode bit should also be checked. </li>
     *      <li> AbortForBadStatus - Flag indicating if all testing should be aborted if the previous pass status is not
     *                               consistent with the field mode status. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> PreviousPcmFailure - Previous pass status not consistent with field mode status. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read previous pass status or field mode status from the module. </li>
     *      <li> PreviousPcmFailure - Previous pass status not consistent with field mode status. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions: </b>
     *      <ul>
     *      <li> GetInfo() - Message tags: IsPreviousPass, IsInFieldMode </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsPreviousPass() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SetPreviousPassStatus() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return Result of determining the previous emissions pass status.
     */
    virtual string DeterminePreviousPass(void);

    /**
     * Verify that the module type matches the type specified in the vehicle build data.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The module type will be read from the module.  It will then be compared against the expected modue type stored in
     * parameter <i>ModuleType</i>.  If the module type read from the module does not match the parameter, the test 
     * sequence will be aborted.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ModuleType - Expected module type being tested. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadModuleType - Message tga to use for reading the module type from the module. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> IncorrectModuleType - Module type is not valid according to parameters. </li>
     *      <li> CommunicationFailure - Could not read module type from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to read the module type from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tags: ReadModuleType </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameter() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     * 
     * @return Result of validating the module type.
     */
    virtual string VerifyModuleType(void);

    /**
     * Read and store the configuration of the vehicle.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The engine configuration will be read from the module.  This data includes:
     * <ul>
     * <li> Engine volume and configuration </li>
     * <li> Type of emissions </li>
     * <li> Emissions model year </li>
     * <li> Vehicle Type </li>
     * </ul>
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> EngineConfiguration - Message tag to use for reading the engine configuration. </li>
     *      <li> VehicleType - Message tag to use for reading the vehicle type from the module. </li>
     *      <li> EmissionsModelYear - Message tag to use for reading the emissions model year from the module. </li>
     *      <li> EmissionsConfiguration - Message tag to use for reading the emissions configuration from the module. </li>
     *      <li> EngineSize - System tag to use for publishing the engine size to the system. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read engine configuration data from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message tags: EngineConfiguration, VehicleType, EmissionsModelYear, EmissionsConfiguration </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return Result of gathering the engine configuration data.
     */
    virtual string DetermineModuleConfig(void);

    /**
     * Verify upline registers are set correctly.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The Upline registers will be checked to ensure all testing and processing prior to rolls testing is complete.
     * The upline register contents will be compared to parameter <i>UplineTestComplete</i>.  The parameter serves as 
     * a mask to determine if any of the upline tests have not passed.  Typical upline test items include VIN programming
     * and odometer initialization.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> UplineTestComplete - Mask to apply to the module data to determine if all upline testing has completed. </li>
     *      <li> VINUplineComplete - Mask to apply to the module data to determine if the VIN has been programmed. </li>
     *      <li> OdometerUplineComplete - Mask to apply to the module data to determine if the odometer has been initialized. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read the upline register from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while reading the upline register from the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message Tags: ReadUplineRegister </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> IsPreviousPass() </li>
     *      <li> GetParameter() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * @return Result of verifying all upline testing has successfully completed.
     */
    virtual string CheckUplineRegisters(void);

    /**
     * Update the rolls affected bits in the EOL status registers to a "Not Done" condition.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The EOL status bits the roll test software updates during the test sequence will be reset to a "Not Done" status.
     * If the vehicle testing is aborted, the technicians in the repair bay will be able to determine which tests
     * have been completed.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> CommunicationFailure - Could not read/write the EOL status registers. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while updating the EOL status register/bytes in the module. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> IsPreviousPass() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of resetting the rolls affected EOL status bytes.
     */
    virtual string ProgramEOLStatus(void);

    /**
     * Verify the SRI (Service Reminder Indicator) registers have a valid bit pattern.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The odometer register will be read from the module.  The values will then be analyzed to verify the registers
     * are updating properly.  Typically, the register update in a round robin sequence.  The exceptions to this are the
     * initial setting and the first two tumbles.  These three special cases will be checked against parameters.
     * The current odometer reading will then be stored for use in verifying the odometer RAM integrity.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> InitialSRIOdometerRegisterUsed - Current odometer register with initial SRI setting. </li>
     *      <li> FirstTumbleSRIOdometerRegisterUsed - Current odometer register with the first tumble SRI setting. </li>
     *      <li> SecondTumbleSRIOdometerRegisterUsed - Current odometer register with the second tumble SRI setting. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> SRIRegistersInvalid - SRI registers are not set up correctly. </li>
     *      <li> CommunicationFailure - Could not read the SRI registers from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to read the SRI registers. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message tags: ReadOdometerRegisters </li>
     *      </ul>
     *      
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsPreviousPass() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> StoreCurrentEEROMOdometerReading() </li>
     *      <li> GetCurrentOdometerEEROMReading() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @return Result of validating the odometer registers are set correctly.
     */
    virtual string EvaluateOdometerIntegrity(void);

    /**
     * Verify the MAP sensor is functioning properly.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The MAP sensor will be checked to ensure it is functioning correctly.  The MAP sensor must be checked with the
     * throttle closed and the engine at idle RPM.  The vehicle will first be checked to ensure the throttle is closed 
     * and the RPM is at idle.  The MAP sensor will then be read from the module and checked to ensure it is within
     * acceptable parameters.  The result of the test will then be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> MinimumMAPReading - Minimum acceptable MAP sensor reading. </li>
     *      <li> MapSensorResultBit - Bit to use for recording the result of the MAP sensor verification. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> EOLMAPSensorResultRegister - Parameter to use for the MAP sensor result register. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> MapSensorThrotlleNotCorrect - Throttle not closed. </li>
     *      <li> MapSensorIdleRPMNotCorrect - Engine RPM not at idle. </li>
     *      <li> MAPSensorTooLow - MAP sensor reading is too low, possible vaccuum leak. </li>
     *      <li> ModuleUpdateCommunicationFailure - Could not record the MAP sensor result in the module. </li>
     *      <li> CommunicationFailure - Could not read the MAP sensor from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while reading the MAP sensor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message tags: ReadMAPSensor </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsPreviousPass() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> IsThrottleAtoDClosed() </li>
     *      <li> IsIdleRPM() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetDataTag() </li>
     *      <li> UpdateEOLStatusRegister() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of verifying the MAP sensor is within proper limits.
     */
    virtual string MonitorMAPSensor(void);

    /**
     * Start the hydraulic cooling fan.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The module will be commanded to start running the hydraulic cooling fan.  Running the hydraulic cooling fan 
     * helps to ensure coolant is distributed throughout the system so the engine does not overheat.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AlwaysRunCoolingFan - Flag indicating if the cooling fan should be commanded to start running regardless
     *                                 of the previous pass status. </li>
     *      <li> NoAbortForHydraulicCoolingFan - Flag indicating if the test should continue if the hydraulic cooling 
     *                                           fan cannot be started. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> StartHydraulicCoolingFanFailed - Could not start the hydraulic cooling fan. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to start the hydraulic cooling fan. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StartModuleKeepAlive() </li>
     *      <li> StartHydraulicCoolingFan() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> IsPreviousPass() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> GetParameterBool() </li>
     *      <li> NeedToRunHydraulicCoolingFan() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of starting the hydraulic cooling fan.
     */
    virtual string StartHydraulicCoolingFan(void);

    /**
     * Stop the hydraulic cooling fan.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The module will be commanded to stop running the hydraulic cooling fan.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> AlwaysRunCoolingFan - Flag to indicate if the hydraulic cooling fan has been started regardless of the 
     *                                 previous pass status of the vehicle. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> StopHydraulicCoolingFanFailed - Could not stop the hydraulic cooling fan. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while commanding the module to stop the hydraulic cooling fan. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StopModuleKeepAlive() </li>
     *      <li> StopHydraulicCoolingFan() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetParameterBool() </li>
     *      <li> NeedToRunHydraulicCoolingFan() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of stopping the hydraulic cooling fan.
     */
    virtual string StopHydraulicCoolingFan(void);

    /**
     * Start the switch monitor.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The background switch monitor will be started.  Test component configuration items will be checked to determine
     * if the redundant brake switch must be monitored as well as the clutch upstop switch.  After these settings are
     * completed, the switch monitor wil be started.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> StartSwitchMonitorFail - Could not start the switch monitor. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to start the switch monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> SetRedundantBrakeSwitchEquipped() </li>
     *      <li> SetClutchUpstopSwitchEquipped() </li>
     *      <li> StartSwitchMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> IsPreviousPass() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> IsRedundantBrakeSwitchEquipped() </li>
     *      <li> IsClutchUpstopSwitchEquipped() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of starting the background switch monitor.
     */
    virtual string StartSwitchMonitor(void);

    /**
     * Stop the switch monitor.
     * <p><b>Category:</b> Test Steo
     * <p><b>Description:</b><br>
     * The test component configuration will be checked to determine if the redundant brake switch and clutch upstop
     * switch results should be checked.  The results for each moinitored switch will be checked.  If any required
     * switch state has not been observed, the operator will be prompted to perform the actions required for the switch
     * to be onserved in the required states.  The switch monitor will then be stopped.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SkipParkNeutralSwitch - Flag indicating if the park/neutral switch check should be skipped on automatice
     *                                   transmission vehicles. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> StopSwitchMonitor - Could not stop the switch monitor. </li>
     *      <li> SwitchMonitorSwitchTestFailure - All switches not detected in all required states. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while stopping the switch monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> IsRedundantBrakeSwitchEquipped() </li>
     *      <li> IsClutchUpstopSwitchEquipped() </li>
     *      <li> StopSwitchMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> IsPreviousPass() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> CheckBrakeSwitchResults() </li>
     *      <li> CheckRedundantBrakeSwitchResults() </li>
     *      <li> CheckParkNeutralSwitchResults() </li>
     *      <li> CheckClutchSwitchResults() </li>
     *      <li> CheckClutchUpstopSwitchResults() </li>
     *      <li> IsManualTransmission() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return Result of stopping the switch monitor and analyzing the switch state results.
     */
    virtual string StopSwitchMonitor(void);

    /**
     * Start the TPS monitor.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The TPS monitor will be started.  The throttle position will first be checked to ensure the throttle is closed.
     * The TPS monitor will then be started to look for the required voltage change on the throttle position sensor.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ReadThrottlePositionNonETC - Message tag to use for reading the throttle position sensor from the module. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> InitialTpsThrotlleNotCorrect - Throttle was not closed prior to starting the TPS monitor. </li>
     *      <li> StartTPSMonitorFail - Failed to start the TPS monitor. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while attempting to start the TPS monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> StartTPSMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> IsPreviousPass() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetDataTag() </li>
     *      <li> IsThrottleAtoDClosed() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     *
     * @return Result of starting the TPS monitor.
     */
    virtual string StartTPSMonitor(void);

    /**
     * Stop the throttle position monitor.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * If the required delta TPS has not been observed, the operator will be instructed to accelerate to speed in an
     * attempt to detect the required delta TPS.  The delta TPS will be checked to ensure the required delta has been
     * observed.  The TPS monitor will then be stopped.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> ScanDelay - Amount of time(ms) to wait between delta TPS checks. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> RequiredDeltaTPS - Required change in TPS readings required to determine if the TPS is functioning correctly. </li>
     *      <li> DeltaTPSSpeedRange - Speed range for the operator to accelerate to if the require TPS delta has not been
     *                                observed. </li>
     *      <li> ThrottlePositionSensorResultBit - Bit to use for recording the delta TPS test result. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag for displaying the target speed range. </li>
     *      <li> EOLTPSResultRegister - Parameter to use for the TPS EOL status register. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> PressAcceleratorPedal - Briefly press the acclerator pedal. </li>
     *      <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *      </ul>
     *  
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> DeltaTPSFail - Failed to detect required TPS delta. </li>
     *      <li> ModuleUpdateCommunicationFailure - Could not record the delta TPS result in the module. </li>
     *      <li> StopTPSMonitor - Could not stop the TPS monitor. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetObservedDeltaTPS() </li>
     *      <li> GetInitialTPSReading() </li>
     *      <li> GetFinalTPSReading() </li>
     *      <li> StopTPSMonitor() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> IsPreviousPass() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> SetStartTime() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetDataTag() </li>
     *      <li> SystemWrite() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendTestResult() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> UpdateEOLStatusRegister() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return Result of stopping the TPS monitor and checking the delta TPS.
     */
    virtual string StopTPSMonitor(void);

    /**
     * Verify the idle speed on the vehicle is set properly.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The throttle will first be checked to ensure it is closed.  If the throttle is not closed, the operator will be
     * instructed to remove their foot from the throttle.  The engine RPM will then be checked to ensure it falls
     * within idle parameters.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> MinimumIdleRPM - Minimum acceptable idle RPM. </li>
     *      <li> MaximumIdleRPM - Maximum acceptable idle RPM. </li>
     *      <li> IdleRPMResultBit - Bit to use for recording the result of the idle RPM test. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> EOLIdleRPMResultRegister - Parameter to use for Idle RPM result register. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> IdleSpeedThrotlleNotCorrect - Timeout waiting for the throttle to be closed. </li>
     *      <li> EngineIdleRPMOutOfRange - Idle RPM is out of range. </li>
     *      <li> ModuleUpdateCommunicationFailure - Could not record the result in the module. </li>
     *      <li> CommunicationFailure - Could not read the engine RPM from the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while checking the idle RPM. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() - Message tags: ReadEngineIdleRPM </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsPreviousPass() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepResult() </li>
     *      <li> IsThrottleClosed() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetDataTag() </li>
     *      <li> UpdateEOLStatusRegister() </li>
     *      <li> SendSubtestResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @return Result of verifying the engine idle RPM falls within parameters.
     */
    virtual string CheckIdleSpeed(void);

    /**
     * Check the results of the brake switch test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The results of the brake switch on and brake switch off checks will be verified.  If a required state has not 
     * been detected, the operator will be prompted to achieve the desired state.  The results will then be recorded
     * in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> BrakeSwitchResultBit - Bit to use for recording the brake switch result in the module. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> EOLBrakeSwitchResultRegister - Parameter to use for the Brake Switch result register. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> BrakeSwitchOnFail - Failed to detect the brake switch in the On position. </li>
     *      <li> BrakeSwitchOffFail - Failed to detect the brake switch in the Off position. </li>
     *      <li> BrakeSwitchFail - Failed to detect the brake switch in both the On and Off positions. </li>
     *      <li> ModuleUpdateCommunicationFailure - Could not record the brake switch result in the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while processing the brake switch results. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> CheckBrakeSwitchOnResult() </li>
     *      <li> CheckBrakeSwitchOffResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetDataTag() </li>
     *      <li> UpdateEOLStatusRegister() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of the brake switch test.
     */
    virtual string CheckBrakeSwitchResults(void);

    /**
     * Check the results of the park/neutral switch test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The results of the park/neutral switch checks will be verified.  If a required state has not been detected, 
     * the operator will be prompted to achieve the desired state.  The results will then be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ParkNeutralSwitchResultBit - Bit to use for recording the park/neutral switch result in the module. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> EOLParkNeutralSwitchResultRegister - Parameter to use for park/neutral result register. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ParkNeutralSwitchOnFail - Did not detect the park/neutral switch in the On position. </li>
     *      <li> ParkNeutralSwitchOffFail - Did not detect the park/neutral switch in the Off position. </li>
     *      <li> ParkNeutralSwitchFail - Did not detect the park/neutral switch in either the On or Off position. </li>
     *      <li> ModuleUpdateCommunicationFailure - Could not record the park/neutral switch result in the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while processing the park/neutral switch results. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> CheckParkNeutralSwitchOnResult() </li>
     *      <li> CheckParkNeutralSwitchOffResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetDataTag() </li>
     *      <li> UpdateEOLStatusRegister() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of the park/neutral switch test.
     */
    virtual string CheckParkNeutralSwitchResults(void);

    /**
     * Check the results of the clutch switch test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The results of the clutch switch test will be verified.  If a state has not been detected, the operator will be 
     * prompted to achieve the desired state.  The results will then be recorded in the module.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ClutchSwitchResultBit - Bit to use for recording the result of the clutch switch test in the module. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> EOLClutchSwitchResultRegister - Parameter to use for recording the clutch switch results. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ClutchSwitchOnFail - Did not detect the clutch switch in the On position. </li>
     *      <li> ClutchSwitchOffFail - Did not detect the clutch switch in the Off position. </li>
     *      <li> ClutchSwitchFail - Did not detect the clutch switch in either the On or Off positions. </li>
     *      <li> ModuleUpdateCommunicationFailure - Could not record the clutch switch test result in the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while processing the clutch switch results. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> CheckClutchSwitchOnResult() </li>
     *      <li> CheckClutchSwitchOffResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetDataTag() </li>
     *      <li> UpdateEOLStatusRegister() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of the clutch switch test.
     */
    virtual string CheckClutchSwitchResults(void);

    /**
     * Check the result of the brake switch on test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of the brake switch on test will be checked.  If the brake switch was not detected in the On position,
     * the operator will be prompted to press the brake pedal.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between brake switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> PressBrakePedal - Press the brake pedal. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> DetectedBrakeSwitchOn() </li>
     *      </ul>
     *      
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     *
     * @return Result of the brake switch on test.
     */
    virtual string CheckBrakeSwitchOnResult(void);

    /**
     * Check the result of the brake switch off test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of the brake switch off test will be checked.  If the brake switch was not detected in the Off position,
     * the operator will be prompted to release the brake pedal.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between brake switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ReleaseBrakePedal - Remove foot from brake pedal. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> DetectedBrakeSwitchOff() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     *
     * @return Result of the brake switch off test.
     */
    virtual string CheckBrakeSwitchOffResult(void);

    /**
     * Check the results of the redundant brake switch test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The redundant brake switch results will be checked. If a state has not been detected, the operator will be 
     * prompted to achieve the desired state.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> BrakeSwitchResultBit - Bit to use for recording the result of the redundant brake switch test in the module. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> EOLBrakeSwitchResultRegister - Parameter to use for recording the redundant brake switch result. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> RedundantBrakeSwitchOnFail - Failed to detect the redundant brake switch in the On position. </li>
     *      <li> RedundantBrakeSwitchOffFail - Failed to detect the redundant brake switch in the Off position. </li>
     *      <li> ModuleUpdateCommunicationFailure - Could not record redundant brake switch result in the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while processing the redundant brake switch results. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> CheckRedundantBrakeSwitchOnResult() </li>
     *      <li> CheckRedundantBrakeSwitchOffResult() </li>
     *      <li> CheckBrakeSwitchOnResult() </li>
     *      <li> CheckBrakeSwitchOffResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameter() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameterInt() </li>
     *      <li> UpdateEOLStatusRegister() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of the redundant brake switch test.
     */
    virtual string CheckRedundantBrakeSwitchResults(void);

    /**
     * Check the result of the redundant brake switch on test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of the redundant brake switch on test will be checked. If the redundant brake switch has not been
     * detected in the On position, the operator will be prompted to apply the brake pedal.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between redundant brake switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> PressBrakePedal - Apply the brake pedal. </li>
     *      </ul>
     *  
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> DetectedRedundantBrakeSwitchOn() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     *
     * @return Result of the redundant brake switch on test.
     */
    virtual string CheckRedundantBrakeSwitchOnResult(void);

    /**
     * Check the result of the redundant brake switch off test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of the redundant brake switch off test will be checked. If the redundant brake switch has not been
     * detected in the Off position, the operator will be prompted to release the brake pedal.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between redundant brake switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ReleaseBrakePedal - REmove foot from brake pedal. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> DetectedRedundantBrakeSwitchOff() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Result of the redundant brake switch off test.
     */

    virtual string CheckRedundantBrakeSwitchOffResult(void);

    /**
     * Check the result of the Park/Neutral switch on test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of the park/neutral switch on test will be checked.  If the park/neutral switch has not been detected 
     * in the On position, the operator will be instructed to shift the transmission to Park.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - AMount of time(ms) to wait between park/neutral switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToPark - Shift the transmission to Park. </li>
     *      </ul>
     * 
     * <b>Module interface Functions:</b>
     *      <ul>
     *      <li> DetectedParkNeutralSwitchOn() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Result of the Park/Neutral switch on test.
     */
    virtual string CheckParkNeutralSwitchOnResult(void);

    /**
     * Check the result of the Park/Neutral switch off test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of the park/neutral switch off test will be checked.  If the park/neutral switch has not been detected 
     * in the Off position, the operator will be instructed to shift the transmission to Drive.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - AMount of time(ms) to wait between park/neutral switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToDrive - Shift the transmission to Drive. </li>
     *      </ul>
     * 
     * <b>Module interface Functions:</b>
     *      <ul>
     *      <li> DetectedParkNeutralSwitchOff() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Result of the Park/Neutral switch off test.
     */
    virtual string CheckParkNeutralSwitchOffResult(void);

    /**
     * Check the result of the clutch switch on test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of the clutch switch on test will be checked.  If the clutch switch has not been detected in the On
     * position, the operator will be instructed to apply the clutch pedal.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - AMount of time to wait between clutch switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> PressClutchPedal - Press the clutch pedal. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> DetectedClutchSwitchOn() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Result of the clutch switch on test.
     */
    virtual string CheckClutchSwitchOnResult(void);

    /**
     * Check the result of the clutch switch off test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The result of the clutch switch off test will be checked.  If the clutch switch has not been detected in the Off
     * position, the operator will be instructed to release the clutch pedal.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - AMount of time to wait between clutch switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ReleaseClutchPedal - Remove foot from clutch pedal. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> DetectedClutchSwitchOff() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Result of the clutch switch off test.
     */
    virtual string CheckClutchSwitchOffResult(void);

    /**
     * Check the results of the clutch Upstop switch test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * If the vehicle is equipped with a clutch upstop switch, the switch test results will be processed.  If a state 
     * has not been detected, the operator will be prompted to achieve the desired state.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ClutchSwitchResultBit - Bit to use for recording the result of the clutch upstop test. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> EOLClutchSwitchResultRegister - Parameter to use for the clutch switch result register. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ClutchUpstopSwitchOnFail - Did not detect the clutch upstop in the On position. </li>
     *      <li> ClutchUpstopSwitchOffFail - Did not detect the clutch upstop switch in the Off position. </li>
     *      <li> ClutchOffUpstopOnFail - Failed to detect the upstop switch on when the clutch switch was off. </li>
     *      <li> ClutchUpstopSwitchFail - Files to detect the clutch upstop switch in either the On or Off position. </li>
     *      <li> ModuleUpdateCommunicationFailure - Could not record the clutch upstop result in the module. </li>
     *      <li> SoftwareFailure - A ModuleException occurred while processing the clutch upstop switch results. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> IsClutchUpstopSwitchEquipped() </li>
     *      <li> CheckClutchUpstopSwitchOnResult() </li>
     *      <li> CheckClutchUpstopSwitchOffResult() </li>
     *      <li> CheckClutchOffUpstopOnResult() </li>
     *      <li> CheckClutchSwitchOnResult() </li>
     *      <li> CheckClutchSwitchOffResult() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetDataTag() </li>
     *      <li> UpdateEOLStatusRegister() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @return Result of the clutch upstop switch test.
     */
    virtual string CheckClutchUpstopSwitchResults(void);

    /**
     * Check the result of the clutch Upstop switch on test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b>
     * The clutch upstop result will be checked to ensure the upstop switch was detected in the On position.  If the 
     * switch has not been detected in the On position, the operator will be instructed to apply the clutch pedal.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between clutch upstop switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> PressClutchPedal - Apply the clutch pedal. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> DetectedClutchUpstopSwitchOn() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Result of the clutch Upstop switch on test.
     */
    virtual string CheckClutchUpstopSwitchOnResult(void);

    /**
     * Check the result of the clutch Upstop switch off test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b>
     * The clutch upstop result will be checked to ensure the upstop switch was detected in the Off position.  If the 
     * switch has not been detected in the Off position, the operator will be instructed to release the clutch pedal.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between clutch upstop switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ReleaseClutchPedal - Release the clutch pedal. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> DetectedClutchUpstopSwitchOff() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Result of the clutch Upstop switch off test.
     */
    virtual string CheckClutchUpstopSwitchOffResult(void);

    /**
     * Check the result of the clutch Off and Upstop On switch test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The clutch switches will be checked to ensure the upstop switch was detected in the On position when the clutch
     * switch was detected in the Off position.  If the correct switch state has not been detected, the operator will
     * be instructed to release the clutch pedal.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between switch checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ReleaseClutchPedal - REmove foot from the clutch pedal. </li>
     *      </ul>
     * 
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> DetectedClutchOffUpstopOn() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return Result of the clutch Off and Upstop On test.
     */
    virtual string CheckClutchOffUpstopOnResult(void);

    /**
     * Check for the transmission to be in neutral.  
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The transmission gear will be checked to ensure it is in Neutral.  If the transmission is not in Neutral, the 
     * operator will be instructed to shift the transmission to Neutral.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between transmission gear checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ShiftToNeutral - Shift the transmission to Neutral. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsTransmissionInNeutral() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     *
     * @return  Flag indicating the transmission is in neutral.
     */
    virtual bool CheckTransmissionInNeutral(void);

    /**
     * Reset all PCM registers.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The PCM registers passed into the function will be reset according to the values in <i>pcmRegisters</i>.
     * The current value in the module will be overwritten with no checking.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> UpdateEOLStatusByte() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return Result of reseting all module registers.
     */
    virtual string ResetStatusRegisters(const XmlNodeMap *pcmRegisters);

    /**
     * Read all PCM registers.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The PCM registers will be read from the module.  The register values will be logged to file. 
     * <br><b><i>Note:</i></b> The register values will not be stored anywhere.  This function is mainly for development
     * and debug use only.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo()  - Message Tags: ReadStatusRegister_<i>[x]</i> </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     *
     * @return Result of reading all module registers.
     */
    virtual string ReadStatusRegisters(void);

    /**
     * Store the previous pass status.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the previous pass status of the vehicle.
     * <p>
     * @param previousPassStatus
     *               Previous pass status read from the module.
     */
    virtual void SetPreviousPassStatus(const bool &previousPassStatus);

    /**
     * Store the current odometer reading.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the current odometer reading from EEROM.  The current odometer reading will be used to verify the odometer 
     * RAM matches the EEROM value.
     * <p>
     * @param currentReading
     *               Current EEROm odometer reading.
     */
    virtual void StoreCurrentEEROMOdometerReading(const UINT16 &currentReading);

    /**
     * Get the current odometer reading.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the current odometer reading as read from the module EEROM.
     * <p>
     * @return Current EEROM odometer reading.
     */
    virtual const UINT16& GetCurrentOdometerEEROMReading(void);

    /**
     * Check if the vehicle is equipped with ETC.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the flag indicating if the vehicle is equipped with ETC.
     * <p>
     * @return  Flag indicating if the vehicle is equipped with ETC.
     */
    virtual const bool &IsETCEquipped(void);

    /**
     * Store the flag indicating if the vehicle is equipped with ETC.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the flag indicating if the vehicle is equipped with ETC.
     * <p>
     * @param equipped  Flag indicating if the vehicle is equipped with ETC.
     */
    virtual void SetETCEquipped(const bool &equipped);

    /**
     * Determine if the hydraulic cooling fan must be cycled.
     * <p><b>Category:</b> Utility
     * <p><b> Description:</b><br>
     * The engine size will be checked to determine if the hydraulic cooling fan must be run.  Typically the hydraulic 
     * cooling fan is only run on 5.7L WK/XK vehicles.  Other plants can remove the test step from the config file.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> EngineSizeForHydraulicCoolingFanTest - Parameter indicating engine size required for running the 
     *                                                  hydraulic cooling fan test. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @return Flag indicating if the hydraulic cooling fan should be tested.
     *      <ul>
     *      <li> True - Run the hydraulic cooling fan test. </li>
     *      <li> False - Do not perform the hydraulic cooling fan test. </li>
     *      </ul>
     */
    virtual bool NeedToRunHydraulicCoolingFan(void);

	/**
	 * Initialize the module interfaces.
	 * <br><p>
     * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
     * Initializes Gateway module interface 
     * <br><p>
	 * 
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> GatewayModuleConfigFile - Module Config file for the gateway module. </li>
	 * 		</ul>
     * 
	 * <b>Gateway Module Interface Functions:</b>
	 * 		<ul>
	 * 		<li> Initialize() </li>
	 * 		</ul>
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GenericEmissionsTCTemplate<ModuleType>::InitializeModuleInterface() </li>
	 * 		<li> GetParameter() </li>
	 * 		<li> ReturnXMLDocument() </li>
	 * 		</ul>
	 * 
	 * @return Status of setting up the module interface.  
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - The module interface was set up properly. </li>
     *      <li> BEP_STATUS_FAIL - The module interface could not be setup. </li>
     *      </ul>
	 */
	virtual BEP_STATUS_TYPE InitializeModuleInterface(void);

    /** Flag to indicate if the vehicle has previously passed rolls testing. */
    bool m_isPreviousPass;

    /** Flag to indicate if the vehicle is equipped with ETC. */
    bool m_etcVehicle;

    /** Size of engine in the vehicle. */
    string m_engineSize;

    /** Type of vehicle being tested. */
    string m_vehicleType;

    /** Type of emissions vehicle is configured with. */
    string m_emissionsType;

    /** Model year the emissions compied with. */
    string m_emissionsYear;

    /** Type of engine controller module vehicle is equipped with. */
    string m_moduleType;

    /** Current odometer reading according to the EEROM. */
    UINT16 m_currentOdometerReading;

    /** SRI Initial setting values. */
    XmlNodeMap m_SRIInitialSetting;

    /** SRI first tumble values. */
    XmlNodeMap m_SRIFirstTumble;

    /** SRI second tumble values. */
    XmlNodeMap m_SRISecondTumble;

    /** Map of EOL Status registers to reset and which bits to reset. */
    XmlNodeMap m_resetEOLStatusRegisters;

    /** PCM registers to reset after a successful test. */
    XmlNodeMap m_resetPCMRegisters;
     
    /** Map to contain the various types of manual transmissions. */
    XmlNodeMap m_manualTransmissionTypes;

    /** PCM registers to reset for development testing. */
    XmlNodeMap m_defaultRegisters;

    /** DataAnalysis object to analyze test data. */
    DataAnalysis m_analyze;

    /** Flag to determine if Checking of the redundant brake switch should be included in the monitoring of switches. */
    bool m_boolTestRedundantBrakeSwitch;

    /** Flag to determine if Checking of the clutch upstop switch should be included in the monitoring of switches. */
    bool m_boolTestClutchUpstopSwitch;

    /** Store the the valid Speed Control Types for this test from configuration file. */
    XmlNodeMap m_validETCSpeedControlTypes;

	/** gateway Module interface */
    VehicleModuleType *m_gatewayModule;
};

//*************************************************************************
#endif //GenericEmissionsTCTemplate_h
