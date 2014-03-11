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

#include "FIDOhal_general.h"
#include "FIDOhal_master.h"
#include "FIDOhal_nonmaster.h"

#include "FIDOeth.h"

#include "inc/LEASH.h"
#include "inc/LTEsemLib.h"

/* NVS functionality */
#include "inc/LEASHdebug.h"
#include "TCP_Config.h"

/* Ethernet interface structure (in RAM) */
extern NGethifnet g_FIDO_Ethernet_Interface;

/* external function to provide Ethernet link state */
enum EthLinkState GetEthLinkState(unsigned int portNum);
extern void* mspace_malloc(mspace msp, size_t bytes);
extern void mspace_free(mspace msp, void* mem);

#define LTE_WAIT_FOREVER (0xffffffff)

#define STACK_SIZE      8000

#define HEAP_SIZE (200*1024)
static unsigned char OS_LTE_heap[HEAP_SIZE];
/* data structure to manage application-specific heap */
mspace OS_LTE_mspace;

/* create a semaphore to protect the heap */
static LTE_sem_t OS_LTE_heapSem;

static int OS_LTE_heapInitialized = 0; /* can't count on os_init being called before alloc */

/*
 * Keep track of offsets from base claim for any requested
 * mutexes.
 */
int g_mutexIndex = 0;

INT32 platformSetSockOpt(int s, int level, int optname,
                         const void *optval, int optlen)
{
  UINT8 opt = *((UINT8*)optval);
  UINT32 optval_as_uint32 = (UINT32)opt;

  if (optname == IP_MULTICAST_TTL) {
    // required because Nexgen only accepts this option if the optlen is 4:
    return setsockopt(s, level, optname, &optval_as_uint32, sizeof(UINT32));
  }
  else {
    // for all others just let 'em go as is:
    return setsockopt(s, level, optname, optval, optlen);
  }
}

/*---------------------------------------------------------------------------
** platformInitMutex( )
**
** Creates mutex to protect the EtherNet/IP thread from the application.
**---------------------------------------------------------------------------
*/
UINT32 platformInitMutex(char* strMutexName)
{
  strMutexName = strMutexName;

  /* Just return a non-zero value because mutexes aren't used */
  return 42;
}

static void InitHeap()
{
  /* Initialize the heap (application-specific) */
  OS_LTE_mspace = LEASHcreateMspace((void *)OS_LTE_heap, HEAP_SIZE, 0);
  OS_LTE_heapSem = LTE_semCreate(1);
  OS_LTE_heapInitialized = 1;
}

/*----------------------------------------------------------------------
** _OsAlloc
**----------------------------------------------------------------------
** Description........:
** Parameter 1 [].....:
** Return.............: returns
**----------------------------------------------------------------------
*/
UINT16* platformAlloc(UINT32 dwLength)
{
  UINT16* ppMem;

  if (!OS_LTE_heapInitialized)
    InitHeap();

  LTE_semTake(OS_LTE_heapSem, LTE_WAIT_FOREVER);
  ppMem = mspace_malloc(OS_LTE_mspace, dwLength);
  LTE_semSignal(OS_LTE_heapSem);

  return (ppMem);
}

/*----------------------------------------------------------------------
** _OsFree
**----------------------------------------------------------------------
** Description........:
** Parameter 1 [].....:
** Return.............: returns
**----------------------------------------------------------------------
*/
UINT16 platformFree (void* ppMem)
{
  if (!OS_LTE_heapInitialized)
    InitHeap();
  LTE_semTake(OS_LTE_heapSem, LTE_WAIT_FOREVER);
  mspace_free(OS_LTE_mspace,ppMem);
  LTE_semSignal(OS_LTE_heapSem);

  return (1);
}

/*---------------------------------------------------------------------------
** platformInitLogFile( )
**
** Creates a log file.
**---------------------------------------------------------------------------
*/

