/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSTRACE.C
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Outputting trace dump to the disk file or the debugger window for Modbus
**
** To use trace functionality there should be two preprocessor definitions
** set: MBUS_TRACE_OUTPUT enabling general trace support and either
** MBUS_TRACE_FILE_OUTPUT to send it to the file or MBUS_TRACE_DEBUGGER_OUTPUT
** to output to th edebug window. 
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

#include "mbsinc.h"

#ifdef MBUS_TRACE_OUTPUT

#if defined(MBUS_TRACE_FILE_OUTPUT) || defined (MBUS_TRACE_DEBUGGER_OUTPUT)
UINT8 gmbusTraceFlag;
UINT16 gmbusTraceTypeFlag;
#endif

#if defined(MBUS_TRACE_FILE_OUTPUT) || defined (MBUS_TRACE_DEBUGGER_OUTPUT) || defined (MBUS_TRACE_SOCKET_OUTPUT)

/*---------------------------------------------------------------------------
** mbusfDumpStr( )
**
** Dumps the string to the file or the debugger window. Accepts variable
** number of arguments. Arguments are passed using printf convention:
** %d in the format string indicates decimal integer argument, %x - hex
** integer, %s - a string, and so on.
**---------------------------------------------------------------------------
*/

void mbusfDumpStr(UINT8 iSeverity, UINT16 iType, UINT32 lHostIpAddr, UINT32 lRemoteIpAddr, const char* lpszFormat, ...)
{    
    char szBuf[MBUS_TRACE_BUFFER_SIZE];
    va_list args;

    /* Build the output string using printf convention */
    va_start(args, lpszFormat);    
    vsprintf(szBuf, lpszFormat, args);
    va_end(args);

#ifdef MBUS_TRACE_SOCKET_OUTPUT
    mbus_dbgWriteLogString(iSeverity, iType, lHostIpAddr, lRemoteIpAddr, szBuf);
#endif

#if defined(MBUS_TRACE_FILE_OUTPUT) || defined (MBUS_TRACE_DEBUGGER_OUTPUT)
	if ((iSeverity & gmbusTraceFlag) || (iType & gmbusTraceTypeFlag))
		mbus_platformWriteLog(iSeverity, iType, lHostIpAddr, lRemoteIpAddr, szBuf, TRUE);
#endif
}

/*---------------------------------------------------------------------------
** mbusfDumpBuf( )
**
** Dumps the buffer contents to the file or the debugger window. 
** Output the buffer as a string of bytes presented in the hexadecimal
** format and separated by a space.
**---------------------------------------------------------------------------
*/

void mbusfDumpBuf(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char* pBuf, INT32 nLen)
{
#if defined(MBUS_TRACE_FILE_OUTPUT) || defined (MBUS_TRACE_DEBUGGER_OUTPUT)
    static char szBuffer[MBUS_MAX_PACKET_SIZE*3];    /* Each byte takes 2 chars plus spaces in between bytes */
    char szTemp[16];
    INT32 i;
    INT32 nByte;
    char *c = szBuffer;

    if ( nLen <= 0 || nLen > MBUS_MAX_PACKET_SIZE || (!((iSeverity & gmbusTraceFlag) || (iType & gmbusTraceTypeFlag))))
        return;
    
    for( i = 0; i < nLen; i++ )
    {
        nByte = (INT32)(UINT8)pBuf[i];    /* Take each byte one by one */
        sprintf(szTemp, "%02x ", nByte);        /* Convert it to the Hex string presentation */
        *c++ = szTemp[0];                            /* Incorporate in the big string */
        *c++ = szTemp[1];
        *c++ = ' ';
    }    
    
    *c++ = 0;

    mbus_platformWriteLog(iSeverity, iType, lSrcIp, lDstIp, szBuffer, FALSE);
#endif /* #ifdef MBUS_TRACE_FILE_OUTPUT || MBUS_TRACE_DEBUGGER_OUTPUT */

#ifdef MBUS_TRACE_SOCKET_OUTPUT
    mbus_dbgWriteLogData(iSeverity, iType, lSrcIp, lDstIp, pBuf, nLen);
#endif /* MBUS_TRACE_SOCKET_OUTPUT*/ 

}

#endif /* #ifdef MBUS_TRACE_FILE_OUTPUT || MBUS_TRACE_DEBUGGER_OUTPUT || MBUS_TRACE_SOCKET_OUTPUT */

char mbusGetTypeAsChar(UINT16 iType)
{
	char cType = '?';

	switch(iType)
	{
	case  MBUS_TRACE_TYPE_DATA_IN:
		cType = '<';
		break;
	case MBUS_TRACE_TYPE_DATA_OUT:
		cType = '>';
		break;
	case MBUS_TRACE_TYPE_MSG:
		cType = '*';
		break;
	case MBUS_TRACE_TYPE_SESSION:
		cType = 'S';
		break;
	case MBUS_TRACE_TYPE_MODBUS:
		cType = 'M';
		break;
	}

	return cType;
}

#endif /* MBUS_TRACE_OUTPUT */
