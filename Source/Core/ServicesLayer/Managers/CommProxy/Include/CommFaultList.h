//******************************************************************************
// Description:
//	Object used to hold a list of (serial) communication faults
//
//==============================================================================
// Copyright (c) 1999-2004 Burke E. Porter Machinery
// 	All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Include/CommFaultList.h $
// 
// 2     4/13/06 3:57a Cward
// cleanup
//
// 2     2/15/06 4:06p Bmeinke
// Fixed the #endif line at the very bottom of the file
//
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     7/07/04 4:54p Bmeinke
// Object used to hold a list of active communication faults
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _COMMFAULTLIST_H_INCLUDED_
#define _COMMFAULTLIST_H_INCLUDED_
//==============================================================================
#include <string>
#include <set>

#include "BepSync.h"

typedef std::set< std::string> 					CommFaultList_t;
typedef CommFaultList_t::iterator				CommFaultListItr_t;
typedef CommFaultList_t::const_iterator			CommFaultListCItr_t;

/**
 * Object used to hold a list of fault tags that we
 * have sent to the FaultServer
 */
class CommFaultList : public CommFaultList_t
{
public:
	CommFaultList();

	~CommFaultList();

	/**
	 * Locks the list for exclusive access
	 *
	 * @return EOK if the list is successfully locked
	 */
	int Lock();

	/**
	 * Unlocks the list
	 *
	 * @return EOK if the list is successfully unlocked
	 */
	int Unlock();

	/**
	 * Adds the given fault to the fault list
	 *
	 * @param faultTag Fault to be added to the fault list
	 *
	 * @return true if the fault was successfully added
	 */
	bool AddFault( const std::string &faultTag);

	/**
	 * Removes the given fault from the fault list
	 *
	 * @param faultTag Fault to be removed from the fault list
	 *
	 * @return true if the fault was successfully removed
	 */
	bool RemoveFault( const std::string &faultTag);

	/**
	 * Checks to see if the given fault is contained in the list
	 *
	 * @param faultTag Fault tag to find in the list
	 *
	 * @return true if the fault is contained in the list
	 */
	bool FindFault( const std::string &faultTag);

private:
	/**
	 * Mutex used to guard against simultaneous modifications to the list
	 */
	mutable BepMutex	m_listLock;
};

#endif // _COMMFAULTLIST_H_INCLUDED_

