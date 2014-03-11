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
** Modbus Platform dependant code for Win CE
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

/* define this if you want logging broken up into multiple, smaller log files */
/* #define MBUS_MULTIPLE_LOG_FILES */
/* number of log file entries before a new file is created */
#define LOG_FILE_ENTRIES	200000

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
    FILE* tmp;
    char szFileName[1000+128];
    int i;

     /* Find the first unused sequential number for creating debug file name */
    for( i = 0; i < 1000; i++ )
    {
        sprintf( szFileName, "mbsdebug%d.txt", i);    
        tmp = fopen( szFileName, "r" );
        if ( !tmp )    /* File does not exist - break and use the name */
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
#if defined(MBUS_TRACE_FILE_OUTPUT) || defined (MBUS_TRACE_DEBUGGER_OUTPUT)

    static char szBuffer[(MBUS_MAX_PACKET_SIZE*3)+1000];
	static char szTimeStamp[64];
	static char szSrcIp[20], szDstIp[20];

    struct _timeb timebuffer;
    char *szTimeline;

	/* always use timestamp */
	bTimeStamp = bTimeStamp;
	/* iSeverity not included in log statement */
	iSeverity = iSeverity;
    
#ifdef MBUS_MULTIPLE_LOG_FILES
	static UINT32 lLogCount = 0;
#endif

    _ftime( &timebuffer );
    szTimeline = ctime( & ( timebuffer.time ) );
	sprintf(szTimeStamp, "%.19s.%hu %u ", szTimeline, timebuffer.millitm, gmbusTickCount);

	if ((lSrcIp != 0) || (lDstIp != 0))
	{
		strcpy(szSrcIp, socketGetAddressString(lSrcIp));
		strcpy(szDstIp, socketGetAddressString(lDstIp));
		sprintf(szBuffer, "%c %s (%s -> %s) %s\n", mbusGetTypeAsChar(iType), szTimeStamp, szSrcIp, szDstIp, szBuf);
	}
	else
		sprintf(szBuffer, "%c %s %s\n", mbusGetTypeAsChar(iType), szTimeStamp, szBuf);

    /* Output the string */
#if defined( MBUS_TRACE_FILE_OUTPUT )

    fputs(szBuffer, fLogFile);    
    fflush( fLogFile );

#ifdef MBUS_MULTIPLE_LOG_FILES
	lLogCount++;

	if (lLogCount%LOG_FILE_ENTRIES == 0)
	{
		platformCloseLogFile();
		platformInitLogFile();
	}

#endif /* MULTIPLE_LOG_FILES */

#elif defined( MBUS_TRACE_DEBUGGER_OUTPUT )
    
    OutputDebugString(szBuffer);

#endif

#endif /* #if defined(TRACE_FILE_OUTPUT) || defined (TRACE_DEBUGGER_OUTPUT) */
}
