//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/Include/ResManagerBase.h $
// $Author:: Rwiersem                                                         $
//   $Date:: 10/30/07 4:01p                                                   $
//
// Description:
//  Single threaded resource manager base class
//
//==============================================================================
//
//     Copyright  1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/Include/ResManagerBase.h $
// 
// 4     10/30/07 4:01p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     4/13/06 4:07a Cward
// Moved the code for Register() and ReloadConfiguration() from the header
// file to the cpp file Removed the chId parameter to IoMssgLoop()
//
// 2     2/15/06 4:02p Bmeinke
// Moved the code for Register() and ReloadConfiguration() from the header
// file to the cpp file
// Removed the chId parameter to IoMssgLoop()
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:29p Cward
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 2     6/26/03 3:56p Khykin
// Updated with Brians latest changes.
//
// 1     6/24/03 2:09p Bmeinke
//
// 1     4/30/03 4:41p Bmeinke
// Migration to new personalized configuration management structure
//
// 16    3/14/03 9:25p Bmeinke
// Command line arguments are processed using the new
// HandleCommandLineArg() virtual method. This allows for proper
// processing of arguments required by sub-classes
//
// 15    2/06/03 10:21a Bmeinke
// Removed unused include's
//
// 14    1/22/03 3:30a Bmeinke
// Added DetachIoPulse() method to detach a pulse from our I/O channel
//
// 1     1/18/03 8:06a Khykin
// Initial Checkin From DCTC
//
// 13    6/05/02 11:23p Bmeinke
// Added static ivar m_resMgrStatus: used by derived classes to know when
// it is time to terminate or re-load config when derived classes do not
// block on MsgReceive() and therefore do not receive the TERMINATE/RELOAD
// pulses
//
// 12    4/04/02 3:00a Bmeinke
// Moved the definition of mSEC_nSEC to BepDefs.h
//
// 11    3/22/02 1:43p Bmeinke
// Added helper method ReadSendersBuffer() to read a client's buffer when
// processing a IoWrite message
//
// 10    2/04/02 4:31p Bmeinke
// Added OcbInit() and OcbFinalize() methods to initialize and cleanup
// OCB's
//
// 9     12/05/01 10:07p Brian.meinke
// Made the global SignalHandler() method a static member of
// ResManagerBase() in order to use Log() functionality
//
// 8     12/05/01 12:14p Brian.meinke
// Added data member m_attachedPulses to hold all the pulse codes attached
// to the I/O channel
// Added AttachIoPulse() method for attaching a single pulse to the I/O
// channel
//
// 7     11/28/01 10:37p Brian.meinke
// Added virtual AttachIoPulses() method to allow descendant classes to
// attach their own specific pulses to the I/O channel
//
// 6     11/21/01 8:42p Brian.meinke
// First integration with existing ResourceManager derived classes
// (UniverseMgr)
//
// 5     11/16/01 9:42a Brian.meinke
// Removed constructor which took command line arguments
//
// 4     11/15/01 2:54p Brian.meinke
// Added constructor using command line arguments
// Added ParseCommandLine method
//
// 3     11/14/01 11:03p Brian.meinke
// Added inode argument to PathAttach
//
// 2     11/14/01 1:58a Brian.meinke
// Added VSS file header
// Added config file node tag definitions
// Now inherits from BepComponent
// Moved implementation to CPP file
// Added SignalHandler()
// Added connection to send control pulses to
// Attach SYSTEM_PULSE's to the I/O channel
// Added static PulseHandler() method to handle the SYSTEM_PULSes
// delivered through the I/O channel
//
//******************************************************************************

#ifndef _RES_MANAGER_BASE_H_INCLUDED
#define _RES_MANAGER_BASE_H_INCLUDED

class ResManagerBase;

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

// Define this here so it does not get defined in the neutrino headers

#ifndef IOFUNC_OCB_T
struct _resMgrIoOcb_t;
#define IOFUNC_OCB_T            struct _resMgrIoOcb_t
#endif

