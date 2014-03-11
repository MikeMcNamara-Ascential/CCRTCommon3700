/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** mbsApi.h
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Modbus Stack API structures and #defines. 
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

#ifndef MBS_API_H
#define MBS_API_H

#define MBUS_MAX_PACKET_SIZE			260			/* Max packet length */
#define MBUS_MAX_NETWORK_PATH_SIZE		256			/* Maximum size of the network path string */
#define MBUS_MAX_HOST_ADDRESSES			16        /* Maximum number of host adapters "IP Addresses" installed on a device */
#define MBUS_BROADCAST_ADDR				0
#define MBUS_MAX_PDU_LEN				250
#define MBUS_DEFAULT_UNIT_ID			0xFF

/* Modbus function codes */
#define MBUS_READ_COIL_STATUS					1
#define MBUS_READ_INPUT_STATUS					2
#define MBUS_READ_HOLDING_REGISTERS				3
#define MBUS_READ_INPUT_REGISTERS				4
#define MBUS_WRITE_SINGLE_COIL					5
#define MBUS_WRITE_SINGLE_REGISTER				6
#define MBUS_READ_EXCEPTION_STATUS				7
#define MBUS_DIAGNOSTICS						8
#define MBUS_WRITE_MULTIPLE_COILS				15
#define MBUS_WRITE_MULTIPLE_REGISTERS			16
#define MBUS_REPORT_SLAVE_ID					17
#define MBUS_READ_WRITE_MULTIPLE_REGISTERS		23
#define MBUS_READ_DEVICE_INFORMATION			43

/* Modbus error codes */
#define MBUS_EXCEPTION_ILLEGAL_FUNCTION					0x01
#define MBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS				0x02
#define MBUS_EXCEPTION_ILLEGAL_DATA_VALUE				0x03
#define MBUS_EXCEPTION_SLAVE_DEVICE_FAILURE				0x04
#define MBUS_EXCEPTION_ACKNOWLEDGE						0x05
#define MBUS_EXCEPTION_SLAVE_DEVICE_BUSY				0x06
#define MBUS_EXCEPTION_MEMORY_PARITY_ERROR				0x08
#define MBUS_EXCEPTION_GATEWAY_PATH_UNAVAILABLE			0x0A
#define MBUS_EXCEPTION_GATEWAY_TARGET_FAILED_RESPONSE	0x0B


/* Notification messages. Second parameter in the callback function will be the Request ID */
#define NM_MBUS_REQUEST_RESPONSE_RECEIVED				100		/* Response received on the previously sent request */
#define NM_MBUS_REQUEST_FAILED_INVALID_NETWORK_PATH		101		/* Unable to connect to the path specified in the request */
#define NM_MBUS_REQUEST_TIMED_OUT						102		/* Request timed out - response never received. */

#define NM_MBUS_ERROR_OUT_OF_MEMORY						200		/* Out of available memory. */
#define NM_MBUS_PENDING_REQUESTS_LIMIT_REACHED			201		/* Number of outstanding object requests exceeded MAX_REQUESTS limit */
#define NM_MBUS_SESSION_COUNT_LIMIT_REACHED				202		/* Number of network peers exceeded MAX_SESSIONS constant */
#define NM_MBUS_ERROR_USING_SOCKETS						203		/* Received an error when using sockets library functions */
#define NM_MBUS_ERROR_INVALID_HOST						204		/* Host IP address or COM port isn't valid */

/* Following error codes may be returned to one of the function calls. They will not be used when invoking the callback function */
#define ERR_MBUS_REQUEST_UNKNOWN_INDEX              300   /* Unknown index specified when trying to get a response */
#define ERR_MBUS_NO_AVAILABLE_RESPONSE              301   /* The response on the request sent has not been received yet */
#define ERR_MBUS_INVALID_REQUESTTYPE                302   /* The request doesn't have a valid type from MBSRequestType */

typedef enum MBSRequestType
{
	MBUS_REQUESTTYPE_TCP		= 1,	/* Request is ModbusTCP */
	MBUS_REQUESTTYPE_SERIAL		= 2		/* Request is Modbus Serial */    
} MBSRequestType;


