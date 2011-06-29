//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Include/TaskMonMssgDefs.h $
// $Author: Cward $
//   $Date: 7/20/05 8:21p $
//
// Description:
//    devctl message definitions fro TaskMon
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/TaskMonMssgDefs.h $
// 
// 1     7/20/05 8:21p Cward
// Added New taskmon. Fixed discrepancies between autoconf and CMake
// builds
// 
// 1     5/17/05 6:56p Bmeinke
// Core R05051101 release with TaskMon updates
// 
// 1     4/25/05 3:24p Bmeinke
// Interface to TaskMon process monitor
// 
//
//******************************************************************************

#ifndef _TASKMONMSSGDEFS_H_INCLUDED_
#define _TASKMONMSSGDEFS_H_INCLUDED_

#include "TaskMonDefs.h"	// TaskMonClass_t
#include <devctl.h>

/**
 * TaskMon devctl() class identifier
 */
#define		_CMD_IOCTL_TASKMON	0x1000

/**
 * Base message identifier for custom TaskMon devctl messages
 */
#define		TASK_MON_MSSG_BASE	0x00

/**
 * Message ID's for TaskMon custom devctl messages
 */
typedef enum
{
	/**
	 * Inform TaskMon that a new process is just starting
	 * Code:    DCMD_TASKMON_CLIENT_REGISTER
	 * Input:   TaskMonReg_t
	 * Output:  None
	 */
	TASK_MON_CLIENT_REG = TASK_MON_MSSG_BASE,

	/**
	 * Inform TaskMon that a new process has successfully been registered
	 * Code:    DCMD_TASKMON_CLIENT_RUNNING
	 * Input:   TaskMonReg_t
	 * Output:  None
	 */
	TASK_MON_CLIENT_UP,

	/**
	 * Inform TaskMon that a process is beginning to shutdown
	 * Code:	DCMD_TASKMON_CLIENT_DOWN
	 * Input:   None
	 * Output:  None
	 */
	TASK_MON_CLIENT_DOWN,

	/**
	 * Inform TaskMon that a process is done (about to exit)
	 * Code:	DCMD_TASKMON_RUNNING
	 * Input:   None
	 * Output:  None
	 */
	TASK_MON_CLIENT_UNREG
} TaskMonDcmd_t;

// Pack structs tightly
#include <_pack1.h>

/**
 * Message sent to TaskMon to register a new process
 */
typedef struct
{
	/**
	 * Class to which this task belongs
	 * 
	 * @see TaskMonClass_t
	 */
	TaskMonClass_t	taskClass;
	
	/**
	 * Length of the procName member
	 */
	int				nameLen;

	/**
	 * NULL terminated process name for TaskMon to display
	 */
	char			procName[1];
} TaskMonReg_t;

// Default structure packing
#include <_packpop.h>

/**
 * devctl() DCMD used to initially register a process with TaskMon
 * Input: TaskMonReg_t
 * Output: None
 */
#define DCMD_TASKMON_CLIENT_REG		__DIOT (_CMD_IOCTL_TASKMON, TASK_MON_CLIENT_REG, TaskMonReg_t)

/**
 * devctl() DCMD used to inform TaskMon that a process has been registered
 * Input: TaskMonReg_t
 * Output: None
 */
#define DCMD_TASKMON_CLIENT_UP		__DIOT (_CMD_IOCTL_TASKMON, TASK_MON_CLIENT_UP, TaskMonReg_t)

/**
 * devctl() DCMD used to inform TaskMon that a process is beginning to shutdown
 * Input: None
 * Output: None
 */
#define DCMD_TASKMON_CLIENT_DOWN	__DION (_CMD_IOCTL_TASKMON, TASK_MON_CLIENT_DOWN)

/**
 * devctl() DCMD used to inform TaskMon that a process is done (about to exit)
 * Input: None
 * Output: None
 */
#define DCMD_TASKMON_CLIENT_UNREG	__DION (_CMD_IOCTL_TASKMON, TASK_MON_CLIENT_UNREG)

#endif // _TASKMONMSSGDEFS_H_INCLUDED_

