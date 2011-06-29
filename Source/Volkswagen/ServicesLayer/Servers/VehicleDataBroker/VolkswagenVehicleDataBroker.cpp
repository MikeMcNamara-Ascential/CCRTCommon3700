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
#include "VolkswagenVehicleDataBroker.h"
#include "INamedDataBroker.h"

//-------------------------------------------------------------------------------------------------
VolkswagenVehicleDataBroker::VolkswagenVehicleDataBroker() : VehicleDataBroker()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
VolkswagenVehicleDataBroker::~VolkswagenVehicleDataBroker()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string VolkswagenVehicleDataBroker::Read(const XmlNode *node, const INT32 rate /*= 0*/)
{
    string result;
    if(!node->getName().compare(GetVehicleBuildTag()))
    {   // If the diagnostic cable is not connected, change brake type to base brake
        INamedDataBroker ndb;
        string response, value;
        // Get the vehicle build info
        XmlNodeMap buildRecord = m_data.getNode(GetVehicleBuildTag())->getChildren();
        if(BEP_STATUS_SUCCESS == ndb.Read(CBL_CONNECT_DATA_TAG, response, true))
        { 
            if(BEP_STATUS_SUCCESS == ndb.GetByTag(CBL_CONNECT_DATA_TAG, value, response))
            {
                if(!atob(value.c_str()))
                {   // No cable connected, set brake type to base brake
                    Log(LOG_DEV_DATA, "No cable connected, setting brake type to %s", GetDataTag("BaseBrakeString").c_str());
                    XmlNodeMapItr iter = buildRecord.find("BrakeType");
                    if(iter != buildRecord.end())
                    {
                        iter->second->setValue(GetDataTag("BaseBrakeString"));
                    }
                }
            }
        }
        // Put the vehicle build into a string
        for(XmlNodeMapItr iter = buildRecord.begin(); iter != buildRecord.end(); iter++)
            result += iter->second->ToString();
    }
    else
    {
        result = VehicleDataBroker::Read(node, rate);
    }
    Log(LOG_DEV_DATA, "VehicleDataBroker::Read(%s) returning: %s", node->getName().c_str(), result.c_str());
    return result;
}
