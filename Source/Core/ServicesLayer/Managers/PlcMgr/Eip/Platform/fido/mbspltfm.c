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
** Modbus Platform dependant code for Fido.
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


/*---------------------------------------------------------------------------
** mbus_platformInitLogFile( )
**
** Creates a trace dump file.
**---------------------------------------------------------------------------
*/

void mbus_platformInitLogFile(void)
{
    /*
	** This function is called during intiallization.
	**
	** It must open a log file (e.g. fLogFile) for future use by the  
	** mbus_platformWriteLog() function.
	*/
}

/*---------------------------------------------------------------------------
** mbus_platformCloseLogFile()
**
** Closes the log file
**---------------------------------------------------------------------------
*/

void mbus_platformCloseLogFile(void)
{

    /* 
	** This function is called when the stack is shutting down 
	** (i.e. clientStop() has been called) 
	**
	** It must close the log file opened by mbus_platformInitLogFile().
	*/
}

/*---------------------------------------------------------------------------
** platformWriteLog()
**
** Writes a record to the log file
**---------------------------------------------------------------------------
*/

void mbus_platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp)
{
	/*
	** This function is called from a variety of places in the stack
	** during execution to write various warnings and data to the log
	** file.
	**
	** The szBuf parameter points to a NULL terminated string containing
	** the ASCII formatted data to be written to the log file.
	**
	** The bTimeStamp parameter indicates whether the stack suggests a 
	** time stamp should be prepended to the log entry.  The general rule 
	** adhered to within the stack is that raw data dumps (in hex ASCII 
	** format) will call this funtion with bTimeStamp == FALSE. Other 
	** notifications call this function with bTimeStamp == TRUE.
	**
	** In practice any log format desired can be accomodated here.
	*/	
#ifdef MBUS_TRACE_FILE_OUTPUT
	int ictl;

    static char szBuffer[(MAX_PACKET_SIZE*3)+1000];
	static char szSrcIp[20], szDstIp[20];

	/* always use timestamp */
	bTimeStamp = bTimeStamp;
	/* iSeverity not included in log statement */
	iSeverity = iSeverity;
    
	if ((lSrcIp != 0) || (lDstIp != 0))
	{
		strcpy(szSrcIp, socketGetAddressString(lSrcIp));
		strcpy(szDstIp, socketGetAddressString(lDstIp));
		sprintf(szBuffer, "%c (%s -> %s) %s\n", traceGetTypeAsChar(iType), szSrcIp, szDstIp, szBuf);
	}
	else
		sprintf(szBuffer, "%c %s\n", traceGetTypeAsChar(iType), szBuf);

    /* Output the string */
	printf(szBuffer);

#endif
}
