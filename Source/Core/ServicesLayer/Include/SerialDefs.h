//******************************************************************************
// Description:
//	Data type definitions for serial server
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
//     $Log: /Core/ServicesLayer/Include/SerialDefs.h $
// 
// 3     6/24/04 12:52a Cward
// Removed pound if 0 block. Updated header block comments to the new
// standard.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:22p Khykin
// Initial checkin.
//
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
//
// 23    4/04/03 11:15a Bmeinke
// Added a SerialProtocol constructure which takes a pointer to a termios
// structure
//
// 22    3/18/03 6:48p Bmeinke
// Added overloaded assignment operator to allow copying of termios
// structures
// Added new methods GetBitsPerByte() and GetByteTime()
//
// 21    2/21/03 7:29p Bmeinke
// Initialize the c_cc[VMIN] and c_cc[VTIME] members of termios in
// InitProtocol()
//
// 20    2/11/03 1:48p Bmeinke
// Added an InitProtocol() function to initialize a SerialProtocol_t
// structure with common settings
//
// 19    2/06/03 10:21a Bmeinke
// Removed some unused define statements
// Changed SerialProtocol_t to be based on POSIX termios struct
//
// 18    8/22/02 12:30p Bmeinke
// Added define for the tag of the XML node whcih contains the Port
// Protocol nodes
// Added define for the tag of the XML node whcih contains the Com Log
// config nodes
// Cleaned up compiler warnings in SerialProtocol class
//
// 17    6/05/02 11:19p Bmeinke
// Removed #if 0/#endif blocks
//
// 16    5/29/02 5:45p Bmeinke
// Added new (unused as of yet) class LogicalPortConfigParser
//
// 15    5/22/02 12:13p Bmeinke
// Added a transmitted data reflection flag to SerialProtocol_t and
// Get/Set methods to SerialProtocol
//
// 14    5/14/02 3:56p Bmeinke
// Added equality comparison operators to compare with SerialProtocol and
// SerialProtocol_t objects
// Added GetProtocol() method to get the raw serial protocol settings
// (returned in a SerialProtocol_t object)
//
// 13    3/22/02 1:44p Bmeinke
// Added defines to suppor new MqueueCommPort port type
//
// 12    2/01/02 9:51p Brian.meinke
// Added CFG_CARD_TYPE_ATTR_TAG XML node tag definition
// Added SerialString_t data type
// Added support to SerialProtocol Object for FIFO size
//
// 11    12/05/01 12:57p Brian.meinke
// Moved LogPortPulseCodes_t pulse code definitions to LogPortDrv.h
//
// 10    11/30/01 3:05p Brian.meinke
// Moved LOG_PORT_NAME_ROOT from LogPortDrv.h to SerialDefs.h
// Added enum LogPortPulseCodes_t to define the possible private pulse
// codes used by the logical port driver
// Added SerialProtocol_t structure in order to pass protocol data between
// the logical port clients and the logical port driver (GCC did not like
// us passing a class object in messages)
// SerialProtocol now inherits privately from SerialProtocol_t
// Changed SerialProtocol constructors due to new inheritance from
// SerialProtocol_t
// Added overloaded SerialProtocol::operator=(SerialProtocol_t&)
// SerialProtocol::Initialize() no expects the XML node passed in to be
// the <Setup> node for the port protocol (removes dependency of
// SerialDefs.h on ResManagerBase.h)
// Removed data members from SerialProtocol dure to inheritance from
// SerialProtocol_t
//
// 9     11/28/01 10:56p Brian.meinke
// Added XML node tag define for fifo size
// Added XML node tag defines for port protocol options
// Fixed bug in SerialProtocol::Initialize() not getting proper parent
// node to read port protocol from
//
// 8     11/26/01 6:55p Brian.meinke
// Added define for deafult serial server process name (name_attach name)
// Added define for root of logical port's pathname (/dev/LogPort)
// Added defines for XML node tags for reading config file
//
// 7     11/21/01 8:45p Brian.meinke
// Added javadoc comments to default SerialProtocol constructor
// Added SerialProtocol copy constructor
// Added SerialProtocol assignment operator
//
// 6     11/16/01 3:11p Brian.meinke
// Added defines for XML configuration tags
//
// 5     11/14/01 11:23p Brian.meinke
// Added SER_IRQ_PULSE_CODE to define the pulse code sent for interrupt
// events
//
// 4     11/14/01 2:21a Brian.meinke
// Added defines for XML node tags used for Card and Port configuration
// Added Interrupt_t enum (used with Mizar configuration)
// Added SerialProtocol::Initialize() to read protocol settings from an
// XML node
// Moved CardInfo class (now called CommCard) to CommCard.h
// Moved LogicalPort class to LogicalPort.h
//
// 3     11/09/01 5:43p Brian.meinke
// First executable version
//
// 2     11/08/01 9:14p Brian.meinke
// Changed UINT32/INT32 to uint32_t/int32_t
// Moved QueRecord_t structure to bottom of file
//
// 1     11/07/01 3:10p Brian.meinke
// Data types and definitions for the serial comm server
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _SERIAL_DEFS_H_INCLUDED
#define _SERIAL_DEFS_H_INCLUDED

