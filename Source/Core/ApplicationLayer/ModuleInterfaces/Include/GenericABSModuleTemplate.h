//*************************************************************************
// Description:
//  Module Interface for a generic ABS Controller module.
//
//===========================================================================
// Copyright (c) 2003-2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/GenericABSModuleTemplate.h $
// 
// 1     11/01/07 1:42p Derickso
// Updated header comments to doxygen format.
// Removed InitializeHook as all it did was call the base class.
// 
// 1     10/05/07 11:28a Derickso
// Updated header comments to doxygen format.
// 
// 16    1/17/07 2:55p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added OpenCom() and CloseCom().
// 
// 15    10/25/06 3:05p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Made the parsing of wheel speeds its own method
// 
// 14    3/30/06 1:34a Cward
// removed protected clause that was added in the previous iteration as it
// causes build errors for some customer specific builds.
//
// 13    11/11/05 4:54p Cward
// Changes for 0511101 iteration
//
// 12    4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 12    4/07/05 5:29p Gswope
// Added methods AllInletValvesOff() and AllInletValvesOn().
//
// 11    3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 10    3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 2     12/22/04 7:46p Bmeinke
// Added Isolate() method
// Added a ClearBrakeSwitchStatus() method to reset the brake switch
// on/off flags
// Moved ReadSensorSpeeds, ReadBrakeSwitch, CurrentBrakeSwitchState,
// SawBrakeSwitchOff, SawBrakeSwitchOn, ClearBrakeSwitchStatus to the
// public section
//
// 9     9/29/04 10:02p Cward
// Added Stop Pump Motor command
//
// 8     9/01/04 8:49p Cward
// Removed inline from Module name function. Readded the javadocs comments
// that had mistakenly removed
//
// 7     8/18/04 11:01p Cward
// changed include to cpp instead of h for template class
//
// 6     8/05/04 2:37a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 5     6/24/04 12:39a Cward
// Added Update Rolls Status Byte method. Updated to new header block
// comments
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
// 2     10/29/03 6:12a Derickso
// Added methods to command the module to recover all
// wheels, dump pressure at all wheels and exit roll
// test mode.
//
// 1     10/27/03 4:33p Derickso
// Added support to read the module configuration.
//
// 4     10/20/03 9:34a Derickso
// Corrected compile errors.
//
// 3     9/15/03 9:50a Derickso
// Added neccessary GetInfo methods to correct compile errors.
//
// 2     8/29/03 3:02p Derickso
// Corrected comile errors.
// Compiles, not tested.
//
// 1     8/27/03 11:40a Derickso
// Greg's DCX CCRT working stuff.
//
// 1     8/21/03 8:57a Derickso
// 05WK CCRT Project
//
// 5     8/20/03 6:41a Derickso
// Recombined cpp and h files.  Compiler does not support the
// implementation being seperate for template classes.
//
// 4     8/19/03 9:37a Derickso
// Updated class names.  Added Template to the end of the Generic class
// names to avoid conflicts with code written earlier.
//
// 2     8/15/03 3:18p Derickso
// Split implementation out to cpp file.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GENERICABSMODULETemplate_H
#define GENERICABSMODULETemplate_H
//==============================================================================

#include "GenericModuleTemplate.cpp"
#include "ModuleException.h"
#include "ABSValveControl.h"

class XmlNode;

/**
 * Generic ABS module interface.<br>
 * The functions contained in this class are common to most ABS modules.  This class acts
 * as a base class for specific ABS module interfaces.
 */
template <class ProtocolFilterType>
class GenericABSModuleTemplate : public GenericModuleTemplate<ProtocolFilterType>
{
    friend class GenericABSModuleTemplateTest;

public:
    /**
     * Class constructor.<br>
     * Brake switch member variables will be intialized.
     */
    GenericABSModuleTemplate();

    /**
     * Class destructor.
     */
    virtual ~GenericABSModuleTemplate();

    /**
     * Return the name of this module.
     * <p><b>Description:</b><br>
     * The name of the module will be returned to the calling function.
     * <p>
     * 
     * @return GenericABSModuleTemplate
     */
    virtual string ModuleName();

