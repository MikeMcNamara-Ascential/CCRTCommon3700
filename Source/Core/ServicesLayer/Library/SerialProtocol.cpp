//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Library/SerialProtocol.cpp $
// $Author: Cward $
// $Date: 4/13/06 3:45a $
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/SerialProtocol.cpp $
// 
// 2     4/13/06 3:45a Cward
// Typecast the parameters being passed to SetBaud(), SetByteGap() and
// SetMsgGap()
//
// 2     2/15/06 3:56p Bmeinke
// Typecast the parameters being passed to SetBaud(), SetByteGap() and
// SetMsgGap()
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************

#include "SerialProtocol.h"



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
SerialProtocol::SerialProtocol( speed_t baud/*=9600*/,
                                Parity_t parity/*=NO_PARITY*/,
                                DataBits_t dataBits/*=BITS8*/,
                                StopBits_t stopBits/*=STOP1*/,
                                uint32_t byteGap/*=0*/,
                                uint32_t msgGap/*=0*/,
                                bool reflection/*=false*/)
{
    // Set standard options
    InitProtocol( *this);

    SetBaud( baud);
    SetParity( parity);
    SetDataBits( dataBits);
    SetStopBits( stopBits);
    SetByteGap( byteGap);
    SetMsgGap( msgGap);
    SetReflection( reflection);
}

/**
 * Copy constructor
 *
 * @param copy   SerialProtocol object to be copied
 */
SerialProtocol::SerialProtocol( const SerialProtocol &copy)
{
    *this = copy;
}

/**
 * Copy constructor
 *
 * @param copy   SerialProtocol object to be copied
 */
SerialProtocol::SerialProtocol( const SerialProtocol_t &copy)
{
    *this = copy;
}

/**
 * Copy constructor
 *
 * @param copy   termios structure to be copied
 */
SerialProtocol::SerialProtocol( const struct termios *copy)
{
    *this = *copy;
}

/**
 * Destructor
 */
SerialProtocol::~SerialProtocol()
{
}

/**
 * Assignment operator
 *
 * @param copy   SerialProtocol object to be copied
 * @return *this
 */
SerialProtocol& SerialProtocol::operator=( const SerialProtocol &copy)
{
    memmove( &m_tio, &copy.m_tio, sizeof(m_tio));
    m_byteGap = copy.m_byteGap;
    m_msgGap = copy.m_msgGap;
    m_reflection = copy.m_reflection;
    return( *this);
}

/**
 * Assignment operator
 *
 * @param copy   SerialProtocol object to be copied
 * @return *this
 */
SerialProtocol& SerialProtocol::operator=( const SerialProtocol_t &copy)
{
    memmove( &m_tio, &copy.m_tio, sizeof(m_tio));
    m_byteGap = copy.m_byteGap;
    m_msgGap = copy.m_msgGap;
    m_reflection = copy.m_reflection;
    return( *this);
}

/**
 * Assignment operator
 *
 * @param copy   termios structure object to be copied
 * @return *this
 */
SerialProtocol& SerialProtocol::operator=( const struct termios &copy)
{
    memmove( &m_tio, &copy, sizeof(m_tio));
    m_byteGap = 0;
    m_msgGap = 0;
    m_reflection = false;
    return( *this);
}

/**
 * Comparison operator
 *
 * @param rhs    Object to be compared against
 * @return true if *this == rhs, false otherwise
 */
bool SerialProtocol::operator==( const SerialProtocol &rhs) const
{
    return( (m_tio.c_iflag == rhs.m_tio.c_iflag) &&
            (m_tio.c_oflag == rhs.m_tio.c_oflag) &&
            (m_tio.c_cflag == rhs.m_tio.c_cflag) &&
            (m_tio.c_lflag == rhs.m_tio.c_lflag) &&
            (m_tio.c_ispeed == rhs.m_tio.c_ispeed) &&
            (m_tio.c_ospeed == rhs.m_tio.c_ospeed) &&
            (m_byteGap == rhs.m_byteGap) &&
            (m_msgGap == rhs.m_msgGap) &&
            (m_reflection == rhs.m_reflection));
}

/**
 * Inequality operator
 *
 * @param rhs    Object to be compared against
 * @return true if *this != rhs, false otherwise
 */
