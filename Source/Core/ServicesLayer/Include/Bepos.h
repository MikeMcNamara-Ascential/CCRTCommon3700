//*************************************************************************
// Description:
//  This module defines the Burke Porter Operating System.  These routines
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
//    $Log: /Core/ServicesLayer/Include/Bepos.h $
// 
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     6/24/04 12:51a Cward
// Removed pound if 0 block. Updated the header comment block to the new
// standard.
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 2     6/20/03 8:48a Khykin
// Latest changes.
//
// 1     4/30/03 4:41p Bmeinke
// Migration to new personalized configuration management structure
//
// 18    4/23/03 2:45p Bmeinke
// Added BposSetThreadPriority() to set the priority of a single thread
//
// 17    11/08/02 8:23a Tbochene
// Added GetEnv()
//
// 16    8/14/02 9:42a Skypig13
// corrected spelling of BposSendEvent()
//
// 15    4/25/02 4:39p Skypig13
// Updated with new changes
//    Revision 1.2  2002/04/24 13:01:46  erickson
//    changes merged from VSS
//
//
// 14    4/03/02 7:34a Kh
// Adding methods to deal with pulses.
//
//
// 13    11/21/01 7:57p Brian.meinke
// Changed 2nd argument to BposAttachName to a void**
//
// 12    11/16/01 1:29p Brian.meinke
// Added function BposAttachName for better Neutrino compatability
//
// 11    8/24/01 2:20p Kh
// Adding a method to close the channel after BposGetTaskId is cal
//
// 10    8/17/01 2:33p Kh
// Adding method to check if a process has registered by a specified name.
//
// 9     8/17/01 11:00a Brian.meinke
// Added 'value' argument to BposAttachTimerToChannel
//
// 8     6/05/01 10:39a Brian.meinke
// Changed char* arguments to const char * in some functions
//
// 7     6/04/01 5:17p Brian.meinke
// Changed arguments for shared memory functions:
// Added address argument in BposDestroySharedMemory
// Changed name argument to address in BposReleaseSharedMemory
//
// 6     5/15/00 1:36p Kh
// Adding a BposSystem function to launch programs.
//
// 5     5/02/01 7:34p Skypig13
// Added #inlcude <time.h>
//
// 4     4/24/01 1:54p Brian.meinke
// Added BposHandlePulse to handle standard pulse code messages
//
// 3     4/17/01 12:26p Brian.meinke
// Added function BposReadInt to read an integer (hex/octal/decimal) from
// a string
//
// 2     4/13/01 1:49p Brian.meinke
// Added BposAttachTimerToPulse to create a timer and attach it to an
// existing channel
//
// 1     4/10/01 10:46a Admin
//
// 1     4/09/01 12:59p Brian.meinke
//
// 3     3/19/01 9:07a Brian.meinke
// Added wrappers for lower level QNX methods (Sendmx, Receivemx, Relay,
// etc)
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 16    12/10/99 2:28a Michael.haugen
// Added Visual Source Safe keywords in header block in order to track VSS
// revision comments.
//
// 11/13/98  DBH
// -Original.
// 01/15/99	DBH
// -Removed prototypes for private functions (moved them to bepos.c).
// -Added prototypes for BposSemWaitWithTO(), BposSetQueueTimeout().
// -Modified prototypes for BposCreateMsgq().
// -Added timeout field to 'struct  bp_msg_q'.
// 04/06/99	DBH
// -Added prototypes for BposActivateAbort(), BposClearAbort(),
//	BposRegisterEvent(), BposRemoveEvent(), BposCheckAbortStatus().
// -Added prototype for BposNextAvlblSmphr().
// -Changed prototype for BposInit().

//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef BPOS_H                  // IF the template has not been included?
#define BPOS_H

/*****************************************************************************
@SM MODULE NAME: Burke Porter Operating System Header File
@SM
@SM $Header: /Core/ServicesLayer/Include/Bepos.h 1     12/21/03 6:26p Cward $
@SM
@SM DESCRIPTION:
@SM ------------
@SM This module implements the Burke Porter Operating System.  These routines
@SM allow application code to make standard operating system calls that are
@SM independent of the operating system being used.  This allows application
@SM code to be easily ported to different operating systems.
@SM
@SM NOTES:
@SM ------
@SM
@SM REVISION HISTORY:
@SM -----------------
@SM $Log: /Core/ServicesLayer/Include/Bepos.h $
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 2     6/20/03 8:48a Khykin
// Latest changes.
//
// 1     4/30/03 4:41p Bmeinke
// Migration to new personalized configuration management structure
//
// 18    4/23/03 2:45p Bmeinke
// Added BposSetThreadPriority() to set the priority of a single thread
//
// 17    11/08/02 8:23a Tbochene
// Added GetEnv()
//
// 16    8/14/02 9:42a Skypig13
// corrected spelling of BposSendEvent()
//
// 15    4/25/02 4:39p Skypig13
// Updated with new changes
//
// 14    4/03/02 7:34a Kh
// Adding methods to deal with pulses.
//
// 13    11/21/01 7:57p Brian.meinke
// Changed 2nd argument to BposAttachName to a void**
//
// 12    11/16/01 1:29p Brian.meinke
// Added function BposAttachName for better Neutrino compatability
//
// 11    8/24/01 2:20p Kh
// Adding a method to close the channel after BposGetTaskId is cal
//
// 10    8/17/01 2:33p Kh
// Adding method to check if a process has registered by a specified name.
//
// 9     8/17/01 11:00a Brian.meinke
// Added 'value' argument to BposAttachTimerToChannel
//
// 8     6/05/01 10:39a Brian.meinke
// Changed char* arguments to const char * in some functions
//
// 7     6/04/01 5:17p Brian.meinke
// Changed arguments for shared memory functions:
// Added address argument in BposDestroySharedMemory
// Changed name argument to address in BposReleaseSharedMemory
//
// 6     5/15/00 1:36p Kh
// Adding a BposSystem function to launch programs.
//
// 5     5/02/01 7:34p Skypig13
// Added #inlcude <time.h>
//
// 4     4/24/01 1:54p Brian.meinke
// Added BposHandlePulse to handle standard pulse code messages
//
// 3     4/17/01 12:26p Brian.meinke
// Added function BposReadInt to read an integer (hex/octal/decimal) from
// a string
//
// 2     4/13/01 1:49p Brian.meinke
// Added BposAttachTimerToPulse to create a timer and attach it to an
// existing channel
//
// 1     4/10/01 10:46a Admin
 *
 * 1     4/09/01 12:59p Brian.meinke
//
// 3     3/19/01 9:07a Brian.meinke
// Added wrappers for lower level QNX methods (Sendmx, Receivemx, Relay,
// etc)
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 16    12/10/99 2:28a Michael.haugen
// Added Visual Source Safe keywords in header block in order to track VSS
// revision comments.
@SM
@SM 11/13/98  DBH
@SM -Original.
@SM 01/15/99	DBH
@SM -Removed prototypes for private functions (moved them to bepos.c).
@SM -Added prototypes for BposSemWaitWithTO(), BposSetQueueTimeout().
@SM -Modified prototypes for BposCreateMsgq().
@SM -Added timeout field to 'struct  bp_msg_q'.
@SM 04/06/99	DBH
@SM -Added prototypes for BposActivateAbort(), BposClearAbort(),
@SM	BposRegisterEvent(), BposRemoveEvent(), BposCheckAbortStatus().
@SM -Added prototype for BposNextAvlblSmphr().
@SM -Changed prototype for BposInit().
@SM
@SM
@SM Copyright 1998, Burke E. Porter Machinery Co. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <time.h>

/**************************** DEFINES ***************************************/

