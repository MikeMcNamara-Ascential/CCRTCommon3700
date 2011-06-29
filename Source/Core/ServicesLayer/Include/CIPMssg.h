//******************************************************************************
// Description:
//    Data defs needed for CIP communication
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
//     $Log: /Core/ServicesLayer/Include/CIPMssg.h $
// 
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
//
// 2     6/17/04 9:25p Bmeinke
// Changed the originator serial number inside the second
// CIPForwardOpenRequest constructor to match the examples from Rockwell
// (for easier packet comparisons)
// Clear m_appReply inside the CIPForwardOpenResponse() constructor
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

#ifndef _CIPMSSG_H_INCLUDED_
#define _CIPMSSG_H_INCLUDED_

#include "CIPHeader.h"
#include "MssgBase.h"

/**
 * Base class for all CIP message objects
 *
 * @author Brian Meinke
 * @version 1
 * @since December 123 2003
 */
class CIPMssg : public MssgBase
{
public:
	CIPMssg() : MssgBase()
	{
	}

	virtual ~CIPMssg()
	{
	}

	virtual int GetLength() const
	{
		return( 0);
	}
};


/**
 * Base class for all CIP request message objects
 *
 * @author Brian Meinke
 * @version 1
 * @since December 123 2003
 */
class CIPRequest : public CIPMssg
{
public:
	CIPRequest() : CIPMssg()
	{
	}

	CIPRequest( uint8_t srvcCode) : CIPMssg()
	{
		m_cipHeader.SetServiceCode( srvcCode);
	}

	CIPRequest( uint8_t srvcCode, const uint8_t *requestpath, uint16_t reqPathLen) : CIPMssg(),
		m_cipHeader( CIPHeader( srvcCode, requestpath, reqPathLen))
	{
	}

	CIPRequest( const CIPRequest &copy) : CIPMssg(),
		m_cipHeader( copy.GetHeader())
	{
	}

	virtual ~CIPRequest()
	{
	}

	const CIPHeader& GetHeader() const
	{
		return( m_cipHeader);
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
		int bytesUsed = m_cipHeader.GetRawBytes(rawBuff);

		return(  bytesUsed);
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
		int bytesUsed = m_cipHeader.CreateFromBuff(rawBuff, buffSz);

		return(  bytesUsed);
	}

	/**
	 * Print out the contents of the message object
	 */
	virtual void Print() const
	{
		m_cipHeader.Print();
	}

	/**
	 * Returns the length (in bytes) of this CIP message
	 *
	 * @return The length (in bytes) of this CIP message
	 */
	virtual int GetLength() const
	{
		return( m_cipHeader.GetLength());
	}

protected:

	CIPHeader	m_cipHeader;
};

/**
 * Base class for all CIP Reply message objects
 *
 * @author Brian Meinke
 * @version 1
 * @since December 123 2003
 */
class CIPReply : public CIPMssg
{
public:
	CIPReply() : CIPMssg()
	{
	}

	CIPReply( uint8_t srvcCode) : CIPMssg()
	{
		m_cipHeader.SetServiceCode( srvcCode);
	}

	virtual ~CIPReply()
	{
	}

	const CIPReplyHeader& GetHeader() const
	{
		return( m_cipHeader);
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
		int bytesUsed = m_cipHeader.GetRawBytes(rawBuff);

		return(  bytesUsed);

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
		int bytesUsed = m_cipHeader.CreateFromBuff(rawBuff, buffSz);

		return(  bytesUsed);
	}

	/**
	 * Print out the contents of the message object
	 */
	virtual void Print() const
	{
		m_cipHeader.Print();
	}

	/**
	 * Returns the length (in bytes) of this CIP message
	 *
	 * @return The length (in bytes) of this CIP message
	 */
	virtual int GetLength() const
	{
		return( m_cipHeader.GetLength());
	}

protected:

	CIPReplyHeader	m_cipHeader;
};


#include <_pack1.h>

