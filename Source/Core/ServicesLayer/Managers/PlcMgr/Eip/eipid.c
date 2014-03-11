/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPID.C
**
** COPYRIGHT (c) 2001-2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Identity object
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
void idParseClassRequest( REQUEST* pRequest );
void idParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance );
void idSendClassAttrAll( REQUEST* pRequest );
void idSendClassAttrSingle( REQUEST* pRequest);
void idSendInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance );
void idSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance );
void idSetInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance );
void idReset( REQUEST* pRequest, UINT32 iInstance  );
UINT16 idGetStatus(UINT32 iInstance);
void idSetDefaultSTRINGI(STRINGI* pString);

EtIPIdentityInfo gIDInfo[MAX_IDENTITY_INSTANCE];    /* Identity information */

#ifdef EIP_ID_USER_ATTRIBUTES
UINT16 idOptionalAttributes[ID_ATTR_OPTIONAL_COUNT] = {
	ID_ATTR_USER_NAME, ID_ATTR_USER_DESCRIPTION, ID_ATTR_USER_LOCATION
};
#endif

UINT8 NULLStringI[8] = {1, 'e', 'n', 'g', 0xDA, 4, 0, 0};


/*---------------------------------------------------------------------------
** idInit( )
**
** Assign default values to the gIDInfo structure
**---------------------------------------------------------------------------
*/

void idInit(void)
{
	EtIPIdentityInfo* pID;

	/* default all identity instance to default identity information */
	for (pID = gIDInfo; pID < &gIDInfo[MAX_IDENTITY_INSTANCE]; pID++)
	{
		pID->iVendor = ID_VENDOR_ID;
		pID->iProductType = ID_PRODUCT_TYPE;
		pID->iProductCode = ID_PRODUCT_CODE;
		pID->bMajorRevision = ID_MAJOR_REVISION;
		pID->bMinorRevision = ID_MINOR_REVISION;
		pID->lSerialNumber = ID_SERIAL_NUMBER;
		pID->iStatus = idGetStatus((UINT16)((pID-gIDInfo)+1));        
		strcpy((char*)pID->productName, ID_PRODUCT_NAME);
		pID->bUseApplicationStatus = FALSE;
		idSetDefaultSTRINGI(&pID->szUserName);
		idSetDefaultSTRINGI(&pID->szUserDescription);
		idSetDefaultSTRINGI(&pID->szUserLocation);
	}
}

/*---------------------------------------------------------------------------
** idGetStatus( )
**
** Sets appropriate status bits based on the current state
**---------------------------------------------------------------------------
*/

UINT16 idGetStatus(UINT32 iInstance)
{
	EtIPIdentityInfo* pID = &gIDInfo[iInstance-1];
    UINT16 iStatus;
	
	if (pID->bUseApplicationStatus)
		return pID->iStatus;

	iStatus = ID_STATUS_CONFIGURED;

#if !defined(ET_IP_SCANNER) || defined(EIP_NO_CCO)
	iStatus |= connectionGetConnDeviceStatus();
#else
	/* Public definition of the status replaced with vendor specific 
	 * expected by RSNetworx
	 */ 
	if ( gbRunMode )
		iStatus |= ID_STATUS_CONNECTED_RUN_MODE;
	else
		iStatus |= ID_STATUS_CONNECTED_IDLE_MODE;
#endif

    return iStatus;
}
                            
/*---------------------------------------------------------------------------
** idParseClassInstanceRequest( )
**
** Determine if it's request for the Class or for the particular instance 
**---------------------------------------------------------------------------
*/

void idParseClassInstanceRequest( REQUEST* pRequest )
{
    UINT16 iTagSize;
    UINT32 iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
    
    if ( iInstance == 0 )
        idParseClassRequest( pRequest );
    else
        idParseInstanceRequest( pRequest, iInstance );
}

/*---------------------------------------------------------------------------
** idParseClassRequest( )
**
** Respond to the class request
**---------------------------------------------------------------------------
*/

