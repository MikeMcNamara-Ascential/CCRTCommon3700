//******************************************************************************
// Description:
//  TaskMonPanel object specialized to displays a list of tasks within a common
//  task class.
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonTaskPanel.cpp $
// 
// 3     10/26/06 11:00a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the number of thread and file descriptors associated with each
// task.
// 
// 2     4/13/06 3:59a Cward
// Update task Mon term for scolling 
//
// 4     4/05/06 11:18a Bmeinke
// Change the title color to white on red to indicate there a re dead
// tasks displayed off screen
// Override the GetBoxChars() method to draw a pseudo scroll bar if we
// have more tasks than rows
// Disable scrolling in our constructor (scrolls manually)
//
// 3     4/04/06 4:55p Bmeinke
// Fixed the scrolling logic
//
// 2     3/20/06 10:54a Bmeinke
// Update() now properly erases any stale lines in a task panel
//
// 1     7/20/05 8:24p Cward
// Added new TaskMon
//
// 2     5/19/05 11:22a Bmeinke
// Fixed not specifying the data field color in memory display mode
//
// 1     5/17/05 6:57p Bmeinke
// Core R05051101 release with TaskMon updates
//
// 2     5/16/05 12:12p Bmeinke
// Use the newly defined CPU color levels
// If a task name is longer than the task name field width, don't try to
// allocate a space padding string with a negative length
//
// 1     4/25/05 4:11p Bmeinke
//
// 1     4/25/05 3:39p Bmeinke
// NewTaskMon task monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "TaskMonTaskPanel.h"

#include <string>
#include <string.h>

/**
 * Construct a new task list panel with 'lines' rows and 'cols' columnss with a top-left
 * screen corner of (top,left)
 *
 * @param title   Window title (default="TaskPanel")
 * @param rows    Number of rows for this panel or -1 for full terminal height
 *                (default=terminal row count)
 * @param columns Number of columns for this panel or -1 for full terminal width
 *                (default=terminal column count)
 * @param begin_y Defines the top left corner of the window (in conjunction with 'left')
 *                (default=0)
 * @param begin_x Defines the top left corner of the window (in conjunction with 'top')
 *                (default=0)
 */
TaskMonTaskPanel::TaskMonTaskPanel( const std::string &title, int rows, int columns, int begin_y, int begin_x) :
    TaskMonPanel( title, rows, columns, begin_y, begin_x), m_lastTaskCount( 0), m_topTask(0), m_haveDeadTasks( false)
{
    m_panelTitle = m_title;

    // We will manually scroll our window
    scrollok( m_window, false);
}

TaskMonTaskPanel::~TaskMonTaskPanel()
{
}

/**
 * Sets the title for the panel
 *
 * @param title
 */
void TaskMonTaskPanel::SetTitle( const std::string &title)
{
    m_panelTitle = title;
    TaskMonPanel::SetTitle( title);
}

/**
 * Method used to refresh/update the data in the panel
 *
 * @param taskList List of tasks in the panel
 * @param displayMode
 *                 What type of data should we display for each process
 * @param keepDead Flag to indicate if we want to display dead tasks
 */
