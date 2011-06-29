//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/ITcpIp.h 5     10/30/07 3:44p Rwiersem $
//
// FILE DESCRIPTION:
//   This class provides all of the methods to communicate via TCP/IP on the
//   system.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/ITcpIp.h $
// 
// 5     10/30/07 3:44p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     9/29/04 10:21p Cward
// Allow for changes in buffersize
//
// 3     4/14/04 6:38p Cward
// The Initialize method sigsegs when default params are used. This was
// due to the default address param being NULL instead of "".
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:15p Khykin
// Initial checkin.
//
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:40  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:06a Khykin
// Initial Checkin
//
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
//
// 28    7/31/02 11:40a Skypig13
// Added fd counter and removed print in Close()
//
// 27    5/30/02 4:21p Skypig13
// Made some methods const and removed uneccesary includes
//
// 26    4/02/02 11:09p Skypig13
// Updated to not close socket when destroyed if initialized with file
// descriptor
//
// 25    2/18/02 7:56p Skypig13
// Updated default value for frame start
//
// 24    2/07/02 6:28p Skypig13
// Updated diagnostics and SetSocket()
//
// 23    2/05/02 12:44a Skypig13
// Added new Initialize method
//
// 22    12/19/01 12:57p Skypig13
// Added GetMessage() for handling multiple messages in receive buffer
//
// 21    12/05/01 2:21p Skypig13
// Added backlog configurability
//
// 20    10/23/01 7:56p Skypig13
// Removed READ_BUFFER_SIZE (used DEFAULT_BUFFER_SIZE)
//
// 19    10/12/01 10:32a Dan.erickson
// Added constructor to allow construction of Tcp/Ip object with
// arguments.
//
// 18    10/01/01 4:33p Skypig13
// Fixed BepException calls to properly give information on errors
//
// 17    9/17/01 8:52a Skypig13
// Added pthread_testcancel() to all loops around interruptable
// kernel calls
//
// 16    8/29/01 1:55p Kh
// Missed the m_debug ivar, removing.
//
// 15    8/27/01 7:28p Kh
// Updating for NTO 6.1
//
// 14    7/06/01 3:16p Kh
// Removing debug methods due to Debuggable inheritance.
//
// 13    7/06/01 10:09a Kh
// Updating due to base class changes.
//
// 12    6/21/00 5:48p Skypig13
// Changed Initialize method to conform with new base class
//
// 11    6/13/01 4:56p Kh
// Changing the ProcessRequest due to common usage in the base class.
//
// 10    6/12/00 8:35a Kh
// Update debug methods for common name usage.
//
// 9     6/11/00 3:27p Kh
// Updating the class to use the m_id methods of the base class.
//
// 8     6/08/00 5:22p Kh
// Updating to take a socket to communicate on.
//
// 7     6/04/00 4:07p Kh
// Updated Initialize methods to take debug status.
//
// 6     6/01/00 1:37p Kh
// Adding debug variable.
//
// 5     5/25/00 8:06a Kh
// Updating comments.
//
// 4     5/25/00 7:41a Kh
// Updating to match the ISystemCommunicaiton interface.
//
// 3     5/17/00 7:33a Kh
// Updating Read method for use with the FrameStart and FrameEnd tags that
// are entered in the configuration file.
//
// 2     5/04/00 5:22p Kh
// Update the Read and Write methods.
//
// 1     5/02/00 8:11p Kh
// Name changed
//
// 1     5/01/01 10:06a Kh
// TCP/IP Interface Class
//
//*************************************************************************
#ifndef ITCP_IP_H
#define ITCP_IP_H
//==============================================================================

#include <stddef.h>
#include <stdlib.h>

#include <netinet/in.h>     // for sockaddr_in definition.

#include "BepDefs.h"
#include "ISystemCommunication.h"

class ITcpIpTest;

/**
 * Class: ITcpIp
 * Description: The interface to the system TCP/IP functions.
 * Category: Design Model::Services Layer::Base Reuse::System Communiation
 * Persistence: Transient
 * Cardinality/Multiplicity: n
 *
 * @author Kevin Hykin
 * @version 1.3
 */
