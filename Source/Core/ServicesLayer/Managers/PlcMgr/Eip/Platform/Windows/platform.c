/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** PLATFORM.C
**
** COPYRIGHT (c) 2001-2009 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Platform dependant code for Win32.
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

/* define this if you want logging broken up into multiple, smaller log files */
/* #define MULTIPLE_LOG_FILES */
/* number of log file entries before a new file is created */
#define LOG_FILE_ENTRIES	200000

BOOL GetTcpIpDataFromRegistry( TCPIP_CONFIG_DATA *psCfg);
static void GetDhcpState(HKEY hKey, BOOL* pDhcpState);
static BOOL GetTcpIPRegistryAddressValue(HKEY hKey, TCHAR* strValue1, TCHAR* strValue2, UINT32* pValue);
static void GetRegistryString(TCHAR* regValue, UINT8* dest, INT32 nDestSize );
BOOL GetMacIDViaNetbios(UINT32 lAddress, UINT8* pMacId);
static TCHAR* GetLoggingDirectory();
BOOL platformGetMacID(UINT32 lAddress, UINT8* pMacId);

/************************* Private variables *******************************/

static FILE        *fLogFile;

/*---------------------------------------------------------------------------
** platformInitLogFile( )
**
** Creates a trace dump file.
**---------------------------------------------------------------------------
*/

void platformInitLogFile(void)
{
    FILE* tmp;
    TCHAR szFileName[1000+128],
		 szLoggingDirectory[1000];
    int i;

	_tcscpy(szLoggingDirectory, GetLoggingDirectory());

	_mkdir(szLoggingDirectory);

     /* Find the first unused sequential number for creating debug file name */
    for( i = 0; i < 1000; i++ )
    {
        _stprintf(szFileName, _T("%sdebug%d.txt"), szLoggingDirectory, i);    
        tmp = _tfopen( szFileName, _T("r"));
        if ( !tmp )    /* File does not exist - break and use the name */
            break;
        fclose(tmp);
    }    

    /* Open the file */
    fLogFile = _tfopen( szFileName, _T("w"));

}

/*---------------------------------------------------------------------------
** platformCloseLogFile()
**
** Closes the log file
**---------------------------------------------------------------------------
*/

void platformCloseLogFile(void)
{
	if (fLogFile != NULL)
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
	sprintf(szTimeStamp, "%.19s.%03hu %u ", szTimeline, timebuffer.millitm, gdwTickCount);

	if ((lSrcIp != 0) || (lDstIp != 0))
	{
		strcpy(szSrcIp, socketGetAddressString(lSrcIp));
		strcpy(szDstIp, socketGetAddressString(lDstIp));
		sprintf(szBuffer, "%c %s (%s -> %s) %s\n", traceGetTypeAsChar(iType), szTimeStamp, szSrcIp, szDstIp, szBuf);
	}
	else
		sprintf(szBuffer, "%c %s %s\n", traceGetTypeAsChar(iType), szTimeStamp, szBuf);

    /* Output the string */
#if defined( TRACE_FILE_OUTPUT )
	if (fLogFile != NULL)
	{
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
	}

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
    WORD    wVersionRequested;
    WSADATA     wsaData;
           
    wVersionRequested = MAKEWORD( 2, 2 ); 
    err = WSAStartup( wVersionRequested, &wsaData );    /* Initialize Winsock */
    if ( err != 0 )
    {      
       notifyEvent( NM_UNABLE_INTIALIZE_WINSOCK, 0, 0 );
       DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "WSAStartup Failed: %d\n", err );
       return;
    }
    
}

/*---------------------------------------------------------------------------
** platformSocketLibCleanup( )
**
** Unload Winsock
**---------------------------------------------------------------------------
*/

void platformSocketLibCleanup()
{        
    WSACleanup();        /* Unload Winsock */
}

