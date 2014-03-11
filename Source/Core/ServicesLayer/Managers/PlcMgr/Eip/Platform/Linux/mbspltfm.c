/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSPLTFM.C
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Modbus Platform dependant code for Win32.
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
**
** Porting: <none>----0<major>         Customization: <none>----0<major>
**
*****************************************************************************
*****************************************************************************
*/

#include "../../eipinc.h"

/************************* Private variables *******************************/

static FILE        *fLogFile;

/*---------------------------------------------------------------------------
** mbus_platformInitLogFile( )
**
** Creates a trace dump file.
**---------------------------------------------------------------------------
*/

void mbus_platformInitLogFile(void)
{
	char szFileName[128];
	FILE *tmp;
	int i;

	/* Find the first unused sequential number for creating debug file name */

	for( i = 0; i < 1000; i++ )
	{
		sprintf( szFileName, "./mbsdebug%d.txt", i);
		tmp = fopen( szFileName, "r" );
		if ( !tmp )	/* File does not exist - break and use the name */
			break;
		fclose(tmp);
	}

	/* Open the file */
	fLogFile = fopen( szFileName, "w" );
}

/*---------------------------------------------------------------------------
** mbus_platformCloseLogFile()
**
** Closes the log file
**---------------------------------------------------------------------------
*/

void mbus_platformCloseLogFile(void)
{
    fclose(fLogFile);
}

/*---------------------------------------------------------------------------
** platformWriteLog()
**
** Writes a record to the log file
**---------------------------------------------------------------------------
*/

void mbus_platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp)
{
#if defined(MBUS_TRACE_FILE_OUTPUT)
    static char szBuffer[(MAX_PACKET_SIZE*3)+1000];
	static char szTimeStamp[64];
	static char szSrcIp[20], szDstIp[20];

    struct timeb timebuffer;
    char *szTimeline;

	/* always use timestamp */
	bTimeStamp = bTimeStamp;
	/* iSeverity not included in log statement */
	iSeverity = iSeverity;
    
#ifdef MULTIPLE_LOG_FILES
	static UINT32 lLogCount = 0;
#endif

    ftime( &timebuffer );
    szTimeline = ctime( & ( timebuffer.time ) );
	sprintf(szTimeStamp, "%.19s.%hu %u ", szTimeline, timebuffer.millitm, gdwTickCount);

	if ((lSrcIp != 0) || (lDstIp != 0))
	{
		strcpy(szSrcIp, socketGetAddressString(lSrcIp));
		strcpy(szDstIp, socketGetAddressString(lDstIp));
		sprintf(szBuffer, "%c %s (%s -> %s) %s\r\n", traceGetTypeAsChar(iType), szTimeStamp, szSrcIp, szDstIp, szBuf);
	}
	else
		sprintf(szBuffer, "%c %s %s\r\n", traceGetTypeAsChar(iType), szTimeStamp, szBuf);

	fputs(szBuffer, fLogFile);
	fflush( fLogFile );

#endif /* #if defined(MBUS_TRACE_FILE_OUTPUT) */
}
