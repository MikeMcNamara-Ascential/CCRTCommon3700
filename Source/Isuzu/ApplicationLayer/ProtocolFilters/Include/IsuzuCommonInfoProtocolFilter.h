//*************************************************************************
// FILE DESCRIPTION:
//	Protocol Filter for handling the Isuzu On-Board Unit Common Information
//  Interface Protocol.
//
//===========================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef IsuzuCommonInfoProtocolFilter_h
#define IsuzuCommonInfoProtocolFilter_h
//-------------------------------------------------------------------------------------------------
#include "ProtocolFilter.h"
//-------------------------------------------------------------------------------------------------
const UINT8  COMPRESSION_TYPE = 0x00;
const UINT16 CRC_POLY         = 0xA001;
const UINT32 DEVICE_NUMBER    = 0x00000000;
const UINT8  ETX              = 0x03;
const UINT8  STX              = 0x02;

const UINT8  DataBodyStartIndex     = 14;
const UINT8  MessageIdIndex         = 9;
const UINT8  MsgDataLengthIndex     = 12;
const UINT8  MsgSerialNumberIndex   = 11;
const UINT8  RequestResponseIdIndex = 2;
const UINT8  TotalDataByteIndex     = 3;

/**
 * Message format
 * --------------------------------------------------------
	No.	Item						Byte	Description
 * --------------------------------------------------------
	1	STX							1		Fixed Value?02H
	2	Compression Type			1		Data Compression ON/OFF(Fixed value for this system:00H)
	3	Request from/to (Device)	1		Upper 4bit: Device Number of Requestee
											Lower 4bit: Device Number of Requestor
												MIMAMORI ECU => On-Board Unit: 17H
												On-Board Unit => MIMAMORI ECU: 71H
	4	Total Data Byte				2		Data byte of Item No. 5 - 11
											Uppermost bit is response required flag (0:No Response, 1:Found Response)
											Upper 2nd bit: Unused (Fix 0)
	5	Device Number				4		Fixed Value:00000000H
	6	Message ID					2		See "Message ID List"
	7	Serial Number				1		Serial number when message is transmitted.
											Except for resending, the value should be incremented 
	8	Message Length				2		Number of bytes in Item No. 9
	9	Data Body					n		This should be less than 4096 bytes data
											MIMAMORI ECU => On-Board Unit: Data
											On-Board Unit=> MIMAMORI ECU: Status (1 byte) + Data Body 
	10	ETX							1		Fixed Value?03H
	11	CRC							2		Cyclic Redundancy Checksum (CRC16)
											With subjects ranging from Item No.2 to 10
 * --------------------------------------------------------
*/

class IsuzuCommonInfoProtocolFilter : public ProtocolFilter
{
public:
	IsuzuCommonInfoProtocolFilter(KeepAliveTimer_t &lastTxTime, 
								  StopCommsBepCondVar *stopCommsBepCondVar = NULL,
								  BepMutex *commPortInUse = NULL);

	virtual ~IsuzuCommonInfoProtocolFilter();

	/**
	 * Add the checksum to the message prior to transmitting the message to the module.
	 *
	 * @param message Message to be sent.
	 */
	virtual void AddChecksumToMessage(SerialString_t &message);

	/**
	 * Extract the data from the response.
	 *
	 * @param moduleResponse
	 *               Response from the module.
	 * @return The data that was included in the response.
	 */
	virtual const SerialString_t ExtractModuleData(SerialString_t &moduleResponse);

	/**
	 * Check if the message received from the module is valid.
	 *
	 * @param moduleResponse
	 *               Response received from the module
	 * @return Status of the operation.
	 */
    virtual const BEP_STATUS_TYPE CheckForValidResponse(const SerialString_t &moduleResponse);

	/**
	 * Check to ensure that checksum on module response
	 * is valid.  No actual checksum validation done in
	 * base class implementation.  If required by
	 * specific protocol filter, it should be overloaded.
	 *
	 * @param moduleResponse
	 *               entire module response from module
	 *
	 * @return True if the message checksum is valid.
	 */
	virtual bool IsChecksumValid(SerialString_t &moduleResponse);

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
     * @since Version 1.0
     */

    virtual const BEP_STATUS_TYPE GetResponse(const std::string &messageTag, SerialString_t &reply);

    virtual const BEP_STATUS_TYPE GetModuleData(std::string messageTag, SerialString_t &reply, SerialArgs_t *args = NULL);
    //virtual const BEP_STATUS_TYPE GetModuleData(std::string messageTag, SerialString_t &reply, SerialArgs_t *args);




private:

	/**
	 * Calculate the CRC addition number.
	 * 
	 * @param dstDat  CRC Addition Number
	 * @param srcDat  CRC Non Addition Number
	 */
	void CrcCalc(UINT8 dstDat, UINT16 *srcDat);

	UINT8 m_msgSerialNumber;
};
//-------------------------------------------------------------------------------------------------
#endif //IsuzuCommonInfoProtocolFilter_h
