//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/MultiCom/MultiComProxy.h $
// $Author: Rwiersem $
//   $Date: 10/30/07 3:58p $
//
// Description:
//  Logical serial port driver for CAN communication
//
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/MultiCom/MultiComProxy.h $
// 
// 2     10/30/07 3:58p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 1     5/11/05 9:39p Cward
// Initial Version
// 
// 1     4/26/05 10:02a Bmeinke
// 
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
// 
// 2     7/09/04 2:14p Bmeinke
// Removed leading path information from the #include "../RawCommProxy.h"
// line
// 
// 4     5/11/04 8:59p Bmeinke
// Removed the message sent condvar from the OCB structure and made it a
// class variable
// Added IsBroadcastCommand() method to check if a TX message is a
// broadcast message or not
// 
// 3     4/29/04 8:01p Bmeinke
// Added a txStatus member to the MultiComIoOcb_t structure. Used to tell
// a client that the MultiCom replied with an error in the "Message sent"
// packet. A client waiting for the MultiCom to say that it has sent a
// message on the bus MUST inspect this data member after being woken up
// from the wait to see if the MultiComm successfully sent the message or
// not.
// 
// 2     4/23/04 5:00p Bmeinke
// Added SCPNormalMode() method (called from MutliComInit())
// Added CRC processing methods
// 
// 1     4/12/04 1:21p Bmeinke
// Changing VSS repositories
// 
// 1     4/12/04 10:34a Bmeinke
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

#ifndef _MULTICOM_PROXY_H_INCLUDED
#define _MULTICOM_PROXY_H_INCLUDED

// Forward class references
class MultiComProxy;

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

// Define this here so it does not get defined in the neutrino headers

#ifndef IOFUNC_OCB_T
struct _CanIoOcb_t;
#define IOFUNC_OCB_T            struct _MultiComIoOcb_t
#endif

#ifndef RESMGR_OCB_T
struct _CanIoOcb_t;
#define RESMGR_OCB_T            struct _MultiComIoOcb_t
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
#include "MultiComException.h"
#include "MultiComDefs.h"

#include <vector>

/**
 * Name to register the CAN port driver process under
 */
#define MULTICOM_PROC_NAME          "MultiComProxy"

/**
 * Path we will adopt under the /dev/CommProxy dir
 */
#define MULTICOM_DEV_PATH           "MultiCom"

/**
 * OCB Structure extended for CAN client conenctions
 */
typedef struct _MultiComIoOcb_t
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
     * Copy of the last message this client sent
     */
    SerialString_t              lastSentMssg;

    /**
     * Object used to signal when the MultiCom has told us that
     * it has transmitted our string to the vehicle
     */
//  MultiComTxCondVar           messageSent;

    /**
     * Flag used to inform a sender of a transmit status error from the
     * MultiCom board. Because a sender will be blocked waiting for a 
     * condvar to be signalled, we set this flag to EIO and then signal
     * the condvar to unblock the sender. If no error occurred during
     * the transmit, we will set this flag to EOK.
     */
    int                     txStatus;
} MultiComIoOcb_t;

/**
 * Attributes to be associated with a MultiCom module in the vehicle
 */
typedef struct
{
    /**
     * OCB of the calling client
     */
    MultiComIoOcb_t *clientOcb;

    /**
     * Cond var used to indicate OK to send to this module
     */
    MultiComRxCondVar   canSend;

    /**
     * Cond var used to indicate data has been received from this module
     * or that the module did not reply within the specified timeout
     */
    MultiComRxCondVar   dataReady;

    /**
     * Timer used to detect response timeouts
     */
    CommGapTimer    replyTimeoutTimer;

    /**
     * Timer used to control how frequently consecutive messages can be sent
     * to a module
     */
    CommGapTimer    mssgGapTimer;

} MultiComMod_t;

/**
 * List used to map a module ID to a client OCB. The OCB gets associated with a module
 * ID when a client sends a diagnostic command to a module
 */
