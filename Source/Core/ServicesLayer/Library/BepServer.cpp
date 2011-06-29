//*************************************************************************
// Description:
//   Base server for all iterative servers.
//
//===========================================================================
// Copyright (c) 2001-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/BepServer.cpp $
// 
// 10    2/22/07 11:02a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Removed #if 0 blocks.
// - Added a separator between the methods.
// 
// 9     1/18/07 12:46p Rwiersem
// Changes for the 07011901 core release:
// 
// - Made the mutex attribute variable a member variable so that it can be
// properly destroyed.
// 
// 8     10/26/06 9:55a Rwiersem
// Changes for the 06102701 core release:
// 
// Added Activate() and Deactivate().  These are used to keep the test 
// components running when a new drive curve starts.
// 
// ReloadConfiguration() now calls Initialize().
// 
// 7     7/20/05 8:56p Cward
// Fixed bug that was causing SIG SEG in the destructor. Added support for
// new TaskMon
//
// 6     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 5     4/29/05 8:00a Gswope
// Removed unused constructor: BepServer(argc,argv)
//
// 5     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 4     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 3     10/24/03 12:04p Khykin
// Updated with latest mutex changes.
//
// 2     10/10/03 9:32a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 2     8/15/03 2:09p Khykin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 3     6/26/03 12:53p Khykin
// Changed the return value for the base Command() method to unsupported.
//
// 2     6/20/03 7:37a Khykin
// Adding protection for a potentially bad lookup.
//
// 1     4/22/03 9:32a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:21a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:40  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:25a Khykin
// Initial Checkin
//
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
//
// 68    10/07/02 1:04p Derickso
// Removed delete of m_comm again.
// It is causing problems during TestResultServer testing.
//
// 67    9/12/02 5:45p Tbochene
// Updated logging to include CR
//
// 66    9/10/02 5:49p Tbochene
// Put delete of m_comm in destructor
//
// 65    8/23/02 7:55a Dan.erickson
// Added verbosity levels to Log calls.
//
// 64    7/31/02 10:46a Skypig13
// Changes due to notification list prototype changes and added
// WriteSubscribeData()
//
// 63    6/19/02 1:49p Skypig13
// Added accessor methods for checking the registered state
//
// 62    6/07/02 11:48a Dan.erickson
// Created m_registered ivar.
// Pushed mutex management down into SetData and GetData methods.
// Added logic to delete m_comm in the Terminate method.
//
// 61    5/06/02 3:47p Dan.erickson
// In SetData(), delNode() does not free the memory associated with
// the node.  Explicitly delete the memory and then get rid of the
// pointer.
//
// 59    4/24/02 3:10p Dan.erickson
// Updated SetData() method to remove the node if it already
// exists in the map and to always add the data node to the map.
//
// 58    4/05/02 2:30p Dan.erickson
// In Write() method, make sure m_nList is not NULL before we attempt to
// call the Notify() method.
//
// 57    4/02/02 10:39p Skypig13
// Put warning in destructor
//
// 56    3/20/02 9:03a Dan.erickson
// Added Initialize method that takes a string as an argument.
// Reverted Subscribe tags processing to get the name of the node.
// Removed deletion of m_comm object in destructor.  This was
// causing the system to hang when some of the servers Terminated.
// QNX recovers memory that was allocated during the process run time.
//
// 55    3/12/02 11:57a Dan.erickson
// Modified LoadConfiguredTags().  Retrieval of Subscribe tags was
// getting the name of the node instead of the value.
//
// 54    2/25/02 8:48p Skypig13
// Updated Initialize()
//
// 53    2/20/02 12:04p Skypig13
// Enabled prints for Publications
//
// 52    2/20/02 11:17a Dan.erickson
// Moved unsubscribe stuff from ReLoadConfiguration to
// UnsubscribeTags and added call to unsubscribeTags to
// Terminate method.
//
// 51    2/19/02 5:49p Skypig13
// Fixed Terminate() sequence
//
// 50    2/19/02 4:36p Skypig13
// Small changes improving performance
//
// 49    2/18/02 7:28p Dan.erickson
// Signal condition variable to exit after HandlePulse.
//
// 48    2/18/02 10:08a Skypig13
// Updated Initialize() and Subscribe()
//
// 47    2/12/02 5:09p Dan.erickson
// Modified Initialize method to take a pointer to a ThreadPool object.
// Created XmlNode versions of many methods.
//
// 46    2/07/02 6:24p Skypig13
// Move LoadConfiguredTags() before thread pool initialization in
// Initialize().
//
// 45    2/05/02 1:42p Skypig13
// Removed warnings
//
// 44    2/04/02 11:37p Skypig13
// Restructured to remove multiple inheritence and include thread pool
// aggregation
//
// 43    1/22/02 8:05p Kh
// Updating the HandlePulse method.
//
// 42    1/18/02 8:21a Kh
// Making change to Register method.
//
// 40    1/16/02 9:21a Dan.erickson
// Modified call to initialize for IPC objects in the Block
// method.  The call to initialize was using DEFAULT_BUFFER_SIZE
// when it should have been using the buffer size of the server
// when initializing a worker object.
//
// 39    1/10/02 3:40p Dan.erickson
// Modified ManageNotifyThreads method.
// Changed != to < in for loop since the != was causing the
// server to SigSeg when the method was called with killAll set
// to true.
//
// 38    1/10/02 11:52a Skypig13
// Updated to use vector of Notify threads to handle publications.
// The original code was a memory leak.
//
// 37    1/07/02 5:19p Dan.erickson
// Debugged ReloadConfiguration method.
// Moved TagList processing to LoadConfiguredTags method.
//
// 36    1/04/02 11:11a Dan.erickson
// Added ReadSubscribeTag method.
// Moved server configuration data under Setup node.
//
// 35    1/04/02 8:16a Kh
// Making change to record the data from the Register method.
//
// 34    1/03/02 4:14p Dan.erickson
// Modified decrement logic when subscribing for tags in Register
// method.
// Added code to update the value of the tag if the
// subscribe returned valid data in Register method.
//
// 33    12/18/01 5:00p Skypig13
// Added return value to Publish()
//
// 32    12/18/01 11:36a Kh
// Updating the Register method to make multiple attempts to subscribe
//
// 31    12/14/01 5:44p Kh
// Adding a Resigter method to deal with subscription issues.
//
// 30    12/13/01 1:05a Skypig13
// Move Run() from header file
//
// 29    12/12/01 11:07p Skypig13
// Update Terminate() method and Run() method for better reliability
//
// 27    12/05/01 11:49a Skypig13
// Changed ReLoadConfiguration() to ReloadConfiguration() on line 319
//
// 26    11/30/01 1:22p Dan.erickson
// Modified to keep a list of tags that have been subscribed
// for as well as a list of tags the server supports.
// Extended the ReloadConfiguration method to unsubscribe for any
// tags that were previously subscribed for.
//
// 25    11/29/01 5:52p Dan.erickson
// Modified Subscribe method to send back " " instead of
// Unavailable if the server is not using configured tags and
// the tag has not yet been written to the server.
//
// 24    11/29/01 3:01p Skypig13
// Changed Subscribe to return data instead of Success
//
// 23    11/29/01 8:35a Kh
// Adding Logging to the subscribe method if it fails.
//
// 22    11/20/01 5:35p Dan.erickson
// Added logic to allow server to subscribe for tags if any are listed.
//
// 21    11/15/01 7:11p Skypig13
// Scoped ClearData()
//
// 20    11/15/01 4:17p Skypig13
// Changed to ClearData() in initialize
//
// 19    11/15/01 4:04p Skypig13
// Clear m_data on initialize
//
// 18    11/01/01 2:57p Dan.erickson
// Compiles, not tested.
//
// 17    10/24/01 12:53p Dan.erickson
// Added mutex locks in read and write methods.
// Added PublishTags node under TagList node.
//
// 16    10/17/01 9:54a Skypig13
// Changed prototype of Read() to include data reate
//
// 15    10/12/01 10:26a Dan.erickson
// Modified server to allow initialization with a tag list.
//
// 14    10/04/01 12:13p Skypig13
// Added diagnostic to Handle()
//
// 13    10/04/01 10:18a Skypig13
// Fixed SIGSEGV in TCP/IP version
//
// 12    10/03/01 4:58p Skypig13
// Added Terminate() to destructor and made Initialize more robust with
// try/catch and NULL checks
//
// 11    10/02/01 11:48p Skypig13
// Many changes to the thread handling and terminate sequences
//
// 10    9/28/01 5:04p Skypig13
// Updates to use thread
//
// 9     9/27/01 4:32p Dan.erickson
// Check to ensure ThreadPool node is listed in config file or pass NULL
// into ThreadPoolBase::Initialize method.
//
// 8     9/27/01 3:47p Skypig13
// Derived from ThreadPoolBase and made Initialze() methods throw
// Exeptions out
//
// 7     9/18/01 7:26a Skypig13
// Removed pulse handling for thread death
//
// 6     9/14/01 2:05p Skypig13
// Updating many things.  Latest testing including Notification
//
// 5     9/04/01 9:08p Skypig13
// Added comments
//
// 4     9/01/01 5:05p Skypig13
// Changed due to changes in AtomicThread
//
// 3     8/31/01 3:28p Skypig13
// Latset version for testing
//
// 2     8/30/01 4:13p Skypig13
// Updated using Notify thread
//
// 1     8/30/01 11:15a Skypig13
// New BepSever base class
//
// 1     8/30/01 9:18a Skypig13
// New base server
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================
#include "BepServer.h"
#include "INamedDataBroker.h"
#include "IBepCommunicationThreadPool.h"