#ifdef  BPOS_GLBLS              /* IF defining the global variables?	*/
#define BPOS_EXT_DEF
#else
#define BPOS_EXT_DEF extern
#endif


// minimum buffer size for all IPC messages not routed through the Q-Message Buffers
#define	MIN_MSG_BUF_SIZE	1024


#define BPOS_EVENT      (BPOS_MAX_EVNTS - 1)    /*protects kernel resources*/
#define BPOS_LOG_EVENT  (BPOS_MAX_EVNTS - 2)    /*synchronizes message logging*/
#define BPOS_ABORT_EVENT (BPOS_MAX_EVNTS - 3)	/*synchronizes abort process*/

#define BPOS_MAX_MSGQs      10    /* The max. number of queue in the system	*/
#define BPOS_MAX_MSGQ_LNGTH 128   /* The max. number of messages in a queue	*/
#define BPOS_MAX_QMSG       1024  /* The max. number of bytes in a message.	*/
#define BPOS_MSGQ_SYNC      1     /* Synchronous queue using two semaphores.*/
#define BPOS_MSGQ_ASYNC     2     /* Asynchronous queue using 1 semaphore.	*/

#define BPOS_USR_SEMS		10
#define BPOS_MSGQ_SEMS		(BPOS_MAX_MSGQs*2)
#define BPOS_SYSTM_SEMS		4

/* Define the system semaphore IDs being used.	*/
#define BPOS_Qs_AVLBL      (BPOS_USR_SEMS + BPOS_MSGQ_SEMS)
#define BPOS_CHNNLS_AVLBL  (BPOS_USR_SEMS + BPOS_MSGQ_SEMS + 1)

#define BPOS_MAX_SEMS      (BPOS_MSGQ_SEMS + BPOS_USR_SEMS + BPOS_SYSTM_SEMS)

#define BPOS_TO				1000	/* No. of milliseconds to delay	*/



//Return Codes
#define SUCCESS             0
#define FAILURE             -1
#define FATAL               -2
#define INVALID_MEMORY      -3
#define ABORT               -4
#define TIMEOUT             -5
#define NEG_RESPONSE        -6

#define BPOS_ERR_BASE       (0 - 0x1000)
#define BP_ERR_Q_OVRN       (BPOS_ERR_BASE + 0x0001) // Asynchronous Q overrun.
#define BP_ERR_MSGQ_ID      (BPOS_ERR_BASE + 0x0003) // invalid ID specified.
#define BP_ERR_MSGQ_TYP     (BPOS_ERR_BASE + 0x0004) // invalid TYPE specified.
#define BP_ERR_QMSG_SZ      (BPOS_ERR_BASE + 0x0005) // invalid message size
													 //  specified.
#define BP_ERR_Q_NACT       (BPOS_ERR_BASE + 0x0006) // queue object not active.
#define BP_ERR_Q_NCOR       (BPOS_ERR_BASE + 0x0007) // queue object not setup
													 //  correctly.
#define BP_ERR_MSGQ_CNT     (BPOS_ERR_BASE + 0x0008) // # messages > system max.
#define BP_ERR_SMPHRS_EXHSTD (BPOS_ERR_BASE +0x0009) // No semaphores left.
#define BP_ERR_SEM_FAIL     (BPOS_ERR_BASE + 0x0010) // semaphore creation error.
#define BP_ERR_SEM_EXISTS   (BPOS_ERR_BASE + 0x0011) // semaphore already exists.
#define BP_ERR_SEM_INVALID  (BPOS_ERR_BASE + 0x0012) // semaphore doesn't exist.
#define BP_ERR_EV_INVALID   (BPOS_ERR_BASE + 0x0013) // event # is invalid
#define BP_ERR_KRNL_FAIL    (BPOS_ERR_BASE + 0x0014) // failure from kernel call.
#define	BP_ERR_ABORT_TABLE	(BPOS_ERR_BASE + 0x0015) // Abort table full
#define BP_ERR_SEM_LOCKED	(BPOS_ERR_BASE + 0x0016) //
#define	BP_ERR_SEM_TIMEOUT	(BPOS_ERR_BASE + 0x0017) //
#define	BP_ERR_MSGQ_EXH		(BPOS_ERR_BASE + 0x0018) //


