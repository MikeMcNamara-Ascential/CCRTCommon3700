//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/Include/CLV295ScannerMgr.h 3     10/30/07 4:08p Rwiersem $
//
// FILE DESCRIPTION:
//  The Pendantmanager handles all pendant inputs and publishes the key presses to
//  objects that need the data.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/Include/CLV295ScannerMgr.h $
// 
// 3     10/30/07 4:08p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     6/07/06 5:36p Cward
// Changes to allow the Input Server, CLV295ScannerMgr,
// MapleKeypadManager, PendantManager, and SymbolScannerMgr configurations
// to be reloaed without restrting the processes.
//
// 1     6/05/06 3:45p Rwiersem
// Made the test class a friend.  Added a flag to tell if we should open
// the logical port.  This is used to run the unit tests without the full
// system.  Added LoadAdditionalConfiguration() items which is used to
// reload the configuration without restarting the process.
//
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
// 2     4/29/05 8:00a Gpattison
// Refactored the entire CLV 295 Scanner Manager so that the InputServer could
// spawn it as an individual input device instead of having it as an object.
// This allows it to be added or removed from the system by making
// only configuration file changes.  It is now a stand alone BepServer type
// executable.
//
// 2     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
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
// 1     6/12/03 4:07p Khykin
// Initial checkin.
//
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:35  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:03a Khykin
// Initial Checkin
//
// 1     1/18/03 8:01a Khykin
// Initial Checkin From DCTC
//
// 1     6/13/02 2:58p Dan.erickson
// Created and minimal testing.
// WARNING: This module has not been tested with a CLV295
// scanner!  Debug may be needed!
//
//*************************************************************************
#ifndef CLV295ScannerMgr_h
#define CLV295ScannerMgr_h
//===========================================================================

#include "BepSync.h"
#include "InputServerDefs.h"
#include "InputDeviceBase.h"
#include "SerialChannel.h"

/**
 * Start and End of message values to wrap commands to the CLV.
 * @since Version 1.0
 */
#define STX   '\x002'
#define ETX   '\x003'
#define CR    '\x00D'
/**
 * Names of valid commands to send to the CLV scanner.
 * @since Version 1.0
 */
const std::string beamOn("BeamOn");
const std::string beamOff("BeamOff");
const std::string stackLightAllOff("StackLightAllOff");
const std::string stackLightAllOn("StackLightAllOn");
const std::string stackLightRed("StackLightRed");
const std::string stackLightYellow("StackLightYellow");
const std::string stackLightGreen("StackLightGreen");
const std::string stackLightBlue("StackLightBlue");
const std::string noRead("NoRead");
const std::string lowPerformance("LowPerformance");
const std::string vinDataRead("VinDataRead");
const std::string barcodeScanned("BarcodeScanned");
const std::string validVinData("ValidVinData");
const std::string validVinDataMaintenanceNeeded("ValidVinDataMaintenanceNeeded");
const std::string badVinData("BadVinData");
const std::string badVinDataMaintenanceNeeded("BadVinDataMaintenanceNeeded");


/**
 * The CLV Scanner Manager class manages the CLV 295 Barcode scanner.
 * This class will turn the scanner beam on when a vehicle is detected at the barcode scanner and
 * will turn the beam off when either a valid barcode is scanned or when the timeout expires.
 * The CLV Scanner Manager class also manages the stack lights in order to provide the operators
 * with a visual indicator of the status of the barcode scan.
 *
 * @author Dan Erickson
 * @version Version 1.0
 * @since May 29, 2002
 */
