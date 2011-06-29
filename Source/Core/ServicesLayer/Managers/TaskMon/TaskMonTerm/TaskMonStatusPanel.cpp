//******************************************************************************
// Description:
//  Class used to display the status panel at the bottom of the main window
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonStatusPanel.cpp $
// 
// 2     10/26/06 11:00a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the number of thread and file descriptors associated with each
// task.
// 
// 1     7/20/05 8:24p Cward
// Added new TaskMon
// 
// 1     5/17/05 6:57p Bmeinke
// Core R05051101 release with TaskMon updates
// 
// 1     4/25/05 4:11p Bmeinke
// 
// 1     4/25/05 3:39p Bmeinke
// NewTaskMon task monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "TaskMonStatusPanel.h"

/**
 * Construct a new TaskMon status panel
 */
TaskMonStatusPanel::TaskMonStatusPanel() :
    TaskMonPanel( "", 1, TaskMonPanel::GetTermCols(), TaskMonPanel::GetTermRows()-1, 0, STATUS_WINDOW)
{
    SetBorder( false);
}

TaskMonStatusPanel::~TaskMonStatusPanel()
{
}

/**
 * Method used to refresh/update the data in the panel
 * 
 * @param displayMode
 *                 What type of task data we are displaying: CPU or memory
 * @param keepDead Current status of the 'keep dead tasks on the screen' flag
 */
void TaskMonStatusPanel::Update( const TaskMonDisplayMode_t &displayMode, bool keepDead)
{
    std::string text;
    int         row, col;
    
    AttributeOn( COLOR_PAIR( GetWindowColor()));

    // How to get help
    Print( m_top, LEFT_JUSTIFY, "<");
    AttributeOn( A_BOLD);
    Print( "F1");
    AttributeOff( A_BOLD);
    Print( "> Help");

    // Are we showing dead processes
    Print( m_top, CENTER_JUSTIFY, "Show dead tasks:    ");
    GetPosition( row, col); col -= 3;
    AttributeOn( A_BOLD);
    Print( row, col, keepDead == true ? "Yes" : "No ");
    AttributeOff( A_BOLD);
    
    // What mode are we in
    Print(m_top, RIGHT_JUSTIFY, "Mode:       ");
    AttributeOn( A_BOLD);
    switch( displayMode)
    {
    case TASKMON_DISPLAY_CPU:       Print(m_top, RIGHT_JUSTIFY, "CPU   ");  break;
    case TASKMON_DISPLAY_MEM:       Print(m_top, RIGHT_JUSTIFY, "Memory");  break;
    case TASKMON_DISPLAY_THREADS:   Print(m_top, RIGHT_JUSTIFY, "Thread");  break;
    case TASKMON_DISPLAY_FDS:       Print(m_top, RIGHT_JUSTIFY, "FDs");     break;
    default:                    break;
    }
    AttributeOff( A_BOLD);

    // Display the panel title
    TaskMonPanel::Update();
}
