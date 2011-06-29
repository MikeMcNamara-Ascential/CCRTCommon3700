//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/Bepos/NtoKrnl.c 3     7/20/05 9:03p Cward $
//
// FILE DESCRIPTION:
//  This module implements the QNX kernel interface for the Burke Porter
//  Operating System.  These routines allow the BPOS to work with systems
//  that have QNX installed.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/Bepos/NtoKrnl.c $
// 
// 3     7/20/05 9:03p Cward
// cleanup
//
// 2     8/18/04 11:22p Cward
// Removed path info from the includes since the makefile points to the
// include directory
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 12:53p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 2     6/20/03 7:35a Khykin
// Added a function to set thread priority.
//
// 2     6/18/03 6:43p Bmeinke
// Fixed KrnlSetThreadPriority() by changing  schedParam.sched_curpriority
// to schedParam.sched_priority
//
// 1     4/22/03 9:32a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:21a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:40  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:25a Khykin
// Initial Checkin
//
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
//
// 36    11/08/02 8:23a Tbochene
// Added GetEnv()
//
// 35    10/01/02 7:57a Khykin
// Updating the handling of the ETIMEOUT.
//
// 34    9/26/02 2:20p Khykin
// Updating the pulse handling for the unblock pulse.
//
// 33    9/24/02 3:32p Bmeinke
// Added MsgReply() to KrnlHandlePulse()'s _PULSE_CODE_UNBLOCK handling
//
// 32    4/03/02 7:34a Kh
// Adding methods to deal with pulses.
//
//
//*************************************************************************

#ifndef KRNL_QNX_C			/* IF the template has not been included? */
#define KRNL_QNX_C

/**************************** INCLUDES *************************************/
	#include <errno.h>
	#include <fcntl.h>
//	#include <i86.h>
	#include <limits.h>
	#include <process.h>
	#include <pthread.h>
	#include <setjmp.h>
	#include <signal.h>
	#include <stdarg.h>
	#include <stdio.h>
	#include <string.h>
	#include <sys/neutrino.h>
	#include <sys/mman.h>
	#include <sys/netmgr.h>
	#include <sys/dispatch.h>
	#include <sys/sched.h>
	#include <sys/stat.h>
	#include <time.h>
	#include <unistd.h>

	#include "BepDefs.h"
	#include "Bepos.h"
	#include "Krnl.h"

/****************************** DEFINES *************************************/

	#define KRNL_GLBLS

/************************** MODULE VARIABLES ********************************/

int             OldPriority;
int             OldSchedAlg;
timer_t         SemTimer;
int             DebugFlag=OFF;
BP_SEMAPHORE    *SemaphorePtr;
int             BposShmemSize;

void SigAlrmHandler (int sig_no);
void SigAbrtHandler (int sig_no);
static void* RelayFunction( void *arg);

/*************************** FUNCTION DEFINITIONS **************************/

/***************************************************************************/

int KrnlRegisterTaskName( const char *name)
{
	// QNX operating system only.
	int rc=BP_ERR_KRNL_FAIL;
	name_attach_t   *attach;

	// Register name & Connect to channel
	if((attach=name_attach( NULL, name, 0)) == NULL)
	{
		KrnlLog ("KrnlRegisterTaskname(%s) name_attach() error: %s\n", name, strerror( errno));
		rc = BP_ERR_KRNL_FAIL;
	}
	else
	{
		rc = (int)attach;
	}
	return(rc);
}

/**
 * Attaches the calling process to the given name
 *
 * @param name     Name to attach for this process
 * @param attach_t Pointer to the name_attach_t structure to return the attach
 *                 data in. If unable to attach the calling process with the given
 *                 name, this will be set to NULL upon exiting
 * @return SUCCESS if no errors occur, BEP_ERR_KRNL_FAIL on error
 */
int KrnlAttachName( const char *name, void **attach_t)
{
	// QNX operating system only.
	int 			rc=SUCCESS;
	name_attach_t   **attach = (name_attach_t**)attach_t;

	// Register name & Connect to channel
	if((*attach=name_attach( NULL, name, 0)) == NULL)
	{
		KrnlLog ("KrnlAttachName(%s) name_attach() error: %s\n", name, strerror( errno));
		rc = BP_ERR_KRNL_FAIL;
	}
	return(rc);
}

/***************************************************************************/

int KrnlUnRegisterTaskName (int name_id)
{
	// QNX operating system only.
	int rc=BP_ERR_KRNL_FAIL;
	name_attach_t   *attach = (name_attach_t*)name_id;

	rc = name_detach( attach, 0);
	if(rc == FAILURE)
		KrnlLog ("KrnlUnRegisterTaskname(): name_detach() error\n");
	else
		name_id	= 0;
	return( rc);
}


/***************************************************************************/

int KrnlGetTaskId( const char *capProcessName)
{	// Return channel Id associated with name
	INT32 status;
	INT32 counter=0;

	do
	{
		status = name_open(capProcessName, 0);
		pthread_testcancel();
	} while((counter++ < 10) && (status == -1) && (errno == EINTR));

	return(status);
}

/***************************************************************************/

int KrnlCloseTaskId(int chid)
{
	INT32 status;
	INT32 counter=0;

	do
	{
		status = name_close(chid);
		pthread_testcancel();
	} while((counter++ < 10) && (status == -1) && (errno == EINTR));

	return(status);
}

// ***********************************************************************
// Desc:	Gets the process ID for the calling process
// Input:	Nothing
// Returns:	Error = -1, else PID

int KrnlGetMyTaskId( void )
{
	return( getpid());
}


// ************************************************************************
// Desc:	Get process priority for given pid
// Input:	iPid	Process Id to get priority for.  If 0, then current process
// Returns:	Error = -1, else Priority

