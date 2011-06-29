//******************************************************************************
// Description:
//    Routines and data defs needed for EthernetIP tester
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/EIPMssg.h $
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
// 2     6/17/04 9:28p Bmeinke
// Changed bytesUsed to uint32_t insode
// EIPSendRRDataMssg::CreateFromBuff() to remove compiler warnings
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
#include "EIPHeader.h"
#include "CommonPacket.h"
#include "CIPMssg.h"

#ifndef _EIPMSSG_H_INCLUDED_
#define _EIPMSSG_H_INCLUDED_

/**
 * Base EthernetIP message object
 */
class EIPMssg : public MssgBase
{
public:
    /**
     * Default constructor. Creates a new Ethernet/IP message object with
     * the given command ID and session handle.
     *
     * @param cmdId   Command ID for the new EIP message object (default=0xFFFF)
     * @param sessHdl Session Handle to use for the EIP message object (default=0xFFFF)
     */
    EIPMssg( uint16_t cmdId=0xFF, uint16_t sessHdl=0)
    {
        memset( &m_header, 0, sizeof( m_header));
        m_header.SetCommand( cmdId);
        m_header.SetSessionHandle( sessHdl);
    }

    /**
     * Constructs a message object from an array of raw bytes
     *
     * @param rawBuff Buffer containing a message object in raw byte form
     */
    EIPMssg( const ByteArray_t &rawBuff) : MssgBase()
    {
        CreateFromBuff(rawBuff.c_str(), rawBuff.size());
    }

    virtual ~EIPMssg()
    {
    }

    /**
     * Returns the header portion of the EIP message
     *
     * @return The header portion of the EIP message
     */
    const EIPHeader& GetHeader() const
    {
        return( m_header);
    }

    /**
     * Returns the header portion of the EIP message
     *
     * @return The header portion of the EIP message
     */
    EIPHeader& GetHeader()
    {
        return( m_header);
    }

    /**
     * Sets the session handle for this message object
     *
     * @param sessionHandle
     *               The session handle for this message object
     */
    void SetSessionHandle( uint32_t sessionHandle)
    {
        m_header.SetSessionHandle( sessionHandle);
    }

    /**
     * Copies the entire message into the given raw byte array
     *
     * @param rawBuff Byte array to copy the message into
     *
     * @return Number of bytes copied into the byte array
     */
    virtual int GetRawBytes( ByteArray_t &rawBuff) const
    {
        int bytesUsed = 0;
        m_header.SetDataLength( CalculateDataSize());
        bytesUsed = m_header.GetRawBytes(rawBuff);
        // Get the raw Ethernet/IP header
        return( bytesUsed);
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
        return( m_header.CreateFromBuff(rawBuff, buffSz));
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        m_header.Print();
    }

    /**
     * Returns a reference to the CIP message object associated with this
     * Ethernet IP message
     *
     * @return A reference to the CIP message object associated with this
     *         Ethernet IP message or NULL if no CIP message is associated
     *         with this EIP message
     */
    virtual const CommonPacket& GetCommonPacket() const
    {
        return( m_commPacket);
    }

    void AddCommonPacketItem( const CommonPacketDataItem *newItem)
    {
        m_commPacket.AddItem(newItem);
        m_header.SetDataLength( CalculateDataSize());
    }

protected:
    virtual int CalculateDataSize() const
    {
        int len = m_commPacket.GetLength();

        return( len);
    }

    /**
     * Message header
     */
    mutable EIPHeader   m_header;

    CommonPacket        m_commPacket;
};

/**
 * Base EthernetIP message object used to request data
 */
class EIPRequest : public EIPMssg
{
public:
    /**
     * Creates a new EIPRequest message object
     *
     * @param cmdId   Command ID of the Ethernet IP request
     * @param sessHdl Session handle for the Ethernet IP communication traffic
     */
    EIPRequest( uint16_t cmdId, uint32_t sessHdl=0) : EIPMssg( cmdId, sessHdl)
    {
    }

    virtual ~EIPRequest()
    {
    }

    /**
     * Sends the EthernetIP request out the given file descriptor
     *
     * @param fd     File descriptor out which the request should be sent
     *
     * @return Number of bytes sent
     */
    virtual int Send( int fd)
    {
        ByteArray_t rawMssg;
        int         totalSent = 0;

        // See how big our CIP message block is
        int len = CalculateDataSize();
        m_header.SetDataLength( len);

        // Get the raw byte form of the message
        if( GetRawBytes( rawMssg) > 0)
        {
            // Send the raw message
            totalSent = write( fd, rawMssg.c_str(), rawMssg.size());
        }

        return( totalSent);
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        // See how big our CIP message block is
        m_header.SetDataLength(CalculateDataSize());

        EIPMssg::Print();
    }

};

