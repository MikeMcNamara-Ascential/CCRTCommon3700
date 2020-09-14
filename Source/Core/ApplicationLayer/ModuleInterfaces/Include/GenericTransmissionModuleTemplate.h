//******************************************************************************
// Description:
//  Module Interface for a generic transmission control module.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/GenericTransmissionModuleTemplate.h $
// 
// 1     11/01/07 1:52p Derickso
// Updated comments to doxygen format.
// Refactored to use vector form of ReadModuleData for clutch volumes.
// Eliminated functions and variables for clutch volume count and index.
// 
// 3     11/01/07 12:08p Derickso
// Refactored many functions.
// Updated header comments to doxygen format.
// 
// 2     10/01/07 2:40p Derickso
// Added new limp-in monitor for HEV applications.
// 
// 1     8/15/07 12:48p Jsemann
// a.	Added GetShiftLeverLow3String()
// b.	Added SetShiftLeverLow3String()
// c.	Added m_shiftLeverLow3
// 
// 14    1/17/07 2:59p Rwiersem
// Changes for the 07011901 core release:
// 
// - Made the test class a friend.
// - Added OpenCom().
// 
// 13    10/25/06 3:08p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added CloseCom()
// 
// 12    9/05/06 3:44p Rwiersem
// Changes for the 06090601 core release:
// 
// - Removed Set methods for engine authentication strings
// - Removed Get methods for engine authentication strings
// - Removed StartQuickLearnEngineAuthentication()
// - Removed StopQuickLearnEngineAuthentication()
// - Removed member variables for engine authentication strings
// - Made CommonTransmissionTest a friend class
// 
// 11    7/05/06 9:53p Cward
// Support for sensing quick shift out of reverse, and prompt suppression
// to shift to reverse or park whil vehicle is in motion
//
// 10    5/08/06 6:46p Cward
// Added methods for shift lever representation, quick learn commands, and
// quick learn authentication response.
//
// 9     11/11/05 4:54p Cward
// Changes for 0511101 iteration
//
// 8     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 7     9/01/04 8:54p Cward
// Removed inlines from function prototypes
//
// 6     8/18/04 11:04p Cward
// changed includes for template classes to cpp from h
//
// 5     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 4     4/28/04 10:54p Cward
// Changes associated with build 040402801
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:32p Cward
//
// 5     11/13/03 6:06p Derickso
// Latest updates from testing at JTE.
//
// 4     9/16/03 8:06p Derickso
// Updated tag for Gear monitor delay.
//
// 3     9/08/03 7:23a Derickso
// Moved EEPROM_TEST_RESULT enum to GenericMOduleTemplate.
//
// 2     8/27/03 7:31a Derickso
// Corrected compiler errors.
// Compiles, not tested.
//
// 1     8/21/03 8:57a Derickso
// 05WK CCRT Project
//
// 6     8/20/03 6:41a Derickso
// Recombined cpp and h files.  Compiler does not support the
// implementation being seperate for template classes.
//
// 5     8/19/03 9:37a Derickso
// Updated class names.  Added Template to the end of the Generic class
// names to avoid conflicts with code written earlier.
//
// 3     8/14/03 5:55p Derickso
// Split implementation out to cpp file.
//
// 2     5/28/03 9:42a Derickso
// Converted to template.
//
// 1     5/27/03 3:31p Derickso
// Periodic backup.
//
// 1     4/25/03 11:12a Derickso
// Periodic backup.
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericTransmissionModuleTemplate_h
#define GenericTransmissionModuleTemplate_h
//==============================================================================

#include <string>

#include "ProtocolFilter.h"
#include "GenericModuleTemplate.cpp"
#include "GearMonitor.cpp"
#include "LimpInDetailMonitor.cpp"
#include "LimpInFaultMonitor.cpp"
#include "TorqueConverterLockupMonitor.cpp"
#include "GearLockUpMonitor.cpp"
#include "ShiftLeverMonitor.cpp"
//-----------------------------------------------------------------------------

/**
 * Representation of switch state.
 */
const string switchOn("On");
const string switchOff("Off");
//-----------------------------------------------------------------------------


/**
 * Generic Transmission Module Interface Class.<br>
 * Module Interface for a generic transmission control module.
 * This class provides the common methods applicable to all transmission
 * control modules.
 *
 * @author Dan Erickson
 * @see GenericModuleTemplate
 * @since April 09, 2003
 */
template <class ProtocolFilterType>
class GenericTransmissionModuleTemplate : public GenericModuleTemplate<ProtocolFilterType>
{
    friend class CommonTransmissionTest;
    friend class TransmissionModuleInterfacesTest;

public:
    /**
     * Class constructor.<br>
     * The constructor is also responsible for creating protocol filter objects to use in communicating with the vehicle.
     */
    GenericTransmissionModuleTemplate(void);

    /**
     * Class destructor.<br>
     * Any background data monitors that are still running will be commanded to stop.  They will then be deleted along
     * with the communication objects created in the constructor.
     */
    virtual ~GenericTransmissionModuleTemplate();

    /**
     * Closes all communication channels to the module.
     * <p><b>Description:</b><br>
     * Any background data monitors that are still running will be commanded to stop and deleted.  The base class will
     * then be called to complete closing the communication channels.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericModuleTemplate::CloseCom() </li>
     *      </ul>
     */
    virtual void CloseCom();
    
    /**
     * Open the communication channels.
     * <p><b>Description:</b><br>
     * The member variables tracking the transmission data will be reset to initial values.  The base class will then
     * be called to complete opening the modue communication channels.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericModuleTemplate::OpenCom() </li>
     *      </ul>
     * 
     * @return Flag indicating if the serial connection was opened successfully.
     */
    virtual bool OpenCom();
    
    /**
     * Perform the requested action.
     * <p><b>Description:</b><br>
     * The requested action or module command will be completed.
     * <br><b><i>Note:</i></b>  No generic data is supported.  This function will return BEP_STATUS_SOFTWARE and throw a
     * ModuleException if called.
     * <p>
     * <b>Data Items Supported:</b> None<p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * @param methodName Action or command to be completed.
     * 
     * @return Status of completing the requested action.
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName) throw(ModuleException);

    /**
     * GetInfo for string types
     * <p><b>Description:</b><br>
     * String type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadCurrentGear </li>
     *      <li> ReadFeatureModelYear </li>
     *      <li> ReadOverDriveCancelSwitch </li>
     *      <li> ReadQuickLearnStatus </li>
     *      <li> ReadShiftLeverPosition </li>
     *      <li> ReadRawShiftLeverPosition </li>
     *      <li> ReadAutostickPosition </li>
     *      <li> ReadSoftwareModelYear </li>
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
     * GetInfo for unsigned character types
     * <p><b>Description:</b><br>
     * Unsigned character type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadClutchVolumeLRCC </li>
     *      <li> ReadClutchVolume2C </li>
     *      <li> ReadClutchVolumeOD </li>
     *      <li> ReadClutchVolumeUD </li>
     *      <li> ReadClutchVolume1ND </li>
     *      <li> ReadClutchVolumeND </li>
     *      <li> ReadClutchVolume23 </li>
     *      <li> ReadClutchVolumeAlt2C </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 &value) throw(ModuleException);

    /**
     * GetInfo for a vector of unsigned character types
     * <p><b>Description:</b><br>
     * A vector of unsigned character type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadClutchVolumes() </li>
     *      <li> ReadEOLStatusBytes() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadClutchVolumes1 </li>
     *      <li> ReadClutchVolumes2 </li>
     *      <li> ReadEOLStatusBytes </li>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT8>&value) throw(ModuleException);

    /**
     * GetInfo for unsigned short types
     * <p><b>Description:</b><br>
     * Unsigned short type data will be read from the module and returned to the calling function.
     * <br><b><i>Note:</i></b>  No generic data is supported.  This function will return BEP_STATUS_SOFTWARE and throw a
     * ModuleException if called.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 & value) throw(ModuleException);

    /**
     * GetInfo for a vector of unsigned short types
     * <p><b>Description:</b><br>
     * A vector of unsigned short type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadEOLStatusBytes() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadEOLStatusBytes </li>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT16>&value) throw(ModuleException);

    /**
     * GetInfo for integer types
     * <p><b>Description:</b><br>
     * Integer type data will be read from the module and returned to the calling function.
     * <br><b><i>Note:</i></b>  No generic data is supported.  This function will return BEP_STATUS_SOFTWARE and throw a
     * ModuleException if called.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, int &value) throw(ModuleException);

    /**
     * GetInfo for bool types
     * <p><b>Description:</b><br>
     * Boolean type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadLimpInStatus </li>
     *      <li> ReadPRNDLStatus </li>
     *      <li> ReadAutostickConfig </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
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
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadTransmissionVehicleSpeed </li>
     *      <li> ReadTransaxleOilTemp </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);

    /**
     * Update the status byte with the result of a test.
     * <p><b>Description:</b><br>
     * The specified <i>address</i> will be updated with the test result and written to the module.  The specified 
     * <i>testBit</i> will be used to hold the test result.
     * <br><b><i>Note:</i></b> The current value in the module will not be read first.  Care must be taken to not 
     * overwrite existing data in the module.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateEOLStatusByte() </li>
     *      </ul>
     *
     * @param address    Status byte to be updated in the module.
     * @param statusByte Value of the status byte.
     * @param testBit    Bit in the status byte to update.
     * @param testResult Value to update bit with.
     * @return Status of recording the test result in the module.
     */
    virtual BEP_STATUS_TYPE UpdateEEPROMTestBit(const UINT16 &address,
                                                UINT8 &statusByte,
                                                const UINT8 testBit,
                                                const EEPROM_TEST_RESULT testResult);

