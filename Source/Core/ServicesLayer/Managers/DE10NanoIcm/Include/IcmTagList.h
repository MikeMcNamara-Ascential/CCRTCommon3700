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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/Include/IcmTagList.h $
// 
// 3     4/13/06 4:04a Cward
// Moved implementation to source file
//
// 2     2/15/06 4:08p Bmeinke
// Moved all code to a cpp file
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

#ifndef _ICM_TAG_LIST_H_INCLUDED
#define _ICM_TAG_LIST_H_INCLUDED

/*******************
 include files
 ******************/
#include <pthread.h>
#include <stdint.h>		// uint32_t, uint64_t, etc

#include <string>
#include <map>

#include "tchar.h"
#include "Bepos.h"			// Standard BEP wrappers
#include "IcmDefs.h"

using namespace std;
/**
 * List of tag strings to be inserted into the ascii log files
 */
typedef multimap< uint32_t, string>		IcmTagList_t;
typedef IcmTagList_t::iterator			IcmTagListItr_t;
typedef IcmTagList_t::const_iterator	IcmTagListCItr_t;
typedef IcmTagList_t::value_type		IcmTagListVal_t;

/**
 * Object used to hold a list of data tags. Used by the IcmMgr to tag
 * data points during a test in response to a client's data tag
 * command.
 *
 * @author Brian Meinke
 * @version 1
 * @since December 28, 2001
 */
class IcmTagList : public IcmTagList_t
{
public:
	IcmTagList();

	~IcmTagList();

	/**
	 * Adds a tag to the tag list
	 *
	 * @param index   Index of the tag to be added
	 * @param tagName New tag to add
	 */
	void AddTag( uint32_t index, const string &tagName);

	/**
	 * Test if the tag list contains any tags
	 *
	 * @return true if the tag list is empty
	 */
	bool Empty(void);

	/**
	 * Adds a tag to the tag list
	 *
	 * @param index   Index of the tag to be added
	 * @param tagName New tag to add
	 */
	void AddTag( uint32_t index, const char *tagName);

	/**
	 * Returns the number of tags currently in the list
	 *
	 * @return The number of tags currently in the list
	 */
	int GetSize(void);

	/**
	 * Clears all tags in the tag list
	 */
	void Clear();

	/**
	 * Returns an iterator for the first item in the tag list with the
	 * given index
	 *
	 * @param index  Index of the tag to be found
	 * @return Iterator for the first item in the tag list with the given index
	 */
	IcmTagListItr_t FindFirstTag( uint32_t index);

	/**
	 * Returns an iterator to the next item in the tag list with an index
	 * matching the index of the given iterator
	 *
	 * @param itr    Iterator pointing to a valid item in the tag list
	 * @return Iterator to the next item in the tag list with an index matching
	 *         the index of the given iterator
	 */
	IcmTagListItr_t FindNextTag( const IcmTagListItr_t &itr);

	/**
	 * Attempts to lock the object for extended use.
	 *
	 * @param block  Flag indicating if caller wants to block until the lock can
	 *               be accomplished (block=true), or retrurn immediately if the
	 *               lock cannot be obtained (block=false).
	 * @return EOK if successful, other value if an error occurrs
	 */
	int Lock( bool block=true);

	/**
	 * Releases a lock on the object (after a successful call to Lock())
	 *
	 * @return EOK if successful, other value if an error occurrs
	 */
	int Unlock();

private:
	/**
	 * Object initialization. Initializes the mutex.
	 */
	void Initialize();

	/**
	 * Mutex used to synchronize access to the tag name
	 */
	pthread_mutex_t		m_tagLock;

};


#endif // _ICM_TAG_LIST_H_INCLUDED
