//******************************************************************************
// Description:
//  Logical serial port driver for CAN communication
//
//==============================================================================
//
//     Copyright © 2003 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/AVT/CAN/AvtCanProxy.h $
// 
// 7     10/30/07 3:56p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 6     10/26/06 10:16a Rwiersem
// Changes for the 06102701 core release:
// 
// Added m_rxObjectMap.
// 
// 5     6/21/06 4:39p Cward
// Added ability to reload config file without rebooting
//
// 1     6/19/06 3:51p Rwiersem
// Made the test class a friend.
//
// 4     7/20/05 9:07p Cward
// Added support for short form transmit messages
//
// 3     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     2/18/05 12:43p Bmeinke
// Removed the global AVT timeout timer (not really used)
// Made the timeout value in WaitForTranslatorAck() an argument (defaults
// to 150ms)
//
// 2     1/17/05 10:04a Bmeinke
// Changed the CanModuleEntryList class to use a std::map<ModuleId_t,
// CanModuleEntry_t*> instead of a C-style array of CanModuleEntry_t
// structs
//
// 2     8/18/04 11:26p Cward
// Removed duplicate definitions and created new common class. Also
// removed unused cond var.
//
// 2     7/07/04 4:58p Bmeinke
// Replaced UpdateClientFifos() with CanAddToClientFifo()
//
// 1     12/21/03 6:28p Cward
//
// 2     6/26/03 1:25p Khykin
// Updated with Brians latest changes.
//
// 3     6/24/03 10:19a Bmeinke
// Moved the define of READER_PROCESSES from RawCommProxy.h to the header
// files for the individual comm proxies
//
// 1     6/24/03 10:12a Bmeinke
//
// 2     6/24/03 9:52a Bmeinke
// Iniaial debug on  Ford Dearbord.
// Chanegd many uint8_t parameters to ModuleId_t
// Added ResetConnectionHandler() to clear the dataReadt' flag for a
// client's module
// Added method  MakeReplyIdFromCmdId() to calculate a module ID based on
// a string being sent to the vehicle
//
// 1     6/20/03 9:18p Bmeinke
//
// 1     6/20/03 8:46a Khykin
// Initial checkin.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _AVT_CAN_PROXY_H_INCLUDED
#define _AVT_CAN_PROXY_H_INCLUDED

// Forward class references
class AvtCanProxy;

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

// Define this here so it does not get defined in the neutrino headers

#ifndef IOFUNC_OCB_T
struct _CanIoOcb_t;
#define IOFUNC_OCB_T            struct _CanIoOcb_t
#endif

#ifndef RESMGR_OCB_T
struct _CanIoOcb_t;
#define RESMGR_OCB_T            struct _CanIoOcb_t
#endif

/**
 * Flag used to indicate whether or not the reader thread will process all received data.
 * If set to 0, the reader thread will simply stick received data into our rx fifo and
 * send a pulse back to our I/O channel to indicate that new rx data needs to be processed
 */
#define READER_PROCESSES        0

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#include "RawCommProxy.h"   // Base class

#include "CanComDefs.h"             // Data definitions
#include "AvtFilterByte.h"          // AVT Filter byte abstraction
#include "AvtTimeoutException.h"
#include <vector>
#include <map>

/**
 * Name to register the CAN port driver process under
 */
#define AVT_CAN_PROC_NAME           "AvtCanProxy"

/**
 * Path we will adopt under the /dev/CommProxy dir
 */
#define AVT_CAN_DEV_PATH            "AvtCanProxy"

/**
 * OCB Structure extended for CAN client conenctions
 */
typedef struct _CanIoOcb_t
{
    /**
     * OCB used (and expected) by the base class I/O routines.
     * !!!!! MUST BE FIRST !!!!!
     */
    CommIoOcb_t                 proxyOcb;

    /**
     * Module ID this client last sent data to
     */
    ModuleId_t                  clientModuleId;

    /**
     * List of filter bytes this client had specified
     */
    AvtFltrByteList             filterByteList;

} CanIoOcb_t;

/**
 * Type used to abstract condvar for CAN read/write flags
 */
typedef BepCondVar< bool>               CanCondVar;

/**
 * Type used to abstract condvar for serial strings
 */
typedef BepCondVar< SerialString_t>     SerialComCondVar;

/**
 * Attributes to be associated with a CAN module in the vehicle
 */
