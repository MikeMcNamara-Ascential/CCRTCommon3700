//******************************************************************************
// Description:
//  Timer object used to wake up at periodic intervals
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
//     $Log: /Tata/Source/Tata/ApplicationLayer/TestComponents/SoundLevel/WaitableTimer.h $
// 
// 1     2/28/06 2:03p Bmeinke
// Seperated from TataMachineTC
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _WAITABLETIMER_H_INCLUDED_
#define _WAITABLETIMER_H_INCLUDED_

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
class WaitableTimer
{
public:
	/**
	 * Create a timer object that will "wake up" at the specified frequency
	 */
	WaitableTimer();

	/**
	 * Destructor (duh)
	 */
	~WaitableTimer();

	/**
	 * Waits for the timer to expire.
	 * 
	 * @param waitIfNotArmed
	 *               Flag indicating if we want to wait for timer events even if the
	 *               timer is not currently armed (i.e. if you want an external thread
	 *               to start and stop the timer)
	 * 
	 * @return EOK if successful or an errno style error code if an error occurs
	 */
	int Wait(bool waitIfNotArmed=false);

	/**
	 * Starts the timer.
	 * 
	 * @param period Timer's firing period (in milliseconds)
	 * 
	 * @return 0 if successful, -1 if an error occurred (use errno for more
	 *         detailed error information)
	 */
	int Start( const uint32_t period);

	/**
	 * Stop the timer
	 */
	int Stop();

	/**
	 * Checks to see if the timer is currently armed (Active)
	 *
	 * @return true if timer is active, false if not active or unable to
	 *         determine if timer is active
	 */
	bool IsTimerArmed();

private:

	/**
	 * Delete the timer resources
	 */
	void Destroy();
	
	/**
	 * Timer ID
	 */
	timer_t				m_timerId;

	/**
	 * Event used to deliver inter-mssg gap timing events
	 */
	struct sigevent		m_timerEvent;

	/**
	 * Channel we will receive timer pulses on
	 */
	int					m_chId;

	/**
	 * Connection to which the timer will deliver its pulses
	 */
	int					m_coId;

	/**
	 * Pulse code to use for our timer pulse
	 */
	static const int	m_timerCode;
	
	/**
	 * Pulse value to use for our timer pulse
	 */
	static const int	m_timerVal;
};

#endif // _WAITABLETIMER_H_INCLUDED_

