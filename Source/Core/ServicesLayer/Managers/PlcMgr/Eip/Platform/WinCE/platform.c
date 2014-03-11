/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** PLATFORM.C
**
** COPYRIGHT (c) 2001,2002 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Platform dependant code for Windows CE.
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

/************************* Private variables *******************************/

FILE		*fLogFile;


/************************* Private Function Prototypes *********************/

void SetUDPBufferSize(void);

/*---------------------------------------------------------------------------
** platformInitLogFile( )
**
** Creates a trace dump file.
**---------------------------------------------------------------------------
*/

void platformInitLogFile(void)
{
	FILE* tmp;
	char szFileName[128];
	int i;
	
	/* Find the first unused sequential number for creating debug file name */
	for( i = 0; i < 1000; i++ )
	{
		sprintf( szFileName, "debug%d.txt", i);	
		tmp = fopen( szFileName, "r" );
		if ( !tmp )	/* File does not exist - break and use the name */
			break;
		fclose(tmp);
	}	

	/* Open the file */
	fLogFile = fopen( szFileName, "w" );
}

/*---------------------------------------------------------------------------
** platformCloseLogFile()
**
** Closes the log file
**---------------------------------------------------------------------------
*/

void platformCloseLogFile(void)
{
	fclose(fLogFile);
}

/*---------------------------------------------------------------------------
** platformWriteLog()
**
** Writes a record to the log file
**---------------------------------------------------------------------------
*/

void platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp)
{
#if defined(TRACE_FILE_OUTPUT) || defined (TRACE_DEBUGGER_OUTPUT)

    static char szBuffer[(MAX_PACKET_SIZE*3)+1000];
	static char szTimeStamp[64];
	static char szSrcIp[20], szDstIp[20];

    struct _timeb timebuffer;
    char *szTimeline;

	/* always use timestamp */
	bTimeStamp = bTimeStamp;
	/* iSeverity not included in log statement */
	iSeverity = iSeverity;
    
#ifdef MULTIPLE_LOG_FILES
	static UINT32 lLogCount = 0;
#endif

    _ftime( &timebuffer );
    szTimeline = ctime( & ( timebuffer.time ) );
	sprintf(szTimeStamp, "%.19s.%hu %u ", szTimeline, timebuffer.millitm, gdwTickCount);

	if ((lSrcIp != 0) || (lDstIp != 0))
	{
		strcpy(szSrcIp, socketGetAddressString(lSrcIp));
		strcpy(szDstIp, socketGetAddressString(lDstIp));
		sprintf(szBuffer, "%c %s (%s -> %s) %s\r\n", traceGetTypeAsChar(iType), szTimeStamp, szSrcIp, szDstIp, szBuf);
	}
	else
		sprintf(szBuffer, "%c %s %s\r\n", traceGetTypeAsChar(iType), szTimeStamp, szBuf);

    /* Output the string */
#if defined( TRACE_FILE_OUTPUT )

    fputs(szBuffer, fLogFile);    
    fflush( fLogFile );

#ifdef MULTIPLE_LOG_FILES
	lLogCount++;

	if (lLogCount%LOG_FILE_ENTRIES == 0)
	{
		platformCloseLogFile();
		platformInitLogFile();
	}

#endif /* MULTIPLE_LOG_FILES */

#elif defined( TRACE_DEBUGGER_OUTPUT )
    
    OutputDebugString(szBuffer);

#endif

#endif /* #if defined(TRACE_FILE_OUTPUT) || defined (TRACE_DEBUGGER_OUTPUT) */
}

/*---------------------------------------------------------------------------
** platformSocketLibInit( )
**
** Initialize the sockets library.
**---------------------------------------------------------------------------
*/

void platformSocketLibInit(void)
{
	INT32    err;	
	WORD	 wVersionRequested;
    WSADATA	 wsaData;
		   
    wVersionRequested = MAKEWORD( 2, 2 ); 
    err = WSAStartup( wVersionRequested, &wsaData );	/* Initialize Winsock */
    if ( err != 0 )
    {      
       notifyEvent( NM_UNABLE_INTIALIZE_WINSOCK, 0, 0 );
	   DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "WSAStartup Failed: %d\n", err );
       return;
    }

	SetUDPBufferSize();
	
}

/*---------------------------------------------------------------------------
** platformSocketLibCleanup( )
**
** Unload Winsock
**---------------------------------------------------------------------------
*/

void platformSocketLibCleanup()
{		
	WSACleanup();		/* Unload Winsock */
}


/*---------------------------------------------------------------------------
** platformGetMacID()
**
** Get Mac ID from the NETBIOS
**---------------------------------------------------------------------------
*/

