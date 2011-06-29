//*************************************************************************
// Description:
//  InputServer will handle data entered by the operator for Vin information and
//  Driver ID information.  The InputServer also sets the state of the
//  Pendant to function in certain methods during the course of a test.
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/InputServer.cpp $
// 
// 12    10/26/06 11:10a Rwiersem
// Changes for the 06102701 core release:
// 
// Removed the call to LoadAdditionalConfigurationItems() in
// HandlePulse().
// This is now called from BepServer.
// 
// 11    6/07/06 5:36p Cward
// Changes to allow the Input Server, CLV295ScannerMgr,
// MapleKeypadManager, PendantManager, and SymbolScannerMgr configurations
// to be reloaed without restrting the processes.
//
// 1     6/05/06 3:44p Rwiersem
// Added a flag to tell if we should launch the input devices.  This is
// used to run the unit tests without the full system.  Added
// HandlePulse() and LoadAddtionalConfigurationItems() which are used to
// reload the configuration without restarting the process.
//
// 10    4/13/06 4:13a Cward
// Moved NextVIN to SysMon to remove a deadlock between InputServer and
// PlantHostInbound Added a NextVIN condition to Publish()
//
// 2     3/20/06 10:57a Bmeinke
// Moved NextVIN to SysMon to remove a deadlock between InputServer and
// PlantHostInbound
// Added a NextVIN condition to Publish()
//
// 9     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 8     5/25/05 8:55p Cward
// Updated AnalyzeValveCross and updated the structure of the Input Server
//
// 7     4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 6     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 4     8/18/04 11:40p Cward
// removed check of pendant managers state in Log statement because it
// would cause the program to crash if pendant manager is not loaded.
//
// 3     7/21/04 11:30a Cward
// Changes to allow Vin Legnth to be configurable
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:42p Cward
//
// 1     10/07/03 11:32a Khykin
// Initial checkin
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:04p Khykin
// Initial checkin.
//
// 1     4/22/03 9:47a Khykin
// Kevins Working Checkin
//
// 3     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:51  tbochene
//
//    PooBag
//
//
// 2     2/08/03 9:56a Khykin
// Updated a logging statement to use printing flags.
//
// 1     1/27/03 7:12a Khykin
// Initial Checkin
//
// 1     1/18/03 8:08a Khykin
// Initial Checkin From DCTC
//
// 8     11/26/02 4:53p Derickso
// Added support for Maple Keypads.
//
// 7     10/22/02 3:46p Derickso
// Modified to publish NextVIN tag to subscribers instead of
// performing an explicit write to the PlantHostInbound object.
//
// 6     10/15/02 1:23p Derickso
// Modified to get PlantHostInbound server name from config file.
//
// 5     8/23/02 9:29a Dan.erickson
// Made DriverNumber publishing conditional on Rolls down status.
//
// 4     7/29/02 8:27a Dan.erickson
// Added diagnostic statements.
// Added functionality to clear key press tag.
// Updated cond var handling.
//
// 3     6/13/02 2:54p Dan.erickson
// Added CLV295 Scanner support.
// WARNING:  The CLV scanner manager has not been tested!
//
// 2     6/07/02 11:52a Dan.erickson
// Move spin up of device managers to Register method.
// Keep track of build record status.
//
// 1     5/31/02 3:02p Dan.erickson
// Created and Debugged.
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================

#include "InputServer.h"

InputServer::InputServer() : BepServer(),m_ndb(NULL),m_launchInputDevices(true)
{
}

InputServer::~InputServer()
{   // Get rid of the NDB comm object
    if (m_ndb != NULL)  delete m_ndb;
    m_ndb = NULL;
}

void InputServer::Initialize(const std::string &fileName)
{   // Call the base class method
    BepServer::Initialize(fileName);
}

void InputServer::Initialize(const XmlNode *document)
{
    // Initialize the base class
    BepServer::Initialize(document);

    // Load the additional configuration items needed by the input server
    LoadAdditionalConfigurationItems(document);

    // Set the default state of the server
    XmlNode serverState(INPUT_SERVER_STATE, INPUT_SERVER_NORMAL_STATE);
    BepServer::Write(&serverState);

    // Launch the individual input devices
    LaunchInputDevices();

    Log(LOG_FN_ENTRY, "InputServer::Initialize() complete\n");
}