    /**
     * Update a status byte in the module.
     * <p><b>Description:</b><br> 
     * The specified value will be written to the indicated status byte in the module.
     * <br><b><i>Note:</i></b> The existing value in the module will be overwritten with the provided value.  It is the
     *                         responsility of the calling function to ensure existing bit settings are preserved.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> WriteEOLStatusByte() </li>
     *      </ul>
     *  
     * @param statusByte Status byte to update.
     * @param value      Value to write to the status byte.
     * @return Status of writing the new value to the indicated status byte.
     */
    virtual BEP_STATUS_TYPE UpdateEOLStatusByte(const UINT16 &statusByte, const UINT8 &value);
    /**
     * Write data to the EOL status registers.
     * <p><b>Description:</b><br>
     * The provided register value will be written to the specified status register.
     * <br><b><i>Note:</i></b> There is no generic implementation as many modules handle recording test results
     *                         differently.
     * 
     * @param eolStatusRegister
     *               EOL status register to be updated.
     * @param registerValue
     *               Value to record in the specified EOL status register.
     * 
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE WriteEOLStatusByte(const UINT16 &eolStatusRegister, const UINT8 &registerValue);

    /**
     * Store the flag indicating if all gears have been observed.
     * <p><b>Description:</b><br>
     * The flag indicating all available transmission gears will be set to the provided value.  If all transmission
     * gears have been observed, this will typically signal the gear monitor to stop monitoring transmission gears.
     * <p>
     * @param allGearsObserved  Flag indicating if all transmission gears have been observed.
    */
    virtual void SetAllGearsSeen(const bool &allGearsObserved);

    /**
     * Store the data indicating if all TCC ock states have been observed.
     * <p><b>Description:</b><br>
     * The flag indicating if all torque converter clutch lock states have been observed will be stored.  If all lock 
     * states have been observed, the TCC lock state monitor will typically stop monitoring TCC lock states.
     * <p>
     * @param allLockStatesObserved   Flag indicating that if all torque converter clutch lock states have been observed.
    */
    virtual void SetAllLockStatesSeen(const bool &allLockStatesObserved);

    /**
     * Return the name of the class.
     * <p><b>Description:</b><br>
     * The name of the class will be returned to the calling function.
     *
     * @returns Generic Transmission Control Module
     */
    virtual string ModuleName();

    /**
     * Monitor transmisison gears.
     * <p><b>Description:</b><br>
     * If all transmission gears have not yet been observed, the current gear will be read from the module.  The gear
     * will then be processed to determine if it has already been detected.  If this is a hybrid vehicle, the failure
     * register can optionally be read to determine if any gears were not observed due to vehicle issues.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> ReadHevFailureData - Flag to indicate if the hybrid failure data should be read. </li>
     *      </ul>
     * 
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadCurrentGear </li>
     *      <li> ReadFailureRegister </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> AllGearsObserved() </li>
     *      <li> ReadModuleData() </li>
     *      <li> CommandModule() </li>
     *      <li> ProcessCurrentGear() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param filter ProtocolFilter object to use for reading the current transmission gear.
     * @return Status of monitoring the transmission gears.
     */
    virtual BEP_STATUS_TYPE MonitorTransmissionGear(IProtocolFilter *filter);

    /**
     * Monitor the limp-in and fault status in the module.
     * <p><b>Description:</b><br>
     * The limp-in status will be read from the transmission.  If the transmission is in limp-in mode, the test sequence
     * will be aborted in order to prevent damage to the vehicle.  If the transmission is not in limp-in mode, the faults
     * will be read from the module.  If any faults read from the module are not in the ignored faults list, the fault
     * will be reported to the TestResultServer and the test sequence will be aborted.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadLimpInStatus </li>
     *      </ul>
     * 
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> LimpInPrompt - Prompt to be displayed if the transmission is in limp-in mode. </li>
     *      <li> ModuleFaultsDetectedPrompt - Prompt to be displayed if faults are detected in the module. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> ReadFaults() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param filter Communication object to use for monitoring the status of the TCM.
     * @return Status of monitoring the limp-in and fault status of the transmission.
     */
    virtual BEP_STATUS_TYPE MonitorLimpInFaultStatus(IProtocolFilter *filter);

    /**
     * Read faults stored in the module.
     * <p><b>Description:</b><br>
     * The number of faults stored in the module will first be read.  If faults have been stored in the module, then
     * the faults will be read from the module.  The provided communication object will be used to read the fault data
     * from the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadFaults </li>
     *      <li> ReadNumberOfFaults </li>
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
     *      <li> ReadFaults() </li>
     *      </ul>
     * 
     * @param filter     Communication object to use to read the fault data from the module.
     * @param faultCodes Fault Codes read from the module.
     * 
     * @return Status of reading the fault data from the module.
     */
    virtual BEP_STATUS_TYPE ReadFaults(IProtocolFilter *filter, FaultVector_t &faultCodes);

    /**
     * Monitor transmission data.  
     * <p><b>Description:</b><br>
     * The transmission data will be monitored.  If all gears and TCC lockup states have not been observed, the current
     * gear and current lockup state will be read from the module and processed.  This function is intended to be used
     * for modules that return the current transmission gear and TCC lock state in the same message.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadTransmissionData </li>
     *      <li> ReadTorqueConverterStatus </li>
     *      <li> ReadCurrentGear </li>
     *      </ul>
     * 
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> AllGearsObserved() </li>
     *      <li> AllLockStatesObserved() </li>
     *      <li> ParseStringResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ProcessTCCLockupState() </li>
     *      <li> ProcessCurrentGear() </li>
     *      </ul>
     * 
     * @param filter  Communication object to use for monitoring the transmission data.
     * @return  Status of monitoring the transmission data.
     */
    virtual BEP_STATUS_TYPE MonitorTransmissionData(IProtocolFilter *filter);

