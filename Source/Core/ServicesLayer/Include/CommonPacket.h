//******************************************************************************
// Description:
//    Common Packet Header definitions
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/CommonPacket.h $
// 
// 3     10/30/07 3:33p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
//
// 1     6/11/04 4:15p Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:11a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 2     1/09/04 1:47p Bmeinke
// Made CommonPacketDataItem::CalcDataLength() pure virtual
// Added CommonPacketSequencedDataItem class (used for the 1st item of an
// I/O message packet)
// Added CommonPacketConnectedDataItem class (used for the 2nd item of an
// I/O message packet)
// Added more checking for NULL CIPMssg* objects in many classes with
// embedded CIPMssg's
// Added non-const versions of CommonPacket::GetItemList()
// Added CommonPacket::GetItemCount() method to return the number of items
// contained in a CommonPacket
// Added CommonPacket::CreateDataItem() method to allocate a new
// CommonPacketDataItem object from a raw byte buffer
// Made CommonPacket::Clear() virtual
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

#ifndef _COMMONPACKET_H_INCLUDED_
#define _COMMONPACKET_H_INCLUDED_

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <list>
#include <arpa/inet.h>

#include "ByteArray.h"
#include "MssgBase.h"
#include "CIPMssg.h"

typedef struct
{
    struct sockaddr_in addr;
    struct ip_mreq mreq;
} IpMulticastAddr_t;


/**
 * Data structure used to represent a data item entry in a common
 * packet header
 */
class CommonPacketDataItem : public MssgBase
{
public:

    CommonPacketDataItem() : MssgBase(),
        m_typeId( 0), m_dataLength(0)
    {
    }

    CommonPacketDataItem( uint16_t type) : MssgBase(),
        m_typeId( type), m_dataLength(0)
    {
    }

    CommonPacketDataItem( const CommonPacketDataItem &copy) : MssgBase(),
        m_typeId( copy.m_typeId), m_dataLength(copy.m_dataLength)
    {
    }

    virtual ~CommonPacketDataItem()
    {
    }

    /**
     * Returns what type of data item this is
     *
     * @return What type of data item this is
     */
    uint16_t GetTypeId() const
    {
        return( m_typeId);
    }

    /**
     * Returns the length of the entire message object
     *
     * @return The length of the entire message object
     */
    int GetLength() const
    {
        m_dataLength = CalcDataLength();

        // Size for the type identifier
        // Size for the data length specifier
        // Length of the data
        return( sizeof( m_typeId) + sizeof( m_dataLength) + m_dataLength);
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        m_dataLength = CalcDataLength();
        printf( "Type $%04X\n", m_typeId);
        printf( "Data Length: %d\n", CalcDataLength());
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
        int             bytesUsed = 0;

        m_dataLength = CalcDataLength();
        // Stick in the type ID
        bytesUsed += AddRawItem( m_typeId, rawBuff);
        // stick in the length
        bytesUsed += AddRawItem( m_dataLength, rawBuff);

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
        int         bytesUsed = 0;

        // Read the Type
        bytesUsed += GetRawItem( m_typeId, &rawBuff[ bytesUsed]);
        // Read how many bytes are in this data packet
        bytesUsed += GetRawItem( m_dataLength, &rawBuff[ bytesUsed]);

        return( bytesUsed);
    }

protected:
    /**
     * Method used to get the length of data contained in the CommonPacketDataItem
     *
     * @return The length of data contained in the CommonPacketDataItem
     */
    virtual uint16_t CalcDataLength() const = 0;

    /**
     * The data type contained in this common packet data item
     */
    uint16_t            m_typeId;

    /**
     * Length of the data in this data item
     */
    mutable uint16_t    m_dataLength;
};


/**
 * NULL Address item. This data type is used to way that all
 * routing information for the common packet is contained in the
 * data packet to follow
 *
 * @author Brian Meinke
 * @version 1
 * @since December 13, 2003
 */
