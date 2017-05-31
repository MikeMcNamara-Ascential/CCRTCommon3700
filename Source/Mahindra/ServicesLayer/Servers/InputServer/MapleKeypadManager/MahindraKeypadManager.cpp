//*************************************************************************
// Description:
//	The MahindraKeypadManager handles all keypad entries and publishes the data to
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/MahindraKeypadManager/MahindraKeypadManager.cpp $
// 
// 2     6/07/06 5:36p Cward
// Changes to allow the Input Server, CLV295ScannerMgr,
// MapleKeypadManager, PendantManager, and SymbolScannerMgr configurations
// to be reloaed without restrting the processes.
//
// 1     6/05/06 3:46p Rwiersem
// Added a flag to tell if we should open the logical port.  This is used
// to run the unit tests without the full system.  Added
// LoadAdditionalConfiguration() items which is used to reload the
// configuration without restarting the process.
//
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
// 2     4/29/05 8:00a Gpattison
// Refactored the entire MapleKeypad Manager so that the InputServer could
// spawn it as an individual input device instead of having it as an object.
// This allows it to be added or removed from the system by making
// only configuration file changes.  It is now a stand alone BepServer type
// executable.
//
// 2     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     3/02/05 1:25p Gswope
// significant rewrite during debugging
// in EvaluateData() changed to work with one character 0x0d as terminator,
//     rather than two character 0x0d,0x0a
// in same, changed debug to only print characters received, was previously printing length+1
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
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:51  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:12a Khykin
// Initial Checkin
//
// 1     1/18/03 8:08a Khykin
// Initial Checkin From DCTC
//
// 1     11/26/02 4:54p Derickso
// New Input Device manager.
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================

#include "INamedDataBroker.h"
#include "MahindraKeypadManager.h"

MahindraKeypadManager::MahindraKeypadManager() : MapleKeypadManager()
{   // Nothing To Do Here
}

MahindraKeypadManager::~MahindraKeypadManager()
{
}

void MahindraKeypadManager::Initialize(const XmlNode *configNode)
{
    // Call the base class
    InputDeviceBase::Initialize(configNode);

    // Load the additional configuration items needed by the MapleKeypadManager
    LoadAdditionalConfigurationItems(configNode);

    // Set the default mode
    SetCurrentState(INPUT_SERVER_NORMAL_STATE);
}

void MahindraKeypadManager::EvaluateData(unsigned char *data, const INT32 &byteCount)
{
    std::string dataType(BEP_UNAVAILABLE_RESPONSE);       // Type of data we received -- VIN, Body Style, Etc
    std::string displayType(BEP_UNAVAILABLE_RESPONSE);    // Type of data to Display
    std::string dataValue(BEP_UNAVAILABLE_RESPONSE);      // The data we received
    std::string secondaryDataValue(BEP_UNAVAILABLE_RESPONSE);      // The data we received
    string response;            // used for receiving the reponse from a Write() to the input server
    bool processData = false;   // Process the data received

    std::string dataString = "";

    Log(LOG_FN_ENTRY, "Enter MahindraKeypadManager::EvaluateData()\n");
    for (INT32 ii = 0; ii < byteCount; ii++)
    {
        char localData[10];
        sprintf(localData,"%c(%02x) ",
                isprint(data[ii]) ? data[ii] : '?', data[ii]);
        dataString += localData;
    }

    // Determine the type of data

    Log(LOG_DEV_DATA,"Received %d bytes from Maple Keypad: %s", byteCount ,dataString.c_str());
    Log(LOG_ERRORS, "%s\n",dataString.c_str());

    if (MapleKeypadManager::GetCurrentMode() == INPUT_SERVER_VIN_STATE)
    {   // Publish the VIN data
        dataType = NEXT_VIN_DATA_TAG;
        displayType = VINDISPLAY_DATA_TAG;
        processData = true;
    }
    else
    {   // Unknown data type
        Log(LOG_ERRORS, "Unknown mode (%s), not processing data:\n", MapleKeypadManager::GetCurrentMode().c_str());
        Log(LOG_ERRORS, "%s\n",dataString.c_str());
        processData = false;
    }
    // Determine if the data needs to be processed
    if (processData)
    {  // Look for a carrige return to determine if the data is valid
        bool foundEndSequence = false;
        INT32 index = 0;
        // let the next loop go past the end of the string, in case no CR in data
        while (!foundEndSequence && (index <= byteCount))
        {  // Check if the end sequence of carriage return was found
            foundEndSequence = (data[index] == CR) ;
            // && (data[index+1] == LF);
            // Increment the index if the Carrigae return character was not found
            if (!foundEndSequence) index++;
        }

        // Determine if valid data was received
// This area could use more cleanup,
// I.E. remove leading/trailing spaces, nulls. Consider validating length,
// adding prefix if they enter only the last 6 etc, etc.

        // at this point, _length_ of data field == index
        if (foundEndSequence)
        {   // Determine the type of data
            if (MapleKeypadManager::GetCurrentMode() == INPUT_SERVER_VIN_STATE)
            {   // Determine if we have a VIN or secondary data
                if(index >= VinLength())
                {
                    Log(LOG_DEV_DATA, "Received VIN data -- Byte count: %d", index);
                    dataType = NEXT_VIN_DATA_TAG;
                    displayType = VINDISPLAY_DATA_TAG;
                    processData = true;
                }
                else if(index >= SecondaryDataLength())
                {
                    Log(LOG_DEV_DATA, "Received %s data -- Byte count: %d", SecondaryDataType().c_str(), index);
                    dataType = SecondaryDataType();
                    processData = true;
                } 
                else
                {
                    Log(LOG_ERRORS, "Received unknown data -- Byte count: %d Secondary Data Length: %d", index, SecondaryDataLength());
                    processData = false;
                }
            }
            else
            {   // Unknown data type
                Log(LOG_ERRORS, "Unknown mode (%s), not processing data: %s", MapleKeypadManager::GetCurrentMode().c_str() ,dataString.c_str());
                processData = false;
            }
            // Determine if the data needs to be processed
            if (processData)
            {   // Extract the data from the buffer
                // ews - 1/28/2005 changed to use index, rather than byteCount-2
                dataValue = std::string((char *)&data[0], index);
                if(index > VinLength())
                    secondaryDataValue = std::string((char *)&data[VinLength()], index);
                Log(LOG_DEV_DATA, "Maple Keypad: %s %s\n", displayType.c_str(), dataValue.c_str());
                // If this is secondary data, setup for Vin entry
                if(!dataType.compare(SecondaryDataType()))
                {   // reset the display on the keypad
                    ClearKeypadDisplay();
                    DisplayMessage("EnterVIN");
                    PositionCursor(GetDataTag("VINEntryRow"), GetDataTag("VINEntryColumn"));
                    EnableBlockMode();
					if(WriteSecondaryDataToNdb())
					{
						INamedDataBroker broker;
						broker.Write(dataType, dataValue, response, true);
					}
                }
                // If this data is a VIN, display it
                if(!dataType.compare(NEXT_VIN_DATA_TAG))
                {
                    XmlElement displayNode(displayType, dataValue);
                    m_inputServerComm->Write(&displayNode,response,true);
                    if(WriteSecondaryDataToNdb() && index > VinLength())
                    {
                        INamedDataBroker broker;
                        broker.Write(SecondaryDataType(), secondaryDataValue, response, true);
                    }
                }
                // Make a couple of nodes to send data around
                XmlElement displayNode(displayType, dataValue);
                Log(LOG_DEV_DATA, "Maple Keypad: %s %s\n", dataType.c_str(), dataValue.c_str());
                XmlElement dataNode(dataType, dataValue);
                // Write the data to the InputServer
                m_inputServerComm->Write(&displayNode,response,true);
                m_inputServerComm->Write(&dataNode,response,true);
            }
            else
            {   // No end sequence found, bad data
                Log(LOG_ERRORS, "No end sequence found in buffer (CR LF), ignoring data:\n");
                Log(LOG_ERRORS, "%s\n",dataString.c_str());
            }
        }
    }
}

