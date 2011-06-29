//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/ITestResultThreadPool.h 2     1/16/04 4:45p Cward $
//
// FILE DESCRIPTION:
//		This is a thread pool class which handles communication with clients of the Test Result server.
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
//    $Log: /Core/ServicesLayer/Include/ITestResultThreadPool.h $
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
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
// 1     6/12/03 4:15p Khykin
// Initial checkin.
//
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:40  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:06a Khykin
// Initial Checkin
//
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
//
// 2     6/18/02 12:52p Dan.erickson
// Updated call to HandleRequest().
//
// 1     2/18/02 3:40p Skypig13
// New thread pool for test result server
//
//*************************************************************************
#ifndef ITestResultServerThreadPool_h
#define ITestResultServerThreadPool_h

//#include "ITestResultServer.h"
#include "IBepCommunicationThreadPool.h"

/**
 * Class used by a server to receive messages on a communication
 * channel opened by an IBepCommunication object and service test
 * result server clients.
 *
 * @author Tim Bochenek
 * @version 1.0
 * @since Version 1.0
 */
class ITestResultServerThreadPool : public IBepCommunicationThreadPool
{
public:
	/**
	 * Class Constructor
	 * @since Version 1.0
	 */
	ITestResultServerThreadPool(BepComponent *comp);
	
    /**
	 * Class Destructor
	 * @since Version 1.0
	 */
	virtual ~ITestResultServerThreadPool(void);

protected:
	/**
	 * Derived class method for performing the work.
	 *
	 * @return Must return zero.
	 * @since Version 1.0
	 */
	virtual INT32 Handle(ThreadPoolInfo* param);

};

#endif // ITestResultServerThreadPool_h
