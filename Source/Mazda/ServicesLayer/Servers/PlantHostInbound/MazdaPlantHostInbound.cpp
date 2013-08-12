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
{   
	Log(LOG_DEV_DATA, "MazdaPlantHostInbound::LoadVehicleBuildRecord - Getting vehicle build from %s", 
		GetVehicleBuildSource().c_str());
	if(!GetVehicleBuildSource().compare(SOURCE_BROADCAST))
	{
		LoadVehicleBuildFromAlc(aon);
	}
	else
	{
		string response;
		if(BEP_SUCCESS_RESPONSE == LoadVehicleBuildFromFile(aon, buildData, true))
		{
			SetVehicleBuildRecordStatus(validStatus);
			m_broker->Write(GetVinReadStatusTag(), READY_TO_TEST, response, true);
		}
		else
		{   // No file exists, try to get it from ALC
			LoadVehicleBuildFromAlc(aon);
		}
	}
	return BEP_SUCCESS_RESPONSE;
}

//-------------------------------------------------------------------------------------------------
const string MazdaPlantHostInbound::Register(void)
{   // Call the base class to register the server
	string result = PlantHostInbound::Register();
	// Make sure the input server is in VIN mode
	UpdateInputServerState();
	return result;
}

//-------------------------------------------------------------------------------------------------
const string MazdaPlantHostInbound::Write(const XmlNode *dataNode)
{
	string response;
	Log(LOG_DEV_DATA, "MazdaPlantHostInbound::Write(Tag: %s, AON Tag: %s) - Enter",
		dataNode->getName().c_str(), GetDataTag("AonTag").c_str());
	string result = BepServer::Write(dataNode);
	if(!dataNode->getName().compare(GetDataTag("AonTag")) && (m_broker != NULL))
	{   // Update the traffic light widget to let driver know vin is being processed
		m_broker->Write(GetVinReadStatusTag(), PROCESSING_VIN, response, true);
		// Load the vehicle build record for the specified AON
		result = LoadVehicleBuildRecord(dataNode->getValue(), m_vehicleBuild, true);
	}
	else if(!dataNode->getName().compare(NEXT_VEHICLE_BUILD_TAG))
	{
		m_broker->Write(GetVinReadStatusTag(), PROCESSING_VIN, response, true);
		m_vehicleBuild.clear(true);
		m_vehicleBuild.DeepCopy(dataNode->getChildren());
		// Now add the derived data
		AddDerivedBuildInfo(m_vehicleBuild);
		m_broker->Write(GetVinReadStatusTag(), READY_TO_TEST, response, true);
		SetVehicleBuildRecordStatus(validStatus);
	}
	else if(m_broker == NULL)
	{
		Log(LOG_DEV_DATA, "INDB object is null!!");
	}
	return(result);
}

//-------------------------------------------------------------------------------------------------
void MazdaPlantHostInbound::AddDerivedBuildInfo(XmlNodeMap &buildData)
{   // Set the wheelbase
	string vin(GetVehicleBuildItem("VIN", buildData));
	INT8 myDigit = BposReadInt(GetDataTag("VinModelYearDigit").c_str());
	string modelYear(vin.substr(myDigit, 1));
	string wb = GetWheelBasePosition(GetVehicleBuildItem("BodyStyle", buildData), modelYear);
	AddVehicleBuildItem(GetWheelbasePositionTag(), wb, buildData);
	Log(LOG_DEV_DATA, "Added wheelbase to build data: %s = %s", GetWheelbasePositionTag().c_str(), wb.c_str());
}

