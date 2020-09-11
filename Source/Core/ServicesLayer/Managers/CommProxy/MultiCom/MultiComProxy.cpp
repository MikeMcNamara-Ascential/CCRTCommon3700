//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/MultiCom/MultiComProxy.cpp $
// $Author: Rwiersem $
//   $Date: 10/26/06 10:30a $
//
// Description:
//  Logical serial port driver for CAN serial communication
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/MultiCom/MultiComProxy.cpp $
// 
// 3     10/26/06 10:30a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the clientOcb->proxyOcb parameter to TransmitMessage().
// 
// 2     4/13/06 3:57a Cward
// cleanup
//
// 3     2/24/06 1:16p Bmeinke
// Removed all references to the USE_READER_THREAD macro
//
// 2     2/15/06 4:06p Bmeinke
// Initialize some uninitialized variables
//
// 1     5/11/05 9:39p Cward
// Initial Version
//
// 1     4/26/05 10:02a Bmeinke
//
// 2     2/18/05 12:46p Bmeinke
// Fixed MultiComInit() so that we will leave the initialization loop if
// the initialization is successful
//
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
//
// 2     8/30/04 3:57p Bmeinke
// Try to initialize the MultiCom board up to 3 times in MultiComInit()
//
// 5     6/17/04 11:14p Bmeinke
// Changed some int's to uint32_t's to remove compiler warnings
//
// 4     5/11/04 8:58p Bmeinke
// Removed the message sent condvar from the OCB structure and made it a
// class variable
// GetModuleIdFromMessage() now returns $5A if the "calculated" module ID
// in a reply message is not $28 (to accomodate the NSCL message)
// No longer arm a client's reply timeout timer inside IoRead if there is
// no data avilable
// Only call WaitToSendToModule() for non-broadcast messages inside
// IoWrite()
// Do not arm a reply timeout timer for broadcast messages inside
// IoWrite()
// Don't boost the thread priority inside WaitForMessageSent()
// ExtractNextPacket() now looks for a $34, $D0 or $D1 as the only valid
// start of packet characters.
// ProcessHBCCTxPacket() now exclusively uses m_clientBeingAcked to know
// what client/message the MultiCom TX status packet is for
// MultiComInit() now checks to see if the calling thread is the reader
// thread before calling SuspendReaderThread() and ResumeReaderThread()
// Added IsBroadcastCommand() method to check if a TX message is a
// broadcast message or not
//
// 3     4/29/04 8:01p Bmeinke
// GetModuleIdFromMessage() returns a module ID of $5A if the module ID in
// a diagnostic reply is $13 to handle the reply for a No Stored Code
// Logging command
// Added a txStatus member to the MultiComIoOcb_t structure. Used to tell
// a client that the MultiCom replied with an error in the "Message sent"
// packet. A client waiting for the MultiCom to say that it has sent a
// message on the bus MUST inspect this data member after being woken up
// from the wait to see if the MultiComm successfully sent the message or
// not.
// Fixed the bus comm logging
// Lowered the timeout in WaitForMessageSent() from 5 seconds to 500ms
// Inspect the txStatus member of the client OCB in WaitForMessageSent()
// after waking up to check if our message was successfully sent or not
// The calculated packet size in ExtractNextPacket() was off by 2 bytes
// Update the txStatus of the client OCB (the client waiting for a message
// to be sent to the vehicle) inside ProcessHBCCTxPacket() to EOK/errno
// Log MultiCom transmit error status
// Fixed CheckForAck()
// Fixed the expected response string inside MultiComReset()
// The RS485/SCP versions are now properly parsed from the MultiCom
// responses to the version request commands
// Changed the inter-byte timeout in GetMultiComResponse() to 200ms (was
// 10)
// Fixed syntax errors in CalculateCRC()
//
// 2     4/23/04 4:59p Bmeinke
// Added CRC processing (add to TX messages and verify on RX messages)
// Recognize the No Stored Code Logging reply as a diagnostic reply
// Expanded the MultiComInit() method
//
// 1     4/12/04 1:21p Bmeinke
// Changing VSS repositories
//
// 1     4/12/04 10:34a Bmeinke
//
// 2     6/24/03 9:58a Bmeinke
// Changed ModuleId_t data type from uint8_t to uint16_t
// Changed the processing of the 'dataReady' flags in the m_moduleList
// list: Only clear the flag when a client empties his FIFO and set it to
// true after we have received a full packet from a module in
// ProcessVehicleMessage()
// When we receive data from a module we always put the received data into
// ur client's fifo(s) (even if we reply to the client right away). This
// way, if a client is  read-blocked waiting for less data than we can
// send to him, his fifo will contain the remaining data on his next
// read() call.
//
// 1     6/20/03 9:17p Bmeinke
//
//******************************************************************************
#define LOG_PARTIAL_PACKET  0

#include "MultiComProxy.h"

const uint8_t AckString[] = ACK_STRING;

const uint32_t  MultiComTimeoutVal  =   10;

uint8_t MultiComProxy::m_crcTable[256] = {
    196,217,254,227,176,173,138,151, 44, 49, 22, 11, 88, 69, 98,127,
    9, 20, 51, 46,125, 96, 71, 90,225,252,219,198,149,136,175,178,
    67, 94,121,100, 55, 42, 13, 16,171,182,145,140,223,194,229,248,
    142,147,180,169,250,231,192,221,102,123, 92, 65, 18, 15, 40, 53,
    215,202,237,240,163,190,153,132, 63, 34,  5, 24, 75, 86,113,108,
    26,  7, 32, 61,110,115, 84, 73,242,239,200,213,134,155,188,161,
    80, 77,106,119, 36, 57, 30,  3,184,165,130,159,204,209,246,235,
    157,128,167,186,233,244,211,206,117,104, 79, 82,  1, 28, 59, 38,
    226,255,216,197,150,139,172,177, 10, 23, 48, 45,126, 99, 68, 89,
    47, 50, 21,  8, 91, 70, 97,124,199,218,253,224,179,174,137,148,
    101,120, 95, 66, 17, 12, 43, 54,141,144,183,170,249,228,195,222,
    168,181,146,143,220,193,230,251, 64, 93,122,103, 52, 41, 14, 19,
    241,236,203,214,133,152,191,162, 25,  4, 35, 62,109,112, 87, 74,
    60, 33,  6, 27, 72, 85,114,111,212,201,238,243,160,189,154,135,
    118,107, 76, 81,  2, 31, 56, 37,158,131,164,185,234,247,208,205,
    187,166,129,156,207,210,245,232, 83, 78,105,116, 39, 58, 29,  0
    };

/**
 * Default Constructor
 */
MultiComProxy::MultiComProxy() : RawCommProxy(),
m_bytesToCome( 0), m_waitingForAck( false),
m_translatorAckString( AckString), m_translatorAckTimeout( 100),
m_ackChid( -1), m_ackCoid( -1), m_clientBeingAcked( NULL),
m_timeoutPulseVal( 0x200)
{
    SetProcessName( MULTICOM_PROC_NAME);
}

/**
 * Constructor from command line arguments
 *
 * @param argc   Number of command line arguments to aprse
 * @param argv   Array of command line argument strings
 */
MultiComProxy::MultiComProxy( int argc, char *argv[]) : RawCommProxy( argc, argv),
m_bytesToCome( 0), m_waitingForAck( false),
m_translatorAckString( AckString), m_translatorAckTimeout( 100),
m_ackChid( -1), m_ackCoid( -1), m_clientBeingAcked( NULL),
m_timeoutPulseVal( 0x200)
{
}

/**
 * Class destructor
 */
MultiComProxy::~MultiComProxy()
{
}

/**
 * Method that is called to command the component to perform any
 * system registration functions necessary.
 *
 * @return Status of the registration process.
 *         Success, Fail
 * @since Version 2.5
 */
