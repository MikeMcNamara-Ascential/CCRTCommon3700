/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** PLATFORM.C
**
** COPYRIGHT (c) 2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Platform dependant code for Netburner SDK
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

#include "..\..\eipinc.h"

/************************* Private variables *******************************/

/************************* Public variables *******************************/

OS_CRIT      gCriticalSection;


INT32 gSocketError;
/************************* Private Function Prototypes *********************/


/************************* Public Function Prototypes *********************/

/*---------------------------------------------------------------------------
** platformInitMutex()
**
** Allocates OS_CRIT mutex object
**---------------------------------------------------------------------------
*/

OS_CRIT* platformInitMutex(char *strMutexName)
{
   OSCritInit( &gCriticalSection );
   return &gCriticalSection;
}

/*---------------------------------------------------------------------------
** platformWriteLog()
**
** Writes a record to the log file
**---------------------------------------------------------------------------
*/

void platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp)
{
    char szTimeStamp[64];
    
    int nSecs = Secs%60, 
		nMins, nHours;
	
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
#if defined( TRACE_FILE_OUTPUT )

#elif defined( TRACE_DEBUGGER_OUTPUT )
    
    if(bTimeStamp) 
    {
        iprintf(szTimeStamp);
    }

    iprintf(szBuf);

#endif

}

/*---------------------------------------------------------------------------
** platformGetMacID()
**
** Get Mac ID from the GetAdaptersInfo
**---------------------------------------------------------------------------
*/

BOOL platformGetMacID(UINT32 lAddress, UINT8* pMacId)
{
    memcpy(pMacId, gConfigRec.mac_address, MAC_ADDR_LENGTH);
    return TRUE;
}

/*---------------------------------------------------------------------------
** platformGetTcpIpCfgData()
**
** Get the TCP/IP configuration data incluidng DHCP flag, subnet mask, gateway
** address, domain name, and name servers.
**---------------------------------------------------------------------------
*/

BOOL platformGetTcpIpCfgData( TCPIP_CONFIG_DATA *psCfg)
{    
    psCfg->lSubnetMask = gConfigRec.ip_Mask;
    psCfg->lGatewayAddr = gConfigRec.ip_GateWay;
    
    /* Set the User-defined path to the object associated with the underlying physical
    ** communication interface (e.g., an [F6] Ethernet Link Object Class) */
    psCfg->iLinkObjPathSize = 4;		/* 4 bytes long */
    psCfg->LinkObjPath[0] = 0x20;		/* Class logical segment */
    psCfg->LinkObjPath[1] = 0xF6;		/* Ethernet Link class ID */
    psCfg->LinkObjPath[2] = 0x24;		/* Instance logical segment */
    psCfg->LinkObjPath[3] = 1;			/* Instance based off of number of TCP/IP interfaces */

	/* Setup the multicast parameters */
	psCfg->bTTLValue = TTL_SUBNET_LEVEL;
	psCfg->MulticastData.bAllocControl = 0;
	psCfg->MulticastData.bReserved = 0;
	psCfg->MulticastData.iNumMcast = CIP_MAX_NUM_MULTICAST_ADDRESS;
	psCfg->MulticastData.lMcastStartAddr = CIP_MCAST_BASE;
    return TRUE;
}

