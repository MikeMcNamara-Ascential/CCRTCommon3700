//*************************************************************************
// Description:
//   Base server for all iterative servers.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Include/MotorController.h $
// 
// 5     10/30/07 4:18p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// - Added m_defaultDriveAxle.
// 
// 4     4/13/06 4:24a Cward
// added getspeeds and getwheelcount
//
// 3     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/16/06 11:33a Rwiersem
// Added methods for loading additional configuration items.  This is used
// to reload the configuration without restarting the process.
//
// 2     8/17/05 8:27p Cward
// Cleaned up javadocs comments
//
// 1     5/11/05 10:06p Cward
//
// 1     5/11/05 9:39p Cward
// Initial Version
//
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 4     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 4     2/18/05 12:54p Bmeinke
// Added Dan E's changes to open the ICM binary files only once
//
// 3     12/22/04 8:25p Bmeinke
// Added a new "DisableSpeedMatching" parameter to allow us to totally
// turn off all speed matching if needed (like with the U152/Explorer that
// is AWD, but not reported as AWD by eCATS)
//
// 2     12/14/04 8:58p Bmeinke
// Removed the #if(USE_BEP_MUTEX) blocks
//
// 4     8/30/04 3:41p Bmeinke
// Added a Publish() method to set the motor loading constants back to
// their default values when we lose retainers up
// Added SetDefaultLoading() method to reset motor loading constants to
// their default values
//
// 3     8/17/04 7:16p Bmeinke
// Added comment to IsMotorLoadingEnabled() prototype
//
// 2     7/23/04 3:14p Bmeinke
// Moved the motor loading parameters to the MotorController class
// Moved the motor loading calculations to the
// MotorController::CalculateTorqueLoad() method
// Added a pure virtual ApplyMotorLoad() method for motor loading
//
// 2     5/25/04 4:28p Cward
// Added declaraions needed to support the conditional clutch at zero
// speed.
//
// 1     12/21/03 6:41p Cward
//
// 3     10/24/03 1:44p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/07/03 7:58a Khykin
// Updated with latest changes from Flatrock.
//
// 33    6/17/03 10:03a Tbochene
// Updated synchronizatrion variables to BepMutex
//
// 31    5/28/03 12:50p Tbochene
// Updated heartbeat to be in a thread class
//
// 30    5/08/03 3:13p Tbochene
// Updated to fix heartbeat if PLC does not update heartbeat
//
// 29    5/08/03 1:45p Tbochene
// Updated Command() method to set command speed and torque to 0.0 when
// mode was changed.  This prevents rolls from taking off when previous
// speed command was set.  Removed SetStatus() in Register() to prevent
// race condition in derived classes.
//
// 28    5/06/03 3:12p Tbochene
// Updated diagnostics in HeartbeatThread()
//
// 27    5/02/03 11:50a Tbochene
// Updated return values of methods to enable status checking
//
// 26    4/10/03 3:37p Tbochene
// Latest Motor Controller changes
//
// 25    4/08/03 4:28p Tbochene
// Moved GetDataTag() stuff to BepServer
//
// 24    3/28/03 9:54a Tbochene
// Lastest files after initial testing
//
// 23    3/17/03 4:13p Tbochene
// Updated diagnostics
//    Revision 1.1.1.1  2003/02/15 00:05:43  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:08a Khykin
// Initial Checkin
//
// 1     1/18/03 8:05a Khykin
// Initial Checkin From DCTC
//
// 21    11/19/02 9:21p Tbochene
// Added SetCommandedSpeed() with float parameter
//
// 20    11/08/02 12:19p Tbochene
// Added Terminate() method so threads will be terminated when Motor
// Controller receives terminate (and process terminates), GetRollCount()
// to allow for more extensible use of class.  Added GetAccelerations()
// and variables to support reading accerations from file (/dev/ICM/...).
// Updated GetSpeeds() to use array data instead of descrete variables.
// Moved Heartbeat function from SerialMotorController().  Converted speed
// to arrays instead of descrete variables and added acceleration array.
//
// 19    11/04/02 7:15a Khykin
// Updated for direct ICM data reads.
//
// 18    10/15/02 6:09a Khykin
// Added another INamedDataBroker interface for performing
// clutch operations and a Semaphore to protect the method from
// multiple accesses.
//
// 17    9/16/02 11:16a Khykin
// Adding methods common to motor controllers.
//
// 16    8/23/02 8:31a Kh
// Synchronizing the pit machine with VSS.
//
// 15    4/29/02 3:36p Kh
// Updating due to changes for the serial motor controller.
//
// 14    4/03/02 1:23p Kh
// Removing the CalculateRPM method due to use only in the Serial.
//
// 13    3/19/02 12:03p Kh
// Adding the managing of faults to the controller.
//
// 12    3/07/02 6:51p Skypig13
// Update to new BepServer structure
//
// 11    3/06/02 2:34p Kh
// Adding notification for subscription data changes.
//
// 10    1/07/02 4:58p Kh
// Adding an IDLE_MODE. Updating for changes to the BepServer.
//
// 9     12/27/01 4:25p Kh
// Adding a Loss Compenation mode to handle the differences between electric and
// hydraulic machines.
//
// 8     12/21/01 1:29p Kh
// Changing the ControlLoop method to the Run method.
//
// 7     11/26/01 1:24p Kh
//
// 6     11/26/01 1:14p Kh
// Fixing constructor definition.
//
// 4     10/30/01 2:53p Kh
// Testing complete.  All looks good.
//
// 3     10/25/01 10:02a Kh
// Checking in the latest code (debug not complete)
//
// 2     10/09/01 3:23p Kh
// All debugged, just waiting for the resource managers to be complete so
// I can test.
//
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MotorController_h
#define MotorController_h
//==============================================================================

