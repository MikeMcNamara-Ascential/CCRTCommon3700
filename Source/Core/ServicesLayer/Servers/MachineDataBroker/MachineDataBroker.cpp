//*************************************************************************
// Description:
//   Machine data broker that is responsible for maintaining and publishing
//   all of the information related to the machine.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MachineDataBroker/MachineDataBroker.cpp $
// 
// 9     10/26/06 11:23a Rwiersem
// Changes for the 06102701 core release:
// 
// Removed handling of the RELOAD_PULSE.  This is now handled by BepServer
// and BepComponent.
// 
// 8     4/13/06 4:16a Cward
// added parens around return value
//
// 7     3/10/06 3:54p Cward
// Updated to allow reloading of config files without having to reboot
//
// 2     3/09/06 8:32a Rwiersem
// Added a try-catch block around the reload configuration pulse handling.
//
// 1     3/06/06 1:21p Rwiersem
// Added methods used when reloading the configuration file.
//
// 6     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 5     4/29/05 8:00a Derickson
// Updated the software to optionally pull the MACHINE environment
// variable and use that instead of the machine number set in the
// configuration file
//
// 5     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 4     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 3     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 2     7/07/04 4:26p Bmeinke
// Make sure handle to the LCTimeDAte data file is not NULL before writing
// to it
//
// 2     5/12/04 9:53p Cward
// Changes for the May 12 iteration
//
// 1     12/21/03 6:41p Cward
//
// 3     10/24/03 1:42p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:30a Khykin
// Initial checkin
//
// 2     8/07/03 2:18p Khykin
// Updated with changes from Flatrock.
//
// 2     6/20/03 8:57a Khykin
// Update due to Ford changes.
//
// 1     6/12/03 4:03p Khykin
// Initial checkin.
//
// 2     4/30/03 10:31a Khykin
// Added new tag (TestResultToPrint) support for printing test reports.
//
// 1     4/22/03 9:48a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:01  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:14a Khykin
// Initial Checkin
//
// 1     1/18/03 8:09a Khykin
// Initial Checkin From DCTC
//
// 4     8/23/02 8:37a Kh
// Synchronizing the pit machine with VSS.
//
// 3     5/24/02 3:07p Kh
// Adding more functionality to support LC tests.
//
// 2     2/21/02 2:08p Skypig13
// Update to new BepServer format
//
// 1     12/28/01 12:12p Kh
// Initial Checkin.
//
//*************************************************************************
#include "DataAnalysis.h"
#include "INamedDataBroker.h"
#include "MachineDataBroker.h"
#include "SoftwareId.h"
#include <math.h>
#include <unistd.h>

//-----------------------------------------------------------------------------
MachineDataBroker::MachineDataBroker(void) : BepServer(),
    m_numberOfTestsSinceLossCompensation(0),
    m_cycleTimerStarted(false),
    m_encoderDataComm(NULL),
    m_supplyEncoderData(false),
    m_encoderCommConfig(NULL),
m_connectToQnxDataServer(false),
    m_speedScalingFactor(1.0), m_currentSpeedScalingFactor(1.0)
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
MachineDataBroker::~MachineDataBroker(void)
{
    if(m_encoderCommConfig != NULL)  delete m_encoderCommConfig;
    m_encoderCommConfig = NULL;
    if(m_encoderDataComm != NULL)
    {
        m_encoderDataComm->ClosePort();
        delete m_encoderDataComm;
        m_encoderDataComm = NULL;
    }
	// Clear out the maintenance data items
	for(UINT8 index = 0; index < m_maintenaceItems.size(); index++)
	{
		delete m_maintenaceItems[index];
	}
	m_maintenaceItems.clear();
    Log(LOG_FN_ENTRY, "MachineDataBroker Destroyed\n");
}

//-----------------------------------------------------------------------------
void MachineDataBroker::Initialize(const std::string &fileName)
{
    BepServer::Initialize(fileName);

}

//-----------------------------------------------------------------------------
void MachineDataBroker::Initialize(const XmlNode *document)
{
    try
    {
        if(document)
        {
            // Initialize the base class component
            BepServer::Initialize(document);

            // set the default driver number for display
            Write(DRIVER_NUMBER_DISPLAY_DATA_TAG, DEFAULT_DRIVER_NUMBER);

            // load the additional configuration items need by the
            // machine data broker
            LoadAdditionalConfigurationItems(document);
			// Create any maintenance items
			try
			{
				LoadMaintenanceItems(document->getChild("Setup/MaintenanceItems"));
			}
			catch(XmlException &excpt)
			{
				Log(LOG_ERRORS, "Maintenance items not setup: %s", excpt.GetReason());
			}
        }
        else
        {
            throw BepException("Configuration node passed is NULL");
        }
    }
    catch(BepException &BepErr)
    {
        Log(LOG_ERRORS, "MachineDataBroker::Initialize() failed: %s", BepErr.what());
        if(m_comm) delete m_comm;m_comm = NULL;
        throw;
    }
    catch(...)
    {
        Log(LOG_ERRORS, "MachineDataBroker::Initialize() failed");
        if(m_comm) delete m_comm;m_comm = NULL;
        throw;
    }
    Log(LOG_FN_ENTRY, "MachineDataBroker::Initialize(%s) done\n", document->ToString().c_str());
}

