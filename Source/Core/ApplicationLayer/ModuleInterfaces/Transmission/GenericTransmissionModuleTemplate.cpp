//******************************************************************************
// Description:
//  Module Interface for a generic transmission control module.
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
//     $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Transmission/GenericTransmissionModuleTemplate.cpp $
// 
// 2     1/02/08 9:03a Jsemann
// ProcessTCCLockupState() - compare changed to find - this allows us to
// specify more than one lockup state in the configuration file per module
// response.
// 
// 1     11/01/07 2:00p Derickso
// Updated to use ReadModuleData and CommandModule instead of
// ProtocolFilter::GetModuleData.
// Refactored to use vector form of ReadModuleData for reading clutch
// volumes instead of using module parameters for number of clutch volumes
// adn clutch volume index.
// 
// 3     11/01/07 12:08p Derickso
// Refactored many functions.
// Updated header comments to doxygen format.
// 
// 2     10/01/07 2:48p Derickso
// Added HEV versions of limp-in monitoring.
// 
// 1     8/15/07 12:47p Jsemann
// a.	GetInfo(string methodName, string &value) - added
// ReadRawShiftLeverPosition
// b.	Added GetShiftLeverLow3String()
// c.	InitializeHook() - added call to SetShiftLeverLow3String()
// d.	ProcessCurrentGear() - added ! for sixth gear comparison
// e.	ProcessCurrentShiftLeverPosition() - Low3 condition changed
// GetShiftLeverLowString() to GetShiftLeverLow3String()
// 
// 10    2/22/07 8:44a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Moved the calls to CloseCom() for the additional threads to their
// individual Stop methods.
// 
// - Moved the calls to OpenCom() for the additional threads to their
// Individual Start methods.
// 
// 9     1/17/07 3:03p Rwiersem
// Changes for the 07011901 core release:
// 
// - Delete the gear monitor, limp in fault monitor, tcc lockup monitor,
// gear lock up monitor, and shift lever monitor threads in CloseCom().
// - Added OpenCom().
// 
// 8     10/25/06 3:11p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a last transmit time to the ProtocolFilters
//     - Added OpenCom() and CloseCom()
// 
// 7     9/05/06 3:47p Rwiersem
// Changes for the 06090601 core release:
// 
// - Removed Set methods for engine authentication strings
// - Removed Get methods for engine authentication strings
// - Removed StartQuickLearnEngineAuthentication()
// - Removed StopQuickLearnEngineAuthentication()
// 
// 6     5/08/06 6:47p Cward
// Added methods for shift lever representation, quick learn commands, and
// quick learn authentication response.
//
// 5     11/11/05 4:54p Cward
// Changes for 0511101 iteration
//
// 4     4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 4     4/04/05 1:01a Derickso
// Fixed problem in ReadFaults() where the result of the call to
// GetModuleData was not being stored in the status variable, so
// the evaluation of the if statement was never being evaluated
// regardless of the result.
//
// 3     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 2     9/01/04 8:56p Cward
// Added ifndef sandwich macros. removed inlines.
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
#ifndef GenericTransmissionModuleTemplate_cpp
#define GenericTransmissionModuleTemplate_cpp
//*************************************************************************
#include "GenericTransmissionModuleTemplate.h"


#include "DataAnalysis.h"
#include "INamedDataBroker.h"
#include "IPromptServer.h"
#include "ITestResultServer.h"


template <class ProtocolFilterType>
GenericTransmissionModuleTemplate<ProtocolFilterType>::GenericTransmissionModuleTemplate(void) :
GenericModuleTemplate<ProtocolFilterType>(),
m_gearMonitor(NULL), m_limpInFaultMonitor(NULL), m_tccLockupMonitor(NULL),
m_gearLockUpMonitor(NULL),m_shiftLeverMonitor(NULL),m_ignoredFaults(NULL),
m_sawGear1(false), m_sawGear2(false), m_sawGear3(false), m_sawGear4(false),
m_sawGear5(false), m_sawGear6(false), m_sawGearReverse(false), m_sawGearPark(false),
m_sawGearNeutral(false), m_allGearsObserved(false), m_sawTCCNoLockState(false),
m_sawTCCPartialLockState(false), m_sawTCCFullLockState(false), m_allLockStatesObserved(false),
m_sawShiftLeverDrive(false), m_sawShiftLeverLow(false),m_sawShiftLeverReverse(false), m_sawShiftLeverPark(false),
m_sawShiftLeverNeutral(false), m_allShiftLeverPositionsObserved(false)
{   // Create new comm objects
    m_tccLockupMonitorComm = new ProtocolFilterType(m_lastTxTime, &m_stopCommsFlag, &m_commsInUse);
    m_limpInFaultMonitorComm = new ProtocolFilterType(m_lastTxTime, &m_stopCommsFlag, &m_commsInUse);
    m_gearMonitorComm = new ProtocolFilterType(m_lastTxTime, &m_stopCommsFlag, &m_commsInUse);
    m_gearLockUpMonitorComm = new ProtocolFilterType(m_lastTxTime, &m_stopCommsFlag, &m_commsInUse);
    m_shiftLeverMonitorComm = new ProtocolFilterType(m_lastTxTime, &m_stopCommsFlag, &m_commsInUse);
}

template <class ProtocolFilterType>
GenericTransmissionModuleTemplate<ProtocolFilterType>::~GenericTransmissionModuleTemplate()
{   // Get rid of any of the monitors that did not die
    if (m_gearMonitor != NULL)
    {   // Need to stop the monitor before deleting it
        m_gearMonitor->StopMonitor();
        m_gearMonitor->WaitUntilDone();
        if (m_gearMonitor != NULL)  delete m_gearMonitor;
    }
    m_gearMonitor = NULL;
    if (m_limpInFaultMonitor != NULL)
    {   // Need to stop the monitor before deleting it
        m_limpInFaultMonitor->StopMonitor();
        m_limpInFaultMonitor->WaitUntilDone();
        if (m_limpInFaultMonitor != NULL)  delete m_limpInFaultMonitor;
    }
    m_limpInFaultMonitor = NULL;
    if (m_tccLockupMonitor != NULL)
    {   // Need to stop the monitor before deleting it
        m_tccLockupMonitor->StopMonitor();
        m_tccLockupMonitor->WaitUntilDone();
        if (m_tccLockupMonitor != NULL)  delete m_tccLockupMonitor;
    }
    m_tccLockupMonitor = NULL;
    if (m_gearLockUpMonitor != NULL)
    {   // Need to stop the monitor before deleting it
        m_gearLockUpMonitor->StopMonitor();
        m_gearLockUpMonitor->WaitUntilDone();
        if (m_gearLockUpMonitor != NULL)  delete m_gearLockUpMonitor;
    }
    m_gearLockUpMonitor = NULL;
    // Get rid of the ShiftLever monitor
    if (m_shiftLeverMonitor != NULL)
    {   // Need to stop the monitor before deleting it
        m_shiftLeverMonitor->StopMonitor();
        m_shiftLeverMonitor->WaitUntilDone();
        if (m_shiftLeverMonitor != NULL)    delete m_shiftLeverMonitor;
    }
    m_shiftLeverMonitor = NULL;
    // Delete the comm objects we created, monitors take care of themselves
    if (m_tccLockupMonitorComm != NULL)   delete m_tccLockupMonitorComm;
    m_tccLockupMonitorComm = NULL;
    if (m_limpInFaultMonitorComm != NULL) delete m_limpInFaultMonitorComm;
    m_limpInFaultMonitorComm = NULL;
    if (m_gearMonitorComm != NULL)        delete m_gearMonitorComm;
    m_gearMonitorComm = NULL;
    if (m_gearLockUpMonitorComm != NULL)  delete m_gearLockUpMonitorComm;
    m_gearLockUpMonitorComm = NULL;
    if (m_shiftLeverMonitorComm != NULL)  delete m_shiftLeverMonitorComm;
    m_shiftLeverMonitorComm = NULL;
}

