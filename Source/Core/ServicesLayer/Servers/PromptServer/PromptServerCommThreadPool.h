//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/PromptServer/PromptServerCommThreadPool.h 1     12/21/03 6:42p Cward $
//
// FILE DESCRIPTION:
//		This is a thread pool class which handles communication with a PromptServer client.
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
//    $Log: /Core/ServicesLayer/Servers/PromptServer/PromptServerCommThreadPool.h $
// 
// 1     12/21/03 6:42p Cward
// 
// 2     10/24/03 1:47p Khykin
// Updated with latest changes from Korea and Flatrock.
// 
// 1     10/07/03 11:29a Khykin
// Initial checkin
// 
// 1     7/08/03 1:00p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:26p Khykin
// Initial checkin
// 
// 1     6/12/03 4:20p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:45  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:09a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:06a Khykin
// Initial Checkin From DCTC
// 
// 1     2/18/02 3:42p Dan.erickson
// Created.
// 
//
//*************************************************************************
#ifndef PromptServerCommThreadPool_h
#define PromptServerCommThreadPool_h

#include "IPromptServer.h"
#include "IBepCommunicationThreadPool.h"

/**
 * Class used by a server to receive Prompt messages on a communication
 * channel opened by an IPromptServer object and service clients.
 *
 * @author Dan Erickson
 * @version 1.0
 * @since Version 1.0
 */
class PromptServerCommThreadPool : public IBepCommunicationThreadPool
{
public:
	/**
	 * Class Constructor
	 * @since Version 1.0
	 */
	PromptServerCommThreadPool(BepComponent *comp) : IBepCommunicationThreadPool(comp)
	{
	}
	/**
	 * Class Destructor
	 * @since Version 1.0
	 */
	virtual ~PromptServerCommThreadPool(void)
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
			if(IsDebugOn())	printf("PromptServerCommThreadPool::Handle(%d)\n", pthread_self());
			std::string message;
			IPromptServer grunt;
			IBepThreadPoolInfo* info = reinterpret_cast<IBepThreadPoolInfo *>(param->data);
			if(info)
			{
				grunt.Initialize(info->comm, info->replyId);
				grunt.HandleRequest(message, m_component);
				pthread_testcancel();
			}
			else throw BepException("Parameter was null in PromptServerCommThreadPool::Handle(%d) done\n", pthread_self());
			if(IsDebugOn())	printf("PromptServerCommThreadPool::Handle(%d) done\n", pthread_self());
		}
		catch(BepException &Err)
		{
			if(IsDebugOn())	printf("PromptServerCommThreadPool::Handle(%d) error: %s\n", pthread_self(), Err.what());
		}
		catch(...)
		{
			if(IsDebugOn())	printf("PromptServerCommThreadPool::Handle(%d) error\n", pthread_self());
		}
		return(0);
	}
};

#endif // PromptServerCommThreadPool_h
