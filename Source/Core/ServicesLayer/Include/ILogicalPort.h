//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Include/ILogicalPort.h $
// $Author:: Rwiersem                                                         $
//   $Date:: 10/30/07 3:40p                                                   $
//
// Description:
//  Logical serial port driver interface client
//
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/ILogicalPort.h $
// 
// 8     10/30/07 3:40p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 7     2/15/06 7:50p Cward
// Added support for Registering Port Drivers, and Reconnecting Drivers
//
// 6     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 4     4/28/04 10:54p Cward
// Changes associated with build 040402801
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:13p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 31    3/20/03 10:48p Bmeinke
// In CommMssgTableEntry::AddResponseFilter(), the isPositive parameter
// was not being passed to the LogportFilterString contructor
//
// 30    3/20/03 8:45p Bmeinke
// Darn syntax errors
//
// 29    3/20/03 8:38p Bmeinke
// Added IsPositiveResponse() (and numerous supporting methods). Specified
// in the config file, used to chekc if a received response is tagged as
// positive or negative
//
// 28    3/18/03 6:51p Bmeinke
// Added support for performing a 5-baud initialization
// Fixed comment for PortSetProtocol()
// Added PortGetProtocol()
// Cleaned up the protocol helper functions (i.e. PortSetByteGap())
// Added LowSpeedInit() (implementaion of the 5-baud initialization)
//
// 27    3/06/03 1:18a Bmeinke
// Made all parameters which specify a timeout period signed (int64_t from
// uint64_t)
//
// 26    2/06/03 3:57p Bmeinke
// Changed portProtocol argument of PortSetRxProtocol() to non-const in
// order to set the MSB in the c_iflag member
//
// 25    2/06/03 10:16a Bmeinke
// PortSetProtocol() now only takes a SerialProtocol_t argument
// ReadPort() now takes two timeout arguments
// Added EstablishComms() method to perform ISO fast initialization
//
// 24    1/21/03 4:38p Bmeinke
// In ComMssgTableEntry::AddResponseFilter, if the lenType argument is
// VAR_LEN, set the response length type of the ComMssgTableEntry to
// variable length
//
// 23    1/16/03 3:19p Bmeinke
// Changed the duration argument to AssertBreak() from nanoseconds to
// milliseconds
//
// 22    1/16/03 2:56p Bmeinke
// Added method GetResponseStringCount() to ComMssgTableEntry
// Changed the name of DropTransmit() to AssertBreak()
//
// 21    1/15/03 10:10a Bmeinke
// Changed the way that variable length reply strings are handled (moved
// implementation lower)
//
// 20    1/10/03 5:55p Bmeinke
// Added support for variable length reply strings and "end of message"
// timeout gap
// Added DisableTransmit() to disable the serial port transmitter for fast
// ISO initialization sequences
//
// 19    12/13/02 8:59p Bmeinke
// Added GetPortFd() method to return the underlying file descriptor used
// to cimmunicate with the logical port driver
//
// 18    11/20/02 2:27a Bmeinke
// Added copy constructor and assignment operator for ComMssgTableEntry
// class
//
// 17    11/18/02 1:19p Bmeinke
// Add reply timeout support to CommMssgTableEntry class
// Added SetReplyTimeout() method to set per message reply timeouts
//
// 16    10/24/02 7:54p Bmeinke
// Added ResetServer() method to command a logical port driver to perform
// a reset operation.
//
// 15    9/16/02 2:47p Derickso
// Made GetBinaryMssg() method public instead of protected.
//
// 14    6/19/02 5:02p Bmeinke
// typo in javadoc comment for ComMessageMap_t typedef
//
// 13    6/10/02 12:54p Bmeinke
// Added a public SetDebug(bool) method
//
// 12    5/29/02 5:49p Bmeinke
// Added ability to open seperate transmit and receive ports
//
// 11    5/14/02 4:02p Bmeinke
// Changed ReadPort()/WritePort() methods to use SerialString_t and
// uint8_t* data types for sending raw messages
// Added ReadPort()/WritePort() methods that take std::string message tag
// arguments to send/receive data based on message tags read from a
// configuration file
//
// 10    4/24/02 5:24a Bmeinke
// Added support for resetting client FIFOs via the ResetConnection()
// method
//
// 9     4/15/02 1:54p Bmeinke
// Added ability to specify reply filter strings to PortSubscribe()
//
// 8     4/02/02 4:37p Bmeinke
// Fixed return type in comment for WritePort(): >=0 return value means
// success (not just 0)
//
// 7     4/01/02 2:26p Bmeinke
// Fixed some typos
//
// 6     4/01/02 1:39p Bmeinke
// Added overloaded ReadPort and WritePort which take std::string
// arguments
//
// 5     3/22/02 1:22p Bmeinke
// Adapted from ILogPort (simply changed the name and include file name)
//
// 4     2/01/02 9:32p Brian.meinke
// Added m_portSubscrMask to control when a client receives notification
// events
// Added a byteCount attribute to subscription requests (don't notify me
// until xx bytes have been received)
// Changed PortSubscribe() for the newly added (above) subscription
// attributes
// Added PortSetNotifyMask() to dynamically modify a client's subscription
// mask flag
//
// 3     12/05/01 11:57a Brian.meinke
// Added code to retry commands if they fail due to an invalid open file
// descriptor
//
// 2     11/30/01 2:52p Brian.meinke
// First working version
//
// 1     11/29/01 3:55p Brian.meinke
// Logical port driver interface client
//
//******************************************************************************

