//******************************************************************************
// Description:
//	Logical serial port driver for J2190 communication
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
//     $Log: /Core/ServicesLayer/Managers/CommProxy/AVT/J2190/AvtJ2190Proxy.h $
// 
// 5     8/18/04 11:29p Cward
// removed duplicate class defintion and moved to common dir. Included new
// class header. Removed unused cond var
//
// 4     6/24/04 1:07a Cward
// Removed pound if 0 block. Updated header block comments to the new
// standard.
//
// 3     3/04/04 5:15p Cward
// Replaced Update Client Fifos method with Can Add To Client Fifo method
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:28p Cward
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 1     6/12/03 4:05p Khykin
// Initial checkin.
//
// 4     6/05/03 7:32a Khykin
// Added new reader thread to the comm proxy.
//
// 3     5/29/03 1:40p Bmeinke
// New dedicated reader thread architecture
// Added J2190ModuleEntryList class: used to maintain a mapping of module
// ID to J2190ModuleEntry_t structs
// Changed m_moduleList from a plain old C-style array to a
// J2190ModuleEntryList object
// Added ObtainClientPortLock()/ReleaseClientPortLock() methods to more
// easily track which clients/modules are locjing the port
//
// 1     4/30/03 4:42p Bmeinke
// Migration to new personalized configuration management structure
//
// 9     4/30/03 3:11p Bmeinke
// Removed the m_testLock mutex (was for debugging only)
//
// 8     4/30/03 3:09p Bmeinke
// Changed the comm timers from J2190ComTimer objects to CommGapTimers
// Removed the J2190ComTimer class
// Added ArmReplyTimeoutTimer() method (overloaded from RawCommProxy)
// Added cond var m_avtAckRxd; used to wait for/signal the reception of an
// AVT ACK string
//
// 7     3/06/03 10:45p Bmeinke
// Added overloaded Register() method (called when we receive a
// REGISTER_PULSE)
//
// 6     3/06/03 12:42a Bmeinke
// Removed the OverrideSerialProcessing() and RestoreSerialProcessing()
// methods
// When sending control messages to the AVT box, use the ReadDriverData()
// method instead of using a custom channel and ionotify()
// Removed m_avtInitDataEv data member (no longer used)
//
// 5     1/27/03 10:28p Bmeinke
// Override ReadPortConfig() and added
// ReadDefaultFilterBytes()/m_defaultFilterByteList to read the list of
// default filter bytes to send to the AVT box (temporary work around for
// now)
// Added OverrideSerialProcessing() and RestoreSerialProcessing() to
// temporarily take over the serial data processing path while processing
// AVT initialization commands and restore the normal serial data
// processing when finished
//
// 4     1/22/03 3:25a Bmeinke
// Removed GetRegisteredName() (base class can handle it)
//
// 3     1/20/03 10:52p Bmeinke
// Modified so as to inherit from new RawCommProxy base class
//
// 2     1/18/03 1:46p Bmeinke
// Initial testing complete using AVT/Skim simulator
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _AVT_J2190_PROXY_H_INCLUDED
#define _AVT_J2190_PROXY_H_INCLUDED

// Forward class references
class AvtJ2190Proxy;

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

// Define this here so it does not get defined in the neutrino headers

#ifndef IOFUNC_OCB_T
struct _J2190IoOcb_t;
#define IOFUNC_OCB_T			struct _J2190IoOcb_t
#endif

#ifndef RESMGR_OCB_T
struct _J2190IoOcb_t;
#define RESMGR_OCB_T			struct _J2190IoOcb_t
#endif

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "RawCommProxy.h"	// Base class

#include "J2190ComDefs.h"	   		// Data definitions
#include "AVTCommDefs.h"			// AVT Data definitions
#include "AvtFilterByte.h"			// AVT Filter byte abstraction
#include "AvtTimeoutException.h"
#include <vector>

/**
 * Name to register the J2190 port driver process under
 */
#define AVT_J2190_PROC_NAME			"AvtJ2190Proxy"

/**
 * Path we will adopt under the /dev/CommProxy dir
 */
#define AVT_J2190_DEV_PATH			"AvtJ2190Proxy"

/**
 * OCB Structure extended for J2190 client conenctions
 */
typedef struct _J2190IoOcb_t
{
	/**
	 * OCB used (and expected) by the base class I/O routines.
	 * !!!!! MUST BE FIRST !!!!!
	 */
	CommIoOcb_t					proxyOcb;

	/**
	 * Module ID this client last sent data to
	 */
	ModuleId_t					clientModuleId;
	
	/**
	 * List of filter bytes this client had specified
	 */
	AvtFltrByteList				filterByteList;

} J2190IoOcb_t;


