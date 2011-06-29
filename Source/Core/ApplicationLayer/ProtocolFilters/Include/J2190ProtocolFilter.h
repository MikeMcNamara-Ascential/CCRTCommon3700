//******************************************************************************
// Description:
//  J2190 Protocol Filter for communicating with vehicle modules.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/Include/J2190ProtocolFilter.h $
// 
// 4     10/30/07 1:37p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javdoc comments.
// 
// 3     10/25/06 3:29p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a timer to tell when the last transmit message was sent.
// This
//       allows the keep alive message to be sent when needed instead of
// on a
//       specific time interval.
// 
// 2     11/11/05 4:54p Cward
// Changes for 0511101 iteration
//
// 1     4/28/04 11:06p Cward
// Added to core
//
// 2     3/02/04 12:34p Derickso
// Updated to use BepMutex instead of BepSemaphore.
//
// 1     1/27/04 9:31a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
//
// 4     1/22/04 12:37p Derickso
// Overloaded WaitForFullResponse to be more efficient and to clear up
// errors retrieving module data.
//
// 3     1/20/04 6:41a Derickso
// Changed Semaphore to BepSemaphore.
//
// 2     1/10/04 9:01a Derickso
// Latest updates based on changes to DCXKeywordProtocol200 protocol
// filter.
//
// 1     12/17/03 10:17a Derickso
// Created.  Initial files from Bill and Greg.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef J2190ProtocolFilter_h
#define J2190ProtocolFilter_h
//==============================================================================

#include "ProtocolFilter.h"

/**
 * J2190 Protocol filter.
 * This class will send and receive J2190 messages.
 * The ExtractModuleData() method has been instantiated to strip the header and
 * checksum from the module response.
 *
 * @author Greg Pattison
 * @version Version 1.0
 * @see ProtocolFilter
 * @since November 04, 2003
 */
class J2190ProtocolFilter : public ProtocolFilter
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
     * @param commsInUse Semaphore to allow only one thread in the process to utilize the comm port at a time.
     * @since Version 1.0
     */
    J2190ProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar = NULL, BepMutex *commsInUse = NULL);

    /**
     * Class destructor.
     * @since Version 1.0
     */
    virtual ~J2190ProtocolFilter();
    /**
     * Initialize the serial channel.
     *
     * @param config The configuration to use for the channel initialization.
     * @return true if the initialization is successful.
     * @since Version 1.0
     */
    virtual bool Initialize(const XmlNode *config);
    /**
     * Send a message to a vehicle module.
     * NOTE:  The module ID will be prepended to the message
     *        prior to sending the message out of the port.
     *
     * @param message Message to be sent.
     *
     * @return Status of the send.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(SerialString_t &message);
    /**
     * Send a message to a vehicle module.
     * NOTE:  The module ID will be prepended to the message
     *        prior to sending the message out of the port.
     *
     * @param message Message to be sent.
     * @param addModuleID
     *                Flag to indicate if the message ID and module ID
     *                should be added to the message prior to sending.
     *
     * @return Status of the send.
     */
    virtual const BEP_STATUS_TYPE SendMessage(SerialString_t &message, const bool addModuleID);
    /**
     * Send a message to vehicle module.
     *
     * @param messageTag Name of message to be retrieved and sent.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag);
    /**
     * Send a message to a vehicle module.
     *
     * @param messageTag Name of the message to be sent.
     * @param args       Extra data to be added to the message.
     * @return Status of the opeation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag, SerialArgs_t &args);
    /**
     * Broadcast a message on the vehicle bus.
     *
     * @param messageTag Tag for the message to be broadcast.
     *
     * @return Status of the broadcast.
     */
    virtual const BEP_STATUS_TYPE SendBusBroadcastMessage(string messageTag);
    /**
     * Send a message to the module and read the response.
     *
     * @param messageTag Tag of message to send.
     * @param reply      Reply from the module.
     * @param args       Optional data to be written to the module.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE GetModuleData(std::string messageTag, SerialString_t &reply, SerialArgs_t *args = NULL);
    /**
     * Wait for a message broadcast on the vehicle bus.
     *
     * @param messageTag Message to look for on the vehicle bus.
     * @param messageWaitTime
     *                   Amount of time to wait for the broadcast message.
     * @param busMssg    Location to store the broadcast response.
     *
     * @return Status of waiting for the broadcast message.
     */
    virtual const BEP_STATUS_TYPE GetBusBroadcastMessage(string messageTag, const long messageWaitTime, SerialString_t &busMssg);
    /**
     * Extract the data from the response.
     *
     * @param moduleResponse
     *               Response from the module.
     * @return The data that was included in the response.
     * @since Version 1.0
     */
    const SerialString_t ExtractModuleData(SerialString_t &moduleResponse);
    /**
     * Get the negative response code indicating the module is not in diagnostic mode.
     *
     * @return Negative response code for module not in diagnostic mode
     * @since Version 1.0
     */
    const UINT8& GetEnterDiagnosticModeCode(void);
    /**
     * Get the negative response code indicating the response is pending
     *
     * @return Negative response code for response pending
     * @since Version 1.0
     */
    const UINT8& GetResponsePendingCode(void);
    /**
     * Get the negative response code indicating the module is busy and to repeat the request
     *
     * @return Negative response code for module busy, repeat request
     * @since Version 1.0
     */
    const UINT8& GetModuleBusyCode(void);
    /**
     * Get the tag for the message to place the module into diagnostic mode
     *
     * @return Tag for the Enter Diagnostic Mode message
     * @since Version 1.0
     */
    const string& GetEnterDiagnosticModeMessageTag(void);
    /**
     * Get the maximum number of additional reads to perform
     * if the module responds with a response pending result.
     *
     * @return Number of additional reads to perform.
     * @since Version 1.0
     */
    const INT32& GetNumberOfResponsePendingReads(void);
    /**
     * Get the flag indicating if diagnostic mode should be
     * entered automatically if the module is not in diagnostic
     * mode.
     *
     * @return True - automatically enter diagnostic mode.
     * @since Version 1.0
     */
    const bool& AutomaticallyEnterDiagnosticMode(void);
    /**
     * Get the diagnostic request code to use for sending messages to the module.
     *
     * @return diagnostic request code of the module.
     * @since Version 1.0
     */
    const SerialString_t GetModuleRequestCode(void);
    /**
     * Get the diagnostic response code to use for receiving messages to the module.
     *
     * @return diagnostic response code of the module.
     * @since Version 1.0
     */
    const SerialString_t GetModuleResponseCode(void);
    /**
     * Get the ID to use for sending message to the module.
     *
     * @return Request ID of the module.
     * @since Version 1.0
     */
    const SerialString_t GetModuleRequestID(void);