BOOL platformGetMacID(UINT32 lAddress, UINT8* pMacId)
{   
	IP_ADAPTER_INFO AdapterInfo[MAX_HOST_ADDRESSES];		/* Allocate information for up to MAX_HOST_ADDRESSES NICs */
	IP_ADAPTER_INFO* pAdapterInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);					/* Save memory size of buffer */

	/* Init to zeroes, in case we fail */
	memset( pMacId, 0, MAC_ADDR_LENGTH );

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) != ERROR_SUCCESS)
		return FALSE;

	pAdapterInfo = AdapterInfo; /* Contains pointer to current adapter info */

	while (pAdapterInfo)
	{
		if (lAddress == inet_addr(pAdapterInfo->IpAddressList.IpAddress.String))
		{
			memcpy(pMacId, pAdapterInfo->Address, MAC_ADDR_LENGTH);
			break;
		}

		pAdapterInfo = pAdapterInfo->Next;
	}

	DumpStr7(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Mac id %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x for address: %s", pMacId[0], pMacId[1], pMacId[2], pMacId[3], pMacId[4], pMacId[5], socketGetAddressString(lAddress));

	/* Get the Mac address */
	return TRUE;
}


/*---------------------------------------------------------------------------
** platformGetTcpIpCfgData()
**
** Get the TCP/IP configuration data
**---------------------------------------------------------------------------
*/

