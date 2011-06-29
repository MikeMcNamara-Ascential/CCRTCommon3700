//*************************************************************************
// FILE DESCRIPTION:
//	The CLV295ScannerMgr manages the overhead CLV295 barcode scanner.  It also
//  manages the stack light associated with the scanner.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/CLV295ScannerMgr/CLV295ScannerMgr.cpp $
// 
// 3     6/07/06 5:36p Cward
// Changes to allow the Input Server, CLV295ScannerMgr,
// MapleKeypadManager, PendantManager, and SymbolScannerMgr configurations
// to be reloaed without restrting the processes.
//
// 1     6/05/06 3:44p Rwiersem
// Added a flag to tell if we should open the logical port.  This is used
// to run the unit tests without the full system.  Added
// LoadAdditionalConfiguration() items which is used to reload the
// configuration without restarting the process.
//
// 2     4/13/06 4:14a Cward
// Initialize 'ii ' to 0 inside TestStackLightLamps()
//
// 2     3/20/06 10:58a Bmeinke
// Initialize 'ii ' to 0 inside TestStackLightLamps()
//
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
// 3     4/29/05 8:00a Gpattison
// Refactored the entire CLV 295 Scanner Manager so that the InputServer could
// spawn it as an individual input device instead of having it as an object.
// This allows it to be added or removed from the system by making
// only configuration file changes.  It is now a stand alone BepServer type
// executable.
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     7/21/04 11:30a Cward
// Changes to allow Vin Legnth to be configurable
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
// 1     6/12/03 4:04p Khykin
// Initial checkin.
//
// 1     4/22/03 9:47a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:52  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:13a Khykin
// Initial Checkin
//
// 1     1/18/03 8:08a Khykin
// Initial Checkin From DCTC
//
// 2     8/14/02 3:44p Skypig13
// Removed compiler warnings
//
// 1     6/13/02 2:56p Dan.erickson
// Created and minimal testing.
// WARNING:  This module has not been tested with an actual
// CLV295 scanner.  Debug may be needed!
//
//*************************************************************************

#include "CLV295ScannerMgr.h"

CLV295ScannerMgr::CLV295ScannerMgr() : InputDeviceBase(), m_vinLength(17),
    m_openLogicalPort(true)
{   // Nothing to do here
}

CLV295ScannerMgr::~CLV295ScannerMgr()
{   // Clear out the CLV commands map
    m_clvCommands.clear(true);
}

void CLV295ScannerMgr::Initialize(const XmlNode *configNode)
{
    // Initialize the base component
    InputDeviceBase::Initialize(configNode);

    // Initialize the scanner comm object
    Log(LOG_FN_ENTRY, "CLV295ScannerMgr::Initialize()\n");

    // load the additional configuration items needed by the CLV295ScannerMgr
    LoadAdditionalConfigurationItems(configNode);
}

void
CLV295ScannerMgr::LoadAdditionalConfigurationItems(const XmlNode *configNode)
{
    const XmlNode *document = configNode->getChild("Setup");

    // Get the Maximum attempts for sending each command and the buffer size
    try
    {
        SetMaxAttempts(document->getChild("MaxAttempts")->getValue());
        SetMaxBufferSize(document->getChild("MaxBufferSize")->getValue());
    }
    catch (...)
    {
        SetMaxAttempts(3);
        SetMaxBufferSize(128);
    }
    // Get the light on durations
    try
    {
        SetStackLightOnDuration(document->getChild("LightOnDuration")->getValue());
        SetStackLightTestDelay(document->getChild("LightTestDelay")->getValue());
        SetCommandDelay(document->getChild("CommandDelay")->getValue());
    }
    catch (...)
    {
        SetStackLightOnDuration(500);
        SetStackLightTestDelay(100);
        SetCommandDelay(100);
    }
    // Get scanner setup details
    try
    {
        SetDataDelay(document->getChild("DataDelay")->getValue());
        SetMaxDecodes(document->getChild("MaxDecodes")->getValue());
        SetReaderIDLength(document->getChild("ReaderIDLength")->getValue());
        SetFISDecodeLength(document->getChild("FISDecodeLength")->getValue());
    }
    catch (...)
    {
        SetDataDelay(10);
        SetMaxDecodes(30);
        SetReaderIDLength(4);
        SetFISDecodeLength(4);
    }
    // Get build data setup info
    try
    {
        SetValidRecordAttempts(atoi(document->getChild("ValidBuildRecordReceivedAttempts")->getValue().c_str()));
        SetValidRecordCheckDelay(atoi(document->getChild("ValidBuildRecordReceivedCheckDelay")->getValue().c_str()));
    }
    catch (XmlException &excpt)
    {
        SetValidRecordAttempts(100);
        SetValidRecordCheckDelay(100);
    }
    // Get the VIN Length from the configuration file
    try
    {
        m_vinLength = BposReadInt(document->getChild("VinLength")->getValue().c_str());
    }
    catch (...)
    {
        Log(LOG_ERRORS,"ERROR CLV295ScannerMgr::Initialize() Unable to find VinLength, defaulting to 17\n");
        m_vinLength = 17;
    }
    // Get the valid CLV commands
    SetCLVCommands(document->getChild("ScannerCommands"));

    // if we should open the logical ports
    if(m_openLogicalPort == true)
    {
    // Setup the serial comm object
    m_clvScannerComm.Initialize(document->getChild("ScannerCommunication"));
}
}