/*---------------------------------------------------------------------------
** platformTestSetTcpIpCfgData()
**
** Ensure a particular TCP/IP object attribute can be set before it's 
** actually set.
**---------------------------------------------------------------------------
*/
BOOL platformTestSetTcpIpCfgData(UINT32 lMask, TCPIP_CONFIG_DATA *psCfg, UINT32 lCurrentIP, UINT8* pbGeneralError)
{
	/* not supported in Netburner */
	if ((lMask & (1 << TCPIP_INST_ATTR_HOST_NAME)) ||
		(lMask & (1 << TCPIP_INST_ATTR_CONFIG_CONTROL)))
	{
		*pbGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
		return FALSE;
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** platformSetTcpIpCfgData()
**
** Set the TCP/IP configuration data including subnet mask, gateway
** address, domain name, and name servers.
**---------------------------------------------------------------------------
*/

void platformSetTcpIpCfgData(UINT32 lMask, TCPIP_CONFIG_DATA *psCfg, UINT32 lCurrentIP)
{    
    /* 
	** This function is called to set the device's
	** TCP/IP configuration data (if supported)
	**
	** The lCurrentIP is the IP address these changes are attempting to be made on
	** 
	** The TCPIP_CONFIG_DATA structure is defined in eiptcpip.h as follows:
	**
	**	typedef struct tagTCPIP_CONFIG_DATA
	**	{
	**		UINT32   lConfigCapability;
	**		UINT32	 lConfigControl;
    **		UINT16   iLinkObjPathSize;
    **		UINT8    LinkObjPath[TCPIP_MAX_LINK_PATH_LENGTH];
	**		UINT32   lIpAddr;
	**		UINT32   lSubnetMask;
	**		UINT32   lGatewayAddr;
	**		UINT32   lNameServer;
	**		UINT32   lNameServer2;
	**		UINT8    szDomainName[TCPIP_MAX_DOMAIN_LENGTH+1];
	**		UINT8    szHostName[TCPIP_MAX_HOST_LENGTH+1];
	**		UINT8	 bTTLValue;
	**		TCPIP_MCAST_DATA MulticastData;
	**	} 
	**	TCPIP_CONFIG_DATA;
	**
	** Please see the EtherNet/IP protocol specification for further information.
	*/
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;
	UINT32 i;

	/* Ensure the configuration is valid */
    for ( i = 0, pTcpipInterface = gsTcpIpInstAttr; 
        i < gnClaimedHostIPAddrCount; 
        i++, pTcpipInterface++)
    {
		if (pTcpipInterface->InterfaceConfig.lIpAddr == psCfg->lIpAddr)
			break;
	}

	if (i >= gnClaimedHostIPAddrCount)
		return;

	/* Currently configured as volatile for life of program */
	if (lMask & (1 << TCPIP_INST_ATTR_TTL_VALUE))
	{
		pTcpipInterface->InterfaceConfig.bTTLValue = psCfg->bTTLValue;
	}

	if (lMask & (1 << TCPIP_INST_ATTR_MCAST_CONFIG))
	{
		pTcpipInterface->InterfaceConfig.MulticastData.bAllocControl = psCfg->MulticastData.bAllocControl;
		pTcpipInterface->InterfaceConfig.MulticastData.bReserved = psCfg->MulticastData.bReserved;
		pTcpipInterface->InterfaceConfig.MulticastData.iNumMcast = psCfg->MulticastData.iNumMcast;
		pTcpipInterface->InterfaceConfig.MulticastData.lMcastStartAddr = psCfg->MulticastData.lMcastStartAddr;
	}
}
/*---------------------------------------------------------------------------
** platformSocketInitTargetData( )
**
** Initialize gHostAddr array. It is passed as a response to the 
** List Targets UCMM request.
**---------------------------------------------------------------------------
*/
void platformSocketInitTargetData( void )
{    
    /* Store the first and only local IP address */
    galHostIPAddr[0] = gConfigRec.ip_Addr;
	gnHostIPAddrCount = 1;
	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "platformSocketInitTargetData the only IP address 0x%x\n", gConfigRec.ip_Addr);
}

/*---------------------------------------------------------------------------
** platformGetCurrentTickCount( )    
**
** Return current msec counter using a high-res performance timer.
**---------------------------------------------------------------------------
*/
UINT32 platformGetCurrentTickCount()
{
    DWORD dwTicks = TimeTick;
    DWORD dwMultiplier = 1000 / TICKS_PER_SECOND;
    DWORD dwLimit = 0xffffffff / dwMultiplier;
    
    while ( dwTicks > dwLimit )
        dwTicks -= dwLimit;
        
    dwTicks *= dwMultiplier;
    
    return dwTicks;
}

/*---------------------------------------------------------------------------
** platformGetNumEnetLinkInstances( )    
**
** Returns the number of supported Ethernet Link instances.  Typically equal
** to the number of TCP/IP object instances supported unless an embedded
** switch is involved
**---------------------------------------------------------------------------
*/
UINT32 platformGetNumEnetLinkInstances()
{
	return gnClaimedHostIPAddrCount;
}

/*---------------------------------------------------------------------------
** platformGetEnetLinkAttributes( )    
**
** Populates passed structure with Ethernet Link attributes including 
** interface flag, speed and MAC address
**---------------------------------------------------------------------------
*/
void platformGetEnetLinkAttributes(UINT32 iInstance, ENETLINK_INST_ATTR* pEnetLinkAttr)
{
	/* Populate with Ethernet Link attributes for local network card */
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig = &gsTcpIpInstAttr[iInstance-1];
	UINT32 lIPAddress = pTcpipConfig->InterfaceConfig.lIpAddr;
	
	/* If product can be accessed via a CIP port other than TCP/IP, then
    ** the link active bit must be determined.  If product only responds
    ** to one source of CIP requests (Ethernet port e.g.), then we can
    ** assume that it is active, since we got this request */
    pEnetLinkAttr->lInterfaceFlags = ENETLINK_FLAG_LINK_ACTIVE | ENETLINK_FLAG_SPEED_DUPLEX_FINE;

    /* The interface flags and interface speed must be determined
    ** from your hardware interface */
    pEnetLinkAttr->lInterfaceFlags &= (UINT32)~EL_FLAG_DUPLEX_MODE;
    pEnetLinkAttr->lInterfaceSpeed = 0;		                        /* 0 means "speed not known" */
    
	platformGetMacID(lIPAddress, pEnetLinkAttr->macAddr);

	pEnetLinkAttr->bInterfaceType = ENETLINK_INTERFACE_TYPE_TWISTED_PAIR;
	pEnetLinkAttr->bInterfaceState = ENETLINK_INTERFACE_STATE_ENABLED;
	pEnetLinkAttr->bAdminState = ENETLINK_ADMIN_STATE_ENABLED;
	strcpy(pEnetLinkAttr->InterfaceLabel, socketGetAddressString(lIPAddress));
}

/*---------------------------------------------------------------------------
** platformSetEnetLinkAttributes( )    
**
** Store settable Ethernet Link attributes 
**---------------------------------------------------------------------------
*/
void platformSetEnetLinkAttributes(UINT32 lMask, UINT32 iInstance, ENETLINK_INST_ATTR* pEnetLinkAttr)
{
	/* No set attributes supported */
}

/*---------------------------------------------------------------------------
** platformTestSetEnetLinkAttributes()
**
** Ensure a particular Ethernet Link object attribute can be set before it's 
** actually set.  Return FALSE if error is generated.
**---------------------------------------------------------------------------
*/
BOOL platformTestSetEnetLinkAttributes(UINT32 lMask, UINT32 iInstance, ENETLINK_INST_ATTR* pEnetLinkAttr, UINT8* pbGeneralError)
{
	/* Netburner has 1 to 1 relationship between TCP/IP object instance and Ethernet Link instance, 
	   so admin state can't be diabled */
	if (lMask & (1 << ENETLINK_INST_ATTR_ADMIN_STATE))
	{
		*pbGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		return FALSE;
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** platformGetHostName( )    
**
** Populates passed szName with host name up to nNameLen chars
**---------------------------------------------------------------------------
*/
INT32 platformGetHostName(char* szName, INT32 nNameLen)
{
	memcpy( (void*)szName, gConfigRec.mac_address, MAC_ADDR_LENGTH); 
	szName[MAC_ADDR_LENGTH] = 0;

	return MAC_ADDR_LENGTH;
}

/*---------------------------------------------------------------------------
** platformInetNtoa( )
**
** Return string representation of the passed IP address
**---------------------------------------------------------------------------
*/
INT8* platformInetNtoa( IN_ADDR IPAddress )
{
	static char gszIPString[32];

	PBYTE ipb = ( PBYTE ) & IPAddress.S_un.S_addr;
    sprintf( gszIPString, "%d.%d.%d.%d", ( int ) ipb[0], ( int ) ipb[1], ( int ) ipb[2], ( int ) ipb[3] );
	
    return (INT8*)gszIPString;
}

/*---------------------------------------------------------------------------
** platformAddrFromPath( )
**
** Get an IP address from the network path representation.
** szAddr is usually an IP string (i.e."216.233.160.129").
**---------------------------------------------------------------------------
*/

UINT32 platformAddrFromPath( const char* szAddr, INT32 nLen )
{    
    return AsciiToIp( szAddr );
}

/*---------------------------------------------------------------------------
** platformSetSockaddrIn( )
**
** Populates a sockaddr_in structure as an AF_INET type.  Used to 
** handle platform specific definitions of sockaddr_in
** 
** pSockaddr   - pointer to sockaddr_in structure to be populated
** iPort       - host order port number to be placed within this address structure
** lIpAddress  - network order IP Address to be placed within this address structure
** 
**---------------------------------------------------------------------------
*/
void platformSetSockaddrIn(struct sockaddr_in* pSockaddr, UINT16 iPort, UINT32 lIpAddress )
{
    memset(pSockaddr, 0, sizeof(struct sockaddr_in) );
    pSockaddr->sin_family = AF_INET;
    pSockaddr->sin_port = htons( iPort );
    platformGetInternetAddress(pSockaddr->sin_addr) = lIpAddress;
}

/*---------------------------------------------------------------------------
** platformSend( )
**
** send TCP data
**---------------------------------------------------------------------------
*/

INT32 platformSend(SOCKET lSockFd, UINT8* buf, INT32 len, INT32 lFlags)
{
	INT32       lSent = 0;
	fd_set		write_fds;
	fd_set		error_fds;

	if ( lSockFd == 0 || lSockFd == (SOCKET)INVALID_SOCKET )
	{
		gSocketError = -1;
		return -1;
	}

    FD_ZERO( &write_fds );
    FD_SET(lSockFd, &write_fds );

    FD_ZERO( &error_fds );
    FD_SET(lSockFd, &error_fds );

	gSocketError = EWOULDBLOCK;
	if ( ZeroWaitSelect( FD_SETSIZE, ( fd_set * ) 0, &write_fds, &error_fds ) != 0 )
	{
        if ( FD_ISSET(lSockFd, &error_fds ) )
		{
			FD_CLR(lSockFd, &error_fds);
			gSocketError = -1;
			return -1;
		}

		if ( FD_ISSET(lSockFd, &write_fds ) )
		{
			FD_CLR(lSockFd, &write_fds);
			lSent = write(lSockFd, (char*)buf, len ); 
		}
	}

	return lSent;
}

/*---------------------------------------------------------------------------
** platformSendTo( )
**
** send UDP data
**---------------------------------------------------------------------------
*/
INT32 platformSendTo(SOCKET lSockFd, UINT8* buf, INT32 len, INT32 iDummy1, struct sockaddr* pto, INT32 iDummy2, WORD iPort)
{
	UDPPacket pkt;
	pkt.SetSourcePort(iPort);
    pkt.SetDestinationPort(((sockaddr_in*)pto)->sin_port);
    pkt.AddData(buf, len);
	pkt.Send(platformGetInternetAddress(((sockaddr_in*)pto)->sin_addr));
	return len;
}

/*---------------------------------------------------------------------------
** platformRecv( )
**
** read data from a socket.
**---------------------------------------------------------------------------
*/

INT32 platformRecv(SOCKET lSockFd, const char* buf, INT32 len, INT32 lFlags)
{
	INT32 ltmpBytesReceived = -1;
	fd_set		read_fds;
	fd_set		error_fds;

	if ( lSockFd == 0 || lSockFd == (SOCKET)INVALID_SOCKET )
	{
		gSocketError = -1;
		return -1;
	}

	FD_ZERO( &read_fds );
	FD_SET(lSockFd, &read_fds );

	FD_ZERO( &error_fds );
	FD_SET(lSockFd, &error_fds );

	gSocketError = EWOULDBLOCK;
	if ( ZeroWaitSelect( FD_SETSIZE, &read_fds, ( fd_set * ) 0, &error_fds ) != 0 )
	{
		if ( FD_ISSET(lSockFd, &error_fds ) )
		{
			FD_CLR(lSockFd, &error_fds);
			gSocketError = -1;
			return -1;
		}

		if ( FD_ISSET(lSockFd, &read_fds ) )
		{
			FD_CLR(lSockFd, &read_fds);
			ltmpBytesReceived = read(lSockFd, (char*)buf, len);
			if (ltmpBytesReceived < 0)
			{
				gSocketError = -1;
				return -1;
			}
		}			
	}

	return ltmpBytesReceived;
}


/*---------------------------------------------------------------------------
** platformCloseSocket( )
**
** Closes a socket if valid.
**---------------------------------------------------------------------------
*/

void platformCloseSocket( SOCKET lSocket )
{    
    if ( lSocket != 0 && lSocket != (SOCKET)INVALID_SOCKET )
		close( lSocket );
}

/*---------------------------------------------------------------------------
** TCPConnectTask( )
**
** Task fucntion that tries to connect new TCP sessions
**---------------------------------------------------------------------------
*/

void TCPConnectTask( void *pd )
{
	SESSION* pSession;
	SOCKET   lSocket;
	INT32    nSessionId;
	UINT32   lClientIPAddr;
	
	OSTimeDly(1);
   
	while (1)
    {           
        platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);	    
        
        for( pSession = gSessions; pSession < gpnSessions; pSession++ )
		{	  
			if ( pSession->lState == OpenSessionLogged )
			{
     		    DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, 0, 0, "TCPConnectTask connect");
				pSession->lState = OpenSessionWaitingForTCPConnection;

				nSessionId = pSession->nSessionId;
				lClientIPAddr = pSession->lClientIPAddr;
				
				platformReleaseMutex(ghClientMutex);

				lSocket = connect( lClientIPAddr, 0, htons( ENCAP_SERVER_PORT ), CONNECT_TIMEOUT );
				
		        platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);	    

				pSession = sessionGetBySessionId( nSessionId );
				
				if ( pSession == NULL )
				    break;
    
                if ( lSocket > 0 )
				{
					SetSocketRxBuffers( lSocket, 15 );
					setsocketackbuffers( lSocket, 15 );
					
					/* Save the info for the newly connected socket */
					pSession->lSocket = lSocket;          
					pSession->lState = OpenSessionTCPConnectionEstablished; 
                    DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, lClientIPAddr, "Connect() succeeded with session Id 0x%x", nRet);					
				}
				else
				{
                      DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, lClientIPAddr, "Connect() failed with error code %d", nRet);
     				  sessionRemove( pSession, FALSE ); 
		        }
                      
            	break;
			}
		}
		
        platformReleaseMutex(ghClientMutex);
		
		OSTimeDly(1);
    }

}