void TaskMonTaskPanel::Update(const TaskList &taskList, TaskMonDisplayMode_t displayMode, bool keepDead)
{
    TaskListCItr_t  taskItr;
    int             currLine = m_top;
    char            junk[ m_cols+2];
    std::string     textLine("");
    int             dataColor = GetWindowColor();
    double          cpuUsage;
    uint64_t        memUsage;
    uint32_t        threads, fds;
    char            memSfx[] = " kmgt";
    int             ii;

    m_haveDeadTasks = false;

    // Erase stale tasks
    MoveTo( m_top, m_left);
    ClearToBottom();

    // If the task count has changed
    if( taskList.size() != m_lastTaskCount)
    {
        // If fewer tasks than last time
        if( taskList.size() < m_lastTaskCount)
        {
            // Erase all tasks in our panel
            MoveTo( currLine, m_left);
            ClearToBottom();
        }
        // If the current task list is empty
        if( 0 == taskList.size())
        {
            // Update the task title
            snprintf( junk, m_cols+1, "%s", m_panelTitle.c_str());
        }
        else
        {
        // Update the task title
            snprintf( junk, m_cols+1, "%s (%d)", m_panelTitle.c_str(), taskList.size());
        }
        TaskMonPanel::SetTitle( junk);
    }

    // See if we have any dead tasks
    for( taskItr = taskList.begin(); taskItr!= taskList.end(); taskItr++)
    {
        const TaskMonTask   *taskEntry = taskItr->second;

        // If this is an empty task, move to the next task
        if( NULL != taskEntry)
        {
            TaskState_t taskState = taskEntry->GetState();
            if( taskState > TASKSTATE_END)      taskState = TASKSTATE_STOP;

            // If this task is dead
            if( (TASKSTATE_UNREG == taskState) || (TASKSTATE_END == taskState))
            {
                m_haveDeadTasks = true;
                break;
            }
        }
    }

    // Start with the first task
    taskItr = taskList.begin();

    // Move forward to the first visible task
    if( 0 != m_topTask) advance( taskItr, m_topTask);

    // Display the list of tasks
    for( ; taskItr!= taskList.end() && currLine <= m_rows; taskItr++)
    {
        const TaskMonTask   *taskEntry = taskItr->second;

        // If this is an empty task, move to the next task
        if( NULL == taskEntry)      continue;

        TaskState_t taskState = taskEntry->GetState();
        if( taskState > TASKSTATE_END)      taskState = TASKSTATE_STOP;

        // If this task is dead and we don't want to display dead tasks
        if( (false == keepDead) && ((TASKSTATE_UNREG == taskState) || (TASKSTATE_END == taskState)) )
        {
            continue;
        }

        textLine = "";

        // Only show the prefix char if the terminal does not support color
        if( (false == m_useColor) && (taskState != TASKSTATE_RUN))
        {
            if( taskState == TASKSTATE_REG)         textLine = "+";
            else if( taskState == TASKSTATE_STOP)   textLine = "*";
            else if( taskState == TASKSTATE_UNREG)  textLine = "-";
            else
            {
                textLine = "?";
                taskState = TASKSTATE_END;
            }
        }

        // Assume we have no CPU usage data to display
        junk[0] = 0;

        switch( displayMode)
        {
        case TASKMON_DISPLAY_NONE:
            break;
        // If we're showing CPU%
        case TASKMON_DISPLAY_CPU:
            // If task is registered (but not yet running)
            if( (taskState == TASKSTATE_REG) ||
                // or If task is running
                (taskState == TASKSTATE_RUN) ||
                // or If task is preparing to exit (but still running)
                (taskState == TASKSTATE_STOP))
            {
                cpuUsage = taskEntry->GetCpuUsage();
                sprintf( junk, " %.2f%%", cpuUsage);

                if( cpuUsage >= CPU_CRITICAL)       dataColor = CPU_RED;
                else if( cpuUsage >= CPU_WARN)      dataColor = CPU_MAGENTA;
                else if( cpuUsage >= CPU_LOW)       dataColor = CPU_YELLOW;
                else                                dataColor = GetWindowColor();
            }
            break;
        // IF we're displaying thread count
        case TASKMON_DISPLAY_THREADS:
            // If task is registered (but not yet running)
            if( (taskState == TASKSTATE_REG) ||
                // or If task is running
                (taskState == TASKSTATE_RUN) ||
                // or If task is preparing to exit (but still running)
                (taskState == TASKSTATE_STOP))
            {
                threads = taskEntry->GetThreadCount();
                sprintf( junk, " %4d", threads % 1000);
                dataColor = GetWindowColor();
            }
            break;
        // IF we're displaying FD count
        case TASKMON_DISPLAY_FDS:
            // If task is registered (but not yet running)
            if( (taskState == TASKSTATE_REG) ||
                // or If task is running
                (taskState == TASKSTATE_RUN) ||
                // or If task is preparing to exit (but still running)
                (taskState == TASKSTATE_STOP))
            {
                fds = taskEntry->GetOpenFds();
                sprintf( junk, " %4d", fds % 1000);
                dataColor = GetWindowColor();
            }
            break;
        case TASKMON_DISPLAY_MEM:
            // If task is registered (but not yet running)
            if( (taskState == TASKSTATE_REG) ||
                // or If task is running
                (taskState == TASKSTATE_RUN) ||
                // or If task is preparing to exit (but still running)
                (taskState == TASKSTATE_STOP))
            {
                memUsage = taskEntry->GetMemoryUsage();

                for(ii=0; memUsage > 8192 && ii < 5; ii++)
                    memUsage /= 1024;

                snprintf( junk, m_cols, " %lld%c", memUsage, memSfx[ii]);
                dataColor = GetWindowColor();
            }
            break;
        }

        // How many spaces after the task name to erase any stale text
        textLine += taskEntry->GetName();

        int padSpaces = m_cols-strlen(junk) - textLine.size() + 1;
        if( padSpaces > 0)  textLine += std::string(padSpaces, ' ');

        // set text color based on task state
        AttributeOff( A_BOLD);
        AttributeOn( COLOR_PAIR(taskState+1));
        Print(currLine, LEFT_JUSTIFY, textLine);

        if( junk[ 0] != 0)
        {
            // Print out the statistic field
            AttributeOn( COLOR_PAIR( dataColor) | A_BOLD);
            Print(currLine, RIGHT_JUSTIFY, "%s", junk);
        }

        currLine++;
    }

    // Back to white on black
    AttributeOff( A_BOLD);
    AttributeOn( COLOR_PAIR( GetWindowColor()));

    // Display the panel title
    TaskMonPanel::Update();

    // How many tasks did we draw this time
    m_lastTaskCount = taskList.size();
}