typedef struct
{
	/**
	 * Priority/timeout tick.  Specifies a tick time for the
	 * connection opening process of 32 ms.
	 * See CIP Common Specification volume 1, section 3-5.5.1.3.
	 */
	uint8_t		m_prioTimeTick;

	/**
	 * Timeout ticks.  Connection opening process will timeout
	 * after 155 32-ms ticks.
	 * See CIP Common Specification volume 1, section 3-5.5.1.3.
	 */
	uint8_t		m_timeoutTicks;

	/**
	 * O->T (originator to target) requested connection ID value.
	 * The 20-COMM-E will assign connection ID's, so this value doesn't matter.
	 */
	uint32_t	m_origTargConnId;

	/**
	 * T->O requested connection ID value.
	 * The 20-COMM-E will assign connection ID's, so this value doesn't matter.
	 */
	uint32_t	m_targOrigConnId;

	/**
	 * Connection serial number. Use any value you wish, as long as it's unique.
	 */
	uint16_t	m_connSerialNum;

	/**
	 * Originator's vendor ID (1 = Allen-Bradley)
	 */
	uint16_t	m_origVendID;

	/**
	 * Originator's serial number.
	 */
	uint32_t	m_origSerialNum;

	/**
	 * Connection timeout multiplier.  Missing from the EtherNet/IP spec,
	 * but a value of 3 maps to 32 packet intervals.
	 */
	uint8_t		m_timeoutMultiplier;

	/**
	 * Reserved bytes
	 */
	uint8_t		m_reserved[ 3];

	/**
	 * O->T requested packet interval (RPI) in microseconds.
	 */
	uint32_t	m_origTargRpi;

	/**
	 * connection parameters.
	 * See CIP Common Specification volume 1, section 3-5.5.1.1.
	 */
	uint16_t	m_origTargParams;

	/**
	 * T->O requested packet interval (RPI) in microseconds.
	 */
	uint32_t	m_targOrigRpi;

	/**
	 * T->O connection parameters.
	 * See CIP Common Specification volume 1, section 3-5.5.1.1.
	 */
	uint16_t	m_targOrigParams;

	/**
	 * Transport type/trigger.
	 */
	uint8_t		m_xportTypeTrig;

	/**
	 * Length of the connection path
	 */
	mutable uint8_t		m_connPathWords;

} CIPFwdOpen_t;

#include <_packpop.h>

/**
 * CIP Forward Open message request object. A ForwardOpen request
 * is used to establish an I/O channel between us and the CIP device we
 * want to talk to. The response to this request will contain 4 data items:
 * 1. A NULL address item
 * 2. An unconnected data message
 * 3. An Originator socket info message: this is the socket info we will
 *    use when we send our I/O broadcast datagram to the CIP device
 * 4. A Target socket info message: this is the socket info we will
 *    receive I/O broadcast datagrams from the CIP device on
 *
 * @author Brian Meinke
 * @version 1
 * @since December 18, 2003
 */
class CIPForwardOpenRequest : public CIPRequest, private CIPFwdOpen_t
{
public:
	CIPForwardOpenRequest() : CIPRequest(0x54)
	{
	}

	CIPForwardOpenRequest( const uint8_t *requestpath, uint16_t reqPathLen) : CIPRequest(0x54, requestpath, reqPathLen)
	{
		// Tick time = 32ms (2^5)
		m_prioTimeTick = 0x05;
		// 155 ticks * 32ms/tick = 4960 ms timeout
		m_timeoutTicks = 0x9B;
		// The connection ID's will be filled in by the 20-COMM-E
		m_origTargConnId = 0x00;
		m_targOrigConnId = 0x00;
		// Must be any UNIQUE value you want
		m_connSerialNum = 0x0009;
		// We dont have a vendor ID, so we just use Allen Bradely's
		m_origVendID = 0x0001;
		// We dont have a serial ID, so we just use Allen Bradely's
		m_origSerialNum =  0x00186C7E;//0x10186C7E;
		m_timeoutMultiplier = 0x00;
		m_reserved[ 0] = m_reserved[ 1] = m_reserved[ 2] = 0x00;
		// Requested packet interval (us to the device)
		m_origTargRpi = 1000000;	// 1000000 us = 1000 ms
		// $48 = Exclusive owner, point-to-point, scheduled, fixed length
		// $2E = data length
		m_origTargParams = 0x482E;
		// Requested packet interval (device to us)
		m_targOrigRpi = 100000;		// 1000000 us = 1000 ms
		// $28 = Exclusive owner, Multicast, scheduled, fixed length
		// $2E = data length
		m_targOrigParams = 0x282E;
		// Class 1 message
		m_xportTypeTrig = 0x01;

		///////////////////////////////////////////////////////////
		// Logical Segment, special, 8-bit address
		m_connPath.push_back(0x34);
		// "See key format table"
		m_connPath.push_back(0x04);
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		// 8-byte electronic key (all 0's = no key)
		m_connPath.push_back(0x00);
		m_connPath.push_back(0x00);
		m_connPath.push_back(0x00);
		m_connPath.push_back(0x00);
		m_connPath.push_back(0x00);
		m_connPath.push_back(0x00);
		m_connPath.push_back(0x00);
		m_connPath.push_back(0x00);
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		// Specify the class ID (class ID = 6 => Assembly Object)
		m_connPath.push_back(0x20);
		m_connPath.push_back(0x04);
		//
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		// Specify the instance ID (Instance ID = 6 => Confiuration)
		//	Instance 6 = 32-Point input with no status bit
		m_connPath.push_back(0x24);
		m_connPath.push_back(0x06);
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		// Specify the consumer connection point (Connection Point=2)
		m_connPath.push_back(0x2c);
		m_connPath.push_back(0x02);
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		// Specify the producer connection point (Connection Point=1)
		m_connPath.push_back(0x2c);
		m_connPath.push_back(0x01);
		///////////////////////////////////////////////////////////
	}