void platformInitLogFile(void)
{

    /*
  ** This function is called during initialization.
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
  ** format) will call this function with bTimeStamp == FALSE. Other
  ** notifications call this function with bTimeStamp == TRUE.
  **
  ** In practice any log format desired can be accommodated here.
  */
#ifdef TRACE_FILE_OUTPUT
  int ictl;

    static char szBuffer[(MAX_PACKET_SIZE*3)+1000];
  static char szSrcIp[20], szDstIp[20];

  /* always use timestamp */
  bTimeStamp = bTimeStamp;
  /* iSeverity not included in log statement */
  iSeverity = iSeverity;

  if ((lSrcIp != 0) || (lDstIp != 0))
  {
    strcpy(szSrcIp, socketGetAddressString(lSrcIp));
    strcpy(szDstIp, socketGetAddressString(lDstIp));
    sprintf(szBuffer, "%c (%s -> %s) %s\n", traceGetTypeAsChar(iType), szSrcIp, szDstIp, szBuf);
  }
  else
    sprintf(szBuffer, "%c %s\n", traceGetTypeAsChar(iType), szBuf);

    /* Output the string */
  printf(szBuffer);

#endif

}

#ifdef EIP_ACD
BOOL platformIsDuplicateIPAddress(UINT32 IPAddress)
{
  // we can ignore the provided IP since we only have one.
  return (FIDO_ethIsDuplicateIP() != 0);
}
#endif

/*---------------------------------------------------------------------------
** platformGetTcpIpCfgData()
**
** Get the TCP/IP configuration data including DHCP flag, subnet mask, gateway
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
  **  typedef struct tagTCPIP_CONFIG_DATA
  **  {
  **    UINT32   lConfigCapability;
  **    UINT32   lConfigControl;
  **    UINT16   iLinkObjPathSize;
  **    UINT8    LinkObjPath[TCPIP_MAX_LINK_PATH_LENGTH];
  **    UINT32   lIpAddr;
  **    UINT32   lSubnetMask;
  **    UINT32   lGatewayAddr;
  **    UINT32   lNameServer;
  **    UINT32   lNameServer2;
  **    UINT8    szDomainName[TCPIP_MAX_DOMAIN_LENGTH+1];
  **    UINT8    szHostName[TCPIP_MAX_HOST_LENGTH+1];
  **    UINT8    bTTLValue;
  **    TCPIP_MCAST_DATA MulticastData;
  **    #ifdef EIP_ACD
  **      UINT8  bACDMode;
  **      UINT8  bACDLCD_Phase;
  **      UINT8  ACDLCD_MacID[MAC_ADDR_LENGTH];
  **      UINT8  ACDLCD_ARPPdu[TCPIP_MAX_ARP_PDU_LENGTH];
  **    #endif
  **  }
  **  TCPIP_CONFIG_DATA;
  **
  ** Please see the EtherNet/IP protocol specification for further information.
  */

#ifdef EIP_ACD

#endif

  // This should be populated with the same value that is passed in except when
  // switching between DHCP and static IP address through TCP/IP Object
  FIDO_get_ethIPaddr(&g_FIDO_Ethernet_Interface.eif_net, (NGuint*)(&psCfg->lIpAddr));

  FIDO_get_ethSubNetMask(&g_FIDO_Ethernet_Interface.eif_net, (NGuint*)(&psCfg->lSubnetMask));
  FIDO_get_ethGateway((NGuint*)(&psCfg->lGatewayAddr));

  FIDO_get_ethDNSserver(1, (NGuint*)(&psCfg->lNameServer));
  FIDO_get_ethDNSserver(2, (NGuint*)(&psCfg->lNameServer2));
  FIDO_get_ethDNSdomain((char*)psCfg->szDomainName);
  FIDO_get_ethHostname(&g_FIDO_Ethernet_Interface.eif_net, (char*)psCfg->szHostName);

  /* Set the User-defined bitmaps used for network configuration options */
  psCfg->lConfigCapability = TCPIP_CAPABILITY_DNS |
                             TCPIP_CAPABILITY_DHCP |
                             TCPIP_CAPABILITY_SETTABLE_CONFIG |
                             TCPIP_CAPABILITY_IP_RESET_NEEDED;
  if (g_tcp_config.use_dhcp == USE_STATIC_IP_ADDRESS)
  {
      psCfg->lConfigControl = TCPIP_CONTROL_USE_STORED_CONFIG;
  }
  else if (g_tcp_config.use_dhcp == USE_DYNAMIC_IP_ADDRESS)
  {
      psCfg->lConfigControl = TCPIP_CONTROL_DHCP;
  }

  /* Setup the multicast parameters */
  psCfg->bTTLValue = g_tcp_config.byTTLValue;
  psCfg->MulticastData.bAllocControl = g_tcp_config.byAllocControl;
  psCfg->MulticastData.bReserved = 0;
  psCfg->MulticastData.iNumMcast = g_tcp_config.iNumMulticast;
  psCfg->MulticastData.lMcastStartAddr = g_tcp_config.lMCastStartAddr;

  if (FIDO_eth_get_NumberOfPorts() > 1) {
    /* Set the Link Object path size to zero to say we do not have an internal link object */
    psCfg->iLinkObjPathSize = 0;
    psCfg->LinkObjPath[0] = 0;
  }
  else {
    /* Set the fixed path to the one and only link object */
    psCfg->iLinkObjPathSize = 4;    /* 4 bytes long */
    psCfg->LinkObjPath[0] = 0x20;   /* Class logical segment */
    psCfg->LinkObjPath[1] = 0xf6;   /* Ethernet Link Class ID */
    psCfg->LinkObjPath[2] = 0x24;   /* Instance Logical Segment */
    psCfg->LinkObjPath[3] = 0x01;   /* Instance 1 */
  }