/*---------------------------------------------------------------------------
** UdpReadTask( )
**
** Task function that sits and waits for any incoming UDP packets
**---------------------------------------------------------------------------
*/

void UdpReadTask( void *pd )
{
   OS_FIFO fifo;
   OSFifoInit( &fifo );

   //REgister to listen for UDP packets on port number 'port'
   RegisterUDPFifo( CLASS1_UDP_PORT, &fifo );

   while ( 1 )
   {
      //We construct a UDP packet object using the FIFO....
      UDPPacket upkt( &fifo, 1 );                      

      //Did we get a valid packet? or just time out?
      if ( upkt.Validate() )
      {         
		 DumpStr0(TRACE_LEVEL_DATA, TYPE_UDP_IN, 0, 0, "UDP packet received:\n");
		 DumpBuf(TRACE_LEVEL_DATA, TYPE_UDP_IN, 0, 0, (char*)upkt.GetDataBuffer(), upkt.GetDataSize());
         
         platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
		 
		 ioParseIOPacket( upkt.GetSourceAddress(), 0, upkt.GetDataBuffer(), upkt.GetDataSize() );   

		 platformReleaseMutex(ghClientMutex);
      }
   }
}

/*---------------------------------------------------------------------------
** BroadcastReadTask( )
**
** Task function that sits and waits for any incoming broadcast UDP packets
**---------------------------------------------------------------------------
*/

