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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Common/MotorController.cpp $
// 
// 4     10/30/07 4:14p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added m_defaultDriveAxle.
// - Added reading of the default drive axle from the configuration file.
// - Use the default drive axle to report the speed in
// UpdateMachineStatus() if no drive axle is specified.
// 
// 3     4/13/06 4:19a Cward
//
// 2     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/16/06 11:33a Rwiersem
// Added methods for loading additional configuration items.  This is used
// to reload the configuration without restarting the process.
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
// 7     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 6     3/11/05 3:38p Cward
// Fixed warning messages related to out of order initializers.
//
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 4     2/18/05 12:54p Bmeinke
// Added Dan E's changes to open the ICM binary files only once
// Re-arranged initializers in the constructor
// Added "dummy" handler statements in the Command() method for
// "SpeedValue" and "TorqueValue" to remove the pesky "Invalid command
// received" messages from the log files
//
// 3     12/22/04 8:25p Bmeinke
// Added a new "DisableSpeedMatching" parameter to allow us to totally
// turn off all speed matching if needed (like with the U152/Explorer that
// is AWD, but not reported as AWD by eCATS)
//
// 2     12/14/04 8:58p Bmeinke
// Removed the #if(USE_BEP_MUTEX) blocks
//
// 4     8/30/04 3:42p Bmeinke
// Added a Publish() method to set the motor loading constants back to
// their default values when we lose retainers up
// Added SetDefaultLoading() method to reset motor loading constants to
// their default values
//
// 3     7/23/04 3:14p Bmeinke
// Moved the motor loading parameters to the MotorController class
// Moved the motor loading calculations to the
// MotorController::CalculateTorqueLoad() method
// Added a pure virtual ApplyMotorLoad() method for motor loading
//
// 4     6/09/04 11:45p Cward
// Updated the header comment block to the new standard
//
// 2     7/08/04 10:51a Bmeinke
// If CheckStatus() fails in MainLoop(), Log an error to say why it failed
//
// 3     5/25/04 4:27p Cward
// Added parameter to specify whether to clutch at zero speed.
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:41p Cward
//
// 3     10/24/03 1:44p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 2     10/10/03 9:58a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:30a Khykin
// Initial checkin
//
// 2     8/11/03 4:55p Khykin
// Updating with code from Tims Laptop.
//
// 25    11/19/02 9:24p Tbochene
// Moved start of the heartbeat thread to Register(), Updated methods that
// read data from ICM to use GetBinaryData(), and implimented use of
// SetCommandedSpeed() with float paramter
//
// 24    11/08/02 12:25p Tbochene
// Convert to use arrays instead of descrete variables for better
// extensability.  Added GetAccelerations().  Removed rpm variables since
// it is derived from speed (calculate as needed since it is needed far
// less than speed)
//
// 23    11/04/02 6:54a Khykin
// Updated to read the current roller speeds from the ICM directly.
//
// 22    10/15/02 6:08a Khykin
// Updated the Clutch method to use its own copy of the
// INamedDataBroker and to not use the GetByTag() method. At
// this point, this method is dangerous and should only be
// used with full knowledge of how it works.
//
// 21    9/16/02 11:14a Khykin
// Updating for changes to the SerialMotor controller.
//
// 20    9/10/02 3:36p Khykin
// Updating for Tim to take to CTC.
//
// 19    9/06/02 10:42a Khykin
// Updating the initialization method parameter format.
//
// 18    8/23/02 8:39a Kh
// Synchronizing the pit machine with VSS.
//
// 16    5/24/02 3:08p Kh
// Adding code to deal with front an rear wheel vehicles.
//
// 15    4/29/02 3:33p Kh
// Updating due to changes for the serial motor contoller.
//
// 14    4/12/02 2:33p Kh
// Updating with changes due to the new SerialMotorController.
//
// 13    4/03/02 1:26p Kh
// Demoting the CalculateRPM method because only used by serial motors.
//
// 12    3/19/02 12:44p Kh
// Updating due to changes with BepComponent.
//
// 11    3/07/02 6:51p Skypig13
// Update to new BepServer structure
//
// 10    3/06/02 2:12p Kh
// Updating for mode changes.
//
// 9     1/07/02 4:58p Kh
// Adding an IDLE_MODE. Updating for changes to the BepServer.
//
// 8     12/27/01 4:25p Kh
// Adding a Loss Compensation mode to handle the differences between electric and
// hydraulic machines.
//
// 7     12/21/01 1:29p Kh
// Changing the ControlLoop method to the Run method.
//
// 6     12/20/01 2:32p Kh
// Component Tested.
//
// 5     11/26/01 1:08p Kh
// Moving the constructor and destructor.
//
// 4     10/30/01 2:52p Kh
// All Tested.  Looks good.
//
// 3     10/25/01 10:02a Kh
// Checking in the latest code (debug not complete)
//
// 2     10/09/01 3:22p Kh
// All debugged, just waiting for the resource managers to be completed to
// test.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#define HEARTBEAT_THREAD_ENABLE 0

//-------------------------------------------------------------------------------------------------
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "MotorController.h"
#include "MotorControllerDefs.h"

//-------------------------------------------------------------------------------------------------
const std::string MotorController::Forward("1");
const std::string MotorController::Reverse("0");
const std::string MotorController::On("1");
const std::string MotorController::Off("0");
const std::string MotorController::Enabled("1");
const std::string MotorController::Disabled("0");
const std::string MotorController::Faulted("2");
const std::string MotorController::True("1");
const std::string MotorController::False("0");

