//*************************************************************************
// FILE DESCRIPTION:
//  The MapleKeypadManager handles all keypad entries and publishes the data to
//  all subscribers.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/Include/MapleKeypadManager.h $
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
// system.  Added LoadAdditionalConfigurationItems() which is used to
// reload the configuration without restarting the process.
//
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
// 1     4/29/05 8:00a Gpattison
// Refactored the entire MapleKeypad Manager so that the InputServer could
// spawn it as an individual input device instead of having it as an object.
// This allows it to be added or removed from the system by making
// only configuration file changes.  It is now a stand alone BepServer type
// executable.
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
// 1     6/12/03 4:17p Khykin
// Initial checkin.
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:43  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:07a Khykin
// Initial Checkin
//
// 1     1/18/03 8:04a Khykin
// Initial Checkin From DCTC
//
// 1     11/26/02 4:55p Derickso
// New Input Device manager.
//
//*************************************************************************
#ifndef MapleKeypadManager_h
#define MapleKeypadManager_h
//===========================================================================

#include "InputServerDefs.h"
#include "InputDeviceBase.h"
#include "SerialChannel.h"

/**
 * Miscellaneous constants to use.
 * @since Version 1.0
 */
#define CR           '\x00D'    /* Carriage Return */
#define LF           '\x00A'    /* Line Feed */
#define ESC          '\x01B'    /* Escape */
#define TERMINATOR   '\x002'    /* Command terminator */
#define CLEAR_SCREEN '\x00C'    /* Clear the keypad display */


/**
 * MapleKeyPadManager.
 * The MapleKeypadManager will manage the Maple Keypads.  The manager will determine the
 * type of data entered on the keypad and make it available to all objects that require
 * the data.  The MapleKeypadManager will also display prompts on the maple keypad for
 * test operators.
 *
 * @author Dan Erickson
 * @version Version 1.0
 * @see AtomicThread
 * @see InputServer
 * @since November 20, 2002
 */
class MapleKeypadManager : public InputDeviceBase
{
    friend class MapleKeypadManagerTest;

public:
    /**
     * Default constructor.
     */
    MapleKeypadManager();
    /**
     * Default destructor.
     * @since Version 1.0
     */
    ~MapleKeypadManager();
    /**
     * Initializes the manager to talk to the keypad as well as any data that needs to
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
     * Set the current state of the Maple Keypad.
     *
     * @param state  State to set the keypad to.
     * @since Version 1.0
     */
    void SetCurrentState(const std::string &state);
    /**
     * Return the size of the keypad buffer.
     *
     * @return Size of the buffer in bytes.
     * @since Version 1.0
     */
    inline INT32 &GetBufferSize(void);
    /**
     * Get the current mode of the Maple Keypad.
     *
     * @return Return the current mode of the keypad.
     * @since Version 1.0
     */
    inline std::string &GetCurrentMode(void);
    /**
     * Get the maximum number of attempts to make.
     *
     * @return Maximum number of attempts to retrieve data.
     * @since Version 1.0
     */
    inline const INT32 &GetMaximumRetries(void);
    /**
     * Return the amount of time to wait between attempts to retrieve data.
     *
     * @return Maximum time delay in ms.
     * @since Version 1.0
     */
    inline const INT32 &GetDataDelay(void);

protected:
    /**
     * Load the additional configuration items needed by the MapleKeypadManager.
     * The "standard" items will be handled by the call to
     * InputDeviceBase::Initialize().
     *
     * @param configNode A pointer to the MapleKeypadManager config node
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *configNode);
    /**
     * Enable Block mode for the keypad display.
     * @since Version 1.0
     */
    void EnableBlockMode(void);
    /**
     * Disable block mode for the keypad display.
     * @since Version 1.0
     */
    void DisableBlockMode(void);
    /**
     * Clear all text from the keypad display.
     * @since Version 1.0
     */
    void ClearKeypadDisplay(void);
    /**
     * Set the position of the cursor on the keypad display.
     *
     * @param row    Row in which to position cursor.
     * @param column Column in which to position cursor.
     * @since Version 1.0
     */
    void PositionCursor(const std::string row, const std::string column);
    /**
     * Set the position of the cursor on the keypad display.
     *
     * @param row    Row in which to position cursor.
     * @param column Column in which to position cursor.
     * @since Version 1.0
     */
    void PositionCursor(const INT16 row, const INT16 column);
    /**
     * Displays a message on the keypad display.
     *
     * @param message Tag of the message to be displayed.
     * @since Version 1.0
     */
    void DisplayMessage(std::string message);
    /**
     * Displays a message on the keypad display.
     *
     * @param message Text to be displayed on the keypad.
     * @param row     The row index for the message.
     * @param column  The column to begin displaying the message.
     * @since Version 1.0
     */
    void DisplayMessage(std::string message, const std::string row, const std::string column);
    /**
     * Determine the type of data received from the keypad.
     *
     * @param data      Data from the Maple Keypad.
     * @param byteCount Number of bytes received from the keypad.
     * @since Version 1.0
     */
    void EvaluateData(unsigned char *data, const INT32 &byteCount);
    /**
     * Utility to print the contents of a buffer before being sent.
     *
     * @param buffer Buffer to be sent.
     * @since Version 1.0
     */
    void PrintSendBuffer(const std::string &buffer);
    /**
     * Waits for the Maple Keypad to be running in the task monitor
     */
    void MapleKeypadManager::WaitForKeypad(void);
    /**
     * Sets the name of the keypad based on the information
     * found in the appropriate configuration file.
     * 
     * @param configNode Starting node in the configuration file
     *                   (Expected to be in MapleKeypadCommunication in InputServerConfig.xml)
     */
    void MapleKeypadManager::SetName(const XmlNode *configNode);
    /**
     * Sets the amount of time to wait before attempting a reconnect.
     * 
     * @param delay_ms amount of time to wait in milliseconds
     */ 
    inline void MapleKeypadManager::SetReconnectDelay(int delay_ms);
    /**
     * Store the current mode of the Maple Keypad.
     *
     * @param mode   Current mode (VIN, Body Style, Driver Number, etc.)
     * @since Version 1.0
     */
    inline void SetCurrentMode(const std::string mode);
    /**
     * Store the size of the keypad buffer.
     *
     * @param bufferSize Size of the buffer in bytes.
     * @since Version 1.0
     */
    inline void SetBufferSize(const INT32 bufferSize);
    /**
     * Store the size of the keypad buffer.
     *
     * @param bufferSize Size of the buffer in bytes.
     * @since Version 1.0
     */
    inline void SetBufferSize(const std::string &bufferSize);
    /**
     * Store the amount of time to wait between attempts to retrieve data from buffer.
     *
     * @param delay  Time to wait in milliseconds.
     * @since Version 1.0
     */
    inline void SetDataDelay(const INT32 delay);
    /**
     * Store the amount of time to wait between attempts to retrieve data from buffer.
     *
     * @param delay  Time to wait in milliseconds.
     * @since Version 1.0
     */
    inline void SetDataDelay(const std::string &delay);
    /**
     * Store the maximum times no data is retrieved from the buffer before
     * we determine that all data has been received.
     *
     * @param maxRetries Maximum times to retry.
     * @since Version 1.0
     */
    inline void SetMaximumRetries(const INT32 maxRetries);
    /**
     * Store the maximum times no data is retrieved from the buffer before
     * we determine that all data has been received.
     *
     * @param maxRetries Maximum times to retry.
     * @since Version 1.0
     */
    inline void SetMaximumRetries(const std::string &maxRetries);