const INT32 InputServer::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 retVal = BEP_STATUS_FAILURE;

    // if this is a reload configuration pulse
    if((code == SYSTEM_PULSE) && (value == RELOAD_PULSE))
    {
        map<string, InputDevice_t*>::const_iterator processItr;

        // Loop through all the enabled input devices
        for (processItr=m_inputDeviceMap.begin();
            processItr!=m_inputDeviceMap.end();
            processItr++)
        {
            // Input device properties object is the second item of the pair
            InputDevice_t    *deviceProcess = processItr->second;
            TerminateDevice(deviceProcess);
        }

        // tell BepServer to reload the configuration
        ReloadConfiguration();

        // if specified, launch the individual input devices
        if(m_launchInputDevices == true)
        {
            LaunchInputDevices();
        }

        retVal = BEP_STATUS_SUCCESS;
    }
    else
    {
        // let the BepComponent handle the other pulses
        retVal = BepComponent::HandlePulse(code, value);
    }

    return(retVal);
}

void InputServer::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    // Get the data tags node
    const XmlNode *dataTags = document->getChild("Setup/DataTags");

    // Get a mapping of the Input device Managers
    try
    {
        StoreInputManagerConfigs(document->getChild("Setup/InputDevices"));
    }
    catch (...)
    {
        StoreInputManagerConfigs(NULL);
    }

    // Get the name of the PlantHostInbound object if there is one
    try
    {
        SetPlantHostInboundName(document->getChild("Setup/PlantHostInboundName")->getValue());
        if ((GetPlantHostInboundName() == BEP_NONE) || GetPlantHostInboundName().empty())
            SetPlantHostNameProvided(false);
        else
            SetPlantHostNameProvided(true);
    }
    catch (BepException &e)
    {   // No object name provided
        SetPlantHostInboundName(BEP_NONE);
        SetPlantHostNameProvided(false);
    }

    // Read in the VIN length
    try
    {
        SetVinLength(BposReadInt(dataTags->getChild("VinLength")->getValue().c_str()));
    }
    catch (...)
    {
        // if VIN length not specified, default to 17
        SetVinLength(17);
    }

    // Read the mapping of input device-to-physical map
    if (ReadInputDeviceMap(document) != true)
    {
        Log( LOG_ERRORS, "\tError reading input device mapping\n");
    }
}

void InputServer::Terminate(void)
{
    Log(LOG_FN_ENTRY, "InputServer::Terminate()\n");

    map<string, InputDevice_t*>::const_iterator processItr;

    // Loop through all the enabled input devices
    for (processItr=m_inputDeviceMap.begin();
        processItr!=m_inputDeviceMap.end();
        processItr++)
    {
        // Input device properties object is the second item of the pair
        InputDevice_t    *deviceProcess = processItr->second;
        TerminateDevice(deviceProcess);
    }
    // Call the base class
    BepServer::Terminate();
}

void InputServer::TerminateDevice(const InputDevice_t *deviceProcess)
{
    IBepCommunication deviceComm;
    string response;

    Log( LOG_FN_ENTRY, "Enter InputServer::TerminateDevice(%s)\n",
         deviceProcess->deviceName.c_str());

    deviceComm.Initialize(deviceProcess->regName,"Client"); // connect to the task
    deviceComm.SetTimeout(5000);                            // set to a five second timeout
    // register and check if successfully
    if (deviceComm.Terminate(response) == BEP_STATUS_SUCCESS)
    {
        Log(LOG_DEV_DATA, "InputServer - Successfully Commanded %s to Terminate\n",
            deviceProcess->regName.c_str());
    }
    else
    {
        Log(LOG_ERRORS, "InputServer - Could NOT Command %s to Terminate, response = %s\n",
            deviceProcess->regName.c_str(),response.c_str());
    }
}

