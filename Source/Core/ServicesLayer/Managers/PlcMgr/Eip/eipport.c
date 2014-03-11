/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPPORT.C
**
** COPYRIGHT (c) 2001-2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
*
** Port object implementation 
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

#include "eipinc.h"

/* local functions */
void portParseClassRequest( REQUEST* pRequest );
void portSendClassAttrAll( REQUEST* pRequest);
void portSendClassAttrSingle( REQUEST* pRequest );
void portParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance );
void portSendInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance );
void portSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance );

/*---------------------------------------------------------------------------
** portParseClassInstanceRequest( )
**
** Determine if it's request for the Class or for the particular instance 
**---------------------------------------------------------------------------
*/

void portParseClassInstanceRequest( REQUEST* pRequest )
{
    UINT16 iTagSize;
    UINT32 iInstance = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize );

    if ( iInstance == 0 )
        portParseClassRequest( pRequest );
    else
        portParseInstanceRequest( pRequest, iInstance );
}

/*---------------------------------------------------------------------------
** portParseClassRequest( )
**
** Respond to the Port class request
**---------------------------------------------------------------------------
*/

void portParseClassRequest( REQUEST* pRequest )
{
    switch( pRequest->cfg.objectCfg.bService )
    {
        case SVC_GET_ATTR_ALL:
            portSendClassAttrAll( pRequest );
            break;
        case SVC_GET_ATTR_SINGLE:
            portSendClassAttrSingle( pRequest );
            break;
        default:
            pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
            break;
    }
}

/*---------------------------------------------------------------------------
** portSendClassAttrAll( )
**
** GetAttributeAll service for Port object class is received 
**---------------------------------------------------------------------------
*/

void portSendClassAttrAll( REQUEST* pRequest )
{
	UINT8* pData;
	UINT16 iEntryPort;
	UINT32 i;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig;
#ifdef ET_IP_MODBUS
	UINT16 iMemSize = (UINT16)(sizeof(UINT16)*(8+(gnClaimedHostIPAddrCount*4)));
#else
	UINT16 iMemSize = (UINT16)(sizeof(UINT16)*(8+(gnClaimedHostIPAddrCount*2)));
#endif

    EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iMemSize);

	pData = pRequest->cfg.objectCfg.pData;
	UINT16_SET(pData, PORT_CLASS_REVISION);
	pData += sizeof(UINT16);
#ifdef ET_IP_MODBUS
	UINT16_SET(pData, ((gnClaimedHostIPAddrCount*2)+1));
#else
	UINT16_SET(pData, (gnClaimedHostIPAddrCount+1));
#endif
	pData += sizeof(UINT16);
#ifdef ET_IP_MODBUS
	UINT16_SET(pData, ((gnClaimedHostIPAddrCount*2)+1));
#else
	UINT16_SET(pData, (gnClaimedHostIPAddrCount+1));
#endif
	pData += sizeof(UINT16);

	iEntryPort = 1;
    for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount; 
         i++, pTcpipConfig++)
    {
		if (pTcpipConfig->InterfaceConfig.lIpAddr == pRequest->lHostIPAddr)
		{
#ifdef ET_IP_MODBUS
			iEntryPort = (UINT16)(TCP_IP_STARTING_PORT_NUMBER+((pTcpipConfig-gsTcpIpInstAttr)*2));
#else
			iEntryPort = (UINT16)(TCP_IP_STARTING_PORT_NUMBER+(pTcpipConfig-gsTcpIpInstAttr));
#endif
			break;
		}
    }

	UINT16_SET(pData, iEntryPort);
	pData += sizeof(UINT16);

	/* The values at index 1 (offset 0) and any non-instantiated instances shall be zero. */
	UINT32_SET(pData, 0);
	pData += sizeof(UINT32);

	/* Backplane Port */
	UINT16_SET(pData, TCP_IP_BACKPLANE_PORT_TYPE);
	pData += sizeof(UINT16);

	UINT16_SET(pData, TCP_IP_BACKPLANE_PORT_NUMBER);
	pData += sizeof(UINT16);


    for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount; 
         i++, pTcpipConfig++)
    {
		UINT16_SET(pData, TCP_IP_PORT_TYPE);
		pData += sizeof(UINT16);

#ifdef ET_IP_MODBUS
		UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+i*2));
