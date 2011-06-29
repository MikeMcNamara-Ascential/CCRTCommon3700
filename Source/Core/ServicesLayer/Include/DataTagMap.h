//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Include/DataTagMap.h $
// $Author: Cward $
//   $Date: 1/16/04 4:45p $
//
// Description:
//	 Data type definitions used to map a tag name to an actual system data tag
//
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
//     $Log: /Core/ServicesLayer/Include/DataTagMap.h $
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
// 1     6/12/03 4:08p Khykin
// Initial checkin.
//
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
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
// 1     2/27/02 8:54p Bmeinke
// Data types used to map a tag name to a system data tag string
//
//******************************************************************************

#ifndef _DATA_TAG_MAP_H_
#define _DATA_TAG_MAP_H_

#include <map>
#include <string>

#include "BepSync.h"		// BepMutex

/**
 * Data type used to represent a tag name
 */
typedef std::string							TagName_t;

/**
 * Data type used to represent a data tag
 */
typedef std::string							DataTag_t;

/**
 * Data type used to map one string to another string
 */
typedef std::map< TagName_t, DataTag_t>	DataTagMap_t;
typedef DataTagMap_t::iterator			DataTagMapItr_t;
typedef DataTagMap_t::value_type		DataTagMapVal_t;

/**
 * Object used to map a list of tag names to system data tags
 *
 * @author Brian Meinke
 * @version 1
 * @since December 28, 2001
 */
class DataTagMap : public DataTagMap_t
{
public:
	/**
	 * Default constructor (does nothing)
	 */
	DataTagMap();

	/**
	 * Default destructor (does even less)
	 */
	~DataTagMap();

	/**
	 * Set the name of this data tag object
	 *
	 * @param name The name we will use to refer to the data item
	 * @param tag The system name of the data item
	 */
	bool AddName( const TagName_t &name, const DataTag_t &tag);

	/**
	 * Set the name of this data tag object
	 *
	 * @param name The name we will use to refer to the data item
	 * @param tag The system name of the data item
	 */
	bool AddName( const char *name, const char *tag);

	/**
	 * Clears all tags in the tag list
	 */
	void Clear();

private:
	/**
	 * Mutex used to synchronize access to the tag name
	 */
	BepMutex		m_tagLock;

};

#endif // _DATA_TAG_MAP_H_
