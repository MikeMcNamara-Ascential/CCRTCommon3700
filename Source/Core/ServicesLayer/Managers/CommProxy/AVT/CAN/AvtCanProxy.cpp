//******************************************************************************
// Description:
//  Logical serial port driver for CAN serial communication
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/AVT/CAN/AvtCanProxy.cpp $
// 
// 6     10/26/06 10:15a Rwiersem
// Changes for the 06102701 core release:
// 
// Added a case for "objNo < 0x10" in ConfigureMessageObject.
// 
// 5     4/13/06 3:52a Cward
// Fixed short form messages (required for multi frame messaging) Removed
// all references to the USE_READER_THREAD macro and beautified
//
// 4     2/24/06 1:16p Bmeinke
// Fixed short form messages (required for multi frame messaging)
// Removed all references to the USE_READER_THREAD macro
//
// 3     2/07/06 12:48p Bmeinke
// Beautified
//
// move usage block to main
//
// 3     7/20/05 9:06p Cward
// Added support for short form transmit messages
//
// 2     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:28p Cward
//
// 5     2/18/05 12:05p Bmeinke
// Removed the global AVT timeout timer (not really used)
// Lowered the ACK timeouts from 1500ms to 500ms
// If we timeout waiting for an AVT ACK, reset the our RX fifo
// Made the timeout value in WaitForTranslatorAck() an argument (defaults
// to 150ms)
// Changed the message used to set the CAN baud rate to the Ford
// recommended command in AvtSetCanSpeed()
//
// 4     1/31/05 9:15a Bmeinke
// Added client pid/tid logging to IoRead() and IoWrite() Log() calls
// Removed #if0/#endif inside ProcessErrorMesage()
// Inside ProcessReaderUnblockRequest(): Reset our receive fifo if the
// client issuing the timeout has the port locked. This was done to try to
// address a problem where we were getting garbage from the AVT at
// Dearborn causing our AVT FIFO to get out of sync
//
// 3     1/17/05 10:02a Bmeinke
// Changed the m_moduleList object so as to contain pointers
// Fixed ProcessErrorMessage() so it properly reports CAN errors
// Need to call RawCommProxy::ProcessReaderUnblockRerquest() at the end of
// AvtCanProxy::ProcessReaderUnblockRerquest()
//
// 2     12/22/04 8:21p Bmeinke
// In ReadPortDataUnlocked(), if we cannot read data from our rx FIFO, log
// an error and break out of the loop
//
// 2     8/17/04 6:46p Bmeinke
// Dont update the bus comm log in IoWrite until JUST before we Transmit
// the message
// Added ProcessReaderUnblockRequest() to handle unblock requests from
// blocked reader clients
// Add AVT initialization packets to the bus comm log
//
// 2     7/07/04 4:58p Bmeinke
// ProcessErrorMessage() will no longer log Break and IFR messages
// Lock m_moduleList in HandleReplyTimeoutPule()
// Replaced UpdateClientFifos() with CanAddToClientFifo()
// No longer lock m_moduleList in IoWrite (potential for deadlock)
// Added a 3 retry loop to AvtInit(): this makes it less likely for AVT
// initialization
// to fail due to a single comm error (like noise)
// Need to call AvtSetCanSpeed() before AvtTwoWireCan() in AvtInit() so
// that the AVT is at the correct network speed when it connects to the
// CAN
// network
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
// $NoKeywords: $
//==============================================================================

#include "AvtCanProxy.h"

const uint8_t AvtAckString[] = AVT_ACK_STRING;

const uint32_t  AvtTimeoutVal   =   10;

/**
 * Default Constructor
 */
AvtCanProxy::AvtCanProxy() : RawCommProxy(),
    m_avtBytesToCome( 0), m_waitingForAck( false),
    m_translatorAckString( AvtAckString), m_translatorAckTimeout( 100),
    m_avtAckChid( -1), m_avtAckCoid( -1), m_clientBeingAcked( NULL),
    m_avtVersion( 0x00), m_avtTimeoutPulseVal( 0x200), m_avtMssgFormat(AVT718_LONG_FORM),
    m_enableMssgPadding( false)
{
    SetProcessName( AVT_CAN_PROC_NAME);
}

/**
 * Constructor from command line arguments
 *
 * @param argc   Number of command line arguments to aprse
 * @param argv   Array of command line argument strings
 */
AvtCanProxy::AvtCanProxy( int argc, char *argv[]) : RawCommProxy( argc, argv),
    m_avtBytesToCome( 0), m_waitingForAck( false),
    m_translatorAckString( AvtAckString), m_translatorAckTimeout( 100),
    m_avtAckChid( -1), m_avtAckCoid( -1), m_clientBeingAcked( NULL),
    m_avtVersion( 0x00), m_avtTimeoutPulseVal( 0x200), m_avtMssgFormat(AVT718_LONG_FORM),
    m_enableMssgPadding( false)
{
}

/**
 * Class destructor
 */
AvtCanProxy::~AvtCanProxy()
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
const std::string AvtCanProxy::Register(void)
{
    std::string retVal( BEP_FAILURE_RESPONSE);

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::Register()\n");
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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::Register( %s)\n", retVal.c_str());

    return( retVal);
}

/**
 * Handles I/O mesages
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t AvtCanProxy::IoMssgLoop()
{
    ResManagerCmd_t status = RES_MGR_CMD_NONE;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::IoMssgLoop()\n");

    // Make sure filter byte list is empty
    ResetFilterBytes();

    // Reset and initialize the AVT box
//  AvtInit();
    ResManagerBase::m_resMgrStatus = RES_MGR_CMD_NONE;

    status = RawCommProxy::IoMssgLoop();

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::IoMssgLoop()\n");

    return( status);
}

/**
 * Reads configuration data from the given XML node
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void AvtCanProxy::Initialize(const XmlNode *document)
{
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::Initialize(%s)\n", document->getName().c_str());

    // Initialize base class
    RawCommProxy::Initialize( document);

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::Initialize()\n");
}

/**
 * Reads logical port configuration from an XML document
 *
 * @param portNode Node containing setup data for our port only
 */