    /**
     * Open the serial connection.
     * <p><b>Description:</b><br>
     * The member variables used to track the brake switch states will be reset to an initial condition.  The base 
     * class will then be called to complete the opening of the communication channel.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericModuleTemplate::OpenCom() </li>
     *      </ul>
     *
     * @return Flag indicating if the communication channel was opened successfully.
     */
    virtual bool OpenCom();
    
    /**
     * Close all communication channels to the module.
     * <p><b>Description:</b><br>
     * The member variables used to track the brake switch states will be reset to an initial condition.  The base 
     * class will then be called to complete the closing of the communication channel.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericModuleTemplate::CloseCom() </li>
     *      </ul>
     */
    virtual void CloseCom();

    /**
     * Enum for the four wheels.
     */
    enum Corner
    {
        LF,RF,LR,RR
    };

    /**
     * Message tag to use for reading the brake switch.
     */
    const string m_readBrakeSwitch;
    /**
     * Message tag to use for reading wheel speed sensor data from the module.
     */
    const string m_readSensorSpeeds;
    /**
     * Message tag to use to reduce brake force at each wheel.
     */
    const string m_actuateReduction;
    /**
     * Message tag to use for recovering brake force at each wheel.
     */
    const string m_actuateRecovery;
    /**
     * Function to use for reading the brake switch state.
     */
    const string m_currentBrakeSwitchStateMethod;
    /**
     * Function to use for reading brake switch on.
     */
    const string m_sawBrakeSwitchOffMethod;
    /**
     * Function to use for reading brake switch on.
     */
    const string m_sawBrakeSwitchOnMethod;
    /**
     * Update the rolls status byte in the module with the overall ABS test result.
     * <p><b>Description:</b><br>
     * The overall ABS test result will be written to the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> UpdateModule<i>[testResult]</i> </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @param testResult Overall ABS test result to write to the module.
     * @return Status of writing the overall ABS test result to the module.
     */
    virtual BEP_STATUS_TYPE UpdateRollsStatusByte(const string &testResult);
    /**
     * GetInfo for string types
     * <p><b>Description:</b><br>
     * String type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> ActuateReduction() </li>
     *      <li> ActuateRecovery() </li>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ActuateReduction </li>
     *      <li> ActuateRecovery </li>
     *      <li> ReadSerialNumber </li>
     *      <li> ReadStopLampSwitch </li>
     *      <li> ReadBrakeSwitch </li>
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
     * GetInfo for boolean types
     * <p><b>Description:</b><br>
     * Boolean type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> ReadBrakeSwitch() </li>
     *      <li> CurrentBrakeSwitchState() </li>
     *      <li> SawBrakeSwitchOff() </li>
     *      <li> SawBrakeSwitchOn() </li>
     *      <li> ReadModuleData() </li>
     *      <li> ModuleName() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> CheckInPlantStatus </li>
     *      <li> IsEvacAndFillComplete </li>
     *      <li> IsFluidFillSwitchOn </li>
     *      <li> IsGSwitchPresent </li>
     *      <li> ReadBrakeSwitch </li>
     *      <li> CurrentBrakeSwitchState </li>
     *      <li> SawBrakeSwitchOff </li>
     *      <li> SawBrakeSwitchOn </li>
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
    virtual BEP_STATUS_TYPE GetInfo(string method, bool &value)  throw(ModuleException);
    /**
     * GetInfo for fault data
     * <p><b>Description:</b><br>
     * Fault data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None<p>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, FaultVector_t & value) throw(ModuleException);

    /**
     * GetInfo for WheelSpeeds_t type data.
     * <p><b>Description:</b><br>
     * Wheelspeed_t data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> ReadSensorSpeeds() </li>
     *      <li> GenericABSModuleTemplate::ModuleName() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadSensorSpeeds </li>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, WheelSpeeds_t &value)  throw(ModuleException);

    /**
     * GetInfo for integer types.
     * <p><b>Description:</b><br>
     * Integer data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None<p>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, int &value) throw(ModuleException);

    /**
     * GetInfo for unsigned char types
     * <p><b>Description:</b><br>
     * Unsigned char type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <p>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 &value) throw(ModuleException);

    /**
     * GetInfo for unsigned short types
     * <p><b>Description:</b><br>
     * Unsigned short type data will be read from the module and returned to the calling function.
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
     *      <li> ReadModuleConfiguration </li>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 &value) throw(ModuleException);

    /**
     * GetInfo for float types
     * <p><b>Description:</b><br>
     * Float type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <p>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);

    /**
     * Command the module to perform the specified action.
     * <p><b>Description:</b><br>
     * The module will be commanded to perform an action.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <p>
     *
     * @param methodName The action to command to the module.
     * @return Status of commanding the module.
     * @throw ModuleException if the method cannot be found
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName) throw(ModuleException);

    /**
     * Command the module to perform a power on reset.
     * <p><b>Description:</b><br>
     * The module will be commanded to perform a power-on reset.  The module will re-initialize its software for a 
     * static electrical check.  This is essentially the same thing as performing a key cycle.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> PerformPowerOnReset </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of performing the power-on reset.
     */
    virtual BEP_STATUS_TYPE PerformPowerOnReset(void);