const std::string MultiComProxy::Register(void)
{
    std::string retVal( BEP_FAILURE_RESPONSE);

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::Register()\n");
    // Do base class stuff first
    if( (retVal=RawCommProxy::Register()) == std::string( BEP_SUCCESS_RESPONSE))
    {
        // Setup the MultiCom box
        if( MultiComInit() == true)
        {
            Log( LOG_DEV_DATA, "MultiCom initialized\n");
        }
        else
        {
            Log( LOG_ERRORS, "\tMultiCom initialization failed\n");
            retVal = std::string( BEP_FAILURE_RESPONSE);
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tBase class initialization failed\n");
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::Register( %s)\n", retVal.c_str());

    return( retVal);
}

/**
 * Perform any server initialization
 *
 * @return true if successful
 *         false if error
 */
bool MultiComProxy::InitManager()
{
    bool    retVal = false;
    int     state = 0;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::InitManager()\n");


    // Step through the initialization
    while( state >= 0)
    {
        switch( state)
        {
        case 0:
            if( RawCommProxy::InitManager() == false)
            {
                Log( LOG_ERRORS, "\tError in base class InitManager\n");
                state = -2;
            }
            break;
        case 1:     // Create the channel/connection to which the ACK pulse can be sent
            m_ackChid = ChannelCreate( 0);
            m_ackCoid = ConnectAttach( ND_LOCAL_NODE, 0, m_ackChid, _NTO_SIDE_CHANNEL, 0);
            if( (m_ackChid == -1) || (m_ackCoid == -1))
            {
                Log( LOG_ERRORS, "\tError creating chId/coId for MultiCom ACK: %s\n", strerror( errno));
                state = -2;
            }
            break;
        case 2:     // Setup the MultiCom timeout timer to 5 seconds
            if( m_respTimeoutTimer.Initialize( m_mssgCoid, REPLY_TIMEOUT_TIMER, m_timeoutPulseVal) == -1)
            {
                Log( LOG_ERRORS, "\tError arming MultiCom reply timeout timer\n");
            }
            break;
        case 3:     // Initialize our message sent flag
            if( m_messageSent.Acquire() == EOK)
            {
                m_messageSent.Signal(MC_NO_MSSG_SENT);

                m_messageSent.Release();
            }
            break;
        default:    // Woo-hoo...it all worked !!!
            retVal = true;
            state = -2;
            break;
        }
        state++;
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::InitManager()\n");

    return( retVal);
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void MultiComProxy::CleanupManager()
{
    Log( LOG_FN_ENTRY, "Enter MultiComProxy::CleanupManager()\n");

    ConnectDetach( m_ackCoid);
    m_ackCoid = -1;

    ChannelDestroy( m_ackChid);
    m_ackChid = -1;

    // Make sure all timers are stopped
    StopComTimers();

    // Cleanup base class
    RawCommProxy::CleanupManager();

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::CleanupManager()\n");
}

/**
 * Method used to check if a serial response string can be added to a
 * client's rx FIFO
 *
 * @param data   Serial string received from the port
 * @param ocb    Client connection identifier
 * @return true if the data should be added to the client's FIFO, false otherwise
 */
bool MultiComProxy::CanAddToClientFifo(const SerialString_t &data, CommIoOcb_t *ocb)
{
    MultiComIoOcb_t *client = (MultiComIoOcb_t*)ocb;
    bool            canAdd=false;
    bool            isDiagMssg;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::CanAddToClientFifo()\n");

    // Check with base class first for filter strings
    if( RawCommProxy::CanAddToClientFifo( data, ocb) == true)
    {
        // Check if this is a diagnostic reply message
        isDiagMssg = IsDiagReplyMsg( data.c_str(), data.size());

        // If this is a response to a diag command and
        if( isDiagMssg)
        {
            try
            {
                const ModuleId_t replyModId = GetModuleIdFromMessage( data.c_str(), data.size());

                Log( LOG_DEV_DATA, "Got a diagnostic reply from module $%02hhX\n", replyModId);
                // If the module responding is this subscriber's module
                if( replyModId == client->clientModuleId)
                {
                    Log( LOG_DEV_DATA, "Found matching client module for module $%02hhX (rx ID=%lld)\n",
                         replyModId, ocb->rxSubscription->id);
                    // Say ok to add to this client's buffer
                    canAdd = true;
                }
                else
                {
                    Log( LOG_DEV_DATA, "Not OK to add to client fifo because module ID does not match: $%02hhX != $%02hhX\n",
                         replyModId, client->clientModuleId);
                }
            }
            catch( BepException &err)
            {
                Log( LOG_ERRORS, "Unable to extract module ID: %s\n", err.GetReason());
            }
        }
        else
        {
            /**
             *  If not a diag message, assume it is a broadcast message. If it
             * passed through the base class' filter string checks, assume it
             * is ok to add the data to this client's fifo
             */
            Log( LOG_DEV_DATA, "Got a NON-diagnostic reply...assuming OK to add to FIFO\n");
            canAdd = true;
        }
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::CanAddToClientFifo()\n");

    return( canAdd);
}

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
ModuleId_t MultiComProxy::GetModuleIdFromMessage( const uint8_t *mssg, size_t msgLen) throw(BepException)
{
    ModuleId_t  retVal;


    // If this is a MultiCom ACK message
    if( mssg[ 0] == MC_HBCC_RCVD_MSSG)
    {
        // Module ID is in the 4th byte
        if( msgLen > 3)
        {
            retVal = mssg[ 3];
        }
        else
        {
            throw( BepException( "Invalid MC_HBCC_RCVD_MSSG packet length"));
        }
    }
    // If this is a MultiCom ACK message
    else if( mssg[ 0] == MC_HBCC_SENT_MSSG)
    {
        // Module ID is in the 6th byte
        if( msgLen > MODULE_ACK_ID_INDEX)
        {
            retVal = mssg[ MODULE_ACK_ID_INDEX];

            /**
             * The "No Stored Code Logging" mesage is an SCP network broadcast
             * message (i.e. no real module ID in the transmitted message. The
             * Multicom TX acknowledgement packet has a random module ID at the
             * module ID index, so just re-map the reply module ID to $5A (which
             * is the value at the module ID index in the message we sent
             */
            if( retVal != 0x28)
            {
                if( (m_clientBeingAcked != NULL) && (m_clientBeingAcked->clientModuleId == 0x5A))
                {
                    Log( LOG_DEV_DATA, "Re-mapping response module ID $%02hhX to $5A\n", retVal);
                    retVal = 0x5A;
                }
            }
        }
        else
        {
            throw( BepException( "Invalid MC_HBCC_RCVD_MSSG packet length"));
        }
    }
    // Not a MultiCom ACK message...Module ID is in the 3rd byte
    else if( msgLen > MODULE_RX_ID_INDEX)
    {
        retVal = mssg[ MODULE_RX_ID_INDEX];
    }
    else
    {
        throw( BepException( "Packet too short to contain module ID"));
    }

    return( retVal);
}

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
IOFUNC_OCB_T* MultiComProxy::OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
{
    MultiComIoOcb_t *ocb = new MultiComIoOcb_t;

    return( (IOFUNC_OCB_T*)ocb);
}

/**
 * Method used to initialize an allocated OCB structure. Called
 * immediately after the OCB is allocated
 *
 * @param ioOcb  OCB structure to be initialized
 */
void MultiComProxy::OcbInit(resMgrIoOcb_t *ioOcb)
{
    MultiComIoOcb_t *newOcb = (MultiComIoOcb_t*)ioOcb;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::OcbInit()\n");

    RawCommProxy::OcbInit( ioOcb);

    newOcb->clientModuleId = INVALID_MODULE_ID;
    newOcb->proxyOcb.replyTimeout = DEF_MOD_TIMEOUT;
    newOcb->txStatus = EOK;

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::OcbInit()\n");
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
void MultiComProxy::OcbFree(resMgrIoOcb_t *ocb)
{
    MultiComIoOcb_t *clientOcb = (MultiComIoOcb_t*)ocb;
    ModuleId_t      clientModId;
    int             err;

    // try to lock the list while we modify the OCB pointer in the m_moduleList
    if( (err=m_moduleList.Acquire()) != EOK)    Log( LOG_ERRORS, "\tError locking module list in OcbFinalize: %s\n",
                                                     strerror( err));

    clientModId = clientOcb->clientModuleId;
    if( clientModId != INVALID_MODULE_ID)
    {
        if( m_moduleList[ clientModId].clientOcb == clientOcb)
        {
            m_moduleList[ clientModId].clientOcb = NULL;
        }
    }

    delete clientOcb;

    if( err == EOK) m_moduleList.Release();
}

/**
 * Method used to finalize an allocated OCB structure. Called immediately
 * before the OCB is destroyed
 *
 * @param ioOcb  OCB structure to be finalized
 */
void MultiComProxy::OcbFinalize(resMgrIoOcb_t *ioOcb)
{
    Log( LOG_FN_ENTRY, "Enter MultiComProxy::OcbFinalize()\n");

    RawCommProxy::OcbFinalize( ioOcb);

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::OcbFinalize()\n");
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
int MultiComProxy::IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
    MultiComIoOcb_t *clientOcb = (MultiComIoOcb_t*)ioOcb;
    CommIoOcb_t     *proxyOcb = &clientOcb->proxyOcb;
    int             retVal = ENOSYS;
    bool            okToNotify = false;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::IoRead(), module=$%X, rx ID=%lld\n",
         clientOcb->clientModuleId, clientOcb->proxyOcb.rxSubscription->id);

    // Get the module ID this client shuld be reading from
    ModuleId_t  clientModId = clientOcb->clientModuleId;

    // IF this is not a "combine" read
    if( (msg->i.xtype & _IO_XTYPE_MASK) == _IO_XTYPE_NONE)
    {
        okToNotify = proxyOcb->rxSubscription->ShouldNotify();
        // Make sure client has performed vaid write or has a valid subscription
        if( (clientModId != INVALID_MODULE_ID) || (okToNotify == true) )
        {
            // If this client is not associated with any particular module
            //  and client has a valid subscription
            if( (clientModId == INVALID_MODULE_ID) && ( okToNotify == true) )
            {
                Log( LOG_IO_MSSG, "Client has a valid, non-module specific subscription\n");

                // Repy to the client with his data
                retVal = RawCommProxy::IoRead(ctp,msg,ioOcb);
            }
            else
            {
                if( (retVal=m_blockedReaders.Lock()) == EOK)
                {
                    CommGapTimer &comTimer = m_moduleList[ clientModId].replyTimeoutTimer;

                    Log( LOG_DEV_DATA, "Client requesting %d bytes from module $%X\n",
                         msg->i.nbytes, clientOcb->clientModuleId);
                    // Check to see if we have any data ready to be read
                    if( ModuleHasReplied( clientModId) == true)
                    {
                        Log( LOG_IO_MSSG, "Client's module $%X has replied\n",
                             clientOcb->clientModuleId);

                        // Reset to default reply timeout
                        proxyOcb->replyTimeout = m_defaultReplyTimeout;
                        if( comTimer.IsTimerArmed())    comTimer.Stop();

                        // Let the base class get data from FIFO and reply to client
                        retVal = RawCommProxy::IoRead(ctp,msg,ioOcb);

                        // Clear the 'module has replied' flag only if no more data in client's FIFO
                        if( proxyOcb->rxSubscription->fifo.GetSize() == 0)
                        {
                            Log( LOG_DEV_DATA, "Clearing OK to send flag for moduel $%X because FIFO is empty\n",
                                 clientModId);
                            SignalModuleReply( clientModId, false);
                        }
                    }
                    // No data and OK for client to block
                    else if( !(proxyOcb->baseOcb.m_ioOcb.ioflag & O_NONBLOCK))
                    {
#if 0
                        // If reply timer is not active
                        if( (comTimer.IsTimerArmed() == false) && (proxyOcb->replyTimeout > 0))
                        {
                            // Re-start the reply timeout timer
                            if( comTimer.Initialize( m_mssgCoid, REPLY_TIMEOUT_TIMER, clientModId) == -1)
                            {
                                Log( LOG_ERRORS, "\tError arming reply timeout timer for module $%X\n", clientModId);
                            }

                            comTimer.Arm( proxyOcb->replyTimeout);
                            Log( LOG_DEV_DATA, "Re-starting reply message "
                                 "watchdog timer for module $%X for %lld ns\n",
                                 clientModId, proxyOcb->replyTimeout);
                        }
                        else
                        {
                            Log( LOG_DEV_DATA, "Reply watchdog timer for module "
                                 "$%X already running (or disabled)\n", clientModId);
                        }
#endif

                        Log( LOG_IO_MSSG, "Adding client $%X to blocked reader list\n",
                             clientOcb->clientModuleId);

                        // Add this client to our "waiting to read" list
                        m_blockedReaders.AddBlockedReader( clientOcb, ctp->rcvid, *msg);

                        Log( LOG_IO_MSSG, "Client $%X added to blocked reader list\n",
                             clientOcb->clientModuleId);

                        // Tell OS posix layer to not reply to the client
                        retVal = _RESMGR_NOREPLY;
                    }
                    else
                    {
                        // No data and client opened us in O_NONBLOCK...
                        // Try again
                        Log( LOG_IO_MSSG, "No data in O_NONBLOCK\n");
                        retVal = EAGAIN;
                    }

                    m_blockedReaders.Unlock();
                }
                else
                {
                    Log( LOG_ERRORS, "\tError #%d locking reader list in IoRead: %s\n", retVal,
                         strerror( retVal));
                    retVal = EIO;
                }
            }
        }
        else
        {
            // If ok to notify client and we are here...the module Id is invalid
            if( okToNotify == false)
            {
                Log( LOG_IO_MSSG, "Invalid module ID: $%X\n", clientModId);
            }
            else
            {
                // !!!!!!!!!!!!!!!!!!!!!!!!!!! //
                /// SHOULD NEVER GET IN HERE ////
                // !!!!!!!!!!!!!!!!!!!!!!!!!!! //

                Log( LOG_IO_MSSG, "Invalid something or other, module ID: $%X\n", clientModId);
            }
            retVal = ENXIO;
        }
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::IoRead(), module=$%X, rx ID=%lld\n",
         clientOcb->clientModuleId, clientOcb->proxyOcb.rxSubscription->id);

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
int MultiComProxy::IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb)
{
    MultiComIoOcb_t *clientOcb = (MultiComIoOcb_t*)ioOcb;
    SerialString_t  rawMessage;
    SerialString_t  &clientMssg = clientOcb->lastSentMssg;
    int             bytesRead=0;
    int             retVal = EINVAL;
    ModuleId_t      moduleId = INVALID_MODULE_ID;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::IoWrite(), moduleId=$%X, rx ID=%lld\n",
         clientOcb->clientModuleId, clientOcb->proxyOcb.rxSubscription->id);

    // Make sure OK for client to write (port not locked)
    if( (CanClientWrite( &clientOcb->proxyOcb) == true) &&
        ((msg->i.xtype & _IO_XTYPE_MASK) == _IO_XTYPE_NONE))
    {
        // Read the message data from the sender
        if( (bytesRead = ReadSendersBuffer(ctp, rawMessage, msg)) > 1)
        {
            // Format the message for the Bus Translator
            clientMssg = rawMessage;
            // Add the CBC (Command Byte Count) to the message header
            //  The CBC DOES NOT include the CRC byte
            clientMssg.insert( clientMssg.begin(), rawMessage.size());
            // Add the MultiCom "SendMessage" header byte
            clientMssg.insert( clientMssg.begin(), MC_SCP_SEND_MSSG);
            // Add the multicom CRC byte
            AddCRC( clientMssg);

            PrintSerialString( LOG_SER_COM, "Sending", clientMssg);

            // Wait for exclusive access to the MultiCom box
            if( (errno=m_translatorLock.Acquire()) == EOK)
            {
                // Say no MultiCom ACK received yet
                if( (retVal=m_ackRxd.Acquire()) == EOK)
                {
                    m_ackRxd.Signal( 0);
                    m_ackRxd.Release();
                }
                else
                {
                    m_ackRxd.Signal( 0);
                    Log( LOG_ERRORS, "\tError clearing MultiCom ACK sync: %s\n", strerror( retVal));
                }

                /**
                 * Tell comm logger which client is being acked. Also
                 * used to tell the reader thread what client is waiting
                 * for the "Message Sent" reply from the MultiCom board
                 */
                m_clientBeingAcked = clientOcb;

                // Clear the TX error flag
                clientOcb->txStatus = EOK;

                // See which module we are sending to
                clientOcb->clientModuleId = clientMssg[ MODULE_TX_ID_INDEX];
                moduleId = clientOcb->clientModuleId;

                m_moduleList[ moduleId].clientOcb = clientOcb;
                bool canSend = true;

                // If this is not a broadcast message
                if( IsBroadcastCommand( clientMssg) == false)
                {
                    // Wait until we can send to this module
                    canSend = WaitToSendToModule( moduleId);
                }

                // Wait until we can send to this module
                if( canSend == true)
                {
                    // Cant send to this module until we get a response (or reply timeout)
                    SetOkToSend( moduleId, false);

                    // Say we are waiting for a MultiCom "message sent" ack for this module
                    ArmMessageSent( clientOcb);

                    // Wait for ACK back from bus translator
                    if( m_ackLock.Acquire() == EOK)
                    {
                        // Clear the 'module has replied' flag only if no more data in client's FIFO
                        if( clientOcb->proxyOcb.rxSubscription->fifo.GetSize() == 0)
                        {
                            Log( LOG_DEV_DATA, "Clearing OK to send flag for moduel $%X because FIFO is empty\n",
                                 moduleId);
                            // Say no data received from module yet
                            SignalModuleReply( moduleId, false);
                        }

                        // Log communication for this serial send (use rawMessage so
                        //  byte count not added to comm log)
                        UpdateBusCommLog( ComDirTx, clientMssg.c_str(), clientMssg.size(), &clientOcb->proxyOcb);

                        // Send this message to the vehicle
                        TransmitMessage( clientMssg, &(clientOcb->proxyOcb));

                        // Set the MultiCom timeout timer
                        m_respTimeoutTimer.Arm( MULTI_COM_TIMEOUT);

                        // Wait for the MultiCom ACK to come back (message received)
                        if( (retVal=WaitForTranslatorAck()) == EOK)
                        {
                            // Wait for the MultiCom box to tell us that it has sent the message
                            if( (retVal=WaitForMessageSent( clientOcb)) == EOK)
                            {
                                // Arm the inter-byte gap timer (if enabled)
                                uint32_t    msGapTime = m_currentPortProtocol.GetMsgGap();
                                if( msGapTime > 0)
                                {
                                    // Retart message gap timer for this module
                                    Log( LOG_DEV_DATA, "Starting message gap timer for module $%X for %d ms\n", moduleId, msGapTime);
                                    m_moduleList[ moduleId].mssgGapTimer.Initialize( m_mssgCoid, MSSG_GAP_TIMER, moduleId);
                                    m_moduleList[ moduleId].mssgGapTimer.Arm( mSEC_nSEC( msGapTime));
                                }

                                // If this is a standard diagnostic messasge
                                if( IsBroadcastCommand( clientMssg) == false)
                                {
                                    // Arm the reply timeout timer for this client
                                    ArmReplyTimeoutTimer( (CommIoOcb_t*)clientOcb, true);
                                }
                                else
                                {
                                    // This is not a standard diag message...assume a broadcast w/ no reply
                                    Log( LOG_DEV_DATA, "Not arming reply timeout timer for broadcast message\n");
                                    // Say we received datat from module because no reply is expected
                                    SignalModuleReply( moduleId, true);
                                    // Say OK to send a message to this module again
                                    SetOkToSend( moduleId, true);
                                }

                                // Tell kernel to update last modification time
                                ioOcb->m_ioOcb.attr->flags |= IOFUNC_ATTR_MTIME |
                                                              IOFUNC_ATTR_DIRTY_TIME;
                            }
                            else
                            {
                                Log( LOG_ERRORS, "\tERROR: MultiCom DID NOT respond with 2nd ACK\n");
                                // Tell client nothing was written
                                bytesRead = 0;
                            }
                        }
                        else
                        {
                            Log( LOG_ERRORS, "\tERROR: MultiCom DID NOT respond with ACK\n");
                            // Tell client nothing was written
                            bytesRead = 0;
                        }
                        // Release this mutex to unblock the thread which sent us this ACK pulse
                        m_ackLock.Release();
                    }
                    else
                    {
                        Log( LOG_ERRORS, "\tError acquiring MultiCom ACK Lock: %s\n", strerror( errno));
                        bytesRead = 0;
                    }

                    // Advance the offset inside the OCB structure
                    ioOcb->m_ioOcb.offset += bytesRead;

                    // Tell the client how many bytes were written
                    _IO_SET_WRITE_NBYTES( ctp, bytesRead);

                    retVal = EOK;
                }
                else
                {
                    retVal = ETIMEDOUT;
                }

                // No-one waiting for ack anymore
                m_clientBeingAcked = NULL;

                m_translatorLock.Release();
            }
            else
            {
                retVal = errno;
            }
        }
        else if( bytesRead == -1)
        {
            Log( LOG_ERRORS, "Error reading client io_write buffer: %s\n", strerror( errno));
            retVal = errno;
        }
        else
        {
            Log( LOG_ERRORS, "Error reading client io_write buffer (%d of %d bytes)\n",
                 bytesRead, msg->i.nbytes);
            retVal = EINVAL;
        }
    }
    else
    {
        retVal = EPERM;
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::IoWrite(), %d bytes status=%d module=$%X, rx ID=%lld\n",
         bytesRead, retVal, clientOcb->clientModuleId, clientOcb->proxyOcb.rxSubscription->id);

    return( retVal);
}

/**
 * Wait for command ACK's from the bus translator
 *
 * @param ackCount Number of ACK responses we expect
 * @return EOK if we received the ACK, error code (e.g. ETIMEDOUT) otherwise
 */
int MultiComProxy::WaitForTranslatorAck( int ackCount)
{
    int             retVal = ETIMEDOUT;
    uint64_t        nsTimeout = ACK_TIMEOUT * ackCount;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::WaitForTranslatorAck( %d)\n", ackCount);

    int curPrio = RaiseThreadPriority( 2);

    if( (retVal=m_ackRxd.Acquire()) == EOK)
    {
        if( (retVal=m_ackRxd.Wait( ackCount, nsTimeout)) == EOK)
        {
            // Say no longer waiting for the ACK
            m_ackRxd.Signal( 0);
            Log( LOG_DEV_DATA, "Got MultiCom ACK\n");
            retVal = EOK;
        }
        else
        {
            Log( LOG_ERRORS, "\tError waiting for MultiCom ACK: %s\n", strerror( retVal));
        }

        m_ackRxd.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking MultiCom ACK sync: %s\n", strerror( retVal));
    }

    SetThreadPriority( curPrio);

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::WaitForTranslatorAck( %d)\n", ackCount);

    return( retVal);
}

/**
 * Waits for the MultiCom board to tell us that our message
 * has been transmitted to the vehicle
 *
 * @param clientOcb OCB of the client that sent the last message to the vehicle
 *
 * @return EOK if the MultiCom responded with the Message Sent reply
 *         error code (e.g. ETIMEDOUT) otherwise
 */
int MultiComProxy::WaitForMessageSent( MultiComIoOcb_t *clientOcb)
{
    int                 retVal = ETIMEDOUT;
    uint64_t            nsTimeout = mSEC_nSEC(500LL);//MULTI_COM_TIMEOUT;
    MultiComTxCondVar   &mssgSent = m_messageSent;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::WaitForMessageSent()\n");

    if( (retVal=mssgSent.Acquire()) == EOK)
    {
        Log( LOG_DEV_DATA, "Locked messageSent flag for module $%02hhX in MultiComProxy::WaitForMessageSent()\n",
             clientOcb->clientModuleId);

        if( (retVal=mssgSent.Wait( MC_MSSG_SENT_ACK, nsTimeout)) == EOK)
        {
            // Say no longer waiting for the MessageSent message
            mssgSent.Signal( MC_NO_MSSG_SENT);
            Log( LOG_DEV_DATA, "Signaled messageSent flag to <MC_NO_MSSG_SENT> in MultiComProxy::WaitForMessageSent()\n",
                 clientOcb->clientModuleId);

            // Grab the TX status
            retVal = clientOcb->txStatus;
            if( retVal == EOK)
            {
                Log( LOG_DEV_DATA, "MultiCom has sent our message to the module $%02hhX\n",
                     clientOcb->clientModuleId);
            }
            else
            {
                Log( LOG_ERRORS, "MultiCom error sending our data to module $%02hhX: %s\n",
                     clientOcb->clientModuleId, strerror( retVal));
            }
        }
        else
        {
            Log( LOG_ERRORS, "\tError waiting for MultiCom to send our message to the module $%02hhX: %s\n",
                 clientOcb->clientModuleId, strerror( retVal));
        }

        mssgSent.Release();
        Log( LOG_DEV_DATA, "Unlocked messageSent flag for module $%02hhX in MultiComProxy::WaitForMessageSent()\n",
             clientOcb->clientModuleId);
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking 'MessageSent' sync: %s\n", strerror( retVal));
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::WaitForMessageSent()\n");

    return( retVal);
}

/**
 * Arms a reply timeout timer for the client designated by 'ocb'
 *
 * @param clientOcb The OCB of the client we are arming the reply timeout timer for
 * @param forceArm  Flag touse if we want to unconditionally arm the timer (deafults to false)
 *
 * @return 0 if timer was armed, any other value if not armed
 */
int MultiComProxy::ArmReplyTimeoutTimer( CommIoOcb_t *clientOcb, bool forceArm)
{
    MultiComIoOcb_t *ocb = (MultiComIoOcb_t*)clientOcb;
    int             retVal = 1;
    ModuleId_t      moduleId = ocb->clientModuleId;
    uint64_t        timeOut;

    /**
     * We will call this method with 'forceArm' set to true ONLY after we have
     * sent a mesage to the MultiCom and the MultiCom has replied with an ACK
     * (to tellus that the message has been sent out on the J1850 bus)
     */
    if( forceArm == true)
    {
        // If we should use the default message timeout value
        if( clientOcb->replyTimeout == 0)
        {
            timeOut = m_defaultReplyTimeout;
        }
        else
        {
            // Use the client's specified timeout
            timeOut = clientOcb->replyTimeout;

            // Reset to default reply timeout
            clientOcb->replyTimeout = m_defaultReplyTimeout;
        }
        // Start the response timout timer
        CommGapTimer &comTimer = m_moduleList[ moduleId].replyTimeoutTimer;
        // Initialize the timeout timers to default attributes
        if( comTimer.Initialize( m_mssgCoid, REPLY_TIMEOUT_TIMER, moduleId) == -1)
        {
            Log( LOG_ERRORS, "\tError arming reply timeout timer for module $%X\n", moduleId);
        }
        comTimer.Arm( timeOut);
        Log( LOG_DEV_DATA, "Starting module $%X comm timer to %llu ns\n",
             moduleId, timeOut);

        retVal = 0;
    }
    return( retVal);
}

/**
 * Method called to read received data from the port
 *
 * @param buff   Buffer to put the received data into
 * @param buffSz Size (in bytes) of the buff parameter
 *
 * @return The number of bytes received from the port
 */
int MultiComProxy::ReadPortDataUnlocked(uint8_t *buff, size_t buffSz)
{
    SerialString_t  packet;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::ReadPortDataUnlocked()\n");

    // Try to extract the next full packet from our FIFO
    while( ExtractNextPacket(*m_rxFifo, packet) > 0)
    {
        PrintSerialString( LOG_SER_COM, "MultiCom Packet", packet);

        // Decipher the MultiCom response
        switch( packet[ 0])
        {
        case MC_SCP_SEND_MSSG:
            ProcessAckMessage(packet, packet.size());
            break;
        case MC_HBCC_SENT_MSSG:
            ProcessHBCCTxPacket(packet, packet.size());
            break;
        case MC_HBCC_RCVD_MSSG:
            ProcessHBCCRxPacket(packet, packet.size());
            break;
        case MC_SCP_GET_FIRMWARE_VER:
        case MC_SCP_NORMAL_MODE:
        case MC_SCP_GET_STATUS:
        case MC_SCB_READ_MSSG:
        case MC_SCP_GET_RCV_BYTE_COUNT:
        case MC_SCP_CLEAR_RCV_BUFF:
        case MC_SCP_CLEAR_XMT_BUFF:
        case MC_SCP_GET_XMT_BYTE_COUNT:
        case MC_SCP_GET_BIT_RATE:
        case MC_SCP_SET_BIT_RATE:
        case MC_SCP_GET_INTER_MSSG_DELAY:
        case MC_SCP_SET_INTER_MSSG_DELAY:
        case MC_SCP_START_GROUP:
        case MC_SCP_STOP_GROUP:
        case MC_SCP_GET_GRP_COUNT:
        case MC_SCP_READ_GRP_MSSG:
        case MC_SCP_GET_GRP_CONTROL:
        case MC_SCP_CLEAR_GROUP_BUFF:
        case MC_SCP_DEF_GRP_MSSG:
        case MC_SCP_DEF_GRP_CONTROL:
        case MC_SCP_TERMINATION_ON:
        case MC_SCP_TERMINATION_OFF:
        case MC_SCP_READ_HBCC_REGISTERS:
        case MC_SCP_WRITE_HBCC_REGISTERS:
        case MC_SCP_READ_HBCC_RAM:
        case MC_SCP_WRITE_HBCC_RAM:
        case MC_SCP_GET_SMT_STATUS:
        case MC_SCP_MONITOR_MODE:
        case MC_SCP_AUTO_XMT_ON:
        case MC_SCP_AUTO_XMT_OFF:
        default:
            break;
        }

        packet.clear();
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::ReadPortDataUnlocked(0)\n");

    return( 0);
}

/**
 * Method called to extract the next full message packet
 * from a FIFO
 *
 * @param fifo   FIFO to extract the packet from
 * @param packet The packet which was extracted from the fifo
 *
 * @return The number of bytes in the extracted packet
 */
int MultiComProxy::ExtractNextPacket(CommFifo &fifo, SerialString_t &packet)
{
    int         readCount, packetSz;
    int         retVal = 0;
    uint8_t *packetStart = NULL;

    // Lock the FIFO while we are using it
    if( fifo.Lock() == EOK)
    {
        // See how many bytes are in the FIFO
        int fifoSz = fifo.GetSize();

        Log( LOG_DEV_DATA, "\tMultiCom FIFO Size: %d\n", fifoSz);

        if( fifoSz >= 2)
        {
            uint8_t buff[ fifoSz];
            int     ii=0;

            // Read whatever is in the fifo
            if( (readCount = fifo.Peek( &buff[ 0], fifoSz)) == fifoSz)
            {
                // Try to find the beginning of a packet
                while(ii < fifoSz)
                {
                    if( (buff[ ii] != 0x34) && (buff[ ii] != 0xD0) && (buff[ ii] != 0xD1))
                    {
                        Log( LOG_ERRORS, "Invalid packet start byte: $%02hhX @ index %d\n", buff[ ii], ii);
                        ii++;
                    }
                    else
                    {
                        Log( LOG_DEV_DATA, "Possible packet start @ index %d\n", ii);
                        break;
                    }
                }

                if( ii > 0)
                {
                    // Remove garbage at the beginning of the fifo
                    fifo.Erase( ii);
                    // Update size to the remaining bytes
                    fifoSz -= ii;
                }

                packetStart = &buff[ ii];

                // If the FIFO contains enough data for a valid MultiCom packet
                if( fifoSz > 2)
                {
                    // Get the packet size from the MBC byte. The full
                    //  packet will include a CRC byte, the command byte and the data:
                    // buff[1] = 0x00 => packet size = 3 (MBC + MBC + CRC)
                    // buff[1] = 0x01 => packet size = 4 (MBC + MBC + Data[1] + CRC)
                    packetSz = packetStart[ 1] + 1 + 2;

                    Log( LOG_DEV_DATA, "\tMultiCom packet length: %d\n", packetSz);

                    // See if we have a full MultiCom message packet
                    if( fifoSz >= packetSz)
                    {
                        packet = SerialString_t( packetStart, packetSz);
                        retVal = packetSz;

                        // Remove this packet from the FIFO
                        fifo.Erase( packetSz);
                    }
                    else
                    {
#if(LOG_PARTIAL_PACKET == 1)
                        const int tempSz = packetSz;
                        uint8_t tempPacket[ tempSz];

                        // The second byte of the buffer should be the MultiCom header byte
                        if( (readCount = fifo.Peek( tempPacket, tempSz)) == tempSz)
                        {
                            UpdateBusCommLog( ComDirRx, tempPacket, tempSz, NULL);
                        }
#endif
                        Log( LOG_DEV_DATA, "Full packet not received yet (%d of %d bytes)\n", fifoSz, packetSz);
                    }
                }
                else
                {
                    Log( LOG_DETAILED_DATA, "Not enough bytes in FIFO for a valid packet (%d)\n", fifoSz);
                }
            }
            else
            {
                Log( LOG_ERRORS, "Error reading from FIFO\n");
            }
        }

        fifo.Unlock();
    }

    return( retVal);
}

/**
 * Processes a reply from the MultiCom module which contains
 * a "Send Message" ack packet
 *
 * @param rxBuff   Reply message buffer to process (containing the message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the message header)
 */
void MultiComProxy::ProcessAckMessage(SerialString_t &rxBuff, uint32_t buffSize)
{
    int err;

    // Check first to see if we are waiting for an ACK from the translator
    if( CheckForAck( rxBuff) == true)
    {
        Log( LOG_DEV_DATA, "Identified ACK...\n");

        UpdateBusCommLog( ComDirBusAck, rxBuff.c_str(), buffSize+1, NULL);

        Log( LOG_DEV_DATA, "Signal MultiCom ACK sync\n");
        if( (err=m_ackRxd.Acquire()) == EOK)
        {
            int ackCount = m_ackRxd.GetValue();
            m_ackRxd.Signal( ackCount+1);
            m_ackRxd.Release();
            Log( LOG_DEV_DATA, "ACK count = %d\n", ackCount+1);
        }
        else
        {
            Log( LOG_ERRORS, "\tError signalling MultiCom ACK sync: %s\n", strerror( err));
        }
    }
}

/**
 * Processes a reply from the MultiCom module which contains
 * an HBCC "message received" status packet
 *
 * @param rxBuff   Reply message buffer to process (containing the message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the message header)
 */
void MultiComProxy::ProcessHBCCRxPacket(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter MultiComProxy::ProcessHBCCRxPacket()\n");

    // Make sure the message has a valid length
    if( (rxBuff.size() > 7) && (buffSize > 7))
    {
        UpdateBusCommLog( ComDirRx, rxBuff.c_str(), rxBuff.size(), NULL);

        // Check for HBCC errors
        const uint8_t   ir1 = rxBuff[ 2];

        if( ir1 & 0x40) Log( LOG_ERRORS, "HBCC IR1 indicates Receive Error\n");
        if( ir1 & 0x20) Log( LOG_ERRORS, "HBCC IR1 indicates Receiver Overrun\n");
        if( ir1 & 0x10) Log( LOG_ERRORS, "HBCC IR1 indicates Unable to ACK\n");

        // Extract the timestamp
        uint32_t    timeStamp = rxBuff[ 5];
        timeStamp = timeStamp << 4 | rxBuff[ 4];
        timeStamp = timeStamp << 4 | rxBuff[ 3];

        //const uint8_t   mssgNo = rxBuff[ 6];

        // If no errors were detected
        if( (ir1 & (0x40 | 0x20 | 0x10)) == 0)
        {
            // The module response follows
            if( (rxBuff.size() > 8) && (buffSize > 8))
            {
                // Extract the module response and strip off the CRC
                const SerialString_t moduleResp( rxBuff.substr( 7, rxBuff.size()-7-1));
                ProcessVehicleMessage( moduleResp.c_str(), moduleResp.size());
            }
        }
        else
        {
            Log( LOG_ERRORS, "Not processing packet due to HBCC error bits ($%02hhX)\n", ir1);
        }
    }
    else
    {
        Log( LOG_ERRORS, "Invalid HBCC packet size\n");
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::ProcessHBCCRxPacket()\n");
}

/**
 * Processes a reply from the MultiCom module which contains
 * an HBCC "message sent" status packet
 *
 * @param rxBuff   Reply message buffer to process (containing the message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the message header)
 */
void MultiComProxy::ProcessHBCCTxPacket(SerialString_t &rxBuff, uint32_t buffSize)
{
    MultiComIoOcb_t *clientOcb = NULL;
    ModuleId_t      moduleId = INVALID_MODULE_ID;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::ProcessHBCCTxPacket()\n");

    // Grab the module ID from this packet
    try
    {
        // Log communication for this client
        UpdateBusCommLog( ComDirBusAck, rxBuff.c_str(), rxBuff.size(), NULL);

        if( rxBuff.size() > 3)
        {
            // What client is talking to the vehicle
            clientOcb = m_clientBeingAcked;

            // If the multicom status byte indicates successful transmission
            if( rxBuff[ 2] & MC_TX_STATUS_OK)
            {
                // Grab the module ID that the message was sent to
                moduleId = GetModuleIdFromMessage( rxBuff.c_str(), rxBuff.size());

                // Signal the "message sent" flag for this module
                if( clientOcb != NULL)
                {
                    if( (errno=m_messageSent.Acquire()) == EOK)
                    {

                        Log( LOG_DEV_DATA, "Signaling messageSent flag for module $%02hhX in MultiComProxy::ProcessHBCCTxPacket()\n",
                             moduleId);
                        clientOcb->txStatus = EOK;
                        m_messageSent.Signal( MC_MSSG_SENT_ACK);
                        m_messageSent.Release();
                    }
                    else
                    {
                        Log( LOG_ERRORS, "Unable to lock messageSent flag for module $%02hhX in MultiComProxy::ProcessHBCCTxPacket()\n",
                             moduleId);
                    }
                }
                else
                {
                    Log( LOG_ERRORS, "NULL OCB for module $%02hhX in MultiComProxy::ProcessHBCCTxPacket()\n",
                         moduleId);
                }
            }
            else
            {
                std::string mcErr("");
                if( rxBuff[ 2] & MC_TX_STATUS_TX_JABBER)        mcErr += std::string( "Transmit jabber watchdog, ");
                if( rxBuff[ 2] & MC_TX_STATUS_NET_DRVR_WD)  mcErr += std::string( "Network driver watchdog, ");
                if( rxBuff[ 2] & MC_TX_STATUS_LOSS_ARB)     mcErr += std::string( "Loss of arbitration limit exceeded, ");
                if( rxBuff[ 2] & MC_TX_STATUS_TX_ERR_LIMIT) mcErr += std::string( "Transmit error limit exceeded");
                Log( LOG_ERRORS, "Multicom TX error $%02hhX: <%s>\n", rxBuff[ 2], mcErr.c_str());

                if( clientOcb != NULL)
                {
                    if( (errno=m_messageSent.Acquire()) == EOK)
                    {
                        Log( LOG_DEV_DATA, "Signaling messageSent flag for module $%02hhX in MultiComProxy::ProcessHBCCTxPacket()\n",
                             moduleId);

                        clientOcb->txStatus = EIO;
                        m_messageSent.Signal( MC_MSSG_SENT_ACK);
                        m_messageSent.Release();
                    }
                    else
                    {
                        Log( LOG_ERRORS, "Unable to lock messageSent flag to signal error in MultiComProxy::ProcessHBCCTxPacket()\n",
                             moduleId);
                    }
                }
                else
                {
                    Log( LOG_ERRORS, "NULL OCB for module $%02hhX in MultiComProxy::ProcessHBCCTxPacket() error status\n",
                         moduleId);
                }
            }
        }
        else
        {
        }
    }
    catch( BepException &err)
    {
        Log( LOG_ERRORS, "Error processing HBCC Tx Packet: %s\n", err.GetReason());
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::ProcessHBCCTxPacket()\n");
}

/**
 * Checks a message buffer for the MultiCom ACK string. If found,
 * the ACK string is removed from the buffer
 *
 * @param msg    Message buffer to check for an MultiCom ACK
 *
 * @return true if the buffer contained an expected MultiCom ACK string;
 *         false if no MultiCom ACK was expected, or if no ACK was found
 */
bool MultiComProxy::CheckForAck( SerialString_t &msg)
{
    bool    retVal = false;
    int     ackPos;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::CheckForAck(), %d bytes\n",
         msg.length());

    // If this message could be an ACK
    if( msg.length() >= (m_translatorAckString.length()-1))
    {
        // Search the string for the ACK string
        ackPos = (int)msg.find( m_translatorAckString);//(0, ackLen, m_translatorAckString, 0, ackLen);

        // If ACK is at the beginning of the string
        if( ackPos == 0)
        {
            Log( LOG_DEV_DATA, "Found ACK at position %d\n", ackPos);
            // Say ACK was found in the response string
            retVal = true;
        }
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::CheckForAck()\n");

    return( retVal);
}

/**
 * Processes a vehicle reply message
 *
 * @param msg    The buffer containing the vehicle reply message
 * @param size   The number of bytes in the vehicle reply message
 */
bool MultiComProxy::ProcessVehicleMessage( const uint8_t *msg, const uint8_t size)
{
    bool            retVal = false;
    int             status;
    BlockedReader_t client;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::ProcessVehicleMessage(), %d bytes\n",
         size);

    // Check for module reply
    if( IsDiagReplyMsg(msg, size))
    {
        if( (status=m_blockedReaders.Lock()) == EOK)
        {
            if( (status=m_moduleList.Acquire()) == EOK)
            {
                try
                {
                    // Get the ID of the module that is replying
                    ModuleId_t  moduleId = GetModuleIdFromMessage( msg, size);

                    // Say OK to send to this module because we got a reply back
                    SetOkToSend( moduleId, true);

                    // Grab the OCB for the client waiting on the given module
                    MultiComIoOcb_t *clientOcb = m_moduleList[ moduleId].clientOcb;

                    if( clientOcb != NULL)
                    {
                        clientOcb->clientModuleId = moduleId;

                        // Stop the reply timeout timer for this module
                        Log( LOG_DEV_DATA, "Stopping reply timeout timer for module $%X\n", moduleId);

                        // Reset client's reply timeout timer
                        m_moduleList[ moduleId].replyTimeoutTimer.Stop();
                        clientOcb->proxyOcb.replyTimeout = m_defaultReplyTimeout;

                        // Put the rest of the data into the client fifo
                        UpdateClientFifos( (const char*)msg, size, (int)PORT_SUBSCRIBE_RX);

                        // Set a flag to say this module has replied
                        SignalModuleReply( moduleId, true);

                        // Check if anyone is waiting for a response
                        if( m_blockedReaders.FindBlockedReader( clientOcb, client) != false)
                        {
                            // Remove this client from the read blocked list
                            m_blockedReaders.RemoveBlockedReader( clientOcb);

                            int maxLen = (size > client.readMsg.i.nbytes ? client.readMsg.i.nbytes : size);

                            // Erase the data we are sending to the client from his FIFO
                            uint8_t junk[ (const int)maxLen];
                            clientOcb->proxyOcb.rxSubscription->fifo.GetBytes( junk, maxLen);

                            // Reply to the client with the data
                            MsgReply( client.replyId, maxLen, msg, maxLen);

                            // Clear the 'module has replied' flag only if no more data in client's FIFO
                            if( clientOcb->proxyOcb.rxSubscription->fifo.GetSize() == 0)
                            {
                                Log( LOG_DEV_DATA, "Clearing OK to send flag for moduel $%X because FIFO is empty\n",
                                     moduleId);
                                // Say no data received from module yet
                                SignalModuleReply( moduleId, false);
                            }

                            Log( LOG_DEV_DATA, "Responding to blocked client with %d data bytes from module $%X\n",
                                 maxLen, moduleId);
                        }
                        else
                        {
                            Log( LOG_DEV_DATA, "No clients waiting for data from module $%X\n",
                                 moduleId);
                        }
                    }
                    else
                    {
                        // Although this is an error, it is still a valid CAN message. We want
                        //  to return true so the calling routine processes the message buffer
                        //  properly
                        Log( LOG_ERRORS, "\tUnexpected response from module $%X\n", moduleId);
                    }

                    retVal = true;
                }
                catch( BepException &err)
                {
                    Log( LOG_ERRORS, "\tError processing vehicle message: %s\n", err.GetReason());
                }

                m_moduleList.Release();
            }
            else
            {
                Log( LOG_ERRORS, "\tUnable to lock module list: %s\n",
                     strerror( status));
                retVal = false;
            }

            m_blockedReaders.Unlock();
        }
        else
        {
            Log( LOG_ERRORS, "\tUnable to lock blocked client list: %s\n",
                 strerror( status));
            retVal = false;
        }
    }
    else
    {
        Log( LOG_DEV_DATA, "Not a diagnostic reply message (%d bytes)\n", size);
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::ProcessVehicleMessage()\n");

    return( retVal);
}

/**
 * Checks if a message is a valid vehicle reply message
 *
 * @param msg    The buffer containing the vehicle reply message
 * @param size   The number of bytes in the vehicle reply message
 *
 * @return true if the buffer appears to be a valid diagnostic reply message
 */
bool MultiComProxy::IsDiagReplyMsg( const uint8_t *msg, const uint8_t size)
{
    bool retVal = false;

    Log(LOG_FN_ENTRY,"Enter MultiComProxy::IsDiagReplyMsg()\n");

    // If the buffer is valid
    if( (msg != NULL) && (size > 0))
    {
        // If the buffer starts with a diagnostic reply identifier or
        //  a Torque Broadcast message
        if( ( msg[ 0] == 0xC4) || ( msg[ 0] == 0x61))
        {
            retVal = true;
        }
    }

    Log(LOG_FN_ENTRY,"Exit MultiComProxy::IsDiagReplyMsg(%d)\n",(int)retVal);

    return(retVal);
}

/**
 * Handler for MSSG_GAP_TIMER pulses
 *
 * @param pulse  Pulse received on the I/O channel
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t MultiComProxy::HandleMssgGapPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    ModuleId_t      moduleId;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::HandleMssgGapPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    // MultiCom did not respond
    if( pulse.value.sival_int == m_timeoutPulseVal)
    {
        retVal = HandleTimeoutPulse( pulse);
    }
    // If the pulse code is a valid module ID for an inter-message gap event
    else if( pulse.value.sival_int <= INVALID_MODULE_ID)
    {
        // See which module we are timing
        moduleId = pulse.value.sival_int & 0xFFFF;

        Log( LOG_FN_ENTRY, "Handling MSSG_GAP_TIMER for module $%X\n", moduleId);

        // Say that it is OK to send to this module again
        SetOkToSend( moduleId);

        // Reset message gap timer
        m_moduleList[ moduleId].mssgGapTimer.Stop();
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::HandleMssgGapPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    return( retVal);
}

/**
 * Handler for MultiCom timeout error pulses
 *
 * @param pulse  Pulse received on the I/O channel
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t MultiComProxy::HandleTimeoutPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::HandleTimeoutPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    Log( LOG_ERRORS, "\tMultiCom Timeout...resetting MultiCom unit\n");

    // Re-initialize the MultiCom box
    MultiComInit();

    // Reset the MultiCom buffer attributes
    if( m_rxBuffLock.Acquire() == EOK)
    {
        // Purger receive buffer
        m_rawRxBuff.clear();
        // Say not expecting any more data
        m_bytesToCome = 0;

        m_rxBuffLock.Release();
    }
    if( m_respTimeoutTimer.IsTimerArmed())  m_respTimeoutTimer.Stop();

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::HandleTimeoutPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    return( retVal);
}

/**
 * Handler for module reply timeout error pulses
 *
 * @param pulse  Pulse received on the I/O channel
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t MultiComProxy::HandleReplyTimeoutPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    ModuleId_t      moduleId;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::HandleReplyTimeoutPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    // MultiCom did not respond
    if( pulse.value.sival_int == m_timeoutPulseVal)
    {
        HandleTimeoutPulse( pulse);
    }
    else
    {
        // This is a message timeout pulse (module did not respond)
        // See which client to respond to with ETIMEDOUT
        moduleId = pulse.value.sival_int;

        if( moduleId < INVALID_MODULE_ID)
        {
            MultiComIoOcb_t *ocb = m_moduleList[ moduleId].clientOcb;

            Log( LOG_FN_ENTRY, "Handling REPLY_TIMEOUT for module $%X\n", moduleId);

            // Check if the module has actually replied or not
            // Clear the 'module has replied' flag only if no more data in client's FIFO
            if( ocb->proxyOcb.rxSubscription->fifo.GetSize() == 0)
            {
                Log( LOG_DEV_DATA, "Clearing OK to send flag for moduel $%X because FIFO is empty\n",
                     moduleId);
                // Say no data received from module yet
                SignalModuleReply( moduleId, false);
            }
            try
            {
                // If module has not yet replied
                if( IsModuleReplyReady(moduleId) == false)
                {
                    Log( LOG_ERRORS, "\tTimeout waiting for module $%X to respond\n",
                         moduleId);

                    // Tell the client that the read timed out
                    NotifyClientErr( &ocb->proxyOcb, ETIMEDOUT);
                }

                // Remove client from blocked reader list
                m_blockedReaders.RemoveBlockedReader( m_moduleList[ moduleId].clientOcb);
            }
            catch( BepException &err)
            {
            }

            // Unblock any clients waiting to read from this module
            //BroadcastModuleReply( moduleId);

            // Say OK to send to this module again
            SetOkToSend( moduleId, true);

            // Set comm timer to inactive
            if( m_moduleList[ moduleId].replyTimeoutTimer.IsTimerArmed())
            {
                m_moduleList[ moduleId].replyTimeoutTimer.Stop();
            }
        }
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::HandleReplyTimeoutPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    return( retVal);
}

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
int MultiComProxy::ResetConnectionHandler( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    MultiComIoOcb_t *clientOcb = (MultiComIoOcb_t*)ioOcb;
    int             retVal = EOK;
    ModuleId_t      clientModId = clientOcb->clientModuleId;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::ResetConnectionHandler()\n");

    retVal = RawCommProxy::ResetConnectionHandler(ctp, msg, ioOcb);
    Log( LOG_DEV_DATA, "Clearing OK to send flag for moduel $%X because FIFO was reset\n",
         clientModId);
    SignalModuleReply( clientModId, false);

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::ResetConnectionHandler()\n");

    return( retVal);
}

/**
 * Server commanded to reset (re-initialize)
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 *
 * @return EOK if successful, other on error
 */
int MultiComProxy::ResetDriverHandler( resmgr_context_t *ctp, io_devctl_t *msg,
                                       resMgrIoOcb_t *ioOcb)
{
    int retVal = EOK;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::ResetDriverHandler()\n");

    // Do base class suff first (resets fifos)
    if( RawCommProxy::ResetDriverHandler( ctp, msg, ioOcb) == EOK)
    {
        // Re-initialize the MultiCom box
        if( MultiComInit() != true)
        {
            retVal = ECOMM;
        }
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::ResetDriverHandler()\n");

    return( _RESMGR_ERRNO(retVal));
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
 * @return EOK if port successfully locked, other values indicate error
 */
int MultiComProxy::ObtainClientPortLock( CommIoOcb_t *clientOcb, int replyId, bool block)
{
    MultiComIoOcb_t *ocb = (MultiComIoOcb_t*)clientOcb;
    int             retVal;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::ObtainClientPortLock( $%X)\n", ocb->clientModuleId);
    retVal = RawCommProxy::ObtainClientPortLock(clientOcb,replyId,block);
    Log( LOG_FN_ENTRY, "Exit MultiComProxy::ObtainClientPortLock( $%X)\n", ocb->clientModuleId);

    return( retVal);
}

/**
 * Method used to release a port lock for a client
 *
 * @param clientOcb  Client OCB containing flag indicating if he already owns the port lock
 * @param releaseAll Flag to indicate if we should release all locks on the port help by
 *                   the client or just a single lock (set =0 vs. decrement by 1)
 *
 * @return true if port lock released, false if unable to release port lock
 */
bool MultiComProxy::ReleaseClientPortLock( CommIoOcb_t *clientOcb, bool releaseAll)
{
    MultiComIoOcb_t *ocb = (MultiComIoOcb_t*)clientOcb;;
    int             retVal;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::ReleaseClientPortLock( $%X)\n", ocb->clientModuleId);
    retVal = RawCommProxy::ReleaseClientPortLock(clientOcb, releaseAll);
    Log( LOG_FN_ENTRY, "Exit MultiComProxy::ReleaseClientPortLock( $%X)\n", ocb->clientModuleId);

    return( retVal);
}

/**
 * Waits for the module with the given ID to be "send" ready
 *
 * @param moduleId ID of the module to wait for
 *
 * @return true iof we can send to the module, false otherwise
 */
bool MultiComProxy::WaitToSendToModule( ModuleId_t moduleId)
{
    bool                retVal = false;
    int                 status;
    MultiComRxCondVar   &sendCondVar = m_moduleList[ moduleId].canSend;
    MultiComIoOcb_t     *clientOcb;
    uint64_t            waitTime;

    Log( LOG_DEV_DATA, "Wait for module $%X to be send ready( %d)\n", moduleId, sendCondVar.GetValue());

    // Wait for the desired module to be "send" ready
    if( sendCondVar.Acquire() == EOK)
    {
        clientOcb = m_moduleList[ moduleId].clientOcb;
        waitTime = mSEC_nSEC( 120LL);//clientOcb->messageTimeout;

        if( (status=sendCondVar.Wait( true, waitTime)) == EOK)
        {
            Log( LOG_DEV_DATA, "Module $%X is send ready\n", moduleId);

            // Say not ok to send
            sendCondVar.Broadcast( false);

            retVal = true;
        }
        else if( status == ETIMEDOUT)
        {
            Log( LOG_ERRORS, "\tTimeout waiting for module $%X\n", moduleId);
            sendCondVar.Broadcast( true);
        }
        else
        {
            Log( LOG_ERRORS, "\tError waiting for send cond var $%X\n", moduleId);
        }

        sendCondVar.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking module mutex $%X\n", moduleId);
    }

    return( retVal);
}

/**
 * Checks to see if we have received data from a module in response
 * to a message we sent to it
 *
 * @param moduleId ID of the module to check the status of
 *
 * @return true if the module has already replied, false if it has not
 */
bool MultiComProxy::ModuleHasReplied( ModuleId_t moduleId) const
{
    // Get a reference to the read condvar for this module
    MultiComRxCondVar   &dataReady = m_moduleList[ moduleId].dataReady;
    bool                retVal = false;

    // If there is data ready
    if( dataReady.GetValue() == true)
    {
        // Already have data available so read it
        retVal = true;
    }
    return( retVal);
}

/**
 * Sets the okToSend falg for the given module ID
 *
 * @param moduleId Id of the module to set the okToSend flag for
 * @param value    New value for the canSend flag (default = true)
 */
void MultiComProxy::SetOkToSend( const ModuleId_t& moduleId, bool value)
{
    MultiComRxCondVar &sendCondVar = m_moduleList[ moduleId].canSend;

    // Lock condvar for exclusive access
    if( sendCondVar.Acquire() == EOK)
    {
        // If set to not OK to send
        if( sendCondVar.GetValue() != value)
        {
            // Say OK to send now
            sendCondVar.Broadcast( value);
        }

        // Unlock the cond var
        sendCondVar.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking module $%X mutex in MultiComProxy::SetOkToSend\n", moduleId);
    }
}

/**
 * Sets the "MessageSent" flag in the clientOcb to "pending" status
 *
 * @param clientOcb OCB containing the message sent flag to arm
 */
void MultiComProxy::ArmMessageSent( MultiComIoOcb_t *clientOcb)
{
    // Lock condvar for exclusive access
    if( m_messageSent.Acquire() == EOK)
    {
        // Say waiting for MultiCom ACK
        m_messageSent.Signal( MC_MSSG_SENT_PEND);

        // Unlock the cond var
        m_messageSent.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking module $%X mutex in MultiComProxy::ArmMessageSent()\n",
             clientOcb->clientModuleId);
    }
}

/**
 * Clears the "MessageSent" flag in the clientOcb
 *
 * @param clientOcb OCB containing the message sent flag to clear
 */
void MultiComProxy::ClearMessageSent( MultiComIoOcb_t *clientOcb)
{
    // Lock condvar for exclusive access
    if( m_messageSent.Acquire() == EOK)
    {
        // Say OK to send now
        m_messageSent.Signal( MC_NO_MSSG_SENT);

        // Unlock the cond var
        m_messageSent.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking module $%X mutex in MultiComProxy::ClearMessageSent()\n",
             clientOcb->clientModuleId);
    }
}

/**
 * Signals any threads waiting to read data from this module that data
 * is available or a reply timeout occurred
 *
 * @param moduleId Id of the module which timed-out or from which we received data
 */
void MultiComProxy::BroadcastModuleReply( const ModuleId_t &moduleId)
{
    MultiComRxCondVar &dataReady = m_moduleList[ moduleId].dataReady;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::BroadcastModuleReply( $%X)\n", moduleId);

    if( dataReady.Acquire() == EOK)
    {
        if( dataReady.GetValue() == false)
        {
            dataReady.Broadcast( true);
        }
        dataReady.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking module mutex $%X\n", moduleId);
    }
    Log( LOG_FN_ENTRY, "Exit MultiComProxy::BroadcastModuleReply( $%X)\n", moduleId);
}

/**
 * Sets the status of the dataReady flag for the module with the given module id
 *
 * @param moduleId Module ID whose dataReady flag we want to set
 * @param value    New status of the given module's dataReady flag (true or false)
 */
void MultiComProxy::SignalModuleReply( const ModuleId_t moduleId, bool value)
{
    Log( LOG_DEV_DATA, "Setting reply flag for module $%X to %d\n", moduleId, value);
    m_moduleList[ moduleId].dataReady.Acquire();
    m_moduleList[ moduleId].dataReady.Signal( value);
    m_moduleList[ moduleId].dataReady.Release();
}

/**
 * Returns the status of the dataReady flag for the module with the given module id.
 *
 * @param moduleId Module ID whose dataReady flag we want to set
 *
 * @return true if the module with the given module ID has replied, false if it has not replied
 */
bool MultiComProxy::IsModuleReplyReady( const ModuleId_t moduleId) const
{
    bool retVal = false;

    m_moduleList[ moduleId].dataReady.Acquire();
    retVal = ModuleHasReplied(moduleId);
    m_moduleList[ moduleId].dataReady.Release();

    return( retVal);
}

/**
 * Reset the MultiCom box and upload any filter bytes we are holding
 *
 * @return true if initialization was successful, false otherwise
 */
bool MultiComProxy::MultiComInit()
{
    bool    retVal = true;
    int         ii=0;
    const int   retries = 3;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::MultiComInit()\n");

    // If we dont need to register or if we have already registered
    if( m_needToRegister == false)
    {
        // Unblock any clients waiting for vehicle responses with an error message
        m_blockedReaders.Lock();
        BlockedReaderListItr_t  itr;
        for( itr=m_blockedReaders.begin(); itr!=m_blockedReaders.end(); itr++)
        {
            // Tell read-blocked clients that link is down
            MsgError( itr->second.replyId, ENOLINK);
        }
        m_blockedReaders.clear();
        m_blockedReaders.Unlock();

        // Temporarily redirect the serial data notifications to a temporary channel
        //  so we can fully validate the MultiCom response messages
        if( m_threadTid != pthread_self())
        {
            SuspendReaderThread();
        }

        // Try to initialize the board up to 3 times
        //for( ii=0; ii<retries; ii++)
        do
        {
            // Reset our RX fifo
            m_rxFifo->Reset();

            try
            {
                // Reset the MultiCom box
                MultiComReset();

                // Turn on the multicom termination resistors
                MultiComTerminationOn();

                // Upload our node ID to the multicom board
                SetHBCCNodeAddress();

                // Read the multicom 485 firmware version
                Read485Version();

                // Read the multicom SCP firmware version
                ReadSCPVersion();

                // Put the mutlicom SCP interface into non-monitor mode
                SCPNormalMode();

                // If we got here, things went well
                retVal = true;
            }
            catch( BepException &err)
            {
                Log( LOG_ERRORS, "Error initializing MultiCom, attempt %d of %d\n", ii, retries);
                retVal = false;
            }
        } while( (ii++ < retries) && (retVal == false));

        if( m_threadTid != pthread_self())
        {
            ResumeReaderThread();
        }
    }
    else
    {
        Log( LOG_DEV_DATA, "Not Initialized yet..cannot initialize MultiCom\n");
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::MultiComInit()\n");

    return( retVal);
}

/**
 * Performs a hardware restart on the MultiCom box
 */
void MultiComProxy::MultiComReset()
{
    const uint8_t   rawBuff[] = { 0x09, 0x00, 0xD2};
    SerialString_t  mcBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    SerialString_t  mcReply;
    const uint8_t   rawReply[] = { 0xC0, 0x00, 0xE7};
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::MultiComReset()\n");

    try
    {
        // Send Reset message to the MultiCom box
        MultiComMssg( mcBuff, mcReply, MultiComTimeoutVal*100);

        // look for first response packet
        packetIdx = FindMssgPacket( rawReply, sizeof(rawReply), mcReply);

        if( packetIdx != -1)
        {
            Log( LOG_DEV_DATA, "MultiComReset reply OK @ index %d\n", packetIdx);
        }
        else
        {
            throw( BepException( "MultiComReset reply NOT OK"));
        }
    }
    catch( MultiComTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tMultiComTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "MultiComReset: No MultiCom response for %d seconds",
                             MultiComTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::MultiComReset()\n");
}

/**
 * Turns on the MultiCom termination resistors
 */
void MultiComProxy::MultiComTerminationOn()
{
    const uint8_t   rawBuff[] = { 0x52, 0x00, 0x02};
    SerialString_t  mcBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    SerialString_t  mcReply;
    const uint8_t   rawReply[] = { 0x52, 0x00, 0x02};
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::MultiComTerminationOn()\n");
    try
    {
        // Send Reset message to the MultiCom box
        MultiComMssg( mcBuff, mcReply, MultiComTimeoutVal*100);

        // look for first response packet
        packetIdx = FindMssgPacket( rawReply, sizeof(rawReply), mcReply);

        if( packetIdx != -1)
        {
            Log( LOG_DEV_DATA, "MultiComTerminationOn reply OK @ index %d\n", packetIdx);
        }
        else
        {
            throw( BepException( "MultiComTerminationOn reply NOT OK"));
        }
    }
    catch( MultiComTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tMultiComTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "MultiComTerminationOn: No MultiCom response for %d seconds",
                             MultiComTimeoutVal/10));
    }
    Log( LOG_FN_ENTRY, "Exit MultiComProxy::MultiComTerminationOn()\n");
}

/**
 * Sets the Node Address register in the MultiCom HBCC
 */
void MultiComProxy::SetHBCCNodeAddress()
{
    const uint8_t   rawBuff[] = { 0x55, 0x02, 0x10, 0xF3, 0x06};
    SerialString_t  mcBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    SerialString_t  mcReply;
    const uint8_t   rawReply[] = { 0x55, 0x00, 0xFB};
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::SetHBCCNodeAddress()\n");
    try
    {
        // Send Reset message to the MultiCom box
        MultiComMssg( mcBuff, mcReply, MultiComTimeoutVal*100);

        // look for first response packet
        packetIdx = FindMssgPacket( rawReply, sizeof(rawReply), mcReply);

        if( packetIdx != -1)
        {
            Log( LOG_DEV_DATA, "SetHBCCNodeAddress reply OK @ index %d\n", packetIdx);
        }
        else
        {
            throw( BepException( "SetHBCCNodeAddress reply NOT OK"));
        }
    }
    catch( MultiComTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tMultiComTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "SetHBCCNodeAddress: No MultiCom response for %d seconds",
                             MultiComTimeoutVal/10));
    }
    Log( LOG_FN_ENTRY, "Exit MultiComProxy::SetHBCCNodeAddress()\n");
}

/**
 * Requests the 485 firmware revision from the MultiCom box
 */
void MultiComProxy::Read485Version()
{
    const uint8_t   rawBuff[] = { 0x90, 0x00, 0xB9};
    SerialString_t  mcBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    SerialString_t  mcReply;
    const char *verStr = "\0";

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::Read485Version()\n");
    try
    {
        // Send Reset message to the MultiCom box
        MultiComMssg( mcBuff, mcReply, MultiComTimeoutVal*100);

        // Quick check for reply validity
        if( (mcReply[ 0] == rawBuff[ 0]) && (mcReply.size() > 2))
        {
            uint16_t    mssgSz = mcReply[ 1] + 2;
            if( mcReply.size() > mssgSz)
            {
                int verLen = mcReply[ 1];
                Log( LOG_DEV_DATA, "Read485Version reply OK\n");
                m_485Version = SerialString_t( &mcReply[ 2], verLen);
                if( m_485Version.size() > 5)
                {
                    verStr = (const char *)m_485Version.substr( 5).c_str();
                }

                // Response is <MAJOR><MINOR><YR><MONTH><DAY><Description>
                Log( LOG_DEV_DATA, "MultiCom 485 Version: %d.%02d %d/%d/%d <%s>\n",
                     m_485Version[ 0], m_485Version[ 1], m_485Version[ 3], m_485Version[ 4], m_485Version[ 2],
                     verStr);
            }
            else
            {
                throw( BepException( "Read485Version reply NOT OK"));
            }
        }
        else
        {
            throw( BepException( "Incorrect command byte or count in Read485Version reply"));
        }
    }
    catch( MultiComTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tMultiComTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "Read485Version: No MultiCom response for %d seconds",
                             MultiComTimeoutVal/10));
    }
    Log( LOG_FN_ENTRY, "Exit MultiComProxy::Read485Version()\n");
}

/**
 * Requests the SCP firmware revision from the MultiCom box
 */
void MultiComProxy::ReadSCPVersion()
{
    const uint8_t   rawBuff[] = { 0x30, 0x00, 0x05};
    SerialString_t  mcBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    SerialString_t  mcReply;
    const char *verStr = "\0";

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::ReadSCPVersion()\n");
    try
    {
        // Send Reset message to the MultiCom box
        MultiComMssg( mcBuff, mcReply, MultiComTimeoutVal*100);

        // Quick check for reply validity
        if( (mcReply[ 0] == rawBuff[ 0]) && (mcReply.size() > 2))
        {
            uint16_t    mssgSz = mcReply[ 1] + 2;
            if( mcReply.size() > mssgSz)
            {
                int verLen = mcReply[ 1];
                Log( LOG_DEV_DATA, "ReadSCPVersion reply OK\n");
                m_scpVersion = SerialString_t( &mcReply[ 2], verLen);

                if( m_scpVersion.size() > 5)
                {
                    verStr = (const char *)m_scpVersion.substr( 5).c_str();
                }

                // Response is <MAJOR><MINOR><YR><MONTH><DAY><Description>
                Log( LOG_DEV_DATA, "MultiCom SCP Version: %d.%02d %d/%d/%d <%s>\n",
                     m_scpVersion[ 0], m_scpVersion[ 1], m_scpVersion[ 3], m_scpVersion[ 4], m_scpVersion[ 2],
                     verStr);
            }
            else
            {
                throw( BepException( "ReadSCPVersion reply NOT OK"));
            }
        }
        else
        {
            throw( BepException( "Incorrect command byte or count in ReadSCPVersion reply"));
        }
    }
    catch( MultiComTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tMultiComTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "ReadSCPVersion: No MultiCom response for %d seconds",
                             MultiComTimeoutVal/10));
    }
    Log( LOG_FN_ENTRY, "Exit MultiComProxy::ReadSCPVersion()\n");
}

/**
 * Requests the SCP interface to go to normal mode (non-monitor mode)
 */
void MultiComProxy::SCPNormalMode()
{
    const uint8_t   rawBuff[] = { 0x31, 0x00, 0x49};
    SerialString_t  mcBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    SerialString_t  mcReply;

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::SCPNormalMode()\n");
    try
    {
        // Send Reset message to the MultiCom box
        MultiComMssg( mcBuff, mcReply, MultiComTimeoutVal*100);

        // Quick check for reply validity
        if( (mcReply[ 0] == rawBuff[ 0]) && (mcReply.size() > 2))
        {
            Log( LOG_DEV_DATA, "SCPNormalMode reply OK\n");
        }
        else
        {
            throw( BepException( "Incorrect command byte or count in SCPNormalMode reply"));
        }
    }
    catch( MultiComTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tMultiComTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "SCPNormalMode: No MultiCom response for %d seconds",
                             MultiComTimeoutVal/10));
    }
    Log( LOG_FN_ENTRY, "Exit MultiComProxy::SCPNormalMode()\n");
}

