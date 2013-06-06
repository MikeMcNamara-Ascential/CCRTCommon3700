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
#include "MazdaPlantHostInbound.h"

//-------------------------------------------------------------------------------------------------
MazdaPlantHostInbound::MazdaPlantHostInbound() : PlantHostInbound()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MazdaPlantHostInbound::~MazdaPlantHostInbound()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string MazdaPlantHostInbound::LoadVehicleBuildRecord(const string &aon, 
														   XmlNodeMap &buildData, 
														   const bool updateStatus)
{   // Break down the AON to be sent to the PLC
	string tagtemplate("AonBytes%dand%d");
	string tag, value;
	string response;
	char buff[32];
	UINT16 tempValue = 0x0000;
	Log(LOG_DEV_DATA, "Sending AON: %s to PLC", aon.c_str());
	for(UINT8 index = 0; index < aon.length(); index += 2)
	{   // Build the new tag to write the data to the PLC
		tag = CreateMessage(buff, sizeof(buff), tagtemplate.c_str(), index, index+1);
		tempValue = ((aon[index] & 0x00FF) << 8);
		tempValue |= (aon[index+1] & 0x00FF);
		value = CreateMessage(buff, sizeof(buff), "%d", tempValue);
		m_broker->Write(tag, value, response, true);
		Log(LOG_DEV_DATA, "Wrote AON data - Tag: %s, Value: %s [%04X]", tag.c_str(), value.c_str(), tempValue);
	}
	m_broker->Write(GetDataTag("RetrieveBuildDataTag"), "1", response, true);
	return BEP_SUCCESS_RESPONSE;
}

//-------------------------------------------------------------------------------------------------
const string MazdaPlantHostInbound::Write(const XmlNode *dataNode)
{
	string response;
	string result = BepServer::Write(dataNode);
	if(!dataNode->getName().compare(GetDataTag("AonTag")) && (m_broker != NULL))
	{   // Update the traffic light widget to let driver know vin is being processed
		m_broker->Write(GetVinReadStatusTag(), PROCESSING_VIN, response, true);
		// Load the vehicle build record for the specified AON
		result = LoadVehicleBuildRecord(dataNode->getValue(), m_vehicleBuild, true);
	}
	return(result);
}