BOOL platformGetTcpIpCfgData( TCPIP_CONFIG_DATA *psCfg)
{
    /* 
	** This function is called to retrieve the device's
	** TCP/IP configuration data.
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

	IP_ADAPTER_INFO AdapterInfo[MAX_HOST_ADDRESSES];		/* Allocate information for up to MAX_HOST_ADDRESSES NICs */
	IP_ADAPTER_INFO* pAdapterInfo;
	FIXED_INFO* pFixedInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);	/* Save memory size of buffer */
	PMIB_IPADDRTABLE pIPAddrTable;
	DWORD i, dwRetVal;
    TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;
	char str[512];

	/* First see if the IP address is already claimed.  If it's claimed, consider the 
	   TCP/IP config data to be initialized and just return the current data */
    for ( i = 0, pTcpipInterface = gsTcpIpInstAttr; 
        i < gnClaimedHostIPAddrCount; 
        i++, pTcpipInterface++)
    {
        if (pTcpipInterface->InterfaceConfig.lIpAddr == psCfg->lIpAddr)
		{
			psCfg->lSubnetMask = pTcpipInterface->InterfaceConfig.lSubnetMask;
			psCfg->lGatewayAddr = pTcpipInterface->InterfaceConfig.lGatewayAddr;
			psCfg->lNameServer = pTcpipInterface->InterfaceConfig.lNameServer;
			psCfg->lNameServer2 = pTcpipInterface->InterfaceConfig.lNameServer2;
			strcpy(psCfg->szDomainName, pTcpipInterface->InterfaceConfig.szDomainName);
			psCfg->lConfigCapability = pTcpipInterface->InterfaceConfig.lConfigCapability;
			psCfg->lConfigControl = pTcpipInterface->InterfaceConfig.lConfigControl;
			psCfg->bTTLValue = pTcpipInterface->InterfaceConfig.bTTLValue;
			psCfg->MulticastData.bAllocControl = pTcpipInterface->InterfaceConfig.MulticastData.bAllocControl;
			psCfg->MulticastData.bReserved = pTcpipInterface->InterfaceConfig.MulticastData.bReserved;
			psCfg->MulticastData.iNumMcast = pTcpipInterface->InterfaceConfig.MulticastData.iNumMcast;
			psCfg->MulticastData.lMcastStartAddr = pTcpipInterface->InterfaceConfig.MulticastData.lMcastStartAddr;
			return TRUE;
		}
    }

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) != ERROR_SUCCESS)
		return FALSE;

	pAdapterInfo = AdapterInfo;

	while (pAdapterInfo)
	{
		/* match the IP address to the one being claimed */
		if (psCfg->lIpAddr == inet_addr(pAdapterInfo->IpAddressList.IpAddress.String))
		{
			psCfg->lGatewayAddr = inet_addr(pAdapterInfo->GatewayList.IpAddress.String);
			if (pAdapterInfo->HaveWins) 
			{
				psCfg->lNameServer = inet_addr(pAdapterInfo->PrimaryWinsServer.IpAddress.String);
				psCfg->lNameServer2 = inet_addr(pAdapterInfo->SecondaryWinsServer.IpAddress.String);
			}
			else
			{
				psCfg->lNameServer = psCfg->lNameServer2 = 0;
			}

			break;
		}

		pAdapterInfo = pAdapterInfo->Next;
	}

	/* Get subnet mask */
	pIPAddrTable = (MIB_IPADDRTABLE*) malloc( sizeof( MIB_IPADDRTABLE) );

	/* Make an initial call to GetIpAddrTable to get the
	 * necessary size into the dwSize variable
	 */
	dwBufLen = sizeof(MIB_IPADDRTABLE);
	if (GetIpAddrTable(pIPAddrTable, &dwBufLen, 0) == ERROR_INSUFFICIENT_BUFFER)
	{
		free( pIPAddrTable );
		pIPAddrTable = (MIB_IPADDRTABLE *) malloc ( dwBufLen );
	}

	if (GetIpAddrTable( pIPAddrTable, &dwBufLen, 0 ) != NO_ERROR )
		return FALSE;

	for (i = 0; i < pIPAddrTable->dwNumEntries; i++)
	{
		if (pIPAddrTable->table[i].dwAddr == psCfg->lIpAddr)
		{
			psCfg->lSubnetMask = pIPAddrTable->table[i].dwMask;
			break;
		}
	}
	free(pIPAddrTable);

	/* Get domain name */
	pFixedInfo = malloc(sizeof(FIXED_INFO));
	dwBufLen = sizeof(FIXED_INFO);

	if( ERROR_BUFFER_OVERFLOW == GetNetworkParams(pFixedInfo, &dwBufLen))
	{
		free(pFixedInfo);
		pFixedInfo = (FIXED_INFO *)malloc(dwBufLen);

		if ((dwRetVal = GetNetworkParams( pFixedInfo, &dwBufLen )) != 0)
		{
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Call to GetNetworkParams failed. Return Value: %08x\n", dwRetVal );
			return FALSE;
		}
	}

	strcpy(psCfg->szDomainName, pFixedInfo->DomainName);
	free(pFixedInfo);

	/* Get the host name of the local machine (or device) */
    if (gethostname((char*)str, TCPIP_MAX_HOST_LENGTH) != SOCKET_ERROR )
    {
		strcpy((char*)psCfg->szHostName, str);
        DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Host Name: %s", str );
    }
	else
	{
		strcpy((char*)psCfg->szHostName, "");
        DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "ERROR: No Host Name");
	}

	/* Set the User-defined bitmaps used for network configuration options
	/* While Windows has the ability to switch between static and DHCP IP addresses, 
	   it doesn't provide an API to accomplish it through CIP, so from a CIP perspective
	   the capability is not supported */
    psCfg->lConfigCapability = 0;
    psCfg->lConfigControl = TCPIP_CONTROL_DHCP;

	/* Setup the multicast parameters */
	psCfg->bTTLValue = TTL_SUBNET_LEVEL;
	psCfg->MulticastData.bAllocControl = 0;
	psCfg->MulticastData.bReserved = 0;
	psCfg->MulticastData.iNumMcast = CIP_MAX_NUM_MULTICAST_ADDRESS;
	psCfg->MulticastData.lMcastStartAddr = CIP_MCAST_BASE;

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Claimed IP address: %s", socketGetAddressString(psCfg->lIpAddr));
	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Subnet mask: %s", socketGetAddressString(psCfg->lSubnetMask));
	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Gateway address: %s", socketGetAddressString(psCfg->lGatewayAddr));
	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Primary name server: %s", socketGetAddressString(psCfg->lNameServer));
	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Seconary name server: %s", socketGetAddressString(psCfg->lNameServer2));
	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Domain name: %s", psCfg->szDomainName);
	
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
	/* not supported in windows CE*/
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
   char		szHostName[1024];
   HOSTENT* lphostent;
   INT32	i;
   
   /* Get host name of the local machine (or device) */
   if ( gethostname(szHostName, 1024) == SOCKET_ERROR )
   {
	  DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "platformSocketInitTargetData error calling gethostname");
	  notifyEvent( NM_ERROR_USING_WINSOCK, 0, 0 );	  
	  return;  
   }	
   
   /* Retrieve host information corresponding to a host name from a host database */
   lphostent = gethostbyname(szHostName);
   if ( lphostent == NULL )
   {
	  DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "platformSocketInitTargetData error calling gethostbyname");
	  notifyEvent( NM_ERROR_USING_WINSOCK, 0, 0 );	  
	  return;  
   }
   
   /* Extract the first (and usually only) IP address */
   memcpy(&galHostIPAddr[0], lphostent->h_addr_list[0], sizeof(UINT32));
   gnHostIPAddrCount = 1;

   /* Extract the rest of IP address if there is more than one */
   for( i = 1; i < MAX_HOST_ADDRESSES; i++ )
   {
	  if ( lphostent->h_addr_list[0][i*4] && 
		   ( lphostent->h_addr_list[0][i*4] != lphostent->h_name[0] ||
		     lphostent->h_addr_list[0][i*4+1] != lphostent->h_name[1] ) )
	  {
		  memcpy(&galHostIPAddr[i], &lphostent->h_addr_list[0][i*4], sizeof(UINT32));
		  gnHostIPAddrCount++;
	  }
	  else
		  break;
   }
}

