//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Include/ABSValveControl.h 3     10/30/07 1:27p Rwiersem $
//
// FILE DESCRIPTION:
//  
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Include/ABSValveControl.h $
// 
// 3     10/30/07 1:27p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added Javadoc comments.
// 
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 2     10/27/03 4:20p Derickso
// Completed writing.
// Compiles, not tested.
//
// 1     10/24/03 3:16p Canyanwu
// Start of DCX working directory
//
// 1     9/30/03 10:58a Derickso
// ABS Valve control thread.
//
//*************************************************************************
#ifndef ABSValveControl_h
#define ABSValveControl_h
//-------------------------------------------------------------------------
#include "BackgroundMonitor.h"

/**
 * Type of valve firing control to use.
 * For example, if the valves should be pulsed or continuously fired.
 */
typedef enum _ValveFireType
{
    PulseBrake=0,
    ContinuousFire
} ValveFireType;

/**
 * ABS Valve control class.
 * This thread provides the capability for firing or
 * pulsing the brakes.
 * This is useful for creating speed differences between
 * different wheels on the vehicle.
 *
 * @author Dan Erickson
 * @since October 01, 2003
 */
class ABSValveControl : public BackgroundMonitor
{
public:
    /**
     * Class Constructor.
     *
     * @param filter   Protocol filter object to use to communicate with the
     *                 vehicle module.
     * @param valveControlFireMessageTag
     *                 Message to send to the module to fire the ABS valves.
     * @param valveControlStopFireMessageTag
     *                 Message to send to the module to stop firing the valves.
     * @param messageGapTime
     *                 Time to delay between subsequent send of the control message.
     * @param valveFireType
     *                 How to fire the valves, Pulse or Continuous.
     * @param pulseLength Time to keep brake active.
     * @param pulseGap Time to delay between valve pulses.
     */
    ABSValveControl(IProtocolFilter *filter, 
                    const string& valveControlFireMessageTag, 
                    const string& valveControlStopFireMessageTag,
                    const INT32& messageGapTime, 
                    const ValveFireType valveFireType, 
                    const INT32 pulseLength=0, 
                    const INT32 pulseGap=0);
    /**
     * Class destructor.
     */
    virtual ~ABSValveControl(void);

protected:
    /**
     * Fire/pulse the ABS valves according to the data used to construct this object.
     * <p>
     * <b>Description:</b><br>
     * The ABS module will be commanded to fire the ABS valves.  If the type of control is set to <i>Pulse</i>,
     * the valves will be commanded to stop firing after the required amount of time.  Otherwise, the valves will 
     * continue firing until commanded to stop.  If the type of control is set to <i>Pulse</i>, there will be a delay 
     * prior to commanding the valves to fire again.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      <li> GetFireMessageTag() </li>
     *      <li> GetValveControlType() </li>
     *      <li> GetPulseLengthTime()) </li>
     *      <li> GetStopFireMessageTag() </li>
     *      <li> GetPulseGapTime() </li>
     *      </ul>
     *
     * @return Status of firing/pulsing the ABS valves.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Valves were commanded successfully. </li>
     *      <li> BEP_STATUS_FAILURE - Could not command valves. </li>
     *      </ul>
     */
    virtual BEP_STATUS_TYPE MonitorData(void);
    /**
     * Get the message tag to use for firing the ABS valves.
     * <p>
     * <b>Description:</b><br>
     * Return the message tag used to fire the ABS valves.  The tag is passed into the constructor when the object is
     * created.
     * <p>
     * 
     * @return Valve fire message tag.
     */
    const string& GetFireMessageTag(void);
    /**
     * Get the message tag to use to stop firing the ABS valves.
     * <p>
     * <b>Description:</b><br>
     * Return the message tag to use to stop firing the ABS valves.  The tag is passed into the constructor when the 
     * object is created.
     * <p>
     * 
     * @return Stop valve fire message tag.
     */
    const string& GetStopFireMessageTag(void);
    /**
     * Get the amount of time to fire the ABS valves.
     * <p>
     * <b>Description:</b><br>
     * Return the length of time in milliseconds to fire the ABS valves.  The pulse length is passed into the 
     * constructor when the object is created.
     * @return Pulse length(in ms) to use for firing the ABS valves.
     */
    const INT32& GetPulseLengthTime(void);
    /**
     * Get the amount of time between firing of the ABS valves.
     * <p>
     * <b>Description:</b><br>
     * Return the length of time in milliseconds to wait before firing the ABS valves again.  The time delay is passed
     * in to constructor when the object is created.
     * <p>
     * 
     * @return Time(in ms) to wait before pulsing the ABS valves again.
     */
    const INT32& GetPulseGapTime(void);
    /**
     * Get the type of valve control to perform.
     * <p>
     * <b>Description:</b><br>
     * Return the type of valve control the calling function requested.  The valve control type is passed in to the
     * constructor when the object is created.
     * <p>
     * 
     * @return Valve fire control type.
     */
    const ValveFireType& GetValveControlType(void);

private:
    /**
     * Message tag for the fire command to send to the module.
     */
    string m_fireMessage;
    /**
     * Message tag for the stop fire command to send to the module.
     */
    string m_stopFireMessage;
    /**
     * Length of time to fire the ABS valves.
     */
    INT32  m_pulseLengthTime;
    /**
     * Length of time between firing of the ABS valves.
     */
    INT32  m_pulseGapTime;
    /**
     * Type of valve control requested.
     */
    ValveFireType m_controlType;
};

//-------------------------------------------------------------------------
#endif  //ABSValveControl_h
