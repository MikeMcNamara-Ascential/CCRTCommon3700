//******************************************************************************
// Description:
//  Module Interface for a generic Emissions/Engine Controller module.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/GenericEmissionsModuleTemplate.h $
// 
// 1     11/01/07 1:45p Derickso
// Updated header comments to doxygen format.
// 
// 1     10/10/07 2:45p Derickso
// Updated header comments to doxygen format.
// 
// 18    2/22/07 8:42a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added GetInFieldModeRegister() and GetInFieldModeBit().
// - Added ActivateHydraulicCoolingFan().
// - Added ToggleMdsInhibit().
// - Added IsInFieldMode().
// - Added SetInFieldModeRegister() and SetInFieldModeBit().
// - Added m_inFieldModeBit and m_inFieldModeRegister.
// 
// 17    1/17/07 2:56p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added OpenCom().
// 
// 16    10/25/06 3:06p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added CloseCom()
// 
// 15    9/05/06 3:43p Rwiersem
// Changes for the 06090601 core release:
// 
// - Added Set methods for engine authentication strings
// - Added Get methods for engine authentication strings
// - Added StartQuickLearnEngineAuthentication()
// - Added StopQuickLearnEngineAuthentication()
// - Added member variables for engine authentication strings
// - Made EngineControllerTest a friend class
// 
// 14    4/13/06 3:15a Cward
// Add ability to skip park neutral checks and prompted ReadAxleSpeeds
//
// 1     2/14/06 10:47a Derickso
// Added ability to skip park/neutral switch check.
// Promoted ReadAxleSpeeds function since it is needed by multiple
// controllers.
//
// 13    11/11/05 4:54p Cward
// Changes for 0511101 iteration
//
// 12    3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 11    10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 10    9/29/04 10:03p Cward
// Made cruise test more generic
//
// 9     9/01/04 8:52p Cward
// Removed inlines from function prototypes. Changed parameter type for
// Read Cruise Brake from bool to string. Added function prototypes for
// starting and stopping the hydraulic fan.
//
// 8     8/18/04 11:03p Cward
// changed includes from h to cpp for template classes, cleaned up un
// needed includes and forward declarations
//
// 7     8/05/04 2:37a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 6     5/25/04 2:35p Cward
// Added deletion of the TPS Monitor in the destructor, added Message Tag
// parameter in Start TPS Monitor function. Update Stop TPS Monitor to
// check for NULL pointers. Update Read Throttle Position to take a
// message tag
//
// 5     3/04/04 3:50p Cward
// Added wait for done method call
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
// 1     12/21/03 6:32p Cward
//
// 5     10/28/03 6:49a Derickso
// Added support for speed control testing.
// Added functionality to read cruise target speed,
// current cruise mode and which cruise button is
// being pressed.
//
// 1     10/24/03 3:16p Canyanwu
// Start of DCX working directory
//
// 4     10/20/03 9:22a Derickso
// Latest changes after debugging and testing with a vehicle.
//
// 3     9/08/03 7:25a Derickso
// Moved EEPROM_TEST_RESULT enum to GenericMOduleTemplate
// since it is used by more than one module class.
//
// 2     8/26/03 8:05a Derickso
// Updated for latest directory structure.
// Compiles, not tested.
//
// 1     8/21/03 8:57a Derickso
// 05WK CCRT Project
//
// 7     8/20/03 6:41a Derickso
// Recombined cpp and h files.  Compiler does not support the
// implementation being seperate for template classes.
//
// 6     8/19/03 9:37a Derickso
// Updated class names.  Added Template to the end of the Generic class
// names to avoid conflicts with code written earlier.
//
// 4     8/13/03 1:01p Derickso
// Split implementation out to cpp file.
//
// 3     6/09/03 7:53a Derickso
// Compiles, not tested.
//
// 1     6/09/03 7:51a Derickso
// Created.
// Compiles, not tested.
//
// 2     5/28/03 7:52a Derickso
// Converted class to template form.
//
// 1     5/27/03 3:31p Derickso
// Periodic backup.
//
// 1     4/25/03 11:11a Derickso
// Periodic backup.
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericEmissionsModuleTemplate_h
#define GenericEmissionsModuleTemplate_h
//==============================================================================

#include "GenericModuleTemplate.cpp"
#include "ThrottlePositionMonitor.cpp"


/**
 * Generic Emissions/Engine Controller Module Interface Class.
 *
 * @author Dan Erickson
 * @see GenericModuleTemplate
 */
template <class ProtocolFilterType>
class GenericEmissionsModuleTemplate : public GenericModuleTemplate<ProtocolFilterType>
{
    friend class EngineControllerTest;

public:
    /**
     * Class constructor.<br>
     * A comm object will be created to use for monitoring the TPS.
     */
    GenericEmissionsModuleTemplate(void);

    /**
     * Class destructor.<br>
     * If the TPS monitor is still running, it will be commanded to stop.  The software will then wait until the monitor
     * stops before deleting the monitor and completing the destruction of this class.
    * <p>
    * <b>Functions Called:</b>
    *      <ul>
    *      <li> ThrottlePositionMonitor::StopMonitor() </li>
    *      <li> ThrottlePositionMonitor::WaitUntilDone() </li>
    *      </ul>
     */
    virtual ~GenericEmissionsModuleTemplate(void);

    /**
     * Close all communication channels to the module.
     * <p><b>Description:</b><br>
     * The TPS monitor will be commanded to stop if it is still running.  All test data will be reset to initial states.
     * The engine, vehicle and emissions lists will all be cleared out so no bad data is used.  The base class will
     * then be called to complete closing of the channel.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ThrottlePositionMonitor::StopMonitor() </li>
     *      <li> ThrottlePositionMonitor::WaitUntilDone() </li>
     *      <li> GenericModuleTemplate::CloseCom() </li>
     *      </ul>
     */
    virtual void CloseCom();
    
    /**
     * Opens the communication channel to the module.
     * <p><b>Description:</b><br>
     * The class will be initialized by calling InitializeHook().  The TPS monitor communication object will be 
     * commanded to open its communication channel as well.  The base class will then be called to complete opening
     * the communication channels.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> InitializeHook() </li>
     *      <li> GenericModuleTemplate::OpenCom() </li>
     *      </ul>
     *
     * @return Flag indicating if the serial connection was opened successfully.
     */
    virtual bool OpenCom();
    
    /**
     * Get the name of the module.
     * <p><b>Description:</b><br>
     * The name of the module will be returned to the calling function.
     * <p>
     *
     * @returns Generic Emissions/Engine Controller
     */
    virtual string ModuleName();

