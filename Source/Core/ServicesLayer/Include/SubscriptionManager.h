//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/SubscriptionManager.h 4     7/20/05 8:52p Cward $
//
// FILE DESCRIPTION:
//		This is a class which handles subscriptions for an active client.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/SubscriptionManager.h $
// 
// 4     7/20/05 8:52p Cward
// Turned on using new thread pools
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
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:24p Khykin
// Initial checkin.
//
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:47  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:10a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 11    11/19/02 9:05p Tbochene
// Added Read() so subscriber information can be read, updated TagSet to
// be a map
//
// 10    9/12/02 5:48p Tbochene
// Removed multi-threaded interface to RTOS
//
// 9     9/10/02 6:35p Tbochene
// Removed all conditional compile stuff
//
// 8     6/19/02 2:49p Skypig13
// Added Register() method
//
// 7     5/30/02 5:04p Skypig13
// Latest stuff that compiles
//
// 6     4/25/02 4:42p Skypig13
// Updated with new changes
//    Revision 1.3  2002/04/12 02:34:08  erickson
//    Updated thread pool base and component to take #define types for thread pool handle and parameters instead of void
//
//    Revision 1.2  2002/04/11 19:19:34  erickson
//    Updated ThreadPoolBase to properly use muticies and updated tests for INamedDataBroker.  Data server runs and SIGSEGV when the Subscription manager receives a subscription request and attemtps to create the first threads in the  Notification Thread Pool (first ContextAllocate()).
//
//    Revision 1.1.1.1  2002/04/08 19:49:03  erickson
//    started
//
//
// 3     4/08/02 1:45p Skypig13
// Added header
//
//*************************************************************************
#ifndef SubscriptionManager_h
#define SubscriptionManager_h

class IPublish;
class IBepCommunication;

#include <map>

#include "BepComponent.h"

#define NEW_TP (1)

#if (NEW_TP)
	#include "TemplateIBepCommunicationThreadPool.h"
#else
	#include "IBepCommunication.h"
#endif // NEW_TP

/**
 * Subscription manager class responsible for managing
 * publications from an IBepCommunication channel and sending response
 * to another IBepCommunication channel
 *
 * @author Carl Erickson
 * @author Tim Bochenek
 * @version 1.4
 * @since Version 1.0
 */
class SubscriptionManager : public BepComponent
{
	friend class SubscriptionManagerTest;
private:
	/**
	 * Class constructor
	 * @since Version 1.0
	 */
	SubscriptionManager(void);
	/**
	 * Class destructor
	 * @since Version 1.0
	 */
	virtual ~SubscriptionManager(void);
	/**
	 * One and only instance of the subscription manager
	 * @since Version 1.0
	 */
	static SubscriptionManager* manager;
#if (NEW_TP)
	#warning using new thread pool

	/**
	 * Communications object for server communications
	 * @since Version 1.2
	 */
	TemplateIBepCommunicationThreadPool* m_comm;
public:
#else
	#warning not using new thread pool
	/**
	 * Ids of threads to use for communications
	 * @since Version 1.2
	 */
	std::vector<pthread_t> m_thread;
public:
	/**
	 * Communications object for server communications
	 * @since Version 1.2
	 */
	IBepCommunication* m_comm;
	/**
	 * Method used to handle requests from servers
	 *
	 * @param args   Arguments to pass to set up thread
	 * @return Results (will be NULL)
	 * @since Version 1.2
	 */
	static void* SubscriptionManager::ThreadMethod(void *args);
#endif // NEW_TP

	/**
	 * Accessor method to get reference to the subscription manager
	 *
	 * @return The subscription manager
	 * @since Version 1.0
	 */
	static SubscriptionManager* GetManager(void);
	/**
	 * Add a subscriber to the subsciption managers list
	 *
	 * @param data       Data to be subscribed for
	 * @param subscriber Object handling the subscription
	 */
	void AddSubscriber(const XmlNode* data, IPublish *subscriber);
	/**
	 * Setup method
	 *
	 * @param document Xml configuration used to setup object
	 * @since Version 1.0
	 */
	virtual void Initialize(const XmlNode *document);
	/**
	 * Request to read a data tag
	 *
	 * @param node    Data to be read
	 * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
	 * @return Value of tag
	 * @since Version 2.0
	 */
	virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
	/**
	 * Method used to set all of the objects in the component
	 * to their original states.  This would includes all
	 * member variables such as buffers, lists, and stored
	 * data.
	 * @since Version 1.0
	 */
	virtual void Reset(void);
	/**
	 * Method used to terminate the component.  This performs
	 * any cleanup that the destructor would not and sets
	 * the state of the component to terminate.
	 * @since Version 1.0
	 */
	virtual void Terminate(void);
	/**
	 * Handle the subscribed for data by the process.
	 *
	 * @param node    Tag and data that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 * @since Version 1.0
	 */
	virtual const std::string Publish(const XmlNode *node);
	/**
	 * Method that is called to command the component to perform any
	 * system registration functions necessary.
	 *
	 * @return Status of the registration process.
	 *         Success, Fail
	 * @since Version 2.5
	 */
	virtual const std::string Register(void);
	/**
	 * Get the named channel the object is using.
	 *
	 * @return Name of the channel (IPC only).
	 * @since Version 1.0
	 */
	const std::string GetName(void);
	/**
	 * Main run method that subclasses can overload if different
	 * behavior is desired.
	 *
	 * @param terminateFlag
	 *               Flag used to terminate the system.
	 * @since Version 2.4
	 */
	virtual void Run(volatile bool *terminateFlag=NULL);
	/**
	 * Delte the singleton instance of the Subscription Manager
	 * @since Version 1.1
	 */
	static void DeleteManager(void);

private:
	/**
	 * Name of communication channel
	 * @since Version 1.0
	 */
	std::string m_name;
	/**
	 * Mutex lock variable for node map data.
	 * @since Version 2.3
	 */
	mutable pthread_mutex_t m_tagDataLock;
	/**
	 * Condition variable set to indicate the server should terminate
	 * @since Version 2.6
	 */
	pthread_cond_t m_endVar;
	/**
	 * Container of sockets
	 * @since Version 1.4
	 */
	typedef std::vector<UINT32> SocketVector;
	/**
	 * Data type definition for contianer of tags being subscribed for and sockets they are connected to
	 * @since Version 1.0
	 */
	typedef std::map<std::string, SocketVector, less<std::string> > TagSet;
	/**
	 * Data type definition for iterator across contianer of tags being subscribed for
	 * @since Version 1.0
	 */
	typedef TagSet::iterator TagSetIter;
	/**
	 * Data type definition for a value stored in a contianer of tags being subscribed for
	 * @since Version 1.0
	 */
	typedef TagSet::value_type TagSetVal;
	/**
	 * List of tags being managed by the subscription manager
	 * @since Version 1.3
	 */
	TagSet m_tagList;
};

#endif // SubscriptionManager_h
