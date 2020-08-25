//*************************************************************************
// FILE DESCRIPTION:
//	Module Interface for the Isuzu engine controller.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "IsuzuDeltaEngineControlModule.h"

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
IsuzuDeltaEngineControlModule<ProtocolFilterType>::IsuzuDeltaEngineControlModule() :
GenericEmissionsModuleTemplate<ProtocolFilterType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
IsuzuDeltaEngineControlModule<ProtocolFilterType>::~IsuzuDeltaEngineControlModule()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
bool IsuzuDeltaEngineControlModule<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{   // Call the base class to begin the initialization
    bool status = GenericEmissionsModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
    // Store the byte containing the number of DTCs reported in the module response
    Log(LOG_DEV_DATA, "IsuzuDeltaEngineControlModule::InitializeHook: Getting Dtc Info from Config");
    UINT8 byte = 0x00;
    try
    {
        byte = BposReadInt(configNode->getChild("Setup/DtcData/DtcCountIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        byte = 0;
        Log(LOG_ERRORS, "DTC Count index not defined, using 0");
    }
    Log(LOG_DEV_DATA, "Setting DTC Count Index to %d", byte);
    DtcCountIndex(&byte);
    // Store the index of the first byte of the first dtc
    try
    {
        byte = BposReadInt(configNode->getChild("Setup/DtcData/DtcStartByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        byte = 1;
        Log(LOG_ERRORS, "DTC start index not defined, using 0");
    }
    Log(LOG_DEV_DATA, "Setting DTC Start Index to %d", byte);
    FirstDtcByteIndex(&byte);
    // Store the number of bytes per DTC
    try
    {
        byte = BposReadInt(configNode->getChild("Setup/DtcData/BytesPerDtc")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        byte = 4;
        Log(LOG_ERRORS, "Bytes per DTC not specified, using 4");
    }
    Log(LOG_DEV_DATA, "Setting bytes per DTC to %d", byte);
    BytesPerDtc(&byte);
    try
    {
        byte = BposReadInt(configNode->getChild("Setup/DtcData/DtcMessageSize")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        byte = DTC_MESSAGE_SIZE;
        Log(LOG_ERRORS, "DTC Message size not defined, using %d", DTC_MESSAGE_SIZE);
    }
    Log(LOG_DEV_DATA, "Setting DTC Message Size to %d", byte);
    m_dtcMessageSize = byte;
    Log(LOG_DEV_DATA, "IsuzuDeltaEngineControlModule::InitializeHook: Exit");
    return status;
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::GetInfo(string methodName, string &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
   
    if(methodName == "ReadVIN") status = ReadModuleData(methodName, value);
    else if(methodName == "ReadSystemSupplierID") status = ReadModuleData(methodName, value);
    else if(methodName == "ReadRepairShopCode") status = ReadModuleData(methodName, value);
    else if(methodName == "ReadProgrammingDate") status = ReadModuleData(methodName, value);
    else if(methodName == "ReadDiagnosticDataIdentifier") status = ReadModuleData(methodName, value);
    else if(methodName == "ReadECUAddress") status = ReadModuleData(methodName, value);
    else if(methodName == "ReadManufacturingDate") status = ReadModuleData(methodName, value);
    else if(methodName == "ReadTraceData") status = ReadModuleData(methodName, value);
    else if(methodName == "ReadSoftwareUsageDesc") status = ReadModuleData(methodName, value);
    else if(methodName == "ReadSoftwareModuleID") status = ReadModuleData(methodName, value);
    else if(methodName == "ReadPartNumber") status = ReadModuleData(methodName, value);
    else if(methodName == "WriteVIN") status = CommandModule(methodName);
    else if(methodName == "ReadDPDSwitch") 
    {
        bool switchOn;
        status = ReadModuleData(methodName, switchOn);
        if(status == BEP_STATUS_SUCCESS)
        {
             if(switchOn == true) 
             {
                 value = "On";
             }else 

             {      
                 value = "Off";
             }   
        }  
        else 
        {
             value = "Unknown";
        }
    }

    // Nothing special to do, just pass to base class for evaluation
    else status = GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::GetInfo(string methodName, UINT16 &value) throw(ModuleException)
{   // Nothing special to do, just pass to base class for evaluation
    return(GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}


//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::GetInfo(string methodName, vector<UINT16>&value) throw(ModuleException)
{   // Nothing special to do, just pass to base class for evaluation
    return(GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}


//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::GetInfo(string methodName, vector<UINT8>&value) throw(ModuleException)
{   // Nothing special to do, just pass to base class for evaluation
    return(GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}


//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::GetInfo(string methodName, int &value) throw(ModuleException)
{   // Nothing special to do, just pass to base class for evaluation
    return(GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}


//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value) throw(ModuleException)
{   // Nothing special to do, just pass to base class for evaluation
    return(GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}


//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::GetInfo(string methodName, bool &value) throw(ModuleException)
{   // Nothing special to do, just pass to base class for evaluation
    return(GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}


//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t &value) throw(ModuleException)
{   // Nothing special to do, just pass to base class for evaluation
    return(GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}


//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::GetInfo(string methodName, float &value) throw(ModuleException)
{   // Nothing special to do, just pass to base class for evaluation
    return(GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::ReadPartNumber(string &partNumber)
{   // Read the part number from the module
    UINT32 pn = 0x00000000;
    BEP_STATUS_TYPE status = ReadModuleData(m_readPartNumber, pn);
    if(BEP_STATUS_SUCCESS == status)
    {   // Good data, convert to string
        char buff[16];
        partNumber = CreateMessage(buff, sizeof(buff), "%d", pn);
        Log(LOG_DEV_DATA, "End Model Part Number: %s  [%08X]", partNumber.c_str(), pn);
    }
    else
    {   // Could not read part number data from the module
        Log(LOG_ERRORS, "Could not read module part number: %s", ConvertStatusToResponse(status).c_str());
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::ReadFaults(FaultStatusVector_t &faultCodes)
{
    SerialString_t moduleResponse(255, 0);
    BEP_STATUS_TYPE status;
    UINT16 dtcCount;

    Log(LOG_FN_ENTRY, "IsuzuDeltaEngineControlModule::ReadFaults() - Enter");

    // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Read th faults from the module
    status = m_protocolFilter->GetModuleDataUUDTResponse("ReadFaults", moduleResponse);
    // Check the status of the read
    if(status == BEP_STATUS_SUCCESS)
    {
        string fullResponse;
        char temp[256];
        for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
            fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
        Log(LOG_DEV_DATA, "Module response: %s\n", fullResponse.c_str());
        //calculate dtc count - final message is end - not a dtc
        dtcCount = (moduleResponse.length() / m_dtcMessageSize) - 1;
        Log(LOG_DEV_DATA, "Calculating number of DTCs\n");
        Log(LOG_DEV_DATA, "response length: %d, FirstDTCByteIndex: %d, BytesPerDTC: %d\n",
            moduleResponse.length(), FirstDtcByteIndex(), BytesPerDtc());

        // Check the number of DTCs reported
        if((moduleResponse[FirstDtcByteIndex()] == 0x00) && (moduleResponse[FirstDtcByteIndex()+1] == 0x00))
        {   // No DTCs reported
            Log(LOG_DEV_DATA, "No faults recorded in the module\n");
        }
        else
        {   // DTCs reported, loop to get them all
            UINT16 currentDTCIndex = FirstDtcByteIndex();
            Log(LOG_DEV_DATA, "Found %d DTCs recorded in the module:\n", dtcCount);
            for(UINT16 currentDTC = 0; currentDTC < dtcCount; currentDTC++)
            {   // Build a string representation of the DTC
                char buffer[8];
                FaultCodeStatus_t dtc("","");
                INT16 currentByte = 0;
                for(; currentByte < (BytesPerDtc()-2); currentByte++)
                {
                    dtc.m_code = dtc.m_code + CreateMessage(buffer, (UINT32)sizeof(buffer), "%02X", 
                                              moduleResponse[currentDTCIndex+currentByte]);
                    if((currentDTCIndex + currentByte) >= (UINT16)moduleResponse.length())
                    {
                        Log(LOG_ERRORS, "Accessing invalid data in moduleResonse, accessing: %d, resonse length: %d",
                            (currentDTCIndex + currentByte), moduleResponse.length());
                        status = BEP_STATUS_SOFTWARE;
                    }
                }
                //advance current byte (blank field between dtc and status)
                currentByte++;
                dtc.m_status = CreateMessage(buffer, (UINT32)sizeof(buffer), "%02X", 
                                          moduleResponse[currentDTCIndex+currentByte]);
                // Log the DTC
                Log(LOG_DEV_DATA, "\tDTC %03d - %s status: %s\n", currentDTC, dtc.m_code.c_str(), dtc.m_status.c_str());
                // Stuff the DTC into the fault vector
                faultCodes.push_back(dtc);
                // Update the index to the next DTC field
                currentDTCIndex += m_dtcMessageSize;
            }
            // Check to make sure all reported faults were stored
            if(faultCodes.size() != dtcCount)
            {
                Log(LOG_ERRORS, "Error storing all reported DTCs - stored: %d, reported: %d\n",
                    faultCodes.size(), dtcCount);
                status = BEP_STATUS_SOFTWARE;
            }
        }
    }
    else
    {   // Error reading faults from the module
        Log(LOG_ERRORS, "Error read faults from %s\n", ModuleName().c_str());
    }
    // Return the status
    Log(LOG_FN_ENTRY, "IsuzuDeltaEngineControlModule::ReadFaults() - Exit:%d", status);
    return(status);
}


//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& IsuzuDeltaEngineControlModule<ProtocolFilterType>::DtcCountIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_dtcCountIndex = *byte;
    return m_dtcCountIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& IsuzuDeltaEngineControlModule<ProtocolFilterType>::FirstDtcByteIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_firstDtcIndex = *byte;
    return m_firstDtcIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& IsuzuDeltaEngineControlModule<ProtocolFilterType>::BytesPerDtc(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_bytesPerDtc = *byte;
    return m_bytesPerDtc;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::EnterNormalMode(void)
{
    Log(LOG_FN_ENTRY, "IsuzuDeltaEngineControlModule::EnterNormalMode\n");
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    Log(LOG_DEV_DATA, "Objects are OK\n");
    // Return the status of Commanding the module to enter diagnostic mode.
    return(CommandModule("EnterNormalMode"));
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::DisableNormalComms(void)
{
    Log(LOG_FN_ENTRY, "IsuzuDeltaEngineControlModule::DisableNormalComms\n");
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    Log(LOG_DEV_DATA, "Objects are OK\n");
    // Return the status of Commanding the module to enter diagnostic mode.
    return(CommandModule("DisableNormalComms"));
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::ReadMemoryLocation(vector<UINT8> &memoryContents)
{
    SerialString_t moduleResponse(255, 0);
    BEP_STATUS_TYPE status;
    UINT16 byteCount;

    Log(LOG_FN_ENTRY, "IsuzuDeltaEngineControlModule::ReadMemoryLocation() - Enter");

    // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Read th faults from the module
    status = m_protocolFilter->ReadModuleData("ReadMemoryLocation", moduleResponse);
    // Check the status of the read
    if(status == BEP_STATUS_SUCCESS)
    {
        string fullResponse;
        char temp[256];
        for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
            fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
        Log(LOG_DEV_DATA, "Module response: %s\n", fullResponse.c_str());

        char buffer[8];
        INT16 currentByte = 5;
        for(; currentByte < (moduleResponse.length() - 1); currentByte++)
        {
            // Stuff the value into the memory contents vector
            memoryContents.push_back(moduleResponse[currentByte]);
        }
    }
    else
    {   // Error reading faults from the module
        Log(LOG_ERRORS, "Error read memory contents from %s\n", ModuleName().c_str());
    }
    // Return the status
    Log(LOG_FN_ENTRY, "IsuzuDeltaEngineControlModule::ReadMemoryLocation() - Exit:%d", status);
    return(status);
}
//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE IsuzuDeltaEngineControlModule<ProtocolFilterType>::GetMAFLearningValue(string messageTag, string &value)
{
    SerialString_t moduleResponse(55, 0);
    BEP_STATUS_TYPE status;


    Log(LOG_FN_ENTRY, "IsuzuDeltaEngineControlModule::GetMAFLearningValue() - Enter");
	Log(LOG_FN_ENTRY, "This is not a drill.");

    status = m_protocolFilter->GetModuleData(messageTag, moduleResponse);
    // Check the status of the read
    if(status == BEP_STATUS_SUCCESS)
    {
        Log(LOG_DEV_DATA, "Parsing Module Response...\n");
        string fullResponse;
        char temp[56];
        for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
            fullResponse += CreateMessage(temp, 56, "%02X ", moduleResponse[ii]);
        Log(LOG_DEV_DATA, "Module response: %s\n", fullResponse.c_str());
		value = "";
		value = fullResponse.substr(9,21);
        Log(LOG_DEV_DATA, "Data Value: %s\n", value.c_str());
    }
    else
    {   // Error reading faults from the module
        Log(LOG_ERRORS, "Error read value from %s\n", ModuleName().c_str());
    }
    // Return the status
    Log(LOG_FN_ENTRY, "IsuzuDeltaEngineControlModule::GetMAFLearningValue() - Exit:%d", status);
    return(status);

}


