/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPMBUS.C
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Bridges functionality between EtherNet/IP and Modbus stacks. 
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

#include "../eipinc.h"

#ifdef ET_IP_MODBUS

/* local functions */
void eipmbsTranslateResponse(REQUEST* pRequest, MBSResponse* pResponse);
void eipmbsWriteSingleRegister(REQUEST* pRequest, UINT32 iRegister, UINT16 iRegisterValue);
void eipmbsWriteSingleCoil(REQUEST* pRequest, UINT16 iCoil, UINT8 iValue);
BOOL eipmbsIdentityParseRequest( REQUEST* pRequest, MBSRequest* pModbusRequest );
void eipmbsIdParseClassRequest( REQUEST* pRequest );
void eipmbsIdClassGAARequest( REQUEST* pRequest );
void eipmbsIdClassGASRequest( REQUEST* pRequest );
void eipmbsIdParseInstanceRequest( REQUEST* pRequest, MBSResponse* pResponse);
void eipmbsIdInstanceGAAResponse( REQUEST* pRequest, MBSResponse* pResponse );
void eipmbsIdInstanceGASResponse( REQUEST* pRequest, MBSResponse* pResponse );
BOOL eipmbsAssemblyParseRequest( REQUEST* pRequest, MBSRequest* pModbusRequest );
void eipmbsAssemblyParseClassRequest( REQUEST* pRequest );
void eipmbsAssemblyClassGASRequest( REQUEST* pRequest );
BOOL eipmbsAssemblyParseInstRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance );
BOOL eipmbsAssemblyInstSetMemberRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance );
void eipmbsAssemblyInstSetMemRsp( REQUEST* pRequest, MBSResponse* pResponse );
BOOL eipmbsAssemblyInstGetMemberRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance );
void eipmbsAssemblyInstGetMemRsp( REQUEST* pRequest, MBSResponse* pResponse );
BOOL eipmbsParameterParseRequest(REQUEST* pRequest, MBSRequest* pModbusRequest);
void eipmbsParamParseClassRequest( REQUEST* pRequest );
BOOL eipmbsParamParseInstanceRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance);
BOOL eipmbsParamInstSASRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance);
BOOL eipmbsParamInstGASRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance);
void eipmbsParamInstSASRsp( REQUEST* pRequest, MBSResponse* pResponse );
void eipmbsParamInstGASRsp( REQUEST* pRequest, MBSResponse* pResponse );
BOOL eipmbsModbusParseRequest( REQUEST* pRequest, MBSRequest* pModbusRequest);
void eipmbsModbusParseClassRequest( REQUEST* pRequest );
void eipmbsModbusClassGAARequest( REQUEST* pRequest );
void eipmbsModbusClassGASRequest( REQUEST* pRequest );
BOOL eipmbsModbusParseInstanceRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance );
BOOL eipmbsMbusReadBitsRqst(REQUEST* pRequest, MBSRequest* pModbusRequest, UINT8 bFunction);
void eipmbsMbusReadBitsRsp(REQUEST* pRequest, MBSResponse* pResponse);
BOOL eipmbsMbusReadRegistersRequest(REQUEST* pRequest, MBSRequest* pModbusRequest, UINT8 bFunction);
void eipmbsMbusReadRegistersResponse(REQUEST* pRequest, MBSResponse* pResponse);
BOOL eipmbsMbusWriteCoilsRqst(REQUEST* pRequest, MBSRequest* pModbusRequest);
void eipmbsMbusWriteCoilsRsp(REQUEST* pRequest, MBSResponse* pResponse);
BOOL eipmbsMbusWriteHoldRegRqst(REQUEST* pRequest, MBSRequest* pModbusRequest);
void eipmbsMbusWriteHoldRegRsp(REQUEST* pRequest, MBSResponse* pResponse);
BOOL eipmbsMbusExecuteModbusRqst(REQUEST* pRequest, MBSRequest* pModbusRequest);
void eipmbsMbusExecuteModbusRsp(REQUEST* pRequest, MBSResponse* pResponse);
void eipmbsHandleExpMessageError(REQUEST* pRequest, UINT8 bModbusError);
void eipmbsGenerateAddressData(REQUEST* pRequest, MBSRequest* pMBSRequest);
UINT32 eipmbsGenerateTimeoutValue(REQUEST* pRequest);

/*---------------------------------------------------------------------------
** eipmbsParseCIPModbusRequest( )
**
** Handle a "Modbus" object request
**---------------------------------------------------------------------------
*/
void eipmbsParseCIPModbusRequest( REQUEST* pRequest)
{
	UINT32 iClass;
	UINT16 iTagSize;
	BOOL bInternalMsg = TRUE;		/* Determines if a Modbus message is needed to generate a CIP response */
	CONNECTION *pConnection;
	MBSRequest Request;
	UINT8* pData;

	if (pRequest->nType == CLASS3_REQUEST)
	{
		pConnection = connectionGetFromSerialNumber( pRequest->cfg.class3Cfg.iConnectionSerialNbr, pRequest->cfg.class3Cfg.iVendorID, pRequest->cfg.class3Cfg.lDeviceSerialNbr );

		if (pConnection != NULL)
		{
			/* assign the Modbus address from the connection */
			pRequest->lIPAddress = platformAddrFromPath( (const char*)pConnection->cfg.modbusCfg.pModbusAddress, (INT32)pConnection->cfg.modbusCfg.iModbusAddressSize);
		}
		else
		{
			/* Connection removed from beneath us */
			pRequest->bGeneralError = ROUTER_ERROR_FAILURE;
			pRequest->iExtendedError = ROUTER_EXT_ERR_LINK_ADDR_NOT_AVAILABLE;
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}
	}

	Request.lMessageTimeout = eipmbsGenerateTimeoutValue(pRequest);

	/* Populate Modbus address */
	eipmbsGenerateAddressData(pRequest, &Request);

	iClass = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_CLASS, 0, &iTagSize);
	if (iTagSize == 0)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
	}
	else
	{
		switch(iClass)
		{
		case ID_CLASS:
			bInternalMsg = eipmbsIdentityParseRequest( pRequest, &Request );
			break;
		case ASSEMBLY_CLASS:
			bInternalMsg = eipmbsAssemblyParseRequest( pRequest, &Request );
			break;
		case CONNMGR_CLASS:
			/* All requests to the Connection Manager object don't require a Modbus message */
			break;
		case PARAMETER_CLASS:
			bInternalMsg = eipmbsParameterParseRequest( pRequest, &Request );
			break;
		case MODBUS_CLASS:
			bInternalMsg = eipmbsModbusParseRequest( pRequest, &Request );
			break;
		default:    /* Do not support this class yet */
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
			break;
		}
	}

	/* If message is being handled by the Modbus translator, wait for the response */
	if (!bInternalMsg)	
	{
		pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
		return;
	}

#ifdef ET_IP_MODBUS_SERIAL
	if (pRequest->bModbusSerialAddress != 0)
	{
		if (modbusIsSerialSessionActive(pRequest->lIPAddress, pRequest->lHostIPAddr))
		{
			eipmbsTranslateResponse(pRequest, NULL);
			/* remove request data for errors */
			if (pRequest->bGeneralError != 0)
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}
	}
	else
#endif /* ET_IP_MODBUS_SERIAL */
	/* Have we communicated with this Modbus device recently? If so,
	   we don't need to verify the availability of the device for
	   non-translatable messages */
	if (modbusIsSessionActive(pRequest->lIPAddress, pRequest->lHostIPAddr))
	{
		eipmbsTranslateResponse(pRequest, NULL);
		/* remove request data for errors */
		if (pRequest->bGeneralError != 0)
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}


	/* Generate a "ping" message to ensure the device is available
	   If device is available, continue with parsing message,
	   Otherwise return respective "device not there" response */
	Request.bFunction = MBUS_READ_DEVICE_INFORMATION;
	Request.iDataSize = 3;

	pData = Request.requestData;

	*pData++ = 0x0E;
	*pData++ = 0x02;
	*pData++ = 0x04;

	pRequest->nModbusRequestID = modbusSendRequest(&Request, FALSE);
	if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
	}	

	pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
}