bool SerialProtocol::operator==( const SerialProtocol_t &rhs) const
{
    return( (m_tio.c_iflag == rhs.m_tio.c_iflag) &&
            (m_tio.c_oflag == rhs.m_tio.c_oflag) &&
            (m_tio.c_cflag == rhs.m_tio.c_cflag) &&
            (m_tio.c_lflag == rhs.m_tio.c_lflag) &&
            (m_tio.c_ispeed == rhs.m_tio.c_ispeed) &&
            (m_tio.c_ospeed == rhs.m_tio.c_ospeed) &&
            (m_byteGap == rhs.m_byteGap) &&
            (m_msgGap == rhs.m_msgGap) &&
            (m_reflection == rhs.m_reflection));
}

/**
 * Inequality operator
 *
 * @param rhs    Object to be compared against
 * @return true if *this != rhs, false otherwise
 */
bool SerialProtocol::operator!=( const SerialProtocol &rhs) const
{
    return( !(*this == rhs));
}

/**
 * Inequality operator
 *
 * @param rhs    Object to be compared against
 * @return true if *this == rhs, false otherwise
 */
bool SerialProtocol::operator!=( const SerialProtocol_t &rhs) const
{
    return( !(*this == rhs));
}

/**
 * Returns the transmit baud rate
 *
 * @return Transmit baud rate
 */
speed_t SerialProtocol::GetBaud() const
{
    return( GetTxBaud());
}

/**
 * Returns the transmit baud rate
 *
 * @return Transmit baud rate
 */
speed_t SerialProtocol::GetTxBaud() const
{
    return( cfgetospeed( &m_tio));
}

/**
 * Returns the input baud rate
 *
 * @return The input baud rate
 */
speed_t SerialProtocol::GetRxBaud() const
{
    return( cfgetispeed( &m_tio));
}

/**
 * Returns the character parity
 *
 * @return The character parity
 */
Parity_t SerialProtocol::GetParity() const
{
    switch( m_tio.c_cflag & (PARENB | PARODD))
    {
    case PARENB | PARODD:	return( ODD_PARITY);
    case PARENB:			return( EVEN_PARITY);
    default:				return( NO_PARITY);
    }
}

/**
 * Returns the data bits per byte
 *
 * @return The data bits per byte
 */
DataBits_t SerialProtocol::GetDataBits() const
{
    switch( m_tio.c_cflag & CSIZE)
    {
    case CS8:	return( BITS8);
    case CS7:	return( BITS7);
    case CS6:	return( BITS6);
    case CS5:	return( BITS5);
    }

    return( BITS8);
}

/**
 * Returns the stop bits per byte
 *
 * @return The stop bits per byte
 */
StopBits_t SerialProtocol::GetStopBits() const
{
    if( !(m_tio.c_cflag & CSTOPB))		return( STOP1);
    else if( GetDataBits() == BITS5)	return( STOP1_5);
    else								return( STOP2);
}

/**
 * Returns the inter-byte gap (in micro seconds)
 *
 * @return The inter-byte gap (in micro seconds)
 */
uint32_t SerialProtocol::GetByteGap() const
{
    return( m_byteGap);
}

/**
 * Returns the inter-message gap (in micro seconds)
 *
 * @return The inter-message gap (in micro seconds)
 */
uint32_t SerialProtocol::GetMsgGap() const
{
    return( m_msgGap);
}

/**
 * Sets the transmit reflection flag. If the transmit reflection flag
 * is true, each byte we transmit will be reflected back to us.
 *
 * @param hasReflection
 *               New value of the transmit reflection flag
 */
bool SerialProtocol::GetReflection() const
{
    return( m_reflection);
}

/**
 * Sets new baud rates for the serial protocol
 *
 * @param txBaud Transmit baud rate
 * @param rxBaud Receive bayd rate (defaults to txBaud if not specified)
 */
void SerialProtocol::SetBaud( speed_t txBaud, speed_t rxBaud/*=0*/)
{
    if( rxBaud == 0)	rxBaud = txBaud;
    SetTxBaud(txBaud);
    SetRxBaud(rxBaud);
}

/**
 * Sets a new transmit baud rate
 *
 * @param txBaud Output baud rate
 */
void SerialProtocol::SetTxBaud( speed_t txBaud)
{
    cfsetospeed( &m_tio, txBaud);
}

/**
 * Sets a new receive baud rate
 *
 * @param txBaud Input baud rate
 */
void SerialProtocol::SetRxBaud( speed_t rxBaud)
{
    cfsetispeed( &m_tio, rxBaud);
}

/**
 * Sets the character parity
 *
 * @param parity New character parity to use
 *
 * @see Parity_t
 */
