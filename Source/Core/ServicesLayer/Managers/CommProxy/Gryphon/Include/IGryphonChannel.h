//******************************************************************************
// Description:
//  Logical port driver for communication on Gryphon
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
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Managers/CommProxy/Gryphon/Include/IGryphonChannel.h $
// 
// 1     11/02/07 12:55p Derickso
// Updated to support UDS protocol.
// Added functions (but did not make active) for only transmitting one
// message at a time to each module.
// 
// 8     11/11/05 4:56p Cward
// Changes for 0511101 iteration
// Corrected comments at OpenPort(), Connect(), and MySocket.
//
// 7     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 6     6/24/04 1:10a Cward
// Removed pound if 0 blocks. Updated the header block comments to the new
// standard.
//
// 5     5/25/04 4:20p Cward
// Added Reset Driver Handler method declaration
//
// 4     3/04/04 5:15p Cward
// Replaced Update Client Fifos method with Can Add To Client Fifo method
//
// 3     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:29p Cward
//
// 3     11/13/03 6:02p Derickso
// Updates to register gryphon usdt server to allow larger range of module
// IDs.
//
// 2     10/07/03 1:05p Derickso
// Latest Gryphon Comm proxy.
//
// 1     9/24/03 6:45a Derickso
// Gryphon Comm Proxy files.
//
// 5     7/03/03 7:24a Gswope
//
// 4     7/01/03 11:25a Gswope
//
// 1     5/08/03 8:41a Gswope
// tested on CAMI machines with vehicle
//
// 3     5/05/03 8:22a Gswope
// placing in stasis
//
// 2     4/22/03 11:25a Gswope
//
// 1     4/22/03 10:38a Gswope
// First Entry
//
// 6     4/15/03 8:30a Gswope
//
// 5     4/07/03 3:14p Gswope
//
// 4     4/05/03 10:22a Gswope
// Daily Update -- class 2 working
//
// 3     4/01/03 8:45a Gswope
//
// 2     3/25/03 12:58p Gswope
//
// 1     3/17/03 4:30p Gswope
// Initial entry into source safe
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef IGryphonChannel_h
#define IGryphonChannel_h
//==============================================================================

#include <stdint.h>
#include "gryphon.h"    // loads all DG supplied .h files
#include "gryphon2.h"   // additional defines
#include "gmsg2.h"      // additional message structure definitions
#include "RawCommProxy.h"       // Base class
#include <list>
#include <algorithm>

#define IGCDEBUG 0

/**
 * OCB definition for communications with the Gryphon box.
 * @since Version 2.0
 */
typedef struct _GryphonIoOcb_t
{
    /**
     * OCB used (and expected) by the base class I/O routines.
     * !!!!! MUST BE FIRST !!!!!
     */
    CommIoOcb_t     proxyOcb;
    /**
     * Module ID this client last sent data to
     */
    vector<uint32_t>        moduleIDs;
    /**
     * response that is expected by this client.
     *
     * in class 2, this is the response code that we are expecting, based on the
     * request that we sent.  only messages that contain this response code or a
     * NAK ($7F) are returned to the client
     */
    uint16_t        expectedResponse;

} GryphonIoOcb_t;

/**
 * Interface class for the Gryphon Server.
 * This serves as a base class for communicating with the
 * GRyphon server and should be overloaded for the different
 * buses the Gryphon can support.
 *
 * @author Gene Swope
 * @author Dan Erickson
 * @version Version 2.0
 * @see RawCommProxy
 * @since April 2003
 */
