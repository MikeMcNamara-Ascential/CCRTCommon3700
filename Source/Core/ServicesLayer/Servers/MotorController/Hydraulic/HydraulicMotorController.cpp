//*************************************************************************
// Description:
//   Base motor controller server.  This provides all of the funcitionality
//   to make the decisions to command the rollers and change the modes.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Hydraulic/HydraulicMotorController.cpp $
// 
// 6     1/18/07 2:56p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added a parameter BoostCheckVehiclePresence to tell if we should
// check for vehicle present during boost.  Added a member variable
// m_checkVehiclePresence to store the parameter value.
// 
// 5     10/26/06 11:32a Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 4     4/13/06 4:23a Cward
// init var
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
// 3     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     1/17/05 9:59a Bmeinke
// Removed references to Simulator Switch (not present on CCRT)
//
// 3     7/23/04 3:14p Bmeinke
// Moved the motor loading parameters to the MotorController class
// Moved the motor loading calculations to the
// MotorController::CalculateTorqueLoad() method
// Implement the new virtual ApplyMotorLoad() method for motor loading
//
// 2     6/24/04 1:16a Cward
// Removed pound if 0 block. Updated header block comments to the new
// standard.
//
// 1     12/21/03 6:41p Cward
//
// 4     10/24/03 1:44p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:30a Khykin
// Initial checkin
//
// 3     8/12/03 2:55p Khykin
// Updated with new directory update changes.
//
// 2     8/11/03 4:38p Khykin
// Updating with what was on Tims Laptop.
//
// 20    5/08/03 1:43p Tbochene
// Updated Register to set status to running since it was removed from
// base class to prevent race condition
//
// 19    5/02/03 11:43a Tbochene
// Changed prototypes of methods to include return type for status
// checking
//
// 18    11/08/02 12:40p Tbochene
// Update to use speed arrays
//
// 17    9/06/02 10:42a Khykin
// Updating the initialization method parameter format.
//
// 16    8/23/02 8:39a Kh
// Synchronizing the pit machine with VSS.
//
// 14    4/29/02 3:32p Kh
// Changing requests for subscription data.
//
// 13    4/12/02 2:40p Kh
// Updating due to changes for the SerialMotorController.
//
// 12    4/03/02 1:26p Kh
// Updating due to changes in the BepServer.
//
// 11    3/19/02 12:34p Kh
// Updating for BepComponent changes.
//
// 10    3/07/02 6:51p Skypig13
// Update to new BepServer structure
//
// 9     3/06/02 6:13p Kh
// Updating.
//
// 8     3/06/02 2:11p Kh
// Updating after debug with the system.
//
// 7     1/07/02 4:58p Kh
// Adding an IDLE_MODE. And updating for changes to BepServer.
//
// 6     12/27/01 4:25p Kh
// Adding a LossCompensation mode to handle the differences between electric and
// hydraulic machines.
//
// 5     12/20/01 2:32p Kh
// Component Tested.
//
// 4     10/30/01 2:52p Kh
// Testing complete.  All looks good.
//
// 3     10/25/01 10:02a Kh
// Checking in the latest code (debug not complete)
//
// 2     10/09/01 3:22p Kh
// All debugged, just waiting for the resource managers to be completed to
// test.
//
// 1     10/09/01 10:51a Admin
// Initial revision, still needs debug.
//
// 1     10/09/01 10:49a Kh
// Initial revision, still needs debug.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "HydraulicMotorController.h"

/**
 * Hydraulic Motor Controller constructor.
 * @since Version 1.0
 */
HydraulicMotorController::HydraulicMotorController() : MotorController()
{
    m_minimumBoostPressure = 50.0;
    m_absMaximumBoostPressure = 800.0;
    m_currentMaximumBoostPressure = 800.0;
    m_minimumSpeedForBoost = 3.0;
    m_psiConversion = 4.095;
    m_previousPressure = -1;
}

/**
 * Motor Controller destructor.
 * @since Version 1.0
 */
HydraulicMotorController::~HydraulicMotorController()
{
}