void SerialProtocol::SetParity( Parity_t parity)
{
    switch( parity)
    {
    case ODD_PARITY:	m_tio.c_cflag |= PARENB| PARODD;						break;
    case EVEN_PARITY:	m_tio.c_cflag = (m_tio.c_cflag | PARENB) & ~(PARODD);	break;
    default:
    case JUNK_PARITY:
    case NO_PARITY:		m_tio.c_cflag &= ~(PARENB);								break;
    }
}

/**
 * Sets the number of data bits per byte
 *
 * @param dataBits Number of data bits per byte
 *
 * @see DataBits_t
 */
void SerialProtocol::SetDataBits( DataBits_t dataBits)
{
    switch( dataBits)
    {
    case BITS7:     m_tio.c_cflag = (m_tio.c_cflag & ~(CSIZE)) | CS7;	break;
    case BITS6:		m_tio.c_cflag = (m_tio.c_cflag & ~(CSIZE)) | CS6;	break;
    case BITS5:		m_tio.c_cflag = (m_tio.c_cflag & ~(CSIZE)) | CS5;	break;
    default:
    case BITSJUNK:
    case BITS8:		m_tio.c_cflag = (m_tio.c_cflag & ~(CSIZE)) | CS8;	break;
    }
}

/**
 * Sets the number of stop bite per byte
 *
 * @param stopBits The number of stop bite per byte
 *
 * @see StopBits_t
 */
void SerialProtocol::SetStopBits( StopBits_t stopBits)
{
    switch( stopBits)
    {
    case STOP1_5:		m_tio.c_cflag |= CSTOPB;			break;
    case STOP2:			m_tio.c_cflag |= CSTOPB;			break;
    default:
    case STOPJUNK:
    case STOP1:			m_tio.c_cflag &= ~(CSTOPB);		break;
    }
}

/**
 * Sets the inter-byte gap for transmitting data
 *
 * @param byteGap New inter-byte gap to use when transmitting
 */
void SerialProtocol::SetByteGap( uint32_t byteGap)
{
    m_byteGap = byteGap;
}

/**
 * Sets the inter-message gap (milli-seconds) for transmitting data
 *
 * @param mssgGap New inter-message gap (microseconds) to use when transmitting
 */
void SerialProtocol::SetMsgGap( uint32_t mssgGap)
{
    m_msgGap = mssgGap;
}

/**
 * Sets the transmit reflection flag. If the transmit reflection flag
 * is true, each byte we transmit will be reflected back to us.
 *
 * @param hasReflection
 *               New value of the transmit reflection flag
 */
void SerialProtocol::SetReflection( bool hasReflection)
{
    m_reflection = hasReflection;
}

/**
 * Initialize the protocol settings from an XML node (typically
 * from a configuration file)
 *
 * @param cfgNode XML node to read the protocol settings from
 */
void SerialProtocol::Initialize( const XmlNode *cfgNode)
{
    const XmlNodeMap &cfgNodes = cfgNode->getChildren();

    ReadBaudXml( cfgNodes);
    ReadParityXml(  cfgNodes);
    ReadDataBitsXml( cfgNodes);
    ReadStopBitsXml( cfgNodes);
    ReadByteGapXml( cfgNodes);
    ReadMsgGapXml( cfgNodes);
    ReadReflectionXml( cfgNodes);
}

/**
 * Get the raw protocol settings
 *
 * @param copy   SerialProtocol_t structure to copy the protocol settings into
 */
void SerialProtocol::GetProtocol( SerialProtocol_t &copy) const
{
    copy = *this;
}


/**
 * Converts a BEP style port protocol to a POSIX termios struct
 *
 * @param tio    termios structure to fill in
 * @param bep    BEP format protocol object to read settings from
 */
void SerialProtocol::BepToPosix( struct termios *tio, const SerialProtocol &bep)
{
    // No hardware flow control
    tio->c_cflag &= ~(IHFLOW | OHFLOW);
    // Enable local mode (no modem) and enable the receiver
    tio->c_cflag |= (CLOCAL | CREAD);
    // Set local options (raw, no echo)
    tio->c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // Set input options (no software flow, ignore parity errors)
    tio->c_iflag &= ~(IXON | IXOFF | IXANY);
    tio->c_iflag |= (IGNPAR);
    // Do not perform output processing
    tio->c_oflag &= ~(OPOST);

    tio->c_cflag = bep.m_tio.c_cflag;
    tio->c_iflag = bep.m_tio.c_iflag;
    tio->c_lflag = bep.m_tio.c_lflag;
    tio->c_oflag = bep.m_tio.c_oflag;
    tio->c_ospeed = bep.m_tio.c_ospeed;
    tio->c_ispeed = bep.m_tio.c_ispeed;
    memcpy(tio->c_cc, bep.m_tio.c_cc, sizeof(bep.m_tio.c_cc));
}