#ifdef EIP_ACD
  /* Set the ACD data */
  psCfg->lConfigCapability |= TCPIP_CAPABILITY_ACD;
  psCfg->bACDMode = g_tcp_config.use_ACD;
  psCfg->bACDLCD_Phase = FIDO_ethGetACDState();
  memcpy(psCfg->ACDLCD_MacID, (const void *)FIDO_ethGetRemoteMAC(), MAC_ADDR_LENGTH);
  memcpy(psCfg->ACDLCD_ARPPdu, (const void *)FIDO_ethGetARPpacket(), TCPIP_MAX_ARP_PDU_LENGTH);
#endif

  DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Claimed IP address: %s", socketGetAddressString(psCfg->lIpAddr));
  DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Subnet mask: %s", socketGetAddressString(psCfg->lSubnetMask));
  DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Gateway address: %s", socketGetAddressString(psCfg->lGatewayAddr));
  DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Primary name server: %s", socketGetAddressString(psCfg->lNameServer));
  DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Secondary name server: %s", socketGetAddressString(psCfg->lNameServer2));
  DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "Host name: %s", psCfg->szHostName);
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
BOOL platformTestSetTcpIpCfgData(UINT32 lMask, TCPIP_CONFIG_DATA *psCfg, UINT32 iCurrentIP, UINT8* pbGeneralError)
{
  // if we get here then I guess we can allow the work to proceed:
  LEASH_INFO("platformTestSetTcpIpCfgData PASSED!\n");
  *pbGeneralError = ROUTER_ERROR_SUCCESS;
  return TRUE;
}

/*---------------------------------------------------------------------------
** platformSetTcpIpCfgData()
**
** Set the TCP/IP configuration data including subnet mask, gateway
** address, domain name, and name servers.
**---------------------------------------------------------------------------
*/

