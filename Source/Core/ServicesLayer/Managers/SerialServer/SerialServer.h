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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/SerialServer/SerialServer.h $
// 
// 5     10/30/07 4:05p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     5/24/06 2:56p Cward
// Updated Serial Server to allow reloading of config files without
// restarting
//
// 1     5/22/06 11:13a Rwiersem
// Made the test class a friend.  Added HandlePulse() and
// LoadAdditionalConfigurationItems() which are used to reload the
// configuration without restarting the process.  Added
// m_relaunchOnConfigReload so the unit tests don't need the full system
// running.
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
// 2     10/24/03 1:30p Khykin
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
// 1     6/12/03 4:23p Khykin
// Initial checkin.
//
// 2     5/19/03 5:22p Khykin
// Updated with Brians latest changes.
//
// 1     4/30/03 4:40p Bmeinke
// Migration to new personalized configuration management structure
//
// 13    4/17/03 3:38p Bmeinke
// Restructured config file to remove LogicalportMap and BoardConfig nodes
// Added -R command line option to allow immediate registration instead of
// waiting for the Register XML command
// Update dcommand line processing to new HandleCommandLineArg() method
//
// 12    3/06/03 10:37p Bmeinke
// Added RegisterhPortDrivers() method to send a pulse to the logical port
// drivers when we receive the Register message
//
// 11    2/06/03 10:19a Bmeinke
// Removed unneeded include files
// Removed m_commBoards member (no longer used)
// Removed ReadBoardConfig() (no longer used)
//
// 10    10/24/02 7:57p Bmeinke
// Added support to the Reset() method to reset the individual logical
// port drivers
// Removed the Command() method (just use base class)
//
// 9     6/21/02 3:05p Bmeinke
// Changed to ResManagerXml in order to properly process the system
// Register message
// Added numerous overrides in order to properly act as a ResManagerXml
// (override methods we do not support)
// Removed the m_nameId ivar; we can now use the m_nameId of the
// ResManagerXml base class
// Added a cond var (m_okToRegister); used to indicate when it is ok to
// process the system Register message (must wait for all port drivers to
// be up and running before we let the system come up)
//
// 8     2/01/02 9:51p Brian.meinke
// Removed references to UniverseClient
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
// 5     11/15/01 2:56p Brian.meinke
// ParseCommandLine() is now virtual
//
// 4     11/14/01 11:11p Brian.meinke
// Moved LogicalPortMap_t/LogicalPortMap to LogicalPort.h
// Added a CommIrqMap_t/CommIrqMap data type to match CommCard objects to
// incoming interrupts
// Added a PathMap_t/PathMap data type to match LogicalPort objects to
// incoming I/O messages
// Added method AttachInterrupts() to attach to the necessary serial
// interrupts to handle serial I/O
// Added method DetachInterrupts() to detach the necessary serial
// interrupts to handle serial I/O
// Added m_pathMap member to track inode/LogicalPort pairs in order to
// match a LogicalPort object to an I/O message
// Override HandleResMgrPulse() in order to process interrupt pulses
// Added HandleInterrupt() method to process interrupt pulses and dispatch
// the appropriate CommCard object
// Added some meat to the IoWrite() method to pass write requests to the
// appropriate LogicalPorts
//
// 3     11/14/01 2:16a Brian.meinke
// Moved many typedefs to other files (CommCard.h, LogicalPort.h)
// Added ConnectLogicalPortPaths() to adopt paths for each of the logical
// ports (as read from config file)
// Add Initialize() method in order to read board and port configurations
// Added ReadBoardConfig()
// Added ReadLogicalPortSetup()
// Added ReadLogicalPortMap()
//
// 2     11/09/01 5:43p Brian.meinke
// First executable version
//
// 1     11/08/01 9:18p Brian.meinke
// Serial Communication Server process
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _SERIAL_SERVER_H_INCLUDED
#define _SERIAL_SERVER_H_INCLUDED

/*******************
 Forward definitions
 ******************/
class SerialServer;

/*******************
 include files
 ******************/
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

#include "ResManagerXml.h"      // Resource manager interface

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include <map>
#include <set>

#include "BepSync.h"            // BepCondVar
#include "SerialDefs.h"         // Serial data type definitions
#include "ILogicalPort.h"       // In order to reset logical port drivers

/*******************
 data types
 ******************/

/**
 * Data type used to hold attributes for a logical port driver
 * program
 */
typedef struct _LogPortDrv_t
{
    /**
     * Name of the logical port relative to /dev/LogicalPorts/
     */
    string  portName;
    /**
     * Name of the executable to be launched for this logical port driver
     */
    string  drvName;
    /**
     * Name the logical port driver will register (i.e. name_attach())
     */
    string  regName;
    /**
     * Path the logical port driver will adopt (i.e. /dev/logicalports/LfMotor)
     */
    string  devPath;
    /**
     * Process ID of the launched logical port driver
     */
    int     drvPid;

    /**
     * Flag indicating whether or not swe should reset the port driver when
     * we get an XML Reset message
     */
    bool    shouldReset;

    /**
     * Flag indicating whether or not swe should relaunch the port driver when
     * we get an XML Reset message (if it has died)
     */
    bool    relaunch;
} LogPortDrv_t;

