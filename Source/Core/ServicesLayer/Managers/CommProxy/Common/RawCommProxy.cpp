//******************************************************************************
// Description:
//  Logical serial port driver for raw serial communication
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Common/RawCommProxy.cpp $
//
// 17    1/18/07 12:51p Rwiersem
// Changes for the 07011901 core release:
//
// - Added DecodeIpAddress().
//
// 16    10/26/06 10:20a Rwiersem
// Changes for the 06102701 core release:
//
// Added Read() and Write().
//
// Added DriverBytesAvailable().
//
// Added the clientOcb parameter to TransmitMessage().
//
// 15    6/21/06 4:39p Cward
// Added ability to reload config file without rebooting
//
// 1     6/15/06 1:22p Rwiersem
// Added LoadAdditionalConfigurationItems() and HandlePulse() which are
// used to reload the configuration without restarting the process.
//
// 14    4/13/06 3:54a Cward
// Re-factored the port locking logic to remove duplicated code initialize
// some uninitialized variables added ReconnectHandler() methods and moved
// the __USAGE block to RawCommProxyMain.cpp
//
// 13    2/15/06 7:56p Cward
// 3     2/15/06 4:05p Bmeinke
// Re-factored the port locking logic to remove duplicated code
// initialize some uninitialized variables
// Added ReconnectHandler() methods
//
// 2     2/06/06 1:47p Bmeinke
// Moved the __USAGE block to RawCommProxyMain.cpp
//
// Updated for 06021501 iteration
//
// 12    11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 11    7/20/05 9:08p Cward
// Added New TaskMon support
//
// 10    4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 9     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     2/18/05 12:45p Bmeinke
// Updated Notify ClientErr.
// Only call NotifyClientErr( BlockedReader_t &) from NotifyClientErr(
// CommIoOcb_t*) if we find a blocked reader client with a matching ocb
//
// 8     2/16/05 9:25p Cward
// Updated Notify ClientErr.
//
// 2     1/17/05 10:05a Bmeinke
// Added a default catch block at the end of SetCommFault()
//
// 7     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 6     7/21/04 11:25a Cward
// Fixed check in  Send Fast Init Message that was caused by a signed  vs
// unsigned problem.
//
// 3     8/30/04 4:07p Bmeinke
// In CanAddToClientFifo(), lock the filter string list of the client OCB
// to avoid possible SIGSEG if client closes his connection while we are
// looking through his filter string list
// In OcbFinalize() lock the filter string list and clear it out
//
// 5     6/24/04 1:08a Cward
// Removed pound if 0 blocks. Updated the header block comments to the new
// standard.
//
// 2     8/17/04 6:47p Bmeinke
// Added ProcessReaderUnblockRequest() to handle unblock requests from
// blocked reader clients
// Added ProcessWriterUnblockRequest() to handle unblock requests from
// blocked writer clients
//
// 2     7/07/04 4:29p Bmeinke
// Swapped the order in which we call BposAttachName() and the base class
// Initmanager() inside InitManager()
// Added a fault server interface object to be used to report
// communication errors
// Added SetCommFault() and ClearCommFault() methods
//
// 4     4/14/04 6:42p Cward
// Send Fast Init Message needed changes so that ISO would work. When
// reflections were used the buffer length was  too long due to adding an
// additional byte. Removed the additional byte.
//
// 3     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:28p Cward
//
// 3     10/24/03 1:10p Khykin
// Updated with mutex changes.
//
// 2     10/10/03 9:39a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:35a Khykin
// Initial checkin
//
// 2     8/07/03 12:50p Khykin
// Updated with latest changes from Flatrock.
//
// 3     6/26/03 1:13p Khykin
// Updated with Brians latest changes.
//
// 4     6/18/03 6:30p Bmeinke
// NotifyClientErr() no onger takes an optional message argument
// Added snd NotifyClientErr() method which operates on BlockedReader_t
// structs
// ReadSendersBuffer() will not fail if 0 bytes are read from the clients
// message buffer
//
// 3     5/29/03 1:52p Bmeinke
// New dedicated reader thread architecture (have one thread dedicated to
// reading data from the driver)
// Added the instantiation of LogportSubscription::m_subIdLock mutex
// because it has been changed to a static data member of
// LogPortSubscription which is a header only class
// Changed calls to CancelDriverNotification() to SuspendReaderThread()
// No longer use the ILogger object inside the CommIoOcb_t structure (was
// causing SIGSEGVs)
// Check the returned subscription ID of the CommIoOcn_t::rxSubscription
// inside OcbInit()
// Changed the name of EstablishCommsHandler() to FastInitHandler()
// ObtainClientPortLock() now returns -1 to indicate that the lock request
// cannot be handled immediately (i.e. port already locked), so don't
// reply to client
// Added READER_PROCESS define to control whether or not the dedicated
// reader thread should process all received data (i.e. parse and stick in
// client fifos)
// FastInithandler() adjust the clock period from 1 ms down to .1 ms for
// better timing resolution/accuracy
// Broke FastInitHandler() into two additional methods:
// SendFastInitBreakSignal() and SendfastInitmessage() for maintainability
// Reflected bytes are now handled via cond-var value (set count before
// transmitting, decrement count when receiving)
// Moved the call to delay() (for inter-byte timing) to the end of the
// transmit loop
//
// 1     4/30/03 4:55p Bmeinke
// Migration to new personalized configuration management structure
//
// 24    4/30/03 3:38p Bmeinke
// Added ArmReplyTimeoutTimer() method
// Moved the CommIoOcb_t::messageTimeout to
// RawCommProxy::m_defaultReplyTimeout (initialized from config file)
// ReadSendersBuffer() now takes an additional io_write_t* argument and
// works properly
// Message gap timer pulses and reply timeout timer pulses have different
// pulse codes (in HandleMssgGapPulse and HandleReplyTimeoutPulse)
// Pulses used for internal signalling use SIGEV_PULSE_PRIO_INHERIT
// instead of PULSE_HI_PRIO
// CleanupManager() calls CancelDriverNotification()
// Moved the sending of the subscription notificaion pulse from
// AddToClientFifo() to UpdateClientFifos()
// Added a default reply timeout timer (m_replyTimeoutTimer)
// IoWrite() now uses ReadSendersBuffer() to read the IO_WRITE message
// from the clients address space
// Added message gap timing via m_messageGapTimer and m_lockedWriters
// JustWrite() arms the reply timeout timer immediately after transmitting
// serial data
// Lock the serial port mutex when we read serial data in response to a
// receive a notification pulse from the devc driver and do not release
// the lock until we have registered for driver notification again
// NotifyClientErr() now returns a bool to indicate if the client was in
// the blocked reader's list
// ResetConnectionHandler() no longer clears our FIFOs in the devc driver
// LowSpeedInit() no longer modifies its thread priority (need to have the
// client do this for us because of priority inheritance)
// TransmitMessage() takes an (optional) additional parameter to say
// whether or not we want it to re-subscribe for serial data (this is only
// an issue with reflective buses like ISO)
//
// 22    4/04/03 6:11p Bmeinke
// Added FlushDriverFifos() method
//
// 21    4/04/03 11:20a Bmeinke
// Added OpenPort() and ConfigurePort() virtual methods to allow more
// logical extension to ethernet comm proxies
// Changed the name of SerialDataRegister() to DriverDataRegister()
// Changed the name of CancelSerialNotification() to
// CancelDriverNotification()
//
// 20    3/24/03 1:29p Bmeinke
// Fixed the indentation on the explanation of the command line arguments
// in the usage block
//
// 19    3/19/03 6:21p Bmeinke
// The logger statement when exiting ResetCoinnectionhandler() was
// printing "Enter RawCommProxy::ResetConnectionHandler()" instead pf
// "Exit...."
//
// 18    3/18/03 6:40p Bmeinke
// Added message gap timing via m_mssgGapTimer and m_mssgGapTimerExpired
// data members
// Replaced ParseCommandLine() with HandleCommandLineArg()
// Changed m_currentPortPortProtocol from SerialProtocol_t to
// SerialProtocol (easier to work with)
// Added m_blockedWriters to manage clients blocked waiting to write to
// the port
// Methods which relied upon port protocol settings (i.e. JustWrite() and
// PosixDevCtl()) now use m_currentPortProtocol instead of
// m_configProtocol
// Added LowSpeedInit() devctl handler. Used to perform ISO 5-baud
// initialization sequences
// HandleResManagerPulse() now has to handle message gap timer pulses
// When waiting for a reflection byte in TransmitMesage(), the timeout is
// now variable based upon the current port baud rate
// Added an overloaded ReadDriverData() which takes a timeout value in
// nano-seconds
//
// 17    3/10/03 6:29p Bmeinke
// If -R command line option was specified (do not wait to register), call
// Register() at end of InitManager()
//
// 16    3/10/03 3:12p Bmeinke
// Added call to tcflush( TCIOFLUSH) to our Register() initialization
// In SerialDataRegister(), if ionotify() != -1, explicitly set status=0
// before leaving to ensure return value is correct
//
// 15    3/06/03 10:41p Bmeinke
// Added overloaded Register() method (called when we receive a
// REGISTER_PULSE)
// Moved the opening/configuring of the serial port from InitManager() to
// Register()
// Fixed bug in ReadDriverData() where we could read more data than the
// caller requested (occurred when the devc driver had more data available
// than we had room for in our read buffer)
// Added -R command line option to bypass the new registration pulse
// mechanism (i.e. pass -R command line option to Register immediately
// during initialization instead of waiting for the REGISTER_PULSE)
//
// 14    3/04/03 3:07p Bmeinke
// Wrap access to client's rxFifos with a mutex locks to avoid race
// conditions
// in ReadDriverData(), use tcischars() to check how much data is actually
// available from the driver
// Cleaned up and added comments to ReadDriverData()
// In HandleResMgrPulse(), pass a 0ms timeout to ReadDriverData()
// Changed the nanospin_ns() to a delay() in createUniqueSubscriptionId()
// (nanospin stalls off ALL threads)
//
// 13    2/21/03 7:23p Bmeinke
// Made all static member variables non-static (static obsolete because no
// more background thread used for reading serial data)
// The serial port driver is no longer opened in non-blocking mode
// Fixed the buffer overflow bug in UpdateBusCommLog()
// Addeed more accurate return values to IoWrite()
// When we receive a DCMD_CHR_TCFLUSH (tcflush()) command, reset our
// clients' FIFOs
// Added ReadDriverData() method to read serial data from the devc driver
// Hande reflection characters via direct reads from the devc driver
// (instead of using ionotify() and the thread pool)
// Removed methods related to the obsolete reader thread
//
// 12    2/11/03 12:22p Bmeinke
// Added LockForClient() and UnlockForClient() methods to perform actual
// lock/unlokc of port
// Use _RESMGR_STATUS() to return accurate status from devctl() handler
// methods
//
// 11    2/06/03 7:14p Bmeinke
// Inside TransmitMessage(), only write entire message string as a blob if
// no inter-byte gap AND no reflection processing
// Changed the logPortEstablishComm_t message structure to use a fixed
// lenght 128 byte message buffer (was a variable length message buffer)
//
// 10    2/06/03 3:51p Bmeinke
// In SerialDataRegister(), if return value from ionotify() indicates data
// already available, call tcischars() to make sure that data is available
// to read before sending a 'read data' pulse to our I/O channel
// Changed the order of port protocol initialization in InitManager()
//
// 9     2/06/03 2:54p Bmeinke
// Fixed bug in AddToClientFifo(): Was sending (possibly) more data to a
// client blocked on read() with more data than he could process
// If we have more data available for a blocked read client than the
// client can handle, add any extra data that the client can't currently
// process to his FIFO
// Changed the tcflush() command in ResetConnectionHandler() to issue a
// TCIOFLUSH instead of just a TCOFLUSH
// In TransmitMessage(), changed the amount of time we wait for reflection
// data from 500ms to 5 seconds (5 baud init reflections come back SLOW)
//
// 8     2/06/03 10:01a Bmeinke
// Initialize m_reflectionCount to 0 during initialization
// Changed SerialProtocol_t to be based on the POSIX termios structure
// Only reply to clients blocked on a read with the maximum amount of data
// they are ready to read (no necessarily the full amount ready to be
// read)
// Added helper method JustWrite() to perform some of the lower level
// writing actions
// If the size of message handled by the tcsetattr() handlers is > sizeof(
// termios), assume the data being passed in is a SerialProtocol_t
// structure
// In UnlockPortHandler(), do not release ALL of a client's port locks
// Removed SetPortProtocolHandler() and SetMssgProtocolHandler()
// Added EstablishCommsHandler() to handle ISO fast initialization
// functionality
// Cleaned up the InitializeProtocol() methods
//
// 7     1/30/03 3:33p Bmeinke
// Added 'totalRead += bytesRead' to ReadDataPortUnlocked()
// Fixed PortUnsubscribeHandler()
// Added PosixDevCtl() method to handle the POSIX terminal control
// devctl() messages
//
// 6     1/27/03 10:19p Bmeinke
// In HandleResMgrPulse(), if we read any data from the serial driver OR
// if the receive fifo has data waiting in it, call ProcessReadData() to
// process the AVT message data (in case we received extra data while
// performing AVT initialization)
//
//
// 5     1/22/03 3:23a Bmeinke
// Register process name using a root og 'logport' instead of 'mgr'
// Use new ionotify() scheme where notification pulses from the serial
// driver are delivered directly to our I/O channel instead of to the
// ReaderThread (which is no longer needed)
//
// 4     1/21/03 9:04p Bmeinke
// Removed all the extra newline characters in the ReaderThread's final
// Log() call
//
// 3     1/21/03 8:57p Bmeinke
// Modified to be a drop-in replacement for the old logical port driver
//
// 2     1/21/03 3:18a Bmeinke
// Fixed header comments
//
// 1     1/20/03 10:41p Bmeinke
// Serial communication proxies for raw (unformatted) serial data
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <algorithm>

#include "RawCommProxy.h"

// Declare (and allocate) the mutex
BepMutex    LogPortSubscription::m_subIdLock;

/**
 * Default Constructor
 */
RawCommProxy::RawCommProxy() : ResManagerMt(),
    m_pathName( COMM_PROXY_DEV_PATH), m_parentName( ""), m_portId( 0),
    m_fifoSize( 1024), m_nameId( NULL),
    m_readThreadId( -1), m_dataReadyChId(-1), m_comPortFd(-1),
    m_readThreadCoid( -1), m_needToRegister( true),
    m_mssgGapExpired( true), m_defaultReplyTimeout( 0LL),
    m_threadChId( -1), m_threadCoId( -1), m_threadTid( -1)
{
    SetProcessName( DEF_PROXY_PROC_NAME);
    InitProtocol( m_configProtocol);
}

/**
 * Constructor from command line arguments
 *
 * @param argc   Number of command line arguments to parse
 * @param argv   Array of command line argument strings
 */
RawCommProxy::RawCommProxy( int argc, char *argv[]) : ResManagerMt(),
    m_pathName( COMM_PROXY_DEV_PATH), m_parentName( ""), m_portId( 0),
    m_fifoSize( 1024), m_nameId( NULL),
    m_readThreadId( -1), m_dataReadyChId(-1), m_comPortFd(-1),
    m_readThreadCoid( -1), m_needToRegister( true),
    m_mssgGapExpired( true), m_defaultReplyTimeout( 0LL),
    m_threadChId( -1), m_threadCoId( -1), m_threadTid( -1)
{
    string  regName( "logport/");

    ParseCommandLine( argc, argv, "P:p:R");

    SetProcessName( regName + m_pathName);
    InitProtocol( m_configProtocol);
}

/**
 * Class destructor
 */