class MultiComModList
{
public:
    /**
     * Default constructor
     */
    MultiComModList()
    {
        MultiComMod_t   newEntry;
        int ii;
        
        for( ii=0; ii<0xFF; ii++)
        {
            m_moduleList[ ii].clientOcb = NULL;
            m_moduleList[ ii].dataReady.Acquire();  m_moduleList[ ii].dataReady.Signal( false); m_moduleList[ ii].dataReady.Release();
            m_moduleList[ ii].canSend.Acquire();    m_moduleList[ ii].canSend.Signal( true);    m_moduleList[ ii].canSend.Release();
        }
    }

    /**
     * Destructor
     */
    ~MultiComModList()
    {
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
     * 
     * @return Reference to the module entry at the given index
     */
    MultiComMod_t& operator[]( uint8_t idx)
    {
        return( m_moduleList[ idx]);
    }
    
private:

    /**
     * Mutex used to guard access to the module list
     */
    BepMutex            m_listLock;
    
    /**
     * List of module objects
     */
    MultiComMod_t   m_moduleList[ 0xFF];
};

/**
 * CommProxy used for MultiCom communication. Logical ports are used to abstract
 * away the underlying physical communication hardware. They can easily
 * be re-mapped to another physical communication port in the case of
 * hardware failure. There is a 1-to-1 relationship between a logical
 * port and a physical communication port.
 * 
 * @author Brian Meinke
 * @version 1
 * @see RawCommProxy
 * @since Feb 10, 2004
 */
class MultiComProxy : public RawCommProxy
{
public:
    /**
     * Default constructor
     */
    MultiComProxy();

    /**
     * Constructor from command line arguments
     * 
     * @param argc   Number of command line arguments to aprse
     * @param argv   Array of command line argument strings
     */
    MultiComProxy( int argc, char *argv[]);

    /**
     * Class destructor
     */
    virtual ~MultiComProxy();
    
    /**
     * Performs any necessary manager cleanup (free memory, detach any
     * adopted path names, etc)
     */
    virtual void CleanupManager();

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
     * Method used to check if a serial response string can be added to a
     * client's rx FIFO
     *
     * @param data   Serial string received from the port
     * @param ocb    Client connection identifier
     * @return true if the data should be added to the client's FIFO, false otherwise
     */
    virtual bool CanAddToClientFifo(const SerialString_t &data, CommIoOcb_t *ocb);

    /**
     * Extracts a module ID from a vehicle response.
     * 
     * @param mssg   Response message received from the vehicle
     * @param msgLen Size of the message (in bytes) to be processed
     * 
     * @return The module ID from the vehicle response
     * @exception BepException
     *                   No module ID can be extracted from the message
     */
    virtual ModuleId_t GetModuleIdFromMessage( const uint8_t *mssg, size_t msgLen) throw(BepException);

    /**
     * The purpose of this is to give us a place to allocate our own ocb. It is
     * called by ocbCalloc, which in turn is called as a result of an open being done
     * on our reource manager's adopted path(e.g. iofunc_open_default causes it to be
     * called). ocbCalloc was registered through our mount structure.
     * 
     * @param ctp    Resource manager's context block
     * @param device Attributes for this client connection (an OCB)
     * 
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
     * 
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
     * 
     * @return 0 if successful
     *         <0 if error occurs
     * @see ResManagerBase::ioWrite
     */
    virtual int IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Wait for command ACK's from the bus translator
     * 
     * @param ackCount Number of ACK responses we expect
     * 
     * @return EOK if we received the ACK, error code (e.g. ETIMEDOUT) otherwise
     */
    virtual int WaitForTranslatorAck( int ackCount=1);
        
    /**
     * Waits for the MultiCom board to tell us that our message
     * has been transmitted to the vehicle
     * 
     * @param clientOcb OCB of the client that sent the last message to the vehicle
     * 
     * @return EOK if the MultiCom responded with the Message Sent reply
     *         error code (e.g. ETIMEDOUT) otherwise
     */
    virtual int WaitForMessageSent( MultiComIoOcb_t *clientOcb);
    
