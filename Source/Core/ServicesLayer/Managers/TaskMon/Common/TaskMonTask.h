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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/Common/TaskMonTask.h $
// 
// 5     10/30/07 4:06p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     10/26/06 3:45p Rwiersem
// Changes for the 06102701 core release:
// 
// Added the number of thread and file descriptors associated with each
// task.
// 
// 3     2/15/06 7:57p Cward
// Updated for 06021501 iteration
//
// 2     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 2     9/14/05 10:12a Bmeinke
// Added m_taskClass member to TaskMonTask_t structure to track which
// class a task belongs to
// Added taskClass parameter to the TaskMonTask constructor
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

#ifndef _TASKMONTASK_H_INCLUDED_
#define _TASKMONTASK_H_INCLUDED_

#include "TaskMonDefs.h"

#include <stdint.h>
#include <string>
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif
/**
 * Attributes for a registered task
 */
class TaskMonTask : protected TaskMonTask_t
{
    friend class TaskMonTaskTest;

public:
    TaskMonTask();

    TaskMonTask( const TaskMonTask_t &copy);

    TaskMonTask( const std::string &name, pid_t pid, TaskState_t state, TaskMonClass_t taskClass, 
                 const double &cpuUsage, const uint64_t &memUsage, const uint32_t threads,
                 const uint32_t fds);

    virtual ~TaskMonTask();

    /**
     * Read the tasks's CPU usage (in %)
     *
     * @return The tasks's CPU usage (in %)
     */
    double GetCpuUsage() const;
    /**
     * Set the tasks's CPU usage (in %)
     *
     * @param cpuUsage The tasks's CPU usage (in %)
     */
    void SetCpuUsage( const double &cpuUsage);
    /**
     * Calculate the CPU usage for this task
     *
     * @return The tasks's CPU usage (in %)
     */
    virtual double UpdateCpuUsage();

    /**
     * Read the tasks's memory usage (in bytes)
     *
     * @return The tasks's CPU usage (in bytes)
     */
    const uint64_t& GetMemoryUsage() const;
    /**
     * Set the tasks's memory usage (in bytes)
     *
     * @param memUsage The tasks's memory usage (in bytes)
     */
    void SetMemoryUsage( const uint64_t &memUsage);
    /**
     * Calculate the memory usage for this task
     *
     * @return The tasks's memory usage (in bytes)
     */
    virtual const uint64_t& UpdateMemoryUsage();

    /**
     * Read the number of threads belonging to this process
     * 
     * @return The number of threads belonging to this process
     */
    uint32_t GetThreadCount() const;
    /**
     * Read the number of threads belonging to this process
     * 
     * @return The number of threads belonging to this process
     */
    void SetThreadCount( uint32_t threadCount);

    /**
     * Sets the number of file descriptors this process has open
     * 
     * @return The number of file descriptors this process has open
     */
    uint32_t GetOpenFds() const;
    /**
     * Sets the number of file descriptors this process has open
     * 
     * @param fds    The number of file descriptors this process has open
     */
    void SetOpenFds( uint32_t fds);
    
    /**
     * Read the tasks's registered name
     *
     * @return The tasks's class.
     */
    TaskMonClass_t GetClass() const;
    /**
     * Set the tasks's registered name
     *
     * @param taskClass The tasks's class.
     */
    void SetClass( const TaskMonClass_t &taskClass);

    /**
     * Read the tasks's registered name
     *
     * @return The tasks's registered name
     */
    std::string GetName() const;
    /**
     * Set the tasks's registered name
     *
     * @param taskName The tasks's registered name
     */
    void SetName( const std::string &taskName);

    /**
     * Read the tasks's process ID
     *
     * @return The tasks's process ID
     */
    pid_t GetPid() const;
    /**
     * Set the tasks's process ID
     *
     * @param taskPid Set the tasks's process ID
     */
    void SetPid( const pid_t &taskPid);

    /**
     * Read the tasks's current state
     *
     * @return The tasks's current state
     * @see TaskState_t
     */
    TaskState_t GetState() const;
    /**
     * Set the tasks's current state
     *
     * @param taskState The tasks's current state
     *
     * @see TaskState_t
     */
    void SetState( TaskState_t taskState);

    int OpenPid(int &fd);

protected:
    /**
     * Name to display for the process
     */
    std::string     m_taskName;

    /**
     * Path to this process' information in the proc filesystem
     */
    std::string     m_pidPath;

    /**
     * Flag indicating if user has 'selected' this task
     */
    bool            m_selected;

    /**
     * Number of samples this task has been running for
     */
    int32_t         m_aliveCount;

    /**
     * Time at which the CPU times were last sampled
     */
    uint64_t        m_lastSampleTime;

    /**
     * Current CPU time consumption
     */
    uint64_t        m_cpuTime;

    /**
     * Time this process was started (Seconds since Jan 1, 1970)
     */
    uint64_t        m_startTime;

    /**
     * File descriptor for our PID data
     */
    int             m_pidFd;
};

#endif // _TASKMONTASK_H_INCLUDED_