class CommonPacketNullAddressItem : public CommonPacketDataItem
{
public:
    CommonPacketNullAddressItem() : CommonPacketDataItem( 0x00)
    {
    }
    ~CommonPacketNullAddressItem()
    {
    }
protected:
    /**
     * Method used to get the length of data contained in the CommonPacketDataItem
     *
     * @return The length of data contained in the CommonPacketDataItem
     */
    virtual uint16_t CalcDataLength() const
    {
        return( 0);
    }
};


/**
 * Common Packet data item used to manage a Connected Data item.
 *
 * @author Brian Meinke
 * @version 1
 * @since December 17, 2003
 */
class CommonPacketConnectedDataItem : public CommonPacketDataItem
{
public:
    /**
     * Create a new UnconnectedData item with an embedded CIPMssg object. The
     * CIPMssg object will get filled in by the CreateFromBuff() method and
     * MUST HAVE a scope equal to or longer than this UnconnectedData object
     */
    CommonPacketConnectedDataItem() : CommonPacketDataItem( 0x00B1)
    {
    }

    virtual ~CommonPacketConnectedDataItem()
    {
    }
};

/**
 * Common Packet data item used to manage an Unconnected Data item with
 * an embedded CIP packet
 *
 * @author Brian Meinke
 * @version 1
 * @since December 17, 2003
 */
class CommonPacketUnconnDataItem : public CommonPacketDataItem
{
public:
    /**
     * Create a new UnconnectedData item with an embedded CIPMssg object. The
     * CIPMssg object will get filled in by the CreateFromBuff() method and
     * MUST HAVE a scope equal to or longer than this UnconnectedData object
     */
    CommonPacketUnconnDataItem( ) : CommonPacketDataItem( 0x00B2)
    {
    }

    virtual ~CommonPacketUnconnDataItem()
    {
    }

protected:
};

/**
 * CommonPacketUnconnData with an embedded CIP packet
 *
 * @author Brian Meinke
 * @version 1
 * @since December 17, 2003
 */
class CommonPacketCIPDataItem : public CommonPacketUnconnDataItem
{
public:
    CommonPacketCIPDataItem() : CommonPacketUnconnDataItem()
    {
    }

    virtual ~CommonPacketCIPDataItem()
    {
    }

    virtual const CIPMssg* GetCipMssg() const = 0;

    /**
     * Copies the entire message into the given raw byte array
     *
     * @param rawBuff Byte array to copy the message into
     *
     * @return Number of bytes copied into the byte array
     */
    virtual int GetRawBytes(ByteArray_t &rawBuff) const
    {
        const CIPMssg*  cipMssg = GetCipMssg();
        int bytesUsed = 0;

        bytesUsed += CommonPacketDataItem::GetRawBytes(rawBuff);
        if( cipMssg != NULL)
        {
            bytesUsed += cipMssg->GetRawBytes(rawBuff);
        }
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
        int     bytesUsed = 0;

        bytesUsed += CommonPacketDataItem::CreateFromBuff( rawBuff, buffSz);
        if( m_dataLength > 0)
        {
            CIPMssg* cipMssg = (CIPMssg*)GetCipMssg();
            if( cipMssg != NULL)
            {
                bytesUsed += cipMssg->CreateFromBuff( &rawBuff[ bytesUsed], m_dataLength);
            }
        }

        return( bytesUsed);
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        const CIPMssg*  cipMssg = GetCipMssg();

        // Make sure our data length is accurate
        CommonPacketDataItem::Print();
        if( (CalcDataLength() > 0) && ( cipMssg != NULL))
        {
            cipMssg->Print();
        }
    }

protected:

    /**
     * Method used to get the length of data contained in the CommonPacketDataItem
     *
     * @return The length of data contained in the CommonPacketDataItem
     */
    virtual uint16_t CalcDataLength() const
    {
        int dataLen = 0;
        const CIPMssg*  cipMssg = GetCipMssg();

        if( cipMssg != NULL)
        {
            dataLen = GetCipMssg()->GetLength();
        }
        return( dataLen);
    }
};

/**
 * Common packet encapsulation of a CIP Forward Open request
 *
 * @author Brian Meinke
 * @version 1
 * @since December 18, 2003
 */
