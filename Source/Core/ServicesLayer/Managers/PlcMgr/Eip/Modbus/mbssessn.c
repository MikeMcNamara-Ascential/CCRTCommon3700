/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSSESSN.C
**
** COPYRIGHT (c) 2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Modbus Session collection implementation
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

BOOL mbus_sessionProcessMsg( MBUS_SESSION* pSession );
void mbus_sessionSwitchRequest( MBUS_SESSION* pSession );


MBUS_SESSION* gpnmbsSessions;						/* Number of the open sessions */
MBUS_SESSION  gmbsSessions[MBUS_MAX_SESSIONS];	/* Session objects array */
INT32    gnmbusSessionIndex;							/* Id to be assigned to the next opened session */


/*---------------------------------------------------------------------------
** mbus_sessionInit( )
**
** Initialize the session array
**---------------------------------------------------------------------------
*/

void mbus_sessionInit(void)
{
	gpnmbsSessions = gmbsSessions; 
	gnmbusSessionIndex = 1;
}

/*---------------------------------------------------------------------------
** mbus_sessionInitialize( )
**
** Initialize the session object
**---------------------------------------------------------------------------
*/

void mbus_sessionInitialize( MBUS_SESSION* pSession )
{    
	memset( pSession, 0, sizeof(MBUS_SESSION) );

	pSession->lSocket = INVALID_SOCKET;
	pSession->nSessionId = MBUS_INVALID_SESSION_INDEX;

	pSession->lHostIPAddr = INADDR_NONE;
	pSession->lClientIPAddr = INADDR_NONE;
}


/*---------------------------------------------------------------------------
** mbus_sessionNew( )
**
** Allocate and initialize a new session object in the array
**---------------------------------------------------------------------------
*/
MBUS_SESSION* mbus_sessionNew( UINT32 lIPAddress, UINT32 lHostIPAddr, BOOL bIncoming, BOOL bIO)
{
	MBUS_SESSION* pSession;
	UINT32 lTick = gmbusTickCount;
	char szDirection[9], szIO[4];

	if ( gpnmbsSessions >= &gmbsSessions[MBUS_MAX_SESSIONS] )    /* Limit of the open sessions reached */
		return NULL;

	pSession = gpnmbsSessions++;

	mbus_sessionInitialize( pSession );

	/* Make sure we don't have a very old session that uses our Id */
	while (mbus_sessionGetBySessionId(gnmbusSessionIndex) != NULL)
	{		
		if ( ++gnmbusSessionIndex == MBUS_INDEX_END )          /* Wrap the unique session identifier if need to */
			gnmbusSessionIndex = 1;
	}

	pSession->nSessionId = gnmbusSessionIndex++;
	if ( gnmbusSessionIndex == MBUS_INDEX_END )                /* Wrap the unique session identifier if need to */
		gnmbusSessionIndex = 1;

	if ( bIncoming )
	{
		pSession->lWaitingForOpenSessionTimeout = lTick + MBUS_INACTIVITY_TIMEOUT;
		pSession->lState = MBUS_SESSION_ESTABLISHED;
		strcpy(szDirection, "incoming");
	}
	else
	{
		pSession->lWaitingForOpenSessionTimeout = lTick + MBUS_DEFAULT_REQUEST_TIMEOUT;        
		pSession->lClientIPAddr = lIPAddress;
		pSession->lState = MBUS_SESSION_LOGGED;
		strcpy(szDirection,"outgoing");
	}

	pSession->lHostIPAddr = lHostIPAddr;
	pSession->bIO = bIO;

	if (bIO)
	{
		strcpy(szIO,"I/O");
	}
	else
	{
		strcpy(szIO,"");
	}

	mbusDumpStr3(MBUS_TRACE_LEVEL_NOTICE, MBUS_TRACE_TYPE_SESSION, lHostIPAddr, lIPAddress,
				"Creating %s %s session %d", szDirection, szIO, pSession->nSessionId); 

	return pSession;
}

