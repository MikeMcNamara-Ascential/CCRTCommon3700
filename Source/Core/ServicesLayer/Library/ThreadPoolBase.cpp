//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/ThreadPoolBase.cpp 3     7/20/05 9:02p Cward $
//
// FILE DESCRIPTION:
//		This is a base thread pool class which provides all the functionality
//		and behavior of the thread pool.  Classes derived from this class
//	    implement the virtual methods Allocate, Block, Handle, and Free.
//		These methods in the ervied classes should also call the
//		corresponding methods in the base class.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/ThreadPoolBase.cpp $
// 
// 3     7/20/05 9:02p Cward
// added support for unblocking. updated debug print message contents
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:35a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:51p Khykin
// Initial checkin.
//
// 1     4/22/03 9:33a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:22a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:44  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:26a Khykin
// Initial Checkin
//
// 1     1/18/03 7:58a Khykin
// Initial Checkin From DCTC
//
// 3     4/25/02 4:36p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:05  erickson
//    started
//
//
// 1     2/11/02 10:05a Bmeinke
// Thread pool base class
//
//*************************************************************************

#include "ThreadPoolBase.h"

//BepMutex ThreadPoolBase::m_muticies[free];
pthread_mutex_t ThreadPoolBase::m_muticies[free+1];
bool ThreadPoolBase::m_muticiesInitialized = false;




ThreadPoolBase::ThreadPoolBase(void) : ThreadPool(), IThread()
{
    InitializeMuticies(this);
}
/**
 * Destructor method for the ThreadPoolBase class.
* @since Version 1.0
 */
ThreadPoolBase::~ThreadPoolBase(void)
{
}
/**
 * Initialize the thread pool.
 *
 * @param threadInfo Xml Node containing the parameters for the behavior of
 *                   the thread pool.
 * @since Version 1.0
 */
void ThreadPoolBase::Initialize(const XmlNode *config)
{
    if(IsDebugOn()) printf("ThreadPoolBase::Initialize(%s)\n", config->ToString().c_str());
    ThreadPool::Initialize(config->getChild("Setup")->getChild("ThreadPool"));    // Set the parameters of the thread pool
    SetInitFunction(ContextAllocate);
    SetBlockingFunction(BlockingMethod);
    SetUnBlockingFunction(UnblockingMethod);
    SetHandlingFunction(HandlingMethod);
    SetDeleteFunction(ContextFree);
    if(IsDebugOn())printf("ThreadPoolBase::Initialize() done\n");
}
/**
 * Initialize the static muticies
 * @since Version 2.1
 */
void ThreadPoolBase::InitializeMuticies(ThreadPoolBase* intitalizer)
{
    if(intitalizer->IsDebugOn()) printf("ThreadPoolBase::InitializeMuticies()\n");
    if(m_muticiesInitialized == false)
    {
        if(intitalizer->IsDebugOn()) printf("Muticies need to be initialized\n");
        pthread_mutexattr_t attr;
        memset( &attr, 0, sizeof( attr));
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_ENABLE);
        for(UINT32 ii = 0; ii < sizeof(m_muticies)/sizeof(m_muticies[0]); ii++)
        {
            memset( &m_muticies[ii], 0, sizeof( m_muticies[ii]));
            pthread_mutex_init(&m_muticies[ii], &attr);
        }
        m_muticiesInitialized = true;
    }
    else if(intitalizer->IsDebugOn()) printf("Muticies don't need to be initialized\n");
}
/**
 * Method to call when we need to create a new thread.
 *
 * NOTE:  ThreadPoolBase::Allocate() must be called first
 * in derived class Allocate()
 *
 * @param thread Reference to the ThreadPool object.
 * @return Input to the blocking function.
 * @since Version 1.0
 */