void platformSetTcpIpCfgData(UINT32 lMask, TCPIP_CONFIG_DATA *psCfg, UINT32 iCurrentIP)
{
    /*
  ** This function is called to set the device's
  ** TCP/IP configuration data (if supported)
  **
  ** The TCPIP_CONFIG_DATA structure is defined in eiptcpip.h as follows:
  **
  **  typedef struct tagTCPIP_CONFIG_DATA
  **  {
  **    UINT32   lConfigCapability;
  **    UINT32   lConfigControl;
  **    UINT16   iLinkObjPathSize;
  **    UINT8    LinkObjPath[TCPIP_MAX_LINK_PATH_LENGTH];
  **    UINT32   lIpAddr;
  **    UINT32   lSubnetMask;
  **    UINT32   lGatewayAddr;
  **    UINT32   lNameServer;
  **    UINT32   lNameServer2;
  **    UINT8    szDomainName[TCPIP_MAX_DOMAIN_LENGTH+1];
  **    UINT8    szHostName[TCPIP_MAX_HOST_LENGTH+1];
  **    UINT8    bTTLValue;
  **    TCPIP_MCAST_DATA MulticastData;
  **    #ifdef EIP_ACD
  **      UINT8  bACDMode;
  **      UINT8  bACDLCD_Phase;
  **      UINT8  ACDLCD_MacID[MAC_ADDR_LENGTH];
  **      UINT8  ACDLCD_ARPPdu[TCPIP_MAX_ARP_PDU_LENGTH];
  **    #endif
  **  }
  **  TCPIP_CONFIG_DATA;
  **
  ** Please see the EtherNet/IP protocol specification for further information.
  */
  BOOL bRet = FALSE;

#ifdef EIP_ACD
  if (lMask & (1 << TCPIP_INST_ATTR_ACD_MODE)) {
    g_tcp_config.use_ACD = psCfg->bACDMode;
    bRet = TRUE;
  }
#endif

  if (lMask & (1 << TCPIP_INST_ATTR_CONFIG_CONTROL))
  {
    switch(psCfg->lConfigControl)
    {
    case TCPIP_CONTROL_USE_STORED_CONFIG:
      g_tcp_config.use_dhcp = USE_STATIC_IP_ADDRESS;
      bRet = TRUE;
      break;
    case TCPIP_CONTROL_DHCP:
      g_tcp_config.use_dhcp = USE_DYNAMIC_IP_ADDRESS;
      bRet = TRUE;
      break;
    }
  }

  if (lMask & (1 << TCPIP_INST_ATTR_INTERFACE_CONFIG))
  {
    if (psCfg->lIpAddr != 0)
      g_tcp_config.ipaddr = psCfg->lIpAddr;

    if (psCfg->lSubnetMask != 0)
      g_tcp_config.subnetMask = psCfg->lSubnetMask;

    g_tcp_config.defaultGateway = psCfg->lGatewayAddr;
    g_tcp_config.dnsSrvr1 = psCfg->lNameServer;
    g_tcp_config.dnsSrvr2 = psCfg->lNameServer2;

    if (strlen((char*)psCfg->szDomainName) > 31) // Nexgen limitation...
      psCfg->szDomainName[31] = 0;

    strcpy((char*)g_tcp_config.domain_name, (const char*)psCfg->szDomainName);

    bRet = TRUE;
  }

  if (lMask & (1 << TCPIP_INST_ATTR_HOST_NAME))
  {
    strcpy((char*)g_tcp_config.host_name, (const char*)psCfg->szHostName);
    bRet = TRUE;
  }

  if (lMask & (1 << TCPIP_INST_ATTR_TTL_VALUE))
  {
    g_tcp_config.byTTLValue = psCfg->bTTLValue;
    bRet = TRUE;
  }

  if (lMask & (1 << TCPIP_INST_ATTR_MCAST_CONFIG))
  {
    g_tcp_config.byAllocControl = psCfg->MulticastData.bAllocControl;
    g_tcp_config.iNumMulticast = psCfg->MulticastData.iNumMcast;
    g_tcp_config.lMCastStartAddr = psCfg->MulticastData.lMcastStartAddr;
    bRet = TRUE;
  }

  if (bRet) {
    SaveTCP_Config();
  }
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
  ** This function is called during initialization.
  **
  ** Upon return, it should have placed the number of IP addresses supported
  ** by the device (i.e. the number of active Ethernet interfaces) in the
  ** global gnHostIPAddrCount.
  **
  ** In addition, it should have placed each supported Ethernet address
  ** as a UINT32) in the global array galHostIPAddr[MAX_HOST_ADDRESSES].
  */

  FIDO_get_ethIPaddr(&g_FIDO_Ethernet_Interface.eif_net, (NGuint*)(&galHostIPAddr[0]));
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
    memset(pSockaddr, 0, sizeof(struct sockaddr_in) );
    pSockaddr->sin_family = AF_INET;
    pSockaddr->sin_port = htons( iPort );
    platformGetInternetAddress(pSockaddr->sin_addr) = lIpAddress;
}

/*---------------------------------------------------------------------------
** platformCopySockAddrInToBuffer( )
**
** Copy platform specific sock_addr_in structure to the wire
**---------------------------------------------------------------------------
*/
void platformCopySockAddrInToBuffer(UINT8* pBuffer, struct sockaddr_in* pSockAddr)
{
  UINT8* pData = pBuffer;

  /* All values should be big-endian */
  *((UINT16*)pData) = pSockAddr->sin_family;
  pData += sizeof(UINT16);
  *((UINT16*)pData) = pSockAddr->sin_port;
  pData += sizeof(UINT16);
  *((UINT32*)pData) = pSockAddr->sin_addr.s_addr;
  pData += sizeof(UINT32);
  memset(pData, 0, 8);
}

