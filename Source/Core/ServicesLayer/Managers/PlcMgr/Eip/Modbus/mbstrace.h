/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** TRACE.H
**
** COPYRIGHT (c) 2001-2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Outputting trace dump to the disk file or the debugger window
**
** To use trace functionality there should be two preprocessor definitions
** set: MBUS_TRACE_OUTPUT enabling general trace support and either
** TRACE_FILE_OUTPUT to send it to the file or TRACE_DEBUGGER_OUTPUT
** to output to the debug window. 
** In case of the file dump the program will output to the debug%n.txt file
** in the et_ip directory (or root directory for WinCE) on the c: drive.
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

#ifndef MBSTRACE_H
#define MBSTRACE_H

#ifdef MBUS_TRACE_OUTPUT

#define MBUS_TRACE_TYPE_DATA_IN			0x0100
#define MBUS_TRACE_TYPE_DATA_OUT		0x0200
#define MBUS_TRACE_TYPE_DATA_FILTER		0xFF00

#define MBUS_TRACE_TYPE_MSG				0x0001
#define MBUS_TRACE_TYPE_SESSION			0x0002
#define MBUS_TRACE_TYPE_MODBUS			0x0004

#define MBUS_TRACE_TYPE_ALL				0xFFFF
#define MBUS_TRACE_TYPE_NONE			0


#define MBUS_TRACE_LEVEL_ERROR			0x01
#define MBUS_TRACE_LEVEL_WARNING		0x02
#define MBUS_TRACE_LEVEL_NOTICE			0x04
#define MBUS_TRACE_LEVEL_DATA			0x08
#define MBUS_TRACE_LEVEL_ALL			0x0f
#define MBUS_TRACE_LEVEL_NONE			0x00

#define MBUS_TRACE_BUFFER_SIZE			1024


#if defined(MBUS_TRACE_FILE_OUTPUT) || defined (MBUS_TRACE_DEBUGGER_OUTPUT)
extern UINT8 gmbusTraceFlag;
extern UINT16 gmbusTraceTypeFlag;
#endif

extern void mbusfDumpStr(UINT8 iSeverity, UINT16 iType, UINT32 lHostIpAddr, UINT32 lRemoteIpAddr, const char* lpszFormat, ...);    /* Variable number of parameters supported */
extern void mbusfDumpBuf(UINT8 iSeverity, UINT16 iType, UINT32 lHostIpAddr, UINT32 lRemoteIpAddr, char* pBuf, INT32 nLen);        /* Output the buffer contents as a string of bytes */

#define mbusDumpStr0(severity, x, y, z, a)    mbusfDumpStr(severity, x, y, z, a)
#define mbusDumpStr1(severity, x, y, z, a, b)    mbusfDumpStr(severity, x, y, z, a, b)
#define mbusDumpStr2(severity, x, y, z, a, b, c)    mbusfDumpStr(severity, x, y, z, a, b, c)
#define mbusDumpStr3(severity, x, y, z, a, b, c, d)    mbusfDumpStr(severity, x, y, z, a, b, c, d)
#define mbusDumpStr4(severity, x, y, z, a, b, c, d, e)    mbusfDumpStr(severity, x, y, z, a, b, c, d, e)
#define mbusDumpStr5(severity, x, y, z, a, b, c, d, e, f)    mbusfDumpStr(severity, x, y, z, a, b, c, d, e, f)
#define mbusDumpBuf(severity, a, b, c, d, e)    mbusfDumpBuf(severity, a, b, c, d, e)
#else        /* If MBUS_TRACE_OUTPUT is not defined replace mbusDump* statements with nothing */
#define mbusDumpStr0(severity, x, y, z, a)    
#define mbusDumpStr1(severity, x, y, z, a, b)    
#define mbusDumpStr2(severity, x, y, z, a, b, c)    
#define mbusDumpStr3(severity, x, y, z, a, b, c, d)    
#define mbusDumpStr4(severity, x, y, z, a, b, c, d, e)    
#define mbusDumpStr5(severity, x, y, z, a, b, c, d, e, f)    
#define mbusDumpBuf(severity, a, b, c, d, e)    
#endif

extern char mbusGetTypeAsChar(UINT16 iType);

#endif
