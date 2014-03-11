/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EtIPAdapter.cpp
**
** COPYRIGHT (c) 2001-2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Ethernet/IP Adapter API implementation
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

// Insert your headers here

#define EXTERN extern "C"

#include ETIPADAPTER_PLATFORM_HEADER
#include "../EtIpApi.h"
#include "../eipclien.h"
#include "EtIPAdapter.h"

#define TARGET_RESOLUTION    0        /* Optimal timer resolution */

/* Main task will be executed once per TIMER_INTERVAL milliseconds */
#if defined( _WIN32_WCE )
#define TIMER_INTERVAL       10       /* Set it to 10 milliseconds for slower WinCE device */
#else
#define TIMER_INTERVAL       1        /* Set it to 1 millisecond for faster Windows PC */
#endif

HANDLE        ghTickEvent;
UINT32        glTimerRes;          /* Main Task timer resolution */
UINT32        gTimerID;            /* Main task timer Id */


/*---------------------------------------------------------------------------
** EIPTask( )
**
** Private EIP Task.  This task is created automatically by a call to 
** EtIPAdapterStart.  Ir exits when gbTerminate is set to TRUE, which is done
** by EtIPAdapterStop();
**---------------------------------------------------------------------------
*/

EXTERN HANDLE ghTickEvent;

#if defined( _WIN32_WCE )
unsigned long __stdcall EIPTask(void* pParam)
#else
unsigned int __stdcall EIPTask(void* pParam)
#endif
{
	pParam = pParam;
	while(!gbTerminated)
	{
		clientMainTask();

		WaitForSingleObject(ghTickEvent, 1);	/* Wait for event or one tick */
		ResetEvent( ghTickEvent );
	}

	timeKillEvent(gTimerID);
	gbReallyTerminated = TRUE;
	return(0);
}

/*---------------------------------------------------------------------------
** DllMain( )
**
** Called when DLL is loaded and unloaded
**---------------------------------------------------------------------------
*/
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	hModule = hModule;
	lpReserved = lpReserved;

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:				
			break;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------
** MainTaskTimer( )
**
** Timer that sets an event to wake up the the main task.
**---------------------------------------------------------------------------
*/
#if !defined( _WIN32_WCE )
#ifdef EIP_64BIT
void MainTaskTimer(UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
#else
void CALLBACK MainTaskTimer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
#endif
{
	//compiler warnings
	wTimerID = wTimerID;
	msg = msg;
	dwUser = dwUser;
	dw1 = dw1;
	dw2 = dw2;

    SetEvent(ghTickEvent);
}
#else
void CALLBACK MainTaskTimer(HWND hwnd, UINT msg, UINT event, DWORD dwTime)
{
	//compiler warnings
	hwnd = hwnd;
	msg = msg;
	event = event;
	dwTime = dwTime;
	
    SetEvent(ghTickEvent);
}
#endif

/*---------------------------------------------------------------------------
** StartEventTriggerTimer( )
**
** Start executing the timer to wake up the the main task.
**---------------------------------------------------------------------------
*/
HANDLE StartEventTriggerTimer(void)
{
#if !defined( _WIN32_WCE )        
	TIMECAPS tc;
#endif
    HANDLE xEvent;

	/* Create the event to be triggered by the timer */
    if((xEvent = CreateEvent (NULL, FALSE, FALSE, NULL)) == NULL)
    {
        return(NULL);
    }

#if !defined( _WIN32_WCE )    
	glTimerRes = TARGET_RESOLUTION;
    
    if ( timeGetDevCaps( &tc, sizeof(TIMECAPS) ) == TIMERR_NOERROR )
    {
        /* Determine the capability of using the target resolution.
           It will not be used if the desired resolution is not supported.
           If it is supported, set it. */
        glTimerRes = PLATFORM_MIN( PLATFORM_MAX( tc.wPeriodMin, TARGET_RESOLUTION ), tc.wPeriodMax );
    }
    
    timeBeginPeriod( glTimerRes );
        
    /* Start the timer */
    gTimerID = timeSetEvent( TIMER_INTERVAL, glTimerRes, MainTaskTimer, (DWORD)xEvent, TIME_PERIODIC );

    if ( !gTimerID ) 
    {
        timeEndPeriod( glTimerRes );
        CloseHandle( xEvent );
        return(NULL);
    }
#else
	gTimerID = SetTimer( NULL, 0, TIMER_INTERVAL, MainTaskTimer );

	if ( !gTimerID ) 
	{		
		CloseHandle( xEvent );
		return(NULL);
	}
#endif

    return( xEvent );
}

/*---------------------------------------------------------------------------
** EtIPAdapterStart( )
**
** Start executing the adapter.
**---------------------------------------------------------------------------
*/

ET_IP_API BOOL ET_IP_CALLBACK EtIPAdapterStart()
{
	UINT32   lThreadId;
	HANDLE   hThread;	
	
	if (!clientStart())
		return FALSE;

	ghTickEvent = StartEventTriggerTimer();	
	gbReallyTerminated = FALSE;
	
	/* Create the EADK task */
#if !defined( _WIN32_WCE )
	hThread = (HANDLE)_beginthreadex( (void *)NULL, 
							  (unsigned)0,
							  EIPTask, 
							  (void *)NULL, 
							  (unsigned)0, 
							  (unsigned *)&lThreadId );

	if ( !hThread )
		return FALSE;

	SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS );	
	SetThreadAffinityMask(hThread, 1);
#else
	hThread = (HANDLE)CreateThread(NULL, 
							  (unsigned)0,
							  EIPTask, 
							  NULL, 
							  (unsigned)0, 
							  (unsigned long*)&lThreadId );

	if ( !hThread )
		return FALSE;

#endif

	SetThreadPriority( hThread, THREAD_PRIORITY_TIME_CRITICAL );	
	CloseHandle( hThread );
	return TRUE;
}

