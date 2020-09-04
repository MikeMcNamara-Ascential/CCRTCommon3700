//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Library/LogPortFilterString.cpp $
// $Author: Cward $
//   $Date: 4/13/06 3:44a $
//
// Description:
//	Logical serial port filter string data type definitions
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/LogPortFilterString.cpp $
// 
// 2     4/13/06 3:44a Cward
// return form equal operator by value
//
// 2     3/20/06 10:51a Bmeinke
// No longer return references from operator=()
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif

#include "LogPortFilterString.h"
#include <algorithm>

/**
 * Default constructor
 *
 * @param fb         Filterbyte value
 * @param mask       Maks to use when testing for equivalency betweek filter byte and
 *                   a received character
 * @param isWidlcard Flag use to indicate if the filter byte value is a literal value
 *                   or a wildcard value
 */
LogPortFilterByte::LogPortFilterByte( uint8_t fb/*=0x00*/, uint8_t mask/*=0xFF*/, bool isWidlcard/*=false*/)
{
    m_fbVal = fb;
    m_mask = mask;
    m_isWildCard = isWidlcard;
};

/**
 * Copy a simple filter byte sata struct
 *
 * @param fb
 */
LogPortFilterByte::LogPortFilterByte( const LogPortFilterByte_t &fb)
{
    m_fbVal = fb.m_fbVal;
    m_isWildCard = fb.m_isWildCard;
    m_mask = fb.m_mask;
}

/**
 * Typecast operator to uint8_t type
 *
 * @return Unsigned 8-bit integer value for the filterbyte
 */
LogPortFilterByte::operator uint8_t() const
{
    return( m_fbVal);
}

/**
 * Typecast operator to uint8_t type
 *
 * @return Signed 8-bit integer value for the filterbyte
 */
LogPortFilterByte::operator char() const
{
    return( m_fbVal);
}

/**
 * Assignement operator
 *
 * @param rhs    LogPortFilterByte object to be copied
 * @return *this
 */
LogPortFilterByte& LogPortFilterByte::operator=( const LogPortFilterByte &rhs)
{
    m_fbVal = rhs.m_fbVal;
    m_isWildCard = rhs.m_isWildCard;
    m_mask = rhs.m_mask;

    return( *this);
}

/**
 * Assignement operator
 *
 * @param rhs    LogPortFilterByte object to be copied
 * @return *this
 */
LogPortFilterByte& LogPortFilterByte::operator=( const LogPortFilterByte_t &rhs)
{
    m_fbVal = rhs.m_fbVal;
    m_isWildCard = rhs.m_isWildCard;
    m_mask = rhs.m_mask;

    return( *this);
}

/**
 * Assignement operator for unsigned 8-bit integer
 *
 * @param rhs    Value to be copied
 * @return Filter byte value
 */
uint8_t LogPortFilterByte::operator=( uint8_t rhs)
{
    m_fbVal = (uint8_t)rhs;
    return( m_fbVal);
}

/**
 * Assignement operator for signed 8-bit integer
 *
 * @param rhs    Value to be copied
 * @return Filter byte value
 */
char LogPortFilterByte::operator=( char rhs)
{
    m_fbVal = (uint8_t)rhs;
    return( rhs);
}

/**
 * Equivalence operator
 *
 * @param rhs    LogPortFilterByte object to test equivalency against
 * @return true if filter byte value equals rhs
 */
bool LogPortFilterByte::operator==( const LogPortFilterByte &rhs) const
{
    return( (m_fbVal == rhs.m_fbVal) && (m_isWildCard == rhs.m_isWildCard) &&
            (m_mask == rhs.m_mask));
}

/**
 * Equivalence operator
 *
 * @param rhs    Unsigned 8-bit integer to test filter byte value against
 * @return true if filter byte value equals rhs
 */
bool LogPortFilterByte::operator==( uint8_t rhs) const
{
    if( m_isWildCard)
    {
        return( m_fbVal == (rhs & m_mask));
    }
    else
    {
        return( m_fbVal == rhs);
    }
}

/**
 * Equivalence operator
 *
 * @param rhs    Signed 8-bit integer to test filter byte value against
 * @return true if filter byte value equals rhs
 */
bool LogPortFilterByte::operator==( char rhs) const
{
    if( m_isWildCard)
    {
        return( m_fbVal == ((uint8_t)rhs & m_mask));
    }
    else
    {
        return( m_fbVal == (uint8_t)rhs);
    }
}

