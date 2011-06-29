//******************************************************************************
//
// Description:
//    TaskMon interface object
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/ITaskMon.h $
// 
// 5     10/26/06 9:18a Rwiersem
// Changes for the 06102701 core release:
// 
// Added GetTaskName() and GetTaskClass().
// 
// 4     4/13/06 3:36a Cward
// Update header comments
//
// 3     2/15/06 7:52p Cward
// Adding suuport for Read Task Info
//
// 1     11/17/05 10:36a Bmeinke
// Updating Tata to latest core
//
// 2     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 2     9/14/05 10:21a Bmeinke
// Added new RefreshTaskList() method to tell TaskMon to remove stale
// tasks
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
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _ITASKMON_H_INCLUDED_
#define _ITASKMON_H_INCLUDED_

#include <string>
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif
#include "TaskMonDefs.h"

/**
 * Interface to the TaskMon process monitor.
 *
 * @author BMeinke
 */
class ITaskMon
{
public:
    /**
     * Constructor
     *
     * @param procName  Default process name of the process to report to TaskMon
     *                  for the Register() and ReportUp() methods
     * @param taskClass Class to which this task belongs (deafult=TASKMAN_CLASS_CORE)
     *
     * @see TaskMonClass_t
     */
    ITaskMon( const char *procName=NULL, TaskMonClass_t taskClass=TASKMON_CLASS_CORE);

    /**
     * Destructor. Sends a ReportTerminate() message to TaskMon
     */
    ~ITaskMon();

    /**
     * Perform initial TaskMon registration
     *
     * @param procName  Process name for TaskMon to display
     * @param taskClass Class to which this task belongs (deafult=TASKMAN_CLASS_CORE)
     *
     * @return EOK if successful or error code if not successful
     * @see TaskMonClass_t
     */
    int Register( const char *procName=NULL, TaskMonClass_t taskClass=TASKMON_CLASS_CORE);

    /**
     * Tell TaskMon that we have been registered
     *
     * @param procName  Process name for TaskMon to display
     * @param taskClass Class to which this task belongs (deafult=TASKMAN_CLASS_CORE)
     *
     * @return EOK if successful or error code if not successful
     * @see TaskMonClass_t
     */
    int ReportUp( const char *procName=NULL, TaskMonClass_t taskClass=TASKMON_CLASS_CORE);

    /**
     * Tell TaskMon that we have received terminate pulse and will
     * be exiting soon.
     *
     * @return EOK if successful or error code if not successful
     */
    int ReportTerminate();

    /**
     * Tell TaskMon that we have finished our cleanup and are exiting.
     *
     * @return EOK if successful or error code if not successful
     */
    int ReportDone();

    /**
     * Toggles the CPU monitoring functionality of TaskMon
     */
    void ToggleCPUMonitoring();

    /**
     * Toggles the memory monitoring functionality of TaskMon
     */
    void ToggleMemoryMonitoring();

    /**
     * Tells TaskMon to refresh his task list (i.e. remove any dead tasks)
     */
    void RefreshTaskList();

    /**
     * Return the name used to register with TaskMon
     * 
     * @return The name used to register with TaskMon
     */
    const std::string& GetTaskName() const;

    /**
     * Return the class used to register with TaskMon
     * 
     * @return The class used to register with TaskMon
     */
    const TaskMonClass_t GetTaskClass() const;

    /**
     * Read task information for the task with the given name in the given class
     *
     * @param taskInfo  Buffer to fill with task information
     * @param taskName  name of the task to get information for (i.e. DataServer)
     * @param taskClass Class to which the task belongs (i.e. TASKMON_CLASS_CORE)
     *
     * @return -1 if an error occurrs
     * @see TaskMonClass_t
     */
    static int ReadTaskInfo( TaskMonTask_t &taskInfo, const char *taskName, TaskMonClass_t taskClass=TASKMON_CLASS_END);

protected:
    /**
     * Get a file descriptor to TaskMon
     *
     * @return Valid file descriptor or -1 if error
     * @see open
     */
    int GetTaskMonFd();

    /**
     * Sends a register message to TaskMon. We send a register
     * message (TaskMonReg_t) is sent from the Register() and
     * ReportUp() methods
     *
     * @param mssgCode devctl command to send:
     *                  DCMD_TASKMON_CLIENT_REG or
     *                  DCMD_TASKMON_CLIENT_UP
     *
     * @return EOK if successful or error code if not successful
     */
    int SendRegisterMessage( int mssgCode);

    /**
     * Sends a register message to TaskMon. We send a register
     * message (TaskMonReg_t) is sent from the Register() and
     * ReportUp() methods
     *
     * @param mssgCode devctl command to send:
     *                  DCMD_TASKMON_CLIENT_DOWN or
     *                  DCMD_TASKMON_CLIENT_UNREG
     *
     * @return EOK if successful or error code if not successful
     */
    int SendEmptyMessage( int mssgCode);

private:
    /**
     * Pathname to TaskMon's device entry (i.e. /dev/TaskMon)
     */
    const std::string   m_taskMonPath;

    /**
     * Process name to register with TaskMon
     */
    std::string         m_procName;

    /**
     * Class to which this task belongs
     *
     * @see TaskMonClass_t
     */
    TaskMonClass_t      m_taskClass;

    /**
     * File descriptor used to communicate for TaskMon communication
     */
    int                 m_taskMonFd;
};

#endif // _ITASKMON_H_INCLUDED_

