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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonHeaderPanel.cpp $
// 
// 1     7/20/05 8:24p Cward
// Added new TaskMon
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

#include "TaskMonHeaderPanel.h"

#include <string.h>
#include <sys/neutrino.h>	// ClockTime()

/**
 * Construct a new TaskMon header panel
 * 
 * @param nodeName Name of the node to be monitored
 * @param keepDead Flag indicating if dead processes will remain on the screen
 * @param loopTime Number of milliseconds between consecutive screen updates
 */
TaskMonHeaderPanel::TaskMonHeaderPanel( const std::string &nodeName, bool keepDead, uint32_t loopTime) :
	TaskMonPanel( nodeName, 1, TaskMonPanel::GetTermCols(), 0, 0, HEADER_WINDOW), 
	m_keepDead( keepDead), m_loopTime( loopTime)
{
	SetBorder( false);
}

TaskMonHeaderPanel::~TaskMonHeaderPanel()
{
}

/**
 * Method used to refresh/update the data in the panel
 * 
 * @param procTask Kernel task used to report overall system CPU load
 * @param keepDead Current status of the 'keep dead processes on the screen' flag
 */
void TaskMonHeaderPanel::Update( const TaskMonTask_t &procTask, bool keepDead)
{
	char		dateBuff[ 1024];
	time_t		time_of_day;
	uint64_t	sampleTime;
	uint32_t	loopTime;

	SetKeepDead( keepDead);
	
	// Calculate the time (ms) since the last update
	ClockTime( CLOCK_REALTIME, NULL, &sampleTime);
	if( 0 == m_lastSampleTime)
	{
		m_lastSampleTime = sampleTime;
		return;
	}
	loopTime = (sampleTime - m_lastSampleTime) / 1000000LL;
	m_lastSampleTime = sampleTime;

	// Get the Time/Date
	time_of_day = time( NULL );
	strftime(dateBuff, 80, "%H:%M:%S %a %b %d, %Y", localtime( &time_of_day));

	AttributeOn( COLOR_PAIR( GetWindowColor()) | A_BOLD);

	// Print the date/time
	Print( m_top, RIGHT_JUSTIFY, "%s", dateBuff);

	// Print the CPU usage
	Print( m_top, LEFT_JUSTIFY, "CPU Load: ");

	if( procTask.m_cpuUsage >= CPU_CRITICAL)		AttributeOn( COLOR_PAIR( CPU_RED) | A_BOLD);
	else if( procTask.m_cpuUsage >= CPU_WARN)	AttributeOn( COLOR_PAIR( CPU_MAGENTA) | A_BOLD);
	else if( procTask.m_cpuUsage >= CPU_LOW)		AttributeOn( COLOR_PAIR( CPU_YELLOW) | A_BOLD);
	else										AttributeOn( COLOR_PAIR( GetWindowColor()) | A_BOLD);
	Print( "%6.2f%%", procTask.m_cpuUsage);

	AttributeOn( COLOR_PAIR( GetWindowColor()) | A_BOLD);
	// Display the panel title
	TaskMonPanel::Update();
}

/**
 * Sets the value of the m_keepDead flag
 * 
 * @param keepDead The value of the m_keepDead flag
 */
void TaskMonHeaderPanel::SetKeepDead( bool keepDead)
{
	m_keepDead = keepDead;
}

/**
 * Sets the value of the m_loopTime member
 * 
 * @param loopTime The value of the m_loopTime member
 */
void TaskMonHeaderPanel::SetLoopTime( uint32_t loopTime)
{
	m_loopTime = loopTime;
}