/*---------------------------------------------------------------------------
** platformGetMacID()
**
** Get Mac ID from the GetAdaptersInfo
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
		return GetMacIDViaNetbios(lAddress, pMacId);

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

	DumpStr7(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, 
			"Mac id %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x for address: %s", 
			pMacId[0], pMacId[1], pMacId[2], pMacId[3], pMacId[4], pMacId[5], 
			socketGetAddressString(lAddress));

	/* Get the Mac address */
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
			strcpy((char*)psCfg->szDomainName, (char*)pTcpipInterface->InterfaceConfig.szDomainName);
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
		return GetTcpIpDataFromRegistry(psCfg);

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

	if (pIPAddrTable != NULL)
	{
		for (i = 0; i < pIPAddrTable->dwNumEntries; i++)
		{
			if (pIPAddrTable->table[i].dwAddr == psCfg->lIpAddr)
			{
				psCfg->lSubnetMask = pIPAddrTable->table[i].dwMask;
				break;
			}
		}
		free(pIPAddrTable);
	}

	/* Get domain name */
	pFixedInfo = (FIXED_INFO*)malloc(sizeof(FIXED_INFO));
	dwBufLen = sizeof(FIXED_INFO);

	if( ERROR_BUFFER_OVERFLOW == GetNetworkParams(pFixedInfo, &dwBufLen))
	{
		free(pFixedInfo);
		pFixedInfo = (FIXED_INFO *)malloc(dwBufLen);

		if ((dwRetVal = GetNetworkParams( pFixedInfo, &dwBufLen )) != 0)
		{
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, 
				"Call to GetNetworkParams failed. Return Value: %08x\n", dwRetVal );
			free(pFixedInfo);
			return FALSE;
		}
	}

	if (pFixedInfo != NULL)
	{
		strncpy((char*)psCfg->szDomainName, (char*)pFixedInfo->DomainName, TCPIP_MAX_DOMAIN_LENGTH);
		free(pFixedInfo);
	}

	/* Get the host name of the local machine (or device) */
    if (gethostname((char*)str, TCPIP_MAX_HOST_LENGTH) != SOCKET_ERROR )
    {
		strncpy((char*)psCfg->szHostName, str, TCPIP_MAX_HOST_LENGTH);
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

    /* Set the User-defined path to the object associated with the underlying physical
    ** communication interface (e.g., an [F6] Ethernet Link Object Class) */
    psCfg->iLinkObjPathSize = 4;		/* 4 bytes long */
    psCfg->LinkObjPath[0] = 0x20;		/* Class logical segment */
    psCfg->LinkObjPath[1] = 0xF6;		/* Ethernet Link class ID */
    psCfg->LinkObjPath[2] = 0x24;		/* Instance logical segment */
    psCfg->LinkObjPath[3] = (UINT8)((pTcpipInterface-gsTcpIpInstAttr)+1);	/* Instance based off of number of TCP/IP interfaces */

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
** actually set.  Return FALSE if error is generated.
**---------------------------------------------------------------------------
*/
BOOL platformTestSetTcpIpCfgData(UINT32 lMask, TCPIP_CONFIG_DATA *psCfg, UINT32 lCurrentIP, UINT8* pbGeneralError)
{
	/* not supported in windows */
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

	/* Volatile for life of program */
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
    char     szHostName[1024];
    HOSTENT* lphostent;
    INT32    i;

    gnHostIPAddrCount = 0;
   
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
** platformGetCurrentTickCount( )    
**
** Return current msec counter using a high-res performance timer.
**---------------------------------------------------------------------------
*/
UINT32 platformGetCurrentTickCount()
{
    static LARGE_INTEGER dFreq = {0};
    LARGE_INTEGER liCount, liFreq, liTotal;

    if ( dFreq.QuadPart == 0 )
    {
        QueryPerformanceFrequency(&liFreq);
        dFreq.QuadPart = liFreq.QuadPart / 1000;

        if ( dFreq.QuadPart == 0 )
            return 0;
    }

    QueryPerformanceCounter(&liCount);
	liTotal.QuadPart = liCount.QuadPart / dFreq.QuadPart;
    return (UINT32)liTotal.QuadPart;
}

/*---------------------------------------------------------------------------
** platformGetCurrentAbsTime( )    
**
** Returns time in seconds from Jan 1, 1970 and current millisecond (0-1000)
**---------------------------------------------------------------------------
*/
void platformGetCurrentAbsTime(UINT32* plSecTime, UINT16* puMillisecTime)
{
	struct _timeb    timebuf;
	_ftime(&timebuf);

	*plSecTime = (UINT32)timebuf.time;
	*puMillisecTime = timebuf.millitm;
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
		*pbGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
		return FALSE;
	}

	return TRUE;
}

#ifdef EIP_QOS
/*---------------------------------------------------------------------------
** platformGetQoSAttributes( )    
**
** Populates passed structure with QoS attributes
**---------------------------------------------------------------------------
*/
void platformGetQoSAttributes( TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig )
{
	/* 
	** This function is called when a host IP address is claimed by the client
	** application.
	**
	** It should populate the pTcpipConfig->QoSConfig structure with
	** the QoS attributes for the local network interface with IP 
	** address pTcpipConfig->InterfaceConfig.lIpAddr 
	**
	** The QoSConfig structure is defined in eipqos.h as
	**
	**	typedef struct tagQOS_INST_ATTR
	**	{
	**	UINT8    b8021QTagEnable;
	**	UINT8	 bDSCPPTPEvent;
	**	UINT8	 bDSCPPTPGeneral;
	**	UINT8	 bDSCPUrgent;
	**	UINT8    bDSCPScheduled;
	**	UINT8	 bDSCPHigh;
	**	UINT8	 bDSCPLow;
	**	UINT8	 bDSCPExplicit;
	**	}
	**	QOS_INST_ATTR;
	**
	** Please see the EtherNet/IP protocol specification for further information.
	*/
	DWORD i;
    TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;

	/* First see if the IP address is already claimed.  If it's claimed, consider the 
	   TCP/IP config data to be initialized and just return the current data */
    for ( i = 0, pTcpipInterface = gsTcpIpInstAttr; 
        i < gnClaimedHostIPAddrCount; 
        i++, pTcpipInterface++)
    {
        if (pTcpipInterface->InterfaceConfig.lIpAddr == pTcpipConfig->InterfaceConfig.lIpAddr)
		{
			/* Data already set */
			return;
		}
    }

	pTcpipConfig->QoSConfig.b8021QTagEnable = FALSE;
	pTcpipConfig->QoSConfig.bDSCPPTPEvent = QOS_DEFAULT_VALUE_DSCP_PTP_EVENT;
	pTcpipConfig->QoSConfig.bDSCPPTPGeneral = QOS_DEFAULT_VALUE_DSCP_PTP_GENERAL;
	pTcpipConfig->QoSConfig.bDSCPUrgent = QOS_DEFAULT_VALUE_DSCP_URGENT;
	pTcpipConfig->QoSConfig.bDSCPScheduled = QOS_DEFAULT_VALUE_DSCP_SCHEDULED;
	pTcpipConfig->QoSConfig.bDSCPHigh = QOS_DEFAULT_VALUE_DSCP_HIGH;
	pTcpipConfig->QoSConfig.bDSCPLow = QOS_DEFAULT_VALUE_DSCP_LOW;
	pTcpipConfig->QoSConfig.bDSCPExplicit = QOS_DEFAULT_VALUE_DSCP_EXPLICIT;
}

/*---------------------------------------------------------------------------
** platformSetQoSAttributes( )    
**
** Store settable QoS attributes 
**---------------------------------------------------------------------------
*/
BOOL platformSetQoSAttributes(UINT32 lMask, TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig )
{
	/* 
	** This function is called when a CIP message wants to set QoS attributes.
	**
	** It should use the pTcpipConfig->QoSConfig structure with
	** the QoS attributes for the local network interface with IP 
	** address pTcpipConfig->InterfaceConfig.lIpAddr 
	**
	** The QoSConfig structure is defined in eipqos.h as
	**
	**	typedef struct tagQOS_INST_ATTR
	**	{
	**	UINT8    b8021QTagEnable;
	**	UINT8	 bDSCPPTPEvent;
	**	UINT8	 bDSCPPTPGeneral;
	**	UINT8	 bDSCPUrgent;
	**	UINT8    bDSCPScheduled;
	**	UINT8	 bDSCPHigh;
	**	UINT8	 bDSCPLow;
	**	UINT8	 bDSCPExplicit;
	**	}
	**	QOS_INST_ATTR;
	**
	** lMask determines which attributes are being set (bit flag)
	**
	** Please see the EtherNet/IP protocol specification for further information.
	*/
	DWORD i;
    TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;

	if (lMask & (1 << QOS_INST_ATTR_8021Q_TAG_ENABLE))
	{
		/* 802.1Q Tag frames not supported */
		return FALSE;
	}


	/* Find the claimed TCP/IP instance and save the QoS data for the life 
	   of the program since we don't have NVS support */
    for ( i = 0, pTcpipInterface = gsTcpIpInstAttr; 
        i < gnClaimedHostIPAddrCount; 
        i++, pTcpipInterface++)
    {
        if (pTcpipInterface->InterfaceConfig.lIpAddr == pTcpipConfig->InterfaceConfig.lIpAddr)
		{

			if (lMask & (1 << QOS_INST_ATTR_DSCP_PTP_EVENT))
				pTcpipInterface->QoSConfig.bDSCPPTPEvent = pTcpipConfig->QoSConfig.bDSCPPTPEvent;
			if (lMask & (1 << QOS_INST_ATTR_DSCP_PTP_GENERAL))
				pTcpipInterface->QoSConfig.bDSCPPTPGeneral = pTcpipConfig->QoSConfig.bDSCPPTPGeneral;
			if (lMask & (1 << QOS_INST_ATTR_DSCP_URGENT))
				pTcpipInterface->QoSConfig.bDSCPUrgent = pTcpipConfig->QoSConfig.bDSCPUrgent;
			if (lMask & (1 << QOS_INST_ATTR_DSCP_SCHEDULED))
				pTcpipInterface->QoSConfig.bDSCPScheduled = pTcpipConfig->QoSConfig.bDSCPScheduled;
			if (lMask & (1 << QOS_INST_ATTR_DSCP_HIGH))
				pTcpipInterface->QoSConfig.bDSCPHigh = pTcpipConfig->QoSConfig.bDSCPHigh;
			if (lMask & (1 << QOS_INST_ATTR_DSCP_LOW))
				pTcpipInterface->QoSConfig.bDSCPLow = pTcpipConfig->QoSConfig.bDSCPLow;
			if (lMask & (1 << QOS_INST_ATTR_DSCP_EXPLICIT))
				pTcpipInterface->QoSConfig.bDSCPExplicit = pTcpipConfig->QoSConfig.bDSCPExplicit;

			return TRUE;
		}
    }

	/* Couldn't find the interface (shouldn't happen) */
	return FALSE;
}

/*---------------------------------------------------------------------------
** platformSetDSCPValue( )    
**
** Sets the DSCP value for each socket to have QoS functionality
**---------------------------------------------------------------------------
*/
INT32 platformSetDSCPValue(SOCKET s, UINT8 bDSCPValue)
{
	INT32 iRet;

	/* DSCP value is 2 bits off because of ECN values */
	bDSCPValue <<= 2;
	iRet = setsockopt(s, IPPROTO_IP, IP_TOS, (const char*)&bDSCPValue, sizeof(bDSCPValue));
	if (iRet == SOCKET_ERROR)
        DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "platformSetDSCPValue failed calling setsockopt() for socket 0x%x.  Error code = %d", s, platformGetLastError());

	return 0;
}

