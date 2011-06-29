//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  The Gen3Outgoing class will build a message to be sent to the GM DVT.
//  
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
#include "Gen3Outgoing.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Gen3Outgoing::Gen3Outgoing(Gen3Interface *server, SerialChannel *commChannel)
{   // Store the pointers
    Server(server);
    CommChannel(commChannel);
}

//-----------------------------------------------------------------------------
Gen3Outgoing::~Gen3Outgoing()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
void Gen3Outgoing::Initialize(const XmlNode *config)
{
    UINT8 data = 0x00;
    // Get the data length offset
    try
    {
        data = BposReadInt(config->getChild("DataLengthOffset")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Gen3Outgoing: Data length offset not defined, using 0x55 - %s", excpt.GetReason());
        data = 0x55;
    }
    DataLengthOffset(&data);
    // Get the Message ID byte
    try
    {
        data = BposReadInt(config->getChild("MessageIdByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Gen3outgoing: Message ID byte not defined, using 0 - %s", excpt.GetReason());
        data = 0;
    }
    MsgIdByte(&data);
    // Get the message length byte
    try
    {
        data = BposReadInt(config->getChild("DataLengthByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Gen3outgoing: Message length byte not defined, using 1 - %s", excpt.GetReason());
        data = 1;
    }
    MsgLengthByte(&data);
    // Get the Mode byte location
    try
    {
        data = BposReadInt(config->getChild("ModeByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Gen3outgoing: Mode byte location not defined, using 2 - %s", excpt.GetReason());
        data = 2;
    }
    ModeByte(&data);
    // Get the data start index
    try
    {
        data = BposReadInt(config->getChild("Outgoing/DataStartIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Gen3outgoing: Data start index not defined, using 3 - %s", excpt.GetReason());
        data = 3;
    }
    DataStartByte(&data);
    // Get the message ID
    try
    {
        data = BposReadInt(config->getChild("Outgoing/MessageId")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "Gen3outgoing: Message ID not defined, using 0x07 - %s", excpt.GetReason());
        data = 0x07;
    }
    MessageId(&data);
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Outgoing::SendMessage(const UINT8 &mode, const UINT8 &dataLength, SerialString_t &message)
{
    SerialString_t g3Msg;   // Final message to send
    UINT8 checkSum = 0x00;
    char buff[16];
    string strLogMsg;
    // Set aside some memory
    g3Msg.reserve(dataLength + 4);
    g3Msg.resize(dataLength + 4);
    // Add the header to the message
    g3Msg[MsgIdByte()] = MessageId();
    checkSum += g3Msg[MsgIdByte()];
    strLogMsg += CreateMessage(buff, sizeof(buff), "\n%02d: %02X ", MsgIdByte()+1, g3Msg[MsgIdByte()]);

    g3Msg[MsgLengthByte()] = dataLength + 1 + DataLengthOffset();  // Add 1 for the mode byte
    checkSum += g3Msg[MsgLengthByte()];
    strLogMsg += CreateMessage(buff, sizeof(buff), "\n%02d: %02X ", MsgLengthByte()+1, g3Msg[MsgLengthByte()]);

    g3Msg[ModeByte()] = mode;
    checkSum += g3Msg[ModeByte()];
    strLogMsg += CreateMessage(buff, sizeof(buff), "\n%02d: %02X ", ModeByte()+1, g3Msg[ModeByte()]);

    // Add the data to the message and calculate the checksum
    for(UINT16 index = 0; index < message.length(); index++) 
    {
        g3Msg[index + DataStartByte()] = message[index];
        checkSum += g3Msg[index + DataStartByte()];
        strLogMsg += CreateMessage(buff, sizeof(buff), "\n%02d: %02X ", index + DataStartByte()+1, g3Msg[index + DataStartByte()]);
    }
    // Take the two's complement of the checksum
    checkSum = (checkSum ^ 0xFF) + 1;
    UINT8 checkSumIndex = DataStartByte() + message.length();
    g3Msg[checkSumIndex] = checkSum;
    strLogMsg += CreateMessage(buff, sizeof(buff), "\n%02d: %02X ", checkSumIndex+1, g3Msg[checkSumIndex]);
    Server()->Log(LOG_DEV_DATA, "Setting checksum (%02X) in byte %d", checkSum, checkSumIndex);
    // Send the message
    Server()->Log(LOG_DEV_DATA, "Sending msg: %s", strLogMsg.c_str());
    BEP_STATUS_TYPE status = CommChannel()->Send(g3Msg);
    Server()->Log(LOG_DEV_DATA, "Sent message: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Outgoing::DataLengthOffset(const UINT8 *offset /*= NULL*/)
{
    if(offset != NULL)  m_dataLengthOffset = *offset;
    return m_dataLengthOffset;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Outgoing::MsgIdByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_messageIdByte = *byte;
    return m_messageIdByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Outgoing::MsgLengthByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_messageLengthByte = *byte;
    return m_messageLengthByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Outgoing::ModeByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_modeByte = *byte;
    return m_modeByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Outgoing::DataStartByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_dataStartByte = *byte;
    return m_dataStartByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& Gen3Outgoing::MessageId(const UINT8 *id /*= NULL*/)
{
    if(id != NULL)  m_messageId = *id;
    return m_messageId;
}

//-----------------------------------------------------------------------------
inline SerialChannel* Gen3Outgoing::CommChannel(SerialChannel *channel /*= NULL*/)
{
    if(channel != NULL)  m_commChannel = channel;
    return m_commChannel;
}

//-----------------------------------------------------------------------------
inline Gen3Interface* Gen3Outgoing::Server(Gen3Interface *server /*= NULL*/)
{
    if(server != NULL)  m_server = server;
    return m_server;
}