    /**
     * Arms a reply timeout timer for the client designated by 'ocb'
     * 
     * @param clientOcb The OCB of the client we are arming the reply timeout timer for
     * @param forceArm  Flag touse if we want to unconditionally arm the timer (deafults to false)
     * 
     * @return 0 if timer was armed, any other value if not armed
     */
    virtual int ArmReplyTimeoutTimer( CommIoOcb_t *clientOcb, bool forceArm=false);

    /**
     * Method called to read received data from the port
     * 
     * @param buff   Buffer to put the received data into
     * @param buffSz Size (in bytes) of the buff parameter
     * 
     * @return The number of bytes received from the port
     */
    virtual int ReadPortDataUnlocked(uint8_t *buff, size_t buffSz);
    
    /**
     * Method called to extract the next full message packet
     * from a FIFO
     * 
     * @param fifo   FIFO to extract the packet from
     * @param packet The packet which was extracted from the fifo
     * 
     * @return The number of bytes in the extracted packet
     */
    virtual int ExtractNextPacket(CommFifo &fifo, SerialString_t &packet);
    
    /**
     * Processes a reply from the MultiCom module which contains
     * a "Send Message" ack packet
     * 
     * @param rxBuff   Reply message buffer to process (containing the message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the message header)
     */
    virtual void ProcessAckMessage(SerialString_t &rxBuff, uint32_t buffSize);
    
    /**
     * Processes a reply from the MultiCom module which contains 
     * an HBCC "message received" status packet
     * 
     * @param rxBuff   Reply message buffer to process (containing the message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the message header)
     */
    virtual void ProcessHBCCRxPacket(SerialString_t &rxBuff, uint32_t buffSize);
    
    /**
     * Processes a reply from the MultiCom module which contains 
     * an HBCC "message sent" status packet
     * 
     * @param rxBuff   Reply message buffer to process (containing the message data)
     * @param buffSize Number of bytes expected for the next full message (as given
     *                 by the message header)
     */
    virtual void ProcessHBCCTxPacket(SerialString_t &rxBuff, uint32_t buffSize);
    
    /**
     * Checks a message buffer for the MultiCom ACK string. If found,
     * the ACK string is removed from the buffer
     * 
     * @param msg    Message buffer to check for an MultiCom ACK
     * 
     * @return true if the buffer contained an expected MultiCom ACK string;
     *         false if no MultiCom ACK was expected, or if no ACK was found
     */
    bool CheckForAck( SerialString_t &msg);
    
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
     * 
     * @return true if the buffer appears to be a valid diagnostic reply message
     */
    bool IsDiagReplyMsg( const uint8_t *msg, const uint8_t size);
    
    /**
     * Handler for MSSG_GAP_TIMER pulses
     * 
     * @param pulse  Pulse received on the I/O channel
     * 
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t HandleMssgGapPulse(io_pulse_t &pulse);

    /**
     * Handler for MultiCom timeout error pulses
     * 
     * @param pulse  Pulse received on the I/O channel
     * 
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t HandleTimeoutPulse(io_pulse_t &pulse);

    /**
     * Handler for module reply timeout error pulses
     * 
     * @param pulse  Pulse received on the I/O channel
     * 
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t HandleReplyTimeoutPulse(io_pulse_t &pulse);
    
    /**
     * Client request to reset his connection to the port driver.
     * Essentially only resets the client's FIFO.
     * 
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * 
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
     * 
     * @return EOK if successful, other on error
     */
    virtual int ResetDriverHandler(resmgr_context_t *ctp, io_devctl_t *msg, 
                                   resMgrIoOcb_t *ioOcb);

    /**
     * Method used to obtain a port lock for a client
     * 
     * @param clientOcb Client OCB containing flag indicating if he already owns the port lock
     * @param replyId   If the port cannot be locked immediately, this is the connection ID
     *                  to respond to when the port is locked for the calling client
     * @param block     Flag indicating if the port lock request should block if the port
     *                  cannot be locked immediately.
     * 
     * @return EOK if port successfully locked, other values indicate error
     */
    virtual int ObtainClientPortLock(CommIoOcb_t *clientOcb, int replyId, bool block);

