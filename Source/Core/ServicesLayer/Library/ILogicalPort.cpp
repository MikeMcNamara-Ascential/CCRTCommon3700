//******************************************************************************
// Description:
//  Logical serial port driver interface client
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/ILogicalPort.cpp $
// 
// 12    10/26/06 9:58a Rwiersem
// Changes for the 06102701 core release:
// 
// Added calls to GetDriverPath() before checking if the port name has
// changed.
// 
// 11    2/15/06 7:50p Cward
// Added support for Registering Port Drivers, and Reconnecting Drivers
//
// 10    11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 9     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 8     2/16/05 9:21p Cward
// Update ReadResponseString
//
// 7     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 6     6/24/04 12:57a Cward
// In initialize, changed the setting of the vehicle Message XML node to
// create a copy. Added a check prior to that to make sure it didn't
// already exist, to prevent a memory leak. In read message map, moved the
// message tags insert statement outside the if so that it is done
// unconditionally.
//
// 5     5/25/04 4:13p Cward
// needed to delet the vehicle message xml node in the destructor to
// prevent a memory leak
//
// 4     4/28/04 10:54p Cward
// Changes associated with build 040402801
//
// 3     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 2     8/18/03 7:44a Khykin
// Updated with Flatrock changes.
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 3:58p Khykin
// Initial checkin.
//
// 2     4/30/03 10:34a Khykin
// Added the initial clearing of the message buffer and the handling of
// send timeouts.
//
// 1     4/22/03 9:50a Khykin
// Kevins Working Checkin
//
// 43    4/07/03 3:15p Bmeinke
// In ResolveTransmitInterpretation(), clear the xmtIntrp string if its
// length is 4 at the bottom of the parsing loop to remove the last
// wildcard reeplacement specifier
//
// 42    3/20/03 8:45p Bmeinke
// Darn syntax errors
//
// 41    3/20/03 8:37p Bmeinke
// In WritePort( SerialString_t, retryCount), retryCount was not getting
// passed to the lowest level WritePort() method
// Added IsPositiveResponse() (and numerous supporting methods). Specified
// in the config file, used to chekc if a received response is tagged as
// positive or negative
//
// 40    3/18/03 6:50p Bmeinke
// Added support for performing a 5-baud initialization
// Fixed comment for PortSetProtocol()
// Added portgetprotocol()
// Cleaned up the protocol helper functions (i.e. portsetByteGap())
// Added LowSpeedInit() (implementaion of the 5-baud initialization)
//
// 39    3/06/03 1:18a Bmeinke
// Syntax error in previous checkin
// Made all parameters which specify a timeout period signed (int64_t from
// uint64_t)
//
// 38    3/06/03 1:13a Bmeinke
// Changed SetReplyTimeout() and GetReplyTimeout() to take/return signed
// int64_t to test for negative timeout values
// In ReadPort( uint8_t*buff...), if the initTimeout is 0, we still need
// to set a 0ns timeout value
//
// 37    2/21/03 7:26p Bmeinke
// Changed all dcmd arguments to devctl() to the new DCMD_* defines
// In ReadPort( const string &mssgTag, SerialString_t &respString),
// changed initWait=-2 and endWait=0 (was -1 and 0)
//
// 36    2/06/03 3:56p Bmeinke
// In PortSetRxProtocol(), set the MSB in c_iflag of the portProtocol
// argument
//
// 35    2/06/03 2:44p Bmeinke
// Inside ReadPort(), only add data  to the response string if the return
// value from read() is > 0
//
// 34    2/06/03 10:15a Bmeinke
// LockPort() and UnlockPort() have 3 reties (based on Kevin's CTC code)
// PortSetProtocol() now uses POSIX 'DCMD_CHR_TCSETATTR' devctl() command
// ReadPort() now takes two timeout arguments
// Added EstablishComms() method to perform ISO fast initialization
//
// 33    1/21/03 4:37p Bmeinke
// When reading the attributes for individual reply strings from the
// config file, only set the end of message timeout value for the
// associated ComMssgTableEntry if the individual response string end of
// message tmeout is greater than that of any previous individual end of
// message timeout values
//
// 32    1/18/03 1:53p Bmeinke
// Modified GetDriverPath() so that it will not add the LOG_PORT_DEV_ROOT
// to the beginning of an absolute logical port port name
//
// 31    1/16/03 3:19p Bmeinke
// Changed the duration argument to AssertBreak() from nanoseconds to
// milliseconds
//
// 30    1/16/03 2:57p Bmeinke
// Changed the name of DropTransmit() to AssertBreak()
//
// 29    1/15/03 10:09a Bmeinke
// Changed parameter to devctl() inside DroptTransmit() from
// _POSIX_DEVDIR_NONE to _POSIX_DEVDIR_TO
// Changed the wayt that variable length reply strings are handled (moved
// implementation lower)
//
// 28    1/10/03 6:00p Bmeinke
// Slick edit REALLY needs a spell checker
//
// 27    1/10/03 5:55p Bmeinke
// Added support for variable length reply strings and "end of message"
// timeout gap
// Added DisableTransmit() to disable the serial port transmitter for fast
// ISO initialization sequences
//
// 26    11/20/02 2:25a Bmeinke
// Set custom reply timeout from inside ReadPort()
// If ReadPort() fails due to reply timeout, reset our connection to
// logical port driver
//
// 25    11/18/02 5:47p Bmeinke
// Changed duplicating input/output driver file descriptors
// Removed blank lines
// Fixed bug when setting a custom receive protocol (was setting transmit
// protocol)
//
// 24    11/18/02 1:19p Bmeinke
// Added SetReplyTimeout() method to set per message reply timeouts
// Read reply timeout attribute for individual reply messages (under
// VehicleMessages section) during initialization
//
// 23    10/24/02 7:54p Bmeinke
// Added ResetServer() method to command a logical port driver to perform
// a reset operation.
// In ResetConnection() and ResetServer(), send commands to both input and
// output drivers if they are not the same port driver
//
// 22    10/22/02 3:39p Bmeinke
// Changed debug printout loop upper bound from 'len' to 'retVal' to only
// print out data that was received
//
// 21    9/24/02 7:05p Bmeinke
// Print out received data inside ReadPort() if debug is enabled
//
// 20    9/06/02 3:58p Bmeinke
// Removed a blank line
//
// 19    8/29/02 8:01p Bmeinke
// If write() inside WritePort() fails, printout reason with perror()
// instead of printf
//
// 18    8/07/02 3:20p Bmeinke
// Removed compiler warnings
//
// 17    6/27/02 7:03p Bmeinke
// Set m_subHandle = LOG_PORT_INVALID_SUB in PortUnsubscribe()
//
// 16    6/12/02 3:22p Bmeinke
// Removed the call to setupNode = portConfig->getChild( "SETUP") from the
// Initialize() method (setupNode not referenced)
//
// 15    6/05/02 11:01p Bmeinke
// Add printout of reply data in Readport() if reply is valid and
// IsDebugOn() == true
// Changed default return value from ReadXmtProtocol() and
// ReadRcvProtocol() to false
//
// 14    5/29/02 5:49p Bmeinke
// Added ability to open seperate transmit and receive ports
//
// 13    5/15/02 1:06p Bmeinke
// Added a call to ILogger::Initialize() from ILogicalPort::Initialize()
// In ReadMessageMap(), catch exception case when no message map exists in
// the config file (e.g. if config file only used for ILogger
// initialization)
//
// 12    5/14/02 4:02p Bmeinke
// Changed ReadPort()/WritePort() methods to use SerialString_t and
// uint8_t* data types for sending raw messages
// Added ReadPort()/WritePort() methods that take std::string message tag
// arguments to send/receive data based on message tags read from a
// configuration file
//
// 11    4/24/02 5:24a Bmeinke
// Added support for resetting client FIFOs via the ResetConnection()
// method
//
// 10    4/15/02 1:53p Bmeinke
// Added ability to specify filter strings to PortSubscribe()
//
// 9     4/02/02 4:35p Bmeinke
// Removed Error printout from WritePort() if the write was successful
//
// 8     4/01/02 2:26p Bmeinke
// Fixed some typos
//
// 7     4/01/02 1:38p Bmeinke
// Added overloaded ReadPort and WritePort which take std::string
// arguments
//
// 6     3/22/02 1:21p Bmeinke
// Adapted from ILogPort (simply changed the name and include file name)
//
// 5     2/01/02 9:32p Brian.meinke
// Added m_portSubscrMask to control when a client receives notification
// events
// Added a byteCount attribute to subscription requests (don't notify me
// until xx bytes have been received)
// Changed PortSubscribe() for the newly added (above) subscription
// attributes
// Added PortSetNotifyMask() to dynamically modify a client's subscription
// mask flag
//
// 4     12/05/01 11:57a Brian.meinke
// Added code to retry commands if they fail due to an invalid open file
// descriptor
//
// 3     11/30/01 4:10p Brian.meinke
// In PortUnsubscribe(): 'changedmsg.portUnsubscribe.flags = false' to
// 'changedmsg.portUnsubscribe.flags = flags'
//
// 2     11/30/01 2:52p Brian.meinke
// First working version
//
// 1     11/29/01 3:55p Brian.meinke
// Logical port driver interface client
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <devctl.h>

#include "tchar.h"
#include "ILogicalPort.h"
//#include "LogPortDefs.h"

#ifdef _NTO_VERSION
    #if (_NTO_VERSION >= 630)
        #include <errno.h>
        #define vsnprintf std::vsnprintf
    #endif
#endif

/**
 * Default constructor
 */
ILogicalPort::ILogicalPort() : ILogger(), m_portDrvChid( -1), m_outPortDrvFd( -1),
    m_inPortDrvFd( -1), m_portSubscrMode( 0), m_portSubscrMask(LOG_PORT_NOTIFY_NEVER),
    m_inPortProtocol( NULL), m_outPortProtocol( NULL), m_inPortName(""),
    m_outPortName(""), m_subHandle(LOG_PORT_INVALID_SUB), m_vehicleMessageXmlNode(NULL)
{
    m_initMessage = NULL;
}

/**
 * Destructor
 */
ILogicalPort::~ILogicalPort()
{
    if( m_initMessage != NULL)      free( m_initMessage);
    m_initMessage = NULL;
    if (m_vehicleMessageXmlNode != NULL)  delete m_vehicleMessageXmlNode;
    m_vehicleMessageXmlNode = NULL;
    ClosePort();
}

/**
 * Object initialization. Used to load the mapping of message tags
 * to  transmit/receive message string pairs for serial communication
 *
 * @param xmlFileName
 *               Name of the XML file to read object configuration data from
 * @return true if the message map table is successfully loaded, false
 *         if an error occurred
 */
bool ILogicalPort::Initialize( const string &xmlFileName)
{
    XmlParser       xmlParser;
    const XmlNode   *xmlDoc;

    try
    {
        xmlDoc = xmlParser.ReturnXMLDocument( xmlFileName);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "ILogicalPort::Initialize() Exception: %s\n", e.what());
    }

    return( Initialize( xmlDoc));
}

/**
 * Object initialization. Used to load the mapping of message tags
 * to  transmit/receive message string pairs for serial messaging
 *
 * @param portConfig
 *               XML node whose children contain the mappings of message tags
 *               to the sequence of serial byytes to be transmitted/received
 *               for the aorementioned message tag
 * @return true if the message map table is successfully loaded, false
 *         if an error occurred
 */
bool ILogicalPort::Initialize( const XmlNode* portConfig)
{
    bool retVal = true;
    const XmlNode *configRoot;

    if(portConfig != NULL)
    {
        // Get Vehicle Message XML node
        try
        {
            if (m_vehicleMessageXmlNode != NULL)  delete m_vehicleMessageXmlNode;
            m_vehicleMessageXmlNode = portConfig->getChild("SerialMessages/VehicleMessages")->Copy();
        }
        catch( XmlException &err)
        {
            Log( LOG_ERRORS, "\tXmlException in ILogicalPort::Initialize: %s\n",
                    err.GetReason());
            retVal = false;
        }
        try
        {
            // Initialize Logger interface
            ILogger::Initialize( portConfig);
        }
        catch( XmlException &err)
        {
        }

        try
        {
            // Read the mapping of message tags to byte strings
            configRoot = portConfig->getChild( "SerialMessages");
            retVal = ReadMessageMap( configRoot);
        }
        catch( XmlException &err)
        {
            Log( LOG_ERRORS, "\tXmlException in ILogicalPort::Initialize: %s\n",
                    err.GetReason());
            retVal = false;
        }
        catch( BepException &err)
        {
            Log( LOG_ERRORS, "\tBepException in ILogicalPort::Initialize: %s\n",
                    err.GetReason());
            retVal = false;
        }

        // Check for an initialization message
        try
        {
            ReadInitMessage( portConfig->getChild( XML_T( "SerialMessages/InitMessage")));
        }
        catch( XmlException &err)
        {
            Log( LOG_ERRORS, "\tBepException in ILogicalPort::Initialize InitMessage: %s\n", err.GetReason());
            if( m_initMessage != NULL)      free( m_initMessage);
            m_initMessage = NULL;
        }
    }
    else
        throw BepException("ILogicalPort::Initialze portConfig == NULL\n");


    return( retVal);
}

