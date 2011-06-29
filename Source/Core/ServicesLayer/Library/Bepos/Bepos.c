//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/Bepos/Bepos.c 3     3/03/05 1:02a Cward $
//
// FILE DESCRIPTION:
//  This module implements the Burke Porter Operating System.  These routines
//  allow application code to make standard operating system calls that are
//  independent of the operating system being used.  This allows application
//  code to be easily ported to different operating systems.
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
//    $Log: /Core/ServicesLayer/Library/Bepos/Bepos.c $
// 
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     8/18/04 11:21p Cward
// Removed path info from includes since makefile already points to the
// include directory
//
// 1     12/21/03 6:27p Cward
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
// 2     6/20/03 7:34a Khykin
// Added new method.
//
// 1     4/30/03 4:37p Bmeinke
// Migration to new personalized configuration management structure
//
// 22    4/23/03 2:45p Bmeinke
// Added BposSetThreadPriority() to set the priority of a single thread
//
// 21    11/08/02 8:23a Tbochene
// Added GetEnv()
//
// 20    8/23/02 7:53a Dan.erickson
// Changed 0(zero) to O in BposReadInt for Octal translation.
//
// 19    8/14/02 9:40a Skypig13
// Fixed spelling of BposSendEvent()
//
// 18    4/03/02 7:34a Kh
// Adding methods to deal with pulses.
//
//
//*************************************************************************

#ifndef		BEPOS_C
#define		BEPOS_C


/**************************** INCLUDES **************************************/
#include <sys/neutrino.h>
#include <inttypes.h>
#include <sys/syspage.h>

#include "BepDefs.h"
#include "Krnl.h"
#include "Bepos.h"


/**************************** DEFINES ***************************************/

#define BPOS_GLBLS
#define BPOS_OBJECT_ACTIVE  1   //indicates a software object is in use.

#define ABORT_TABLE_SIZE	20

/**************************** VARIABLES *************************************/

volatile BP_SEMAPHORE   *Semaphores;
volatile BP_MESSAGE_Q   *Msg_qs;
volatile int			*Msg_q_index;
volatile int			*Next_sem;
volatile int			*Abort;

SHARED_MEM				Bpos_mem;
int						TaskID;
int						Creator;

/********************** PRIVATE FUNCTION PROTOTYPES *************************/

/*****************************************************************************
@SF NextAvlblSmphr() returns the next available semaphore ID number.
@SF
@SF PARAMETERS:
@SF None.
@SF
@SF RETURNS:
@SF If success -> next available semaphore ID.
@SF Otherwise  -> error code (always negative - see bep.h)
*****************************************************************************/
int NextAvlblSmphr (void);

/*****************************************************************************
@SF GetBposSharedMemSize() returns the size (in bytes) of the shared memory
@SF area required for internal use by Bpos.
@SF
@SF PARAMETERS:
@SF None.
@SF
@SF RETURNS:
@SF Size of required shared memory object in bytes.
*****************************************************************************/
unsigned long GetBposSharedMemSize (void);

/*****************************************************************************
@SF SetupSharedMem() maps the Bpos data structures onto the Bpos shared
@SF memory object.
@SF
@SF PARAMETERS:
@SF *addr  - pointer to shared memory.
@SF
@SF RETURNS:
@SF None.
*****************************************************************************/
void SetupSharedMem (char *addr);


/****************************************************************************/
/************************ PUBLIC FUNCTION DEFINITIONS ***********************/
/****************************************************************************/

/***************************************************************************/

void ( *BposInstallSigHandler( int sig_no , void (*func)(int))) (int)
{
	return( signal (sig_no, func));
}

/***************************************************************************/

void ( *BposUninstallSigHandler( int sig_no)) (int)
{
	return( signal( sig_no, SIG_DFL));
}


/***************************************************************************/
/***************************************************************************/

int BposRegisterTaskName( const char *name)
{
	return( KrnlRegisterTaskName( name));
}

/**
 * Attaches the calling process to the given name
 *
 * @param name     Name to attach for this process
 * @param attach_t name_attach_t structure to return the attach data in.
 *                 If unable to attach the calling process with the given name,
 *                 this will be set to NULL upon exiting
 * @return SUCCESS if no errors occur, BEP_ERR_KRNL_FAIL on error
 */
int BposAttachName( const char *name, void **attach_t)
{
	return( KrnlAttachName( name, attach_t));
}

/***************************************************************************/

