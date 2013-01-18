//*************************************************************************
// FILE DESCRIPTION:
//   Vehicle data broker that is responsible for maintaining and publishing
//   all of the information related to the vehicle.
//
//===========================================================================
// Copyright (c) 2013- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "AymesaVehicleDataBroker.h"
#include "INamedDataBroker.h"

//-------------------------------------------------------------------------------------------------
AymesaVehicleDataBroker::AymesaVehicleDataBroker() : VehicleDataBroker()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
AymesaVehicleDataBroker::~AymesaVehicleDataBroker()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string AymesaVehicleDataBroker::Read(const XmlNode *node, const INT32 rate)
{
	string result;
	Log(LOG_FN_ENTRY, "AymesaVehicleDataBroker::Read(%s) - Enter", node->getName().c_str());
	if(!node->getName().compare(GetVehicleBuildTag()))
	{   // If the diagnostic cable is connected, change brake type to ABS
		INamedDataBroker ndb;
		string response, value;
		// Get the vehicle build info
		XmlNodeMap buildRecord = m_data.getNode(GetVehicleBuildTag())->getChildren();
		if(BEP_STATUS_SUCCESS == ndb.Read(CBL_CONNECT_DATA_TAG, response, true))
		{ 
			if(BEP_STATUS_SUCCESS == ndb.GetByTag(CBL_CONNECT_DATA_TAG, value, response))
			{
				if(atob(value.c_str()))
				{   // Cable connected, set brake type to ABS
					Log(LOG_DEV_DATA, "Cable connected, setting brake type to %s", GetDataTag("AbsString").c_str());
					XmlNodeMapItr iter = buildRecord.find("BrakeType");
					if(iter != buildRecord.end())
					{
						iter->second->setValue(GetDataTag("AbsString"));
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
	Log(LOG_DEV_DATA, "AymesaVehicleDataBroker::Read(%s) returning: %s", node->getName().c_str(), result.c_str());
	return result;
}