class IGryphonChannel : public RawCommProxy
{
public:
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
    IOFUNC_OCB_T* OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device);
    /**
     * Method used to initialize an allocated OCB structure. Called
     * immediately after the OCB is allocated
     *
     * @param ioOcb  OCB structure to be initialized
     */
    void OcbInit(resMgrIoOcb_t *ioOcb);
    /**
     * The purpose of this is to give us a place to free our ocb. It is called
     * by ResManagerBase::ocbFree, which in turn is called as a result of the close
     * being done (e.g. iofunc_close_ocb_default causes it to be called).
     * ResManagerBase::ocbFree was registered through our mount structure.
     *
     * @param ocb    Client's Open Context Block for this connection
     *
     * @see ResManagerBase::ocbFree
     */
    void OcbFree(resMgrIoOcb_t *ocb);
    /**
     * Method used to finalize an allocated OCB structure. Called immediately
     * before the OCB is destroyed
     *
     * @param ioOcb  OCB structure to be finalized
     */
    void OcbFinalize(resMgrIoOcb_t *ioOcb);
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
    int IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb);
    /**
     * Attempt to determine what the response code will be for this request.
     * the response will be filtered against this calculated code if not 0
     *
     * @param rawMessage pointer to the outgoing message buffer
     *
     * @return value of the expected response for this request
     */
    virtual int getExpectedFromRaw(SerialString_t rawMessage) = 0;
    virtual int getRespCode(const char *rawMessage) = 0;
    virtual vector<UINT32> getModuleIdsFromRaw(SerialString_t rawMessage, bool &isJ1939PGNRequest) = 0;
    virtual UINT32 getModuleId(const SerialString_t &rawMessage) = 0;

    virtual bool UsingGryphonUSDT() = 0;
    virtual bool IsModuleIDPresent(const vector<UINT32> incomingModuleIDs, int keyID);
    virtual string GetModuleIDsString(const vector<UINT32> incomingModuleIDs);
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
     *
     * @param buff   A working buffer for us to use
     * @param buffSz size of the buffer
     *
     * @return
     */
    int ReadPortDataUnlocked(uint8_t *buff, size_t buffSz);
    /**
     * need to override this function, because we are an ethernet connection,
     * and the set baud, etc. functions cause the system to burp.
     *
     * @param portCfg not used
     * @param mode    not used
     *
     * @return always returns 0 (success)
     */
    int ConfigurePort( const SerialProtocol &portCfg, int mode);
    /**
     * These are the functions that we do as part two of initialization (registration)
     * When complete, the physical path to the proper bus is ready to use.
     *
     * Note that we had to override the base class Register().
     * Normally this would all be in OpenPort(), but because we want to
     * process the responses to the commands with the normal receive functions,
     * we need to return from OpenPort() before proceding with the subsequent
     * commands.
     *
     * @return if successful, returns BEP_SUCCESS_RESPONSE
     */
    const std::string Register(void) ;
    /**
     * Constructor from command line arguments
     */
    IGryphonChannel(void);
    /**
     * Constructor from command line arguments
     *
     * @param argc   Number of command line arguments to parse
     * @param argv   Array of command line argument strings
     */
    IGryphonChannel( int argc, char *argv[]);
    /**
     * Class destructor.
     * @since Version 1.0
     */
    virtual ~IGryphonChannel(void);
    /**
     * Initializes the channel.
     * This includes logging on to the gryphon server,
     * determining what type of channel we are,
     * Initializing that channel
     * setting up the USDT interface, if required
     *
     * @return
     */
    void Initialize(const XmlNode *document);
    /**
     * Open the port to talk to the Gryphon server
     *
     * @param devName The device to use for communications.
     * @param mode    Mode to use for opening the device.
     *
     * @return A file descriptor to use for communicatin with the device driver or
     *         -1 if an error occurred
     * @since Version 1.0
     */
    int OpenPort( const char *devName, int mode);
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
     * Additional functionality needed when client unlocks.
     * We need to remove the module id that the client is looking for
     *
     * @param clientOcb  pointer to client
     * @param releaseAll passed to base class function
     *
     * @return return value from base class function.
     */
    int UnlockForClient( CommIoOcb_t *clientOcb, bool releaseAll=true);
    /**
     * Establish an ethernet connection to the Gryphon box
     * This should probably be performed on launch
     *
     * @param ipAddr IP address of the Gryphon box
     * @param port   Port to conenct to - default is 7000 DECIMAL
     * @return socket id # or -1 if failed
     */
    int Connect( std::string ipAddr, uint16_t port=7000);
    /**
     * Establish a session with the Gryphon box
     * This will also act as a re-login. Gryphon handles it ok without a disconnect
     * This initiates the sequence of: login, get configuration, get event names
     * response is handled in receive data
     *
     * @param userId   User name
     * @param passwd   User password
     * @param ctype    Channel type desired (from gdev.h)
     *
     * @return EOK if successful, any other value is an error code
     */
    int Login( std::string userId, std::string passwd, const uint16_t ctype);
    /**
     * Close a connection to the Gryphon
     * performed on shutdown
     */
    void Disconnect();
    /**
     * Gets the current physical vehicle channel
     * This is provided for outside users, although they probably will never need it.
     *
     * @return Current channel number for this client
     */
    int GetChannelId(void);
    /**
     * utility function
     * Sets the channel override value, used to select other than the first
     * card channel that matches the type and subtype.
     *
     * @param value  offset from first one found. 0=use first one; 1=use second one, etc
     */
    void SetChannelOverride(int value);
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

    virtual bool IsBroadcastModuleID(const UINT32 locModule);

