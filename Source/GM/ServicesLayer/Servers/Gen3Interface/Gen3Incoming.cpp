//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  The DSA Server will interface to the Multi-Function Tester (MFT) using the DSA Protocol.
//  Messages will be received from the MFT and processed.  A response will be generated to
//  transmit back to the MFT.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
// 
//*************************************************************************
#include "Gen3Incoming.h"
#include "INamedDataBroker.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Gen3Incoming::Gen3Incoming(Gen3Interface *server)
{   // Store the pointer to the instantiating object
    m_server = server;
}

//-----------------------------------------------------------------------------
Gen3Incoming::~Gen3Incoming()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
void Gen3Incoming::Initialize(const XmlNode *config)
{
    UINT8 data = 0x00;
    // Get the data length offset
    try
    {
        data = BposReadInt(config->getChild("DataLengthOffset")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Data Length Offset not defined, using 0x55: %s", excpt.GetReason());
        data = 0x55;
    }
    DataLengthOffset(&data);
    // Get the expected message ID
    try
    {
        data = BposReadInt(config->getChild("Incoming/ExpectedMessageId")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Expected message ID not defined, using 0x07: %s", excpt.GetReason());
        data = 0x07;
    }
    ExpectedMessageId(&data);
    // Get the byte location that contains the message ID
    try
    {
        data = BposReadInt(config->getChild("MessageIdByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Message ID byte not defined, using 0: %s", excpt.GetReason());
        data = 0;
    }
    MessageIdByte(&data);
    // Get the mode 1 data byte location
    try
    {
        data = BposReadInt(config->getChild("Mode1DataByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Mode 1 data byte location not specified, using 4: %s", excpt.GetReason());
        data = 4;
    }
    Mode1DataByte(&data);
    // Get the byte location of the data byte count
    try
    {
        data = BposReadInt(config->getChild("DataLengthByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Data length byte not defined, using 1: %s", excpt.GetReason());
        data = 1;
    }
    MessageLengthByte(&data);
    // Get the mode byte
    try
    {
        data = BposReadInt(config->getChild("ModeByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Mode byte not defined, using 2: %s", excpt.GetReason());
        data = 2;
    }
    ModeByte(&data);
    // Store the data byte locations for the supported modes.
    StoreMode1DataByteLocations(config);
    StoreMode2DataByteLocations(config->getChild("Messages/Mode2/Incoming"));
    StoreMode4DataByteLocations(config->getChild("Messages/Mode4/Incoming"));
    StoreMode6DataByteLocations(config->getChild("Messages/Mode6/Incoming"));
    StoreMode7DataByteLocations(config->getChild("Messages/Mode7/Incoming"));
    StoreMode20DataByteLocations(config->getChild("Messages/Mode20/Incoming"));
    StoreMode42DataByteLocations(config->getChild("Messages/Mode42/Incoming"));
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Incoming::ProcessMessage(const SerialString_t &message)
{
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    // Ensure the message is for us
    if(message[MessageIdByte()] == ExpectedMessageId())
    {   // The message is for us, make sure it is the right length
        UINT8 expectedLength = (message[MessageLengthByte()] - DataLengthOffset()) + 3;  // 3 bytes are for ID, len, checksum
        if(message.length() >= expectedLength)
        {   // Verify the proper checksum
            if(ValidateCheckSum(message))
            {   // Valid message, begin processing
                status = HandleMessage(message);
            }
            else
            {   // Invalid checksum
                Server()->Log(LOG_ERRORS, "Gen3Incoming: Message checksum is not valid");
                status = BEP_STATUS_ERROR;
            }
        }
        else
        {   // Partial message received, not processing
            Server()->Log(LOG_ERRORS, "Gen3Incoming: Partial message received - %02X bytes, expected %02X bytes",
                          message.length(), expectedLength);
            status = BEP_STATUS_ERROR;
        }
    }
    else
    {   // This message is not for us
        Server()->Log(LOG_ERRORS, "Gen3Incoming: Message ID is incorrect - %02X, expected %02X",
                      message[MessageIdByte()], ExpectedMessageId());
        status = BEP_STATUS_ERROR;
    }
    // Return the processing status
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Incoming::HandleMessage(const SerialString_t &message)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Server()->Log(LOG_DEV_DATA, "Handling Mode %d Message", message[ModeByte()]);
    INT16 index, type, name, paramNum, paramVal;
    switch(message[ModeByte()])
    {
    case 1:  // Mode 1 message
        if(message[MessageTypeByte()] == 22)
        {
            index = (message[Mode1DataByte()] << 8) | message[Mode1DataByte()+1];
        }
        else
        {
            index = 0;
        }
        status = Server()->HandleMode1Message(message[MessageTypeByte()], index);
        break;

    case 2:  // Mode 2 message - icm data dump
        // Get the Marker info from the message
        index = (message[MarkerIndexByte()] << 8) | message[MarkerIndexByte()+1];
        type  = (message[Mode2MarkerTypeByte()] << 8) | message[Mode2MarkerTypeByte()+1];
        name  = (message[Mode2MarkerNameByte()] << 8) | message[Mode2MarkerNameByte()+1];
        status = Server()->HandleMode2Message(type, name, index);
        break;

    case 4:  // Mode 4 message - machine control
        status = Server()->HandleMode4Message(message[Mode4CommandByte()], message[Mode4CommandValueByte()]);
        break;

    case 6:  // Mode 6 message - parameter download
        paramNum = message[ParameterNumberByte()];
        paramVal = (message[ParameterValueByte()] << 8) | message[ParameterValueByte()+1];
        status = Server()->HandleMode6Message(paramNum, paramVal);
        break;

    case 7:  // Mode 7 message - Store data markers
        type  = (message[Mode7MarkerTypeByte()] << 8) | message[Mode7MarkerTypeByte()+1];
        name  = (message[Mode7MarkerNameByte()] << 8) | message[Mode7MarkerNameByte()+1];
        status = Server()->HandleMode7Message(type, name);
        break;

    case 20:  // Mode 20 message - brake force data
        type = (message[Mode20TestResultIdByte()] << 8) | message[Mode20TestResultIdByte()+1];
        status = Server()->HandleMode20Message(type);
        break;

    case 42:  // Mode 42 message - schedule message
        {
            vector<UINT8> scheduledMsgs;
            UINT8 currentMessage = 0;
            UINT8 unscheduledMessages = Mode42NumberAllowableScheduledMsgs();
            for(UINT8 index = Mode42MessageSlot0Byte(); 
                 currentMessage < Mode42NumberAllowableScheduledMsgs();
                 index++, currentMessage++)
            {   // Schedule the next message if it is not set to 0xFF
                scheduledMsgs.push_back(message[index]);
                if(message[index] != 0xFF)
                {
                    Server()->Log(LOG_DEV_DATA, "Gen3Incoming: Scheduling Mode 1 Message %d at index: %d", message[index], index);
                    unscheduledMessages--;
                }
            }
            // If there are no scheduled messages, turn off StartTest signal
            StartVehicleTest(unscheduledMessages < Mode42NumberAllowableScheduledMsgs());
            // Send the scheduled messages to the server for handling
            status = Server()->HandleMode42Message(scheduledMsgs);
        }
        break;

    default:
        Server()->Log(LOG_ERRORS, "Gen3Incoming: Unknown Mode - %d", message[ModeByte()]);
        status = BEP_STATUS_ERROR;
        break;
    }
    return status;
}

//-----------------------------------------------------------------------------
bool Gen3Incoming::ValidateCheckSum(const SerialString_t &message)
{
    UINT8 checksum = 0x00;
    for(UINT8 index = 0; index < (message.length()-1); index++)
    {
        checksum += message[index];
    }
    checksum = (checksum ^ 0xFF) + 1;  // Calculate 2's complement checksum
    Server()->Log(LOG_DETAILED_DATA, "Gen3Incoming: Checksum validation - calculated: %02X, message: %02X",
                  checksum, message[message.length() - 1]);
    return(checksum == message[message.length() - 1]);
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Incoming::ExpectedMessageId(const UINT8 *expectedId) /*= NULL*/
{
    if(expectedId != NULL)  m_expectedMessageId = *expectedId;
    return m_expectedMessageId;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Incoming::DataLengthOffset(const UINT8 *offset) /*= NULL*/
{
    if(offset != NULL)  m_dataLengthOffset = *offset;
    return m_dataLengthOffset;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Incoming::MessageIdByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_messageIdByte = *byte;
    return m_messageIdByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Incoming::MessageLengthByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_messageLengthByte = *byte;
    return m_messageLengthByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Incoming::ModeByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_modeByte = *byte;
    return m_modeByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Incoming::Mode1DataByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_mode1DataByte = *byte;
    return m_mode1DataByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Incoming::MessageTypeByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_messageTypeByte = *byte;
    return m_messageTypeByte;
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::Mode2MarkerTypeByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)   m_mode2MarkerTypeByte = *byte;
    return m_mode2MarkerTypeByte;
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::Mode2MarkerNameByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_mode2MarkerNameByte = *byte;
    return m_mode2MarkerNameByte;
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::Mode7MarkerTypeByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_mode7MarkerTypeByte = *byte;
    return m_mode7MarkerTypeByte;
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::Mode7MarkerNameByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_mode7MarkerNameByte = *byte;
    return m_mode7MarkerNameByte;
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::MarkerIndexByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_markerIndexByte = *byte;
    return m_markerIndexByte;
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::ParameterNumberByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_paramNumberByte = *byte;
    return m_paramNumberByte;
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::ParameterValueByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_paramValueByte = *byte;
    return m_paramValueByte;
}

//-----------------------------------------------------------------------------
inline Gen3Interface* Gen3Incoming::Server(void)
{
    return m_server;
}

//-----------------------------------------------------------------------------
void Gen3Incoming::StoreMode1DataByteLocations(const XmlNode *config)
{
    UINT8 data = 0;
    // Get the message type byte
    try
    {
        data = BposReadInt(config->getChild("MessageTypeByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Message type byte not defined, using 3: %s", excpt.GetReason());
        data = 3;
    }
    MessageTypeByte(&data);
}

//-----------------------------------------------------------------------------
void Gen3Incoming::StoreMode2DataByteLocations(const XmlNode *config)
{
    UINT8 data = 0;
    // Get the marker type byte
    try 
    {
        data = BposReadInt(config->getChild("MarkerTypeByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Marker type byte not defined, using 7: %s", excpt.GetReason());
        data = 7;
    }
    Mode2MarkerTypeByte(&data);
    // Get the marker name byte
    try
    {
        data = BposReadInt(config->getChild("MarkerNameByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Marker name byte not defined, using 5: %s", excpt.GetReason());
        data = 5;
    }
    Mode2MarkerNameByte(&data);
    // Get the marker index byte
    try
    {
        data = BposReadInt(config->getChild("MarkerIndexByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Marker name byte not defined, using 3: %s", excpt.GetReason());
        data = 3;
    }
    MarkerIndexByte(&data);
}

//-----------------------------------------------------------------------------
void Gen3Incoming::StoreMode4DataByteLocations(const XmlNode *config)
{
    UINT8 data = 0;
    try
    {
        data = BposReadInt(config->getChild("CommandByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Mode 4 command byte not specified, using 3: %s", excpt.GetReason());
        data = 3;
    }
    Mode4CommandByte(&data);
    try
    {
        data = BposReadInt(config->getChild("CommandValueByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Mode 4 command value byte not specified, using 4: %s", excpt.GetReason());
        data = 4;
    }
    Mode4CommandValueByte(&data);
}

//-----------------------------------------------------------------------------
void Gen3Incoming::StoreMode6DataByteLocations(const XmlNode *config)
{
    UINT8 data = 0;
    try
    {
        data = BposReadInt(config->getChild("ParameterNumberByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Parameter Number byte not defined, using 3: %s", excpt.GetReason());
        data = 3;
    }
    ParameterNumberByte(&data);
    try
    {
        data = BposReadInt(config->getChild("ParameterValueByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Parameter Value byte not defined, using %d: %s", 
                      ParameterNumberByte()+1, excpt.GetReason());
        data = ParameterNumberByte()+1;
    }
    ParameterValueByte(&data);
}

//-----------------------------------------------------------------------------
void Gen3Incoming::StoreMode7DataByteLocations(const XmlNode *config)
{
    UINT8 data = 0;
    // Get the marker type byte
    try 
    {
        data = BposReadInt(config->getChild("MarkerTypeByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Marker type byte not defined, using 3: %s", excpt.GetReason());
        data = 3;
    }
    Mode7MarkerTypeByte(&data);
    // Get the marker name byte
    try
    {
        data = BposReadInt(config->getChild("MarkerNameByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Marker name byte not defined, using 5: %s", excpt.GetReason());
        data = 5;
    }
    Mode7MarkerNameByte(&data);
}

//-----------------------------------------------------------------------------
void Gen3Incoming::StoreMode20DataByteLocations(const XmlNode *config)
{
    UINT8 data = 0;
    // Get the test result ID location
    try 
    {
        data = BposReadInt(config->getChild("TestResultIdByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Test result ID byte not defined for mode 20 commands, using 3: %s", excpt.GetReason());
        data = 3;
    }
    Mode20TestResultIdByte(&data);
}

//-----------------------------------------------------------------------------
void Gen3Incoming::StoreMode42DataByteLocations(const XmlNode *config)
{
    UINT8 data = 0;
    try
    {
        data = BposReadInt(config->getChild("MsgSlot0Index")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Msg Slot 0 not defined, using 3: %s", excpt.GetReason());
        data = 3;
    }
    Mode42MessageSlot0Byte(&data);
    try
    {
        data = BposReadInt(config->getChild("TotalMsgSlots")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Total allowable scheduled mode 1 messages not defined, using 20: %s", excpt.GetReason());
        data = 20;
    }
    Mode42NumberAllowableScheduledMsgs(&data);
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::Mode4CommandByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_mode4CommandByte = *byte;
    return m_mode4CommandByte;
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::Mode4CommandValueByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_mode4CommandValueByte = *byte;
    return m_mode4CommandValueByte;
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::Mode20TestResultIdByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_mode20TestResultIdByte = *byte;
    return m_mode20TestResultIdByte;
}

//-----------------------------------------------------------------------------
UINT8 Gen3Incoming::Mode42MessageSlot0Byte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_mode42MsgSlot0Byte = *byte;
    return m_mode42MsgSlot0Byte;
}

//-----------------------------------------------------------------------------
const UINT8& Gen3Incoming::Mode42NumberAllowableScheduledMsgs(const UINT8 *total /*= NULL*/)
{
    if(total != NULL)  m_mode42AllowableNumberScheduledMsgs = *total;
    return m_mode42AllowableNumberScheduledMsgs;
}

//-----------------------------------------------------------------------------
void Gen3Incoming::StartVehicleTest(const bool startTest)
{
    INamedDataBroker ndb;
    string response;
    Server()->Log(LOG_DEV_DATA, "Signaling StartTest with value %d", startTest ? "1" : "0");
    ndb.Write(START_TEST_DATA_TAG, startTest ? "1" : "0", response, true);
}
