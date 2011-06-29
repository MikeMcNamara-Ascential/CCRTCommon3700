//*************************************************************************
//
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Hydraulic/HydraulicMotorController.h $
// 
// 5     1/18/07 2:56p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added m_checkVehiclePresence.
// 
// 4     10/26/06 11:32a Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 3     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/16/06 11:34a Rwiersem
// Added methods for loading additional configuration items and handling
// the reload configuration pulse.  This is used to reload the
// configuration without restarting the process.
//
// 2     7/20/05 9:21p Cward
// Added support for commandable maximum boost pressure.
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
// 2     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     7/23/04 3:14p Bmeinke
// Moved the motor loading parameters to the MotorController class
// Moved the motor loading calculations to the
// MotorController::CalculateTorqueLoad() method
// Implement the new virtual ApplyMotorLoad() method for motor loading
//
// 1     12/21/03 6:41p Cward
//
// 2     10/24/03 1:44p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 2     6/26/03 1:30p Khykin
// Updated with Tims latest changes.
//
// 15    5/02/03 11:47a Tbochene
// Updated return values of methods to enable status checking
//
// 14    11/20/02 6:59a Khykin
// Updated for testing.
//
// 13    9/06/02 10:45a Khykin
// Adding a register method to conform to launcher strategy
//
// 12    4/12/02 2:41p Kh
// Updating due to changes in MotorController.
//
// 11    3/19/02 11:58a Kh
// Updating due to changes in BepComponent and to add Fault detect
//
// 10    3/07/02 6:51p Skypig13
// Update to new BepServer structure
//
// 9     3/07/02 2:45p Kh
// Updating.
//
// 7     1/07/02 4:58p Kh
// Adding an IDLE_MODE. Updating for changes to the BepServer.
//
// 6     12/27/01 4:25p Kh
// Adding a Loss Compensation mode to handle the differences between electric and
// hydraulic machines.
//
// 5     12/20/01 2:30p Kh
// Updating reset and reloadconfig methods.
//
// 4     10/25/01 10:02a Kh
// Checking in the latest code (debug not complete)
//
// 3     10/17/01 10:08p Kh
// Test checkin.
//
// 2     10/09/01 3:23p Kh
// All debugged, just waiting for the resource managers to be complete so
// I can test.
//
//
//*************************************************************************
#ifndef HYDRAULICMOTORCONTROLLER_H
#define HYDRAULICMOTORCONTROLLER_H
//==============================================================================
#include "MotorController.h"

#define CONTROLLOOP 0

// forward declaration of the test class
class HydraulicMotorControllerTest;

/**
 * Hydraulic Motor Controller class responsible for controlling
 * the hydraulic motor controller.
 *
 * @author Kevin Hykin
 * @version Version 1.3
 */
class HydraulicMotorController : public MotorController
{

    friend class HydraulicMotorControllerTest;

public:
    /**
     * Hydraulic Motor Controller constructor.
     * @since Version 1.0
     */
    HydraulicMotorController();

    /**
     * Motor Controller destructor.
     * @since Version 1.0
     */
    virtual ~HydraulicMotorController();

    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Method used to set all of the objects in the component
     * to their original states.  This would includes all
     * member variables such as buffers, lists, and stored
     * data.
     * @since Version 1.0
     */
    virtual void Reset(void);
    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
     *         BEP_SUCCESS_RESPONSE == Registered
     * @since Version 1.0
     */
    virtual const std::string Register(void);
    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     */
    virtual const std::string Publish(const XmlNode *node);
    /**
     * Request to Perform an action
     *
     * @param node    Action to be performed
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     */
    virtual const std::string Command(const XmlNode *node);
    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     * @since Version 2.0
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
    /**
     * Get the current system maximum boost pressure
     *
     * @return Current system maximum boost pressure
     */
    float GetMaximumBoostPressure(void);
protected:
    /**
     * Load the additional configuration items needed by the hydraulic motor
     * controller.  The "standard" items will be handled by the call to
     * MotorController::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    /**
     * Method that will read and update the local machine
     * information for the motor controller.
     * @since Version 1.0
     */
    virtual void UpdateMachineStatus(void);
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
     * @return Status of operation
     * @since Version 1.0
     */
    virtual const INT32 Boost(void);