// Forward Class Definitions
//class SerialProtocol;
//class LogicalPort;

//Include files
#include <inttypes.h>		// uint*_t
//#include <mqueue.h>			//
#include <termios.h>		// POSIX terminal control
#include <string>
//#include <map>

//#include "BepDefs.h"
#include "XmlNode.h"
//#include "ILogger.h"
//#include "tchar.h"

//using namespace std;

//******************************************************************************
//******************************************************************************

/**
 * Max size for data in a que record
 */
#define	MAX_QUE_REC_LEN			2048

/**
 * Default name to register serial server with
 */
#define SER_SRVR_NAME			"mgr/SerialServer"

/**
 * Root path for serial devices to adopt paths under
 */
#define	SERIAL_DEV_ROOT			"/dev/Serial/"

/**
 * Root path for serial devices to adopt paths under
 */
#define	LOG_PORT_DEV_ROOT		"/dev/LogicalPorts/"

/**
 * Prfeix of name to use when registering our process
 */
#define LOG_PORT_NAME_ROOT		"logport/"

/**
 * XML node tag for configuration node containing serial card setup information
 * (address, irq, etc)
 */
#define CFG_BRD_CFG_TAG			XML_T("BoardConfig")

/**
 * XML Node tag for configuration node containing logical to physical port
 * mapping information
 */
#define CFG_LOG_PORT_MAP_TAG	XML_T("LogicalPortMap")

/**
 * XML node tag for configuration node containing logical port configuration
 */
#define CFG_LOG_PORT_TAG		XML_T("LogicalPorts")

/**
 * XML node tag for configuration node containing logical port protocol
 * configuration (baud, etc)
 */
#define CFG_LOG_PORT_PROT_TAG	XML_T( "Protocol")

/**
 * XML node tag for configuration node specifying the FIFO size to use
 */
#define CFG_LOG_FIFO_TAG		XML_T("FifoSize")

/**
 * XML node tag for configuration node specifying the baud rate
 */
#define CFG_BAUD_TAG			XML_T("Baud")

/**
 * XML node tag for configuration node specifying the port parity
 */
#define CFG_PARITY_TAG			XML_T("Parity")

/**
 * XML node tag for configuration node specifying the EVEN parity
 */
#define CFG_PAR_EVEN_VAL_TAG	XML_T("Even")

/**
 * XML node tag for configuration node specifying ODD parity
 */
#define CFG_PAR_ODD_VAL_TAG		XML_T("Odd")

/**
 * XML node tag for configuration node specifying the number of data bits per byte
 */
#define CFG_DATA_BITS_TAG		XML_T("DataBits")

/**
 * XML node tag for configuration node specifying the number of stop bits per byte
 */
#define CFG_STOP_BITS_TAG		XML_T("StopBits")

/**
 * XML node tag for configuration node specifying the interrupt mode to use
 */
#define CFG_INTERRUPTS_TAG		XML_T("Interrupts")

/**
 * XML node tag for configuration node specifying transmit interrupts
 */
#define CFG_INT_TX_VAL_TAG		XML_T("Tx")

/**
 * XML node tag for configuration node specifying transmit and receive interrupts
 */
#define CFG_INT_TXRX_VAL_TAG	XML_T("TxRx")

/**
 * XML node tag for configuration node specifying transmit and receive interrupts
 */
#define CFG_INT_RXTX_VAL_TAG	XML_T("RxTx")

/**
 * XML node tag for configuration node specifying receive interrupts
 */
#define CFG_INT_RX_VAL_TAG		XML_T("Rx")

/**
 * XML node tag for configuration node specifying the inter-byte gap
 */
#define CFG_BYTE_GAP_TAG		XML_T("ByteGap")

/**
 * XML node tag for configuration node specifying the inter-message gap
 */
#define CFG_MSSG_GAP_TAG		XML_T("MssgGap")