/**
 * Non-equivalence operator
 *
 * @param rhs    LogPortFilterByte object to test equivalency against
 * @return false if filter byte value equals rhs
 */
bool LogPortFilterByte::operator!=( const LogPortFilterByte &rhs) const
{
    return( !( *this == rhs));
}

/**
 * Non-equivalence operator
 *
 * @param rhs    Unsigned 8-bit integer to test filter byte value against
 * @return false if filter byte value equals rhs
 */
bool LogPortFilterByte::operator!=( uint8_t rhs) const
{
    return( !( *this == rhs));
}

/**
 * Non-equivalence operator
 *
 * @param rhs    Signed 8-bit integer to test filter byte value against
 * @return false if filter byte value equals rhs
 */
bool LogPortFilterByte::operator!=( char rhs) const
{
    return( !( *this == rhs));
}

/**
 * Marks the filter byte as a wildcard and assigns a mask value
 * to the filter byte object
 *
 * @param flag   Flag to use for marking the filter byte as a wildcard (flag=true
 *               means filter byte is a wildcard)
 * @param mask   Mask value to use if filter byte represents a wildcard character
 */
void LogPortFilterByte::MakeWildcard( bool flag/*=true*/, uint8_t mask/*=0x00*/)
{
    m_isWildCard = flag;
    m_mask = mask;
}



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
LogPortFilterString::LogPortFilterString( bool isPositive/*=true*/,
                                          RespLenType_t lenType/*=FIXED_LEN*/,
                                          const uint64_t timeout/*=0LL*/) :
    LogPortFilterString_t(),
    m_lenType( lenType),
    m_endGapTime( timeout),             //  was m_endGapTime( 0LL),
    m_positiveResponse( isPositive)
{
};

/**
 * Copy consctructor
 *
 * @param copy   Object to copy into *this
 */
LogPortFilterString::LogPortFilterString( const LogPortFilterString &copy) :
    LogPortFilterString_t( copy),
    m_lenType( copy.m_lenType),
    m_endGapTime( copy.m_endGapTime),
    m_positiveResponse( copy.m_positiveResponse)
{
};

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
LogPortFilterString::LogPortFilterString( SerialString_t &tmpString,
                                          bool isPositive/*=false*/,
                                          int fltrSize/*=-1*/,
                                          RespLenType_t lenType/*=FIXED_LEN*/,
                                          const uint64_t timeout/*=0LL*/) :
    LogPortFilterString_t(),
    m_lenType( lenType),
    m_endGapTime( timeout),
    m_positiveResponse( isPositive)
{
    LogPortFilterByte	fltrByte(0x00, 0x00, true);
    register uint32_t ii;

    // If no size specified, use the length of the given string
    if( fltrSize == -1)	fltrSize = tmpString.size();

    // Reserve enough romm for all the elements we will need
    LogPortFilterString_t::reserve( fltrSize);
    // Add the literal filter bytes (taken from the given SerialString)
    for( ii=0; ii<tmpString.size(); ii++)
    {
        push_back( tmpString[ ii]);
    }
    // Add any necessary trailing wildcards to the end of the filter
    while( ii++ < size())
    {
        push_back( fltrByte);
    }
}

/**
 * Destructor
 */
LogPortFilterString::~LogPortFilterString()
{
}

/**
 * Parses the comm-delimitted ASCII string (configString) into a series
 * of LogPortFilterByte objects
 *
 * @param configString
 *               Comma-delimitted ASCII string (read from config file) to be
 *               parsed into a sequence of LogPortFilterByte objects
 */
void LogPortFilterString::ReadFilterString( const char *configString)
{
    const char 	*ptr1, *ptr2;

    ptr1 = ptr2 = configString;
    while( (ptr1=strchr( ptr2, ',')) != NULL)
    {
        try
        {
            LogPortFilterByte fbVal = ReadLogPortFilterByte( ptr2);
            LogPortFilterString_t::push_back( fbVal);
        }
        catch( ...)
        {
            break;
        }
        ptr2 = ptr1+1;
    }
    if( (ptr2 != NULL) && (ptr2[0] != 0))
    {
        try
        {
            LogPortFilterByte fbVal = ReadLogPortFilterByte( ptr2);
            LogPortFilterString_t::push_back( fbVal);
        }
        catch( ...)
        {
        }
    }
}

