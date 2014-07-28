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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/SymbolScannerMgr/SymbolScannerMgr.cpp $
// 
// 4     2/22/07 11:10a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added MaxVinLength configuration item.
// - Added processing for MaxVinLength in EvaluateData().
// - Removed #if 0 blocks.
// 
// 3     6/07/06 5:36p Cward
// Changes to allow the Input Server, CLV295ScannerMgr,
// MapleKeypadManager, PendantManager, and SymbolScannerMgr configurations
// to be reloaed without restrting the processes.
//
// 1     6/05/06 3:48p Rwiersem
// Added LoadAdditionalConfiguration() items which is used to reload the
// configuration without restarting the process.
//
// 2     4/13/06 4:16a Cward
// Checking in the changes below
//
//  a) c. 197  added new node, "DataTerminateCharacter" which uses BposReadInt()
//          to convert the value, allowing decimal or hex entry. existing node
//          "DataTerminationCharacter" processing is retained for
//          backward compatibility.
//  b) changed atoi() calls to BposReadInt(), to allow parameters to be
//          specified in either hex or decimal. several locations. This change
//          is inherently backward compatible.
//
// 2     3/20/06 10:59a Bmeinke
// Added an INamedDataBroker object to write NEXT_VIN_TAG to the task that
// handles it (not necessarily the InputServer)
//
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
// 5     4/29/05 8:00a Gpattison
// Refactored the entire Symbol Scanner Manager so that the InputServer could
// spawn it as an individual input device instead of having it as an object.
// This allows it to be added or removed from the system by making
// only configuration file changes.  It is now a stand alone BepServer type
// executable.
//
// 5     4/07/05 5:30p Gpattiso
// Changed the data string termination from only using a Carriage Return,
// to support the use of any character based on configuration parameters.
// Change should be backwards compatible.
//
// 4     7/21/04 11:30a Cward
// Changes to allow Vin Legnth to be configurable
//
// 3     5/25/04 4:22p Cward
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
// 1     7/08/03 12:58p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:04p Khykin
// Initial checkin.
//
// 1     4/22/03 9:47a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.4  2003/02/23 17:14:27  tbochene
//    Updated so it works at Toluca with CR/LF, mostly debug statements added
//
//    Revision 1.3  2003/02/22 01:33:29  tbochene
//    Updated at Toluca
//
//
// 1     1/27/03 7:12a Khykin
// Initial Checkin
//
// 1     1/18/03 8:08a Khykin
// Initial Checkin From DCTC
//
// 5     11/07/02 12:38p Derickso
// Modified EvaluateData() to only use the buffered data if a carriage
// return
// character was found in the buffer, or the buffer is exactly the same
// length
// as the VIN or Driver Number.
//
// 4     11/07/02 6:59a Derickso
// Updated EvaluateData() to not process scanned data if a
// carriage return character is not found in the buffer.
//
// 3     8/14/02 3:44p Skypig13
// Removed compiler warnings
//
// 2     6/13/02 2:55p Dan.erickson
// Utilize new SerialChannel::GetRemainingData() instead of
// writing our own loop to do the same thing.
//
// 1     5/31/02 3:03p Dan.erickson
// Created and Debugged.
//
//*************************************************************************

#include "SymbolScannerMgr.h"

//-------------------------------------------------------------------------------------------------
SymbolScannerMgr::SymbolScannerMgr() : InputDeviceBase(),m_scannerComm(NULL),
m_badDataPosition(0), m_badCharacterReplacement(0x31),m_badDataIndicator(0x00),
m_lookForTerminator(true), m_correctBadScannerData(false), m_twoDimensionBarcode(false),
m_vinLength(17),m_driverNumberLength(7)
{   // Nothing to do here
}