    /**
     * Read switches from module. 
     * <p><b>Description:</b><br>
     * Switch data will be read from the module and analyzed to determine which brake switch states have been detected.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadBrakeSwitch </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      <li> ReadModuleData() </li>
     *      </ul>
     * 
     * @param filter Communication object to use for reading the switch data from the module.
     * @return Status of reading the switch data from the module.
     */
    virtual BEP_STATUS_TYPE ReadSwitches(IProtocolFilter *filter);

    /**
     * Run the pump motor.
     * <p><b>Description:</b><br>
     * The module will be commanded to run the pump motor.  This helps ensure fluid is distributed through the system.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> RunPumpMotor </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of running the pump motor.
     */
    virtual BEP_STATUS_TYPE RunPumpMotor(void);

    /**
     * Stop the pump motor.
     * <p><b>Description:</b><br>
     * Command the module to stop running the pump motor.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> StopPumpMotor </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     * 
     * @return Status of commanding the module to stop the pump motor.
     */
    virtual BEP_STATUS_TYPE StopPumpMotor(void);

    /**
     * Isolates the specified corner.
     * <p><b>Description:</b><br>
     * The specified corner (LF, RF, LR, RR) will be isolated.  This typically results in brake pressure being removed
     * from the specified wheel.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> Isolate<i>[corner]</i> </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @param corner Wheel to be isolated.
     * @return Status of isolating the specified wheel.
     * @throws ModuleException if CheckObjectStatus() fails
     */
    virtual BEP_STATUS_TYPE Isolate(const string &corner)  throw(ModuleException);

    /**
     * Actuate reduction on the specified corner.
     * <p><b>Description:</b><br>
     * The module will be commanded to reduce brake pressure at the specified wheel.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> Actuate<i>[corner]</i>Reduction </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @param corner Wheel to reduce brake pressure.
     * @return Status of reducing brake pressure at the specified wheel.
     * @throws ModuleException if CheckObjectStatus() fails
     */
    virtual BEP_STATUS_TYPE ActuateReduction(const string &corner)  throw(ModuleException);

    /**
     * Actuate recovery on the specified corner.
     * <p><b>Description:</b><br>
     * Command the module to recover brake force at the specified wheel.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> Actuate<i>[corner]</i>Recovery </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @param corner Wheel to recover brake pressure.
     * @return Status of recovering brake pressure at the specified wheel.
     * @throws ModuleException if CheckObjectStatus() fails
     */
    virtual BEP_STATUS_TYPE ActuateRecovery(const string &corner)  throw(ModuleException);

    /**
     * Command the module to hold all valves and pressures.
     * <p><b>Description:</b><br>
     * The module will be commanded to hold all valves in the current state.  This results in the current brake pressure 
     * being held constant at each wheel.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> HoldAllValves </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     * 
     * @return Status of commanding the module to hold all valves.
     */
    virtual BEP_STATUS_TYPE HoldAllValves(void) throw(ModuleException);

	/**
	 * Command the module to hold a certain valves pressure.
	 *
	 * @return Status of the operation.
	 * @exception ModuleException
	 */
	virtual BEP_STATUS_TYPE HoldValves(const string &valves) throw(ModuleException);