protected:
    //ComTimer    m_replyTimeoutTimer;
    /**
     * utility function
     * rounds to next even multiple of 4
     *
     * @param locLen incoming value
     * @return corrected value
    */
    int Align32(int locLen, bool noPadding = false);
    /**
     * returns a pointer to the text associated with an event number
     * provided for outside users, for logging purposes
     *
     * @param enumber Number of the event to look up
     * @return Pointer to associated text.
     */
    char *GetEventName( const uint8_t enumber);
    /**
     * Request the text for the various event names for this channel
     *
     * @return standard status
     */
    int GetEventNames(void);
    /**
     * Interrogate the Gryphon server to obtain channel mapping information.
     * Data is stored in Config Array
     *
     * @return Standard Status 0: successful
     */
    int GetGryphonConfiguration(void) ;
    /**
     * Inform the Gryphon server to send all messages.
     *
     * @return Status of the operation.
     * @since Version 1.0
     */
    int BroadcastOn(void);
    /**
     * Set a filter in the Gryphon server.
     *
     * @param locMode Mode of the filter.
     *
     * @return Status of the operation.
     * @since Version 1.0
     */
    int SetFilterMode(uint8_t locMode);
    /**
     * Set the mode for the default filter in the Gryphon server.
     *
     * @param locMode Mode of the default filter.
     *
     * @return Status of the operation.
     * @since Version 1.0
     */
    int SetDefaultFilterMode(uint8_t locMode);
    /**
     * Add a filter to the Gryphon server.
     *
     * @param locFlags   Flags for the filter.
     * @param locOffset  Offset.
     * @param locLength  filter length.
     * @param locField   ???
     * @param locOperator
     *                   ???
     * @param firstValue ???
     * @param secondValue
     *                   ???
     *
     * @return Status of the operation.
     * @since Version 1.0
     */
    int AddFilter(uint8_t locFlags, uint16_t locOffset, uint16_t locLength, uint8_t locField, uint8_t locOperator,
                  const char *firstValue, const char *secondValue);
    /**
     * Enable all of the events.
     *
     * @return Status of the operation.
     * @since Version 1.0
     */
    int EventEnable(void);
    /**
     * Initialize the card that will be used. This essentially
     * resets the hardware for the card.
     *
     * @return Status of the operation.
     * @since Version 1.0
     */
    int InitCard(void);
    /**
     * Thread that calls InitCard and logs return value.
     *
     * @return void*.
     */
    static void* InitCardThread(void *arg);
    /**
     * Copy the configuration data returned by the Gryphon server.
     *
     * @param inBuf   Data from the Gryphon server.
     * @param datalen Length of the data.
     * @since Version 1.0
     */
    void copyConfig(const uint8_t *inBuf, const uint16_t  datalen);
    /**
     * Adds gryphon header and padding as required.
     * Sends data to ethernet port previously established
     *
     * @param inBuf     pointer to data to be sent
     * @param inSize    size of data to be sent
     * @param frameType frame type per gmsg.h
     * @param hootoo    destination of the data
     */
    int SendRawFrame(const uint8_t *inBuf, const uint16_t inSize,
                     const uint8_t frameType, const uint8_t hootoo = SD_CARD, 
                     bool noPadding = false, int msgLength = 0);
    /**
     * Adds both sets of headers and padding as required.
     * target protocol specific, code in protocol module
     *
     * @param locData The data to be sent.
     * @param inBuf   The input buffer.
     */
    virtual void BuildCardMessage(SerialString_t &locData, const SerialString_t &inBuf);
    /**
     * Check to see if this message should be blocked.
     * This is to prevent us from receiving a message twice. Once from the card
     * directly, then again as processed by the USDT handler.
     * It is implemented by blocking messages that come straight from the card that
     * will be handled by the USDT handler.
     *
     * @param inBuf  pointer to the buffer to evaluate.
     *
     * @return true if message should be ignored.
     */
    virtual bool CheckForBlock(const uint8_t *inBuf) = 0;
    /**
     * adds protocol specific gryphon data header for the outgoing data message
     *
     * WARNING!  do NOT include the length bytes in the message if using USDT
     *
     * @param locData
     * @param inBuf   source for data content.
     */
    virtual void BuildMessage(SerialString_t &locData, const SerialString_t &inBuf) = 0;
    /**
     * Wraps data message in Gryphon frame.
     *
     * @param locData
     * @param inBuf
     */
    virtual void WrapMessage(SerialString_t &locData, const SerialString_t &inBuf);

    virtual void BufferGryphonMessage(SerialString_t &frameData);
    /**
     * This processes the message passed in.
     *
     * @param inBuf  Pointer to the first character of the message
     * @param inSize overall size of the message stream
     *               (could be multiple messages, used for pointer limiting)
     * @return number of bytes processed from stream
     */
    int DecodeRead(const uint8_t *inBuf, const uint16_t inSize);
    /**
     * Processes control messages received from card channels
     *
     * @param inBuf   pointer to data to evaluate
     * @param datalen overall length of message buffer
     */
    BEP_STATUS_TYPE processNewCardMsg(const uint8_t *inBuf, const uint16_t  datalen);
    /**
     * Processes control messages received from USDT handler
     *
     * @param inBuf   pointer to data to evaluate
     * @param datalen size of received message
     */
    BEP_STATUS_TYPE processNewUsdtMsg(const uint8_t *inBuf, const uint16_t  datalen);
    /**
     * Processes control messages received from the gryphon server
     *
     * @param inBuf   pointer to data to be evaluated
     * @param datalen size of message received
     */
    BEP_STATUS_TYPE processNewServerMsg(const uint8_t *inBuf, const uint16_t  datalen);
	    /**
     * Processes control messages received from the J1939TP
     *
     * @param inBuf   pointer to data to be evaluated
     * @param datalen size of message received
     */
    BEP_STATUS_TYPE processNewJ1939ConfigurationtMsg(const uint8_t *inBuf, const uint16_t  datalen);
    /**
     * process control data received in a new message,
     *
     * Note that this section only processes control type data, not vehicle messages
     * all the processing is done here, rather than passing it to a client.
     *
     * @param inBuf  pointer to the received data to process
     *
     * @return Message processing status.
     *         BEP_STATUS_SUCCESS - message was processed properly.
     */
    BEP_STATUS_TYPE processNewMessage(const uint8_t *inBuf);
    /**
     * Copies event names for this channel to local structure
     * called by receive data processor
     *
     * @param inBuf   pointer to message from gryphon
     * @param datalen length of entire message from gryphon
     */
    void CopyEventNames(const void *inBuf, const uint16_t datalen);
    /**
     * This function does any addtional required setup for this specific protocol
     *
     * Before sending USDT messages, we need to register with the handler.
     * This method accomplishes that task for us.  Note that although the protocol
     * allows us to provide additional functional addresses, we do not implement
     * that capability at this time.  If this capability is needed, the separate
     * "CMD_USDT_SET_FUNCTIONAL" messge should be implemented.
     *
     * @return EOK if successful, any other value is an error code
     */
    virtual int ChannelSpecificInit(void) = 0;
    /**
     * utility function
     * Used in each message, as a message serial number.
     * Gryphon documentation calls this the "context" field.
     * Automatically incremented each time it is called.
     *
     * @return value to use in message
     */
    uint8_t UseContext(void);

    /**
     * Method used to close a handle to the communication device driver
     *
     * @return 0 if successful or -1 if an error occurred
     */
    virtual int ClosePort();

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
     * Arms a reply timeout timer for the client designated by 'ocb'
     *
     * @param clientOcb The OCB of the client we are arming the reply timeout timer for
     * @param forceArm  Flag touse if we want to unconditionally arm the timer (deafults to false)
     * @return 0 if timer was armed, any other value if not armed
     */
    virtual int ArmReplyTimeoutTimer( CommIoOcb_t *clientOcb, bool forceArm=false);

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
    virtual bool AddToClientFifo(const char *buff, size_t len, CommIoOcb_t *ocb);

    /**
     * DEVELOPPER NOTE:
     * If you can not get the gryphon box to respond to this command, and you are
     * sure the hardware is capable of it, you probably need to go to the front
     * panel of the gryphon, select "system programs" and make sure that
     * "USDT layer" is running.  Once turned on from the front panel, it will load
     * each time the box it restarted.
     *
     * Turning it on from a telnet session will NOT cause it
     * to start each time, but will allow it to work for the current session.
     *
     * @return
     */
    bool RegisterWithUsdt(string pathName ="", int attempts = 1); 
    bool RegisterWithUsdtNonLegacy(string pathName ="", int attempts = 1);
    /**
     * DEVELOPPER NOTE:
     * If you can not get the gryphon box to respond to this command, and you are
     * sure the hardware is capable of it, you probably need to go to the front
     * panel of the gryphon, select "system programs" and make sure that
     * "USDT layer" is running.  Once turned on from the front panel, it will load
     * each time the box it restarted.
     *
     * Turning it on from a telnet session will NOT cause it
     * to start each time, but will allow it to work for the current session.
     *
     * @return
     */
    int UnRegisterWithUsdt(void);
    /**
     * The minimum message seperation time will be set in the Gryphon box.  If seperationTime is set to 0.0, no command will
     * be sent to the Gryphon and the default will be used.
     * 
     * @param seperationTime
     *               Minimum seperation time for consecutive frames in a multi-frame message.
     * 
     * @return Status of setting the minimum seperation time.
     */
    int SetMinimumMessageSeperationTimeMultiplier(float seperationTimeMultiplier);
    int SetMinimumMessageSeperationTime(UINT8 seperationTime);
	/**
	 * Enable higher level beacon software support for j1939 
	 * porotocol 
	 *  
	 * 
	 * @author JSemann (8/20/2020) 
	 * 
	 * @return int Status of request made to gryphon
	 */
    int EnableJ1939TransportProtocol();
	/**
	 * Claim an address for tester (aka scan tool) so others cannot 
	 * take 
	 *  
	 * This makes it so only messages destined for our address and 
	 * broadcast messages are sent to us from gryphon 
	 * 
	 * @author JSemann (8/20/2020)
	 * 
	 * @param addressToClaim tester address (usually 0xf1 or 0xf9)
	 * 
	 * @return int Status of request made to gryphon
	 */
    int ClaimJ1939Address(UINT8 addressToClaim, bool filterMessages);
    int SetOptimization(bool optimizeForLatency);

    /**
     * Reverse the bytes in the provided variable.
     *
     * @since Version 1.0
     */
    void ReverseBytes(unsigned int &byteString);
	bool IsValidSourceType(uint8_t sdByte);
	bool IsValidFrameType(uint8_t frameTypeByte);

    /**
     * This is used to select an alternate physical channel of the
     * designated type and subtype.
     * Set at instantiation time, used at "connect()s"
     */
    int m_channelOverride;
    // assume there will never be more than 103 pairs.
    struct InOut m_nodeMap[104];
    int m_nodePairCount;
    // Setup data for the Gryphon
    struct InOutSetup m_nodePairSetupMap[104];
    int m_nodePairSetupCount;
    bool m_nonLegacyDevice;
    bool m_j1939Channel;
    bool m_optimizeForLatency;
    /**
     * Current channel type and subtype, from gdev.h file
     * This will be established at instantiation, depending on protocol.
     * And is used by "connect()"
     */
    int m_channelType;
    /**
     * Flag indicating that we have successfully logged into the gryphon
     */
    BepCondVar<bool> LoggedIn;
    BepCondVar<bool> UsdtRegistered;
    BepCondVar<bool> UsdtStMinMultiplier;
    BepCondVar<bool> UsdtStMinFlowControl;
    BepCondVar<bool> EventNamesLoaded;
    BepCondVar<bool> BroadcastOnAck;
    BepCondVar<bool> PassAllAck;
    BepCondVar<bool> EventEnableAck;
    BepCondVar<bool> InitAck;
    BepCondVar<bool> J1939TPEnableAck;
    BepCondVar<bool> J1939AddressClaim;
    BepCondVar<bool> SetOptimizationAck;
    std::string IPAddr;
    std::string UserId;
    std::string PortPathAlias;
    std::string PortDevPath;
    std::string Password;
    uint16_t myPort ;
    int m_usdtRegistrationAttempts;
    /**
     * This is the logical channel that is assigned to our ethernet connection
     * during the longon() process
     */
    int m_clientChannel;
    int m_destinationType;
    /** Length of CAN protocol module IDs in bits */
    int m_moduleIDByteLength;
    /**
     * current physical channel number that we are using to the vehicle.
     */
    int m_channelId;