//=============================================================================
BepServer::BepServer(void) : BepComponent(), m_comm(NULL), m_tags(false), m_registered(false)
{
	try
	{
		memset( &m_attr, 0, sizeof(m_attr));
		memset( &m_tagDataLock, 0, sizeof( m_tagDataLock));

		pthread_mutexattr_init(&m_attr);
		pthread_mutexattr_settype( &m_attr, PTHREAD_MUTEX_ERRORCHECK);
		pthread_mutexattr_setrecursive(&m_attr, PTHREAD_RECURSIVE_DISABLE);
		pthread_mutex_init(&m_tagDataLock, &m_attr);

		memset( &m_endVar, 0, sizeof( m_endVar));
		pthread_cond_init(&m_endVar, NULL);
		ClearData();	// Make sure data has been purged
	}
	catch(BepException &BepErr)
	{
		Log(LOG_ERRORS, "Exception %s was raised in BepServer::BepServer()\n", BepErr.what());
	}
	catch(...)
	{
		Log(LOG_ERRORS, "Unknown Exception was raised in BepServer::BepServer()\n");
	}
}

//=============================================================================
BepServer::~BepServer(void)
{
	Log(LOG_FN_ENTRY, "BepServer::~BepServer() -- begin\n");

	ClearData();	// Make sure data has been purged

	m_publishTags.clear(true);
	m_dataTags.clear(true);

	// if not previously terminated, terminate
	if(GetStatus() != BEP_TERMINATE_STATUS)	Terminate();
	pthread_mutex_destroy(&m_tagDataLock);
	pthread_mutexattr_destroy(&m_attr);
	pthread_cond_destroy(&m_endVar);

	if(m_comm != NULL)
	{
//      Log(LOG_DEV_DATA, "BepServer::~BepServer() -- Deleting m_comm\n");
		delete m_comm;
//      Log(LOG_DEV_DATA, "BepServer::~BepServer() -- Done deleting m_comm\n");
		m_comm = NULL;
	}

	Log(LOG_FN_ENTRY, "BepServer::~BepServer() -- done\n");
}

