//******************************************************************************
// Description:
//    System Monitor Task
//
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Servers/SystemMonitor/Include/SystemMonitor.h $
// 
// 9     10/31/07 10:08a Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 8     2/22/07 11:18a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added SetSystemMonitorRegistered() and IsSystemMonitorRegistered().
// - Added m_sysMonRegistered.
// - Added m_useFaultServer.
// 
// 7     1/18/07 3:10p Rwiersem
// Changes for the 07011901 core release:
//                                                
// - Made the test class a friend.
// - Added plcSystemIsReady, lfMotorOperationalFault,
// rfMotorOperationalFault, lrMotorOperationalFault,
// rrMotorOperationalFault, ltMotorOperationalFault,
// rtMotorOperationalFault, and motorCommFault to the ControlData class.
// - Added a priority level parameter to DisplayPrompt() and
// RemovePrompt().
// - Added m_plcSendsPLCSystemIsReady.
// 
// 6     4/13/06 4:28a Cward
// Added an assignment operator and an Assign() method to the ControlData
// class
//
// 2     3/20/06 11:00a Bmeinke
// Added an assignment operator and an Assign() method to the ControlData
// class
//
// 5     2/15/06 8:04p Cward
// made methods virtual to aid in unit testing
//
// 4     7/20/05 9:25p Cward
// New Control Data object scheme.
//
// 3     4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 2     11/15/04 11:13a Bmeinke
// Added an abortPb member to the ControlData class
//
// 2     7/21/04 5:35p Cward
// Updated Module comment header block
//
// 1     7/21/04 10:18a Cward
//
// 3     4/28/04 10:54p Cward
// Changes associated with build 040402801
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:42p Cward
//
// 4     10/24/03 1:51p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 2     10/10/03 10:20a Khykin
// Added Zerospeed to the control data.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/07/03 8:08a Khykin
// Updated with latest changes from Flatrock.
//
// 1     6/26/03 5:40p Khykin
// Initial checkin.
//
// 4     6/26/03 3:57p Khykin
// Updated with VMESystem is ready logic.
//
// 3     6/24/03 7:49a Khykin
// Adding the ability to write the files.
//
// 2     6/20/03 8:51a Khykin
// Updated for Ford changes.
//
// 1     6/12/03 4:06p Khykin
// Initial checkin.
//
// 3     5/19/03 12:08p Khykin
// Updated cable connect logic to debounce and abort if lost during a
// vehicle test.
//
// 2     4/30/03 10:31a Khykin
// Added reprinting functionality.
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:47  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:10a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 16    11/22/02 3:17p Khykin
// Added flag so that a valid VIN is not required for start (KOREA)
//
// 15    10/21/02 12:01p Khykin
// Added flags for LC prompting and support in the DisplayPrompt method to
// blank the screen.
//
// 14    8/23/02 8:35a Kh
// Synchronizing the pit machine with VSS.
//
// 12    5/24/02 3:05p Kh
// Converting to a BepServer to deal with deadlock issues.
//
// 11    4/25/02 4:34p Bmeinke
// Added vehicle present hysterisis logic
//
// 10    3/19/02 12:14p Kh
// Updating due to changes with BepComponent.
//
// 9     2/27/02 3:16p Bmeinke
// Made m_dataBroker, m_promptServer and m_faultServer objects dynamically
// allocated so they will find their respective servers properly upon
// instantiation
//
// 8     2/19/02 8:05p Bmeinke
// Modified the PublishedTagList class so it can be used for Subscription
// data items as well as items we publish
// Added new rate argument to Read() (it was added to BepComponent)
//
// 7     2/06/02 1:56p Bmeinke
// Removed references to testEnabled
//
// 6     2/06/02 1:20p Bmeinke
// Removed references to calCylEnabled and simCylEnabled
//
// 5     2/01/02 10:32p Brian.meinke
// Cleaned up command line argument processing
// Added cable connect logic
// Added support for the RESET command
// Modified DisplayPrompt() to jive with the IPromptServer object's
// interface
// Added MINIMAL support for IFaultServer interface
//
// 4     12/21/01 12:56p Brian.meinke
// Changed SYS_MON_NAME from rolls/SystemMonitor to core/SystemMonitor
//
// 3     10/31/01 2:12p Brian.meinke
// Moved implementation of Terminate() to cpp file
//
// 2     10/17/01 2:36p Brian.meinke
// Added SYS_MON_PULSE_CODE for private pulse codes
//
// 1     9/19/01 11:26p Brian.meinke
// System Monitor Task (similar to PDOS house keeping task)
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef SYS_MON_INCLUDED
#define SYS_MON_INCLUDED
//==============================================================================


