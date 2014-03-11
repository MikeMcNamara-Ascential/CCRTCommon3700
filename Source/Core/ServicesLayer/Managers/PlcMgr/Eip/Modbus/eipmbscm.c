/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPMBSCM.C
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Connection Manager proxy for Modbus devices. 
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
void eipmbsconnmgrParseClassRequest( REQUEST* pRequest );
BOOL eipmbsconnmgrParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance );
void eipmbsconnmgrSendClassAttrAll( REQUEST* pRequest );
void eipmbsconnmgrSendClassAttrSingle( REQUEST* pRequest);
void eipmbsioSendIOPacket(CONNECTION* pConnection, UINT8* pModbusData, UINT16 iDataSize );
void eipmbsconnmgrWriteSingleReg(CONNECTION* pConnection, UINT32 iRegister, UINT16 iRegisterValue);
void eipmbsParseConnectionPath( CONNECTION_CFG* pConfig );
BOOL eipmbsconnmgrNULLConnection(SESSION* pSession, REQUEST* pRequest, CONNECTION* pConnection, FWD_OPEN* pFwdOpen);
BOOL eipmbsconnmgrSendConfigData(REQUEST* pRequest, CONNECTION* pConnection);
BOOL eipmbsconnmgrVerifyConnection(REQUEST* pRequest, CONNECTION* pConnection, FWD_OPEN* pFwdOpen);
void PopulateModbusAddrFromConn(MBSRequest* pRequest, CONNECTION* pConnection);


/*---------------------------------------------------------------------------
** eipmbsconnmgrParseRequest( )
**
** Respond to the "Modbus" Connection Manager object request
**---------------------------------------------------------------------------
*/
void eipmbsconnmgrParseRequest( REQUEST* pRequest )
{
	UINT32  iInstance;
	UINT16  iTagSize;

	iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
	if ((iTagSize == 0) || ( iInstance != 0 ))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	eipmbsconnmgrParseClassRequest( pRequest );
}

/*---------------------------------------------------------------------------
** eipmbsconnmgrParseClassRequest( )
**
** Respond to the "Modbus" Connection Manager class request
**---------------------------------------------------------------------------
*/

void eipmbsconnmgrParseClassRequest( REQUEST* pRequest )
{
    switch( pRequest->cfg.objectCfg.bService )
    {
        case SVC_GET_ATTR_ALL:
            eipmbsconnmgrSendClassAttrAll( pRequest );
            break;
        case SVC_GET_ATTR_SINGLE:
            eipmbsconnmgrSendClassAttrSingle( pRequest);
            break;
        default:
            pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
            break;
    }
}

/*---------------------------------------------------------------------------
** eipmbsconnmgrSendClassAttrAll( )
**
** GetAttributeAll service for "Modbus" Connection Manager class is received 
**---------------------------------------------------------------------------
*/

void eipmbsconnmgrSendClassAttrAll( REQUEST* pRequest )
{
	UINT8* pData;

	EtIP_realloc(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(5*sizeof(UINT16)));

	pData = pRequest->cfg.objectCfg.pData;

	/* class revision */
	UINT16_SET(pData, CONNMGR_CLASS_REVISION);
	pData += sizeof(UINT16);
	/* Max instance */
	UINT16_SET(pData, 1);
	pData += sizeof(UINT16);
	/* Number of Instances */
	UINT16_SET(pData, 1);
	pData += sizeof(UINT16);
	/* Max class attribute */
	UINT16_SET(pData, CONNMGR_CLASS_ATTR_MAX_INST_ATTR);
	pData += sizeof(UINT16);
	/* Max instance attribute */
	UINT16_SET(pData, 0);
}

/*--------------------------------------------------------------------------------
** eipmbsconnmgrSendClassAttrSingle( )
**
** GetAttributeSingle request has been received for the "Modbus" Connection Manager class
**--------------------------------------------------------------------------------
*/
void eipmbsconnmgrSendClassAttrSingle( REQUEST* pRequest)
{
    UINT16 iVal, iTagSize;
    UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);

	switch (iAttribute)
	{
	case CONNMGR_CLASS_ATTR_REVISION:
        iVal = ENCAP_TO_HS(CONNMGR_CLASS_REVISION);
        EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_CLASS_ATTR_MAX_INSTANCE:
	case CONNMGR_CLASS_ATTR_NUM_INSTANCES:
        iVal = ENCAP_TO_HS(1);
        EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_CLASS_ATTR_MAX_CLASS_ATTR:
        iVal = ENCAP_TO_HS(CONNMGR_CLASS_ATTR_MAX_INST_ATTR);
        EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_CLASS_ATTR_MAX_INST_ATTR:
        iVal = ENCAP_TO_HS(0);
        EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
    default:
        pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
        break;
	}
}