    /**
     * GetInfo for string types
     * <p><b>Description:</b><br>
     * String type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadSerialNumber </li>
     *      <li> ReadEmissionsModelYear </li>
     *      <li> ReadEmissionsConfiguration </li>
     *      <li> ReadEngineType </li>
     *      <li> ReadVehicleType </li>
     *      <li> ReadAirConditionerStatus </li>
     *      <li> GetACStatusBit() </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was successfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data item from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, string &value) throw(ModuleException);

    /**
     * GetInfo for unsigned short types
     * <p><b>Description:</b><br>
     * Unsigned short data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadUplineRegister </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 &value) throw(ModuleException);

    /**
     * GetInfo for a vector of unsigned short types
     * <p><b>Description:</b><br>
     * A vector of unsigned short data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The values read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT16>&value) throw(ModuleException);

    /**
     * GetInfo for a vector of unsigned char types
     * <p><b>Description:</b><br>
     * A vector of unsigned character data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The vector data read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT8> & value) throw(ModuleException);

    /**
     * GetInfo for integer types
     * <p><b>Description:</b><br>
     * Integer data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadEngineIdleRPM </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The vector data read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, int &value) throw(ModuleException);

    /**
     * GetInfo for unsigned char types
     * <p><b>Description:</b><br>
     * Unsigned character data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadNeutralState </li>
     * 	    </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 &value) throw(ModuleException);

    /**
     * GetInfo for bool types
     * <p><b>Description:</b><br>
     * Boolean type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckBrakeSwitchOn() </li>
     *      <li> CheckBrakeSwitchOff() </li>
     *      <li> CheckParkNeutralSwitchOn() </li>
     *      <li> CheckParkNeutralSwitchOff() </li>
     *      <li> CheckClutchSwitchOn() </li>
     *      <li> CheckClutchSwitchOff() </li>
     *      <li> IsPreviousPass() </li>
     *      <li> IsInFieldMode() </li>
     *      <li> ReadModuleData() </li>
     *      <li> CheckRedundantBrakeSwitchOn() </li>
     *      <li> CheckRedundantBrakeSwitchOff() </li>
     *      <li> CheckClutchUpstopSwitchOn() </li>
     *      <li> CheckClutchUpstopSwitchOff() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> CheckBrakeSwitchOn </li>
     *      <li> CheckBrakeSwitchOff </li>
     *      <li> CheckParkNeutralSwitchOn </li>
     *      <li> CheckParkNeutralSwitchOff </li>
     *      <li> CheckClutchSwitchOn </li>
     *      <li> CheckClutchSwitchOff </li>
     *      <li> IsPreviousPass </li>
     *      <li> IsInFieldMode </li>
     *      <li> ReadTransmissionType </li>
     *      <li> CheckRedundantBrakeSwitchOn </li>
     *      <li> CheckRedundantBrakeSwitchOff </li>
     *      <li> CheckClutchUpstopSwitchOn </li>
     *      <li> CheckClutchUpstopSwitchOff </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, bool &value) throw(ModuleException);

    /**
     * GetInfo for FaultVector_t  types
     * <p><b>Description:</b><br>
     * Fault data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadFaults() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadFaults </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The faults read from the module.
     * 
     * @return Status of reading the faults from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Faults were succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read faults from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, FaultVector_t &value) throw(ModuleException);

    /**
     * GetInfo for float types
     * <p><b>Description:</b><br>
     * Float type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadMAPSensor </li>
     *      <li> ReadThrottleVoltage </li>
     *      <li> ReadCruiseTargetSpeed </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);

    /**
     * Update a status byte in the module.
     * <p><b>Description:</b><br>
     * The specified <i>value</i> will be written to the <i>statusByte</i>.  If the update fails, module security may
     * be unlocked and the update attempted again.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> WriteEOLStatusRegister() </li>
     *      <li> UnlockIfUpdateFailed() </li>
     *      <li> UnlockModuleSecurity() </li>
     *      </ul>
     *
     * @param statusByte Status byte to update.
     * @param value      Value to write to the status byte.
     * @return Status of updating the status byte in the module.
     */
    virtual BEP_STATUS_TYPE UpdateEOLStatusByte(const UINT16 &statusByte, const UINT16 &value);

    /**
     * Update the status byte with the result of a test.
     * <p><b>Description:</b><br>
     * The status register <i>address</i> will be read from the module to ensure we are updating the latest greatest
     * information.  The <i>testBit</i> will then be updated with the <i>testResult</i> and written back to the module.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadEOLStatusRegister() </li>
     *      <li> UpdateEOLStatusByte() </li>
     *      </ul>
     *
     * @param address    Status byte to be updated.
     * @param testBit    Bit in the status byte to update.
     * @param testResult Result to record in the testBit.
     * @return Status of updating the status register to the specified test result.
     */
    virtual BEP_STATUS_TYPE UpdateEEPROMTestBit(const UINT16 address, const UINT16 testBit,
                                                const EEPROM_TEST_RESULT testResult);

    /**
     * Read the current cruise mode from the module.
     * <p><b>Description:</b><br>
     * The cruise mode (On, Off, Set, Resume, Engage, etc.) will be read from the modue and returned to the calling 
     * function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * @param message The message tag to be sent to the module to read the current cruise status.
     * @param currentMode
     *               Current mode of the cruise.
     *
     * @return Status of reading the cruise mode from the module.
     */
    virtual BEP_STATUS_TYPE ReadCruiseMode(const string &message, UINT16 &currentMode);

    /**
     * Read the switch being pressed from the module.
     * <p><b>Description:</b><br>
     * The current cruise switch being pressed will be returned to the calling function.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadCruiseSwitch </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * @param switchPressed
     *               Current switch being pressed.
     * @return Status of reading the currently pressed cruise switch from the module.
     */
    virtual BEP_STATUS_TYPE ReadCruiseSwitch(string &switchPressed);

    /**
     * Read the brake switch position from the module.
     * <p><b>Description:</b><br>
     * Read the cruise brake cutout status from the module.  The cruise cutout reason will be checked to ensure the 
     * cutout was a result of the brake being pressed.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadCruiseBrake </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * @param cutoutReason
     *               Reason Cruise mode was cutout.
     * @return Status of reading the cruise brake cutout status from the module.
     */
    virtual BEP_STATUS_TYPE ReadCruiseBrake(string &cutoutReason);
    /**
     * Read the requested axle speed from the module.
     * <p><b>Description:</b><br>
     * The axle speed for the specified axle will be read from the module and returned to the calling function.
     * <p>
     *
     * @param axle      Which axle to read the speed from.
     * @param axleSpeed Speed of the axle read from the module.
     * @return Status of reading the requested axle speed from the module.
     */
    virtual BEP_STATUS_TYPE ReadAxleSpeed(const string &axle, float &axleSpeed);

    /**
     * Begin a diagnostic session with the module.
     * <p><b>Description:</b><br>
     * The module will be commanded to enter a diagnostic session.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> EnterDiagnosticSession </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of beginning a diagnostic session with the module.
     */
    virtual BEP_STATUS_TYPE EnterDiagnosticSession();