/**
 * XML node tag for configuration node specifying whether we will have
 * transmitted bytes reflected back to us
 */
#define CFG_REFLECTION_TAG		XML_T("Reflection")

/**
 * XML node tag for configuration node serial comm logger attributes
 */
#define CFG_COM_LOG_TAG			XML_T("CommLog")

/**
 * XML attribute node tag used to specify which serial card a logical port is
 * mapped to
 */
#define CFG_BOARD_ATTR_TAG		XML_T("boardName")

/**
 * XML attribute node tag used to specify which physical serial port a logical
 * port is mapped to
 */
#define CFG_PHYS_PORT_ATTR_TAG	XML_T("physicalPort")

/**
 * XML attribute node tag used to specify our logical port number
 */
#define CFG_LOG_PORT_ID_TAG		XML_T("logicalPort")

/**
 * XML attribute node tag used to specify whether or not a
 * logical port is enabled
 */
#define CFG_PORT_EN_ATTR_TAG	XML_T("enabled")

/**
 * XML attribute tag use to specify the name of the logical
 * port driver executable to use for a logical port
 */
#define CFG_PORT_DRV_ATTR_TAG	XML_T("driver")

/**
 * XML attribute tag used to specify a serial card type
 */
#define CFG_CARD_TYPE_ATTR_TAG	XML_T("type")

/**
 * XML attribute tag used to specify comm board base address
 */
#define CFG_SER_BASE_ADDR_TAG	XML_T("baseAddress")

/**
 * XML attribute tag used to specify comm board serial interrupt vector
 */
#define CFG_SER_COM_IRQ_TAG		XML_T("comIrq")

/**
 * XML attribute tag used to specify comm board timer interrupt vector
 */
#define CFG_SER_TIMER_IRQ_TAG	XML_T("timerIrq")


//******************************************************************************
//******************************************************************************
		

/**
 * Data type used to hold serial dat strings
 */
typedef basic_string< uint8_t>	SerialString_t;

/**
 * Data bits per byte
 * 	BITS8
 * 	BITS7
 * 	BITS6
 * 	BITS5
 */
typedef enum
{
	/**
	 * 8 Data bits
	 */
	BITS8 		= 0xC0,
	/**
	 * 7 Data bits
	 */
	BITS7 		= 0x40,
	/**
	 * 6 Data bits
	 */
	BITS6 		= 0x80,
	/**
	 * 5 Data bits
	 */
	BITS5 		= 0x00,
	/**
	 * ??? Data bits
	 */
	BITSJUNK	= 0x7F
} DataBits_t;

/**
 * Character parity
 * 	NO_PARITY
 * 	ODD_PARITY
 * 	EVEN_PARITY
 * 	JUNK_PARITY
 */
typedef enum
{
	/**
	 * No parity
	 */
	NO_PARITY = 0x00,
	/**
	 * Odd parity
	 */
	ODD_PARITY = 0x01,
	/**
	 * Even parity
	 */
	EVEN_PARITY = 0x03,
	/**
	 * ??? parity
	 */
	JUNK_PARITY = 0x7F
} Parity_t;

/**
 * Framing stop bits
 * 	STOP1
 * 	STOP1_5
 * 	STOP2
 * 	STOPJUNK
 */
typedef enum
{
	/**
	 * 1 Stop bit
	 */
	STOP1		= 0x04,
	/**
	 * 1.5 Stop bit
	 */
	STOP1_5		= 0x08,
	/**
	 * 2 Stop bits
	 */
	STOP2		= 0x0C,
	/**
	 * ??? Stop bits
	 */
	STOPJUNK	= 0x7F
} StopBits_t;

/**
 * Interrupt ooperation mode
 * 	NO_INTERR
 * 	X_R_INTERR
 * 	R_INTERR
 * 	X_INTERR
 */
typedef enum
{
	/**
	 * No interrupts (polled mode)
	 */
	NO_INTERR = 0x00,        // serial chip bits
	/**
	 * Transmit and receive interrupts
	 */
	X_R_INTERR = 0x12,
	/**
	 * Receive interrupts only
	 */
	R_INTERR = 0x10,
	/**
	 * Transmit interrupts only
	 */
	X_INTERR = 0x02
} Interrupt_t;

/**
 * Possible types of serial cards
 * 	SERIAL_TYPE_NONE
 * 	MIZAR_QUAD_SERIAL
 * 	IBM_COM
 * 	IBM_LPT
 * 	INTERNAL_PROCESS
 */
