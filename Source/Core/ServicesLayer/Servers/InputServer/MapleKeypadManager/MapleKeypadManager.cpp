//*************************************************************************
// Description:
//	The MapleKeypadManager handles all keypad entries and publishes the data to
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/MapleKeypadManager/MapleKeypadManager.cpp $
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
#include "MapleKeypadManager.h"

MapleKeypadManager::MapleKeypadManager() : InputDeviceBase(), m_nextState(INPUT_SERVER_NORMAL_STATE),
m_stateChanged(false), m_openLogicalPort(true)
{   // Nothing To Do Here
}

MapleKeypadManager::~MapleKeypadManager()
{
}

void MapleKeypadManager::Initialize(const XmlNode *configNode)
{
    // Call the base class
    InputDeviceBase::Initialize(configNode);

    // Load the additional configuration items needed by the MapleKeypadManager
    LoadAdditionalConfigurationItems(configNode);

    // Set the default mode
    SetCurrentState(INPUT_SERVER_NORMAL_STATE);
}

void
MapleKeypadManager::LoadAdditionalConfigurationItems(const XmlNode *configNode)
{
    const XmlNode *document = configNode->getChild("Setup");

    // if we should open the logical ports
    if(m_openLogicalPort == true)
    {
        // Setup the name of the Keypad 
        SetName(document->getChild("MapleKeypadCommunication"));
        SetReconnectDelay(250);
        // Wait for Maple Keypad's serial port to be set with the Serial Server
        WaitForKeypad();
        // Setup the Maple Keypad Communication object
        m_keypadComm.Initialize(document->getChild("MapleKeypadCommunication"));
    }

    // Set the keypad buffer size
    try
    {
        const XmlNode *keypadCommSetup = document->getChild("MapleKeypadCommunication")->getChild("Setup");
        SetBufferSize(keypadCommSetup->getChild("Communication")->getChild("BufferSize")->getValue());
    }
    catch (...)
    {
        SetBufferSize(128);
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
	// Store the VIN length
	INT32 vinLength = 17;
	try
	{
		vinLength = BposReadInt(document->getChild("VinLength")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "VIN Length not defined, using 17 - %s", excpt.GetReason());
		vinLength = 17;
	}
	VinLength(&vinLength);
    // Load any secondary data item
    bool useSecondaryDataItem = false;
    string dataType("");
    INT32 dataLength = 999;
    try
    {
        dataType = document->getChild("SecondaryDataTag")->getValue();
        useSecondaryDataItem = true;
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Secondary data item type not defined, not configuring secondary data: %s", excpt.GetReason());
        useSecondaryDataItem = false;
    }
    if(useSecondaryDataItem)
    {
        try
        {
            dataLength = BposReadInt(document->getChild("SecondaryDataLength")->getValue().c_str());
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "Secondary data item length not defined, not configuring secondary data: %s", excpt.GetReason());
            useSecondaryDataItem = false;
        }
		bool writeToNdb = false;
		try
		{
			writeToNdb = atob(document->getChild("WriteSecondaryDataToNdb")->getValue().c_str());
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Not writing secondary data items to the NDB - %s", excpt.GetReason());
			writeToNdb = false;
		}
		WriteSecondaryDataToNdb(&writeToNdb);
    }
    // Store the secondary data items
    SecondaryDataSupported(&useSecondaryDataItem);
    SecondaryDataType(&dataType);
    SecondaryDataLength(&dataLength);
}

const std::string MapleKeypadManager::Publish(const XmlNode *node)
{
    if (node->getName() == INPUT_SERVER_STATE)
    {
        // Set the pendant mode if this is a state change
        Log(LOG_DEV_DATA, "Setting Pendant Manager to state %s\n", node->getValue().c_str());
        // Set the pendant mode if this  is a state change
        Log(LOG_DEV_DATA, "Setting Pendant Manager to state %s\n", node->getValue().c_str());
        // Store the value of the future Input Server state to a member variable
        //  and set a flag indicating that the input server state needs to be updated.
        //  This allows the thread calling the Publish method to return immediately
        //  without having to do all the processing involved in changing the
        //  input server state.
        m_nextState = node->getValue();
        m_stateChanged = true;
    }
    // Call the base class method to do it's thing
    return(BepServer::Publish(node));
}