//-----------------------------------------------------------------------------
void MachineDataBroker::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    // get the data file information and update the current state of the machine
    m_dataFile = document->getChild("Setup")->getChild("DataFile")->getValue();
    Log(LOG_DEV_DATA, "Using Data File: %s\n", m_dataFile.c_str());
    GetMachineState(m_dataFile);

    // make the software Id available
    std::string custId = document->getChild("Setup")->getChild("CustId")->getValue();
    std::string custBld = document->getChild("Setup")->getChild("CustBld")->getValue();
    int vehicleCenteringPositionLeftLimit;
    int vehicleCenteringPositionRightLimit;
    try
    {
        vehicleCenteringPositionLeftLimit = atoi(document->getChild("Setup")->getChild("VehicleCenteringPositionLeftLimit")->getValue().c_str());
        vehicleCenteringPositionRightLimit = atoi(document->getChild("Setup")->getChild("VehicleCenteringPositionRightLimit")->getValue().c_str());
    }
    catch(...)
    {
        vehicleCenteringPositionLeftLimit = 0;
        vehicleCenteringPositionRightLimit = 0;

    }
    SetVehicleCenteringPositionLimits(vehicleCenteringPositionLeftLimit, vehicleCenteringPositionRightLimit);
    SoftwareId SftwId(custId, custBld);
    XmlNode softId(SOFTWARE_ID, SftwId.GetId());
    Write(&softId);

    SetMachineNumber(document);
    SetupCycleTimer(document);
    SetupPowerSupplyMonitoring(document);
    SetupEncoderDataSupply(document);
    SetupIcmDataTimer(document);
    // Save the ICM data file names
    try
    {
        m_icmDataFiles = document->getChild("Setup/IcmDataFiles")->Copy();
    }
    catch(XmlException &excpt)
    {
        m_icmDataFiles = NULL;
    }
    // Determine if we need to connect to the QNX Data Server
    bool connectToServer = false;
    try
    {
        connectToServer = atob(document->getChild("Setup/ConnectToQnxDataServer")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Not connecting to QNX Data Server: %s", excpt.GetReason());
    }
    ConnectToQnxDataServer(&connectToServer);
    if(ConnectToQnxDataServer())    m_qnxDataServer.UpdateQnxDataServerVariable(&softId);
    // Set the speed scaling factor
    try
    {
        m_speedScalingFactor = atof(document->getChild("Setup/SpeedScalingFactor")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Speed Scaling factor not specified, using 1.0 - %s", excpt.GetReason());
        m_speedScalingFactor = 1.0;
    }
    m_currentSpeedScalingFactor = m_speedScalingFactor;
}

//-----------------------------------------------------------------------------
void MachineDataBroker::SetMachineNumber(const XmlNode *document)
{
    string machineNumber("0");
    try
    {   // Determine how the machine number will be set
        string machineNumParam = document->getChild("Setup/UseConfigMachineNumber")->getValue();
        bool useConfigMachineNumber = ((stricmp(machineNumParam.c_str(), "yes") == 0) ||
                                       (stricmp(machineNumParam.c_str(), "true") == 0) ||
                                       (stricmp(machineNumParam.c_str(), "1") == 0));
        // Set the machine number if we are not using the machine number from the config file
        if(!useConfigMachineNumber)
        {   // Get the machine number from the environment variable
            string machineNumber(getenv(document->getChild("Setup/DataTags/MachineNumberEnvVar")->getValue().c_str()));
            Log(LOG_DEV_DATA, "Using MachineNumber from environment variable: %s", machineNumber.c_str());
            Write(document->getChild("Setup/DataTags/MachineNumberTag")->getValue(), machineNumber);
        }
        else
        {
            Log(LOG_DEV_DATA, "Using MachineNumber from config file");
        }
    }
    catch(XmlException &excpt)
    {
        Log(LOG_DEV_DATA, "UseConfigMachineNumber parameter not specified, using MachineNumber from config file");
    }
}

//-----------------------------------------------------------------------------
void MachineDataBroker::SetupCycleTimer(const XmlNode *document)
{
    UINT64 rate;

    try
    {
        const XmlNode *ctNode = document->getChild( "Setup")->getChild( "CycleTimeUpdateRate");
        rate = atol( ctNode->getValue().c_str());
    }
    catch(BepException &e)
    {
        rate = 500; // default to 500 ms delay
    }

    Log( LOG_DEV_DATA, "Using CycleTimeUpdateRate = %f\n", rate);
    // if the update was not shut off
    if(rate)
    {
        // convert the time to seconds
        m_cycleTimeTimer.SetPulseCode( MACHINE_DATA_BROKER_PULSE_CODE);
        m_cycleTimeTimer.SetPulseValue( CYCLE_TIMER_PULSE);
        m_cycleTimeTimer.Initialize(GetProcessName().c_str(), mSEC_nSEC(rate),
                                    mSEC_nSEC(rate));
        m_cycleTimeTimer.Stop();
    }
}

//-----------------------------------------------------------------------------
void MachineDataBroker::SetupPowerSupplyMonitoring(const XmlNode *document)
{
    // set up the power supply timer
    UINT64 powerSupplyUpdateRate;    // rate of power supply updates
    try
    {
        const XmlNode *psNode = document->getChild( "Setup")->getChild( "PowerSupplyUpdateRate");
        powerSupplyUpdateRate = atol( psNode->getValue().c_str());
    }
    catch(BepException &e)
    {
        powerSupplyUpdateRate = 500; // default to 500 ms delay
    }
    Log( LOG_DEV_DATA, "Using PowerSupplyUpdateRate = %f\n", powerSupplyUpdateRate);

    if(powerSupplyUpdateRate)          // if the update was not shut off
    {
        // setup the timer
        m_PowerSupplyTimer.SetPulseCode(MACHINE_DATA_BROKER_PULSE_CODE);
        m_PowerSupplyTimer.SetPulseValue(POWER_SUPPLY_PULSE);
        m_PowerSupplyTimer.Initialize(GetProcessName().c_str(),
                                      (mSEC_nSEC(powerSupplyUpdateRate)),
                                      (mSEC_nSEC(powerSupplyUpdateRate)));
        m_PowerSupplyTimer.Stop();
    }

    // read in the encoder 5V power supply minimum and maximum
    try
    {
        const XmlNode *psMinNode = document->getChild( "Setup")->getChild( "Encoder5VMinimum");
        m_encoder5vMin = atof( psMinNode->getValue().c_str());
    }
    catch(BepException &e)
    {
        m_encoder5vMin = 4.75;
    }

    try
    {
        const XmlNode *psMaxNode = document->getChild( "Setup")->getChild( "Encoder5VMaximum");
        m_encoder5vMax = atof( psMaxNode->getValue().c_str());
    }
    catch(BepException &e)
    {
        m_encoder5vMax = 5.25;
    }
}

//-----------------------------------------------------------------------------
void MachineDataBroker::SetupEncoderDataSupply(const XmlNode *document)
{
    UINT64 encoderDataUpdateRate = 0;
    bool startTimer = false;
    // Load the encoder data timer information
    try
    {
        encoderDataUpdateRate = atol(document->getChild("Setup/EncoderDataUpdateRate")->getValue().c_str());
        startTimer = true;
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Encoder data update rate not specified, not supplying encoder data to external systems: %s",
            excpt.GetReason());
        encoderDataUpdateRate = 0;
        startTimer = false;
    }
    // Determine if the comm data config should be saved
    if(startTimer)
    {
        try
        {
            m_encoderCommConfig = document->getChild("Setup/EncoderDataComm")->Copy();
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "Encoder data communication not defined, not supplying encoder data: %s", excpt.GetReason());
            startTimer = false;
        }
    }
    SupplyEncoderDataExternally(&startTimer);
    // Setup the timer if a data rate was specified
    if(SupplyEncoderDataExternally())
    {   // Setup the timer
        m_encoderDataTimer.SetPulseCode(MACHINE_DATA_BROKER_PULSE_CODE);
        m_encoderDataTimer.SetPulseValue(ENCODER_DATA_PULSE);
        m_encoderDataTimer.Initialize(GetProcessName(), mSEC_nSEC(encoderDataUpdateRate), mSEC_nSEC(encoderDataUpdateRate));
        m_encoderDataTimer.Stop();
    }
    // Save the string format
    try
    {
        m_encoderDataStringFormat.DeepCopy(document->getChild("Setup/EncoderDataString")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not load encoder data string format: %s",excpt.GetReason());
    }
    // Save the encoder data format
    try
    {
        m_encoderDataFormat = document->getChild("Setup/EncoderDataformat")->getValue();
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No data formatter specified, using 5.1f: %s", excpt.GetReason());
        m_encoderDataFormat = "%5.1f";
    }
}