int KrnlGetPriority( int iPid )
{
	return( getprio( iPid));
}


// ************************************************************************
// Desc:	Set process priority for given pid
// Input:	iPid	Process Id to Set priority for.  If 0, then current process.
//			iPriority to set to
// Returns:	Error = -1, else Priority

int KrnlSetPriority( int iPid , int iPriority )
{
	return( setprio( iPid, iPriority));
}


/**
 * Sets the priority for the specified thread in the specified process
 *
 * @param iPid      Process ID of process in which the thread whose priority we want to
 *                  modify resides
 * @param iTid      Thread ID of the thread we want to set the priority of
 * @param iPriority New priority level for the specifiedc thread
 *
 * @return -1 if an error occurred (priority not changed) or previous priority
 *         of the thread
 * @since Version 1.0
 */
int KrnlSetThreadPriority( int iPid, int iTid, int iPriority )
{
	int					retVal = -1;
	struct sched_param	schedParam;

	// Read current scheduling parameters
	if( SchedGet( iPid, iTid, &schedParam) != -1)
	{
		// Hold onto current priority for return
		retVal = schedParam.sched_curpriority;

		// Set new priority
		schedParam.sched_priority = iPriority;
		if( SchedSet( iPid, iTid, SCHED_NOCHANGE, &schedParam) == -1)
		{
			retVal = -1;
		}
	}

	return( retVal);
}


// ***********************************************************************
// Desc:
// Input:
// Returns:	 0 = Success
//			-1 = Error

int KrnlSend( int iPid , char *capXmtBuff , char *capRcvBuff , int iXmtLen , int iRcvLen )
{
	INT32 status;
	INT32 counter=0;

	do
	{
		status = MsgSend( iPid, capXmtBuff, iXmtLen, capRcvBuff, iRcvLen);
		pthread_testcancel();
	} while((counter++ < 10) && (status == -1) && (errno == EINTR));

	return(status);
}


// Not Supported
// ***********************************************************************
// Desc:
// Input:
// Returns:	 Pid of sending process on success
//			-1 = Error

int KrnlCReceive( int iPid , char *capRcvBuff , int iRcvLen )
{
//	return( Creceive( iPid, capRcvBuff, iRcvLen));
	return( -1);
}


// ***********************************************************************
// Desc:
// Input:
// Returns:	 Pid of sending process on success
//			-1 = Error

int KrnlReceive( int iPid , char *capRcvBuff , int iRcvLen )
{
	INT32 status;
	INT32 counter=0;
	void *tempBuff = (void *)capRcvBuff;

	do
	{
//		status = MsgReceive(iPid, capRcvBuff, iRcvLen, NULL);
		status = MsgReceive(iPid, tempBuff, iRcvLen, NULL);
		pthread_testcancel();
	} while((counter++ < 10) && (status == -1) && (errno == EINTR));

	return(status);
}


// **************************************************************************
// Desc:
// Input:
// Returns:	 0 = Success
//			-1 = Error

int KrnlReply( int iPid , char *capXmtBuff , int iXmtLen )
{
	return( MsgReply( iPid, EOK, capXmtBuff, iXmtLen));
}



// ***********************************************************************
// Desc:	Opens share memory and returns pointer to its location.
// Input:	1 == Create and Open access to shared memory because it does not exist.
//			0 == Open access to shared memory only because it already exists.
// Returns:	Successful:		Character pointer to where shared memory is mapped to.
//			Unsuccessful:	Pointer value of -1.

char *KrnlCreateSharedMemory( const char *name, int size, int *filedesc)
{
	int     iAccess, iOpenType;
	void    *addr;
	char    memName[ FILENAME_MAX];

	addr = (void*)FAILURE;					// assume not successful

	iOpenType = O_RDWR | O_CREAT;			// read/write/create
	iAccess = S_IRWXU | S_IRWXG | S_IRWXO;	// give everyone access

	// Fix-up name for Neutrino shared memory
	memName[ 0] = 0;
	if(name [0] != '/')	   strcpy( memName, "/");
	strcat( memName, name);

	// I want access to shared memory, so open a handle for doing this
	*filedesc = shm_open (memName, iOpenType, (mode_t)iAccess);

	if(*filedesc != -1)
	{
		if(0 == ftruncate(*filedesc, size))
		{
			// Now map this much of shared memory into my area and get a pointer
			//	to where it begins in my memory.
			addr = mmap( 0, size, PROT_READ|PROT_WRITE, MAP_SHARED, *filedesc, 0);
			if(addr == MAP_FAILED)
			{
				// too bad
				KrnlLog ("KrnlCreateSharedMemory(), mmap() failed: %s\n", strerror( errno));
				addr = (void*)FAILURE;
			}
		}
		else
		{
			KrnlLog ("KrnlCreateSharedMemory(): ftruncate() failed\n");
		}
	}
	else
	{
		KrnlLog ("KrnlCreateSharedMemory(), shm_open() failed: %s\n", strerror( errno));
	}

	// return shared memory pointer/status
	return( (char*)addr);
}

// ***********************************************************************
// Desc:	Opens share memory and returns pointer to its location.
// Input:	1 == Create and Open access to shared memory because it does not exist.
//			0 == Open access to shared memory only because it already exists.
// Returns:	Successful:		Character pointer to where shared memory is mapped to.
//			Unsuccessful:	Pointer value of -1.

