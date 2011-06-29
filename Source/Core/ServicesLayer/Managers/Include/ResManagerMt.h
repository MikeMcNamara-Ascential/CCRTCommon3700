//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/Include/ResManagerMt.h $
// $Author:: Rwiersem                                                         $
//   $Date:: 10/30/07 4:01p                                                   $
//
// Description:
//  Multi-threaded resource manager base class using thread pool
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/Include/ResManagerMt.h $
// 
// 5     10/30/07 4:01p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     4/13/06 4:08a Cward
// moved implementation to source file
//
// 2     2/15/06 4:03p Bmeinke
// Moved the code for HandlePulse() and Terminate() from the header file
// to the cpp file
//
// 3     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:29p Cward
//
// 2     10/24/03 11:56a Khykin
// Added the include of shed.h.
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 2     6/26/03 3:56p Khykin
// Updated with Brians latest changes.
//
// 2     5/29/03 2:27p Bmeinke
// SetThreadPriority() now takes an optional second parameter, algorithm,
// to allow the scheduling algorithm to be changed (defaults to
// SCHED_NOCHANGE)
//
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
//
// 11    11/01/02 1:22p Bmeinke
// Added ConnectWaitChid() method to create a connection to the channel
// that the resource manager is blocked on in order to send control pulses
// to the manager
// Removed InitManager() (not needed anymore due to addition of
// ConnectWaitChid() method)
//
// 10    9/06/02 6:24p Bmeinke
// Added RaiseThreadPriority() and SetThreadPriority() methods
//
// 9     4/04/02 3:02a Bmeinke
// Call ConnectDetach( m_mssgCoid) inside Terminate() method
//
// 8     2/27/02 3:00p Bmeinke
// Changed threadPoolParam_t to THREAD_POOL_PARAM_T in static thread pool
// methods for compatability with other thread pool objects
// Forward pulses received by IBepCommunication objects to the I/O channel
//
// 7     12/07/01 2:55p Brian.meinke
// Changed thread pool interface prototypes to always take a
// threadPoolParam_t data types; derived classes which need different data
// types should typecast this to the appropriate data type in the methods
// they override
//
// 6     11/28/01 10:38p Brian.meinke
// Added support for reading thread priority from config file
//
// 5     11/21/01 8:41p Brian.meinke
// First integration with existing ResourceManager derived classes
// (UniverseMgr)
//
// 4     11/16/01 9:43a Brian.meinke
// Removed constructor which took command line arguments
// Changed InitManager() to no longer take a threadChid argument; instead
// it calls GetWaitChid() inside InitManager()
//
// 3     11/15/01 2:55p Brian.meinke
// Added constructor using command line arguments
//
// 2     11/14/01 2:04a Brian.meinke
// Added VSS file header
// Added XML node tag definitions
// Moved implementation to CPP file
// ThreadPoolInit now takes an XML node to configure the thread pool with
// Override Initialize() method in order to configure the thread pool from
// an XML node
// Removed calls to pthread_kill: the thread pool block function
// (resmgr_block) is a cancellation point
// Added short delay between cancellation of pool thread inside
// KillPoolThreads()
// Added virtual GetWaitChid() method to retrieve the channel on which to
// listen for control pulses
// IoMssgLoop() no longer takes a chid parameter: replaced with a call to
// GetWaitChid()
//
//******************************************************************************

#ifndef _RES_MANAGER_MT_H_INCLUDED
#define _RES_MANAGER_MT_H_INCLUDED

class ResManagerMt;

#include <sched.h>

#include "ResManagerBase.h"
#include "ThreadList.h"

// Header Files

///////////////////////////////////////////////////////////////////////////////
//      THREAD POOL STUFF
///////////////////////////////////////////////////////////////////////////////

/**
 * XML node tag of the thread pool configuration node
 */
#define CFG_THREAD_POOL_TAG     XML_T("MgrPool")

/**
 * XML attribute tag for the thread pool low water attribute
 */
