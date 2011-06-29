/**************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/SubscriptionManager.cpp 5     4/13/06 3:46a Cward $
//
// FILE DESCRIPTION:
//   SubscriptionManager is a singleton that handles publish events for
//   all the client subscription connections.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/SubscriptionManager.cpp $
// 
// 5     4/13/06 3:46a Cward
// Removed the default parameter value specifications from the cpp file
//
// 2     2/15/06 3:57p Bmeinke
// Removed the default parameter value specifications from the cpp file
// (should only be in the h file)
//
// 4     10/27/04 6:46p Cward
// Changes for 04102701 build. Primarily to back out most of the
// Dataserver changes mad in the previous (04101401) build.
//
// 3     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 12:52p Khykin
// Updated with latest mutex changes.
//
// 2     10/10/03 9:35a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 3:58p Khykin
// Initial checkin.
//
// 1     4/22/03 9:51a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.2  2003/02/23 17:16:53  tbochene
//    Removed code so publications will only happen once
//
//    Revision 1.1.1.1  2003/02/15 00:06:34  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:18a Khykin
// Initial Checkin
//
// 1     1/18/03 8:12a Khykin
// Initial Checkin From DCTC
//
// 15    11/20/02 1:25a Tbochene
// Updated AddSubscribers() and Publish() so they work with subscriber
// information retreival functions
//
// 14    11/19/02 9:11p Tbochene
// Added Read() suport
//
// 13    9/26/02 4:37p Tbochene
// Removed compiler warnings
//
// 12    9/12/02 5:49p Tbochene
// Removed multi-threaded interfac to RTOS
//
// 11    9/10/02 6:37p Tbochene
// Removed all conditional compile stuff
//
// 10    7/31/02 12:11p Skypig13
// Updated notify
//
// 9     6/25/02 5:05p Skypig13
// Removed compiler warning by adding return value for Register()
//
// 8     6/19/02 3:03p Skypig13
// Added Register() method
//
// 7     5/30/02 5:04p Skypig13
// Latest stuff that compiles
//
// 6     4/29/02 11:47a Skypig13
// Included m_comm in initialize list
//
// 5     4/25/02 4:46p Skypig13
// Updated with new changes
//    Revision 1.5  2002/04/24 20:46:54  erickson
//    Fixed add subscribers
//
//    Revision 1.4  2002/04/12 15:16:03  erickson
//    added comments about threadpool problems
//
//    Revision 1.3  2002/04/12 02:34:08  erickson
//    Updated thread pool base and component to take #define types for thread pool handle and parameters instead of void
//
//    Revision 1.2  2002/04/11 19:19:34  erickson
//    Updated ThreadPoolBase to properly use muticies and updated tests for INamedDataBroker.  Data server runs and SIGSEGV when the Subscription manager receives a subscription request and attemtps to create the first threads in the  Notification Thread Pool (first ContextAllocate()).
//
//    Revision 1.1.1.1  2002/04/08 19:49:01  erickson
//    started
//
//
// 2     4/08/02 1:23p Skypig13
// Updated Subscription manager
//
// 1     4/03/02 12:25a Skypig13
// Manager of subscribers after subscriber has passed them from data
// server
//
*************************************************************************/

#include <pthread.h>
#include <sys/syspage.h>
#include <sys/neutrino.h>
#include <sys/resource.h>

// NOTE: Do not change include order.  INamedDataBroker must be after NotificationThreadPool to compile properly!!!!!
#include "SubscriptionManager.h"
#include "NotificationThreadPool.h"
#include "IActiveClientCommunication.h"
#include "INamedDataBroker.h"

#define TEST_ONLY (0)

/**
 * Create the subscription manager singleton object
 * @since Version 1.0
 */
SubscriptionManager* SubscriptionManager::manager = NULL;

SubscriptionManager::SubscriptionManager(void) : BepComponent(), m_comm(NULL)
{
    pthread_mutexattr_t attr;

	memset( &attr, 0, sizeof( attr));
	memset( &m_tagDataLock, 0, sizeof( m_tagDataLock));

	pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_DISABLE);
    pthread_mutex_init(&m_tagDataLock, &attr);

    pthread_cond_init(&m_endVar, NULL);
}
SubscriptionManager::~SubscriptionManager(void)
{
	pthread_mutex_destroy(&m_tagDataLock);
	pthread_cond_destroy(&m_endVar);
	Reset();
#if (!NEW_TP)
	for(UINT32 ii = 0; ii < m_thread.size(); ii++)
	{
		pthread_cancel(m_thread[ii]);
		pthread_kill(m_thread[ii], SIGURG);
	}
#endif //!NEW_TP
}
/**
 * Provides a reference to the singleton object. Race condtion here.
 * Should be protected with a mutex of some sort. FIXME.
 */
