//*************************************************************************
// FILE DESCRIPTION:
//	The Pendantmanager handles all pendant inputs and publishes the key presses to
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/PendantManager/PendantManager.cpp $
// 
// 2     6/07/06 5:36p Cward
// Changes to allow the Input Server, CLV295ScannerMgr,
// MapleKeypadManager, PendantManager, and SymbolScannerMgr configurations
// to be reloaed without restrting the processes.
//
// 1     6/05/06 3:47p Rwiersem
// Added a flag to tell if we should open the logical port.  This is used
// to run the unit tests without the full system.  Added
// LoadAdditionalConfiguration() items which is used to reload the
// configuration without restarting the process.
//
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
// 3     4/29/05 8:00a Gpattison
// Refactored the entire Pendant Manager so that the InputServer could
// spawn it as an individual input device instead of having it as an object.
// This allows it to be added or removed from the system by making
// only configuration file changes.  It is now a stand alone BepServer type
// executable.
//
// 3     5/12/04 9:53p Cward
// Changes for the May 12 iteration
//
// 2     4/28/04 10:54p Cward
// Changes associated with build 040402801
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
// 2     5/19/03 12:23p Khykin
// Updates for publishing the F1 and F2 keys.
//
// 1     5/01/03 7:29a Derickso
//
// 2     4/07/03 9:29a Khykin
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
// 5     8/23/02 9:36a Dan.erickson
// Updated to allow conditional updating of DriverNumber.
//
// 4     8/14/02 3:44p Skypig13
// Removed compiler warnings
//
// 3     7/29/02 8:29a Dan.erickson
// Rewrote to be generic for all manufacturers.
//
// 2     6/07/02 11:53a Dan.erickson
// Final debug and testing.
//
// 1     5/31/02 3:03p Dan.erickson
// Created and Debugged.
//
//*************************************************************************

#include "PendantManager.h"
#include "GeneralUtilities.h"

PendantManager::PendantManager() : InputDeviceBase(),
m_currentCharacter('@'),m_currentState(INPUT_SERVER_NORMAL_STATE), m_currentIndex(0),
m_abortButtonPressed(true),m_publishKeyPress(true),m_nextState(INPUT_SERVER_NORMAL_STATE),
m_prevVinReadStatus(NO_VIN),m_stateChanged(false),m_blockStateChanges(false),m_buildReqComplete(false),
m_openLogicalPort(true)
{   // Nothing to do here
}

PendantManager::~PendantManager()
{   // Clear the key pad mapping
    m_keyPad.clear(true);
}

void PendantManager::Initialize(const XmlNode *configNode)
{
    // Call the base class
    InputDeviceBase::Initialize(configNode);

    Log(LOG_DEV_DATA,"PendantManager::Initialize()\n");

    // Load the additional configuration items needed by the PendantManager
    LoadAdditionalConfigurationItems(configNode);
    Log(LOG_DEV_DATA, "LoadAdditionalConfigurationItems() - Complete"); 
    // Initialize the data
    ResetData(CHARACTER_DATA);
    Log(LOG_DEV_DATA, "ResetData() - Complete"); 
    SetAbortButtonPressed(false);
    Log(LOG_DEV_DATA, "SetAbortButtonPressed() - Complete"); 
    // Default to Normal State
    SetCurrentState(INPUT_SERVER_NORMAL_STATE);
    Log(LOG_DEV_DATA,"PendantManager::Initialize() complete -- Current state: %s.\n", GetCurrentState().c_str());
}

void PendantManager::LoadAdditionalConfigurationItems(const XmlNode *configNode)
{
    const XmlNode *document = configNode->getChild("Setup");

    // if we should open the logical ports
    if(m_openLogicalPort == true)
    {
        // Get the serial channel config data
        m_pendantComm.Initialize(document->getChild("PendantCommunication"));
    }

    // Save the pendant mode configurations
    SetModeConfigurations(document->getChild("PendantModes"));
}