#define CFG_POOL_LOW_TAG        XML_T("lowWater")

/**
 * XML attribute tag for the thread pool high water attribute
 */
#define CFG_POOL_HI_TAG         XML_T("hiWater")

/**
 * XML attribute tag for the thread pool maximum attribute
 */
#define CFG_POOL_MAX_TAG        XML_T("maximum")

/**
 * XML attribute tag for the thread pool thread increment attribute
 */
#define CFG_POOL_INC_TAG        XML_T("increment")

/**
 * XML attribute tag for the thread pool thread increment attribute
 */
#define CFG_POOL_PRIO_TAG       XML_T("priority")


/**
 * Minimum number of blocked/waiting thread in the thread pool
 */
#define THREAD_POOL_MIN 5
/**
 * Maximum number of blocked/waiting thread in the thread pool
 */
#define THREAD_POOL_MAX 50

/**
 * Handle passed to thread pool context allocator
 */
typedef struct _threadPoolHandle_t
{
    /**
     * Resource manager's dispatch pointer
     */
    dispatch_t      *dpp;

    /**
     * Controlling resource manager for the thread pool
     */
    ResManagerMt    *ioMgr;

    /**
     * Connection on which pool threads need to deliver important pulses to
     */
    int             parentCoId;
} threadPoolHandle_t;


/**
 * Data type passed to the ThreadPool handler functions
 */
typedef struct _threadPoolParam_t
{
    /**
     * Handle to the thread pool
     */
    threadPoolHandle_t  *poolHandle;

    /**
     * PID of parent process
     */
    pid_t               parent;

    /**
     * Context structure expected by the lower level  api functions
     */
    dispatch_context_t  *dispatchCtx;

} threadPoolParam_t;

/**
 * Resource manager using thread pool to handle I/O operations
 *
 * @author Brian.Meinke
 * @version 1.0
 */
class ResManagerMt : public ResManagerBase
{
public:
    /**
     * Default constructor
     */
    ResManagerMt();

    /**
     * Class destructor
     */
    ~ResManagerMt();

    /**
     * Initializes data needed by the thread pool and its threads
     *
     * @param poolNode XML node containing thread pool attributes
     */
    virtual void ThreadPoolInit( const XmlNode *poolNode=NULL);

    /**
     * Performs any necessary manager cleanup (free memory, detach any
     * adopted path names, etc)
     */
    virtual void CleanupManager();

    /**
     * Method called to enter I/O message processing loop. Will not
     * return until we get a reload config pulse or a terminate pulse
     *
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    ResManagerCmd_t Execute();

    /**
     * Reads configuration data from the given XML node
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Will hadle pulse if overriden.
     *
     * @param code   Pulse code
     * @param value  Pulse value
     * @return If pulse is handled.
     *         Handled     == BEP_STATUS_SUCCESS
     *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);

    /**
     * Method used to terminate the component.  This performs
     * any cleanup that the destructor would not and sets
     * the state of the component to terminate.
     * @since Version 1.0
     */
    virtual void Terminate(void);

protected:

    /**
     * Handles I/O mesages
     *
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t IoMssgLoop();

    ///////////////////////////////////////////////////////////////////////////
    // Thread pool functions
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    /**
     * Function called by kernel to allocate a new struct to pass to a new thread
     *
     * @param handle OUr user defined handle: it is the connection ID to listen on
     * @return Pointer to the new structure to use with the new thread
     */
    static THREAD_POOL_PARAM_T *poolContextAlloc(THREAD_POOL_HANDLE_T *handle);

    /**
     * Function called by kernel to allocate a new struct to pass to a new thread
     *
     * @param handle OUr user defined handle: it is the connection ID to listen on
     * @return Pointer to the new structure to use with the new thread
     */
    virtual threadPoolParam_t *PoolContextAlloc(threadPoolHandle_t *handle);