/*---------------------------------------------------------------------------
** EtIPAdapterStop( )
**
** Stop running the adapter. Should always be called before unloading the DLL.
**---------------------------------------------------------------------------
*/

ET_IP_API void ET_IP_CALLBACK EtIPAdapterStop()
{	
	clientStop();			/* Remove all connections, sessions, requests */	
}

/*---------------------------------------------------------------------------
** EtIPAddAssemblyInstance( )
**
** Add a new assembly object. nAssemblyInstance must be unique among assembly
** object collection. Connection points and configuration connection instance
** for the new connection - all fall in the category of the assembly instance.
** nType must be a combination of a type from EtIPAssemblyAccessType with a type 
** from EtIPAssemblyDirectionType and, optionally, one of the types from 
** tagEtIPAssemblyAdditionalFlag defined in EtIPApi.h.
** nOffset will determine offset of this particular assembly in the assembly
** buffer. nOffset may be passed as INVALID_OFFSET - in this case it will be 
** automatically assigned by the stack from the unalocated assembly space. 
** nLength is the assembly object size in bytes.
** szAssemblyName is the optional tag name associated with the assembly
** instance. It may be specified by the incoming Forward Open instead of the 
** assembly instance. Can be passed as NULL if there is no associated tag name.
** Returns 0 in case of success or one of the ERR_ASSEMBLY_... error codes.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPAddAssemblyInstance(int nAssemblyInstance, int nType, int nOffset, int nLength, char* szAssemblyName)
{
	return clientAddAssemblyInstance(nAssemblyInstance, (UINT16)nType, nOffset, (UINT16)nLength, szAssemblyName);
}

/*---------------------------------------------------------------------------
** EtIPRemoveAssemblyInstance( )
**
** Removes previosly configured assembly object. nAssemblyInstance is the 
** unique instance of the assembly object.
** Returns 0 in case of success or ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID
** the instance specified is invalid, or ERR_ASSEMBLY_INSTANCE_USED if 
** there is an active or pending connection that is configured to use 
** this instance.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPRemoveAssemblyInstance(int nAssemblyInstance)
{
	return clientRemoveAssemblyInstance(nAssemblyInstance);
}

/*---------------------------------------------------------------------------
** EtIPRemoveAllAssemblyInstances( )
**
** Removes all previosly configured assembly objects.
** Returns 0 in case of success or ERR_ASSEMBLY_INSTANCE_USED if 
** there is an active or pending connection that is configured to use 
** this instance.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPRemoveAllAssemblyInstances(void)
{
	return clientRemoveAllAssmInstances();
}

/*---------------------------------------------------------------------------
** EtIPAddAssemblyMember( )
**
** Add a new member for the assembly object. If members are used, this function 
** will be called multiple times after calling EtIPAddAssemblyInstance. 
** The first call will be for offset 0. All sebsequent ones will be for the 
** offset starting where the previous EtIPAddAssemblyMember ended. 
** EtIPAddAssemblyMember should cover all data size allocated for this
** assembly instance. If a few bits should be skipped EtIPAddAssemblyMember
** will be called with nMember set to INVALID_MEMBER. There is no byte or word 
** padding.
** nAssemblyInstance must be a valid instance previously allocated with a 
** EtIPAddAssemblyInstance successful call. nMember is a member id associated
** with this particular member. nBitSize is the member size in bits.  
** Returns 0 in case of success or one of the ERR_ASSEMBLY_... error codes.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPAddAssemblyMember(int nAssemblyInstance, int nMember, int nBitSize)
{
	return clientAddAssemblyMember(nAssemblyInstance, nMember, (UINT16)nBitSize);
}

/*---------------------------------------------------------------------------
** EtIPRemoveAllAssemblyMembers( )
**
** Removes previosly configured assembly object members. Gives the client
** application a chance to repopulate member array if needed. There should 
** be no active or pending connections using this assembly instance for the 
** call to succeed.
** nAssemblyInstance is previously configured instance of the assembly object.
** Returns 0 in case of success or ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID
** the instance specified is invalid, or ERR_ASSEMBLY_INSTANCE_USED if 
** there is an active or pending connection that is configured to use 
** this instance.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPRemoveAllAssemblyMembers(int nAssemblyInstance)
{
	return clientRemoveAllAssemblyMembers(nAssemblyInstance);
}

/*---------------------------------------------------------------------------
** EtIPGetAssemblyInstances( )
**
** Returns total number of configured assemblies.
** pnAssemblyInstanceArray should point to the array of MAX_ASSEMBLIES integers.
** It will receive the assembly instances.
** Example:
**
** int AssemblyInstanceArray[MAX_ASSEMBLIES];
** int nNumAssemblies = EtIPGetAssemblyInstances(AssemblyInstanceArray);
**
** After return AssemblyInstanceArray[0] will have the first assembly instance,
** AssemblyInstanceArray[1] will have the second assembly instance ...
** AssemblyInstanceArray[nNumAssemblies-1] will have the last assembly 
** instance.
**---------------------------------------------------------------------------
*/

ET_IP_API INT32 ET_IP_CALLBACK EtIPGetAssemblyInstances(INT32 *pnAssemblyInstanceArray)
{
	return clientGetAssemblyInstances(pnAssemblyInstanceArray);
}