//=============================================================================
void BepServer::Initialize(const std::string &fileName)
{
	BepComponent::Initialize(fileName);
}

//=============================================================================
void BepServer::Initialize(const XmlNode *document)
{
	try
	{
		if(document) SetDebug(document->getChildren().getNode("Debug"));
	}
	catch(...)
	{
	}
	Log(LOG_FN_ENTRY, "BepServer::Initialize()");
	try
	{
		if(document)
		{
			BepComponent::Initialize(document);								// Initialize the base class component

			LoadConfiguredTags(document->getChild("Setup"));				// Set up tags to be provided

			try
			{
				// store the data tags (map gets cleared first)
				m_dataTags.DeepCopy(document->getChild("Setup/DataTags")->getChildren());
			}
			catch(...)
			{
			}

			if( GetStatus() != BEP_DEACTIVATE_STATUS)
			{
				if(m_comm == NULL)
				{
					Log(LOG_DEV_DATA, "Creating Thread Pool\n");
					m_comm = new IBepCommunicationThreadPool(this);		// Create communication channel
				}

				if(dynamic_cast<IBepCommunicationThreadPool *>(m_comm) != NULL)
				{
					m_comm->Initialize(document);								// Initialize and start all thread pools
					m_comm->StartThreadPool(reinterpret_cast<void *>(m_comm));	// Start thread pool
				}
				else throw BepException("ThreadPool is not an IBepCommunicationThreadPool object");
			}
		}
		else throw BepException("Configuration node passed is NULL");
	}
	catch(BepException &BepErr)
	{
		Log(LOG_ERRORS, "BepServer::Initialize() failed: %s\n", BepErr.what());
		if(m_comm != NULL) delete m_comm;
		m_comm = NULL;
		throw;
	}
	catch(...)
	{
		Log(LOG_ERRORS, "BepServer::Initialize() failed\n");
		if(m_comm != NULL) delete m_comm;
		m_comm = NULL;
		throw;
	}

	if( GetStatus() != BEP_DEACTIVATE_STATUS)
	{
		RegisterWithTaskMon();
	}

	Log(LOG_FN_ENTRY, "BepServer::Initialize(%s) done\n", document->ToString().c_str());
}

