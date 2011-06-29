//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Include/HwDataMap.h $
// $Author: Cward $
//   $Date: 1/16/04 4:45p $
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
//     $Log: /Core/ServicesLayer/Include/HwDataMap.h $
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:09p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:36  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:04a Khykin
// Initial Checkin
//
// 1     1/18/03 8:02a Khykin
// Initial Checkin From DCTC
//
// 7     8/01/02 11:22a Bmeinke
// Added return( 8this) to dataMapItem_t::operator=() method
//
// 6     10/16/01 1:16a Brian.meinke
// Added sestructor for dataMapItem_t class
//
// 5     5/18/01 10:34a Brian.meinke
// Removed the 'level' argument in the 2nd constructor (not used)
//
// 4     4/25/01 4:52p Brian.meinke
// Changed type of dataTag_t from std::string to XmlString
//
// 3     4/23/01 2:59p Brian.meinke
// Changed class name to HwDataMap
//
// 2     4/18/01 6:41p Brian.meinke
// Syntax, syntax, syntax...
//
// 1     4/18/01 5:56p Brian.meinke
// Data types to support lookup up of data items via tag.
//
//******************************************************************************

#ifndef _HW_MAP_H_INCLUDED_
#define _HW_MAP_H_INCLUDED_

/*******************
 Forward definitions
 ******************/
class HwDataMap;
class dataMapItem_t;

/*******************
 include files
 ******************/
#include <map>
#include <string>
#include <pthread.h>
#include "BepDefs.h"
#include "XmlDefs.h"

/**
 * Type to descripe data item direction (input, output or both)
 * @since 04/18/2001
 */
typedef enum _portDir_t
{
	PORT_DIR_IN		= 0x01,
	PORT_DIR_OUT	= 0x02,
	PORT_DIR_BI		= PORT_DIR_IN | PORT_DIR_OUT
} portDir_t;

/**
 * Type used to describe a named data item
 *
 * @author Brian Meinke
 * @version 1.0
 * @since 04/18/2001
 */
class dataMapItem_t
{
public:
	dataMapItem_t();
	
	dataMapItem_t( UINT16 chnl, UINT16 bit, bool logic, portDir_t dir, UINT16 board);

	
	dataMapItem_t( const dataMapItem_t &copy);

	~dataMapItem_t();

	dataMapItem_t& operator=( const dataMapItem_t &rhs);

	/**
	 * Item's channel Index on the board (0 based)
	 */
	UINT16		channelIdx;
	/**
	 * Bit index on the board (0 based, -1=entire channel)
	 */
	UINT16		bitIdx;
	/**
	 * Logic level (true=positive true, false=positive false)
	 */
	bool		logicLvl;
	/**
	 * Port direction (PORT_DIR_IN or PORT_DIR_OUT)
	 */
	portDir_t	portDir;
	/**
	 * Board on which the item is located (in case of multiple boards; 0 based)
	 */
	UINT16		boardIdx;
};

/**
 * Data item tag type
 *
 * @since 04/18/2001
 */
typedef XmlString						dataTag_t;

/**
 * Raw table type definition
 *
 * @see map
 * @since 04/18/2001
 */
typedef map <dataTag_t, dataMapItem_t*>	rawDataMap_t;
typedef rawDataMap_t::iterator			rawDataMapItr_t;
typedef rawDataMap_t::const_iterator	rawDataMapCItr_t;
typedef rawDataMap_t::value_type		rawDataMapVal_t;
typedef rawDataMap_t::reference			rawDataMapRef_t;

/**
 * Class to hold a table of named data items. Items are sorted by name.
 *
 * @author Brian Meinke
 * @version 1.0
 * @see rawDataMap_t
 * @see DataMapItem
 * @since 04/18/2001
 */
class HwDataMap : private rawDataMap_t
{
public:
	
	HwDataMap();
	~HwDataMap();

	/**
	 * Returns an iterator that points to the beginning of the data table
	 *
	 * @return An iterator that points to the beginning of the data table
	 */
	rawDataMapItr_t begin();
	/**
	 * Returns a constant iterator that points to the beginning of the data table
	 *
	 * @return A constant iterator that points to the beginning of the data table
	 */
	rawDataMapCItr_t begin() const;

	/**
	 * Returns an iterator that points just a past the last item in the table
	 *
	 * @return An iterator that points just a past the last item in the table
	 */
	rawDataMapItr_t end();
	/**
	 * Returns a constant iterator that points just a past the last item in the table
	 *
	 * @return A constant iterator that points just a past the last item in the table
	 */
	rawDataMapCItr_t end() const;

	/**
	 * Inserts a new item into the table with the given tag and properties
	 *
	 * @param tag    Tag associated with the data item
	 * @param item   Properties for the new data item
	 * @return Pointer to the new data item
	 */
	dataMapItem_t const* insert(const dataTag_t &tag, const dataMapItem_t &item);

	/**
	 * Removes the data item with the given tag from the table
	 *
	 * @param tag    Tag associated with the data item
	 * @return
	 */
	size_t erase(const dataTag_t& tag);

	/**
	 * Removes all items from the table
	 */
    void clear();

	/**
	 * Check if table is empty.
	 *
	 * @return true if table is empty
	 *         false if table contains any data items
	 */
    bool empty() const;

	/**
	 * Searches for a data item with the given tag
	 *
	 * @param tag    Tag of the data item to find
	 * @return Pointer to data item if found
	 *         NULL if not found
	 */
	dataMapItem_t const * find(const dataTag_t& tag);
	
	/**
	 * Searches for a data item with the given tag
	 *
	 * @param tag    Tag of the data item to find
	 * @return Pointer to data item if found
	 *         NULL if not found
	 */
	dataMapItem_t const * find(const dataTag_t& tag) const;

	/**
	 * Attempts to return the data item at the given index in the table
	 *
	 * @param index  0 based index of the data item to be returned
	 * @return Pointer to data item if found
	 *         NULL if not found
	 */
	dataMapItem_t const * operator[](const size_t index);

	/**
	 * Returns the number oif items in the table
	 *
	 * @return The number oif items in the table
	 */
	size_t size() const;

	/**
	 * XML Node Tag for the Channel Index Attribute
	 *
	 * @see XmlString
	 */
	const static XmlString	channelIdxTag;
	/**
	 * XML Node Tag for the Bit Index Attribute
	 *
	 * @see XmlString
	 */
	const static XmlString	bitIdxTag;
	/**
	 * XML Node Tag for the Logic Level Attribute
	 *
	 * @see XmlString
	 */
	const static XmlString	logicLvlTag;
	/**
	 * XML Node Tag for the Port Direction Attribute
	 *
	 * @see XmlString
	 */
	const static XmlString	portDirTag;
	/**
	 * XML Node Tag for the Board Index Attribute
	 *
	 * @see XmlString
	 */
	const static XmlString	boardIdxTag;

private:
	mutable pthread_mutex_t	m_tableLock;
};


#endif	// _HW_MAP_H_INCLUDED_

