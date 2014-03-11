/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPPCCC.C
**
** COPYRIGHT (c) 2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** PCCC object implementation 
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
void pcccParseClassRequest( REQUEST* pRequest );
BOOL pcccParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance );
void pcccSendClassAttrAll( REQUEST* pRequest );
void pcccSendClassAttrSingle( REQUEST* pRequest);
BOOL pcccExecuteService( REQUEST* pRequest);
BOOL pcccExecuteLocalPCCC( REQUEST* pRequest);

const UINT8 PCCC_EXTENSION_COMMAND_LIST[] = {0x06, 0x07, 0x0E, 0x0F};


BOOL pcccIsExtentionCommandList(UINT8 bCommand)
{
	INT32 i;

	for (i = 0; i < (INT32)sizeof(PCCC_EXTENSION_COMMAND_LIST); i++)
	{
		if ((bCommand & PCCC_COMMAND_COMMAND_MASK) == PCCC_EXTENSION_COMMAND_LIST[i])
			return TRUE;
	}

	return FALSE;
}

BOOL pcccParseClassInstanceRqst( REQUEST* pRequest )
{
    UINT16 iTagSize;
    UINT32 iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
    
    if ( iInstance == 0 )
    {
		pcccParseClassRequest( pRequest );
		return TRUE;
	}

    return pcccParseInstanceRequest( pRequest, iInstance );

}

void pcccParseClassRequest( REQUEST* pRequest )
{
    switch( pRequest->cfg.objectCfg.bService )
    {
        case SVC_GET_ATTR_ALL:
            pcccSendClassAttrAll( pRequest );
            break;
        case SVC_GET_ATTR_SINGLE:
            pcccSendClassAttrSingle( pRequest);
            break;
        default:        
            pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
            break;
    }
}

BOOL pcccParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
	if (iInstance != 1)
	{
        pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

    switch( pRequest->cfg.objectCfg.bService )
    {
        case PCCC_EXECUTE_SERVICE:
            return pcccExecuteService(pRequest);
		case PCCC_EXECUTE_LOCAL_PCCC:
            return pcccExecuteLocalPCCC(pRequest);
    }

	pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
	return TRUE;
}


void pcccSendClassAttrAll( REQUEST* pRequest )
{
    PCCC_CLASS_ATTRIBUTE pcccAttr;

    pcccAttr.iRevision = ENCAP_TO_HS(PCCC_CLASS_REVISION);
    pcccAttr.iMaxInstance = ENCAP_TO_HS(1);
	pcccAttr.iNumInstance = ENCAP_TO_HS(1);
    pcccAttr.iMaxClassAttr = ENCAP_TO_HS(PCCC_CLASS_ATTR_MAX_INST_ATTR);
    pcccAttr.iMaxInstanceAttr = ENCAP_TO_HS(0);
    
    EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&pcccAttr, sizeof(PCCC_CLASS_ATTRIBUTE) );
}

void pcccSendClassAttrSingle( REQUEST* pRequest)
{
    UINT16 iVal;
    UINT16 iTagSize;
    UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);

    switch( iAttribute )
    {
        case PCCC_CLASS_ATTR_REVISION:
            iVal = ENCAP_TO_HS(PCCC_CLASS_REVISION);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case PCCC_CLASS_ATTR_MAX_INSTANCE:
            iVal = ENCAP_TO_HS(1);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
		case PCCC_CLASS_ATTR_NUM_INSTANCES:
            iVal = ENCAP_TO_HS(1);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case PCCC_CLASS_ATTR_MAX_CLASS_ATTR:
            iVal = ENCAP_TO_HS(PCCC_CLASS_ATTR_MAX_INST_ATTR);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case PCCC_CLASS_ATTR_MAX_INST_ATTR:
            iVal = ENCAP_TO_HS(0);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        default:
            pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
            break;
    }
}

BOOL pcccExecuteService( REQUEST* pRequest)
{
	UINT8* pData = pRequest->cfg.objectCfg.pData;
	UINT8 bLength = *pData;

	if ((bLength > pRequest->cfg.objectCfg.iDataSize) ||
		(pRequest->cfg.objectCfg.iDataSize - bLength < 4))
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return TRUE;
	}

    pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
    notifyEvent( NM_CLIENT_PCCC_REQUEST_RECEIVED, pRequest->nIndex, 0 );
	return FALSE;
}

BOOL pcccExecuteLocalPCCC( REQUEST* pRequest)
{
	if (pRequest->cfg.objectCfg.iDataSize < 4)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return TRUE;
	}

    pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
    notifyEvent( NM_CLIENT_PCCC_REQUEST_RECEIVED, pRequest->nIndex, 0 );
	return FALSE;
}

