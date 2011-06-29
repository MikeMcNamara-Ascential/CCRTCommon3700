//******************************************************************************
// Description:
//  Keyword 82 Protocol Filter for communicating with vehicle modules.
//
//==============================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#ifndef Keyword82ProtocolFilter_h
#define Keyword82ProtocolFilter_h
//-----------------------------------------------------------------------------
#include "ProtocolFilter.h"
//-----------------------------------------------------------------------------
class Keyword82ProtocolFilter : public ProtocolFilter
{
public:
    /**
     * Class Constructor
     * 
     * @param lastTxTime Variable used to hold the time of the last transmit operation. This is
     *                   designed to be a shared timestamp that all the protocol filters for
     *                   this module interface use so the module keep alive knows how long
     *                   it has been since the last string was sent to the module so as not to
     *                   send unnecessary keep alive messages
     * @param stopCommsBepCondVar
     *                   Conditional variable to allow stopping the communications in the middle of a retry loop.
     * @param commsInUse Semaphore to allow only one thread in the process to utilize the comm port at a time.
     */
    Keyword82ProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar = NULL, BepMutex *commsInUse = NULL);
    /**
     * Class destructor.
     */
    virtual ~Keyword82ProtocolFilter();
    /**
     * Initialize the serial channel.
     *
     * @param config The configuration to use for the channel initialization.
     * @return true if the initialization is successful.
     */
    virtual bool Initialize(const XmlNode *config);
    /**
     * Send a message to a vehicle module.
     *
     * @param message Message to be sent.
     * @return Status of the send.
     */
    virtual const BEP_STATUS_TYPE SendMessage(SerialString_t &message);
    /**
     * Send a message to vehicle module.
     *
     * @param messageTag Name of message to be retrieved and sent.
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE SendMessage(string messageTag);
    /**
     * Send a message to a vehicle module.
     *
     * @param messageTag Name of the message to be sent.
     * @param args       Extra data to be added to the message.
     * @return Status of the opeation.
     */
    virtual const BEP_STATUS_TYPE SendMessage(string messageTag, SerialArgs_t &args);
    /**
     * Read a module response to the message with the given tag.
     *
     * @param messageTag Tag of the message response to read.
     * @param reply      Reply from the module.
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE GetResponse(const std::string &messageTag, SerialString_t &reply);
    /**
     * Extract the data from the response.
     *
     * @param moduleResponse
     *               Response from the module.
     * @return The data that was included in the response.
     */
    virtual const SerialString_t ExtractModuleData(SerialString_t &moduleResponse);
    /**
     * Add the checksum to the message prior to transmitting the message to the module.
     *
     * @param message Message to be sent.
     */
    virtual void AddChecksumToMessage(SerialString_t &message);
    /**
     * Calculate the checksum for the provided message.
     *
     * @param message Message to calculate the checksum for
     * @return Checksum of the provided message.
     */
    UINT16 ComputeTwoByteCheckSum(const SerialString_t &message);

protected:
    /**
     * Wait for a window to send the message.  
     * The comm bus will be monitored for an acceptable window prior to sending the message.
     * 
     * @return Flag indicating if an acceptable window to send the message exists.
     */
    bool WaitForSendWindow(void);

private:
    /**
     * Get/Set the amount of time that indicates an acceptable window exists for transmitting a message.
     * 
     * @param windowTime Amount of time that indicates an acceptable window exists for transmitting a message.
     * 
     * @return Amount of time that indicates an acceptable window exists for transmitting a message.
     */
    const UINT32& MessageWindowDetectTime(const UINT32 *windowTime = NULL);
    /**
     * Get/Set the number of attempts to detect the message send window on the vehicle bus.
     * 
     * @param windowDetectAttempts
     *               Number of attempts to detect the message send window on the vehicle bus.
     * 
     * @return Number of attempts to detect the message send window on the vehicle bus.
     */
    const INT32& MessageWindowDetectAttempts(const INT32 *windowDetectAttempts = NULL);
    /** Amount of timie between bytes that indicates an acceptable message transmit window */
    UINT32 m_messageWindowDetectTime;
    /** Maximum number of times to wait for a send window to exist */
    INT8   m_maxWindowDetectAttempts;
};
//-----------------------------------------------------------------------------
#endif //Keyword82ProtocolFilter_h