//-----------------------------------------------------------------------------
void MachineDataBroker::SetupIcmDataTimer(const XmlNode *document)
{
    UINT64 updateRate = 0;
    try
    {
        updateRate = atol(document->getChild("Setup/IcmDataUpdateRate")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Not starting ICM data timer: %s", excpt.GetReason());
        updateRate = 0;
    }
    if(updateRate > 0)
    {
        Log(LOG_DEV_DATA, "Setting up ICM data timer with %dms update rate", updateRate);
        m_icmDataTimer.SetPulseCode(MACHINE_DATA_BROKER_PULSE_CODE);
        m_icmDataTimer.SetPulseValue(ICM_DATA_PULSE);
        m_icmDataTimer.Initialize(GetProcessName(), mSEC_nSEC(updateRate), mSEC_nSEC(updateRate));
        m_icmDataTimer.Stop();
    }
}

//-----------------------------------------------------------------------------
const std::string MachineDataBroker::Register(void)
{
    std::string status = BEP_FAILURE_RESPONSE;

    // register the base class
    status = BepServer::Register();
    Log(LOG_DEV_DATA, "Base class registered, creating ICM data supplier");
    // Create a new ICM data supplier object
    if (m_icmDataFiles != NULL)
    {
        m_icmData = new IcmDataSupplier(m_icmDataFiles, RollerCount());
        if(BEP_STATUS_SUCCESS == m_icmData->OpenIcmDataFiles())
        {
            m_icmDataTimer.Start();
            Log(LOG_DEV_DATA, "Opened ICM data files and started timer");
        }
        else
        {
            Log(LOG_ERRORS, "Could not open ICM data files, not starting ICM data timer");
        }
    }
    // start the cycle time timer
    Log(LOG_DEV_DATA, "Starting cycle timer...");
    m_cycleTimeTimer.Start();

    // start the power supply timer
    Log(LOG_DEV_DATA, "Starting power supply timer...");
    m_PowerSupplyTimer.Start();
    // Start the encoder data timer and open the serial channel to supply the data
    if(SupplyEncoderDataExternally())
    {   // Open the data files
        OpenSpeedFile();
        OpenForceFile();
        OpenWheelDistanceFile();
        // Create a new comm object
        m_encoderDataComm = new SerialChannel();
        m_encoderDataComm->Initialize(m_encoderCommConfig);
        // Determine the number of rollers on the machine
        
        // Start the timer
        m_encoderDataTimer.Start();
    }
    
    // Determine if we need to connect to the QNX Data Server
    if(ConnectToQnxDataServer())  m_qnxDataServer.ConnectToQnxDataServer();

    // Set the current speed scaling factor
    INamedDataBroker ndb;
    string response, value;
    if(BEP_STATUS_SUCCESS == ndb.Read("SpeedDisplayScaling", response, true))
    {
        if(BEP_STATUS_SUCCESS == ndb.GetByTag("SpeedDisplayScaling", value, response))
        {
            m_currentSpeedScalingFactor = !value.compare("0") ? 1.0 : m_speedScalingFactor;
        }
    }
    Log(LOG_FN_ENTRY, "MachineDataBroker::Register() - exit");
    return(status);
}

//-----------------------------------------------------------------------------
void MachineDataBroker::Terminate(void)
{   // store the state of the machine
    StoreMachineState(m_dataFile);
    // call the base class terminate method
    BepServer::Terminate();
}

//-----------------------------------------------------------------------------
const INT32 MachineDataBroker::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 retVal = BEP_STATUS_FAILURE;

    Log( LOG_FN_ENTRY, "Enter MachineDataBroker::HandlePulse(%d, %d)\n", code, value);

    switch(code)
    {
    case MACHINE_DATA_BROKER_PULSE_CODE:
        // If this is our own pulse telling us to update the cycle timer
        switch(value)
        {
        case CYCLE_TIMER_PULSE:     // Update test cycle time
            retVal = CycleTimePulseHandler();
            break;

        case POWER_SUPPLY_PULSE:    // update power supplies
            retVal = PowerSupplyPulseHandler();
            break;

        case ENCODER_DATA_PULSE:    // Update encoder data to external systems
            retVal = EncoderDataPulseHandler();
            break;

        case ICM_DATA_PULSE:
            retVal = IcmDataPulseHandler();
            break;

        default:
            Log(LOG_ERRORS, "Pulse value not supported: %d", value);
            break;
        }
        break;
    default:
        retVal = BepServer::HandlePulse( code, value);
        break;
    }

    Log( LOG_FN_ENTRY, "Exit MachineDataBroker::HandlePulse(%d, %d)\n", code, value);

    return( retVal);
}

//-----------------------------------------------------------------------------
INT32 MachineDataBroker::CycleTimePulseHandler(void)
{
    Log(LOG_FN_ENTRY, "MachineDataBroker::CycleTimerPulseHandler()\n");
    // if the test is in progress
    if(ReadSubscribeData(GetDataTag("TestInProgress")) == "1")
    {
        // if the timer has not been started
        if(m_cycleTimerStarted == false)
        {
            m_cycleTimerStarted = true;
            UpdateCycleTime(true);
        }
        else
        {
            UpdateCycleTime();
        }

    }
    else
    {    // if the cycle timer was started, stop it
        if(m_cycleTimerStarted == true)
        {
            Log(LOG_DEV_DATA,"Stopping the cycle timer\n");
            m_cycleTimerStarted = false;    // stop the cycle timer
            UpdateCycleTime();
        }
    }

    return(BEP_STATUS_SUCCESS);

}

//-----------------------------------------------------------------------------
void MachineDataBroker::UpdateCycleTime(bool start) /* = false */
{
    UINT16 cycleTime = 0;

    // if this is the start of cycle time counting
    if(start)
    {
        Log(LOG_DEV_DATA,"Starting the cycle timer\n");
        m_vehicleTestStart = time(NULL);
        cycleTime = 0;
    }
    // else get the current cycle time
    else
    {
        // get the current time
        time_t  currentTime = time(NULL);

        // determine the time since start
        double diff = difftime(currentTime, m_vehicleTestStart);
        if(diff < 0) diff = 0;
        if(diff > 999 * 60) diff = 999 * 60;
        cycleTime= (UINT16) diff;
    }

    Log(LOG_DEV_DATA,"Current cycle time = %d\n",cycleTime);
    char buffer[32];
    memset(buffer, 0, 32);
    sprintf(buffer,"%d",cycleTime);
    Write(GetDataTag("CycleTime"), std::string(buffer));
}

