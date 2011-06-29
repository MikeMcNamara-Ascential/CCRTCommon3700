//*************************************************************************
// Description:
//  Module Interface for a generic ABS Controller module.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Brake/GenericABSModuleTemplate.cpp $
// 
// 1     11/01/07 1:04p Derickso
// Added function separators.
// Updated to use ReadModuleData and CommandModule instead of
// ProtocolFilter::GetModuleData.
// Removed InitializeHook as it did nothing but call the base class.
// 
// 2     10/05/07 11:29a Derickso
// Updated to use CommandModule instead of GetModuleData.
// 
// 1     10/01/07 4:02p Derickso
// Added Check In Plant Status.
// 
// 8     1/17/07 2:43p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added OpenCom() and CloseCom().
// 
// 7     10/25/06 2:52p Rwiersem
// Changes for the 06102701 core release:
// 
//    - Made the parsing of wheel speeds its own method
// 
// 6     11/11/05 4:54p Cward
// Changes for 0511101 iteration
//
// 5     4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 4     4/07/05 5:29p Gswope
// Added methods AllInletValvesOff() and AllInletValvesOn().
//
// 4     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 2     9/29/04 9:59p Cward
// Added Stop Pump Motor
//
// 1     8/18/04 10:53p Cward
// Moved some Template class implementation files.
//
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "GenericABSModuleTemplate.h"


