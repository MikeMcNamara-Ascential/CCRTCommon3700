//******************************************************************************
// Description:
//   This defines the JohnDeerePlantHostInbound object.
//
//===========================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log$
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef JohnDeerePlantHostInbound_h
#define JohnDeerePlantHostInbound_h
//==============================================================================

#include "PlantHostInbound.h"

// conversion from millimeters to inches                                   
const float mmToInches = 0.03936996;

/**
 * JohnDeerePlantHostInbound object.
 * This class is responsible for translating broadcast data to a format usable by
 * the system.  It only handles data coming from the plant broadcast system into
 * the Burke Porter system.
 */
class JohnDeerePlantHostInbound : public PlantHostInbound
{
public:
	/**
	 * Class constructor
	 */
	JohnDeerePlantHostInbound();

	/**
	 * Class destructor.
	 */
	virtual ~JohnDeerePlantHostInbound();




protected:

	/**
	 * Load the additional configuration items needed by plant host inbound.
	 * The "standard" items will be handled by the call to
	 * BepServer::Initialize().
	 *
	 * @param document A pointer to a parsed configuration file.
	 */
	virtual void LoadAdditionalConfigurationItems(const XmlNode *document);

	/**
	 * Returns the build record for the requested vin.
	 *
	 * @param vin          Vin of the vehicle to be tested.
	 * @param buildData    Place to store the build data retrieved.
	 * @param updateStatus Flag to indicate if system status or prompts should 
	 *                     be updated.
	 * 
	 * @return Status of the operation (Success, Unavailable,...)
	 */
	virtual const string LoadVehicleBuildFromFile(const string &vin, 
												  XmlNodeMap &buildData, 
												  const bool &updateStatus);
	virtual const string LoadVehicleBuildRecord(const string &vin, XmlNodeMap &buildData, const bool updateStatus);

	/**
	 * Handle the subscribed for data by the process.
	 *
	 * @param node    Tag that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 */
	virtual const string Publish(const XmlNode *node);

	/**
	 * Update the state of the InputServer if it needs to be changed.
	 */
	virtual void UpdateInputServerState(void);



private:

	/**
	 * Secondary build file selection data.
	 */
	XmlNode *m_secondarySelectedData;

	/**
	 * Manual Secondary selection data.
	 */
	XmlNode *m_manualSecondaryData;
};

#endif // JohnDeerePlantHostInbound_h