    /**
     * This function applies a load to the drive axle of the machine
     *
     * @param motorLoad The amount of torquw to apply to the drive axle
     * @param masterRoll The drive wheel of the vehicle
     * @return Staus of operation
     * @since Version 1.1
     */
    const INT32 ApplyMotorLoad( float motorLoad, int masterRoll);

    /**
     * This function resets the motor loading constants to their default values. This
     * is done when the rolls are lowered to ensure that a "custom" loading profile
     * does not get applied to the wrong vehicle type.
     *
     * @since Version 1.1
     */
    virtual void SetDefaultLoading();

    /**
     * This function applies a speed to all wheels. It should
     * be used in all custom testing like Loss Compensation, Debug, ...
     * @return Status of operation
     * @since Version 1.0
     */
    virtual const INT32 Speed(void);
    /**
     * This function applies a torque to all wheels. Since this is
     * a hydraulic motor controller, if 0 torque is commanded, it
     * will disable boost and declutch the drives.
     *
     * @return Status of operation
     * @since Version 1.0
     */
    virtual const INT32 Torque(void);
    /**
     * This function applies a speed all wheels. It will handle
     * any startup sequencing that will need to be dealt with
     * to achieve the commanded speed.
     *
     * @return Status of operation
     * @since Version 1.1
     */
    virtual const INT32 LossCompensation(void);
    /**
     * This function will stop all rolls when called.  Its primary
     * use is stoping the rear rolls at the end of a brake test.
     * The function brake() in main will set motomode to QUICK_BRAKE
     * when it done sampling data.
     * @return Status of operation
     * @since Version 1.0
     */
    virtual const INT32 QuickBrake(void);

    /**
     * The function sets the boost pressure in the system.
     *
     * @param pressure The pressure to set for the system.
     * @return The status of commanding the pressure.
     * @since Version 1.0
     */
    INT32 SetBoostPressure(float pressure);

    /**
     * Method to read the current boost pressure from the system
     * and return it.
     *
     * @return The cuurent boost pressure.
     * @since Version 1.0
     */
    float GetBoostPressure(void);

    /**
     * Set the maximum boost pressure allowed for the machine.
     * By allowing the maximum boost pressure to be set from
     * outside the motor controller object, it allows tests
     * to change the system boost performance for simulating
     * different vehicle load characterists.
     *
     * The method checks to ensure the pressure parameter is
     * within the max and min allowed.  If not, it defaults to
     * the nearest bound.
     *
     * @param pressure value to set maximum boost pressure too.
     */
    void SetMaximumBoostPressure(float pressure);

    /**
     * PID algorithm for controlling the motors.
     *
     * @param desired The desired value for the speed (must have same units as actual).
     * @param actual  The current value for speed (must have same units as desired).
     * @param prop    Multiplication factor for adjusting to error (desired - actual)
     * @param integ   Controls how much error is used in the long term adjustment.
     * @param min     Mimimum value for output.
     * @param max     Maximum value for output.
     * @return The new value to use as the command.
     * @since Version 1.1
     */
    float MyPid(float desired, float actual, float prop, float integ,
                float min, float max);

    /**
     * Method to log system information at different stages of
     * execution.
     * @since Version 1.2
     */
    void LogInfo(void);

    /**
     * The minimum amount of pressure to use when boosting.
     * @since Version 1.0
     */
    float m_minimumBoostPressure;
    /**
     * The absolute maximum amount pressure to use when boosting.
     * @since Version 1.0
     */
    float m_absMaximumBoostPressure;
    /**
     * The current maximum amount pressure to use when boosting.
     */
    float m_currentMaximumBoostPressure;
    /**
     * The boost pressure to use for the system.
     * @since Version 1.0
     */
    float m_boostPressure;
    /**
     * The conversion factor to convert PSI to counts for AOUT.
     * information.
     * @since Version 1.0
     */
    float m_psiConversion;
    /**
     * The status of the machine boost.
     * @since Version 1.0
     */
    bool m_boost;

    /**
     * The previously commanded pressure.  This will reduce
     * the amount of nominal traffic on the system by allowing
     * the controller to send when the pressure changes.
     * @since Version 1.3
     */
    float m_previousPressure;
    /**
     * Wether to check for VehiclePresence during Boost mode
     * @since Version 1.0
     */
    bool m_checkVehiclePresence;
};
#endif // HYDRAULICMOTORCONTROLLER_H
