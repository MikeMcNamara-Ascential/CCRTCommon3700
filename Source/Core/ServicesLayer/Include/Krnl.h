//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/Krnl.h 1     12/21/03 6:27p Cward $
//
// FILE DESCRIPTION:
//  This header file contains defines and function prototypes for the OS
//  kernel routines.
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
//    $Log: /Core/ServicesLayer/Include/Krnl.h $
// 
// 1     12/21/03 6:27p Cward
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
// 2     6/20/03 8:50a Khykin
// Updated for Ford changes.
// 
// 1     4/30/03 4:40p Bmeinke
// Migration to new personalized configuration management structure
// 
// 16    4/23/03 2:45p Bmeinke
// Added KrnlSetThreadPriority() to set the priority of a single thread
// 
// 15    11/08/02 8:23a Tbochene
// Added GetEnv()
// 
// 14    4/03/02 1:22p Kh
// Adding methods to deal with pulses.
//
//
//*************************************************************************
#ifndef KRNL_H                  /* IF the template has not been included? */
#define KRNL_H

/**************************** DEFINES ***************************************/

#include <semaphore.h>
#include <time.h>

#ifdef  KRNL_GLBLS              /* IF defining the global variables? */
#define KRNL_EXT_DEF
#else
#define KRNL_EXT_DEF extern
#endif

#define AUTO_RESET          0x00008000L
#define NO_RESET            0x00000000L

#define BPOS_EVENT_TRUE     1
#define BPOS_EVENT_FALSE    0
#define BPOS_EVNT_CLR       1
#define BPOS_SYSTM_ABRT     11

#define NONE                0
#define MVME147             1

#define OS_SERIAL_VEC       0x43
#define MAX_ARGS			5

#define	SUPER_USER_UID		0
#define	MAX_NONSU_PRIORITY	19
#define	MAX_SU_PRIORITY		29

#define ROUND_ROBIN			0
#define FIFO				1
#define ADAPTIVE			2
#define INVALID_ALG			-1

struct shared_mem_tag
{
	char	name[32];
	char	*addr;
	int		filedesc;
	int		size;
};
typedef struct shared_mem_tag	SHARED_MEM;

typedef sem_t    BP_SEMAPHORE;


/***************************************************************************/
/***************************************************************************/
/**************************** FUNCTION PROTOTYPES **************************/
/***************************************************************************/
/***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * KrnlRegisterTaskName() registers a task with the OS by a user assigned name.
 * This is useful for obtaining a task's process id, since the id can be any
 * arbitrary number.
 *
 * @param name   The user assigned name for the task (32 or less characters).
 *
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlRegisterTaskName( const char *name);

/**
 * Attaches the calling process to the given name
 * 
 * @param name     Name to attach for this process
 * @param attach_t Pointer to the name_attach_t structure to return the attach
 *                 data in. If unable to attach the calling process with the given
 *                 name, this will be set to NULL upon exiting
 * @return SUCCESS if no errors occur, BEP_ERR_KRNL_FAIL on error
 */
KRNL_EXT_DEF int KrnlAttachName( const char *name, void **attach_t);

/**
 * KrnlUnRegisterTaskName() clears an assigned task name from the task name pool.
 *
 * @param name_id The user assigned name for the task (32 or less characters).
 *
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlUnRegisterTaskName (int name_id);


/**
 * KrnlGetTaskID() gets the task ID of the requesting task.
 *
 * @param capProcessName
 *               Name of the process to get pid for (registered name)
 * @return Task ID number.
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlGetTaskId( const char *capProcessName);

/**
 * KrnlCloseTaskId() will close the file descriptor that
 * was opened from the KrnlGetTaskId() function.
 *
 * @param chid   The file descriptor that is to be closed.
 * @return The status of the attempted closing.
 */
KRNL_EXT_DEF int KrnlCloseTaskId(int chid);

