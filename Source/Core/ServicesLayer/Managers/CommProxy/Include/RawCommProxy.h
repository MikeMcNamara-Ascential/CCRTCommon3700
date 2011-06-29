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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Include/RawCommProxy.h $
// 
// 12    1/18/07 12:54p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added DecodeIpAddress().
// 
// 11    10/26/06 10:24a Rwiersem
// Changes for the 06102701 core release:
// 
// Added Read() and Write().
// 
// Added DriverBytesAvailable().
// 
// Added a CommIoOcb_t parameter to TransmitMessage().
// 
// 10    6/21/06 4:39p Cward
// Added ability to reload config file without rebooting
//
// 1     6/15/06 1:22p Rwiersem
// Made the test class a friend.  Added LoadAdditionalConfigurationItems()
// and HandlePulse() which are used to reload the configuration without
// restarting the process.
//
// 9     2/15/06 7:57p Cward
// Updated for 06021501 iteration
//
// 8     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 7     7/20/05 9:10p Cward
// Added new TaskMon support. Moved implementation from header.
//
// 6     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 5     6/24/04 1:11a Cward
// Removed pound if 0 block. Updated header block comments to the new
// standard.

// 2     8/17/04 6:47p Bmeinke
// Added ProcessReaderUnblockRequest() to handle unblock requests from
// blocked reader clients
// Added ProcessWriterUnblockRequest() to handle unblock requests from
// blocked writer clients
//
// 2     7/07/04 4:31p Bmeinke
// Added fault server interface for reporting communication faults
// Added CommFaultList object to maintain a list of active comm faults
//
// 4     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 3     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:29p Cward
//
// 1     10/07/03 11:35a Khykin
// Initial checkin
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 2     6/26/03 3:56p Khykin
// Updated with Brians latest changes.
//
// 5     6/24/03 10:19a Bmeinke
// Moved the define of READER_PROCESSES from RawCommProxy.h to the header
// files for the individual comm proxies
//
// 4     6/18/03 6:31p Bmeinke
// NotifyClientErr() no longer takes an optional message argument
// Added snd NotifyClientErr() method which operates on BlockedReader_t
// structs
//
// 3     5/29/03 1:54p Bmeinke
// New dedicated reader thread architecture (have one thread dedicated to
// reading data from the driver)
// Added the instantiation of LogportSubscription::m_subIdLock mutex
// because it has been changed to a static data member of
// LogPortSubscription which is a header only class
// Changed the name of EstablishCommsHandler() to FastInitHandler()
// Broke FastInitHandler() into two additional methods:
// SendFastInitBreakSignal() and SendfastInitmessage() for maintainability
//
// 1     4/30/03 4:44p Bmeinke
// Migration to new personalized configuration management structure
//
// 14    4/30/03 3:40p Bmeinke
// Moved the CommIoOcb_t::messageTimeout to
// RawCommProxy::m_defaultReplyTimeout (initialized from config file)
// Added ArmReplyTimeoutTimer() method
// NotifyClientErr() now returns a bool to indicate if the client was in
// the blocked reader's list
// TransmitMessage() takes an (optional) additional parameter to say
// whether or not we want it to re-subscribe for serial data (this is only
// an issue with reflective buses like ISO)
// ReadSendersBuffer() now takes an additional io_write_t* argument and
// works properly
// Removed m_messageGapPulseVal (not used anymore)
// Message gap timer pulses and reply timeout timer pulses have different
// pulse codes (in HandleMssgGapPulse and HandleReplyTimeoutPulse)
// Removed m_dataReadyCoid (not used)
// Added a default reply timeout timer (m_replyTimeoutTimer)
//
// 13    4/04/03 6:11p Bmeinke
// Added FlushDriverFifos() method
//
// 12    4/04/03 11:21a Bmeinke
// Added OpenPort() and ConfigurePort() virtual methods to allow more
// logical extension to ethernet comm proxies
// Changed the name of SerialDataRegister() to DriverDataRegister()
// Changed the name of CancelSerialNotification() to
// CancelDriverNotification
//
// 11    3/18/03 6:41p Bmeinke
// Added message gap timing via m_mssgGapTimer and m_mssgGapTimerExpired
// data members
// Replaced ParseCommandLine() with HandleCommandLineArg()
// Changed m_currentPortPortProtocol from SerialProtocol_t to
// SerialProtocol (easier to work with)
// Added m_blockedWriters to manage clients blocked waiting to write to
// the port
// Added LowSpeedInit() devctl handler. Used to perform ISO 5-baud
// initialization sequences
// Added an overloaded ReadDriverData() which takes a timeout value in
// nano-seconds
//
// 10    3/06/03 10:42p Bmeinke
// Added overloaded Register() method (called when we receive a
// REGISTER_PULSE)
//
// 9     2/21/03 7:24p Bmeinke
// Made all static member variables non-static (static obsolete because no
// more background thread used for reading serial data)
// Added ReadDriverData() method to read serial data from the devc driver
// Removed methods related to the obsolete reader thread
//
// 8     2/11/03 12:22p Bmeinke
// Added LockForClient() and UnlockForClient() methods to perform actual
// locking/unlocking of port
//
// 7     2/06/03 10:04a Bmeinke
// Added helper method JustWrite() to perform some of the lower level
// writing actions
// Removed SetPortProtocolHandler() and SetMssgProtocolHandler()
// Added EstablishCommsHandler() to handle ISO fast initialization
// functionality
// Moved the BepToPosix() and PosixToBep() methods to the SerialProtocol
// class
// Changed m_defaultPortProtocol and m_currentPortProtocol from termios to
// SerialProtocol_t
// Removed m_blockedReadClient member (use m_blockedReaders)
//
// 6     1/30/03 3:34p Bmeinke
// Added PosixDevCtl() method to handle the POSIX terminal control
// devctl() messages
// ReadPortData() now returns the correct number of bytes read (was always
// returning 0)
//
// 5     1/27/03 10:20p Bmeinke
// Made ReadPortConfig() virtual
//
// 4     1/22/03 3:29a Bmeinke
// GetRegisteredName() now returns 'logport/<LogiicalPortName>' instead of
// DEF_PROXY_PROC_NAME
//
// 3     1/21/03 8:57p Bmeinke
// Modified to be a drop-in replacement for the old logical port driver
//
// 2     1/21/03 3:19a Bmeinke
// Fixed header comments
// Added default port attributes structure (m_defaultPortProtocol)
//
// 1     1/20/03 10:42p Bmeinke
// Serial comm proxy for raw (unformatted) serial data
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _RAW_COMM_PROXY_H_INCLUDED
#define _RAW_COMM_PROXY_H_INCLUDED

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

