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
#ifndef MazdaVehicleDataBroker_h
#define MazdaVehicleDataBroker_h
//-------------------------------------------------------------------------------------------------
#include "VehicleDataBroker.h"
//-------------------------------------------------------------------------------------------------
class MazdaVehicleDataBroker : public VehicleDataBroker
{
public:
    MazdaVehicleDataBroker();
    virtual ~MazdaVehicleDataBroker();




protected:

    /**
     * Read the current vehicle build record from the WorkCellController.
     *
     * @return Result of the operation - BEP_SUCCESS_RESPONSE, BEP_ERROR_RESPONSE, etc.
     */
    virtual const string ReadLatestVehicleBuildRecord(bool publishAfterRead=true);


};
//-------------------------------------------------------------------------------------------------
#endif //MazdaVehicleDataBroker_h
