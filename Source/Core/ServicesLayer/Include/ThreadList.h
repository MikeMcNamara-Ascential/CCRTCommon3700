//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Include/ThreadList.h $
// $Author: Cward $
//   $Date: 1/16/04 4:45p $
//
// Description:
//	Thread safe data type definitions for a list of thread id's.
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
//     $Log: /Core/ServicesLayer/Include/ThreadList.h $
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 12:00p Khykin
// Updated with latest mutex changes.
//
// 2     10/10/03 9:31a Khykin
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
// 1     6/12/03 4:25p Khykin
// Initial checkin.
//
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:48  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:10a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 2     9/24/01 4:12p Dan.erickson
// Added removeAll method to remove all threads from a list.
//
// 1     9/20/01 10:42a Brian.meinke
// Thread safe data type definitions for a list of thread id's.
//
//******************************************************************************

#ifndef _THREAD_LIST_H_INCLUDED
#define _THREAD_LIST_H_INCLUDED

//******************************************************************************
// Forward Class Definitions
//******************************************************************************
class ThreadList;

//******************************************************************************
// Header Files
//******************************************************************************
#include <pthread.h>			// basic thread and mutex
#include <map>					// STL map

//******************************************************************************
// Type Definitions
//******************************************************************************

/**
 * Base data type for our list of threads which will need to be killed when we
 * reconfigure or terminate
 */
typedef	std::map<pthread_t, pthread_t>	ThreadList_t;

/**
 * Iterator for ThreadList_t data type
 */
typedef	ThreadList_t::iterator			ThreadListItr_t;

/**
 * Constant iterator for ThreadList_t data type
 */
typedef	ThreadList_t::const_iterator	ThreadListCItr_t;

/**
 * Value type contained by ThreadList_t data type
 */
typedef	ThreadList_t::value_type		ThreadListVal_t;

/**
 * Thread safe list to maintain cancleable threads
 *
 * @author Brian Meinke
 * @version 1.0
 * @see std::vector
 */
class ThreadList : public ThreadList_t
{
public:
	ThreadList();
	
	~ThreadList();					
	
	/**
	 * Adds a thread ID to the list
	 *
	 * @param newThread Thread ID to add
	 */
	void add( pthread_t newThread);

	/**
	 * Removes a thread from the list and optionally cancels the thread
	 *
	 * @param newThread Thread ID to remove
	 * @param cancel    Flag to cancel thread's execution before removing from list
	 */
	void remove( pthread_t newThread, bool cancel/*=true*/);
	
    /**
	 * Remove all threads from the list.
	 */
	void removeAll(void);


private:
	/**
	 * Mutex used to lock the thread list for exclusive access
	 */
    pthread_mutex_t		m_listMutex;
};


#endif // _THREAD_LIST_H_INCLUDED

