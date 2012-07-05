// Description:
//  Serial ProtocolFilter for communicating with vehicle modules.
//  This protocol filter just sends the raw data and expects raw data back.
//  No Checksum is required and there is no data header required.
//
//==============================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#include "SerialProtocolFilter.h"

//-------------------------------------------------------------------------------------------------
SerialProtocolFilter::SerialProtocolFilter(KeepAliveTimer_t &lastTxTime, 
										   StopCommsBepCondVar *stopCommsBepCondVar /*= NULL*/, 
										   BepMutex *commsInUse /*= NULL*/) :
ProtocolFilter(lastTxTime, stopCommsBepCondVar, commsInUse)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
SerialProtocolFilter::~SerialProtocolFilter()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE SerialProtocolFilter::CheckForValidResponse(const SerialString_t &moduleResponse)
{   // If the response is at least one byte, then it is a valid response
	Log(LOG_DEV_DATA, "Response Valid: %s", moduleResponse.length() > 0 ? "True" : "False");
	return (moduleResponse.length() > 0) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
}

//-------------------------------------------------------------------------------------------------
const SerialString_t SerialProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{   // There is no header or checksum to strip off, jut return the modue response
	PrintSerialString("Module Response", moduleResponse);
	return moduleResponse;
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE SerialProtocolFilter::SendMessage(SerialString_t &message)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	Log(LOG_FN_ENTRY, "SerialProtocolFilter::SendMessage() - Enter");
	// Clear the FIFO so bad data is not gathered
	Log(LOG_DETAILED_DATA, "Resetting the connection");
	ResetConnection();
	Log(LOG_DETAILED_DATA, "Connection reset complete");
	INT32 bytesSent = ILogicalPort::WritePort(message, GetNumberOfRetries());
	SetLastTxTime();
	Log(LOG_DETAILED_DATA, "Sent the message");
	// Determine the result of the write
	if(bytesSent < 0)        status = BEP_STATUS_ERROR;
	else if(bytesSent == 0)  status = BEP_STATUS_FAILURE;
	else if(bytesSent > 0)   status = BEP_STATUS_SUCCESS;
	// Return the status
	Log(LOG_FN_ENTRY, "SerialProtocolFilter::SendMessage() - Exit");
	return status;
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE SerialProtocolFilter::SendMessage(string messageTag)
{
	return ProtocolFilter::SendMessage(messageTag);
}

//-------------------------------------------------------------------------------------------------
const BEP_STATUS_TYPE SerialProtocolFilter::SendMessage(string messageTag, SerialArgs_t &args)
{
	return ProtocolFilter::SendMessage(messageTag, args);
}
