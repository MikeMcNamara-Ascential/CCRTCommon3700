//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/TemplateThreadPool.h 2     4/13/06 3:38a Cward $
//
// FILE DESCRIPTION:
//   Thread pool class
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/TemplateThreadPool.h $
// 
// 2     4/13/06 3:38a Cward
// Init some vars
//
// 2     2/15/06 3:52p Bmeinke
// Initialize some uninitialized variables
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 11:59a Khykin
// Updated with mutex changes.
//
// 2     10/10/03 9:31a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:25p Khykin
// Initial checkin.
//
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:48  tbochene
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
// 6     10/10/02 11:38a Khykin
// Updated the thread pool in the Housekeeping method HandleDone state to
// validate against the HighWater limit instead of the low water limit.
// Without the change the thread pool was creating and destroying every
// cycle.
//
// 5     9/12/02 5:47p Tbochene
// Updated to derive from ILogger
//
// 4     9/10/02 6:36p Tbochene
// Updated to allow thread recycling
//
// 3     7/31/02 11:54a Skypig13
// Added print in catch
//
// 2     6/19/02 2:51p Skypig13
// Updated Initialize() method and added NULL check for configuration
//
// 1     6/03/02 2:54p Skypig13
// Thread pool class to potentially replace ThreadPoolBase
//
//*************************************************************************

#ifndef TemplateThreadPool_h
#define TemplateThreadPool_h

#include <map>
#include <signal.h>
#include <pthread.h>

#include "XmlNode.h"
#include "Debuggable.h"
#include "IThreadPool.h"
#include "AbstractThread.h"

/**
 * String representing parameter used to set up low water value
 * @since Version 1.0
 */
#define LowWaterString "LowWater"

/**
 * String representing parameter used to set up high water value
 * @since Version 1.0
 */
#define HighWaterString "HighWater"

/**
 * String representing parameter used to set up maximum number of threads
 * @since Version 1.0
 */
#define MaximumString "Maximum"

/**
 * String representing parameter used to set up increment value
 * @since Version 1.0
 */
#define IncrementString "Increment"

/**
 * String representing parameter used to start thread pool when created or not
 * @since Version 1.0
 */
#define StartOnCreateString "StartOnCreate"

using namespace std;

class AbstractThread;
class TemplateThreadPoolTester;

/**
 * Data type definition of thread map type
 * @since Version 1.0
 */
typedef std::map<pthread_t, AbstractThread*> ThreadMapType;
/**
 * Data type definition of iterator for Thread Map
 * @since Version 1.0
 */
typedef ThreadMapType::iterator ThreadMapIter;
/**
 * Data type definition of iterator for Thread Map
 * @since Version 1.0
 */
typedef ThreadMapType::value_type ThreadMapVal;
/**
 * Thread container class
 *
 * @author Tim Bochenek
 * @version 1.1
 * @see AbstractThread
 * @see std::map
 * @since Version 1.0
 */
class ThreadMap : public ThreadMapType
{
public:
	/**
	 * Class constructor
	 *
	 * @return Name of the map
	 * @since Version 1.0
	 */
	ThreadMap(const std::string name) : ThreadMapType(), m_name(name)
	{
	}
	/**
	 * Class destructor
	 *
	 * @since Version 1.0
	 */
	virtual ~ThreadMap(void)
	{
	}
	/**
	 * Returns the name of the container
	 *
	 * @return Name of container
	 * @since Version 1.0
	 */
	inline const std::string& GetName(void)
	{
		return(m_name);
	}
	/**
	 * Inserts a thread into the map
	 *
	 * @param iter iterator to insert
	 * @since Version 1.0
	 */
	inline void Insert(ThreadMapIter& iter)
	{
		Insert(iter->second);
	}
	/**
	 * Inserts a thread into the map
	 *
	 * @param thread Thread object
	 * @since Version 1.0
	 */
	inline void Insert(AbstractThread* thread)
	{
		insert(ThreadMapVal(thread->GetId(), thread));
	}
	/**
	 * Removes a thread from the map
	 *
	 * @param iter iterator to remove from map
	 * @since Version 1.0
	 */
	inline void Remove(ThreadMapIter& iter)
	{
		Remove(iter->second);
	}
	/**
	 * Removes a thread from the map
	 *
	 * @param thread Thread object
	 * @since Version 1.0
	 */
	inline void Remove(AbstractThread* thread)
	{
		erase(thread->GetId());
	}
private:
	/**
	 * Name of container
	 * @since Version 1.0
	 */
	const std::string m_name;
};