/**
 * Type used to abstract condvar for J2190 read/write flags
 */
typedef BepCondVar< bool>				J2190CondVar;


/**
 * Attributes to be associated with a J2190 module in the vehicle
 */
typedef struct
{
	/**
	 * OCB of the calling client
	 */
	J2190IoOcb_t	*clientOcb;

	/**
	 * Cond var used to indicate OK to send to this module
	 */
	J2190CondVar	canSend;

	/**
	 * Cond var used to indicate data has been received from this module
	 * or that the module did not reply within the specified timeout
	 */
	J2190CondVar	dataReady;

	/**
	 * Timer used to detect response timeouts
	 */
	CommGapTimer	replyTimeoutTimer;

	/**
	 * Timer used to control how frequently consecutive messages can be sent
	 * to a module
	 */
	CommGapTimer	mssgGapTimer;

} J2190ModuleEntry_t;

/**
 * List used to map a module ID to a client OCB. The OCB gets associated with a module
 * ID when a client sends a diagnostic command to a module
 */
class J2190ModuleEntryList
{
public:
	/**
	 * Default constructor
	 */
	J2190ModuleEntryList()
	{
		J2190ModuleEntry_t	newEntry;
		int ii;
		
		for( ii=0; ii<0xFF; ii++)
		{
			m_moduleList[ ii].clientOcb = NULL;
			m_moduleList[ ii].dataReady.Acquire();	m_moduleList[ ii].dataReady.Signal( false);	m_moduleList[ ii].dataReady.Release();
			m_moduleList[ ii].canSend.Acquire();	m_moduleList[ ii].canSend.Signal( false);	m_moduleList[ ii].canSend.Release();
		}
	}

	/**
	 * Destructor
	 */
	~J2190ModuleEntryList()
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
	 * @return Reference to the module entry at the given index
	 */
	J2190ModuleEntry_t& operator[]( uint8_t idx)
	{
		return( m_moduleList[ idx]);
	}
	
private:

	/**
	 * Mutex used to guard access to the module list
	 */
	BepMutex			m_listLock;
	
	/**
	 * List of module objects
	 */
	J2190ModuleEntry_t	m_moduleList[ 0xFF];
};

/**
 * J2190 Logical port driver class. Logical ports are used to abstract
 * away the underlying physical communication hardware. They can easily
 * be re-mapped to another physical communication port in the case of
 * hardware failure. There is a 1-to-1 relationship between a logical
 * port and a physical communication port.
 *
 * @author Brian Meinke
 * @version 1
 * @see LogicalPortDrv
 * @since Jan 30, 2002
 */
class AvtJ2190Proxy : public RawCommProxy
{
public:
	/**
	 * Default constructor
	 */
	AvtJ2190Proxy();

	/**
	 * Constructor from command line arguments
	 *
	 * @param argc   Number of command line arguments to aprse
	 * @param argv   Array of command line argument strings
	 */
	AvtJ2190Proxy( int argc, char *argv[]);

	/**
	 * Class destructor
	 */
	virtual ~AvtJ2190Proxy();

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
	 * @return EOK if we received the ACK, error code (e.g. ETIMEDOUT) otherwise
	 */
	int WaitForTranslatorAck();
		
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
	 * Processes a match function table status message from the AVT box
	 *
	 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
	 * @param buffSize Number of bytes expected for the next full message (as given
	 *                 by the header byte of the AVT message)
	 */
	virtual void ProcessMatchFunctionMessage(SerialString_t &rxBuff, uint32_t buffSize);

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
	 * Processes a reply from the AVT module which contains board status information
	 *
	 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
	 * @param buffSize Number of bytes expected for the next full message (as given
	 *                 by the header byte of the AVT message)
	 */
	virtual void ProcessBoardStatusMessage(SerialString_t &rxBuff, uint32_t buffSize);

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
	 * Checks if a message is a broadcast message on the J1850 bus
	 *
	 * @param msg    The buffer containing the message data
	 * @param size   The number of bytes in the message data buffer
	 * @return true if the buffer appears to be a bus broadcast message
	 */
	bool IsBusBroadcastMsg( const uint8_t *msg, const uint8_t size);
	
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
	 * Extracts the next full J2190 message packet from a client's fifo
	 *
	 * @param ocb    OCB of the client
	 * @param mssg   The next full packet contained in the client's fifo
	 *
	 * @return Number of bytes read from the FIFO or -(errno) if an error occurred
	 */
	int GetNextClientPacket( J2190IoOcb_t *ocb, SerialString_t &mssg);