//Debug bit mask
#define KRNL_DEBUG      0x00000001      //Kernal debugging
#define BPOS_DEBUG      0x00000002      //BP Operating System debugging
#define ICM_BD_DEBUG    0x00000004      //ICM bd. debugging
#define DIO_BD_DEBUG    0x00000008      //Digital I/O bd. debugging
#define AIN_BD_DEBUG    0x00000010      //Analog input bd. debugging
#define AOUT_BD_DEBUG   0x00000020      //Analog out bd. debugging
#define SER_BD_DEBUG    0x00000040      //Serial bd. debugging
#define SC_BD_DEBUG     0x00000080      //System controller bd. debugging
#define CF_DEBUG        0x80000000      //Component file debugging
#define ALL_DEBUG       0xFFFFFFFF      //Global debugging



/*************************** DATA TYPES *************************************/

enum DebugType
{
    DebugType_SET,        //Turn on diagnostic messages.
    DebugType_CLEAR,      //Turn off diagnostic messages.
    DebugType_TEST        //Check state of diagnostic messages (on or off).
};

enum IsrType
{
    IsrType_NO_ADJUST,      //Just install isr normally.
    IsrType_ADJUST,         //Install isr, then adjust it so "ORI.W #$0700"
							//  statement is first.
    IsrType_PATCH,          //Patch isr to call the old isr when finished,
							//  then install the isr.
    IsrType_PATCH_ADJUST    //Patch, install, then adjust isr.
};

enum SmphrAction
{
	SmphrAction_CLEAR,		//Clear semaphore counts and events, don't change
							//	creation status.
	SmphrAction_RESET		//Clear semaphore counts, events, and set all to
							//	'not created' status
};

struct  bp_msg_q
{
    unsigned char q_typ;
    unsigned char actv;
    unsigned char in;
    unsigned char out;
    short msg_size;
    short q_lngth;
    short data_avlbl;
    short space_avlbl;
	long timeout;
    unsigned char data[BPOS_MAX_MSGQ_LNGTH] [BPOS_MAX_QMSG];
};
typedef struct bp_msg_q     BP_MESSAGE_Q;


/**************************** VARIABLES *************************************/

//INT32	iDemoMode=FALSE;


/****************************************************************************/
/****************************************************************************/
/**************************** FUNCTION PROTOTYPES ***************************/
/****************************************************************************/
/****************************************************************************/


