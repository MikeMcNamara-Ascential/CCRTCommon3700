//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/ThreadPoolBase.h 5     10/30/07 3:52p Rwiersem $
//
// FILE DESCRIPTION:
//      This is a base thread pool class which provides all the functionality
//      and behavior of the thread pool.  Classes derived from this class
//      implement the virtual methods Allocate, Block, Handle, and Free.
//      These methods in the ervied classes should also call the
//      corresponding methods in the base class.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/ThreadPoolBase.h $
// 
// 5     10/30/07 3:52p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     4/13/06 3:39a Cward
// Make ThreadPool Tester a friend class
//
// 2     2/15/06 3:52p Bmeinke
// Added the "class" qualifier to the "friend ThreadPoolTester" statement
//
// 3     7/20/05 8:53p Cward
// Support for unblocking added
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/10/03 9:31a Khykin
// Updated with latest Flatrock changes.
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
// 15    4/25/02 4:43p Skypig13
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
// 11    4/02/02 11:17p Skypig13
// Updated Initialize()
//
// 10    2/18/02 8:04p Bmeinke
// Change ThreadPool* references to THREAD_POOL* in static methods
//
// 9     2/07/02 9:51p Skypig13
// Removed defines
//
// 8     2/05/02 12:51a Skypig13
// Big changes matching BepServer
//
//*************************************************************************

#ifndef ThreadPoolBase_h
#define ThreadPoolBase_h

class IThread;
class ThreadPoolBase;
class ThreadPoolTester;


#include "IThread.h"
#include "XmlNode.h"


#include "ThreadPool.h"

/**
 * This is the base class for thread pools using the existing
 * ThreadPool class.
 *
 * @author Tim Bochenek
 * @version 2.1
 * @see Thread
 * @since Version 1.0
 */
class ThreadPoolBase : public ThreadPool, public IThread
{
public:
    friend class ThreadPoolTester;    // Testing class
    /**
     * Constructor method for the ThreadPoolBase class.
     * @since Version 1.0
     */
    ThreadPoolBase(void);
    /**
     * Destructor method for the ThreadPoolBase class.
    * @since Version 1.0
     */
    virtual ~ThreadPoolBase(void);

    /**
     * Initialize the thread pool.
     *
     * @param config XML Node containing the parameters for the behavior of the 
     *               thread pool.
     */
    virtual void Initialize(const XmlNode *config);

    /**
     * Initialize the static muticies
     * @since Version 2.1
     */
    static void InitializeMuticies(ThreadPoolBase* intitalizer);

    /**
     * Method to call when we need to create a new thread.
     *
     * NOTE:  ThreadPoolBase::Allocate() must be called first
     * in derived class Allocate()
     *
     * @param thread Reference to the ThreadPool object.
     * @return Input to the blocking function.
     * @since Version 1.0
     */
    static THREAD_POOL_PARAM_T* ContextAllocate(THREAD_POOL_HANDLE_T *thread);

    /**
     * Method to call to perform the blocking operation.
     *
     * NOTE:  ThreadPoolBase::Block() must be called first
     * in derived class Block()
     *
     * @param param  Result of the ContextAllocate method.
     * @return Input to the handling function.
     * @since Version 1.0
     */
    static THREAD_POOL_PARAM_T* BlockingMethod(THREAD_POOL_PARAM_T *param);

    /**
     * Method to call to perform the unblocking operation.
     *
     * NOTE:  ThreadPoolBase::Unblock() must be called first
     * in derived class Unblock()
     *
     * @param param  Result of the ContextAllocate method.
     * @since Version 1.0
     */
    static void UnblockingMethod(THREAD_POOL_PARAM_T *param);

    /**
     * Method to call to perform the work.
     *
     * NOTE:  ThreadPoolBase::Handle() must be called first
     * in derived class Handle()
     *
     * @param param  Result of the BlockingMethod method.
     * @return Zero.
     * @since Version 1.l
     */
    static INT32 HandlingMethod(THREAD_POOL_PARAM_T *param);

    /**
     * Method to call to destroy a thread.
     *
     * NOTE:  ThreadPoolBase::Free() must be called first
     * in derived class Free()
     *
     * @param param  Output from ContextAllocate method.
     * @since Version 1.0
     */
    static void ContextFree(THREAD_POOL_PARAM_T *param);

protected:
    /**
     * Dervied class method for performing setup for each thread.
     * @since Version 1.0
     */
    virtual void Allocate(ThreadPoolInfo *param);

    /**
     * Dervied class method for performing the blocking operation.
     * @since Version 1.0
     */
    virtual void Block(ThreadPoolInfo *param);

    /**
     * Dervied class method for performing the unblocking operation.
     * @since Version 1.0
     */
    virtual void Unblock(ThreadPoolInfo *param);
    /**
     * Derived class method for performing the work.
     *
     * @return Must return zero.
     * @since Version 1.0
     */
    virtual INT32 Handle(ThreadPoolInfo *param);
    /**
     * Dervied class method for destroying threads.
     * If any memory was dynamicaly declared, it must be deleted
     * in the derived Free method.
     * @since Version 1.0
     */
    virtual void Free(ThreadPoolInfo *param);

private:
protected:
    enum
    {
        allocate=0, block, unblock, handle, free
    };
    /**
     * Array of mutex objects to use for synchronization
     * @since Version 1.0
     */
    static pthread_mutex_t m_muticies[free+1];
    /**
     * Flag indicating whether the muticies have been initialized.
     * Prevents multiple initialization of the muticies
     * @since Version 2.1
     */
    static bool m_muticiesInitialized;
};
#endif   //ThreadPoolBase_h

