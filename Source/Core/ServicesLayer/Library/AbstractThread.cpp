//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Library/AbstractThread.cpp $
// $Author: Cward $
//   $Date: 1/16/04 4:51p $
//
// Description:
//  Base class for POSIX thread wrapper
//
//==============================================================================
//
//     Copyright  2002 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Core/ServicesLayer/Library/AbstractThread.cpp $
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
	#include <errno.h>
#endif

#include "AbstractThread.h"
#include "BepException.h"

AbstractThread::AbstractThread(void) : ILogger(), m_thread(0), m_pool(NULL)
{
}

AbstractThread::AbstractThread(const XmlNode* config, IThreadPool* pool /*= NULL*/) : ILogger(), m_thread(0), m_pool(pool)
{
    if(config != NULL)
    {
        ILogger::Initialize(config);
        Log("AbstractThread::AbstractThread(%s, %p)\n", config->getName().c_str(), pool);
    }
}

AbstractThread::~AbstractThread(void)									   	
{
}

void AbstractThread::Initialize(void)
{
    Log("AbstractThread::Initialize()\n");
    if(pthread_attr_init(&m_attributes) != 0)
        throw BepException("Failed to initialize attributes:  %s", strerror(errno));
    if(pthread_attr_setdetachstate(&m_attributes, PTHREAD_CREATE_DETACHED) != 0)
        throw BepException("Failed to set detached:  %s", strerror(errno));
    CreateThread();
}

void AbstractThread::Resume(void)
{
    Initialize();
}

pthread_t AbstractThread::GetId(void)
{
    return(m_thread);
}

const bool AbstractThread::IsInThreadPool(void)
{
    return(GetPool() != NULL);
}

IThreadPool* AbstractThread::GetPool(void)
{
    return(m_pool);
}


void AbstractThread::Block(void)
{
    Log("AbstractThread::Block() done\n");
}

void AbstractThread::Handle(void)
{
    Log("AbstractThread::Handle() done\n");
}

// static member function
void *AbstractThread::ExecuteFunction(void *Arg)
{
    try
    {
        AbstractThread *Thread = reinterpret_cast<AbstractThread *>(Arg);
        if(Thread)
        {
            bool recycle = false;
            Thread->Log("Thread %d Started, Thread = %p, IsInThreadPool = %u\n", pthread_self(), Thread, Thread->IsInThreadPool());
            // install the cleanup handler just in case the thread is canceled
            pthread_cleanup_push(&(AbstractThread::Cancel), Arg);
            do
            {
                Thread->Block();    // I said block!
                if(Thread->IsInThreadPool())
                {
                    Thread->GetPool()->Housekeeping(Thread->GetId(), IThreadPool::BlockDone);
                }
                Thread->Handle();   // I said run!
                if(Thread->IsInThreadPool())
                {
                    recycle = Thread->GetPool()->Housekeeping(Thread->GetId(), IThreadPool::HandleDone);
                }
            }
            while(recycle == true);
            // if the thread has not been canceled, remove the handler and do not execute
            pthread_cleanup_pop(0);
            Thread->Log("AbstractThread Deleting thread %p, %d\n", Thread);
            delete Thread;
        }
        else printf("The thread installed was NULL\n");
    }
    catch(BepException &err)
    {
        printf("Thread exception: %s\n", err.what());
    }
    return(NULL);
}

// static member function
void AbstractThread::Cancel(void *Arg)
{
    AbstractThread *at = reinterpret_cast< AbstractThread * >(Arg);
    delete at;		
}

void AbstractThread::CreateThread(void)
{
    INT32 stat;
    if((stat = pthread_create(&m_thread, &m_attributes, ExecuteFunction, reinterpret_cast<void*>(this))) == 0)
    {
        Log("Created thread %d\n", GetId());
    }
    else throw BepException("Could not create thread stat = %d", stat);
}
