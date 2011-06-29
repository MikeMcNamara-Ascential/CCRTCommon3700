//******************************************************************************
// Description:
//  Classes used to manage a monitored task
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/Common/TaskMonTask.cpp $
// 
// 3     10/26/06 10:49a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the number of thread and file descriptors associated with each
// task.
// 
// 2     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 3     10/11/05 1:53p Bmeinke
// Inside UpdateMemoryUsage(), exclude MAP_PHYS regions from the memory
// size calculations
//
// 2     9/14/05 10:11a Bmeinke
// Added m_taskClass member to TaskMonTask_t structure to track which
// class a task belongs to
// Added GetClass() and SetClass() methods in order to access the new
// m_taskClass data member
//
// 1     7/20/05 8:24p Cward
// Added new TaskMon
//
// 1     5/17/05 6:57p Bmeinke
// Core R05051101 release with TaskMon updates
//
// 1     4/25/05 4:30p Bmeinke
//
// 1     4/25/05 4:11p Bmeinke
//
// 1     4/25/05 3:39p Bmeinke
// NewTaskMon task monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "TaskMonTask.h"

#include <stdio.h>          // *printf()
#include <unistd.h>         // open()
#include <fcntl.h>          // O_RDWR
#include <limits.h>         // _POSIX_PATH_MAX
#include <string.h>         // memcpy()
#include <sys/neutrino.h>   // ClockTime()
#include <sys/procfs.h>     // procfs_info
#include <sys/mman.h>       // PROT_EXEC
#include <sys/syspage.h>    // SYSPAGE_ENTRY( num_cpu)

#define MAX_SEGMENTS 512
procfs_mapinfo  g_memInfo[ MAX_SEGMENTS];

TaskMonTask::TaskMonTask()
{
    m_taskState = TASKSTATE_UNREG;
    m_taskPid = -1;
    m_cpuUsage = 0.0;
    m_taskClass = TASKMON_CLASS_END;
    m_threads = 0;
    m_fds = 0;

    m_taskName = "";
    m_lastSampleTime = 0;
    m_cpuTime = 0;
    m_aliveCount = 0;
    m_memoryUsage = 0LL;
    m_pidFd = -1;

    m_pidPath = "";
}

TaskMonTask::TaskMonTask( const TaskMonTask_t &copy)
{
    char    procPathBuff[ _POSIX_PATH_MAX+1];

    m_taskState = copy.m_taskState;
    m_taskPid   = copy.m_taskPid;
    m_cpuUsage = copy.m_cpuUsage;
    m_memoryUsage = copy.m_memoryUsage;
    m_threads = copy.m_threads;
    m_fds = copy.m_fds;
    m_taskClass = copy.m_taskClass;

    m_taskName  = "";
    m_lastSampleTime = 0;
    m_cpuTime = 0;
    m_aliveCount = 0;
    m_pidFd = -1;

    sprintf(procPathBuff, "/proc/%d/as", m_taskPid);
    m_pidPath = procPathBuff;
}

TaskMonTask::TaskMonTask( const std::string &name, pid_t pid, TaskState_t state,
                          TaskMonClass_t taskClass, const double &cpuUsage,
                          const uint64_t &memUsage, const uint32_t threads,
                          const uint32_t fds)
{
    char    procPathBuff[ _POSIX_PATH_MAX+1];

    m_taskState = state;
    m_taskPid = pid;
    m_cpuUsage = cpuUsage;
    m_memoryUsage = memUsage;
    m_threads = threads;
    m_fds = fds;
    m_taskClass = taskClass;

    m_taskName = name;
    m_lastSampleTime = 0;
    m_cpuTime = 0;
    m_aliveCount = 0;
    m_pidFd = -1;

    sprintf(procPathBuff, "/proc/%d/as", m_taskPid);
    m_pidPath = procPathBuff;
}

TaskMonTask::~TaskMonTask()
{
    close( m_pidFd);
}

/**
 * Read the tasks's CPU usage (in %)
 *
 * @return The tasks's CPU usage (in %)
 */
double TaskMonTask::GetCpuUsage() const
{
    return( m_cpuUsage);
}

/**
 * Set the tasks's CPU usage (in %)
 *
 * @param cpuUsage The tasks's CPU usage (in %)
 */