/**
 * Returns the pid for the calling process
 *
 * @return The pid for the calling process
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlGetMyTaskId( void );

/**
 * Returns the priority for the specified process
 *
 * @param iPid   pid of process to get priority for
 * @return The priority for the specified process
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlGetPriority( int iPid );

/**
 * Sets the priority for the specified process
 *
 * @param iPid      pid of process to set the priority of
 * @param iPriority New priority level for the specifiedc process
 * @return ????
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlSetPriority( int iPid , int iPriority );

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
KRNL_EXT_DEF int KrnlSetThreadPriority( int iPid, int iTid, int iPriority);

/**
 * Sends an IPC to the specified process
 *
 * @param iPid       Process to send the message to
 * @param capXmtBuff Buffer to send to the receiving process
 * @param capRcvBuff Buffer to recevie reply into
 * @param iXmtLen    Size, in bytes, of the send buffer
 * @param iRcvLen    Size, in bytes, of the reply buffer
 * @return If successfule: 0
 *         else          : -1
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlSend( int iPid , char *capXmtBuff , char *capRcvBuff , int iXmtLen , int iRcvLen );

/**
 * Checks if an IPC message is waiting for us
 *
 * @param iPid       Process ID to check for message from (0 for any process)
 * @param capRcvBuff Buffer to recevie reply into
 * @param iRcvLen    Size, in bytes, of the reply buffer
 * @return If successfule: 0
 *         else          : -1
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlCReceive( int iPid , char *capRcvBuff , int iRcvLen );

/**
 * Waits for an IPC message from the specified process
 *
 * @param iPid       Process ID to wait for message from (0 for any process)
 * @param capRcvBuff Buffer to recevie reply into
 * @param iRcvLen    Size, in bytes, of the reply buffer
 * @return If successfule: 0
 *         else          : -1
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlReceive( int iPid , char *capRcvBuff , int iRcvLen );

/**
 * Sends an IPC reply to the specified process
 *
 * @param iPid       Process to send the reply message to
 * @param capXmtBuff Buffer to send to the receiving process
 * @param iXmtLen    Size, in bytes, of the send buffer
 * @return If successfule: 0
 *         else          : -1
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlReply( int iPid , char *capXmtBuff , int iXmtLen );

/**
 * KrnlCreateSharedMemory() creates a shared memory object and returns a pointer
 * to its location.
 *
 * @param name     SF
 *                 SF PARAMETERS:
 *                 Pointer to the string containing the shared mem object name.
 *
 * @param size     Size of the shared memory object in bytes.
 *
 * @param filedesc pointer to the file descriptor (handle) for the shared memory object. The file
 *                 descriptor value is set by this function.
 *
 * @return If successful - Character pointer to where shared memory is mapped to.
 *         else          - Pointer value of -1.
 * @since Version 1.0
 */
KRNL_EXT_DEF char *KrnlCreateSharedMemory( const char *name, int size, int *filedesc);


/**
 * KrnlOpenSharedMemory() creates a shared memory object and returns a pointer to
 * its location.
 *
 * @param name     SF
 *                 SF PARAMETERS:
 *                 Pointer to the string containing the shared mem object name.
 * @param size     Size of the shared memory object in bytes.
 * @param filedesc pointer to the file descriptor (handle) for the shared memory object. The file
 *                 descriptor value is set by this function.
 * @return If successful - Character pointer to where shared memory is mapped to.
 *         else          - Pointer value of -1.
 * @since Version 1.0
 */
KRNL_EXT_DEF char *KrnlOpenSharedMemory( const char *name, int size, int *filedesc);


/**
 * Destroys a shared memory object that was opened by a successful call to
 * KrnlCreateSharedMemory().
 *
 * @param name     Pointer to the string containing the shared mem object name.
 * @param address  Pointer to the start of memory to be unmapped
 * @param size     Size of the shared memory object in bytes.
 * @param filedesc the file descriptor (handle) for the shared memory object.
 * @since Version 1.0
 */
KRNL_EXT_DEF void KrnlDestroySharedMemory( const char *name, void* address, int size, int filedesc);


/**
 * KrnlReleaseSharedMemory() releases a shared memory object that was opened by a
 * successful call to KrnlOpenSharedMemory().
 *
 * @param address  Pointer to the start of memory to be unmapped
 * @param size     Size of the shared memory object in bytes.
 * @param filedesc the file descriptor (handle) for the shared memory object.
 * @return
 * @since Version 1.0
 */