class CLV295ScannerMgr : public InputDeviceBase
{
    friend class CLV295ScannerMgrTest;

public:
    /**
     * Class Constructor.
     */
    CLV295ScannerMgr();
    /**
     * Class destructor.
     * @since Version 1.0
     */
    ~CLV295ScannerMgr();
    /**
     * Initializes the scanner manager to talk to the scanner as well as any data that needs to
     * be setup before we begin processing inputs.
     *
     * @param configNode Configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *configNode);
    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 1.1
     */
    virtual const std::string Publish(const XmlNode *node);
    /**
     * Main Run method to control the process
     *
     * @param terminateFlag
     *               Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
    /**
     * Retrieves the command string for the command name specified.  The string is what will be
     * wrapped and sent to the CLV.
     *
     * @param commandName
     *               Name of the command to send to the CLV.
     * @return The command string that will be sent to the CLV.
     * @since Version 1.0
     */
    std::string GetCLVCommand(const std::string &commandName);
    /**
     * Get the length of time in ms to leave the stack light illuminated.
     *
     * @return Time in ms.
     * @since Version 1.0
     */
    const INT32 GetStackLightOnDuration(void);
    /**
     * Get the length of time in ms to leave the stack light illuminated during a lamp test.
     *
     * @return Time in ms.
     * @since Version 1.0
     */
    const INT32 GetStackLightTestDelay(void);
    /**
     * Get the maximum number of times to send a command before logging a failure.
     *
     * @return Number of times to send the command.
     * @since Version 1.0
     */
    INT32 GetMaxAttempts(void);
    /**
     * Get the length of time to delay before resending a failed command.
     *
     * @return Delay time in ms.
     * @since Version 1.0
     */
    const INT32 GetCommandDelay(void);
    /**
     * Return the maximum size of the scanner buffer.
     *
     * @return Size of the buffer in bytes.
     * @since Version 1.0
     */
    const INT32 GetMaxBufferSize(void);
    /**
     * Get the time to wait for data to arrive at the port.
     *
     * @return Time to wait in ms.
     * @since Version 1.0
     */
    const INT32 GetDataDelay(void);
    /**
     * Return the maximum number of decodes allowed for each label.
     *
     * @return Maximum number of decodes allowed.
     * @since Version 1.0
     */
    const INT32 GetMaxDecodes(void);
    /**
     * Return the length of the Reader ID field.
     *
     * @return Length of the field.
     * @since Version 1.0
     */
    const INT32 GetReaderIDLength(void);
    /**
     * Return the length of the FIS decode field.
     *
     * @return Length of the field.
     * @since Version 1.0
     */
    const INT32 GetFISDecodeLength(void);
    /**
     * Detects wether a vehicle is present at the CLV scanner.
     *
     * @return True if a vehicle is at the CLV barcode scanner.  False otherwise.
     * @since Version 1.0
     */
    bool IsVehicleAtScanner(void);
    /**
     * Get the number of checks to make for a valid build record.
     *
     * @return The number of checks to make for a valid build record.
     */
    const INT32& GetValidRecordAttempts(void);
    /**
     * Get the amount of time to wait between each check for a valid build record.
     *
     * @return The amount of time in ms to wait between each check for a valid build record.
     */
    const INT32& GetValidRecordCheckDelay(void);

protected:
    /**
     * Load the additional configuration items needed by the CLV295ScannerMgr.
     * The "standard" items will be handled by the call to
     * InputDeviceBase::Initialize().
     *
     * @param configNode A pointer to the CLV295Scanner config node
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *configNode);
    /**
     * Retrieve the data from the scanner for the label that was scanned.
     *
     * @param scanData Data sent by the scanner.
     * @return The number of bytes read from the port.
     * @since Version 1.0
     */
    virtual INT32 ReadScannerData(unsigned char *scanData);
    /**
     * Extract the VIN data from the scanned data buffer.
     *
     * @param data      Scanned data buffer.
     * @param byteCount Number of bytes received from the scanner.
     * @param vin       The vin that was scanned.
     * @return The number of decodes performed by the scanner.
     * @since Version 1.0
     */
    virtual const INT32 ExtractVinData(unsigned char *data, const INT32 &byteCount, std::string &vin);
    /**
     * Determine which command to send to the CLV scnner to illuminate the appropriate lights.
     * Valid build record AND decodes less than max      --> valid vin data.
     * Valid build record AND decodes greater than max   --> valid vin data, maintenance needed.
     * Invalid build record AND decodes less than max    --> bad vin data.
     * Invalid build record AND decodes greater than max --> bad vin data, maintenance needed.
     *
     * @param buildRecordValid
     *                   Indicates whether a valid build record was loaded.
     * @param validDecodes Number of decodes for the current label is less than the max allowed.
     * @return Command to send to the CLV scanner.
     * @since Version 1.0
     */
    const std::string EvaluateStatus(const bool &buildRecordValid, const bool &validDecodes);
    /**
     * Send a command to the CLV 295 barcode scanner.
     *
     * @param command The command to send.
     */
    virtual BEP_STATUS_TYPE SendCommandToCLV(const std::string &command);
    /**
     * Get the echo from the CLV scanner for the command that was just sent.
     *
     * @param command Command that we are looking for an echo for.
     * @return Status of the echo.
     * @since Version 1.0
     */
    virtual BEP_STATUS_TYPE GetCommandEcho(const std::string &command);
    /**
     * Send a command to the CLV to illuminate the specified stack lights.
     *
     * @param light  Which light or lights to illuminate.
     * @param duration Length of time in ms to leave light illuminated.
     * @since Version 1.0
     */
    virtual void FlashStackLight(const std::string &light, const INT32 &duration);
    /**
     * Test the operation of the stack light lamps.
     * @since Version 1.0
     */
    void TestStackLightLamps(void);
    /**
     * Store the CLV commands listed in the configuration file.
     *
     * @param commands Valid CLV commands.
     * @since Version 1.0
     */
    void SetCLVCommands(const XmlNode *commands);
    /**
     * Store the length of time in ms to leave the stack light illuminated.
     *
     * @param duration Time in ms the stack light should remain illuminated.
     * @since Version 1.0
     */
    void SetStackLightOnDuration(const INT32 &duration);
    /**
     * Store the length of time in ms to leave the stack light illuminated.
     *
     * @param duration Time in ms the stack light should remain illuminated.
     * @since Version 1.0
     */
    void SetStackLightOnDuration(const std::string &duration);
    /**
     * Length of time in ms to leave stack light illuminated during a lamp test.
     *
     * @param delay  Time in ms the light should remain illuminated.
     * @since Version 1.0
     */
    void SetStackLightTestDelay(const INT32 &delay);
    /**
     * Length of time in ms to leave stack light illuminated during a lamp test.
     *
     * @param delay  Time in ms the light should remain illuminated.
     * @since Version 1.0
     */
    void SetStackLightTestDelay(const std::string &delay);
    /**
     * Store the maximum number of times to send a command before failing.
     *
     * @param attempts Number of attempts for each command.
     * @since Version 1.0
     */
    void SetMaxAttempts(const INT32 &attempts);
    /**
     * Store the maximum number of times to send a command before failing.
     *
     * @param attempts Number of attempts for each command.
     * @since Version 1.0
     */
    void SetMaxAttempts(const std::string &attempts);
    /**
     * Store the time to delay in ms between sends of the same command.
     *
     * @param delay  Time to delay in ms.
     * @since Version 1.0
     */
    void SetCommandDelay(const INT32 &delay);
    /**
     * Store the time to delay in ms between sends of the same command.
     *
     * @param delay  Time to delay in ms.
     * @since Version 1.0
     */
    void SetCommandDelay(const std::string &delay);
    /**
     * Store the maximum size of the scanner buffer.
     *
     * @param bufferSize Size of the buffer in bytes.
     * @since Version 1.0
     */
    void SetMaxBufferSize(const INT32 &bufferSize);
    /**
     * Store the maximum size of the scanner buffer.
     *
     * @param bufferSize Size of the buffer in bytes.
     * @since Version 1.0
     */
    void SetMaxBufferSize(const std::string &bufferSize);
    /**
     * Store the delay time in ms.
     *
     * @param delay  Length of delay time in ms.
     * @since Version 1.0
     */
    void SetDataDelay(const INT32 &delay);
    /**
     * Store the delay time in ms.
     *
     * @param delay  Length of delay time in ms.
     * @since Version 1.0
     */
    void SetDataDelay(const std::string &delay);
    /**
     * Set the maximum number of decodes allowed for each label.
     *
     * @param decodes Number of decodes allowed.
     * @since Version 1.0
     */
    void SetMaxDecodes(const INT32 &decodes);
    /**
     * Set the maximum number of decodes allowed for each label.
     *
     * @param decodes Number of decodes allowed.
     * @since Version 1.0
     */
    void SetMaxDecodes(const std::string &decodes);
    /**
     * Set the length of the reader ID field in the scanner response.
     *
     * @param length Length of the field.
     * @since Version 1.0
     */
    void SetReaderIDLength(const INT32 &length);
    /**
     * Set the length of the reader ID field in the scanner response.
     *
     * @param length Length of the field.
     * @since Version 1.0
     */
    void SetReaderIDLength(const std::string &length);
    /**
     * Store the length of the FIS decode field in the scanner response.
     *
     * @param length Length of the field.
     * @since Version 1.0
     */
    void SetFISDecodeLength(const INT32 &length);
    /**
     * Store the length of the FIS decode field in the scanner response.
     *
     * @param length Length of the field.
     * @since Version 1.0
     */
    void SetFISDecodeLength(const std::string &length);
    /**
     * Store the flag for the number of checks to make for a valid build record prior
     * to continuing with processing data.
     *
     * @param attempts  Number of checks to make for a valid build record.
     */
    void SetValidRecordAttempts(const INT32 &attempts);
    /**
     * Store the amount of time to wait between each check for a valid build record.
     *
     * @param checkDelay  Time in ms to wait between each check for a valid build record.
     */
    void SetValidRecordCheckDelay(const INT32 &checkDelay);
    /**
     * Determine if the current vehicle build record has a valid
     * status
     *
     * @return true if the build record status is valid and false if
     *         invalid
     */
    bool IsBuildRecordValid(void);

private:
    /**
     * Condition variable to indicate a vehicle is at the barcode scanner.
     * @since Version 1.2
     */
    BepCondVar<std::string, false> m_vehicleAtScanner;
    /**
     * Condition informing that the build record status was received.
     * @since Version 1.0
     */
    BepCondVar<std::string, false> m_buildRecordStatusReceived;
    /**
     * CLV scanner communication object.
     * @since Version 1.0
     */
    SerialChannel m_clvScannerComm;
    /**
     * Valid commands that can be sent to the CLV scanner.
     * @since Version 1.0
     */
    XmlNodeMap m_clvCommands;
    /**
     * Length of time in ms to leave stack light illuminated.
     * @since Version 1.0
     */
    INT32 m_stackLightOnDuration;
    /**
     * Length of time to leave stack light illuminated during a test of the lights.
     * @since Version 1.0
     */
    INT32 m_stackLightTestDelay;
    /**
     * Maximum number of times to send a command before failing and loggin an error.
     * @since Version 1.0
     */
    INT32 m_maxAttempts;
    /**
     * Length of time in ms to wait between sending commands.
     * @since Version 1.0
     */
    INT32 m_commandDelay;
    /**
     * Maximum number of bytes to hold in a buffer.
     * @since Version 1.0
     */
    INT32 m_maxBufferSize;
    /**
     * Time to wait between checks for characters at the port.
     * @since Version 1.0
     */
    INT32 m_dataDelay;
    /**
     * Maximum number of decodes allowed before maintenance is alerted.
     * @since Version 1.0
     */
    INT32 m_maxFisDecodes;
    /**
     * Length of the Reader ID field in the scanner response.
     * @since Version 1.0
     */
    INT32 m_readerIDLength;
    /**
     * Length of the FIS decodes field in the scanner response.
     * @since Version 1.0
     */
    INT32 m_fisDecodeLength;
    /**
     * The number of checks to make for a valid build record brfore moving on.
     */
    INT32 m_validRecordReceivedAttempts;
    /**
     * The amount of time to waut between checks for a valid build record.
     */
    INT32 m_validRecordCheckDelay;
    /**
     * Mutex lock variable for CLV scanner data.
     * @since Version 1.0
     */
    mutable pthread_mutex_t m_clvScannerDataLock;
    /**
     * Value of the Vin length
     */
    INT32 m_vinLength;
    /**
     * Flag to tell if we should open the logical port.  This allows us to run
     * the unit tests without the full system.
     */
    bool m_openLogicalPort;
};

#endif  // CLV295ScannerMgr_h