    /**
     * Initializes the data in the thread pool parameter data type
     *
     * @param handle    Thread pool handle
     * @param poolParam Thread instance data
     * @return 0 if successful, -1 if an error occurrs
     */
    virtual int InitializePoolParam( threadPoolHandle_t *handle,
                                     threadPoolParam_t *poolParam);
    /**
     * Function called to free memory used for a thread argument struct.
     *
     * @param ctp Thread context structure to free.
     */
    static void poolContextFree(THREAD_POOL_PARAM_T *ctp);

    /**
     * Function called to free memory used for a thread argument struct.
     *
     * @param ctp Thread context structure to free.
     */
    virtual void PoolContextFree(threadPoolParam_t *ctp);

    /**
     * Initializes the data in the thread pool parameter data type.
     *
     * @param ctp Thread context structure to finalize.
     * 
     * @return 0 if successful, -1 if an error occurrs
     */
    virtual void FinalizePoolParam( threadPoolParam_t *ctp);

    /**
     * Function in which each thread in the thread pool will block
     *
     * @param ctp    Thread context structure
     * @return
     */
    static THREAD_POOL_PARAM_T *poolBlockFunc(THREAD_POOL_PARAM_T *ctp);

    /**
     * Function in which each thread in the thread pool will block
     *
     * @param ctp    Thread context structure
     * @return
     */
    virtual threadPoolParam_t *PoolBlockFunc(threadPoolParam_t *ctp);

    /**
     * Function called when our thread returns from the block function
     *
     * @param ctp    Thread context structure
     */
    static void poolUnBlockFunc( THREAD_POOL_PARAM_T *ctp);

    /**
     * Function called when our thread returns from the block function
     *
     * @param ctp    Thread context structure
     */
    virtual void PoolUnBlockFunc(threadPoolParam_t *ctp);

    /**
     * Function called after our thread has left the blocked state and is ready
     * to do some work
     *
     * @param ctp    Thread context structure for this thread to work with
     * @return 0
     */
    static int poolHandlerFunc(THREAD_POOL_PARAM_T *ctp);

    /**
     * Function called after our thread has left the blocked state and is ready
     * to do some work
     *
     * @param ctp    Thread context structure for this thread to work with
     * @return 0
     */
    virtual int PoolHandlerFunc(threadPoolParam_t *ctp);

    /**
     * Kills any thread-pool threads that are hanging around
     */
    virtual void KillPoolThreads();

    /**
     * See which channel to wait on
     *
     * @return Channel id to wait for messages on
     */
    virtual int GetWaitChid();

    /**
     * Creates a connection to the channel that the resource manager is
     * blocked on in order to send control pulses to the manager
     */
    virtual void ConnectWaitChid();

    /**
     * Change the priority of the calling thread by the specified amount
     *
     * @param priorityDelta
     *               Amount to change the thread priority by
     * @return Previous priority of the thread
     */
    int RaiseThreadPriority( int priorityDelta);

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
    int SetThreadPriority( int newPriority, int algorithm=SCHED_NOCHANGE);

    /**
     * List of 'cancelable' pool threads
     */
    ThreadList              m_threadList;

    /**
     * Handle data passed into thread pool Functions
     */
    threadPoolHandle_t      m_poolHandle;

    /**
     * Default attributes for our thread pool
     *
     * @see thread_pool_attr_t
     */
    thread_pool_attr_t      m_poolAttribs;

    /**
     * Attributes for threads in the thread pool
     */
    pthread_attr_t          m_poolThreadAttributues;

    /**
     * Thread pool handle
     */
    thread_pool_t           *m_threadPool;

    /**
     * Connection to which child threads can send dat to the parent
     */
    int                     m_waitCoId;

    /**
     * Priority at which to run the threads int he thread pool
     */
    int                     m_poolThreadPriority;
private:
    /**
     * Channel on which we wait for messages from
     */
    int                     m_waitChId;
};


#endif  // _RES_MANAGER_MT_H_INCLUDED

