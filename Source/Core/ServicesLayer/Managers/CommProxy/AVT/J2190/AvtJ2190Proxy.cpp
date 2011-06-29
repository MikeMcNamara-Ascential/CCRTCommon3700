//******************************************************************************
// Description:
//  Logical serial port driver for J2190 serial communication
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/AVT/J2190/AvtJ2190Proxy.cpp $
// 
// 10    1/18/07 12:49p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added {0x91,0x12} as a valid reply in AvtReset().
// 
// 9     10/26/06 10:18a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the clientOcb->proxyOcb parameter to TransmitMessage().
// 
// 8     4/13/06 3:53a Cward
// cleanup
//
// 3     2/24/06 1:16p Bmeinke
// Removed all references to the USE_READER_THREAD macro
//
// 2     2/15/06 4:03p Bmeinke
// Initialize some uninitialized variables
//
// 7     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 6     6/24/04 1:06a Cward
// Removed pound if 0 blocks. Updated header block comments to the new
// standard.
//
// 5     5/25/04 4:16p Cward
// Locking of the module list was removed in I/O write to allow concurrent
// module comms
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
// 1     12/21/03 6:28p Cward
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 2     6/26/03 12:54p Khykin
// Updated with Brians latest changes.
//
// 4     6/18/03 6:25p Bmeinke
// Fixed typo in OcbFree ( '== EOK' should have been ' != EOK')
// Check the return value from m_blockedReaders.AddBlockedClient() in
// IoRead()
// Need to remove client from blocked reader list before we reply to the
// client in ProcessVehicleMessage()
// Changed the processing inside HandleReplyTimeoutPulse: Lock the blocked
// reader list at beginning and check if the client's module has actually
// replied before sending an error
// Added GetNextClientPacket() method to extract the next full J2190
// message from a client's fifo
//
// 3     5/29/03 1:37p Bmeinke
// New dedicated reader thread architecture
// Added J2190ModuleEntryList class: used to maintain a mapping of module
// ID to J2190ModuleEntry_t structs
// Changed m_moduleList from a plain old C-style array to a
// J2190ModuleEntryList object
// Added more logger output to display a  client's rx subscription ID
// during client transactions
// Remove the OCB mapping for a client who closes his connection and his
// OCB is mapped to an entry in the m_moduleList (i.e. m_moduleList[
// clientId]->clientOcb = NULL) in OcbFree()
// No longer set a client's module ID (in the OCB) to 0 in OcbFinalize()
// Call UpdateBusCommLog() before callin ProcessVehicleMessage() insize
// ProcessValidPacket()
// Removed the call to BroadcastModuleReply()  when a module fails to
// respond (inside HandleReplyTimeoutPulse())
// Changed DetachIoPulse()/CancelDriverNotification() pairs to
// SuspendReaderThread()
// Changed AttachIoPulse()/DriverDataRegister() pairs to
// ResumeReaderThread()
// Aded ObtainClientPortLock()/ReleaseClientPortLock() methods to more
// easily track which clients/modules are locjing the port
//
// 1     4/30/03 4:55p Bmeinke
// Migration to new personalized configuration management structure
//
// 15    4/30/03 3:20p Bmeinke
// Changed the comm timers from J2190ComTimer objects to CommGapTimers
// Added ArmReplyTimeoutTimer() method (overloaded from RawCommProxy)
// Added cond var m_avtAckRxd; used to wait for/signal the reception of an
// AVT ACK string
// Moved the CommIoOcb_t::messageTimeout to
// RawCommProxy::m_defaultReplyTimeout
// ReadSendersBuffer() now takes an additional io_write_t* argument
// Set a module's reply flag to false just before we send a new message to
// the module (in IoWrite)
// Arm the AVT reply timeout timer to 5 seconds bfore we start waiting for
// the ACK to come back (after transmitting a message)
// ReadPortDataUnlocked() no longer modifies its thread priority
// Inter-message gap timing starts when we send a message to a module- not
// when we get the module response (in ProcessVehicleMessage)
// Messagegap timer pulses and reply timeout timer pulses have different
// pulse codes (in HandleMssgGapPulse and HandleReplyTimeoutPulse)
//
// 14    4/07/03 3:12p Bmeinke
// Change the default response timeout value from 120ms to 500ms
// Lock the blocked reader list in IoRead() when checking if a client's
// module has replied
// Remove client from blocked reader list in HandleReplyTimeout()
//
// 13    3/24/03 7:43p Bmeinke
// Changed the order of the calls to UpdateClientFifos() and
// SignalModuleReply() in ProcessVehicleMessage() to avoid a race
// condition
//
// 12    3/10/03 3:10p Bmeinke
// Replaced the call to UploadFilterByte( 0x26, 0x01) with
// m_filterByteList( AddFilterByte( 0x26, 0x10) & RestoreFilterBytes() to
// ensure that the $26 filter byte gets sent to the AVT box when
// performing multiple AvtInit sequences
// Added response validation to the AVT initialization routines
//
// 11    3/06/03 10:44p Bmeinke
// Added overloaded Register() method (called when we receive a
// REGISTER_PULSE)
// Moved the call to AvtInit() from to Register()
// Added a call to DisconnectVPW() to the destructor so that the AVT will
// be disconnected the next time we start up
//
// 10    3/06/03 12:41a Bmeinke
// Removed the OverrideSerialProcessing() and RestoreSerialProcessing()
// methods
// When sending control messages to the AVT box, use the ReadDriverData()
// method instead of using a custom channel and ionotify()
//
// 9     2/21/03 7:06p Bmeinke
// In ProcessValidPacket(), don't analyze the received data buffer until
// at least 2 bytes (AVT header size) have been received
//
// 8     2/06/03 10:08a Bmeinke
// Changed all references to m_blockedReadClients to m_blockedReaders
//
// 7     1/30/03 3:30p Bmeinke
// In PortUnsubscribeHandler(), moved the
// OverrideSerialProcessing()/RestoreSerialProcessing() calls outside the
// for loop in which we delete filter bytes from the AVT box
//
// 6     1/27/03 10:27p Bmeinke
// Override ReadPortConfig() and added ReadDefaultFilterBytes() to read
// the list of default filter bytes to send to the AVT box (temporary work
// around for now)
// Change the default message timeout to 120 mSec in OcbInit
// Interpret the pulse value of reply timeout pulses as module ID's
// instead of OCB pointers
// At the end of PortSubscribeHandler() and PortUnsibscribeHandler() if
// the receive FIFO contains more data than when we started, send a pulse
// back to the I/O channel to say there is AVT data to be processed
// Added OverrideSerialProcessing() and RestoreSerialProcessing() to
// temporarily take over the serial data processing path while processing
// AVT initialization commands and restore the normal serial data
// processing when finished
//
// 5     1/22/03 8:32a Bmeinke
// Removed the printf() inside AvtInit()
//
// 4     1/22/03 3:23a Bmeinke
// Dont explicitly call SetProcessName() in the command line argument
// constructor (let the base class hndel it after it parses the command
// line and can determine what our logical port name is)
// Use new ionotify() scheme where notification pulses from the serial
// driver are delivered directly to our I/O channel instead of to the
// ReaderThread (which is no longer needed)
//
// 3     1/20/03 10:52p Bmeinke
// Modified so as to inherit from new RawCommProxy base class
//
// 2     1/18/03 1:43p Bmeinke
// Initial testing complete using AVT/Skim simulator
//
// 1     1/17/03 7:35p Bmeinke
// Communication proxy for AVT J2190 communication
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "AvtJ2190Proxy.h"

const uint8_t AvtAckString[] = AVT_ACK_STRING;

const uint32_t  AvtTimeoutVal   =   50;

/**
 * Default Constructor
 */
AvtJ2190Proxy::AvtJ2190Proxy() : RawCommProxy(),
    m_avtBytesToCome( 0), m_waitingForAck( false),
    m_translatorAckString( AvtAckString), m_translatorAckTimeout( 100),
    m_avtAckChid( -1), m_avtAckCoid( -1), m_clientBeingAcked( NULL),
    m_avtVersion( 0x00), m_avtTimeoutPulseVal( 0x200)
{
    SetProcessName( AVT_J2190_PROC_NAME);
}

/**
 * Constructor from command line arguments
 *
 * @param argc   Number of command line arguments to aprse
 * @param argv   Array of command line argument strings
 */
AvtJ2190Proxy::AvtJ2190Proxy( int argc, char *argv[]) : RawCommProxy( argc, argv),
    m_avtBytesToCome( 0), m_waitingForAck( false),
    m_translatorAckString( AvtAckString), m_translatorAckTimeout( 100),
    m_avtAckChid( -1), m_avtAckCoid( -1), m_clientBeingAcked( NULL),
    m_avtVersion( 0x00), m_avtTimeoutPulseVal( 0x200)
{
}

/**
 * Class destructor
 */
AvtJ2190Proxy::~AvtJ2190Proxy()
{
    // If we have ben registered (i.e. have opened the serial port)
    if( m_needToRegister == false)
    {
        // Disconnect from the J1850 network until we can establish
        //  our $26 filter byte
        DisconnectVPW();
    }
}

/**
 * Method that is called to command the component to perform any
 * system registration functions necessary.
 *
 * @return Status of the registration process.
 *         Success, Fail
 * @since Version 2.5
 */