#endif /* EIP_QOS */

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

/*---------------------------------------------------------------------------
** GetTcpIpDataFromRegistry()
**
** Get the TCP/IP configuration data incluidng DHCP flag, subnet mask, gateway
** address, domain name, and name servers.  Windows NT doesn't support
** GetAdaptersInfo() and this can be used as a backup
**---------------------------------------------------------------------------
*/

BOOL GetTcpIpDataFromRegistry( TCPIP_CONFIG_DATA *psCfg)
{    
    TCHAR szStr[256];
    TCHAR *p;
	char addr[20];
    UINT32 i;
    HKEY   hKey, hNetCardKey, hTcpipKey;
    DWORD  dwSubkeys = 0;
	UINT32  lAddress;
    unsigned long lType, lSize;
	BOOL bDhcpState;

    /* Set the User-defined bitmaps used for network configuration options */
	/* While Windows has the ability to switch between static and DHCP IP addresses, 
	   it doesn't provide an API to accomplish it through CIP, so from a CIP perspective
	   the capability is not supported */
    psCfg->lConfigCapability = 0;
    psCfg->lConfigControl = TCPIP_CONTROL_DNS_ENABLE & (~TCPIP_CONTROL_CONFIG_BITMASK);
	
	GetRegistryString(_T("Domain"), psCfg->szDomainName, TCPIP_MAX_DOMAIN_LENGTH);
    DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Domain Name: %s", psCfg->szDomainName);

    psCfg->lNameServer = psCfg->lNameServer2 = 0;
    GetRegistryString(_T("NameServer"), (unsigned char*)szStr, sizeof(szStr)-1);            

    if( (p = _tcstok(szStr, _T(" \n"))) != NULL ) 
    {
#if defined( _UNICODE )		
			wcstombs(addr, p, _tcslen(p));
#else // #if defined( _UNICODE )	
			strcpy(addr, p);
#endif // #if defined( _UNICODE )

        psCfg->lNameServer = inet_addr(addr);
        
        if( (p = _tcstok(NULL,_T(" \n"))) != NULL)
        {
            if( _tcslen( p ) )
			{
#if defined( _UNICODE )		
				wcstombs(addr, p, _tcslen(p));
#else // #if defined( _UNICODE )	
				strcpy(addr, p);
#endif // #if defined( _UNICODE )

				psCfg->lNameServer2 = inet_addr(addr);
			}
        }
    }
    
    DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Name Server: %s, Name Server 2: %s", socketGetAddressString(psCfg->lNameServer), socketGetAddressString(psCfg->lNameServer2));

    /*
    ** For Windows NT 4.0, the default gateway address is stored in the
    ** registry under the TCP/IP adapter.  First, go to the network cards
    ** key to enumerate the adapters.
    */
    if( RegOpenKeyExA( HKEY_LOCAL_MACHINE,
                     _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards"),
                     0,
                     KEY_READ,
                     &hKey ) != ERROR_SUCCESS )
        return FALSE;

    /* find the number of subkeys (i.e. number of network cards) */
    if (RegQueryInfoKey(hKey, NULL, NULL, NULL,
                        &dwSubkeys,               /* number of subkeys */
                        NULL, NULL, NULL, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
    {
        RegCloseKey( hKey );
        return FALSE;
    }

    if (dwSubkeys == 0)
    {
        DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "No network cards found!");
        RegCloseKey( hKey );
        return FALSE;
    }

    i = 0;
    /* Continue enumerating the network cards, until we find one which has a TCP/IP port */
    while ((RegEnumKey(hKey, i, szStr, 255) != ERROR_NO_MORE_ITEMS) && 
           (gnHostIPAddrCount < MAX_HOST_ADDRESSES))
    {
        /* Open the network card key under services */
        if (RegOpenKeyExA(hKey, szStr, 0, KEY_READ, &hNetCardKey) == ERROR_SUCCESS)
        {
            lType = REG_SZ;
            _tcscpy((TCHAR*)szStr, _T("System\\CurrentControlSet\\Services\\"));
            lSize = (sizeof(szStr)/sizeof(TCHAR))-(unsigned long)_tcslen((const TCHAR*)szStr);
            if (RegQueryValueExA(hNetCardKey, _T("ServiceName"), NULL, &lType, (UINT8*)&szStr[_tcslen((const char*)szStr)], &lSize) == ERROR_SUCCESS)
            {
                /* See if there is a TCP/IP key under the service */
                _tcscat((TCHAR*)szStr, _T("\\Parameters\\Tcpip"));
                if( RegOpenKeyExA( HKEY_LOCAL_MACHINE, szStr, 0, KEY_READ, &hTcpipKey ) == ERROR_SUCCESS )
                {
                    /* Found a TCP/IP key, read the default gateway */
                    lType = REG_BINARY;
                    lSize = sizeof(szStr);

                    if (GetTcpIPRegistryAddressValue(hTcpipKey, _T("IPAddress"), _T("DhcpIPAddress"), &lAddress))
                    {
                        /* found the correct address, get the rest of the parameters */
                        if (psCfg->lIpAddr == lAddress)
                        {
                            GetTcpIPRegistryAddressValue(hTcpipKey, _T("SubnetMask"), _T("DhcpSubnetMask"), &psCfg->lSubnetMask);
                            GetTcpIPRegistryAddressValue(hTcpipKey, _T("DefaultGateway"), _T("DhcpDefaultGateway"), &psCfg->lGatewayAddr);
                            GetDhcpState(hTcpipKey, &bDhcpState);

                            DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "IP Address for Network Card %d: %s", gnClaimedHostIPAddrCount+1, socketGetAddressString(psCfg->lIpAddr));
                            DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Subnet Mask for Network Card %d: %s", gnClaimedHostIPAddrCount+1, socketGetAddressString(psCfg->lSubnetMask));
                            DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Default Gateway for Network Card %d: %s", gnClaimedHostIPAddrCount+1, socketGetAddressString(psCfg->lGatewayAddr));
                            DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "DHCP State for Network Card %d: %d", gnClaimedHostIPAddrCount+1, bDhcpState);

                            RegCloseKey( hTcpipKey );
                            RegCloseKey(hNetCardKey);
                            RegCloseKey( hKey );
                            
						    if (bDhcpState)
	                             psCfg->lConfigControl |= TCPIP_CONTROL_DHCP;
                            else
                                 psCfg->lConfigControl |= TCPIP_CONTROL_USE_STORED_CONFIG;
							
							return TRUE;
                        }
                    }

                    RegCloseKey( hTcpipKey );
                }

            }

            RegCloseKey(hNetCardKey);
        }

        i++;
    }

    RegCloseKey( hKey );
    return FALSE;
}