#ifndef _ILOGICAL_PORT_H_INCLUDED
#define _ILOGICAL_PORT_H_INCLUDED

#include <fcntl.h>              // Open flags (O_RDWR, etc)
#include <termios.h>            // POSIX terminal control
#include <sys/dcmd_chr.h>       // devc devctl defines

#include "LogPortMessage.h"     // Logical Prt Driver custom devctl messages
#include "ILogger.h"            // Logger interface
#include "SerialDefs.h"         // Serial comm definitions
#include "SerialProtocol.h"
#include "LogPortFilterString.h"
#include "BepXmlUtils.h"
#include "XmlNode.h"

#include <string>

/*
    Define the various XML node tags used for configuring an ILogicalPort
    object from an XML config file
*/
/////////////////////////////////////////////////////////////////////////

/**
 * XML node tag for the node whose children contain the vehicle
 * message tag mappings and description
 */
#define VEH_MESSAGES_TAG        XML_T("VehicleMessages")

/**
 * XML node tag of the node which contains, as children, the byte streams
 * to be transmitted for the various message tag
 */
#define XMT_MESSAGES_TAG        XML_T("TransmitMessages")
/**
 * XML node tag of the node which contains, as children, the serial
 * protocol settings necessary to send the data associated with the
 * various serial messages
 */
#define XMT_PROTOCOLS_TAG       XML_T("TransmitProtocols")
/**
 * XML node tag of the node which contains, as children, the characters
 * to use for filling in wildcard characters in the serial data strings
 * read from the <TransmitMessages> section for the various serial
 * messages
 */
#define XMT_INTERPS_TAG         XML_T("TransmitInterpretations")


/**
 * XML node tag for the node containing the tag of the node in the
 * <TransmitMessages> section which contains the actual byte stream
 * to be sent for a given message tag
 */
#define XMT_MESSAGE_TAG         XML_T("TransmitMessage")
/**
 * XML node tag for the node containing the tag of the node in the
 * <TransmitProtocols> section which contains the necessary port protocol
 * settings for a given serial message to be sent out a port
 */
#define XMT_PROTOCOL_TAG        XML_T("TransmitProtocol")
/**
 * XML node tag for the node containing the tag of the node in the
 * <TransmitInterpretations> section which contains the characters
 * to use when filling in wildcards characters in the byte stream
 * read from the <TransmitMessages> section of the config file for a
 * given message tag
 */
#define XMT_INTERP_TAG          XML_T("TransmitInterpretation")
/**
 * XML node tag of the node which contains the number of retry attempts
 * should be made to send a given serial message
 */
#define XMT_RETRY_TAG           XML_T("RetryCount")

/**
 * XML node tag of the node which contains the number of failure response retry attempts
 * should be made to send a given serial message
 */
#define XMT_RESPONSE_FAILURE_RETRY_TAG           XML_T("ResponseFailureRetryCount")

/**
 * XML node tag of the node which contains the flag which indicates a message does not
 * send a message to illicit a response
 */
#define XMT_IS_BUS_BROADCAST_MESSAGE           XML_T("IsBusBroadcastMessage")

/**
 * XML node tag of the node which contains the flag which indicates a message does not
 * send a message to illicit a response
 */
#define XMT_IS_PGN_REQUEST           XML_T("IsPGNRequest")

/**
 * XML node tag of the node which contains the number of failure response retry attempts
 * should be made to send a given serial message
 */
#define XMT_RESPONSE_PENDING_READS_TAG           XML_T("ResponsePendingReads")

/**
 * XML node tag of the node which contains, as children, the byte streams
 * to be received for the various message tag
 */
#define RCV_MESSAGES_TAG        XML_T("ReplyMessages")
/**
 * XML node tag of the node which contains, as children, the serial
 * protocol settings necessary to receive the data associated with the
 * various serial messages
 */
#define RCV_PROTOCOLS_TAG       XML_T("ReplyProtocols")
/**
 * XML node tag of the node which contains, as children, the characters
 * to use for filling in wildcard characters in the serial data strings
 * read from the <ReplyMessages> section for the various serial
 * messages
 */
#define RCV_INTERPS_TAG         XML_T("ReplyInterpretations")

/**
 * XML node tag for the node containing the tag of the node in the
 * <ReplyMessages> section which contains the actual byte stream
 * to be received for a given message tag
 */
