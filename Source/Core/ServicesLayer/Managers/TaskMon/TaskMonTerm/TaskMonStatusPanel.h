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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonStatusPanel.h $
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

#ifndef _TASKMONSTATUSPANEL_H_INCLUDED_
#define _TASKMONSTATUSPANEL_H_INCLUDED_

#include "TaskMonPanel.h"

class TaskMonStatusPanel : public TaskMonPanel
{
public:
	/**
	 * Construct a new TaskMon status panel
	 */
	TaskMonStatusPanel();
	
	~TaskMonStatusPanel();
	
	/**
	 * Method used to refresh/update the data in the panel
	 * 
	 * @param displayMode
	 *                 What type of task data we are displaying: CPU or memory
	 * @param keepDead Current status of the 'keep dead tasks on the screen' flag
	 */
	void Update( const TaskMonDisplayMode_t &displayMode, bool keepDead);
};

#endif // _TASKMONSTATUSPANEL_H_INCLUDED_

