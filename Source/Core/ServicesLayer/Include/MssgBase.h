//******************************************************************************
// Description:
//    Base message class. Serves as the base for all Ethernet/IP nad CIP message
//		objects
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
//     $Log: /Core/ServicesLayer/Include/MssgBase.h $
// 
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
//
// 2     6/17/04 9:53p Bmeinke
// Changed some int's to uint32_t to remove compiler warnings
//
// 1     6/11/04 4:15p Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:11a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 2     1/05/04 10:28p Bmeinke
// Needed to include stdio.h for PrintMssg()
//
// 1     12/23/03 8:15p Bmeinke
// Initial checkin
//
// 1     12/23/03 8:13p Bmeinke
// Initial checkin
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _MSSGBASE_H_INCLUDED_
#define _MSSGBASE_H_INCLUDED_
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "ByteArray.h"

/**
 * Base class for all Ethernet/IP and CIP message objects
 *
 * @author Brian Meinke
 * @version 1
 * @since December 12, 2003
 */
class MssgBase
{
public:
	/**
	 * Default constructor. Creates an empty message object
	 */
	MssgBase()
	{
	}

	virtual ~MssgBase()
	{
	}

	/**
	 * Print out the contents of the message object
	 */
	virtual void Print() const = 0;

	/**
	 * Copies the entire message into the given raw byte array
	 *
	 * @param rawBuff Byte array to copy the message into
	 *
	 * @return Number of bytes copied into the byte array
	 */
	virtual int GetRawBytes( ByteArray_t &rawBuff) const = 0;

	/**
	 * Creates a new message object from the data contained in the given
	 * raw byte array
	 *
	 * @param rawBuff Byte array continaing the raw byte format of the omessage object
	 * @param buffSz  Number of bytes in the raw buffer
	 *
	 * @return Number of bytes used from the raw buffer to create the object
	 */
	virtual int CreateFromBuff( const uint8_t *rawBuff, size_t buffSz) = 0;

	/**
	 * Creates a new message object from the data contained in the given
	 * raw byte array
	 *
	 * @param rawBuff Byte array continaing the raw byte format of the omessage object
	 *
	 * @return Number of bytes used from the raw buffer to create the object
	 */
	virtual int CreateFromBuff( const ByteArray_t &rawBuff)
	{
		return( CreateFromBuff( rawBuff.c_str(), rawBuff.size()));
	}

protected:
	template< class T>int AddRawItem( const T& item, ByteArray_t &rawBuff) const
	{
		uint32_t		 ii;
		const uint8_t	*junk = (const uint8_t*)&item;

		for( ii=0; ii<sizeof( item); ii++)
		{
			rawBuff.push_back( junk[ ii]);
		}

		return( sizeof( item));
	}

	template< class T>int GetRawItem( T& item, const uint8_t *rawBuff)
	{
		const T		*junk = (const T*)rawBuff;

		item = *junk;

		return( sizeof( item));
	}


};

#endif // _MSSGBASE_H_INCLUDED_


