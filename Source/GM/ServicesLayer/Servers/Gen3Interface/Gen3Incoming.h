//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  The Gen3Incoming class will handle incoming data from the GM DVT.  The
//  message will be processed and any necessary data items updated.
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
#ifndef Gen3Incoming_h
#define Gen3Incoming_h
//-----------------------------------------------------------------------------
#include "Gen3Interface.h"
//-----------------------------------------------------------------------------
class Gen3Interface;
//-----------------------------------------------------------------------------

class Gen3Incoming
{
public:
    /**
     * Class constructor.
     * 
     * @param server Pointer to the server that instantiated this object.
     */
    Gen3Incoming(Gen3Interface *server);
    /**
     * Class destructor.
     */
    virtual ~Gen3Incoming();
    /**
     * Initialize the class.
     * Data items and byte locations will be stored in member variables for more convenient access;
     * 
     * @param config Configuration data to use for initializing the object.
     */
    void Initialize(const XmlNode *config);
    /**
     * Process the message received from the DVT.
     * The message will be checked to ensure it is valid prior to processing.
     * 
     * @param message Message to be processed.
     * 
     * @return Status of processing the message.  If the message is not valid, BEP_STATUS_ERROR will be returned.
     */
    BEP_STATUS_TYPE ProcessMessage(const SerialString_t &message);

private:
    /**
     * Handle the message.
     * The mode will be determined and the appropriate actions will be performed.
     * 
     * @param message Message to be analyzed.
     * 
     * @return Status of handling the message.
     */
    BEP_STATUS_TYPE HandleMessage(const SerialString_t &message);
    /**
     * Validate the message checksum.
     * The checksum will be calculated and compared to the checksum in the message.
     * 
     * @param message
     * 
     * @return 
     */
    bool ValidateCheckSum(const SerialString_t &message);
    /**
     * Get/Set the expected message ID.
     * This value will be compared against the message ID in the incoming message to ensure the message is intended for BEP.
     * 
     * @param expectedId Expected Message ID
     * 
     * @return Expected Message ID
     */
    const UINT8& ExpectedMessageId(const UINT8 *expectedId = NULL);
    /**
     * Get/Set the data length offset.
     * 
     * @param offset Data length offset.
     * 
     * @return Data length offset.
     */
    const UINT8& DataLengthOffset(const UINT8 *offset = NULL);
    /**
     * Get/Set the location of the Message ID in the incoming message.
     * 
     * @param byte   Byte containing the message ID.
     * 
     * @return Byte containing the message ID.
     */
    const UINT8& MessageIdByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte containing the number of data bytes in the message.
     * 
     * @param byte   Byte containing the number of data bytes in the message.
     * 
     * @return Byte containing the number of data bytes in the message.
     */
    const UINT8& MessageLengthByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte containing the mode data.
     * 
     * @param byte   Byte containing the mode data.
     * 
     * @return Byte containing the mode data.
     */
    const UINT8& ModeByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte the contains the Mode 1 data.
     * 
     * @param byte   Byte the contains the Mode 1 data.
     * 
     * @return Byte the contains the Mode 1 data.
     */
    const UINT8& Mode1DataByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte containing the message type.
     * 
     * @param byte   Byte containing the message type.
     * 
     * @return Byte containing the message type.
     */
    const UINT8& MessageTypeByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte containing the marker type to be retrieved.
     * 
     * @param byte   Byte containing the marker type to be retrieved.
     * 
     * @return Byte containing the marker type to be retrieved.
     */
    const UINT8& Mode2MarkerTypeByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte containing the marker name to be retrieved.
     * 
     * @param byte   Byte containing the marker name to be retrieved.
     * 
     * @return Byte containing the marker name to be retrieved.
     */
    const UINT8& Mode2MarkerNameByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte containing the marker type to be set.
     * 
     * @param byte   Byte containing the marker type to be set.
     * 
     * @return Byte containing the marker type to be set.
     */
    const UINT8& Mode7MarkerTypeByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte containing the marker name to be set.
     * 
     * @param byte   Byte containing the marker name to be set.
     * 
     * @return Byte containing the marker name to be set.
     */
    const UINT8& Mode7MarkerNameByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte containing the marker index to be retrieved.
     * 
     * @param byte   Byte containing the marker index to be retrieved.
     * 
     * @return Byte containing the marker index to be retrieved.
     */
    const UINT8& MarkerIndexByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte location containing the parameter number.
     * 
     * @param byte   Byte location containing the parameter number.
     * 
     * @return Byte location that contains the parameter number.
     */
    const UINT8& ParameterNumberByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte location containing the parameter value.
     * 
     * @param byte   Byte location containing the parameter value.
     * 
     * @return Byte location that contains the parameter value.
     */
    const UINT8& ParameterValueByte(const UINT8 *byte = NULL);
    /**
     * Get the pointer to the main Gen3 Interface process.
     * 
     * @return Pointer to the main Gen 3 Interface process.
     */
    Gen3Interface* Server(void);
    /**
     * Store the data byte location information for Mode 1 messages.
     * 
     * @param config Mode 1 message data locations.
     */
    void StoreMode1DataByteLocations(const XmlNode *config);
    /**
     * Store the data byte location information for Mode 2 messages.
     * 
     * @param config Mode 2 message data locations.
     */
    void StoreMode2DataByteLocations(const XmlNode *config);
    /**
     * Store the data byte location information for Mode 4 messages.
     * 
     * @param config Mode 4 message data locations.
     */
    void StoreMode4DataByteLocations(const XmlNode *config);
    /**
     * Store the data byte location information for Mode 6 messages.
     * 
     * @param config Mode 6 message data locations.
     */
    void StoreMode6DataByteLocations(const XmlNode *config);
    /**
     * Store the data byte location information for Mode 7 messages.
     * 
     * @param config Mode 7 message data locations.
     */
    void StoreMode7DataByteLocations(const XmlNode *config);
    /**
     * Store the data byte location information for Mode 20 messages.
     * 
     * @param config Mode 20 message data locations.
     */
    void StoreMode20DataByteLocations(const XmlNode *config);
    /**
     * Store the data byte location information for Mode 42 messages.
     * 
     * @param config Mode 42 message data locations.
     */
    void StoreMode42DataByteLocations(const XmlNode *config);
    /**
     * Get/Set the byte location containing the Mode 4 command byte.
     * 
     * @param byte   The byte location containing the Mode 4 command byte.
     * 
     * @return The byte location containing the Mode 4 command byte.
     */
    const UINT8& Mode4CommandByte(const UINT8 *byte = NULL);
    /**
     * Set/Get the location of the mode 4 command value byte.
     * 
     * @param byte   Location of the mode 4 command value byte.
     * 
     * @return Location of the mode 4 command value byte.
     */
    const UINT8& Mode4CommandValueByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the test result ID byte in mode 20 commands.
     * 
     * @param byte   The location of the test result ID byte in mode 20 commands.
     * 
     * @return The location of the test result ID byte in mode 20 commands.
     */
    const UINT8& Mode20TestResultIdByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the byte location that contains the first scheduled Mode 1 message.
     * 
     * @param byte   Byte location that contains the first scheduled Mode 1 message.
     * 
     * @return Byte location that contains the first scheduled Mode 1 message.
     */
    UINT8 Mode42MessageSlot0Byte(const UINT8 *byte = NULL);
    /**
     * Get/Set the total number of allowable scheduled mode 1 messages.
     * 
     * @param total  Total number of allowable scheduled mode 1 messages.
     * 
     * @return Total number of allowable scheduled mode 1 messages.
     */
    const UINT8& Mode42NumberAllowableScheduledMsgs(const UINT8 *total = NULL);
    /**
     * Command the system to either start or end a test.
     * 
     * @param startTest Flag indicating if a test should be started or stopped.
     */
    void StartVehicleTest(const bool startTest);
    /** Expected Message ID. */
    UINT8 m_expectedMessageId;
    /** Offset to use for determining the number of data bytes in the message. */
    UINT8 m_dataLengthOffset;
    /** Byte index of the message ID. */
    UINT8 m_messageIdByte;
    /** Message byte that contains the number of data bytes in the message. */
    UINT8 m_messageLengthByte;
    /** Byte containing the Mode. */
    UINT8 m_modeByte;
    /** Byte containing any special mode 1 data to be processed */
    UINT8 m_mode1DataByte;
    /** Byte containing the message type. */
    UINT8 m_messageTypeByte;
    /** Type of marker to be retrieved. */
    UINT8 m_mode2MarkerTypeByte;
    /** Name of the marker to be retrieved. */
    UINT8 m_mode2MarkerNameByte;
    /** Type of marker to be set. */
    UINT8 m_mode7MarkerTypeByte;
    /** Name of the marker to be set. */
    UINT8 m_mode7MarkerNameByte;
    /** Index of the marker to be retrieved. */
    UINT8 m_markerIndexByte;
    /** Byte location containing the parameter number. */
    UINT8 m_paramNumberByte;
    /** Byte location containing the parameter value. */
    UINT8 m_paramValueByte;
    /** Byte continaing the mode 4 command to process. */
    UINT8 m_mode4CommandByte;
    /** Byte location containing the value of the Mode 4 command. */
    UINT8 m_mode4CommandValueByte;
    /** Byte location containing the test result id in a mode 20 command */
    UINT8 m_mode20TestResultIdByte;
    /** Byte location containing the first scheduled Mode 1 message */
    UINT8 m_mode42MsgSlot0Byte;
    /** Total number of allowable scheduled Mode 1 messages */
    UINT8 m_mode42AllowableNumberScheduledMsgs;
    /** Pointer to the server that instantiated this object. */
    Gen3Interface *m_server;
};

//-----------------------------------------------------------------------------
#endif //Gen3Incoming_h
