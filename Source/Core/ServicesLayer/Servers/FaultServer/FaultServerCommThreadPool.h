//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/FaultServer/FaultServerCommThreadPool.h 1     12/21/03 6:42p Cward $
//
// FILE DESCRIPTION:
//		This is a thread pool class which handles communication with a FaultServer client.
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
//    $Log: /Core/ServicesLayer/Servers/FaultServer/FaultServerCommThreadPool.h $
// 
// 1     12/21/03 6:42p Cward
// 
// 1     10/07/03 11:32a Khykin
// Initial checkin
// 
// 1     7/08/03 12:58p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:25p Khykin
// Initial checkin
// 
// 1     6/12/03 4:09p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:36  tbochene
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
// 1     2/18/02 11:39a Dan.erickson
// New comm file.
// 
//
//*************************************************************************
#ifndef FaultServerCommThreadPool_h
#define FaultServerCommThreadPool_h

#include "IFaultServer.h"
#include "IBepCommunicationThreadPool.h"

/**
 * Class used by a server to receive Fault messages on a communication
 * channel opened by an IFaultServer object and service clients.
 *
 * @author Dan Erickson
 * @version 1.0
 * @since Version 1.0
 */
class FaultServerCommThreadPool : public IBepCommunicationThreadPool
{
public:
	/**
	 * Class Constructor
	 * @since Version 1.0
	 */
	FaultServerCommThreadPool(BepComponent *comp) : IBepCommunicationThreadPool(comp)
	{
	}
	/**
	 * Class Destructor
	 * @since Version 1.0
	 */
	virtual ~FaultServerCommThreadPool(void)
	{
	}
protected:
	/**
	 * Derived class method for performing the work.
	 *
	 * @return Must return zero.
	 * @since Version 1.0
	 */
	virtual INT32 Handle(ThreadPoolInfo *param)
	{
		try
		{
			ThreadPoolBase::Handle(param);
			if(IsDebugOn())	printf("FaultServerCommThreadPool::Handle(%d)\n", pthread_self());
			std::string message;
			IFaultServer grunt;
			IBepThreadPoolInfo* info = reinterpret_cast<IBepThreadPoolInfo *>(param->data);
			if(info)
			{
				grunt.Initialize(info->comm, info->replyId);
				grunt.HandleRequest(message, m_component);
				pthread_testcancel();
			}
			else throw BepException("Parameter was null in FaultServerCommThreadPool::Handle(%d) done\n", pthread_self());
			if(IsDebugOn())	printf("FaultServerCommThreadPool::Handle(%d) done\n", pthread_self());
		}
		catch(BepException &Err)
		{
			if(IsDebugOn())	printf("FaultServerCommThreadPool::Handle(%d) error: %s\n", pthread_self(), Err.what());
		}
		catch(...)
		{
			if(IsDebugOn())	printf("FaultServerCommThreadPool::Handle(%d) error\n", pthread_self());
		}
		return(0);
	}
};

#endif // FaultServerCommThreadPool_h