#ifdef __cplusplus
extern "C"
{
#endif


/*****************************************************************************
@SF BposInstallSigHandler()
@SF 	Installs a signal handler
@SF
@SF PARAMETERS:
@SF 	sig_no	Signal number
@SF		func	function pointer to call when signal occurs
@SF
@SF RETURNS:
@SF 	-1		error
@SF		else	success
*****************************************************************************/
void ( *BposInstallSigHandler( int sig_no , void (*func)(int))) (int);


/*****************************************************************************
@SF BposUninstallSigHandler()
@SF 	Uninstalls a signal handler
@SF
@SF PARAMETERS:
@SF 	sig_no	signal number to remove
@SF
@SF RETURNS:
@SF 	-1		error
@SF		else	success
*****************************************************************************/
void ( *BposUninstallSigHandler( int sig_no)) (int);

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
int BposReadInt( const char *numString);


/*****************************************************************************
@SF BposRegisterTaskName()
@SF 	Register a name with the OS so other tasks can find it
@SF
@SF PARAMETERS:
@SF 	Pointer to the name to register
@SF
@SF RETURNS:
@SF 	-1	Error
@SF		0<	Name ID.  Needed to unregister the name.
*****************************************************************************/
BPOS_EXT_DEF int BposRegisterTaskName( const char *name);

/**
 * Attaches the calling process to the given name
 *
 * @param name     Name to attach for this process
 * @param attach_t name_attach_t structure to return the attach data in.
 *                 If unable to attach the calling process with the given name,
 *                 this will be set to NULL upon exiting
 * @return SUCCESS if no errors occur, BEP_ERR_KRNL_FAIL on error
 */
BPOS_EXT_DEF int BposAttachName( const char *name, void **attach_t);

/*****************************************************************************
@SF BposUnRegisterTaskName()
@SF 	UnRegister a name that was register with the OS using
@SF		BposRegisterTaskName().
@SF
@SF PARAMETERS:
@SF 	Name ID to remove that was returned by BposRegisterTaskName().
@SF
@SF RETURNS:
@SF 	-1	Error
@SF		0	Success
*****************************************************************************/
BPOS_EXT_DEF int BposUnRegisterTaskName (int name_id);


/*****************************************************************************
@SF BposGetTaskId()
@SF 	Get the process (task) ID of a processes that registered a given name.
@SF
@SF PARAMETERS:
@SF 	Registered name to search for.
@SF
@SF RETURNS:
@SF 	-1	Error
@SF		0<	Process ID of task.
*****************************************************************************/
BPOS_EXT_DEF int BposGetTaskId( const char *capProcessName);

/*****************************************************************************
@SF BposCloseTaskId()
@SF 	Close the file descriptor that was passed to the function.
@SF
@SF PARAMETERS:
@SF 	Channel file descriptor to close.
@SF
@SF RETURNS:
@SF 	<0	Error
@SF		0	Success
*****************************************************************************/
BPOS_EXT_DEF int BposCloseTaskId( int chid);

/*****************************************************************************
@SF BposGetMyTaskId()
@SF		Get the process (task) ID for the calling task.
@SF
@SF PARAMETERS:
@SF 	None
@SF
@SF RETURNS:
@SF 	-1	Error
@SF		0<	Process ID of task.
*****************************************************************************/
BPOS_EXT_DEF int BposGetMyTaskId( void );


/*****************************************************************************
@SF BposGetPriority()
@SF 	Get the priority for the task with the given process ID.
@SF
@SF PARAMETERS:
@SF 	0 = iPid	Get priority for calling process.
@SF		0 < iPid	Get priority for process with the given PID.
@SF
@SF RETURNS:
@SF 	-1		Error
@SF 	0<		Priority
*****************************************************************************/
BPOS_EXT_DEF int BposGetPriority( int iPid );


/*****************************************************************************
@SF BposSetPriority()
@SF 	Set the priority of a task.
@SF
@SF PARAMETERS:
@SF 	iPid		Process ID to be affected.  If zero, then calling task.
@SF 	iPriority	Priority to the set to.
@SF
@SF RETURNS:
@SF 	Previous priority or -1 on error.
@SF
*****************************************************************************/
BPOS_EXT_DEF int BposSetPriority( int iPid , int iPriority );


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
BPOS_EXT_DEF int BposSetThreadPriority( int iPid , int iTid , int iPriority);

/*****************************************************************************
@SF BposSend()
@SF 	Sends a message to another task and waits for a reply message. See
@SF		BposReply().
@SF
@SF PARAMETERS:
@SF 	iPid		Process that the message is being sent to.
@SF 	*capXmtBuff	Pointer to message being sent.
@SF 	*capRcvBuff	Pointer to where response is to be placed.
@SF 	iXmtLen		Length of transmit message
@SF 	iRcvLen		Length of receive buffer.
@SF
@SF RETURNS:
@SF 	-1	Error
@SF		0	Success
*****************************************************************************/
BPOS_EXT_DEF int BposSend( int iPid , char *capXmtBuff , char *capRcvBuff , int iXmtLen , int iRcvLen );


/*****************************************************************************
@SF BposCReceive()
@SF 	Checks to see if there is message being sent to the calling task.  The
@SF		function always returns whether or not there is a message being sent.
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
BPOS_EXT_DEF int BposCReceive( int iPid , char *capRcvBuff , int iRcvLen );


/*****************************************************************************
@SF BposReceive()
@SF 	Wait until a message is received.
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
BPOS_EXT_DEF int BposReceive( int iPid , char *capRcvBuff , int iRcvLen );

/*****************************************************************************
@SF BposReply()
@SF 	Sends a reply message to a task that is blocked from having sent a
@SF		message using the BposSend() function.
@SF
@SF PARAMETERS:
@SF 	iPid		The process ID that the reply message is to be sent to.
@SF 	*capXmtBuff	Pointer to response message.
@SF 	iXmtLen		Length of response message in bytes.
@SF
@SF RETURNS:
@SF 	-1	Error
@SF		0	Success
*****************************************************************************/
BPOS_EXT_DEF int BposReply( int iPid , char *capXmtBuff , int iXmtLen );


/*****************************************************************************
@SF BposCreateSharedMemory() creates a shared memory object for use by any
@SF task that uses bepos.  After creation, the shared memory is opened and
@SF mapped to local task memory.
@SF
@SF PARAMETERS:
@SF name    -> A character string used to identify the shared memory object.
@SF size    -> Size of the shared memory object in bytes.
@SF filedesc-> pointer to file descriptor (handle) for shared memory object.
@SF             This parameter is updated by the function.
@SF
@SF RETURNS:
@SF If success  -> starting address of shared memory.
@SF Otherwise   -> error code (always negative - see bep.h).
*****************************************************************************/
BPOS_EXT_DEF char *BposCreateSharedMemory( const char *name, int size, int *filedesc);


/*****************************************************************************
@SF BposOpenSharedMemory() opens a shared memory object for use by any
@SF task that uses bepos.  After creation, the shared memory is opened and
@SF mapped to local task memory.
@SF
@SF PARAMETERS:
@SF name    -> A character string used to identify the shared memory object.
@SF size    -> Size of the shared memory object in bytes.
@SF filedesc-> pointer to file descriptor (handle) for shared memory object.
@SF             This parameter is updated by the function.
@SF
@SF RETURNS:
@SF If success -> starting address of shared memory.
@SF Otherwise  -> error code (always negative - see bep.h).
*****************************************************************************/
BPOS_EXT_DEF char *BposOpenSharedMemory( const char *name, int size, int *filedesc);


/*****************************************************************************
@SF BposDestroySharedMemory() destroys a shared memory object.  Any tasks
@SF that are using the shared memory object must close it first before this
@SF action is taken.
@SF
@SF PARAMETERS:
@SF name    -> A character string used to identify the shared memory object.
@SF address -> Pointer to the start of memory to be unmapped
@SF size    -> Size of the shared memory object in bytes.
@SF filedesc-> pointer to file descriptor (handle) for shared memory object.
@SF
@SF RETURNS:
@SF None.
*****************************************************************************/
BPOS_EXT_DEF void BposDestroySharedMemory( const char *name, void *address, int size, int filedesc);


/*****************************************************************************
@SF BposCloseSharedMemory() closes a shared memory object for a task.  Any
@SF tasks that are still using the shared memory object are not affected.
@SF This function must be executed by all tasks using a shared memory object
@SF before that object is destroyed.
@SF
@SF PARAMETERS:
@SF address -> Pointer to the start of memory to be unmapped
@SF size    -> Size of the shared memory object in bytes.
@SF filedesc-> pointer to file descriptor (handle) for shared memory object.
@SF
@SF RETURNS:
@SF None.
*****************************************************************************/
BPOS_EXT_DEF void BposReleaseSharedMemory(char *address, int size, int filedesc);


/*****************************************************************************
@SF BposSleep() suspends the calling task for the specified number of
@SF milliseconds.  After the specified time period has elapsed the task
@SF will be returned to the ready state. If it is the highest priority task
@SF it will be returned to the CPU.
@SF
@SF PARAMETERS:
@SF sleep_time - The number of milliseconds to suspend the task.
@SF              This time has a 10 millisecond resolution for PDOS.
@SF              This time has a  2 millisecond resolution for QNX.
@SF RETURNS:
@SF SUCCESS   - Successful.
@SF other     - Error code.
*****************************************************************************/
BPOS_EXT_DEF int BposSleep (long sleep_time);


/*****************************************************************************
@SF BposKillTask() kills a task.  The option exists to deallocate or
@SF not deallocate the memory of the killed task.
@SF
@SF PARAMETERS:
@SF task          - task number
@SF dealloc_mem	  - TRUE to deallocate memory after task dies,
@SF					FALSE to keep the memory.
@SF
@SF RETURNS:
@SF SUCCESS   - Successful.
@SF other     - Error code.
*****************************************************************************/
BPOS_EXT_DEF int BposKillTask (int task, int dealloc_mem);


/*****************************************************************************
@SF BposCreateTask() creates a task.  The *command parameter usually gives
@SF a filename to be run as the new task, but it can be any montitor
@SF command or can be NULL.
@SF
@SF PARAMETERS:
@SF mem       - The task size in 1K increments.
@SF priority  - task priority.
@SF tics      - number of tics given to the task each time it is scheduled.
@SF port      - output port number.
@SF *command  - name of the file to run as the new task.
@SF *task     - pointer to new task number.
@SF
@SF RETURNS:
@SF SUCCESS   - Successful.
@SF other     - Error code.
*****************************************************************************/
BPOS_EXT_DEF int BposCreateTask( const int algrthm, const int priority, const int port, const int mem,
									const char *command, const char *arg_str);


/*****************************************************************************
@SF BposDebug() turns on/off diagnostic messages for selected modules
@SF and returns the diagnostic setting for the selected module.
@SF
@SF PARAMETERS:
@SF action    -> Type of diagnostic messaging action to take.
@SF debug_bits-> Debug flag bit mask to be set, cleared, or checked against.
@SF               See the debug defines in bep.h.
@SF
@SF RETURNS:
@SF Current diagnostic state for all modules (ON or OFF)
*****************************************************************************/
BPOS_EXT_DEF unsigned long BposDebug (enum DebugType action, unsigned long debug_bits);


/*****************************************************************************
@SF BposInit() allocates (if not already allocated) shared memory for
@SF bepos functions.  If shared memory is already allocated, this function
@SF attaches to the shared memory.  System global variables are then mapped
@SF to this memory so any task linking to bepos can communicate with any
@SF other task that also links to bepos.  If this function allocates shared
@SF memory, it will also intialize system control semaphores and global
@SF variables.
@SF
@SF PARAMETERS:
@SF task_id	-> User assigned task identification number.  This number
@SF             may or may not be used, depending on the system OS.
@SF
@SF RETURNS:
@SF If success  -> 0
@SF Otherwise   -> error code (always negative - see bep.h).
*****************************************************************************/
BPOS_EXT_DEF int BposInit (short task_no);


/*****************************************************************************
@SF BposClose() deallocates shared memory if the same task allocated it, or
@SF just detaches from it otherwise.
@SF
@SF PARAMETERS:
@SF None.
@SF
@SF RETURNS:
@SF None.
*****************************************************************************/
BPOS_EXT_DEF void BposClose (void);


/*****************************************************************************
@SF BposLockTask() calls a kernel function to lock a task.  A locked task
@SF will prevent any other tasks from receiving CPU time.  Normal task
@SF scheduling is suspended.
@SF
@SF PARAMETERS:
@SF None.
@SF
@SF RETURNS:
@SF SUCCESS   - Successful.
@SF other     - Error code.
*****************************************************************************/
BPOS_EXT_DEF int BposLockTask (void);


/*****************************************************************************
@SF BposUnlockTask() calls a kernel function to unlock a task.  Normal task
@SF scheduling is resumed.
@SF
@SF PARAMETERS:
@SF None.
@SF
@SF RETURNS:
@SF SUCCESS   - Successful.
@SF other     - Error code.
*****************************************************************************/
BPOS_EXT_DEF int BposUnlockTask (void);


/*****************************************************************************
@@@SF BposSemTestWait() verifys that the semaphore number is valid and
@@@SF then calls a function to implement the test and wait process.  This
@@@SF process tests a semaphore to see if it has a positive count
@@@SF and waits on the semaphore if it has value >= 1, reducing the count.
@@@SF If the count value is < 1, a wait operation is not performed and the
@@@SF count is not changed.  The purpose of this function is to allow a
@@@SF task to use a semaphore protected resource only if it is available,
@@@SF preventing the task from suspending until the resource is free.
@@@SF
@@@SF PARAMETERS:
@@@SF sem_number -> The semaphore ID to test and wait on.
@@@SF
@@@SF RETURNS:
@@@SF If success -> 0
@@@SF Otherwise  -> error code (always negative - see bep.h)
*****************************************************************************/
BPOS_EXT_DEF int BposSemTestWait (int sem_number);


/*****************************************************************************
@@@SF BposSemWait() verifies that the semaphore number is valid and
@@@SF then calls a function to implement the semaphore wait process.  If the
@@@SF semaphore count is >= 1, the task is granted access to the protected
@@@SF resource, otherwise the task suspends until the resource is free.
@@@SF
@@@SF PARAMETERS:
@@@SF sem_number -> The semphore ID to suspend on.
@@@SF
@@@SF RETURNS:
@@@SF If success -> 0
@@@SF Otherwise  -> error code (always negative - see bep.h)
*****************************************************************************/
BPOS_EXT_DEF int BposSemWait (int sem_number);


/*****************************************************************************
@@@SF BposSemWaitWithTO() verifies that the semaphore number is valid and
@@@SF then calls a function to implement the semaphore wait process.  This
@@@SF function is identical to BposSemWait(), but specifies a timeout for
@@@SF the wait process.  If the semaphore wait process is not successful
@@@SF within the specified timeout period, the wait process is terminated
@@@SF and the semaphore count is incremented by one, and a timeout error code
@@@SF is returned.
@@@SF
@@@SF PARAMETERS:
@@@SF sem_number -> The semphore ID to suspend on.
@@@SF timeout    -> timeout period in milliseconds
@@@SF
@@@SF RETURNS:
@@@SF If success -> 0
@@@SF Otherwise  -> error code (always negative - see bep.h)
*****************************************************************************/
BPOS_EXT_DEF int BposSemWaitWithTO (int sem_number, long timeout);


/*****************************************************************************
@@@SF BposSemSignal() verifies that the semaphore number is valid and
@@@SF then calls a function to implement the semaphore signal process.
@@@SF Signaling a semaphore increments the count, freeing the resource for
@@@SF access by one more pending task.
@@@SF
@@@SF PARAMETERS:
@@@SF sem_number -> The semphore ID to suspend on.
@@@SF
@@@SF RETURNS:
@@@SF If success -> 0.
@@@SF Otherwise  -> error code (always negative - see bep.h)
*****************************************************************************/
BPOS_EXT_DEF int BposSemSignal (int sem_number);


/*****************************************************************************
@@@SF BposSemCreate() creates a semaphore with the specified number of counts
@@@SF (signals) and the specified ID.  If the specified ID is negative, then
@@@SF the next available semaphore ID is selected and returned by the
@@@SF function.
@@@SF
@@@SF PARAMETERS:
@@@SF sem_number -> The ID number of the created semaphore.  If negative
@@@SF                   the function selects the next available ID.
@@@SF sem_count  -> The number of signals on the created semaphore.
@@@SF
@@@SF RETURNS:
@@@SF If success -> The ID number of the created semaphore.
@@@SF Otherwise  -> error code (always negative - see bep.h)
*****************************************************************************/
BPOS_EXT_DEF int BposSemCreate (int sem_number, int sem_count);


/*****************************************************************************
@@@SF BposSemDelete() deletes the specified semaphore.
@@@SF
@@@SF PARAMETERS:
@@@SF sem_number -> The semphore ID to delete.
@@@SF
@@@SF RETURNS:
@@@SF If success -> 0.
@@@SF Otherwise  -> error code (always negative - see bep.h)
*****************************************************************************/
BPOS_EXT_DEF int BposSemDelete (int sem_number);


/***************************************************************************
@SF BposCreateTimer() creates a system timer which can be used by
@SF 	BposStartTimer() and PbosStopTimer().  These timers run for a specified
@SF 	time period and then expire.  When they expire, they assert a QNX
@SF 	signal.
@SF
@SF PARAMETERS:
@SF 	None.
@SF
@SF RETURNS:
@SF 	If successful	: Timer ID number.
@SF 	else			: error code (see bep.h).
****************************************************************************/
BPOS_EXT_DEF int BposCreateTimer( void );


/***************************************************************************
@SF BposCreateTimerWithProxy() creates a system timer which can be used by
@SF 	BposStartTimer() and PbosStopTimer().  These timers run for a specified
@SF 	time period and then expire.  When they expire, they assert a QNX
@SF 	signal.
@SF
@SF PARAMETERS:
@SF 	ipProxy		Pointer to where Proxy ID used will be stored.
@SF		iPid		Process ID to attach proxy to (0 == current process)
@SF		cpMessage	Pointer to message to attach to proxy when timer expires
@SF		iBytes		Number of bytes in the message
@SF		iPriority	Priority of the Proxy (-1 == priority of caller)
@SF
@SF RETURNS:
@SF 	If successful	: Timer ID number.
@SF 	else			: -1 error code (see bep.h).
****************************************************************************/
BPOS_EXT_DEF int BposCreateTimerWithProxy( int *ipProxy , pid_t iPid , char *cpMessage , int iBytes , int iPriority );


/***************************************************************************
@SF BposStartTimer() sets a timer to expire after a specified time period
@SF 	has elapsed, and then starts the timer.
@SF
@SF PARAMETERS:
@SF 	timer_id	- The id number of the timer to be started.
@SF 	iDelayTime	- The length of time before the first time out.	[ms]
@SF		iRepTime	- Repatition rate of following time outs.		[ms]
@SF
@SF RETURNS:
@SF 	If successful	: 0
@SF 	else			: error code (see bep.h).
****************************************************************************/
BPOS_EXT_DEF int BposStartTimer (timer_t timer_id, long iDelayTime , long iRepTime );


/***************************************************************************
@SF BposStopTimer() stops a timer before it expires.  When a timer is
@SF 	stopped, a signal is not asserted by QNX.
@SF
@SF PARAMETERS:
@SF 	timer_id	- The id number of the timer to be stopped.
@SF
@SF RETURNS:
@SF 	If successful	: 0
@SF 	else			: error code (see bep.h).
****************************************************************************/
BPOS_EXT_DEF int BposStopTimer (timer_t timer_id);


/***************************************************************************
@SF BposDeleteTimer()
@SF		Deletes a timer.
@SF
@SF PARAMETERS:
@SF 	timer_id	- The id number of the timer to be stopped.
@SF
@SF RETURNS:
@SF 	If successful	: 0
@SF 	else			: error code (see bep.h).
****************************************************************************/
BPOS_EXT_DEF int BposDeleteTimer (timer_t timer_id);


/***************************************************************************
@SF BposDeleteTimerWithProxy()
@SF		Deletes a timer and the associated proxy
@SF
@SF PARAMETERS:
@SF 	timer_id	- The id number of the timer to be stopped.
@SF 	iProxy		- The id number of the proxy to be removed
@SF
@SF RETURNS:
@SF 	If successful	: 0
@SF 	else			: error code (see bep.h).
****************************************************************************/
BPOS_EXT_DEF int BposDeleteTimerWithProxy( timer_t timer_id , int iProxy );


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
BPOS_EXT_DEF pid_t BposCreateProxy( pid_t iPid , char *cpMessage , int iBytes , int iPriority );


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
BPOS_EXT_DEF pid_t BposDeleteProxy( pid_t iProxy );


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
BPOS_EXT_DEF pid_t BposTriggerProxy( pid_t iProxy );


/*****************************************************************************
@SF BposCreateMsgq() creates a message queue with a specific q_id. Each
@SF message in the queue will be of length msg_sz, and the queue will
@SF hold upto msg_cnt messages.
@SF If the caller attempts to recreate an existing queue the queue
@SF will be flushed of all data and restored to an initial state.
@SF Message queue #1 is used by the system for system messaging.
@SF
@SF PARAMETERS:
@SF q_id   -> The handle to create the message queue with.  If this value is
@SF             0, the queue handle is assigned automatically.  Note that
@SF             manual and automatic assignment of queue handles should not
@SF             be mixed - use one system or the other exclusively.
@SF q_type -> The type of queue to create i.e.
@SF             BPOS_SYNC_Q - A synchronous queue uses two counting
@SF                           semaphores, one indicating how much space
@SF                           is available and one indicating how much
@SF                           data is available. Tasks sending data to a
@SF                           synchronous queue will be suspended if the
@SF                           queue becomes full.
@SF             BPOS_ASYNC_Q- A queue using only one counting semaphore
@SF                           to indicate that data is available. Only
@SF                           tasks requesting data from the queue will
@SF                           be suspended if the queue is empty.
@SF msg_cnt -> The number of messages that the queue can hold.
@SF msg_size-> The maximum size of a message.
@SF timeout -> The number of milliseconds before a timeout occurs waiting
@SF				to send to the queue or waiting to receive from queue.
@SF
@SF RETURNS:
@SF If success -> queue id is returned (always positive).
@SF Otherwise  -> error code (always negative - see bep.h).
*****************************************************************************/
BPOS_EXT_DEF int BposCreateMsgq (short q_id, short q_type, short msg_cnt,
                                    short msg_size, long timeout);


/*****************************************************************************
@SF BposSetQueueTimeout() sets the timeout period for the queue send and get
@SF operations.  The timeout only applies to the send operation if the queue
@SF is synchronous.
@SF
@SF PARAMETERS:
@SF q_id    -> The handle of the message queue.
@SF timeout -> The number of milliseconds before a timeout occurs waiting
@SF				to send to the queue or waiting to receive from queue.
@SF
@SF RETURNS:
@SF None.
*****************************************************************************/
BPOS_EXT_DEF void BposSetQueueTimeout (short q_id, long timeout);


/*****************************************************************************
@SF BposSendQmsg() stuffs a message into the specified queue. If the
@SF specified queue is a synchronous queue and the queue is full, the
@SF calling task will suspend until space becomes available.
@SF If the queue is asyncronous and full, a system error
@SF message will be posted and the oldest data in the queue will
@SF be over written. The number of bytes specified at queue
@SF creation will be copied from the source address to the queue
@SF
@SF PARAMETERS:
@SF q_id - The handle of the message queue to stuff the message into.
@SF msg  - A pointer to the message data to copy to the queue.
@SF
@SF RETURNS:
@SF If success -> 0
@SF Otherwise  -> error code (always negative - see bep.h)
*****************************************************************************/
BPOS_EXT_DEF int BposSendQmsg (const short q_id, const unsigned char *msg);


/******************************************************************************
@SF BposMsgqRcv() retrieves a message from the specified queue. If the queue
@SF is empty the calling task suspends until data becomes available.
@SF If data is available the task will continue to run unless a higher
@SF priority task takes the CPU. The caller must have a enough memory at the
@SF target address to hold the number of bytes that were specified as the
@SF message size when the queue was created.
@SF
@SF PARAMETERS:
@SF q_id      -> A handle of the queue to stuff the message into.
@SF *msg_rcvd -> The address to copy the message to.
@SF
@SF RETURNS:
@SF If success -> 0
@SF Otherwise  -> error code (always negative - see bep.h).
*****************************************************************************/
BPOS_EXT_DEF int BposGetQmsg (const short q_id, unsigned char *msg_rcvd);


/*****************************************************************************
@SF BposActivateAbort() should be called from a single task when a system
@SF abort has occurred.  This function sets the global abort flag and
@SF continually awakens suspended tasks until another task calls
@SF BposClearAbort().
@SF
@SF PARAMETERS:
@SF None.
@SF
@SF RETURNS:
@SF None.
*****************************************************************************/
BPOS_EXT_DEF void BposActivateAbort (void);


/*****************************************************************************
@SF BposClearAbort() should be called from a single task after all tasks
@SF have responded to a system abort.  This function clears the global
@SF Abort flag so normal processing can continue.
@SF
@SF PARAMETERS:
@SF None.
@SF
@SF RETURNS:
@SF None.
*****************************************************************************/
BPOS_EXT_DEF void BposClearAbort (void);


/*****************************************************************************
@SF BposCheckAbortStatus() reports the status of the global abort flag.
@SF
@SF PARAMETERS:
@SF None.
@SF
@SF RETURNS:
@SF SUCCESS   - Successful.
@SF other     - Error code.
*****************************************************************************/
BPOS_EXT_DEF int BposCheckAbortStatus (void);


/***************************************************************************
@SF BposLog() is used like printf, but calls a kernel routine that ensures
@SF that the messages print out in the correct order, and without
@SF interruption by other application generated informational messages.
@SF
@SF PARAMETERS:
@SF *format_str	- Pointer to printf style format string.
@SF ...			- arguments for conversion specifiers in format string.
@SF
@SF RETURNS:
@SF None.
****************************************************************************/
BPOS_EXT_DEF void BposLog( const char *cpFmt, ...);

/*
====>	HERE HERE HERE
*/

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
BPOS_EXT_DEF int BposSendmx(int         iPid,
							unsigned    uSmsgParts,
							unsigned    uRmsgParts,
							const void  *spSmsg,
							const void  *spRmsg );

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
BPOS_EXT_DEF int BposReceivemx( pid_t       iPid,
								unsigned    uParts,
								void  		*spRmsg);

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
BPOS_EXT_DEF int BposReplymx(pid_t      iPid,
							unsigned    uRmsgParts,
							void  		*spRmsg );

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
BPOS_EXT_DEF int BposCreceivemx(pid_t       iPid,
								unsigned    sRmsgParts,
								void  		*spRmsg );

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
BPOS_EXT_DEF int BposRelay(	pid_t iPid1, pid_t iPid2, int len, char *data);

/**
 * Creates a new IPC commujnicattion channel for the calling process
 *
 * @param flags  Flag controlling how the kernel will use the channel to notify
 *               the calling process for certain events (disconnected from
 *               client, thread death, etc)
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
BPOS_EXT_DEF int BposChannelCreate( unsigned int flags);

/**
 * Destroys the channel identified by channelId. This mnakes the channel
 * unavailable for client connections.
 *
 * @param channelId Channel to destroy
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
BPOS_EXT_DEF int BposChannelDestroy( int channelId);

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
BPOS_EXT_DEF int BposConnectAttach(unsigned long nd, pid_t pid, int chid, unsigned index, int flags);

/**
 * Closes an open connection
 *
 * @param connectId Connection to be closed
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
BPOS_EXT_DEF int BposConnectDetach( int connectId);

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
BPOS_EXT_DEF int BposSendPulse(int coid, int priority, int code, int value);

/**
 * Creates a channel and connection to that channel within the calling process.
 * This is usefule when creating a timer with a pulse.
 *
 * @param channelId Buffer into which the newly created channel ID willbe stored
 * @param connectId Buffer into which the newly created connection ID willbe stored
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 */
BPOS_EXT_DEF int BposCreateSelfAttachment( int *channelId, int *connectId);

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
BPOS_EXT_DEF timer_t BposCreateTimerWithPulse( int *channelId, int *connectId, int pulseCode);

/**
 * Destroys a previously created timer and it's associated pulse
 *
 * @param timerId   Timer to destroy
 * @param channelId IPC channel id use to deliver the pulse
 * @param connectId IPC connection id used for the channel
 * @return 0 if successful
 *         -1 if error (use errno for more error information)
 */
int BposDeleteTimerWithPulse( timer_t timerId, int channelId, int connectId);

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
int BposAttachTimerToChannel( int channelId, int *connectId, int timerCode, int value);

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
int BposDeliverEvent(int rcvid, const struct sigevent* event );

/**
 * Send and event to the specified channel.
 *
 * @param coid     The connection to send the event to.
 * @param priority The priority to send the event at.
 * @param code     The code to associate with the event.
 * @param value    The value to associate with the event.
 * @return The status of the operation.
 */
int BposSendEvent(int coid, int priority, int code, int value );

/**
 * Wait for an event to be sent to the specified channel.
 *
 * @param chid   The channel to wait on.
 * @param pulse  The pulse information that was received.
 * @param bytes  The number of bytes that were received.
 * @param info   Information about the pulse (not updated currently)
 * @return The status of the operation.
 */
int BposReceiveEvent(int chid, void* pulse, int bytes, struct _msg_info* info );

/**
 * Checks if a pulse is one of the standard OS pulses and handles it accordingly
 *
 * @param pulse  Pointer to the pulse data (struct _pulse*)
 * @return 1 if pulse not handled (not a standard pules)
 *         0 if pulse was handled
 *         -1 if error
 */
int BposHandlePulse( void* pulse);

/*****************************************************************************
@SF BposYield()
@SF 	Yield CPU to another process at the same priority as the calling
@SF 	process.
@SF
@SF PARAMETERS:
@SF
@SF RETURNS:
*****************************************************************************/
BPOS_EXT_DEF void BposYield( void );

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************


/**
 * Perform a command as if it were executed on the command line.
 *
 * @param command the command to execute on the command line.
 * @return 0  success
 *         -1 error
 */
int BposSystemCommand(const char *command);

/**
 * Checks for the existance of a task in the system.
 *
 * @param taskName The name to check for.
 * @return The success or failure of the check for the tasks
 *         existance.
 * @since Version 1.2
 */
int BposIsTaskAlive(const char *taskName);

/**
 * Gets the value of an environment variable
 *
 * @param var    Name of environment variable
 * @return The value of the environment variable (empty string if not found)
 * @since Version 17
 */
const char* BposGetEnv(const char *var);

/**
 * Gets the number of seconds that have elapsed since the PC was
 * booted.
 *
 * @return The number of seconds since bootup as a double precision floating
 * 			point value
 */
double BposSecondsSinceBootUp( void );



// ***************************************************************************

#ifdef __cplusplus
}
#endif

#endif	// BPOS_H

