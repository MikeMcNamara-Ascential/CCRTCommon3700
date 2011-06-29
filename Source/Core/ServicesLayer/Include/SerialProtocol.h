//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Include/SerialProtocol.h $
// $Author: Rwiersem $
// $Date: 10/30/07 3:50p $
//
// Description:
//  Defines the Serial Protocol Class for the serial server
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/SerialProtocol.h $
// 
// 2     10/30/07 3:50p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************

#ifndef _SERIALPROTOCOL_H_INCLUDED
#define _SERIALPROTOCOL_H_INCLUDED

// Forward Class Definitions
//class SerialProtocol;
//class LogicalPort;

//Include files
//#include <inttypes.h>     // uint*_t
//#include <mqueue.h>           //
//#include <termios.h>      // POSIX terminal control
//#include <string>
//#include <map>

//#include "BepDefs.h"
//#include "XmlNode.h"
//#include "ILogger.h"
//#include "tchar.h"

//using namespace std;

#include "SerialDefs.h"

//******************************************************************************
//******************************************************************************


/**
 * Serial porotocol data type
 *
 * @author Brian Meinke
 * @version 1.0
 */
class SerialProtocol : public SerialProtocol_t
{
public:

    /**
     * Default constructor
     *
     * @param baud       Baud rate
     * @param parity     Parity
     * @param dataBits   Number of bata bits per byte
     * @param stopBits   Number of stop bits per byte
     * @param byteGap    Inter-byte gap
     * @param msgGap     Inter-message gap
     * @param reflection true if the transmitted data is reflected back to our receiver
     */
    SerialProtocol( speed_t baud=9600,
                    Parity_t parity=NO_PARITY,
                    DataBits_t dataBits=BITS8,
                    StopBits_t stopBits=STOP1,
                    uint32_t byteGap=0,
                    uint32_t msgGap=0,
                    bool reflection=false);
    
    /**
     * Copy constructor
     *
     * @param copy   SerialProtocol object to be copied
     */
    SerialProtocol( const SerialProtocol &copy);
    
    /**
     * Copy constructor
     *
     * @param copy   SerialProtocol object to be copied
     */
    SerialProtocol( const SerialProtocol_t &copy);
    
    /**
     * Copy constructor
     *
     * @param copy   termios structure to be copied
     */
    SerialProtocol( const struct termios *copy);

    /**
     * Destructor
     */
    ~SerialProtocol();

    /**
     * Assignment operator
     *
     * @param copy   SerialProtocol object to be copied
     * @return *this
     */
    SerialProtocol& operator=( const SerialProtocol &copy);

    /**
     * Assignment operator
     *
     * @param copy   SerialProtocol object to be copied
     * @return *this
     */
    SerialProtocol& operator=( const SerialProtocol_t &copy);

    /**
     * Assignment operator
     *
     * @param copy   termios structure object to be copied
     * @return *this
     */
    SerialProtocol& operator=( const struct termios &copy);

    /**
     * Comparison operator
     *
     * @param rhs    Object to be compared against
     * @return true if *this == rhs, false otherwise
     */
    bool operator==( const SerialProtocol &rhs) const;

    /**
     * Inequality operator
     *
     * @param rhs    Object to be compared against
     * @return true if *this != rhs, false otherwise
     */
    bool operator==( const SerialProtocol_t &rhs) const;

    /**
     * Inequality operator
     *
     * @param rhs    Object to be compared against
     * @return true if *this != rhs, false otherwise
     */
    bool operator!=( const SerialProtocol &rhs) const;

    /**
     * Inequality operator
     *
     * @param rhs    Object to be compared against
     * @return true if *this == rhs, false otherwise
     */
    bool operator!=( const SerialProtocol_t &rhs) const;

    /**
     * Returns the transmit baud rate
     *
     * @return Transmit baud rate
     */
    speed_t GetBaud() const;

    /**
     * Returns the transmit baud rate
     *
     * @return Transmit baud rate
     */
    speed_t GetTxBaud() const;

    /**
     * Returns the input baud rate
     *
     * @return The input baud rate
     */
    speed_t GetRxBaud() const;
    
    /**
     * Returns the character parity
     *
     * @return The character parity
     */
    Parity_t GetParity() const;


    /**
     * Returns the data bits per byte
     *
     * @return The data bits per byte
     */
    DataBits_t GetDataBits() const;

    /**
     * Returns the stop bits per byte
     *
     * @return The stop bits per byte
     */
    StopBits_t GetStopBits() const;

    /**
     * Returns the inter-byte gap (in micro seconds)
     *
     * @return The inter-byte gap (in micro seconds)
     */
    uint32_t GetByteGap() const;

    /**
     * Returns the inter-message gap (in micro seconds)
     *
     * @return The inter-message gap (in micro seconds)
     */
    uint32_t GetMsgGap() const;
    
    /**
     * Sets the transmit reflection flag. If the transmit reflection flag is 
     * true, each byte we transmit will be reflected back to us.
     */
    bool GetReflection() const;

    /**
     * Sets new baud rates for the serial protocol
     *
     * @param txBaud Transmit baud rate
     * @param rxBaud Receive bayd rate (defaults to txBaud if not specified)
     */
    void SetBaud( speed_t txBaud, speed_t rxBaud=0);