private:
    /**
     * Check to see if a response is pending from the given module ID.
     * 
     * @param moduleId Module ID to check for response pending.
     * 
     * @return FLag indicating if a response is pending from the given module ID.
     */
    bool IsResponsePending(const vector<UINT32> &moduleIds);
    /**
     * Add the given module ID to the blocked list.
     * This indicated a message has been sent and a response is pending.
     * 
     * @param moduleId Module ID to wait for a response from.
     */
    void SetModuleResponsePending(const vector<UINT32> &moduleIds);
    /**
     * Remove a module ID from the response pending list.
     * 
     * @param moduleId Module ID to remove from the response pending list.
     */
    void ClearModuleResponsePending(const vector<UINT32> &moduleId);

    /**
     * this is the eternet socket that we have open to talk to the gryphon
     * if we were not able to connect, this is set to -1
     */
    int MySocket;
    /**
     * This is a message serial counter.  Increment it when used.
     */
    uint8_t GlobalContext;
    /**
     * contains a cross reference of event numbers to names
     */
    struct EventStructure EventIDs;
    /**
     * contains an array of channel descriptors
     */
    struct ChannelConfig Channels;
    /**
     * List of module IDs that have a response pending from the vehicle.
     */
    typedef list<vector<UINT32> >              ResponsePendingList;
    typedef ResponsePendingList::iterator      ResponsePendingItr;
    ResponsePendingList                        m_moduleResponsePending;
    BepMutex                                   m_responsePendingMutex;
};

#endif