//-------------------------------------------------------------------------------------------------
std::vector<std::string> MotorController::MotorDescriptions;

//-------------------------------------------------------------------------------------------------
MotorController::MotorController(void) : BepServer(), m_dataComm(NULL),
m_commandComm(NULL), m_faultComm(NULL), m_brakeDetectLimit(-3.0),
m_brakeClearLimit(1.0), m_speedDeltaForBoost(2.0), m_updateRate(100),
    m_minimumSpeedForBoost(2.0), m_defaultAccelConst( 0.0), m_accelerationLoadingConstant( 0.0),
m_defaultSpeedConst( 0.0), m_speedLoadingConstant( 0.0),
m_defaultTorqueConst( 0.0), m_torqueLoadingConstant( 0.0), m_motorLoadingEnabled( true),
    m_disableSpeedMatching( false), m_speedFile(-1), m_accelFile(-1),
m_clutchOnZeroSpeed(true), m_heartBeatThread(NULL),m_defaultDriveAxle("Rear")
{   // Set up descriptions
    MotorDescriptions.push_back("LeftFront");
    MotorDescriptions.push_back("RightFront");
    MotorDescriptions.push_back("LeftRear");
    MotorDescriptions.push_back("RightRear");
    MotorDescriptions.push_back("LeftTandem");
    MotorDescriptions.push_back("RightTandem");

    for(UINT8 ii = 0; ii < 4; ii++)
    {
        m_speeds.push_back(0.0);            // Fill speed container
        m_accelerations.push_back(0.0); // Fill acceperation container
    }
}

//-------------------------------------------------------------------------------------------------
MotorController::~MotorController(void)
{
    Log(LOG_FN_ENTRY, "MotorController::~MotorController()\n");
    if(m_dataComm)
    {
        delete m_dataComm; m_dataComm = NULL;
    }
    if(m_commandComm)
    {
        delete m_commandComm; m_commandComm = NULL;
    }
    if(m_faultComm)
    {
        delete m_faultComm; m_faultComm = NULL;
    }
    if(m_heartBeatThread)
    {
        m_heartBeatThread->SetStatus(BEP_TERMINATE_STATUS);
    }

    // Close the data files that may have been opened
    CloseBinaryDataFile(m_speedFile);
    CloseBinaryDataFile(m_accelFile);
    Log(LOG_FN_ENTRY, "MotorController::~MotorController() done\n");
}

//-------------------------------------------------------------------------------------------------
void MotorController::Initialize(const std::string &fileName)
{
    try
    {
        Log(LOG_FN_ENTRY, "MotorController::Initialize(%s)\n", fileName.c_str());
        SetConfigFile(fileName);       // Set name of configuration file
        const XmlNode* document = m_parser.ReturnXMLDocument(fileName)->Copy();    // Parse configuration file
        if(document)
        {
            Initialize(document);      // initialize server
        }
        else Log(LOG_ERRORS, "MotorController::Initialize() Failed to parse configuration file");
    }
    catch(BepException& BepErr)
    {
        Log(LOG_ERRORS, "MotorController::Initialize(%s) failed: %s", fileName.c_str(), BepErr.what());
        throw;
    }
    catch(...)
    {
        Log(LOG_ERRORS, "MotorController::Initialize() failed: for an unknown reason");
        throw;
    }
    Log(LOG_FN_ENTRY, "MotorController::Initialize(%s) done\n", fileName.c_str());
}

//-------------------------------------------------------------------------------------------------
void MotorController::Initialize(const XmlNode *document)
{
    if(document != NULL)
    {
        BepServer::Initialize(document);
        Log(LOG_FN_ENTRY, "MotorController::Initialize()\n");

        // load the additional configuration items needed by the motor controller
        // we need to scope this call to make sure the base class method is
        // called
        MotorController::LoadAdditionalConfigurationItems(document);

        // Create heartbeat thread if it does not exist
        if(!m_heartBeatThread)
        {
            m_heartBeatThread = new HeartBeatThread(document, this);
        }
    }
    else
    {
        Log(LOG_ERRORS, "Invalid Configuration File Provided");
        throw BepException("Invalid Configuration File Provided");
    }
    SetStatus(BEP_INITIALIZED_STATUS);  // set the status to initialized
    Log(LOG_FN_ENTRY, "MotorController::Initialize() done\n");
}

