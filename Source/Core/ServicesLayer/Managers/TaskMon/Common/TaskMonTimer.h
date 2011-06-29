//******************************************************************************
// Description:
//  Timer object used to deliver pulses to a channel in order to refresh task
//	data and the TaskMonTerm screen at regular intervals
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/Common/TaskMonTimer.h $
// 
// 2     11/11/05 4:56p Cward
// Changes for 0511101 iteration
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

#ifndef _TASKMONTIMER_H_INCLUDED_
#define _TASKMONTIMER_H_INCLUDED_

#include "BepDefs.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>			// memset()
#include <errno.h>
#include <sys/neutrino.h>	// timer support

/**
 * Timer used to update the TaskMon screen
 *
 * @author Brian Meinke
 * @version 1
 * @since Dec 2, 2001
 */
class TaskMonTimer
{
public:
	TaskMonTimer();

	~TaskMonTimer();

	/**
	 * Loads the comm timer with the specified timeout value. Timer notification
	 * is accompished via a pulse with the specified code and priority and is
	 * delivered to the dispatch context pointed to by dpp. A message_atatch()
	 * call must be made after this function is invoked to properly attach the
	 * pulse to the channel managed by the specified dispatch pointer. To start
	 * the timer, call Arm().
	 *
	 * @param coId      Connection to deliver the timer expiration pulse on
	 * @param pulseCode The pulse code to be delivered when the timer expires
	 * @param pulseVal  Value of the pulse to be sent when the timer expires
	 * @param prio      Priority to send the timer expiration pulse to
	 * @return -1 if an error occurred any other value indicates success
	 *         (use errno for more detailed error information)
	 * @see Arm
	 */
	int Initialize( int coId, uint8_t pulseCode=SYSTEM_PULSE, uint32_t pulseVal=TIMER_PULSE, int prio=SIGEV_PULSE_PRIO_INHERIT);

#if 0
	/**
	 * Starts the timer. A previously successful call to Load() must be
	 * done prior to calling this method
	 *
	 * @param delay    Initial timer timeout values (in milliseconds)
	 * @param interval Timer repeat value (in milliseconds). default=0
	 *
	 * @return 0 if successful, -1 if an error occurred (use errno for more
	 *         detailed error information)
	 */
	int Arm( const uint32_t delay, const uint32_t interval=0);
#endif

	/**
	 * Starts the timer. A previously successful call to Load() must be
	 * done prior to calling this method
	 *
	 * @param delay    Initial timer timeout values (in nanoseconds)
	 * @param interval Timer repeat value (in nanoseconds). default=0
	 * @return 0 if successful, -1 if an error occurred (use errno for more
	 *         detailed error information)
	 */
	int Arm( const uint64_t &delay, const uint64_t &interval=0);

	/**
	 * Stop the timer
	 */
	int Stop();

	/**
	 * Stop and destroy the timer
	 */
	void Destroy();

	/**
	 * Checks to see if the timer is currently armed (Active)
	 *
	 * @return true if timer is active, false if not active or unable to
	 *         determine if timer is active
	 */
	bool IsTimerArmed();

private:

	/**
	 * Timer ID
	 */
	timer_t				m_timerId;

	/**
	 * Event used to deliver inter-mssg gap timing events
	 */
	struct sigevent		m_timerEvent;
};

#endif // _TASKMONTIMER_H_INCLUDED_