/*******************
 include files
 ******************/
#include <map>
#include <string>
#include <set>
#include <sys/netmgr.h>

#include "BepDefs.h"
#include "BepServer.h"
#include "INamedDataBroker.h"
#include "IPromptServer.h"
#include "IFaultServer.h"
#include "tchar.h"
#include "BepTimer.h"
#include "BepSemaphore.h"

/**
 * Pulse code used for private SystemMonitor pulses
 */
#define SYS_MON_PULSE_CODE      SYSTEM_PULSE+1

/**
 * Pulse value used to indicate we need to re-evaluate control stuff.
 * Typically issued in response to receiving updated data for stuff
 * we have subscribed for.
 */
#define PROCESS_CONTROL_PULSE   0

/**
 * Pulse value used to indicate that the VehiclePresence status needs
 * to be re-evaluated.
 */
#define VEHICLE_PRESENT_PULSE   10

/**
 * Pulse value used to indicate that the Loss Compensation switch status
 * needs to be re-evaluated.
 */
#define LOSS_COMPENSATION_PULSE 20

/**
 * Pulse value used to indicate that the Cable Connect needs to be
 * re-evaluated.
 */
#define CABLE_CONNECT_PULSE     30

/**
 * Pulse value used to indicate that it is time to turn off the wheelbase
 * adjustment pulse.
 */ 
#define STOP_WB_ADJUST_PULSE    90

/**
 * Name to use when registering with OS
 * @since version 1.0
 */
#define SYS_MON_NAME            "core/SystemMonitor"

/**
 * The KeyPress value used to start a test.
 */
#define PENDANT_START_TEST              "Pass"

/**
 * The VINReadStatus indicates a valid VIN has been read
 */
#define VALID_VEHICLE_VIN               "Green"

/**
 * The VINReadStatus indicates an invalid VIN has been read
 */
#define INVALID_VEHICLE_VIN             "Red"

/**
 * Control logic data
 *
 * @author Brian Meinke
 * @version 1.0
 */
class ControlData
{
    friend class SystemMonitorTest;

public:

    /**
     * Allocate a new ControlData object. Required due to non-public
     * constructor
     *
     * @return A new ControlData object
     */
    static ControlData* Allocate();

    /**
     * Class destructor
     */
    virtual ~ControlData();

    /**
     * Assignment operator
     *
     * @param rhs    Object to be copied
     *
     * @return this
     */
    ControlData& operator=( const ControlData& rhs);

    string  vehVinReadStatus;
    bool    startTestPb;
    bool    startTestCommand;

    bool    vehiclePresent;
    bool    rollsDown;
    bool    rollsUp;

    bool    cableConnect6Pin;
    bool    cableConnect8Pin;
    bool    cableConnect16Pin;
    bool    cableConnect;

    bool    calSwitch;
    bool    calSwitchToPLC;
    bool    calCycle;
    bool    calInProgress;
    bool    calConditionsCorrect;
    bool    calKeyCycled;

    bool    simSwitch;
    bool    simCycle;
    bool    simInProgress;

    bool    testStarted;
    bool    testInProgress;
    bool    testRequested;

    string  inputServerState;
    string  keyPress;

    bool    powerSense;
    bool    rackFault;
    bool    machineFault;
    bool    wheelbaseInPosition;
    bool    vmeSystemReady;
    bool    zerospeed;

    bool    plcSystemIsReady;

    bool    lfMotorOperationalFault;
    bool    rfMotorOperationalFault;
    bool    lrMotorOperationalFault;
    bool    rrMotorOperationalFault;
    bool    ltMotorOperationalFault;
    bool    rtMotorOperationalFault;

    bool    motorCommFault;
    bool    reprintRequest;
    bool    abortPb;

protected:

    /**
     * Assigns the value of rhs to this
     *
     * @param rhs    ControlData object to be copied
     *
     * @return this
     */
    virtual ControlData& Assign( const ControlData& rhs);

    /**
     * Default constructor: MUST NOT BE PUBLIC!!!!!!!
     * The base SystemMonitor class will instantiate a ControlData
     * object at various points in the code. Because SystemMonitor-
     * derived classes can use a different ControlData-derived class
     * for data management, we CANNOT assume that the SystemMonitor
     * base class will be using a base ControlData object. By
     * makeing the constructor private, any ControlData objects are
     * forced to be allocated through the allocator methods. It
     * is declared as protected so that derived classes have access
     * to it.
     */
    ControlData();

};

/**
 * System Monitor class.
 * Handles system events: Start Test push button, ABORT, VIN processing, etc
 *
 * @author Brian Meinke
 * @version 1.2
 */
class SystemMonitor : public BepServer
{
    friend class SystemMonitorTest;

public:
    /**
     * Default constructor. Set debug to false, channel ID's to -1, etc
     */
    SystemMonitor();

    /**
     * Constructor from command line args
     *
     * @param argc   Number of command line arguments
     * @param argv   Array of command line args
     */
    SystemMonitor( int argc, char *argv[]);

    /**
     * Class destructor
     */
    virtual ~SystemMonitor();

    /**
     * Process system changes until a terminate message is received.
     *
     * @param terminateFlag
     *               Flag used to terminate the system.
     * @since Version 1.1
     */
    virtual void Run(volatile bool *terminateFlag=NULL);

protected:
    /**
     * method for component initialization.
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Overloaded BepServer Publish method which provides the ability
     * to handle the fault level messages that were requested.
     *
     * @param node the published information to deal with.
     * @return the status of the operation.
     */
    virtual const std::string Publish(const XmlNode *node);

    /**
     * Request to write a data tag
     *
     * @param tag    Tag to be written
     * @param value  Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Write(const std::string &tag, const std::string &value);

    /**
     * Request to write a data tag
     *
     * @param node    Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 1.3
     */
    virtual const std::string Write(const XmlNode *node);

    /**
     * Handle pulses generated by the system monitor.
     *
     * @param code   Pulse code
     * @param value  Pulse value
     * @return If pulse is handled.
     *         Handled     == BEP_STATUS_SUCCESS
     *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);

    /**
     * Method used to terminate the component.  This performs
     * any cleanup that the destructor would not and sets
     * the state of the component to terminate.
     * @since Version 1.0
     */
    virtual void Terminate(void);

    /**
     * Monitors for system events such as ABORT, CAL, SIM, START_TEST, etc
     *
     * @param ctrl   Current value of control data needed for system control events
     */
    virtual void UpdateControl(ControlData *ctrl);

    /**
     * Reads current values of control data items. The m_subscribedData data is
     * updated asynchronously via subscription notifications from the servers to
     * whoim we are subscribed
     *
     * @param ctrl   Control Data structure to fill with values
     */
    virtual void GetCurrentControlData( ControlData *ctrl);

    /**
     * Check the machine status and set/clear any faults that
     * occur.
     *
     * @param ctrl   Current value of control data needed for system control events
     * @return The status of the machine.
     *         true - good, false - bad
     */
    virtual void CheckMachineStatus( ControlData *ctrl);

    /**
     * Vehicle presence logic
     *
     * @param ctrl        Current value of control data needed for system control events
     * @param checkStatus check the status of the vehicle presence due the debounce
     *                    time is up.
     */
    virtual void CheckVehiclePresent( ControlData *ctrl, bool checkStatus = false);

    /**
     * Cable Connect logic
     *
     * @param ctrl        Current value of control data needed for system control events
     * @param checkStatus check the status of the vehicle presence due the debounce
     *                    time is up.
     */
    virtual void CheckCableConnect( ControlData *ctrl, bool checkStatus = false);

    /**
     * Test ABORT logic
     *
     * @param ctrl Current value of control data needed for system control events
     */
    virtual void CheckAbort( ControlData *ctrl);