char *KrnlOpenSharedMemory( const char *name, int size, int *filedesc)
{
	int     iAccess, iOpenType;
	void    *addr;
	char    memName[ FILENAME_MAX];
	struct stat memStat;

	addr = (void*)FAILURE;					// assume not successful

	iOpenType = O_RDWR;						// read/write
	iAccess = S_IRWXU | S_IRWXG | S_IRWXO;	// give everyone access

	// Fix-up name for Neutrino shared memory
	memName[ 0] = 0;
	if(name [0] != '/')	   strcpy( memName, "/");
	strcat( memName, name);

	// I want access to shared memory, so open a handle for doing this
	*filedesc = shm_open( name, iOpenType, (mode_t)iAccess);

	if(*filedesc != -1)
	{
		// If no size specified
		if(size == 0)
		{
			// Determine size of memory object
			fstat( *filedesc, &memStat);
			size = memStat.st_size;
		}
		// Now map this much of shared memory into my area and get a pointer
		//	to where it begins in my memory.
		addr = mmap( 0, size, PROT_READ|PROT_WRITE, MAP_SHARED, *filedesc, 0);
		if(addr == MAP_FAILED)
		{
			// too bad
			KrnlLog ("KrnlOpenSharedMemory(), mmap() failed: %s\n", strerror( errno));
			KrnlReleaseSharedMemory( name, size, *filedesc);
			addr = (void*)FAILURE;
		}
	}
	else
	{
		KrnlLog ("KrnlOpenSharedMemory(), shm_open() failed: %s\n", strerror( errno));
	}

	// return shared memory pointer/status
	return( (char*)addr);
}


/**
 * Destroys a shared memory object that was opened by a successful call to
 * KrnlCreateSharedMemory().
 *
 * @param name     Pointer to the string containing the shared mem object name.
 * @param address  Pointer to the start of memory to be unmapped
 * @param size     Size of the shared memory object in bytes.
 * @param filedesc the file descriptor (handle) for the shared memory object.
 */
void KrnlDestroySharedMemory( const char *name, void* address, int size, int filedesc)
{
	struct stat memStat;
	char    memName[ FILENAME_MAX];

	// Fix-up name for Neutrino shaed memory
	memName[ 0] = 0;
	if(name [0] != '/')	   strcpy( memName, "/");
	strcat( memName, name);

	// If no size specified
	if(size == 0)
	{
		// Determine size of memory object
		fstat( filedesc, &memStat);
		size = memStat.st_size;
	}

	// Release the shared memory
	KrnlReleaseSharedMemory((char *)address, size, filedesc);

	// Remove the shared memory from QNX memory
	if(-1 == shm_unlink (memName))
		KrnlLog ("KrnlDestroyShmem(): shm_unlink() failed\n");
}


// **************************************************************************
// Desc:	Releases shared memory created by successful return from
//			QNXOpenSharedMemory().
// Input:	None.
// Returns:	None.

void KrnlReleaseSharedMemory( const char *address, int size, int filedesc)
{
	// Unmap the shared memory from my area
	(void) munmap((void*)address, size);
	// Close the shared memory handler
	(void) close(filedesc);
}


/***************************************************************************/

int KrnlSleep (long sleep_time)
{
// QNX operating system only.
	int rc = BP_ERR_KRNL_FAIL;

	rc = delay ((unsigned int)sleep_time);
	if(rc != SUCCESS)
		rc = BP_ERR_KRNL_FAIL;
	return(rc);
}


/***************************************************************************/

int KrnlKillTask (int task_id, short dealloc_mem)
{
// QNX operating system only.
	int rc = BP_ERR_KRNL_FAIL;
	int status;

	//kill task by setting a signal on it.
	status = kill ((pid_t)task_id, SIGTERM);

	if(0 == dealloc_mem)   dealloc_mem =0;

	if(status == 0)
		rc = SUCCESS;
	else
	{
		rc = BP_ERR_KRNL_FAIL;
		KrnlLog ("KrnlKillTask(): error code %d returned\n",status);
	}
	return(rc);
}


/***************************************************************************/

int KrnlCreateTask (const int algrthm, const int priority, int port, int mem,
					const char *command, const char *arg_str)
{
// QNX operating system only.
	int     rc = BP_ERR_KRNL_FAIL;
	int     ii, old_alg;
	int     process_id;				//process ID number for new process
	int     sched_alg;				//scheduling algorithm
	char    *arglist[MAX_ARGS];		//argument list built from arg_str
	char    *arg;					//individual argument from arg_str
	struct  sched_param sched_info;	//data structure for sched_setscheduler()


	ii = 0;
	arglist[ii++] = (char*)command;
	if(0 == port)  port = 0;
	if(0 == mem)   mem = 0;

	//Parse arg_str for individual arguments, put each argument into
	//arglist[] as a seperate string.  This is how spawnv() wants arguments
	//packaged.
	arg = strtok((char*)arg_str, " ");
	while(arg != NULL)
	{
		arglist[ii++] = arg;
		arg = strtok (NULL, " ");
	}
	arglist[ii] = NULL;

	//Attempt to create the new process.  The process will be started as a
	//background task.
	errno = EOK;
	process_id = spawnv(P_NOWAIT, command, arglist);

	switch(algrthm)
	{
	case ROUND_ROBIN:
		sched_alg = SCHED_RR; break;
	case FIFO:
		sched_alg = SCHED_FIFO; break;
	case ADAPTIVE:
		sched_alg = SCHED_OTHER; break;
	default:
		KrnlLog ("KrnlCreateTask(): invalid scheduling algorithm");
		sched_alg = INVALID_ALG;
	}

	//If process created ok, set priority and scheduling algorithm of the
	//new process
	if(process_id >= 0)
	{
		sched_info.sched_priority = priority;
		errno = EOK;
		old_alg = sched_setscheduler (process_id, sched_alg, &sched_info);
		if(old_alg != -1)
			rc = process_id;
		else
		{
			rc = BP_ERR_KRNL_FAIL;
			KrnlLog ("KrnlCreateTask(): sched_setscheduler() failure because: %s\n", strerror(errno));
		}
	}
	else
	{
		rc = BP_ERR_KRNL_FAIL;
		KrnlLog ("KrnlCreateTask(): spawnv() failure because: %s\n", strerror(errno));
	}
	return(rc);
}