typedef struct
{
    /**
     * OCB of the calling client
     */
    CanIoOcb_t  *clientOcb;

    /**
     * Cond var used to indicate OK to send to this module
     */
    CanCondVar  canSend;

    /**
     * Cond var used to indicate data has been received from this module
     * or that the module did not reply within the specified timeout
     */
    CanCondVar  dataReady;

    /**
     * Timer used to detect response timeouts
     */
    CommGapTimer    replyTimeoutTimer;

    /**
     * Timer used to control how frequently consecutive messages can be sent
     * to a module
     */
    CommGapTimer    mssgGapTimer;

} CanModuleEntry_t;

/**
 * List used to map a module ID to a client OCB. The OCB gets associated with a module
 * ID when a client sends a diagnostic command to a module
 */
class CanModuleEntryList
{
public:
    /**
     * Default constructor
     */
    CanModuleEntryList()
    {
        m_moduleList.clear();
    }

    /**
     * Destructor
     */
    ~CanModuleEntryList()
    {
        m_moduleList.clear();

    }

    /**
     * Lock the list for exclusive access
     *
     * @return EOK if successfully locked, any other value is an errno-type error code
     */
    inline int Acquire()
    {
        return( m_listLock.Acquire());
    }

    /**
     * Release an exclusive lock on the list
     *
     * @return EOK if successfully locked, any other value is an errno-type error code
     */
    inline int Release()
    {
        return( m_listLock.Release());
    }

    /**
     * Array index operator
     *
     * @param idx    Index of the module entry to be returned
     * @return Reference to the module entry at the given index
     */
    CanModuleEntry_t* operator[]( uint16_t idx)
    {
        CanModuleEntryListItr_t itr;

        itr = m_moduleList.find( idx);
        if( m_moduleList.end() == itr)
        {
            CanModuleEntry_t        *newEntry = new CanModuleEntry_t;
            newEntry->clientOcb = NULL;
            newEntry->dataReady.Acquire();  newEntry->dataReady.Signal( false); newEntry->dataReady.Release();
            newEntry->canSend.Acquire();    newEntry->canSend.Signal( true);    newEntry->canSend.Release();
            itr = m_moduleList.insert( m_moduleList.begin(), CanModuleEntryListVal_t(idx, newEntry));
        }
        return( itr->second);
    }

private:
    // Define the data type we will use to hold the module entries
    typedef std::map< ModuleId_t, CanModuleEntry_t*>    CanModuleEntryList_t;
    typedef CanModuleEntryList_t::iterator          CanModuleEntryListItr_t;
    typedef CanModuleEntryList_t::const_iterator        CanModuleEntryListCItr_t;
    typedef CanModuleEntryList_t::value_type            CanModuleEntryListVal_t;

    /**
     * Mutex used to guard access to the module list
     */
    BepMutex            m_listLock;

    /**
     * List of module objects
     */
//  CanModuleEntry_t    *m_moduleList;//[ 0xFFFF];
    CanModuleEntryList_t    m_moduleList;
};

/**
 * CAN Logical port driver class. Logical ports are used to abstract
 * away the underlying physical communication hardware. They can easily
 * be re-mapped to another physical communication port in the case of
 * hardware failure. There is a 1-to-1 relationship between a logical
 * port and a physical communication port.
 *
 * @author Ross Wiersema
 * @version 1
 * @see LogicalPortDrv AVTJ2190Proxy
 * @since Jan 30, 2002
 */
class AvtCanProxy : public RawCommProxy
{
    friend class AvtCanProxyTest;

    /**
     * Message format to use when sending data to the AVT-718
     * AVT718_LONG_FORM,
     * AVT718_SHORT_FORM
     */
    typedef enum
    {
        /**
         * Long form messages (don't add message length to client write requests)
         */
        AVT718_LONG_FORM,
        /**
         * Short form messages (add message length to client write requests)
         */
        AVT718_SHORT_FORM
    } Avt718MssgFormat_t;

public:
    /**
     * Default constructor
     */
    AvtCanProxy();

    /**
     * Constructor from command line arguments
     *
     * @param argc   Number of command line arguments to aprse
     * @param argv   Array of command line argument strings
     */
    AvtCanProxy( int argc, char *argv[]);

