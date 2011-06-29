//******************************************************************************
// Description:
//  TaskMonPanel object specialized to displays a list of tasks within a common
//	task class.
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonTaskPanel.h $
// 
// 2     4/13/06 3:59a Cward
// Update task Mon term for scolling 
//
// 2     4/05/06 11:19a Bmeinke
// Change the title color to white on red to indicate there a re dead
// tasks displayed off screen
// Override the GetBoxChars() method to draw a pseudo scroll bar if we
// have more tasks than rows
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

#ifndef _TASKMONTASKPANEL_H_INCLUDED_
#define _TASKMONTASKPANEL_H_INCLUDED_

#include "TaskMonPanel.h"

class TaskMonTaskPanel : public TaskMonPanel
{
public:
	/**
	 * Construct a new task list panel with 'rows' rows and 'columns' columnss with a top-left
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
	TaskMonTaskPanel( const std::string &title="TaskPanel", int rows=TaskMonPanel::GetTermRows(),
				  int columns=TaskMonPanel::GetTermCols(), int begin_y=0, int begin_x=0);

	virtual ~TaskMonTaskPanel();

	/**
	 * Sets the title for the panel
	 *
	 * @param title
	 */
	virtual void SetTitle( const std::string &title);

	/**
	 * Method used to refresh/update the data in the panel
	 *
	 * @param taskList List of tasks in the panel
	 * @param displayMode
	 *                 What type of data should we display for each process
	 * @param keepDead Flag to indicate if we want to display dead tasks
	 */
	void Update(const TaskList &taskList, TaskMonDisplayMode_t displayMode, bool keepDead=true);

	/**
	 * Scrolls the windows contents by the specified number of lines
	 *
	 * @param lines  Number of lines to scroll the windows by
	 */
	virtual void Scroll( int lines=1);

protected:
	/**
	 * Sets the default color index for this window's title
	 */
	virtual int GetTitleColor() const;

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
	virtual void GetBoxChars( chtype &uLine, chtype &bLine,
							  chtype &lLine, chtype &rLine,
							  chtype &ulCorner, chtype &urCorner,
							  chtype &blCorner, chtype &brCorner) const;

	/**
	 * Number of tasks last displayed
	 */
	uint32_t	m_lastTaskCount;

	/**
	 * Base title for this task panel. We will display a task count
	 * along side the 'real' title, so we need to hold onto the title
	 * passed into the constructor
	 */
	std::string	m_panelTitle;

	/**
	 * First column that we can print text into
	 */
	int			m_startCol;

	/**
	 * First row that we can print text into
	 */
	int			m_startRow;

	/**
	 * Index of the first task listed in the window. Used to
	 * prevent task names being scrolled off the screen
	 */
	int			m_topTask;

	/**
	 * Flag indicating if there are any dead tasks in the list
	 */
	bool		m_haveDeadTasks;
};

#endif // _TASKMONTASKPANEL_H_INCLUDED_