#ifndef RESMGR_OCB_T
struct _resMgrIoOcb_t;
#define RESMGR_OCB_T            struct _resMgrIoOcb_t
#endif

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Header Files
#include <errno.h>
#include <stdarg.h>             // Variable argument stuff
#include <unistd.h>             // getpid
#include <fcntl.h>              // O_CREAT
#include <semaphore.h>          // POSIX semaphores
#include <sys/resmgr.h>         // Resource Manager Stuff
#include <sys/dispatch.h>       // Message dispatch interface
#include <sys/types.h>          // pid_t, etc
#include <pthread.h>            // basic thread and mutex
#include <sys/netmgr.h>         // ND_LOCAL_NODE define
#include <sys/iofunc.h>

#include <string>
#include <map>

#include "BepComponent.h"       // Standard BEP base component
#include "Bepos.h"              // Standard types
#include "BepException.h"       // Standard Exception
#include "XmlParser.h"          // XML Parser definition
#include "XmlDefs.h"            // XML definitions
#include "XmlNode.h"            // XML Node definitions
#include "BepXmlUtils.h"        // XML Utilities
#include "LogDefs.h"            // Logger Interface
#include "GeneralUtilities.h"   // Miscellaneous utilities
#include "HwDataMap.h"          // Data Table
#include "ThreadList.h"         // Thread safe thread id list
#include "tchar.h"              // Generic wide/ascii character functions
#include "IBepCommunication.h"  // Comm objects

///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Function type used to allocate new resource manager OCBs
 */
typedef IOFUNC_OCB_T    *(*ocb_calloc_t)(resmgr_context_t *ctp, IOFUNC_ATTR_T *attr);
/**
 * Function type used to de-allocate old resource manager OCBs
 */
typedef void            (*ocb_free_t)(IOFUNC_OCB_T *ocb);

#ifdef __cplusplus
}
#endif

/**
 * Config File Setup Section node tag
 */
#define CFG_SETUP_TAG       XML_T("Setup")

/**
 * Config File Data Map Section Tag
 */
#define CFG_DATAMAP_TAG     XML_T("DataMap")

/**
 * Config file tag name for a BAse Address specifier node
 */
#define CFG_BASEADDR_TAG    XML_T("BaseAddress")

/**
 * Config File Verbose Node Tag
 */
#define CFG_VERBOSE_TAG     XML_T("Verbose")

/**
 * Config File Local Echo Node Tag
 */
#define CFG_LOCALECHO_TAG   XML_T("LocalEcho")

/**
 * Config File Demo Mode Node Tag
 */
#define CFG_DEMO_TAG        XML_T("Demo")

/**
 * Base Address AUTO detection node value
 */
#define CFG_AUTO_VAL        XML_T("AUTO")

/**
 * Resource Manager Open Control Block type
 * Used to hold data for a client connection
 */
typedef struct _resMgrIoOcb_t
{
    /**
     * POSIX I/O dispatch OCB structure
     */
    iofunc_ocb_t    m_ioOcb;

    /**
     * Pointer to the controlling I/O manager
     */
    ResManagerBase  *m_mgr;

    /**
     * Information about the last message that this client sent
     */
    struct _msg_info            mssgInfo;

} resMgrIoOcb_t;


/**
 * Data type used to hold information about system pulses we receive
 */
enum _ResManagerCmd_t
{
    /**
     * Normal operation
     */
    RES_MGR_CMD_NONE,

    /**
     * Recevied a reload config pules
     */
    RES_MGR_CMD_CONFIG,

    /**
     * Received a terminate pulse
     */
    RES_MGR_CMD_TERMINATE
};
typedef enum _ResManagerCmd_t   ResManagerCmd_t;

/**
 * Resource manager base class.
 *
 * @author Brian.Meinke
 * @version 1.0
 */
class ResManagerBase : public BepComponent
{
public:
    /**
     * Default constructor
     */
    ResManagerBase();

    /**
     * Class destructor
     */
    ~ResManagerBase();

    /**
     * Perform any server initialization
     *
     * @return true if successful
     *         false if error
     */
    virtual bool InitManager();

    /**
     * Performs any necessary manager cleanup (free memory, detach any
     * adopted path names, etc)
     */
    virtual void CleanupManager();

    /**
     * Method called to enter I/O message processing loop. Will not
     * return until we get a reload config pulse or a terminate pulse
     *
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    ResManagerCmd_t Execute();

    /**
     * Reads configuration data from the given XML node
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         Success, Fail
     * @since Version 2.5
     */
    const std::string Register(void);

