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
#include "MazdaVehicleDataBroker.h"

//-------------------------------------------------------------------------------------------------
MazdaVehicleDataBroker::MazdaVehicleDataBroker() : VehicleDataBroker()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MazdaVehicleDataBroker::~MazdaVehicleDataBroker()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string MazdaVehicleDataBroker::ReadLatestVehicleBuildRecord(bool publishAfterRead/*=true*/)
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