    /**
     * Method used to release a port lock for a client
     * 
     * @param clientOcb  Client OCB containing flag indicating if he already owns the port lock
     * @param releaseAll Flag to indicate if we should release all locks on the port help by
     *                   the client or just a single lock (set =0 vs. decrement by 1)
     * 
     * @return true if port lock released, false if unable to release port lock
     */
    virtual bool ReleaseClientPortLock(CommIoOcb_t *clientOcb, bool releaseAll=true);

    /**
     * Waits for the module with the given ID to be "send" ready
     * 
     * @param moduleId ID of the module to wait for
     * 
     * @return true iof we can send to the module, false otherwise
     */
    bool WaitToSendToModule( ModuleId_t moduleId);

    /**
     * Checks to see if we have received data from a module in response
     * to a message we sent to it
     * 
     * @param moduleId ID of the module to check the status of
     * 
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
     * Sets the "MessageSent" flag in the clientOcb to "pending" status
     * 
     * @param clientOcb OCB containing the message sent flag to arm
     */
    void ArmMessageSent( MultiComIoOcb_t *clientOcb);

    /**
     * Clears the "MessageSent" flag in the clientOcb
     * 
     * @param clientOcb OCB containing the message sent flag to clear
     */
    void ClearMessageSent( MultiComIoOcb_t *clientOcb);

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
     *                 Module ID whose dataReady flag we want to set
     * @param value    New status of the given module's dataReady flag (true or false)
     */
    void SignalModuleReply( const ModuleId_t moduleId, bool value);

    /**
     * Returns the status of the dataReady flag for the module with the given module id.
     * 
     * @param moduleId Module ID whose dataReady flag we want to set
     * 
     * @return true if the module with the given module ID has replied, false if it has not replied
     */
    bool IsModuleReplyReady( const ModuleId_t moduleId) const;

    /**
     * Reset the MultiCom box and upload any filter bytes we are holding
     * 
     * @return true if initialization was successful, false otherwise
     */
    bool MultiComInit();

    /**
     * Performs a hardware restart on the MultiCom box
     */
    void MultiComReset();

    /**
     * Turns on the MultiCom termination resistors
     */
    void MultiComTerminationOn();

    /**
     * Sets the Node Address register in the MultiCom HBCC
     */
    void SetHBCCNodeAddress();

    /**
     * Requests the 485 firmware revision from the MultiCom box
     */
    void Read485Version();

    /**
     * Requests the SCP firmware revision from the MultiCom box
     */
    void ReadSCPVersion();

    /**
     * Requests the SCP interface to go to normal mode (non-monitor mode)
     */
    void SCPNormalMode();
    
    /**
     * Sends a message to and gets a reply back from the MultiCom interface box
     * 
     * @param mssg     Message to be sent to the MultiCom box
     * @param response Reply from the MultiCom box
     * @param timeOut  Timeout (in milliseconds) for the reply to come back
     */
    void MultiComMssg( const SerialString_t &mssg, SerialString_t &response, uint32_t timeOut);

    /**
     * Waits for a response message from the MultiCom box. The resonse message is put into
     * the response buffer.
     * 
     * @param response Buffer where the MultiCom response message string will be put
     * 
     * @return Number of bytes received from the MultiCom box
     */
    int GetMultiComResponse( SerialString_t &response);

    /**
     * Searches for 'searchMssg' within the response string. This
     * function will parse the 'response' string packet-by-packet
     * instead of dimply doing a 'find' to avoid errnoeously finding
     * the desired 'searchMssg' within  part of another message
     * packet
     * 
     * @param searchMssg The MultiCom message packet to find
     * @param mssgLen    The lenght of searchMssg in bytes
     * @param response   Buffer containing the MultiCom response message string
     * 
     * @return The position of 'searchMssg' within 'response', or -1 if the
     *         packet cannot be located
     */
    int FindMssgPacket( const uint8_t *searchMssg, uint32_t mssgLen, const SerialString_t &response);