//-------------------------------------------------------------------------------------------------
void MotorController::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    const XmlNode* setup;

        try
        {
        setup = document->getChild("Setup");
            const XmlNode *motorParameters = setup->getChild("MotorParameters");
            m_brakeDetectLimit = atof(motorParameters->getChild("BrakeDetectLimit")->getValue().c_str());
            m_brakeClearLimit = atof(motorParameters->getChild("BrakeClearLimit")->getValue().c_str());
            m_speedDeltaForBoost = atof(motorParameters->getChild("SpeedDeltaForBoost")->getValue().c_str());
            m_reverseBoost = (BposReadInt(motorParameters->getChild("ReverseBoost")->getValue().c_str()) == 1);
            m_updateRate = BposReadInt(motorParameters->getChild("UpdateInterval")->getValue().c_str());
            m_minimumSpeedForBoost = atof(motorParameters->getChild("MinimumSpeedForBoost")->getValue().c_str());

        SetupMotorLoading(motorParameters);

            try
            {   // get the array for reading the speed values
                m_speedArray = motorParameters->getChild("CurrentSpeeds")->getValue().c_str();
            }
            catch(...)
            {
                m_speedArray = "/dev/Icm/Block/Speed";
            }
            try
        {   // get the array for reading the acceleration values
                m_accelerationArray = motorParameters->getChild("CurrentAccelerations")->getValue().c_str();
            }
            catch(...)
            {
                m_accelerationArray = "/dev/Icm/Block/Acceleration";
            }

            try
            {
                m_disableSpeedMatching = motorParameters->getChild("DisableSpeedMatching")->getValue() == "1";
            }
            catch( ...)
            {
                m_disableSpeedMatching = false;
            }
    }
        catch(BepException &Err)
        {
            Log(LOG_ERRORS, "MotorController::Initialize Error: %s\n", Err.what());
            throw;
        }

        try
        {   // set up the node for the data tags
            m_dataTags = (XmlNodeMap)setup->getChild("DataTags")->getChildren();
        }
        catch(BepException &Err)
        {
            Log(LOG_ERRORS, "MotorController::m_dataTags::Initialize Error: %s\n", Err.what());
            throw;
        }
        try
    {   // set up the node for the fault tags
            m_faults = (XmlNodeMap)setup->getChild("Faults")->getChildren();
        }
        catch(BepException &Err)
        {
            Log(LOG_ERRORS, "MotorController::m_faults::Initialize Error: %s\n", Err.what());
            throw;
        }
        // Determine if the rollers should clutch automatically when zero speed is obtained
        try
        {
            SetClutchOnZeroSpeed(!setup->getChild("ClutchOnZeroSpeed")->getValue().compare("Yes"));
        }
        catch(BepException &Err)
        {
            // Default to always clutch on zero speed in the event it is not specified
            Log(LOG_ERRORS, "MotorController::Initialize() - clutch determination error - %s", Err.GetReason());
            SetClutchOnZeroSpeed(true);
        }

    try
    {
        m_defaultDriveAxle = setup->getChild("DefaultDriveAxle")->getValue().c_str();
    }
    catch(BepException &Err)
    {
        // default to the rear axle
        m_defaultDriveAxle = "Rear";
    }
}

//-------------------------------------------------------------------------------------------------
void MotorController::SetupMotorLoading(const XmlNode *motorParameters)
{
    try
    {
        m_defaultAccelConst = atof(motorParameters->getChild("AccelerationLoading")->getValue().c_str());
    }
    catch( ...)
    {
        m_defaultAccelConst = 0.0;
    }
    m_accelerationLoadingConstant = m_defaultAccelConst;
    try
    {
        m_defaultSpeedConst = atof(motorParameters->getChild("SpeedLoading")->getValue().c_str());
    }
    catch( ...)
    {
        m_defaultSpeedConst = 0.0;
    }
    m_speedLoadingConstant = m_defaultSpeedConst;
    try
    {
        m_defaultTorqueConst = atof(motorParameters->getChild("TorqueLoading")->getValue().c_str());
    }
    catch( ...)
    {
        m_defaultTorqueConst = 0.0;
    }
    m_torqueLoadingConstant = m_defaultTorqueConst;
    try
    {
        m_minSpeedForLoading = atof(motorParameters->getChild("MotorLoadingMinSpeed")->getValue().c_str());
    }
    catch( ...)
    {
        m_minSpeedForLoading = 2.0;
    }
    try
    {
        m_maxSpeedForLoading = atof(motorParameters->getChild("MotorLoadingMaxSpeed")->getValue().c_str());
    }
    catch( ...)
    {
        m_maxSpeedForLoading = 0.0;
    }
}

//-------------------------------------------------------------------------------------------------
const std::string MotorController::Register(void)
{
    std::string status;
    Log(LOG_FN_ENTRY, "MotorController::Register()\n");

    if((status = BepServer::Register()) == BEP_SUCCESS_RESPONSE)
    {   // If successful, complete initialization
        try
        {   // set up the main controller comm
            if(!m_dataComm)     m_dataComm = new INamedDataBroker(IsDebugOn());
            // set up clutch comm
            if(!m_commandComm)  m_commandComm = new INamedDataBroker(IsDebugOn());
            // set up the fault server comm
            if(!m_faultComm)
            {
                m_faultComm = new IFaultServer();
                m_faultComm->Initialize(FAULT_SERVER_NAME);
            }

            if(m_dataComm)
            {
                std::string message, tag, value;

                m_dataComm->Read(GetDataTag("RollCount"), message, true);
                m_dataComm->GetNext(tag, value, message);
                SetData(tag, value);
            }

            UINT32 tempMotorNum = BposReadInt(GetData(GetDataTag("RollCount")).c_str());

            if(tempMotorNum > m_speeds.size())
            {
                const UINT32 change = tempMotorNum - m_speeds.size();

                for(UINT8 ii = 0; ii < change; ii++)
                {   // Resize if too small
                    m_speeds.push_back(0.0);        // Fill speed container
                    m_accelerations.push_back(0.0); // Fill acceperation container
                }
            }

            if(m_heartBeatThread) m_heartBeatThread->Resume();  // Launch heartbeat thread
            Log(LOG_DEV_DATA, "MotorController::Register() done\n");
        }
        catch(BepException &Err)
        {
            Log(LOG_ERRORS, "Error Initializing the MotorController: %s\n", Err.what());
            status = BEP_TERMINATE_STATUS;    // else mark terminate
            SetStatus(status);
        }
        Log(LOG_FN_ENTRY, "MotorController::Register() done: %s\n", status.c_str());
    }

    return(status);
}