/**
 * Check if a given response string matches the filter string
 *
 * @param rhs    Response string to compares against the filter string
 * @return true if the response string matches (passes through) the filter string
 */
bool LogPortFilterString::operator==(const SerialString_t &rhs) const
{
    bool	retVal = false;
    SerialString_t::const_iterator	sItr = sItr=rhs.begin();
    LogPortFilterStringCItr_t		lItr = lItr=begin();

    // Size must match first
    if( size() == rhs.size())
    {
        retVal = true;
        // Examine each element of each string
        while( (sItr!=rhs.end()) && (lItr!=end()) && (retVal == true))
        {
            // If these elements do not match
            if( *lItr != *sItr)
            {
                // Bye bye
                retVal = false;
            }
            // Look at next element
            sItr++;
            lItr++;
        }
    }

    return( retVal);
}

/**
 * Method used to check if a given SerialString matches this filter string
 *
 * @param rhs    SerialString object to be compared
 * @return true if the SerialString "matches" the filter string
 */
bool LogPortFilterString::IsStringValid( const SerialString_t &rhs) const
{
    bool	retVal = false;
    SerialString_t::const_iterator	sItr = sItr=rhs.begin();
    LogPortFilterStringCItr_t		lItr = lItr=begin();

    // Size must match first or filter must be set to variable length
    if( (size() == rhs.size()) || (m_lenType == VAR_LEN))
    {
        retVal = true;
        // Examine each element of each string
        while( (sItr!=rhs.end()) && (lItr!=end()) && (retVal == true))
        {
            // If these elements do not match
            if( *lItr != *sItr)
            {
                // Bye bye
                retVal = false;
            }
            // Look at next element
            sItr++;
            lItr++;
        }
    }

    return( retVal);
}

/**
 * Checks to see if this object represents a positive reply or a
 * negative reply.
 *
 * @return true if the string represents a positive reply, false otherwise
 */
bool LogPortFilterString::IsPositiveResponse() const
{
    return( m_positiveResponse);
}

/**
 * Returns the response message type (fixed length or variable length)
 *
 * @return The response message type (fixed length or variable length)
 */
RespLenType_t LogPortFilterString::GetRespLenType() const
{
    return( m_lenType);
}

void LogPortFilterString::SetRespLenType(RespLenType_t responseLengthType)
{
	m_lenType = responseLengthType;
}

/**
 * Returns the response message type (fixed length or variable length)
 *
 * @return The response message type (fixed length or variable length)
 */
const uint64_t& LogPortFilterString::GetEndGapTime() const
{
    return( m_endGapTime);
}


/**
 * Reads LogPort filter byte value from an ASCII sting. Throws a
 * BepException if the string does not contain valid LogPort filter
 * byte data.
 *
 * @param str    String to read the LogPort filter byte information from
 * @return A new LogPortFilterByte object constructed from the data conatined
 *         in the string passed in
 */
LogPortFilterByte LogPortFilterString::ReadLogPortFilterByte( const char *str)
{
    uint8_t	fbVal=0x00, fbMask=0xFF;
    bool	isWc=false;
    int		lenMax = strlen( str);

    // If the string length is long enough to contain a LogPortFilterByte
    if( lenMax > 3)
    {
        // Check for a full wildcard
        if( (str[ 2] == '?') && (str[ 3] == '?'))	// 0x??
        {
            fbVal = 0x00;
            fbMask = 0x00;
            isWc = true;
        }
        // Check for a partial wildcard
        else if( str[ 2] == '?')					// 0xY?
        {
            fbVal = atoh( &str[ 2]) * 0x10;
            fbMask = 0xF0;
            isWc = true;
        }
        // Check for a partial wildcard
        else if( str[ 3] == '?')					// 0x?Y
        {
            fbVal = atoh( &str[ 3]);
            fbMask = 0x0F;
            isWc = true;
        }
        // No wildcard characters..value is a literal character
        else
        {
            fbVal = atoh( str);
            fbMask = 0xFF;
            isWc = false;
        }
    }
    else
    {
        throw( BepException( "Invalid FilterByte string in ReadLogPortFilterByte"));
    }

    return( LogPortFilterByte( fbVal, fbMask, isWc));
}


/**
 * Default constructor
 */
