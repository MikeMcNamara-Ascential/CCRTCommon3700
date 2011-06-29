//******************************************************************************
// Description:
//  TaskMonTask object specialized for reading kernel CPU usage
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/Common/TaskMonProcTask.cpp $
// 
// 3     10/26/06 10:48a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the number of thread and file descriptors associated with each
// task.
// 
// 2     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 2     9/14/05 10:10a Bmeinke
// Added task class parameter when calling the base class constructor
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

#include "TaskMonProcTask.h"

#include <stdio.h>          // *printf()
#include <unistd.h>         // open()
#include <fcntl.h>          // O_RDWR
#include <limits.h>         // _POSIX_PATH_MAX
#include <string.h>         // memset()
#include <sys/neutrino.h>   // ClockTime()
#include <sys/procfs.h>     // procfs_info
#include <sys/syspage.h>    // SYSPAGE_ENTRY( num_cpu)

/**
 * Number of CPUs in the current computer
 */
const uint16_t TaskMonProcTask::m_cpuCount = _syspage_ptr->num_cpu;

TaskMonProcTask::TaskMonProcTask() : TaskMonTask( "TaskMon", 1, TASKSTATE_RUN, TASKMON_CLASS_CORE, 0.0, 0, 0, 0)
{
    m_threadSampleTime.reserve(TaskMonProcTask::m_cpuCount);
    m_threadSampleTime.resize(TaskMonProcTask::m_cpuCount);

    m_threadCpuTime.reserve(TaskMonProcTask::m_cpuCount);
    m_threadCpuTime.resize(TaskMonProcTask::m_cpuCount);
}

TaskMonProcTask::~TaskMonProcTask()
{
}

/**
 * Calculate the CPU usage for this task
 */
double TaskMonProcTask::UpdateCpuUsage()
{
    char            procPathBuff[ _POSIX_PATH_MAX+1];
    procfs_status   threadInfo;
    uint64_t        threadCpuTime;
    double          threadCpuUsage;
    uint64_t        currSampleTime;
    double          sampleDelta;
    int             fd;
    uint16_t        ii;

    m_cpuUsage = 0.0;
    // Path to the task's procfs entry
    sprintf(procPathBuff, "/proc/%d/as", m_taskPid);
    if((fd = open(procPathBuff, O_RDONLY)) != -1)
    {
        memset( &threadInfo, 0, sizeof( threadInfo));
        // Caluclate how much time the kernel's idle threads (priority=0) have consumed.
        //  This will tell us hown much spare CPU time the system has.
        for( ii=0; ii<TaskMonProcTask::m_cpuCount; ii++)
        {
            threadCpuTime = 0LL;
            threadCpuUsage = 0.0;

            threadInfo.tid = ii+1;
            if( EOK == devctl( fd, DCMD_PROC_TIDSTATUS, &threadInfo, sizeof( threadInfo), 0 ))
            {
                // If this is an idle thread
                if( threadInfo.priority == 0)
                {
                    // See how much total time this thread has run
                    threadCpuTime = threadInfo.sutime;

                    ClockTime( CLOCK_REALTIME, NULL, &currSampleTime);

                    // Calculate what percentage of the total time (since the last sample) this process
                    //  has used
                    sampleDelta = (double)(currSampleTime - m_threadSampleTime[ii]);
                    if( sampleDelta > 0)
                    {
                        // How much time has this thread run since the last sample
                        threadCpuUsage = (double)(threadCpuTime - m_threadCpuTime[ii]);
                        // What % of the last interval has this thread consumed
                        threadCpuUsage = (threadCpuUsage/sampleDelta);

                        if( threadCpuUsage < 0)     threadCpuUsage = 0;
                        if( threadCpuUsage > 100.0) threadCpuUsage = 100.0;

                        // Total idle thread CPU usage since last sample
                        m_cpuUsage += threadCpuUsage;
                    }

                    m_threadSampleTime[ii] = currSampleTime;
                    m_threadCpuTime[ii] = threadCpuTime;
                }
            }
        }

        close( fd);

        // System CPU usage will be whatever % of time the idle threads DID NOT run
        m_cpuUsage = 100 * (1 - (m_cpuUsage/TaskMonProcTask::m_cpuCount));
    }
    else
    {
        ClockTime( CLOCK_REALTIME, NULL, &threadCpuTime);
        m_cpuTime = threadCpuTime;
        currSampleTime = threadCpuTime;
    }

    return( m_cpuUsage);
}