const std::string CLV295ScannerMgr::Publish(const XmlNode *node)
{
    if ((node->getName() == VEHICLE_AT_BARCODE_SCANNER_TAG) &&
        (node->getValue() == AT_BARCODE_SCANNER_VALUE))
    {   // Signal the CLV scanner manager to do its thing
        Log(LOG_DEV_DATA, "Detected vehicle at barcode scanner, signaling CLV scanner");
        m_vehicleAtScanner.Signal(AT_BARCODE_SCANNER_VALUE);
        // Wait and then clear the cond var
        BposSleep(500);
        m_vehicleAtScanner.Signal(NOT_AT_BARCODE_SCANNER_VALUE);
    }
    else if (node->getName() == BUILD_RECORD_STATUS_TAG)
    {
        m_buildRecordStatusReceived.Signal(BUILD_RECORD_STATUS_RECEIVED);
        // Wait and then clear the cond var
        BposSleep(100);
        m_buildRecordStatusReceived.Signal(BUILD_RECORD_STATUS_NOT_RECEIVED);
    }
    // Call the base class method to do it's thing
    return(BepServer::Publish(node));
}

void CLV295ScannerMgr::Run(volatile bool *terminateFlag /* =NULL */)
{
    unsigned char scanData[GetMaxBufferSize()];
    INT32 byteCount = 0, fisDecodes = 0;
    std::string vin;
    string response;    // used for receiving the reponse from a Write() to the input server

    m_vehicleAtScanner.Acquire();
    // Start looking for VIN data
    while (GetStatus() != BEP_TERMINATE)
    {   // Check for a vehicle at the scanner
        m_vehicleAtScanner.Wait(AT_BARCODE_SCANNER_VALUE);
        Log(LOG_DEV_DATA, "Vehicle at barcode scanner, turning the beam on");
        // Turn on the scanner beam
        if (SendCommandToCLV(beamOn) == BEP_STATUS_SUCCESS)
        {
            Log(LOG_DEV_DATA, "Scanner beam turned on");
            // Read the data from the scanner
            byteCount = ReadScannerData(scanData);
            // Turn off the overhead scanner beam
            SendCommandToCLV(beamOff);
            Log(LOG_DEV_DATA, "Scanner beam turned off");
            Log(LOG_DEV_DATA, "Read %d bytes from the scanner", byteCount);
        }
        // Get the VIN data if anything was scanned
        if (byteCount >= (m_vinLength + GetReaderIDLength() + GetFISDecodeLength()))
        {   // Get the VIN from the data and write to the InputServer
            fisDecodes = ExtractVinData(scanData, byteCount, vin);
            XmlNode vinNode(NEXT_VIN_DATA_TAG, vin);
            m_inputServerComm->Write(&vinNode,response,true);
            // Turn on the stack light for label received
            SendCommandToCLV(barcodeScanned);
            // Wait for the build record status to be published
            // NOTE:  The build data will initially be invalid, wait for a while before
            //        lighting any of the stack lights.
            m_buildRecordStatusReceived.Acquire();
            m_buildRecordStatusReceived.Wait(BUILD_RECORD_STATUS_RECEIVED);
            m_buildRecordStatusReceived.Release();
            // Wait for the build record to be validated
            INT32 validRecordAttempts = GetValidRecordAttempts();
            while (!IsBuildRecordValid() && validRecordAttempts--)  BposSleep(GetValidRecordCheckDelay());
            std::string stackLights = EvaluateStatus(IsBuildRecordValid(), (fisDecodes < GetMaxDecodes()));
            FlashStackLight(stackLights, GetStackLightOnDuration());
        }
        else
        {   // Flash the stack lights for no label scanned
            FlashStackLight(noRead, GetStackLightOnDuration());
        }
    }
    m_vehicleAtScanner.Release();
}

