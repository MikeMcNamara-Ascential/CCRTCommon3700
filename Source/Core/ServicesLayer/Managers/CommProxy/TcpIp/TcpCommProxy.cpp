//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/TcpIp/TcpCommProxy.cpp $
// $Author: Rwiersem $
//   $Date: 1/18/07 12:56p $
//
// Description:
//    Comm proxy used to talk via a TCP/IP connection
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/TcpIp/TcpCommProxy.cpp $
// 
// 7     1/18/07 12:56p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added a call to DecodeIpAddress() when getting the IP address in
// ReadPortConfig().
// 
// 6     5/08/06 7:32p Cward
// Fixed missing updates
//
// 5     4/13/06 4:01a Cward
// removed usage block
//
// 4     2/15/06 7:57p Cward
// Updated for 06021501 iteration
//
// 3     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:29p Cward
//
//
//******************************************************************************
#include <netdb.h>      // gethostbyname, hostent
#include <arpa/inet.h>  // hotns

#include "TcpCommProxy.h"

//==============================================================================

/**
 * Default Constructor
 */
TcpCommProxy::TcpCommProxy() : RawCommProxy(),
    m_ipAddress( "Invalid"), m_ipPort( (uint32_t)-1), m_amClient( true)
{
}

/**
 * Constructor from command line arguments
 *
 * @param argc   Number of command line arguments to parse
 * @param argv   Array of command line argument strings
 */
TcpCommProxy::TcpCommProxy( int argc, char *argv[]) : RawCommProxy(),
    m_ipAddress( "Invalid"), m_ipPort( -1), m_amClient( true)
{
    ParseCommandLine( argc, argv, "i:P:p:R");
}

/**
 * Class destructor
 */
TcpCommProxy::~TcpCommProxy()
{
    Disconnect();
}

/**
 * Method used to handle a command line argument. Called from
 * ParseCommandLine() for each comamnd line argument encountered.
 *
 * @param option The command line option (i.e. 'f' if -f specified on command line)
 * @param value  Value associated with the command line option being processed.
 *               May be NULL is no argument is associated with the command line
 *               option
 *
 * @return true if the argument was processed, false if not recognized
 */
bool TcpCommProxy::HandleCommandLineArg( int option, const char *value)
{
    bool    retVal = true;

    Log( LOG_FN_ENTRY, "Enter TcpCommProxy::HandleCommandLineArg( %c, '%s')\n", option, optarg == NULL ? "" : optarg);

    switch( option)
    {
    case 'i':       // IP Address to connect to
        if( optarg != NULL)
        {
            ParseIpAddress( optarg, m_ipAddress, m_ipPort);
        }
        break;
    default:
        RawCommProxy::HandleCommandLineArg( option, value);
        break;
    }
    Log( LOG_FN_ENTRY, "Exit TcpCommProxy::HandleCommandLineArg( %c, '%s')\n", option, optarg == NULL ? "" : optarg);

    return( retVal);
}

/**
 * Parses an ip address & port number from a string with
 * the following format <address>:<port>
 *
 * @param rawIpAddr String containing the ip address and port number in the
 *                  following format <address>:<port>
 * @param ipAddress IP Address parsed from the given rawIpAddr string
 * @param portNum   Port number parsed from the rwIpAddr string
 */
void TcpCommProxy::ParseIpAddress( const char *rawIpAddr, std::string &ipAddress, int &portNum)
{
    char *port;

    portNum = 0;
    if( (port = strchr( (char*)rawIpAddr, ':')) != NULL)
    {
        // Grab the IP address from the beginning of the string
        ipAddress = std::string( rawIpAddr, ((int)port-(int)rawIpAddr)+1);

        // Read the port number specified after the colon
        if( port[1] != 0)       portNum = BposReadInt( &port[ 1]);
    }
    else
    {
        ipAddress = std::string( rawIpAddr);
    }
}