/*---------------------------------------------------------------------------
** EtIPGetAssemblyConfig( )
**
** Return configuration for the particular assembly instance.
** nAssemblyInstance is the assembly instance obtained by calling 
** EtIPGetAssemblyInstances().
** pAssemblyConfig is the pointer to the EtIPAssemblyConfig structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** Returns 0 in case of a success, or ERR_ASSEMBLY_INVALID_INSTANCE
** if assembly with this instance does not exist.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPGetAssemblyConfig(int nAssemblyInstance, EtIPAssemblyConfig* pAssemblyConfig)
{
	return clientGetAssemblyConfig( nAssemblyInstance, pAssemblyConfig );
}

/*-------------------------------------------------------------------------------
** EtIPGetAssemblyMemberData( )
**
** Fills the provided buffer with the assembly data for the particular 
** member of an assembly instance. pData is the pointer to the buffer to
** be filled in. nSize is the allocated buffer length in bytes.
** nAssemblyInstance and nMember identify the target member of an assembly object.
** Returns the number of bytes populated in pData, which will be the least of 
** the member size and the passed buffer length. 
**--------------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPGetAssemblyMemberData( int nAssemblyInstance, int nMember, unsigned char* pData, int nSize )
{
	return (INT32)clientGetAssemblyMemberData(nAssemblyInstance, nMember, (UINT8*)pData, (UINT16)nSize);
}

/*---------------------------------------------------------------------------
** EtIPSetAssemblyMemberData( )
**
** Fills the assembly member with the data from the passed data buffer.
** nAssemblyInstance and nMember identify the target member.
** pData is the pointer to the buffer to copy data from. 
** nSize is the buffer length in bytes. If member size is less than 
** nSize only member size bits will be copied from the pData. 
** Returns the actual size of copied data in bytes, which will be the 
** least of the member size and the passed buffer length.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSetAssemblyMemberData( int nAssemblyInstance, int nMember, unsigned char* pData, int nSize )
{
	return (INT32)clientSetAssemblyMemberData(nAssemblyInstance, nMember, (UINT8*)pData, (UINT16)nSize);
}

/*-----------------------------------------------------------------------------
** EtIPGetAssemblyInstanceData( )
**
** Fills the provided buffer with the assembly data for the particular 
** assembly instance. pData is the pointer to the buffer. nSize is the 
** buffer length in bytes. Returns the actual size of copied data which will be 
** the least of assembly object size and the passed buffer length.
** Returns 0 if nAssemblyInstance has not been configured.
**-----------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPGetAssemblyInstanceData( int nAssemblyInstance, unsigned char* pData, int nSize )
{
	return (INT32)clientGetAssemblyInstanceData(nAssemblyInstance, (UINT8*)pData, (UINT16)nSize);
}

/*------------------------------------------------------------------------------
** EtIPSetAssemblyInstanceData( )
**
** Fills the assembly object with the passed buffer data. pData is the 
** pointer to the buffer. iSize is the buffer length in bytes. 
** Returns the actual size of copied data in bytes, which may be less than nSize
** if the assembly object size is less than nSize.
** Returns 0 if nAssemblyInstance has not been configured.
**------------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSetAssemblyInstanceData( int nAssemblyInstance, unsigned char* pData, int nSize )
{
	return (INT32)clientSetAssemblyInstanceData(nAssemblyInstance, (UINT8*)pData, (UINT16)nSize);
}

/*-------------------------------------------------------------------------------
** EtIPGetAssemblyCombinedData( )
**
** Fills the provided buffer with the assembly data for one or more 
** assembly instances. pData is the pointer to the buffer. nSize is the 
** number of bytes to copy. nOffset is the start copy offset in the assembly 
** buffer ASSEMBLY_SIZE long where data for all instances is stored.
** Return the number of bytes copied in the provided buffer, which will be equal 
** to nSize if successful or 0 if nOffset+nSize exceed the ASSEMBLY_SIZE.
**-------------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPGetAssemblyCombinedData( unsigned char* pData, int nOffset, int nSize )
{
	return (INT32)clientGetAssemblyCombinedData((UINT8*)pData, (INT32)nOffset, (UINT16)nSize);
}

/*-----------------------------------------------------------------------------
** EtIPSetAssemblyCombinedData( )
**
** Fills one or more assembly instances with the passed buffer data. pData is 
** the pointer to the buffer. nSize is the number of bytes to copy. 
** nOffset is the start copy offset in the assembly buffer ASSEMBLY_SIZE 
** long where data for all instances is stored.
** Return the number of bytes copied from the provided buffer, which will be 
** equal to nSize if successful or 0 if nOffset+nSize exceed the ASSEMBLY_SIZE.
**------------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSetAssemblyCombinedData( unsigned char* pData, int nOffset, int nSize )
{
	return (INT32)clientSetAssemblyCombinedData((UINT8*)pData, (INT32)nOffset, (UINT16)nSize);
}

/*---------------------------------------------------------------------------
** EtIPAutomaticRunIdleEnforcement( )
**
** Enforces the presense of the 32-bit header in the O->T direction and enforces
** the lack of a 32-bit header in the T->O direction for ExclusiveOwner I/O 
** connections.  Enforces no 32-bit header for either direction on InputOnly or
** ListenOnly connections.
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPAutomaticRunIdleEnforcement(BOOL bEnforce)
{
	clientAutomaticRunIdleEnforcement(bEnforce);
}

/*---------------------------------------------------------------------------
** EtIPGetNumConnections( )
**
** Returns total number of connections : both opened and pending outgoing.
**---------------------------------------------------------------------------
*/

ET_IP_API int ET_IP_CALLBACK EtIPGetNumConnections()
{
	return clientGetNumConnections();
}

/*---------------------------------------------------------------------------
** EtIPGetConnectionInstances( )
**
** Returns total number of connections in the state other than 
** ConnectionNonExistent.
** pnConnectionIDArray should point to the array of MAX_CONNECTIONS integers.
** It will receive the connection instances.
** Example:
**
** int ConnectionInstanceArray[MAX_CONNECTIONS];
** int nNumConnections = EtIPGetConnectionInstances(ConnectionInstanceArray);
**
** After return ConnectionInstanceArray[0] will have the first connection instance,
** ConnectionInstanceArray[1] will have the second connection instance ...
** ConnectionInstanceArray[nNumConnections-1] will have the last connection 
** instance.
**---------------------------------------------------------------------------
*/