/*---------------------------------------------------------------------------
** eipModbusCallbackFunction( )
**
** Handles notifications from the Modbus stack
**---------------------------------------------------------------------------
*/
void eipModbusCallbackFunction( INT32 nEvent, INT32 nParam1, INT32 nParam2 )
{
	REQUEST* pRequest;
	CONNECTION* pConnection;
	MBSResponse Response;
    FWD_OPEN    FwdOpen;

	/* compiler warning */
	nParam2 = nParam2;

	switch(nEvent)
	{
	case NM_MBUS_REQUEST_RESPONSE_RECEIVED:
		/* Find our request */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if (pRequest->nModbusRequestID == nParam1)
			{
				modbusGetResponse(pRequest->nModbusRequestID, &Response);
				eipmbsTranslateResponse(pRequest, &Response);
				if (pRequest->bGeneralError != 0)
					EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
				break;
			}
		}

		/* already found a request */
		if (pRequest < gpnRequests)
			break;

		for (pConnection = gConnections; pConnection < gpnConnections; pConnection++)
		{
			if (pConnection->nModbusReadRequestID == nParam1)
			{
				modbusGetResponse(nParam1, &Response);
				eipmbsconnmgrTranslateReadRsp(pConnection, &Response);
				break;
			}

			if (pConnection->nModbusWriteRequestID == nParam1)
			{
				modbusGetResponse(nParam1, &Response);
				eipmbsconnmgrTranslateWriteRsp(pConnection, &Response);
				break;
			}
		}

		break;
	case NM_MBUS_REQUEST_TIMED_OUT:
		/* Find our request */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if (pRequest->nModbusRequestID == nParam1)
			{
				pRequest->bGeneralError = ROUTER_ERROR_FAILURE;
				pRequest->iExtendedError = ROUTER_EXT_ERR_UNCONNECTED_SEND_TIMED_OUT;
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
				pRequest->nState = REQUEST_RESPONSE_RECEIVED;
				break;
			}
		}

		/* already found a request */
		if (pRequest < gpnRequests)
			break;

		for (pConnection = gConnections; pConnection < gpnConnections; pConnection++)
		{
			/* Remove the connection if the device went away */
			if ((pConnection->nModbusReadRequestID == nParam1) ||
				(pConnection->nModbusWriteRequestID == nParam1))
			{
				if (pConnection->lConnectionState == ConnectionNonExistent)
				{
					if ((pRequest = requestGetByRequestId(pConnection->nRequestId)) != NULL)
					{
						connmgrStats.iOpenOtherRejects++;
					    connmgrInitFwdOpenTypeFromBuf(pRequest->cfg.objectCfg.pData, &FwdOpen, pRequest->cfg.objectCfg.bService == LARGE_FWD_OPEN_CMD_CODE);
						connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_LINK_ADDR_NOT_AVAILABLE);
						pRequest->nState = REQUEST_RESPONSE_RECEIVED;
					}
				}

				connectionRemove(pConnection, FALSE);
				break;
			}
		}
		break;
	case NM_MBUS_REQUEST_FAILED_INVALID_NETWORK_PATH:
		/* Find our request */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if (pRequest->nModbusRequestID == nParam1)
			{
				pRequest->bGeneralError = ROUTER_ERROR_FAILURE;
				pRequest->iExtendedError = ROUTER_EXT_ERR_LINK_ADDR_NOT_AVAILABLE;
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
				pRequest->nState = REQUEST_RESPONSE_RECEIVED;
				break;
			}
		}

		/* already found a request */
		if (pRequest < gpnRequests)
			break;

		for (pConnection = gConnections; pConnection < gpnConnections; pConnection++)
		{
			/* Remove the connection if the device went away */
			if ((pConnection->nModbusReadRequestID == nParam1) ||
				(pConnection->nModbusWriteRequestID == nParam1))
			{
				if (pConnection->lConnectionState == ConnectionNonExistent)
				{
					if ((pRequest = requestGetByRequestId(pConnection->nRequestId)) != NULL)
					{
						connmgrStats.iOpenOtherRejects++;
					    connmgrInitFwdOpenTypeFromBuf(pRequest->cfg.objectCfg.pData, &FwdOpen, pRequest->cfg.objectCfg.bService == LARGE_FWD_OPEN_CMD_CODE);
						connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_LINK_ADDR_NOT_AVAILABLE);
						pRequest->nState = REQUEST_RESPONSE_RECEIVED;
					}
				}

				connectionRemove(pConnection, FALSE);
				break;
			}
		}
		break;
	}
}

/*---------------------------------------------------------------------------
** eipmbsTranslateResponse( )
**
** Translate a Modbus response into a CIP response
**---------------------------------------------------------------------------
*/
void eipmbsTranslateResponse(REQUEST* pRequest, MBSResponse* pResponse)
{
	UINT16 iTagSize;
	UINT32 iClass = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_CLASS, 0, &iTagSize),
		   iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);

	/* If request already has an error associated with it, just return now */
	if (pRequest->bGeneralError != 0)
	{
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	switch(iClass)
	{
	case ID_CLASS:
		if (iInstance == 0)
			eipmbsIdParseClassRequest(pRequest);
		else
			eipmbsIdParseInstanceRequest(pRequest, pResponse);

		/* All Identity translations only require a single Modbus transaction,
		   so this CIP request is done */
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		break;
	case ASSEMBLY_CLASS:
		if (iInstance == 0)
		{
			/* Class attributes/services only require a single Modbus transaction.
			so this CIP request is done */
			eipmbsAssemblyParseClassRequest(pRequest);
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}

		switch (pRequest->cfg.objectCfg.bService)
		{
		case SVC_SET_MEMBER:
			eipmbsAssemblyInstSetMemRsp(pRequest, pResponse);
			break;

		case SVC_GET_MEMBER:
			eipmbsAssemblyInstGetMemRsp(pRequest, pResponse);
			break;

		default:
			DumpStr3(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, pRequest->lIPAddress, pRequest->lHostIPAddr,
				"eipmbsTranslateResponse Internal Logic FAILURE - Unsupported request: Class = %d, Instance = %d, Service = %d", iClass, iInstance, pRequest->cfg.objectCfg.bService);
		}
		break;
	case CONNMGR_CLASS:
		eipmbsconnmgrParseRequest(pRequest);
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		break;
	case PARAMETER_CLASS:
		if (iInstance == 0)
		{
			/* Class attributes/services only require a single Modbus transaction.
			so this CIP request is done */
			eipmbsParamParseClassRequest(pRequest);
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}

		switch (pRequest->cfg.objectCfg.bService)
		{
		case SVC_SET_ATTR_SINGLE:
			eipmbsParamInstSASRsp(pRequest, pResponse);
			break;

		case SVC_GET_ATTR_SINGLE:
			eipmbsParamInstGASRsp(pRequest, pResponse);
			break;

		default:
			DumpStr3(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, pRequest->lIPAddress, pRequest->lHostIPAddr,
				"eipmbsTranslateResponse Internal Logic FAILURE - Unsupported request: Class = %d, Instance = %d, Service = %d", iClass, iInstance, pRequest->cfg.objectCfg.bService);
		}
		break;
	case MODBUS_CLASS:
		if (iInstance == 0)
		{
			/* Class attributes/services only require a single Modbus transaction.
			so this CIP request is done */
			eipmbsModbusParseClassRequest(pRequest);
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}

		switch (pRequest->cfg.objectCfg.bService)
		{
		case MODBUS_SVC_READ_DISCRETE_INPUTS:
		case MODBUS_SVC_READ_COILS:
			eipmbsMbusReadBitsRsp(pRequest, pResponse);
			break;
		case MODBUS_SVC_READ_INPUT_REGISTERS:
		case MODBUS_SVC_READ_HOLDING_REGISTERS:
			eipmbsMbusReadRegistersResponse(pRequest, pResponse);
			break;
		case MODBUS_SVC_WRITE_COILS:
			eipmbsMbusWriteCoilsRsp(pRequest, pResponse);
			break;
		case MODBUS_SVC_WRITE_HOLDING_REGISTERS:
			eipmbsMbusWriteHoldRegRsp(pRequest, pResponse);
			break;
		case MODBUS_SVC_EXECUTE_MODBUS:
			eipmbsMbusExecuteModbusRsp(pRequest, pResponse);
			break;
		default:
			DumpStr3(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, pRequest->lIPAddress, pRequest->lHostIPAddr,
				"eipmbsTranslateResponse Internal Logic FAILURE - Unsupported request: Class = %d, Instance = %d, Service = %d", iClass, iInstance, pRequest->cfg.objectCfg.bService);
		}
		break;

	default:
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, pRequest->lIPAddress, pRequest->lHostIPAddr, 
			"eipmbsTranslateResponse Internal Logic FAILURE - Unsupported request: Class = %d", iClass);
	}

}


/*---------------------------------------------------------------------------
** eipmbsIdentityParseRequest( )
**
** Determine if a translatable message needs to be generated for a 
** request targetted for the "Modbus" Identity object
**---------------------------------------------------------------------------
*/

BOOL eipmbsIdentityParseRequest( REQUEST* pRequest, MBSRequest* pModbusRequest)
{
    UINT16 iTagSize;
    UINT32 iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
	UINT32 iAttribute;
	UINT8* pData;

	if (iTagSize == 0)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

	/* Class attributes don't need to be translated */
	if (iInstance == 0)
		return TRUE;

	/* Only instance 1 is supported */
	if (iInstance != 1)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		/* Attempt to get the Modbus product name to populate the GAA */
		pModbusRequest->bFunction = MBUS_READ_DEVICE_INFORMATION;
		pModbusRequest->iDataSize = 3;

		pData = pModbusRequest->requestData;

		*pData++ = 0x0E;
		*pData++ = 0x02;
		*pData++ = 0x04;

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	case SVC_GET_ATTR_SINGLE:
		iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
		if (iTagSize == 0)
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			return TRUE;
		}

		if (iAttribute == ID_ATTR_PRODUCT_NAME)
		{
			/* Attempt to get the Modbus product name to populate the GAS */
			pModbusRequest->bFunction = MBUS_READ_DEVICE_INFORMATION;
			pModbusRequest->iDataSize = 3;

			pData = pModbusRequest->requestData;

			*pData++ = 0x0E;
			*pData++ = 0x02;
			*pData++ = 0x04;

			pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
			if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
			{
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				return TRUE;
			}

			return FALSE;
		}
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** eipmbsIdParseClassRequest( )
**
** Respond to the "Modbus" Identity class request
**---------------------------------------------------------------------------
*/

void eipmbsIdParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		eipmbsIdClassGAARequest( pRequest );
		break;
	case SVC_GET_ATTR_SINGLE:
		eipmbsIdClassGASRequest( pRequest);
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		break;
	}
}

/*---------------------------------------------------------------------------
** eipmbsIdParseInstanceRequest( )
**
** Respond to the "Modbus" Identity instance request
**---------------------------------------------------------------------------
*/

void eipmbsIdParseInstanceRequest( REQUEST* pRequest, MBSResponse* pResponse)
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		eipmbsIdInstanceGAAResponse(pRequest, pResponse);
		break;
	case SVC_GET_ATTR_SINGLE:
		eipmbsIdInstanceGASResponse(pRequest, pResponse);
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		break;
	}
}

/*---------------------------------------------------------------------------
** eipmbsIdClassGAARequest( )
**
** GetAttributeAll service for "Modbus" Identity class is received 
**---------------------------------------------------------------------------
*/
void eipmbsIdClassGAARequest( REQUEST* pRequest )
{
	UINT8* pData;

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(UINT16)*5));

	pData = pRequest->cfg.objectCfg.pData;

	UINT16_SET(pData, ID_CLASS_REVISION);
	pData += sizeof(UINT16);
	UINT16_SET(pData, 1);
	pData += sizeof(UINT16);
	UINT16_SET(pData, 1);
	pData += sizeof(UINT16);
	UINT16_SET(pData, ID_CLASS_ATTR_MAX_CLASS_ATTR);
	pData += sizeof(UINT16);
	UINT16_SET(pData, ID_ATTR_PRODUCT_NAME);
	pData += sizeof(UINT16);
}