    /**
     * Unlock module security so data can be written to the module.
     * <p><b>Description:</b><br>
     * The security seed will be read from the module.  The security unlock key will be generated from the seed and 
     * written to the module to unlock security.
     * <br><b><i>Note:</i></b>  Every engine controller type has a different security unlock algortihm. No generic method
     *                          is available.  This function will return BEP_STATUS_SOFTWARE if called.
     *
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE UnlockModuleSecurity();

    /**
     * Begin monitoring the throttle position sensor.
     * <p><b>Description:</b><br>
     * The initial throttle position value will be read from the module and stored for later comparison.  It is generally
     * desired to detect a specific change in the throttle position sensor during the test to verify that TPS is
     * functioning properly.  After the initial value is stored, the background monitor will be started to periodically
     * monitor the TPS.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> TPSMonitorDelay - Amount of time(ms) to wait between TPS reads. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      <li> SetInitialTPS() </li>
     *      <li> ThrottlePositionMonitor::Resume() </li>
     *      </ul>
     * 
     * @param messageTag Message tag to use for monitoring the TPS from the module.
     * @param resuming   Flag to indicate if the TPS monitor is being resumed after being stopped.  If it is being resumed, the initial TPS
     *                   reading will not be reset.
     * 
     * @return Status of starting the TPS background monitor.
     * @see ThrottlePositionMonitor
     */
    virtual BEP_STATUS_TYPE StartTPSMonitor(const string &messageTag, const bool resuming = false);

    /**
     * Stop monitoring the throttle position sensor.
     * <p><b>Description:</b><br>
     * The TPS background monitor will be commanded to stop.  Once the monitor stops, it will be deleted.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ThrottlePositionMonitor::StopMonitor() </li>
     *      <li> ThrottlePositionMonitor::WaitUntilDone() </li>
     *      </ul>
     *
     * @return Status of stopping the TPS background monitor.
    * @see ThrottlePositionMonitor
     */
    virtual BEP_STATUS_TYPE StopTPSMonitor();

    /**
     * Read switches from module. 
     * <p><b>Description:</b><br>
     * The observed switch statuses will be checked to determine if switches need to be read from the module.  If 
     * switches are read from the module, the data will be analyzed to determine which switches and which switch states
     * are currently present.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadSwitches </li>
     *      <li> ReadBrakeSwitchOn </li>
     *      <li> ReadBrakeSwitchOff </li>
     *      <li> ReadRedundantBrakeSwitchOn </li>
     *      <li> ReadRedundantBrakeSwitchOff </li>
     *      <li> ReadParkNeutralSwitchOn </li>
     *      <li> ReadParkNeutralSwitchOff </li>
     *      <li> ReadClutchSwitchOn </li>
     *      <li> ReadClutchSwitchOff </li>
     *      <li> ReadClutchUpstopSwitchOn </li>
     *      <li> ReadClutchUpstopSwitchOff </li>
     *      </ul>
     * 
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedBrakeSwitchOn() </li>
     *      <li> DetectedBrakeSwitchOff() </li>
     *      <li> IsRedundantBrakeSwitchEquipped() </li>
     *      <li> DetectedRedundantBrakeSwitchOn() </li>
     *      <li> DetectedRedundantBrakeSwitchOff() </li>
     *      <li> IsAutomaticTransmission() </li>
     *      <li> DetectedParkNeutralSwitchOn() </li>
     *      <li> DetectedParkNeutralSwitchOff() </li>
     *      <li> SkipParkNeutralSwitch() </li>
     *      <li> DetectedClutchSwitchOn() </li>
     *      <li> DetectedClutchSwitchOff() </li>
     *      <li> IsClutchUpstopSwitchEquipped() </li>
     *      <li> DetectedClutchOffUpstopOn() </li>
     *      <li> DetectedClutchUpstopSwitchOff() </li>
     *      <li> SetBrakeSwitchOnDetected() </li>
     *      <li> SetBrakeSwitchOffDetected() </li>
     *      <li> SetRedundantBrakeSwitchOnDetected() </li>
     *      <li> SetRedundantBrakeSwitchOffDetected() </li>
     *      <li> SetParkNeutralSwitchOnDetected() </li>
     *      <li> SetParkNeutralSwitchOffDetected() </li>
     *      <li> SetClutchSwitchOnDetected() </li>
     *      <li> SetClutchSwitchOffDetected() </li>
     *      <li> SetClutchUpstopSwitchOnDetected() </li>
     *      <li> SetClutchUpstopSwitchOffDetected() </li>
     *      <li> SetClutchOffUpstopOnDetected() </li>
     *      <li> ParseBooleanResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param filter Communication object to use for reading the switch states from the module.
     * @return Status of reading switch states from the module.
     */
    virtual BEP_STATUS_TYPE ReadSwitches(IProtocolFilter *filter);

    /**
     * Launch the switch monitor.
     * <p><b>Description:</b><br>
     * The transmission type will be read from the module to determine if the Park/Neutral switch should be checked
     * or if the clutch switch should be checked.  The base class will then be called to start the background switch
     * monitor.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadTransmissionType </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      <li> SetTransmissionType() </li>
     *      <li> GenericModuleTemplate::StartSwitchMonitor() </li>
     *      </ul>
     *
     * @return Status of starting the switch monitor.
     */
    virtual BEP_STATUS_TYPE StartSwitchMonitor(void) throw(ModuleException);

    /**
     * Read throttle position from module.
     * <p><b>Description:</b><br>
     * If the required delta throttle has not been observed, the current throttle position will be read from the module.
     * The data will then be analyzed to determine if the required delta has been observed.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> DeltaTPSRequired - Required throttle delta to observed during the test. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetObservedDeltaTPS() </li>
     *      <li> ReadModuleData() </li>
     *      <li> GetInitialTPSReading() </li>
     *      <li> SetFinalTPSReading() </li>
     *      <li> SetObservedDeltaTPS() </li>
     *      </ul>
     *
     * @param filter Communication object to use for reading the throttle position sensor from the module.
     * @return Status of reading the throttle position sensor from the module.
     */
    virtual BEP_STATUS_TYPE ReadThrottlePosition(IProtocolFilter *filter, const string &messageTag);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the brake switch on state was detected.
     * <p>
     * @return Flag indicating if the brake switch on state was detected.
     */
    virtual const bool &DetectedBrakeSwitchOn(void);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the brake switch off state was detected.
     * <p>
     * @return Flag indicating if the brake switch off state was detected.
     */
    virtual const bool &DetectedBrakeSwitchOff(void);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the redundant brake switch on state was detected.
     * <p>
     * @return Flag indicating if the redundant brake switch on state was detected.
     */
    virtual const bool &DetectedRedundantBrakeSwitchOn(void);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the redundant brake switch off state was detected.
     * <p>
     * @return Flag indicating if the redundant brake switch off state was detected.
     */
    virtual const bool &DetectedRedundantBrakeSwitchOff(void);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the clutch switch on state was detected.
     * <p>
     * @return Flag indicating if the clutch switch on state was detected.
     */
    virtual const bool &DetectedClutchSwitchOn(void);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the clutch switch off state was detected.
     * <p>
     * @return Flag indicating if the clutch switch off state was detected.
     */
    virtual const bool &DetectedClutchSwitchOff(void);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the clutch upstop switch on state was detected.
     *
     * @return Flag indicating if the clutch upstop switch on state was detected.
     */
    virtual const bool &DetectedClutchUpstopSwitchOn(void);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the clutch upstop switch off state was detected.
     * <p>
     * @return Flag indicating if the clutch upstop switch off state was detected.
     */
    virtual const bool &DetectedClutchUpstopSwitchOff(void);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the Clutch Off and Upstop On switch states were
     * detected at the same time.
     * <p>
     * @return Flag indicating if the clutch Off and Upstop On switch states were detected.
     */
    virtual const bool &DetectedClutchOffUpstopOn(void);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the Park/Neutral switch on state was detected.
     * <p>
     * @return Flag indicating if the Park/Neutral switch on state was detected.
     */
    virtual const bool &DetectedParkNeutralSwitchOn(void);