/****************************************************************************/

void KrnlDebug (int action)
{
// QNX operating system only.

	DebugFlag = action;
}


/***************************************************************************/

int KrnlInit (BP_SEMAPHORE *sem_addr_from_bpos, int mem_size_from_bpos)
{
// QNX operating system only.
	int ok_to_cont, rc=FAILURE;

	signal (SIGALRM, SigAlrmHandler);
	signal (SIGABRT, SigAbrtHandler);

	SemTimer = KrnlCreateTimer();
	if(SemTimer >= 0)
		ok_to_cont = TRUE;
	else
	{
		ok_to_cont = FALSE;
		KrnlLog ("KrnlInit(): CreateTimer() failure\n");
	}

	SemaphorePtr = sem_addr_from_bpos;
	BposShmemSize = mem_size_from_bpos;

	if(ok_to_cont == TRUE)
		rc = SUCCESS;
	else
		rc = FAILURE;
	return(rc);
}


/***************************************************************************/

void KrnlClose (void)
{
// QNX operating system only.
	(void) timer_delete (SemTimer);
}


/***************************************************************************/

int KrnlLockTask (void)
{
// QNX operating system only.
	int     rc = BP_ERR_KRNL_FAIL;
	struct  sched_param sched_info;
	uid_t   uid;

	//get current scheduling algorithm - save for later restoration
	OldSchedAlg = sched_getscheduler (0);
	if(OldSchedAlg != -1)
	{
		//get current priority - save for later restoration
		OldPriority = getprio (0);
		if(OldPriority != -1)
		{
			//get user id.  It tells us if we are superuser or not.
			uid = getuid();
			//Set our priority to the maximum allowed.
			if(uid == SUPER_USER_UID)
				sched_info.sched_priority = MAX_SU_PRIORITY;
			else
				sched_info.sched_priority = MAX_NONSU_PRIORITY;
			rc = sched_setscheduler (0, SCHED_FIFO, &sched_info);
			if(rc >= 0)
				rc = SUCCESS;
			else
			{
				rc = BP_ERR_KRNL_FAIL;
				KrnlLog ("KrnlLockTask(): sched_setscheduler() error\n");
			}
			//get user id.  It tells us if we are superuser or not.
			uid = getuid();
			//Set our priority to the maximum allowed.
			if(uid == SUPER_USER_UID)
				sched_info.sched_priority = MAX_SU_PRIORITY;
			else
				sched_info.sched_priority = MAX_NONSU_PRIORITY;
		}
		else
		{
			rc = BP_ERR_KRNL_FAIL;
			KrnlLog ("KrnlLockTask(): getprio() error\n");
		}
	}
	else
	{
		rc = BP_ERR_KRNL_FAIL;
		KrnlLog ("KrnlLockTask(): sched_getscheduler() error\n");
	}
	return(rc);
}


/***************************************************************************/

int KrnlUnlockTask (void)
{
// QNX operating system only.
	int     rc = BP_ERR_KRNL_FAIL;
	struct  sched_param sched_info;

	//Restore the priority and sched. alg. of process previous to when
	//process was locked.
	sched_info.sched_priority = OldPriority;
	rc = sched_setscheduler (0, OldSchedAlg, &sched_info);
	if(rc >= 0)
		rc = SUCCESS;
	else
	{
		rc = BP_ERR_KRNL_FAIL;
		KrnlLog ("KrnlUnLockTask(): sched_setscheduler() error\n");
	}

	return(rc);
}


/***************************************************************************/

int KrnlSemTestWait (int sem_number)
{
// QNX operating system only.
	int   status, rc = BP_ERR_KRNL_FAIL;

	status = sem_trywait (&SemaphorePtr[sem_number]);
	if(status == 0)
		rc = SUCCESS;
	else if(errno == EAGAIN)
		rc = BP_ERR_SEM_LOCKED;
	else
	{
		rc = BP_ERR_KRNL_FAIL;
		KrnlLog ("KrnlSemTestWait(): sem_trywait() error\n");
	}

	return(rc);
}


/***************************************************************************/

int KrnlSemWait (int sem_number)
{
#if 0
	INT32 status;
	INT32 counter=0;
	// wait here until the semaphore is avaliable
	do
	{
		status = sem_wait(sem_number);
		// if an error occurrs and it is not a signal interrupt
		if((status == -1) && (errno != EINTR))
		{
			KrnlLog ("Could Not Wait On Semaphore: %s", strerror( errno ));
		}
		pthread_testcancel();
	}while((counter++ < 10) && (status == -1));


	return(status);
#endif
	return(-1);
}


/***************************************************************************/

int KrnlSemWaitWithTO (int sem_number, long timeout)
{
// QNX operating system only.
	int status, rc=BP_ERR_KRNL_FAIL;

	status = KrnlStartTimer (SemTimer, timeout, 0);
	if(status == SUCCESS)
	{
		status = sem_wait (&SemaphorePtr[sem_number]);
		if(status == 0)
			rc = SUCCESS;
		else if(errno == EINTR)
		{
			rc = BP_ERR_SEM_TIMEOUT;
			KrnlLog ("KrnlSemWaitWithTO(): semaphore interrupted\n");
		}
		else
		{
			rc = BP_ERR_KRNL_FAIL;
			KrnlLog ("KrnlSemWaitWithTO(): sem_wait() failed\n");
		}
		(void) KrnlStopTimer (SemTimer);
	}
	else
		rc = status;

	return(rc);
}


/***************************************************************************/

