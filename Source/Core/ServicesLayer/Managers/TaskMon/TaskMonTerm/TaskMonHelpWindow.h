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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonHelpWindow.h $
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
// 1     4/25/05 4:11p Bmeinke
// 
// 1     4/25/05 3:39p Bmeinke
// NewTaskMon task monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _TASKMONHELPWINDOW_H_INCLUDED_
#define _TASKMONHELPWINDOW_H_INCLUDED_

#include "TaskMonPanel.h"

class TaskMonHelpWindow : public TaskMonPanel
{
public:
	TaskMonHelpWindow();

	~TaskMonHelpWindow();
	
	/**
	 * Method used to refresh/update the data in the panel
	 */
	virtual void Update();

	/**
	 * Scrolls the windows contents by the specified number of lines
	 *
	 * @param lines  Number of lines to scroll the windows by
	 */
	virtual void Scroll(int lines);
private:

	/**
	 * Number of rows needed to for the help window
	 */
	static const int	m_windowRows;
	/**
	 * Number of columns needed to for the help window
	 */
	static const int	m_windowCols;
	/**
	 * Upper left hand window corner's start row for window to be centered
	 */
	static const int	m_windowTopY;
	/**
	 * Upper left hand window corner's start column for window to be centered
	 */
	static const int	m_windowTopX;
};

#endif // _TASKMONHELPWINDOW_H_INCLUDED_
