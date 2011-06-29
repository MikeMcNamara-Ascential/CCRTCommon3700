//******************************************************************************
// Description:
//	Object used to hold a list of (serial) communication faults
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
//     $Log: /Core/ServicesLayer/Managers/CommProxy/Common/CommFaultList.cpp $
// 
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 2     1/17/05 10:05a Bmeinke
// Wrapped the insertion of a fault tag into the fault list inside
// AddFault() with a try/catch
//
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
//
// 1     7/07/04 4:54p Bmeinke
// Object used to hold a list of active communication faults
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <errno.h>

#include "CommFaultList.h"

CommFaultList::CommFaultList() : CommFaultList_t()
{
}

CommFaultList::~CommFaultList()
{
}

/**
 * Locks the list for exclusive access
 *
 * @return EOK if the list is successfully locked
 */
int CommFaultList::Lock()
{
	return( m_listLock.Acquire());
}

/**
 * Unlocks the list
 *
 * @return EOK if the list is successfully unlocked
 */
int CommFaultList::Unlock()
{
	return( m_listLock.Release());
}

/**
 * Adds the given fault to the fault list
 *
 * @param faultTag Fault to be added to the fault list
 *
 * @return true if the fault was successfully added
 */
bool CommFaultList::AddFault( const std::string &faultTag)
{
	std::pair< CommFaultListItr_t, bool>	retVal( end(), false);

	if( (errno=Lock()) == EOK)
	{
		try
		{
			retVal = insert( faultTag);
		}
		catch( ...)
		{
			retVal.second = false;
		}

		Unlock();
	}

	return( retVal.second);
}

/**
 * Removes the given fault from the fault list
 *
 * @param faultTag Fault to be removed from the fault list
 *
 * @return true if the fault was successfully removed
 */
bool CommFaultList::RemoveFault( const std::string &faultTag)
{
	bool retVal = false;

	if( (errno=Lock()) == EOK)
	{
		if( erase( faultTag) != 0)
		{
			retVal = true;
		}

		Unlock();
	}

	return( retVal);
}

/**
 * Checks to see if the given fault is contained in the list
 *
 * @param faultTag Fault tag to find in the list
 *
 * @return true if the fault is contained in the list
 */
bool CommFaultList::FindFault( const std::string &faultTag)
{
	bool retVal = false;

	if( (errno=Lock()) == EOK)
	{
		if( find( faultTag) != end())
		{
			retVal = true;
		}

		Unlock();
	}

	return( retVal);
}