THREAD_POOL_PARAM_T* ThreadPoolBase::ContextAllocate(THREAD_POOL_HANDLE_T *thread)
{
    THREAD_POOL_PARAM_T* info = NULL;
    try
    {   // Get mutex
        if(pthread_mutex_lock(&m_muticies[allocate]) == EOK)
        {
            if(thread)
            {
				if(thread->IsDebugOn()) printf("TPB::ContextAllocate 1(%d, %p)\n", pthread_self(), thread);
                info = new THREAD_POOL_PARAM_T;  // Create parameters for next methods
                info->thread = thread;
				if(info->thread->IsDebugOn()) printf("TPB::ContextAllocate 2(%d, %p)\n", pthread_self(), info->thread);
                if(pthread_mutex_unlock(&m_muticies[allocate]) == EOK)
                {
					if(info->thread->IsDebugOn()) printf("TPB::ContextAllocate unlocked mutex()\n");
                    info->thread->Allocate(info);       // Do some work
					if(info->thread->IsDebugOn()) printf("TPB::Allocate finished()\n");
				}
                else throw BepException("Unable to release mutex");
            }
            else throw BepException("thread object was null");
        }
        else throw BepException("Unable to acquire mutex");
    }
    catch(BepException &BepErr)
    {
        pthread_mutex_unlock(&m_muticies[allocate]);
        printf("Error in ThreadPoolBase::ContextAllocate(%d): %s\n", pthread_self(), BepErr.what());
    }
    catch(...)
    {
        pthread_mutex_unlock(&m_muticies[allocate]);
        printf("Error in ThreadPoolBase::ContextAllocate(%d)\n", pthread_self());
    }
    return((THREAD_POOL_PARAM_T*)info);
}
/**
 * Method to call to perform the blocking operation.
 *
 * NOTE:  ThreadPoolBase::Block() must be called first
 * in derived class Block()
 *
 * @param param  Result of the ContextAllocate method.
 * @return Input to the handling function.
 * @since Version 1.0
 */
THREAD_POOL_PARAM_T* ThreadPoolBase::BlockingMethod(THREAD_POOL_PARAM_T *param)
{
    try
    {   // Get mutex
        if(pthread_mutex_lock(&m_muticies[block]) == EOK)
        {
            if(param)
            {
                if(param->thread)
                {
                    if(pthread_mutex_unlock(&m_muticies[block]) == EOK)
                    {
                        param->thread->Block(param);    // Do some work
                    }
                    else throw BepException("Unable to release mutex");
                }
                else throw BepException("thread object was null");
            }
            else throw BepException("param was null");
        }
        else throw BepException("Unable to acquire mutex");
    }
    catch(BepException &BepErr)
    {
        pthread_mutex_unlock(&m_muticies[block]);
        printf("Error in ThreadPoolBase::BlockingMethod(%d): %s\n", pthread_self(), BepErr.what());
    }
    catch(...)
    {
        pthread_mutex_unlock(&m_muticies[block]);
        printf("Error in ThreadPoolBase::BlockingMethod(%d)\n", pthread_self());
    }
    return(param);
}
/**
 * Method to call to perform the unblocking operation.
 *
 * @param param  Result of the ContextAllocate method.
 */
void ThreadPoolBase::UnblockingMethod(THREAD_POOL_PARAM_T *param)
{
    try
    {   // Get mutex
        if(pthread_mutex_lock(&m_muticies[unblock]) == EOK)
        {
            if(param)
            {
                if(param->thread)
                {
                    if(pthread_mutex_unlock(&m_muticies[unblock]) == EOK)
                    {
                        param->thread->Unblock(param);    // Do some work
                    }
                    else throw BepException("Unable to release mutex");
                }
                else throw BepException("thread object was null");
            }
            else throw BepException("param was null");
        }
        else throw BepException("Unable to acquire mutex");
    }
    catch(BepException &BepErr)
    {
        pthread_mutex_unlock(&m_muticies[unblock]);
        printf("Error in ThreadPoolBase::UnblockingMethod(%d): %s\n", pthread_self(), BepErr.what());
    }
    catch(...)
    {
        pthread_mutex_unlock(&m_muticies[unblock]);
        printf("Error in ThreadPoolBase::UnblockingMethod(%d)\n", pthread_self());
    }
}
/**
 * Method to call to perform the work.
 *
 * NOTE:  ThreadPoolBase::Handle() must be called first
 * in derived class Handle()
 *
 * @param param  Result of the BlockingMethod method.
 * @return Zero.
 * @since Version 1.l
 */
