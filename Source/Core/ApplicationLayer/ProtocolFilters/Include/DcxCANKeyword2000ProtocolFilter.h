//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/ProtocolFilters/Include/DcxCANKeyword2000ProtocolFilter.h 3     12/06/06 1:29p Derickso $
//
// FILE DESCRIPTION:
//	Protocol Filter for handling Keyword 2000 Protocol as implemented by DaimlerChrysler
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
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/ProtocolFilters/Include/DcxCANKeyword2000ProtocolFilter.h $
// 
// 3     12/06/06 1:29p Derickso
// Moved typedef to base class and added SetLastTxTime to Send function.
// 
// 2     11/14/06 3:38p Bbarrett
// Latest core components do not die.
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 1     12/01/04 2:35p Derickso
// Created new VSS Database.
// 
// 1     6/09/04 8:44a Derickso
// Dcx Keyword Protocol 2000 on CAN bus protocol filter.
// 
// 1     1/27/04 9:31a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 6     1/20/04 6:41a Derickso
// Changed Semaphore to BepSemaphore.
// 
// 5     12/16/03 12:40p Derickso
// Added handling for module busy negative response code.
// 
// 4     11/29/03 1:26p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
// 
// 1     10/24/03 3:18p Canyanwu
// Start of DCX working directory
// 
// 3     10/20/03 9:40a Derickso
// Added accessor and settor methods for data byte index.
// Added loggin level masks to Log statements.
// 
// 2     9/22/03 5:30p Derickso
// Added SensMessage methods to correct comile errors.
// Updated to add module request ID to each message prior
// to sending the message.
// 
// 1     8/25/03 1:52p Derickso
// Created.  Compiles, not tested.
// 
//*************************************************************************
#ifndef DcxCANKeyword2000ProtocolFilter_h
#define DcxCANKeyword2000ProtocolFilter_h

#include "ProtocolFilter.h"

/**
 * DaimlerChrysler Keyword Protocol 2000 filter as used on the CAN bus.
 * This class will send and receive Keyword 2000 messages.
 * The ExtractModuleData() method has been instantiated to strip the header and
 * checksum from the module response.
 * 
 * @author Dan Erickson
 * @version Version 1.0
 * @see ProtocolFilter
 * @since August 23, 2003
 */
class DcxCANKeyword2000ProtocolFilter : public ProtocolFilter
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
	 * @since Version 1.0
	 */
	DcxCANKeyword2000ProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar = NULL, BepMutex *commPortInUse = NULL);
	/**
	 * Class destructor.
	 * @since Version 1.0
	 */
	virtual ~DcxCANKeyword2000ProtocolFilter();
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
	 * Get the ID to use for sending message to the module.
	 * 
	 * @return Request ID of the module.
	 * @since Version 1.0
	 */
	const SerialString_t GetModuleRequestID(void);
	/**
	 * Get the index to the data byte count in the message.
	 * 
	 * @return Data byte count index.
	 * @since Version 1.0
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
	 * Store the module id for requests.
	 * 
	 * @param moduleID ID to use for sending messages to the module.
	 * @since Version 1.0
	 */
	void SetModuleRequestID(const UINT16& moduleID);
	/**
	 * Store the index to the data byte count in the message.
	 * 
	 * @param dataByteCountIndex
	 *               Index to the data byte count.
	 * @since Version 1.0
	 */
	void SetDataByteCountIndex(const INT32& dataByteCountIndex);
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
	 * Index to the number of data bytes returned by the module.
	 * @since Version 1.0
	 */
	INT32 m_dataByteCountIndex;
	/**
	 * ID of the module to send the message to.
	 * @since Version 1.0
	 */
	UINT16 m_moduleRequestID;
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

#endif //DcxCANKeyword2000ProtocolFilter_h