/*---------------------------------------------------------------------------
** platformCopySockAddrInToBuffer( )
**
** Copy platform specific sock_addr_in structure from the wire
**---------------------------------------------------------------------------
*/
void platformCopySockAddrInFromBuffer(struct sockaddr_in* pSockAddr, UINT8* pBuffer)
{
  UINT8* pData = pBuffer;

  /* All values should be big-endian */
  pSockAddr->sin_family = *((UINT16*)pData);
  pData += sizeof(UINT16);
  pSockAddr->sin_port = *((UINT16*)pData);
  pData += sizeof(UINT16);
  pSockAddr->sin_addr.s_addr = *((UINT32*)pData);
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

  return ngOSClockGetTime();
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
  return FIDO_eth_get_NumberOfPorts();
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
  /* Populate with Ethernet Link attributes for local network card with IP lIPAddress
  **
  ** The EnetLinkConfig structure is defined in eipetlnk.h as
  **
  **  typedef struct tagENETLINK_INST_ATTR
  **  {
  **    UINT32   lInterfaceSpeed;
  **    UINT32   lInterfaceFlags;
  **    UINT8    macAddr[MAC_ADDR_LENGTH];
  **    UINT16   iInterfaceControlBits;
  **    UINT16   iForcedInterfaceSpeed;
  **    UINT8     bInterfaceType;
  **    UINT8     bInterfaceState;
  **    UINT8     bAdminState;
  **    INT8     InterfaceLabel[ENETLINK_INTERFACE_MAX_LABEL_SIZE];
  **  }
  **  ENETLINK_INST_ATTR;
  */

  enum EthLinkState LinkState;
  NGuint lIPAddress;

  // determine the link state of the port defined by iInstance:
  // Note: Ethernet link object instance 1 is port 1, instance 2 is port 2
  LinkState = GetEthLinkState(iInstance);

  switch(LinkState)
  {
  case ETH_LINK_STATE_UNKNOWN:
    pEnetLinkAttr->lInterfaceFlags = 0;
    pEnetLinkAttr->lInterfaceSpeed = 0;
    break;
  case ETH_LINK_STATE_100BASETX:
    pEnetLinkAttr->lInterfaceFlags = ENETLINK_FLAG_LINK_ACTIVE |
       ((g_tcp_config.ifc_use_autonegotiation[iInstance-1] == USE_LINK_FORCED)?0x10:ENETLINK_FLAG_SPEED_DUPLEX_FINE) |
                                                   ENETLINK_FLAG_RESET_REQUIRED;
    pEnetLinkAttr->lInterfaceSpeed = 100;
    break;
  case ETH_LINK_STATE_100BASETX_FD:
    pEnetLinkAttr->lInterfaceFlags = ENETLINK_FLAG_LINK_ACTIVE |
       ((g_tcp_config.ifc_use_autonegotiation[iInstance-1] == USE_LINK_FORCED)?0x10:ENETLINK_FLAG_SPEED_DUPLEX_FINE) |
                                                   EL_FLAG_DUPLEX_MODE |
                                                   ENETLINK_FLAG_RESET_REQUIRED;
    pEnetLinkAttr->lInterfaceSpeed = 100;
    break;
  case ETH_LINK_STATE_10BASETX:
    pEnetLinkAttr->lInterfaceFlags = ENETLINK_FLAG_LINK_ACTIVE |
       ((g_tcp_config.ifc_use_autonegotiation[iInstance-1] == USE_LINK_FORCED)?0x10:ENETLINK_FLAG_SPEED_DUPLEX_FINE) |
                                                   ENETLINK_FLAG_RESET_REQUIRED;
    pEnetLinkAttr->lInterfaceSpeed = 10;
    break;
  case ETH_LINK_STATE_10BASETX_FD:
    pEnetLinkAttr->lInterfaceFlags = ENETLINK_FLAG_LINK_ACTIVE |
       ((g_tcp_config.ifc_use_autonegotiation[iInstance-1] == USE_LINK_FORCED)?0x10:ENETLINK_FLAG_SPEED_DUPLEX_FINE) |
                                                   EL_FLAG_DUPLEX_MODE |
                                                   ENETLINK_FLAG_RESET_REQUIRED;
    pEnetLinkAttr->lInterfaceSpeed = 10;
    break;
  }

  // provide the same MAC regardless of iInstance
  FIDO_get_ethMACAddr(&g_FIDO_Ethernet_Interface.eif_net, pEnetLinkAttr->macAddr);

  // set the control bits...
  pEnetLinkAttr->iInterfaceControlBits = 1; // first we assume full auto negotiation
  if (g_tcp_config.ifc_use_autonegotiation[iInstance-1] == USE_LINK_FORCED) {
    // nope, we're set to a forced speed/duplex
    // so tell 'em the forced duplex:
    if (g_tcp_config.ifc_duplex[iInstance-1] == USE_LINK_FULL_DUPLEX)
      pEnetLinkAttr->iInterfaceControlBits = 2;
    else if (g_tcp_config.ifc_duplex[iInstance-1] == USE_LINK_HALF_DUPLEX)
      pEnetLinkAttr->iInterfaceControlBits = 0;
    else
      pEnetLinkAttr->iInterfaceControlBits = 0; // we should never get here...

    // and tell 'em the forced speed:
    if (g_tcp_config.ifc_speed[iInstance-1] == USE_LINK_SPEED_10_MBPS)
      pEnetLinkAttr->iForcedInterfaceSpeed = 10;
    else if (g_tcp_config.ifc_speed[iInstance-1] == USE_LINK_SPEED_100_MBPS)
      pEnetLinkAttr->iForcedInterfaceSpeed = 100;
    else
      pEnetLinkAttr->iForcedInterfaceSpeed = 0; // we should never get here...
  }
  else {
    pEnetLinkAttr->iForcedInterfaceSpeed = 0;
  }

    // we only support twisted pair for now
  pEnetLinkAttr->bInterfaceType = ENETLINK_INTERFACE_TYPE_TWISTED_PAIR;

  // and state attribute simply follows the adminState attribute
  pEnetLinkAttr->bInterfaceState = g_tcp_config.byAdminState[iInstance-1];

  FIDO_get_ethIPaddr(&g_FIDO_Ethernet_Interface.eif_net, &lIPAddress);
  strcpy(pEnetLinkAttr->InterfaceLabel, socketGetAddressString(lIPAddress));
  strcat(pEnetLinkAttr->InterfaceLabel, (iInstance == 1)?"_1":"_2");

  pEnetLinkAttr->bAdminState = g_tcp_config.byAdminState[iInstance-1];
}