int KrnlSemSignal (int sem_number)
{
#if 0
	INT32 status;
	INT32 counter=0;
	// wait here until the semaphore is avaliable
	do
	{
		status = sem_post(sem_number);
		// if an error occurrs and it is not a signal interrupt
		if((status == -1) && (errno != EINTR))
		{
			KrnlLog ("Could Not Post A Semaphore: %s", strerror( errno ));
		}
		pthread_testcancel();
	}while((counter++ < 10) && (status == -1));

	return(status);
#endif
	return(-1);
}


/***************************************************************************/

int KrnlSemCreate (int sem_number, int sem_count)
{
// QNX operating system only.
	int status, rc=BP_ERR_KRNL_FAIL;

	status = sem_init (&SemaphorePtr[sem_number],1,sem_count);
	if(status == 0)
		rc = SUCCESS;
	else
	{
		rc = BP_ERR_KRNL_FAIL;
		KrnlLog ("KrnlSemCreate(): sem_init() error\n");
	}

	return(rc);
}


/***************************************************************************/

int KrnlSemDelete (int sem_number)
{
// QNX operating system only.
	int status, rc=BP_ERR_KRNL_FAIL;

	status = sem_destroy (&SemaphorePtr[sem_number]);
	if(status == 0)
		rc = SUCCESS;
	else
	{
		rc = BP_ERR_KRNL_FAIL;
		KrnlLog ("KrnlSemDelete(): sem_destroy() error\n");
	}
	return(rc);
}


/****************************************************************************/

timer_t KrnlCreateTimer (void)
{
	timer_t             timer_id;
	struct sigevent     tmout;

	tmout.sigev_signo = SIGALRM;
	if(timer_create (CLOCK_REALTIME, &tmout, &timer_id) == -1)
	{
		timer_id = -1;
	}
	return(timer_id);
}


/****************************************************************************/

timer_t KrnlCreateTimerWithProxy( int *ipProxy , pid_t iPid , char *cpMessage , int iBytes , int iPriority )
{
	timer_t             timer_id;
	struct sigevent     tmout;

	// Create the comm channel
	if((*ipProxy = KrnlCreateProxy(0, cpMessage, iBytes, iPriority)) == SUCCESS)
	{
		// Connect to our own channel
		if((tmout.sigev_coid = ConnectAttach( 0,0, *ipProxy, 0, 0)) != -1)
		{
			// Initialize the Timer to give us a Pulse
			SIGEV_PULSE_INIT(&tmout, *ipProxy, SIGEV_PULSE_PRIO_INHERIT,  iPid, 0);
			//  Create the OS timer object
			timer_create( CLOCK_REALTIME, &tmout, &timer_id);
		}
	}

	return( timer_id);
}


/****************************************************************************/

int KrnlStartTimer (timer_t timer_id, long iDelayTime , long iRepTime )
{
	struct itimerspec   timer;
	int                 status, rc = BP_ERR_KRNL_FAIL;

	timer.it_value.tv_sec = iDelayTime / 1000;
	timer.it_value.tv_nsec = (iDelayTime % 1000) * 1000000;
	timer.it_interval.tv_sec = iRepTime / 1000;
	timer.it_interval.tv_nsec = (iRepTime % 1000) * 1000000;
	status = timer_settime (timer_id, 0, &timer, NULL);
	if(status == SUCCESS)
		rc = SUCCESS;
	else
	{
		rc = BP_ERR_KRNL_FAIL;
		KrnlLog ("KrnlStartTimer(): timer_settime() failure\n");
	}
	return(rc);
}


/****************************************************************************/

int KrnlStopTimer (timer_t timer_id)
{
	struct itimerspec   timer;
	int                 status, rc = BP_ERR_KRNL_FAIL;

	timer.it_value.tv_sec = 0;
	timer.it_value.tv_nsec = 0;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_nsec = 0;
	status = timer_settime (timer_id, 0, &timer, NULL);
	if(status == SUCCESS)
		rc = SUCCESS;
	else
	{
		rc = BP_ERR_KRNL_FAIL;
		KrnlLog ("KrnlStopTimer(): timer_settime() failure\n");
	}
	return(rc);
}


/****************************************************************************/

int KrnlDeleteTimer (timer_t timer_id)
{
	return( timer_delete( timer_id));
}


/****************************************************************************/

int KrnlDeleteTimerWithProxy( timer_t timer_id , int iProxy )
{
	int     iStatus=SUCCESS;		// assume all is good

	if(SUCCESS != timer_delete( timer_id))	   // remove timer first cuz it uses proxy
	{
		// bummer
		iStatus = FAILURE;
	}
	if(SUCCESS != KrnlDeleteProxy( iProxy))	   // remove proxy last
	{
		// bummer
		iStatus = FAILURE;
	}

	return( iStatus);
}


/****************************************************************************/

pid_t KrnlCreateProxy( pid_t iPid , char *cpMessage , int iBytes , int iPriority )
{
	return( (pid_t)ChannelCreate( 0));
}


/****************************************************************************/

pid_t KrnlDeleteProxy( pid_t iProxy )
{
	return( ChannelDestroy( iProxy));
}


/***************************************************************************
@SF KrnlTriggerProxy()
@SF		Triggers a proxy that was created with BposCreateProxy()
@SF
@SF PARAMETERS:
@SF 	iProxy		Proxy ID that is to be triggered
@SF
@SF RETURNS:
@SF 	If successful	: PID that owns the proxy
@SF 	-1				: error
****************************************************************************/
pid_t KrnlTriggerProxy( pid_t iProxy )
{
	return( -1);
}


/****************************************************************************/

void KrnlActivateAbort (int *abort)
{
// QNX operating system only.
	pid_t   proc_group;

	proc_group = getpgrp();
	kill (-proc_group, SIGABRT);
	*abort = TRUE;
}