//=============================================================================
void BepServer::ReloadConfiguration(void)
{
	Log(LOG_FN_ENTRY, "BepServer::ReloadConfiguration()\n");
	UnsubscribeTags( true);	// Unsubscribe for any and all tags
	m_publishTags.clear(true); // Clear the maps
	try
	{
		Initialize(GetConfigFile());
		Register();	// Register the server so we subscribe for our tags again
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "BepServer::ReloadConfig Exception: %s\n", e.what());
	}

	Log(LOG_FN_ENTRY, "Exit BepServer::ReloadConfig() done\n");
}

//=============================================================================
void BepServer::Abort(void)
{
}

//=============================================================================
void BepServer::Reset(void)
{
	ReloadConfiguration();
}

//=============================================================================
void BepServer::Terminate(void)
{
	Log(LOG_FN_ENTRY, "BepServer::Terminate()\n");
	UnsubscribeTags();	// Remove subscriptions for any tags
	BepComponent::Terminate();
	ClearData();	// Make sure data has been purged
	pthread_cond_signal(&m_endVar);	// Send signal that we gotta go.
	// Get rid of the communications thread pool
}

//=============================================================================
void BepServer::Activate(void)
{
	Log(LOG_FN_ENTRY,"Enter BepServer::Activate()\n");

	// Subscribe for our tags
	Log( LOG_DEV_DATA, "Subscribing for our tags\n");
	// Subscribe for any and all tags
	SubscribeTags();

	BepComponent::Activate();

	Log(LOG_FN_ENTRY,"Exit BepServer::Activate()\n");
}

//=============================================================================
void BepServer::Deactivate(void)
{
	Log(LOG_FN_ENTRY,"Enter BepServer::Deactivate()\n");

	// Remove subscriptions for any tags
	Log( LOG_DEV_DATA, "Unsubscribing for our tags\n");
	// Unsubscribe for any and all tags
	UnsubscribeTags( false);

	BepComponent::Deactivate();

	Log(LOG_FN_ENTRY,"Exit BepServer::Deactivate()\n");
}

//=============================================================================
const std::string BepServer::Read(const std::string &tag, const INT32 rate /* = 0 */)
{	// Create a node from the data and call the Read method
	XmlNode node(tag,"");
	return(Read(&node, rate));
}

//=============================================================================
const std::string BepServer::Read(const XmlNode *node, const INT32 rate	/*= 0*/)
{
	Log(LOG_DETAILED_DATA, "BepServer::Read(%s, %d)\n", node->ToString().c_str(), rate);
	std::string result(BEP_ERROR_RESPONSE);
	if(FindTag(node->getName())) result = GetData(node->getName());	// Read some stuff
	else result	= BEP_UNSUPPORTED_RESPONSE;
	return(result);
}

//=============================================================================
const std::string BepServer::Write(const std::string &tag, const std::string &value)
{	// Create a node from the data and call the Write method
	XmlNode node(tag, value);
	return(Write(&node));
}

//=============================================================================
const std::string BepServer::Write(const XmlNode *node)
{
	std::string result(BEP_ERROR_RESPONSE);
	if(FindTag(node->getName()))
	{	// Write some stuff
		result = SetData(node);							// Put data in my list
		if(m_nList != NULL)	m_nList->Notify(node);		// Do the notify thing
		else Log(LOG_ERRORS, "Invalid NotificationList in %s or no subscribers\n", GetProcessName().c_str());
	}
	else result	= BEP_UNSUPPORTED_RESPONSE;

	return(result);
}

