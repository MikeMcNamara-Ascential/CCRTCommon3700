/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** PLATFORM.C
**
** COPYRIGHT (c) 2003 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Platform dependant code for Linux environments.
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
#include "eipinc.h"


/*
** Private variable declarations
*/
FILE        *fLogFile;

/*
** Private functions prototypes
*/
static char* ParseFileLine( char *szData, char *szStr );
static BOOL platformGetMacID(UINT32 lAddress, UINT8* pMacId);

/* Local IP address is always 127.0.0.1 */
#define             LOCAL_IP_ADDRESS	0x100007F


/*---------------------------------------------------------------------------
** platformSleep( )
**
** Approximate a sleep for the client thread.
**---------------------------------------------------------------------------
*/

void platformSleep(int ticks)
{
	usleep(ticks * 1000);
}

/*---------------------------------------------------------------------------
** platformGetCurrentTickCount( )
**
** Get the system time.
**---------------------------------------------------------------------------
*/

UINT32 platformGetCurrentTickCount(void)
{
	UINT32 lTimeVal;
	struct timeval sTimeValue;

	gettimeofday(&sTimeValue,NULL);
	lTimeVal = ((sTimeValue.tv_sec * 1000) + (sTimeValue.tv_usec / 1000));

	return(lTimeVal);
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
	pEnetLinkAttr->lInterfaceSpeed = 0;								/* 0 means "speed not known" */

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
	/* Linux has 1 to 1 relationship between TCP/IP object instance and Ethernet Link instance, 
	   so admin state can't be diabled */
	if(lMask & (1 << ENETLINK_INST_ATTR_ADMIN_STATE))
	{
		*pbGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		return FALSE;
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** platformGetTcpIpCfgData()
**
** Get the TCP/IP configuration data from the system
**---------------------------------------------------------------------------
*/
BOOL platformGetTcpIpCfgData( TCPIP_CONFIG_DATA *psCfg)
{
	struct ifreq *ifrp;
	int sockfd;
	int i, iInterfaces;
	struct ifconf ifc;
	struct sockaddr_in *addr;
	char buf[sizeof(struct ifreq)*MAX_HOST_ADDRESSES];
	FILE *f;
	char *p;
	char szTmp[500];
	UINT32 lTempGateway;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;
	char str[512];

	/* First see if the IP address is already claimed.  If it's claimed, consider the 
	   TCP/IP config data to be initialized and just return the current data */
	for( i = 0, pTcpipInterface = gsTcpIpInstAttr; 
	   i < gnClaimedHostIPAddrCount; 
	   i++, pTcpipInterface++)
	{
		if(pTcpipInterface->InterfaceConfig.lIpAddr == psCfg->lIpAddr)
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

	// we need a socket to call ioctl() on
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if( sockfd < 0 )
		return FALSE;

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;

	if(ioctl(sockfd, SIOCGIFCONF, (caddr_t)&ifc) < 0)
		return FALSE;

	iInterfaces = ifc.ifc_len/sizeof(struct ifreq);

	for(i = 0, ifrp = ifc.ifc_req; 
	   i < iInterfaces; i++, ifrp++)
	{
		if((ifrp->ifr_addr.sa_family == AF_INET) ||
		   (ifrp->ifr_addr.sa_family == AF_INET6))
		{
			addr = (struct sockaddr_in *)&(ifrp->ifr_addr);

			//match the IP address
			if(platformGetInternetAddress(addr->sin_addr) == psCfg->lIpAddr)
				break;
		}
	}

	//couldn't find IP address
	if(i == iInterfaces)
		return FALSE;

	if(ioctl(sockfd, SIOCGIFNETMASK, ifrp) >= 0)
	{
		addr = (struct sockaddr_in *)&(ifrp->ifr_netmask);
		psCfg->lSubnetMask = platformGetInternetAddress(addr->sin_addr);
	}

	platformCloseSocket(sockfd);

	// Open and parse the resolv.conf file to get the domain name and domain name server addresses
	if((f = fopen("/etc/resolv.conf","r")) != NULL)
	{
		//Walk through the file to find the domain name and and domain name server entries
		while( fgets( szTmp, sizeof(szTmp), f) != NULL )
		{
			if((p = ParseFileLine(szTmp,"domain")) != NULL)	// Got the Domain name
			{
				// Strip the CR/LF if it exists
				if(*(p + strlen(p) - 1) == '\n') *(p + strlen(p) - 1) = 0;

				strncpy(psCfg->szDomainName,p, TCPIP_MAX_DOMAIN_LENGTH);
			}
			else if((p = ParseFileLine(szTmp,"nameserver")) != 0) // Got a Domain Name Server address
			{
				if(psCfg->lNameServer == 0)
					psCfg->lNameServer = inet_addr(p);
			}
		}

		fclose(f);

	} /* end of resolv.conf file parsing */
	else
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "platformGetTcpIpCfgData unable to open /etc/resolv.conf file");
	}

	/* User netstat to get gateway address */
	if(system("netstat -nr > tmpfile") == 0)
	{
		if((f = fopen("tmpfile","r")) != NULL)
		{
			/* the format of the file is as follows:
			 * Line 1 = Header information (Destination, Gateway, etc)
			 * Line 2-n = Destination addr, gateway addr, etc
			 */

			/* skip first line */
			if(fgets( szTmp, sizeof(szTmp), f) != NULL)
			{
				while(fgets( szTmp, sizeof(szTmp), f) != NULL)
				{
					p = strtok(szTmp, "\n\r\t ");
					if(p == NULL)
						continue;

					DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Dest addr = %s", p);

					p = strtok(NULL, "\n\r\t ");
					if(p == NULL)
						continue;

					DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Gateway addr = %s", p);

					lTempGateway = inet_addr(p);
					if((lTempGateway != 0) && (lTempGateway != INADDR_NONE))
					{
						psCfg->lGatewayAddr = lTempGateway;
						break;
					}
				}
			}

			fclose(f);
		}
	}
	else
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "platformGetTcpIpCfgData unable to execute netstat -nr");
	}

	/* Get the host name of the local machine (or device) */
	if(gethostname((char*)str, TCPIP_MAX_HOST_LENGTH) != SOCKET_ERROR )
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
	/* While Linux has the ability to switch between static and DHCP IP addresses, 
	   an API to accomplish this through CIP is not provided, so from a CIP perspective
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
	psCfg->LinkObjPath[3] = (pTcpipInterface-gsTcpIpInstAttr)+1;	/* Instance based off of number of TCP/IP interfaces */

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
	/* not supported in Linux */
	if((lMask & (1 << TCPIP_INST_ATTR_HOST_NAME)) ||
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
	** TCP/IP configuration data (if supported).
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
	for( i = 0, pTcpipInterface = gsTcpIpInstAttr; 
	   i < gnClaimedHostIPAddrCount; 
	   i++, pTcpipInterface++)
	{
		if(pTcpipInterface->InterfaceConfig.lIpAddr == psCfg->lIpAddr)
			break;
	}

	if(i >= gnClaimedHostIPAddrCount)
		return;

	/* Currently configured as volatile for life of program */
	if(lMask & (1 << TCPIP_INST_ATTR_TTL_VALUE))
	{
		pTcpipInterface->InterfaceConfig.bTTLValue = psCfg->bTTLValue;
	}

	if(lMask & (1 << TCPIP_INST_ATTR_MCAST_CONFIG))
	{
		pTcpipInterface->InterfaceConfig.MulticastData.bAllocControl = psCfg->MulticastData.bAllocControl;
		pTcpipInterface->InterfaceConfig.MulticastData.bReserved = psCfg->MulticastData.bReserved;
		pTcpipInterface->InterfaceConfig.MulticastData.iNumMcast = psCfg->MulticastData.iNumMcast;
		pTcpipInterface->InterfaceConfig.MulticastData.lMcastStartAddr = psCfg->MulticastData.lMcastStartAddr;
	}
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
	UINT32 i;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;

	/* First see if the IP address is already claimed.  If it's claimed, consider the 
	   TCP/IP config data to be initialized and just return the current data */
	for( i = 0, pTcpipInterface = gsTcpIpInstAttr; 
	   i < gnClaimedHostIPAddrCount; 
	   i++, pTcpipInterface++)
	{
		if(pTcpipInterface->InterfaceConfig.lIpAddr == pTcpipConfig->InterfaceConfig.lIpAddr)
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

	if(lMask & (1 << QOS_INST_ATTR_8021Q_TAG_ENABLE))
	{
		/* 802.1Q Tag frames not supported */
		return FALSE;
	}


	/* Find the claimed TCP/IP instance and save the QoS data for the life 
	   of the program since we don't have NVS support */
	for( i = 0, pTcpipInterface = gsTcpIpInstAttr; 
	   i < gnClaimedHostIPAddrCount; 
	   i++, pTcpipInterface++)
	{
		if(pTcpipInterface->InterfaceConfig.lIpAddr == pTcpipConfig->InterfaceConfig.lIpAddr)
		{

			if(lMask & (1 << QOS_INST_ATTR_DSCP_PTP_EVENT))
				pTcpipInterface->QoSConfig.bDSCPPTPEvent = pTcpipConfig->QoSConfig.bDSCPPTPEvent;
			if(lMask & (1 << QOS_INST_ATTR_DSCP_PTP_GENERAL))
				pTcpipInterface->QoSConfig.bDSCPPTPGeneral = pTcpipConfig->QoSConfig.bDSCPPTPGeneral;
			if(lMask & (1 << QOS_INST_ATTR_DSCP_URGENT))
				pTcpipInterface->QoSConfig.bDSCPUrgent = pTcpipConfig->QoSConfig.bDSCPUrgent;
			if(lMask & (1 << QOS_INST_ATTR_DSCP_SCHEDULED))
				pTcpipInterface->QoSConfig.bDSCPScheduled = pTcpipConfig->QoSConfig.bDSCPScheduled;
			if(lMask & (1 << QOS_INST_ATTR_DSCP_HIGH))
				pTcpipInterface->QoSConfig.bDSCPHigh = pTcpipConfig->QoSConfig.bDSCPHigh;
			if(lMask & (1 << QOS_INST_ATTR_DSCP_LOW))
				pTcpipInterface->QoSConfig.bDSCPLow = pTcpipConfig->QoSConfig.bDSCPLow;
			if(lMask & (1 << QOS_INST_ATTR_DSCP_EXPLICIT))
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
	iRet = setsockopt(s, IPPROTO_IP, SO_PRIORITY, &bDSCPValue, sizeof(bDSCPValue));
	if(iRet == SOCKET_ERROR)
		DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "platformSetDSCPValue failed calling setsockopt() for socket 0x%x.  Error code = %d", s, platformGetLastError());

	return 0;
}

#endif /* EIP_QOS */

/*---------------------------------------------------------------------------
** ParseFileLine()
**
** Private function for parsing Linux config file lines. Returns a pointer to
** the first non-whitespace character within szData after the token szStr
**---------------------------------------------------------------------------
*/

char* ParseFileLine( char *szData, char *szStr )
{

	char *p;

	/*
	** See if szStr is present within szData.  Return NULL if it isn't.
	*/

	if((p = strstr(szData,szStr)) == NULL)
	{
		return(NULL);
	}

	/*
	** Jump over the search token within szData, find the next occurence
	** of a non-whitespace character, and return a pointer to it.
	*/

	p += strlen(szStr);

	while((*p == '\t') || (*p == ' ')) ++p;
	return(p);

}

/*---------------------------------------------------------------------------
** platformInitMutex()
**
** Create and initialize a Mutex
**---------------------------------------------------------------------------
*/

PLATFORM_MUTEX_TYPE platformInitMutex(char *strMutexName)
{

	PLATFORM_MUTEX_TYPE pMutex;

	pMutex = malloc(sizeof(pthread_mutex_t));

	if(pMutex != NULL)
	{
		pthread_mutex_init(pMutex, NULL);
	}

	return(pMutex);
}

/*---------------------------------------------------------------------------
** platformDiscardMutex()
**
** Destroy a Mutex
**---------------------------------------------------------------------------
*/

void platformDiscardMutex(PLATFORM_MUTEX_TYPE xMutex)
{

	pthread_mutex_destroy( xMutex );
	free (xMutex);

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
	struct ifreq *ifrp;
	int sockfd;
	int i, iInterfaces;
	struct ifconf ifc;
	struct sockaddr_in *addr;
	char buf[sizeof(struct ifreq)*MAX_HOST_ADDRESSES];

	gnHostIPAddrCount = 0;

	// we need a socket to call ioctl() on
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if( sockfd < 0 )
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Unable to obtain socket to retrieve IP address");
		return;
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;

	if(ioctl(sockfd, SIOCGIFCONF, (caddr_t)&ifc) < 0)
	{
		platformCloseSocket(sockfd);
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Unable to obtain local IP address");
		return;
	}

	iInterfaces = ifc.ifc_len/sizeof(struct ifreq);

	for(i = 0, ifrp = ifc.ifc_req; 
	   i < iInterfaces; i++, ifrp++)
	{
		if((ifrp->ifr_addr.sa_family == AF_INET) ||
		   (ifrp->ifr_addr.sa_family == AF_INET6))
		{
			addr = (struct sockaddr_in *)&(ifrp->ifr_addr);
			//don't include "local" address
			if(platformGetInternetAddress(addr->sin_addr) != LOCAL_IP_ADDRESS)
			{
				galHostIPAddr[gnHostIPAddrCount] = platformGetInternetAddress(addr->sin_addr);
				gnHostIPAddrCount++;
			}
		}
	}

	platformCloseSocket(sockfd);

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "socketInitTargetData Total Host IP Addresses: %d", gnHostIPAddrCount);
}

/*---------------------------------------------------------------------------
** platformGetMacID()
**
** Get Mac ID from the GetAdaptersInfo
**---------------------------------------------------------------------------
*/
BOOL platformGetMacID(UINT32 lAddress, UINT8* pMacId)
{
	struct ifreq *ifrp;
	int sockfd;
	int i, iInterfaces;
	struct ifconf ifc;
	struct sockaddr_in *addr;
	char buf[sizeof(struct ifreq)*MAX_HOST_ADDRESSES];

	// we need a socket to call ioctl() on
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if( sockfd < 0 )
		return FALSE;

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;

	if(ioctl(sockfd, SIOCGIFCONF, (caddr_t)&ifc) < 0)
	{
		platformCloseSocket(sockfd);
		return FALSE;
	}

	iInterfaces = ifc.ifc_len/sizeof(struct ifreq);

	for(i = 0, ifrp = ifc.ifc_req; 
	   i < iInterfaces; i++, ifrp++)
	{
		if((ifrp->ifr_addr.sa_family == AF_INET) ||
		   (ifrp->ifr_addr.sa_family == AF_INET6))
		{
			addr = (struct sockaddr_in *)&(ifrp->ifr_addr);

			//match the IP address
			if(platformGetInternetAddress(addr->sin_addr) == lAddress)
			{
				if(ioctl(sockfd, SIOCGIFHWADDR, ifrp) < 0)
				{
					platformCloseSocket(sockfd);
					return FALSE;
				}

				memcpy(pMacId, ifrp->ifr_hwaddr.sa_data, MAC_ADDR_LENGTH);

				DumpStr7(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, 
						 "Mac id %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x for address: %s", 
						 pMacId[0], pMacId[1], pMacId[2], pMacId[3], pMacId[4], pMacId[5], 
						 socketGetAddressString(lAddress));
				platformCloseSocket(sockfd);
				return TRUE;
			}
		}
	}

	platformCloseSocket(sockfd);
	return FALSE;
}

/*---------------------------------------------------------------------------
** platformInitLogFile( )
**
** Creates a trace dump file.
**---------------------------------------------------------------------------
*/

void platformInitLogFile(void)
{
	char szFileName[128];
	FILE *tmp;
	int i;

	/* Find the first unused sequential number for creating debug file name */

	for( i = 0; i < 1000; i++ )
	{
		sprintf( szFileName, "./debug%d.txt", i);
		tmp = fopen( szFileName, "r" );
		if( !tmp ) /* File does not exist - break and use the name */
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
#if defined(TRACE_FILE_OUTPUT)
	static char szBuffer[(MAX_PACKET_SIZE*3)+1000];
	static char szTimeStamp[64];
	static char szSrcIp[20], szDstIp[20];

	struct timeb timebuffer;
	char *szTimeline;

	/* always use timestamp */
	bTimeStamp = bTimeStamp;
	/* iSeverity not included in log statement */
	iSeverity = iSeverity;

#ifdef MULTIPLE_LOG_FILES
	static UINT32 lLogCount = 0;
#endif

	ftime( &timebuffer );
	szTimeline = ctime( & ( timebuffer.time ) );
	sprintf(szTimeStamp, "%.19s.%hu %u ", szTimeline, timebuffer.millitm, gdwTickCount);

	if((lSrcIp != 0) || (lDstIp != 0))
	{
		strcpy(szSrcIp, socketGetAddressString(lSrcIp));
		strcpy(szDstIp, socketGetAddressString(lDstIp));
		sprintf(szBuffer, "%c %s (%s -> %s) %s\r\n", traceGetTypeAsChar(iType), szTimeStamp, szSrcIp, szDstIp, szBuf);
	}
	else
		sprintf(szBuffer, "%c %s %s\r\n", traceGetTypeAsChar(iType), szTimeStamp, szBuf);

	fputs(szBuffer, fLogFile);
	fflush( fLogFile );

#endif /* #if defined(TRACE_FILE_OUTPUT) */
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
	if( ( lIPAddress = inet_addr( addr ) ) == ERROR_STATUS )
	{
		if( (phost = gethostbyname(addr)) != NULL )
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