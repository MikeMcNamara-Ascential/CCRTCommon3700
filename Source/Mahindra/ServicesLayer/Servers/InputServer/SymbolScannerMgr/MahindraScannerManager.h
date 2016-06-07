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
#ifndef MahindraScannerManager_h
#define MahindraScannerManager_h
//-------------------------------------------------------------------------------------------------
#include "SymbolScannerMgr.h"

//-------------------------------------------------------------------------------------------------
class MahindraScannerManager : public SymbolScannerMgr
{
public:
	MahindraScannerManager();
	virtual ~MahindraScannerManager();

    /**
     * Initialization method that parses the command line
     */
    virtual void Initialize(int argc, char *argv[]);

	/**
	 * Initializes the scanner manager to talk to the scanner as well as any data that needs to
	 * be setup before we begin processing inputs.
	 *
	 * @param document Configuration document.
	 */
	virtual void Initialize(const XmlNode *document);




protected:

    /**
     * Process the 2D barcode.
     * The data fields will be parsed out of the scanner data and written to the system.
     * 
     * @param barcodeData
     *                  Barcode data received from the scanner.
     * @param byteCount Number of bytes received from the scanner.
     */
    virtual void ProcessTwoDimensionalBarcode(const SerialString_t &barcodeData, const INT32 &byteCount);



private:

	/**
	 * Get/Set the length of the barcode that indicates if the vehicle is equipped
	 * with RWAL.
	 * 
	 * @param barcodeLength
	 *               RWAL equipped barcode length.
	 * 
	 * @return RWAL equipped barcode length.
	 */
	const INT8& RwalEquippedBarcodeLength(const INT8 *barcodeLength = NULL);

	/**
	 * Get/Set the position of the vehicle type character in the scanned data string.
	 * 
	 * @param position Position of the vehicle type character in the scanned data string.
	 * 
	 * @return Position of the vehicle type character in the scanned data string.
	 */
	const UINT8& VehicleTypePosition(const UINT8 *position = NULL);

    /**
	 * Get/Set the vehicle type length in the scanned data string.
	 * 
	 * @param length Length of the vehicle type in the scanned data string.
	 * 
	 * @return Length of the vehicle type in the scanned data string.
	 */
	const UINT8& VehicleTypeLength(const UINT8 *length = NULL);

	// Position of the vehicle type character in the scanned data.
	UINT8 m_vehicleTypePosition;
    // Length of the vehicle type in the scanned data.
	UINT8 m_vehicleTypeLength;
	// Map of the code to vehicle type data so the VIN can be picked out correctly
	XmlNodeMap m_vehicleTypeCodes;

	// Barcode length for RWAL equipped vehicles
	INT8 m_rwalEquippedBarcodeLength;

};
//-------------------------------------------------------------------------------------------------
#endif  //MahindraScannerManager_h
