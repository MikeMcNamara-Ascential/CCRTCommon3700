//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Library/ITaskMon.cpp $
// $Author: Rwiersem $
//   $Date: 10/26/06 9:58a $
//
// Description:
//    TaskMon interface object
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/ITaskMon.cpp $
// 
// 4     10/26/06 9:58a Rwiersem
// Changes for the 06102701 core release:
// 
// Added GetTaskName() and GetTaskClass().
// 
// 3     2/15/06 7:52p Cward
// Adding suuport for Read Task Info
//
// 2     11/11/05 4:56p Cward
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
//
//******************************************************************************

#include "TaskMonMssgDefs.h"
#include "ITaskMon.h"
#include "BepDefs.h"    // SYSTEM_PULSE

#include <stdio.h>      // sprintf
#include <unistd.h>     // open, close
#include <fcntl.h>      // O_RDWR
#include <string.h>     // strncpy
#include <libgen.h>     // basename

/**
 * Constructor
 *
 * @param procName  Default process name of the process to report to TaskMon
 *                  for the Register() and ReportUp() methods
 * @param taskClass Class to which this task belongs (deafult=TASKMAN_CLASS_CORE)
 *
 * @see TaskMonClass_t
 */
ITaskMon::ITaskMon( const char *procName/*=NULL*/, TaskMonClass_t taskClass/*=TASKMON_CLASS_CORE*/) :
    m_taskMonPath( "/dev/TaskMon/TaskMon"), m_taskClass(taskClass), m_taskMonFd( -1)
{
    if( procName != NULL)   m_procName = basename( const_cast<char*>(procName));
    // Try to get an fd to TaskMon
    m_taskMonFd = GetTaskMonFd();
}

/**
 * Destructor. Sends a ReportTerminate() message to TaskMon
 */
ITaskMon::~ITaskMon()
{
    if( -1 != m_taskMonFd)
    {
        ReportTerminate();
        close(m_taskMonFd);
        m_taskMonFd = -1;
    }
}

/**
 * Perform initial TaskMon registration
 *
 * @param procName  Process name for TaskMon to display
 * @param taskClass Class to which this task belongs (deafult=TASKMAN_CLASS_CORE)
 *
 * @return EOK if successful or error code if not successful
 * @see TaskMonClass_t
 */
int ITaskMon::Register( const char *procName, TaskMonClass_t taskClass)
{
    // Update process name
    if( procName != NULL)   m_procName = std::string( basename( const_cast<char*>(procName)));
    m_taskClass = taskClass;

    return( SendRegisterMessage(DCMD_TASKMON_CLIENT_REG));
}

/**
 * Tell TaskMon that we have been registered
 *
 * @param procName  Process name for TaskMon to display
 * @param taskClass Class to which this task belongs (deafult=TASKMAN_CLASS_CORE)
 *
 * @return EOK if successful or error code if not successful
 * @see TaskMonClass_t
 */
int ITaskMon::ReportUp( const char *procName, TaskMonClass_t taskClass)
{
    // Update process name
    if( procName != NULL)   m_procName = std::string( basename( const_cast<char*>(procName)));
    m_taskClass = taskClass;

    return( SendRegisterMessage(DCMD_TASKMON_CLIENT_UP));
}

/**
 * Tell TaskMon that we have received terminate pulse and will
 * be exiting soon.
 *
 * @return EOK if successful or error code if not successful
 */
int ITaskMon::ReportTerminate()
{
    return( SendEmptyMessage(DCMD_TASKMON_CLIENT_DOWN));
}

/**
 * Tell TaskMon that we have finished our cleanup and are exiting.
 *
 * @return EOK if successful or error code if not successful
 */
int ITaskMon::ReportDone()
{
    return( SendEmptyMessage(DCMD_TASKMON_CLIENT_UNREG));
}

/**
 * Get a file descriptor to TaskMon
 *
 * @return Valid file descriptor or -1 if error
 * @see open
 */