//-------------------------------------------------------------------------------------------------
SymbolScannerMgr::~SymbolScannerMgr()
{
    // remove the communication obj memory
    if (NULL != m_scannerComm)
    {
        delete m_scannerComm;
        m_scannerComm = NULL;
    }
    // Clear the barcode items
    m_twoDimensionBarcodeItems.clear(true);
}

//-------------------------------------------------------------------------------------------------
void SymbolScannerMgr::Initialize(const XmlNode *configNode)
{
    // Initialize the base component
    InputDeviceBase::Initialize(configNode);

    // Initialize the scanner comm object
    Log(LOG_FN_ENTRY, "SymbolScannerMgr::Initialize()\n");

    // load the additional configuration items needed by the SymbolScannerMgr
    LoadAdditionalConfigurationItems(configNode);

    Log(LOG_FN_ENTRY, "SymbolScannerMgr::Initialize() done\n");
}

//-------------------------------------------------------------------------------------------------
void SymbolScannerMgr::LoadAdditionalConfigurationItems(const XmlNode *configNode)
{
    const XmlNode *document = configNode->getChild("Setup");

    if (NULL == m_scannerComm)
    {
        m_scannerComm = new SerialChannel();
        m_scannerComm->Initialize(document->getChild("ScannerCommunication"));
    }

    // Get the size of the scanner buffer
    try
    {
        const XmlNode *scannerCommSetup = document->getChild("ScannerCommunication")->getChild("Setup");
        SetScannerBufferSize(scannerCommSetup->getChild("Communication")->getChild("BufferSize")->getValue());
    }
    catch (...)
    {
        SetScannerBufferSize(128);
    }
    // Get the data delay time and the maximum retries
    try
    {
        SetDataDelay(document->getChild("DataDelay")->getValue());
        SetMaximumRetries(document->getChild("MaximumRetries")->getValue());
    }
    catch (...)
    {
        SetDataDelay(10);
        SetMaximumRetries(3);
    }
    // Get the VIN Length from the configuration file
    try
    {
        m_vinLength = BposReadInt(document->getChild("VinLength")->getValue().c_str());
    }
    catch (...)
    {
        Log(LOG_ERRORS,"ERROR SymbolScannerMgr::Initialize() Unable to find VinLength, defaulting to 17\n");
        m_vinLength = 17;
    }
    // Get the max VIN Length from the configuration file
    try
    {
        m_maxVinLength = BposReadInt(document->getChild("MaxVinLength")->getValue().c_str());
    }
    catch (...)
    {
        Log(LOG_ERRORS,"ERROR SymbolScannerMgr::Initialize() Unable to find MaxVinLength, defaulting to 17\n");
        m_maxVinLength = 17;
    }
    // Get the Driver Number length from the configuration file
    try
    {
        m_driverNumberLength = BposReadInt(document->getChild("DriverNumberLength")->getValue().c_str());
    }
    catch (...)
    {
        Log(LOG_ERRORS,"ERROR SymbolScannerMgr::Initialize() Unable to find DriverNumberLength, defaulting to 7\n");
        m_driverNumberLength = 7;
    }
    // Determine if we need to look for a data terminator
    try
    {
        StoreLookForDataTerminator(document->getChild("LookForCarriageReturn")->getValue() == "Yes");
    }
    catch (...)
    {
        StoreLookForDataTerminator(true);
    }

    try
    {
        StoreLookForDataTerminator(document->getChild("LookForDataTerminator")->getValue() == "Yes");
    }
    catch (...)
    {
    }

    // Get the actual termination charater
    try
    {
       m_terminationCharacter =
          atoh(document->getChild("DataTerminationCharacter")->getValue().c_str());
    }
    catch (...)
    {
       try
       {
          m_terminationCharacter =
             BposReadInt(document->getChild("DataTerminateCharacter")->getValue().c_str());
       }
       catch (...)
       {
        m_terminationCharacter = CR;
    }
    }

    // Determine if bad scanner data should be corrected
    try
    {
        CorrectBadScannerData(document->getChild("CorrectBadScannerData")->getValue() == "Yes");
        if (CorrectBadScannerData())
        {   // Store the bad dat replacement info
            StoreBadDataIndex(BposReadInt(document->getChild("BadDataIndex")->getValue().c_str()));
            BadDataIndicator(atoh(document->getChild("BadDataIndicator")->getValue().c_str()));
            BadDataReplacement(atoh(document->getChild("BadDataReplacementValue")->getValue().c_str()));
        }
    }
    catch (...)
    {
        CorrectBadScannerData(false);
        StoreBadDataIndex(0);
        BadDataIndicator(0);
        BadDataReplacement(0);
    }
    // Load the secondary data items if they exist
    bool useSecondaryDataItems = false;
    string secondaryDataType("");
    INT32 secondaryDataLength = 999;
    try
    {
        secondaryDataType = document->getChild("SecondaryDataTag")->getValue();
        useSecondaryDataItems = true;
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Secondary data type not defined, not configuring for secondary data support: %s", excpt.GetReason());
        useSecondaryDataItems = false;
    }
    if(useSecondaryDataItems)
    {
        try
        {
            secondaryDataLength = BposReadInt(document->getChild("SecondaryDataLength")->getValue().c_str());
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "Secondary data length not defined, not configuring for secondary data support: %s", excpt.GetReason());
            useSecondaryDataItems = false;
        }
    }
    // Store the secondary data items
    SecondaryDataSupported(&useSecondaryDataItems);
    SecondaryDataType(&secondaryDataType);
    SecondaryDataLength(&secondaryDataLength);
	// Check if a 2D barcode is being used
	bool twoDimensionBarcode = false;
	try
	{
		twoDimensionBarcode = atob(document->getChild("TwoDimensionBarcode")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Not configuring 2D barcode - %s", excpt.GetReason());
		twoDimensionBarcode = false;
	}
	UseTwoDimensionBarcode(&twoDimensionBarcode);
	if(UseTwoDimensionBarcode())
	{
		m_twoDimensionBarcodeItems.DeepCopy(document->getChild("TwoDimensionBarcodeItems")->getChildren());
	}
}