/*---------------------------------------------------------------------------
** eipmbsParseCIPModbusFORequest( )
**
** Incoming connection to Modbus request received - store addresses and parse
** Forward Open
**---------------------------------------------------------------------------
*/
BOOL eipmbsParseCIPModbusFORequest(SESSION* pSession, REQUEST* pRequest, 
									CONNECTION* pConnection, FWD_OPEN* pFwdOpen, BOOL bLargeFwdOpen)
{
    CONNECTION  *pLoopConnection, *pMulticastProducer, 
				*pOTConnection = NULL, *pTOConnection = NULL;
	UINT16      iTagSize, iClassTagSize, iInstanceTagSize,
				iModuleConfigSize, usPort;
	UINT32      iClass, iInstance;
	UINT8		*pModuleConfigData;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;

	/* No redundant owner or variable sized class1 supported */
	if ((pConnection->cfg.bRedundantOwner) ||
		((connectionIsIOConnection(pConnection)) &&
		 ((pConnection->cfg.bClass1VariableConsumer) ||
		  (pConnection->cfg.bClass1VariableConsumer))))
	{
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONFIG_FORMAT);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	/* Support only Class 1 and Class 3 connections */
    if (((connectionIsIOConnection(pConnection) == FALSE) && (pConnection->cfg.bTransportClass != Class3)) ||
        (((pConnection->cfg.bTransportType & (~SERVER_CLASS_TYPE_MASK)) == 0) && (pConnection->cfg.bTransportType & CLASS_TYPE_MASK) != Cyclic))
	{
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_CLASS_TRIGGER_INVALID);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	iClass = (UINT32)pduGetTagByType(&pConnection->cfg.PDU, TAG_TYPE_CLASS, 0, &iClassTagSize);    
	iInstance = (UINT32)pduGetTagByType(&pConnection->cfg.PDU, TAG_TYPE_INSTANCE, 0, &iInstanceTagSize);    

	/* Check for some basic path conformance. The path should reference an object or a tag or an object/tag */
	if ( iClassTagSize == 0 || iInstanceTagSize == 0 )
	{
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}
	else if (((connectionIsIOConnection(pConnection)) && (iClass != ASSEMBLY_CLASS)) ||
			 ((pConnection->cfg.bTransportClass == Class3) && (iClass != ROUTER_CLASS))) 
	{
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	eipmbsParseConnectionPath( &pConnection->cfg );

	/* Check if keying is being used, then it must match the target Ids */
	pduGetTagByType(&pConnection->cfg.PDU, TAG_TYPE_KEY, 0, &iTagSize);    
	if ( iTagSize != 0 )
	{
		/* Check if Vendor Id and Product code are filled and match the target ones */
		if ( ( pConnection->cfg.deviceId.iVendorID && pConnection->cfg.deviceId.iVendorID != MODBUS_VENDOR_ID ) ||
			 ( pConnection->cfg.deviceId.iProductCode && pConnection->cfg.deviceId.iProductCode != MODBUS_PRODUCT_CODE ) )
		{
			connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_VENDOR_PRODUCT_CODE_MISMATCH);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}
    
		/* Check if Product type is filled and match the target one */
		if ( pConnection->cfg.deviceId.iProductType && pConnection->cfg.deviceId.iProductType != MODBUS_PRODUCT_TYPE )
		{
			connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_PRODUCT_TYPE_MISMATCH);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		/* Check if Revisions are filled and match the target ones */
		if ( ( (pConnection->cfg.deviceId.bMajorRevision & MAJOR_REVISION_MASK) &&
			   (pConnection->cfg.deviceId.bMajorRevision & MAJOR_REVISION_MASK) != 0 ) ||
			 ( pConnection->cfg.deviceId.bMinorRevision != 0 && 
			   pConnection->cfg.deviceId.bMinorRevision != 0 &&
			   !((pConnection->cfg.deviceId.bMajorRevision & MAJOR_REVISION_MASK) == 0) &&
			   !( (pConnection->cfg.deviceId.bMajorRevision & MAJOR_REVISION_MASK) == 0 && 
				  (pConnection->cfg.deviceId.bMajorRevision & KEY_COMPATIBILITY_FLAG) &&
				  (pConnection->cfg.deviceId.bMinorRevision <= 0) ) ) )            
		{
			connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_REVISION_MISMATCH);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}                
	}

	/* get the module config data */
	pModuleConfigData = (UINT8*)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_DATA, 0, &iModuleConfigSize );

	if (pConnection->cfg.bTransportClass == Class3)
	{
		/* must connected to instance 1 of the message router */
		if (pConnection->cfg.iTargetConfigConnInstance != 1)
		{
			connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}
	}

	/* Assign connection sizes */
    if ( bLargeFwdOpen )
	{
		pConnection->cfg.modbusCfg.iModbusProduceSize = (UINT16)pFwdOpen->lLargeLeTOConnParams;
		pConnection->cfg.modbusCfg.iModbusConsumeSize = (UINT16)pFwdOpen->lLargeLeOTConnParams;
	}
	else
	{
		pConnection->cfg.modbusCfg.iModbusProduceSize = (UINT16)(pFwdOpen->iLeTOConnParams & DATA_SIZE_MASK);
		pConnection->cfg.modbusCfg.iModbusConsumeSize = (UINT16)(pFwdOpen->iLeOTConnParams & DATA_SIZE_MASK);
	}

	if (pConnection->cfg.bTransportClass != Class0)
	{
		/* Remove 2 bytes for data sequence count that is used when data changes */
		pConnection->cfg.modbusCfg.iModbusProduceSize -= DATA_SEQUENCE_COUNT_SIZE;
		pConnection->cfg.modbusCfg.iModbusConsumeSize -= DATA_SEQUENCE_COUNT_SIZE;
	}

	if (pConnection->cfg.bTransportClass == Class3)
	{
		pConnection->cfg.iCurrentProducingSize = pConnection->cfg.modbusCfg.iModbusProduceSize;
		pConnection->cfg.iCurrentConsumingSize = pConnection->cfg.modbusCfg.iModbusConsumeSize;
	}
	else
	{
		pConnection->cfg.iCurrentProducingSize = pConnection->cfg.modbusCfg.iModbusProduceSize;
		/* Remove Run/Idle header */
		pConnection->cfg.modbusCfg.iModbusConsumeSize -= 4;
		pConnection->cfg.iCurrentConsumingSize = pConnection->cfg.modbusCfg.iModbusConsumeSize;
	}


	/* 32-bit Run/Idle header required for Modbus */
	pConnection->cfg.bInputRunProgramHeader = TRUE;
	pConnection->cfg.bOutputRunProgramHeader = FALSE;

	if (connectionIsIOConnection(pConnection))
	{
		/* Make sure for received either connection points */
		if ( pConnection->cfg.iTargetConsumingConnPoint == INVALID_CONN_POINT || pConnection->cfg.iTargetProducingConnPoint == INVALID_CONN_POINT )
		{
			connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		if (iModuleConfigSize > 0)
		{
			if (iModuleConfigSize > MAX_MODBUS_WRITE_SIZE)
			{
				connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONFIG_FORMAT);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			/* verify that the configuration instance is a holding register or coil */
			if ((pConnection->cfg.iTargetConfigConnInstance >= HOLDING_REGISTER_START) &&
				(pConnection->cfg.iTargetConfigConnInstance <= HOLDING_REGISTER_END))
			{
				if ((iModuleConfigSize%2) || (pConnection->cfg.iTargetConfigConnInstance + iModuleConfigSize/2 > HOLDING_REGISTER_END))
				{
					connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONFIG_FORMAT);
					connectionRemove( pConnection, FALSE );
					return TRUE;
				}
			}
			else
			{
				connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

		}

        if ((pConnection->cfg.iTargetConsumingConnPoint != MODBUS_HEARTBEAT_CONN_POINT) &&
            (pConnection->cfg.iTargetConsumingConnPoint != MODBUS_LISTEN_ONLY_CONN_POINT))
        {
			/* must be consuming holding registers or coils */
			if ((pConnection->cfg.iTargetConsumingConnPoint >= HOLDING_REGISTER_START) &&
				(pConnection->cfg.iTargetConsumingConnPoint <= HOLDING_REGISTER_END))
			{
				/* Holding registers are 16-bit values, so data must match up */
				if ((pConnection->cfg.modbusCfg.iModbusConsumeSize%2) ||
					(pConnection->cfg.modbusCfg.iModbusConsumeSize > MAX_MODBUS_WRITE_SIZE) ||
					(pConnection->cfg.iTargetConsumingConnPoint+pConnection->cfg.modbusCfg.iModbusConsumeSize/2 > HOLDING_REGISTER_END))
				{
					connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
					connectionRemove( pConnection, FALSE );
					return TRUE;
				}
			}
			else
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
		}
		else
		{
            /* Require Run/Idle header for heartbeat connection (size previously removed) */
			if (pConnection->cfg.modbusCfg.iModbusConsumeSize != 0)
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
                connectionRemove( pConnection, FALSE );
		        return TRUE;
            }
		}

		if (pConnection->cfg.iTargetProducingConnPoint != MODBUS_HEARTBEAT_CONN_POINT)
		{
			if (((pConnection->cfg.iTargetProducingConnPoint >= HOLDING_REGISTER_START) &&
				(pConnection->cfg.iTargetProducingConnPoint <= HOLDING_REGISTER_END)) ||
				((pConnection->cfg.iTargetProducingConnPoint >= INPUT_REGISTER_START) &&
				(pConnection->cfg.iTargetProducingConnPoint <= INPUT_REGISTER_END)))
			{
				/* Registers are 16-bit values, so data must match up */
				if ((pConnection->cfg.modbusCfg.iModbusProduceSize%2) ||
					(pConnection->cfg.modbusCfg.iModbusProduceSize > MAX_MODBUS_WRITE_SIZE) ||
					((pConnection->cfg.iTargetProducingConnPoint >= HOLDING_REGISTER_START) &&
					(pConnection->cfg.iTargetProducingConnPoint <= HOLDING_REGISTER_END) &&
					((pConnection->cfg.iTargetProducingConnPoint+pConnection->cfg.modbusCfg.iModbusProduceSize/2) > HOLDING_REGISTER_END)) ||
					((pConnection->cfg.iTargetProducingConnPoint >= INPUT_REGISTER_START) &&
					(pConnection->cfg.iTargetProducingConnPoint <= INPUT_REGISTER_END) &&
					((pConnection->cfg.iTargetProducingConnPoint+pConnection->cfg.modbusCfg.iModbusProduceSize/2) > INPUT_REGISTER_END)))
				{
					connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
					connectionRemove( pConnection, FALSE );
					return TRUE;
				}

			}
			else
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
		}
		else
		{
            /* Don't allow for Run/Idle header for heartbeat connection */
			if (pConnection->cfg.modbusCfg.iModbusProduceSize != 0)
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
                connectionRemove( pConnection, FALSE );
		        return TRUE;
            }
		}
	}

	if ((pConnection->cfg.iConsumingConnectionType == NullConnType ) && 
		(pConnection->cfg.iProducingConnectionType == NullConnType))
	{
		/* NULL forward open */
		return eipmbsconnmgrNULLConnection(pSession, pRequest, pConnection, pFwdOpen);
	}

	/* must be bi-direction connection */
	if ((pConnection->cfg.iConsumingConnectionType == NullConnType ) ||
		(pConnection->cfg.iProducingConnectionType == NullConnType))
	{
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONN_TYPE);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	if ( ( pConnection->cfg.iConsumingConnectionType != PointToPoint ) || 
		 ( pConnection->cfg.iProducingConnectionType != PointToPoint && pConnection->cfg.iProducingConnectionType != Multicast ) )
	{
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONN_TYPE);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		/* Check if we already have a connection with the same serial numbers,
		   then it's a duplicate Forward Open */
		if ( pLoopConnection != pConnection && pLoopConnection->lConnectionState == ConnectionEstablished &&
			 pLoopConnection->iConnectionSerialNbr == pConnection->iConnectionSerialNbr &&
			 pLoopConnection->iVendorID == pConnection->iVendorID &&
			 pLoopConnection->lDeviceSerialNbr == pConnection->lDeviceSerialNbr)
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_DUPLICATE_FWD_OPEN);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}
	}

	/* Reset timeout ticks based on the connection settings */
	connectionRecalcTimeouts( pConnection );

	if (connectionIsIOConnection(pConnection))
	{
		/* Check what incoming connection are already configured for these connection points */    
		for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
		{
			if ( pLoopConnection == pConnection || pLoopConnection->cfg.bOriginator )
				continue;

			/* ensure a write register is not already being controlled by another connection */
			if (eipmbsIsDataConnPoint(pLoopConnection->cfg.iTargetConsumingConnPoint) &&
				(pConnection->cfg.modbusCfg.iModbusConsumeSize > 0))
			{
				if (((pConnection->cfg.iTargetConsumingConnPoint >= pLoopConnection->cfg.iTargetConsumingConnPoint) && 
					(pConnection->cfg.iTargetConsumingConnPoint < pLoopConnection->cfg.iTargetConsumingConnPoint+pLoopConnection->cfg.modbusCfg.iModbusConsumeSize)) ||
				((pConnection->cfg.iTargetConsumingConnPoint+pConnection->cfg.modbusCfg.iModbusConsumeSize >= pLoopConnection->cfg.iTargetConsumingConnPoint) && 
					(pConnection->cfg.iTargetConsumingConnPoint+pConnection->cfg.modbusCfg.iModbusConsumeSize < pLoopConnection->cfg.iTargetConsumingConnPoint+pLoopConnection->cfg.modbusCfg.iModbusConsumeSize)))
					pOTConnection = pLoopConnection;
			}

			if ((pLoopConnection->cfg.iTargetProducingConnPoint != MODBUS_HEARTBEAT_CONN_POINT) &&
				(pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint ) &&
				(pLoopConnection->cfg.modbusCfg.iModbusProduceSize == pConnection->cfg.modbusCfg.iModbusProduceSize) &&
			    /* Ensure no overlap between Class0 and Class1 connections */
				(pLoopConnection->cfg.bTransportClass == pConnection->cfg.bTransportClass))
				pTOConnection = pLoopConnection;

			/* Ensure that configuration instance is only written to once */
			if ((iModuleConfigSize > 0) && 
				((pConnection->cfg.iTargetConfigConnInstance == pLoopConnection->cfg.iTargetConsumingConnPoint) ||
				(pConnection->cfg.iTargetConfigConnInstance == pLoopConnection->cfg.iTargetConfigConnInstance)))
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_OWNERSHIP_CONFLICT);
				connectionRemove( pConnection, FALSE );
		        return TRUE;
			}
		}
	}

	/* If a new instance */
	if ( pOTConnection == NULL && pTOConnection == NULL )
	{
		pConnection->cfg.nInstance = connectionGetUnusedInstance();
	}
	else 
	{
		if ( pOTConnection != NULL ||
			 pConnection->cfg.iProducingConnectionType != Multicast ||
			 pTOConnection->cfg.iProducingConnectionType != Multicast ||
			 pConnection->cfg.lProducingDataRate != pTOConnection->cfg.lProducingDataRate )
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_OWNERSHIP_CONFLICT);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		pConnection->cfg.nInstance = connectionGetUnusedInstance();                                        
	}

	pTcpipInterface = tcpipFindClaimedHostAddress( pSession->lHostIPAddr );
	if (pTcpipInterface == NULL)
	{
		/* This shouldn't happen */
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	/* For Class 1 using UDP packets to transport I/O 
	 * For Class 3 using TCP packets to transport data or I/O */
	usPort = (UINT16)(connectionIsIOConnection(pConnection) ? CLASS1_UDP_PORT : pTcpipInterface->iPort);

	/* Populate the sReceiveAddr */
	platformSetSockaddrIn( &pConnection->sReceiveAddr, usPort, pConnection->cfg.lHostIPAddr );

	/* Populate the sTransmitAddr (IP Address is actually set below) */
	platformSetSockaddrIn( &pConnection->sTransmitAddr, usPort, htonl( INADDR_ANY ) );
 
	if ( pConnection->cfg.iProducingConnectionType != Multicast )
	{
		/* Use transmit address if any sent with Forward Open, otherwise - use default */
		if ( platformGetInternetAddress(pRequest->sTOTagAddr.sin_addr) != 0 )
		{
			memcpy( &pConnection->sTransmitAddr, &pRequest->sTOTagAddr, sizeof(struct sockaddr_in) );
		}
		else		
			platformGetInternetAddress( pConnection->sTransmitAddr.sin_addr ) = pSession->lClientIPAddr;
	}
	else
	{
		/* Determine multicast address to use based on Ethernet Link attribute */
		switch(pTcpipInterface->InterfaceConfig.MulticastData.bAllocControl)
		{
		case 0:
			/* multicast algorithm */
			platformGetInternetAddress(pConnection->sTransmitAddr.sin_addr) = platformGetInternetAddress(pTcpipInterface->MulticastBaseAddr.sin_addr) + htonl(pTcpipInterface->lMulticastAddrCount%CIP_MAX_NUM_MULTICAST_ADDRESS);
			break;
		case 1:
			/* "static" multicast address */
			platformGetInternetAddress(pConnection->sTransmitAddr.sin_addr) = pTcpipInterface->InterfaceConfig.MulticastData.lMcastStartAddr + htonl(pTcpipInterface->lMulticastAddrCount%pTcpipInterface->InterfaceConfig.MulticastData.iNumMcast);
			break;
		}

		pTcpipInterface->lMulticastAddrCount++;

		/* Check if this is a multicast connection hooking up to one of the previously 
		established multicast connections. */
		pMulticastProducer = connectionGetAnyMultiProducer( pConnection );

		if ( pMulticastProducer != NULL)
		{
			pConnection->lProducingCID = pMulticastProducer->lProducingCID;
			pConnection->sTransmitAddr = pMulticastProducer->sTransmitAddr;            
		}
	}

	if (connectionIsIOConnection(pConnection))
	{
		/* Ensure that the write data can be cached to determine the correct
		   Modbus write function */
		pConnection->iModbusConsumeDataSize = pConnection->cfg.modbusCfg.iModbusConsumeSize;
		pConnection->pModbusConsumeData = EtIP_malloc(NULL, pConnection->iModbusConsumeDataSize);
		if ((pConnection->pModbusConsumeData == NULL) &&
			(pConnection->iModbusConsumeDataSize > 0))
		{
			connmgrStats.iOpenResourceRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_NO_RESOURCE, 0);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		/* Determine the initial "write type" */
		if ((pConnection->cfg.lProducingDataRate == pConnection->cfg.lConsumingDataRate) &&
			((pConnection->cfg.iTargetProducingConnPoint >= HOLDING_REGISTER_START) &&
						(pConnection->cfg.iTargetProducingConnPoint <= HOLDING_REGISTER_END)) &&
			(pConnection->cfg.modbusCfg.iModbusConsumeSize > 0) && (pConnection->cfg.modbusCfg.iModbusProduceSize > 0))
		{
			pConnection->iModbusWriteType = ModbusWriteTypeReadWrite;
		}
		else
		{

			pConnection->iModbusWriteType = ModbusWriteTypeWriteMultiple;
		}
	}

	//send configuration data to target
	if (iModuleConfigSize > 0)
		return eipmbsconnmgrSendConfigData(pRequest, pConnection);

	return eipmbsconnmgrVerifyConnection(pRequest, pConnection, pFwdOpen);
}