#else
		UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+i));
#endif
		pData += sizeof(UINT16);

#ifdef ET_IP_MODBUS
		UINT16_SET(pData, TCP_IP_MODBUS_TCP_PORT_TYPE);
		pData += sizeof(UINT16);

		UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+(i*2)+1));
		pData += sizeof(UINT16);
#endif
    }
}

/*---------------------------------------------------------------------------
** portSendClassAttrSingle( )
**
** GetAttributeSingle service for Port object class is received 
**---------------------------------------------------------------------------
*/

void portSendClassAttrSingle( REQUEST* pRequest )
{    
    UINT16 iVal;
	UINT8* pData;
    UINT16 iTagSize;
	UINT32 i;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig;

    UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
    
    switch( iAttribute )
    {
        case PORT_CLASS_ATTR_REVISION:
            iVal = ENCAP_TO_HS(PORT_CLASS_REVISION);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case PORT_CLASS_ATTR_MAX_INSTANCE:

#ifdef ET_IP_MODBUS
			iVal = (UINT16)ENCAP_TO_HS(((gnClaimedHostIPAddrCount*2)+1));
#else
			iVal = (UINT16)ENCAP_TO_HS(gnClaimedHostIPAddrCount+1);
#endif
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case PORT_CLASS_ATTR_NUM_INSTANCES:
#ifdef ET_IP_MODBUS
			iVal = (UINT16)ENCAP_TO_HS(((gnClaimedHostIPAddrCount*2)+1));
#else
			iVal = (UINT16)ENCAP_TO_HS(gnClaimedHostIPAddrCount+1);
#endif
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
 		case PORT_CLASS_ATTR_MAX_CLASS_ATTR:
            iVal = ENCAP_TO_HS(PORT_CLASS_ATTR_ALL_PORTS);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
			break;
		case PORT_CLASS_ATTR_MAX_INST_ATTR:
            iVal = ENCAP_TO_HS(PORT_INST_ATTR_PORT_NODE_ADDRESS);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
			break;
       case PORT_CLASS_ATTR_ENTRY_PORT:
			iVal = 1;
			for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount; 
				 i++, pTcpipConfig++)
			{
				if (pTcpipConfig->InterfaceConfig.lIpAddr == pRequest->lHostIPAddr)
				{
#ifdef ET_IP_MODBUS
					iVal = (UINT16)(TCP_IP_STARTING_PORT_NUMBER+((pTcpipConfig-gsTcpIpInstAttr)*2));
#else
					iVal = (UINT16)(TCP_IP_STARTING_PORT_NUMBER+(pTcpipConfig-gsTcpIpInstAttr));
#endif
					break;
				}
			}
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16));
			UINT16_SET(pRequest->cfg.objectCfg.pData, iVal);
            break;
        case PORT_CLASS_ATTR_ALL_PORTS:
#ifdef ET_IP_MODBUS
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(UINT32)+(sizeof(UINT16)*2*((gnClaimedHostIPAddrCount*2)+1))));
#else
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(UINT32)+(sizeof(UINT16)*2*(gnClaimedHostIPAddrCount+1))));
#endif

			pData = pRequest->cfg.objectCfg.pData;

			/* The values at index 1 (offset 0) and any non-instantiated instances shall be zero. */
			UINT32_SET(pData, 0);
			pData += sizeof(UINT32);

			/* Backplane Port */
			UINT16_SET(pData, TCP_IP_BACKPLANE_PORT_TYPE);
			pData += sizeof(UINT16);

			UINT16_SET(pData, TCP_IP_BACKPLANE_PORT_NUMBER);
			pData += sizeof(UINT16);

			for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount; 
				 i++, pTcpipConfig++)
			{
				UINT16_SET(pData, TCP_IP_PORT_TYPE);
				pData += sizeof(UINT16);

#ifdef ET_IP_MODBUS
				UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+i*2));
#else
				UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+i));
#endif
				pData += sizeof(UINT16);

#ifdef ET_IP_MODBUS
				UINT16_SET(pData, TCP_IP_MODBUS_TCP_PORT_TYPE);
				pData += sizeof(UINT16);

				UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+(i*2)+1));
				pData += sizeof(UINT16);
