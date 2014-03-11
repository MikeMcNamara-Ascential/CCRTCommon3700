/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPETLNK.C
**
** COPYRIGHT (c) 2001-2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Ethernet Link object implementation 
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
void enetlinkParseClassRequest( REQUEST* pRequest );
void enetlinkSendClassAttrAll( REQUEST* pRequest );
void enetlinkSendClassAttrSingle( REQUEST* pRequest );
void enetlinkParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance );
void enetlinkSendInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance );
void enetlinkSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance );
void enetlinkSetInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance );

UINT16 enetlinkOptionalAttributes[ENETLINK_ATTR_OPT_COUNT] = {ENETLINK_INST_ATTR_INTERFACE_TYPE, ENETLINK_INST_ATTR_INTERFACE_STATE, ENETLINK_INST_ATTR_ADMIN_STATE, ENETLINK_INST_ATTR_INTERFACE_LABEL};

/*---------------------------------------------------------------------------
** enetlinkInit( )
**
** Initialize Ethernet Link object
**---------------------------------------------------------------------------
*/

void enetlinkInit( ENETLINK_INST_ATTR* pEnetLinkAttr )
{   	
    memset(pEnetLinkAttr, 0, sizeof(ENETLINK_INST_ATTR)); 
}

/*---------------------------------------------------------------------------
** enetlinkParseClassInstanceRqst( )
**
** Determine if it's request for the Class or for the particular instance 
**---------------------------------------------------------------------------
*/

void enetlinkParseClassInstanceRqst( REQUEST* pRequest )
{
    UINT16 iTagSize;
    UINT32 iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
    
    if ( iInstance == 0 )
        enetlinkParseClassRequest( pRequest );
    else
        enetlinkParseInstanceRequest( pRequest, iInstance );
}

/*---------------------------------------------------------------------------
** enetlinkParseClassRequest( )
**
** Respond to the Ethernet Link class request
**---------------------------------------------------------------------------
*/

void enetlinkParseClassRequest( REQUEST* pRequest )
{
    switch( pRequest->cfg.objectCfg.bService )
    {
        case SVC_GET_ATTR_ALL:
            enetlinkSendClassAttrAll( pRequest );
            break;
        case SVC_GET_ATTR_SINGLE:
            enetlinkSendClassAttrSingle( pRequest );
            break;
        default:        
            pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
            break;
    }
}

/*---------------------------------------------------------------------------
** enetlinkSendClassAttrAll( )
**
** GetAttributeAll service for Ethernet Link class is received 
**---------------------------------------------------------------------------
*/

void enetlinkSendClassAttrAll( REQUEST* pRequest )
{
    UINT16 iVal = ENCAP_TO_HS(ENETLINK_CLASS_REVISION);
    EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
}

/*---------------------------------------------------------------------------
** enetlinkSendClassAttrSingle( )
**
** GetAttributeSingle service for Ethernet Link class is received 
**---------------------------------------------------------------------------
*/

void enetlinkSendClassAttrSingle( REQUEST* pRequest )
{    
    UINT16 iVal, iTagSize, iNumberOfAttributes, i;
    UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
	UINT8* pData;
	UINT16 *pAttrData;
    
    switch( iAttribute )
    {
        case ENETLINK_CLASS_ATTR_REVISION:
            iVal = ENCAP_TO_HS(ENETLINK_CLASS_REVISION);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case ENETLINK_CLASS_ATTR_MAX_INSTANCE:
            iVal = (UINT16)ENCAP_TO_HS(platformGetNumEnetLinkInstances());
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case ENETLINK_CLASS_ATTR_NUM_INSTANCES:
            iVal = (UINT16)ENCAP_TO_HS(platformGetNumEnetLinkInstances());
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
		case ENETLINK_CLASS_ATTR_OPT_ATTR_LIST:
			iNumberOfAttributes = sizeof(enetlinkOptionalAttributes)/sizeof(UINT16);
			EtIP_realloc(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)((iNumberOfAttributes+1)*sizeof(UINT16)));

			pData = pRequest->cfg.objectCfg.pData;
			UINT16_SET(pData, iNumberOfAttributes);
			pData += sizeof(UINT16);

			for (i = 0, pAttrData = enetlinkOptionalAttributes; 
				i < iNumberOfAttributes; 
				i++, pAttrData++, pData += sizeof(UINT16))
			{
				UINT16_SET(pData, *pAttrData);
			}
			break;
 		case ENETLINK_CLASS_ATTR_MAX_CLASS_ATTR:
            iVal = ENCAP_TO_HS(ENETLINK_CLASS_ATTR_MAX_INST_ATTR);            
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );            
			break;
		case ENETLINK_CLASS_ATTR_MAX_INST_ATTR:
            iVal = ENCAP_TO_HS(ENETLINK_INST_ATTR_INTERFACE_LABEL);            
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );            
			break;       
        default:
            pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
            break;
    }        
}


