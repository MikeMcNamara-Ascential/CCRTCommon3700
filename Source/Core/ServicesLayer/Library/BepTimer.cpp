//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/BepTimer.cpp 3     4/13/05 7:01p Cward $
//
// FILE DESCRIPTION:
//   Data server that will be the interface between the User Interface and Real-Time System
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Library/BepTimer.cpp $
// 
// 3     4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 2     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//*************************************************************************

#include "BepTimer.h"

//#include <string>
//#include <time.h>
#include <errno.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

#include "Bepos.h"
//#include "BepDefs.h"
//#include "Debuggable.h"
#include "BepException.h"


BepTimer::BepTimer(const bool debug /*= false*/) : Debuggable(debug), m_id(-1),
    m_pulseCode( SYSTEM_PULSE), m_pulseValue( TIMER_PULSE)
{
}

BepTimer::BepTimer(const std::string name, const UINT64 interval, const UINT64 delay, const bool debug /*= false*/, INT32 priority /*= -1*/)
    : Debuggable(debug), m_id(-1), m_pulseCode( SYSTEM_PULSE), m_pulseValue( TIMER_PULSE)
{
    if(IsDebugOn())	printf("BepTimer(%s, %lld, %lld, %d)\n", name.c_str(), interval, delay, debug);
    Initialize(name, interval, delay, debug, priority);
}

BepTimer::~BepTimer(void)
{
    DeleteTimer();
}

void BepTimer::Initialize(const std::string name, const UINT64 interval, const UINT64 delay, const bool debug /*= false*/, INT32 priority /*= -1*/)
{	// Initialize signal event structure
    if(IsDebugOn())	printf("BepTimer::Initialize(%s, %lld, %lld)\n", name.c_str(), interval, delay);
    SetDebug(debug);
    int coid;
    INT32 usePriority = (priority == -1) ? getprio(0) : priority;
    // get the connection id, if cant throw exception
    if((coid = BposGetTaskId(name.c_str())) == -1)
        throw BepException("Could Not Get Task ID: %s", strerror(errno));
    if(IsDebugOn())	printf("Using coid: %d\n", coid);
    EventInit( coid, usePriority);	// Initialize the event to be sent when the timer expires
    SetInitialDelay(delay);			// set the initial delay before event (must not be 0 or wont work)
    SetInterval(interval);			// set the interval between events
    DeleteTimer();					// if there is a timer delete it
    CreateTimer();					// Create a new timer
    if(IsDebugOn())	printf("BepTimer::Initialize() done\n");
}

void BepTimer::Initialize(int connectionID, const UINT64 interval, const UINT64 delay, const bool debug /*= false*/, INT32 priority /*= -1*/)
{	// Initialize signal event structure
    if(IsDebugOn())	printf("BepTimer::Initialize(%d, %lld, %lld)\n", connectionID, interval, delay);
    SetDebug(debug);
    INT32 usePriority = (priority == -1) ? getprio(0) : priority;
    EventInit( connectionID, usePriority);	// Initialize the event to be sent when the timer expires
    SetInitialDelay(delay);					// set the initial delay before event (must not be 0 or wont work)
    SetInterval(interval);					// set the interval between events
    DeleteTimer();							// if there is a timer delete it
    CreateTimer();							// Create a new timer
    if(IsDebugOn())	printf("BepTimer::Initialize() done\n");
}

void BepTimer::Start(void)
{
    if(timer_settime(GetTimerId(), 0, &m_timerSpec, NULL) == -1)
        throw BepException("BepTimer::Start Error: %s", strerror(errno));
    if(IsDebugOn())	printf("BepTimer, Started\n");
}

void BepTimer::Stop(void)
{
    itimerspec timerSpec;
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = 0;
    timerSpec.it_value.tv_sec = 0;
    timerSpec.it_value.tv_nsec = 0;
    if(timer_settime(GetTimerId(), 0, &timerSpec, NULL) == -1)
        throw BepException("BepTimer::Stop Error: %s", strerror(errno));
}

const UINT32 BepTimer::GetTimerId(void)
{
    return(m_id);
}

const UINT64 BepTimer::GetTimeLeft(void)
{
    itimerspec tempSpec;
    timer_gettime(GetTimerId(), &tempSpec);
    return(tempSpec.it_value.tv_nsec + (tempSpec.it_value.tv_sec * NANOS_PER_SECOND));
}

const UINT64 BepTimer::GetInterval(void)
{
    itimerspec tempSpec;
    timer_gettime(GetTimerId(), &tempSpec);
    return(tempSpec.it_interval.tv_nsec + (tempSpec.it_interval.tv_sec * NANOS_PER_SECOND));
}

const UINT64 BepTimer::GetInitialDelay(void)
{
    return(m_timerSpec.it_value.tv_nsec + (m_timerSpec.it_value.tv_sec * NANOS_PER_SECOND));
}

int BepTimer::GetPulseCode()
{
    return( m_pulseCode);
}

int BepTimer::GetPulseValue()
{
    return( m_pulseValue);
}

void BepTimer::SetInterval(const UINT64 &interval)
{
    if(IsDebugOn())	printf("Setting Interval: %lld nano seconds\n", interval);
    m_timerSpec.it_interval.tv_sec = interval / NANOS_PER_SECOND;
    m_timerSpec.it_interval.tv_nsec = interval % NANOS_PER_SECOND;
}

void BepTimer::SetInitialDelay(const UINT64 &delay)
{
    if(IsDebugOn())	printf("Setting Delay: %lld nano seconds\n", delay);
    m_timerSpec.it_value.tv_sec = delay / NANOS_PER_SECOND;
    m_timerSpec.it_value.tv_nsec = delay % NANOS_PER_SECOND;
}

void BepTimer::SetPulseCode( int pulseCode)
{
    m_pulseCode = pulseCode;
}

void BepTimer::SetPulseValue( int pulseValue)
{
    m_pulseValue = pulseValue;
}

void BepTimer::CreateTimer()
{
    // try to create the timer, if cant throw an exception
    if(timer_create(CLOCK_REALTIME, &m_event, &m_id) == (INT32)BEP_STATUS_ERROR)
        throw BepException("Error Creating Timer: %s", strerror(errno));
}

void BepTimer::DeleteTimer(void)
{
    timer_t timerId = GetTimerId();
    if(timerId != -1)
    {
        if(IsDebugOn())	printf("Deleting The Timer: %d\n", timerId);
        timer_delete(timerId);
    }
    SetTimerId(-1);
}

void BepTimer::SetTimerId(const INT32 id)
{
    m_id = id;
}

void BepTimer::EventInit( int coId, int prio)
{
    m_event.sigev_notify = SIGEV_PULSE;
    SIGEV_PULSE_INIT(&m_event, coId, prio, m_pulseCode, m_pulseValue);
}

