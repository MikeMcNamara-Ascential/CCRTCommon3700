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
#include "IsuzuCommonInfoProtocolFilter.h"

//-------------------------------------------------------------------------------------------------
IsuzuCommonInfoProtocolFilter::IsuzuCommonInfoProtocolFilter(KeepAliveTimer_t &lastTxTime, 
															 StopCommsBepCondVar *stopCommsBepCondVar/* = NULL*/,
															 BepMutex *commPortInUse /*= NULL*/) : 
ProtocolFilter(lastTxTime, stopCommsBepCondVar, commPortInUse) 
{  
	m_msgSerialNumber = 0;
}

//-------------------------------------------------------------------------------------------------
IsuzuCommonInfoProtocolFilter::~IsuzuCommonInfoProtocolFilter()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void IsuzuCommonInfoProtocolFilter::AddChecksumToMessage(SerialString_t &message)
{
	UINT16 checksum = 0x0000;
	for(UINT16 index = 1; index < message.length(); index++)
	{
		CrcCalc(message[index], &checksum);
	}
	// Set the checksum in the message
	message.push_back((checksum & 0xFF00) >> 8);
	message.push_back(checksum & 0x00FF);
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE IsuzuCommonInfoProtocolFilter::CheckForValidResponse(const SerialString_t &moduleResponse)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	if((moduleResponse[0] == STX) && (moduleResponse[moduleResponse.length() - 3] == ETX))
	{
		status = BEP_STATUS_SUCCESS;
	}
	else
	{
		status = BEP_STATUS_FAILURE;
	}
	Log(LOG_DEV_DATA, "Check for valid response: %s", ConvertStatusToResponse(status).c_str());
	return status;
}

//-------------------------------------------------------------------------------------------------
void IsuzuCommonInfoProtocolFilter::CrcCalc(UINT8 dstDat, UINT16 *srcDat)
{
	UINT16 temp = *srcDat ^ dstDat;
	for(UINT8 index = 0; index < 8; index++)
	{
		if(temp & 0x0001)
		{
			temp = (temp >> 1) ^ CRC_POLY;
		}
		else 
		{
			temp >>= 1;
		}
	}
	*srcDat = temp;
}

//-------------------------------------------------------------------------------------------------
const SerialString_t IsuzuCommonInfoProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{
	Log(LOG_FN_ENTRY, "IsuzuCommonInfoProtocolFilter::ExtractModuleData() - Enter");
	string fullResponse;
	SerialString_t responseData;
	char buff[4096];
	for(UINT32 index = 0; index < moduleResponse.length(); index++)
	{
		fullResponse += CreateMessage(buff, sizeof(buff), "$%02X ", moduleResponse[index]);
	}
	Log(LOG_DEV_DATA, "Extracting data from response string: %s", fullResponse.c_str());
	UINT16 dbCount = ((moduleResponse[TotalDataByteIndex] & 0x07) << 8) | moduleResponse[TotalDataByteIndex+1];
	if(moduleResponse.length() >= dbCount)
	{
		m_msgSerialNumber = moduleResponse[MsgSerialNumberIndex];
		UINT16 dataByteCount = (moduleResponse[MsgDataLengthIndex] << 8) | moduleResponse[MsgDataLengthIndex+1];
		Log(LOG_DEV_DATA, "Message Serial Number: %02X, Data Bytes: %d", m_msgSerialNumber, dataByteCount);
//		m_msgSerialNumber++;
		Log(LOG_DEV_DATA, "Incremented message serial number: %02X", m_msgSerialNumber);
		responseData = moduleResponse.substr(DataBodyStartIndex, dataByteCount);
	}
	else
	{
		Log(LOG_ERRORS, "Invalid message received");
	}
	Log(LOG_FN_ENTRY, "IsuzuCommonInfoProtocolFilter::ExtractModuleData() - Exit");
	return responseData;
}

//-------------------------------------------------------------------------------------------------
bool IsuzuCommonInfoProtocolFilter::IsChecksumValid(SerialString_t &moduleResponse)
{
	Log(LOG_FN_ENTRY, "IsuzuCommonInfoProtocolFilter::IsChecksumValid() - Enter");
	UINT16 msgLength = moduleResponse.length();
	UINT16 msgChecksum = (moduleResponse[msgLength - 2] << 8) | moduleResponse[msgLength - 1];
	UINT16 dataLength = ((moduleResponse[TotalDataByteIndex] << 8) | moduleResponse[TotalDataByteIndex+1]) & 0x7FFF;
	UINT16 calcChecksum = 0x0000;
	for(UINT16 index = 1; index < dataLength; index++)
	{
		CrcCalc(moduleResponse[index], &calcChecksum);
	}
	Log(LOG_DEV_DATA, "Calculated Checksum: %04X,  Message Checksum: %04X", calcChecksum, msgChecksum);
	Log(LOG_FN_ENTRY, "IsuzuCommonInfoProtocolFilter::IsChecksumValid() - Exit");
	return calcChecksum == msgChecksum;
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE IsuzuCommonInfoProtocolFilter::SendMessage(SerialString_t &message)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	SerialString_t msg;
	UINT16 dataLen = message.length() - 2;
	/*
	 * Add ifxed data fields to the message length:
	 *  	4 = Device Number Length
	 * 		1 = Message Serial Number
	 *		2 = Message Length
	 *		1 = ETX
	 *		2 = CRC
	 */
	UINT16 totalBytes = message.length() + 4 + 1 + 2 + 1 + 2;
	msg.push_back(STX);
	msg.push_back(COMPRESSION_TYPE);
	msg.push_back(0x71);                                    // 3. Response/Request ID
	msg.push_back((totalBytes & 0xFF00) >> 8);              // 4. Total Data Bytes
	msg.push_back(totalBytes & 0x00FF);
	msg.push_back(0x00);                                    // 5. Device Number
	msg.push_back(0x00);
	msg.push_back(0x00);
	msg.push_back(0x00);
	msg.push_back(message[0]);                              // 6. Message ID
	msg.push_back(message[1]);
	msg.push_back(m_msgSerialNumber);                     // 7. Message Serial Number
	msg.push_back((dataLen & 0xFF00) >> 8);                 // 8. Message Length
	msg.push_back(dataLen & 0x00FF);
	for(UINT32 index = 2; index < message.length(); index++)// 9. Data Body
	{
		msg.push_back(message[index]);
	}
	msg.push_back(ETX);
	AddChecksumToMessage(msg);
	// Clear the Fifos so bad data is not gathered
	Log(LOG_DETAILED_DATA, "Resetting The Connection\n");
	ResetConnection();
	Log(LOG_DETAILED_DATA, "Reset The Connection\n");
	// Send the message and return the result
	PrintSerialString( "Isuzu ProtocolFilter Sending", msg);
	// Write message
	INT32 bytesSent = ILogicalPort::WritePort(msg, GetNumberOfRetries());
	SetLastTxTime();
	Log(LOG_DETAILED_DATA, "Sent the message\n");
	m_msgSerialNumber++;
	// Determine the result of the write
	if(bytesSent < 0)		status = BEP_STATUS_ERROR;
	else if(bytesSent == 0)	status = BEP_STATUS_FAILURE;
	else if(bytesSent > 0)	status = BEP_STATUS_SUCCESS;

	return status;
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE IsuzuCommonInfoProtocolFilter::SendMessage(string messageTag)
{
	return ProtocolFilter::SendMessage(messageTag);
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE IsuzuCommonInfoProtocolFilter::SendMessage(string messageTag, SerialArgs_t &args)
{
	return ProtocolFilter::SendMessage(messageTag, args);
}