class CommonPacketFwdOpenDataItem : public CommonPacketCIPDataItem
{
public:
    CommonPacketFwdOpenDataItem() : CommonPacketCIPDataItem()
    {
    }

    CommonPacketFwdOpenDataItem( const uint8_t *requestPath, uint16_t reqPathLen) : CommonPacketCIPDataItem(),
        m_cipMssg( requestPath, reqPathLen)
    {
    }

    virtual ~CommonPacketFwdOpenDataItem()
    {
    }

    void SetRequestPath( const ByteArray_t &requestPath)
    {
        m_cipMssg.SetRequestPath(requestPath);
    }

    void SetRequestPath( const uint8_t requestPath[])
    {
        SetRequestPath( ByteArray_t( requestPath, sizeof( requestPath)));
    }

    /**
     * Returns a reference to the embedded CIPForwardOpenRequest message object
     *
     * @return A reference to the embedded CIPForwardOpenRequest message object
     */
    virtual const CIPMssg* GetCipMssg() const
    {
        return( &m_cipMssg);
    }

    /**
     * Sets the requested packet interval parameters (in microseconds)
     *
     * @param origRpi The desired originator-to-target requested packet interval (us)
     * @param targRpi The desired target-to-originator requested packet interval (us)
     */
    void SetRequestedPacketInterval( const uint32_t origRpi, const uint32_t targRpi)
    {
        m_cipMssg.SetTimingParams( targRpi, origRpi);
    }

protected:

    /**
     * Embedded ForwardOpen request message object
     */
    CIPForwardOpenRequest   m_cipMssg;
};

/**
 * Common packet wrapper for a Forward Open response
 *
 * @author Brian Meinke
 * @version 1
 * @since December 18, 2003
 */
class CommonPacketFwdOpenRespItem : public CommonPacketCIPDataItem
{
public:
    CommonPacketFwdOpenRespItem() : CommonPacketCIPDataItem()
    {
    }

    virtual ~CommonPacketFwdOpenRespItem()
    {
    }

    /**
     * Returns a reference to the embedded CIPForwardOpenResponse message object
     *
     * @return A reference to the embedded CIPForwardOpenResponse message object
     */
    virtual const CIPMssg* GetCipMssg() const
    {
        return( &m_cipMssg);
    }
protected:

    /**
     * Embedded ForwardOpen response message object
     */
    CIPForwardOpenResponse  m_cipMssg;
};

/**
 * Common packet wrapper for a CIP Sequenced Address data item
 *
 * @author Brian Meinke
 * @version 1
 * @since January 7, 2004
 */
class CommonPacketSequencedDataItem : public CommonPacketDataItem
{
public:
    CommonPacketSequencedDataItem( uint32_t connId=(uint32_t)-1, uint32_t seqNo=0) : CommonPacketDataItem( 0x8002),
        m_connId( connId), m_seqNo( seqNo)
    {
    }

    void SetConnectionId( uint32_t connId)
    {
        m_connId = connId;
    }
    uint32_t GetConnectionId() const
    {
        return( m_connId);
    }

    void SetSequenceNo( uint32_t seqNo)
    {
        m_seqNo = seqNo;
    }
    uint32_t GetSequenceNo() const
    {
        return( m_seqNo);
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        CommonPacketDataItem::Print();
        printf( "Connection ID: $%08X\n", m_connId);
        printf( "Sequence Number: %d\n", m_seqNo);
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

        bytesUsed += CommonPacketDataItem::GetRawBytes(rawBuff);
        bytesUsed += AddRawItem( m_connId, rawBuff);
        bytesUsed += AddRawItem( m_seqNo, rawBuff);
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
    virtual int CreateFromBuff(const uint8_t *rawBuff, size_t buffSz)
    {
        int bytesUsed = 0;

        bytesUsed += CommonPacketDataItem::CreateFromBuff(rawBuff, buffSz);
        bytesUsed += GetRawItem( m_connId, &rawBuff[ bytesUsed]);
        bytesUsed += GetRawItem( m_seqNo, &rawBuff[ bytesUsed]);

        return( bytesUsed);
    }

protected:
    /**
     * Method used to get the length of data contained in the CommonPacketDataItem
     *
     * @return The length of data contained in the CommonPacketDataItem
     */
    virtual uint16_t CalcDataLength() const
    {
        return( sizeof( m_connId) + sizeof( m_seqNo));
    }

    uint32_t        m_connId;

    uint32_t        m_seqNo;
};

/**
 * CommonPacketDataItem specialized to hold IP address information
 *
 * @author Brian Meinke
 * @version 1
 * @since December 17, 2003
 */
class CommonPacketTcpDataItem : public CommonPacketDataItem
{
public:
    CommonPacketTcpDataItem( uint16_t typeId=0x8000) : CommonPacketDataItem( typeId),
        m_sockFamily( AF_INET), m_portNo( 2222), m_ipAddress( INADDR_ANY)
    {
    }

