//******************************************************************************
// Description:
//    Ethernet/IP encapsulation header data definitions
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/EIPHeader.h $
// 
// 2     10/30/07 3:35p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
//
// 2     6/17/04 9:27p Bmeinke
// Set the context value to 0xAC180DB20000007ELL in the default
// EIPHeader() constrcutor to match the example from Rockwell (for easier
// packet comparisons)
// Changed bytesREad and totalRead to uint32_t inside EIPHeader::Read() to
// remove compiler warnings
//
// 1     6/11/04 4:15p Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:11a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
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

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>              // socket stuff
#include <sys/types.h>          // socket stuff
#include <sys/socket.h>         // socket stuff
#include <netinet/tcp.h>        // socket stuff
#include <netinet/in.h>         // socket stuff
#include <string>

#include "MssgBase.h"
#include "ByteArray.h"

#ifndef _EIPHDR_H_INCLUDED_
#define _EIPHDR_H_INCLUDED_

#include <_pack1.h>

/**
 * Header sent along with each Industrial Protocol encapsulation message
 */
typedef struct
{
    uint16_t    command;
    uint16_t    dataLength;
    uint32_t    sessHdl;
    uint32_t    status;
    uint64_t    context;
    uint32_t    options;
} EIPHdr_t;

#include <_packpop.h>

/**
 * Object used to manage an Ethernet IP message header
 *
 * @author Brian Meinke
 * @version 1
 * @since December 12, 2003
 */
class EIPHeader : public MssgBase
{
public:
    /**
     * Default constructor. Creates an empty header object
     */
    EIPHeader() : MssgBase()
    {
        memset( &m_header, 0, sizeof( m_header));
        m_header.context = 0xAC180DB20000007ELL;
    }

    /**
     * Constructs a EIP Header object from an array of raw bytes
     *
     * @param rawBuff Buffer containing a EIP header in raw byte form
     */
    EIPHeader( const ByteArray_t &rawBuff) : MssgBase()
    {
        CreateFromBuff(rawBuff.c_str(), rawBuff.size());
    }

    ~EIPHeader()
    {
    }

    /**
     * Sends this EthernetIP header out the given file descriptor
     *
     * @param fd     File descriptor out which the header should be sent
     *
     * @return Number of bytes sent
     */
    int Send( int fd)
    {
        return( write( fd, &m_header, sizeof( m_header)));
    }

    /**
     * Reads an EthernetIP header from the given file descriptor
     *
     * @param fd     File descriptor from which the header should be read
     *
     * @return Number of bytes read
     */
    int Read( int fd)
    {
        uint8_t rawBuff[ sizeof( m_header)];// = (uint8_t*)&m_header;
        uint32_t bytesRead, totalRead=0;

        // Loop until full message is received
        while( totalRead < sizeof( m_header))
        {
            // Read next chunk of data
            bytesRead = read( fd, &rawBuff[ totalRead], sizeof( m_header)-totalRead);
            if( bytesRead > 0)  totalRead += bytesRead;
            else                break;
        }

        if( totalRead == sizeof( m_header))
        {
            EIPHeader::CreateFromBuff( rawBuff, totalRead);
        }

        return( totalRead);
    }

    /**
     * Creates a new message object from the data contained in the given
     * raw byte array
     *
     * @param rawBuff Byte array continaing the raw byte format of the omessage object
     * @param buffSz  Number of bytes in the raw buffer
     *
     * @return Number of bytes used from the raw buffer to create the object
     */
    virtual int CreateFromBuff( const uint8_t *rawBuff, size_t buffSz)
    {
        memset( &m_header, 0, sizeof( m_header));
        memmove( &m_header, rawBuff, sizeof( m_header));

        return( sizeof( m_header));
    }

    const EIPHdr_t& GetHeader() const
    {
        return( m_header);
    }

    /**
     * Copies the entire header structure into the given raw byte array
     *
     * @param rawBuff Byte array to copy the header data into
     *
     * @return Number of bytes copied into the byte array
     */
    virtual int GetRawBytes( ByteArray_t &rawBuff) const
    {
        ByteArray_t hdrBuff;
        const uint8_t *rawHdr = (const uint8_t *)&m_header;
        int     origSize = rawBuff.size();

        hdrBuff.assign( rawHdr, sizeof( m_header));
        rawBuff += hdrBuff;

        return( rawBuff.length()-origSize);
    }

    /**
     * Returns a reference to the underlying EIP header structure
     *
     * @return A reference to the underlying EIP header structure
     */
    EIPHdr_t& GetHeader()
    {
        return( m_header);
    }

    /**
     * Returns the EIP command ID from the header
     *
     * @return The EIP command ID from the header
     */
    const uint16_t GetCommand() const
    {
        return( m_header.command);
    }

    /**
     * Returns the EIP data length from the header
     *
     * @return The EIP data length from the header
     */
    const uint16_t GetDataLength() const
    {
        return( m_header.dataLength);
    }

    /**
     * Returns the EIP session handle from the header
     *
     * @return The EIP session handle from the header
     */
    const uint32_t GetSessionHandle() const
    {
        return( m_header.sessHdl);
    }

    /**
     * Returns the EIP status from the header
     *
     * @return The EIP status from the header
     */
    const uint32_t GetStatus() const
    {
        return( m_header.status);
    }

    /**
     * Returns the EIP context from the header
     *
     * @return The EIP context from the header
     */
    const uint64_t& GetContext() const
    {
        return( m_header.context);
    }

    /**
     * Returns the EIP options flag from the header
     *
     * @return The EIP options flag from the header
     */
    const uint32_t GetOptions() const
    {
        return( m_header.options);
    }

    /**
     * Sets the EIP command ID
     *
     * @param cmdId  EIP command ID to set
     */
    void SetCommand( uint16_t cmdId)
    {
        m_header.command = cmdId;
    }

    /**
     * Sets the EIP data length
     *
     * @param dataLen EIP data length to set.
     */
    void SetDataLength( uint16_t dataLen)
    {
        m_header.dataLength = dataLen;
    }

    /**
     * Sets the EIP session handle
     *
     * @param sessHdl EIP session handle to set.
     */
    void SetSessionHandle( uint32_t sessHdl)
    {
        m_header.sessHdl = sessHdl;
    }

    /**
     * Sets the EIP context
     *
     * @param context EIP context to set.
     */
    void SetContext( const uint64_t &context)
    {
        m_header.context = context;
    }

    /**
     * Sets the EIP options flag
     *
     * @param options EIP options flag to set.
     */
    void SetOptions( uint32_t options)
    {
        m_header.options = options;
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        printf( "    Command: $%04X\n", m_header.command);
        printf( "Data Length: $%04X\n", m_header.dataLength);
        printf( "    Session: $%08X\n", m_header.sessHdl);
        printf( "     Status: $%08X ", m_header.status);
        switch( m_header.status)
        {
        case 0x00:  printf( "Success\n");                                   break;
        case 0x01:  printf( "Invalid or unsupported command\n");            break;
        case 0x02:  printf( "Insufficient memory to handle the request\n"); break;
        case 0x03:  printf( "Poorly formed or incorrect data\n");           break;
        case 0x64:  printf( "Invlid session handle\n");                     break;
        case 0x65:  printf( "Invalid message length\n");                    break;
        case 0x69:  printf( "Unsupported protocol\n");                      break;
        default:    printf( "Unknown error\n");                             break;

        }

        printf( "    Context: $%llX\n", m_header.context);
        printf( "    Options: $%08X\n", m_header.options);
    }

private:
    EIPHdr_t    m_header;

};

#endif //_EIPHDR_H_INCLUDED_
