//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  The Tesla Data Message class handles setting data in the particular Tesla
//  message it is configured to handle.
//
//===========================================================================
// Copyright (c) 2017- Burke E. Porter Machinery
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
#include "TeslaDataMessage.h"

//-------------------------------------------------------------------------------------------------
TeslaDataMessage::TeslaDataMessage(const INT16 numDataItems, const UINT8 msgType, const UINT8 msgSubType /*= 0x00*/)
{
	m_msgData.reserve(numDataItems);
	m_msgData.resize(numDataItems);
	m_msgType = msgType;
	m_msgSubType = msgSubType;
}

//-------------------------------------------------------------------------------------------------
TeslaDataMessage::~TeslaDataMessage()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void TeslaDataMessage::UpdateMessageData(const UINT8 &byteLocation, const INT16 &data)
{
	m_msgData[byteLocation] = data;
}

//-------------------------------------------------------------------------------------------------
void TeslaDataMessage::UpdateMessageData(const UINT8 &startingLocation, const float data[], const INT16 &dataCount)
{
	for(UINT8 index = startingLocation; index < (startingLocation + dataCount); index++)
	{
		m_msgData[index] = (UINT16)(data[index - startingLocation] * 10.0);
	}
}

//-------------------------------------------------------------------------------------------------
SerialString_t TeslaDataMessage::GetMessage()
{
	UINT8 chkSum = STX + m_msgType + ETX;
	SerialString_t msg;
	msg.push_back(STX);
	msg.push_back(m_msgType);
	if(m_msgSubType != 0x00)
	{
		msg.push_back(m_msgSubType);
		chkSum += m_msgSubType;
	}
	for(UINT8 index = 0; index < m_msgData.size(); index++)
	{
		UINT8 hiByte = (m_msgData[index] & 0xFF00) >> 8;
		UINT8 loByte = m_msgData[index] & 0x00FF;
		msg.push_back(hiByte);
		msg.push_back(loByte);
		chkSum += (hiByte + loByte);
	}
	msg.push_back(ETX);
	msg.push_back(chkSum);

	return msg;
}