// Define this here so it does not get defined in the neutrino headers

#ifndef IOFUNC_OCB_T
struct _commIoOcb_t;
#define IOFUNC_OCB_T            struct _commIoOcb_t
#endif

#ifndef RESMGR_OCB_T
struct _commIoOcb_t;
#define RESMGR_OCB_T            struct _commIoOcb_t
#endif

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "ResManagerMt.h"       // Base logical port driver class

#include <limits.h>             // uint8_t
#include <stdint.h>             // uint8_t
#include <atomic.h>             // atomic_add/atomic_sub
#include <sync.h>               // BARRIER_SERIAL_THREAD
#include <termios.h>            // POSIX terminal control
#include <sys/dcmd_chr.h>       // POSIX terminal devctl() command ID's
#include <sys/netmgr.h>         // _ND_LOCAL_NODE
#include <sys/select.h>         // select()
#include <sys/neutrino.h>       // struct _msg_info

#include <set>
#include <map>
#include <string>

#include "BepSync.h"                // BepMutex
#include "LogPortDefs.h"            // Data definitions
#include "BepTimer.h"               // Base class timer object
#include "LogPortFilterString.h"    // AVT Filter byte abstraction
#include "CommFifo.h"               // CommFifo object
#include "IFaultServer.h"           // Comm Fault reporting
#include "SerialProtocol.h"
#include "CommFaultList.h"

#ifndef READER_PROCESSES
    /**
     * Flag used to indicate whether or not the reader thread will process all received data.
     * If set to 0, the reader thread will simply stick received data into our rx fifo and
     * send a pulse back to our I/O channel to indicate that new rx data needs to be processed
     */
    #define READER_PROCESSES        1
#endif

/**
 * Root directory under which comm proxy devices adopt paths
 */
#define COMM_PROXY_ROOT             LOG_PORT_DEV_ROOT

/**
 * Name to register the comm proxy process under
 */
#define DEF_PROXY_PROC_NAME         "logport/CommProxy"

/**
 * Path we will adopt under the /dev/CommProxy dir
 */
#define COMM_PROXY_DEV_PATH         "CommProxy"

/**
 * OCB Structure extended for J2190 client conenctions
 */
typedef struct _commIoOcb_t
{
    /**
     * OCB used (and expected) by the base class I/O routines.
     * !!!!! MUST BE FIRST !!!!!
     */
    resMgrIoOcb_t               baseOcb;

    /**
     * Mask bits to identify what events a client is subscribed for (RX, TX, etc)
     *
     * @see logPortNotifyMask_t
     */
    uint32_t                    subMask;

    /**
     * Flag to indicate if this client has locked the port for exclusive use
     */
    int                         portLockCount;

    /**
     * Client's subscription ID
     */
    LogPortSubscriptionId_t     subscriptionId;

    /**
     * Subscription attributes for RX data subscription
     */
    LogPortSubscription         *rxSubscription;

    /**
     * Subscription attributes for TX data subscription
     */
    LogPortSubscription         *txSubscription;

    /**
     * Subscription attributes for port lock events
     */
    LogPortSubscription         *lockSubscription;

    /**
     * Subscription attributes for port unlock events
     */
    LogPortSubscription         *unlockSubscription;

    /**
     * Mask bits to filter when a subscribed client wants to be notified of
     * port events
     *
     * @see logPortNotifyMask_t
     */
    long                            notifyModeMask;

    /**
     * Number of bytes the client wants buffered prior to being notified
     */
    off_t                       notifyByteCount;

    /**
     * Amount of time to wait (in nanoseconds) for a reply message before a
     * timeout condition is raised. This is a temporary timeout value which
     * will be reset after sending the first message after this value is
     * specified
     */
    uint64_t                    replyTimeout;

    /**
     * notification list used by iofunc_notify*()
     */
    iofunc_notify_t             notify[3];

    /**
     * Reply timeout timer for this client
     */
    CommGapTimer                replyTimeoutTimer;

} CommIoOcb_t;