class ITcpIp : public ISystemCommunication
{
public:
    friend class ITcpIpTest;
    /**
     * Constructors (generated)
     */
    ITcpIp(void);
    /**
     * Constructors
     *
     * @param type       Type of object (Client, Server, etc.)
     * @param port       Port to connect to
     * @param address    IP address to use
     * @param id         Socket
     * @param debug      If diagnostic prints should be sent to the console
     * @param frameStart Frame Delimiter indicating beginning of frame
     * @param frameEnd   Frame Delimiter indicating end of frame
     * @param backlog    Number of clients that can be waiting on accepted connections
     */
    ITcpIp(const std::string type,
           const INT16 port,
           const std::string address,
           const INT32 id = -1,
           const bool debug = false,
           const std::string frameStart = BEP_XML_ROOT,
           const std::string frameEnd = BEP_XML_ROOT_END,
           const INT32 backlog = 5);
    /**
     * Destructor (generated)
     */
    virtual ~ITcpIp(void);
    /**
     * Method used to initialize the object with an XML
     * configuration.  The configuration must be parsed to
     * the XML node that needs to be configured.
     *
     * The TCP node must be parsed by the main applliation
     * and then the node reference provided to the method
     * for initialization.
     * Note: if more than one TCP/IP port configuration
     * needed, the process would need the knowlege to parse
     * the nodes.
     *
     * @param config Node that points to the parsed documents TCP node
     *               configuration.
     * @param id     The socket id to communiate on.  If this is set, the
     *               object will configure itself to communicate on the
     *               socket provided.
     * @param rid    Receive id of sender (not used)
     */
    void Initialize(const XmlNode *config,
                    const INT32 id=-1,
                    const INT32 rid=-1);
    /**
     * Method to initialize the TCP/IP object as a client
     * or server.
     *
     * @param type    The type of object to initialize (Client or Server)
     * @param port    The port number to connect to.
     * @param address The address to connect to.
     *                Note: This is only needed by the client.
     * @param debug   Debug status. "On" or "Off"
     */
    virtual void Initialize(const std::string &type,
                            const INT16 port=-1,
                            const std::string address="",
                            const bool debug=false);
    /**
     * Method to initialize the TCP/IP object as a client
     * or server.
     *
     * @param type    The type of object to initialize (Client or Server)
     * @param port    The port number to connect to.
     * @param address The address to connect to.
     *                Note: This is only needed by the client.
     * @param debug   Debug status. "On" or "Off"
     */
    virtual void Initialize(const char *type,
                            const INT16 port=-1,
                            const char *address="",
                            const bool debug=false);
    /**
    * Method to initialize the TCP/IP object as a client
    * or server.
     *
     * @param type       Type of object (Client, Server, etc.)
     * @param port       Port to connect to
     * @param address    IP address to use
     * @param id         Socket
     * @param frameStart Frame Delimiter indicating beginning of frame
     * @param frameEnd   Frame Delimiter indicating end of frame
     * @param backlog    Number of clients that can be waiting on accepted connections
     * @param debug      If diagnostic prints should be sent to the console
     */
    virtual void Initialize(const std::string type,
                            const INT16 port,
                            const std::string address,
                            const INT32 id,
                            const bool debug = false,
                            const std::string frameStart = BEP_XML_ROOT,
                            const std::string frameEnd = BEP_XML_ROOT_END,
                            const INT32 backlog = 5);
    /**
     * Method used by a server to receive requests from
     * clients.
     *
     * @return The socket that the new client is on.
     */
    INT32 ProcessRequest(void);
    /**
     * Method used to read data from the specified socket.
     * It returns the socket number of the sender.
     *
     * @param message Reference to the message object to put the received
     *                message in.
     * @param id      The socket to perform the read operation on.
     * @return The status of the read process.
     *         -1 is fail
     */
    virtual INT32 Read(std::string &message, INT32 id=-1);
    /**
     * Method used to write data to the opened TCP/IP socket
     * in the object.
     *
     * @param id      The socket to perform the write operation on.
     * @param message The information to write to the socket.
     */
    virtual void Write(const std::string &message, INT32 id=-1);
    /**
     * Method used to write data to the opened TCP/IP socket
     * in the object.
     *
     * @param id      The socket to perform the write operation on.
     * @param message The information to write to the socket.
     */
    virtual void Write(const char *message, INT32 id=-1);
    /**
     * Method used to get the IP address that the server
     * is operating on.
     *
     * @return A string containing the IP address of the server in
     *         the form: xxx.xxx.xxx.xxx
     */
    const std::string GetIPAddress();
    /**
     * Get the Structure that holds the server information
     * for TCP/IP communication.
     *
     * @return The server information structure.
     */
    const struct sockaddr_in GetServer();
    /**
     * Get the port number that the TCP/IP object is
     * operating on.
     *
     * @return The current port number.
     */
    const INT16 GetPort();
    /**
     * Get the port number that the TCP/IP object is
     * operating on.
     *
     * @return The current socket number.
     */
    const INT32 GetSock() const;
    /**
     * Get the frame start string.  The start string is
     * what is used to indicate a start of transmission.
     *
     * @return The start string.
     */
    const std::string GetFrameStart();
    /**
     * Get the frame end string.  The end string is
     * what is used to indicate a start of transmission.
     *
     * @return The end string.
     */
    const std::string GetFrameEnd();
    /**
     * Get the number of allowable pending connection requests
     *
     * @return Number of requests allowed in backlog
     * @since Version 1.0
     */
    const INT32 GetBacklog(void);
    /**
     * Closes communication channel object is connected to
     * @since Version 1.3
     */
    virtual void Close(void) const;

