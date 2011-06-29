//******************************************************************************
// Description:
//	Logical serial port filter string data type definitions
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/LogPortFilterString.h $
// 
// 4     4/13/06 3:37a Cward
// changed return type on equal operator
//
// 2     3/20/06 10:50a Bmeinke
// No longer return references from operator=()
//
// 3     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 11:53a Khykin
// Updated with mutex changes.
//
// 2     10/10/03 9:30a Khykin
// Updated with latest Flatrock changes.
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
// 1     6/12/03 4:17p Khykin
// Initial checkin.
//
// 1     4/22/03 9:47a Khykin
// Kevins Working Checkin
//
// 8     3/20/03 8:38p Bmeinke
// Added IsPositiveResponse() (and numerous supporting methods). Specified
// in the config file, used to chekc if a received response is tagged as
// positive or negative
//
// 7     2/06/03 10:31a Bmeinke
// Cleaned up comments (spelling errors)
//
// 6     1/21/03 9:01p Bmeinke
// Added m_endGapTime data member used for variable length replies, this
// is the amount of time (nanoseconds) to wait between consecutive
// response bytes before we assume we have received the full response
//
// 5     1/15/03 10:10a Bmeinke
// Moved implementation of variable length reply strings into the
// LogPortFilterString class
//
// 4     8/01/02 1:26p Bmeinke
// Cleaned up compiler warnings
//
// 3     6/19/02 5:10p Bmeinke
// Changed locking mechanism from BeoMutex to raw pthread_mutex_t due to
// problems with multiple instantiations of J2190Client objects in AirBag
// test component
// Added error printouts if unable to lock the list
//
// 2     4/15/02 1:48p Bmeinke
// Added operators for converting/comparing between filter strings/serial
// strings and filter bytes/unsigned chars
//
// 1     4/04/02 5:20p Bmeinke
// Logical port filter string implementation. used to filter messages sent
// and received through a logical serial port
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _LOG_PORT_FLTR_STRING_H_INCLUDED
#define _LOG_PORT_FLTR_STRING_H_INCLUDED

#include <inttypes.h>
#include <vector>
#include <list>
#include <pthread.h>
#include "LogPortMessage.h"

/**
 * Attributes for a response string length (Fixed or Variable length)
 */
typedef enum
{
	/**
	 * Response string has a fixed length
	 */
	FIXED_LEN	= 0,

	/**
	 * Response string has a variable length
	 */
	VAR_LEN = 1
} RespLenType_t;

/**
 * Object used to represent a single byte of a LogPortFilterString
 *
 * @author Brian Meinke
 * @version 1
 * @since March 24, 2002
 */
class LogPortFilterByte : public LogPortFilterByte_t
{
public:
	/**
	 * Default constructor
	 *
	 * @param fb         Filterbyte value
	 * @param mask       Maks to use when testing for equivalency betweek filter byte and
	 *                   a received character
	 * @param isWidlcard Flag use to indicate if the filter byte value is a literal value
	 *                   or a wildcard value
	 */
	LogPortFilterByte( uint8_t fb=0x00, uint8_t mask=0xFF, bool isWidlcard=false);

	/**
	 * Copy a simple filter byte sata struct
	 *
	 * @param fb
	 */
	LogPortFilterByte( const LogPortFilterByte_t &fb);

	/**
	 * Typecast operator to uint8_t type
	 *
	 * @return Unsigned 8-bit integer value for the filterbyte
	 */
	operator uint8_t() const;

	/**
	 * Typecast operator to uint8_t type
	 *
	 * @return Signed 8-bit integer value for the filterbyte
	 */
	operator char() const;

	/**
	 * Assignement operator
	 *
	 * @param rhs    LogPortFilterByte object to be copied
	 * @return *this
	 */
	LogPortFilterByte& operator=( const LogPortFilterByte &rhs);

	/**
	 * Assignement operator
	 *
	 * @param rhs    LogPortFilterByte object to be copied
	 * @return *this
	 */
	LogPortFilterByte& operator=( const LogPortFilterByte_t &rhs);

	/**
	 * Assignement operator for unsigned 8-bit integer
	 *
	 * @param rhs    Value to be copied
	 * @return Filter byte value
	 */
	uint8_t operator=( uint8_t rhs);

	/**
	 * Assignement operator for signed 8-bit integer
	 *
	 * @param rhs    Value to be copied
	 * @return Filter byte value
	 */
	char operator=( char rhs);