/*---------------------------------------------------------------------------
** eipmbsconnmgrVerifyConnection( )
**
** Verify that the Modbus device can read the registers specified in the connection
** or in the case of a "Write Only" device, that the device exists
**---------------------------------------------------------------------------
*/
BOOL eipmbsconnmgrVerifyConnection(REQUEST* pRequest, CONNECTION* pConnection, FWD_OPEN* pFwdOpen)
{
	MBSRequest Request;
	UINT8 *pModbusData;

	if ((connectionIsIOConnection(pConnection)) &&
		(pConnection->cfg.modbusCfg.iModbusProduceSize != 0))
	{
		/* Verify that the target can handle reading the specified range of registers */

		if ((pConnection->cfg.iTargetProducingConnPoint >= HOLDING_REGISTER_START) &&
			(pConnection->cfg.iTargetProducingConnPoint <= HOLDING_REGISTER_END))
		{
			/* Data validated, convert to a Modbus message */
			PopulateModbusAddrFromConn(&Request, pConnection);
			Request.lMessageTimeout = pConnection->cfg.lProducingDataRate;

			Request.bFunction = MBUS_READ_HOLDING_REGISTERS;
			Request.iDataSize = 4;

			pModbusData = Request.requestData;
			UINT16_BE_SET(pModbusData, pConnection->cfg.iTargetProducingConnPoint-1);	/* Starting address */
			pModbusData += sizeof(UINT16);
			UINT16_BE_SET(pModbusData, pConnection->cfg.modbusCfg.iModbusProduceSize/2);	/* Number of registers */
			pModbusData += sizeof(UINT16);

			pConnection->nModbusReadRequestID = modbusSendRequest(&Request, TRUE);
			if (pConnection->nModbusReadRequestID < MBUS_REQUEST_INDEX_BASE)
			{
				connmgrStats.iOpenResourceRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_NO_RESOURCE, 0);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
		}
		else if ((pConnection->cfg.iTargetProducingConnPoint >= INPUT_REGISTER_START) &&
				(pConnection->cfg.iTargetProducingConnPoint <= INPUT_REGISTER_END))
		{

			/* Data validated, convert to a Modbus message */
			PopulateModbusAddrFromConn(&Request, pConnection);
			Request.lMessageTimeout = pConnection->cfg.lProducingDataRate;

			Request.bFunction = MBUS_READ_INPUT_REGISTERS;
			Request.iDataSize = 4;

			pModbusData = Request.requestData;
			UINT16_BE_SET(pModbusData, pConnection->cfg.iTargetProducingConnPoint-1);	/* Starting address */
			pModbusData += sizeof(UINT16);
			UINT16_BE_SET(pModbusData, pConnection->cfg.modbusCfg.iModbusProduceSize/2);	/* Number of registers */
			pModbusData += sizeof(UINT16);

			pConnection->nModbusReadRequestID = modbusSendRequest(&Request, TRUE);
			if (pConnection->nModbusReadRequestID < MBUS_REQUEST_INDEX_BASE)
			{
				connmgrStats.iOpenResourceRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_NO_RESOURCE, 0);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
		}
		else
		{
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress, 
				"eipmbsconnmgrVerifyTarget Internal Logic FAILURE pConnection->cfg.iTargetProducingConnPoint set to %d", pConnection->cfg.iTargetProducingConnPoint);
		}

		pConnection->nRequestId = pRequest->nIndex;
		pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
		return FALSE;
	}

	/* Just verify that the target exists with a simple read */
	PopulateModbusAddrFromConn(&Request, pConnection);
	Request.lMessageTimeout = pConnection->cfg.lProducingDataRate;

	Request.bFunction = MBUS_READ_INPUT_REGISTERS;
	Request.iDataSize = 4;

	pModbusData = Request.requestData;
	UINT16_BE_SET(pModbusData, 0);	/* Starting address */
	pModbusData += sizeof(UINT16);
	UINT16_BE_SET(pModbusData, 1);	/* Number of registers */
	pModbusData += sizeof(UINT16);

	pConnection->nModbusReadRequestID = modbusSendRequest(&Request, TRUE);
	if (pConnection->nModbusReadRequestID < MBUS_REQUEST_INDEX_BASE)
	{
		connmgrStats.iOpenResourceRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_NO_RESOURCE, 0);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	pConnection->nRequestId = pRequest->nIndex;
	pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
	return FALSE;
}