typedef struct tagMBSRequest                /* Structure used to transfer Modbus message */
{
	INT8    networkPath[MBUS_MAX_NETWORK_PATH_SIZE];/* Null terminated peer network path string for ModbusTCP requests. */
	UINT8   bFunction;								/* Function code */
	UINT8   requestData[MBUS_MAX_PACKET_SIZE];		/* Data that should be sent to the target. If not used iDataSize must be 0. */
	UINT16	iDataSize;								/* The actual size of the requestData */
    UINT32  lMessageTimeout;						/* Message Timeout in milliseconds. If 0, MBUS_DEFAULT_REQUEST_TIMEOUT will be used instead. */
	UINT32  lHostAddress;							/* Host IP Address in UINT32 format or COM port number for the opened session */
	UINT8	bModbusMsgType;							/* Send message on ModbusTCP or Modbus SL */
	UINT8	bUnitID;								/* Unit ID for messages */
}
MBSRequest;

typedef struct tagMBSResponse               /* Structure used to get the response to a Modbus message */
{
	UINT8   bGeneralStatus;                        /* 0 in case of successful response, or one of the Modbus general codes. */
	UINT8   responseData[MBUS_MAX_PACKET_SIZE];   /* Response Data in case of a success. */
	UINT16  iDataSize;                             /* The size of the responseData in bytes */
}
MBSResponse;

typedef struct tagMBSCounters
{
		// comm lib status
	UINT16 usDiagRegister;
	UINT8  ucExceptionRegister;
	UINT16 usBusMsgCounter;
	UINT16 usBusErrorCounter;
	UINT16 usSlaveMsgCounter;
	UINT16 usExceptionRespCounter;
	UINT16 usNoResponseCounter;
	UINT16 usRxOverrunCounter;
}
MBSCounters;

/* The following macros are required for certain types of processors (i.e. ARM).
   They replace word and double word addressing with individual byte handling */

/* Please note that we assume that over the wire the data always is transferred
   in big-endian format, even if target and/or originator are using little-endian */

#ifndef BE_HIBYTE
#define BE_HIBYTE(w)           ((UINT8)(w))
#endif
#ifndef BE_LOBYTE
#define BE_LOBYTE(w)           ((UINT8)(((UINT16)(w) >> 8) & 0xFF))
#endif
#ifndef BE_HIWORD
#define BE_HIWORD(l)           ((UINT16)(l))
#endif
#ifndef BE_LOWORD
#define BE_LOWORD(l)           ((UINT16)(((UINT32)(l) >> 16) & 0xFFFF))
#endif
#ifndef BE_MAKEWORD
#define BE_MAKEWORD(a, b)      ((UINT16)((((UINT8)(b)) & 0xff) | ((UINT16)((((UINT8)(a)) << 8) & 0xff00))))
#endif
#ifndef BE_MAKELONG                            
#define BE_MAKELONG(a, b)      ((INT32)((((UINT16)(b)) & 0xffff) | ((UINT32)((((UINT16)(a)) << 16) & 0xffff0000))))
#endif

#define UINT32_BE_SET( ptr, value )  { *(UINT8*)(ptr) = BE_LOBYTE(BE_LOWORD((UINT32)value));  \
                    *(UINT8*)(((UINT8*)(ptr))+1) = BE_HIBYTE(BE_LOWORD((UINT32)value));  \
                    *(UINT8*)(((UINT8*)(ptr))+2) = BE_LOBYTE(BE_HIWORD((UINT32)value));  \
                    *(UINT8*)(((UINT8*)(ptr))+3) = BE_HIBYTE(BE_HIWORD((UINT32)value));  }

#define UINT32_BE_GET( ptr ) (UINT32) (BE_MAKELONG(BE_MAKEWORD((*(UINT8*)(ptr)),(*(UINT8*)(ptr+1))),BE_MAKEWORD((*(UINT8*)(ptr+2)),(*(UINT8*)(ptr+3)))))

#define UINT16_BE_SET( ptr, value )  { *(UINT8*)(ptr) = BE_LOBYTE((UINT16)value);  \
                    *(UINT8*)(((UINT8*)(ptr))+1) = BE_HIBYTE((UINT16)value); } 

#define UINT16_BE_GET( ptr )  (UINT16) (BE_MAKEWORD((*(UINT8*)(ptr)),(*(UINT8*)(ptr+1))))



#ifdef EIP_BIG_ENDIAN

#define _BE_SWAPL_(a)              (a)
#define _BE_SWAPS_(a)              (a)

#else    /* #ifdef EIP_BIG_ENDIAN */

#define _BE_SWAPL_(a)              (((a) << 24) & 0xff000000) | (((a) << 8) & 0xff0000) | \
                                (((a) >> 8) & 0xff00) | ((((UINT32) (a)) >> 24) & 0xff)

#define _BE_SWAPS_(a)              (((a & 0xff) << 8) | (((a) >> 8)) & 0xff)


#endif    /* #ifdef EIP_BIG_ENDIAN */


#endif /* MBS_API_H */