/*---------------------------------------------------------------------------
** platformSetEnetLinkAttributes( )
**
** Store Ethernet Link attributes
**---------------------------------------------------------------------------
*/
void platformSetEnetLinkAttributes(UINT32 lMask, UINT32 iInstance, ENETLINK_INST_ATTR* pEnetLinkAttr)
{
    /*
  ** This function is called to set the device's Ethernet Link object
  ** configuration data (if supported)
  **
  ** The EnetLinkConfig structure is defined in eipetlnk.h as
  **
  **  typedef struct tagENETLINK_INST_ATTR
  **  {
  **    UINT32   lInterfaceSpeed;
  **    UINT32   lInterfaceFlags;
  **    UINT8    macAddr[MAC_ADDR_LENGTH];
  **    UINT16   iInterfaceControlBits;
  **    UINT16   iForcedInterfaceSpeed;
  **    UINT8    bInterfaceType;
  **    UINT8    bInterfaceState;
  **    UINT8    bAdminState;
  **    INT8     InterfaceLabel[ENETLINK_INTERFACE_MAX_LABEL_SIZE];
  **  }
  **  ENETLINK_INST_ATTR;
  **
  ** Please see the EtherNet/IP protocol specification for further information.
  */
  BOOL bRet = FALSE;

  if (lMask & (1 << ENETLINK_INST_ATTR_INTERFACE_CONTROL)) {
    if (pEnetLinkAttr->iInterfaceControlBits & ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED) {
      g_tcp_config.ifc_use_autonegotiation[iInstance-1] = USE_LINK_AUTONEG;
    }
    else {
      g_tcp_config.ifc_use_autonegotiation[iInstance-1] = USE_LINK_FORCED;
    }

    if (g_tcp_config.ifc_use_autonegotiation[iInstance-1] == USE_LINK_FORCED) {
      if (pEnetLinkAttr->iInterfaceControlBits & ENETLINK_INTERFACE_CONTROL_FORCED_FULL_DUPLEX) {
        g_tcp_config.ifc_duplex[iInstance-1] = USE_LINK_FULL_DUPLEX;
      }
      else {
        g_tcp_config.ifc_duplex[iInstance-1] = USE_LINK_HALF_DUPLEX;
      }

      switch(pEnetLinkAttr->iForcedInterfaceSpeed)
      {
      case 10:
        g_tcp_config.ifc_speed[iInstance-1] = USE_LINK_SPEED_10_MBPS;
        break;
      case 100:
        g_tcp_config.ifc_speed[iInstance-1] = USE_LINK_SPEED_100_MBPS;
        break;
      }
    }

    bRet = TRUE;
  }

  if (lMask & (1 << ENETLINK_INST_ATTR_ADMIN_STATE))
  {
    g_tcp_config.byAdminState[iInstance-1] = pEnetLinkAttr->bAdminState;
    bRet = TRUE;
  }

  if (bRet) {
    SaveTCP_Config();
  }
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
  if (lMask == (1 << ENETLINK_INST_ATTR_ADMIN_STATE)) {
    // first a basic check to see if the question makes sense at all:
    if (iInstance > platformGetNumEnetLinkInstances()) {
      LEASH_INFO("platformTestSetEnetLinkAttributes FAILED, iInstance = %d!\n", iInstance);
      *pbGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
       return FALSE;
    }
    // and then check that we are not trying to disable our one and only port:
    else if ((platformGetNumEnetLinkInstances() == 1) &&
             (pEnetLinkAttr->bAdminState == ENETLINK_ADMIN_STATE_DISABLED)) {
      LEASH_INFO("platformTestSetEnetLinkAttributes FAILED, cannot disable our one and only port!\n");
      *pbGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
      return FALSE;
    }
    // and finally we do NOT, under ANY circumstances, allow BOTH ports to become disabled:
    else if ( ((iInstance == 1) && (g_tcp_config.byAdminState[1] == ENETLINK_INTERFACE_STATE_DISABLED)) ||
              ((iInstance == 2) && (g_tcp_config.byAdminState[0] == ENETLINK_INTERFACE_STATE_DISABLED)) )
    {
      *pbGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
      return FALSE;
    }
  }

  // if we get here then I guess we can allow the work to proceed:
  LEASH_INFO("platformTestSetEnetLinkAttributes PASSED!\n");
  *pbGeneralError = ROUTER_ERROR_SUCCESS;
  return TRUE;
}