    /**
     * Class destructor
     */
    virtual ~AvtCanProxy();

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
     * Reads logical port configuration from an XML document
     *
     * @param portNode Node containing setup data for our port only
     */
    void ReadPortConfig( const XmlNode *portNode);

    /**
     * Reads the list of default filter bytes to upload to the AVT box
     *
     * @param portNode Node containing setup data for our port only
     */
    void ReadDefaultFilterBytes( const XmlNode *portNode);

    /**
     * Handles I/O mesages
     *
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t IoMssgLoop();

    /**
     * Method used to check if a serial response string can be added to a
     * client's rx FIFO
     *
     * @param data   Serial string reseived from the port
     * @param ocb    Client connection identifier
     * @return true if the data should be added to the client's FIFO, false otherwise
     */
    virtual bool CanAddToClientFifo(const SerialString_t &data, CommIoOcb_t *ocb);

    /**
     * Extracts a module ID from a vehicle response
     *
     * @param mssg    Response message received from the vehicle
     * @param msgLen Length of the response message in bytes
     *
     * @return The module ID from the vehicle response
     */
    virtual ModuleId_t GetModuleIdFromMessage( const uint8_t *mssg, size_t msgLen);

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
     * Wait for command ACK's from the bus translator
     *
     * @param ackCount Number of ACK responses we expect (default=1)
     * @param timeOut  The total amount of time (ms) to wait for all ACKs
     *                 to come back (default=150)
     *
     * @return EOK if we received the ACK, error code (e.g. ETIMEDOUT) otherwise
     */
    virtual int WaitForTranslatorAck( int ackCount=1, uint32_t timeOut=150);

    /**
     * Arms a reply timeout timer for the client designated by 'ocb'
     *
     * @param clientOcb The OCB of the client we are arming the reply timeout timer for
     * @param forceArm  Flag touse if we want to unconditionally arm the timer (deafults to false)
     * @return 0 if timer was armed, any other value if not armed
     */
    virtual int ArmReplyTimeoutTimer( CommIoOcb_t *clientOcb, bool forceArm=false);

    /**
     * Method called to read received data from the port
     *
     * @param buff   Buffer to put the received data into
     * @param buffSz Size (in bytes) of the buff parameter
     * @return The number of bytes received from the port
     */
    virtual int ReadPortDataUnlocked(uint8_t *buff, size_t buffSz);

    /**
     * Processes a "valid reply packet" reply from the AVT module
     *
     * @param rxBuff   Reply message buffer to process (containing the AVT message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the header byte of the AVT message)
     */
    virtual void ProcessValidPacket( SerialString_t &rxBuff, uint32_t buffSize);

    /**
     * Processes an "error message" reply from the AVT module
     *
     * @param rxBuff   Reply message buffer to process (containing the AVT message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the header byte of the AVT message)
     */
    virtual void ProcessErrorMesage(SerialString_t &rxBuff, uint32_t buffSize);

    /**
     * Processes a "command message error" reply from the AVT module
     *
     * @param rxBuff   Reply message buffer to process (containing the AVT message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the header byte of the AVT message)
     */
    virtual void ProcessCommandMessageError(SerialString_t &rxBuff, uint32_t buffSize);

    /**
     * Processes a "arbitration lost" reply from the AVT module
     *
     * @param rxBuff   Reply message buffer to process (containing the AVT message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the header byte of the AVT message)
     */
    virtual void ProcessArbitrationLost(SerialString_t &rxBuff, uint32_t buffSize);

    /**
     * Processes a reply from the AVT module which indicated an internal AVT error
     *
     * @param rxBuff   Reply message buffer to process (containing the AVT message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the header byte of the AVT message)
     */
    virtual void ProcessAvtHwError(SerialString_t &rxBuff, uint32_t buffSize);

    /**
     * Processes a reply from the AVT module which contains processor memory values
     *
     * @param rxBuff   Reply message buffer to process (containing the AVT message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the header byte of the AVT message)
     */
    virtual void ProcessProcesorMemoryMessage(SerialString_t &rxBuff, uint32_t buffSize);

    /**
     * Processes a reply from the AVT module which contains operational mode information
     *
     * @param rxBuff   Reply message buffer to process (containing the AVT message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the header byte of the AVT message)
     */
    virtual void ProcessOperationalReport(SerialString_t &rxBuff, uint32_t buffSize);