/**
 * Reads logical port configuration from an XML document
 *
 * @param portNode Node containing setup data for our port only
 */
void TcpCommProxy::ReadPortConfig( const XmlNode *portNode)
{
    Log( LOG_FN_ENTRY, "Enter TcpCommProxy::ReadPortConfig(%s)\n", portNode->getName().c_str());

    try
    {
        // Initialize base class
        RawCommProxy::ReadPortConfig( portNode);
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tXmlException initializing ResManagerMt:: %s\n",
             err.GetReason());
    }
    catch( BepException &err)
    {
        Log( LOG_ERRORS, "\tBepException initializing ResManagerMt:: %s\n",
             err.GetReason());
    }

    // Read TCP/IP specific configuration
    try
    {
        const XmlNode *ipNode = portNode->getChild( "Setup/IpAddress");
        m_ipAddress = DecodeIpAddress(XmlToAscii( ipNode->getValue()));
        
        const XmlNode *portNum = portNode->getChild( "Setup/PortNo");
        m_ipPort = atoi( portNum->getValue().c_str());

        const XmlNode *typeNode = portNode->getChild( "Setup/TcpType");
        m_amClient = (stricmp( "Server", typeNode->getValue().c_str()) != 0);

        Log( LOG_DEV_DATA, "\tConnect to %s:%d as %s\n", m_ipAddress.c_str(), m_ipPort, m_amClient ? "Client" : "Server");

        m_comPortName = "/dev/socket";
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tNo IP Address speicified, checking for host\n");
        try 
        {
            //Check for hostname specification
            m_ipAddress = portNode->getChild( "Setup/Host")->getValue();
        }
        catch (XmlException &err)
        {
            throw( BepException( "No IP or Host address information in ( <Setup/SerialDevice> )"));
        }
    }

    Log( LOG_FN_ENTRY, "Exit TcpCommProxy::ReadPortConfig()\n");
}

/**
 * Reads the port protocol for this logical port from
 * the given XML node
 *
 * @param portNode Node containing setup data for our port only
 */
