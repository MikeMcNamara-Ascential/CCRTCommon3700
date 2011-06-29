//******************************************************************************
// Description:
//  Module Interface for a generic Emissions/Engine Controller module.
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
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/EngineController/GenericEmissionsModuleTemplate.cpp $
// 
// 1     11/01/07 1:32p Derickso
// Updated to use ReadModuleData and CommandModule instead of
// ProtocolFilter::GetModuleData directly.
// 
// 1     10/10/07 2:48p Derickso
// Refactored to make use of CommandModule and ReadModuleData.
// 
// 11    2/22/07 8:41a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added a SkipParkNeutralSwitch() check in ReadSwitches().
// - Added ActivateHydraulicCoolingFan().
// - Added InFieldModeRegister and InFieldModeBit configuration items.
// - Added IsInFieldMode().
// - Added GetInFieldModeRegister() and GetInFieldModeBit().
// - Added SetInFieldModeRegister() and SetInFieldModeBit().
// - Added ToggleMdsInhibit().
// 
// 10    1/17/07 2:50p Rwiersem
// Changes for the 07011901 core release:
// 
// - Set the member variables to their default state in CloseCom().
// - Added OpenCom().
// 
// 9     10/25/06 3:01p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a last transmit time to the ProtocolFilter
//     - Added CloseCom()
// 
// 8     9/05/06 3:39p Rwiersem
// Changes for the 06090601 core release:
// 
// - Fixed "all switches detected" logic in ReadSwitches()
// - Fixed ClutchUpstopSwitchOff detected logic in ReadSwitches()
// - Added Set methods for engine authentication strings
// - Added Get methods for engine authentication strings
// - Added StartQuickLearnEngineAuthentication()
// - Added StopQuickLearnEngineAuthentication()
// 
// 7     4/13/06 3:14a Cward
// Updates for optional park/neutral switch testing
//
// 1     2/14/06 10:21a Derickso
// Updates for optional Park/Neutral switch testing.
//
// 6     11/11/05 4:54p Cward
// Changes for 0511101 iteration
//
// 5     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 4     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 3     9/29/04 10:02p Cward
// Made Cruise test more generic
//
// 2     9/01/04 8:46p Cward
// Changed parameter type for Read Cruise Brake from bool to string. Added
// functions for starting and stopping the hydraulic cooling fan.
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
#include "GenericEmissionsModuleTemplate.h"
#include "DataAnalysis.h"

template <class ProtocolFilterType>
GenericEmissionsModuleTemplate<ProtocolFilterType>::GenericEmissionsModuleTemplate(void) :
GenericModuleTemplate<ProtocolFilterType>(), m_tpsMonitor(NULL),
m_brakeSwitchOn(false), m_brakeSwitchOff(false), m_redundantBrakeSwitchOn(false),
m_redundantBrakeSwitchOff(false), m_boolMonitorRedundantBrakeSwitch(false),
m_parkNeutralSwitchOn(false), m_parkNeutralSwitchOff(false), m_boolMonitorParkNeutralSwitch(true),
m_clutchSwitchOn(false), m_clutchSwitchOff(false), m_clutchUpstopSwitchOn(false),
m_clutchUpstopSwitchOff(false), m_clutchOffUpstopOn(false), m_boolMonitorClutchUpstopSwitch(false),
m_initialTPS(0.0), m_finalTPSReading(0.0), m_observedDeltaTPS(0.0), m_skipParkNeutralSwitchCheck(false)
{   // Create a new TPS monitor comm object
    m_tpsMonitorComm = new ProtocolFilterType(m_lastTxTime, &m_stopCommsFlag, &m_commsInUse);
}

template <class ProtocolFilterType>
GenericEmissionsModuleTemplate<ProtocolFilterType>::~GenericEmissionsModuleTemplate(void)
{   // Delete the TPS monitor
    if (m_tpsMonitor != NULL)
    {   // Stop reading the throttle position
        Log(LOG_DEV_DATA, "Signaling the TPS monitor to stop...\n");
        m_tpsMonitor->StopMonitor();
        m_tpsMonitor->WaitUntilDone();
        if (m_tpsMonitor != NULL)
            delete m_tpsMonitor;
    }
    m_tpsMonitor = NULL;
    // Delete the comm objects we created
    if (m_tpsMonitorComm != NULL) delete m_tpsMonitorComm;
    m_tpsMonitorComm = NULL;
}

template <class ProtocolFilterType>
void GenericEmissionsModuleTemplate<ProtocolFilterType>::CloseCom()
{
    // Delete the TPS monitor
    if(m_tpsMonitor != NULL)
    {   // Stop reading the throttle position
        Log(LOG_DEV_DATA, "Signaling the TPS monitor to stop...\n");
        m_tpsMonitor->StopMonitor();
        m_tpsMonitor->WaitUntilDone();
        if(m_tpsMonitor != NULL)
            delete m_tpsMonitor;

        m_tpsMonitor = NULL;
    }

    m_brakeSwitchOn = false;
    m_brakeSwitchOff = false;
    m_redundantBrakeSwitchOn = false;
    m_redundantBrakeSwitchOff = false;
    m_boolMonitorRedundantBrakeSwitch = false;
    m_parkNeutralSwitchOn = false;
    m_parkNeutralSwitchOff = false;
    m_boolMonitorParkNeutralSwitch = true;
    m_clutchSwitchOn = false;
    m_clutchSwitchOff = false;
    m_clutchUpstopSwitchOn = false;
    m_clutchUpstopSwitchOff = false;
    m_clutchOffUpstopOn = false;
    m_boolMonitorClutchUpstopSwitch = false;
    m_initialTPS = 0.0;
    m_finalTPSReading = 0.0;
    m_observedDeltaTPS = 0.0;
    m_skipParkNeutralSwitchCheck = false;

    // Clear out XmlNodeMaps
    m_engineTypes.clear(true);
    m_vehicleTypes.clear(true);
    m_emissionsTypes.clear(true);
    if( NULL != m_tpsMonitorComm)   m_tpsMonitorComm->CloseCom();

    GenericModuleTemplate<ProtocolFilterType>::CloseCom();
}

template <class ProtocolFilterType>
bool GenericEmissionsModuleTemplate<ProtocolFilterType>::OpenCom()
{
    bool status = false;

    // Re-initialize our data members from the config file data
    InitializeHook( m_configNode);

    // Re-open the TPS monitor comm object
    if( NULL != m_tpsMonitorComm)   m_tpsMonitorComm->OpenCom();

    status = GenericModuleTemplate<ProtocolFilterType>::OpenCom();

    return( status);
}