/**
 * Method used to read the low speed module initialization message
 * from the config file <InitMessage> node
 *
 * @param initMssgNode
 *               Pointer to the <InitMessage> node to read the low speed module
 *               initialization message byte from
 */
void ILogicalPort::ReadInitMessage( const XmlNode *initMssgNode)
{
    int                 ii=0, mSize;
    XmlNodeMapCItr      itr;
    const XmlNodeMap    &byteNodes = initMssgNode->getChildren();

    // Make sure we de-allocate previous allocation
    if( m_initMessage != NULL)  free( m_initMessage);
    m_initMessage = NULL;

    mSize = sizeof( *m_initMessage) + (byteNodes.size()-1) * sizeof( CommInitByte);
    m_initMessage = (CommInitMessage_t*)calloc( 1, mSize);
    m_initMessage->byteCount = byteNodes.size();

    for( itr=byteNodes.begin(); itr!=byteNodes.end(); itr++)
    {
        CommInitByte    &mssgByte = m_initMessage->mssg[ ii++];
        const XmlNode   *byteNode = itr->second;

        try
        {
            std::string junk = XmlToAscii( byteNode->getAttribute( XML_T( "byteType"))->getValue());
            if( (junk[ 0] == 'T') || (junk[ 0] == 't')) mssgByte.byteType = 0;
            if( (junk[ 0] == 'R') || (junk[ 0] == 'r')) mssgByte.byteType = 1;
        }
        catch( XmlException &err)
        {
            mssgByte.byteType = 0;
        }

        try
        {
            std::string junk = XmlToAscii(  byteNode->getAttribute( XML_T( "timeOut"))->getValue());
            mssgByte.timeOut = BposReadInt( junk.c_str());
        }
        catch( XmlException &err)
        {
            mssgByte.timeOut = 0;
        }

        try
        {
            std::string junk = XmlToAscii( byteNode->getAttribute( XML_T( "value"))->getValue());
            mssgByte.literalVal = BposReadInt( junk.c_str());
        }
        catch( XmlException &err)
        {
            mssgByte.literalVal = 0;
        }

        try
        {
            std::string junk = XmlToAscii( byteNode->getAttribute( XML_T( "refByte"))->getValue());
            mssgByte.refByte = BposReadInt( junk.c_str());
        }
        catch( XmlException &err)
        {
            mssgByte.refByte = -1;
        }

        try
        {
            std::string junk = XmlToAscii( byteNode->getAttribute( XML_T( "byteMask"))->getValue());
            mssgByte.byteMask = BposReadInt( junk.c_str());
        }
        catch( XmlException &err)
        {
            mssgByte.byteMask = 0;
        }

        try
        {
            std::string junk = XmlToAscii( byteNode->getAttribute( XML_T( "baud"))->getValue());
            mssgByte.baud = BposReadInt( junk.c_str());
        }
        catch( XmlException &err)
        {
            mssgByte.baud = 0;
        }
        try
        {
            std::string junk = XmlToAscii( byteNode->getAttribute( XML_T( "parity"))->getValue());
            if( (junk[ 0] == 'E') || (junk[ 0] == 'e')) mssgByte.parity = EVEN_PARITY;
            else if( (junk[ 0] == 'O') || (junk[ 0] == 'o')) mssgByte.parity = ODD_PARITY;
            else mssgByte.parity = NO_PARITY;
        }
        catch( XmlException &err)
        {
            mssgByte.parity = NO_PARITY;
        }
        try
        {
            std::string junk = XmlToAscii( byteNode->getAttribute( XML_T( "dataBits"))->getValue());
            if(BposReadInt( junk.c_str()) == 5) mssgByte.dataBits = BITS5;  
            else if(BposReadInt( junk.c_str()) == 6) mssgByte.dataBits = BITS6;  
            else if(BposReadInt( junk.c_str()) == 7) mssgByte.dataBits = BITS7;  
            else mssgByte.dataBits = BITS8;  
        }
        catch( XmlException &err)
        {
            mssgByte.dataBits = BITS8;
        }
        try
        {
            std::string junk = XmlToAscii( byteNode->getAttribute( XML_T( "stopBits"))->getValue());
            if(BposReadInt( junk.c_str()) == 2) mssgByte.stopBits = STOP2;  
            else mssgByte.stopBits = STOP1;  
        }
        catch( XmlException &err)
        {
            mssgByte.stopBits = STOP1;
        }
        try
        {
            std::string junk = XmlToAscii( byteNode->getAttribute( XML_T( "updatePortSettings"))->getValue());
            if(BposReadInt( junk.c_str()) == 1) mssgByte.updatePortSettings = true;  
            else mssgByte.updatePortSettings = false;  
        }
        catch( XmlException &err)
        {
            mssgByte.updatePortSettings = false;
        }
    }
}

/**
 * Opens a logical port for communication
 *
 * @param portName Name of the logical port to be opened
 * @param mode     Mode in which to open the port: O_RDONLY, O_WRONLY or
 *                 O_RDWR. The default is O_RDWR.
 * @return true if successfully opened, false on error
 */
bool ILogicalPort::OpenPort( const char *portName, int mode)
{
    bool    retVal = true;

    // If not currently opened
    if( m_inPortDrvFd == -1)
    {
        // Make full path to the port's dev pathname
        GetDriverPath(portName, m_inPortName);
        m_outPortName = m_inPortName;

        // Attempt to open the port
        if( (m_inPortDrvFd = open( m_inPortName.c_str(), mode)) == -1)
        {
            // STBY
            retVal = false;
            Log( LOG_ERRORS, "Error opening port driver '%s': %s\n", m_inPortName.c_str(),
                    strerror( errno));
        }
        else
        {
            // Same port to send and receive
            m_outPortDrvFd = m_inPortDrvFd;
        }
    }
    else
    {
        std::string newInPortName;

        GetDriverPath(portName, newInPortName);
        if( newInPortName != m_inPortName)
        {
            Log( LOG_ERRORS, "OpenPort() Error invalid port name: <%s> != <%s>\n", portName, m_inPortName.c_str());
            // Invalid request
            errno = EINVAL;
            retVal = false;
        }
    }

    return( retVal);
}

/**
 * Opens a logical port for communication
 *
 * @param inPortName Name of the logical port to be opened for receiving data
 * @param outPortName
 *                   Name of the logical port to be opened for transmitting data
 * @return true if successfully opened, false on error
 */
bool ILogicalPort::OpenPort( const char *inPortName, const char *outPortName)
{
    bool    retVal = true;

    // If not currently opened
    if( (m_inPortDrvFd == -1) && (m_outPortDrvFd != -1))
    {
        // Make full path to the port's dev pathname
        GetDriverPath(inPortName, m_inPortName);
        GetDriverPath(outPortName, m_outPortName);

        // Attempt to open the port
        if( (m_inPortDrvFd = open( m_inPortName.c_str(), O_RDONLY)) == -1)
        {
            // STBY
            retVal = false;
            Log( LOG_ERRORS, "Error opening input port driver '%s': %s\n", m_inPortName.c_str(),
                    strerror( errno));
        }
        else if( (m_outPortDrvFd = open( m_outPortName.c_str(), O_RDWR)) == -1)
        {
            // STBY
            retVal = false;
            Log( LOG_ERRORS, "Error opening output port driver '%s': %s\n", m_outPortName.c_str(),
                    strerror( errno));

            // Close the input driver
            close( m_inPortDrvFd);
            m_inPortDrvFd = -1;

        }
    }
    else
    {
        std::string newInPortName, newOutPortName;

        // Make full path to the port's dev pathname
        GetDriverPath(inPortName, newInPortName);
        GetDriverPath(outPortName, newOutPortName);
        if( (newInPortName != m_inPortName) || (newOutPortName != m_outPortName))
        {
            Log( LOG_ERRORS, "OpenPort() Error invalid port name: <%s,%s> != <%s,%s>\n", inPortName, outPortName,
                 m_inPortName.c_str(),m_outPortName.c_str());
            // Invalid request
            errno = EINVAL;
            retVal = false;
        }
    }

    return( retVal);
}


bool ILogicalPort::OpenPort( const string &portName, int mode /*=O_RDWR*/)
{
    return( OpenPort( portName.c_str(),mode));
}

/**
 * Closes the logical port
 */
void ILogicalPort::ClosePort()
{
    // Close the input driver
    if( m_inPortDrvFd != -1)
    {
        close( m_inPortDrvFd);
        if( m_inPortProtocol != NULL)
        {
            delete m_inPortProtocol;
            m_inPortProtocol = NULL;
        }
    }
    // Close output driver (If different input/output drivers)
    if( m_outPortDrvFd != m_inPortDrvFd)
    {
        close( m_outPortDrvFd);
        if( m_outPortProtocol != NULL)
        {
            delete m_outPortProtocol;
            m_outPortProtocol = NULL;
        }

    }

    m_inPortDrvFd = -1;
    m_outPortDrvFd = -1;

    m_portSubscrMode = 0;
    m_portSubscrMask = LOG_PORT_NOTIFY_NEVER;
}

/**
 * Request to lock the port for exclusive use. The flags parameter
 * allows you to specify a blocking mode for this operation: If O_NONBLOCK
 * is specified and the port is already locked by another process, an error
 * is returned. If O_NONBLOCK is NOT specified, this operation will block
 * until the port can be locked.
 *
 * @param flags  Optional flags argument.
 * @return true if the port was successfully locked, false if an error
 *         occurred (use errno for more detailed error information)
 */
bool ILogicalPort::LockPort( int flags)
{
    logPortMsg_t    msg;
    int             error = ETIMEDOUT;
    int             retry = 3;

    do
    {
        msg.lockPort.flags = flags;
        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_LOCK, &msg, sizeof( msg), NULL);

    }while((error != EOK) && (retry--));

    return( error == EOK);
}

/**
 * Request to lock the port for exclusive use. The flags parameter
 * allows you to specify a blocking mode for this operation: If O_NONBLOCK
 * is specified and the port is already locked by another process, an error
 * is returned. If O_NONBLOCK is NOT specified, this operation will block
 * until the port can be locked.
 *
 * @param flags  Optional flags argument.
 * @return true if the port was successfully locked, false if an error
 *         occurred (use errno for more detailed error information)
 */
int ILogicalPort::LockPortDebug( int flags)
{
    logPortMsg_t    msg;
    int             error = ETIMEDOUT;
    int             retry = 3;

    do
    {
        msg.lockPort.flags = flags;
        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_LOCK, &msg, sizeof( msg), NULL);

    }while((error != EOK) && (retry--));

    return( error );
}

/**
 * Request to unlock a port. A successful call to LockPort() must have
 * been issued for this method to succeed.
 *
 * @return true if port was unlocked, false if an error occurred (use errno
 *         for more detailed error information)
 */
bool ILogicalPort::UnlockPort()
{
    int             error;
    int             retry = 3;

    do
    {
        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_UNLOCK, NULL, 0, NULL);
    }while((error != EOK) && (retry--));

    return( error == EOK);
}
/**
 * Request to unlock a port. A successful call to LockPort() must have
 * been issued for this method to succeed.
 *
 * @return true if port was unlocked, false if an error occurred (use errno
 *         for more detailed error information)
 */
int ILogicalPort::UnlockPortDebug()
{
    int             error;
    int             retry = 3;

    do
    {
        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_UNLOCK, NULL, 0, NULL);
    }while((error != EOK) && (retry--));

    return( error == EOK);
}

/**
 * Requests to be notified when port events occur. The bitwise
 * flag argument lets you specify which port events you want
 * notifications for. The current port events that are supported are:
 *   PORT_SUBSCRIBE_RX to be notified of received data
 *   PORT_SUBSCRIBE_TX to be notified of transmit data
 *   PORT_SUBSCRIBE_ALL to be notified of all port events
 *   PORT_SUBSCRIBE_LOCK to be notified when someone locks thport
 *   PORT_SUBSCRIBE_UNLOCK to be notified when someone unlocks thport
 *   PORT_NOTIFY_EXISTING to be notified if the port driver FIFOs
 *                        currently contain data
 * When the desired port event(s) occur, the event will be delivered
 * to the client. It is recommended that pulses be used for notifications
 * because the driver can deliver a bitwise flag indicating which
 * events occurred in the pulse's value.sival_int member.
 *
 * @param event      Event to be used by the driver to deliver the notification events
 * @param flags      Bitwise flag indicating which port events you want notifications
 *                   for. The current port events that are supported are:
 *                   PORT_SUBSCRIBE_RX, PORT_SUBSCRIBE_TX, PORT_SUBSCRIBE_ALL
 *                   PORT_SUBSCRIBE_LOCK, PORT_SUBSCRIBE_UNLOCK and PORT_NOTIFY_EXISTING
 * @param modeMask   Mask bits to filter when a notifications should be sent by the server
 *                   (default is LOG_PORT_NOTIFY_ALWAYS)
 * @param byteCount  Number of bytes that the server should buffer before sending the
 *                   notification to the client
 * @param fltrString Optional filter string used to filter incoming data string
 * @return true if successfully registered for notifications, false if an
 *         error occurred (use errno for more detailed error information)
 * @see NotifyEvent_t
 * @see logPortNotifyMask_t
 */