/**
* Class that will manage a pool of threads.
*
* @author Tim Bochenek
* @version 1.1
* @see AtomicThread
* @since Version 1.0
*/
template<class ThreadType>
class TemplateThreadPool : public ILogger, public IThreadPool
{
public:
	friend class TemplateThreadPoolTester;	// Testing class
	/**
	 * Class constructor (default)
	 *
	 * @since Version 1.0
	 */
	inline TemplateThreadPool(const pthread_t& creator = pthread_self()) : ILogger(),
	m_lowWater(DefaultLowWater), m_highWater(DefaultHighWater), m_max(2*DefaultHighWater),
	m_increment(DefaultIncrement), m_blockThreads("Blocked"), m_handleThreads("Handle"),
	m_creator(creator), m_config(NULL)
	{
        pthread_mutexattr_t attr;

		memset( &attr, 0, sizeof( attr));
		memset( &m_threadPoolMutex, 0, sizeof( m_threadPoolMutex));

		pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_ENABLE);
		pthread_mutex_init(&m_threadPoolMutex, &attr);	// Initialize mutex
    }
	/**
	 * Class destructor
	 * @since Version 1.0
	 */
	inline virtual ~TemplateThreadPool(void)
	{
		Log(LOG_FN_ENTRY, "TemplateThreadPool::~TemplateThreadPool()\n");
		for(ThreadMapIter Iter = m_blockThreads.begin(); Iter != m_blockThreads.end(); ++Iter)
		{	// Kill all threads currently blocking
			pthread_cancel(Iter->first);
			pthread_kill(Iter->first, SIGURG);
		}
		for(ThreadMapIter Iter = m_handleThreads.begin(); Iter != m_handleThreads.end(); ++Iter)
		{	// Kill all threads currently handling
			pthread_cancel(Iter->first);
			pthread_kill(Iter->first, SIGURG);
		}
		pthread_mutex_destroy(&m_threadPoolMutex);	// Destroy mutex
		if(m_config)
		{
			delete m_config;
			m_config = NULL;
		}
		Log(LOG_FN_ENTRY, "TemplateThreadPool::~TemplateThreadPool() done\n");
	}
	/**
	 * Initialize the thread pool.
	 *
	 * @param configuration
	 *               Xml Node containing the parameters for the behavior of
	 *               the thread pool.
	 * @since Version 1.0
	 */
	inline virtual void Initialize(const XmlNode *configuration)
	{
		try
		{
			if(configuration)
			{
				m_config = configuration->Copy();
				ILogger::Initialize(configuration);
				Log(LOG_FN_ENTRY, "TemplateThreadPool::Initialize(%s)\n", configuration->ToString().c_str());
			}
			else throw BepException("TemplateThreadPool::Initialize() failed because:  No configuration\n");
		}
		catch(BepException& Err)
		{
			Log(LOG_ERRORS, "TemplateThreadPool::Initialize(%s) failed because: %s\n", configuration->ToString().c_str(), Err.what());
		}
		try
		{
			if(configuration)
			{
				const XmlNodeMap& kids = configuration->getChild("Setup")->getChild("ThreadPool")->getChildren();

				Initialize(kids.getNode("Debug")->getValue() == "On",
						   kids.getNode(StartOnCreateString)->getValue() == "Yes",
						   atol(kids.getNode(LowWaterString)->getValue().c_str()),
						   atol(kids.getNode(HighWaterString)->getValue().c_str()),
						   atol(kids.getNode(MaximumString)->getValue().c_str()),
						   atol(kids.getNode(IncrementString)->getValue().c_str()));
			}
			else throw BepException("Configuration node was NULL.  (Will use defaults lw = %d, hw = %d, inc = %d, max = %d, debug = %d)",
									GetLowWater(), GetHighWater(), GetIncrement(), GetMaximum(), IsDebugOn());
		}
		catch(BepException& Err)
		{
			Log(LOG_ERRORS, "TemplateThreadPool::Initialize(%s) failed because: %s\n", configuration->ToString().c_str(), Err.what());
		}
	}
	/**
	 * Initialize the thread pool.
	 *
	 * @param Debug  Turn on diagnostic print information
	 * @param Go     Start thread pool immediately after creating
	 * @param Low    Minimum number of blocked threads
	 * @param Hi     Maximum number of blocked threads
	 * @param Max    Maximum number of threads
	 * @param Inc    Increment value used to create new threads
	 * @since Version 1.0
	 */
	inline virtual void Initialize(const bool Debug = false, const bool Go = false,
								   const unsigned int Low = DefaultLowWater,
								   const unsigned int Hi = DefaultHighWater,
								   const unsigned int Max  = 2*DefaultHighWater,
								   const unsigned int Inc = 1)
	{
		SetDebug(Debug);
		Log(LOG_FN_ENTRY, "TemplateThreadPool::Initialize(%u, %u, %u, %u, %u, %u)\n", Debug, Go, Low, Hi, Max, Inc);
		SetLowWater(Low);
		SetHighWater(Hi);
		SetMaximum(Max);
		SetIncrement(Inc);
		if(Go) Start();
		Log(LOG_FN_ENTRY, "TemplateThreadPool::Initialize(%u, %u, %u, %u, %u, %u) done\n", Debug, Go, Low, Hi, Max, Inc);
	}
	/**
	 * Start the thread pool
	 * @since Version 1.0
	 */
	inline void Start(void)
	{
		Log(LOG_FN_ENTRY, "TemplateThreadPool::Start()\n");
		Housekeeping(pthread_self(), IThreadPool::Go);
		Log(LOG_FN_ENTRY, "TemplateThreadPool::Start() done\n");
	}
	/**
	 * Sets the minimum number of threads that will be blocked
	 * waiting to service clients
	 *
	 * @param limit  Value to set for low water limit
	 * @since Version 1.0
	 */
	inline void SetLowWater(const unsigned int limit)
	{
		Log(LOG_FN_ENTRY, "TemplateThreadPool::SetLowWater(%u)\n", limit);
		if(limit >= IThreadPool::DefaultLowWater)
		{
			if(limit < GetHighWater())
			{
				m_lowWater = limit;	// Set limit
			}
			else
			{
				m_lowWater = GetHighWater() - 1;
				Log(LOG_FN_ENTRY, "Tried to set low water to %d.  Must set low water less than high water (%d).  Will set to %d\n", limit, GetHighWater(), GetLowWater());
			}
		}
		else
		{
			m_lowWater = IThreadPool::DefaultLowWater; // Default if bad value
			Log(LOG_FN_ENTRY, "Tried to set low water to %d.  Must set low water greater than minimum (%d).  Will set to %d\n", limit, IThreadPool::DefaultLowWater, GetLowWater());
		}
	}
	/**
	 * Sets the maximum number of threads that will be blocked
	 * waiting to service clients
	 *
	 * @param limit  Value to set for high water limit
	 * @since Version 1.0
	 */
	inline void SetHighWater(const unsigned int limit)
	{
		Log(LOG_FN_ENTRY, "TemplateThreadPool::SetHighWater(%u)\n", limit);
		if(limit < GetMaximum())
		{
			if(limit > GetLowWater())
			{
				m_highWater = limit; // Set limit
			}
			else
			{
				const unsigned int testValue = GetLowWater() + 1;
				if(testValue < IThreadPool::DefaultHighWater)	m_highWater = testValue;
				else											m_highWater	= testValue;
				Log(LOG_FN_ENTRY, "Tried to set high water to %u.  Must set high water greater than low water (%u).  Will set to %u\n", limit, GetLowWater(), GetHighWater());
			}
		}
		else
		{
			m_highWater = GetMaximum() - 1;	// Default if bad value
			Log(LOG_FN_ENTRY, "Tried to set high water to %u.  Must set high water less than maiximum (%u).  Will set to %u\n", limit, GetMaximum(), m_highWater);
		}
	}
	/**
	 * Sets the maximum number of threads that will be created
	 * blocked threads plus threads servicing clients
	 *
	 * @param limit  Value to set for maximum limit
	 * @since Version 1.0
	 */
	inline void SetMaximum(const unsigned int limit)
	{
		Log(LOG_FN_ENTRY, "TemplateThreadPool::SetMaximum(%u)\n", limit);
		if(limit > GetHighWater())
		{
			m_max = limit; // Set limit
		}
		else
		{
			m_max = GetHighWater() + 1;	// Default if bad value
			Log(LOG_FN_ENTRY, "Tried to set maximum to %u.  Must set maximum greater than high water (%u).  Will set to %u\n", limit, GetHighWater(), GetMaximum());
		}
	}
	/**
	 * Sets the number of threads that will be created when the
	 * minimum number of blocking threads limit was reached
	 *
	 * @param increment  Value to set for increment
	 * @since Version 1.0
	 */
	inline void SetIncrement(const unsigned int increment)
	{
		Log(LOG_FN_ENTRY, "TemplateThreadPool::SetIncrement(%u)\n", increment);
		if(increment >= IThreadPool::DefaultIncrement) m_increment = increment;	// Default if bad value
		else
		{
			m_increment = IThreadPool::DefaultIncrement;		// Set limit
			Log(LOG_FN_ENTRY, "Tried to set increment to %u.  Cannot set increment less than %u.  Will set to %u\n", increment, IThreadPool::DefaultIncrement, GetIncrement());
		}
	}
	/**
	 * Gets the minimum number of threads that will be blocked
	 * waiting to service clients
	 *
	 * @return  Value to set for minimum limit
	 * @since Version 1.0
	 */
	inline const unsigned int GetLowWater(void)
	{
		return(m_lowWater);	// Get limit
	}
	/**
	 * Gets the maximum number of threads that will be blocked
	 * waiting to service clients
	 *
	 * @return  Value to set for maximum limit
	 * @since Version 1.0
	 */
	inline const unsigned int GetHighWater(void)
	{
		return(m_highWater);	// Get limit
	}
	/**
	 * Gets the maximum number of threads that will be created
	 * blocked threads plus threads servicing clients
	 *
	 * @return  Value to set for maximum limit
	 * @since Version 1.0
	 */
	inline const unsigned int GetMaximum(void)
	{
		return(m_max);	// Get limit
	}
	/**
	 * Gets the number of threads that will be created when the
	 * minimum number of blocking threads limit was reached
	 *
	 * @return  Value to set for maximum limit
	 * @since Version 1.0
	 */
	inline const unsigned int GetIncrement(void)
	{
		return(m_increment);	// Get limit
	}
	/**
	 * Get the total number of threads blocking
	 *
	 * @return Total number of threads blocked in the thread pool
	 * @since Version 1.0
	 */
	inline const unsigned int GetBlockedThreadCount(void)
	{
//		Log(LOG_FN_ENTRY, "TemplateThreadPool::GetBlockedThreadCount(%u)\n", m_blockThreads.size());
		return(m_blockThreads.size());
	}
	/**
	 * Get the total number of threads
	 *
	 * @return Total number of threads getting action in the thread pool
	 * @since Version 1.0
	 */
	inline const unsigned int GetHandlingThreadCount(void)
	{
//		Log(LOG_FN_ENTRY, "TemplateThreadPool::GetHandlingThreadCount(%u)\n", m_handleThreads.size());
		return(m_handleThreads.size());
	}
	/**
	 * Get the total number of threads
	 *
	 * @return Total number of threads in the thread pool
	 * @since Version 1.0
	 */
	inline const unsigned int GetThreadCount(void)
	{
//		Log(LOG_FN_ENTRY, "TemplateThreadPool::GetThreadCount(%u)\n", GetBlockedThreadCount() + GetHandlingThreadCount());
		return(GetBlockedThreadCount() + GetHandlingThreadCount());
	}