void MahindraKeypadManager::SetCurrentState(const std::string &state)
{   // Decide which messages to display on the keypad
    Log(LOG_FN_ENTRY, "MahindraKeypadManager::SetCurrentState(%s)\n", state.c_str());
    // Get the VIN
    if (state == INPUT_SERVER_VIN_STATE)
    {   // Clear the display
        ClearKeypadDisplay();
        // Display the Enter VIN message
        DisplayMessage("EnterModelCode");
        // Position the cursor for data entry
        PositionCursor(GetDataTag("VINEntryRow"), GetDataTag("VINEntryColumn"));
        // Enable block mode on the keypad
        EnableBlockMode();
    }
    // Ready to test
    else if (state == INPUT_SERVER_TEST_RESULT_STATE)
    {   // Clear the display
        ClearKeypadDisplay();
        // Disable block mode on the keypad
        DisableBlockMode();
        // Display Ready to Test message
        DisplayMessage("ReadyToTest");
        // Display the current VIN on the keypad
        DisplayMessage(Read(NEXT_VIN_DATA_TAG), GetDataTag("VINDisplayLine"), GetDataTag("VINDisplayColumn"));
    }
    // Not ready for inputs
    else
    {   // Clear the display
        ClearKeypadDisplay();
        // Disable block mode on the keypad
        DisableBlockMode();
        // Display the not ready to test message
        DisplayMessage("NotReady");
        DisplayMessage("PleaseWait");
    }
    // Store the current keypad mode
    SetCurrentMode(state);
}
void MahindraKeypadManager::Run(volatile bool *terminateFlag) /* =NULL */
{
    unsigned char inputData[GetBufferSize()];
    INT32 byteCount = 0;
    Log(LOG_FN_ENTRY, "MahindraKeypadManager::Execute() begin\n");
    // clear the keypad buffer
    memset(inputData, 0, GetBufferSize());
    // Loop until the server is terminated
    while (GetStatus() != BEP_TERMINATE)
    {   // Get the data from the buffer
        Log(LOG_DEV_DATA,"Waiting for Mahindra Keypad data\n");
        byteCount = m_keypadComm.Get(inputData, GetBufferSize());
        // Check if the current input server state needs to be updated
        if (m_stateChanged)
        {

            SetCurrentState(m_nextState);
            m_stateChanged = false;
        }
        Log(LOG_DEV_DATA,"Received %d characters from Mahindra Keypad\n", byteCount);
        // Check for data
        if (byteCount > 0)
        {   // Get the remaining data from the buffer
            m_keypadComm.GetRemainingSerialData(inputData, GetBufferSize(), byteCount, GetMaximumRetries(), GetDataDelay());
            // Evaluate the data received
            EvaluateData(inputData, byteCount);
            // Clear the scanner buffer
            memset(inputData, 0, GetBufferSize());
        }
    }
}