//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
GenericABSModuleTemplate<ProtocolFilterType>::GenericABSModuleTemplate() : GenericModuleTemplate<ProtocolFilterType>(),
    m_readBrakeSwitch("ReadBrakeSwitch"), m_readSensorSpeeds("ReadSensorSpeeds"),
    m_actuateReduction("ActuateReduction"),     m_actuateRecovery("ActuateRecovery"),
    m_currentBrakeSwitchStateMethod("CurrentBrakeSwitchState"),
    m_sawBrakeSwitchOffMethod("SawBrakeSwitchOff"), m_sawBrakeSwitchOnMethod("SawBrakeSwitchOn"),
    m_valveController(NULL)
{   // Log the function entry
    Log(LOG_FN_ENTRY, ":%s Entering constructor", GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str());
    // Initialize the member variables
    m_currentBrakeSwitchState = false;
    m_sawBrakeSwitchOff = false;
    m_sawBrakeSwitchOn = false;
    // Log the function exit
    Log(LOG_FN_ENTRY, ":%s Exiting constructor", GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str());
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
GenericABSModuleTemplate<ProtocolFilterType>::~GenericABSModuleTemplate()
{   // Nothing special to do here
    // Get rid of the valve controller if it was created
	if (m_valveController != NULL)
	{	// Stop firing the valves
		Log(LOG_DEV_DATA, "Signaling the ABS valve controller thread to stop...\n");
		m_valveController->StopMonitor();
		m_valveController->WaitUntilDone();
		delete m_valveController;
	}
	m_valveController = NULL;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
string GenericABSModuleTemplate<ProtocolFilterType>::ModuleName()
{
    return "GenericABSModuleTemplate";
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
bool GenericABSModuleTemplate<ProtocolFilterType>::OpenCom()
{
    bool status = false;
    
    // Initialize the member variables
    m_currentBrakeSwitchState = false;
    m_sawBrakeSwitchOff = false;
    m_sawBrakeSwitchOn = false;

    status = GenericModuleTemplate<ProtocolFilterType>::OpenCom();

    return( status);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
void GenericABSModuleTemplate<ProtocolFilterType>::CloseCom()
{
    // Initialize the member variables
    m_currentBrakeSwitchState = false;
    m_sawBrakeSwitchOff = false;
    m_sawBrakeSwitchOn = false;
    // Get rid of the valve controller if it was created
	if (m_valveController != NULL)
	{	// Stop firing the valves
		Log(LOG_DEV_DATA, "Signaling the ABS valve controller thread to stop...\n");
		m_valveController->StopMonitor();
		m_valveController->WaitUntilDone();
		delete m_valveController;
	}
	m_valveController = NULL;
    GenericModuleTemplate<ProtocolFilterType>::CloseCom();
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, string &value) throw(ModuleException)
{   // Perform the reduction portion of the ABS test
    BEP_STATUS_TYPE retVal = BEP_STATUS_ERROR;
    if (methodName == m_actuateReduction)      retVal = ActuateReduction(value);
    // Perform the recovery portion of the ABS test
    else if (methodName == m_actuateRecovery)  retVal = ActuateRecovery(value);
    // Read the serial number from the module
    if (methodName == "ReadSerialNumber")         retVal = ReadModuleData(methodName, value);
    // Read the stop lamp switch status
    else if (methodName == "ReadStopLampSwitch")  retVal = ReadModuleData(methodName, value);
    // Read the brake switch position
    else if (methodName == m_readBrakeSwitch)     retVal = ReadModuleData("ReadBrakeSwitch", value);
     // No special method, try the base class
     else  retVal = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
     // Return the result
    return retVal;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(string method, bool &value)  throw(ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering bool GetInfo(%s)",
        GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str(), method.c_str());
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // Any of these could throw module exception
    // Read the brake switch status from the module
    if (method == m_readBrakeSwitch)                    retVal = ReadBrakeSwitch(value);
    // Get the current brake switch state
    else if (method == m_currentBrakeSwitchStateMethod) retVal = CurrentBrakeSwitchState(value);
    // Determine if the brake switch was detected in the off position
    else if (method == m_sawBrakeSwitchOffMethod)       retVal = SawBrakeSwitchOff(value);
    // Determine if the brake switch was detected in the on position
    else if (method == m_sawBrakeSwitchOnMethod)        retVal = SawBrakeSwitchOn(value);
    // Check for the presence of a G-switch
    else if (method == "IsGSwitchPresent")          retVal = ReadModuleData(method, value);
    // Check if the fluid fill switch is on
    else if (method == "IsFluidFillSwitchOn")       retVal = ReadModuleData(method, value);
    // Read the Evac and Fill complete status
    else if (method == "IsEvacAndFillComplete") retVal = ReadModuleData(method, value);
    // Check if the vehicle is in In-Plant status yet
	else if (method == "CheckInPlantStatus")	retVal = ReadModuleData(method, value);
	else
    {   // No special method, try using the base class
        Log(LOG_DEV_DATA, "%s::GetInfo(bool) calling GenericModuleTemplate::GetInfo(bool)",
            GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str());
        retVal = GenericModuleTemplate<ProtocolFilterType>::GetInfo(method, value);
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting GetInfo bool", GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str());
    // Return the results of the operation
    return retVal;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::UpdateRollsStatusByte(const string &testResult)
{   // Write the test result to the module
    return CommandModule("UpdateModule"+testResult);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::UpdateEvacAndFillResult(void)
throw(ModuleException)
{   // Command the module to update the evacand fill data in the module
    return CommandModule("UpdateEvacAndFill");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::DisableDiagnosticSpeedLimit(void)
throw(ModuleException)
{   // Command the module to disable diagnostic speed limit
    return CommandModule("DisableDiagnosticSpeedLimit");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::CommandAllValvesOff(void) 
throw(ModuleException)
{	// Command the module to turn power off to all valves
	return CommandModule("TurnAllValvesOff");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t & value)
throw(ModuleException)
{   // No special method, try the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(string method, WheelSpeeds_t &value)  throw(ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering WheelSpeeds_t GetInfo(%s)\n",
        GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str(), method.c_str());
    // set the initial value
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // Any of these could throw module exception
    // Read the wheel speed sensors
    if (method == m_readSensorSpeeds) retVal = ReadSensorSpeeds(value);
    else
    {   // No special method, try using the base class
        Log(LOG_DEV_DATA, "%s::GetInfo(WheelSpeeds_t) calling GenericModuleTemplate::GetInfo(WheelSpeeds_t)\n",
            GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str());
        retVal = GenericModuleTemplate<ProtocolFilterType>::GetInfo(method, value);
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting GetInfo WheelSpeeds_t\n", GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str());
    // Return the return value
    return retVal;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, int &value) throw(ModuleException)
{   // No special method available, try using the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value) throw(ModuleException)
{   // No special method, try the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT16 &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the module configuration
    if (methodName == "ReadModuleConfiguration")  status = ReadModuleData(methodName, value);
    // No special method, try the base class
    else    status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, float &value) throw(ModuleException)
{   // No special method, try the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName) throw(ModuleException)
{   // No special method, just try the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::PerformPowerOnReset(void)
{   // Command the module to perform the power on reset
    return CommandModule("PerformPowerOnReset");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::ReadSwitches(IProtocolFilter *filter)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering ReadSwitches", ModuleName().c_str());
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string messageTag("ReadBrakeSwitch");
    // Read the brake switch using the provided protocol filter
    status = ReadModuleData(messageTag, m_currentBrakeSwitchState, NULL, NULL, filter);
    // Check the status of the read brake switch
    if (status == BEP_STATUS_SUCCESS)
    {   // Good read, Extract the data from the response
        try
        {   
            Log(LOG_DEV_DATA, "%s: %s\n", messageTag.c_str(), m_currentBrakeSwitchState ? "On" : "Off");
            // Determine the status of the brake switch
            if (m_currentBrakeSwitchState && !m_sawBrakeSwitchOn)
            {   // Bake switch is on
                Log(LOG_DEV_DATA, "%s: ReadSwitches: Saw Brake Switch on", ModuleName().c_str());
                m_sawBrakeSwitchOn = true;
            }
            else if (!m_currentBrakeSwitchState && !m_sawBrakeSwitchOff)
            {   // Brake switch is off
                Log(LOG_DEV_DATA, "%s: ReadSwitches: Saw Brake Switch off", ModuleName().c_str());
                m_sawBrakeSwitchOff = true;
            }
            else
            {   // Already observed state, nothing to do.
            }
        }
        catch (XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading data from the module
        Log(LOG_ERRORS, "Error reading brake switch fromthe module - status: %s\n",
            ConvertStatusToResponse(status).c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting ReadSwitches", ModuleName().c_str());
    // Return the status of the operation
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::RunPumpMotor(void)
{   // Command the module to run the pump motor
    return CommandModule("RunPumpMotor");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::StopPumpMotor(void)
{   // Command the module to stop running the pump motor
    return CommandModule("StopPumpMotor");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::Isolate(const string &corner)  throw(ModuleException)
{   // Command the module to perform reduction on specified wheel
    return CommandModule("Isolate"+corner);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::ActuateReduction(const string &corner)  throw(ModuleException)
{   // Command the module to perform reduction on specified wheel
    return CommandModule("Actuate"+corner+"Reduction");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::ActuateRecovery(const string &corner)  throw(ModuleException)
{   // Command the module to perform recovery on specified wheel
    return CommandModule("Actuate"+corner+"Recovery");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::HoldAllValves(void) throw(ModuleException)
{   // Command the module to hold all valves
    return CommandModule("HoldAllValves");
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::HoldValves(const string &valves) throw(ModuleException)
{   // Command the module to hold all valves
	return CommandModule("Hold"+valves+"Valves");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::AllInletValvesOff(void) throw(ModuleException)
{   // Command the module to dump all pressure at all wheels
    return CommandModule("AllInletValvesOff");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::DumpValves(const string &valves) throw(ModuleException)
{   // Command the module to dump all pressure at all wheels
    return CommandModule("Dump"+valves+"Valves");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::AllInletValvesOn(void) throw(ModuleException)
{   // Command the module to dump all pressure at all wheels
    return CommandModule("AllInletValvesOn");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::RecoverAllValves(void) throw(ModuleException)
{   // Command the module to recover pressure at all wheels
    return CommandModule("RecoverAllValves");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::StopRollTestMode(void) throw(ModuleException)
{   // Command module to exit roll test mode
    return CommandModule("StopRollTestMode");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::CalibrateGSensor(void)  throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Command the module to calibrate the G-Sensor
    status = CommandModule("CalibrateGSensor");
    // Check the status of the operation
    bool ignoreResults = true;
    try
    {
        ignoreResults = !m_configNode->getChild("Setup/IgnoreCalibarateGSensorResult")->getValue().compare("Yes");
    }
    catch (XmlException &excpt)
    {
        ignoreResults = true;
    }
    if ((BEP_STATUS_SUCCESS == status) || ignoreResults)
    {   // Record the results in the module
        status = CommandModule("RecordCalibrateResults");
        status = ignoreResults ? BEP_STATUS_SUCCESS : status;
        Log(LOG_ERRORS, "Record G-Sensor calibration results in the module status: %s\n",
            ConvertStatusToResponse(status).c_str());
    }
    else
    {   // Failed to calibrate the sensor
        Log(LOG_ERRORS, "Failed to calibrate G-Sensor - status: %s\n", ConvertStatusToResponse(status).c_str());
    }
    // Return the status of the operation
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::ProgramTireSize(const UINT16 &tireSize)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialArgs_t tireSizeArgs;
    // Prepare the tire size for programming
    tireSizeArgs.push_back((tireSize & 0xFF00) >> 8);
    tireSizeArgs.push_back(tireSize & 0x00FF);
    // Program the tire size into the module
    status = CommandModule("ProgramTireSize", &tireSizeArgs);
    // Log the result of the programming
    Log(LOG_ERRORS, "Programmed tire size into module: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::ReadCurrentTireSize(UINT16 &tireSize)
{
    return ReadModuleData("ReadCurrentTireSize", tireSize);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::ReadSensorSpeeds(WheelSpeeds_t &speeds)  throw(ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering ReadSensorSpeeds", ModuleName().c_str());
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // The response from the protocol Filter
    SerialString_t response;
    Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Checking object status", ModuleName().c_str());
    // Check to see that all our objects are in place
    // This throws a ModuleException if things aren't good
    CheckObjectsStatus();
    Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Calling GetModuleData", ModuleName().c_str());
    status = m_protocolFilter->GetModuleData("ReadSensorSpeeds", response);
    // Ask the protocol filter for the info
    if (status == BEP_STATUS_SUCCESS)
    {
        status = ParseSensorSpeeds( response, speeds);
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting ReadSensorSpeeds", ModuleName().c_str());
    // Return the status of the operation
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::ParseSensorSpeeds(const SerialString_t &response, WheelSpeeds_t &speeds)  throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering ParseSensorSpeeds", ModuleName().c_str());

    try
    {
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Getting reply interpretation for LF", ModuleName().c_str());
        // Get the reply interpretation from the Xml
        const XmlNode *replyInterpNodeLF = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationLF");
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Getting reply interpretation for RF", ModuleName().c_str());
        const XmlNode *replyInterpNodeRF = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationRF");
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Getting reply interpretation for LR", ModuleName().c_str());
        const XmlNode *replyInterpNodeLR = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationLR");
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Getting reply interpretation for RR", ModuleName().c_str());
        const XmlNode *replyInterpNodeRR = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationRR");
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Parsing LF Response", ModuleName().c_str());

        // Try to parse the response into something readable
        speeds[LFWHEEL] = ParseFloatResponse(replyInterpNodeLF->getValue(), response);
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Parsing RF Response", ModuleName().c_str());
        speeds[RFWHEEL] = ParseFloatResponse(replyInterpNodeRF->getValue(), response);
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Parsing LR Response", ModuleName().c_str());
        speeds[LRWHEEL] = ParseFloatResponse(replyInterpNodeLR->getValue(), response);
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Parsing RR Response", ModuleName().c_str());
        speeds[RRWHEEL] = ParseFloatResponse(replyInterpNodeRR->getValue(), response);
        // Log the wheel speed sensors read from the module
        Log(LOG_DEV_DATA, "Parsed Sensor Speeds: \n\tLF = %g\n\tRF = %g\n\tLR = %g\n\tRR = %g\n",
            speeds[LFWHEEL], speeds[RFWHEEL], speeds[LRWHEEL], speeds[RRWHEEL]);
    }
    catch (XmlException &ex)
    {   // Log it
        Log(LOG_ERRORS, "%s: XmlError: %s", ModuleName().c_str(), ex.GetReason());
        // set status to a software error
        status = BEP_STATUS_SOFTWARE;
    }
    
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting ParseSensorSpeeds", ModuleName().c_str());
    // Return the status of the operation
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::ReadBrakeSwitch(bool &switchStatus)  throw(ModuleException)
{   // Logging the function entry
    Log(LOG_FN_ENTRY, "%s: ReadBrakeSwitch()", ModuleName().c_str());
    // Return the status of the read
    return(ReadModuleData("ReadBrakeSwitch", switchStatus));
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::CurrentBrakeSwitchState(bool &state)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering CurrentBrakeSwitchState", ModuleName().c_str());
    state = m_currentBrakeSwitchState;
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting CurrentBrakeSwitchState", ModuleName().c_str());
    // Return the status of the operation
    return BEP_STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::SawBrakeSwitchOff(bool &flag)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering SawBrakeSwitchOff", ModuleName().c_str());
    flag = m_sawBrakeSwitchOff;
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting SawBrakeSwitchOff", ModuleName().c_str());
    // Return the status of the operation
    return BEP_STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::SawBrakeSwitchOn(bool &flag)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering SawBrakeSwitchOn", ModuleName().c_str());
    flag = m_sawBrakeSwitchOn;
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting SawBrakeSwitchOn", ModuleName().c_str());
    // Return the status of the operation
    return BEP_STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::ClearBrakeSwitchStatus(void)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering ClearBrakeSwitchStatus", ModuleName().c_str());
    m_sawBrakeSwitchOn = false;
    m_sawBrakeSwitchOff = false;
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting ClearBrakeSwitchStatus", ModuleName().c_str());
    // Return the status of the operation
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
bool GenericABSModuleTemplate<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{
    Log(LOG_FN_ENTRY, "%s: InitializeHook()", GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str());
    return GenericModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::StartFiringTractionControlValves(const string &valves, 
																					   const INT32& messageGap)
{	// Check to see if there is already a valve controller
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
	if (m_valveController == NULL)
	{	// No controller, create a new one
		string fireMessageTag("Fire"+valves+"TractionControlValves");
		string stopFireMessageTag("Stop"+valves+"TractionControlValves");
		m_valveController = new ABSValveControl(m_protocolFilter, fireMessageTag, stopFireMessageTag, 
												messageGap, ContinuousFire, 0);
		// Start the thread running
		m_valveController->Resume(false);
		status = BEP_STATUS_SUCCESS;
	}
	// Return the status
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::StopFiringTractionControlValves(void)
{	// Stop the firing the traction control valves
	BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
	if (m_valveController != NULL)
	{	// Stop firing the valves
		Log(LOG_DEV_DATA, "Signaling the ABS valve controller thread to stop...\n");
		m_valveController->StopMonitor();
		m_valveController->WaitUntilDone();
		delete m_valveController;
	}
	// Set the pointer to NULL so we can restart later if needed
	Log(LOG_DEV_DATA, "ABS control thread complete, setting pointer to NULL\n");
	m_valveController = NULL;
	// Completed, update the result and return
	Log(LOG_DEV_DATA, "StopFiringTractionControlValves() complete!\n");
	status = BEP_STATUS_SUCCESS;
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::StartPulsingTractionControlValves(const string &valves, 
																						const INT32 &pulseLength, 
																						const INT32 &pulseDelay)
{	// Check to see if there is already a valve controller
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
	if (m_valveController == NULL)
	{	// No controller, create a new one
		string fireMessageTag("Fire"+valves+"TractionControlValves");
		string stopFireMessageTag("Stop"+valves+"TractionControlValves");
		m_valveController = new ABSValveControl(m_protocolFilter, fireMessageTag, stopFireMessageTag, 
												pulseDelay, PulseBrake, pulseLength, pulseDelay);
		// Start the thread running
		m_valveController->Resume(false);
		status = BEP_STATUS_SUCCESS;
	}
	// Return the status
	return status;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericABSModuleTemplate<ProtocolFilterType>::StopPulsingTractionControlValves(void)
{
	return StopFiringTractionControlValves();
}