/**
 * Sends a message to and gets a reply back from the MultiCom interface box
 *
 * @param mssg     Message to be sent to the MultiCom box
 * @param response Reply from the MultiCom box
 * @param timeOut  Timeout (in milliseconds) for the reply to come back
 */
void MultiComProxy::MultiComMssg( const SerialString_t &mssg, SerialString_t &response, uint32_t timeOut)
{
    uint32_t    packetSize=1;
    Log( LOG_FN_ENTRY, "Enter MultiComProxy::MultiComMssg()\n");

    try
    {
        // Lock RX fifo so ReadPortData() doesnt nab the response data
        if( m_rxBuffLock.Acquire() == EOK)
        {
            if( m_comGuard.Acquire() == EOK)
            {
                // Remove any garbage from port FIFOs
                tcflush( m_comPortFd, TCIOFLUSH);
                m_comGuard.Release();
            }

            timeOut /= 10;

            // Send the command to the MultiCom box
            TransmitMessage( mssg);

            response.clear();

            Log( LOG_DEV_DATA, "Wait for MultiComMssg response\n");

            // Read the AVT response for this message
            while( response.length() < packetSize)
            {
                if( GetMultiComResponse( response) > 0)
                {
                    packetSize = response.size();
                    Log( LOG_DEV_DATA, "Received %d of %d bytes for this packet\n",
                         response.length(), packetSize);
                }
                else
                {
                    Log( LOG_DEV_DATA, "MultiComMssg Stopped sending data at %d of %d bytes\n",
                         response.length(), packetSize);
                    break;
                }
            }

            m_rxBuffLock.Release();
        }
        else
        {
            Log( LOG_ERRORS, "\tError acquiring RX buff lock: %s\n", strerror( errno));
        }
    }
    catch( ...)
    {
        m_rxBuffLock.Release();
        throw;
    }

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::MultiComMssg()\n");
}