/*---------------------------------------------------------------------------
** platformGetMacID()
**
** Get Mac ID from the Netbios API
**---------------------------------------------------------------------------
*/

BOOL GetMacIDViaNetbios(UINT32 lAddress, UINT8* pMacId)
{
	ASTAT Adapter;
	NCB Ncb;  
	UCHAR uRetCode;
	LANA_ENUM   lenum;
	UINT32 i;
    UINT32* pIP;

	/* Init to zeroes, in case we fail */
	memset( pMacId, 0, MAC_ADDR_LENGTH );

	/* Use the netbios interface to get the mac address
	** See microsoft's knowledge base article ID: Q118623 */
	memset( &Ncb, 0, sizeof(Ncb) );
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);
	uRetCode = Netbios( &Ncb );
	if( uRetCode != NRC_GOODRET )
		return FALSE;

	/* find the index of the IP address */
	for ( i = 0, pIP = galHostIPAddr;
		  i < gnHostIPAddrCount; 
		  i++, pIP++)
	{
		if (*pIP == lAddress)
			break;
	}

	/* IP address not found */
	if (i == gnHostIPAddrCount)
		return FALSE;

	/* use ip index for mac id */
	memset( &Ncb, 0, sizeof(Ncb) );
	Ncb.ncb_command = NCBRESET;
	Ncb.ncb_lana_num = lenum.lana[i];
	uRetCode = Netbios( &Ncb );
	if( uRetCode != NRC_GOODRET )
		return FALSE;

	memset( &Ncb, 0, sizeof (Ncb) );
	Ncb.ncb_command = NCBASTAT;
	Ncb.ncb_lana_num = lenum.lana[i];
	strcpy( (char*)Ncb.ncb_callname,  "*              " );
	Ncb.ncb_buffer = (unsigned char *) &Adapter;
	Ncb.ncb_length = sizeof(Adapter);
	uRetCode = Netbios( &Ncb );
	if( uRetCode != NRC_GOODRET )
		return FALSE;

	memcpy( (void*)pMacId, Adapter.adapt.adapter_address, MAC_ADDR_LENGTH );
	return TRUE;
}


