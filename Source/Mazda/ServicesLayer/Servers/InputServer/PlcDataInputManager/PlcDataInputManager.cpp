//*************************************************************************
// FILE DESCRIPTION:
//  The PlcDataInputManager handles vehicle data type inputs and publishes 
//  the data to objects that need the data.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "PlcDataInputManager.h"

//-------------------------------------------------------------------------------------------------
PlcDataInputManager::PlcDataInputManager() : InputDeviceBase(), m_dataBroker(NULL)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
PlcDataInputManager::~PlcDataInputManager()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void PlcDataInputManager::Initialize(const XmlNode *document)
{   // Call the base class to begin the initialization
    InputDeviceBase::Initialize(document);
    Log(LOG_FN_ENTRY, "PlcDataInputManager::Initialize() - Enter");
    // Load the vehicle type translation table
    try
    {
        m_vehicleTypeLookupTable.DeepCopy(document->getChild("Setup/VehicleTypeDecode")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No vehicle type conversion specified: %s", excpt.GetReason());
        m_vehicleTypeLookupTable.clear(true);
    }
    Log(LOG_FN_ENTRY, "PlcDataInputManager::Initialize() - Exit");
}

//-------------------------------------------------------------------------------------------------
const string PlcDataInputManager::Register(void)
{   // Call the base class to begin the registration
    string result(InputDeviceBase::Register());
    Log(LOG_FN_ENTRY, "PlcDataInputManager::Register() - Enter");
    // Create a new INdb object
    if(m_dataBroker != NULL)  delete m_dataBroker;
    m_dataBroker = new INamedDataBroker();
    // Return the registration result
    Log(LOG_FN_ENTRY, "PlcDataInputManager::Register() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
const string PlcDataInputManager::Publish(const XmlNode *node)
{   // Determine if we need to handle this item specially
    if(!node->getName().compare(GetDataTag("VehicleTypeFromPlc")))
    {   // Need to pass this on so the correct data file can be loaded
        string response;
        string vehicleType = LookupVehicleType(BposReadInt(node->getValue().c_str()));
        if(vehicleType.compare(GetDataTag("DefaultVehicleModel")))
        {
            m_dataBroker->Write(NEXT_VIN_DATA_TAG, vehicleType, response, true);
        }
        else
        {   // Unknow vehicle type, do not publish
            Log(LOG_DEV_DATA, "Unknown vehicle type: %s", node->getValue().c_str());
            m_dataBroker->Write(VINDISPLAY_DATA_TAG, vehicleType, response, true);
        }
    }
    // Do the normal publish handling
    return BepServer::Publish(node);
}

//-------------------------------------------------------------------------------------------------
string PlcDataInputManager::LookupVehicleType(const INT32 &type)
{   // Attempt to find the specified vehicle type
    char buff[16];
    string vehicleTypeLookup = CreateMessage(buff, sizeof(buff), "Type%02d", type);
    XmlNodeMapItr iter = m_vehicleTypeLookupTable.find(vehicleTypeLookup);
    string vehicleModel(GetDataTag("DefaultVehicleModel"));
    if(iter != m_vehicleTypeLookupTable.end())
    {   // Get the translation if it was found
        vehicleModel = iter->second->getValue();
    }
    // Return the translated type
    Log(LOG_DEV_DATA, "Lookup vehicle type: %02d returning: %s", type, vehicleModel.c_str());
    return vehicleModel;
}
