//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Library/ThreadList.cpp $
// $Author: Cward $
//   $Date: 1/23/04 9:25a $
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
//     $Log: /Core/ServicesLayer/Library/ThreadList.cpp $
// 
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include "ThreadList.h"

#include <string>           // STL for memset
using namespace std;



ThreadList::ThreadList() : ThreadList_t()	
{
    pthread_mutexattr_t attr;

    memset( &attr, 0, sizeof( attr));
    memset( &m_listMutex, 0, sizeof( m_listMutex));

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_ENABLE);
    pthread_mutex_init( &m_listMutex, &attr);	
}

ThreadList::~ThreadList()					
{
    pthread_mutex_destroy( &m_listMutex); 	
}

/**
 * Adds a thread ID to the list
 *
 * @param newThread Thread ID to add
 */
void ThreadList::add( pthread_t newThread)
{
    pthread_mutex_lock( &m_listMutex);
    ThreadList::insert( ThreadListVal_t( newThread, newThread));
    pthread_mutex_unlock( &m_listMutex);
}

/**
 * Removes a thread from the list and optionally cancels the thread
 *
 * @param newThread Thread ID to remove
 * @param cancel    Flag to cancel thread's execution before removing from list
 */
void ThreadList::remove( pthread_t newThread, bool cancel/*=true*/)
{
    pthread_mutex_lock( &m_listMutex);
    ThreadListItr_t	itr = ThreadList_t::find( newThread);
    if( itr != ThreadList_t::end())
    {
        ThreadList_t::erase( itr);
    }
    if( (cancel == true) && (newThread > 1))
    {
        // Set cancellation on the thread
        pthread_cancel( newThread);
        // Signal thread (to unblock)
        pthread_kill( newThread, SIGURG);
    }

    pthread_mutex_unlock( &m_listMutex);
}
/**
 * Remove all threads from the list.
 */
void ThreadList::removeAll(void)
{
    pthread_mutex_lock(&m_listMutex);
    clear();
    pthread_mutex_unlock(&m_listMutex);
}


