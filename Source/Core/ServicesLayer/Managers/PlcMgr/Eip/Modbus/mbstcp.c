/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSTCP.C
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Handles Modbus/TCP specifics
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
**
** Porting: <none>0----<major>         Customization: <none>0----<major>
**
*****************************************************************************
*****************************************************************************
*/

#include "mbsinc.h"

#ifdef ET_IP_MODBUS

BOOL mbus_tcpSend(MBUS_SESSION* pSession, INT32 lLen, INT32* plenSent);
void mbus_TcpAdjustHeader(mbus_MbapHeader* pHdr);


#ifndef SYNCHRONOUS_CONNECTION
/*---------------------------------------------------------------------------
** mbus_tcpStartConnection( )
**
** Starts a TCP connection process
**---------------------------------------------------------------------------
*/

BOOL mbus_tcpStartConnection( MBUS_SESSION* pSession )                 
{    
	UINT32                lOptSet = 1;
	struct sockaddr_in    sCli_addr;
	INT32                 nStatus;
	UINT32                 lAddrLen;        
	UINT32                lSize;

	/* Get a handle to the new TCP socket */
	if ( ( pSession->lSocket = platformSocket( AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET )
	{
		mbus_notifyEvent(NM_MBUS_ERROR_USING_SOCKETS, 0, 0);                
		return FALSE;
	}

	/* Keepalives are being sent */
	if ( ( nStatus = platformSetSockOpt( pSession->lSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&lOptSet, sizeof(lOptSet) ) ) == SOCKET_ERROR )
		mbusDumpStr0(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "socketStartTCPConnection error setting option SO_KEEPALIVE");

	/* Allocate big enough send and receive buffers */
	lSize = MBUS_SMALL_BUFFER_SIZE;    
	if ( ( nStatus = platformSetSockBufSize( pSession->lSocket, SOL_SOCKET, SO_SNDBUF, (char *)&lSize, sizeof(lSize) ) ) == SOCKET_ERROR )
		mbusDumpStr0(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "socketStartTCPConnection error setting option SO_SNDBUF");
	if ( ( nStatus = platformSetSockBufSize( pSession->lSocket, SOL_SOCKET, SO_RCVBUF, (char *)&lSize, sizeof(lSize) ) ) == SOCKET_ERROR )
		mbusDumpStr0(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "socketStartTCPConnection error setting option SO_RCVBUF");

	/* Bind the socket */
	memset( ( char * ) &sCli_addr, 0, sizeof( sCli_addr ) );
	sCli_addr.sin_family = AF_INET;
	sCli_addr.sin_port = htons( 0 );
	platformGetInternetAddress(sCli_addr.sin_addr) = pSession->lHostIPAddr;

	if ( platformBind( pSession->lSocket, ( struct sockaddr * ) & sCli_addr, sizeof( sCli_addr ) ) == SOCKET_ERROR ) 
	{
		mbus_notifyEvent(NM_MBUS_ERROR_USING_SOCKETS, 0, 0);        
		mbusDumpStr0(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "socketStartTCPConnection error calling platformBind function");        
		return FALSE;
	}   

	mbusDumpStr1(MBUS_TRACE_LEVEL_WARNING, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, 
				"mbus_tcpStartConnection connecting using socket 0x%x", pSession->lSocket); 

	/* Set socket mode to non-blocking */
	lAddrLen = 1;  
	nStatus = platformIoctl(pSession->lSocket, FIONBIO, &lAddrLen);  

	memset( ( char * ) &sCli_addr, 0, sizeof( sCli_addr ) );
	sCli_addr.sin_family = AF_INET;
	sCli_addr.sin_port = htons( MODBUS_PORT );
	platformGetInternetAddress(sCli_addr.sin_addr) = pSession->lClientIPAddr;

	/* Connect to the listening server */
	platformConnect( pSession->lSocket, ( struct sockaddr * )&sCli_addr, sizeof( sCli_addr ));

	return TRUE;

}

/*---------------------------------------------------------------------------
** mbus_tcpContinueConnection( )
**
** Check the progress of the TCP connection request
**---------------------------------------------------------------------------
*/

BOOL mbus_tcpContinueConnection( MBUS_SESSION* pSession, BOOL* bTCPConnectionSuccessfullyCompleted ) 
{
	INT32                nSockets;
	INT32                nfds;
	struct timeval       timeout;
	fd_set               writeSet, exceptionSet;
     
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	FD_ZERO( &writeSet );
	FD_ZERO( &exceptionSet );

	FD_SET( pSession->lSocket, &writeSet );
	FD_SET( pSession->lSocket, &exceptionSet );

	nfds = (INT32)(pSession->lSocket + 1);

	nSockets = platformSelect( nfds, NULL, &writeSet, &exceptionSet, &timeout );
  
	if ( FD_ISSET( pSession->lSocket, &exceptionSet ) )
		return FALSE;

	*bTCPConnectionSuccessfullyCompleted = FD_ISSET( pSession->lSocket, &writeSet );

	return TRUE;
}
#endif /* SYNCHRONOUS_CONNECTION */

/*---------------------------------------------------------------------------
** mbus_tcpRecv( )
**
** Check if we received some data on one of our TCP sockets.
** If yes, read it and return the session index indicating to the main scanner 
** task that we got new TCP data for that session.
**---------------------------------------------------------------------------
*/
INT32 mbus_tcpRecv(MBUS_SESSION* pSession)
{
	INT32	lBytesReceived = 0,
			ltmpBytesReceived = 0,
			lBytesRequested,
			ulSockError;
	mbus_MbapHeader* pHdr = (mbus_MbapHeader*)gmbusMessageBuffer;
	UINT32      lTick;

	/*
	** Note that this function does not assume disabling of the Nagle algorithm
	** on the part of the Client.  It is prepared to read fragments of a Client 
	** request on each successive pass until it gets a complete request.
	*/

	if ( pSession->lSocket == (SOCKET)INVALID_SOCKET )
	   return -1;
 
	/* Check for any partial packets received with the previous tick read */
	if ( pSession->pPartialRecvPacket != NULL )
	{
		lTick = gmbusTickCount;
		if ( IS_TICK_GREATER(lTick, pSession->lPartialRecvPacketTimeoutTick) )
		{
			/* Partial packet is old and is being discarded */
			mbus_free(&pSession->pPartialRecvPacket, &pSession->iPartialRecvPacketSize );
		}
		else
		{
			lBytesReceived = pSession->iPartialRecvPacketSize;
			memcpy( gmbusMessageBuffer, pSession->pPartialRecvPacket, lBytesReceived );

			/* In the old buffer bytes are in host order. Put them back into network order. Meaningful only for big-endian platforms. */
			mbus_TcpAdjustHeader((mbus_MbapHeader*)gmbusMessageBuffer);
		}
	}

	/* 
	** If we don't already have a complete MBAP header, try to get the rest 
	** of it.
	*/
	while (lBytesReceived < MBUS_HEADER_SIZE)
	{
		/* Try to read the data. All sockets are non-blocking, so we'll get the result right away */
		ltmpBytesReceived = platformRecv( pSession->lSocket, (char*)( pHdr + lBytesReceived ),
														MBUS_HEADER_SIZE - lBytesReceived, 0 );

		if( ltmpBytesReceived <= 0 )    
		{
			ulSockError = platformGetLastSocketError( pSession->lSocket );    /* Error occured - check what caused the error*/            

			if( ltmpBytesReceived < 0 && ulSockError == EWOULDBLOCK) /* Nothing more to read for now */
			{
				if ( lBytesReceived && lBytesReceived > pSession->iPartialRecvPacketSize )
				{
					/* Got partial packet - save it for the next tick read */
					mbus_realloc(&pSession->pPartialRecvPacket, 
						&pSession->iPartialRecvPacketSize, gmbusMessageBuffer, (UINT16)lBytesReceived );
					pSession->lPartialRecvPacketTimeoutTick = gmbusTickCount + MBUS_PARTIAL_TCP_PACKET_TIMEOUT;
				}                
				return -1;            /* Nothing to read */                
			}
			else 
			{
				/* Connection has failed */
				gmbus_counters.usBusErrorCounter++;
				mbusDumpStr3(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
						"mbus_tcpRecv got error %d reading MBap header, socket = 0x%x, ltmpBytesReceived = %d", ulSockError, pSession->lSocket, ltmpBytesReceived);
				mbus_sessionRemove(pSession, TRUE);            
				return -1;
			}
		}

		lBytesReceived += ltmpBytesReceived;    /* Count the data received */
	}

	if ( pSession->pPartialRecvPacket == NULL )
	{
		/* If we got here we have at least the encapsulation header. Now get the packet size from the 
		   header and read the rest of the packet from the socket */
		mbus_TcpAdjustHeader(pHdr);
	}

	lBytesRequested = pHdr->Length + MBUS_HEADER_SIZE-1;

	while ( lBytesReceived < lBytesRequested )    /* Loop until we read the whole packet */
	{
		/* Read the data */
		ltmpBytesReceived = platformRecv( pSession->lSocket, (char*)&gmbusMessageBuffer[lBytesReceived], lBytesRequested - lBytesReceived, 0 );

		if( ltmpBytesReceived <= 0 )    
		{
			ulSockError = platformGetLastSocketError(pSession->lSocket);    /* Error occured - check what caused the error*/            

			if( ulSockError == EWOULDBLOCK )
			{
				if ( lBytesReceived && (lBytesReceived > pSession->iPartialRecvPacketSize))
				{
					/* Got partial packet - save it for the next tick read */
					mbus_realloc( &pSession->pPartialRecvPacket, 
						&pSession->iPartialRecvPacketSize, gmbusMessageBuffer, (UINT16)lBytesReceived );
					pSession->lPartialRecvPacketTimeoutTick = gmbusTickCount + MBUS_PARTIAL_TCP_PACKET_TIMEOUT;
				}

				return -1;            /* Nothing to read */                
			}
			else
			{
				gmbus_counters.usBusErrorCounter++;
				mbusDumpStr1(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
							"mbus_tcpRecv got error %d", ulSockError);    /* Connection has failed */
				mbus_sessionRemove(pSession, TRUE);            
				return -1;
			}
		}

		lBytesReceived += ltmpBytesReceived;
	}

	/* If we started with the partial packet - discard it now */   
	if ( pSession->pPartialRecvPacket != NULL )
		mbus_free( &pSession->pPartialRecvPacket, &pSession->iPartialRecvPacketSize );

	/* Ignore commands with too many bytes of data */
	if ( lBytesRequested > MBUS_HEADER_SIZE + MBUS_MAX_PDU_LEN + MBUS_ADU_OVERHEAD )
	{
		gmbus_counters.usBusErrorCounter++;
		mbusDumpStr3(MBUS_TRACE_LEVEL_WARNING, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
					"mbus_tcpRecv received a command with too many bytes: ERR - received %d bytes on socket 0x%x for session %d", lBytesRequested, pSession->lSocket, pSession->nSessionId);
		return -1;
	}

	mbusDumpStr3(MBUS_TRACE_LEVEL_DATA, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"mbus_tcpRecv received %d bytes on socket 0x%x for session %d", lBytesReceived, pSession->lSocket, pSession->nSessionId);
	mbusDumpBuf(MBUS_TRACE_LEVEL_DATA, MBUS_TRACE_TYPE_DATA_IN, pSession->lClientIPAddr,
		pSession->lHostIPAddr, (char*)gmbusMessageBuffer, lBytesReceived);

	/* If we have the full message, update the diagnostic counters */
	gmbus_counters.usSlaveMsgCounter++;

	return lBytesReceived;
}

/*---------------------------------------------------------------------------
** mbus_tcpSendPartial( )
**
** Send partial TCP packet
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/

BOOL mbus_tcpSendPartial(MBUS_SESSION* pSession)
{
    INT32 len, lenSent;
    
    len = pSession->iPartialSendPacketSize;            
    memcpy(gmbusMessageBuffer, pSession->pPartialSendPacket, len);       

    mbus_free( &pSession->pPartialSendPacket, &pSession->iPartialSendPacketSize );

    mbusDumpStr2(MBUS_TRACE_LEVEL_DATA, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, 
			"mbus_tcpSendPartial about to send partial %d bytes to socket 0x%x", len, pSession->lSocket);
    mbusDumpBuf(MBUS_TRACE_LEVEL_DATA, MBUS_TRACE_TYPE_DATA_OUT, pSession->lHostIPAddr, 
        pSession->lClientIPAddr, (char*)gmbusMessageBuffer, len);

    return mbus_tcpSend( pSession, len, &lenSent);
}


/*---------------------------------------------------------------------------
** mbus_tcpSend( )
**
** Send TCP packet
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/

BOOL mbus_tcpSend(MBUS_SESSION* pSession, INT32 lLen, INT32* plenSent)
{   
	INT32        lSent = 0;
	INT32        lSockError;

	if ( pSession->lSocket == (SOCKET)INVALID_SOCKET )
	{
		(*plenSent) = ERROR_STATUS;
		return TRUE;
	}

	if ( lLen > MBUS_MAX_PACKET_SIZE )    /* The length is invalid */
	{
		mbusDumpStr0(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, 
				"mbus_tcpSend data is corrupted");
		(*plenSent) = ERROR_STATUS;
		return TRUE;
	}

	/* Send TCP data */
	lSent = platformSend( pSession->lSocket, gmbusMessageBuffer, lLen, 0 );

	if( lSent <= 0 )    
	{
		lSockError = platformGetLastSocketError(pSession->lSocket);

		if( lSent < 0 && lSockError != EWOULDBLOCK )    /* session has been broken */
		{
			mbusDumpStr1(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, 
				"mbus_tcpSend got error %d", lSockError);
			mbus_sessionRemove( pSession, TRUE );            
			(*plenSent) = ERROR_STATUS;
			return FALSE;
		} 

		lSent = 0;
	}

	if ( lSent < lLen )
	{
		pSession->iPartialSendPacketSize = (UINT16)(lLen - lSent);
		pSession->pPartialSendPacket = mbus_malloc( &gmbusMessageBuffer[lSent], pSession->iPartialSendPacketSize );                        
	}

	if ( lSent )
	{
		mbusDumpStr2(MBUS_TRACE_LEVEL_DATA, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, 
			"mbus_tcpSend sent %d bytes to socket 0x%x", lSent, pSession->lSocket);
		mbusDumpBuf(MBUS_TRACE_LEVEL_DATA, MBUS_TRACE_TYPE_DATA_OUT, pSession->lHostIPAddr, 
			pSession->lClientIPAddr, (char*)gmbusMessageBuffer, lLen);
	}

	/* Set the number of bytes sent */
	(*plenSent) = lSent;

	/* reset the session timeout */
	mbus_sessionResetTimeout(pSession);
	return TRUE;
}

void mbus_TcpAdjustHeader(mbus_MbapHeader* pHdr)
{
	pHdr->TcpTransId = _BE_SWAPS_(pHdr->TcpTransId);
	pHdr->PrtocolId = _BE_SWAPS_(pHdr->PrtocolId);
	pHdr->Length = _BE_SWAPS_(pHdr->Length);
}

#endif
