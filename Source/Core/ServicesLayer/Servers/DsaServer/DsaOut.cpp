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
#include "DsaOut.h"
#include <time.h>
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
DsaOut::DsaOut(const XmlNode *config, DsaServer *server)
{   // Store the pointer to the server
    Server(server);
    // Store the message data
    StoreMessageData(config);
}

//-----------------------------------------------------------------------------
DsaOut::~DsaOut()
{   // Nothing special to be done
}

//-----------------------------------------------------------------------------
void DsaOut::StoreMessageData(const XmlNode *config)
{   
    const XmlNode *outGoingData = config->getChild("Setup/OutgoingMessageData");
    // Set the size of the outgoing message
    INT16 messageLength;
    try
    {
        messageLength = BposReadInt(outGoingData->getChild("MessageLength")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaOut::StoreMessageData - Message length not defined, using 44: %s", excpt.GetReason());
        messageLength = 44;
    }
    m_dsaMessage.reserve(messageLength);
    m_dsaMessage.resize(messageLength);
    memset(&m_dsaMessage[0], 0, messageLength);
    // Store the byte location data
    try
    {
        m_byteLocations.DeepCopy(outGoingData->getChild("ByteLocations")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaOut::StoreMessageData - Byte location descriptions not defined - %s", excpt.GetReason());
        m_byteLocations.clear(true);
    }
    // Setup the message ID
    SetupMessageId(outGoingData);
    // Set default data items
    UpdateMessageData("StatusInfo1", 0x01);
    UpdateMessageData("VehicleCenteringCopy", 127);
    UpdateMessageData("VehicleCentering", 127);
    UpdateMessageData("ExtendedUnitState1", 0x01);
    UpdateMessageData("ExtendedUnitState2", 0xFB);
}

//-----------------------------------------------------------------------------
void DsaOut::SetupMessageId(const XmlNode *config)
{   // Get the message ID from the config
    INT8 messageId;
    try
    {
        messageId = BposReadInt(config->getChild("MessageId")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaOut::SetupMessageId - Message ID not defined, using 1 - %s", excpt.GetReason());
        messageId = 1;
    }
    // Store the message ID in the message
    UpdateMessageData(GetByteLocation("MessageIdByte"), messageId);
    Server()->Log(LOG_DEV_DATA, "DsaOut::SetupMessageId - Message ID set");
}

//-----------------------------------------------------------------------------
UINT8 DsaOut::GetByteLocation(const string &byteDescription)
{
    UINT8 location = 0;
    XmlNodeMapItr iter = m_byteLocations.find(byteDescription);
    if(iter != m_byteLocations.end())
    {   // Get the byte location 
        location = BposReadInt(iter->second->getValue().c_str());
    }
    else
    {
        Server()->Log(LOG_ERRORS, "DsaOut::GetByteLocation - Byte location not defined for %s, using 0", 
                      byteDescription.c_str());
    }
    Server()->Log(LOG_DEV_DATA, "DsaOut::GetByteLocation - %s:%d", byteDescription.c_str(), location);
    return location;
}

//-----------------------------------------------------------------------------
inline void DsaOut::UpdateMessageData(const UINT8 byteLocation, INT8 data)
{   // Place the data into the message
    Server()->Log(LOG_DEV_DATA, "DsaOut::UpdateMessageData(byte: %d, value: %d)", byteLocation, data);
    m_dsaMessage[byteLocation] = data;
    // Update the message checksum
    m_dsaMessage[GetByteLocation("Checksum")] = CalculateChecksum();
}

//-----------------------------------------------------------------------------
inline void DsaOut::UpdateMessageData(const string &byteLocation, INT8 data)
{
    UpdateMessageData(GetByteLocation(byteLocation), data);
}

//-----------------------------------------------------------------------------
void DsaOut::UpdateDataBit(const string &byte, const UINT8 &bit, INT8 value)
{   // Create a mask to use for the bit
    UINT8 bitMask = 0x01 << bit;
    INT8 currentData = m_dsaMessage[GetByteLocation(byte)];
    if(value)
    {   // Set the data bit 
        currentData |= bitMask;
    }
    else
    {   // Clear the bit 
        currentData &= ~(bitMask);
    }
    // Store the data in the message
    UpdateMessageData(byte, currentData);
}

//-----------------------------------------------------------------------------
bool DsaOut::ProcessResponse(void)
{   // Update the time stamp in the message
    time_t currentTime = time(NULL);
    UpdateMessageData("TimeHi", ((currentTime & 0xFF00) >> 8));
    UpdateMessageData("TimeLo", (currentTime & 0x00FF));
    // Send the response to the MFT
    return Server()->SendResponse(m_dsaMessage);
}

//-----------------------------------------------------------------------------
DsaServer* DsaOut::Server(DsaServer *server /*= NULL*/)
{
    if(server != NULL)  m_server = server;
    return m_server;
}

//-----------------------------------------------------------------------------
UINT8 DsaOut::CalculateChecksum(void)
{
    UINT8 checksum = 0;
    UINT8 checksumByte = GetByteLocation("Checksum");
    for(UINT8 index = 0; index < checksumByte; index++)  checksum ^= m_dsaMessage[index];
    return checksum;
}