/**
 * EthernetIP NOP message object
 */
class EIPNopMssg : public EIPRequest
{
public:
    EIPNopMssg(uint16_t sessHdl=0) : EIPRequest( 0x0000, sessHdl)
    {
    }

    virtual ~EIPNopMssg()
    {
    }
};

/**
 * EthernetIP ListServices message object
 */
class EIPListServicesMssg : public EIPRequest
{
public:
    EIPListServicesMssg() : EIPRequest( 0x0004)
    {
    }

    virtual ~EIPListServicesMssg()
    {
    }
};

/**
 * EthernetIP ListIdentity message object
 */
class EIPListIdentityMssg : public EIPRequest
{
public:
    EIPListIdentityMssg(uint16_t sessHdl=0) : EIPRequest( 0x0063, sessHdl)
    {
    }

    virtual ~EIPListIdentityMssg()
    {
    }
};

/**
 * EthernetIP ListInterfaces message object
 */
class EIPListInterfacesMssg : public EIPRequest
{
public:
    EIPListInterfacesMssg(uint16_t sessHdl=0) : EIPRequest( 0x0064, sessHdl)
    {
    }

    virtual ~EIPListInterfacesMssg()
    {
    }
};

/**
 * EthernetIP RegisterSession message object
 */
class EIPRegisterSessionMssg : public EIPRequest
{
public:
    EIPRegisterSessionMssg() : EIPRequest( 0x0065),
        m_protocolVersion( 1), m_options( 0)
    {
    }

    virtual ~EIPRegisterSessionMssg()
    {
    }

    /**
     * Copies the entire message into the given raw byte array
     *
     * @param rawBuff Byte array to copy the message into
     *
     * @return Number of bytes copied into the byte array
     */
    virtual int GetRawBytes(ByteArray_t &rawBuff) const
    {
        int bytesUsed = 0;

        bytesUsed += EIPMssg::GetRawBytes(rawBuff);
        bytesUsed += AddRawItem( m_protocolVersion, rawBuff);
        bytesUsed += AddRawItem( m_options, rawBuff);

        return( bytesUsed);
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
        int bytesUsed = 0;

        bytesUsed += EIPMssg::CreateFromBuff( rawBuff, buffSz);
        bytesUsed += GetRawItem( m_protocolVersion, &rawBuff[ bytesUsed]);
        bytesUsed += GetRawItem( m_options, &rawBuff[ bytesUsed]);

        return( bytesUsed);
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        EIPMssg::Print();
        printf( "Protocol Version: %d\n", m_protocolVersion);
        printf( "         Options: $%04X\n", m_options);
    }
protected:

    virtual int CalculateDataSize() const
    {
        return( EIPMssg::CalculateDataSize() + sizeof(m_protocolVersion) + sizeof(m_options));
    }


    uint16_t    m_protocolVersion;

    uint16_t    m_options;
};

/**
 * EthernetIP UnregisterSession message object
 */
class EIPUnRegisterSessionMssg : public EIPRequest
{
public:
    EIPUnRegisterSessionMssg(uint16_t sessHdl) : EIPRequest( 0x0066, sessHdl)
    {
    }

    virtual ~EIPUnRegisterSessionMssg()
    {
    }
};

/**
 * EthernetIP SendRRData message object
 */
class EIPSendRRDataMssg : public EIPRequest
{
public:
    EIPSendRRDataMssg(uint16_t sessHdl) : EIPRequest( 0x006F, sessHdl),
        m_ifcHandle( 0), m_timeoutVal( 0)
    {

    }

