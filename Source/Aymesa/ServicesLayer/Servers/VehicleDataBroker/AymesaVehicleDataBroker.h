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
#ifndef AymesaVehicleDataBroker_h
#define AymesaVehicleDataBroker_h
//-------------------------------------------------------------------------------------------------
#include "VehicleDataBroker.h"
//-------------------------------------------------------------------------------------------------
class AymesaVehicleDataBroker : public VehicleDataBroker
{
public:
	AymesaVehicleDataBroker();
	virtual ~AymesaVehicleDataBroker();

	/**
	 * Request to read a data tag
	 *
	 * @param node    Data to be read
	 * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
	 * @return Value of tag
	 */
	virtual const string Read(const XmlNode *node, const INT32 rate);
};
//-------------------------------------------------------------------------------------------------
#endif //AymesaVehicleDataBroker_h