#define RCV_MESSAGE_TAG         XML_T("ReplyMessage")
/**
 * XML node tag for the node containing the tag of the node in the
 * <ReplyMessages> section which contains the necessary port protocol
 * settings for a given serial message to be received on a port
 */
#define RCV_PROTOCOL_TAG        XML_T("ReplyProtocol")
/**
 * XML node tag for the node containing the tag of the node in the
 * <ReplyInterpretations> section which contains the characters
 * to use when filling in wildcards characters in the byte stream
 * read from the <ReplyMessages> section of the config file for a
 * given message tag
 */
#define RCV_INTERP_TAG          XML_T("ReplyInterpretation")

/**
 * Object used to hold the transmit/receive message data pairs for a
 * given message tag
 *
 * @author Brian Meinke
 * @version 1
 * @since March 13, 2002
 */
class ComMssgTableEntry
{
public:
    /**
     * Default constructor
     */
    ComMssgTableEntry();

    /**
     * Copy constructor
     *
     * @param copy   Objec tto be copied into *this
     */
    ComMssgTableEntry( const ComMssgTableEntry &copy);

    /**
     * Destructor
     */
    ~ComMssgTableEntry();

    /**
     * Check if a valid transmit protocol is specified for this message
     *
     * @return true if a vaild transmit protocol is specified for this message,
     *         false otherwise
     */
    bool IsXmtProtocolValid();

    /**
     * Check if a valid receive protocol is specified for this message
     *
     * @return true if a vaild receive protocol is specified for this message,
     *         false otherwise
     */
    bool IsRcvProtocolValid();

    /**
     * Sets the number of times the message should be re-transmitted
     * if communication problems are encountered
     *
     * @param retryCount The number of times the message should be re-transmitted
     *                   if communication problems are encountered
     */
    void SetRetryCount( int retryCount);

    /**
     * Returns the number of times the message should be re-transmitted
     * if communication problems are encountered
     *
     * @return The number of times the message should be re-transmitted
     *         if communication problems are encountered
     */
    int GetRetryCount() const;

    /**
     * Sets the number of times the message should be re-transmitted
     * if an unexpected response is received
     *
     * @param retryCount The number of times the message should be re-transmitted
     *                   if communication problems are encountered
     */
    void SetResponseFailureRetryCount( int retryCount);

    /**
     * Returns the number of times the message should be re-transmitted
     * if an unexpected response is received
     *
     * @return The number of times the message should be re-transmitted
     *         if communication problems are encountered
     */
    int GetResponseFailureRetryCount() const;

    /**
     * Sets the flag indicating message does not need to be sent in order to illicit
     * a response
     *
     * @param isBusBroadcastMessage     Sets flag indicating message does not need to be sent in order to illicit
     *                                  a response
     */
    void SetBoolIsBusBroadcastMessage( bool isBusBroadcastMessage);

    /**
     * Returns the flag indicating message does not need to be sent in order to illicit
     * a response
     *
     * @return  flag indicating message does not need to be sent in order to illicit
     *          a response
     */
    bool GetBoolIsBusBroadcastMessage() const;

    /**
     * Sets the flag indicating message ID will be specified
     * explicitly
     *
     * @param isPGNRequest     			Sets the flag indicating
     *  								message ID will be specified
     *  								explicitly
     */
    void SetBoolIsPGNRequest( bool isPGNRequest);

    /**
     * Returns the flag indicating message ID will be specified
     * explicitly in transmit
     *
     * @return     						flag indicating
     *  								message ID will be specified
     *  								explicitly
     */
    bool GetBoolIsPGNRequest() const;

    /**
     * Sets the number of times the message should be re-transmitted
     * if response pending message is received
     *
     * @param retryCount The number of times the message should be re-transmitted
     *                   if communication problems are encountered
     */
    void SetResponsePendingReads( int retryCount);

    /**
     * Returns the number of times the message should be re-transmitted
     * if response pending message is received
     *
     * @return The number of times the message should be re-transmitted
     *         if communication problems are encountered
     */
    int GetResponsePendingReads() const;

    /**
     * Returns the ascii string of comma seperated transmit message bytes
     *
     * @return The ascii string of comma seperated transmit message bytes
     */
    const string &GetXmtString() const;

    /**
     * Sets a new transmit string value
     *
     * @param xmtString New Transmit string value
     */
    void SetTransmitString( const char *xmtString);

    /**
     * Sets a new transmit string value
     *
     * @param xmtString New Transmit string value
     */
    void SetTransmitString( const string &xmtString);

    /**
     * Sets a new transmit protocol
     *
     * @param xmtProtocol New Transmit protocol value
     */
    void SetTransmitProtocol( SerialProtocol &xmtProtocol);

    /**
     * Sets the timeout value (in nanoseconds) for response messages from
     * the module.
     *
     * @param nsTimeout Timeout value (in nanoseconds) for response messages from
     *                  the module
     */
    void SetReplyTimeout( const int64_t& nsTimeout);

