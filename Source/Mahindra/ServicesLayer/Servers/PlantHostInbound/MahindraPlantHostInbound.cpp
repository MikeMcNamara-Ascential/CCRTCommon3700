//******************************************************************************
// Description:
//   This defines the MahindraPlantHostInbound object.
//
//===========================================================================
// Copyright (c) 2007 Burke E. Porter Machinery
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

#include "./Include/MahindraPlantHostInbound.h"

//-----------------------------------------------------------------------------
MahindraPlantHostInbound::MahindraPlantHostInbound() : PlantHostInbound(), m_secondarySelectedData(NULL)
{
}

//-----------------------------------------------------------------------------
MahindraPlantHostInbound::~MahindraPlantHostInbound()
{
}

//-----------------------------------------------------------------------------
void MahindraPlantHostInbound::LoadAdditionalConfigurationItems(const XmlNode *document)
{	// Call the base class to begin the loading
	PlantHostInbound::LoadAdditionalConfigurationItems(document);
	// Get the secondary selection data config data
	try
	{
		m_secondarySelectedData = document->getChild("Setup/DataTags/SecondarySelectionData")->Copy();
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Secondary selection data item not configured: %s", excpt.GetReason());
		m_secondarySelectedData = NULL;
	}
	// Get the manual secondary selection code
	try
	{
		m_manualSecondaryData = document->getChild("Setup/DataTags/ManualSecondaryData")->Copy();
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Secondary selection data item not configured: %s", excpt.GetReason());
		m_manualSecondaryData = NULL;
	}
}

