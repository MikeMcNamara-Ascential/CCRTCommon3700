//*************************************************************************
// Description:
//   Class that provides the methods to create a thead pool and manage it.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/ThreadPool.h $
// 
// 6     10/30/07 3:51p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 5     10/26/06 9:22a Rwiersem
// Changes for the 06102701 core release:
// 
// Added StopThreadPool().
// 
// 4     7/20/05 8:53p Cward
// Support of Unblocking Function added
//
// 3     6/24/04 12:53a Cward
// Removed pound if 0 block. Updated header block comments to the new
// standard.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
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
// 1     6/12/03 4:25p Khykin
// Initial checkin.
//
// 1     4/22/03 9:41a Khykin
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
// 23    4/25/02 4:42p Skypig13
// Updated with new changes
//    Revision 1.2  2002/04/11 19:19:34  erickson
//    Updated ThreadPoolBase to properly use muticies and updated tests for INamedDataBroker.  Data server runs and SIGSEGV when the Subscription manager receives a subscription request and attemtps to create the first threads in the  Notification Thread Pool (first ContextAllocate()).
//
//    Revision 1.1.1.1  2002/04/08 19:49:03  erickson
//    started
//
//
// 21    4/02/02 11:16p Skypig13
// Added thread attributes to pool attributes
//
// 20    3/22/02 1:47p Bmeinke
// Changed $Header: /Ccrt/Source/Core/ServicesLayer/Include/ThreadPool.h 6     10/30/07 3:51p Rwiersem $ to $Log: /Ccrt/Source/Core/ServicesLayer/Include/ThreadPool.h $
// 
// 6     10/30/07 3:51p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 5     10/26/06 9:22a Rwiersem
// Changes for the 06102701 core release:
// 
// Added StopThreadPool().
// 
// 4     7/20/05 8:53p Cward
// Support of Unblocking Function added
//
// 3     6/24/04 12:53a Cward
// Removed pound if 0 block. Updated header block comments to the new
// standard.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
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
// 1     6/12/03 4:25p Khykin
// Initial checkin.
//
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//  Revision 1.1.1.1  2003/02/15 00:05:48  tbochene
//
//  PooBag
//
//
// 1     1/27/03 7:10a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 23    4/25/02 4:42p Skypig13
// Updated with new changes
//  Revision 1.2  2002/04/11 19:19:34  erickson
//  Updated ThreadPoolBase to properly use muticies and updated tests for INamedDataBroker.  Data server runs and SIGSEGV when the Subscription manager receives a subscription request and attemtps to create the first threads in the  Notification Thread Pool (first ContextAllocate()).
//
//  Revision 1.1.1.1  2002/04/08 19:49:03  erickson
//  started
//  in file header comment
//
// 21    4/02/02 11:16p Skypig13
// Added thread attributes to pool attributes
// Changed m_poolAttr.handle = reinterpret_cast<dispatch_t *>(arg) to
// m_poolAttr.handle = (THREAD_POOL_HANDLE_T*)arg in StartThreadPool
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef ThreadPool_h
#define ThreadPool_h
//==============================================================================



struct ThreadPoolInfo;
class ThreadPoolBase;

// this must be declared before includes else you will go blind!

#ifndef THREAD_POOL_PARAM_T
    #define THREAD_POOL_PARAM_T ThreadPoolInfo
#endif

#ifndef THREAD_POOL_HANDLE_T
    #define THREAD_POOL_HANDLE_T ThreadPoolBase
#endif

/**
 * Object passed around by ThreadPool library.
 * @since Version 2.0
 */
struct ThreadPoolInfo
{
    /**
     * Thread object being used by the thread pool
     * @since Version 1.0
     */
    THREAD_POOL_HANDLE_T* thread;
    /**
     * Additional data needed by thread pool
     * @since Version 1.0
     */
    void* data;
};



#include <vector>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/resource.h>

#include "BepDefs.h"
#include "XmlNode.h"
#include "XmlParser.h"
#include "Debuggable.h"
#include "ThreadList.h"
#include "BepException.h"       // base exception definition
#include "GeneralUtilities.h"   // general system utilities

class ThreadTester;
class ThreadPoolTester;

/**
 * Thread Pool class that controls creation and destroying
 * of a thread pool.
 *
 * Note: The user must declare the structures handle_t
 * and param_t and must include this file prior to
 * including any other file in the system.
 *
 * @author Kevin Hykin
 * @version 1.4
 */
class ThreadPool : public ThreadList, public Debuggable
{
    friend class ThreadTester;
    friend class ThreadPoolTester;
public:
    /**
     * Definition of the function that initializes the
     * created threads.
     */
    typedef THREAD_POOL_PARAM_T *(*NewFunction)(THREAD_POOL_HANDLE_T *handle);
    /**
     * Definition of the function that blocks the thread
     * until work should be done.
     */
    typedef THREAD_POOL_PARAM_T *(*BlockingFunction)(THREAD_POOL_PARAM_T *ctp);