ET_IP_API INT32 ET_IP_CALLBACK EtIPGetConnectionInstances(INT32 *pnConnectionInstanceArray)
{
	return clientGetConnectionInstances(pnConnectionInstanceArray);
}

/*---------------------------------------------------------------------------
** EtIPGetConnectionState( )
**
** Return the state of the particular connection from the EtIPConnectionState
** enumeration.
** nConnectionInstance is the connection instance obtained by calling 
** EtIPGetConnectionInstances() and should be between 1 and MAX_CONNECTIONS. 
**---------------------------------------------------------------------------
*/

ET_IP_API int ET_IP_CALLBACK EtIPGetConnectionState(int nConnectionInstance)
{
	return clientGetConnectionState( nConnectionInstance );
}

/*---------------------------------------------------------------------------
** EtIPGetConnectionRunIdleFlag( )
**
** Should be called for incoming I/O connections only to get the Run/Idle 
** flag that came with the run/idle header in the I/O message.
** Returns a value from the EtIPConnectionRunIdleFlag enumeration.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPGetConnectionRunIdleFlag(int nConnectionInstance)                    
{
    return clientGetConnectionRunIdleFlag(nConnectionInstance);
}

/*---------------------------------------------------------------------------
** EtIPGetConnectionConfig( )
**
** Return configuration for the particular connection.
** nConnectionInstance is the connection instance obtained by calling 
** EtIPGetConnectionInstances().
** pConfig is the pointer to the EtIPConnectionConfig structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state.
**---------------------------------------------------------------------------
*/

ET_IP_API int ET_IP_CALLBACK EtIPGetConnectionConfig(int nConnectionInstance, EtIPConnectionConfig* pConfig)
{
	return clientGetConnectionConfig( nConnectionInstance, pConfig );
}

/*---------------------------------------------------------------------------
** EtIPGetConnectionInternalCfg( )
**
** Return internal configuration for the particular connection.
** nConnectionInstance is the connection instance obtained by calling 
** EtIPGetConnectionInstances().
** pConfig is the pointer to the EtIPInternalConnectionConfig structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPGetConnectionInternalCfg(INT32 nConnectionInstance, EtIPInternalConnectionConfig* pConfig)
{
	return clientGetConnectionInternalCfg(nConnectionInstance, pConfig);
}

/*---------------------------------------------------------------------------
** EtIPGetConnectionStats( )
**
** Return connection stats for the particular connection.
** nConnectionInstance is the connection instance obtained by calling 
** EtIPGetConnectionInstances().
** pStats is the pointer to the EtIPConnectionStats structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** bClear will reset stats to 0 if TRUE
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPGetConnectionStats(INT32 nConnectionInstance, EtIPConnectionStats* pStats, BOOL bClear)
{
    return clientGetConnectionStats(nConnectionInstance, pStats, bClear);
}

/*---------------------------------------------------------------------------
** EtIPSetConnectionProducedSize( )
**
** Set the producing size for a variable sized class1 connection.
** nConnectionInstance is the connection instance obtained by calling 
** clientGetConnectionInstances().
** iSize is the new size sent on the I/O connection. The size must be equal to or 
** lesser than the original connection size
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state. Or 
** ERR_INVALID_VARIABLE_CONNECTION_SIZE if the size specified is too large.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSetConnectionProducedSize(INT32 nConnectionInstance, UINT16 iSize)
{
	return clientSetConnectionProducedSize(nConnectionInstance, iSize);
}

/*---------------------------------------------------------------------------
** EtIPSetNewConnectionInstance( )
**
** Set a new connection ID for a particular connection.  This is aimed at applications
** organizing their target connections because the EIP stack initially assigned it an ID.
** nOldConnectionInstance is the existing connection instance
** nNewConnectionInstance is the new connection instance
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nOldConnectionInstance is out of the connection instance range or doesn't exist.
** Also returns ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED if nNewConnectionInstance is 
** already in use.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSetNewConnectionInstance(INT32 nOldConnectionInstance, INT32 nNewConnectionInstance)
{
	return clientSetNewConnectionInstance(nOldConnectionInstance, nNewConnectionInstance);
}

/*---------------------------------------------------------------------------
** EtIPSendConnectionResponse( )
**
** Send a response to a potential connection.
** nConnectionInstance is the connection instance passed by NM_CONNECTION_VERIFICATION notification
** pErrorInfo is the response from the application.  If bGeneralStatus = 0, connection is approved/successful
** bAppSize is the size of Application Data (in words) appended to the ForwardOpen reply (typically 0)
** pAppData is the Application Data (in words) appended to the ForwardOpen reply
** See CIP specification for details on error codes.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSendConnectionResponse(INT32 nConnectionInstance, EtIPErrorInfo* pErrorInfo, UINT8 bAppSize, UINT8* pAppData)
{
	return clientSendConnectionResponse(nConnectionInstance, pErrorInfo, bAppSize, pAppData);
}

/*---------------------------------------------------------------------------
** EtIPAbortAllConnections( )
**
** Remove all connections.  This will close all originated and abort
** all target connections regardless of class.
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPAbortAllConnections()
{
	clientAbortAllConnections();
}

/*---------------------------------------------------------------------------
** EtIPGetRunMode()
**
** Returns Run mode setting. Initially set to FALSE. May be modified by 
** calling EtIPSetRunMode() function.
** The Run mode is being sent as the first bit of the 32-bit I/O header, 
** which (if configured) is sent with every producing I/O packet.
**---------------------------------------------------------------------------
*/
ET_IP_API BOOL ET_IP_CALLBACK EtIPGetRunMode(void)
{
	return clientGetRunMode();
}

/*---------------------------------------------------------------------------
** EtIPSetRunMode()
**
** Set Run mode by passing TRUE, or Idle mode by pasing FALSE as a parameter.
** The state is being sent as the first bit of the 32-bit I/O header, 
** which (if configured) is sent with every producing I/O packet.
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPSetRunMode( BOOL bRunMode )
{
	clientSetRunMode(bRunMode);
}

/*---------------------------------------------------------------------------
** EtIPRegisterEventCallBack( )
**
** Register client application callback function
**---------------------------------------------------------------------------
*/