    /**
     * Sets a new transmit baud rate
     *
     * @param txBaud Output baud rate
     */
    void SetTxBaud( speed_t txBaud);
    
    /**
     * Sets a new receive baud rate.
     *
     * @param rxBaud Input baud rate.
     */
    void SetRxBaud( speed_t rxBaud);

    /**
     * Sets the character parity
     *
     * @param parity New character parity to use
     *
     * @see Parity_t
     */
    void SetParity( Parity_t parity);

    /**
     * Sets the number of data bits per byte
     *
     * @param dataBits Number of data bits per byte
     *
     * @see DataBits_t
     */
    void SetDataBits( DataBits_t dataBits);

    /**
     * Sets the number of stop bite per byte
     *
     * @param stopBits The number of stop bite per byte
     *
     * @see StopBits_t
     */
    void SetStopBits( StopBits_t stopBits);

    /**
     * Sets the inter-byte gap for transmitting data
     *
     * @param byteGap New inter-byte gap to use when transmitting
     */
    void SetByteGap( uint32_t byteGap);

    /**
     * Sets the inter-message gap (milli-seconds) for transmitting data
     *
     * @param mssgGap New inter-message gap (microseconds) to use when transmitting
     */
    void SetMsgGap( uint32_t mssgGap);

    /**
     * Sets the transmit reflection flag. If the transmit reflection flag
     * is true, each byte we transmit will be reflected back to us.
     *
     * @param hasReflection
     *               New value of the transmit reflection flag
     */
    void SetReflection( bool hasReflection);

    /**
     * Initialize the protocol settings from an XML node (typically
     * from a configuration file)
     *
     * @param cfgNode XML node to read the protocol settings from
     */
    void Initialize( const XmlNode *cfgNode);


    /**
     * Get the raw protocol settings
     *
     * @param copy   SerialProtocol_t structure to copy the protocol settings into
     */
    void GetProtocol( SerialProtocol_t &copy) const;

    /**
     * Converts a BEP style port protocol to a POSIX termios struct
     *
     * @param tio    termios structure to fill in
     * @param bep    BEP format protocol object to read settings from
     */
    static void BepToPosix( struct termios *tio, const SerialProtocol &bep);
    
    /**
     * Converts a BEP style port protocol to a POSIX termios struct
     *
     * @param tio    termios structure to fill in
     * @param bep    BEP format protocol object to read settings from
     */
    static void BepToPosix( struct termios *tio, const SerialProtocol_t &bep);

    /**
     * Converts a POSIX termios struct to a BEP style port protocol
     *
     * @param tio    termios structure to read settings from
     * @param bep    BEP format protocol object to fill in
     */
    static void PosixToBep( const struct termios *tio, SerialProtocol &bep);
    
    /**
     * Converts a POSIX termios struct to a BEP style port protocol
     *
     * @param tio    termios structure to read settings from
     * @param bep    BEP format protocol object to fill in
     */
    static void PosixToBep( const struct termios *tio, SerialProtocol_t &bep);

    /**
     * Calculates the number of bits in each transmitted byte
     *
     * @return The number of bits in each transmitted byte
     */
    uint8_t GetBitsPerByte();

    /**
     * Returns the number of nanoseconds required to tranmit on byte
     *
     * @return The number of nanoseconds required to tranmit on byte
     */
    uint64_t GetByteTime();

private:

    /**
     * Reads the baud rate from a list of port protocol setup nodes
     *
     * @param cfgNodes Port protocol configuration nodes
     */
    void ReadBaudXml( const XmlNodeMap& cfgNodes);

    /**
     * Reads the parity setting from a list of port protocol setup nodes
     *
     * @param cfgNodes Port protocol configuration nodes
     */
    void  ReadParityXml(  const XmlNodeMap& cfgNodes);

    /**
     * Reads the number of data bits per byte from a list of port
     * protocol setup nodes
     *
     * @param cfgNodes Port protocol configuration nodes
     */
    void  ReadDataBitsXml( const XmlNodeMap& cfgNodes);

    /**
     * Reads the number of stop bits per byte from a list of port
     * protocol setup nodes
     *
     * @param cfgNodes Port protocol configuration nodes
     */
    void  ReadStopBitsXml( const XmlNodeMap& cfgNodes);

    /**
     * Reads the inter-byte gap (ms) from a list of port protocol setup nodes
     *
     * @param cfgNodes Port protocol configuration nodes
     */
    void  ReadByteGapXml( const XmlNodeMap& cfgNodes);

    /**
     * Reads the inter-message gap (ms) from a list of port protocol setup nodes
     *
     * @param cfgNodes Port protocol configuration nodes
     */
    void  ReadMsgGapXml( const XmlNodeMap& cfgNodes);
    
    /**
     * Reads the reflection property from a list of port protocol setup nodes.
     * Reflections are most commonly found in single wire communication
     * setups like ISO-K
     *
     * @param cfgNodes Port protocol configuration nodes
     */
    void ReadReflectionXml( const XmlNodeMap& cfgNodes);

};

//******************************************************************************
//******************************************************************************


#endif  // _SERIAL_DEFS_H_INCLUDED

