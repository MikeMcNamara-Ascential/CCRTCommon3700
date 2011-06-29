//*************************************************************************
// Description:
//   Motor interface class
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/Motor/SerialMotor.h $
// 
// 3     10/30/07 4:17p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     8/17/05 8:19p Cward
// moved implementation out of header files.
//
// 1     5/11/05 10:06p Cward
//
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:44p Bmeinke
// Initial full core integration for Ford
//
// 1     6/14/04 10:56a Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:01a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 2     1/13/04 4:43p Bmeinke
// Adapted from Motor class in order to remove serial communication
// dependency from the base Motor class
// NOTE: Any calls made to Log() MUIST be scoped to Motor::Log() due to
// multiple ILogger inheritance from ILogicalPort and Motor base classes
//
//
//*************************************************************************
// $NoKeywords: $
//============================================================================
#ifndef _SERIALMOTOR_H_INCLUDED_
#define _SERIALMOTOR_H_INCLUDED_
//============================================================================

#include "Motor.h"
#include "ILogicalPort.h"


/**
 * Motor interface class
 *
 * @author Tim Bocehenk
 */
class SerialMotor : public Motor, public ILogicalPort
{
public:

    /**
     * Motor class consructor.
     */
    SerialMotor(int detachState=PTHREAD_CREATE_DETACHED);

    /**
     * Motor class desructor.
     */
    virtual ~SerialMotor(void);

    /**
     * Motor initialization method.  This will set up the motor
     * base on the configuration that is provided.
     *
     * @param config The configuration to use for motor initialization.
     * @return EOK if the initialization is successful, else error code (errno)
     */
    virtual const INT32 Initialize(const XmlNode *config);

protected:

    /**
     * Send a command to the drive
     *
     * @param message Command tag to send to the drive
     *
     * @return If operation succeeded
     */
    virtual const INT32 SendCommand(const std::string& message) = 0;

    /**
     * Send a variable length command to the drive
     *
     * @param message  Pattern from which to build command
     * @param tag Response tag
     * @param readResponseDelay
     *                 Amount of time to wait (in ms) before reading response data
     *
     * @return If operation succeeded
     */
    virtual const INT32 SendVariableCommand(const std::string message, const std::string tag, const INT32 readResponseDelay = 0) = 0;

    /**
     * Log a serial string to the logger
     *
     * @param Prefix   Data to be prepended to the string
     * @param response Serial string to be printed
     * @param mask     Verbocity mask for logger
     */
    void LogSerialString(const UINT32 mask, const std::string Prefix, const SerialString_t& response);

    /**
     * Get the string representing how a speed command should be built
     *
     * @return Speed string
     */
    virtual const char* GetTorqueCommand(void) = 0;

    /**
     * Get the string representing how a torque command should be built
     *
     * @return Torqu string
     */
    virtual const char* GetSpeedCommand(void) = 0;

};
#endif // _SERIALMOTOR_H_INCLUDED_