LogPortFilterStringList::LogPortFilterStringList() : LogPortFilterStringList_t()
{
    // set up the mutex
    pthread_mutexattr_t attr;

    memset( &attr, 0, sizeof( attr));
    memset( &m_listLock, 0, sizeof( m_listLock));

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_ENABLE);
    pthread_mutex_init( &m_listLock, &attr);
};

/**
 * Destructor
 */
LogPortFilterStringList::~LogPortFilterStringList()
{
    pthread_mutex_destroy( &m_listLock);
}

/**
 * Adds a new filter string to the list
 *
 * @param filterString
 *               Filter string to be added
 * @return Number of filter strings in the list
 */
int LogPortFilterStringList::AddFilterString( const LogPortFilterString &filterString)
{
    int 		retVal=0;

    if( pthread_mutex_lock( &m_listLock) == EOK)
    {
        LogPortFilterStringList_t::push_back( filterString);
        retVal = LogPortFilterStringList_t::size();
        pthread_mutex_unlock( &m_listLock);
    }
    else
    {
        perror( "LogPortFilterStringList::AddFilterString Failed");
    }
    return( retVal);
}

/**
 * Removes a filter string from the list
 *
 * @param filterString
 *               Filter string to be removed from the list
 * @return Number of filter strings in the list
 */
int LogPortFilterStringList::RemoveFilterString( const LogPortFilterString &filterString)
{
    int 		retVal=0;

    if( pthread_mutex_lock( &m_listLock) == EOK)
    {
        LogPortFilterStringListItr_t	itr;
        // If string is in the list
        if( (itr=std::find( begin(), end(), filterString)) != end())
        {
            // Remove it from the list
            LogPortFilterStringList_t::erase( itr);
            retVal = LogPortFilterStringList_t::size();
        }
        pthread_mutex_unlock( &m_listLock);
    }
    else
    {
        perror( "LogPortFilterStringList::RemoveFilterString Failed");
    }

    return( retVal);
}

/**
 * Removes a filter string from the list
 *
 * @param filterIdx Index into the filter list of the filter string to remove
 * @return Number of filter strings in the list
 */
int LogPortFilterStringList::RemoveFilterString( uint32_t filterIdx)
{
    int 		retVal=0;

    if( pthread_mutex_lock( &m_listLock) == EOK)
    {
        // If the desired index is valid
        if( filterIdx < size())
        {
            LogPortFilterStringListItr_t	itr = begin();
            std::advance( itr, filterIdx);
            LogPortFilterStringList_t::erase( itr);
        }
        pthread_mutex_unlock( &m_listLock);
    }
    else
    {
        perror( "LogPortFilterStringList::RemoveFilterString Failed");
    }

    return( retVal);
}

/**
 * Tests a SerialString_t object against the filters contained in
 * the list.
 *
 * @param testString SerislString_t object to test against the filters in the list
 * @return true if the testString matches ANY of the filters in the list,
 *         false if the testString does not match ANY of the filters
 */
bool LogPortFilterStringList::IsStringValid( const SerialString_t &testString) const
{
    LogPortFilterStringListCItr_t	itr;
    bool							retVal = false;

    // Test against all filter strings or until we get a match
    for( itr=begin(); itr!=end() && retVal == false; itr++)
    {
        const LogPortFilterString &filter = *itr;
        retVal = filter.IsStringValid( testString);
    }

    return( retVal);
}

/**
 * Checks to see if the given string represents a positive reply or a
 * negative reply. If the string does not match any of our receive
 * filters, it is considered to be a negative response.
 *
 * @param testString SerialString_t object to return the response type for
 *
 * @return true if the string represents a positive reply, false otherwise
 */
bool LogPortFilterStringList::IsPositiveResponse( const SerialString_t &testString) const
{
    LogPortFilterStringListCItr_t	itr;
    bool							retVal = false;

    // Test against all filter strings or until we get a match
    for( itr=begin(); itr!=end(); itr++)
    {
        const LogPortFilterString &filter = *itr;
        if( filter.IsStringValid( testString) == true)
        {
            retVal = filter.IsPositiveResponse();
            break;
        }
    }

    return( retVal);
}

/**
 * Lock the list for exclusive access
 *
 * @return EOK if successfully locked, other value indicates error
 */
int LogPortFilterStringList::Lock()
{
    return( pthread_mutex_lock( &m_listLock));
}

/**
 * Unlock the list so other threads can access it
 *
 * @return
 */
int LogPortFilterStringList::Unlock()
{
    return( pthread_mutex_unlock( &m_listLock));
}