void TcpCommProxy::ReadPortProtocol( const XmlNode *portNode)
{
    Log( LOG_FN_ENTRY, "Enter TcpCommProxy::ReadPortProtocol(%s)\n", portNode->getName().c_str());

    RawCommProxy::ReadPortProtocol(portNode);

    /*
        Check for socket options:
            TCP_NOWAIT
    */

    Log( LOG_FN_ENTRY, "Exit TcpCommProxy::ReadPortProtocol()\n");
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void TcpCommProxy::CleanupManager()
{
    Log( LOG_FN_ENTRY, "Enter TcpCommProxy::CleanupManager()\n");

    Disconnect();

    RawCommProxy::CleanupManager();

    Log( LOG_FN_ENTRY, "Exit TcpCommProxy::CleanupManager()\n");
}

/**
 * Method used to open a handle to the communication device driver
 *
 * @param devName Path to the communication device to be opened (i.e. /dev/ser1)
 * @param mode    Mode to open the device in (i.e. O_RDWR | O_NONBLOCK)
 *
 * @return A file descriptor to use for communicatin with the device driver or
 *         -1 if an error occurred
 */
int TcpCommProxy::OpenPort( const char *devName, int mode)
{
    int retVal;

    retVal = Connect();

    return( retVal);

}

/**
 * Method used to close a handle to the communication device driver
 *
 * @return 0 if successful or -1 if an error occurred
 */
int TcpCommProxy::ClosePort()
{
    if( m_comPortFd != -1)
    {
        Disconnect();
    }

    return( 0);
}

/**
 * Apply configuration settings to a communication port
 *
 * @param portCfg Port configuration data
 * @param mode    When the settings whould be applied:
 *                TCASNOW: Apply settings now
 *                TCSDRAIN: Wait until all output has been transmitted before
 *                applying new configuration
 *                TCSAFLUSH: Wait until all output has been transmitted before
 *                applying new configuration at which point all
 *                received data is flushed
 *
 * @return 0 if successful, -1 if an error occurred
 */
int TcpCommProxy::ConfigurePort( const SerialProtocol &portCfg, int mode)
{
    return( 0);
}

/**
 * Tells the communication driver to flush our FIFO(s)
 *
 * @param mode   Which FIFO(s) to flush:
 *               TCIOFLUSH: Flush our rx and tx FIFOs
 *               TCIFLUSH:  Flush only our rx FIFO
 *               TCOFLUSH:  Flush only our tx FIFO
 * @return -1 if an error occurred, any other value for success
 */
int TcpCommProxy::FlushDriverFifos( int mode)
{
    return( 0);
}


/**
 * Connects to the server defined by the m_serverIp and m_serverPort data members
 *
 * @return -1 if an error occurred. Any other value represents a valid file descriptor
 *         to use when communicating with the server
 */
int TcpCommProxy::Connect()
{
    struct hostent *hostEnt;
    struct sockaddr_in server;
    int     sock;
    int     retVal = -1;

    // Log the function entry
    Log( LOG_FN_ENTRY, "Enter TcpCommProxy::Connect() ipAddr=<%s>, port=%d\n", m_ipAddress.c_str(), m_ipPort);

    // Make sure no double opens
    Disconnect();

    // Clear out the server info
    memset( &server, 0, sizeof(server));

    // Get the host name
    if( (hostEnt = gethostbyname(m_ipAddress.c_str())) != NULL)
    {
        // Fill in the sockaddr struct with the true IP Address
        memcpy(&server.sin_addr, hostEnt->h_addr, hostEnt->h_length);
        server.sin_port = htons(m_ipPort);
        server.sin_family = hostEnt->h_addrtype;
        retVal = -1;
        while (retVal == -1)
        {//wait for connection to be established

            // return from this call is our system "port" number
            if( (sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != -1)
            {
                int on = 1;
                m_comPortFd = sock;
                if( (retVal=setsockopt( m_comPortFd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof( on))) != -1)
                {
                    // this will crash if socket above returns as NULL
                    if( (retVal = connect(m_comPortFd, (struct sockaddr*)&server, sizeof(server))) == -1)
                    {
                        Log( LOG_ERRORS, "Error connecting to <%s>:%d : %s\n", m_ipAddress.c_str(), m_ipPort, strerror( errno));
                        close( m_comPortFd);
                    }
                    else
                    {
                        retVal = m_comPortFd;
                    }
                }
                else
                {
                    Log( LOG_ERRORS, "Error connecting to <%s>:%d : %s\n", m_ipAddress.c_str(), m_ipPort, strerror( errno));
                    close( m_comPortFd);
                }
            }
            else
            {
                Log( LOG_ERRORS, "Error connecting to <%s>:%d : %s\n", m_ipAddress.c_str(), m_ipPort, strerror( errno));
            }
            if (retVal == -1 || sock == -1)
            {
                delay( 1000);
            }
        }
    }
    else
    {
        Log( LOG_ERRORS, "Error resolving host <%s>\n", m_ipAddress.c_str());
    }

    // will return a retVal < 0 for an error, another thing to handle.
    Log( LOG_FN_ENTRY, "Exit TcpCommProxy::Connect() ipAddr=<%s>, port=%d, socket=%d\n", m_ipAddress.c_str(), m_ipPort, retVal);

    return(retVal);
}

/**
 * Disconnects from the server defined by the m_serverIp and m_serverPort data members
 */
void TcpCommProxy::Disconnect()
{
    Log( LOG_FN_ENTRY, "Enter TcpCommProxy::Disconnect()\n");

    if( m_comPortFd != -1)
    {
        close( m_comPortFd);
        m_comPortFd = -1;
    }

    // pull the plug on the ethernet?
    Log( LOG_FN_ENTRY, "Exit TcpCommProxy::Disconnect()\n");
}

