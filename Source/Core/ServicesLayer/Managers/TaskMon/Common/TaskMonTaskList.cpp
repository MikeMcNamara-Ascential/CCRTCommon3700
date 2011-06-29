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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/Common/TaskMonTaskList.cpp $
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

#include "TaskMonTaskList.h"

TaskList::TaskList() : TaskList_t()
{
}

TaskList::~TaskList()
{
}

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
TaskMonTask* TaskList::Add( const std::string &taskName, pid_t taskPid,
                            TaskState_t taskState, TaskMonClass_t taskClass,
                            TaskUpdateFlags_t flags)
{
    TaskListItr_t   itr;
    TaskMonTask     *newTask = NULL;
    bool            retVal = false;

    // Make sure the task does not already exist in the list
    if( (itr=TaskList_t::find( taskName)) == TaskList_t::end())
    {
        newTask = new TaskMonTask( taskName, taskPid, taskState, taskClass, 0.0, 0, 0, 0);
        TaskList_t::insert( TaskListVal_t( taskName, newTask));
    }
    // If any fields need to be updated
    else if( flags & TASKUPDATE_ALL)
    {
        newTask = itr->second;
        retVal = Update( *newTask, flags);
    }

    return( newTask);
}

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
TaskMonTask* TaskList::Add( const TaskMonTask &newTask, TaskUpdateFlags_t flags)
{
    TaskListItr_t   itr;
    TaskMonTask*    retVal = NULL;

    // Make sure the task does not already exist in the list
    if( (itr=TaskList_t::find( newTask.GetName())) == TaskList_t::end())
    {
        retVal = new TaskMonTask( newTask);
        TaskList_t::insert( TaskListVal_t( newTask.GetName(), retVal));
    }
    // If any fields need to be updated
    else if( flags & TASKUPDATE_ALL)
    {
        Update( newTask, flags);
        retVal = itr->second;
    }

    return( retVal);
}

/**
 * Updates the CPU statistics for the given task in the task list
 *
 * @param taskName Name of the task to be removed
 *
 * @return true if the task was removed
 *         false if the task could not be removed
 */
bool TaskList::Remove( const std::string &taskName)
{
    TaskListItr_t   itr;
    bool            retVal = false;

    // Make sure the task already exist in the list
    if( (itr=TaskList_t::find( taskName)) != TaskList_t::end())
    {
        TaskMonTask*    oldTask = itr->second;
        TaskList_t::erase( itr);

        delete oldTask;

        retVal = true;
    }

    return( retVal);
}

/**
 * Updates the CPU statistics for the given task in the task list
 *
 * @param newTask Task to be removed from the list (based solely on the taskName
 *                field)
 *
 * @return true if the task was removed
 *         false if the task could not be removed
 */
bool TaskList::Remove( const TaskMonTask &newTask)
{
    TaskListItr_t   itr;
    bool            retVal = false;

    // Make sure the task already exist in the list
    if( (itr=TaskList_t::find( newTask.GetName())) != TaskList_t::end())
    {
        TaskMonTask*    oldTask = itr->second;
        TaskList_t::erase( itr);

        delete oldTask;

        retVal = true;
    }

    return( retVal);
}

/**
 * Updates the CPU statistics for the given task in the task list
 *
 * @param newTask TeskEntry_t
 *
 * @return
 */
bool TaskList::Update( const TaskMonTask &newTask, int flags)
{
    TaskListItr_t   itr;
    bool            retVal = false;

    // Make sure the task does not already exist in the list
    if( ( (itr=TaskList_t::find( newTask.GetName())) != TaskList_t::end()) &&
        ( flags & TASKUPDATE_ALL))
    {
        TaskMonTask     *curTask = itr->second;

        if( TASKUPDATE_PID & flags)     curTask->SetPid( newTask.GetPid());
        if( TASKUPDATE_STATE & flags)   curTask->SetState( newTask.GetState());
        if( TASKUPDATE_CPU & flags)     curTask->SetCpuUsage( newTask.GetCpuUsage());
        if( TASKUPDATE_MEM & flags)     curTask->SetMemoryUsage( newTask.GetMemoryUsage());
        if( TASKUPDATE_CLASS & flags)   curTask->SetClass( newTask.GetClass());
        if( TASKUPDATE_THREADS & flags) curTask->SetThreadCount( newTask.GetThreadCount());
        if( TASKUPDATE_FDS & flags)     curTask->SetOpenFds( newTask.GetOpenFds());

        retVal = true;
    }

    return( retVal);
}

