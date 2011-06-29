//*************************************************************************
// FILE DESCRIPTION:
//  Module Interface for the Electronic Park Brake.
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
// 
//*************************************************************************
#include "ElectronicParkBrakeModule.h"

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
ElectronicParkBrakeModule<ProtocolFilterType>::ElectronicParkBrakeModule() : GenericModuleTemplate<ProtocolFilterType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
ElectronicParkBrakeModule<ProtocolFilterType>::~ElectronicParkBrakeModule()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
inline string ElectronicParkBrakeModule<ProtocolFilterType>::ModuleName(void)
{
    return "Electronic Park Brake";
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilter>
bool ElectronicParkBrakeModule<ProtocolFilter>::InitializeHook(const XmlNode *configNode)
{   // Begin by initializing the base class
    bool status = GenericModuleTemplate<ProtocolFilter>::InitializeHook(configNode);
    // Initialize the keep alive comm object
    if(m_keepAliveComm != NULL)
    {
        Log(LOG_DEV_DATA, "BoschAbsModule::InitializeHook: Initializing the keep alive comm object\n");
        m_keepAliveComm->Initialize(configNode);
    }
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
BEP_STATUS_TYPE ElectronicParkBrakeModule<ProtocolFilter>::PerformAssemblyCheck()
{   // Command the module to perform reduction on specified wheel
    return CommandModule("PerformAssemblyCheck");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
BEP_STATUS_TYPE ElectronicParkBrakeModule<ProtocolFilter>::ParameterArbitration()
{   // Command the module to perform reduction on specified wheel
    return CommandModule("ParameterArbitration");
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
BEP_STATUS_TYPE ElectronicParkBrakeModule<ProtocolFilter>::UnlockModuleSecurity()
{   
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    UINT32 seed = 0;           // Security seed read from the module 
    SerialArgs_t securityKey;  // Security unlock key
    // Read the security seed from the module
    status = ReadModuleData("ReadSecuritySeed", seed);
    if (BEP_STATUS_SUCCESS == status)
    {   // Read security seed from the module. Check for special conditions
        Log(LOG_DEV_DATA, "Read security seed: %08X\n", seed);
        if (seed == 0x00000000)
        {   // Module already unlocked, no need to do anything else
            Log(LOG_DEV_DATA, "Module Security already unlocked\n");
        }
        // Security not unlocked, proceed with algorithm
        else
        {   // Calculate security key
            securityKey = CalculateSecurityKey(seed);
            // Calculated the security key, unlock the module
            Log(LOG_DEV_DATA, "Calculated security key\n");
            status = CommandModule("UnlockModuleSecurity", &securityKey);
            // Check the status of the unlock
            if (BEP_STATUS_SUCCESS == status)
            {   // Module Security unlocked
                Log(LOG_DEV_DATA, "Unlocked Module Security\n");
                status = BEP_STATUS_SUCCESS;
            }
            else
            {   // Error unlocking module
                Log(LOG_ERRORS, "Failed to unlock module security - status: %s\n",
                    ConvertStatusToResponse(status).c_str());
            }
        }
    }
    else
    {   // Error reading security seed
        Log(LOG_ERRORS, "Failed to read security seed from the module - status: %s\n",
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template <class ProtocolFilter>
const SerialArgs_t ElectronicParkBrakeModule<ProtocolFilter>::CalculateSecurityKey(const UINT32 &seed)
{
    // Define factors used to unlock the EPB module
    UINT32 appKey = 0x70F76203;     // Used for Security lvl 1
    UINT32 key1 = 0;
    UINT32 key2 = seed;
    UINT32 finalKey;
    // Storage location to format and return the calculated key
    SerialArgs_t key;

    //Calculate security key
    key1 = seed ^ appKey;
    RotateKey(key2);
    key2 = key2 ^ appKey;
    finalKey = key1 + key2;

    // Log the calculate security key
    Log(LOG_DEV_DATA,"Seed: Seed0=%02X, Seed1=%02X, Seed2=%02X%, Seed3=%02X\n",
        (UINT8)((0xFF000000 & seed) >> 24),
        (UINT8)((0x00FF0000 & seed) >> 16),
        (UINT8)((0x0000FF00 & seed) >> 8),
        (UINT8)(0x000000FF & seed));
    // Log the calculate security key
    Log(LOG_DEV_DATA,"Calculated security key: key0=%02X, key1=%02X, key2=%02X%, key3=%02X\n",
        (UINT8)((0xFF000000 & finalKey) >> 24),
        (UINT8)((0x00FF0000 & finalKey) >> 16),
        (UINT8)((0x0000FF00 & finalKey) >> 8),
        (UINT8)(0x000000FF & finalKey));

    // push the key into the variable used to return the key
    key.push_back((UINT8)((0xFF000000 & finalKey) >> 24));  //MSByte of security key
    key.push_back((UINT8)((0x00FF0000 & finalKey) >> 16));  //High middle byte of security key
    key.push_back((UINT8)((0x0000FF00 & finalKey) >> 8));   //Lower middle byte of security key
    key.push_back((UINT8)(0x000000FF & finalKey));         //LSByte of security key
    // return the calculated key
    return key;
}

template <class ProtocolFilter>
void ElectronicParkBrakeModule<ProtocolFilter>::RotateKey(UINT32 &key)
{           
    UINT32 mask = 0x00000001;
    UINT32 newKey = 0x00000000;
    for (INT16 ii=0; ii < sizeof(key)*8; ii++)
    {
        newKey = newKey | ((mask & key) >> (ii));
        // Don't shift after evaluating the last bit
        if (ii < sizeof(key)*8 - 1)
        {
            newKey = newKey << 1;
            mask = mask << 1;
        }
    }
    key = newKey;
    Log(LOG_DEV_DATA,"Rotated key: %X\n", key);
}
