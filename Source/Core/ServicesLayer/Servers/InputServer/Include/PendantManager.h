//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/Include/PendantManager.h 3     10/30/07 4:09p Rwiersem $
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/Include/PendantManager.h $
// 
// 3     10/30/07 4:09p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     6/07/06 5:36p Cward
// Changes to allow the Input Server, CLV295ScannerMgr,
// MapleKeypadManager, PendantManager, and SymbolScannerMgr configurations
// to be reloaed without restrting the processes.
//
// 1     6/05/06 3:46p Rwiersem
// Made the test class a friend.  Added a flag to tell if we should open
// the logical port.  This is used to run the unit tests without the full
// system.  Added LoadAdditionalConfigurationItems() which is used to
// reload the configuration without restarting the process.
//
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
// 2     4/29/05 8:00a Gpattison
// Refactored the entire Pendant Manager so that the InputServer could
// spawn it as an individual input device instead of having it as an object.
// This allows it to be added or removed from the system by making
// only configuration file changes.  It is now a stand alone BepServer type
// executable.
//
// 2     5/12/04 9:53p Cward
// Changes for the May 12 iteration
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
// 1     6/12/03 4:19p Khykin
// Initial checkin.
//
// 2     5/19/03 11:25a Khykin
// Updated with the function key definitions.
//
// 1     5/01/03 7:32a Derickso
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:44  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:08a Khykin
// Initial Checkin
//
// 1     1/18/03 8:05a Khykin
// Initial Checkin From DCTC
//
// 4     8/23/02 8:22a Dan.erickson
// Added functionality to only conditionally send DriverNumber to the GUI
// and to the VehicleDataBroker.
//
// 3     7/29/02 8:29a Dan.erickson
// Rewrote to be generic for all manufacturers.
//
// 2     6/07/02 11:54a Dan.erickson
// Final debug and Testing.
//
// 1     5/31/02 3:04p Dan.erickson
// Created and Debugged.
//
//*************************************************************************
#ifndef PendantManager_h
#define PendantManager_h
//===========================================================================

#include "InputServerDefs.h"
#include "InputDeviceBase.h"
#include "SerialChannel.h"

class InputServer;

/**
 * Special key mappings for the pendant.
 * @since Version 1.0
 */
#define UP       'U'
#define DOWN     'D'
#define LEFT     'L'
#define RIGHT    'R'
#define F1       '['
#define F2       ']'

/**
 * Input Mode setup information.
 * @since Version 1.2
 */
const std::string dataLength("DataLength");
const std::string dataMap("DataMap");
const std::string dataTag("DataTag");
const std::string defaultStartIndex("DefaultStartIndex");
const std::string displayTag("DisplayTag");
const std::string invalidKeyPress("InvalidKeyPress");
const std::string publishKeyPress("PublishKeyPress");
const std::string validKeyPad("ValidKeyPad");
/**
 * Data map field attributes
 * @since Version 1.2
 */
const std::string currentFieldIndicator("CurrentFieldIndicator");
const std::string defaultValue("DefaultValue");
const std::string typeField("Type");
/**
 * Key press values that require special handling.
 * @since Version 1.2
 */
const std::string abortValue("Abort");
const std::string decrementIndex("DecrementIndex");
const std::string incrementIndex("IncrementIndex");
const std::string nextCharacter("NextCharacter");
const std::string previousCharacter("PreviousCharacter");
const std::string functionKey1(REPRINT_RESULTS);
const std::string functionKey2("FunctionKey2");
/**
 * Valid data field types.
 * @since Version 1.2
 */
const std::string alphanumericType("Alphanumeric");
const std::string characterType("Character");
const std::string integerType("Integer");
/**
 * Types of data the pendant manages.
 * @since Version 1.0
 */
typedef enum
{
    VIN_DATA,
    DRIVER_NUMBER_DATA,
    CHARACTER_DATA,
} INPUT_DATA_TYPE;