protected:
    /**
     * Attempts to read an entire message from the Comm Proxy.
     * If no data is returned in the alloted time, the
     * read will timeout and this method will return.
     *
     * @param response Buffer to read the response into
     *
     * @return Number of characters received or -1 on error
     */
    virtual int WaitForFullResponse( SerialString_t &response);

private:
    /**
     * Set the negative response code indicating the module is not in diagnostic mode.
     *
     * @param  enterDiagnosticModeCode Negative response code for module not in diagnostic mode
     * @since Version 1.0
     */
    void SetEnterDiagnosticModeCode(const UINT8 &enterDiagnosticModeCode);
    /**
     * Set the negative response code indicating the response is pending.
     *
     * @param  responsePendingCode Negative response code for response pending
     * @since Version 1.0
     */
    void SetResponsePendingCode(const UINT8 &responsePendingCode);
    /**
     * Set the negative response code indicating the module is busy and to repeat the request.
     *
     * @param  moduleBusyCode Negative response code for module busy, repeat request
     * @since Version 1.0
     */
    void SetModuleBusyCode(const UINT8 &moduleBusyCode);
    /**
     * Set the tag for the Enter Diagnostic Mode message
     *
     * @param  tag Tag for the Enter Diagnostic Mode message
     * @since Version 1.0
     */
    void SetEnterDiagnosticModeMessageTag(const string& tag);
    /**
     * Set the flag to determine if diagnostic mode should
     * be entered automatically.
     *
     * @param autoEnterDiagMode
     *               True - automatically enter diagnostic mode if module
     *               is not in diagnostic mode.
     *               False - do not automatically enter diagnostic mode.
     * @since Version 1.0
     */
    void SetAutomaticallyEnterDiagnosticMode(const bool& autoEnterDiagMode);
    /**
     * Store the maximum number of reads to perform if the
     * module responds with a response pending result.
     *
     * @param responsePendingReads
     *               Number of reads to perform.
     * @since Version 1.0
     */
    void SetResponsePendingReads(const INT32& responsePendingReads);
    /**
     * Store the module diagnostic request code.
     *
     * @param moduleRequestCode
     *               diagnostic code used for sending messages to module
     * @since Version 1.0
     */
    void SetModuleRequestCode(const UINT8& moduleRequestCode);
    /**
     * Store the module diagnostic response code.
     *
     * @param moduleResponseCode
     *               diagnostic code used by module for sending responses
     * @since Version 1.0
     */
    void SetModuleResponseCode(const UINT8& moduleResponseCode);
    /**
     * Store the module id for requests.
     *
     * @param moduleID ID to use for sending messages to the module.
     * @since Version 1.0
     */
    void SetModuleRequestID(const UINT8& moduleID);
    /**
     * Negative response code indicating the module is not in Diagnostic mode.
     * @since Version 1.0
     */
    UINT8 m_enterDiagModeCode;
    /**
     * Negative response code indicating the response is pending.
     * @since Version 1.0
     */
    UINT8 m_responsePendingCode;
    /**
     * Negative response code indicating the module is busy and to repeat the request.
     * @since Version 1.0
     */
    UINT8 m_moduleBusyCode;
    /**
     * Maximum number of reads to perform if we get a response
     * pending negative response from the module.
     * @since Version 1.0
     */
    INT32 m_responsePendingReads;
    /**
     * ID of the module to send the message to.
     * @since Version 1.0
     */
    UINT8 m_moduleRequestID;
    /**
     * Diagnostic request byte that must be added to message
     * before it is sent to the module
     * @since Version 1.0
     */
    UINT8 m_moduleRequestCode;
    /**
     * Diagnostic response byte that must that all messages from
     * module contain.
     * @since Version 1.0
     */
    UINT8 m_moduleResponseCode;
    /**
     * Flag to indicate if diagnostic mode should be automatically
     * entered if the module is not in diagnostic mode.
     * @since Version 1.0
     */
    bool m_autoEnterDiagMode;
    /**
     * Tag for the Enter Diagnostic Mode message
     * @since Version 1.0
     */
    string m_enterDiagnosticModeMessageTag;
};

#endif //J2190ProtocolFilter_h