RawCommProxy::~RawCommProxy()
{
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
bool RawCommProxy::HandleCommandLineArg( int option, const char *value)
{
    bool    retVal = true;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::HandleCommandLineArg( %c, '%s')\n", option, optarg == NULL ? "" : optarg);

    switch( option)
    {
    case 'p':       // Path to adopt
        m_pathName = optarg;
        break;
    case 'P':       // Name of parent process to notify
        if (value != NULL)
            m_parentName = value;
        else
            m_parentName = "mgr/SerialServer";
        Log( LOG_DEV_DATA, "parent process = %s\n", m_parentName.c_str());
        break;
    case 'R':
        m_needToRegister = false;
        break;
    default:
        ResManagerBase::HandleCommandLineArg( option, value);
        break;
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::HandleCommandLineArg( %c, '%s')\n", option, optarg == NULL ? "" : optarg);

    return( retVal);
}

/**
 * Reads configuration data from the given XML node
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void RawCommProxy::Initialize(const XmlNode *document)
{
    Log( LOG_FN_ENTRY, "Enter RawCommProxy::Initialize(%s)\n", document->getName().c_str());

    // load the additional configuration items need by the RawCommProxy
    LoadAdditionalConfigurationItems(document);

    m_reflectionCount.Acquire();
    m_reflectionCount.Signal( 0);
    m_reflectionCount.Release();

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::Initialize()\n");
}

const INT32 RawCommProxy::HandlePulse(const INT32 code, const INT32 value)
{
    INT32   retVal = BEP_STATUS_FAILURE;

    // if this is a reload configuration pulse
    if((code == SYSTEM_PULSE) && (value == RELOAD_PULSE))
    {
        const XmlNode *document =
            m_parser.ReturnXMLDocument(GetConfigFile());

        // load the additional configuration items needed by the RawCommProxy
        LoadAdditionalConfigurationItems(document);

        retVal = BEP_STATUS_SUCCESS;
    }
    else
    {
        retVal = BepComponent::HandlePulse(code, value);
    }

    return( retVal);
}

void RawCommProxy::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    if( document != NULL)
    {
        try
        {
            XmlString portNode = XmlString( XML_T("Setup/LogicalPorts/"));
            portNode += m_pathName;

            // Read port configuration
            ReadPortConfig( document->getChild( portNode));
        }
        catch( XmlException &err)
        {
            Log( LOG_ERRORS, "\tError reading port configuration\n");
            throw;
        }
    }
}

/**
 * Reads logical port configuration from an XML document
 *
 * @param portNode Node containing setup data for our port only
 */
void RawCommProxy::ReadPortConfig( const XmlNode *portNode)
{
    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReadPortConfig(%s)\n", portNode->getName().c_str());

    try
    {
        // Initialize base class
        ResManagerMt::Initialize( portNode);
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tXmlException initializing ResManagerMt:: %s\n",
             err.GetReason());
    }
    catch( BepException &err)
    {
        Log( LOG_ERRORS, "\tBepException initializing ResManagerMt:: %s\n",
             err.GetReason());
    }

    try
    {
        // Read the name of the serial driver to talk to
        const XmlString &tmp = portNode->getChild( XML_T( "Setup/SerialDevice"))->getValue();
        m_comPortName = XmlToAscii( tmp);
        Log( LOG_DEV_DATA, "Talking on serial device '%s'\n",
             m_comPortName.c_str());
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tNO SERIAL DRIVER SPECIFIED\n");
        throw( BepException( "No serial driver no ( <Setup/SerialDevice> )"));
    }

    try
    {
        // Read the default reply timeout value (in milliseconds)
        const std::string tmp = XmlToAscii( portNode->getChild( XML_T( "Setup/DefaultReplyTimeout"))->getValue());
        // Convert milliseconds to nanoseconds
        m_defaultReplyTimeout = mSEC_nSEC( atoi( tmp.c_str()));
        Log( LOG_DEV_DATA, "Default reply timeout: %lld ns\n", m_defaultReplyTimeout);
    }
    catch( XmlException &err)
    {
    }

    // Read the logging config for serial communication
    ReadCommLogConfig( portNode);

    // Read and set the default port protocol for this port
    ReadPortProtocol( portNode);

    // Read name of Bus Comm log file here
    ReadCommLogFileName( portNode);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReadPortConfig()\n");
}

/**
 * Reads the logger configuration for logging serial communication data
 *
 * @param portNode Node containing setup data for our port only
 */
void RawCommProxy::ReadCommLogConfig( const XmlNode *portNode)
{
    XmlString       commLogSetupPath;
    const XmlNode   *comLogNode;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReadCommLogConfig()\n");

    // Default comm logging under file with same name as our port name
    //m_generalCommLog.SetAdditionalLogFileName( GetProcessName());

    try
    {
        comLogNode = portNode->getChild( XML_T( "Setup/CommLog"));
        // Default comm logging under file with same name as our port name
        m_generalCommLog.Initialize(comLogNode);

        // Stick the "content type" attribute into the log file. This
        //  attribute will be used by the GUI to know how to parse the
        //  data inside the log file.
        const XmlNode *contentTypeNode = comLogNode->getChild( XML_T("ContentType"));

        // Make sure data is in ASCII format for logger
        string contentType = XmlToAscii(contentTypeNode->getValue());
        m_generalCommLog.Log( "%s", contentType.c_str());
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tXmlException: %s\n", err.GetReason());
    }


    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReadCommLogConfig()\n");
}

/**
 * Reads the port protocol for this logical port from
 * the given XML node
 *
 * @param portNode Node containing setup data for our port only
 */
void RawCommProxy::ReadPortProtocol( const XmlNode *portNode)
{
    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReadPortProtocol(%s)\n", portNode->getName().c_str());
    const XmlNode *protocolNode;
    const XmlNode *dataNode;

    try
    {
        protocolNode = portNode->getChild( XML_T( "Setup/Protocol"));
        Log( LOG_DEV_DATA, "Protocol settings read from node 'Setup/Protocol'\n");
    }
    catch( XmlException &err)
    {
        Log( LOG_DEV_DATA, "'Setup/Protocol' node does not exist...read protocol from 'Setup'\n");
        protocolNode = portNode->getChild( XML_T("Setup"));
    }

    try
    {
        dataNode = protocolNode->getChild( XML_T( "FifoSize"));
        m_fifoSize = atol( dataNode->getValue().c_str());
    }
    catch( XmlException &err)
    {
        m_fifoSize = 1024;
    }
	m_rxFifo = new CommFifo(m_fifoSize);

    try
    {
        // read the protocol options from the config file
        //  we'll need these later to fill in the termios structure
        //  after we have opened the serial port
        m_configProtocol.Initialize( protocolNode);
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tError in m_configProtocol.Initialize: %s\n", err.GetReason());
    }

    try
    {
        ILogger::Initialize( portNode);
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tError in ILogger::Initialize: %s\n", err.GetReason());
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReadPortProtocol()\n");
}

/**
 * Reads the name of the file to log serial communication to
 *
 * @param portNode Node containing setup data for our port only
 */
void RawCommProxy::ReadCommLogFileName( const XmlNode *portNode)
{
    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReadCommLogFileName(%s)\n", portNode->getName().c_str());
    try
    {
        if( portNode != NULL)
        {
            const XmlNode *commLogNode = portNode->getChild( "CommLogFile");
            m_generalCommLog.SetAdditionalLogFileName( commLogNode->getValue());
        }
    }
    catch( ...)
    {
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReadCommLogFileName(%s)\n", portNode->getName().c_str());
}

/**
 * Method used to attach pulses to the resource manager's I/O
 * channel
 *
 * @return -1 on error, any other value indicates success
 */
int RawCommProxy::AttachIoPulses()
{
    int         retVal;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::AttachIoPulses()\n");

    // Attach the pulse code we will use to be notified by the serial driver
    //  when new data is available to read
    if( (retVal = AttachIoPulse( SERIAL_DATA_READY)) != -1)
    {
        if( (retVal = AttachIoPulse( LOG_PORT_NOTIFY_PULSE_CODE)) != -1)
        {
            // Attach the Interrupt pulse to our resource manager's channel
            if( (retVal = AttachIoPulse( MSSG_GAP_TIMER)) != -1)
            {
                // Attach the Interrupt pulse to our resource manager's channel
                if( (retVal = AttachIoPulse( REPLY_TIMEOUT_TIMER)) != -1)
                {
                    // Attach the Interrupt pulse to our resource manager's channel
                    if( (retVal = AttachIoPulse( BYTE_GAP_TIMER)) != -1)
                    {
                        // Attach the register pulse to our resource manager's channel
                        if( (retVal = AttachIoPulse( REGISTER_PULSE)) != -1)
                        {
                            retVal = ResManagerBase::AttachIoPulses();
                        }
                        else
                        {
                            Log( LOG_ERRORS, "\tUnable to attach register pulse: %s\n",
                                 strerror( errno));
                        }
                    }
                    else
                    {
                        Log( LOG_ERRORS, "\tUnable to attach byte timer pulse: %s\n",
                             strerror( errno));
                    }
                }
                else
                {
                    Log( LOG_ERRORS, "\tUnable to attach reply timeout timer pulse: %s\n",
                         strerror( errno));
                }
            }
            else
            {
                Log( LOG_ERRORS, "\tUnable to attach message timer pulse: %s\n",
                     strerror( errno));
            }
        }
        else
        {
            Log( LOG_ERRORS, "\tUnable to attach subscription notification pulse: %s\n",
                 strerror( errno));
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tUnable to attach serial data notification pulse: %s\n",
             strerror( errno));
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::AttachIoPulses()\n");

    return( retVal);
}

/**
 * Perform any server initialization
 *
 * @return true if successful
 *         false if error
 */
bool RawCommProxy::InitManager()
{
    bool    retVal = false;
    string  path( COMM_PROXY_ROOT);
    int     state = 0;
    pthread_mutexattr_t dataLockAttr;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::InitManager()\n");

    // Step through the initialization
    while( state >= 0)
    {
        switch( state)
        {
        case 0:
            memset( &dataLockAttr, 0, sizeof( dataLockAttr));
            memset( &m_dataLock, 0, sizeof( m_dataLock));
            memset( &m_subIdLock, 0, sizeof( m_subIdLock));

            pthread_mutexattr_init(&dataLockAttr);
            pthread_mutexattr_settype( &dataLockAttr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutexattr_setrecursive(&dataLockAttr, PTHREAD_RECURSIVE_ENABLE);
            pthread_mutex_init( &m_dataLock, &dataLockAttr);
            pthread_mutex_init( &m_subIdLock, &dataLockAttr);
            m_portLock.Acquire();
            m_portLock.Signal( false);
            m_portLock.Release();
            break;
        case 1:     // Initialize the base class
            if( (retVal = ResManagerBase::InitManager()) != true)
            {
                Log( LOG_ERRORS, "\tError in base class InitManager: %s\n",
                     strerror( errno));
                state = -2;
            }
            break;
        case 2:     // Attach a process name (for XML messages)
            if( BposAttachName( GetProcessName().c_str(), (void**)(&m_nameId)) != SUCCESS)
            {
                Log( LOG_ERRORS, "\tError registering name %s: %s\n",
                     GetProcessName().c_str(), strerror( errno));
                state = -2;
            }
            break;
        case 3:     // Adopt our '/dev' path
            // Adopted path will be the same name as our logical port name
            if (m_pathName[0] != '/')
                path = path + m_pathName;
            else
                path = m_pathName;

            // Attach our pseudo main serial path
            if( PathAttach( path.c_str(), m_rxFifo->GetSize()) == -1)
            {
                Log( LOG_ERRORS, "\tError attaching path: %s\n", strerror( errno));
                state = -2;
            }
            break;
        case 4:     // Allocate a dispatch context
            if( RegisterManager() == -1)
            {
                Log( LOG_ERRORS, "\tError registering resource manager: %s\n", strerror( errno));
                state = -2;
            }
            break;
        case 5:
            m_mssgGapTimer.Initialize( m_mssgCoid, MSSG_GAP_TIMER);
            break;
        case 6:
            if( m_needToRegister == false)
            {
                if( Register() != std::string( BEP_SUCCESS_RESPONSE))
                {
                    Log( LOG_ERRORS, "\tRegister() failed\n");
                    state = -2;
                }
            }
            break;
        default:    // Woo-hoo...it all worked !!!
            retVal = true;
            state = -2;
            break;
        }
        state++;
    }

    // If we need to notify a parent process that we are up and running
    if( m_parentName.empty() == false)
    {
        // Find parent's channel
        int parentCoid = name_open( m_parentName.c_str(), 0);
        if( parentCoid != -1)
        {
            // Send parent a pulse
            MsgSendPulse( parentCoid,
                          SIGEV_PULSE_PRIO_INHERIT,
                          SYSTEM_PULSE,
                          0);
            name_close( parentCoid);
        }
        else
        {
            Log( LOG_ERRORS, "\tUnable to name_open '%s'\n", m_parentName.c_str());
            retVal = false;
        }

        // Parent process will send use pulses for the following signals
        signal( SIGINT, SIG_IGN);
        signal( SIGWINCH, SIG_IGN);

        // Parent process will use SIGQUIT to terminate us
        //signal( SIGQUIT, SIG_DFL);
        signal( SIGQUIT, SignalHandler);
    }
    else
    {
        // No parent process, so handle the following signals
        signal( SIGINT, SignalHandler);
        signal( SIGQUIT, SignalHandler);
        signal( SIGWINCH, SignalHandler);
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::InitManager()\n");

    return( retVal);
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void RawCommProxy::CleanupManager()
{
    Log( LOG_FN_ENTRY, "Enter RawCommProxy::CleanupManager()\n");

    TerminateReaderThread();

    // Shutdown our message gap timer
    m_mssgGapTimer.Stop();

    ResManagerMt::CleanupManager();

    // Detach our adopted path
    if( m_resMgrId != -1)
    {
        Log( LOG_DEV_DATA, "Detach path name\n");
        PathDetach( m_resMgrId);
        m_resMgrId = -1;
    }

    // Unregister our name
    if( m_nameId != NULL)
    {
        Log( LOG_DEV_DATA, "Unregister task name\n");
        BposUnRegisterTaskName( (int)m_nameId);
        m_nameId = NULL;
    }

    pthread_mutex_destroy( &m_dataLock);
    pthread_mutex_destroy( &m_subIdLock);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::CleanupManager()\n");
}

/**
 * Method that is called to command the component to perform any
 * system registration functions necessary.
 *
 * @return Status of the registration process.
 *         Success, Fail
 * @since Version 2.5
 */
const std::string RawCommProxy::Register(void)
{
    std::string retVal( BEP_FAILURE_RESPONSE);
    int     state = 0;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::Register()\n");
    // Step through the initialization
    while( state >= 0)
    {
        switch( state)
        {
        case 0:     // Open the serial port (for writing data)
            if( (m_comPortFd=OpenPort( m_comPortName.c_str())) == -1)
            {
                Log( LOG_ERRORS, "\tError opening device '%s': %s\n", m_comPortName.c_str(),
                     strerror( errno));
                state = -2;
            }
            else
            {
                Log( LOG_DEV_DATA, "Serial device '%s' opened\n", m_comPortName.c_str());
            }
            break;
        case 1:     // Set port protocol based on config file settings
            if( ConfigurePort(m_configProtocol, TCSADRAIN) == -1)
            {
                Log( LOG_ERRORS, "\tError setting port attributes from config file: %s\n",
                     strerror( errno));
                state = -2;
            }
            else
            {
                Log( LOG_DEV_DATA, "Port configured\n");
                m_defaultPortProtocol = m_configProtocol;
                m_currentPortProtocol = m_configProtocol;;
            }
            break;
        case 2:
            // Flush any pending data
            if( FlushDriverFifos( TCIOFLUSH) == -1)
            {
                Log( LOG_ERRORS, "\tError flushing driver FIFOS: %s\n", strerror( errno));
                state = -2;
            }
            else
            {
                Log( LOG_DEV_DATA, "Driver FIFOs flushed\n");
            }
            break;
        case 3:
            if( StartReaderThread() == true)
            {
                Log( LOG_DEV_DATA, "Reader thread launched; thread ID=%d\n", m_threadTid);
            }
            else
            {
                state = -2;
            }
            break;
        case 4:
            try
            {
                m_faultServer.Initialize( FAULT_SERVER_NAME);
            }
            catch( BepException &err)
            {
                Log( LOG_ERRORS, "\tError initializing IFaultServer: %s\n", err.GetReason());
                state = -2;
            }
            break;
        default:    // Woo-hoo...it all worked !!!
            retVal = std::string( BEP_SUCCESS_RESPONSE);
            m_needToRegister = false;
            state = -2;
            break;
        }
        state++;
    }

    // Unregister with TaskMon to say we have a problem (if we had a problem)
    if (retVal != BEP_SUCCESS_RESPONSE)
        ReportTaskEnding();
    // Tell TaskMon we are up and running
    else
        ReportTaskUp();

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::Register( %s)\n", retVal.c_str());

    return( retVal);
}

/**
 * Method used to open a handle to the communication device driver
 *
 * @param devName Path to the communication device to be opened (i.e. /dev/ser1)
 * @param mode    Mode to open the device in (i.e. O_RDWR | O_NONBLOCK)
 *
 * @return A file descriptor to use for communicatin with the device driver or
 *         -1 if an error occurred
 */
int RawCommProxy::OpenPort( const char *devName, int mode)
{
    int retVal;

    retVal = open( devName, mode);

    return( retVal);

}

/**
 * Method used to close a handle to the communication device driver
 *
 * @return 0 if successful or -1 if an error occurred
 */
int RawCommProxy::ClosePort()
{
    if( m_comPortFd != -1)
    {
        close( m_comPortFd);
        m_comPortFd = -1;
    }

    return( 0);
}

/**
 * Apply configuration settings to a communication port
 *
 * @param portCfg Port configuration data
 * @param mode    When the settings whould be applied:
 *                TCASNOW: Apply settings now
 *                TCSDRAIN: Wait until all output has been transmitted before
 *                applying new configuration
 *                TCSAFLUSH: Wait until all output has been transmitted before
 *                applying new configuration at which point all
 *                received data is flushed
 *
 * @return 0 if successful, -1 if an error occurred
 */
int RawCommProxy::ConfigurePort( const SerialProtocol &portCfg, int mode)
{
    int retVal;
    retVal = tcsetattr( m_comPortFd, mode, &portCfg.m_tio);
    return( retVal);
}

/**
 * Apply configuration settings to a communication port
 *
 * @param portCfg Port configuration data
 * @param mode    When the settings whould be applied:
 *                TCASNOW: Apply settings now
 *                TCSDRAIN: Wait until all output has been transmitted before
 *                applying new configuration
 *                TCSAFLUSH: Wait until all output has been transmitted before
 *                applying new configuration at which point all
 *                received data is flushed
 *
 * @return 0 if successful, -1 if an error occurred
 */
int RawCommProxy::ConfigurePort( const struct termios *portCfg, int mode)
{
    SerialProtocol newProtocol( portCfg);
    // Retain the same port configuration settings
    newProtocol.SetByteGap( m_currentPortProtocol.GetByteGap());
    newProtocol.SetMsgGap( m_currentPortProtocol.GetMsgGap());
    newProtocol.SetReflection( m_currentPortProtocol.GetReflection());

    return( ConfigurePort(newProtocol));
}

/**
 * Apply configuration settings to a communication port
 *
 * @param portCfg Port configuration data
 * @param mode    When the settings whould be applied:
 *                TCASNOW: Apply settings now
 *                TCSDRAIN: Wait until all output has been transmitted before
 *                applying new configuration
 *                TCSAFLUSH: Wait until all output has been transmitted before
 *                applying new configuration at which point all
 *                received data is flushed
 *
 * @return 0 if successful, -1 if an error occurred
 */
int RawCommProxy::ConfigurePort( const SerialProtocol_t &portCfg, int mode)
{
    const SerialProtocol newProtocol = portCfg;
    return( ConfigurePort(newProtocol));
}

string RawCommProxy::DecodeIpAddress(const string &ipInfo)
{
    string ipAddress;
    // Get the index of each "." in the IP Address
    INT32 ip1EndIndex = ipInfo.find_first_of(".");
    INT32 ip2EndIndex = ipInfo.find_first_of(".", ip1EndIndex+1);
    INT32 ip3EndIndex = ipInfo.find_first_of(".", ip2EndIndex+1);
    Log(LOG_DETAILED_DATA, "\tip1EndIndex: %d", ip1EndIndex);
    Log(LOG_DETAILED_DATA, "\tip2EndIndex: %d", ip2EndIndex);
    Log(LOG_DETAILED_DATA, "\tip3EndIndex: %d\n\n", ip3EndIndex);
    // Check if the first part of the strng must be decoded
    string ipPart;
    ipPart = ipInfo.substr(0, ip1EndIndex);
    ipAddress = isdigit(ipPart[0]) ? (ipPart + ".") : (string(getenv(ipPart.c_str())) + string("."));
    Log(LOG_DETAILED_DATA, "\tipAdress: %s", ipAddress.c_str());
    // Check if the second part must be decoded
    ipPart = ipInfo.substr(ip1EndIndex+1, ip2EndIndex-ip1EndIndex-1);
    ipAddress += isdigit(ipPart[0]) ? (ipPart + ".") : (string(getenv(ipPart.c_str())) + string("."));
    Log(LOG_DETAILED_DATA, "\tipAdress: %s", ipAddress.c_str());
    // Check if the third part must be decoded
    ipPart = ipInfo.substr(ip2EndIndex+1, ip3EndIndex-ip2EndIndex-1);
    ipAddress += isdigit(ipPart[0]) ? (ipPart + ".") : (string(getenv(ipPart.c_str())) + string("."));
    Log(LOG_DETAILED_DATA, "\tipAdress: %s", ipAddress.c_str());
    // Check if the last part must be decoded
    ipPart = ipInfo.substr(ip3EndIndex+1, ipInfo.npos-ip3EndIndex-1);
    ipAddress += isdigit(ipPart[0]) ? (ipPart) : (string(getenv(ipPart.c_str())));
    Log(LOG_DEV_DATA, "\tipAdress: %s", ipAddress.c_str());
    // Return the result
    return ipAddress;
}

/**
 * Tells the communication driver to flush our FIFO(s)
 *
 * @param mode   Which FIFO(s) to flush:
 *               TCIOFLUSH: Flush our rx and tx FIFOs
 *               TCIFLUSH:  Flush only our rx FIFO
 *               TCOFLUSH:  Flush only our tx FIFO
 * @return -1 if an error occurred, any other value for success
 */
int RawCommProxy::FlushDriverFifos( int mode)
{
    return( tcflush( m_comPortFd, mode));
}

/**
 * Processes reflected bytes (if any) in the given buffer. If any
 * reflected bytes are determined to be present in the buffer, they
 * are removed from the buffer and the size argument is adjusted
 * accordingly
 *
 * @param buff   Buffer of received data bytes
 * @param size   Number of bytes in the buff argument to process. If any reflected
 *               bytes are processed from buff, this argument will be adjusted
 *               according to the number of reflected bytes processed
 */
void RawCommProxy::ProcessReflection( uint8_t *buff, uint32_t &size)
{
    uint32_t    reflCount = m_reflectionCount.GetValue();

    // If any reflection bytes expected
    if( reflCount)
    {
        Log( LOG_DEV_DATA, "Processing %d reflection bytes from %d byte buffer\n",
             reflCount, size);
        // If we expect more reflected bytes than we have received
        if( reflCount > (uint32_t)size)
        {
            PrintSerialString( LOG_SER_COM, "\tReflection data", buff, size);
            // All bytes assumed to be reflected
            reflCount -= size;
            size = 0;
        }
        else
        {
            PrintSerialString( LOG_SER_COM, "\tReflection data", buff, reflCount);
            uint32_t ii, jj;
            // Copy valid data to beginning of buffer
            for( ii=reflCount, jj=0; ii<size; ii++, jj++)
            {
                buff[ jj] = buff[ ii];
            }

            // Update number of valid bytes in buff
            size -= reflCount;

            // No more reflected bytes expected
            reflCount = 0;
        }

        // Update expected reflection count value
        m_reflectionCount.Signal( reflCount);
    }
}

/**
 * Method used to place received data into the FIFO's for subscribed
 * clients
 *
 * @param buff   Buffer of received data
 * @param len    Number of bytes in the FIFO
 * @param flag   Flag indicating which FIFOs to process (Rx or Tx)
 */
void RawCommProxy::UpdateClientFifos( const char *buff, size_t len, int flag)
{
    PortSubscrListItr_t subItr;
    CommIoOcb_t         *ocb;
    bool                okToAdd=true;
    SerialString_t      data( (const uint8_t*)buff, len);
    uint32_t            subscrPulseVal = 0;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::UpdateClientFifos()\n");

    // IF we need to update receive data subscribers
    if( flag & PORT_SUBSCRIBE_RX)
    {
        m_rxSubscribers.Lock();
        // Loop through all subscribers
        for(subItr=m_rxSubscribers.begin(); subItr!=m_rxSubscribers.end(); subItr++)
        {
            subscrPulseVal |= PORT_SUBSCRIBE_RX;
            // Say OK to add data to FIFO (assume no filtering)
            okToAdd = true;

            ocb = subItr->second;
            if( ocb != NULL)
            {
                // If we should add this data to the subscribers FIFO
                okToAdd = CanAddToClientFifo( data, ocb);
                if( okToAdd)
                {
                    // Stop the reply timer for this client because we have data for him
                    ocb->replyTimeoutTimer.Stop();

                    // Add the data to the client's fifo
                    AddToClientFifo( buff, len, ocb);

                    if( IOFUNC_NOTIFY_INPUT_CHECK(ocb->notify, (int)len, 0))
                    {
                        iofunc_notify_trigger(ocb->notify, len, IOFUNC_NOTIFY_INPUT);
                    }
                }
            }
            else
            {
                Log( LOG_DEV_DATA, "NULL OCB IN UpdateClientFifos\n");
            }
        }
        m_rxSubscribers.Unlock();
    }

    if( flag & PORT_SUBSCRIBE_TX)
    {
        subscrPulseVal |= PORT_SUBSCRIBE_TX;

        // Say OK to add data to FIFO
        okToAdd = true;

        // Loop through all subscribers
        for(subItr=m_txSubscribers.begin(); subItr!=m_txSubscribers.end(); subItr++)
        {
            ocb = subItr->second;
            if( ocb != NULL)
            {
                //ocb->txFifo.AddBytes( (const uint8_t*)buff, len);
                ocb->baseOcb.m_ioOcb.attr->nbytes += len;
            }
            else
            {
                Log( LOG_DEV_DATA, "NULL OCB IN UpdateClientFifos\n");
            }
        }
    }

    // If we have any subscribers to notify
    if( subscrPulseVal)
    {
        // Send ourselves a pulse to perform subscription updates
        MsgSendPulse( m_mssgCoid, SIGEV_PULSE_PRIO_INHERIT,
                      LOG_PORT_NOTIFY_PULSE_CODE, subscrPulseVal);
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::UpdateClientFifos()\n");
}

/**
 * Method used to check if a serial response string can be added to a
 * client's rx FIFO
 *
 * @param data   Serial string reseived from the port
 * @param ocb    Client connection identifier
 * @return true if the data should be added to the client's FIFO, false otherwise
 */
bool RawCommProxy::CanAddToClientFifo( const SerialString_t &data, CommIoOcb_t *ocb)
{
    bool                            retVal = true;
    LogPortFilterStringList         &fltrList = ocb->rxSubscription->filterList;
    LogPortFilterStringListItr_t    itr;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::CanAddToClientFifo()\n");
    // If client has any filter strings
    if( fltrList.size() > 0)
    {
        fltrList.Lock();
        for( itr=fltrList.begin(); itr!=fltrList.end(); itr++)
        {
            // Get reference to the filter string
            const LogPortFilterString &filter = *itr;
            // IF data matches the filter string
            if( filter.IsStringValid( data) == true)
            {
                // Ok to ADD to FIFO
                Log( LOG_DEV_DATA, "Response matches filter string\n");
                retVal = true;
                break;
            }
            else
            {
                // No match...look at next filter
                Log( LOG_DEV_DATA, "Response DOES NOT match filter string\n");
                retVal = false;
            }
        }
        fltrList.Unlock();
    }
    else
    {
        Log( LOG_DEV_DATA, "No filter strings for client\n");
        retVal = true;
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::CanAddToClientFifo(%d)\n", retVal);

    return( retVal);
}

/**
 * Adds the gives serial string to the specified client's fifo
 *
 * @param buff   Serial data to be added to the client's fifo
 * @param len    Length of the buffer
 * @param ocb    Client whose FIFO we should update
 */
bool RawCommProxy::AddToClientFifo( const char *buff, size_t len, CommIoOcb_t *ocb)
{
    const size_t    bufSize = 1024;
    uint8_t         clientFifo[ bufSize];
    int             bytesRead, err;
    uint32_t        maxReply;
    BlockedReader_t client;
    bool            retVal = false;

    if( (ocb != NULL) && (ocb->rxSubscription != NULL))
    {
        if( (err=ocb->rxSubscription->fifo.Lock()) == EOK)
        {
            Log( LOG_DEV_DATA, "\tAddToClientFifo: Client FIFO Locked\n");

            if( (errno=m_blockedReaders.Lock()) == EOK)
            {
                // If this client is blocked waiting for data to be received
                if(m_blockedReaders.FindBlockedReader(ocb, client) != false)
                {
                    // Determine the maximum number of bytes we can send to the client
                    if (len > (size_t)client.readMsg.i.nbytes)
                        maxReply = client.readMsg.i.nbytes;
                    else
                        maxReply = len;

                    Log( LOG_DEV_DATA, "Replying to blocked client (%d,%d) with %d bytes (%d, %d)\n",
                         ocb->baseOcb.mssgInfo.pid, ocb->baseOcb.mssgInfo.tid, maxReply, len, client.readMsg.i.nbytes);

                    PrintSerialString(LOG_DEV_DATA, "Client's Data", (const uint8_t*)buff, maxReply);

                    // Remove this client from the blocked reader list
                    m_blockedReaders.RemoveBlockedReader(ocb);

                    // Send the data to the client
                    MsgReply( client.replyId, maxReply, buff, maxReply);

                    // Add remaining data to the client's FIFO
                    if(len > maxReply)
                    {
                        ocb->rxSubscription->fifo.AddBytes( (const uint8_t*)buff+maxReply, len-maxReply);
                        ocb->baseOcb.m_ioOcb.attr->nbytes += len - maxReply;
                    }

                    // Say we already replied to the client with his data
                    retVal = true;
                }
                else
                {
                    Log( LOG_DEV_DATA, "Adding %d bytes to subscriber (%d,%d) FIFO\n",
                         len, ocb->baseOcb.mssgInfo.pid, ocb->baseOcb.mssgInfo.tid);
                    ocb->rxSubscription->fifo.AddBytes( (const uint8_t*)buff, len);
                    ocb->baseOcb.m_ioOcb.attr->nbytes += len;

                    // Say we DID NOT reply to the client with his data
                    retVal = false;
                }

                m_blockedReaders.Unlock();
            }
            else
            {
                Log( LOG_ERRORS, "\tError locking blocked reader list: %s\n", strerror( errno));
            }

            ocb->rxSubscription->fifo.Unlock();
            Log( LOG_DEV_DATA, "\tAddToClientFifo: Client FIFO Unlocked\n");
        }
        else
        {
            Log( LOG_ERRORS, "\tError %d locking client's fifo: %s\n", err ,strerror( err));
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tInvalid ocb/fifo\n");
        ocb->rxSubscription->fifo.AddBytes( (const uint8_t*)buff, len);
        ocb->baseOcb.m_ioOcb.attr->nbytes += len;
    }
    if( GetVerboseMask() & LOG_DEV_DATA)
    {
        if( (bytesRead = ocb->rxSubscription->fifo.Peek( clientFifo, bufSize)) > 0)
        {
            PrintSerialString( LOG_DEV_DATA, "FIFO Contents", clientFifo, bytesRead);
        }
    }

    return( retVal);
}

/**
 * Adds the gives serial string to the specified client's fifo
 *
 * @param buff   Serial data to be added to the client's fifo
 * @param len    Length of the buffer
 */
bool RawCommProxy::AddToClientFifo( const SerialString_t &buff, CommIoOcb_t *ocb)
{
    return( AddToClientFifo( (const char *)buff.c_str(), (size_t)buff.size(), ocb));
}

/**
 * Method used to check if a client owns the port lock
 *
 * @param clientOcb Client OCB containing flag indicating if he owns the port lock
 * @return true if client can write to port, false if client cannot write
 */
bool RawCommProxy::ClientOwnsPortLock( CommIoOcb_t *clientOcb)
{
    bool    retVal = false;
    int     lockErr;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ClientOwnsPortLock()\n");

    // Lock the condvar while checking its value
    lockErr = m_portLock.Acquire();
    // If we locked the port (or already have the port locked)
    if( (EOK == lockErr) || (EDEADLK == lockErr))
    {
        // If port is currently locked
        if( m_portLock.GetValue() == true)
        {
            // Ability to write depends on whether or not this client owns
            //  the current port lock
            if( clientOcb->portLockCount > 0)
            {
                Log( LOG_DEV_DATA, "Client owns locked port\n");
                retVal = true;
            }
            else
            {
                Log( LOG_DEV_DATA | LOG_ERRORS, "Client DOES NOT own locked port\n");
                // Give it a second.. geez
                BposSleep(50);
            }
        }
        else
        {
            Log( LOG_DEV_DATA | LOG_ERRORS, "Port is NOT currently locked\n");
        }

        if (EOK == lockErr)
            m_portLock.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError checking port lock mutex: %s\n",
             strerror( lockErr));
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ClientOwnsPortLock()\n");

    return( retVal);
}

/**
 * Method used to check if a client owns the port lock
 *
 * @param clientOcb Client OCB containing flag indicating if he owns the port lock
 * @return true if client can write to port, false if client cannot write
 */
bool RawCommProxy::CanClientWrite( CommIoOcb_t *clientOcb)
{
    bool    retVal = false;
    int     lockErr;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::CanClientWrite()\n");

    // Lock the condvar while checking its value
    lockErr = m_portLock.Acquire();
    // If we locked the port (or already have the port locked)
    if( (EOK == lockErr) || (EDEADLK == lockErr))
    {
        // If port currently not locked
        if( false == IsPortLocked())
        {
            // OK for client to write
            Log( LOG_DEV_DATA, "OK to write...port not locked\n");
            retVal = true;
        }
        else
        {
            // Ability to write depends on whether or not this client owns
            //  the current port lock
            retVal = ClientOwnsPortLock( clientOcb);
            if (true == retVal)
                Log(LOG_DEV_DATA, "Client can write\n");
            else
                Log(LOG_DEV_DATA | LOG_ERRORS, "Client can NOT write\n");
        }

        if (EOK == lockErr)
            m_portLock.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError checking port lock mutex: %s\n",
             strerror( lockErr));
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::CanClientWrite()\n");

    return( retVal);
}

/**
 * Method used to check the port lock status
 *
 * @return true if port is currently locked, false if port is currently unlocked
 */
bool RawCommProxy::IsPortLocked()
{
    bool    retVal = true;
    int     lockErr;

    // Lock port to read value
    lockErr = m_portLock.Acquire();
    // If we just locked the port (or already have the port locked)
    if( (EOK == lockErr) || (EDEADLK == lockErr))
    {
        // Read status of port lock
        retVal = m_portLock.GetValue();

        // Only release the lock if we just locked it
        if (EOK == lockErr)
            m_portLock.Release();
    }

    return( retVal);
}

/**
 * Adds data to the communication log file associated with the given
 * client OCB
 *
 * @param comDir     Direction of the communication (transmit or receive)
 * @param commString Communication data to be logged
 * @param byteCount  Number of bytes to be logged
 * @param clientOcb     OCB associated with the client doing the communication. If NULL,
 *                   data will be logged to a general serial comm log file
 */
void RawCommProxy::UpdateBusCommLog( LogPortCommDir_t comDir, const uint8_t *commString,
                                     uint32_t byteCount, CommIoOcb_t *clientOcb)
{
    std::string         logBuff;
    char                byteBuff[ 10];
    register uint32_t   ii;

    // Say which direction this communication is
    switch( comDir)
    {
    case ComDirTx:
        logBuff = std::string( "Tx");
        break;
    case ComDirRx:
        logBuff = std::string( "Rx");
        break;
    case ComDirBusAck:
        logBuff = std::string( "BusAck");
        break;
    case ComDirErr:
        logBuff = std::string( "Error");
        break;
    default:
        logBuff = std::string( "??");
        break;
    }

    // Fill in serial message data
    for( ii=0; ii<byteCount; ii++)
    {
        sprintf( byteBuff, " $%02hhX", commString[ ii] & 0xFF);
        logBuff += byteBuff;
    }


    {
        // Put into general communication log file
        m_generalCommLog.Log( logBuff.c_str());
    }
}

/**
 * See which channel to wait on
 *
 * @return Channel id to wait for messages on
 */
int RawCommProxy::GetWaitChid()
{
    int retVal=-1;
    if( m_nameId != NULL)
    {
        retVal = m_nameId->chid;
    }
    return( retVal);
}

/**
 * Method called to read received data from the port (without a mutex wrapper)
 *
 * @param buff   Buffer to put the received data into
 * @param buffSz Size (in bytes) of the buff parameter
 * @return The number of bytes received from the port
 */
int RawCommProxy::ReadPortData(uint8_t *buff, size_t buffSz)
{
    int readCount = 0;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReadPortData()\n");

    // Lock the raw receive buffer
    if( (readCount=m_rxBuffLock.Acquire()) == EOK)
    {
        // See if we have a full AVT message packet
        readCount = ReadPortDataUnlocked(buff,buffSz);

        m_rxBuffLock.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tUnable to lock raw rx buffer in ReadPortData: %s\n", strerror( readCount));
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReadPortData(%d)\n", readCount);

    return( readCount);
}


/**
 * The purpose of this is to give us a place to allocate our own ocb. It is
 * called by ocbCalloc, which in turn is called as a result of an open being done
 * on our reource manager's adopted path(e.g. iofunc_open_default causes it to be
 * called). ocbCalloc was registered through our mount structure.
 *
 * @param ctp    Resource manager's context block
 * @param device Attributes for this client connection (an OCB)
 * @return Pointer to newly allocated OCB or NULL if error
 * @see ResManagerBase::ocbCalloc
 */
IOFUNC_OCB_T* RawCommProxy::OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
{
    CommIoOcb_t *j2190Ocb = new CommIoOcb_t;

    return( (IOFUNC_OCB_T*)j2190Ocb);
}

/**
 * Method used to initialize an allocated OCB structure. Called
 * immediately after the OCB is allocated
 *
 * @param ioOcb  OCB structure to be initialized
 */
void RawCommProxy::OcbInit(resMgrIoOcb_t *ioOcb)
{
    CommIoOcb_t *newOcb = (CommIoOcb_t*)ioOcb;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::OcbInit( %d, %d)\n",
         ioOcb->mssgInfo.pid, ioOcb->mssgInfo.tid);

    newOcb->portLockCount = 0;
    newOcb->subMask = 0;

    // Allocate a new subscription object for rx data
    newOcb->rxSubscription = new LogPortSubscription( 1);
    if( newOcb->rxSubscription->id == LOG_PORT_INVALID_SUB)
    {
        Log( LOG_ERRORS, "\tERROR Unable to create subscription ID\n");
    }
    newOcb->rxSubscription->Invalidate();

    newOcb->txSubscription = NULL;
    newOcb->lockSubscription = NULL;
    newOcb->unlockSubscription = NULL;
    newOcb->replyTimeout = 0;


    // Initialize the ionotify structures to empty
    IOFUNC_NOTIFY_INIT(newOcb->notify);
    for( int ii=0; ii<3; ii++)
    {
        newOcb->notify[ ii].list = NULL;
    }

    // Add client with a dummy notification event (not used unless client
    //  actually subscribes).
    m_rxSubscribers.AddSubscriber( newOcb->rxSubscription->id, (IOFUNC_OCB_T*)newOcb);
    Log( LOG_DEV_DATA, "Client rx ID = %lld\n", newOcb->rxSubscription->id);

    // Initialize the reply timeout timer for this client
    newOcb->replyTimeoutTimer.Initialize(m_mssgCoid, REPLY_TIMEOUT_TIMER);

    ResManagerBase::OcbInit( &newOcb->baseOcb);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::OcbInit( %d, %d)\n",
         ioOcb->mssgInfo.pid, ioOcb->mssgInfo.tid);
}

/**
 * The purpose of this is to give us a place to free our ocb. It is called
 * by ResManagerBase::ocbFree, which in turn is called as a result of the close
 * being done (e.g. iofunc_close_ocb_default causes it to be called).
 * ResManagerBase::ocbFree was registered through our mount structure.
 *
 * @param ocb    Client's Open Context Block for this connection
 * @see ResManagerBase::ocbFree
 */
void RawCommProxy::OcbFree(resMgrIoOcb_t *ocb)
{
    CommIoOcb_t *j2190Ocb = (CommIoOcb_t*)ocb;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::OcbFree( %d, %d)\n",
         ocb->mssgInfo.pid, ocb->mssgInfo.tid);
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::OcbFree( %d, %d)\n",
         ocb->mssgInfo.pid, ocb->mssgInfo.tid);
    delete j2190Ocb;
}

/**
 * Method used to finalize an allocated OCB structure. Called immediately
 * before the OCB is destroyed
 *
 * @param ioOcb  OCB structure to be finalized
 */
void RawCommProxy::OcbFinalize(resMgrIoOcb_t *ioOcb)
{
    CommIoOcb_t     *oldOcb = (CommIoOcb_t*)ioOcb;
    BlockedReader_t readClient;
    BlockedWriter_t writeClient;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::OcbFinalize( %d, %d)\n",
         ioOcb->mssgInfo.pid, ioOcb->mssgInfo.tid);

    if( (errno=m_blockedReaders.Lock()) == EOK)
    {
        // If this client is in a blocked read condition
        if( m_blockedReaders.FindBlockedReader(oldOcb, readClient) == true)
        {
            // Remove client from blocked reader list
            m_blockedReaders.RemoveBlockedReader( oldOcb);

            // Tell blocked client that read was cancelled
            MsgError( readClient.replyId, ECANCELED);
        }
        m_blockedReaders.Unlock();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking blocked reader list: %s\n", strerror( errno));
    }

    m_blockedWriters.Lock();
    // If this client is in a blocked read condition
    if( m_blockedWriters.FindBlockedWriter(oldOcb, writeClient) == true)
    {
        // Remove client from blocked reader list
        m_blockedWriters.RemoveBlockedWriter( oldOcb);
        // Tell blocked client that read was cancelled
        MsgError( writeClient.replyId, ECANCELED);
    }
    m_blockedWriters.Unlock();

    // If this client had an exclusive on the port
    while( oldOcb->portLockCount > 0)
    {
        // Make sure any locks help by this client are released
        ReleaseClientPortLock( oldOcb);
    }

    // Free memory allocated for subscriptions
    if( oldOcb->rxSubscription != NULL)
    {
        m_rxSubscribers.DelSubscriber( oldOcb->rxSubscription->id);
        delete oldOcb->rxSubscription;
        oldOcb->rxSubscription = NULL;
    }
    if( oldOcb->txSubscription != NULL)
    {
        m_txSubscribers.DelSubscriber( oldOcb->txSubscription->id);
        delete oldOcb->txSubscription;
        oldOcb->txSubscription = NULL;
    }
    if( oldOcb->lockSubscription != NULL)
    {
        m_lockSubscribers.DelSubscriber( oldOcb->lockSubscription->id);
        delete oldOcb->lockSubscription;
        oldOcb->lockSubscription = NULL;
    }
    if( oldOcb->unlockSubscription != NULL)
    {
        m_unlockSubscribers.DelSubscriber( oldOcb->unlockSubscription->id);
        delete oldOcb->unlockSubscription;
        oldOcb->unlockSubscription = NULL;
    }

    // Turn off loggin for comm data
//  oldOcb->commLogger.DisableLogger();

    ResManagerBase::OcbFinalize( (resMgrIoOcb_t *)oldOcb);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::OcbFinalize( %d, %d)\n",
         ioOcb->mssgInfo.pid, ioOcb->mssgInfo.tid);
}

/**
 * Function synthesized by the base-layer library when the last
 * close has been received for a particular OCB. Called by ioCloseOcb
 *
 * @param ctp      Dispatch interface pointer
 * @param reserved Reserved for ther base-layer library
 * @param ioOcb    Control block for the client
 * @return 0 if successful
 *         < 0 if error occurs
 * @see ResManagerBase::ioCloseOcb
 */
int RawCommProxy::IoCloseOcb(resmgr_context_t *ctp, void *reserved, resMgrIoOcb_t *ioOcb)
{
    int         retVal;
    CommIoOcb_t *clientOcb = (CommIoOcb_t*)ioOcb;
    pid_t       pid = ioOcb->mssgInfo.pid;
    int         tid = ioOcb->mssgInfo.tid;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::IoCloseOcb( %d, %d)\n",pid, tid);

    // Remove this client's ionotify events
    iofunc_notify_remove(ctp, clientOcb->notify);

    retVal = ResManagerBase::IoCloseOcb( ctp, reserved, ioOcb);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::IoCloseOcb( %d, %d)\n",pid, tid);

    return( retVal);
}

/**
 * Message handler for _IO_READ I/O messages. Called by ResManagerBase::ioRead
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Read message data
 * @param ioOcb  Control Block for this client
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManagerBase::ioRead
 */
int RawCommProxy::IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
    int                 retVal = EIO;
    CommIoOcb_t         *clientOcb = (CommIoOcb_t*)ioOcb;
    register uint32_t   count;
    int                 bytesRead;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::IoRead( pid %d, tid %d)\n", ctp->info.pid, ctp->info.tid);

    // If this is not a combine message and we have data to transfer
    if( (msg->i.xtype & _IO_XTYPE_MASK) == _IO_XTYPE_NONE)
    {
        // Hold onto the info for this message
        MsgInfo( ctp->rcvid, &ioOcb->mssgInfo);

        if( (retVal=clientOcb->rxSubscription->fifo.Lock()) == EOK)
        {
            Log( LOG_DEV_DATA, "\tIoRead: Client FIFO Locked\n");
            count = clientOcb->rxSubscription->fifo.GetSize();
            Log( LOG_DEV_DATA, "\tClient FIFO contains %d bytes\n", count);

            // Calculate how many bytes to transfer from client's FIFO
            if (count > (uint32_t)msg->i.nbytes)
                count = msg->i.nbytes;

            // If we need to transfer any data
            if(count > 0)
            {
                // Temp buffer
                const int buffLen = count;
                uint8_t buff[ buffLen];

                // Copy FIFO to temp buffer
                bytesRead = clientOcb->rxSubscription->fifo.GetBytes( buff, count);

                // Update time attributes for our "file"
                ((iofunc_attr_t*)(ioOcb->m_ioOcb.attr))->flags |= IOFUNC_ATTR_ATIME |
                                                                  IOFUNC_ATTR_DIRTY_TIME;

                PrintSerialString( LOG_DEV_DATA, "\tData already available", buff, bytesRead);

                // Reply to client with data
                MsgReply( ctp->rcvid, bytesRead, buff, bytesRead);

                // Tell the client how many bytes were read
                _IO_SET_READ_NBYTES( ctp, count);
            }
            // No data and OK for client to block
            else
            {
                Log( LOG_DEV_DATA, "\tClient requesting %d bytes, %d available\n",
                     msg->i.nbytes, clientOcb->rxSubscription->fifo.GetSize());

                // If port not opened in non-blocking mode
                if(!(clientOcb->baseOcb.m_ioOcb.ioflag & O_NONBLOCK))
                {
                    Log( LOG_DEV_DATA, "\tAdding client to blocked reader list\n");
                    m_blockedReaders.AddBlockedReader(clientOcb, ctp->rcvid, *msg);
                }
                else
                {
                    // No data to transfer
                    MsgReply( ctp->rcvid, EOK, NULL, 0);
                    Log( LOG_DEV_DATA, "\tClient's FIFO is empty\n");
                }
            }
            clientOcb->rxSubscription->fifo.Unlock();
            Log( LOG_DEV_DATA, "\tIoRead: Client FIFO Unlocked\n");

            retVal = _RESMGR_NOREPLY;
        }
        else
        {
            Log( LOG_ERRORS, "\tError %d locking client's fifo: %s\n", retVal ,strerror( retVal));
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tUnsupported combine read\n");
        retVal = ENOSYS;
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::IoRead( pid %d, tid %d)\n", ctp->info.pid, ctp->info.tid);

    return( retVal);
}

/**
 * Message handler for _IO_WRITE I/O messages. Called ny ResManagerBase::ioWrite
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Write message data
 * @param ioOcb  Control Block for this client
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManagerBase::ioWrite
 */
int RawCommProxy::IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb)
{
    int             bytesRead = 0, retVal = EOK;
    off_t           nbytes;
    CommIoOcb_t     *comOcb = (CommIoOcb_t*)ioOcb;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::IoWrite( pid %d, tid %d)\n", ctp->info.pid, ctp->info.tid);

    // Check for port lock and no combine message
    if( (msg->i.xtype & _IO_XTYPE_MASK) == _IO_XTYPE_NONE)
    {
        // Hold onto the info for this message
        ioOcb->mssgInfo = ctp->info;

        // If we can read any data into our buffer
        if( msg->i.nbytes > 0)
        {
            SerialString_t  writeString;
            // Read the message data from the sender
            bytesRead = ReadSendersBuffer(ctp, writeString, msg);
            if( bytesRead > 0)
            {
                // Check to be sure that the inter-mssg gap has passed
                if( m_mssgGapExpired == true)
                {
                    // If the reply timeout timer is running, stop it
                    comOcb->replyTimeoutTimer.Stop();

                    nbytes = JustWrite( ctp, ioOcb, writeString.c_str(), bytesRead);
                }
                else
                {
                    Log( LOG_DEV_DATA, "\tAdding client to blocked writer list, %d bytes\n",
                         bytesRead);

                    PrintSerialString( LOG_SER_COM, "Blocked Write Mssg",  writeString);
                    // Add client to blocked writer list
                    m_blockedWriters.AddBlockedWriter( ctp, ctp->rcvid,
                                                       writeString,
                                                       (const IOFUNC_OCB_T *)ioOcb);

                    // No data sent
                    nbytes = 0;
                    _IO_SET_WRITE_NBYTES( ctp, 0);

                    // Dont reply to client
                    retVal = _RESMGR_NOREPLY;
                }
            }
            else
            {
                Log( LOG_ERRORS, "\tError %d reading io_write message data: %s\n",
                     errno, strerror( errno));
                _IO_SET_WRITE_NBYTES( ctp, 0);
                retVal = errno;
            }
        }
        else
        {
            // Client did not send us any data to write
            Log( LOG_DEV_DATA, "\tClient write() buffer is empty\n");
            _IO_SET_WRITE_NBYTES( ctp, 0);
            retVal = EOK;
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tInvalid XTYPE write request in IoWrite\n");
        retVal = ENOSYS;
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::IoWrite( pid %d, tid %d), %d bytes status=%d\n", ctp->info.pid, ctp->info.tid, bytesRead, retVal);

    return( retVal);
}

/**
 * IoWrite handler used to just write data out to the serial driver
 *
 * @param ctp    Dispatch Interface pointer
 * @param ioOcb    ClientOCB for which we are sending the message
 * @param xmtSting The serial data to be sent
 * @param xmtLen   The number of bytes to be written
 * @return -1 if an error occurred, any other value represents the number of bytes written
 */
int RawCommProxy::JustWrite( resmgr_context_t *ctp, resMgrIoOcb_t *ioOcb, const uint8_t *xmtSting, const int xmtLen)
{
    int             retVal = -1;
    CommIoOcb_t     *clientOcb = (CommIoOcb_t*)ioOcb;
//  const uint32_t  mssgGap = m_configProtocol.GetMsgGap();
    const uint32_t  mssgGap = m_currentPortProtocol.GetMsgGap();

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::JustWrite()\n");

    // Check for port lock and no combine message
    if( CanClientWrite(clientOcb) == true)
    {
        // Print out the string
        PrintSerialString( LOG_DEV_DATA, "\tTX String", xmtSting, xmtLen);

        // Log communication for this serial send
        UpdateBusCommLog( ComDirTx, xmtSting, xmtLen, NULL);

        // Set flag to false here so no other clients will write
        //  until we have written and the timer expires
        if (mssgGap > 0)
            m_mssgGapExpired = false;

        // Add this message to the transmit queue
        retVal = TransmitMessage( xmtSting, xmtLen, clientOcb);

        // Arm the reply timeout timer if we wrote any data
        if( retVal > 0)
        {
            ArmReplyTimeoutTimer( clientOcb);
        }
        else if(retVal <= 0)
        {
            Log( LOG_DEV_DATA | LOG_ERRORS, "Not arming reply timeout timer because of transmit error\n");
        }

        // Arm the message gap timer
        if( mssgGap > 0)
        {
            Log( LOG_DEV_DATA, "\tArming message gap timer for %d ms\n", mssgGap);
            m_mssgGapTimer.Arm( mSEC_nSEC(mssgGap));
        }

        if( retVal > 0)
        {
            // Tell kernel to update last modification time
            ioOcb->m_ioOcb.attr->flags |= IOFUNC_ATTR_MTIME |
                                          IOFUNC_ATTR_DIRTY_TIME;

            // Advance the offset inside the OCB structure
            ioOcb->m_ioOcb.offset += retVal;
        }
        else
        {
            Log( LOG_ERRORS, "\tError #%d transmitting data: %s\n", errno, strerror( errno));
            retVal = errno;
        }

        // Tell the client how many bytes were written
        _IO_SET_WRITE_NBYTES( ctp, retVal);
    }
    else
    {
        // Client does not have write permissions
        retVal = _RESMGR_STATUS( ctp, EPERM);
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::JustWrite(), %d bytes\n", retVal);

    return( retVal);
}

/**
 * Handler for kernel pulses issued as a result of a alient's attempt
 * to unblock during the I/O mesage phase of an I/O operation.
 * Called by ioUnblock
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Pulse from the kernel
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioUnblock
 */
int RawCommProxy::IoUnblock(resmgr_context_t *ctp, io_pulse_t *msg, resMgrIoOcb_t *ioOcb)
{
    int             retVal;
    CommIoOcb_t     *clientOcb = (CommIoOcb_t*)ioOcb;
    BlockedReader_t readClient;
    BlockedWriter_t writeClient;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::IoUnblock( pid %d, tid %d)\n", ctp->info.pid, ctp->info.tid);

    if( (errno=m_blockedReaders.Lock()) == EOK)
    {
        // Check if this client is in our blocked reader list
        if( m_blockedReaders.FindBlockedReader( clientOcb, readClient) != false)
        {
            ProcessReaderUnblockRequest( ioOcb, readClient);
        }

        m_blockedReaders.Unlock();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking blocked reader list: %s\n", strerror( errno));
    }

    // If this client is in a blocked read condition
    if( m_blockedWriters.FindBlockedWriter( clientOcb, writeClient) == true)
    {
        ProcessWriterUnblockRequest( ioOcb, writeClient);
    }

    // Remove this client from the list of clients waiting fo lock the port
    if( m_lockWaitList.PopClient( clientOcb) == true)
    {
        Log( LOG_DEV_DATA, "Removed client from blocked locker list\n");
    }

    // Reply to client with ETIMEDOUT error
    retVal = _RESMGR_STATUS( ctp, ETIMEDOUT);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::IoUnblock( pid %d, tid %d)\n", ctp->info.pid, ctp->info.tid);

    return( retVal);
}

/**
 * Handler used to perform any special processing required when a
 * blocked reader client requests to be unblocked (via TimerTimeout)
 *
 * @param ioOcb  Control block for the calling client
 * @param reader Reference to the blocked reader's attributes
 */
void RawCommProxy::ProcessReaderUnblockRequest(resMgrIoOcb_t *ioOcb, BlockedReader_t &reader)
{
    CommIoOcb_t     *clientOcb = (CommIoOcb_t*)ioOcb;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ProcessReaderUnblockRequest()\n");

    // Stop the reply timeout timer
    if( clientOcb->replyTimeoutTimer.IsTimerArmed())
    {
        Log( LOG_DEV_DATA, "Stopping client's reply timeout timer\n");
        clientOcb->replyTimeoutTimer.Stop();
    }

    // Remove this client from the list of blocked readers
    Log( LOG_DEV_DATA, "Removing client from blocked reader list\n");
    m_blockedReaders.RemoveBlockedReader( clientOcb);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ProcessReaderUnblockRequest()\n");
}

/**
 * Handler used to perform any special processing required when a
 * blocked writer client requests to be unblocked (via TimerTimeout)
 *
 * @param ioOcb  Control block for the calling client
 * @param writer Reference to the blocked writer's attributes
 */
void RawCommProxy::ProcessWriterUnblockRequest(resMgrIoOcb_t *ioOcb, BlockedWriter_t &writer)
{
    CommIoOcb_t     *clientOcb = (CommIoOcb_t*)ioOcb;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ProcessWriterUnblockRequest()\n");

    Log( LOG_DEV_DATA, "Removing client from blocked writer list\n");

    // Remove client from blocked reader list
    m_blockedWriters.RemoveBlockedWriter( clientOcb);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ProcessWriterUnblockRequest()\n");
}

/**
 * Handler routine for custom device commands (in response to
 * a client devctl() call)
 *
 * @param ctp    Resource Manager context block
 * @param msg    Received data message data
 * @param ioOcb  Client Open Context Block
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManagerBase::ioDevCtl
 */
int RawCommProxy::IoDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    int             retVal = -1;
    int             devCmd = msg->i.dcmd & 0xFFFF;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::IoDevCtl( pid %d, tid %d) cmd=%d\n", ctp->info.pid, ctp->info.tid, devCmd);

    // Hold onto the info for this message
    ioOcb->mssgInfo = ctp->info;

    switch( devCmd)
    {
    case LOG_PORT_LOCK_PORT:
        // Try to obtain the port lock
        retVal = LockPortHandler(ctp, msg, ioOcb);
        break;
    case LOG_PORT_UNLOCK_PORT:
        // Release the port lock
        retVal = UnLockPortHandler(ctp, msg, ioOcb);
        break;
    case LOG_PORT_SUBSCRIBE:
        // Subscribe for port events
        retVal = PortSubscribeHandler(ctp, msg, ioOcb);
        break;
    case LOG_PORT_UNSUBSCRIBE:
        // Unsubscribe for port events
        retVal = PortUnsubscribeHandler(ctp, msg, ioOcb);
        break;
    case LOG_PORT_RESET_CONN:
        // Reset a client's connection
        retVal = ResetConnectionHandler( ctp, msg, ioOcb);
        break;
    case LOG_PORT_RESET_DRIVER:
        // Reset a client's connection
        retVal = ResetDriverHandler( ctp, msg, ioOcb);
        break;
    case LOG_PORT_SET_REPLY_TIMEOUT:
        // Reset a client's connection
        retVal = SetReplyTimeoutHandler( ctp, msg, ioOcb);
        break;
    case LOG_PORT_ESTABLISH_COMMS:
        retVal = FastInitHandler(ctp, msg, ioOcb);
        break;
    case LOG_PORT_LOW_SPEED_INIT:
        retVal = LowSpeedInit(ctp, msg, ioOcb);
        break;
    case LOG_PORT_RECONNECT:
        // Verify that we can reconnect
        if( (retVal=ReconnectHandlerVerify(ctp, msg, ioOcb)) == EOK)
        {
            // Reconnect to the driver
            if( (retVal = ReconnectHandler(ctp, msg, ioOcb)) == EOK)
            {
                // Clenup after econnect
                retVal = ReconnectHandlerFinalize(ctp, msg, ioOcb);
            }
        }
        break;

        /******************************************
            Posix terminal compatability handlers
        ******************************************/
    default:
        retVal = PosixDevCtl( ctp, msg, ioOcb);
        break;
    }

    ////////////////////////////////////////////////////////////
    //
    Log(LOG_FN_ENTRY, "Exit RawCommProxy::IoDevCtl( pid %d, tid %d) cmd=%d\n", ctp->info.pid, ctp->info.tid, (msg->i.dcmd & 0xFFFF));

    return( retVal);
}

/**
 * Handler routine for standard POSIX terminal control device commands
 * (in response to a client devctl() call)
 *
 * @param ctp    Resource Manager context block
 * @param msg    Received data message data
 * @param ioOcb  Client Open Context Block
 *
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManagerBase::ioDevCtl
 */
int RawCommProxy::PosixDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    typedef union
    {
        struct termios          tios;
        struct _xtype_readcond  readcond;
        struct _ttyinfo         ttyinfo;
        struct winsize          winsize;
        int                     data32;
        unsigned int            udata32;
        int                     flags;
        SerialProtocol_t        bepAttr;
    }   TermDevCtl_t;
    int             retVal = -1;
    char            *tempName;
    struct termios  tempTio;
    TermDevCtl_t    *dcp = (TermDevCtl_t*)_DEVCTL_DATA(msg->i);
    CommIoOcb_t     *clientOcb = (CommIoOcb_t*)ioOcb;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::PosixDevCtl( %ld)\n", msg->i.dcmd);

    switch( msg->i.dcmd)
    {
    case DCMD_CHR_TCSETATTRF:
        m_currentPortProtocol = dcp->tios;
        if( msg->i.nbytes > (int32_t)sizeof( dcp->tios))
        {
            m_currentPortProtocol.SetByteGap( dcp->bepAttr.m_byteGap);
            m_currentPortProtocol.SetMsgGap( dcp->bepAttr.m_msgGap);
            m_currentPortProtocol.SetReflection( dcp->bepAttr.m_reflection);
        }
        retVal = ConfigurePort( &dcp->tios, TCSAFLUSH);
        retVal = (retVal == -1 ? errno : EOK);
        break;
    case DCMD_CHR_TCSETATTRD:
        m_currentPortProtocol = dcp->tios;
        if( msg->i.nbytes > (int32_t)sizeof( dcp->tios))
        {
            m_currentPortProtocol.SetByteGap( dcp->bepAttr.m_byteGap);
            m_currentPortProtocol.SetMsgGap( dcp->bepAttr.m_msgGap);
            m_currentPortProtocol.SetReflection( dcp->bepAttr.m_reflection);
        }
        retVal = ConfigurePort( &dcp->tios, TCSADRAIN);
        msg->o.ret_val = 0;
        retVal = (retVal == -1 ? errno : EOK);
        break;
    case DCMD_CHR_TCSETATTR:
        m_currentPortProtocol = dcp->tios;
        if( msg->i.nbytes > (int32_t)sizeof( dcp->tios))
        {
            m_currentPortProtocol.SetByteGap( dcp->bepAttr.m_byteGap);
            m_currentPortProtocol.SetMsgGap( dcp->bepAttr.m_msgGap);
            m_currentPortProtocol.SetReflection( dcp->bepAttr.m_reflection);
        }
        retVal = ConfigurePort( &dcp->tios, TCSANOW);
        msg->o.ret_val = 0;
        retVal = (retVal == -1 ? errno : EOK);
        break;
    case DCMD_CHR_TCGETATTR:
        if( msg->i.nbytes > (int32_t)sizeof( dcp->tios))
        {
            dcp->bepAttr.m_byteGap = m_currentPortProtocol.GetByteGap();
            dcp->bepAttr.m_msgGap = m_currentPortProtocol.GetMsgGap();
            dcp->bepAttr.m_reflection = m_currentPortProtocol.GetReflection();
        }
        retVal = tcgetattr( m_comPortFd, &tempTio);
        if( retVal != -1)
        {
            dcp->tios.c_cflag = tempTio.c_cflag;
            dcp->tios.c_iflag = tempTio.c_iflag;
            dcp->tios.c_lflag = tempTio.c_lflag;
            dcp->tios.c_oflag = tempTio.c_oflag;
            dcp->tios.c_ispeed = tempTio.c_ispeed;
            dcp->tios.c_ospeed = tempTio.c_ospeed;
            memcpy(dcp->tios.c_cc, tempTio.c_cc, sizeof(tempTio.c_cc));
            msg->o.ret_val = 0;
            retVal = _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + sizeof(tempTio));
        }
        else
        {
            retVal = (retVal == -1 ? errno : EOK);
        }
        break;
    case DCMD_CHR_SERCTL:
        uint32_t    dur;
        dur = *((uint32_t*)_DEVCTL_DATA( *msg));
        // if we are handling a tcsendbreak()
        if(dur & _CTL_BRK_CHG)
        {
            // Strip out the duration
            dur >>= 16;
            // Execute the tcsendbreak for the client
            retVal = tcsendbreak( m_comPortFd, dur);
            retVal = (retVal == -1 ? errno : EOK);
            // Wait for the break to end
            delay( dur);
        }
        else
        {
            // TODO: Add support for DTR/RTS control

            // For now we'll only handle the tcsendbreak() messages
            retVal = ENOSYS;
        }
        break;
    case DCMD_CHR_TCFLUSH:
        if (clientOcb->rxSubscription != NULL)
            clientOcb->rxSubscription->fifo.Reset();
        if (clientOcb->txSubscription != NULL)
            clientOcb->txSubscription->fifo.Reset();
        retVal = FlushDriverFifos( dcp->data32);
        retVal = (retVal == -1 ? errno : EOK);
        break;
    case DCMD_CHR_TCDRAIN:
        retVal = tcdrain( m_comPortFd);
        retVal = (retVal == -1 ? errno : EOK);
        break;
    case DCMD_CHR_TCGETPGRP:
        retVal = dcp->data32 = tcgetpgrp( m_comPortFd);
        if (retVal == -1)
            retVal = errno;
        else
            retVal = _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + sizeof(dcp->data32));
        break;
    case DCMD_CHR_TCSETPGRP:
        retVal = tcsetpgrp( m_comPortFd, dcp->data32);
        retVal = (retVal == -1 ? errno : EOK);
        break;
    case DCMD_CHR_TCGETSID:
        retVal = ( m_comPortFd);
        retVal = dcp->data32 = tcgetsid( m_comPortFd);
        if (retVal == -1)
            retVal = errno;
        else
            retVal = _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + sizeof(dcp->data32));
        break;
    case DCMD_CHR_TCSETSID:
        retVal = tcsetsid( m_comPortFd, dcp->data32);
        retVal = (retVal == -1 ? errno : EOK);
        break;
    case DCMD_CHR_TTYINFO:
        tempName = ttyname( m_comPortFd);
        if( tempName != NULL)
        {
            strncpy( dcp->ttyinfo.ttyname, tempName, TTY_NAME_MAX);
            retVal = _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + sizeof(dcp->ttyinfo));
        }
        else
        {
            retVal = errno;
        }
        break;
    case DCMD_CHR_ISATTY:
        if (isatty(m_comPortFd) == 0)
            retVal = ENOTTY;
        else
            retVal = EOK;
        break;
    case DCMD_CHR_GETSIZE:
        retVal = tcgetsize( m_comPortFd, (int*)&dcp->winsize.ws_row, (int*)&dcp->winsize.ws_col);
        if (retVal == -1)
            retVal = errno;
        else
            retVal = _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + sizeof(dcp->winsize));
        break;
    case DCMD_CHR_SETSIZE:
        retVal = tcsetsize( m_comPortFd, dcp->winsize.ws_row, dcp->winsize.ws_col);
        retVal = (retVal == -1 ? errno : EOK);
        break;
    case DCMD_CHR_ISCHARS:
//      retVal = dcp->data32 = tcischars( m_comPortFd);
        retVal = dcp->data32 = clientOcb->rxSubscription->fifo.GetSize();
        if (retVal == -1)
            retVal = errno;
        else
            retVal = _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + sizeof(dcp->data32));
        break;
    default:
        // Just pass the request through to the serial driver
        retVal = devctl( m_comPortFd, msg->i.dcmd, msg, msg->i.nbytes, &msg->o.ret_val);
        retVal = (retVal == -1 ? errno : EOK);
        break;
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::PosixDevCtl( %ld)\n", msg->i.dcmd);

    return( retVal);
}

/**
 * Handler responsible for installing, polling, or removing a notification
 * handler. Called by ioNotify
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Notification message
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioNotify
 */
int RawCommProxy::IoNotify(resmgr_context_t *ctp, io_notify_t *msg, resMgrIoOcb_t *ioOcb)
{
    CommIoOcb_t *clientOcb = (CommIoOcb_t*)ioOcb;
    int         retVal;
    int         trigger = _NOTIFY_COND_OUTPUT;
    int         trigLevel[] = {1,1,1};

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::IoNotify( pid %d, tid %d)\n", ctp->info.pid, ctp->info.tid);

    // If client's read FIFO has any data ready to read
    if( clientOcb->rxSubscription->fifo.GetSize() > 0)
    {
        // Notify client immediately of data ready
        trigger |= _NOTIFY_COND_INPUT;
    }

    retVal = iofunc_notify( ctp, msg, clientOcb->notify, trigger, trigLevel, &trigger);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::IoNotify( pid %d, tid %d)\n", ctp->info.pid, ctp->info.tid);

    return( retVal);
}

/**
 * IoRead helper function used to reply to a read() request when a client
 * has data available in his fifo
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Read message data
 * @param ioOcb  Control Block for this client
 * @see IoRead
 */
void RawCommProxy::ReplyToReadClient(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
    CommIoOcb_t *clientOcb = (CommIoOcb_t*)ioOcb;
    int     err;

    if( (err=clientOcb->rxSubscription->fifo.Lock()) == EOK)
    {
        uint32_t    count = clientOcb->rxSubscription->fifo.GetSize();

        // Calculate how many bytes to transfer from client's FIFO
        if (count > (uint32_t)msg->i.nbytes)
            count = msg->i.nbytes;

        // If we need to transfer any data
        if(count > 0)
        {
            // Temp buffer
            const int buffLen = count;
            uint8_t buff[ buffLen];

            // Copy FIFO to temp buffer
            clientOcb->rxSubscription->fifo.GetBytes( buff, count);

            // Tell the client how many bytes were read
            _IO_SET_READ_NBYTES( ctp, count);

            // Reply to client with data
            MsgReply( ctp->rcvid, count, buff, count);
        }
        else
        {
            // Tell the client how many bytes were read
            _IO_SET_READ_NBYTES( ctp, count);

            // Reply to client with data
            MsgReply( ctp->rcvid, 0, NULL, 0);
        }

        // Update time attributes for our "file"
        ((iofunc_attr_t*)(ioOcb->m_ioOcb.attr))->flags |= IOFUNC_ATTR_ATIME |
                                                          IOFUNC_ATTR_DIRTY_TIME;
        clientOcb->rxSubscription->fifo.Unlock();
    }
    else
    {
        Log( LOG_ERRORS, "\tError %d locking client's fifo in ReplyToReadClient: %s\n",
             err, strerror( err));
    }
}

/**
 * Client request to lock port for exclusive use
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int RawCommProxy::LockPortHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                  resMgrIoOcb_t *ioOcb)
{
    logPortMsg_t        *portMsg = (logPortMsg_t*)_DEVCTL_DATA(*msg);
    logPortLockPort_t   &lockMsg = portMsg->lockPort;
    CommIoOcb_t         *clientOcb = (CommIoOcb_t*)ioOcb;
    int                 retVal = _RESMGR_STATUS( ctp, EBUSY);
    bool                blockWait = (lockMsg.flags & O_NONBLOCK) != O_NONBLOCK;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::LockPortHandler()\n");

    // If client does not want to block
    retVal = ObtainClientPortLock( clientOcb, ctp->rcvid, blockWait);
    // If we were able to process the request right now
    if( retVal != -1)
    {
        // Return the status
        retVal = _RESMGR_STATUS( ctp, retVal);
    }
    else
    {
        Log( LOG_DEV_DATA, "\tNOT replying to client's lock request\n");
        retVal = _RESMGR_NOREPLY;
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::LockPortHandler()\n");

    return( retVal);
}

/**
 * Client request to unlock the port. Client must have completed a
 * successful lock port command prior to this.
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int RawCommProxy::UnLockPortHandler( resmgr_context_t *ctp, io_devctl_t *msg,
                                     resMgrIoOcb_t *ioOcb)
{
    CommIoOcb_t *clientOcb = (CommIoOcb_t*)ioOcb;
    int         retVal;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::UnLockPortHandler()\n");

    // Release the port lock (no-op if client does not own the lock)
    if( ReleaseClientPortLock( clientOcb, false) == true)
    {
        retVal = _RESMGR_STATUS( ctp, EOK);
    }
    else
    {
        retVal = _RESMGR_STATUS( ctp, errno);
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::UnLockPortHandler()\n");

    return( retVal);
}

/**
 * Method used to obtain a port lock for a client
 *
 * @param clientOcb Client OCB containing flag indicating if he already owns the port lock
 * @param replyId   If the port cannot be locked immediately, this is the connection ID
 *                  to respond to when the port is locked for the calling client
 * @param block     Flag indicating if the port lock request should block if the port
 *                  cannot be locked immediately.
 *
 * @return EOK if port successfully locked, other values indicate error, -1 indicates that we
 *         should not reply to the client
 */
int RawCommProxy::ObtainClientPortLock( CommIoOcb_t *clientOcb, int replyId, bool block)
{
    int err = EOK;
    int lockErr;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ObtainClientPortLock()\n");

    // Lock the condvar while checking its value
    lockErr = m_portLock.Acquire();
    // If we locked the port (or already have the port locked)
    if( (EOK == lockErr) || (EDEADLK == lockErr))
    {
        // If client does not already have port locked
        if( false == ClientOwnsPortLock(clientOcb))
        {
            try
            {
                // Add this client to the end of the "waiting to lock" list
                int waitCount = m_lockWaitList.AddClient(clientOcb, replyId);

                // If port currently not locked and we are the only entry so far
                if( (m_portLock.GetValue() == false) && ( waitCount == 1))
                {
                    Log( LOG_DEV_DATA, "Port currently unlocked...lock for next client\n");
                    // Try to lock the port
                    err = LockForNextClient( false);
                }
                // Port is currently locked or someone
                else if( block == false)
                {
                    Log( LOG_ERRORS, "\tPort currently locked in ObtainClientPortLock()\n");
                    // Do not block...so remove client from the list
                    m_lockWaitList.PopClient( clientOcb);
                    err = EBUSY;
                }
                else
                {
                    Log( LOG_DEV_DATA, "Port currently locked...wait for it\n");
                    err = -1;
                }
            }
            catch( BepException &bepErr)
            {
                Log( LOG_ERRORS, "\tError adding lock client: %s\n", bepErr.GetReason());
                err = EIO;
            }
        }
        else
        {
            // Add another lock for this client
            clientOcb->portLockCount++;
            Log( LOG_DEV_DATA, "Adding 1 to existing lock count: %d\n",
                 clientOcb->portLockCount);
        }

        if (EOK == lockErr)
            m_portLock.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tUnable to obtain condvar lock in ObtainClientPortLock(): %s\n",
             strerror( lockErr));
        errno = err = lockErr;
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ObtainClientPortLock()\n");

    return( err);
}

/**
 * Method used to release a port lock for a client
 *
 * @param clientOcb Client OCB containing flag indicating if he already owns the port lock
 * @param releaseAll Flag to indicate if we should release all locks on the port help by
 *                   the client or just a single lock (set =0 vs. decrement by 1)
 * @return true if port lock released, false if unable to release port lock
 */
bool RawCommProxy::ReleaseClientPortLock( CommIoOcb_t *clientOcb, bool releaseAll)
{
    bool    retVal = false;
    bool    getNextLockClient = false;
    int     lockErr, waitCount;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReleaseClientPortLock()\n");

    // Lock the condvar while checking its value
    lockErr = m_portLock.Acquire();
    // If we locked the port (or already have the port locked)
    if( (EOK == lockErr) || (EDEADLK == lockErr))
    {
        // Make sure port is locked and that this client owns the lock
        if( true == ClientOwnsPortLock(clientOcb))
        {
            // Release the port lock for this client
            if( UnlockForClient( clientOcb, releaseAll) == 0)
            {
                Log( LOG_DEV_DATA, "Client has no more port locks\n");

                // Signal any waiting threads of new unlock status
                m_portLock.Signal( false);

                // If no clients waiting to lock the port
                if( (waitCount=m_lockWaitList.size()) == 0)
                {
                    Log( LOG_DEV_DATA, "No clients waiting to lock the port\n");
                    // Update LOCK activity subscribers
                    MsgSendPulse( m_mssgCoid, SIGEV_PULSE_PRIO_INHERIT,
                                  LOG_PORT_NOTIFY_PULSE_CODE, PORT_SUBSCRIBE_LOCK);
                }
                else
                {
                    Log( LOG_DEV_DATA, "%d clients waiting to lock the port\n",
                         waitCount);
                    getNextLockClient = true;
                }
            }
            else
            {
                Log( LOG_DEV_DATA, "Port still locked...count: %d\n",
                     clientOcb->portLockCount);
            }

            // Return success to client
            retVal = true;
        }

        // if we need to lock again for a waiting client
        if( getNextLockClient)
        {
            LockForNextClient();
        }

        if (EOK == lockErr)
            m_portLock.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tUnable to obtain condvar lock in CanClientWrite: %s\n",
             strerror( lockErr));
        errno = lockErr;
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReleaseClientPortLock()\n");

    return( retVal);
}

/**
 * Locks the port for the next waiting client in m_lockWaitlist
 *
 * @param sendReply Flag indicating if we should reply to the client or not
 * @return EOK if port successfully locked
 */
int RawCommProxy::LockForNextClient( bool sendReply)
{
    CommIoOcb_t     *nextClient;
    int             status=EBUSY, replyId;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::LockForNextClient()\n");

    // If port currently not locked
    if( m_portLock.GetValue() == false)
    {
        // Get the next client from the lsit
        if( (status = m_lockWaitList.GetNextClient( nextClient, replyId)) == EOK)
        {
            Log( LOG_DEV_DATA, "Port currently unlocked...let client have it\n");

            // If we dont need to send a relpy to the client
            if (sendReply == false)
                replyId = -1;

            status = LockForClient( nextClient, replyId);
        }
        else
        {
            Log( LOG_DEV_DATA, "No more clients waiting to lock the port: %s\n",
                 strerror( status));
        }
    }
    else
    {
        Log( LOG_DEV_DATA, "Port is currently locked\n");

    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::LockForNextClient()\n");

    return( status);
}

/**
 * Locks the port for the client associated with 'clientOcb'
 *
 * @param clientOcb OCB associated with the client for whom we are locking the port
 * @param replyId   The reply ID to which we should reply after having locked the port
 *
 * @return EOK if port successfully locked
 */
int RawCommProxy::LockForClient( CommIoOcb_t *clientOcb, int replyId)
{
    int     status=EBUSY;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::LockForClient( %d, %d), lock count=%d\n",
         clientOcb->baseOcb.mssgInfo.pid, clientOcb->baseOcb.mssgInfo.tid, clientOcb->portLockCount);

    // If port currently not locked
    if( m_portLock.GetValue() == false)
    {
        Log( LOG_DEV_DATA, "Incrementing client port lock count\n");

        // Signal the port as locked
        m_portLock.Signal( true);
        // Increment port lock count
        clientOcb->portLockCount++;

        // If we have a valid reply ID to send the lock status to
        if( replyId > 0)
        {
            // Reply to client that port is locked
            MsgReply( replyId, EOK, NULL, 0);
            // Tell res. manager library to not reply to client
            status = _RESMGR_NOREPLY;
        }
        else
        {
            Log( LOG_DEV_DATA, "Not replying to client\n");
            // Say port successfully locked
            status = EOK;
        }
    }
    else
    {
        Log( LOG_DEV_DATA, "Port is currently locked\n");

    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::LockForClient( %d, %d), lock count=%d\n",
         clientOcb->baseOcb.mssgInfo.pid, clientOcb->baseOcb.mssgInfo.tid, clientOcb->portLockCount);

    return( status);
}

/**
 * Unlocks the port for the client associated with 'clientOcb'
 *
 * @param clientOcb  OCB associated with the client for whom we are locking the port
 * @param releaseAll Flag to indicate if we should release all locks on the port help by
 *                   the client or just a single lock (set =0 vs. decrement by 1)
 *
 * @return The number of port locks the client still holds
 */
int RawCommProxy::UnlockForClient( CommIoOcb_t *clientOcb, bool releaseAll)
{
    Log( LOG_FN_ENTRY, "Enter RawCommProxy::UnlockForClient( %d)\n", clientOcb->portLockCount);

    // Release all of this client's locks on the port
    if (releaseAll)
        clientOcb->portLockCount = 0;
    // Release one of this client's locks on the port
    else
        clientOcb->portLockCount--;

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::UnlockForClient(%d)\n", clientOcb->portLockCount);

    return( clientOcb->portLockCount);
}


/**
 * Arms a reply timeout timer for the client designated by 'ocb'
 *
 * @param clientOcb The OCB of the client we are arming the reply timeout timer for
 * @param forceArm  Flag touse if we want to unconditionally arm the timer (deafults to false)
 * @return 0 if timer was armed, any other value if not armed
 */
int RawCommProxy::ArmReplyTimeoutTimer( CommIoOcb_t *clientOcb, bool forceArm)
{
    int retVal = 0;
    uint64_t    replyTimeout;

    // Default to our default timeout value
    replyTimeout = m_defaultReplyTimeout;
    // If the client has a custom timeout value, use it
    if (clientOcb->replyTimeout)
        replyTimeout = clientOcb->replyTimeout;

    // IF we have a valid reply timout time
    if( replyTimeout > 0)
    {
        // Arm the reply timeout timer
        Log( LOG_DEV_DATA, "Arming reply timeout timer for %lld ns\n", replyTimeout);

        clientOcb->replyTimeoutTimer.Arm( replyTimeout);
    }
    // TODO: Do we need this
    clientOcb->replyTimeout = m_defaultReplyTimeout;

    return( retVal);
}

/**
 * Method called to read received data from the port
 *
 * @param buff   Buffer to put the received data into
 * @param buffSz Size (in bytes) of the buff parameter
 * @return The number of bytes received from the port
 */
int RawCommProxy::ReadPortDataUnlocked(uint8_t *buff, size_t buffSz)
{
    int         bytesRead;
    uint32_t    totalRead=0;
    int         err;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReadPortDataUnlocked()\n");

    while( totalRead < buffSz)
    {
        // If we read anything from the fifo
        Log(LOG_DEV_DATA, "Getting Bytes totalRead: %d, buffSz: %d\n", totalRead, buffSz);
        if( (bytesRead=m_rxFifo->GetBytes(&buff[ totalRead], buffSz-totalRead)) <= 0)
        {
            break;
        }
        totalRead += bytesRead;
    }

    if( totalRead > 0)
    {
        // Log communication for this client
        UpdateBusCommLog( ComDirRx, buff, totalRead, NULL);

        Log( LOG_DEV_DATA, "\tWait to lock m_reflectionCount\n");
        if( (err=m_reflectionCount.Acquire()) == EOK)
        {
            Log( LOG_DEV_DATA, "\tm_reflectionCount locked\n");

            // Check for and handle reflection bytes
            ProcessReflection( buff, totalRead);

            Log( LOG_DEV_DATA, "\tUnlocking m_reflectionCount\n");
            m_reflectionCount.Release();
        }
        else
        {
            Log( LOG_ERRORS, "\tError locking reflection mutex in ReadPortDataUnlocked: %s\n",
                 strerror( err));
        }

        // Log the received data
        PrintSerialString( LOG_DEV_DATA, "Read", buff, totalRead);
    }


    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReadPortDataUnlocked(%d)\n", totalRead);

    return( totalRead);
}

/**
 * Reads serial data from the serial driver
 *
 * @param buff    Buffer to read the serial data into
 * @param buffLen Number of bytes to read
 * @param timeOut Timeout value (in ms)
 * @return Number of bytes read, or -1 if an error occurred
 */
int RawCommProxy::ReadDriverData( uint8_t *buff, size_t buffLen, uint32_t timeOut)
{
    uint64_t    nsTimeOut = mSEC_nSEC( timeOut);

    return( ReadDriverData( buff, buffLen, nsTimeOut));
}

/**
 * Reads serial data from the serial driver
 *
 * @param buff    Buffer to read the serial data into
 * @param buffLen Number of bytes to read
 * @param timeOut Timeout value (in nano-seconds)
 * @return Number of bytes read, or -1 if an error occurred
 */
int RawCommProxy::ReadDriverData( uint8_t *buff, size_t buffLen, const uint64_t &timeOut)
{
    int         retVal = -1, bytesRead;
    uint32_t    totalRead = 0, bytesAvail;
    uint64_t    timeRemaining;
    struct sigevent event;

    event.sigev_notify = SIGEV_UNBLOCK;

    if( (bytesRead=m_comGuard.Acquire()) == EOK)
    {
        while( totalRead < buffLen)
        {
            // See how much data the driver has available right now
            bytesAvail = DriverBytesAvailable();

            // If driver does not have any data available right now
            if( (int)bytesAvail <= 0)
            {
                // Perform a blocking read with the timeout passed in
                bytesAvail = buffLen-totalRead;
                Log( LOG_DETAILED_DATA, "\tWait up to %lld ns for %d data bytes\n", timeOut, bytesAvail);
                TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_SEND|_NTO_TIMEOUT_REPLY, &event, &timeOut, &timeRemaining);
            }
            else
            {
                // Driver has data available...read what is there
                Log( LOG_DETAILED_DATA, "\tDriver has %d bytes available\n", bytesAvail);
                // Don't read more than we can handle
                bytesAvail = min( bytesAvail, buffLen-totalRead);
            }

            // Try to read data from the driver
            if((bytesRead=Read( &buff[ totalRead], bytesAvail)) > 0)
            {
                totalRead += bytesRead;
            }
            else
            {
                // If an error occurred
                if( bytesRead < 0)
                {
                    if( (errno != EINTR) && (errno != ETIMEDOUT))
                    {
                        Log( LOG_ERRORS, "\tError %d reading driver data %s\n", errno, strerror( errno));
                    }
                    // Leave on errors
                    break;
                }
                else
                {
                    // No data available (not sure why)
                    Log( LOG_ERRORS, "\tread(%d) returned %d bytes after %lld ns\n", bytesAvail, bytesRead, timeOut-timeRemaining);
                    delay( 1);
                }
            }
            Log( LOG_DEV_DATA, "\tRead %d, %d bytes\n", bytesRead, totalRead);
        }

        Log( LOG_SER_COM, "Received %d data bytes\n", totalRead);

        m_comGuard.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError %d locking com guard: %s\n", bytesRead, strerror( bytesRead));
    }

    // If no data read
    if( (bytesRead == 0) && (totalRead == 0))
    {
        retVal = 0;
    }
    else if( totalRead > 0)
    {
        retVal = totalRead;
    }

    return( retVal);
}

