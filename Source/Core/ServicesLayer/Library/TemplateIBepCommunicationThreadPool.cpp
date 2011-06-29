//******************************************************************************
// Description:
//	Implementation of the Communication Thread and Template IBep Communication
//	ThreadPool class methods.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Core/ServicesLayer/Library/TemplateIBepCommunicationThreadPool.cpp $
// 
// 3     6/09/04 11:38p Cward
// Added new standard header comment block
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "TemplateIBepCommunicationThreadPool.h"

CommunicationThread::CommunicationThread(void) : AbstractThread(), m_component(NULL), m_replyId(-1)
{
}
CommunicationThread::CommunicationThread(const XmlNode* config, IThreadPool* pool /*= NULL*/)
    : AbstractThread(config , pool), m_component(NULL), m_replyId(-1)
{
	Log("AbstractThread::AbstractThread(%p, %p)\n", config, pool);
}


CommunicationThread::~CommunicationThread(void)
{
    if(IsDebugOn())	printf("CommunicationThread::~CommunicationThread()\n");
}

void CommunicationThread::Initialize(void)
{
	Log("CommunicationThread::Initialize()\n");
	if(IsInThreadPool())
	{
		TemplateIBepCommunicationThreadPool* pool = dynamic_cast<TemplateIBepCommunicationThreadPool*>(GetPool());
		if(pool != NULL)
		{
			m_component = pool->m_component;
			if(pool->m_server.GetCommunicationType() == "TCP/IP")
			{
				m_server.Initialize(pool->m_server.GetId(), pool->m_server.GetPort(), pool->m_server.GetIPAddress(),
									pool->m_server.GetFrameStart(), pool->m_server.GetFrameEnd(), pool->m_server.IsDebugOn());
			}
			else if(pool->m_server.GetCommunicationType() == "IPC")
			{
				m_server.Initialize(pool->m_server.GetId(), pool->m_server.GetName(), pool->m_server.IsDebugOn(), -1, DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT);
			}
			else throw BepException("Bad type of communication object in CommunicationThread::Initialize()");
		}
		else Log("Thread dynamic_cast failed\n");
	}
	else Log("ThreadThreadPool was NULL\n");

	AbstractThread::Initialize();
	Log("CommunicationThread::Initialize() done\n");
}
void CommunicationThread::Block(void)
{
	if(IsInThreadPool())
	{
		TemplateIBepCommunicationThreadPool* pool = dynamic_cast<TemplateIBepCommunicationThreadPool*>(GetPool());
		if(pool != NULL)
		{
			m_replyId = m_server.WaitForRequest();
		}
		else Log("Thread dynamic_cast failed\n");
	}
	else Log("ThreadThreadPool was NULL\n");
}
void CommunicationThread::Handle(void)
{
	if(IsInThreadPool())
	{
		TemplateIBepCommunicationThreadPool* pool = dynamic_cast<TemplateIBepCommunicationThreadPool*>(GetPool());
		if(pool != NULL)
		{
			std::string message;
			m_worker.Initialize(m_server, m_replyId);
			m_worker.HandleRequest(message, m_component, false);
			pthread_testcancel();
		}
		else Log("Thread dynamic_cast failed\n");
	}
	else Log("ThreadThreadPool was NULL\n");
}

///////////////// TemplateIBepCommunicationThreadPool Methods //////////////////////////


TemplateIBepCommunicationThreadPool::TemplateIBepCommunicationThreadPool(BepComponent* comp,
                                                                         const pthread_t& creator /*= pthread_self()*/)
    : TemplateIBepCommunicationThreadPoolType(creator), m_component(comp)
{
}

TemplateIBepCommunicationThreadPool::~TemplateIBepCommunicationThreadPool(void)
{
}

void TemplateIBepCommunicationThreadPool::Initialize(const XmlNode *configuration)
{
    m_server.Initialize(configuration->getChild("Setup")->getChild("Communication"));
    TemplateIBepCommunicationThreadPoolType::Initialize(configuration);
}
