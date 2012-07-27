//******************************************************************************
//
// FILE:    BepCommunicaitons
//     $Archive:: /Ccrt/Sour $
//     $Author: Rwiersem $
//     $Date: 10/30/07 3:37p $
//
// Description:
//  Class to encapsulate Burke Porter Communication protocol details
//
//==============================================================================
//
//     Copyright © 2001 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/IBepCommunication.h $
// 
// 4     10/30/07 3:37p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     10/26/06 9:15a Rwiersem
// Changes for the 06102701 core release:
// 
// Added Activate() and Deactivate().  These are used to keep the test 
// components running when a new drive curve starts.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 3     10/24/03 11:37a Khykin
// Update for timeout issues.
//
// 2     10/10/03 9:20a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 2     8/06/03 3:37p Khykin
// Updated with changes from Flatrock.
//
// 1     6/12/03 4:10p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:37  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:05a Khykin
// Initial Checkin
//
// 1     1/18/03 8:02a Khykin
// Initial Checkin From DCTC
//
// 73    11/21/02 8:36a Khykin
// Updating for managine the timeout and buffer size information.
//
// 72    10/21/02 11:47a Khykin
//
// 71    7/31/02 11:32a Skypig13
// Removed print from Close()
//
// 70    6/27/02 3:49p Skypig13
// Fixed spelling in debug statement and updated SetMessageStatus()
//
// 69    6/25/02 5:16p Skypig13
// Move SetMessageStatus() to protected
//
// 68    6/25/02 5:05p Skypig13
// Added SetMessageStatus()/GetMessageStatus() and updated AddAttribute()
// to be more robust
//
// 67    6/19/02 2:24p Skypig13
// Added new GetNext() method
//
// 66    6/18/02 12:47p Dan.erickson
// Modified HandleRequest().
//  - Added messagePopulated flag to support subscriptions to
// TestResultServer.
//  - Made the method virtual so ITestResultServer could extend.
//
// 65    5/30/02 4:16p Skypig13
// Made GetId() const and added Close()
//
// 64    4/25/02 4:40p Skypig13
// Updated with new changes
//     Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//     started
//
//
// 62    4/02/02 11:05p Skypig13
// Updates for IPublish interface and efficiency
//
// 61    3/07/02 6:48p Kh
// Changing the scope of the method GetRequestType to public.
//
// 60    2/12/02 5:15p Dan.erickson
// Modified to make better use of XmlNode data.
//
// 59    2/05/02 12:35a Skypig13
// Restructured to accompany BepServerChanges
//
// 58    12/18/01 11:32a Kh
// Updating the Register method.
//
// 57    12/14/01 5:44p Kh
// Adding a Register method.
//
// 56    12/14/01 4:58p Admin
//
// 55    12/04/01 1:07p Skypig13
// Added backlog configuration for TCP/IP communicaitons
//
// 54    11/29/01 5:56p Dan.erickson
// Added Read, Write, Publish and Command methods that accept
// a XmlNode as an argument.
//
// 53    11/01/01 9:47p Skypig13
// Moved method bodies from header to source file for some long methods
//
// 52    11/01/01 6:05p Dan.erickson
// Removed const for BepComponent item in SpecialProcessing prototype.
//
// 51    11/01/01 3:31p Dan.erickson
// Added name to Initialize method prototype for
// IInterProcessCommunication.
//
// 50    11/01/01 11:25a Dan.erickson
// Added SpecialProcessing method.
//
// 49    10/29/01 2:08p Dan.erickson
// Removed get and set procedures for description and process attributes.
// these should be in a derived class.
//
// 48    10/22/01 3:43p Dan.erickson
// Added Get and Set methods for m_process and m_description
// member variables.
//
// 47    10/17/01 9:56a Skypig13
// Changed prototype of Read() to include data reate
//
// 46    10/16/01 4:30p Dan.erickson
// Added SetDescription and GetDescription methods to support servers.
// Added m_description member and changed BuildMessage to virtual so it
// can be overloaded.
//
// 45    10/12/01 10:31a Dan.erickson
// Modified Initialize method to allow initialization of Tcp/Ip objects.
//
// 44    10/08/01 5:29p Skypig13
// Fixed Initialize() prototype
//
// 43    10/05/01 4:35p Skypig13
// Added GetDataRate() and updated IsDataOk()
//
// 42    10/04/01 3:01p Skypig13
// Made ValidateMessage() virtual and added AddAttribute() method
//
// 41    9/26/01 9:00a Skypig13
// Added GetCommunicationType() method
//
// 40    9/18/01 9:09a Skypig13
// Added ResetCommunications method.
//
// 39    9/14/01 2:32p Skypig13
// Add Publish method
//
// 38    9/10/01 3:15p Dan.erickson
// Changed prototype to accept a socket id to talk on.
//
// 37    9/04/01 9:05p Skypig13
// Added DanIBepCommunicationTest as friend
//
// 36    8/31/01 10:51a Skypig13
// Fixed bug on line 457
//
// 35    8/31/01 10:48a Skypig13
// Add GetName() method
//
// 34    8/30/01 4:15p Skypig13
// Restored default parameters
//
// 33    8/28/01 8:24p Skypig13
// Change parameters to not take defaults and add timeout to Write()
//
// 32    8/24/01 1:40p Skypig13
// make m_parser protected
//
// 31    8/13/01 12:13p Skypig13
// Fixed comments
//
// 30    7/25/01 2:41p Skypig13
// Added GetByTag() for Kevin
//
// 29    7/25/01 9:47a Kh
// Changed the static local oper in DoOperation to m_oper in the class.
// It behaved like a class static variable (Bad!).
//
// 28    7/20/01 10:03a Skypig13
// Changed prototype for Subscribe() and Unsubscribe() for Kevin
//
// 27    7/20/01 9:25a Skypig13
// Change for consistent handling of pulses
//
// 25    7/19/01 10:24a Skypig13
// Added ReloadConfiguration()
//
// 24    7/17/01 7:01p Skypig13
// Add pulse handling
//
// 23    7/17/01 8:56a Skypig13
//
// 22    7/12/01 3:15p Skypig13
// Made GetNext() virtual
//
// 21    7/11/01 7:26p Skypig13
// Changed m_sendData to XmlNodeMap and tested
//
// 20    7/11/01 5:55p Skypig13
// Removed XmlNodeMap for m_sendData and added discrete parameter
// initialization.
//
// 18    7/06/00 10:09p Skypig13
// Derive from Debuggable
//
// 17    7/06/00 6:03p Skypig13
// Changed prototype for CheckAttribute to be virtual
//
// 16    7/06/00 4:03p Skypig13
// Added Write data test
//
// 15    7/03/00 9:26p Skypig13
// Changed prototype for GetNext() to include out parameter
// for message in case of error response
//
// 14    7/03/00 8:55p Skypig13
// Oops, fixed or/and mix-up in IsDataOk()
//
// 13    7/03/00 8:42p Skypig13
// Change IsDataOk() to check for all error conditions
//
// 12    7/03/00 11:10a Skypig13
// Changed methods GetRequestType() and ValidateMessage() to accept a bool
// parameter to determine whether or not to parse message or use document
// already parsed.
//
// 11    6/27/00 8:23p Skypig13
// All server-side tests complete
//
// 10    6/26/00 2:36p Skypig13
// Testing server-side happy-day complete.
//
// 9     6/22/00 4:31p Skypig13
// Latest testing and working
//
// 8     6/21/00 5:48p Skypig13
// Testing functionality of IPC and OpenChannel tests.
// Made some interface changes
//
// 7     6/19/00 10:19a Skypig13
// Changed prototype of HandlRequest() to take BepComponent* instead
// of a BepComponent& (coding of servers is easier since this can be
// passed in without any derererence).
//
// 6     6/13/01 7:23p Admin
// Change Boolean to bool
//
// 5     6/13/01 7:08p Skypig13
// Added interface for ProcessRequest()
//
// 4     6/12/01 9:57a Skypig13
// Changes to interface for more commonality between other classes
//
// 3     6/08/01 12:54p Skypig13
// Weekly backup
//
//******************************************************************************