/*---------------------------------------------------------------------------
** eipmbsconnmgrNULLConnection( )
**
** Incoming "NULL" connection request for Modbus device received
**---------------------------------------------------------------------------
*/
BOOL eipmbsconnmgrNULLConnection(SESSION* pSession, REQUEST* pRequest, CONNECTION* pConnection, FWD_OPEN* pFwdOpen)
{
	CONNECTION  *pLoopConnection, 
				*pOwnerConnection = NULL;

	/* compiler warning - pSession may be used for debug statements */
	pSession = pSession;

    for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
    {
        /* Check if we already have a connection with the same serial numbers,
           then it's a duplicate Forward Open */
        if ( pLoopConnection != pConnection && pLoopConnection->lConnectionState == ConnectionEstablished &&
			 !pLoopConnection->cfg.bOriginator &&
             pLoopConnection->iConnectionSerialNbr == pConnection->iConnectionSerialNbr &&
             pLoopConnection->iVendorID == pConnection->iVendorID &&
             pLoopConnection->lDeviceSerialNbr == pConnection->lDeviceSerialNbr)
        {
			pOwnerConnection = pLoopConnection;
			break;
        }
    }

	if (pOwnerConnection == NULL)
	{
		/* Couldn't find the matching connection, connection will be rejected */
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_NO_CONTROLLING_CONNECTION);
        connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	if ((pConnection->cfg.bTransportClass != pOwnerConnection->cfg.bTransportClass) ||
        (pConnection->cfg.bTransportType != pOwnerConnection->cfg.bTransportType) || 
		(pConnection->cfg.iTargetConfigConnInstance != pOwnerConnection->cfg.iTargetConfigConnInstance) ||
		(pConnection->cfg.iTargetProducingConnPoint != pOwnerConnection->cfg.iTargetProducingConnPoint) ||
		(pConnection->cfg.modbusCfg.iModbusProduceSize != pOwnerConnection->cfg.modbusCfg.iModbusProduceSize) ||
		(pConnection->cfg.iTargetConsumingConnPoint != pOwnerConnection->cfg.iTargetConsumingConnPoint) ||
		(pConnection->cfg.modbusCfg.iModbusConsumeSize != pOwnerConnection->cfg.modbusCfg.iModbusConsumeSize) ||
		(pConnection->cfg.lProducingDataRate != pOwnerConnection->cfg.lProducingDataRate) ||
		(pConnection->cfg.lConsumingDataRate != pOwnerConnection->cfg.lConsumingDataRate) ||
		(pConnection->cfg.bProductionOTInhibitInterval != pOwnerConnection->cfg.bProductionOTInhibitInterval) ||
		(pConnection->cfg.bProductionTOInhibitInterval != pOwnerConnection->cfg.bProductionTOInhibitInterval) ||
		(pConnection->cfg.iProducingPriority != pOwnerConnection->cfg.iProducingPriority) ||
		(pConnection->cfg.iConsumingPriority != pOwnerConnection->cfg.iConsumingPriority) ||
		(pConnection->cfg.bTimeoutMultiplier != pOwnerConnection->cfg.bTimeoutMultiplier))
	{
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
		connectionRemove( pConnection, FALSE );
	    return TRUE;
	}

	if (gbAppVerifyConnection)
	{
		pConnection->cfg.nInstance = connectionGetUnusedInstance();
		pConnection->nRequestId = pRequest->nIndex;
		pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
		notifyEvent( NM_CONNECTION_VERIFICATION, pConnection->cfg.nInstance, 0 );
		return FALSE;
	}
	
	return eipmbsconnmgrSendConfigData(pRequest, pConnection);
}