/**
 * Data type used to hold the mapping of a logical port's name
 * to the driver program used for that logical port
 */
typedef map< string, LogPortDrv_t*>     LogPortDrvMap_t;
typedef LogPortDrvMap_t::iterator       LogPortDrvMapItr_t;
typedef LogPortDrvMap_t::const_iterator LogPortDrvMapCItr_t;
typedef LogPortDrvMap_t::value_type     LogPortDrvMapVal_t;

/**
 * Data type used to hold the mapping of a logical port's name
 * to the driver program used for that logical port. Has overloaded
 * methods to facilitate adding and removing contained pairs more
 * easily
 */
class LogPortDrvMap : private LogPortDrvMap_t
{
public:
    /**
     * Method used to add a new Port/Driver pair to the list
     *
     * @param portName Name of the logical port to add
     * @param regName  Name the logical port driver with register with
     * @param drvName  Name of the driver to use for the given port name
     * @param shouldReset
     *                 Flag indicating if we should send a reset pulse to this driver
     * @param relaunch Flag indicating id the driver should be re-launched if it dies
     * @param pid      Process ID of the spawned port driver
     */
    void AddDriver( const string &portName, const string &regName, const string &drvName, bool shouldReset=false, bool relaunch=false, int pid=-1)
    {
        LogPortDrv_t    *newDrv = new LogPortDrv_t;

        newDrv->portName = portName;
        newDrv->drvName = drvName;
        newDrv->regName = regName;
        newDrv->devPath = std::string( LOG_PORT_DEV_ROOT) + portName;
        newDrv->shouldReset = shouldReset;
        newDrv->relaunch = relaunch;
        newDrv->drvPid = pid;
        insert( LogPortDrvMapVal_t( portName, newDrv));
    }

    /**
     * Method used to remove a Port/Driver pair from the list
     *
     * @param portName Name of the logical port to add
     */
    void RemovePort( const string &portName)
    {
        LogPortDrvMapItr_t  itr;
        if( (itr=find(portName)) != end())
        {
            delete itr->second;
        }
        erase( portName);
    }

    void SetPid( const string &portName, int pid)
    {
        LogPortDrvMapItr_t  itr;
        if( (itr=find(portName)) != end())
        {
            itr->second->drvPid = pid;
        }
    }

    iterator begin()
    {
        return( LogPortDrvMap_t::begin());
    }

    const_iterator begin() const
    {
        return( LogPortDrvMap_t::begin());
    }

    iterator end()
    {
        return( LogPortDrvMap_t::end());
    }

    const_iterator end() const
    {
        return( LogPortDrvMap_t::end());
    }

    iterator find( const key_type& key )
    {
        return( LogPortDrvMap_t::find( key));
    }

    const_iterator find( const key_type& key ) const
    {
        return( LogPortDrvMap_t::find( key));
    }

    size_type size()
    {
        return( LogPortDrvMap_t::size());
    }

    void clear()
    {
        LogPortDrvMapItr_t  itr;
        for( itr=begin(); itr!=end(); itr++)
        {
            delete itr->second;
            itr->second = NULL;
        }

        LogPortDrvMap_t::clear();
    }
};


/**
 * Serial server class. Used to manager communication (serial/parallel)
 * resources
 *
 * @author Brian Meinke
 */
class SerialServer : public ResManagerXml
{
    friend class SerialServerTest;

public:
    /**
     * Default class constructor
     */
    SerialServer();

    /**
     * Class constructor with command line arguments
     *
     * @param argc   Number of arguments to parse
     * @param argv   Array of command line arguments
     */
    SerialServer( int argc, char *argv[]);

    /**
     * Class destructor
     */
    ~SerialServer();

    /**
     * Perform any server initialization
     *
     * @return true if successful
     *         false if error
     */
    bool InitManager();

    /**
     * Performs any necessary manager cleanup (free memory, detach any
     * adopted path names, etc)
     */
    virtual void CleanupManager();

    /**
     * Reads configuration data from the given XML node
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Will handle pulse if overriden.
     *
     * @param code   Pulse code
     * @param value  Pulse value
     * @return If pulse is handled.
     *         Handled     == BEP_STATUS_SUCCESS
     *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         Success, Fail
     * @since Version 2.5
     */
    virtual const std::string Register(void);