    virtual ~EIPSendRRDataMssg()
    {
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        EIPMssg::Print();
        printf( "Interface Handle: $%08X\n", m_ifcHandle);
        printf( "Timeout: %d\n", m_timeoutVal);
        if( m_commPacket.GetItemList().size() > 0)
        {
            m_commPacket.Print();
        }

    }
    /**
     * Copies the entire message into the given raw byte array
     *
     * @param rawBuff Byte array to copy the message into
     *
     * @return Number of bytes copied into the byte array
     */
    virtual int GetRawBytes(ByteArray_t &rawBuff) const
    {
        int bytesUsed = 0;

        bytesUsed += EIPMssg::GetRawBytes(rawBuff);
        bytesUsed += AddRawItem( m_ifcHandle, rawBuff);
        bytesUsed += AddRawItem( m_timeoutVal, rawBuff);
        bytesUsed += m_commPacket.GetRawBytes(rawBuff);

        return( bytesUsed);
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
        uint32_t    bytesUsed = 0;

        m_commPacket.Clear();

        bytesUsed += EIPMssg::CreateFromBuff( rawBuff, buffSz);
        bytesUsed += GetRawItem( m_ifcHandle, &rawBuff[ bytesUsed]);
        bytesUsed += GetRawItem( m_timeoutVal, &rawBuff[ bytesUsed]);
        if( buffSz > bytesUsed)
        {
            bytesUsed += m_commPacket.CreateFromBuff(&rawBuff[ bytesUsed], buffSz-bytesUsed+1);
        }

        return( bytesUsed);

    }
private:
    virtual int CalculateDataSize() const
    {
        int len = EIPMssg::CalculateDataSize() + sizeof(m_ifcHandle) + sizeof(m_timeoutVal);
        return( len);
    }
    uint32_t    m_ifcHandle;

    uint16_t    m_timeoutVal;
};


/**
 * EthernetIP SendRRData message object specialized for a ForwardOpen request
 *
 * @author Brian Meinke
 * @version 1
 * @since December 22, 2003
 */
class EIPForwardOpenRequest : public EIPSendRRDataMssg
{
public:
    EIPForwardOpenRequest( uint16_t sessHdl) : EIPSendRRDataMssg( sessHdl)
    {
        const uint8_t reqPath[] = { 0x20, 0x06, 0x24, 0x01 };

        // m_nullAddress and m_fwdOpenReq will be cleaned up by the CommonPacket
        //  object when its Clear() method is called
        m_nullAddress = new CommonPacketNullAddressItem;
        m_fwdOpenReq = new CommonPacketFwdOpenDataItem( reqPath, 4);

        AddCommonPacketItem( m_nullAddress);
        AddCommonPacketItem( m_fwdOpenReq);
    }

    EIPForwardOpenRequest( uint16_t sessHdl, uint32_t origRpi, uint32_t targRpi) : EIPSendRRDataMssg( sessHdl)
    {
        const uint8_t reqPath[] = { 0x20, 0x06, 0x24, 0x01 };

        m_nullAddress = new CommonPacketNullAddressItem;
        m_fwdOpenReq = new CommonPacketFwdOpenDataItem(reqPath, 4);

        // m_nullAddress and m_fwdOpenReq will be cleaned up by the CommonPacket
        //  object when its Clear() method is called
        m_fwdOpenReq->SetRequestedPacketInterval( origRpi, targRpi);

        AddCommonPacketItem( m_nullAddress);
        AddCommonPacketItem( m_fwdOpenReq);
    }

    virtual ~EIPForwardOpenRequest()
    {
    }

private:
    /**
     * NULL address item needed for the ForwardOpen request
     */
    CommonPacketNullAddressItem *m_nullAddress;

    /**
     * Encapsulated CIP forward open message
     */
    CommonPacketFwdOpenDataItem *m_fwdOpenReq;
};

/**
 * EthernetIP ListIdentity message object
 */
class EIPSendUnitDataMssg : public EIPRequest
{
public:
    EIPSendUnitDataMssg(uint16_t sessHdl) : EIPRequest( 0x0070, sessHdl)
    {
    }

    virtual ~EIPSendUnitDataMssg()
    {
    }
};

/**
 * EthernetIP ListIdentity message object
 */
class EIPIndicateStatusMssg : public EIPRequest
{
public:
    EIPIndicateStatusMssg(uint16_t sessHdl) : EIPRequest( 0x0072, sessHdl)
    {
    }

    virtual ~EIPIndicateStatusMssg()
    {
    }
};

/**
 * EthernetIP ListIdentity message object
 */
class EIPCancelMssg : public EIPRequest
{
public:
    EIPCancelMssg(uint16_t sessHdl) : EIPRequest( 0x0073, sessHdl)
    {
    }

    virtual ~EIPCancelMssg()
    {
    }
};

#endif //_EIPMSSG_H_INCLUDED_