/*--------------------------------------------------------------------------------
** eipmbsIdClassGASRequest( )
**
** GetAttributeSingle request has been received for the "Modbus" Identity class
**--------------------------------------------------------------------------------
*/

void eipmbsIdClassGASRequest( REQUEST* pRequest )
{
	UINT16 iVal;
	UINT16 iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);

	switch( iAttribute )
	{
		case ID_CLASS_ATTR_REVISION:
			iVal = ENCAP_TO_HS(ID_CLASS_REVISION);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
			break;
		case ID_CLASS_ATTR_MAX_INSTANCE:
		case ID_CLASS_ATTR_NUM_INSTANCES:
			iVal = ENCAP_TO_HS(1);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
			break;
		case ID_CLASS_ATTR_MAX_CLASS_ATTR:
			iVal = ENCAP_TO_HS(ID_CLASS_ATTR_MAX_CLASS_ATTR);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
			break;
		case ID_CLASS_ATTR_MAX_INST_ATTR:
			iVal = ENCAP_TO_HS(ID_ATTR_PRODUCT_NAME);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );        
			break;
		default:
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			break;
	}
}

/*---------------------------------------------------------------------------
** eipmbsIdInstanceGAAResponse( )
**
** Modbus response to Identity instance GetAttributeAll is translated to CIP response
**---------------------------------------------------------------------------
*/
void eipmbsIdInstanceGAAResponse( REQUEST* pRequest, MBSResponse* pResponse )
{
	UINT8 *pData;
	BOOL bValidModbusResponse = TRUE;
	UINT16 iMessageSize;
	UINT8 iProductNameSize;

	if ((pResponse->bGeneralStatus != 0) ||
		(pResponse->responseData[0] != 0x0E) ||
		(pResponse->responseData[1] != 2) ||
		((pResponse->responseData[2] != 0) && (pResponse->responseData[2] != 0xFF)) ||
		(pResponse->responseData[3] != 4) ||
		(pResponse->responseData[4] != 1))
		bValidModbusResponse = FALSE;

	if (bValidModbusResponse)
	{
		iProductNameSize = (UINT8)(PLATFORM_MIN(32, pResponse->responseData[5]));
	}
	else
	{
		iProductNameSize = (UINT8)strlen(UNKNOWN_MODBUS_DEVICE);
	}

	iMessageSize = (UINT16)(sizeof(UINT8)+iProductNameSize+sizeof(UINT16)*4+sizeof(UINT8)*2+sizeof(UINT32)*1);

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iMessageSize);
    if ( pRequest->cfg.objectCfg.pData == NULL )
    {
        notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return;
	}

	pData = pRequest->cfg.objectCfg.pData;

	UINT16_SET(pData, MODBUS_VENDOR_ID);		/* Vendor */
	pData += sizeof(UINT16);

	UINT16_SET(pData, MODBUS_PRODUCT_TYPE);		/* Product Type */
	pData += sizeof(UINT16);

	UINT16_SET(pData, MODBUS_PRODUCT_CODE);		/* Product Code */
	pData += sizeof(UINT16);

	*pData++ = 0; /* Major Revision */
	*pData++ = 0; /* Minor Revision */

	UINT16_SET(pData, 0);		/* Status */
	pData += sizeof(UINT16);

	UINT32_SET(pData, 0);		/* Serial number */
	pData += sizeof(UINT32);

	*pData++ = iProductNameSize;

	if (bValidModbusResponse)
	{
		memcpy( pData, &pResponse->responseData[6], iProductNameSize );    
	}
	else
	{
		memcpy( pData, UNKNOWN_MODBUS_DEVICE, iProductNameSize );    
	}

	pData += iProductNameSize;
}

/*---------------------------------------------------------------------------
** eipmbsIdInstanceGASResponse( )
**
** Modbus response to Identity instance GetAttributeSingle is translated to CIP response
**---------------------------------------------------------------------------
*/
void eipmbsIdInstanceGASResponse( REQUEST* pRequest, MBSResponse* pResponse )
{
	UINT16 iTagSize, iVal;
	UINT8 iProductNameSize;
	UINT8 *pData;
	UINT32 lVal,
			iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);

	switch (iAttribute)
	{
	case ID_ATTR_VENDOR_ID:
		iVal = ENCAP_TO_HS(MODBUS_VENDOR_ID);            
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ID_ATTR_DEVICE_TYPE:
		iVal = ENCAP_TO_HS(MODBUS_PRODUCT_TYPE);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ID_ATTR_PRODUCT_CODE:
		iVal = ENCAP_TO_HS(MODBUS_PRODUCT_CODE);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ID_ATTR_REVISION:
		iVal = ENCAP_TO_HS(0);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ID_ATTR_STATUS:
		iVal = ENCAP_TO_HS(0);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ID_ATTR_SERIAL_NBR:
		lVal = ENCAP_TO_HL(0);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
		break;
	case ID_ATTR_PRODUCT_NAME:
		if ((pResponse->bGeneralStatus != 0) ||
			(pResponse->responseData[0] != 0x0E) ||
			(pResponse->responseData[1] != 2) ||
			((pResponse->responseData[3] != 0) && (pResponse->responseData[2] != 0xFF)) ||
			(pResponse->responseData[5] != 1) ||
			(pResponse->responseData[6] != 4))
		{
			iProductNameSize = (UINT8)strlen(UNKNOWN_MODBUS_DEVICE);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(iProductNameSize+1));
			if ( pRequest->cfg.objectCfg.pData == NULL )
			{
				notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				return;
			}

			pData = pRequest->cfg.objectCfg.pData;                        
			*pData++ = iProductNameSize;            
			memcpy( (char*)pData, UNKNOWN_MODBUS_DEVICE, iProductNameSize );            
		}
		else
		{
			iProductNameSize = (UINT8)(PLATFORM_MIN(32, pResponse->responseData[7]));	
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(iProductNameSize+1));

			if ( pRequest->cfg.objectCfg.pData == NULL )
			{
				notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				return;
			}
		
			pData = pRequest->cfg.objectCfg.pData;

			*pData++ = iProductNameSize;
			memcpy( pData, &pResponse->responseData[8], iProductNameSize );    
		}
		break;

	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		break;
	}
}








/*---------------------------------------------------------------------------
** eipmbsAssemblyParseRequest( )
**
** Respond to the "Modbus" Assembly object request
**---------------------------------------------------------------------------
*/

BOOL eipmbsAssemblyParseRequest( REQUEST* pRequest, MBSRequest* pModbusRequest)
{
    UINT16 iTagSize;
    UINT32 iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);

	if (iTagSize == 0)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

	/* Class attributes don't need to be translated */
	if (iInstance == 0)
		return TRUE;

	return eipmbsAssemblyParseInstRqst( pRequest, pModbusRequest, iInstance );
}

/*---------------------------------------------------------------------------
** eipmbsAssemblyParseClassRequest( )
**
** Respond to the "Modbus" Assembly class request
**---------------------------------------------------------------------------
*/

void eipmbsAssemblyParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_SINGLE:
		eipmbsAssemblyClassGASRequest( pRequest);
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		break;
	}
}

/*---------------------------------------------------------------------------
** eipmbsAssemblyParseInstRqst( )
**
** Respond to the "Modbus" Assembly instance request
**---------------------------------------------------------------------------
*/

BOOL eipmbsAssemblyParseInstRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
		case SVC_SET_MEMBER:
			return eipmbsAssemblyInstSetMemberRqst( pRequest, pModbusRequest, iInstance );

		case SVC_GET_MEMBER:
			return eipmbsAssemblyInstGetMemberRqst( pRequest, pModbusRequest, iInstance );

		default:
			break;
	}

	pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
	return TRUE;
}

/*--------------------------------------------------------------------------------
** eipmbsAssemblyClassGASRequest( )
**
** GetAttributeSingle request has been received for the "Modbus" Assembly object class
**--------------------------------------------------------------------------------
*/
void eipmbsAssemblyClassGASRequest( REQUEST* pRequest )
{
    UINT16        iVal;
    UINT16        iTagSize;
    UINT32        iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	/* Attribute is required */
    if ( iTagSize == 0 )
    {
        pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
        return; 
    }
        
    switch( iAttribute )
    {
    case ASSEMBLY_CLASS_ATTR_REVISION:            
        iVal = ENCAP_TO_HS(ASSEMBLY_CLASS_REVISION);
        EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );    
        break;
    default:
        pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
        break;
    }
}


