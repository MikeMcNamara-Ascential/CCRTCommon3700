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
//     $Log: /Core/ServicesLayer/Include/EIPResponse.h $
// 
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
//
// 2     6/17/04 9:51p Bmeinke
// Changed MssgBaseData var name to eipData for better C++ coding
// standards compliance
// Changed some ints to uint32_t to remove compiler warnings
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

#ifndef _EIPRESPONSE_H_INCLUDED_
#define _EIPRESPONSE_H_INCLUDED_

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>				// socket stuff
#include <sys/types.h>			// socket stuff
#include <sys/socket.h>			// socket stuff
#include <netinet/tcp.h>		// socket stuff
#include <netinet/in.h>			// socket stuff
#include <string>

#include "EIPMssg.h"
#include "CommonPacket.h"

/**
 * Generic EIP response holder
 */
class EIPResponse : public EIPMssg
{
public:
	EIPResponse( uint16_t sessHdl=0) : EIPMssg( 0x0000, sessHdl)
	{
	}

	virtual ~EIPResponse()
	{
	}

	/**
	 * Reads an EthernetIP response from the given file descriptor
	 *
	 * @param fd     File descriptor from which the request should be read
	 *
	 * @return Number of bytes read
	 */
	virtual int Read( int fd)
	{
		int bytesRead;
		int dataRead=0, totalRead = 0;

		//printf( "EIPResponse::Read()\n");
		// Write out the header
		if( (bytesRead=m_header.Read(fd)) > 0)
		{
			totalRead += bytesRead;

			// If there is any message data to send
			const int dataSz = m_header.GetDataLength();
			if( dataSz > 0)
			{
				uint8_t 	eipData[ dataSz];

				// Clear our buffer to read the response data into
				memset( eipData, 0, dataSz);

				// Read the message data
				while( dataRead < dataSz)
				{
					if( (bytesRead=read( fd, &eipData[ dataRead], dataSz-dataRead)) > 0)
					{
						dataRead += bytesRead;
					}
					else
					{
						break;
					}
				}

				if( dataRead > 0)
				{
					// Update our message data content
					SetEipData( ByteArray_t(eipData, dataRead));
					totalRead += dataRead;
				}
			}
		}
		else
		{
			perror( "Error reading header");
		}

		return( totalRead);
	}

	/**
	 * Returns the session handle returned inside the response
	 *
	 * @return The session handle returned inside the response
	 */
	uint32_t GetSessionHandle() const
	{
		return( m_header.GetSessionHandle());
	}

	/**
	 * Copies the given ByteArray_t object into the Common Packet
	 * data portion of this message
	 *
	 * @param eipData ByteArray_t object to be copied into the Common
	 *                 Packet data portion of this EIP message
	 *
	 * @return The number of elements copied
	 */
	virtual int SetEipData( const ByteArray_t &eipData)
	{
		return( m_commonPacket.CreateFromBuff( eipData.c_str(), eipData.size()));
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
		int buffSz = 0;

		buffSz += EIPMssg::GetRawBytes(rawBuff);
		buffSz += m_commonPacket.GetRawBytes(rawBuff);

		return( buffSz);
	}

	/**
	 * Print out the contents of the message object
	 */
	virtual void Print() const
	{
		EIPMssg::Print();

		// Put it in the raw buffer
		m_commonPacket.Print();
	}

	const CommonPacket& GetCommonPacket() const
	{
		return( m_commonPacket);
	}

protected:
	/**
	 * Common packet formatted data
	 */
	CommonPacket		m_commonPacket;
};

/**
 * EthernetIP SendRRData response message object
 */
class EIPSendRRDataResp : public EIPResponse
{
public:
	EIPSendRRDataResp() : EIPResponse()
	{
	}

	virtual ~EIPSendRRDataResp()
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
		int buffSz = 0;

		// We need to inject some SendRRData specific data into the raw buffer.
		//	(don't call EIPResponse::GetRawBytes())
		buffSz += EIPMssg::GetRawBytes(rawBuff);
		buffSz += AddRawItem( m_ifcHandle, rawBuff);
		buffSz += AddRawItem( m_timeoutVal, rawBuff);
		buffSz += m_commonPacket.GetRawBytes(rawBuff);

		return( buffSz);
	}

	/**
	 * Copies the given ByteArray_t object into the data portion of this EIP message
	 *
	 * @param eipData ByteArray_t object to be copied into the data portion of this EIP message
	 *
	 * @return The number of elements copied
	 */
	virtual int SetEipData(const ByteArray_t &eipData)
	{
		const uint8_t	*junk = eipData.c_str();
		uint32_t		bytesUsed = 0;

		bytesUsed += GetRawItem( m_ifcHandle, &junk[ bytesUsed]);
		bytesUsed += GetRawItem( m_timeoutVal, &junk[ bytesUsed]);
		// Create a Common Packet object
		if( eipData.size() > bytesUsed)
		{
			bytesUsed += EIPResponse::SetEipData(eipData.substr(bytesUsed));
		}

		return( bytesUsed);
	}

protected:
	uint32_t	m_ifcHandle;

	uint16_t	m_timeoutVal;
};

#endif //_EIPRESPONSE_H_INCLUDED_