	/**
	 * Equivalence operator
	 *
	 * @param rhs    LogPortFilterByte object to test equivalency against
	 * @return true if filter byte value equals rhs
	 */
	bool operator==( const LogPortFilterByte &rhs) const;

	/**
	 * Equivalence operator
	 *
	 * @param rhs    Unsigned 8-bit integer to test filter byte value against
	 * @return true if filter byte value equals rhs
	 */
	bool operator==( uint8_t rhs) const;

	/**
	 * Equivalence operator
	 *
	 * @param rhs    Signed 8-bit integer to test filter byte value against
	 * @return true if filter byte value equals rhs
	 */
	bool operator==( char rhs) const;

	/**
	 * Non-equivalence operator
	 *
	 * @param rhs    LogPortFilterByte object to test equivalency against
	 * @return false if filter byte value equals rhs
	 */
	bool operator!=( const LogPortFilterByte &rhs) const;

	/**
	 * Non-equivalence operator
	 *
	 * @param rhs    Unsigned 8-bit integer to test filter byte value against
	 * @return false if filter byte value equals rhs
	 */
	bool operator!=( uint8_t rhs) const;

	/**
	 * Non-equivalence operator
	 *
	 * @param rhs    Signed 8-bit integer to test filter byte value against
	 * @return false if filter byte value equals rhs
	 */
	bool operator!=( char rhs) const;

	/**
	 * Marks the filter byte as a wildcard and assigns a mask value
	 * to the filter byte object
	 *
	 * @param flag   Flag to use for marking the filter byte as a wildcard (flag=true
	 *               means filter byte is a wildcard)
	 * @param mask   Mask value to use if filter byte represents a wildcard character
	 */
	void MakeWildcard( bool flag=true, uint8_t mask=0x00);

};

/**
 * Raw data type use to represent a sequence of logical port filter bytes
 */
typedef vector< LogPortFilterByte>				LogPortFilterString_t;
typedef LogPortFilterString_t::iterator			LogPortFilterStringItr_t;
typedef LogPortFilterString_t::const_iterator	LogPortFilterStringCItr_t;

/**
 * Object used to hold response filter strings for LogPort messages
 *
 * @author Brian Meinke
 * @version 1
 * @since March 24, 2002
 */
class LogPortFilterString : public LogPortFilterString_t
{
public:
	/**
	 * Default consctructor
	 *
	 * @param isPositive true if the string represents a positive response, false if it
	 *                   represents a negative response
	 * @param lenType    Response length type: Fixed length or variable length filter
	 * @param timeout    For variable length replies, this is the amount (nanoseconds) of time to wait
	 *                   between consecutive response bytes before we assume we have received the full
	 *                   response
	 */
	LogPortFilterString( bool isPositive=true, RespLenType_t lenType=FIXED_LEN, const uint64_t timeout=0LL);

	/**
	 * Copy consctructor
	 *
	 * @param copy   Object to copy into *this
	 */
	LogPortFilterString( const LogPortFilterString &copy);

	/**
	 * Construct a filter string based on the template SerialString, tmpString,
	 * and set the filter string length tofltrSize (appending wildcard filter
	 * bytes as needed)
	 *
	 * @param tmpString  Template serial string to use as the leading bytes of the filter
	 * @param isPositive true if the string represents a positive response, false if it
	 *                   represents a negative response. Default=true
	 * @param fltrSize   (optional) Desired size of the filter resulting filter string. A
	 *                   value of negative means to use the length of the SerialString_t
	 *                   passed in
	 * @param lenType    Response length type: Fixed length or variable length filter. Default
	 *                   is FIXED_LEN
	 * @param timeout    For variable length replies, this is the amount (nanoseconds) of time to wait
	 *                   between consecutive response bytes before we assume we have received the full
	 *                   response. Default = 0
	 */
	LogPortFilterString( SerialString_t &tmpString,
                         bool isPositive=false,
                         int fltrSize=-1,
                         RespLenType_t lenType=FIXED_LEN,
						 const uint64_t timeout=0LL);

	/**
	 * Destructor
	 */
	~LogPortFilterString();

	/**
	 * Parses the comm-delimitted ASCII string (configString) into a series
	 * of LogPortFilterByte objects
	 *
	 * @param configString
	 *               Comma-delimitted ASCII string (read from config file) to be
	 *               parsed into a sequence of LogPortFilterByte objects
	 */
	void ReadFilterString( const char *configString);

