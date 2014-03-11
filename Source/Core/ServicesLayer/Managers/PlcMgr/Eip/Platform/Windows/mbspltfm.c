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

/* define this if you want logging broken up into multiple, smaller log files */
/* #define MBUS_MULTIPLE_LOG_FILES */
/* number of log file entries before a new file is created */
#define LOG_FILE_ENTRIES	200000

static char* GetLoggingDirectory();
static unsigned int __stdcall WinSerialTask(void* pParam);

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
    char szFileName[1000+128],
		 szLoggingDirectory[1000];
    int i;

	strcpy(szLoggingDirectory, GetLoggingDirectory());

	_mkdir(szLoggingDirectory);

     /* Find the first unused sequential number for creating debug file name */
    for( i = 0; i < 1000; i++ )
    {
        sprintf( szFileName, "%smbsdebug%d.txt", szLoggingDirectory, i);    
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

char* GetLoggingDirectory()
{
	HKEY FileKey;
	static char FileString[1000];
	char TempFileString[1000];
	DWORD FileStringLen = 1000;

	//default logging directory
	strcpy(FileString, "c:\\et_ip\\modbus\\");

	if (!RegOpenKeyExA(HKEY_CURRENT_USER, _T("Software\\Pyramid Solutions\\EtherNetIP\\Settings"), 0, KEY_QUERY_VALUE, &FileKey))
	{
		if (!RegQueryValueExA(FileKey, _T("ModbusLoggingDirectory"), NULL, NULL, (LPBYTE)TempFileString, &FileStringLen))
		{
			strcpy(FileString, TempFileString);
			if (TempFileString[strlen(TempFileString)-1] != '\\')
				strcat(FileString, "\\");
		}

		RegCloseKey(FileKey);
	}

	return FileString;
}


#ifdef ET_IP_MODBUS_SERIAL
COMPortData* gpnCOMPorts;						/* Number of COM Ports in use*/
COMPortData	gaCOMPorts[MBUS_MAX_COM_PORTS];		/* COM Ports in use array */

void platformSerialLibInit()
{
	gpnCOMPorts = gaCOMPorts;
}

void platformSerialInitTargetData()
{
    TCHAR	szStr[256];
	TCHAR	szValue[256];
	HKEY	hKey;
	UINT32	dwValues = 0,
			dwStrLen = 255,
			dwValueLen = 255,
			dwType = REG_SZ;
	UINT32	i;

	gpnHostCOMPorts = galHostCOMPorts;

	//serial ports are stored in the registry
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
					 _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
					 0,
					 KEY_READ,
					 &hKey ) != ERROR_SUCCESS )
		return;

	/* find the number of subkeys (i.e. number of serial ports) */
	if (RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, 
						&dwValues, /* number of values */
						NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		RegCloseKey( hKey );
		return;
	}

	if (dwValues == 0)
	{
		mbusDumpStr0(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_MSG, 0, 0, "No serial ports found!");
		RegCloseKey( hKey );
		return;
	}

	i = 0;
	/* Continue enumerating the serial ports */
	while ((RegEnumValue(hKey, i, szStr, &dwStrLen, NULL, &dwType, szValue, &dwValueLen) != ERROR_NO_MORE_ITEMS) && 
		   (gpnHostCOMPorts < &galHostCOMPorts[MBUS_MAX_COM_PORTS]))
	{
		/* parse out the COM port number */
		*gpnHostCOMPorts = atoi(&szValue[3]);

		gpnHostCOMPorts++;

		/* reinitialize values */
		dwStrLen = 255;
		dwValueLen = 255;
		dwType = REG_SZ;
		i++;
	}

	RegCloseKey( hKey );

}

PLATFORM_PORT_HANDLE platformInitSerialPort(UINT32 lPort)
{
	HANDLE	hPort;
	TCHAR		strPort[20];
	DCB				dcb;
	COMMTIMEOUTS	ct;
	UINT32		lThreadId;
	HANDLE		hThread;

	sprintf(strPort, "COM%d", lPort);

	/* Open the port and configure it */
	hPort = CreateFile(strPort,
            GENERIC_READ|GENERIC_WRITE,
             0,
             NULL,
             OPEN_EXISTING,
             0, //FILE_FLAG_SEQUENTIAL_SCAN,
             NULL);

	if(hPort == INVALID_HANDLE_VALUE) 
		return INVALID_HANDLE_VALUE;

	GetCommState(hPort, &dcb);
	
	dcb.BaudRate = 9600;
	dcb.fBinary = TRUE;               
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = TRUE;
	dcb.fOutX = FALSE;  
	dcb.fInX = FALSE;	  
	dcb.fErrorChar = FALSE;   
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = FALSE;   
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY; 
	dcb.StopBits = TWOSTOPBITS;
	
	if(!SetCommState(hPort, &dcb))
	{
		CloseHandle(hPort);
		return INVALID_HANDLE_VALUE;
	}

	GetCommTimeouts(hPort, &ct);
	ct.ReadIntervalTimeout =  MAXDWORD;		/* 50	*/
	ct.ReadTotalTimeoutMultiplier = 0;	/* 3	*/		
	ct.ReadTotalTimeoutConstant = 0;	/* 200	*/
	ct.WriteTotalTimeoutMultiplier = 5;
	ct.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts(hPort, &ct);

	//add a COM Port structure for this thread
	gpnCOMPorts->hCOMPort = hPort;

	/* Fire up a Serial Task*/
	hThread = CreateThread( NULL, 0, (void*)WinSerialTask, gpnCOMPorts, 0, &lThreadId );

	if ( !hThread )
	{
		CloseHandle(hPort);
		return INVALID_HANDLE_VALUE;
	}

	SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );
	SetThreadPriority( hThread, THREAD_PRIORITY_TIME_CRITICAL );
	
	//setup COM port
	SetupComm(hPort, MSG_BUF_SIZE, MSG_BUF_SIZE);

	gpnCOMPorts++;

	return hPort;
} 

