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
#ifndef DsaOut_h
#define DsaOut_h
//-----------------------------------------------------------------------------
#include "DsaServer.h"

class DsaServer;

typedef basic_string<INT8>	DsaSerialString_t;


/**
 * DSA outgoing message processor.
 * This class will construct a response to each message received from the Multi Function Tester.
 * 
 * @author Dan Erickson
 * @see DsaServer
 * @since March 05, 2008
 */
class DsaOut
{
public:
    /**
     * Class constructor.
     * Configuration data items will also be stored.
     * 
     * @param config Configuration Data to use for setting up the object.
     * @param server Pointer to the server that created this object.
     */
    DsaOut(const XmlNode *config, DsaServer *server);
    /**
     * Class destructor.
     */
    virtual ~DsaOut();
    /**
     * Update data in the outgoing message.
     * The specified data will be inserted into the message at the specified location.
     * 
     * @param byteLocation
     *               Location at which to place the provided data.
     * @param data   The data to be placed into the message.
     */
    void UpdateMessageData(const string &byteLocation, INT8 data);
    /**
     * Update the error status data to be returned to the MFT.
     * 
     * @param byte Error byte to be updated.
     * @param bit  Bit in the byte to store the data value.
     * @param value     Value to be stored in the bit.
     */
    void UpdateDataBit(const string &byte, const UINT8 &bit, INT8 value);
    /**
     * Process the data to send to the MFT in the response message.
     * 
     * @return Flag indicating if the response was properly processed.
     */
    bool ProcessResponse(void);

protected:
    /**
     * Store any necessary data for processing an outgoing message.
     * 
     * @param config Configuration data.
     */
    void StoreMessageData(const XmlNode *config);
    /**
     * Update data in the outgoing message.
     * The specified data will be inserted into the message at the specified location.
     * 
     * @param byteLocation
     *               Location at which to place the provided data.
     * @param data   The data to be placed into the message.
     */
    void UpdateMessageData(const UINT8 byteLocation, INT8 data);

private:
    /**
     * The message ID data will be retrieved from the config data.  The message ID will then be
     * set into the message.
     * 
     * @param config Configuration data to use for setting up the message ID.
     */
    void SetupMessageId(const XmlNode *config);
    /**
     * Get the location for the provided byte description.
     * 
     * @param byteDescription
     *               Byte description to find the location for.
     * 
     * @return Location of the byte in the message.
     */
    UINT8 GetByteLocation(const string &byteDescription);
    /**
     * Get/Set the pointer to the server that created this object.
     * 
     * @param server Pointer to the server that created this object.
     * 
     * @return Pointer to the server that created this object.
     */
    DsaServer* Server(DsaServer *server = NULL);
    /**
     * Calculate the checksum of the message.
     * 
     * @return Calculated checsum to be added to the message.
     */
    UINT8 CalculateChecksum(void);
    /**
     * Pointer to the server that created this object.
     * 
     * @see DsaServer
     */
    DsaServer *m_server;
    /**
     * Message to be sent to the MFT in response to a service request.
     */
    DsaSerialString_t m_dsaMessage;
    /**
     * List of byte locations in the message.
     */
    XmlNodeMap m_byteLocations;
};

//-----------------------------------------------------------------------------
#endif  //Dsaout_h