INT32 CLV295ScannerMgr::ReadScannerData(unsigned char *scanData)
{
    INT32 byteCount = 0;
    // Clear the data buffer
    memset(scanData, 0, GetMaxBufferSize());
    Log(LOG_DEV_DATA, "Reading data from CLV scanner");
    // Get the remaining data from the port
    m_clvScannerComm.GetRemainingSerialData(scanData, GetMaxBufferSize(), byteCount, GetMaxAttempts(), GetDataDelay());
    Log(LOG_DEV_DATA, "CLV::ReadScannerDataRead %d bytes from CLV scanner", byteCount);
    // Build the message to log
    char buf[32];
    string logString;

    for (INT32 index=0; index < byteCount; index++)
    {
        logString += CreateMessage(buf, sizeof(buf), "%02X ", scanData[index]);
    }
    Log(LOG_DEV_DATA, "Received %d bytes from CLV scanner - data: %s", byteCount, logString.c_str());
    return byteCount;
}

const INT32 CLV295ScannerMgr::ExtractVinData(unsigned char *data, const INT32 &byteCount, std::string &vin)
{
    INT32 index = 0, decodes = 0;
    // Look for the trailing CR character since all data ends with this
    while ((data[index] != CR) && (index < byteCount)) index++;
    // Get the vin from the scanner data
    vin = std::string((char *)&data[index-m_vinLength], m_vinLength);
    // Get the number of decodes from the scanner data
    std::string decodesStr((char *)&data[index-m_vinLength-GetFISDecodeLength()], GetFISDecodeLength()-1);
    decodes = atoi(decodesStr.c_str());
    return decodes;
}

const std::string CLV295ScannerMgr::EvaluateStatus(const bool &buildRecordValid, const bool &validDecodes)
{
    std::string command;
    if (buildRecordValid && validDecodes)
    {   // All is well
        command = validVinData;
    }
    else if (buildRecordValid && !validDecodes)
    {   // Good vin data, need scanner maintenance
        command = validVinDataMaintenanceNeeded;
    }
    else if (!buildRecordValid && validDecodes)
    {   // Bad Build record
        command = badVinData;
    }
    else if (!buildRecordValid && !validDecodes)
    {   // Bad build record, need scanner maintenance
        command = badVinDataMaintenanceNeeded;
    }
    return command;
}

BEP_STATUS_TYPE CLV295ScannerMgr::SendCommandToCLV(const std::string &command)
{   // Build the command string to send to the CLV scanner
    SerialString_t clvCommandString;
    clvCommandString += STX;
    string clvCommand = GetCLVCommand(command);
    // Translate the ascii data into serial string data
    for (UINT32 index = 0; index < clvCommand.size(); index++)
    {
        clvCommandString += (UINT8)clvCommand[index];
    }
    clvCommandString += ETX;
    m_clvScannerComm.PrintSerialString(command.c_str(), clvCommandString, LOG_DEV_DATA);
    // Clear out the buffer
    m_clvScannerComm.ResetConnection();
    // Send the command to the CLV
    BEP_STATUS_TYPE sendStatus = m_clvScannerComm.Send(clvCommandString);
    // Log an error if the command was not successful
    if (sendStatus != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "Error sending command to CLV295 -- status:%d, command:%s[<$%02X>%s<$%02X>]\n",
            sendStatus, command.c_str(), STX, GetCLVCommand(command).c_str(), ETX);
    }
    else
    {   // Get the command echo from the scanner
        sendStatus = GetCommandEcho(GetCLVCommand(command));
    }
    return sendStatus;
}