/*****************************************************************************
@SF BposReadInt( const char *numString)
@SF 	Reads am integer from a string buffer.
@SF 		A prefix of $, 0x, or 0X will be interpreted as hex
@SF 		A prefix of 0 will be interpretedas octal
@SF			All others will be interpreted as decimal
@SF
@SF PARAMETERS:
@SF 	Pointer to the buffer to read the number from
@SF
@SF RETURNS:
@SF		integer value of the string buffer
*****************************************************************************/
int BposReadInt( const char *numString)
{
	int		retVal;
	char 	*numPtr;

	// Check Hex
	if( (numPtr=strstr((char *)numString, "0x")) != NULL)
        sscanf( numPtr, "0x%X", &retVal);
	else if( (numPtr=strstr((char *)numString, "0X")) != NULL)
        sscanf( numPtr, "0X%X", &retVal);
	else if( (numPtr=strstr((char *)numString, "$")) != NULL)
        sscanf( numPtr, "$%X", &retVal);
	// Check octal (this is checking against an O 0x4F not a 0 0x30)
	else if( numString[0] == 'O')
        sscanf( numPtr, "%o", &retVal);
	// Default to decimal
	else
        retVal = atoi( numString);

	return( retVal);
}

/***************************************************************************/

int BposUnRegisterTaskName (int name_id)
{
	return ( KrnlUnRegisterTaskName ( name_id));
}

/***************************************************************************/

int BposGetTaskId( const char *capProcessName)
{
	return( KrnlGetTaskId( capProcessName));
}

/***************************************************************************/

int BposCloseTaskId( int chid)
{
	return( KrnlCloseTaskId( chid));
}

// ************************************************************************
// Desc:	Get process id with for the process name passed
// Input:
// Returns:	Error = -1, else PID

int BposGetMyTaskId( void )
{
	return( KrnlGetMyTaskId());
}


// ************************************************************************
// Desc:	Get process priority for given pid
// Input:	iPid	Process Id to get priority for.  If 0, then current process
// Returns:	Error = -1, else Priority

int BposGetPriority( int iPid )
{
	return( KrnlGetPriority( iPid));
}

// ************************************************************************
// Desc:	Set process priority for given pid
// Input:	iPid	Process Id to Set priority for.  If 0, then current process.
//			iPriority to set to
// Returns:	Error = -1, else Priority