bool ILogicalPort::PortSubscribe(const NotifyEvent_t &event, int flags, int modeMask,
                                 int byteCount, const LogPortFilterString *fltrString)
{
    logPortMsg_t    msg;
    int             error, ii;

    // If not already subscribed
    if( m_subHandle == LOG_PORT_INVALID_SUB)
    {
        // Hold onto subscription mask in case of comm error
        m_portSubscrMode = flags;
        m_portSubscrMask = modeMask;

        // Check if a filter string was specified
        if( fltrString != NULL)
        {
            m_portSubscrMode |= PORT_NOTIFY_FILTER;
            flags |= PORT_NOTIFY_FILTER;
            for( ii=0; ii<(int)fltrString->size() && ii<LOG_PORT_FLTR_STRING_LEN; ii++)
            {
                msg.portSubscribe.filterString[ ii] = (*fltrString)[ii];
            }
            msg.portSubscribe.filterByteCount = ii;
        }

        msg.portSubscribe.flags = flags;
        msg.portSubscribe.notifyEvent = event;
        msg.portSubscribe.modeMask = modeMask;
        msg.portSubscribe.byteCount = byteCount;

        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_SUBSCR, &msg, sizeof( msg), NULL);
        // If file descriptor is invalid
        if( error == EOK)
        {
            m_subHandle = msg.portSubscribeReply.handle;

            // Keep a copy in case of port comm errors
            m_portSubscEvent = event;
        }

    }
    else
    {
        Log(LOG_ERRORS, "Handle was poop\n");
        errno = error = EINVAL;
    }

    return( error == EOK);
}

bool ILogicalPort::FilterSubscribe(const NotifyEvent_t &event, const string &rcvTag)
{
    bool                   retVal = false;
    ComMessageMapItr_t     itr;
    LogPortFilterStringList    rcvFilters;

    // Lookup the reply strings for the given message tag
    if( (itr=m_mssgTags.find( rcvTag)) != m_mssgTags.end())
    {
        const ComMssgTableEntry& mssgTbl = itr->second;
        mssgTbl.GetFilterStringList( rcvFilters);
        // If any filter strings associated with the given tag
        if( rcvFilters.size() > 0)
        {
            // Use first reply filter string as the default
            LogPortFilterStringListItr_t itr = rcvFilters.begin();
            const LogPortFilterString& filter = *itr;
            int replyLen = filter.size();

            // Subscribe ONLY for this type of vehicle response
            retVal = PortSubscribe(event,
                                   PORT_SUBSCRIBE_RX,
                                   LOG_PORT_NOTIFY_ALWAYS,
                                   replyLen,
                                   &filter);
        }
        else
        {
            printf( "No reply filters associated with message tag '%s'\n",
                    rcvTag.c_str());
        }
    }
    else
    {
        printf( "Message tag '%s' not found in message tag list\n",
                rcvTag.c_str());
    }

    return( retVal);
}

/**
 * Request to be "unregistered" for port events. The flags
 * parameter specifies which port events you want to be
 * unregistered for.
 *
 * @param flags  Bitwise flag indicating which port events you want notifications
 *               for. The current port events that are supported are:
 *               PORT_SUBSCRIBE_RX, PORT_SUBSCRIBE_TX, PORT_SUBSCRIBE_ALL and
 *               PORT_NOTIFY_EXISTING
 *
 * @return true if successfully "unregistered", false if an error
 *         occurred (use errno for more detailed error information)
 */
bool ILogicalPort::PortUnsubscribe( int flags)
{
    logPortMsg_t    msg;
    int             error;

    if( m_subHandle != LOG_PORT_INVALID_SUB)
    {
        msg.portUnsubscribe.flags = flags;
        msg.portUnsubscribe.handle = m_subHandle;
        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_UNSUBSCR, &msg, sizeof( msg), NULL);
        // If file descriptor is invalid
        if( error == EOK)
        {
            m_portSubscrMode &= ~(flags);
            m_subHandle = LOG_PORT_INVALID_SUB;
        }
    }
    else
    {
        errno = error = EINVAL;
    }

    return( error == EOK);
}

/**
 * Sets or changes the notification mask for port events.
 *
 * @param newMask New bit-wise filter mask for when to be notified of events we're
 *                subscribed for
 * @return true if successful, false if an error occurred (use errno for
 *         more detailed error information)
 */
bool ILogicalPort::PortSetNotifyMask( int newMask)
{
    logPortMsg_t    msg;
    int             error = EINVAL;

    if( m_subHandle != LOG_PORT_INVALID_SUB)
    {
        // Don't change subscription events
        msg.portSubscribe.flags = 0;
        //Only modify the notification filter mask
        msg.portSubscribe.modeMask = newMask;

        // Send an "empty" subscription request with new mask
        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_SUBSCR, &msg, sizeof( msg), NULL);

        // If mask successfully changed
        if( error == EOK)
        {
            // Update our mask holder
            m_portSubscrMask = newMask;
        }
    }
    else
    {
        errno = error = EINVAL;
    }

    return( error == EOK);
}

/**
 * Request to change port prtotcol parameters.
 *
 * @param portProtocol
 *               New port protocol to apply to the port
 *
 * @return true if successful, false if an error occurred (use errno for
 *         more detailed error information)
 */
bool ILogicalPort::PortSetProtocol( const SerialProtocol_t &portProtocol)
{
    int     error;

    // Apply new settings
    error = devctl( m_inPortDrvFd, DCMD_CHR_TCSETATTR, (void*)&portProtocol, sizeof( portProtocol), NULL);

    return( error == EOK);
}

/**
 * Read current port prtotcol parameters. The current
 * port protocol is returned in the portProtocol argument.
 *
 * @param portProtocol
 *               Current protocol as returned by the serial driver
 *
 * @return true if successful, false if an error occurred (use errno for
 *         more detailed error information)
 */
bool ILogicalPort::PortGetProtocol( SerialProtocol_t &portProtocol)
{
    int     error;

    // Apply new settings
    error = devctl( m_inPortDrvFd, DCMD_CHR_TCGETATTR, (void*)&portProtocol, sizeof( portProtocol), NULL);

    return( error == EOK);
}

/**
 * Request to change port prtotcol parameters used to receive data.
 * The current port protocol is returned in the oldPRotocol argument.
 * The flags argument allows you to specify which data members of
 * the portProtocol structure are valid: PORT_SET_BAUD,
 * PORT_SET_PARITY,  PORT_SET_DATA_BITS, PORT_SET_STOP_BITS,
 * PORT_SET_INTR, PORT_SET_BYTE_GAP, PORT_SET_MSSG_GAP and PORT_SET_ALL
 *
 * @param portProtocol
 *               New receive port protocol to apply to the port
 * @param oldProtocol
 *               Buffer to place the current port protocol used to receive data
 * @param flags  Bitwise compination of flags saying which data members of
 *               the portProtocol struct are valid: PORT_SET_BAUD
 *               PORT_SET_PARITY, PORT_SET_DATA_BITS, PORT_SET_STOP_BITS,
 *               PORT_SET_INTR, PORT_SET_BYTE_GAP, PORT_SET_MSSG_GAP and
 *               PORT_SET_ALL. THe default value is PORT_SET_ALL
 * @return true if successful, false if an error occurred (use errno for
 *         more detailed error information)
 */
bool ILogicalPort::PortSetRxProtocol( SerialProtocol_t &portProtocol,
                                      SerialProtocol_t *oldProtocol,
                                      int flags)
{
    bool retVal;

    // Read current protocol
    if( oldProtocol != NULL)
    {
        PortGetProtocol( *oldProtocol);
    }

    // Set the special BEP 'rx attribute' flag
    portProtocol.m_tio.c_iflag |= 0x80000000;

    // Apply new settings
    if( (retVal=PortSetProtocol(portProtocol)) == true)
    {
        // Keep a copy of the new protocol settings in case of comm error
        if( m_inPortProtocol != NULL)
        {
            delete m_inPortProtocol;
        }
        m_inPortProtocol = new SerialProtocol_t;
        memmove(m_inPortProtocol, &portProtocol, sizeof(portProtocol));
    }

    return( retVal);
}

/**
 * Request to change port prtotcol parameters used to transmit data.
 * The current port protocol is returned in the oldPRotocol argument.
 * The flags argument allows you to specify which data members of
 * the portProtocol structure are valid: PORT_SET_BAUD,
 * PORT_SET_PARITY,  PORT_SET_DATA_BITS, PORT_SET_STOP_BITS,
 * PORT_SET_INTR, PORT_SET_BYTE_GAP, PORT_SET_MSSG_GAP and PORT_SET_ALL
 *
 * @param portProtocol
 *               New transmit port protocol to apply to the port
 * @param oldProtocol
 *               Buffer to place the current port protocol used to transmit data
 * @param flags  Bitwise compination of flags saying which data members of
 *               the portProtocol struct are valid: PORT_SET_BAUD
 *               PORT_SET_PARITY, PORT_SET_DATA_BITS, PORT_SET_STOP_BITS,
 *               PORT_SET_INTR, PORT_SET_BYTE_GAP, PORT_SET_MSSG_GAP and
 *               PORT_SET_ALL. THe default value is PORT_SET_ALL
 * @return true if successful, false if an error occurred (use errno for
 *         more detailed error information)
 */
bool ILogicalPort::PortSetTxProtocol( const SerialProtocol_t &portProtocol,
                                      SerialProtocol_t *oldProtocol,
                                      int flags)
{
    bool retVal;

    // Read current protocol
    if( oldProtocol != NULL)
    {
        PortGetProtocol( *oldProtocol);
    }

    // If file descriptor is invalid
    if( (retVal=PortSetProtocol(portProtocol)) == true)
    {
        // Keep a copy of the new protocol settings in case of comm error
        if( m_outPortProtocol != NULL)
        {
            delete m_outPortProtocol;
        }
        m_outPortProtocol = new SerialProtocol_t;
        memmove(m_outPortProtocol, &portProtocol, sizeof(portProtocol));
    }

    return( retVal);
}



bool ILogicalPort::PortSetBaud( speed_t newBaud, speed_t *oldBaud/*=NULL*/)
{
    SerialProtocol_t    newProtocol;
    bool                retVal;

    // Read current protocol
    tcgetattr( m_inPortDrvFd, &newProtocol.m_tio);
    if( oldBaud != NULL)
    {
        *oldBaud = cfgetospeed( &newProtocol.m_tio);
    }

    // Set new speed in termios struct
    cfsetospeed( &newProtocol.m_tio, newBaud);
    cfsetispeed( &newProtocol.m_tio, newBaud);
    retVal = PortSetProtocol( newProtocol);
    return( retVal);
}

/**
 * Sets a new parity for the port
 *
 * @param newParity New parity to apply to the port
 * @param oldParity Buffer to receive the current parity setting (can be
 *                  NULL)
 * @return true if successful, false if an error occurred (use errno for
 *         more detailed error information)
 * @see Parity_t
 */
bool ILogicalPort::PortSetParity( Parity_t newParity, Parity_t *oldParity/*=NULL*/)
{
    SerialProtocol  newProtocol;
    bool            retVal;

    // Read current protocol
    PortGetProtocol( newProtocol);

    if( oldParity != NULL)  *oldParity = newProtocol.GetParity();

    newProtocol.SetParity( newParity);

    // Set new parity
    retVal = PortSetProtocol( newProtocol);
    return( retVal);
}

/**
 * Sets a new data bit count for the port
 *
 * @param newDataBits Number of data bits to apply to the port
 * @param oldDataBits Buffer to receive the current number of data bits for
 *                  the port (can be NULL)
 * @return true if successful, false if an error occurred (use errno for
 *         more detailed error information)
 * @see DataBits_t
 */
bool ILogicalPort::PortSetDataBits( DataBits_t newDataBits, DataBits_t *oldDataBits/*=NULL*/)
{
    SerialProtocol  newProtocol;
    bool                retVal;

    // Read current protocol
    PortGetProtocol( newProtocol);

    if( oldDataBits != NULL)    *oldDataBits = newProtocol.GetDataBits();

    // set new protocol
    newProtocol.SetDataBits( newDataBits);
    retVal = PortSetProtocol( newProtocol);
    return( retVal);
}

/**
 * Sets a new stop bit count for the port
 *
 * @param newStopBits
 *               Number of stop bits to apply to the port
 * @param oldStopBits
 *               Buffer to receive the current number of stop bits for
 *               the port (can be NULL)
 * @return true if successful, false if an error occurred (use errno for
 *         more detailed error information)
 * @see StopBits_t
 */
bool ILogicalPort::PortSetStopBits( StopBits_t newStopBits, StopBits_t *oldStopBits/*=NULL*/)
{
    SerialProtocol  newProtocol;
    bool            retVal;

    // Read current protocol
    PortGetProtocol( newProtocol);

    if( oldStopBits != NULL)    *oldStopBits = newProtocol.GetStopBits();

    newProtocol.SetStopBits( newStopBits);
    retVal = PortSetProtocol( newProtocol);
    return( retVal);
}

