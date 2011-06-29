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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/Common/TaskMonTimer.cpp $
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

#include "TaskMonTimer.h"

TaskMonTimer::TaskMonTimer() : m_timerId( -1)
{
}

TaskMonTimer::~TaskMonTimer()
{
	Destroy();
}

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
int TaskMonTimer::Initialize( int coId, uint8_t pulseCode, uint32_t pulseVal, int prio)
{
	// Make sure not currently running
	Destroy();

	// Initialize the event to be delivered when the timer expires
	SIGEV_PULSE_INIT( &m_timerEvent, coId, prio, pulseCode, pulseVal);

	// Create the timer
	m_timerId = TimerCreate( CLOCK_REALTIME, &m_timerEvent);

	return( m_timerId);
}

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
int TaskMonTimer::Arm( const uint32_t delay, const uint32_t interval=0)
{
	int 			retVal = EBUSY;
	struct _itimer	timerTime;

	// Stop any current timer activity
	Stop();

	timerTime.nsec = mSEC_nSEC( delay);
	timerTime.interval_nsec = mSEC_nSEC( interval);

	// Start the timer
	retVal = TimerSettime( m_timerId, 0, &timerTime, NULL);

	return( retVal);
}
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
int TaskMonTimer::Arm( const uint64_t &delay, const uint64_t &interval/*=0*/)
{
	int 			retVal = EBUSY;
	struct _itimer	timerTime;

	// Stop any current timer activity
	Stop();

	timerTime.nsec = delay;
	timerTime.interval_nsec = interval;

	// Start the timer
	retVal = TimerSettime( m_timerId, 0, &timerTime, NULL);
	if(retVal == -1)
		printf( "\tERROR starting the timer: %s\n", strerror( errno));

	return( retVal);
}

/**
 * Stop the timer
 */
int TaskMonTimer::Stop()
{
	int retVal;
	struct _itimer	timerTime;
	memset( &timerTime, 0, sizeof( timerTime));

	// Stop the timer
	if( (retVal = TimerSettime( m_timerId, 0, &timerTime, NULL)) == -1)
	{
		printf( "\tERROR stopping timer: %s\n", strerror( errno));
	}

	return( retVal);
}

/**
 * Stop and destroy the timer
 */
void TaskMonTimer::Destroy()
{
	// Stop and destroy the timer
	if( m_timerId != -1)
	{
		Stop();

		// Destroy the timer
		TimerDestroy(m_timerId);

		m_timerId = -1;
	}
}

/**
 * Checks to see if the timer is currently armed (Active)
 *
 * @return true if timer is active, false if not active or unable to
 *         determine if timer is active
 */
bool TaskMonTimer::IsTimerArmed()
{
	bool	retVal = false;
	struct _timer_info timerInfo;

	if( m_timerId != -1)
	{
		if( TimerInfo( 0, m_timerId, 0, &timerInfo) != -1)
		{
			retVal = ((timerInfo.flags & _NTO_TI_ACTIVE) == _NTO_TI_ACTIVE);
		}
	}
	return( retVal);
}