/*--------------------------------------------------------------------------------
** eipmbsAssemblyInstSetMemberRqst( )
**
** SetMember request has been received for the "Modbus" Assembly object instance
**--------------------------------------------------------------------------------
*/
BOOL eipmbsAssemblyInstSetMemberRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance )
{
	UINT16	iTagSize, iSize, iTemp, i;
	UINT32	iMembers = 1;                /* Default is a single member */
	BOOL	bExtendedFormat = FALSE;
	UINT8	*pData, *pCIPData, *pModbusData;
	UINT32	iStartMember = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_MEMBER, 0, &iTagSize );

	if ( iTagSize == 0 )
	{
		/* Member is required */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;        
		return TRUE; 
	}

    if ( iStartMember & ASSEMBLY_EXTENDED_FORMAT_FLAG )
    {
		bExtendedFormat = TRUE;
	}
	else
	{
		iMembers = iStartMember;
	}

	/* Non-writable data */
	if ((iInstance >= INPUT_REGISTER_START) &&
		(iInstance <= INPUT_REGISTER_END))
	{
		/* Generate a read request to determine the correct error code */
		pModbusRequest->bFunction = MBUS_READ_INPUT_REGISTERS;
		pModbusRequest->iDataSize = 4;

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-INPUT_REGISTER_START);	/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, iMembers);	/* Number of registers */
		pModbusData += sizeof(UINT16);

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	if ((iInstance >= DISCRETE_INPUT_START) &&
		(iInstance <= DISCRETE_INPUT_END))
	{
		/* Generate a read request to determine the correct error code */
		pModbusRequest->bFunction = MBUS_READ_INPUT_STATUS;
		pModbusRequest->iDataSize = 4;

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-DISCRETE_INPUT_START);		/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, iMembers); /* Number of input bits */

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}


	if (!(((iInstance >= HOLDING_REGISTER_START) &&
		(iInstance <= HOLDING_REGISTER_END)) ||
		((iInstance >= COIL_START) &&
		(iInstance <= COIL_END))))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

	pData = pRequest->cfg.objectCfg.pData;
	iSize = pRequest->cfg.objectCfg.iDataSize;

	if ( bExtendedFormat )
	{
		if ( pRequest->cfg.objectCfg.iDataSize < 3 )
		{    
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;            
			return TRUE; 
		}

		/* The only extended format supported is Multiple Sequential Members one */
		if ( (*pData) != ASSEMBLY_MULTIPLE_MEMBER_FLAG )
		{    
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;            
			return TRUE;
		}

		pData++;        

		iMembers = UINT16_GET( pData );    
		pData += sizeof(UINT16);
		iSize -= 3;
	}

	if ((iInstance >= HOLDING_REGISTER_START) &&
		(iInstance <= HOLDING_REGISTER_END))
	{
		/* Holding registers are 16-bit values, so data must match up */
		if ((iMembers*2 > MAX_MODBUS_WRITE_SIZE) ||
			(iInstance+iMembers > HOLDING_REGISTER_END))
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
			return TRUE;
		}
		else if (iSize < iMembers*2)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			return TRUE;
		}
		else if (iSize > iMembers*2)
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			return TRUE;
		}

		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_WRITE_MULTIPLE_REGISTERS;
		pModbusRequest->iDataSize = (UINT16)(5+pRequest->cfg.objectCfg.iDataSize);

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-HOLDING_REGISTER_START);		/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, iSize/2); /* Number of registers */
		pModbusData += sizeof(UINT16);
		*pModbusData++ = (UINT8)iSize;		/* Number of bytes */

		for (i = 0, pCIPData = pData; 
			i < iSize/2; 
			i++, pCIPData += sizeof(UINT16), pModbusData += sizeof(UINT16))
		{
			iTemp = UINT16_GET(pCIPData);
			UINT16_BE_SET(pModbusData, iTemp);
		}

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	if ((iInstance >= COIL_START) &&
		(iInstance <= COIL_END))
	{
		if ((iSize > MAX_MODBUS_WRITE_SIZE) ||
			(iInstance+(iSize*8) > COIL_END))
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
			return TRUE;
		}
		else if (iSize < (iMembers+7)/8)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			return TRUE;
		}
		else if (iSize > (iMembers+7)/8)
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			return TRUE;
		}

		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_WRITE_MULTIPLE_COILS;
		pModbusRequest->iDataSize = (UINT16)(5+iSize);

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-COIL_START);		/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, iSize*8); /* Number of coils */
		pModbusData += sizeof(UINT16);
		*pModbusData++ = (UINT8)iSize;	/* Number of bytes */

		memcpy(pModbusData, pRequest->cfg.objectCfg.pData, iSize);

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	/* this shouldn't happen */
	pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
	return TRUE; 
}


/*---------------------------------------------------------------------------
** eipmbsAssemblyInstSetMemRsp( )
**
** Modbus response to Assembly instance SetMember is translated to CIP response
**---------------------------------------------------------------------------
*/
void eipmbsAssemblyInstSetMemRsp( REQUEST* pRequest, MBSResponse* pResponse )
{
	UINT16	iTagSize;
	UINT32	iInstance, iStartMember;    
	BOOL	bExtendedFormat = FALSE;
	UINT8	*pData;

	iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);

	/* If Modbus success, return CIP success */
	if (pResponse->bGeneralStatus == 0)
	{
		/* Trying to set a "readable" instance, reply accordingly */
		if (((iInstance >= INPUT_REGISTER_START) &&
			(iInstance <= INPUT_REGISTER_END)) ||
			((iInstance >= DISCRETE_INPUT_START) &&
			(iInstance <= DISCRETE_INPUT_END)) )
		{
			pRequest->bGeneralError = ROUTER_ERROR_MEMBER_NOT_SETTABLE;
		}
		else
		{
			pRequest->bGeneralError = 0;
		}
		
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	if (((iInstance >= INPUT_REGISTER_START) &&
		(iInstance <= INPUT_REGISTER_END)) ||
		((iInstance >= DISCRETE_INPUT_START) &&
		(iInstance <= DISCRETE_INPUT_END)) )
	{
		/* Modbus error generated for "read" request, must mean register doesn't exist */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
	}

	iStartMember = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_MEMBER, 0, &iTagSize );

	if ((iInstance >= HOLDING_REGISTER_START) ||
		(iInstance <= HOLDING_REGISTER_END))
	{
		if ((pRequest->nModbusContext != 0) ||
			(pRequest->cfg.objectCfg.iDataSize != 4))
		{
			eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
			return;
		}

		/* Special case exists if attempting to write a single register 
		 * In this case, try again with a write single register command */
		if ( iStartMember & ASSEMBLY_EXTENDED_FORMAT_FLAG )
			bExtendedFormat = TRUE;

		pData = pRequest->cfg.objectCfg.pData;

		if ( bExtendedFormat )
			pData += (1+sizeof(UINT16));

		pRequest->nModbusContext = pRequest->nModbusRequestID;
		iTagSize = UINT16_GET(pData);
		eipmbsWriteSingleRegister(pRequest, iInstance, iTagSize);
		return;
	}

	if ((iInstance >= COIL_START) ||
		(iInstance <= COIL_END))
	{
		if ((pRequest->nModbusContext != 0) ||
			(pRequest->cfg.objectCfg.iDataSize != 1))
		{
			eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
			return;
		}

		/* Special case exists if attempting to write a single coil
		 * In this case, try again with a write single coil command */
		if ( iStartMember & ASSEMBLY_EXTENDED_FORMAT_FLAG )
			bExtendedFormat = TRUE;

		pData = pRequest->cfg.objectCfg.pData;

		if ( bExtendedFormat )
			pData += (1+sizeof(UINT16));

		pRequest->nModbusContext = pRequest->nModbusRequestID;
		eipmbsWriteSingleCoil(pRequest, (UINT16)(iInstance-COIL_START+1), *pRequest->cfg.objectCfg.pData);
		return;
	}

	eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
}

/*--------------------------------------------------------------------------------
** eipmbsAssemblyInstGetMemberRqst( )
**
** GetMember request has been received for the "Modbus" Assembly object instance
**--------------------------------------------------------------------------------
*/
BOOL eipmbsAssemblyInstGetMemberRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance )
{
	UINT16      iTagSize;
	UINT32      iMembers = 1;                /* Default is a single member */
	BOOL        bExtendedFormat = FALSE;
	UINT32      iStartMember;
	UINT8		*pData, *pModbusData;

	/* Make sure it's a valid piece of Modbus data */
	if (!((iInstance >= HOLDING_REGISTER_START) &&
		 (iInstance <= DISCRETE_INPUT_END)))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

	iStartMember = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_MEMBER, 0, &iTagSize );

	if ( iTagSize == 0 )
	{
		/* Member is required */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

    if ( iStartMember & ASSEMBLY_EXTENDED_FORMAT_FLAG )
    {
		bExtendedFormat = TRUE;
	}
	else
	{
		iMembers = iStartMember;
	}

	if ( bExtendedFormat )
	{
		if ( pRequest->cfg.objectCfg.iDataSize < 1 )
		{    
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			return TRUE;
		}

		pData = pRequest->cfg.objectCfg.pData;

		/* The only extended format supported is Multiple Sequential Members one */
		if ( (*pData) != ASSEMBLY_MULTIPLE_MEMBER_FLAG )
		{    
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
			return TRUE;
		}

		pData++;        
		iMembers = UINT16_GET( pData );
	}

	if ((iInstance >= HOLDING_REGISTER_START) &&
		(iInstance <= HOLDING_REGISTER_END))
	{
		/* Holding registers are 16-bit values, so data must match up */
		if (iMembers*2 > MAX_MODBUS_READ_SIZE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			return TRUE;
		}

		if (iInstance+iMembers > HOLDING_REGISTER_END)
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_MEMBER;
			return TRUE;
		}

		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_READ_HOLDING_REGISTERS;
		pModbusRequest->iDataSize = (UINT16)(4+pRequest->cfg.objectCfg.iDataSize);

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-HOLDING_REGISTER_START);	/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, iMembers);	/* Number of registers */
		pModbusData += sizeof(UINT16);

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	if ((iInstance >= INPUT_REGISTER_START) &&
		(iInstance <= INPUT_REGISTER_END))
	{
		/* Holding registers are 16-bit values, so data must match up */
		if (iMembers*2 > MAX_MODBUS_READ_SIZE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			return TRUE;
		}

		if (iInstance+iMembers > INPUT_REGISTER_END)
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_MEMBER;
			return TRUE;
		}

		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_READ_INPUT_REGISTERS;
		pModbusRequest->iDataSize = (UINT16)(4+pRequest->cfg.objectCfg.iDataSize);

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-INPUT_REGISTER_START);	/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, iMembers);	/* Number of registers */
		pModbusData += sizeof(UINT16);

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	if ((iInstance >= DISCRETE_INPUT_START) &&
		(iInstance <= DISCRETE_INPUT_END))
	{
		if (iMembers/8 > MAX_MODBUS_READ_SIZE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			return TRUE;
		}

		if (iInstance+iMembers > DISCRETE_INPUT_END)
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_MEMBER;
			return TRUE;
		}

		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_READ_INPUT_STATUS;
		pModbusRequest->iDataSize = 4;

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-DISCRETE_INPUT_START);		/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, iMembers); /* Number of input bits */

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	if ((iInstance >= COIL_START) &&
		(iInstance <= COIL_END))
	{
		if (iMembers/8 > MAX_MODBUS_READ_SIZE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			return TRUE;
		}

		if (iInstance+iMembers > COIL_END)
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_MEMBER;
			return TRUE;
		}

		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_READ_COIL_STATUS;
		pModbusRequest->iDataSize = 4;

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-COIL_START);		/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, iMembers); /* Number of coils */

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	/* this shouldn't happen */
	pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
	return TRUE; 
}


