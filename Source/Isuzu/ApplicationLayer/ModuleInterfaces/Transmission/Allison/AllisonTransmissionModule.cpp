//*************************************************************************
// FILE:
//    $Header:
//
// FILE DESCRIPTION:
//	Module Interface for the Daimler Chrysler Allison Transmission controller.
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
#include "AllisonTransmissionModule.h"
//*************************************************************************

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
AllisonTransmissionModule<ProtocolFilterType>::AllisonTransmissionModule(void) : 
GenericTransmissionModuleTemplate<ProtocolFilterType>()
{   // Nothing special to do
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
AllisonTransmissionModule<ProtocolFilterType>::~AllisonTransmissionModule(void)
{   // Nothing special to do
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
bool AllisonTransmissionModule<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{   // Call the base class to begin the initialization
    bool status = GenericTransmissionModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
    // Store the byte containing the number of DTCs reported in the module response
    Log(LOG_DEV_DATA, "AllisonTransmissionModule::InitializeHook: Getting Dtc Info from Config");
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
    Log(LOG_DEV_DATA, "AllisonTransmissionModule::InitializeHook: Exit");
    return status;
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE AllisonTransmissionModule<ProtocolFilterType>::MonitorTransmissionGear(IProtocolFilter *filter)
{
    try
    {
        Log(LOG_DEV_DATA, "Entering AllisonTransmissionModule::MonitorTransmissionGear");
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
            //status = ReadModuleData("ReadCurrentGear", currentGear, NULL, NULL, filter);
            SerialString_t msg;
            std::string temp = "ReadShiftLeverPosition";
            try
            {
                status = m_protocolFilter->GetBusBroadcastMessage(temp, (long)500, msg);
            }
            catch (exception e)
            {
                Log(LOG_ERRORS, "MonitorTransmissionGear::GetBusBroadcastMessage error: %s", e.what());
            }
            catch (...)
            {
                Log(LOG_ERRORS, "uh oh, MonitorTransmissionGear::GetBusBroadcastFailed for something");
            }
            // Check the status of the read
            if(status == BEP_STATUS_SUCCESS)
        	{   // Valid message, extract the data
                Log(LOG_DEV_DATA,"Message is valid, parsing gear data\n");
                UINT8 gear = (UINT8) ParseUnsignedIntegerResponse("InterpShiftLeverPosition", msg); 
                Log(LOG_DEV_DATA,"Parsed response:: gear = %d", gear);

                if (gear == 125) currentGear = "N";     //neutral
                else if (gear == 126) currentGear = "1"; //first gear
                else if (gear == 127) currentGear = "2"; //second gear
                else if (gear == 128) currentGear = "3"; //third gear
                else if (gear == 129) currentGear = "4"; //fourth gear
                else if (gear == 130) currentGear = "5"; //fifth gear
                else if (gear == 131) currentGear = "6"; //sixth gear
                else if (gear == 124) currentGear = "R"; //reverse
                else if (gear == 251) currentGear = "P"; //park
                else if (gear == 254) 
                {
                    currentGear = "E"; //error
                    status = BEP_STATUS_FAILURE;
                }
                else 
                {
                    Log(LOG_ERRORS, "Message could not be parsed, gear '%d' found", gear);
                    status = BEP_STATUS_FAILURE;
                }
                
                Log(LOG_DEV_DATA,"Found gear:: gear: %d, currentGear: %s", gear, currentGear.c_str());
            }
        	else
        	{   // Could not find a matching message, give up on it
        		char buff[256];
        		string logMessage;
        		for(UINT8 index = 0; index < msg.length(); index++)
        		{
        			logMessage += CreateMessage(buff, sizeof(buff), " $%02X", msg[index]);
        		}
        		Log(LOG_ERRORS, "No matching message entry for %s", logMessage.c_str());
                status = BEP_STATUS_FAILURE;
        	}
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
                    Log(LOG_ERRORS, "%s XmlException in MonitorTransmissionGear: %s\n", ModuleName().c_str(), err.GetReason());
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
        Log(LOG_DEV_DATA, "AllisonTransmissionModule::MonitorTransmissionGear returning status: %d", (int) status);
        return(status);
    }
    catch (exception e)
    {
        Log(LOG_ERRORS, "FindMyError: %s", e.what());
        return BEP_STATUS_ERROR;
    }
    catch (...)
    {
        Log(LOG_ERRORS, "FindMyError sucks");
        return BEP_STATUS_ERROR;
    }
}

//-------------------------------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE AllisonTransmissionModule<ProtocolFilterType>::ReadPartNumber(string &partNumber)
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
BEP_STATUS_TYPE AllisonTransmissionModule<ProtocolFilterType>::ReadFaults(FaultStatusVector_t &faultCodes)
{
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status;
    uint8_t     dtcCount=0, dtcIdx=0;
    uint16_t    dtcVal;
    char    dtcStr[ 16];

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData("ReadFaults", response);

    if (status == BEP_STATUS_SUCCESS)
    {
        // The first byte after the ID is a DTC count
        //if( response.size() >= 2)
        //first 7 bytes are header bytes
        if ( response.size() >= 8)
        {
            // Erase the first byte
            //response.erase( 0, 1);
            // Read the number of DTC's to follow
            //dtcCount = response[ 0];

            //no count byte so we erase header bytes and divide remaining # bytes by 4
            //because each DTC has 3 bytes abd a status byte
            response.erase( 0, 7);
            dtcCount =  response.size()/4;
        }
        Log( LOG_DEV_DATA, "AllisonTransmissionModule - Module is reporting %d DTC's\n", dtcCount);

        // Extract the DTCs from the respnse
        for ( dtcIdx=0; (dtcIdx<dtcCount) && (response.size()>1); dtcIdx++)
        {
            // Erase the first byte  - high byte = $00 (always?)
            response.erase( 0, 1);

            // DTC are sent as [ HI,LO]
            dtcVal = response[ 0];
            dtcVal = ((dtcVal << 8) & 0xFF00) | response[1];
            Log( LOG_DEV_DATA, "\tDTC %d: $%04X\n", dtcIdx, dtcVal);

            // Erase the bytes for this DTC
            response.erase( 0,3);

            // Add the DTC to the fault list
            FaultCodeStatus_t dtc("","");
            dtc.m_code = CreateMessage(dtcStr, 16, "%04X", dtcVal);
            faultCodes.push_back(dtc);
            /*std::string faultStr( CreateMessage(dtcStr, 16, "%04X", dtcVal));
            FaultCode_t fault(faultStr);
            faultCodes.push_back( fault);*/
        }

        // If we did not receive the full DTC list from the module
        if ( dtcIdx < dtcCount)
        {
            // Log a message
            Log( LOG_ERRORS, "Did not receive full list of DTCs: %d of %d DTCs received\n",
                 dtcIdx, dtcCount);
        }

        return(BEP_STATUS_SUCCESS);
    }
    else
    {
        return(status);
    }
}
//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline BEP_STATUS_TYPE AllisonTransmissionModule<ProtocolFilterType>::ReadFaults(IProtocolFilter *filter, 
                                                                               FaultVector_t &faultCodes)
{   // Just call the base class to handle this
    return GenericTransmissionModuleTemplate<ProtocolFilterType>::ReadFaults(filter, faultCodes);
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& AllisonTransmissionModule<ProtocolFilterType>::DtcCountIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_dtcCountIndex = *byte;
    return m_dtcCountIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& AllisonTransmissionModule<ProtocolFilterType>::FirstDtcByteIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_firstDtcIndex = *byte;
    return m_firstDtcIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& AllisonTransmissionModule<ProtocolFilterType>::BytesPerDtc(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_bytesPerDtc = *byte;
    return m_bytesPerDtc;
}


//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
const string AllisonTransmissionModule<ProtocolFilterType>::GetCurrentState(void)
{
    Log(LOG_DEV_DATA, "Entering AllisonTransmissionModule::GetCurrentState");
    //Get the current state from the module
    //moduleStatus = m_vehicleModule.GetInfo(GetDataTag(currentStateTag), currentState);

    SerialString_t msg;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_FAILURE;  // Used to store return result for module read
    std::string temp = "ReadShiftLeverPosition";
    string currentState(BEP_NO_DATA);                   // Used to store current state from module read
    try 
    {
        moduleStatus = m_protocolFilter->GetBusBroadcastMessage(temp, (long)500, msg);
    }
    catch (exception e)
    {
        Log(LOG_ERRORS, "GetCurrentState::GetBusBroadcastMessage error: %s", e.what());
    }
    catch (...)
    {
        Log(LOG_ERRORS, "uh oh, GetCurrentState::GetBusBroadcastFailed for something");
    }
    if (moduleStatus == BEP_STATUS_SUCCESS)
    {   // Valid message, extract the data
        UINT8 state = (UINT8) ParseUnsignedIntegerResponse("InterpShiftLeverPosition", msg);

        if (state == 125) currentState = "N";     //neutral
        else if (state == 126) currentState = "1"; //first state
        else if (state == 127) currentState = "2"; //second state
        else if (state == 128) currentState = "3"; //third state
        else if (state == 129) currentState = "4"; //fourth state
        else if (state == 130) currentState = "5"; //fifth state
        else if (state == 131) currentState = "6"; //sixth state
        else if (state == 124) currentState = "R"; //reverse
        else if (state == 251) currentState = "P"; //park
        else if (state == 254) currentState = "E"; //error
        else Log(LOG_ERRORS, "Message could not be parsed, state '%d' found", state);

        Log(LOG_DEV_DATA,"Parsed response:: state: '%d', currentState: '%s'", state, currentState.c_str());
    }
    else
    {   // Could not find a matching message, give up on it
        char buff[256];
        string logMessage;
        for(UINT8 index = 0; index < msg.length(); index++)
        {
            logMessage += CreateMessage(buff, sizeof(buff), " $%02X", msg[index]);
        }
        Log(LOG_ERRORS, "No matching message entry for %s", logMessage.c_str());
    }

    return currentState;
}
