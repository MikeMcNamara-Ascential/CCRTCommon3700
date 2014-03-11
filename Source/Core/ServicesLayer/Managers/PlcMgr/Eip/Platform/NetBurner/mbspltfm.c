/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSPLTFM.C
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Modbus Platform dependant code for Win32.
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
**
** Porting: <none>----0<major>         Customization: <none>----0<major>
**
*****************************************************************************
*****************************************************************************
*/

#include "../../eipinc.h"

/*---------------------------------------------------------------------------
** mbus_platformWriteLog()
**
** Writes a record to the log file
**---------------------------------------------------------------------------
*/

void mbus_platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp)
{
    char szTimeStamp[64];
    
    int nMins, nHours,
		nSecs = Secs%60;
	
	if (Secs != 0)
	{
		nMins = Secs / 60; 
		nHours = Secs / 3600; 
	}
	else
	{
		nHours = nMins = 0;
	}
	

    if(bTimeStamp)
    {
        sprintf(szTimeStamp, "\n%2.2d:%2.2d:%2.2d %u ", nHours, nMins, nSecs, gdwTickCount);
    }

    /* Output the string */
#if defined( MBUS_TRACE_FILE_OUTPUT )

#elif defined( MBUS_TRACE_DEBUGGER_OUTPUT )
    
    if(bTimeStamp) 
    {
        iprintf(szTimeStamp);
    }

    iprintf(szBuf);

#endif
}

/*---------------------------------------------------------------------------
** ModbusTCPConnectTask( )
**
** Task fucntion that tries to connect new TCP session for Modbus/TCP
**---------------------------------------------------------------------------
*/

void ModbusTCPConnectTask( void *pd )
{
	MBUS_SESSION* pSession;
	SOCKET   lSocket;
	INT32    nSessionId;
	UINT32   lClientIPAddr;
	
	OSTimeDly(1);
   
	while (1)
    {           
        platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);	    
        
		for( pSession = gmbsSessions; pSession < gpnmbsSessions; pSession++)
		{	  
			if ( pSession->lState == MBUS_SESSION_LOGGED)
			{
     		    mbusDumpStr0(MBUS_TRACE_LEVEL_NOTICE, MBUS_TRACE_TYPE_SESSION, 0, 0, "ModbusTCPConnectTask connect");
				pSession->lState = MBUS_SESSION_WAITING_FOR_TCP_CONNECTION;

				nSessionId = pSession->nSessionId;
				lClientIPAddr = pSession->lClientIPAddr;
				
				platformReleaseMutex(ghClientMutex);

				lSocket = connect( lClientIPAddr, 0, htons( MODBUS_PORT ), CONNECT_TIMEOUT );
				
		        platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);	    

				pSession = mbus_sessionGetBySessionId( nSessionId );
				
				if ( pSession == NULL )
				    break;
    
                if ( lSocket > 0 )
				{
					SetSocketRxBuffers( lSocket, 15 );
					setsocketackbuffers( lSocket, 15 );
					
					/* Save the info for the newly connected socket */
					pSession->lSocket = lSocket;          
					pSession->lState = MBUS_SESSION_ESTABLISHED; 
                    mbusDumpStr1(MBUS_TRACE_LEVEL_NOTICE, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, lClientIPAddr, "Connect() succeeded with session Id 0x%x", nRet);					
				}
				else
				{
                      mbusDumpStr1(MBUS_TRACE_LEVEL_NOTICE, MBUS_TRACE_TYPE_SESSION, pSession->lHostIPAddr, lClientIPAddr, "Connect() failed with error code %d", nRet);
     				  mbus_sessionRemove( pSession, FALSE ); 
		        }
                      
            	break;
			}
		}
		
        platformReleaseMutex(ghClientMutex);
		
		OSTimeDly(1);
    }

}