/****************************************************************************/

void KrnlClearAbort (int *abort)
{
//@@@S Clear BPOS abort flag

	*abort = FALSE;
}


/****************************************************************************/

void KrnlLog (char *format_str, ...)
{
// QNX operating system only.
	char    caBuff[1024];
	va_list vaArgs;

	// setup
	va_start (vaArgs, format_str);

	// generate the string
	vsprintf (caBuff, format_str, vaArgs);

	// send string to all the desired places
	{
		printf (caBuff);
		fflush (stdout);
	}

	// remove variable stuff
	va_end (vaArgs);
}

/*****************************************************************************
@SF KrnlSendmx()
@SF 	Sends a message to another task and waits for a reply message. See
@SF		KrnlReply().Allows for sending multi-part messages ((i.e. header/body)
@SF
@SF PARAMETERS:
@SF 	iPid		If zero then message will be accepted from any process,
@SF					else messages will be accepted from only that task indicated.
@SF		uSmsgParts	Number of entries in the spSmsg array
@SF		uRmsgParts	Number of entries in the spRmsg array
@SF		spSmsg		Message parts to be sent
@SF		spRmsg		Where to receive reply message
@SF
@SF RETURNS:
@SF 	-1	No message waiting or error.
@SF		0<	Process ID of task that sent the message.
*****************************************************************************/
int KrnlSendmx(int          iPid,
			   unsigned    uSmsgParts,
			   unsigned    uRmsgParts,
			   const void  *spSmsg,
			   const void  *spRmsg )
{
#if 0
	return( Sendmx(iPid,uSmsgParts,uRmsgParts,spSmsg,spRmsg));
#else
	return( -1);
#endif
}

/*****************************************************************************
@SF KrnlReceivemx()
@SF 	Wait until a message is received.Allows for receiving messages in more
@SF 	than one part (i.e. header/body)
@SF
@SF PARAMETERS:
@SF 	iPid		If zero then message will be accepted from any process,
@SF					else messages will be accepted from only that task indicated.
@SF 	uParts		Number of items in spRmsg
@SF 	spRmsg		Buffer to receive message parts into
@SF
@SF RETURNS:
@SF 	-1	No message waiting or error.
@SF		0<	Process ID of task that sent the message.
*****************************************************************************/
int KrnlReceivemx( pid_t        iPid,
				   unsigned     uParts,
				   void         *spRmsg)
{
#if 0
	return( Receivemx( iPid, uParts, spRmsg));
#else
	return( -1);
#endif
}

/*****************************************************************************
@SF KrnlReply()
@SF 	Sends a reply message to a task that is blocked from having sent a
@SF		message using the KrnlSend() function. Allows replying with multipart
@SF		messages (i.e. header/body).
@SF
@SF PARAMETERS:
@SF 	iPid		The process ID that the reply message is to be sent to.
@SF		uRmsgParts	Number of entries in the __rmsg array
@SF		spRmsg		POinter to message parts to reply with
@SF
@SF RETURNS:
@SF 	-1	Error
@SF		0	Success
*****************************************************************************/
int KrnlReplymx(pid_t       iPid,
				unsigned    uRmsgParts,
				void        *spRmsg )
{
#if 0
	return( Replymx(iPid, uRmsgParts, spRmsg ));
#else
	return( -1);
#endif
}

/*****************************************************************************
@SF KrnlCReceivemx()
@SF 	Checks to see if there is message being sent to the calling task.  The
@SF		function always returns whether or not there is a message being sent.
@SF		Allows for receiving multipart messages (i.e. header/body).
@SF
@SF PARAMETERS:
@SF 	iPid		If zero then message will be accepted from any process,
@SF					else messages will be accepted from only that task indicated.
@SF 	*capRcvBuff	Pointer to where response is to be placed.
@SF 	iRcvLen		Length of receive buffer.
@SF
@SF RETURNS:
@SF 	-1	No message waiting or error.
@SF		0<	Process ID of task that sent the message.
*****************************************************************************/
int KrnlCreceivemx(pid_t        iPid,
				   unsigned     sRmsgParts,
				   void         *spRmsg )
{
#if 0
	return( Creceivemx(iPid, sRmsgParts, spRmsg ));
#else
	return( -1);
#endif
}

/*****************************************************************************
@SF KrnlRelay()
@SF 	Relays a message received from process A to process B as if the
@SF     original message had been sent directly from Pprocess A to process B.
@SF
@SF PARAMETERS:
@SF 	iPid1		Process which originall ysent the message
@SF 	iPid2		PRocess to which the message will be relayed
@SF
@SF RETURNS:
@SF 	-1	Source or Target does not exist or aren't waiting for a message
@SF 	0	Message properly relayed from A to B
*****************************************************************************/
typedef struct
{
	pid_t   from;
	pid_t   to;
	int     len;
	char    *data;
} RelayArg;

int KrnlRelay(  pid_t iPid1, pid_t iPid2, int len, char *data)
{
	RelayArg    arg;

	// Fill in destination/source pid's
	arg.from = iPid1;   arg.to = iPid2;
	// Fill in source message data
	arg.len = len;      arg.data = data;

	// Spin a thread to complete the transaction
	ThreadCreate( 0, RelayFunction, &arg, NULL);
	return( 0);
}

void* RelayFunction( void *arg)
{
	RelayArg    *mssg = (RelayArg   *)arg;
	char        rxBuff[ 1024];
	struct      _msg_info   info;

	// Send to final destination
	MsgSend( mssg->to, mssg->data, mssg->len, rxBuff, 1024);
	// Get length of data from the MsgSend endpoint (valid bytes in rxBuff)
	MsgInfo( mssg->to, &info);
	// Send real data to the message originator
	MsgReply( mssg->from, EOK, rxBuff, info.msglen);

	return( 0);
}