/*---------------------------------------------------------------------------
** GetTcpIPRegistryAddressValue( )        Private function used only within this module
**
** Read TCP/IP address string from the registry.
** hKey is the TCP/IP registry key.
** strValue1 is the first value to try to get the address (Used with DHCP disabled)
** strValue2 is the second value to try to get the address (Used with DHCP enabled)
** pValue is returned IP address value
**---------------------------------------------------------------------------
*/

BOOL GetTcpIPRegistryAddressValue(HKEY hKey, TCHAR* strValue1, TCHAR* strValue2, UINT32* pValue)
{
    TCHAR szStr[256];
    unsigned long lType = REG_BINARY,
                  lSize = sizeof(szStr);

    if( RegQueryValueEx( hKey, strValue1, NULL, &lType, (UINT8*)szStr, &lSize ) == ERROR_SUCCESS )
    {
        /* strValue1 is empty, try strValue2 */
        if ((_tcslen(szStr) == 0) || (szStr[0] == '0'))
        {
            lSize = sizeof(szStr);

            if (RegQueryValueEx( hKey, strValue2, NULL, &lType, (UINT8*)szStr, &lSize ) != ERROR_SUCCESS )
                return FALSE;

            if ((_tcslen(szStr) == 0) || (szStr[0] == '0'))
                return FALSE;
        }
    }
    else if (strValue2 != NULL)
    {
        if (RegQueryValueEx( hKey, strValue2, NULL, &lType, (UINT8*)szStr, &lSize ) != ERROR_SUCCESS )
            return FALSE;
    }
    else
    {
        return FALSE;
    }

    (*pValue) = inet_addr((const char*)szStr);
    return TRUE;
}


