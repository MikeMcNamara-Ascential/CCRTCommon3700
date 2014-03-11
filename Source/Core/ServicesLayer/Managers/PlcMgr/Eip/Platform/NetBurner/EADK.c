#include "predef.h" 
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <taskmon.h>
#include <ucos.h>
#include <AutoUpdate.h>

#include "..\..\eipinc.h"

/* Configuration of the target for Class 1 Connections */

#define CUSTOM_SHORT_IO_SERVICE	0x7f
#define SHORT_IO_ON				1
#define SHORT_IO_OFF			0

#define ASSEMBLY_INPUT_INSTANCE				100
#define ASSEMBLY_OUTPUT_INSTANCE			101
#define ASSEMBLY_CONFIGURATION_INSTANCE		102
#define ASSEMBLY_INSTANCE_DATA_SIZE			200
#define ASSEMBLY_INSTANCE_CONFIG_SIZE		4

/* Application global variables */

BOOL	gbShortIO = FALSE;

void fnCallback( int nEvent, int nParam1, int nParam2 );
void updateOutput(void);

const char * AppName="EIPScanner";
char gbShortIOFlag;

//Make sure they're 4 byte aligned to keep the ColdFire happy
DWORD   EIPTaskStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD   TCPConnectTaskStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD   UdpTestStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD   ModbusTCPConnectTaskStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD   BroadcastTestStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD   MulticastTestStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

void updateInput ( INT32 nParam )
{
	UINT8 buf[ASSEMBLY_INSTANCE_DATA_SIZE];
	int nDataSize = clientGetAssemblyInstanceData(nParam, buf, ASSEMBLY_INSTANCE_DATA_SIZE);
	iprintf("New input data received for connection with instance %d, data size %d\n", nParam, nDataSize);

	if (gbShortIOFlag)
	{
		clientSetAssemblyInstanceData(2, buf, ASSEMBLY_INSTANCE_DATA_SIZE);
	}
}

void updateProducingData(INT32 nParam1)
{
	INT32	lDataSize;
	UINT8	abBuf[ASSEMBLY_INSTANCE_DATA_SIZE];
	
	lDataSize = clientGetAssemblyInstanceData(nParam1, abBuf, ASSEMBLY_INSTANCE_DATA_SIZE );
	
	clientSetAssemblyInstanceData( nParam1+1, abBuf, lDataSize );
}

void establishConnection ( INT32 nParam )
{
	EtIPConnectionConfig cfg;
	int nStatus = clientGetConnectionConfig( nParam, &cfg );	

	if ( !nStatus )	/* 0 indicates success */
	{
		UINT8 buf[ASSEMBLY_INSTANCE_DATA_SIZE];
		memset(buf, 0, ASSEMBLY_INSTANCE_DATA_SIZE);
		clientSetAssemblyInstanceData(cfg.iTargetProducingConnPoint, buf, ASSEMBLY_INSTANCE_DATA_SIZE);
	}
}

/****************************************************************************
* FUNCTION:		fnCallback
* DESCRIPTION:	A callback function for use by the stack to notify us of 
*				various events
* INPUTS:		N/A
* RETURNS:		N/A
****************************************************************************/