    CommonPacketTcpDataItem( const CommonPacketTcpDataItem &copy) : CommonPacketDataItem( copy),
        m_sockFamily( AF_INET), m_portNo( 2222), m_ipAddress( INADDR_ANY)
    {
        m_sockFamily = copy.m_sockFamily;

        m_portNo = copy.m_portNo;

        m_ipAddress = copy.m_ipAddress;
    }

    virtual ~CommonPacketTcpDataItem()
    {
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

        bytesUsed += CommonPacketDataItem::CreateFromBuff(rawBuff, buffSz);
        bytesUsed += GetRawItem( m_sockFamily, &rawBuff[ bytesUsed]);
        bytesUsed += GetRawItem( m_portNo, &rawBuff[ bytesUsed]);
        bytesUsed += GetRawItem( m_ipAddress, &rawBuff[ bytesUsed]);

        m_sockFamily = htons((uint16_t) m_sockFamily);

        bytesUsed += 8; // There are 6 pad bytes
        return( bytesUsed);
    }

    /**
     * Copies the entire header structure into the given raw byte array
     *
     * @param rawBuff Byte array to copy the header data into
     *
     * @return Number of bytes copied into the byte array
     */
    virtual int GetRawBytes(ByteArray_t &rawBuff) const
    {
        int bytesUsed = 0;

        bytesUsed += CommonPacketDataItem::GetRawBytes( rawBuff);
        bytesUsed += AddRawItem( m_sockFamily, rawBuff);
        bytesUsed += AddRawItem( m_portNo, rawBuff);
        bytesUsed += AddRawItem( m_ipAddress, rawBuff);
        for( int ii=0; ii<8; ii++, bytesUsed++)
        {
            rawBuff.push_back( 0x00);
        }

        return( bytesUsed);
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        struct in_addr  junk;
        junk.s_addr = m_ipAddress;

        CommonPacketDataItem::Print();
        printf( "Socket Family: %d\n", m_sockFamily);
        printf( "Port Number: %d\n", htons((uint16_t) m_portNo));
        printf( "IP Address: %s\n", inet_ntoa( junk));
    }

    void GetSocketInfo( IpMulticastAddr_t &socketInfo) const
    {
        memset(&socketInfo.addr, 0, sizeof(socketInfo.addr));
        memset(&socketInfo.mreq, 0, sizeof(socketInfo.mreq));
        socketInfo.addr.sin_addr.s_addr = htonl((uint32_t)INADDR_ANY);
        socketInfo.addr.sin_family = m_sockFamily;
        socketInfo.addr.sin_port = m_portNo;
        socketInfo.mreq.imr_multiaddr.s_addr = m_ipAddress;
        socketInfo.mreq.imr_interface.s_addr = htonl((uint32_t) INADDR_ANY);
    }

protected:
    /**
     * Method used to get the length of data contained in the CommonPacketDataItem
     *
     * @return The length of data contained in the CommonPacketDataItem
     */
    virtual uint16_t CalcDataLength() const
    {
        return( sizeof( m_sockFamily) +  sizeof( m_portNo) + sizeof( m_ipAddress));
    }

public:
    uint16_t    m_sockFamily;

    uint16_t    m_portNo;