/*---------------------------------------------------------------------------
** enetlinkParseInstanceRequest( )
**
** Respond to the Ethernet Link instance request
**---------------------------------------------------------------------------
*/

void enetlinkParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
    if ((iInstance >= 1) && (iInstance <= platformGetNumEnetLinkInstances()))
    {
        switch( pRequest->cfg.objectCfg.bService )
        {
            case SVC_GET_ATTR_ALL:
                enetlinkSendInstanceAttrAll( pRequest, iInstance );
                break;
            case SVC_GET_ATTR_SINGLE:
                enetlinkSendInstanceAttrSingle( pRequest, iInstance );
                break;        
            case SVC_SET_ATTR_SINGLE:
				enetlinkSetInstanceAttrSingle( pRequest, iInstance );
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
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
	}
}


/*---------------------------------------------------------------------------
** enetlinkSendInstanceAttrAll( )
**
** GetAttributeAll service for Ethernet Link instance is received
**---------------------------------------------------------------------------
*/

void enetlinkSendInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance )
{
    UINT8* pData;
    ENETLINK_INST_ATTR EnetLinkConfig;
    UINT8  bInterfaceLabelSize;
	UINT16 iMessageSize,
		   iUnimplementedAttributesSize;
	
	/* dynamically get Ethernet Link attributes */
	platformGetEnetLinkAttributes(iInstance, &EnetLinkConfig);
	bInterfaceLabelSize = (UINT8)strlen((char*)EnetLinkConfig.InterfaceLabel);
	
	iUnimplementedAttributesSize = ENETLINK_INTERFACE_COUNTER_SIZE + ENETLINK_MEDIA_COUNTER_SIZE;
#ifndef EIP_INTERFACE_CONTROL_SUPPORT
    iUnimplementedAttributesSize += ENETLINK_INTERFACE_CONTROL_SIZE;
    iMessageSize = (UINT16)(2*sizeof(UINT32) + MAC_ADDR_LENGTH + 4*sizeof(UINT8) + bInterfaceLabelSize + iUnimplementedAttributesSize);
#else
    iMessageSize = (UINT16)(2*sizeof(UINT32) + MAC_ADDR_LENGTH + 2*sizeof(UINT16) + 4*sizeof(UINT8) + bInterfaceLabelSize + iUnimplementedAttributesSize);
#endif

    EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iMessageSize);

    if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
    {
        pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;    /* Out of memory */    
        return;
    }
    
    pData = pRequest->cfg.objectCfg.pData;

	UINT32_SET( pData, EnetLinkConfig.lInterfaceSpeed);
    pData += sizeof(UINT32);
    UINT32_SET( pData, EnetLinkConfig.lInterfaceFlags );
    pData += sizeof(UINT32);
        
    memcpy( pData, EnetLinkConfig.macAddr, MAC_ADDR_LENGTH );
	pData += MAC_ADDR_LENGTH;

	/* space holders for unimplemented attributes */
	memset(pData, 0, iUnimplementedAttributesSize);
	pData += iUnimplementedAttributesSize;