/**
 * Creates a new IPC commujnicattion channel for the calling process
 *
 * @param flags  Flag controlling how the kernel will use the channel to notify
 *               the calling process for certain events (disconnected from
 *               client, thread death, etc)
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
int KrnlChannelCreate( unsigned int flags)
{
	return( ChannelCreate( flags));
}
/**
 * Destroys the channel identified by channelId. This mnakes the channel
 * unavailable for client connections.
 *
 * @param channelId Channel to destroy
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
int KrnlChannelDestroy( int channelId)
{
	return( ChannelDestroy( channelId));
}


/**
 * Creates a connection to an open channel
 *
 * @param nd     Node identifier (0 for local node)
 * @param pid    Process which owns the channel to connect to
 * @param chid   Channel ID to connect to
 * @param index  Starting value for connection ID to be used (use _NTO_SIDE_CHANNEL)
 * @param flags  Flag used to control how the channel will be treated across exec calls
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
int KrnlConnectAttach(unsigned long nd, pid_t pid, int chid, unsigned index, int flags)
{
	return( ConnectAttach( nd, pid, chid, index, flags));
}
/**
 * Closes an open connection
 *
 * @param connectId Connection to be closed
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
int KrnlConnectDetach( int connectId)
{
	return( ConnectDetach( connectId));
}

/**
 * Sends a pulse to a channel connection
 *
 * @param coid     Connection to send the pulse to
 * @param priority Priority at which to send the pulse
 * @param code     Pulse code to be sent
 * @param value    Value of the pulse to be sent
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
int KrnlSendPulse(int coid, int priority, int code, int value)
{
	// If no priority specified
	if(priority == -1)
	{
		// Use priority of process
		priority = getprio( 0);
	}
	return( MsgSendPulse( coid, priority, code, value));
}

/**
 * Creates a channel and connection to that channel within the calling process.
 * This is usefule when creating a timer with a pulse.
 *
 * @param channelId Buffer into which the newly created channel ID willbe stored
 * @param connectId Buffer into which the newly created connection ID willbe stored
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
int KrnlCreateSelfAttachment( int *channelId, int *connectId)
{
	int status = -1;

	if((*channelId=KrnlChannelCreate( 0)) != -1)
	{
		*connectId = KrnlConnectAttach(  ND_LOCAL_NODE,  0, *channelId,
										 _NTO_SIDE_CHANNEL, 0);
		if(*connectId == -1)
		{
			KrnlChannelDestroy( *channelId);
			KrnlLog("KrnlCreateSelfAttachment failed in KrnlConnectAttach\n");
		}
		else
		{
			status = 0;
		}
	}
	else
	{
		KrnlLog ("KrnlCreateSelfAttachment failed in KrnlChannelCreate\n");
	}

	return( status);
}

/**
 * Creates an OS timer and attches a Pulse to it.
 *
 * @param channelId Optional buffer to receive the newly created IPC channel id
 *                  (use NULL if no channel ID is needed)
 * @param connectId Optional buffer to receive the newly created IPC connection id
 *                  (use NULL if no channel ID is needed)
 * @param pulseCode Pulse code to be sent to your process when the timer expires
 * @return >=0 Timer ID if successful
 *         -1 if error (use errno for more error information)
 */
timer_t KrnlCreateTimerWithPulse( int *channelId, int *connectId, int pulseCode)
{
	timer_t             timer_id=-1;
	struct sigevent     tmout;
	int                 chId;

	// Create the comm channel and connect to it
	if(KrnlCreateSelfAttachment( &chId, &tmout.sigev_coid) != -1)
	{
		// Initialize the Timer to give us a Pulse
		SIGEV_PULSE_INIT(&tmout, tmout.sigev_coid, SIGEV_PULSE_PRIO_INHERIT,  pulseCode, 0);

		//  Create the OS timer object
		timer_create( CLOCK_REALTIME, &tmout, &timer_id);

		if(channelId != NULL)  *channelId = chId;
		if(connectId != NULL)  *connectId = tmout.sigev_coid;
	}
	else
	{
		KrnlLog( "KrnlCreateTimerWithPulse error: %s\n", strerror( errno));
	}

	return( timer_id);
}

/**
 * Destroys a previously created timer and it's associated pulse
 *
 * @param timerId   Timer to destroy
 * @param channelId IPC channel id use to deliver the pulse
 * @param connectId IPC connection id used for the channel
 * @return 0 if successful
 *         -1 if error (use errno for more error information)
 */
int KrnlDeleteTimerWithPulse( timer_t timerId, int channelId, int connectId)
{
	int retVal;

	if((retVal=KrnlConnectDetach( connectId)) != -1)
	{
		if((retVal=KrnlChannelDestroy( channelId)) != -1)
		{
			retVal = KrnlDeleteTimer( timerId);
		}
	}

	return( retVal);
}

/**
 * Creates a timer and aataches it to the given channel
 *
 * @param channelId Channel to attach the timer pulse to
 * @param connectId Connection ID for this attachment (NULL if not needed)
 * @param timerCode Timer Pulse code or signal number.
 * @param value     Timer Pulse value.
 * @return timer ID if successful
 *         -1 if error
 */
