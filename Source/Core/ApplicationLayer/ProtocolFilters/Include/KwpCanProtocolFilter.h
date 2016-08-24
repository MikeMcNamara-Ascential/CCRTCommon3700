//*************************************************************************
// FILE:
//    $Header: /Nissan/Source/Nissan/ApplicationLayer/ProtocolFilters/Include/KwpCanProtocolFilter.h 3     1/09/08 1:23p Mmcnamar $
//
// FILE DESCRIPTION:
//	Keyword 2000 ProtocolFilter customized for Bosch module in Nissan
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
//    $Log: /Nissan/Source/Nissan/ApplicationLayer/ProtocolFilters/Include/KwpCanProtocolFilter.h $
// 
// 3     1/09/08 1:23p Mmcnamar
// Updated for CAN ABS: Added prototypes for SendMessage( ).
// 
// 2     5/29/07 8:10a Gswope
// fixed bug in constructor
// 
// 1     2/09/07 1:50p Gswope
// Initial entry
// 
// 1     2/23/05 8:26a Gswope
// new location, code as of 1/1/2005
// 
// 1     11/22/04 11:09a Gswope
// Updates for sensor calibration.
//    Revision 1.1  2004/04/10 20:54:54  bereza
//    code complete. config complete. all uncompiled yet
//
// 
//*************************************************************************
#ifndef KwpCanProtocolFilter_h
#define KwpCanProtocolFilter_h

#include "ProtocolFilter.h"

/**
 * KwpBosch Protocol 2000 filter.
 * This class will send and receive KwpBosch 2000 messages.
 * The ExtractModuleData() method has been instantiated to strip the header and
 * checksum from the module response.
 * 
 * @author Dan Erickson
 * @author Ross Wiersma
 * @version Version 1.0
 * @see ProtocolFilter
 * @since December 17, 2002
 */

