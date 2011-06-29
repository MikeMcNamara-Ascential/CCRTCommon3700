//****************************************************************************
// $Archive: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogMsgQueue.cpp $
// $Date: 10/26/06 11:20a $
//
// CLASS:
//      LogMsgQueue
//
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogMsgQueue.cpp 3     10/26/06 11:20a Rwiersem $
//
//============================================================================
//
//    Copyright (c) 1999 Burke E. Porter Machinery
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogMsgQueue.cpp $
// 
// 3     10/26/06 11:20a Rwiersem
// Changes for the 06102701 core release:
// 
// Added calls to destroy the pthread objects in the destructor.
// 
// 2     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 1     12/21/03 6:41p Cward
//    Revision 1.11  2003/10/22 15:19:16  tenharmsel
//    Working on making the Log Daemon tests work
//
//    Revision 1.10  2003/10/09 21:35:56  swieton
//    initializing memory for mutexes, fixed possible file-leak
//
//    Revision 1.9  2003/10/07 23:19:47  swieton
//    Added stuff for mutext tests
//
//    Revision 1.8  2003/10/03 16:30:22  crosby
//    Enqueue will never block;
//    changed the size check to be outside the mutex lock to avoid
//    blocking on it.
//
//    Revision 1.7  2003/10/03 13:15:05  crosby
//    Rewrote the drop-message-when-full logic, clearer to read.
//
//    Revision 1.6  2003/10/02 23:16:31  swieton
//    Using internal logger output
//
//    Revision 1.5  2003/10/02 23:07:50  swieton
//    Output now using internal logger
//
//    Revision 1.4  2003/10/02 16:16:20  crosby
//    cerr --> cout
//
//    Revision 1.3  2003/10/02 15:59:16  crosby
//    mutex type is now PTHREAD_MUTEX_ERRORCHECK.
//    locking/unlocking is now checked.
//
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
//
// 1     8/07/03 1:52p Khykin
// Initial checkin.
//    Revision 1.5  2003/07/25 17:39:08  crosby
//    fixed comments
//
//    Revision 1.4  2003/07/21 21:16:11  crosby
//    Removed recursive mutex lock semantics
//
//    Revision 1.3  2003/07/21 20:33:32  crosby
//    Recursive pthread mutex lock
//
//    Revision 1.2  2003/07/11 21:08:43  crosby
//    Now managing a queue of LogMessage* instead of string.
//
//    Revision 1.1  2003/07/10 22:53:06  crosby
//    Complete
//
//
//******************************************************************************
#include "LogMsgQueue.h"
#include "LogDaemon.h"

/**
 * Create a new LogMsgQueue with a maximum size.
 * @param size Determines the maximum number of elements allowed in the queue.
 */
LogMsgQueue::LogMsgQueue(unsigned int size) {
    // Set max queue size
    maxSize = size;

    // Zero out things before initializing them
    memset(&lockAtts, 0, sizeof(lockAtts));
    memset(&lock, 0, sizeof(lock));
    
    // Setup the mutex: non-recursive, error checking
    pthread_mutexattr_init(&lockAtts);
    pthread_mutexattr_settype(&lockAtts, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&lock, &lockAtts);
    
    // Setup the condition variables
    memset(&notEmpty, 0, sizeof(notEmpty));
    memset(&notFull, 0, sizeof(notFull));
    
    pthread_cond_init(&notEmpty, 0);
    pthread_cond_init(&notFull, 0);
    
}

LogMsgQueue::~LogMsgQueue()
{
    // destroy the pthread items
    pthread_cond_destroy(&notEmpty);
    pthread_cond_destroy(&notFull);
    pthread_mutexattr_destroy(&lockAtts);
    pthread_mutex_destroy(&lock);
}

/**
 * Place a LogMessage on the end of the queue.
 * This method will NOT block if the queue is full --
 * the message will simply be destroyed.
 * @param item The LogMessage to put on the queue
 */
void LogMsgQueue::Enqueue(LogMessage* item) {
    // Check queue size before proceeding.
    // NOTE: Technically, this is thread-unsafe.  We'll be checking
    // the queue size without locking the mutex.  This is to avoid,
    // at all costs, blocking in this method.  The risk of
    // anything worse than being slightly wrong about the queue
    // size is minimal.
    if (queue.size() >= maxSize) {
        // Queue is full.  Drop message,
        // and delete it (thus fulfilling the consumer contract)
        LogDaemon::out("LogMsgQueue::Enqueue: queue full, dropping message\n");
        delete(item);
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // This was causing lockups because there was no delay
        delay( 10);
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


        return;
    }

    // Lock the queue
    //cout << "Enqueue: Locking Mutex" << endl;
    int stat = pthread_mutex_lock(&lock);
    //cout << "Enqueue: Mutex Locked" << endl;
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogMsgQueue::Enqueue FAILED to get lock.");
        // The producer/consumer contract surrounding this queue
        // says the de-queuer process MUST deallocate LogMessage objects.
        // We're short-circuiting that:
        delete(item);
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // This was causing lockups because there was no delay
        delay( 10);
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        return;
    }

    // Add the item
    queue.push(item);

    // Send the "not empty" signal
    pthread_cond_signal(&notEmpty);

    // Unlock the queue
    stat = pthread_mutex_unlock(&lock);
    if (stat != EOK) {
        LogDaemon::out("LogMsgQueue::Enqueue: FAILED to unlock mutex.");
    }
}

/**
 * Take a LogMessage off the front of the queue.
 * This method will block if the queue is empty, wait
 * for an item to become available on the queue, then
 * return the front of the queue.
 * @return The dequeued item
 */
LogMessage* LogMsgQueue::Dequeue() {
    // Lock the queue
    int stat = pthread_mutex_lock(&lock);
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogMsgQueue::Dequeue FAILED to get lock.");
        // The LogWriter process is equipped to handle NULLs.
        return NULL;
    }

    
    // Check for empty
    while (queue.empty()) {
        // Block, waiting for "not empty" signal
        stat = pthread_cond_wait(&notEmpty, &lock);
        if(stat != EOK)
        {
            if(stat == EINVAL)
            {
                cerr << "#" << flush;
            }
        }
    }
    // Dequeue an item
    LogMessage* val = queue.front();
    queue.pop();

    // Send the "not full" signal
    pthread_cond_signal(&notFull);
    // Unlock the queue
    stat = pthread_mutex_unlock(&lock);
    if (stat != EOK) {
        LogDaemon::out("LogMsgQueue::Dequeue: FAILED to unlock mutex.");
        // nothing good can come of this
    }
    // Return the item
    return val;
}



