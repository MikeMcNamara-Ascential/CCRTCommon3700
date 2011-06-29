//*************************************************************************
// $Archive: /Ccrt/Source/Core/ServicesLayer/Include/AtomicThread.h $
// $Author: Rwiersem $
//   $Date: 10/30/07 3:29p $
//
// FILE DESCRIPTION:
//   Base server for all iterative servers.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/AtomicThread.h $
// 
// 4     10/30/07 3:29p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 2     10/24/03 11:12a Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 2     10/10/03 9:19a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 2     6/24/03 7:39a Khykin
// Updated with changes made by Tim.
//
// 19    5/20/03 2:15p Tbochene
// Removed pthread_cancel from destructor and added delete of thread after
// Execute() has returned
//
// 18    4/09/03 10:05a Tbochene
// Added cancel cleanup handling
//
// 17    2/05/02 1:44p Skypig13
// Removed warnings
//
// 16    1/10/02 11:50a Skypig13
// Update destructor to join thread and ExecuteMethod()
//
// 15    10/18/01 3:52p Skypig13
// Update to pass in detach state, remove some old methods not needed
// anymore
//
// 14    10/10/01 9:54a Skypig13
// Removed thread tracking inside thread
//
// 13    10/09/01 1:06p Brian.meinke
// Added GetThreadId() public method
//
// 12    9/18/01 7:23a Skypig13
// Removed pulse sending upon delete and removed pthread_cancel() from
// destructor
//
// 11    9/14/01 1:52p Skypig13
// Fixed up destructor and made to be created detached;
//
// 10    9/07/01 3:22p Kh
// Updating the EINTR handling.
//
// 9     9/07/01 10:55a Skypig13
// Clean up init and tear down
//
// 8     9/04/01 9:02p Skypig13
// Added pthred_cancel() in destructor and additional diagnostics
//
// 7     9/01/01 5:17p Skypig13
// Moved SetStatus() immediately after Execute() so status read
// will be correct before pulse is sent (prevent race condition)
//
// 6     9/01/01 5:05p Skypig13
// No longer Debuggable
//
// 5     8/31/01 5:57p Skypig13
// Latest version
//
// 4     8/31/01 5:26p Skypig13
// Updated
//
// 3     8/31/01 3:29p Skypig13
// Latest revision
//
// 2     8/30/01 4:13p Skypig13
// Updates to be abstract base class
//
// 1     8/29/01 5:00p Skypig13
// Class encapsulating a POSIX thread
//
//
//*************************************************************************
#ifndef AtomicThread_h
#define AtomicThread_h

#include <string>
//#include <errno.h>
#include <pthread.h>

//#include "Bepos.h"
#include "Status.h"
#include "BepDefs.h"
//#include "BepException.h"
//#include "IInterProcessCommunication.h"

class AtomicThreadTest; // Forward reference to test class

/**
* Thread running constant
* @since Version 1.0
*/
const std::string ThreadRunning("Running");
/**
 * Thread suspended constant
 * @since Version 1.0
 */
const std::string ThreadSuspended("Suspended");
/**
 * Thread terminated constant
 * @since Version 1.0
 */
const std::string ThreadTerminated("Terminated");
/**
 * Class that encapsulates the functionality of a single POSIX thread
 *
 * @author Tim Bochenek
 * @version 1.3
 * @since 1.0
 */
class AtomicThread : public Status
{
public:
    friend class AtomicThreadTest;  // Test class
    
    /**
     * Default constructor
     *
     * @param detachState
     *               Set up detached state (PTHREAD_CREATE_JOINABLE, PTHREAD_CREATE_DETACHED)
     * @since Version 1.0
     */
    AtomicThread(UINT32 detachState = PTHREAD_CREATE_JOINABLE);
    
    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~AtomicThread(void);

    /**
     * Initializer method
     *
     * @param detachState
     *               Set up detached state (PTHREAD_CREATE_JOINABLE, PTHREAD_CREATE_DETACHED)
     * @since Version 1.0
     */
    void Initialize(UINT32 detachState = PTHREAD_CREATE_JOINABLE);

    /**
     * Starts thread if it was created suspended
     *
     * @param deleteWhenDone
     *               Flag indicating if the thread object should be automatically deleted when
     *               finished running. Set to false if you want to manage the lifetime of the
     *               thread object. Default is true
     * @since Version 1.0
     */
    void Resume(bool deleteWhenDone=true);
    
    /**
     * Checks if thread is running
     *
     * @return If thread is running
     * @since Version 1.0
     */
    const bool IsThreadRunning(void);

    /**
     * Checks if thread is done
     *
     * @return If thread is done (terminated)
     * @since Version 1.0
     */
    const bool IsThreadTerminated(void);

    /**
     * Returns the thread Id
     *
     * @return Thread Id
     * @since Version 1.0
     */
    INT32 GetId(void);
    /**
     * Get the initial thread id from the stored
     * member variable.
     *
     * Added by Greg Pattison
     *
     * @return inital thread id
     * @since 06/17/04
     */
    pthread_t GetInitialThreadId(void);

protected:

    /**
     * Funciton to be executed when thread is run
     *
     * @return Status
     * @since Version 1.0
     */
    virtual void* Execute(void) = 0;

    /**
     * Function prototype for function that is called by thread.
     *
     * @param thread Pointer to the thread object to execute.
     * 
     * @return Status
     */
    static void *ExecuteFunction(void *thread);

    /**
     * Cleanup handler in case the thread is canceled
     *
     * @param thread Pointer to thread object
     * @since Version 1.3
     */
    static void Cancel(void *thread);

    /**
     * Returns the thread ID of the underlying OS thread
     *
     * @return The thread ID of the underlying OS thread
     */
    pthread_t GetThreadId() const;

    /**
     * Flag indicating if the thread object should be automatically deleted when
     * finished running. Set to false if you want to manage the lifetime of the
     * thread object. Default is true
     */
    bool        m_deleteWhenDone;

private:
    /**
     * Creaetes thread to run
     * @since Version 1.0
     */
    void CreateThread(void);
    /**
     * Set the initial thread id
     *
     * Added by Greg Pattison
     *
     * @param threadId Value to set the initial thread Id to
     * @since 06/17/04
     */
    void SetInitialThreadId(pthread_t &threadId);
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
     * Store the original thread Id
     *
     * Added by Greg Pattison
     * @since 06/17/04
     */
    pthread_t m_initialThreadId;
};
#endif // AtomicThread_h