/**
 * Sets a new inter-byte gap for the port
 *
 * @param newByteGap New inter-byte gap (in ms) to apply to the port
 * @param oldByteGap Buffer to receive the current inter-byte gap for the
 *                   port (can be NULL)
 * @return true if successful, false if an error occurred (use errno for
 *         more detailed error information)
 */
bool ILogicalPort::PortSetByteGap( uint32_t newByteGap, uint32_t *oldByteGap/*=NULL*/)
{
    SerialProtocol  newProtocol;
    bool            retVal;

    // Read current protocol
    PortGetProtocol( newProtocol);

    if( oldByteGap != NULL) *oldByteGap = newProtocol.GetByteGap();

    newProtocol.SetByteGap( newByteGap);
    // Set new protocol
    retVal = PortSetProtocol( newProtocol);

    return( retVal);
}

/**
 * Sets a new inter-message gap for the port
 *
 * @param newMssgGap New inter-message gap (in ms) to apply to the port
 * @param oldMssgGap Buffer to receive the current inter-message gap for the
 *                   port (can be NULL)
 * @return true if successful, false if an error occurred (use errno for
 *         more detailed error information)
 */
bool ILogicalPort::PortSetMssgGap( uint32_t newMssgGap, uint32_t *oldMssgGap/*=NULL*/)
{
    SerialProtocol  newProtocol;
    bool            retVal;

    // Read current protocol
    PortGetProtocol( newProtocol);

    if( oldMssgGap != NULL) *oldMssgGap = newProtocol.GetMsgGap();

    newProtocol.SetMsgGap( newMssgGap);

    // Set new protocol
    retVal = PortSetProtocol( newProtocol);

    return( retVal);
}



/**
 * Reads data from the logical port
 *
 * @param buff     Buffer to read the data into
 * @param len      Number of bytes to read from theport
 * @param initWait Number of milliseconds to wait for the first character to arrive:
 *                 0: Return immediately if no data is available (non-blocking read)
 *                 -1: Use same value as endWait
 *                 -2: No initTime timeout (Block until all data has been received)
 * @param endWait  Number of milliseconds to wait for subsequent characters to arrive
 *                 after the first character has been received:
 *                 0: Return immediately if no additional data is available (non-blocking read)
 *                 -1: Use same value as initWait
 *                 -2: No endTime timeout (Block until all data has been received)
 *
 * @return Number of bytes read, -1 if an error occurred or EOF was reached
 *         (use errno to check for error or EOF condition)
 */
int ILogicalPort::ReadPort( uint8_t *buff, size_t len, long initWait, long endWait)
{
    int                     bytesRead, totalRead=0;
    struct sigevent         timeoutEvent;
    SerialString_t          response;
    uint64_t                mssgTimeout = 0;
    bool                    haveTimeout = true;

    SIGEV_UNBLOCK_INIT( &timeoutEvent);

    Log( LOG_DETAILED_DATA, "ILogicalPort::ReadPort(initWait: %ld, endWait: %ld)\n", initWait, endWait);
    // Can't both be -1, so default to a blocking read
    if( (initWait == -1) && (endWait == -1))    initWait = endWait = -2;
    // If supposed to use same value as endWait
    else if( initWait == -1)                    initWait = endWait;
    // If supposed to use same value as initWait
    else if( endWait == -1)                     endWait = initWait;

    // Convert msec timeouts to nano-seconds
    if( initWait >= 0)      mssgTimeout = mSEC_nSEC( initWait);
    else                    haveTimeout = false;

    // clear out the buffer
    memset(buff, 0, len);
    Log( LOG_DETAILED_DATA, "Start looping for data - mssgTimeout: %lld, initWait: %ld, endWait: %ld\n", mssgTimeout, initWait, endWait);
    // Loop until all data received (or timeout on variable length reply)
    while( (size_t)totalRead < len)
    {
        // If we have some timeouts for this read
        if( haveTimeout)
        {
            Log( LOG_DETAILED_DATA, "Setting timeout to %lld - {initWait: %ld, endWait: %ld}\n", mssgTimeout, initWait, endWait);
            // Set a timeout on the read operation to try and detect the end
            TimerTimeout( CLOCK_MONOTONIC, _NTO_TIMEOUT_REPLY|_NTO_TIMEOUT_SEND, NULL, &mssgTimeout, NULL);
        }
        else
        {
            Log( LOG_DETAILED_DATA, "No timeout provided - mssgTimeout: %lld, initWait: %ld, endWait: %ld\n", mssgTimeout, initWait, endWait);
        }

        // Read data from logical port
        if( (bytesRead = read( m_inPortDrvFd, &buff[ totalRead], len-totalRead)) == -1)
        {
            // if nothing read and an error occurred, set the status to -1
            if(totalRead == 0)  totalRead = -1;
            // if the return it EINTER, reset to ETIMEDOUT (Resource Manager Thing)
            if(errno == EINTR)  errno = ETIMEDOUT;
            // Print out reason for read() failure
            Log( LOG_DETAILED_DATA, "ILogicalPort::ReadPort: %s\n", strerror( errno));
            break;
        }
        else
        {
            // If we actually received something
            if( bytesRead > 0)
            {
                // Build up a response string to use for variable length response
                //  optomizations
                response += SerialString_t( &buff[ totalRead], bytesRead);

                PrintSerialString( "ReadPort", &buff[ totalRead], bytesRead, LOG_DETAILED_DATA);

                // Update total nuymber of bytes read
                totalRead += bytesRead;
            }
            else if (bytesRead == 0)
            {
                Log( LOG_DETAILED_DATA, "ILogicalPort::ReadPort read zero bytes from port\n");
                break;
            }
            else
            {   // Something is not right
                Log( LOG_DETAILED_DATA|LOG_ERRORS, "ILogicalPort::ReadPort read returned %d -- UNEXPECTED!\n", bytesRead);
                break;
            }

            // If we have an end of message timeout gap time specified
            if( endWait >= 0)
            {
                mssgTimeout = mSEC_nSEC( endWait);
                haveTimeout = true;
                Log( LOG_DETAILED_DATA, "Have timeout, resetting mssgTimeout to %ld\n", endWait);
            }
            else
            {
                mssgTimeout = mSEC_nSEC( 0);
                haveTimeout = false;
                Log( LOG_DETAILED_DATA, "Do NOT Have timeout, resetting mssgTimeout to 0\n");
            }
        }
    }

    return( totalRead);
}

/**
 * Reads data from the logical port
 *
 * @param buff     Buffer to read the data into
 * @param initWait Number of milliseconds to wait for the first character to arrive:
 *                 0: Return immediately if no data is available (non-blocking read)
 *                 -1: Use same value as endWait
 *                 -2: No initTime timeout (Block until all data has been received)
 * @param endWait  Number of milliseconds to wait for subsequent characters to arrive
 *                 after the first character has been received:
 *                 0: Return immediately if no additional data is available (non-blocking read)
 *                 -1: Use same value as initWait
 *                 -2: No endTime timeout (Block until all data has been received)
 *
 * @return Number of bytes read, -1 if an error occurred or EOF was reached
 *         (use errno to check for error or EOF condition)
 */
int ILogicalPort::ReadPort( SerialString_t &buff, long initWait, long endWait)
{
    int retVal = -1;
    int origLen = buff.size();
    const int len = 1024;
    uint8_t tempBuff[ len];
    int bytesRead = 0;

    // Read until no more data available
    while( (bytesRead=ReadPort( tempBuff, len, initWait, endWait)) > 0)
    {
        buff += SerialString_t( tempBuff, bytesRead);

        // Calculate total number of bytes received so far
        retVal = buff.size() - origLen;

        // If we received the full amount that our temp buffer can hold
        if( bytesRead == len)
        {
            // Check for more data using our endWait for all timeouts
            initWait = endWait;
        }
        else
        {
            break;
        }
    }

    // No data received (and we haven't recevied any prior data)
    if( (bytesRead == 0) && (retVal < 0))   retVal = 0;

    return( retVal);
}

/**
 * Reads the reply message for the serial message with the given
 * message tag (as read form the config file)
 *
 * @param mssgTag    Tag of the message to read
 * @param respString Buffer to put the response string into
 * @return Number of bytes read if successful,
 *         -1 if an error occurred (use errno for more detailed error
 *         information)
 */
int ILogicalPort::ReadPort( const string &mssgTag, SerialString_t &respString)
{
    const size_t        len = 1024;
    uint8_t             buff[ len];
    int                 readCount = -1;
    ComMessageMapItr_t  itr;
    long                initWait=-2, endWait=0;

    // Check for a custom reply timeout
    itr = m_mssgTags.find( mssgTag);
    if( itr != m_mssgTags.end())
    {
        ComMssgTableEntry   &mssgEntry = itr->second;

        initWait = nSEC_mSEC( mssgEntry.GetReplyTimeout());
        if( initWait == 0)
            initWait = -2;

        endWait = nSEC_mSEC( mssgEntry.GetEndMssgGap());
    }

    // Wait for a response to be received
    readCount = ReadPort( buff, len, initWait, endWait);
    if( readCount >= 0)
    {
        respString = SerialString_t( buff, readCount);

        // Check if the response matches any of the expected responses
        if( IsResponseValid( mssgTag, respString) == false)
        {
            Log( LOG_DETAILED_DATA|LOG_ERRORS, "Response string is invalid (%d bytes)\n", readCount);
            readCount = -1;
            errno = ENOMSG;
        }
        else
        {
            Log( LOG_DETAILED_DATA|LOG_ERRORS, "Response string is valid (%d bytes)\n", readCount);
        }
    }

    return( readCount);
}

/**
 * Writes data to the port
 *
 * @param buff       Buffer of data to write
 * @param len        Number of bytes to write to the port
 * @param retryCount Number of times to re-send the data if the previous send fails
 * @return Number of bytes written if successful,
 *         -1 if an error occurred (use errno for more
 *         detailed error information)
 */
int ILogicalPort::WritePort( const uint8_t *buff, size_t len, int retryCount)
{
    int retVal = 0;
    int ii;

    // Make several attempts to send the data
    for( ii=0; (ii<retryCount) && (retVal != (int)len); ii++)
    {
        if( (retVal = write( m_outPortDrvFd, buff, len)) == -1)
        {
            Log( LOG_ERRORS, "ILogicalPort::WritePort() Error writing to port");
            // If timeout occurred
            if( errno == ETIMEDOUT)
            {
                // Reset our connection to the server
                ResetConnection();
            }
        }
        PrintSerialString( "WritePort", buff, len);
    }

    return( retVal);
}

/**
 * Writes data to the port. The NULL terminator is NOT written
 *
 * @param buff   Buffer of data to write
 * @param retryCount Number of times to re-send the data if the previous send fails
 * @return Number of bytes written if successful,
 *         -1 if an error occurred (use errnofor more
 *         detailed error information)
 */
int ILogicalPort::WritePort( const SerialString_t &buff, int retryCount)
{
// If this line returns an error in bastring.h the file needs to be fixed.
// file is: /usr/qnx630/target/qnx6/usr/include/g++-3/std/bastring.h
// Add a type cast the 'return ""' -> 'return (charT const *)""'
    return( WritePort( buff.c_str(), buff.length(), retryCount));
}

/**
 * Send the transmit message for the serial message with the given
 * message tag (as read form the config file)
 *
 * @param mssgTag Tag of the message to send
 * @return Number of bytes written if successful,
 *         -1 if an error occurred (use errno for more detailed error
 *         information)
 */
int ILogicalPort::WritePort( const string &mssgTag)
{
    int                 retVal = -1;
    ComMessageMapItr_t  itr;
    SerialString_t      xmtString;

    // Lookup the transmit data for this message
    itr = m_mssgTags.find( mssgTag);
    if( itr != m_mssgTags.end())
    {
        ComMssgTableEntry   &mssgEntry = itr->second;
        const int64_t       &timeOut = mssgEntry.GetReplyTimeout();

        // Set proper reply timeout before sending message
        if( timeOut != 0)
        {
            SetReplyTimeout( timeOut);
        }

        // Check if we need to set a new transmit protocol before sending
        //  this message
        if( SetXmtProtocol( mssgEntry) == true)
        {
            // Get the serial data to send
            const string &msgFmt = mssgEntry.GetXmtString();
            int retryCount = mssgEntry.GetRetryCount();

            // Parse the ascii-string into a SerialString_t
            if( GetBinaryMssg( msgFmt, xmtString) == true)
            {
                // Send the message
                retVal = WritePort(xmtString, retryCount);
                if( retVal == (int)xmtString.length())
                {
                    if( SetRcvProtocol( mssgEntry) == false)
                    {
                        retVal = -1;
                    }
                }
            }
        }
    }
    else
    {
        errno = ENOMSG;
    }

    return( retVal);
}

/**
 * Applies a temporary (current message only) reply timeout in nanoseconds.
 * This timeout will effect the curent message transaction only.
 *
 * @param nsTimeout Number of nanosecnds to wait for a reply before a timeout should be
 *                  raised.
 * @return 0 if successful, -1 if error
 */
