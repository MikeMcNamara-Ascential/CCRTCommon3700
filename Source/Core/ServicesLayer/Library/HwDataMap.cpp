//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Library/HwDataMap.cpp $
// $Author: Cward $
//   $Date: 4/13/06 3:40a $
//
// Description:
//	Data types to support lookup up of data items via tag.
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/HwDataMap.cpp $
// 
// 3     4/13/06 3:40a Cward
// Init some vars
//
// 2     2/15/06 3:54p Bmeinke
// Initialize some uninitialized variables
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 12:11p Khykin
// Updated with latest mutex changes.
//
// 2     10/10/03 9:32a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:29p Khykin
// Initial checkin.
//
// 1     4/22/03 9:40a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:30  tbochene
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
// 5     10/16/01 1:16a Brian.meinke
// clear() was using a const iterator; should be a non-const iterator
//
// 4     5/17/01 2:20p Brian.meinke
// Removed printf statements from find() methods if name does not exist in
// table
//
// 3     5/16/01 12:20p Brian.meinke
// Removed recursize attribute of mutex
// Added diagnostic print statements if an operation fails
//
// 2     4/23/01 2:57p Brian.meinke
// Seperated into .cpp and .h
//
// 1     4/23/01 10:44a Brian.meinke
// Data types to support lookup up of data items via tag.
//
//******************************************************************************

#include <errno.h>
#include <stdio.h>
#include "HwDataMap.h"

//******************************************************************************

/**
 * XML Node Tag for the Channel Index Attribute
 *
 * @see XmlString
 */
const XmlString	HwDataMap::channelIdxTag( XML_T("channelIndex"));

/**
 * XML Node Tag for the Bit Index Attribute
 *
 * @see XmlString
 */
const XmlString	HwDataMap::bitIdxTag( XML_T("bitIndex"));

/**
 * XML Node Tag for the Logic Level Attribute
 *
 * @see XmlString
 */
const XmlString	HwDataMap::logicLvlTag( XML_T("logic"));

/**
 * XML Node Tag for the Port Direction Attribute
 *
 * @see XmlString
 */
const XmlString	HwDataMap::portDirTag( XML_T("portDir"));

/**
 * XML Node Tag for the Board Index Attribute
 *
 * @see XmlString
 */
const XmlString	HwDataMap::boardIdxTag( XML_T("boardIdx"));

//******************************************************************************

HwDataMap::HwDataMap() : rawDataMap_t()
{
    pthread_mutexattr_t attr;

	memset( &attr, 0, sizeof( attr));
	memset( &m_tableLock, 0, sizeof( m_tableLock));

	pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_DISABLE);
    pthread_mutex_init( &m_tableLock, &attr);
}
HwDataMap::~HwDataMap()
{
	clear();
	pthread_mutex_destroy( &m_tableLock);
}

/**
 * Returns an iterator that points to the beginning of the data table
 *
 * @return An iterator that points to the beginning of the data table
 */
rawDataMapItr_t HwDataMap::begin()
{
	rawDataMapItr_t	retVal;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		retVal = rawDataMap_t::begin();

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::begin: %s\n", strerror( mtxErr));
	}
	return( retVal);
}

/**
 * Returns a constant iterator that points to the beginning of the data table
 *
 * @return A constant iterator that points to the beginning of the data table
 */
rawDataMapCItr_t HwDataMap::begin() const
{
	rawDataMapCItr_t retVal;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		retVal = rawDataMap_t::begin();

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::begin: %s\n", strerror( mtxErr));
	}
	return( retVal);
}

/**
 * Returns an iterator that points just a past the last item in the table
 *
 * @return An iterator that points just a past the last item in the table
 */
rawDataMapItr_t HwDataMap::end()
{
	rawDataMapItr_t	retVal;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		retVal = rawDataMap_t::end();

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::end: %s\n", strerror( mtxErr));
	}
	return( retVal);
}
/**
 * Returns a constant iterator that points just a past the last item in the table
 *
 * @return A constant iterator that points just a past the last item in the table
 */
rawDataMapCItr_t HwDataMap::end() const
{
	rawDataMapCItr_t	retVal;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		retVal = rawDataMap_t::end();

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::end: %s\n", strerror( mtxErr));
	}
	return( retVal);
}

/**
 * Inserts a new item into the table with the given tag and properties
 *
 * @param tag    Tag associated with the data item
 * @param item   Properties for the new data item
 * @return Pointer to the new data item
 */
dataMapItem_t const* HwDataMap::insert(const dataTag_t &tag, const dataMapItem_t &item)
{
	dataMapItem_t	*mapVal;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		rawDataMapItr_t itr = rawDataMap_t::find( tag);

		// If tag does not exist yet
		if( itr == rawDataMap_t::end())
		{
			// Create new object
			mapVal = new dataMapItem_t( item);
			// Add to table
			rawDataMap_t::insert( rawDataMapVal_t(tag, mapVal));
		}
		else
		{
			// Tag exists, so replace data item properties
			mapVal = itr->second;
			mapVal->bitIdx = item.bitIdx;
			mapVal->boardIdx = item.boardIdx;
			mapVal->channelIdx = item.channelIdx;
			mapVal->logicLvl = item.logicLvl;
			mapVal->portDir = item.portDir;
		}

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::insert: %s\n", strerror( mtxErr));
	}

	return( (dataMapItem_t const*)mapVal);
}