	CIPForwardOpenRequest( const CIPForwardOpenRequest &copy) :
		CIPRequest(copy)
	{
		m_prioTimeTick = copy.m_prioTimeTick;
		m_timeoutTicks = copy.m_timeoutTicks;
		m_origTargConnId = copy.m_origTargConnId;
		m_targOrigConnId = copy.m_targOrigConnId;
		m_connSerialNum = copy.m_connSerialNum;
		m_origVendID = copy.m_origVendID;
		m_origSerialNum = copy.m_origSerialNum;
		m_timeoutMultiplier = copy.m_timeoutMultiplier;
		m_reserved[ 0] = m_reserved[ 1] = m_reserved[ 2] = 0x00;
		m_origTargRpi = copy.m_origTargRpi;
		m_origTargParams = copy.m_origTargParams;
		m_targOrigRpi = copy.m_targOrigRpi;
		m_targOrigParams = copy.m_targOrigParams;
		m_xportTypeTrig = copy.m_xportTypeTrig;
		m_connPath.assign( copy.m_connPath);
	}

	virtual ~CIPForwardOpenRequest()
	{
	}

	/**
	 * Sets the requested packet interval parameters (in microseconds)
	 *
	 * @param origRpi The desired originator-to-target requested packet interval (us)
	 * @param targRpi The desired target-to-originator requested packet interval (us)
	 */
	void SetTimingParams( const uint32_t origRpi, const uint32_t targRpi)
	{
		m_targOrigRpi = targRpi;
		m_origTargRpi = origRpi;
	}

	/**
	 * Reads the requested packet interval parameters (in microseconds)
	 *
	 * @param targRpi The desired target-to-originator requested packet interval (us)
	 * @param origRpi The desired originator-to-target requested packet interval (us)
	 */
	void GetTimingParams( uint32_t &targRpi, uint32_t &origRpi) const
	{
		targRpi = m_targOrigRpi;
		origRpi = m_origTargRpi;
	}

	/**
	 * Sets the request path for this CIP message
	 *
	 * @param requestpath
	 *               The request path for this CIP message
	 */
	void SetRequestPath( const ByteArray_t requestpath)
	{
		m_cipHeader.SetRequestPath(requestpath);
	}

