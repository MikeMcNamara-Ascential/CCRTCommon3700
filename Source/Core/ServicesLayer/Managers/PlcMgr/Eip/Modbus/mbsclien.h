/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPMBUS.H
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Modbus Library main processing task.
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

#ifndef EIPCLIEN_H
#define EIPCLIEN_H

#ifndef ERROR_STATUS
#define ERROR_STATUS    (-1)        /* Return in case of a failure */
#endif
#define MBUS_DEFAULT_REQUEST_TIMEOUT	2000	/* Default time out of 2 seconds */

typedef void MBS_EventCallbackType( INT32 nEvent, INT32 nParam1, INT32 nParam2 );    /* Callback function to be called in case of any notification or error event */

extern UINT32 gmbusTickCount;							/* Stored system tick count in msecs */
extern PLATFORM_MUTEX_TYPE     ghmbusClientMutex;		/* Used to protect integrity of the client calls */
extern MBSCounters	gmbus_counters;						/* Tracks messages and error conditions */

/*---------------------------------------------------------------------------
** modbusInit( )
**
** Initialize Modbus funtionality
**---------------------------------------------------------------------------
*/
extern BOOL modbusInit(MBS_EventCallbackType* pCallback);

/*---------------------------------------------------------------------------
** modbusRelease( )
**
** Stop Modbus functionality
**---------------------------------------------------------------------------
*/
extern void modbusRelease(void);


/*---------------------------------------------------------------------------
** modbusService( )
**
** Executes one iteration of the Modbus Protocol stack.
** Function called at repeated intervals by the EtherNet/IP stack
** to provide the Modbus stack with a time slice in which to 
** execute.
**---------------------------------------------------------------------------
*/
extern void modbusService(UINT32 lTickCount);

/*---------------------------------------------------------------------------
** modbusClaimIPAddress( )
**
** Claim one of the host IP addresses determined by EtherNet/IP stack.
** Returned TRUE if claimed successfully
**---------------------------------------------------------------------------
*/
extern BOOL modbusClaimIPAddress(UINT32 lIPAddress);

/*---------------------------------------------------------------------------
** modbusUnclaimIPAddress( )
**
** Unclaim one of the host IP addresses determined by EtherNet/IP stack.
** Returned TRUE if claimed successfully
**---------------------------------------------------------------------------
*/
extern BOOL modbusUnclaimIPAddress(UINT32 lIPAddress);

/*---------------------------------------------------------------------------
** modbusUpdateClaimIPAddress( )
**
** Update a claimed IP address.  Called when IP address of device is updated.
**---------------------------------------------------------------------------
*/
extern void modbusUpdateClaimIPAddress(UINT32 lIPOldAddress, UINT32 lIPNewAddress);

/*---------------------------------------------------------------------------
** modbusGetCOMPorts( )
**
** Returns an array of the COM port numbers
** alCOMPorts should point to the preallocated unsigned array 
** where COM ports should be stored.
** Returns the number of the obtained COM ports
**---------------------------------------------------------------------------
*/
extern INT32 modbusGetCOMPorts(UINT32* alCOMPorts);

/*---------------------------------------------------------------------------
** modbusClaimCOMPort( )
**
** Claim one of the COM ports returned by modbusGetCOMPorts().
** Will be called repeatedly for each claimed COM port.
** Returned TRUE if claimed successfully, FALSE if the COM port is not in 
** the host list.
**---------------------------------------------------------------------------
*/
extern INT32 modbusClaimCOMPort(UINT32 lCOMPort);

/*---------------------------------------------------------------------------
** modbusGetClaimedCOMPorts( )
**
** Returns an array with the claimed COM ports.
** alCOMPorts should point to the preallocated unsigned array where the 
** claimed COM ports will be stored.
** Returns the number of the claimed COM ports
**---------------------------------------------------------------------------
*/
extern INT32 modbusGetClaimedCOMPorts(UINT32* alCOMPorts);

/*---------------------------------------------------------------------------
** modbusClaimAllCOMPorts( )
**
** Claims the ownership of all available COM ports.
**---------------------------------------------------------------------------
*/
extern void modbusClaimAllCOMPorts();

/*---------------------------------------------------------------------------
** modbusSendRequest( )
**
** Send Modbus message
** modbusGetResponse will be used to get the response.
** pRequest points to a structure containing the request parameters.
** bIO determines if its an I/O data request.  I/O data requests will be sent
** on a separate TCP connection (if available)
** Check the return value. If it's greater than or equal to MBUS_REQUEST_INDEX_BASE,
** then it is a request Id to be used when calling modbusGetResponse.
** If it is less than REQUEST_INDEX_BASE then it's one of the 
** errors listed above.
**---------------------------------------------------------------------------
*/
extern INT32 modbusSendRequest( MBSRequest* pRequest, BOOL bIO);

/*---------------------------------------------------------------------------
** modbusGetResponse( )
**
** Get a response for the previously transferred Modbus message.
** nRequestId is a request Id returned from the previous call to the  
** modbusSendRequest function. 
** pResponse contain response data in case of a success and
** error indormation in case of an error.
** Returns 0 if able to get the response or one of the error constants 
** listed above. If return value is 0 it does not guarantee the Modbus
** response. You need to check bGeneralStatus member of pResponse to 
** determine whether the target returned success or an error. bGeneralStatus
** of 0 indicates a success. 
**---------------------------------------------------------------------------
*/
extern INT32 modbusGetResponse(INT32 nRequestId, MBSResponse* pResponse);


/*---------------------------------------------------------------------------
** modbusIsSessionActive( )
**
** Determine if there has been recent communication to a particular TCP device
**---------------------------------------------------------------------------
*/
extern BOOL modbusIsSessionActive(UINT32 lIPAddress, UINT32 lHostIPAddress);

/*---------------------------------------------------------------------------
** modbusIsSerialSessionActive( )
**
** Determine if there has been recent communication to a particular serial device
**---------------------------------------------------------------------------
*/
extern BOOL modbusIsSerialSessionActive(UINT32 lAddress, UINT32 lComPort);

#endif