    /**
     * <b>Description:</b><br>
     * Get the flag indicating if the Park/Neutral switch off state was detected.
     *
     * @return Flag indicating if the Park/Neutral switch off state was detected.
     */
    virtual const bool &DetectedParkNeutralSwitchOff(void);

    /**
     * <b>Description:</b><br>
     * Check if the vehicle is equipped with an automatic transmission.
     *
     * @return Flag indicating if the vehicle is equipped with an automatic transmission.
     */
    virtual const bool &IsAutomaticTransmission(void);

    /**
     * <b>Description:</b><br>
     * Get the initial TPS reading taken at closed throttle conditions.
     * <p>
     * @return Initial throttle position sensor reading.
     */
    virtual const float &GetInitialTPSReading(void);

    /**
     * <b>Description:</b><br>
     * Get the final throttle position reading taken when the required delta was observed.
     * <p>
     * @return Final throttle position sensor reading.
     */
    virtual const float &GetFinalTPSReading(void);

    /**
     * <b>Description:</b><br>
     * Get the delta throttle position observed during the delta TPS test.
     * <p>
     * @return Observed delta throttle position.
     */
    virtual const float &GetObservedDeltaTPS(void);

    /**
     * <b>Description:</b><br>
     * Get the register that contains the OK To Ship flag.
     * <p>
     * @return OK To Ship register.
     */
    virtual const UINT16 &GetOkToShipRegister(void);

    /**
     * <b>Description:</b><br>
     * Get the OK To Ship bit.
     * <p>
     * @return OK to ship bit.
     */
    virtual const INT32 &GetOKToShipBit(void);

    /**
     * <b>Description:</b><br>
     * Get the register that contains the In Field Mode bit.
     * <p>
     * @return In Field Mode register.
     */
    virtual const UINT16 &GetInFieldModeRegister();

    /**
     * <b>Description:</b><br>
     * Get the In Field Mode bit.
     * <p>
     * @return In Field Mode bit.
     */
    virtual const INT32 &GetInFieldModeBit();

    /**
     * <b>Description:</b><br>
     * Get the A/C status bit.
     * <p>
     * @return A/C status bit.
     */
    virtual const INT32 &GetACStatusBit(void);

    /**
     * Activate the hydraulic cooling fan.
     * <br><p>
     * <b>Description:</b><br>
     * This function is used in conjunction with the HydraulicFanMonitor class.  This function
     * will send a command to the module to activate the hydraulic cooling fan.
     * <br><p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> StartHydraulicCoolingFan </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     * 
     * @param filter Comm object to use for activating the hydraulic cooling fan.
     * 
     * @return Result of activating the hydraulic cooling fan.
     */
    virtual BEP_STATUS_TYPE ActivateHydraulicCoolingFan(IProtocolFilter *filter);
    /**
     * Start the hydraulic cooling fan.
     * <p><b>Description:</b><br>
     * A command will be sent to the module to start running the hydraulic cooling fan.  This will ensure any bubbles
     * in the cooling system are worked out and that the engine will not overheat.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> StartHydraulicCoolingFan </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of commanding the hydraulic cooling fan to run.
     */
    virtual BEP_STATUS_TYPE StartRadiatorFan(string fanType);
    /**
     * Stop the hydraulic cooling fan.
     * <p><b>Description:</b><br>
     * A command will be sent to the module to stop running the hydraulic cooling fan.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> StopHydraulicCoolingFan </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of commanding the hydraulic cooling fan to stop running.
     */
    virtual BEP_STATUS_TYPE StopRadiatorFan(string fanType);

    /**
     * <p><b>Description:</b><br>
     * Store the flag indicating if the vehicle is equipped with a redundant brake switch.
     * <p>
     * @param isRedundantBrakeEquipped
     *               Flag indicating if the vehicle has a redunadant brake switch.
     */
    virtual void SetRedundantBrakeSwitchEquipped(const bool &isRedundantBrakeEquipped);

    /**
     * <p><b>Description:</b><br>
     * Get the flag indicating if the vehicle is equipped with a redundant brake switch.
     * <p>
     * @return Flag indicating if the vehicle is equipped with a redundant brake switch.
     */
    virtual const bool& IsRedundantBrakeSwitchEquipped(void);

    /**
     * <p><b>Description:</b><br>
     * Store the flag indicating if the vehicle is equipped with the clutch Upstop switch.
     * <p>
     * @param isClutchUpstopEquipped
     *               Flag indicating if the vehicle has a clutch upstop switch.
     */
    virtual void SetClutchUpstopSwitchEquipped(const bool &isClutchUpstopEquipped);

    /**
     * <p><b>Description:</b><br>
     * Get the flag indicating if the vehicle is equipped with a clutch upstop switch.
     * <p>
     * @return Flag indicating if the vehicle is equipped with a clutch upstop switch.
     */
    virtual bool& IsClutchUpstopSwitchEquipped(void);
    /**
     * <p><b>Description:</b><br>
     * Set/Get the flag indicating if the park/neutral switch should be checked.
     * <p>
     * @param skipParkNeutralSwitch
     *               Flag indicating if the park/neutral switch should be checked.
     *
     * @return Flag indicating if the park/neutral switch should be checked.
     */
    virtual const bool& SkipParkNeutralSwitch(const bool *skipParkNeutralSwitch = NULL);

    /**
     * Return the current state of the brake switches.
     * <p><b>Description:</b><br>
     * The brake switch data will be read from the module.  The brake switch and redundant brake switch values will be
     * extracted from the module response and returned to the calling function.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadSwitches </li>
     *      <li> ReadBrakeSwitchOn </li>
     *      <li> ReadRedundantBrakeSwitchOn </li>
     *      </ul>
     * 
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ParseBooleanResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param brakeSwitch  Flag indicating if the brake switch is currently on.
     * @param redundantBrakeSwitch  Flag indicating if the redundant brake switch is currently on.
     *
     * @return Status of reading the brake switch states from the module.
     */
    virtual BEP_STATUS_TYPE ReadCurrentBrakeSwitchStates(bool &brakeSwitch, bool &redundantBrakeSwitch);

    /**
     * Return the current state of the clutch and clutch-Upstop switches.
     * <p><b>Description:</b><br>
     * The switch data will be read from the module.  The clutch switch and clutch upstop switch state will be extracted 
     * from the module response and returned to the calling function.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadSwitches </li>
     *      <li> ReadClutchSwitchOn </li>
     *      <li> ReadClutchUpstopSwitchOn </li>
     *      </ul>
     * 
     * <b>Protocol Filter Functions: </b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsAutomaticTransmission() </li>
     *      <li> ParseBooleanResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param clutchSwitch  Flag indicating if the clutch switch is on.
     * @param clutchUpstopSwitch  Flag indicating if the clutch upstop switch is on.
     *
     * @return Status of reading the clutch and clutch upstop switch states.
     */
    virtual BEP_STATUS_TYPE ReadCurrentClutchSwitchStates(bool &clutchSwitch, bool &clutchUpstopSwitch);