protected:
	/**
	 * Moves a thread from one container to another
	 *
	 * @param Who    Which thread to move
	 * @param from   Where to move it from
	 * @param to     Where to move it to
	 * @since Version 1.1
	 */
	inline void MoveThread(const pthread_t& Who, ThreadMap& from, ThreadMap& to)
	{
		Log(LOG_FN_ENTRY, "TemplateThreadPool::MoveThread() Moving %d from %s(%u) to %s(%u)\n",
			Who, from.GetName().c_str(), from.size(), to.GetName().c_str(), to.size());
		if(Who != m_creator)
		{
			pthread_mutex_lock(&m_threadPoolMutex);
			ThreadMapIter Iter = from.find(Who);

			if(Iter != from.end())
			{	// Remove from blocking and put into handling
				to.Insert(Iter);
				from.Remove(Iter);
				Log(LOG_FN_ENTRY, "Success! %d moved, %s(%u) to %s(%u)\n", Who, from.GetName().c_str(),
					from.size(), to.GetName().c_str(), to.size());
			}
			else
			{
				Log(LOG_FN_ENTRY, "Oh crap here is the list!\n");
				for(Iter = from.begin(); Iter != from.end(); ++Iter)
				{
					Log(LOG_FN_ENTRY, "Id = %d, pointer = %p\n", Iter->first, Iter->second);
				}
			}
			pthread_mutex_unlock(&m_threadPoolMutex);
		}
		else Log(LOG_ERRORS, "Tried to move creator thread.  No action will be taken\n");
		Log(LOG_FN_ENTRY, "TemplateThreadPool::MoveThread(%d, from %s(%u), to %s(%u))\n",
			Who, from.GetName().c_str(), from.size(), to.GetName().c_str(), to.size());
	}
	/**
	 * Creates threads for the thread pool.
	 *
	 * @param num           Number of threads to create
	 * @param destination   Where to put thread
	 * @since Version 1.0
	 */
	inline const int CreateThreads(const INT32 num, ThreadMap& destination)
	{
		ThreadType* thread = NULL;
		Log(LOG_FN_ENTRY, "TemplateThreadPool::CreateThreads(%d)\n", num);

		INT32 delta, created = 0, count = 3;				// How many threads really needed, and created, and counter for anti-infinite loop
		delta = GetLowWater() - GetBlockedThreadCount();	// get the initial count
		if(delta > 0)	// if there is a need to create a thread
		{
			do
			{
				delta = GetLowWater() - GetBlockedThreadCount();	// get the initial count
				for(unsigned int ii = GetIncrement(); (num > 0) && (delta > 0) && (GetThreadCount() < GetMaximum()) && (ii > 0); ii--)
				{	// Create until increment is exhaused or limit is reached
					try
					{
						thread = new ThreadType(m_config, this);	// Create thread managed by this pool
						thread->Initialize();						// Start the thread
						Log(LOG_FN_ENTRY, "Inserting %d, %p\n", thread->GetId(), thread);
						pthread_mutex_lock(&m_threadPoolMutex);
						destination.Insert(thread);					// Put thread in map
						pthread_mutex_unlock(&m_threadPoolMutex);
						created++;
					}
					catch(BepException& BepErr)
					{
						Log(LOG_ERRORS, "TemplateThreadPool::CreateThreads() error: %s\n", BepErr.what());
					}
				}
				if(delta - (GetLowWater() - GetBlockedThreadCount()) == 0) count--;	// If thread was not created
				delta = GetLowWater() - GetBlockedThreadCount();
			}	// Keep going unless no more need to be created, the loop repeater is done, or number of threads requested have been created
			while((delta > 0) && (count > 0) && (num >= created));
		}
		Log(LOG_FN_ENTRY, "TemplateThreadPool::CreateThreads() done.  Created %u threads\n", created);
		return(created);
	}
	/**
	 * Creates thread in the thread pool.
	 *
	 * @param Who    Id of thread to delete
	 * @param from   What map to delete from
	 * @since Version 1.0
	 */
	inline void DeleteThread(const pthread_t& Who, ThreadMap& from)
	{
		Log(LOG_FN_ENTRY, "TemplateThreadPool::DeleteThread(%d) from %s\n", Who, from.GetName().c_str());
		if(Who != m_creator)
		{
			if(from.size() > 0)
			{
				pthread_mutex_lock(&m_threadPoolMutex);
				ThreadMapIter Iter = from.find(Who);
				if(Iter != from.end())
				{
					Log(LOG_FN_ENTRY, "found %d\n", Who);
					from.Remove(Iter);
				}
				else Log(LOG_ERRORS, "could not find %d\n", Who);
				pthread_mutex_unlock(&m_threadPoolMutex);
			}
			else Log(LOG_ERRORS, "Map %s is empty\n", from.GetName().c_str());
		}
	}
	/**
	 * Manage the the thread pool
	 *
	 * @param Who    Who initiated the call (which thread)
	 * @param Reason What state was thread in when called
	 *
	 *               IThread::Block  == Thread unblocked
	 *               IThread::Handle == Thread is done handling request
	 * @since Version 1.0
	 */
	inline virtual const bool Housekeeping(const pthread_t& Who, const unsigned long Reason)
	{
		Log(LOG_FN_ENTRY, "TemplateThreadPool::Housekeeping(%d, %lu)\n", Who, Reason);
		bool recycle = false;	// Whether or not the thread should recycle
		INT32 delta=1, created = 0;
		switch(Reason)
		{
		case IThreadPool::BlockDone:
			MoveThread(Who, m_blockThreads, m_handleThreads);		// Move thread from blocking to handling (thread was Unblocked)
		case IThreadPool::Go:
			delta = GetLowWater() - GetBlockedThreadCount();		// Thread pool is started
			created = CreateThreads(delta, m_blockThreads);			// Create threads
			break;
		case IThreadPool::HandleDone:
			if(GetBlockedThreadCount() < GetHighWater())				// Thread is done working
			{
				// Thread should be recycled
				MoveThread(Who, m_handleThreads, m_blockThreads);	// Move thread from handling to blocking
				delta = GetLowWater() - GetBlockedThreadCount();	// How many to create
				recycle = true;										// Tell the thread to recycle
			}
			else DeleteThread(Who, m_handleThreads);				// Remove thread from handle pool
			created = CreateThreads(delta, m_blockThreads);			// Create new threads if needed
			break;
		default:
			Log(LOG_ERRORS, "What the...I got %lu\n", Reason);
			break;
		}
		Log(LOG_FN_ENTRY, "TemplateThreadPool::Housekeeping(%d, %lu) done.  Created %d thread%s.  %s thread\n", Who, Reason, created, (created == 1) ? "" : "s", recycle ? "Recycled" : "Did not recycle");
		return(recycle);
	}
	/**
	 * Minimum number of threads that will be blocked waiting to service clients
	 * @since Version 1.0
	 */
	unsigned int m_lowWater;
	/**
	 * Maximum number of threads that will be blocked waiting to service clients
	 * @since Version 1.0
	 */
	unsigned int m_highWater;
	/**
	 * Maximum number of threads that can be created
	 * @since Version 1.0
	 */
	unsigned int m_max;
	/**
	 * Number of threads that will be created when the number of
	 * threads blocked falls below minimum blocked limit
	 * @since Version 1.0
	 */
	unsigned int m_increment;
	/**
	 * Container of blocking threads
	 * @since Version 1.0
	 */
	ThreadMap m_blockThreads;
	/**
	 * Container of handling threads
	 * @since Version 1.0
	 */
	ThreadMap m_handleThreads;
	/**
	 * Thread that created the thread pool
	 * @since Version 1.0
	 */
	const pthread_t m_creator;
	/**
	 * Mutex used to control creation and deletion of threads
	 * @since Version 1.0
	 */
	pthread_mutex_t m_threadPoolMutex;
	/**
	 * Configuration information used to set up Thread Pool and threads
	 * @since Version 1.1
	 */
	XmlNode* m_config;
};

#endif // TemplateThreadPool_h
