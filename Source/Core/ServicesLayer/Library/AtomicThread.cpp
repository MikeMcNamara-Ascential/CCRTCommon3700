//******************************************************************************
// Description:
//   Base server for all iterative servers.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Library/AtomicThread.cpp $
// 
// 3     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 2     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "AtomicThread.h"

//#include <string>
#include <errno.h>
//#include <pthread.h>

#include "Bepos.h"
//#include "Status.h"
//#include "BepDefs.h"
#include "BepException.h"
//#include "IInterProcessCommunication.h"

AtomicThread::AtomicThread(UINT32 detachState /*= PTHREAD_CREATE_JOINABLE */)
    : Status(ThreadSuspended), m_deleteWhenDone( true), m_thread(-1)
{
    Initialize(detachState);
}

AtomicThread::~AtomicThread(void)
{
    pthread_t id = GetId();
    if(id != -1)
    {
        if(pthread_kill(id, SIGURG) != EOK) if(DEBUG) perror("Thread could not be killed");
        if(pthread_join(id, NULL) != EOK) if(DEBUG) perror("Thread join failed");
    }
}

void AtomicThread::Initialize(UINT32 detachState /*= PTHREAD_CREATE_JOINABLE */)
{
    if(pthread_attr_init(&m_attributes) != EOK)
        throw BepException("Failed to initialize attributes:  %s", strerror(errno));
    if(pthread_attr_setdetachstate(&m_attributes, detachState & PTHREAD_DETACHSTATE_MASK) != EOK)
        throw BepException("Failed to set detached:  %s", strerror(errno));
}

void AtomicThread::Resume(bool deleteWhenDone /*=true*/)
{
    if(!IsThreadRunning())
    {
        m_deleteWhenDone = deleteWhenDone;
        CreateThread();
    }
}

const bool AtomicThread::IsThreadRunning(void)
{
    return(GetStatus() == ThreadRunning);
}

const bool AtomicThread::IsThreadTerminated(void)
{
    return(GetStatus() == ThreadTerminated);
}

INT32 AtomicThread::GetId(void)
{
    return(m_thread);
}

pthread_t AtomicThread::GetInitialThreadId(void)
{
    return(m_initialThreadId);
}

// static member function
void *AtomicThread::ExecuteFunction(void *ptr)
{
	AtomicThread *thread = (AtomicThread *)ptr;

    void* val=NULL;
    try
    {
        if(thread)
        {	// Install the cleanup handler just in case the thread is canceled
            pthread_cleanup_push(&AtomicThread::Cancel, (void *)thread);
            BposSleep(0);
            // Run user's defined function
            val = thread->Execute();
            // Set status to terminated
            thread->SetStatus(ThreadTerminated);
            BposSleep(0);
            // if the thread has not been canceled, remove the handler and do not execute
            pthread_cleanup_pop(0);
        }
        else printf("The thread installed was NULL");
    }
    catch(BepException &BepErr)
    {
        printf("Thread BepException: %s\n", BepErr.Reason().c_str());
    }
    catch(...)
    {
        printf("Thread Unknown Exception\n");
    }

    // If the thread object is valid
    if( thread != NULL)
    {
		int detached, threadId=thread->m_thread;
        // Set status to terminated
        thread->SetStatus(ThreadTerminated);

		// If thread is joinable
		pthread_attr_getdetachstate(&thread->m_attributes, &detached);
		if(	detached == PTHREAD_CREATE_JOINABLE)
		{
			// Join the thread
			pthread_join( threadId, NULL);
		}
        // Invalidate the thread ID
        thread->m_thread = -1;

        // If we need to cleanup
        if( thread->m_deleteWhenDone)
        {
            // Cleanup thread memory
            delete thread;
        }
    }

    return(val);
}

// staticmember function
void AtomicThread::Cancel(void *ptr)
{
	AtomicThread *thread = (AtomicThread *)ptr;

    if( thread->m_deleteWhenDone)
    {
        delete thread;
    }
}

pthread_t AtomicThread::GetThreadId() const
{
    return(m_thread);
}

void AtomicThread::CreateThread(void)
{
    if(pthread_create(&m_thread, &m_attributes, ExecuteFunction, (void*)this) == EOK)
    {
        SetStatus(ThreadRunning);
		// Store the original thread Id so that it never gets invalidate by ExecuteFunction
		SetInitialThreadId(m_thread);
	}
    else throw BepException("Could not create thread\n");
}

void AtomicThread::SetInitialThreadId(pthread_t &threadId)
{
	m_initialThreadId = threadId;
}