#ifndef IBepCommunicaiton_h
#define IBepCommunicaiton_h

#include <string>

#include "ITcpIp.h"
#include "BepDefs.h"
#include "XmlDefs.h"
#include "IPublish.h"
#include "XmlParser.h"
#include "Debuggable.h"
#include "IOpenChannelIpc.h"
#include "ISystemCommunication.h"
#include "IInterProcessCommunication.h"

class BepComponent;             // Forward reference to class to prevent circular includes with BepComponent
class IBepCommunicationTest;    // Forward reference to testing class
class DanIBepCommunicationTest; // Forward reference to another testing class
/**
 * Interface class that packages requests into messages that
 * conform to the Bep messaging protocol.
 *
 * @author Tim Bochenek
 * @author Kevin Hykin
 * @version 2.90
 * @since Version 1.0
 */
class IBepCommunication : public Debuggable, public IPublish
{
public:
    friend class IBepCommunicationTest;     // Testing class
    friend class DanIBepCommunicationTest;  // Another testing class
    /**
     * Class constructor (default)
     * @since Version 1.0
     */

    IBepCommunication(void);
    /**
     * Class destructor
     * @since Version 1.0
     */
    virtual ~IBepCommunication(void);
    /**
     * Initialize method to set up object (IPC only)
     *
     * @param id         Id of channel to connect to
     * @param name       Name of the channel to talk on.
     * @param rid        Reply id
     * @param debug      Diagnostic print variable
     * @param size       Size of receive buffer to create (>= DEFAULT_BUFFER_SIZE)
     * @param timeout    Timeout to wait when sending a message (>= DEFAULT_TIMEOUT)
     * @since Version 2.0
     */
    void Initialize(const INT32 id, const std::string name,
                    const bool debug = false, const INT32 rid = -1,
                    const INT32 size = DEFAULT_BUFFER_SIZE,
                    const INT32 timeout = DEFAULT_TIMEOUT);