typedef enum
{
    ThreadRun,
    ThreadRunning,
    ThreadSuspend,
    ThreadSuspended,
    ThreadTerminate,
    ThreadTerminated
} CommThreadState_t;

/**
 * Raw serial port communication proxy. Used to perform port locking and serial
 * data logging. Clients will communicate to a serial device through this process.
 *
 * @author Brian Meinke
 * @version 1
 * @see ResManagerMt
 * @since Jan 20, 2003
 */
class RawCommProxy : public ResManagerMt
{
    friend class CommonCommProxyTest;

public:
    /**
     * Default constructor
     */
    RawCommProxy();

    /**
     * Constructor from command line arguments
     *
     * @param argc   Number of command line arguments to aprse
     * @param argv   Array of command line argument strings
     */
    RawCommProxy( int argc, char *argv[]);

    /**
     * Class destructor
     */
    virtual ~RawCommProxy();

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
     * Perform any server initialization
     *
     * @return true if successful
     *         false if error
     */
    virtual bool InitManager();

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         Success, Fail
     * @since Version 2.5
     */
    virtual const std::string Register(void);

protected:

    /**
     * Load the additional configuration items needed by the RawCommProxy.
     * The "standard" items will be handled by the call to
     * ResManager::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);

    /**
     * Reads logical port configuration from an XML document
     *
     * @param portNode Node containing setup data for our port only
     */
    virtual void ReadPortConfig( const XmlNode *portNode);

    /**
     * Reads the logger configuration for logging serial communication data
     *
     * @param portNode Node containing setup data for our port only
     */
    void ReadCommLogConfig( const XmlNode *portNode);

    /**
     * Reads the port protocol for this logical port from
     * the given XML node
     *
     * @param portNode Node containing setup data for our port only
     */
    virtual void ReadPortProtocol( const XmlNode *portNode);

    /**
     * Reads the name of the file to log serial communication to
     *
     * @param portNode Node containing setup data for our port only
     */
    void ReadCommLogFileName( const XmlNode *portNode);

    /**
     * Method used to attach pulses to the resource manager's I/O
     * channel
     *
     * @return -1 on error, any other value indicates success
     */
    virtual int AttachIoPulses();

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
    virtual bool HandleCommandLineArg( int option, const char *value);

    /**
     * Method used to open a handle to the communication device driver
     *
     * @param devName Path to the communication device to be opened (i.e. /dev/ser1)
     * @param mode    Mode to open the device in (i.e. O_RDWR | O_NONBLOCK)
     *
     * @return A file descriptor to use for communicatin with the device driver or
     *         -1 if an error occurred
     */
    virtual int OpenPort( const char *devName, int mode=O_RDWR);

    /**
     * Method used to close a handle to the communication device driver
     *
     * @return 0 if successful or -1 if an error occurred
     */
    virtual int ClosePort();

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
    virtual int ConfigurePort( const SerialProtocol &portCfg, int mode=TCSANOW);

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
    virtual int ConfigurePort( const struct termios *portCfg, int mode=TCSANOW);

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
    virtual int ConfigurePort( const SerialProtocol_t &portCfg, int mode=TCSANOW);

    /**
     * Decode the IP Adress from the configuration file.
     * Any text data will be translated using available
     * environment variables and replaced in the string.
     * For example, 192.168.MACHINE.20 will be decoded to
     * 192.168.3.20 for machine number 3.
     *
     * @param ipInfo IP Address from the config file.
     *
     * @return Decoded IP adress per above example.
     */
    virtual string DecodeIpAddress(const string &ipInfo);

    /**
     * Tells the communication driver to flush our FIFO(s)
     *
     * @param mode   Which FIFO(s) to flush:
     *               TCIOFLUSH: Flush our rx and tx FIFOs
     *               TCIFLUSH:  Flush only our rx FIFO
     *               TCOFLUSH:  Flush only our tx FIFO
     * @return -1 if an error occurred, any other value for success
     */
    virtual int FlushDriverFifos( int mode);

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
    virtual void ProcessReflection( uint8_t *buff, uint32_t &size);

