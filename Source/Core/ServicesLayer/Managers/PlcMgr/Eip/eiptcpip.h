/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** TCP/IP.H
**
** COPYRIGHT (c) 2001-2009 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** TCP/IP object 
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
**
** Porting: <none>0----<major>         Customization: <none>0----<major>
**
*****************************************************************************/


#ifndef PYRAMID_SOLUTIONS_TCPIP_H
#define PYRAMID_SOLUTIONS_TCPIP_H

#define READ_CONFIG_FROM_REGISTRY


#define TCPIP_CLASS                        0xF5            /* TCP/IP class identifier */
#define TCPIP_CLASS_REVISION            2                /* TCP/IP class revision */

/* Class and instance attribute numbers */
#define TCPIP_CLASS_ATTR_REVISION              1
#define TCPIP_CLASS_ATTR_MAX_INSTANCE          2
#define TCPIP_CLASS_ATTR_NUM_INSTANCES         3
#define TCPIP_CLASS_ATTR_OPT_ATTR_LIST         4
#define TCPIP_CLASS_ATTR_OPT_SVC_LIST          5
#define TCPIP_CLASS_ATTR_MAX_CLASS_ATTR        6
#define TCPIP_CLASS_ATTR_MAX_INST_ATTR         7

#define TCPIP_INST_ATTR_INTERFACE_STATUS       1
#define TCPIP_INST_ATTR_CONFIG_CAPABILITY      2
#define TCPIP_INST_ATTR_CONFIG_CONTROL         3
#define TCPIP_INST_ATTR_LINK_PATH              4
#define TCPIP_INST_ATTR_INTERFACE_CONFIG       5
#define TCPIP_INST_ATTR_HOST_NAME              6
#define TCPIP_INST_ATTR_TTL_VALUE			   8
#define TCPIP_INST_ATTR_MCAST_CONFIG		   9
#ifdef EIP_ACD
#define TCPIP_INST_ATTR_ACD_MODE			   10
#define TCPIP_INST_ATTR_ACD_LAST_CONFLICT	   11
#endif
#ifdef EIP_QUICK_CONNECT
#define TCPIP_INST_ATTR_QUICK_CONNECT		   12
#endif

#if defined (EIP_ACD) && defined (EIP_QUICK_CONNECT)
#define TCPIP_ATTR_OPT_COUNT				5	
#elif defined EIP_QUICK_CONNECT
#define TCPIP_ATTR_OPT_COUNT				3
#elif defined EIP_ACD
#define TCPIP_ATTR_OPT_COUNT				4
#else
#define TCPIP_ATTR_OPT_COUNT				2
#endif



/* Attribute data lengths */
#define TCPIP_MAX_DOMAIN_LENGTH       48
#define TCPIP_MAX_HOST_LENGTH         64

/* Status attribute bit-map */
#define TCPIP_STATUS_NOT_CONFIGURED			0x00000000
#define TCPIP_STATUS_VALID_CONFIGURATION	0x00000001
#define TCPIP_STATUS_VALID_CONFIG_HARDWARE	0x00000002
#define TCPIP_STATUS_MCAST_PENDING			0x00000010
#define TCPIP_STATUS_INTERFACE_CONFIG_PENDING	0x00000020
#define TCPIP_STATUS_ACD_STATUS				0x00000040


/* Capability attribute bit-map */
#define TCPIP_CAPABILITY_BOOTP				0x00000001
#define TCPIP_CAPABILITY_DNS				0x00000002
#define TCPIP_CAPABILITY_DHCP				0x00000004
#define TCPIP_CAPABILITY_DHCP_DNS			0x00000008
#define TCPIP_CAPABILITY_SETTABLE_CONFIG	0x00000010
#define TCPIP_CAPABILITY_HARDWARE_CONFIG	0x00000020
#define TCPIP_CAPABILITY_IP_RESET_NEEDED	0x00000040
#define TCPIP_CAPABILITY_ACD				0x00000080

/* Control attribute bit-map */
#define TCPIP_CONTROL_CONFIG_BITMASK       0x00000007
#define TCPIP_CONTROL_USE_STORED_CONFIG    0x00000000
#define TCPIP_CONTROL_BOOTP                0x00000001
#define TCPIP_CONTROL_DHCP                 0x00000002
#define TCPIP_CONTROL_DNS_ENABLE           0x00000010

#define TCPIP_MAX_LINK_PATH_LENGTH           12
#define TCPIP_MAX_ARP_PDU_LENGTH			 28

#define TCPIP_QUICK_CONNECT_ENABLED				1
#define TCPIP_QUICK_CONNECT_DISABLED			0

