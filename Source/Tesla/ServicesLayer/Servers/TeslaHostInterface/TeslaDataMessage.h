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
#ifndef TeslaDataMessage_h
#define TeslaDataMessage_h
//-------------------------------------------------------------------------------------------------
#include <vector>
#include "TeslaHostInterface.h"

class TeslaHostInterface;


//-------------------------------------------------------------------------------------------------
class TeslaDataMessage
{
public:
	TeslaDataMessage(const INT16 numDataItems, const UINT8 msgType, const UINT8 msgSubType = 0x00);
	virtual ~TeslaDataMessage();

	void UpdateMessageData(const UINT8 &byteLocation, const INT16 &data);
	void UpdateMessageData(const UINT8 &startingLocation, const float data[], const INT16 &dataCount);

	SerialString_t GetMessage();



private:

	UINT8 m_msgType;
	UINT8 m_msgSubType;

	vector<INT16> m_msgData;

};

//-------------------------------------------------------------------------------------------------
#endif //TeslaDataMessage_h