/**
 * Waits for a response message from the MultiCom box. The resonse message is put into
 * the response buffer.
 *
 * @param response Buffer where the MultiCom response message string will be put
 *
 * @return Number of bytes received from the MultiCom box
 */
int MultiComProxy::GetMultiComResponse( SerialString_t &response)
{
    int         totalRead=0, bytesRead;
    uint8_t     buff[ 128];
    uint64_t    timeOut = mSEC_nSEC( 5000LL);

    Log( LOG_FN_ENTRY, "Enter MultiComProxy::GetMultiComResponse()\n");

    // Clear the current contents of the FIFO
    m_rxFifo->Reset();

    if( (bytesRead = ReadDriverData( buff, 1, timeOut)) > 0)
    {
        timeOut = mSEC_nSEC( 200LL);
        do
        {
            response += SerialString_t( buff, bytesRead);
            totalRead += bytesRead;
            bytesRead = ReadDriverData( buff, 128, timeOut);
        }
        while( bytesRead > 0);
    }

    // If we did not read any data or if we did not timeout (sometimes timeouts show up as EINTR)
    if( (totalRead == 0) || ((errno != ETIMEDOUT) && (errno != EINTR)) )
    {
        Log( LOG_ERRORS, "\tError waiting for MultiCom response (%d): %s\n", errno, strerror( errno));
    }

    // If we received any data
    if( totalRead > 0)
    {
        PrintSerialString( LOG_DEV_DATA, "\tMultiCom Response", response);
    }
    // No data received...
    else
    {
        Log( LOG_ERRORS, "\tTimeout waiting for MultiCom response\n");
        throw( MultiComTimeoutException( timeOut));
    }

    Log( LOG_FN_ENTRY, "Exit MultiComProxy::GetMultiComResponse()\n");

    return( totalRead);
}

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
int MultiComProxy::FindMssgPacket( const uint8_t *searchMssg, uint32_t mssgLen, const SerialString_t &response)
{
    int retVal = -1;
    int mbc = 0;
    int ii = 0;
    const int mssgSz = response.length();
    const SerialString_t    searchPacket( searchMssg, mssgLen);

    PrintSerialString( LOG_FN_ENTRY, "Enter MultiComProxy::FindMssgPacket()", searchPacket);

    // Process all the bytes in the message
    while( ii < mssgSz)
    {
        // If the response has enough bytes for the MIB and MBC
        if( response.size() > 2)
        {
            // Get the length of the next full packet
            mbc = response[ ii+1];
            /**
             * Check for a full packet.
             *  A full packet will have a length of MBC+3:
             *      1   byte for the MIB
             *      1   byte for the MBC
             *      MBC bytes of data
             *      1   byte for the CRC
             */
            if( mssgSz >= (ii+mbc+3))
            {
                // Evaluate this packet
                const SerialString_t subPacket = response.substr( ii, mbc+3);
                PrintSerialString( LOG_DEV_DATA, "\tNext packet", subPacket);
                // If this is the desired packet
                if( subPacket.find(searchPacket) == 0)
                {
                    Log( LOG_DEV_DATA, "Found MultiCom packet at position %d\n", ii);
                    retVal = ii;
                    break;
                }
                ii += mbc + 3;
            }
            else
            {
                // No more data...sorry
                break;
            }
        }
        else
        {
            break;
        }
    }

    PrintSerialString( LOG_FN_ENTRY, "Exit MultiComProxy::FindMssgPacket()", searchPacket);

    return( retVal);
}