    /**
     * Returns the timeout value (in nanoseconds) for response messages from
     * the module.
     *
     * @return Timeout value (in nanoseconds) for response messages from the module.
     *         A value of 0 indicates no reply timeout
     */
    const int64_t& GetReplyTimeout() const;

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
    void AddResponseFilter( const char *filterString, RespLenType_t lenType=FIXED_LEN, bool isPositive=true);

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
    void AddResponseFilter( const string &filterString, RespLenType_t lenType=FIXED_LEN, bool isPositive=true);

    /**
     * Sets a new receive protocol
     *
     * @param rcvProtocol New receive protocol value
     */
    void SetReceiveProtocol( SerialProtocol &rcvProtocol);

    /**
     * Checks if a response from the vehicle is valid. Validity is based
     * upon comparisons of the response string against the possible
     * filter strings
     *
     * @param response Response string to be checked for validity
     * @return true if the response string matches any of the filter strings
     */
    bool IsValidResponse( const SerialString_t &response) const;

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
    bool IsPositiveResponse( const SerialString_t &response) const;

    /**
     * Retrieves the list of filter strings for this message table entry
     *
     * @param filters Buffer into whic hthe filter strings should be placed
     */
    void GetFilterStringList( LogPortFilterStringList &filters) const;

    /**
     * Returns the transmit protocol setting for this object
     *
     * @return The transmit protocol setting for this object
     */
    const SerialProtocol &GetXmtProtocol() const;

    /**
     * Returns the transmit protocol setting for this object
     *
     * @param xmtProtocol
     *               SerialProtocol object to be filled in with the transmit
     *               protocol setting for this object
     */
    void GetXmtProtocol( SerialProtocol &xmtProtocol) const;

    /**
     * Returns the transmit protocol setting for this object
     *
     * @param xmtProtocol
     *               SerialProtocol_t object to be filled in with the transmit
     *               protocol setting for this object
     */
    void GetXmtProtocol( SerialProtocol_t &xmtProtocol) const;

    /**
     * Returns the transmit protocol setting for this object
     *
     * @return The transmit protocol setting for this object
     */
    const SerialProtocol &GetRcvProtocol() const;

    /**
     * Returns the receive protocol setting for this object
     *
     * @param rcvProtocol
     *               SerialProtocol object to be filled in with the receive
     *               protocol setting for this object
     */
    void GetRcvProtocol( SerialProtocol &rcvProtocol) const;

    /**
     * Returns the receive protocol setting for this object
     *
     * @param rcvProtocol
     *               SerialProtocol_t object to be filled in with the receive
     *               protocol setting for this object
     */
    void GetRcvProtocol( SerialProtocol_t &rcvProtocol) const;

    /**
     * Sets the "end of mssg" read timeout gap. Used to determine if a variable length
     * reply message has been fully received
     *
     * @param endMssgGap The "end of mssg" read timeout gap in nanoseconds. Used to determine if a variable
     *                   length reply message has been fully received
     */
    void SetEndMssgGap( const int64_t &endMssgGap);

    /**
     * Returns the "end of mssg" read timeout gap. Used to determine if a variable length
     * reply message has been fully received
     *
     * @return The "end of mssg" read timeout gap. Used to determine if a variable length reply
     *         message has been fully received
     */
    const int64_t& GetEndMssgGap() const;

    /**
     * Sets the response message length type (fixed or variable)
     *
     * @param respLenType
     *               The response message length type (fixed or variable)
     */
    void SetRespLenType( const RespLenType_t &respLenType);

    /**
     * Returns the response message length type (fixed or variable)
     *
     * @return The response message length type (fixed or variable)
     */
    const RespLenType_t GetRespLenType() const;

    /**
     * Assignment operator
     *
     * @param copy   Object to be copied
     * @return *this
     */
    ComMssgTableEntry& operator=( const ComMssgTableEntry &copy);

    /**
     * Returns the number of response strings defined for this object
     *
     * @return The number of response strings defined for this object
     */
    int GetResponseStringCount() const;

private:

    /**
     * Message to be sent to the vehicle
     */
    string                  m_xmtString;

    /**
     * List of receive filter strings
     */
    LogPortFilterStringList m_rcvFilters;

    /**
     * Number of time to retransmit the message
     */
    int                     m_retryCount;

    /**
     * Number of time to retransmit the message
     */
    INT32                  m_responseFailureRetryCount;

    /**
     * Flag indicating message does not need to be transmitted
     * in order to illicit response
     */
    bool                  m_isBusBroadcastMessage;

    /**
     * Flag indicating module id will be explicity provided in 
     * transmit message 
     */
    bool                  m_isPGNRequest;

    /**
     * Number of time to retransmit the message
     */
    INT32                   m_responsePendingReads;

    /**
     * Special port protocol required before message can be transmitted
     */
    SerialProtocol          m_xmtProtocol;