KRNL_EXT_DEF void KrnlReleaseSharedMemory( const char *address, int size, int filedesc);


/**
 * KrnlSleep() suspends the calling task for the specified number of SF
 * milliseconds. After the specified time period has elapsed the task will be
 * returned to the ready state. If it is the highest priority task it will be
 * returned to the CPU.
 *
 * @param sleep_time The number of milliseconds to suspend the task.
 *
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlSleep (long sleep_time);


/**
 * kills a task.
 *
 * @param task_id Task number to kill
 * @param dealloc_mem
 *                Not used for QNX.  For compatibility with krnl modules for other OS's.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlKillTask (int task_id, short dealloc_mem);


/**
 * KrnlCreateTask() creates a task.  The *command parameter usually gives a
 * filename to be run as the new task, but it can be any monitor command or can
 * be NULL.
 *
 * @param algrthm  Scheduling algorithm (ROUND_ROBIN, FIFO, or ADAPTIVE).
 * @param priority task priority.
 * @param port     output port number.
 * @param mem      The task size in 1K increments.
 * @param command  name of the file to run as the new task.
 * @param arg_str  command line argument string.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlCreateTask (const int algrthm, const int priority, int port, int mem,
						const char *command, const char *arg_str);


/**
 * KrnlDebug() sets the module DebugFlag variable to the value passed in as a
 * parameter.  BposDebug interprets a debug mask and calls this routine to set the
 * kernal debug status.
 *
 * @param action
 * @return
 * @since Version 1.0
 */
KRNL_EXT_DEF void KrnlDebug (int action);


/**
 * KrnlInit() initializes the kernal module for use.  It installs signal handlers,
 * initializes some global pointers, etc.  This function is called by BposInit() -
 * it should not be called directly from an application.
 *
 * @param sem_addr_from_bpos
 *               shared memory address pointer of the semaphore array (address is determined by
 *               bpos).
 * @param mem_size_from_bpos
 *               size of the shared memory segment required by bpos.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlInit (BP_SEMAPHORE *sem_addr_from_bpos,
						   int mem_size_from_bpos);
			
/**
 * KrnlClose() reverses any allocations or creations performed by KrnlInit(). This
 * function is called by BposClose - it should not be called directly from an
 * application.
 *
 * @return
 * @since Version 1.0
 */
KRNL_EXT_DEF void KrnlClose (void);


/**
 * KrnlLockTask() locks a task.  A locked task will prevent any other tasks from
 * receiving CPU time.  Normal task scheduling is suspended.
 *
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlLockTask (void);


/**
 * unlocks a task.  Normal task scheduling is resumed.
 *
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlUnlockTask (void);


/**
 * KrnlSemTestWait() tests a semaphore to see if it has a positive count SF and
 * waits on the semaphore if it has value >= 1, reducing the count. The CPU will
 * be returned to the highest priority ready task.
 *
 * @param sem_number The semaphore ID to test and wait on.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlSemTestWait (int sem_number);


/**
 * KrnlSemWait() verifies that the semaphore number is valid and then calls a
 * function to implement the semaphore wait process. It also handles error
 * detection and reporting for the semaphore wait process.
 *
 * @param sem_number The semphore ID to suspend on.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlSemWait (int sem_number);


/**
 * KrnlSemWaitWithTO() verifies that the semaphore number is valid and then calls
 * a function to implement the semaphore wait process with a specified timeout
 * period.  It also handles error detection and reporting for the semaphore wait
 * process.
 *
 * @param sem_number The semphore ID to suspend on.
 * @param timeout    Maximum # of milliseconds before event is set to awaken task.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlSemWaitWithTO (int sem_number, long timeout);


/**
 * KrnlSemSignal() verifies that the semaphore number is valid and then calls a
 * function to implement the semaphore signal process. It also handles error
 * detection and reporting for the semaphore signal process.
 *
 * @param sem_number The semphore ID to suspend on.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlSemSignal (int sem_number);


/**
 * KrnlSemCreate() creates a semaphore with the specified number of counts
 * (signals) and the specified ID.
 *
 * @param sem_number The semphore ID to create.
 * @param sem_count  The number of signals on the created semaphore.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlSemCreate (int sem_number, int sem_count);


/**
 * deletes the specified semaphore.
 *
 * @param sem_number The semphore ID to delete.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlSemDelete (int sem_number);


/**
 * creates a system timer which can be used by KrnlStartTimer() and
 * KrnlStopTimer().  These timers run for a specified time period and then expire.
 * When they expire, they assert a QNX signal.
 *
 * @return If successful   : Timer ID number.
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF timer_t KrnlCreateTimer (void);


/**
 * OBSOLETE - use pulse family functions
 *
 * @param ipProxy   OBSOLETE - use pulse family functions
 * @param iPid      OBSOLETE - use pulse family functions
 * @param cpMessage OBSOLETE - use pulse family functions
 * @param iBytes    OBSOLETE - use pulse family functions
 * @param iPriority OBSOLETE - use pulse family functions
 * @return OBSOLETE - use pulse family functions
 * @since Version 1.0
 */