/*---------------------------------------------------------------------------
** eipmbsAssemblyInstGetMemRsp( )
**
** Modbus response to Assembly instance GetMember is translated to CIP response
**---------------------------------------------------------------------------
*/
void eipmbsAssemblyInstGetMemRsp( REQUEST* pRequest, MBSResponse* pResponse )
{
	UINT16      iTagSize, iTemp;
	UINT32      iMembers = 1;                /* Default is a single member */
	BOOL        bExtendedFormat = FALSE;
	UINT32      iInstance, iStartMember;
	UINT8		*pData, *pModbusData;
	UINT8		i, iByteCount;

	/* If Modbus failure, return CIP failure */
	if (pResponse->bGeneralStatus != 0)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_MEMBER;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	/* recompute instance and member information */
	iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
	iStartMember = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_MEMBER, 0, &iTagSize );

	if ( iStartMember & ASSEMBLY_EXTENDED_FORMAT_FLAG )
		bExtendedFormat = TRUE;

	if ( bExtendedFormat )
	{
		iMembers = UINT16_GET(pRequest->cfg.objectCfg.pData);
	}
	else
	{
		iMembers = iStartMember;
	}

	pModbusData = pResponse->responseData;
	iByteCount = *pModbusData++;

	if (((iInstance >= HOLDING_REGISTER_START) &&
		 (iInstance <= HOLDING_REGISTER_END)) ||
		 ((iInstance >= INPUT_REGISTER_START) &&
		 (iInstance <= INPUT_REGISTER_END)))
	{
		/* byte count must equal number of members  * 2*/
		if ((iByteCount != pResponse->iDataSize-1) ||
			((UINT32)(iByteCount/2) != iMembers) ||
			(iByteCount%2 != 0))
		{
			pRequest->bGeneralError = ROUTER_ERROR_SERVICE_ERROR;
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iByteCount);

		for (i = 0, pData = pRequest->cfg.objectCfg.pData;
			i < iByteCount/2;
			i++, pData += sizeof(UINT16), pModbusData += sizeof(UINT16))
		{
			iTemp = UINT16_BE_GET(pModbusData);
			UINT16_SET(pData, iTemp);
		}
		
		/* Registers successfully transfered */
		pResponse->bGeneralStatus = 0;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	if (((iInstance >= COIL_START) &&
		 (iInstance <= COIL_END)) ||
		 ((iInstance >= DISCRETE_INPUT_START) &&
		 (iInstance <= DISCRETE_INPUT_END)))
	{
		/* byte count must equal number of members / 8 */
		if ((iByteCount != pResponse->iDataSize-1) ||
			(iByteCount != ((iMembers+7)/8)))
		{
			pRequest->bGeneralError = ROUTER_ERROR_SERVICE_ERROR;
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, pModbusData, iByteCount);

		/* Registers successfully transfered */
		pResponse->bGeneralStatus = 0;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}	

	/* this shouldn't happen */
	pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
	pRequest->nState = REQUEST_RESPONSE_RECEIVED;
}




/*---------------------------------------------------------------------------
** eipmbsParameterParseRequest( )
**
** Respond to the Parameter object request
**---------------------------------------------------------------------------
*/
BOOL eipmbsParameterParseRequest(REQUEST* pRequest, MBSRequest* pModbusRequest)
{
    UINT16 iTagSize;
    UINT32 iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);

	if (iTagSize == 0)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

	/* Class attributes don't need to be translated */
    if (iInstance == 0)
		return TRUE;

	return eipmbsParamParseInstanceRqst( pRequest, pModbusRequest, iInstance );
}

void eipmbsParamParseClassRequest( REQUEST* pRequest )
{
    UINT16 iVal, iTagSize;
	UINT32 iAttribute;

	/* Only GAS of Attribute 1 is supported */

	if (pRequest->cfg.objectCfg.bService != SVC_GET_ATTR_SINGLE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		return;
	}

    iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	/* Attribute is required */
    if ( iTagSize == 0 )
    {
        pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
        return; 
    }

	if (iAttribute != PARAMETER_CLASS_ATTR_REVISION)
	{
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
        return; 
	}

    iVal = ENCAP_TO_HS(PARAMETER_CLASS_REVISION);
    EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );    
}

BOOL eipmbsParamParseInstanceRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance)
{
	switch( pRequest->cfg.objectCfg.bService )
	{
		case SVC_SET_ATTR_SINGLE:
			return eipmbsParamInstSASRqst( pRequest, pModbusRequest, iInstance );

		case SVC_GET_ATTR_SINGLE:
			return eipmbsParamInstGASRqst( pRequest, pModbusRequest, iInstance );

		default:
			break;
	}

	pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
	return TRUE;
}

BOOL eipmbsParamInstSASRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance)
{
	UINT16  iTagSize, iSize, iTemp;
	UINT8	*pData, *pModbusData;
	UINT32	iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	/* Attribute is required */
    if ( iTagSize == 0 )
    {
        pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
        return TRUE; 
    }

	if (iAttribute != PARAMETER_ATTR_VALUE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
        return TRUE; 
	}

	if (!(((iInstance >= HOLDING_REGISTER_START) &&
		(iInstance <= HOLDING_REGISTER_END)) ||
		((iInstance >= COIL_START) &&
		(iInstance <= COIL_END))))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

	/* Non-writable data */
	if (((iInstance >= INPUT_REGISTER_START) &&
		(iInstance <= INPUT_REGISTER_END)) ||
		((iInstance >= DISCRETE_INPUT_START) &&
		(iInstance <= DISCRETE_INPUT_END)) )
	{
		/* Generate a read request to determine the correct error code */
		if ((iInstance >= INPUT_REGISTER_START) &&
			(iInstance <= INPUT_REGISTER_END))
		{
			pModbusRequest->bFunction = MBUS_READ_INPUT_REGISTERS;
			pModbusRequest->iDataSize = 4;

			pModbusData = pModbusRequest->requestData;
			UINT16_BE_SET(pModbusData, iInstance-INPUT_REGISTER_START);	/* Starting address */
			pModbusData += sizeof(UINT16);
			UINT16_BE_SET(pModbusData, 1);					/* Number of registers */
			pModbusData += sizeof(UINT16);

			pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
			if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
			{
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				return TRUE;
			}
		}
		else if ((iInstance >= DISCRETE_INPUT_START) &&
				(iInstance <= DISCRETE_INPUT_END))
		{
			pModbusRequest->bFunction = MBUS_READ_INPUT_STATUS;
			pModbusRequest->iDataSize = 4;

			pModbusData = pModbusRequest->requestData;
			UINT16_BE_SET(pModbusData, iInstance-DISCRETE_INPUT_START);	/* Starting address */
			pModbusData += sizeof(UINT16);
			UINT16_BE_SET(pModbusData, 1);								/* Number of input bits */

			pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
			if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
			{
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
				return TRUE;
			}
		}

		return FALSE;
	}

	pData = pRequest->cfg.objectCfg.pData;
	iSize = pRequest->cfg.objectCfg.iDataSize;

	if ((iInstance >= HOLDING_REGISTER_START) &&
		(iInstance <= HOLDING_REGISTER_END))
	{
		/* Holding registers are 16-bit values, so data must match up */
		if (iSize < 2)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			return TRUE;
		}
		else if (iSize > 2)
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			return TRUE;
		}

		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_WRITE_MULTIPLE_REGISTERS;
		pModbusRequest->iDataSize = (UINT16)(5+iSize);

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-HOLDING_REGISTER_START);		/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, 1);		/* Number of registers */
		pModbusData += sizeof(UINT16);
		*pModbusData++ = (UINT8)iSize;		/* Number of bytes */

		iTemp = UINT16_GET(pData);
		UINT16_BE_SET(pModbusData, iTemp);

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	if ((iInstance >= COIL_START) &&
		(iInstance <= COIL_END))
	{

		if (iSize < 1)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			return TRUE;
		}
		else if (iSize > 1)
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			return TRUE;
		}

		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_WRITE_MULTIPLE_COILS;
		pModbusRequest->iDataSize = (UINT16)(5+iSize);

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-COIL_START);		/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, 1); /* Number of coils */
		pModbusData += sizeof(UINT16);
		*pModbusData++ = (UINT8)iSize;	/* Number of bytes */

		memcpy(pModbusData, pRequest->cfg.objectCfg.pData, iSize);

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	/* this shouldn't happen */
	pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
	return TRUE; 
}

void eipmbsParamInstSASRsp( REQUEST* pRequest, MBSResponse* pResponse )
{
	UINT16	iTagSize;
	UINT32	iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);

	/* If Modbus success, return CIP success */
	if (pResponse->bGeneralStatus == 0)
	{
		/* Trying to set a "readable" instance, reply accordingly */
		if (((iInstance >= INPUT_REGISTER_START) &&
			(iInstance <= INPUT_REGISTER_END)) ||
			((iInstance >= DISCRETE_INPUT_START) &&
			(iInstance <= DISCRETE_INPUT_END)) )
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
		}
		else
		{
			pRequest->bGeneralError = 0;
		}

		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	if (((iInstance >= INPUT_REGISTER_START) &&
		(iInstance <= INPUT_REGISTER_END)) ||
		((iInstance >= DISCRETE_INPUT_START) &&
		(iInstance <= DISCRETE_INPUT_END)) )
	{
		/* Modbus error generated for "read" request, must mean register doesn't exist */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
	}

	if ((iInstance >= HOLDING_REGISTER_START) ||
		(iInstance <= HOLDING_REGISTER_END))
	{
		if ((pRequest->nModbusContext != 0) ||
			(pRequest->cfg.objectCfg.iDataSize != 2))
		{
			eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
			return;
		}

		/* Special case exists if attempting to write a single register 
		 * In this case, try again with a write single register command */
		pRequest->nModbusContext = pRequest->nModbusRequestID;
		iTagSize = UINT16_GET(pRequest->cfg.objectCfg.pData);
		eipmbsWriteSingleRegister(pRequest, iInstance, iTagSize);
		return;
	}

	if ((iInstance >= COIL_START) ||
		(iInstance <= COIL_END))
	{
		if ((pRequest->nModbusContext != 0) ||
			(pRequest->cfg.objectCfg.iDataSize != 1))
		{
			eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
			return;
		}

		/* Special case exists if attempting to write a single coil
		 * In this case, try again with a write single coil command */
		pRequest->nModbusContext = pRequest->nModbusRequestID;
		eipmbsWriteSingleCoil(pRequest, (UINT16)(iInstance-COIL_START+1), *pRequest->cfg.objectCfg.pData);
		return;
	}

	eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
}