int ITaskMon::GetTaskMonFd()
{
    return( open( m_taskMonPath.c_str(), O_RDWR));
}

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
int ITaskMon::SendRegisterMessage( int mssgCode)
{
    int             retVal = EINVAL;
    TaskMonReg_t    *regMssg;
    char            *mssgBuff;

    // If we have a valid process name to send to TaskMon
    if( false == m_procName.empty())
    {
        int retries = 3;

        // Allocate the message buffer
        const int buffSize = m_procName.size() + sizeof( TaskMonReg_t);
        mssgBuff = new char[ buffSize];
        regMssg = (TaskMonReg_t*)mssgBuff;

        // Try up to 3 times or until successful
        while( (EOK != retVal) && (0 < retries))
        {
            // Fill in message data
            regMssg->taskClass = m_taskClass;
            regMssg->nameLen = m_procName.size();
            strcpy( regMssg->procName, m_procName.c_str());

            // Send the message
            retVal = devctl( m_taskMonFd, mssgCode, regMssg, buffSize, NULL);

            // If our fd is no longer valid
            if( EBADF == retVal)
            {
                // Try to get a new fd
                close( m_taskMonFd);
                m_taskMonFd = GetTaskMonFd();
            }

            retries--;
        }

        delete[] mssgBuff;
    }

    return( retVal);
}

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
int ITaskMon::SendEmptyMessage( int mssgCode)
{
    int retVal = EINVAL;
    int retries = 3;

    // Try up to 3 times or until successful
    while( (EOK != retVal) && (0 < retries))
    {
        // Send the message
        retVal = devctl( m_taskMonFd, mssgCode, NULL, 0, NULL);

        // If our fd is no longer valid
        if( EBADF == retVal)
        {
            // Try to get a new fd
            m_taskMonFd = GetTaskMonFd();
        }

        retries--;
    }

    return( retVal);
}

/**
 * Toggles the CPU monitoring functionality of TaskMon
 */
void ITaskMon::ToggleCPUMonitoring()
{
    if( -1 == m_taskMonFd)
    {
        m_taskMonFd = GetTaskMonFd();
    }

    MsgSendPulse( m_taskMonFd, SIGEV_PULSE_PRIO_INHERIT,
                  SYSTEM_PULSE, TIMER_PULSE+1);
}

/**
 * Toggles the memory monitoring functionality of TaskMon
 */
void ITaskMon::ToggleMemoryMonitoring()
{
    if( -1 == m_taskMonFd)
    {
        m_taskMonFd = GetTaskMonFd();
    }

    MsgSendPulse( m_taskMonFd, SIGEV_PULSE_PRIO_INHERIT,
                  SYSTEM_PULSE, TIMER_PULSE+2);
}

/**
 * Tells TaskMon to refresh his task list (i.e. remove any dead tasks)
 */
void ITaskMon::RefreshTaskList()
{
    if( -1 == m_taskMonFd)
    {
        m_taskMonFd = GetTaskMonFd();
    }

    MsgSendPulse( m_taskMonFd, SIGEV_PULSE_PRIO_INHERIT,
                  SYSTEM_PULSE, TIMER_PULSE+3);
}

/**
 * Return the name used to register with TaskMon
 * 
 * @return The name used to register with TaskMon
 */
const std::string& ITaskMon::GetTaskName() const
{
    return( m_procName);
}

/**
 * Return the class used to register with TaskMon
 * 
 * @return The class used to register with TaskMon
 */
const TaskMonClass_t ITaskMon::GetTaskClass() const
{
    return( this->m_taskClass);
}

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
int ITaskMon::ReadTaskInfo( TaskMonTask_t &taskInfo, const char *taskName, TaskMonClass_t taskClass)
{
    char        taskPath[ _POSIX_PATH_MAX+1];
    const char  *className;
    int         fd;
    int         retVal = -1;

    // Convert TaskClass type into subdir name
    switch( taskClass)
    {
    case TASKMON_CLASS_CORE:        className = "Core/";        break;
    case TASKMON_CLASS_SERIAL:      className = "Serial/";      break;
    case TASKMON_CLASS_COMPONENT:   className = "Component/";   break;
    default:                        className = "";             break;
    }

    sprintf(taskPath, "/dev/TaskMon/Tasks/%s%s", className, taskName);
    // Open the "file" for the desired task
    fd = open( taskPath, O_RDONLY);
    if( -1 != fd)
    {
        // Read the task information
        retVal = read( fd, &taskInfo, sizeof( taskInfo));
        // If wrong number of bytes read
        if( retVal != sizeof( taskInfo))
        {
            // Error
            retVal = -1;
        }

        close( fd);
    }

    return( retVal);
}


