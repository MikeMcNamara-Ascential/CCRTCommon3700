//*************************************************************************
// FILE DESCRIPTION:
//   Vehicle data broker that is responsible for maintaining and publishing
//   all of the information related to the vehicle.
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
#include "ToyotaVehicleDataBroker.h"

//-------------------------------------------------------------------------------------------------
ToyotaVehicleDataBroker::ToyotaVehicleDataBroker() : VehicleDataBroker()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
ToyotaVehicleDataBroker::~ToyotaVehicleDataBroker()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string ToyotaVehicleDataBroker::Publish(const XmlNode *node)
{   // Check if this is an engine type
    if(!node->getName().compare(GetDataTag("SixCylinderEngineTag")))
    {   // Set the selected engine size
        SetEngineSize(GetDataTag("EngineType" + node->getValue()));
        // Update the shift parameters on the screen
        UpdateTransmissionShiftPoints();
    }
    else if(!node->getName().compare(GetDataTag("SpeedDisplayScalingTag")))
    {   // Set the selected speedometer type
        SetSpeedoType(GetDataTag("SpeedoType" + node->getValue()));
        // Update the shift parameters on the screen
        UpdateTransmissionShiftPoints();
    }
    // Call the base class to do the publishing
    return VehicleDataBroker::Publish(node);
}

//-------------------------------------------------------------------------------------------------
void ToyotaVehicleDataBroker::LoadAdditionalConfigurationItems(const XmlNode *document)
{   // Call the base class to start loading items
    VehicleDataBroker::LoadAdditionalConfigurationItems(document);
    // Set the default engine size and speedo type
    SetEngineSize(GetDataTag("DefaultEngineType"));
    SetSpeedoType(GetDataTag("DefaultSpeedoType"));
}

//-------------------------------------------------------------------------------------------------
const string ToyotaVehicleDataBroker::ReadLatestVehicleBuildRecord(bool publishAfterRead/*=true*/)
{   // Call the base class to start the process and handle the bulk
    string result = VehicleDataBroker::ReadLatestVehicleBuildRecord(publishAfterRead);
    // See if we can get the current engine and speedo types from the system
    INamedDataBroker ndb;
    string response;
    string value;
    if(ndb.Read(GetDataTag("SixCylinderEngineTag"), response, true) == BEP_STATUS_SUCCESS)
    {
        if(ndb.GetByTag(GetDataTag("SixCylinderEngineTag"), value, response) == BEP_STATUS_SUCCESS)
        {
            SetEngineSize(GetDataTag("EngineType" + value));
        }
    }
    if(ndb.Read(GetDataTag("SpeedDisplayScalingTag"), response, true) == BEP_STATUS_SUCCESS)
    {
        if(ndb.GetByTag(GetDataTag("SpeedDisplayScalingTag"), value, response) == BEP_STATUS_SUCCESS)
        {
            SetSpeedoType(GetDataTag("SpeedoType" + value));
        }
    }
    // Make sure the shift points are displayed
    UpdateTransmissionShiftPoints();
    return result;
}

//-------------------------------------------------------------------------------------------------
void ToyotaVehicleDataBroker::UpdateTransmissionShiftPoints(void)
{   // Get an XmlNodeMap of the transmission shift points
    string buildDataNode("TransmissionShiftPoints/" + GetEngineSize() + "/" + GetSpeedoType());
    try
    {
        const XmlNodeMap &shiftPoints = m_data.getNode(GetVehicleBuildTag())->getChild(buildDataNode)->getChildren();
        // Publish the new shift points
        for(XmlNodeMapCItr iter = shiftPoints.begin(); iter != shiftPoints.end(); iter++)
        {   // Write the data to all subscribers
            Write(iter->second);
        }
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No build data loaded, cannot display shift points");
    }
}

//-------------------------------------------------------------------------------------------------
inline const string& ToyotaVehicleDataBroker::GetEngineSize(void)
{
    return m_engineSize;
}

//-------------------------------------------------------------------------------------------------
inline const string& ToyotaVehicleDataBroker::GetSpeedoType(void)
{
    return m_speedoType;
}

//-------------------------------------------------------------------------------------------------
inline void ToyotaVehicleDataBroker::SetEngineSize(const string &engineSize)
{
    m_engineSize = engineSize;
    Write(GetDataTag("EngineTypeTag"), engineSize);
}

//-------------------------------------------------------------------------------------------------
inline void ToyotaVehicleDataBroker::SetSpeedoType(const string &speedoType)
{
    m_speedoType = speedoType;
    Write(GetDataTag("SpeedoTypeTag"), speedoType);
}
