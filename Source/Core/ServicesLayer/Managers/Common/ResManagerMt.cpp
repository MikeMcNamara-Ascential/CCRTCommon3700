//******************************************************************************
// Description:
//	Multi-threaded resource manager base class using thread pool
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/Common/ResManagerMt.cpp $
// 
// 5     4/13/06 3:50a Cward
// moved implementation from h to cpp file
//
// 2     2/15/06 4:02p Bmeinke
// Moved the code for HandlePulse() and Terminate() from the header file
// to the cpp file
//
// 4     6/24/04 1:04a Cward
// Removed pound if 0 block. Updated the header block comments to the new
// standard.
//
// 3     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:29p Cward
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
// 2     6/26/03 4:27p Khykin
// Updated with Brians latest changes.
//
// 2     5/29/03 2:33p Bmeinke
// Fixed the ordering of the arguments passed to pthread_kill() in
// KillPoolThreads()
// Added a second parameter, algorithm, to SetThreadPriority() to allow a
// thread's scheduling algorithm to be changed (defaults to
// SCHED_NOCHANGE)
//
// 1     4/22/03 9:50a Khykin
// Kevins Working Checkin
//
// 24    3/26/03 10:32p Bmeinke
// Restore thread pool thread priorities to desired level (based on config
// file) before blocking in PoolBlockFunc()
//
// 23    3/14/03 9:23p Bmeinke
// Create the threads in the thread pool in a detached state
// Thread pool threads add their thread ID's to the blocked thread list in
// PoolContextAlloc()
// Thread pool threads remove their thread ID's from the blocked thread
// list in PoolContextFree()
// Changed the dealy between killing consecutive pool threads from 1ms to
// 10ms
//
// 22    1/18/03 2:07a Bmeinke
// Added the calls to pthread_cancel()/pthread_join() in
// KillPoolThreads(); potentially a bug in 6.2 final?
//
// 21    1/03/03 4:22p Bmeinke
// Added Logger statements to InitializePoolParam()
//
// 20    11/20/02 2:26a Bmeinke
// Added ResManagerMt:: prefix to logger statements
// Removed if 0 block from KillPoolThreads()
//
// 19    11/01/02 1:21p Bmeinke
// Removed InitManager() (not needed any more due to addition of
// ConnectWaitChid() method)
//
// 18    11/01/02 1:21p Bmeinke
// Added ConnectWaitChid() method to create a connection to the channel
// that the resource manager is blocked on in order to send control pulses
// to the manager
//
// 17    10/08/02 2:56p Bmeinke
// Removed individual thread cancellation and joining for threads in the
// thread pool: thread_pool_destroy now takes care of all that (starting
// with QNX 6.1)
//
// 16    9/06/02 6:28p Bmeinke
// Added RaiseThreadPriority() and SetThreadPriority() methods
//
// 15    8/26/02 7:42p Bmeinke
// Added call to m_threadList.remove( pthread_self(), false) at end of
// thread pool block function
// Removed call to m_threadList.remove( pthread_self(), false) from thread
// pool un-block function
//
// 14    4/04/02 2:59a Bmeinke
// Rearranged order of ivars in initialization list of constructors
// Set process priority to the level specified by the thread pool priority
// specified in the config file in Initialize()
// Removed the "Thread #%d" from calls to Log() calls from thread pool
// functions
// Removed a couple of unnecessary calls to Log() in PoolBlockFunc()
//
// 13    3/22/02 1:10p Bmeinke
// Fixed bug in InitManager(): m_waitChid not being set to the value of
// threadChid passed in
//
// 12    2/27/02 2:57p Bmeinke
// Changed threadPoolParam_t to THREAD_POOL_PARAM_T in static thread pool
// methods for compatability with other thread pool objects
//
// 11    12/12/01 9:58p Brian.meinke
// Changed ThreadPoolInit() to only set the priority of the pool threads
// if the priority has not already been set
//
// 10    12/07/01 6:05p Brian.meinke
// Don't set m_poolThreadPriority in ThreadPoolInit() unless its value ==
// -1 (uninitialized)
//
// 9     12/07/01 2:54p Brian.meinke
// Changed thread pool interface prototypes to always take a
// threadPoolParam_t data types; derived classes which need different data
// types should typecast this to the appropriate data type in the methods
// they override
//
// 8     11/30/01 1:00p Brian.meinke
// Cleaned up extra long lines (extended past 80th column)
//
// 7     11/28/01 11:04p Brian.meinke
// Added support for reading thread priority from config file
// Removed setting ResManagerBase::m_pulseSigCoid=GetWaitChid() in
// IoMssgLoop()
// If MsgReceivePulse() returns -1 in IoMssgLoop(), leave with
// RES_MGR_CMD_TERMINATE (used for Ctrl-C quitting)
//
// 6     11/21/01 8:42p Brian.meinke
// First integration with existing ResourceManager derived classes
// (UniverseMgr)
//
// 5     11/16/01 9:43a Brian.meinke
// Removed constructor which took command line arguments
// Changed InitManager() to no longer take a threadChid argument; instead
// it calls GetWaitChid() inside InitManager()
//
// 4     11/15/01 2:55p Brian.meinke
// Added constructor using command line arguments
//
// 3     11/14/01 11:04p Brian.meinke
// Call HandleResMgrPulse() inside IoMssgLoop() when we receive a pulse
//
// 2     11/14/01 2:01a Brian.meinke
// ThreadPoolInit now takes an XML node to configure the thread pool with
// Override Initialize() method in order to configure the thread pool from
// an XML node
// Removed calls to pthread_kill: the thread pool block function
// (resmgr_block) is a cancellation point
// Added short delay between cancellation of pool thread inside
// KillPoolThreads()
//
// 1     11/13/01 10:35p Brian.meinke
// Multi-threaded Resource Manager base class with no XML message support.
// Multi-threadedness obtained via thread pool to handle I/O messages
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <sched.h>