ET_IP_API void ET_IP_CALLBACK EtIPRegisterEventCallBack( LogEventCallbackType *pfnLogEvent )
{
	clientRegisterEventCallBack( pfnLogEvent );
}

/*---------------------------------------------------------------------------
** EtIPRegisterMessageCallBack( )
**
** Register client application log message function
** This is only utilized with TRACE_OUTPUT defined
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPRegisterMessageCallBack( LogMessageCallbackType *pfnLogMessage )
{
    clientRegisterMessageCallBack( pfnLogMessage );
}

/*---------------------------------------------------------------------------
** EtIPSetDebuggingLevel( )
**
** Sets the level of debug statements written to a file/debug window if
** tracing is turned on.  The two filters are ANDed to determine if message
** should be logged.  See EtIPApi.h for filter macros (TRACE_LEVEL_* and TRACE_TYPE_*)
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPSetDebuggingLevel(UINT8 iDebugLevel, UINT16 iDebugType)
{
	clientSetDebuggingLevel(iDebugLevel, iDebugType);
}

/*---------------------------------------------------------------------------
** EtIPStartRequestGroup( )
**
** Starts the unconnected request group used to send multiple service 
** message. All subsequent EtIPSendUnconnectedRequest and 
** EtIPSendEPATHUnconnectedRequest calls issued before next EtIPStopRequestGroup
** is called are grouped into a single multiple service request. It will be sent
** after EtIPStopRequestGroup is called.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPStartRequestGroup()
{
    return clientStartRequestGroup();
}

/*---------------------------------------------------------------------------
** EtIPStopRequestGroup( )
**
** Stops forming the unconnected request group used to send multiple service 
** message. All EtIPSendUnconnectedRequest and EtIPSendEPATHUnconnectedRequest 
** calls issued between EtIPStartRequestGroup and EtIPStopRequestGroup
** are grouped into a single multiple service request. It will be sent
** after EtIPStopRequestGroup is called.
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPStopRequestGroup()
{
    clientStopRequestGroup();
}

/*---------------------------------------------------------------------------
** EtIPSendUnconnectedRequest( )
**
** Send either UCMM or Unconnected Send depending on whether szNetworkPath 
** specifies local or remote target.
** EtIPGetUnconnectedResponse will be used to get the response.
** pRequest points to a structure containing the request parameters.
** Check the return value. If it's greater than or equal to REQUEST_INDEX_BASE,
** then it is a request Id to be used when calling EtIPGetUnconnectedResponse.
** If it is less than REQUEST_INDEX_BASE then it's one of the 
** errors in the header file.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSendUnconnectedRequest( EtIPObjectRequest* pRequest )
{
	return clientSendUnconnectedRequest( pRequest );
}

/*---------------------------------------------------------------------------
** EtIPSendEPATHUnconnectedRequest( )
**
** Same as EtIPSendUnconnectedRequest above, but EPATH is populated by the 
** user. May be used in cases where more complicated EPATH should be used.
** Any available segment type mey be include in EPATH or the user may
** specify word size for parameters like class, instance, or member.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSendEPATHUnconnectedRequest( EtIPEPATHObjectRequest* pEPATHRequest )
{
	return clientSendEPATHUnconnectedRqst( pEPATHRequest );
}

/*---------------------------------------------------------------------------
** EtIPGetUnconnectedResponse( )
**
** Get a response for the previously transferred UCMM or an
** Unconnected Send message.
** nRequestId is a request Id returned from the previous call to the  
** EtIPSendUnconnectedRequest function. 
** pResponse contain response data in case of a success and
** extended error indormation in case of an error.
** Returns 0 if able to get the response or one of the error constants 
** listed above. If return value is 0 it does not guarantee the successful
** CIP response. You need to check bGeneralStatus member of pResponse to 
** determine whether the target returned success or an error. bGeneralStatus
** of 0 indicates a success. If bGeneralStatus is other than 0, iExtendedStatus
** will contain an optional extended error code and errorDescription will
** contain the error text corresponding to the general and extended error codes.
**---------------------------------------------------------------------------
*/

ET_IP_API int ET_IP_CALLBACK EtIPGetUnconnectedResponse(int nRequestId, EtIPObjectResponse* pResponse)
{
	return clientGetUnconnectedResponse(nRequestId, pResponse);
}

