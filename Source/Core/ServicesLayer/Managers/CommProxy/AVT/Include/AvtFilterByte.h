//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Managers/CommProxy/AVT/Include/AvtFilterByte.h $
// $Author:: Cward                                                            $
//   $Date:: 12/21/03 6:28p                                                   $
//
// Description:
//	AVT box filter byte management objects
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
//     $Log: /Core/ServicesLayer/Managers/CommProxy/AVT/Include/AvtFilterByte.h $
// 
// 1     12/21/03 6:28p Cward
// 
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:26p Khykin
// Initial checkin
// 
// 1     6/12/03 4:05p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
// 
// 6     1/18/03 9:10a Bmeinke
// Added #include <algorithm> so compiler could find the 'find()' method
// from the STL
// 
// 5     11/15/02 2:59p Bmeinke
// Added #include <atomic.h> for atomi_*() functions
// 
// 4     9/06/02 6:20p Bmeinke
// Changed return value of GetKey() to uint16_t
// Fixed calculation of filter byte key values in CalculateFbKey()
// Fixed reference count bug in DelFilterByte()
// 
// 3     8/01/02 3:21p Bmeinke
// Removed compiler warnings
// 
// 2     6/27/02 7:09p Bmeinke
// Changed AvtFilterByteList from using a BepMutex to a raw
// pthread_mutex_t (BepMutex was not working for some reason)
// 
// 1     6/24/02 9:19a Bmeinke
// AVT box filter byte management objects
// 
//******************************************************************************

#ifndef _AVT_FILTER_BYTE_H_INCLUDED
#define _AVT_FILTER_BYTE_H_INCLUDED

#include <pthread.h>
#include <atomic.h>
#include <vector>
#include <algorithm>

/**
 * Object used to hold a filter byte used with the AVT box. A 
 * filter byte consists of a value and a position. When the AVT
 * box receives data from the J1850 bus, only messages which have
 * a byte whose value matches the filter byte's value in the position
 * associated with the filter byte object will be forwarded by
 * the AVT box
 */
class AvtFilterByte
{
public:
	/**
	 * Default constructor
	 */
	AvtFilterByte() : m_refCount( 0)
	{
		AddRef();
	}

	/**
	 * Create a filter byte with the given value and position
	 * 
	 * @param fbVal  Filter byte value
	 * @param fbPos  Filter byte position
	 */
	AvtFilterByte( uint8_t fbVal, uint8_t fbPos) : m_refCount( 0)
	{
		AddRef();
		Create( fbVal, fbPos);
	}

	/**
	 * Create a filter byte with the given value and position
	 * 
	 * @param fbVal  Filter byte value
	 * @param fbPos  Filter byte position
	 */
	void Create( uint8_t fbVal, uint8_t fbPos)
	{
		m_filterByteKey = CalculateFbKey(fbVal,fbPos);
	}

	/**
	 * Return the filter byte key
	 * 
	 * @return The filter byte key
	 */
	uint16_t GetKey() const
	{
		return( m_filterByteKey);
	}

	/**
	 * Return the filter byte value
	 * 
	 * @return The filter byte value
	 */
	uint8_t GetFbVal() const
	{
		return( (m_filterByteKey >> 8) & 0xFF);
	}

	/**
	 * Return the filter byte position
	 * 
	 * @return The filter byte position
	 */
	uint8_t GetFbPos() const
	{
		return( m_filterByteKey & 0xFF);
	}

	/**
	 * Get both the filter byte value and position
	 * 
	 * @param fbVal  Returned filter byte value
	 * @param fbPos  Returned filter byte position
	 */
	void GetFbValAndPos( uint8_t &fbVal, uint8_t &fbPos) const
	{
		DecomposeFbKey(fbVal,fbPos);
	}

	/**
	 * Increment the reference count to the filter byte
	 * 
	 * @return The previous value of the reference count (before it was incremented)
	 */
	unsigned AddRef()
	{
		return( atomic_add_value( &m_refCount, 1));
	}

	/**
	 * Decrement the reference count to the filter byte
	 * 
	 * @return The previous value of the reference count (before it was decremented)
	 */
	unsigned DelRef()
	{
		return( atomic_sub_value(&m_refCount, 1));
	}

	/**
	 * Return the reference count on this filter byte
	 * 
	 * @return The reference count on this filter byte
	 */
	unsigned GetRef() const
	{
		return( m_refCount);
	}

	bool operator<( const AvtFilterByte &rhs) const
	{
		return( m_filterByteKey < rhs.m_filterByteKey);
	}

	bool operator>( const AvtFilterByte &rhs) const
	{
		return( m_filterByteKey > rhs.m_filterByteKey);
	}

	bool operator==( const AvtFilterByte &rhs) const
	{
		return( m_filterByteKey == rhs.m_filterByteKey);
	}

	bool operator==( const uint16_t &key) const
	{
		return( m_filterByteKey == key);
	}

	friend bool operator==( const uint16_t &key, const AvtFilterByte &fb)
	{
		return( fb == key);
	}

	AvtFilterByte& operator=(const AvtFilterByte &rhs)
	{
		if( &rhs != this)
		{
			m_filterByteKey = rhs.m_filterByteKey;
		}

		return( *this);
	}
private:

	/**
	 * Calculates the filter byte key using the given filter byte 
	 * value and position.
	 * 
	 * @param fbVal  The filter byte value
	 * @param fbPos  The filter byte position
	 * @return The filter byte key composed of the given filter byte value 
	 *         and position.
	 */
	uint16_t CalculateFbKey( uint8_t fbVal, uint8_t fbPos) const
	{
		uint16_t	fbKey = fbVal;
		fbKey <<= 8;
		fbKey |= (uint16_t)fbPos;
		return( fbKey);
	}

	/**
	 * Break a filter byte key into its corresponding value and 
	 * position values
	 * 
	 * @param fbVal  Returned filter byte value
	 * @param fbPos  Returned filter byte position
	 */
	void DecomposeFbKey( uint8_t &fbVal, uint8_t &fbPos) const
	{
		fbPos = m_filterByteKey & 0xFF;
		fbVal = (m_filterByteKey >> 8) & 0xFF;
	}

	/**
	 * Filter byte key composed of both the filter byte value and the 
	 * filter position (within a vehicle response message)
	 */
	uint16_t	m_filterByteKey;

	/**
	 * Number of references to this filter byte
	 */
	unsigned	m_refCount;
};

/**
 * Object used to maintain a local list of filter bytes used for
 * filtering messages received from the vehicle
 */
typedef std::vector< AvtFilterByte>			AvtFltrByteList_t;
typedef AvtFltrByteList_t::iterator			AvtFltrByteListItr_t;
typedef AvtFltrByteList_t::const_iterator	AvtFltrByteListCItr_t;
typedef AvtFltrByteList_t::value_type		AvtFltrByteListVal_t;

/**
 * Object used to maintain a list of AVT filter bytes
 * 
 * @author Brian Meinke
 * @version 1
 * @since June 13, 2002 (game 7 of the Stanley Cup Finals)
 */
class AvtFltrByteList : public AvtFltrByteList_t
{
public:
	/**
	 * Default constructor
	 */
	AvtFltrByteList() : AvtFltrByteList_t()
	{
        pthread_mutexattr_t attr;
		
		memset( &attr, 0, sizeof( attr));
		memset( &m_listLock, 0, sizeof( m_listLock));
        
		pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_DISABLE);
		pthread_mutex_init( &m_listLock, &attr);
	}
	
	/**
	 * Destructor
	 */
	~AvtFltrByteList()
	{
		pthread_mutex_destroy( &m_listLock);
	}

	/**
	 * Add a filter byte to the list. If the filter byte already exists,
	 * the reference count of the existing filter byte will be incremented
	 * 
	 * @param fbVal  Filter byte value to be added
	 * @param fbPos  Filter position value to be added
	 * @return The number of outstanding references to the filter byte
	 */
	unsigned AddFilterByte( uint8_t fbVal, uint8_t fbPos)
	{
		AvtFilterByte			newFb( fbVal, fbPos);
		unsigned	   			retVal = 0;
		AvtFltrByteListItr_t	fbItr;

		if( pthread_mutex_lock( &m_listLock) == EOK)
		{
			// See if this filter byte alread yexists in the list
			fbItr = std::find( begin(), end(), newFb.GetKey());

			// If it does not yet exist
			if( fbItr == end())
			{
				// Add it
				push_back( newFb);
				retVal = newFb.GetRef();
			}
			else
			{
				// Already exists, so increment the reference count
				AvtFilterByte &oldFb = *fbItr;
				retVal = oldFb.AddRef();
			}

			pthread_mutex_unlock( &m_listLock);
		}

		return( retVal);
	}
	
	/**
	 * Removes a filter byte from the list. The filter byte will not be
	 * totally removed from the list until its reference count reaches 0.
	 * Each call to DelFilterByte() decrements the reference count on the
	 * given filter byte.
	 * 
	 * @param fbVal  Filter byte value to be removed
	 * @param fbPos  Filter position value to be removed
	 * @return The number of outstanding references to the filter byte
	 */
	unsigned DelFilterByte( uint8_t fbVal, uint8_t fbPos)
	{
		AvtFilterByte	newFb( fbVal, fbPos);
		uint16_t	   	fbKey = newFb.GetKey();
		unsigned	   	retVal = (unsigned)-1;

		if( pthread_mutex_lock( &m_listLock) == EOK)
		{
			// See if this filter byte alread yexists in the list
			AvtFltrByteListItr_t	fbItr = find( begin(), end(), fbKey);

			// If it exists
			if( fbItr != end())
			{
				// Decrement the reference count
				AvtFilterByte &oldFb = *fbItr;
				retVal = oldFb.DelRef();
				// If no more references to this filter byte
				if( retVal == 1)
				{
					// Remove it from the list
					AvtFltrByteList_t::erase( fbItr);
					retVal = 0;
				}
			}

			pthread_mutex_unlock( &m_listLock);
		}

		return( retVal);
	}

private:
	/**
	 * Mutex to guard against simultaneous access
	 */
	pthread_mutex_t		m_listLock;
};

#endif //_AVT_FILTER_BYTE_H_INCLUDED