#include <vector>

#include "BepSync.h"
#include "BepServer.h"
#include "IFaultServer.h"
#include "HeartBeatThread.h"
#include "MotorControllerDefs.h"

/**
 * Data type definition of roll indicies
 */
typedef enum Rolls
{
    All = -1, LeftFront = 0, RightFront, LeftRear, RightRear, LeftTandem, RightTandem
};

/**
 * MotorController base class.  This enforces the interface
 * between the different motorcontrollers and the system.
 *
 * @author Kevin Hykin
 * @author Tim Bochenek
 */
class MotorController : public BepServer
{
    friend class CommonMotorControllerTest;
public:
    /**
     * Descriptions for each motor
     */
    static std::vector<std::string> MotorDescriptions;
    /**
     * Constant representing forward roller direction
     */
    const static std::string Forward;
    /**
     * Constant representing reverse roller direction
     */
    const static std::string Reverse;
    /**
     * Constant representing on state of a bit
     */
    const static std::string On;
    /**
     * Constant representing off state of a bit
     */
    const static std::string Off;
    /**
     * Constant representing enabled state
     */
    const static std::string Enabled;
    /**
     * Constant representing disabled state
     */
    const static std::string Disabled;
    /**
     * Constant representing faulted state
     */
    const static std::string Faulted;
    /**
     * Constant representing true string value
     */
    const static std::string True;
    /**
     * Constant representing false string value
     */
    const static std::string False;
    /**
     * Motor Controller constructor.
     */
    MotorController(void);
    /**
     * Motor Controller destructor.
     */
    virtual ~MotorController(void);
    /**
     * Method for initialization.
     *
     * @param fileName The configuration file name.
     */
    void Initialize(const std::string &fileName);
    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
     *         BEP_SUCCESS_RESPONSE == Registered
     */
    virtual const std::string Register(void);
    /**
     * Main run method that subclasses can overload if different
     * behavior is desired.
     *
     * @param terminateFlag
     *               Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
    /**
     * Main control loop
     */
    virtual void MainLoop(void);
    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
    /**
     * Request to Perform an action
     *
     * @param node    Action to be performed
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     */
    virtual const std::string Command(const XmlNode *node);
    /**
     * Request to write a data tag.  This will respond with "Unsupported"
     * to processes that request to write information.  Data can only be
     * updated with "Command" messages!
     *
     * @param node    Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     */
    virtual const std::string Write(const XmlNode *node);
    /**
     * Method used to terminate the component.  This performs
     * any cleanup that the destructor would not and sets
     * the state of the component to terminate.
     */
    virtual void Terminate(void);
    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     */
    virtual const std::string Publish(const XmlNode *node);
    /**
     * Return the value of the special command
     *
     * @return
     */
    const std::string GetSpecialCommand(void);
    /**
     * Method to get the motor mode.
     *
     * @param roll   Roll for which to get the motor mode (All == all rolls)
     *
     * @return The current motor mode.
     */
    virtual const std::string GetMotorMode(const INT8 roll = All);

protected:
    /**
     * Load the additional configuration items needed by the motor
     * controller.  The "standard" items will be handled by the call to
     * BepServer::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    /**
     * Setup motor loading based on the data in the configuration file.  If
     * the motor loading tags are missing, default values will be set.
     *
     * @param motorParameters A pointer to the motor parameters node
     */
    virtual void SetupMotorLoading(const XmlNode *motorParameters);
    /**
     * Method to set the motor mode.
     *
     * @param motorMode The mode to run the motor task in.
     * @param roll      Roll to set mode for
     */
    virtual void SetMotorMode(const std::string &motorMode, const INT8 roll = All);
    /**
     * This function controls wheel speed as needed in various
     * tests. For AWD, if a side to side speed dif is detected,
     * the slower wheel will be, "boosted" to match the faster
     * wheel. In RWD or FWD modes the left side is in the torque
     * mode, while the right side is speed matching.
     * The non-driven axle is always in the speed match mode.
     * When driving in reverse, the non-driven axle may be boosted.
     * Assumption is made that reverse driving stays below 15 mph
     * or so.
     *
     * REVERSE boost was introduced to allow steering for RWD
     * vehicles
     * DISABLE means to zero the torque (without extra enable
     * command).
     * LF/RF/LR/RR command pattern is maintained for all modes
     * RPM values calculated for motor commands include
     *
     * @return Staus of operation
     */
    virtual const INT32 Boost(void)=0;
    /**
     * This function applies a speed to all wheels. It should
     * be used in all custom testing like Debug, ...
     *
     * @return Staus of operation
     */
    virtual const INT32 Speed(void)=0;
    /**
     * This function applies a torque to all wheels. It should
     * be used in all custom testing like Debug, ...
     *
     * @return Staus of operation
     */
    virtual const INT32 Torque(void)=0;
    /**
     * This function applies a speed all wheels. It will handle
     * any startup sequencing that will need to be dealt with
     * to achieve the commanded speed.
     *
     * @return Staus of operation
     */
    virtual const INT32 LossCompensation(void)=0;
    /**
     * This function calculates the amount of torque load to be applied
     * for motor loading
     *
     * @return Torque to be applied to the drive axle
     */
    virtual float CalculateTorqueLoad(int masterRoll);
    /**
     * This function applies a load to the drive axle of the machine
     *
     * @param motorLoad The amount of torquw to apply to the drive axle
     * @param masterRoll The drive wheel of the vehicle
     * @return Staus of operation
     */
    virtual const INT32 ApplyMotorLoad( float motorLoad, int masterRoll) = 0;
    /**
     * This function resets the motor loading constants to their default values. This
     * is done when the rolls are lowered to ensure that a "custom" loading profile
     * does not get applied to the wrong vehicle type.
     */
    virtual void SetDefaultLoading();
    /**
     * This function will stop all rolls when called.  Its primary
     * use is stoping the rear rolls at the end of a brake test.
     * The function brake() in main will set motomode to QUICK_BRAKE
     * when it done sampling data.
     * @return Staus of operation
     */
    virtual const INT32 QuickBrake(void);
    /**
     * Assist the rear brakes of the vehicle in stopping.  This
     * is used for small cars that do not have any rear brake
     * force.  (Ex. In the Neon with Teves ABS, if the rear wheels
     * are rotating at 20KPH more than the front a fault is
     * recorded).
     * @return Staus of operation
     */
    virtual const INT32 RearBrakeAssist(void);
    /**
     * Command to upload parameters to a serial motor drive.
     * @return Staus of operation
     */
    virtual const INT32 UploadDriveParameters(void);
    /**
     * Command motor controller to enter mode where all
     * rolls can be commanded independantly
     * @return Staus of operation
     */
    virtual const INT32 IndividualControlMode(void);
    /**
     * This is a mode that can be overridden if a special
     * mode is needed that is not defined in the base class.
     * @return Staus of operation
     */
    virtual const INT32 SpecialMode(void);
    /**
     * Method that will read and update the local machine
     * information for the motor controller.
     * @return Staus of operation
     */
    virtual void UpdateMachineStatus(void);
    /**
     * Get the current wheel speeds.
     *
     * @param buff   Optional buffer to fill in with speeds
     * @param count  Number of items in the buffer passed in
     *
     * @return The status of the operation.
     */
    const INT32 GetSpeeds(float *buff=NULL, int count=0);
    /**
     * Get all of the roller accelerations.
     *
     * @return The status of the operation.
     */
    const INT32 GetAccelerations(void);
    /**
     * Return the value of the special (non-standard) command
     *
     * @return
     */
    void SetSpecialCommand(const std::string& command);
    /**
     * Method to set the clutch status.  "On" or "Off"
     *
     * @param clutch The status of the clutch. "On" or "Off"
     * @return The status of the operation.
     */
    const INT32 Clutch(std::string clutch);
    /**
     * Method to set the boost status.  "On" or "Off"
     *
     * @param boost The status of the machine boosting. "On" or "Off"
     * @return The status of the operation.
     */
    const INT32 SetBoostStatus(std::string boost);
    /**
     * Enable or disable motor loading
     *
     * @param enable true to enable motor loading, false to disable (default=true)
     */
    virtual void EnableMotorLoading( bool enable=true);
    /**
     * Check if motor loading is currently enabled
     *
     * @return true if motor loading is currently enabled, false if it is didabled
     */
    virtual const bool IsMotorLoadingEnabled() const;
    /**
     * Create a string containing all float values in array with
     * the supplied prfix
     *
     * @param data   Array of data to convert
     * @param prefix Prefix to put at beginning of string
     *
     * @return String with prefix and data
     */
    const std::string CreateStringFromFloat(const std::vector<float>& data, const std::string prefix);
    /**
     * Determines if the boost should be applied to rolls
     *
     * @param masterSpeed
     *                   Speed of master roll
     * @param slaveSpeed Speed of slave (on same axle as master) roll
     *
     * @return If boost should be applied
     */
    const bool ShouldBoost(const float masterSpeed, const float slaveSpeed);
    /**
     * Check the status of the system based on the system
     * entries listed under the faults node.
     * Note:  All faults must be subscribed for.
     *
     * @return The status of the system.
     */
    virtual const INT32 CheckStatus(void);
    /**
     * Returns amount of time to wait between loop
     * iterations for the motor controller
     *
     * @return Delay time in ms between loop iterations
     */
    const INT32 GetUpdateRate(void) const;
    /**
     * Store the flag indicating of the rollers should be
     * clutched automatically when zero speed is achieved.
     *
     * @param clutchOnZeroSpeed
     *               True - clutch rollers when zero speed is achieved.
     */
    void SetClutchOnZeroSpeed(const bool& clutchOnZeroSpeed);
    /**
     * Get the flag indicating if the roller should
     * automatically clutch when zero speed is achieved.
     *
     * @return True - Rollers will automatically clutch on zero speed.
     */
    const bool& ClutchOnZeroSpeed(void);
    /**
     * Returns the number of rollers to be controlled during a test
     *
     * @return 4 or 6
     */
    const INT32 GetWheelCount(void);
    /**
     * Named Data Broker interface to retrieve all of the machine
     * information to make control decisions.
     */
    INamedDataBroker *m_dataComm;
    /**
     * Named Data Broker interface for commanding the clutches.
     * Note: this must be independant of the m_dataComm due the
     * clutch command will be asynchronous.
     */
    INamedDataBroker *m_commandComm;

