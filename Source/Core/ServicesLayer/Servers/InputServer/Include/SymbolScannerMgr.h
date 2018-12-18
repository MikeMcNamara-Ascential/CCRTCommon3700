//*************************************************************************
// FILE DESCRIPTION:
//  The SymbolScannerMgr handles all scanner inputs and writes the data to
//  the InputServer for publishing to requesting objects..
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/Include/SymbolScannerMgr.h $
// 
// 5     10/30/07 4:09p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     2/22/07 11:09a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added m_maxVinLength.
// 
// 3     6/07/06 5:36p Cward
// Changes to allow the Input Server, CLV295ScannerMgr,
// MapleKeypadManager, PendantManager, and SymbolScannerMgr configurations
// to be reloaed without restrting the processes.
//
// 1     6/05/06 3:46p Rwiersem
// Added LoadAdditionalConfigurationItems() which is used to reload the
// configuration without restarting the process.
//
// 2     4/13/06 4:15a Cward
//  Added an INamedDataBroker object to write NEXT_VIN_TAG to the task
// that handles it (not necessarily the InputServer)
//
// 2     3/20/06 10:59a Bmeinke
// Added an INamedDataBroker object to write NEXT_VIN_TAG to the task that
// handles it (not necessarily the InputServer)
//
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
// 4     4/29/05 8:00a Gpattison
// Refactored the entire SymbolScanner Manager so that the InputServer could
// spawn it as an individual input device instead of having it as an object.
// This allows it to be added or removed from the system by making
// only configuration file changes.  It is now a stand alone BepServer type
// executable.
//
// 4     4/07/05 5:30p Gpattiso
// Changed the data string termination from only using a Carriage Return,
// to support the use of any character based on configuration parameters.
// Change should be backwards compatible.
//
// 3     5/25/04 4:23p Cward
// Added functionality to correct bad scanner data if bad data is
// consistently received in a specified location.
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:42p Cward
//
// 1     10/07/03 11:32a Khykin
// Initial checkin
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:24p Khykin
// Initial checkin.
//
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:47  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:10a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 2     6/13/02 2:57p Dan.erickson
// Added JavaDoc comment for class.
//
// 1     5/31/02 3:04p Dan.erickson
// Created and Debugged.
//
//*************************************************************************
#ifndef SymbolScannerMgr_h
#define SymbolScannerMgr_h
//===========================================================================

#include "InputDeviceBase.h"
#include "SerialChannel.h"
#include "INamedDataBroker.h"

#define CR   '\x00D'

const INT32 maxRetries = 3;

/**
 * The SymbolScannerMgr class manages the Symbol hand held scanners.
 * It retrieves data from the scanner buffer and sends VIN data and
 * Driver Number data to the InputServer for display on the GUI and
 * for use throughout the system.
 *
 * @author Dan Erickson
 * @version Version 1.3
 * @since Version 1.0
 */