/**
 * Stops all the com timers (reply timeout and message gap timers)
 */
void MultiComProxy::StopComTimers()
{
    int ii;
    m_respTimeoutTimer.Stop();
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

/**
 * Adds a CRC byte to the given multicom data string
 *
 * @param mssg   Multicom data string to add the CRC to
 */
void MultiComProxy::AddCRC( SerialString_t &mssg)
{
    mssg.push_back( CalculateCRC( mssg));
}

/**
 * Checks the CRC in a received multicom data string against a
 * calculated CRC value from the data string body
 *
 * @param mssg   Multicom data string to validate the CRC value for
 *
 * @return true if the CRC is correct, false if it is incorrect
 */
bool MultiComProxy::ValidateCRC( SerialString_t &mssg)
{
    uint8_t crcVal = mssg[ mssg.size() - 1];
    uint8_t actCrc = CalculateCRC(mssg.c_str(), mssg.size()-1);
    bool    retVal = false;

    if( actCrc == crcVal)
    {
        retVal = true;
    }
    else
    {
        Log( LOG_ERRORS, "ERROR: CRC mismatch: $%02hhX != $%02hhX\n", crcVal, actCrc);
    }
    return( retVal);
}

/**
 * Calculates the Multicom CRC bytes for the given data array
 *
 * @param mssg     Data to calculate the CRC for
 *
 * @return Calculated CRC value
 */
uint8_t MultiComProxy::CalculateCRC( const SerialString_t &mssg)
{
    uint8_t accum = 0xFF;
    SerialString_t::const_iterator  itr=mssg.begin();

    if( mssg.empty() == false)
    {
        for( itr=mssg.begin(); itr!=mssg.end(); itr++)
        {
            accum = MultiComProxy::m_crcTable[accum ^ (*itr)];
        }
    }
    else
    {
        Log( LOG_ERRORS, "ERROR: Empty message for CRC calculation\n");
    }

    return( accum);

}

/**
 * Calculates the Multicom CRC bytes for the given data array
 *
 * @param mssg     Data to calculate the CRC for
 * @param mssgSize Number of bytes from mssg to use for the CRC calculation
 *
 * @return Calculated CRC value
 */
uint8_t MultiComProxy::CalculateCRC( const uint8_t *mssg, int mssgSize)
{
    uint8_t accum = 0xFF;
    int     ii;

    if( (mssg != NULL) && ( mssgSize > 0))
    {
        for( ii=0; ii<mssgSize; ii++)
        {
            accum = MultiComProxy::m_crcTable[accum ^ mssg[ ii]];
        }
    }
    else
    {
        Log( LOG_ERRORS, "ERROR: Empty message for CRC calculation\n");
    }

    return( accum);

}

/**
 * Checks to see if the given command string is a broadcast command
 *
 * @param The command string to check
 * @return true if the string is a broadcast command, false otherwise
 */
bool MultiComProxy::IsBroadcastCommand( const SerialString_t &cmd)
{
    bool retVal = false;

    if( (cmd.size() > 2) && (cmd[ 2] == 0xC1))
    {
        retVal = true;
    }

    return( retVal);
}