int KrnlAttachTimerToChannel( int channelId, int *connectId, int timerCode, int value)
{
	timer_t     timerId=-1;	   // timer ID for timer
	struct sigevent event;

	event.sigev_coid = KrnlConnectAttach(  ND_LOCAL_NODE, 0, channelId, _NTO_SIDE_CHANNEL, 0);

	if(event.sigev_coid != -1)
	{
		// set up the kind of event that we want to deliver -- a pulse
		SIGEV_PULSE_INIT( &event, event.sigev_coid, SIGEV_PULSE_PRIO_INHERIT, timerCode, value);

		// create the timer, binding it to the event
		if(timer_create( CLOCK_REALTIME, &event, &timerId) == -1)
		{
			KrnlLog("KrnlAttachTimerToChannel timer_create error: %s\n", strerror( errno));
			ConnectDetach( event.sigev_coid);
			timerId = -1;
		}
	}
	else
	{
		KrnlLog("KrnlAttachTimerToChannel ConnectAttach error: %s\n", strerror( errno));
	}

	if(connectId != NULL)
	{
		*connectId = event.sigev_coid;
	}

	return( timerId);

}

/**
 * Send an event to the specified process.  This is typically
 * used by a server process to send asynchronus information
 * to a client.
 *
 * @param rcvid  The ID to send the event to.
 * @param event  The event structure that contains the information for
 *               the recipient.
 * @return The status of the operation.
 */
int KrnlDeliverEvent(int rcvid, const struct sigevent* event )
{
	return(MsgDeliverEvent(rcvid, event));
}

/**
 * Send and event to the specified channel.
 *
 * @param coid     The connection to send the event to.
 * @param priority The priority to send the event at.
 * @param code     The code to associate with the event.
 * @param value    The value to associate with the event.
 * @return The status of the operation.
 */
int KrnlSendEvent(int coid, int priority, int code, int value )
{
	return(MsgSendPulse ( coid, priority, code, value ));
}

/**
 * Wait for an event to be sent to the specified channel.
 *
 * @param chid   The channel to wait on.
 * @param pulse  The pulse information that was received.
 * @param bytes  The number of bytes that were received.
 * @param info   Information about the pulse (not updated currently)
 * @return The status of the operation.
 */
int KrnlReceiveEvent(int chid, void* pulse, int bytes, struct _msg_info* info )
{
	return(MsgReceivePulse( chid, pulse, bytes, info ));
}

/**
 * Checks if a pulse is one of the standard OS pulses and handles it accordingly
 *
 * @param pulse  Pointer to the pulse data (struct _pulse*)
 * @return 1 if pulse not handled (not a standard pules)
 *         0 if pulse was handled
 *         -1 if error
 */
int KrnlHandlePulse( void* pulse)
{
	struct _pulse *pulseMsg = (struct _pulse *)pulse;
	int     retVal = BEP_STATUS_FAILURE;

	switch(pulseMsg->code)
	{
	case _PULSE_CODE_DISCONNECT:
		ConnectDetach( pulseMsg->scoid);
	case _PULSE_CODE_THREADDEATH:
		retVal = BEP_STATUS_SUCCESS;
		break;
	case _PULSE_CODE_UNBLOCK:
		// Client wants to unblock...so send an empty reply
		MsgError( pulseMsg->value.sival_int, ETIMEDOUT);
		retVal = BEP_STATUS_SUCCESS;
		break;
	case _PULSE_CODE_COIDDEATH:
	case _PULSE_CODE_NET_ACK:
		retVal = BEP_STATUS_SUCCESS;
	default:
		break;
	}

	return( retVal);
}
/*****************************************************************************
@SF KrnlYield()
@SF 	Yield CPU to another process at the same priority as the calling
@SF 	process.
@SF
@SF PARAMETERS:
@SF
@SF RETURNS:
*****************************************************************************/
void KrnlYield( void )
{
	SchedYield();
}

/**
 * Perform a command as if it were executed on the command line.
 *
 * @param command the command to execute on the command line.
 * @return -1 if and error occured
 */
int KrnlSystemCommand(const char *command)
{
	return(system(command));	// send the requested command
}

/**
 * Checks for the existance of a task in the system.
 *
 * @param taskName The name to check for.
 * @return The success or failure of the check for the tasks
 *         existance.
 * @since Version 1.2
 */
int KrnlIsTaskAlive(const char *taskName)
{
	int ii = 0;	// loop counter
	int status;	// status of the operation
	int	nameId;
	int isAlive = BEP_STATUS_ERROR;

	// First check if the name exists
	do
	{
		nameId = name_open( taskName, 0);
		pthread_testcancel();
	}
	while((nameId == -1) && (ii++ < 5));

	if(nameId != -1)	// if it existed
	{
		do
		{
			status = name_close( nameId);
		}
		while(status == EINTR);
		isAlive = BEP_STATUS_SUCCESS;	// light the cigars
	}

	return(isAlive);
}

/**
 * Gets the value of an environment variable
 *
 * @param var    Name of environment variable
 * @return The value of the environment variable (empty string if not found)
 * @since Version 17
 */
const char* KrnlGetEnv(const char* var)
{
	static char data[256];				// Holder for return value
	char* envData = NULL;				// Pointer to hold value from call to OS retreiving environment variable
	memset(data, 0, sizeof(data));		// Clear data
	envData = getenv(var);				// Get value of environment variable
	if(envData) strcpy(data, envData);	// If data exists copy it
	return(data);
}
/****************************************************************************/
/*********************** PRIVATE FUNCTION DEFINITIONS ***********************/
/****************************************************************************/

void SigAlrmHandler (int sig_no)
{
// QNX operating system only.

	KrnlLog ("SIGALRM handler %d!\n", sig_no);
}

/*****************************************************************************/

void SigAbrtHandler (int sig_no)
{
// QNX operating system only.

	KrnlLog ("SIGABRT handler %d!\n", sig_no);
}


/***************************************************************************/

	#if 0
long KrnlInstallIsr (short vector, long isr_ptr)
{
// PDOS operating system only.

	return(xvec(vector, isr_ptr));
}
	#endif


// *********************************************************************
#endif	// KRNL_QNX_C
// end