#ifdef EIP_QOS
/*
** The TCPIP_INTERFACE_INSTANCE_ATTRIBUTES structure is defined in eiptcpip.h
** as follows (we care here about the QOS_INST_ATTR struct)
**
**  typedef struct tagTCPIP_INTERFACE_INSTANCE_ATTRIBUTES
**  {
**    UINT32    lInterfaceStatus;
**    TCPIP_CONFIG_DATA   InterfaceConfig;
**    #ifdef EIP_QOS
**      QOS_INST_ATTR   QoSConfig;
**    #endif
**    struct    sockaddr_in MulticastBaseAddr;              // Multicast address base
**    UINT32    lMulticastAddrCount;                        // Number of used Multicast addresses
**    SOCKET    alClass1Socket[NUM_CONNECTION_GROUPS];      // UDP socket handling Class1 connections
**    SOCKET    lBroadcastSocket;                           // broadcast sockets
**    SOCKET    lPassiveWaitSocket;                         // listening sockets array
**    #ifdef TRACE_SOCKET_OUTPUT
**      SOCKET    lLogSrvSock;                              // Socket for sending log messages
**    #endif
**    UINT32    lListIdentityTimeout;  // when to stop listening for list identity broadcast responses
**  }
**  TCPIP_INTERFACE_INSTANCE_ATTRIBUTES;
**
**  The QOS_INST_ATTR is defined in eipqos.h as follows:
**
**  typedef struct tagQOS_INST_ATTR
**  {
**    UINT8  b8021QTagEnable;
**    UINT8  bDSCPPTPEvent;
**    UINT8  bDSCPPTPGeneral;
**    UINT8  bDSCPUrgent;
**    UINT8  bDSCPScheduled;
**    UINT8  bDSCPHigh;
**    UINT8  bDSCPLow;
**    UINT8  bDSCPExplicit;
**  }
**  QOS_INST_ATTR;
**
*/
void platformGetQoSAttributes( TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig )
{
  pTcpipConfig->QoSConfig.b8021QTagEnable = g_tcp_config.b8021QTagEnable;
  pTcpipConfig->QoSConfig.bDSCPPTPEvent   = g_tcp_config.bDSCPPTPEvent;
  pTcpipConfig->QoSConfig.bDSCPPTPGeneral = g_tcp_config.bDSCPPTPGeneral;
  pTcpipConfig->QoSConfig.bDSCPUrgent     = g_tcp_config.bDSCPUrgent;
  pTcpipConfig->QoSConfig.bDSCPScheduled  = g_tcp_config.bDSCPScheduled;
  pTcpipConfig->QoSConfig.bDSCPHigh       = g_tcp_config.bDSCPHigh;
  pTcpipConfig->QoSConfig.bDSCPLow        = g_tcp_config.bDSCPLow;
  pTcpipConfig->QoSConfig.bDSCPExplicit   = g_tcp_config.bDSCPExplicit;
}