const std::string PendantManager::Publish(const XmlNode *node)
{
    Log(LOG_DEV_DATA,"PendantManager::Publish() - (%s)\n",node->ToString().c_str());

    if (node->getName() == INPUT_SERVER_STATE)
    {
        // Set the pendant mode if this  is a state change
        Log(LOG_DEV_DATA, "Setting Pendant Manager to state %s\n", node->getValue().c_str());
        // Store the value of the future Input Server state to a member variable
        //  and set a flag indicating that the input server state needs to be updated.
        //  This allows the thread calling the Publish method to return immediately
        //  without having to do all the processing involved in changing the
        //  input server state.
        m_nextState = node->getValue();
        m_stateChanged = true;
		if(!node->getValue().compare(INPUT_SERVER_DRIVER_NUMBER_STATE))
		{   // Only a test component places the Input server into driver number mode
			// Unblock state changes
			Log(LOG_DEV_DATA, "Driver Number state enabled, unblocking state changes.");
			m_blockStateChanges = false;
			m_buildReqComplete = true;
		}
    }
    if (node->getName() == NEXT_VIN_DATA_TAG)
    {
        Log(LOG_DEV_DATA, "New VIN Scanned, setting state to TestResult, blocking state changes");
        SetCurrentState(INPUT_SERVER_TEST_RESULT_STATE);
        m_blockStateChanges = true;
        m_buildReqComplete = false;
    }
    if (node->getName() == VEH_PRESENT_DATA_TAG)
    {   // unblock if vehicle is present and Vehicle Build read is complete
        if (m_blockStateChanges && (node->getValue() == "1") && m_buildReqComplete)
        {
            m_blockStateChanges = false; 
            Log(LOG_DEV_DATA, "Vehicle present and build record request complete - unblocking state changes");
        }
    }
    if (node->getName() == VIN_READ_STATUS_TAG)
    {
        if ((m_prevVinReadStatus == PROCESSING_VIN) && (node->getValue() != PROCESSING_VIN))
        {   // unblock if vehicle is present and Vehicle Build request is complete
            m_buildReqComplete = true; 
            if (Read(VEH_PRESENT_DATA_TAG) == "1")
            {
                m_blockStateChanges = false; 
                Log(LOG_DEV_DATA, "Build record request complete and vehilce present - unblocking state changes");
            }
        }
        m_prevVinReadStatus = node->getValue();
    }
    // Call the base class method to do it's thing
    return(BepServer::Publish(node));
}

void PendantManager::Run(volatile bool *terminateFlag) /* =NULL */
{
    Log(LOG_DEV_DATA,"PendantManager::Execute() begin\n");
    unsigned char keyPress[128];
    INT32 byteCount;
    // Look for keypresses until the InputServer tells us to stop
    while (GetStatus() != BEP_TERMINATE)
    {   // Wait for a key to be pressed
        Log(LOG_DEV_DATA,"Waiting for Key Press\n");
        byteCount = m_pendantComm.Get(keyPress, 128);
        // Check if the current input server state needs to be updated
        if (m_stateChanged && !m_blockStateChanges)
        {
            SetCurrentState(m_nextState);
            m_stateChanged = false;
        }
        // Process the key press
        if (byteCount > 0)
        {   // Process all the key presses that have been queued up
            for (INT16 ii = 0; ii < byteCount; ii++)
            {
                Log(LOG_DEV_DATA,"Key Pressed: %c <$%02X>\n", isprint(keyPress[ii]) ? keyPress[ii] : '?', keyPress[ii]);
                ProcessKeyPress(keyPress[ii]);
            }
        }
    }
}

