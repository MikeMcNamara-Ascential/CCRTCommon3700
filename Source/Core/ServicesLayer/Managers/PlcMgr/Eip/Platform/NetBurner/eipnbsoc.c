/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** SOCKET.C
**
** COPYRIGHT (c) 2001,2002 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Ethernet Socket interface.
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

#include "..\..\eipinc.h"

OS_FIFO gMulticastFifo;
int     gnMulticastCount;

/*---------------------------------------------------------------------------
** socketPostListen( )
**
** Prepare the claimed TCP listening sockets. It will be reused when 
** waiting for the new TCP incoming connections.
**---------------------------------------------------------------------------
*/
void socketPostListen(SOCKET* pWaitSocket, UINT32 lClaimedHostIPAddr)
{
    (*pWaitSocket) = listen( INADDR_ANY, htons( ENCAP_SERVER_PORT ), 3 );	
	
}  /* end socketPostListen( ) */

   
/*---------------------------------------------------------------------------
** socketCheckIncomingSession( )
**
** Check if we have the new incoming TCP session
**---------------------------------------------------------------------------
*/

void socketCheckIncomingSession()
{    
    SOCKET              lSocket;
    SESSION*            pSession;
    TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig;
	IPADDR				lClientAddr;
    UINT16				iPort;
            
            
    /* Make sure the number of open sessions is under the limit */
    if ( gpnSessions >= &gSessions[MAX_SESSIONS] )
        return;
    
    /* check all listening sockets for incomming requests */ 
    for(pTcpIpConfig = gsTcpIpInstAttr; 
        pTcpIpConfig < &gsTcpIpInstAttr[gnClaimedHostIPAddrCount]; 
        pTcpIpConfig++)
    {
        /* set only the prepared sockets */
        if( pTcpIpConfig->lPassiveWaitSocket == (SOCKET)INVALID_SOCKET )
            continue;

        if ( dataavail( pTcpIpConfig->lPassiveWaitSocket ) )
        {
			DumpStr1(TRACE_LEVEL_DATA, TRACE_TYPE_SESSION, pTcpIpConfig->InterfaceConfig.lIpAddr, 0, "dataavail returned TRUE for socket %d\n", pTcpIpConfig->lPassiveWaitSocket);
			lSocket = accept( pTcpIpConfig->lPassiveWaitSocket, &lClientAddr, &iPort, 1 ); 
			DumpStr1(TRACE_LEVEL_DATA, TRACE_TYPE_SESSION, pTcpIpConfig->InterfaceConfig.lIpAddr, lClientAddr, 
					"accept returned socket %d", lSocket);

            if ( lSocket > 0 )
			{
				DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pTcpIpConfig->InterfaceConfig.lIpAddr, lClientAddr, 
						"socketCheckIncomingSession opened new incoming socket %d", lSocket);            
        
				/* Allocate new session object */
				pSession = sessionNew( lClientAddr, pTcpIpConfig->InterfaceConfig.lIpAddr, TRUE );
                      
				if ( pSession == NULL )    /* Exceeded maximum number of open sessions */
				{
					DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_SESSION, pTcpIpConfig->InterfaceConfig.lIpAddr, 0, "socketCheckIncomingSession exceeded maximum number of open sessions");
					continue;
				}

				/* Save the info for the newly connected socket */
				pSession->lSocket = lSocket;          
				pSession->lClientIPAddr = lClientAddr;
			}
		}
    }
    
}  /* end socketCheckIncomingSession( ) */

#ifdef ET_IP_SCANNER

/*---------------------------------------------------------------------------
** INT32 socketJoinMulticastGroup(CONNECTION* pConnection)
**
** Join multicast group, so we can receive on a connection where we are the 
** originator and Target -> Originator transport mode set to Multicast ( as
** opposed to PointToPoint )
**---------------------------------------------------------------------------
*/

INT32 socketJoinMulticastGroup(CONNECTION* pConnection)
{
    RegisterMulticastFifo( platformGetInternetAddress(pConnection->sReceiveAddr.sin_addr), CLASS1_UDP_PORT, &gMulticastFifo );	
	gnMulticastCount++;
	
	return gnMulticastCount;
}

/*-----------------------------------------------------------------------------
** INT32 socketDropMulticastGroup(CONNECTION* pConnection)
**
** Drop out membership in the multicast group once the connection is terminated
**-----------------------------------------------------------------------------
*/

INT32 socketDropMulticastGroup(CONNECTION* pConnection)
{
    if ( gnMulticastCount )
		gnMulticastCount--;
	
	UnregisterMulticastFifo( platformGetInternetAddress(pConnection->sReceiveAddr.sin_addr), CLASS1_UDP_PORT );	
	
	return gnMulticastCount;
}

#endif /* #ifdef ET_IP_SCANNER */

/****************************************************************************
**
** End of SOCKET.C
**
*****************************************************************************
*/