#include "ResManagerMt.h"

/**
 * Default constructor
 */
ResManagerMt::ResManagerMt() : ResManagerBase(),
	m_threadPool( NULL), m_waitCoId( -1), m_poolThreadPriority( -1),
	m_waitChId( -1)
{
	memset( &m_poolHandle, 0, sizeof( m_poolHandle));
	memset( &m_poolAttribs, 0, sizeof(m_poolAttribs));
	pthread_attr_init(&m_poolThreadAttributues);
	SetProcessName( "ResManagerMt");
};

/**
 * Class destructor
 */
ResManagerMt::~ResManagerMt()
{
}

/**
 * Initializes data needed by the thread pool and its threads
 *
 * @param poolNode XML node containing thread pool attributes
 */
void ResManagerMt::ThreadPoolInit( const XmlNode *poolNode)
{
	Log( LOG_FN_ENTRY, "Enter ResManagerMt::ThreadPoolInit()\n");

	struct sched_param	threadSched;

	// Initialize the thread pool handle data
	m_poolHandle.dpp = m_dispatchPtr;
	m_poolHandle.ioMgr = this;
	m_poolHandle.parentCoId = m_waitCoId;

	// Initialize thread pool with defaults
	memset(&m_poolAttribs, 0, sizeof(m_poolAttribs));
	m_poolAttribs.handle = (THREAD_POOL_HANDLE_T*)&m_poolHandle;
	m_poolAttribs.context_alloc = ResManagerMt::poolContextAlloc;
	m_poolAttribs.block_func = ResManagerMt::poolBlockFunc;
	m_poolAttribs.unblock_func = ResManagerMt::poolUnBlockFunc;
	m_poolAttribs.handler_func = ResManagerMt::poolHandlerFunc;
	m_poolAttribs.context_free = ResManagerMt::poolContextFree;

	if( poolNode != NULL)
	{
		const XmlNodeMap &poolAttribs = poolNode->getAttributes();
		try
		{
			const XmlString &val = poolAttribs.getNode( CFG_POOL_LOW_TAG)->getValue();
			m_poolAttribs.lo_water = atol( val.c_str());
		}
		catch( XmlException &err)
		{
			m_poolAttribs.lo_water = THREAD_POOL_MIN;
		}

		try
		{
			const XmlString &val = poolAttribs.getNode( CFG_POOL_HI_TAG)->getValue();
			m_poolAttribs.hi_water = atol( val.c_str());
		}
		catch( XmlException &err)
		{
			m_poolAttribs.hi_water = THREAD_POOL_MAX;
		}
		try
		{
			const XmlString &val = poolAttribs.getNode( CFG_POOL_MAX_TAG)->getValue();
			m_poolAttribs.maximum = atol( val.c_str());
		}
		catch( XmlException &err)
		{
			m_poolAttribs.maximum = THREAD_POOL_MAX;
		}
		try
		{
			const XmlString &val = poolAttribs.getNode( CFG_POOL_INC_TAG)->getValue();
			m_poolAttribs.increment = atol( val.c_str());
		}
		catch( XmlException &err)
		{
			m_poolAttribs.increment = 1;
		}
		try
		{
			if(m_poolThreadPriority == -1)
			{
				const XmlString &val = poolAttribs.getNode( CFG_POOL_PRIO_TAG)->getValue();
				m_poolThreadPriority = atol( val.c_str());
			}
		}
		catch( XmlException &err)
		{
			if( m_poolThreadPriority == -1)
			{
				m_poolThreadPriority = getprio( 0);
			}
		}
	}
	else
	{
		m_poolAttribs.lo_water = THREAD_POOL_MIN;
		m_poolAttribs.hi_water = THREAD_POOL_MAX;
		m_poolAttribs.maximum = THREAD_POOL_MAX;
		m_poolAttribs.increment = 1;
		if( m_poolThreadPriority == -1)
		{
			m_poolThreadPriority = getprio( 0);
		}
	}

	// Set priority for pool threads
	pthread_attr_init(&m_poolThreadAttributues);
	pthread_attr_getschedparam( &m_poolThreadAttributues, &threadSched);
	threadSched.sched_priority = m_poolThreadPriority;
    pthread_attr_setinheritsched( &m_poolThreadAttributues, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedparam( &m_poolThreadAttributues, &threadSched);
	pthread_attr_setdetachstate( &m_poolThreadAttributues, PTHREAD_CREATE_DETACHED);

	m_poolAttribs.attr = &m_poolThreadAttributues;

	// Create the thread pool
	m_threadPool = thread_pool_create(&m_poolAttribs, 0);
	Log( LOG_FN_ENTRY, "Exit ResManagerMt::ThreadPoolInit(), thread Priority = %d\n",
		 m_poolThreadPriority);
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void ResManagerMt::CleanupManager()
{
	Log( LOG_FN_ENTRY, "Enter ResManagerMt::CleanupManager()\n");

	// Shutdown the thread pool
	KillPoolThreads();

	// Cleanup base class
	ResManagerBase::CleanupManager();

	if( m_waitChId != -1)
	{
		ChannelDestroy( m_waitChId);
		m_waitChId = -1;
	}

	if( m_waitCoId != -1)
	{
		ConnectDetach( m_waitCoId);
		m_waitCoId = -1;
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerMt::CleanupManager()\n");
}

/**
 * Method called to enter I/O message processing loop. Will not
 * return until we get a reload config pulse or a terminate pulse
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t ResManagerMt::Execute()
{
	ResManagerCmd_t retVal = RES_MGR_CMD_NONE;

	Log( LOG_FN_ENTRY, "Enter ResManagerMt::Execute()\n");
	while( retVal == RES_MGR_CMD_NONE)
	{
		retVal = RES_MGR_CMD_NONE;
		if( InitManager() == true)
		{
			// Start the thread running
			thread_pool_start( m_threadPool);

			// Handle I/O messages
			retVal = IoMssgLoop();

			// Cleanup berfore shutting down
			CleanupManager();
		}
		else
		{
			retVal = RES_MGR_CMD_TERMINATE;
		}
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerMt::Execute()\n");

	return( retVal);
}

/**
 * Reads configuration data from the given XML node
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void ResManagerMt::Initialize(const XmlNode *document)
{
	const XmlNode* poolNode=NULL;

	Log( LOG_FN_ENTRY, "Enter ResManagerMt::Initialize()\n");

	if( document != NULL)
	{
		ResManagerBase::Initialize( document);

		try
		{
			poolNode = document->getChild(CFG_SETUP_TAG)->getChild(CFG_THREAD_POOL_TAG);
		}
		catch( XmlException &err)
		{
			poolNode = NULL;
		}
	}
	// Read thread pool config
	ThreadPoolInit( poolNode);
	if( m_poolThreadPriority != -1)
	{
		BposSetPriority( 0, m_poolThreadPriority);
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerMt::Initialize()\n");
}

/**
 * Will hadle pulse if overriden.
 *
 * @param code   Pulse code
 * @param value  Pulse value
 * @return If pulse is handled.
 *         Handled     == BEP_STATUS_SUCCESS
 *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
 */
const INT32 ResManagerMt::HandlePulse(const INT32 code, const INT32 value)
{
	Log( LOG_FN_ENTRY, "Enter ResManagerMt::HandlePulse(%d,%d)\n", code,value);

	MsgSendPulse( m_mssgCoid, SIGEV_PULSE_PRIO_INHERIT, code, value);

	Log( LOG_FN_ENTRY, "Exit ResManagerMt::HandlePulse(%d,%d)\n", code,value);

	return( BEP_STATUS_SUCCESS);
}

/**
 * Method used to terminate the component.  This performs
 * any cleanup that the destructor would not and sets
 * the state of the component to terminate.
 * @since Version 1.0
 */
void ResManagerMt::Terminate(void)
{
	Log( LOG_FN_ENTRY, "Enter ResManagerMt::Terminate()\n");

	MsgSendPulse( m_mssgCoid, PULSE_HI_PRIO, SYSTEM_PULSE, TERMINATE_PULSE);
	ConnectDetach( m_mssgCoid);
	m_mssgCoid = -1;

	Log( LOG_FN_ENTRY, "Exit ResManagerMt::Terminate()\n");
}

/**
 * Handles I/O mesages
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t ResManagerMt::IoMssgLoop()
{
	ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
	io_pulse_t	pulse;
	int 		rcvId;
	int			chId;

	// See which channel to get messages on
	chId = GetWaitChid();

	Log( LOG_FN_ENTRY, "Enter ResManagerMt::IoMssgLoop(), waitChid=%ld\n", chId);

	// Service resource manager type messages
	while( retVal == RES_MGR_CMD_NONE)
	{
		// Reset pulse command
		retVal = RES_MGR_CMD_NONE;

		// Wait for incoming pulses
		rcvId = MsgReceivePulse( chId, &pulse, sizeof( pulse), NULL);
		// If we got a pulse
		if( rcvId == 0)
		{
			// Handle the pulse
			retVal = HandleResMgrPulse( pulse);
		}
		else if( rcvId != -1)
		{
			Log( "\tReceived message\n");
			// Reply with "Not Supported"
			MsgReply( rcvId, ENOSYS, NULL, 0);
		}
		else
		{
			Log( "\tReceived error: %s\n", strerror( errno));
			retVal = RES_MGR_CMD_TERMINATE;
		}
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerMt::IoMssgLoop()\n");
	return( retVal);
}

///////////////////////////////////////////////////////////////////////////
// Thread pool functions
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

/**
 * Function called by kernel to allocate a new struct to pass to a new thread
 *
 * @param handle OUr user defined handle: it is the connection ID to listen on
 * @return Pointer to the new structure to use with the new thread
 */
THREAD_POOL_PARAM_T *ResManagerMt::poolContextAlloc(THREAD_POOL_HANDLE_T *handle)
{
	threadPoolHandle_t	*poolHandle = (threadPoolHandle_t*)handle;
	ResManagerMt	*ioMgr = (ResManagerMt*)ResManagerBase::m_ioMgrForOcb;

	return( (THREAD_POOL_PARAM_T *)ioMgr->PoolContextAlloc( poolHandle));
}

/**
 * Function called by kernel to allocate a new struct to pass to a new thread
 *
 * @param handle OUr user defined handle: it is the connection ID to listen on
 * @return Pointer to the new structure to use with the new thread
 */
threadPoolParam_t *ResManagerMt::PoolContextAlloc(threadPoolHandle_t *handle)
{
	threadPoolParam_t	*retVal = NULL;

	Log( LOG_FN_ENTRY, "Enter ResManagerMt::PoolContextAlloc()\n");

	m_threadList.add( pthread_self());

	retVal = new threadPoolParam_t;

	InitializePoolParam(handle, retVal);

	Log( LOG_FN_ENTRY, "Exit ResManagerMt::PoolContextAlloc()\n");

	return( retVal);
}

/**
 * Initializes the data in the thread pool parameter data type
 *
 * @param handle    Thread pool handle
 * @param poolParam Thread instance data
 * @return 0 if successful, -1 if an error occurrs
 */
int ResManagerMt::InitializePoolParam( threadPoolHandle_t *handle,
								 threadPoolParam_t *poolParam)
{
	int retVal = 0;

	Log( LOG_FN_ENTRY, "Enter ResManagerMt::InitializePoolParam()\n");
	poolParam->poolHandle = handle;
	poolParam->parent =  getpid();
	poolParam->dispatchCtx = dispatch_context_alloc( m_dispatchPtr);
	if( poolParam->dispatchCtx == NULL)
	{
		Log( LOG_ERRORS, "\tThread #%d Error allocating dispatch context: %s\n",
			 pthread_self(), strerror( errno));
		retVal = -1;
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerMt::InitializePoolParam()\n");
	return( retVal);
}

/**
 * Function called to free memory used for a thread argument struct
 *
 * @param param  Thread context structure to free
 */
void ResManagerMt::poolContextFree(THREAD_POOL_PARAM_T *ctp)
{
	threadPoolParam_t	*poolParam = (threadPoolParam_t*)ctp;
	ResManagerMt		*ioMgr = (ResManagerMt*)ResManagerBase::m_ioMgrForOcb;

	ioMgr->PoolContextFree(poolParam);
}

/**
 * Function called to free memory used for a thread argument struct
 *
 * @param param  Thread context structure to free
 */
void ResManagerMt::PoolContextFree(threadPoolParam_t *ctp)
{
	if( ctp != NULL)
	{
		Log( LOG_FN_ENTRY, "Enter ResManagerMt::PoolContextFree()\n");

		m_threadList.remove( pthread_self(), false);

		FinalizePoolParam( ctp);
		delete ctp;
		ctp = NULL;
		Log( LOG_FN_ENTRY, "Exit ResManagerMt::PoolContextFree()\n");
	}
}

/**
 * Initializes the data in the thread pool parameter data type
 *
 * @param handle    Thread pool handle
 * @param poolParam Thread instance data
 * @return 0 if successful, -1 if an error occurrs
 */
void ResManagerMt::FinalizePoolParam( threadPoolParam_t *ctp)
{
	dispatch_context_free(ctp->dispatchCtx);
	ctp->dispatchCtx = NULL;
}


/**
 * Function in which each thread in the thread pool will block
 *
 * @param ctp    Thread context structure
 * @return
 */
THREAD_POOL_PARAM_T *ResManagerMt::poolBlockFunc(THREAD_POOL_PARAM_T *ctp)
{
	threadPoolParam_t	*poolParam = (threadPoolParam_t*)ctp;
	ResManagerMt 		*ioMgr = (ResManagerMt*)poolParam->poolHandle->ioMgr;

	return( (THREAD_POOL_PARAM_T*)ioMgr->PoolBlockFunc(poolParam));
}

/**
 * Function in which each thread in the thread pool will block
 *
 * @param ctp    Thread context structure
 * @return
 */
threadPoolParam_t *ResManagerMt::PoolBlockFunc(threadPoolParam_t *ctp)
{
	dispatch_context_t	*newCtx=NULL;
	struct sched_param	schedParam;

	Log( LOG_FN_ENTRY, "Enter ResManagerMt::PoolBlockFunc()\n");

	// Make sure I am at the deired priority
	SchedGet(0, pthread_self(), &schedParam);
	schedParam.sched_priority = m_poolThreadPriority;
	SchedSet(0, pthread_self(), SCHED_NOCHANGE, &schedParam);

	newCtx = dispatch_block(ctp->dispatchCtx);
	if( newCtx == NULL)
	{
		Log( LOG_ERRORS, "\tThread #%d Error in dispatch_block: %s\n",
			 pthread_self(), strerror( errno));
	}
	else if( ctp->dispatchCtx != newCtx)
	{
		Log( LOG_ERRORS, "\tThread #%d New dispatch context allocated\n",
			 pthread_self());
		ctp->dispatchCtx = newCtx;
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerMt::PoolBlockFunc()\n");
	return( ctp);
}

/**
 * Function called when our thread returns from the block function
 *
 * @param ctp    Thread context structure
 */
void ResManagerMt::poolUnBlockFunc( THREAD_POOL_PARAM_T *ctp)
{
	threadPoolParam_t	*poolParam = (threadPoolParam_t*)ctp;
	ResManagerMt		*ioMgr = (ResManagerMt*)poolParam->poolHandle->ioMgr;

	ioMgr->PoolUnBlockFunc( poolParam);
}

/**
 * Function called when our thread returns from the block function
 *
 * @param ctp    Thread context structure
 */
void ResManagerMt::PoolUnBlockFunc(threadPoolParam_t *ctp)
{
	Log( LOG_FN_ENTRY, "Enter ResManagerMt::PoolUnBlockFunc()\n");

	if( (ctp != NULL) && (ctp->dispatchCtx != NULL))
	{
		dispatch_unblock( ctp->dispatchCtx);
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerMt::PoolUnBlockFunc()\n");
}

/**
 * Function called after our thread has left the blocked state and is ready
 * to do some work
 *
 * @param ctp    Thread context structure for this thread to work with
 * @return 0
 */
int ResManagerMt::poolHandlerFunc(THREAD_POOL_PARAM_T *ctp)
{
	threadPoolParam_t	*poolParam = (threadPoolParam_t*)ctp;
	ResManagerMt 		*ioMgr = (ResManagerMt*)poolParam->poolHandle->ioMgr;

	return( ioMgr->PoolHandlerFunc( poolParam));
}

/**
 * Function called after our thread has left the blocked state and is ready
 * to do some work
 *
 * @param ctp    Thread context structure for this thread to work with
 * @return 0
 */
int ResManagerMt::PoolHandlerFunc(threadPoolParam_t *ctp)
{
	int retVal;

	Log( LOG_FN_ENTRY, "Enter ResManagerMt::PoolHandlerFunc()\n");


	// Handle the IO messgae that unblocked us
	retVal = dispatch_handler(ctp->dispatchCtx);

	Log( LOG_FN_ENTRY, "Exit ResManagerMt::PoolHandlerFunc()\n");

	return( retVal);
}

/**
 * Kills any thread-pool threads that are hanging around
 */
void ResManagerMt::KillPoolThreads()
{
	Log( LOG_FN_ENTRY, "Enter ResManagerMt::KillPoolThreads( %d)\n", m_threadList.size());
	/**********************************************************************
		Prior to QNX 6.1, we had to manually kill all the threads in our
		thread pool. Now we simply need to call thread_pool_destroy
		and let the kernel take care of killing the threads in our
		thread pool :)
	**********************************************************************/

#if 0
	/**********************************************************************
		Looks like the thread_pool_destroy() is broke again in 6.2 final :(
	**********************************************************************/
	// If not already destroyed
	if( m_threadPool != NULL)
	{
		pthread_t		threadId;
		ThreadListItr_t	itr;

		// Cancel all threads first
		while( (itr=m_threadList.begin()) != m_threadList.end())
		{
			threadId = itr->first;
			Log( "Stopping thread %d\n", threadId);
			// If not current thread and not main thread of execution
			if( (threadId != pthread_self()) && (threadId > 1))
			{
				// Set the cancellation state of the thread
				pthread_cancel( threadId);
				// Signal the thread in order to break it out of a non-cancellation
				//	point blocking function
				pthread_kill(threadId, SIGXCPU);
				delay( 10);
			}
			Log( "Thread %d done, %d left\n", threadId, m_threadList.size());
			m_threadList.erase( threadId);
		}

		// Destroy the thread pool
		Log( "thread_pool_destroy\n");
		thread_pool_destroy( m_threadPool);
		m_threadPool = NULL;
		m_threadList.clear();
	}
#else
	// Destroy the thread pool
	Log( "thread_pool_destroy\n");
	thread_pool_destroy( m_threadPool);
	m_threadPool = NULL;
#endif
	Log( LOG_FN_ENTRY, "Exit ResManagerMt::KillPoolThreads( %d)\n", m_threadList.size());
}

/**
 * See which channel to wait on
 *
 * @return Channel id to wait for messages on
 */
int ResManagerMt::GetWaitChid()
{
	return( m_waitChId);
}

/**
 * Creates a connection to the channel that the resource manager is
 * blocked on in order to send control pulses to the manager
 */
void ResManagerMt::ConnectWaitChid()
{
	int threadChId = GetWaitChid();

	// Create channel/connection to communicate with threads
	if( m_waitChId == -1)
	{
		// Don't creat a channel if one is already specified
		if( threadChId == -1)
		{
			m_waitChId = ChannelCreate( 0);
		}
		else
		{
			m_waitChId = threadChId;
		}
	}

	// If no connection for threads to talk on yet
	if( m_waitCoId == -1)
	{
		// If channel not specified
		if( threadChId == -1)
		{
			// Use our default channel
			threadChId = m_waitChId;
		}
		m_waitCoId = ConnectAttach( ND_LOCAL_NODE,
									0,
									threadChId,
									_NTO_SIDE_CHANNEL,
									0);
	}
}

/**
 * Change the priority of the calling thread by the specified amount
 *
 * @param priorityDelta
 *               Amount to change the thread priority by
 * @return Previous priority of the thread
 */
int ResManagerMt::RaiseThreadPriority( int priorityDelta)
{
	int					retVal = -1;
	struct sched_param	schedParam;

	if( SchedGet( 0, 0, &schedParam) != -1)
	{
		retVal = schedParam.sched_curpriority;

		schedParam.sched_curpriority += priorityDelta;

		SchedSet( 0, 0, SCHED_NOCHANGE, &schedParam);
	}

	return( retVal);
}

/**
 * Set the priority of the calling thread to the specified values
 *
 * @param newPriority
 *                  New priority of the thread
 * @param algorithm The scheduling algorithm to use (SCHED_NOCHANGE is default)
 *                  SCHED_NOCHANGE
 *                  SCHED_FIFO
 *                  SCHED_RR
 *                  SCHED_OTHER
 *                  SCHED_SPORADIC
 *                  SCHED_ADJTOHEAD
 *                  SCHED_ADJTOTAIL
 *                  SCHED_MAXPOLICY
 *
 * @return Previous priority of the thread
 */
int ResManagerMt::SetThreadPriority( int newPriority, int algorithm)
{
	int					retVal = -1;
	struct sched_param	schedParam;

	if( SchedGet( 0, 0, &schedParam) != -1)
	{
		retVal = schedParam.sched_curpriority;

		schedParam.sched_priority = newPriority;

		SchedSet( 0, 0, algorithm, &schedParam);
	}

	return( retVal);
}