/**
 * Scrolls the windows contents by the specified number of lines
 *
 * @param lines  Number of lines to scroll the windows by
 */
void TaskMonTaskPanel::Scroll( int lines)
{
    // If scrolling down
    if( lines > 0)
    {
        // If we have more tasks than rows
        if( (int)m_lastTaskCount >= m_rows)
        {
            // What task would be at the top if we scroll
            uint32_t top = m_topTask + lines;
            // What task would be at the bottom if we scroll
            uint32_t bottom = top + m_rows;

            // If we are trying to scroll past the end of the list
            if( bottom > m_lastTaskCount)
            {
                top = m_lastTaskCount - m_rows;
            }
            m_topTask = top;
        }
    }
    else
    {
        // If want to scroll farther up than possible
        if( (m_topTask+lines) < 0)  m_topTask = 0;
        else                        m_topTask += lines;
    }
}

/**
 * Sets the default color index for this window's title
 */
int TaskMonTaskPanel::GetTitleColor() const
{
    int colorIdx = TaskMonPanel::GetTitleColor();

    // If we have tasks that are not displayed
    if( ((int)m_lastTaskCount >= m_rows) && (true == m_haveDeadTasks))
    {
        colorIdx = DEAD_TASKS;
    }

    return( colorIdx);
}

/**
 * Gets the characters to use for box drawing
 *
 * @param uLine    Upper edge vertical line character
 * @param bLine    Bottom edge vertical line character
 * @param lLine    Left edge horizontal line char
 * @param hLine    Right edge horizontal line char
 * @param ulCorner Upper left corner character
 * @param urCorner Upper right corner character
 * @param blCorner Bottom left corner character
 * @param brCorner Bottom right corner character
 */
void TaskMonTaskPanel::GetBoxChars( chtype &uLine, chtype &bLine,
                                    chtype &lLine, chtype &rLine,
                                    chtype &ulCorner, chtype &urCorner,
                                    chtype &blCorner, chtype &brCorner) const
{
    // Get the default box characters
    TaskMonPanel::GetBoxChars( uLine, bLine, lLine, rLine, ulCorner, urCorner, blCorner, brCorner);

    // If we are using a border and we have more tasks than rows
    if( (int)m_lastTaskCount > m_rows)
    {
        // Show a pseudo scroll bar
        rLine = ' ' | A_REVERSE;
    }
}

