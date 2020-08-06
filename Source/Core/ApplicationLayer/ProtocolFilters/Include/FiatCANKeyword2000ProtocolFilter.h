//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/ProtocolFilters/Include/FiatCANKeyword2000ProtocolFilter.h 3     12/06/06 1:29p Derickso $
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
// 
//*************************************************************************
#ifndef FiatCANKeyword2000ProtocolFilter_h
#define FiatCANKeyword2000ProtocolFilter_h

#include "DcxCANKeyword2000ProtocolFilter.h"

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
class FiatCANKeyword2000ProtocolFilter : public DcxCANKeyword2000ProtocolFilter
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
	FiatCANKeyword2000ProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar = NULL, BepMutex *commPortInUse = NULL);
	/**
	 * Class destructor.
	 * @since Version 1.0
	 */
	virtual ~FiatCANKeyword2000ProtocolFilter();

protected:

private:
	virtual void SetModuleRequestID(string);//const UINT16& moduleID);
};

#endif //DcxCANKeyword2000ProtocolFilter_h