	/**
	 * Set the maximum vehicle number length.
	 * 
	 * @param maxVehicleNumberLength - The maximum vehicle number length.
	 */
	inline void SetMaxVehicleNumberLength(const UINT16 maxVehicleNumberLength);
	/**
	 * Get the maximum vehicle number length.
	 * 
	 * @return The maximum vehicle number length.
	 */
	inline UINT16 GetMaxVehicleNumberLength(void);

private:
    /**
     * Get/Set the flag indicating a secondary data item has been configured.
     * 
     * @param secdataSupported
     *               Flag indicating if a secondary data item has been configured.
     * 
     * @return Flag indicating if a secondary data item has been configured.
     */
    const bool& SecondaryDataSupported(const bool *secdataSupported = NULL);
    /**
     * Get/Set the data tag for the secondary data type.
     * 
     * @param secDataType
     *               Data tag to use for the secondary data item.
     * 
     * @return Data tag to use for the secondary data item.
     */
    const string& SecondaryDataType(const string *secDataType = NULL);
    /**
     * Get/Set the length of the secondary data item.
     * 
     * @param dataLength Length of the secondary data item.
     * 
     * @return Length of the secondary data item.
     */
    const INT32& SecondaryDataLength(const INT32 *dataLength = NULL);
    /**
     * Get/Set the length of the VIN.
     * 
     * @param length Length of the VIN.
     * 
     * @return Length of the VIN.
     */
    const INT32& VinLength(const INT32 *length = NULL);
    /**
     * Communication object to talk with the Maple Keypad.
     * @since Version 1.0
     */
    SerialChannel m_keypadComm;
    /**
     * Current input mode.
     * @since Version 1.0
     */
    std::string m_currentMode;
    /**
     * Size of the buffer for the Maple Keypad.
     * @since Version 1.0
     */
    INT32 m_bufferSize;
    /**
     * Name of the Maple Keypad, denoting whether
     * it is LeftMapleKeypad or RightMapleKeypad
     */
    std::string m_name;
    /**
     * Amount of delay between incoming characters.
     * @since Version 1.0
     */
    INT32 m_dataDelay;
    /**
     * Maximum number of attempts to get data.
     * @since Version 1.0
     */
    INT32 m_maxRetries;
    /**
     * Amount of time to wait before reconnecting
     */
    int m_reconnectDelay;
    /**
     * Store the future state of the Input Server state.  Used to
     * allow the publish method to return immediately without
     * having to process the state change during the Publish.
     */
    string m_nextState;
    /**
     * Flag indicating that the Input Server State needs to be
     * updated to the value held in m_nextState
     */
    bool m_stateChanged;
    /**
     * Flag to tell if we should open the logical port.  This allows us to run
     * the unit tests without the full system.
     */
    bool m_openLogicalPort;
    /**
     * Flag indicating if a secondary data type is supported.
     */
    bool m_secondaryDataSupported;
    /**
     * Dat tag/type to usefr publishng the secondary data type.
     */
    string m_secondaryDataType;
    /**
     * Length of the secondary data item.
     */
    INT32 m_secondaryDataLength;
    /**
     * Length of the VIN.
     */
    INT32 m_vinLength;
	/**
	 * The maximum length of the vehicle number.  A value of two will allow
	 * a value of 0-99.
	 */
	UINT16 m_maxVehicleNumberLength;
};

#endif   // MapleKeypadManager_h