//-----------------------------------------------------------------------------
INT32 MachineDataBroker::PowerSupplyPulseHandler(void)
{
    float currentVoltage = 0.0;
    bool  voltageBelowMin = true;
    bool  voltageAboveMax = true;

    Log(LOG_FN_ENTRY, "Enter MachineDataBroker::PowerSupplyPulseHandler()\n");

    // read the current encoder 5V power supply voltage
    currentVoltage = atof(ReadSubscribeData(GetDataTag("Encoder5VPowerSupplyVoltage")).c_str());

    // compare the voltage to the minimum value
    if(currentVoltage < m_encoder5vMin)
    {
        Log(LOG_ERRORS, "Encoder 5V power supply below minimum\n");
        Log(LOG_ERRORS, "    min: %.2f value: %.2f\n",m_encoder5vMin,currentVoltage);
        voltageBelowMin = true;
    }
    else voltageBelowMin = false;

    // compare the voltage to the maximum value
    if(currentVoltage > m_encoder5vMax)
    {
        Log(LOG_ERRORS, "Encoder 5V power supply above maximum\n");
        Log(LOG_ERRORS, "    max: %.2f value: %.2f\n",m_encoder5vMax,currentVoltage);
        voltageAboveMax = true;
    }
    else voltageAboveMax = false;

    // if voltage is below min or voltage is above max
    if((voltageBelowMin == true) || (voltageAboveMax == true))
    {
        Write(GetDataTag("Upper5VPowerSupplyTolerance"), "1");    // set the failure bit
    }
    else Write(GetDataTag("Upper5VPowerSupplyTolerance"), "0");    // clear the failure bit

    Log(LOG_FN_ENTRY, "Exit MachineDataBroker::PowerSupplyPulseHandler()\n");

    return(BEP_STATUS_SUCCESS);
}