//-------------------------------------------------------------------------------------------------
string MazdaPlantHostInbound::CreateBuildRecord(void)
{   // Get all the data and add it to the build record
	m_vehicleBuild.clear(true);
	string result(BEP_SUCCESS_RESPONSE);
	string response;
	string itemValue;
	if(m_broker != NULL)
	{
		for(XmlNodeMapCItr iter = m_buildDataTags.begin(); 
			 (iter != m_buildDataTags.end()) && !result.compare(BEP_SUCCESS_RESPONSE); iter++)
		{   // Read the data from the system
			if(m_broker->Read(iter->second->getValue(), response, true) == BEP_STATUS_SUCCESS)
			{   // Add the data to the build record
				if(BEP_STATUS_SUCCESS == m_broker->GetByTag(iter->second->getValue(), itemValue, response))
				{   // Add the data to the build record
					string buildTag(iter->second->getAttribute("BuildTag")->getValue());
					if(!buildTag.compare(BODY_STYLE_TAG))
					{   // Need to translate this first
						int ibs = atoi(itemValue.c_str());
						char bs1 = (ibs & 0xFF00) >> 8;
						char bs2 = (ibs & 0x00FF);
						itemValue = string(1, bs1);
						itemValue += string(1, bs2);
					}
					else if(!buildTag.compare(VIN_DATA_TAG))
					{
						m_broker->Write(VINDISPLAY_DATA_TAG, itemValue, response, true);
					}
					else if(!buildTag.compare("Transmission"))
					{
						itemValue = atob(itemValue.c_str()) ? "Automatic" : "Manual";
						Log(LOG_DEV_DATA, "Set transmission type to %s", itemValue.c_str());
					}
					AddVehicleBuildItem(buildTag, itemValue, m_vehicleBuild);
					Log(LOG_DEV_DATA, "Added build item: %s = %s", buildTag.c_str(), itemValue.c_str());
				}
			}
			else
			{
				Log(LOG_ERRORS, "Could not gat value for build item %s", iter->second->getValue().c_str());
				result = BEP_STATUS_FAILURE;
			}
		}
		// Now add the derived data
		AddDerivedBuildInfo(m_vehicleBuild);
	}
	return result;
}

//-------------------------------------------------------------------------------------------------
const string MazdaPlantHostInbound::GetWheelBasePosition(const string &selectData, const string &modelYear)
{
	Log(LOG_FN_ENTRY, "MazdaPlantHostInbound::GetWheelBasePosition()");
	// Reload the wheelbase table in case of edits
	LoadWheelbasePositionTable();
	string bodyStyleMask, selectWheelBase, response, wheelbaseTag;
	string modelYearMask;
	UINT32 ii = 0, weight = 0, selectWeight = 0;
	bool charPositionMatch = false;
	// Set the selected wheel base to the current wheelbase
	Log(LOG_DEV_DATA, "Reading current wheelbase position, tag:%s", GetWheelbasePositionTag().c_str());
	m_broker->Read(GetWheelbasePositionTag(), response, true);
	m_broker->GetNext(wheelbaseTag, selectWheelBase, response);
	// Loop through the list of wheel bases
	for(XmlNodeMapItr iter = m_wheelBasePositions.begin(); iter != m_wheelBasePositions.end(); iter++)
	{   // Calculate the weight of the specified body style to the current wheelbase
		bodyStyleMask = iter->second->getAttributes().getNode("BodyStyle")->getValue();
		try
		{
			modelYearMask = iter->second->getAttributes().getNode("ModelYear")->getValue();
		}
		catch(XmlException &excpt)
		{
			modelYearMask = "????";
		}

		Log(LOG_DEV_DATA, "Evaluating mask - Body style:%s, Model Year: %s", bodyStyleMask.c_str(), modelYearMask.c_str());
		// Check if the model year is a match before evaluating the body style
		if (!modelYear.compare(modelYearMask) || !modelYearMask.compare("????") || !modelYearMask.compare("?"))
		{
			weight = 0;
			ii = 0;
			do
			{   // If the current mask character is ? or characters match, keep checking
				charPositionMatch = (bodyStyleMask[ii] == '?') || (bodyStyleMask[ii] == selectData[ii]);
				// Determine if the weight needs to be updated
				if(charPositionMatch && (bodyStyleMask[ii] == selectData[ii])) weight++;
			} while(charPositionMatch && (ii++ < selectData.length()));
			// Evaluate the weight of this wheelbase position
			Log(LOG_DEV_DATA, "weight: %d\n", weight);
			if(charPositionMatch && (weight > selectWeight))
			{   // Save the current weight and wheelbase position
				selectWeight = weight;
				selectWheelBase = iter->second->getAttributes().getNode("Wheelbase")->getValue();
			}
			Log(LOG_DEV_DATA, "Select Weight: %d, Select Wheelbase(x10): %s\n", selectWeight, selectWheelBase.c_str());
		}
	}
	Log(LOG_FN_ENTRY, "WorkCellController::GetWheelBasePosition() done, returning: %s\n", selectWheelBase.c_str());
	return(selectWheelBase);
}