//-------------------------------------------------------------------------------------------------
void MotorController::Run(volatile bool *terminateFlag /*=NULL*/)
{
    Log(LOG_FN_ENTRY, "MotorController::Run()\n");
    while(GetStatus() != BEP_TERMINATE_STATUS)
    {   // if we are ready do run
        if(GetStatus() == BEP_RUNNING_STATUS)
        {
            MainLoop();
        }
        BposSleep(GetUpdateRate());    // wait for next update
    }
    Log(LOG_FN_ENTRY, "MotorController::Run() done\n");
}

//-------------------------------------------------------------------------------------------------
void MotorController::MainLoop(void)
{   // Call for foreground loop processing
    Log(LOG_FN_ENTRY, "MotorController::MainLoop()\n");
    UpdateMachineStatus(); // get the current machine status

    INT32 status;
    std::string response;

    if((status=CheckStatus()) == BEP_STATUS_SUCCESS)
    {
        std::string mode = GetMotorMode();
        Log(LOG_DEV_DATA, "MotorMode: %s\n", mode.c_str());
        if(mode == BOOST_MODE)                      status = Boost();
        else if(mode == QUICK_BRAKE_MODE)           status = QuickBrake();
        else if(mode == REAR_BRAKE_ASSIST_MODE)     status = RearBrakeAssist();
        else if(mode == SPEED_MODE)                 status = Speed();
        else if(mode == TORQUE_MODE)                status = Torque();
        else if(mode == NO_LIMIT_TORQUE_MODE)       status = Torque();
        else if(mode == LOSSCOMPENSATION_MODE)      status = LossCompensation();
        else if(mode == UPLOAD_PARAMETERS_MODE)     status = UploadDriveParameters();
        else if(mode == INDIVIDUAL_CONTROL_MODE)    status = IndividualControlMode();
        else
        {
            if((status = SpecialMode()) == EOK) Log(LOG_ERRORS, "Unexpected Command: %s\n", mode.c_str());
        }
    }
    else
    {
        Log( LOG_DEV_DATA, "CheckStatus returned %d...skipping mode selection\n", status);
        status = EOK;
    }

    // if the heartbeat thread was disabled
    if(!m_heartBeatThread)
    {
        // Update the motor controller task heartbeat
        if(ReadSubscribeData(GetDataTag("MotorControllerTaskHeartbeat")) == "0")
        {
            m_commandComm->Write(GetDataTag("MotorControllerTaskHeartbeat"),"1", response, true);
        }
        else m_commandComm->Write(GetDataTag("MotorControllerTaskHeartbeat"),"0", response, true);
    }

    Log(LOG_FN_ENTRY, "MotorController::MainLoop() done status: %s\n", strerror(status));
}

//-------------------------------------------------------------------------------------------------
const std::string MotorController::Read(const XmlNode *node, const INT32 rate) /*= 0*/
{
    std::string value;
    if(rate == 0) Log(LOG_FN_ENTRY, "MotorController::Read(%s, %d)\n", node->ToString().c_str(), rate);

    value = BepServer::Read(node, rate);

    if(rate == 0) Log(LOG_FN_ENTRY, "MotorController::Read(%s, %d) returned %s done\n", node->ToString().c_str(), rate, value.c_str());
    return(value);
}

//-------------------------------------------------------------------------------------------------
const std::string MotorController::Command(const XmlNode *node)
{
    Log(LOG_FN_ENTRY, "MotorController::Command(%s)\n", node->ToString().c_str());
    std::string status(BEP_INVALID_RESPONSE);

    const std::string name = node->getName(), value = node->getValue();
    if(name == MOTOR_MODE)
    {
        Log(LOG_DEV_DATA, "Commanded to %s Mode\n", value.c_str());
        if((value == BOOST_MODE) || (value == QUICK_BRAKE_MODE) || (value == REAR_BRAKE_ASSIST_MODE) ||
           (value == SPEED_MODE) || (value == TORQUE_MODE) || (value == LOSSCOMPENSATION_MODE) ||
           (value == UPLOAD_PARAMETERS_MODE) || (value == INDIVIDUAL_CONTROL_MODE))
        {
            SetMotorMode(value);        // Set the motor mode
            SetData(name, value);

            status = BEP_SUCCESS_RESPONSE;  // It's all good
        }
        else Log(LOG_ERRORS, "Invalid Motor Mode Received: %s\n", value.c_str());
    }
    else if(name == COMMAND_SPEED)
    {
        Log(LOG_DEV_DATA, "Commanded to Speed %s\n", value.c_str());
        SetData(node);
        status = BEP_SUCCESS_RESPONSE;
    }
    else if(name == COMMAND_TORQUE)
    {
        Log(LOG_DEV_DATA, "Commanded to Torque %s\n", value.c_str());
        SetData(node);
        status = BEP_SUCCESS_RESPONSE;
    }
    else if(name == COMMAND_CLUTCH)
    {
        Log(LOG_DEV_DATA, "Commanded to Clutch %s\n", value.c_str());
        status = (Clutch(value) == EOK) ? BEP_SUCCESS_RESPONSE : BEP_FAILURE_RESPONSE;
    }
    else if( name == COMMAND_MOTOR_LOADING)
    {
        if( (value == "1") || (value == "On") || (value == "Enabled"))  EnableMotorLoading( true);
        else                                                            EnableMotorLoading( false);
        status = BEP_SUCCESS_RESPONSE;
    }
    else if( name == MOTOR_LOADING_ACCEL_K)
    {
        if( value.empty() == true)  m_accelerationLoadingConstant = 0.0;
        else                        m_accelerationLoadingConstant = fabs( atof( value.c_str()));
        status = BEP_SUCCESS_RESPONSE;
    }
    else if( name == MOTOR_LOADING_SPEED_K)
    {
        if( value.empty() == true)  m_speedLoadingConstant = 0.0;
        else                        m_speedLoadingConstant = fabs( atof( value.c_str()));
        status = BEP_SUCCESS_RESPONSE;
    }
    else if( name == MOTOR_LOADING_TORQUE_K)
    {
        Log(LOG_DEV_DATA, "Motor loading torque K %s\n", value.c_str());
        if( value.empty() == true)  m_torqueLoadingConstant = 0.0;
        else                        m_torqueLoadingConstant = fabs( atof( value.c_str()));
        status = BEP_SUCCESS_RESPONSE;
    }
    else if( (name == "SpeedValue") || (name == "TorqueValue"))
    {
        // This is here only to eliminate the "Invalid  command: SpeedValue" annoying log
        //  message
        status = BepServer::Write(node);
    }
    else Log(LOG_ERRORS, "Invalid Command: %s\n", name.c_str());

    Log(LOG_FN_ENTRY, "MotorController::Command(%s) [status = %s] done\n", node->ToString().c_str(), status.c_str());

    return(status);
}