void PendantManager::ProcessKeyPress(const char &keyPress)
{
    Log(LOG_DEV_DATA,"PendantManager::ProcessKeyPress() begin\n");
    // Translate the key press
    string response;    // used for receiving the reponse from a Write() to the input server
    std::string keyValue = GetKeyPressValue(keyPress);
    try
    {
        Log(LOG_FN_ENTRY, "PendantManager::ProcessKeyPress() -- pendantState: %s, keyValue: %s\n", GetCurrentState().c_str(), keyValue.c_str());
        // Verify that the key press was valid
        if (invalidKeyPress != keyValue)
        {   // Create a node for the key press
            XmlNode keyPressNode(PENDANT_KEY_PRESS, keyValue);
            // Determine how to process the keypress
            if (nextCharacter == keyValue)
            {   // Increment the current character and add it to the input string
                AddInputCharacter(IncrementCharacter());
            }
            else if (previousCharacter == keyValue)
            {   // Decrement the current character and add it to the input string
                AddInputCharacter(DecrementCharacter());
            }
            else if (incrementIndex == keyValue)
            {   // Increment the data string index
                IncrementInputDataIndex();
                // Place an index character in the current field
                AddInputCharacter(GetCurrentFieldIndicator());
                // Reset the current character data
                ResetData(CHARACTER_DATA);
            }
            else if (decrementIndex == keyValue)
            {   // Decrement the data string index
                DecrementInputDataIndex();
                // Reset the current character data
                ResetData(CHARACTER_DATA);
            }
            else if ((INPUT_SERVER_TEST_RESULT_STATE == GetCurrentState()) &&
                     RollsAreDown() && (abortValue == keyValue))
            {   // Flag the abort button as being pressed
                SetAbortButtonPressed(true);
                // Reset the VIN mode
                XmlNode pendantState(INPUT_SERVER_STATE, INPUT_SERVER_VIN_STATE);
                m_inputServerComm->Write(&pendantState,response,true);
            }
            else if ((INPUT_SERVER_DRIVER_NUMBER_STATE == GetCurrentState()) &&
                     !HasAbortButtonBeenPressed() && (abortValue == keyValue))
            {   // Flag the abort button as being pressed and allow update
                SetAbortButtonPressed(true);
            }
            else
            {   // Add the key value to the input data
                AddInputCharacter(keyPress);
            }
            // Determine if the key press should be published.  F1 key is always published.
            if (PublishKeypress() || (functionKey1 == keyValue) || (functionKey2 == keyValue))
            {   // Write the key press node to the input server for publishing
                m_inputServerComm->Write(&keyPressNode,response,true);
            }
        }
        else
        {   // Log that an invalid key was pressed
            Log(LOG_ERRORS, "Invalid key press (%c) for %s mode\n", keyPress, GetCurrentState().c_str());
        }
    }
    catch (XmlException ex)
    {
        Log(LOG_DEV_DATA,"XmlException in ProcessKeyPress - %s\n", ex.what());
    }
    catch (...)
    {
        Log(LOG_DEV_DATA,"Unknown Exception in ProcessKeyPress\n");

    }
    // Print out the current input data
    Log(LOG_DEV_DATA,"Current State: %s, Current Input String: %s\n", GetCurrentState().c_str(), GetInputData().c_str());
}

void PendantManager::ResetData(const INPUT_DATA_TYPE &inputItem)
{
    switch (inputItem)
    {
    case VIN_DATA:
        // Setting the current state to VIN will reset all the neccessary data items
        SetCurrentState(INPUT_SERVER_VIN_STATE);
        break;

    case DRIVER_NUMBER_DATA:
        // Setting the current stat to Driver number will reset all the data items
        SetCurrentState(INPUT_SERVER_DRIVER_NUMBER_STATE);
        break;

    case CHARACTER_DATA:
        // Set the default character to start with
        SetCharacter('@');
        break;

    default:
        Log(LOG_ERRORS, "Error: PendantManager::ResetData() called with %d. Valid options are VIN_DATA(%d), DRIVER_NUMBER_DATA(%d) or CHARACTER_DATA(%d)\n",
            inputItem, VIN_DATA, DRIVER_NUMBER_DATA, CHARACTER_DATA);
    }
}