#ifdef EIP_INTERFACE_CONTROL_SUPPORT
	UINT16_SET(pData, EnetLinkConfig.iInterfaceControlBits);
	pData += sizeof(UINT16);
	UINT16_SET(pData, EnetLinkConfig.iForcedInterfaceSpeed);
	pData += sizeof(UINT16);
#endif

	*pData++ = EnetLinkConfig.bInterfaceType;
	*pData++ = EnetLinkConfig.bInterfaceState;
	*pData++ = EnetLinkConfig.bAdminState;
    *pData++ = bInterfaceLabelSize;
        
    memcpy( pData, EnetLinkConfig.InterfaceLabel, bInterfaceLabelSize);    
	pData += bInterfaceLabelSize;
}


/*--------------------------------------------------------------------------------
** enetlinkSendInstanceAttrSingle( )
**
** GetAttributeSingle request has been received for the Ethernet Link object instance
**--------------------------------------------------------------------------------
*/

void enetlinkSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
    UINT8* pData;
	UINT32 lVal;
    UINT16 iTagSize;
    UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
    ENETLINK_INST_ATTR EnetLinkConfig;
    UINT8  bInterfaceLabelSize;
    
	/* dynamically get Ethernet Link attributes */
	platformGetEnetLinkAttributes(iInstance, &EnetLinkConfig);
	bInterfaceLabelSize = (UINT8)strlen((char*)EnetLinkConfig.InterfaceLabel);

    switch( iAttribute )
    {
        case ENETLINK_INST_ATTR_INTERFACE_SPEED:
			lVal = ENCAP_TO_HL(EnetLinkConfig.lInterfaceSpeed);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
            break;
        case ENETLINK_INST_ATTR_INTERFACE_FLAGS:
			lVal = ENCAP_TO_HL(EnetLinkConfig.lInterfaceFlags);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
            break;
        case ENETLINK_INST_ATTR_PHYSICAL_ADDRESS:
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, EnetLinkConfig.macAddr, MAC_ADDR_LENGTH );
            break;
#ifdef EIP_INTERFACE_CONTROL_SUPPORT
 		case ENETLINK_INST_ATTR_INTERFACE_CONTROL:
           EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(UINT16)*2));
            if ( pRequest->cfg.objectCfg.pData != NULL )
            {
                pData = pRequest->cfg.objectCfg.pData;
				UINT16_SET(pData, EnetLinkConfig.iInterfaceControlBits);
				pData += sizeof(UINT16);
				UINT16_SET(pData, EnetLinkConfig.iForcedInterfaceSpeed);
            }
			break;
#endif
		case ENETLINK_INST_ATTR_INTERFACE_TYPE:
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &EnetLinkConfig.bInterfaceType, sizeof(UINT8));
			break;
		case ENETLINK_INST_ATTR_INTERFACE_STATE:
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &EnetLinkConfig.bInterfaceState, sizeof(UINT8));
			break;
		case ENETLINK_INST_ATTR_ADMIN_STATE:
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &EnetLinkConfig.bAdminState, sizeof(UINT8));
			break;
		case ENETLINK_INST_ATTR_INTERFACE_LABEL:
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(bInterfaceLabelSize + sizeof(UINT8)));
            if ( pRequest->cfg.objectCfg.pData != NULL )
            {
                pData = pRequest->cfg.objectCfg.pData;                        
                *pData++ = bInterfaceLabelSize;            
                memcpy( (char*)pData, EnetLinkConfig.InterfaceLabel, bInterfaceLabelSize);            
            }
			break;

        default:
            pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
            break;
    }    
}

/*--------------------------------------------------------------------------------
** enetlinkSetInstanceAttrSingle( )
**
** SetAttributeSingle request has been received for the Ethernet Link object instance
**--------------------------------------------------------------------------------
*/

void enetlinkSetInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{    
    UINT16 iTagSize;
    UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
	UINT8  bValue;
	UINT8* pData;
    ENETLINK_INST_ATTR NewEnetLinkConfig, OldEnetLinkConfig;
    SESSION*  pSession;
    BOOL bSendData = TRUE;

    switch( iAttribute )
    {
#ifdef EIP_INTERFACE_CONTROL_SUPPORT
		case ENETLINK_INST_ATTR_INTERFACE_CONTROL:
			if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16)+sizeof(UINT16))
			{
				pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
				break;
			}
			if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16)+sizeof(UINT16))
			{
				pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
				break;
			}

			pData = pRequest->cfg.objectCfg.pData;
			NewEnetLinkConfig.iInterfaceControlBits = UINT16_GET(pData);
			if (NewEnetLinkConfig.iInterfaceControlBits & ~ENETLINK_INTERFACE_CONTROL_BITS_MASK)
			{
				pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
				break;
			}

			pData += sizeof(UINT16);
			NewEnetLinkConfig.iForcedInterfaceSpeed = UINT16_GET(pData);
			if (NewEnetLinkConfig.iInterfaceControlBits & ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED)
			{
				if (NewEnetLinkConfig.iForcedInterfaceSpeed != 0)
				{
					pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
					break;
				}
			}
			else if ((NewEnetLinkConfig.iForcedInterfaceSpeed != 10) &&
					 (NewEnetLinkConfig.iForcedInterfaceSpeed != 100))
			{
				pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
				break;
			}

			if (!platformTestSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_INTERFACE_CONTROL), iInstance, &NewEnetLinkConfig, &pRequest->bGeneralError))
				break;
			
			/* Values have been verified.  Send the response before setting the values,
			   otherwise the originating device may not get the response */
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			requestService( pRequest, &bSendData );

			/* Set the data */
			platformSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_INTERFACE_CONTROL), iInstance, &NewEnetLinkConfig);
			break;
#endif
		case ENETLINK_INST_ATTR_ADMIN_STATE:
			if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT8))
			{
				pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
				break;
			}
			if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT8))
			{
				pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
				break;
			}

			pData = pRequest->cfg.objectCfg.pData;
			bValue = *pData;
			if ((bValue < 1) || (bValue > 2))
			{
				pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
				break;
			}

			NewEnetLinkConfig.bAdminState = bValue;

			if (!platformTestSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_ADMIN_STATE), iInstance, &NewEnetLinkConfig, &pRequest->bGeneralError))
				break;

			/* Get the current value of the admin state */
			platformGetEnetLinkAttributes(iInstance, &OldEnetLinkConfig);

			if ((NewEnetLinkConfig.bAdminState == ENETLINK_ADMIN_STATE_DISABLED) &&
				(OldEnetLinkConfig.bAdminState == ENETLINK_ADMIN_STATE_ENABLED)) 
			{
				/* This ensures the request goes out before the "link" (IP address) is disabled */
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
				pRequest->nState = REQUEST_RESPONSE_RECEIVED;
				requestService( pRequest, &bSendData );

				/* remove all sessions associated with this IP address */
				for( pSession = gpnSessions-1; pSession >= gSessions; pSession-- )
				{
					if (pSession->lHostIPAddr == pRequest->lIPAddress)
						sessionRemove(pSession, FALSE);
				}

				platformSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_ADMIN_STATE), iInstance, &NewEnetLinkConfig);
				return;
			}

			platformSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_ADMIN_STATE), iInstance, &NewEnetLinkConfig);
			break;

        case ENETLINK_INST_ATTR_INTERFACE_SPEED:
        case ENETLINK_INST_ATTR_INTERFACE_FLAGS:
        case ENETLINK_INST_ATTR_PHYSICAL_ADDRESS:
		case ENETLINK_INST_ATTR_INTERFACE_TYPE:
		case ENETLINK_INST_ATTR_INTERFACE_STATE:
		case ENETLINK_INST_ATTR_INTERFACE_LABEL:
            pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
            break;
        default:
            pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			break;
    }    

    EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}