    /**
     * Changes state of Drivers Monitor and put up some prompts
     *
     * @param ctrl Current value of control data needed for system control events
     */
    virtual void UpdateDriverMonitor( ControlData *ctrl);

    /**
     * Check to determine if a reprint request has been
     * received.
     *
     * @param ctrl Current value of control data needed for system control events
     */
    void CheckReprintRequest( ControlData *ctrl);

    /**
     * Loss Compensation control logic
     *
     * Note: The calibration switch is debounced by the System Monitor.  The debounced
     * signal is: LossCompensationSwitchToPLC
     *
     * @param ctrl        Current value of control data needed for system control events
     * @param checkStatus check the status of the loss compensation switch due
     *                    the debounce time is up.
     */
    void CheckLossCompensation( ControlData *ctrl, bool checkStatus = false);

    /**
     * Simulator control logic
     *
     * @param ctrl Current value of control data needed for system control events
     */
    void CheckSimulate( ControlData *ctrl);

    /**
     * Testing control logic
     *
     * @param ctrl Current value of control data needed for system control events
     */
    virtual void CheckTesting( ControlData *ctrl) = 0;

    /**
     * Displays a prompt for the driver.
     *
     * @param whichBox     Prompt box index to put prompt into
     * @param promptTag    Tag for prompt message to be displayed
     * @param color        The color to set the prompt box background to.
     * @param removePrompt Flag to indicate if the previous prompt should be removed.
     * @param priorityLevel
     *                  Priority level of the prompt (0=Lowest, 9=Highest).
     */
    virtual void DisplayPrompt(INT32 whichBox, const char *promptTag, 
                               const char *color = NULL, 
                               bool removePrompt = true, 
                               INT32 priorityLevel = MIN_PRIORITY);

    /**
     * Displays a prompt for the driver.
     *
     * @param whichBox     Prompt box index to put prompt into
     * @param promptTag    Tag for prompt message to be displayed
     * @param removePrompt Flag to indicate if the previous prompt should be removed.
     * @param priorityLevel
     *                  Priority level of the prompt (0=Lowest, 9=Highest).
     */
    virtual void DisplayPrompt(INT32 whichBox, const char *promptTag, 
                               bool removePrompt, 
                               INT32 priorityLevel = MIN_PRIORITY);

    /**
     * Removes a driver prompt message
     *
     * @param whichBox  Prompt box index to remove prompt from
     * @param promptTag Tag for prompt message to be displayed
     * @param color     The color to set the prompt box background to.
     * @param priorityLevel
     *                  Priority level of the prompt (0=Lowest, 9=Highest).
     */
    virtual void RemovePrompt(INT32 whichBox, const char *promptTag, 
                              const char *color = NULL, 
                              INT32 priorityLevel = MIN_PRIORITY);

    /**
     * Reads a named data item via the named data borkerinterface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return Value requested converted to bool
     */
    bool& ReadNdbData( const char *tag, bool &val);

    /**
     * Reads a named data item via the named data borkerinterface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return Value requested converted to char
     */
    char& ReadNdbData( const char *tag, char &val);

    /**
     * Reads a named data item via the named data borkerinterface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return Value requested converted to int
     */
    int& ReadNdbData( const char *tag, int &val);

    /**
     * Reads a named data item via the named data borkerinterface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return Value requested converted to long
     */
    long& ReadNdbData( const char *tag, long &val);

    /**
     * Reads a named data item via the named data borkerinterface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return Value requested converted to float
     */
    float& ReadNdbData( const char *tag, float &val);

    /**
     * Reads a named data item via the named data borker interface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return String value requested
     */
    string& ReadNdbData( const char *tag, string &val);

    /**
     * Reads a named data item via the named data borker interface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return the value requested
     */
    bool& ReadNdbData( const string &tag, bool &val);

    /**
     * Reads a named data item via the named data borker interface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return the value requested
     */
    char& ReadNdbData( const string &tag, char &val);

    /**
     * Reads a named data item via the named data borker interface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return the value requested
     */
    int& ReadNdbData( const string &tag, int &val);

    /**
     * Reads a named data item via the named data borker interface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return the value requested
     */
    long& ReadNdbData( const string &tag, long &val);

