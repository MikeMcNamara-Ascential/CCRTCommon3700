//******************************************************************************
// Description:
//  General definitions for task monitor
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/TaskMonDefs.h $
// 
// 5     10/26/06 9:21a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the number of thread and file descriptors associated with each
// task.
// 
// 4     4/13/06 3:38a Cward
// define DEAD_TASKS
//
// 2     4/05/06 10:55a Bmeinke
// Added DEAD_TASKS to the ColorCodes_t enumeration
//
// 3     2/15/06 7:52p Cward
// Adding suuport for Read Task Info
//
// 2     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 2     9/14/05 4:28p Bmeinke
// Added TASKUPDATE_CLASS to TaskUpdateFlags_t
//
// 1     7/20/05 8:21p Cward
// Added New taskmon. Fixed discrepancies between autoconf and CMake
// builds
//
// 1     5/17/05 6:56p Bmeinke
// Core R05051101 release with TaskMon updates
//
// 2     5/16/05 12:10p Bmeinke
// Added CpuLevels_t enum to define the different CPU color levels
//
// 1     4/25/05 3:24p Bmeinke
// Interface to TaskMon process monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _TASKMONDEFS_H_INCLUDED_
#define _TASKMONDEFS_H_INCLUDED_

#include <stdint.h>

#include <string>
#include <map>

/**
 * Task class definitions. Used to break the task list up into
 * 4 different classes.
 */
typedef enum
{
    /**
     * Core task class. Includes processes under the Servers tree and
     * some of the processes under the Managers tree (ICM, PLC)
     */
    TASKMON_CLASS_CORE=0,
    /**
     * Serial task class. Includes processes under the CommProxy tree
     */
    TASKMON_CLASS_SERIAL,
    /**
     * Component task class. Includes processes under the Components
     */
    TASKMON_CLASS_COMPONENT,
    /**
     * Marker to indicate the end (and the size) of the task class list
     */
    TASKMON_CLASS_END

} TaskMonClass_t;

/**
 * Task states:
 *  TASKSTATE_REG
 *  TASKSTATE_RUN
 *  TASKSTATE_STOP
 *  TASKSTATE_UNREG
 */
typedef enum
{
    /**
     * Task is registering (starting up)
     */
    TASKSTATE_REG,
    /**
     * Task is fully registered and running
     */
    TASKSTATE_RUN,
    /**
     * Task is preparing to shutdown
     */
    TASKSTATE_STOP,
    /**
     * Task is unregistered (done)
     */
    TASKSTATE_UNREG,
    /**
     * Marker to indicate the end (and the size) of the task state list
     */
    TASKSTATE_END

} TaskState_t;

/**
 * Flags used to tell what items in a TaskMonTask structure should
 * be updated:
 *  TASKUPDATE_NONE
 *  TASKUPDATE_NAME
 *  TASKUPDATE_PID
 *  TASKUPDATE_STATE
 *  TASKUPDATE_CPU
 *  TASKUPDATE_ALL
 *  TASKUPDATE_THREADS
 *  TASKUPDATE_FDS
 */
typedef enum
{
    /**
     * Do not update any fields
     */
    TASKUPDATE_NONE     = 0x0000,
    /**
     * Update only the task name
     */
    TASKUPDATE_NAME     = 0x0001,
    /**
     * Update only the task PID
     */
    TASKUPDATE_PID      = 0x0002,
    /**
     * Update only the task state
     */
    TASKUPDATE_STATE    = 0x0004,
    /**
     * Update only the task CPU statistics
     */
    TASKUPDATE_CPU      = 0x0008,
    /**
     * Update only the task memory statistics
     */
    TASKUPDATE_MEM      = 0x0010,
    /**
     * Update only the task class
     */
    TASKUPDATE_CLASS    = 0x0020,
    /**
     * Update only the task class
     */
    TASKUPDATE_THREADS  = 0x0040,
    /**
     * Update only the task class
     */
    TASKUPDATE_FDS      = 0x0080,
    /**
     * Update all task fields
     */
    TASKUPDATE_ALL      = 0xFFFF

} TaskUpdateFlags_t;



/**
 * Flags used to tell what data to display for each process
 *  TASKMON_DISPLAY_NONE
 *  TASKMON_DISPLAY_CPU
 *  TASKMON_DISPLAY_MEM
 *  TASKMON_DISPLAY_THREADS
 *  TASKMON_DISPLAY_FDS
 */
typedef enum
{
    /**
     * Display no data for each process
     */
    TASKMON_DISPLAY_NONE,

    /**
     * Display CPU usage for each process
     */
    TASKMON_DISPLAY_CPU,

    /**
     * Display memory usage for each process
     */
    TASKMON_DISPLAY_MEM,

    /**
     * Display number of threads
     */
    TASKMON_DISPLAY_THREADS,

    /**
     * Display number of file descriptors
     */
    TASKMON_DISPLAY_FDS

} TaskMonDisplayMode_t;

/**
 * CPU usage boundaries used to determine CPU field colors
 */
typedef enum
{
    /**
     * Low CPU usage
     */
    CPU_LOW = 10,

    /**
     * Moderate CPU usage
     */
    CPU_WARN = 50,
    /**
     * High CPU usage
     */
    CPU_CRITICAL = 80,

} CpuLevels_t;

/**
 * Color indices to be used when drawing text
 */
typedef enum
{
    /**
     * Default color (white on black)
     */
    DEFAULT_COLOR = 10,

    /**
     * Dead task border is red on black
     */
    DEAD_TASKS = 11,

    /**
     * CPU usage > 10%
     */
    CPU_YELLOW = 40,
    /**
     * CPU usage > 50%
     */
    CPU_MAGENTA = 41,
    /**
     * CPU usage > 80%
     */
    CPU_RED = 42,
    /**
     * Colors for the header panel
     */
    HEADER_WINDOW = 50,
    /**
     * Colors for the status panel
     */
    STATUS_WINDOW = 51,
    /**
     * Colors for a hilited key item
     */
    HILITE_KEY = 52

} ColorCodes_t;


#include <_pack1.h>

typedef struct
{
    /**
     * Current state of the task (registered, running, ending, dead)
     *
     * @see TaskState_t
     */
    TaskState_t m_taskState;

    /**
     * Process ID for the task
     */
    pid_t       m_taskPid;

    /**
     * Class that this task belongs to
     */
    TaskMonClass_t  m_taskClass;

    /**
     * Computed CPU utilization based on CPU time consumption
     */
    double      m_cpuUsage;

    /**
     * Total memory usage for this process (bytes)
     */
    uint64_t    m_memoryUsage;

    /**
     * Number of threads belonging to this process
     */
    uint32_t    m_threads;

    /**
     * Number of file descriptors this process has open
     */
    uint32_t    m_fds;

} TaskMonTask_t;

#include <_packpop.h>



#endif // _TASKMONDEFS_H_INCLUDED_