void TaskMonTask::SetCpuUsage( const double &cpuUsage)
{
    m_cpuUsage = cpuUsage;
}

/**
 * Calculate the CPU usage for this task
 *
 * @return The tasks's CPU usage (in %)
 */
double TaskMonTask::UpdateCpuUsage()
{
    procfs_info     procInfo;
    uint64_t        taskCpuTime = 0LL;
    uint64_t        currSampleTime;
    double          cpuTimeDelta;
    double          sampleDelta;

    // If this task is no longer registered
    if( TASKSTATE_UNREG == m_taskState)
    {
        m_cpuUsage = -0.001;
        m_threads = 0;
        m_fds = 0;
    }
    else
    {
        m_pidFd = OpenPid(m_pidFd);

        // Path to the task's procfs entry
        if(-1 != m_pidFd)
        {
            // Ask procnto for he process run times
            if( EOK != devctl( m_pidFd, DCMD_PROC_INFO, &procInfo, sizeof( procInfo), 0 ))
            {
                // Set the task state to 'unknown'
                m_taskState = TASKSTATE_END;
                close( m_pidFd);
                m_pidFd = -1;
            }
            else if(TASKSTATE_END == m_taskState)
            {
                m_taskState = TASKSTATE_RUN;
            }

            taskCpuTime = procInfo.stime + procInfo.utime;
            m_threads = procInfo.num_threads;
            m_fds = procInfo.num_fdcons;

            ClockTime( CLOCK_REALTIME, NULL, &currSampleTime);
        }
        // Unable to read PID information
        else
        {
            ClockTime( CLOCK_REALTIME, NULL, &taskCpuTime);
            m_cpuTime = taskCpuTime;
            currSampleTime = taskCpuTime;
            // If this task's last state was TASKSTATE_STOP
            if( TASKSTATE_STOP == m_taskState)
            {
                // Move the task to unregistered
                m_taskState = TASKSTATE_UNREG;
            }
            // If this task is not already unregistered
            if( TASKSTATE_UNREG != m_taskState)
            {
                // Set the task state to 'unknown'
                m_taskState = TASKSTATE_END;
            }
            m_threads = 0;
            m_fds = 0;

            close( m_pidFd);
            m_pidFd = -1;
        }

        // If this is the first reading we have taken
        if( 0 == m_lastSampleTime)
        {
            m_lastSampleTime = currSampleTime;
            m_cpuUsage = 0.0;
        }
        else
        {
            sampleDelta = (double)(currSampleTime - m_lastSampleTime);
            // Calculate what percentage of the total time (since the last sample) this process
            //  has used
            if( sampleDelta > 0)
            {
                cpuTimeDelta = 100.0 * (double)(taskCpuTime - m_cpuTime);
                m_cpuUsage = cpuTimeDelta/sampleDelta;
            }
            else
            {
                m_cpuUsage = -99.99;
            }

            m_cpuTime = taskCpuTime;
            m_lastSampleTime = currSampleTime;
        }
    }

    if( m_cpuUsage > 100.0)
    {
        m_cpuUsage = 100.0;
    }

    return( m_cpuUsage);
}

/**
 * Read the tasks's memory usage (in bytes)
 *
 * @return The tasks's CPU usage (in bytes)
 */
const uint64_t& TaskMonTask::GetMemoryUsage() const
{
    return( m_memoryUsage);
}

/**
 * Set the tasks's memory usage (in bytes)
 *
 * @param cpuUsage The tasks's memory usage (in bytes)
 */
void TaskMonTask::SetMemoryUsage( const uint64_t &memUsage)
{
    m_memoryUsage = memUsage;
}

/**
 * Calculate the memory usage for this task
 *
 * @return The tasks's memory usage (in bytes)
 */