void idParseClassRequest( REQUEST* pRequest )
{
    switch( pRequest->cfg.objectCfg.bService )
    {
        case SVC_GET_ATTR_ALL:
            idSendClassAttrAll( pRequest );
            break;
        case SVC_GET_ATTR_SINGLE:
            idSendClassAttrSingle( pRequest);
            break;
        default:
            pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
            break;
    }
}

/*---------------------------------------------------------------------------
** idParseInstanceRequest( )
**
** Respond to the instance request
**---------------------------------------------------------------------------
*/

void idParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
    if ( iInstance <= MAX_IDENTITY_INSTANCE)
    {
        switch( pRequest->cfg.objectCfg.bService )
        {
            case SVC_GET_ATTR_ALL:
                idSendInstanceAttrAll( pRequest, iInstance );
                break;
            case SVC_GET_ATTR_SINGLE:
                idSendInstanceAttrSingle( pRequest, iInstance );
                break;
			case SVC_SET_ATTR_SINGLE:
                idSetInstanceAttrSingle( pRequest, iInstance );
				break;
            case SVC_RESET:
                idReset( pRequest, iInstance  );
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
** idSendClassAttrAll( )
**
** GetAttributeAll service for Identity class is received 
**---------------------------------------------------------------------------
*/

void idSendClassAttrAll( REQUEST* pRequest )
{
	UINT8 *pData;

    EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16)*4);
	if (pRequest->cfg.objectCfg.pData == NULL)
	{
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
        pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;    /* Out of memory */
        return;
	}

	pData = pRequest->cfg.objectCfg.pData;

	UINT16_SET(pData, ID_CLASS_REVISION);
	pData += sizeof(UINT16);
	UINT16_SET(pData, MAX_IDENTITY_INSTANCE);
	pData += sizeof(UINT16);
	UINT16_SET(pData, ID_CLASS_ATTR_MAX_INST_ATTR);
	pData += sizeof(UINT16);
#ifdef EIP_ID_USER_ATTRIBUTES
	UINT16_SET(pData, ID_ATTR_USER_LOCATION);
	pData += sizeof(UINT16);
#else
	UINT16_SET(pData, ID_ATTR_PRODUCT_NAME);
	pData += sizeof(UINT16);
#endif
}

/*--------------------------------------------------------------------------------
** idSendClassAttrSingle( )
**
** GetAttributeSingle request has been received for the Identity class
**--------------------------------------------------------------------------------
*/

void idSendClassAttrSingle( REQUEST* pRequest)
{
    UINT16 iVal;
    UINT16 iTagSize;
#if defined (EIP_ID_USER_ATTRIBUTES)
	UINT8* pData;
	UINT16 *p16;
#endif
    UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);

    switch( iAttribute )
    {
        case ID_CLASS_ATTR_REVISION:
            iVal = ENCAP_TO_HS(ID_CLASS_REVISION);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case ID_CLASS_ATTR_MAX_INSTANCE:
		case ID_CLASS_ATTR_NUM_INSTANCES:
            iVal = ENCAP_TO_HS(MAX_IDENTITY_INSTANCE);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
#if defined (EIP_ID_USER_ATTRIBUTES)
		case ID_CLASS_ATTR_OPT_ATTR_LIST:
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16)*(ID_ATTR_OPTIONAL_COUNT+1));

			pData = pRequest->cfg.objectCfg.pData;

			UINT16_SET(pData, ID_ATTR_OPTIONAL_COUNT);
			pData += sizeof(UINT16);
			for (p16 = idOptionalAttributes; 
				p16 < &idOptionalAttributes[ID_ATTR_OPTIONAL_COUNT]; 
				p16++, pData += sizeof(UINT16))
			{
				UINT16_SET(pData, *p16);
			}
			break;
#endif
        case ID_CLASS_ATTR_MAX_CLASS_ATTR:
            iVal = ENCAP_TO_HS(ID_CLASS_ATTR_MAX_INST_ATTR);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case ID_CLASS_ATTR_MAX_INST_ATTR:
#ifdef EIP_ID_USER_ATTRIBUTES
            iVal = ENCAP_TO_HS(ID_ATTR_USER_LOCATION);
#else
            iVal = ENCAP_TO_HS(ID_ATTR_PRODUCT_NAME);
#endif
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        default:
            pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
            break;
    }
}