//-------------------------------------------------------------------------------------------------
const std::string SymbolScannerMgr::Register(void)
{
    // create a named data broker interface
    if(m_dataBroker != NULL)
    {
        delete m_dataBroker;
        m_dataBroker = NULL;
    }
    m_dataBroker = new INamedDataBroker;

    return( InputDeviceBase::Register());
}

void SymbolScannerMgr::Run(volatile bool *terminateFlag /* =NULL */)
{   // Keep looping until the Server tells us to die
    SerialString_t scanData;
    INT32 byteCount = 0;

    Log(LOG_FN_ENTRY, "SymbolScannerMgr::Execute() begin\n");
    // Clear the scanner buffer data
    scanData.erase();
    // Loop until the server is terminated
    while (GetStatus() != BEP_TERMINATE)
    {   // Get the data out of the buffer
        Log(LOG_DEV_DATA,"Waiting for scan data\n");
        // wait for data up to 3 seconds, just to be able to terminate
        byteCount = m_scannerComm->ReadPort(scanData, GetDataDelay(), GetDataDelay()/2);
        Log(LOG_DEV_DATA, "Received %d bytes from scanner\n", byteCount);
        // Check to see if any data was read out
        if (byteCount > 0)
        {   // Evaluate the data we received
            EvaluateData(scanData, byteCount);
            // Clear the scanner buffer data
            scanData.erase();
        }
    }
}