    /**
     * Flag to indicate if the transmit protocol has been initialized
     */
    bool                    m_xmtProtocolValid;

    /**
     * Special port protocol needed for message to be properly received
     */
    SerialProtocol          m_rcvProtocol;

    /**
     * Flag to indicate if the receive protocol has been initialized
     */
    bool                    m_rcvProtocolValid;

    /**
     * Timeout value (in nanoseconds) for response messages from the module
     */
    int64_t                 m_replyTimeout;

    /**
     * This is the amount of time to wait (in nanoseconds) between consecutive read
     * operations before considering a message as fully reseived for variable length
     * responses.
     */
    int64_t                 m_endMssgGap;

    /**
     * What type of response string are we waiting for?
     * Fixed length? => Wait for fixed number of characters to arrive
     * Variable Length? => After characters start to arrive wait xx millliseconds
     *  between consecutive read operations before considering the message as complete
     */
    RespLenType_t           m_respLenType;
};

/**
 * Object used to hold the mappings of message tag to message string
 */
typedef std::map< string, ComMssgTableEntry>    ComMessageMap_t;
typedef ComMessageMap_t::iterator               ComMessageMapItr_t;
typedef ComMessageMap_t::const_iterator         ComMessageMapCItr_t;
typedef ComMessageMap_t::value_type             ComMessageMapVal_t;

/**
 * Logical port driver interface class
 *
 * @author Brian Meinke
 * @version 1
 * @see LogPortDrv
 * @since Nov 29, 2001
 */
class ILogicalPort : public ILogger
{
public:

    /**
     * Default constructor
     */
    ILogicalPort();

    /**
     * Destructor
     */
    ~ILogicalPort();

    /**
     * Object initialization. Used to load the mapping of message tags
     * to  transmit/receive message string pairs for serial communication
     *
     * @param xmlFileName
     *               Name of the XML file to read object configuration data from
     * @return true if the message map table is successfully loaded, false
     *         if an error occurred
     */
    bool Initialize( const string &xmlFileName);

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
    bool Initialize( const XmlNode* portConfig);

    /**
     * Method used to read the low speed module initialization message from the 
     * config file "InitMessage" node.
     *
     * @param initMssgNode Pointer to the "InitMessage" node to read the low 
     *                     speed module initialization message byte from.
     */
    void ReadInitMessage( const XmlNode *initMssgNode);

    /**
     * Opens a logical port for communication
     *
     * @param portName Name of the logical port to be opened
     * @param mode     Mode in which to open the port: O_RDONLY, O_WRONLY or
     *                 O_RDWR. The default is O_RDWR.
     * @return true if successfully opened, false on error
     */
    bool OpenPort( const char *portName, int mode=O_RDWR);

    /**
     * Opens a logical port for communication. This form of the OpenPort
     * method is used to transmit data on one physical com port and receive
     * data on a different physical com port
     *
     * @param inPortName Name of the logical port to be opened for receiving data
     * @param outPortName
     *                   Name of the logical port to be opened for transmitting data
     * @return true if successfully opened, false on error
     */
    bool OpenPort( const char *inPortName, const char *outPortName);

    /**
     * Opens a logical port for communication
     *
     * @param portName Name of the logical port to be opened
     * @param mode     Mode in which to open the port: O_RDONLY, O_WRONLY or
     *                 O_RDWR. The default is O_RDWR.
     * @return true if successfully opened, false on error
     */
    bool OpenPort( const string &portName, int mode=O_RDWR);

    /**
     * Closes the logical port
     */
    void ClosePort();

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
    bool LockPort( int flags=0);

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
    int LockPortDebug( int flags=0);

    /**
     * Request to unlock a port. A successful call to LockPort() must have
     * been issued for this method to succeed.
     *
     * @return true if port was unlocked, false if an error occurred (use errno
     *         for more detailed error information)
     */
    bool UnlockPort();
    /**
     * Request to unlock a port. A successful call to LockPort() must have
     * been issued for this method to succeed.
     *
     * @return true if port was unlocked, false if an error occurred (use errno
     *         for more detailed error information)
     */
    int UnlockPortDebug();

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
    bool PortSubscribe(const NotifyEvent_t &event, int flags, int modeMask,
                       int byteCount=1, const LogPortFilterString *fltrString=NULL);

    bool FilterSubscribe(const NotifyEvent_t &event, const string &rcvTag);

    /**
     * Request to be "unregistered" for port events. The flags
     * parameter specifies which port events you want to be
     * unregistered for.
     *
     * @param flags  Bitwise flag indicating which port events you want notifications
     *               for. The current port events that are supported are:
     *               PORT_SUBSCRIBE_RX, PORT_SUBSCRIBE_TX, PORT_SUBSCRIBE_ALL
     *               PORT_SUBSCRIBE_LOCK, PORT_SUBSCRIBE_UNLOCK and PORT_NOTIFY_EXISTING
     *
     * @return true if successfully "unregistered", false if an error
     *         occurred (use errno for more detailed error information)
     */
    bool PortUnsubscribe( int flags=PORT_SUBSCRIBE_ALL);