    /**
     * Command the engine controller to start the QuickLearn Engine Authentication Routine.
     * <p><b>Description:</b><br>
     * The module will be commanded to start the quicklearn engin wuthentication routine.  This essentially causes the
     * engine RPM to rise to the correct level to perform the quicklearn procedure.  The status will then be read back
     * from the module to confirm the engine authentication procedure has started.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> StartEngineAuthentication </li>
     *      <li> ReadEngineAuthenticationStatus </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      <li> ReadModuleData() </li>
     *      <li> GetEngineAuthenticationRunningString() </li>
     *      </ul>
     *
     * @return Status of commanding the module to start the quicklearn engine authentication routine.
     */
    virtual BEP_STATUS_TYPE StartQuickLearnEngineAuthentication(void);

    /**
     * Command the engine controller to stop the QuickLearn Engine Authentication Routine.
     * <p><b>Description:</b><br>
     * The module will be commanded to stop the quicklearn authentication routine in the module.  This will return the
     * engine RPM to normal.  The status will then be read from the module to confirm the engine authentication procedure
     * has stopped running.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> StopEngineAuthentication </li>
     *      <li> ReadEngineAuthenticationStatus </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      <li> ReadModuleData() </li>
     *      <li> GetEngineAuthenticationStoppedString() </li>
     *      </ul>
     *
     * @return Status of commanding the module to stop the engine authentication routine.
     */
    virtual BEP_STATUS_TYPE StopQuickLearnEngineAuthentication(void);

    /**
     * Toggle the MDS bit to either inhibit MDS from activating or enabling the MDS to activate.
     * <p><b>Description:</b><br>
     * The MDS feature can be disabled to make it easier for the torque converter to lockup in the transmission.
     * This will reduce the amount of time needed to wait to see full lock-up for the torque converter clutch.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateEEPROMTestBit() </li>
     *      </ul>
     * 
     * @param mdsEnabled Flag specifying which state to place the MDS enable bit.
     *                   <ul>
     *                   <li> True  - Enable the MDS feature </li>
     *                   <li> False - Disable the MDS feature</li>
     *                   </ul>
     * @param mdsRegister
     *                   Register that contains the MDS enable bit.
     * @param mdsEnableBit
     *                   Specific bit used by the module to determine if MDS is enabled.
     * 
     * @return Status of commanding the MDS to the specified state.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - The MDS feature was toggled to the desired state. </li>
     *         <li> BEP_STATUS_FAILURE - The MDS feature could not be toggled to the specified state. </li>
     *         </ul>
     */
    BEP_STATUS_TYPE ToggleMdsInhibit(const bool &mdsEnabled, const UINT16 mdsRegister, const UINT16 mdsEnableBit);
    /**
     * <p><b>Description:</b><br>
     * Get the string representation of engine authentication stopped state.
     * <p>
     * @return String representation of engine authentication stopped state.
     */
    const string &GetEngineAuthenticationStoppedString(void);
    /**
     * <p><b>Description:</b><br>
     * Get the string representation of engine authentication running state.
     * <p>
     * @return String representation of engine authentication running state.
     */
    const string &GetEngineAuthenticationRunningString(void);
    /**
     * <p><b>Description:</b><br>
     * Get the string representation of engine authentication stop pending state.
     * <p>
     * @return String representation of engine authentication stop pending state.
     */
    const string &GetEngineAuthenticationStopPendingString(void);
    /**
     * <p><b>Description:</b><br>
     * Get the string representation of engine authentication start pending state.
     * <p>
     * @return String representation of engine authentication start pending state.
     */
    const string &GetEngineAuthenticationStartPendingString(void);

protected:
    /**
     * Initialize the GenericEmissionsModuleTemplate class.
     * <p><b>Description:</b><br>
     * Perform any initialization specific to the GenericEmissionsModuleTemplate class.  Data from the configuration file will
     * be stored in local member variables for quick and convenient access at a later time.  The base class will then
     * be called to complete the initialization.
     * <p>
     * <b>Module Parmaeters:</b>
     *      <ul>
     *      <li> EngineTypes - List of engine type translations. </li>
     *      <li> VehicleTypes - List of body style type translations. </li>
     *      <li> EmissionsTypes - List of valid emissions configurations. </li>
     *      <li> OKToShipRegister - Register being used for OK to Ship status. </li>
     *      <li> ACStatusBit - Status bit being used to indicate if the AC clutch is engaged. </li>
     *      <li> OKToShipBit - Bit used to indicate the OK to Ship status. </li>
     *      <li> UnlockIfUpdateFailed - Flag to indicate if security should be unlocked if a register update fails. </li>
     *      <li> InFieldModeRegister - Register used to record the PCM mode state (Field or Plant mode). </li>
     *      <li> InFieldModeBit - Bit used to record the PCM mode state. </li>
     *      <li> EngineAuthenticationRepresentation/Stopped - String indicating engine authentication has stopped. </li>
     *      <li> EngineAuthenticationRepresentation/Running - String indicating engine authentication is running. </li>
     *      <li> EngineAuthenticationRepresentation/StopPending - String indicating engine authentication has been
     *                                                            commanded to stop. </li>
     *      <li> EngineAuthenticationRepresentation/StartPending - String indicating the engine authentication has been
     *                                                             commanded to start. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetOkToShipRegister() </li>
     *      <li> SetACStatusBit() </li>
     *      <li> SetOKToShipBit() </li>
     *      <li> SetUnlockIfUpdateFailed() </li>
     *      <li> SetInFieldModeRegister() </li>
     *      <li> SetInFieldModeBit() </li>
     *      <li> SetEngineAuthenticationStoppedString() </li>
     *      <li> SetEngineAuthenticationRunningString() </li>
     *      <li> SetEngineAuthenticationStopPendingString() </li>
     *      <li> SetEngineAuthenticationStartPendingString() </li>
     *      <li> GenericModuleTemplate::InitializeHook() </li>
     *      </ul>
     * 
     * @param configNode Configuration data to be used to initialize the GenericEmissionsModuleTemplate class.
     * 
     * @return Flag indicating if initialization has completed.
     *         <ul>
     *         <li> True - Initialization has been completed. </li>
     *         <li> False - Initialization did not complete. </li>
     *         </ul>
     */
    virtual bool InitializeHook(const XmlNode *configNode);

    /**
     * <b>Description:</b><br>
     * Check if the brake switch on state was detected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedBrakeSwitchOn() </li>
     *      </ul>
     * 
     * @param sawBrakeSwitchOn  Flag indicating brake switch on has been detected.
     * @return Status of checking the brake switch on flag.
     */
    virtual BEP_STATUS_TYPE CheckBrakeSwitchOn(bool &sawBrakeSwitchOn);

