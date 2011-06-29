//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/IActiveClientCommunicationThreadPool.h 2     1/16/04 4:45p Cward $
//
// FILE DESCRIPTION:
//		This is a thread pool class which handles communication with an active client.
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Include/IActiveClientCommunicationThreadPool.h $
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:10p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:37  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:04a Khykin
// Initial Checkin
//
// 1     1/18/03 8:02a Khykin
// Initial Checkin From DCTC
//
// 7     4/25/02 4:40p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//    started
//
//
// 5     4/02/02 11:04p Skypig13
// Updates for new data server
//
// 4     2/25/02 8:50p Skypig13
// Put diagnostics in constructor and destructor
//
// 3     2/13/02 12:55p Skypig13
// Updated Handle()
//
// 2     2/07/02 6:31p Skypig13
// Moved ThreadPoolBase::Handle() to first line of Handle()
//
// 1     2/05/02 12:33a Skypig13
// Classes needed to communicate with an active client (streaming reader
// or subscriber on a TCP/IP socket)
//
//*************************************************************************
#ifndef IActiveClientCommunicationThreadPool_h
#define IActiveClientCommunicationThreadPool_h

#include "IBepCommunicationThreadPool.h"
#include "BepComponent.h"
#include "IActiveClientCommunication.h"

/**
 * Class used by a server to receive messages on a communication
 * channel opened by an IBepCommunication object and service active clients.
 * An active client is one that requests streamimg data or has and active connection
 * like a TCP/IP socket requesting subscription data
 *
 * @author Tim Bochenek
 * @version 1.0
 * @since Version 1.0
 */
class IActiveClientCommunicationThreadPool : public IBepCommunicationThreadPool
{
public:
	/**
	 * Class Constructor
	 * @since Version 1.0
	 */
	IActiveClientCommunicationThreadPool(BepComponent *comp);
	
    /**
	 * Class Destructor
	 * @since Version 1.0
	 */
	virtual ~IActiveClientCommunicationThreadPool(void);
    void RemoveActiveComms(void);

protected:
	/**
	 * Derived class method for performing the work.
	 *
	 * @return Must return zero.
	 * @since Version 1.0
	 */
	virtual INT32 Handle(ThreadPoolInfo* param);

	/**
	 * Stored reference to client comms
	 *
	 */
    vector <IActiveClientCommunication *> m_clientComms;

};

#endif // IActiveClientCommunicationThreadPool_h