/*---------------------------------------------------------------------------
** SetUDPBufferSize( )
**
** Set UDP registry key to reserve the buffer size
**---------------------------------------------------------------------------
*/

void SetUDPBufferSize(void)
{
   HKEY    hKey;
   UINT32  lDisp;
   UINT32  lBufferSize;
   HRESULT hRes;
   static const TCHAR szRegistryKey[] =  TEXT("Comm\\AFD");
   static const TCHAR szRegValue[]    =  TEXT("DgramBuffer");

   hRes = RegCreateKeyExW(HKEY_LOCAL_MACHINE, szRegistryKey, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &lDisp);
   if (hRes != ERROR_SUCCESS) 
   {
	   DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Unable to create AFD key to set UDP buffer size in registry");
       return;
   }
   
   lBufferSize = COMMON_BUFFER_SIZE/1000;
   hRes = RegSetValueExW(hKey, szRegValue, 0, REG_DWORD, (LPBYTE)&lBufferSize, sizeof(UINT32));
   RegCloseKey(hKey);                  
}


/*---------------------------------------------------------------------------
** platformGetCurrentTickCount( )	
**
** Return current msec counter using a high-res performance timer.
**---------------------------------------------------------------------------
*/
UINT32 platformGetCurrentTickCount()
{
	static double dFreq = 0;
	LARGE_INTEGER liCount, liFreq;

	if ( dFreq == 0 )
	{
		QueryPerformanceFrequency(&liFreq);
		dFreq = (double)liFreq.QuadPart / 1000;

		if ( dFreq == 0 )
			return 0;
	}

	QueryPerformanceCounter(&liCount);
	return( (DWORD)((double)liCount.QuadPart / dFreq) );
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
	/* Windows has 1 to 1 relationship between TCP/IP object instance and Ethernet Link instance, 
	   so admin state can't be diabled */
	if (lMask & (1 << ENETLINK_INST_ATTR_ADMIN_STATE))
	{
		*pbGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		return FALSE;
	}

	return TRUE;
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
    UINT32      lIPAddress = (UINT32)ERROR_STATUS;
    struct hostent    *phost;
    char addr[MAX_NETWORK_PATH_SIZE];

    memcpy(addr, szAddr, nLen);
    addr[nLen] = 0;

    /*
    ** Look up the name, assumed initially it is in dotted format.
    ** If it wasn't dotted, maybe it's a symbolic name 
    */
    if ( ( lIPAddress = inet_addr( addr ) ) == ERROR_STATUS )
    {
        if ( (phost = gethostbyname(addr)) != NULL )
        {
            memcpy( &lIPAddress, phost->h_addr_list[0], sizeof(UINT32) );
        }
    }

    return lIPAddress;
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
** platformCopySockAddrInToBuffer( )
**
** Populates an EIP "sockaddr_in" buffer with a sockaddr_in structure.
** 
** This function copies the entire structure and then adjusts the first 2
** bytes to match the EIP expectations
**---------------------------------------------------------------------------
*/
void platformCopySockAddrInToBuffer( UINT8* pBuffer, struct sockaddr_in* pSockAddr )
{
	memcpy(pBuffer, pSockAddr, sizeof(struct sockaddr_in));
	*((UINT16*)pBuffer) = htons(AF_INET);
}

/*---------------------------------------------------------------------------
** platformCopySockAddrInFromBuffer( )
**
** Populates a sockaddr_in structure with an EIP "sockaddr_in" buffer from
** a packet.
** 
** This function copies the entire structure and then adjusts the sin_family
** element to match Windows expectations
**---------------------------------------------------------------------------
*/
void platformCopySockAddrInFromBuffer( struct sockaddr_in* pSockAddr, UINT8* pBuffer )
{
	memcpy(pSockAddr, pBuffer, sizeof(struct sockaddr_in));
	pSockAddr->sin_family = AF_INET;
}