const std::string AvtJ2190Proxy::Register(void)
{
    std::string retVal( BEP_FAILURE_RESPONSE);

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::Register()\n");

    // Do base class stuff first
    if( (retVal=RawCommProxy::Register()) == std::string( BEP_SUCCESS_RESPONSE))
    {
        // Setup the AVT box
        if( AvtInit() == true)
        {
            Log( LOG_DEV_DATA, "AVT initialized\n");
        }
        else
        {
            Log( LOG_ERRORS, "\tAVT initialization failed\n");
            retVal = std::string( BEP_FAILURE_RESPONSE);
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tBase class initialization failed\n");
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::Register( %s)\n", retVal.c_str());

    return( retVal);
}

/**
 * Handles I/O mesages
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t AvtJ2190Proxy::IoMssgLoop()
{
    ResManagerCmd_t status = RES_MGR_CMD_NONE;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::IoMssgLoop()\n");

    // Make sure filter byte list is empty
    ResetFilterBytes();

    // Reset and initialize the AVT box
//  AvtInit();
    ResManagerBase::m_resMgrStatus = RES_MGR_CMD_NONE;

    status = RawCommProxy::IoMssgLoop();

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::IoMssgLoop()\n");

    return( status);
}

/**
 * Reads configuration data from the given XML node
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void AvtJ2190Proxy::Initialize(const XmlNode *document)
{
    const XmlString comLogpath( XML_T( "Setup/CommLog"));

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::Initialize(%s)\n", document->getName().c_str());

    // Initialize base class
    RawCommProxy::Initialize( document);

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::Initialize()\n");
}

/**
 * Reads logical port configuration from an XML document
 *
 * @param portNode Node containing setup data for our port only
 */
void AvtJ2190Proxy::ReadPortConfig( const XmlNode *portNode)
{
    try
    {
        // Base class reads majority of the port configuration
        RawCommProxy::ReadPortConfig( portNode);

    }
    catch( ...)
    {
    }
}

/**
 * Reads the list of default filter bytes to upload to the AVT box
 *
 * @param portNode Node containing setup data for our port only
 */
void AvtJ2190Proxy::ReadDefaultFilterBytes( const XmlNode *portNode)
{
    XmlNodeMapCItr      nodeItr;
    const XmlNodeMap&   nodes = portNode->getChild( XML_T( "Setup/FilterBytes"))->getChildren();
    std::string         fbString;
    uint8_t             fbVal, fbPos;

    // Erase anything currently in the list
    m_defaultFilterByteList.clear();

    // Iterate through all the filter byte nodes
    for( nodeItr=nodes.begin(); nodeItr!=nodes.end(); nodeItr++)
    {
        try
        {
            const XmlNode *fbNode = nodeItr->second;

            // Read the filter byte value
            fbString = XmlToAscii( fbNode->getAttribute( XML_T( "value"))->getValue());;
            fbVal = atoh( fbString.c_str());

            // Read the filter byte position
            fbString = XmlToAscii( fbNode->getAttribute( XML_T( "position"))->getValue());;
            fbPos = atoh( fbString.c_str());

            Log( LOG_DEV_DATA, "\tAdding filter byte $%02hhX at position %d\n",
                 fbVal, fbPos);

            // Add this filter byte to the default filter byte list
            m_defaultFilterByteList.AddFilterByte( fbVal, fbPos);
        }
        catch( XmlException &err)
        {
        }
    }
}

/**
 * Perform any server initialization
 *
 * @return true if successful
 *         false if error
 */
bool AvtJ2190Proxy::InitManager()
{
    bool    retVal = false;
    ModuleId_t modId;
    int     state = 0;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::InitManager()\n");

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
            m_avtAckChid = ChannelCreate( 0);
            m_avtAckCoid = ConnectAttach( ND_LOCAL_NODE, 0, m_avtAckChid, _NTO_SIDE_CHANNEL, 0);
            if( (m_avtAckChid == -1) || (m_avtAckCoid == -1))
            {
                Log( LOG_ERRORS, "\tError creating chId/coId for AVT ACK: %s\n", strerror( errno));
                state = -2;
            }
            break;
        case 2:     // Setup the AVT timeout timer to 5 seconds
            if( m_avtTimeoutTimer.Initialize( m_mssgCoid, REPLY_TIMEOUT_TIMER, m_avtTimeoutPulseVal) == -1)
            {
                Log( LOG_ERRORS, "\tError arming AVT reply timeout timer\n");
            }
            break;
        case 3:
            // Initialize the module list
            for(modId=0x00; modId != (ModuleId_t)-1; modId++)
            {
                // Say no response yet
                SignalModuleReply( modId, false);
                // Say OK to send to this module
                SetOkToSend( modId);
            }
            break;
        default:    // Woo-hoo...it all worked !!!
            retVal = true;
            state = -2;
            break;
        }
        state++;
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::InitManager()\n");

    return( retVal);
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void AvtJ2190Proxy::CleanupManager()
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::CleanupManager()\n");

    ConnectDetach( m_avtAckCoid);
    m_avtAckCoid = -1;

    ChannelDestroy( m_avtAckChid);
    m_avtAckChid = -1;

    // Make sure all timers are stopped
    StopComTimers();

    // Cleanup base class
    RawCommProxy::CleanupManager();

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::CleanupManager()\n");
}

/**
 * Method used to check if a serial response string can be added to a
 * client's rx FIFO
 *
 * @param data   Serial string reseived from the port
 * @param ocb    Client connection identifier
 * @return true if the data should be added to the client's FIFO, false otherwise
 */
bool AvtJ2190Proxy::CanAddToClientFifo(const SerialString_t &data, CommIoOcb_t *ocb)
{
    J2190IoOcb_t    *client = (J2190IoOcb_t*)ocb;
    bool            canAdd=false;
    bool            isDiagMssg;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::CanAddToClientFifo()\n");

    // Check with base class first for filter strings
    if( RawCommProxy::CanAddToClientFifo( data, ocb) == true)
    {
        // Check if this is a diagnostic reply message
        isDiagMssg = IsDiagReplyMsg( data.c_str(), data.size());

        // If this is a response to a diag command and
        if( isDiagMssg)
        {
            const ModuleId_t replyModId = data[ 1];

            Log( LOG_DEV_DATA, "Got a diagnostic reply from module $%02hhX\n",
                replyModId);
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

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::CanAddToClientFifo()\n");

    return( canAdd);
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
IOFUNC_OCB_T* AvtJ2190Proxy::OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
{
    J2190IoOcb_t    *j2190Ocb = new J2190IoOcb_t;

    return( (IOFUNC_OCB_T*)j2190Ocb);
}

/**
 * Method used to initialize an allocated OCB structure. Called
 * immediately after the OCB is allocated
 *
 * @param ioOcb  OCB structure to be initialized
 */
void AvtJ2190Proxy::OcbInit(resMgrIoOcb_t *ioOcb)
{
    J2190IoOcb_t    *newOcb = (J2190IoOcb_t*)ioOcb;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::OcbInit()\n");

    RawCommProxy::OcbInit( ioOcb);

    newOcb->clientModuleId = 0xFF;
    newOcb->proxyOcb.replyTimeout = mSEC_nSEC( 500LL);

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::OcbInit()\n");
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
void AvtJ2190Proxy::OcbFree(resMgrIoOcb_t *ocb)
{
    J2190IoOcb_t    *j2190Ocb = (J2190IoOcb_t*)ocb;
    ModuleId_t      clientModId;
    int             err;

    // try to lock the list while we modify the OCB pointer in the m_moduleList
    if( (err=m_moduleList.Acquire()) != EOK)    Log( LOG_ERRORS, "\tError locking module list in OcbFinalize: %s\n",
                                                     strerror( err));

    clientModId = j2190Ocb->clientModuleId;
    if( clientModId != 0xFF)
    {
        if( m_moduleList[ clientModId].clientOcb == j2190Ocb)
        {
            m_moduleList[ clientModId].clientOcb = NULL;
        }
    }

    delete j2190Ocb;

    if( err == EOK) m_moduleList.Release();
}

/**
 * Method used to finalize an allocated OCB structure. Called immediately
 * before the OCB is destroyed
 *
 * @param ioOcb  OCB structure to be finalized
 */
void AvtJ2190Proxy::OcbFinalize(resMgrIoOcb_t *ioOcb)
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::OcbFinalize()\n");

//  J2190IoOcb_t    *oldOcb = (J2190IoOcb_t*)ioOcb;
//  oldOcb->clientModuleId = 0;

    RawCommProxy::OcbFinalize( ioOcb);

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::OcbFinalize()\n");
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
int AvtJ2190Proxy::IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
    J2190IoOcb_t    *clientOcb = (J2190IoOcb_t*)ioOcb;
    CommIoOcb_t     *proxyOcb = &clientOcb->proxyOcb;
    int             retVal = ENOSYS;
    bool            okToNotify = false;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::IoRead(), module=$%02hhX, rx ID=%lld\n",
        clientOcb->clientModuleId, clientOcb->proxyOcb.rxSubscription->id);

    // Get the module ID this client shuld be reading from
    ModuleId_t  clientModId = clientOcb->clientModuleId;

    // IF this is not a "combine" read
    if( (msg->i.xtype & _IO_XTYPE_MASK) == _IO_XTYPE_NONE)
    {
        okToNotify = proxyOcb->rxSubscription->ShouldNotify();
        // Make sure client has performed vaid write or has a valid subscription
        if( (clientModId != 0xFF) || (okToNotify == true) )
        {
            // If this client is not associated with any particular module
            //  and client has a valid subscription
            if( (clientModId == 0xFF) && ( okToNotify == true) )
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

                    Log( LOG_DEV_DATA, "Client requesting %d bytes from module $%02hhX\n",
                         msg->i.nbytes, clientOcb->clientModuleId);
                    // Check to see if we have any data ready to be read
                    if( ModuleHasReplied( clientModId) == true)
                    {
                        Log( LOG_IO_MSSG, "Client's module $%02hhX has replied\n",
                             clientOcb->clientModuleId);

                        // Reset to default reply timeout
                        proxyOcb->replyTimeout = m_defaultReplyTimeout;
                        if( comTimer.IsTimerArmed())      comTimer.Stop();

                        // Let the base class get data from FIFO and reply to client
                        retVal = RawCommProxy::IoRead(ctp,msg,ioOcb);

                        // Clear the 'module has replied' flag only if no more data in client's FIFO
                        if( proxyOcb->rxSubscription->fifo.GetSize() == 0)
                        {
                            Log( LOG_DEV_DATA, "Clearing OK to send flag for moduel $%02hhXX because FIFO is empty\n",
                                 clientModId);
                            SignalModuleReply( clientModId, false);
                        }
                    }
                    // No data and OK for client to block
                    else if( !(proxyOcb->baseOcb.m_ioOcb.ioflag & O_NONBLOCK))
                    {
                        // If reply timer is not active
                        if( (comTimer.IsTimerArmed() == false) && (proxyOcb->replyTimeout > 0))
                        {
                            // Re-start the reply timeout timer
                            if( comTimer.Initialize( m_mssgCoid, REPLY_TIMEOUT_TIMER, clientModId) == -1)
                            {
                                Log( LOG_ERRORS, "\tError arming reply timeout timer for module $%02hhX\n", clientModId);
                            }

                            comTimer.Arm( proxyOcb->replyTimeout);
                            Log( LOG_DEV_DATA, "Re-starting reply message "
                                 "watchdog timer for module $%02hhX for %lld ns\n",
                                 clientModId, proxyOcb->replyTimeout);
                        }
                        else
                        {
                            Log( LOG_DEV_DATA, "Reply watchdog timer for module "
                                 "$%02hhX already running\n", clientModId);
                        }

                        Log( LOG_IO_MSSG, "Adding client $%02hhX to blocked reader list\n",
                             clientOcb->clientModuleId);

                        // Add this client to our "waiting to read" list
                        if( (errno=m_blockedReaders.AddBlockedReader( clientOcb, ctp->rcvid, *msg)) == EOK)
                        {
                            Log( LOG_IO_MSSG, "Client $%02hhX (%lld) added to blocked reader list\n",
                                 clientOcb->clientModuleId, proxyOcb->rxSubscription->id);

                            // Tell OS posix layer to not reply to the client
                            retVal = _RESMGR_NOREPLY;
                        }
                        else
                        {
                            Log( LOG_ERRORS, "Client $%02hhX NOT added to blocked reader list, returning BUSY: %s\n",
                                 clientOcb->clientModuleId, strerror( errno));
                            retVal = EBUSY;
                        }
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
                Log( LOG_IO_MSSG, "Invalid module ID: $%02X\n", clientModId);
            }
            else
            {
                // !!!!!!!!!!!!!!!!!!!!!!!!!!! //
                /// SHOULD NEVER GET IN HERE ////
                // !!!!!!!!!!!!!!!!!!!!!!!!!!! //

                Log( LOG_IO_MSSG, "Invalid something or other, module ID: $%02X\n", clientModId);
            }
            retVal = ENXIO;
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::IoRead(), module=$%02hhX, rx ID=%lld\n",
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
int AvtJ2190Proxy::IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb)
{
    J2190IoOcb_t    *clientOcb = (J2190IoOcb_t*)ioOcb;
    SerialString_t  rawMessage, busMessage;
    int             bytesRead = 0;
    int             retVal = EINVAL;
    ModuleId_t      moduleId = 0x00;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::IoWrite(), moduleId=$%02hhX, rx ID=%lld\n",
         clientOcb->clientModuleId, clientOcb->proxyOcb.rxSubscription->id);

    // Make sure OK for client to write (port not locked)
    if( (CanClientWrite( &clientOcb->proxyOcb) == true) &&
        ((msg->i.xtype & _IO_XTYPE_MASK) == _IO_XTYPE_NONE))
    {
        // Read the message data from the sender
        if( (bytesRead = ReadSendersBuffer(ctp, rawMessage, msg)) > 1)
        {
            // Format the message for the Bus Translator
            busMessage = rawMessage;
            busMessage.insert( busMessage.begin(), 0x00+rawMessage.length());

            PrintSerialString( LOG_SER_COM, "Sending", busMessage);

            // Wait for exclusive access to the AVT box
            if( (errno=m_translatorLock.Acquire()) == EOK)
            {
                // Say no AVT ACK received yet
                if( (retVal=m_avtAckRxd.Acquire()) == EOK)
                {
                    m_avtAckRxd.Signal( false);
                    m_avtAckRxd.Release();
                }
                else
                {
                    m_avtAckRxd.Signal( false);
                    Log( LOG_ERRORS, "\tError clearing AVT ACK sync: %s\n", strerror( retVal));
                }

                // Tell comm logger which client is being acked
                m_clientBeingAcked = clientOcb;

                // Log communication for this serial send (use rawMessage so
                //  byte count not added to comm log)
                UpdateBusCommLog( ComDirTx, rawMessage.c_str(), rawMessage.length(),
                                  &clientOcb->proxyOcb);

                // If this is a broadcast message for the vehicle bus
                if( IsBusBroadcastMsg(rawMessage.c_str(), bytesRead))
                {
                    // Wait for ACK back from bus translator
                    if( m_avtAckLock.Acquire() == EOK)
                    {
                        // Send this message to the vehicle
                        TransmitMessage( busMessage, &(clientOcb->proxyOcb));

                        if( (retVal=WaitForTranslatorAck()) == EOK)
                        {
                            // Tell kernel to update last modification time
                            ioOcb->m_ioOcb.attr->flags |= IOFUNC_ATTR_MTIME |
                                                            IOFUNC_ATTR_DIRTY_TIME;
                        }
                        else
                        {
                            Log( LOG_ERRORS, "\tERROR: AVT Translator DID NOT respond with ACK\n");
                            // Tell client nothing was written
                            bytesRead = 0;
                        }
                        // Release this mutex to unblock the thread which sent us this ACK pulse
                        m_avtAckLock.Release();
                    }
                    else
                    {
                        Log( LOG_ERRORS, "\tError acquiring AVT ACK Lock: %s\n", strerror( errno));
                        bytesRead = 0;
                    }

                    // Advance the offset inside the OCB structure
                    ioOcb->m_ioOcb.offset += bytesRead;

                    // Tell the client how many bytes were written
                    _IO_SET_WRITE_NBYTES( ctp, bytesRead);
                }
                else
                {
                    // See which module we are sending to
                    clientOcb->clientModuleId = rawMessage[ 1];
                    moduleId = clientOcb->clientModuleId;

                        m_moduleList[ moduleId].clientOcb = clientOcb;
                        // Wait until we can send to this module
                        if( WaitToSendToModule( moduleId) == true)
                        {
                            // Wait for ACK back from bus translator
                            if( m_avtAckLock.Acquire() == EOK)
                            {
                                // Say no data received from module yet
                                SignalModuleReply( moduleId, false);

                                // Send this message to the vehicle
                                TransmitMessage( busMessage);

                                // Set the AVT timeout timer
                                m_avtTimeoutTimer.Arm( mSEC_nSEC( 5000LL));
                                // Wait for the AVT ACK to come back (message on the bus)
                                if( (retVal=WaitForTranslatorAck()) == EOK)
                                {
                                    // Retart message gap timer for this module
                                    Log( LOG_DEV_DATA, "Starting message gap timer for module $%02X for %d ms\n", moduleId, 120);
                                    m_moduleList[ moduleId].mssgGapTimer.Initialize( m_mssgCoid, MSSG_GAP_TIMER, moduleId);
                                    m_moduleList[ moduleId].mssgGapTimer.Arm( mSEC_nSEC(120LL));

                                    // Arm the reply timeout timer for this client
                                    ArmReplyTimeoutTimer( (CommIoOcb_t*)clientOcb, true);

                                    // Tell kernel to update last modification time
                                    ioOcb->m_ioOcb.attr->flags |= IOFUNC_ATTR_MTIME |
                                                                    IOFUNC_ATTR_DIRTY_TIME;
                                }
                                else
                                {
                                    Log( LOG_ERRORS, "\tERROR: AVT Translator DID NOT respond with ACK\n");
                                    // Tell client nothing was written
                                    bytesRead = 0;
                                }
                                // Release this mutex to unblock the thread which sent us this ACK pulse
                                m_avtAckLock.Release();
                            }
                            else
                            {
                                Log( LOG_ERRORS, "\tError acquiring AVT ACK Lock: %s\n", strerror( errno));
                                bytesRead = 0;
                            }

                            // Advance the offset inside the OCB structure
                            ioOcb->m_ioOcb.offset += bytesRead;

                            // Tell the client how many bytes were written
                            _IO_SET_WRITE_NBYTES( ctp, bytesRead);
                        }
                }

                // No-one waiting for ack anymore
                m_clientBeingAcked = NULL;

                m_translatorLock.Release();
            }
            else
            {
                Log( LOG_ERRORS, "\tERROR: Unable to lock translator mutex in IoWrite: %s\n",
                     strerror( errno));
                retVal = EIO;
            }
        }
        else if( bytesRead == -1)
        {
            retVal = errno;
        }
        else
        {
            retVal = EINVAL;
        }
    }
    else
    {
        retVal = EPERM;
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::IoWrite(), %d bytes status=%d module=$%02hhX, rx ID=%lld\n",
         bytesRead, retVal, clientOcb->clientModuleId, clientOcb->proxyOcb.rxSubscription->id);

    return( retVal);
}

/**
 * Wait for command ACK's from the bus translator
 *
 * @return EOK if we received the ACK, error code (e.g. ETIMEDOUT) otherwise
 */
int AvtJ2190Proxy::WaitForTranslatorAck()
{
    int             retVal = ETIMEDOUT;
    uint64_t        nsTimeout = mSEC_nSEC( 1500LL);

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::WaitForTranslatorAck()\n");

    int curPrio = RaiseThreadPriority( 2);

    if( (retVal=m_avtAckRxd.Acquire()) == EOK)
    {
        if( (retVal=m_avtAckRxd.Wait( true, nsTimeout)) == EOK)
        {
            // Say no longer waiting for the ACK
            Log( LOG_DEV_DATA, "Got AVT ACK\n");
            retVal = EOK;
        }
        else
        {
            Log( LOG_ERRORS, "\tError waiting for AVT ACK: %s\n", strerror( retVal));
        }

        m_avtAckRxd.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking AVT ACK sync: %s\n", strerror( retVal));
    }

    SetThreadPriority( curPrio);

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::WaitForTranslatorAck()\n");

    return( retVal);
}

/**
 * Arms a reply timeout timer for the client designated by 'ocb'
 *
 * @param clientOcb The OCB of the client we are arming the reply timeout timer for
 * @param forceArm  Flag touse if we want to unconditionally arm the timer (deafults to false)
 * @return 0 if timer was armed, any other value if not armed
 */
int AvtJ2190Proxy::ArmReplyTimeoutTimer( CommIoOcb_t *clientOcb, bool forceArm)
{
    J2190IoOcb_t    *j2190Ocb = (J2190IoOcb_t*)clientOcb;
    int             retVal = 1;
    ModuleId_t      moduleId = j2190Ocb->clientModuleId;
    uint64_t        timeOut;

    // We will call this method with 'forceArm' set to true ONLY after we have
    //  sent a mesage to the AVT and the AVT has replied with an ACK (to tell
    //  us that the message has been sent out on the J1850 bus)
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
            Log( LOG_ERRORS, "\tError arming reply timeout timer for module $%02hhX\n", moduleId);
        }
        comTimer.Arm( timeOut);
        Log( LOG_DEV_DATA, "Starting module $%02X comm timer to %llu ns\n",
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
 * @return The number of bytes received from the port
 */
int AvtJ2190Proxy::ReadPortDataUnlocked(uint8_t *buff, size_t buffSz)
{
    int readCount = 0;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ReadPortDataUnlocked()\n");

    // The first byte of the buffer should be the AVT header byte
    readCount = m_rxFifo.Peek( &buff[ 0], 1);
    m_avtBytesToCome = buff[ 0] & 0x0F;

    // See if we have a full AVT message packet
    while( (m_avtBytesToCome <= (m_rxFifo.GetSize()-1)) && (readCount == 1))
    {
        // Extract this packet
        if( m_rxFifo.GetBytes( buff, m_avtBytesToCome+1) == (int)(m_avtBytesToCome+1))
        {
            m_rawRxBuff = SerialString_t( buff, m_avtBytesToCome+1);

            Log( LOG_DEV_DATA, "\tAVT Bytes: %d\n", m_avtBytesToCome);
            Log( LOG_DEV_DATA, "\tAVT Buff Len: %d\n", m_rawRxBuff.length());
            PrintSerialString( LOG_SER_COM, "AVT Message Data", m_rawRxBuff);

            // Stop the AVT watchdog (have a complete message)
            Log( LOG_DEV_DATA, "\tStopping AVT watchdog timer\n");
            m_avtTimeoutTimer.Stop();

            Log( LOG_DEV_DATA, "\tAVT Command byte: $%02hhX\n", buff[ 0]);

            // Decipher the AVT response
            switch( buff[ 0] & 0xF0)
            {
            case 0x00:
            case 0x10:
                ProcessValidPacket(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0x20:
                ProcessErrorMesage(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0x30:
                ProcessCommandMessageError(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0x40:
                ProcessMatchFunctionMessage(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0x60:
                ProcessOperationalReport(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0x90:
                ProcessBoardStatusMessage(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0xC0:
                if( buff[ 1] == 0x00)   Log( LOG_DEV_DATA, "4X mode disabled\n");
                else                    Log( LOG_DEV_DATA, "4X mode enabled\n");
                break;
            case 0xE0:
                ProcessProcesorMemoryMessage(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0xF0:
                Log( LOG_DEV_DATA, "Block transfer mode status byte\n");
                break;

            case 0x50:
            case 0x70:
            case 0x80:
            case 0xA0:
            case 0xB0:
            case 0xD0:
                // These are reserved command response ID's
                break;
            default:
                break;
            }
        }

        // Check if there is another packet waiting
        readCount = m_rxFifo.Peek( &buff[ 0], 1);
        m_avtBytesToCome = buff[ 0] & 0x0F;
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ReadPortDataUnlocked(%d)\n", readCount);

    return( 0);
}

/**
 * Processes a "valid reply packet" reply from the AVT module
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtJ2190Proxy::ProcessValidPacket( SerialString_t &rxBuff, uint32_t buffSize)
{
    CommIoOcb_t     *commOcb = NULL;
    int             err;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ProcessValidPacket()\n");

    // Log communication for this ACK
    if( m_clientBeingAcked != NULL) commOcb = &m_clientBeingAcked->proxyOcb;

    // Check first to see if we are waiting for an ACK from the translator
    if( CheckForAvtAck( rxBuff) == true)
    {
        Log( LOG_DEV_DATA, "Identified ACK...\n");

        UpdateBusCommLog( ComDirBusAck, rxBuff.c_str(), buffSize+1, commOcb);

        Log( LOG_DEV_DATA, "Signal AVT ACK sync\n");
        if( (err=m_avtAckRxd.Acquire()) == EOK)
        {
            m_avtAckRxd.Signal( true);
            m_avtAckRxd.Release();
        }
        else
        {
            Log( LOG_ERRORS, "\tError signalling AVT ACK sync: %s\n", strerror( err));
        }
    }
    // If the message contains at least a valid vehicle response AVT header
    else if( rxBuff.length() > 2)
    {
        // Extract the vehicle message to process (trim off AVT header bytes)
        SerialString_t  vehicleMssg = rxBuff.substr(2, buffSize-1);

        // Log communication for this client
        UpdateBusCommLog( ComDirRx, vehicleMssg.c_str(), vehicleMssg.length(), commOcb);

        // buffSize indicates the number of bytes following the initial
        //  count byte, so we need t oaccount for the
        // No ACK expected, check if vehicle reply data
        if( ProcessVehicleMessage( vehicleMssg.c_str(), vehicleMssg.length()) == true)
        {
            // Remove count bytes from message
            rxBuff.erase(0,2);
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ProcessValidPacket()\n");
}

/**
 * Processes an "error message" reply from the AVT module
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtJ2190Proxy::ProcessErrorMesage(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ProcessErrorMesage()\n");

    // Log communication for this ACK
    CommIoOcb_t     *commOcb = NULL;
    if( m_clientBeingAcked != NULL) commOcb = &m_clientBeingAcked->proxyOcb;

    if( rxBuff.length() >= buffSize)
    {
        uint8_t errCode = rxBuff[ 1];
        string  errString( "\tAVT Error: \n");

        if( errCode & AVT_STAT_BAD_CRC)     errString += string( "\tBad CRC\n");
        if( errCode & AVT_STAT_NUM_BITS)    errString += string( "\tIncomplete message (incorrect number of bits\n");
        if( errCode & AVT_STAT_BREAK)       errString += string( "\tBreak received\n");
        if( errCode & AVT_STAT_IFR)         errString += string( "\tIFR Data\n");
        if( errCode & AVT_STAT_ARBITRATION) errString += string( "\tLost arbitration\n");
        if( errCode & AVT_STAT_SUCCESS)     errString += string( "\tTransmission successful\n");
        if( errCode & AVT_STAT_DEVICE)      errString += string( "\tFrom this device\n");
        if( errCode & AVT_STAT_BAD_MSSG)    errString += string( "\tBad message (message too short or too long\n");

        Log( errString.c_str());

        // Log communication for this ACK
        UpdateBusCommLog( ComDirErr, rxBuff.c_str(), buffSize+1, commOcb);
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ProcessErrorMesage()\n");
}

/**
 * Processes a "command message error" reply from the AVT module
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtJ2190Proxy::ProcessCommandMessageError(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ProcessCommandMessageError()\n");

    if( rxBuff.length() >= buffSize)
    {
        uint8_t errCode = rxBuff[ 1];
        string  errString( "\tAVT Command Message Error: \n");

        if( errCode & AVT_STAT_BAD_CRC)     errString += string( "\tBad CRC\n");
        if( errCode & AVT_STAT_NUM_BITS)    errString += string( "\tIncomplete message (incorrect number of bits\n");
        if( errCode & AVT_STAT_BREAK)       errString += string( "\tBreak received\n");
        if( errCode & AVT_STAT_IFR)         errString += string( "\tIFR Data\n");
        if( errCode & AVT_STAT_ARBITRATION) errString += string( "\tLost arbitration\n");
        if( errCode & AVT_STAT_SUCCESS)     errString += string( "\tTransmission successful\n");
        if( errCode & AVT_STAT_DEVICE)      errString += string( "\tFrom this device\n");
        if( errCode & AVT_STAT_BAD_MSSG)    errString += string( "\tBad message (message too short or too long\n");

        Log( errString.c_str());

        // Log communication for this ACK
        CommIoOcb_t     *commOcb = NULL;
        if( m_clientBeingAcked != NULL) commOcb = &m_clientBeingAcked->proxyOcb;
        UpdateBusCommLog( ComDirErr, rxBuff.c_str(), buffSize+1, commOcb);
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ProcessCommandMessageError()\n");
}

/**
 * Processes a match function table status message from the AVT box
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtJ2190Proxy::ProcessMatchFunctionMessage(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ProcessMatchFunctionMessage()\n");

    if( rxBuff.length() >= buffSize)
    {
        if( rxBuff[ 0] == 0x40)
        {
            Log( LOG_DEV_DATA, "Match function filtering disabled\n");
        }
        else if( (rxBuff[ 0] == 0x41) && (rxBuff[ 1] == 0xB7))
        {
            Log( LOG_DEV_DATA, "Match table full, entry ignored\n");
        }
        else if( rxBuff[ 0] == 0x42)
        {
            Log( LOG_DEV_DATA, "Match table entry $%02hhX = $%02hhX\n", rxBuff[ 1], rxBuff[ 2]);
        }
        else
        {
            PrintSerialString( LOG_DEV_DATA, "Unknown 0x4X message", m_rawRxBuff);
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ProcessMatchFunctionMessage()\n");
}

/**
 * Processes a "arbitration lost" reply from the AVT module
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtJ2190Proxy::ProcessArbitrationLost(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ProcessArbitrationLost()\n");

    if( rxBuff.length() >= buffSize)
    {
        uint8_t errCode = rxBuff[ 1];
        string  errString( "\tAVT Arbitration Error: \n");

        if( errCode & AVT_STAT_BAD_CRC)     errString += string( "\tBad CRC\n");
        if( errCode & AVT_STAT_NUM_BITS)    errString += string( "\tIncomplete message (incorrect number of bits\n");
        if( errCode & AVT_STAT_BREAK)       errString += string( "\tBreak received\n");
        if( errCode & AVT_STAT_IFR)         errString += string( "\tIFR Data\n");
        if( errCode & AVT_STAT_ARBITRATION) errString += string( "\tLost arbitration\n");
        if( errCode & AVT_STAT_SUCCESS)     errString += string( "\tTransmission successful\n");
        if( errCode & AVT_STAT_DEVICE)      errString += string( "\tFrom this device\n");
        if( errCode & AVT_STAT_BAD_MSSG)    errString += string( "\tBad message (message too short or too long\n");

        Log( errString.c_str());

        // Log communication for this ACK
        CommIoOcb_t     *commOcb = NULL;
        if( m_clientBeingAcked != NULL) commOcb = &m_clientBeingAcked->proxyOcb;
        UpdateBusCommLog( ComDirErr, rxBuff.c_str(), buffSize+1, commOcb);
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ProcessArbitrationLost()\n");
}

/**
 * Processes a reply from the AVT module which indicated an internal AVT error
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtJ2190Proxy::ProcessAvtHwError(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ProcessAvtHwError()\n");

    if( rxBuff.length() >= buffSize)
    {
        uint8_t errCode = rxBuff[ 1];
        string  errString( "\tAVT Internal Error: \n");

        switch( rxBuff[ 1])
        {
        case 0x01:
            Log( "\tAVT HBCC Error $%02hhX: Initialization error - Response not 01\n", errCode);
            break;
        case 0x02:
            Log( "\tAVT HBCC Error $%02hhX: No 'message received OK flag', from loopback test\n", errCode);
            break;
        case 0x03:
            Log( "\tAVT HBCC Error $%02hhX: No 'message transmitted OK flag', from loopback test\n", errCode);
            break;
        case 0x04:
            Log( "\tAVT HBCC Error $%02hhX: Test message byte #1 error, from loopback test\n", errCode);
            break;
        case 0x05:
            Log( "\tAVT HBCC Error $%02hhX: Test message byte #3 error, from loopback test\n", errCode);
            break;
        case 0x06:
            Log( "\tAVT HBCC Error $%02hhX: No IRQ detected\n", errCode);
            break;
        case 0x08:
            Log( "\tAVT HBCC Error $%02hhX: IR1 error, IR1 byte follows\n", errCode);
            break;
        case 0x09:
            Log( "\tAVT HBCC Error $%02hhX: IR2 error, IR2 byte follows\n", errCode);
            break;
        case 0x0A:
            Log( "\tAVT HBCC Error $%02hhX: IR3 error, IR3 byte follows\n", errCode);
            break;
        case 0x0B:
            Log( "\tAVT Internal Error $%02hhX: No buffer available, error bits follow\n", errCode);
            break;
        case 0x0D:
            Log( "\tAVT Internal Error $%02hhX: Transmit message too short\n", errCode);
            break;
        case 0x0E:
            Log( "\tAVT Internal Error $%02hhX: Transmit message too long\n", errCode);
            break;
        case 0x0F:
            Log( "\tAVT SCC Error $%02hhX: FIFO #1 overflow\n", errCode);
            break;
        case 0x31:
            Log( "\tAVT Internal Error $%02hhX: Received a watchdog timeout (AVT-716 PWM mode only)\n", errCode);
            break;

        default:
            Log( "\tAVT Internal Error $%02hhX: Unknown error\n", errCode);
            break;
        }

        Log( errString.c_str());
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ProcessAvtHwError()\n");
}

/**
 * Processes a reply from the AVT module which contains board status information
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtJ2190Proxy::ProcessBoardStatusMessage(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ProcessBoardStatusMessage()\n");

    if( rxBuff.length() >= 2)
    {
        switch( rxBuff[ 1])
        {
        case 0x01: Log( LOG_DEV_DATA, "Command Acknowledged.\n");
            break;
        case 0x02: Log( LOG_DEV_DATA, "SCC initializations complete.\n");
            break;
        case 0x03: Log( LOG_DEV_DATA, "HBCC initializations complete.\n");
            break;
        case 0x04: Log( LOG_DEV_DATA, "Firmware version number follows.\n");
            break;
        case 0x05: Log( LOG_DEV_DATA, "VPW operation (Harris HIP7030A0).\n");
            break;
        case 0x06: Log( LOG_DEV_DATA, "PWM operation (Motorola).\n");
            break;
        case 0x07: Log( LOG_DEV_DATA, "VPW operation (Motorola).\n");
            break;
        case 0x08: Log( LOG_DEV_DATA, "DLC initializations complete.\n");
            break;
        case 0x09: Log( LOG_DEV_DATA, "VPW auto transmit mode (Harris).\n");
            break;
        case 0x0A: Log( LOG_DEV_DATA, "VPW operation (Harris HIP 7010).\n");
            break;
        case 0x0B: Log( LOG_DEV_DATA, "ISO 9141-2 operations.\n");
            break;
        case 0x0C: Log( LOG_DEV_DATA, "FIFOs reset.\n");
            break;
        case 0x0D: Log( LOG_DEV_DATA, "CCD operations.\n");
            break;
        case 0x0E: Log( LOG_DEV_DATA, "GM’s ALDL (8192 UART) operations.\n");
            break;
        case 0x0F: Log( LOG_DEV_DATA, "KeyWord Protocol 2000 operations.\n");
            break;
        case 0x10: Log( LOG_DEV_DATA, "CAN operations.\n");
            break;
        case 0x11: Log( LOG_DEV_DATA, "CAN device reset completed.\n");
            break;
        case 0x12: Log( LOG_DEV_DATA, "Board at startup idle, awaiting mode switch command.\n");
            break;
        case 0x13: Log( LOG_DEV_DATA, "Configuration NVRAM initialized.\n");
            break;
        case 0x14: Log( LOG_DEV_DATA, "UBP operations.\n");
            break;
        case 0x15: Log( LOG_DEV_DATA, "First byte received, time follows, two bytes, 0.5 millisecond resolution. (716 KWP only)\n");
            break;
        case 0x16: Log( LOG_DEV_DATA, "VPW operations with BDLC.\n");
            break;
        case 0x17: Log( LOG_DEV_DATA, "BDLC initialization complete.\n");
            break;
        case 0x18: Log( LOG_DEV_DATA, "LIN pending transmit message purged.\n");
            break;
        case 0x19: Log( LOG_DEV_DATA, "LIN operations (using UBP transceiver).\n");
            break;
        case 0x2A: Log( LOG_DEV_DATA, "FLASH Block is empty.\n");
            break;
        case 0x2B: Log( LOG_DEV_DATA, "FLASH Block has been erased. Flash status follows (1 byte).\n");
            break;
        case 0x2C: Log( LOG_DEV_DATA, "FLASH load program is running; restricted version (FL_PGM) - AVT-820 only.\n");
            break;
        case 0x2D: Log( LOG_DEV_DATA, "FLASH load program is running, regular version (LD_FIFO) - AVT-820 only.\n");
            break;
        case 0x2E: Log( LOG_DEV_DATA, "FLASH load program is running - AVT-718 only.\n");
            break;
        case 0x30: Log( LOG_DEV_DATA, "CHR operations (716 special mode).\n");
            break;
        case 0x31: Log( LOG_DEV_DATA, "PWM operations with ZINGER device.\n");
            break;
        case 0x32: Log( LOG_DEV_DATA, "ZINGER inititalization complete.\n");
            break;
        case 0x34: Log( LOG_DEV_DATA, "Default match table loaded. (AVT-831, 832 only)\n");
            break;
        case 0x35: Log( LOG_DEV_DATA, "EEPROM match table loaded. (AVT-831, 832 only)\n");
            break;
        case 0x00:
        case 0x2F:
        case 0x33:
        case 0x36:
            break;
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ProcessBoardStatusMessage()\n");
}

/**
 * Processes a reply from the AVT module which contains processor memory values
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtJ2190Proxy::ProcessProcesorMemoryMessage(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ProcessProcesorMemoryMessage()\n");

    // If buffer is long enough to contain address and value
    if( rxBuff.length() >= 6)
    {
        Log( LOG_DEV_DATA, "PRocessor Memory: Address $%02hhX%02hhX%02hhX%02hhX; Value: $%02hhX\n",
             rxBuff[ 1], rxBuff[ 2], rxBuff[ 3], rxBuff[ 4], rxBuff[ 5]);
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ProcessProcesorMemoryMessage()\n");
}

/**
 * Processes a reply from the AVT module which contains operational mode information
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtJ2190Proxy::ProcessOperationalReport(SerialString_t &rxBuff, uint32_t buffSize)
{
    float       timerInt;
    uint32_t    usInterval;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ProcessOperationalReport()\n");

    if( rxBuff.length() >= buffSize)
    {
        switch( rxBuff[ 1])
        {
        case 0x04:
            PrintSerialString( LOG_DEV_DATA, "Keep alive message", rxBuff);
            break;
        case 0x05:
            Log( LOG_DEV_DATA, "Keep alive message length=0, suppressed\n");
            break;
        case 0x06:
            if( rxBuff[ 2] == 0x00)     Log( LOG_DEV_DATA, "Disable forwarding network messages from 'this device'\n");
            else                        Log( LOG_DEV_DATA, "Enable forwarding network messages from 'this device'\n");
            break;
        case 0x07:
            Log( LOG_DEV_DATA, "Keep alive interval: %d seconds\n", rxBuff[ 2]);
            break;
        case 0x08:
            if( rxBuff[ 2] == 0x00)     Log( LOG_DEV_DATA, "Message time stamps are disabled\n");
            else                        Log( LOG_DEV_DATA, "Message time stamps are enabled\n");
            break;
        case 0x09:
            if( rxBuff[ 2] == 0x00)     Log( LOG_DEV_DATA, "Network loading messages are disabled\n");
            else                        Log( LOG_DEV_DATA, "Network loading messages are enabled\n");
        case 0x0A:
            usInterval = ((uint32_t)rxBuff[ 2] << 24) | ((uint32_t)rxBuff[ 3] << 16) |
                         ((uint32_t)rxBuff[ 4] << 8) | rxBuff[ 5];
            Log( LOG_DEV_DATA, "Network loading message time timterval: %dus\n", usInterval);
            break;
        case 0x0F:
            Log( LOG_DEV_DATA, "Netwrok loading report: $%02hhX $%02hhX\n", rxBuff[ 2], rxBuff[ 3]);
            break;
        case 0x16:
            if( rxBuff[ 2] == 0x00)     Log( LOG_DEV_DATA, "Timer alert disabled\n");
            else                        Log( LOG_DEV_DATA, "Timer alert enabled\n");
            break;
        case 0x17:
            timerInt = rxBuff[ 2] * 0.0625;
            Log( LOG_DEV_DATA, "Timer alert interval: %7.3f seconds\n", timerInt);
            break;
        case 0x18:
            usInterval = ((uint32_t)rxBuff[ 2] << 24) | ((uint32_t)rxBuff[ 3] << 16) |
                        ((uint32_t)rxBuff[ 4] << 8) | rxBuff[ 5];
            Log( LOG_DEV_DATA, "Timer value: \%d us\n", usInterval);
            break;
        case 0x1F:
            if( rxBuff[ 2] == 0x00)     Log( LOG_DEV_DATA, "Match function control: Match = forward mesage to host\n");
            else                        Log( LOG_DEV_DATA, "Match function control: Match = discard the message\n");
            break;
        case 0x20:
            if( rxBuff[ 2] == 0x00)     Log( LOG_DEV_DATA, "Exit block transfer (transmit) mode\n");
            else                        Log( LOG_DEV_DATA, "Enter block transfer (transmit) mode\n");
            break;
        case 0x21:
            if( rxBuff[ 2] == 0x00)     Log( LOG_DEV_DATA, "Block transfer, transmit, no-echo is disabled\n");
            else                        Log( LOG_DEV_DATA, "Block transfer, transmit, no-echo is enabled\n");
            break;

        default:
            break;
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ProcessOperationalReport()\n");
}

/**
 * Checks a message buffer for the AVT ACK string. If found,
 * the ACK string is removed from the buffer
 *
 * @param msg    Message buffer to check for an AVT ACK
 * @return true if the buffer contained an expected AVT ACK string;
 *         false if no AVT ACK was expected, or if no ACK was found
 */
bool AvtJ2190Proxy::CheckForAvtAck( SerialString_t &msg)
{
    bool    retVal = false;
    int     ackPos;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::CheckForAvtAck(), %d bytes\n",
         msg.length());

    // If this message could be an ACK
    if( msg.length() >= (m_translatorAckString.length()-1))
    {
        // Search the string for the ACK string
        ackPos = msg.find( m_translatorAckString);//(0, ackLen, m_translatorAckString, 0, ackLen);
        // If ACK is at the beginning of the string
        if( ackPos == 0)
        {
            Log( LOG_DEV_DATA, "Found ACK at position %d\n", ackPos);
            // Say ACK was found in the response string
            retVal = true;
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::CheckForAvtAck()\n");

    return( retVal);
}
/**
 * Processes a vehicle reply message
 *
 * @param msg    The buffer containing the vehicle reply message
 * @param size   The number of bytes in the vehicle reply message
 * @return true if the message is a valid vehicle response message, false otherwise
 */
bool AvtJ2190Proxy::ProcessVehicleMessage( const uint8_t *msg, const uint8_t size)
{
    bool            retVal = false;
    int             err;
    ModuleId_t      moduleId = 0xFF;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ProcessVehicleMessage(), %d bytes\n",
         size);

    // Get the ID of the module that is replying
    if( size > 1)   moduleId = msg[ 1];

    // Check for module reply
    if( IsDiagReplyMsg(msg, size))
    {
        if( (err=m_blockedReaders.Lock()) == EOK)
        {
            if( (err=m_moduleList.Acquire()) == EOK)
            {
                // Grab the OCB for the client waiting on the given module
                J2190IoOcb_t    *clientOcb = m_moduleList[ moduleId].clientOcb;

                // Say OK to send to this module because we got a reply back
                SetOkToSend( moduleId, true);

                if( clientOcb != NULL)
                {
                    clientOcb->clientModuleId = moduleId;

                    // Retart message gap timer for this module
                    Log( LOG_DEV_DATA, "Stopping reply timeout timerfor module $%02X\n", moduleId);

                    // Reset client's reply timeout timer
                    m_moduleList[ moduleId].replyTimeoutTimer.Stop();
                    clientOcb->proxyOcb.replyTimeout = m_defaultReplyTimeout;

                        // Set a flag to say this module has replied
                        SignalModuleReply( moduleId, true);

                    // Put the data into the client fifos and respond to any
                    //  clients read-blocked on this mnodule
                    UpdateClientFifos( (const char*)msg, size, (int)PORT_SUBSCRIBE_RX);

                    /**
                     * The call to UpdateClientFifos() calls AddToClientFifo() which
                     * will automatically reply to read blocked clients. We only
                     * need to check if the FIFO for the current client is empty.
                     * If it is, set the module reply flag for the client's module
                     * to fals so that subsequent reads() by this client will return
                     * no data
                     */

                    // Clear the 'module has replied' flag only if no more data in client's FIFO
                    if( clientOcb->proxyOcb.rxSubscription->fifo.GetSize() == 0)
                    {
                        Log( LOG_DEV_DATA, "Clearing OK to send flag for moduel $%02hhXX because FIFO is empty\n",
                             moduleId);
                        // Say no data received from module yet
                        SignalModuleReply( moduleId, false);
                    }
                }
                else
                {
                    // Although this is an error, it is still a valid J2190 message. We want
                    //  to return true so the calling routine processes the message buffer
                    //  properly
                    Log( LOG_ERRORS, "\tUnexpected response from module $%02hhX\n", moduleId);
                }
                m_moduleList.Release();

                retVal = true;
            }
            else
            {
                Log( LOG_ERRORS, "\tUnable to lock module list: %s\n",
                     strerror( err));
                retVal = false;
            }

            m_blockedReaders.Unlock();
        }
        else
        {
            Log( LOG_ERRORS, "\tUnable to lock blocked client list: %s\n",
                 strerror( err));
            retVal = false;
        }

        // Restore priority to original
        //SetThreadPriority( oldPrio);
    }
    // Check for bus broadcast messages
    else if( IsBusBroadcastMsg( msg, size))
    {
        PortSubscrListItr_t rxItr;
        SerialString_t  msgString( msg, size);

        // Set a flag to say this module has replied
        if( size > 1)   SignalModuleReply( moduleId, true);

        // Check if any subscribers for this broadcast data
        for( rxItr=m_rxSubscribers.begin(); rxItr!=m_rxSubscribers.end(); rxItr++)
        {
            J2190IoOcb_t    *ocb = rxItr->second;
            // Get a reference to the client's rx subscription
            LogPortSubscription *sub = ocb->proxyOcb.rxSubscription;

            if( sub != NULL)
            {
                // If the client has a valid subscription
                if( sub->ShouldNotify())
                {
                    // If this is data that the client has subscribed for
                    if( sub->filterList.IsStringValid( msgString))
                    {

#if 0
                        // Add the data to the client's FIFO
                        sub->fifo.AddBytes( msg, size);
                        // Tell the client he has data ready
                        MsgDeliverEvent( sub->coId, &sub->event);
#else
                        /**
                         * Add the data to the client's FIFO. If the client is blocked on a
                         * read() call, this will reply to the client and return 'true'. If the
                         * client is not blocked on a read call, this will simply add the data to
                         * the client's fifo and return false (we must then tell the client that
                         * he has data ready to be read).
                         */
                        if( false == AddToClientFifo( msgString, &ocb->proxyOcb))
                        {
                            // Tell the client he has data ready
                            MsgDeliverEvent( sub->coId, &sub->event);
                            Log( LOG_DEV_DATA, "Notified RX subscriber %lld on CoId %d\n",
                             sub->id, sub->coId);
                    }
#endif
                    }
                }
            }
        }
    }
    else
    {
        Log( LOG_DEV_DATA, "Not a diagnostic reply message ($%02X, %d bytes)\n",
             moduleId, size);
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ProcessVehicleMessage()\n");

    return( retVal);
}

/**
 * Checks if a message is a valid vehicle reply message
 *
 * @param msg    The buffer containing the vehicle reply message
 * @param size   The number of bytes in the vehicle reply message
 * @return true if the buffer appears to be a valid diagnostic reply message
 */
bool AvtJ2190Proxy::IsDiagReplyMsg( const uint8_t *msg, const uint8_t size)
{
    bool retVal = false;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::IsDiagReplyMsg()\n");

    if( size >= J2190_DIAG_REPLY_MIN_SZ)
    {
        if( (msg[ 0] == J2190DIAG_RESP_HDR_BYTE) ||
            ((msg[ 0] >= 0xA0) && (msg[ 0] < 0xF0)) )
        {
            retVal = true;
        }
    }
    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::IsDiagReplyMsg(%d)\n", (int)retVal);

    return( retVal);
}

/**
 * Checks if a message is a broadcast message on the J1850 bus
 *
 * @param msg    The buffer containing the message data
 * @param size   The number of bytes in the message data buffer
 * @return true if the buffer appears to be a bus broadcast message
 */
bool AvtJ2190Proxy::IsBusBroadcastMsg( const uint8_t *msg, const uint8_t size)
{
    bool    retVal = false;

    if( msg[ 0] > 0x3F)
    {
        retVal = true;
    }

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
ResManagerCmd_t AvtJ2190Proxy::HandleMssgGapPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    ModuleId_t      moduleId;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::HandleMssgGapPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    // AVT did not respond
    if( pulse.value.sival_int == m_avtTimeoutPulseVal)
    {
        retVal = HandleAvtTimeoutPulse( pulse);
    }
    // If the pulse code is a valid module ID for an inter-message gap event
    else if( pulse.value.sival_int <= 0xFF)
    {
        // See which module we are timing
        moduleId = pulse.value.sival_int & 0xFF;

        Log( LOG_FN_ENTRY, "Handling MSSG_GAP_TIMER for module $%02hhX\n", moduleId);

        // Say that it is OK to send to this module again
        SetOkToSend( moduleId);

        // Reset message gap timer
        m_moduleList[ moduleId].mssgGapTimer.Stop();
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::HandleMssgGapPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    return( retVal);
}

/**
 * Handler for AVT timeout error pulses
 *
 * @param pulse  Pulse received on the I/O channel
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t AvtJ2190Proxy::HandleAvtTimeoutPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::HandleAvtTimeoutPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    Log( LOG_ERRORS, "\tAVT Timeout...resetting AVT unit\n");

    // Re-initialize the AVT box
    AvtInit();

    // Reset the AVT buffer attributes
    if( m_rxBuffLock.Acquire() == EOK)
    {
        // Purger receive buffer
        m_rawRxBuff.clear();
        // Say not expecting any more data
        m_avtBytesToCome = 0;

        m_rxBuffLock.Release();
    }
    if( m_avtTimeoutTimer.IsTimerArmed())   m_avtTimeoutTimer.Stop();

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::HandleAvtTimeoutPulse(%d,%d)\n",
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
ResManagerCmd_t AvtJ2190Proxy::HandleReplyTimeoutPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    ModuleId_t      moduleId = pulse.value.sival_int;
    BlockedReader_t client;
    SerialString_t  replyMssg;
    int             maxLen;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::HandleReplyTimeoutPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    // AVT did not respond
    if( pulse.value.sival_int == m_avtTimeoutPulseVal)
    {
        HandleAvtTimeoutPulse( pulse);
    }
    else if( m_moduleList[ moduleId].clientOcb != NULL)
    {
        // This is a message timeout pulse (module did not respond)
        // See which client to respond to with ETIMEDOUT
        J2190IoOcb_t *ocb = m_moduleList[ moduleId].clientOcb;

        Log( LOG_FN_ENTRY, "Handling REPLY_TIMEOUT for module $%02hhX\n", moduleId);
        Log( LOG_ERRORS, "\tTimeout waiting for module $%02hhX to respond\n",moduleId);

        try
        {
            if( (errno=m_blockedReaders.Lock()) == EOK)
            {
                // If client is blocked waiting for a reply
                if( m_blockedReaders.FindBlockedReader( ocb, client) == true)
                {
                    // If module has not yet replied
                    if( IsModuleReplyReady(moduleId) == false)
                    {
                        // Tell the client that the read timed out
                        NotifyClientErr( client, ETIMEDOUT);
                    }
                    // Module has replied
                    else
                    {
                        // Say no data received from module yet
                        SignalModuleReply( moduleId, false);

                        // Get next full message packet from client's fifo
                        if( (maxLen=GetNextClientPacket(ocb, replyMssg)) > 0)
                        {
                            // Determine the max number of bytes to reply with
                            maxLen = (maxLen > client.readMsg.i.nbytes ? client.readMsg.i.nbytes : maxLen);
                            // Reply to the client with the data
                            MsgReply( client.replyId, maxLen, replyMssg.c_str(), maxLen);

                            Log( LOG_DEV_DATA, "Responding to blocked client with %d data bytes from module $%02hhX\n",
                                 maxLen, moduleId);
                            PrintSerialString( LOG_DEV_DATA, "Response String", replyMssg);
                        }
                        else
                        {
                            Log( LOG_ERRORS, "\tERROR: Module responded, but no data in client's fifo\n");
                            NotifyClientErr( client, ETIMEDOUT);
                        }
                    }
                    // Remove client from blocked reader list
                    m_blockedReaders.RemoveBlockedReader( ocb);
                }
                // Reply timeout and client not waiting for data
                else
                {
                    Log (LOG_DEV_DATA, "Client for module $%02hhX is not in blocked reader list\n",
                         moduleId);
                }

                m_blockedReaders.Unlock();
            }
            else
            {
            }
        }
        catch( BepException &err)
        {
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::HandleReplyTimeoutPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

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
int AvtJ2190Proxy::PortSubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                        resMgrIoOcb_t *ioOcb)
{
    logPortMsg_t            *portMsg = (logPortMsg_t*)_DEVCTL_DATA( msg->i);
    logPortSubscribe_t      &subMsg = portMsg->portSubscribe;
    J2190IoOcb_t            *clientOcb = (J2190IoOcb_t*)ioOcb;
    int                     retVal;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::PortSubscribeHandler(), size = %d\n", ctp->size);

    // We need to upload a filter byte for this particular subscription string
    if( (retVal = pthread_mutex_lock( &m_dataLock)) == EOK)
    {
        // If subscribing for received data
        //  and  not already subscribed for RX data
        //  and the client is providing a filter string
        //  and the filter string is valid
        if( (subMsg.flags & PORT_SUBSCRIBE_RX) &&
            (clientOcb->proxyOcb.rxSubscription->ShouldNotify() == false) &&
            (subMsg.flags & PORT_NOTIFY_FILTER) &&
            (subMsg.filterByteCount > 0))
        {
            LogPortFilterByte   filterByte = subMsg.filterString[ 0];
            uint8_t             fbVal = (uint8_t)filterByte;

            // If client not already using this filter byte
            if( clientOcb->filterByteList.AddFilterByte(fbVal, 1) == 1)
            {
                try
                {
                    // We need to handle the response to the upload filter byte command
                    //  directly
                    SuspendReaderThread();

                    // Add this filter byte
                    UploadFilterByte( fbVal, 1);

                }
                catch( BepException &err)
                {
                    retVal = EIO;
                }
                // Restore normal serial data processing
                ResumeReaderThread();
            }
        }

        pthread_mutex_unlock( &m_dataLock);

        retVal = RawCommProxy::PortSubscribeHandler(ctp,msg,ioOcb);
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking data in PortSubscribeHandler: %s\n",
             strerror( retVal));
        retVal = EBUSY;
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::PortSubscribeHandler()\n");

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
int AvtJ2190Proxy::PortUnsubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                          resMgrIoOcb_t *ioOcb)
{
    logPortMsg_t            *portMsg = (logPortMsg_t*)_DEVCTL_DATA( msg->i);
    logPortUnsubscribe_t    &clientMsg = portMsg->portUnsubscribe;
    J2190IoOcb_t            *clientOcb = (J2190IoOcb_t*)ioOcb;
    int                     retVal;
    AvtFltrByteList         &filterByteList = clientOcb->filterByteList;
    LogPortFilterStringList &filterList = clientOcb->proxyOcb.rxSubscription->filterList;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::PortUnsubscribeHandler()\n");

    // Need to lock this operation to avoid duplicate subscription ID's
    if( (retVal = pthread_mutex_lock( &m_dataLock)) == EOK)
    {
        Log( LOG_DEV_DATA, "Client subscription masks: $%04X\t$%04X\n",
             clientOcb->proxyOcb.subMask, clientMsg.flags);

        // Clear out mask bits for the events being unsubscribed for
        clientOcb->proxyOcb.subMask &= ~(PORT_SUBSCRIBE_RX);

        // If unsubscribing for a filtered subscription
        if( (clientMsg.flags & PORT_SUBSCRIBE_RX) &&
            (clientOcb->proxyOcb.subMask & PORT_NOTIFY_FILTER))
        {
            // Clear filter mask bit
            clientOcb->proxyOcb.subMask &= ~(PORT_NOTIFY_FILTER);

            try
            {
                // We need to handle the response to the upload filter byte command
                //  directly
                SuspendReaderThread();

                // Walk through and delete each of the client's filter bytes
                for( register uint32_t ii=0; ii<filterByteList.size(); ii++)
                {
                    AvtFilterByte &fb = filterByteList[ ii];
                    uint8_t fbVal, fbPos;

                    // Get filter byte value and position
                    fb.GetFbValAndPos(fbVal,fbPos);

                    // Delete this filter byte
                    DelAvtFilterByte( fbVal ,fbPos);
                }
            }
            catch( BepException &err)
            {
                retVal = EIO;
            }

            ResumeReaderThread();

            // Clear this client's filter byte list
            filterByteList.clear();

            // Clear filter strings for this client
            filterList.clear();

            // Remove this subscriber for real
            m_rxSubscribers.DelSubscriber( clientMsg.handle);
        }
        else
        {
            Log( LOG_DEV_DATA, "Subscription is not a filter subscription\n");
            retVal = EOK;
        }
        pthread_mutex_unlock( &m_dataLock);

        // If OK to continue with unsubscribe request
        if( retVal == EOK)
        {
            retVal = RawCommProxy::PortUnsubscribeHandler(ctp,msg,ioOcb);
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking data in PortUnsubscribeHandler: %s\n",
             strerror( retVal));
        retVal = EBUSY;
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::PortUnsubscribeHandler()\n");

    return( retVal);
}

/**
 * Server commanded to reset (re-initialize)
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int AvtJ2190Proxy::ResetDriverHandler( resmgr_context_t *ctp, io_devctl_t *msg,
                                       resMgrIoOcb_t *ioOcb)
{
    int retVal = EOK;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ResetDriverHandler()\n");

    // Do base class suff first (resets fifos)
    if( RawCommProxy::ResetDriverHandler( ctp, msg, ioOcb) == EOK)
    {
        // Re-initialize the AVT box
        if( AvtInit() != true)
        {
            retVal = ECOMM;
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ResetDriverHandler()\n");

    return( _RESMGR_ERRNO(retVal));
}

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
int AvtJ2190Proxy::ObtainClientPortLock( CommIoOcb_t *clientOcb, int replyId, bool block)
{
    J2190IoOcb_t    *j2190Ocb = (J2190IoOcb_t*)clientOcb;;
    int retVal;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ObtainClientPortLock( $%02hhX)\n", j2190Ocb->clientModuleId);
    retVal = RawCommProxy::ObtainClientPortLock(clientOcb,replyId,block);
    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ObtainClientPortLock( $%02hhX)\n", j2190Ocb->clientModuleId);

    return( retVal);
}

/**
 * Method used to release a port lock for a client
 *
 * @param clientOcb Client OCB containing flag indicating if he already owns the port lock
 * @param releaseAll Flag to indicate if we should release all locks on the port help by
 *                   the client or just a single lock (set =0 vs. decrement by 1)
 * @return true if port lock released, false if unable to release port lock
 */
bool AvtJ2190Proxy::ReleaseClientPortLock( CommIoOcb_t *clientOcb, bool releaseAll)
{
    J2190IoOcb_t    *j2190Ocb = (J2190IoOcb_t*)clientOcb;;
    int retVal;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ReleaseClientPortLock( $%02hhX)\n", j2190Ocb->clientModuleId);
    retVal = RawCommProxy::ReleaseClientPortLock(clientOcb, releaseAll);
    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ReleaseClientPortLock( $%02hhX)\n", j2190Ocb->clientModuleId);

    return( retVal);
}

/**
 * Extracts the next full J2190 message packet from a client's fifo
 *
 * @param ocb    OCB of the client
 * @param mssg   The next full packet contained in the client's fifo
 *
 * @return Number of bytes read from the FIFO or -(errno) if an error occurred
 */
int AvtJ2190Proxy::GetNextClientPacket( J2190IoOcb_t *ocb, SerialString_t &mssg)
{
    CommFifo    &fifo = ocb->proxyOcb.rxSubscription->fifo;
    int         fifoSz;
    int         packetSz = 0;

    // Module has replied, so send data to client
    if( (errno=fifo.Lock()) == EOK)
    {
        // Extract first full packet from the client's fifo
        if( (fifoSz = fifo.GetSize()) > 0)
        {
            const uint32_t  size = fifoSz+1;
            uint8_t         replyMsg[ size];

            // See how big the packet is
            fifo.Peek( &replyMsg[0], 1);
            packetSz = replyMsg[0]+1;

            // Extract the packet from the buffer
            fifo.GetBytes( replyMsg, packetSz);
            mssg = SerialString_t( replyMsg, packetSz);
        }
        fifo.Unlock();
    }
    else
    {
        packetSz = -errno;
        Log( LOG_ERRORS, "\tError locking fifo: %s\n", strerror( errno));
    }

    return( packetSz);
}

/**
 * Waits for the module with the given ID to be "send" ready
 *
 * @param moduleId ID of the module to wait for
 * @return true iof we can send to the module, false otherwise
 */
bool AvtJ2190Proxy::WaitToSendToModule( uint8_t moduleId)
{
    bool            retVal = false;
    int             status;
    J2190CondVar    &sendCondVar = m_moduleList[ moduleId].canSend;
    J2190IoOcb_t    *clientOcb;
    uint64_t        waitTime;

    Log( LOG_DEV_DATA, "Wait for module $%02X to be send ready( %d)\n", moduleId, sendCondVar.GetValue());

    // Wait for the desired module to be "send" ready
    if( sendCondVar.Acquire() == EOK)
    {
        clientOcb = m_moduleList[ moduleId].clientOcb;
        waitTime = mSEC_nSEC( 120LL);//clientOcb->messageTimeout;

        if( (status=sendCondVar.Wait( true, waitTime)) == EOK)
        {
            Log( LOG_DEV_DATA, "Module $%02X is send ready\n", moduleId);

            // Say not ok to send
            sendCondVar.Broadcast( false);

            retVal = true;
        }
        else if( status == ETIMEDOUT)
        {
            Log( LOG_ERRORS, "\tTimeout waiting for module $%02X\n", moduleId);
            sendCondVar.Broadcast( true);
        }
        else
        {
            Log( LOG_ERRORS, "\tError waiting for send cond var $%02X\n", moduleId);
        }

        sendCondVar.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking module mutex $%02X\n", moduleId);
    }

    return( retVal);
}

/**
 * Checks to see if we have received data from a module in response
 * to a message we sent to it
 *
 * @param moduleId ID of the module to check the status of
 * @return true if the module has already replied, false if it has not
 */
bool AvtJ2190Proxy::ModuleHasReplied( ModuleId_t moduleId) const
{
    // Get a reference to the read condvar for this module
    J2190CondVar    &dataReady = m_moduleList[ moduleId].dataReady;
    bool            retVal = false;

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
void AvtJ2190Proxy::SetOkToSend( const ModuleId_t& moduleId, bool value)
{
    J2190CondVar &sendCondVar = m_moduleList[ moduleId].canSend;

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
        Log( LOG_ERRORS, "\tError locking module $%02X mutex in AvtJ2190Proxy::SetOkToSend\n", moduleId);
    }
}

/**
 * Signals any threads waiting to read data from this module that data
 * is available or a reply timeout occurred
 *
 * @param moduleId Id of the module which timed-out or from which we received data
 */
void AvtJ2190Proxy::BroadcastModuleReply( const ModuleId_t &moduleId)
{
    J2190CondVar &dataReady = m_moduleList[ moduleId].dataReady;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::BroadcastModuleReply( $%02X)\n", moduleId);

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
        Log( LOG_ERRORS, "\tError locking module mutex $%02X\n", moduleId);
    }
    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::BroadcastModuleReply( $%02X)\n", moduleId);
}

/**
 * Sets the status of the dataReady flag for the module with the given module id
 *
 * @param moduleId
 *               Module ID whose dataReady flag we want to set
 * @param value  New status of the given module's dataReady flag (true or false)
 */
void AvtJ2190Proxy::SignalModuleReply( const uint8_t moduleId, bool value)
{
    Log( LOG_DEV_DATA, "Setting reply flag for module $%02X to %d\n", moduleId, value);
    m_moduleList[ moduleId].dataReady.Acquire();
    m_moduleList[ moduleId].dataReady.Signal( value);
    m_moduleList[ moduleId].dataReady.Release();
}

/**
 * Returns the status of the dataReady flag for the module with the given module id.
 *
 * @param moduleId
 *                 Module ID whose dataReady flag we want to set
 * @return true if the module with the given module ID has replied, false if it has not replied
 */
bool AvtJ2190Proxy::IsModuleReplyReady( const uint8_t moduleId) const
{
    bool retVal = false;

    m_moduleList[ moduleId].dataReady.Acquire();
    retVal = ModuleHasReplied(moduleId);
    m_moduleList[ moduleId].dataReady.Release();

    return( retVal);
}

/**
 * Reset the AVT box and upload any filter bytes we are holding
 *
 * @return true if initialization was successful, false otherwise
 */
bool AvtJ2190Proxy::AvtInit()
{
    bool    retVal = true;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::AvtInit()\n");

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

        try
        {
            // Temporarily redirect the serial data notifications to a temporary channel
            //  so we can fully validate the AVT response messages
            SuspendReaderThread();

            // Disconnect from the J1850 network until we can establish
            //  our $26 filter byte
            DisconnectVPW();

            // Read the firmware version from the AVT box
            ReadAvtVersion();

            // Reset the AVT box
            AvtReset();

            // Disconnect from the J1850 network until we can establish
            //  our $26 filter byte (in case firmware version is one
            //  that comes up connected to the bus)
            DisconnectVPW();

            // Make sure we are in VPW mode
            SetAvtVPW();

            // ALWAYS upload the $26 filter byte for diagnostic responses
            m_filterByteList.AddFilterByte( 0x26, 0x01);
            RestoreFilterBytes();

            // Re-connect to the vehicle network after initialization
            ConnectVPW();
        }
        catch( BepException &err)
        {
            retVal = false;
            Log( LOG_ERRORS, "\tError initializing AVT box: %s\n",
                 err.GetReason());
        }

        ResumeReaderThread();
    }
    else
    {
        Log( LOG_DEV_DATA, "Not Initialized yet..cannot initialize AVT\n");
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::AvtInit()\n");

    return( retVal);
}

/**
 * Performs a hardware restart on the AVT box
 */
void AvtJ2190Proxy::AvtReset()
{
    const uint8_t   rawBuff[] = { 0xF1, 0xA5};
    SerialString_t  avtBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    SerialString_t  avtReply;
    const uint8_t   rawReply[5][2] =  {{ 0x91, 0x05},{ 0x91, 0x07},{ 0x91, 0x09},{ 0x91, 0x0A}, { 0x91, 0x12}};
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::AvtReset()\n");

    try
    {
        // Send Reset message to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);
        for( int ii=0; ii<5 && packetIdx==-1; ii++)
        {
            packetIdx = FindAvtPacket( rawReply[ ii], sizeof( rawReply[ ii]), avtReply);
        }

        if( packetIdx != -1)
        {
            Log( LOG_DEV_DATA, "AvtReset reply OK @ index %d\n", packetIdx);
        }
        else
        {
            Log( LOG_ERRORS, "\tAvtReset reply NOT OK\n");
        }
    }
    catch( AvtTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tAvtTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "AvtReset: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::AvtReset()\n");
}

/**
 * Clears all filter bytes from the AVT box
 */
void AvtJ2190Proxy::ClearAvtFilterBytes()
{
    const uint8_t   rawBuff[] = { 0x31, 0x7B};
    SerialString_t  avtBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    const uint8_t   rawReply[] = { 0x40};
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ClearAvtFilterBytes()\n");

    try
    {
        // Send message to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);
        if( (packetIdx=FindAvtPacket( rawReply, sizeof( rawReply), avtReply)) != -1)
        {
            Log( LOG_DEV_DATA, "Filter bytes cleared\n");
        }
        else
        {
            Log( LOG_ERRORS, "\tError in AVT ClearFilterBytes; response: $%02hhX\n",
                 avtReply[ 0]);
        }
    }
    catch( AvtTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tAvtTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "ClearAvtFilterBytes: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ClearAvtFilterBytes()\n");
}

/**
 * Uploads a filter byte to the AVT box.
 *
 * @param filterByte The position of the filter in incoming vehicle messages
 * @param position   The value of the filter byte
 */
void AvtJ2190Proxy::UploadFilterByte( uint8_t filterByte, uint8_t position)
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::UploadFilterByte(%d, $%02X)\n", filterByte, position);

    // If this is the first time to add this filter byet
    if( m_filterByteList.AddFilterByte( filterByte, position) == 1)
    {
        // Send filter byte to the AVT box
        SetAvtFilterByte( filterByte, position);
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::UploadFilterByte(%d, $%02X)\n", filterByte, position);
}

/**
 * Removes a filter byte from the AVT box's list
 *
 * @param filterByte Filter byte to be removed to the AVT box
 * @param position   Position of the filter byte to be removed
 */
void AvtJ2190Proxy::DelAvtFilterByte( uint8_t filterByte, uint8_t position)
{
    unsigned status;

    // Need to lock this operation to avoid duplicate subscription ID's
    if( pthread_mutex_lock( &m_dataLock) == EOK)
    {
        // Dont remove the default diag reply byte
        if( (filterByte != 0x26) || (position != 1))
        {
            // If no more references to this filter byte
            status = m_filterByteList.DelFilterByte(filterByte, position);
            if( status == 0)
            {
                Log( LOG_DEV_DATA, "\tNo more references to filter byte $%02hhX @ %d\n",
                     filterByte, position);

                // Disconnect so no garbage
                DisconnectVPW();

                // erase all AVT box filter bytes (from AVT box itself)
                ClearAvtFilterBytes();

                // Restore the remaining
                RestoreFilterBytes();

                // Reconnect after filter bytes restored
                ConnectVPW();
            }
            else
            {
                Log( LOG_DEV_DATA, "\tFilter byte $%02hhX @ %d has %d references remaining\n",
                     filterByte, position, status);
            }
        }

        pthread_mutex_unlock( &m_dataLock);
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking mutex in DelAvtFilterByte( $%02hhX, %d)\n",
             filterByte, position);
    }
}

/**
 * Sends a filter byte to the AVT box
 *
 * @param filterByte Filter byte to be uploaded to the AVT box
 * @param position   Position at which to add the filter byte
 */
void AvtJ2190Proxy::SetAvtFilterByte( uint8_t filterByte, uint8_t position)
{
    const uint8_t   rawBuff[] = { 0x32, position, filterByte};
    const uint8_t   rawReply[] = { 0x42, position, filterByte};
    SerialString_t  avtBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    SerialString_t  avtReply;
    SerialString_t::size_type   respPos;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::SetAvtFilterByte( $%02hhX, $%02hhX)\n",
         filterByte, position);

    try
    {
        // Send message to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);
        // Validate the AVT reply message
        respPos = FindAvtPacket( rawReply, sizeof( rawReply), avtReply);
        if( (int)respPos != -1)
        {
            // Erase our expected response string from the full response
            avtReply.erase( respPos, sizeof( rawReply));
            // If we received more data from the AVT than just the response to our
            //  'add filter byte' command
            if( avtReply.length() > 0)
            {
                // Add extraneous data to our rx fifo
                m_rxFifo.AddBytes( avtReply.c_str(), avtReply.length());
            }
            Log( LOG_DEV_DATA, "Filter byte $%02hhX added at position %d\n",
                 filterByte, position);
        }
        else
        {
            Log( LOG_ERRORS, "\tError adding Filter byte $%02hhX at position %d\n",
                 filterByte, position);
            Log( LOG_ERRORS, "\tSize: $%02hhx == 3\n", avtReply.size());
            if(avtReply.length() > 0)   Log( LOG_ERRORS, "\t   0: $%02hhx == $42\n", avtReply[ 0]);
            if(avtReply.length() > 1)   Log( LOG_ERRORS, "\t   1: $%02hhx == $%02hhX\n", avtReply[ 1], position);
            if(avtReply.length() > 2)   Log( LOG_ERRORS, "\t   2: $%02hhx == $%02hhX\n", avtReply[ 2], filterByte);
        }
    }
    catch( AvtTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tAvtTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "SetAvtFilterByte: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::SetAvtFilterByte( $%02hhX, $%02hhX)\n",
         filterByte, position);
}

/**
 * Restores any AVT filter bytes we are holding following a warm reset
 */
void AvtJ2190Proxy::RestoreFilterBytes()
{
    AvtFltrByteListItr_t    fbItr;
    uint8_t     fbVal, fbPos;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::RestoreFilterBytes()\n");

    for( fbItr=m_filterByteList.begin(); fbItr!=m_filterByteList.end(); fbItr++)
    {
        AvtFilterByte   &oldFb = *fbItr;
        oldFb.GetFbValAndPos(fbVal,fbPos);

        // Upload this filter byte to the AVT box
        SetAvtFilterByte( fbVal, fbPos);
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::RestoreFilterBytes()\n");
}

/**
 * Clears any filter bytes we are holding/maintaining
 */
void AvtJ2190Proxy::ResetFilterBytes()
{
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ResetFilterBytes()\n");

    // Erase any filter bytes we are holding
    m_filterByteList.clear();

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ResetFilterBytes()\n");
}

/**
 * Connects the AVT box to the VPW J1850 Bus Network
 */
void AvtJ2190Proxy::ConnectVPW()
{
    const uint8_t   rawBuff[] = { 0x52, 0x24, 0x01};
    SerialString_t  avtBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    const uint8_t   rawReply[] = { 0x62, 0x24, 0x01};
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ConnectVPW()\n");

    try
    {
        // Send message to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);
        if( (packetIdx=FindAvtPacket( rawReply, sizeof( rawReply), avtReply)) != -1)
        {
            Log( LOG_DEV_DATA, "ConnectVPW reply OK\n");
        }
        else
        {
            Log( LOG_ERRORS, "\tConnectVPW reply NOT OK\n");
        }
    }
    catch( AvtTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tAvtTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "ConnectVPW: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ConnectVPW()\n");
}

/**
 * Forces the AVT box into VPW mode
 */
void AvtJ2190Proxy::SetAvtVPW()
{
    const uint8_t   rawBuff[] = { 0xE1, 0x33};
    SerialString_t  avtBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    const uint8_t   rawReply[4][2] =  {{ 0x91, 0x05},{ 0x91, 0x07},{ 0x91, 0x09},{ 0x91, 0x0A}};
    SerialString_t  avtReply;
    int             packetIdx=-1;

    // If reply message looks good:
    //  0x91 0x05:  VPW Mode (Harris HIP 7030A0)
    //  0x91 0x07:  VPW Mode (Motorola)
    //  0x91 0x09:  VPW Auto transmit mode (Harris)
    //  0x91 0x0A:  VPW Mode (Harris HIP 7010)

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::SetAvtVPW()\n");

    try
    {
        // Send message to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);

        for( int ii=0; ii<4 && packetIdx==-1; ii++)
        {
            packetIdx = FindAvtPacket( rawReply[ ii], sizeof( rawReply[ ii]), avtReply);
        }

        // If any problem with the reply
        if( packetIdx == -1)
        {
            PrintSerialString( LOG_ERRORS, "\tSetAvtVPW Invalid AVT response", avtReply);
            throw( BepException( "SetAvtVPW Invalid AVT response"));
        }
    }
    catch( AvtTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tAvtTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "SetAvtVPW: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::SetAvtVPW()\n");
}

/**
 * Disconnects the AVT box from the VPW J1850 Bus Network
 */
void AvtJ2190Proxy::DisconnectVPW()
{
    const uint8_t   rawBuff[] = { 0x52, 0x24, 0x00};
    SerialString_t  avtBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    const uint8_t   rawReply[] = { 0x62, 0x24, 0x00};
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::DisconnectVPW()\n");

    try
    {
        // Send message to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);
        if( (packetIdx=FindAvtPacket( rawReply, sizeof( rawReply), avtReply)) != -1)
        {
            Log( LOG_DEV_DATA, "DisconnectVPW reply OK\n");
        }
        else
        {
            PrintSerialString( LOG_ERRORS, "\tDisconnectVPW reply NOT OK", avtReply);
        }
    }
    catch( AvtTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tAvtTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "DisconnectVPW: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::DisconnectVPW()\n");
}

/**
 * Requests the firmware revision from the AVT box
 */
void AvtJ2190Proxy::ReadAvtVersion()
{
    const uint8_t   rawBuff[] = { 0xB0};
    SerialString_t  avtBuff = SerialString_t( rawBuff, sizeof( rawBuff));
    const uint8_t   rawReply[] = { 0x92, 0x04};
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::ReadAvtVersion()\n");

    try
    {
        // Send message to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal * 100);

        // If proper length response and it is a version number reply
        if( (avtReply.size() >= 3) && ((packetIdx=FindAvtPacket( rawReply, sizeof( rawReply), avtReply)) != -1) )
        {
            m_avtVersion = avtReply[ 2];
            Log( LOG_DEV_DATA, "AVT Version: $%02hhX\n", m_avtVersion);
        }
        else
        {
            PrintSerialString( LOG_ERRORS, "\tReadAvtVersion Invalid AVT response", avtReply);
            throw( BepException( "ReadAvtVersion Invalid AVT response"));
        }
    }
    catch( AvtTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tAvtTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "ReadAvtVersion: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::ReadAvtVersion()\n");
}

/**
 * Sends a message to and gets a reply back from the AVT interface box
 *
 * @param avtMssg Message to be sent to the AVT box
 * @param avtResponse
 *                Reply from the AVT box
 * @param timeOut Timeout (in milliseconds) for the reply to come back
 */
void AvtJ2190Proxy::AvtMssg( const SerialString_t &avtMssg, SerialString_t &avtResponse,
                             uint32_t timeOut)
{
    uint32_t    packetSize=1;
    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::AvtMssg()\n");

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

            // Send the command to the AVT box
            TransmitMessage( avtMssg);

            avtResponse.clear();

            Log( LOG_DEV_DATA, "Wait for AVT response\n");

            // Read the AVT response for this message
            while( avtResponse.length() < packetSize)
            {
                if( GetAvtResponse( avtResponse) > 0)
                {
                    packetSize = (avtResponse[ 0] & 0x0F) + 1;
                    Log( LOG_DEV_DATA, "Received %d of %d bytes for this packet\n",
                         avtResponse.length(), packetSize);
                }
                else
                {
                    Log( LOG_DEV_DATA, "AVT Stopped sending data at %d of %d bytes\n",
                         avtResponse.length(), packetSize);
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

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::AvtMssg()\n");
}

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
int AvtJ2190Proxy::GetAvtResponse( SerialString_t &avtResponse)
{
    int         totalRead=0, bytesRead;
    uint8_t     buff[ 128];
    uint32_t    timeOut = 5000;

    Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::GetAvtResponse()\n");

    // Clear the current contents of the FIFO
    m_rxFifo.Reset();

    if( (bytesRead = ReadDriverData( buff, 1, timeOut)) > 0)
    {
        timeOut = 10;
        do
        {
            avtResponse += SerialString_t( buff, bytesRead);
            totalRead += bytesRead;
            bytesRead = ReadDriverData( buff, 128, timeOut);
        }
        while( bytesRead > 0);
    }

    // If we did not read any data or if we did not timeout (sometimes timeouts show up as EINTR)
    if( (totalRead == 0) || ((errno != ETIMEDOUT) && (errno != EINTR)) )
    {
        Log( LOG_ERRORS, "\tError waiting for AVT response (%d): %s\n", errno, strerror( errno));
    }

    // If we received any data
    if( totalRead > 0)
    {
        PrintSerialString( LOG_DEV_DATA, "\tAVT Response", avtResponse);
    }
    // No data received...
    else
    {
        Log( LOG_ERRORS, "\tTimeout waiting for AVT response\n");
        throw( AvtTimeoutException( timeOut));
    }

    Log( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::GetAvtResponse()\n");

    return( totalRead);
}

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
int AvtJ2190Proxy::FindAvtPacket( const uint8_t *searchMssg, uint32_t mssgLen, const SerialString_t &avtResponse)
{
    int retVal = -1;
    int packetSz;
    int ii;
    const int mssgSz = avtResponse.length();
    const SerialString_t    searchPacket( searchMssg, mssgLen);

//  Log( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::FindAvtPacket()\n");
    PrintSerialString( LOG_FN_ENTRY, "Enter AvtJ2190Proxy::FindAvtPacket()", searchPacket);

    ii = 0;
    while( ii < mssgSz)
    {
        // Get the length of the next full packet
        packetSz = avtResponse[ ii] & 0x0F;
        // If this entire packet is in the buffer
        if( mssgSz >= (ii+packetSz+1))
        {
            // Evaluate this packet
            const SerialString_t subPacket = avtResponse.substr( ii, packetSz+1);
            PrintSerialString( LOG_DEV_DATA, "\tNext packet", subPacket);
            // If this is the desired packet
            if( subPacket.find(searchPacket) == 0)
            {
                Log( LOG_DEV_DATA, "Found AVT packet at position %d\n", ii);
                retVal = ii;
                break;
            }
            ii += packetSz + 1;
        }
        else
        {
            // No more data...sorry
            break;
        }

    }

    PrintSerialString( LOG_FN_ENTRY, "Exit AvtJ2190Proxy::FindAvtPacket()", searchPacket);

    return( retVal);
}