    /**
     * Sets or changes the notification mask for port events.
     *
     * @param newMask New bit-wise filter mask for when to be notified of events we're
     *                subscribed for
     * @return true if successful, false if an error occurred (use errno for
     *         more detailed error information)
     */
    bool PortSetNotifyMask( int newMask);

    /**
     * Request to change port prtotcol parameters.
     *
     * @param portProtocol
     *               New port protocol to apply to the port
     *
     * @return true if successful, false if an error occurred (use errno for
     *         more detailed error information)
     */
    bool PortSetProtocol( const SerialProtocol_t &portProtocol);

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
    bool PortGetProtocol( SerialProtocol_t &portProtocol);

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
    bool PortSetRxProtocol( SerialProtocol_t &portProtocol,
                            SerialProtocol_t *oldProtocol,
                            int flags=PORT_SET_ALL);
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
    bool PortSetTxProtocol( const SerialProtocol_t &portProtocol,
                            SerialProtocol_t *oldProtocol,
                            int flags=PORT_SET_ALL);

    /**
     * Sets a new baud rate for the port
     *
     * @param newBaud New baud rate to apply to the port
     * @param oldBaud Buffer to receive the current baud rate for the port (can
     *                be NULL)
     * @return true if successful, false if an error occurred (use errno for
     *         more detailed error information)
     */
    bool PortSetBaud( speed_t newBaud, speed_t *oldBaud=NULL);

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
    bool PortSetParity( Parity_t newParity, Parity_t *oldParity=NULL);

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
    bool PortSetDataBits( DataBits_t newDataBits, DataBits_t *oldDataBits=NULL);

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
    bool PortSetStopBits( StopBits_t newStopBits, StopBits_t *oldStopBits=NULL);

    /**
     * Sets a new inter-byte gap for the port
     *
     * @param newByteGap New inter-byte gap (in ms) to apply to the port
     * @param oldByteGap Buffer to receive the current inter-byte gap for the
     *                   port (can be NULL)
     * @return true if successful, false if an error occurred (use errno for
     *         more detailed error information)
     */
    bool PortSetByteGap( uint32_t newByteGap, uint32_t *oldByteGap=NULL);

    /**
     * Sets a new inter-message gap for the port
     *
     * @param newMssgGap New inter-message gap (in ms) to apply to the port
     * @param oldMssgGap Buffer to receive the current inter-message gap for the
     *                   port (can be NULL)
     * @return true if successful, false if an error occurred (use errno for
     *         more detailed error information)
     */
    bool PortSetMssgGap( uint32_t newMssgGap, uint32_t *oldMssgGap=NULL);

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
    int ReadPort( uint8_t *buff, size_t len, long initWait=-2, long endWait=0);

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
    int ReadPort( SerialString_t &buff, long initWait=-2, long endWait=0);

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
    int ReadPort( const string &mssgTag, SerialString_t &respString);

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
    int WritePort( const uint8_t *buff, size_t len, int retryCount=3);

    /**
     * Writes data to the port. The NULL terminator is NOT written
     *
     * @param buff       Buffer of data to write
     * @param retryCount Number of times to re-send the data if the previous send fails
     * @return Number of bytes written if successful,
     *         -1 if an error occurred (use errno for more
     *         detailed error information)
     */
    int WritePort( const SerialString_t &buff, int retryCount=3);

    /**
     * Send the transmit message for the serial message with the given
     * message tag (as read form the config file)
     *
     * @param mssgTag Tag of the message to send
     * @return Number of bytes written if successful,
     *         -1 if an error occurred (use errno for more detailed error
     *         information)
     */
    int WritePort( const string &mssgTag);

    /**
     * Applies a temporary (current message only) reply timeout in nanoseconds.
     * This timeout will effect the curent message transaction only.
     *
     * @param nsTimeout Number of nanosecnds to wait for a reply before a timeout should be
     *                  raised.
     * @return 0 if successful, -1 if error
     */
    int SetReplyTimeout( const int64_t &nsTimeout);

    /**
     * Checks if a response string from the vehicle is a valid
     * response for the given message tag
     *
     * @param mssgTag  Message tag to use when looking up valid message response
     *                 strings
     * @param response Response string from the vehicle
     * @return true if the given response string is a valid response
     */
    bool IsResponseValid( const string &mssgTag, const SerialString_t &response) const;

    /**
     * Checks if a response from the vehicle is expected
     * for the given message tag
     *
     * @param mssgTag  Message tag to use when looking up valid message response
     *                 strings
     * @return true if response is expected
     */
    bool IsResponseExpected( const string &mssgTag);

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
    bool IsPositiveResponse( const string &mssgTag, const SerialString_t &response) const;

    /**
     * Reset our connection to the logical port (clears FIFOs)
     *
     * @return EOK if successful, any other value indicates an error code
     */
    int ResetConnection();

