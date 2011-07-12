//******************************************************************************
// Description:
//  Logical serial port driver for BBK Wireless device communication
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/BBKWireless/BBKWirelessProxy.h $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _BBK_WIRELESS_PROXY_H_INCLUDED
#define _BBK_WIRELESS_PROXY_H_INCLUDED

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

/**
 * Flag used to indicate whether or not the reader thread will process all received data.
 * If set to 0, the reader thread will simply stick received data into our rx fifo and 
 * send a pulse back to our I/O channel to indicate that new rx data needs to be processed
 */
#define READER_PROCESSES		1

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "RawCommProxy.h"		// Base logical port driver class

#include <stdint.h>				// uint8_t
#include <set>
#include <map>
#include <string>
#include <fcntl.h>				// O_CREAT

/**
 * Name to register the ISO port driver process under
 */
#define BBK_WIRELESS_PROC_NAME           "BBKWirelessProxy"

/**
 * ISO serial port communication proxy. Used to perform port locking and serial
 * data logging. Clients will communicate to a serial device through this process.
 *
 * @author Brian Meinke
 * @version 1
 * @see RawCommProxy
 * @since Jan 20, 2003
 */
class BBKWirelessProxy : public RawCommProxy
{
public:
    /**
     * Default constructor
     */
    BBKWirelessProxy();

    /**
     * Constructor from command line arguments
     *
     * @param argc   Number of command line arguments to aprse
     * @param argv   Array of command line argument strings
     */
    BBKWirelessProxy( int argc, char *argv[]);

    /**
     * Class destructor
     */
    virtual ~BBKWirelessProxy();

    /**
     * Reads logical port configuration from an XML document
     *
     * @param portNode Node containing setup data for our port only
     */
    virtual void ReadPortConfig( const XmlNode *portNode);

    /**
     * Writes data to the driver
     * 
     * @param buff   The data buffer to write
     * @param bytes  The number of bytes to write from buff
     * 
     * @return The number of bytes written, or -1 if an error occurred (errno is set).
     */
    virtual int Write( const void* buff, size_t bytes);

private: 

    /**
     * Start byte of BBK wireless protocol
     */
    uint8_t             m_startByte;

    /**
     * Start byte of BBK wireless protocol
     */
    uint8_t             m_stopByte;

};

#endif // _BBK_WIRELESS_PROXY_H_INCLUDED