KRNL_EXT_DEF timer_t KrnlCreateTimerWithProxy( int *ipProxy , pid_t iPid , char *cpMessage , int iBytes , int iPriority );


/**
 * KrnlStartTimer() sets a timer to expire after a specified time period has
 * elapsed, and then starts the timer.
 *
 * @param timer_id   The id number of the timer to be started.
 * @param iDelayTime The length of time before the first time out. [ms]
 * @param iRepTime   Repatition rate of following time outs.       [ms]
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlStartTimer (timer_t timer_id, long iDelayTime , long iRepTime );


/**
 * KrnlStopTimer() stops a timer before it expires.  When a timer is stopped, a
 * signal is not asserted by QNX.
 *
 * @param timer_id The id number of the timer to be stopped.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlStopTimer (timer_t timer_id);


/**
 * Deletes a timer.
 *
 * @param timer_id The id number of the timer to be stopped.
 * @return If successful   : 0
 *         else            : error code (see bep.h).
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlDeleteTimer (timer_t timer_id);


/**
 * OBSOLETE - use pulse family functions
 *
 * @param timer_id OBSOLETE - use pulse family functions
 * @param iProxy   OBSOLETE - use pulse family functions
 * @return OBSOLETE - use pulse family functions
 * @since Version 1.0
 */
KRNL_EXT_DEF int KrnlDeleteTimerWithProxy( timer_t timer_id , int iProxy );


/**
 * OBSOLETE - use pulse family functions
 *
 * @param iPid      OBSOLETE - use pulse family functions
 * @param cpMessage OBSOLETE - use pulse family functions
 * @param iBytes    OBSOLETE - use pulse family functions
 * @param iPriority OBSOLETE - use pulse family functions
 * @return OBSOLETE - use pulse family functions
 * @since Version 1.0
 */
KRNL_EXT_DEF pid_t KrnlCreateProxy( pid_t iPid , char *cpMessage , int iBytes , int iPriority );


/**
 * OBSOLETE - use pulse family functions
 *
 * @param iPid      OBSOLETE - use pulse family functions
 * @return OBSOLETE - use pulse family functions
 * @since Version 1.0
 */
KRNL_EXT_DEF pid_t KrnlDeleteProxy( pid_t iProxy );


/**
 * OBSOLETE - use pulse family functions
 *
 * @param iPid      OBSOLETE - use pulse family functions
 * @return OBSOLETE - use pulse family functions
 * @since Version 1.0
 */
KRNL_EXT_DEF pid_t KrnlTriggerProxy( pid_t iProxy );


/**
 * KrnlActivateAbort() sets the SIGABRT signal on all processes in the same process
 * group as the module that called it.  It then sets the abort flag (passed as a
 * parameter) to TRUE.
 *
 * @param abort  Pointer to the abort flag (passed from bpos).
 * @return
 * @since Version 1.0
 */
KRNL_EXT_DEF void KrnlActivateAbort (int *abort);