/*---------------------------------------------------------------------------
** idSendInstanceAttrAll( )
**
** GetAttributeAll service for Identity instance is received
**---------------------------------------------------------------------------
*/

void idSendInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance )
{
    UINT8* pData;
	EtIPIdentityInfo* pID = &gIDInfo[iInstance-1];
    UINT8 bProductNameSize = (UINT8)strlen((char*)pID->productName);    /* Product name size */
	UINT16 iResponseSize;

	iResponseSize = (UINT16)(ID_INSTANCE_ATTRIBUTE_SIZE + bProductNameSize + sizeof(UINT8));

    EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iResponseSize);

    if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
    {
        pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;    /* Out of memory */    
        return;
    }
        
    pData = pRequest->cfg.objectCfg.pData;

    UINT16_SET(pData, pID->iVendor);
    pData += sizeof(UINT16);

    UINT16_SET(pData, pID->iProductType);
    pData += sizeof(UINT16);

    UINT16_SET(pData, pID->iProductCode);
    pData += sizeof(UINT16);

    *pData++ = pID->bMajorRevision;
    *pData++ = pID->bMinorRevision;

    pID->iStatus = idGetStatus(iInstance);        
    UINT16_SET(pData, pID->iStatus);
    pData += sizeof(UINT16);

    UINT32_SET(pData, pID->lSerialNumber);
    pData += sizeof(UINT32);

    *pData++ = bProductNameSize;
        
    memcpy( pData, pID->productName, bProductNameSize );    
	pData += bProductNameSize;
}


/*--------------------------------------------------------------------------------
** idSendInstanceAttrSingle( )
**
** GetAttributeSingle request has been received for the Identity object instance
**--------------------------------------------------------------------------------
*/

void idSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
    UINT8* pData;
    UINT32 lVal;
    UINT16 iVal;
    UINT16 iTagSize;
    UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
	EtIPIdentityInfo* pID = &gIDInfo[iInstance-1];
    UINT8  bProductNameSize = (UINT8)strlen((char*)pID->productName);    /* Product name size */

    switch( iAttribute )
    {
        case ID_ATTR_VENDOR_ID:
            iVal = ENCAP_TO_HS(pID->iVendor);            
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case ID_ATTR_DEVICE_TYPE:
            iVal = ENCAP_TO_HS(pID->iProductType);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case ID_ATTR_PRODUCT_CODE:
            iVal = ENCAP_TO_HS(pID->iProductCode);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case ID_ATTR_REVISION:
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16) );        
            if ( pRequest->cfg.objectCfg.pData != NULL )
            {
                pData = pRequest->cfg.objectCfg.pData;            
                *pData = pID->bMajorRevision;
                *(pData+1) = pID->bMinorRevision;            
            }
            break;
        case ID_ATTR_STATUS:
            pID->iStatus = idGetStatus(iInstance);        
            iVal = ENCAP_TO_HS(pID->iStatus);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
            break;
        case ID_ATTR_SERIAL_NBR:
            lVal = ENCAP_TO_HL(pID->lSerialNumber);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );        
            break;
        case ID_ATTR_PRODUCT_NAME:
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(bProductNameSize + sizeof(UINT8)) );        
            if ( pRequest->cfg.objectCfg.pData != NULL )
            {
                pData = pRequest->cfg.objectCfg.pData;                        
                *pData++ = bProductNameSize;            
                memcpy( (char*)pData, pID->productName, bProductNameSize );            
            }
            break;
