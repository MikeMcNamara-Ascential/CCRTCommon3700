//*************************************************************************
// FILE DESCRIPTION:
//  The MazdaMapleKeypadMgr handles vehicle data type inputs and publishes 
//  the data to objects that need the data.
//
//===========================================================================
// Copyright (c) 20013- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "INamedDataBroker.h"
#include "MazdaMapleKeypadMgr.h"

//-------------------------------------------------------------------------------------------------
MazdaMapleKeypadMgr::MazdaMapleKeypadMgr() : MapleKeypadManager(), m_inHalfAutoMode(false)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MazdaMapleKeypadMgr::~MazdaMapleKeypadMgr()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string MazdaMapleKeypadMgr::Publish(const XmlNode *node)
{
	Log(LOG_FN_ENTRY, "MazdaMapleKeypadMgr::Publish(tag: %s, value: %s) - Enter", 
		node->getName().c_str(), node->getValue().c_str());
	if(!node->getName().compare(GetDataTag("HalfAutoModeTag")))
	{
		bool inHalfAuto = atob(node->getValue().c_str());
		InHalfAutoMode(&inHalfAuto);
	}
	Log(LOG_FN_ENTRY, "MazdaMapleKeypadMgr::Publish(tag: %s, value: %s) - Exit", 
		node->getName().c_str(), node->getValue().c_str());
	return MapleKeypadManager::Publish(node);
}

//-------------------------------------------------------------------------------------------------
void MazdaMapleKeypadMgr::Run(volatile bool *terminateFlag/*=NULL*/)
{
	unsigned char inputData[GetBufferSize()];
	INT32 byteCount = 0;
	Log(LOG_FN_ENTRY, "MazdaMapleKeypadMgr::Execute() begin");
	// clear the keypad buffer
	memset(inputData, 0, GetBufferSize());
	SetCurrentState(INPUT_SERVER_VIN_STATE);
	// Loop until the server is terminated
	while (GetStatus() != BEP_TERMINATE)
	{   // Get the data from the buffer
		Log(LOG_DEV_DATA,"Waiting for Maple Keypad data");
		byteCount = GetKeypadInput(inputData, GetBufferSize());
		// Check for data
		if (byteCount > 0)
		{   // Check if we are in half auto mode
			if(InHalfAutoMode())
			{
				ProcessHalfAutoModeInputs(inputData, byteCount);
			}
			else
			{   // Evaluate the data received
				EvaluateData(inputData, byteCount);
			}
			// Clear the scanner buffer
			memset(inputData, 0, GetBufferSize());
		}
	}
}

//-------------------------------------------------------------------------------------------------
void MazdaMapleKeypadMgr::LoadAdditionalConfigurationItems(const XmlNode *configNode)
{
	try
	{
		m_halfAutoItems.DeepCopy(configNode->getChild("Setup/HalfAutoInputs")->getChildren());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Half Auto Inputs not defined - %s", excpt.GetReason());
	} 
	MapleKeypadManager::LoadAdditionalConfigurationItems(configNode);
}

//-------------------------------------------------------------------------------------------------
INT32 MazdaMapleKeypadMgr::GetKeypadInput(UINT8 *inputData, INT32 bufferSize)
{
	INT32 byteCount = m_keypadComm.Get(inputData, bufferSize);
	if(byteCount > 0)
	{
		m_keypadComm.GetRemainingSerialData(inputData, bufferSize, byteCount, 
											GetMaximumRetries(), GetDataDelay());
	}
	Log(LOG_DEV_DATA,"Received %d characters from Maple Keypad", byteCount);
	return byteCount;
}

//-------------------------------------------------------------------------------------------------
const bool& MazdaMapleKeypadMgr::InHalfAutoMode(const bool *inHalfAuto /*= NULL*/)
{
	if(inHalfAuto != NULL)  m_inHalfAutoMode = *inHalfAuto;
	return m_inHalfAutoMode;
}

//-------------------------------------------------------------------------------------------------
void MazdaMapleKeypadMgr::ProcessHalfAutoModeInputs(UINT8 *inputData, INT32 byteCount)
{  
	Log(LOG_FN_ENTRY, "MazdaMapleKeypadMgr::ProcessHalfAutoModeInputs() - Enter");
	INamedDataBroker broker;
	string response;
	// We should start with the AON number, so pull it out first
	string aonData = string((char *)&inputData[0], byteCount-1);
	if(aonData.length() == SecondaryDataLength())
	{
		XmlNode buildData(NEXT_VEHICLE_BUILD_TAG, "");
		Log(LOG_DEV_DATA, "Received %s = %s", SecondaryDataType().c_str(), aonData.c_str());
		buildData.addChild(SecondaryDataType(), aonData, ELEMENT_NODE);
		broker.Write("Aon", aonData, response, true);
		// Get the vehicle options from the user
		for(XmlNodeMapCItr iter = m_halfAutoItems.begin(); iter != m_halfAutoItems.end(); iter++)
		{
			bool optionValid = false;
			Log(LOG_DEV_DATA, "Processing build option %s", iter->second->getName().c_str());
			do
			{
				string buildItem = iter->second->getName();
				string promptMessage = iter->second->getAttribute("Prompt")->getValue();
				UINT8 index = 1;
				char buff[32];
				for(XmlNodeMapCItr optionIter = iter->second->getChildren().begin();
				   optionIter != iter->second->getChildren().end(); optionIter++)
				{
					promptMessage += CreateMessage(buff, sizeof(buff), "  %d-%s", index++, 
												   optionIter->second->getValue().c_str());
				}
				ClearKeypadDisplay();
				DisplayMessage(promptMessage, 
							   iter->second->getAttribute("RowIndex")->getValue(),
							   iter->second->getAttribute("ColumnIndex")->getValue());
				PositionCursor(GetDataTag("VINEntryRow"), GetDataTag("VINEntryColumn"));
				EnableBlockMode();
				UINT8 optionInput[GetBufferSize()];
				INT32 optionByteCount = 0;
				do
				{
					optionByteCount = GetKeypadInput(optionInput, GetBufferSize());
				} while((optionByteCount <= 0) && InHalfAutoMode() && (GetStatus() != BEP_TERMINATE));
				if(optionByteCount > 0)
				{
					INT32 optionNumber = atoi(string((char *)&optionInput[0], optionByteCount - 1).c_str()) - 1;
					if(optionNumber <= iter->second->getChildren().size())
					{
						optionValid = true;
						string optionValue = iter->second->getChildren().getNode(optionNumber)->getValue();
						buildData.addChild(buildItem, optionValue, ELEMENT_NODE);
						Log(LOG_DEV_DATA, "Added build item %s = %s", buildItem.c_str(), optionValue.c_str());
					}
				}
			} while(!optionValid && InHalfAutoMode() && (GetStatus() != BEP_TERMINATE));
			Log(LOG_DEV_DATA, "Done processing build option %s", iter->second->getName().c_str());
		}
		Log(LOG_DEV_DATA, "Done getting build options from the operator");
		// Write the build data to the Plant Host Inbound object
		broker.Write(&buildData, response, true);
		Log(LOG_DEV_DATA, "Wrote build data to PlantHostInbound");
		ClearKeypadDisplay();
		DisplayMessage("EnterVIN");
		PositionCursor(GetDataTag("VINEntryRow"), GetDataTag("VINEntryColumn"));
		EnableBlockMode();
	}
	else
	{
		Log(LOG_ERRORS, "Invalid AON entered (%s), length = %d, expected %d", 
			aonData.c_str(), aonData.length(), SecondaryDataLength());
	}
}

