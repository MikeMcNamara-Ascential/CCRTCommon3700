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
//     $Log: /Core/ServicesLayer/Include/CIPHeader.h $
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
// 1     2/24/04 11:09a Bmeinke
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

#ifndef _CIPHDR_H_INCLUDED_
#define _CIPHDR_H_INCLUDED_

#include "MssgBase.h"

/**
 * Class used to manage a CIP Message header
 *
 * @author Brian Meinke
 * @version 1
 * @since December 17, 2003
 */
class CIPHeader : public MssgBase
{
public:
	/**
	 * Creates an empty CIPHeader with no service code and no request path
	 */
	CIPHeader() : MssgBase(),
		m_serviceCode( 0x00)
	{
	}

	/**
	 * Creates a new CIPHeader object which is a copy of the given CIPHeader object
	 *
	 * @param copy   CIPHeader object to be copied into *this
	 */
	CIPHeader( const CIPHeader &copy) : MssgBase(),
		m_serviceCode( copy.m_serviceCode), m_requestPath( copy.m_requestPath)
	{
	}

	/**
	 * Creates a new CIPHeader object with the given service code and request path
	 *
	 * @param srvcCode
	 * @param reqPath  CIP request path for this CIPHeader
	 */
	CIPHeader( uint8_t srvcCode, const ByteArray_t &reqPath) : MssgBase() ,
		m_serviceCode( srvcCode), m_requestPath( reqPath)
	{
	}

	/**
	 * Creates a new CIPHeader object with the given service code and request path
	 *
	 * @param srvcCode
	 * @param reqPathLen Size (in bytes) of the CIP request path
	 * @param reqPath    CIP request path for this CIPHeader
	 */
	CIPHeader( uint8_t srvcCode, const uint8_t *reqPath, uint16_t reqPathLen) : MssgBase() ,
		m_serviceCode( srvcCode), m_requestPath( reqPath, reqPathLen)
	{
	}

	/**
	 * Returns the CIP service code contained in this CIP header
	 *
	 * @return The CIP service code contained in this CIP header
	 */
	uint8_t GetSerivceCode() const
	{
		return( m_serviceCode);
	}

	/**
	 * Returns the CIP request path contained in this CIP header
	 *
	 * @return The CIP request path contained in this CIP header
	 */
	const ByteArray_t GetRequestPath() const
	{
		return( m_requestPath);
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
		uint8_t	reqPathLen = m_requestPath.size() / 2;
		int		bytesUsed = 0;

		bytesUsed += AddRawItem( m_serviceCode,rawBuff);
		bytesUsed += AddRawItem( reqPathLen,rawBuff);
		rawBuff += m_requestPath;
		bytesUsed += m_requestPath.size();

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
		uint8_t	reqPathLen;
		int bytesUsed = 0;

		bytesUsed += GetRawItem( m_serviceCode, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( reqPathLen, &rawBuff[ bytesUsed]);
		m_requestPath = ByteArray_t( &rawBuff[ bytesUsed], reqPathLen);
		bytesUsed += m_requestPath.size();

		return( bytesUsed);
	}

	/**
	 * Print out the contents of the message object
	 */
	virtual void Print() const
	{
		ByteArrayCItr_t	itr;

		printf( "Service Code: $%02hhX\n", m_serviceCode);
		printf( "Request Path Length: %d\n", m_requestPath.size());
		if( m_requestPath.size() > 0)
		{
			printf( "Request Path: \n");
			for( itr=m_requestPath.begin(); itr!=m_requestPath.end(); itr++)
			{
				printf( "$%02hhX ", *itr);
			}
			printf( "\n");
		}
	}

	int GetLength() const
	{
		return( m_requestPath.size() + 2*sizeof(uint8_t));
	}

	void SetRequestPath( const ByteArray_t &requestPath)
	{
		m_requestPath.assign( requestPath);
	}
	void SetServiceCode( uint8_t serviceCode)
	{
		m_serviceCode = serviceCode;
	}
protected:
	uint8_t		m_serviceCode;

	ByteArray_t	m_requestPath;
};

/**
 * Header structure contained in CIP response messages
 *
 * @author Brian Meinke
 * @version 1
 * @since December 21, 2003
 */
class CIPReplyHeader : public MssgBase
{
public:
	CIPReplyHeader( uint8_t srvcCode=0x00) : MssgBase(),
		m_serviceCode( srvcCode), m_reserved( 0x00),
		m_generalStatus( 0x00)
	{
	}

	uint8_t GetSerivceCode() const
	{
		return( m_serviceCode);
	}

	uint8_t GetGeneralStatus() const
	{
		return( m_generalStatus);
	}

	const ByteArray_t& GetAddlStatus() const
	{
		return( m_addlStatus);
	}

	int GetLength() const
	{
		int len = sizeof( m_serviceCode) + sizeof( m_reserved) +
			sizeof( m_generalStatus) + sizeof( uint8_t) + m_addlStatus.size();

		return( len);
	}

	void SetServiceCode( uint8_t serviceCode)
	{
		m_serviceCode = serviceCode;
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
		uint8_t	addlStatsSz = m_addlStatus.size() / 2;
		int		bytesUsed = 0;

		bytesUsed += AddRawItem( m_serviceCode, rawBuff);
		bytesUsed += AddRawItem( m_reserved, rawBuff);
		bytesUsed += AddRawItem( m_generalStatus, rawBuff);
		bytesUsed += AddRawItem( addlStatsSz, rawBuff);
		rawBuff += m_addlStatus;
		bytesUsed += m_addlStatus.size();

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
		uint8_t			addlStatsSz;
		int				bytesUsed = 0;

		bytesUsed += GetRawItem( m_serviceCode, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_reserved, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( m_generalStatus, &rawBuff[ bytesUsed]);
		bytesUsed += GetRawItem( addlStatsSz, &rawBuff[ bytesUsed]);
		m_addlStatus = ByteArray_t( &rawBuff[ bytesUsed], addlStatsSz*2);
		bytesUsed += m_addlStatus.size();

		return( bytesUsed);
	}

	/**
	 * Print out the contents of the message object
	 */
	virtual void Print() const
	{
		ByteArrayCItr_t	itr;

		printf( "Service Code: $%02hhX\n", m_serviceCode);
		printf( "General Status: $%02hhX\n", m_generalStatus);
		printf( "Add'l Status Words: %d\n", m_addlStatus.size());
		if( m_addlStatus.size() > 0)
		{
			printf( "Additional Status: \n");
			for( itr=m_addlStatus.begin(); itr!=m_addlStatus.end(); itr++)
			{
				printf( "$%02hhX ", *itr);
			}
			printf( "\n");
		}
	}

private:
	uint8_t			m_serviceCode;

	uint8_t			m_reserved;

	uint8_t			m_generalStatus;

	ByteArray_t		m_addlStatus;
};

#endif // _CIPHDR_H_INCLUDED_