void SymbolScannerMgr::EvaluateData(SerialString_t &data, const INT32 &byteCount)
{
    bool processData = true; // Process the data that is extracted from the buffer
    bool foundDataTerminator = false;   // CR character found in data stream
    INT32 index = 0;
    INT32 dataLength = 0;    // Length of data we are looking for
    std::string scanValue;   // Data value to write to the InputServer
    std::string dataType(BEP_UNAVAILABLE_RESPONSE);    // Type of data that was scanned (VIN, DriverNumber)
    std::string displayType(BEP_UNAVAILABLE_RESPONSE); // Which display to write the data value to

    Log(LOG_DEV_DATA, "SymbolScannerMgr::EvaluateData() -- byteCount:%d\nBuffer:\n", byteCount);
    for (INT32 ii = 0; ii < byteCount; ii++)
        Log(LOG_DEV_DATA, "\tii:%d -- %c <$%02X>\n", ii, isprint(data[ii]) ? data[ii] : '?', data[ii]);
    // Determine if we should look for a data terminator
    if (LookForTerminationCharacter())
    {   // Look for a carriage return so we know where the data ends
        while (!foundDataTerminator && (index < byteCount))
        {   // Check if the current character is a termination character
            foundDataTerminator = (data[index] == m_terminationCharacter);
            // Increment the index if the data termination character was not found
            if (!foundDataTerminator) index++;
        }
    }
    else
    {   // Skip the data terminator search and just use last byte as index
        foundDataTerminator = true;
        index = byteCount ;
    }
    // Only process the data if we found the data termination character
    Log(LOG_DEV_DATA, "foundDataTerminator: %d, index: %d, Byte count: %d, vinLength: %d, driverNumberLength: %d\n",
        foundDataTerminator, index, byteCount, m_vinLength, m_driverNumberLength);

    if(UseTwoDimensionBarcode())
    {
        ProcessTwoDimensionalBarcode(data, byteCount);
    }
    else if(foundDataTerminator || (byteCount >= m_vinLength) || (byteCount == m_driverNumberLength) || (byteCount == SecondaryDataLength()))
    {   // Determine what type of data we received
        if(SecondaryDataSupported() && (SecondaryDataLength() > m_vinLength) && (index >= SecondaryDataLength()))
        {   // Check for secondary data before VIN
            dataLength = SecondaryDataLength();
            dataType = SecondaryDataType();
            Log(LOG_DEV_DATA, "Received %s scan -- Byte count: %d", dataType.c_str(), dataLength);
        }
        else if (index >= m_vinLength)
        {
            // Set the tag to use and the length of the data we are looking for
            dataLength = (index >= m_maxVinLength) ? m_maxVinLength : m_vinLength;
            dataType = NEXT_VIN_DATA_TAG;
            displayType = VINDISPLAY_DATA_TAG;
            Log(LOG_DEV_DATA, "Received VIN scan -- Byte count: %d\n", byteCount);
        }
        else if(SecondaryDataSupported() && (SecondaryDataLength() < m_vinLength) && (index >= SecondaryDataLength()))
        {   // Check for VIN data before checking for secondary data
            dataLength = SecondaryDataLength();
            dataType = SecondaryDataType();
            Log(LOG_DEV_DATA, "Received %s scan -- Byte count: %d", dataType.c_str(), dataLength);
        }
        else if (index >= m_driverNumberLength)
        {   // Set the tag to use and the length of the data we are looking for
            dataLength = m_driverNumberLength;
            dataType = DRIVER_NUMBER_DATA_TAG;
            displayType = DRIVER_NUMBER_DISPLAY_DATA_TAG;
            Log(LOG_DEV_DATA, "Received driver scan -- Byte count: %d\n", byteCount);
        }
        else
        {   // We received an invalid scan
            processData = false;
            Log(LOG_DEV_DATA, "Received Invalid scan -- Byte count: %d, data:\n", byteCount);
            for (INT32 ii = 0; ii < byteCount; ii++)
                Log(LOG_DEV_DATA, "\tByte:%d -- %c <$%02X>\n", ii, isprint(data[ii]) ? data[ii] : '?', data[ii]);
        }
        // Determine if we should process the data
        if (processData)
        {   // Check if any "bad" scanner data should be corrected
            if (CorrectBadScannerData() && (data[BadDataIndex()] == BadDataIndicator()))
            {   // Correct the bad data
                Log(LOG_ERRORS, "Bad scanner data -- Index: %d, Indicator: %02X, {Scanned value: %02X}",
                    BadDataIndex(), BadDataIndicator(), data[BadDataIndex()]);
                data[BadDataIndex()] = BadDataReplacement();
                Log(LOG_ERRORS, "Bad scanner data replaced with %02X -- (New buffer value: %02X)",
                    BadDataReplacement(), data[BadDataIndex()]);
            }
            // Get the valid scan data from our local data buffer
            scanValue = std::string((char *)&data[index-dataLength], dataLength);
            Log(LOG_DEV_DATA, "Symbol Scanner: %s(%s)\n", dataType.c_str(), scanValue.c_str());
            // Make a couple of nodes to send data around
            XmlElement displayNode(displayType, scanValue);
            XmlElement dataNode(dataType, scanValue);
            // Write the data to the InputServer
            string response;
            // Only send the display node if a display tag was specified
            if(displayType != BEP_UNAVAILABLE_RESPONSE)
            {
            (void)m_dataBroker->Write(&displayNode,response,true);
            }
            (void)m_dataBroker->Write(&dataNode,response,true);
        }
    }
    else
    {   // Incomplete scan received
        Log(LOG_DEV_DATA, "Received Incomplete scan -- Byte count: %d, data:\n", byteCount);
        if (GetLogStatus() && (GetVerboseMask() & LOG_DEV_DATA))
        {
            for (INT32 ii = 0; ii < byteCount; ii++)
                Log("\tByte:%d -- %c <$%02X>\n", ii, isprint(data[ii]) ? data[ii] : '?', data[ii]);
        }
    }
}