    in_addr_t   m_ipAddress;
};

/**
 * Data type used to manage a list of common packet data packets
 */
typedef std::list< const CommonPacketDataItem*>     CommonPacketItemList_t;
typedef CommonPacketItemList_t::iterator            CommonPacketItemListItr_t;
typedef CommonPacketItemList_t::const_iterator      CommonPacketItemListCItr_t;

/**
 * Class used to manage a common packet
 *
 * @author Brian Meinke
 * @version 1
 * @since December 13, 2003
 */
class CommonPacket : public MssgBase
{
public:
    /**
     * Default constructor. Creates an empty header object
     */
    CommonPacket() : MssgBase()
    {
    }

    /**
     * Constructs a CommonPacketHeader object from an array of raw bytes
     *
     * @param rawBuff Buffer containing a CIP heder in raw byte form
     */
    CommonPacket( const ByteArray_t &rawBuff) : MssgBase()
    {
        CreateFromBuff(rawBuff.c_str(), rawBuff.size());
    }

    virtual ~CommonPacket()
    {
        Clear();
    }

    /**
     * Returns a reference to the item list in the header
     *
     * @return A reference to the item list in the header
     */
    const CommonPacketItemList_t& GetItemList() const
    {
        return( m_itemList);
    }

    /**
     * Returns a reference to the item list in the header
     *
     * @return A reference to the item list in the header
     */
    CommonPacketItemList_t& GetItemList()
    {
        return( m_itemList);
    }

    /**
     * Adds a common packet data item to this common packet format data packet
     *
     * @param dataItem Common packet data item to add to this common packet format data packet
     */
    void AddItem( const CommonPacketDataItem *dataItem)
    {
        m_itemList.push_back( dataItem);
    }

    /**
     * Returns the number of CommonPacketDataItems contained in this CommonPacket
     *
     * @return The number of CommonPacketDataItems contained in this CommonPacket
     */
    uint16_t GetItemCount() const
    {
        return( m_itemList.size());
    }

    /**
     * Gets the length of this common packet formatted data packet
     *
     * @return The length of this common packet formatted data packet
     */
    int GetLength() const
    {
        CommonPacketItemListCItr_t  itr;
        int     packetLen = 0;
        int     ii=0, itemLen;

        if( m_itemList.size() > 0)
        {
            // Start with the sizeof our item count specifier
            packetLen += sizeof( uint16_t);
            for( ii=0, itr=m_itemList.begin(); itr!=m_itemList.end(); ii++, itr++)
            {
                const CommonPacketDataItem  *dataItem = *itr;
                itemLen = dataItem->GetLength();
                packetLen += itemLen;
            }
        }

        return( packetLen);
    }

    /**
     * Gets the length of the data segment of this common packet formatted data packet
     *
     * @return The length of the data segment of this common packet formatted data packet
     */
    int GetDataLength() const
    {
        CommonPacketItemListCItr_t  itr;
        int     packetLen = 0;
        int     ii=0, itemLen;

        if( m_itemList.size() > 0)
        {
            for( ii=0, itr=m_itemList.begin(); itr!=m_itemList.end(); ii++, itr++)
            {
                const CommonPacketDataItem  *dataItem = *itr;
                itemLen = dataItem->GetLength();
                packetLen += itemLen;
            }
        }

        return( packetLen);
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
        uint16_t    itemCount;
        int         buffIdx=0;

        Clear();

        // Extract the item count
        buffIdx += GetRawItem( itemCount, &rawBuff[ buffIdx]);

        // Create 'itemCount' data items
        while( itemCount-- > 0)
        {
            CommonPacketDataItem    *newItem = NULL;

            buffIdx += CreateDataItem( &rawBuff[ buffIdx], buffSz-buffIdx+1, newItem);

            // If we dont know what type this is
            if( newItem != NULL)
            {
                m_itemList.push_back( newItem);
            }
        }

        return( buffIdx);
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
        CommonPacketItemListCItr_t  itr;
        int         bytesUsed = 0;
        uint16_t    itemCount = m_itemList.size();

        // Copy the item count into the buffer
        bytesUsed += AddRawItem( itemCount, rawBuff);

        if(m_itemList.size() > 0)
        {
            // Now we get to copy the item list int othe buffer (hoorray!!)
            for( itr=m_itemList.begin(); itr!=m_itemList.end(); itr++)
            {
                const CommonPacketDataItem *dataItem = *itr;

                if( dataItem != NULL)
                {
                    bytesUsed += dataItem->GetRawBytes( rawBuff);
                }
            }
        }

        return( bytesUsed);
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        CommonPacketItemListCItr_t  itr;
        int                     itemIdx=0;

        printf( "Item Count: %d\n", m_itemList.size());
        if( m_itemList.size() > 0)
        {
            for( itr=m_itemList.begin(); itr!=m_itemList.end(); itr++)
            {
                printf( "\tItem %d\n", itemIdx++);  fflush( stdout);
                const CommonPacketDataItem  *item = *itr;
                item->Print();
            }
        }
    }