int BposSetPriority( int iPid , int iPriority )
{
	return( KrnlSetPriority( iPid, iPriority));
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
int BposSetThreadPriority( int iPid , int iTid , int iPriority )
{
	return( KrnlSetThreadPriority( iPid, iTid, iPriority));
}



// **************************************************************************
// Desc:
// Input:
// Returns:	 0 = Success
//			-1 = Error

int BposSend( int iPid , char *capXmtBuff , char *capRcvBuff , int iXmtLen , int iRcvLen )
{
	return( KrnlSend( iPid, capXmtBuff, capRcvBuff, iXmtLen, iRcvLen));
}


// **************************************************************************
// Desc:
// Input:
// Returns:	 Pid of sending process on success
//			-1 = Error

int BposCReceive( int iPid , char *capRcvBuff , int iRcvLen )
{
	return( KrnlCReceive( iPid, capRcvBuff, iRcvLen));
}


// **************************************************************************
// Desc:
// Input:
// Returns:	 Pid of sending process on success
//			-1 = Error

int BposReceive( int iPid , char *capRcvBuff , int iRcvLen )
{
	return( KrnlReceive( iPid, capRcvBuff, iRcvLen));
}


// **************************************************************************
// Desc:
// Input:
// Returns:	 0 = Success
//			-1 = Error

int BposReply( int iPid , char *capXmtBuff , int iXmtLen )
{
    return( KrnlReply( iPid, capXmtBuff, iXmtLen));
}


/***************************************************************************/

char *BposCreateSharedMemory( const char *name, const int size, int *filedesc)
{
	char *ptr;

	ptr = KrnlCreateSharedMemory(name, size, filedesc);
	return (ptr);
}


/****************************************************************************/

char *BposOpenSharedMemory( const char *name, const int size, int *filedesc)
{
	char *ptr;

	ptr = KrnlOpenSharedMemory (name, size, filedesc);
	return (ptr);
}


/****************************************************************************/

void BposDestroySharedMemory( const char *name, void *address, const int size, int filedesc)
{
	KrnlDestroySharedMemory( name, address, size, filedesc);
}


/****************************************************************************/

void BposReleaseSharedMemory( char *address, const int size, int filedesc)
{
	KrnlReleaseSharedMemory( address, size, filedesc);
}


/****************************************************************************/

int BposSleep (const long sleep_time)
{
    int rc;

    rc = KrnlSleep ((long)sleep_time);
    return (rc);
}


/****************************************************************************/

int BposKillTask (const int task, const int dealloc_mem)
{
    int rc;

    rc = KrnlKillTask (task, dealloc_mem);
    return (rc);
}


/****************************************************************************/

int BposCreateTask (const int algrthm, const int priority, const int port,
					const int mem, const char *command, const char *arg_str)
{
    int rc;

	rc = KrnlCreateTask (algrthm, priority, port, mem, command, arg_str);
    return (rc);
}



/***************************************************************************/

unsigned long BposDebug (const enum DebugType action,
						 const unsigned long debug_bits)
{
    static long debug=OFF;

    if (action == DebugType_SET)
        debug |= debug_bits;
    else if (action == DebugType_CLEAR)
        debug &= ~debug_bits;

	if (debug & KRNL_DEBUG)
		KrnlDebug (ON);
	else
		KrnlDebug (OFF);

	return (debug & debug_bits);
}


/****************************************************************************/

int BposInit (const short task_no)
{
    int rc = FAILURE;
	int	status, unlock_status=FAILURE;

	TaskID = task_no;

	//@S Lock task so that only one task will create the BPOS shared memory
	//@S object.
	status = KrnlLockTask ();

	if (status == SUCCESS)
	{
		strcpy (Bpos_mem.name,"BPOS");
		Bpos_mem.size = GetBposSharedMemSize();

		//@S attempt to open the BPOS shared memory object
		Bpos_mem.addr = KrnlOpenSharedMemory (Bpos_mem.name, Bpos_mem.size,
												&Bpos_mem.filedesc);

		//@S if BPOS shared memory object could not be opened, it has
		//@S not yet been created, so create it now.
		if (Bpos_mem.addr == (char *)FAILURE)
		{
			Creator = YES;
			Bpos_mem.addr = KrnlCreateSharedMemory (Bpos_mem.name,
													Bpos_mem.size,
													&Bpos_mem.filedesc);
			if (Bpos_mem.addr == (char *)FAILURE)
				status = FAILURE;
			else
				status = SUCCESS;
		}
		else
		{
			status = SUCCESS;
			Creator = NO;
		}

		//@S If BPOS shared memory was created or opened successfully,
		//@S setup and initialize the shared memory object.
		if (status == SUCCESS)
		{
			SetupSharedMem (Bpos_mem.addr);
			status = KrnlInit((BP_SEMAPHORE*)Semaphores, Bpos_mem.size);

			//@S If kernel successfully intialized and this task created
			//@S the BPOS shared memory, than intialize the BPOS system.
			//@S Only one task should do this, which is why we check the
			//@S Creator variable.
			if ((status == SUCCESS) && (Creator == YES))
			{
				BposClearAbort ();
				*Msg_q_index = 0;
				*Next_sem = BPOS_USR_SEMS + BPOS_MSGQ_SEMS - 1;
				//@S Make sure the Message queue mutex is
				//@S available.
				status = BposSemCreate (BPOS_Qs_AVLBL, 1);
			}
		}
		//@S Unlock the task so normal scheduling is restored.
		// 'unlock status' prevents us from overwriting an error returned
		//  in 'status' by a previous function call.
		unlock_status = KrnlUnlockTask();
	}
	if ((unlock_status != SUCCESS) || (status != SUCCESS))
		rc = FAILURE;
	else
		rc = SUCCESS;

	return (rc);
}


/****************************************************************************/

void BposClose (void)
{
	KrnlClose();

	//@S If this task created the Bpos shared memory object, then it
	//@S should destroy it.  Otherwise, just detach from it.
	if (Creator == YES)
	{
		KrnlDestroySharedMemory (Bpos_mem.name,
								 Bpos_mem.addr,
								 Bpos_mem.size,
								 Bpos_mem.filedesc);
		BposLog ("Destroying BPOS shared memory\n");
	}
	else
		KrnlReleaseSharedMemory (Bpos_mem.addr, Bpos_mem.size,
								 Bpos_mem.filedesc);
}


/****************************************************************************/

int BposLockTask (void)
{
    int rc;

    rc = KrnlLockTask ();
    return (rc);
}

/****************************************************************************/

int BposUnlockTask (void)
{
    int rc;

    rc = KrnlUnlockTask ();
    return (rc);
}

/****************************************************************************/

int BposSemTestWait (const int sem_number)
{
	int rc;

    //@S IF semaphore ID is valid? THEN destroy the semaphore.
    if ((sem_number >= 0) && (sem_number < BPOS_MAX_SEMS))
	{
		rc = KrnlSemTestWait (sem_number);
	}
	else
	{
		rc = BP_ERR_SEM_FAIL;
	}
	return (rc);
}

/****************************************************************************/

int BposSemWait (const int sem_number)
{
	int rc;

    //@S IF semaphore ID is valid? THEN destroy the semaphore.
    if ((sem_number >= 0) && (sem_number < BPOS_MAX_SEMS))
	{
		rc = KrnlSemWait (sem_number);
	}
	else
	{
		rc = BP_ERR_SEM_FAIL;
	}
	return (rc);
}

/****************************************************************************/

int BposSemWaitWithTO (const int sem_number, const long timeout)
{
	int rc;

    //@S IF semaphore ID is valid? THEN destroy the semaphore.
    if ((sem_number >= 0) && (sem_number < BPOS_MAX_SEMS))
	{
		rc = KrnlSemWaitWithTO (sem_number, timeout);
	}
	else
	{
		rc = BP_ERR_SEM_FAIL;
	}
	return (rc);
}


/****************************************************************************/

int BposSemSignal (const int sem_number)
{
	int rc;

    //@S IF semaphore ID is valid? THEN destroy the semaphore.
    if ((sem_number >= 0) && (sem_number < BPOS_MAX_SEMS))
	{
		rc = KrnlSemSignal (sem_number);
	}
	else
	{
		rc = BP_ERR_SEM_FAIL;
	}
	return (rc);
}


/****************************************************************************/

int BposSemCreate (const int sem_number, const int sem_count)
{
	int rc, real_sem_number;
    int status = FAILURE;

    //@S IF semaphore ID is valid? THEN create the semaphore.
    if (sem_number < BPOS_MAX_SEMS)
    {
        if (sem_number < 0)
        {
            real_sem_number = NextAvlblSmphr();
            if (real_sem_number < 0)
                status = BP_ERR_SMPHRS_EXHSTD;
            else
                status = SUCCESS;
        }
        else
        {
            real_sem_number = sem_number;
            status = SUCCESS;
        }
        if (status == SUCCESS)
        {
   		    status = KrnlSemCreate (real_sem_number, sem_count);
   		    if (status == 0)
   		        rc = real_sem_number;
   		    else
   		        rc = status;
   		}
   		else
   		    rc = status;
   	}
	else
	{
		rc = BP_ERR_SEM_FAIL;
	}
	return (rc);
}


/****************************************************************************/

int BposSemDelete (const int sem_number)
{
	int rc;

    //@S IF semaphore ID is valid? THEN destroy the semaphore.
    if ((sem_number >= 0) && (sem_number < BPOS_MAX_SEMS))
	{
		rc = KrnlSemDelete (sem_number);
	}
	else
	{
		rc = BP_ERR_SEM_FAIL;
	}
	return (rc);
}


/****************************************************************************/

int BposCreateTimer( void )
{
	return( (int)KrnlCreateTimer());
}


/****************************************************************************/

int BposCreateTimerWithProxy( int *ipProxy , pid_t iPid , char *cpMessage , int iBytes , int iPriority )
{
	return( (int)KrnlCreateTimerWithProxy( ipProxy, iPid, cpMessage, iBytes, iPriority));
}


/****************************************************************************/

int BposStartTimer (timer_t timer_id, long iDelayTime , long iRepTime )
{
	return( KrnlStartTimer( timer_id, iDelayTime, iRepTime));
}


/****************************************************************************/

int BposStopTimer (timer_t timer_id)
{
	return( KrnlStopTimer( timer_id));
}


/****************************************************************************/

int BposDeleteTimer (timer_t timer_id)
{
	return( KrnlDeleteTimer( timer_id));
}


/****************************************************************************/

int BposDeleteTimerWithProxy( timer_t timer_id , int iProxy )
{
	return( KrnlDeleteTimerWithProxy( timer_id, iProxy));
}


/***************************************************************************
@SF BposCreateProxy()
@SF		Creates a proxy that can be triggered to send a message to a task.
@SF
@SF PARAMETERS:
@SF 	iPid		Process (task) ID to receive the message
@SF 	cpMessage	Pointer to message that is to be sent
@SF 	iBytes		Length of the message in bytes
@SF 	iPriority	Priority of the proxy.  -1 == priority of calling process
@SF
@SF RETURNS:
@SF 	If successful	: Proxy ID
@SF 	else			: error code (see bep.h).
****************************************************************************/
pid_t BposCreateProxy( pid_t iPid , char *cpMessage , int iBytes , int iPriority )
{
	return( KrnlCreateProxy( iPid, cpMessage, iBytes, iPriority));
}


/***************************************************************************
@SF BposDeleteProxy()
@SF		Deletes a proxy that was created with BposCreateProxy()
@SF
@SF PARAMETERS:
@SF 	iProxy		Proxy ID that is to be deleted
@SF
@SF RETURNS:
@SF 	If successful	: 0
@SF 	else			: error code (see bep.h).
****************************************************************************/
pid_t BposDeleteProxy( pid_t iProxy )
{
	return( KrnlDeleteProxy( iProxy));
}


/***************************************************************************
@SF BposTriggerProxy()
@SF		Triggers a proxy that was created with BposCreateProxy()
@SF
@SF PARAMETERS:
@SF 	iProxy		Proxy ID that is to be triggered
@SF
@SF RETURNS:
@SF 	If successful	: PID that owns the proxy
@SF 	-1				: error
****************************************************************************/
pid_t BposTriggerProxy( pid_t iProxy )
{
	return( KrnlTriggerProxy( iProxy));
}


/****************************************************************************/

int BposCreateMsgq (const short q_id_param, const short q_type,
					const short msg_cnt, const short msg_size,
					const long timeout)
{
	int rc = 0;
    short space_avlbl_sem = 0;
    short data_avlbl_sem = 0;
	int q_id;

	//@S IF q_id  valid? ELSE RETURN (BP_ERR_MSGQ_ID)
    if ((q_id_param < 0) || (q_id_param >= BPOS_MAX_MSGQs))
		rc = BP_ERR_MSGQ_ID;
	//@S IF the system is to
	else if ((q_id_param == 0) && (*Msg_q_index >= BPOS_MAX_MSGQs))
		rc = BP_ERR_MSGQ_EXH;
	//@S IF the q_typ is valid? ELSE RETURN (BP_ERR_MSGQ_TYP)
	else if ((q_type != BPOS_MSGQ_SYNC) && (q_type != BPOS_MSGQ_ASYNC))
		rc = BP_ERR_MSGQ_TYP;
	//@S IF the msg_cnt is valid? ELSE RETURN (BP_ERR_QMSG_SZ)
	else if ((msg_cnt <= 0) || (msg_cnt > BPOS_MAX_MSGQ_LNGTH))
		rc = BP_ERR_MSGQ_CNT;
	//@S IF the msg_sz is valid? ELSE RETURN (BP_ERR_MSGQ_SZ)
	else if ((msg_size <= 0) || (msg_size > BPOS_MAX_QMSG))
		rc = BP_ERR_QMSG_SZ;
	else
	{
		if (q_id_param == 0)
			q_id = (*Msg_q_index)++;
		else
			q_id = q_id_param;
		//@S Create a semaphore with no signals for indicating
		//@S data is available in the queue.
		data_avlbl_sem = NextAvlblSmphr ();
		rc = BposSemCreate (data_avlbl_sem, 0);
		//@S IF the queue is a synchronous queue THEN...
		//@S     Create semaphore indicating that space is
		//@S     available with the appropriate number of
		//@S     signals on it.
		if (q_type == BPOS_MSGQ_SYNC)
		{
			space_avlbl_sem = NextAvlblSmphr ();
			rc = BposSemCreate (space_avlbl_sem,msg_cnt-1);
		}
		//@S IF we were successful creating the
		//@S semaphores? THEN...
		if (rc != SUCCESS)
		{
			BposLog ("BposCreateMsgq - ERROR!!!\n");
		}
		else
		{
			//@S WAIT for access to the critical queue
			//@S management data areas.
			rc = BposSemWait (BPOS_Qs_AVLBL);
			if (rc != SUCCESS)
			{
				BposLog ("BposCreateMsgq - Error waiting for avail. queue!\n");
				//@S Set the queue sz and message length.
			}
			else
			{
				Msg_qs [q_id].q_lngth = msg_cnt;
				Msg_qs [q_id].msg_size = msg_size;

				//@S Set timeout period and queue type.
				//@S Set IN to zero and OUT to point to
				//@S the last thing item in the queue.
				Msg_qs [q_id].timeout = timeout;
				Msg_qs [q_id].q_typ = q_type;
				Msg_qs [q_id].in = 0;
				Msg_qs [q_id].out = 0;

				//@S Assign the semaphore ID's and
				//@S activate the queue.
				Msg_qs [q_id].space_avlbl = space_avlbl_sem;
				Msg_qs [q_id].data_avlbl = data_avlbl_sem;
				Msg_qs [q_id].actv = BPOS_OBJECT_ACTIVE;

				//@S SIGNAL that access to the queue
				//@S data areas is now available.
				rc = BposSemSignal (BPOS_Qs_AVLBL);
				if (rc == SUCCESS)
				{
					if (BposDebug (DebugType_TEST,BPOS_DEBUG))
						BposLog ("BposCreateMsgq - Successful!\n");
					rc = q_id;
				}
			}
		}
	}
	return (rc);
}

/****************************************************************************/

void BposSetQueueTimeout (short q_id, long timeout)
{
	Msg_qs [q_id].timeout = timeout;
}

/****************************************************************************/

int BposSendQmsg (const short q_id, const unsigned char *msg)
{
	int rc = SUCCESS;
    short msg_q_ovrn = FALSE;

//	BposLog("send msgQ addr = %X	q_id = %d  \n",&Msg_qs[q_id],q_id);

	//@S IF the q_id is valid? THEN...  ELSE RETURN (BP_ERR_MSGQ_ID)
    if ((q_id <= 0) || (q_id > BPOS_MAX_MSGQs))
		rc = BP_ERR_MSGQ_ID;
	//@S ELSE IF the Msg_qs[q_id] is ACTIVE? THEN...
	//@S ELSE RETURN (BP_ERR_MSGQ_NACT)
	else if (Msg_qs [q_id].actv != BPOS_OBJECT_ACTIVE)
		rc = BP_ERR_Q_NACT;
	else
	{
		//@S IF the queue is synchronous? THEN...
		//@S     WAIT for queue space available.
		if (Msg_qs[q_id].q_typ == BPOS_MSGQ_SYNC)
		{
//  	        BposLog ("Send to SYNC Q - Semaphore count = %d\n",
//                        Semaphores[Msg_qs[q_id].space_avlbl].count);
			rc = BposSemWait (Msg_qs[q_id].space_avlbl);
		}

		if (rc == SUCCESS)
		{

			//@S WAIT for access to critical queue managment data areas.
			//@S     Increment the input pointer
			rc = BposSemWait (BPOS_Qs_AVLBL);
			if (rc == SUCCESS)
			{
				//@S check for zero divide first.
				//@S We don't want to kill a task!
				if(Msg_qs[q_id].q_lngth>0)
				{
					Msg_qs[q_id].in = ((++Msg_qs[q_id].in) %
									   Msg_qs[q_id].q_lngth);

					//@S IF input pointer was equal to the output pointer
					//@S     THEN... Msg Q overrun <- TRUE
					if (Msg_qs[q_id].in == Msg_qs[q_id].out)
						msg_q_ovrn = TRUE;

					//@S Copy the msg to the queue.
					memmove ((void *)&(Msg_qs[q_id].data[Msg_qs[q_id].in]),
							 (void *) msg, Msg_qs[q_id].msg_size);

					//@S SIGNAL that access to queue data area is
					//@S		available.
					//@S SIGNAL that data is now availble in the queue.
					rc = BposSemSignal (BPOS_Qs_AVLBL);
					if (rc == SUCCESS)
					{
						rc = BposSemSignal (Msg_qs[q_id].data_avlbl);
						if (rc == SUCCESS)
						{
							//@S IF Msg Q Overrun = TRUE
							//@S THEN... Report it to someone.
							if (msg_q_ovrn == TRUE)
								 BposLog ("BposSendQmsg - Q %d OVERRUN!!!\n",
										  q_id);
							rc = SUCCESS;

						}
					}
				}
				//@S ENDIF the Msg_qs[q_id] is not setup correctly.
				else rc = BP_ERR_Q_NCOR;
			}
		}
	}

	return(rc);
}

/****************************************************************************/

int BposGetQmsg (const short q_id, unsigned char *msg_rcvd)
{
   int rc;

    if (BposDebug (DebugType_TEST, BPOS_DEBUG))
    	BposLog ("get msgQ addr = %X	q_id = %d  \n",&Msg_qs[q_id],q_id);
	//@S IF the q_id is valid? THEN...  ELSE RETURN (BP_ERR_MSGQ_ID)
    if ((q_id <= 0) || (q_id > BPOS_MAX_MSGQs))
		rc = BP_ERR_MSGQ_ID;
	//@S IF the Msg_qs[q_id] is ACTIVE? THEN...
	//@S ELSE RETURN (BP_ERR_MSGQ_NACT)
	else if (Msg_qs [q_id].actv != BPOS_OBJECT_ACTIVE)
		rc = BP_ERR_Q_NACT;

	else
	{
		//@S IF the parameters are assumed to be valid do...
		//@S WAIT for message queue message to be available.
		//@S WAIT for access to the critical queue managment data areas.

		if (Msg_qs[q_id].timeout == 0)
			rc = BposSemWait (Msg_qs[q_id].data_avlbl);
		else
			rc = BposSemWaitWithTO (Msg_qs[q_id].data_avlbl,
									Msg_qs[q_id].timeout);
		if (rc == SUCCESS)
		{
			//@S NOTE!!! If two tasks are waiting for data there is NO
			//@S guarantee which task will get the next data element in
			//@S the queue. The software will guarantee that there is data
			//@S available though.
			rc = BposSemWait (BPOS_Qs_AVLBL);
			if (rc == SUCCESS)
			{
				//@S check for zero divide first. We don't want to kill
				//@S a task!
				if (Msg_qs[q_id].q_lngth > 0)
				{
					//@S Increment the output pointer
					Msg_qs[q_id].out =
							((++Msg_qs[q_id].out) % Msg_qs[q_id].q_lngth);

					//@S Copy the message from the queue to the
					//@S	target data area.
					memmove ((void *) msg_rcvd,
						 (void *)(Msg_qs[q_id].data[Msg_qs[q_id].out]),
						 Msg_qs[q_id].msg_size);

					//@S SIGNAL that access to the queue data area is
					//@S now available.
					rc = BposSemSignal (BPOS_Qs_AVLBL);
					if (rc == SUCCESS)
					{
						//@S IF the queue is synchronous THEN signal that
						//@S space is available.
						if (Msg_qs[q_id].q_typ == BPOS_MSGQ_SYNC)
							rc = BposSemSignal (Msg_qs[q_id].space_avlbl);
					}
				}
				//@S ENDIF the Msg_qs[q_id] is not setup correctly.
				else rc = BP_ERR_Q_NCOR;
			}
		}
	}

	return(rc);
}


/****************************************************************************/

void BposActivateAbort (void)
{
int	*iAbort;

	iAbort = (int *)Abort;

	if (BposDebug (DebugType_TEST,BPOS_DEBUG))
		BposLog ("BposActivateAbort started.\n");
	KrnlActivateAbort ( iAbort);
}


/****************************************************************************/

void BposClearAbort (void)
{
int	*iAbort;

	iAbort = (int *)Abort;

	//@S Clear BPOS abort flag
	if (BposDebug (DebugType_TEST,BPOS_DEBUG))
		BposLog ("BposClearAbort() started.\n");
	KrnlClearAbort (iAbort);
}


/****************************************************************************/

int BposCheckAbortStatus (void)
{
	return (*Abort);
}


/****************************************************************************/

void BposLog (const char *format_str, ...)
{
	char caBuff[256];
	va_list	vaArgs;

	va_start (vaArgs, format_str);


	vsprintf (caBuff, format_str, vaArgs);
	KrnlLog (caBuff);


	va_end (vaArgs);
}


/****************************************************************************/
/*********************** PRIVATE FUNCTION DEFINITIONS ***********************/
/****************************************************************************/

int NextAvlblSmphr (void)
{
    return ((*Next_sem)--);
}


/****************************************************************************/

unsigned long GetBposSharedMemSize (void)
{
    unsigned long mem_size = 0;

    //Message queue memory allocation
    mem_size += sizeof (BP_MESSAGE_Q) * BPOS_MAX_MSGQs;
	//Message queue array index memory allocation
	mem_size += sizeof (int);
    //Semaphore memory allocation
    mem_size += sizeof (BP_SEMAPHORE) * BPOS_MAX_SEMS;
    //'Next semaphore' memory allocation
    mem_size += sizeof (int);
	//Abort flag memory allocation
	mem_size += sizeof (int);
    return (mem_size);
}

/****************************************************************************/

void SetupSharedMem (char *addr)
{
    char *new_addr;

    new_addr = addr;
    Msg_qs = (BP_MESSAGE_Q *)new_addr;
    new_addr += sizeof (BP_MESSAGE_Q) * BPOS_MAX_MSGQs;
    Semaphores = (BP_SEMAPHORE *) new_addr;
    new_addr += sizeof (BP_SEMAPHORE) * BPOS_MAX_SEMS;
    Next_sem = (int *)new_addr;
    new_addr += sizeof (int);
	Abort = (int *)new_addr;
	new_addr += sizeof (int);
	Msg_q_index = (int *)new_addr;
	new_addr += sizeof (int);
}



/*****************************************************************************
@SF BposSendmx()
@SF 	Sends a message to another task and waits for a reply message. See
@SF		BposReply().Allows for sending multi-part messages ((i.e. header/body)
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
int BposSendmx(int          iPid,
				unsigned    uSmsgParts,
				unsigned    uRmsgParts,
				const void	*spSmsg,
				const void	*spRmsg )
{
	return( KrnlSendmx(iPid,uSmsgParts,uRmsgParts,spSmsg,spRmsg));
}

/*****************************************************************************
@SF BposReceivemx()
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
int BposReceivemx( pid_t        iPid,
                   unsigned     uParts,
                   void			*spRmsg)
{
	return( KrnlReceivemx( iPid, uParts, spRmsg));
}

/*****************************************************************************
@SF BposReply()
@SF 	Sends a reply message to a task that is blocked from having sent a
@SF		message using the BposSend() function. Allows replying with multipart
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
int BposReplymx(pid_t       iPid,
				unsigned    uRmsgParts,
				void		*spRmsg )
{
	return( KrnlReplymx(iPid, uRmsgParts, spRmsg ));
}

/*****************************************************************************
@SF BposCReceivemx()
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
int BposCreceivemx(pid_t        iPid,
                   unsigned     sRmsgParts,
                   void			*spRmsg )
{
	return( KrnlCreceivemx(iPid, sRmsgParts, spRmsg ));
}

/*****************************************************************************
@SF BposRelay()
@SF 	Relays a message received from process A to process B as if the
@SF     original message had been sent directly from Pprocess A to process B.
@SF
@SF PARAMETERS:
@SF 	iPid1		Process which originall ysent the message
@SF 	iPid2		Process to which the message will be relayed
@SF 	len			Number of bytes to send to destination
@SF 	data		Buffer to send to the destination
@SF
@SF RETURNS:
@SF 	-1	Source or Target does not exist or aren't waiting for a message
@SF 	0	Message properly relayed from A to B
*****************************************************************************/
int BposRelay(	pid_t iPid1, pid_t iPid2, int len, char *data)
{
	return( KrnlRelay(	iPid1, iPid2, len, data));
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
int BposChannelCreate( unsigned int flags)
{
	return( KrnlChannelCreate( flags));
}

/**
 * Destroys the channel identified by channelId. This mnakes the channel
 * unavailable for client connections.
 *
 * @param channelId Channel to destroy
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
int BposChannelDestroy( int channelId)
{
	return( KrnlChannelDestroy( channelId));
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
int BposConnectAttach(unsigned long nd, pid_t pid, int chid, unsigned index, int flags)
{
	return( KrnlConnectAttach(nd, pid, chid, index, flags));
}

/**
 * Closes an open connection
 *
 * @param connectId Connection to be closed
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
int BposConnectDetach( int connectId)
{
	return( KrnlConnectDetach( connectId));
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
int BposSendPulse(int coid, int priority, int code, int value)
{
	return( KrnlSendPulse(coid, priority, code, value));
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
int BposCreateSelfAttachment( int *channelId, int *connectId)
{
	return( KrnlCreateSelfAttachment( channelId, connectId));
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
timer_t BposCreateTimerWithPulse( int *channelId, int *connectId, int pulseCode)
{
	return( KrnlCreateTimerWithPulse( channelId, connectId, pulseCode));
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
int BposDeleteTimerWithPulse( timer_t timerId, int channelId, int connectId)
{
	return( KrnlDeleteTimerWithPulse( timerId, channelId, connectId));

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
int BposAttachTimerToChannel( int channelId, int *connectId, int timerCode, int value)
{
	return( KrnlAttachTimerToChannel( channelId, connectId, timerCode, value));
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
int BposDeliverEvent(int rcvid, const struct sigevent* event )
{
	return(KrnlDeliverEvent(rcvid, event));
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
int BposSendEvent(int coid, int priority, int code, int value )
{
	return(KrnlSendEvent(coid, priority, code, value));
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
int BposReceiveEvent(int chid, void* pulse, int bytes, struct _msg_info* info )
{
	return(KrnlReceiveEvent(chid, pulse, bytes, info ));
}

/**
 * Checks if a pulse is one of the standard OS pulses and handles it accordingly
 *
 * @param pulse  Pointer to the pulse data (struct _pulse*)
 * @return 1 if pulse not handled (not a standard pules)
 *         0 if pulse was handled
 *         -1 if error
 */
int BposHandlePulse( void* pulse)
{
	return( KrnlHandlePulse( pulse));
}

/*****************************************************************************
@SF BposYield()
@SF 	Yield CPU to another process at the same priority as the calling
@SF 	process.
@SF
@SF PARAMETERS:
@SF
@SF RETURNS:
*****************************************************************************/
void BposYield( void )
{
	KrnlYield();
}


/**
 * Perform a command as if it were executed on the command line.
 *
 * @param command the command to execute on the command line.
 * @return -1 if and error occured
 */
int BposSystemCommand(const char *command)
{
	return( KrnlSystemCommand(command));
}

/**
 * Checks for the existance of a task in the system.
 *
 * @param taskName The name to check for.
 * @return The success or failure of the check for the tasks
 *         existance.
 * @since Version 1.2
 */
int BposIsTaskAlive(const char *taskName)
{
	return( KrnlIsTaskAlive(taskName));
}

/**
 * Gets the value of an environment variable
 *
 * @param var    Name of environment variable
 * @return The value of the environment variable (empty string if not found)
 * @since Version 17
 */
const char* BposGetEnv(const char *var)
{
	return(KrnlGetEnv(var));
}


double BposSecondsSinceBootUp( void )
{
	uint64_t	cycles, cps;
	double		sec;

	cycles = ClockCycles();

	cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
	sec = (double)cycles/cps;

	return(sec);
}

#endif	// BEPOS_C
// end

