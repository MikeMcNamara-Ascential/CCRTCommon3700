//******************************************************************************
// Description:
//  Serial Communication Server
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/SerialServer/SerialServer.cpp $
// 
// 7     10/26/06 10:45a Rwiersem
// Changes for the 06102701 core release:
// 
// Now don't kill the port drivers when reloading the configuration file.
// 
// 6     5/24/06 2:56p Cward
// Updated Serial Server to allow reloading of config files without
// restarting
//
// 1     5/22/06 11:14a Rwiersem
// Added HandlePulse() and LoadAdditionalConfigurationItems() which are
// used to reload the configuration without restarting the process.  Added
// m_relaunchOnConfigReload so the unit tests don't need the full system
// running.
//
// 5     4/13/06 4:10a Cward
// Use the new FixUpPathString() method from GeneralUtilities
//
// 2     3/20/06 10:53a Bmeinke
// Use the new FixUpPathString() method from GeneralUtilities
//
// 4     7/20/05 9:13p Cward
// Added new TaskMon support
//
// 3     4/01/05 10:19a Cward
// Determine whether an explicit name or a default name should be used
// when adding port drivers
//
// 2     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:28p Cward
//
// 2     10/24/03 1:29p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 2     10/10/03 9:43a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:33a Khykin
// Initial checkin
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 1     6/12/03 3:59p Khykin
// Initial checkin.
//
// 2     5/19/03 5:24p Khykin
// Updated with Brians latest changes to work with a single node entry in
// the configuration file.
//
// 1     4/30/03 4:46p Bmeinke
// Migration to new personalized configuration management structure
//
// 22    4/17/03 3:37p Bmeinke
// Restructured config file to remove LogicalportMap and BoardConfig nodes
// Added -R command line option to allow immediate registration instead of
// waiting for the Register XML command
// Update dcommand line processing to new HandleCommandLineArg() method
//
// 21    3/26/03 10:33p Bmeinke
// Replaced my relative path name resolver logic with the C library
// realpath() function in FixUpDriverPath()
//
// 20    3/06/03 10:38p Bmeinke
// Added RegisterPortDrivers() method to send a pulse to the logical port
// drivers when we receive the Register message
//
// 19    2/21/03 7:27p Bmeinke
// Added signal handler for SIGCHLD
//
// 18    2/06/03 10:18a Bmeinke
// Removed m_commBoards member (no longer used)
// Removed ReadBoardConfig() (no longer used)
//
// 17    11/21/02 8:59a Khykin
// Disabling the auto enabling of the logger.
//
// 16    10/24/02 7:57p Bmeinke
// Removed the Command() method (just use base class)
//
// 15    10/24/02 7:55p Bmeinke
// Added support to the Reset() method to reset the individual logical
// port drivers
//
// 14    8/01/02 3:46p Bmeinke
// Removed compiler warnings
//
// 13    6/27/02 7:11p Bmeinke
// Changed driver-wait logic inside LaunchPortDrivers(): a generic pulse
// was causing the SerialServer to wake up too soon and think it had
// timed-out waiting for the child logical port drivers to register
//
// 12    6/21/02 3:05p Bmeinke
// Changed to ResManagerXml in order to properly process the system
// Register message
// Added numerous overrides in order to properly act as a ResManagerXml
// (override methods we do not support)
// Send the child port drivers a SIGQUIT signal when it is time to
// terminate
// Added a cond var (m_okToRegister); used to indicate when it is ok to
// process the system Register message (must wait for all port drivers to
// be up and running before we let the system come up)
//
// 11    4/25/02 4:45p Skypig13
// Updated with new changes
//     Revision 1.1  2002/04/25 18:39:19  erickson
//     added from VSS
//
//
// 10    2/01/02 9:51p Brian.meinke
// Removed references to UniverseClient
//
// 9     12/05/01 11:56a Brian.meinke
// Moved the installation of the signal handlers to InitManager
// If a parent process name is specified, ignore the SIGINT, SIGQUIT and
// SIGWINCH signals: the parent process will send the appropriate pulses
// to us
//
// 8     11/28/01 11:05p Brian.meinke
// Added tab characters to beginning of error logging strings
//
// 7     11/26/01 6:53p Brian.meinke
// Changed comm model to use seperate processes for each of the logical
// ports: No longer use objects within this process to handle serial I/O
//
// 6     11/16/01 9:44a Brian.meinke
// Removed base class constructor which took command line arguments; so
// now we call ParseCommandLine() explicitly from our command line
// constructor
// Changed InitManager() to no longer take a threadChid argument; instead
// it calls GetWaitChid() inside InitManager()
//
// 5     11/15/01 2:55p Brian.meinke
// Use base constructors which take command line arguments
// Use ResManagerBase::ParseCommandLine
//
// 4     11/14/01 11:09p Brian.meinke
// Added method AttachInterrupts() to attach to the necessary serial
// interrupts to handle serial I/O
// Added m_pathMap member to track inode/LogicalPort pairs in order to
// match a LogicalPort object to an I/O message
// When attaching paths for LogicalPorts, specify a "file" size equal to
// the LogicalPort's fifo size and specify an inode value equal to the
// LogicalPort's port number
// Override HandleResMgrPulse() in order to process interrupt pulses
// Added HandleInterrupt() method to process interrupt pulses and dispatch
// the appropriate CommCard object
// Added some meat to the IoWrite() method to pass write requests to the
// appropriate LogicalPorts
//
// 3     11/14/01 2:13a Brian.meinke
// Moved declaration of ResManagerBase::m_ioMgrForOcb to
// ResManagerBase.cpp
// Tell universe client to use *this as its ILogger object
// Removed modification to thread pool attributes: attributes now
// configured via config file
// Now adopt paths for each of the logical ports (as read from config
// file)
// Add Initialize() method in order to read board and port configurations
// Added ReadBoardConfig()
// Added ReadLogicalPortSetup()
// Added ReadLogicalPortMap()
//
// 2     11/09/01 5:45p Brian.meinke
// First executable version
//
// 1     11/08/01 9:17p Brian.meinke
// Serial Communication server process
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "SerialServer.h"
#include "LogPortDefs.h"
#include <libgen.h>     // dirname()
#include <limits.h>     // PATH_MAX
#include <stdlib.h>     // system()
#include <sys/stat.h>       // systemstat
#include <signal.h>     // systemstat