#ifdef EIP_ID_USER_ATTRIBUTES
		case ID_ATTR_USER_NAME:
			if (pID->szUserName.bLength == 0)
			{
				EtIP_realloc(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULLStringI, sizeof(NULLStringI));
				break;
			}

			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(pID->szUserName.bLength+8) );
			if ( pRequest->cfg.objectCfg.pData == NULL )
			{
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				break;
			}

			pData = pRequest->cfg.objectCfg.pData;
			clientSetSTRINGI(pData, &pID->szUserName);
			break;
		case ID_ATTR_USER_DESCRIPTION:
			if (pID->szUserDescription.bLength == 0)
			{
				EtIP_realloc(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULLStringI, sizeof(NULLStringI));
				break;
			}

			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(pID->szUserDescription.bLength+8) );        
			if ( pRequest->cfg.objectCfg.pData == NULL )
			{
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				break;
			}

			pData = pRequest->cfg.objectCfg.pData;
			clientSetSTRINGI(pData, &pID->szUserDescription);
			break;
		case ID_ATTR_USER_LOCATION:
			if (pID->szUserLocation.bLength == 0)
			{
				EtIP_realloc(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULLStringI, sizeof(NULLStringI));
				break;
			}

			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(pID->szUserLocation.bLength+8) );
			if ( pRequest->cfg.objectCfg.pData == NULL )
			{
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				break;
			}

			pData = pRequest->cfg.objectCfg.pData;
			clientSetSTRINGI(pData, &pID->szUserLocation);
			break;
#endif
        default:
            pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
            break;
    }            
}

/*--------------------------------------------------------------------------------
** idSetInstanceAttrSingle( )
**
** SetAttributeSingle request has been received for the Identity object instance
**--------------------------------------------------------------------------------
*/
void idSetInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
#ifdef EIP_ID_USER_ATTRIBUTES
    UINT16 iTagSize;
    UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
	EtIPIdentityInfo* pID = &gIDInfo[iInstance-1];

    switch( iAttribute )
    {
        case ID_ATTR_VENDOR_ID:
        case ID_ATTR_DEVICE_TYPE:
        case ID_ATTR_PRODUCT_CODE:
        case ID_ATTR_REVISION:
        case ID_ATTR_STATUS:
        case ID_ATTR_SERIAL_NBR:
        case ID_ATTR_PRODUCT_NAME:
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
            break;
		case ID_ATTR_USER_NAME:
			if (pRequest->cfg.objectCfg.iDataSize < 8)
			{
				pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
				break;
			}

			if (pRequest->cfg.objectCfg.pData[7]+8 != pRequest->cfg.objectCfg.iDataSize)
			{
				pRequest->bGeneralError = ROUTER_ERROR_PARTIAL_DATA;
				break;
			}

			clientGetSTRINGI(&pID->szUserName, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize);
			break;
		case ID_ATTR_USER_DESCRIPTION:
			if (pRequest->cfg.objectCfg.iDataSize < 8)
			{
				pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
				break;
			}

			if (pRequest->cfg.objectCfg.pData[7]+8 != pRequest->cfg.objectCfg.iDataSize)
			{
				pRequest->bGeneralError = ROUTER_ERROR_PARTIAL_DATA;
				break;
			}

			clientGetSTRINGI(&pID->szUserDescription, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize);
			break;
		case ID_ATTR_USER_LOCATION:
			if (pRequest->cfg.objectCfg.iDataSize < 8)
			{
				pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
				break;
			}

			if (pRequest->cfg.objectCfg.pData[7]+8 != pRequest->cfg.objectCfg.iDataSize)
			{
				pRequest->bGeneralError = ROUTER_ERROR_PARTIAL_DATA;
				break;
			}

			clientGetSTRINGI(&pID->szUserLocation, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize);
			break;
        default:
            pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
            break;
    }
#else
	/* compiler warning */
	iInstance = iInstance;

	pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
#endif

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}

/*---------------------------------------------------------------------------
** idReset( )
**
** Reset request is recieved.
**---------------------------------------------------------------------------
*/

void idReset( REQUEST* pRequest, UINT32 iInstance)
{    
    UINT8  bReset = 0;
    
    if ( pRequest->cfg.objectCfg.iDataSize )
    {
        bReset = *pRequest->cfg.objectCfg.pData;
        EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );    
        if ( bReset > 1)    /* Only 0-1 could be passed */
        {
            pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
            return;
        }
    }
    
    EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );    
    
    ucmmSendObjectResponse( pRequest );    /* We are about to reset - send the response first */
    
    connectionRemoveAll();
    requestRemoveAll();    
	requestGroupRemoveAll();    
	sessionRemoveAll();

	/* Specific object data that needs reset */
	connmgrReset(bReset);
	tcpipReset(bReset);

	notifyEvent(NM_ID_RESET, iInstance, bReset);
}