//-----------------------------------------------------------------------------
const string MahindraPlantHostInbound::LoadVehicleBuildFromFile(const string &vin,
																XmlNodeMap &buildData,
																const bool &updateStatus)
{
	XmlParser parser;
	string secSelData;	   // Secondary selection data item
	string buildDataFile;  // Filename containing the build data we need to load
	string status;		   // Status of loading the selected build data
	string response;	   // Response message from the NDB
	string ndbMsg;		   // Message data from the NDB
	string secondarySelectionData = GetDataTag("SecondarySelectionDataTag");
	const XmlNodeMap *manualDataMap = &m_manualSecondaryData->getChildren();

	Log(LOG_FN_ENTRY,"MahindraPlantHostInbound::LoadVehicleBuildFromFile() - Enter");
	try
	{
		if(ReadSubscribeData(GetDataTag("VehicleTypeFromPLC")) != "0" && m_manualSecondaryData != NULL)
		{//PLC Button selected - override any vins scanned
			XmlNodeMapCItr valueIter = manualDataMap->find(string("VehicleType" + ReadSubscribeData(GetDataTag("VehicleTypeFromPLC"))));
			if(valueIter != manualDataMap->end())
			{
				secSelData = valueIter->second->getValue();
			}
			else
			{
				Log(LOG_ERRORS, "Could not find secondary data for Tag");
				secSelData = "Undefined";
			}
			m_broker->Write(secondarySelectionData,secSelData.c_str(),response,true); 
		}
		// Read the secondary selection data that was scanned
		else if(m_broker->Read(secondarySelectionData,response,true) == BEP_STATUS_SUCCESS)
		{
			m_broker->GetNext(secondarySelectionData,secSelData,response);
			Log(LOG_DEV_DATA,"Read %s Response %s",secondarySelectionData.c_str(),secSelData.c_str());
		}
		// Ensure a secondary selection data item is supported
		string charSelectType("range");
		if(m_secondarySelectedData != NULL)
		{
			XmlNodeMapCItr type = m_secondarySelectedData->getAttributes().find("SelectedCharacterType");
			charSelectType = (type != m_secondarySelectedData->getAttributes().end()) ? type->second->getValue() : "";
		}
		// Determine what portion of the secondary selection data to use
		if(!charSelectType.compare("range"))
		{	// Pull a range of characters from the data string
			Log(LOG_DEV_DATA,"%s Start Index: %d, length: %d",
				GetDataTag("SecondarySelectionData").c_str(), VehicleBuildFileVinStart(), VehicleBuildFileVinLength());
			buildDataFile = secSelData.substr(VehicleBuildFileVinStart(),VehicleBuildFileVinLength());
		}
		else if((m_secondarySelectedData != NULL) && (m_secondarySelectedData->getChildren().size() > 0))
		{	// Pull specific characters from the data string
			Log(LOG_DEV_DATA, "Pulling specific characters from the VIN string to get a ZTS code");
			for(XmlNodeMapCItr iter = m_secondarySelectedData->getChildren().begin();
			   iter != m_secondarySelectedData->getChildren().end(); iter++)
			{
				UINT8 index = BposReadInt(iter->second->getValue().c_str());
				buildDataFile += secSelData[(index < secSelData.length()) ? index : 0];
			}
		}
		else
		{	// Just use the VIN that was passed in
			Log(LOG_DEV_DATA, "No secondary selection data item defined, extracting VIN characters - index: %d, length: %d",
				VehicleBuildFileVinStart(), VehicleBuildFileVinLength());
			buildDataFile = vin.substr(VehicleBuildFileVinStart(),VehicleBuildFileVinLength());
		}
		Log(LOG_DEV_DATA, "Extracted characters for file name: %s", buildDataFile.c_str());
		// build the file name
		string buildRecordFile = GetVehicleBuildFilePath() + GetDataTag("SecondarySelectionDataTag") + buildDataFile + ".xml";

		// Get the vehicle build record from file
		Log(LOG_DEV_DATA,"Retrieving build record from %s\n", buildRecordFile.c_str());
		SetVehicleBuild(parser.ReturnXMLDocument(buildRecordFile)->getChild(VEHICLE_BUILD_TAG), buildData);

		// add the secondary selection data item to the build data
		XmlNode secSelNode(GetDataTag("SecondarySelectionDataTag"),secSelData);
		buildData.addNode(secSelNode.Copy());
		XmlNode vinNode(VIN_DATA_TAG, vin);
		buildData.addNode(vinNode.Copy());

		XmlNode wheelbaseInchesX10Node("WheelbasePositionInchesX10",buildData.getNode("Wheelbase")->getValue());
		buildData.addNode(wheelbaseInchesX10Node.Copy());
		buildData.delNode("Wheelbase");

		// Add the tire size settings
		bool largeTire = atob(buildData.getNode("UseLargeTireOpening")->getValue().c_str());
		XmlNode retRollHiNode("RetRollHiPosition", largeTire ? "1" : "0");
		XmlNode retRollLoNode("RetRollLoPosition", largeTire ? "0" : "1");
		buildData.addNode(retRollHiNode.Copy());
		buildData.addNode(retRollLoNode.Copy());

		status = BEP_SUCCESS_RESPONSE;
	}
	catch(...)
	{
		// There was an error getting the build record
		status = BEP_UNAVAILABLE_RESPONSE;
		Log(LOG_ERRORS,"Error getting vehicle build from file");
		ClearVehicleBuild(buildData, updateStatus);
	}

	Log(LOG_FN_ENTRY,"MahindraPlantHostInbound::LoadVehicleBuildFromFile() complete, returning: %s\n", status.c_str());

	return(status);
}