const INT32 SymbolScannerMgr::GetScannerBufferSize(void)
{
    return m_scannerBufferSize;
}

const INT32 SymbolScannerMgr::GetDataDelay(void)
{
    return m_dataDelay;
}

INT16 SymbolScannerMgr::GetMaxRetries(void)
{
    return m_maxRetries;
}

void SymbolScannerMgr::SetDummyScannerCommObject(SerialChannel *commObj)
{
    m_scannerComm = commObj;
}

inline const bool& SymbolScannerMgr::LookForTerminationCharacter(void)
{
    return m_lookForTerminator;
}

inline void SymbolScannerMgr::StoreLookForDataTerminator(const bool &lookForTerminator)
{
    m_lookForTerminator = lookForTerminator;
}

inline void SymbolScannerMgr::CorrectBadScannerData(const bool &correctBadData)
{
    m_correctBadScannerData = correctBadData;
}

//-------------------------------------------------------------------------------------------------
inline const bool& SymbolScannerMgr::CorrectBadScannerData(void)
{
    return m_correctBadScannerData;
}

//-------------------------------------------------------------------------------------------------
inline void SymbolScannerMgr::StoreBadDataIndex(const INT16& index)
{
    m_badDataPosition = index;
}

//-------------------------------------------------------------------------------------------------
inline const INT16& SymbolScannerMgr::BadDataIndex(void)
{
    return m_badDataPosition;
}

//-------------------------------------------------------------------------------------------------
inline void SymbolScannerMgr::BadDataReplacement(const UINT8& data)
{
    m_badCharacterReplacement = data;
}

//-------------------------------------------------------------------------------------------------
inline const UINT8& SymbolScannerMgr::BadDataReplacement(void)
{
    return m_badCharacterReplacement;
}

//-------------------------------------------------------------------------------------------------
inline void SymbolScannerMgr::BadDataIndicator(const UINT8& indicator)
{
    m_badDataIndicator = indicator;
}

//-------------------------------------------------------------------------------------------------
inline const UINT8& SymbolScannerMgr::BadDataIndicator(void)
{
    return m_badDataIndicator;
}