template <class ProtocolFilterType>
inline string GenericEmissionsModuleTemplate<ProtocolFilterType>::ModuleName()
{
    return "Generic Emissions/Engine Controller";
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, string &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the serial number from the module
    if (methodName == "ReadSerialNumber")                status = ReadModuleData(methodName, value);
    // Read the emissions compliance model year
    else if (methodName == "ReadEmissionsModelYear")     status = ReadModuleData(methodName, value);
    // Read the emissions configuration
    else if (methodName == "ReadEmissionsConfiguration")
    {   // Read the engine type code from the module
        UINT8 emissionsTypeCode = 0x00;
        status = ReadModuleData(methodName, emissionsTypeCode);
        // Check the status of the read
        if (BEP_STATUS_SUCCESS == status)
        {   // Convert the code into a string representation
            XmlNodeMapItr iter;
            char emissionsTag[16];
            CreateMessage(emissionsTag, sizeof(emissionsTag), "Emissions_%02X", emissionsTypeCode);
            if ((iter = m_emissionsTypes.find(emissionsTag)) != m_emissionsTypes.end())
            {   // Found a translation
                value = iter->second->getValue();
            }
            else
            {   // No translation listed
                Log(LOG_ERRORS, "No emissions type translation for emissions code: %04X\n", emissionsTypeCode);
                value = "Not Listed";
            }
        }
    }
    // Read the engine type the vehicle is equipped with
    else if (methodName == "ReadEngineType")
    {   // Read the engine type code from the module
        UINT16 engineTypeCode = 0x0000;
        status = ReadModuleData(methodName, engineTypeCode);
        // Check the status of the read
        if (BEP_STATUS_SUCCESS == status)
        {   // Convert the code into a string representation
            XmlNodeMapItr iter;
            char engineTag[16];
            CreateMessage(engineTag, sizeof(engineTag), "Engine_%04X", engineTypeCode);
            if ((iter = m_engineTypes.find(engineTag)) != m_engineTypes.end())
            {   // Found a translation
                value = iter->second->getValue();
            }
            else
            {   // No translation listed
                Log(LOG_ERRORS, "No engine type translation for engine code: %04X\n", engineTypeCode);
                value = "Not Listed";
            }
        }
    }
    // Read the vehicle body style
    else if (methodName == "ReadVehicleType")
    {   // Read the engine type code from the module
        UINT16 vehicleTypeCode = 0x0000;
        status = ReadModuleData(methodName, vehicleTypeCode);
        // Check the status of the read
        if (BEP_STATUS_SUCCESS == status)
        {   // Convert the code into a string representation
            XmlNodeMapItr iter;
            char vehicleTag[16];
            CreateMessage(vehicleTag, sizeof(vehicleTag), "Vehicle_%04X", vehicleTypeCode);
            if ((iter = m_vehicleTypes.find(vehicleTag)) != m_vehicleTypes.end())
            {   // Found a translation
                value = iter->second->getValue();
            }
            else
            {   // No translation listed
                Log(LOG_ERRORS, "No vehicle type translation for vehicle code: %04X\n", vehicleTypeCode);
                value = "Not Listed";
            }
        }
    }
    // Read the air conditioner on/off status
    else if (methodName == "ReadAirConditionerStatus")
    {   // Read the engine type code from the module
        UINT8 acStatusCode = 0x00;
        status = ReadModuleData(methodName, acStatusCode);
        // Check the status of the read
        if (BEP_STATUS_SUCCESS == status)
        {   // Check the A/C status bit to determine if the A/C is On
            UINT8 acStatusMask = 0x01 << GetACStatusBit();
            if (acStatusCode & acStatusMask)   value = "On";
            else                               value = "Off";
            // Log the data
            Log(LOG_DEV_DATA, "A/C status: %s\n", value.c_str());
        }
    }
    // Try using the base class to get the info
    else   status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT16 &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the upline status register
    if (methodName == "ReadUplineRegister")          status = ReadModuleData(methodName, value);
    // Try to use the base class to get the data
    else   status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<UINT16>&value) throw(ModuleException)
{   // Nothing special to do, just pass to base class for evaluation
    return(GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<UINT8>&value) throw(ModuleException)
{   // Nothing special to do, just pass to base class for evaluation
    return(GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, int &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the idle engine rpm from the module
    if (methodName == "ReadEngineIdleRPM")   status = ReadModuleData(methodName, value);
    // Nothing special to do, just pass to base class for evaluation
    else  status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Get the neutral cell id
    if (methodName == "ReadNeutralState")  status = ReadModuleData(methodName, value);
    // No special implementation, return the status of the base class operation
    else  status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, bool &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Check if brake switch on state was detected
    if (methodName == "CheckBrakeSwitchOn")              status = CheckBrakeSwitchOn(value);
    // Check if brake switch off state was detected
    else if (methodName == "CheckBrakeSwitchOff")        status = CheckBrakeSwitchOff(value);
    // Check if the park/neutral switch on state was detected
    else if (methodName == "CheckParkNeutralSwitchOn")   status = CheckParkNeutralSwitchOn(value);
    // Check if the park/neutral switch off state was detected
    else if (methodName == "CheckParkNeutralSwitchOff")  status = CheckParkNeutralSwitchOff(value);
    // Check if the clutch switch on state was detected
    else if (methodName == "CheckClutchSwitchOn")        status = CheckClutchSwitchOn(value);
    // Check if the clutch switch off state was detected
    else if (methodName == "CheckClutchSwitchOff")       status = CheckClutchSwitchOff(value);
    // Determine if the vehicle has previously passed rolls testing
    else if (methodName == "IsPreviousPass")             status = IsPreviousPass(value);
    // Determine if module is in field mode
    else if (methodName == "IsInFieldMode")          status = IsInFieldMode(value);
    // Determine if this is an automatic transmission
    else if (methodName == "ReadTransmissionType")       status = ReadModuleData(methodName, value);
    // Check if redundant brake switch on state was detected
    else if (methodName == "CheckRedundantBrakeSwitchOn")       status = CheckRedundantBrakeSwitchOn(value);
    // Check if redundant brake switch off state was detected
    else if (methodName == "CheckRedundantBrakeSwitchOff")  status = CheckRedundantBrakeSwitchOff(value);
    // Check if clutch Upstop switch on state was detected
    else if (methodName == "CheckClutchUpstopSwitchOn")         status = CheckClutchUpstopSwitchOn(value);
    // Check if clutch Upstop switch off state was detected
    else if (methodName == "CheckClutchUpstopSwitchOff")    status = CheckClutchUpstopSwitchOff(value);
    // Try to use the base class to get the info
    else   status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the current stored faults
    if (methodName == "ReadFaults") status = ReadFaults(value);
    // Try to use the base class to get the info
    else   status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, float &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the MAP sensor value
    if (methodName == "ReadMAPSensor")               status = ReadModuleData(methodName, value);
    // Read the throttle sensor voltage
    else if (methodName == "ReadThrottleVoltage")    status = ReadModuleData(methodName, value);
    // Read the throttle sensor voltage
    else if (methodName == "ReadAcceleratorPedalSensor1")    status = ReadModuleData(methodName, value);
    // Read the speed control target speed
    else if (methodName == "ReadCruiseTargetSpeed")  status = ReadModuleData(methodName, value);
    // Read the Vehicle speed
    else if (methodName == "ReadVehicleSpeed")       status = ReadModuleData(methodName, value);
    // Try using the base class to get the info
    else   status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::UpdateEOLStatusByte(const UINT16 &statusByte, const UINT16 &value)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Write the data to the module
    status = WriteEOLStatusRegister(statusByte, value);
    // Determine the status of the update
    if (BEP_STATUS_SUCCESS == status)
    {   // EEPROM updated successfully
        Log("Updated EOL status byte -- Status byte: %04X, Value: %04X\n", statusByte, value);
    }
    else if ((BEP_STATUS_FAILURE == status) && (UnlockIfUpdateFailed()))
    {   // Module security may not be unlocked, try unlocking and writing again
        status = UnlockModuleSecurity();
        if (BEP_STATUS_SUCCESS == status)
        {
            status = WriteEOLStatusRegister(statusByte, value);
            // Determine the status of the update
            if (BEP_STATUS_SUCCESS == status)
            {   // EEPROM updated successfully
                Log("Updated EOL status byte -- Status byte: %04X, Value: %04X\n", statusByte, value);
            }
            else
            {   // Error updating status byte
                Log("Error updating EOL status byte -- Status: %s, Status byte: %04X, Value: %04X\n",
                    ConvertStatusToResponse(status).c_str(), statusByte, value);
            }
        }
        else
        {
            Log(LOG_ERRORS, "Could not unlock security after failed write - status: %s", ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {   // Error updating status byte
        Log("Error updating EOL status byte -- Status: %s, Status byte: %04X, Value: %04X\n",
            ConvertStatusToResponse(status).c_str(), statusByte, value);
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::UpdateEEPROMTestBit(const UINT16 address, const UINT16 testBit,
                                                                                        const EEPROM_TEST_RESULT testResult)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    DataAnalysis analyze;
    UINT16 statusByte = 0x0000;
    // Shift the test result to the proper position
    UINT16 eepromBitMask = 1 << testBit;
    // Read the byte from the module
    status = ReadEOLStatusRegister(address, statusByte);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {   // Clear the appropriate bits
        UINT16 eepromUpdate = analyze.ConvertData((UINT32)statusByte, (UINT32)eepromBitMask,
                                                  (testResult == EEPROM_PASS ? CLEAR_BITS : SET_BITS));
        // Write the new value to the EEPROM
        status = UpdateEOLStatusByte(address, eepromUpdate);
        // Determine if there was an error updating the EEPROM
        if (BEP_STATUS_SUCCESS != status) Log("Error updating EEPROM - status:%s, status byte:%04X, value:%04X\n",
                                             ConvertStatusToResponse(status).c_str(), address, eepromUpdate);
    }
    else
    {   // Error reading the status byte from the module
        Log("Error reading status byte from the PCM - status: %d, status byte: $%04X\n", status, address);
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadCruiseMode(const string &message, UINT16 &currentMode)
{   // Read the cruise mode from the module
    return ReadModuleData(message, currentMode);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadCruiseSwitch(string &switchPressed)
{   // Read the cruise switch being pressed
    return ReadModuleData("ReadCruiseSwitch", switchPressed);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadCruiseBrake(string &cutoutReason)
{   // Read the brake switch for cruise
    return ReadModuleData("ReadCruiseBrake", cutoutReason);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadAxleSpeed(const string &axle, float &axleSpeed)
{   // Get the axle speed from the module
    return ReadModuleData("ReadAxleSpeed" + axle, axleSpeed);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::EnterDiagnosticSession()
{   // Command the module to enter diagnostic session
    return CommandModule("EnterDiagnosticSession");
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::UnlockModuleSecurity()
{
    Log(LOG_ERRORS, "UnlockModuleSecurity() not implemented.  Please implement before using\n");
    return BEP_STATUS_SOFTWARE;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::StartTPSMonitor(const string &messageTag, const bool resuming /*= false*/)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Get the Initial TPS reading
    float initialTPS;
    // Check if we need to read the initial TPS
    if (!resuming)  status = GetInfo(messageTag, initialTPS);
    else                status = BEP_STATUS_SUCCESS;
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {   // Store the initial TPS reading
        if (!resuming)  SetInitialTPS(initialTPS);
        // Check if the throttle position sensor monitor exists
        if (m_tpsMonitor == NULL)
        {   // Create a new tps monitor object
            string tpsMonitorDelay = m_configNode->getChild("Setup/TPSMonitorDelay")->getValue();
            m_tpsMonitor = new ThrottlePositionMonitor<ProtocolFilterType>(this, m_tpsMonitorComm, messageTag,
                                                                           BposReadInt(tpsMonitorDelay.c_str()));
            // Start the thread monitoring
            m_tpsMonitor->Resume(false);
            status = BEP_STATUS_SUCCESS;
        }
        else
        {   // Throttle position monitor object does not already exists
            Log(LOG_DEV_DATA, "TPS monitor already exists, do not need to start\n");
            status = BEP_STATUS_SUCCESS;
        }
    }
    else
    {   // Error getting the initial TPS reading
        Log(LOG_ERRORS, "Error reading initial TPS value - status: %s\n", ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::StopTPSMonitor()
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // If the throttle position monitor exists, delete it
    if (m_tpsMonitor != NULL)
    {   // Stop reading the throttle position
        Log(LOG_DEV_DATA, "Signaling the TPS monitor to stop...\n");
        m_tpsMonitor->StopMonitor();
        m_tpsMonitor->WaitUntilDone();
        if (m_tpsMonitor != NULL)
            delete m_tpsMonitor;
    }
    // Set the pointer to NULL in case we need it later
    Log(LOG_DEV_DATA, "Tps monitor complete, setting the pointer to NULL\n");
    m_tpsMonitor = NULL;
    // Set the status to success
    status = BEP_STATUS_SUCCESS;
    Log(LOG_DEV_DATA, "StopTPSMonitor() complete\n");
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadSwitches(IProtocolFilter *filter)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    SerialString_t moduleResponse;
    bool allSwitchesDetected = false;
    // Determine if switches still need to be monitored - inline broken JS added check for upstop off
    if(DetectedBrakeSwitchOn() && DetectedBrakeSwitchOff())    // Both brake switch states detected
    {
        if((IsRedundantBrakeSwitchEquipped() && DetectedRedundantBrakeSwitchOn()) || !IsRedundantBrakeSwitchEquipped())
        {
            if((IsRedundantBrakeSwitchEquipped() && DetectedRedundantBrakeSwitchOff()) || !IsRedundantBrakeSwitchEquipped())
            {
                if (IsAutomaticTransmission() && ((DetectedParkNeutralSwitchOn() && DetectedParkNeutralSwitchOff()) ||
                    SkipParkNeutralSwitch()))
                {                      //Auto trans
                    allSwitchesDetected = true;
                }
                else if(!IsAutomaticTransmission() && DetectedClutchSwitchOn() && DetectedClutchSwitchOff())
                {                      //Manual
                    if(IsClutchUpstopSwitchEquipped() && DetectedClutchOffUpstopOn() && DetectedClutchUpstopSwitchOff())
                    {                  //Upstop Switch equipped
                        allSwitchesDetected = true;
                    }
                    else if(!IsClutchUpstopSwitchEquipped())
                    {
                        allSwitchesDetected = true;
                    }
                }
            }
        }
    }
    if(allSwitchesDetected)
    {
        // Saw all required switches in both on and off states, stop monitoring
        Log(LOG_DEV_DATA, "Detected all switches in both ON and OFF states\n");
    }
    else
    {   // Read the switches from the module
        status = filter->GetModuleData("ReadSwitches", moduleResponse);
        // Check the status of the read
        if (BEP_STATUS_SUCCESS == status)
        {   // Good read, extract the data
            try
            {   // Check for brake switch on if not detected yet
                if (!DetectedBrakeSwitchOn())
                {
                    SetBrakeSwitchOnDetected(ParseBooleanResponse(GetReplyInterpretationTag("ReadBrakeSwitchOn"), moduleResponse));
                    if (DetectedBrakeSwitchOn()) Log(LOG_DEV_DATA, "Detected Brake switch ON\n");
                }
                // Check for brake switch off if not detected yet
                if (!DetectedBrakeSwitchOff())
                {
                    SetBrakeSwitchOffDetected(ParseBooleanResponse(GetReplyInterpretationTag("ReadBrakeSwitchOff"), moduleResponse));
                    if (DetectedBrakeSwitchOff()) Log(LOG_DEV_DATA, "Detected Brake switch OFF\n");
                }
                // Check for Redundant brake switch on if not detected yet
                if (IsRedundantBrakeSwitchEquipped() && !DetectedRedundantBrakeSwitchOn())
                {
                    SetRedundantBrakeSwitchOnDetected(ParseBooleanResponse(GetReplyInterpretationTag("ReadRedundantBrakeSwitchOn"), moduleResponse));
                    if (DetectedRedundantBrakeSwitchOn()) Log(LOG_DEV_DATA, "Detected Redundant Brake switch ON\n");
                }
                // Check for Redundant brake switch off if not detected yet
                if (IsRedundantBrakeSwitchEquipped() && !DetectedRedundantBrakeSwitchOff())
                {
                    SetRedundantBrakeSwitchOffDetected(ParseBooleanResponse(GetReplyInterpretationTag("ReadRedundantBrakeSwitchOff"), moduleResponse));
                    if (DetectedRedundantBrakeSwitchOff())  Log(LOG_DEV_DATA, " Detected Redundant Brake switch OFF\n");
                }
                // Check for Park/Neutral switch on if not detected yet and transmission is automatic
                if ((IsAutomaticTransmission() && !SkipParkNeutralSwitch()) && !DetectedParkNeutralSwitchOn())
                {
                    SetParkNeutralSwitchOnDetected(ParseBooleanResponse(GetReplyInterpretationTag("ReadParkNeutralSwitchOn"), moduleResponse));
                    if (DetectedParkNeutralSwitchOn()) Log(LOG_DEV_DATA, "Detected Park/Neutral switch ON\n");
                }
                // Check for Park/Neutral switch off if not detected yet and transmission is automatic
                if ((IsAutomaticTransmission() && !SkipParkNeutralSwitch()) && !DetectedParkNeutralSwitchOff())
                {
                    SetParkNeutralSwitchOffDetected(ParseBooleanResponse(GetReplyInterpretationTag("ReadParkNeutralSwitchOff"), moduleResponse));
                    if (DetectedParkNeutralSwitchOff()) Log(LOG_DEV_DATA, "Detected Park/Neutral switch OFF\n");
                }
                // Check for clutch switch on if not detected yet and transmission is manual
                if (!IsAutomaticTransmission() && !DetectedClutchSwitchOn())
                {
                    SetClutchSwitchOnDetected(ParseBooleanResponse(GetReplyInterpretationTag("ReadClutchSwitchOn"), moduleResponse));
                    if (DetectedClutchSwitchOn()) Log(LOG_DEV_DATA, "Detected Clutch switch ON\n");
                }
                // Check for clutch switch off if not detected and transmission is manual
                if (!IsAutomaticTransmission() && !DetectedClutchSwitchOff())
                {
                    SetClutchSwitchOffDetected(ParseBooleanResponse(GetReplyInterpretationTag("ReadClutchSwitchOff"), moduleResponse));
                    if (DetectedClutchSwitchOff()) Log(LOG_DEV_DATA, "Detected Clutch switch OFF\n");
                }
                // Check for clutch Upstop switch on if not detected yet and transmission is manual
                if (!IsAutomaticTransmission() && IsClutchUpstopSwitchEquipped() && !DetectedClutchUpstopSwitchOn())
                {
                    SetClutchUpstopSwitchOnDetected(ParseBooleanResponse(GetReplyInterpretationTag("ReadClutchUpstopSwitchOn"), moduleResponse));
                    if (DetectedClutchUpstopSwitchOn()) Log(LOG_DEV_DATA, "Detected ClutchUpstop switch ON\n");
                }
                // Check for clutch Upstop switch off if not detected and transmission is manual
                if (!IsAutomaticTransmission() && IsClutchUpstopSwitchEquipped() && !DetectedClutchUpstopSwitchOff())
                {
                    SetClutchUpstopSwitchOffDetected(ParseBooleanResponse(GetReplyInterpretationTag("ReadClutchUpstopSwitchOff"), moduleResponse));
                    if (DetectedClutchUpstopSwitchOff()) Log(LOG_DEV_DATA, "Detected ClutchUpstop switch OFF\n");
                }
                // Check for clutch Off and Upstop switch On were detected at the same time. Transmission is manual
                if (!IsAutomaticTransmission() && IsClutchUpstopSwitchEquipped() && !DetectedClutchOffUpstopOn())
                {
                    SetClutchOffUpstopOnDetected( (ParseBooleanResponse(GetReplyInterpretationTag("ReadClutchUpstopSwitchOn"), moduleResponse)) &&
                                                  (ParseBooleanResponse(GetReplyInterpretationTag("ReadClutchSwitchOff"), moduleResponse)));
                    if (DetectedClutchOffUpstopOn()) Log(LOG_DEV_DATA, "Detected Clutch Off Upstop switch On\n");
                }
            }
            catch (XmlException &err)
            {   // Log the reason for the exception
                Log(LOG_ERRORS, (ModuleName() + " XmlException in ReadSwitches: " + err.GetReason()).c_str());
                // Set the status to software error
                status = BEP_STATUS_SOFTWARE;
            }
        }
        else
        {   // Error reading the switches
            Log(LOG_ERRORS, "Error reading switches from %s - status: %s\n", ModuleName().c_str(),
                ConvertStatusToResponse(status).c_str());
        }
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::StartSwitchMonitor(void) throw(ModuleException)
{   // Need to determine type of transmission first
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool isAutomaticTransmission = false;
    moduleStatus = GetInfo("ReadTransmissionType", isAutomaticTransmission);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == moduleStatus)
    {   // Store the transmission type
        Log(LOG_DEV_DATA, "Automatic Transmission: %s\n", isAutomaticTransmission ? "Yes" : "No");
        SetTransmissionType(isAutomaticTransmission);
        // Go ahead and start the monitor
        moduleStatus = GenericModuleTemplate<ProtocolFilterType>::StartSwitchMonitor();
    }
    else
    {   // Could not determine transmission type - not starting switch monitor
        Log(LOG_ERRORS, "Error reading transmission type from the module, NOT STARTING SWITCH MONITOR!\n");
    }
    // Return the status
    return moduleStatus;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadThrottlePosition(IProtocolFilter *filter, const string &messageTag)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    float currentTPS = 0.0;
    float requiredDeltaTPS = atof(m_configNode->getChild("Setup/DeltaTPSRequired")->getValue().c_str());
    // Determine if there is anything to be done
    if (GetObservedDeltaTPS() <= requiredDeltaTPS)
    {   // Read the throttle position from the module
        status = ReadModuleData(messageTag, currentTPS, NULL, NULL, filter);
        // Check the status of the read
        if (BEP_STATUS_SUCCESS == status)
        {   // Compare the current TPS reading to the initial
            float currentDeltaTPS = currentTPS - GetInitialTPSReading();
            // Check to see if desired delta TPS was observed.  If so, stop monitoring throttle position
            if (currentDeltaTPS >= requiredDeltaTPS)
            {   // Required delta TPS observed
                SetFinalTPSReading(currentTPS);      // Store the final TPS reading
                SetObservedDeltaTPS(currentDeltaTPS);    // Store the observed delta
            }
        }
        else
        {   // Error reading throttle position sensor
            Log(LOG_ERRORS, "Error reading delta throttle position - status: %s\n", ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {
        status = BEP_STATUS_SUCCESS;
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetTransmissionType(const bool &isAutomaticTransmission)
{
    m_automaticTransmission = isAutomaticTransmission;
}

template <class ProtocolFilterType>
inline const bool &GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedBrakeSwitchOn(void)
{
    return m_brakeSwitchOn;
}

template <class ProtocolFilterType>
inline const bool &GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedBrakeSwitchOff(void)
{
    return m_brakeSwitchOff;
}

template <class ProtocolFilterType>
inline const bool &GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedClutchSwitchOn(void)
{
    return m_clutchSwitchOn;
}

template <class ProtocolFilterType>
inline const bool &GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedClutchSwitchOff(void)
{
    return m_clutchSwitchOff;
}

template <class ProtocolFilterType>
inline const bool &GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedParkNeutralSwitchOn(void)
{
    return m_parkNeutralSwitchOn;
}

template <class ProtocolFilterType>
inline const bool &GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedParkNeutralSwitchOff(void)
{
    return m_parkNeutralSwitchOff;
}

template <class ProtocolFilterType>
inline const bool &GenericEmissionsModuleTemplate<ProtocolFilterType>::IsAutomaticTransmission(void)
{
    return m_automaticTransmission;
}

template <class ProtocolFilterType>
inline const bool& GenericEmissionsModuleTemplate<ProtocolFilterType>::SkipParkNeutralSwitch(const bool *skipParkNeutralSwitch)
{   // Determine if the value should be stored
    if (skipParkNeutralSwitch != NULL)  m_skipParkNeutralSwitchCheck = *skipParkNeutralSwitch;
    return m_skipParkNeutralSwitchCheck;
}

template <class ProtocolFilterType>
inline const float &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInitialTPSReading(void)
{
    return m_initialTPS;
}

template <class ProtocolFilterType>
inline const float &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetFinalTPSReading(void)
{
    return m_finalTPSReading;
}

template <class ProtocolFilterType>
inline const float &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetObservedDeltaTPS(void)
{
    return m_observedDeltaTPS;
}

template <class ProtocolFilterType>
inline const UINT16 &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetOkToShipRegister(void)
{
    return m_okToShipRegister;
}

template <class ProtocolFilterType>
inline const INT32 &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetOKToShipBit(void)
{
    return m_okToShipBit;
}

template <class ProtocolFilterType>
inline const INT32 &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetACStatusBit(void)
{
    return m_acStatusBit;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ActivateHydraulicCoolingFan(IProtocolFilter *filter)
{   // Command the module to start the hydraulic cooling fan
    return CommandModule("StartHydraulicCoolingFan", NULL, NULL, filter);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::StartRadiatorFan(string fanType)
{   // Command the module to start the hydraulic cooling fan
    return CommandModule("Start" + fanType + "Fan");
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::StopRadiatorFan(string fanType)
{   // Command the module to stop the hydraulic cooling fan
    return CommandModule("Stop" + fanType + "Fan");
}

template <class ProtocolFilterType>
bool GenericEmissionsModuleTemplate<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{
    bool retVal = true;

    // Load the engine type translation table
    try
    {
        m_engineTypes.DeepCopy(configNode->getChild("Setup/EngineTypes")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "XmlException loading engine type translation table: %s\n", excpt.GetReason());
        m_engineTypes.clear(true);
    }
    // Load the vehicle type translation table
    try
    {
        m_vehicleTypes.DeepCopy(configNode->getChild("Setup/VehicleTypes")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "XmlException loading vehicle type translation table: %s\n", excpt.GetReason());
        m_vehicleTypes.clear(true);
    }
    // Load the emissions type translation table
    try
    {
        m_emissionsTypes.DeepCopy(configNode->getChild("Setup/EmissionsTypes")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "XmlException loading emissions type translation table: %s\n", excpt.GetReason());
        m_emissionsTypes.clear(true);
    }
    // Get the OK to Ship register
    try
    {
        SetOkToShipRegister(configNode->getChild("Setup/OKToShipRegister")->getValue());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "XmlException getting Ok TO ship register: %s\n", excpt.GetReason());
        SetOkToShipRegister(0x0000);
    }
    // Get the A/C status bit
    try
    {
        SetACStatusBit(BposReadInt(configNode->getChild("Setup/ACStatusBit")->getValue().c_str()));
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "XmlException getting A/C status bit: %s\n", excpt.GetReason());
        SetACStatusBit(0);
    }
    // Get the OK to ship bit
    try
    {
        SetOKToShipBit(BposReadInt(configNode->getChild("Setup/OKToShipBit")->getValue().c_str()));
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "XmlException getting OK To ship bit: %s\n", excpt.GetReason());
        SetOKToShipBit(0);
    }
    // Get the flag indiacting if module security should be unlocked if status byte update fails.
    try
    {
        SetUnlockIfUpdateFailed(configNode->getChild("Setup/UnlockIfUpdateFailed")->getValue() == "True");
    }
    catch (XmlException &excpt)
    {
        SetUnlockIfUpdateFailed(false);
    }
    // Get the in field mode register
    try
    {
        SetInFieldModeRegister(atoh(m_configNode->getChild("Setup/InFieldModeRegister")->getValue().c_str()));
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "XmlException getting In Field Mode register: %s\n", excpt.GetReason());
        SetInFieldModeRegister(0x0000);
    }
    // Get the in field mode bit
    try
    {
        SetInFieldModeBit(BposReadInt(m_configNode->getChild("Setup/InFieldModeBit")->getValue().c_str()));
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "XmlException getting In Field Mode bit: %s\n", excpt.GetReason());
        SetInFieldModeBit(0);
    }
    // Get the QuickLearn Engine Authentication representations
    try
    {                                  // Get the Engine Authentication Routine Stopped string
        SetEngineAuthenticationStoppedString(configNode->getChild("Setup/EngineAuthenticationRepresentation/Stopped")->getValue());
    }
    catch(XmlException &err)
    {                                  // Error getting Routine Stopped string
        Log(LOG_ERRORS, "Error getting Setup/EngineAuthenticationRepresentation/Stopped: %s\n", err.Reason().c_str());
        SetEngineAuthenticationStoppedString("Routine Not Running");
    }
    try
    {                                  // Get the Engine Authentication Routine Running string
        SetEngineAuthenticationRunningString(configNode->getChild("Setup/EngineAuthenticationRepresentation/Running")->getValue());
    }
    catch(XmlException &err)
    {                                  // Error getting Routine Running string
        Log(LOG_ERRORS, "Error getting Setup/EngineAuthenticationRepresentation/Running: %s\n", err.Reason().c_str());
        SetEngineAuthenticationRunningString("Routine Running");
    }
    try
    {                                  // Get the Engine Authentication Stop Request Pending string
        SetEngineAuthenticationStopPendingString(configNode->getChild("Setup/EngineAuthenticationRepresentation/StopPending")->getValue());
    }
    catch(XmlException &err)
    {                                  // Error getting Stop Request Pending string
        Log(LOG_ERRORS, "Error getting Setup/EngineAuthenticationRepresentation/StopPending: %s\n", err.Reason().c_str());
        SetEngineAuthenticationStopPendingString("Stop Request Pending");
    }
    try
    {                                  // Get the Engine Authentication Start Request Pending string
        SetEngineAuthenticationStartPendingString(configNode->getChild("Setup/EngineAuthenticationRepresentation/StartPending")->getValue());
    }
    catch(XmlException &err)
    {                                  // Error getting Start Request Pending string
        Log(LOG_ERRORS, "Error getting Setup/EngineAuthenticationRepresentation/StartPending: %s\n", err.Reason().c_str());
        SetEngineAuthenticationStartPendingString("Start Request Pending");
    }
    // Call the base class to finish off the initialization
    retVal = GenericModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);

    return( retVal);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckBrakeSwitchOn(bool &sawBrakeSwitchOn)
{
    sawBrakeSwitchOn = DetectedBrakeSwitchOn();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckBrakeSwitchOff(bool &sawBrakeSwitchOff)
{
    sawBrakeSwitchOff = DetectedBrakeSwitchOff();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckParkNeutralSwitchOn(bool &sawParkNeutralSwitchOn)
{
    sawParkNeutralSwitchOn = DetectedParkNeutralSwitchOn();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckParkNeutralSwitchOff(bool &sawParkNeutralSwitchOff)
{
    sawParkNeutralSwitchOff = DetectedParkNeutralSwitchOff();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckClutchSwitchOn(bool &sawClutchSwitchOn)
{
    sawClutchSwitchOn = DetectedClutchSwitchOn();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckClutchSwitchOff(bool &sawClutchSwitchOff)
{
    sawClutchSwitchOff = DetectedClutchSwitchOff();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::IsPreviousPass(bool &previousPass)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    UINT16 okToShipRegister;
    // Read the OK To Ship register
    status = ReadEOLStatusRegister(GetOkToShipRegister(), okToShipRegister);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {   // Good read, determine if vehicle has previously passed rolls testing
        Log(LOG_DEV_DATA, "OK to ship bit: %d\n", GetOKToShipBit());
        UINT16 okToShipMask = 0x0001 << GetOKToShipBit();
        Log(LOG_DEV_DATA, "IsPreviousPass: okToShipRegister - 0x%04X {Mask: 0x%04X}\n", okToShipRegister, okToShipMask);
        DataAnalysis analyze;
        previousPass = !analyze.CompareData(okToShipRegister, okToShipMask, ALL_BITS_SET);
        Log(LOG_DEV_DATA, "Previous Pass: %s\n", previousPass ? "True" : "False");
    }
    else
    {   // Error reading the OK To Ship register
        Log(LOG_ERRORS, "Error reading OK To Ship Register (0x%04X) - status: %s\n", GetOkToShipRegister(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes)
{
    Log(LOG_ERRORS, "ReadFaults() not implemented, please implement this method before using\n");
    return BEP_STATUS_SOFTWARE;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadEOLStatusRegister(const UINT16 &eolStatusRegister, UINT16 &registerContents)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    SerialArgs_t processRegister;
    // Place the register into the message
    processRegister.push_back((eolStatusRegister & 0xFF00) >> 8);  // Upper byte of register
    processRegister.push_back(eolStatusRegister & 0x00FF);         // Lower byte of register
    // Read the process register from the module
    status = ReadModuleData("ReadProcessRegister", registerContents, &processRegister);
    // Check the status of the read
    Log(LOG_ERRORS, "Read Process Register - register: %04x, Contents: %04X - status: %s",
                eolStatusRegister, registerContents, ConvertStatusToResponse(status).c_str());
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::WriteEOLStatusRegister(const UINT16 &eolStatusRegister, const UINT16 &registerValue)
{   // Write the value to the PCM EEPROM
    SerialString_t moduleResponse;
    SerialArgs_t arguments;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // place the register into the vector
    arguments.push_back((eolStatusRegister & 0xFF00) >> 8);  // Upper byte of register
    arguments.push_back(eolStatusRegister & 0x00FF);         // Lower byte of register
    // Place the register value into the vector
    arguments.push_back((registerValue & 0xFF00) >> 8);  // Upper byte of data
    arguments.push_back(registerValue & 0x00FF);         // Lower byte of data
    // Write the data to the module
    status = CommandModule("WriteEOLStatusRegister", &arguments);
    // Determine the status of the update
    if (BEP_STATUS_SUCCESS == status)
    {   // EEPROM updated successfully
        Log("Updated Process Register -- Register: %04X, Value: %04X\n", eolStatusRegister, registerValue);
    }
    else
    {   // Error updating status byte
        Log("Error updating Process Register -- Status: %s, Process Register: %04X, Value: %04X\n",
            ConvertStatusToResponse(status).c_str(), eolStatusRegister, registerValue);
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::WriteEOLStatusRegister(const UINT8 &eolStatusRegister, const UINT8 &registerValue)
{   // Write the value to the PCM EEPROM
    SerialString_t moduleResponse;
    SerialArgs_t arguments;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // place the register into the vector
    arguments.push_back(eolStatusRegister);
    // Place the register value into the vector
    arguments.push_back(registerValue);
    // Write the data to the module
    status = CommandModule("WriteEOLStatusRegister", &arguments);
    // Determine the status of the update
    if (BEP_STATUS_SUCCESS == status)
    {   // EEPROM updated successfully
        Log("Updated Process Register -- Register: %02X, Value: %02X\n", eolStatusRegister, registerValue);
    }
    else
    {   // Error updating status byte
        Log("Error updating Process Register -- Status: %s, Process Register: %02X, Value: %02X\n",
            ConvertStatusToResponse(status).c_str(), eolStatusRegister, registerValue);
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadSecuritySeed(UINT16 &seed)
{   // Read the security seed from the module
    return ReadModuleData("ReadSecuritySeed", seed);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadSecuritySeed(UINT32 &seed)
{   // Read the security seed from the module
    return ReadModuleData("ReadSecuritySeed", seed);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CalculateSecurityKey(const UINT16 &seed, SerialArgs_t &key)
{
    Log(LOG_ERRORS, "CalculateSecurityKey() not implemented, please implement before using\n");
    return BEP_STATUS_SOFTWARE;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetBrakeSwitchOnDetected(const bool &switchOnDetected)
{
    m_brakeSwitchOn = switchOnDetected;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetBrakeSwitchOffDetected(const bool &switchOffDetected)
{
    m_brakeSwitchOff = switchOffDetected;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetParkNeutralSwitchOnDetected(const bool &switchOnDetected)
{
    m_parkNeutralSwitchOn = switchOnDetected;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetParkNeutralSwitchOffDetected(const bool &switchOffDetected)
{
    m_parkNeutralSwitchOff = switchOffDetected;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetClutchSwitchOnDetected(const bool &switchOnDetected)
{
    m_clutchSwitchOn = switchOnDetected;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetClutchSwitchOffDetected(const bool &switchOffDetected)
{
    m_clutchSwitchOff = switchOffDetected;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetInitialTPS(const float &initialTPS)
{
    m_initialTPS = initialTPS;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetFinalTPSReading(const float &finalTPS)
{
    m_finalTPSReading = finalTPS;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetObservedDeltaTPS(const float &observedDeltaTPS)
{
    m_observedDeltaTPS = observedDeltaTPS;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetOkToShipRegister(const UINT16 &okToShipRegister)
{
    m_okToShipRegister = okToShipRegister;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetOkToShipRegister(const string &okToShipRegister)
{
    SetOkToShipRegister(atoh(okToShipRegister.c_str()));
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetOKToShipBit(const INT32 &bit)
{
    m_okToShipBit = bit;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetACStatusBit(const INT32 &bit)
{
    m_acStatusBit = bit;
}

template <class ProtocolFilterType>
inline const bool& GenericEmissionsModuleTemplate<ProtocolFilterType>::UnlockIfUpdateFailed(void)
{
    return m_unlockIfUpdateFailed;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetUnlockIfUpdateFailed(const bool &unlock /*= false*/)
{
    m_unlockIfUpdateFailed = unlock;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetRedundantBrakeSwitchEquipped(const bool &isRedundantBrakeEquipped)
{
    m_boolMonitorRedundantBrakeSwitch = isRedundantBrakeEquipped;
}

template <class ProtocolFilterType>
inline const bool& GenericEmissionsModuleTemplate<ProtocolFilterType>::IsRedundantBrakeSwitchEquipped(void)
{
    return m_boolMonitorRedundantBrakeSwitch;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetRedundantBrakeSwitchOnDetected(const bool &switchOnDetected)
{
    m_redundantBrakeSwitchOn = switchOnDetected;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetRedundantBrakeSwitchOffDetected(const bool &switchOffDetected)
{
    m_redundantBrakeSwitchOff = switchOffDetected;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckRedundantBrakeSwitchOn(bool &sawBrakeSwitchOn)
{
    sawBrakeSwitchOn = DetectedRedundantBrakeSwitchOn();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckRedundantBrakeSwitchOff(bool &sawBrakeSwitchOff)
{
    sawBrakeSwitchOff = DetectedRedundantBrakeSwitchOff();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
inline const bool& GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedRedundantBrakeSwitchOn(void)
{
    return m_redundantBrakeSwitchOn;
}

template <class ProtocolFilterType>
inline const bool& GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedRedundantBrakeSwitchOff(void)
{
    return m_redundantBrakeSwitchOff;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetClutchUpstopSwitchEquipped(const bool &isClutchUpstopEquipped)
{
    m_boolMonitorClutchUpstopSwitch = isClutchUpstopEquipped;
}

template <class ProtocolFilterType>
inline bool& GenericEmissionsModuleTemplate<ProtocolFilterType>::IsClutchUpstopSwitchEquipped(void)
{
    return m_boolMonitorClutchUpstopSwitch;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetClutchUpstopSwitchOnDetected(const bool &switchOnDetected)
{
    m_clutchUpstopSwitchOn = switchOnDetected;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetClutchUpstopSwitchOffDetected(const bool &switchOffDetected)
{
    m_clutchUpstopSwitchOff = switchOffDetected;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetClutchOffUpstopOnDetected(const bool &switchOffDetected)
{
    m_clutchOffUpstopOn = switchOffDetected;
}


template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckClutchUpstopSwitchOn(bool &sawClutchSwitchOn)
{
    sawClutchSwitchOn = DetectedClutchUpstopSwitchOn();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckClutchUpstopSwitchOff(bool &sawClutchSwitchOff)
{
    sawClutchSwitchOff = DetectedClutchUpstopSwitchOff();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::CheckClutchOffUpstopOnResult(bool &sawClutchSwitchOff)
{
    sawClutchSwitchOff = DetectedClutchOffUpstopOn();
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
inline const bool& GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedClutchUpstopSwitchOn(void)
{
    return m_clutchUpstopSwitchOn;
}

template <class ProtocolFilterType>
inline const bool& GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedClutchUpstopSwitchOff(void)
{
    return m_clutchUpstopSwitchOff;
}

template <class ProtocolFilterType>
inline const bool& GenericEmissionsModuleTemplate<ProtocolFilterType>::DetectedClutchOffUpstopOn(void)
{
    return m_clutchOffUpstopOn;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadCurrentBrakeSwitchStates(bool &brakeSwitch, bool &redundantBrakeSwitch)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    SerialString_t moduleResponse;
    //set defaults. Fail - set to oppsosite and they should return the same value.
    brakeSwitch = false;
    redundantBrakeSwitch  = true;
    // Read the switches from the module
    status = m_protocolFilter->GetModuleData("ReadSwitches", moduleResponse);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {   // Good read, extract the data
        try
        {
            brakeSwitch = ParseBooleanResponse(GetReplyInterpretationTag("ReadBrakeSwitchOn"), moduleResponse);
            redundantBrakeSwitch = ParseBooleanResponse(GetReplyInterpretationTag("ReadRedundantBrakeSwitchOn"), moduleResponse);
        }
        catch (XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, (ModuleName() + " XmlException in ReadCurrentBrakeSwitchSates: " + err.GetReason()).c_str());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the switches
        Log(LOG_ERRORS, "Error in %s::ReadCurrentBrakeSwitchStates - status: %s\n", ModuleName().c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ReadCurrentClutchSwitchStates(bool &clutchSwitch, bool &clutchUpstopSwitch)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    SerialString_t moduleResponse;
    //set defaults. Fail - both are the same and they should return the oppsoite value.
    clutchSwitch = false;
    clutchUpstopSwitch  = false;
    // Read the switches from the module
    status = m_protocolFilter->GetModuleData("ReadSwitches", moduleResponse);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {   // Good read, extract the data
        if (!IsAutomaticTransmission())
        {
            try
            {
                clutchSwitch = ParseBooleanResponse(GetReplyInterpretationTag("ReadClutchSwitchOn"), moduleResponse);
                clutchUpstopSwitch = ParseBooleanResponse(GetReplyInterpretationTag("ReadClutchUpstopSwitchOn"), moduleResponse);
            }
            catch (XmlException &err)
            {   // Log the reason for the exception
                Log(LOG_ERRORS, (ModuleName() + " XmlException in ReadCurrentClutchSwitchSates: " + err.GetReason()).c_str());
                // Set the status to software error
                status = BEP_STATUS_SOFTWARE;
            }
        }
        else
        {
            status = BEP_STATUS_SOFTWARE;
            Log(LOG_ERRORS, "Error in %s::ReadCurrentClutchSwitchSates reason - Automatic Transmission\n");
        }
    }
    else
    {   // Error reading the switches
        Log(LOG_ERRORS, "Error in %s::ReadCurrentClutchSwitchSates - status: %s\n", ModuleName().c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::IsInFieldMode(bool &inFieldMode)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    UINT16 inFieldModeRegister;
    // Read the OK To Ship register
    status = ReadEOLStatusRegister(GetInFieldModeRegister(), inFieldModeRegister);

    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {   // Good read, determine if vehicle has previously passed rolls testing
        Log(LOG_DEV_DATA, "In Field Mode bit: %d\n", GetInFieldModeBit());
        UINT16 inFieldModeMask = 0x0001 << GetInFieldModeBit();
        Log(LOG_DEV_DATA, "IsInFieldMode: inFieldModeRegister - 0x%04X {Mask: 0x%04X}\n", inFieldModeRegister, inFieldModeMask);
        DataAnalysis analyze;
        inFieldMode = analyze.CompareData(inFieldModeRegister, inFieldModeMask, ALL_BITS_SET);
        Log(LOG_DEV_DATA, "In Field Mode: %s\n", inFieldMode ? "True" : "False");
    }
    else
    {   // Error reading the in field mode register
        Log(LOG_ERRORS, "Error reading In Field Mode Register (0x%04X) - status: %s\n", GetInFieldModeRegister(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline const UINT16 &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInFieldModeRegister()
{
    return m_inFieldModeRegister;
}

template <class ProtocolFilterType>
inline const INT32 &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInFieldModeBit()
{
    return m_inFieldModeBit;
}

template <class ProtocolFilterType>
void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetInFieldModeRegister(const UINT16 inFieldModeRegister)
{
    m_inFieldModeRegister = inFieldModeRegister;
}

template <class ProtocolFilterType>
void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetInFieldModeBit(const INT32 &inFieldModeBit)
{
    m_inFieldModeBit = inFieldModeBit;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::StartQuickLearnEngineAuthentication()
{   // Command the module to begin the quicklearn procedure
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    string engineState("Unknown");
    SerialString_t moduleResponse;
    // 10 second timeout for 'Start Request Pending' reads
    int engineStateReads = 20;

    // Command the module
    moduleStatus = CommandModule("StartEngineAuthentication");
    // Check the status of the command
    if(BEP_STATUS_SUCCESS == moduleStatus)
    {   //read module status
        do
        {
            moduleStatus = ReadModuleData("ReadEngineAuthenticationStatus", engineState);
            Log(LOG_DEV_DATA, "StartAuthentication ReadEngineAuthenticationStatus: Engine State %s", engineState.c_str());
            BposSleep(500);
        } while (moduleStatus == BEP_STATUS_SUCCESS && engineState == GetEngineAuthenticationStartPendingString() && engineStateReads--);
        if(BEP_STATUS_SUCCESS == moduleStatus)
        {   // Determine the running status
            status = (engineState == GetEngineAuthenticationRunningString()) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
        }
        else
        {
            status = BEP_STATUS_FAILURE;
            Log(LOG_ERRORS, "StartQuickLearnEngineAuthentication:ReadEngineAuthenticationSataus Error commanding module: %s", 
                ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {
        status = BEP_STATUS_FAILURE;
        Log(LOG_ERRORS, "StartEngineAuthentication: Error commanding module: %s", ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::StopQuickLearnEngineAuthentication()
{                                      // Command the module to begin the quicklearn procedure
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    string engineState("Unknown");
    SerialString_t moduleResponse;
    // Command the module
    moduleStatus = CommandModule("StopEngineAuthentication");
    // Check the status of the command
    if(BEP_STATUS_SUCCESS == moduleStatus)
    {                                  //read module status
        moduleStatus = ReadModuleData("ReadEngineAuthenticationStatus", engineState);
        if(BEP_STATUS_SUCCESS == moduleStatus)
        {
            status = (engineState == GetEngineAuthenticationStoppedString()) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            Log(LOG_DEV_DATA, "StopAuthentication: ReadEngineAuthenticationStatus: Engine State %s", engineState.c_str());
        }
        else
        {   
            status = BEP_STATUS_FAILURE;
            Log(LOG_ERRORS, "ReadEngineAuthenticationSataus Error commanding module: %s", ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {
        status = BEP_STATUS_FAILURE;
        Log(LOG_ERRORS, "StopEngineAuthentication: Error commanding module: %s", ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericEmissionsModuleTemplate<ProtocolFilterType>::ToggleMdsInhibit(const bool &mdsEnabled, 
                                                                                            const UINT16 mdsRegister,
                                                                                            const UINT16 mdsEnableBit)
{
    EEPROM_TEST_RESULT enableState = mdsEnabled ? EEPROM_FAIL : EEPROM_PASS;
    Log(LOG_DEV_DATA, "Toggling bit to: %s", mdsEnableBit ? "Enable" : "Disable");
    return UpdateEEPROMTestBit(mdsRegister, mdsEnableBit, enableState);
}

// Get the QuickLearn Engine Authentication representations
template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetEngineAuthenticationStoppedString(const string &stoppedString)
{
    m_authenticationStopped = stoppedString;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetEngineAuthenticationRunningString(const string &runningString)
{
    m_authenticationRunning = runningString;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetEngineAuthenticationStopPendingString(const string &pendingString)
{
    m_authenticationStopPending = pendingString;
}

template <class ProtocolFilterType>
inline void GenericEmissionsModuleTemplate<ProtocolFilterType>::SetEngineAuthenticationStartPendingString(const string &pendingString)
{
    m_authenticationStartPending = pendingString;
}

template <class ProtocolFilterType>
inline const string &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetEngineAuthenticationStoppedString(void)
{
    return m_authenticationStopped;
}

template <class ProtocolFilterType>
inline const string &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetEngineAuthenticationRunningString(void)
{
    return m_authenticationRunning;
}
template <class ProtocolFilterType>
inline const string &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetEngineAuthenticationStopPendingString(void)
{
    return m_authenticationStopPending;
}
template <class ProtocolFilterType>
inline const string &GenericEmissionsModuleTemplate<ProtocolFilterType>::GetEngineAuthenticationStartPendingString(void)
{
    return m_authenticationStartPending;
}