int ILogicalPort::SetReplyTimeout( const int64_t &nsTimeout)
{
    logPortMsg_t    msg;
    int             error;
    int             retVal = -1;

    // If port opened
    if( m_outPortDrvFd != -1)
    {
        if( nsTimeout >= 0)
        {
            // Set reply timeout for next transmit
            msg.replyTimeout.nsTimeout = nsTimeout;

            error = devctl( m_inPortDrvFd, DCMD_LOGPORT_TIMEOUT, &msg, sizeof( msg), NULL);
            // If file descriptor is invalid
            if( error == EOK)
            {
                retVal = 0;
            }
        }
        else if( nsTimeout == (int64_t)mSEC_nSEC( -2))
        {
            // Dont pass invalid values to the driver. a value of -2
            //  is used internally to denote 'block forever'
            retVal = 0;
        }
        else
        {
            errno = error = EINVAL;
        }
    }
    else
    {
        errno = error = EINVAL;
    }

    return( retVal);
}


/**
 * Forms a fully qualified path to the logical port driver reponsible
 * for managing traffic for the given logical port name
 *
 * @param portName   Name of the logical port driver to get the path for
 * @param driverPath Buffer to receive the fully qualified path to the port driver
 */
void ILogicalPort::GetDriverPath( const char *portName, string &driverPath)
{
    const int len = strlen( portName) + strlen( LOG_PORT_DEV_ROOT) + 2;
    char portPath[ len];

    portPath[ 0] = 0;
    // If thisis not an absolute path
    if( portName[ 0] != '/')
    {
        // Use the default logical port driver root
        strcpy( portPath, LOG_PORT_DEV_ROOT);
    }
    strcat( portPath, portName);

    driverPath = portPath;
}

/**
 * Checks if a response string from the vehicle is a valid
 * response for the given message tag
 *
 * @param mssgTag  Message tag to use when looking up valid message response
 *                 strings
 * @param response Response string from the vehicle
 * @return true if the given response string is a valid response
 */
bool ILogicalPort::IsResponseValid( const string &mssgTag, const SerialString_t &response) const
{
    bool                retVal=false;
    ComMessageMapCItr_t mssgItr;

    // Find the message information for this message tag
    if( (mssgItr=m_mssgTags.find( mssgTag)) != m_mssgTags.end())
    {
        // Get the message table entry for this message tag
        const ComMssgTableEntry &mssgEntry = mssgItr->second;

        // Check if the response string we have matches any of the
        //  response filter strings from the config file
        retVal = mssgEntry.IsValidResponse(response);
    }

    return( retVal);
}

/**
 * Checks to see if the given string represents a positive reply or a
 * negative reply for the responses associated with the given message
 * tag. If the string does not match any of the receive filters for the
 * given message tag, it is considered to be a negative response.
 *
 * @param mssgTag  Tag of the message whose response filters we wish to test the
 *                 response string against
 * @param response The resopnse string we received
 *
 * @return true if the string represents a positive reply, false otherwise
 */
bool ILogicalPort::IsPositiveResponse( const string &mssgTag, const SerialString_t &response) const
{
    bool                retVal=false;
    ComMessageMapCItr_t mssgItr;

    // Find the message information for this message tag
    if( (mssgItr=m_mssgTags.find( mssgTag)) != m_mssgTags.end())
    {
        // Get the message table entry for this message tag
        const ComMssgTableEntry &mssgEntry = mssgItr->second;

        // Check if the response string we have represents a positive or negative response
        //  for the response filters associated with the given message tag
        retVal = mssgEntry.IsPositiveResponse(response);
    }

    return( retVal);
}

/**
 * Check if we need to set a new transmit protocol before sending a message
 *
 * @param mssgEntry ComMssgTableENtry object which contains the message to be sent and,
 *                  potentially, a new transmit protocol required to send the message
 * @return
 */
bool ILogicalPort::SetXmtProtocol( ComMssgTableEntry &mssgEntry)
{
    bool                retVal = true;

    // If we need a new transmit protocol or  a new reseive protocol for this
    //  message
    if( (mssgEntry.IsXmtProtocolValid()) || (mssgEntry.IsRcvProtocolValid()) )
    {
        // Assume something fails below
        retVal = false;

        // Lock the port before setting new protocol
        if( (retVal=LockPort()) == true)
        {
            if( mssgEntry.IsXmtProtocolValid())
            {
                SerialProtocol_t    newProtocol;
                mssgEntry.GetXmtProtocol( newProtocol);
                // Set new protocol and get a copy of the previous
                retVal = PortSetTxProtocol( newProtocol, &m_tempTxPortProtocol);
            }
        }
    }

    return( retVal);
}

/**
 * Check if we need to set a new receive protocol before sending a message
 *
 * @param mssgEntry ComMssgTableENtry object which contains the message to be sent and,
 *                  potentially, a new transmit protocol required to send the message
 * @return
 */
bool ILogicalPort::SetRcvProtocol( ComMssgTableEntry &mssgEntry)
{
    bool                retVal = true;
    SerialProtocol_t    *oldProtocol = NULL;

    // If we have a new receive protocol
    if( mssgEntry.IsRcvProtocolValid())
    {
        // Assume something below fails
        retVal = false;

        // If noe transmit protocol was specified
        if( !(mssgEntry.IsXmtProtocolValid()))
        {
            // Need to get a copy of the existing port protocol
            oldProtocol = &m_tempRxPortProtocol;
        }

        SerialProtocol_t    newProtocol;
        mssgEntry.GetRcvProtocol( newProtocol);

        // Set new protocol and get a copy of the previous
        retVal = PortSetRxProtocol( newProtocol, oldProtocol);
    }
    // If a transmit protocol was specified for this message
    else if( mssgEntry.IsXmtProtocolValid())
    {
        // Restore the original port protocol
        PortSetRxProtocol( m_tempRxPortProtocol, NULL);
    }

    return( retVal);
}

/**
 * Reset our connection to the logical port (clears FIFOs)
 *
 * @return EOK if successful, any other value indicates an error code
 */
int ILogicalPort::ResetConnection()
{
    int             error = EOK;

    error = devctl( m_inPortDrvFd, DCMD_LOGPORT_RST_CONN, NULL, 0, NULL);
    if( (error == EOK) && ( m_inPortName != m_outPortName))
    {
        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_RST_CONN, NULL, 0, NULL);
    }

    return( error);
}

/**
 * Resets the port driver (re-initialize)
 *
 * @return EOK if successful, any other value indicates an error code
 */
int ILogicalPort::ResetServer()
{
    int             error = EOK;

    error = devctl( m_inPortDrvFd, DCMD_LOGPORT_RST_DRV, NULL, 0, NULL);

    if( (error == EOK) && (m_inPortName != m_outPortName))
    {
        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_RST_CONN, NULL, 0, NULL);
    }

    return( error);
}

/**
 * Asserts a break condition on the port for the specified number of nano-seconds.
 *
 * @param duration Number of milli-seconds to assert break for
 * @return EOK if successful, any other value indicates an error code
 */
int ILogicalPort::AssertBreak( int duration)
{
    int error = EOK;

    if( tcsendbreak( m_outPortDrvFd, duration) != 0)
    {
        error = errno;
    }

    return( error);
}

/**
 * Establish a communication link with a module by driving
 * the communication channel low for a period of time and
 * then driving the signal high.
 *
 * @param lowDuration The time in milliseconds to drive the line low.
 * @param hiDuration  The time in milliseconds to drive the time high.
 * @param initMssg    Initialization message to send to the module after the initialization waveform is done
 * @param mssgLen     Number of bytes in the initialization message
 * @return return EOK if successful, any other value indicates an error code
 */
int ILogicalPort::EstablishComms( UINT32 lowDuration, UINT32 hiDuration, const uint8_t *initMssg, const int mssgLen)
{
    logPortEstablishComms_t *msg;
    int                     nbytes = 0;
    int                     error = ETIMEDOUT;
    int                     retry = 3;

    nbytes = sizeof( logPortEstablishComms_t)-1 + mssgLen;
    msg = (logPortEstablishComms_t*)calloc(1, nbytes);
    // If an initialization message was passed in
    if( (mssgLen > 0) && (initMssg != NULL))
    {
        // Copy the initialization message to the devctl message
        memcpy( msg->initMssg, initMssg, mssgLen);
        msg->mssgLen = mssgLen;
    }

    // set up the hi and low times
    msg->nsLowTime = mSEC_nSEC( lowDuration);
    msg->nsHiTime = mSEC_nSEC( hiDuration);

    do
    {
        // send the command to the port driver to establish connections
        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_EST_COMMS, msg, nbytes, NULL);
        // If the attempt was unsuccessful, wait a bit before attempting again
        if(EOK != error) 
        {
        Log( LOG_ERRORS, "Error Establishing Comms: %s\n", strerror(errno));
            BposSleep(300);
        }
        else
        {
            Log(LOG_DEV_DATA, "Established comms with the module");
        }
    }while((error != EOK) && (retry--));

    free( msg);

    return( error);
}

/**
 * Performs a 5-baud module initialization
 *
 * @return return EOK if successful, any other value indicates an error code
 */
int ILogicalPort::LowSpeedInit()
{
    int     nbytes;
    int     retry = 3;
    int     error = ENODATA;

    if( m_initMessage != NULL)
    {
        // Calculate the size of the message to send to the server
        nbytes = sizeof(*m_initMessage) +
                (m_initMessage->byteCount - 1) * sizeof(CommInitByte);
        do
        {
            // send the command to the port driver to establish connections
            error = devctl( m_inPortDrvFd, DCMD_LOGPORT_5BAUD_INIT, m_initMessage, nbytes, NULL);
            Log( LOG_ERRORS, "LowSpeedInit Error: %s\n", strerror(errno));

        }while((error != EOK) && (retry--));
    }
    else
        Log(LOG_ERRORS, "Unable to perform a low speed initialization m_initMessage is empty!\n");

    errno = error;

    return( error);
}

/**
 * Requests the comm proxy to re-establish a connection to the
 * underlying communication driver
 *
 * @return return EOK if successful, any other value indicates an error code
 */
int ILogicalPort::ReconnectDriver()
{
    int error = EPERM;

    // Must have the port locked before issuing the reconnect request
    if( true == LockPort())
    {
        // Ask the comm proxy to re-connect
        error = devctl( m_inPortDrvFd, DCMD_LOGPORT_RECONNECT, NULL, 0, NULL);

        if( (error == EOK) && (m_inPortName != m_outPortName))
        {
            error = devctl( m_inPortDrvFd, DCMD_LOGPORT_RECONNECT, NULL, 0, NULL);
        }

        UnlockPort();
    }
    else
    {
        error = ENOLCK;
    }

    return( error);
}

void ILogicalPort::RegisterPortDriver(void)
{
    MsgSendPulse(m_inPortDrvFd, SIGEV_PULSE_PRIO_INHERIT, REGISTER_PULSE, 0);
}

/**
 * Reads the mapping of message tags to their corresponding serial
 * byte strings
 *
 * @param mssgMapNode
 *               Parent XML node of the nodes containing the message tag names and
 *               serial byte string pairs
 * @return true if the message tag mappings were read successfully, false otherwise
 */
bool ILogicalPort::ReadMessageMap( const XmlNode *mssgMapNode)
{
    XmlNodeMapCItr  xmlItr;
    string          xmtString;
    SerialProtocol  xmtProtocol, rcvProtocol;
    bool            retVal = true;

    try
    {
        // Check if a message map exists
        const XmlNode *mssgNode = mssgMapNode->getChild(VEH_MESSAGES_TAG);
        try
        {
            const XmlNodeMap &mssgNodes = mssgNode->getChildren();

            // Iterate all the nodes and fill in our map
            for( xmlItr=mssgNodes.begin(); xmlItr!=mssgNodes.end() && retVal == true; xmlItr++)
            {
                // The first item is the message tag
                const std::string &mssgTag = xmlItr->first;
                ComMssgTableEntry   mssgEntry;
                int retryCount = ReadRetryCount( mssgMapNode, mssgTag);
                INT32 rxFailureRetryCount = ReadResponseFailureRetryCount( mssgMapNode, mssgTag);
                INT32 responsePendingReads = ReadResponsePendingReads( mssgMapNode, mssgTag);

                if( retryCount < 0) retryCount = 3;
                mssgEntry.SetRetryCount( retryCount);
                mssgEntry.SetResponseFailureRetryCount( rxFailureRetryCount);
                mssgEntry.SetResponsePendingReads( responsePendingReads);

                if( ReadTransmitString( mssgMapNode, mssgTag, xmtString) == true)
                {
                    mssgEntry.SetTransmitString( xmtString);

                    if( ReadXmtProtocol( mssgMapNode, mssgTag, xmtProtocol) == true)
                    {
                        mssgEntry.SetTransmitProtocol(xmtProtocol);
                    }

                    // Read the list of valid response strings for this tag
                    if( ReadResponseString( mssgMapNode, mssgTag, mssgEntry) == true)
                    {
                        // Check if this reply message has a special response protocol
                        if( ReadRcvProtocol( mssgMapNode, mssgTag, rcvProtocol) == true)
                        {
                            mssgEntry.SetReceiveProtocol( rcvProtocol);
                        }
                    }
                    else
                    {
                        Log(LOG_ERRORS, "\tError reading response string '%s'\n", mssgTag.c_str());
                        retVal = false;
                    }
                    m_mssgTags.insert( ComMessageMapVal_t( mssgTag, mssgEntry));
                }
                else
                {
                    Log(LOG_ERRORS, "\tError reading transmit string '%s'\n", mssgTag.c_str());
                    retVal = false;
                }
            }
        }
        catch( XmlException &err)
        {
            Log(LOG_ERRORS, "\tError reading messagemap: %s\n", err.GetReason());
            retVal = false;
        }
    }
    catch( XmlException &err)
    {
        // This is OK. If we are here it simply means there is no message
        //  map node to read.
        Log(LOG_ERRORS, "\tERROR: No message map in ILogicalPort::ReadMessageMap\n");
    }

    return( retVal);
}

