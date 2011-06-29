//******************************************************************************
// Description:
//  Class used to manage the collection of panels and windows to display
//	data from TaskMon
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonTerm.h $
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

#ifndef _TASKMONTERM_H_INCLUDED_
#define _TASKMONTERM_H_INCLUDED_

#include "TaskMonDefs.h"
#include "TaskMonHeaderPanel.h"
#include "TaskMonStatusPanel.h"
#include "TaskMonTaskPanel.h"
#include "TaskMonHelpWindow.h"
#include "TaskMonTimer.h"
#include "TaskMonTask.h"

/**
 * Undefine these here because they get defined in the curses.h file
 **/
#undef clear
#undef erase


#include "XmlNode.h"

#include <stdint.h>
#include <string>
#include <map>
#include <vector>
#include <utility>

typedef std::pair<int, int>					TermColor_t;

typedef std::vector< TermColor_t>			TermColorList_t;

/**
 * Object used to map atask's device path to a file descriptor
 */
typedef std::map< std::string, int>		TaskFdMap_t;
typedef TaskFdMap_t::iterator			TaskFdMapItr_t;
typedef TaskFdMap_t::const_iterator		TaskFdMapCItr_t;
typedef TaskFdMap_t::value_type			TaskFdMapVal_t;

class TaskMonTerm
{
public:
	/**
	 * Constructor from command line args
	 * 
	 * @param argc   Number of command line arguments
	 * @param argv   List of command line arguments
	 */
	TaskMonTerm( int argc, char *argv[]);

	~TaskMonTerm();

	int Run();

	/**
	 * Returns what display mode we are operating in: No Data, CPU usage or 
	 * memory usage
	 * 
	 * @return Display mode we are operating in: No Data, CPU usage or 
	 *         memory usage
	 */
	TaskMonDisplayMode_t GetDisplayMode() const;

	/**
	 * Sets what display mode we are operating in: No Data, CPU usage or
	 * memory usage
	 * 
	 * @param displayMode
	 *               What display mode we are operating in: No Data, CPU usage or
	 *               memory usage
	 */
	void SetDisplayMode( TaskMonDisplayMode_t displayMode);
	
protected:
	/**
	 * Initializes our attribuutes
	 */
	void Initialize();

	/**
	 * Initializes our task state to color mappings
	 * 
	 * @param setupNode Pointer to the config file <Setup> node
	 */
	void InitializeColors( const XmlNode *setupNode);

	/**
	 * Parses command line arguments
	 * 
	 * @param argc   Number of command line arguments to parse
	 * @param argv   Array of command line arguments
	 */
	void ParseCommandLine( int argc, char *argv[]);

	/**
	 * Initializes the curses library (terminal control)
	 */
	void CursesInit();

	/**
	 * Cleanup the curses library when done
	 */
	void CursesClose();

	/**
	 * Updates the screen with the currently running process lists
	 */
	void UpdateScreen();

	/**
	 * Updates the screen with the currently running process lists
	 */
	void UpdateTaskList();
	
	/**
	 * Updates the task statistics for the given task
	 * 
	 * @param newTask  Task object to be updated
	 * @param taskName Name of the task
	 * @param taskDir  Directory under to which the task belongs
	 *                 (i.e. /net/1/dev/TaskMon/Tasks/Core/)
	 * 
	 * @return EOK if successful
	 */
	int UpdateTaskStats( TaskMonTask_t &newTask, const std::string &taskName, const std::string &taskDir);
	
	/**
	 * Checks to see if the user has pressed a key and processes the key press
	 * 
	 * @param keyCode Key code for the key that was pressed
	 * 
	 * @return true if a key was pressed, false if no key was pressed
	 */
	bool CheckKeyPress( int &keyCode);

	/**
	 * Checks for and processes any key presses
	 * 
	 * @return Which key was pressed or -1 if no keys were pressed
	 */
	int HandleKeyboard();
	
	/**
	 * Removes dead processes from the screen
	 */
	void RemoveDeadProcesses();
	
private:
	/**
	 * Connection to deliver timer pulses to
	 */
	int						m_timerCoid;
	
	/**
	 * Flag used to indicate whether or not we want to keep 'dead'
	 * tasks on the screen or not
	 */
	bool					m_keepDead;

	/**
	 * Flag used to indicate whether or not we want to CPU usage for
	 * monitored tasks
	 */
	bool					m_showCpu;

	/**
	 * Name of the node being monitored
	 */
	std::string				m_nodeName;

	/**
	 * Interval (ms) at which we will update the screen
	 */
	int32_t					m_loopTime;

	/**
	 * Priority to run at
	 */
	int						m_priority;

	/**
	 * Name of the config file to read config data from
	 */
	std::string				m_configFile;

	/**
	 * List of tasks
	 */
	TaskList				m_taskList[ TASKMON_CLASS_END];

	/**
	 * Mapping of task device path to a file descriptor
	 */
	TaskFdMap_t				m_taskFdMap;

	/**
	 * Header panel
	 */
	TaskMonHeaderPanel		*m_headerPanel;

	/**
	 * Status panel
	 */
	TaskMonStatusPanel		*m_statusPanel;

	/**
	 * Task list panels
	 */
	TaskMonTaskPanel		*m_taskPanels[ TASKMON_CLASS_END];

	/**
	 * Task used to report overall system CPU load
	 */
	TaskMonTask_t			m_procTask;

	/**
	 * List of fg and bg color pairs
	 */
	TermColorList_t			m_termColors;

	/**
	 * Flag to indicate if we should use the real curses box drawing
	 */
	bool					m_useRealBox;

	/**
	 * What data should we display for each process
	 */
	TaskMonDisplayMode_t	m_displayMode;

	/**
	 * Index of the currently selected panel
	 */
	int						m_activePanel;

	/**
	 * "Help" window used to display key definitions
	 */
	TaskMonHelpWindow		*m_helpWindow;
};

#endif // _TASKMONTERM_H_INCLUDED_