    /**
     * Reloads the configuration file and resets the
     * base components objects.
     * @since Version 1.0
     */
    virtual void ReloadConfiguration(void);

    /**
     * Mutex used to guard our signal handler
     */
    static pthread_mutex_t  m_signalMtx;

    /**
     * Connection to deliver control pulses to
     */
    static int              m_pulseSigCoId;

protected:

    /**
     * Parses command line parameters
     *
     * @param argc      Number of command line arguments in the argv parameter
     * @param argv      Command line option strings
     * @param optstring getopt() style list of command line switches to look for
     */
    virtual void ParseCommandLine( int argc, char *argv[], const char *optstring=NULL);

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
     * Reads configuration data from config file
     *
     * @return true if config data successfully loaded, false otherwise
     */
    bool ReadConfigFile();

    /**
     * Initializes the message dispatch interface
     *
     * @return true on success
     *         false on error
     */
    bool InitDispatchInterface();

    /**
     * Registers the calling process as a resource manager
     *
     * @return 0 if successful
     *         -1 if error; use errno to obtain more error information
     */
    int RegisterManager();

    /**
     * Method used to attach pulses to the resource manager's I/O
     * channel
     *
     * @return -1 on error, any other value indicates success
     */
    virtual int AttachIoPulses();

    /**
     * Method used to attach a pulse to the resource manager's I/O
     * channel
     *
     * @param pulseCode Pulse code to attach to the resource manager's I/O channel
     * @return -1 on error, any other value indicates success
     */
    int AttachIoPulse( int pulseCode);

    /**
     * Method used to detach a pulse from the resource manager's I/O
     * channel
     *
     * @param pulseCode Pulse code to detach to the resource manager's I/O channel
     * @return -1 on error, any other value indicates success
     */
    void DetachIoPulse( int pulseCode);

    /**
     * Initializes the I/O message handler functions to the default OS values.
     *
     * @param ocbAllocFn Function to be used to allocate OCBs
     * @param ocbFreeFn  Function to be used to free OCBs
     */
    void InitHandlers( void* ocbAllocFn, void* ocbFreeFn);

    /**
     * Attaches the calling process as the IO manager of the given 'pathToAdopt'
     * path. The OS will send messages to our process when IO occurs within the
     * 'pathToAdopt' path. The OS will include a unit number in IO messages
     * pertaining to our path. This unit number is identical to the device number
     * returned by this function. The OS does not include the path in its IO
     * messages, so a process must keep track of device number/path relationships.
     *
     * @param pathToAdopt
     *               Path namespace to take over
     * @param size   Size of the resource (in bytes)
     * @param mode   File 'type' our path will appear as (i.e. Regular File, Directory, etc)
     * @param inode  inode value to assign to this path attachment
     * @param flags  Flags to pass to the kernel (0=default)
     * @param ioAttr Attribute structure to use for this path (default=NULL).
     *               If NULL, a default attribute structure will be used
     * @param init   Flag to indicate if we need to initialize the attributes
     *               structure (normally false)
     * @return the device number associated with the path.
     */
    int PathAttach( const char *pathToAdopt, int size=0, int mode=S_IFNAM|0666,
                    int inode=0, int flags=0, IOFUNC_ATTR_T *ioAttr=NULL, bool init=true);

    /**
     * Attaches the calling process as the IO manager of the given 'pathToAdopt'
     * path. The OS will send messages to our process when IO occurs within the
     * 'pathToAdopt' path. The OS will include a unit number in IO messages
     * pertaining to our path. This unit number is identical to the device number
     * returned by this function. The OS does not include the path in its IO
     * messages, so a process must keep track of device number/path relationships.
     * THe attributes structure is assumed to have already been intialized (i.e.
     * this function WILL NOT initialize it)
     *
     * @param pathToAdopt
     *               Path namespace to take over
     * @param ioAttr Attribute structure to use for this path (default=NULL).
     *               If NULL, a default attribute structure will be used
     * @return the device number associated with the path.
     */
    int PathAttach( const char *pathToAdopt, IOFUNC_ATTR_T *ioAttr);