//=============================================================================
const std::string BepServer::Command(const std::string &tag, const std::string &value)
{	// Create a node from the data and call the command method
	XmlNode node(tag, value);
	return(Command(&node));
}

//=============================================================================
const std::string BepServer::Command(const XmlNode *node)
{
	return(BepComponent::Command( node));
}

//=============================================================================
const std::string BepServer::Subscribe(const std::string &tag, const std::string &Process)
{	// Create a node from the data and call the Subscribe method
	XmlNode node(tag, Process);
	return(Subscribe(&node));
}

//=============================================================================
const std::string BepServer::Subscribe(const XmlNode *node)
{
	std::string result;
	if(FindTag(node->getName()))
	{
		result = BepComponent::Subscribe(node);
		if((result == BEP_UNAVAILABLE_RESPONSE) && !UseConfiguredTags())
		{
			result.erase();
		}
	}
	else result	= BEP_UNSUPPORTED_RESPONSE;
	return(result);
}

//=============================================================================
const std::string BepServer::Register(void)
{
	INT32 status = BEP_STATUS_ERROR;		// the status of the subscribe operation
	status = SubscribeTags();
	// if registered, flag
	if(status == BEP_STATUS_SUCCESS)
		SetRegistered(true);

	BepComponent::Register();

	return((status == BEP_STATUS_SUCCESS) ? BEP_SUCCESS_RESPONSE : BEP_FAILURE_RESPONSE);
}

//=============================================================================
const INT32 BepServer::HandlePulse(const INT32 code, const INT32 value)
{
	Log(LOG_FN_ENTRY, "BepServer::HandlePulse(%d, %d)\n", code, value);
	INT32 stat = BepComponent::HandlePulse(code, value);
	return(stat);
}

//=============================================================================
inline bool BepServer::UseConfiguredTags(void)
{
	return(m_tags);
}

//=============================================================================
void BepServer::Run(volatile bool *terminateFlag /* =NULL */)
{
	if( (errno=pthread_mutex_lock(&m_tagDataLock)) == EOK)
	{
		while(GetStatus() != BEP_TERMINATE)
		{
			pthread_cond_wait(&m_endVar, &m_tagDataLock);
		}
		// I think this is correct KGH to release all thread that are blocked
		pthread_mutex_unlock(&m_tagDataLock);
	}
	else
	{
		Log( LOG_ERRORS, "\tError lockimg m_tagDataLock in BepServer::Run(): %s\n", strerror(errno));
	}
}

//=============================================================================
const std::string BepServer::ReadSubscribeData(const std::string &tag, const INT32 rate	/*=0*/)
{
	return(GetData(tag));
}

//=============================================================================
const std::string BepServer::WriteSubscribeData(const std::string &tag, const std::string &value)
{
	return(SetData(tag, value));
}

//=============================================================================
void BepServer::ClearData(void)
{
	if(pthread_mutex_lock(&m_tagDataLock) == EOK)
	{
		m_data.clear(true);
		pthread_mutex_unlock(&m_tagDataLock);
	}
}

//=============================================================================
const std::string BepServer::GetData(const std::string &tag)
{
	Log(LOG_DETAILED_DATA, "BepServer::GetData(%s)\n", tag.c_str());
	std::string data = BEP_ERROR_RESPONSE;
	if(pthread_mutex_lock(&m_tagDataLock) == EOK)
	{	// Get the subscribed data
		try
		{
			Log(LOG_DETAILED_DATA, "There are %u tags in the map\n", m_data.size());
			data = m_data.getNode(tag)->getValue();
		}
		catch(...)
		{	// The data was not available
			Log(LOG_ERRORS, "BepServer::GetData(%s) was unable to provide requested data\n", tag.c_str());
			data = BEP_UNAVAILABLE_RESPONSE;
		}
		pthread_mutex_unlock(&m_tagDataLock);
	}
	else
		Log(LOG_ERRORS, "BepServer::GetData(%s) was unable to access the requested data\n", tag.c_str());

	Log(LOG_DETAILED_DATA, "BepServer::GetData(%s) done, returning %s\n", tag.c_str(), data.c_str());
	return(data);
}

