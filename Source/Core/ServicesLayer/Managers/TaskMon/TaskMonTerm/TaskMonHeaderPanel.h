//******************************************************************************
// Description:
//  Class used to display the header panel at the top of the main window
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonHeaderPanel.h $
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

#ifndef _TASKMONHEADERPANEL_H_INCLUDED_
#define _TASKMONHEADERPANEL_H_INCLUDED_

#include "TaskMonPanel.h"
#include "TaskMonTask.h"

#include <string>
 
/**
 * TaskMonPanel object used to maintain the header panel. The panel
 * occupies the first two rows (full screen width) and has the panel
 * title set to the name of the node being monitored.
 */
class TaskMonHeaderPanel : public TaskMonPanel
{
public:
	/**
	 * Construct a new TaskMon header panel
	 * 
	 * @param nodeName Name of the node to be monitored
	 * @param keepDead Flag indicating if dead processes will remain on the screen
	 * @param loopTime Number of milliseconds between consecutive screen updates
	 */
	TaskMonHeaderPanel( const std::string &nodeName, bool keepDead, uint32_t loopTime);

	virtual ~TaskMonHeaderPanel();

	/**
	 * Method used to refresh/update the data in the panel
	 * 
	 * @param procTask Kernel task used to report overall system CPU load
	 * @param keepDead Current status of the 'keep dead processes on the screen' flag
	 */
	virtual void Update( const TaskMonTask_t &procTask, bool keepDead);

	/**
	 * Sets the value of the m_keepDead flag
	 * 
	 * @param keepDead The value of the m_keepDead flag
	 */
	void SetKeepDead( bool keepDead);

	/**
	 * Sets the value of the m_loopTime member
	 * 
	 * @param loopTime The value of the m_loopTime member
	 */
	void SetLoopTime( uint32_t loopTime);
	
protected:
	/**
	 * Flag indicating if dead processes will remain on the screen
	 */
	bool		m_keepDead;

	/**
	 * Number of milliseconds between consecutive screen updates
	 */
	int			m_loopTime;

	/**
	 * Time stamp from last update
	 */
	uint64_t	m_lastSampleTime;
};

#endif // _TASKMONHEADERPANEL_H_INCLUDED_