    /**
     * Initialize method to set up object (TCP/IP only)
     *
     * @param id         Id of channel to connect to
     * @param port       Port number to communicate on
     * @param address    TCP/IP address to use
     * @param frameStart Frame start delimiter
     * @param frameEnd   Frame end delimiter
     * @param backlog    Number of pending connection requests to allow in backlog
     * @param debug      Diagnostic print variable
     * @since Version 2.0
     */
    void IBepCommunication::Initialize(const INT32 id, const INT16 port = 0,
                                       const std::string address = "localhost",
                                       const std::string frameStart = BEP_XML_ROOT,
                                       const std::string frameEnd = BEP_XML_ROOT_END,
                                       const bool debug = false, const INT32 backlog = 5);
    /**
     * Set up class
     *
     * @param config configuration node
     * @param id     Id of channel to connect to (optional).
     *               If this parameter is passed in (and not -1)
     *               the class will connect to channel instead of
     *               creating it.
     * @param rid    Receive id (used only to create IPC class that connects
     *               to channel with message waiting)
     * @since Verison 1.0
     */
    virtual void Initialize(const XmlNode *config, const INT32 id = -1,
                            const INT32 rid = -1);
    /**
     * Create a worker object that will get requests from
     * a channel already open and with message(s) waiting
     *
     * @param creator Object that opened channel to connect to
     * @param id     The id of open channel to talk on.
     * @since Version 1.1
     */
    void Initialize(IBepCommunication &creator, INT32 id = -1);
    /**
     * Create a worker object that will get requests from
     * a channel already open and with message(s) waiting
     *
     * @param name    Name of server
     * @param type    Type of object (Client/Server)
     * @param debug   Diagnostic print variable
     * @param size    Size of receive buffer to create (>= DEFAULT_BUFFER_SIZE)
     * @param timeout Timeout to wait when sending a message (>= DEFAULT_TIMEOUT)
     * @since Version 2.7
     */
    void Initialize(const std::string &name, const std::string &type,
                    const bool debug = false,
                    const INT32 size = DEFAULT_BUFFER_SIZE,
                    const INT32 timeout = DEFAULT_TIMEOUT);
    /**
     * Clear information from the buffer.
     * 
     * @return Status of operation
     *         Defined return types:
     *         Buffer was cleared                == BEP_STATUS_SUCCESS
     * @since Version 1.0
     */
    virtual INT32 Clear();
    /**
     * Read information in the system based on the tag value
     * passed.
     *
     * @param tag     Data to be read
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of operation
     *         Defined return types:
     *         All tags sent and received Ok                == BEP_STATUS_SUCCESS
     *         Tag was added to list (waiting for last tag) == 1
     *         Read operation failed to read all tags       == -1
     *         Read operation failed                        == -2
     * @since Version 1.0
     */
    virtual INT32 Read(const std::string &tag, std::string &response,
                       const bool IsFinal = false);
    /**
     * Read information in the system based on the tag value
     * passed.
     *
     * @param node   Information to be read.
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of operation
     *         Defined return types:
     *         All tags sent and received Ok                == BEP_STATUS_SUCCESS
     *         Tag was added to list (waiting for last tag) == 1
     *         Read operation failed to read all tags       == -1
     *         Read operation failed                        == -2
     * @since Version 1.0
     */
    virtual INT32 Read(const XmlNode *node, std::string &response,
                       const bool IsFinal = false);
    /**
     * Write data to a server
     *
     * @param tag      Tag of data to be written
     * @param value    Acutal data to be written
     * @param response Data returned by operation
     * @param IsFinal  If this tag is the las tag to be requested
     * @return Status of operation
     * @since Version 1.0
     */
    virtual INT32 Write(const std::string &tag, const std::string &value,
                        std::string &response, const bool IsFinal = false);
    /**
     * Write data to a server
     *
     * @param node   Data to be written.
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of operation
     * @since Version 1.0
     */
    virtual INT32 Write(const XmlNode *node, std::string &response, const bool IsFinal = false);
    /**
     * Command a value in the system.  This would be used in the case where one 
     * process in the system needs to inform another process of an event.  An 
     * example is when the user presses the enter key and the driver display is 
     * on the start selection a Start event should be sent to the System 
     * Controller.
     *
     * @param tag      Command to be executed.
     * @param value    The value of the command being executed.
     * @param response Data returned by operation.
     * @param IsFinal  If this tag is the las tag to be requested.
     * 
     * @return Status of operation.
     */
    virtual INT32 Command(const std::string &tag, const std::string &value,
                          std::string &response, const bool IsFinal = false);
    /**
     * Command a value in the system.  This would be used in
     * the case where one process in the system needs to inform
     * another process of an event.  An example is when the
     * user presses the enter key and the driver display is on
     * the start selection a Start event should be sent to the
     * System Controller.
     *
     * @param node   Information to be commanded.
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of operation
     * @since Version 1.0
     */
    virtual INT32 Command(const XmlNode *node, std::string &response, const bool IsFinal = false);
    /**
     * Subscribe for notification of a specified tag
     *
     * @param tag    Tag to subcribe for notification
     * @param value    Server name that will be notified when data changes
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of subsribe operation
     * @since Version 1.0
     */
    virtual INT32 Subscribe(const std::string &tag, const std::string &value,
                            std::string &response, const bool IsFinal = false);
    /**
     * Subscribe for notification of a specified tag
     *
     * @param node   Data to be Subscribed for.
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of subsribe operation
     * @since Version 1.0
     */
    virtual INT32 Subscribe(const XmlNode *node, std::string &response, const bool IsFinal = false);
    /**
     * Remove from subscription list for notification of a specified tag
     *
     * @param tag    Tag to subcribe for notification
     * @param value    Server name that will be removed from subscription list
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of subsribe operation
     * @since Version 1.0
     */
    virtual INT32 Unsubscribe(const std::string &tag, const std::string &value,
                              std::string &response, const bool IsFinal = false);
    /**
     * Remove from subscription list for notification of a specified tag
     *
     * @param node   Data to Unsubscribe for.
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of subsribe operation
     * @since Version 1.0
     */
    virtual INT32 Unsubscribe(const XmlNode *node, std::string &response, const bool IsFinal = false);
    /**
     * Method used to handle data published to us from a server
     *
     * @param tag      Tag to publish to subcribers
     * @param value    Server name subscribed for data
     * @param response Data returned by operation
     * @param IsFinal  If this tag is the las tag to be requested
     *
     * @return Status of publish operation
     * @since Version 1.0
     */
    virtual INT32 Publish(const std::string &tag, const std::string &value,
                          std::string &response, const bool IsFinal = false);
    /**
     * Method used to handle data published to us from a server
     *
     * @param node     Node to be Published.
     * @param response Data returned by operation
     * @param IsFinal  If this tag is the las tag to be requested
     *
     * @return Status of publish operation
     * @since Version 2.4
     */
    virtual INT32 Publish(const XmlNode *node, std::string &response, const bool IsFinal = false);
    /**
     * Sends a register message to the object that is connected
     * to the communication channel
     *
     * @param server   Server to command
     * @param status   Command result
     * @param response Data returned by operation
     * @param IsFinal  If this tag request to perform
     * @return Status of operation
     * @since Version 2.6
     */
    virtual INT32 Register(const std::string &server, const std::string &status,
                           std::string &response, const bool IsFinal = false);
    /**
     * Sends a register message to the object that is connected
     * to the communication channel
     *
     * @param node   Server to command and result
     * @param response Data returned by operation
     * @param IsFinal  If this tag request to perform
     * @return Status of operation
     * @since Version 2.6
     */
    virtual INT32 Register(const XmlNode *node, std::string &response, const bool IsFinal = false);
    /**
     * Sends a terminate message to the object that is connected
     * to the communication channel
     *
     * @param response Response to request
     * @return Status of operation
     * @since Version 1.3
     */
    INT32 Terminate(std::string &response);
    /**
     * Sends a reset message to the object that is connected
     * to the communication channel
     *
     * @param response Response to request
     * @return Status of operation
     * @since Version 1.4
     */
    INT32 Reset(std::string &response);
    /**
     * Sends an abort message to the object that is connected
     * to the communication channel
     *
     * @param response Response to request
     * @return Status of operation
     * @since Version 1.4
     */
    INT32 Abort(std::string &response);
    /**
     * Sends a activate message to the object that is connected
     * to the communication channel
     *
     * @param response Response to request
     * @return Status of operation
     * @since Version 1.4
     */
    INT32 Activate(std::string &response);
    /**
     * Sends a deactivate message to the object that is connected
     * to the communication channel
     *
     * @param response Response to request
     * @return Status of operation
     * @since Version 1.4
     */
    INT32 Deactivate(std::string &response);
    /**
     * Sends a message to the object that is connected
     * to the communication channel to reload its configuration
     *
     * @param response Response to request
     * @return Status of operation
     * @since Version 1.4
     */
    INT32 ReloadConfiguration(std::string &response);
    /**
     * Wait for incomming messages.
     *
     * @return channel id of sender (BEP_STATUS_ERROR if failed)
     * @since Version 1.0
     */
    const INT32 WaitForRequest(void);
    /**
     * Wait for incomming messages with a specified timeout.
     *
     * @param timeout The amount of time to wait in nano-seconds.
     *
     * @return channel id of sender (BEP_STATUS_ERROR if failed)
     * @since Version 1.0
     */
    const INT32 WaitForRequestWithTimeout(uint64_t timeout);
    /**
     * Method that will actually handle the request from the
     * client.  This method calls the servers methods to perform
     * the requested operations.  This is the implementation of
     * the Visitor pattern in this class.
     *
     * @param message Message to populate with data that was sent back to client
     * @param server  Server that will fulfill the request of the client.
     * @param messagePopulated
     *                Flag to specify if the message being passed in has already been populated.
     *                TRUE - do not get message from the client.
     *                FALSE - get the message from the client.
     * @return Status of operation
     *         BEP_STATUS_ERROR    == operation failed
     *         BEP_STATUS_SUCCESS  == sucess
     * @since Version 1.0
     */
    virtual const INT32 HandleRequest(std::string &message, BepComponent *server = NULL, const bool &messagePopulated = false);
    /**
     * Retrieves the next tag from the response
     *
     * @param tag     Data requested
     * @param value   Value of data
     * @param message Message being processed
     * @return Number of items left to process
     * @since Version 1.0
     */
    virtual const INT32 GetNext(std::string &tag, std::string &value, std::string &message);
    /**
     * Retrieves the next tag from the response.
     *
     * @param node    Node to populate.
     * @param message Message being processed.
     * 
     * @return Number of items left to process
     */
    const INT32 GetNext(XmlNode &node, std::string &message);
    /**
     * Retrieves the tag requested from the response
     *
     * @param tag     Data requested
     * @param value   Value of data
     * @param message Message being processed
     * @return Status of operation
     *         BEP_STATUS_SUCCESS == Tag requested is valid in response
     *         BEP_STATUS_ERROR   == Tag not in response
     *         BEP_STATUS_FAILURE == No tags in response
     * @since Version 1.5
     */
    virtual const INT32 GetByTag(const std::string tag, std::string &value, std::string &message);
    /**
     * Returns the status of data (from GetNext() for example)
     *
     * @return Status of data
     * @since Version 2.10
     */
    const std::string GetMessageStatus(void);

