//*************************************************************************
// FILE DESCRIPTION:
//  PlantHostInbound for Mazda.
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
#ifndef MazdaPlantHostInbound_h
#define MazdaPlantHostInbound_h
//-------------------------------------------------------------------------------------------------
#include "PlantHostInbound.h"
//-------------------------------------------------------------------------------------------------
class MazdaPlantHostInbound : public PlantHostInbound
{
public:
	MazdaPlantHostInbound();
	virtual ~MazdaPlantHostInbound();

	/**
	 * Get the vehicle build record for the specfied AON.
	 *
	 * @param aon       Vehicle Identifier.
	 * @param buildData Place to store the vehicle build data.
	 * @param updateStatus
	 *                  Flag to indicate if the system status should be updated.
	 *                  The system status should only be updated for build records retrieved for the vehicle to be
	 *                  tested on the roll test machine.
	 *
	 * @return Status of the load operation.
	 */
	virtual const string LoadVehicleBuildRecord(const string &aon, XmlNodeMap &buildData, const bool updateStatus);

	/**
	 * Request to write a data tag
	 *
	 * @param dataNode  Data to be written
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 */
	virtual const string Write(const XmlNode *dataNode);
};
//-------------------------------------------------------------------------------------------------
#endif //MazdaPlantHostInbound_h