    /**
     * Resets the port driver (re-initialize)
     *
     * @return EOK if successful, any other value indicates an error code
     */
    int ResetServer();

    /**
     * Asserts a break condition on the port for the specified number of nano-seconds.
     *
     * @param duration Number of milli-seconds to assert break for
     * @return EOK if successful, any other value indicates an error code
     */
    int AssertBreak( int duration);

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
    int EstablishComms( UINT32 lowDuration, UINT32 hiDuration, const uint8_t *initMssg=NULL, const int mssgLen=0);

    /**
     * Performs a 5-baud module initialization
     *
     * @return return EOK if successful, any other value indicates an error code
     */
    int LowSpeedInit();

    /**
     * Requests the comm proxy to re-establish a connection to the
     * underlying communication driver
     *
     * @return return EOK if successful, any other value indicates an error code
     */
    int ReconnectDriver();
    /**
     * Send the register pulse to the port driver so it will register.
     */
    void RegisterPortDriver(void);
    /**
     * Set the status of the debug messages.
     *
     * @param value  The new debug status. On or Off
     * @since Version 1.0
     */
    void SetDebug(const bool value);

    /**
     * Parses an ascii string of comma seperated fields of serial
     * message bytes (i.e. "0x12,0x34,0x56") into a binary sequence
     * of unsigned character bytes ( i.e. [0x12 0x34 0x56] )
     *
     * @param asciiMsg String of comma seperated serial bytes
     * @param binMsg   Binary array of bytes representing a serial string
     * @return true if the message was properly parsed
     */
    bool GetBinaryMssg( const string &asciiMsg, SerialString_t &binMsg);

    /**
     * Returns the file descriptor used to communicate cia the logical port
     *
     * @return The file descriptor used to communicate cia the logical port
     */
    int GetPortFd() const;
    /**
     * Outputs a serial string via the logger
     *
     * @param prefix    String to pre-pend to output (i.e. "Response" would
     *                  result in "Response: $12...")
     * @param srlString Serial string to be printed
     * @param verboseMask
     *                  Optional logger verbose mask. Defaults to LOG_DETAILED_DATA
     */
    void PrintSerialString( const char *prefix, const SerialString_t &srlString, int verboseMask=LOG_DETAILED_DATA);

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
    void PrintSerialString( const char *prefix, const uint8_t *srlString, uint32_t len, int verboseMask=LOG_DETAILED_DATA);



protected:

    /**
     * Forms a fully qualified path to the logical port driver reponsible
     * for managing traffic for the given logical port name
     *
     * @param portName   Name of the logical port driver to get the path for
     * @param driverPath Buffer to receive the fully qualified path to the port driver
     */
    void GetDriverPath( const char *portName, string &driverPath);

    /**
     * Reads the mapping of message tags to their corresponding serial
     * byte strings
     *
     * @param mssgMapNode
     *               Parent XML node of the nodes containing the message tag names and
     *               serial byte string pairs
     * @return true if the message tag mappings were read successfully, false otherwise
     */
    bool ReadMessageMap( const XmlNode *mssgMapNode);

    /**
     * Reads the number of times a message should be re-sent in case
     * of communication problems
     *
     * @param configNode Parent XML node containing the mapping of message tags
     *                   to serial byte strings
     * @param mssgTag    The message tag to read the serial transmit byte string for
     * @return Number of times to retry
     */
    int ReadRetryCount( const XmlNode *configNode, const string &mssgTag);

    /**
     * Reads the number of times a message should be re-sent if the response is determined 
     * to be incorrect
     *
     * @param configNode Parent XML node containing the mapping of message tags
     *                   to serial byte strings
     * @param mssgTag    The message tag to read the serial transmit byte string for
     * @return Number of times to retry
     */
    int ReadResponseFailureRetryCount( const XmlNode *configNode, const string &mssgTag);

    /**
     * Reads the flag which indicates message does not need to be sent to illicit a response
     *
     * @param configNode Parent XML node containing the mapping of message tags
     *                   to serial byte strings
     * @param mssgTag    The message tag to read the serial transmit byte string for
     * @return bool is bus broadcast message
     */
    bool ReadBoolIsBusBroadcastMessage( const XmlNode *configNode, const string &mssgTag);

    /**
     * Reads the flag which indicates message is to be transmitted
     * as a PGN style message not adding modules fixed identifier
     *
     * @param configNode Parent XML node containing the mapping of message tags
     *                   to serial byte strings
     * @param mssgTag    The message tag to read the serial transmit byte string for
     * @return bool is PGN request
     */
    bool ReadBoolIsPGNRequest( const XmlNode *configNode, const string &mssgTag);