    /**
     * Definition of the function that is called when the
     * thread pool is destroyed the work
     */
    typedef void (*UnblockingFunction)(THREAD_POOL_PARAM_T *ctp);
    /**
     * Definition of the function that performs the work
     * once the thread is unblocked.
     */
    typedef INT32 (*HandlingFunction)(THREAD_POOL_PARAM_T *ctp);
    /**
     * Definition of the function that cleans up when the
     * thread is to be destroyed.
     */
    typedef void (*DeleteFunction)(THREAD_POOL_PARAM_T *ctp);
public:
    /**
     * Constructor method for the ThreadPool class.
     *
     */
    ThreadPool(void);
    /**
     * Destructor method for the ThreadPool class.
     *
     */
    ~ThreadPool(void);
    /**
     * Method used to initialize the thread pool.  It takes
     * a parsed XmlNode and configures itself based on the
     * parameters that were provided.
     *
     * @param config The xml node that contains the setup information.
     */
    void Initialize(const XmlNode *config);
    /**
     * Used to create a thread to start the thead pool.
     *
     * @param arg    Reference to the "handle" that the thread should
     *               use. Note: if handle is NULL the SetHandle() method
     *               should have been called prior to starting the thread.
     */
    void StartThreadPool(void *arg);
    /**
     * Used to terminate all threads in the thead pool.
     */
    virtual void StopThreadPool();
    /**
     * Method used to get the handle reference.
     *
     * @return The handle reference.
     */
    const THREAD_POOL_HANDLE_T *GetHandle();
    /**
     * Set the handle reference.
     *
     * @param handle    Reference to the new handle.
     */
    void SetHandle(THREAD_POOL_HANDLE_T *handle);
    /**
     * Set the reference to the function used to initialize
     * new threads.
     *
     * @param ncf       Reference to the function to be used to
     *                  initialize new threads.
     */
    void SetInitFunction(NewFunction ncf);
    /**
     * Set the reference to the function that the threads
     * block on.
     *
     * @param bf       Reference to the function the threads
     *                 should use to block on.
     */
    void SetBlockingFunction(BlockingFunction bf);
    /**
     * Set the reference to the function that the threads
     * block on.
     *
     * @param ubf Reference to the function the threads should use to block on.
     */
    void SetUnBlockingFunction(UnblockingFunction ubf);
    /**
     * Set the reference to the function that the threads
     * use to handle the requests they receive.
     *
     * @param hf Reference to the function the threads should use to handle 
     *           requests.
     */
    void SetHandlingFunction(HandlingFunction hf);
    /**
     * Set the function that will be used by each thread in the
     * pool when they are terminated
     *
     * @param df Reference to the function that handles the deletion of the 
     *           thread.
     */
    void SetDeleteFunction(DeleteFunction df);
    /**
     * Get the current minimum number of avaliable threads.
     *
     * @return The minimum number of avaliable threads.
     */
    INT32 GetMinThreadsAvaliable(void);
    /**
     * Set the minimum number of threads to have free at any
     * one time.
     *
     * @param minThreads The minimum number of threads to have waiting
     *                   while the pool is operating.
     */
    void SetMinThreadsAvaliable(INT32 minThreads);
    /**
     * Get the current maximum number of avaliable threads.
     *
     * @return The maximum number of avaliable threads.
     */
    int GetMaxThreadsAvaliable(void);
    /**
     * Set the maximum number of threads avaliable before
     * the pool will start to delete them.
     *
     * @param maxThreads The maximum number of threads to have waiting
     *                   while the pool is operating.
     */
    void SetMaxThreadsAvaliable(INT32 maxThreads);
    /**
     * Get the current thread increment value. This is the value
     * that specifies the number of threads to launch when the minumum
     * blocked threads threshold is met.
     *
     * @return The minimum number of avaliable threads.
     */
    INT32 GetThreadIncrement(void);
    /**
     * Set the number of threads to launch when the minimum
     * threads on the system is less than the specified
     * minimum.
     *
     * @param increment The number to increment by when the minimum number
     *                  is reached.
     */
    void SetThreadIncrement(INT32 increment);
    /**
     * Get the maximum number of threads that can be run by the pool.
     *
     * @return The current maximum number of threads the pool will handle.
     */
    INT32 GetMaximumThreads(void);
    /**
     * Set the maximum number of threads that the pool will manage.
     *
     * @param maxNumber Maximum number of threads to have running at any one
     *                  time.
     */
    void SetMaximumThreads(INT32 maxNumber);
    /**
     * Add myself to the thread list.
     */
    void AddSelf(void);
    /**
     * Remove my thread ID from the thread list
     */
    void RemoveSelf(void);
protected:
    /**
     * Reference to the created thread pool.
     */
    thread_pool_t *m_TPR;
private:
    /**
     * Thread pool attributes structure.
     */
    thread_pool_attr_t m_poolAttr;

    /**
     * Attributes for individual threads in the thread pool
     * @since Version 1.4
     */
    pthread_attr_t m_attr;
    /**
     * lock used to allow multiple threads to read but only one to write.
     */
    pthread_rwlock_t m_dataLock;
};


#endif  // ThreadPool_h