#endif
			}
            break;
        default:
            pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
            break;
    }        
}


/*---------------------------------------------------------------------------
** portParseInstanceRequest( )
**
** Respond to the Port object instance request
**---------------------------------------------------------------------------
*/

void portParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
#ifdef ET_IP_MODBUS
    if ( iInstance <= gnClaimedHostIPAddrCount*2+1)
#else
    if ( iInstance <= gnClaimedHostIPAddrCount+1)
#endif
    {
        switch( pRequest->cfg.objectCfg.bService )
        {
            case SVC_GET_ATTR_ALL:
                portSendInstanceAttrAll( pRequest, iInstance );
                break;
            case SVC_GET_ATTR_SINGLE:
                portSendInstanceAttrSingle( pRequest, iInstance );
                break;        
            default:
                pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
                break;
        }
    }
    else
    {
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
	}
}


/*---------------------------------------------------------------------------
** portSendInstanceAttrAll( )
**
** GetAttributeAll service for Port instance is received
**---------------------------------------------------------------------------
*/
void portSendInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance )
{
    UINT8* pData;
	UINT8  bPortNameSize;
	UINT16 iPortType;
    
    EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );

	if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
	{
		bPortNameSize = sizeof(PORT_BACKPLANE_PORT_NAME);
		iPortType = TCP_IP_BACKPLANE_PORT_TYPE;
	}
#ifdef ET_IP_MODBUS
	else if (iInstance%2 == 1)
	{
		/* Modbus/TCP port */
		bPortNameSize = sizeof(PORT_MODBUS_TCP_PORT_NAME);
		iPortType = TCP_IP_MODBUS_TCP_PORT_TYPE;
	}
#endif
	else
	{
		/* Normal EtherNet/IP port */
		bPortNameSize = sizeof(PORT_PORT_NAME);
		iPortType = TCP_IP_PORT_TYPE;
	}
    
    pRequest->cfg.objectCfg.iDataSize = (UINT16)( sizeof(UINT16) +                  /* port type */
		                                sizeof(UINT16) +                  /* port number */
										6 +                  /* port EPATH */
										1 + bPortNameSize +      /* length of port name + port name */
			      						sizeof(UINT16) );                 /* port node address - not supported */
        
    pRequest->cfg.objectCfg.pData = EtIP_malloc( NULL, pRequest->cfg.objectCfg.iDataSize );
    
    if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
    {
        notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
        pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;    /* Out of memory */    
        return;
    }

    pData = pRequest->cfg.objectCfg.pData;
            
    UINT16_SET( pData, iPortType );
    pData += sizeof(UINT16);

	UINT16_SET( pData, iInstance );
    pData += sizeof(UINT16);

	if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
	{
		/* Modbus/TCP port */
		*pData++ = 2;
		*pData++ = 0;
		*pData++ = 0x20;
		*pData++ = 0x64;
		*pData++ = 0x24;
	    *pData++ = TCP_IP_BACKPLANE_PORT_NUMBER;

		*pData++ = bPortNameSize;
		memcpy( (char*)pData, PORT_BACKPLANE_PORT_NAME, sizeof(PORT_BACKPLANE_PORT_NAME) );  
		pData += bPortNameSize;
	}
#ifdef ET_IP_MODBUS
	else if (iInstance%2 == 1)
	{
		/* Modbus/TCP port */
		*pData++ = 2;
		*pData++ = 0;
		*pData++ = 0x20;
		*pData++ = 0x65;
		*pData++ = 0x24;
	    *pData++ = (UINT8)(iInstance/2);

		*pData++ = bPortNameSize;
		memcpy( (char*)pData, PORT_MODBUS_TCP_PORT_NAME, sizeof(PORT_MODBUS_TCP_PORT_NAME) );  
		pData += bPortNameSize;

	}
