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





protected:

	/**
	 * Add any information that was derived from the vehicle build to the vehicle build record.
	 *
	 * @param buildData Build data map to add the information to.
	 */
	virtual void AddDerivedBuildInfo(XmlNodeMap &buildData);

	/**
	 * Create the vehicle build record with the ALC data.
	 * 
	 * @return Result of creating the build record.
	 */
	string CreateBuildRecord(void);

	/**
	 * Evaluates the criteria to determine which wheelbase position to select.
	 *
	 * @param selectData Data used to determine which wheelbase to select
	 * @param modelYear  Model year character from the VIN
	 * @return Wheelbase position for the specified selectData.
	 */
	virtual const string GetWheelBasePosition(const string &selectData, const string &modelYear);

	/**
	 * Load the additional configuration items needed by plant host inbound.
	 * The "standard" items will be handled by the call to
	 * BepServer::Initialize().
	 *
	 * @param document A pointer to a parsed configuration file.
	 */
	virtual void LoadAdditionalConfigurationItems(const XmlNode *document);

	/**
	 * Handle the subscribed for data by the process.
	 *
	 * @param node    Tag that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 */
	virtual const string Publish(const XmlNode *node);




private:

	// System tags to read to get build data
	XmlNodeMap m_buildDataTags;

};
//-------------------------------------------------------------------------------------------------
#endif //MazdaPlantHostInbound_h