    /**
     * <b>Description:</b><br>
     * Check to see if the brake switch off state was detected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedBrakeSwitchOff() </li>
     *      </ul>
     * 
     * @param sawBrakeSwitchOff  Flag indicating brake switch off has been detected.
     * @return Status of checking the brake switch off flag.
     */
    virtual BEP_STATUS_TYPE CheckBrakeSwitchOff(bool &sawBrakeSwitchOff);

    /**
     * <b>Description:</b><br>
     * Check if the redundant brake switch on state was detected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedRedundantBrakeSwitchOn() </li>
     *      </ul>
     * 
     * @param sawBrakeSwitchOn  Flag indicating redundant brake switch on has been detected.
     * @return Status of checking the redundant brake switch on flag.
     */
    virtual BEP_STATUS_TYPE CheckRedundantBrakeSwitchOn(bool &sawBrakeSwitchOn);

    /**
     * <b>Description:</b><br>
     * Check to see if the redundant brake switch off state was detected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedRedundantBrakeSwitchOff() </li>
     *      </ul>
     * 
     * @param sawBrakeSwitchOff  Flag indicating redundant brake switch off has been detected.
     * @return Status of checking the redundant brake switch off flag.
     */
    virtual BEP_STATUS_TYPE CheckRedundantBrakeSwitchOff(bool &sawBrakeSwitchOff);

    /**
     * <b>Description:</b><br>
     * Check if the park/neutral switch on state was detected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedParkNeutralSwitchOn() </li>
     *      </ul>
     * 
     * @param sawParkNeutralSwitchOn
     *               Flag indicating if the park/neutral switch on state was detected.
     * @return Status of chacking the park/neutral switch on flag.
     */
    virtual BEP_STATUS_TYPE CheckParkNeutralSwitchOn(bool &sawParkNeutralSwitchOn);

    /**
     * <b>Description:</b><br>
     * Check if the park/neutral switch off state was detected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedParkNeutralSwitchOff() </li>
     *      </ul>
     * 
     * @param sawParkNeutralSwitchOff
     *               Flag indicating if the park/neutral switch off state was detected.
     * @return Status of checking the park/neutral switch off flag.
     */
    virtual BEP_STATUS_TYPE CheckParkNeutralSwitchOff(bool &sawParkNeutralSwitchOff);

    /**
     * <b>Description:</b><br>
     * Check if the clutch switch on state was detected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedClutchSwitchOn() </li>
     *      </ul>
     * 
     * @param sawClutchSwitchOn
     *               Flag indicating if the clutch switch on state was detected.
     * @return Status of checking the clutch switch on flag.
     */
    virtual BEP_STATUS_TYPE CheckClutchSwitchOn(bool &sawClutchSwitchOn);

    /**
     * <b>Description:</b><br>
     * Check if the clutch switch off state was detected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedClutchSwitchOff() </li>
     *      </ul>
     * 
     * @param sawClutchSwitchOff
     *               Flag indicating if the clutch switch off state was detected.
     * @return Status of checking the clutch switch off flag.
     */
    virtual BEP_STATUS_TYPE CheckClutchSwitchOff(bool &sawClutchSwitchOff);

    /**
     * <b>Description:</b><br>
     * Check if the clutch upstop switch on state was detected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedClutchUpstopSwitchOn() </li>
     *      </ul>
     * 
     * @param sawClutchSwitchOn
     *               Flag indicating if the clutch upstop switch on state was detected.
     * @return Status of checking the clutch upstop switch on flag.
     */
    virtual BEP_STATUS_TYPE CheckClutchUpstopSwitchOn(bool &sawClutchSwitchOn);

    /**
     * <b>Description:</b><br>
     * Check if the clutch upstop switch off state was detected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedClutchUpstopSwitchOff() </li>
     *      </ul>
     * 
     * @param sawClutchSwitchOff
     *               Flag indicating if the clutch upstop switch off state was detected.
     * @return Status of checking the clutch upstop switch off flag.
     */
    virtual BEP_STATUS_TYPE CheckClutchUpstopSwitchOff(bool &sawClutchSwitchOff);

    /**
     * <b>Description:</b><br>
     * Check if the Clutch Off and Upstop On switch states were detected simultaneously.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DetectedClutchOffUpstopOn() </li>
     *      </ul>
     * 
     * @param switchOffDetected
     *               Flag indicating if Clutch Off and Upstop On state switch states were detected simultaneously.
     * @return Status of checking the clutch off and upstop on switch flag.
     */
    virtual BEP_STATUS_TYPE CheckClutchOffUpstopOnResult(bool &sawClutchSwitchOff);

    /**
     * Determine if the vehicle has previously passed rolls.
     * <p><b>Description:</b><br>
     * The OK to Ship register will be read from the module.  The OK to Ship bit will then be checked to determine if
     * the vehicle has been marked as OK to Ship.  This would indicate the vehicle has previously passed roll test
     * emissions testing.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadEOLStatusRegister() </li>
     *      <li> GetOkToShipRegister() </li>
     *      <li> GetOKToShipBit() </li>
     *      </ul>
     * 
     * @param previousPass
     *               Flag indicating if rolls testing has been passed previously.
     * @return Status of checking the previous pass state of the vehicle.
     */
    virtual BEP_STATUS_TYPE IsPreviousPass(bool &previousPass);

    /**
     * Determine if the vehicle is in field mode.
     * <p><b>Description:</b><br>
     * The field mode register will be read from the module.  The field mode bit will then be checked to determine if
     * the vehicle is currently in field mode or plant mode.  The mode bit should always match up with the OK to Ship
     * bit.  If the module is marked as OK to Ship, it should also be in field mode.  If the vehicle is in plant mode,
     * it should not be marked as OK to Ship.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadEOLStatusRegister() </li>
     *      <li> GetInFieldModeRegister() </li>
     *      <li> GetInFieldModeBit() </li>
     *      </ul>
     * 
     * @param inFieldMode
     *               Flag indicating if the vehicle has been placed in field mode.
     * @return Status of checking the field mode state of the vehicle.
     */
    virtual BEP_STATUS_TYPE IsInFieldMode(bool &inFieldMode);

    /**
     * Read faults stored in the module.
     * <p><b>Description:</b><br>
     * Specific modules have different algortihms for reading faults, so no generic implementation has been provided.
     * <br><b><i>Note:</i></b> This function is intended to be overloaded and will return BEP_STATUS_SOFTWARE if called.
     * <p>
     * @param faultCodes Fault Codes read from the module.
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);

    /**
     * Read an EOL status register from the module.
     * <p><b>Description:</b><br>
     * The specified EOL status register will be read from the module.  The register contents will be stored in 
     * <i>registerContents</i> for return to the calling function.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadProcessRegister </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * @param eolStatusRegister
     *               EOL status register to read from the module.
     * @param registerContents
     *               Register value read from the module.
     * @return Status of reading the contents of the specified EOL status register.
     */
    virtual BEP_STATUS_TYPE ReadEOLStatusRegister(const UINT16 &eolStatusRegister, UINT16 &registerContents);

