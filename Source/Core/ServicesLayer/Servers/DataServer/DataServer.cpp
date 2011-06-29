//*************************************************************************
//
// Description:
//   Data server that will be the interface between the User Interface and Real-Time System
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/DataServer/DataServer.cpp $
// 
// 6     4/13/06 4:12a Cward
// removed default parameter value
//
// 2     3/20/06 10:54a Bmeinke
// Removed the default argument value from Run() should appear in the
// header only)
//
// 5     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 4     7/20/05 9:14p Cward
// Added new TaskMon support.
//
// 3     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 2     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 1     12/21/03 6:42p Cward
//
// 2     10/24/03 1:32p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:33a Khykin
// Initial checkin
//
// 1     7/08/03 12:58p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:42p Khykin
// Initial checkin.
//
// 2     5/29/03 6:19a Khykin
// Updated for Shutdown strategy.
//
// 32    2/07/03 4:34p Tbochene
// Fixed compile problem
//
// 31    2/05/03 1:30p Tbochene
// Added sending of Shutdown command when GUI sends terminate
//
// 30    10/16/02 3:35p Tbochene
// Added retry in Register() so when opening socket fails the process does
// not terminate.
//
// 29    9/10/02 6:06p Tbochene
// Removed delay from Register() method
//
// 28    8/23/02 8:03a Dan.erickson
// Added 10 second sleep before registering DataServer.
//
// 27    6/25/02 5:02p Skypig13
// Removed compiler warning in Register() by adding return value
//
// 26    6/19/02 2:06p Skypig13
// New with Register()
//
// 25    5/30/02 5:03p Skypig13
// Latest stuff that compiles
//
// 24    4/25/02 4:38p Skypig13
// Updated with new changes
//    Revision 1.4  2002/04/12 22:16:28  erickson
//    comments
//
//    Revision 1.3  2002/04/11 19:19:34  erickson
//    Updated ThreadPoolBase to properly use muticies and updated tests for INamedDataBroker.  Data server runs and SIGSEGV when the Subscription manager receives a subscription request and attemtps to create the first threads in the  Notification Thread Pool (first ContextAllocate()).
//
//    Revision 1.2  2002/04/08 19:53:58  erickson
//    small formatting tweak
//
//    Revision 1.1.1.1  2002/04/08 19:49:03  erickson
//    started
//
//
// 22    4/08/02 1:15p Skypig13
// New structure
//
// 21    4/02/02 10:53p Skypig13
// New Data server tested and working
//
// 20    2/26/02 2:05p Skypig13
// Works with a fd leak
//
// 19    2/07/02 6:27p Skypig13
// Updated pintouts and initialize of Named Data Broker Object
//
// 18    2/05/02 1:43p Skypig13
// Removed warnings and clutter
//
// 17    2/05/02 12:20a Skypig13
// Update to new BepCommunication structure
//
// 16    1/10/02 12:55p Skypig13
// Updated subscriber functions
//
// 15    12/19/01 11:18p Skypig13
// Updated BeASubscriber() to make it play nicer
//
// 14    12/19/01 10:56p Skypig13
// Added unsubscribe to exit handler for BeASubscriber()
//
// 13    12/19/01 3:24p Skypig13
// Subscribe() function working
//
// 11    12/13/01 10:29a Skypig13
// Changed printf to Log() in Read() and Write()
//
// 10    12/13/01 1:07a Skypig13
// Serveral updates to diagnostics and added Run()
//
// 9     11/15/01 8:27p Skypig13
// Updates
//
// 8     11/08/01 9:35p Skypig13
// Update Subscribe to not use XmlNodes
//
// 7     10/29/01 1:25p Skypig13
// Removed m_INdbConfig and made all INamedDataBroker objects
// use Initialize with individual parameters instead of XML node
//
// 6     10/25/01 9:55p Skypig13
// Working with TCP/IP streaming data
//
// 5     10/17/01 10:59a Skypig13
// Latest version working (without streaming reads)
//
// 4     10/16/01 4:19p Skypig13
// Clean up printouts
//
// 3     10/16/01 1:31a Skypig13
// Took out a few annoying printf's and macro preprocessor defines
//
// 2     10/12/01 11:24a Skypig13
// Latest files for DataServer
//
// 1     10/05/01 4:31p Skypig13
// Data server for connection from GUI to RTOS
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================

#include "DataServer.h"
#include "SubscriptionManager.h"
#include "IActiveClientCommunicationThreadPool.h"

