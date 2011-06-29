//******************************************************************************
// Description:
//  Class used to manage a list of tasks being monitored
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/Common/TaskMonTaskList.h $
// 
// 3     10/26/06 10:51a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the number of thread and file descriptors associated with each
// task.
// 
// 2     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 2     9/14/05 10:13a Bmeinke
// Added a taskClass parameter to the TaskList::Add() methods
//
// 1     7/20/05 8:24p Cward
// Added new TaskMon
//
// 1     5/17/05 6:57p Bmeinke
// Core R05051101 release with TaskMon updates
//
// 1     4/25/05 4:29p Bmeinke
//
// 1     4/25/05 4:11p Bmeinke
//
// 1     4/25/05 3:39p Bmeinke
// NewTaskMon task monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _TASKMONTASKLIST_H_INCLUDED_
#define _TASKMONTASKLIST_H_INCLUDED_

#include "TaskMonDefs.h"
#include "TaskMonTask.h"

/**
 * Data type used to hold manage the list of registered client tasks
 */
typedef std::map< std::string, TaskMonTask*>    TaskList_t;
typedef TaskList_t::iterator                    TaskListItr_t;
typedef TaskList_t::const_iterator              TaskListCItr_t;
typedef TaskList_t::value_type                  TaskListVal_t;

class TaskList : public TaskList_t
{
public:
    TaskList();

    ~TaskList();

    /**
     * Adds a new client to the list
     *
     * @param taskName  Name of the new task
     * @param taskPid   Process ID for the new task
     * @param taskState State of the task (default=TASKSTATE_REG)
     * @param taskClass Class this task belongs to
     * @param flags     Flag indicating which fields (if any) should be updated if
     *                  a task with a matching name already exists in the list
     *                  (default=TASKUPDATE_NONE)
     *
     * @return A reference to the newly allocated task or NULL we cannot allocate a new task
     * @see TaskMonTask
     * @see TaskState_t
     */
    TaskMonTask* Add( const std::string &taskName, pid_t taskPid,
                      TaskState_t taskState, TaskMonClass_t taskClass,
                      TaskUpdateFlags_t flags);

    /**
     * Adds a new client to the list
     *
     * @param newTask New task to be added
     * @param flags   Flag indicating which fields (if any) should be updated if
     *                a task with a matching name already exists in the list
     *                (default=TASKUPDATE_NONE)
     *
     * @return A reference to the newly allocated task or NULL we cannot allocate a new task
     * @see TaskMonTask
     * @see TaskState_t
     */
    TaskMonTask* Add( const TaskMonTask &newTask, TaskUpdateFlags_t flags=TASKUPDATE_NONE);

    /**
     * Updates the CPU statistics for the given task in the task list
     *
     * @param taskName Name of the task to be removed
     *
     * @return true if the task was removed
     *         false if the task could not be removed
     */
    bool Remove( const std::string &taskName);

    /**
     * Updates the CPU statistics for the given task in the task list
     *
     * @param newTask Task to be removed from the list (based solely on the taskName
     *                field)
     *
     * @return true if the task was removed
     *         false if the task could not be removed
     */
    bool Remove( const TaskMonTask &newTask);

    /**
     * Updates the CPU statistics for the given task in the task list
     *
     * @param newTask TeskEntry_t
     *
     * @return
     */
    bool Update( const TaskMonTask &newTask, int flags=TASKUPDATE_CPU|TASKUPDATE_THREADS|TASKUPDATE_FDS);
};

#endif // _TASKMONTASKLIST_H_INCLUDED_