bool InputServer::ReadInputDeviceMap( const XmlNode *docNode)
{
    bool                retVal = true;
    bool                deviceEnabled;
    const XmlNodeMap&   deviceNodes = docNode->getChild("Setup/InputDevices")->getChildren();
    XmlNodeMapCItr      xmlItr;
    const string        enabledStr("1");

    Log(LOG_FN_ENTRY, "Enter InputServer::ReadInputDeviceMap()\n");

    Log(LOG_DEV_DATA, "%d InputDevice nodes\n", deviceNodes.size());
    // Read which input devices are enabled and what process to launch for each
    for (xmlItr=deviceNodes.begin(); xmlItr!=deviceNodes.end(); xmlItr++)
    {
        // Assume input device is disabled
        deviceEnabled = false;

        const XmlNode *deviceNode = xmlItr->second;
        const XmlNodeMap& attribs = deviceNode->getAttributes();

        // Get the name of this input device
        const string inputDeviceName(deviceNode->getName());
        // Get the name of the process for this input device
        string inputDevice(attribs.getNode(CFG_INPUT_DEVICE_PROCESS_ATTR_TAG)->getValue());

        Log(LOG_DEV_DATA,"Name of the process before expansion: %s\n", inputDevice.c_str());

        // Expand any environment variables in the input device path
        FixUpDevicePath(inputDevice);

        // Check if the input device is enabled
        if (attribs.getNode(CFG_INPUT_DEVICE_EN_ATTR_TAG)->getValue() == enabledStr)
        {
            deviceEnabled = true;
        }

        // If this input device is enabled
        if (deviceEnabled == true)
        {
            std::string regName;
            try
            {
                // See if a name is explicitly supplied in the config file
                regName = deviceNode->getChild( "Setup/Logging/ProcessName")->getValue();
            }
            catch (XmlException &err)
            {
                // Use the default name that the input device will register with
                regName = INPUT_DEVICE_NAME_ROOT + inputDeviceName;
            }

            // Add this device to our list of input devices
            m_inputDeviceMap.AddDevice(inputDeviceName, regName, inputDevice);
            Log(LOG_DEV_DATA, "Adding input device %s (%s) -> %s\n",
                inputDeviceName.c_str(), regName.c_str(), inputDevice.c_str());
        }
        else
        {
            Log(LOG_DEV_DATA, "\tInput Device %s is disabled\n",
                inputDeviceName.c_str());
        }
    }

    Log(LOG_FN_ENTRY, "Exit InputServer::ReadInputDeviceMap()\n");

    return(retVal);
}

const std::string InputServer::Write(const XmlNode *node)
{
    std::string response;

    Log(LOG_FN_ENTRY, "InputServer::Write(%s)\n", node->ToString().c_str());

    if (node->getName() == NEXT_VIN_DATA_TAG)
    {
        XmlNode stateNode(INPUT_SERVER_STATE,INPUT_SERVER_TEST_RESULT_STATE);
        BepServer::Write(&stateNode);
    }
    else if (node->getName() == VINDISPLAY_DATA_TAG)
    {   // Determine if a vin update is allowed -- Rolls down
        if (RollsAreDown())
        {   // Write the node
            m_ndb->Write(node, response, true);
        }
    }
    else if ((node->getName() == DRIVER_NUMBER_DATA_TAG) ||
             (node->getName() == DRIVER_NUMBER_DISPLAY_DATA_TAG))
    {   // Set sent status to Not Sent
        if (node->getName() == DRIVER_NUMBER_DATA_TAG) SetDriverNumberStatus(driverNumberNotSent);
        // Write the data to the NamedDataBroker
        if (SendDriverNumber())
        {   // Create a NamedDataBroker client and write the driver number
            m_ndb->Write(node, response, true);
            // Set the sent status
            if ((node->getName() == DRIVER_NUMBER_DATA_TAG))
            {
                SetDriverNumberStatus(driverNumberSent);
                Log(LOG_DEV_DATA, "Wrote %s and set %s status to %s\n",
                    node->ToString().c_str(), DRIVER_NUMBER_DATA_TAG, GetDriverNumberStatus().c_str());
            }
        }
        // Set the input server state if the driver number is written
        if (node->getName() != DRIVER_NUMBER_DISPLAY_DATA_TAG)
        {
            XmlNode stateNode(INPUT_SERVER_STATE,INPUT_SERVER_NORMAL_STATE);
            BepServer::Write(&stateNode);
        }
    }
    else if (node->getName() == PENDANT_CLEAR_KEY_PRESS)
    {   // Remove the keypress node from our map
        XmlNodeMapItr iter;
        if ((iter = m_data.find(PENDANT_KEY_PRESS)) != m_data.end())
        {
            m_data.delNode(iter->second->getName());
            delete iter->second;
        }
    }
    // Write the node to our internal data and return the result
    return(BepServer::Write(node));
}