void BroadcastReadTask( void *pd )
{
	struct sockaddr_in sClientAddr;

	OS_FIFO fifo;
	OSFifoInit( &fifo );

	//REgister to listen for UDP packets on port number 'port'
	RegisterUDPFifo( ENCAP_SERVER_PORT, &fifo );

	while ( 1 )
	{
		//We construct a UDP packet object using the FIFO....
		UDPPacket upkt( &fifo, 1 );                      

		//Did we get a valid packet? or just time out?
		if ( upkt.Validate() )
		{
			platformGetInternetAddress(sClientAddr.sin_addr) = upkt.GetSourceAddress();
			sClientAddr.sin_port = upkt.GetSourcePort();

            platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
			socketHandleBroadcasts(gsTcpIpInstAttr, &sClientAddr, upkt.GetDataBuffer(), upkt.GetDataSize() );   
			platformReleaseMutex(ghClientMutex);
		}
	}
}

#ifdef ET_IP_SCANNER

/*---------------------------------------------------------------------------
** MulticastReadTask( )
**
** Task function that sits and waits for any incoming multicast UDP packets
**---------------------------------------------------------------------------
*/

void MulticastReadTask( void *pd )
{   
   gnMulticastCount = 0;
	
   OSFifoInit( &gMulticastFifo );

   while ( 1 )
   {
      if ( gnMulticastCount )
	  {	   
		  // We construct a UDP packet object using the FIFO....
		  UDPPacket upkt( &gMulticastFifo, 1 );                      

		  // Did we get a valid packet? or just time out?
		  if ( upkt.Validate() )
		  {         
             platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
			 
			 ioParseIOPacket( upkt.GetSourceAddress(), 0, upkt.GetDataBuffer(), upkt.GetDataSize() );   

			 platformReleaseMutex(ghClientMutex);
		  }
	  }
	  else
		  OSTimeDly(1);
   }
}

#endif /* ET_IP_SCANNER */