BOOL eipmbsParamInstGASRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance)
{
	UINT16      iTagSize;
	UINT8		*pModbusData;
	UINT32	iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	/* Make sure it's a valid piece of Modbus data */
	if (!((iInstance >= HOLDING_REGISTER_START) &&
		 (iInstance <= DISCRETE_INPUT_END)))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

	/* Attribute is required */
    if ( iTagSize == 0 )
    {
        pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
        return TRUE; 
    }

	if (iAttribute != PARAMETER_ATTR_VALUE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
        return TRUE; 
	}

	if ((iInstance >= HOLDING_REGISTER_START) &&
		(iInstance <= HOLDING_REGISTER_END))
	{
		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_READ_HOLDING_REGISTERS;
		pModbusRequest->iDataSize = 4;

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-HOLDING_REGISTER_START);	/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, 1);	/* Number of registers */
		pModbusData += sizeof(UINT16);

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	if ((iInstance >= INPUT_REGISTER_START) &&
		(iInstance <= INPUT_REGISTER_END))
	{
		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_READ_INPUT_REGISTERS;
		pModbusRequest->iDataSize = 4;

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-INPUT_REGISTER_START);	/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, 1);	/* Number of registers */
		pModbusData += sizeof(UINT16);

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	if ((iInstance >= DISCRETE_INPUT_START) &&
		(iInstance <= DISCRETE_INPUT_END))
	{
		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_READ_INPUT_STATUS;
		pModbusRequest->iDataSize = 4;

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-DISCRETE_INPUT_START);		/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, 1);				/* Number of input bits */

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	if ((iInstance >= COIL_START) &&
		(iInstance <= COIL_END))
	{
		/* Data validated, convert to a Modbus message */
		pModbusRequest->bFunction = MBUS_READ_COIL_STATUS;
		pModbusRequest->iDataSize = 4;

		pModbusData = pModbusRequest->requestData;
		UINT16_BE_SET(pModbusData, iInstance-COIL_START);		/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, 1); /* Number of coils */

		pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
		if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return TRUE;
		}

		return FALSE;
	}

	/* this shouldn't happen */
	pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
	return TRUE; 
}

void eipmbsParamInstGASRsp( REQUEST* pRequest, MBSResponse* pResponse )
{
	UINT16      iTagSize, iTemp;
	UINT32      iInstance;
	UINT8		*pModbusData;
	UINT8		iByteCount;

	/* If Modbus failure, return CIP failure */
	if (pResponse->bGeneralStatus != 0)
	{
		eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	/* recompute instance information */
	iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);

	pModbusData = pResponse->responseData;
	iByteCount = *pModbusData++;

	if (((iInstance >= HOLDING_REGISTER_START) &&
		 (iInstance <= HOLDING_REGISTER_END)) ||
		 ((iInstance >= INPUT_REGISTER_START) &&
		 (iInstance <= INPUT_REGISTER_END)))
	{
		if ((iByteCount != pResponse->iDataSize-1) ||
			(iByteCount != 2))
		{
			pRequest->bGeneralError = ROUTER_ERROR_SERVICE_ERROR;
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iByteCount);

		iTemp = UINT16_BE_GET(pModbusData);
		UINT16_SET(pRequest->cfg.objectCfg.pData, iTemp);

		/* Registers successfully transfered */
		pResponse->bGeneralStatus = 0;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	if (((iInstance >= COIL_START) &&
		 (iInstance <= COIL_END)) ||
		 ((iInstance >= DISCRETE_INPUT_START) &&
		 (iInstance <= DISCRETE_INPUT_END)))
	{
		if ((iByteCount != pResponse->iDataSize-1) ||
			(iByteCount != 1))
		{
			pRequest->bGeneralError = ROUTER_ERROR_SERVICE_ERROR;
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, pModbusData, iByteCount);

		/* Registers successfully transfered */
		pResponse->bGeneralStatus = 0;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}	

	/* this shouldn't happen */
	pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
	pRequest->nState = REQUEST_RESPONSE_RECEIVED;
}


/*---------------------------------------------------------------------------
** eipmbsModbusParseRequest( )
**
** Respond to the Modbus object request
**---------------------------------------------------------------------------
*/

BOOL eipmbsModbusParseRequest( REQUEST* pRequest, MBSRequest* pModbusRequest)
{
    UINT16 iTagSize;
    UINT32 iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);

	if (iTagSize == 0)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

	/* Class attributes don't need to be translated */
    if (iInstance == 0)
		return TRUE;

	return eipmbsModbusParseInstanceRqst( pRequest, pModbusRequest, iInstance );
}

/*---------------------------------------------------------------------------
** eipmbsModbusParseClassRequest( )
**
** Respond to the Modbus class request
**---------------------------------------------------------------------------
*/

void eipmbsModbusParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		eipmbsModbusClassGAARequest( pRequest );
		break;
	case SVC_GET_ATTR_SINGLE:
		eipmbsModbusClassGASRequest( pRequest);
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		break;
	}
}

/*---------------------------------------------------------------------------
** eipmbsModbusParseInstanceRqst( )
**
** Respond to the Modbus instance request
**---------------------------------------------------------------------------
*/

BOOL eipmbsModbusParseInstanceRqst( REQUEST* pRequest, MBSRequest* pModbusRequest, UINT32 iInstance )
{
	if (iInstance != 1)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return TRUE;
	}

	switch( pRequest->cfg.objectCfg.bService )
	{
	case MODBUS_SVC_READ_DISCRETE_INPUTS:
		return eipmbsMbusReadBitsRqst(pRequest, pModbusRequest, MBUS_READ_INPUT_STATUS);
	case MODBUS_SVC_READ_COILS:
		return eipmbsMbusReadBitsRqst(pRequest, pModbusRequest, MBUS_READ_COIL_STATUS);
	case MODBUS_SVC_READ_INPUT_REGISTERS:
		return eipmbsMbusReadRegistersRequest(pRequest, pModbusRequest, MBUS_READ_INPUT_REGISTERS);
	case MODBUS_SVC_READ_HOLDING_REGISTERS:
		return eipmbsMbusReadRegistersRequest(pRequest, pModbusRequest, MBUS_READ_HOLDING_REGISTERS);
	case MODBUS_SVC_WRITE_COILS:
		return eipmbsMbusWriteCoilsRqst(pRequest, pModbusRequest);
	case MODBUS_SVC_WRITE_HOLDING_REGISTERS:
		return eipmbsMbusWriteHoldRegRqst(pRequest, pModbusRequest);
	case MODBUS_SVC_EXECUTE_MODBUS:
		return eipmbsMbusExecuteModbusRqst(pRequest, pModbusRequest);
	}

	pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
	return TRUE;
}

/*--------------------------------------------------------------------------------
** eipmbsModbusClassGAARequest( )
**
** GetAttributeAll request has been received for the Modbus object class
**--------------------------------------------------------------------------------
*/
void eipmbsModbusClassGAARequest( REQUEST* pRequest )
{
	MODBUS_CLASS_ATTR attr;

	attr.iRevision = ENCAP_TO_HS(MODBUS_CLASS_REVISION);
	attr.iNumInstances = ENCAP_TO_HS(1);
	attr.iMaxInstance = ENCAP_TO_HS(1);
	attr.iMaxClassAttr = ENCAP_TO_HS(MODBUS_CLASS_ATTR_MAX_INST_ATTR);
	attr.iMaxInstanceAttr = ENCAP_TO_HS(0);
			
	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&attr, ASSEMBLY_CLASS_ATTR_SIZE );
}

/*--------------------------------------------------------------------------------
** eipmbsModbusClassGASRequest( )
**
** GetAttributeSingle request has been received for the Modbus object class
**--------------------------------------------------------------------------------
*/
void eipmbsModbusClassGASRequest( REQUEST* pRequest )
{
    UINT16        iVal;
    UINT16        iTagSize;
    UINT32        iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	/* Attribute is required */
    if ( iTagSize == 0 )
    {
        pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
        return; 
    }
        
    switch( iAttribute )
    {
        case MODBUS_CLASS_ATTR_REVISION:            
            iVal = ENCAP_TO_HS(MODBUS_CLASS_REVISION);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );    
            break;
        case MODBUS_CLASS_ATTR_MAX_INSTANCE:
		case MODBUS_CLASS_ATTR_NUM_INSTANCES:
            iVal = ENCAP_TO_HS(1);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );    
			break;
		case MODBUS_CLASS_ATTR_MAX_CLASS_ATTR:
            iVal = ENCAP_TO_HS(MODBUS_CLASS_ATTR_MAX_INST_ATTR);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );    
            break;
		case MODBUS_CLASS_ATTR_MAX_INST_ATTR:
            iVal = ENCAP_TO_HS(0);
            EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );    
            break;
        default:
            pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
            break;
    }
}

/*--------------------------------------------------------------------------------
** eipmbsMbusReadBitsRqst( )
**
** Read "bits" service (Coils or Discrete Inputs) request for Modbus object
**--------------------------------------------------------------------------------
*/
BOOL eipmbsMbusReadBitsRqst(REQUEST* pRequest, MBSRequest* pModbusRequest, UINT8 bFunction)
{
	UINT8* pData = pRequest->cfg.objectCfg.pData;
	UINT16 iStartingAddress, iNumberOfBits;

	if (pRequest->cfg.objectCfg.iDataSize < 4)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return TRUE;
	}

	if (pRequest->cfg.objectCfg.iDataSize > 4)
	{
		pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
		return TRUE;
	}

	iStartingAddress = UINT16_GET(pData);
	pData += sizeof(UINT16);
	iNumberOfBits = UINT16_GET(pData);
	
	if ((iNumberOfBits/8 > MAX_MODBUS_READ_SIZE) ||
		(iStartingAddress+iNumberOfBits > 0x10000))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		return TRUE;
	}

	/* Data validated, convert to a Modbus message */
	pModbusRequest->bFunction = bFunction;
	pModbusRequest->iDataSize = 4;

	pData = pModbusRequest->requestData;
	UINT16_BE_SET(pData, iStartingAddress);		/* Starting address */
	pData += sizeof(UINT16);
	UINT16_BE_SET(pData, iNumberOfBits); /* Number of coils/inputs */

	pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
	if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return TRUE;
	}

	return FALSE;
}