    /**
     * Indicates whether or not a terminate command was received
     *
     * @return If a terminate message was received
     * @since Version 1.3
     */
    const bool WasTermiateReceived(void);

    /**
     * Get the named channel the object is using.
     *
     * @return Name of the channel (IPC only).
     * @since Version 1.6
     */
    virtual const std::string GetName(void);
    /**
     * Get port TCP/IP object is connected to
     *
     * @return Port number
     * @since Version 1.0
     */
    const INT16 GetPort(void);
    /**
     * Get the IP address that the server
     * is operating on.
     *
     * @return A string containing the IP address of the server in
     *         the form: xxx.xxx.xxx.xxx
     */
    const std::string GetIPAddress(void);
    /**
     * Get the frame start string.  The start string is
     * what is used to indicate a start of transmission.
     *
     * @return The start string.
     */
    const std::string GetFrameStart(void);
    /**
     * Get the frame end string.  The end string is
     * what is used to indicate a start of transmission.
     *
     * @return The end string.
     */
    const std::string GetFrameEnd(void);
    /**
     * Resets Communication structure so a message can be resent.
     *
     * @since Version 1.7
     */
    void ResetCommunications(void);
    /**
     * Gets the type of communication used (TCP/IP, IPC, etc.)
     *
     * @return Communication type used by class
     * @since Version 1.8
     */
    const std::string GetCommunicationType(void);
    /**
     * Gets the data rate from the message request (for read operations - is number of times per second)
     *
     * @return Data rate
     * @since Version 1.9
     */
    const INT32 GetDataRate(void);
    /**
     * Sets the data rate from the message request (for read operations - is number of times per second)
     *
     * @param rate Times per second data will be sent
     * @since Version 1.2
     */
    void SetDataRate(const INT32 rate);
    /**
     * Get the time that sending data will fail if not sent and replied to
     *
     * @return The amount of time in millliseconds before a timeout will
     *         occur.
     * @since Version 1.3
     */
    const UINT32 GetTimeout(void);
    /**
     * Set the time that sending data will fail if not sent and replied to
     *
     * @param timeout Value to be set for timeout (in millliseconds)
     * @since Version 1.3
     */
    void SetTimeout(const UINT32 timeout);
    /**
     * Returns the id of the channel being used by object
     *
     * @return Id for channel (channel id, socket number, etc.)
     * @since Version 2.5
     */
    const INT32 GetId(void) const;
    /**
     * Checks if data response was Ok
     *
     * @param data   Data to be evaluated
     * @return If data is not a failure mode
     * @since Version 1.0
     */
    bool IsDataOk(const XmlString &data);
    /**
     * Performs selected operation (Read, Write, Subscribe, ...)
     *
     * @param operation Operation type (Read, Write, ...)
     * @param dataNode  Data for operation
     * @param message   Response data
     * @param IsFinal   If the tag is the last to be processed
     * @return Status of entire operation
     * @since Version 1.0
     */
    INT32 DoOperation(const std::string operation, const XmlNode *dataNode,
                      std::string &message, const bool IsFinal);
    /**
     * Decodes the type of request
     *
     * @param message Message to decode
     * @return Type of message
     * @since Version 1.0
     */
    const std::string GetRequestType(const std::string &message);
    /**
     * Decodes the type of request
     *
     * @param document Document to decode
     * @return Type of message
     * @since Version 1.0
     */
    const std::string GetRequestType(const XmlNode* document);
    /**
     * Closes communication channel object is connected to
     * @since Version 2.8
     */
    virtual void Close(void) const;
    /**
     * Get the size of the receive buffer.
     *
     * @return The size of the receive buffer.
     * @since Version 2.9
     */
    INT32 GetRxBufferSize(void);
    
protected:
    /**
     * Sets the status of data (for GetNext() for example)
     *
     * @param stat   Status to set
     * @since Version 2.10
     */
    void SetMessageStatus(const std::string& stat);
    /**
     * Return to the sender a response that indicates
     *
     * @param tag    Tag to subcribe for notification
     * @param response Data returned by operation
     * @return Status of subsribe operation
     * @since Version 1.0
     */
    virtual INT32 Error(const std::string &tag, std::string &response);
    /**
     * Return to the sender a response that indicates
     *
     * @param node    Tag to subcribe for notification
     * @param response Data returned by operation
     * @return Status of subsribe operation
     * @since Version 1.0
     */
    virtual INT32 Error(const XmlNode *node, std::string &response);
    /**
     * Special handling to be done for each node.
     *
     * @param node   Node on which to operate.
     * @param type   Type of operation being performed.
     * @param server The server requesting the special processing. 
     */
    virtual void SpecialProcessing(XmlNode *node, const std::string type, BepComponent *server);
    /**
     * Gets the request from the sender
     *
     * @param message Buffer to populate with request
     * @return Length of message
     * @since Version 1.1
     */
    const INT32 GetRequest(std::string &message);
    /**
     * Validate an incomming message and return if it is valid
     * or not.
     *
     * @param doc    Document to be validated
     * @return Status of operation
     * @since Version 1.0
     */
    virtual bool ValidateMessage(const XmlNode* doc);
    /**
     * Build a message compliant with the Bep communication protocol
     *
     * @param function Function message will perform.
     * @param message  Location to store the new message.  
     * 
     * @return Message that was built.
     */
    virtual void BuildMessage(const std::string function, std::string& message);
    /**
     * Send response to request
     *
     * @param doc       Data to send
     * @param component Component to use when handling request
     * @return If operation succeded
     * @since Version 1.0
     */
    virtual const bool SendResponse(XmlNode* doc, BepComponent* component);
    /**
     * Send response to request
     *
     * @param message   Data to send
     * @return If operation succeded
     * @since Version 1.0
     */
    const bool SendResponse(const std::string &message);
    /**
     * Sets the type of communication used (TCP/IP, IPC, etc.)
     *
     * @param commType Communication type used by class
     * @since Version 1.8
     */
    void SetCommunicationType(const std::string &commType);
    /**
     * Checks to see if the node is already in the
     * m_sendData map.  If not, it adds it to the list.
     *
     * @param tag    Tag of the node we are interested in.
     * @return The node we were looking for.
     * @since Version 2.3
     */
    const XmlNode* FindNode(const std::string &tag);
    /**
     * Indicates whether or not a terminate command was received
     *
     * @param terminate Status to set terminate variable
     * @since Version 1.3
     */
    void SetTerminate(const bool terminate);
    /**
     * Determines if message received was a terminate message
     *
     * @param node   Node to process
     * @return If message was a terminate message
     * @since Version 1.3
     */
    const bool IsTerminateMessage(const XmlNode* node);
    /**
     * Determines if message received was an abort message
     *
     * @param node   Node to process
     * @return If message was an abort message
     * @since Version 1.3
     */
    const bool IsAbortMessage(const XmlNode* node);
    /**
     * Determines if message received was a reset message
     *
     * @param node   Node to process
     * @return If message was a reset message
     * @since Version 1.3
     */
    const bool IsResetMessage(const XmlNode* node);
    /**
     * Verifies that the attribute is valid
     *
     * @param attribute value of attribute
     * @return If valid
     * @since Version 1.8
     */
    virtual bool CheckAttribute(const XmlNode *attribute);
    /**
     * Adds an attribute to an XmlNode
     *
     * @param name   Name of attribute
     * @param value  Value of attribute
     * @param node   Node to add attribute to
     * @since Version 1.8
     */
    void AddAttribute(const std::string &name, const std::string &value, XmlNode* &node);