/*---------------------------------------------------------------------------
** eipmbsconnmgrTranslateReadRsp( )
** 
** Send "Configuration data" as a Modbus Write message
**---------------------------------------------------------------------------
*/
BOOL eipmbsconnmgrSendConfigData(REQUEST* pRequest, CONNECTION* pConnection)
{
	MBSRequest Request;
    UINT16  i, iTemp, iModuleConfigSize;
    UINT8	*pModuleConfigData, *pModbusData, *pCIPData;

	/* get the module config data */
	pModuleConfigData = (UINT8*)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_DATA, 0, &iModuleConfigSize );

	PopulateModbusAddrFromConn(&Request, pConnection);
	Request.lMessageTimeout = pConnection->cfg.lConsumingDataRate;

	Request.bFunction = MBUS_WRITE_MULTIPLE_REGISTERS;
	Request.iDataSize = (UINT16)(5+iModuleConfigSize);

	pModbusData = Request.requestData;
	UINT16_BE_SET(pModbusData, pConnection->cfg.iTargetConfigConnInstance-1);		/* Starting address */
	pModbusData += sizeof(UINT16);
	UINT16_BE_SET(pModbusData, iModuleConfigSize/2);					/* Number of registers */
	pModbusData += sizeof(UINT16);
	*pModbusData++ = (UINT8)iModuleConfigSize;					/* Number of bytes */

	for (i = 0, pCIPData = pModuleConfigData; 
		i < iModuleConfigSize/2; 
		i++, pCIPData += sizeof(UINT16), pModbusData += sizeof(UINT16))
	{
		iTemp = UINT16_GET(pCIPData);
		UINT16_BE_SET(pModbusData, iTemp);
	}

	pConnection->nModbusWriteRequestID = modbusSendRequest(&Request, TRUE);
	pConnection->nRequestId = pRequest->nIndex;
	pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
	return FALSE;
}

/*---------------------------------------------------------------------------
** eipmbsconnmgrTranslateReadRsp( )
** 
** Handle a Modbus read response for a connection
**---------------------------------------------------------------------------
*/
void eipmbsconnmgrTranslateReadRsp(CONNECTION* pConnection, MBSResponse* pResponse)
{
	REQUEST* pRequest;
	MBSRequest Request;
	UINT8 *pModbusData, *pCIPData;
    FWD_OPEN    FwdOpen;
	UINT16 i, iTemp;

	/* Clear the Read Request ID */
	pConnection->nModbusReadRequestID = 0;

	switch(pConnection->lConnectionState)
	{
	case ConnectionNonExistent:
		/* Trying to establish a connection to a Modbus device
		   This is the initial read to verify the connection */

		if ((pRequest = requestGetByRequestId(pConnection->nRequestId)) == NULL)
		{
			/* Request Id was not found, just silently remove the connection */
			connmgrStats.iOpenOtherRejects++;
#ifdef CONNECTION_STATS
			gSystemStats.iNumFailedConnections++;
#endif
			connectionRemove( pConnection, TRUE );
			return;
		}

		/* If Modbus failure, translate to CIP failure */
		if (pResponse->bGeneralStatus != 0)
		{
			/* remove the remnents of the Forward Open request */
			connmgrInitFwdOpenTypeFromBuf(pRequest->cfg.objectCfg.pData, &FwdOpen, pRequest->cfg.objectCfg.bService == LARGE_FWD_OPEN_CMD_CODE);

			switch (pResponse->bGeneralStatus)
			{
				case MBUS_EXCEPTION_ILLEGAL_FUNCTION:
					if ((connectionIsIOConnection(pConnection)) &&
						(pConnection->cfg.modbusCfg.iModbusConsumeSize != 0))
					{
						/* Only apply error code if there isn't any T->O data */
						connmgrStats.iOpenOtherRejects++;
						connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONN_POINT);
						pRequest->nState = REQUEST_RESPONSE_RECEIVED;
						connectionRemove( pConnection, FALSE );
						return;
					}
					break;
				case MBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS:
				case MBUS_EXCEPTION_ILLEGAL_DATA_VALUE:
					if ((connectionIsIOConnection(pConnection)) &&
						(pConnection->cfg.modbusCfg.iModbusConsumeSize != 0))
					{
						/* Only apply error code if there isn't any T->O data */
						connmgrStats.iOpenOtherRejects++;
						connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONN_SIZE);
						pRequest->nState = REQUEST_RESPONSE_RECEIVED;
						connectionRemove( pConnection, FALSE );
						return;
					}
					break;
				case MBUS_EXCEPTION_SLAVE_DEVICE_FAILURE:
					connmgrStats.iOpenOtherRejects++;
#ifdef CONNECTION_STATS
					gSystemStats.iNumFailedConnections++;
#endif
					pRequest->bGeneralError = ROUTER_ERROR_DEV_IN_WRONG_STATE;
					pRequest->nState = REQUEST_RESPONSE_RECEIVED;
					connectionRemove( pConnection, FALSE );
					return;
				default:
					connmgrStats.iOpenOtherRejects++;
					connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_UNKNOWN_MODBUS_ERROR, pResponse->bGeneralStatus);
					pRequest->nState = REQUEST_RESPONSE_RECEIVED;
					connectionRemove( pConnection, FALSE );
					return;
			}
		}

		/* Successful response or tolerable failure */
		if (gbAppVerifyConnection)
		{
			notifyEvent( NM_CONNECTION_VERIFICATION, pConnection->cfg.nInstance, 0 );
			return;
		}

		connmgrPrepareConnection(NULL, pConnection, pRequest, 0, NULL);
		break;
	case ConnectionEstablished:
		if (pResponse->bGeneralStatus != 0)
		{
			if ((pResponse->bGeneralStatus == MBUS_EXCEPTION_ILLEGAL_FUNCTION) &&
				(pConnection->iModbusWriteType == ModbusWriteTypeReadWrite))
			{
				/* Device doesn't support Read/Write Function, try write only */
				pConnection->iModbusWriteType = ModbusWriteTypeWriteMultiple;
				pConnection->nModbusReadRequestID = 0;

				if (pConnection->bRunIdleFlag)
				{
					PopulateModbusAddrFromConn(&Request, pConnection);
					Request.lMessageTimeout = pConnection->cfg.lConsumingDataRate;

					Request.bFunction = MBUS_WRITE_MULTIPLE_REGISTERS;
					Request.iDataSize = (UINT16)(5+pConnection->iModbusConsumeDataSize);

					pModbusData = Request.requestData;
					UINT16_BE_SET(pModbusData, pConnection->cfg.iTargetConsumingConnPoint-1);		/* Starting address */
					pModbusData += sizeof(UINT16);
					UINT16_BE_SET(pModbusData, pConnection->iModbusConsumeDataSize/2); /* Number of registers */
					pModbusData += sizeof(UINT16);
					*pModbusData++ = (UINT8)pConnection->iModbusConsumeDataSize;		/* Number of bytes */

					for (i = 0, pCIPData = pConnection->pModbusConsumeData; 
						i < pConnection->iModbusConsumeDataSize/2; 
						i++, pCIPData += sizeof(UINT16), pModbusData += sizeof(UINT16))
					{
						iTemp = UINT16_GET(pCIPData);
						UINT16_BE_SET(pModbusData, iTemp);
					}

					pConnection->nModbusWriteRequestID = modbusSendRequest(&Request, TRUE);
				}

				return;
			}

			/* Modbus will send an exception if the Read/Write command has no
			   registers to write, make an allowance for it */
			if ((pResponse->bGeneralStatus != MBUS_EXCEPTION_ILLEGAL_DATA_VALUE) ||
				(pConnection->iModbusWriteType != ModbusWriteTypeReadWrite) ||
				(pConnection->bRunIdleFlag))
			{
				/* If there is an error, just remove the connection */
				connectionRemove( pConnection, FALSE );
				return;
			}
		}

		/* Ignore the byte count in the response data */
		eipmbsioSendIOPacket( pConnection, (UINT8*)(pResponse->responseData+1), (UINT16)(pResponse->iDataSize-1));
		break;
	}
}

