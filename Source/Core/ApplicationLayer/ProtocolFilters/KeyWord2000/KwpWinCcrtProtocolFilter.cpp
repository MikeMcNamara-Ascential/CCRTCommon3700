//*************************************************************************
// FILE:
//    $Header: Core\ApplicationLayer\ProtocolFilters\KeyWord2000\KwpWinCcrtProtocolFilter.cpp 1     6/7/12 3:49p Tkelch $
//
// FILE DESCRIPTION:
//   Interface class used to communicate with WinCcrtInterface
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
//    $Log: /Core/ServicesLayer/Library/IWinInterfaceServer.cpp $
//
// 1     6/7/12 3:49p Tkelch
// Initial creation and development of interface file
//
//*************************************************************************

#include "KwpWinCcrtProtocolFilter.h"

KwpWinCcrtProtocolFilter::KwpWinCcrtProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar, BepMutex *commPortInUse) : KwpCanProtocolFilter(lastTxTime, stopCommsBepCondVar, commPortInUse)
{
}

KwpWinCcrtProtocolFilter::~KwpWinCcrtProtocolFilter()
{
}

KwpWinCcrtProtocolFilter::Initialize(std::string server)
{
   m_WinCcrtComm.Initialize(server);
}

const BEP_STATUS_TYPE KwpWinCcrtProtocolFilter::SendMessage(std::string messageTag, std::string msgValue)
{	// Add the module ID to the message
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
   
	Log(LOG_DETAILED_DATA, "KwpWinCcrtProtocolFilter::SendMessage - Resetting The Connection\n");
	ResetConnection();
	Log(LOG_DETAILED_DATA, "KwpWinCcrtProtocolFilter::SendMessage - Reset The Connection\n");
	
   // Send the message and return the result
   status = m_WinCcrtComm.Write(messageTag, msgValue, response, true);
   
	// Return the status
	return(status);
}

int KwpWinCcrtProtocolFilter::WaitForFullResponse(SerialString_t &response)
{
	int         retVal = 0;
	// set up the receive timeout
	Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::WaitForFullResponse - Waiting for response from module\n");
	Log(LOG_DEV_DATA, "\t -- Module Response Delay: %d\n", GetResponseDelay());
	Log(LOG_DEV_DATA, "\t -- Response End Delay: %d\n", GetResponseEndDelay());
	
   retVal = m_WinCcrtComm.Read(response, GetResponseDelay(), GetResponseEndDelay());
   
	Log(LOG_DEV_DATA, "Done waiting for response.  %d bytes received\n", retVal);
	return( retVal);
}


