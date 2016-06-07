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
#include "MahindraScannerManager.h"

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
void MahindraScannerManager::Initialize(int argc, char *argv[])
{
	InputDeviceBase::Initialize(argc, argv);
}

//-------------------------------------------------------------------------------------------------
void MahindraScannerManager::Initialize(const XmlNode *document)
{   // Call the base class to begin the initialization
	SymbolScannerMgr::Initialize(document);
	Log(LOG_FN_ENTRY, "MahindraScannerManager::Initialize() - Enter");
	// Store the rwal equipped data length
	INT8 bcodeLength = 0;
	try
	{ 
		bcodeLength = BposReadInt(document->getChild("Setup/Parameters/RWalEquippedBarcodeLength")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_DEV_DATA, "RWAL Barcode length not specified, no RWAL vehicles will be identified - %s", excpt.GetReason());
		bcodeLength = -1;
	}
	RwalEquippedBarcodeLength(&bcodeLength);
	// Store the vehicle type character position
	UINT8 position = 0;
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
    // Store the vehicle type length
	UINT8 length = 0;
	try
	{
		length = BposReadInt(document->getChild("Setup/Parameters/VehicleTypeLength")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Vehicle type character position not defined, using 20: %s", excpt.GetReason());
		length = 20;
	}
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
{   
    Log(LOG_DEV_DATA, "MahindraScannerManager:ProcessTwoDimensionalBarcode - Enter");
    // Determine if the vehicle is equipped with RWAL
	string response;
	bool isRwalEquipped = (barcodeData.length() >= RwalEquippedBarcodeLength());
	m_dataBroker->Write(GetDataTag("RwalEquippedTag"), isRwalEquipped ? "1" : "0", response, true);
	Log(LOG_DEV_DATA, "RWAL Equipped: %s", isRwalEquipped ? "True" : "False");
	// Next determine which vehicle type this is
    if(barcodeData.length() >= VehicleTypePosition())
    {
	    string code((char *)&barcodeData[VehicleTypePosition()], 1);
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
		    string vin, model;
		    if(length < 0)
		    {
			    length = barcodeData.length() - startIndex + 1;
		    }
		    if(byteCount - m_vehicleTypePosition > 0)
            {
                vin = string((char *)&barcodeData[startIndex], length);
                model = string((char *)&barcodeData[m_vehicleTypePosition], byteCount - m_vehicleTypePosition);
		        // Write the VIN
		        Log(LOG_DEV_DATA, "Writing VIN %s for vehicle type %s", vin.c_str(), vehicleType.c_str());
		        m_dataBroker->Write(NEXT_VIN_DATA_TAG, vin, response, true);
		        m_dataBroker->Write(VINDISPLAY_DATA_TAG, vin, response, true);
		        m_dataBroker->Write(GetDataTag("ZtsCodeTag"), vin, response, true);
                Log(LOG_DEV_DATA, "Writing model code %s", model.c_str());
                m_dataBroker->Write("ModelCode", model, response, true);
            }
            else
                Log(LOG_ERRORS, "Barcode format invalid: length:%d VehicleTypePosition:%d, ignoring scan.\n", byteCount, m_vehicleTypePosition);
	     }
	    else
	    {
		    Log(LOG_ERRORS, "VIN location data not defined for %s", vehicleType.c_str());
	    }
    }
    else
        Log(LOG_ERRORS, "Cannot find vehicle type char: length:%d VehicleTypePosition:%d, ignoring scan.\n", byteCount, m_vehicleTypePosition);

    Log(LOG_ERRORS, "MahindraScannerManager:ProcessTwoDimensionalBarcode - Exit");
}

//-------------------------------------------------------------------------------------------------
const INT8& MahindraScannerManager::RwalEquippedBarcodeLength(const INT8 *barcodeLength /*= NULL*/)
{
	if(barcodeLength != NULL)   m_rwalEquippedBarcodeLength = *barcodeLength;
	return m_rwalEquippedBarcodeLength;
}

//-------------------------------------------------------------------------------------------------
const UINT8& MahindraScannerManager::VehicleTypePosition(const UINT8 *position /*= NULL*/)
{
	if(position != NULL)   m_vehicleTypePosition = *position;
	return m_vehicleTypePosition;
}

//-------------------------------------------------------------------------------------------------
const UINT8& MahindraScannerManager::VehicleTypeLength(const UINT8 *length /*= NULL*/)
{
	if(length != NULL)   m_vehicleTypeLength = *length;
	return m_vehicleTypeLength;
}