    /**
     * Request to read a data tag.
     *
     * @param tag  Data to be read
     * @param rate The rate to read the data.
     * 
     * @return Value of tag.
     */
    virtual const std::string Read(const std:: string &tag, const INT32 rate) {
        return( BepComponent::Read( tag, rate));
    }
    /**
     * Request to write a data tag
     *
     * @param tag    Tag to be read
     * @param value  Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Write(const std:: string &tag, const std:: string &value) {
        return( BepComponent::Write( tag, value));
    }
    /**
     * Request to be removed from the list to be notified
     * of a change in a data tag
     *
     * @param tag     Tag for who is requesting to be unsubscribed.
     * @param Process The process name of who wanted the data
     * @return Status of operation
     *         BEP_SUCCESS_RESPONSE == Subscriber successfully removed
     *         BEP_FAILURE_RESPONSE == Error removing subscriber
     * @since Version 2.0
     */
    virtual const std::string Unsubscribe(const std:: string &tag, const std:: string &Process) {
        return( BepComponent::Unsubscribe( tag, Process));
    }
    /**
     * Handle the subscribed for data by the process.
     *
     * @param tag    Tag that was subscribed for
     * @param value  New data value of the tag
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 2.1
     */
    virtual const std::string Publish(const std:: string &tag, const std:: string &value) {
        return( BepComponent::Publish( tag, value));
    }

    /**
     * Method used to set all of the objects in the component
     * to their original states.  This would includes all
     * member variables such as buffers, lists, and stored
     * data.
     * @since Version 1.0
     */
    virtual void Reset(void);

    /**
     * Request to be inserted into the list to be notified
     * of a change in a data tag
     *
     * @param tag      Tag for who is requesting the subscription informaiton.
     * @param Process  The process name of who wants the data
     *               requested.
     * @return Status of operation
     *         BEP_SUCCESS_RESPONSE == Subscriber successfully added
     *         BEP_FAILURE_RESPONSE == Error adding subscriber
     * @since Version 2.0
     */
    virtual const std::string Subscribe(const std:: string &tag, const std:: string &Process) {
        return( BepComponent::Subscribe( tag, Process));
    }

protected:

    /**
     * Load the additional configuration items needed by the serial server.
     * The "standard" items will be handled by the call to
     * ResManager::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);

    /**
     * Sends a reset command to the given port driver
     *
     * @param portDrv Port driver to reset
     * @return true if successful, false on error
     * @since Version 1.0
     */
    bool ResetPortDriver( const LogPortDrv_t *portDrv);

    /**
     * Reads the logical-to-physical port mapping table from the
     * specified XML node
     *
     * @param docNode Reference to a parsed configuration document.
     * @return true if successful, false on error
     * @since Version 1.0
     */
    bool ReadLogicalPortMap( const XmlNode *docNode);

    /**
     * Reads the data tag names and properties
     *
     * @param setupNode Node to use as the root DATA_MAP node
     * @return 0 if successful
     *         -1 if error
     */
    virtual int ReadDataMap(const XmlElement *setupNode) {
        return( 0);
    }

    /**
     * Expands environment variable names in the port driver name
     *
     * @param portDrv Path of the port driver to be cleaned up
     */
    void FixUpDriverPath( string &portDrv);

    /**
     * Launches the necessary logical port driver processes
     */
    void LaunchPortDrivers();

    /**
     * Launches the given port driver processes
     *
     * @param portDrv Port driver to launch
     * @return true if successful, false on error
     */
    bool LaunchPortDriver(LogPortDrv_t  *portDrv);

    /**
     * Commands the prot drivers to register
     */
    void RegisterPortDrivers();

    /**
     * Commands the given port driver to register
     */
    bool RegisterPortDriver( const LogPortDrv_t *portDrv);

    /**
     * Relaunches and registers the given port driver if it has died
     */
    void RelaunchPortDriver( LogPortDrv_t *portDrv);

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
    virtual bool HandleCommandLineArg(int option, const char *value);

    /**
     * Kills and waits for each of the logical port drivers we launched
     */
    void KillPortDrivers();

    /**
     * Function invoked in response to a signal to our process
     *
     * @param SigNo  Siagnal we caught
     */
    static void SigChildHandler( int SigNo);

    /**
     * See which channel to wait on
     *
     * @return Channel id to wait for messages on
     */
    virtual int GetWaitChid() {
        int retVal = -1;

        if( m_nameId != NULL)
        {
            retVal = m_nameId->chid;
        }
        return( retVal);
    }

private:

    /**
     * Mapping of logical port name to port driver executable name
     */
    LogPortDrvMap       m_portDrvMap;

    /**
     * Flag used to indicate if it is OK to process the system 'register'
     * message. We have to wait for all the logical port drivers to be
     * up and running before we can reply to the register message.
     */
    BepCondVar<bool>    m_okToRegister;

    /**
     * Flag to indicate whether or not we need to wait for the
     * register pulse
     */
    bool                m_waitForRegister;

    /**
     * Flag to indicate whether or not to relaunch the logical ports when the
     * reload configuration pulse is received.  This allows us to do the unit
     * tests without the full system running.
     */
    bool                m_relaunchOnConfigReload;
};


#endif // _SERIAL_SERVER_H_INCLUDED