    /**
     * Set the size of the receive buffer.
     *
     * @return The size of the receive buffer.
     * @since Version 2.9
     */
    void SetRxBufferSize(INT32 length);


protected:
    /**
     * Communication object (IPC, TCP/IP, ...)
     * @since Version 1.0
     */
    ISystemCommunication* m_ISystemCommunication;
    /**
     * Data to be sent to destination
     * @since Version 1.0
     */
    XmlNodeMap m_sendData;
    /**
     * Xml parser to do Xml dirty work
     * @since Version 1.0
     */
    XmlParser m_parser;

    /**
     * Mutex used to guard member data
     */
    pthread_mutexattr_t m_lockAttr;
    pthread_mutex_t     m_lock;

private:
    /**
     * Status variable indicating whether or not a terminate message
     * was received
     * @since Version 1.3
     */
    bool m_terminate;
    /**
     * The last operation that was performed.
     * @since Viersion 1.5
     */
    std::string m_oper;
    /**
     * The type of communication used (TCP/IP, IPC, etc.)
     * @since Version 1.8
     */
    std::string m_commType;
    /**
     * The size of the receive buffer.
     * @since Version 2.9
     */
    INT32 m_rxBufferSize;
    /**
     * Rate at which data will be sent
     * @since Version 2.2
     */
    INT32 m_rate;
    /**
     * Status of message (used primarily for error codes)
     * @since Version 2.10
     */
    std::string m_msgStat;

};
#endif  // IBepCommunicaiton