    /**
     * Synchronization varible used to protect the clutch commanding
     */
    BepMutex m_clutchSync;
    /**
     * Synchronization varible used to protect startup
     */
    BepMutex m_startEndSync;
    /**
     * Synchronization varible used to protect access to the Motor Mode
     */
    BepMutex m_modeAccess;
    /**
     * Fault Server interface to report faults to the machine.
     */
    IFaultServer *m_faultComm;
/* ************ */
/* Updated Data */
/* ************ */
    /**
     * Array of speeds for wheels
     */
    std::vector<float> m_speeds;
    /**
     * Array of accelerations for wheels
     */
    std::vector<float> m_accelerations;
/* ************** */
/* Parameter Data */
/* ************** */
    /**
     * Acceleration to determine when the vehicle is braking.
     */
    float m_brakeDetectLimit;
    /**
     * Acceleration to determine when the vehicle has stopped braking and begun accelerating.
     */
    float m_brakeClearLimit;
    /**
     * Speed difference between wheels to engage boost.
     */
    float m_speedDeltaForBoost;
    /**
     * The rate at which to update the the motor status.  This
     * is in mili-seconds.  Ex 100 would update the drives 10
     * times a second.
     */
    INT32 m_updateRate;
    /**
     * Parser that is used to load the configuration file.
     */
    XmlParser m_parser;
    /**
     * The minimum speed the rollers must be moving to apply boost.
     */
    float m_minimumSpeedForBoost;