const std::string InputServer::Publish(const XmlNode *node)
{
    if (node->getName() == ROLLS_DOWN_DATA_TAG)
    {   // Store the rolls down status
      try
      {
            SetRollsDownStatus(node->getValue() == rollsDown);
            // If conditions are correct, update the driver number display
            if (SendDriverNumber() && (GetDriverNumberStatus() == driverNumberNotSent))
            {   // Send the driver number and update the GUI
                std::string response, driverNumber = Read(DRIVER_NUMBER_DATA_TAG);
                if (driverNumber == BEP_UNSUPPORTED_RESPONSE)
                {
               driverNumber = "T123456";
                    Log(LOG_DEV_DATA, "Got %s for DriverNumber...using %s\n",
                        BEP_UNSUPPORTED_RESPONSE, driverNumber.c_str());
                }
                m_ndb->Write(DRIVER_NUMBER_DATA_TAG, driverNumber, response);
                m_ndb->Write(DRIVER_NUMBER_DISPLAY_DATA_TAG, driverNumber, response, true);
                // Set the sent status
                SetDriverNumberStatus(driverNumberSent);
            }
        }
        catch (...)
        {
         Log(LOG_DEV_DATA,"Hit the trap\n");
      }
   }
    else if(node->getName() == NEXT_VIN_DATA_TAG)
    {
        XmlNode stateNode(INPUT_SERVER_STATE,INPUT_SERVER_TEST_RESULT_STATE);
        BepServer::Write(&stateNode);
    }
    // Call the base class method to do it's thing
    return(BepServer::Publish(node));
}

const std::string InputServer::Register(void)
{
    // Call the base class to register
    string retVal = BepServer::Register();

    // Initialize the WorkCellController communications
   Log(LOG_FN_ENTRY, "InputServer::Register() - begin\n");
    if (IsPlantHostInboundNameSpecified()) m_wccComm.Initialize(GetPlantHostInboundName(), "Client", IsDebugOn());
    if (m_ndb == NULL)  m_ndb = new INamedDataBroker(IsDebugOn());

    RegisterInputDevices();
    // Set the default driver number
    SetDefaultDriverNumber(DEFAULT_DRIVER_NUMBER);
    // Call the base class Register to complete registration
    Log(LOG_DEV_DATA,"InputServer::Register() - end\n");
    return(retVal);
}

std::string InputServer::GetCurrentDriverNumber(void)
{
    std::string tag, currentDriverNumber(DEFAULT_DRIVER_NUMBER), response;

    // We can't Read() until we've registered
    if( NULL != m_ndb)
    {
    // Get the current driver number
    m_ndb->Read(DRIVER_NUMBER_DATA_TAG, response, true);
    m_ndb->GetNext(tag, currentDriverNumber, response);
    }
    // Return the driver number
   return(currentDriverNumber);
}

const bool InputServer::SendDriverNumber(void)
{   // Get the current driver number
    std::string currentDriverNumber;
    currentDriverNumber = GetCurrentDriverNumber();
   bool retval = false;

    Log(LOG_DEV_DATA, "SendDriverNumber() - Rolls Down: %s, DriverNumber Status: %s, Default DriverNumber: %s,"
        " Current DriverNumber: %s, InputServerState: %s\n",
        (RollsAreDown() ? "True" : "False"), GetDriverNumberStatus().c_str(),
        GetDefaultDriverNumber().c_str(),currentDriverNumber.c_str(),GetInputServerState().c_str());
    // Determine if the driver number should be sent
    if (RollsAreDown()) retval = true;
else
{
        if (GetDefaultDriverNumber() == currentDriverNumber) retval = true;
        else if (INPUT_SERVER_DRIVER_NUMBER_STATE == GetInputServerState()) retval = true;
    }
return(retval);
}

void InputServer::LaunchInputDevices()
{
    map<string, InputDevice_t*>::const_iterator deviceItr;
    bool        devicesRunning = true;

    // Loop through all the enabled Individual input components
    for ( deviceItr=m_inputDeviceMap.begin(); deviceItr!=m_inputDeviceMap.end(); deviceItr++)
    {
        // input device properties object is the second item of the pair
        InputDevice_t    *inputDevice = deviceItr->second;

        if (false == LaunchInputDevice( inputDevice))
        {
            Log( LOG_ERRORS, "\tError launching input device %s\n", inputDevice->deviceName.c_str());
            devicesRunning = false;
        }
    }
}

