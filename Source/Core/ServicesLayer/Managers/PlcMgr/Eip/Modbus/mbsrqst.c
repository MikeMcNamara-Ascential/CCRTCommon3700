/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSRQST.C
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Request array used for scheduling Modbus activity
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

MBUS_REQUEST	gmbusRequests[MAX_MBUS_REQUESTS];        /* Requests array */
INT32			gnmbusIndex;                        /* Index to be assigned to the next request */
UINT16			giTransIDIndex;
MBUS_REQUEST*	gpnmbusRequests;

UINT8 gmbusMessageBuffer[MBUS_MAX_PACKET_SIZE];   /* Buffer storing incoming and outgoing packets */

void  mbus_requestInitialize( MBUS_REQUEST* pRequest );
INT32 mbus_requestInitFromRequest( MBUS_REQUEST* pRequest, MBSRequest* pModbusRequest );
MBUS_REQUEST* mbus_requestNewOutgoing(BOOL bDeleteOnSend, UINT32 lHostIPAddr, BOOL bIO);
void mbus_requestTCPSend(MBUS_REQUEST* pRequest, MBUS_SESSION* pSession);
void mbus_requestSerialSend(MBUS_REQUEST* pRequest);
void mbus_requestResponseReceived( MBUS_REQUEST* pRequest, UINT32 nResponseEvent);

/*---------------------------------------------------------------------------
** mbus_requestInit( )
**
** Initialize request array
**---------------------------------------------------------------------------
*/

void mbus_requestInit()
{
    gpnmbusRequests = gmbusRequests;        
    gnmbusIndex = MBUS_REQUEST_INDEX_BASE;
	giTransIDIndex = MBUS_TRANSACTION_ID_BASE;
}

/*---------------------------------------------------------------------------
** mbus_requestInitialize( )
**
** Initialize a request
**---------------------------------------------------------------------------
*/

void mbus_requestInitialize( MBUS_REQUEST* pRequest )
{        
    memset( pRequest, 0, sizeof(MBUS_REQUEST) );

    pRequest->nIndex = MBUS_INVALID_REQUEST_INDEX;
	pRequest->nSessionId = MBUS_INVALID_SESSION_INDEX;
    pRequest->lIPAddress = INADDR_NONE;
    pRequest->lHostIPAddr = INADDR_NONE;        
}

/*---------------------------------------------------------------------------
** mbus_requestNewOutgoing( )
**
** Allocate and initialize new request in the array for outgoing requests
**---------------------------------------------------------------------------
*/
MBUS_REQUEST* mbus_requestNewOutgoing(BOOL bDeleteOnSend, UINT32 lHostIPAddr, BOOL bIO)
{
	MBUS_REQUEST* pRequest;

	if ( gpnmbusRequests >= &gmbusRequests[MAX_MBUS_REQUESTS] )    
		return NULL;            /* Request limit has been reached */

	pRequest = gpnmbusRequests;
	gpnmbusRequests++;

	mbus_requestInitialize( pRequest );

    pRequest->nIndex = gnmbusIndex++;
	pRequest->bDeleteOnSend = bDeleteOnSend;
	pRequest->lHostIPAddr = lHostIPAddr;
	pRequest->bIO = bIO;

	/* Wrap the unique request identifier if need to */
	if ( gnmbusIndex == MBUS_INDEX_END )
		gnmbusIndex = MBUS_REQUEST_INDEX_BASE;

	return pRequest;
}