class SymbolScannerMgr : public InputDeviceBase
{
    friend class SymbolScannerMgrTest;
public:
    /**
     * Class constructor.
     */
    SymbolScannerMgr();
    /**
     * Default class destructor.
     * @since Version 1.0
     */
    virtual ~SymbolScannerMgr();
    /**
     * Initializes the scanner manager to talk to the scanner as well as any data that needs to
     * be setup before we begin processing inputs.
     *
     * @param document Configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
     *         BEP_SUCCESS_RESPONSE == Registered
     * @since Version 1.0
     */
    virtual const std::string Register(void);
    /**
     * Main Run method to control the process
     *
     * @param terminateFlag
     *               Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
    /**
     * Return the size of the scanner buffer in bytes.
     *
     * @return Size of the scanner buffer.
     * @since Version 1.0
     */
    const INT32 GetScannerBufferSize(void);
    /**
     * Get the time to delay between checks for new data at the serial port.
     *
     * @return Time to delay in ms.
     * @since Version 1.0
     */
    const INT32 GetDataDelay(void);
    /**
     * Get the maximum number of retries allowed.
     *
     * @return Number of retries.
     * @since Version 1.0
     */
    INT16 GetMaxRetries(void);
    /**
     * Determine if a data terminator signifies the end of the data.
     *
     * @return True - Data terminator return signifies end of data.
     * @since Version 1.2
     */
    const bool& LookForTerminationCharacter(void);

protected:
    /**
     * Load the additional configuration items needed by the SymbolScannerMgr.
     * The "standard" items will be handled by the call to
     * InputDeviceBase::Initialize().
     *
     * @param configNode A pointer to the Pendant config node
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *configNode);
    /**
     * Determines whether a VIN or DriverNumber was scanned.  The data is then extracted from the
     * array and written to the InputServer.
     *
     * @param data   Scanner data.
     * @param byteCount Number of valid bytes in the buffer.
     * @since Version 1.0
     */
    virtual void EvaluateData(SerialString_t &data, const INT32 &byteCount);
    /**
     * Store the size of the scanner buffer.
     *
     * @param size   Size of the buffer in bytes.
     * @since Version 1.0
     */
    void SetScannerBufferSize(const INT32 &size);
    /**
     * Store the size of the scanner buffer.
     *
     * @param size   Size of the buffer in bytes.
     * @since Version 1.0
     */
    void SetScannerBufferSize(const std::string &size);
    /**
     * Store the time to wait between checks for new data at the serial port.
     *
     * @param delay  Delay time in ms.
     * @since Version 1.0
     */
    void SetDataDelay(const INT32 &delay);
    /**
     * Store the time to wait between checks for new data at the serial port.
     *
     * @param delay  Delay time in ms.
     * @since Version 1.0
     */
    void SetDataDelay(const std::string &delay);
    /**
     * Set the maximum number of retires to attemp to get a data item.
     *
     * @param retries Maximum number of retries.
     * @since Version 1.0
     */
    void SetMaximumRetries(const INT16 &retries);
    /**
     * Set the maximum number of retires to attemp to get a data item.
     *
     * @param retries Maximum number of retries.
     * @since Version 1.0
     */
    void SetMaximumRetries(const std::string &retries);
    /**
     * Store the flag indicating if we need to look for a data termination
     * character at the end of the scanned data.
     *
     * @param lookForTerminator  Flag to indicate if a data terminator
     *                               signifies the end of the data stream.
     * @since Version 1.2
     */
    void StoreLookForDataTerminator(const bool &lookForTerminator);
    /**
     * Store the flag indicating if bad scanner data
     * should be corrected prior to extracting the data
     * from the buffer.
     *
     * @param correctBadData
     *               Flag to indicate if bad scanner data should be corrected.
     * @since Version 1.3
     */
    void CorrectBadScannerData(const bool &correctBadData);
    /**
     * Determine if bad scanner data should be corrected
     * prior to extracting data from the buffer.
     *
     * @return True - Correct bad scanner data.
     * @since Version 1.3
     */
    const bool &CorrectBadScannerData(void);
    /**
     * Store the index where the bad scanner data occurs.
     *
     * @param index  Index of bad scanner data.
     * @since Version 1.3
     */
    void StoreBadDataIndex(const INT16& index);
    /**
     * Get the index of the bad scanner data.
     *
     * @return Index where the bad scanner data occurs.
     * @since Version 1.3
     */
    const INT16& BadDataIndex(void);
    /**
     * Store the character to use when replacing the bad
     * scanner data.
     *
     * @param data   Bad data replacement value.
     * @since Version 1.3
     */
    void BadDataReplacement(const UINT8& data);
    /**
     * Get the data to use to replace bad scanner data.
     *
     * @return Value to use to replace bad scanner data.
     */
    const UINT8& BadDataReplacement(void);
    /**
     * Store the data that indicates we received bad scanner data.
     *
     * @param indicator Bad data indicator.
     */
    void BadDataIndicator(const UINT8& indicator);
    /**
     * Get the data value that indicates bad data was
     * received from the scanner.
     *
     * @return Bad data indicator.
     */
    const UINT8& BadDataIndicator(void);
    /**
     * Allow testing software to pass in a SerialChannel object
     * to provide a method for dummying out the object so that
     * the mgr does not try and connect to a communication obj
     * that does not exist during a unit test
     *
     * @param commObj The dummy SerialChannel obj
     */
    void SetDummyScannerCommObject(SerialChannel *commObj);