/*---------------------------------------------------------------------------
** mbus_sessionRemove( )
**
** Remove the session from the array
**---------------------------------------------------------------------------
*/
void mbus_sessionRemove( MBUS_SESSION* pSessionRemove, BOOL bRelogOutgoingRequests )
{
	MBUS_SESSION* pSession;
	MBUS_REQUEST* pRequest;
	INT32   nSessionId = pSessionRemove->nSessionId;

	mbusDumpStr1(MBUS_TRACE_LEVEL_NOTICE, MBUS_TRACE_TYPE_SESSION, pSessionRemove->lHostIPAddr, pSessionRemove->lClientIPAddr, 
			"mbus_sessionRemove %d", pSessionRemove->nSessionId);

	if ( gpnmbsSessions > gmbsSessions)
		gpnmbsSessions--;                                /* Adjust the total number of open sessions */

	if ( pSessionRemove->lSocket != INVALID_SOCKET )
	{
		platformCloseSocket(pSessionRemove->lSocket);    /* Close Winsock TCP socket */
		pSessionRemove->lSocket = INVALID_SOCKET;
	}

	if ( pSessionRemove->pPartialRecvPacket != NULL )
		mbus_free(&pSessionRemove->pPartialRecvPacket, &pSessionRemove->iPartialRecvPacketSize );

	if ( pSessionRemove->pPartialSendPacket != NULL )
		mbus_free( &pSessionRemove->pPartialSendPacket, &pSessionRemove->iPartialSendPacketSize );

	for( pSession = pSessionRemove; pSession < gpnmbsSessions; pSession++ )        /* Shift the sessions with the higher index to fill in the void */
		memcpy( pSession, pSession+1, sizeof(MBUS_SESSION) );    
        
	for( pRequest = (gpnmbusRequests - 1); pRequest >= gmbusRequests; pRequest-- )
	{
		if ( pRequest->nSessionId == nSessionId )
		{
			pRequest->nSessionId = MBUS_INVALID_SESSION_INDEX;

			if ( bRelogOutgoingRequests && pRequest->nState != MBUS_REQUEST_RESPONSE_RECEIVED )        /* Relog outstanding explicit requests if appropriate */ 
				pRequest->nState = MBUS_REQUEST_LOGGED;                    
		}
	}        
}


/*---------------------------------------------------------------------------
** mbus_sessionRemoveAll( )
**
** Close and remove all open sessions
**---------------------------------------------------------------------------
*/

void mbus_sessionRemoveAll(void)
{
	MBUS_SESSION* pSession;
        
	for( pSession = gpnmbsSessions - 1; pSession >= gmbsSessions; pSession-- )
		mbus_sessionRemove( pSession, FALSE );        

	gpnmbsSessions = gmbsSessions;
}


/*---------------------------------------------------------------------------
** mbus_sessionFindOutgoing( )
**
** Return the session index for the particular client IP address
**---------------------------------------------------------------------------
*/
MBUS_SESSION* mbus_sessionFindOutgoing(UINT32 lIPAddress, UINT32 lHostIPAddr, UINT8 bIOStatus)
{
	MBUS_SESSION* pSession;
        
	for( pSession = gmbsSessions; pSession < gpnmbsSessions; pSession++ )
	{
		if ((pSession->lClientIPAddr == lIPAddress) &&
			(pSession->lHostIPAddr == lHostIPAddr))
		{
			if ((bIOStatus == 2) || (pSession->bIO == bIOStatus))
				return pSession;
		}
	}

	return NULL;
}


/*---------------------------------------------------------------------------
** mbus_sessionIsIdle( )
**
** Return TRUE if there no active requests for the specified
** session. Otherwise return FALSE.
**---------------------------------------------------------------------------
*/