/*---------------------------------------------------------------------------
** mbus_requestInitFromRequest( )
**
** Initialize parameters from MBSRequest structure. 
** Return 0 in case of success, or one of the error codes.
**---------------------------------------------------------------------------
*/
INT32 mbus_requestInitFromRequest( MBUS_REQUEST* pRequest, MBSRequest* pModbusRequest )
{
    if ( !pModbusRequest->lMessageTimeout )
        pModbusRequest->lMessageTimeout = MBUS_DEFAULT_REQUEST_TIMEOUT;

	pRequest->lRequestTimeoutTick = gmbusTickCount + pModbusRequest->lMessageTimeout;

	pRequest->cfg.bFunction = pModbusRequest->bFunction;
	pRequest->cfg.pData = mbus_malloc( pModbusRequest->requestData, pModbusRequest->iDataSize );
	pRequest->cfg.iDataSize = pModbusRequest->iDataSize;
	pRequest->bUnitID = pModbusRequest->bUnitID;

	pRequest->TcpTransID = giTransIDIndex++;

	if ( pRequest->cfg.iDataSize && pRequest->cfg.pData == NULL )         
		return NM_MBUS_ERROR_OUT_OF_MEMORY;
        
	return 0;
}

/*---------------------------------------------------------------------------
** mbus_requestNewMessage( )
**
** Queue new Modbus request message. 
** Return the request index or one of the error codes.
**---------------------------------------------------------------------------
*/
INT32 mbus_requestNewMessage(MBSRequest* pModbusRequest, BOOL bIO)
{
	MBUS_REQUEST* pRequest;

	/* Schedule new Modbus request */
	pRequest = mbus_requestNewOutgoing(FALSE, pModbusRequest->lHostAddress, bIO);
 
	if ( pRequest == NULL )
	{
		/* No more space for requests */
		mbus_notifyEvent( NM_MBUS_PENDING_REQUESTS_LIMIT_REACHED, 0, 0 );
		return NM_MBUS_PENDING_REQUESTS_LIMIT_REACHED;        
	}

	if ( mbus_requestInitFromRequest( pRequest, pModbusRequest ) == NM_MBUS_ERROR_OUT_OF_MEMORY )
	{
		mbus_requestRemove( pRequest );
		mbus_notifyEvent( NM_MBUS_ERROR_OUT_OF_MEMORY, 0, 0 );
		return NM_MBUS_ERROR_OUT_OF_MEMORY;
	}

#ifdef ET_IP_MODBUS_SERIAL
	if (pModbusRequest->bModbusMsgType == MBUS_REQUESTTYPE_TCP)
	{
#endif
		pRequest->lIPAddress = platformAddrFromPath( (const char*)pModbusRequest->networkPath, (INT32)strlen((const char*)pModbusRequest->networkPath) );
		if ( pRequest->lIPAddress == (UINT32)ERROR_STATUS )
		{
			mbus_requestRemove( pRequest );
			return NM_MBUS_REQUEST_FAILED_INVALID_NETWORK_PATH;
		}
#ifdef ET_IP_MODBUS_SERIAL
	}
	else if (pModbusRequest->bModbusMsgType == MBUS_REQUESTTYPE_SERIAL)
	{
		pRequest->hPort = mbus_rtuGetPortHandle(pModbusRequest->lHostAddress);
		if (pRequest->hPort == NULL)
		{
			mbus_requestRemove( pRequest );
			return NM_MBUS_ERROR_INVALID_HOST;
		}
	}
#endif

	pRequest->nState = MBUS_REQUEST_LOGGED;
	return pRequest->nIndex;
}

/*---------------------------------------------------------------------------
** mbus_requestRemove( )
**
** Remove the request from the array
**---------------------------------------------------------------------------
*/

void mbus_requestRemove(MBUS_REQUEST* pRequest)
{
    MBUS_REQUEST* pLoopRequest;

	mbus_free( &pRequest->cfg.pData, &pRequest->cfg.iDataSize );

    gpnmbusRequests--;
        
    /* Shift the requests with the bigger index to fill in the void */
    for( pLoopRequest = pRequest; pLoopRequest < gpnmbusRequests; pLoopRequest++ )                
        memcpy( pLoopRequest, pLoopRequest+1, sizeof(MBUS_REQUEST) );    
}

/*---------------------------------------------------------------------------
** mbus_requestRemoveAll( )
**
** Remove all requests from the array
**---------------------------------------------------------------------------
*/