void AvtCanProxy::ReadPortConfig( const XmlNode *portNode)
{
    try
    {
        try
        {
            // See what type of TX message format we will use
            if( NULL != portNode)
            {
                if( XmlString( "Short") == portNode->getChild( "Setup/TxMessageFormat")->getValue())
                {
                    m_avtMssgFormat = AVT718_SHORT_FORM;
                }
                else
                {
                    m_avtMssgFormat = AVT718_LONG_FORM;
                }
            }
            else
            {
                throw( BepException( "NULL configuration node"));
            }
        }
        catch( ...)
        {
            m_avtMssgFormat = AVT718_LONG_FORM;
        }

    try
    {
            // See if auto message padding is enabled
            if( NULL != portNode)
            {
                if( XmlString( "1") == portNode->getChild( "Setup/EnablePadding")->getValue())
                {
                    m_enableMssgPadding = true;
                }
                else
                {
                    m_enableMssgPadding = false;
                }
            }
            else
            {
                throw( BepException( "NULL configuration node"));
            }
        }
        catch( ...)
        {
            m_enableMssgPadding = false;
        }

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
void AvtCanProxy::ReadDefaultFilterBytes( const XmlNode *portNode)
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
bool AvtCanProxy::InitManager()
{
    bool    retVal = false;
    int     state = 0;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::InitManager()\n");


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
        default:    // Woo-hoo...it all worked !!!
            retVal = true;
            state = -2;
            break;
        }
        state++;
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::InitManager()\n");

    return( retVal);
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void AvtCanProxy::CleanupManager()
{
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::CleanupManager()\n");

    ConnectDetach( m_avtAckCoid);
    m_avtAckCoid = -1;

    ChannelDestroy( m_avtAckChid);
    m_avtAckChid = -1;

    // Make sure all timers are stopped
    StopComTimers();

    // Cleanup base class
    RawCommProxy::CleanupManager();

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::CleanupManager()\n");
}

/**
 * Method used to check if a serial response string can be added to a
 * client's rx FIFO
 *
 * @param data   Serial string reseived from the port
 * @param ocb    Client connection identifier
 * @return true if the data should be added to the client's FIFO, false otherwise
 */
bool AvtCanProxy::CanAddToClientFifo(const SerialString_t &data, CommIoOcb_t *ocb)
{
    CanIoOcb_t      *client = (CanIoOcb_t*)ocb;
    bool            canAdd=false;
    bool                isDiagMssg;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::CanAddToClientFifo()\n");

    // Check with base class first for filter strings
    if( RawCommProxy::CanAddToClientFifo( data, ocb) == true)
    {
        // Check if this is a diagnostic reply message
        isDiagMssg = IsDiagReplyMsg( data.c_str(), data.size());

        // If this is a response to a diag command
        if( isDiagMssg)
        {
            const ModuleId_t replyModId = GetModuleIdFromMessage( data.c_str(), data.size());

            Log( LOG_DEV_DATA, "Got a diagnostic reply from module $%04X\n",
                replyModId);
                    // If the module responding is this subscriber's module
            if( replyModId == client->clientModuleId)
            {
                Log( LOG_DEV_DATA, "Found matching client module for module $%04X (rx ID=%lld)\n",
                     replyModId, ocb->rxSubscription->id);
                        // Say ok to add to this client's buffer
                canAdd = true;
            }
                    else
                    {
                Log( LOG_DEV_DATA, "Not OK to add to client fifo because module ID does not match: $%04X != $%04X\n",
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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::CanAddToClientFifo()\n");

    return( canAdd);
}

/**
 * Extracts a module ID from a vehicle response
 *
 * @param mssg    Response message received from the vehicle
 * @param mssgLen Length of the response message in bytes
 *
 * @return The module ID from the vehicle response
 */
ModuleId_t AvtCanProxy::GetModuleIdFromMessage( const uint8_t *mssg, size_t msgLen)
{
    ModuleId_t  retVal = INVALID_CAN_ID;

    //  if short mode message take data from message
    if(AVT718_SHORT_FORM == m_avtMssgFormat)
    {
        if( msgLen > 2)
        {
            retVal = (((uint16_t)mssg[ 0]) << 8 | mssg [1]) - 0x08;
        }
    }
    // long mode message just send 0x760
    else
    {
        if( msgLen > 3)
        {
            retVal = (((uint16_t)mssg[ 2]) << 8 | mssg [3]) - 0x08;
        }
        //retVal = 0x760;
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
 * @return Pointer to newly allocated OCB or NULL if error
 * @see ResManagerBase::ocbCalloc
 */
IOFUNC_OCB_T* AvtCanProxy::OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
{
    CanIoOcb_t  *canOcb = new CanIoOcb_t;

    return( (IOFUNC_OCB_T*)canOcb);
}

/**
 * Method used to initialize an allocated OCB structure. Called
 * immediately after the OCB is allocated
 *
 * @param ioOcb  OCB structure to be initialized
 */
void AvtCanProxy::OcbInit(resMgrIoOcb_t *ioOcb)
{
    CanIoOcb_t  *newOcb = (CanIoOcb_t*)ioOcb;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::OcbInit()\n");

    RawCommProxy::OcbInit( ioOcb);

    newOcb->clientModuleId = INVALID_CAN_ID;
    newOcb->proxyOcb.replyTimeout = mSEC_nSEC( 500LL);

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::OcbInit()\n");
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
void AvtCanProxy::OcbFree(resMgrIoOcb_t *ocb)
{
    CanIoOcb_t      *canOcb = (CanIoOcb_t*)ocb;
    ModuleId_t      clientModId;
    int             err;

    // try to lock the list while we modify the OCB pointer in the m_moduleList
    if( (err=m_moduleList.Acquire()) != EOK)    Log( LOG_ERRORS, "\tError locking module list in OcbFinalize: %s\n",
                                                     strerror( err));

    clientModId = canOcb->clientModuleId;
    if( clientModId != INVALID_CAN_ID)
    {
        if( m_moduleList[ clientModId]->clientOcb == canOcb)
        {
            m_moduleList[ clientModId]->clientOcb = NULL;
        }
    }

    delete canOcb;

    if( err == EOK) m_moduleList.Release();
}

/**
 * Method used to finalize an allocated OCB structure. Called immediately
 * before the OCB is destroyed
 *
 * @param ioOcb  OCB structure to be finalized
 */
void AvtCanProxy::OcbFinalize(resMgrIoOcb_t *ioOcb)
{

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::OcbFinalize()\n");

    RawCommProxy::OcbFinalize( ioOcb);

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::OcbFinalize()\n");
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
int AvtCanProxy::IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
    CanIoOcb_t      *clientOcb = (CanIoOcb_t*)ioOcb;
    CommIoOcb_t     *proxyOcb = &clientOcb->proxyOcb;
    int             retVal = ENOSYS;
    bool            okToNotify = false;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::IoRead( pid %d, tid %d), moduleId=$%X, rx ID=%lld\n",
         ctp->info.pid, ctp->info.tid, clientOcb->clientModuleId,
         clientOcb->proxyOcb.rxSubscription->id);

    // Get the module ID this client shuld be reading from
    ModuleId_t  clientModId = clientOcb->clientModuleId;

    // IF this is not a "combine" read
    if( (msg->i.xtype & _IO_XTYPE_MASK) == _IO_XTYPE_NONE)
    {
        okToNotify = proxyOcb->rxSubscription->ShouldNotify();
        // Make sure client has performed vaid write or has a valid subscription
        if( (clientModId != INVALID_CAN_ID) || (okToNotify == true) )
        {
            // If this client is not associated with any particular module
            //  and client has a valid subscription
            if( (clientModId == INVALID_CAN_ID) && ( okToNotify == true) )
            {
                Log( LOG_IO_MSSG, "Client has a valid, non-module specific subscription\n");

                // Repy to the client with his data
                retVal = RawCommProxy::IoRead(ctp,msg,ioOcb);
            }
            else
            {
                if( (retVal=m_blockedReaders.Lock()) == EOK)
                {
                    CommGapTimer &comTimer = m_moduleList[ clientModId]->replyTimeoutTimer;

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
                            Log( LOG_DEV_DATA, "Clearing OK to send flag for module $%X because FIFO is empty\n",
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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::IoRead( pid %d, tid %d), moduleId=$%X, rx ID=%lld\n",
         ctp->info.pid, ctp->info.tid, clientOcb->clientModuleId, clientOcb->proxyOcb.rxSubscription->id);

    return( retVal);
}

/**
 * Message handler for _IO_WRITE I/O messages. Called by ResManagerBase
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Write message data
 * @param ioOcb  Control Block for this client
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManagerBase::ioWrite
 */
int AvtCanProxy::IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb)
{
    CanIoOcb_t      *clientOcb = (CanIoOcb_t*)ioOcb;
    SerialString_t  rawMessage, busMessage;
    const uint8_t   rawString[] = { 0x73, 0x07, 0x02, 0x01};
    SerialString_t  executeCommand( rawString, sizeof(rawString));
    int             bytesRead;
    int             retVal = EINVAL;
    ModuleId_t      moduleId = 0x0000;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::IoWrite( pid %d, tid %d), moduleId=$%X, rx ID=%lld\n",
         ctp->info.pid, ctp->info.tid, clientOcb->clientModuleId, clientOcb->proxyOcb.rxSubscription->id);

    // Make sure OK for client to write (port not locked)
    if( (CanClientWrite( &clientOcb->proxyOcb) == true) &&
        ((msg->i.xtype & _IO_XTYPE_MASK) == _IO_XTYPE_NONE))
    {
        // Read the message data from the sender
        if( (bytesRead = ReadSendersBuffer(ctp, rawMessage, msg)) > 1)
        {
            // Format the message for the Bus Translator
            if( AVT718_SHORT_FORM == m_avtMssgFormat)
            {
                busMessage = SerialString_t(1, (uint8_t)rawMessage.size()) + rawMessage;
                Log( "Sending short form message\n");
            }
            else
            {
            busMessage = rawMessage;
                Log( "Sending long form message\n");
            }

            PrintSerialString( LOG_SER_COM, "Sending", busMessage);

            // Wait for exclusive access to the AVT box
            if( (errno=m_translatorLock.Acquire()) == EOK)
            {
                // Say no AVT ACK received yet
                if( (retVal=m_avtAckRxd.Acquire()) == EOK)
                {
                    m_avtAckRxd.Signal( 0);
                    m_avtAckRxd.Release();
                }
                else
                {
                    m_avtAckRxd.Signal( 0);
                    Log( LOG_ERRORS, "\tError clearing AVT ACK sync: %s\n", strerror( retVal));
                }

                // Tell comm logger which client is being acked
                m_clientBeingAcked = clientOcb;

                if( AVT718_SHORT_FORM == m_avtMssgFormat)
                {
                    uint16_t junk = rawMessage[1];

                    junk = ((junk << 8) & 0xFF00) | rawMessage[ 2];
                    // See which module we are sending to
                    clientOcb->clientModuleId = junk;
                }
                else
                {
                // See which module we are sending to (based on tx object mapping)
                    clientOcb->clientModuleId = m_txObjectMap[ executeCommand[ 2]];//rawMessage[ 1];
                    //clientOcb->clientModuleId = 0x760;
                }
                moduleId = clientOcb->clientModuleId;

                // If noone communicating with this module yet
                m_moduleList[ moduleId]->clientOcb = clientOcb;
                    // Wait until we can send to this module
                    if( WaitToSendToModule( moduleId) == true)
                    {
                        // Cant send to this module until we get a response (or reply timeout)
                        SetOkToSend( moduleId, false);

                        // Wait for ACK back from bus translator
                        if( m_avtAckLock.Acquire() == EOK)
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
                        UpdateBusCommLog( ComDirTx, busMessage.c_str(), busMessage.length(),
                                          &clientOcb->proxyOcb);

                            // Send this message to the vehicle
                            TransmitMessage( busMessage);

                            // Wait for the AVT ACK to come back (message received)
                            if( (retVal=WaitForTranslatorAck()) == EOK)
                            {
                            // Long form messages give us 2 ACKs
                            if( AVT718_LONG_FORM == m_avtMssgFormat)
                            {
                                // Tell the AVT to send the message to the vehicle
                                TransmitMessage(executeCommand);
                                // Wait for 2 ACK responses from the AVT box
                                retVal = WaitForTranslatorAck( 2, 500);
                            }
                            if( EOK == retVal)
                            {
                                    // Arm the inter-byte gap timer (if enabled)
                                    uint32_t    msGapTime = m_currentPortProtocol.GetMsgGap();
                                    if( msGapTime > 0)
                                    {
                                    // Retart message gap timer for this module
                                    Log( LOG_DEV_DATA, "Starting message gap timer for module $%X for %d ms\n",
                                         moduleId, msGapTime);
                                    m_moduleList[ moduleId]->mssgGapTimer.Initialize( m_mssgCoid, MSSG_GAP_TIMER, moduleId);
                                    m_moduleList[ moduleId]->mssgGapTimer.Arm( mSEC_nSEC( msGapTime));
                                }

                                    // Arm the reply timeout timer for this client
                                    ArmReplyTimeoutTimer( (CommIoOcb_t*)clientOcb, true);

                                    // Tell kernel to update last modification time
                                    ioOcb->m_ioOcb.attr->flags |= IOFUNC_ATTR_MTIME |
                                                                    IOFUNC_ATTR_DIRTY_TIME;

                                ClearCommFault();
                            }
                                else
                                {
                                std::string faultReason("ERROR: AVT Translator DID NOT respond with 2nd ACK");
                                Log( LOG_ERRORS, "\t%s\n", faultReason.c_str());
                                SetCommFault( faultReason);

                                int err;
                                // Reset the AVT in case it's in a bad condition
                                if( (err=m_comGuard.Acquire()) == EOK)
                                {
                                    m_rxFifo->Reset();
                                    FlushDriverFifos( TCIOFLUSH);
                                    m_comGuard.Release();
                                }
                                else
                                {
                                    Log( LOG_ERRORS, "Error locking comGuard in IoWrite() to flush FIFOs: %s\n",
                                         strerror( err));
                                }

                                // Since the AVT never ack'd our command, assume our module message never
                                //  got sent.
                                SetOkToSend( moduleId, true);

                                    // Tell client nothing was written
                                    bytesRead = 0;
                                }
                            }
                            else
                            {
                            std::string faultReason("ERROR: AVT Translator DID NOT respond with 1st ACK");
                            Log( LOG_ERRORS, "\t%s\n", faultReason.c_str());
                            SetCommFault( faultReason);

                            int err;
                            // Reset the AVT in case it's in a bad condition
                            if( (err=m_comGuard.Acquire()) == EOK)
                            {
                                m_rxFifo->Reset();
                                FlushDriverFifos( TCIOFLUSH);
                                m_comGuard.Release();
                            }
                            else
                            {
                                Log( LOG_ERRORS, "Error locking comGuard in IoWrite() to flush FIFOs: %s\n",
                                     strerror( err));
                            }

                            // Since the AVT never ack'd our command, assume our module message never
                            //  got sent.
                            SetOkToSend( moduleId, true);

                                // Tell client nothing was written
                                bytesRead = 0;
                            }
                            // Release this mutex to unblock the thread which sent us this ACK pulse
                            m_avtAckLock.Release();
                        }
                        else
                        {
                            Log( LOG_ERRORS, "\tError acquiring AVT ACK Lock: %s\n", strerror( errno));

                        // Since we couldn't command the AVT, our module message never got sent.
                        SetOkToSend( moduleId, true);

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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::IoWrite( pid %d, tid %d), %d bytes status=%d module=$%X, rx ID=%lld\n",
         ctp->info.pid, ctp->info.tid, bytesRead, retVal,
         clientOcb->clientModuleId, clientOcb->proxyOcb.rxSubscription->id);

    return( retVal);
}

/**
 * Wait for command ACK's from the bus translator
 *
 * @param ackCount Number of ACK responses we expect (default=1)
 * @param timeOut  The total amount of time (ms) to wait for all ACKs
 *                 to come back (default=150)
 *
 * @return EOK if we received the ACK, error code (e.g. ETIMEDOUT) otherwise
 */
int AvtCanProxy::WaitForTranslatorAck( int ackCount,/*=1*/ uint32_t timeOut)/*=150*/
{
    int             retVal = ETIMEDOUT;
    uint64_t    nsTimeout = mSEC_nSEC( timeOut);// * ackCount;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::WaitForTranslatorAck( %d)\n", ackCount);

    int curPrio = RaiseThreadPriority( 2);

    if( (retVal=m_avtAckRxd.Acquire()) == EOK)
    {
        if( (retVal=m_avtAckRxd.Wait( ackCount, nsTimeout)) == EOK)
        {
            // Say no longer waiting for the ACK
            m_avtAckRxd.Signal( 0);
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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::WaitForTranslatorAck( %d)\n", ackCount);

    return( retVal);
}

/**
 * Arms a reply timeout timer for the client designated by 'ocb'
 *
 * @param clientOcb The OCB of the client we are arming the reply timeout timer for
 * @param forceArm  Flag touse if we want to unconditionally arm the timer (deafults to false)
 * @return 0 if timer was armed, any other value if not armed
 */
int AvtCanProxy::ArmReplyTimeoutTimer( CommIoOcb_t *clientOcb, bool forceArm)
{
    CanIoOcb_t      *canOcb = (CanIoOcb_t*)clientOcb;
    int             retVal = 1;
    ModuleId_t      moduleId = canOcb->clientModuleId;
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
        CommGapTimer &comTimer = m_moduleList[ moduleId]->replyTimeoutTimer;
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
 * @return The number of bytes received from the port
 */
int AvtCanProxy::ReadPortDataUnlocked(uint8_t *buff, size_t buffSz)
{
    int readCount = 0;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ReadPortDataUnlocked()\n");

    // The first byte of the buffer should be the AVT header byte
    readCount = m_rxFifo->Peek( &buff[ 0], 1);
    m_avtBytesToCome = buff[ 0] & 0x0F;

    // See if we have a full AVT message packet
    while( (m_avtBytesToCome <= (m_rxFifo->GetSize()-1)) && (readCount == 1))
    {
        // Extract this packet
        if( m_rxFifo->GetBytes( buff, m_avtBytesToCome+1) == (int)(m_avtBytesToCome+1))
        {
            m_rawRxBuff = SerialString_t( buff, m_avtBytesToCome+1);

            Log( LOG_DEV_DATA, "\tAVT Bytes: %d\n", m_avtBytesToCome);
            Log( LOG_DEV_DATA, "\tAVT Buff Len: %d\n", m_rawRxBuff.length());
            PrintSerialString( LOG_SER_COM, "AVT Message Data", m_rawRxBuff);

            Log( LOG_DEV_DATA, "\tAVT Command byte: $%02hhX\n", buff[ 0]);

            // Decipher the AVT response
            switch( buff[ 0] & 0xF0)
            {
            case 0x00:
            case 0x10:
            case 0x80:
                ProcessValidPacket(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0x20:
                ProcessErrorMesage(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0x30:
                ProcessCommandMessageError(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0x60:
                ProcessOperationalReport(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0x90:
                ProcessBoardStatusMessage(m_rawRxBuff, m_avtBytesToCome);
                break;
            case 0xE0:
                ProcessProcesorMemoryMessage(m_rawRxBuff, m_avtBytesToCome);
                break;

            case 0x40:
            case 0x50:
            case 0x70:
            case 0xA0:
            case 0xB0:
            case 0xC0:
            case 0xD0:
            case 0xF0:
                // These are reserved command response ID's
                break;
            default:
                break;
            }
        }
        else
        {
            Log( LOG_ERRORS, "Error reading %d bytes from RX fifo: %s\n", m_avtBytesToCome+1, strerror( errno));
            break;
        }

        // Check if there is another packet waiting
        readCount = m_rxFifo->Peek( &buff[ 0], 1);
        m_avtBytesToCome = buff[ 0] & 0x0F;
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ReadPortDataUnlocked(%d)\n", readCount);

    return( 0);
}

/**
 * Processes a "valid reply packet" reply from the AVT module
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtCanProxy::ProcessValidPacket( SerialString_t &rxBuff, uint32_t buffSize)
{
    CommIoOcb_t     *commOcb = NULL;
    int             err;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ProcessValidPacket()\n");

    // Log communication for this ACK
    if( m_clientBeingAcked != NULL) commOcb = &m_clientBeingAcked->proxyOcb;

    // Check first to see if we are waiting for an ACK from the translator
    if( CheckForAvtAck( rxBuff) == true)
    {
        Log( LOG_DEV_DATA, "Identified ACK...\n");

        ClearCommFault();

        UpdateBusCommLog( ComDirBusAck, rxBuff.c_str(), buffSize+1, commOcb);

        Log( LOG_DEV_DATA, "Signal AVT ACK sync\n");
        if( (err=m_avtAckRxd.Acquire()) == EOK)
        {
            int ackCount = m_avtAckRxd.GetValue();
            m_avtAckRxd.Signal( ackCount+1);
            m_avtAckRxd.Release();
            Log( LOG_DEV_DATA, "ACK count = %d\n", ackCount+1);
        }
        else
        {
            Log( LOG_ERRORS, "\tError signalling AVT ACK sync: %s\n", strerror( err));
        }
    }
    // If the message contains at least a valid vehicle response AVT header
    else if( rxBuff.length() > 1)
    {
        // Extract the vehicle message to process (trim off AVT header bytes)
        //SerialString_t    vehicleMssg = rxBuff.substr(2, buffSize-1);
        SerialString_t  vehicleMssg = rxBuff;

        // Log communication for this client
        UpdateBusCommLog( ComDirRx, vehicleMssg.c_str(), vehicleMssg.length(), commOcb);

        // Strip off the AVT header bytes
        if(AVT718_SHORT_FORM == m_avtMssgFormat)
        {
            vehicleMssg.erase(0, 2);
        }

        // buffSize indicates the number of bytes following the initial
        //  count byte, so we need t oaccount for the
        // No ACK expected, check if vehicle reply data
        if( ProcessVehicleMessage( vehicleMssg.c_str(), vehicleMssg.length()) == true)
        {
            // Remove count bytes from message
            rxBuff.erase(0,2);
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ProcessValidPacket()\n");
}

/**
 * Processes an "error message" reply from the AVT module
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtCanProxy::ProcessErrorMesage(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ProcessErrorMesage()\n");

    if( (rxBuff.length() >= buffSize) && (rxBuff.length() >= 4))
    {
        uint8_t canErr = rxBuff[ 2];
        uint8_t canStat = rxBuff[ 3];
        uint8_t faultMask = 0x00;
        string  errString( "\tCAN Error: \n");

        // We can ignore certain errors
        faultMask = (uint8_t)(~(0x01 | 0x40 | 0x80));
        Log( LOG_ERRORS, "\tCAN Error register: $%02hhX; mask = $%02hhX\n", canErr, faultMask);
        if( (canErr & faultMask) != 0)
        {
            errString = std::string( "\tCAN Error: \n");
            // These bit definitions are taken from the AVT document entitled "can_03c.pdf"
            if( canErr & 0x01)  errString += std::string( "$01: SIE (Status change interrupt)\n");
            if( canErr & 0x02)  errString += std::string( "$02: IntPnd (Interrupt Pending) does not match the interrupt register\n");
            if( canErr & 0x04)  errString += std::string( "$04: RXIE (Receive Interrupt) or TXIE (Transmit Interrupt) source does not match the IntPnd\n");
            if( canErr & 0x08)  errString += std::string( "$08: Auto respond object mode error (processing the auto respond object)\n");
            if( canErr & 0x10)  errString += std::string( "$10: Auto respond object mode error (processing the $7x $25 command)\n");
            if( canErr & 0x20)  errString += std::string( "$20: Short form transmit failure timeout\n");
            if( canErr & 0x40)  errString += std::string( "$40: Reserved\n");
            if( canErr & 0x80)  errString += std::string( "$80: Reserved\n");
        Log( errString.c_str());
            SetCommFault( errString);
        }

        Log( LOG_ERRORS, "\tCAN Status register: $%02hhX, mask = $%02hhX\n", canStat, faultMask);
        faultMask = (uint8_t)(~(AVT_CAN_TXOK | AVT_CAN_RXOK));
        if( (canStat & faultMask) != 0)
        {
            errString = std::string( "\tCAN Error Status: \n");
            // These bit definitions are taken from the AVT document entitled "can_03c.pdf"
            if( canStat & AVT_CAN_LEC0) errString += std::string( "$01: Last error code 0 is set\n");
            if( canStat & AVT_CAN_LEC1) errString += std::string( "$02: Last error code 1 is set\n");
            if( canStat & AVT_CAN_LEC2) errString += std::string( "$04: Last error code 2 is set\n");
            if( canStat & AVT_CAN_TXOK) errString += std::string( "$08: A message has been successfully transmitted\n");
            if( canStat & AVT_CAN_RXOK) errString += std::string( "$10: A message has been successfully received\n");
            if( canStat & AVT_CAN_WAKE) errString += std::string( "$20: CAN device awakened by bus activity after being commanded to sleep\n");
            if( canStat & AVT_CAN_WARN) errString += std::string( "$40: More than 96 consecutive CAN bus errors\n");
            if( canStat & AVT_CAN_BOFF) errString += std::string( "$80: More than 256 consecutive CAN bus errors, bus off\n");
        Log( errString.c_str());
            SetCommFault( errString);
        }

        // Log communication for this ACK
        UpdateBusCommLog( ComDirErr, rxBuff.c_str(), buffSize+1, NULL);
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ProcessErrorMesage()\n");
}

/**
 * Processes a "command message error" reply from the AVT module
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtCanProxy::ProcessCommandMessageError(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ProcessCommandMessageError()\n");

    if( rxBuff.length() >= buffSize)
    {
        uint32_t    ii;
        char        avtErr[ 5 + 1];
        std::string faultString( "AVT Command Error: ");

        for( ii=0; ii<buffSize; ii++)
        {
            sprintf( avtErr, "$%02hhX ", rxBuff[ ii]);
            faultString += avtErr;
        }
        SetCommFault( faultString);
        Log( LOG_ERRORS, "\t%s\n", faultString.c_str());
        // Log communication for this error message
        CommIoOcb_t     *commOcb = NULL;
        if( m_clientBeingAcked != NULL) commOcb = &m_clientBeingAcked->proxyOcb;
        UpdateBusCommLog( ComDirErr, rxBuff.c_str(), buffSize+1, commOcb);
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ProcessCommandMessageError()\n");
}

/**
 * Processes a "arbitration lost" reply from the AVT module
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtCanProxy::ProcessArbitrationLost(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ProcessArbitrationLost()\n");

    if( rxBuff.length() >= buffSize)
    {
        uint8_t errCode = rxBuff[ 1];
        string  errString( "\tAVT Arbitration Error: \n");

        if( errCode & AVT_STAT_BAD_CRC)     errString += string( "\tBad CRC\n");
        if( errCode & AVT_STAT_NUM_BITS)    errString += string( "\tIncomplete message (incorrect number of bits)\n");
        if( errCode & AVT_STAT_BREAK)       errString += string( "\tBreak received\n");
        if( errCode & AVT_STAT_IFR)         errString += string( "\tIFR Data\n");
        if( errCode & AVT_STAT_ARBITRATION) errString += string( "\tLost arbitration\n");
        if( errCode & AVT_STAT_SUCCESS)     errString += string( "\tTransmission successful\n");
        if( errCode & AVT_STAT_DEVICE)      errString += string( "\tFrom this device\n");
        if( errCode & AVT_STAT_BAD_MSSG)    errString += string( "\tBad message (message too short or too long)\n");

        SetCommFault( errString);

        Log( errString.c_str());

        // Log communication for this ACK
        CommIoOcb_t     *commOcb = NULL;
        if( m_clientBeingAcked != NULL) commOcb = &m_clientBeingAcked->proxyOcb;
        UpdateBusCommLog( ComDirErr, rxBuff.c_str(), buffSize+1, commOcb);
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ProcessArbitrationLost()\n");
}

/**
 * Processes a reply from the AVT module which indicated an internal AVT error
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtCanProxy::ProcessAvtHwError(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ProcessAvtHwError()\n");

    if( rxBuff.length() >= buffSize)
    {
        uint8_t errCode = rxBuff[ 1];
        string  errString( "\tAVT Hardware Error: ");
        char        tempErr[ 1024];
        switch( rxBuff[ 1])
        {
        case 0x01:
            sprintf( tempErr, "AVT HBCC Error $%02hhX: Initialization error - Response not 01\n", errCode);
            break;
        case 0x02:
            sprintf( tempErr, "AVT HBCC Error $%02hhX: No 'message received OK flag', from loopback test\n", errCode);
            break;
        case 0x03:
            sprintf( tempErr, "AVT HBCC Error $%02hhX: No 'message transmitted OK flag', from loopback test\n", errCode);
            break;
        case 0x04:
            sprintf( tempErr, "AVT HBCC Error $%02hhX: Test message byte #1 error, from loopback test\n", errCode);
            break;
        case 0x05:
            sprintf( tempErr, "AVT HBCC Error $%02hhX: Test message byte #3 error, from loopback test\n", errCode);
            break;
        case 0x06:
            sprintf( tempErr, "AVT HBCC Error $%02hhX: No IRQ detected\n", errCode);
            break;
        case 0x08:
            sprintf( tempErr, "AVT HBCC Error $%02hhX: IR1 error, IR1 byte follows\n", errCode);
            break;
        case 0x09:
            sprintf( tempErr, "AVT HBCC Error $%02hhX: IR2 error, IR2 byte follows\n", errCode);
            break;
        case 0x0A:
            sprintf( tempErr, "AVT HBCC Error $%02hhX: IR3 error, IR3 byte follows\n", errCode);
            break;
        case 0x0B:
            sprintf( tempErr, "AVT Internal Error $%02hhX: No buffer available, error bits follow\n", errCode);
            break;
        case 0x0D:
            sprintf( tempErr, "AVT Internal Error $%02hhX: Transmit message too short\n", errCode);
            break;
        case 0x0E:
            sprintf( tempErr, "AVT Internal Error $%02hhX: Transmit message too long\n", errCode);
            break;
        case 0x0F:
            sprintf( tempErr, "AVT SCC Error $%02hhX: FIFO #1 overflow\n", errCode);
            break;
        case 0x31:
            sprintf( tempErr, "AVT Internal Error $%02hhX: Received a watchdog timeout (AVT-716 PWM mode only)\n", errCode);
            break;

        default:
            sprintf( tempErr, "AVT Internal Error $%02hhX: Unknown error\n", errCode);
            break;
        }
        errString += tempErr;
        Log( errString.c_str());
        SetCommFault( errString);
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ProcessAvtHwError()\n");
}

/**
 * Processes a reply from the AVT module which contains processor memory values
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtCanProxy::ProcessProcesorMemoryMessage(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ProcessProcesorMemoryMessage()\n");

    // If buffer is long enough to contain address and value
    if( rxBuff.length() >= 6)
    {
        Log( LOG_DEV_DATA, "PRocessor Memory: Address $%02hhX%02hhX%02hhX%02hhX; Value: $%02hhX\n",
             rxBuff[ 1], rxBuff[ 2], rxBuff[ 3], rxBuff[ 4], rxBuff[ 5]);
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ProcessProcesorMemoryMessage()\n");
}

/**
 * Processes a reply from the AVT module which contains operational mode information
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtCanProxy::ProcessOperationalReport(SerialString_t &rxBuff, uint32_t buffSize)
{
    float       timerInt;
    uint32_t    usInterval;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ProcessOperationalReport()\n");

    if( rxBuff.length() >= buffSize)
    {
        switch( rxBuff[ 1])
        {
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
        default:
            break;
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ProcessOperationalReport()\n");
}

/**
 * Processes a reply from the AVT module which contains board status information
 *
 * @param rxBuff   Reply message buffer to process (containing the AVT message data)
 * @param buffSize Number of bytes expected for the next full message (as given
 *                 by the header byte of the AVT message)
 */
void AvtCanProxy::ProcessBoardStatusMessage(SerialString_t &rxBuff, uint32_t buffSize)
{
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ProcessBoardStatusMessage()\n");

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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ProcessBoardStatusMessage()\n");
}

/**
 * Checks a message buffer for the AVT ACK string. If found,
 * the ACK string is removed from the buffer
 *
 * @param msg    Message buffer to check for an AVT ACK
 * @return true if the buffer contained an expected AVT ACK string;
 *         false if no AVT ACK was expected, or if no ACK was found
 */
bool AvtCanProxy::CheckForAvtAck( SerialString_t &msg)
{
    bool    retVal = false;
    int     ackPos;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::CheckForAvtAck(), %d bytes\n",
         msg.length());

    // If this message could be an ACK
    if( msg.length() >= (m_translatorAckString.length()-1))
    {
        // If ACK is at the beginning of the string
        if((msg[0] & 0xF0) == 0x80)
        {
            Log( LOG_DEV_DATA, "Found ACK at position %d\n", ackPos);
            // Say ACK was found in the response string
            retVal = true;
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::CheckForAvtAck()\n");

    return( retVal);
}
/**
 * Processes a vehicle reply message
 *
 * @param msg    The buffer containing the vehicle reply message
 * @param size   The number of bytes in the vehicle reply message
 * @return true if the message is a valid vehicle response message, false otherwise
 */
bool AvtCanProxy::ProcessVehicleMessage( const uint8_t *msg, const uint8_t size)
{
    bool            retVal = false;
    int             err;
    BlockedReader_t client;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ProcessVehicleMessage(), %d bytes\n",
         size);

    // Check for module reply
    if( IsDiagReplyMsg(msg, size))
    {
        // Clear any active comm faults
        ClearCommFault();

        if( (err=m_blockedReaders.Lock()) == EOK)
        {
            if( (err=m_moduleList.Acquire()) == EOK)
            {
                // Get the ID of the module that is replying
                ModuleId_t  moduleId = GetModuleIdFromMessage( msg, size);

                if( moduleId != INVALID_CAN_ID)
                {
                // Say OK to send to this module because we got a reply back
                SetOkToSend( moduleId, true);

                // Grab the OCB for the client waiting on the given module
                    CanIoOcb_t  *clientOcb = m_moduleList[ moduleId]->clientOcb;

                if( clientOcb != NULL)
                {
                    clientOcb->clientModuleId = moduleId;

                    // Stop the reply timeout timer for this module
                    Log( LOG_DEV_DATA, "Stopping reply timeout timerfor module $%X\n", moduleId);

                    // Reset client's reply timeout timer
                        m_moduleList[ moduleId]->replyTimeoutTimer.Stop();
                    clientOcb->proxyOcb.replyTimeout = m_defaultReplyTimeout;

                    // Put the rest of the data into the client fifo
                    UpdateClientFifos( (const char*)msg, size, (int)PORT_SUBSCRIBE_RX);

                    // Set a flag to say this module has replied
                    SignalModuleReply( moduleId, true);

                    // Check if anyone is waiting for a response
                    if( m_blockedReaders.FindBlockedReader( clientOcb, client) != false)
                    {
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

                        // Remove this client from the read blocked list
                        m_blockedReaders.RemoveBlockedReader( clientOcb);

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
                else
                {
                    // The module ID in the reply message is garbage
                    Log( LOG_ERRORS, "\tInvalid CAN module id $%X\n", moduleId);
                }
                m_moduleList.Release();
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
    else
    {
        Log( LOG_DEV_DATA, "Not a diagnostic reply message (%d bytes)\n", size);
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ProcessVehicleMessage()\n");

    return( retVal);
}

/**
 * Checks if a message is a valid vehicle reply message
 *
 * @param msg    The buffer containing the vehicle reply message
 * @param size   The number of bytes in the vehicle reply message
 * @return true if the buffer appears to be a valid diagnostic reply message
 */
bool AvtCanProxy::IsDiagReplyMsg( const uint8_t *msg, const uint8_t size)
{
    bool retVal = false;

    Log(LOG_FN_ENTRY,"Enter AvtCanProxy::IsDiagReplyMsg()\n");

    if(size >= CAN_DIAG_REPLY_MIN_SZ)
    {
        // Since the module id is not included in the CAN messages received
        // by the AVT, this check is not needed.  So if the lenght is okay,
        // return true;
        // if(msg[0] == CANDIAG_RESP_HDR_BYTE) retVal = true;
        retVal = true;
    }
    Log(LOG_FN_ENTRY,"Exit AvtCanProxy::IsDiagReplyMsg(%d)\n",(int)retVal);

    return(retVal);
}

/**
 * Handler used to perform any special processing required when a
 * blocked reader client requests to be unblocked (via TimerTimeout)
 *
 * @param ioOcb  Control block for the calling client
 * @param reader Reference to the blocked reader's attributes
 */
void AvtCanProxy::ProcessReaderUnblockRequest(resMgrIoOcb_t *ioOcb, BlockedReader_t &reader)
{
    CanIoOcb_t  *clientOcb = (CanIoOcb_t*)ioOcb;
    int         err;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ProcessReaderUnblockRequest()\n");

    if( clientOcb != NULL)
    {
        ModuleId_t      clientModId = clientOcb->clientModuleId;
        CommGapTimer    &comTimer = m_moduleList[ clientModId]->replyTimeoutTimer;

        // If reply timer is not active
        if( comTimer.IsTimerArmed() == true)
        {
            // Stop the client specific timeout timer
            comTimer.Stop();
            Log( LOG_DEV_DATA, "Stopping reply timeout timer for module $%X due to unblock request\n", clientModId);

            // If this client has the port locked
            if( clientOcb->proxyOcb.portLockCount > 0)
            {
                Log( LOG_DEV_DATA, "Resetting RX fifo due to response timeout\n");
                if( (err=m_rxFifo->Lock()) == EOK)
                {
                    m_rxFifo->Reset();
                    m_rxFifo->Unlock();
                    Log( LOG_DEV_DATA, "RX fifo reset\n");
                }
                else
                {
                    Log( LOG_ERRORS, "Error %d locking RX fifo for reset in AvtCanProxy::ProcessReaderUnblockRequest(): %s\n",
                         err, strerror( err));
                }
            }
            else
            {
                Log( LOG_DEV_DATA, "Not resetting RX fifo because client does not have port locked\n");
            }
        }
        else
        {
            Log( LOG_DEV_DATA, "Reply watchdog timer for module "
                 "$%X not running (or disabled)\n", clientModId);
        }
    }
    else
    {
        Log( LOG_ERRORS, "NULL OCB in AvtCanProxy::ProcessReaderUnblockRequest()\n");
    }

    // Let the base class do wht it needs to
    RawCommProxy::ProcessReaderUnblockRequest( ioOcb, reader);

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ProcessReaderUnblockRequest()\n");
}

/**
 * Handler for MSSG_GAP_TIMER pulses
 *
 * @param pulse  Pulse received on the I/O channel
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t AvtCanProxy::HandleMssgGapPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    ModuleId_t      moduleId;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::HandleMssgGapPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    // AVT did not respond
    if( pulse.value.sival_int == m_avtTimeoutPulseVal)
    {
        retVal = HandleAvtTimeoutPulse( pulse);
    }
    // If the pulse code is a valid module ID for an inter-message gap event
    else if( pulse.value.sival_int <= INVALID_CAN_ID)
    {
        // See which module we are timing
        moduleId = pulse.value.sival_int & 0xFFFF;

        Log( LOG_FN_ENTRY, "Handling MSSG_GAP_TIMER for module $%X\n", moduleId);

        // Say that it is OK to send to this module again
        SetOkToSend( moduleId);

        // Reset message gap timer
        m_moduleList[ moduleId]->mssgGapTimer.Stop();
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::HandleMssgGapPulse(%d,%d)\n",
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
ResManagerCmd_t AvtCanProxy::HandleAvtTimeoutPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::HandleAvtTimeoutPulse(%d,%d)\n",
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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::HandleAvtTimeoutPulse(%d,%d)\n",
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
ResManagerCmd_t AvtCanProxy::HandleReplyTimeoutPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    ModuleId_t      moduleId;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::HandleReplyTimeoutPulse(%d,%d)\n",
         pulse.code, pulse.value.sival_int);

    // AVT did not respond
    if( pulse.value.sival_int == m_avtTimeoutPulseVal)
    {
        HandleAvtTimeoutPulse( pulse);
    }
    else if( (errno=m_moduleList.Acquire()) == EOK)
    {
        // This is a message timeout pulse (module did not respond)
        // See which client to respond to with ETIMEDOUT
        moduleId = pulse.value.sival_int;

        Log( LOG_FN_ENTRY, "Handling REPLY_TIMEOUT for module $%X\n", moduleId);

        if( moduleId < INVALID_CAN_ID)
        {
            CanIoOcb_t *ocb = m_moduleList[ moduleId]->clientOcb;
            if( ocb != NULL)
            {
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
                    m_blockedReaders.RemoveBlockedReader( m_moduleList[ moduleId]->clientOcb);
                }
        catch( BepException &err)
        {
        }

            // Say OK to send to this module again
            SetOkToSend( moduleId, true);

        // Set comm timer to inactive
                if( m_moduleList[ moduleId]->replyTimeoutTimer.IsTimerArmed())
                {
                    m_moduleList[ moduleId]->replyTimeoutTimer.Stop();
                }
            }
            else
            {
                Log( LOG_ERRORS, "NULL OCB for module $%04hhX in AvtCanProxy::HandleReplyTimeoutPulse()\n", moduleId);
            }
        }
        else
        {
            Log( LOG_ERRORS, "Invalid CAN module ID $%04hhX in AvtCanProxy::HandleReplyTimeoutPulse()\n", moduleId);
        }

        m_moduleList.Release();
    }
    else
    {
        Log( LOG_ERRORS, "Error locking module list: %s\n", strerror( errno));
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::HandleReplyTimeoutPulse(%d,%d)\n",
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
int AvtCanProxy::PortSubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                        resMgrIoOcb_t *ioOcb)
{
    logPortMsg_t            *portMsg = (logPortMsg_t*)_DEVCTL_DATA( msg->i);
    logPortSubscribe_t      &subMsg = portMsg->portSubscribe;
    CanIoOcb_t              *clientOcb = (CanIoOcb_t*)ioOcb;
    int                     retVal;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::PortSubscribeHandler(), size = %d\n", ctp->size);

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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::PortSubscribeHandler()\n");

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
int AvtCanProxy::PortUnsubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                          resMgrIoOcb_t *ioOcb)
{
    logPortMsg_t            *portMsg = (logPortMsg_t*)_DEVCTL_DATA( msg->i);
    logPortUnsubscribe_t    &clientMsg = portMsg->portUnsubscribe;
    CanIoOcb_t              *clientOcb = (CanIoOcb_t*)ioOcb;
    int                     retVal;
    AvtFltrByteList         &filterByteList = clientOcb->filterByteList;
    LogPortFilterStringList &filterList = clientOcb->proxyOcb.rxSubscription->filterList;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::PortUnsubscribeHandler()\n");

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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::PortUnsubscribeHandler()\n");

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
int AvtCanProxy::ResetConnectionHandler( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    CanIoOcb_t      *clientOcb = (CanIoOcb_t*)ioOcb;
    int             retVal = EOK;
    ModuleId_t      clientModId = clientOcb->clientModuleId;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ResetConnectionHandler()\n");

    retVal = RawCommProxy::ResetConnectionHandler(ctp, msg, ioOcb);
    Log( LOG_DEV_DATA, "Clearing OK to send flag for moduel $%X because FIFO was reset\n",
         clientModId);
    SignalModuleReply( clientModId, false);

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ResetConnectionHandler()\n");

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
int AvtCanProxy::ResetDriverHandler( resmgr_context_t *ctp, io_devctl_t *msg,
                                       resMgrIoOcb_t *ioOcb)
{
    int retVal = EOK;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ResetDriverHandler()\n");

    // Do base class suff first (resets fifos)
    if( RawCommProxy::ResetDriverHandler( ctp, msg, ioOcb) == EOK)
    {
        // Re-initialize the AVT box
        if( AvtInit() != true)
        {
            retVal = ECOMM;
        }
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ResetDriverHandler()\n");

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
int AvtCanProxy::ObtainClientPortLock( CommIoOcb_t *clientOcb, int replyId, bool block)
{
    CanIoOcb_t  *canOcb = (CanIoOcb_t*)clientOcb;;
    int retVal;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ObtainClientPortLock( $%X)\n", canOcb->clientModuleId);
    retVal = RawCommProxy::ObtainClientPortLock(clientOcb,replyId,block);
    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ObtainClientPortLock( $%X)\n", canOcb->clientModuleId);

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
bool AvtCanProxy::ReleaseClientPortLock( CommIoOcb_t *clientOcb, bool releaseAll)
{
    CanIoOcb_t  *canOcb = (CanIoOcb_t*)clientOcb;;
    int retVal;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ReleaseClientPortLock( $%X)\n", canOcb->clientModuleId);
    retVal = RawCommProxy::ReleaseClientPortLock(clientOcb, releaseAll);
    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ReleaseClientPortLock( $%X)\n", canOcb->clientModuleId);

    return( retVal);
}

/**
 * Waits for the module with the given ID to be "send" ready
 *
 * @param moduleId ID of the module to wait for
 * @return true iof we can send to the module, false otherwise
 */
bool AvtCanProxy::WaitToSendToModule( ModuleId_t moduleId)
{
    bool            retVal = false;
    int             status;
    CanCondVar      &sendCondVar = m_moduleList[ moduleId]->canSend;
    CanIoOcb_t      *clientOcb;
    uint64_t        waitTime;

    Log( LOG_DEV_DATA, "Wait for module $%X to be send ready( %d)\n", moduleId, sendCondVar.GetValue());

    // Wait for the desired module to be "send" ready
    if( (status=sendCondVar.Acquire()) == EOK)
    {
        clientOcb = m_moduleList[ moduleId]->clientOcb;
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
            Log( LOG_ERRORS, "\tError %d waiting for send cond var $%X: %s\n",
                 status, moduleId, strerror( status));
        }

        sendCondVar.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError %d locking module mutex $%X: %s\n",
             status, moduleId, strerror( status));
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
bool AvtCanProxy::ModuleHasReplied( ModuleId_t moduleId) const
{
    // Get a reference to the read condvar for this module
    CanCondVar      &dataReady = m_moduleList[ moduleId]->dataReady;
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
void AvtCanProxy::SetOkToSend( const ModuleId_t& moduleId, bool value)
{
    CanCondVar &sendCondVar = m_moduleList[ moduleId]->canSend;

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
        Log( LOG_ERRORS, "\tError locking module $%X mutex in AvtCanProxy::SetOkToSend\n", moduleId);
    }
}

/**
 * Signals any threads waiting to read data from this module that data
 * is available or a reply timeout occurred
 *
 * @param moduleId Id of the module which timed-out or from which we received data
 */
void AvtCanProxy::BroadcastModuleReply( const ModuleId_t &moduleId)
{
    CanCondVar &dataReady = m_moduleList[ moduleId]->dataReady;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::BroadcastModuleReply( $%X)\n", moduleId);

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
    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::BroadcastModuleReply( $%X)\n", moduleId);
}

/**
 * Sets the status of the dataReady flag for the module with the given module id
 *
 * @param moduleId
 *               Module ID whose dataReady flag we want to set
 * @param value  New status of the given module's dataReady flag (true or false)
 */
void AvtCanProxy::SignalModuleReply( const ModuleId_t moduleId, bool value)
{
    Log( LOG_DEV_DATA, "Setting reply flag for module $%X to %d\n", moduleId, value);
    m_moduleList[ moduleId]->dataReady.Acquire();
    m_moduleList[ moduleId]->dataReady.Signal( value);
    m_moduleList[ moduleId]->dataReady.Release();
}

/**
 * Returns the status of the dataReady flag for the module with the given module id.
 *
 * @param moduleId
 *                 Module ID whose dataReady flag we want to set
 * @return true if the module with the given module ID has replied, false if it has not replied
 */
bool AvtCanProxy::IsModuleReplyReady( const ModuleId_t moduleId) const
{
    bool retVal = false;

    m_moduleList[ moduleId]->dataReady.Acquire();
    retVal = ModuleHasReplied(moduleId);
    m_moduleList[ moduleId]->dataReady.Release();

    return( retVal);
}

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
ModuleId_t AvtCanProxy::MakeReplyIdFromCmdId( const uint8_t *avtTxObjCmd, size_t len)
{
    ModuleId_t retVal = INVALID_CAN_ID;

    if( len > 7)
    {
        retVal = (avtTxObjCmd[ 6] << 8) | (avtTxObjCmd[ 7] + 8);
        Log( LOG_DEV_DATA, "Destination node $%X maps to reply node $%X\n",
            (avtTxObjCmd[ 6] << 8) | avtTxObjCmd[ 7], retVal);
    }
    else
    {
         Log( LOG_ERRORS, "Error: AVT command object not long enough (%d) to contain a node ID\n",
              len);
    }

    return( retVal);
}

/**
 * Reset the AVT box and upload any filter bytes we are holding
 *
 * @return true if initialization was successful, false otherwise
 */
bool AvtCanProxy::AvtInit()
{
    int     retries=0;
    bool    retVal = false;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::AvtInit()\n");

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

            // Reset our rx fifo
            FlushDriverFifos(TCIOFLUSH);
            m_rxFifo->Reset();

            // Try to initialize up to 3 times before failing
            while( (retries < 3) && ( retVal == false))
            {
                // Clear any active AVT comm errors
                ClearCommFault();

                try
                {
            // Reset the AVT box
            AvtReset();

                    // Put the 718 into CAN mode
                    AvtCanMode();

                    /**
                     * Set the baud rate before setting the physical layer. The 718
                     * defaults to 250kbps baud rate. If the CAN network is at a
                     * different baud rate than 250kbps, setting the physical layer
                     * first will generate numerous nuiscance error messages
            */
                    AvtSetCanSpeed();

                    // Tell AVT to use 2 wire CAN
                    AvtTwoWireCan();

                    // Setup AVT receive object
                    SetupReceiveObject();

                    // Setup AVT transmit object
                    SetupTransmitObject();

                    // If firmware is new enough and we want short form messaging
                    if( (m_avtVersion >=  0x59) && (AVT718_SHORT_FORM == m_avtMssgFormat))
                    {
                        // Enable multi-frame support
                        AvtMultiFrameMode();

                        // Enable automatic message padding
                        if( true == m_enableMssgPadding)
                        {
                            AvtMultiFramePadding();
                        }
                    }


                    retVal = true;
                }
        catch( BepException &err)
        {
                    std::string faultReason("Error initializing AVT box: ");
                    faultReason += err.GetReason();
                    Log( LOG_ERRORS, "\t%s: %s\n", faultReason.c_str(), err.GetReason());
                    SetCommFault( faultReason);
                }

                retries++;
            }
        }
        catch( BepException &err)
        {
            std::string faultReason("Error initializing AVT box: ");
            faultReason += err.GetReason();
            Log( LOG_ERRORS, "\t%s: %s\n", faultReason.c_str(), err.GetReason());
            SetCommFault( faultReason);
        }

        // Reset our rx fifo
        FlushDriverFifos(TCIOFLUSH);
        m_rxFifo->Reset();

        ResumeReaderThread();
    }
    else
    {
        Log( LOG_DEV_DATA, "Not Initialized yet..cannot initialize AVT\n");
    }


    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::AvtInit()\n");

    return( retVal);
}

/**
 * Performs a hardware restart on the AVT box
 */
void AvtCanProxy::AvtReset()
{
    const uint8_t   rawBuff[] = { 0xF1, 0xA5};
    const uint8_t   rawReply[] = {0x91,0x12};
    SerialString_t  avtBuff( rawBuff, sizeof( rawBuff));
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::AvtReset()\n");

    try
    {
        // Send Reset message to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);

        // look for first response packet
        packetIdx = FindAvtPacket(rawReply,sizeof(rawReply),avtReply);

        if( packetIdx != -1)
        {
            Log( LOG_DEV_DATA, "AvtReset reply OK @ index %d\n", packetIdx);

            // Look for the AVT firmware version
            avtReply.erase(0, 2);
            if( avtReply.size() >= 3)
            {
                m_avtVersion = avtReply[ 2];
                Log( "AVT-718 firmware version is $%02hhX (%X.%X)\n",
                     m_avtVersion, m_avtVersion/0x10, m_avtVersion&0x0F);
            }
            else
            {
                Log( LOG_ERRORS, "Unable to determine AVT-718 firmware version\n");
            }
        }
        else
        {
            Log( LOG_ERRORS, "\tAvtReset reply NOT OK\n");
            throw(BepException("Invalid response to AvtReset command"));
        }
    }
    catch( AvtTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tAvtTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "AvtReset: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::AvtReset()\n");
}

void AvtCanProxy::AvtCanMode()
{
    const uint8_t   rawBuff[] = {0xE1,0x99};
    const uint8_t   rawReply[] = {0x91,0x10};
    SerialString_t  avtBuff(rawBuff,sizeof(rawBuff));
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log(LOG_FN_ENTRY,"Enter AvtCanProxy::AvtCanMode()\n");

    try
    {
        // Send CanMode message to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);

        // look for first response packet
        packetIdx = FindAvtPacket(rawReply,sizeof(rawReply),avtReply);

        if( packetIdx != -1)
        {
            Log( LOG_DEV_DATA, "AvtCanMode reply OK @ index %d\n", packetIdx);
        }
        else
        {
            Log( LOG_ERRORS, "\tAvtCanMode reply NOT OK\n");
            throw(BepException("Invalid response to AvtCanMode command"));
        }
    }
    catch(AvtTimeoutException &err)
    {
        Log(LOG_ERRORS,"\tAvtTimeoutException: %s\n",err.GetReason());
        throw(BepException("AvtCanMode: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log(LOG_FN_ENTRY,"Exit AvtCanProxy::AvtCanMode()\n");
}

void AvtCanProxy::AvtTwoWireCan()
{
    const uint8_t   rawBuff[] = {0x72,0x11,0x02};
    const uint8_t   rawReply[] = {0x82,0x11,0x02};
    SerialString_t  avtBuff(rawBuff,sizeof(rawBuff));
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log(LOG_FN_ENTRY,"Enter AvtCanProxy::AvtTwoWireCan()\n");

    try
    {
        // Send TwoWireCan message to the avt box
        AvtMssg(avtBuff,avtReply,AvtTimeoutVal*100);
        packetIdx = FindAvtPacket(rawReply,sizeof(rawReply),avtReply);

        if(packetIdx != -1)
        {
            Log(LOG_DEV_DATA,"AvtTwoWireCan reply OK @ index %d\n",packetIdx);
        }
        else
        {
            Log(LOG_ERRORS,"\tAvtTwoWireCan reply NOT OK\n");
            throw(BepException("Invalid response to AvtTwoWireCan command"));
        }
    }
    catch(AvtTimeoutException &err)
    {
        Log(LOG_ERRORS,"\tAvtTimeoutException: %s\n",err.GetReason());
        throw(BepException("AvtTwoWireCan: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log(LOG_FN_ENTRY,"Exit AvtCanProxy::AvtTwoWireCan()\n");
}

void AvtCanProxy::AvtSetCanSpeed()
{
//  const uint8_t   rawBuff[] = {0x72,0x0A,0x02};    // set to 500KB
//  const uint8_t   rawReply[] = {0x82,0x0A,0x02};
    const uint8_t   rawBuff[] = {0x73,0x0B,0x40,0x2B};   // set to 500KB
    const uint8_t   rawReply[] = {0x83,0x0B,0x40,0x2B};
    SerialString_t  avtBuff(rawBuff,sizeof(rawBuff));
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log(LOG_FN_ENTRY,"Enter AvtCanProxy::AvtSetCanSpeed()\n");

    try
    {
        // Send SetCanSpeed message to the avt box
        AvtMssg(avtBuff,avtReply,AvtTimeoutVal*100);
        packetIdx = FindAvtPacket(rawReply,sizeof(rawReply),avtReply);

        if(packetIdx != -1)
        {
            Log(LOG_DEV_DATA,"AvtSetCanSpeed reply OK @ index %d\n",packetIdx);
        }
        else
        {
            Log(LOG_ERRORS,"\tAvtSetCanSpeed reply NOT OK\n");
            throw(BepException("Invalid response to AvtSetCanSpeed command"));
        }
    }
    catch(AvtTimeoutException &err)
    {
        Log(LOG_ERRORS,"\tAvtTimeoutException: %s\n", err.GetReason());
        throw(BepException("AvtSetCanSpeed: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log(LOG_FN_ENTRY,"Exit AvtCanProxy::AvtSetCanSpeed()\n");
}

void AvtCanProxy::SetupReceiveObject()
{
    Log(LOG_FN_ENTRY,"Enter AvtCanProxy::SetupReceiveObject()\n");

    // Disable message object 1
    EnableMessageObject( 1, AVT_CAN_RX_OBJECT, false);
    delay( 5);

    // Configure object 1 as a receive object for us (module ID=$768)
    ConfigureMessageObject( 1, AVT_CAN_RX_OBJECT, 0x00, 0x0768);
    delay( 5);

    // Enable message object 1
    EnableMessageObject( 1, AVT_CAN_RX_OBJECT, true);
    delay( 5);

    Log(LOG_FN_ENTRY,"Exit AvtCanProxy::SetupReceiveObject()\n");
}

void AvtCanProxy::SetupTransmitObject()
{
    Log(LOG_FN_ENTRY,"Enter AvtCanProxy::SetupTransmitObject()\n");

    // Disable message object 2
    EnableMessageObject( 2, AVT_CAN_TX_OBJECT, false);
    delay( 5);

    // Configure object 2 as a transmit object to module $760
    ConfigureMessageObject( 2, AVT_CAN_TX_OBJECT, 0x07, 0x0760);
    delay( 5);

    // Enable message object 2
    EnableMessageObject( 2, AVT_CAN_TX_OBJECT, true);
    delay( 5);

    Log(LOG_FN_ENTRY,"Exit AvtCanProxy::SetupTransmitObject()\n");
}

void AvtCanProxy::EnableMessageObject( uint8_t objNo, AvtCanMessageObj_t objType, bool enable)/*=true*/
{
    const uint8_t   txEnableObject[] = { 0x73, 0x04, objNo, (enable == true ? objType : 00)};
    const uint8_t   rxEnableObject[] = { 0x83, 0x04, objNo, (enable == true ? objType : 00)};
    SerialString_t  avtBuff( txEnableObject, sizeof(txEnableObject));
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::EnableMessageObject(%d, %d)\n", objNo, enable);

    try
    {
        // Enable (or disable) message object
        AvtMssg(avtBuff,avtReply,AvtTimeoutVal*100);
        packetIdx = FindAvtPacket( rxEnableObject, sizeof(rxEnableObject), avtReply);

        if(packetIdx != -1)
        {
            Log(LOG_DEV_DATA,"EnableMessageObject reply OK @ index %d\n",packetIdx);
        }
        else
        {
            Log(LOG_ERRORS,"\tEnableMessageObject reply NOT OK\n");
            throw(BepException("Invalid response to EnableMessageObject command for object %d",
                               objNo));
        }
    }
    catch(AvtTimeoutException &err)
    {
        Log(LOG_ERRORS,"\tAvtTimeoutException: %s\n", err.GetReason());
        throw(BepException("EnableMessageObject: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::EnableMessageObject(%d, %d)\n", objNo, enable);
}

void AvtCanProxy::ConfigureMessageObject( uint8_t objNo, AvtCanMessageObj_t objType,
                                          uint8_t dataLen, ModuleId_t moduleId)
{
    // Message object is an 11-bit object
    const uint8_t   msbID = (moduleId >> 8) & 0x00FF;
    const uint8_t   lsbID = moduleId & 0x00FF;
    const uint8_t   txObjectCfg[] = { 0x77, 0x05, objNo, objType, 0x01, dataLen, msbID, lsbID};
    const uint8_t   rxObjectCfg[] = { 0x87, 0x05, objNo, objType, 0x01, dataLen, msbID, lsbID};
    SerialString_t  avtBuff( txObjectCfg, sizeof(txObjectCfg));
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log(LOG_FN_ENTRY, "Enter AvtCanProxy::ConfigureMessageObject(%d, %02hhX, %d, $%X)\n",
        objNo, objType, dataLen, moduleId);

    if( objNo < 0x10)
    {
        if( AVT_CAN_TX_OBJECT == objType)   m_txObjectMap[ objNo] = moduleId;
        else                                m_rxObjectMap[ objNo] = moduleId;
    }

    try
    {
        // Disable message object 1
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);
        packetIdx = FindAvtPacket( rxObjectCfg, sizeof(rxObjectCfg), avtReply);

        if( packetIdx != -1)
        {
            Log(LOG_DEV_DATA, "ConfigureMessageObject reply OK @ index %d\n",packetIdx);
        }
        else
        {
            Log(LOG_ERRORS, "\tConfigureMessageObject reply NOT OK\n");
            throw(BepException("Invalid response to ConfigureMessageObject command for object %d",
                               objNo));
        }
    }
    catch(AvtTimeoutException &err)
    {
        Log(LOG_ERRORS,"\tAvtTimeoutException: %s\n", err.GetReason());
        throw(BepException("ConfigureMessageObject: No AVT response for %d seconds",
                           AvtTimeoutVal/10));
    }

    Log(LOG_FN_ENTRY, "Exit AvtCanProxy::ConfigureMessageObject(%d, %02hhX, %d, $%X)\n",
        objNo, objType, dataLen, moduleId);
}

void AvtCanProxy::AvtMultiFrameMode()
{
    // Receive MFM with object xx, transmit with object xx+1
    MakeMFMObjectPair( 0x01, 0x02);
    MakeMFMObjectPair( 0x03, 0x04);
    MakeMFMObjectPair( 0x05, 0x06);
    MakeMFMObjectPair( 0x07, 0x08);
    MakeMFMObjectPair( 0x09, 0x0A);
    MakeMFMObjectPair( 0x0B, 0x0C);
    MakeMFMObjectPair( 0x0D, 0x0E);
}

void AvtCanProxy::MakeMFMObjectPair( uint8_t rxObject, uint8_t txObject)
{
    // Receive MFM with rxObject, transmit with object txObject
    const uint8_t   tx_enableMultiFrame[] = {0x73,0x28,rxObject,txObject};
    const uint8_t   rx_enableMultiFrame[] = {0x83,0x28,rxObject,txObject};
    SerialString_t  avtBuff = SerialString_t(tx_enableMultiFrame,sizeof(tx_enableMultiFrame));
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log(LOG_FN_ENTRY,"Enter AvtCanProxy::MakeMFMObjectPair($%02hhX, $%02hhX)\n", rxObject, txObject);

    try
    {
        // Send message to enable multi-frame message support to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);
        packetIdx = FindAvtPacket( rx_enableMultiFrame, sizeof(rx_enableMultiFrame), avtReply);

        if(packetIdx != -1)
        {
            Log(LOG_DEV_DATA,"MakeMFMObjectPair reply OK @ index %d\n",packetIdx);
        }
        else
        {
            Log(LOG_ERRORS,"\tMakeMFMObjectPair reply NOT OK\n");
            throw( BepException( "Invalid response to MakeMFMObjectPair command"));
        }
    }
    catch(AvtTimeoutException &err)
    {
        Log(LOG_ERRORS,"\tMakeMFMObjectPair: %s\n",err.GetReason());
        throw(BepException("MakeMFMObjectPair: No AVT response for %d seconds",
                           AvtTimeoutVal/10));
    }

    Log(LOG_FN_ENTRY,"Exit AvtCanProxy::MakeMFMObjectPair($%02hhX, $%02hhX)\n", rxObject, txObject);
}


void AvtCanProxy::AvtMultiFramePadding()
{
    const uint8_t   tx_enablePadding[] = {0x73,0x27,0x01,0x00};
    const uint8_t   rx_enablePadding[] = {0x83,0x27,0x01,0x00};
    SerialString_t  avtBuff = SerialString_t(tx_enablePadding,sizeof(tx_enablePadding));
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log(LOG_FN_ENTRY,"Enter AvtCanProxy::AvtMultiFramePadding()\n");

    try
    {
        // Send message to enable multi-frame message support to the avt box
        AvtMssg( avtBuff, avtReply, AvtTimeoutVal*100);
        packetIdx = FindAvtPacket( rx_enablePadding, sizeof(rx_enablePadding), avtReply);

        if(packetIdx != -1)
        {
            Log(LOG_DEV_DATA,"AvtMultiFramePadding reply OK @ index %d\n",packetIdx);
        }
        else
        {
            Log(LOG_ERRORS,"\tAvtMultiFramePadding reply NOT OK\n");
            throw( BepException( "Invalid response to AvtMultiFramePadding command"));
        }
    }
    catch(AvtTimeoutException &err)
    {
        Log(LOG_ERRORS,"\tAvtTimeoutException: %s\n", err.GetReason());
        throw(BepException("AvtMultiFrameMode: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log(LOG_FN_ENTRY,"Exit AvtCanProxy::AvtMultiFramePadding()\n");
}

/**
 * Clears all filter bytes from the AVT box
 */
void AvtCanProxy::ClearAvtFilterBytes()
{
    const uint8_t   rawBuff[] = { 0x31, 0x7B};
    const uint8_t   rawReply[] = { 0x40};
    SerialString_t  avtBuff( rawBuff, sizeof( rawBuff));
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ClearAvtFilterBytes()\n");

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
            throw(BepException("Invalid response to ClearFilterBytes command"));
        }
    }
    catch( AvtTimeoutException &err)
    {
        Log( LOG_ERRORS, "\tAvtTimeoutException: %s\n", err.GetReason());
        //m_rxBuffLock.Release();
        throw( BepException( "ClearAvtFilterBytes: No AVT response for %d seconds",
                             AvtTimeoutVal/10));
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ClearAvtFilterBytes()\n");
}

/**
 * Uploads a filter byte to the AVT box.
 *
 * @param filterByte The position of the filter in incoming vehicle messages
 * @param position   The value of the filter byte
 */
void AvtCanProxy::UploadFilterByte( uint8_t filterByte, uint8_t position)
{
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::UploadFilterByte(%d, $%02X)\n", filterByte, position);

    // If this is the first time to add this filter byet
    if( m_filterByteList.AddFilterByte( filterByte, position) == 1)
    {
        // Send filter byte to the AVT box
        SetAvtFilterByte( filterByte, position);
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::UploadFilterByte(%d, $%02X)\n", filterByte, position);
}

/**
 * Removes a filter byte from the AVT box's list
 *
 * @param filterByte Filter byte to be removed to the AVT box
 * @param position   Position of the filter byte to be removed
 */
void AvtCanProxy::DelAvtFilterByte( uint8_t filterByte, uint8_t position)
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

                // erase all AVT box filter bytes (from AVT box itself)
                ClearAvtFilterBytes();

                // Restore the remaining
                RestoreFilterBytes();
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
void AvtCanProxy::SetAvtFilterByte( uint8_t filterByte, uint8_t position)
{
    const uint8_t   rawBuff[] = { 0x32, position, filterByte};
    const uint8_t   rawReply[] = { 0x42, position, filterByte};
    SerialString_t  avtBuff( rawBuff, sizeof( rawBuff));
    SerialString_t  avtReply;
    SerialString_t::size_type   respPos;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::SetAvtFilterByte( $%02hhX, $%02hhX)\n",
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
                m_rxFifo->AddBytes( avtReply.c_str(), avtReply.length());
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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::SetAvtFilterByte( $%02hhX, $%02hhX)\n",
         filterByte, position);
}

/**
 * Restores any AVT filter bytes we are holding following a warm reset
 */
void AvtCanProxy::RestoreFilterBytes()
{
    AvtFltrByteListItr_t    fbItr;
    uint8_t     fbVal, fbPos;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::RestoreFilterBytes()\n");

    for( fbItr=m_filterByteList.begin(); fbItr!=m_filterByteList.end(); fbItr++)
    {
        AvtFilterByte   &oldFb = *fbItr;
        oldFb.GetFbValAndPos(fbVal,fbPos);

        // Upload this filter byte to the AVT box
        SetAvtFilterByte( fbVal, fbPos);
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::RestoreFilterBytes()\n");
}

/**
 * Clears any filter bytes we are holding/maintaining
 */
void AvtCanProxy::ResetFilterBytes()
{
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ResetFilterBytes()\n");

    // Erase any filter bytes we are holding
    m_filterByteList.clear();

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ResetFilterBytes()\n");
}

/**
 * Requests the firmware revision from the AVT box
 */
void AvtCanProxy::ReadAvtVersion()
{
    const uint8_t   rawBuff[] = { 0xB0};
    const uint8_t   rawReply[] = { 0x92, 0x04};
    SerialString_t  avtBuff( rawBuff, sizeof( rawBuff));
    SerialString_t  avtReply;
    int             packetIdx=-1;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::ReadAvtVersion()\n");

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

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::ReadAvtVersion()\n");
}

/**
 * Sends a message to and gets a reply back from the AVT interface box
 *
 * @param avtMssg Message to be sent to the AVT box
 * @param avtResponse
 *                Reply from the AVT box
 * @param timeOut Timeout (in milliseconds) for the reply to come back
 */
void AvtCanProxy::AvtMssg( const SerialString_t &avtMssg, SerialString_t &avtResponse,
                             uint32_t timeOut)
{
    uint32_t    packetSize=1;
    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::AvtMssg()\n");

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
            UpdateBusCommLog( ComDirTx, avtMssg.c_str(), avtMssg.length(), NULL);
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

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::AvtMssg()\n");
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
int AvtCanProxy::GetAvtResponse( SerialString_t &avtResponse)
{
    int         totalRead=0, bytesRead;
    uint8_t     buff[ 128];
    uint32_t    timeOut = 5000;

    Log( LOG_FN_ENTRY, "Enter AvtCanProxy::GetAvtResponse()\n");

    // Clear the current contents of the FIFO
    m_rxFifo->Reset();

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
        UpdateBusCommLog( ComDirRx, avtResponse.c_str(), avtResponse.length(), NULL);
        PrintSerialString( LOG_DEV_DATA, "\tAVT Response", avtResponse);
    }
    // No data received...
    else
    {
        Log( LOG_ERRORS, "\tTimeout waiting for AVT response\n");
        throw( AvtTimeoutException( timeOut));
    }

    Log( LOG_FN_ENTRY, "Exit AvtCanProxy::GetAvtResponse()\n");

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
int AvtCanProxy::FindAvtPacket( const uint8_t *searchMssg, uint32_t mssgLen, const SerialString_t &avtResponse)
{
    int retVal = -1;
    int packetSz;
    int ii;
    const int mssgSz = avtResponse.length();
    const SerialString_t    searchPacket( searchMssg, mssgLen);

    PrintSerialString( LOG_FN_ENTRY, "Enter AvtCanProxy::FindAvtPacket()", searchPacket);

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

    PrintSerialString( LOG_FN_ENTRY, "Exit AvtCanProxy::FindAvtPacket()", searchPacket);

    return( retVal);
}

/**
 * Stops all the com timers (reply timeout and message gap timers)
 */
void AvtCanProxy::StopComTimers()
{
    int ii;

    for( ii=0; ii<0x100; ii++)
    {
        if( m_moduleList[ ii]->replyTimeoutTimer.IsTimerArmed())
        {
            m_moduleList[ ii]->replyTimeoutTimer.Stop();
        }

        if( m_moduleList[ ii]->mssgGapTimer.IsTimerArmed())
        {
            m_moduleList[ ii]->mssgGapTimer.Stop();
        }
    }
}