    /**
     * Reads the number of response pending reads
     *
     * @param configNode Parent XML node containing the mapping of message tags
     *                   to serial byte strings
     * @param mssgTag    The message tag to read the serial transmit byte string for
     * @return Number of times to retry
    */
    int ReadResponsePendingReads( const XmlNode *configNode, const string &mssgTag);

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
    bool ReadTransmitString( const XmlNode *configNode,
                             const string &mssgTag,
                             string &stringVal);
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
    bool ResolveTransmitInterpretation( const XmlNode *configNode,
                                        const string &mssgTag,
                                        string &stringVal);

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
    bool ReadTransmitInterpretation( const XmlNode *configNode,
                                     const string &mssgTag,
                                     string &stringVal);

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
    bool ReadXmtProtocol( const XmlNode *configNode,
                          const string &mssgTag,
                          SerialProtocol &xmtProtocol);

    /**
     * Reads the valid serial response string for the given message
     * tag from the XML nodes conatined under the configNode parent
     * XML node
     *
     * @param configNode   Parent XML node containing the mapping of message tags
     *                  to serial byte strings
     * @param mssgTag   The message tag to read the serial receive string for
     * @param mssgEntry The message entry in the communication table.
     * 
     * @return true if the receive byte string was successfully read,
     *         false otherwise
     */
    bool ReadResponseString( const XmlNode *configNode,
                             const string &mssgTag,
                             ComMssgTableEntry &mssgEntry);
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
    bool ReadResponseInterpretation( const XmlNode *configNode,
                                     const string &mssgTag,
                                     string &stringVal);

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
    bool ReadRcvProtocol( const XmlNode *configNode,
                          const string &mssgTag,
                          SerialProtocol &rcvProtocol);

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
    bool ReadRespLenType( const XmlNode *respNode, RespLenType_t &respLenType);

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
    bool ReadRcvEndGapTime( const XmlNode *respNode, int64_t &endMssgGap);

    /**
     * Reads the response type for a message filter. The response type
     * will be either 'positive' or 'negative'.
     *
     * @param respNode Node containing the attributes for a module reply string
     *
     * @return true if the response string represents a positive response or
     *         false if it represents a negative response
     */
    bool ReadResponseType( const XmlNode *respNode);

    /**
     * Fills in wildcard characters in a vehicle message string with
     * byte values passed in the variable argument list
     *
     * @param outString Formatted serial string (comma seperated byte fields)
     * @param fmt       Base serial mesage string with printf-style wildcard
     *                  delimitter fields used to build the final serial string
     */
    void FormatAsciiMssg( string &outString, const char *fmt, ...);

    /**
     * Check if we need to set a new transmit protocol before sending a message
     *
     * @param mssgEntry ComMssgTableENtry object which contains the message to be sent and,
     *                  potentially, a new transmit protocol required to send the message
     * @return
     */
    bool SetXmtProtocol( ComMssgTableEntry &mssgEntry);

    /**
     * Check if we need to set a new receive protocol before sending a message
     *
     * @param mssgEntry ComMssgTableENtry object which contains the message to be sent and,
     *                  potentially, a new transmit protocol required to send the message
     * @return
     */
    bool SetRcvProtocol( ComMssgTableEntry &mssgEntry);

    /**
     * Channel ID of the desired logical port driver
     */
    int                 m_portDrvChid;

    /**
     * File descriptor for our connection to the logical port driver used for
     * sending data
     */
    int                 m_outPortDrvFd;

    /**
     * File descriptor for our connection to the logical port driver used for
     * receiving data
     */
    int                 m_inPortDrvFd;

    /**
     * Subscription mask
     */
    int                 m_portSubscrMode;

    /**
     * Subscription filter mask
     */
    int                 m_portSubscrMask;

    /**
     * Event to use for subscription notifications
     */
    NotifyEvent_t       m_portSubscEvent;

    /**
     * Port protocol settings for receiving data
     */
    SerialProtocol_t    *m_inPortProtocol;

    /**
     * Port protocol settings for receiving data
     */
    SerialProtocol_t    *m_outPortProtocol;

    /**
     * Object used to map a tag to a transmit/response message pair
     */
    ComMessageMap_t     m_mssgTags;

    /**
     * Name of the logical port we are using to receive data
     */
    string              m_inPortName;

    /**
     * Name of the logical port we are using to transmit data
     */
    string              m_outPortName;

    /**
     * Subscription ID
     */
    uint64_t            m_subHandle;

    /**
     * Temporary SerialProtocol_t object used to hold the existing port
     * protocol when sending a message which requires a new transmit or
     * receive protocol
     */
    SerialProtocol_t    m_tempRxPortProtocol;

    /**
     * Temporary SerialProtocol_t object used to hold the existing port
     * protocol when sending a message which requires a new transmit or
     * receive protocol
     */
    SerialProtocol_t    m_tempTxPortProtocol;

    /**
     * Low speed initialization message
     */
    CommInitMessage_t   *m_initMessage;

    /**
     *  Stores the Vehicle Message Xml node.
     */
    const XmlNode *m_vehicleMessageXmlNode;

};

#endif // _ILOGICAL_PORT_H_INCLUDED