//=============================================================================
const std::string BepServer::SetData(const std::string &tag, const std::string &value)
{
	XmlNode node(tag, value);
	return(SetData(&node));
}

//=============================================================================
const std::string BepServer::SetData(const XmlNode *dataNode)
{
	std::string printNode;
	try
	{
		dataNode->writeNode(printNode);
		Log(LOG_FN_ENTRY, "BepServer::SetData(%s)\n", printNode.c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "XmlException in BepServer::SetData() - %s", excpt.GetReason());
	}
	std::string response = BEP_ERROR_RESPONSE;
	XmlNodeMapItr iter;
	if(pthread_mutex_lock(&m_tagDataLock) == EOK)
	{
		try
		{	// WARNING: Do Not Change This Due To The Fact Value Can Be A List of Nodes not just a single value
			if((iter = m_data.find(dataNode->getName())) != m_data.end())
			{	// Remove the node so we can easily replace it
				delete iter->second;
				m_data.delNode(dataNode->getName());
			}
			m_data.addNode(dataNode->Copy());		// Insert into table
			response = BEP_SUCCESS_RESPONSE;		// Success!
		}
		catch(...)
		{
			Log(LOG_ERRORS, "BepServer::SetData(%s) was unable to set the requested data\n", printNode.c_str());
			response = BEP_UNAVAILABLE_RESPONSE;
		}
		pthread_mutex_unlock(&m_tagDataLock);
	}
	else
		Log(LOG_ERRORS, "BepServer::SetData(%s) was unable to access the requested data\n", printNode.c_str());

	Log(LOG_FN_ENTRY, "BepServer::SetData(%s) done, returning %s\n", printNode.c_str(), response.c_str());
	return(response);
}

//=============================================================================
void BepServer::SetConfiguredTags(const bool useConfiguredTags)
{
	m_tags = useConfiguredTags;
}

//=============================================================================
const std::string BepServer::Publish(const std::string &tag, const std::string &value)
{	// Build a node from the data and call the Publish method
	XmlNode node(tag, value);
	return(Publish(&node));
}

//=============================================================================
const std::string BepServer::Publish(const XmlNode *node)
{
	try
	{
		Log(LOG_FN_ENTRY, "BepServer::Publish(%s)\n", node->getName().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "XmlException in BepServer::Publish() - %s", excpt.GetReason());
	}
	return(SetData(node));
}

//=============================================================================
inline bool BepServer::FindTag(const std::string &tag)
{
	bool found = false;
	if(UseConfiguredTags())	   // If server configured with tags, make sure this is a supported tag
	{
		if(m_publishTags.find(tag) != m_publishTags.end()) found = true;
	}
	else found = true;			  // Server was not configured with tags, all are valid
	return(found);
}

//=============================================================================
void BepServer::LoadConfiguredTags(const XmlNode *setup)
{
	try
	{
		SetConfiguredTags(true);
		const XmlNodeMap &tagList = setup->getChild("TagList/PublishTags")->getChildren();
		Log(LOG_DEV_DATA, "Retrieved Publish tags\n");
		BepServer::ClearData();
		Log(LOG_DEV_DATA, "Cleared data from all lists\n");
		XmlNodeMapCItr iter = tagList.begin();
		for(; iter != tagList.end(); iter++)
		{	// Place the tag in the data map and the list of tags we support
			if(pthread_mutex_lock(&m_tagDataLock) == EOK)
			{
				m_data.addNode((*iter).second->Copy());
				pthread_mutex_unlock(&m_tagDataLock);
			}
			m_publishTags.addNode((*iter).second->Copy());
		}
		Log(LOG_DEV_DATA, "Stored all Publish Tags\n");
	}
	catch(...)
	{
		SetConfiguredTags(false);
	}
	try
	{	// Subscribe for any tags listed
		Log(LOG_DEV_DATA, "Preparing to deal with subscribe tags: %s\n", setup->ToString().c_str());
		const XmlNodeMap &subscribeTags = setup->getChild("TagList/SubscribeTags")->getChildren();
		Log(LOG_DEV_DATA, "Retrieved Subscribe Tags\n");
		try
		{
			for(XmlNodeMapCItr iter = subscribeTags.begin(); iter != subscribeTags.end(); iter++)
			{	// Place the tag into the subscribe tags list

// NOTE: if the following line has a compiling error for '_Alty' is protected, the file
//       /usr/qnx630/target/qnx6/usr/include/cpp/list needs to be changed.  Change the
//       "protected:" to "public:" above
//              typedef typename _Alloc::template rebind<_Ty>::other _Alty;
				m_subscribeTags.push_back((*iter).second->getName());
			}
			Log(LOG_DEV_DATA, "Stored all Subscribe Tags\n");
		}
		catch(BepException &Err)
		{
			Log(LOG_ERRORS, "BepServer Error Subscribing For Tags: %s\n", Err.what());
		}
	}
	catch(...)
	{	// Do nothing.  Subscribe tags do not need to be declared
		Log(LOG_ERRORS, "No SubscribeTags node found\n");
	}
}

