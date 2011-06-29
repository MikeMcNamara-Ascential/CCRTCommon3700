//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/IActiveClientCommunicationThreadPool.cpp 1     1/16/04 4:51p Cward $
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
//    $Log: /Core/ServicesLayer/Library/IActiveClientCommunicationThreadPool.cpp $
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//*************************************************************************
#include "IActiveClientCommunicationThreadPool.h"


IActiveClientCommunicationThreadPool::IActiveClientCommunicationThreadPool(BepComponent *comp)
        : IBepCommunicationThreadPool(comp)
{
    SetDebug(comp->IsDebugOn());
    if(IsDebugOn())	printf("IActiveClientCommunicationThreadPool()\n");
}

IActiveClientCommunicationThreadPool::~IActiveClientCommunicationThreadPool(void)
{
    if(IsDebugOn())	printf("~IActiveClientCommunicationThreadPool()\n");
}

void IActiveClientCommunicationThreadPool::RemoveActiveComms()
{
    #if 0
    //Needs to be updated, causes issues on client restart
    if(IsDebugOn())	printf("Deleting Client Comm()\n");
    for(UINT16 i = 0; i < m_clientComms.size(); i++)
    {
        if(m_clientComms[i] != NULL)
        {
            m_clientComms[i]->SetClientClosed();
            printf("Setting Client Closed: %d\n", i);
            delete m_clientComms[i];
        }
    }
    //signal client is closing
    m_clientComms.clear();
    #endif
}
INT32 IActiveClientCommunicationThreadPool::Handle(ThreadPoolInfo* param)
{   // NOTE:  Do not delete this object in this method, the subscription manager will do it
    std::string message;
    IActiveClientCommunication* grunt = new IActiveClientCommunication();
    try
    {
        ThreadPoolBase::Handle(param);
        if(IsDebugOn())	printf("IActiveClientCommunicationThreadPool::Handle(%d)\n", pthread_self());
        IBepThreadPoolInfo* info = reinterpret_cast<IBepThreadPoolInfo *>(param->data);
        if(info)
        {
            grunt->Initialize(info->comm, info->replyId);
            grunt->HandleRequest(message, m_component);
            pthread_testcancel();
        }
        else throw BepException("Parameter was null in IActiveClientCommunicationThreadPool::Handle(%d) done\n", pthread_self());
        if(IsDebugOn())	printf("IActiveClientCommunicationThreadPool::Handle(%d) done\n", pthread_self());
    }
    catch(BepException &Err)
    {
        if(IsDebugOn())	printf("IActiveClientCommunicationThreadPool::Handle(%d) error: %s\n", pthread_self(), Err.what());
    }
    catch(...)
    {
        if(IsDebugOn())	printf("IActiveClientCommunicationThreadPool::Handle(%d) error\n", pthread_self());
    }
    if(grunt->GetRequestType(message) == BEP_READ || grunt->GetRequestType(message) == BEP_COMMAND) 
    {    
        if(IsDebugOn()) printf("IActiveClientCommunicationThreadPool::Deleting grunt\n");
        delete grunt;
    }
    else
    {//Store reference to delete once client app is closed
        m_clientComms.push_back(grunt);
    }
    return(0);
}