void fnCallback ( INT32 nEvent, INT32 nParam1, INT32 nParam2 )
{
	/* If new I/O data has been received on the Class1 connection, get it from the stack */
    switch ( nEvent )
	{
		case NM_REQUEST_RESPONSE_RECEIVED:
		{
			EtIPObjectResponse Response;
			int nStatus = clientGetUnconnectedResponse( nParam1, &Response );
			iprintf("Response for request %d received with Status %d, Error '%s', Data Size %d\n",
				nParam1, Response.bGeneralStatus, Response.errorDescription, Response.iDataSize );
		}
		break;
		case NM_ASSEMBLY_NEW_INSTANCE_DATA:
			updateInput( nParam1 );
			break;
		case NM_REQUEST_FAILED_INVALID_NETWORK_PATH:
			iprintf("Request %d failed - invalid network path\n");
			break;

		case NM_REQUEST_TIMED_OUT:
			iprintf("Request %d failed - timed out waiting for response\n");
			break;

		case NM_CONNECTION_ESTABLISHED:
			iprintf("Connection with instance %d successfully opened\n", nParam1);
			establishConnection(nParam1);
			break;

		case NM_CONN_CONFIG_FAILED_INVALID_NETWORK_PATH:
			iprintf("Connection with instance %d failed - invalid network path\n", nParam1);
			break;

		case NM_CONN_CONFIG_FAILED_NO_RESPONSE:
			iprintf("Connection with instance %d failed - no response\n", nParam1);
			break;

		case NM_CONNECTION_TIMED_OUT:
			iprintf("Connection with instance %d timed out\n", nParam1);
			break;

		case NM_CONNECTION_CLOSED:
			iprintf("Connection with instance %d closed\n", nParam1);
			break;

		case NM_CLIENT_OBJECT_REQUEST_RECEIVED:
			{
				EtIPObjectRequest Request;
				EtIPObjectResponse Response;

				int nStatus = clientGetClientRequest( nParam1, &Request );
				BOOL ProcessMsg = FALSE;
				
				Response.bGeneralStatus = 0;
				Response.iExtendedStatus = 0;
				Response.iDataSize = 0;

				switch( Request.bService )
				{
					default:
						Response.bGeneralStatus = ROUTER_ERROR_INVALID_DESTINATION;
						break;
				}

				if (!ProcessMsg)
				{
					switch( Request.iClass )
					{
						default:
							Response.bGeneralStatus = ROUTER_ERROR_INVALID_DESTINATION;
					}
				}

				nStatus = clientSendClientResponse( nParam1, &Response );
				iprintf("Client Response %d sent with Status %d\n", nParam1, nStatus);
			}
			break;

		default:
			break;
	}
}

//The stack task calling main stack task every tick
void EIPTask( void *pd )
{    	
    //int n = 1;
    
	while (1)
    {
      OSTimeDly(1);
      
      clientMainTask();      
            
      //if ( n++ % 50 == 0 )
        //  iprintf("TimeTick %d Secs %d\n", TimeTick, Secs);

    }
}


extern "C" void UserMain(void * pd)
{
    InitializeStack();
    if (EthernetIP==0)
        GetDHCPAddress(); 
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();
    EnableTaskMonitor();
 
    /* Register the callback function. */
    clientRegisterEventCallBack(&fnCallback );
 
    /* startup the scanner */
    clientStart();

    clientClaimAllHostIPAddress();
 
    OSTaskCreate( EIPTask,
                ( void * )0,
                 &EIPTaskStk[USER_TASK_STK_SIZE],
                 EIPTaskStk,
                 MAIN_PRIO - 6 );
 
    OSTaskCreate( TCPConnectTask,
                ( void * )0,
                 &TCPConnectTaskStk[USER_TASK_STK_SIZE],
                 TCPConnectTaskStk,
                 MAIN_PRIO - 5 );

    OSTaskCreate( UdpReadTask,
                ( void * )0,
                 &UdpTestStk[USER_TASK_STK_SIZE],
                 UdpTestStk,
                 MAIN_PRIO - 4 );

#ifdef ET_IP_MODBUS
    OSTaskCreate( ModbusTCPConnectTask,
                ( void * )0,
                 &ModbusTCPConnectTaskStk[USER_TASK_STK_SIZE],
                 ModbusTCPConnectTaskStk,
                 MAIN_PRIO - 3 );
#endif
	
    OSTaskCreate( BroadcastReadTask,
                ( void * )0,
                 &BroadcastTestStk[USER_TASK_STK_SIZE],
                 BroadcastTestStk,
                 MAIN_PRIO - 2 );

	clientAddAssemblyInstance(ASSEMBLY_OUTPUT_INSTANCE, AssemblyStatic | AssemblyProducing, INVALID_OFFSET, ASSEMBLY_INSTANCE_DATA_SIZE, NULL);
	clientAddAssemblyInstance(ASSEMBLY_INPUT_INSTANCE, AssemblyStatic | AssemblyConsuming, INVALID_OFFSET, ASSEMBLY_INSTANCE_DATA_SIZE, NULL);
	clientAddAssemblyInstance(ASSEMBLY_CONFIGURATION_INSTANCE, AssemblyStatic | AssemblyConfiguration, INVALID_OFFSET, ASSEMBLY_INSTANCE_CONFIG_SIZE, NULL);

    while (1)
      OSTimeDly(1);  
}