void PendantManager::SetCurrentState(const std::string &state)
{
    try
    {
        string response;    // used for receiving the reponse from a Write() to the input server
        // Get the state being transitioned out of
        std::string previousState = GetCurrentState();
        // Store the current state
        m_currentState = state;
        bool writeDataNode = false;   // Default to not updating the gui display
        XmlNode dataNode("temp","");
        Log(LOG_DEV_DATA, "PendantManager::SetCurrentState(state:%s)\n", state.c_str());
        // Determine if the mode is valid
        XmlNodeMapItr inputModeIter;
        if ((inputModeIter = m_pendantModes.find(GetCurrentState())) == m_pendantModes.end())
        {   // Invalid mode, default to Normal
            Log(LOG_ERRORS, "Invalid Pendant mode:%s, defaulting to %s\n", state.c_str(), INPUT_SERVER_NORMAL_STATE);
            m_currentState = INPUT_SERVER_NORMAL_STATE;
            inputModeIter = m_pendantModes.find(GetCurrentState());
        }
        if (inputModeIter != m_pendantModes.end())
        {   // Set up the keypad for the specified mode
            SetKeyPad(inputModeIter->second->getChild(validKeyPad));
            // Determine if key presses need to be published
            SetPublishKeypress(inputModeIter->second->getChild(publishKeyPress)->getValue() == "Yes");
            // Set the length of the data string
            SetInputDataLength(inputModeIter->second->getChild(dataLength)->getValue());
            // Set the defaul index to 0
            SetCurrentIndex(0);
            // Set the data tag to use for reporting the input data
            SetInputDataTag(inputModeIter->second->getChild(dataTag)->getValue());
            // Set the pendant for the new input mode
            if ((INPUT_SERVER_VIN_STATE == GetCurrentState()) ||
                (INPUT_SERVER_DRIVER_NUMBER_STATE == GetCurrentState()))
            {   // Set the data map
                SetInputDataMap(inputModeIter->second->getChild(dataMap));
                // Set the default input data value
                m_inputData.erase();
                for (XmlNodeMapItr iter = m_inputDataMap.begin(); iter != m_inputDataMap.end(); iter++)
                    m_inputData += iter->second->getAttribute(defaultValue)->getValue();
                // Set the default index and the value at that index
                SetCurrentIndex(inputModeIter->second->getChild(defaultStartIndex)->getValue());
                std::string fieldIndicator = inputModeIter->second->getChild(currentFieldIndicator)->getValue();
                if (isdigit(fieldIndicator[0])) SetCurrentFieldIndicator(atoi(fieldIndicator.c_str()));
                else SetCurrentFieldIndicator(fieldIndicator[0]);
                m_inputData[GetCurrentIndex()] = GetCurrentFieldIndicator();
                // Set the abort button status for VIN mode only if previous state was not TestResult
                Log(LOG_DEV_DATA, "Determining if Abort button has been pressed -- Current State: %s, Previous State: %s\n",
                    GetCurrentState().c_str(), previousState.c_str());
                if ((INPUT_SERVER_VIN_STATE == GetCurrentState()) &&
                    (INPUT_SERVER_TEST_RESULT_STATE == previousState) &&
                    !IsBuildRecordValid() && RollsAreDown())
                {   // An invalid vin was entered, allow re-entry without first pressing Abort button
                    SetAbortButtonPressed(true);
                }
                else if ((INPUT_SERVER_VIN_STATE == GetCurrentState()) &&
                         (INPUT_SERVER_TEST_RESULT_STATE != previousState))
                {   // We are re-entering the vin after the abort button was pressed
                    SetAbortButtonPressed(false);
                }
                // Set the Abort button pressed status for driver number mode
                if ((INPUT_SERVER_DRIVER_NUMBER_STATE == GetCurrentState()) &&
                    (Read(DRIVER_NUMBER_DATA_TAG) == DEFAULT_DRIVER_NUMBER))
                {   // Set the abort status to true if the default driver number is displayed
                    SetAbortButtonPressed(true);
                }
                else if (INPUT_SERVER_DRIVER_NUMBER_STATE == GetCurrentState())
                {   // Set the abort status to false if default driver number is not display
                    SetAbortButtonPressed(false);
                }
                // Set the display tag
                SetDisplayTag(inputModeIter->second->getChild(displayTag)->getValue());
                // Build the display node
                dataNode.setName(GetDisplayTag());
                dataNode.setValue(GetInputData());
                // Determine if the data can be displayed
                Log(LOG_DEV_DATA, "Determining data display availability: Current State: %s, IsVinUpdateAllowed: %s\n",
                    GetCurrentState().c_str(), IsVinUpdateAllowed() ? "Yes" : "No");
                if (INPUT_SERVER_VIN_STATE == GetCurrentState() && !IsVinUpdateAllowed())
                    writeDataNode = false;          // Do not allow data to be updated on the GUI
                else if ((INPUT_SERVER_DRIVER_NUMBER_STATE == GetCurrentState()) && !IsDriverNumberUpdateAllowed())
                    writeDataNode = false;          // Do not allow the data to be updated on the GUI
                else
                    writeDataNode = true;           // Allow data to be updated on the GUI
            }
            Log(LOG_DEV_DATA,"SetCurrentState() - writeDataNode value = %s\n", writeDataNode ? "True" : "False");
            // Send the node to the InputServer
            if (writeDataNode) m_inputServerComm->Write(&dataNode,response,true);
        }
        else Log(LOG_ERRORS, "Error, Commanded State Not In List: %s (Warning possibly not initialized correctly)\n", state.c_str());
    }
    catch (BepException &e)
    {
        Log(LOG_DEV_DATA, "Error Updating The Current State: %s\n", e.what());
    }
}