/* Class attribute structure */
typedef struct tagTCPIP_CLASS_ATTR
{
   UINT16  iRevision;
   UINT16  iMaxInstance;
   UINT16  iNumInstances;
   UINT16  iMaxClassAttr;
   UINT16  iMaxInstanceAttr;
}
TCPIP_CLASS_ATTR;

#define TCPIP_CLASS_ATTR_SIZE    10
#define TCPIP_INTERFACE_CONFIG_SIZE        20
#define TCPIP_INTERFACE_MCAST_SIZE         8

typedef struct tagTCPIP_MCAST_DATA
{
	UINT8 bAllocControl;
	UINT8 bReserved;
	UINT16 iNumMcast;
	UINT32 lMcastStartAddr;
}
TCPIP_MCAST_DATA;


/* TCP/IP Configuration DATA */
typedef struct tagTCPIP_CONFIG_DATA
{
    UINT32   lConfigCapability;
    UINT32	 lConfigControl;
    UINT16   iLinkObjPathSize;
    UINT8    LinkObjPath[TCPIP_MAX_LINK_PATH_LENGTH];
    UINT32   lIpAddr;
    UINT32   lSubnetMask;
    UINT32   lGatewayAddr;
    UINT32   lNameServer;
    UINT32   lNameServer2;
    UINT8    szDomainName[TCPIP_MAX_DOMAIN_LENGTH+1];
    UINT8    szHostName[TCPIP_MAX_HOST_LENGTH+1];
	UINT8	 bTTLValue;
	TCPIP_MCAST_DATA MulticastData;
#ifdef EIP_ACD
	UINT8	 bACDMode;
	UINT8	 bACDLCD_Phase;
	UINT8	 ACDLCD_MacID[MAC_ADDR_LENGTH];
	UINT8	 ACDLCD_ARPPdu[TCPIP_MAX_ARP_PDU_LENGTH];
#endif
#ifdef EIP_QUICK_CONNECT
	UINT8	 bQuickConnect;
#endif
} 
TCPIP_CONFIG_DATA;

typedef struct tagTCPIP_INTERFACE_INSTANCE_ATTRIBUTES
{
    UINT32    lInterfaceStatus;    
    TCPIP_CONFIG_DATA   InterfaceConfig;
#ifdef EIP_NSLED_PER_IP
	EtIPNetworkStatus eSavedNetworkStatus;
#endif
#ifdef EIP_QOS
	QOS_INST_ATTR		QoSConfig;
#endif
    struct    sockaddr_in MulticastBaseAddr;              /* Multicast address base */
    UINT32    lMulticastAddrCount;                        /* Number of used Multicast addresses */
    SOCKET    alClass1Socket[NUM_CONNECTION_GROUPS];      /* UDP socket handling Class1 connections */
#ifdef EIP_LIMIT_NUM_IO_PACKETS_RECEIVED
	SOCKET*	  pNextClass1SocketToProcess;
#endif
    SOCKET    lBroadcastSocket;                           /* broadcast sockets */
    SOCKET    lPassiveWaitSocket;                         /* listening sockets array */
#ifdef TRACE_SOCKET_OUTPUT
    SOCKET    lLogSrvSock;                                /* Socket for sending log messages */
#endif
	UINT32    lListIdentityTimeout;						  /* when to stop listening for list identity broadcase responses */
	UINT16	  iPort;									  /* TCP port used to send/recv (typically ENCAP_SERVER_PORT) */
}
TCPIP_INTERFACE_INSTANCE_ATTRIBUTES;

extern TCPIP_INTERFACE_INSTANCE_ATTRIBUTES gsTcpIpInstAttr[MAX_HOST_ADDRESSES];

extern void tcpipInit(void);
extern void tcpipSocketCleanupAll(void);
extern BOOL tcpipAddClaimedIPAddress(UINT32 lAddress, UINT16 iPort);
extern BOOL tcpipRemoveClaimedIPAddress(UINT32 lAddress);
extern TCPIP_INTERFACE_INSTANCE_ATTRIBUTES*    tcpipFindClaimedHostAddress( UINT32 lHostIPAddress );
extern void tcpipParseClassInstanceRequest( REQUEST* pRequest );
extern void tcpipReset(UINT8 bReset);

extern BOOL platformGetTcpIpCfgData(TCPIP_CONFIG_DATA *psCfg);
extern void platformSetTcpIpCfgData(UINT32 lMask, TCPIP_CONFIG_DATA *psCfg, UINT32 lCurrentIP);
extern BOOL platformTestSetTcpIpCfgData(UINT32 lMask, TCPIP_CONFIG_DATA *psCfg, UINT32 lCurrentIP, UINT8* pbGeneralError);

#ifdef EIP_QOS
extern void platformGetQoSAttributes( TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig );
extern BOOL platformSetQoSAttributes(UINT32 lMask, TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig );
#endif

#endif /* #ifndef TCPIP_H */