//-------------------------------------------------------------------------------------------------
void MazdaPlantHostInbound::LoadAdditionalConfigurationItems(const XmlNode *document)
{   // Begin by loading the items from the base class
	PlantHostInbound::LoadAdditionalConfigurationItems(document);
	// Store the vehicle build item tags
	try
	{
		m_buildDataTags.DeepCopy(document->getChild("Setup/BuildDataTags")->getChildren());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not store system tags to use for creating build records: %s", excpt.GetReason());
		m_buildDataTags.clear(true);
	}
	// Store the name of the wheelbase file
	string wbName = "";
	try
	{
		wbName = getenv("USR_ROOT") + document->getChild("Setup/WheelbaseConfigFile")->getValue();
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Error getting wheelbase table file name - %s", excpt.GetReason());
		wbName = "";
	}
	WheelbaseFileName(&wbName);
}

//-------------------------------------------------------------------------------------------------
const string MazdaPlantHostInbound::Publish(const XmlNode *node)
{
	Log(LOG_FN_ENTRY, "MazdaPlantHostInbound::Publish(tag: %s, value: %s) - Enter", 
		node->getName().c_str(), node->getValue().c_str());
	string result = PlantHostInbound::Publish(node);
	string response;
	// Determine if there is any special handling for this tag
	if(!node->getName().compare(GetDataTag("VehicleBuildOk")) && atob(node->getValue().c_str()))
	{   
		m_broker->Write(GetDataTag("RetrieveBuildDataTag"), "0", response, true);
		// Create the build record from the ALC data
		result = CreateBuildRecord();
		// Set build read status OK and clear the bit for retrieve AON
		string buildStatus = !result.compare(BEP_SUCCESS_RESPONSE) ? READY_TO_TEST: NO_VIN;
		m_broker->Write(GetVinReadStatusTag(), buildStatus, response, true);
		SetVehicleBuildRecordStatus(!buildStatus.compare(READY_TO_TEST) ? validStatus : BEP_INVALID_RESPONSE);
	}
	else if(!node->getName().compare(GetDataTag("VehicleBuildError")) && atob(node->getValue().c_str()))
	{   // Set build read status error and clear the bit for retrieve AON
		m_broker->Write(GetVinReadStatusTag(), NO_VIN, response, true);
		m_broker->Write(GetDataTag("RetrieveBuildDataTag"), "0", response, true);
		// Display vin read error
		m_promptComm.DisplayPrompt(2, GetDataTag("BuildDataError"), response);
		SetVehicleBuildRecordStatus(BEP_INVALID_RESPONSE);
	}
	Log(LOG_FN_ENTRY, "MazdaPlantHostInbound::Publish(tag: %s, value: %s) - Exit", 
		node->getName().c_str(), node->getValue().c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
void MazdaPlantHostInbound::UpdateInputServerState(void)
{   // Always want the input server in VIN state
	string response;
	if(m_inputServerComm.Read(GetInputServerStateTag(), response, true) == BEP_STATUS_SUCCESS)
	{
		string currentState("Unknown");
		m_inputServerComm.GetByTag(GetInputServerStateTag(), currentState, response);
		if(currentState.compare(INPUT_SERVER_VIN_STATE))
		{   // Input server is not in VIN state, send it there
			m_inputServerComm.Write(GetInputServerStateTag(), INPUT_SERVER_VIN_STATE, response, true);
			Log(LOG_DEV_DATA,"Set InputServer state to %s\n", INPUT_SERVER_VIN_STATE);
		}
	}
}

//-------------------------------------------------------------------------------------------------
void MazdaPlantHostInbound::LoadVehicleBuildFromAlc(const string &aon)
{
	string tagtemplate("AonBytes%dand%d");
	string tag, value;
	string response;
	char buff[32];
	UINT16 tempValue = 0x0000;
	Log(LOG_DEV_DATA, "Sending AON: %s to PLC", aon.c_str());
	// Break down the AON to be sent to the PLC
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
}

//-------------------------------------------------------------------------------------------------
void MazdaPlantHostInbound::LoadWheelbasePositionTable(void)
{
	XmlParser parser;
	try
	{
		const XmlNode *wheelbaseConfig = parser.ReturnXMLDocument(WheelbaseFileName());
		SetWheelBasePositions(wheelbaseConfig->getChild("VehicleConfig"));
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Error loading wheelbase position table - %s", excpt.GetReason());
	}
	catch(BepException &excpt)
	{
		Log(LOG_ERRORS, "BepException loading wheelbase position table from file - %s", excpt.GetReason());
	}
}

//-------------------------------------------------------------------------------------------------
string MazdaPlantHostInbound::WheelbaseFileName(const string *fileName /*= NULL*/)
{
	if(fileName != NULL)   m_wheelbaseFileName = *fileName;
	return m_wheelbaseFileName;
}