//-------------------------------------------------------------------------------------------------
const std::string MotorController::Write(const XmlNode *node)
{
    std::string response = Command(node);
    if(response == BEP_INVALID_RESPONSE)
        response = BepServer::Write(node);
    return(response);
}

//-------------------------------------------------------------------------------------------------
void MotorController::Terminate(void)
{
    Log(LOG_FN_ENTRY, "MotorController::Terminate()\n");
    BepServer::Terminate();
    Log(LOG_FN_ENTRY, "MotorController::Terminate() done\n");
}

//-------------------------------------------------------------------------------------------------
const std::string MotorController::Publish(const XmlNode *node)
{
    std::string status;

	status = BepServer::Publish(node);

    Log(LOG_FN_ENTRY, "Enter MotorController::Publish(%s)\n", node->ToString().c_str());

    // Rolls were Up, now they are going down
    if((node->getName() == GetDataTag("RollsUpElevDown")) && (node->getValue() == False))
    {
        // Reset the motor loading constants to their default values
        SetDefaultLoading();

        // Reset the vechile axle count to 2 (4 wheels)
        SetData( GetDataTag( "VehicleAxleCount"), "2");
    }
	else if(!node->getName().compare(ROLLS_UP_DATA_TAG) && !node->getValue().compare("1"))
	{   // Clutch when rolls are raised if necessary
		Clutch(Enabled);
	}
	else if(!node->getName().compare(ZEROSPEED_TAG) && !node->getValue().compare("1"))
	{   // Clutch when zero speed is achieved if necessary
		if (ClutchOnZeroSpeed() &&
            (ReadSubscribeData(GetDataTag("RollsUpElevatorDown")) == True) &&
            (ReadSubscribeData(MACHINE_CLUTCH) == Disabled))
		{
			Clutch(Enabled);
		}
	}

    Log(LOG_FN_ENTRY, "Exit MotorController::Publish(%s)\n", node->ToString().c_str());

    return( status);
}

//-------------------------------------------------------------------------------------------------
const std::string MotorController::GetSpecialCommand(void)
{
    return(m_specialCommand);
}

//-------------------------------------------------------------------------------------------------
const std::string MotorController::GetMotorMode(const INT8 roll) /*= All*/
{
    XmlNode motorModeNode(MOTOR_MODE,"");
    return(Read(&motorModeNode));
}

//-------------------------------------------------------------------------------------------------
void MotorController::SetMotorMode(const std::string &motorMode, const INT8 roll) /*= All*/
{
    XmlNode motorModeNode(MOTOR_MODE, motorMode);
    BepServer::Write(&motorModeNode);
}