/**
 * PendantManager class.
 * This class manages all data entered on the DCX style pendant.
 * The PendantManager has three modes:
 * Normal -- All key presses are published under the tag KeyPress with the key press as the value.
 * VIN -- The left and right arrow keys move forward and backward through the vin string.  The up and
 *        down arrows cycle through letters A-Z.  The number keys just insert the corresponding number.
 *        When the index reaches the end of the string, the value is automatically written to the
 *        WorkCellController so the build record can be obtained.  As each character is entered, it is
 *        also displayed in the VINDisplay area of the GUI.
 * DriverNumber -- behavior is similar to the VIN mode.  Characters are written to the DriverNumberDisplay
 *        area of the GUI.
 *
 * @author Dan Erickson
 * @version Version 1.2
 * @see PendantManager
 */
class PendantManager : public InputDeviceBase
{
    friend class PendantManagerTest;

public:
    /**
     * Default constructor for the Pendant manager.
     *
     * @since Version 1.0
     */
    PendantManager();
    /**
     * Default destructor.
     * @since Version 1.0
     */
    virtual ~PendantManager();
    /**
     * Initializes the pendant manager to talk to the pendant as well as any data that needs to
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
     * Processes the key that was just pressed.  If the input server is in vin state, the vin data will
     * be manipulated.  If the input server is in driver number state, the driver number data will be
     * manipulated.  If the input server is in Normal state, the data will be published to all subscribers.
     *
     * @param keyPress The pendant key that was pressed.
     * @since Version 1.0
     */
    void ProcessKeyPress(const char &keyPress);
    /**
     * Reset the vin and driver number character arrays for the next round of input.
     * The arrays will be reset to default values as follows:
     *           - VIN: ?????????Model YearPlant Code______
     *           - Driver Number: T______
     *
     * @param inputItem Which array to reset.
     * @since Version 1.0
     */
    virtual void ResetData(const INPUT_DATA_TYPE &inputItem);
    /**
     * Store the current state of the PendantManager.
     *
     * @param state  State to set the PendantManager to.
     * @since Version 1.0
     */
    void SetCurrentState(const std::string &state);
    /**
     * Return the current state of the Pendant.
     *
     * @return Current input state.
     * @since Version 1.0
     */
    inline const std::string& GetCurrentState(void);
    /**
     * Get the name of the GUI display tag.
     *
     * @return Name of the tag.
     * @since Version 1.2
     */
    inline const std::string& GetDisplayTag(void);
    /**
     * Return the tag to use for reporting the final data string.
     *
     * @return Name of the tag.
     * @since Version 1.2
     */
    inline const std::string& GetInputDataTag(void);
    /**
     * Return the value mapped to by the key that was pressed.
     * For example, if the left arrow was pressed, the value "Pass" might be returned.
     *
     * @param keyPress The key that was pressed.
     * @return Value of the key.
     * @since Version 1.0
     */
    const std::string GetKeyPressValue(const char &keyPress);
    /**
     * Get the publish keypress tag.
     *
     * @return True - Publish all key presses.
     *         False - Do not publish key presses.
     * @since Version 1.2
     */
    inline const bool& PublishKeypress(void);
    /**
     * Get the length of the input data string.
     *
     * @return Length of the data string.
     * @since Version 1.2
     */
    inline const INT32& GetInputDataLength(void);
    /**
     * Get the current input data.
     *
     * @return Input data string.
     * @since Version 1.2
     */
    inline std::string& GetInputData(void);
    /**
     * Get the current index into the input data string.
     *
     * @return Current index.
     * @since Version 1.2
     */
    inline const INT32& GetCurrentIndex(void);
    /**
     * Get the current field indicator.
     *
     * @return Field indicator character.
     * @since Version 1.2
     */
    inline const unsigned char& GetCurrentFieldIndicator(void);

protected:
    /**
     * Load the additional configuration items needed by the PendantManager.
     * The "standard" items will be handled by the call to
     * InputDeviceBase::Initialize().
     *
     * @param configNode A pointer to the Pendant config node
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *configNode);
    /**
     * Store the key pad to value mapping.
     *
     * @param keyPad The mapping to be stored.
     * @since Version 1.0
     */
    inline void SetKeyPad(const XmlNode *keyPad);
    /**
     * Set the GUI display tag.
     *
     * @param tag    Name of the tag.
     * @since Version 1.2
     */
    inline void SetDisplayTag(const std::string &tag);
    /**
     * Set the tag for reporting the final data string.
     *
     * @param tag    Name of the tag.
     * @since Version 1.2
     */
    inline void SetInputDataTag(const std::string &tag);
    /**
     * Add a character to the input string.
     *
     * @param character
     *               Character to add the input data string.
     * @since Version 1.0
     */
    void AddInputCharacter(const unsigned char &character);
    /**
     * Increment the current input data index.
     *
     * @param increment Amount to increment the index by.
     * @since Version 1.0
     */
    void IncrementInputDataIndex(const INT16 &increment = 1);
    /**
     * Decrement the input data index.
     *
     * @param decrement Amount to decrement the array index.
     * @since Version 1.0
     */
    void DecrementInputDataIndex(const INT16 &decrement = 1);
    /**
     * Set the current character to the specified value.
     *
     * @param character Character to set.
     * @since Version 1.0
     */
    inline void SetCharacter(const char &character = 'A');
    /**
     * Increment the current character.  Wraps around to 'A' after 'Z' is incremented.
     *
     * @return The new character.
     * @since Version 1.0
     */
    inline char& IncrementCharacter(void);
    /**
     * Decrement the current character.  Wraps around to 'Z' after 'A' is incremented.
     *
     * @return The new character.
     * @since Version 1.0
     */
    inline char& DecrementCharacter(void);
    /**
     * Makes the determination of whether a vin update is
     * allowed.
     * A Vin update is allowed under the following conditions:
     *    - No valid build record is loaded.
     *    - A valid build record is loaded and the operator presses the Abort button.
     *
     * @return True - Vin updates are allowed.
     *         False - Vin updates are NOT allowed.
     * @since Version 1.1
     */
    inline const bool IsVinUpdateAllowed(void);
    /**
     * Makes the determination of whether a driver number update is allowed.
     * A driver number update is allowed under the following conditions:
     *      - The current driver number is the same as the default driver number
     *      - The Abort(0) key is pressed when the current driver number is not the default driver number.
     *
     * @return True - Driver number updates are allowed.
     *         False - Driver number updates are not allowed.
     * @since Version 1.3
     */
    inline const bool IsDriverNumberUpdateAllowed(void);
    /**
     * Stores whether the abort button has been pressed.
     *
     * @param abortPressed True if the abort button has been pressed.
     * @since Version 1.1
     */
    inline void SetAbortButtonPressed(const bool &abortPressed);
    /**
     * Returns whether or not the abort button has been pressed.
     *
     * @return True if the abort button has been pressed.
     * @since Version 1.1
     */
    inline const bool& HasAbortButtonBeenPressed(void);
    /**
     * Store the input mode configuration data.
     *
     * @param inputModes Valid input modes.
     * @since Version 1.2
     */
    inline void SetModeConfigurations(const XmlNode *inputModes);
    /**
     * Set the publish key press flag.
     *
     * @param publishKeyPress
     *               True - Publish all key presses.
     *               False - Do not publish key presses.
     * @since Version 1.2
     */
    inline void SetPublishKeypress(const bool &publishKeyPress);
    /**
     * Set the valid length of the input data.
     *
     * @param length Length of the data string.
     * @since Version 1.2
     */
    inline void SetInputDataLength(const INT32 &length);
    /**
     * Set the valid length of the input data.
     *
     * @param length Length of the data string.
     * @since Version 1.2
     */
    inline void SetInputDataLength(const std::string &length);
    /**
     * Store the data map for the current input mode.
     *
     * @param dataMap Data map.
     * @since Version 1.2
     */
    inline void SetInputDataMap(const XmlNode *dataMap);
    /**
     * Set the current index into the input data string.
     *
     * @param index  input string index.
     * @since Version 1.2
     */
    inline void SetCurrentIndex(const INT32 &index);
    /**
     * Set the current index into the input data string.
     *
     * @param index  input string index.
     * @since Version 1.2
     */
    inline void SetCurrentIndex(const std::string &index);
    /**
     * Stores the default current field character used for current index.
     *
     * @param indicator Character to use.
     * @since Version 1.2
     */
    inline void SetCurrentFieldIndicator(const unsigned char &indicator);
    /**
     * Returns the lowered status of the retaining rolls.
     *
     * @return True - Retaining rolls are down / Elevators up.
     * @since Version 1.3
     */
    bool RollsAreDown(void);
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
     * SerialChannel object to use to get key presses from the pendant.
     * @since Version 1.0
     */
    SerialChannel m_pendantComm;
    /**
     * Current character selected using the arrow keys.  Has a range 'A' to 'Z' inclusive.
     * @since Version 1.0
     */
    char m_currentCharacter;
    /**
     * The GUI display tag for where to place the keypress information.
     * @since Version 1.2
     */
    std::string m_displayTag;
    /**
     * Tag to use for reporting the data string.
     * @since Version 1.2
     */
    std::string m_inputDataTag;
    /**
     * Holds the current state of the PendantManager.  Valid states are Normal, Vin, and DriverNumber.
     * @since Version 1.0
     */
    std::string m_currentState;
    /**
     * Data string being input by the operator.
     * @since Version 1.2
     */
    std::string m_inputData;
    /**
     * Length of the data string being input by the operator.
     * @since Version 1.2
     */
    INT32 m_inputDataLength;
    /**
     * Current index into the input field data.
     * @since Version 1.2
     */
    INT32 m_currentIndex;
    /**
     * The character to place at the current index to indicate where the next character will be placed.
     * @since Version 1.2
     */
    unsigned char m_currentFieldIndicator;
    /**
     * Stores wheter the abort button has been pressed.
     * @since Version 1.1
     */
    bool m_abortButtonPressed;
    /**
     * Flag for publishing key presses.
     * @since Version 1.2
     */
    bool m_publishKeyPress;
    /**
     * Stores the mapping of keys on the pendant to values to return.
     * @since Version 1.0
     */
    XmlNodeMap m_keyPad;
    /**
     * Map to hold the valid pendant modes and the data and actions associated with each mode.
     * @since Version 1.2
     */
    XmlNodeMap m_pendantModes;
    /**
     * Holds the map for the input string and what values are valid.
     * @since Version 1.2
     */
    XmlNodeMap m_inputDataMap;
    /**
     * Store the future state of the Input Server state.  Used to
     * allow the publish method to return immediately without
     * having to process the state change during the Publish.
     */
    string m_nextState;
    /**
     * Store the current VINReadStatus value so we can track which 
     * state it is changeing from. Transition from yellow indicates 
     * the build request is complete. 
     */
    string m_prevVinReadStatus;
    /**
     * Flag indicating that the Input Server State needs to be
     * updated to the value held in m_nextState
     */
    bool m_stateChanged;
    /**
     * Flag used to prevent unnecessary state changes that create race conditoins.
     * State changes are blocked after a new VIN is entered, and unblocked after the
     * VIN read status transistions from yellow, and a vehilce is present
     */
    bool m_blockStateChanges;
    /**
     * Flag used to track the WCC build request status for determining when to unblock
     * state changes
     */
    bool m_buildReqComplete;
    /**
     * Flag to tell if we should open the logical port.  This allows us to run
     * the unit tests without the full system.
     */
    bool m_openLogicalPort;
};
#endif  // PendantManager.h