/**
 * Reads the number of times a message should be re-sent in case
 * of communication problems
 *
 * @param configNode Parent XML node containing the mapping of message tags
 *                   to serial byte strings
 * @param mssgTag    The message tag to read the serial transmit byte string for
 * @return Number of times to retry
 */
int ILogicalPort::ReadRetryCount( const XmlNode *configNode, const string &mssgTag)
{
    int     retVal = 3;
    XmlString   mainMssgTag = AsciiToXml( mssgTag);

    try
    {
        // Read Retry count for this message
        const XmlNode *xmlNode;
        xmlNode = configNode->getChild( VEH_MESSAGES_TAG)->getChild( mainMssgTag)->getChild( XMT_RETRY_TAG);
        retVal = BposReadInt( XmlToAscii( xmlNode->getValue()).c_str());
    }
    catch( XmlException &err)
    {
        // Default to 3 retries if no count specified in file
        retVal = 3;
    }

    return( retVal);
}

/**
 * Reads the number of times a message should be re-sent if the response is determined 
 * to be incorrect
 *
 * @param configNode Parent XML node containing the mapping of message tags
 *                   to serial byte strings
 * @param mssgTag    The message tag to read the serial transmit byte string for
 * @return Number of times to retry
 */
int ILogicalPort::ReadResponseFailureRetryCount( const XmlNode *configNode, const string &mssgTag)
{
    int     retVal = -1;
    XmlString   mainMssgTag = AsciiToXml( mssgTag);

    try
    {
        // Read Retry count for this message
        const XmlNode *xmlNode;
        xmlNode = configNode->getChild( VEH_MESSAGES_TAG)->getChild( mainMssgTag)->getChild( XMT_RESPONSE_FAILURE_RETRY_TAG);
        retVal = BposReadInt( XmlToAscii( xmlNode->getValue()).c_str());
    }
    catch( XmlException &err)
    {
        // Default to 3 retries if no count specified in file
        retVal = -1;
    }

    return( retVal);
}

/**
 * Reads the number of response pending reads
 *
 * @param configNode Parent XML node containing the mapping of message tags
 *                   to serial byte strings
 * @param mssgTag    The message tag to read the serial transmit byte string for
 * @return Number of times to retry
 */
int ILogicalPort::ReadResponsePendingReads( const XmlNode *configNode, const string &mssgTag)
{
    int     retVal = -1;
    XmlString   mainMssgTag = AsciiToXml( mssgTag);

    try
    {
        // Read Retry count for this message
        const XmlNode *xmlNode;
        xmlNode = configNode->getChild( VEH_MESSAGES_TAG)->getChild( mainMssgTag)->getChild( XMT_RESPONSE_PENDING_READS_TAG);
        retVal = BposReadInt( XmlToAscii( xmlNode->getValue()).c_str());
    }
    catch( XmlException &err)
    {
        // Default to 3 retries if no count specified in file
        retVal = -1;
    }

    return( retVal);
}

/**
 * Reads the serial byte string to be transmitted for the given
 * message tag from the XML nodes conatined under the mssgMap
 * parent XML node
 *
 * @param configNode   Parent XML node containing the mapping of message tags
 *                  to serial byte strings
 * @param mssgTag   The message tag to read the serial transmit byte string for
 * @param stringVal The buffer into which the serial transmit byte string will
 *                  be read
 * @return true if the transmit byte string was successfully read,
 *         false otherwise
 */
bool ILogicalPort::ReadTransmitString( const XmlNode *configNode,
                                       const string &mssgTag,
                                       string &stringVal)
{
    bool        retVal = true;
    XmlString   mainMssgTag = AsciiToXml( mssgTag);

    try
    {
        // Read which tag to use for the transmit node name
        const XmlNode *xmlNode;
        xmlNode = configNode->getChild( VEH_MESSAGES_TAG)->getChild( mainMssgTag)->getChild( XML_T(XMT_MESSAGE_TAG));
        const XmlString &xmtTag = xmlNode->getValue();

        // Look-up this tag in the transmit messages section
        xmlNode = configNode->getChild( XMT_MESSAGES_TAG)->getChild( xmtTag);
        stringVal = XmlToAscii( xmlNode->getValue());
        // Trim off any leading garbage
        int         junkStart;
        if( (junkStart=stringVal.find_first_of( "0x")) != 0)
        {
            stringVal.erase( 0, junkStart);
        }

        // Replace wildcard characters with transmit interpretation bytes
        retVal = ResolveTransmitInterpretation( configNode, mssgTag, stringVal);
    }
    catch( XmlException &err)
    {
        Log(LOG_ERRORS, "\tError reading transmit string %s: %s\n",
             mssgTag.c_str(), err.GetReason());
        retVal = false;
    }

    return( retVal);
}

/**
 * Fills in any wildcard characters in a transmit byte string with
 * serial byte values from the Transmit Interpretation section node
 * associated with the transmit message tag given by the mssgTag
 * parameter
 *
 * @param configNode   Parent XML node containing the mapping of message tags
 *                  to serial byte strings
 * @param mssgTag   The message tag to read the transmit interpretation bytes for
 * @param stringVal The buffer into which the transmit interpretation bytes will
 *                  be read
 * @return true if the transmit interpretation bytes were successfully read,
 *         false otherwise
 */
bool ILogicalPort::ResolveTransmitInterpretation( const XmlNode *configNode,
                                                  const string &mssgTag,
                                                  string &stringVal)
{
    bool                retVal = true;
    string              xmtIntrp;
    const string        wc( "?");
    const string        delim(",");
    string::size_type   wcIdx=0;
    string              newMsgFormat;

    // If any wildcard characters to be replaced
    if( stringVal.find_first_of( wc, wcIdx) != string::npos)
    {
        try
        {
            // Read the char values to use when replacing wildcards
            retVal = ReadTransmitInterpretation( configNode, mssgTag, xmtIntrp);
            if( retVal == true)
            {
                wcIdx = 0;
                // Find all wildcard specifiers in the transmit string
                while( (wcIdx=stringVal.find_first_of( wc, wcIdx)) != string::npos)
                {
                    const char *intrpString = xmtIntrp.c_str();
                    if( stringVal[ wcIdx+1] == '?')
                    {
                        // Replace '0x??'
                        if( xmtIntrp.length() > 3)
                        {
                            // Replace '0x??' with '0x%02X'
                            stringVal.replace( wcIdx, 2, &intrpString[ 2], 2);
                        }
                        else
                        {
                            // Replace '0x??' with '0x%02X'
                            stringVal.replace( wcIdx, 2, "%02X");
                        }
                    }
                    else if( stringVal[ wcIdx-1] == 'x')    // 0x?n
                    {
                        // Replace '0x?n'
                        if( xmtIntrp.length() > 3)
                        {
                            // Replace '0x?n' with '0xMn'
                            stringVal.replace( wcIdx, 1, &intrpString[ 2], 1);
                        }
                        else
                        {
                            // Replace '0x?n' with '0x%01Xn'
                            stringVal.replace( wcIdx, 1, "%01X");
                        }
                    }
                    else if( stringVal[ wcIdx-2] == 'x')    // 0xn?
                    {
                        // Replace '0xn?'
                        if( xmtIntrp.length() > 3)
                        {
                            // Replace '0xn?' with '0xMn'
                            stringVal.replace( wcIdx, 1, &intrpString[ 3], 1);
                        }
                        else
                        {
                            // Replace '0x?n' with '0xn%01X'
                            stringVal.replace( wcIdx, 1, "%01X");
                        }
                        // Replace '0xn?' with '0xn%01X'
                        stringVal.replace( wcIdx, 1, "%01X");
                    }

                    // Remove this replace string
                    if( xmtIntrp.length() > 4)
                    {
                        xmtIntrp.erase(0,5);
                    }
                    else if( xmtIntrp.length() == 4)
                    {
                        xmtIntrp.clear();
                    }
                }
            }
        }
        catch( XmlException &err)
        {
            Log(LOG_ERRORS, "\tXmlException in ILogicalPort::ResolveTransmitInterpretation: %s\n",
                    err.GetReason());
            retVal = false;
        }
    }

    return( retVal);
}

/**
 * Reads the transmit interpretation bytes for the transmit message
 * with the message tag given in the mssgTag parameter. Transmit
 * intrepretation bytes are used to fill in the wildcard characters
 * contained in a transmit message string.
 *
 * @param configNode   Parent XML node containing the mapping of message tags
 *                  to serial byte strings
 * @param mssgTag   The message tag to read the transmit interpretation bytes for
 * @param stringVal The buffer into which the transmit interpretation bytes will
 *                  be read
 * @return true if the transmit interpretation bytes were successfully read,
 *         false otherwise
 */
bool ILogicalPort::ReadTransmitInterpretation( const XmlNode *configNode,
                                               const string &mssgTag,
                                               string &stringVal)
{
    bool        retVal = true;
    XmlString   mainMssgTag = AsciiToXml( mssgTag);
    int         junkStart;

    try
    {
        // Read which tag to use for the transmit node name
        const XmlNode *xmlNode;
        xmlNode = configNode->getChild( VEH_MESSAGES_TAG)->getChild( mainMssgTag)->getChild( XMT_INTERP_TAG);
        const XmlString &xmtTag = xmlNode->getValue();

        if( xmtTag.empty() == false)
        {
            // Look-up this tag in the transmit interpretations section
            xmlNode = configNode->getChild( XMT_INTERPS_TAG)->getChild( xmtTag);
            stringVal = XmlToAscii( xmlNode->getValue());
            // Trim off any leading garbage
            if( (junkStart=stringVal.find_first_of( "0x")) != 0)
            {
                stringVal.erase( 0, junkStart);
            }
        }
        retVal = true;
    }
    catch( XmlException &err)
    {
        Log(LOG_ERRORS, "\tError reading transmit interpretation string %s: %s\n",
             mssgTag.c_str(), err.GetReason());
        retVal = false;
    }

    return( retVal);
}

/**
 * Reads the transmit protocol for the transmit message with the
 * message tag given in the mssgTag parameter. If a transmit protocol
 * is specified, the following events will occur:
 *     1. The port will be locked
 *     2. The transmit protocol will be applied to the port
 *     3. The message data will be sent
 *     4. If a receive protocol is specified, it will be applied to the
 *        port immediately after the transmit data has been sent
 *
 * @param configNode Parent XML node containing the mapping of message tags
 *                   to serial byte strings
 * @param mssgTag    The message tag to read the transmit protocol for
 * @param xmtProtocol
 *                   Transmit protocol for the given message
 * @return true if the transmit interpretation bytes were successfully read,
 *         false otherwise
 */
bool ILogicalPort::ReadXmtProtocol( const XmlNode *configNode,
                                    const string &mssgTag,
                                    SerialProtocol &xmtProtocol)
{
    bool        retVal = false;
    XmlString   mainMssgTag = AsciiToXml( mssgTag);

    try
    {
        // Read which tag to use for the transmit node name
        const XmlNode *xmlNode;
        xmlNode = configNode->getChild( VEH_MESSAGES_TAG)->getChild( mainMssgTag)->getChild( XMT_PROTOCOL_TAG);
        const XmlString &protTag = xmlNode->getValue();

        // If a transmit protocol tag was specified
        if( protTag.empty() == false)
        {
            // Look-up this tag in the transmit protocols section
            xmlNode = configNode->getChild( XMT_PROTOCOLS_TAG)->getChild( protTag);

            // Read the transmit protocol for the given message
            xmtProtocol.Initialize( xmlNode);

            retVal = true;
        }
    }
    catch( XmlException &err)
    {
    }

    return( retVal);
}

/**
 * Reads the valid serial response string for the given message
 * tag from the XML nodes conatined under the configNode parent
 * XML node
 *
 * @param configNode   Parent XML node containing the mapping of message tags
 *                  to serial byte strings
 * @param mssgTag   The message tag to read the serial receive string for
 * @param stringVal The buffer into which the serial receive byte string will
 *                  be read
 * @return true if the receive byte string was successfully read,
 *         false otherwise
 */
