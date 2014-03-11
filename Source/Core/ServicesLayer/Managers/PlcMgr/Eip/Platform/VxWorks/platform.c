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
** Platform dependant code for VxWorks environments.
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
#include <net/if.h>
#include <net/if_types.h>
#include <ipnet/ipioctl.h>
#include "eipinc.h"

/*
** Private functions prototypes
*/

/*---------------------------------------------------------------------------
** platformWriteLog()
**
** Writes a record to the log file
**---------------------------------------------------------------------------
*/

void platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp)
{
#if defined(TRACE_OUTPUT)

	static char szTimeStamp[64];
	static char szSrcIp[20], szDstIp[20];
	struct timespec tspec;

	/* always use timestamp */
	bTimeStamp = bTimeStamp;
	/* iSeverity not included in log statement */
	iSeverity = iSeverity;
    
	clock_gettime(CLOCK_REALTIME, &tspec);
	sprintf(szTimeStamp, "%5ld.%03ld ", tspec.tv_sec, tspec.tv_nsec/1000/1000);

	if ((lSrcIp != 0) || (lDstIp != 0))
	{
		strcpy(szSrcIp, socketGetAddressString(lSrcIp));
		strcpy(szDstIp, socketGetAddressString(lDstIp));
		printf("%c %s (%s -> %s) %s\n", traceGetTypeAsChar(iType), szTimeStamp, szSrcIp, szDstIp, szBuf);
	}
	else
		printf("%c %s %s\n", traceGetTypeAsChar(iType), szTimeStamp, szBuf);

#endif /* #if defined(TRACE_OUTPUT) */
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
void platformGetEnetLinkAttributes(UINT32 iInstance, UINT32 lIPAddress, ENETLINK_INST_ATTR* pEnetLinkAttr)
{
	/* 
	** This function is called when a CIP message wants to set Ethernet Link attributes.
	**
	** It should use the pEnetLinkAttr structure with
	** the Ethernet Link attributes for the local network interface defined
	** for iInstance
	**
	** The EnetLinkConfig structure is defined in eiptcpip.h as
	**
	**	typedef struct tagENETLINK_INST_ATTR
	**	{
    **	UINT32   lInterfaceSpeed;
    **	UINT32   lInterfaceFlags;
    **	UINT8    macAddr[MAC_ADDR_LENGTH];
	**	UINT8	 bInterfaceType;
	**	UINT8	 bInterfaceState;
	**	UINT8	 bAdminState;
	**	INT8     InterfaceLabel[ENETLINK_INTERFACE_MAX_LABEL_SIZE];
	**	}
	**	ENETLINK_INST_ATTR;
	**
	** lMask determines which attributes are being set (bit flag)
	**
	** Please see the EtherNet/IP protocol specification for further information.
	*/

	/* If product can be accessed via a CIP port other than TCP/IP, then
    ** the link active bit must be determined.  If product only responds
    ** to one source of CIP requests (Ethernet port e.g.), then we can
    ** assume that it is active, since we got this request */
    pEnetLinkAttr->lInterfaceFlags = ENETLINK_FLAG_LINK_ACTIVE | ENETLINK_FLAG_SPEED_DUPLEX_FINE;

    /* The interface flags and interface speed must be determined
    ** from your hardware interface */
    pEnetLinkAttr->lInterfaceFlags = 0;
    pEnetLinkAttr->lInterfaceSpeed = 0;
	pEnetLinkAttr->iInterfaceControlBits = 0;
	pEnetLinkAttr->iForcedInterfaceSpeed = 0;

	/* Retrieving MAC ID BSP specific */
	sysEnetAddrGet(0, pTcpipConfig->EnetLinkConfig.macAddr);

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
void platformSetEnetLinkAttributes(UINT32 lMask, UINT32 iInstance, UINT32 lIPAddress, ENETLINK_INST_ATTR* pEnetLinkAttr)
{
	/* 
	** This function is called when a CIP message wants to set Ethernet Link attributes.
	**
	** It should use the pEnetLinkAttr structure with
	** the Ethernet Link attributes for the local network interface defined
	** for iInstance
	**
	** The EnetLinkConfig structure is defined in eiptcpip.h as
	**
	**	typedef struct tagENETLINK_INST_ATTR
	**	{
    **	UINT32   lInterfaceSpeed;
    **	UINT32   lInterfaceFlags;
    **	UINT8    macAddr[MAC_ADDR_LENGTH];
	**	UINT8	 bInterfaceType;
	**	UINT8	 bInterfaceState;
	**	UINT8	 bAdminState;
	**	INT8     InterfaceLabel[ENETLINK_INTERFACE_MAX_LABEL_SIZE];
	**	}
	**	ENETLINK_INST_ATTR;
	**
	** lMask determines which attributes are being set (bit flag)
	**
	** Please see the EtherNet/IP protocol specification for further information.
	*/

	/**************** BSP specific ******************/
}

BOOL platformTestSetEnetLinkAttributes(UINT32 lMask, UINT32 iInstance, UINT32 lIPAddress, ENETLINK_INST_ATTR* pEnetLinkAttr, UINT8* pbGeneralError)
{
	/**************** BSP specific ******************/

	/* Default to accept */
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
	**		BOOL     bDhcpState;
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

	/**************** BSP specific ******************/

	psCfg->lConfigControl = TCPIP_CONTROL_USE_STORED_CONFIG;
	psCfg->lNameServer = 0;
	psCfg->lNameServer2 = 0;
	psCfg->lGatewayAddr = glDefaultGateway;
	psCfg->lSubnetMask = glSubNetMask;
	strcpy(psCfg->szDomainName, "");

    /* Because this is a multiport device, there isn't a single path associated 
	** with the underlying physical communication interface 
	** (e.g., an [F6] Ethernet Link Object Class), so set it to 0
	*/
    psCfg->iLinkObjPathSize = 0;

	/* Set the User-defined bitmaps used for network configuration options
    ** These should be or'd values of the TCPIP_CAPABILITY_xxx and TCPIP_CONTROL_xxx
    ** literals */
    psCfg->lConfigCapability = TCPIP_CAPABILITY_DHCP | TCPIP_CAPABILITY_SETTABLE_CONFIG
							  | TCPIP_CAPABILITY_HARDWARE_CONFIG | TCPIP_CAPABILITY_IP_RESET_NEEDED;

	/* Setup the multicast parameters - hard coded and unchangable by SE choice */
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
	/**************** BSP specific ******************/

	/* Default to accept */
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
	** lMask determines which attributes are being set (bit flag)
	**
	** Please see the EtherNet/IP protocol specification for further information.
	*/
	
	if (lMask & (1 << TCPIP_INST_ATTR_INTERFACE_CONFIG))
	{
		/**************** BSP specific ******************/
	}

	if (lMask & (1 << TCPIP_INST_ATTR_HOST_NAME))
	{
		/**************** BSP specific ******************/
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

	/**************** BSP specific ******************/
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

	/**************** BSP specific ******************/

	/* setting QoS data not supported */
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
	/**************** BSP specific ******************/

	return 0;
}

#endif /* EIP_QOS */




/*---------------------------------------------------------------------------
** platformSocketInitTargetData( )
**
** Initialize gHostAddr array. It is passed as a response to the
** List Targets UCMM request.
**---------------------------------------------------------------------------
*/
void platformSocketInitTargetData( void )
{
	/* 
	** This function is called during initiallization.
	** 
	** Upon return, it should have placed the number of IP addresses supported
	** by the device (i.e. the number of active Ethernet interfaces) in the 
	** global gnHostIPAddrCount.
	**
	** In addition, it should have placed each supported Ethernet address 
	** as a UINT32) in the global array galHostIPAddr[MAX_HOST_ADDRESSES].  
	*/    

	/**************** BSP specific ******************/

    memcpy(&galHostIPAddr[0], &glIpAddress, sizeof(UINT32));
    gnHostIPAddrCount = 1;
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
    memset(pSockaddr, 0, sizeof( struct sockaddr_in ) );
    pSockaddr->sin_len = sizeof( struct sockaddr_in );
    pSockaddr->sin_family = AF_INET;
    pSockaddr->sin_port = htons( iPort );
    platformGetInternetAddress(pSockaddr->sin_addr ) = lIpAddress;
}

/*---------------------------------------------------------------------------
** platformCopySockAddrInToBuffer( )
**
** Populates an EIP "sockaddr_in" buffer with a sockaddr_in structure.
** 
** CIP assumes the structure is:
** 
** struct sockaddr_in
** {
**      int16   sin_family;     - Address family (big-endian)
**      uint16 	sin_port;       - Address Port (big-endian)
**      uint32  sin_addr;       - IP Address (big_endian)
**      int8   	sin_zero[8];    - padding
** };
** 
** VxWorks definition is:
** 
** struct sockaddr_in
** {
**      u_char  sin_len;
**      u_char  sin_family;
**      u_short sin_port;
**      struct  in_addr sin_addr;	- effectivly a uint32
**      char    sin_zero[8];
**  };
** 
** This function copies the entire structure and then adjustes the first 2
** bytes to match the EIP expectations
**---------------------------------------------------------------------------
*/
void platformCopySockAddrInToBuffer( UINT8* pBuffer, struct sockaddr_in* pSockAddr )
{
    /* Copy the entire structure */
    memcpy(&pBuffer[2], &(pSockAddr->sin_port), sizeof( struct sockaddr_in ) - 2 );
    
    /* Adjust the 1st two bytes */
	pBuffer[ 0 ] = 0;
	pBuffer[ 1 ] = pSockAddr->sin_family;
}

/*---------------------------------------------------------------------------
** platformCopySockAddrInFromBuffer( )
**
** Populates a sockaddr_in structure with an EIP "sockaddr_in" buffer from
** a packet.
** 
** struct sockaddr_in
** {
**      int16   sin_family;     - Address family (big-endian)
**      uint16 	sin_port;       - Address Port (big-endian)
**      uint32  sin_addr;       - IP Address (big_endian)
**      int8   	sin_zero[8];    - padding
** };
** 
** VxWorks definition is:
** 
** struct sockaddr_in
** {
**      u_char  sin_len;
**      u_char  sin_family;
**      u_short sin_port;
**      struct  in_addr sin_addr;	- effectivly a uint32
**      char    sin_zero[8];
**  };
** 
** This function copies the entire structure and then adjustes the first 2
** elements to match the VxWorks expectations
**---------------------------------------------------------------------------
*/
void platformCopySockAddrInFromBuffer( struct sockaddr_in* pSockAddr, UINT8* pBuffer )
{
    /* Copy the entire structure */
    memcpy(&(pSockAddr->sin_port), &pBuffer[2], sizeof( struct sockaddr_in ) - 2 );
    
    /* Adjust 1st two elements */
	pSockAddr->sin_len = sizeof( struct sockaddr_in );
	pSockAddr->sin_family = pBuffer[ 1 ];
}


void platformGetCurrentAbsTime(UINT32* plSecTime, UINT16* puMillisecTime)
{
	struct timespec tspec;

	clock_gettime(CLOCK_REALTIME, &tspec);
	
	*plSecTime = tspec.tv_sec;
	*puMillisecTime = tspec.tv_nsec/1000000;
}