void PendantManager::AddInputCharacter(const unsigned char &character)
{
    bool incrementIndex = false;   // Flag to determine if GUI needs updating
    string response;    // used for receiving the reponse from a Write() to the input server

    Log(LOG_DEV_DATA,"AddInputCharacter() Enter - Character: %c\n",character);

    // Need to handle VIN and Driver number modes specially
    if (((GetCurrentState() == INPUT_SERVER_VIN_STATE) && IsVinUpdateAllowed()) ||
        ((GetCurrentState() == INPUT_SERVER_DRIVER_NUMBER_STATE) && IsDriverNumberUpdateAllowed()))
    {
        Log(LOG_DEV_DATA,"AddInputCharacter(), current index: %d\n",GetCurrentIndex());
        // Get the data type for the current index
        std::string dataType = m_inputDataMap.getNode(GetCurrentIndex())->getAttribute(typeField)->getValue();
        Log(LOG_DEV_DATA,"AddInputCharacter(), data type: %s\n",dataType.c_str());
        // Check to make sure this is a valid value to add to the current index
        if ((integerType == dataType) && isdigit(character))
        {
            Log(LOG_DEV_DATA,"AddInputCharacter() - Type is Integer, current m_inputData = %s\n",m_inputData.c_str());
            // Add the number to the input string
            m_inputData[GetCurrentIndex()] = character;
            m_inputData[GetCurrentIndex()+1] = GetCurrentFieldIndicator();
            // Advance the index
            incrementIndex = true;
            Log(LOG_DEV_DATA,"AddInputCharacter() - Added Char %c, now m_inputData = %s\n",character,m_inputData.c_str());
        }
        else if (((characterType == dataType) && (isalpha(character))) ||
                 ((alphanumericType == dataType) && (isalnum(character))))
        {   // Add the character to the input string
            m_inputData[GetCurrentIndex()] = character;
            m_inputData[GetCurrentIndex()+1] = GetCurrentFieldIndicator();
            // If this is really a digit, advance the index
            if (isdigit(character)) incrementIndex = true;
        }
        else if ((m_inputDataMap.getNode(GetCurrentIndex())->getAttribute(defaultValue)->getValue()[0] == character) ||
                 (character == GetCurrentFieldIndicator()) || (character == '_'))
        {   // Allow the default character to be added or the current chacacter to be cleared
            m_inputData[GetCurrentIndex()] = character;
            incrementIndex = false;
        }
        // Display the input data string on the GUI
        XmlNode displayNode(GetDisplayTag(), GetInputData());
        m_inputServerComm->Write(&displayNode,response,true);
        Log(LOG_DEV_DATA,"AddInputCharacter() - Wrote InputData to InputServer\n");
        // Increment the index if needed
        if (incrementIndex) IncrementInputDataIndex();
    }
    else if ((GetCurrentState() != INPUT_SERVER_VIN_STATE) &&
             (GetCurrentState() != INPUT_SERVER_DRIVER_NUMBER_STATE))
    {   // Just add the data to the input string
        m_inputData[GetCurrentIndex()] = character;
    }
    Log(LOG_DEV_DATA,"AddInputCharacter() - Exit\n");
}