//=============================================================================
void BepServer::UnsubscribeTags(bool clearList)
{	// Unsubscribe for any tags that have been subscribed for if registered
	if(IsRegistered())
	{
		INamedDataBroker broker(IsDebugOn(), DEFAULT_BUFFER_SIZE, 10000);
		std::string response;

		SubscriptionListIter itr = m_subscribeTags.begin();
		for( itr=m_subscribeTags.begin(); itr!=m_subscribeTags.end(); itr++)
		{
			const std::string &tagName = *itr;
			Log(LOG_DEV_DATA, "Unsubscribing from tag: %s", tagName.c_str());
			if(!tagName.empty())
			{
				broker.Unsubscribe(tagName, m_comm->GetName(), response, true);
			}
		}
		if( true == clearList)
		{
			m_subscribeTags.clear();
		}
	}
}

//=============================================================================
INT32 BepServer::SubscribeTags(void)
{
	INT32 status = BEP_STATUS_ERROR;		// the status of the subscribe operation

	if(m_subscribeTags.size() > 0)
	{
		// create a broker interface to subscribe with
		INamedDataBroker broker(IsDebugOn(), DEFAULT_BUFFER_SIZE, 10000);
		std::string response;					// response buffer
		INT32 retries = 0;						// retry counter
		do
		{
			int numTags = m_subscribeTags.size()-1;	// total number of tags

			Log(LOG_DEV_DATA, "Number Of Tags: %d, %d\n", numTags+1, status);
			for(SubscriptionListIter iter = m_subscribeTags.begin(); iter != m_subscribeTags.end(); iter++)
			{
				// Place the tag into the subscribe tags list
//                status = broker.Subscribe((*iter), m_comm->GetName(), response, (numTags-- == 0));
				Log(LOG_DEV_DATA, "Subscribing for %s", (*iter).c_str());
				status = broker.Subscribe((*iter), m_comm->GetName(), response, true);
				// update the subscription information
				std::string tag, value;
				broker.GetNext(tag, value, response);
				// Add the subscribe data to the map
				if(m_comm->IsDataOk(value))
				{
					if(pthread_mutex_lock(&m_tagDataLock) == EOK)
					{
						m_data.addNode(new XmlNode(tag, value));
						pthread_mutex_unlock(&m_tagDataLock);
					}
					else
					{
						Log(LOG_ERRORS, "Could not lock mutex to set subscribed data");
					}
				}
				else Log(LOG_ERRORS, "%s Could Not Subscribe For %s\n", GetProcessName().c_str(), tag.c_str());
			}
			Log(LOG_DEV_DATA, "BepServer::SubscribeTags() %d\n", status);
		}
		while((status != BEP_STATUS_SUCCESS) && (retries++ < 10));
	}
	else status	= BEP_STATUS_SUCCESS;

	return( status);
}

//=============================================================================
const bool BepServer::IsRegistered(void)
{
	return(m_registered);
}

//=============================================================================
void BepServer::SetRegistered(const bool registered)
{
	m_registered = registered;
}

//=============================================================================
const std::string BepServer::GetDataTag(const std::string dataTag)
{
	std::string value;
	try
	{
		value = m_dataTags.getNode(dataTag)->getValue();
	}
	catch(XmlException &e)
	{
		Log(LOG_ERRORS, "Invalid Data Tag Requested: %s, %s\n", dataTag.c_str(), e.what());
		value.erase();
		value = dataTag;
	}
	catch(...)
	{
		Log(LOG_ERRORS, "Unsupported Data Tag: %s\n", dataTag.c_str());
		value.erase();
		value = dataTag;
	}

	return(value);
}