/*---------------------------------------------------------------------------
** eipmbsMbusReadBitsRsp( )
**
** Modbus response to Read "bits" service (Coils or Discrete Inputs) is 
** translated to CIP response
**---------------------------------------------------------------------------
*/
void eipmbsMbusReadBitsRsp(REQUEST* pRequest, MBSResponse* pResponse)
{
	UINT8* pData = pRequest->cfg.objectCfg.pData;
	UINT16 iStartingAddress, iNumberOfBits;
	UINT8 iByteCount;

	/* If Modbus failure, return CIP failure */
	if (pResponse->bGeneralStatus != 0)
	{
		eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
		return;
	}

	/* recompute Modbus information */
	iStartingAddress = UINT16_GET(pData);
	pData += sizeof(UINT16);
	iNumberOfBits = UINT16_GET(pData);

	pData = pResponse->responseData;
	iByteCount = *pData++;

	/* byte count must equal number of inputs / 8 */
	if ((iByteCount != pResponse->iDataSize-1) ||
		(iByteCount != ((iNumberOfBits+7)/8)))
	{
		pRequest->bGeneralError = ROUTER_ERROR_SERVICE_ERROR;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	/* ensure any excess bits are set to 0 */
	if (iNumberOfBits%8 != 0)
		(*(pData+iByteCount-1)) &= ((UINT8)((1 << (iNumberOfBits%8))-1));

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, pData, iByteCount);

	/* Registers successfully transfered */
	pResponse->bGeneralStatus = 0;
	pRequest->nState = REQUEST_RESPONSE_RECEIVED;
}

/*--------------------------------------------------------------------------------
** eipmbsMbusReadBitsRqst( )
**
** Read "registers" service (Holding or Inputs) request for Modbus object
**--------------------------------------------------------------------------------
*/
BOOL eipmbsMbusReadRegistersRequest(REQUEST* pRequest, MBSRequest* pModbusRequest, UINT8 bFunction)
{
	UINT8* pData = pRequest->cfg.objectCfg.pData;
	UINT16 iStartingAddress, iNumberOfRegisters;

	if (pRequest->cfg.objectCfg.iDataSize < 4)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return TRUE;
	}

	if (pRequest->cfg.objectCfg.iDataSize > 4)
	{
		pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
		return TRUE;
	}

	iStartingAddress = UINT16_GET(pData);
	pData += sizeof(UINT16);
	iNumberOfRegisters = UINT16_GET(pData);
	
	if ((iNumberOfRegisters*2 > MAX_MODBUS_READ_SIZE) ||
		(iStartingAddress+iNumberOfRegisters > 0x10000))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		return TRUE;
	}

	/* Data validated, convert to a Modbus message */
	pModbusRequest->bFunction = bFunction;
	pModbusRequest->iDataSize = 4;

	pData = pModbusRequest->requestData;
	UINT16_BE_SET(pData, iStartingAddress);		/* Starting address */
	pData += sizeof(UINT16);
	UINT16_BE_SET(pData, iNumberOfRegisters); /* Number of registers */

	pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
	if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return TRUE;
	}

	return FALSE;

}

/*---------------------------------------------------------------------------
** eipmbsMbusReadRegistersResponse( )
**
** Modbus response to Read "register" service (Holding or Inputs) is 
** translated to CIP response
**---------------------------------------------------------------------------
*/
void eipmbsMbusReadRegistersResponse(REQUEST* pRequest, MBSResponse* pResponse)
{
	UINT8 *pData = pRequest->cfg.objectCfg.pData,
		  *pModbusData;
	UINT16 iStartingAddress, iNumberOfRegisters, iTemp;
	UINT8 i, iByteCount;

	/* If Modbus failure, return CIP failure */
	if (pResponse->bGeneralStatus != 0)
	{
		eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
		return;
	}

	/* recompute Modbus information */
	iStartingAddress = UINT16_GET(pData);
	pData += sizeof(UINT16);
	iNumberOfRegisters = UINT16_GET(pData);

	pData = pResponse->responseData;
	iByteCount = *pData++;

	/* byte count must equal number of inputs / 8 */
	if ((iByteCount != pResponse->iDataSize-1) ||
		(iByteCount != (iNumberOfRegisters*2)))
	{
		pRequest->bGeneralError = ROUTER_ERROR_SERVICE_ERROR;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iByteCount);

	/* transfer the data */
	for (i = 0, pModbusData = pData, pData = pRequest->cfg.objectCfg.pData; 
		i < iByteCount/2; 
		i++, pData += sizeof(UINT16), pModbusData += sizeof(UINT16))
	{
		iTemp = UINT16_BE_GET(pModbusData);
		UINT16_SET(pData, iTemp);
	}

	/* Registers successfully transfered */
	pResponse->bGeneralStatus = 0;
	pRequest->nState = REQUEST_RESPONSE_RECEIVED;
}

/*--------------------------------------------------------------------------------
** eipmbsMbusWriteCoilsRqst( )
**
** Write coils service request for Modbus object
**--------------------------------------------------------------------------------
*/
BOOL eipmbsMbusWriteCoilsRqst(REQUEST* pRequest, MBSRequest* pModbusRequest)
{
	UINT8 *pData = pRequest->cfg.objectCfg.pData,
		  *pModbusData;
	UINT16 iStartingAddress, iNumberOfCoils;
	UINT8 iByteCount;

	if (pRequest->cfg.objectCfg.iDataSize < 4)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return TRUE;
	}

	iStartingAddress = UINT16_GET(pData);
	pData += sizeof(UINT16);
	iNumberOfCoils = UINT16_GET(pData);
	pData += sizeof(UINT16);

	if (pRequest->cfg.objectCfg.iDataSize < 4+((iNumberOfCoils+7)/8))
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return TRUE;
	}

	if (pRequest->cfg.objectCfg.iDataSize > 4+((iNumberOfCoils+7)/8))
	{
		pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
		return TRUE;
	}

	if ((iNumberOfCoils/8 > MAX_MODBUS_WRITE_SIZE) ||
		(iStartingAddress+iNumberOfCoils > 0x10000))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		return TRUE;
	}

	/* Data validated, convert to a Modbus message */
	pModbusRequest->bFunction = MBUS_WRITE_MULTIPLE_COILS;
	pModbusRequest->iDataSize = (UINT16)(pRequest->cfg.objectCfg.iDataSize+1);
	iByteCount = (UINT8)(pRequest->cfg.objectCfg.iDataSize-4);

	pModbusData = pModbusRequest->requestData;
	UINT16_BE_SET(pModbusData, iStartingAddress);		/* Starting address */
	pModbusData += sizeof(UINT16);
	UINT16_BE_SET(pModbusData, iNumberOfCoils); /* Number of coils */
	pModbusData += sizeof(UINT16);
	*pModbusData++ = iByteCount;	/* Number of bytes */

	/* ensure any excess bits are set to 0 */
	if (iNumberOfCoils%8 != 0)
		(*(pData+iByteCount-1)) &= ((UINT8)((1 << (iNumberOfCoils%8))-1));

	memcpy(pModbusData, pData, iByteCount);

	pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
	if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return TRUE;
	}

	return FALSE;
}

/*---------------------------------------------------------------------------
** eipmbsMbusWriteCoilsRsp( )
**
** Modbus response to Write Coils service is translated to CIP response
**---------------------------------------------------------------------------
*/
void eipmbsMbusWriteCoilsRsp(REQUEST* pRequest, MBSResponse* pResponse)
{
	UINT16 iStartingAddress, iNumberOfCoils;
	UINT8	*pData = pRequest->cfg.objectCfg.pData;

	/* recompute Modbus information */
	iStartingAddress = UINT16_GET(pData);
	pData += sizeof(UINT16);
	iNumberOfCoils = UINT16_GET(pData);
	pData += sizeof(UINT16);

	/* If Modbus success, return CIP success */
	if (pResponse->bGeneralStatus == 0)
	{
		pRequest->bGeneralError = 0;

		/* reflect the starting address and number of coils written */

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16)*2);

		pData = pRequest->cfg.objectCfg.pData;
		UINT16_SET(pData, iStartingAddress);
		pData += sizeof(UINT16);
		UINT16_SET(pData, iNumberOfCoils);

		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	/* If Modbus failure, return CIP failure */
	if ((pRequest->nModbusContext != 0) ||
		(iNumberOfCoils != 1))
	{
		eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
		return;
	}

	/* Special case exists if attempting to write a single coil
	 * In this case, try again with a write single coil command */
	pRequest->nModbusContext = pRequest->nModbusRequestID;
	eipmbsWriteSingleCoil(pRequest, iStartingAddress, *pData);
}

/*--------------------------------------------------------------------------------
** eipmbsMbusWriteHoldRegRqst( )
**
** Write holding register service request for Modbus object
**--------------------------------------------------------------------------------
*/
BOOL eipmbsMbusWriteHoldRegRqst(REQUEST* pRequest, MBSRequest* pModbusRequest)
{
	UINT8 *pData = pRequest->cfg.objectCfg.pData,
		  *pModbusData;
	UINT16 i, iStartingAddress, iNumberOfRegisters, iTemp;

	if (pRequest->cfg.objectCfg.iDataSize < 4)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return TRUE;
	}

	iStartingAddress = UINT16_GET(pData);
	pData += sizeof(UINT16);
	iNumberOfRegisters = UINT16_GET(pData);
	pData += sizeof(UINT16);

	if (pRequest->cfg.objectCfg.iDataSize < 4+iNumberOfRegisters*2)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return TRUE;
	}

	if (pRequest->cfg.objectCfg.iDataSize > 4+iNumberOfRegisters*2)
	{
		pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
		return TRUE;
	}

	if ((iNumberOfRegisters*2 > MAX_MODBUS_WRITE_SIZE) ||
		(iStartingAddress+iNumberOfRegisters > 0x10000))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		return TRUE;
	}

	/* Data validated, convert to a Modbus message */
	pModbusRequest->bFunction = MBUS_WRITE_MULTIPLE_REGISTERS;
	pModbusRequest->iDataSize = (UINT16)(5+pRequest->cfg.objectCfg.iDataSize);

	pModbusData = pModbusRequest->requestData;
	UINT16_BE_SET(pModbusData, iStartingAddress);		/* Starting address */
	pModbusData += sizeof(UINT16);
	UINT16_BE_SET(pModbusData, iNumberOfRegisters); /* Number of registers */
	pModbusData += sizeof(UINT16);
	*pModbusData++ = (UINT8)(iNumberOfRegisters*2);		/* Number of bytes */

	for (i = 0; 
		i < iNumberOfRegisters; 
		i++, pData += sizeof(UINT16), pModbusData += sizeof(UINT16))
	{
		iTemp = UINT16_GET(pData);
		UINT16_BE_SET(pModbusData, iTemp);
	}

	pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
	if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return TRUE;
	}

	return FALSE;
}