SubscriptionManager* SubscriptionManager::GetManager(void)
{
	if(SubscriptionManager::manager == NULL)
	{
		manager = new SubscriptionManager();
	}
	return(manager);
}
void SubscriptionManager::AddSubscriber(const XmlNode* data, IPublish* subscriber)
{
	std::string tag;

	Log(LOG_FN_ENTRY, "SubscriptionManager::AddSubscriber(%s, %p)\n", data->ToString().c_str(), subscriber);

	// Wait for the first subscription request to establish an IPC thread pool
	if(m_nList == NULL)
	{
		Log(LOG_FN_ENTRY, "Create a notification list\n");
		NotificationThreadPool<IActiveClientCommunication>* pool = new NotificationThreadPool<IActiveClientCommunication>();
		pool->Initialize(m_nListConfig);
		pool->Start();
		m_nList = pool;
	}
	if(subscriber != NULL)
	{
		if(m_nList != NULL)
		{
// build CcrtR04102701, switching back to the previous code.
// leaving the non-working code (in the else) to use as a starting point later
#if 1
			const XmlNodeMap& kids = data->getChildren();
			TagSetIter Iter;
			for(XmlNodeMapCItr iter = kids.begin(); iter != kids.end(); ++iter)
			{
				m_nList->AddNotification(iter->second->getName(), subscriber);
				Iter = m_tagList.find(iter->second->getName());
				if(Iter != m_tagList.end())
				{
					Iter->second.push_back(atoi(subscriber->GetName().c_str()));
				}
				else
				{
					SocketVector sktVect;
					sktVect.push_back(atoi(subscriber->GetName().c_str()));
					m_tagList.insert(TagSetVal(iter->second->getName(), sktVect));
				}
			}
#else
// non-working code..... remove this comment after altering (CKW 10/27/2004)
			TagSetIter Iter;
			m_nList->AddNotification(data->getName(), subscriber);
			Iter = m_tagList.find(data->getName());
			if(Iter != m_tagList.end())
			{
				Iter->second.push_back(atoi(subscriber->GetName().c_str()));
			}
			else
			{
				SocketVector sktVect;
				sktVect.push_back(atoi(subscriber->GetName().c_str()));
				m_tagList.insert(TagSetVal(data->getName(), sktVect));
			}
#endif
		}
		else throw BepException("SubscriptionManager::AddSubscriber() m_nList was null when it should not have been\n");
	}
	else throw BepException("SubscriptionManager::AddSubscriber() adding NULL subscriber\n");
	Log(LOG_FN_ENTRY, "SubscriptionManager::AddSubscriber(%s, %p) done\n", data->ToString().c_str(), subscriber);
}
void SubscriptionManager::Initialize(const XmlNode *document)
{	// Initialize the server
	Log(LOG_FN_ENTRY, "SubscriptionManager::Initialize(%s)\n", document->ToString().c_str());
	if(m_comm == NULL)
	{	// if we've been Initialized() before, don't do anything here
		BepComponent::Initialize(document);
#if (NEW_TP)
		m_comm = new TemplateIBepCommunicationThreadPool(this);
		m_comm->Initialize(document);
		m_comm->Start();
#else
		pthread_t thread;
		m_comm = new IBepCommunication();
		m_comm->Initialize(document->getChild("Setup")->getChild("Communication"));
		pthread_attr_t attributes;
		pthread_attr_init(&attributes);
		pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);

		for(INT32 ii = 0; ii < atol(document->getChild("Setup")->getChild("ThreadPool")->getChild("MinBlockedLimit")->getValue().c_str()); ii++)
		{
			pthread_create(&thread, &attributes, &SubscriptionManager::ThreadMethod, NULL);
			m_thread.push_back(thread);
		}
#endif // NEW_TP
		m_name = document->getChild("Setup")->getChild("Communication")->getChild("Name")->getValue();
	}
}
void SubscriptionManager::Reset(void)
{
	std::string response;
	Log(LOG_FN_ENTRY, "SubscriptionManager::Reset()\n");
	if(m_nList)	m_nList->RemoveAll();
	INamedDataBroker iNdb(IsDebugOn());
	for(TagSetIter iter = m_tagList.begin(); iter != m_tagList.end(); ++iter)
	{
		iNdb.Unsubscribe(iter->first, GetName(), response, true);
		iter->second.clear();
	}
	m_tagList.clear();
	Log(LOG_FN_ENTRY, "SubscriptionManager::Reset() done\n");
}
const std::string SubscriptionManager::Read(const XmlNode *node, const INT32 rate)
{
	std::string Name(node->getName()), Tag = node->getValue(), Value("-1");

	TagSetIter Iter = m_tagList.find(Tag);

	char buffer[256];		// buffer to create messages with
	if(Name == "GetNumberOfSubscribers")
	{
		UINT32 size = 0;

		if(Iter != m_tagList.end())
		{
			size = Iter->second.size();
		}
		Value = CreateMessage(buffer, sizeof(buffer), "%d:%d", size, m_nList->GetNumberOfSubscribers(Tag));
	}
	else if(Name == "GetSubscriberSocket")
	{
		if(Iter != m_tagList.end())
		{
			for(UINT32 ii = 0; ii < Iter->second.size(); ii++)
			{
				Value += CreateMessage(buffer, sizeof(buffer), "%d,", Iter->second[ii]);
			}
		}
	}
	else Value = BepComponent::Read(node);

	return(Value);
}
void SubscriptionManager::Terminate(void)
{
	Log(LOG_FN_ENTRY, "SubscriptionManager::Terminate()\n");
	pthread_cond_signal(&m_endVar);
	BepComponent::Terminate();	// Call base class to clean up
}
const std::string SubscriptionManager::Publish(const XmlNode* node)
{
	if(IsDebugOn())	printf("SubscriptionManager::Publish(%s)\n", node->ToString().c_str());
#if (TEST_ONLY)
#warning Test is on (should be turned off for production)
	std::string Message;
	ITcpIp Tester;
	XmlNode Node(BEP_XML_ROOT, "");
	Node.addChild(BEP_XML_MSG_TYPE, BEP_PUBLISH, ATTRIBUTE_NODE);
	Node.addChild(node->Copy());
	Node.writeNode(Message);

	try
	{
		int Socket = atoi(node->getAttributes().getNode("Socket")->getValue().c_str());

		Tester.Initialize("Client", -1, "", Socket, false);
		Tester.Write(Message);

		Log(LOG_DEV_DATA, "Special Publish called\n");
	}
	catch(BepException& BepErr)
	{	// Node does not have socket attribute
		TagSetIter iter = m_tagList.find(node->getName());

		if(iter != m_tagList.end())
		{
			for(UINT32 ii = 0; ii < iter->second.size(); ii++)
			{
				Tester.Initialize("Client", -1, "", iter->second[ii], false);
				Tester.Write(Message);
				Log(LOG_DEV_DATA, "Wrote to %d with %s\n", iter->second[ii], Message.c_str());
			}
		}

		Log(LOG_DEV_DATA, "Regular Publish called\n");
	}
#else
	m_nList->Notify(node);
    Log(LOG_DEV_DATA, "NL Publish called\n");
#endif

	return(BEP_SUCCESS_RESPONSE);
}
const std::string SubscriptionManager::Register(void)
{
	SetStatus(BEP_RUNNING_STATUS);
	return(BEP_SUCCESS_RESPONSE);
}
void SubscriptionManager::Run(volatile bool *terminateFlag)
{
	Log(LOG_FN_ENTRY, "SubscriptionManager::Run()");
#if (PROFILING_ON)
#warning Profiling on
	struct rusage usage;

	UINT64 stamp1=0, stamp2=0, user2=0, user1=0, cycles=0, use=0;
#endif
	pthread_mutex_lock(&m_tagDataLock);

	Log(LOG_FN_ENTRY, "Status = %s", GetStatus().c_str());
	while(GetStatus() != BEP_TERMINATE_STATUS)	// Run until we should quit
	{
#if (PROFILING_ON)
		stamp2 = ClockCycles();
		if(getrusage(RUSAGE_SELF, &usage) < 0)
		{
			perror("getrusage");
		}
		else
		{
			user2 = usage.ru_utime.tv_sec * 1000000 + usage.ru_utime.tv_usec;
			cycles = stamp2 - stamp1;
			use = user2 - user1;
			printf("utime = %llu ", use);
			printf("cycles = %llu ", cycles);
			printf("utilization = %2.1f \n\n",
				   100.0 * ((float) (use) / ((cycles * 1000000) / SYSPAGE_ENTRY(qtime)->cycles_per_sec)));
		}
		user1 = user2;
		stamp1 = ClockCycles();
		BposSleep(5);
#else
		BepComponent::Run();	// Call base class
		pthread_cond_wait(&m_endVar, &m_tagDataLock);
#endif // PROFILING_ON
	}
	pthread_mutex_lock(&m_tagDataLock);
}
const std::string SubscriptionManager::GetName(void)
{
	return(m_name);
}

void SubscriptionManager::DeleteManager(void)
{
    if(manager)
    {
        delete manager;
    }
    manager = NULL;
}

#if (!NEW_TP)
void* SubscriptionManager::ThreadMethod(void *args)
{
	INT32 id;
	std::string message;
	IBepCommunication worker;

	SubscriptionManager* manager = SubscriptionManager::GetManager();
	while(manager->GetStatus() != BEP_TERMINATE_STATUS)
	{
		try
		{
			id = manager->GetManager()->m_comm->WaitForRequest();
			worker.Initialize(*manager->GetManager()->m_comm, id);			// Wait for message and set up worker
			worker.HandleRequest(message, manager);	// Handle incomming request
		}
		catch(BepException &e)
		{
			printf("Exception In SubscriptionManager::ThreadMethod: %s\n", e.what());
		}
		catch(...)
		{
			printf("Unexpected Exception In SubscriptionManager::ThreadMethod\n");
		}
	}
	return(NULL);
}
#endif // !NEW_TP

