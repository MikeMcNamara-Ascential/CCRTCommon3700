//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/IWinInterfaceServer.cpp 1     6/7/12 3:49p Tkelch $
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

#include "IWinInterfaceServer.h"

IWinInterfaceServer::IWinInterfaceServer(void) : IBepCommunication()
{
   
}
IWinInterfaceServer::~IWinInterfaceServer(void)
{
}


const BEP_STATUS_TYPE IWinInterfaceServer::SendMessage(std::string messageTag)
{	// Send the message and return the result
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
   
   // Send the string to the WinCcrtInterface
   // convert to <uint_8> vector?
   if(strlen(messageTag) == ILogicalPort::WritePort(messageTag, strlen(messageTag), GetNumberOfRetries()))
   {
      Log(LOG_DEV_DATA, "IWinInterfaceServer::SendMessage - Successfully sent: %s\n");
      status = BEP_STATUS_SUCCESS;
   }
   else
   {
      Log(BEP_STATUS_ERROR, "IWinInterfaceServer::SendMessage - Failed to send: %s\n");
   }
   
	// Return the status
	return(status);
}

int IWinInterfaceServer::WaitForFullResponse(SerialString_t &response)
{
	int         retVal = 0;
	// set up the receive timeout
	ComMssgTableEntry msgAttr;
	Log(LOG_DEV_DATA, "IWinInterfaceServer::WaitForFullResponse Waiting for response from module\n");
	Log(LOG_DEV_DATA, "\t -- Module Response Delay: %d\n", GetResponseDelay());
	Log(LOG_DEV_DATA, "\t -- Response End Delay: %d\n", GetResponseEndDelay());
	retVal = ReadPort( response, GetResponseDelay(), GetResponseEndDelay());
	Log(LOG_DEV_DATA, "Done waiting for response.  %d bytes received\n", retVal);
	return( retVal);
}