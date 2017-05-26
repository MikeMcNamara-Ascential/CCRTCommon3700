//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//  Module Interface for the Mando ABS.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log$
// 
//*************************************************************************
#include "MandoAbsModule.h"

//-----------------------------------------------------------------------------
template <class ProtocolType>
MandoAbsModule<ProtocolType>::MandoAbsModule() : GenericABSModuleTemplate<ProtocolType>()
{   // Nothing to do
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
MandoAbsModule<ProtocolType>::~MandoAbsModule()
{   // Nothing to do
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
inline string MandoAbsModule<ProtocolType>::ModuleName()
{
    return "Mando ABS Module";
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::ReadFaults(FaultVector_t &faultCodes)
{
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "MandoAbsModule::ReadFaults() - Enter");
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Read th faults from the module
    status = m_protocolFilter->GetModuleData("ReadFaults", response);
    // Check the status of the read
    if(status == BEP_STATUS_SUCCESS)
    {   // Determine if there are any faults present
        if(response[DtcCountIndex()] > 0)
        {   // Faults are present, parse them out
            UINT8 currentDtc = DtcStartIndex();
            char buff[32];
            string reportedDtc;
            for(UINT8 dtcCount = 0; dtcCount < response[DtcCountIndex()]; dtcCount++)
            {
                reportedDtc = CreateMessage(buff, sizeof(buff), "%02X%02X", response[currentDtc], response[currentDtc+1]);
                Log(LOG_DEV_DATA, "DTC %02d: %s", dtcCount+1, reportedDtc.c_str());
                faultCodes.push_back(reportedDtc);
                currentDtc += BytesPerDtc();
            }
        }
        else
        {   // No faults present, clear the fault vector
            Log(LOG_DEV_DATA, "No faults detected in the module");
            faultCodes.clear();
        }
    }
    else
    {   // Error reading faults from the module
        Log(LOG_ERRORS, "Error read faults from %s\n", ModuleName().c_str());
    }
    // Return the status
    Log(LOG_FN_ENTRY, "MandoAbsModule::ReadFaults() - Exit");
    return(status);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::CheckEspEquipped(bool &response) throw(ModuleException)
{
	BEP_STATUS_TYPE status;

    // Log the function entry
	Log(LOG_FN_ENTRY, "Enter MandoAbsModule CheckEspEquipped");

	// Check to see that all our objects are in place
	CheckObjectsStatus();
    
    status = ReadModuleData("ReadEspEquipped", response);

	return( status);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::ReadIoSignals(SerialString_t &response) throw(ModuleException)
{
	BEP_STATUS_TYPE status;

    // Log the function entry
	Log(LOG_FN_ENTRY, "Enter MandoAbsModule ReadIoSignals");

	// Check to see that all our objects are in place
	CheckObjectsStatus();

	status = m_protocolFilter->GetModuleData("ReadIoSignals", response);

	return( status);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::ReadEolTestByte(SerialString_t &response) throw(ModuleException)
{
	BEP_STATUS_TYPE status;

    // Log the function entry
	Log(LOG_FN_ENTRY, "Enter MandoAbsModule ReadEolTestByte");

	// Check to see that all our objects are in place
	CheckObjectsStatus();

	status = m_protocolFilter->GetModuleData("ReadEolTestByte", response);

	return( status);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::WriteEolTestByte(UINT16 &testByte) throw(ModuleException)
{
	BEP_STATUS_TYPE status;

    // Log the function entry
	Log(LOG_FN_ENTRY, "Enter MandoAbsModule WriteEolTestByte");

	// Check to see that all our objects are in place
	CheckObjectsStatus();

    SerialString_t response;
    SerialArgs_t args;

    args.clear();
    args.push_back(testByte);
    status = m_protocolFilter->GetModuleData("WriteEolTestByte", response, &args);

	return( status);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::ReadSensorSpeeds(WheelSpeeds_t &speeds)  throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "MandoAbsModule::ReadSensorSpeeds() - Enter");
    string wheels[] = {"ReadLfSpeed", "ReadRfSpeed", "ReadLrSpeed", "ReadRrSpeed"};
    UINT8 index = 0;
    float wheelSpeed = 0.0;
    do
    {   // Read the current sensor speed from the module
        status = ReadModuleData(wheels[index], wheelSpeed);
        if(BEP_STATUS_SUCCESS == status) 
        {
            Log(LOG_DEV_DATA, "ReadSensorSpeeds - %s: %.2f", wheels[index].c_str(), wheelSpeed);
            speeds[index] = wheelSpeed;
        }
        else
        {
            Log(LOG_ERRORS, "ReadSensorSpeeds - %s: Could not read sensor speed - %s", 
                wheels[index].c_str(), ConvertStatusToResponse(status).c_str());
            speeds[index] = 0.0;
        }
        index++;
    } while((index < 4) && (BEP_STATUS_SUCCESS == status));
    // Log the exit and return the status
    Log(LOG_FN_ENTRY, "MandoAbsModule::ReadSensorSpeeds() - Exit");
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::EspRecovery(const string &valve)  throw(ModuleException)
{   // Command the module to perform reduction on specified wheel
    return CommandModule(valve+"Recovery");
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::EspReduction(const string &corner)  throw(ModuleException)
{   // Command the module to perform reduction on specified wheel
    return CommandModule("EspReduction");
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
bool MandoAbsModule<ProtocolType>::InitializeHook(const XmlNode *configNode)
{
    // We need a keep alive, so initialize the keep alive protocol filter
    if( m_keepAliveComm != NULL)
    {
        Log( LOG_DEV_DATA, "MandoAbsModule::InitializeHook: Initializing the keep alive comm object\n");
        m_keepAliveComm->Initialize( configNode);
    }
    // Load the DTC data
    LoadDtcData(configNode->getChild("Setup/DtcData"));
    // Call the base class to complete the initialization
    return GenericABSModuleTemplate<ProtocolType>::InitializeHook(configNode);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
void MandoAbsModule<ProtocolType>::LoadDtcData(const XmlNode *dtcData)
{
    UINT8 data = 0x00;
    // Store the index to the number of DTCs returned by the module
    try
    {   // Store the index to the number of DTCs returned by the module
        data = BposReadInt(dtcData->getChild("DtcCountIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "DTC Count Index not defined, using 1: %s", excpt.GetReason());
        data = 1;
    }
    DtcCountIndex(&data);
    // Store the number of bytes for each DTC record
    data = 0;
    try
    {
        data = BposReadInt(dtcData->getChild("BytesPerDtc")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Bytes per DTC not defined, using 3: %s", excpt.GetReason());
        data = 3;
    }
    BytesPerDtc(&data);
    // Store the starting index of the first DTC
    data = 0;
    try
    {
        data = BposReadInt(dtcData->getChild("DtcStartIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "DTC starting index not defined, using 2: %s", excpt.GetReason());
        data = 2;
    }
    DtcStartIndex(&data);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
const UINT8& MandoAbsModule<ProtocolType>::DtcCountIndex(const UINT8 *index /*= NULL*/)
{
    if(index != NULL)  m_dtcCountIndex = *index;
    return m_dtcCountIndex;
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
const UINT8& MandoAbsModule<ProtocolType>::BytesPerDtc(const UINT8 *numBytes /*= NULL*/)
{
    if(numBytes != NULL)  m_bytesPerDtc = *numBytes;
    return m_bytesPerDtc;
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
const UINT8& MandoAbsModule<ProtocolType>::DtcStartIndex(const UINT8 *index /*= NULL*/)
{
    if(index != NULL)  m_dtcStartIndex = *index;
    return m_dtcStartIndex;
}