//-------------------------------------------------------------------------------------------------
void MotorController::SetDefaultLoading()
{
    m_accelerationLoadingConstant = m_defaultAccelConst;
    m_speedLoadingConstant  = m_defaultSpeedConst;
    m_torqueLoadingConstant = m_defaultTorqueConst;
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::QuickBrake(void)
{
    Log(LOG_ERRORS, "MotorController::QuickBrake() not implemented\n");
    SetMotorMode(BOOST_MODE);
    return(ENOTSUP);
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::RearBrakeAssist(void)
{
    Log(LOG_ERRORS, "MotorController::RearBrakeAssist() not implemented\n");
    SetMotorMode(BOOST_MODE);
    return(ENOTSUP);
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::UploadDriveParameters(void)
{
    Log(LOG_ERRORS, "MotorController::UploadDriveParameters() not implemented\n");
    SetMotorMode(BOOST_MODE);
    return(ENOTSUP);
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::IndividualControlMode(void)
{
    Log(LOG_ERRORS, "MotorController::IndividualControlMode() not implemented\n");
    SetMotorMode(BOOST_MODE);
    return(ENOTSUP);
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::SpecialMode(void)
{
    Log(LOG_ERRORS, "MotorController::SpecialMode() no special modes implemented\n");
    return(ENOTSUP);
}

//-------------------------------------------------------------------------------------------------
void MotorController::UpdateMachineStatus(void)
{
    Log(LOG_FN_ENTRY, "MotorController::UpdateMachineStatus()\n");

#if 0
    if (ClutchOnZeroSpeed())
    {
        if(ReadSubscribeData(GetDataTag("RollsUpElevatorDown")) == True)
        {
            if(ReadSubscribeData(MACHINE_CLUTCH) == Disabled)
            {
                Clutch(Enabled);
            }
        }
    }
#endif 
    if(GetSpeeds() == BEP_STATUS_SUCCESS)
    {
        float speed;
        char speedValue[16];

        // Determine which speed to send to the GUI
        if(ReadSubscribeData(GetDataTag("DriveWheel")) == "Front")
        {
            speed = m_speeds[LeftFront];
        }
        else if(ReadSubscribeData(GetDataTag("DriveWheel")) == "Rear")
        {
            speed = m_speeds[LeftRear];
        }
        else if(m_defaultDriveAxle == "Front")
        {
            speed = m_speeds[LeftFront];
        }
        else
        {
            speed = m_speeds[LeftRear];
        }

        // Update the wheel speeds to the GUI
        CreateMessage(speedValue, sizeof(speedValue), "%3.2f", fabs(speed));

        const XmlNode node(GetDataTag("SpeedValue"), std::string(speedValue));
        Write(&node);
        Log(LOG_DEV_DATA, "The Speed Value: %s\n", speedValue);
    }
    else Log(LOG_ERRORS, "MotorController Could Not UpdateMachineStatus because: %s\n", strerror(errno));

    Log(LOG_FN_ENTRY, "MotorController::UpdateMachineStatus() done %s\n", CreateStringFromFloat(m_speeds, "Speeds: ").c_str());
}

//-------------------------------------------------------------------------------------------------
float MotorController::CalculateTorqueLoad( int masterRoll)
{
    float       trqTotal = 0.0;
    float       trqAccel = 0.0;
    float       trqSpeed = 0.0;
    bool        inTrqSpeedBand = false;

    Log(LOG_FN_ENTRY, "MotorController::CalculateTorqueLoad - Enter (%d); accelK=%.2F spdK=%.2f enabled=%d\n", masterRoll, m_accelerationLoadingConstant, m_speedLoadingConstant, m_motorLoadingEnabled);

    // determine the vehicle drive axle
    if( masterRoll != All)
    {
        // Check to see if our speed is within the defined torque loading speed band parameters
        if( (m_speeds[ masterRoll] > m_minSpeedForLoading) && 
			((m_speeds[ masterRoll] < m_maxSpeedForLoading) || (m_maxSpeedForLoading <= 0)) )
        {
            inTrqSpeedBand = true;
        }

        // check the speed to determine if motor loading should be applied
        if( ((m_accelerationLoadingConstant != 0) || (m_speedLoadingConstant != 0) || (m_torqueLoadingConstant != 0)) && 
			( inTrqSpeedBand == true) && ( m_motorLoadingEnabled == true))
        {
            // calculate the torque to apply based on delta acceration
            Log(LOG_DEV_DATA, "Calculating torque loading\n");
            trqAccel = m_accelerations[masterRoll] * m_accelerationLoadingConstant;
            // calculate the % torque necessary for windage simulation
            if(m_speedLoadingConstant > 0.0)
            {
                trqSpeed = ((m_speeds[masterRoll] * m_speeds[masterRoll]) * m_speedLoadingConstant) / 2500;
            }
            // sum the % torques for overall % torque
            trqTotal = trqAccel + trqSpeed + m_torqueLoadingConstant;

            //TO GIVE
            Log(LOG_DEV_DATA, "Calculated torque load of %.2f\n", trqTotal);
        }
        else if( inTrqSpeedBand == false)
        {
            Log( LOG_DEV_DATA, "Out of torque loading speed range\n");
            trqTotal = 0;
        }
    }

    Log(LOG_FN_ENTRY, "MotorController::CalculateTorqueLoad - Exit(%d) trqTotal=%.2f\n", masterRoll, trqTotal);

    return( trqTotal);
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::GetSpeeds(float *buff, int count)
{
    INT32 stat;
    Log(LOG_FN_ENTRY, "MotorController::GetSpeeds()\n");

    if( NULL == buff)
    {
    float temp[m_speeds.size()];
        stat = GetBinaryData(temp, sizeof(temp), m_speedArray, m_speedFile);

    for(UINT32 ii = 0; ii < m_speeds.size(); ii++) m_speeds[ii] = temp[ii];

    Log(LOG_DETAILED_DATA, "%s\n", CreateStringFromFloat(m_speeds, "Speeds: ").c_str());
    }
    else
    {
        stat = GetBinaryData(buff, count*sizeof( float), m_speedArray, m_speedFile);
    }
    Log(LOG_FN_ENTRY, "MotorController::GetSpeeds(stat: %d) done\n", stat);
    return(stat);
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::GetAccelerations(void)
{
    Log(LOG_FN_ENTRY, "MotorController::GetAccelerations()\n");

    float temp[m_accelerations.size()];
    const INT32 stat = GetBinaryData(temp, sizeof(temp), m_accelerationArray, m_accelFile);

    for(UINT32 ii = 0; ii < m_accelerations.size(); ii++) m_accelerations[ii] = temp[ii];

    Log(LOG_DETAILED_DATA, "%s\n", CreateStringFromFloat(m_accelerations, "Accelerations: ").c_str());
    Log(LOG_FN_ENTRY, "MotorController::GetAccelerations() done\n");
    return(stat);
}

//-------------------------------------------------------------------------------------------------
void MotorController::SetSpecialCommand(const std::string& command)
{
    m_specialCommand = command;
    Log(LOG_FN_ENTRY, "MotorController::SetSpecialCommand(%s)\n", m_specialCommand.c_str());
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::Clutch(std::string clutch)
{
    Log(LOG_FN_ENTRY, "MotorController::Clutch(%s)\n", clutch.c_str());
    INT32 status = EOK, stat;
    bool updateClutch = false;
    std::string clutchValue, declutchValue, response;

    if((status = m_clutchSync.Acquire()) == EOK)
    {
        Log(LOG_DEV_DATA, "Acquired clutch synchronization object\n");
        response = ReadSubscribeData(MACHINE_CLUTCH);     // Get clutch value
        Log(LOG_FN_ENTRY, "Clutch status = %s, Comamnd = %s\n", response.c_str(), clutch.c_str());

        if(response != BEP_UNAVAILABLE_RESPONSE)
        {   // Communication was ok
            if(clutch == True)
            {   // Command was to Clutch
                Log(LOG_FN_ENTRY, "Comamnd Clutch\n");
                if(response == Disabled)
                {   // Check the machine conditions
                    if((ReadSubscribeData(GetDataTag("RollsUpElevatorDown")) == True) &&
                       (ReadSubscribeData(GetDataTag("Zerospeed")) == True))
                    {   // Clutch the machine
                        clutchValue = Enabled;
                        declutchValue = Disabled;
                        Log(LOG_FN_ENTRY, "Enable clutch\n");
                    }
                    else
                    {   // Declutch machine
                        clutchValue = Disabled;
                        declutchValue = Enabled;
                        Log(LOG_ERRORS, "Invalid Conditions To Clutch: %s = %s, %s = %s\n",
                            GetDataTag("RollsUpElevatorDown").c_str(),
                            ReadSubscribeData(GetDataTag("RollsUpElevatorDown")).c_str(),
                            GetDataTag("Zerospeed").c_str(),
                            ReadSubscribeData(GetDataTag("Zerospeed")).c_str());
                    }
                    updateClutch = true;
                }
                else Log(LOG_FN_ENTRY, "Already clutched\n");
            }
            else if(clutch == False)
            {   // if not clutched, declutch
                if(response == Enabled)
                {
                    Log(LOG_FN_ENTRY, "Command Declutch\n");
                    clutchValue = Disabled;
                    declutchValue = Enabled;
                    updateClutch = true;
                }
                else Log(LOG_FN_ENTRY, "Already Declutched\n");
            }
            else Log(LOG_ERRORS, "Unknown Clutch Comamnd = %s\n", clutch.c_str());

            if(updateClutch)
            {   // update the system with Clutch Declutch values
                std::string tag, value;
                Log(LOG_FN_ENTRY, "Updating Clutch = %s, Declutch = %s\n", clutchValue.c_str(), declutchValue.c_str());
                if(m_commandComm)
                {
                    status = m_commandComm->Write(MACHINE_CLUTCH, clutchValue, response, false);
                    stat = m_commandComm->Write(MACHINE_DECLUTCH, declutchValue, response, true);

                    while(m_commandComm->GetNext(tag, value, response) > 0)
                    {
                        if((status != EOK) || (stat != EOK))
                        {   // if the send was not successful Log why
                            Log(LOG_ERRORS, "Error %s, value: %s\n", tag.c_str(), value.c_str());
                            status = EIO;
                        }
                    }
                }
                else Log(LOG_ERRORS, "Unable to update clutch because named data broker not inititalized\n");
            }
        }
        else
        {
            status  = ENODATA;
            Log(LOG_ERRORS, "Failed to read clutch status error = %s\n", response.c_str());
        }
        if((status = m_clutchSync.Release()) == EOK)
        {
            Log(LOG_DEV_DATA, "Relesed clutch synchronization object\n");
        }
        else Log(LOG_ERRORS, "Failed to relese clutch synchronization object because: %s\n", strerror(status));
    }
    else Log(LOG_ERRORS, "Failed to acquire clutch synchronization object because: %s\n", strerror(status));

    Log(LOG_FN_ENTRY, "MotorController::Clutch(%s)\n", clutch.c_str());
    return(status);
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::SetBoostStatus(std::string boost)
{
    INT32 status = EOK;
    std::string tag, value, response;
    Log(LOG_FN_ENTRY, "MotorController::SetBoostStatus(%s)\n", boost.c_str());

    // if the set status is different than current
    if(boost == "On")   value = Enabled;
    else                value = Disabled;
    if(ReadSubscribeData(BOOST_MODE) != value)
    {   // Write The boost bit
        Log(LOG_DEV_DATA, "Setting Boost Status %s\n", value.c_str());
        status = m_dataComm->Write(BOOST_MODE, value, response, true);
        if(status != EOK)
        {   // if the send was not successful figure out why
            status = EIO;

            if(m_dataComm->GetNext(tag, value, response) == 0)
                Log(LOG_ERRORS, "Error Boosting, Status: %s\n", value.c_str());
            else
                Log(LOG_ERRORS, "Error Boosting, Can Not Read Status\n");
        }
    }
    else Log(LOG_DEV_DATA, "Set Boost Status Not Changing %s\n", boost.c_str());

    Log(LOG_FN_ENTRY, "MotorController::SetBoostStatus(%s) done\n", boost.c_str());

    return(status);
}

//-------------------------------------------------------------------------------------------------
void MotorController::EnableMotorLoading( bool enable)
{
    m_motorLoadingEnabled = enable;
}

//-------------------------------------------------------------------------------------------------
const bool MotorController::IsMotorLoadingEnabled() const
{
    return( m_motorLoadingEnabled);
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::CheckStatus(void)
{
    INT32 status = EOK;
    std::string response;

    Log(LOG_FN_ENTRY, "MotorController::CheckStatus()\n");

    for(UINT32 ii = 0; ii < m_faults.size(); ii++)
    {   // if the fault exists, report it and log it
        try
        {
            response = ReadSubscribeData(GetDataTag(m_faults.getNode(ii)->getName()));
            XmlNode *state = const_cast< XmlNode * >(m_faults.getNode(ii)->getAttributes().getNode("PreviousState"));
            if(response != state->getValue())
            {
                state->setValue(response);
                if(response == Enabled)    // if the new value is a fault being set
                {
                    Log(LOG_DEV_DATA, "Setting Fault: %s, %s, %s\n", m_faults.getNode(ii)->getName().c_str(),
                        m_faults.getNode(ii)->getValue().c_str(), response.c_str());
                    status = EIO;
                    if(m_faultComm)    // if the server has been registered
                        m_faultComm->SetFault(m_faults.getNode(ii)->getName(), m_faults.getNode(ii)->getValue(),
                                              GetProcessName(), response, true);
                }
                else
                {   // if the new value is a fault being cleared
                    Log(LOG_DEV_DATA, "Clearing Fault: %s, %s, %s\n", m_faults.getNode(ii)->getName().c_str(),
                        m_faults.getNode(ii)->getValue().c_str(), response.c_str());

                    if(m_faultComm)    // if the server has been registered
                        m_faultComm->ClearFault(m_faults.getNode(ii)->getName(), response, true);
                }
            }
        }
        catch(XmlException &e)
        {
            Log(LOG_ERRORS, "Warning, Not Subscribed For: %s\n", m_faults.getNode(ii)->getName().c_str());
        }
    }

    Log(LOG_FN_ENTRY, "MotorController::CheckStatus() done\n");

    return(status);
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::GetUpdateRate(void) const
{
    return(m_updateRate);
}

//-------------------------------------------------------------------------------------------------
const std::string MotorController::CreateStringFromFloat(const std::vector<float>& data, const std::string prefix)
{
    Log(LOG_FN_ENTRY, "MotorController::CreateStringFromFloat()\n");

    char buff[255];
    std::string temp = prefix;

    for(UINT8 ii = 0; ii < data.size(); ii++)
    {
        temp += CreateMessage(buff, sizeof(buff), "%3.2f, ", m_speeds[ii]);
    }
    Log(LOG_FN_ENTRY, "MotorController::CreateStringFromFloat(%s) done\n", temp.c_str());
    return(temp);
}

//-------------------------------------------------------------------------------------------------
const bool MotorController::ShouldBoost(const float masterSpeed, const float slaveSpeed)
{   // Default to no boost
    bool shouldBoost = false;
    Log(LOG_FN_ENTRY, "MotorController::ShouldBoost(%3.2f, %3.2f)\n", masterSpeed, slaveSpeed);

    if((masterSpeed < 0.0) || (slaveSpeed < 0.0))
    {   // If either speed is negative (reverse)
        Log(LOG_CLASSCOMPONENT_DATA, "Speeds are negative, master = %3.2f, slave = %3.2f\n", masterSpeed, slaveSpeed);
        if(m_reverseBoost == true)
        {   // If reverse boost is enabled
            shouldBoost = true; // Enable boost
            Log(LOG_CLASSCOMPONENT_DATA, "Reverse boost enabled\n");
        }
        else Log(LOG_CLASSCOMPONENT_DATA, "Reverse boost disabled via configuration\n");
    }
    else
    {   // Speeds are not negative
        if((fabs(masterSpeed - slaveSpeed)) > m_speedDeltaForBoost)
        {   // Speed differential is great enough to require boost
            shouldBoost = true; // Enable boost
        }
    }
    Log(LOG_FN_ENTRY, "MotorController::ShouldBoost(%3.2f, %3.2f) done\n", masterSpeed, slaveSpeed);

    return(shouldBoost);
}

//-------------------------------------------------------------------------------------------------
void MotorController::SetClutchOnZeroSpeed(const bool& clutchOnZeroSpeed)
{
    m_clutchOnZeroSpeed = clutchOnZeroSpeed;
}

//-------------------------------------------------------------------------------------------------
const bool& MotorController::ClutchOnZeroSpeed(void)
{
    return(m_clutchOnZeroSpeed);
}

//-------------------------------------------------------------------------------------------------
const INT32 MotorController::GetWheelCount(void)
{
    const std::string junk( ReadSubscribeData( GetDataTag( "VehicleAxleCount")));
    INT32   wheelCount;

    wheelCount = atoi(junk.c_str());
    if( wheelCount <= 0)    wheelCount = 2;

    return( 2*wheelCount);
}