    /**
     * Method used to place received data into the FIFO's for subscribed
     * clients
     *
     * @param buff   Buffer of received data
     * @param len    Number of bytes in the FIFO
     * @param flag   Flag indicating which FIFOs to process (Rx or Tx)
     */
    virtual void UpdateClientFifos( const char *buff, size_t len, int flag);

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
    virtual IOFUNC_OCB_T* OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device);

    /**
     * Method used to initialize an allocated OCB structure. Called
     * immediately after the OCB is allocated
     *
     * @param ioOcb  OCB structure to be initialized
     */
    virtual void OcbInit(resMgrIoOcb_t *ioOcb);

    /**
     * The purpose of this is to give us a place to free our ocb. It is called
     * by ResManagerBase::ocbFree, which in turn is called as a result of the close
     * being done (e.g. iofunc_close_ocb_default causes it to be called).
     * ResManagerBase::ocbFree was registered through our mount structure.
     *
     * @param ocb    Client's Open Context Block for this connection
     * @see ResManagerBase::ocbFree
     */
    virtual void OcbFree(resMgrIoOcb_t *ocb);

    /**
     * Method used to finalize an allocated OCB structure. Called immediately
     * before the OCB is destroyed
     *
     * @param ioOcb  OCB structure to be finalized
     */
    virtual void OcbFinalize(resMgrIoOcb_t *ioOcb);

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
    virtual int IoCloseOcb(resmgr_context_t *ctp, void *reserved, resMgrIoOcb_t *ioOcb);

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
    virtual int IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb);

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
    virtual int IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * IoWrite handler used to just write data out to the serial driver
     *
     * @param ctp    Dispatch Interface pointer
     * @param ioOcb    ClientOCB for which we are sending the message
     * @param xmtSting The serial data to be sent
     * @param xmtLen   The number of bytes to be written
     * @return -1 if an error occurred, any other value represents the number of bytes written
     */
    virtual int JustWrite( resmgr_context_t *ctp, resMgrIoOcb_t *ioOcb, const uint8_t *xmtSting, const int xmtLen);

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
    virtual int IoUnblock(resmgr_context_t *ctp, io_pulse_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler used to perform any special processing required when a
     * blocked reader client requests to be unblocked (via TimerTimeout)
     *
     * @param ioOcb  Control block for the calling client
     * @param reader Reference to the blocked reader's attributes
     */
    virtual void ProcessReaderUnblockRequest(resMgrIoOcb_t *ioOcb, BlockedReader_t &reader);

    /**
     * Handler used to perform any special processing required when a
     * blocked writer client requests to be unblocked (via TimerTimeout)
     *
     * @param ioOcb  Control block for the calling client
     * @param writer Reference to the blocked writer's attributes
     */
    virtual void ProcessWriterUnblockRequest(resMgrIoOcb_t *ioOcb, BlockedWriter_t &writer);

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
    virtual int IoDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

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
    virtual int PosixDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

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
    virtual int IoNotify(resmgr_context_t *ctp, io_notify_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * IoRead helper function used to reply to a read() request when a client
     * has data available in his fifo
     *
     * @param ctp    Dispatch Interface pointer
     * @param msg    Read message data
     * @param ioOcb  Control Block for this client
     * @see IoRead
     */
    virtual void ReplyToReadClient(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Client request to lock port for exclusive use
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int LockPortHandler(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Client request to unlock the port. Client must have completed a
     * successful lock port command prior to this.
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int UnLockPortHandler( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Method used to check if a serial response string can be added to a
     * client's rx FIFO
     *
     * @param data   Serial string reseived from the port
     * @param ocb    Client connection identifier
     * @return true if the data should be added to the client's FIFO, false otherwise
     */
    virtual bool CanAddToClientFifo( const SerialString_t &data, CommIoOcb_t *ocb);

    /**
     * Adds the gives serial string to the specified client's fifo
     *
     * @param buff   Serial data to be added to the client's fifo
     * @param len    Length of the buffer
     * @param ocb    Client whose FIFO we should update
     *
     * @return true if the client was blocked on a read and we
     *         replied to him before returning, false if we
     *         did not reply to the client
     */
    virtual bool AddToClientFifo( const char *buff, size_t len, CommIoOcb_t *ocb);

    /**
     * Adds the gives serial string to the specified client's fifo
     *
     * @param buff   Serial data to be added to the client's fifo
     * @param ocb    Client whose FIFO we should update
     *
     * @return true if the client was blocked on a read and we
     *         replied to him before returning, false if we
     *         did not reply to the client
     */
    virtual bool AddToClientFifo( const SerialString_t &buff, CommIoOcb_t *ocb);

    /**
     * Method used to check if a client can currently write (i.e.
     * does the client own the port lock or is the port currently
     * unlocked)
     *
     * @param clientOcb Client OCB containing flag indicating if he owns the port lock
     *
     * @return true if client can write to port, false if client cannot write
     */
    virtual bool CanClientWrite( CommIoOcb_t *clientOcb);

    /**
     * Method used to check if a client owns the port lock
     *
     * @param clientOcb Client OCB containing flag indicating if he owns the port lock
     * @return true if client can write to port, false if client cannot write
     */
    virtual bool ClientOwnsPortLock( CommIoOcb_t *clientOcb);

    /**
     * Method used to obtain a port lock for a client
     *
     * @param clientOcb  Client OCB containing flag indicating if he already owns the port lock
     * @param replyId If the port cannot be locked immediately, this is the connection ID
     *                to respond to when the port is locked for the calling client
     * @param block   Flag indicating if the port lock request should block if the port
     *                cannot be locked immediately.
     * @return EOK if port successfully locked, other values indicate error
     */
    virtual int ObtainClientPortLock( CommIoOcb_t *clientOcb, int replyId, bool block);

    /**
     * Method used to release a port lock for a client
     *
     * @param clientOcb Client OCB containing flag indicating if he already owns the port lock
     * @param releaseAll Flag to indicate if we should release all locks on the port help by
     *                   the client or just a single lock (set =0 vs. decrement by 1)
     * @return true if port lock released, false if unable to release port lock
     */
    virtual bool ReleaseClientPortLock( CommIoOcb_t *clientOcb, bool releaseAll=true);

    /**
     * Locks the port for the next waiting client in m_lockWaitlist
     *
     * @param sendReply Flag indicating if we should reply to the client or not
     * @return EOK if port successfully locked
     */
    virtual int LockForNextClient( bool sendReply=true);

    /**
     * Locks the port for the client associated with 'clientOcb'
     *
     * @param clientOcb OCB associated with the client for whom we are locking the port
     * @param replyId   The reply ID to which we should reply after having locked the port
     *
     * @return EOK if port successfully locked
     */
    virtual int LockForClient( CommIoOcb_t *clientOcb, int replyId=-1);

    /**
     * Unlocks the port for the client associated with 'clientOcb'
     *
     * @param clientOcb  OCB associated with the client for whom we are locking the port
     * @param releaseAll Flag to indicate if we should release all locks on the port help by
     *                   the client or just a single lock (set =0 vs. decrement by 1)
     *
     * @return The number of port locks the client still holds
     */
    virtual int UnlockForClient( CommIoOcb_t *clientOcb, bool releaseAll=true);

    /**
     * Method used to check the port lock status
     *
     * @return true if port is currently locked, false if port is currently unlocked
     */
    bool IsPortLocked();

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
    virtual void UpdateBusCommLog( LogPortCommDir_t comDir, const uint8_t *commString,
                                   uint32_t byteCount, CommIoOcb_t *clientOcb);

    /**
     * See which channel to wait on
     *
     * @return Channel id to wait for messages on
     */
    virtual int GetWaitChid();

    /**
     * Method called to read received data from the port (without a mutex wrapper)
     *
     * @param buff   Buffer to put the received data into
     * @param buffSz Size (in bytes) of the buff parameter
     * @return The number of bytes received from the port
     */
    virtual int ReadPortData(uint8_t *buff, size_t buffSz);

    /**
     * Arms a reply timeout timer for the client designated by 'ocb'
     *
     * @param clientOcb The OCB of the client we are arming the reply timeout timer for
     * @param forceArm  Flag touse if we want to unconditionally arm the timer (deafults to false)
     * @return 0 if timer was armed, any other value if not armed
     */
    virtual int ArmReplyTimeoutTimer( CommIoOcb_t *clientOcb, bool forceArm=false);

    /**
     * Method called to read received data from the port (without a mutex wrapper)
     *
     * @param buff   Buffer to put the received data into
     * @param buffSz Size (in bytes) of the buff parameter
     * @return The number of bytes received from the port
     */
    virtual int ReadPortDataUnlocked(uint8_t *buff, size_t buffSz);

    /**
     * Reads serial data from the serial driver
     *
     * @param buff    Buffer to read the serial data into
     * @param buffLen Number of bytes to read
     * @param timeOut Timeout value (in ms)
     * @return Number of bytes read, or -1 if an error occurred
     */
    int ReadDriverData( uint8_t *buff, size_t buffLen, uint32_t timeOut);

    /**
     * Reads bytes from the driver
     * 
     * @param buf    Buffer to read the bytes into
     * @param bytes  Maximum number of bytes to try and read
     * 
     * @return The number of bytes actually read, or -1 (errno is set).
     */
    virtual int Read( void *buf, size_t bytes);
    
    /**
     * Checks how many bytes are available for reading from the driver
     * 
     * @return How many bytes are available for reading from the driver
     */
    virtual int DriverBytesAvailable();
    
    /**
     * Reads serial data from the serial driver
     *
     * @param buff    Buffer to read the serial data into
     * @param buffLen Number of bytes to read
     * @param timeOut Timeout value (in nanoSeconds)
     * @return Number of bytes read, or -1 if an error occurred
     */
    int ReadDriverData( uint8_t *buff, size_t buffLen, const uint64_t &timeOut);

    /**
     * Handles pulses delivered to our resource manager I/O channel
     *
     * @param pulse  Pulse received on the I/O channel
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t HandleResMgrPulse(io_pulse_t &pulse);

    /**
     * Handler for driver 'data ready to read' notification pulses.
     *
     * @param pulse  Pulse received on the I/O channel
     *
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t HandleDriverNotificationPulse(io_pulse_t &pulse);

    /**
     * Handler for MSSG_GAP_TIMER pulses
     *
     * @param pulse  Pulse received on the I/O channel
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t HandleMssgGapPulse(io_pulse_t &pulse);

    /**
     * Handler for module reply timeout error pulses
     *
     * @param pulse  Pulse received on the I/O channel
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t HandleReplyTimeoutPulse(io_pulse_t &pulse);

    /**
     * Reads all available data from the serial port
     *
     * @param mssg   Buffer to read the serial data into
     * @return Number of bytes read from the port
     */
    virtual int ReadSerialData( SerialString_t &mssg);

    /**
     * Notifies a read-blocked client of a message error
     *
     * @param ocb     OCB of the client to whom we need to repond
     * @param errCode Error code to respond with
     * @return Return the status of removing a client.
     */
    virtual bool NotifyClientErr( const CommIoOcb_t *ocb, int errCode);

    /**
     * Notifies a read-blocked client of a message error
     *
     * @param client  Blocked reader slient to be inblocked
     * @param errCode Error code to respond with
     *
     * @return true
     */
    virtual bool NotifyClientErr( const BlockedReader_t &client, int errCode);

    /**
     * Sends subscriptions notifications to all clients subscribed for events
     * contained in the eventMask parameter
     *
     * @param eventMask Bitwise flag indicating which port event(s) occurred
     */
    virtual void UpdateSubscribers( int eventMask);

    /**
     * Method called in response to a "data ready" pulse from the reader thread
     *
     * @return RES_MGR_CMD_NONE if successful,
     *         RES_MGR_CMD_TERMINATE on error
     */
    virtual ResManagerCmd_t ProcessReadData();

    /**
     * Handler method for client subscription requests
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int PortSubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler method for client unsubscription requests
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int PortUnsubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                       resMgrIoOcb_t *ioOcb);

    /**
     * Client request to reset his connection to the port driver.
     * Essentially only resets the client's FIFO.
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int ResetConnectionHandler( resmgr_context_t *ctp, io_devctl_t *msg,
                                        resMgrIoOcb_t *ioOcb);

    /**
     * Server commanded to reset (re-initialize)
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int ResetDriverHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                   resMgrIoOcb_t *ioOcb);

    /**
     * Devctl handler used to set a temporary message reply timeout value
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int SetReplyTimeoutHandler( resmgr_context_t *ctp, io_devctl_t *msg,
                                        resMgrIoOcb_t *ioOcb);

    /**
     * Devctl handler used to temporarily disable the transmitter on the physical serial port
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int BreakHandler( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Devctl handler used to perform an ISO-K fast initialization
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     *
     * @return EOK if successful, other on error
     */
    virtual int FastInitHandler(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Sends an ISOK fast initializtion break pattern( N ms low, M ms high)
     *
     * @param nsLowTime Number of nano seconds to assert the break signal for
     * @param nsHiTime  Number of nanoseconds to wait after the break signal has been asserted
     *
     * @return EOKJ if successful, error code if an error occurs
     */
    virtual int SendFastInitBreakSignal( const uint64_t &nsLowTime, const uint64_t &nsHiTime);

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
    virtual int SendFastInitMessage( const uint8_t *initMssg, uint32_t mssgLen);

    /**
     * Perform a low speed (i.e. 5 baud) initialization
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     *
     * @return EOK if successful, other on error
     */
    virtual int LowSpeedInit(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Method called before performing a LOG_PORT_RECONNECT. This method
     * should check that t he calling client owns the active port lock and
     * should suspend the reader thread
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     *
     * @return EOK if successful, other on error
     */
    virtual int ReconnectHandlerVerify(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

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
    virtual int ReconnectHandler(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

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
    virtual int ReconnectHandlerFinalize(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Method used to check if we should notify a client that subscription
     * data is available
     *
     * @param clientOcb    Client's connection ID containing his subscription attributes
     * @param eventMask The type of notification event were checking (TRansmit, Receive, etc)
     * @return Mask indicating which notifications events can be delivered
     */
    virtual int ShouldNotifyClient( const CommIoOcb_t *clientOcb, int eventMask);

    /**
     * Checks if the subscriber described by the sub argument can receive
     * subscription notifications
     *
     * @param sub    subscriber whose ability to receive subscription notifications
     *               should be checked
     * @return true if the subscriber can be notified, false otherwise
     */
    virtual bool CanNotifySubscriber( LogPortSubscription *sub);

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
    virtual int TransmitMessage( const SerialString_t &message, CommIoOcb_t *clientOcb=NULL);

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
    virtual int TransmitMessage( const uint8_t *message, int msgLen, CommIoOcb_t *clientOcb=NULL);

    /**
     * Writes data to the driver
     * 
     * @param buff   The data buffer to write
     * @param bytes  The number of bytes to write from buff
     * 
     * @return The number of bytes written, or -1 if an error occurred (errno is set).
     */
    virtual int Write( const void* buff, size_t bytes);
    
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
    int ReadSendersBuffer( resmgr_context_t *ctp, SerialString_t &rawMessage, const io_write_t *writeMsg);

    /**
     * Creates a new and unique subscription ID
     *
     * @return Unique subscription ID
     */
    LogPortSubscriptionId_t CreateUniqueSubscriptionId();

    /**
     * Registers the caller for notifications from the serial driver when data is
     * available to be read from the serial port
     *
     * @param dataEvent sigevent structure used to deliver the notification event
     * @return 0 if successful, -1 if an error occurred
     */
    virtual int DriverDataRegister( const struct sigevent *dataEvent);

    /**
     * Unregisters the caller for notifications from the serial driver when data is
     * available to be read from the serial port
     */
    virtual void CancelDriverNotification();

    /**
     * Set a fault to indicate a communication error
     *
     * @param faultReason    Description of the comm fault
     */
    virtual void SetCommFault( const std::string &faultReason);

    /**
     * Clears a fault which indicated a communication error
     */
    virtual void ClearCommFault();

    /**
     * Changes the port protocol settings
     *
     * @param newProtocol
     *               New protocol to be applied
     * @param flags  ProtocolFlags_t flag indicating which of the port protocol
     *               settings are valid (and should be applied).
     * @see ProtocolFlags_t
     */
    void SetPortProtocol( SerialProtocol newProtocol, uint32_t flags=PORT_SET_ALL);

    /**
     * Changes the port protocol settings
     *
     * @param newProtocol
     *               New protocol to be applied
     * @param flags  ProtocolFlags_t flag indicating which of the port protocol
     *               settings are valid (and should be applied).
     * @see ProtocolFlags_t
     */
    void SetPortProtocol( const SerialProtocol_t &newProtocol, uint32_t flags=PORT_SET_ALL);

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
    void InitializeProtocol( SerialProtocol &portProtocol, uint32_t flags=PORT_SET_ALL);

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
    void InitializeProtocol( SerialProtocol_t &portProtocol, uint32_t flags=PORT_SET_ALL);

    /**
     * Outputs a serial string via the logger
     *
     * @param logMask   Verbose mask required for string to be printed
     * @param prefix    String to pre-pend to output (i.e. "Response" would
     *                  result in "Response: $12...")
     * @param srlString Serial string to be printed
     */
    void PrintSerialString( uint32_t logMask, const char *prefix, const SerialString_t &srlString);

    /**
     * Outputs a serial string via the logger
     *
     * @param logMask   Verbose mask required for string to be printed
     * @param prefix    String to pre-pend to output (i.e. "Response" would
     *                  result in "Response: $12...")
     * @param srlString Serial string to be printed
     * @param len       Number of bytse in the serial string
     */
    void PrintSerialString( uint32_t logMask, const char *prefix, const uint8_t *srlString, uint32_t len);

    /**
     * Retrieves the name under which the process should register
     *
     * @param procName Returned name to register as
     */
    virtual void GetRegisteredName( string &procName);

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
    int FlushComBuffs( int action=TCIOFLUSH);

    /**
     * Stub function used for reading driver data in a background thread. Calls RawCommProxy::ReaderThread()
     *
     * @param arg    A pointer to a RaawCommProxy object
     *
     * @return NULL
     */
    static void* ReaderThreadStub( void *arg);

    /**
     * Class method used to read serial data from the devc driver in a background thread
     */
    virtual void ReaderThread();

    /**
     * Creates the reader thread
     *
     * @return true if the thread was successfully created, false otherwise
     */
    virtual bool StartReaderThread();

    /**
     * Pause the reader thread in order to read data directly from the driver
     *
     * @return true if the thread was suspended, false otherwise
     */
    virtual bool SuspendReaderThread();

    /**
     * Resumes the reader thread after it has been suspended
     *
     * @return true if the thread was suspended, false otherwise
     */
    virtual bool ResumeReaderThread();

    /**
     * Terminates the reader thread
     *
     * @return true if the thread was terminated, false otherwise
     */
    virtual bool TerminateReaderThread();

    /**
     * Returns the TaskMon class that this process will belong to
     *
     * @return The TaskMon class that this process will belong to
     * @see TaskMonClass_t
     */
    virtual TaskMonClass_t GetTaskClass() const;

protected:

    /**
     * Mutex used to guard our shared data
     */
    pthread_mutex_t             m_dataLock;

    /**
     * Mutex used to ensure only one thread can create a subscription ID at a time
     */
    pthread_mutex_t             m_subIdLock;

    /**
     * Path for our comm proxy to adopt
     */
    string                      m_pathName;

    /**
     * Name of this logical port
     */
    string                      m_parentName;

    /**
     * Port identifier number
     */
    uint32_t                    m_portId;

    /**
     * Client fifo sizes
     */
    uint32_t                    m_fifoSize;

    /**
     * Default port comm attributes
     */
    SerialProtocol_t            m_defaultPortProtocol;

    /**
     * Current port protocol settings
     */
    SerialProtocol              m_currentPortProtocol;

    /**
     * Protocol options as read from the config file
     */
    SerialProtocol              m_configProtocol;

    /**
     * Registered name identifier
     *
     * @see name_attach
     */
    name_attach_t               *m_nameId;

    /**
     * Condvar used to lock the port for exclusive use
     */
    BepCondVar< bool>           m_portLock;

    /**
     * List of clients subscribed for received data
     *
     * @see PortSubscrList
     */
    PortSubscrList              m_rxSubscribers;

    /**
     * List of clients subscribed for transmit data
     *
     * @see PortSubscrList
     */
    PortSubscrList              m_txSubscribers;

    /**
     * List of clients subscribed for port lock events
     *
     * @see PortSubscrList
     */
    PortSubscrList              m_lockSubscribers;

    /**
     * List of clients subscribed for port unlock events
     *
     * @see PortSubscrList
     */
    PortSubscrList              m_unlockSubscribers;

    /**
     * Logger interface object used to log general serial communication
     * data
     */
    ILogger                     m_generalCommLog;

    /**
     * Variable used to hold the number of reflection bytes expected from
     * the port
     */
    BepCondVar< uint32_t>       m_reflectionCount;

    /**
     * List of clients waiting to lock the port
     */
    LockWaitList                m_lockWaitList;

    /**
     * List of clients waiting for data to be received
     */
    BlockedReaderList           m_blockedReaders;

    /**
     * List of clients waiting to write data
     */
    BlockedWriterList           m_blockedWriters;

    /**
     * FIFO to receive data into
     */
    SerialString_t              m_rawRxBuff;

    /**
     * Mutex used to guard access to the rx buff
     */
    BepMutex                    m_rxBuffLock;

    /**
     * Pulse used to notify our thread pool that new data is available from the serial port
     *
     * @see io_pulse_t
     */
    io_pulse_t                  m_irqPulse;

    /**
     * Thread ID of the thread used to perform read() operations from the serial port
     */
    pthread_t                   m_readThreadId;

    /**
     * Channel which we will be listening on for the reader thread to deliver the
     * "data ready" pulses during the Avt Initialization sequence
     */
    int                         m_dataReadyChId;

    /**
     * Mutex to guard against simultaneous access to the com port by different threads
     */
    BepMutex                    m_comGuard;

    /**
     * Event we pass to the serial driver to tell him how to notify us when data is
     * available for reading
     */
    struct sigevent             m_dataEvent;

    /**
     * Event we pass to the serial driver to tell him how to notify us when data is
     * available for reading during the AVT initialization process
     */
    struct sigevent             m_avtInitDataEv;

    /**
     * Name of the serial port to communicate on
     */
    std::string                 m_comPortName;

    /**
     * File descriptor for the serial port we use to communicate with the AVT box
     */
    int                         m_comPortFd;

    /**
     * Connection on which the reader threadis listening for pulses on. Used to send the
     * thread a terminate pulse
     */
    int                         m_readThreadCoid;

    /**
     * Buffer into which received data is placed
     */
    CommFifo                    m_rxFifo;

    /**
     * Flag to indicate whether or not we need to wait for the
     * register pulse
     */
    bool                        m_needToRegister;

    /**
     * Flag used to say whether or not the inter-message gap has expired
     */
    bool                        m_mssgGapExpired;

    /**
     * Timer used to time inter-message gaps
     */
    CommGapTimer                m_mssgGapTimer;

    /**
     * Default reply timeout time (ns)
     */
    uint64_t                    m_defaultReplyTimeout;

    /**
     * Channel used by the reader thread to receive pulses on
     */
    int                         m_threadChId;

    /**
     * Connection to the reader thread channel to send pulses to
     */
    int                         m_threadCoId;

    /**
     * Thread ID of the reader thread
     */
    int                         m_threadTid;

    /**
     * Cond var used to control the executiuon of the reader thread
     */
    BepCondVar< CommThreadState_t>      m_threadControl;

    /**
     * Fault server interface used to set/clear comm faults
     */
    IFaultServer                    m_faultServer;

    /**
     * List of fault that we have sent to the fault server. Each
     * call to SetCommFault() adds an entry to this list; each call
     * to ClearCommFault() removes an entry from this list. The purpose
     * of this list is to avoid unnecessary IPC messages to the fault
     * server to clear a fault that is not set
     */
    CommFaultList               m_activeFaultList;

};

#endif // _RAW_COMM_PROXY_H_INCLUDED
