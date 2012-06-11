//*************************************************************************
// Description:
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
//    $Log: /Core/ServicesLayer/Include/WinCcrtInterface.h $
// 
// 1     6/7/12 3:49p Tkelch
// Initial creation and development of interface file
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef KwpWinCcrtProtocolFilter_h
#define KwpWinCcrtProtocolFilter_h

#include "ProtocolFilter.h"

class KwpWinCcrtProtocolFilter : public KwpCanProtocolFilter
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
      * Initialize the serial channel.
      *
      * @param config The configuration to use for the channel initialization.
      * @return true if the initialization is successful.
      */
      virtual bool Initialize(std::string server);
      
      /**
      * Send a message to a vehicle module.
      * NOTE:  The module ID will be prepended to the message
      *        prior to sending the message out of the port.
      *
      * @param message Message to be sent.
      * 
      * @return Status of the send.
      */
      virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag, std::string msgValue);
      
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
      