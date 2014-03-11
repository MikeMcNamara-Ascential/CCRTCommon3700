
/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPMBUS.C
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** This module contains the implementation of the Modbus library
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

PLATFORM_MUTEX_TYPE     ghmbusClientMutex;			/* Used to protect integrity of the client calls */
UINT32 gmbusTickCount;								/* Stored system tick count in msecs */

MBSCounters	gmbus_counters;							/* Tracks messages and error conditions */

UINT32* gpmbusHostIPAddr;							/* Number of host addresses */
UINT32  galmbusHostIPAddr[MBUS_MAX_HOST_ADDRESSES];		/* Host address array */

/*---------------------------------------------------------------------------
** modbusInit( )
**
** Initialize Modbus library
**---------------------------------------------------------------------------
*/
BOOL modbusInit(MBS_EventCallbackType* pCallback)
{
	gfnmbsLogEvent = pCallback;

	gpmbusHostIPAddr = galmbusHostIPAddr;

	/* setup the counters */
	memset(&gmbus_counters, 0, sizeof(MBSCounters));

#if defined(MBUS_TRACE_FILE_OUTPUT) || defined (MBUS_TRACE_DEBUGGER_OUTPUT)
    gmbusTraceFlag = MBUS_TRACE_LEVEL_ALL;
	gmbusTraceTypeFlag = MBUS_TRACE_TYPE_ALL;
#endif

#ifdef MBUS_TRACE_FILE_OUTPUT
	mbus_platformInitLogFile();
#endif

#ifdef MBUS_TRACE_SOCKET_OUTPUT
    mbusdbgInit();
#endif

#ifdef MBUS_STACK_MEMORY
    gpmbusMemoryPool = gmbusMemoryPool;        /* Mark memory pool as empty */
#endif

#ifdef ET_IP_MODBUS_SERIAL
	mbus_rtuInit();
#endif

    /* Make sure that any of the API calls running in the context of a client thread
       are not executed at the same time as another API call or the EML thread is executed */
    ghmbusClientMutex = platformInitMutex(_T("EtIPModbusClientMutex"));
	if(ghmbusClientMutex == NULL)
	{
		mbusDumpStr0(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_MSG, 0, 0, "Could not initialize mutex.  Stack is not initialized");
		return FALSE;
	}

	platformReleaseMutex(ghmbusClientMutex);

	/* Initialize event array */
	mbus_notifyInit();

	/* initialize sessions */
	mbus_sessionInit();

	/* initialize requests */
	mbus_requestInit();

	return TRUE;
}

/*---------------------------------------------------------------------------
** modbusRelease( )
**
** Stop Modbus functionality
**---------------------------------------------------------------------------
*/
void modbusRelease(void)
{
    mbus_requestRemoveAll();
	mbus_sessionRemoveAll();

#ifdef MBUS_TRACE_FILE_OUTPUT
	mbus_platformCloseLogFile();
#endif

#ifdef MBUS_TRACE_SOCKET_OUTPUT
    mbus_dbgCleanup();
#endif
}

/*---------------------------------------------------------------------------
** modbusService( )
**
** Executes one iteration of the Modbus Protocol stack.
** Function called at repeated intervals by the EtherNet/IP stack
** to provide the Modbus stack with a time slice in which to 
** execute.
**---------------------------------------------------------------------------
*/
void modbusService(UINT32 lTickCount)
{
	MBUS_REQUEST* pRequest;
	MBUS_SESSION* pSession;
#ifdef ET_IP_MODBUS_SERIAL
	mbsClaimedCOMPort* pCOMPort;
#endif

	platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);

	gmbusTickCount = lTickCount;

    /* Service all TCP sessions */
    for( pSession = gpnmbsSessions-1; pSession >= gmbsSessions; pSession-- )
        mbus_sessionService( pSession );
	
#ifdef ET_IP_MODBUS_SERIAL
	/* Service all COM ports */
	for (pCOMPort = galClaimedHostCOMPorts; pCOMPort < gpnClaimedHostCOMPorts; pCOMPort++)
		mbus_rtuService(pCOMPort->hPort);
#endif

    /* Service all requests */
    for( pRequest = gpnmbusRequests-1; pRequest >= gmbusRequests; pRequest-- )
        mbus_requestService( pRequest );

	platformReleaseMutex(ghmbusClientMutex);

	mbus_notifyService();
}

