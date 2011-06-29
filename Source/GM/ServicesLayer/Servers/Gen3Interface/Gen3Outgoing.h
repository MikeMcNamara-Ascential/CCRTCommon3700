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
#ifndef Gen3Outgoing_h
#define Gen3Outgoing_h
//-----------------------------------------------------------------------------
#include "Gen3Interface.h"
//-----------------------------------------------------------------------------
class Gen3Interface;
//-----------------------------------------------------------------------------

class Gen3Outgoing
{
public:
    /**
     * Class constructor.
     * 
     * @param server Pointer to the server that instantiated this object.
     * @param commChannel
     *               Serial communication channel to use for transmitting messages.
     */
    Gen3Outgoing(Gen3Interface *server, SerialChannel *commChannel);
    /**
     * Class destructor.
     */
    virtual ~Gen3Outgoing();
    /**
     * Initialize the class.
     * 
     * @param config Configuration data to use for setting up the class.
     */
    void Initialize(const XmlNode *config);
    /**
     * Send the provided message.  The mode information and message will be wrapped and sent to the GM DVT.
     * 
     * @param mode    Message mode to be sent.
     * @param message Message to be sent.
     * 
     * @return Status of sending the message.
     */
    BEP_STATUS_TYPE SendMessage(const UINT8 &mode, const UINT8 &dataLength, SerialString_t &message);

private:
    /**
     * Get/Set the offset to apply to the message data length.
     * 
     * @param offset Offset to apply to the message data length.
     * 
     * @return Offset to apply to the message data length.
     */
    const UINT8& DataLengthOffset(const UINT8 *offset = NULL);
    /**
     * Get/Set the byte location for the message ID.
     * 
     * @param byte   Byte location for the message ID.
     * 
     * @return Byte location for the message ID.
     */
    const UINT8& MsgIdByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte location for the message length.
     * 
     * @param byte   Byte location for the message length.
     * 
     * @return Byte location for the message length.
     */
    const UINT8& MsgLengthByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte location for the mode.
     * 
     * @param byte   Byte location for the mode.
     * 
     * @return Byte location for the mode.
     */
    const UINT8& ModeByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte location for the actual start of data/
     * 
     * @param byte   Byte location for the start of data.
     * 
     * @return Byte location for the start of data.
     */
    const UINT8& DataStartByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the message ID to be added to every outgoing message.
     * 
     * @param id     Message ID to be added to the message.
     * 
     * @return Message ID to be added to the message.
     */
    const UINT8& MessageId(const UINT8 *id = NULL);
    /**
     * Get/Set the communication object to use for sending the message.
     * 
     * @param channel Communication object to use for sending messages.
     * 
     * @return Communication object to use for sending messages.
     */
    SerialChannel* CommChannel(SerialChannel *channel = NULL);
    /**
     * Get/Set the server object that instantiated this object.
     * 
     * @param server Pointer to the server that created this object.
     * 
     * @return Pointer to the server that created this object.
     */
    Gen3Interface* Server(Gen3Interface *server = NULL);
    /**
     * Offset to use for calculating the message length.
     */
    UINT8 m_dataLengthOffset;
    /**
     * Message byte location for the message ID.
     */
    UINT8 m_messageIdByte;
    /**
     * Message byte location for the message length.
     */
    UINT8 m_messageLengthByte;
    /**
     * Message byte location for the Mode.
     */
    UINT8 m_modeByte;
    /**
     * Message byte location where the actual data begins.
     */
    UINT8 m_dataStartByte;
    /**
     * Message ID to be added to every outgoing message.
     */
    UINT8 m_messageId;
    /**
     * Communication channel to use for sending messages.
     */
    SerialChannel *m_commChannel;
    /**
     * Pointer to the server that created this object.
     */
    Gen3Interface *m_server;
};
//-----------------------------------------------------------------------------
#endif //Gen3Outgoing_h
