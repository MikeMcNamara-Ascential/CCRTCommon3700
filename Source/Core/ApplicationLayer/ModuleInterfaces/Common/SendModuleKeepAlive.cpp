//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Common/SendModuleKeepAlive.cpp 3     10/25/06 2:58p Rwiersem $
//
// FILE DESCRIPTION:
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Common/SendModuleKeepAlive.cpp $
// 
// 3     10/25/06 2:58p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a timer to tell when the last transmit message was sent.
// This
//       allows the keep alive message to be sent when needed instead of
// on a
//       specific time interval.
// 
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 2     10/20/03 9:31a Derickso
// Added logic to prevent thread from being deleted while the
// communication port is locked.
// Added a flag that the instantiateing object can use to determine if the
// thread has completed and exited.  These threads typically self-delete
// when commanded to exit.
//
// 1     8/27/03 7:51a Derickso
// Renamed back to SendModuleKeepAlive.
//
// 2     8/08/03 4:50p Khykin
// Updated with changes from Flatrock.
//
// 1     8/08/03 2:24p Khykin
// Initial checkin.
//    Revision 1.1  2003/05/27 13:29:15  tenharmsel
//    Moving customer specific module interfaces into a Customers directory
//
//    Revision 1.1.1.1  2003/05/07 15:11:59  tenharmsel
//    Bringing in the GenericModuleInterfaces
//
//    Revision 1.1  2003/02/26 20:55:04  bereza
//    added SendModuleKeepAlive class and methods to GenericModule
//
//
// 1     1/30/03 8:23a Derickso
// Created.
//
//*************************************************************************

#include "SendModuleKeepAlive.h"
#include "GeneralUtilities.h"

SendModuleKeepAlive::SendModuleKeepAlive(IProtocolFilter *vehicleComm,
                                         const std::string keepAliveMessageTag,
                                         const INT32 keepAliveTime,
                                         const bool responseRequired) :
    BackgroundMonitor(vehicleComm, keepAliveTime), 
    m_keepAliveMessageTag(keepAliveMessageTag),
    m_responseRequired(responseRequired), 
    m_keepAliveTime( keepAliveTime)
{   // Nothing special to do here
}

SendModuleKeepAlive::~SendModuleKeepAlive(void)
{   // Nothing special to be done
}

BEP_STATUS_TYPE SendModuleKeepAlive::MonitorData(void)
{
    return SendKeepAlive(GetKeepAliveMessageTag(), IsResponseRequired());
}

const BEP_STATUS_TYPE SendModuleKeepAlive::SendKeepAlive(const std::string &keepAliveMessageTag,
                                                         const bool &keepAliveResponseRequired)
{
    uint64_t        currTime, lastTxTime, timeBeforeSend;
    const uint64_t  gapTime = mSEC_nSEC( m_keepAliveTime);
    SerialString_t  reply;
    ProtocolFilter  *protocolFilter = NULL;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(m_vehicleComm != NULL) 
	{   // Try to convert the vehicle comm interface to a ProtocolFilterObject
		try
		{
			protocolFilter = dynamic_cast<ProtocolFilter*>(m_vehicleComm);
		}
		catch( ...)
		{
			protocolFilter = NULL;
		}
    
		// Sample the current time
		ClockTime( CLOCK_REALTIME, NULL, &currTime);
		// If we are dealing with a valid ProtocolFilter (or derived) object
		if( NULL != protocolFilter)
		{
			// Get the time of the last transmit to this module
			lastTxTime = protocolFilter->GetLastTxTime();
			// Calculate when the next message needs to be sent
			timeBeforeSend = lastTxTime + gapTime;
		}
		else
		{
			// Assume OK to send if we can't get the time of last transmit
			timeBeforeSend = currTime;
		}

		// IF the message should be sent now (or sooner)
		if( timeBeforeSend <= currTime)
		{
			if (keepAliveResponseRequired)
			{
				status = m_vehicleComm->GetModuleData(keepAliveMessageTag, reply);
			}
			else
			{
				status = m_vehicleComm->SendMessage(keepAliveMessageTag);
			}

			// Reset message gap timer to default keep alive time
			SetInterMessageGap( mSEC_nSEC(m_keepAliveTime));
		}
		else
		{
			// calculate time remaining before keep alive should be sent
			timeBeforeSend -= currTime;
			// Reset message gap timer to default keep alive time
			SetInterMessageGap( nSEC_mSEC( timeBeforeSend));
			status = BEP_STATUS_SUCCESS;
		}
	}
    
    // Return the status
    return status;
}

inline const std::string& SendModuleKeepAlive::GetKeepAliveMessageTag(void)
{
    return m_keepAliveMessageTag;
}

inline const bool& SendModuleKeepAlive::IsResponseRequired(void)
{
    return m_responseRequired;
}