    /**
     * Forms a device path for a manager to adopt
     *
     * @param pathToAdopt
     *               Suffix to be adopted (i.e Ain, Icm, etc)
     * @return
     */
    string MakeDevPath( const char *pathToAdopt);

    /**
     * Detaches the calling process from the path identified by deviceNumber. The
     * deviceNumber should have been obtained through a previously successful call
     * to PathAttach.
     *
     * @param deviceNumber
     *               Device to detach the calling process from (obtained from PathAttach)
     *
     * @return 0 if successful
     *         -1 if error; use errno to obtain more error information
     *
     * @see PathAttach
     */
    int PathDetach( int deviceNumber);

    ///////////////////////////////////////////////////////////////////////////
    //      I/O Handler methods
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    /**
     * Handles I/O mesages
     *
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t IoMssgLoop();

    /**
     * Handles pulses delivered to our resource manager I/O channel
     *
     * @param pulse  Pulse received on the I/O channel
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t HandleResMgrPulse( io_pulse_t &pulse);

    /**
     * The purpose of this is to give us a place to allocate our own ocb. It is
     * called as a result of the open being done (e.g. iofunc_open_default
     * causes it to be called). We registered it through the mount structure.
     *
     * @param ctp    Resource manager's context block
     * @param device Attributes for this client connection (an OCB)
     * @return Pointer to newly allocated OCB or NULL if error
     */
    static IOFUNC_OCB_T* ocbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device);

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
    virtual void OcbInit( resMgrIoOcb_t *ioOcb);

    /**
     * The purpose of this is to give us a place to free our ocb.
     * It is called as a result of the close being done
     * (e.g. iofunc_close_ocb_default causes it to be called). We
     * registered it through the mount structure.
     *
     * @param ocb    Client's Open Context Block for this connection
     */
    static void ocbFree(IOFUNC_OCB_T *ocb);

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
    virtual void OcbFinalize( resMgrIoOcb_t *ioOcb);

    /**
     * Message handler for _IO_OPEN I/O messages
     *
     * @param ctp    Resource Manager context block
     * @param msg    Received data message data
     * @param handle Pointer to client connection attributes (an OCB)
     * @param extra  Pointer to manager specific private data (optional)
     * @return 0 the number of bytes sent to the client
     *         <0 if error occurs
     * @see open
     */
    static int ioOpen( resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra);

    /**
     * Message handler for _IO_OPEN I/O messages
     *
     * @param ctp    Resource Manager context block
     * @param msg    Received data message data
     * @param handle Pointer to client connection attributes (an OCB)
     * @param extra  Pointer to manager specific private data (optional)
     * @return 0 the number of bytes sent to the client
     *         <0 if error occurs
     * @see ResManagerBase::ioOpen
     */
    virtual int IoOpen( resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra);

    /**
     * Message handler for _IO_READ I/O messages
     *
     * @param ctp    Dispatch Interface pointer
     * @param msg    Read message data
     * @param ocb    Control Block for this client
     * @return 0 if successful
     *         <0 if error occurs
     */
    static int ioRead(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);

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
    virtual int IoRead( resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Message handler for _IO_WRITE I/O messages
     *
     * @param ctp    Dispatch Interface pointer
     * @param msg    Write message data
     * @param ocb    Control Block for this client
     * @return 0 if successful
     *         <0 if error occurs
     */
    static int ioWrite( resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

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
    virtual int IoWrite( resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Function synthesized by the base-layer library when the last
     * close has been received for a particular OCB.
     *
     * @param ctp      Dispatch interface pointer
     * @param reserved Reserved for ther base-layer library
     * @param ocb      Control block for the client
     * @return 0 if successful
     *         < 0 if error occurs
     */
    static int ioCloseOcb(resmgr_context_t *ctp, void *reserved, RESMGR_OCB_T *ocb);

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
     * Handles the message that requests information about the
     * resource associated with the passed OCB
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Stat structure to return to the client
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioStat(resmgr_context_t *ctp, io_stat_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Handles the message that requests information about the
     * resource associated with the passed OCB. Called by ioStat
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Stat structure to return to the client
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManagerBase::ioStat
     */
    virtual int IoStat(resmgr_context_t *ctp, io_stat_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler responsible for installing, polling, or removing a notification
     * handler
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Notification message
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioNotify(resmgr_context_t *ctp, io_notify_t *msg, RESMGR_OCB_T *ocb);

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
     * Handler routine for custom device commands (in response to
     * a client devctl() call)
     *
     * @param ctp    Resource Manager context block
     * @param msg    Received data message data
     * @param ocb    Client Open Context Block block
     * @return 0 if successful
     *         < 0 if error occurs
     */
    static int ioDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);

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
    virtual int IoDevCtl( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler for kernel pulses issued as a result of a alient's attempt
     * to unblock during the I/O mesage phase of an I/O operation
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Pulse from the kernel
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioUnblock(resmgr_context_t *ctp, io_pulse_t *msg, RESMGR_OCB_T *ocb);

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
     * Handler responsible for returning the value of a configurable
     * parameter (name) for the resource associated with the passed OCB
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Pathconf message
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 0if error
     */
    static int ioPathconf(resmgr_context_t *ctp, io_pathconf_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Handler responsible for returning the value of a configurable
     * parameter (name) for the resource associated with the passed OCB.
     * Called by ioPathconf
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Pathconf message
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 0if error
     * @see ResManagerBase::ioPathconf
     */
    virtual int IoPathconf(resmgr_context_t *ctp, io_pathconf_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler called in response to a client lseek() call
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Lseek message (how far to seek and from where)
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioLseek(resmgr_context_t *ctp, io_lseek_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Handler called in response to a client lseek() call. Called by ioLseek
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Lseek message (how far to seek and from where)
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManagerBase::ioLseek
     */
    virtual int IoLseek(resmgr_context_t *ctp, io_lseek_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler responsible for changing the mode of the resource associated
     * with the passed OCB to the mode passed in the msg parameter
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Chmod message (new mode value)
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioChmod(resmgr_context_t *ctp, io_chmod_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Handler responsible for changing the mode of the resource associated
     * with the passed OCB to the mode passed in the msg parameter.
     * Called by ioChmod
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Chmod message (new mode value)
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManagerBase::ioChmod
     */
    virtual int IoChmod(resmgr_context_t *ctp, io_chmod_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler responsible for changin the User ID/Groiup ID for the resource
     * associated with the passed OCB
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Chown message (new user/group ID)
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioChown(resmgr_context_t *ctp, io_chown_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Handler responsible for changin the User ID/Groiup ID for the resource
     * associated with the passed OCB. Called by ioChown
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Chown message (new user/group ID)
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManagerBase::ioChown
     */
    virtual int IoChown(resmgr_context_t *ctp, io_chown_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler responsible for modifying the update time for the resource
     * associated with the passed OCB
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Update time message (new update time to be applied)
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioUtime(resmgr_context_t *ctp, io_utime_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Handler responsible for modifying the update time for the resource
     * associated with the passed OCB. Called by ioUtime
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Update time message (new update time to be applied)
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManagerBase::ioUtime
     */
    virtual int IoUtime(resmgr_context_t *ctp, io_utime_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Similar to the io_open() connect handler except an already open file
     * descriptor is given instead of a pathname
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Openfd message (file descriptor to open)
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioOpenFd(resmgr_context_t *ctp, io_openfd_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Similar to the io_open() connect handler except an already open file
     * descriptor is given instead of a pathname. Called by ioOpenFd
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Openfd message (file descriptor to open)
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManagerBase::ioOpenFd
     */
    virtual int IoOpenFd(resmgr_context_t *ctp, io_openfd_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler used to allocate or free space occupied by the resource
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Space message (how much to free and from where to start)
     * @param ocb    Control block for the calling client
     * @return Number of bytes
     */
    static int ioSpace(resmgr_context_t *ctp, io_space_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Handler used to allocate or free space occupied by the resource.
     * Called by ioSpace
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Space message (how much to free and from where to start)
     * @param ioOcb  Control block for the calling client
     * @return Number of bytes
     * @see ResManagerBase::ioSpace
     */
    virtual int IoSpace(resmgr_context_t *ctp, io_space_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler to allow the process manager to mmap() files from your
     * resource manager
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Mmap message (protection, size, offset, etc)
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioMmap(resmgr_context_t *ctp, io_mmap_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Handler to allow the process manager to mmap() files from your
     * resource manager. Called by ioMmap
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Mmap message (protection, size, offset, etc)
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManagerBase::ioMmap
     */
    virtual int IoMmap(resmgr_context_t *ctp, io_mmap_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler for custom resource manager messages (more general variation of
     * devctl() handler, but less portable)
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Custom message
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioMsg(resmgr_context_t *ctp, io_msg_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Handler for custom resource manager messages (more general variation of
     * devctl() handler, but less portable). Called by ioMsg
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Custom message
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManagerBase::ioMsg
     */
    virtual int IoMsg(resmgr_context_t *ctp, io_msg_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler for a flush command
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Sync message (what to flush to disk)
     * @param ocb    Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     */
    static int ioSync(resmgr_context_t *ctp, io_sync_t *msg, RESMGR_OCB_T *ocb);

    /**
     * Handler for a flush command. Called by ioSync
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Sync message (what to flush to disk)
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManagerBase::ioSync
     */
    virtual int IoSync(resmgr_context_t *ctp, io_sync_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Routine invoked by the resource manager library in response to an
     * attached pulse.
     *
     * @param ctp    Dispatch context pointer (from dispatch_context_alloc)
     * @param code   Code for the pulse we received
     * @param flags  ????? (no documentation in API)
     * @param handle Pointer to a ResManager object
     * @return 0 if successful; -1 if an error occurred
     */
    static int PulseHandler( message_context_t *ctp, int code, unsigned flags, void *handle);

    /**
     * Function invoked inresponse to a signal to our process
     *
     * @param SigNo  Siagnal we caught
     */
    static void SignalHandler( int SigNo);

    /**
     * Helper function to read a client's IoWrite buffer
     *
     * @param ctp    Resource Manager context block
     * @param buff   Buffer to read the data into
     * @param len    Length of the buff parameter
     * @return Number of bytes read if successful
     *         -1 if error occurs
     */
    int ReadSendersBuffer( resmgr_context_t *ctp, uint8_t* buff, size_t len);

    /**
     * Pulse code we have attached to our resource manager's I/O message channel
     */
    int                     m_ioChnlPulseCode;

    /**
     * Connection created in order for our resource manager to receive
     * generic messages and pulses on his I/O message channel
     */
    int                     m_mssgCoid;

    /**
     * Demo mode flagI/O Handler methods
     */
    bool                    m_demoMode;

    /**
     * Structure to allow us to patch in our own ocb allocating & freeing functions
     */
    iofunc_funcs_t          m_ocbFuncs;

    /**
     * The mount structure used to patch in our OCB alloc/free functions
     */
    iofunc_mount_t          m_mountPoint;

    /**
     * Dispatch interface handle (used by the kernel)
     */
    dispatch_t              *m_dispatchPtr;

    /**
     * Attributes for our manager
     */
    resmgr_attr_t           m_resmgrAttr;

    /**
     * Context pointer for our manager (message info, etc)
     */
    resmgr_context_t        *m_ctp;

    /**
     * Holder of the ID for the path we adopt
     */
    int                     m_resMgrId;

    /**
     * Connection handler pointers (to handle client connections)
     */
    resmgr_connect_funcs_t  m_connectFuncs;

    /**
     * I/O Handlre pointers (to handle client I/O requests)
     */
    resmgr_io_funcs_t       m_ioFuncs;

    /**
     * Function attributes for I/O functions (offset, file size, etc)
     */
    IOFUNC_ATTR_T           m_ioFuncAttr;

    /**
     * List of pulses attached to our I/O channel
     */
    vector< int>            m_attachedPulses;

    /**
     * Pointer needed to initialize ResManagerBase* inside the OCB
     */
    static ResManagerBase*  m_ioMgrForOcb;

    /**
     * Current run status for the resource manager. Used to re-start the
     * resource manager when a Terminate or Reload pulse is received
     */
    static volatile ResManagerCmd_t m_resMgrStatus;

private:

};


#endif  // _RES_MANAGER_BASE_H_INCLUDED