//-------------------------------------------------------------------------------------------------
void SymbolScannerMgr::SetScannerBufferSize(const INT32 &size)
{
    m_scannerBufferSize = size;
}

//-------------------------------------------------------------------------------------------------
void SymbolScannerMgr::SetScannerBufferSize(const std::string &size)
{
    SetScannerBufferSize(BposReadInt(size.c_str()));
}

//-------------------------------------------------------------------------------------------------
void SymbolScannerMgr::SetDataDelay(const INT32 &delay)
{
    m_dataDelay = delay;
}

//-------------------------------------------------------------------------------------------------
void SymbolScannerMgr::SetDataDelay(const std::string &delay)
{
    SetDataDelay(BposReadInt(delay.c_str()));
}

//-------------------------------------------------------------------------------------------------
void SymbolScannerMgr::SetMaximumRetries(const INT16 &retries)
{
    m_maxRetries = retries;
}

//-------------------------------------------------------------------------------------------------
void SymbolScannerMgr::SetMaximumRetries(const std::string &retries)
{
    SetMaximumRetries(BposReadInt(retries.c_str()));
}

//-------------------------------------------------------------------------------------------------
inline const bool& SymbolScannerMgr::SecondaryDataSupported(const bool *dataSupported /*= NULL*/)
{
    if(dataSupported != NULL)  m_secondaryDataSupport = *dataSupported;
    return m_secondaryDataSupport;
}

//-----------------------------------------------------------------------------
inline const string& SymbolScannerMgr::SecondaryDataType(const string *dataType /*= NULL*/)
{
    if(dataType != NULL)  m_secondaryDataTag = *dataType;
    return m_secondaryDataTag;
}

//-----------------------------------------------------------------------------
inline const INT32& SymbolScannerMgr::SecondaryDataLength(const INT32 *length /*= NULL*/)
{
    if(length != NULL)  m_secondaryDataLength = *length;
    return m_secondaryDataLength;
}

//-------------------------------------------------------------------------------------------------
void SymbolScannerMgr::ProcessTwoDimensionalBarcode(const SerialString_t &barcodeData, const INT32 &byteCount)
{
    INT32 startIndex = 0;
    string response;
    for(XmlNodeMapCItr iter = m_twoDimensionBarcodeItems.begin(); 
         (iter != m_twoDimensionBarcodeItems.end()); 
         iter++)
    {   // Get the current item and publish to the system
        INT32 length = BposReadInt(iter->second->getAttribute("Length")->getValue().c_str());
        INT32 startIndexOverride;
        try
        {//If specified use explicit start index
            startIndexOverride = BposReadInt(iter->second->getAttribute("StartIndex")->getValue().c_str());
        }
        catch  (XmlException &excpt)
        {
            startIndexOverride = startIndex;
        }
        string data = string((char *)&barcodeData[startIndexOverride], length);
        // Write the data to the system
        m_dataBroker->Write(iter->second->getValue(), data, response, true);
        Log(LOG_DEV_DATA, "Processed %s:%s - %s", 
            iter->second->getName().c_str(), iter->second->getValue().c_str(), data.c_str());
        if(!iter->second->getAttribute("DisplayTag")->getValue().empty())
        {
            m_dataBroker->Write(iter->second->getAttribute("DisplayTag")->getValue(), data, response, true);
            Log(LOG_DEV_DATA, "Updated display tag: %s - %s", 
                iter->second->getAttribute("DisplayTag")->getValue().c_str(), data.c_str());
        }
        // Update the start index to the next data item
        startIndex += length;
    }
}

//-------------------------------------------------------------------------------------------------
const bool& SymbolScannerMgr::UseTwoDimensionBarcode(const bool *isTwoDimension /*= NULL*/)
{
    if(isTwoDimension != NULL)  m_twoDimensionBarcode = *isTwoDimension;
    return m_twoDimensionBarcode;
}
