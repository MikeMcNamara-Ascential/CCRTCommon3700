//******************************************************************************
// Description:
//  Logical serial port driver for BBK Wireless device serial communication
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/BBKWireless/BBKWirelessProxy.cpp $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "BBKWirelessProxy.h"

/**
 * Default Constructor
 */
BBKWirelessProxy::BBKWirelessProxy() : RawCommProxy()
{
    SetProcessName( BBK_WIRELESS_PROC_NAME);
}

/**
 * Constructor from command line arguments
 *
 * @param argc   Number of command line arguments to aprse
 * @param argv   Array of command line argument strings
 */
BBKWirelessProxy::BBKWirelessProxy( int argc, char *argv[]) : RawCommProxy( argc, argv)
{
}

/**
 * Class destructor
 */
BBKWirelessProxy::~BBKWirelessProxy()
{
}

/**
 * Reads logical port configuration from an XML document
 *
 * @param portNode Node containing setup data for our port only
 */
void BBKWirelessProxy::ReadPortConfig(const XmlNode *portNode)
{
	Log( LOG_FN_ENTRY, "Enter BBKWirelessCommProxy::ReadPortConfig()\n");
	// Read base class configuration
	RawCommProxy::ReadPortConfig(portNode);

    try
    {
        // Read start/stop bytes requried for communicating with the BBK Wireless device
        m_startByte = atoh(portNode->getChild( "Setup/StartByte")->getValue().c_str());
        Log( LOG_DEV_DATA, "Using startByte '%X'\n", m_startByte);
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tBepException in BBKWirelessProxy::ReadPortConfig StartByte: %s, Using 0x66\n", err.GetReason());
        m_startByte = 0x66;
    }

    try
    {
        // Read start/stop bytes requried for communicating with the BBK Wireless device
        m_stopByte = atoh(portNode->getChild( "Setup/StopByte")->getValue().c_str());
        Log( LOG_DEV_DATA, "Using StopByte '%X'\n", m_startByte);
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tBepException in BBKWirelessProxy::ReadPortConfig StopByte: %s, Using 0x88\n", err.GetReason());
        m_stopByte = 0x88;
    }

	Log( LOG_FN_ENTRY, "Exit BBKWirelessCommProxy::ReadPortConfig()\n");
}

/**
 * Addes the Start and Stop bytesjust before writing data to the driver
 * 
 * @param buff   The data buffer to write
 * @param bytes  The number of bytes to write from buff
 * 
 * @return The number of bytes written, or -1 if an error occurred (errno is set).
 */
int BBKWirelessProxy::Write( const void* buff, size_t bytes)
{
    uint8_t* pUint8_t = (uint8_t*)buff;
    SerialString_t newMessage;
    newMessage.push_back(m_startByte);
    newMessage.push_back(m_startByte);
    for (UINT8 ii = 0; ii < bytes; ii++)
    {
        newMessage.push_back(pUint8_t[ii]);
    }
    newMessage.push_back(m_stopByte);
    newMessage.push_back(m_stopByte);
    bytes = bytes + 4;  // Account for new Start/Stop Bytes
    void* newBuff = &newMessage;

    PrintSerialString( LOG_DEV_DATA, "\tBBKWireless TX String", newMessage);

    return(RawCommProxy::Write(newBuff, bytes));
}