/**
 * Converts a BEP style port protocol to a POSIX termios struct
 *
 * @param tio    termios structure to fill in
 * @param bep    BEP format protocol object to read settings from
 */
void SerialProtocol::BepToPosix( struct termios *tio, const SerialProtocol_t &bep)
{
    // No hardware flow control
    tio->c_cflag &= ~(IHFLOW | OHFLOW);
    // Enable local mode (no modem) and enable the receiver
    tio->c_cflag |= (CLOCAL | CREAD);
    // Set local options (raw, no echo)
    tio->c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // Set input options (no software flow, ignore parity errors)
    tio->c_iflag &= ~(IXON | IXOFF | IXANY);
    tio->c_iflag |= (IGNPAR);
    // Do not perform output processing
    tio->c_oflag &= ~(OPOST);

    tio->c_cflag = bep.m_tio.c_cflag;
    tio->c_iflag = bep.m_tio.c_iflag;
    tio->c_lflag = bep.m_tio.c_lflag;
    tio->c_oflag = bep.m_tio.c_oflag;
    tio->c_ospeed = bep.m_tio.c_ospeed;
    tio->c_ispeed = bep.m_tio.c_ispeed;
    memcpy(tio->c_cc, bep.m_tio.c_cc, sizeof(bep.m_tio.c_cc));
}

/**
 * Converts a POSIX termios struct to a BEP style port protocol
 *
 * @param tio    termios structure to read settings from
 * @param bep    BEP format protocol object to fill in
 */
void SerialProtocol::PosixToBep( const struct termios *tio, SerialProtocol &bep)
{
    bep.m_tio.c_cflag = tio->c_cflag;
    bep.m_tio.c_iflag = tio->c_iflag;
    bep.m_tio.c_lflag = tio->c_lflag;
    bep.m_tio.c_oflag = tio->c_oflag;
    bep.m_tio.c_ospeed = tio->c_ospeed;
    bep.m_tio.c_ispeed = tio->c_ispeed ;
    memcpy(bep.m_tio.c_cc, tio->c_cc, sizeof(bep.m_tio.c_cc));
}

/**
 * Converts a POSIX termios struct to a BEP style port protocol
 *
 * @param tio    termios structure to read settings from
 * @param bep    BEP format protocol object to fill in
 */
void SerialProtocol::PosixToBep( const struct termios *tio, SerialProtocol_t &bep)
{
    bep.m_tio.c_cflag = tio->c_cflag;
    bep.m_tio.c_iflag = tio->c_iflag;
    bep.m_tio.c_lflag = tio->c_lflag;
    bep.m_tio.c_oflag = tio->c_oflag;
    bep.m_tio.c_ospeed = tio->c_ospeed;
    bep.m_tio.c_ispeed = tio->c_ispeed ;
    memcpy(bep.m_tio.c_cc, tio->c_cc, sizeof(bep.m_tio.c_cc));
}

/**
 * Calculates the number of bits in each transmitted byte
 *
 * @return The number of bits in each transmitted byte
 */
uint8_t SerialProtocol::GetBitsPerByte()
{
    uint8_t	bitsPerByte = 0;
    // Calculate the number of bits per transmitted byte
    switch( GetDataBits())
    {
    default:
    case BITS8:	bitsPerByte += 8;	break;
    case BITS7:	bitsPerByte += 7;	break;
    case BITS6:	bitsPerByte += 6;	break;
    case BITS5:	bitsPerByte += 5;	break;
    }
    switch( GetStopBits())
    {
    default:
    case STOP1:		bitsPerByte += 1;	break;
    case STOP1_5:
    case STOP2:		bitsPerByte += 2;	break;
    }
    switch( GetParity())
    {
    case ODD_PARITY:
    case EVEN_PARITY:	bitsPerByte += 1;	break;
    default:
    case NO_PARITY:		break;
    }

    return( bitsPerByte);
}

/**
 * Returns the number of nanoseconds required to tranmit on byte
 *
 * @return The number of nanoseconds required to tranmit on byte
 */
uint64_t SerialProtocol::GetByteTime()
{
    speed_t		baud = GetTxBaud();
    uint8_t		bitsPerByte = GetBitsPerByte();
    uint64_t	byteTime = 1;

    if( baud > 0)
    {
        // Caluculate the number of ms it should take for a reflection to come back
        byteTime = ((uint64_t)bitsPerByte * (uint64_t)1000000000LL) / (uint64_t)baud;
    }

    // If baud faster than 1000000*bits (not sure how this could happen)
    if( byteTime == 0)
    {
        // Default to 1 ns
        byteTime = 1;
    }

    return( byteTime);
}