UINT32 platformSerialSendMessage(PLATFORM_PORT_HANDLE hPort, UINT8* Message, UINT16 Size)
{
	UINT32	BytesWritten;
	
	WriteFile(hPort, Message, Size, &BytesWritten, NULL);

	return BytesWritten;
}

COMPortData* platformFindCOMPort(PLATFORM_PORT_HANDLE hPort)
{
	COMPortData* pCOMPort;

	for (pCOMPort = gaCOMPorts; pCOMPort < gpnCOMPorts; pCOMPort++)
	{
		if (pCOMPort->hCOMPort == hPort)
			return pCOMPort;
	}

	return NULL;
}

UINT8 platformSerialPeekMessage(PLATFORM_PORT_HANDLE hPort)
{
	COMPortData* pCOMPort = platformFindCOMPort(hPort);

	if (pCOMPort == NULL)
		return FALSE;

	return (pCOMPort->InData[pCOMPort->OutMsgIndex][0] != 0);
}

INT32 platformSerialGetMessage(PLATFORM_PORT_HANDLE hPort, UINT8* Message)
{
	INT32 iSize;
	COMPortData* pCOMPort = platformFindCOMPort(hPort);

	if (pCOMPort == NULL)
		return 0;

	iSize = pCOMPort->InData[pCOMPort->OutMsgIndex][0];

	memcpy(Message, &pCOMPort->InData[pCOMPort->OutMsgIndex][1], iSize);
	memset(&pCOMPort->InData[pCOMPort->OutMsgIndex][0],0,MSG_BUF_SIZE);

	pCOMPort->OutMsgIndex++;

	if(pCOMPort->OutMsgIndex == NUM_MSGS)
		pCOMPort->OutMsgIndex = 0;

	return iSize;
}

unsigned int __stdcall WinSerialTask(void* pParam)
{
	UINT32	TimeOut;
	UINT8	BytesStored = 0,
			InMsgIndex = 0;

	COMPortData* pCOMPort = (COMPortData*)pParam;
	UINT32	BytesRead;
	UINT32	TmpTickCount;

	pCOMPort->OutMsgIndex = 0;

	PurgeComm(pCOMPort->hCOMPort, PURGE_RXCLEAR);
	
	while(1) 
	{
		/* If the buffer is full, just idle till the app catches up */

		if( pCOMPort->InData[InMsgIndex][0] != 0 )
		{
			Sleep(1);
			continue;
		}

		/* If BytesStored > 0, we're in the process of receiving a message */

		if( BytesStored )
		{
			/*
			** If we're timed out or we've received the maximum message size, 
			** the message is complete.  Save the message size in its 1st byte, 
			** increment InMsgIndex and reset BytesStored to 0;
			*/

			TmpTickCount = GetTickCount();

			if(( TmpTickCount >= TimeOut ) || ( BytesStored == MSG_BUF_SIZE ))
			{
				pCOMPort->InData[InMsgIndex][0] = BytesStored;
			
				++ InMsgIndex;
				if(InMsgIndex == NUM_MSGS)
					InMsgIndex = 0;

				BytesStored = 0;
			}
		}
		
		/* Always try to read a byte */
		if(ReadFile(pCOMPort->hCOMPort, &pCOMPort->InData[InMsgIndex][BytesStored+1], 1, &BytesRead, NULL))
		{

			/* 
			** If we've gotten a byte, increment the BytesRead index and reset
			** the message time out.
			*/

			if(BytesRead == 1)
			{
				++BytesStored;
				TimeOut = GetTickCount() + MAX_CHAR_DELAY;
				continue;
			}
		}

		Sleep(1);
	}
}
#endif /* ET_IP_MODBUS_SERIAL */