DataServer::DataServer(void) : BepServer(), m_commConfig(NULL), m_ndb(NULL)
{
}

DataServer::~DataServer(void)
{									   // Clean up memory
	if(m_commConfig) delete m_commConfig;m_commConfig = NULL;
	SubscriptionManager::GetManager()->DeleteManager();
	if(m_ndb != NULL)  delete m_ndb;
	m_ndb = NULL;
}
void DataServer::Initialize(const XmlNode *document)
{									   // Initialize base class
	try
	{
		if(document) SetDebug(document->getChild("DataServer/Setup/Debug")->getValue());
	}
	catch(...)
	{
	}
	Log(LOG_FN_ENTRY, "DataServer::Initialize(%s)\n", document->ToString().c_str());
	BepComponent::Initialize(document->getChild("DataServer"));	   // Initialize the base class component (don't use BepServer so thread pool does not get started)

	// Get the list of ndb tags data server can command
	try
	{
		m_ndbCommandTags.DeepCopy(document->getChild("DataServer/Setup/NdbCommandTags")->getChildren());
	}
	catch(...)
	{	// Could not get list of valid part numbers
		Log(LOG_ERRORS,"XML Exception loading NDB Command Tags List\n");
		m_ndbCommandTags.clear(true);
	}

	// Get the list of ndb tags the data server can write
	try
	{
		m_ndbWriteTags.DeepCopy(document->getChild("DataServer/Setup/NdbWriteTags")->getChildren());
	}
	catch(XmlException &excpt)
	{	// Could not get list of valid part numbers
		Log(LOG_ERRORS,"XML Exception loading NDB write Tags List: %s", excpt.GetReason());
		m_ndbWriteTags.clear(true);
	}
	m_comm = new IActiveClientCommunicationThreadPool(this);	// Create a thread pool for TCP connections
	m_commConfig = document->getChild("DataServer")->Copy();	// Save configuration for communication

	SubscriptionManager::GetManager()->Initialize(document->getChild("SubscriptionManager"));

	RegisterWithTaskMon();

	Log(LOG_FN_ENTRY, "DataServer::Initialize(%s) done\n", document->ToString().c_str());
}

const std::string DataServer::Command(const XmlNode *node)
{

	std::string status("Not Supported");	// set the status to invalid
	string response(BEP_NO_DATA);

	string tag = node->getName();
	string tagValue = node->getValue();

	Log(LOG_FN_ENTRY, "DataServer::Command() - Tag = %s, Tag Value = %s\n", tag.c_str(),tagValue.c_str());
	//determine if tag is in list of tags that can be commanded
	XmlNodeMapItr iter = m_ndbCommandTags.find(tag);
	if((tag == SYSTEM_RESTART_DATA_TAG))
	{//Data server configured to handle tag - write to ndb
		if(m_ndb != NULL)
		{
			Log(LOG_DEV_DATA, "DataServer::Command() - Shutting down system\n");
			m_ndb->Command(tag,tagValue,response,true);
			status = "Success";
		}
		else
		{
			Log(LOG_ERRORS, "DataServer::Command() - NDB comm object does not exist, cannot command system to do anything.");
			status = BEP_SOFTWAREFAIL_RESPONSE;
		}
	}
	else if((tag == CLIENT_CLOSED_DATA_TAG))
	{//Client has been closed, clean up Active Clients
		if(m_comm != NULL)
		{
			IActiveClientCommunicationThreadPool *clientComm = (IActiveClientCommunicationThreadPool*) m_comm;
			clientComm->RemoveActiveComms();
		}
	}
	else if((iter != m_ndbCommandTags.end()))
	{//Data server configured to handle tag - write to ndb - client feedback causing write to success
		if(tagValue != BEP_SUCCESS_RESPONSE)
		{
			if(m_ndb != NULL)
			{
				m_ndb->Command(tag,tagValue,response,true);
				//BepServer::Write(tag,tagValue);
				status = "Success";
			}
			else
			{
				Log(LOG_ERRORS, "DataServer::Command() - NDB comm object does not exist, cannot command system to do anything.");
				status = BEP_SOFTWAREFAIL_RESPONSE;
			}
		}
		else
		{
			status = "Success";
		}
	}
	else if(iter == m_ndbCommandTags.end())
	{	// Check if this is a writeable item
		if(m_ndbWriteTags.find(tag) != m_ndbWriteTags.end())
		{
			if(tagValue != BEP_SUCCESS_RESPONSE)
			{
				if(m_ndb != NULL)
				{
					Log(LOG_DEV_DATA, "DataServer::Command() - write NDB data");
					m_ndb->Write(tag,tagValue,response,true);
					status = "Success";
				}
				else
				{
					Log(LOG_ERRORS, "DataServer::Command() - NDB comm object does not exist, cannot write system to do anything.");
					status = BEP_SOFTWAREFAIL_RESPONSE;
				}
			}
		}
	}

	Log(LOG_FN_ENTRY, "DataServer::Command() - Exit, %s\n", status.c_str());

	return(status);
}

