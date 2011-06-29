//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Library/ThreadPool.cpp $
// $Author: Rwiersem $
// $Date: 10/26/06 10:00a $
//
// Description:
//   Class that provides the methods to create a thead pool and manage it.
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/ThreadPool.cpp $
// 
// 3     10/26/06 10:00a Rwiersem
// Changes for the 06102701 core release:
// 
// Added StopThreadPool().
// 
// 2     7/20/05 9:00p Cward
// Added support for unblocking. Removed the thread pool destroy from the
// destructor. Moved to derived class.
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include "ThreadPool.h"     // this must be declared before includes else you will go blind!




const THREAD_POOL_HANDLE_T *ThreadPool::GetHandle()
{
    pthread_rwlock_rdlock(&m_dataLock);
    THREAD_POOL_HANDLE_T *temp = m_poolAttr.handle;
    pthread_rwlock_unlock(&m_dataLock);
    return(temp);
}
void ThreadPool::SetHandle(THREAD_POOL_HANDLE_T *handle)
{
    pthread_rwlock_wrlock(&m_dataLock);
    m_poolAttr.handle = handle;
    pthread_rwlock_unlock(&m_dataLock);
}
void ThreadPool::SetInitFunction(NewFunction ncf)
{
    pthread_rwlock_wrlock(&m_dataLock);
    m_poolAttr.context_alloc = ncf;
    pthread_rwlock_unlock(&m_dataLock);
}
void ThreadPool::SetBlockingFunction(BlockingFunction bf)
{
    pthread_rwlock_wrlock(&m_dataLock);
    m_poolAttr.block_func = bf;
    pthread_rwlock_unlock(&m_dataLock);
}
void ThreadPool::SetUnBlockingFunction(UnblockingFunction ubf)
{
   pthread_rwlock_wrlock(&m_dataLock);
   m_poolAttr.unblock_func = ubf;
   pthread_rwlock_unlock(&m_dataLock);
}
void ThreadPool::SetHandlingFunction(HandlingFunction hf)
{
    pthread_rwlock_wrlock(&m_dataLock);
    m_poolAttr.handler_func = hf;
    pthread_rwlock_unlock(&m_dataLock);
}
void ThreadPool::SetDeleteFunction(DeleteFunction df)
{
    pthread_rwlock_wrlock(&m_dataLock);
    m_poolAttr.context_free = df;
    pthread_rwlock_unlock(&m_dataLock);
}
INT32 ThreadPool::GetMinThreadsAvaliable()
{
    pthread_rwlock_rdlock(&m_dataLock);
    int temp = m_poolAttr.lo_water;
    pthread_rwlock_unlock(&m_dataLock);
    return(temp);
}
void ThreadPool::SetMinThreadsAvaliable(INT32 minThreads)
{
    pthread_rwlock_wrlock(&m_dataLock);
    m_poolAttr.lo_water = minThreads;
    pthread_rwlock_unlock(&m_dataLock);
}
INT32 ThreadPool::GetMaxThreadsAvaliable()
{
    pthread_rwlock_rdlock(&m_dataLock);
    int temp = m_poolAttr.hi_water;
    pthread_rwlock_unlock(&m_dataLock);
    return(temp);
}
void ThreadPool::SetMaxThreadsAvaliable(INT32 maxThreads)
{
    pthread_rwlock_wrlock(&m_dataLock);
    m_poolAttr.hi_water = maxThreads;
    pthread_rwlock_unlock(&m_dataLock);
}
INT32 ThreadPool::GetThreadIncrement()
{
    pthread_rwlock_rdlock(&m_dataLock);
    int temp = m_poolAttr.increment;
    pthread_rwlock_unlock(&m_dataLock);
    return(temp);
}
void ThreadPool::SetThreadIncrement(INT32 increment)
{
    pthread_rwlock_wrlock(&m_dataLock);
    m_poolAttr.increment = increment;
    pthread_rwlock_unlock(&m_dataLock);
}
int ThreadPool::GetMaximumThreads()
{
    pthread_rwlock_rdlock(&m_dataLock);
    int temp = m_poolAttr.maximum;
    pthread_rwlock_unlock(&m_dataLock);
    return(temp);
}
void ThreadPool::SetMaximumThreads(INT32 maxNumber)
{
    pthread_rwlock_wrlock(&m_dataLock);
    m_poolAttr.maximum = maxNumber;
    pthread_rwlock_unlock(&m_dataLock);
}
void ThreadPool::AddSelf(void)
{
    if(IsDebugOn()) printf("AddSelf()\n");
    add(pthread_self());    // add myself to the thread list
}
void ThreadPool::RemoveSelf(void)
{
    if(IsDebugOn()) printf("RemoveSelf()\n");
    remove(pthread_self(),false);
}
void ThreadPool::StartThreadPool(void *arg)
{
    if(IsDebugOn()) printf("Starting Thread Pool\n");
    // create the thread pool
    if(arg)
    {
        m_poolAttr.handle = (THREAD_POOL_HANDLE_T*)arg;
        if(IsDebugOn()) printf("ThreadPool::StartThreadPool() arg is %p\n", arg);
        if((m_TPR = thread_pool_create(&m_poolAttr, 0)) == NULL)
            throw BepException("Unable to create a thread pool: %s", strerror(errno));
        if(IsDebugOn()) printf("Thread Pool Created\n");
        // Start the thread pool
        if(thread_pool_start(m_TPR) != EOK)
        {
            throw BepException("Unable to start the thread pool");
        }
    }
    else throw BepException("Error: ThreadPool::StartThreadPool(arg == NULL)");
    if(IsDebugOn()) printf("Thread Pool Started\n");
}