/*---------------------------------------------------------------------------
** idSetDefaultSTRINGI( )
**
** Set a STRINGI structure to a zero length english string
**---------------------------------------------------------------------------
*/
void idSetDefaultSTRINGI(STRINGI* pString)
{
	memcpy(pString->aLang, "eng", 3);
	pString->bEPATH = 0xDA;					/* SHORT_STRING type */
	pString->iCharacterSet = 4;
	pString->bLength = 0;
}

/*---------------------------------------------------------------------------
** idCheckElectronicKey( )
**
** Check that an electronic key segment matches this device
**---------------------------------------------------------------------------
*/
BOOL idCheckElectronicKey(EtIPDeviceID* pDeviceID, UINT8* pbGeneralError, UINT16* piExtendedError)
{
	if (pDeviceID->bMajorRevision & KEY_COMPATIBILITY_FLAG)
	{
		/* Compatible keying - accept lower major revisions, no wildcards */

		if ((pDeviceID->iVendorID == 0) || 
			(pDeviceID->iProductCode == 0) ||
			((pDeviceID->iProductType == 0) && (gIDInfo[0].iProductType != 0)) ||
			((pDeviceID->bMajorRevision & MAJOR_REVISION_MASK) == 0))
		{
			*pbGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
			*piExtendedError = 0;
			return FALSE;
		}

		/* Check if Vendor Id and Product code are filled and match the target ones */
		if ((pDeviceID->iVendorID != gIDInfo[0].iVendor) ||
			 (pDeviceID->iProductCode != gIDInfo[0].iProductCode) )
		{
			*pbGeneralError = ROUTER_ERROR_FAILURE;
			*piExtendedError = ROUTER_EXT_ERR_VENDOR_PRODUCT_CODE_MISMATCH;
			return FALSE;
		}

		/* Check if Product type is filled and match the target one */
		if (pDeviceID->iProductType != gIDInfo[0].iProductType)
		{
			*pbGeneralError = ROUTER_ERROR_FAILURE;
			*piExtendedError = ROUTER_EXT_ERR_PRODUCT_TYPE_MISMATCH;
			return FALSE;
		}

		/* Check if Revisions are filled and match the target ones */
		if ((pDeviceID->bMajorRevision & MAJOR_REVISION_MASK) > gIDInfo[0].bMajorRevision)
		{
			*pbGeneralError = ROUTER_ERROR_FAILURE;
			*piExtendedError = ROUTER_EXT_ERR_REVISION_MISMATCH;
			return FALSE;
		}

	}
	else
	{
		/* exact keying, but allow wildcards */

		/* Check if Vendor Id and Product code are filled and match the target ones */
		if ( ( pDeviceID->iVendorID && pDeviceID->iVendorID != gIDInfo[0].iVendor ) ||
			 ( pDeviceID->iProductCode && pDeviceID->iProductCode != gIDInfo[0].iProductCode ) )
		{
			*pbGeneralError = ROUTER_ERROR_FAILURE;
			*piExtendedError = ROUTER_EXT_ERR_VENDOR_PRODUCT_CODE_MISMATCH;
			return FALSE;
		}

		/* Check if Product type is filled and match the target one */
		if ( pDeviceID->iProductType && pDeviceID->iProductType != gIDInfo[0].iProductType )
		{
			*pbGeneralError = ROUTER_ERROR_FAILURE;
			*piExtendedError = ROUTER_EXT_ERR_PRODUCT_TYPE_MISMATCH;
			return FALSE;
		}

		if (((pDeviceID->bMajorRevision & MAJOR_REVISION_MASK) && 
			(pDeviceID->bMajorRevision & MAJOR_REVISION_MASK) != gIDInfo[0].bMajorRevision) ||
			((pDeviceID->bMinorRevision) && 
			(pDeviceID->bMinorRevision) != gIDInfo[0].bMinorRevision))
		{
			*pbGeneralError = ROUTER_ERROR_FAILURE;
			*piExtendedError = ROUTER_EXT_ERR_REVISION_MISMATCH;
			return FALSE;
		}
	}

	return TRUE;
}




