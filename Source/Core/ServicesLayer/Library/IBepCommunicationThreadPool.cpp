//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/IBepCommunicationThreadPool.cpp 3     10/26/06 9:56a Rwiersem $
//
// FILE DESCRIPTION:
//      This is a thread pool class which handles communication with an active client.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/IBepCommunicationThreadPool.cpp $
// 
// 3     10/26/06 9:56a Rwiersem
// Changes for the 06102701 core release:
// 
// Added StopThreadPool().
// 
// 2     7/20/05 8:58p Cward
// Moved thread pool destroy out of base class destructor and into this
// destructor. was causing SIGSEG
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//*************************************************************************
#include "IBepCommunicationThreadPool.h"

#include "INotification.h"


IBepCommunicationThreadPool::IBepCommunicationThreadPool(BepComponent *comp)
    : ThreadPoolBase(), m_component(comp), m_priority(10)
{
}

IBepCommunicationThreadPool::~IBepCommunicationThreadPool(void)
{
// need to call the cleanup here, rather than in the base class
  if(IsDebugOn()) printf("IBepCommunicationThreadPool::~IBepCommunicationThreadPool\n");
  StopThreadPool();
}

void IBepCommunicationThreadPool::Initialize(const XmlNode* config)
{
    if(config)
    {
        try
        {
            SetDebug(config->getChild("Setup")->getChild("ThreadPool")->getChild("Debug")->getValue());
        }
        catch(...)
        {
            printf("SetDebug threw\n");
        }
    }

    if(IsDebugOn()) printf("IBepCommunicationThreadPool::Initialize(%s)\n", config->ToString().c_str());
    ThreadPoolBase::Initialize(config);     // initialize the base class thread pool
    m_comm.Initialize(config->getChild("Setup")->getChild("Communication"));
    if(IsDebugOn()) printf("IBepCommunicationThreadPool::Initialize() done\n");
}

bool IBepCommunicationThreadPool::IsDataOk(const XmlString &data)
{
    return(m_comm.IsDataOk(data));
}

const std::string IBepCommunicationThreadPool::GetName(void)
{
    return(m_comm.GetName());
}

void IBepCommunicationThreadPool::SetPriority(int priority)
{
    m_priority = priority;
}

void IBepCommunicationThreadPool::Allocate(ThreadPoolInfo *param)
{
    IBepThreadPoolInfo* info = NULL;
    try
    {
        ThreadPoolBase::Allocate(param);
        info = new IBepThreadPoolInfo();
        info->replyId = -1;
        param->data = reinterpret_cast<void *>(info);
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Allocate(%d)\n", pthread_self());
    }
    catch(BepException &Err)
    {
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Allocate(%d) error: %s\n", pthread_self(), Err.what());
        if(info) delete info;info = NULL;
    }
    catch(...)
    {
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Allocate(%d) error\n", pthread_self());
        if(info) delete info;info = NULL;
    }
}

void IBepCommunicationThreadPool::Block(ThreadPoolInfo *param)
{
    try
    {
        // ensure that the thread returns to its initial priority upon being recycled
        struct sched_param  schedParam;
        SchedGet( 0, pthread_self(), &schedParam);
        schedParam.sched_priority = m_priority;
        SchedSet( 0, pthread_self(), SCHED_NOCHANGE, &schedParam);

        // block waiting for the next request
        ThreadPoolBase::Block(param);
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Block(%d)\n", pthread_self());
        IBepThreadPoolInfo* info = reinterpret_cast<IBepThreadPoolInfo *>(param->data);
        if(m_comm.GetCommunicationType() == "TCP/IP")
            info->comm.Initialize(m_comm.GetId(), m_comm.GetPort(), m_comm.GetIPAddress(),
                                  m_comm.GetFrameStart(), m_comm.GetFrameEnd(), m_comm.IsDebugOn());
        else if(m_comm.GetCommunicationType() == "IPC")
            // can not just initialize with the default buffer size, must be that of the class, especially when the class could be requesting more than
            // one thing
            // info->comm.Initialize(m_comm.GetId(), m_comm.GetName(), m_comm.IsDebugOn(), -1, m_comm.DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT);
            info->comm.Initialize(m_comm.GetId(), m_comm.GetName(), m_comm.IsDebugOn(), -1, m_comm.GetRxBufferSize(), m_comm.GetTimeout());
        else throw BepException("Bad type of communication object in IBepCommunicationThreadPool::Block()");
        info->replyId = info->comm.WaitForRequest();
    }
    catch(BepException &Err)
    {
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Block(%d) error: %s\n", pthread_self(), Err.what());
    }
    catch(...)
    {
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Block(%d) error\n", pthread_self());
    }
}

INT32 IBepCommunicationThreadPool::Handle(ThreadPoolInfo *param)
{
    try
    {
        ThreadPoolBase::Handle(param);
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Handle(%d)\n", pthread_self());
        std::string message;
        IBepCommunication grunt;
        IBepThreadPoolInfo* info = reinterpret_cast<IBepThreadPoolInfo *>(param->data);
        if(info)
        {
            grunt.Initialize(info->comm, info->replyId);
            grunt.HandleRequest(message, m_component, false);
            pthread_testcancel();
        }
        else throw BepException("Parameter was null in IBepCommunicationThreadPool::Handle(%d) done\n", pthread_self());
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Handle(%d) done\n", pthread_self());
    }
    catch(BepException &Err)
    {
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Handle(%d) error: %s\n", pthread_self(), Err.what());
    }
    catch(...)
    {
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Handle(%d) error\n", pthread_self());
    }
    return(0);
}

void IBepCommunicationThreadPool::Free(ThreadPoolInfo *param)
{
    try
    {
        ThreadPoolBase::Free(param);
        IBepThreadPoolInfo* info = reinterpret_cast<IBepThreadPoolInfo *>(param->data);
        if(info) delete info;info = NULL;
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Free(%d)\n", pthread_self());
    }
    catch(BepException &Err)
    {
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Free(%d) error: %s\n", pthread_self(), Err.what());
    }
    catch(...)
    {
        if(IsDebugOn()) printf("IBepCommunicationThreadPool::Free(%d) error\n", pthread_self());
    }
}

void IBepCommunicationThreadPool::StopThreadPool()
{
    if(IsDebugOn()) printf("Enter IBepCommunicationThreadPool::StopThreadPool()\n");
    ThreadPool::StopThreadPool();
    //m_comm.Close();
    if(IsDebugOn()) printf("Exit IBepCommunicationThreadPool::StopThreadPool()\n");
}

