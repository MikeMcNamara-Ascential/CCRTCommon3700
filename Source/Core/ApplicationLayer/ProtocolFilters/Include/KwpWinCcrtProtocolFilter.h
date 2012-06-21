//*************************************************************************
// FILE:
//    $Header: /Nissan/Source/Nissan/ApplicationLayer/ProtocolFilters/Include/KwpWinCcrtProtocolFilter.h 3     1/09/08 1:23p Mmcnamar $
//
// FILE DESCRIPTION:
//	Keyword 2000 ProtocolFilter customized for Bosch module in Nissan
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
//    $Log: /Nissan/Source/Nissan/ApplicationLayer/ProtocolFilters/Include/KwpWinCcrtProtocolFilter.h $
// 
// 3     1/09/08 1:23p Mmcnamar
// Updated for CAN ABS: Added prototypes for SendMessage( ).
// 
// 2     5/29/07 8:10a Gswope
// fixed bug in constructor
// 
// 1     2/09/07 1:50p Gswope
// Initial entry
// 
// 1     2/23/05 8:26a Gswope
// new location, code as of 1/1/2005
// 
// 1     11/22/04 11:09a Gswope
// Updates for sensor calibration.
//    Revision 1.1  2004/04/10 20:54:54  bereza
//    code complete. config complete. all uncompiled yet
//
// 
//*************************************************************************
#ifndef KwpWinCcrtProtocolFilter_h
#define KwpWinCcrtProtocolFilter_h

#include "ProtocolFilter.h"
#include "IBepCommunication.h"

/**
 * KwpBosch Protocol 2000 filter.
 * This class will send and receive KwpBosch 2000 messages.
 * The ExtractModuleData() method has been instantiated to strip the header and
 * checksum from the module response.
 * 
 * @author Dan Erickson
 * @author Ross Wiersma
 * @version Version 1.0
 * @see ProtocolFilter
 * @since December 17, 2002
 */

class KwpWinCcrtProtocolFilter : public ProtocolFilter
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
     */
	KwpWinCcrtProtocolFilter(KeepAliveTimer_t &lastTxTime, 
                         StopCommsBepCondVar *stopCommsBepCondVar, 
                         BepMutex *commPortInUse = NULL);
    /**
     * Class destructor.
     */
    virtual ~KwpWinCcrtProtocolFilter();

     /**
     * Initialize the server channel.
     *
     * @param config The configuration to use for the channel initialization.
     * @return true if the initialization is successful.
     */
     virtual bool InitializeServer(std::string server);
      
    /**
     * Send a message to vehicle module.
     *
     * @param messageTag Name of message to be retrieved and sent.
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag);
 
	/**
	 * Extract the data from the response.
	 *
	 * @param moduleResponse
	 *               Response from the module.
	 * @return The data that was included in the response.
     */
	const SerialString_t ExtractModuleData(SerialString_t &moduleResponse);


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
	virtual int WaitForFullResponse(SerialString_t &response);


private:
	
   IBepCommunication m_WinCcrtComm;
   
 
};
#endif //KwpWinCcrtProtocolFilter_h
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