    /**
     * Write data to an EOL status register.
     * <p><b>Description:</b><br>
     * The specifed EOL status register will be updated with the provided register value.
     * <br><b><i>Note:</i></b> The EOL status register will not be read first.  The value contained in the status register
     *                         will be overwritten with <i>registerValue</i>.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> WriteEOLStatusRegister </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @param eolStatusRegister
     *               Process register to update.
     * @param registerValue
     *               Value to write to the register.
     * @return Status of updating the value of the specified EOL status register.
     */
    virtual BEP_STATUS_TYPE WriteEOLStatusRegister(const UINT16 &eolStatusRegister, const UINT16 &registerValue);

    /**
     * Write data to an EOL status register.
     * <p><b>Description:</b><br>
     * The specifed EOL status register will be updated with the provided register value.
     * <br><b><i>Note:</i></b> The EOL status register will not be read first.  The value contained in the status register
     *                         will be overwritten with <i>registerValue</i>.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> WriteEOLStatusRegister </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @param eolStatusRegister
     *               Process register to update.
     * @param registerValue
     *               Value to write to the register.
     * @return Status of updating the value of the specified EOL status register.
     */
    virtual BEP_STATUS_TYPE WriteEOLStatusRegister(const UINT8 &eolStatusRegister, const UINT8 &registerValue);

    /**
     * Read the security seed from the module.
     * <p><b>Description:</b><br>
     * The security seed will be read from the module.  The seed can be used to calculate a security key for unlocking
     * module security.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadSecuritySeed </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     * 
     * @param seed   Security seed read from the module.
     * @return Status of reading the security seed from the module.
     */
    virtual BEP_STATUS_TYPE ReadSecuritySeed(UINT16 &seed);
    /**
     * Read the security seed from the module.
     * <p><b>Description:</b><br>
     * The security seed will be read from the module.  The seed can be used to calculate a security key for unlocking
     * module security.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadSecuritySeed </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     * 
     * @param seed   Security seed read from the module.
     * @return Status of reading the security seed from the module.
     */
    virtual BEP_STATUS_TYPE ReadSecuritySeed(UINT32 &seed);

    /**
     * Calculate the security access key for the module.
     * <p><b>Description:</b><br>
     * Specific modules have different algorithms for calculating the security unlock key from the provided seed.
     * No generic implementation is available.
     * <p>
     * @param seed   Seed to use for calculating the key.
     * @param key    Calculated key for unlocking module security.
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE CalculateSecurityKey(const UINT16 &seed, SerialArgs_t &key);

    /**
     * Check if module security should be unlocked if an update fails.
     * <p><b>Description:</b><br>
     * The flag determining if module security should be unlock if an update to an EOL status register fails will be 
     * returned to the calling function.
     * <p>
     * @return  Flag indicating if module security should be unlocked if the status byte update failed.
     */
    virtual const bool& UnlockIfUpdateFailed(void);

    /**
     * Communication object to use for monitoring throttle position sensor.
     */
    ProtocolFilterType *m_tpsMonitorComm;

    /**
     * Throttle Position Sensor Monitor object;
     */
    ThrottlePositionMonitor<ProtocolFilterType> *m_tpsMonitor;

    /**
     * <b>Description:</b><br>
     * Store the flag indicating if the vehicle is equipped with an automatic transmission.
     * <p>
     * @param isAutomaticTransmission
     *               Flag indicating if the vehicle is equipped with an automatic transmission.
     */
    virtual void SetTransmissionType(const bool &isAutomaticTransmission);

    /**
     * <b>Description:</b><br>
     * Store the flag indicating if the brake switch On state was detected.
     * <p>
     * @param switchOnDetected
     *               Flag indicating if the brake switch on state was detected.
     */
    void SetBrakeSwitchOnDetected(const bool &switchOnDetected);

    /**
     * <b>Description:</b><br>
     * Store the flag indicating if the brake switch Off state was detected.
     * <p>
     * @param switchOffDetected
     *               Flag indicating if the brake switch Off state was detected.
     */
    void SetBrakeSwitchOffDetected(const bool &switchOffDetected);

    /**
     * <b>Description:</b><br>
     * Store the flag indicaint if the redundant brake switch On state was detected.
     * <p>
     * @param switchOnDetected
     *               Flag indicating if the Redundant brake switch on state was detected.
     */
    void SetRedundantBrakeSwitchOnDetected(const bool &switchOnDetected);

    /**
     * <b>Description:</b><br>
     * Store the flag indicating if the redundant brake switch Off state was detected.
     * <p>
     * @param switchOffDetected
     *               Flag indicating if the redundant brake switch off state was detected.
     */
    void SetRedundantBrakeSwitchOffDetected(const bool &switchOffDetected);

    /**
     * <b>Description:</b><br>
     * Store the flag indicating if the Park/Neutral switch On state was detected.
     * <p>
     * @param switchOnDetected
     *               Flag indicaint if the Park/Neutral switch on state was detected.
     */
    void SetParkNeutralSwitchOnDetected(const bool &switchOnDetected);

    /**
     * <b>Description:</b><br>
     * Store the flag indicating if the Park/Neutral switch Off state was detected.
     * <p>
     * @param switchOffDetected
     *               Flag indicating if the Park/Neutral switch off state was detected.
     */
    void SetParkNeutralSwitchOffDetected(const bool &switchOffDetected);

    /**
     * <b>Description:</b><br>
     * Store the flag indicating the clutch switch on state was detected.
     * <p>
     * @param switchOnDetected
     *               Flag indicating if the clutch switch on state was detected.
     */
    void SetClutchSwitchOnDetected(const bool &switchOnDetected);

    /**
     * <b>Description:</b><br>
     * Store the flag indcating if the clutch switch Off state was detected.
     * <p>
     * @param switchOffDetected
     *               Flag indicating if the clutch switch off state was detected.
     */
    void SetClutchSwitchOffDetected(const bool &switchOffDetected);

    /**
     * <b>Description:</b><br>
     * Store the flag indicating if the clutch upstop switch on state was detected.
     * <p>
     * @param switchOnDetected
     *               Flag indicating if the clutch upstop switch on state was detected.
     */
    void SetClutchUpstopSwitchOnDetected(const bool &switchOnDetected);

    /**
     * <b>Description:</b><br>
     * Store the flag indicating if the clutch upstop switch off state was detected.
     * <p>
     * @param switchOffDetected
     *               Flag indicating if the clutch upstop switch off state was detected.
     */
    void SetClutchUpstopSwitchOffDetected(const bool &switchOffDetected);

    /**
     * <b>Description:</b><br>
     * Store the flag indicating that the clutch off and upstop on switch states were detected simultaneously.
     * <p>
     * @param switchOffDetected
     *               Flag indicating the clutch off and upstop on states were detected simultaneously.
     */
    void SetClutchOffUpstopOnDetected(const bool &switchOffDetected);

private:
    /**
     * <b>Description:</b><br>
     * Store the initial TPS reading at closed throttle conditions.
     * <p>
     * @param initialTPS Initial throttle position reading at closed throttle conditions.
     */
    void SetInitialTPS(const float &initialTPS);