bool ILogicalPort::ReadResponseString( const XmlNode *configNode,
                                       const string &mssgTag,
                                       ComMssgTableEntry &mssgEntry)
{
    bool            retVal = true;
    XmlString       mainMssgTag = AsciiToXml( mssgTag);
    string          respFilter;
    int             junkStart;
    int64_t         replyTimeout = 0LL;
    int64_t         endMssgGap;
    RespLenType_t   respLenType = FIXED_LEN;
    bool            isPositive;

    try
    {
        // Read which tag to use for the transmit node name
        const XmlNode *xmlNode;
        xmlNode = configNode->getChild( VEH_MESSAGES_TAG)->getChild( mainMssgTag)->getChild( RCV_MESSAGE_TAG);
        const XmlString &rcvTag = xmlNode->getValue();

        // Look-up this tag in the ReplyMessages section
        xmlNode = configNode->getChild( RCV_MESSAGES_TAG)->getChild( rcvTag);
        try
        {
            // Read the response timeout value for this message
            const XmlNode* timeoutNode = xmlNode->getAttribute( XML_T("ReplyTimeout"));
            _tsscanf( timeoutNode->getValue().c_str(), XML_T("%lld"), &replyTimeout);
            replyTimeout = mSEC_nSEC( replyTimeout);
        }
        catch( ...)
        {
            replyTimeout = (int64_t)mSEC_nSEC( -2);
        }
        mssgEntry.SetReplyTimeout( replyTimeout);


        const XmlNodeMap &respNodes = xmlNode->getChildren();
        XmlNodeMapCItr  itr;

        // Each child node contains a response filter string
        for( itr=respNodes.begin(); itr!=respNodes.end(); itr++)
        {
            const XmlNode *rcvNode = itr->second;

            respFilter = XmlToAscii( rcvNode->getValue());

            // Trim off any leading garbage
            if( (junkStart=respFilter.find_first_of( "0x")) != 0)
            {
                respFilter.erase( 0, junkStart);
            }

            // Assume fixed length reply strings
            respLenType = FIXED_LEN;

            // Check if this response is a variable length response
            if( ReadRespLenType( rcvNode, respLenType) == true)
            {
                // Read the end of message timeout value
                if( ReadRcvEndGapTime( rcvNode, endMssgGap) == true)
                {
                    // If the end of message timeout for this string
                    //  is greater than any of the other end of message
                    //  timeouts
                    if( endMssgGap > mssgEntry.GetEndMssgGap())
                    {
                        // Use this timeout as the timeout when
                        //  reading a response for this message tag
                        mssgEntry.SetEndMssgGap( endMssgGap);
                    }
                }
            }

            // see what type of response this is (positive or negative)
            isPositive = ReadResponseType( rcvNode);

            mssgEntry.AddResponseFilter( respFilter.c_str(), respLenType, isPositive);
        }
    }
    catch( XmlException &err)
    {

        Log(LOG_ERRORS, "\tError reading receive string %s: %s\n",
             mssgTag.c_str(), err.GetReason());
        retVal = false;
    }

    return( retVal);
}

/**
 * Reads the receive interpretation bytes for the receive message
 * with the message tag given in the mssgTag parameter. Transmit
 * intrepretation bytes are used to fill in the wildcard characters
 * contained in a transmit message string.
 *
 * @param configNode   Parent XML node containing the mapping of message tags
 *                  to serial byte strings
 * @param mssgTag   The message tag to read the receive interpretation bytes for
 * @param stringVal The buffer into which the receive interpretation bytes will
 *                  be read
 * @return true if the receive interpretation bytes were successfully read,
 *         false otherwise
 */
bool ILogicalPort::ReadResponseInterpretation( const XmlNode *configNode,
                                               const string &mssgTag,
                                               string &stringVal)
{
    return( true);
}

/**
 * Reads the receive protovcol for the reply message with the message
 * tag given in the mssgTag parameter. The protocl( if it exists) will
 * be applied to the port immediately following the successfull
 * transmission of the transmit string
 *
 * @param configNode Parent XML node containing the mapping of message tags
 *                   to serial byte strings
 * @param mssgTag    The message tag to read the transmit protocol for
 * @param rcvProtocol
 *                   Transmit protocol for the given message
 * @return true if the transmit interpretation bytes were successfully read,
 *         false otherwise
 */
bool ILogicalPort::ReadRcvProtocol( const XmlNode *configNode,
                                    const string &mssgTag,
                                    SerialProtocol &rcvProtocol)
{
    bool        retVal = false;
    XmlString   mainMssgTag = AsciiToXml( mssgTag);

    try
    {
        // Read which tag to use for the reply node name
        const XmlNode *xmlNode;
        xmlNode = configNode->getChild( VEH_MESSAGES_TAG)->getChild( mainMssgTag)->getChild( RCV_PROTOCOL_TAG);
        const XmlString &protTag = xmlNode->getValue();

        // If a reply protocol tag was specified
        if( protTag.empty() == false)
        {
            // Look-up this tag in the transmit protocols section
            xmlNode = configNode->getChild( RCV_PROTOCOLS_TAG)->getChild( protTag);

            // Read the transmit protocol for the given message
            rcvProtocol.Initialize( xmlNode);

            retVal = true;
        }
    }
    catch( XmlException &err)
    {
    }

    return( retVal);
}

/**
 * Reads the reply length type (fixed or variable length reply string) for the
 * reply message with the message tag given in the mssgTag parameter.
 *
 * @param respNode Node containing the attributes for a module reply string
 * @param respLenType
 *                 Buffer to receive the reply string length type (FIXED_LEN or VAR_LEN)
 *                 as read from the config file
 * @return true if the reply length type was successfully read,
 *         false otherwise
 */
bool ILogicalPort::ReadRespLenType( const XmlNode *respNode, RespLenType_t &respLenType)
{
    bool        retVal = false;

    // Default to a fixed length reply string
    respLenType = FIXED_LEN;

    // Read the response timeout value for this message
    try
    {
        // Look-up this tag in the ReplyMessages section
        const XmlNode *lenNode = respNode->getAttribute( XML_T("lengthType"));
        if( lenNode->getValue() == XmlString( XML_T( "Variable")) )
        {
            respLenType = VAR_LEN;
        }
        Log(LOG_ERRORS, "\tReply Length Type for '%s' = %s (%d)\n", respNode->getName().c_str(),
                lenNode->getValue().c_str(), (int)respLenType);

        retVal = true;
    }
    catch( ...)
    {
    }

    return( retVal);
}

/**
 * Reads the end of message timeout gap time for the reply message with the
 * message tag given in the mssgTag parameter. The "end gap time" is used to
 * determine when the last character of a variable length reply string has been
 * received via a timeout condition on a read operation.
 *
 * @param respNode Node containing the attributes for a module reply string
 * @param endMssgGap Buffer to receive the end of message timeout gap time as read from
 *                   the config file
 * @return true if the reply length type was successfully read,
 *         false otherwise
 */
bool ILogicalPort::ReadRcvEndGapTime( const XmlNode *respNode, int64_t &endMssgGap)
{
    bool        retVal = false;
    int64_t     replyTimeout = 0LL;

    // Read the response timeout value for this message
    try
    {
        const XmlNode *xmlNode = respNode->getAttribute( XML_T("endGapTime"));
        _tsscanf( xmlNode->getValue().c_str(), XML_T("%lld"), &replyTimeout);
        endMssgGap = (int64_t)mSEC_nSEC( replyTimeout);

        Log(LOG_ERRORS, "\tEnd Gap Time for '%s' = %lld ns\n", respNode->getName().c_str(), endMssgGap);
        retVal = true;
    }
    catch( ...)
    {
        endMssgGap = 0LL;
    }

    return( retVal);
}

/**
 * Reads the response type for a message filter. The response type
 * will be either 'positive' or 'negative'.
 *
 * @param respNode Node containing the attributes for a module reply string
 *
 * @return true if the response string represents a positive response or
 *         false if it represents a negative response
 */
bool ILogicalPort::ReadResponseType( const XmlNode *respNode)
{
    bool        retVal = true;

    // Read the response timeout value for this message
    try
    {
        const XmlString &respType = respNode->getAttribute( XML_T("responseType"))->getValue();
        // Either positive or negative, so only look at first character
        if( (respType[ 0] == 'n') || (respType[ 0] == 'N'))
        {
            retVal = false;
        }
    }
    catch( ...)
    {
        retVal = true;
    }

    return( retVal);
}


/**
 * Set the status of the debug messages.
 *
 * @param value  The new debug status. On or Off
 * @since Version 1.0
 */
void ILogicalPort::SetDebug(const bool value)
{
    Debuggable::SetDebug(value);
}


/**
 * Parses an ascii string of comma seperated fields of serial
 * message bytes (i.e. "0x12,0x34,0x56") into a binary sequence
 * of unsigned character bytes ( i.e. [0x12 0x34 0x56] )
 *
 * @param asciiMsg String of comma seperated serial bytes
 * @param binMsg   Binary array of bytes representing a serial string
 * @return true if the message was properly parsed
 */
bool ILogicalPort::GetBinaryMssg( const string &asciiMsg, SerialString_t &binMsg)
{
    const char *ptr1, *ptr2;
    uint8_t srlByte;

    ptr2 = ptr1 = asciiMsg.c_str();

    // Fields in string are comma seperated hex values
    while( (ptr2=strchr( ptr1, ',')) != NULL)
    {
        // Convert this field to unsigned int
        srlByte = atoh( ptr1);
        // Add to binary message
        binMsg.push_back( srlByte);
        // Start looking after the field we just converted
        ptr1 = ptr2+1;
    }
    // If any fields left over
    if( (ptr1 != NULL) && (ptr1[0] != 0))
    {
        // Convert last field
        srlByte = atoh( ptr1);
        binMsg.push_back( srlByte);
    }

    return( true);
}

/**
 * Returns the file descriptor used to communicate cia the logical port
 *
 * @return The file descriptor used to communicate cia the logical port
 */
int ILogicalPort::GetPortFd() const
{
    return( m_inPortDrvFd);
}


/**
 * Fills in wildcard characters in a vehicle message string with
 * byte values passed in the variable argument list
 *
 * @param outString Formatted serial string (comma seperated byte fields)
 * @param fmt       Base serial mesage string with printf-style wildcard
 *                  delimitter fields used to build the final serial string
 */
void ILogicalPort::FormatAsciiMssg( string &outString, const char *fmt, ...)
{
    // If any wildcards to replace
    if( strchr( (char*)fmt, '%') != NULL)
    {
        va_list args;
        int mssgSize = 1;
        int ii, len = strlen( fmt);

        // Count number of commas to determine number of bytes in the mnessage
        for( ii=0; ii<len; ii++)
        {
            if( fmt[ ii] == ',')    mssgSize++;
        }
        // Allocate buffer big enough for fully formatted string
        //  // Each binary byte = 0x??, = 5 ascii characters
        const int buffSize = mssgSize * 5;
        char tmpBuff[ buffSize+1];

        // Fill in wildcard characters
        va_start( args, fmt);
        vsnprintf( tmpBuff, buffSize, fmt, args);
        va_end( args);

        outString = tmpBuff;
    }
    else
    {
        // No wildcards...assign format string to the output string
        outString = string( fmt);
    }
}


/**
 * Outputs a serial string via the logger
 *
 * @param prefix    String to pre-pend to output (i.e. "Response" would
 *                  result in "Response: $12...")
 * @param srlString Serial string to be printed
 * @param verboseMask
 *                  Optional logger verbose mask. Defaults to LOG_DETAILED_DATA
 */
void ILogicalPort::PrintSerialString( const char *prefix, const SerialString_t &srlString, int verboseMask)
{
    PrintSerialString( prefix, srlString.c_str(), srlString.length(), verboseMask);
}

/**
 * Outputs a serial string via the logger
 *
 * @param prefix    String to pre-pend to output (i.e. "Response" would
 *                  result in "Response: $12...")
 * @param srlString Serial string to be printed
 * @param len       Number of bytse in the serial string
 * @param verboseMask
 *                  Optional logger verbose mask. Defaults to LOG_DETAILED_DATA
 */
void ILogicalPort::PrintSerialString( const char *prefix, const uint8_t *srlString, uint32_t len, int verboseMask)
{
    std::string     logStr;
    char            junk[ 6];
    uint32_t        ii;

    if( GetVerboseMask() & verboseMask)
    {
        // Avoid SIGSEGV
        if( prefix == NULL) prefix = " ";

        logStr = std::string(prefix) + std::string(": ");

        // Build logger string
        for( ii=0; ii<len; ii++)
        {
            sprintf( junk, "$%02hhX ", srlString[ ii]);
            logStr+= std::string( junk);
        }
        logStr += std::string( "\n");

        Log( verboseMask, logStr.c_str());
    }
}


//////////// class ComMssgTableEntry methods //////

/**
 * Default constructor
 */
ComMssgTableEntry::ComMssgTableEntry() :
    m_retryCount( 3), m_xmtProtocolValid( false), m_rcvProtocolValid( false),
    m_replyTimeout( 0LL), m_endMssgGap( 0LL), m_respLenType( FIXED_LEN),
    m_responsePendingReads(-1),m_responseFailureRetryCount(-1)
{
}

