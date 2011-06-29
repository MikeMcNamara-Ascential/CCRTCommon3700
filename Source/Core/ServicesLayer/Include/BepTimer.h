//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/BepTimer.h 4     10/30/07 3:32p Rwiersem $
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/BepTimer.h $
// 
// 4     10/30/07 3:32p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:06p Khykin
// Initial checkin.
//
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:34  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:03a Khykin
// Initial Checkin
//
// 1     1/18/03 8:01a Khykin
// Initial Checkin From DCTC
//
// 13    5/24/02 2:51p Kh
// Adding the ability to change the notification pulse code.
//
// 12    4/08/02 1:14p Skypig13
// Removed compiler wrning on line 273 by adding typecast
//
// 11    4/04/02 3:05a Bmeinke
// Changed timer arguments back to nano-seconds
//
// 10    4/03/02 1:22p Kh
// Updating the time settings to be in micro seconds.
//
// 8     2/05/02 12:32a Skypig13
// Removed compiler warnings
//
// 7     2/01/02 9:34p Brian.meinke
// Made the timer pulse's code and value dynamic (member variables)
// Added methods to make sub-classing possible (EventnNit, etc)
//
// 6     1/31/02 8:38a Kh
// Updating the Initialize method to take a debug parameter.
//
// 5     1/21/02 9:31p Kh
// Adding a Stop method.
//
// 4     11/20/01 4:39p Skypig13
// Addded include files
//
// 3     11/20/01 4:41p Kh
// Updating and testing.
//
// 2     10/12/01 11:24a Skypig13
// Latest files for DataServer
//
// 1     10/04/01 8:16p Skypig13
// Timer to encapsulate POSIX timer
//
//*************************************************************************

#ifndef BepTimer_h
#define BepTimer_h

#include <string>
#include <time.h>
//#include <errno.h>
//#include <sys/netmgr.h>
//#include <sys/neutrino.h>

//#include "Bepos.h"
#include "BepDefs.h"
#include "Debuggable.h"
//#include "BepException.h"

/**
 * Timer cass that will send a pulse when it has expired.
 * Class encapsulates the functionality of a POSIX (realtime extensions) timer
 *
 * @author Tim Bochenek
 * @since Version 1.1
 */
class BepTimer : public Debuggable
{
public:
    /**
     * Class constructor (default)
     * @since Version 1.0
     */
    BepTimer(const bool debug = false);
    
    /**
     * Class constructor
     *
     * @param name     The name of the channel to create a timer for.
     * @param interval The interval to create the timer for.
     * @param delay    The delay to use prior to the first interrupt occurring.
     * @param debug    The status of the debug information.
     * @param priority The priority to use for the timer.
     * @since Version 1.0
     */
    BepTimer(const std::string name, const UINT64 interval, const UINT64 delay, const bool debug = false, INT32 priority = -1);
    
    /**
     * Class destructor
     * @since Version 1.0
     */
    virtual ~BepTimer(void);
    
    /**
     * Method used to set up timer
     *
     * @param name     Name of channel to send the pulse to
     * @param interval At what interval should the timer expire
     * @param delay    Initial delay value of timer
     * @param debug    The debug flag value.
     * @param priority The priority to use for the timer.
     */
    virtual void Initialize(const std::string name, const UINT64 interval, const UINT64 delay, const bool debug = false, INT32 priority = -1);

    /**
     * Method used to set up timer
     *
     * @param connectionID The connection id that will receive the pulse.
     * @param interval     At what interval should the timer expire
     * @param delay        Initial delay value of timer
     * @param debug        The debug flag value.
     * @param priority     The priority to use for the timer.
     */
    virtual void Initialize(int connectionID, const UINT64 interval, const UINT64 delay, const bool debug = false, INT32 priority = -1);

    /**
     * Method used to start the timer.
     * @since Version 1.0
     */
    void Start(void);

    /**
     * Method used to stop the timer.
     * @since Version 1.1
     */
    void Stop(void);

    /**
     * Gets the id of the timer
     *
     * @return Id of timer
     * @since Version 1.0
     */
    const UINT32 GetTimerId(void);

    /**
     * Gets the amount of time left before timer expires (nanoseconds)
     *
     * @return Time left before timer expires
     * @since Version 1.0
     */
    const UINT64 GetTimeLeft(void);

    /**
     * Returns the time for the timer expiration (nanoseconds)
     *
     * @return Interval between timer expirations (nanoseconds)
     * @since Version 1.0
     */
    const UINT64 GetInterval(void);

    /**
     * Gets the amount of time that the timer will delay initally (nanoseconds)
     *
     * @return Initial delay (nanoseconds)
     * @since Version 1.0
     */
    const UINT64 GetInitialDelay(void);

    /**
     * Returns the code of the pulse to be sent when the timer expires
     *
     * @return The code of the pulse to be sent when the timer expires
     * @since Version 1.0
     */
    int GetPulseCode();

    /**
     * Returns the value of the pulse to be sent when the timer expires
     *
     * @return The value of the pulse to be sent when the timer expires
     * @since Version 1.0
     */
    int GetPulseValue();

    /**
     * Sets the interval time (nano seconds) for timer expiration value
     *
     * @param interval Timer expiration value
     * @since Version 1.0
     */
    void SetInterval(const UINT64 &interval);

    /**
     * Sets the initial timer wait value (nanoseconds)
     *
     * @param delay  Initial delay value (nanoseconds)
     * @since Version 1.0
     */
    void SetInitialDelay(const UINT64 &delay);

    /**
     * Sets the code of the pulse to be sent when the timer expires
     *
     * @param pulseCode     code to set
     * @since Version 1.0
     */
    void SetPulseCode( int pulseCode);

    /**
     * Sets the value of the pulse to be sent when the timer expires
     *
     * @param pulseValue     Value to set
     * @since Version 1.0
     */
    void SetPulseValue( int pulseValue);

protected:
    /**
     * Creates a new OS timer.
     */
    void CreateTimer();

    /**
     * Deletes the timer associated with this object
     * @since Version 1.0
     */
    void DeleteTimer(void);

    /**
     * Sets the id of the timer
     *
     * @param id     Value to set
     * @since Version 1.0
     */
    void SetTimerId(const INT32 id);

    /**
     * Initializes the event structure to be signalled when the timer expires
     *
     * @param coId   Connection ID to deliver the timer timeout event to
     * @param prio   The priority to send the evnt at
     */
    void EventInit( int coId, int prio);


private:
    /**
     * Id of the timer
     * @since Version 1.0
     */
    timer_t m_id;
    /**
     * Timer control structure
     * @since Version 1.0
     */
    itimerspec m_timerSpec;
    /**
     * Event structure controlling how timer behaves
     * @since Version 1.0
     */
    sigevent m_event;

    /**
     * Pulse code to be sent when the timer expires
     */
    int m_pulseCode;

    /**
     * Pulse value to be sent when the timer expires
     */
    int m_pulseValue;
};

#endif // BepTimer_h