    /**
     * Check the lockup status of the torque converter clutch.
     * <p><b>Description:</b><br>
     * The torque converter lockup state will be monitored.  The current torque converter clutch lock state will be read
     * from the module if all lock states have not yet been observed.  The current lock state will then be processed.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadTorqueConverterStatus </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TCCNoLockupObserved() </li>
     *      <li> TCCPartialLockupObserved() </li>
     *      <li> TCCFullLockupObserved() </li>
     *      <li> ReadModuleData() </li>
     *      <li> ProcessTCCLockupState() </li>
     *      </ul>
     *
     * @param filter Communication object to use for monitoring the torque converter clutch lockup state.
     * @return Status of monitoring the TCC lock up states.
     */
    virtual BEP_STATUS_TYPE MonitorTorqueConverterLockupStatus(IProtocolFilter *filter);

    /**
     * Start the quicklearn procedure.
     * <p><b>Description:</b><br>
     * Signal the module to begin the quicklearn procedure.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> StartQuickLearn </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     * 
     * @return Status of commanding the module to start the quicklearn procedure.
     */
    virtual BEP_STATUS_TYPE StartQuickLearn();

    /**
     * Stop the quicklearn procedure.
     * <p><b>Description:</b><br>
     * Signal the module to stop the quicklearn procedure.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> StopQuickLearn </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     * 
     * @return Status of commanding the module to stop the quicklearn procedure.
     */
    virtual BEP_STATUS_TYPE StopQuickLearn();
    /**
     * Clear the diagnostic Tell-Tales from the module.
     * <p><b>Description:</b><br>
     * The module will be commanded to clear the diagnostic retention information.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ClearDiagnosticRetention </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of clearing the diagnostic retention.
     */
    virtual BEP_STATUS_TYPE ClearDiagnosticRetention(void);

    /**
     * Start the Limp-In and Fault monitor thread.
     * <p><b>Description:</b><br>
     * The list of ignored faults and the fault definitions will be saved for reporting purposes.  A new Limp-in/Fault
     * monitor will be created and started.
     * <p>
     * 
     * @param ignoredFaults
     *                  List of faults to ignore.
     * @param faultList Fault definitions and code.
     * 
     * @return Status of starting the Limp-in/Fault monitor thread.
     * @see LimpInFaultMonitor
     */
    virtual BEP_STATUS_TYPE StartLimpInFaultMonitor(XmlNodeMap *ignoredFaults, XmlNodeMap *faultList);

    /**
     * Stop the Limp-In and Fault monitor thread.
     * <p><b>Description:</b><br>
     * The background data monitor that is moniitoring the module for faults and limp-in status will be stopped 
     * and deleted.  The comm object being used for the monitor will be commanded to close its communication channel.
     * <p>
     * @return Status of stopping the Limp-in/Fault monitor.
     */
    virtual BEP_STATUS_TYPE StopLimpInFaultMonitor(void);

    /**
     * Begin monitoring transmission gears.
     * <p><b>Description:</b><br>
     * Start the thread to monitor gears from the transmission.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> GearMonitorDelay - Amount of time(ms) to wait between gear reads. </li>
     *      </ul>
     * 
     * @return Status of starting the background gear monitor thread.
     * @see GearMonitor
     */
    virtual BEP_STATUS_TYPE StartGearMonitor(void);

    /**
     * Stop monitoring transmission gears.
     * <p><b>Description:</b><br>
     * Stop the thread monitoring transmission gears.  The background monitor thread will be deleted and the communication
     * object being used to monitor transmission gears will be commanded to close its communication channel.
     * <p>
     *
     * @return Status of stopping the transmission gear monitor.
     */
    virtual BEP_STATUS_TYPE StopGearMonitor(void);

    /**
     * Begin monitoring torque converter clutch lock states.
     * <p><b>Description:</b><br>
     * Start the thread to monitor the torque converter clutch lockup status.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> TCCMonitorDelay - Amount of time(ms) to wait between TCC state reads. </li>
     *      </ul>
     * 
     * @return Status of starting the background thread to monitor torque converter clutch states.
     * @see TorqueConverterLockupMonitor
     */
    virtual BEP_STATUS_TYPE StartLockupMonitor(void);

    /**
     * Stop monitoring torque converter clutch lock states.
     * <p><b>Description:</b><br>
     * Stop the thread that is monitoring the torque converter clutch lockup status.  The background monitor will be
     * deleted.  The communication object being used to read the TCC states from the module will be commanded to close
     * its communication channel.
     * <p>
     * @return Status of stopping the torque converter clutch monitor.
     */
    virtual BEP_STATUS_TYPE StopLockupMonitor(void);

    /**
     * Begin monitoring gears and TCC states.
     * <p><b>Description:</b><br>
     * Start the thread to monitor transmission data.  The current transmission gear and torque converter lockup state 
     * will be monitored using the thread that is created.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> GearMonitorDelay - Amount of time(ms) to wait between transmission data reads. </li>
     *      </ul>
     * 
     * @return Status of starting the gear and lock state monitor.
     * @see GearLockUpMonitor
     */
    virtual BEP_STATUS_TYPE StartGearLockUpMonitor(void);

    /**
     * Stop monitoring gears and TCC lock states.
     * <p><b>Description:</b><br>
     * Stop the thread that is monitoring transmission data.  The background monitor will be deleted and the communciation
     * channel being utilized by the thread will be closed.
     * <p>
     * @return Status of stopping the gear and TCC lock state monitor.
     */
    virtual BEP_STATUS_TYPE StopGearLockUpMonitor(void);

    /**
     * Monitor the shift lever position.
     * <p><b>Description:</b><br>
     * If all shift lever positions have not been observed, the current shift lever position will be read from the 
     * transmission control module.  If the shift lever position is successfully read from the module, the position 
     * will be processed to determine if all required shift lever positions have ben observed.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadShiftLeverPosition </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> AllShiftLeverPositionsObserved() </li>
     *      <li> ReadModuleData() </li>
     *      <li> ProcessCurrentShiftLeverPosition() </li>
     *      </ul>
     * 
     * @param filter ProtocolFilter object to use for reading the current transmission shift lever position.
     *
     * @return Status of reading the current shift lever position from the module.
     */
    virtual BEP_STATUS_TYPE MonitorTransmissionShiftLever(IProtocolFilter *filter);
    /**
     * Store the flag indicating if all transmission shift lever positions have been observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if all transmission shift lever positions have been observed will be stored.  This flag
     * is used to determine if the background shift lever monitor should continue reading and processing shift lever
     * positions.
     * <p>
     * 
     * @param allShiftLeverPositionsObserved
     *               Flag indicating if all required transmission shift lever positions have been observed.
     */
    virtual void SetAllShiftLeverPositionsSeen(const bool &allShiftLeverPositionsObserved);
    /**
     * Begin monitoring shift lever positions.
     * <p><b>Description:</b><br>
     * Start the thread to monitor shift lever positions from the transmission.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> ShiftLeverMonitorDelay - Amount of time(ms) to wait between shift lever reads. </li>
     *      </ul>
     * 
     * @return Status of starting the background shift lever monitor.
     * @see ShiftLeverMonitor
     */
    virtual BEP_STATUS_TYPE StartShiftLeverMonitor(void);
    /**
     * Stop monitoring the shift lever positions.
     * <p><b>Description:</b><br>
     * Stop the thread monitoring transmission shift lever positions.  The thread will be deleted and the communication 
     * channel being utilized by the thread will be commanded to close.
     * <p>
     * @return Status of stopping the shift lever monitor.
     */
    virtual BEP_STATUS_TYPE StopShiftLeverMonitor(void);

    /**
     * Get the flag indicating if first gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if first gear was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if first gear was observed.
     */
    const bool &Gear1Observed(void);

    /**
     * Get the flag indicating if second gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if second gear was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if second gear was observed.
     */
    const bool &Gear2Observed(void);

    /**
     * Get the flag indicating if third gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if third gear was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if third gear was observed.
     */
    const bool &Gear3Observed(void);

    /**
     * Get the flag indicating if fourth gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if fourth gear was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if fourth gear was observed.
     */
    const bool &Gear4Observed(void);