/**
 * Copy constructor
 *
 * @param copy   Objec tto be copied into *this
 */
ComMssgTableEntry::ComMssgTableEntry( const ComMssgTableEntry &copy) :
    m_retryCount( 3), m_xmtProtocolValid( false), m_rcvProtocolValid( false),
    m_replyTimeout( 0LL), m_endMssgGap( 0LL), m_respLenType( FIXED_LEN),
    m_responsePendingReads(-1),m_responseFailureRetryCount(-1)
{
    *this = copy;
}

/**
 * Destructor
 */
ComMssgTableEntry::~ComMssgTableEntry()
{
}

/**
 * Check if a valid transmit protocol is specified for this message
 *
 * @return true if a vaild transmit protocol is specified for this message,
 *         false otherwise
 */
bool ComMssgTableEntry::IsXmtProtocolValid()
{
    return( m_xmtProtocolValid);
}

/**
 * Check if a valid receive protocol is specified for this message
 *
 * @return true if a vaild receive protocol is specified for this message,
 *         false otherwise
 */
bool ComMssgTableEntry::IsRcvProtocolValid()
{
    return( m_rcvProtocolValid);
}

/**
 * Sets the number of times the message should be re-transmitted
 * if communication problems are encountered
 *
 * @param retryCount The number of times the message should be re-transmitted
 *                   if communication problems are encountered
 */
void ComMssgTableEntry::SetRetryCount( int retryCount)
{
    m_retryCount = retryCount;
}

/**
 * Returns the number of times the message should be re-transmitted
 * if communication problems are encountered
 *
 * @return The number of times the message should be re-transmitted
 *         if communication problems are encountered
 */
int ComMssgTableEntry::GetRetryCount() const
{
    return( m_retryCount);
}
/**
 * Sets the number of times the message should be re-transmitted
 * if communication problems are encountered
 *
 * @param retryCount The number of times the message should be re-transmitted
 *                   if communication problems are encountered
 */
void ComMssgTableEntry::SetResponseFailureRetryCount( int responseFailureRetryCount)
{
    m_responseFailureRetryCount = responseFailureRetryCount;
}

/**
 * Returns the number of times the message should be re-transmitted
 * if communication problems are encountered
 *
 * @return The number of times the message should be re-transmitted
 *         if communication problems are encountered
 */
int ComMssgTableEntry::GetResponseFailureRetryCount() const
{
    return( m_responseFailureRetryCount);
}
/**
 * Sets the number of times the message should be re-transmitted
 * if communication problems are encountered
 *
 * @param retryCount The number of times the message should be re-transmitted
 *                   if communication problems are encountered
 */
void ComMssgTableEntry::SetResponsePendingReads( int responsePendingReads)
{
    m_responsePendingReads = responsePendingReads;
}

/**
 * Returns the number of times the message should be re-transmitted
 * if communication problems are encountered
 *
 * @return The number of times the message should be re-transmitted
 *         if communication problems are encountered
 */
int ComMssgTableEntry::GetResponsePendingReads() const
{
    return( m_responsePendingReads);
}

/**
 * Returns the ascii string of comma seperated transmit message bytes
 *
 * @return The ascii string of comma seperated transmit message bytes
 */
const string &ComMssgTableEntry::GetXmtString() const
{
    return( m_xmtString);
}

/**
 * Sets a new transmit string value
 *
 * @param xmtString New Transmit string value
 */
void ComMssgTableEntry::SetTransmitString( const char *xmtString)
{
    m_xmtString = string( xmtString);
}

/**
 * Sets a new transmit string value
 *
 * @param xmtString New Transmit string value
 */
void ComMssgTableEntry::SetTransmitString( const string &xmtString)
{
    m_xmtString = xmtString;
}

/**
 * Sets a new transmit protocol
 *
 * @param xmtProtocol New Transmit protocol value
 */
void ComMssgTableEntry::SetTransmitProtocol( SerialProtocol &xmtProtocol)
{
    m_xmtProtocol = xmtProtocol;
    m_xmtProtocolValid = true;
}

/**
 * Sets the timeout value (in nanoseconds) for response messages from
 * the module.
 *
 * @param nsTimeout Timeout value (in nanoseconds) for response messages from
 *                  the module
 */
void ComMssgTableEntry::SetReplyTimeout( const int64_t& nsTimeout)
{
    m_replyTimeout = nsTimeout;
}

/**
 * Returns the timeout value (in nanoseconds) for response messages from
 * the module.
 *
 * @return Timeout value (in nanoseconds) for response messages from the module.
 *         A value of 0 indicates no reply timeout
 */
const int64_t& ComMssgTableEntry::GetReplyTimeout() const
{
    return( m_replyTimeout);
}

/**
 * Adds a new response filter string
 *
 * @param filterString
 *                   New response filter string to add to the response filter string list
 * @param lenType    Flag used to indicate whether the expected response will be a ficed length
 *                   response (i.e. we know ALL the bytes to be returned by the module) or variable
 *                   length( i.e. the module will stream back bytes using a framing character to indicate
 *                   the end of the data stream). Default = FIXED_LEN
 * @param isPositive Flag used to indicate if the given response filter represents a
 *                   positive response (true) or a negative response( false). Default
 *                   value = false
 */
void ComMssgTableEntry::AddResponseFilter( const char *filterString,
                                           RespLenType_t lenType /*=FIXED_LEN*/,
                                           bool isPositive /*=true*/)
{
    LogPortFilterString newFilterString( isPositive, lenType);

    newFilterString.ReadFilterString( filterString);
    m_rcvFilters.AddFilterString( newFilterString);

    // If any of our response strings are variable length
    if( lenType == VAR_LEN)
    {
        // Wee need to use the variable lenght reply alogrithm
        //  i.e. Read  responses byt-by-byte until
        SetRespLenType( lenType);
    }
}

/**
 * Adds a new response filter string
 *
 * @param filterString
 *                   New response filter string to add to the response filter string list
 * @param lenType    Flag used to indicate whether the expected response will be a ficed length
 *                   response (i.e. we know ALL the bytes to be returned by the module) or variable
 *                   length( i.e. the module will stream back bytes using a framing character to indicate
 *                   the end of the data stream). Default = FIXED_LEN
 * @param isPositive Flag used to indicate if the given response filter represents a
 *                   positive response (true) or a negative response( false). Default
 *                   value = false
 */
void ComMssgTableEntry::AddResponseFilter( const string &filterString,
                                           RespLenType_t lenType /*=FIXED_LEN*/,
                                           bool isPositive /*=true*/)
{
    LogPortFilterString newFilterString( isPositive, lenType);

    newFilterString.ReadFilterString( filterString.c_str());
    m_rcvFilters.AddFilterString( newFilterString);
}

/**
 * Sets a new receive protocol
 *
 * @param rcvProtocol New receive protocol value
 */
void ComMssgTableEntry::SetReceiveProtocol( SerialProtocol &rcvProtocol)
{
    m_rcvProtocol = rcvProtocol;
    m_rcvProtocolValid = true;
}

/**
 * Checks if a response from the vehicle is valid. Validity is based
 * upon comparisons of the response string against the possible
 * filter strings
 *
 * @param response Response string to be checked for validity
 * @return true if the response string matches any of the filter strings
 */
bool ComMssgTableEntry::IsValidResponse( const SerialString_t &response) const
{
    return( m_rcvFilters.IsStringValid( response));
}

/**
 * Checks to see if the given string represents a positive reply or a
 * negative reply for the respons filters we contain. If the string does
 * not match any of the receive filters for the given message tag, it
 * is considered to be a negative response.
 *
 * @param response The resopnse string we received
 *
 * @return true if the string represents a positive reply, false otherwise
 */
bool ComMssgTableEntry::IsPositiveResponse( const SerialString_t &response) const
{
    return( m_rcvFilters.IsPositiveResponse( response));
}

/**
 * Retrieves the list of filter strings for this message table entry
 *
 * @param filters Buffer into whic hthe filter strings should be placed
 */
void ComMssgTableEntry::GetFilterStringList( LogPortFilterStringList &filters) const
{
    LogPortFilterStringListCItr_t   itr;

    for( itr=m_rcvFilters.begin(); itr!=m_rcvFilters.end(); itr++)
    {
        filters.AddFilterString( *itr);
    }
}

/**
 * Returns the transmit protocol setting for this object
 *
 * @return The transmit protocol setting for this object
 */
const SerialProtocol &ComMssgTableEntry::GetXmtProtocol() const
{
    return( m_xmtProtocol);
}

/**
 * Returns the transmit protocol setting for this object
 *
 * @param xmtProtocol
 *               SerialProtocol object to be filled in with the transmit
 *               protocol setting for this object
 */
void ComMssgTableEntry::GetXmtProtocol( SerialProtocol &xmtProtocol) const
{
    xmtProtocol = m_xmtProtocol;
}

/**
 * Returns the transmit protocol setting for this object
 *
 * @param xmtProtocol
 *               SerialProtocol_t object to be filled in with the transmit
 *               protocol setting for this object
 */
void ComMssgTableEntry::GetXmtProtocol( SerialProtocol_t &xmtProtocol) const
{
    m_xmtProtocol.GetProtocol( xmtProtocol);
}

/**
 * Returns the transmit protocol setting for this object
 *
 * @return The transmit protocol setting for this object
 */
const SerialProtocol &ComMssgTableEntry::GetRcvProtocol() const
{
    return( m_rcvProtocol);
}

/**
 * Returns the receive protocol setting for this object
 *
 * @param rcvProtocol
 *               SerialProtocol object to be filled in with the receive
 *               protocol setting for this object
 */
void ComMssgTableEntry::GetRcvProtocol( SerialProtocol &rcvProtocol) const
{
    rcvProtocol = m_rcvProtocol;
}

/**
 * Returns the receive protocol setting for this object
 *
 * @param rcvProtocol
 *               SerialProtocol_t object to be filled in with the receive
 *               protocol setting for this object
 */
void ComMssgTableEntry::GetRcvProtocol( SerialProtocol_t &rcvProtocol) const
{
    m_rcvProtocol.GetProtocol( rcvProtocol);
}

/**
 * Sets the "end of mssg" read timeout gap. Used to determine if a variable length
 * reply message has been fully received
 *
 * @param endMssgGap The "end of mssg" read timeout gap in nanoseconds. Used to determine if a variable
 *                   length reply message has been fully received
 */
void ComMssgTableEntry::SetEndMssgGap( const int64_t &endMssgGap)
{
    m_endMssgGap = endMssgGap;
}

/**
 * Returns the "end of mssg" read timeout gap. Used to determine if a variable length
 * reply message has been fully received
 *
 * @return The "end of mssg" read timeout gap. Used to determine if a variable length reply
 *         message has been fully received
 */
const int64_t& ComMssgTableEntry::GetEndMssgGap() const
{
    return( m_endMssgGap);
}

/**
 * Sets the response message length type (fixed or variable)
 *
 * @param respLenType
 *               The response message length type (fixed or variable)
 */
void ComMssgTableEntry::SetRespLenType( const RespLenType_t &respLenType)
{
    m_respLenType = respLenType;
}

/**
 * Returns the response message length type (fixed or variable)
 *
 * @return The response message length type (fixed or variable)
 */
const RespLenType_t ComMssgTableEntry::GetRespLenType() const
{
    return( m_respLenType);
}

/**
 * Assignment operator
 *
 * @param copy   Object to be copied
 * @return *this
 */
ComMssgTableEntry& ComMssgTableEntry::operator=( const ComMssgTableEntry &copy)
{
    m_retryCount = copy.m_retryCount;
    m_xmtString = copy.m_xmtString;
//      m_rcvFilters = copy.m_rcvFilters;
    m_rcvFilters.assign( copy.m_rcvFilters.begin(), copy.m_rcvFilters.end());

    m_retryCount = copy.m_retryCount;
    m_xmtProtocol = copy.m_xmtProtocol;
    m_xmtProtocolValid = copy.m_xmtProtocolValid;
    m_rcvProtocol = copy.m_rcvProtocol;
    m_rcvProtocolValid = copy.m_rcvProtocolValid;
    m_replyTimeout = copy.m_replyTimeout;

    return( *this);
}

/**
 * Returns the number of response strings defined for this object
 *
 * @return The number of response strings defined for this object
 */
int ComMssgTableEntry::GetResponseStringCount() const
{
    return( m_rcvFilters.size());
}

/**
 * Checks if a response from the vehicle is expected
 * for the given message tag
 *
 * @param mssgTag  Message tag to use when looking up valid message response
 *                 strings
 * @return true if response is expected
 */
bool ILogicalPort::IsResponseExpected(const string &mssgTag)
{
    bool        retVal = true;
    try
    {
        // Read 'ReplyMessage' node
        string rcvTag = m_vehicleMessageXmlNode->getChild( mssgTag)->getChild( RCV_MESSAGE_TAG)->getValue();
        // If 'ReplyMessage' is empty, return False (Response NOT expected)
        retVal = !(rcvTag.empty());
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "ILogicalPort::IsResponseExpected() Error Getting XML Node");
    }

    return( retVal);
}