/**
 * sets the abort flag (passed as a parameter) to FALSE.
 *
 * @param abort  Pointer to the abort flag (passed from bpos).
 * @return
 * @since Version 1.0
 */
KRNL_EXT_DEF void KrnlClearAbort (int *abort);


/**
 * KrnlLog() is used like printf, but coordinates the messages so they print in
 * the proper order.
 *
 * @param format_str Pointer to printf style format string.
 * @return
 * @since Version 1.0
 */
KRNL_EXT_DEF void KrnlLog (char *format_str, ...);

/**
 * Sends a message to another task and waits for a reply message. See KrnlReply().
 * Allows for sending multi-part messages ((i.e. header/body)
 *
 * @param iPid       If zero then message will be accepted from any process, else messages will be
 *                   accepted from only that task indicated.
 * @param uSmsgParts Number of entries in the spSmsg array
 * @param uRmsgParts Number of entries in the spRmsg array
 * @param spSmsg     Message parts to be sent
 * @param spRmsg     Where to receive reply message
 * @return -1  No message waiting or error.
 *         0<  Process ID of task that sent the message.
 * @since Version 1.0
 */
int KrnlSendmx(int          iPid,
				unsigned    uSmsgParts,
				unsigned    uRmsgParts,
				const void  *spSmsg,
				const void  *spRmsg );

/**
 * Wait until a message is received.Allows for receiving messages in more than one
 * part (i.e. header/body)
 *
 * @param iPid   If zero then message will be accepted from any process, else messages will be
 *               accepted from only that task indicated.
 *
 * @param uParts Number of items in spRmsg
 * @param spRmsg Buffer to receive message parts into
 * @return -1  No message waiting or error.
 *         0<  Process ID of task that sent the message.
 * @since Version 1.0
 */
int KrnlReceivemx( pid_t        iPid,
                   unsigned     uParts,
                   void  		*spRmsg);

/**
 * Sends a reply message to a task that is blocked from having sent a message
 * using the KrnlSend() function. Allows replying with multipart messages
 * (i.e. header/body).
 *
 * @param iPid       The process ID that the reply message is to be sent to.
 * @param uRmsgParts Number of entries in the __rmsg array
 * @param spRmsg     Pointer to message parts to reply with
 * @return -1  Error
 *         0   Success
 * @since Version 1.0
 */
int KrnlReplymx(pid_t       iPid,
				unsigned    uRmsgParts,
				void  		*spRmsg );

/**
 * Checks to see if there is message being sent to the calling task.  The function
 * always returns whether or not there is a message being sent. Allows for
 * receiving multipart messages (i.e. header/body).
 *
 * @param iPid       If zero then message will be accepted from any process, else messages will be
 *                   accepted from only that task indicated.
 *
 * @param sRmsgParts Pointer to where response is to be placed.
 * @param spRmsg     Length of receive buffer.
 * @return -1  No message waiting or error.
 *         0<  Process ID of task that sent the message.
 * @since Version 1.0
 */
int KrnlCreceivemx(pid_t        iPid,
                   unsigned     sRmsgParts,
                   void  		*spRmsg );

/**
 * Relays a message received from process A to process B as if the original message
 * had been sent directly from Pprocess A to process B.
 *
 * @param iPid1  Process which originall ysent the message
 * @param iPid2  Process to which the message will be relayed
 * @param len
 * @param data
 * @return -1  Source or Target does not exist or aren't waiting for a message
 *         0   Message properly relayed from A to B
 * @since Version 1.0
 */
int KrnlRelay(	pid_t iPid1, pid_t iPid2, int len, char *data);

/**
 * Creates a new IPC commujnicattion channel for the calling process
 *
 * @param flags  Flag controlling how the kernel will use the channel to notify
 *               the calling process for certain events (disconnected from
 *               client, thread death, etc)
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 * @since Version 1.0
 */
int KrnlChannelCreate( unsigned int flags);

/**
 * Destroys the channel identified by channelId. This mnakes the channel
 * unavailable for client connections.
 *
 * @param channelId Channel to destroy
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 * @since Version 1.0
 */
int KrnlChannelDestroy( int channelId);

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
 * @since Version 1.0
 */