    /**
     * Get the flag indicating if fifth gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if fifth gear was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if fifth gear was observed.
     */
    const bool &Gear5Observed(void);

    /**
     * Get the flag indicating if sixth gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if sixth gear was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if sixth gear was observed.
     */
    const bool &Gear6Observed(void);

    /**
     * Get the flag indicating if park was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if park was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if park was observed.
     */
    const bool &GearParkObserved(void);

    /**
     * Get the flag indicating if reverse gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if reverse gear was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if reverse gear was observed.
     */
    const bool &GearReverseObserved(void);

    /**
     * Get the flag indicating if neutral gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if neutral gear was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if neutral gear was observed.
     */
    const bool &GearNeutralObserved(void);

    /**
     * Get the flag indicating if Torque Converter clutch No-Lockup state was observed.
     * <p><b>Description:</b><br>
     * The flag indicating if the torque converter clutch no-lock state was observed will be returned to the calling
     * function.
     * <p>
     * @return Flag indicating if the torque converter clutch no lockup state was observed.
     */
    const bool &TCCNoLockupObserved(void);

    /**
     * Get the flag indicating if Torque Converter clutch Partial-Lockup state was observed.
     * <p><b>Description:</b><br>
     * The flag indicating if the torque converter clutch Partial-lock state was observed will be returned to the calling
     * function.
     * <p>
     * @return Flag indicating if the torque converter clutch partial lockup state was observed.
     */
    const bool &TCCPartialLockupObserved(void);

    /**
     * Get the flag indicating if Torque Converter clutch Full-Lockup state was observed.
     * <p><b>Description:</b><br>
     * The flag indicating if the torque converter clutch Full-lock state was observed will be returned to the calling
     * function.
     * <p>
     * @return Flag indicating if the torque converter clutch full lockup state was observed.
     */
    const bool &TCCFullLockupObserved(void);

    /**
     * Get the flag indicating if the drive shift lever position was observed.
     * <p><b>Description:</b><br>
     * The flag indicating if the drive shift lever position was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if the drive shift lever position was observed.
     */
    const bool &ShiftLeverDriveObserved(void);
    /**
     * Get the flag indicating if the low shift lever position was observed.
     * <p><b>Description:</b><br>
     * The flag indicating if the low shift lever position was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if the low shift lever position was observed.
     */
    const bool &ShiftLeverLowObserved(void);
    /**
     * Get the flag indicating if the low/3 shift lever position was observed.
     * <p><b>Description:</b><br>
     * The flag indicating if the low/3 shift lever position was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if the low/3 shift lever position was observed.
     */
    const bool &ShiftLeverLow3Observed(void);
    /**
     * Get the flag indicating if the park shift lever position was observed.
     * <p><b>Description:</b><br>
     * The flag indicating if the park shift lever position was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if the park shift lever position was observed.
     */
    const bool &ShiftLeverParkObserved(void);
    /**
     * Get the flag indicating if the reverse shift lever position was observed.
     * <p><b>Description:</b><br>
     * The flag indicating if the reverse shift lever position was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if the reverse shift lever position was observed.
     */
    const bool &ShiftLeverReverseObserved(void);
    /**
     * Get the flag indicating if the neutral shift lever position was observed.
     * <p><b>Description:</b><br>
     * The flag indicating if the neutral shift lever position was observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if the neutral shift lever position was observed.
     */
    const bool &ShiftLeverNeutralObserved(void);
    /**
     * Check if all required ShiftLevers have been observed.
     * <p><b>Description:</b><br>
     * The flag indicating all required shift lever positions have been observed will be returned to the calling function.
     * <p>
     * @return Flag indicating if all required shift lever positions have been observed.
     */
    virtual bool& AllShiftLeverPositionsObserved(void);
    /**
     * Get the string representation for Shift Lever Position in Park.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Park will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Park.
     */
    const string &GetShiftLeverParkString(void);

    /**
     * Get the string representation for Shift Lever Position in Neutral.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Neutral will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Neutral.
     */
    const string &GetShiftLeverNeutralString(void);

    /**
     * Get the string representation for Shift Lever Position in Reverse.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Reverse will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Reverse.
     */
    const string &GetShiftLeverReverseString(void);

    /**
     * Get the string representation for Shift Lever Position in Drive.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Drive will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Drive.
     */
    const string &GetShiftLeverDriveString(void);

    /**
     * Get the string representation for Shift Lever Position in Auto-Stick Up.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Auto-Stick Up will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Auto-Stick Up.
     */
    const string &GetShiftLeverUpString(void);

    /**
     * Get the string representation for Shift Lever Position in Auto-Stick Down.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Auto-Stick Down will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Auto-Stick Down.
     */
    const string &GetShiftLeverDownString(void);

    /**
     * Get the string representation for Shift Lever Position in Auto-Stick Open.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Auto-Stick Open will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Auto-Stick Open.
     */
    const string &GetShiftLeverOpenString(void);

    /**
     * Get the string representation for Shift Lever Position in Low.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Low will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Low.
     */
    const string &GetShiftLeverLowString(void);

    /**
     * Get the string representation for Shift Lever Position in First.
     * <p><b>Description:</b><br>
     * The string representing shift lever position First will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in First.
     */
    const string &GetShiftLeverFirstString(void);
    /**
     * Get the string representation for Shift Lever Position in Low-3.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Low-3 will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Low-3.
     */
    const string &GetShiftLeverLow3String(void);
	/**
     * Get the string representation for Shift Lever Position in Second.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Second will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Second.
     */
    const string &GetShiftLeverSecondString(void);

    /**
     * Get the string representation for Shift Lever Position in Third.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Third will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Third.
     */
    const string &GetShiftLeverThirdString(void);

    /**
     * Get the string representation for Shift Lever Position in Autostick.
     * <p><b>Description:</b><br>
     * The string representing shift lever position Autostick will be returned to the calling function.
     * <p>
     * @return String representation for shift lever position in Autostick.
     */
    const string &GetShiftLeverAutostickString(void);
    /**
     * Get the string representation for first gear.
     * <p><b>Description:</b><br>
     * The string representation for first gear will be returned to the calling function.
     * <p>
     * @return String representation for first gear.
     */
    const string &GetGear1String(void);

    /**
     * Get the string representation for second gear.
     * <p><b>Description:</b><br>
     * The string representation for second gear will be returned to the calling function.
     * <p>
     * @return String representation for second gear.
     */
    const string &GetGear2String(void);

    /**
     * Get the string representation for third gear.
     * <p><b>Description:</b><br>
     * The string representation for third gear will be returned to the calling function.
     * <p>
     * @return String representation for third gear.
     */
    const string &GetGear3String(void);

    /**
     * Get the string representation for fourth gear.
     * <p><b>Description:</b><br>
     * The string representation for fourth gear will be returned to the calling function.
     * <p>
     * @return String representation for fourth gear.
     */
    const string &GetGear4String(void);

    /**
     * Get the string representation for fifth gear.
     * <p><b>Description:</b><br>
     * The string representation for fifth gear will be returned to the calling function.
     * <p>
     * @return String representation for fifth gear.
     */
    const string &GetGear5String(void);

    /**
     * Get the string representation for sixth gear.
     * <p><b>Description:</b><br>
     * The string representation for sixth gear will be returned to the calling function.
     * <p>
     * @return String representation for sixth gear.
     */
    const string &GetGear6String(void);

    /**
     * Get the string representation for park gear.
     * <p><b>Description:</b><br>
     * The string representation for park gear will be returned to the calling function.
     * <p>
     * @return String representation for park gear.
     */
    const string &GetGearParkString(void);

    /**
     * Get the string representation for reverse gear.
     * <p><b>Description:</b><br>
     * The string representation for reverse gear will be returned to the calling function.
     * <p>
     * @return String representation for reverse gear.
     */
    const string &GetGearReverseString(void);