    /**
     * Clears the CommonPacketDataItems from our list and
     * frees the memory we allocated for each of them
     */
    virtual void Clear()
    {
        CommonPacketItemListCItr_t  itr;
        int     itemIdx = 0;
        if( m_itemList.size() > 0)
        {
            for( itr=m_itemList.begin(); itr != m_itemList.end(); itr++)
            {
                const CommonPacketDataItem *dataItem = *itr;
                delete dataItem;
                itemIdx++;
            }
        }
        m_itemList.clear();
    }

    /**
     * Returns a reference to the first data item in the data item list
     * with a type ID matching typeID
     *
     * @param typeId The type of data item to return
     *
     * @return A reference to the first data item in the data item list with
     *         a type ID matching typeID or NULL a data item with a matching
     *         type ID is contained in the list
     */
    const CommonPacketDataItem* GetDataItem( uint16_t typeId) const
    {
        CommonPacketItemListCItr_t  itr;
        const CommonPacketDataItem* retVal = NULL;

        for( itr=m_itemList.begin(); itr!=m_itemList.end(); itr++)
        {
            const CommonPacketDataItem* dataItem = *itr;

            if( dataItem == NULL)   continue;

            if( dataItem->GetTypeId() == typeId)
            {
                retVal = dataItem;
                break;
            }
        }

        return( retVal);
    }
protected:

    /**
     * Creates a new CommonPacketDataItem from the given byte buffer
     *
     * @param rawBuff Buffer to create the new CommonPacketDataItem from
     * @param buffSz  Number of bytes available in the raw byte buffer
     * @param newItem Reference to the object to be created
     *
     * @return Number of bytes used to create the new CommonPacketDataItem object
     */
    virtual int CreateDataItem( const uint8_t *rawBuff, size_t buffSz,  CommonPacketDataItem* &newItem)
    {
        uint16_t    typeId;
        uint16_t    srvcCode;
        int         buffIdx=0;

        // Read the type of the next data item. We do not want
        //  to increment buffIdx here because it will
        //  be incremented below when we call CreateFromBuff()
        GetRawItem( typeId, &rawBuff[ buffIdx]);
        switch( typeId)
        {
        case 0x0000:
            newItem = new CommonPacketNullAddressItem;
            break;
        case 0x00B2:
            // See which type of CIP message is embedded
            GetRawItem( srvcCode, &rawBuff[ buffIdx+4]);
            switch( srvcCode)
            {
            case 0xD4:
                newItem = new CommonPacketFwdOpenRespItem;
                break;
            default:
                break;
            }
            break;
        case 0x8000:
        case 0x8001:
            newItem = new CommonPacketTcpDataItem( typeId);
            break;
        default:
            break;
        }

        // If we dont know what type this is
        if( newItem != NULL)
        {
            // Read the next data item
            buffIdx += newItem->CreateFromBuff( &rawBuff[ buffIdx], buffSz-buffIdx+1);
        }

        return( buffIdx);
    }

private:
    CommonPacketItemList_t  m_itemList;
};

#endif // _COMMONPACKET_H_INCLUDED_