//-----------------------------------------------------------------------------
const string MahindraPlantHostInbound::LoadVehicleBuildRecord(const string &vin, 
															  XmlNodeMap &buildData, 
															  const bool updateStatus)
{	// Clear the vehicle build record before reloading it
	Log(LOG_FN_ENTRY,"Enter CheryPlantHostInbound::LoadVehicleBuildRecord(%s)\n",vin.c_str());
	string status(BEP_ERROR_RESPONSE), response, buildRecord;
	// Invalidate vehicle build since we are about to clear it and reload with a new one
	if(updateStatus) SetVehicleBuildRecordStatus(BEP_INVALID_RESPONSE);
	ClearVehicleBuild(buildData, updateStatus);
	string source = GetVehicleBuildSource();
	if(SOURCE_FILE == source)
	{	// Get the vehicle build record from file
		status = LoadVehicleBuildFromFile(vin, buildData, updateStatus);
	}
	else if(SOURCE_BROADCAST == source)
	{	// Request the vehicle build record from broadcast
		status = LoadVehicleBuildFromBroadcast(vin, buildData, updateStatus);
	}
	// If the vehicle build was retrieved, store it.
	if(status == BEP_SUCCESS_RESPONSE)
	{	// Add information derived from the vehicle build information to the vehicle build record
		AddDerivedBuildInfo(buildData);
		if(updateStatus)
		{	// Set the vehicle build status to valid
			SetVehicleBuildRecordStatus(validStatus);
			m_broker->Write(GetVinReadStatusTag(), READY_TO_TEST, response, true);
		}
		Log("PlantHostInbound::CheryPlantHostInbound - Loaded vehicleBuild with:\n");
		for(XmlNodeMapItr iter = buildData.begin(); iter != buildData.end(); iter++)
			Log("\t%s\n", (*iter).second->ToString().c_str());
	}
	else
	{	// Log that an error occurred loading vehicle build data
		m_broker->Write(GetVinReadStatusTag(), NO_VIN, response, true);
		Log(LOG_ERRORS,"Error Loading VehicleBuild from: %s for data: %s\n", source.c_str(), vin.c_str());
	}
	// Update the Input Server State if needed
	if(updateStatus)
	{	// Update the DataInputServer state
		UpdateInputServerState();
	}
	// Publish the vehicle build to any subscribers
	if(status == BEP_SUCCESS_RESPONSE)
	{
		XmlNode nextVehicleBuild(GetVehicleBuildTag(), "");
		for(XmlNodeMapItr iter = m_vehicleBuild.begin(); iter != m_vehicleBuild.end(); iter++)
		{	// Add the build item to the node
			nextVehicleBuild.addChild(iter->second->Copy());
		}
		// Write the next build data
		Write(&nextVehicleBuild);
		// Get rid of the node
		nextVehicleBuild.clear();
	}
	Log(LOG_FN_ENTRY,"CheryPlantHostInbound::LoadVehicleBuildRecord() returning %s\n", status.c_str());
	return(status);
}

//-----------------------------------------------------------------------------
const string MahindraPlantHostInbound::Publish(const XmlNode *node)
{
	string result = BEP_ERROR_RESPONSE, response, loadResult;
	Log("MahindraPlantHostInbound::Publish(%s)\n", node->ToString().c_str());
	result = BepServer::Publish(node);
	// Only do special stuff if publish was successful
	if(result == BEP_SUCCESS_RESPONSE)
	{
		if(node->getName() == GetVehiclePresentTag())
		{	// Update the DataInputServer state
			bool vehiclePresent = (ReadSubscribeData(GetVehiclePresentTag()) == vehicleIsPresent ? true : false);
			bool retainersDown = (ReadSubscribeData(GetRetainersDownTag()) == retainingRollsDown ? true : false);
			if(!vehiclePresent && retainersDown)
			{//vehicle left update to invalid status
				SetVehicleBuildRecordStatus(BEP_INVALID_RESPONSE);
			}
			UpdateInputServerState();
		}
		else if(!node->getName().compare(GetDataTag("SecondarySelectionDataTag")) ||
				!node->getName().compare(GetNextVinTag()))
		{	// Update the traffic light widget to let driver know vin is being processed
			Log(LOG_FN_ENTRY,"SecondarySelectionDataMatch\n");
			SetVehicleBuildRecordStatus(validStatus);
			UpdateInputServerState();
			Log(LOG_FN_ENTRY,"Return from update state\n");
			m_broker->Write(GetVinReadStatusTag(), PROCESSING_VIN, response, true);
			// Load the vehicle build record for the specified vin
			Log(LOG_FN_ENTRY,"Before Load\n");
			loadResult = LoadVehicleBuildRecord(node->getValue(), m_vehicleBuild, true);
			Log(LOG_FN_ENTRY,"After Load\n");
			if(loadResult == BEP_SUCCESS_RESPONSE)
			{	// Publish the vehicle build to any subscribers
				XmlNode nextVehicleBuild(GetVehicleBuildTag(), "");
				for(XmlNodeMapItr iter = m_vehicleBuild.begin(); iter != m_vehicleBuild.end(); iter++)
				{	// Add the build item to the node
					nextVehicleBuild.addChild(iter->second->Copy());
				}
				// Write the next build data
				Write(&nextVehicleBuild);
				// Get rid of the node
				nextVehicleBuild.clear();
			}
		}
	}
	return(result);
}

//-----------------------------------------------------------------------------
void MahindraPlantHostInbound::UpdateInputServerState(void)
{   // Make sure to keep the input server in VIN state
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