    /**
     * Default acceleration loading constant used for loading purposes. This
     * value will be "applied" when the retainers are dropped.
     */
    float   m_defaultAccelConst;

    /**
     * Acceleration loading constant used for loading purposes.
     */
    float   m_accelerationLoadingConstant;

    /**
     * Default speed loading constant used for loading purposes. This
     * value will be "applied" when the retainers are dropped.
     */
    float   m_defaultSpeedConst;

    /**
     * Speed loading constant used for loading purposes.
     */
    float   m_speedLoadingConstant;

    /**
     * Default torque loading constant used for loading purposes. This
     * value will be "applied" when the retainers are dropped.
     */
    float   m_defaultTorqueConst;

    /**
     * Torque loading constant used for loading purposes.
     */
    float   m_torqueLoadingConstant;

    /**
     * Minimum speed for torque loading to be applied
     */
    float   m_minSpeedForLoading;

    /**
     * Maximum speed for torque loading to be applied
     */
    float   m_maxSpeedForLoading;

    /**
     * System data item used to turn motor loading on/off
     */
    bool    m_motorLoadingEnabled;

    /**
     * Flag used to totally disable speed matching
     * (like an AWD override)
     */
    bool    m_disableSpeedMatching;

    /**
     * File descriptor for the wheel speed data.
     */
    INT32  m_speedFile;
    /**
     * File descriptor for the wheel acceleration data.
     */
    INT32  m_accelFile;

private:
    /**
     * Faults that the controller should monitor to
     * ensure that the system is functioning correctly.
     */
    XmlNodeMap m_faults;
    /**
     * The location of the speed array for updating the motor
     * speed information.
     */
    std::string m_speedArray;
    /**
     * The location of the acceleration array for updating the motor
     * acceleration information.
     */
    std::string m_accelerationArray;
    /**
     * Flag indicating if boost should be applied to rolls when in reverse
     */
    bool m_reverseBoost;
    /**
     * Flag to determine if rollers should be clutched
     * when zero speed is achieved.
     */
    bool m_clutchOnZeroSpeed;
    /**
     * Value to set for special (non-standard commands)
     */
    std::string m_specialCommand;
    /**
     * Thread object used to manage heartbeat
     */
    HeartBeatThread* m_heartBeatThread;
    /**
     * String to tell which axle speed should be sent to the GUI if neither is
     * selected.
     */
    std::string m_defaultDriveAxle;
};
#endif // MotorController_h