	/**
	 * Print out the contents of the message object
	 */
	virtual void Print() const
	{
		ByteArrayCItr_t	itr;

		CIPRequest::Print();
		printf( "            Priority/Time Tick: %d\n", m_prioTimeTick);
		printf( "                 Timeout Ticks: %d\n", m_timeoutTicks);
		printf( "            O->T Connection ID: %d\n", m_origTargConnId);
		printf( "            T->O Connection ID: %d\n", m_targOrigConnId);
		printf( "      Connection Serial Number: $%08X\n", m_connSerialNum);
		printf( "           Originator Venor ID: %d\n", m_origVendID);
		printf( "      Originator Serial Number: %d\n", m_origSerialNum);
		printf( "            Timeout Multiplier: %d\n", m_timeoutMultiplier);
		printf( "O->T Requested Packet Interval: %d\n", m_origTargRpi);
		printf( "    O->T Connection Paramaters: %X\n", m_origTargParams);
		printf( "T->O Requested Packet Interval: %d\n", m_targOrigRpi);
		printf( "    T->O Connection Paramaters: %X\n", m_targOrigParams);
		printf( "        Transport Type/Trigger: %X\n", m_xportTypeTrig);
		printf( "        Connection Path length: %d\n", m_connPath.size()/2);
		printf( "Connection Path:\n\t");
		for( itr=m_connPath.begin(); itr!=m_connPath.end(); itr++)
		{
			printf( "$%02hhX ", *itr);
		}
		printf( "\n");
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
		int 	bytesUsed = 0;

		if( (bytesUsed=CIPRequest::GetRawBytes(rawBuff)) > 0)
		{
			m_connPathWords = m_connPath.size() / 2;

			bytesUsed += AddRawItem( m_prioTimeTick, rawBuff);
			bytesUsed += AddRawItem( m_timeoutTicks, rawBuff);
			bytesUsed += AddRawItem( m_origTargConnId, rawBuff);
			bytesUsed += AddRawItem( m_targOrigConnId, rawBuff);
			bytesUsed += AddRawItem( m_connSerialNum, rawBuff);
			bytesUsed += AddRawItem( m_origVendID, rawBuff);
			bytesUsed += AddRawItem( m_origSerialNum, rawBuff);
			bytesUsed += AddRawItem( m_timeoutMultiplier, rawBuff);
			bytesUsed += AddRawItem( m_reserved[ 0], rawBuff);
			bytesUsed += AddRawItem( m_reserved[ 1], rawBuff);
			bytesUsed += AddRawItem( m_reserved[ 2], rawBuff);
			bytesUsed += AddRawItem( m_origTargRpi, rawBuff);
			bytesUsed += AddRawItem( m_origTargParams, rawBuff);
			bytesUsed += AddRawItem( m_targOrigRpi, rawBuff);
			bytesUsed += AddRawItem( m_targOrigParams, rawBuff);
			bytesUsed += AddRawItem( m_xportTypeTrig, rawBuff);
			bytesUsed += AddRawItem( m_connPathWords, rawBuff);
			rawBuff += m_connPath;
			bytesUsed += m_connPath.size();
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
		int		bytesUsed = 0;

		bytesUsed += GetRawItem( m_prioTimeTick, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_timeoutTicks, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_origTargConnId, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_targOrigConnId, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_connSerialNum, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_origVendID, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_origSerialNum, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_timeoutMultiplier, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_reserved[ 0], &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_reserved[ 1], &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_reserved[ 2], &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_origTargRpi, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_origTargParams, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_targOrigRpi, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_targOrigParams, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_xportTypeTrig, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_connPathWords, &rawBuff[ bytesUsed]);
		if( m_connPathWords > 0)
		{
			m_connPath = ByteArray_t( &rawBuff[ bytesUsed], m_connPathWords*2);
			bytesUsed += m_connPath.size();
		}
		else
		{
			m_connPath.clear();
		}

		return( bytesUsed);
	}

	/**
	 * Returns the length (in bytes) of this CIP message
	 *
	 * @return The length (in bytes) of this CIP message
	 */
	virtual int GetLength() const
	{
		int len = CIPRequest::GetLength();

		len += sizeof( CIPFwdOpen_t) + m_connPath.size();
		return( len);
	}

private:

	/**
	 * Connection Path
	 */
	ByteArray_t	m_connPath;
};


#include <_pack1.h>

typedef struct
{
	/**
	 * O->T (originator to target) actual connection ID value.
	 */
	uint32_t	m_origTargConnId;

	/**
	 * T->O actual connection ID value.
	 */
	uint32_t	m_targOrigConnId;

	/**
	 * Connection serial number. Use any value you wish, as long as it's unique.
	 */
	uint16_t	m_connSerialNum;

	/**
	 * Originator's vendor ID (1 = Allen-Bradley)
	 */
	uint16_t	m_origVendID;

	/**
	 * Originator's serial number.
	 */
	uint32_t	m_origSerialNum;

	/**
	 * O->T actual packet interval (API) in microseconds.
	 */
	uint32_t	m_origTargApi;

	/**
	 * T->O actual packet interval (API) in microseconds.
	 */
	uint32_t	m_targOrigApi;

	/**
	 * Number of 16-bit words in the Application Reply Field
	 */
	uint8_t		m_appReplySize;

	uint8_t		m_reserved;

} CIPFwdOpenRsp_t;

#include <_packpop.h>

/**
 * CIP Forward Open reply  object. Response to our ForwardOpen request.
 * The response to this request will contain 4 data items:
 * 1. A NULL address item
 * 2. An unconnected data message
 * 3. An Originator socket info message: this is the socket info we will
 *    use when we send our I/O broadcast datagram to the CIP device
 * 4. A Target socket info message: this is the socket info we will
 *    receive I/O broadcast datagrams from the CIP device on
 *
 * @author Brian Meinke
 * @version 1
 * @since December 18, 2003
 */
class CIPForwardOpenResponse : public CIPReply, public CIPFwdOpenRsp_t
{
public:
	CIPForwardOpenResponse() : CIPReply()
	{
		m_appReply.clear();
	}