bool InputServer::LaunchInputDevice(InputDevice_t    *deviceProcess)
{
    string      cfgFile(GetConfigFile());
    string      deviceCmd;
    string      parentCmd;
    bool            deviceRunning = true;
    const string &device = deviceProcess->deviceName;

    Log(LOG_FN_ENTRY, "Enter InputServer::LaunchInputDevice(%s)\n", device.c_str());

    // Expand the path to the config file
    FixUpDevicePath(cfgFile);

    // Initialize "static" command line arguments to child process
    //////////////////////////////////////////////////////////////

    // What parent to respond to after startup
    parentCmd = "-P" + GetProcessName();
    // What config to load config data from
    cfgFile = "-f" + GetConfigFile();
    // Device name argument passed to child process
    deviceCmd = "-p" + device;

    Log(LOG_DEV_DATA, "\tSpawning %s -vf %s  %s %s\n", deviceProcess->processName.c_str(),
        cfgFile.c_str(), parentCmd.c_str(), deviceCmd.c_str());
    deviceProcess->devicePid=spawnl( P_NOWAIT,
                                     deviceProcess->processName.c_str(),   // Process name
                                     deviceProcess->processName.c_str(),   // Arg 0
                                     "-vf",                      // Arg 2
                                     cfgFile.c_str(),            // Arg 3
                                     parentCmd.c_str(),          // Arg 4
                                     deviceCmd.c_str(),            // Arg 5
                                     NULL);
    if ( deviceProcess->devicePid == -1)
    {
        Log( LOG_ERRORS, "\tError spawning %s: %s\n",
             deviceProcess->processName.c_str(), strerror( errno));
    }
    else
    {
        int     drvrCoid;
        struct _pulse pulse;
        struct sigevent event;
        uint64_t timeout = mSEC_nSEC( 10000);   // 10 seconds
        uint64_t startTime, curTime;
        struct timespec timerSpec;

        Log( LOG_DEV_DATA, "Wait for %s (%s)\n", deviceProcess->processName.c_str(), deviceProcess->regName.c_str());

        // Get the time at which we launched the device
        clock_gettime( CLOCK_REALTIME, &timerSpec);
        startTime = timespec2nsec( &timerSpec);

        // Use a 10 second timeout waiting for a pulse from the input device
        event.sigev_notify = SIGEV_UNBLOCK;
        TimerTimeout( CLOCK_REALTIME,
                      _NTO_TIMEOUT_RECEIVE,
                      &event, &timeout, NULL);

        // Wait for device to tell us he is up and running
        while (MsgReceive(deviceProcess->devicePid, &pulse, sizeof( pulse), NULL) != -1)
        {
            // Get the current time
            clock_gettime( CLOCK_REALTIME, &timerSpec);
            curTime = timespec2nsec( &timerSpec);

            // If we have waited long enough for the input device to register
            if ( (curTime-startTime) >= timeout)
            {
                Log( LOG_ERRORS, "\tInput device %s failed to register '%s' in time: %s\n",
                     deviceProcess->processName.c_str(), deviceProcess->regName.c_str(), strerror( errno));
                deviceRunning = false;
                break;
            }
            // Check if he is up and running
            else if ( (drvrCoid=name_open( deviceProcess->regName.c_str(), 0)) != -1)
            {
                Log( LOG_DEV_DATA, "Input device %s registered name '%s' in time\n",
                     deviceProcess->processName.c_str(), deviceProcess->regName.c_str());
                name_close( drvrCoid);
                break;
            }
            // We received some type of miscellaneous pulse
            else
            {
                // Handle the pulse (if applicable)
                BepComponent::HandlePulse( pulse.code, pulse.value.sival_int);

                // Adjust timeout for the amount of time we've been waiting
                curTime = timeout - (curTime-startTime);
                // Reset the timeout
                TimerTimeout( CLOCK_REALTIME,
                              _NTO_TIMEOUT_RECEIVE,
                              &event, &curTime, NULL);
            }
        }
    }
    Log( LOG_FN_ENTRY, "Exit InputServer::LaunchInputDevice(%s), running=%d\n",
         device.c_str(), deviceRunning);

    return( deviceRunning);
}

void InputServer::RegisterInputDevices()
{
    map<string, InputDevice_t*>::const_iterator processItr;

    // Loop through all the enabled input devices
    for (processItr=m_inputDeviceMap.begin();
        processItr!=m_inputDeviceMap.end();
        processItr++)
    {
        // Input device properties object is the second item of the pair
        InputDevice_t    *deviceProcess = processItr->second;
        RegisterInputDevice(deviceProcess);
    }
}