/*---------------------------------------------------------------------------
** EtIPRegisterObjectsForClientProcessing( )
**
** Provides the list of the objects that will be processed by the client 
** application. When the request is received for one of these objects
** the NM_CLIENT_OBJECT_REQUEST_RECEIVED notification message will be 
** sent to the client app. The client application can use EtIPGetClientRequest()
** and EtIPSendClientResponse() after that to get the request and 
** send the response.
** pClassNumberList is the pointer to the integer array. Each integer represent
** the class number of the object that will be proccessed by the client 
** application.
** nNumberOfClasses is the number of classes in the list.
** pServiceNumberList is the pointer to the integer array. Each integer represent
** the service that will be proccessed by the client application.
** nNumberOfServices is the number of the services in the list.
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPRegisterObjectsForClientProcessing(UINT32* pClassNumberList, UINT32 nNumberOfClasses, 
																	 UINT32* pServiceNumberList, UINT32 nNumberOfServices)
{
	clientRegObjectsForClientProc(pClassNumberList, nNumberOfClasses, pServiceNumberList, nNumberOfServices);
}

/*---------------------------------------------------------------------------
** EtIPRegisterRequestsForClientProcessing( )
**
** Provides the list of specific explicit requests that will be processed by the client 
** application.  The intent is to override or extend existing functionality of the 
** objects already in the EtherNet/IP library without modifying the source directly.
** When the request is received for one of these objects the 
** NM_CLIENT_OBJECT_REQUEST_RECEIVED notification message will be 
** sent to the client app. The client application can use EtIPGetClientRequest()
** and EtIPSendClientResponse() after that to get the request and 
** send the response.
** pSpecificRequests is the pointer to an array of specific services within objects
** already included in the EtherNet/IP stack. Each "specific object" represents
** the service/class/instance/attribute combination that will be proccessed 
** by the client application.
** nNumberOfSpecificObjects is the number of the specific object requests in the list.
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPRegisterRequestsForClientProcessing(EtIPObjectRegisterType* pSpecificRequests, UINT32 nNumberOfSpecificObjects)
{
	clientRegRequestsForClientProc(pSpecificRequests, nNumberOfSpecificObjects);
}

/*---------------------------------------------------------------------------
** EtIPGetClientRequest( )
**
** Gets the pending request that should be processed by the client
** application. The objects for the client application processing must
** be registered in advance by calling EtIPRegisterObjectsForClientProcessing()
** function.
** The client application will get NM_CLIENT_OBJECT_REQUEST_RECEIVED callback
** with a new request Id. The same request Id should be passed when calling
** EtIPGetClientRequest() function.
** pRequest is the pointer to the EtIPObjectRequest structure that will have 
** the request parameters on return.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPGetClientRequest( int nRequestId, EtIPObjectRequest* pRequest )
{
	return clientGetClientRequest( nRequestId, pRequest );
}

/*---------------------------------------------------------------------------
** EtIPGetEPATHClientRequest( )
**
** Same as EtIPGetClientRequest above, but EPATH may be used in cases where 
** more complicated EPATH may be used.
** Any available segment type may be include in EPATH including duplicate
** occurences of the same type.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPGetEPATHClientRequest( int nRequestId, EtIPEPATHObjectRequest* pEPATHRequest )
{
    return clientGetEPATHClientRequest( nRequestId, pEPATHRequest );
}

/*---------------------------------------------------------------------------
** EtIPGetPCCCRequest( )
**
** Gets the pending PCCC request that should be processed by the client
** application. 
** The client application will get NM_CLIENT_PCCC_REQUEST_RECEIVED callback
** with a new request Id. The same request Id should be passed when calling
** EtIPGetPCCCRequest() function.
** pRequest is the pointer to the EtIPPCCCRequest structure that will have 
** the request parameters on return.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/

ET_IP_API int ET_IP_CALLBACK EtIPGetPCCCRequest( int nRequestId, EtIPPCCCRequest* pPCCCRequest)
{
	return clientGetPCCCRequest(nRequestId, pPCCCRequest);
}

/*---------------------------------------------------------------------------
** clientSendClientResponse( )
**
** Send a response to the previously received request registered to be
** processed by the client application.
** The client application previously polled the request by calling
** EtIPGetClientRequest() function and now sends the response to that
** request.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSendClientResponse(int nRequestId, EtIPObjectResponse* pResponse)
{
	return clientSendClientResponse( nRequestId, pResponse );
}

/*---------------------------------------------------------------------------
** EtIPSendPCCCResponse( )
**
** Send a response to the previously received PCCC request.
** The client application previously polled the request by calling
** EtIPGetPCCCRequest() function and now sends the response to that
** request.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSendPCCCResponse(int nRequestId, EtIPPCCCResponse* pResponse)
{
	return clientSendPCCCResponse(nRequestId, pResponse);
}

/*---------------------------------------------------------------------------
** EtIPGetIdentityInfo( )
**
** Returns identity information.
** iInstance is the Identity Instance number to get the data from
** pInfo is the pointer to the EtIPIdentityInfo structure with the retrieved
** identity information.
** The client application is responsible for allocating EtIPIdentityInfo
** structure and passing its pointer with the EtIPGetIdentityInfo function
** call. On return the structure fill be filled with identity information.
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPGetIdentityInfo(UINT32 iInstance, EtIPIdentityInfo* pInfo)
{
    return clientGetIdentityInfo(iInstance, pInfo );
}

/*---------------------------------------------------------------------------
** EtIPSetIdentityInfo( )
**
** Updates identity information.
** iInstance is the Identity Instance number to set the data to
** pInfo is the pointer to the EtIPIdentityInfo structure with the new
** identity information.
** To change only some of the identity parameters the client application
** can call EtIPGetIdentityInfo, modify only appropriate structure members
** and then call EtIPSetIdentityInfo.
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPSetIdentityInfo(UINT32 iInstance,  EtIPIdentityInfo* pInfo )
{
    return clientSetIdentityInfo(iInstance, pInfo );
}

/*---------------------------------------------------------------------------
** EtIPSetIdentityStatus( )
**
** Explicitly sets the Identity Status so it is not determined by the stack.
** iInstance is the Identity Instance number to set the status
** iStatus is the current status.
** To change only some of the identity parameters the client application
** can call EtIPGetIdentityInfo, modify only appropriate structure members
** and then call EtIPSetIdentityInfo.
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPSetIdentityStatus(UINT32 iInstance, UINT16 iStatus)
{
	return clientSetIdentityStatus(iInstance, iStatus);
}

/*---------------------------------------------------------------------------
** EtIPClearIdentityStatus( )
**
** Clears application's setting of Identity status so it can be determined 
** by the stack.
** iInstance is the Identity Instance number to set the status
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPClearIdentityStatus(UINT32 iInstance)
{
	clientClearIdentityStatus(iInstance);
}

/*---------------------------------------------------------------------------
** EtIPGetHostIPAddress( )
**
** Returns an array of the host IP addresses as Internet unsigned long numbers
** alHostIPAddresses should point to the preallocated unsigned long array 
** where IP address should be stored.
** iNumHostIPAddressesAllowed indicates the number of unsigned long numbers
** preallocated in alHostIPAddresses.
** Returns the number of the obtained IP addresses or iNumHostIPAddressesAllowed
** (whichever is smaller).
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPGetHostIPAddress(UINT32* alHostIPAddresses, UINT32 iNumHostIPAddressesAllowed)
{
	return clientGetHostIPAddress(alHostIPAddresses, iNumHostIPAddressesAllowed);
}

/*---------------------------------------------------------------------------
** EtIPClaimHostIPAddress( )
**
** Claim one of the host IP addresses returned by EtIPGetHostIPAddress().
** It can be called repeatedly for each claimed IP address.  A TCP port can also
** be specified with the IP address.  If 0 is specified, the default TCP port
** 44818 (ENCAP_SERVER_PORT) will be used.  Note that each IP address can only
** use one port.
** Returned TRUE if claimed successfully, FALSE if the IP address is not in 
** the host IP address list.
**---------------------------------------------------------------------------
*/
ET_IP_API BOOL ET_IP_CALLBACK EtIPClaimHostIPAddress(UINT32 lHostIPAddress, UINT16 iPort)
{
	return clientClaimHostIPAddress( lHostIPAddress, iPort);
}

