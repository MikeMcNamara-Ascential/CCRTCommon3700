//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Include/AbstractThread.h $
// $Author: Rwiersem $
//   $Date: 10/30/07 3:28p $
//
// Description:
//  Base class for POSIX thread wrapper
//
//==============================================================================
//
//     Copyright  2002 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/AbstractThread.h $
// 
// 3     10/30/07 3:28p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 2     10/24/03 11:11a Khykin
// Updated with latest changes from Korea and Flatrock.
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
// 1     6/12/03 4:04p Khykin
// Initial checkin.
//
// 1     4/22/03 9:40a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:32  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:02a Khykin
// Initial Checkin
//
// 1     1/18/03 8:00a Khykin
// Initial Checkin From DCTC
//
// 7     10/07/02 9:54a Khykin
// Changing Log statement for case that might be a SIGSEG.
//
// 6     10/07/02 6:19a Khykin
// Updating for lockup issues when pthread_cancel was called.
//
// 5     9/12/02 5:47p Tbochene
// Updated to derive from ILogger
//
// 4     9/10/02 6:33p Tbochene
// Updated to allow thread to recycle
//
// 3     6/13/02 3:09p Dan.erickson
// Commented out debug messages so they do not always print.
//
// 2     6/07/02 12:04p Dan.erickson
// Limit printing of diagnostics.
//
// 1     6/03/02 1:55p Skypig13
// Thread class that is base class for threads to be used in
// TemplateThreadPool
//
//******************************************************************************

#ifndef AbstractThread_h
#define AbstractThread_h

#include <pthread.h>

#include "ILogger.h"
#include "IThreadPool.h"

/**
 * Class that encapsulates the functionality of a single POSIX thread
 *
 * @author Tim Bochenek
 * @version 1.0
 * @since 1.0
 */
class AbstractThread : public ILogger
{
public:
    /**
     * Default constructor
     * @since Version 1.0
     */
    AbstractThread(void);
    
    /**
     * Initializing constructor
     *
     * @param config Setup for object
     * @param pool   Thread pool that is managing the thread
     * @since Version 1.0
     */
    AbstractThread(const XmlNode* config, IThreadPool* pool = NULL);

    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~AbstractThread(void);                                      

    /**
     * Initializer method.
     */
    virtual void Initialize(void);

    /**
     * Starts thread if it was created suspended
     * @since Version 1.0
     */
    void Resume(void);

    /**
     * Returns the thread Id
     *
     * @return Thread Id
     * @since Version 1.0
     */
    pthread_t GetId(void);

    /**
     * Indicates if the thread was created in a thread pool
     *
     * @return If thread is in a thread pool
     * @since Version 1.0
     */
    const bool IsInThreadPool(void);

protected:
    /**
     * Gets the reference to the thread pool that the thread is in
     *
     * @return Thread pool pointer
     * @since Version 1.0
     */
    IThreadPool* GetPool(void);

    /**
     * Funciton to be called when thread is created and blocked
     * @since Version 1.0
     */
    virtual void Block(void);

    /**
     * Funciton to be called when Block() returns and should do some work
     * @since Version 1.0
     */
    virtual void Handle(void);

    /**
     * Function prototype for function that is called by thread
     *
     * @param Arg    Arguments for function to be executed (pointer to derived object)
     * @return Status
     * @since Version 1.0
     */
    static void *AbstractThread::ExecuteFunction(void *Arg);
    
    static void Cancel(void *Arg);

protected:
    /**
     * Creaetes thread to run
     * @since Version 1.0
     */
    void CreateThread(void);

    /**
     * Thread id
     * @since Version 1.0
     */
    pthread_t m_thread;
    /**
     * Attributes used to manage thread
     * @since Version 1.0
     */
    pthread_attr_t m_attributes;
    /**
     * Thred pool that thread is in
     * @since Version 1.0
     */
    IThreadPool* m_pool;
};
#endif // AbstractThread_h