/*---------------------------------------------------------------------------
** eipmbsconnmgrTranslateWriteRsp( )
** 
** Handle a Modbus write response for a connection
**---------------------------------------------------------------------------
*/
void eipmbsconnmgrTranslateWriteRsp(CONNECTION* pConnection, MBSResponse* pResponse)
{
	UINT16 iTemp, iModuleConfigSize;
	REQUEST* pRequest;
    UINT8	*pModuleConfigData;
    FWD_OPEN    FwdOpen;

	/* Clear the Write Request ID */
	pConnection->nModbusWriteRequestID = 0;

	switch (pConnection->lConnectionState)
	{
	case ConnectionNonExistent:
		/* Trying to establish a connection to a Modbus device
		   This is the attempt to write configuration data */

		if ((pRequest = requestGetByRequestId(pConnection->nRequestId)) == NULL)
		{
			/* Request Id was not found, just silently remove the connection */
			connmgrStats.iOpenOtherRejects++;
#ifdef CONNECTION_STATS
			gSystemStats.iNumFailedConnections++;
#endif
			connectionRemove( pConnection, TRUE );
			return;
		}

		/* remove the remnents of the Forward Open request */
		connmgrInitFwdOpenTypeFromBuf(pRequest->cfg.objectCfg.pData, &FwdOpen, pRequest->cfg.objectCfg.bService == LARGE_FWD_OPEN_CMD_CODE);

		/* Response came from configuration data */
		if (pResponse->bGeneralStatus != 0)
		{
			if (pResponse->bGeneralStatus == MBUS_EXCEPTION_ILLEGAL_FUNCTION)
			{
				/* get the module config data */
				pModuleConfigData = (UINT8*)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_DATA, 0, &iModuleConfigSize );

				if ((iModuleConfigSize == 2) && (pConnection->nModbusReadRequestID == 0))
				{
					/* set ReadRequest to nonzero/non-request value to help the state machine */
					pConnection->nModbusReadRequestID = MBUS_INVALID_REQUEST_INDEX;

					iTemp = UINT16_BE_GET(pModuleConfigData);
					eipmbsconnmgrWriteSingleReg(pConnection, pConnection->cfg.iTargetConfigConnInstance, iTemp);
					return;
				}
			}

			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONFIG_FORMAT);
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			connectionRemove( pConnection, FALSE );
			return;
		}

		eipmbsconnmgrVerifyConnection(pRequest, pConnection, &FwdOpen);
		break;
	case ConnectionEstablished:
		switch (pConnection->iModbusWriteType)
		{
		case ModbusWriteTypeWriteMultiple:
			if (pResponse->bGeneralStatus != 0)
			{
				/* If the multiple write command fails and we're only writing a
				   single register, try to write a single register 
				   Otherwise, remove the connection */
				if ((pResponse->bGeneralStatus != MBUS_EXCEPTION_ILLEGAL_FUNCTION) ||
					(pConnection->cfg.modbusCfg.iModbusConsumeSize != 2))
				{
					connectionRemove( pConnection, FALSE );
					return;
				}

				pConnection->iModbusWriteType = ModbusWriteTypeWriteSingle;
				iTemp = UINT16_BE_GET(pConnection->pModbusConsumeData);
				eipmbsconnmgrWriteSingleReg(pConnection, pConnection->cfg.iTargetConsumingConnPoint, iTemp);
			}
			break;
		case ModbusWriteTypeWriteSingle:
			if (pResponse->bGeneralStatus != 0)
			{
				/* If there is an error, just remove the connection */
				connectionRemove( pConnection, FALSE );
				return;
			}
			break;
		}
		break;
	}
}

/*---------------------------------------------------------------------------
** eipmbsioSendIOPacket( )
** 
** Send Modbus data on a class1 connection
**---------------------------------------------------------------------------
*/
void eipmbsioSendIOPacket(CONNECTION* pConnection, UINT8* pModbusData, UINT16 iDataSize )
{
    UINT8*  pData = gmsgBuf;
	UINT16 i, iTemp;
#ifdef CONNECTION_STATS
    UINT32  lSendDelay;
    UINT32  lTickCount = gdwTickCount;
#endif /* CONNECTION_STATS */

#ifdef CONNECTION_STATS
	if ( pConnection->bCollectingStats )
	{
		if (pConnection->lStartSendTick != 0 )
		{
			if (pConnection->lLastSendTick )
			{
				if ( lTickCount >= pConnection->lLastSendTick )
					lSendDelay = lTickCount - pConnection->lLastSendTick;
				else
					lSendDelay = (0xffffffff - pConnection->lLastSendTick) + lTickCount;

				if ( lSendDelay >= pConnection->cfg.lProducingDataRate )
					lSendDelay -= pConnection->cfg.lProducingDataRate;
				else
					lSendDelay = 0;

				if ( lSendDelay > pConnection->lMaxSendDelay )
					pConnection->lMaxSendDelay = lSendDelay;
			}
			pConnection->lLastSendTick = lTickCount;
		}

		if ( !pConnection->lStartSendTick )
			pConnection->lStartSendTick = lTickCount;
    
		pConnection->lSendPackets++;
	}
#endif /* CONNECTION_STATS */

	pData = ioFormatIOPacketHeader(pConnection, iDataSize, gmsgBuf);

	for (i = 0; 
		i < iDataSize/2; 
		i++, pData += sizeof(UINT16), pModbusData += sizeof(UINT16))
	{
		iTemp = UINT16_GET(pModbusData);
		UINT16_BE_SET(pData, iTemp);
	}

	if (pConnection->cfg.bTransportClass == Class1)
		iDataSize += DATA_SEQUENCE_COUNT_SIZE;

	socketClass1Send( pConnection, (CPFHDR_SIZE + iDataSize) );                    /* Produce data */    
}