/**
 * Reads bytes from the driver
 * 
 * @param buf    Buffer to read the bytes into
 * @param bytes  Maximum number of bytes to try and read
 * 
 * @return The number of bytes actually read, or -1 (errno is set).
 */
int RawCommProxy::Read( void *buf, size_t bytes)
{
    return( read( m_comPortFd, buf, bytes));
}

/**
 * Checks how many bytes are available for reading from the driver
 * 
 * @return How many bytes are available for reading from the driver
 */
int RawCommProxy::DriverBytesAvailable()
{
    // See how much data the driver has available right now
    return( tcischars( m_comPortFd));
}

/**
 * Handles pulses delivered to our resource manager I/O channel
 *
 * @param pulse  Pulse received on the I/O channel
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t RawCommProxy::HandleResMgrPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;

    Log( LOG_FN_ENTRY, "Enter HandleResMgrPulse( %d, %d)\n", pulse.code,
         pulse.value.sival_int);
    // See which pulse we are getting
    switch( pulse.code)
    {
    case LOG_PORT_NOTIFY_PULSE_CODE:    // Need to update subscribers
        UpdateSubscribers( pulse.value.sival_int);
        break;
    case MSSG_GAP_TIMER:
        retVal = HandleMssgGapPulse( pulse);
        break;
    case REPLY_TIMEOUT_TIMER:
        retVal = HandleReplyTimeoutPulse( pulse);
        break;
    case SERIAL_DATA_READY:
        // If I just read any data or if there is data waiting in my FIFO
        if( (pulse.value.sival_int > 0) || (m_rxFifo->GetSize() > 0))
        {
            ProcessReadData();
        }
        break;
    case REGISTER_PULSE:
        Register();
        break;

    case _PULSE_CODE_UNBLOCK:
        int replyId;
        replyId = pulse.value.sival_int;
        m_blockedReaders.RemoveBlockedReader( replyId);
        MsgError( replyId, ETIMEDOUT);
        break;
    default:
        if( (pulse.code == SYSTEM_PULSE) && (pulse.value.sival_int == TERMINATE_PULSE))
        {
            Log( LOG_DEV_DATA, "\n\tGot TERMINATE_PULSE\n\n");
        }
        retVal = ResManagerBase::HandleResMgrPulse(pulse);
        break;
    }
    Log( LOG_FN_ENTRY, "Exit HandleResMgrPulse( %d, %d)\n", pulse.code,
         pulse.value.sival_int);

    return( retVal);
}

/**
 * Handler for driver 'data ready to read' notification pulses.
 *
 * @param pulse  Pulse received on the I/O channel
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t RawCommProxy::HandleDriverNotificationPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    uint8_t         buff[m_fifoSize];
    int             bytesRead, totalRead=0;

    Log( LOG_FN_ENTRY, "Enter HandleDriverNotificationPulse( %d, %d)\n", pulse.code,
         pulse.value.sival_int);
    if( (bytesRead=m_comGuard.Acquire()) == EOK)
    {
        // Read (up to) 128 bytes from the driver with a 0ms timeout
        while ((bytesRead = ReadDriverData(buff, m_fifoSize, (uint32_t)0)) > 0)
        {
            // Add newly received data to rx fifo
            m_rxFifo->AddBytes( buff, bytesRead);
            totalRead += bytesRead;

            PrintSerialString( LOG_SER_COM, "Comm Message", buff, bytesRead);
        }
        m_comGuard.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tUnable to lock driver in HandleDriverNotificationPulse(): %s\n", strerror( bytesRead));
    }

#if READER_PROCESSES == 1
    // If I just read any data or if there is data waiting in my FIFO
    if( (totalRead > 0) || (m_rxFifo->GetSize() > 0))
    {
        ProcessReadData();
    }
#else
    // Tell the thread pool to porcess new data
    MsgSendPulse( m_mssgCoid, SIGEV_PULSE_PRIO_INHERIT, SERIAL_DATA_READY, totalRead);
#endif
    Log( LOG_FN_ENTRY, "Exit HandleDriverNotificationPulse( %d, %d)\n", pulse.code,
         pulse.value.sival_int);

    return( retVal);
}

/**
 * Handler for MSSG_GAP_TIMER pulses
 *
 * @param pulse  Pulse received on the I/O channel
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t RawCommProxy::HandleMssgGapPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    int         nbytes = 0;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::HandleMssgGapPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    // If we have at least one client waiting to write
    if( m_blockedWriters.empty() == false)
    {
        m_blockedWriters.Lock();

        // Get the next client waiting to send data
        BlockedWriter_t writer = m_blockedWriters.front();
        // remove this client from the blocked writer list
        m_blockedWriters.pop_front();

        m_blockedWriters.Unlock();

        // Write the message
        nbytes = JustWrite( writer.ctp,
                            (resMgrIoOcb_t*)writer.ocb,
                            writer.writeMsg.c_str(),
                            writer.writeMsg.length());

        // Reply to the client to unblick him
        if (nbytes >= 0)
            MsgReply(writer.replyId, nbytes, NULL, 0);
        else
            MsgError(writer.replyId, errno);

        /////////////////////////////////////////////////
        //
        // Dont clear m_mssgGapExpired until all blocked
        //  writers have finished
        //
        /////////////////////////////////////////////////
    }
    // No more clients waiting to write
    else
    {
        // say the message gap has passed
        m_mssgGapExpired = true;
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::HandleMssgGapPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    return( retVal);
}

/**
 * Handler for module reply timeout error pulses
 *
 * @param pulse  Pulse received on the I/O channel
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t RawCommProxy::HandleReplyTimeoutPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    BlockedReaderListCItr_t itr;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::HandleReplyTimeoutPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    Log( LOG_DEV_DATA, "Locking blocked reader list\n");
    if( (errno=m_blockedReaders.Lock()) == EOK)
    {
        // Unblock all clients blocked on a read
        for( itr=m_blockedReaders.begin(); itr!=m_blockedReaders.end(); itr++)
        {
            //const CommIoOcb_t         *ocb = itr->first;
            const BlockedReader_t   &client = itr->second;

            // Tell the client that the read timed out, and remove it from the list
            (void)NotifyClientErr( client, ETIMEDOUT);
        }

        Log( LOG_DEV_DATA, "Clearing blocked reader list\n");
        m_blockedReaders.clear();
        Log( LOG_DEV_DATA, "Unlocking blocked reader list\n");
        m_blockedReaders.Unlock();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking blocked reader list: %s\n", strerror( errno));
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::HandleReplyTimeoutPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    return( retVal);
}

/**
 * Reads all available data from the serial port
 *
 * @param mssg   Buffer to read the serial data into
 * @return Number of bytes read from the port
 */