    /**
     * Process the 2D barcode.
     * The data fields will be parsed out of the scanner data and written to the system.
     * 
     * @param barcodeData
     *                  Barcode data received from the scanner.
     * @param byteCount Number of bytes received from the scanner.
     */
    virtual void ProcessTwoDimensionalBarcode(const SerialString_t &barcodeData, const INT32 &byteCount);

	/** Items contained in the two dimension barcode */
	XmlNodeMap m_twoDimensionBarcodeItems;

	/**
	 * Interface to NamedDataBroker process.
	 * Used to read and write data in the system.
	 */
	INamedDataBroker    *m_dataBroker;



private:

    /**
     * Get/Set the flag indicating if 2D barcodes are in use.
     * 
     * @param isTwoDimension
     *               Flag indicating if 2D barcodes are in use.
     * 
     * @return Flag indicating if 2D barcodes are in use.
     */
    const bool& UseTwoDimensionBarcode(const bool *isTwoDimension = NULL);
    /**
     * Serial Comm object to talk to the Scanner.
     */
    SerialChannel *m_scannerComm;
    /**
     * Size of the scanner buffer.
     */
    INT32 m_scannerBufferSize;
    /**
     * Time in ms to wait between checks for scan data.
     */
    INT32 m_dataDelay;
    /**
     * Number of times to attempt to get data from the serial port.
     */
    INT16 m_maxRetries;
    /**
     * Index of the bad scanner data character.
     */
    INT16 m_badDataPosition;
    /**
     * Character to replace bad scanner data.
     */
    UINT8 m_badCharacterReplacement;
    /**
     * Value that indicates bad data was sent by the scanner.
     */
    UINT8 m_badDataIndicator;
    /**
     * Flag to indicate if the scanner will append a data termination character
     * to the end of the scanned data to serve as a terminator.
     *
     */
    bool m_lookForTerminator;
    /**
     * Flag to indicate if "bad" scanner data should be corrected.
     */
    bool m_correctBadScannerData;
    /** Flag to indicate if a 2D barcode is being used */
    bool m_twoDimensionBarcode;
    /**
     * Store the termination character for the scanner input
     */
    char m_terminationCharacter;
    /**
     * Parser for the configuration file. This is protected due to
     * non supported feature in gcc 2.95 when we go to 3.0 this
     * can go private.
     */
    XmlParser    m_parser;
    /**
     * Value of the Vin length
     */
    INT32 m_vinLength;
    /**
     * Value of the Maximum Vin length
     */
    INT32 m_maxVinLength;
    /**
     * Value of the driver number length
     */
    INT32 m_driverNumberLength;

    /**
     * Get/Set the flag indicating if secondary data is supported.
     * 
     * @param dataSupported
     *               Flag indicating if secondary data is supported.
     * 
     * @return Flag indicating if secondary data is supported.
     */
    const bool& SecondaryDataSupported(const bool *dataSupported = NULL);
    /**
     * Get/Set the string denoting the type of the secondary data.  The string will also be used to
     * publish the data to the system.
     * 
     * @param dataType Type/Tag of the secondary data.
     * 
     * @return Type/Tag of the secondary data.
     */
    const string& SecondaryDataType(const string *dataType = NULL);
    /**
     * Get/Set the length of the secondary data.
     * 
     * @param length Length of the secondary data.
     * 
     * @return Length of the secondary data.
     */
    const INT32& SecondaryDataLength(const INT32 *length = NULL);
    /**
     * Flag indicating if secondary data is supported.
     */
    bool m_secondaryDataSupport;
    /**
     * Data tag to use to publish the secondary data item.
     */
    string m_secondaryDataTag;
    /**
     * Length of the secondary data.
     */
    INT32 m_secondaryDataLength;
    /**
     * Position of the index in Toyota barcode scans
     */
    INT32 m_vinStartIndex;
    /**
     * Tag to write Toyota barcode info to
     */
    string m_barcodeInfoTag;
};
#endif  // SymbolScannerMgr_h