/*---------------------------------------------------------------------------
** eipmbsParseIOPacket( )
** 
** Handle a "Modbus" class1 packet and translate to the appropriate 
** Modbus request
**---------------------------------------------------------------------------
*/
void eipmbsParseIOPacket(CONNECTION* pConnection, UINT8 *pData, UINT16 iLen)
{
	MBSRequest Request;
	UINT8 *pCIPData, *pModbusData;
	UINT16 iTemp, i;

	switch (pConnection->iModbusWriteType)
	{
	case ModbusWriteTypeReadWrite:
		if (pConnection->bRunIdleFlag)
		{
			/* Make a spare copy in case this is the wrong write function */
			memcpy(pConnection->pModbusConsumeData, pData, pConnection->iModbusConsumeDataSize);

			PopulateModbusAddrFromConn(&Request, pConnection);
			Request.lMessageTimeout = pConnection->cfg.lProducingDataRate;

			Request.bFunction = MBUS_READ_WRITE_MULTIPLE_REGISTERS;
			Request.iDataSize = (UINT16)(9+iLen);

			pModbusData = Request.requestData;
			UINT16_BE_SET(pModbusData, pConnection->cfg.iTargetProducingConnPoint-1);		/* Read Starting address */
			pModbusData += sizeof(UINT16);
			UINT16_BE_SET(pModbusData, pConnection->cfg.modbusCfg.iModbusProduceSize/2); /* Number of read registers */
			pModbusData += sizeof(UINT16);
			UINT16_BE_SET(pModbusData, pConnection->cfg.iTargetConsumingConnPoint-1);		/* Write Starting address */
			pModbusData += sizeof(UINT16);
			UINT16_BE_SET(pModbusData, iLen/2); /* Number of write registers */
			pModbusData += sizeof(UINT16);
			*pModbusData++ = (UINT8)iLen;		/* Number of bytes */

			for (i = 0, pCIPData = pData; 
				i < iLen/2; 
				i++, pCIPData += sizeof(UINT16), pModbusData += sizeof(UINT16))
			{
				iTemp = UINT16_GET(pCIPData);
				UINT16_BE_SET(pModbusData, iTemp);
			}

			pConnection->nModbusReadRequestID = modbusSendRequest(&Request, TRUE);
		}
		else
		{
			/* Read/Write function won't accept not writing data, so during "idle" connection
			   only do a read of the holding registers */
			PopulateModbusAddrFromConn(&Request, pConnection);
			Request.lMessageTimeout = pConnection->cfg.lProducingDataRate;

			Request.bFunction = MBUS_READ_HOLDING_REGISTERS;
			Request.iDataSize = 4;

			pModbusData = Request.requestData;
			UINT16_BE_SET(pModbusData, pConnection->cfg.iTargetProducingConnPoint-1);	/* Starting address */
			pModbusData += sizeof(UINT16);
			UINT16_BE_SET(pModbusData, pConnection->cfg.modbusCfg.iModbusProduceSize/2);	/* Number of registers */
			pModbusData += sizeof(UINT16);

			pConnection->nModbusReadRequestID = modbusSendRequest(&Request, TRUE);
		}

		break;
	case ModbusWriteTypeWriteMultiple:
		/* Only send data when in Run mode */
		if (pConnection->bRunIdleFlag)
		{
			PopulateModbusAddrFromConn(&Request, pConnection);
			Request.lMessageTimeout = pConnection->cfg.lConsumingDataRate;

			Request.bFunction = MBUS_WRITE_MULTIPLE_REGISTERS;
			Request.iDataSize = (UINT16)(5+iLen);

			pModbusData = Request.requestData;
			UINT16_BE_SET(pModbusData, pConnection->cfg.iTargetConsumingConnPoint-1);		/* Starting address */
			pModbusData += sizeof(UINT16);
			UINT16_BE_SET(pModbusData, pConnection->cfg.modbusCfg.iModbusConsumeSize/2); /* Number of registers */
			pModbusData += sizeof(UINT16);
			*pModbusData++ = (UINT8)pConnection->cfg.modbusCfg.iModbusConsumeSize;		/* Number of bytes */

			for (i = 0, pCIPData = pData; 
				i < pConnection->cfg.modbusCfg.iModbusConsumeSize/2; 
				i++, pCIPData += sizeof(UINT16), pModbusData += sizeof(UINT16))
			{
				iTemp = UINT16_GET(pCIPData);
				UINT16_BE_SET(pModbusData, iTemp);
			}

			pConnection->nModbusWriteRequestID = modbusSendRequest(&Request, TRUE);
		}
		break;
	case ModbusWriteTypeWriteSingle:
		/* Only send data when in Run mode */
		if (pConnection->bRunIdleFlag)
		{
			iTemp = UINT16_GET(pData);
			eipmbsconnmgrWriteSingleReg(pConnection, pConnection->cfg.iTargetConsumingConnPoint, iTemp);
		}
		break;
	}

}