INT32 ThreadPoolBase::HandlingMethod(THREAD_POOL_PARAM_T *param)
{
    INT32 stat = 0; // At this time you must return 0 (OS supports nothing else as of Neutrino 6.0)
    try
    {   // Get mutex
        if(pthread_mutex_lock(&m_muticies[handle]) == EOK)
        {
            if(param)
            {
                if(param->thread)
                {
                    if(pthread_mutex_unlock(&m_muticies[handle]) == EOK)
                    {
                        param->thread->Handle(param);    // Do some work
                    }
                    else throw BepException("Unable to release mutex");
                }
                else throw BepException("thread object was null");
            }
            else throw BepException("param was null");
        }
        else throw BepException("Unable to acquire mutex");
    }
    catch(BepException &BepErr)
    {
        pthread_mutex_unlock(&m_muticies[handle]);
        printf("Error in ThreadPoolBase::HandlingMethod(%d): %s\n", pthread_self(), BepErr.what());
    }
    catch(...)
    {
        pthread_mutex_unlock(&m_muticies[handle]);
        printf("Error in ThreadPoolBase::HandlingMethod(%d)\n", pthread_self());
    }
    return(stat);
}
/**
 * Method to call to destroy a thread.
 *
 * NOTE:  ThreadPoolBase::Free() must be called first
 * in derived class Free()
 *
 * @param param  Output from ContextAllocate method.
 * @since Version 1.0
 */
void ThreadPoolBase::ContextFree(THREAD_POOL_PARAM_T *param)
{
#if 1
    int lockStat;
    // Get mutex
    if( (lockStat=pthread_mutex_lock(&m_muticies[free])) == EOK)
    {
        try
        {
            param->thread->Free(param);    // Do some work
            delete param;
            param = NULL;
        }
        catch(BepException &BepErr)
        {
            printf("Error in ThreadPoolBase::ContextFree(%d): %s\n", pthread_self(), BepErr.what());
        }
        catch(...)
        {
            printf("Error in ThreadPoolBase::ContextFree(%d)\n", pthread_self());
        }
        pthread_mutex_unlock(&m_muticies[free]);
    }
    else
    {
        printf( "Error locking mutex in ContextFree(): %s\n", strerror( lockStat));
    }

#else
/*
Who's the jack ass that did this? Whoever it is should be fired...oh wait a minute...they were
    */
    try
    {
        // Get mutex
        if(pthread_mutex_lock(&m_muticies[free]) == EOK)
        {
            if(param)
            {
                if(param->thread)
                {
                    if(pthread_mutex_unlock(&m_muticies[free]) == EOK)
                    {
                        param->thread->Free(param);    // Do some work
                        delete param;
                    }
                    else throw BepException("Unable to release mutex");
                }
                else throw BepException("thread object was null");
            }
            else throw BepException("param was null");
        }
        else throw BepException("Unable to acquire mutex");
    }
    catch(BepException &BepErr)
    {
        pthread_mutex_unlock(&m_muticies[free]);
        printf("Error in ThreadPoolBase::ContextFree(%d): %s\n", pthread_self(), BepErr.what());
    }
    catch(...)
    {
        printf("Error in ThreadPoolBase::ContextFree(%d)\n", pthread_self());
    }
#endif
}
/**
 * Dervied class method for performing setup for each thread.
 * @since Version 1.0
 */
void ThreadPoolBase::Allocate(ThreadPoolInfo *param)
{
    if(IsDebugOn()) printf("ThreadPoolBase::Allocate()\n");
    AddSelf();
}
/**
 * Dervied class method for performing the blocking operation.
 * @since Version 1.0
 */
void ThreadPoolBase::Block(ThreadPoolInfo *param)
{
    pthread_testcancel();
}
/**
 * Dervied class method for performing the unblocking operation.
 * @since Version 1.0
 */
void ThreadPoolBase::Unblock(ThreadPoolInfo *param)
{
   ThreadListCItr_t  itr;
   int            threadId;

//   if((itr = ThreadList_t::begin()) != ThreadList_t::end())
   while((itr = ThreadList_t::begin()) != ThreadList_t::end())
   {
      threadId = itr->first;
      remove( threadId, true);
   }
}
/**
 * Derived class method for performing the work.
 *
 * @return Must return zero.
 * @since Version 1.0
 */
INT32 ThreadPoolBase::Handle(ThreadPoolInfo *param)
{
    pthread_testcancel();
    return(0);  // This is the only value that is supported as of QNX NTO 6.0
}
/**
 * Dervied class method for destroying threads.
 * If any memory was dynamicaly declared, it must be deleted
 * in the derived Free method.
 * @since Version 1.0
 */
void ThreadPoolBase::Free(ThreadPoolInfo *param)
{
    RemoveSelf();
}