    /**
     * Get the string representation for neutral gear.
     * <p><b>Description:</b><br>
     * The string representation for neutral gear will be returned to the calling function.
     * <p>
     * @return String representation for neutral gear.
     */
    const string &GetGearNeutralString(void);

    /**
     * Get the string representation for the Torque Converter Clutch No Lock state.
     * <p><b>Description:</b><br>
     * The string representation for the Torque Converter Clutch No Lock state will be returned to the calling function.
     * <p>
     * @return String representation for no lock state.
     */
    const string &GetTCCNoLockString(void);

    /**
     * Get the string representation for the Torque Converter Clutch Partial Lock state.
     * <p><b>Description:</b><br>
     * The string representation for the Torque Converter Clutch Partial Lock state will be returned to the calling function.
     * <p>
     * @return String representation for partial lock state.
     */
    const string &GetTCCPartialLockString(void);

    /**
     * Get the string representation for the Torque Converter Clutch Full Lock state.
     * <p><b>Description:</b><br>
     * The string representation for the Torque Converter Clutch Full Lock state will be returned to the calling function.
     * <p>
     * @return String representation for full lock state.
     */
    const string &GetTCCFullLockString(void);

    /**
     * Check if all required gears have been observed.
     * <p><b>Description:</b><br>
     * The flag indicating if all required transmission gears have been observed will be returned to the calling function.
     * <p>
     * @return flag indicating if all required gears have been observed.
     */
    virtual bool& AllGearsObserved(void);

    /**
     * Check if all Torque Converter Clutch lock states have been observed.
     * <p><b>Description:</b><br>
     * The flag indicating if all torque converter clutch states have been observed will be returned to the calling function.
     * <p>
     * @return  Flag indicating if all required lock states have been observed.
     */
    virtual bool& AllLockStatesObserved(void);

    /**
     * Set the flag indicating if first gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if first gear was observed will be stored.
     * <p>
     * @param observedGear1
     *               Flag indicating if first gear was observed.
     */
    void SetObservedGear1(const bool &observedGear1);

    /**
     * Set the flag indicating if second gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if second gear was observed will be stored.
     * <p>
     * @param observedGear2
     *               Flag indicating if second gear was observed.
     */
    void SetObservedGear2(const bool &observedGear2);

    /**
     * Set the flag indicating if third gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if third gear was observed will be stored.
     * <p>
     * @param observedGear3
     *               Flag indicating if third gear was observed.
     */
    void SetObservedGear3(const bool &observedGear3);

    /**
     * Set the flag indicating if fourth gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if fourth gear was observed will be stored.
     * <p>
     * @param observedGear4
     *               Flag indicating if fourth gear was observed.
     */
    void SetObservedGear4(const bool &observedGear4);

    /**
     * Set the flag indicating if fifth gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if fifth gear was observed will be stored.
     * <p>
     * @param observedGear5
     *               Flag indicating if fifth gear was observed.
     */
    void SetObservedGear5(const bool &observedGear5);

    /**
     * Set the flag indicating if sixth gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if sixth gear was observed will be stored.
     * <p>
     * @param observedGear6
     *               Flag indicating if sixth gear was observed.
     */
    void SetObservedGear6(const bool &observedGear6);

    /**
     * Set the flag indicating if park gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if park gear was observed will be stored.
     * <p>
     * @param observedGearPark
     *               Flag indicating if park gear was observed.
     */
    void SetObservedGearPark(const bool &observedGearPark);

    /**
     * Set the flag indicating if reverse gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if reverse gear was observed will be stored.
     * <p>
     * @param observedGearReverse
     *               Flag indicating if reverse gear was observed.
     */
    void SetObservedGearReverse(const bool &observedGearReverse);

    /**
     * Set the flag indicating if neutral gear was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if neutral gear was observed will be stored.
     * <p>
     * @param observedGearNeutral
     *               Flag indicating if neutral gear was observed.
     */
    void SetObservedGearNeutral(const bool &observedGearNeutral);

    /**
     * Set the flag indicating Torque Converter Clutch No-Lockup state was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if Torque Converter Clutch No-Lockup state was observed will be stored.
     * <p>
     * @param observedTCCNoLock
     *               Flag indicating if Torque Converter Clutch No-Lockup state was observed.
     */
    void SetObservedTCCNoLockState(const bool &observedTCCNoLock);

    /**
     * Set the flag indicating Torque Converter Clutch Partial-Lockup state was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if Torque Converter Clutch Partial-Lockup state was observed will be stored.
     * <p>
     * @param observedTCCPartialLock
     *               Flag indicating if Torque Converter Clutch Partial-Lockup state was observed.
     */
    void SetObservedTCCPartialLockState(const bool &observedTCCPartialLock);

    /**
     * Set the flag indicating Torque Converter Clutch Full-Lockup state was observed.
     * <p><b>Description:</b><br>
     * The data flag indicating if Torque Converter Clutch Full-Lockup state was observed will be stored.
     * <p>
     * @param observedTCCFullLock
     *               Flag indicating if Torque Converter Clutch Full-Lockup state was observed.
     */
    void SetObservedTCCFullLockState(const bool &observedTCCFullLock);

     /**
     * Monitor the different statuses of the AT learn procedure
     * <p><b>Description:</b><br>
     * The data flag indicating if Torque Converter Clutch Full-Lockup state was observed will be stored.
     * <p>
     * @param 
     */
    virtual BEP_STATUS_TYPE MonitorATLearnData(float &accelPedalPos, string &converterStatus, string &currentGear);


    virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);