void DataServer::Terminate(void)
{
	std::string response, tag, value;
	Log(LOG_FN_ENTRY, "DataServer::Terminate()\n");
	SubscriptionManager::GetManager()->Reset();

	SubscriptionManager::GetManager()->Terminate();

	if(m_ndb != NULL)
	{
		m_ndb->Command(SYSTEM_RESTART_DATA_TAG, std::string("0"), response, true);
		m_ndb->GetNext(tag, value, response);
	}
	else
	{
		Log(LOG_ERRORS, "DataServer::Terminate() - NDB comm object does not exist, not setting restart flag to 0");
	}

	Log(LOG_FN_ENTRY, "DataServer::Terminate() done\n");
}
const std::string DataServer::Read(const XmlNode *node, const INT32 rate)	 /*= 0*/
{
	Log(LOG_DETAILED_DATA, "DataServer::Read() - Enter (Xmlnode: %s, Rate: %d)\n", node->ToString().c_str(), rate);
	std::string result(BEP_ERROR_RESPONSE);
	if("GUIKeepAlive" != node->getName())
	{
		// Call base class to handle read
		result = BepServer::Read(node,rate);
	}
	else
	{
		Log(LOG_DEV_DATA, "BepServer::Read() - Received GUIKeepAlive Message, returning 1\n");
		result = "1";
	}
	Log(LOG_DETAILED_DATA, "DataServer::Read() - Exit, result = %s\n",result.c_str());
	return(result);
}
const std::string DataServer::Write(const XmlNode *node)
{
	std::string response, value, tag;
	Log(LOG_FN_ENTRY, "DataServer::Write(%s)\n", tag.c_str());
	if(m_ndb != NULL)
	{
		m_ndb->Write(node, response, true);
		m_ndb->GetNext(tag, value, response);
	}
	else
	{
		Log(LOG_ERRORS, "DataServer::Write() - NDB comm object does not exist, not writing data");
		value = BEP_SOFTWAREFAIL_RESPONSE;
	}
	return(value);
}
void DataServer::Run(volatile bool *terminateFlag)
{
	while(SubscriptionManager::GetManager()->GetStatus() != std::string(BEP_RUNNING_STATUS))
	{								   // Wait until subscription manager has registered
		BposSleep(1000);
	}
	Register();
	SubscriptionManager::GetManager()->Run();	 // Let subscription manager handle when to end
	SubscriptionManager::DeleteManager();	 // Get rid of memory
}
const std::string DataServer::Subscribe(const XmlNode *node)
{
	return(BEP_SUCCESS_RESPONSE);
}
const std::string DataServer::Register(void)
{
	Log(LOG_FN_ENTRY, "DataServer::Register()\n");
	bool Done = false;				   // If we are ready to go
	// Create a new NDB comm object
	if(m_ndb != NULL)  delete m_ndb;
	m_ndb = NULL;
	m_ndb = new INamedDataBroker();
	do
	{
		try
		{	// Inititialize thread pool
			m_comm->Initialize(m_commConfig);	 // Initialize thread pool
			m_comm->StartThreadPool(reinterpret_cast<void *>(m_comm));	  // Start thread pool
			Log(LOG_FN_ENTRY, "DataServer::Register() done\n");
			Done = true;
		}
		catch(BepException& Err)
		{	// If it failed wait for 5 seconds and retry
			Log(LOG_FN_ENTRY, "Waiting to register DataServer because: %s\n", Err.what());
			BposSleep(5000);
		}
		catch(...)
		{	// If we got here it is bad, just fail
			Log(LOG_FN_ENTRY, "Register failed for unknown reason\n");
			Done = true;
		}
	}
	while(!Done);

	ReportTaskUp();

	return(BEP_SUCCESS_RESPONSE);
}

const std::string DataServer::Publish(const XmlNode *node)
{	// Just return success
	return(BEP_SUCCESS_RESPONSE);
}
