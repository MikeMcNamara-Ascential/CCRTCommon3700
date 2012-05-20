//*************************************************************************
// FILE DESCRIPTION:
//  The Mahindra Scanner Manager handles all scanner inputs and writes the data to
//  the InputServer for publishing to requesting objects..
//
//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MahindarScannerManager.h"

//-------------------------------------------------------------------------------------------------
MahindraScannerManager::MahindraScannerManager() : SymbolScannerMgr()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MahindraScannerManager::~MahindraScannerManager()
{   // Clear the vehicle type codes
	m_vehicleTypeCodes.clear(true);
}

//-------------------------------------------------------------------------------------------------
void MahindraScannerManager::Initialize(const XmlNode *document)
{   // Call the base class to begin the initialization
	SymbolScannerMgr::Initialize(document);
	Log(LOG_FN_ENTRY, "MahindraScannerManager::Initialize() - Enter");
	// Store the vehicle type character position
	position = 0;
	try
	{
		position = BposReadInt(document->getChild("Setup/Parameters/VehicleTypePosition")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Vehicle type character position not defined, using 17: %s", excpt.GetReason());
		position = 17;
	}
	VehicleTypePosition(&position);
	// Load the vehicle type codes
	try
	{
		m_vehicleTypeCodes.DeepCopy(document->getChild("Setup/Parameters/VehicleCodes")->getChildren());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Vehicle type codes not defined!  %s", excpt.GetReason());
		m_vehicleTypeCodes.clear(true);
	}
	Log(LOG_FN_ENTRY, "MahindraScannerManager::Initialize() - Exit");
}

//-------------------------------------------------------------------------------------------------
void MahindraScannerManager::ProcessTwoDimensionalBarcode(const SerialString_t &barcodeData, 
														  const INT32 &byteCount)
{   // First determine which vehicle type this is
	string code(barcodeData[VehicleTypePosition()]);
	XmlNodeMapItr iter = m_vehicleTypeCodes.find(code);
	string vehicleType;
	if(iter != m_vehicleTypeCodes.end())
	{   // Found the type listed
		vehicleType = iter->second->getValue();
	}
	else
	{   // No special type for this code, use the default
		vehicleType = m_vehicleTypeCodes.getNode("Default")->getValue();
	}
	Log(LOG_DEV_DATA, "Found vehicle type code %s, processing data for %s", 
		code.c_str(), vehicleType.c_str());
	// Get the VIN data from the scan
	XmlNodeMapItr vehicleIter = m_twoDimensionBarcodeItems.find(vehicleType);
	if(vehicleIter != m_twoDimensionBarcodeItems.end())
	{
		UINT8 startIndex = BposReadInt(vehicleIter->second->getAttribute("StartIndex")->getValue().c_str());
		INT8  length = BposReadInt(vehicleIter->second->getAttribute("Length")->getValue().c_str());
		string vin;
		if(length > 0)
		{
			vin = barcodeData.substr(startIndex, length);
		}
		else
		{
			vin = barcodeData.substr(startIndex);
		}
		// Write the VIN
		string response;
		Log(LOG_DEV_DATA, "Writing VIN %s for vehicle type %s", vin.c_str(), vehicleType.c_str());
		m_dataBroker->Write(NEXT_VIN_DATA_TAG, vin, response, true);
	}
	else
	{
		Log(LOG_ERRORS, "VIN location data not defined for %s", vehicleType.c_str());
	}
}

//-------------------------------------------------------------------------------------------------
const UINT8& MahindraScannerManager::VehicleTypePosition(const UINT8 *position /*= NULL*/)
{
	if(position != NULL)   m_vehicleTypePosition = *position;
	return m_vehicleTypePosition;
}