void mbus_requestRemoveAll(void)
{
    MBUS_REQUEST* pRequest;
        
    for( pRequest = gpnmbusRequests-1; pRequest >= gmbusRequests; pRequest-- )
		mbus_free( &pRequest->cfg.pData, &pRequest->cfg.iDataSize );

    gpnmbusRequests = gmbusRequests;
}

/*---------------------------------------------------------------------------
** mbus_requestResponseReceived( )
**
** Received the response
**---------------------------------------------------------------------------
*/

void mbus_requestResponseReceived( MBUS_REQUEST* pRequest, UINT32 nResponseEvent)
{
	/* Set the timeout for the client application to get the results */
	pRequest->lRequestTimeoutTick = gmbusTickCount + MBUS_DEFAULT_REQUEST_TIMEOUT;
	pRequest->nState = MBUS_REQUEST_RESPONSE_RECEIVED;

	mbusDumpStr2(MBUS_TRACE_LEVEL_NOTICE, MBUS_TRACE_TYPE_MODBUS, pRequest->lHostIPAddr, pRequest->lIPAddress,
		"Response received to request %d with Error = 0x%x",
		pRequest->nIndex, pRequest->bGeneralError);

	mbus_notifyEvent(nResponseEvent, pRequest->nIndex, 0);    
}

/*---------------------------------------------------------------------------
** mbus_requestGetByRequestId( )
**
** Find a request based on the request Id
**---------------------------------------------------------------------------
*/