void PendantManager::IncrementInputDataIndex(const INT16 &increment) /*= 1*/
{
    string response;    // used for receiving the reponse from a Write() to the input server
    // Reset the character data
    ResetData(CHARACTER_DATA);
    // If no value has been added to this spot, insert default value
    if ((unsigned char)m_inputData[GetCurrentIndex()] == GetCurrentFieldIndicator())
    {   // Get the default value for this index
        std::string defaultFieldValue = m_inputDataMap.getNode(GetCurrentIndex())->getAttribute(defaultValue)->getValue();
        m_inputData[GetCurrentIndex()] = defaultFieldValue[0];
    }
    // Handle vin mode specially
    if ((GetCurrentState() == INPUT_SERVER_VIN_STATE) && IsVinUpdateAllowed())
    {   // Incrment the index
        m_currentIndex += increment;
    }
    else if (GetCurrentState() != INPUT_SERVER_VIN_STATE)
    {   // No restrictions on index increment
        m_currentIndex += increment;
    }
    // Check if the complete string has been entered
    Log(LOG_DEV_DATA,"Checking for complete data: Current Index: %d, Data Length: %d\n",
        GetCurrentIndex(), GetInputDataLength());
    if (GetCurrentIndex() == GetInputDataLength())
    {
        // Write the data tag to the input server
        XmlNode dataNode(GetInputDataTag(), GetInputData());
        m_inputServerComm->Write(&dataNode,response,true);

        // Switch to test result mode
        if ((GetCurrentState() == INPUT_SERVER_VIN_STATE) ||
            (GetCurrentState() == INPUT_SERVER_DRIVER_NUMBER_STATE))
        {
            SetCurrentState(INPUT_SERVER_TEST_RESULT_STATE);
            // Clear the abort button status
            SetAbortButtonPressed(false);
        }
    }
}

void PendantManager::DecrementInputDataIndex(const INT16 &decrement) /*= 1*/
{
    // Only perform actions if vin updates are allowed or the curent state is driver number
    if (((INPUT_SERVER_VIN_STATE == GetCurrentState()) && IsVinUpdateAllowed()) ||
        (INPUT_SERVER_DRIVER_NUMBER_STATE == GetCurrentState()))
    {   // Clear the value to the default field before moving the index
        AddInputCharacter('_');
        // Decrement the index
        m_currentIndex -= decrement;
        // Keep the index inside the array
        if (m_currentIndex < 0)
        {   // Do not wrap around to the end of the array
            SetCurrentIndex(0);
        }
        // Clear the current character to the default value also
        AddInputCharacter(GetCurrentFieldIndicator());
    }
}

const bool PendantManager::IsVinUpdateAllowed(void)
{
    Log(LOG_DEV_DATA,"PendantManager::IsVinUpdateAllowed() --\n");
    Log(LOG_DEV_DATA,"\tIsBuildRecordValid() - %s\n", IsBuildRecordValid() ? "TRUE" : "FALSE");
    Log(LOG_DEV_DATA,"\tHasAbortButtonBeenPressed() - %s\n", HasAbortButtonBeenPressed() ? "TRUE" : "FALSE");
    Log(LOG_DEV_DATA,"PendantManager::IsVinUpdateAllowed() returning - %s\n", (!IsBuildRecordValid() ||
                                                                               (IsBuildRecordValid() && HasAbortButtonBeenPressed())) ? "True" : "False");
    return(!IsBuildRecordValid() ||
           (IsBuildRecordValid() && HasAbortButtonBeenPressed()));
}

const bool PendantManager::IsDriverNumberUpdateAllowed(void)
{
    Log(LOG_DEV_DATA,"PendantManager::IsDriverNumberUpdateAllowed() --\n");
    Log(LOG_DEV_DATA,"\tCurrent Driver Number - %s\n", Read(DRIVER_NUMBER_DATA_TAG).c_str());
    Log(LOG_DEV_DATA,"\tHasAbortButtonBeenPressed() - %s\n", HasAbortButtonBeenPressed() ? "TRUE" : "FALSE");
    return((Read(DRIVER_NUMBER_DATA_TAG) == DEFAULT_DRIVER_NUMBER) ||
           ((Read(DRIVER_NUMBER_DATA_TAG) != DEFAULT_DRIVER_NUMBER) && HasAbortButtonBeenPressed()));
}