    /**
     * Reads a named data item via the named data borker interface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return the value requested
     */
    float& ReadNdbData( const string &tag, float &val);

    /**
     * Reads a named data item via the named data borker interface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return the value requested
     */
    string& ReadNdbData( const string &tag, string &val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    virtual INT32 WriteNdbData( const char *tag, const string &val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const char *tag, bool val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const char *tag, char val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const char *tag, int val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const char *tag, long val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const char *tag, float val);

    /**
     * Writes a named data item via the named data borker interface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const string &tag, const string &val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const string &tag, bool val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const string &tag, char val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const string &tag, int val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const string &tag, long val);

    /**
     * Write a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 WriteNdbData( const string &tag, float val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     *
     * @return The status of the operation.
     */
    virtual INT32 CommandNdbData( const char *tag, const string &val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 CommandNdbData( const char *tag, bool val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 CommandNdbData( const char *tag, char val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
    * @return The status of the operation.
     */
    INT32 CommandNdbData( const char *tag, int val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 CommandNdbData( const char *tag, long val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 CommandNdbData( const char *tag, float val);

    /**
     * Command a named data item via the named data borker interface
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 CommandNdbData( const string &tag, const string &val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 CommandNdbData( const string &tag, bool val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 CommandNdbData( const string &tag, char val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 CommandNdbData( const string &tag, int val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 CommandNdbData( const string &tag, long val);

    /**
     * Command a value to the server that manages the specified system
     * data.
     *
     * @param tag    Tag of the data item to read
     * @param val    Value of the tagged item requested (returned from server)
     * @return The status of the operation.
     */
    INT32 CommandNdbData( const string &tag, float val);

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
     * Special processing to be done to each node.  Process and description 
     * attributes are added to each node.
     *
     * @param node   Node to add attributes to.
     * @param type   Type of operation we are performing.
     * @param server The server that requested the special processing.
     */
    virtual void SpecialProcessing(XmlNode *node, const std:: string type, BepServer *server);

    /**
     * Process the fault that was received.
     *
     * @param fault  The fault that was received.
     * @return The status of the system.
     *         true - system updated
     *         false - system not updated
     * @since Version 1.1
     */
    bool ProcessFault(const XmlNode *fault);

    /**
     * Wait for a change in the system data and update the system
     * with the results.
     * @since Version 1.1
     */
    void WaitForChange(void);

    /**
     * Notify the System Monitor that information has changed
     * and needs to be re-examined.
     * @since Version 1.1
     */
    void NotifyDataChanged(void);

    /**
     * Allocates a new ControlData structure to be used to maintain the
     * current state of the system
     *
     * @return
     */
    virtual ControlData* ControlAllocate() const;

    /**
     * Frees a ControlData structure previously allocated via ControlAllocate
     *
     * @param controlData
     *               ControlData structure previously allocated via ControlAllocate
     */
    virtual void ControlFree(ControlData* &controlData) const;

    /**
     * Sets registered status
     * @param isSysMonRegistered bool indicates registered status
     */
    void SetSystemMonitorRegistered(bool isSysMonRegistered);

    /**
     * Checks flag to see if system monitor is already
     * registered
     *
     * @return true if systen monitor has already registered
     */
    bool IsSystemMonitorRegistered(void);

    /**
     * Get flag to denote if SysMon is responsible for
     * adjusting the wheelbase.
     * 
     * @return true if SysMon is responsible for wheelbase adjustment
     */
    bool const GetSystemMonitorWheelbaseAdjust(void);

    /**
     * Set flag to denote if SysMon is responsible for
     * adjusting the wheelbase.
     * 
     * @param value  bool to set as flag
     */
    void SetSystemMonitorWheelbaseAdjust(bool value);

    /**
     * Start sending the wheelbase adjustment pulse to the PLC.
     */ 
    void StartWheelbaseAdjust(void);
    /**
     * Stop sending the wheelbase adjustment pulse to the PLC.
     */ 
    void StopWheelbaseAdjust(void);

    /**
     * Interface to NamedDataBroker process.
     * Used to read and write data in the system.
     * @since Version 1.0
     */
    INamedDataBroker    *m_dataBroker;

    /**
     * Mutex used to giard against simultaneous access to the named data broker object
     */
    BepMutex            m_ndbLock;

    /**
     * Interface to the Prompt Server
     */
    IPromptServer       *m_promptServer;

    /**
     * Interface to the fault server
     */
    IFaultServer        *m_faultServer;

    /**
     * Reference to the fault information (Name, Description, ...).
     */
    XmlNode *m_faultInformation;

    /**
     * Received VIN queue.
     * vin[ 0] = vehicle currently under under test
     * vin[ 1] = 1st vehicle waiting to be tested
     * vin[ 2] = 2nd vehicle waiting to be tested
     */
    string              m_vinQueue[ 3];

    /**
     * "Previous" control data values
     */
    ControlData         *m_oldCtrl;

    /**
     * Flag to indicate if we should wait for the Register messsage before
     * doing the Register message processing (debug, mostly)
     */
    bool                m_useRegisterMssg;

    /**
     * Number of mili-seconds required for vehicle present to be asserted
     * before a vehicle should be considered really present
     */
    double              m_vehiclePresHystTime;

    /**
     * The number of mili-seconds to wait when debouncing the Loss
     * Compensation switch signal.
     */
    double              m_lcSwitchHystTime;

    /**
     * The number of mili-seconds to wait when debouncing the Loss
     * Compensation switch signal.
     */
    double              m_cableConnectHystTime;

    /**
     * The type of machine that is being monitored. "2400", "360", "3600"
     */
    string              m_machineType;

    /**
     * Timer object used to send the SystemMonitor a pulse after the
     * vehicle present filter time has elapsed.
     */
    BepTimer            m_vehiclePresentTimer;

    /**
     * Timer object used to send the SystemMonitor a pulse after the
     * loss compensation delay has elapsed.
     */
    BepTimer            m_lossCompensationTimer;

    /**
     * Timer object used to send the SystemMonitor a pulse after the
     * cable connect delay has elapsed.
     */
    BepTimer            m_cableConnectTimer;

    /**
     * Semaphore for signaling that a thread is going to update
     * the system information.change has been received
     * @since Version 1.1
     */
    BepSemaphore            *m_updateControl;

    /**
     * Flag that indicates wether or not a valid vin is needed
     * to start a test.
     * @version 1.2
     */
    bool                m_validVINNeededForStart;

    /**
     * Counter used to determine how long the cable has been disconnected.
     * @version 1.2
     */
    int                 m_cableDisconnectCounter;

    /**
     * Flag that indicates if an abort is sent when a cable disconnect is detected
     * during a test.
     * @since Version 1.2
     */
    bool                m_disconnectAbort;
    /**
     * Flag used to determine if the system should abort if
     * the PLC indicates that the Abort push button was pressed.
     *
     * Added to provide backwards support to machines that don't
     * have an abort pushbutton and can't be sure the PLC code
     * is written correctly to not set the abort pushbutton bit
     * in the PLC map.
     */
    bool                m_abortOnAbortPb;
    /**
     * Flag used to determine if the system should fault if 
     * PLCSystem is ready bit is dropped from the PLC
     * 
     * This parameter was added to provide backwards support 
     * to machines that don't have an HMI interface as the 
     * the PLC code in these systems will not send a PLCReady bit
     */
    bool                m_plcSendsPLCSystemIsReady;
    /**
     * Flag used to determine if the system monitor is registered and
     * all servers are created
     * 
     * This parameter was added to fix a bug that would allow the system to 
     * start showing prompts and setting faults before servers were created
     */
    bool                m_sysMonRegistered;
    /**
     * Flag used to determine if we should use the fault server 
     * to set and display specific machine faults
     */
    bool                m_useFaultServer;
    /**
     * Flag used to denote that it is the SystemMonitor's
     * responsibility to adjust the wheelbase on
     * publish of WheelbasePositionInchesX10.
     */
    bool                m_systemMonitorWheelbaseAdjust;     
    /**
     * The number of mili-seconds to wait before killing the wheelbase move bit
     */
    double              m_wheelbasePulseTime;   
    /**
     * Timer object used to send the SystemMonitor a pulse after the
     * vehicle present filter time has elapsed.
     */
    BepTimer            m_wheelbasePulseTimer;
};

#endif