template <class ProtocolFilterType>
void GenericTransmissionModuleTemplate<ProtocolFilterType>::CloseCom()
{
    // Get rid of any of the monitors that did not die
    if (m_gearMonitor != NULL)
    {   // Need to stop the monitor before deleting it
        m_gearMonitor->StopMonitor();
        m_gearMonitor->WaitUntilDone();
        if (m_gearMonitor != NULL)  delete m_gearMonitor;
    }
    m_gearMonitor = NULL;
    if (m_limpInFaultMonitor != NULL)
    {   // Need to stop the monitor before deleting it
        m_limpInFaultMonitor->StopMonitor();
        m_limpInFaultMonitor->WaitUntilDone();
        if (m_limpInFaultMonitor != NULL)  delete m_limpInFaultMonitor;
    }
    m_limpInFaultMonitor = NULL;
    if (m_tccLockupMonitor != NULL)
    {   // Need to stop the monitor before deleting it
        m_tccLockupMonitor->StopMonitor();
        m_tccLockupMonitor->WaitUntilDone();
        if (m_tccLockupMonitor != NULL)  delete m_tccLockupMonitor;
    }
    m_tccLockupMonitor = NULL;
    if (m_gearLockUpMonitor != NULL)
    {   // Need to stop the monitor before deleting it
        m_gearLockUpMonitor->StopMonitor();
        m_gearLockUpMonitor->WaitUntilDone();
        if (m_gearLockUpMonitor != NULL)  delete m_gearLockUpMonitor;
    }
    m_gearLockUpMonitor = NULL;
    // Get rid of the ShiftLever monitor
    if (m_shiftLeverMonitor != NULL)
    {   // Need to stop the monitor before deleting it
        m_shiftLeverMonitor->StopMonitor();
        m_shiftLeverMonitor->WaitUntilDone();
        if (m_shiftLeverMonitor != NULL)    delete m_shiftLeverMonitor;
    }
    m_shiftLeverMonitor = NULL;
/*
    if( NULL != m_gearMonitorComm)          m_gearMonitorComm->CloseCom();
    if( NULL != m_limpInFaultMonitorComm)   m_limpInFaultMonitorComm->CloseCom();
    if( NULL != m_tccLockupMonitorComm)     m_tccLockupMonitorComm->CloseCom();
    if( NULL != m_gearLockUpMonitorComm)    m_gearLockUpMonitorComm->CloseCom();
    if( NULL != m_shiftLeverMonitorComm)    m_shiftLeverMonitorComm->CloseCom();
*/
    GenericModuleTemplate<ProtocolFilterType>::CloseCom();
}

template <class ProtocolFilterType>
bool GenericTransmissionModuleTemplate<ProtocolFilterType>::OpenCom()
{
    bool status = false;
    
    m_sawGear1 = false;
    m_sawGear2 = false;
    m_sawGear3 = false;
    m_sawGear4 = false;
    m_sawGear5 = false;
    m_sawGear6 = false;
    m_sawGearReverse = false;
    m_sawGearPark = false;
    m_sawGearNeutral = false;
    m_allGearsObserved = false;
    m_sawTCCNoLockState = false;
    m_sawTCCPartialLockState = false;
    m_sawTCCFullLockState = false;
    m_allLockStatesObserved = false;
    m_sawShiftLeverDrive = false;
    m_sawShiftLeverLow = false;
    m_sawShiftLeverReverse = false;
    m_sawShiftLeverPark = false;
    m_sawShiftLeverNeutral = false;
    m_allShiftLeverPositionsObserved = false;
/*
    if( NULL != m_gearMonitorComm)          m_gearMonitorComm->OpenCom();
    if( NULL != m_limpInFaultMonitorComm)   m_limpInFaultMonitorComm->OpenCom();
    if( NULL != m_tccLockupMonitorComm)     m_tccLockupMonitorComm->OpenCom();
    if( NULL != m_gearLockUpMonitorComm)    m_gearLockUpMonitorComm->OpenCom();
    if( NULL != m_shiftLeverMonitorComm)    m_shiftLeverMonitorComm->OpenCom();
*/
    status = GenericModuleTemplate<ProtocolFilterType>::OpenCom();

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::GetInfo(string methodName) 
throw(ModuleException)
{   // No special implementation, just call the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, string &value)
throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the current gear from the module
    if (methodName == "ReadCurrentGear")                 status = ReadModuleData(methodName, value);
    // Read the Feature Module year from the module
    else if (methodName == "ReadFeatureModelYear")       status = ReadModuleData(methodName, value);
    // Read the over drive cancel switch status - on/off
    else if (methodName == "ReadOverDriveCancelSwitch")  status = ReadModuleData(methodName, value);
    // Read the Status of the quicklearn procedure
    else if (methodName == "ReadQuickLearnStatus")       status = ReadModuleData(methodName, value);
    // Read the position of the shift lever
    else if (methodName == "ReadShiftLeverPosition")     status = ReadModuleData(methodName, value);
	// Read the raw position of the shift lever
	else if (methodName == "ReadRawShiftLeverPosition")	 status = ReadModuleData(methodName, value);
    // Read the position of the autostick
    else if (methodName == "ReadAutostickPosition")      status = ReadModuleData(methodName, value);
    // Read the software model year of the module
    else if (methodName == "ReadSoftwareModelYear")      status = ReadModuleData(methodName, value);
    // Call the base class to get the information requested
    else  status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value)
throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the LRCC clutch volume from the module
    if (methodName == "ReadClutchVolumeLRCC")       status = ReadModuleData(methodName, value);
    // Read the 2C clutch volume from the module
    else if (methodName == "ReadClutchVolume2C")    status = ReadModuleData(methodName, value);
    // Read the OD clutch volume from the module
    else if (methodName == "ReadClutchVolumeOD")    status = ReadModuleData(methodName, value);
    // Read the UD clutch volume from the module
    else if (methodName == "ReadClutchVolumeUD")    status = ReadModuleData(methodName, value);
    // Read the First ND clutch volume from the module
    else if (methodName == "ReadClutchVolume1ND")   status = ReadModuleData(methodName, value);
    // Read the Normal ND clutch volume from the module
    else if (methodName == "ReadClutchVolumeND")    status = ReadModuleData(methodName, value);
    // Read the first 2-3 shift clutch volume from the module
    else if (methodName == "ReadClutchVolume23")    status = ReadModuleData(methodName, value);
    // Read the alternate 2C clutch volume from the module
    else if (methodName == "ReadClutchVolumeAlt2C") status = ReadModuleData(methodName, value);
    // No special method, try using the base class
    else   status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<UINT8>&value)
throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the first set of clutch volumes from the module
    if (methodName == "ReadClutchVolumes1")       status = ReadClutchVolumes("ReadClutchVolumes1", value);
    // Read the second set of clutch volumes from the module
    else if (methodName == "ReadClutchVolumes2")  status = ReadClutchVolumes("ReadClutchVolumes2", value);
    // Read the EOL status bytes from the module
    else if (methodName == "ReadEOLStatusBytes")  status = ReadEOLStatusBytes(value);
    // No special method, try the base class
    else   status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT16 & value)
throw(ModuleException)
{   // No special method, try the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<UINT16>&value)
throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the EOL status bytes from the module
    if (methodName == "ReadEOLStatusBytes")  status = ReadEOLStatusBytes(value);
    // No special method, try using the base class
    else   status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, int &value)
throw(ModuleException)
{   // No special method, try the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, bool &value)
throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Check if the transmission is in Limp-In mode
    if (methodName == "ReadLimpInStatus")     status = ReadModuleData(methodName, value);
    // Check if the PRNDL status indicates All-On
    else if (methodName == "ReadPRNDLStatus") status = ReadModuleData(methodName, value);
    // Check if shift lever is autostick
    else if (methodName == "ReadAutostickConfig") status = ReadModuleData(methodName, value);
    // Try to use the base class to get the info
    else   status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t &value)
throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the current stored faults
    if (methodName == "ReadFaults") status = ReadFaults(value);
    // Try to use the base class to get the info
    else   status = GenericTransmissionModuleTemplate::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, float &value)
throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the vehicle speed from the transmission control module
    if (methodName == "ReadTransmissionVehicleSpeed")  status = ReadModuleData(methodName, value);
    // Read the trans-axle oil temperature from the transmission
    else if (methodName == "ReadTransaxleOilTemp")     status = ReadModuleData(methodName, value);
    // Try using the base class to get the info
    else   status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::UpdateEEPROMTestBit(
                                                                                          const UINT16 &address,
                                                                                          UINT8 &statusByte,
                                                                                          const UINT8 testBit,
                                                                                          const EEPROM_TEST_RESULT testResult)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    DataAnalysis analyze;
    // Shift the test result to the proper position
    UINT8 eepromBitMask = 1 << testBit;
    // Clear the appropriate bits
    statusByte = analyze.ConvertData(statusByte, eepromBitMask,
                                     (testResult == EEPROM_PASS ? CLEAR_BITS : SET_BITS));
    // Write the new value to the EEPROM
    status = UpdateEOLStatusByte(address, statusByte);
    // Determine if there was an error updating the EEPROM
    if (BEP_STATUS_SUCCESS != status) Log("Error updating EEPROM - status:%s, status byte:%04X, value:%04X\n",
                                         ConvertStatusToResponse(status).c_str(), address, statusByte);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::UpdateEOLStatusByte(
                                                                                          const UINT16 &statusByte,
                                                                                          const UINT8 &value)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Write the data to the module
    status = WriteEOLStatusByte(statusByte, value);
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
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetAllGearsSeen(const bool &allGearsObserved)
{
    m_allGearsObserved = allGearsObserved;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetAllLockStatesSeen(const bool &allLockStatesObserved)
{
    m_allLockStatesObserved = allLockStatesObserved;
}

template <class ProtocolFilterType>
inline string GenericTransmissionModuleTemplate<ProtocolFilterType>::ModuleName()
{
    return "Generic Transmission Control Module";
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::MonitorTransmissionGear(IProtocolFilter *filter)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string currentGear("Unknown");
    DataAnalysis analyze;
    SerialString_t moduleResponse;
    // Check if all gears have been observed
    if (!AllGearsObserved())
    {   // Read the current gear from the module
        status = ReadModuleData("ReadCurrentGear", currentGear, NULL, NULL, filter);
        try
        {
            if ((BposReadInt(m_configNode->getChild("Setup/ReadHevFailureData")->getValue().c_str())) &&
                (BEP_STATUS_SUCCESS == status))
            {   // If we want to see the failure reason, and we are okay thus far...
                CommandModule("ReadFailureRegister", NULL, NULL, filter);
            }
        }
        catch (XmlException &err)
        {
            Log(LOG_ERRORS, "Setup/ReadHevFailureData not found, skipping step\n");
        }
        // Check the status of the read
        if (BEP_STATUS_SUCCESS == status)
        {   // Pick the gear out of the response
            try
            {   // Process the current gear
                ProcessCurrentGear(currentGear);
            }
            catch (XmlException &err)
            {   // Log the reason for the exception
                Log(LOG_ERRORS, "%s XmlException in ReadCurrentGear: %s\n", ModuleName().c_str(), err.GetReason());
                // Set the status to software error
                status = BEP_STATUS_SOFTWARE;
            }
        }
        else
        {   // Error reading current gear from the module
            Log(LOG_ERRORS, "Error reading current gear from the transmission module - status: %s\n",
                ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {   // All gears already observed
        status = BEP_STATUS_SUCCESS;
    }
    // Return the status
    return status;
}
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::MonitorLimpInFaultStatus(IProtocolFilter *filter)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    bool isLimpIn = true;
    bool hasFaults = true;
    // Read the limp-in status from the module
    status = ReadModuleData("ReadLimpInStatus", isLimpIn, NULL, NULL, filter);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {   // Good read, analyze the data
        try
        {   
            Log(LOG_DEV_DATA, "Transmission in Limp-In: %s\n", isLimpIn ? "True" : "False");
            // Determine if the module is in limp-in
            if (isLimpIn)
            {   // Need to abort the test
                INamedDataBroker ndb;
                string response;
                ndb.Write(ABORT_DATA_TAG, "1", response, true);
                // Display the reason for the Abort
                IPromptServer ips;
                ips.DisplayPrompt(1, m_configNode->getChild("Setup/LimpInPrompt")->getValue(), response);
            }
            else
            {   // Not in limp-in, check for faults
                FaultVector_t faultCodes;
                status = ReadFaults(filter, faultCodes);
                // Check the status of the read
                if (BEP_STATUS_SUCCESS == status)
                {   // Good read, evaluate the data
                    hasFaults = !faultCodes.empty();
                    Log(LOG_DEV_DATA, "GenericTransmissionModuleTemplate::MonitorLimpInFaultStatus - Module Faults Present: %s\n",
                        hasFaults ? "True" : "False");
                    // Determine if the module has faults
                    if (hasFaults)
                    {   // Parse the faults into a vector for easy manipulation
                        bool reportFault = false;
						INamedDataBroker ndb;
						string response;
                        ITestResultServer rpt;
						rpt.Initialize(TEST_RESULT_SERVER_NAME, "Client", false);
                        for (INT32 index = 0; ((UINT16)index < faultCodes.size()) && !reportFault; index++)
                        {   // Check if the fault is not ignored
                            string faultTag = "ModuleFault_" + faultCodes[index];
                            if (m_ignoredFaults->find(faultTag) == m_ignoredFaults->end())
                            {
                                reportFault = true;
                                // Get the fault description from the fault file
								if(UseCommonModuleFaultList())
								{
									XmlNode readFaultNode(COMMON_MODULE_FAULT_TAG, faultTag);
									XmlNode faultNode("", "");
									Log(LOG_DEV_DATA, "Reading Common Fault from system: %s", faultTag.c_str());
									INT32 status = ndb.Read(&readFaultNode, response, true);
									if(BEP_STATUS_SUCCESS == status)
									{
										status = ndb.GetNext(faultNode, response);
										XmlNode *modFaultNode = const_cast<XmlNode *>(faultNode.getChildren().getNode(0));
										Log(LOG_DEV_DATA, "Common Fault: %s", modFaultNode->ToString().c_str());
										rpt.ReportDTC(string("Transmission"), modFaultNode);
										Log(LOG_DEV_DATA, "Reported Fault");
									}
									else
									{
										Log(LOG_ERRORS, "Could not read %s,%s: %s", 
											COMMON_MODULE_FAULT_TAG, faultTag.c_str(), ConvertStatusToResponse(status).c_str());
									}
								}
								else
								{
									string faultDescription;
									try
									{
										faultDescription = m_faultList->getNode(faultTag)->getAttributes().getNode("Description")->getValue();
									}
									catch (...)
									{
										char buffer[128];
										faultDescription = CreateMessage(buffer, sizeof(buffer),"%s Description Unavailable", faultTag.c_str());
									}
									try
									{
										rpt.ReportDTC("Transmission", faultTag, faultCodes[index], faultDescription);
									}
									catch (BepException &excpt)
									{
										Log(LOG_ERRORS, "MonitorLimpInFaultStatus:ReportDTC - Error: %s\n", excpt.GetReason());
									}
								}
                            }
                        }
                        // If found a fault that is not ignored, abort all testing
                        if (reportFault)
                        {   
                            // Need to abort the test
                            bool DisableFaultAbort = 0;
                            try
                            {
                                DisableFaultAbort = m_configNode->getChild("Setup/DisableFaultAbort")->getValue() == "Yes";
                            }
                            catch (XmlException &excpt)
                            {
                                Log(LOG_ERRORS, "Not specified to skip abort, using false - %s", excpt.GetReason());
                            }
                            if (DisableFaultAbort)
                            {
                                Log(LOG_DEV_DATA, "Dcx42RLETransmissionModule::MonitorLimpInFaultStatus - Not aborting for faults DisableFaultAbort: %s\n",
                                    DisableFaultAbort ? "True" : "False");
                            }
                            else
                            {
                                ndb.Write(ABORT_DATA_TAG, "1", response, true);
                                // Display the reason for the Abort
                                IPromptServer ips;
                                ips.Initialize(PROMPT_SERVER_NAME);
                                ips.DisplayPrompt(1, m_configNode->getChild("Setup/ModuleFaultsDetectedPrompt")->getValue(), response);
                            }
                        }
                    }
                }
                else
                {   // Error reading the limp-in status
                    Log(LOG_ERRORS, "Error monitoring Fault status from the module - status: %s\n",
                        ConvertStatusToResponse(status).c_str());
                }
            }
        }
        catch (XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s XmlException in MonitorLimpInFaultStatus: %s\n", ModuleName().c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the limp-in status
        Log(LOG_ERRORS, "Error monitoring Limp-In status from the module - status: %s\n",
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::ReadFaults(IProtocolFilter *filter,
                                                                                  FaultVector_t &faultCodes)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    bool hasFaults = true;
    // read number of faults
    Log(LOG_DEV_DATA, "GenericTransmissionModuleTemplate::ReadFaults(filter,vector)\n");
    status = filter->GetModuleData("ReadFaults", moduleResponse);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {   // Good read, evaluate the data
        hasFaults = ParseBooleanResponse(GetReplyInterpretationTag("ReadNumberOfFaults"), moduleResponse);
        // Determine if the module has faults
        if (hasFaults)
        {   // Parse the faults into a vector for easy manipulation
            status = ReadFaults(faultCodes);
        }
        else
        {  // Clear the vector
            faultCodes.clear();
        }
    }
    // Return status of the fault read
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::MonitorTransmissionData(IProtocolFilter *filter)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    string lockupState("Unknown");
    string currentGear("Unknown");
    // Determine if any transmission data should be retrieved
    if (!AllGearsObserved() || !AllLockStatesObserved())
    {   // Read the transmission data from the module
        status = filter->GetModuleData("ReadTransmissionData", moduleResponse);
        if (BEP_STATUS_SUCCESS == status)
        {   // Extract the transmission data
            try
            {   // Extract the data from the reply message
                lockupState = ParseStringResponse(GetReplyInterpretationTag("ReadTorqueConverterStatus"), moduleResponse);
                currentGear = ParseStringResponse(GetReplyInterpretationTag("ReadCurrentGear"), moduleResponse);
                // Process the transmission data
                if (!AllLockStatesObserved())  ProcessTCCLockupState(lockupState);
                if (!AllGearsObserved())       ProcessCurrentGear(currentGear);
            }
            catch (XmlException &err)
            {
                Log(LOG_ERRORS, "XmlException in MonitorTransmissionData - %s\n", err.GetReason());
                status = BEP_STATUS_SOFTWARE;
            }
        }
        else
        {   // Error reading transmission data from the module
            Log(LOG_ERRORS, "Error reading data from the transmission - status: %s\n", ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {   // No need to collect transmission data
        status = BEP_STATUS_SUCCESS;
    }
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::MonitorTorqueConverterLockupStatus(IProtocolFilter *filter)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    string lockupStatus = "Unknown";
    DataAnalysis analyze;
    // Determine if TCC status should be read
    if (!TCCNoLockupObserved() || !TCCPartialLockupObserved() || !TCCFullLockupObserved())
    {   // Read the torque converter clutch lockup status
        status = ReadModuleData("ReadTorqueConverterStatus", lockupStatus, NULL, NULL, filter);
        // Check the status of the read
        if (BEP_STATUS_SUCCESS == status)
        {   // Good read, evaluate the data
            ProcessTCCLockupState(lockupStatus);
        }
        else
        {   // Error reading the Torque converter clutch lockup status
            Log(LOG_ERRORS, "Error monitoring Torque Converter Clutch lockup status from the module - status: %s\n",
                ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {   // Observed all states, just return success
        status = BEP_STATUS_SUCCESS;
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StartQuickLearn()
{   // Command the module to begin the quicklearn procedure
    return CommandModule("StartQuickLearn");
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StopQuickLearn()
{   // Command the module to begin the quicklearn procedure
    return CommandModule("StopQuickLearn");
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::ClearDiagnosticRetention(void)
{   // Command the module to clear diagnostic retention information
    return CommandModule("ClearDiagnosticRetention");
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StartLimpInFaultMonitor(XmlNodeMap *ignoredFaults, XmlNodeMap *faultList)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Save the list of faults to ignore
    m_ignoredFaults = ignoredFaults;
    m_faultList = faultList;
    // Check if the limp-in/fault status monitor exists
    if (m_limpInFaultMonitor == NULL)
    {   // Get the time to delay between gear checks
        string statusMonitorDelay = m_configNode->getChild("Setup/LimpInMonitorDelay")->getValue();
        if( NULL != m_limpInFaultMonitorComm)   m_limpInFaultMonitorComm->OpenCom();
        // Limp-In/Fault monitor does not exist, create a new one
        m_limpInFaultMonitor = new LimpInFaultMonitor<ProtocolFilterType>(this, m_limpInFaultMonitorComm,
                                                                          BposReadInt(statusMonitorDelay.c_str()));
        // Start the thread running
        m_limpInFaultMonitor->Resume(false);
        status = BEP_STATUS_SUCCESS;
    }
    else
    {   // Limp-In/Fault monitor already exists, no need to start
        Log(LOG_DEV_DATA, "Limp-In/Fault Monitor exists, do not need to start\n");
        status = BEP_STATUS_SUCCESS;
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StopLimpInFaultMonitor(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // If the Limp-In/Fault monitor thread exists, delete it
    if (m_limpInFaultMonitor != NULL)
    {   // The thread exists, delete it
        m_limpInFaultMonitor->StopMonitor();
        m_limpInFaultMonitor->WaitUntilDone();
        if (m_limpInFaultMonitor != NULL) delete m_limpInFaultMonitor;
        // Set the poitner to NULL so we can restart later if needed
        m_limpInFaultMonitor = NULL;
        Log(LOG_DEV_DATA, "Limp-In / Fault monitor thread complete, setting pointer to NULL\n");
        if( NULL != m_limpInFaultMonitorComm)   m_limpInFaultMonitorComm->CloseCom();
    }
    Log(LOG_FN_ENTRY, "StopLimpInFaultMonitor() Complete!\n");
    status = BEP_STATUS_SUCCESS;
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StartGearMonitor(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Check if the gear monitor exists
    if (m_gearMonitor == NULL)
    {   // Get the time to delay between gear checks
        string gearMonitorDelay = m_configNode->getChild("Setup/GearMonitorDelay")->getValue();
        if( NULL != m_gearMonitorComm)          m_gearMonitorComm->OpenCom();
        // Gear monitor does not exist, create a new one
        m_gearMonitor = new GearMonitor<ProtocolFilterType>(this, m_gearMonitorComm, BposReadInt(gearMonitorDelay.c_str()));
        // Start the thread running
        m_gearMonitor->Resume(false);
        status = BEP_STATUS_SUCCESS;
    }
    else
    {   // Gear monitor already exists, no need to start
        Log(LOG_DEV_DATA, "Gear Monitor exists, do not need to start\n");
        status = BEP_STATUS_SUCCESS;
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StopGearMonitor(void)
{
    Log(LOG_DEV_DATA, "Entering GenericTransmissionModuleTemplate::StopGearMonitor");
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // If the gear monitor thread exists, delete it
    if (m_gearMonitor != NULL)
    {   // The thread exists, delete it
        m_gearMonitor->StopMonitor();
        m_gearMonitor->WaitUntilDone();
        if (m_gearMonitor != NULL)  delete m_gearMonitor;
        // Set the poitner to NULL so we can restart later if needed
        m_gearMonitor = NULL;
        Log(LOG_DEV_DATA, "GearMonitor thread complete, setting pointer to NULL\n");
        if( NULL != m_gearMonitorComm)          m_gearMonitorComm->CloseCom();
    }
    status = BEP_STATUS_SUCCESS;
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StartLockupMonitor(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Check if the torque converter clutch monitor exists
    if (m_tccLockupMonitor == NULL)
    {   // Get the time to delay between TCC lockup checks
        string tccMonitorDelay = m_configNode->getChild("Setup/TCCMonitorDelay")->getValue();
        if( NULL != m_tccLockupMonitorComm)     m_tccLockupMonitorComm->OpenCom();
        // torque converter clutch monitor does not exist, create a new one
        m_tccLockupMonitor = new TorqueConverterLockupMonitor<ProtocolFilterType>(this, m_tccLockupMonitorComm,
                                                                                  BposReadInt(tccMonitorDelay.c_str()));
        // Start the thread running
        m_tccLockupMonitor->Resume(false);
        status = BEP_STATUS_SUCCESS;
    }
    else
    {   // TCC lockup monitor already exists, no need to start
        Log(LOG_DEV_DATA, "Torque Converter Clutch Monitor exists, do not need to start\n");
        status = BEP_STATUS_SUCCESS;
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StopLockupMonitor(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // If the torque converter clutch monitor thread exists, delete it
    if (m_tccLockupMonitor != NULL)
    {   // The thread exists, delete it
        m_tccLockupMonitor->StopMonitor();
        m_tccLockupMonitor->WaitUntilDone();
        if (m_tccLockupMonitor != NULL)  delete m_tccLockupMonitor;
        // Set the poitner to NULL so we can restart later if needed
        m_tccLockupMonitor = NULL;
        Log(LOG_DEV_DATA, "TCC lockup thread complete, setting pointer to NULL\n");
        if( NULL != m_tccLockupMonitorComm)     m_tccLockupMonitorComm->CloseCom();
    }
    status = BEP_STATUS_SUCCESS;
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StartGearLockUpMonitor(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Only create the monitor one time
    if (m_gearLockUpMonitor == NULL)
    {   // get the time to delay between data reads
        INT32 dataMonitorDelay = BposReadInt(m_configNode->getChild("Setup/GearMonitorDelay")->getValue().c_str());
        if( NULL != m_gearLockUpMonitorComm)    m_gearLockUpMonitorComm->OpenCom();
        // Create a new monitor and start it running
        m_gearLockUpMonitor = new GearLockUpMonitor<ProtocolFilterType>(this, m_gearLockUpMonitorComm, dataMonitorDelay);
        m_gearLockUpMonitor->Resume(false);
        status = BEP_STATUS_SUCCESS;
    }
    else
    {   // Monitor already exists, do not create a new one
        Log(LOG_DEV_DATA, "Transmission Data Monitor already exists, not starting a new one!\n");
        status = BEP_STATUS_SUCCESS;
    }
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StopGearLockUpMonitor(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // If the transmission data monitor exists, delete it
    if (m_gearLockUpMonitor != NULL)
    {
        m_gearLockUpMonitor->StopMonitor();
        m_gearLockUpMonitor->WaitUntilDone();
        if (m_gearLockUpMonitor != NULL)  delete m_gearLockUpMonitor;
        m_gearLockUpMonitor = NULL;
        Log(LOG_DEV_DATA, "Transmission data monitor complete, setting pointer to NULL\n");
        if( NULL != m_gearLockUpMonitorComm)    m_gearLockUpMonitorComm->CloseCom();
    }
    status = BEP_STATUS_SUCCESS;
    return status;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::Gear1Observed(void)
{
    return m_sawGear1;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::Gear2Observed(void)
{
    return m_sawGear2;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::Gear3Observed(void)
{
    return m_sawGear3;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::Gear4Observed(void)
{
    return m_sawGear4;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::Gear5Observed(void)
{
    return m_sawGear5;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::Gear6Observed(void)
{
    return m_sawGear6;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::GearParkObserved(void)
{
    return m_sawGearPark;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::GearReverseObserved(void)
{
    return m_sawGearReverse;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::GearNeutralObserved(void)
{
    return m_sawGearNeutral;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::TCCNoLockupObserved(void)
{
    return m_sawTCCNoLockState;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::TCCPartialLockupObserved(void)
{
    return m_sawTCCPartialLockState;
}

template <class ProtocolFilterType>
inline const bool &GenericTransmissionModuleTemplate<ProtocolFilterType>::TCCFullLockupObserved(void)
{
    return m_sawTCCFullLockState;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverParkString(void)
{
    return m_shiftLeverPark;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverNeutralString(void)
{
    return m_shiftLeverNeutral;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverReverseString(void)
{
    return m_shiftLeverReverse;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverDriveString(void)
{
    return m_shiftLeverDrive;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverUpString(void)
{
    return m_shiftLeverUp;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverDownString(void)
{
    return m_shiftLeverDown;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverOpenString(void)
{
    return m_shiftLeverOpen;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverLowString(void)
{
    return m_shiftLeverLow;
}

template <class ProtocolFilterType>
inline const string& GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverLow3String(void)
{
    return m_shiftLeverLow3;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverFirstString(void)
{
    return m_shiftLeverFirst;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverSecondString(void)
{
    return m_shiftLeverSecond;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverThirdString(void)
{
    return m_shiftLeverThird;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetShiftLeverAutostickString(void)
{
    return m_shiftLeverThird;
}


template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetGear1String(void)
{
    return m_gear1;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetGear2String(void)
{
    return m_gear2;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetGear3String(void)
{
    return m_gear3;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetGear4String(void)
{
    return m_gear4;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetGear5String(void)
{
    return m_gear5;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetGear6String(void)
{
    return m_gear6;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetGearParkString(void)
{
    return m_gearPark;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetGearReverseString(void)
{
    return m_gearReverse;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetGearNeutralString(void)
{
    return m_gearNeutral;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetTCCNoLockString(void)
{
    return m_tccNoLock;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetTCCPartialLockString(void)
{
    return m_tccPartialLock;
}

template <class ProtocolFilterType>
inline const string &GenericTransmissionModuleTemplate<ProtocolFilterType>::GetTCCFullLockString(void)
{
    return m_tccFullLock;
}

template <class ProtocolFilterType>
inline bool& GenericTransmissionModuleTemplate<ProtocolFilterType>::AllGearsObserved(void)
{
    return m_allGearsObserved;
}

template <class ProtocolFilterType>
inline bool& GenericTransmissionModuleTemplate<ProtocolFilterType>::AllLockStatesObserved(void)
{
    return m_allLockStatesObserved;
}

template <class ProtocolFilterType>
bool GenericTransmissionModuleTemplate<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{
    // Log Method Entry
    Log(LOG_FN_ENTRY, "GenericTransmissionModuleTemplate::InitializeHook - Enter\n");

    // Get the shift lever string representations
    try
    {   // Get the shift lever park string
        SetShiftLeverParkString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Park")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever park string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Park: %s\n", err.Reason().c_str());
        SetShiftLeverParkString("Park");
    }
    // Get the shift lever string representations
    try
    {   // Get the shift lever Neutral string
        SetShiftLeverNeutralString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Neutral")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever Neutral string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Neutral: %s\n", err.Reason().c_str());
        SetShiftLeverNeutralString("Neutral");
    }
    // Get the shift lever string representations
    try
    {   // Get the shift lever Reverse string
        SetShiftLeverReverseString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Reverse")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever Reverse string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Reverse: %s\n", err.Reason().c_str());
        SetShiftLeverReverseString("Reverse");
    }
    // Get the shift lever string representations
    try
    {   // Get the shift lever Drive string
        SetShiftLeverDriveString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Drive")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever Drive string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Drive: %s\n", err.Reason().c_str());
        SetShiftLeverDriveString("Drive");
    }
    // Get the shift lever string representations
    try
    {   // Get the shift lever Low 3 string
        SetShiftLeverLow3String(configNode->getChild("Setup/ShiftLeverStringRepresentation/Low3")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever Low string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Low3: %s\n", err.Reason().c_str());
        SetShiftLeverLow3String("Low3");
    }
	// Get the shift lever string representations
	try
    {   // Get the shift lever Up string
        SetShiftLeverUpString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Up")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever Up string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Up: %s\n", err.Reason().c_str());
        SetShiftLeverUpString("Up");
    }
    // Get the shift lever string representations
    try
    {   // Get the shift lever Down string
        SetShiftLeverDownString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Down")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever Down string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Down: %s\n", err.Reason().c_str());
        SetShiftLeverDownString("Down");
    }
    // Get the shift lever string representations
    try
    {   // Get the shift lever Open string
        SetShiftLeverOpenString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Open")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever Open string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Open: %s\n", err.Reason().c_str());
        SetShiftLeverOpenString("Open");
    }

    try
    {   // Get the shift lever low string
        SetShiftLeverLowString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Low")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever Up string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Low: %s\n", err.Reason().c_str());
        SetShiftLeverLowString("Low");
    }
    try
    {   // Get the shift lever 1st string
        SetShiftLeverFirstString(configNode->getChild("Setup/ShiftLeverStringRepresentation/First")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever 1st string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/First: %s\n", err.Reason().c_str());
        SetShiftLeverFirstString("First");
    }
    try
    {   // Get the shift lever 2nd string
        SetShiftLeverSecondString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Second")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift 2nd Third string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Secound: %s\n", err.Reason().c_str());
        SetShiftLeverSecondString("Second");
    }
    try
    {   // Get the shift lever Third string
        SetShiftLeverThirdString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Third")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever Third string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Third: %s\n", err.Reason().c_str());
        SetShiftLeverThirdString("Third");
    }
    try
    {   // Get the shift lever Autostick string
        SetShiftLeverAutostickString(configNode->getChild("Setup/ShiftLeverStringRepresentation/Autostick")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting shift lever Autostick string
        Log(LOG_ERRORS, "Error getting Setup/ShiftLeverStringRepresentation/Autostick: %s\n", err.Reason().c_str());
        SetShiftLeverAutostickString("Autostick");
    }

    // Get the gear string representations
    try
    {   // Get the first gear string
        SetGear1String(configNode->getChild("Setup/GearStringRepresentation/First")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting first gear string
        Log(LOG_ERRORS, "Error getting Setup/GearStringRepresentation/First: %s\n", err.Reason().c_str());
        SetGear1String("First");
    }
    try
    {   // Get the second gear string
        SetGear2String(configNode->getChild("Setup/GearStringRepresentation/Second")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting second gear string
        Log(LOG_ERRORS, "Error getting Setup/GearStringRepresentation/Second: %s\n", err.Reason().c_str());
        SetGear2String("Second");
    }
    try
    {   // Get the third gear string
        SetGear3String(configNode->getChild("Setup/GearStringRepresentation/Third")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting third gear string
        Log(LOG_ERRORS, "Error getting Setup/GearStringRepresentation/Third: %s\n", err.Reason().c_str());
        SetGear3String("Third");
    }
    try
    {   // Get the fourth gear string
        SetGear4String(configNode->getChild("Setup/GearStringRepresentation/Fourth")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting fourth gear string
        Log(LOG_ERRORS, "Error getting Setup/GearStringRepresentation/Fourth: %s\n", err.Reason().c_str());
        SetGear4String("Fourth");
    }
    try
    {   // Get the fifth gear string
        SetGear5String(configNode->getChild("Setup/GearStringRepresentation/Fifth")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting Fifth gear string
        Log(LOG_ERRORS, "Error getting Setup/GearStringRepresentation/Fifth: %s\n", err.Reason().c_str());
        SetGear5String("Fifth");
    }
    try
    {   // Get the sixth gear string
        SetGear6String(configNode->getChild("Setup/GearStringRepresentation/Sixth")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting sixth gear string
        Log(LOG_ERRORS, "Error getting Setup/GearStringRepresentation/Sixth: %s\n", err.Reason().c_str());
        SetGear6String("Sixth");
    }
    try
    {   // Get the park gear string
        SetGearParkString(configNode->getChild("Setup/GearStringRepresentation/Park")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting park gear string
        Log(LOG_ERRORS, "Error getting Setup/GearStringRepresentation/Park: %s\n", err.Reason().c_str());
        SetGearParkString("Park");
    }
    try
    {   // Get the reverse gear string
        SetGearReverseString(configNode->getChild("Setup/GearStringRepresentation/Reverse")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting reverse gear string
        Log(LOG_ERRORS, "Error getting Setup/GearStringRepresentation/Reverse: %s\n", err.Reason().c_str());
        SetGearReverseString("Reverse");
    }
    try
    {   // Get the Neutral gear string
        SetGearNeutralString(configNode->getChild("Setup/GearStringRepresentation/Neutral")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting neutral gear string
        Log(LOG_ERRORS, "Error getting Setup/GearStringRepresentation/Neutral: %s\n", err.Reason().c_str());
        SetGearNeutralString("Neutral");
    }
    try
    {   // Get the TCC No-Lock string
        SetTccNoLockString(configNode->getChild("Setup/TCCLockStateStringRepresentation/NoLock")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting TCC No-Lock string
        Log(LOG_ERRORS, "Error getting Setup/TCCLockStateStringRepresentation/NoLock: %s\n", err.Reason().c_str());
        SetTccNoLockString("NoLock");
    }
    try
    {   // Get the TCC Partial-Lock string
        SetTccPartialLockString(configNode->getChild("Setup/TCCLockStateStringRepresentation/PartialLock")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting TCC Partial-Lock string
        Log(LOG_ERRORS, "Error getting Setup/TCCLockStateStringRepresentation/PartialLock: %s\n", err.Reason().c_str());
        SetTccPartialLockString("PartialLock");
    }
    try
    {   // Get the TCC Full-Lock string
        SetTccFullLockString(configNode->getChild("Setup/TCCLockStateStringRepresentation/FullLock")->getValue());
    }
    catch (XmlException &err)
    {   // Error getting TCC No-Lock string
        Log(LOG_ERRORS, "Error getting Setup/TCCLockStateStringRepresentation/FullLock: %s\n", err.Reason().c_str());
        SetTccFullLockString("FullLock");
    }
	try
	{	// Do not check TECHM Limp-In for HEV-HB, check the HCP for Limp-In instead.
		SetCheckTechmLimpIn(configNode->getChild("Setup/CheckTechmLimpIn")->getValue() == "Yes");
	}
	catch (XmlException &err)
	{	// Error getting LimpIn boolean parameter.
		Log(LOG_ERRORS, "Error getting Setup/CheckTechmLimpIn: %s - default to true\n", err.Reason().c_str());
		SetCheckTechmLimpIn(true);
	}
	// Determine if a common module fault list is being used
	bool commonList = false;
	try
	{
		commonList = atob(configNode->getChild("Setup/UseCommonModuleFaultList")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not get common module fault list flag, not using common fault list");
		commonList = false;
	}
	UseCommonModuleFaultList(&commonList);
    // Initialize all comm objects here
    m_keepAliveComm->Initialize(configNode);
    m_gearMonitorComm->Initialize(configNode);
    m_limpInFaultMonitorComm->Initialize(configNode);
    m_tccLockupMonitorComm->Initialize(configNode);
    m_gearLockUpMonitorComm->Initialize(configNode);
    m_shiftLeverMonitorComm->Initialize(configNode);
    // Log Method Exit
    Log(LOG_FN_ENTRY, "GenericTransmissionModuleTemplate::InitializeHook - Exit\n");
    // Call the base class to finish off the initialization
    return GenericModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
}

template <class ProtocolFilterType>
void GenericTransmissionModuleTemplate<ProtocolFilterType>::ProcessTCCLockupState(const string &lockState)
{
    Log(LOG_DEV_DATA, "Torque Converter Clutch Status: %s\n", lockState.c_str());
    //  Check for no lock state
    if (!TCCNoLockupObserved())
    {   // No-Lock state was not observed, check if it just was
        SetObservedTCCNoLockState((lockState.find(GetTCCNoLockString()) != lockState.npos));
        if (TCCNoLockupObserved()) Log(LOG_DEV_DATA, "Observed Torque Converter Clutch in No-Lock state\n");
    }
    //  Check for partial lock state
    if (!TCCPartialLockupObserved())
    {   // Partial-Lock state was not observed, check if it just was
        SetObservedTCCPartialLockState((lockState.find(GetTCCPartialLockString()) != lockState.npos));
        if (TCCPartialLockupObserved()) Log(LOG_DEV_DATA, "Observed Torque Converter Clutch in Partial-Lock state\n");
    }
    //  Check for full lock state
    if (!TCCFullLockupObserved())
    {   // Full-Lock state was not observed, check if it just was
        SetObservedTCCFullLockState((lockState.find(GetTCCFullLockString()) != lockState.npos));
        if (TCCFullLockupObserved()) Log(LOG_DEV_DATA, "Observed Torque Converter Clutch in Full-Lock state\n");
    }
}

template <class ProtocolFilterType>
void GenericTransmissionModuleTemplate<ProtocolFilterType>::ProcessCurrentGear(const string &currentGear)
{
    Log(LOG_DEV_DATA, "Current Gear: %s\n", currentGear.c_str());
    // Check for first gear
    if (!Gear1Observed())
    {   // First gear was not observed, check if it just was
        SetObservedGear1(!currentGear.compare(GetGear1String()));
        if (Gear1Observed()) Log(LOG_DEV_DATA, "Saw First Gear\n");
    }
    // Check for second gear
    if (!Gear2Observed())
    {   // Second gear was not observed, check if it just was
        SetObservedGear2(!currentGear.compare(GetGear2String()));
        if (Gear2Observed()) Log(LOG_DEV_DATA, "Saw Second Gear\n");
    }
    // Check for third gear
    if (!Gear3Observed())
    {   // Third gear was not observed, check if it just was
        SetObservedGear3(!currentGear.compare(GetGear3String()));
        if (Gear3Observed()) Log(LOG_DEV_DATA, "Saw Third Gear\n");
    }
    // Check for fourth gear
    if (!Gear4Observed())
    {   // Fourth gear was not observed, check if it just was
        SetObservedGear4(!currentGear.compare(GetGear4String()));
        if (Gear4Observed()) Log(LOG_DEV_DATA, "Saw Fourth Gear\n");
    }
    // Check for fifth gear
    if (!Gear5Observed())
    {   // Fifth gear was not observed, check if it just was
        SetObservedGear5(!currentGear.compare(GetGear5String()));
        if (Gear5Observed()) Log(LOG_DEV_DATA, "Saw Fifth Gear\n");
    }
    // Check for sixth gear
    if (!Gear6Observed())
    {   // Sixth gear was not observed, check if it just was
        SetObservedGear6(!currentGear.compare(GetGear6String()));
        if (Gear6Observed()) Log(LOG_DEV_DATA, "Saw Sixth Gear\n");
    }
//  // Check for Park gear
//  if (!GearParkObserved())
//  {   // Park gear was not observed, check if it just was
//      SetObservedGearPark(!currentGear.compare(GetGearParkString()));
//      if (GearParkObserved()) Log(LOG_DEV_DATA, "Saw Park Gear\n");
//  }
//  // Check for reverse gear
//  if (!GearReverseObserved())
//  {   // Reverse gear was not observed, check if it just was
//      SetObservedGearReverse(!currentGear.compare(GetGearReverseString()));
//      if (GearReverseObserved()) Log(LOG_DEV_DATA, "Saw Reverse Gear\n");
//  }
//  // Check for neutral gear
//  if (!GearNeutralObserved())
//  {   // Neutral gear was not observed, check if it just was
//      SetObservedGearNeutral(!currentGear.compare(GetGearNeutralString()));
//      if (GearNeutralObserved()) Log(LOG_DEV_DATA, "Saw Neutral Gear\n");
//  }
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StartShiftLeverMonitor(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if (m_shiftLeverMonitor == NULL)
    {   // Get the time to delay between shift lever checks
        string shiftLeverMonitorDelay = m_configNode->getChild("Setup/ShiftLeverMonitorDelay")->getValue();
        if( NULL != m_shiftLeverMonitorComm)    m_shiftLeverMonitorComm->OpenCom();
        // ShiftLever monitor does not exist, create a new one
        m_shiftLeverMonitor = new ShiftLeverMonitor<ProtocolFilterType>(this, m_shiftLeverMonitorComm, BposReadInt(shiftLeverMonitorDelay.c_str()));
        // Start the thread running
        m_shiftLeverMonitor->Resume(false);
        status = BEP_STATUS_SUCCESS;
    }
    else
    {   // ShiftLever monitor already exists, no need to start
        Log(LOG_DEV_DATA, "ShiftLever Monitor exists, do not need to start\n");
        status = BEP_STATUS_SUCCESS;
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::MonitorTransmissionShiftLever(IProtocolFilter *filter)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string currentShiftLeverPosition = "Unknown";
    DataAnalysis analyze;
    SerialString_t moduleResponse;
    // Check if all shift lever positions have been observed
    if (!AllShiftLeverPositionsObserved())
    {   // Read the current shift lever position from the module
        status = ReadModuleData("ReadShiftLeverPosition", currentShiftLeverPosition, NULL, NULL, filter);
        // Check the status of the read
        if (BEP_STATUS_SUCCESS == status)
        {   // Process the shift lever position
                ProcessCurrentShiftLeverPosition(currentShiftLeverPosition);
        }
        else
        {   // Error reading current shift lever position from the module
            Log(LOG_ERRORS, "Error reading current shift lever position from the transmission module - status: %s\n",
                ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {   // All shift lever positions already observed
        status = BEP_STATUS_SUCCESS;
    }
    // Return the status
    return status;
}


template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::StopShiftLeverMonitor(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // If the ShiftLever monitor thread exists, delete it
    if (m_shiftLeverMonitor != NULL)
    {   // The thread exists, delete it
        m_shiftLeverMonitor->StopMonitor();
        m_shiftLeverMonitor->WaitUntilDone();
        if (m_shiftLeverMonitor != NULL)    delete m_shiftLeverMonitor;
        // Set the poitner to NULL so we can restart later if needed
        m_shiftLeverMonitor = NULL;
        Log(LOG_DEV_DATA, "ShiftLeverMonitor thread complete, setting pointer to NULL\n");
        if( NULL != m_shiftLeverMonitorComm)    m_shiftLeverMonitorComm->CloseCom();
    }
    status = BEP_STATUS_SUCCESS;
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline bool& GenericTransmissionModuleTemplate<ProtocolFilterType>::AllShiftLeverPositionsObserved(void)
{
    return m_allShiftLeverPositionsObserved;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetAllShiftLeverPositionsSeen(const bool &allShiftLeverPositionsObserved)
{
    m_allShiftLeverPositionsObserved = allShiftLeverPositionsObserved;
}

template <class ProtocolFilterType>
inline const bool& GenericTransmissionModuleTemplate<ProtocolFilterType>::ShiftLeverDriveObserved(void)
{
    return m_sawShiftLeverDrive;
}

template <class ProtocolFilterType>
inline const bool& GenericTransmissionModuleTemplate<ProtocolFilterType>::ShiftLeverLowObserved(void)
{
    return m_sawShiftLeverLow;
}

template <class ProtocolFilterType>
inline const bool& GenericTransmissionModuleTemplate<ProtocolFilterType>::ShiftLeverLow3Observed(void)
{
    return m_sawShiftLeverLow3;
}

template <class ProtocolFilterType>
inline const bool& GenericTransmissionModuleTemplate<ProtocolFilterType>::ShiftLeverParkObserved(void)
{
    return m_sawShiftLeverPark;
}

template <class ProtocolFilterType>
inline const bool& GenericTransmissionModuleTemplate<ProtocolFilterType>::ShiftLeverReverseObserved(void)
{
    return m_sawShiftLeverReverse;
}

template <class ProtocolFilterType>
inline const bool& GenericTransmissionModuleTemplate<ProtocolFilterType>::ShiftLeverNeutralObserved(void)
{
    return m_sawShiftLeverNeutral;
}

template <class ProtocolFilterType>
void GenericTransmissionModuleTemplate<ProtocolFilterType>::ProcessCurrentShiftLeverPosition(const string &currentShiftLeverPosition)
{
    Log(LOG_DEV_DATA, "Current Shift Lever Position: %s\n", currentShiftLeverPosition.c_str());
    // Check for drive shift lever position and display
    if (!ShiftLeverDriveObserved())
    {   // drive ShiftLever was not observed, check if it just was
        SetObservedShiftLeverDrive(!currentShiftLeverPosition.compare(GetShiftLeverDriveString()));
        if (ShiftLeverDriveObserved()) Log(LOG_DEV_DATA, "Saw Drive Shift Lever Position\n");
    }
    // Check for Low ShiftLever
    if (!ShiftLeverLowObserved())
    {   // Low ShiftLever was not observed, check if it just was
        SetObservedShiftLeverLow(!currentShiftLeverPosition.compare(GetShiftLeverLowString()));
        if (ShiftLeverLowObserved()) Log(LOG_DEV_DATA, "Saw Low ShiftLever\n");
    }
    // Check for Low 3 ShiftLever
    if (!ShiftLeverLow3Observed())
    {   // Low 3 ShiftLever was not observed, check if it just was
        SetObservedShiftLeverLow3(!currentShiftLeverPosition.compare(GetShiftLeverLow3String()));
        if (ShiftLeverLow3Observed()) Log(LOG_DEV_DATA, "Saw Low 3 ShiftLever\n");
    }
    // Check for Park ShiftLever
    if (!ShiftLeverParkObserved())
    {   // Park ShiftLever was not observed, check if it just was
        SetObservedShiftLeverPark(!currentShiftLeverPosition.compare(GetShiftLeverParkString()));
        if (ShiftLeverParkObserved())   Log(LOG_DEV_DATA, "Saw Park ShiftLever\n");
    }
    // Check for reverse ShiftLever
    if (!ShiftLeverReverseObserved())
    {   // Reverse ShiftLever was not observed, check if it just was
        SetObservedShiftLeverReverse(!currentShiftLeverPosition.compare(GetShiftLeverReverseString()));
        if (ShiftLeverReverseObserved()) Log(LOG_DEV_DATA, "Saw Reverse ShiftLever\n");
    }
    // Check for neutral ShiftLever
    if (!ShiftLeverNeutralObserved())
    {   // Neutral ShiftLever was not observed, check if it just was
        SetObservedShiftLeverNeutral(!currentShiftLeverPosition.compare(GetShiftLeverNeutralString()));
        if (ShiftLeverNeutralObserved()) Log(LOG_DEV_DATA, "Saw Neutral ShiftLever\n");
    }
}


template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::WriteEOLStatusByte(const UINT16 &eolStatusRegister,
                                                                                                 const UINT8 &registerValue)
{
    Log(LOG_ERRORS, "No generic implementation available.\n");
    return BEP_STATUS_SOFTWARE;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::ReadEOLStatusBytes(vector<UINT8> &statusBytes)
{
    Log(LOG_ERRORS, "No generic implementation available.\n");
    return BEP_STATUS_SOFTWARE;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::ReadEOLStatusBytes(vector<UINT16> &statusBytes)
{
    Log(LOG_ERRORS, "No generic implementation available.\n");
    return BEP_STATUS_SOFTWARE;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes)
{
    Log(LOG_ERRORS, "ReadFaults() not implemented, please implement this method before using\n");
    return BEP_STATUS_SOFTWARE;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::ReadClutchVolumes(const string clutchVolumeMessage,
                                                                                         vector<UINT8> &clutchVolumes)
{   // Read the clutch volumes from the transmission
    clutchVolumes.clear();
    BEP_STATUS_TYPE status = ReadModuleData(clutchVolumeMessage, clutchVolumes);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {   // Good read, log the data
        for (UINT16 index = 0; index < clutchVolumes.size(); index++)
        {   // Log the clutch volume 
            Log("Clutch volume %d: %02X", index+1, clutchVolumes[index]);
        }
    }
    else
    {   // Error reading clutch volems from the module
        Log(LOG_ERRORS, "Error reading clutch volumes from the transmission - Message: %s, status: %s\n",
            clutchVolumeMessage.c_str(), ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedGear1(const bool &observedGear1)
{
    m_sawGear1 = observedGear1;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedGear2(const bool &observedGear2)
{
    m_sawGear2 = observedGear2;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedGear3(const bool &observedGear3)
{
    m_sawGear3 = observedGear3;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedGear4(const bool &observedGear4)
{
    m_sawGear4 = observedGear4;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedGear5(const bool &observedGear5)
{
    m_sawGear5 = observedGear5;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedGear6(const bool &observedGear6)
{
    m_sawGear6 = observedGear6;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedGearPark(const bool &observedGearPark)
{
    m_sawGearPark = observedGearPark;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedGearReverse(const bool &observedGearReverse)
{
    m_sawGearReverse = observedGearReverse;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedGearNeutral(const bool &observedGearNeutral)
{
    m_sawGearNeutral = observedGearNeutral;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedTCCNoLockState(const bool &observedTCCNoLock)
{
    m_sawTCCNoLockState = observedTCCNoLock;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedTCCPartialLockState(const bool &observedTCCPartialLock)
{
    m_sawTCCPartialLockState = observedTCCPartialLock;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedTCCFullLockState(const bool &observedTCCFullLock)
{
    m_sawTCCFullLockState = observedTCCFullLock;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedShiftLeverDrive(const bool &observedShiftLeverDrive)
{
    m_sawShiftLeverDrive = observedShiftLeverDrive;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedShiftLeverLow(const bool &observedShiftLeverLow)
{
    m_sawShiftLeverLow = observedShiftLeverLow;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedShiftLeverLow3(const bool &observedShiftLeverLow3)
{
    m_sawShiftLeverLow3 = observedShiftLeverLow3;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedShiftLeverPark(const bool &observedShiftLeverPark)
{
    m_sawShiftLeverPark = observedShiftLeverPark;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedShiftLeverReverse(const bool &observedShiftLeverReverse)
{
    m_sawShiftLeverReverse = observedShiftLeverReverse;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetObservedShiftLeverNeutral(const bool &observedShiftLeverNeutral)
{
    m_sawShiftLeverNeutral = observedShiftLeverNeutral;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverParkString(const string &leverString)
{
    m_shiftLeverPark = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverNeutralString(const string &leverString)
{
    m_shiftLeverNeutral = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverReverseString(const string &leverString)
{
    m_shiftLeverReverse = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverDriveString(const string &leverString)
{
    m_shiftLeverDrive = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverUpString(const string &leverString)
{
    m_shiftLeverUp = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverDownString(const string &leverString)
{
    m_shiftLeverDown = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverOpenString(const string &leverString)
{
    m_shiftLeverOpen = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverLowString(const string &leverString)
{
    m_shiftLeverLow = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverLow3String(const string &leverString)
{
    m_shiftLeverLow3 = leverString;

}
template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverFirstString(const string &leverString)
{
    m_shiftLeverFirst = leverString;
}
template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverSecondString(const string &leverString)
{
    m_shiftLeverSecond = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverThirdString(const string &leverString)
{
    m_shiftLeverThird = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetShiftLeverAutostickString(const string &leverString)
{
    m_shiftLeverAutostick = leverString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetGear1String(const string &gearString)
{
    m_gear1 = gearString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetGear2String(const string &gearString)
{
    m_gear2 = gearString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetGear3String(const string &gearString)
{
    m_gear3 = gearString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetGear4String(const string &gearString)
{
    m_gear4 = gearString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetGear5String(const string &gearString)
{
    m_gear5 = gearString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetGear6String(const string &gearString)
{
    m_gear6 = gearString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetGearParkString(const string &gearString)
{
    m_gearPark = gearString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetGearReverseString(const string &gearString)
{
    m_gearReverse = gearString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetGearNeutralString(const string &gearString)
{
    m_gearNeutral = gearString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetTccNoLockString(const string &noLockString)
{
    m_tccNoLock = noLockString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetTccPartialLockString(const string &partialLockString)
{
    m_tccPartialLock = partialLockString;
}

template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetTccFullLockString(const string &fullLockString)
{
    m_tccFullLock = fullLockString;
}
template <class ProtocolFilterType>
inline void GenericTransmissionModuleTemplate<ProtocolFilterType>::SetCheckTechmLimpIn(bool check)
{
	m_checkTechmLimpIn = check;
}
template <class ProtocolFilterType>
inline bool GenericTransmissionModuleTemplate<ProtocolFilterType>::GetCheckTechmLimpIn(void)
{
	return m_checkTechmLimpIn;
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType>
bool GenericTransmissionModuleTemplate<ProtocolFilterType>::UseCommonModuleFaultList(const bool *uselist /*= NULL*/)
{
	if(uselist != NULL)  m_useCommonFaultList = *uselist;
	return m_useCommonFaultList;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericTransmissionModuleTemplate<ProtocolFilterType>::MonitorATLearnData(float &accelPedalPos, string &converterStatus, string &currentGear)
{
    BEP_STATUS_TYPE status, statusPedal, statusConverter, finalStatus = BEP_STATUS_ERROR;
    SerialString_t moduleResponsePedalPos, moduleResponseCurrentGear, moduleResponseExtra;

    //Read the important items from TCU module
    status = m_protocolFilter->GetModuleData("CurrentGear", moduleResponseCurrentGear);
    statusPedal = m_protocolFilter->GetModuleData("AcceleratorPedalPosition", moduleResponsePedalPos);
    statusConverter = m_protocolFilter->GetModuleData("ConverterStatus", moduleResponseExtra);

    // Check the status of the read
    if ( (BEP_STATUS_SUCCESS == status) && (BEP_STATUS_SUCCESS == statusPedal) && (BEP_STATUS_SUCCESS == statusConverter) )
    {   
        // Good read, evaluate the data
        try
        {   
            // Interpret the data
            converterStatus = ParseStringResponse("ConverterStatus", moduleResponseExtra); 
            accelPedalPos = ParseFloatResponse("AcceleratorPedalPosition", moduleResponsePedalPos);
            currentGear = ParseStringResponse("CurrentGear", moduleResponseCurrentGear);

            Log(LOG_DEV_DATA, "AT learn Module Values: \n \t converterStatus: %s \n \t accelPedalPos: %f\n \t currentGear: %s \n \t",
                 converterStatus.c_str(), accelPedalPos, currentGear.c_str());

            finalStatus = BEP_STATUS_SUCCESS;
        }
        catch (XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s XmlException in MonitorATLearnData: %s\n",
                ModuleName().c_str(), err.GetReason());
            // Set the status to software error
            finalStatus = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading AT learn items
        Log(LOG_ERRORS, "Error monitoring AT learn data from the module - status: %s\n",
            ConvertStatusToResponse(status).c_str());
    }

    return finalStatus;
}

#endif //GenericTransmissionModuleTemplate_cpp