/**
 * Removes the data item with the given tag from the table
 *
 * @param tag    Tag associated with the data item
 * @return
 */
size_t HwDataMap::erase(const dataTag_t& tag)
{
	dataMapItem_t	*mapVal;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		rawDataMapItr_t itr = rawDataMap_t::find( tag);

		if( itr != rawDataMap_t::end())
		{
			mapVal = itr->second;
			delete mapVal;
			rawDataMap_t::erase( itr);
		}

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::erase: %s\n", strerror( mtxErr));
	}

	return( size());
}

/**
 * Removes all items from the table
 */
void HwDataMap::clear()
{
	rawDataMapItr_t	itr;
	int		   		mtxErr, ii=size();

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		for( itr=rawDataMap_t::begin(), ii=0; itr!=rawDataMap_t::end(); itr++, ii++)
		{
			dataMapItem_t*	item = itr->second;
			delete item;
		}
		rawDataMap_t::clear();

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::clear: %s\n", strerror( mtxErr));
	}
}
/**
 * Check if table is empty.
 *
 * @return true if table is empty
 *         false if table contains any data items
 */
bool HwDataMap::empty() const
{
	bool isEmpty = false;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		isEmpty = rawDataMap_t::empty();

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::empty: %s\n", strerror( mtxErr));
	}

	return( isEmpty);
}


/**
 * Searches for a data item with the given tag
 *
 * @param tag    Tag of the data item to find
 * @return Pointer to data item if found
 *         NULL if not found
 */
dataMapItem_t const * HwDataMap::find(const dataTag_t& tag)
{
	dataMapItem_t const *retVal = (dataMapItem_t const *)NULL;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		rawDataMapItr_t itr = rawDataMap_t::find( tag);
		if( itr != rawDataMap_t::end())
		{
			retVal = (dataMapItem_t const *)itr->second;
		}

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::find: %s\n", strerror( mtxErr));
	}

	return( retVal);
}
/**
 * Searches for a data item with the given tag
 *
 * @param tag    Tag of the data item to find
 * @return Pointer to data item if found
 *         NULL if not found
 */
dataMapItem_t const * HwDataMap::find(const dataTag_t& tag) const
{
	dataMapItem_t const *retVal = (dataMapItem_t const *)NULL;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		rawDataMapCItr_t itr = rawDataMap_t::find( tag);
		if( itr != rawDataMap_t::end())
		{
			retVal = (dataMapItem_t const *)itr->second;
		}

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::find: %s\n", strerror( mtxErr));
	}

	return( retVal);
}

/**
 * Attempts to return the data item at the given index in the table
 *
 * @param index  0 based index of the data item to be returned
 * @return Pointer to data item if found
 *         NULL if not found
 */
dataMapItem_t const * HwDataMap::operator[](const size_t index)
{
	dataMapItem_t const *retVal=NULL;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		rawDataMapItr_t itr = rawDataMap_t::begin();
		if( index < size())
		{
			advance( itr, index);
			retVal = (dataMapItem_t const *)itr->second;
		}
		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::operator[]: %s\n", strerror( mtxErr));
	}

	return( retVal);
}

/**
 * Returns the number oif items in the table
 *
 * @return The number oif items in the table
 */
size_t HwDataMap::size() const
{
	size_t	retVal = 0;
	int	mtxErr;

	if( (mtxErr=pthread_mutex_lock( &m_tableLock)) == EOK)
	{
		retVal = rawDataMap_t::size();

		pthread_mutex_unlock( &m_tableLock);
	}
	else
	{
		printf( "Cannot lock mutex in HwDataMap::size: %s\n", strerror( mtxErr));
	}

	return( retVal);
}


//////////////////////////////////////////////////

dataMapItem_t::dataMapItem_t() : channelIdx( 0), bitIdx( (UINT16)-1), logicLvl( true),
    portDir( PORT_DIR_BI), boardIdx( 0)
{
}

dataMapItem_t::dataMapItem_t( UINT16 chnl, UINT16 bit, bool logic, portDir_t dir, UINT16 board) :
    channelIdx( chnl), bitIdx( bit), logicLvl( logic), portDir( dir),
    boardIdx( board)
{
}

dataMapItem_t::dataMapItem_t( const dataMapItem_t &copy)
{
    *this = copy;
}

dataMapItem_t::~dataMapItem_t()
{
}

dataMapItem_t& dataMapItem_t::operator=( const dataMapItem_t &rhs)
{
    channelIdx = rhs.channelIdx;
    bitIdx = rhs.bitIdx;
    logicLvl = rhs.logicLvl;
    portDir = rhs.portDir;
    boardIdx = rhs.boardIdx;

    return( *this);
}
////////////////////////////////////////////////