/*---------------------------------------------------------------------------
** EtIPGetClaimedHostIPAddress( )
**
** Returns an array with the claimed host IP addresses.
** alClaimedHostIPAddresses should point to the preallocated unsigned long array 
** where the claimed IP address will be stored.
** iNumHostIPAddressesAllowed indicates the number of unsigned long numbers
** preallocated in alHostIPAddresses.
** Returns the number of the claimed IP addresses or iNumHostIPAddressesAllowed 
** (whichever is smaller).
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPGetClaimedHostIPAddress(UINT32* alClaimedHostIPAddresses, UINT32 iNumHostIPAddressesAllowed)
{
	return clientGetClaimedHostIPAddress( alClaimedHostIPAddresses, iNumHostIPAddressesAllowed);
}

/*---------------------------------------------------------------------------
** EtIPClaimAllHostIPAddress( )
**
** Claims the ownership of all available host IP addresses.
** Function should be called only once after the EtIPAdapterStart() call.
** Returns FAIL if method was called more than once 
**		before the corresponding call to EtIPAdapterStop().
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPClaimAllHostIPAddress()
{
	clientClaimAllHostIPAddress();
}

/*---------------------------------------------------------------------------
** EtIPUnclaimHostIPAddress( )
**
** Unclaim one of the host IP addresses returned by EtIPGetHostIPAddress().
** Will be called repeatedly for each claimed IP address.
** Returned TRUE if unclaimed successfully, FALSE if the IP address is not in 
** the host IP address list.
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPUnclaimHostIPAddress( UINT32 lHostIPAddress )
{
	return clientUnclaimHostIPAddress(lHostIPAddress);
}

/*---------------------------------------------------------------------------
** EtIPUnclaimAllHostIPAddress( )
**
** Removes the ownership of all available claimed host IP addresses.
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPUnclaimAllHostIPAddress(void)
{
	clientUnclaimAllHostIPAddress();
}

/*---------------------------------------------------------------------------
** EtIPModbusGetCOMPorts( )
**
** Returns an array of the COM port numbers
** Only used if ET_IP_MODBUS_SERIAL is defined
** alCOMPorts should point to the preallocated unsigned array 
** where COM ports should be stored.
** Returns the number of the obtained COM ports
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPModbusGetCOMPorts(UINT32* alCOMPorts)
{
	return clientModbusGetCOMPorts(alCOMPorts);
}

/*---------------------------------------------------------------------------
** EtIPModbusClaimCOMPort( )
**
** Claim one of the COM ports returned by EtIPModbusGetCOMPorts().
** Will be called repeatedly for each claimed COM port.
** Only used if ET_IP_MODBUS_SERIAL is defined
** Returned TRUE if claimed successfully, FALSE if the COM port is not in 
** the host list.
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPModbusClaimCOMPort(UINT32 lCOMPort)
{
	return clientModbusClaimCOMPort(lCOMPort);
}

/*---------------------------------------------------------------------------
** EtIPModbusGetClaimedCOMPorts( )
**
** Returns an array with the claimed COM ports.
** Only used if ET_IP_MODBUS_SERIAL is defined
** alCOMPorts should point to the preallocated unsigned array where the 
** claimed COM ports will be stored.
** Returns the number of the claimed COM ports
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPModbusGetClaimedCOMPorts(UINT32* alCOMPorts)
{
	return clientModbusGetClaimedCOMPorts(alCOMPorts);
}

/*---------------------------------------------------------------------------
** EtIPModbusClaimAllCOMPorts( )
**
** Claims the ownership of all available COM ports.
** Only used if ET_IP_MODBUS_SERIAL is defined
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPModbusClaimAllCOMPorts()
{
	clientModbusClaimAllCOMPorts();
}
/*---------------------------------------------------------------------------
** EtIPGetTickCount( )
**
** Returns the system tick count.
**---------------------------------------------------------------------------
*/
ET_IP_API UINT32 ET_IP_CALLBACK EtIPGetTickCount()
{
	return clientGetTickCount();
}