/*---------------------------------------------------------------------------
** modbusSendRequest( )
**
** Send Modbus message
** modbusGetResponse will be used to get the response.
** pRequest points to a structure containing the request parameters.
** bIO determines if its an I/O data request.  I/O data requests will be sent
** on a separate TCP connection (if available)
** Check the return value. If it's greater than or equal to MBUS_REQUEST_INDEX_BASE,
** then it is a request Id to be used when calling modbusGetResponse.
** If it is less than REQUEST_INDEX_BASE then it's one of the 
** errors listed above.
**---------------------------------------------------------------------------
*/
INT32 modbusSendRequest( MBSRequest* pRequest, BOOL bIO)
{
	INT32  nRequest;

	if ((pRequest->bModbusMsgType != MBUS_REQUESTTYPE_TCP) &&
		(pRequest->bModbusMsgType != MBUS_REQUESTTYPE_SERIAL))
	{
		return ERR_MBUS_INVALID_REQUESTTYPE;
	}

	platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);

#ifdef ET_IP_MODBUS_SERIAL
	if (pRequest->bModbusMsgType == MBUS_REQUESTTYPE_TCP)
	{
#endif
		/* If the user did not specify Request Host IPAddress - use the first one */
		if (pRequest->lHostAddress == INADDR_NONE )
			pRequest->lHostAddress = galmbusHostIPAddr[0];

#ifdef ET_IP_MODBUS_SERIAL
	}
	else if (pRequest->bModbusMsgType == MBUS_REQUESTTYPE_SERIAL)
	{
		if (pRequest->lHostAddress == 0)
			pRequest->lHostAddress = galClaimedHostCOMPorts[0].iPort;
	}
#endif

	/* Schedule message */
	nRequest = mbus_requestNewMessage(pRequest, bIO);

	platformReleaseMutex(ghmbusClientMutex);

	mbusDumpStr1(MBUS_TRACE_LEVEL_NOTICE, MBUS_TRACE_TYPE_MODBUS, pRequest->lHostAddress, platformAddrFromPath(pRequest->networkPath, (INT32)strlen(pRequest->networkPath)),
		"Sending request %d", nRequest);

	return nRequest;
}

/*---------------------------------------------------------------------------
** modbusGetResponse( )
**
** Get a response for the previously transferred Modbus message.
** nRequestId is a request Id returned from the previous call to the  
** modbusSendRequest function. 
** pResponse contain response data in case of a success and
** error indormation in case of an error.
** Returns 0 if able to get the response or one of the error constants 
** listed above. If return value is 0 it does not guarantee the Modbus
** response. You need to check bGeneralStatus member of pResponse to 
** determine whether the target returned success or an error. bGeneralStatus
** of 0 indicates a success. 
**---------------------------------------------------------------------------
*/
INT32 modbusGetResponse(INT32 nRequestId, MBSResponse* pResponse)
{
    MBUS_REQUEST* pRequest;        
    
    platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);
    
    pRequest = mbus_requestGetByRequestId( nRequestId );

    if ( pRequest == NULL )
    {
        platformReleaseMutex(ghmbusClientMutex);
        return ERR_MBUS_REQUEST_UNKNOWN_INDEX;        /* Request Id was not found */
    }
    
    if ( pRequest->nState != MBUS_REQUEST_RESPONSE_RECEIVED )
    {
        platformReleaseMutex(ghmbusClientMutex);
        mbusDumpStr2(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_MODBUS, pRequest->lHostIPAddr, pRequest->lIPAddress, 
				"modbusGetResponse RequestId = 0x%x, state = %d - response not ready yet", nRequestId, pRequest->nState);
        return ERR_MBUS_NO_AVAILABLE_RESPONSE;    /* Request is still pending without a response */
    }

    pResponse->bGeneralStatus = pRequest->bGeneralError;
    pResponse->iDataSize = pRequest->cfg.iDataSize;
    memcpy( pResponse->responseData, pRequest->cfg.pData, pResponse->iDataSize );                

    mbus_requestRemove( pRequest );
    platformReleaseMutex(ghmbusClientMutex);
    return 0;    
}