BEP_STATUS_TYPE CLV295ScannerMgr::GetCommandEcho(const std::string &command)
{
    unsigned char echoData[command.length()+10];   // Make a buffer slightly larger than the command
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    UINT32 byteCount = 0;
    Log(LOG_DEV_DATA, "Message sent to CLV scanner, waiting for reply");
    // Get the data, look for a carriage return to signal the end of the command echo
    bool endOfString = false;
    UINT8 currentByteCount = 0;
    UINT8 echoByte;
    INT32 attempts = 0;
    do
    {   // Get the data from the port
        currentByteCount = m_clvScannerComm.Get(&echoByte, 1);
        // Check for any data data
        if (currentByteCount > 0)
        {   // We got data
            echoData[byteCount] = echoByte;
            endOfString = echoByte == CR;
            byteCount++;
            attempts = 0;
            Log(LOG_DEV_DATA, "Received data: %02X, endOfString: %s, ByteCount: %d", echoByte, endOfString ? "True" : "False", byteCount);
        }
        else
        {   // No data received
            attempts++;
        }
        // Need to get carriage return before moving on
    } while ((byteCount < command.length()+1) && !endOfString && (attempts < GetMaxAttempts()));
    Log(LOG_DEV_DATA, "All serial data retrieved, %d bytes", byteCount);
    // Make sure this is the data we are looking for
    if (((UINT32)byteCount >= command.length()) && (echoData[byteCount-1] == CR))
    {   // Get the echo from the array, discarding the carriage return
        std::string echo((char *)&echoData[byteCount-1-command.length()-1], command.length());
        Log(LOG_DEV_DATA, "Echo received for command: %s", command.c_str());
        status = BEP_STATUS_SUCCESS;
    }
    else
    {   // Incomplete echo
        Log(LOG_ERRORS, "Incomplete echo from CLV scanner -- byte count: %d, command length: %d", byteCount, command.length());
        status = BEP_STATUS_FAILURE;
    }
    // Return the echo status
    return status;
}

void CLV295ScannerMgr::FlashStackLight(const std::string &light, const INT32 &duration)
{
    SendCommandToCLV(light);
    BposSleep(duration);
    SendCommandToCLV(stackLightAllOff);
}

void CLV295ScannerMgr::TestStackLightLamps()
{   // Set the lamps to light
    std::string lamps[] = {stackLightRed, stackLightYellow, stackLightGreen, stackLightBlue, stackLightAllOn};
    // Set the number of lamps to light
    INT16 numberOfLamps = 5;
    // Illuminate each lamp in the list
    for (INT16 ii=0; ii < numberOfLamps; ii++)
    {
        FlashStackLight(lamps[ii], GetStackLightTestDelay());
    }
}

bool CLV295ScannerMgr::IsBuildRecordValid(void)
{
    return(Read(BUILD_RECORD_STATUS_TAG) == BUILD_RECORD_VALID);
}

inline bool CLV295ScannerMgr::IsVehicleAtScanner(void)
{
    return(Read(VEHICLE_AT_BARCODE_SCANNER_TAG) == AT_BARCODE_SCANNER_VALUE);
}

inline void CLV295ScannerMgr::SetValidRecordAttempts(const INT32 &attempts)
{
    m_validRecordReceivedAttempts = attempts;
}

inline void CLV295ScannerMgr::SetValidRecordCheckDelay(const INT32 &checkDelay)
{
    m_validRecordCheckDelay = checkDelay;
}

inline const INT32& CLV295ScannerMgr::GetValidRecordAttempts(void)
{
    return m_validRecordReceivedAttempts;
}

inline const INT32& CLV295ScannerMgr::GetValidRecordCheckDelay(void)
{
    return m_validRecordCheckDelay;
}