/*---------------------------------------------------------------------------
** eipmbsioSendReadRequest( )
** 
** Time to produce a Modbus read request for a class1 connection
**---------------------------------------------------------------------------
*/
void eipmbsioSendReadRequest(CONNECTION* pConnection, CONNECTION* pMulticastProducer)
{
	MBSRequest Request;
    UINT8 *pData, *pModbusData;

#ifdef CONNECTION_STATS
    UINT32  lSendDelay;
    UINT32  lTickCount = gdwTickCount;
#endif /* CONNECTION_STATS */


	/* Only produce once for all multicast connections producing for the same connection point */
	if (pMulticastProducer != NULL)
		return;

	/* Read/Write message is handled when O->T is received */
	if (pConnection->iModbusWriteType == ModbusWriteTypeReadWrite)
		return;

    if ( pConnection->cfg.iTargetProducingConnPoint == MODBUS_HEARTBEAT_CONN_POINT ||
		 pConnection->cfg.iTargetProducingConnPoint == MODBUS_LISTEN_ONLY_CONN_POINT )					
	{
		/* Connection statistics are usually computed when the read function
		   returns from Modbus, but in this case, there is no read */
#ifdef CONNECTION_STATS
		if ( pConnection->bCollectingStats )
		{
			if (pConnection->lStartSendTick != 0 )
			{
				if (pConnection->lLastSendTick )
				{
					if ( lTickCount >= pConnection->lLastSendTick )
						lSendDelay = lTickCount - pConnection->lLastSendTick;
					else
						lSendDelay = (0xffffffff - pConnection->lLastSendTick) + lTickCount;

					if ( lSendDelay >= pConnection->cfg.lProducingDataRate )
						lSendDelay -= pConnection->cfg.lProducingDataRate;
					else
						lSendDelay = 0;

					if ( lSendDelay > pConnection->lMaxSendDelay )
						pConnection->lMaxSendDelay = lSendDelay;
				}
				pConnection->lLastSendTick = lTickCount;
			}

			if ( !pConnection->lStartSendTick )
				pConnection->lStartSendTick = lTickCount;
    
			pConnection->lSendPackets++;
		}
#endif /* CONNECTION_STATS */

		pData = ioFormatIOPacketHeader(pConnection, 0, gmsgBuf);
		/* Produce data */
		if (pConnection->cfg.bTransportClass == Class1)
			socketClass1Send( pConnection, (CPFHDR_SIZE + DATA_SEQUENCE_COUNT_SIZE));                    
		else
			socketClass1Send( pConnection, CPFHDR_SIZE );
		return;
	}

	/* Send a read request on Modbus */
	if ((pConnection->cfg.iTargetProducingConnPoint >= HOLDING_REGISTER_START) &&
		(pConnection->cfg.iTargetProducingConnPoint <= HOLDING_REGISTER_END))
	{
		/* Data validated, convert to a Modbus message */
		PopulateModbusAddrFromConn(&Request, pConnection);
		Request.lMessageTimeout = pConnection->cfg.lProducingDataRate;

		Request.bFunction = MBUS_READ_HOLDING_REGISTERS;
		Request.iDataSize = 4;

		pModbusData = Request.requestData;
		UINT16_BE_SET(pModbusData, pConnection->cfg.iTargetProducingConnPoint-1);	/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, pConnection->cfg.modbusCfg.iModbusProduceSize/2);	/* Number of registers */
		pModbusData += sizeof(UINT16);

		pConnection->nModbusReadRequestID = modbusSendRequest(&Request, TRUE);
	}
	else if ((pConnection->cfg.iTargetProducingConnPoint >= INPUT_REGISTER_START) &&
			(pConnection->cfg.iTargetProducingConnPoint <= INPUT_REGISTER_END))
	{

		/* Data validated, convert to a Modbus message */
		PopulateModbusAddrFromConn(&Request, pConnection);
		Request.lMessageTimeout = pConnection->cfg.lProducingDataRate;

		Request.bFunction = MBUS_READ_INPUT_REGISTERS;
		Request.iDataSize = 4;

		pModbusData = Request.requestData;
		UINT16_BE_SET(pModbusData, pConnection->cfg.iTargetProducingConnPoint-1);	/* Starting address */
		pModbusData += sizeof(UINT16);
		UINT16_BE_SET(pModbusData, pConnection->cfg.modbusCfg.iModbusProduceSize/2);	/* Number of registers */
		pModbusData += sizeof(UINT16);

		pConnection->nModbusReadRequestID = modbusSendRequest(&Request, TRUE);
	}
}


/*--------------------------------------------------------------------------------
** eipmbsconnmgrWriteSingleReg( )
**
** Write a single Holding register for a connection
**--------------------------------------------------------------------------------
*/
void eipmbsconnmgrWriteSingleReg(CONNECTION* pConnection, UINT32 iRegister, UINT16 iRegisterValue)
{
	UINT8	*pData;
	MBSRequest Request;

	/* Data validated, convert to a Modbus message */
	PopulateModbusAddrFromConn(&Request, pConnection);
	Request.lMessageTimeout = pConnection->cfg.lConsumingDataRate;

	Request.bFunction = MBUS_WRITE_SINGLE_REGISTER;
	Request.iDataSize = 4;

	pData = Request.requestData;
	UINT16_BE_SET(pData, iRegister-1);		/* Starting address */
	pData += sizeof(UINT16);
	UINT16_BE_SET(pData, iRegisterValue); /* Value of register*/

	pConnection->nModbusWriteRequestID = modbusSendRequest(&Request, TRUE);
}

/*---------------------------------------------------------------------------
** eipmbsParseConnectionPath
**
** Parse Modbus Connection Path and store in CONNECTION_CFG structure members.
**---------------------------------------------------------------------------
*/
void eipmbsParseConnectionPath( CONNECTION_CFG* pConfig )
{    
	UINT16    iTagSize;
	ETAG_DATATYPE    data;

	pConfig->iTargetConsumingConnPoint = INVALID_CONN_POINT;
	pConfig->iTargetProducingConnPoint = INVALID_CONN_POINT;

	data = pduGetTagByType(&pConfig->PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
	if ( iTagSize )
		pConfig->iTargetConfigConnInstance = (UINT32)data;
	else
		pConfig->iTargetConfigConnInstance = INVALID_INSTANCE;
    
	data = pduGetTagByType(&pConfig->PDU, TAG_TYPE_INHIBIT_INTERVAL, 0, &iTagSize);
	if ( iTagSize )
		pConfig->bProductionTOInhibitInterval = (UINT8)data;

	data = pduGetTagByType(&pConfig->PDU, TAG_TYPE_KEY, 0, &iTagSize);
	if ( iTagSize == sizeof(EtIPDeviceID) )
		memcpy( &pConfig->deviceId, (UINT8*)data, sizeof(EtIPDeviceID));

	data = pduGetTagByType(&pConfig->PDU, TAG_TYPE_CONN_POINT, 0, &iTagSize);
	if ( iTagSize )
		pConfig->iTargetConsumingConnPoint = (UINT32)data;

	data = pduGetTagByType(&pConfig->PDU, TAG_TYPE_CONN_POINT, 1, &iTagSize);
	if ( iTagSize )
		pConfig->iTargetProducingConnPoint = (UINT32)data;

	data = pduGetTagByType( &pConfig->PDU, TAG_TYPE_LINK_ADDRESS, 0, &iTagSize );
	if ( iTagSize )
	{
		pConfig->modbusCfg.pModbusAddress = (UINT8*)data;
		pConfig->modbusCfg.iModbusAddressSize = iTagSize;
	}
	else
	{
		pConfig->modbusCfg.iModbusAddressSize = 0;
	}
}

/*---------------------------------------------------------------------------
** eipmbsIsDataConnPoint( )
**
** Return TRUE if passed instance should correspond to a Modbus connection
** point with some data, FALSE otherwise
**---------------------------------------------------------------------------
*/

BOOL eipmbsIsDataConnPoint( UINT32 iInstance )
{
	if ( iInstance != MODBUS_HEARTBEAT_CONN_POINT &&
		 iInstance != MODBUS_LISTEN_ONLY_CONN_POINT &&
		 iInstance != 0 &&
		 iInstance != INVALID_CONN_POINT && 
		 iInstance != INVALID_INSTANCE )
		 return TRUE;
	else
		return FALSE;
}

void PopulateModbusAddrFromConn(MBSRequest* pRequest, CONNECTION* pConnection)
{
	UINT8 bSlot;
	UINT16 iTagSize;

#ifdef ET_IP_MODBUS_SERIAL
	if (pConnection->bModbusSerialAddress != 0)
	{
		pRequest->bUnitID = pConnection->bModbusSerialAddress;
		pRequest->bModbusMsgType = MBUS_REQUESTTYPE_SERIAL;
		pRequest->lHostAddress = pConnection->cfg.lHostIPAddr;
	}
	else
	{
#endif
		memcpy(pRequest->networkPath, pConnection->cfg.modbusCfg.pModbusAddress, pConnection->cfg.modbusCfg.iModbusAddressSize);
		pRequest->networkPath[pConnection->cfg.modbusCfg.iModbusAddressSize] = 0;
		pRequest->lHostAddress = pConnection->cfg.lHostIPAddr;
		pRequest->bModbusMsgType = MBUS_REQUESTTYPE_TCP;

		/* Check if there is additional Modbus routing needed */
		bSlot = (UINT8)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_SLOT, 0, &iTagSize );
		if (iTagSize == 0)
		{
			pRequest->bUnitID = MBUS_DEFAULT_UNIT_ID;
		}
		else
		{
			pRequest->bUnitID = bSlot;
		}
#ifdef ET_IP_MODBUS_SERIAL
	}
#endif
}

#endif /* ET_IP_MODBUS */