/*-------------------------------------------------------------------------------
** EtIPSetKeepTCPOpenForActiveCorrespondingUDPFlag()
**
** This flag is set to TRUE by default. Use this call to change 
** KeepTCPOpenForActiveCorrespondingUDP flag. 
** When KeepTCPOpenForActiveCorrespondingUDP flag is set to TRUE the TCP 
** socket that was used to establish a I/O connection is kept open
** for the duration of the I/O connection.
** If this flag is set to False - TCP socket that was used to establish an
** I/O connection is closed if there is no other activity on this TCP 
** socket for more than 1 minute after connection is opened. Setting this flag
** to FALSE lets you save some resources in lueu of possible incompatibility 
** with some hardware/software. Incompatible hardware includes Rockwell devices. 
** When communicating with Rockwell hardware this flag should be kept as TRUE.
** Even when this flag is set to TRUE the other side of the I/O connection
** may close the TCP socket. This will be handled and I/O connection will be 
** kept open regardless what the bKeepTCPOpenForActiveCorrespondingUDP flag is.
** nConnectionInstance may be set to a particular connection instance and in this 
** case the flag will only apply to that connection. It may also be set to 
** INVALID_INSTANCE and it will apply to all connections already opened or will 
** be opened in the future that did not receive the same call for their instance 
** number.
**--------------------------------------------------------------------------------
*/
ET_IP_API int ET_IP_CALLBACK EtIPSetKeepTCPOpenForActiveCorrespondingUDPFlag( BOOL bKeepTCPOpenForActiveCorrespondingUDP, INT32 nConnectionInstance )
{
    return clientSetKeepTCPOpenForUDP(bKeepTCPOpenForActiveCorrespondingUDP, nConnectionInstance);
}   

/*---------------------------------------------------------------------------
** EtIPSetSTRINGI( )
**
** Convenient utility function to set a STRINGI to a data buffer
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPSetSTRINGI(UINT8* pDataBuffer, STRINGI* pString)
{
	clientSetSTRINGI(pDataBuffer, pString);
}

/*---------------------------------------------------------------------------
** EtIPGetSTRINGI( )
**
** Convenient utility function to get a STRINGI from a data buffer
** Returns 0 for success, ERR_STRINGI_BAD_SIZE if iBufferSize is too small
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPGetSTRINGI(STRINGI* pString, UINT8* pDataBuffer, UINT32 iBufferSize)
{
	clientGetSTRINGI(pString, pDataBuffer, iBufferSize);
}

/*---------------------------------------------------------------------------
** EtIPLogMessage( )
**
** Logs the provided NULL terminated message in the stack's log file with a TRACE_LEVEL_USER filter bit
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPLogMessage(char *szMsg)
{
	clientLogMessage(szMsg);
}

/*---------------------------------------------------------------------------
** EtIPGetNetworkStatus( )
**
** Returns the state of the network indicator (LED).  If multiple IP addresses
** are used and EIP_NSLED_PER_IP is defined, lHostIP is the "claimed" IP address
** to retrieve the NS LED status.  Otherwise lHostIP is ignored.
**---------------------------------------------------------------------------
*/
ET_IP_API EtIPNetworkStatus ET_IP_CALLBACK EtIPGetNetworkStatus(UINT32 lHostIP)
{
	return clientGetNetworkStatus(lHostIP);
}

/*---------------------------------------------------------------------------
** EtIPSendBroadcastListIdentity( )
**
** Sends out a List Identity broadcast message
** EtIPGetListIdentityResponse will be used to get the response(s)
** lIPAddress is the host address used to send the message
** iMaxDelay is the maximum delay allowed by targets to respond
** Returns TRUE if messages was successfully sent
**---------------------------------------------------------------------------
*/
ET_IP_API BOOL ET_IP_CALLBACK EtIPSendBroadcastListIdentity(UINT32 lIPAddress, UINT16 iMaxDelay)
{
	return clientSendBroadcastListIdentity(lIPAddress, iMaxDelay);
}

/*---------------------------------------------------------------------------
** EtIPGetListIdentityResponse( )
**
** Get a response for the previously sent List Identity broadcast.
** nId is a Id returned from the NM_NEW_LIST_IDENTITY_RESPONSE notification 
** Returns 0 in case of success or one of the error codes.
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPGetListIdentityResponse(INT32 nId, EtIPListIdentityInfo* pInfo)
{
	return clientGetListIdentityResponse(nId, pInfo);
}

/*---------------------------------------------------------------------------
** EtIPSetApplicationVerifyConnection( )
**
** Determines whether the application will verify connections before they
** are accepted.  If the application verifies connections it will recieve a
** NM_CONNECTION_VERIFICATION notification
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPSetApplicationVerifyConnection(BOOL bVerify)
{
	clientSetAppVerifyConnection(bVerify);
}

/*---------------------------------------------------------------------------
** EtIPSetBackplaneSupport( )
**
** Determines whether the application support unconnected requests to the 
** "backplane" port.  Requests targeted at the "device" slot will be handled by 
** the EtherNet/IP stack.  Requests for slots outside of the "device" will 
** receive a NM_BACKPLANE_REQUEST_RECEIVED notification.  The client 
** application can use EtIPGetClientRequest() and EtIPSendClientResponse() 
** after that to get the request and send the response.
** bEnable enables/disables support for responding to backplane requests
** iSlot is the slot for the EtherNet/IP stack "device"
**---------------------------------------------------------------------------
*/
ET_IP_API void ET_IP_CALLBACK EtIPSetBackplaneSupport(BOOL bEnable, UINT16 iSlot)
{
	clientSetBackplaneSupport(bEnable, iSlot);
}

/*---------------------------------------------------------------------------
** EtIPGetSystemStats( )
**
** Return system connection stats.
** pStats is the pointer to the EtIPSystemStats structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** bClear will reset stats to 0 if TRUE
** Returns 0 in case of a success, or -1 if CONNECTION_STATS isn't defined
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPGetSystemStats(EtIPSystemStats* pStats, BOOL bClear)
{
	return clientGetSystemStats(pStats, bClear);
}

/*---------------------------------------------------------------------------
** EtIPSetSystemStats( )
**
** Set system connection stats.  The intent is to reset specific members
** of the EtIPSystemStats structure to 0.
** pStats is the pointer to the EtIPSystemStats structure allocated
** by the calling application. 
** Returns 0 in case of a success, or -1 if CONNECTION_STATS isn't defined
**---------------------------------------------------------------------------
*/
ET_IP_API INT32 ET_IP_CALLBACK EtIPSetSystemStats(EtIPSystemStats* pStats)
{
	return clientSetSystemStats(pStats);
}