    /**
     * Processes a reply from the AVT module which contains board status information
     *
     * @param rxBuff   Reply message buffer to process (containing the AVT message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the header byte of the AVT message)
     */
    virtual void ProcessBoardStatusMessage(SerialString_t &rxBuff, uint32_t buffSize);

    /**
     * Checks a message buffer for the AVT ACK string. If found,
     * the ACK string is removed from the buffer
     *
     * @param msg    Message buffer to check for an AVT ACK
     * @return true if the buffer contained an expected AVT ACK string;
     *         false if no AVT ACK was expected, or if no ACK was found
     */
    bool CheckForAvtAck( SerialString_t &msg);

    /**
     * Processes a vehicle reply message
     *
     * @param msg    The buffer containing the vehicle reply message
     * @param size   The number of bytes in the vehicle reply message
     */
    bool ProcessVehicleMessage( const uint8_t *msg, const uint8_t size);

    /**
     * Checks if a message is a valid vehicle reply message
     *
     * @param msg    The buffer containing the vehicle reply message
     * @param size   The number of bytes in the vehicle reply message
     * @return true if the buffer appears to be a valid diagnostic reply message
     */
    bool IsDiagReplyMsg( const uint8_t *msg, const uint8_t size);

    /**
     * Handler used to perform any special processing required when a
     * blocked reader client requests to be unblocked (via TimerTimeout)
     *
     * @param ioOcb  Control block for the calling client
     * @param reader Reference to the blocked reader's attributes
     */
    virtual void ProcessReaderUnblockRequest(resMgrIoOcb_t *ioOcb, BlockedReader_t &reader);

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
     * Handler for AVT timeout error pulses
     *
     * @param pulse  Pulse received on the I/O channel
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t HandleAvtTimeoutPulse(io_pulse_t &pulse);

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
     * Handler method for client subscription requests
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int PortSubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                     resMgrIoOcb_t *ioOcb);

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
     * Method used to obtain a port lock for a client
     *
     * @param clientOcb  Client OCB containing flag indicating if he already owns the port lock
     * @param replyId If the port cannot be locked immediately, this is the connection ID
     *                to respond to when the port is locked for the calling client
     * @param block   Flag indicating if the port lock request should block if the port
     *                cannot be locked immediately.
     * @return EOK if port successfully locked, other values indicate error
     */
    virtual int ObtainClientPortLock(CommIoOcb_t *clientOcb, int replyId, bool block);

    /**
     * Method used to release a port lock for a client
     *
     * @param clientOcb Client OCB containing flag indicating if he already owns the port lock
     * @param releaseAll Flag to indicate if we should release all locks on the port help by
     *                   the client or just a single lock (set =0 vs. decrement by 1)
     * @return true if port lock released, false if unable to release port lock
     */
    virtual bool ReleaseClientPortLock(CommIoOcb_t *clientOcb, bool releaseAll=true);

    /**
     * Waits for the module with the given ID to be "send" ready
     *
     * @param moduleId ID of the module to wait for
     * @return true iof we can send to the module, false otherwise
     */
    bool WaitToSendToModule( ModuleId_t moduleId);

    /**
     * Checks to see if we have received data from a module in response
     * to a message we sent to it
     *
     * @param moduleId ID of the module to check the status of
     * @return true if the module has already replied, false if it has not
     */
    bool ModuleHasReplied( ModuleId_t moduleId) const;

    /**
     * Sets the okToSend falg for the given module ID
     *
     * @param moduleId Id of the module to set the okToSend flag for
     * @param value    New value for the canSend flag (default = true)
     */
    void SetOkToSend( const ModuleId_t& moduleId, bool value=true);

    /**
     * Signals any threads waiting to read data from this module that data
     * is available or a reply timeout occurred
     *
     * @param moduleId Id of the module which timed-out or from which we received data
     */
    void BroadcastModuleReply( const ModuleId_t &moduleId);

    /**
     * Sets the status of the dataReady flag for the module with the given module id
     *
     * @param moduleId
     *               Module ID whose dataReady flag we want to set
     * @param value  New status of the given module's dataReady flag (true or false)
     */
    void SignalModuleReply( const ModuleId_t moduleId, bool value);

    /**
     * Returns the status of the dataReady flag for the module with the given module id.
     *
     * @param moduleId
     *                 Module ID whose dataReady flag we want to set
     * @return true if the module with the given module ID has replied, false if it has not replied
     */
    bool IsModuleReplyReady( const ModuleId_t moduleId) const;

    /**
     * Formulates the node ID expected in a module reply string based on the node ID
     * we send out in our message to the vehicle
     *
     * @param avtTxObjCmd
     *               AVT Mesage object used to create the transmit object we will use to send data to
     *               the desired module/node
     * @param len    Number of bytes in the AVT command string
     *
     * @return INVALID_CAN_ID if the AVT command string is not long enough to contain a valid
     *         destination node ID. Any other value represents the expected reply node ID
     */
    virtual ModuleId_t MakeReplyIdFromCmdId( const uint8_t *avtTxObjCmd, size_t len);

    /**
     * Reset the AVT box and upload any filter bytes we are holding
     *
     * @return true if initialization was successful, false otherwise
     */
    bool AvtInit();

    /**
     * Performs a hardware restart on the AVT box
     */
    void AvtReset();

    /**
     * Tell the AVT to enter CAN communication mode.
     */
    void AvtCanMode();

    /**
     * Tell the AVT to use 2 wire CAN commnication mode.
     */
    void AvtTwoWireCan();

    /**
     * Tell the AVT to set the speed for CAN communications.
     */
    void AvtSetCanSpeed();

    /**
     * Setup the AVT receive object.
     */
    void SetupReceiveObject();

    /**
     * Setup the AVT transmit object.
     */
    void SetupTransmitObject();

    /**
     * Enable or disable a message object
     *
     * @param objNo  Message object number to be enabled (or disabled)
     * @param objType  What type of message object should this be:
     *                 AVT_CAN_RX_OBJECT or AVT_CAN_TX_OBJECT
     * @param enable Flag indicating if we want to enable (true) or disable (false) the
     *               message object
     */
    void EnableMessageObject( uint8_t objNo, AvtCanMessageObj_t objType, bool enable=true);

    /**
     * Configure a message object as a receive object or
     * a transmit object
     *
     * @param objNo    Message object to be configured
     * @param objType  What type of message object should this be:
     *                 AVT_CAN_RX_OBJECT or AVT_CAN_TX_OBJECT
     * @param dataLen  Message data length
     * @param moduleId Node ID for the module on the CAN bus
     */
    void ConfigureMessageObject( uint8_t objNo, AvtCanMessageObj_t objType,
                                 uint8_t dataLen, ModuleId_t moduleId);

    /**
     * Enable ISO-15765 (multi-frame) message support
     */
    void AvtMultiFrameMode();

    /**
     * Creates a multi-frame message object pairing
     *
     * @param rxObject Receive object to use when receiving responses to MFM messages
     *                 transmitted using TX object # txObject
     * @param txObject Message object used to transmit a MFM message
     */
    void MakeMFMObjectPair( uint8_t rxObject, uint8_t txObject);

    /**
     * Enable message padding for multi-frame messages
     */
    void AvtMultiFramePadding();

    /**
     * Clears all filter bytes from the AVT box
     */
    void ClearAvtFilterBytes();

    /**
     * Uploads a filter byte to the AVT box.
     *
     * @param filterByte The position of the filter in incoming vehicle messages
     * @param position   The value of the filter byte
     */
    void UploadFilterByte( uint8_t filterByte, uint8_t position);

    /**
     * Removes a filter byte from the AVT box's list
     *
     * @param filterByte Filter byte to be removed to the AVT box
     * @param position   Position of the filter byte to be removed
     */
    void DelAvtFilterByte( uint8_t filterByte, uint8_t position);

    /**
     * Sends a filter byte to the AVT box
     *
     * @param filterByte Filter byte to be uploaded to the AVT box
     * @param position   Position at which to add the filter byte
     */
    void SetAvtFilterByte( uint8_t filterByte, uint8_t position);

    /**
     * Restores any AVT filter bytes we are holding following a warm reset
     */
    void RestoreFilterBytes();

    /**
     * Clears any filter bytes we are holding/maintaining
     */
    void ResetFilterBytes();

    /**
     * Requests the firmware revision from the AVT box
     */
    void ReadAvtVersion();

    /**
     * Sends a message to and gets a reply back from the AVT interface box
     *
     * @param avtMssg Message to be sent to the AVT box
     * @param avtResponse
     *                Reply from the AVT box
     * @param timeOut Timeout (in milliseconds) for the reply to come back
     */
    void AvtMssg( const SerialString_t &avtMssg, SerialString_t &avtResponse, uint32_t timeOut);

    /**
     * Waits for a response message from the AVT box. The resonse message is put into
     * the avtResponse buffer. If chId == -1, GetAvtResponse() will not wait for the
     * "data ready" pulses to arrive, but will instead just read as much data as is in
     * the fifo
     *
     * @param avtResponse
     *               Buffer where the AVT response message string will be put
     * @return Number of bytes received from the AVT box
     */
    int GetAvtResponse( SerialString_t &avtResponse);

    /**
     * Searches for 'searchMssg' within the avtResponse string. This
     * function will parse the 'avtResponse' string packet-by-packet
     * instead of dimply doing a 'find' to avoid errnoeously finding
     * the desired 'searchMssg' within  part of another message
     * packet
     *
     * @param searchMssg The AVT message packet to find
     * @param mssgLen    The lenght of searchMssg in bytes
     * @param avtResponse
     *                   Buffer containing the AVT response message string
     *
     * @return The position of 'searchMssg' within 'avtResponse', or -1 if the
     *         packet cannot be located
     */
    int FindAvtPacket( const uint8_t *searchMssg, uint32_t mssgLen, const SerialString_t &avtResponse);

    /**
     * Stops all the com timers (reply timeout and message gap timers)
     */
    void StopComTimers();

