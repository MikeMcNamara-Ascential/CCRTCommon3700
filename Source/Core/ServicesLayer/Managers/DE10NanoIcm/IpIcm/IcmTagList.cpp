//******************************************************************************
// Description:
//	Object to manage log file tags for ICM data
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/IcmTagList.cpp $
//
// 1     4/13/06 3:11a Cward
// Moved implementation from headers to cpp
//
// 1     2/15/06 12:05p Bmeinke
// Moved code from header to cpp file
//
// 1     2/03/06 1:55p Bmeinke
// Moved code from the header to cpp file
//
// 2     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:28p Cward
//
// 3     10/24/03 1:17p Khykin
// Updated with mutex changes.
//
// 2     10/10/03 9:40a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:34a Khykin
// Initial checkin
//
// 2     8/07/03 7:30a Khykin
// Updated for new directory structure.
//
// 1     6/12/03 4:12p Khykin
// Initial checkin.
//
// 2     5/19/03 11:19a Khykin
// Updated with Brians latest changes.
//
// 1     4/30/03 4:42p Bmeinke
// Migration to new personalized configuration management structure
//
// 3     9/24/02 11:53a Tbochene
// Removed compiler warning on line 91
//
// 2     8/23/02 11:26a Kh
// Updating for changes at the pit machine.
//
// 1     1/07/02 10:34a Brian.meinke
// Object used to manage tags assigned by ICM clients to data array index
// points
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================


/*******************
 include files
 ******************/
#include <stdio.h>			// perror, printf
#include <stdarg.h>			// Variable argument stuff
#include <stdlib.h>			// rand
#include <string.h>			// memmove, strlen, etc
#include <time.h>			// POSIX Timers
#include <signal.h>			// POSIX signals
#include <semaphore.h>		// POSIX semaphores
#include <errno.h>
#include <pthread.h>
#include <inttypes.h>		// uint32_t, uint64_t, etc

#include <string>
#include <map>

#include "tchar.h"
#include "Bepos.h"			// Standard BEP wrappers
#include "IcmTagList.h"

IcmTagList::IcmTagList() : IcmTagList_t()
{
}

IcmTagList::~IcmTagList()
{
}

/**
 * Adds a tag to the tag list
 *
 * @param index   Index of the tag to be added
 * @param tagName New tag to add
 */
void IcmTagList::AddTag(uint32_t index, const string &tagName)
{
    Lock();
    insert(IcmTagListVal_t(index, tagName));
    Unlock();
}

/**
 * Test if the tag list contains any tags
 *
 * @return true if the tag list is empty
 */
bool IcmTagList::Empty(void)
{
    Lock();
    bool isEmpty = empty();
    Unlock();
    return(isEmpty);
}

/**
 * Adds a tag to the tag list
 *
 * @param index   Index of the tag to be added
 * @param tagName New tag to add
 */
void IcmTagList::AddTag(uint32_t index, const char *tagName)
{
    string tag(tagName);
    AddTag(index, tag);
}

/**
 * Returns the number of tags currently in the list
 *
 * @return The number of tags currently in the list
 */
int IcmTagList::GetSize(void)
{
    Lock();
    int listSize = size();
    Unlock();
    return(listSize);
}

/**
 * Clears all tags in the tag list
 */
void IcmTagList::Clear()
{
    Lock();
    clear();
    Unlock();
}

/**
 * Returns an iterator for the first item in the tag list with the
 * given index
 *
 * @param index  Index of the tag to be found
 * @return Iterator for the first item in the tag list with the given index
 */
IcmTagListItr_t IcmTagList::FindFirstTag(uint32_t index)
{
    IcmTagListItr_t retVal;
    Lock();
    retVal = find(index);
    Unlock();
    return(retVal);
}

/**
 * Returns an iterator to the next item in the tag list with an index
 * matching the index of the given iterator
 *
 * @param itr    Iterator pointing to a valid item in the tag list
 * @return Iterator to the next item in the tag list with an index matching
 *         the index of the given iterator
 */
IcmTagListItr_t IcmTagList::FindNextTag(const IcmTagListItr_t &itr)
{
    IcmTagListItr_t nextItem = itr;

    Lock();
    // If the iterator is valid
    if (itr != end())
    {
        // Move to next item in the list
        nextItem++;
        // No match- invalidate iterator
        if (nextItem->first != itr->first)
            nextItem = end();
    }
    Unlock();


    return(nextItem);
}

/**
 * Attempts to lock the object for extended use.
 *
 * @param block  Flag indicating if caller wants to block until the lock can
 *               be accomplished (block=true), or retrurn immediately if the
 *               lock cannot be obtained (block=false).
 * @return EOK if successful, other value if an error occurrs
 */
int IcmTagList::Lock(bool block)
{
    int retVal;
    if (block)
        retVal = pthread_mutex_lock(&m_tagLock);
    else
        retVal = pthread_mutex_trylock(&m_tagLock);
    return(retVal);
}

/**
 * Releases a lock on the object (after a successful call to Lock())
 *
 * @return EOK if successful, other value if an error occurrs
 */
int IcmTagList::Unlock()
{
    return(pthread_mutex_unlock(&m_tagLock));
}

/**
 * Object initialization. Initializes the mutex.
 */
void IcmTagList::Initialize()
{
    pthread_mutexattr_t attr;

    memset(&attr, 0, sizeof(attr));
    memset(&m_tagLock, 0, sizeof(m_tagLock));

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_ENABLE);
    pthread_mutex_init(&m_tagLock, &attr);
}