const uint64_t& TaskMonTask::UpdateMemoryUsage()
{
    int             actMemorySegs = 0;

    // If this task is still registered
    if( TASKSTATE_UNREG != m_taskState)
    {
        m_pidFd = OpenPid(m_pidFd);

        // Path to the task's procfs entry
        if(-1 != m_pidFd)
        {
            // Ask procnto for the process memory usage
            if( EOK != devctl( m_pidFd, DCMD_PROC_PAGEDATA, g_memInfo, sizeof( g_memInfo), &actMemorySegs))
            {
                // Set the task state to 'unknown'
                m_taskState = TASKSTATE_END;

                // Nothing read
                actMemorySegs = 0;

                close( m_pidFd);
                m_pidFd = -1;
            }
            else if(TASKSTATE_END == m_taskState)
            {
                m_taskState = TASKSTATE_RUN;
            }
        }
        // Unable to read PID information
        else
        {
            // Nothing read
            actMemorySegs = 0;

            close( m_pidFd);
            m_pidFd = -1;

            // If this task's last state was TASKSTATE_STOP
            if( TASKSTATE_STOP == m_taskState)
            {
                // Move the task to unregistered
                m_taskState = TASKSTATE_UNREG;
            }
            // If this task is not already unregistered
            if( TASKSTATE_UNREG != m_taskState)
            {
                // Set the task state to 'unknown'
                m_taskState = TASKSTATE_END;
            }
        }

        // If there are not more memory segments than we asked for
        if( actMemorySegs <= MAX_SEGMENTS)
        {
            // Reset usage data to 0
            m_memoryUsage = 0LL;

            // Count all memory segments
            while( actMemorySegs-- > 0)
            {
                // If this is not stack and is not executable and is not mmap'd
                if( !(g_memInfo[actMemorySegs].flags & (MAP_STACK | PROT_EXEC | MAP_PHYS)) )
                {
                    // Must be data...add to total
                    m_memoryUsage += g_memInfo[actMemorySegs].size;
                }
            }
        }
    }

    return( m_memoryUsage);
}

/**
 * Read the number of threads belonging to this process
 * 
 * @return The number of threads belonging to this process
 */
uint32_t TaskMonTask::GetThreadCount() const
{
    return( m_threads);
}
/**
 * Read the number of threads belonging to this process
 * 
 * @return The number of threads belonging to this process
 */
void TaskMonTask::SetThreadCount( uint32_t threadCount)
{
    m_threads = threadCount;
}

/**
 * Reads the number of file descriptors this process has open
 * 
 * @return The number of file descriptors this process has open
 */
uint32_t TaskMonTask::GetOpenFds() const
{
    return( m_fds);
}
/**
 * Sets the number of file descriptors this process has open
 * 
 * @param fds    The number of file descriptors this process has open
 */
void TaskMonTask::SetOpenFds( uint32_t fds)
{
    m_fds = fds;
}

/**
 * Read the tasks's registered name
 *
 * @return The tasks's registered name
 */
TaskMonClass_t TaskMonTask::GetClass() const
{
    return( m_taskClass);
}

/**
 * Set the tasks's registered name
 *
 * @param taskName The tasks's registered name
 */
void TaskMonTask::SetClass( const TaskMonClass_t &taskClass)
{
    m_taskClass = taskClass;
}

/**
 * Read the tasks's registered name
 *
 * @return The tasks's registered name
 */
std::string TaskMonTask::GetName() const
{
    return( m_taskName);
}

/**
 * Set the tasks's registered name
 *
 * @param taskName The tasks's registered name
 */
void TaskMonTask::SetName( const std::string &taskName)
{
    if( taskName != m_taskName)
    {
        m_taskName = taskName;
    }
}
/**
 * Read the tasks's process ID
 *
 * @return The tasks's process ID
 */
pid_t TaskMonTask::GetPid() const
{
    return( m_taskPid);
}

/**
 * Set the tasks's process ID
 *
 * @param taskPid Set the tasks's process ID
 */
void TaskMonTask::SetPid( const pid_t &taskPid)
{
    m_taskPid = taskPid;
}

/**
 * Read the tasks's current state
 *
 * @return The tasks's current state
 * @see TaskState_t
 */
TaskState_t TaskMonTask::GetState() const
{
    return( m_taskState);
}

/**
 * Set the tasks's current state
 *
 * @param taskState The tasks's current state
 *
 * @see TaskState_t
 */
void TaskMonTask::SetState( TaskState_t taskState)
{
    m_taskState = taskState;
}


int TaskMonTask::OpenPid(int &fd)
{
    if( -1 == fd)
    {
        fd = open(m_pidPath.c_str(), O_RDONLY);
    }

    return( fd);
}