int RawCommProxy::ReadSerialData( SerialString_t &mssg)
{
    int         bytesRead;
    uint8_t     buff[ 1024];

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReadSerialData()\n");

    if( (m_comGuard.Acquire()) == EOK)
    {
        // Loop until no more data to read
        while( (bytesRead=m_rxFifo->GetBytes( buff, 1024)) > 0)
        {
            mssg += SerialString_t( buff, bytesRead);
        }

        m_comGuard.Release();
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReadSerialData( %d)\n", mssg.length());

    return( mssg.length());
}

/**
 * Notifies a read-blocked client of a message error
 *
 * @param ocb     OCB of the client to whom we need to repond
 * @param errCode Error code to respond with
 */
bool RawCommProxy::NotifyClientErr( const CommIoOcb_t *ocb, int errCode)
{
    BlockedReader_t     client;
    bool clientRemoved = false;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::NotifyClientErr()\n");

    // Check if any clients are "read" blocked on this module
    if( (errno=m_blockedReaders.Lock()) == EOK)
    {
        // Check if any clients are "read" blocked on this module
        if( m_blockedReaders.FindBlockedReader( ocb, client) != false)
        {
            // Check if client is in blocked reader list
            m_blockedReaders.RemoveBlockedReader( client.replyId);

            Log( LOG_DEV_DATA, "Unblocking client %lld\n", ocb->rxSubscription->id);
            NotifyClientErr( client, errCode);

            // flag that the client was removed
            clientRemoved = true;
        }
        else
        {
            Log( LOG_DEV_DATA, "Unable to find client %lld in blocked reader list\n",
                ocb->rxSubscription->id);
        }

        m_blockedReaders.Unlock();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking blocked readers list: %s\n", strerror( errno));
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::NotifyClientErr( %d)\n", clientRemoved);

    return(clientRemoved);
}

/**
 * Notifies a read-blocked client of a message error
 *
 * @param client  Blocked reader client to be inblocked
 * @param errCode Error code to respond with
 *
 * @return true
 */
bool RawCommProxy::NotifyClientErr( const BlockedReader_t &client, int errCode)
{
    resMgrIoOcb_t     *clientOcb = (resMgrIoOcb_t*)client.ocb;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::NotifyClientErr()\n");

    if( clientOcb != NULL)
    {
    Log( "Replying to client (%d, %d) error %s\n",
         clientOcb->mssgInfo.pid, clientOcb->mssgInfo.tid, strerror( errCode));
    }
    MsgError( client.replyId, errCode);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::NotifyClientErr()\n");
    return(true);
}

/**
 * Sends subscriptions notifications to all clients subscribed for events
 * contained in the eventMask parameter
 *
 * @param eventMask Bitwise flag indicating which port event(s) occurred
 */
void RawCommProxy::UpdateSubscribers( int eventMask)
{
    PortSubscrListItr_t itr;
    CommIoOcb_t     *clientOcb;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::UpdateSubscribers( %d)\n", eventMask);

    if( eventMask & PORT_SUBSCRIBE_RX)
    {
        Log( LOG_DEV_DATA, "Notifying Rx subscribers: %d\n", m_rxSubscribers.size());
        m_rxSubscribers.Lock();
        for( itr=m_rxSubscribers.begin(); itr!=m_rxSubscribers.end(); itr++)
        {
            clientOcb = itr->second;
            if( ShouldNotifyClient( clientOcb, eventMask) & PORT_SUBSCRIBE_RX)
            {
                NotifyEvent_t &ev = clientOcb->rxSubscription->event;
                // If client wants to use a pulse for notifications
                if(ev.sigev_notify == SIGEV_PULSE)
                {
                    // Set pulse value to proper event
                    ev.sigev_value.sival_int = PORT_SUBSCRIBE_RX;
                }
                MsgDeliverEvent( clientOcb->rxSubscription->coId, &ev);
                Log( LOG_DEV_DATA, "Notified Rx subscriber on channel '%d'\n",
                     clientOcb->rxSubscription->coId);
            }
            else
            {
                Log( LOG_DEV_DATA, "\tNot notifying RX subscriber %lld\n",
                     clientOcb->rxSubscription->id);
            }
        }
        m_rxSubscribers.Unlock();
    }

    if( eventMask & PORT_SUBSCRIBE_TX)
    {
        Log( LOG_DEV_DATA, "Notifying Tx subscribers: %d\n", m_txSubscribers.size());
        m_txSubscribers.Lock();
        for( itr=m_txSubscribers.begin(); itr!=m_txSubscribers.end(); itr++)
        {
            clientOcb = itr->second;
            if( ShouldNotifyClient( clientOcb, eventMask) & PORT_SUBSCRIBE_TX)
            {
                NotifyEvent_t &ev = clientOcb->txSubscription->event;
                // If client wants to use a pulse for notifications
                if(ev.sigev_notify == SIGEV_PULSE)
                {
                    // Set pulse value to proper event
                    ev.sigev_value.sival_int = PORT_SUBSCRIBE_TX;
                }
                MsgDeliverEvent( clientOcb->txSubscription->coId, &ev);
                Log( LOG_DEV_DATA, "Notified Tx subscriber on channel '%d'\n",
                     clientOcb->rxSubscription->coId);
            }
        }
        m_txSubscribers.Unlock();
    }

    if( eventMask & PORT_SUBSCRIBE_LOCK)
    {
        Log( LOG_DEV_DATA, "Notifying Lock subscribers: %d\n", m_lockSubscribers.size());
        m_lockSubscribers.Lock();
        for( itr=m_lockSubscribers.begin(); itr!=m_lockSubscribers.end(); itr++)
        {
            clientOcb = itr->second;
            if( ShouldNotifyClient( clientOcb, eventMask) & PORT_SUBSCRIBE_LOCK)
            {
                NotifyEvent_t &ev = clientOcb->lockSubscription->event;
                // If client wants to use a pulse for notifications
                if(ev.sigev_notify == SIGEV_PULSE)
                {
                    // Set pulse value to proper event
                    ev.sigev_value.sival_int = PORT_SUBSCRIBE_LOCK;
                }
                MsgDeliverEvent( clientOcb->lockSubscription->coId, &ev);
                Log( LOG_DEV_DATA, "Notified Lock subscriber on channel '%d'\n",
                     clientOcb->rxSubscription->coId);

            }
        }
        m_lockSubscribers.Unlock();
    }

    if( eventMask & PORT_SUBSCRIBE_UNLOCK)
    {
        Log( LOG_DEV_DATA, "Notifying Unlock subscribers: %d\n", m_unlockSubscribers.size());
        m_unlockSubscribers.Lock();
        for( itr=m_unlockSubscribers.begin(); itr!=m_unlockSubscribers.end(); itr++)
        {
            clientOcb = itr->second;
            if( ShouldNotifyClient( clientOcb, eventMask) & PORT_SUBSCRIBE_UNLOCK)
            {
                NotifyEvent_t &ev = clientOcb->unlockSubscription->event;
                // If client wants to use a pulse for notifications
                if(ev.sigev_notify == SIGEV_PULSE)
                {
                    // Set pulse value to proper event
                    ev.sigev_value.sival_int = PORT_SUBSCRIBE_UNLOCK;
                }
                MsgDeliverEvent( clientOcb->unlockSubscription->coId, &ev);
                Log( LOG_DEV_DATA, "Notified Unlock subscriber on channel '%d'\n",
                     clientOcb->rxSubscription->coId);
            }
        }
        m_unlockSubscribers.Unlock();
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::UpdateSubscribers( %d)\n", eventMask);
}

/**
 * Method called in response to a "data ready" pulse from the reader thread
 *
 * @return RES_MGR_CMD_NONE if successful,
 *         RES_MGR_CMD_TERMINATE on error
 */
ResManagerCmd_t RawCommProxy::ProcessReadData()
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    const size_t    buffSz = (size_t)m_fifoSize; //2048;//1024;
    uint8_t         buff[ buffSz];
    size_t          readCount=0;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ProcessReadData()\n");
    // Read the received data from the port
    if( (readCount=ReadPortData( buff, buffSz)) > 0)
    {
        // Stick new data into subscribed client's FIFO's
        UpdateClientFifos( (const char*)buff, readCount, (int)PORT_SUBSCRIBE_RX);
    }

    Log( LOG_SER_COM, "RX %d bytes of data\n", readCount);

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ProcessReadData()\n");

    return( retVal);
}

/**
 * Handler method for client subscription requests
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int RawCommProxy::PortSubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                       resMgrIoOcb_t *ioOcb)
{
    logPortMsg_t            *portMsg = (logPortMsg_t*)_DEVCTL_DATA( msg->i);
    logPortSubscribe_t      &subMsg = portMsg->portSubscribe;
    logPortSubscribeReply_t &reply = portMsg->portSubscribeReply;
    CommIoOcb_t             *clientOcb = (CommIoOcb_t*)ioOcb;
    int                     retVal;
    LogPortSubscriptionId_t subId = LOG_PORT_INVALID_SUB;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::PortSubscribeHandler(), size = %d\n", ctp->size);

    // Need to lock this operation to avoid duplicate subscription ID's
    if( (retVal = pthread_mutex_lock( &m_dataLock)) == EOK)
    {
        clientOcb->subMask |= subMsg.flags;

        // If subscribing for received data
        if( subMsg.flags & PORT_SUBSCRIBE_RX)
        {
            // If not already subscribed for RX data
            if( clientOcb->rxSubscription->ShouldNotify() == false)
            {
                // Get a reference to the existing RX subscription
                clientOcb->rxSubscription->count = subMsg.byteCount;
                clientOcb->rxSubscription->SetNotifyEvent( subMsg.notifyEvent, ctp->rcvid);
                clientOcb->rxSubscription->notifyModeMask = subMsg.modeMask;

                // If there is a filter string specified for this subscription
                if( (subMsg.flags & PORT_NOTIFY_FILTER) && (subMsg.filterByteCount > 0))
                {
                    Log( LOG_DEV_DATA, "Adding Rx Filter String for coId %d\n", ctp->rcvid);
                    Log( LOG_DEV_DATA, "Rx Filter String length = %d\n", subMsg.filterByteCount);

                    // Copy the desired filter string
                    LogPortFilterString filterString;
                    //filterString.reserve( subMsg.filterByteCount);
                    for( uint32_t ii=0; ii<subMsg.filterByteCount; ii++)
                    {
                        LogPortFilterByte   fb = LogPortFilterByte( subMsg.filterString[ ii]);
                        filterString.push_back( fb);
                        Log( LOG_DEV_DATA, "\tAdded filter byte %d: $%02X $%02X %d\n",
                             ii, fb.m_fbVal, fb.m_mask, fb.m_isWildCard);
                    }
                    // Add filter string to the clients filter list
                    clientOcb->rxSubscription->filterList.AddFilterString(filterString);
                    Log( LOG_DEV_DATA, "Added Rx Filter String for coId %d\n", ctp->rcvid);
                }

                // Already subscribed for rx data
                reply.flags |= PORT_SUBSCRIBE_RX;
                Log( LOG_DEV_DATA, "Added Rx subscriber() for coId %d\n", ctp->rcvid);
            }
        }

        // If subscribing for transmit data
        if( (subMsg.flags & PORT_SUBSCRIBE_TX) && (clientOcb->txSubscription == NULL))
        {
            clientOcb->txSubscription = new LogPortSubscription( subMsg.byteCount,
                                                                 subMsg.modeMask,
                                                                 subMsg.notifyEvent,
                                                                 ctp->rcvid);
            if (LOG_PORT_INVALID_SUB == subId)
                subId = clientOcb->txSubscription->id;

            // Add to transmit data subscribers list
            m_txSubscribers.AddSubscriber( clientOcb->txSubscription->id, clientOcb);
            reply.flags |= PORT_SUBSCRIBE_TX;
            Log( LOG_DEV_DATA, "Added Tx subscriber() for coId %d\n", ctp->rcvid);
        }

        // If subscribing for port lock events
        if( (subMsg.flags & PORT_SUBSCRIBE_LOCK) && (clientOcb->lockSubscription == NULL))
        {
            clientOcb->lockSubscription = new LogPortSubscription( subMsg.byteCount,
                                                                   subMsg.modeMask,
                                                                   subMsg.notifyEvent,
                                                                   ctp->rcvid);
            if (LOG_PORT_INVALID_SUB == subId)
                subId = clientOcb->lockSubscription->id;

            // Add to transmit data subscribers list
            m_lockSubscribers.AddSubscriber( clientOcb->lockSubscription->id, clientOcb);

            reply.flags |= PORT_SUBSCRIBE_LOCK;
            Log( LOG_DEV_DATA, "Added Lock subscriber()\n");

            // Check current port lock status
            if( IsPortLocked() == true)
            {
                // Port is currently locked, so deliver pulse now
                /////////////////////////////////////////////////

                // If client wants to use a pulse for notifications
                if(subMsg.notifyEvent.sigev_notify == SIGEV_PULSE)
                {
                    // Set pulse value to proper event
                    subMsg.notifyEvent.sigev_value.sival_int |= PORT_SUBSCRIBE_LOCK;
                }
                MsgDeliverEvent( ctp->rcvid, &subMsg.notifyEvent);
            }
        }

        // If subscribing for port unlock events
        if( (subMsg.flags & PORT_SUBSCRIBE_UNLOCK) && (clientOcb->unlockSubscription == NULL))
        {
            clientOcb->unlockSubscription = new LogPortSubscription( subMsg.byteCount,
                                                                     subMsg.modeMask,
                                                                     subMsg.notifyEvent,
                                                                     ctp->rcvid);
            if (LOG_PORT_INVALID_SUB == subId)
                subId = clientOcb->unlockSubscription->id;

            // Add to transmit data subscribers list
            m_unlockSubscribers.AddSubscriber( clientOcb->unlockSubscription->id, clientOcb);

            reply.flags |= PORT_SUBSCRIBE_UNLOCK;

            // Check current port lock status
            if( IsPortLocked() == false)
            {
                // Port is currently unlocked, so deliver pulse now
                /////////////////////////////////////////////////

                // If client wants to use a pulse for notifications
                if(subMsg.notifyEvent.sigev_notify == SIGEV_PULSE)
                {
                    // Set pulse value to proper event
                    subMsg.notifyEvent.sigev_value.sival_int |= PORT_SUBSCRIBE_UNLOCK;
                }
                MsgDeliverEvent( ctp->rcvid, &subMsg.notifyEvent);
            }
        }

        // If clients wants notification of exisating data in FIFOs
        if( subMsg.flags & PORT_NOTIFY_EXISTING)
        {
            if (reply.flags)
                reply.flags |= PORT_NOTIFY_EXISTING;
        }

        // If no other subscriptions were made, default to the RX subscription ID
        if (LOG_PORT_INVALID_SUB == subId)
            subId = clientOcb->rxSubscription->id;

        reply.handle = subId;
        memset( &msg->o, 0, sizeof( msg->o));
        msg->o.nbytes = sizeof( reply);
        SETIOV( ctp->iov, &msg->o, sizeof( msg->o)+msg->o.nbytes);
        retVal = _RESMGR_NPARTS(1);

        pthread_mutex_unlock( &m_dataLock);
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking data in PortSubscribeHandler: %s\n",
             strerror( retVal));
        retVal = EBUSY;
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::PortSubscribeHandler()\n");

    return( retVal);
}

/**
 * Handler method for client unsubscription requests
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int RawCommProxy::PortUnsubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                         resMgrIoOcb_t *ioOcb)
{
    logPortMsg_t            *portMsg = (logPortMsg_t*)_DEVCTL_DATA( msg->i);
    logPortUnsubscribe_t    &clientMsg = portMsg->portUnsubscribe;
    CommIoOcb_t             *clientOcb = (CommIoOcb_t*)ioOcb;
    int                     retVal = EINVAL;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::PortUnsubscribeHandler()\n");
    // Need to lock this operation to avoid duplicate subscription ID's
    if( (retVal = pthread_mutex_lock( &m_dataLock)) == EOK)
    {
        // Clear out mask bits for the events being unsubscribed for
        clientOcb->subMask &= ~(PORT_SUBSCRIBE_RX);

        if( clientMsg.flags & PORT_SUBSCRIBE_RX)
        {
            Log( LOG_FN_ENTRY, "Removing rx subscription %lld\n", clientMsg.handle);
            // Dont actually remove the client from the list. By unmasking
            //  the RX flag above, the client will not be sent notifications
            //  for RX data.
            //m_rxSubscribers.DelSubscriber( clientMsg.handle);
            clientOcb->rxSubscription->Invalidate();
            // Clear any filter strings for this client
            clientOcb->rxSubscription->filterList.clear();
        }

        if( clientMsg.flags & PORT_SUBSCRIBE_TX)
        {
            Log( LOG_FN_ENTRY, "Removing tx subscription %lld\n", clientMsg.handle);
            m_txSubscribers.DelSubscriber( clientMsg.handle);
            delete clientOcb->txSubscription;
            clientOcb->txSubscription = NULL;
        }

        if( clientMsg.flags & PORT_SUBSCRIBE_LOCK)
        {
            Log( LOG_FN_ENTRY, "Removing lock subscription %lld\n", clientMsg.handle);
            m_lockSubscribers.DelSubscriber( clientMsg.handle);
            delete clientOcb->lockSubscription;
            clientOcb->lockSubscription = NULL;
        }

        if( clientMsg.flags & PORT_SUBSCRIBE_UNLOCK)
        {
            Log( LOG_FN_ENTRY, "Removing unlock subscription %lld\n", clientMsg.handle);
            m_unlockSubscribers.DelSubscriber( clientMsg.handle);
            delete clientOcb->unlockSubscription;
            clientOcb->unlockSubscription = NULL;
        }

        memset( &msg->o, 0, sizeof( msg->o));
        msg->o.nbytes = 0;
        SETIOV( ctp->iov, &msg->o, sizeof( msg->o)+msg->o.nbytes);
        retVal = _RESMGR_NPARTS(1);

        pthread_mutex_unlock( &m_dataLock);
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking data in PortUnsubscribeHandler: %s\n",
             strerror( retVal));
        retVal = EBUSY;
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::PortUnsubscribeHandler()\n");

    return( retVal);
}

/**
 * Client request to reset his connection to the port driver.
 * Essentially only resets the client's FIFO.
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int RawCommProxy::ResetConnectionHandler( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    CommIoOcb_t *clientOcb = (CommIoOcb_t*)ioOcb;
    int             retVal = EOK;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ResetConnectionHandler()\n");

    if (clientOcb->rxSubscription != NULL)
        clientOcb->rxSubscription->fifo.Reset();
    if (clientOcb->txSubscription != NULL)
        clientOcb->txSubscription->fifo.Reset();

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ResetConnectionHandler()\n");

    return( _RESMGR_STATUS( ctp, retVal));
}

/**
 * Server commanded to reset (re-initialize)
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int RawCommProxy::ResetDriverHandler( resmgr_context_t *ctp, io_devctl_t *msg,
                                      resMgrIoOcb_t *ioOcb)
{
    int retVal = EOK;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ResetDriverHandler()\n");

    if( (m_comGuard.Acquire()) == EOK)
    {
        // Flush the output buffer on the serial port
        FlushDriverFifos( TCIOFLUSH);

        m_comGuard.Release();
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ResetDriverHandler()\n");

    return( _RESMGR_STATUS( ctp, retVal));
}

/**
 * Devctl handler used to set a temporary message reply timeout value
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int RawCommProxy::SetReplyTimeoutHandler( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    logPortMsg_t                *portMsg = (logPortMsg_t*)_DEVCTL_DATA(msg->i);
    logPortSetReplyTimeout_t    &toMssg = portMsg->replyTimeout;
    CommIoOcb_t                 *clientOcb = (CommIoOcb_t*)ioOcb;
    int                         retVal = EOK;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::SetReplyTimeoutHandler( %lld)\n", toMssg.nsTimeout);

    clientOcb->replyTimeout = toMssg.nsTimeout;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::SetReplyTimeoutHandler( %lld)\n", toMssg.nsTimeout);

    return( _RESMGR_STATUS( ctp, retVal));
}

/**
 * Devctl handler used to temporarily disable the transmitter on the physical serial port
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int RawCommProxy::BreakHandler( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    int         retVal = EOK;
    uint32_t    *msgData = (uint32_t*)_DEVCTL_DATA(msg->i);
    uint32_t    duration;

    // Get the desired number of milliseconds from the message
    duration = (uint64_t)(*msgData >> 16);

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::BreakHandler( %lld)\n", duration);

    if(*msgData & _CTL_BRK_CHG)
    {
        if( (m_comGuard.Acquire()) == EOK)
        {
            if( (tcsendbreak( m_comPortFd, duration)) == -1)
            {
                Log( LOG_ERRORS, "\ttcsendbreak failed: %s\n",
                     strerror( errno));
                retVal = errno;
            }

            m_comGuard.Release();
        }
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::BreakHandler( %lld)\n", duration);

    return( _RESMGR_STATUS( ctp, retVal));
}

/**
 * micorsec to nano-second conversion
 */
#define uSEC_nSEC( x)   ((x) * 1000LL)
#define ADJUST_TICK     1

/**
 * Devctl handler used to perform an ISO-K fast initialization
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 *
 * @return EOK if successful, other on error
 */
int RawCommProxy::FastInitHandler(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    int                     retVal = EOK;   // set the return to OK
    logPortMsg_t            *portMsg = (logPortMsg_t*)_DEVCTL_DATA(*msg);
    logPortEstablishComms_t &msgData = portMsg->establishComms;
    int                     oldPrio;
    char                    timeStamp[ 30];
    char                    dbgBuff[ 1024];
    char                    junk[ 128];

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::FastInitHandler( %lld, %lld)\n", msgData.nsLowTime, msgData.nsHiTime);

    dbgBuff[ 0] = '\n';
    dbgBuff[ 1] = 0;

    // Read the current port settings
    tcgetattr( m_comPortFd, &m_currentPortProtocol.m_tio);

    // Clear out the FIFOs in the devc driver
    FlushDriverFifos( TCIOFLUSH);

    // Suspend the reader thread so we can process reflections directly
    SuspendReaderThread();

    // Kick my priority up high
    oldPrio = SetThreadPriority( 63);

#if ADJUST_TICK
    // Lower the clock period and get the current clock period
    struct _clockperiod     oldPeriod, newPeriod;
    newPeriod.nsec = uSEC_nSEC( 100LL); // .1 ms (100us)
    newPeriod.fract = 0LL;
    ClockPeriod( CLOCK_REALTIME, &newPeriod, &oldPeriod, 0);
#endif

    sprintf(junk, "%s : Sending Break for %lld ms\n", GetTimeString( timeStamp), nSEC_mSEC( msgData.nsLowTime));
    strcat( dbgBuff, junk);

    // Send the initialization break signal
    if( (retVal=SendFastInitBreakSignal( msgData.nsLowTime, msgData.nsHiTime)) == EOK)
    {
        // If we need to send an initialization message
        if( (msgData.mssgLen > 0) && (retVal == EOK))
        {
            strcat( dbgBuff, GetTimeString( timeStamp));
            strcat( dbgBuff, " : Sending initialization string\n");

            // Send the init message
            retVal = SendFastInitMessage( msgData.initMssg, msgData.mssgLen);

            Log( LOG_DEV_DATA, dbgBuff);
        }
    }
    else
    {
        FlushDriverFifos( TCIFLUSH);
        Log( LOG_ERRORS, "\tSendFastInitBreakSignal() failed(%d): %s\n", retVal, strerror( retVal));
        errno = retVal;
    }

#if ADJUST_TICK
    // Restore the clock period
    ClockPeriod( CLOCK_REALTIME, &oldPeriod, &newPeriod, 0);
#endif

    // Restore original priority
    (void)SetThreadPriority( oldPrio);

    // Resume the reader thread to properly process reflected characters/data
    ResumeReaderThread();

    msg->o.ret_val = retVal;

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::FastInitHandler( %lld, %lld), status=%d: %s\n",
         msgData.nsLowTime, msgData.nsHiTime, retVal, strerror( retVal));

    return( _RESMGR_STATUS( ctp, retVal));
}

/**
 * Sends an ISOK fast initializtion break pattern( N ms low, M ms high)
 *
 * @param nsLowTime Number of nano seconds to assert the break signal for
 * @param nsHiTime  Number of nanoseconds to wait after the break signal has been asserted
 *
 * @return EOKJ if successful, error code if an error occurs
 */
int RawCommProxy::SendFastInitBreakSignal( const uint64_t &nsLowTime, const uint64_t &nsHiTime)
{
    int                     retVal = EOK;   // set the return to OK
    struct _itimer          itime;
    struct sigevent         breakEvent;
    timer_t                 breakTimer;
    int                     chId, coId;
    io_pulse_t              breakPulse;
    bool                    timesUp = false;
    uint32_t                msLoTime = nSEC_mSEC( nsLowTime);

    // Create a channel/connection to receive the timer pulse on
    chId = ChannelCreate( 0);
    coId = ConnectAttach( ND_LOCAL_NODE, 0, chId, _NTO_SIDE_CHANNEL, 0);

    // Initialize the event to be delivered for our 'break' timer
    SIGEV_PULSE_INIT( &breakEvent, coId, 63,  NEXT_READ_CODE, NEXT_READ_CODE);

    // Create the 'break' timer
    breakTimer = TimerCreate(CLOCK_REALTIME, &breakEvent);

    // Start the timer to expire after the total time (hi+low) has elapsed
    memset(&itime, 0, sizeof(itime));
    itime.interval_nsec = 0;
    itime.nsec = nsLowTime + nsHiTime;
    TimerSettime( breakTimer, 0, &itime, NULL);

    // if successfully sent break to the comm port
    if( tcsendbreak( m_comPortFd, msLoTime) != -1)
    {
        // Loop until our timer expires
        while( timesUp == false)
        {
            // Wait for the timer pulse to arrive
            switch( MsgReceivePulse( chId, (void*)&breakPulse, sizeof( breakPulse), NULL))
            {
            case 0:
                // If this is our timer pulse
                if( breakPulse.code == NEXT_READ_CODE)
                {
                    // Done waiting
                    timesUp = true;
                }
                break;
            case -1:
                Log( LOG_ERRORS, "\tError waiting for break timer: %s\n", strerror( errno));
                retVal = errno;
                // Done waiting
                timesUp = true;
                break;
            default:
                break;
            }
        }
    }
    else
    {
        Log( LOG_ERRORS, "\ttcsendbreak failed(%d): %s\n", errno, strerror( errno));
        retVal = errno;
        FlushDriverFifos( TCIFLUSH);
    }

    return( retVal);
}

/**
 * Sends a serial message to a vehicle module for an ISO-k fast initialization sequence.
 * This differs from a normal tranmit in that this method processes any reflected data
 * directly instead of using the reader thread/thread pool to process reflections
 * asynchronously
 *
 * @param initMssg The serial data to be sent
 * @param mssgLen  The number of bytes to be sent
 *
 * @return EOK if successful, error code if an error occurs
 */
int RawCommProxy::SendFastInitMessage( const uint8_t *initMssg, uint32_t mssgLen)
{
    int             retVal = EOK;   // set the return to OK
    uint32_t        reflCount;
    const bool      reflection = m_currentPortProtocol.GetReflection();
    const uint64_t  byteTime = m_currentPortProtocol.GetByteTime();
    const uint64_t  byteGap = mSEC_nSEC( m_currentPortProtocol.GetByteGap());
    uint8_t         reflChar;

    // If we have a byte gap
    if( byteGap > 0)
    {
        // Send the message byte-by-byte
        for( reflCount=0; reflCount<mssgLen; reflCount++)
        {
            // Send the next byte
            if( Write( &initMssg[ reflCount], 1) == 1)
            {
                // Log communication for this serial send
                UpdateBusCommLog( ComDirTx, &initMssg[reflCount], 1, NULL);

                // Wait to send the next byte
                if (byteGap > 0)
                    nanospin_ns(byteGap);

                if( reflection)
                {
                    if( ReadDriverData( &reflChar, 1, 5LL*byteTime) == 1)
                    {
                        // Log communication for this reflected char
                        UpdateBusCommLog( ComDirRx, &reflChar, 1, NULL);
                    }
                    else
                    {
                        Log( LOG_ERRORS, "\tError waiting for reflection byte %d: %s\n", reflCount, strerror( errno));
                        retVal = errno;
                        break;
                    }
                }
            }
            else
            {
                Log( LOG_ERRORS, "\tError transmitting initialization string: %s\n",
                     strerror( errno));
                retVal = errno;
                break;
            }
        }
        // Eat the reflected NULL character
        if( reflection)
        {
            // If the drive is going to pass back break signals
            if( !(m_currentPortProtocol.m_tio.c_iflag & IGNBRK))
            {
                // Read the reflected break character
            if( ReadDriverData( &reflChar, 1, 5LL*byteTime) == 1)
            {
                // Log communication for this reflected char
                UpdateBusCommLog( ComDirRx, &reflChar, 1, NULL);
            }
            else
            {
                Log( LOG_ERRORS, "\tError waiting for reflected NULL byte: %s\n", strerror( errno));
                retVal = errno;
            }
        }
            else
            {
                Log( LOG_DETAILED_DATA, "Not expecting NULL character reflection\n");
            }
        }
    }
    else
    {
        // Write message as a packet
        if( (reflCount=Write( initMssg, mssgLen)) == mssgLen)
        {
            // Log communication for this serial send
            UpdateBusCommLog( ComDirTx, initMssg, mssgLen, NULL);

            // If talking on a reflective bus
            if( reflection)
            {
//              const uint32_t  buffSz=reflCount+1;
                const uint32_t  buffSz=reflCount;
                uint8_t         reflBuff[ buffSz];
                int             signedReflCount;

                // Eat the reflected bytes
//              if( (reflCount=ReadDriverData( reflBuff, buffSz, 5LL*byteTime*buffSz)) > 0)
                // Read Driver Data returns the number of bytes received, or -1 if there is an
                // error or timeout. The check for greater than 0 in the if statement is checking for
                // not an error etc. Since reflCount is an unsigned 32 bit number, the 0xFFFFFFFF returned
                // is evaluated after the assignment and is interpreted as a very large positive number.
                if( (signedReflCount=ReadDriverData( reflBuff, buffSz, 5LL*byteTime*buffSz)) > 0)
                {
                    reflCount=signedReflCount; // value is > 0
                    PrintSerialString( LOG_DEV_DATA, "Reflection received", reflBuff, reflCount);
                    // Log communication for this reflected char
                    UpdateBusCommLog( ComDirRx, reflBuff, reflCount, NULL);
                }
                else
                {
                    Log( LOG_ERRORS, "\tError waiting for reflection bytes: %s\n", strerror(errno));
                    retVal = errno;
                }
            }
            // Log communication for this serial send
            UpdateBusCommLog( ComDirTx, initMssg, mssgLen, NULL);
        }
        else
        {
            Log( LOG_ERRORS, "\tError transmitting initialization string: %s\n",
                 strerror( errno));
            retVal = errno;
        }
    }

    if (retVal == EOK)
        PrintSerialString(LOG_DEV_DATA, "Initialization string sent", initMssg, mssgLen);

    return( retVal);
}

/**
 * Perform a low speed (i.e. 5 baud) initialization
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 *
 * @return EOK if successful, other on error
 */
int RawCommProxy::LowSpeedInit(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    int                 retVal = EOK;   // set the return to OK
    CommIoOcb_t         *clientOcb = (CommIoOcb_t*)ioOcb;
    CommInitMessage_t   *initMssg = (CommInitMessage_t*)_DEVCTL_DATA(*msg);
    int                 ii;
    SerialProtocol_t    currProtocol;
    uint64_t            lastComm, currTime;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::LowSpeedInit()\n");

    // Suspend the reader thread so we can process reflections directly
//    SuspendReaderThread();

    // Read current port protocol
    tcgetattr( m_comPortFd, &currProtocol.m_tio);

    ClockTime( CLOCK_REALTIME, NULL, &lastComm);

    // Assume all OK
    retVal = _RESMGR_STATUS( ctp, retVal);

    // Check if first byte is at a different baud
    if (initMssg->mssg[ 0].baud > 0 || initMssg->mssg[ 0].updatePortSettings)
    {
        m_currentPortProtocol.SetBaud(initMssg->mssg[ 0].baud);
        if(initMssg->mssg[ 0].updatePortSettings)
        {
            m_currentPortProtocol.SetParity(initMssg->mssg[ 0].parity);
            m_currentPortProtocol.SetDataBits(initMssg->mssg[ 0].dataBits);
            m_currentPortProtocol.SetStopBits(initMssg->mssg[ 0].stopBits);
        }
        ConfigurePort( &m_currentPortProtocol.m_tio, TCSANOW);

        // If low baud rate, give the Mizar time to adapt
        if (initMssg->mssg[0].baud < 100)
            delay(500);
    }
    Log( "\tPort Speed: %d\n", m_currentPortProtocol.GetBaud());
    Log( "\tByteCount: %d\n", initMssg->byteCount);

    // send out the message
    for( ii=0; ii<initMssg->byteCount; ii++)
    {
        CommInitByte    &initByte = initMssg->mssg[ ii];

        Log( "\tLowSpeedInit byte %d\n", ii);
        Log( "\tLowSpeedInit byte literalVal %02x\n", initByte.literalVal);

        ClockTime( CLOCK_REALTIME, NULL, &currTime);

        // Check if this byte's value is based on a previous byte's value
        if( (initByte.refByte >= 0) && (initByte.refByte < initMssg->byteCount))
        {
            initByte.literalVal = initMssg->mssg[ initByte.refByte].literalVal ^ initByte.byteMask;
            Log( "\tLowSpeedInit byte %d referencing byte %d, mask = $%02hhX\n", ii,
                 initByte.refByte, initByte.byteMask);
        }

        // If this is a transmit byte
        if( initByte.byteType == 0)
        {
            Log( "\tLowSpeedInit byte %d is a TX byte\n", ii);
            // See if we have a minimum time required before sending the next byte
            if( initByte.timeOut > 0)
            {
                uint64_t    timeOut = mSEC_nSEC( initByte.timeOut);
                timeOut = (timeOut - (currTime - lastComm));

                Log( "\tLowSpeedInit byte %d TX timeout = %d ms\n", ii,
                     initByte.timeOut);

                nanospin_ns( timeOut);
            }

            UpdateBusCommLog( ComDirTx, &initByte.literalVal, 1, NULL);

            // send the next byte
            TransmitMessage( &initByte.literalVal, 1, clientOcb);
        }
        else
        {
            Log( "\tLowSpeedInit byte %d is a RX byte, timeout = %d ms\n", ii, initByte.timeOut);
            if( ReadDriverData( &initByte.literalVal, 1,  (uint32_t)initByte.timeOut) == -1)
            {
                Log( LOG_ERRORS, "\tError receiving byte %d of 5 baud init: %s\n", ii, strerror( errno));
                retVal = _RESMGR_STATUS( ctp, errno);
                break;
            }
            UpdateBusCommLog( ComDirRx, &initByte.literalVal, 1, NULL);
//            AddToClientFifo( (const char *)&initByte.literalVal, 1, clientOcb);
        }

        // time of our last communication
        ClockTime( CLOCK_REALTIME, NULL, &lastComm);

        // Check if next byte is at a different baud
        if( (ii < (initMssg->byteCount-1)) && ((initMssg->mssg[ ii+1].baud > 0) || initMssg->mssg[ ii+1].updatePortSettings))
        {
            Log( "\tLowSpeedInit byte %d needs baud rate = %d updateportsettings = %d\n", ii+1, initMssg->mssg[ ii+1].baud,
                 initMssg->mssg[ ii+1].updatePortSettings);

            m_currentPortProtocol.SetBaud(initMssg->mssg[ ii+1].baud);
            if(initMssg->mssg[ ii+1].updatePortSettings)
            {
                m_currentPortProtocol.SetParity(initMssg->mssg[ ii+1].parity);
                m_currentPortProtocol.SetDataBits(initMssg->mssg[ ii+1].dataBits);
                m_currentPortProtocol.SetStopBits(initMssg->mssg[ ii+1].stopBits);
            }
            ConfigurePort( &m_currentPortProtocol.m_tio, TCSANOW);
        }
    }
    // Read current port protocol
    ConfigurePort( &currProtocol.m_tio, TCSANOW);

    // Resume the reader thread to properly process reflected characters/data
//    ResumeReaderThread();

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::LowSpeedInit()\n");

    return( retVal);
}

/**
 * Method called before performing a LOG_PORT_RECONNECT. This method
 * should check that t he calling client owns the active port lock and
 * should cancel any driver notifications (ionotify)
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 *
 * @return EOK if successful, other on error
 */
int RawCommProxy::ReconnectHandlerVerify(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    int             retVal = EPERM;
    int             lockErr;
    CommIoOcb_t         *clientOcb = (CommIoOcb_t*)ioOcb;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReconnectHandlerVerify()\n");

    // Lock the condvar while checking its value
    lockErr = m_portLock.Acquire();
    // If we locked the port (or already have the port locked)
    if( (EOK == lockErr) || (EDEADLK == lockErr))
    {
        // If port is currently locked
        if( true == IsPortLocked())
        {
            // Ability to reconnect depends on whether or not this client owns
            //  the current port lock
            if( true == ClientOwnsPortLock( clientOcb))
            {
                // Suspend the reader thread while we reconnect
                SuspendReaderThread();

                Log( LOG_DEV_DATA, "OK to reconnect...Client owns locked port\n");
                retVal = EOK;
            }
        }
        else
        {
            Log( LOG_DEV_DATA|LOG_ERRORS, "NOT OK to reconnect...port is NOT locked\n");
        }

        if (EOK == lockErr)
            m_portLock.Release();
    }
    else
    {
        retVal = lockErr;
        Log( LOG_ERRORS, "\tError checking port lock mutex: %s\n",
             strerror( retVal));
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReconnectHandlerVerify()\n");

    return( retVal);
}

/**
 * Method used to reconnect to the underlying communication driver (devc,
 * socket, etc) in response to a client LOG_PORT_RECONNECT devctl message.
 * The clinet issuing the reconnect request MUST have the port locked prior
 * to performing this request
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 *
 * @return EOK if successful, other on error
 */
int RawCommProxy::ReconnectHandler(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    int                 retVal = EOK;   // set the return to OK

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReconnectHandler()\n");

    // Close our current driver connection
    if( 0 == ClosePort())
    {
        // Re-establish our connection to the driver
        if( (m_comPortFd=OpenPort( m_comPortName.c_str())) != -1)
        {
            Log( LOG_DEV_DATA, "Reconnected to driver, fd=%d\n", m_comPortFd);
            retVal = EOK;

        }
        else
        {
            Log( LOG_ERRORS, "Error opening port: %s\n", strerror( errno));
            retVal = errno;
        }
    }
    else
    {
        Log( LOG_ERRORS, "Error closing port: %s\n", strerror( errno));
        retVal = errno;
    }

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReconnectHandler()\n");

    return( retVal);
}

/**
 * Method called after performing a LOG_PORT_RECONNECT. This method
 * resumes the reader thread and flushed our driverside FIFOs
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 *
 * @return EOK if successful, other on error
 */
int RawCommProxy::ReconnectHandlerFinalize(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    int retVal = EOK;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReconnectHandlerFinalize()\n");

    // Flush our driverside FIFOs
    FlushDriverFifos( TCIOFLUSH);

    // Resume the reader thread after we reconnect
    ResumeReaderThread();

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReconnectHandlerFinalize()\n");

    return( retVal);
}

/**
 * Method used to check if we should notify a client that subscription
 * data is available
 *
 * @param clientOcb    Client's connection ID containing his subscription attributes
 * @param eventMask The type of notification event were checking (TRansmit, Receive, etc)
 * @return Mask indicating which notifications events can be delivered
 */
int RawCommProxy::ShouldNotifyClient( const CommIoOcb_t *clientOcb, int eventMask)
{
    int retVal = 0;

    // IF the OCB is valid
    if( clientOcb != NULL)
    {
        // If we're checking for received data notification criteria
        if( (eventMask & PORT_SUBSCRIBE_RX) &&
            (CanNotifySubscriber( clientOcb->rxSubscription) == true))
        {
            retVal |= PORT_SUBSCRIBE_RX;
        }
        // If we're checking for transmitted data notification criteria
        if( (eventMask & PORT_SUBSCRIBE_TX) &&
            (CanNotifySubscriber( clientOcb->txSubscription) == true))
        {
            retVal |= PORT_SUBSCRIBE_TX;
        }
        // If we're checking for transmitted data notification criteria
        if( (eventMask & PORT_SUBSCRIBE_LOCK) &&
            (CanNotifySubscriber( clientOcb->lockSubscription) == true))
        {
            retVal |= PORT_SUBSCRIBE_LOCK;
        }
        // If we're checking for transmitted data notification criteria
        if( (eventMask & PORT_SUBSCRIBE_UNLOCK) &&
            (CanNotifySubscriber( clientOcb->unlockSubscription) == true))
        {
            retVal |= PORT_SUBSCRIBE_UNLOCK;
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tclientOcb == NULL, so no notification\n");
        retVal = 0;
    }
    return( retVal);
}

/**
 * Checks if the subscriber described by the sub argument can receive
 * subscription notifications
 *
 * @param sub    subscriber whose ability to receive subscription notifications
 *               should be checked
 * @return true if the subscriber can be notified, false otherwise
 */
bool RawCommProxy::CanNotifySubscriber( LogPortSubscription *sub)
{
    bool retVal = false;

    // If subscription is valid
    if( sub != NULL)
    {
        // If subscription event is valid
        if( sub->ShouldNotify())
        {
            // If desired number of bytes received to notify client
            if( sub->fifo.GetSize() >= sub->count)
            {
                // Say we should notify the client
                retVal = true;
            }
            else
            {
                Log( LOG_DEV_DATA, "\tNot enough bytes in FIFO to notify client FIFO size: %d subscription size: %d\n",
                     sub->fifo.GetSize(), sub->count);
                retVal = false;
            }
        }
    }

    return( retVal);
}

/**
 * Adds the given message to the transmit queue and signals an
 * event to start the message transmission
 *
 * @param message Message to be sent to the vehicle
 * @param clientOcb Client we are transmitting data for. May be NULL if the write occurs
 *                  for reasons other than a client's Write message
 * 
 * @return Number of bytes written or -1 on error
 */
int RawCommProxy::TransmitMessage( const SerialString_t &message, CommIoOcb_t *clientOcb)
{
    return( TransmitMessage( message.c_str(), message.length(), clientOcb));
}

/**
 * Adds the given message to the transmit queue and signals an
 * event to start the message transmission
 *
 * @param message Message to be sent to the vehicle
 * @param msgLen  Length of the message to be sent to the vehicle
 * @param clientOcb Client we are transmitting data for. May be NULL if the write occurs
 *                  for reasons other than a client's Write message
 * 
 * @return Number of bytes written or -1 on error
 */
int RawCommProxy::TransmitMessage( const uint8_t *message, int msgLen, CommIoOcb_t *clientOcb)
{
    int             retVal = 0;
    int             err, ii, reflCount=0;
    uint64_t        byteGap = mSEC_nSEC( m_currentPortProtocol.GetByteGap());
    bool            reflection = m_currentPortProtocol.GetReflection();
    uint64_t        byteTime = m_currentPortProtocol.GetByteTime();

    PrintSerialString( LOG_SER_COM, "Transmitting", message, msgLen);

    // Set our reflection count based on the message size
    if( reflection)
    {
        m_reflectionCount.Acquire();
        reflCount = m_reflectionCount.GetValue();
        m_reflectionCount.Signal( reflCount+msgLen);
        m_reflectionCount.Release();
    }

    // If no inter-byte gap
    if( byteGap == 0)
    {
        // We want to ensure that the reader thread doesn't read
        //  while we're writing
        if( (err=m_comGuard.Acquire()) == EOK)
        {
            // Write the entire message
            retVal = Write( message, msgLen);
            m_comGuard.Release();
        }
        else
        {
            Log( LOG_ERRORS, "\tError locking port mutex in TransmitMessage: %s\n",
                 strerror( err));
            retVal = -1;
        }
    }
    else
    {
        // Send the message byte by byte
        for( ii=0; ii<msgLen; ii++)
        {
            // Lock/Unlock for each byte so we can safely receive responses
            //  between transmits
            if( (err=m_comGuard.Acquire()) == EOK)
            {
                // Make sure this byte gets sent properly
                Log( LOG_SER_COM, "\tTransmitting 1 byte $%02hhX\n", message[ ii]);

                retVal = Write( &message[ ii], 1);

                m_comGuard.Release();
            }
            else
            {
                Log( LOG_ERRORS, "\tError locking port mutex in TransmitMessage: %s\n",
                     strerror( err));
                break;
            }

            // If an error occurred while writing this byte
            if( retVal == -1)
            {
                Log( LOG_ERRORS, "\tError writing byte %d of message: %s\n",
                     ii, strerror( errno));
                break;
            }

            // Wait until we can send the next byte
            if (byteGap > 0)
                nanospin_ns(byteGap);
        }
        // return number of bytes sent
        retVal = ii;
    }

    // Wait for all reflected data to arrive
    if( reflection)
    {
        Log(LOG_DEV_DATA,"Waiting for %d reflection bytes",reflCount);

        m_reflectionCount.Acquire();
        err = m_reflectionCount.Wait( reflCount, 10LL*byteTime*msgLen);
        m_reflectionCount.Release();

        if( err != EOK)
        {
            Log( LOG_ERRORS, "\tError waiting for all reflected data: %s\n", strerror( err));
        }
    }

    return( retVal);
}

/**
 * Writes data to the driver
 * 
 * @param buff   The data buffer to write
 * @param bytes  The number of bytes to write from buff
 * 
 * @return The number of bytes written, or -1 if an error occurred (errno is set).
 */
int RawCommProxy::Write( const void* buff, size_t bytes)
{
    Log(LOG_ERRORS, "\tComm port FD: %d\n", m_comPortFd);
    return( write( m_comPortFd, buff, bytes));
}

/**
 * Changes the port protocol settings
 *
 * @param newProtocol
 *               New protocol to be applied
 * @param flags  ProtocolFlags_t flag indicating which of the port protocol
 *               settings are valid (and should be applied).
 * @see ProtocolFlags_t
 */
void RawCommProxy::SetPortProtocol( SerialProtocol newProtocol, uint32_t flags)
{
    // Fill in default protocol settings
    InitializeProtocol( newProtocol, flags);

    // Apply new port protocol
    ConfigurePort( newProtocol);

    // Set m_currentProtocol to newly applied protocol
    m_currentPortProtocol = newProtocol;
}

/**
 * Initializes the invalid members of the given SerialProtocol object
 * to the default values. 'Invalid members' are the protocol settings
 * for which the corresponding bit in the flags argument are not set.
 *
 * @param portProtocol
 *               Protocol object to initialize
 * @param flags  Bitmask indicating which members of the SerialProtocol object are
 *               already valid  (i.e. do not need to be initialized)
 */
void RawCommProxy::InitializeProtocol( SerialProtocol &portProtocol, uint32_t flags)
{
    // If all protocol settings are not valid
    if( flags != PORT_SET_ALL)
    {
        // Check if new baud not valid
        if( !(flags & PORT_SET_BAUD))
        {
            portProtocol.SetBaud( m_defaultPortProtocol.m_tio.c_ispeed);
        }

        // Check if new parity not valid
        if( !(flags & PORT_SET_PARITY))
        {
            switch( m_defaultPortProtocol.m_tio.c_cflag & (PARENB|PARODD))
            {
            case PARENB:
                portProtocol.SetParity(EVEN_PARITY);   break;
            case PARENB | PARODD:
                portProtocol.SetParity(ODD_PARITY);    break;
            default:
                portProtocol.SetParity(NO_PARITY);     break;
            }
        }

        // Check if new data bits not valid
        if( !(flags & PORT_SET_DATA_BITS))
        {
            switch(m_defaultPortProtocol.m_tio.c_cflag & CSIZE)
            {
            case CS5:
                portProtocol.SetDataBits(BITS5);   break;
            case CS6:
                portProtocol.SetDataBits(BITS6);   break;
            case CS7:
                portProtocol.SetDataBits(BITS7);   break;
            default:
            case CS8:
                portProtocol.SetDataBits(BITS8);   break;
            }
        }

        // Check if new stop bits not valid
        if( !(flags & PORT_SET_STOP_BITS))
        {
            switch( m_defaultPortProtocol.m_tio.c_cflag & CSTOPB)
            {
            case CSTOPB:
                portProtocol.SetStopBits(STOP2);   break;
            default:
                portProtocol.SetStopBits(STOP1);   break;
            }
        }

        // Check if new inter-byte gap not valid
        if (!(flags & PORT_SET_BYTE_GAP))
            portProtocol.SetByteGap(m_defaultPortProtocol.m_byteGap);

        // Check if new inter-message gap not valid
        if (!(flags & PORT_SET_MSSG_GAP))
            portProtocol.SetMsgGap(m_defaultPortProtocol.m_msgGap);
    }
}

/**
 * Initializes the invalid members of the given SerialProtocol object
 * to the default values. 'Invalid members' are the protocol settings
 * for which the corresponding bit in the flags argument are not set.
 *
 * @param portProtocol
 *               Protocol object to initialize
 * @param flags  Bitmask indicating which members of the SerialProtocol object are
 *               already valid  (i.e. do not need to be initialized)
 */
void RawCommProxy::InitializeProtocol( SerialProtocol_t &portProtocol, uint32_t flags)
{
    // If all protocol settings are not valid
    if( flags != PORT_SET_ALL)
    {
        // Check if new baud not valid
        if( !(flags & PORT_SET_BAUD))
        {
            portProtocol.m_tio.c_ospeed = m_defaultPortProtocol.m_tio.c_ospeed;
            portProtocol.m_tio.c_ispeed = m_defaultPortProtocol.m_tio.c_ispeed;
        }

        // Check if new parity not valid
        if( !(flags & PORT_SET_PARITY))
        {
            portProtocol.m_tio.c_cflag &= ~(PARENB | PARODD);
            portProtocol.m_tio.c_cflag |= (m_defaultPortProtocol.m_tio.c_cflag & (PARENB | PARODD));
        }

        // Check if new data bits not valid
        if( !(flags & PORT_SET_DATA_BITS))
        {
            portProtocol.m_tio.c_cflag &= ~(CSIZE);
            portProtocol.m_tio.c_cflag |= (m_defaultPortProtocol.m_tio.c_cflag & CSIZE);
        }

        // Check if new stop bits not valid
        if( !(flags & PORT_SET_STOP_BITS))
        {
            portProtocol.m_tio.c_cflag &= ~(CSTOPB);
            portProtocol.m_tio.c_cflag |= (m_defaultPortProtocol.m_tio.c_cflag & CSTOPB);
        }

        // Check if new inter-byte gap not valid
        if( !(flags & PORT_SET_BYTE_GAP))
        {
            portProtocol.m_byteGap = m_defaultPortProtocol.m_byteGap;
        }

        // Check if new inter-message gap not valid
        if( !(flags & PORT_SET_MSSG_GAP))
        {
            portProtocol.m_msgGap = m_defaultPortProtocol.m_msgGap;
        }
    }
}

/**
 * Changes the port protocol settings
 *
 * @param newProtocol
 *               New protocol to be applied
 * @param flags  ProtocolFlags_t flag indicating which of the port protocol
 *               settings are valid (and should be applied).
 * @see ProtocolFlags_t
 */
void RawCommProxy::SetPortProtocol( const SerialProtocol_t &newProtocol, uint32_t flags)
{
    const SerialProtocol xmtProtocol( newProtocol);
    SetPortProtocol( xmtProtocol, flags);
}

/**
 * Helper function to read a client's IoWrite buffer
 *
 * @param ctp        Resource Manager context block
 * @param rawMessage Data from client's send buffer
 * @param writeMsg   I/O write message buffer we are reding data for
 *
 * @return Number of bytes read if successful
 *         -1 if error occurs
 */
int RawCommProxy::ReadSendersBuffer( resmgr_context_t *ctp, SerialString_t &rawMessage, const io_write_t *writeMsg)
{
    const size_t    buffSz = writeMsg->i.nbytes;
    uint8_t         rawData[ buffSz+1];
    int             retVal;
    off_t           offset = sizeof(writeMsg->i);

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ReadSendersBuffer(%d)\n", buffSz);

    // Read senders write data
    if( (retVal = resmgr_msgread( ctp, rawData, buffSz, offset)) != -1)
    {
        rawMessage = SerialString_t( rawData, retVal);
        Log(LOG_DEV_DATA, "rawMessage(%d)", rawMessage.size());
    }
    // If an error occurred
    else
    {
        // Return error status
        Log( LOG_ERRORS, "\tError reading senders write buffer (%d of %d bytes): %s\n", retVal, buffSz, strerror( errno));
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ReadSendersBuffer( %d)\n", retVal);

    return( retVal);

}

/**
 * Creates a new and unique subscription ID
 *
 * @return Unique subscription ID
 */
LogPortSubscriptionId_t RawCommProxy::CreateUniqueSubscriptionId()
{
    LogPortSubscriptionId_t     subId = LOG_PORT_INVALID_SUB;
    struct timespec             curTime;

    // Need to lock this operation to avoid duplicate subscription ID's
    if( pthread_mutex_lock( &m_subIdLock) == EOK)
    {
        // Use current time (with nanosec's) as subscription ID
        if( clock_gettime( CLOCK_REALTIME, &curTime) != -1 )
        {
            // Convert time to a 64-bit "subscription handle"
            subId = timespec2nsec( &curTime);
        }
        // Wait here for a little bit so no sub ID's are the same
        delay( 1);
        pthread_mutex_unlock( &m_subIdLock);
    }
    return( subId);
}

/**
 * Registers the caller for notifications from the serial driver when data is
 * available to be read from the serial port
 *
 * @param dataEvent sigevent structure used to deliver the notification event
 * @return 0 if successful, -1 if an error occurred
 */
int RawCommProxy::DriverDataRegister( const struct sigevent *dataEvent)
{
    int status;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::DriverDataRegister()\n");
    if( (status=m_comGuard.Acquire()) == EOK)
    {
        // Cancel notification so we dnot have multiple notification requests
        //  active simultaneoulsy
        CancelDriverNotification();

        // Re-register for notifications of received data
        status = ionotify( m_comPortFd, _NOTIFY_ACTION_POLLARM, _NOTIFY_COND_INPUT, dataEvent);
        if( status == -1)
        {
            Log( LOG_ERRORS, "\tDriverDataRegister() ionotify failed: %s\n", strerror( errno));
        }
        // If data is currently available
        else if( status & _NOTIFY_COND_INPUT)
        {
            int bytesWaiting;
            if( (bytesWaiting=DriverBytesAvailable()) > 0)
            {
                Log( LOG_DEV_DATA, "DriverDataRegister() %d bytes available, forwarding notification pulse\n", bytesWaiting);
                // Send a pulse to our channel
                MsgSendPulse( dataEvent->sigev_coid, dataEvent->sigev_priority,
                              dataEvent->sigev_code, dataEvent->sigev_value.sival_int+1);
            }
            else
            {
                Log( LOG_ERRORS, "\tSerial driver says data is ready, but tcischars() <= 0\n");
            }
            status = 0;
        }

        m_comGuard.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\Error locking com port mutex: %s\n", strerror( status));
        status = -1;
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::DriverDataRegister()\n");

    return( status);
}

/**
 * Unregisters the caller for notifications from the serial driver when data is
 * available to be read from the serial port
 */
void RawCommProxy::CancelDriverNotification()
{
    int status;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::CancelDriverNotification()\n");
    if( (status=m_comGuard.Acquire()) == EOK)
    {
        // Pass a NULL sigevent struct to cancel our subscription
        status = ionotify( m_comPortFd, _NOTIFY_ACTION_POLL, _NOTIFY_COND_INPUT, NULL);
        // If error
        if( status == -1)
        {
            Log( LOG_ERRORS, "\tCancelDriverNotification() ionotify failed: %s\n", strerror( errno));
        }

        m_comGuard.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\Error locking com port mutex: %s\n", strerror( status));
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::CancelDriverNotification()\n");
}

/**
 * Set a fault to indicate a communication error
 *
 * @param faultReason    Description of the comm fault
 */
void RawCommProxy::SetCommFault( const std::string &faultReason)
{
    std::string fault;
    std::string response;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::SetCommFault(%s)\n", faultReason.c_str());

    m_activeFaultList.Lock();
    try
    {
        fault = m_pathName + std::string( "CommFault");

        // If this fault is not already active
        if( true == m_activeFaultList.AddFault( fault))
        {
            m_faultServer.SetFault(fault, faultReason, m_pathName, response,
                                    true);
        }
    }
    catch( BepException &err)
    {
        Log( LOG_ERRORS, "\tError setting comm fault '%s'; reason: %s\n",
            faultReason.c_str(), err.GetReason());
    }
    catch( ...)
    {
        Log( LOG_ERRORS, "\tUnknown error setting comm fault '%s'\n",
            faultReason.c_str());
    }
    m_activeFaultList.Unlock();

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::SetCommFault(%s)\n", faultReason.c_str());
}

/**
 * Clears a fault which indicated a communication error
 */
void RawCommProxy::ClearCommFault()
{
    std::string fault;
    std::string response;

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ClearCommFault()\n");

    m_activeFaultList.Lock();
    try
    {
        fault = m_pathName + std::string( "CommFault");

        // If this fault is active
        if( true == m_activeFaultList.RemoveFault( fault))
        {
            m_faultServer.ClearFault( fault, response, true);
        }
    }
    catch( BepException &err)
    {
        Log( LOG_ERRORS, "\tError clearing comm fault; reason: %s\n",
            err.GetReason());
    }
    m_activeFaultList.Unlock();

    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ClearCommFault()\n");
}

/**
 * Outputs a serial string via the logger
 *
 * @param logMask   Verbose mask required for string to be printed
 * @param prefix    String to pre-pend to output (i.e. "Response" would
 *                  result in "Response: $12...")
 * @param srlString Serial string to be printed
 */
void RawCommProxy::PrintSerialString( uint32_t logMask, const char *prefix,
                        const SerialString_t &srlString)
{
    PrintSerialString( logMask, prefix, srlString.c_str(), srlString.length());
}

/**
 * Outputs a serial string via the logger
 *
 * @param logMask   Verbose mask required for string to be printed
 * @param prefix    String to pre-pend to output (i.e. "Response" would
 *                  result in "Response: $12...")
 * @param srlString Serial string to be printed
 * @param len       Number of bytse in the serial string
 */
void RawCommProxy::PrintSerialString( uint32_t logMask, const char *prefix,
                        const uint8_t *srlString, uint32_t len)
{
    int ii;
    const int buffLen = len;
    char prnBuff[ buffLen * 5 + 1], hexBuff[ 5];

    // Make sure required verbosity enabled
    if( GetVerboseMask() & logMask)
    {
        // Avoid SIGSEGV
        if( prefix == NULL) prefix = " ";
        int lineNumber = 0;

        // Build logger string
        for( ii=0, prnBuff[ 0] = 0; ii<buffLen; ii++)
        {
            sprintf( hexBuff, "$%02hhX ", srlString[ ii]);
            strcat( prnBuff, hexBuff);
            if ((ii + 1) % 128 == 0)
            {//New ling
                Log(logMask, "%s[%d]: %s\n", prefix, lineNumber, prnBuff);
                lineNumber++;
                prnBuff[0] = 0; 
            }
        }
        //Log( logMask, "%s: %s\n", prefix, prnBuff);
        Log(logMask, "%s[%d]: %s\n", prefix, lineNumber, prnBuff); 
    }
}

/**
 * Retrieves the name under which the process should register
 *
 * @param procName Returned name to register as
 */
void RawCommProxy::GetRegisteredName( string &procName)
{
    procName = "logport/" + m_pathName;
}

/**
 * Flush the buffers specifred by the (optional, def=TCIOFLUSH) action argument:
 * TCIFLUSH: flush output buffer
 * TCOFLUSH: Flush input buffer
 * TCIOFLUSH: Flush input and output buffer
 *
 * @param action Which buffer to flush:
 *               TCIFLUSH: flush output buffer
 *               TCOFLUSH: Flush input buffer
 *               TCIOFLUSH: Flush input and output buffer
 * @return 0 if successful, -1 if an error occurred
 */
int RawCommProxy::FlushComBuffs( int action)
{
    int retVal = -1;
    if( (m_comGuard.Acquire()) == EOK)
    {
        retVal = FlushDriverFifos(action);

        m_comGuard.Release();
    }
    return( retVal);
}

/**
 * Stub function used for reading driver data in a background thread. Calls RawCommProxy::ReaderThread()
 *
 * @param arg    A pointer to a RaawCommProxy object
 *
 * @return NULL
 */
void* RawCommProxy::ReaderThreadStub( void *arg)
{
    RawCommProxy*   proxy = (RawCommProxy*)arg;

    proxy->ReaderThread();

    return( NULL);
}

/**
 * Class method used to read serial data from the devc driver in a background thread
 */
void RawCommProxy::ReaderThread()
{
    io_pulse_t  pulse;
    int         err;
    bool        timeToStop = false;
    CommThreadState_t   state;

    // Create the channel and connection to receive pulses on
    BposCreateSelfAttachment( &m_threadChId, &m_threadCoId);

    // Subscribe to the devc driver for data notifications
    SIGEV_PULSE_INIT( &m_dataEvent, m_threadCoId, m_poolThreadPriority+1,
                      SERIAL_DATA_READY, 0);
    DriverDataRegister( &m_dataEvent);

    // Tell the world that we are running
    m_threadControl.Acquire();
    m_threadControl.Signal( ThreadRunning);
    m_threadControl.Release();

    // Wait for a message
    while( timeToStop == false)
    {
        err = MsgReceivePulse( m_threadChId, &pulse, sizeof( pulse), NULL);
        if( err == 0)
        {
            Log( LOG_DEV_DATA, "ReaderThread handling pulse %d,%d\n", pulse.code, pulse.value.sival_int);
            // See which pulse we are getting
            switch( pulse.code)
            {
            case SERIAL_DATA_READY:
                HandleDriverNotificationPulse( pulse);
                DriverDataRegister( &m_dataEvent);
                break;
            case SYSTEM_PULSE:
                switch( pulse.value.sival_int)
                {
                case TERMINATE_PULSE:
                    m_threadControl.Acquire();
                    CancelDriverNotification();
                    ConnectDetach( m_threadCoId);
                    ChannelDestroy( m_threadChId);
                    m_threadControl.Signal( ThreadTerminated);
                    m_threadControl.Release();
                    timeToStop = true;
                    break;
                case RESET_PULSE+1:
                    m_threadControl.Acquire();
                    state = m_threadControl.GetValue();
                    switch( state)
                    {
                    case ThreadSuspend:
                        CancelDriverNotification();
                        m_threadControl.Signal( ThreadSuspended);
                        break;
                    case ThreadRun:
                        DriverDataRegister( &m_dataEvent);
                        m_threadControl.Signal( ThreadRunning);
                        break;
                    case ThreadTerminate:
                        timeToStop = true;
                        CancelDriverNotification();
                        ConnectDetach( m_threadCoId);
                        ChannelDestroy( m_threadChId);
                        m_threadControl.Signal( ThreadTerminated);
                        break;
                    default:
                        break;
                    }
                    m_threadControl.Release();
                    break;
                }

                break;
            default:
                Log( LOG_DEV_DATA, "\n\tGot TERMINATE_PULSE\n\n");
                timeToStop = true;
                break;
            }
        }
    }
}

/**
 * Creates the reader thread
 *
 * @return true if the thread was successfully created, false otherwise
 */
bool RawCommProxy::StartReaderThread()
{
    bool    retVal = false;
    int     err;
    uint64_t    timeOut = mSEC_nSEC( 5000LL);

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::StartReaderThread()\n");

    if( (err=m_threadControl.Acquire()) == EOK)
    {
        // If thread is not already running
        if( m_threadTid == -1)
        {
            // Tell the thread to suspend
            m_threadControl.Signal( ThreadRun);
            pthread_create( &m_threadTid, NULL, ReaderThreadStub, (void*)this);
            // Wait for the thread to be suspended
            if( (err=m_threadControl.Wait( ThreadRunning, timeOut)) == EOK)
            {
                retVal = true;
            }
            else
            {
                Log( LOG_ERRORS, "\tError error waiting for thread to start: %s\n", strerror( err));
            }
        }
        else
        {
            Log( LOG_DEV_DATA, "Reader thread is already created: %d\n", m_threadTid);
            retVal = true;
        }
        m_threadControl.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking thread state mutex: %s\n", strerror( err));
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::StartReaderThread()\n");

    return( retVal);
}

/**
 * Pause the reader thread in order to read data directly from the driver
 *
 * @return true if the thread was suspended, false otherwise
 */
bool RawCommProxy::SuspendReaderThread()
{
    bool    retVal = false;
    int     err;
    uint64_t    timeOut = mSEC_nSEC( 5000LL);

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::SuspendReaderThread()\n");

    if( (err=m_threadControl.Acquire()) == EOK)
    {
        // If thread is not suspended and the thread ID is valid
        if( (m_threadControl.GetValue() != ThreadSuspended) && (-1 != m_threadTid))
        {
            // Tell the thread to suspend
            m_threadControl.Signal( ThreadSuspend);
            MsgSendPulse( m_threadCoId, SIGEV_PULSE_PRIO_INHERIT, SYSTEM_PULSE, RESET_PULSE+1);
            // Wait for the thread to be suspended
            if( (err=m_threadControl.Wait( ThreadSuspended, timeOut)) == EOK)
            {
                retVal = true;
            }
            else
            {
                Log( LOG_ERRORS, "\tError error waiting for thread to suspend: %s\n", strerror( err));
            }
        }
        else
        {
            Log( LOG_DEV_DATA, "Reader thread is already suspended\n");
            retVal = true;
        }
        m_threadControl.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking thread state mutex: %s\n", strerror( err));
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::SuspendReaderThread()\n");

    return( retVal);
}

/**
 * Resumes the reader thread after it has been suspended
 *
 * @return true if the thread was resumed, false otherwise
 */
bool RawCommProxy::ResumeReaderThread()
{
    bool    retVal = false;
    int     err;
    uint64_t    timeOut = mSEC_nSEC( 5000LL);

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::ResumeReaderThread()\n");

    if( (err=m_threadControl.Acquire()) == EOK)
    {
        // If thread is not suspended and the thread ID is valid
        if( (m_threadControl.GetValue() != ThreadRunning) && (-1 != m_threadTid))
        {
            m_threadControl.Signal( ThreadRun);
            MsgSendPulse( m_threadCoId, SIGEV_PULSE_PRIO_INHERIT, SYSTEM_PULSE, RESET_PULSE+1);
            if( (err=m_threadControl.Wait( ThreadRunning, timeOut)) == EOK)
            {
                retVal = true;
            }
            else
            {
                Log( LOG_ERRORS, "\tError error waiting for thread to resume: %s\n", strerror( err));
            }
        }
        m_threadControl.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking thread state mutex: %s\n", strerror( err));
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::ResumeReaderThread()\n");

    return( retVal);
}

/**
 * Terminates the reader thread
 *
 * @return true if the thread was terminated, false otherwise
 */
bool RawCommProxy::TerminateReaderThread()
{
    bool    retVal = false;
    int     err;
    uint64_t    timeOut = mSEC_nSEC( 5000LL);

    Log( LOG_FN_ENTRY, "Enter RawCommProxy::TerminateReaderThread()\n");

    if( (err=m_threadControl.Acquire()) == EOK)
    {
        // If thread is not suspended and thread ID is valid
        if( (m_threadControl.GetValue() != ThreadRunning) && (-1 != m_threadTid))
        {
            m_threadControl.Signal( ThreadTerminate);
            MsgSendPulse( m_threadCoId, SIGEV_PULSE_PRIO_INHERIT, SYSTEM_PULSE, RESET_PULSE+1);
            if( (err=m_threadControl.Wait( ThreadTerminated, timeOut)) == EOK)
            {
                pthread_join( m_threadTid, NULL);
                retVal = true;
            }
            else
            {
                pthread_kill( m_threadTid, SIGKILL);
                Log( LOG_ERRORS, "\tError error waiting for thread to terminate: %s\n", strerror( err));
            }
        }
        m_threadTid = -1;
        m_threadControl.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking thread state mutex: %s\n", strerror( err));
    }
    Log( LOG_FN_ENTRY, "Exit RawCommProxy::TerminateReaderThread()\n");

    return( retVal);
}

/**
 * Returns the TaskMon class that this process will belong to
 *
 * @return The TaskMon class that this process will belong to
 * @see TaskMonClass_t
 */
TaskMonClass_t RawCommProxy::GetTaskClass() const
{
    return( TASKMON_CLASS_SERIAL);
}