typedef enum
{
	/**
	 * Undefined card type
	 */
	SERIAL_TYPE_NONE,
	/**
	 * Mizar 8300 serial card
	 */
	MIZAR_QUAD_SERIAL,
	/**
	 * IBM com port (16550 UART)
	 */
	IBM_COM,
	/**
	 * IBM Parallel port
	 */
	IBM_LPT,
	/**
	 * Pseudo-port used to communicate with a process simulating a com port
	 */
	INTERNAL_PROCESS,
	/**
	 * Pseudo-port used to communicate with message queue
	 */
	MQUEUE_PORT

} SerialCard_t;

/**
 * Logical port status definitions
 * 	PORT_RESET
 * 	PORT_READY
 * 	PORT_BUSY
 * 	PORT_BUSY_BLOCKED
 * 	PORT_BLOCKED
 */
typedef enum
{
	PORT_RESET = 0,
	PORT_READY = 1,
	PORT_BUSY = 2,
	PORT_BUSY_BLOCKED = 3,
	PORT_BLOCKED = 4
} PortStatus_t;

// DO NOT CHANGE THE MIZAR DEFINES BECAUSE THEY ARE SELECTED FOR AGREEMENT
// WITH DEFINES IN THE MIZAR BOARD LIBRARY FUNCTIONS

/**
 * Physical port layout for mizar 8300 quad serial card
 * 	MIZAR_SP2A
 * 	MIZAR_SP2B
 * 	MIZAR_SP1A
 * 	MIZAR_SP1B
 * 	MIZAR_TIMER
 */
typedef enum
{
	MIZAR_SP2A = 1,
	MIZAR_SP2B = 2,
	MIZAR_SP1A = 3,
	MIZAR_SP1B = 4,
	MIZAR_TIMER = 5
} MizarPorts_t;

/**
 * Physical layout for IBM serial com ports
 * 	IBM_COM1
 * 	IBM_COM2
 * 	IBM_COM3
 * 	IBM_COM4
 * 	IBM_COM5
 * 	IBM_COM6
 * 	IBM_COM7
 * 	IBM_COM8
 */
typedef enum
{
	IBM_COM1 = 1,
	IBM_COM2 = 2,
	IBM_COM3 = 3,
	IBM_COM4 = 4,
	IBM_COM5 = 5,
	IBM_COM6 = 6,
	IBM_COM7 = 7,
	IBM_COM8 = 8,
} IbmComPort_t;

/**
 * Physical layout for IBM parallel com ports
 * 	IBM_LPT1
 * 	IBM_LPT2
 * 	IBM_LPT3
 * 	IBM_LPT4
 */
typedef enum
{
	IBM_LPT1 = 1,
	IBM_LPT2 = 2,
	IBM_LPT3 = 3,
	IBM_LPT4 = 4,
} IpmLptPort_t;

//******************************************************************************
//******************************************************************************

typedef struct
{
	/**
	 * Standard POSIX terminal attributes used by the QNX terminal control
	 * functions
	 */
	struct termios	m_tio;

	/**
	 * Inter byte gap for transmitting in milli-seconds
	 */
	uint32_t		m_byteGap;

	/**
	 * Inter-message gap for transmitting in milli-seconds
	 */
	uint32_t		m_msgGap;

	/**
	 * Transmit reflection flag
	 */
	bool			m_reflection;

} SerialProtocol_t;

/**
 * Initializes the SerialProtocol_t structure to common BEP settings:
 *  No flow control
 *  Local mode, receiver enabled
 *  Raw, no echo, no signals
 *  Ignore parity and break conditions (on input)
 *  No output processing
 *
 * @param protocol SerialProtocol_t structure to initialize
 */
inline void InitProtocol( SerialProtocol_t &protocol)
{
	// No hardware flow control
	// Enable local mode (no modem) and enable the receiver
	protocol.m_tio.c_cflag = (CLOCAL | CREAD);
	
	// Set local options (raw, no echo)
	protocol.m_tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	
	// Set input options (no software flow, ignore parity errors)
	protocol.m_tio.c_iflag = (IGNPAR | IGNBRK);
	
	// Do not perform output processing
	protocol.m_tio.c_oflag = 0;// &= ~(OPOST);

	protocol.m_tio.c_cc[VINTR] = protocol.m_tio.c_cc[VQUIT] = protocol.m_tio.c_cc[VSUSP] = 0;
	protocol.m_tio.c_cc[ VMIN] = 1;
	protocol.m_tio.c_cc[VTIME] = 0;
}



//******************************************************************************
//******************************************************************************


#endif	// _SERIAL_DEFS_H_INCLUDED