    /**
     * Retrieves the total number of sockets opened
     *
     * @return Number of opened sockets
     * @since Version 1.2
     */
    static const UINT32 GetNumSockets(void);
    /**
     * Set the flag indicating that the socket should be closed when
     * the object is deleted or re-initialized
     *
     * @param close  Whether to close the socket
     */
    void SetCloseOnExit(const bool close);

protected:
    /**
     * Method used to initialize the TCP/IP object if
     * manual configuration was used.
     */
    virtual void Initialize(void);
    /**
     * Method used to set the IP address of the server.
     * Note: this is for a client application only.
     *
     * @param value  The address of the server.  It must be in the form
     *               of: xxx.xxx.xxx.xxx
     *               Ex. 10.0.1.120
     */
    virtual void SetIPAddress(const std::string &value);
    /**
     * Set the port number that the TCP/IP object is
     * operating on.
     *
     * @param value  The new port number.
     */
    virtual void SetPort(INT16 value);
    /**
     * Set the port number that the TCP/IP object is
     * operating on.
     *
     * @param value  The new port number.
     */
    virtual void SetPort(const std::string &value);
    /**
     * Set the socket number that the TCP/IP object is
     * operating on.
     *
     * @param value  The new socket number.
     */
    virtual void SetSock(const INT32 &value);
    /**
     * Set the Structure that holds the server information
     * for TCP/IP communication.
     *
     * @param value  The server information structure.
     */
    virtual void SetServer(const struct sockaddr_in &value);
    /**
     * Set the frame start string.  The start string is
     * what is used to indicate a start of
     *
     * @param value  The server information structure.
     */
    virtual void SetFrameStart(const std::string &value);
    /**
     * Set the frame end string.  The end string is
     * what is used to indicate a start of
     *
     * @param value  The server information structure.
     */
    virtual void SetFrameEnd(const std::string &value);
    /**
     * Method used to open a TCP/IP socket for communication.
     */
    void OpenSocket(void);
    /**
     * Method used to close a TCP/IP socket communication.
     *
     * @param newValue New socket value being requested
     */
    void CloseSocket(const INT16& newValue = -1) const;
    /**
     * Assign a local protocol address to a socket.
     */
    void BindSocket(void);
    /**
     * Set up number of allowable pending connection requests
     *
     * @param backlog Number of requests to set as backlog number
     * @since Version 1.0
     */
    void SetBackLog(INT32 backlog);
    /**
     * Gets message waiting in buffer or memory
     *
     * @param message message to populate
     * @param buffer  data received
     * @return If message found
     * @since Version 1.1
     */
    const bool GetMessage(std::string &message, char *buffer = NULL);

    /**
     * Return if the socket should be closed when the object
     * is deleted or re-initialized
     *
     * @return If the socket should be closed
     * @since Version 1.2
     */
    inline const bool GetCloseOnExit(void) const;

private:
    /**
     * Store the buffer size to use for communications.
     *
     * @return Buffer size to use.
     */
    const INT32& GetBufferSize(void);
    /**
     * Store the communication buffer size.
     *
     * @param bufferSize Size of the communication buffer that should be used.
     */
    void SetBufferSize(const INT32 &bufferSize);
    /**
     * The port that the object is operating on.
     * @since 1.0
     */
    INT16 m_port;
    /**
     * Size of the communication buffer to use.
     */
    INT32 m_bufferSize;
    /**
     * The buffer that holds all of the received data that
     * has not been processed.
     * @since 1.0
     */
    std::string m_receivedData;
    /**
     * The server structure that holds the TCP/IP server
     * configuration.
     * @since 1.0
     */
    struct sockaddr_in m_server;
    /**
     * The address of the server that the object is connected
     * to (in the case that the object is a client object).
     * @since 1.0
     */
    std::string m_address;
    /**
     * The string that indicates the start of a
     * transmission.
     * @since 1.0
     */
    std::string m_frameStart;
    /**
     * The string that indicates the end of a
     * transmission.
     * @since 1.0
     */
    std::string m_frameEnd;
    /**
     * Backlog of incomming connection requests
     * @since Version 1.0
     */
    INT32 m_backlog;
    /**
     * Flag indicating whether to close socket when re-initialized or deleted
     * @since Version 1.2
     */
    bool m_closeOnExit;
    /**
     * Total number of open sockets
     * @since Version 1.2
     */
    static UINT32 m_openSockets;
};

#endif