void InputServer::RegisterInputDevice(const InputDevice_t *deviceProcess)
{
    IBepCommunication deviceComm;
    string response;
    string status;

    Log( LOG_FN_ENTRY, "Enter InputServer::RegisterInputDevice(%s)\n",
         deviceProcess->deviceName.c_str());

    deviceComm.Initialize(deviceProcess->regName,"Client"); // connect to the task
    deviceComm.SetTimeout(5000);                            // set to a five second timeout
    // register and check if successfully
    if (deviceComm.Register(BEP_REGISTER, status, response, true) == BEP_STATUS_SUCCESS)
    {
        Log(LOG_DEV_DATA, "InputServer - Successfully Commanded %s to Register\n",
            deviceProcess->regName.c_str());

    }
    else
    {
        Log(LOG_ERRORS, "InputServer - Could NOT Command %s to Register\n",
            deviceProcess->regName.c_str());
    }
}

void InputServer::FixUpDevicePath(string &deviceProcess)
{
    string::size_type   envStart, envEnd;
    char                *envValue;
    char                realPath[PATH_MAX+1];


    // Look for simple HOME character (~)
    while ((envStart=deviceProcess.find('~')) != deviceProcess.npos)
    {
        // Erase the '~' and replace it with $(HOME)
        deviceProcess.erase( envStart, 1);
        char *homeDir = getenv("HOME");
        if (homeDir != NULL)    deviceProcess.insert( envStart, homeDir);
        else                    deviceProcess.insert( envStart, "/home");
    }

    // Now replace any '$()' delimitted environ vars
    while ((envStart=deviceProcess.find("$(")) != deviceProcess.npos)
    {
        // Look for the end of the environ var name
        if ((envEnd = deviceProcess.find(')', envStart)) != deviceProcess.npos)
        {
            // Get the environment variable name
            string envName( deviceProcess.substr( envStart+2, envEnd-envStart-2));

            Log(LOG_DEV_DATA,"EnvName: %s\n",envName.c_str());

            // If it is a valid envirnoment variable name
            if ((envValue=getenv( envName.c_str())) != NULL)
            {
                // Replace it in the string
                deviceProcess.erase(envStart, envEnd+1);
                deviceProcess.insert(envStart, getenv( envName.c_str()));
            }

            Log(LOG_DEV_DATA,"deviceProcess in FixUpDevicePath: %s\n",deviceProcess.c_str());
        }
    }

    // Now resolve relative path to absolute path
    if (NULL != realpath(deviceProcess.c_str(), realPath))
    {
        deviceProcess = string(realPath);
        Log(LOG_DEV_DATA,"FixUpDevicePath realpath If statement:\n\trealPath: %s, deviceProcess%s\n",
            realPath,deviceProcess.c_str());
    }
    else
    {
        Log(LOG_DEV_DATA,"deviceProcess in FixUpDevicePath realpath Else statement: %s\n",deviceProcess.c_str());
        Log(LOG_ERRORS, "real path failed!!, Reason: %s\n", strerror(errno));
    }
}

inline const std::string InputServer::GetInputServerState(void)
{
    return(Read(INPUT_SERVER_STATE));
}

inline const bool& InputServer::RollsAreDown(void)
{
    return(m_rollsDownStatus);
}

inline const std::string& InputServer::GetDriverNumberStatus(void)
{
    return(m_driverNumberStatus);
}

inline const std::string& InputServer::GetDefaultDriverNumber(void)
{
    return(m_defaultDriverNumber);
}

inline const std::string InputServer::GetPlantHostInboundName(void)
{
    return(m_plantHostInboundName);
}

inline const bool InputServer::IsPlantHostInboundNameSpecified(void)
{
    return(m_plantHostSpecified);
}

inline UINT16 InputServer::GetVinLength()
{
    return(m_vinLength);
};

inline void InputServer::SetDriverNumberStatus(const std::string &status)
{
    Log(LOG_DEV_DATA, "Setting DriverNumber status to %s\n", status.c_str());
    m_driverNumberStatus = status;
}

inline void InputServer::SetDefaultDriverNumber(const std::string driverNumber)
{
    m_defaultDriverNumber = driverNumber;
}

inline void InputServer::SetRollsDownStatus(const bool &status)
{
    m_rollsDownStatus = status;
}

inline void InputServer::StoreInputManagerConfigs(const XmlNode *config)
{
    if (config != NULL) m_inputDeviceManagers.DeepCopy(config->getChildren());
    else m_inputDeviceManagers.clear(true);
}

inline void InputServer::SetPlantHostInboundName(const std::string &name)
{
    m_plantHostInboundName = name;
}

inline void InputServer::SetPlantHostNameProvided(const bool status)
{
    m_plantHostSpecified = status;
}

inline void InputServer::SetVinLength(const UINT16 vinLength)
{
    m_vinLength = vinLength;
}