    /**
     * Command module to turn off all inlet valves.
     * <p><b>Description:</b><br>
     * The module will be commanded to turn off all inlet valves.  This prevents brake fluid from entering the valve
     * assembly at each wheel.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> AllInletValvesOff </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of commanding all inlet valves to the off state.
     */
    virtual BEP_STATUS_TYPE AllInletValvesOff(void) throw(ModuleException);
    /**
     * Command module to dump pressure at selected wheels.
     * <p><b>Description:</b><br>
     * The module will be commanded to dump all brake pressure at the specified valves.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> Dump<i>[valves]</i>Valves </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @param valves Valves to dump brake pressure at.
     *
     * @return Status of dumping brake pressure at the specified valves.
     */
    virtual BEP_STATUS_TYPE DumpValves(const string &valves) throw(ModuleException);

    /**
     * Command module to turn on all inlet valves.
     * <p><b>Description:</b><br>
     * The module will be commanded to turn on all inlet valves.  This allows brake fluid to enter the valve assembly 
     * at each wheel. 
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> AllInletValvesOn </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of commanding the module to set all inlet valves to the On state.
     */
    virtual BEP_STATUS_TYPE AllInletValvesOn(void) throw(ModuleException);
    /**
     * Command the module to recover brake pressure at all wheels.
     * <p><b>Description:</b><br>
     * The module will be commanded to recover brake pressure at all wheels.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> RecoverAllValves </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of commanding the module to recover brake force at all valves.
     */
    virtual BEP_STATUS_TYPE RecoverAllValves(void) throw(ModuleException);

    /**
     * Command the module to exit roll test mode.
     * <p><b>Description:</b><br>
     * The module will be commanded to stop roll test mode.  Control will be returned to the ECU.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> StopRollTestMode </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of commanding the module to stop roll test mode.
     */
    virtual BEP_STATUS_TYPE StopRollTestMode(void) throw(ModuleException);

    /**
     * Command the module to calibrate the G-Sensor.
     * <p><b>Description:</b><br>
     * The module will be commanded to calibrate the G-sensor.  The calibration should be performed with the vehicle at
     * zero speed.  Calibration should only be performed on vehicles equipped with a G-sensor.  If the calibration was
     * successful, the results will be recorded in the module.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> IgnoreCalibarateGSensorResult - Flag to determine if the calibration result should be ignored. </li>
     *      </ul>
     * 
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> CalibrateGSensor </li>
     *      <li> RecordCalibrateResults </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     * 
     * @return Status of calibrating the G-sensor.
     */
    virtual BEP_STATUS_TYPE CalibrateGSensor(void)  throw(ModuleException);

    /**
     * The provided tire size will be programmed into the module.
     * <p><b>Description:</b><br>
     * The tire size will be written to the module.  If the corret tire size is not programmed, the ABS will not be able
     * to determine the correct vehicle speed.  This may result on the speedometer indicating an incorrect speed.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ProgramTireSize </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @param tireSize  Tire size to program into the module.
     * @return  Status of programming the tire size into the module.
     */
    virtual BEP_STATUS_TYPE ProgramTireSize(const UINT16 &tireSize);

    /**
     * Read the tire size.
     * <p><b>Description:</b><br>
     * Read the tire size currently programmed into the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadCurrentTireSize </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * @param tireSize  The tire size read from the module.
     * @return  Status of reading the tire size from the module.
     */
    virtual BEP_STATUS_TYPE ReadCurrentTireSize(UINT16 &tireSize);

    /**
     * Disable the diagnostic speed limit.
     * <p><b>Description:</b><br>
     * Command the module to disable the diagnostic speed limit to prevent faults from being generated in the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> DisableDiagnosticSpeedLimit </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     * 
     * @return Status of disabling the diagnostic speed limit.
     */
    virtual BEP_STATUS_TYPE DisableDiagnosticSpeedLimit(void) throw(ModuleException);
    /**
     * Update the Evac and Fill result stored in the module.
     * <p><b>Description:</b><br>
     * The Evac and Fill result will be stored in the module.  This is typically used in a development condition where
     * the upline tool has not bought off the evac and fill process.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> UpdateEvacAndFill </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of updating the evac and fill result in the module.
     */
    virtual BEP_STATUS_TYPE UpdateEvacAndFillResult(void) throw(ModuleException);