BOOL platformSetQoSAttributes(UINT32 lMask, TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig )
{
  BOOL bRet = FALSE;

  if (lMask == (1 << QOS_INST_ATTR_8021Q_TAG_ENABLE)) {
    g_tcp_config.b8021QTagEnable = pTcpipConfig->QoSConfig.b8021QTagEnable;
    bRet = TRUE;
  }

  if (lMask == (1 << QOS_INST_ATTR_DSCP_PTP_EVENT)) {
    g_tcp_config.bDSCPPTPEvent = pTcpipConfig->QoSConfig.bDSCPPTPEvent;
    bRet = TRUE;
  }

  if (lMask == (1 << QOS_INST_ATTR_DSCP_PTP_GENERAL)) {
    g_tcp_config.bDSCPPTPGeneral = pTcpipConfig->QoSConfig.bDSCPPTPGeneral;
    bRet = TRUE;
  }

  if (lMask == (1 << QOS_INST_ATTR_DSCP_URGENT)) {
    g_tcp_config.bDSCPUrgent = pTcpipConfig->QoSConfig.bDSCPUrgent;
    bRet = TRUE;
  }

  if (lMask == (1 << QOS_INST_ATTR_DSCP_SCHEDULED)) {
    g_tcp_config.bDSCPScheduled = pTcpipConfig->QoSConfig.bDSCPScheduled;
    bRet = TRUE;
  }

  if (lMask == (1 << QOS_INST_ATTR_DSCP_HIGH)) {
    g_tcp_config.bDSCPHigh = pTcpipConfig->QoSConfig.bDSCPHigh;
    bRet = TRUE;
  }

  if (lMask == (1 << QOS_INST_ATTR_DSCP_LOW)) {
    g_tcp_config.bDSCPLow = pTcpipConfig->QoSConfig.bDSCPLow;
    bRet = TRUE;
  }

  if (lMask == (1 << QOS_INST_ATTR_DSCP_EXPLICIT)) {
    g_tcp_config.bDSCPExplicit = pTcpipConfig->QoSConfig.bDSCPExplicit;
    bRet = TRUE;
  }

  if (bRet) {
    SaveTCP_Config();
  }

  return bRet;
}

INT32 platformSetDSCPValue(SOCKET s, UINT8 bDSCPValue)
{
  int rv, TOSval = bDSCPValue;

  TOSval = TOSval << 2; // shift into DSCP field.

  LEASH_INFO("platformSetDSCPValue() attempting to set DSCP to %d on socket %d\n",
      bDSCPValue, s);

  rv = setsockopt(s, IPPROTO_IP, IP_TOS, &TOSval, sizeof(int));
  if (rv != NG_EOK)
    return SOCKET_ERROR;
  else
    return NG_EOK;
}
#endif
