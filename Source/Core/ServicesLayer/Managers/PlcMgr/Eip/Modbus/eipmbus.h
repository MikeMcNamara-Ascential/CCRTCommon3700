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

#ifndef EIPMBUS_H
#define EIPMBUS_H

#define MODBUS_CLASS			0x44

#define MODBUS_VENDOR_ID		65534
#define MODBUS_PRODUCT_TYPE		0x28
#define MODBUS_PRODUCT_CODE		0
#define MAX_MODBUS_ASSEMBLY_INSTANCE		0x40000

#define MODBUS_CLASS_REVISION          1        /* Modbus class revision */

#define MODBUS_CLASS_ATTR_REVISION			1	/* Revision                            */
#define MODBUS_CLASS_ATTR_MAX_INSTANCE		2	/* Max instance                        */
#define MODBUS_CLASS_ATTR_NUM_INSTANCES		3	/* Number of instances                 */
#define MODBUS_CLASS_ATTR_MAX_CLASS_ATTR	6	/* Max class attribute number implemented   */
#define MODBUS_CLASS_ATTR_MAX_INST_ATTR		7	/* Max instance attribute number implemented   */

#define MODBUS_SVC_READ_DISCRETE_INPUTS		0x4B
#define MODBUS_SVC_READ_COILS				0x4C
#define MODBUS_SVC_READ_INPUT_REGISTERS		0x4D
#define MODBUS_SVC_READ_HOLDING_REGISTERS	0x4E
#define MODBUS_SVC_WRITE_COILS				0x4F
#define MODBUS_SVC_WRITE_HOLDING_REGISTERS	0x50
#define MODBUS_SVC_EXECUTE_MODBUS			0x51

#define MAX_MODBUS_WRITE_SIZE				246	/* Max number of bytes that can written for Modbus data*/
#define MAX_MODBUS_READ_SIZE				250 /* Max number of bytes that can read for Modbus data*/

#define MODBUS_HEARTBEAT_CONN_POINT			0x50000
#define MODBUS_LISTEN_ONLY_CONN_POINT		0x50001

#define HOLDING_REGISTER_START				0x00001
#define HOLDING_REGISTER_END				0x10000
#define INPUT_REGISTER_START				0x10001
#define INPUT_REGISTER_END					0x20000
#define COIL_START							0x20001
#define COIL_END							0x30000
#define DISCRETE_INPUT_START				0x30001
#define DISCRETE_INPUT_END					0x40000


#define UNKNOWN_MODBUS_DEVICE		"Unknown Modbus Device"

/* Class attribute structure */
typedef struct tagMODBUS_CLASS_ATTR
{
   UINT16  iRevision;
   UINT16  iMaxInstance;
   UINT16  iNumInstances;
   UINT16  iMaxClassAttr;
   UINT16  iMaxInstanceAttr;
}
MODBUS_CLASS_ATTR;

/* The EtherNet/IP stack doesn't natively support the Parameter Object, so
 * it's values will be stored here */
#define PARAMETER_CLASS			0x0F

#define PARAMETER_CLASS_REVISION        1		/* Parameter class revision */

#define PARAMETER_CLASS_ATTR_REVISION	1		/* Revision                 */

#define PARAMETER_ATTR_VALUE            1		/* Parameter Value */


extern void eipModbusCallbackFunction( INT32 nEvent, INT32 nParam1, INT32 nParam2 );

extern void eipmbsParseCIPModbusRequest( REQUEST* pRequest );

#endif