const std::string PendantManager::GetKeyPressValue(const char &keyPress)
{   // Find the key in the map
    std::string keyValue(invalidKeyPress);
    std::string keyPressTag("K");
    // Build the tag.  The DCX pendant uses F1 = [ and F2 = ], which can not be used as tags
    if (keyPress == (char)F1)      keyPressTag += "F1";
    else if (keyPress == (char)F2) keyPressTag += "F2";
    else
    {   // If the character is not alphanumeric, then append the hex code
        if(!isalnum(keyPress))
        {
            char buff[8];
            keyPressTag += CreateMessage(buff, sizeof(buff), "%02X", keyPress);
        }
        else
        {
            keyPressTag += keyPress;
        }
    }
    XmlNodeMapItr iter;
    if ((iter = m_keyPad.find(keyPressTag)) != m_keyPad.end())
    {   // Get the value the key maps to
        keyValue = iter->second->getValue();
    }
    return(keyValue);
}

inline const std::string& PendantManager::GetCurrentState(void)
{
    return m_currentState;
}

inline const std::string& PendantManager::GetDisplayTag(void)
{
    return m_displayTag;
}

inline const std::string& PendantManager::GetInputDataTag(void)
{
    return m_inputDataTag;
}

inline const bool& PendantManager::PublishKeypress(void)
{
    return m_publishKeyPress;
}

inline const INT32& PendantManager::GetInputDataLength(void)
{
    return m_inputDataLength;
}

inline std::string& PendantManager::GetInputData(void)
{
    return m_inputData;
}

inline const INT32& PendantManager::GetCurrentIndex(void)
{
    return m_currentIndex;
}

inline const unsigned char& PendantManager::GetCurrentFieldIndicator(void)
{
    return m_currentFieldIndicator;
}

bool PendantManager::RollsAreDown(void)
{
    return(Read(ROLLS_DOWN_DATA_TAG) == "1");
}

bool PendantManager::IsBuildRecordValid()
{
    return(Read(BUILD_RECORD_STATUS_TAG) == BUILD_RECORD_VALID);
}

inline void PendantManager::SetKeyPad(const XmlNode *keyPad)
{
    if (keyPad != NULL) m_keyPad.DeepCopy(keyPad->getChildren());
    else m_keyPad.clear(true);
}

inline void PendantManager::SetDisplayTag(const std::string &tag)
{
    m_displayTag = tag;
}

inline void PendantManager::SetInputDataTag(const std::string &tag)
{
    m_inputDataTag = tag;
}

inline void PendantManager::SetCharacter(const char &character /*= 'A'*/)
{
    m_currentCharacter = character;
}

inline char& PendantManager::IncrementCharacter(void)
{   // Increment the current character
    m_currentCharacter++;
    // Make sure it is still a valid character
    if (m_currentCharacter > 'Z') SetCharacter();
    return m_currentCharacter;
}

inline char& PendantManager::DecrementCharacter(void)
{   // Decrement the current character
    m_currentCharacter--;
    // Make sure this is still a valid character
    if (m_currentCharacter < 'A') SetCharacter('Z');
    return m_currentCharacter;
}

inline void PendantManager::SetAbortButtonPressed(const bool &abortPressed)
{
    m_abortButtonPressed = abortPressed;
}

inline const bool& PendantManager::HasAbortButtonBeenPressed(void)
{
    return m_abortButtonPressed;
}

inline void PendantManager::SetModeConfigurations(const XmlNode *inputModes)
{
    m_pendantModes.DeepCopy(inputModes->getChildren());
}

inline void PendantManager::SetPublishKeypress(const bool &publishKeyPress)
{
    m_publishKeyPress = publishKeyPress;
}

inline void PendantManager::SetInputDataLength(const INT32 &length)
{
    m_inputDataLength = length;
}

inline void PendantManager::SetInputDataLength(const std::string &length)
{
    SetInputDataLength(atoi(length.c_str()));
}

inline void PendantManager::SetInputDataMap(const XmlNode *dataMap)
{
    m_inputDataMap.DeepCopy(dataMap->getChildren());
}

inline void PendantManager::SetCurrentIndex(const INT32 &index)
{
    m_currentIndex = index;
}

inline void PendantManager::SetCurrentIndex(const std::string &index)
{
    SetCurrentIndex(BposReadInt(index.c_str()));
}

inline void PendantManager::SetCurrentFieldIndicator(const unsigned char &indicator)
{
    m_currentFieldIndicator = indicator;
}