/*---------------------------------------------------------------------------
** GetRegistryString( )        Private function used only within this module
**
** Read TCP/IP string from the registry.
** regValue is a string with the value we need to obtain.
** dest points to the destination buffer where we need to store the string.
** nDestSize is the size of the dest buffer.
**---------------------------------------------------------------------------
*/

void GetRegistryString(TCHAR* regValue, UINT8* dest, INT32 nDestSize )
{
   HKEY   hKey;
   unsigned long lType;
   TCHAR  str[256];
   unsigned long lSize;

   dest[0] = 0;    /* Init with an empty string */

   if(RegOpenKeyExA( HKEY_LOCAL_MACHINE,
                    _T("System\\CurrentControlSet\\Services\\Tcpip\\Parameters"),
                    0,
                    KEY_READ,
                    &hKey ) == ERROR_SUCCESS )
   {
      /* Read the value string now */
      lType = REG_SZ;
      lSize = nDestSize;
      RegQueryValueEx( hKey, regValue, NULL, &lType, dest, &lSize );

      /* If string not there, then check the corresponding DHCP string */
      if( dest[0] == 0 )
      {
         /* Allocate memory for a dhcp string, and create the Dhcp string to
         ** query the Dhcp registry value */
         _tcscpy(str, _T("Dhcp"));
         _tcscat(str, regValue );

         lSize = nDestSize;
         RegQueryValueEx( hKey, str, NULL, &lType, dest, &lSize );
      }

      RegCloseKey( hKey );
   }
}