/**
 * Reads the baud rate from a list of port protocol setup nodes
 *
 * @param cfgNodes Port protocol configuration nodes
 */
void SerialProtocol::ReadBaudXml( const XmlNodeMap& cfgNodes)
{
    try
    {
        const XmlNode *node = cfgNodes.getNode( CFG_BAUD_TAG);
        SetBaud( (speed_t)atof( node->getValue().c_str()));
    }
    catch( ...)
    {
    }
}

/**
 * Reads the parity setting from a list of port protocol setup nodes
 *
 * @param cfgNodes Port protocol configuration nodes
 */
void  SerialProtocol::ReadParityXml(  const XmlNodeMap& cfgNodes)
{
    try
    {
        const XmlNode *node = cfgNodes.getNode( CFG_PARITY_TAG);
        const XmlString &val = node->getValue();
        if( val == CFG_PAR_EVEN_VAL_TAG)		SetParity(EVEN_PARITY);
        else if( val == CFG_PAR_ODD_VAL_TAG)	SetParity(ODD_PARITY);
        else									SetParity(NO_PARITY);
    }
    catch( ...)
    {
    }
}

/**
 * Reads the number of data bits per byte from a list of port
 * protocol setup nodes
 *
 * @param cfgNodes Port protocol configuration nodes
 */
void  SerialProtocol::ReadDataBitsXml( const XmlNodeMap& cfgNodes)
{
    try
    {
        const XmlNode *node = cfgNodes.getNode( CFG_DATA_BITS_TAG);
        float val = atof( node->getValue().c_str());
        if( val == 8)		SetDataBits( BITS8);
        else if( val == 7)	SetDataBits( BITS7);
        else if( val == 6)	SetDataBits( BITS6);
        else if( val == 5)	SetDataBits( BITS5);
        else				SetDataBits( BITSJUNK);
    }
    catch( ...)
    {
    }
}

/**
 * Reads the number of stop bits per byte from a list of port
 * protocol setup nodes
 *
 * @param cfgNodes Port protocol configuration nodes
 */
void  SerialProtocol::ReadStopBitsXml( const XmlNodeMap& cfgNodes)
{
    try
    {
        const XmlNode *node = cfgNodes.getNode( CFG_STOP_BITS_TAG);
        uint32_t val = (uint32_t)(10 * atof( node->getValue().c_str()));
        if( val == 10)			SetStopBits( STOP1);
        else if( val == 15)		SetStopBits( STOP1_5);
        else if( val == 20)		SetStopBits( STOP2);
        else					SetStopBits( STOPJUNK);
    }
    catch( ...)
    {
    }
}

/**
 * Reads the inter-byte gap (ms) from a list of port protocol setup nodes
 *
 * @param cfgNodes Port protocol configuration nodes
 */
void  SerialProtocol::ReadByteGapXml( const XmlNodeMap& cfgNodes)
{
    try
    {
        const XmlNode *node = cfgNodes.getNode( CFG_BYTE_GAP_TAG);
        SetByteGap( (uint32_t)atof( node->getValue().c_str()));
    }
    catch( ...)
    {
    }
}

/**
 * Reads the inter-message gap (ms) from a list of port protocol setup nodes
 *
 * @param cfgNodes Port protocol configuration nodes
 */
void  SerialProtocol::ReadMsgGapXml( const XmlNodeMap& cfgNodes)
{
    try
    {
        const XmlNode *node = cfgNodes.getNode( CFG_MSSG_GAP_TAG);
        SetMsgGap( (uint32_t)atof( node->getValue().c_str()));
    }
    catch( ...)
    {
    }
}

/**
 * Reads the reflection property from a list of port protocol setup nodes.
 * Reflections are most commonly found in single wire communication
 * setups like ISO-K
 *
 * @param cfgNodes Port protocol configuration nodes
 */
void SerialProtocol::ReadReflectionXml( const XmlNodeMap& cfgNodes)
{
    try
    {
        const XmlNode *node = cfgNodes.getNode( CFG_REFLECTION_TAG);
        const XmlString &nodeVal = node->getValue();
        if( nodeVal == XML_T( "1"))			SetReflection( true);
        else if( nodeVal == XML_T( "0"))	SetReflection( false);
    }
    catch( ...)
    {
    }
}