void ThreadPool::StopThreadPool()
{
    if(IsDebugOn()) printf("    -- ready to delete threads\n");
    if(m_TPR != NULL)
    {
        thread_pool_destroy(m_TPR);
        m_TPR = NULL;
    }
    if(IsDebugOn()) printf("    -- done deleting threads\n");
}

void ThreadPool::Initialize(const XmlNode *config)
{
    // Stop the current thread pool so we can re-config
    StopThreadPool();

    try
    {   // set the debug status
        if(config) SetDebug(config->getChild("Debug")->getValue());
    }
    catch(XmlException &Err)
    {
        printf("ThreadPool Warning: No Debug status defined in config file\n%s\n", config->ToString().c_str());
        SetDebug(false);
    }
    memset(&m_poolAttr, 0, sizeof(m_poolAttr)); // reset all of the thread pool attributes
    if(pthread_attr_init(&m_attr) != EOK) printf("Failed to initialize thread attributes because: %s\n", strerror(errno));
    // set the threads in the thread pool to detached
    pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_DETACHED);
    m_poolAttr.attr = &m_attr;
    try
    {   // read the setup from the configuration file or default to single thread behavior
        if(config)
        {
            m_poolAttr.lo_water = atol(config->getChildren().getNode("MinBlockedLimit")->getValue().c_str());
            m_poolAttr.hi_water = atol(config->getChildren().getNode("MaxBlockedLimit")->getValue().c_str());
            m_poolAttr.increment = atol(config->getChildren().getNode("Increment")->getValue().c_str());
            m_poolAttr.maximum = atol(config->getChildren().getNode("Maximum")->getValue().c_str());
        }
        else
        {
            m_poolAttr.lo_water = 1;
            m_poolAttr.hi_water = 1;
            m_poolAttr.increment = 0;
            m_poolAttr.maximum = 1;
        }
    }
    catch(XmlException &XmlErr)
   {
      // if not set up correctly, set up to defaults
        m_poolAttr.lo_water = 2;
        m_poolAttr.hi_water = 4;
        m_poolAttr.increment = 1;
        m_poolAttr.maximum = 30;
    }
    if(IsDebugOn()) printf("ThreadPool::Initialize()\nm_poolAttr.lo_water = %d\nm_poolAttr.hi_water = %d\n"
                           "m_poolAttr.increment = %d\nm_poolAttr.maximum = %d\n",
                           m_poolAttr.lo_water, m_poolAttr.hi_water, m_poolAttr.increment, m_poolAttr.maximum);
}
ThreadPool::~ThreadPool(void)
{

   // NOTE THIS CAN NOT BE DONE HERE
   // thread_pool_destroy() calls ContextAllocate with an object pointer
   // of the derived type. This in turn calls Allocate in the derived class.
   // Unfortunately, the derived class destructor has already run before we
   // get to this point, so the Allocate function it is trying to call has
   // already been removed from memory.

   // moved this block to IBepCommunicationThreadPool to attempt to resolve

//   if(IsDebugOn()) printf("ThreadPool::~ThreadPool() -- ready to delete threads\n");
//   if(m_TPR != NULL) thread_pool_destroy(m_TPR);
//   if(IsDebugOn()) printf("ThreadPool::~ThreadPool() -- done deleting threads\n");

   // destroy the thread lock
    while(pthread_rwlock_destroy(&m_dataLock) == EBUSY)
        delay(50);
   if(IsDebugOn()) printf("ThreadPool::~ThreadPool() -- done\n");
}
ThreadPool::ThreadPool(void) : ThreadList(), m_TPR(NULL)
{   // initialize the thread lock with default attributes
    if(pthread_rwlock_init(&m_dataLock, NULL) != EOK)
        throw BepException("Error, Can Not Initialize The Data Lock In ThreadPool");
}


