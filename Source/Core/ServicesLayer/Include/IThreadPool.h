//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/IThreadPool.h 1     12/21/03 6:27p Cward $
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
//    $Log: /Core/ServicesLayer/Include/IThreadPool.h $
// 
// 1     12/21/03 6:27p Cward
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
// 1     6/12/03 4:15p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:40  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:06a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
// 
// 2     9/10/02 6:32p Tbochene
// Updated constants to be part of class and prototype of Housekeeping()
// 
//
//*************************************************************************

#ifndef IThreadPool_h
#define IThreadPool_h

class XmlNode;

#include <pthread.h>

/**
 * Interface definition for a thread pool
 *
 * @author Tim Bochenek
 * @version 1.0
 * @since Version 1.0
 */
class IThreadPool
{
public:
    friend class AbstractThread;        // Base Thread class
    /**
	 * Default minimum increment value for thread pool
	 * @since Version 1.0
	 */
	const static unsigned int DefaultIncrement = 1;
	/**							
	 * Default minimum low water value for thread pool
	 * @since Version 1.0
	 */
	const static unsigned int DefaultLowWater = 1;
	/**
	 * Default maximum high water value for thread pool
	 * @since Version 1.0
	 */
	const static unsigned int DefaultHighWater = 10;
	/**
     * Identifier indicating thread pool is starting
     * @since Version 1.0
     */
    const static unsigned long Go = 0;
    /**
     * Identifier indicating thread finished block state
     * @since Version 1.0
     */
	const static unsigned long BlockDone = IThreadPool::Go + 1;
    /**
     * Identifier indicating thread finished handle state
     * @since Version 1.0
     */
	const static unsigned long HandleDone = IThreadPool::BlockDone + 1;
    /**
     * Default constructor
     * @since Version 1.0
     */
    IThreadPool(void)
    {
    }
    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~IThreadPool(void)
    {
    }
    /**
     * Initialize the thread pool.
     *
     * @param configuration
     *               Xml Node containing the parameters for the behavior of
     *               the thread pool.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode* configuration) = 0;
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
    virtual void Initialize(const bool Debug = false, const bool Go = false,
                            const unsigned int Low = DefaultLowWater,
                            const unsigned int Hi = DefaultHighWater,
                            const unsigned int Max  = 2*DefaultHighWater,
                            const unsigned int Inc = 1) = 0;
    /**
     * Start the thread pool
     * @since Version 1.0
     */
    virtual void Start(void) = 0;
    /**
     * Sets the minimum number of threads that will be blocked
     * waiting to service clients
     *
     * @param limit  Value to set for low water limit
     * @since Version 1.0
     */
    virtual void SetLowWater(const unsigned int limit) = 0;
    /**
     * Sets the maximum number of threads that will be blocked
     * waiting to service clients
     *
     * @param limit  Value to set for high water limit
     * @since Version 1.0
     */
    virtual void SetHighWater(const unsigned int limit) = 0;
    /**
     * Sets the maximum number of threads that will be created
     * blocked threads plus threads servicing clients
     *
     * @param limit  Value to set for maximum limit
     * @since Version 1.0
     */
    virtual void SetMaximum(const unsigned int limit) = 0;
    /**
     * Sets the number of threads that will be created when the
     * minimum number of blocking threads limit was reached
     *
     * @param increment  Value to set for increment
     * @since Version 1.0
     */
    virtual void SetIncrement(const unsigned int increment) = 0;
    /**
     * Gets the minimum number of threads that will be blocked
     * waiting to service clients
     *
     * @return  Value to set for minimum limit
     * @since Version 1.0
     */
    virtual inline const unsigned int GetLowWater(void) = 0;
    /**
     * Gets the maximum number of threads that will be blocked
     * waiting to service clients
     *
     * @return  Value to set for maximum limit
     * @since Version 1.0
     */
    virtual inline const unsigned int GetHighWater(void) = 0;
    /**
     * Gets the maximum number of threads that will be created
     * blocked threads plus threads servicing clients
     *
     * @return  Value to set for maximum limit
     * @since Version 1.0
     */
    virtual inline const unsigned int GetMaximum(void) = 0;
    /**
     * Gets the number of threads that will be created when the
     * minimum number of blocking threads limit was reached
     *
     * @return  Value to set for maximum limit
     * @since Version 1.0
     */
    virtual inline const unsigned int GetIncrement(void) = 0;
    /**
     * Get the total number of threads blocking
     *
     * @return Total number of threads blocked in the thread pool
     * @since Version 1.0
     */
    virtual inline const unsigned int GetBlockedThreadCount(void) = 0;
    /**
     * Get the total number of threads
     *
     * @return Total number of threads getting action in the thread pool
     * @since Version 1.0
     */
    virtual inline const unsigned int GetHandlingThreadCount(void) = 0;
    /**
     * Get the total number of threads
     *
     * @return Total number of threads in the thread pool
     * @since Version 1.0
     */
    virtual inline const unsigned int GetThreadCount(void) = 0;
protected:
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
	virtual const bool Housekeeping(const pthread_t& Who, const unsigned long Reason) = 0;
};

#endif // IThreadPool_h