std::string CLV295ScannerMgr::GetCLVCommand(const std::string &commandName)
{
    std::string command = BEP_UNSUPPORTED_RESPONSE;
    XmlNodeMapItr iter;
    if ((iter = m_clvCommands.find(commandName)) != m_clvCommands.end())
    {   // Get the command string to send to the CLV
        command = iter->second->getValue();
    }
    return command;
}

const INT32 CLV295ScannerMgr::GetStackLightOnDuration(void)
{
    return m_stackLightOnDuration;
}

const INT32 CLV295ScannerMgr::GetStackLightTestDelay(void)
{
    return m_stackLightTestDelay;
}

INT32 CLV295ScannerMgr::GetMaxAttempts(void)
{
    return m_maxAttempts;
}

const INT32 CLV295ScannerMgr::GetCommandDelay(void)
{
    return m_commandDelay;
}

const INT32 CLV295ScannerMgr::GetMaxBufferSize(void)
{
    return m_maxBufferSize;
}

const INT32 CLV295ScannerMgr::GetDataDelay(void)
{
    return m_dataDelay;
}

const INT32 CLV295ScannerMgr::GetMaxDecodes(void)
{
    return m_maxFisDecodes;
}

const INT32 CLV295ScannerMgr::GetReaderIDLength(void)
{
    return m_readerIDLength;
}

const INT32 CLV295ScannerMgr::GetFISDecodeLength(void)
{
    return m_fisDecodeLength;
}

void CLV295ScannerMgr::SetCLVCommands(const XmlNode *commands)
{
    if (commands != NULL) m_clvCommands.DeepCopy(commands->getChildren());
    else m_clvCommands.clear(true);
}

void CLV295ScannerMgr::SetStackLightOnDuration(const INT32 &duration)
{
    m_stackLightOnDuration = duration;
}

void CLV295ScannerMgr::SetStackLightOnDuration(const std::string &duration)
{
    SetStackLightOnDuration(atoi(duration.c_str()));
}

void CLV295ScannerMgr::SetStackLightTestDelay(const INT32 &delay)
{
    m_stackLightTestDelay = delay;
}

void CLV295ScannerMgr::SetStackLightTestDelay(const std::string &delay)
{
    SetStackLightTestDelay(atoi(delay.c_str()));
}

void CLV295ScannerMgr::SetMaxAttempts(const INT32 &attempts)
{
    m_maxAttempts = attempts;
}

void CLV295ScannerMgr::SetMaxAttempts(const std::string &attempts)
{
    SetMaxAttempts(atoi(attempts.c_str()));
}

void CLV295ScannerMgr::SetCommandDelay(const INT32 &delay)
{
    m_commandDelay = delay;
}

void CLV295ScannerMgr::SetCommandDelay(const std::string &delay)
{
    SetCommandDelay(atoi(delay.c_str()));
}

void CLV295ScannerMgr::SetMaxBufferSize(const INT32 &bufferSize)
{
    m_maxBufferSize = bufferSize;
}

void CLV295ScannerMgr::SetMaxBufferSize(const std::string &bufferSize)
{
    SetMaxBufferSize(atoi(bufferSize.c_str()));
}

void CLV295ScannerMgr::SetDataDelay(const INT32 &delay)
{
    m_dataDelay = delay;
}

void CLV295ScannerMgr::SetDataDelay(const std::string &delay)
{
    SetDataDelay(atoi(delay.c_str()));
}

void CLV295ScannerMgr::SetMaxDecodes(const INT32 &decodes)
{
    m_maxFisDecodes = decodes;
}

void CLV295ScannerMgr::SetMaxDecodes(const std::string &decodes)
{
    SetMaxDecodes(atoi(decodes.c_str()));
}

void CLV295ScannerMgr::SetReaderIDLength(const INT32 &length)
{
    m_readerIDLength = length;
}

void CLV295ScannerMgr::SetReaderIDLength(const std::string &length)
{
    SetReaderIDLength(atoi(length.c_str()));
}

void CLV295ScannerMgr::SetFISDecodeLength(const INT32 &length)
{
    m_fisDecodeLength = length;
}

void CLV295ScannerMgr::SetFISDecodeLength(const std::string &length)
{
    SetFISDecodeLength(atoi(length.c_str()));
}