    /**
     * Read the wheel speeds from the module.
     * <p><b>Description:</b><br>
     * The raw wheel speed sensor data will be read from the module for all wheels.  The data will then be parsed to
     * translate the wheel speeds into MPH.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadSensorSpeeds </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseSensorSpeeds() </li>
     *      </ul>
     *
     * @param speeds Wheel speed sensor values read from the module.
     * @return The status of reading the wheel speed sensors from the module.
     */
    virtual BEP_STATUS_TYPE ReadSensorSpeeds(WheelSpeeds_t &speeds)  throw(ModuleException);

    /**
     * Read the brake switch on status.
     * <p><b>Description:</b><br>
     * The brake switch on status will be read from the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadBrakeSwitch </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      <li> ReadModuleData() </li>
     *      </ul>
     * 
     * @param status The brake switch on status. True is on.
     * @return The status of reading the brake switch on status.
     */
    virtual BEP_STATUS_TYPE ReadBrakeSwitch(bool &switchStatus)  throw(ModuleException);

    /**
     * Get the current brake switch state.
     * <p><b>Description:</b><br>
     * The current brake switch state previously read from the module will be returned to the calling function.
     * <br><b><i>Note:</i></b> True denotes the brake switch is in the "On" position.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param state  Current state of the brake switch.
     *
     * @return BEP_STATUS_SUCCESS
     */
    virtual BEP_STATUS_TYPE CurrentBrakeSwitchState(bool &state);
    /**
     * Check to see if the brake switch was detected in the Off position.
     * <p><b>Description:</b><br>
     * The flag indicating the brake switch was detected in the "Off" position will be returned to the calling function.
     * A value of true denotes the brake switch was detected in the "Off" position.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * @param flag   Flag indicating if the brake switch was detected in the "Off" position.
     *
     * @return BEP_STATUS_SUCCESS
     */
    virtual BEP_STATUS_TYPE SawBrakeSwitchOff(bool &flag);
    /**
     * Check to see if the brake switch was detected in the On position.
     * <p><b>Description:</b><br>
     * The flag indicating the brake switch was detected in the "On" position will be returned to the calling function.
     * A value of true denotes the brake switch was detected in the "On" position.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * @param flag   Flag indicating if the brake switch was detected in the "On" position.
     *
     * @return BEP_STATUS_SUCCESS
     */
    virtual BEP_STATUS_TYPE SawBrakeSwitchOn(bool &flag);

