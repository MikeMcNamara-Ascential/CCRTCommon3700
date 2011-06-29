//******************************************************************************
// Description:
//  Class used to display the online help window for TaskMonTerm
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonHelpWindow.cpp $
// 
// 3     10/26/06 10:59a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the number of thread and file descriptors associated with each
// task.
// 
// 2     4/13/06 3:59a Cward
// Update task Mon term for scolling 
//
// 2     4/05/06 11:14a Bmeinke
// Changedthe length of the 'dashes' line in Update() per the change to
// m_cols in the TaskMonPanel base class
//
// 1     7/20/05 8:24p Cward
// Added new TaskMon
//
// 2     5/19/05 1:57p Bmeinke
// Added new private data members m_windowRows, m_windowCols, m_windowTopY
// and m_windowTopX to ensure that the help window displays in the center
// of the screen
//
// 1     5/17/05 6:57p Bmeinke
// Core R05051101 release with TaskMon updates
//
// 2     5/16/05 12:11p Bmeinke
// Use the newly defined CPU color levels
//
// 1     4/25/05 4:11p Bmeinke
//
// 1     4/25/05 3:39p Bmeinke
// NewTaskMon task monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "TaskMonHelpWindow.h"

#include <string>
#include <string.h>

/**
 * Number of rows needed to for the help window
 */
const int   TaskMonHelpWindow::m_windowRows = 17;
/**
 * Number of columns needed to for the help window
 */
const int   TaskMonHelpWindow::m_windowCols = 46;
/**
 * Upper left hand window corner's start row for window to be centered
 */
const int   TaskMonHelpWindow::m_windowTopY = (TaskMonPanel::GetTermRows() - TaskMonHelpWindow::m_windowRows) / 2;
/**
 * Upper left hand window corner's start column for window to be centered
 */
const int   TaskMonHelpWindow::m_windowTopX = (TaskMonPanel::GetTermCols() - TaskMonHelpWindow::m_windowCols) / 2;


TaskMonHelpWindow::TaskMonHelpWindow() : TaskMonPanel( "Help", m_windowRows, m_windowCols, m_windowTopY, m_windowTopX, STATUS_WINDOW)
{
    SetActive( true);
    scrollok( m_window, false);
}

TaskMonHelpWindow::~TaskMonHelpWindow()
{
}

/**
 * Method used to refresh/update the data in the panel
 */
void TaskMonHelpWindow::Update()
{
    const std::string dashes( m_cols, '=');

    //                                                     1111111111222222222233333333334444
    //                                           01234567890123456789012345678901234567890123
    AttributeOn( COLOR_PAIR( GetWindowColor()) | A_BOLD);
    Print(1, LEFT_JUSTIFY,  dashes.c_str());
    Print(1, CENTER_JUSTIFY,    "Keys");
    Print(2, LEFT_JUSTIFY,  " <c>: Show CPU usage");
    Print(3, LEFT_JUSTIFY,  " <k>: Toggle keeping dead tasks on screen");
    Print(4, LEFT_JUSTIFY,  " <m>: Show memory usage");
    Print(5, LEFT_JUSTIFY,  " <t>: Show thread count");
    Print(6, LEFT_JUSTIFY,  " <f>: Show open file count");

    Print(7, LEFT_JUSTIFY,  " <r>: Refresh the screen");
    Print(8, LEFT_JUSTIFY,  "<F2>: Toggle gathering of memory data");

    Print(9, LEFT_JUSTIFY,  dashes.c_str());
    Print(9, CENTER_JUSTIFY,    "Color Legend");

    AttributeOn( COLOR_PAIR( GetWindowColor()) | A_BOLD);   Print(10, LEFT_JUSTIFY,         "Tasks");
    AttributeOff( A_BOLD);
    AttributeOn( COLOR_PAIR(TASKSTATE_REG+1));      Print(11, LEFT_JUSTIFY,         "Just started");
    AttributeOn( COLOR_PAIR(TASKSTATE_RUN+1));      Print(12, LEFT_JUSTIFY,     "Running normally");
    AttributeOn( COLOR_PAIR(TASKSTATE_STOP+1));     Print(13, LEFT_JUSTIFY,     "Ready to terminate");
    AttributeOn( COLOR_PAIR(TASKSTATE_UNREG+1));    Print(14, LEFT_JUSTIFY,     "Exited normally");
    AttributeOn( COLOR_PAIR(TASKSTATE_END+1));      Print(15, LEFT_JUSTIFY,     "Exited abnormally");

    AttributeOn( COLOR_PAIR( GetWindowColor()) | A_BOLD);   Print(10, RIGHT_JUSTIFY,    "CPU");
    AttributeOn( COLOR_PAIR(DEFAULT_COLOR) | A_BOLD);   Print(11, RIGHT_JUSTIFY,    "< %d%%", CPU_LOW);
    AttributeOn( COLOR_PAIR(CPU_YELLOW) | A_BOLD);      Print(12, RIGHT_JUSTIFY,    "> %d%%", CPU_LOW);
    AttributeOn( COLOR_PAIR(CPU_MAGENTA) | A_BOLD);     Print(13, RIGHT_JUSTIFY,    "> %d%%", CPU_WARN);
    AttributeOn( COLOR_PAIR(CPU_RED) | A_BOLD);         Print(14, RIGHT_JUSTIFY,    "> %d%%", CPU_CRITICAL);

    AttributeOn( COLOR_PAIR( GetWindowColor()) | A_BOLD);
    Print(16, LEFT_JUSTIFY,     dashes.c_str());
    Print(17, CENTER_JUSTIFY, "Press ESC to close this window");

    TaskMonPanel::Update();
}

/**
 * Scrolls the windows contents by the specified number of lines
 *
 * @param lines  Number of lines to scroll the windows by
 */
void TaskMonHelpWindow::Scroll(int lines)
{
    lines = lines;

    return;
}