/*---------------------------------------------------------------------------
** modbusClaimIPAddress( )
**
** Claim one of the host IP addresses determined by EtherNet/IP stack.
** Returned TRUE if claimed successfully
**---------------------------------------------------------------------------
*/
BOOL modbusClaimIPAddress(UINT32 lIPAddress)
{
	platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);

	if (gpmbusHostIPAddr >= &galmbusHostIPAddr[MBUS_MAX_HOST_ADDRESSES])
	{
		/* not enough room to claim IP address */
		platformReleaseMutex(ghmbusClientMutex);
		return FALSE;
	}

	*gpmbusHostIPAddr = lIPAddress;
	gpmbusHostIPAddr++;

	platformReleaseMutex(ghmbusClientMutex);
	
	return TRUE;
}


/*---------------------------------------------------------------------------
** modbusUnclaimIPAddress( )
**
** Unclaim one of the host IP addresses determined by EtherNet/IP stack.
** Returned TRUE if claimed successfully
**---------------------------------------------------------------------------
*/
BOOL modbusUnclaimIPAddress(UINT32 lIPAddress)
{
	UINT32 i, *pIP, *pLoopIP;
	MBUS_SESSION* pSession;

	platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);

    for( i = 0, pIP = galmbusHostIPAddr; 
         pIP < gpmbusHostIPAddr;
         i++, pIP++)
    {
        if (*pIP == lIPAddress)
            break;
    }

	if (pIP == gpmbusHostIPAddr)
	{
		/* Didn't find the address */
		platformReleaseMutex(ghmbusClientMutex);
		return FALSE;
	}

	/* Close all sessions with this IP */
	for( pSession = gpnmbsSessions-1; pSession >= gmbsSessions; pSession-- )
	{
		if (pSession->lHostIPAddr == lIPAddress)
			mbus_sessionRemove(pSession, FALSE);
	}


	for( pLoopIP = pIP; pLoopIP < &galmbusHostIPAddr[MBUS_MAX_HOST_ADDRESSES]; pLoopIP++)
		*pLoopIP = *(pLoopIP+1);

	gpmbusHostIPAddr--;
	platformReleaseMutex(ghmbusClientMutex);
	
	return TRUE;
}

/*---------------------------------------------------------------------------
** modbusUpdateClaimIPAddress( )
**
** Update a claimed IP address.  Called when IP address of device is updated.
**---------------------------------------------------------------------------
*/
void modbusUpdateClaimIPAddress(UINT32 lIPOldAddress, UINT32 lIPNewAddress)
{
	UINT32* plIPAddress;
	platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);

	for (plIPAddress = galmbusHostIPAddr; plIPAddress < gpmbusHostIPAddr; plIPAddress++)
	{
		if (*plIPAddress == lIPOldAddress)
		{
			*plIPAddress = lIPNewAddress;
			platformReleaseMutex(ghmbusClientMutex);	
			return;
		}
	}
	
	platformReleaseMutex(ghmbusClientMutex);	
}

/*---------------------------------------------------------------------------
** modbusGetCOMPorts( )
**
** Returns an array of the COM port numbers
** alCOMPorts should point to the preallocated unsigned array 
** where COM ports should be stored.
** Returns the number of the obtained COM ports
**---------------------------------------------------------------------------
*/
INT32 modbusGetCOMPorts(UINT32* alCOMPorts)
{
#ifdef ET_IP_MODBUS_SERIAL
	UINT32 *pInCOMPort, *pOutCOMPort;
	int Count;

	platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);
	platformSerialInitTargetData();

	Count = gpnHostCOMPorts-galHostCOMPorts;
	for (pInCOMPort = galHostCOMPorts, pOutCOMPort = alCOMPorts; 
		pInCOMPort < gpnHostCOMPorts; 
		pInCOMPort++, pOutCOMPort++)
	{
		*pOutCOMPort = *pInCOMPort;
	}

	platformReleaseMutex(ghmbusClientMutex);

	return Count;
#else
	return 0;
#endif
}

