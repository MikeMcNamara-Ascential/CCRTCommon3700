//*************************************************************************
// Description:
//   Motor object that is used to command a serial motor.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/Baldor/BaldorMotor.h $
// 
// 3     10/30/07 4:16p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     8/17/05 8:27p Cward
// Beautified module and cleaned up javadocs comments
//
// 1     5/11/05 10:06p Cward
//
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 2     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:41p Cward
// 2     7/09/04 2:16p Bmeinke
// Derived from the new SerialMotor class (added to the inheritance tree
// to remove serial dependencies from the base MotorClass)
//
// 2     6/17/04 11:27p Bmeinke
// Changed parent class from Motor to SerialMotor
// Moved all method implementations to the cpp file
//
// 3     10/24/03 1:45p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/07/03 8:13a Khykin
// Updated with latest changes from Flatrock.
//
// 1     6/13/03 12:01p Bmeinke
// Tims latest changes
//
// 18    5/20/03 2:11p Tbochene
// Updated to use new inheritance structure
//
// 17    5/08/03 6:41p Tbochene
// Removed fault server interface and reporting (PlcMgr does this) and
// updated enabling communication fault reporting to report based on a
// threshhold of faults before it gets reported
//
// 16    5/08/03 1:49p Tbochene
// Added parameters to enable/disable thread lockup detection and setting
// of communication faults.  Changes to remove race conditions causing
// segmentation faults.
//
// 15    5/02/03 11:50a Tbochene
// Updated return values of methods to enable status checking
//
// 14    4/10/03 3:37p Tbochene
// Latest Motor Controller changes
//
// 13    3/28/03 9:54a Tbochene
// Lastest files after initial testing
//
// 12    3/26/03 2:36p Tbochene
// Remove old baggage
//
// 11    3/26/03 12:39p Tbochene
// Updated for robustness
//
// 9     11/27/02 8:43a Tbochene
// Updated from latest testing (removed unused defines)
//
// 8     11/19/02 9:35p Tbochene
// Removed embedded NULL from command strings and added diagnostics for
// debugging communications
//
// 7     11/08/02 11:59a Tbochene
// Made Enable(), Disable(), SetSpeed(), SetTorque(), and SetRPM()
// protected.  Added Set/GetFaultStatus() methods and converted to use
// SerialString_t instead of SERIAL_STRING
//
// 6     10/14/02 4:28p Tbochene
// Updated prototypes for many methods
//
// 5     9/16/02 11:15a Khykin
// Adding logging information.
//
// 4     9/06/02 10:44a Khykin
// Adding a fault server interface to report motor faults.
//
// 3     5/24/02 2:55p Kh
// Debugging.
//
// 2     4/12/02 2:42p Kh
// Updating for changes due to issues with the drives.
//
// 1     4/03/02 1:24p Kh
// Initial Checkin.
//
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef BaldorMotor_h
#define BaldorMotor_h
//==============================================================================

#include <string>
#include <string.h>

#include "Motor.h"

/* *********************** Baldor Motor Commands *************************
Ax      - Address motor x
AA      - Address All motors
AUn     - Auto Tune to number n
C       - Clear existing fault
CALC    - Calculates preset paramters from motor name plate
D       - Disable controller
DL      - Download parameters
E       - Enable motor
ECHO_ON - Echo On
ECHO_OFF - Echo Off
F       - returns current Fault code
GO      - process TAR command
H       - goto Home position
HF      - Help Fault.  like F command but gives text
HL      - Help Log.  like L command but gives text
HP      - Help Parameter.  full list
HPxxxx  - text of parmater xxxx
HPxxxx nnnn - change parameter xxxx to nnnn (Pxxxx nnnn).
ID      - returns list of power base settings
IO      - returns current IO status
Jx      - Jogs x.  (Jx = J+x,  J-x).
L       - Log.  send last 15 faults
M       - return last Move command
Mxxxx   - Move to xxxx
m       - incremental move
mxxxx   - incremental move to xxxx
O       - report current Output
Ox      - set current Output
P       - list all Parameters
Pxxxx   - list Parameter xxxx
Pxxxx nnnn - set Parameter xxxx to nnnn
S       - Stop
SCxxxx  - lock and unlock where xxxx is Security Code
ST      - Status
STATUS  - full Status
T       - report last Torque command
Tnnnn   - set Torque (+/- 2048  == +/- 100%).
TAR xxxx - Target Register
TP      - Test Points
TPF     - Test Point File
TPT     - Test Point Trigger
V       - last Velocity command
Vxxxx   - set Velocity (rpm).
vxxxx   - sets maximum speed for position command
W       - Position to absolute zero
Z       - set current postion to Zero
************************************************************************ */


#include "SerialMotor.h"

// defined commands to send to the drive
#define RPM_COMMAND     "V%d\rE\r"      /* command an rpm (Velocity x, enable drive)                    */
#define TORQUE_COMMAND  "T%d\rE\r"      /* command a torque (Torque x, enable drive)                    */

/**
 * Motor class that is responsible for controlling one electric
 * motor.  This currently has been designed to support the
 * Baldor 18H and 25M model drive.
 *
 * @author Kevin Hykin
 * @author Tim Bochenek
 */
class BaldorMotor : public SerialMotor
{
public:
    friend class MotorTest;
    friend class SerialMotorControllerTest;
    /**
     * BaldorMotor class consructor.
     */
    BaldorMotor(void);
    /**
     * BaldorMotor class desructor.
     */
    virtual ~BaldorMotor(void);

    /**
     * Upload the specified drive parameters to the motor.
     *
     * @param parameters The parameters to upload.
     * @return The status of the operation.
     */
    virtual const INT32 UploadDriveParameters(const XmlNode *parameters);

protected:
    /**
     * Send a command to the drive
     *
     * @param message Command tag to send to the drive
     *
     * @return If operation succeeded
     */
    const INT32 SendCommand(const std::string& message);
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
    virtual const INT32 SendVariableCommand(const std::string message, const std::string tag, const INT32 readResponseDelay = 0);

    /**
     * Enable the motor.  This must be done prior to commanding the
     * motor.
     *
     * @param enable Whether to enable (true) or disable (false) the drive
     *
     * @return The status of the motor enabling.
     */
    virtual const INT32 Enable(const bool enable);

    /**
     * Read the type of drive that the motor is commanding.
     *
     * @return The drive type.
     *         S18 - Baldor 18h
     *         S25 - Baldor 25m
     */
    const std::string ReadDriveType(void);

    /**
     * Get the string representing how a speed command should be built
     *
     * @return Speed string
     */
    const char* GetTorqueCommand(void);

    /**
     * Get the string representing how a torque command should be built
     *
     * @return Torqu string
     */
    const char* GetSpeedCommand(void);
};

#endif // BaldorMotor_h
