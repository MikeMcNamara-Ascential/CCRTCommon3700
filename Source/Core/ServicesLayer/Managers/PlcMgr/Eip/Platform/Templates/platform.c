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
** Platform Template 
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

FILE        *fLogFile;

/*---------------------------------------------------------------------------
** platformInitLogFile( )
**
** Creates a log file.
**---------------------------------------------------------------------------
*/

void platformInitLogFile(void)
{

    /*
	** This function is called during intiallization.
	**
	** It must open a log file (e.g. fLogFile) for future use by the  
	** platformWriteLog() function.
	*/

}

/*---------------------------------------------------------------------------
** platformCloseLogFile()
**
** Closes the log file
**---------------------------------------------------------------------------
*/

void platformCloseLogFile(void)
{

    /* 
	** This function is called when the stack is shutting down 
	** (i.e. clientStop() has been called) 
	**
	** It must close the log file opened by platformInitLogFile().
	*/

}

/*---------------------------------------------------------------------------
** platformWriteLog()
**
** Writes a record to the log file
**---------------------------------------------------------------------------
*/

void platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp)
{

	/*
	** This function is called from a variety of places in the stack
	** during execution to write various warnings and data to the log
	** file.
	**
	** The szBuf parameter points to a NULL terminated string containing
	** the ASCII formatted data to be written to the log file.
	**
	** The bTimeStamp parameter indicates whether the stack suggests a 
	** time stamp should be prepended to the log entry.  The general rule 
	** adhered to within the stack is that raw data dumps (in hex ASCII 
	** format) will call this funtion with bTimeStamp == FALSE. Other 
	** notifications call this function with bTimeStamp == TRUE.
	**
	** In practice any log format desired can be accomodated here.
	*/

}

/*---------------------------------------------------------------------------
** platformSocketLibInit( )
**
** Initialize the sockets library.
**---------------------------------------------------------------------------
*/

void platformSocketLibInit(void)
{
    
	/*
	** This function is called during intiallization to allow for 
	** of the platform's sockets library if required (as it is on the
	** Windows platform).
	**
	** If no such initiallization is required, simply return.
	*/

}

/*---------------------------------------------------------------------------
** platformSocketLibCleanup( )
**
** Unload Winsock
**---------------------------------------------------------------------------
*/

void platformSocketLibCleanup()
{     
	
    /*
	** This function is called during shutdown to shutdown the platform's 
	** sockets library if required (as it is on the Windows platform).
	**
	** If no such shutdown is required, simply return.
	*/
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
	** Please see the EtherNet/IP protocol specification for further information.,
	*/

	return FALSE;
}

/*---------------------------------------------------------------------------
** platformTestSetTcpIpCfgData()
**
** Ensure a particular TCP/IP object attribute can be set before it's 
** actually set. Return FALSE if error is generated.
**---------------------------------------------------------------------------
*/
BOOL platformTestSetTcpIpCfgData(UINT32 lMask, TCPIP_CONFIG_DATA *psCfg, UINT32 lCurrentIP, UINT8* pbGeneralError)
{
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
}

/*---------------------------------------------------------------------------
** platformSocketInitTargetData( )
**
** Initialize galHostAddr array. It is passed as a response to the 
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
 
}

/*---------------------------------------------------------------------------
** platformGetCurrentTickCount( )    
**
** Return current msec counter using a high-res performance timer.
**---------------------------------------------------------------------------
*/
UINT32 platformGetCurrentTickCount()
{
    /*
	** This function is called periodically during execution of the stack to
	** retrieve the system clock in milliseconds.
	*/
}

/*---------------------------------------------------------------------------
** platformGetCurrentAbsTime( )
**
** This retrieves time in seconds from Jan 1, 1970 and 
** current millisecond (0-1000)
**---------------------------------------------------------------------------
*/
void platformGetCurrentAbsTime(UINT32* plSecTime, UINT16* puMillisecTime)
{

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
}

/*---------------------------------------------------------------------------
** platformCopySockAddrInToBuffer( )
**
** Populates an EIP "sockaddr_in" buffer with a sockaddr_in structure.
** 
**---------------------------------------------------------------------------
*/
void platformCopySockAddrInToBuffer( UINT8* pBuffer, struct sockaddr_in* pSockAddr )
{
}

/*---------------------------------------------------------------------------
** platformCopySockAddrInFromBuffer( )
**
** Populates a sockaddr_in structure with an EIP "sockaddr_in" buffer from
** a packet.
** 
**---------------------------------------------------------------------------
*/
void platformCopySockAddrInFromBuffer( struct sockaddr_in* pSockAddr, UINT8* pBuffer )
{
}

#ifdef EIP_ACD
/*---------------------------------------------------------------------------
** platformIsDuplicateIPAddress( )    
**
** Determines if the current IP address used by the EtherNet/IP stack is a
** duplicate IP address on the network.  Requires Address Conflict Detection
** support (#define EIP_ACD) from the platform
**---------------------------------------------------------------------------
*/
BOOL platformIsDuplicateIPAddress(UINT32 lIPAddress)
{
	return FALSE;
}

#endif

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
	/* 
	** This function is called when a host IP address is claimed by the client
	** application.
	**
	** It should populate the pEnetLinkAttr structure with
	** the Ethernet Link attributes for the local network interface
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
	** Please see the EtherNet/IP protocol specification for further information.
	*/
	
}

/*---------------------------------------------------------------------------
** platformSetEnetLinkAttributes( )    
**
** Store settable Ethernet Link attributes 
**---------------------------------------------------------------------------
*/
void platformSetEnetLinkAttributes(UINT32 lMask, UINT32 iInstance, ENETLINK_INST_ATTR* pEnetLinkAttr)
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
	/* Default to accept */
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
	return SOCKET_ERROR;
}

#endif /* EIP_QOS */