	virtual ~CIPForwardOpenResponse()
	{
	}
	/**
	 * Print out the contents of the message object
	 */
	virtual void Print() const
	{
		ByteArrayCItr_t	itr;

		CIPReply::Print();
		printf( "            O->T Connection ID: $%08X\n", m_origTargConnId);
		printf( "            T->O Connection ID: $%08X\n", m_targOrigConnId);
		printf( "      Connection Serial Number: $%08X\n", m_connSerialNum);
		printf( "           Originator Venor ID: %d\n", m_origVendID);
		printf( "      Originator Serial Number: $%08X\n", m_origSerialNum);
		printf( "   O->T Actual Packet Interval: %d\n", m_origTargApi);
		printf( "   T->O Actual Packet Interval: %d\n", m_targOrigApi);
		printf( "         Application ReplySize: %d words\n", m_appReplySize);
		if( m_appReplySize > 0)
		{
			printf( "Application Reply:\n\t");
			for( itr=m_appReply.begin(); itr!=m_appReply.end(); itr++)
			{
				printf( "$%02hhX ", *itr);
			}
			printf( "\n");
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
		uint8_t	appReplySz = m_appReply.size() / 2;
		int 	bytesUsed = 0;

		if( (bytesUsed=CIPReply::GetRawBytes(rawBuff)) > 0)
		{
			bytesUsed += AddRawItem( m_origTargConnId, rawBuff);
			bytesUsed += AddRawItem( m_targOrigConnId, rawBuff);
			bytesUsed += AddRawItem( m_connSerialNum, rawBuff);
			bytesUsed += AddRawItem( m_origVendID, rawBuff);
			bytesUsed += AddRawItem( m_origSerialNum, rawBuff);
			bytesUsed += AddRawItem( m_origTargApi, rawBuff);
			bytesUsed += AddRawItem( m_targOrigApi, rawBuff);
			bytesUsed += AddRawItem( appReplySz, rawBuff);
			bytesUsed += AddRawItem( m_reserved, rawBuff);
			rawBuff += m_appReply;
			bytesUsed += m_appReply.size();
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
		int		bytesUsed = 0;

		bytesUsed += CIPReply::CreateFromBuff(rawBuff, buffSz);
		bytesUsed += GetRawItem( m_origTargConnId, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_targOrigConnId, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_connSerialNum, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_origVendID, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_origSerialNum, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_origTargApi, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_targOrigApi, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_appReplySize, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_reserved, &rawBuff[ bytesUsed]);
		if( m_appReplySize > 0)
		{
			m_appReply = ByteArray_t( &rawBuff[ bytesUsed], m_appReplySize*2);
			bytesUsed += m_appReply.size();
		}
		else
		{
			m_appReply.clear();
		}

		return( bytesUsed);
	}

	/**
	 * Returns the length (in bytes) of this CIP message
	 *
	 * @return The length (in bytes) of this CIP message
	 */
	virtual int GetLength() const
	{
		return( CIPReply::GetLength() + sizeof( CIPFwdOpenRsp_t) + m_appReply.size());
	}

private:
	/**
	 * Connection Path
	 */
	ByteArray_t	m_appReply;
};

#endif // _CIPMSSG_H_INCLUDED_