//-----------------------------------------------------------------------------
INT32 MachineDataBroker::EncoderDataPulseHandler(void)
{
    Log(LOG_FN_ENTRY, "MachineDataBroker::EncoderDataPulseHandler() - Enter");
    float speed[RollerCount()];
    float force[RollerCount()];
    float dist[RollerCount()];
    // Get encoder data
    INT32 speedStat = GetWheelSpeeds(speed);
    INT32 forceStat = GetForces(force);
    INT32 distStat =  GetWheelDistances(dist);
    // Load the data in an XmlNode
    XmlNode encoderData("EncoderData","");
    char buffer[32];
    // Store the encoder data so we can easily build a data string
    if((BEP_STATUS_SUCCESS == speedStat) && (BEP_STATUS_SUCCESS == forceStat) && (BEP_STATUS_SUCCESS == distStat))
    {
        for(INT32 index = LFWHEEL; index < RollerCount(); index++)
        {
            encoderData.addChild(m_speedTags[index], 
                                 CreateMessage(buffer, sizeof(buffer), m_encoderDataFormat.c_str(), speed[index]), 
                                 ELEMENT_NODE);
            encoderData.addChild(m_forceTags[index], 
                                 CreateMessage(buffer, sizeof(buffer), m_encoderDataFormat.c_str(), force[index]), 
                                 ELEMENT_NODE);
            encoderData.addChild(m_distTags[index], 
                                 CreateMessage(buffer, sizeof(buffer), m_encoderDataFormat.c_str(), dist[index]), 
                                 ELEMENT_NODE);
        }
        // Send the data string
        string message(BuildEncoderDataString(&encoderData));
        BEP_STATUS_TYPE stat = m_encoderDataComm->Send(message);
        Log(LOG_DEV_DATA, "Sent encoder data: %s", ConvertStatusToResponse(stat).c_str());
    }
    Log(LOG_FN_ENTRY, "MachineDataBroker::EncoderDataPulseHandler() - Exit");
    return BEP_STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
INT32 MachineDataBroker::IcmDataPulseHandler(void)
{   // Get the current wheel speeds
    float rollerSpeeds[RollerCount()];
    float maxRollerSpeed = 0.0;
    BEP_STATUS_TYPE status = m_icmData->GetWheelSpeeds(rollerSpeeds);
    // Log the roller speeds
    for(UINT8 index = 0; index < RollerCount(); index++)
    {
        Log(LOG_DEV_DATA, "Roller %d speed: %.2f", index, rollerSpeeds[index]);
        maxRollerSpeed = fabs(rollerSpeeds[index]) > fabs(maxRollerSpeed) ? rollerSpeeds[index] : maxRollerSpeed;
    }
    maxRollerSpeed *= m_currentSpeedScalingFactor;
    // Store the maximum roller speed for anyone who wants to read it
    char buff[32];
    string maxSpeed(CreateMessage(buff, sizeof(buff), "%.2f", maxRollerSpeed));
    Log(LOG_DEV_DATA, "Setting max roller speed to %s", maxSpeed.c_str());
    SetData(string("MaxRollerSpeed"), maxSpeed);
    return status;
}

//-----------------------------------------------------------------------------
void MachineDataBroker::GetMachineState(const std::string &dataFile)
{
    char timeBuffer[20], dateBuffer[20];
    memset(timeBuffer, 0, sizeof(timeBuffer));
    memset(dateBuffer, 0, sizeof(dateBuffer));
    INT32 testsSinceLossCompensation;

    // set up the data file to use
    std::string buffer = getenv("FTP_ROOT");
    buffer += dataFile;
    Log(LOG_DEV_DATA, "Machine DataBroker Opening: %s\n", buffer.c_str());
    // read the information
    FILE *fp = fopen(buffer.c_str(), "r");
    if(fp != NULL)
    {
        Log(LOG_DEV_DATA, "File Exists\n");
        fscanf(fp, "%s ,%s ,%d", timeBuffer, dateBuffer, &testsSinceLossCompensation);
        fclose(fp);
    }
    else
    {
        Log(LOG_DEV_DATA, "File Does Not Exist: %s\n", strerror(errno));
        strcpy(timeBuffer, "00:00:00");
        strcpy(dateBuffer, "00/00/00");
        testsSinceLossCompensation = 0;
    }

    // update the LC time and date information and publish it
    buffer = timeBuffer;
    Write(MACHINE_LOSS_TIME, buffer);
    buffer = dateBuffer;
    Write(MACHINE_LOSS_DATE, buffer);
    UpdateNumberOfTestsSinceLossCompensation(&testsSinceLossCompensation);
}

//-----------------------------------------------------------------------------
void MachineDataBroker::StoreMachineState(const std::string &dataFile)
{
    std::string buffer = getenv("FTP_ROOT");    // get the current user information
    buffer += dataFile;
    FILE *fp = fopen(buffer.c_str(), "w");
    std::string time = Read(MACHINE_LOSS_TIME);
    std::string date = Read(MACHINE_LOSS_DATE);
    Log(LOG_DEV_DATA, "Storing Time: %s, Date: %s, Tests Since Loss Compensation: %d\n",
        time.c_str(), date.c_str(), GetNumberOfTestsSinceLossCompensation());
    if( fp != NULL)
    {
        fprintf(fp, "%s ,%s ,%d", time.c_str(), date.c_str(), GetNumberOfTestsSinceLossCompensation());
        fclose(fp);
    }
}

//-----------------------------------------------------------------------------
const std::string MachineDataBroker::Command(const XmlNode *node)
{
    std::string result(BEP_ERROR_RESPONSE);
    if(FindTag(node->getName()))
    {   // Write some stuff
        result = SetData(node);             // Put data in my list
        if(m_nList != NULL) m_nList->Notify(node);     // Do the notify thing
        else Log(LOG_ERRORS, "Invalid NotificationList in %s or no subscribers\n", GetProcessName().c_str());
    }
	else if(!node->getName().compare(GetDataTag("ResetMaintenanceItem")))
	{   // Find the maintenance item and reset the elapsed time
		bool done = false;
		for(vector<MaintenanceItem*>::iterator iter = m_maintenaceItems.begin();
			(iter != m_maintenaceItems.end()) && !done; iter++)
		{
			if(!(*iter)->MaintenanceItemName().compare(node->getValue()))
			{
				(*iter)->ResetInUseTime();
				done = true;
			}
		}
		StoreMaintenanceItemElapsedTime(node->getValue(), 0.0);
	}
    else result = BEP_UNSUPPORTED_RESPONSE;
    return(result);
}

//-----------------------------------------------------------------------------
const std::string MachineDataBroker::Publish(const XmlNode *node)
{
    std::string result(BEP_ERROR_RESPONSE);
    // if the publication is a test result publication
    if(node->getName() == TEST_RESULT_TAG)
    {   // update the results to print
        XmlNode testResultToPrint(TEST_RESULT_TO_PRINT_TAG, node->getValue());
        Write(&testResultToPrint);
    }
    // Check for a VIN publish
    else if(VIN_DATA_TAG == node->getName())
    {   // If loss compensation is VIN reset counter, else increment counter
        INT32 resetNumberOfTests = 0;
        if(LOSS_COMPENSATION_VIN == node->getValue())
            UpdateNumberOfTestsSinceLossCompensation(&resetNumberOfTests);
        else
            UpdateNumberOfTestsSinceLossCompensation(NULL);
        // Update the file to store the most recent data
        StoreMachineState(m_dataFile);
    }
    // Check for a cable connect
    else if(!node->getName().compare(GetDataTag("RHDSelected")))
    {
        string sideConnected(!node->getValue().compare("0") ? "Left" : "Right");
        XmlNode lastCableConnected(GetDataTag("LastCableConnected"), sideConnected);
        Write(&lastCableConnected);
    }
    // Check vehicle alignment
    else if(!node->getName().compare(GetDataTag("VehicleCenteringPosition")))
    {
        string position = "center";
        if (atoi(node->getValue().c_str()) < m_vehicleCenteringPositionLeftLimit)
        {
            position = "right";
        }
        else if (atoi(node->getValue().c_str()) > m_vehicleCenteringPositionRightLimit)
        {
            position = "left";
        }
        else
        {
            position = "center";
        }
        Write("VehicleCenteringpositionDisplay", position.c_str());
    }
    // Need to scale speed
    else if(!node->getName().compare("SpeedDisplayScaling"))
    {   // Determine the scaling to apply
        if(!node->getValue().compare("1"))
        {
            m_currentSpeedScalingFactor = m_speedScalingFactor;
        }
        else
        {   // Remove any speed scaling
            m_currentSpeedScalingFactor = 1.0;
        }
    }
	else if(!node->getName().compare(ROLLS_UP_DATA_TAG) && atob(node->getValue().c_str()))
	{   // Rolls up, start the maintenance timers
		for(vector<MaintenanceItem*>::iterator iter = m_maintenaceItems.begin(); 
			 iter != m_maintenaceItems.end(); iter++)
		{   // Start the timer if it is not already running
			if(!((*iter)->IsTimerRunning()))
			{
				(*iter)->StartInUseTimer();
				Log(LOG_DEV_DATA, "Started timer for maintenance item: %s", (*iter)->MaintenanceItemName().c_str());
			}
		}
	}
	else if(!node->getName().compare(ROLLS_DOWN_DATA_TAG) && atob(node->getValue().c_str()))
	{   // Rolls down, stop the timers
		for(vector<MaintenanceItem*>::iterator iter = m_maintenaceItems.begin();
			 iter != m_maintenaceItems.end(); iter++)
		{   // If the machine is in use (timer is running), stop the timer
			if((*iter)->IsTimerRunning())
			{   // Stop the timer and update the time
				double elapsedTime = (*iter)->StopInUseTimer();
				Log(LOG_DEV_DATA, "Stopped timer for maintenance item: %s - total elapsed time: %0.6f hours",
					(*iter)->MaintenanceItemName().c_str(), elapsedTime);
				// Store the elapsed time
				StoreMaintenanceItemElapsedTime((*iter)->MaintenanceItemName(), elapsedTime);
			}
		}
	}
    // then just call the base class to update the internal data
    result = BepServer::Publish(node);
    // Update the variable in the QNX Data Server 
    if(ConnectToQnxDataServer())  m_qnxDataServer.UpdateQnxDataServerVariable(node);

    return(result);
}

//-----------------------------------------------------------------------------
const string MachineDataBroker::Write(const XmlNode *node)
{   // Update the data in the QNX Data Server if we are connected
    if(ConnectToQnxDataServer())  m_qnxDataServer.UpdateQnxDataServerVariable(node);
    // Let the base class continue to handle the write functionality
    return BepServer::Write(node);
}

//-----------------------------------------------------------------------------
inline const string MachineDataBroker::Write(const string &tag, const string &value)
{
    return BepServer::Write(tag, value);
}

//-----------------------------------------------------------------------------
inline const INT32 MachineDataBroker::GetNumberOfTestsSinceLossCompensation(void)
{
    return(m_numberOfTestsSinceLossCompensation);
}

//-----------------------------------------------------------------------------
inline void MachineDataBroker::UpdateNumberOfTestsSinceLossCompensation(const INT32 *numberOfTests) /*= NULL*/
{
    if(numberOfTests != NULL)
        m_numberOfTestsSinceLossCompensation = *numberOfTests;
    else
        m_numberOfTestsSinceLossCompensation++;
    // Log the data and then publish to everyone
    Log(LOG_DEV_DATA, "Number of tests since last loss compensation cycle: %d\n", m_numberOfTestsSinceLossCompensation);
    char buffer[10];
    Write(TESTS_SINCE_LOSS_COMP_TAG, itoa(GetNumberOfTestsSinceLossCompensation(), buffer, 10));
}

//-----------------------------------------------------------------------------
const bool &MachineDataBroker::SupplyEncoderDataExternally(const bool *supplyData) /*= NULL*/
{
    if(supplyData != NULL)  m_supplyEncoderData = *supplyData;
    return m_supplyEncoderData;
}

//-----------------------------------------------------------------------------
INT32 MachineDataBroker::GetWheelSpeeds(float wheelSpeed[])
{
    INT32 testStatus = BEP_STATUS_FAILURE;  // overall status
    BEP_STATUS_TYPE openStatus = BEP_STATUS_ERROR;
    int retry = 3;  // the number of retrys to attempt

    Log(LOG_FN_ENTRY, "GetWheelSpeeds():Roller Count = %d\n", RollerCount());

    // Allow only one thread to access SpeedFile at a time
    if(EOK == m_speedFileMutex.Acquire())
    {
        try
        {   // clear out the wheel speeds
            memset(wheelSpeed, 0, (sizeof(float) * RollerCount()));
            if(!IsSpeedFileOpen())
            {   // Open the speed file
                openStatus = OpenSpeedFile();
                if(BEP_STATUS_SUCCESS != openStatus)
                {   // Error opening the speed file
                    Log(LOG_ERRORS, "Error opening speed file, not reading wheel speeds");
                }
            }
            // Ensure the file is open before we attempt to read
            if(IsSpeedFileOpen())
            {
                do
                {   // Read the speed data
                    lseek(m_speedFile, 0, SEEK_SET);
                    UINT32 bytes = read(m_speedFile, (void *) wheelSpeed, (sizeof(float) * RollerCount()));
                    if(bytes != (sizeof(float) * RollerCount()))
                    {   // if interrupted during a read, read again
                        if(errno == EINTR)
                        {
                            Log(LOG_ERRORS, "Interrupted Trying To Read %s\n", GetDataTag("CurrentSpeeds").c_str());
                        }
                        // else display the error report
                        else
                        {
                            Log(LOG_ERRORS, "Error Reading The File Information: %d, %s\n",
                                bytes, strerror(errno));
                        }
                    }
                    else
                    {
                        testStatus = BEP_STATUS_SUCCESS;
                        if(RollerCount() == 4)
                            Log(LOG_DEV_DATA, "Read speeds: %.2f, %.2f, %.2f, %.2f\n",
                                wheelSpeed[0], wheelSpeed[1], wheelSpeed[2], wheelSpeed[3]);
                        else
                            Log(LOG_DEV_DATA, "Read speeds: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
                                wheelSpeed[0], wheelSpeed[1], wheelSpeed[2], wheelSpeed[3],
                                wheelSpeed[4], wheelSpeed[5]);
                    }
                    // if the file could not be read, give up the time slice
                    if(testStatus != BEP_STATUS_SUCCESS)    BposSleep(1);
                }while((testStatus != BEP_STATUS_SUCCESS) && (retry--));
                // release the file mutex
                m_speedFileMutex.Release();
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "GetWheelSpeeds Exception: %s\n", e.what());
            testStatus = BEP_STATUS_ERROR;
            // release the file mutex
            m_speedFileMutex.Release();
        }
    }
    else
    {
        Log(LOG_ERRORS, "ERROR: Unable to aquire speed file mutex in GetWheelSpeeds: %s\n", errno);
        testStatus = BEP_STATUS_ERROR;
    }
    return(testStatus);
}

//-----------------------------------------------------------------------------
INT32 MachineDataBroker::GetWheelDistances(float wheelDistance[])
{
    INT32 testStatus = BEP_STATUS_FAILURE;  // overall status
    BEP_STATUS_TYPE openStatus = BEP_STATUS_ERROR;
    int retry = 3;  // the number of retrys to attempt

    Log(LOG_FN_ENTRY,"MachineDataBroker: GetWheelDistances()");

    // Allow only one thread to access SpeedFile at a time
    if(EOK == m_wheelDistanceFileMutex.Acquire())
    {
        try
        {   // clear out the wheel distances
            memset(wheelDistance, 0, (sizeof(float) * RollerCount()));
            if(!IsWheelDistanceFileOpen())
            {   // Open the WheelDistance file
                openStatus = OpenWheelDistanceFile();
                if(BEP_STATUS_SUCCESS != openStatus)
                {   // Error opening the WheelDistance file
                    Log(LOG_ERRORS, "Error opening WheelDistance file, not reading wheel distances");
                }
            }
            // Ensure the file is open before we attempt to read
            if(IsWheelDistanceFileOpen())
            {
                do
                {
                    // Read the WheelDistance data
                    lseek(m_wheelDistanceFile, 0, SEEK_SET);
                    UINT32 bytes = read(m_wheelDistanceFile, (void *) wheelDistance, (sizeof(float) * RollerCount()));
                    if(bytes != (sizeof(float) * RollerCount()))
                    {   // if interrupted during a read, read again
                        if(errno == EINTR)
                        {
                            Log(LOG_ERRORS, "Interrupted Trying To Read %s\n", GetDataTag("CurrentDistances").c_str());
                        }
                        else
                        {
                            Log(LOG_ERRORS, "Error Reading The File Information: %d, %s\n", bytes, strerror(errno));
                        }
                    }
                    else
                    {
                        testStatus = BEP_STATUS_SUCCESS;
                        if(RollerCount() == 4)
                            Log(LOG_DEV_DATA, "Read distances: %.2f, %.2f, %.2f, %.2f\n",
                                wheelDistance[0], wheelDistance[1], wheelDistance[2], wheelDistance[3]);
                        else
                            Log(LOG_DEV_DATA, "Read distances: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
                                wheelDistance[0], wheelDistance[1], wheelDistance[2], 
                                wheelDistance[3], wheelDistance[4], wheelDistance[5]);
                    }
                    // if the file could not be open, give up the time slice
                    if(testStatus != BEP_STATUS_SUCCESS)    BposSleep(1);
                }while( (testStatus != BEP_STATUS_SUCCESS) && (retry--));
                // release the file mutex
                m_wheelDistanceFileMutex.Release();
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "GetWheelDistances Exception: %s\n", e.what());
            testStatus = BEP_STATUS_ERROR;
            // release the file mutex
            m_wheelDistanceFileMutex.Release();
        }
    }
    else
    {
        Log(LOG_ERRORS, "ERROR: Unable to aquire wheel distance file mutex in GetWheelDistances: %s\n", errno);
        testStatus = BEP_STATUS_ERROR;
    }

    return(testStatus);
}

//-----------------------------------------------------------------------------
INT32 MachineDataBroker::GetForces(float force[])
{
    INT32 testStatus = BEP_STATUS_FAILURE;  // overall status
    BEP_STATUS_TYPE openStatus = BEP_STATUS_ERROR;
    int retry = 3;  // the number of retrys to attempt

    Log(LOG_FN_ENTRY,"MachineDataBroker: GetForces()");
    // Allow only one thread to access SpeedFile at a time
    if(EOK == m_forceFileMutex.Acquire())
    {
        try
        {   // clear out the forces
            memset(force, 0, (sizeof(float) * RollerCount()));
            if(!IsForceFileOpen())
            {   // Open the force file
                openStatus = OpenForceFile();
                if(BEP_STATUS_SUCCESS != openStatus)
                {   // Error opening the force file
                    Log(LOG_ERRORS, "Error opening force file, not reading forces");
                }
            }
            // Ensure the file is open before we attempt to read
            if(IsForceFileOpen())
            {
                do
                {   // Read the force data
                    lseek(m_forceFile, 0, SEEK_SET);
                    UINT32 bytes = read(m_forceFile, (void *) force, (sizeof(float) * RollerCount()));
                    if(bytes != (sizeof(float) * RollerCount()))
                    {   // if interrupted during a read, read again
                        if(errno == EINTR)
                        {
                            Log(LOG_ERRORS, "Interrupted Trying To Read %s\n", GetDataTag("CurrentForces").c_str());
                        }
                        else
                        {
                            Log(LOG_ERRORS, "Error Reading The File Information: %d, %s\n", bytes, strerror(errno));
                        }
                    }
                    else
                    {
                        testStatus = BEP_STATUS_SUCCESS;
                        if(RollerCount() == 4)
                            Log(LOG_DEV_DATA, "Read forces: %.2f, %.2f, %.2f, %.2f\n",
                                force[0], force[1], force[2], force[3]);
                        else
                            Log(LOG_DEV_DATA, "Read forces: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
                                force[0], force[1], force[2], force[3], force[4], force[5]);
                    }
                    // if the file could not be open, give up the time slice
                    if(testStatus != BEP_STATUS_SUCCESS)    BposSleep(1);
                }while( (testStatus != BEP_STATUS_SUCCESS) && (retry--));
                // release the file mutex
                m_forceFileMutex.Release();
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "GetForces Exception: %s\n", e.what());
            testStatus = BEP_STATUS_ERROR;
            // release the file mutex
            m_forceFileMutex.Release();
        }
    }
    else
    {
        Log(LOG_ERRORS, "ERROR: Unable to aquire force file mutex in GetForces: %s\n", errno);
        testStatus = BEP_STATUS_ERROR;
    }

    return(testStatus);
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE MachineDataBroker::OpenForceFile(void)
{
    INT32 attempts = 3;
    Log(LOG_DEV_DATA, "File Open: %s\n", GetDataTag("CurrentForces").c_str());
    do
    {
        m_forceFile = open(GetDataTag("CurrentForces").c_str(), O_RDONLY);
        SetForceFileOpen(m_forceFile != -1);
    } while(!IsForceFileOpen() && attempts--);

    Log(LOG_ERRORS, "Force File Opened: %s", IsForceFileOpen() ? "Success" : "Failure");
    return(IsForceFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE);
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE MachineDataBroker::CloseForceFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT32 closeAttempts = 3;
    if(IsForceFileOpen())
    {   // Force file is open, close it
        do
        {
            closeStatus = close(m_forceFile) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(closeStatus == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA, "Force File closed");
                SetForceFileOpen(false);
                m_forceFile = -1;
            }
            else
            {
                Log(LOG_DEV_DATA, "Error closing Force File: %s", strerror(errno));
            }
        } while(IsForceFileOpen() && closeAttempts--);
    }
    else
    {
        Log(LOG_DEV_DATA, "Force file not open, not attempting to close");
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return(closeStatus);
}

//-----------------------------------------------------------------------------
inline bool& MachineDataBroker::IsForceFileOpen(void)
{
    return m_forceFileOpen;
}

//-----------------------------------------------------------------------------
inline void MachineDataBroker::SetForceFileOpen(bool fileOpen)
{
    m_forceFileOpen = fileOpen;
}

//-----------------------------------------------------------------------------
inline void MachineDataBroker::SetVehicleCenteringPositionLimits(int leftLimit, int rightLimit)
{
    m_vehicleCenteringPositionLeftLimit = leftLimit;
    m_vehicleCenteringPositionRightLimit = rightLimit;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE MachineDataBroker::OpenSpeedFile(void)
{
    INT32 attempts = 3;
    Log(LOG_DEV_DATA, "File Open: %s\n", GetDataTag("CurrentSpeeds").c_str());
    do
    {
        m_speedFile = open(GetDataTag("CurrentSpeeds").c_str(), O_RDONLY);
        SetSpeedFileOpen(m_speedFile != -1);
    } while(!IsSpeedFileOpen() && attempts--);

    Log(LOG_ERRORS, "Speed File Opened: %s", IsSpeedFileOpen() ? "Success" : "Failure");
    return(IsSpeedFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE);
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE MachineDataBroker::CloseSpeedFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT32 closeAttempts = 3;
    if(IsSpeedFileOpen())
    {   // Speed file is open, close it
        do
        {
            closeStatus = close(m_speedFile) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(closeStatus == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA, "Speed File closed");
                SetSpeedFileOpen(false);
                m_speedFile = -1;
            }
            else
            {
                Log(LOG_DEV_DATA, "Error closing Speed File: %s", strerror(errno));
            }
        } while(IsSpeedFileOpen() && closeAttempts--);
    }
    else
    {
        Log(LOG_DEV_DATA, "Speed file not open, not attempting to close");
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return(closeStatus);
}

//-----------------------------------------------------------------------------
inline bool& MachineDataBroker::IsSpeedFileOpen(void)
{
    return(m_speedFileOpen);
}

//-----------------------------------------------------------------------------
inline void MachineDataBroker::SetSpeedFileOpen(bool fileOpen)
{
    m_speedFileOpen = fileOpen;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE MachineDataBroker::OpenWheelDistanceFile(void)
{
    INT32 attempts = 3;
    Log(LOG_DEV_DATA, "File Open: %s\n", GetDataTag("CurrentDistances").c_str());
    do
    {
        m_wheelDistanceFile = open(GetDataTag("CurrentDistances").c_str(), O_RDONLY);
        SetWheelDistanceFileOpen(m_wheelDistanceFile != -1);
    } while(!IsWheelDistanceFileOpen() && attempts--);

    Log(LOG_ERRORS, "WheelDistance File Opened: %s", IsWheelDistanceFileOpen() ? "Success" : "Failure");
    return(IsWheelDistanceFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE);
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE MachineDataBroker::CloseWheelDistanceFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT32 closeAttempts = 3;
    if(IsWheelDistanceFileOpen())
    {   // WheelDistance file is open, close it
        do
        {
            closeStatus = close(m_wheelDistanceFile) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(closeStatus == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA, "WheelDistance File closed");
                SetWheelDistanceFileOpen(false);
                m_wheelDistanceFile = -1;
            }
            else
            {
                Log(LOG_DEV_DATA, "Error closing WheelDistance File: %s", strerror(errno));
            }
        } while(IsWheelDistanceFileOpen() && closeAttempts--);
    }
    else
    {
        Log(LOG_DEV_DATA, "WheelDistance file not open, not attempting to close");
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return(closeStatus);
}

//-----------------------------------------------------------------------------
inline bool& MachineDataBroker::IsWheelDistanceFileOpen(void)
{
    return(m_wheelDistanceFileOpen);
}

//-----------------------------------------------------------------------------
inline void MachineDataBroker::SetWheelDistanceFileOpen(bool fileOpen)
{
    m_wheelDistanceFileOpen = fileOpen;
}

//-----------------------------------------------------------------------------
inline INT32 MachineDataBroker::RollerCount(void)
{
    return BposReadInt(GetDataTag(ROLLER_COUNT).c_str());
}

//-----------------------------------------------------------------------------
inline float MachineDataBroker::GetCurrentSpeedScalingFactor(void)
{
    return m_currentSpeedScalingFactor;
}

//-----------------------------------------------------------------------------
string MachineDataBroker::BuildEncoderDataString(const XmlNode *encoderData)
{
    string dataString("");
    for(XmlNodeMapCItr iter = m_encoderDataStringFormat.begin(); iter != m_encoderDataStringFormat.end(); iter++)
    {
        if(!iter->second->getAttribute("Type")->getValue().compare("Const"))
        {
            dataString += iter->second->getValue(); 
        }
        else if(!iter->second->getAttribute("Type")->getValue().compare("Data"))
        {
            XmlNodeMapCItr dataIter = encoderData->getChildren().find(iter->second->getValue());
            dataString += (dataIter != encoderData->getChildren().end()) ? dataIter->second->getValue() : "00.0";
        }
    }
    Log(LOG_DEV_DATA, "Encoder data string: %s", dataString.c_str());
    return dataString;
}

//-----------------------------------------------------------------------------
const bool& MachineDataBroker::ConnectToQnxDataServer(const bool *connectToServer) /*= NULL*/
{
    if(connectToServer != NULL)  m_connectToQnxDataServer = *connectToServer;
    return m_connectToQnxDataServer;
}

//-----------------------------------------------------------------------------
void MachineDataBroker::LoadMaintenanceItems(const XmlNode *config)
{   // Load the current in use time file
	Log(LOG_FN_ENTRY, "MachineDataBroker::LoadMaintenanceItems() - Enter");
	XmlParser parser;
	m_maintenanceTimeFileName = getenv("USR_ROOT") + config->getAttribute("MaintenanceTrackingFile")->getValue();
	// Make sure the file actually exists
	if(!access(m_maintenanceTimeFileName.c_str(), R_OK | W_OK))
	{   // Need to copy the XmlNode since after this function, the parser is destroyed and there will be 
		// nothing at the other end of the pointer.
		m_maintenanceItemTimes = parser.ReturnXMLDocument(m_maintenanceTimeFileName)->Copy();
	}
	else
	{
		Log(LOG_DEV_DATA, "No maintenance tracking file found, creating new tracking list");
		m_maintenanceItemTimes = new XmlNode("MaintenanceItems", "");
	}
	// Create all the maintenance items
	for(XmlNodeMapCItr iter = config->getChildren().begin(); iter != config->getChildren().end(); iter++)
	{   // Find the current elapsed time of the current item
		XmlNodeMapCItr mItemIter = m_maintenanceItemTimes->getChildren().find(iter->second->getName());
		double elapsedTime = 0.0;
		if(mItemIter != m_maintenanceItemTimes->getChildren().end())
		{
			elapsedTime = atof(mItemIter->second->getValue().c_str());
		}
		else
		{
			elapsedTime = 0.0;
			m_maintenanceItemTimes->addChild(iter->second->getName(), "0.0", ELEMENT_NODE);
		}
		// Create a new maintenance item
		Log(LOG_DEV_DATA, "Creating maintenance item for %s with current elapsed time %0.6f hours", 
			iter->second->getName().c_str(), elapsedTime);
		MaintenanceItem *item = new MaintenanceItem(this);
		item->Initialize(iter->second, elapsedTime);
		m_maintenaceItems.push_back(item);
	}
	Log(LOG_FN_ENTRY, "MachineDataBroker::LoadMaintenanceItems() - Exit");
}

//-----------------------------------------------------------------------------
void MachineDataBroker::StoreMaintenanceItemElapsedTime(string itemName, double elapsedTime)
{
	Log(LOG_FN_ENTRY, "MachineDataBroker::StoreMaintenanceItemElapsedTime(name: %s, elapsedTime: %0.6f) - Enter", itemName.c_str(), elapsedTime);
	XmlNodeMapCItr itemIter = m_maintenanceItemTimes->getChildren().find(itemName);
	if(itemIter != m_maintenanceItemTimes->getChildren().end())
	{
		char buff[32];
		Log(LOG_DEV_DATA, "Updating stored time...");
		itemIter->second->setValue(string(CreateMessage(buff, sizeof(buff), "%0.6f", elapsedTime)));
		Log(LOG_DEV_DATA, "Done storing elapsed time");
		// Write the times out to file
		FILE *maintenanceFile = NULL;
		if((maintenanceFile = fopen(m_maintenanceTimeFileName.c_str(), "w")) != NULL)
		{  
			Log(LOG_DEV_DATA, "Writing updated times to file...");
			fprintf(maintenanceFile, "%s", m_maintenanceItemTimes->ToString().c_str());
			fclose(maintenanceFile);
			Log(LOG_DEV_DATA, "Done writing updated times to file");
		}
		else
		{
			Log(LOG_ERRORS, "Could not open %s for updating maintenance item times", m_maintenanceTimeFileName.c_str());
		}
	}
	Log(LOG_FN_ENTRY, "MachineDataBroker::StoreMaintenanceItemElapsedTime(name: %s, elapsedTime: %0.6f) - Exit", itemName.c_str(), elapsedTime);
}