void MapleKeypadManager::Run(volatile bool *terminateFlag) /* =NULL */
{
    unsigned char inputData[GetBufferSize()];
    INT32 byteCount = 0;
    Log(LOG_FN_ENTRY, "MapleKeypadManager::Execute() begin\n");
    // clear the keypad buffer
    memset(inputData, 0, GetBufferSize());
    // Loop until the server is terminated
    while (GetStatus() != BEP_TERMINATE)
    {   // Get the data from the buffer
        Log(LOG_DEV_DATA,"Waiting for Maple Keypad data\n");
        byteCount = m_keypadComm.Get(inputData, GetBufferSize());
        // Check if the current input server state needs to be updated
        if (m_stateChanged)
        {
            SetCurrentState(m_nextState);
            m_stateChanged = false;
        }
        Log(LOG_DEV_DATA,"Received %d characters from Maple Keypad\n", byteCount);
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

void MapleKeypadManager::SetCurrentState(const std::string &state)
{   // Decide which messages to display on the keypad
    Log(LOG_FN_ENTRY, "MapleKeypadManager::SetCurrentState(%s)\n", state.c_str());
    // Get the VIN
    if (state == INPUT_SERVER_VIN_STATE)
    {   // Clear the display
        ClearKeypadDisplay();
        // Display the Enter VIN message
        DisplayMessage("EnterVIN");
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

void MapleKeypadManager::EnableBlockMode(void)
{
    Log(LOG_FN_ENTRY, "MapleKeypadManager::EnableBlockMode() \n");
    std::string command;
    unsigned char escape = ESC;
    // Enable the keypad
    command = std::string("}") + std::string("2") + TERMINATOR;
    m_keypadComm.Send(&escape, 1);
    m_keypadComm.Send(command);
    // Build the command to enter block mode
    command = "{" + GetDataTag("BlockModeFieldStart") + GetDataTag("BlockModeFieldEnd") + GetDataTag("BlockModeFillCharacter") + TERMINATOR;
    if (IsDebugOn()) PrintSendBuffer(command);
    // Send the command to the Maple keypad
    m_keypadComm.Send(&escape, 1);
    m_keypadComm.Send(command);
}

void MapleKeypadManager::DisableBlockMode(void)
{
    std::string command;
    // Build the command to exit block mode
    command = "}" + GetDataTag("DisableKeyboard") + TERMINATOR;
    if (IsDebugOn()) PrintSendBuffer(command);
    // Send the command to the Maple Keypad
    unsigned char escape = ESC;
    m_keypadComm.Send(&escape, 1);
    m_keypadComm.Send(command);
}

void MapleKeypadManager::ClearKeypadDisplay(void)
{   // Send the command to the Maple Keypad
    Log(LOG_FN_ENTRY, "MapleKeypadManager::ClearKeypadDisplay()\n");
    unsigned char cls = CLEAR_SCREEN;
    m_keypadComm.Send(&cls, 1);
}

void MapleKeypadManager::PositionCursor(const std::string row, const std::string column)
{   // Move the cursor to the desired location
    Log(LOG_FN_ENTRY, "MapleKeypadManager::PositionCursor(%s, %s)\n", row.c_str(), column.c_str());
    std::string command = "x" + column + row + TERMINATOR;
    if (IsDebugOn()) PrintSendBuffer(command);
    // Send the command to move the cursor
    unsigned char escape = ESC;
    m_keypadComm.Send(&escape, 1);
    m_keypadComm.Send(command);
}

void MapleKeypadManager::PositionCursor(const INT16 row, const INT16 column)
{
    char displayRow[3], displayColumn[3];
    // Conver the row and column to string
    sprintf(displayRow, "%02d", row);
    sprintf(displayColumn, "%02d", column);
    // Now position the cursor
    PositionCursor(displayRow, displayColumn);
}

void MapleKeypadManager::DisplayMessage(std::string message)
{
    std::string row, column, text;
    XmlNodeMapItr messageNode;
    Log(LOG_FN_ENTRY, "MapleKeypadManager::DisplayMessage(%s)\n", message.c_str());
    row.erase();
    column.erase();
    text.erase();
    // Get the message node from the data tags
    if ((messageNode = m_dataTags.find(message)) != m_dataTags.end())
    {   // Get the row and column data
        try
        {   // Get the row data
            row = messageNode->second->getAttribute("RowIndex")->getValue();
        }
        catch (BepException &err)
        {   // Row index may not have been specified
            row = "1";
            Log(LOG_ERRORS, "MapleKeypadManager::DisplayMessage() No RowIndex attribute specified, using %s\n", row.c_str());
        }
        try
        {   // Get the column index
            column = messageNode->second->getAttribute("ColumnIndex")->getValue();
        }
        catch (BepException &err)
        {   // Column index may not have been specified
            column = "1";
            Log(LOG_ERRORS, "MapleKeypadManager::DisplayMessage() No ColumnIndex attribute specified, using %s\n", column.c_str());
        }
        // Get the text to be displayed
        text = messageNode->second->getValue();
        // Display the message on the keypad
        DisplayMessage(text, row, column);
    }
    else
    {   // Message not specified in the dat tags
        Log(LOG_ERRORS, "MapleKeypadManager::DisplayMessage() Could not find message node with tag %s\n", message.c_str());
    }
}

void MapleKeypadManager::DisplayMessage(std::string text, const std::string row, const std::string column)
{   // Move the cursor to the desired location
    Log(LOG_DEV_DATA, "MapleKeypadManager::DisplayMessage(%s, %s, %s)\n", text.c_str(), row.c_str(), column.c_str());
    PositionCursor(row, column);
    // Now display the text
    if (IsDebugOn()) PrintSendBuffer(text);
    m_keypadComm.Send(text);
}

// modified this routine to only look for CR (backward compatible, CR/LF will also work)
// changed a number of printf's to Log
//
void MapleKeypadManager::EvaluateData(unsigned char *data, const INT32 &byteCount)
{
    std::string dataType(BEP_UNAVAILABLE_RESPONSE);       // Type of data we received -- VIN, Body Style, Etc
    std::string displayType(BEP_UNAVAILABLE_RESPONSE);    // Type of data to Display
    std::string dataValue(BEP_UNAVAILABLE_RESPONSE);      // The data we received
    string response;            // used for receiving the reponse from a Write() to the input server
    bool processData = false;   // Process the data received

    std::string dataString = "";
    Log(LOG_FN_ENTRY, "Enter MapleKeypadManager::EvaluateData()\n");
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

    if (GetCurrentMode() == INPUT_SERVER_VIN_STATE)
    {   // Publish the VIN data
        dataType = NEXT_VIN_DATA_TAG;
        displayType = VINDISPLAY_DATA_TAG;
        processData = true;
    }
    else
    {   // Unknown data type
        Log(LOG_ERRORS, "Unknown mode (%s), not processing data:\n", GetCurrentMode().c_str());
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
            if (GetCurrentMode() == INPUT_SERVER_VIN_STATE)
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
                Log(LOG_ERRORS, "Unknown mode (%s), not processing data: %s", GetCurrentMode().c_str() ,dataString.c_str());
                processData = false;
            }
            // Determine if the data needs to be processed
            if (processData)
            {   // Extract the data from the buffer
                // ews - 1/28/2005 changed to use index, rather than byteCount-2
                dataValue = std::string((char *)&data[0], index);
                Log(LOG_DEV_DATA, "Maple Keypad: %s %s\n", displayType.c_str(), dataValue.c_str());
                // If this is secondary data, setup for VIn entry
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

void MapleKeypadManager::WaitForKeypad(void)
{
    TaskMonTask_t keypadTask;
    do
    {   // Get the state of the keypad in the task monitor
        if (ITaskMon::ReadTaskInfo(keypadTask, m_name.c_str(), TASKMON_CLASS_SERIAL) != -1)
        {
               // If the state is not run, register the port driver
            if(keypadTask.m_taskState != TASKSTATE_RUN)
            {   // Delay before attempting to reconnect/register since the other PC may not be completely up.
                BposSleep(m_reconnectDelay);
                // Register the port driver
                m_keypadComm.RegisterPortDriver();
            }
            else
            {
                Log(LOG_FN_ENTRY, "Info: Keypad Task already in run state\n");
            }
            // Delay after registering to allow the port driver to finish registering.
            BposSleep(m_reconnectDelay);
        }
        else
        {
            Log(LOG_ERRORS, "Unable to read keypad task info - Install latest TaskMon / TaskMonTerm to /usr/local/bin dir\n");
        }
        // Keep looking until state is run
    } while(keypadTask.m_taskState != TASKSTATE_RUN);
}

void MapleKeypadManager::SetName(const XmlNode *configNode)
{
    // Setting the name based on configuration file
    m_name = configNode->getChild("Setup")->getChild("Logging")->getChild("ProcessName")->getValue();
    Log(LOG_FN_ENTRY, "Name of keypad was set to: %s", m_name.c_str());
}

inline void MapleKeypadManager::PrintSendBuffer(const std::string &buffer)
{   // Print each character individually
    Log(LOG_DEV_DATA,"MapleKeypadManager::PrintSendBuffer() -- buffer length: %d\n", buffer.length());
    for (UINT16 ii = 0; ii < buffer.length(); ii++)
        Log(LOG_DEV_DATA,"Byte: %03d -- %c <%02X>\n", ii, (isprint(buffer[ii]) ? buffer[ii] : '?'), buffer[ii]);
}

INT32 &MapleKeypadManager::GetBufferSize(void)
{
    return m_bufferSize;
}

inline std::string &MapleKeypadManager::GetCurrentMode(void)
{
    return m_currentMode;
}

const INT32 &MapleKeypadManager::GetMaximumRetries(void)
{
    return m_maxRetries;
}

const INT32 &MapleKeypadManager::GetDataDelay(void)
{
    return m_dataDelay;
}

inline void MapleKeypadManager::SetReconnectDelay(int delay_ms)
{
    m_reconnectDelay = delay_ms;
}

inline void MapleKeypadManager::SetCurrentMode(const std::string mode)
{
    m_currentMode = mode;
}

inline void MapleKeypadManager::SetBufferSize(const INT32 bufferSize)
{
    m_bufferSize = bufferSize;
}

inline void MapleKeypadManager::SetBufferSize(const std::string &bufferSize)
{
    SetBufferSize(atoi(bufferSize.c_str()));
}

inline void MapleKeypadManager::SetDataDelay(const INT32 delay)
{
    m_dataDelay = delay;
}

inline void MapleKeypadManager::SetDataDelay(const std::string &delay)
{
    SetDataDelay(atoi(delay.c_str()));
}

inline void MapleKeypadManager::SetMaximumRetries(const INT32 maxRetries)
{
    m_maxRetries = maxRetries;
}

inline void MapleKeypadManager::SetMaximumRetries(const std::string &maxRetries)
{
    SetMaximumRetries(atoi(maxRetries.c_str()));
}

//-----------------------------------------------------------------------------
inline const bool& MapleKeypadManager::SecondaryDataSupported(const bool *secdataSupported /*= NULL*/)
{
    if(secdataSupported != NULL)  m_secondaryDataSupported = *secdataSupported;
    return m_secondaryDataSupported;
}

//-----------------------------------------------------------------------------
inline const string& MapleKeypadManager::SecondaryDataType(const string *secDataType /*= NULL*/)
{
    if(secDataType != NULL)  m_secondaryDataType = *secDataType;
    return m_secondaryDataType;
}

//-----------------------------------------------------------------------------
inline const INT32& MapleKeypadManager::SecondaryDataLength(const INT32 *dataLength /*= NULL*/)
{
    if(dataLength != NULL)  m_secondaryDataLength = *dataLength;
    return m_secondaryDataLength;
}

//-----------------------------------------------------------------------------
inline const INT32& MapleKeypadManager::VinLength(const INT32 *length /*= NULL*/)
{
    if(length != NULL)  m_vinLength = *length;
    return m_vinLength;
}

//-----------------------------------------------------------------------------
const bool& MapleKeypadManager::WriteSecondaryDataToNdb(const bool *writeToNdb)
{
	if(writeToNdb != NULL)  m_writeSecondaryDataToNdb = *writeToNdb;
	return m_writeSecondaryDataToNdb;
}
