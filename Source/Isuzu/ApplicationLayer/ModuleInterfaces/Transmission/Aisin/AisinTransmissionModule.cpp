//*************************************************************************
// FILE:
//    $Header:
//
// FILE DESCRIPTION:
//	Module Interface for the Daimler Chrysler Aisin Transmission controller.
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
//*************************************************************************
#include "AisinTransmissionModule.h"
//*************************************************************************

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
AisinTransmissionModule<ProtocolFilterType>::AisinTransmissionModule(void) : 
GenericTransmissionModuleTemplate<ProtocolFilterType>()
{   // Nothing special to do
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
AisinTransmissionModule<ProtocolFilterType>::~AisinTransmissionModule(void)
{   // Nothing special to do
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
bool AisinTransmissionModule<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{   // Call the base class to begin the initialization
    bool status = GenericTransmissionModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
    // Store the byte containing the number of DTCs reported in the module response
    Log(LOG_DEV_DATA, "AisinTransmissionModule::InitializeHook: Getting Dtc Info from Config");
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
        byte = 3;
        Log(LOG_ERRORS, "Bytes per DTC not specified, using 1");
    }
    Log(LOG_DEV_DATA, "Setting bytes per DTC to %d", byte);
    BytesPerDtc(&byte);
    Log(LOG_DEV_DATA, "AisinTransmissionModule::InitializeHook: Exit");
    return status;
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE AisinTransmissionModule<ProtocolFilterType>::MonitorLimpInFaultStatus(IProtocolFilter *filter)
{
    FaultVector_t faultCodes;
    BEP_STATUS_TYPE status = ReadFaults(filter, faultCodes);
    // if ReadFaults was successful, then determine if there are any faults
    if(BEP_STATUS_SUCCESS == status)
    {   // Check if faults are present
        if(!faultCodes.empty())
        {   // Process the faults
            bool reportFault = false;
            ITestResultServer rpt;
            for (INT32 index = 0; ((UINT16)index < faultCodes.size()) && !reportFault; index++)
            {   // Check if the fault is not ignored
                string faultTag = "ModuleFault_" + faultCodes[index];
                if (m_ignoredFaults->find(faultTag) == m_ignoredFaults->end())
                {
                    reportFault = true;
                    // Get the fault description from the fault file
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
            // If found a fault that is not ignored, abort all testing
            if (reportFault)
            {   // Need to abort the test
                INamedDataBroker ndb;
                string response;
                ndb.Write(ABORT_DATA_TAG, "1", response, true);
                // Display the reason for the Abort
                IPromptServer ips;
                ips.Initialize(PROMPT_SERVER_NAME);
                ips.DisplayPrompt(1, m_configNode->getChild("Setup/ModuleFaultsDetectedPrompt")->getValue(), response);
            }
        }
    }
    else
    {   // Error getting fault data
        Log(LOG_ERRORS, "Error reading fault data from the module: %s", ConvertStatusToResponse(status).c_str());
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE AisinTransmissionModule<ProtocolFilterType>::MonitorTransmissionGear(IProtocolFilter *filter)
{
    BEP_STATUS_TYPE status    = BEP_STATUS_ERROR;
    string currentGear        = "Unknown";
    DataAnalysis analyze;
    static bool initializedObservedGears = false;
    if(!initializedObservedGears)
    {   // Set specific gears to observed since that data does not come across
        SetObservedGearReverse(true);
        SetObservedGearNeutral(true);
    }
    // Check if all gears have been observed
    if(!AllGearsObserved())
    {   // Read the current gear from the module
        status = ReadModuleData("ReadCurrentGear", currentGear, NULL, NULL, filter);
        // Check the status of the read
        if(BEP_STATUS_SUCCESS == status)
        {   // Pick the gear out of the responses
            try
            {
                Log(LOG_DEV_DATA, "Current Gear: %s", currentGear.c_str());
                // Check for first gear
                if(!Gear1Observed())
                {   // First gear was not observed, check if it just was
                    SetObservedGear1(analyze.CompareData(currentGear, GetGear1String(), EQUAL));
                    if(Gear1Observed()) Log(LOG_DEV_DATA, "Saw First Gear\n");
                }
                // Check for second gear
                if(!Gear2Observed())
                {   // Second gear was not observed, check if it just was
                    SetObservedGear2(analyze.CompareData(currentGear, GetGear2String(), EQUAL));
                    if(Gear2Observed()) Log(LOG_DEV_DATA, "Saw Second Gear\n");
                }
                // Check for third gear
                if(!Gear3Observed())
                {   // Third gear was not observed, check if it just was
                    SetObservedGear3(analyze.CompareData(currentGear, GetGear3String(), EQUAL));
                    if(Gear3Observed()) Log(LOG_DEV_DATA, "Saw Third Gear\n");
                }
                // Check for fourth gear
                if(!Gear4Observed())
                {   // Fourth gear was not observed, check if it just was
                    SetObservedGear4(analyze.CompareData(currentGear, GetGear4String(), EQUAL));
                    if(Gear4Observed()) Log(LOG_DEV_DATA, "Saw Fourth Gear\n");
                }
                // Check for fifth gear
                if(!Gear5Observed())
                {   // Fifth gear was not observed, check if it just was
                    SetObservedGear5(analyze.CompareData(currentGear, GetGear5String(), EQUAL));
                    if(Gear5Observed()) Log(LOG_DEV_DATA, "Saw Fifth Gear\n");
                }
                // Check for sixth gear
                if(!Gear6Observed())
                {   // Sixth gear was not observed, check if it just was
                    SetObservedGear6(analyze.CompareData(currentGear, GetGear6String(), EQUAL));
                    if(Gear6Observed()) Log(LOG_DEV_DATA, "Saw Sixth Gear\n");
                }
            }
            catch(XmlException &err)
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
    return(status);
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE AisinTransmissionModule<ProtocolFilterType>::ReadPartNumber(string &partNumber)
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
BEP_STATUS_TYPE AisinTransmissionModule<ProtocolFilterType>::ReadFaults(FaultStatusVector_t &faultCodes)
{
    SerialString_t moduleResponse(255, 0);
    BEP_STATUS_TYPE status;
    UINT16 dtcCount;

    Log(LOG_FN_ENTRY, "AisinTransmissionModule::ReadFaults() - Enter");

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
        dtcCount = (moduleResponse.length() / DTC_MESSAGE_SIZE) - 1;
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
                currentDTCIndex += DTC_MESSAGE_SIZE;
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
    Log(LOG_FN_ENTRY, "AisinTransmissionModule::ReadFaults() - Exit:%d", status);
    return(status);
}
//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline BEP_STATUS_TYPE AisinTransmissionModule<ProtocolFilterType>::ReadFaults(IProtocolFilter *filter, 
                                                                               FaultVector_t &faultCodes)
{   // Just call the base class to handle this
    return GenericTransmissionModuleTemplate<ProtocolFilterType>::ReadFaults(filter, faultCodes);
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& AisinTransmissionModule<ProtocolFilterType>::DtcCountIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_dtcCountIndex = *byte;
    return m_dtcCountIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& AisinTransmissionModule<ProtocolFilterType>::FirstDtcByteIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_firstDtcIndex = *byte;
    return m_firstDtcIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& AisinTransmissionModule<ProtocolFilterType>::BytesPerDtc(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_bytesPerDtc = *byte;
    return m_bytesPerDtc;
}