private:

    /**
     * Flag to indicate how many more byte to expect from the AVT module
     */
    size_t                      m_avtBytesToCome;

    /**
     * Array of objects used to manage module send-locks and message
     * timeouts. The module ID will server as the index into the array.
     * When a message is sent, the mutex corresponding to the destination
     * module's ID will be locked. Once a reply is received or if the reply
     * timeouts, the mutex will be unlocked.
     */
    mutable CanModuleEntryList  m_moduleList;

    /**
     * Mutex used to gain exclusive access to the AVT translator
     */
    BepMutex                    m_translatorLock;

    /**
     * Flag to indicate if we are waiting for an ACK back from
     * the translator
     */
    bool                        m_waitingForAck;

    /**
     * String sent from theAVT translator when ACKing commands we send to it
     */
    const SerialString_t        m_translatorAckString;

    /**
     * Number of milliseconds to wait for the AVT to respond to our
     * messages with an ACK
     */
    const int                   m_translatorAckTimeout;

    /**
     * Object used to maintain a local list of filter bytes used for
     * filtering messages received from the vehicle. Filter bytes in
     * this list will 'come and go'
     */
    AvtFltrByteList             m_filterByteList;

    /**
     * Object used to maintain a local list of filter bytes used for
     * filtering messages received from the vehicle. The filter bytes in
     * this list are always active
     */
    AvtFltrByteList             m_defaultFilterByteList;

    /**
     * Channel on which pulses will be received when a thread is
     * expecting an ACK from the AVT box
     */
    int                         m_avtAckChid;

    /**
     * Connection to which pulses should be sent when a thread is
     * expecting an ACK from the AVT box
     */
    int                         m_avtAckCoid;

    /**
     * Mutex used to wait for an acknowledgement of a received AVT ACK
     * pulse
     */
    BepMutex                    m_avtAckLock;

    /**
     * Pointer to the client whose message is being acked. Used for
     * serial communication logging
     */
    CanIoOcb_t              *m_clientBeingAcked;

    /**
     * AVT Firmware version
     */
    uint8_t                     m_avtVersion;

    /**
     * Timer pulse value sent for AVT comm timeouts
     */
    const uint16_t              m_avtTimeoutPulseVal;

    /**
     * Object used to wait for/signal the reseption of an AVT ACK string
     */
    BepCondVar< uint8_t>        m_avtAckRxd;

    /**
     * Array used to map AVT transmit objects to module IDs
     */
    ModuleId_t                  m_txObjectMap[ 0x10];

    /**
     * Array used to map AVT receive objects to module IDs
     */
    ModuleId_t                  m_rxObjectMap[ 0x10];

    /**
     * Message format to use when transmitting data to the AVT-718
     */
    Avt718MssgFormat_t          m_avtMssgFormat;

    /**
     * Flag indicating if we want the AVT-718 to auto-pad messages
     * to the vehicle
     */
    bool                        m_enableMssgPadding;
};

#endif // _AVT_CAN_PROXY_H_INCLUDED