/**
 * Default class constructor
 */
SerialServer::SerialServer() : ResManagerXml(), m_waitForRegister( true),
    m_relaunchOnConfigReload(true)
{
    SetProcessName( SER_SRVR_NAME);
};

/**
 * Class constructor with command line arguments
 *
 * @param argc   Number of arguments to parse
 * @param argv   Array of command line arguments
 */
SerialServer::SerialServer( int argc, char *argv[]) : ResManagerXml(), m_waitForRegister( true)
{
    // Use default process name
    SetProcessName( SER_SRVR_NAME);
    ParseCommandLine( argc, argv, "R");
}

/**
 * Class destructor
 */
SerialServer::~SerialServer()
{
}

/**
 * Perform any server initialization
 *
 * @return true if successful
 *         false if error
 */
bool SerialServer::InitManager()
{
    bool retVal = true;

    Log( LOG_FN_ENTRY, "Enter InitManager()\n");

    // Set flag to false so we do not reply to a register message
    m_okToRegister.Acquire();
    m_okToRegister.Signal( false);
    m_okToRegister.Release();

    // Register our task name
    if( BposAttachName( GetProcessName().c_str(), (void**)&m_nameId) != -1)
    {
        // Initialize base class with channel for our name
        if( (retVal = ResManagerXml::InitManager()) == true)
        {
            // Install a SIGCHLD handler to catch port drivers that die
            signal( SIGCHLD, SigChildHandler);
            signal( SIGINT, SignalHandler);
            signal( SIGQUIT, SignalHandler);
            signal( SIGWINCH, SignalHandler);
            //          signal( SIGCHLD, SignalHandler);

            // Attach main serial path
            if( (m_resMgrId=PathAttach( SERIAL_DEV_ROOT, m_portDrvMap.size())) != -1)
            {
                if( RegisterManager() != -1)
                {
                    // Fire-up the logical port drivers
                    LaunchPortDrivers();

                }
                else
                {
                    Log( LOG_ERRORS, "\tError registering resource manager: %s\n",
                         strerror( errno));
                    retVal = false;
                }
            }
            else
            {
                Log( LOG_ERRORS, "\tError attaching path: %s\n", strerror( errno));
                retVal = false;
            }
        }
        else
        {
            Log( LOG_ERRORS, "\tError in base class InitManager: %s\n",
                 strerror( errno));
            retVal = false;
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tError attaching name '%s': %s\n", GetProcessName().c_str(),
             strerror( errno));
        retVal = false;
    }

    // If -R command line option specified
    if( (retVal == true) && ( m_waitForRegister == false))
    {
        // Register immediately
        Register();
    }

    Log( LOG_FN_ENTRY, "Exit  InitManager()\n");
    return( retVal);
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void SerialServer::CleanupManager()
{
    Log( LOG_FN_ENTRY, "Enter CleanupManager()\n");

    // Do base class cleanup
    ResManagerXml::CleanupManager();

    // Un-register our name
    if( m_nameId != NULL)
    {
        BposUnRegisterTaskName( (int)m_nameId);
        m_nameId = NULL;
    }

    // Detach our adopted path
    if( m_resMgrId != -1)
    {
        PathDetach( m_resMgrId);
        m_resMgrId = -1;
    }

    // Kill off the port drivers we launched
    KillPortDrivers();

    // Clear out our list of port drivers
    m_portDrvMap.clear();

    Log( LOG_FN_ENTRY, "Exit CleanupManager()\n");
}

/**
 * Reads configuration data from the given XML node
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void SerialServer::Initialize(const XmlNode *document)
{
    Log( LOG_FN_ENTRY, "Enter SerialServer::Initialize()\n");

    ResManagerXml::Initialize( document);

    // Load the additional configuration items needed by the serial server
    LoadAdditionalConfigurationItems(document);

    Log( LOG_FN_ENTRY, "Exit SerialServer::Initialize()\n");
}

const INT32 SerialServer::HandlePulse(const INT32 code, const INT32 value)
{
    INT32   retVal = BEP_STATUS_FAILURE;

    // if this is a reload configuration pulse
    if((code == SYSTEM_PULSE) && (value == RELOAD_PULSE))
    {
        // if we should relaunch the logical ports, kill them first

        // Killing the port drivers will cause any comm proxies in use by other
        // tasks to be invalid.
#if 0
        if(m_relaunchOnConfigReload == true)
        {
            KillPortDrivers();
        }
#endif
        const XmlNode *document =
            m_parser.ReturnXMLDocument(GetConfigFile());

        // load the additional configuration items needed by the
        // serial server
        LoadAdditionalConfigurationItems(document);
#if 0
        // if we should relaunch the logical ports, launch them
        if(m_relaunchOnConfigReload == true)
        {
            LaunchPortDrivers();
        }
#endif
        retVal = BEP_STATUS_SUCCESS;
    }
    else
    {
        retVal = BepComponent::HandlePulse(code, value);
    }

    return( retVal);
}

void SerialServer::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    // Read the mapping of logical ports-to-physical ports
    if( ReadLogicalPortMap( document) != true)
    {
        Log( LOG_ERRORS, "\tError reading logical port mapping\n");
    }
}

/**
 * Method that is called to command the component to perform any
 * system registration functions necessary.
 *
 * @return Status of the registration process.
 *         Success, Fail
 * @since Version 2.5
 */
const std::string SerialServer::Register(void)
{
    uint64_t    timeOutTime;
    string      status( BEP_FAIL_RESPONSE);
    bool        waitValue = true;
    int         waitErr;

    Log( LOG_FN_ENTRY, "Enter SerialServer::Register()\n");

    timeOutTime = 10LL * NANOS_PER_SECOND;

    // Wait for register flag to be true
    if( (waitErr=m_okToRegister.Acquire()) == EOK)
    {
        if( (waitErr=m_okToRegister.Wait( waitValue, timeOutTime)) == EOK)
        {
            Log( LOG_DEV_DATA, "All port drivers reported as running...\n");

            // Command each of the prot drivers to register
            RegisterPortDrivers();

            // Call base class method to register with TaskMon
            status = ResManagerBase::Register();
        }
        else
        {
            Log( LOG_ERRORS, "Error waiting for port drivers in REgister(): %s\n",
                 strerror(waitErr) );
        }

        m_okToRegister.Release();
    }
    else
    {
        Log( LOG_ERRORS, "Error locking register cond var: %s\n", strerror( waitErr));
    }

    Log( LOG_FN_ENTRY, "Exit SerialServer::Register( %s)\n", status.c_str());

    return( status);
}

/**
 * Method used to set all of the objects in the component
 * to their original states.  This would includes all
 * member variables such as buffers, lists, and stored
 * data.
 * @since Version 1.0
 */
void SerialServer::Reset(void)
{
    LogPortDrvMapItr_t  itr;

    Log( LOG_FN_ENTRY, "Enter SerialServer::Reset()\n");

    // Install a SIGCHLD handler to catch port drivers that die
    signal( SIGCHLD, SigChildHandler);

    // Iterate through all the drivers
    for( itr=m_portDrvMap.begin(); itr!=m_portDrvMap.end(); itr++)
    {
        // Get the logical port name to open/reset/close
        const string &portName = itr->first;
        LogPortDrv_t    *portDrv = itr->second;

        // Make sure the port object is valid
        if( portDrv != NULL)
        {
            // If we should not reset this port
            if( portDrv->shouldReset == false)
            {
                Log( LOG_DEV_DATA, "Skipping port '%s' because we do not need to reset it\n", portName.c_str());
            }
        // Try to open the port
            else
            {
                ResetPortDriver( portDrv);
            }

            // Check if we should re-launch the port driver
            if( true == portDrv->relaunch)
            {
                RelaunchPortDriver( portDrv);
            }
        }
        else
        {
            Log( LOG_ERRORS, "\tNULL port driver in SerialServer::Reset()\n");
        }
    }

    Log( LOG_FN_ENTRY, "Exit SerialServer::Reset()\n");
}

/**
 * Sends a reset command to the given port driver
 *
 * @param portDrv Port driver to reset
 * @return true if successful, false on error
 * @since Version 1.0
 */
bool SerialServer::ResetPortDriver( const LogPortDrv_t *portDrv)
{
    ILogicalPort        logPort;
    bool            retVal = true;
    const string        &portName = portDrv->portName;

    Log( LOG_FN_ENTRY, "Enter SerialServer::ResetPortDriver(%s)\n", portName.c_str());

    // If this port needs to be reset
    if( true == portDrv->shouldReset)
    {
        // Try to open the port
        retVal = logPort.OpenPort( portName);
        if( true == retVal)
        {
                // Lock the port while we reset it
                if( logPort.LockPort() == true)
                {
                Log( LOG_DEV_DATA, "Resetting port '%s'\n", portName.c_str());
                // Tell driver to reset
                logPort.ResetServer();

                    logPort.UnlockPort();
                }
                else
                {
                    Log( LOG_ERRORS, "\tError locking '%s' for driver reset: %s\n",
                         portName.c_str(), strerror( errno));
                }

                // Close port
                logPort.ClosePort();
            }
            else
            {
                Log( LOG_ERRORS, "\tError opening '%s' for driver reset: %s\n",
                     portName.c_str(), strerror( errno));
            }
        }
    Log( LOG_FN_ENTRY, "Exit SerialServer::ResetPortDriver(%s), status=%d\n", portName.c_str(), retVal);

    return( retVal);
}

/**
 * Reads the logical-to-physical port mapping table from the
 * specified XML node
 *
 * @param docNode Reference to a parsed configuration document.
 * @return true if successful, false on error
 * @since Version 1.0
 */
bool SerialServer::ReadLogicalPortMap( const XmlNode *docNode)
{
    bool                retVal = true;
    bool                portEnabled;
    bool                shouldReset;
    bool                relaunch;
    const XmlNodeMap&   portNodes = docNode->getChild( XML_T( "Setup/LogicalPorts"))->getChildren();
    XmlNodeMapCItr      xmlItr;
    const XmlString     enabledStr( XML_T("1"));

    Log( LOG_FN_ENTRY, "Enter SerialServer::ReadLogicalPortMap()\n");

    Log( LOG_DEV_DATA, "%d LogicalPort nodes\n", portNodes.size());
    // Read which logical port drivers are enabled and what process to launch for each
    for( xmlItr=portNodes.begin(); xmlItr!=portNodes.end(); xmlItr++)
    {
        // Assume port is disabled
        portEnabled = false;

        const XmlNode *portNode = xmlItr->second;
        const XmlNodeMap& attribs = portNode->getAttributes();

        // Get the name of this logical port
        const string logPortName( XmlToAscii( portNode->getName()));
        // Get the name of the port driver for this port
        string portDrv( XmlToAscii( attribs.getNode( CFG_PORT_DRV_ATTR_TAG)->getValue()));

        // See if we should reset this port driver when we get an XML Reset message
        shouldReset = false;
        try
        {
            if( stricmp( attribs.getNode( XML_T( "needsReset"))->getValue().c_str(), "yes") == 0)
            {
                shouldReset = true;
            }
        }
        catch( ...)
        {
            shouldReset = false;
        }
        // See if we should re-launch this port driver when we get an XML Reset message
        relaunch = false;
        try
        {
            if( stricmp( attribs.getNode( XML_T( "relaunch"))->getValue().c_str(), "yes") == 0)
            {
                relaunch = true;
            }
        }
        catch( ...)
        {
            relaunch = false;
        }

        // Expand any environment variables in the driver path
        if( EOK == FixUpPathString( portDrv))
        {
        // Check if the logical port is enabled
        if(attribs.getNode( CFG_PORT_EN_ATTR_TAG)->getValue() == enabledStr)
        {
            portEnabled = true;
        }

        // If this port is enabled
        if( portEnabled == true)
        {
            std::string regName;
            try
            {
                // See if a name is explicitly supplied in the config file
                regName = portNode->getChild( "Setup/Logging/ProcessName")->getValue();
            }
            catch( XmlException &err)
            {
                // Use the default name that the logical port will register with
                regName = LOG_PORT_NAME_ROOT + portNode->getName();
            }

            // Add this port to our list of port drivers
            m_portDrvMap.AddDriver( logPortName, regName, portDrv, shouldReset, relaunch);
            Log( LOG_DEV_DATA, "Adding logical port %s (%s) -> %s\n",
                 logPortName.c_str(), regName.c_str(), portDrv.c_str());
        }
        else
        {
            Log( LOG_DEV_DATA, "\tlogical port %s is disabled\n",
                 logPortName.c_str());
        }
    }
        else
        {
            portEnabled = false;
            Log( LOG_ERRORS, "\tERROR: Invalid logical port driver does not exist: %s\n",
                 portDrv.c_str());
        }
    }

    Log( LOG_FN_ENTRY, "Exit SerialServer::ReadLogicalPortMap()\n");

    return( retVal);
}

/**
 * Expands environment variable names in the port driver name
 *
 * @param portDrv Path of the port driver to be cleaned up
 */
void SerialServer::FixUpDriverPath( string &portDrv)
{
    string::size_type   envStart, envEnd;
    char                *envValue;
    char                realPath[ PATH_MAX+1];

    // Look for simple HOME character (~)
    while( (envStart=portDrv.find('~')) != portDrv.npos)
    {
        // Erase the '~' and replace it with $(HOME)
        portDrv.erase( envStart, 1);
        char *homeDir = getenv( "HOME");
        if( homeDir != NULL)    portDrv.insert( envStart, homeDir);
        else                    portDrv.insert( envStart, "/home");
    }

    // Now replace any '$()' delimitted environ vars
    while( (envStart=portDrv.find("$(")) != portDrv.npos)
    {
        // Look for the end of the environ var name
        if( (envEnd = portDrv.find(')', envStart)) != portDrv.npos)
        {
            // Get the environment variable name
            string envName( portDrv.substr( envStart+2, envEnd-envStart-2));
            // If it is a valid envirnoment variable name
            if( (envValue=getenv( envName.c_str())) != NULL)
            {
                // Replace it in the string
                portDrv.erase( envStart, envEnd+1);
                portDrv.insert( envStart, getenv( envName.c_str()));
            }
        }
    }

    // Now resolve relative path to abcolute path
    if( realpath( portDrv.c_str(), realPath) == 0)
    {
        portDrv = string( realPath);
    }
}

/**
 * Launches the necessary logical port driver processes
 */
void SerialServer::LaunchPortDrivers()
{
    LogPortDrvMapCItr_t drvItr;
    int         ii;
    bool        driversRunning = true;

    // Loop through all the enabled logical port
    for( drvItr=m_portDrvMap.begin(); drvItr!=m_portDrvMap.end(); drvItr++)
    {
        // Port driver properties object is the second item of the pair
        LogPortDrv_t    *portDrv = drvItr->second;

        if(false == LaunchPortDriver( portDrv))
        {
            Log( LOG_ERRORS, "\tError launching port driver %s\n", portDrv->portName.c_str());
            driversRunning = false;
        }
    }

    // Set status of register flag (used to process the register message)
    if( (ii=m_okToRegister.Acquire()) == EOK)
    {
        m_okToRegister.Signal( driversRunning);
        m_okToRegister.Release();
    }
    else
    {
        Log( LOG_ERRORS, "Error locking register cond var: %s\n", strerror( ii));
    }
}

/**
 * Launches the given port driver processes
 *
 * @param portDrv Port driver to launch
 * @return true if successful, false on error
 */
bool SerialServer::LaunchPortDriver(LogPortDrv_t    *portDrv)
{
    string      cfgFile(GetConfigFile());
    string      portCmd;
    string      parentCmd;
    bool            driverRunning = true;
    const string &port = portDrv->portName;

    Log( LOG_FN_ENTRY, "Enter SerialServer::LaunchPortDriver(%s)\n", port.c_str());

    // Expand the path to the config file
    if( EOK == FixUpPathString( cfgFile))
    {
    // Initialize "static" command line arguments to child process
    //////////////////////////////////////////////////////////////

    // What parent to respond to after startup
    parentCmd = "-P" + GetProcessName();
    // What config to load config data from
    cfgFile = "-f" + GetConfigFile();
        // Port name argument passed to child process
        portCmd = "-p" + port;

    Log( LOG_DEV_DATA, "\tSpawning %s -vf %s  %s %s\n", portDrv->drvName.c_str(),
         cfgFile.c_str(), parentCmd.c_str(), portCmd.c_str());
        portDrv->drvPid=spawnl( P_NOWAIT,
                                portDrv->drvName.c_str(),   // Process name
                                portDrv->drvName.c_str(),   // Arg 0
                                "-vf",                      // Arg 2
                                cfgFile.c_str(),            // Arg 3
                                parentCmd.c_str(),          // Arg 4
                                portCmd.c_str(),            // Arg 5
                                NULL);
        if( portDrv->drvPid == -1)
        {
        Log( LOG_ERRORS, "\tError spawning %s: %s\n",
             portDrv->drvName.c_str(), strerror( errno));
            driverRunning = false;
        }
        else
        {
            int     drvrCoid;
            struct _pulse pulse;
            struct sigevent event;
            uint64_t timeout = mSEC_nSEC( 10000);   // 10 seconds
            uint64_t startTime, curTime;
            struct timespec timerSpec;

            Log( LOG_DEV_DATA, "Wait for %s (%s), pid %d\n", portDrv->drvName.c_str(), portDrv->regName.c_str(), portDrv->drvPid);

            // Get the time at which we launched the driver
            clock_gettime( CLOCK_REALTIME, &timerSpec);
            startTime = timespec2nsec( &timerSpec);

            // Use a 10 second timeout waiting for a pulse from the driver
            event.sigev_notify = SIGEV_UNBLOCK;
            TimerTimeout( CLOCK_REALTIME,
                          _NTO_TIMEOUT_RECEIVE,
                          &event, &timeout, NULL);

            // Wait for driver to tell us he is up and running
            while( MsgReceive(m_nameId->chid, &pulse, sizeof( pulse), NULL) != -1)
            {
                // Get the current time
                clock_gettime( CLOCK_REALTIME, &timerSpec);
                curTime = timespec2nsec( &timerSpec);

                // If we have waited long enough for the driver to register
                if( (curTime-startTime) >= timeout)
                {
                    Log( LOG_ERRORS, "\tPort driver %s failed to register '%s' in time: %s\n",
                         portDrv->drvName.c_str(), portDrv->regName.c_str(), strerror( errno));
                driverRunning = false;
                    break;
                }
                // Check if he is up and running
                else if( (drvrCoid=name_open( portDrv->regName.c_str(), 0)) != -1)
                {
                    Log( LOG_DEV_DATA, "Port driver %s registered name '%s' in time (drvrCoid = %d)\n",
                         portDrv->drvName.c_str(), portDrv->regName.c_str(), drvrCoid);
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
    }
    else
    {
        Log( LOG_ERRORS, "Invalid configuration file name: %s\n", cfgFile.c_str());
        driverRunning = false;
    }

    Log( LOG_FN_ENTRY, "Exit SerialServer::LaunchPortDriver(%s), running=%d\n", port.c_str(), driverRunning);

    return( driverRunning);
    }

/**
 * Commands the port drivers to register
 */
void SerialServer::RegisterPortDrivers()
{
    LogPortDrvMapCItr_t drvItr;

    // Loop through all the enabled logical port
    for( drvItr=m_portDrvMap.begin(); drvItr!=m_portDrvMap.end(); drvItr++)
    {
        // Port driver properties object is the second item of the pair
        LogPortDrv_t    *portDrv = drvItr->second;

        RegisterPortDriver( portDrv);
    }
}

/**
 * Commands the given port driver to register
 */
bool SerialServer::RegisterPortDriver( const LogPortDrv_t *portDrv)
{
    int drvrCoid;
    bool retVal = false;

    Log( LOG_FN_ENTRY, "Enter SerialServer::RegisterPortDriver(%s)\n", portDrv->portName.c_str());

        if( (drvrCoid=name_open( portDrv->regName.c_str(), 0)) != -1)
        {
            MsgSendPulse( drvrCoid, SIGEV_PULSE_PRIO_INHERIT, REGISTER_PULSE, 0);
        Log(LOG_DEV_DATA, "Sent REGISTER pulse to %s (drvrCoid = %d)\n", portDrv->drvName.c_str(), drvrCoid);
            name_close( drvrCoid);
        retVal = true;
    }
        else
        {
            Log(LOG_ERRORS, "\tCould NOT find %s to send REGISTER pulse\n", portDrv->drvName.c_str());
        }

    Log( LOG_FN_ENTRY, "Exit SerialServer::RegisterPortDriver(%s)\n", portDrv->portName.c_str());

    return( retVal);
}

/**
 * Relaunches and registers the given port driver if it has died
 */
void SerialServer::RelaunchPortDriver( LogPortDrv_t *portDrv)
{
    struct stat     drvStat;
    char                pidPath[128];
    const std::string   portName = portDrv->portName;

    Log( LOG_FN_ENTRY, "Enter SerialServer::RelaunchPortDriver(%s)\n", portDrv->portName.c_str());

    // Check to see if we need to relaunch this port driver
    if( true == portDrv->relaunch)
    {
        // Check to see if this port driver died
        sprintf( pidPath, "/dev/LogicalPorts/%s", portDrv->portName.c_str());
        if( -1 == stat( pidPath, &drvStat))
        {
            Log( "Relaunching port driver %s because it is no longer running\n", portName.c_str());
            // Relaunch this driver
            if(LaunchPortDriver( portDrv))
            {
                Log( LOG_ERRORS, "\tPort driver %s relaunched, register it...\n", portName.c_str());
                // Register the driver
                if( true == RegisterPortDriver( portDrv))
                {
                    Log( LOG_ERRORS, "\tPort driver %s registered\n", portName.c_str());
                }
                else
                {
                    Log( LOG_ERRORS, "\tError registering port driver %s\n", portName.c_str());
                }
            }
            else
            {
                Log( LOG_ERRORS, "\tError launching port driver %s\n", portName.c_str());
            }
        }
        else
        {
            Log( LOG_DEV_DATA, "\tDriver %s still running\n", portName.c_str());
        }
    }
    else
    {
        Log( LOG_DEV_DATA, "\tDriver %s not configured to relaunch\n",
             portDrv->portName.c_str());
    }

    Log( LOG_FN_ENTRY, "Exit SerialServer::RelaunchPortDriver(%s)\n", portDrv->portName.c_str());
}

/**
 * Method used to handle a command line argument. Called from
 * ParseCommandLine() for each comamnd line argument encountered.
 *
 * @param option The command line option (i.e. 'f' if -f specified on command line)
 * @param value  Value associated with the command line option being processed.
 *               May be NULL is no argument is associated with the command line
 *               option
 *
 * @return true if the argument was processed, false if not recognized
 */
bool SerialServer::HandleCommandLineArg(int option, const char *value)
{
    bool    retVal = true;

    Log( LOG_FN_ENTRY, "Enter SerialServer::HandleCommandLineArg( %c, '%s')\n", option, optarg == NULL ? "" : optarg);

    switch( option)
    {
    case 'R':
        m_waitForRegister = false;
        break;
    default:
        retVal = ResManagerBase::HandleCommandLineArg( option, value);
        break;
    }
    Log( LOG_FN_ENTRY, "Exit SerialServer::HandleCommandLineArg( %c, '%s')\n", option, optarg == NULL ? "" : optarg);

    return( retVal);
}


/**
 * Kills and waits for each of the logical port drivers we launched
 */
void SerialServer::KillPortDrivers()
{
    LogPortDrvMapItr_t  itr;
    LogPortDrv_t        *portDrv;
    int                 loopCount=0, coId;

    // Un-install our SIGCHLD handler while we terminate the port drivers
    signal( SIGCHLD, SIG_DFL);

    for( itr=m_portDrvMap.begin(); itr!=m_portDrvMap.end(); itr++)
    {
        portDrv = itr->second;
        // Send a pulse to the port driver
        coId = name_open( portDrv->regName.c_str(), 0);
        if( coId != -1)
        {
            Log( LOG_DEV_DATA, "Terminating %s\n", portDrv->regName.c_str());
            kill( portDrv->drvPid, SIGQUIT);
            name_close( coId);
            loopCount = 0;

            Log( LOG_DEV_DATA, "Waiting for %s to terminate\n", portDrv->regName.c_str());
            while( (coId=name_open( portDrv->regName.c_str(), 0)) != -1)
            {
                name_close( coId);
                delay( 100);

                if( loopCount++ > 100)
                {
                    Log( LOG_ERRORS, "\tTimeout wiating for %s to terminate\n",
                         portDrv->regName.c_str());
                    break;
                }
            }
        }
        else
        {
            Log( LOG_ERRORS, "Unable to locate port driver %s\n",
                 portDrv->regName.c_str());
        }
    }

    m_portDrvMap.clear();
    // Set flag to false so we do not reply to a register message
    m_okToRegister.Acquire();
    m_okToRegister.Signal( false);
    m_okToRegister.Release();
}


/**
 * Function invoked in response to a signal to our process
 *
 * @param SigNo  Siagnal we caught
 */
void SerialServer::SigChildHandler( int SigNo)
{
    SerialServer *ioMgr = (SerialServer *)ResManagerBase::m_ioMgrForOcb;

    ioMgr->Log( LOG_FN_ENTRY, "Enter SerialServer::SigChildHandler(%d)\n",  SigNo);
    // Lock this operation
    if( pthread_mutex_lock( &ResManagerBase::m_signalMtx) == EOK)
    {
        // If one of the port drivers has died
        if( SIGCHLD == SigNo)
        {
            LogPortDrvMapItr_t  drvItr;

            // Loop through our port drivers to see which (if any) have died
            for( drvItr=ioMgr->m_portDrvMap.begin(); drvItr!=ioMgr->m_portDrvMap.end(); drvItr++)
            {
                // Port driver properties object is the second item of the pair
                LogPortDrv_t    *portDrv = drvItr->second;

                ioMgr->RelaunchPortDriver( portDrv);
            }
        }
        pthread_mutex_unlock( &ResManagerBase::m_signalMtx);
    }
    ioMgr->Log( LOG_FN_ENTRY, "Exit SerialServer::SigChildHandler(%d)\n", SigNo);
}

