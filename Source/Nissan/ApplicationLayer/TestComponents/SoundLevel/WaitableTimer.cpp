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
//     $Log: /Tata/Source/Tata/ApplicationLayer/TestComponents/SoundLevel/WaitableTimer.cpp $
// 
// 1     2/28/06 2:03p Bmeinke
// Seperated from TataMachineTC
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "WaitableTimer.h"
#include "BepDefs.h"		// mSEC_nSEC


const int WaitableTimer::m_timerCode = 0x5A;
const int WaitableTimer::m_timerVal = 0xA55A;

WaitableTimer::WaitableTimer() : m_timerId( -1), 
	m_chId( -1), m_coId( -1)
{
}

WaitableTimer::~WaitableTimer()
{
	Destroy();
}

int WaitableTimer::Wait(bool waitIfNotArmed)
{
	struct _pulse	timerPulse;
	int 			status = EINVAL;

	// Make sure the timer is armed our we we don't care if it is armed or not
	if( (true == IsTimerArmed()) || (true == waitIfNotArmed))
	{
		// Wait for our pulse to come in
		while( EOK != status)
		{
			status = MsgReceivePulse( m_chId, &timerPulse, sizeof( timerPulse), NULL);
			// Did we get a pulse ?
			if(0 == status)
			{
				// Was it our timer pulse?
				if( (WaitableTimer::m_timerCode == timerPulse.code) &&
					(WaitableTimer::m_timerVal == timerPulse.value.sival_int))
				{
					// Yippee!!
					status = EOK;
				}
			}
			else
			{
				// Roh-roh George...
				status = errno;
				break;
			}
		}
	}

	return( status);
}

int WaitableTimer::Start( const uint32_t period)
{
	int 			status = -1;
	const uint64_t	interval = mSEC_nSEC(period);
	struct _itimer	timerTime;

	// Stop any current timer activity
	Stop();

	// Create a channel to receive the timer pulses on
	m_chId = ChannelCreate(0);
	if( -1 != m_chId )
	{
		// Create a connection for the timer to deliver pulses to
		m_coId = ConnectAttach( 0,  0, m_chId, _NTO_SIDE_CHANNEL, 0);
		if( -1 != m_coId)
		{
			// Initialize the event to be delivered when the timer expires
			SIGEV_PULSE_INIT( &m_timerEvent, m_coId, SIGEV_PULSE_PRIO_INHERIT, 
							  WaitableTimer::m_timerCode, WaitableTimer::m_timerVal);

			memset( &timerTime, 0, sizeof( timerTime));
			timerTime.nsec = interval;
			timerTime.interval_nsec = interval;

			// Create the timer
			m_timerId = TimerCreate( CLOCK_REALTIME, &m_timerEvent);
			if( -1 != m_timerId)
			{
				// Start the timer
				status = TimerSettime( m_timerId, 0, &timerTime, NULL);
			}
			else
			{
				status = errno;
			}
		}
	}
	
	return( status);
}

int WaitableTimer::Stop()
{
	int status;
	struct _itimer	timerTime;

	// Stop the timer
	memset( &timerTime, 0, sizeof( timerTime));
	if( (status = TimerSettime( m_timerId, 0, &timerTime, NULL)) == -1)
	{
		printf( "\tERROR stopping timer: %s\n", strerror( errno));
	}

	return( status);
}

bool WaitableTimer::IsTimerArmed()
{
	bool	status = false;
	struct _timer_info timerInfo;

	if( m_timerId != -1)
	{
		if( TimerInfo( 0, m_timerId, 0, &timerInfo) != -1)
		{
			status = ((timerInfo.flags & _NTO_TI_ACTIVE) == _NTO_TI_ACTIVE);
		}
	}
	return( status);
}

void WaitableTimer::Destroy()
{
	// Stop and destroy the timer
	if( m_timerId != -1)
	{
		Stop();

		// Destroy the timer
		TimerDestroy(m_timerId);

		m_timerId = -1;
	}

	if( -1 != m_coId)	ConnectDetach( m_coId);
	if( -1 != m_chId)	ChannelDestroy( m_chId);

	m_coId = m_chId = -1;
	
}