/*---------------------------------------------------------------------------
** modbusClaimCOMPort( )
**
** Claim one of the COM ports returned by modbusGetCOMPorts().
** Will be called repeatedly for each claimed COM port.
** Returned TRUE if claimed successfully, FALSE if the COM port is not in 
** the host list.
**---------------------------------------------------------------------------
*/
INT32 modbusClaimCOMPort(UINT32 lCOMPort)
{
#ifdef ET_IP_MODBUS_SERIAL
	UINT32* pCOMPort;
	mbsClaimedCOMPort* pClaimedCOMPort;
	BOOL bRet;

	platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);

	/* Check first that it's not already claimed */
	for (pClaimedCOMPort = galClaimedHostCOMPorts; 
		pClaimedCOMPort < gpnClaimedHostCOMPorts; 
		pClaimedCOMPort++)
	{
		if (pClaimedCOMPort->iPort == lCOMPort)
			break;
	}

	if ( pClaimedCOMPort < gpnClaimedHostCOMPorts )
	{
		platformReleaseMutex(ghmbusClientMutex);
		return TRUE;
	}

	/* Check that it's in the COM port list */
	for (pCOMPort = galHostCOMPorts;
		pCOMPort < gpnHostCOMPorts; 
		pCOMPort++ )
	{
		if ((*pCOMPort) == lCOMPort)
			break;
	}

	if (pCOMPort >= gpnHostCOMPorts)
	{
		platformReleaseMutex(ghmbusClientMutex);
		return FALSE;
	}

	bRet = mbus_rtuAddClaimedCOMPort(lCOMPort);

	platformReleaseMutex(ghmbusClientMutex);
	return bRet;    
#else
	return FALSE;
#endif
}

/*---------------------------------------------------------------------------
** modbusGetClaimedCOMPorts( )
**
** Returns an array with the claimed COM ports.
** alCOMPorts should point to the preallocated unsigned array where the 
** claimed COM ports will be stored.
** Returns the number of the claimed COM ports
**---------------------------------------------------------------------------
*/
INT32 modbusGetClaimedCOMPorts(UINT32* alCOMPorts)
{
#ifdef ET_IP_MODBUS_SERIAL
	UINT32 *pOutCOMPort;
	mbsClaimedCOMPort* pInCOMPort;
	int Count;

	platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);

	Count = gpnClaimedHostCOMPorts-galClaimedHostCOMPorts;
	for (pInCOMPort = galClaimedHostCOMPorts, pOutCOMPort = alCOMPorts; 
		pInCOMPort < gpnClaimedHostCOMPorts; 
		pInCOMPort++, pOutCOMPort++)
	{
		*pOutCOMPort = pInCOMPort->iPort;
	}

	platformReleaseMutex(ghmbusClientMutex);

	return Count;
#else
	return 0;
#endif
}

/*---------------------------------------------------------------------------
** modbusClaimAllCOMPorts( )
**
** Claims the ownership of all available COM ports.
**---------------------------------------------------------------------------
*/
void modbusClaimAllCOMPorts()
{
#ifdef ET_IP_MODBUS_SERIAL
    UINT32* pCOMPort;
    
    for (pCOMPort = galHostCOMPorts; pCOMPort < gpnHostCOMPorts; pCOMPort++)
    {
        modbusClaimCOMPort(*pCOMPort);        
    }
#endif
}

/*---------------------------------------------------------------------------
** modbusIsSessionActive( )
**
** Determine if there has been recent communication to a particular device
**---------------------------------------------------------------------------
*/
BOOL modbusIsSessionActive(UINT32 lIPAddress, UINT32 lHostIPAddress)
{
	MBUS_SESSION* pSession;

	platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);
	pSession = mbus_sessionFindOutgoing(lIPAddress, lHostIPAddress, 2);
	platformReleaseMutex(ghmbusClientMutex);

	return (pSession != NULL);
}

/*---------------------------------------------------------------------------
** modbusIsSerialSessionActive( )
**
** Determine if there has been recent communication to a particular serial device
**---------------------------------------------------------------------------
*/
BOOL modbusIsSerialSessionActive(UINT32 lAddress, UINT32 lComPort)
{
#ifdef ET_IP_MODBUS_SERIAL
	MBUS_SL_SESSION* pSession;

	platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);
	pSession = mbus_rtuFindOutgoing(lAddress, lComPort);
	platformReleaseMutex(ghmbusClientMutex);

	return (pSession != NULL);
#else
	return FALSE;
#endif
}

#endif