protected:
    /**
     * Intialize the module interface.
     * <p><b>Description:</b><br>
     * The shift lever representations will be stored as well as the gear and torque converter lock state representations.
     * The communication objects to use for the various background monitors will also be initialized.
     * The base class will then be called to complete the initialization.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> ShiftLeverStringRepresentation/Park - String representation for shift lever position park. </li>
     *      <li> ShiftLeverStringRepresentation/Neutral - String representation for shift lever position neutral. </li>
     *      <li> ShiftLeverStringRepresentation/Reverse - String representation for shift lever position reverse. </li>
     *      <li> ShiftLeverStringRepresentation/Drive - String representation for shift lever position drive. </li>
     *      <li> ShiftLeverStringRepresentation/Low3 - String representation for shift lever position Low-3. </li>
     *      <li> ShiftLeverStringRepresentation/Up - String representation for shift lever position Up. </li>
     *      <li> ShiftLeverStringRepresentation/Down - String representation for shift lever position Down. </li>
     *      <li> ShiftLeverStringRepresentation/Open - String representation for shift lever position Open. </li>
     *      <li> ShiftLeverStringRepresentation/Low - String representation for shift lever position Low. </li>
     *      <li> ShiftLeverStringRepresentation/First - String representation for shift lever position first. </li>
     *      <li> ShiftLeverStringRepresentation/Second - String representation for shift lever position second. </li>
     *      <li> ShiftLeverStringRepresentation/Third - String representation for shift lever position third. </li>
     *      <li> ShiftLeverStringRepresentation/Autostick - String representation for shift lever position autostick. </li>
     *      <li> GearStringRepresentation/First - String representation for first gear. </li>
     *      <li> GearStringRepresentation/Second - String representation for second gear. </li>
     *      <li> GearStringRepresentation/Third - String representation for third gear. </li>
     *      <li> GearStringRepresentation/Fourth - String representation for fourth gear. </li>
     *      <li> GearStringRepresentation/Fifth - String representation for fifth gear. </li>
     *      <li> GearStringRepresentation/Sixth - String representation for sixth gear. </li>
     *      <li> GearStringRepresentation/Park - String representation for park gear. </li>
     *      <li> GearStringRepresentation/Reverse - String representation for reverse gear. </li>
     *      <li> GearStringRepresentation/Neutral - String representation for neutral gear. </li>
     *      <li> TCCLockStateStringRepresentation/NoLock - String representation for TCC no lock state. </li>
     *      <li> TCCLockStateStringRepresentation/PartialLock - String representation for TCC partial lock state. </li>
     *      <li> TCCLockStateStringRepresentation/FullLock - String representation for TCC full lock state. </li>
     *      <li> CheckTechmLimpIn - Flag indicating if limp-in is being checked on a hybrid vehicle. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetShiftLeverParkString() </li>
     *      <li> SetShiftLeverNeutralString() </li>
     *      <li> SetShiftLeverReverseString() </li>
     *      <li> SetShiftLeverDriveString() </li>
     *      <li> SetShiftLeverLow3String() </li>
     *      <li> SetShiftLeverUpString() </li>
     *      <li> SetShiftLeverDownString() </li>
     *      <li> SetShiftLeverOpenString() </li>
     *      <li> SetShiftLeverLowString() </li>
     *      <li> SetShiftLeverFirstString() </li>
     *      <li> SetShiftLeverSecondString() </li>
     *      <li> SetShiftLeverThirdString() </li>
     *      <li> SetGear1String() </li>
     *      <li> SetGear2String() </li>
     *      <li> SetGear3String() </li>
     *      <li> SetGear4String() </li>
     *      <li> SetGear5String() </li>
     *      <li> SetGear6String() </li>
     *      <li> SetGearParkString() </li>
     *      <li> SetGearReverseString() </li>
     *      <li> SetGearNeutralString() </li>
     *      <li> SetTccNoLockString() </li>
     *      <li> SetTccPartialLockString() </li>
     *      <li> SetTccFullLockString() </li>
     *      <li> SetCheckTechmLimpIn() </li>
     *      <li> GenericModuleTemplate::InitializeHook() </li>
     *      </ul>
     *
     * @param configNode Data to use for initializing the module interface.
     * @return Flag indicating if initialization has completed.
     */
    virtual bool InitializeHook(const XmlNode *configNode);

    /**
     * Process the current lock state read from the transmission.
     * <p><b>Description:</b><br>
     * The appropriate flags will be set to indicate the current lock state of the Torque Converter Clutch.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TCCNoLockupObserved() </li>
     *      <li> SetObservedTCCNoLockState() </li>
     *      <li> GetTCCNoLockString() </li>
     *      <li> TCCPartialLockupObserved() </li>
     *      <li> SetObservedTCCPartialLockState() </li>
     *      <li> GetTCCPartialLockString() </li>
     *      <li> TCCFullLockupObserved() </li>
     *      <li> SetObservedTCCFullLockState() </li>
     *      <li> GetTCCFullLockString() </li>
     *      </ul>
     *
     * @param lockState     Current lock state of the Torque Converter Clutch
     */
    virtual void ProcessTCCLockupState(const string &lockState);

    /**
     * Process the current transmission gear read from the module.
     * <p><b>Description:</b><br>
     * The appropriate flags will be set to indicate the current gear so that all observed gears can be tracked.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> Gear1Observed() </li>
     *      <li> SetObservedGear1() </li>
     *      <li> GetGear1String() </li>
     *      <li> Gear2Observed() </li>
     *      <li> SetObservedGear2() </li>
     *      <li> GetGear2String() </li>
     *      <li> Gear3Observed() </li>
     *      <li> SetObservedGear3() </li>
     *      <li> GetGear3String() </li>
     *      <li> Gear4Observed() </li>
     *      <li> SetObservedGear4() </li>
     *      <li> GetGear4String() </li>
     *      <li> Gear5Observed() </li>
     *      <li> SetObservedGear5() </li>
     *      <li> GetGear5String() </li>
     *      <li> Gear6Observed() </li>
     *      <li> SetObservedGear6() </li>
     *      <li> GetGear6String() </li>
     *      <li> GearParkObserved() </li>
     *      <li> SetObservedGearPark() </li>
     *      <li> GetGearParkString() </li>
     *      <li> GearReverseObserved() </li>
     *      <li> SetObservedGearReverse() </li>
     *      <li> GetGearReverseString() </li>
     *      <li> GearNeutralObserved() </li>
     *      <li> SetObservedGearNeutral() </li>
     *      <li> GetGearNeutralString() </li>
     *      </ul>
     * 
     * @param  currentGear  Current transmission gear.
     */
    virtual void ProcessCurrentGear(const string &currentGear);
    /**
     * Read the EOL status bytes from the module.
     * <p><b>Description:</b><br>
     * The EOL status bytes will be read from the module.
     * <br><b><i>Note:</i></b> Many modules handle EOL status bytes differently.  There is no generic implementation.
     * <p>
     * @param statusBytes
     *               Status bytes read from the module.
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE ReadEOLStatusBytes(vector<UINT8> &statusBytes);

    /**
     * Read the EOL status bytes from the module.
     * <p><b>Description:</b><br>
     * The EOL status bytes will be read from the module.
     * <br><b><i>Note:</i></b> Many modules handle EOL status bytes differently.  There is no generic implementation.
     * <p>
     * @param statusBytes
     *               Status bytes read from the module.
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE ReadEOLStatusBytes(vector<UINT16> &statusBytes);

    /**
     * Read faults stored in the module.
     * <p><b>Description:</b><br>
     * The fault data will be read from the module.
     * <br><b><i>Note:</i></b> Many modules handle fault data differently.  There is no generic implementation.
     * <p>
     * @param faultCodes Fault Codes read from the module.
     * @return BEP_STATUS_SOFTWARE
     */
    //virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);

    /**
     * Read the transmission clutch volumes.
     * <p><b>Description:</b><br>
     * Read the transmission clutch volumes from the module.  This is typically done after the quicklearn procedure
     * in order to report the clutch volumes to the plant host system.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * @param clutchVolumeMessage
     *               Message to use for reading the clutch volumes.
     * @param clutchVolumes
     *               Clutch volumes read from the module.
     * @return Status of reading the clutch volumes from the module.
     */
    virtual BEP_STATUS_TYPE ReadClutchVolumes(const string clutchVolumeMessage, vector<UINT8> &clutchVolumes);

    /**
     * Communication object to use for monitoring the current transmission gear.
     */
    ProtocolFilterType *m_gearMonitorComm;
    /**
     * Gear Monitor object
     */
    GearMonitor<ProtocolFilterType> *m_gearMonitor;
    /**
     * Communication object to use for monitoring the limp-in status
     * and fault status of the module.
     */
    ProtocolFilterType *m_limpInFaultMonitorComm;
    /**
     * DTC Fault list.
     */
    XmlNodeMap *m_faultList;
    /**
     * Object to use for monitoring limp-in status and fault status.
     */
    LimpInFaultMonitor<ProtocolFilterType> *m_limpInFaultMonitor;
    /**
     * Communication object to use for monitoring the torque converter lockup status
     * and fault status of the module.
     */
    ProtocolFilterType *m_tccLockupMonitorComm;
    /**
     * Object to use for monitoring the lockup status of the torque
     * converter clutch.
     */
    TorqueConverterLockupMonitor<ProtocolFilterType> *m_tccLockupMonitor;
    /**
     * Object to use for monitoring transmission data.
     */
    GearLockUpMonitor<ProtocolFilterType> *m_gearLockUpMonitor;
    /**
     * Communication object to use for monitoring transmission data.
     */
    ProtocolFilterType *m_gearLockUpMonitorComm;
    /**
     * ShiftLever Monitor object
     */
    ShiftLeverMonitor<ProtocolFilterType> *m_shiftLeverMonitor;
    /**
     * Communication object to use for monitoring the current transmission ShiftLever.
     */
    ProtocolFilterType *m_shiftLeverMonitorComm;
    /**
     * Process the current transmission Shift Lever position.
     * <p><b>Description:</b><br>
     * The appropriate flags will be set to indicate the current shift lever position so that all observed shift lever 
     * positions can be tracked.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShiftLeverDriveObserved() </li>
     *      <li> SetObservedShiftLeverDrive() </li>
     *      <li> GetShiftLeverDriveString() </li>
     *      <li> ShiftLeverLowObserved() </li>
     *      <li> SetObservedShiftLeverLow() </li>
     *      <li> GetShiftLeverLowString() </li>
     *      <li> ShiftLeverLow3Observed() </li>
     *      <li> SetObservedShiftLeverLow3() </li>
     *      <li> GetShiftLeverLow3String() </li>
     *      <li> ShiftLeverParkObserved() </li>
     *      <li> SetObservedShiftLeverPark() </li>
     *      <li> GetShiftLeverParkString() </li>
     *      <li> ShiftLeverReverseObserved() </li>
     *      <li> SetObservedShiftLeverReverse() </li>
     *      <li> GetShiftLeverReverseString() </li>
     *      <li> ShiftLeverNeutralObserved() </li>
     *      <li> SetObservedShiftLeverNeutral() </li>
     *      <li> GetShiftLeverNeutralString() </li>
     *      </ul>
     * 
     * @param currentShiftLeverPosition
     *               Current shift lever position read from the module.
     */
    virtual void ProcessCurrentShiftLeverPosition(const string &currentShiftLeverPosition);
    /**
     * Set the flag indicating if the drive shift lever position was observed.
     * <p><b>Description:</b><br>
     * Store the data flag indicating if the drive shift lever position was observed.
     * <p>
     * @param observedShiftLeverDrive
     *               Flag indicating if the drive shift lever position observed.
     */
    void SetObservedShiftLeverDrive(const bool &observedShiftLeverDrive);
    /**
     * Set the flag indicating if the Low shift lever position was observed.
     * <p><b>Description:</b><br>
     * Store the data flag indicating if the Low shift lever position was observed.
     * <p>
     * @param observedShiftLeverLow
     *               Flag indicating if the Low shift lever position observed.
     */
    void SetObservedShiftLeverLow(const bool &observedShiftLeverLow);
    /**
     * Set the flag indicating if the Low 3 shift lever position was observed.
     * <p><b>Description:</b><br>
     * Store the data flag indicating if the Low 3 shift lever position was observed.
     * <p>
     * @param observedShiftLeverLow3
     *               Flag indicating if the Low 3 shift lever position observed.
     */
    void SetObservedShiftLeverLow3(const bool &observedShiftLeverLow3);
    /**
     * Set the flag indicating if the Park shift lever position was observed.
     * <p><b>Description:</b><br>
     * Store the data flag indicating if the Park shift lever position was observed.
     * <p>
     * @param observedShiftLeverPark
     *               Flag indicating if the Park shift lever position observed.
     */
    void SetObservedShiftLeverPark(const bool &observedShiftLeverPark);
    /**
     * Set the flag indicating if the Reverse shift lever position was observed.
     * <p><b>Description:</b><br>
     * Store the data flag indicating if the Reverse shift lever position was observed.
     * <p>
     * @param observedShiftLeverReverse
     *               Flag indicating if the Reverse shift lever position observed.
     */
    void SetObservedShiftLeverReverse(const bool &observedShiftLeverReverse);
    /**
     * Set the flag indicating if the Neutral shift lever position was observed.
     * <p><b>Description:</b><br>
     * Store the data flag indicating if the Neutral shift lever position was observed.
     * <p>
     * @param observedShiftLeverNeutral
     *               Flag indicating if the Neutral shift lever position observed.
     */
    void SetObservedShiftLeverNeutral(const bool &observedShiftLeverNeutral);
    /**
     * List of faults for the background fault monitor to ignore.
     */
    XmlNodeMap *m_ignoredFaults;