    /**
     * <b>Description:</b><br>
     * Store the final TPS reading when the required delta is observed.
     * <p>
     * @param finalTPS Final throttle position reading.
     */
    void SetFinalTPSReading(const float &finalTPS);

    /**
     * <b>Description:</b><br>
     * Store the observed delta throttle position.
     * <p>
     * @param observedDeltaTPS
     *               Observed delta throttle position sensor reading.
     */
    void SetObservedDeltaTPS(const float &observedDeltaTPS);

    /**
     * <b>Description:</b><br>
     * Store the register containing the OK To Ship flag.
     * <p>
     * @param okToShipRegister
     *               Register location containing the OK To Ship flag.
     */
    void SetOkToShipRegister(const UINT16 &okToShipRegister);

    /**
     * <b>Description:</b><br>
     * Store the register containing the OK To Ship flag.
     * <p>
     * @param okToShipRegister
     *               Register location containing the OK To Ship flag.
     */
    void SetOkToShipRegister(const string &okToShipRegister);

    /**
     * <b>Description:</b><br>
     * store the OK to ship bit.
     * <p>
     * @param bit    OK to ship bit index.
     */
    void SetOKToShipBit(const INT32 &bit);

    /**
     * <b>Description:</b><br>
     * Store the register containing the in field mode bit.
     * <p>
     * @param inFieldModeRegister
     *               Register location containing the in field mode bit.
     */
    void SetInFieldModeRegister(const UINT16 inFieldModeRegister);

    /**
     * <b>Description:</b><br>
     * Store the in field mode bit.
     * <p>
     * @param bit    in field mode index.
     */
    void SetInFieldModeBit(const INT32 &inFieldModeBit);

    /**
     * <b>Description:</b><br>
     * Store the flag indicating if module security should be unlocked if a status byte update fails.
     * <p>
     * 
     * @param unlock Flag indicating if module security should be unlocked if a status register update fails.
     */
    void SetUnlockIfUpdateFailed(const bool &unlock = false);

    /**
     * <b>Description:</b><br>
     * Store the A/C status bit.
     * <p>
     * @param bit    A/C status bit.
     */
    void SetACStatusBit(const INT32 &bit);

    /**
     * <b>Description:</b><br>
     * Set the string representation of engine authentication stopped state.
     * <p>
     * @param stoppedString
     *               String representation of engine authentication stopped state.
     */
    void SetEngineAuthenticationStoppedString(const string &stoppedString);

    /**
     * <b>Description:</b><br>
     * Set the string representation of engine authentication running state.
     * <p>
     * @param runningString
     *               String representation of engine authentication running state.
     */
    void SetEngineAuthenticationRunningString(const string &runningString);

    /**
     * <b>Description:</b><br>
     * Set the string representation of engine authentication stop request pending state.
     * <p>
     * @param pendingString
     *               String representation of engine authentication stop request pending state.
     */
    void SetEngineAuthenticationStopPendingString(const string &pendingString);

    /**
     * <b>Description:</b><br>
     * Set the string representation of engine authentication start request pending state.
     * <p>
     * @param pendingString
     *               String representation of engine authentication start request pending state.
     */
    void SetEngineAuthenticationStartPendingString(const string &pendingString);

    /**
     * Flag to signal brake switch On seen.
     */
    bool m_brakeSwitchOn;
    /**
     * Flag to signal brake switch Off seen.
     */
    bool m_brakeSwitchOff;

    /**
     * Flag to signal redundant brake switch On seen.
     */
    bool m_redundantBrakeSwitchOn;

    /**
     * Flag to signal redundant brake switch Off seen.
     */
    bool m_redundantBrakeSwitchOff;

    /**
     * Flag to determine if Reading of the redundant brake switch should be included in the
     * ReadSwitchMonitor background thread.
     */
    bool m_boolMonitorRedundantBrakeSwitch;

    /**
     * Flag to signal Park/Neutral switch On seen.
     */
    bool m_parkNeutralSwitchOn;

    /**
     * Flag to signal Park/Neutral switch Off seen.
     */
    bool m_parkNeutralSwitchOff;

    /**
     * Flag to determine if the Park/Neutral switch should be included in the ReadSwitchMonitor
     * background thread.
     */
    bool m_boolMonitorParkNeutralSwitch;

    /**
     * Flag to signal Clutch switch On seen.
     */
    bool m_clutchSwitchOn;

    /**
     * Flag to signal Clutch switch Off seen.
     */
    bool m_clutchSwitchOff;

    /**
     * Flag to signal clutch upstop switch On seen.
     */
    bool m_clutchUpstopSwitchOn;

    /**
     * Flag to signal clutch upstop switch Off seen.
     */
    bool m_clutchUpstopSwitchOff;

    /**
     * Flag to signal clutch Off and Upstop On seen at the same time.
     */
    bool m_clutchOffUpstopOn;
    /**
     * Flag to determine if Reading of the clutch upstop switch should be included in the
     * ReadSwitchMonitor background thread.
     */
    bool m_boolMonitorClutchUpstopSwitch;

    /**
     * Flag to indicate if the vehicle is equipped with an automatic transmission.
     */
    bool m_automaticTransmission;

    /**
     * Initial throttle position at closed throttle conditions.
     */
    float m_initialTPS;

    /**
     * Final throttle position reading when required delta observed.
     */
    float m_finalTPSReading;

    /**
     * Delta throttle position observed during delta throttle test.
     */
    float m_observedDeltaTPS;

    /**
     * A/C status bit.
     */
    INT32 m_acStatusBit;

    /**
     * OK to ship bit.
     */
    INT32 m_okToShipBit;

    /**
     * Register containing the OK To Ship flag.
     */
    UINT16 m_okToShipRegister;

    /**
     * In-Field mode bit.
     */
    INT32 m_inFieldModeBit;

    /**
     * Register containing the In-Field mode bit.
     */
    UINT16 m_inFieldModeRegister;

    /**
     * Flag to indicate if security should be unlocked if status byte update failed.
     */
    bool m_unlockIfUpdateFailed;
    /**
     * Flag to indicate if the park/neutral switch should be checked.
     */
    bool m_skipParkNeutralSwitchCheck;

    /**
     * Translation from hex code to ASCII engine type.
     */
    XmlNodeMap  m_engineTypes;

    /**
     * Translation from hex code to ASCII vehicle type.
     */
    XmlNodeMap  m_vehicleTypes;

    /**
     * Translation from hex code to ASCII emissions type.
     */
    XmlNodeMap  m_emissionsTypes;

    /**
     * Translation table from hex to ASCII speed control mode.
     */
    XmlNodeMap  m_speedControlModes;
    /**
     * String representation of engine authentication stopped state.
     */
    string m_authenticationStopped;
    /**
     * String representation of engine authentication running state.
     */
    string m_authenticationRunning;
    /**
     * String representation of engine authentication stop pending state.
     */
    string m_authenticationStopPending;
    /**
     * String representation of engine authentication start pending state.
     */
    string m_authenticationStartPending;
};

//*************************************************************************
#endif  // GenericEmissionsModuleTemplate_h