#endif
	else
	{
		/* Normal EtherNet/IP port */
		*pData++ = 2;
		*pData++ = 0;
		*pData++ = 0x20;
		*pData++ = TCPIP_CLASS;
		*pData++ = 0x24;
#ifdef ET_IP_MODBUS
	    *pData++ = (UINT8)(iInstance/2);
#else
	    *pData++ = (UINT8)(iInstance-1);
#endif

		*pData++ = bPortNameSize;
		memcpy( (char*)pData, PORT_PORT_NAME, sizeof(PORT_PORT_NAME) );  
		pData += bPortNameSize;
	}

    *pData++ = 0x10;
    *pData++ = 0;
}


/*--------------------------------------------------------------------------------
** portSendInstanceAttrSingle( )
**
** GetAttributeSingle request has been received for the Port object instance
**--------------------------------------------------------------------------------
*/

void portSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{    
	UINT16 iVal;
	UINT8* pData;
	UINT8  bVal[6];
	UINT16 iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	switch( iAttribute )
	{
		case PORT_INST_ATTR_PORT_TYPE:
			if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
			{
				iVal = ENCAP_TO_HS(TCP_IP_BACKPLANE_PORT_TYPE);
			}
#ifdef ET_IP_MODBUS
			else if (iInstance%2 == 1)
			{
				iVal = ENCAP_TO_HS(TCP_IP_MODBUS_TCP_PORT_TYPE);
			}
#endif
			else
			{
				iVal = ENCAP_TO_HS(TCP_IP_PORT_TYPE);
			}

			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;

        case PORT_INST_ATTR_PORT_NUMBER:
            iVal = (UINT16)ENCAP_TO_HS(iInstance);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        
        case PORT_INST_ATTR_PORT_OBJECT:
			if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
			{
				bVal[0] = 2;
				bVal[1] = 0;
				bVal[2] = 0x20;
				bVal[3] = 0x64;
				bVal[4] = 0x24;
				bVal[5] = TCP_IP_BACKPLANE_PORT_NUMBER;
			}
#ifdef ET_IP_MODBUS
			else if (iInstance%2 == 1)
			{
				bVal[0] = 2;
				bVal[1] = 0;
				bVal[2] = 0x20;
				bVal[3] = 0x65;
				bVal[4] = 0x24;
				bVal[5] = (UINT8)(iInstance/2);
			}
#endif
			else
			{
				bVal[0] = 2;
				bVal[1] = 0;
				bVal[2] = 0x20;
				bVal[3] = TCPIP_CLASS;
				bVal[4] = 0x24;
				bVal[5] = (UINT8)(iInstance-1);
			}

			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, bVal, 6);        
			break;

		case PORT_INST_ATTR_PORT_NAME:
			if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
			{
				EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(PORT_BACKPLANE_PORT_NAME) + sizeof(UINT8)) );
				if ( pRequest->cfg.objectCfg.pData == NULL )
				{
					pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				}
				else
				{
					pData = pRequest->cfg.objectCfg.pData;
					*pData++ = sizeof(PORT_BACKPLANE_PORT_NAME);
					memcpy( (char*)pData, PORT_BACKPLANE_PORT_NAME, sizeof(PORT_BACKPLANE_PORT_NAME) );
				}
			}
#ifdef ET_IP_MODBUS
			else if (iInstance%2 == 1)
			{
				EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(PORT_MODBUS_TCP_PORT_NAME) + sizeof(UINT8)) );
				if ( pRequest->cfg.objectCfg.pData == NULL )
				{
					pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				}
				else
				{
					pData = pRequest->cfg.objectCfg.pData;
					*pData++ = sizeof(PORT_MODBUS_TCP_PORT_NAME);
					memcpy( (char*)pData, PORT_MODBUS_TCP_PORT_NAME, sizeof(PORT_MODBUS_TCP_PORT_NAME) );
				}
			}
#endif
			else
			{
				EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(PORT_PORT_NAME) + sizeof(UINT8)) );
				if ( pRequest->cfg.objectCfg.pData == NULL )
				{
					pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				}
				else
				{
					pData = pRequest->cfg.objectCfg.pData;
					*pData++ = sizeof(PORT_PORT_NAME);
					memcpy( (char*)pData, PORT_PORT_NAME, sizeof(PORT_PORT_NAME) );
				}
			}
            break;
		case PORT_INST_ATTR_PORT_NODE_ADDRESS:
			bVal[0] = 0x10;
			bVal[1] = 0;
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, bVal, 2);        
			break;
		default:
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			break;
    }
}