private:
    /**
     * Set the string representation for Shift Lever Position in Park.
     * <p><b>Description:</b><br>
     * Store the string representation for the park shift lever position.
     * <p>
     * @param leverString
     *               String representation for Park Shift Lever Position.
     */
    void SetShiftLeverParkString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in Neutral.
     * <p><b>Description:</b><br>
     * Store the string representation for the Neutral shift lever position.
     * <p>
     * @param leverString
     *               String representation for Neutral Shift Lever Position.
     */
    void SetShiftLeverNeutralString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in Reverse.
     * <p><b>Description:</b><br>
     * Store the string representation for the Reverse shift lever position.
     * <p>
     * @param leverString
     *               String representation for Reverse Shift Lever Position.
     */
    void SetShiftLeverReverseString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in Drive.
     * <p><b>Description:</b><br>
     * Store the string representation for the Drive shift lever position.
     * <p>
     * @param leverString
     *               String representation for Drive Shift Lever Position.
     */
    void SetShiftLeverDriveString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in Up.
     * <p><b>Description:</b><br>
     * Store the string representation for the Up shift lever position.
     * <p>
     * @param leverString
     *               String representation for Up Shift Lever Position.
     */
    void SetShiftLeverUpString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in Down.
     * <p><b>Description:</b><br>
     * Store the string representation for the Down shift lever position.
     * <p>
     * @param leverString
     *               String representation for Down Shift Lever Position.
     */
    void SetShiftLeverDownString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in Open.
     * <p><b>Description:</b><br>
     * Store the string representation for the Open shift lever position.
     * <p>
     * @param leverString
     *               String representation for Open Shift Lever Position.
     */
    void SetShiftLeverOpenString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in Low.
     * <p><b>Description:</b><br>
     * Store the string representation for the Low shift lever position.
     * <p>
     * @param leverString
     *               String representation for Low Shift Lever Position.
     */
    void SetShiftLeverLowString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in First.
     * <p><b>Description:</b><br>
     * Store the string representation for the First shift lever position.
     * <p>
     * @param leverString
     *               String representation for First Shift Lever Position.
     */
    void SetShiftLeverFirstString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in Low 3.
     * <p><b>Description:</b><br>
     * Store the string representation for the Low 3 shift lever position.
     * <p>
     * @param leverString
     *               String representation for Low 3 Shift Lever Position.
     */
    void SetShiftLeverLow3String(const string &leverString);

	/**
     * Set the string representation for Shift Lever Position in Second.
     * <p><b>Description:</b><br>
     * Store the string representation for the Second shift lever position.
     * <p>
     * @param leverString
     *               String representation for Second Shift Lever Position.
     */
    void SetShiftLeverSecondString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in Third.
     * <p><b>Description:</b><br>
     * Store the string representation for the Third shift lever position.
     * <p>
     * @param leverString
     *               String representation for Third Shift Lever Position.
     */
    void SetShiftLeverThirdString(const string &leverString);

    /**
     * Set the string representation for Shift Lever Position in Autostick.
     * <p><b>Description:</b><br>
     * Store the string representation for the Autostick shift lever position.
     * <p>
     * @param leverString
     *               String representation for Autostick Shift Lever Position.
     */
    void SetShiftLeverAutostickString(const string &leverString);

    /**
     * Set the string representation for first gear.
     * <p><b>Description:</b><br>
     * Store the string representation for first gear.
     * <p>
     * @param gearString String representation for first gear.
     */
    void SetGear1String(const string &gearString);

    /**
     * Set the string representation for Second gear.
     * <p><b>Description:</b><br>
     * Store the string representation for Second gear.
     * <p>
     * @param gearString String representation for Second gear.
     */
    void SetGear2String(const string &gearString);

    /**
     * Set the string representation for Third gear.
     * <p><b>Description:</b><br>
     * Store the string representation for Third gear.
     * <p>
     * @param gearString String representation for Third gear.
     */
    void SetGear3String(const string &gearString);

    /**
     * Set the string representation for Fourth gear.
     * <p><b>Description:</b><br>
     * Store the string representation for Fourth gear.
     * <p>
     * @param gearString String representation for Fourth gear.
     */
    void SetGear4String(const string &gearString);

    /**
     * Set the string representation for Fifth gear.
     * <p><b>Description:</b><br>
     * Store the string representation for Fifth gear.
     * <p>
     * @param gearString String representation for Fifth gear.
     */
    void SetGear5String(const string &gearString);

    /**
     * Set the string representation for Sixth gear.
     * <p><b>Description:</b><br>
     * Store the string representation for Sixth gear.
     * <p>
     * @param gearString String representation for Sixth gear.
     */
    void SetGear6String(const string &gearString);

    /**
     * Set the string representation for Park gear.
     * <p><b>Description:</b><br>
     * Store the string representation for Park gear.
     * <p>
     * @param gearString String representation for Park gear.
     */
    void SetGearParkString(const string &gearString);

    /**
     * Set the string representation for Reverse gear.
     * <p><b>Description:</b><br>
     * Store the string representation for Reverse gear.
     * <p>
     * @param gearString String representation for Reverse gear.
     */
    void SetGearReverseString(const string &gearString);

    /**
     * Set the string representation for Neutral gear.
     * <p><b>Description:</b><br>
     * Store the string representation for Neutral gear.
     * <p>
     * @param gearString String representation for Neutral gear.
     */
    void SetGearNeutralString(const string &gearString);

    /**
     * Set the string representation for the Torque Converter Clutch no lock state.
     * <p><b>Description:</b><br>
     * Store the string representation for the TCC no lock state.
     * @param noLockString
     *               String representation for Torque Converter Clutch no lock state.
     */
    void SetTccNoLockString(const string &noLockString);

    /**
     * Set the string representation for the Torque Converter Clutch partial lock state.
     * <p><b>Description:</b><br>
     * Store the string representation for the TCC partial lock state.
     * @param partialLockString
     *               String representation for Torque Converter Clutch partial lock state.
     */
    void SetTccPartialLockString(const string &partialLockString);

    /**
     * Set the string representation for the Torque Converter Clutch full lock state.
     * <p><b>Description:</b><br>
     * Store the string representation for the TCC full lock state.
     * @param fullLockString
     *               String representation for Torque Converter Clutch full lock state.
     */
    void SetTccFullLockString(const string &fullLockString);
	/**
	 * Store the parameter regarding limp-in location.
     * <p><b>Description:</b><br>
     * The flag indicating if limp-in should be read from the TECHM will be stored.  This is typically set to "True"
     * for hybrid vehicles.
     * <p>
     * @param check  Flag indicating if limp-in should be read from the TECHM.
	 */
	void SetCheckTechmLimpIn(bool check);
	/**
	 * Retrieve the parameter regarding limp-in location.
     * <p><b>Description:</b><br>
     * The flag indicating if limp-in should be read from the TECHM will be returned to the calling function.
     * <p>
     * @return Flag indicating if limp-in should be read from the TECHM instead of the transmission module.
	 */
	bool GetCheckTechmLimpIn(void);
	/**
	 * Get/Set the flag indicating if the common module fault list should be used when reporting faults from the background monitors
	 * 
	 * @param uselist Flag to determine if the common module fault list should be used.
	 * 
	 * @return Flag to determine if the common module fault list should be used.
	 */
	bool UseCommonModuleFaultList(const bool *uselist = NULL);
    /**
     * Flag to indicate first gear was observed.
     */
    bool m_sawGear1;
    /**
     * Flag to indicate second gear was observed.
     */
    bool m_sawGear2;
    /**
     * Flag to indicate third gear was observed.
     */
    bool m_sawGear3;
    /**
     * Flag to indicate fourth gear was observed.
     */
    bool m_sawGear4;
    /**
     * Flag to indicate fifth gear was observed.
     */
    bool m_sawGear5;
    /**
     * Flag to indicate sixth gear was observed.
     */
    bool m_sawGear6;
    /**
     * Flag to indicate reverse gear was observed.
     */
    bool m_sawGearReverse;
    /**
     * Flag to indicate park gear was observed.
     */
    bool m_sawGearPark;
    /**
     * Flag to indicate neutral gear was observed.
     */
    bool m_sawGearNeutral;
    /**
     * Flag to indicate all required gears have been observed.
     */
    bool m_allGearsObserved;
    /**
     * Flag to indicate the torque converter clutch was observed in the No-Lockup state.
     */
    bool m_sawTCCNoLockState;
    /**
     * Flag to indicate the torque converter clutch was observed in the Partial-Lockup state.
     */
    bool m_sawTCCPartialLockState;
    /**
     * Flag to indicate the torque converter clutch was observed in the Full-Lockup state.
     */
    bool m_sawTCCFullLockState;
    /**
     * Flag to indicate all required torque converter clutch lock states have been observed.
     */
    bool m_allLockStatesObserved;
    /**
     * Flag to indicate drive shift lever position was observed.
     */
    bool m_sawShiftLeverDrive;
    /**
     * Flag to indicate Low shift lever position was observed.
     */
    bool m_sawShiftLeverLow;
    /**
     * Flag to indicate Low 3 shift lever position was observed.
     */
    bool m_sawShiftLeverLow3;
    /**
     * Flag to indicate reverse shift lever position was observed.
     */
    bool m_sawShiftLeverReverse;
    /**
     * Flag to indicate park shift lever position was observed.
     */
    bool m_sawShiftLeverPark;
    /**
     * Flag to indicate neutral shift lever position was observed.
     */
    bool m_sawShiftLeverNeutral;
    /**
     * Flag to indicate all required shift lever position have been observed.
     */
    bool m_allShiftLeverPositionsObserved;
    /**
	 * Indication that the Limp-In test should be done in the Trans Module
	 * (HEV HB's use the HCP for this, not the TECHM)
	 */
	bool m_checkTechmLimpIn;
	/** Flag to use to determine if a common fault list is in use */
	bool m_useCommonFaultList;
	/**
     * String representation for shift lever position park.
     */
    string m_shiftLeverPark;
    /**
     * String representation for shift lever position neutral.
     */
    string m_shiftLeverNeutral;
    /**
     * String representation for shift lever position reverse.
     */
    string m_shiftLeverReverse;
    /**
     * String representation for shift lever position drive.
     */
    string m_shiftLeverDrive;
    /**
     * String representation for shift lever position Up.
     */
    string m_shiftLeverUp;
    /**
     * String representation for shift lever position Down.
     */
    string m_shiftLeverDown;
    /**
     * String representation for shift lever position Open.
     */
    string m_shiftLeverOpen;
    /**
     * String representation for shift lever position Low.
     */
    string m_shiftLeverLow;
    /**
     * String representation for shift lever position drive.
     */
    string m_shiftLeverLow3;
	/**
     * String representation for shift lever position First.
     */
    string m_shiftLeverFirst;
    /**
     * String representation for shift lever position Second.
     */
    string m_shiftLeverSecond;
    /**
     * String representation for shift lever position Third.
     */
    string m_shiftLeverThird;
    /**
     * String representation for shift lever position Autostick.
     */
    string m_shiftLeverAutostick;
    /**
     * String representation for first gear.
     */
    string m_gear1;
    /**
     * String representation for second gear.
     */
    string m_gear2;
    /**
     * String representation for third gear.
     */
    string m_gear3;
    /**
     * String representation for fourth gear.
     */
    string m_gear4;
    /**
     * String representation for fifth gear.
     */
    string m_gear5;
    /**
     * String representation for sixth gear.
     */
    string m_gear6;
    /**
     * String representation for park gear.
     */
    string m_gearPark;
    /**
     * String representation for reverse gear.
     */
    string m_gearReverse;
    /**
     * String representation for neutral gear.
     */
    string m_gearNeutral;
    /**
     * String representation of TCC no lock state.
     */
    string m_tccNoLock;
    /**
     * String representation of TCC partial lock state.
     */
    string m_tccPartialLock;
    /**
     * String representation of TCC full lock state.
     */
    string m_tccFullLock;
};

//*************************************************************************
#endif  // GenericTransmissionModuleTemplate_h
