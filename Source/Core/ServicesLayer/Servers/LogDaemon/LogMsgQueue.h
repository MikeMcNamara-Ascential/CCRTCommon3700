//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/LogMsgQueue.h $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		LogMsgQueue
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/LogMsgQueue.h 1     12/21/03 6:41p Cward $
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/LogMsgQueue.h $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.5  2003/10/22 15:19:16  tenharmsel
//    Working on making the Log Daemon tests work
//
//    Revision 1.4  2003/10/07 23:19:47  swieton
//    Added stuff for mutext tests
//
//    Revision 1.3  2003/10/02 23:16:31  swieton
//    Using internal logger output
//
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 1     8/07/03 9:23a Khykin
// Updated with changes from Flatrock.
//    Revision 1.6  2003/07/25 17:08:14  swieton
//    Added friend logMsgQueueTest
//
//    Revision 1.5  2003/07/11 21:17:02  crosby
//    Using LogMessage* instead of string
//
//    Revision 1.4  2003/07/10 22:49:09  crosby
//    Implemented.  This class now represents a synchronized producer/consumer pipe.
//
//    Revision 1.3  2003/07/10 19:06:06  crosby
//    Changed ifdef to ifndef!
//
//
//******************************************************************************

#ifndef _LOG_MSG_QUEUE_H_INCLUDED_
#define _LOG_MSG_QUEUE_H_INCLUDED_

#include "LogMessage.h"
#include <queue>
#include <pthread.h>
#include <iostream.h>

/* convenience typedef for the string queue */
typedef std::queue<LogMessage*> LogMsgQueue_t;

/**
 * Holds log messages from LogDaemon's receptor thread and stores them 
 * before LogWriter uses them.
 */
class LogMsgQueue {
	friend class LogMsgQueueTest;
	public:
		LogMsgQueue(unsigned int size);
		~LogMsgQueue();
		void Enqueue(LogMessage*);
		LogMessage* Dequeue();
		int Size() {return queue.size();}

	private:
		pthread_mutexattr_t lockAtts;
		pthread_mutex_t lock;    // lock for the queue
		pthread_cond_t notEmpty; // signal for "no longer empty"
		pthread_cond_t notFull;  // signal for "no longer full"
		LogMsgQueue_t queue;
		unsigned int maxSize;
};

#endif

	