/*---------------------------------------------------------------------------
** GetDhcpState( )    Private function used only within this module
**
** hKey is the TCP/IP registry key.
** pDhcpState is returned DHCP State value
**---------------------------------------------------------------------------
*/

void GetDhcpState(HKEY hKey, BOOL* pDhcpState)
{
    unsigned long lSize = REG_DWORD;
    unsigned long lType = sizeof(BOOL);
    (*pDhcpState) = 0;

    RegQueryValueEx( hKey, _T("EnableDHCP"), NULL, &lType, (UINT8*)pDhcpState, &lSize);
}


TCHAR* GetLoggingDirectory()
{
	HKEY FileKey;
	static TCHAR FileString[1000];
	TCHAR TempFileString[1000];
	DWORD FileStringLen = 1000;

	//default logging directory
	_tcscpy(FileString, _T("c:\\et_ip\\"));

	if (!RegOpenKeyExA(HKEY_CURRENT_USER, _T("Software\\Pyramid Solutions\\EtherNetIP\\Settings"), 0, KEY_QUERY_VALUE, &FileKey))
	{
		if (!RegQueryValueExA(FileKey, _T("LoggingDirectory"), NULL, NULL, (LPBYTE)TempFileString, &FileStringLen))
		{
			_tcscpy(FileString, TempFileString);
			if (TempFileString[_tcslen(TempFileString)-1] != '\\')
				_tcscat(FileString, _T("\\"));
		}

		RegCloseKey(FileKey);
	}

	return FileString;
}