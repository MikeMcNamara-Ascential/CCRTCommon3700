//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/TcpIp/TcpCommProxy.h $
// $Author: Rwiersem $
//   $Date: 10/30/07 3:59p $
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/TcpIp/TcpCommProxy.h $
// 
// 3     10/30/07 3:59p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:29p Cward
//
//
//******************************************************************************

#ifndef _TCPCOMMPROXY_H_INCLUDED
#define _TCPCOMMPROXY_H_INCLUDED

// Forward class references
class TcpCommProxy;
struct _CommIoOcb_t;

typedef struct _CommIoOcb_t TcpIoOcb_t;

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

// Define this here so it does not get defined in the neutrino headers

#ifndef IOFUNC_OCB_T
#define IOFUNC_OCB_T            TcpIoOcb_t
#endif

#ifndef RESMGR_OCB_T
#define RESMGR_OCB_T            TcpIoOcb_t
#endif

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "RawCommProxy.h"   // Base class

#include <sys/types.h>              // socket stuff
#include <sys/socket.h>             // socket stuff
#include <netinet/tcp.h>

#include <vector>

/**
 * Name to register the J2190 port driver process under
 */
#define TCP_PROXY_NAME          "TcpProxy"

/**
 * Path we will adopt under the /dev/CommProxy dir
 */
#define AVT_J2190_DEV_PATH      "TcpIp"

/**
 * Comm proxy class used to talk via TCP/IP
 *
 * @author BMeinke
 * @version 1.0
 * @see RawCommProxy
 * @since September 26, 2003
 */
class TcpCommProxy : public RawCommProxy
{
public:

    /**
     * Default Constructor
     */
    TcpCommProxy();

    /**
     * Constructor from command line arguments
     *
     * @param argc   Number of command line arguments to parse
     * @param argv   Array of command line argument strings
     */
    TcpCommProxy(int argc, char *argv[]);

    /**
     * Class destructor
     */
    ~TcpCommProxy();

    /**
     * Performs any necessary manager cleanup (free memory, detach any
     * adopted path names, etc)
     */
    virtual void CleanupManager();

protected:
    
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
    virtual bool HandleCommandLineArg(int option, const char *value);

    /**
     * Parses an ip address & port number from a string with
     * the following format address:port
     *
     * @param rawIpAddr String containing the ip address and port number in the
     *                  following format address:port
     * @param ipAddress IP Address parsed from the given rawIpAddr string
     * @param portNum   Port number parsed from the rwIpAddr string
     */
    void ParseIpAddress( const char *rawIpAddr, std::string &ipAddress, int &portNum);

    /**
     * Reads logical port configuration from an XML document
     *
     * @param portNode Node containing setup data for our port only
     */
    virtual void ReadPortConfig( const XmlNode *portNode);

    /**
     * Reads the port protocol for this logical port from
     * the given XML node
     *
     * @param portNode Node containing setup data for our port only
     */
    virtual void ReadPortProtocol( const XmlNode *portNode);

    /**
     * Method used to open a handle to the communication device driver
     *
     * @param devName Path to the communication device to be opened (i.e. /dev/ser1)
     * @param mode    Mode to open the device in (i.e. O_RDWR | O_NONBLOCK)
     *
     * @return A file descriptor to use for communicatin with the device driver or
     *         -1 if an error occurred
     */
    virtual int OpenPort( const char *devName, int mode=O_RDWR);

    /**
     * Method used to close a handle to the communication device driver
     *
     * @return 0 if successful or -1 if an error occurred
     */
    virtual int ClosePort();

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
    virtual int ConfigurePort( const SerialProtocol &portCfg, int mode=TCSANOW);

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
     * Connects to the server defined by the m_serverIp and m_serverPort data members
     *
     * @return -1 if an error occurred. Any other value represents a valid file descriptor
     *         to use when communicating with the server
     */
    int Connect();

    /**
     * Disconnects from the server defined by the m_serverIp and m_serverPort data members
     */
    void Disconnect();

    /**
     * IP Address of the server we will connect to
     */
    std::string     m_ipAddress;

    /**
     * Server port number we will connect to
     */
    int             m_ipPort;

    /**
     * Flag used to indicate if we will act as a TCP/IP client (connect()) or server (accept())
     */
    bool            m_amClient;

private:

};


#endif //_TCPCOMMPROXY_H_INCLUDED