int KrnlConnectAttach(unsigned long nd, pid_t pid, int chid, unsigned index, int flags);

/**
 * Closes an open connection
 *
 * @param connectId Connection to be closed
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 * @since Version 1.0
 */
int KrnlConnectDetach( int connectId);

/**
 * Sends a pulse to a channel connection
 *
 * @param coid     Connection to send the pulse to
 * @param priority Priority at which to send the pulse
 * @param code     Pulse code to be sent
 * @param value    Value of the pulse to be sent
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 * @since Version 1.0
 */
int KrnlSendPulse(int coid, int priority, int code, int value);

/**
 * Creates a channel and connection to that channel within the calling process.
 * This is usefule when creating a timer with a pulse.
 *
 * @param channelId Buffer into which the newly created channel ID willbe stored
 * @param connectId Buffer into which the newly created connection ID willbe stored
 * @return 0 if successful
 *         -1 if error (use errrno for more error information)
 * @since Version 1.0
 */
int KrnlCreateSelfAttachment( int *channelId, int *connectId);

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
 * @since Version 1.0
 */
timer_t KrnlCreateTimerWithPulse( int *channelId, int *connectId, int pulseCode);

/**
 * Destroys a previously created timer and it's associated pulse
 *
 * @param timerId   Timer to destroy
 * @param channelId IPC channel id use to deliver the pulse
 * @param connectId IPC connection id used for the channel
 * @return 0 if successful
 *         -1 if error (use errno for more error information)
 * @since Version 1.0
 */
int KrnlDeleteTimerWithPulse( timer_t timerId, int channelId, int connectId);

/**
 * Creates a timer and aataches it to the given channel
 *
 * @param channelId Channel to attach the timer pulse to
 * @param connectId Connection ID for this attachment (NULL if not needed)
 * @param timerCode Timer Pulse code or signal number.
 * @param value     Timer Pulse value.
 * @return timer ID if successful
 *         -1 if error
 * @since Version 1.0
 */
int KrnlAttachTimerToChannel( int channelId, int *connectId, int timerCode, int value);

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
int KrnlDeliverEvent(int rcvid, const struct sigevent* event );

/**
 * Send and event to the specified channel.
 * 
 * @param coid     The connection to send the event to.
 * @param priority The priority to send the event at.
 * @param code     The code to associate with the event.
 * @param value    The value to associate with the event.
 * @return The status of the operation.
 */
int KrnlSendEvent(int coid, int priority, int code, int value );

/**
 * Wait for an event to be sent to the specified channel.
 * 
 * @param chid   The channel to wait on.
 * @param pulse  The pulse information that was received.
 * @param bytes  The number of bytes that were received.
 * @param info   Information about the pulse (not updated currently)
 * @return The status of the operation.
 */
int KrnlReceiveEvent(int chid, void* pulse, int bytes, struct _msg_info* info );

/**
 * Checks if a pulse is one of the standard OS pulses and handles it accordingly
 *
 * @param pulse  Pointer to the pulse data (struct _pulse*)
 * @return 1 if pulse not handled (not a standard pules)
 *         0 if pulse was handled
 *         -1 if error
 * @since Version 1.0
 */
int KrnlHandlePulse( void* pulse);

/**
 * Yield CPU to another process at the same priority as the calling process.
 * @since Version 1.0
 */
void KrnlYield( void );

/**
 * Perform a command as if it were executed on the command line.
 *
 * @param command the command to execute on the command line.
 * @return -1 if and error occured
 * @since Version 1.1
 */
int KrnlSystemCommand(const char *command);

/**
 * Checks for the existance of a task in the system.
 *
 * @param taskName The name to check for.
 * @return The success or failure of the check for the tasks
 *         existance.
 * @since Version 1.2
 */
int KrnlIsTaskAlive(const char *taskName);

/**
 * Gets the value of an environment variable
 * 
 * @param var    Name of environment variable
 * @return The value of the environment variable (empty string if not found)
 * @since Version 17
 */
const char* KrnlGetEnv(const char* var);

#ifdef __cplusplus
}
#endif

// ********************************************************************
#endif	// KRNL_H
// end