	/**
	 * Waits for the module with the given ID to be "send" ready
	 *
	 * @param moduleId ID of the module to wait for
	 * @return true iof we can send to the module, false otherwise
	 */
	bool WaitToSendToModule( uint8_t moduleId);

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
	void SignalModuleReply( const uint8_t moduleId, bool value);

	/**
	 * Returns the status of the dataReady flag for the module with the given module id.
	 *
	 * @param moduleId
	 *                 Module ID whose dataReady flag we want to set
	 * @return true if the module with the given module ID has replied, false if it has not replied
	 */
	bool IsModuleReplyReady( const uint8_t moduleId) const;

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
 	* Connects the AVT box to the VPW J1850 Bus Network
	 */
	void ConnectVPW();

	/**
	 * Forces the AVT box into VPW mode
	 */
	void SetAvtVPW();

	/**
	 * Disconnects the AVT box from the VPW J1850 Bus Network
	 */
	void DisconnectVPW();

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
	void StopComTimers()
	{
		int ii;
		m_avtTimeoutTimer.Stop();
		for( ii=0; ii<0x100; ii++)
		{
			if( m_moduleList[ ii].replyTimeoutTimer.IsTimerArmed())	
			{
				m_moduleList[ ii].replyTimeoutTimer.Stop();
			}

			if( m_moduleList[ ii].mssgGapTimer.IsTimerArmed())	
			{
				m_moduleList[ ii].mssgGapTimer.Stop();
			}
		}
	}


private:

	/**
	 * Flag to indicate how many more byte to expect from the AVT module
	 */
	size_t						m_avtBytesToCome;

	/**
	 * Timer used to reset AVT buffer if no response after certain
	 * amount of time
	 */
	CommGapTimer				m_avtTimeoutTimer;

	/**
	 * Array of objects used to manage module send-locks and message
	 * timeouts. The module ID will server as the index into the array.
	 * When a message is sent, the mutex corresponding to the destination
	 * module's ID will be locked. Once a reply is received or if the reply
	 * timeouts, the mutex will be unlocked.
	 */
	mutable J2190ModuleEntryList	m_moduleList;

	/**
	 * Mutex used to gain exclusive access to the AVT translator
	 */
	BepMutex					m_translatorLock;

	/**
	 * Flag to indicate if we are waiting for an ACK back from
	 * the translator
	 */
	bool						m_waitingForAck;

	/**
	 * String sent from theAVT translator when ACKing commands we send to it
	 */
	const SerialString_t		m_translatorAckString;

	/**
	 * Number of milliseconds to wait for the AVT to respond to our
	 * messages with an ACK
	 */
	const int					m_translatorAckTimeout;

	/**
	 * Object used to maintain a local list of filter bytes used for
	 * filtering messages received from the vehicle. Filter bytes in
	 * this list will 'come and go'
	 */
	AvtFltrByteList				m_filterByteList;

	/**
	 * Object used to maintain a local list of filter bytes used for
	 * filtering messages received from the vehicle. The filter bytes in
	 * this list are always active
	 */
	AvtFltrByteList				m_defaultFilterByteList;

	/**
	 * Channel on which pulses will be received when a thread is
	 * expecting an ACK from the AVT box
	 */
	int							m_avtAckChid;
	
	/**
	 * Connection to which pulses should be sent when a thread is
	 * expecting an ACK from the AVT box
	 */
	int							m_avtAckCoid;

	/**
	 * Mutex used to wait for an acknowledgement of a received AVT ACK
	 * pulse
	 */
	BepMutex					m_avtAckLock;

	/**
	 * Pointer to the client whose message is being acked. Used for
	 * serial communication logging
	 */
	J2190IoOcb_t				*m_clientBeingAcked;

	/**
	 * AVT Firmware version
	 */
	uint8_t						m_avtVersion;

	/**
	 * Timer pulse value sent for AVT comm timeouts
	 */
	const uint16_t				m_avtTimeoutPulseVal;
	
	/**
	 * Object used to wait for/signal the reseption of an AVT ACK string
	 */
	BepCondVar< bool>			m_avtAckRxd;
};

#endif // _AVT_J2190_PROXY_H_INCLUDED