BOOL  mbus_sessionIsIdle(INT32 nSessionId, INT32 nRequestIdExclude )
{
	MBUS_REQUEST*    pRequest;

	if ( nRequestIdExclude != (INT32)MBUS_INVALID_REQUEST_INDEX )
	{
		for( pRequest = gpnmbusRequests-1; pRequest >= gmbusRequests; pRequest-- )
		{
			if ( pRequest->nSessionId == nSessionId && 
				 ( nRequestIdExclude == MBUS_INVALID_REQUEST_INDEX || pRequest->nIndex != nRequestIdExclude ) )
					return FALSE;
		}    
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** mbus_sessionResetTimeout( )
**
** Resets the timeout for the TCP connection for a session when there is
** activity.  If the timeout is reached, the session is removed
**---------------------------------------------------------------------------
*/
void mbus_sessionResetTimeout( MBUS_SESSION* pSession )
{
	pSession->lWaitingForOpenSessionTimeout = gmbusTickCount+MBUS_INACTIVITY_TIMEOUT;
}


/*---------------------------------------------------------------------------
** mbus_sessionGetBySessionId( )
**
** Returns session pointer based on the session id.
** Returns NULL if the session id was not found.
**---------------------------------------------------------------------------
*/
MBUS_SESSION* mbus_sessionGetBySessionId(INT32 nId)
{
    MBUS_SESSION* pSession;

	if ( nId == MBUS_INVALID_SESSION_INDEX )
		return NULL;
    
    for ( pSession = gmbsSessions; pSession < gpnmbsSessions; pSession++ ) 
    {
        if ( pSession->nSessionId == nId )             
            return pSession;                        
    }

    return NULL;
}

/*---------------------------------------------------------------------------
** mbus_sessionService( )
**
** Service a particular session
**---------------------------------------------------------------------------
*/
void mbus_sessionService( MBUS_SESSION* pSession )
{
#if !defined(SYNCHRONOUS_CONNECTION)    
	BOOL    bTCPConnectionSuccessfullyCompleted;
#endif
	BOOL    bHandleMoreData = TRUE;
	UINT32  lTick = gmbusTickCount;
    
	switch( pSession->lState )
	{

/* if there is a separate thread for establishing a new TCP session */
#if !defined(SYNCHRONOUS_CONNECTION)
		case MBUS_SESSION_LOGGED:
		{               
			if( !mbus_tcpStartConnection( pSession ) ) 
			{
				mbus_sessionSwitchRequest(pSession);
				mbus_sessionRemove( pSession, FALSE );
				break;
			}

			pSession->lState = MBUS_SESSION_WAITING_FOR_TCP_CONNECTION;
		}
		break;

		case MBUS_SESSION_WAITING_FOR_TCP_CONNECTION:
		{    
			if( !mbus_tcpContinueConnection( pSession, &bTCPConnectionSuccessfullyCompleted ) ) 
			{                
				mbus_sessionSwitchRequest(pSession);
				mbus_sessionRemove( pSession, FALSE );
				break;
			}

			if ( bTCPConnectionSuccessfullyCompleted )
				pSession->lState = MBUS_SESSION_ESTABLISHED;                        
			else if ( IS_TICK_GREATER( lTick, pSession->lWaitingForOpenSessionTimeout ) )
			{
				mbus_sessionSwitchRequest(pSession);
				mbus_sessionRemove( pSession, FALSE );
			}
		}
		break;

#else	/* if there is no separate thread for establishing a new TCP session */

		case MBUS_SESSION_LOGGED:            
		{    
			if ( IS_TICK_GREATER( lTick, pSession->lWaitingForOpenSessionTimeout ) )
			{
				mbus_sessionSwitchRequest(pSession);
				mbus_sessionRemove( pSession, FALSE );
			}
		}
		break;

		case MBUS_SESSION_WAITING_FOR_TCP_CONNECTION:
		{    
			if ( IS_TICK_GREATER( lTick, pSession->lWaitingForOpenSessionTimeout ) )
			{
				mbus_sessionSwitchRequest(pSession);
				mbus_sessionRemove( pSession, FALSE );
			}
		}
		break;

#endif

		case MBUS_SESSION_ESTABLISHED:
		{
			if ( pSession->iPartialSendPacketSize )
				bHandleMoreData = mbus_tcpSendPartial( pSession );

			while (bHandleMoreData)
			{
				if (mbus_tcpRecv( pSession ) <= 0 )
					break;

				bHandleMoreData = mbus_sessionProcessMsg( pSession );    /* Process TCP data */
				mbus_sessionResetTimeout(pSession);
			}

			if ( IS_TICK_GREATER( lTick, pSession->lWaitingForOpenSessionTimeout ) &&
				mbus_sessionIsIdle(pSession->nSessionId, MBUS_INVALID_REQUEST_INDEX))
			{
				/* session has timed out from inactivity */
				mbus_sessionRemove( pSession, FALSE );
			}
		}
		break;
	}
}

BOOL mbus_sessionProcessMsg( MBUS_SESSION* pSession )
{
	MBUS_REQUEST* pRequest;
	mbus_MbapHeader* pHdr = (mbus_MbapHeader*)gmbusMessageBuffer;

	/* this is a reply to a message, check the outstanding requests */
	for( pRequest = gmbusRequests; pRequest < gpnmbusRequests; pRequest++ )
	{
		if ((pSession->lClientIPAddr == pRequest->lIPAddress) &&
			(pHdr->TcpTransId == pRequest->TcpTransID))
		{
			mbus_requestLogTCPResponse(pRequest);
			break;
		}
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** mbus_sessionSwitchRequest( )
**
** If another session is open to the same IP address, use it and consider
** it the only session available to the device
**---------------------------------------------------------------------------
*/
void mbus_sessionSwitchRequest( MBUS_SESSION* pSession )
{
	MBUS_REQUEST* pRequest;
	MBUS_SESSION* pSessionSwitch = mbus_sessionFindOutgoing(pSession->lClientIPAddr, pSession->lHostIPAddr, 2);
	if (pSessionSwitch == NULL)
		return;

	for( pRequest = gmbusRequests; pRequest < gpnmbusRequests; pRequest++ )
	{
		if ((pRequest->nSessionId == pSession->nSessionId) &&
			(pRequest->nState == MBUS_REQUEST_WAITING_FOR_SESSION))
			pRequest->nSessionId = pSessionSwitch->nSessionId;
	}

	pSessionSwitch->bOnlySingleSessionSupported = TRUE;
}

#endif /* ET_IP_MODBUS */
