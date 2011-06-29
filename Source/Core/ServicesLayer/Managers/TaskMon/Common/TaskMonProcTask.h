//******************************************************************************
// Description:
//  TaskMonTask object specialized for reading kernel CPU usage
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/Common/TaskMonProcTask.h $
// 
// 1     7/20/05 8:24p Cward
// Added new TaskMon
// 
// 1     5/17/05 6:57p Bmeinke
// Core R05051101 release with TaskMon updates
// 
// 1     4/25/05 4:30p Bmeinke
// 
// 1     4/25/05 4:11p Bmeinke
// 
// 1     4/25/05 3:39p Bmeinke
// NewTaskMon task monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _TASKMONPROCTASK_H_INCLUDED_
#define _TASKMONPROCTASK_H_INCLUDED_

#include "TaskMonTask.h"

#include <stdint.h>
#include <vector>

/**
 * TaskMon task specialized for reading kernel CPU usage
 */
class TaskMonProcTask : public TaskMonTask
{
public:
	
	TaskMonProcTask();

	~TaskMonProcTask();
	
	/**
	 * Calculate the CPU usage for this task
	 * 
	 * @return The tasks's CPU usage (in %)
	 */
	virtual double UpdateCpuUsage();

private:

	/**
	 * Number of CPUs in the current computer
	 */
	static const uint16_t m_cpuCount;
	
	/**
	 * Time at which the CPU times were last sampled
	 */
	std::vector<uint64_t>	m_threadSampleTime;

	/**
	 * Current CPU time consumption
	 */
	std::vector<uint64_t>	m_threadCpuTime;
};

#endif // _TASKMONPROCTASK_H_INCLUDED_