/*---------------------------------------------------------------------------
** eipmbsMbusWriteHoldRegRsp( )
**
** Modbus response to Write Register service is translated to CIP response
**---------------------------------------------------------------------------
*/
void eipmbsMbusWriteHoldRegRsp(REQUEST* pRequest, MBSResponse* pResponse)
{
	UINT16 iStartingAddress, iNumberOfRegisters, iTemp;
	UINT8	*pData = pRequest->cfg.objectCfg.pData;

	/* recompute Modbus information */
	iStartingAddress = UINT16_GET(pData);
	pData += sizeof(UINT16);
	iNumberOfRegisters = UINT16_GET(pData);
	pData += sizeof(UINT16);

	/* If Modbus success, return CIP success */
	if (pResponse->bGeneralStatus == 0)
	{
		pRequest->bGeneralError = 0;

		/* reflect the starting address and number of registers written */

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16)*2);

		pData = pRequest->cfg.objectCfg.pData;
		UINT16_SET(pData, iStartingAddress);
		pData += sizeof(UINT16);
		UINT16_SET(pData, iNumberOfRegisters);

		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	/* If Modbus failure, return CIP failure */
	if ((pRequest->nModbusContext != 0) ||
		(iNumberOfRegisters != 1))
	{
		eipmbsHandleExpMessageError(pRequest, pResponse->bGeneralStatus);
		return;
	}

	/* Special case exists if attempting to write a single register
	 * In this case, try again with a write single register command */
	pRequest->nModbusContext = pRequest->nModbusRequestID;
	iTemp = UINT16_GET(pData);
	eipmbsWriteSingleRegister(pRequest, iStartingAddress, iTemp);
}

/*--------------------------------------------------------------------------------
** eipmbsMbusExecuteModbusRqst( )
**
** "Execute Modbus" (generic Modbus request) service request for Modbus object
**--------------------------------------------------------------------------------
*/
BOOL eipmbsMbusExecuteModbusRqst(REQUEST* pRequest, MBSRequest* pModbusRequest)
{
	UINT8* pData = pRequest->cfg.objectCfg.pData;
	UINT8 bFunction;

	if (pRequest->cfg.objectCfg.iDataSize < 1)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return TRUE;
	}

	if (pRequest->cfg.objectCfg.iDataSize > 252)
	{
		pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
		return TRUE;
	}

	bFunction = *pData++;
	if ((bFunction < 1) || (bFunction > 127))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		return TRUE;
	}

	/* Data validated, convert to a Modbus message */
	pModbusRequest->bFunction = bFunction;
	pModbusRequest->iDataSize = (UINT16)(pRequest->cfg.objectCfg.iDataSize-1);

	memcpy(pModbusRequest->requestData, pData, pModbusRequest->iDataSize);

	pRequest->nModbusRequestID = modbusSendRequest(pModbusRequest, FALSE);
	if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return TRUE;
	}

	return FALSE;
}

/*---------------------------------------------------------------------------
** eipmbsMbusExecuteModbusRsp( )
**
** Modbus response to "Execute Modbus" (generic Modbus request) service
**---------------------------------------------------------------------------
*/
void eipmbsMbusExecuteModbusRsp(REQUEST* pRequest, MBSResponse* pResponse)
{
	UINT8	bFunction = *pRequest->cfg.objectCfg.pData;

	if (pResponse->iDataSize > 252)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	/* If Modbus success, return CIP success */
	if (pResponse->bGeneralStatus == 0)
	{
		pRequest->bGeneralError = 0;

		/* Copy the response data */
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(pResponse->iDataSize+1));

		pRequest->cfg.objectCfg.pData[0] = bFunction;
		memcpy(&pRequest->cfg.objectCfg.pData[1], pResponse->responseData, pResponse->iDataSize);
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
	}
	else
	{
		/* If Modbus failure, return CIP failure */
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, 2);
		pRequest->cfg.objectCfg.pData[0] = (UINT8)(bFunction | MBUS_EXCEPTION_RESPONSE_MASK);
		pRequest->cfg.objectCfg.pData[1] = pResponse->bGeneralStatus;
	}

	pRequest->bGeneralError = 0;
	pRequest->nState = REQUEST_RESPONSE_RECEIVED;
}


/*--------------------------------------------------------------------------------
** eipmbsWriteSingleRegister( )
**
** Write a single Holding register
**--------------------------------------------------------------------------------
*/
void eipmbsWriteSingleRegister(REQUEST* pRequest, UINT32 iRegister, UINT16 iRegisterValue)
{
	UINT8	*pData;
	MBSRequest Request;

	/* Data validated, convert to a Modbus message */
	eipmbsGenerateAddressData(pRequest, &Request);
	Request.lMessageTimeout = eipmbsGenerateTimeoutValue(pRequest);

	Request.bFunction = MBUS_WRITE_SINGLE_REGISTER;
	Request.iDataSize = 4;

	pData = Request.requestData;
	UINT16_BE_SET(pData, iRegister);		/* Starting address */
	pData += sizeof(UINT16);
	UINT16_BE_SET(pData, iRegisterValue); /* Value of register*/

	pRequest->nModbusRequestID = modbusSendRequest(&Request, FALSE);
	if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
	}
}

/*--------------------------------------------------------------------------------
** eipmbsWriteSingleRegister( )
**
** Write a single Coil
**--------------------------------------------------------------------------------
*/
void eipmbsWriteSingleCoil(REQUEST* pRequest, UINT16 iCoil, UINT8 iValue)
{
	UINT8	*pData;
	UINT16 iCoilValue;
	MBSRequest Request;

	/* Data validated, convert to a Modbus message */
	eipmbsGenerateAddressData(pRequest, &Request);
	Request.lMessageTimeout = eipmbsGenerateTimeoutValue(pRequest);

	Request.bFunction = MBUS_WRITE_SINGLE_COIL;
	Request.iDataSize = 4;

	iCoilValue = (UINT16)(iValue ? 0xFF : 0);

	pData = Request.requestData;
	UINT16_BE_SET(pData, iCoil);		/* Starting address */
	pData += sizeof(UINT16);
	UINT16_BE_SET(pData, iCoilValue); /* Value of register*/

	pRequest->nModbusRequestID = modbusSendRequest(&Request, FALSE);
	if (pRequest->nModbusRequestID < MBUS_REQUEST_INDEX_BASE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
	}
}

void eipmbsHandleExpMessageError(REQUEST* pRequest, UINT8 bModbusError)
{
	switch(bModbusError)
	{
	case MBUS_EXCEPTION_ILLEGAL_FUNCTION:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		break;
	case MBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS:
		pRequest->bGeneralError = ROUTER_ERROR_OBJ_DOES_NOT_EXIST;
		break;
	case MBUS_EXCEPTION_ILLEGAL_DATA_VALUE:
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		break;
	case MBUS_EXCEPTION_SLAVE_DEVICE_FAILURE:
		pRequest->bGeneralError = ROUTER_ERROR_DEV_IN_WRONG_STATE;
		break;
	case MBUS_EXCEPTION_SLAVE_DEVICE_BUSY:
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_UNKNOWN_MODBUS_ERROR;
		pRequest->iExtendedError = bModbusError;
	}

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
	pRequest->nState = REQUEST_RESPONSE_RECEIVED;
}

/*--------------------------------------------------------------------------------
** eipmbsGenerateAddressData( )
**
** Setup addressing for Modbus TCP or Serial
**--------------------------------------------------------------------------------
*/
void eipmbsGenerateAddressData(REQUEST* pRequest, MBSRequest* pMBSRequest)
{
#ifdef ET_IP_MODBUS_SERIAL
	if (pRequest->bModbusSerialAddress != 0)
	{
		pMBSRequest->bUnitID = pRequest->bModbusSerialAddress;
		pMBSRequest->bModbusMsgType = MBUS_REQUESTTYPE_SERIAL;
		pMBSRequest->lHostAddress = pRequest->bModbusSerialAddress;
	}
	else
	{
#endif
		strcpy(pMBSRequest->networkPath, socketGetAddressString(pRequest->lIPAddress));
		pMBSRequest->bModbusMsgType = MBUS_REQUESTTYPE_TCP;
		pMBSRequest->lHostAddress = pRequest->lHostIPAddr;
		pMBSRequest->bUnitID = (UINT8)((pRequest->iExtendedPathSize > 0) ? pRequest->pExtendedPath[1] : MBUS_DEFAULT_UNIT_ID);

#ifdef ET_IP_MODBUS_SERIAL
	}
#endif

}

/*--------------------------------------------------------------------------------
** eipmbsGenerateTimeoutValue( )
**
** Determine timeout value for a Modbus message
**--------------------------------------------------------------------------------
*/
UINT32 eipmbsGenerateTimeoutValue(REQUEST* pRequest)
{
	UINT32 iRet = 0;

	if (IS_TICK_GREATER(pRequest->lRequestTimeoutTick, gdwTickCount))
	{
		iRet = pRequest->lRequestTimeoutTick - gdwTickCount;
	}
	else
	{
		iRet = gdwTickCount - pRequest->lRequestTimeoutTick;
	}

	/* take off 100ms so the Modbus request will timeout before
	   the EtherNet/IP request does */
	if (iRet > 100)
		iRet -= 100;

	return iRet;
}

#endif /* #ifdef ET_IP_MODBUS */