class KwpCanProtocolFilter : public ProtocolFilter
{
    public:
    /**
     * Class constructor.
	 *
     * @param lastTxTime Variable used to hold the time of the last transmit operation. This is
     *                   designed to be a shared timestamp that all the protocol filters for
     *                   this module interface use so the module keep alive knows how long
     *                   it has been since the last string was sent to the module so as not to
     *                   send unnecessary keep alive messages
     *
	 * @param commsInUse Semaphore to allow only one thread in the process to utilize the comm port at a time.
     */
	KwpCanProtocolFilter(KeepAliveTimer_t &lastTxTime, 
                         StopCommsBepCondVar *stopCommsBepCondVar, 
                         BepMutex *commPortInUse = NULL);
    /**
     * Class destructor.
     */
    virtual ~KwpCanProtocolFilter();
    /**
	 * Initialize the serial channel.
     *
	 * @param config The configuration to use for the channel initialization.
	 * @return true if the initialization is successful.
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
     */
    virtual const BEP_STATUS_TYPE SendMessage(SerialString_t &message);
    /**
     * Send a message to vehicle module.
     *
     * @param messageTag Name of message to be retrieved and sent.
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag);
    /**
     * Send a message to a vehicle module.
     *
     * @param messageTag Name of the message to be sent.
     * @param args       Extra data to be added to the message.
     * @return Status of the opeation.
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag, SerialArgs_t &args);
	/**
	 * Send a message to the module and read the response.
	 * 
	 * @param messageTag Tag of message to send.
	 * @param reply      Reply from the module.
	 * @param args       Optional data to be written to the module.
	 * @return Status of the operation.
     */
	virtual const BEP_STATUS_TYPE GetModuleData(std::string messageTag, SerialString_t &reply, SerialArgs_t *args = NULL);
	virtual const BEP_STATUS_TYPE GetModuleDataUUDTResponse(std::string messageTag, SerialString_t &reply, SerialArgs_t *args = NULL);
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
     */
	const SerialString_t ExtractModuleData(SerialString_t &moduleResponse);
	/**
	 * Get the negative response code indicating the module is not in diagnostic mode.
	 * 
	 * @return Negative response code for module not in diagnostic mode
     */
	const UINT8& GetEnterDiagnosticModeCode(void);
	/**
	 * Get the negative response code indicating the response is pending
	 * 
	 * @return Negative response code for response pending
     */
	const UINT8& GetResponsePendingCode(void);
    /**
	 * Get the negative response code indicating the module is busy and to repeat the request
	 * 
	 * @return Negative response code for module busy, repeat request
     */
	const UINT8& GetModuleBusyCode(void);
	/**
	 * Get the tag for the message to place the module into diagnostic mode
	 * 
	 * @return Tag for the Enter Diagnostic Mode message
	 */
	const string& GetEnterDiagnosticModeMessageTag(void);
	/**
	 * Get the maximum number of additional reads to perform
	 * if the module responds with a response pending result.
	 * 
	 * @return Number of additional reads to perform.
	 */
	const INT32& GetNumberOfResponsePendingReads(void);
	/**
	 * Get the flag indicating if diagnostic mode should be
	 * entered automatically if the module is not in diagnostic
	 * mode.
	 * 
	 * @return True - automatically enter diagnostic mode.
	 */
	const bool& AutomaticallyEnterDiagnosticMode(void);
	/**
	 * Get the ID to use for sending message to the module.
	 * 
	 * @return Request ID of the module.
	 */
	const SerialString_t GetModuleRequestID(void);
	/**
	 * Get the index to the data byte count in the message.
	 * 
	 * @return Data byte count index.
	 */
	const INT32& GetDataByteCountIndex(void);

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
	virtual int WaitForFullResponseUUDT( SerialString_t &response);
    virtual const BEP_STATUS_TYPE GetResponseUUDT(SerialString_t &reply);

private:
	/**
	 * Set the negative response code indicating the module is not in diagnostic mode.
	 * 
	 * @param  enterDiagnosticModeCode Negative response code for module not in diagnostic mode
	 */
	void SetEnterDiagnosticModeCode(const UINT8 &enterDiagnosticModeCode);
	/**
	 * Set the negative response code indicating the response is pending.
	 * 
	 * @param  responsePendingCode Negative response code for response pending
	 */
	void SetResponsePendingCode(const UINT8 &responsePendingCode);
	/**
	 * Set the negative response code indicating the module is busy and to repeat the request.
	 * 
	 * @param  moduleBusyCode Negative response code for module busy, repeat request
	 */
	void SetModuleBusyCode(const UINT8 &moduleBusyCode);
	/**
	 * Set the tag for the Enter Diagnostic Mode message
	 * 
	 * @param  tag Tag for the Enter Diagnostic Mode message
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
	 */
	void SetAutomaticallyEnterDiagnosticMode(const bool& autoEnterDiagMode);
	/**
	 * Store the maximum number of reads to perform if the
	 * module responds with a response pending result.
	 * 
	 * @param responsePendingReads
	 *               Number of reads to perform.
	 */
	void SetResponsePendingReads(const INT32& responsePendingReads);
	/**
	 * Store the module id for requests.
	 * 
	 * @param moduleID ID to use for sending messages to the module.
	 */
	void SetModuleRequestID(const UINT32& moduleID);
	/**
	 * Store the index to the data byte count in the message.
	 * 
	 * @param dataByteCountIndex
	 *               Index to the data byte count.
	 */
	void SetDataByteCountIndex(const INT32& dataByteCountIndex);
	/**
	 * Negative response code indicating the module is not in Diagnostic mode.
	 */
	UINT8 m_enterDiagModeCode;
	/**
	 * Negative response code indicating the response is pending.
	 */
	UINT8 m_responsePendingCode;
	/**
	 * Negative response code indicating the module is busy and to repeat the request.
	 */
	UINT8 m_moduleBusyCode;
	/**
	 * Maximum number of reads to perform if we get a response
	 * pending negative response from the module.
	 */
	INT32 m_responsePendingReads;
	/**
	 * Index to the number of data bytes returned by the module.
	 */
	INT32 m_dataByteCountIndex;
	/**
	 * ID of the module to send the message to.
	 */
	UINT32 m_moduleRequestID;
	/**
	 * Flag to indicate if diagnostic mode should be automatically
	 * entered if the module is not in diagnostic mode.
	 */
	bool m_autoEnterDiagMode;
	/**
	 * Tag for the Enter Diagnostic Mode message
	 */
	string m_enterDiagnosticModeMessageTag;
};
#endif //KwpCanProtocolFilter_h
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
