//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Library/ITestResultThreadPool.cpp $
// $Author: Cward $
// $Date: 1/16/04 4:51p $
//
// Description:
//		This is a thread pool class which handles communication with clients of the Test Result server.
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
//     $Log: /Core/ServicesLayer/Library/ITestResultThreadPool.cpp $
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include "ITestResultThreadPool.h"

#include "ITestResultServer.h"

ITestResultServerThreadPool::ITestResultServerThreadPool(BepComponent *comp)
    : IBepCommunicationThreadPool(comp)
{
}

ITestResultServerThreadPool::~ITestResultServerThreadPool(void)
{
}

INT32 ITestResultServerThreadPool::Handle(ThreadPoolInfo* param)
{
    try
    {
        ThreadPoolBase::Handle(param);
        if(IsDebugOn())	printf("ITestResultServerThreadPool::Handle(%d)\n", pthread_self());
        std::string message;
        ITestResultServer grunt;
        IBepThreadPoolInfo* info = reinterpret_cast<IBepThreadPoolInfo *>(param->data);
        if(info)
        {
            grunt.Initialize(info->comm, info->replyId);
            grunt.HandleRequest(message, m_component, false);
            pthread_testcancel();
        }
        else throw BepException("Parameter was null in ITestResultServerThreadPool::Handle(%d) done\n", pthread_self());
        if(IsDebugOn())	printf("ITestResultServerThreadPool::Handle(%d) done\n", pthread_self());
    }
    catch(BepException &Err)
    {
        if(IsDebugOn())	printf("ITestResultServerThreadPool::Handle(%d) error: %s\n", pthread_self(), Err.what());
    }
    catch(...)
    {
        if(IsDebugOn())	printf("ITestResultServerThreadPool::Handle(%d) error\n", pthread_self());
    }
    return(0);
}