MBUS_REQUEST* mbus_requestGetByRequestId( INT32 nRequestId )
{    
	MBUS_REQUEST* pRequest;        

	/* Find our request */
	for( pRequest = gmbusRequests; pRequest < gpnmbusRequests; pRequest++ )
	{
		if ( pRequest->nIndex == nRequestId )
			return pRequest;
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** mbus_requestTCPSend( )
**
** Send the request on Modbus/TCP
**---------------------------------------------------------------------------
*/
void mbus_requestTCPSend(MBUS_REQUEST* pRequest, MBUS_SESSION* pSession)
{
	UINT16 iMessageSize;
	INT32 lSent, lSockError;
	UINT8* pData;
	UINT16 iLength;

	if (pRequest->bGeneralError != 0)
	{
		gmbusMessageBuffer[MBUS_HEADER_SIZE] = (UINT8)(pRequest->cfg.bFunction | 0x80);
		gmbusMessageBuffer[MBUS_HEADER_SIZE+1] = pRequest->bGeneralError;
		iMessageSize = MBUS_HEADER_SIZE+2;
		iLength = 3;

		/* update counter if this is an exception response */
		gmbus_counters.usExceptionRespCounter++;
	}
	else
	{
		gmbusMessageBuffer[MBUS_HEADER_SIZE] = pRequest->cfg.bFunction;

		/* copy in the Modbus message to send it all out at once */
		memcpy(gmbusMessageBuffer+MBUS_HEADER_SIZE+1, pRequest->cfg.pData, pRequest->cfg.iDataSize);
		iMessageSize = (UINT16)(MBUS_HEADER_SIZE+pRequest->cfg.iDataSize+1);
		iLength = (UINT16)(pRequest->cfg.iDataSize+2);
	}

	pData = gmbusMessageBuffer;
	UINT16_BE_SET(pData, pRequest->TcpTransID);
	pData += sizeof(UINT16);
	UINT16_BE_SET(pData, MBUS_MODBUS_PROTOCOL);
	pData += sizeof(UINT16);
	UINT16_BE_SET(pData, iLength);
	pData += sizeof(UINT16);
	*pData = pRequest->bUnitID;

	lSent = platformSend(pSession->lSocket, gmbusMessageBuffer, iMessageSize, 0);
	if( lSent <= 0 )
	{
		lSockError = platformGetLastSocketError(pSession->lSocket);

		if( lSent < 0 && lSockError != EWOULDBLOCK )    /* session has been broken */
		{
			mbusDumpStr1(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_MODBUS, pRequest->lHostIPAddr, pRequest->lIPAddress,
						"platformSend got error %d", lSockError);
			mbus_sessionRemove(pSession, TRUE);
			return;
		}

		lSent = 0;
	}

	if(lSent != iMessageSize)
	{
		mbus_sessionRemove(pSession, TRUE);
		gmbus_counters.usBusErrorCounter++;
		return;
	}
	else
	{
        mbusDumpStr3(MBUS_TRACE_LEVEL_DATA, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, 
			"mbus_requestTCPSend sent %d bytes to socket 0x%x for session %d", lSent, pSession->lSocket, pSession->nSessionId);
        mbusDumpBuf(MBUS_TRACE_LEVEL_DATA, MBUS_TRACE_TYPE_DATA_OUT, pSession->lHostIPAddr, 
            pSession->lClientIPAddr, (char*)gmbusMessageBuffer, lSent);
	}

	/* Update the diagnostic counter */
	gmbus_counters.usBusMsgCounter++;

	if (pRequest->bUnitID == 0)
	{
		/* Broadcast requests don't generate a response */
		mbus_requestRemove( pRequest ); 
	}
	else
	{
		pRequest->nState = MBUS_REQUEST_WAITING_FOR_RESPONSE;
	}
	
	return;
}   

#ifdef ET_IP_MODBUS_SERIAL
/*---------------------------------------------------------------------------
** mbus_requestSerialSend( )
**
** Send the request on Modbus Serial
**---------------------------------------------------------------------------
*/
void mbus_requestSerialSend(MBUS_REQUEST* pRequest)
{
	UINT16 iLength, iMessageSize, iCRC;
	INT32 lSent;

	gmbusMessageBuffer[0] = pRequest->bUnitID;

	if (pRequest->bGeneralError != 0)
	{
		gmbusMessageBuffer[1] = (UINT8)(pRequest->cfg.bFunction | 0x80);
		gmbusMessageBuffer[2] = pRequest->bGeneralError;
		iMessageSize = iLength = 3;

		/* update counter if this is an exception response */
		gmbus_counters.usExceptionRespCounter++;
	}
	else
	{
		gmbusMessageBuffer[1] = pRequest->cfg.bFunction;

		/* copy in the Modbus message to send it all out at once */
		memcpy(gmbusMessageBuffer+2, pRequest->cfg.pData, pRequest->cfg.iDataSize);
		iMessageSize = iLength = (UINT16)(pRequest->cfg.iDataSize+2);
	}

	iCRC = mbs_rtuCalcCRC(gmbusMessageBuffer, iMessageSize);
	UINT16_BE_SET(&gmbusMessageBuffer[iMessageSize], iCRC);
	iLength += 2;

	lSent = platformSerialSendMessage(pRequest->hPort, gmbusMessageBuffer, iLength);

	if( lSent <= 0 )
	{
		mbusDumpStr1(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_MODBUS, pRequest->lHostIPAddr, pRequest->lIPAddress,
					"platformSerialSendMessage got error %d", lSockError);
		return;
	}

	if(lSent != iLength)
	{
		gmbus_counters.usBusErrorCounter++;
		return;
	}
	else
	{
        mbusDumpStr3(MBUS_TRACE_LEVEL_DATA, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, 
			"mbus_requestSerialSend sent %d bytes to socket 0x%x for session %d", lSent, pSession->lSocket, pSession->nSessionId);
        mbusDumpBuf(MBUS_TRACE_LEVEL_DATA, MBUS_TRACE_TYPE_DATA_OUT, pSession->lHostIPAddr, 
            pSession->lClientIPAddr, (char*)gmbusMessageBuffer, lSent);
	}

	/* Update the diagnostic counter */
	gmbus_counters.usBusMsgCounter++;

	if (pRequest->bUnitID == 0)
	{
		/* Broadcast requests don't generate a response */
		mbus_requestRemove( pRequest ); 
	}
	else
	{
		pRequest->nState = MBUS_REQUEST_WAITING_FOR_RESPONSE;
	}

	return;
}
#endif /* ET_IP_MODBUS_SERIAL */

/*---------------------------------------------------------------------------
** mbus_requestLogTCPResponse( )
**
** Log response received for the previously sent ModbusTCP request
**---------------------------------------------------------------------------
*/
void mbus_requestLogTCPResponse(MBUS_REQUEST* pRequest)
{
	mbus_MbapHeader* pHdr = (mbus_MbapHeader*)gmbusMessageBuffer;
	UINT8* pData = gmbusMessageBuffer + MBUS_HEADER_SIZE;

	/* Store the response in the request object */
	if (*pData & MBUS_EXCEPTION_RESPONSE_MASK)
	{
		pRequest->bGeneralError = *(pData+1);
		mbus_free( &pRequest->cfg.pData, &pRequest->cfg.iDataSize );
	}
	else
	{
		mbus_realloc(&pRequest->cfg.pData, &pRequest->cfg.iDataSize,
					 (UINT8*)(pData+1), (UINT16)(pHdr->Length-2));

		if ( pRequest->cfg.iDataSize && pRequest->cfg.pData == NULL )
		{
			mbus_notifyEvent( NM_MBUS_ERROR_OUT_OF_MEMORY, 0, 0 );
			mbus_requestRemove( pRequest );    /* Out of memory */        
			return;
		}
	}

	mbus_requestResponseReceived( pRequest, NM_MBUS_REQUEST_RESPONSE_RECEIVED);  
}

/*---------------------------------------------------------------------------
** mbus_requestLogSerialResponse( )
**
** Log response received for the previously sent serial request
**---------------------------------------------------------------------------
*/
void mbus_requestLogSerialResponse(MBUS_REQUEST* pRequest, UINT16 iMsgSize)
{
	UINT8* pData = &gmbusMessageBuffer[1];

	/* Store the response in the request object */
	if (*pData & MBUS_EXCEPTION_RESPONSE_MASK)
	{
		pRequest->bGeneralError = *(pData+1);
		mbus_free( &pRequest->cfg.pData, &pRequest->cfg.iDataSize );
	}
	else
	{
		mbus_realloc(&pRequest->cfg.pData, &pRequest->cfg.iDataSize,
					 (UINT8*)(pData+1), (UINT16)(iMsgSize-2));

		if ( pRequest->cfg.iDataSize && pRequest->cfg.pData == NULL )
		{
			mbus_notifyEvent( NM_MBUS_ERROR_OUT_OF_MEMORY, 0, 0 );
			mbus_requestRemove( pRequest );    /* Out of memory */        
			return;
		}
	}

	mbus_requestResponseReceived( pRequest, NM_MBUS_REQUEST_RESPONSE_RECEIVED);

}


/*---------------------------------------------------------------------------
** mbus_requestService( )
**
** Service a particular request
**---------------------------------------------------------------------------
*/

void mbus_requestService(MBUS_REQUEST* pRequest)
{    
	UINT32 lTick = gmbusTickCount;
	MBUS_SESSION *pSession, *pSession2;
#ifdef ET_IP_MODBUS_SERIAL
	MBUS_SL_SESSION *pSLSession;
#endif
	
	switch( pRequest->nState )
	{
		case MBUS_REQUEST_LOGGED:
		{
#ifdef ET_IP_MODBUS_SERIAL
			if (pRequest->hPort == NULL)	/* A ModbusTCP request */
			{
#endif
				/* Check if the session with this server has already been established */
				pSession = mbus_sessionFindOutgoing( pRequest->lIPAddress, pRequest->lHostIPAddr, (UINT8)(pRequest->bIO ? 1 : 0));

				if ( pSession == NULL )
				{
					/* Check for the opposite type of session */
					pSession = mbus_sessionFindOutgoing( pRequest->lIPAddress, pRequest->lHostIPAddr, (UINT8)(pRequest->bIO ? 0 : 1));

					if ((pSession == NULL) || (!pSession->bOnlySingleSessionSupported))
					{
						pSession2 = mbus_sessionNew( pRequest->lIPAddress, pRequest->lHostIPAddr, FALSE, pRequest->bIO);

						if (pSession2 != NULL)
							pSession = pSession2;
						else
						{
							/* If there is an available session use it because the maximum number of open sessions has been reached */
							mbus_notifyEvent(NM_MBUS_SESSION_COUNT_LIMIT_REACHED, 0, 0);

							if (pSession == NULL)
							{
								/* No sessions available */
								mbus_requestRemove(pRequest);
								return;
							}
						}   
					}
				}

				pRequest->nSessionId = pSession->nSessionId;
					
				if ( pSession->lState == MBUS_SESSION_ESTABLISHED )
				{
					mbus_requestTCPSend( pRequest, pSession );
					return;
				}
        
				/* Session has not been successfully established yet */
				pRequest->nState = MBUS_REQUEST_WAITING_FOR_SESSION;
#ifdef ET_IP_MODBUS_SERIAL
			}
			else
			{
				pSLSession = mbus_rtuFindOutgoing(pRequest->lIPAddress, pRequest->lHostIPAddr);
				if (pSLSession == NULL )
				{
					pSLSession = mbus_rtuSessionNew(pRequest->lIPAddress, pRequest->lHostIPAddr, FALSE);
					if (pSLSession == NULL)
					{
						/* No sessions available */
						mbus_requestRemove(pRequest);
						return;
					}
				}

				pRequest->nSessionId = pSLSession->nSessionId;

				mbus_requestSerialSend( pRequest );
			}
#endif
		}
		break;

		case MBUS_REQUEST_WAITING_FOR_SESSION:
		{
			pSession = mbus_sessionGetBySessionId( pRequest->nSessionId );

			if ( pSession == NULL || IS_TICK_GREATER( lTick, pRequest->lRequestTimeoutTick ) )
			{
				mbus_notifyEvent( NM_MBUS_REQUEST_FAILED_INVALID_NETWORK_PATH, pRequest->nIndex, 0 );
				mbus_requestRemove( pRequest );
				break;
			}

			if ( pSession->lState == MBUS_SESSION_ESTABLISHED )
			{
				mbus_requestTCPSend( pRequest, pSession );
				return;
			}
		}
		break;
    
		case MBUS_REQUEST_WAITING_FOR_RESPONSE:
		{
			if ( IS_TICK_GREATER( lTick, pRequest->lRequestTimeoutTick ) )
			{                    
				mbus_notifyEvent(NM_MBUS_REQUEST_TIMED_OUT, pRequest->nIndex, 0);

				/* Check if need to drop the session that went offline */
				if ( mbus_sessionIsIdle( pRequest->nSessionId, pRequest->nIndex ) )
				{
#ifdef ET_IP_MODBUS_SERIAL
					if (pRequest->hPort == NULL)	/* A ModbusTCP request */
					{
#endif
						pSession = mbus_sessionGetBySessionId( pRequest->nSessionId );
						if (pSession != NULL)
							mbus_sessionRemove(pSession, TRUE );
#ifdef ET_IP_MODBUS_SERIAL
					}
					else
					{
						pSLSession = mbus_rtuSessionGetBySessionId( pRequest->nSessionId );
						if (pSLSession != NULL)
							mbus_rtuSessionRemove(pSLSession, TRUE );
					}
#endif
				}

				mbus_requestRemove(pRequest);                
			}
		}
		break;

		case MBUS_REQUEST_RESPONSE_RECEIVED:
		{
			/* Check for the client read timeout */
			if ( IS_TICK_GREATER( lTick, pRequest->lRequestTimeoutTick ) )
				mbus_requestRemove(pRequest);            
		}
		break;
	}
}

#endif /* ET_IP_MODBUS */