/**
 * Method for initialization.
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void HydraulicMotorController::Initialize(const XmlNode *document)
{
    if (document != NULL)
    {   // initialize the hydraulic motor controller
        try
        {
            LoadAdditionalConfigurationItems(document);
        }
        catch (XmlException &e)
        {
            Log(LOG_ERRORS, "HydraulicMotorController::Initialize Error: %s\n", e.what());
            throw;
        }

        // initialize the base class last so that the task will not be registered until ready to run
        MotorController::Initialize(document);
    }
    else
        throw BepException("Invalid Configuration File Provided");
}

void HydraulicMotorController::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    const XmlNode *setup = document->getChild("Setup")->getChild("HydraulicMotorParameters");

    try
    {
        m_minimumBoostPressure = atof(setup->getChild("MinimumBoostPressure")->getValue().c_str());
    }
    catch ( ...)
    {
        m_minimumBoostPressure = 50.0;
    }

    try
    {
        m_absMaximumBoostPressure = atof(setup->getChild("MaximumBoostPressure")->getValue().c_str());
    }
    catch (...)
    {
        m_absMaximumBoostPressure = 800.0;
    }

    try
    {
        m_boostPressure = atof(setup->getChild("BoostPressure")->getValue().c_str());
    }
    catch (...)
    {
        m_boostPressure = 600.0;
    }

    try
    {
        m_psiConversion = atof(setup->getChild("PSIConversion")->getValue().c_str());
    }
    catch (...)
    {
        m_psiConversion = 4.095;
    }

    try
    {   //some wheels will reflect back causing no VehiclePresence during test. This stops Boost!!
        m_checkVehiclePresence = atoi(setup->getChild("BoostCheckVehiclePresence")->getValue().c_str());
    }
    catch (...)
    {   //Default to check for VehiclePresence during boost. This is the original mode.
        m_checkVehiclePresence = 1;
    }

    // Save the absolute maximum boost pressure to the current maximum boost pressure.
    //  This then allows the system to change the maximum boost pressure at anytime without
    //  losing the absolute maximum boost pressure the system should allow.
    m_currentMaximumBoostPressure = m_absMaximumBoostPressure;
}

const std::string HydraulicMotorController::Register(void)
{
    std::string status = MotorController::Register();
/*
    // If not running on a 360 or 2400 machine, terminate
    if((ReadSubscribeData(GetDataTag("MachineType")) == "360") ||
       (ReadSubscribeData(GetDataTag("MachineType")) == "2400"))
*/
    // if running on a 3600 machine, terminate
    if (ReadSubscribeData(GetDataTag("MachineType")) == "0")
    {
        Log(LOG_DEV_DATA, "Hydraulic Machine Detected, Running\n");
        SetStatus(BEP_RUNNING_STATUS);
    }
    else
    {
        Log(LOG_DEV_DATA, "Electric Machine Detected, Terminating\n");
        SetStatus(BEP_TERMINATE_STATUS);
        status = BEP_FAILURE_RESPONSE;
    }

    return(status);
}
const std::string HydraulicMotorController::Publish(const XmlNode *node)
{
    std::string status;

    Log(LOG_FN_ENTRY, "Enter HydraulicMotorController::Publish(%s)\n", node->ToString().c_str());

    // Rolls were Up, now they are going down
    if ((node->getName() == GetDataTag("RollsUpElevDown")) && (node->getValue() == False))
    {
        // Reset the max boost pressure to its original from the configuration file
        SetMaximumBoostPressure(m_absMaximumBoostPressure);
    }

    status = MotorController::Publish(node);

    Log(LOG_FN_ENTRY, "Exit HydraulicMotorController::Publish(%s)\n", node->ToString().c_str());

    return( status);
}
const std::string HydraulicMotorController::Command(const XmlNode *node)
{
    Log(LOG_FN_ENTRY, "HydraulicMotorController::Command(%s)\n", node->ToString().c_str());
    std::string status(BEP_INVALID_RESPONSE);

    const std::string name = node->getName(), value = node->getValue();
    if (MAX_BOOST_PRESSURE_VALUE == name)
    {
        Log(LOG_DEV_DATA, "Commanded to %s maximum boost pressure\n", value.c_str());
        if ( value.empty() != true)
        {
            status = BEP_SUCCESS_RESPONSE;
            SetMaximumBoostPressure(atof(value.c_str()));
        }
        else Log(LOG_ERRORS, "Invalid maximum boost pressure received: %s\n", value.c_str());
    }
    else if (RESET_MAX_BOOST_PRESSURE == name)
    {
        status = BEP_SUCCESS_RESPONSE;
        SetMaximumBoostPressure(m_absMaximumBoostPressure);
    }

    else status = MotorController::Command(node);

    Log(LOG_FN_ENTRY, "HydraulicMotorController::Command(%s) [status = %s] done\n",
        node->ToString().c_str(), status.c_str());

    return(status);
}
void HydraulicMotorController::UpdateMachineStatus(void)
{   // call the base class to update its status
    Log(LOG_FN_ENTRY, "HydraulicMotorController::UpdateMachineStatus()\n");
    try
    {
        MotorController::UpdateMachineStatus();
    }
    catch (XmlException &e)
    {
        Log(LOG_ERRORS, "MotorController Could Not Update The Machine, XmlException %s\n", e.what());
    }
    catch (BepException &e)
    {
        Log(LOG_ERRORS, "MotorController Could Not Update The Machine, BepException %s\n", e.what());
    }

    Log(LOG_FN_ENTRY, "HydraulicMotorController::UpdateMachineStatus() done\n");
}
const INT32 HydraulicMotorController::Boost(void)
{
    INT32 status = BEP_STATUS_SUCCESS;  // status to use for operations in boost
    std::string tag;        // tag for retrieving data
    std::string value;      // the value of the retrieved data
    std::string response;   // the response from the requested data

    Log(LOG_FN_ENTRY, "Motor Controller Boosting()\n");
    LogInfo();  // log the system information

    // Disable boost at low speeds or
    // If (drive wheel < minspeed) OR zerospeed OR not clutched
    // OR rolls down OR cal key on OR sim key on
    // OR no vehicle presence AND parameter set to check vehicle presence
    if (((m_speeds[LeftFront] < m_minimumSpeedForBoost) && (ReadSubscribeData(GetDataTag("DriveWheel")) == "Front")) ||
        ((m_speeds[LeftRear] < m_minimumSpeedForBoost) && (ReadSubscribeData(GetDataTag("DriveWheel")) == "Rear")) ||
        (ReadSubscribeData(GetDataTag("Zerospeed")) == "1") || (ReadSubscribeData(MACHINE_CLUTCH) == "0") ||
        (ReadSubscribeData(GetDataTag("RollsUpElevatorDown")) == "0") || (ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")) == "1") ||
        ((ReadSubscribeData(GetDataTag("VehiclePresence")) == "0") && (m_checkVehiclePresence == 1)))
    {
        Log(LOG_DEV_DATA, "Boost Disabled\n");
        if (ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")) == "0")   // if not here because of Loss Compensation
        {
            // because this is a hydraulic controller task for an AtoD card
            // set the boost to 0
            status = SetBoostPressure(0);
            if (status != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Boost Could Not Set Pressure to 0\n");
            SetBoostStatus("Off");  // turn the boost off
        }
    }
    // ELSE Enable boost to eliminate speed difference between axles
    else
    {
        Log(LOG_DEV_DATA, "Boost Enabled\n");
        float newPres;  // the new pressure to apply to the hydraulic system

        // if the non driven wheel is faster than the driven by the delta, dont drive
        if (((ReadSubscribeData(GetDataTag("DriveWheel")) == "Rear") && ((m_speeds[RightFront] - m_speeds[RightRear]) >= m_speedDeltaForBoost)) ||
            ((ReadSubscribeData(GetDataTag("DriveWheel")) == "Front") && ((m_speeds[RightRear] - m_speeds[RightFront]) >= m_speedDeltaForBoost)))
        {
            m_boost = false;    // signal stop boosting
        }
        // else if the driven wheel is faster than the non-driven wheel
        else if (((ReadSubscribeData(GetDataTag("DriveWheel")) == "Rear") && (m_speeds[RightRear] > m_speeds[RightFront])) ||
                 ((ReadSubscribeData(GetDataTag("DriveWheel")) == "Front") && (m_speeds[RightFront] > m_speeds[RightRear])))
        {
            m_boost = true;     // signal boost
        }

        // if we are boosting calculate the new pressure
        if (m_boost)
        {   // new pressure = boost pressue + ((the boost pressure - current psi)/10)
            float currentPSI = GetBoostPressure();
            newPres = m_boostPressure + ((m_boostPressure - currentPSI) / 10);
            Log(LOG_DEV_DATA, "New Pressure %f, %f = %f\n", m_boostPressure, currentPSI, newPres);
            // range check the new boost pressure
            if (newPres > m_currentMaximumBoostPressure)    // if greater than max
                newPres = m_currentMaximumBoostPressure;        // set to max
            else if (newPres < m_minimumBoostPressure)  // if less than min
                newPres = m_minimumBoostPressure;       // set to min
        }
        else        // else set the pressure to the minimum pressure
            newPres = 0;

        // set the boost pressure
        status = SetBoostPressure(newPres);
        if (status != BEP_STATUS_SUCCESS)
            Log(LOG_ERRORS, "Boost Could Not Command New Pressure: %s\n", value.c_str());

        // Display boost status on panel (marquee?)
        status = SetBoostStatus("On");
        if (status != EOK)
            Log(LOG_ERRORS, "Boost Could Not Command Boost Light: %s\n", value.c_str());
    }
    return(status);
}

const INT32 HydraulicMotorController::ApplyMotorLoad( float motorLoad, int masterRoll)
{
    return( EOK);
}

void HydraulicMotorController::SetDefaultLoading()
{
    // Set the max boost pressure back to the default
    SetMaximumBoostPressure(m_absMaximumBoostPressure);
}

const INT32 HydraulicMotorController::Speed(void)
{   // compare the current MPH to the driven MPH
    INT32 status;           // status to use for operations in boost
    std::string tag;        // tag for retrieving data
    std::string value;      // the value of the retrieved data
    std::string response;   // the response from the requested data
    float newPres;          // the new pressure to apply to the hydraulic system

    Log(LOG_FN_ENTRY, "Motor Controller Speed()\n");
    LogInfo();

    // if the rollers are too slow, drive them
    float commandedSpeed = atof(ReadSubscribeData(COMMAND_SPEED).c_str());
#if CONTROLLOOP
    Log(LOG_DEV_DATA, "Using Control Loop\n");
    if ((commandedSpeed > m_minimumSpeedForBoost) &&
        (ReadSubscribeData(MACHINE_CLUTCH) == "1") &&
        ((ReadSubscribeData(GetDataTag("VehiclePresence")) == "1") ||
         (ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")) == "1")))
    {
        float boostPressure = GetBoostPressure();   // get the current boost pressure
        Log(LOG_DEV_DATA, "BoostPressure: %f, %f, %f, %f, %f\n", boostPressure, m_speeds[LeftFront], m_speeds[RightFront], m_speeds[LeftRear], m_speeds[RightRear]);
        // if currently boosting, adjust for control
        if (boostPressure > 0)
        {   // if not at commanded speed, increase the boost pressure by 10%
            if ((m_speeds[RightFront] < commandedSpeed) && (m_speeds[LeftFront] < commandedSpeed) &&
                (m_speeds[LeftRear] < commandedSpeed) && (m_speeds[RightRear] < commandedSpeed))
            {
                newPress = m_boostPressure + ((m_boostPressure - boostPressure) * .1);
            }
            // if all have overshot the commanded speed decrease by 10%
            else if ((m_speeds[RightFront] > commandedSpeed) || (m_speeds[LeftFront] > commandedSpeed) ||
                     (m_speeds[LeftRear] > commandedSpeed) || (m_speeds[RightRear] > commandedSpeed))
            {
                newPress = m_boostPressure - ((m_boostPressure - boostPressure) * .1);
            }
            // else we must be at a good pressure compensate for losses
            else
            {
                newPress = m_minimumBoostPressure;
            }
        }
        else
        {
            newPres = m_boostPressure;
        }

        // range check the boost
        //////////////////////////

        // if greater than max, set to max
        if (newPres > m_currentMaximumBoostPressure)    newPres = m_currentMaximumBoostPressure;
        // if less than min, set to min
        else if (newPres < m_minimumBoostPressure)       newPres = m_minimumBoostPressure;
        Log(LOG_DEV_DATA, "New Pressure: %f\n", newPres);
    }
    else // disable due to machine conditions
    {
        Log(LOG_DEV_DATA, "Commanded Speed: %f, MinimumSpeedForBoost: %f, Clutch: %s, VehiclePresent: %s, LossCompensation: %s\n",
            commandedSpeed, m_minimumSpeedForBoost, ReadSubscribeData(MACHINE_CLUTCH).c_str(),
            ReadSubscribeData(GetDataTag("VehiclePresence")).c_str(),
            ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")).c_str());
        newPres = 0;
    }
#else
    if ((commandedSpeed > m_minimumSpeedForBoost) &&
        (ReadSubscribeData(MACHINE_CLUTCH) == "1") &&
        ((ReadSubscribeData(GetDataTag("VehiclePresence")) == "1") ||
         (ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")) == "1")))
    {
        if ((m_speeds[RightFront] < commandedSpeed) || (m_speeds[LeftFront] < commandedSpeed) ||
            (m_speeds[LeftRear] < commandedSpeed) || (m_speeds[RightRear] < commandedSpeed))
        {
            // new pressure = boost pressue + ((the boost pressure - current psi)/10)
//          newPres = m_boostPressure + ((m_boostPressure - GetBoostPressure()) / 10);
            newPres = m_previousPressure + 50; // ((m_boostPressure - GetBoostPressure()) / 10);
            Log(LOG_DEV_DATA, "New Boost Pressure: %f\n", newPres);
            // range check the new boost pressure
            if (newPres > m_currentMaximumBoostPressure)    // if greater than max
                newPres = m_currentMaximumBoostPressure;        // set to max
            else if (newPres < m_minimumBoostPressure)  // if less than min
                newPres = m_minimumBoostPressure;       // set to min
        }
        else
        {
            newPres = m_previousPressure - 10;
            if (newPres < m_minimumBoostPressure)       // if less than min
                newPres = m_minimumBoostPressure;       // set to min
        }
    }
    else                        // else set the pressure to 0 to stop
    {
        newPres = 0;
    }
#endif

    // set the boost pressure
    status = SetBoostPressure(newPres);
    if (status != BEP_STATUS_SUCCESS)
        Log(LOG_ERRORS, "Speed Could Not Command New Pressure: %s\n", value.c_str());

    // Display boost status on panel (marquee?)
    status = SetBoostStatus("On");
    if (status != EOK)
        Log(LOG_ERRORS, "Speed Could Not Command Boost Light: %s\n", value.c_str());
    return(status);
}
const INT32 HydraulicMotorController::Torque(void)
{   // compare the current MPH to the driven MPH
    INT32 status;           // status to use for operations in boost
    std::string tag;        // tag for retrieving data
    std::string value;      // the value of the retrieved data
    std::string response;   // the response from the requested data
    float newPres;          // the new pressure to apply to the hydraulic system

    Log(LOG_FN_ENTRY, "Motor Controller Torque()\n");
    LogInfo();

    // if the rollers are too slow, drive them
    float currentTorque = atof(ReadSubscribeData(COMMAND_TORQUE).c_str());
    if ((currentTorque > 0) &&
        (ReadSubscribeData(MACHINE_CLUTCH) == "1") &&
        ((ReadSubscribeData(GetDataTag("VehiclePresence")) == "1") ||
         (ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")) == "1")))
    {   // new pressure = boost pressue + ((the boost pressure - current psi)/10)
        newPres = m_boostPressure + ((m_boostPressure - GetBoostPressure()) / 10);
        Log(LOG_DEV_DATA, "New Boost Pressure: %f\n", newPres);
        // range check the new boost pressure
        if (newPres > m_currentMaximumBoostPressure)    // if greater than max
            newPres = m_currentMaximumBoostPressure;        // set to max
        else if (newPres < m_minimumBoostPressure)  // if less than min
            newPres = m_minimumBoostPressure;       // set to min
    }
    else        // else set the pressure to the minimum pressure
        newPres = 0;

    // set the boost pressure
    status = SetBoostPressure(newPres);
    if (status != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "Speed Could Not Command New Pressure: %s\n", value.c_str());
    }
    else
    {   // Display boost status on panel (marquee?)
        if (newPres) status = SetBoostStatus("On");
        else        status = SetBoostStatus("Off");

        if (status != EOK)
            Log(LOG_ERRORS, "Speed Could Not Command Boost Light: %s\n", value.c_str());
    }
    return(status);
}
const INT32 HydraulicMotorController::LossCompensation(void)
{   // The Hydraulic Motor Controller must boost the LossCompensationPressure
    return(Speed());
}
const INT32 HydraulicMotorController::QuickBrake(void)
{
    INT32 status;           // status to use for operations in boost
    std::string tag;        // tag for retrieving data
    std::string value;      // the value of the retrieved data
    std::string response;   // the response from the requested data
    float newPres=0;        // the new pressure to apply to the hydraulic system

    Log(LOG_FN_ENTRY, "Motor Controller QuickBrake()\n");

    // set the boost pressure
    status = SetBoostPressure(newPres);
    if (status != BEP_STATUS_SUCCESS)
        Log(LOG_ERRORS, "Speed Could Not Quick Brake: %d\n", status);
    else
    {   // Display boost status on panel (marquee?)
        if (newPres) status = SetBoostStatus("On");
        else        status = SetBoostStatus("Off");

        if (status != EOK)
            Log(LOG_ERRORS, "Speed Could Not Command Boost Light: %s\n", value.c_str());
    }
    return(status);
}
INT32 HydraulicMotorController::SetBoostPressure(float pressure)
{
    INT32 status = BEP_STATUS_SUCCESS;
    Log(LOG_FN_ENTRY, "Setting Boost Pressure: %f\n", pressure);
    if (m_previousPressure != pressure)
    {
        m_previousPressure = pressure;

        /*
        // convert the new pressure to counts to send to AOUT
        float newPres = pressure * m_psiConversion;
        // set the boost pressure
        char buffer[10];
        sprintf(buffer, "%4.4f", newPres);
        */

        // set the boost pressure
        char buffer[10];
        sprintf(buffer, "%d", (short)pressure);

        Log(LOG_DEV_DATA, "Setting Boost Pressure: %s\n", buffer);
        std::string value = buffer;
        std::string response = "";

        if (ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")) == "0")
        {
            // write "normal" boost pressure
            status = m_dataComm->Write(GetDataTag("BoostPressureOut"), value, response, true);
        }
        else                                  // loss compensation switch is on
        {
            // write loss compensation boost pressure
            status = m_dataComm->Write(GetDataTag("LossCompensationPressure"), value, response, true);
        }
    }


    return(status);
}
float HydraulicMotorController::GetBoostPressure(void)
{
    INT32 status;
    std::string value, response;
    float currentPSI=0;

    Log(LOG_FN_ENTRY, "HydraulicMotorController::GetBoostPressure\n");
// test to prove if there is a memory leak in the NDB Read() method
    status = m_dataComm->Read(GetDataTag("BoostPressure"), response, true);
    if (status == BEP_STATUS_SUCCESS)
    {   // get the response
//      status = m_dataComm->GetByTag(GetDataTag("BoostPressure"), value, response);
        // test to determine if GetByTag might introduce a leak
        std::string tag;
        status = m_dataComm->GetNext(tag, value, response);

        if (status == BEP_STATUS_SUCCESS)
        {
            /*
            // convert voltage to psi
            if(m_psiConversion > 0) currentPSI = atof(value.c_str()) / m_psiConversion;
            else                    throw BepException("ERROR PSIConversion: %f", m_psiConversion);
            */
            currentPSI = atof(value.c_str());
            Log(LOG_DEV_DATA, "Current PSI: %f\n", currentPSI);
        }
        else
            Log(LOG_ERRORS, "Error Geting Tag For BoostPressure\n");
    }
    else
        Log(LOG_ERRORS, "Error Reading BoostPressure\n");
    return(currentPSI);
}
void HydraulicMotorController::Reset(void)
{
    MotorController::Reset();
    BepComponent::Initialize(GetConfigFile());
}
const std::string HydraulicMotorController::Read(const XmlNode *node, const INT32 rate) /* = 0 */
{
    std::string value;
    if (rate == 0) Log(LOG_FN_ENTRY, "HydraulicMotorController::Read(%s, %d)\n", node->ToString().c_str(), rate);

    value = BepServer::Read(node, rate);

    if (rate == 0) Log(LOG_FN_ENTRY, "HydraulicMotorController::Read(%s, %d) returned %s done\n", node->ToString().c_str(), rate, value.c_str());

    return(value);
}
void HydraulicMotorController::SetMaximumBoostPressure(float pressure)
{
    if ((pressure > m_absMaximumBoostPressure) || (pressure < m_minimumBoostPressure))
    {
        Log(LOG_ERRORS, "ERROR, SetMaximumBoostPressure() - "
            "Maximum boost pressure out of limits:\n\tMaxLimit = %f, MinLimit = %f, SetValue = %f\n",
            m_absMaximumBoostPressure,m_minimumBoostPressure,pressure);
        Log(LOG_ERRORS, "ERROR, SetMaximumBoostPressure() - "
            "Leaving Maximum boost pressure at previous value of %f\n", m_currentMaximumBoostPressure);
    }
    else
    {
        Log(LOG_DEV_DATA, "SetMaximumBoostPressure() - Setting Max Boost Pressure from %f, to %f\n",
            m_currentMaximumBoostPressure,pressure);
        m_currentMaximumBoostPressure = pressure;
    }
}
float HydraulicMotorController::GetMaximumBoostPressure(void)
{
    return(m_currentMaximumBoostPressure);
}
void HydraulicMotorController::LogInfo(void)
{
    Log(LOG_DEV_DATA,
        "rfMPH: %f, rrMph: %f\nm_minimumSpeedForBoost: %f\nDriveAxle: %s\nZerospeed: %s\nClutch: %s\nRollsUpElevatorDown: %s\nLossCompensationSwitchStatus: %s\nVehiclePresent: %s\n",
        m_speeds[RightFront], m_speeds[RightRear], m_minimumSpeedForBoost, ReadSubscribeData(GetDataTag("DriveWheel")).c_str(),
        ReadSubscribeData(GetDataTag("Zerospeed")).c_str(), ReadSubscribeData(MACHINE_CLUTCH).c_str(),
        ReadSubscribeData(GetDataTag("RollsUpElevatorDown")).c_str(), ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")).c_str(),
        ReadSubscribeData(GetDataTag("VehiclePresence")).c_str());
}
#if CONTROLLOOP
float HydraulicMotorController::MyPid(int reset, float desired, float actual,
                                      float prop, float integ, float min, float max)
{
    static float    ErrSum;
    float           error, newCmd;

    if (reset)
        ErrSum=0;

    // current error
    error = desired - actual;

    // long term error
    ErrSum += error * integ;

    // limit long term error
    if (ErrSum < min) ErrSum = min;
    if (ErrSum > max) ErrSum = max;

    // calculate the PI for new fan speed
    newCmd = error * prop + ErrSum;

    // range check
    if (newCmd < min) newCmd = min;
    if (newCmd > max) newCmd = max;

    return(newCmd);
}
#endif