    /**
     * Method used to reset the brake switch status.
     * <p><b>Description:</b><br>
     * The brake switch on and brake switch off flags will both be set to false indicating the states have not been 
     * observed in the module.
     * <p>
     *
     * @return BEP_STATUS_SUCCESS
     */
    virtual BEP_STATUS_TYPE ClearBrakeSwitchStatus(void);
    /**
     * Traction control system fires ABS valves. 
     * <p><b>Description:</b><br>
     * A check is performed to see if a valve controller thread is already active. 
     * If no thread is running then a new thread is created that specifies the firing 
     * of the traction control valves and the valves are fired continuously. 
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ABSValveControl() </li>
     *      <li> Resume() </li>
     *      </ul>
     * 
     * @param valves
     *          Valves to fire.
     *          
     * @param messageGap 
     *          Time (in ms) delay between subsequent sends of the fire valve message.          
     * 
     * @return Status of firing the traction control valves.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Successfully fired the valves. </li>
     *      <li> BEP_STATUS_FAILURE - Could not fire the valves. </li>
     *      </ul>
     */
	virtual BEP_STATUS_TYPE StartFiringTractionControlValves(const string &valves, const INT32& messageGap);
	/**
     * Stop the continuous fire of the specified valves.
     * <p><b>Description:</b><br>
     * Stop the traction control system from firing the ABS valves. If the valve
     * controller thread is running it sends a message to stop the valves 
     * from firing and waits until it is done. Then the pointer is reset 
     * again for later use in the case that the traction control system
     * is called upon again to fire the valves.    
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StopMonitor() </li>
     *      <li> WaitUntilDone() </li>
     *      </ul>        
     * 
     * @return Status result of stopping the firing of the ABS valves.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Successfully ended the traction control system firing the ABS valves. </li>
     *      <li> BEP_STATUS_FAILURE - Unable to stop the valve firing.  </li>
     *      </ul>
     */
	virtual BEP_STATUS_TYPE StopFiringTractionControlValves(void);
    /**
     * Traction control system pulses ABS valves. 
     * <p><b>Description:</b><br>
     * A check is performed to see if a valve controller thread is already active. 
     * If no thread is running then a new thread is created that specifies pulses 
     * of the traction control valves at a specified frequency. This is similar in 
     * purpose to firing the traction control valves except here the pulses allow
     * firing of the valves at specified time intervals and the amount
     * of time to keep each pulse active. 
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ABSValveControl() </li>
     *      <li> Resume() </li>
     *      </ul>
     * 
     * @param valves      Valves to pulse.
	 * @param pulseLength Amount of time to keep brake active.
	 * @param pulseDelay  Delay between pulses.
     * 
     * @return Status of pulsing the traction control valves.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Successfully enabled pulsing of the valves. </li>
     *      <li> BEP_STATUS_FAILURE - Could not enable pulsing of the valves.  </li>
     *      </ul>
     */
	virtual BEP_STATUS_TYPE StartPulsingTractionControlValves(const string &valves, 
															  const INT32 &pulseLength, 
															  const INT32 &pulseDelay);
    /**
     * Stop pulsing the specified valves. 
     * <p><b>Description:</b><br>
     * Returns the status of the method that stops firing the traction control valves (the status
     * is returned through a call to the method that stops firing the traction control valves).      
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StopFiringTractionControlValves() </li>
     *      </ul>
     * 
     * @return Status of stopping the traction control valves from pulsing.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Successfully stopped pulsing of the valves. </li>
     *      <li> BEP_STATUS_FAILURE - Could not stop pulsing of the valves.  </li>
     *      </ul>
     */                              
	virtual BEP_STATUS_TYPE StopPulsingTractionControlValves(void);
    BEP_STATUS_TYPE CheckSpeedDeltas(WheelSpeeds_t &speedDeltas, UINT8 rollerIndex); 

        /**
     * Command the module to turn power off to all valves.
     * <p><b>Description:</b><br>
     * The module will be commanded to turn off all valves.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> TurnAllValvesOff </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of commanding all valves off.
     */
    virtual BEP_STATUS_TYPE CommandAllValvesOff(void) throw(ModuleException); 

protected:
    /**
     * Parse the wheel speeds from a module response string.
     * <p><b>Description:</b><br>
     * The wheel speeds read from the module will be translated to MPH and returned in <i>speeds</i>.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadSensorSpeeds/ReplyInterpretationLF </li>
     *      <li> ReadSensorSpeeds/ReplyInterpretationRF </li>
     *      <li> ReadSensorSpeeds/ReplyInterpretationLR </li>
     *      <li> ReadSensorSpeeds/ReplyInterpretationRR </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      <li> ParseFloatResponse() </li>
     *      </ul>
     * 
     * @param response The response string from the module
     * @param speeds   Wheel speeds translated from the module response data.
     * 
     * @return The status of parsing the wheel speed data.
     */
    virtual BEP_STATUS_TYPE ParseSensorSpeeds(const SerialString_t &response, WheelSpeeds_t &speeds)  throw(ModuleException);
    /**
     * Initialize hook for the GenericABSModuleTemplate, simply calls
     * the GenericModuleTemplate initialize hook.
     *
     * @param configNode The XML Config node for the module
     * @return Status of the operation, in this instance the
     * value returned by GenericModuleTemplate's initialize hook.
     */
    virtual bool InitializeHook(const XmlNode *configNode);
    
    /**
     * Current brake switch state.
     */
    bool m_currentBrakeSwitchState;

    /**
     * Brake switch on observed flag.
     */
    bool m_sawBrakeSwitchOn;

    /**
     * Brake switch off observed flag.
     */
    bool m_sawBrakeSwitchOff;

    /**
	 * Valve control thread for continuous fire or pulsing of the brakes.
	 */
	ABSValveControl  *m_valveController;
};

//-----------------------------------------------------------------------------
#endif  //GENERICABSMODULETemplate_H