    /**
     * Stops all the com timers (reply timeout and message gap timers)
     */
    void StopComTimers();
    
    /**
     * Adds a CRC byte to the given multicom data string
     * 
     * @param mssg   Multicom data string to add the CRC to
     */
    virtual void AddCRC( SerialString_t &mssg);
    
    /**
     * Checks the CRC in a received multicom data string against a
     * calculated CRC value from the data string body
     * 
     * @param mssg   Multicom data string to validate the CRC value for
     * 
     * @return true if the CRC is correct, false if it is incorrect
     */
    virtual bool ValidateCRC( SerialString_t &mssg);
    
    /**
     * Calculates the Multicom CRC bytes for the given data array
     * 
     * @param mssg     Data to calculate the CRC for
     * 
     * @return Calculated CRC value
     */
    virtual uint8_t CalculateCRC( const SerialString_t &mssg);
    
    /**
     * Calculates the Multicom CRC bytes for the given data array
     * 
     * @param mssg     Data to calculate the CRC for
     * @param mssgSize Number of bytes from mssg to use for the CRC calculation
     * 
     * @return Calculated CRC value
     */
    virtual uint8_t CalculateCRC( const uint8_t *mssg, int mssgSize);

    /**
     * Checks to see if the given command string is a broadcast command
     * 
     * @param cmd The command string to check
     * 
     * @return true if the string is a broadcast command, false otherwise
     */
    virtual bool IsBroadcastCommand( const SerialString_t &cmd);
    
private:

    /**
     * Flag to indicate how many more byte to expect from the MultiCom module
     */
    size_t                      m_bytesToCome;

    /**
     * Timer used to reset MultiCom buffer if no response after certain 
     * amount of time
     */
    CommGapTimer                m_respTimeoutTimer;

    /**
     * Array of objects used to manage module send-locks and message 
     * timeouts. The module ID will server as the index into the array.
     * When a message is sent, the mutex corresponding to the destination
     * module's ID will be locked. Once a reply is received or if the reply
     * timeouts, the mutex will be unlocked.
     */
    mutable MultiComModList     m_moduleList;

    /**
     * Mutex used to gain exclusive access to the MultiCom translator
     */
    BepMutex                    m_translatorLock;

    /**
     * Flag to indicate if we are waiting for an ACK back from
     * the translator 
     */
    bool                        m_waitingForAck;

    /**
     * String sent from the MultiCom translator when ACKing commands we send to it
     */
    const SerialString_t        m_translatorAckString;

    /**
     * Number of milliseconds to wait for the MultiCom to respond to our 
     * messages with an ACK
     */
    const int                   m_translatorAckTimeout;

    /**
     * Channel on which pulses will be received when a thread is 
     * expecting an ACK from the MultiCom box
     */
    int                         m_ackChid;
    
    /**
     * Connection to which pulses should be sent when a thread is 
     * expecting an ACK from the MultiCom box
     */
    int                         m_ackCoid;

    /**
     * Mutex used to wait for an acknowledgement of a received MultiCom ACK 
     * pulse
     */
    BepMutex                    m_ackLock;

    /**
     * Pointer to the client whose message is being acked. Used for 
     * serial communication logging
     */
    MultiComIoOcb_t             *m_clientBeingAcked;

    /**
     * MultiCom 485 Firmware version
     */
    SerialString_t              m_485Version;

    /**
     * MultiCom SCP Firmware version
     */
    SerialString_t              m_scpVersion;

    /**
     * Timer pulse value sent for MultiCom comm timeouts
     */
    const uint16_t              m_timeoutPulseVal;
    
    /**
     * Object used to wait for/signal the reception of an MultiCom ACK string
     */
    BepCondVar< int>            m_ackRxd;
    
    /**
     * CRC lookup table
     */
    static uint8_t              m_crcTable[ 256];
    
    /**
     * Flag used to signal that our message has been sent to the vehicle
     */
    MultiComTxCondVar               m_messageSent;

};

#endif // _MULTICOM_PROXY_H_INCLUDED