	/**
	 * Check if a given response string matches the filter string
	 *
	 * @param rhs    Response string to compares against the filter string
	 * @return true if the response string matches (passes through) the filter string
	 */
	bool operator==(const SerialString_t &rhs) const;

	/**
	 * Method used to check if a given SerialString matches this filter string
	 *
	 * @param rhs    SerialString object to be compared
	 * @return true if the SerialString "matches" the filter string
	 */
	bool IsStringValid( const SerialString_t &rhs) const;

	/**
	 * Checks to see if this object represents a positive reply or a
	 * negative reply.
	 *
	 * @return true if the string represents a positive reply, false otherwise
	 */
	bool IsPositiveResponse() const;

	/**
	 * Returns the response message type (fixed length or variable length)
	 *
	 * @return The response message type (fixed length or variable length)
	 */
	RespLenType_t GetRespLenType() const;

	/**
	 * Returns the response message type (fixed length or variable length)
	 *
	 * @return The response message type (fixed length or variable length)
	 */
	const uint64_t& GetEndGapTime() const;

private:

	/**
	 * Reads LogPort filter byte value from an ASCII sting. Throws a
	 * BepException if the string does not contain valid LogPort filter
	 * byte data.
	 *
	 * @param str    String to read the LogPort filter byte information from
	 * @return A new LogPortFilterByte object constructed from the data conatined
	 *         in the string passed in
	 */
	LogPortFilterByte ReadLogPortFilterByte( const char *str);

	/**
	 * Is this a fixed length filter string or a variable length filter string
	 */
	RespLenType_t	m_lenType;

	/**
	 * 	For variable length replies, this is the amount (nanoseconds) of time to wait
	 * 	between consecutive response bytes before we assume we have received the full
	 * 	response
	 */
	uint64_t		m_endGapTime;

	bool			m_positiveResponse;

};

/**
 * Data type used to hold a list of filter byte strings
 */
typedef std::list< LogPortFilterString>				LogPortFilterStringList_t;
typedef LogPortFilterStringList_t::iterator			LogPortFilterStringListItr_t;
typedef LogPortFilterStringList_t::const_iterator	LogPortFilterStringListCItr_t;

/**
 * Object used to manage a list of LogPortFilterString objects
 *
 * @author Brian Meinke
 * @version 1
 * @since April 4, 2002
 */
class LogPortFilterStringList : public LogPortFilterStringList_t
{
public:

	/**
	 * Default constructor
	 */
	LogPortFilterStringList();

	/**
	 * Destructor
	 */
	~LogPortFilterStringList();

	/**
	 * Adds a new filter string to the list
	 *
	 * @param filterString
	 *               Filter string to be added
	 * @return Number of filter strings in the list
	 */
	int AddFilterString( const LogPortFilterString &filterString);

	/**
	 * Removes a filter string from the list
	 *
	 * @param filterString
	 *               Filter string to be removed from the list
	 * @return Number of filter strings in the list
	 */
	int RemoveFilterString( const LogPortFilterString &filterString);

	/**
	 * Removes a filter string from the list
	 *
	 * @param filterIdx Index into the filter list of the filter string to remove
	 * @return Number of filter strings in the list
	 */
	int RemoveFilterString( uint32_t filterIdx);

	/**
	 * Tests a SerialString_t object against the filters contained in
	 * the list.
	 *
	 * @param testString SerislString_t object to test against the filters in the list
	 * @return true if the testString matches ANY of the filters in the list,
	 *         false if the testString does not match ANY of the filters
	 */
	bool IsStringValid( const SerialString_t &testString) const;

	/**
	 * Checks to see if the given string represents a positive reply or a
	 * negative reply. If the string does not match any of our receive
	 * filters, it is considered to be a negative response.
	 *
	 * @param testString SerialString_t object to return the response type for
	 *
	 * @return true if the string represents a positive reply, false otherwise
	 */
	bool IsPositiveResponse( const SerialString_t &testString) const;

	/**
	 * Lock the list for exclusive access
	 *
	 * @return EOK if successfully locked, other value indicates error
	 */
	int Lock();

	/**
	 * Unlock the list so other threads can access it
	 *
	 * @return
	 */
	int Unlock();

private:
	/**
	 * Mutex used to lock thelist for exclusive access
	 */
	pthread_mutex_t	m_listLock;
};

#endif	// _LOG_PORT_FLTR_STRING_H_INCLUDED
