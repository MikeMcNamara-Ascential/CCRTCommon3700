//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Include/LogPortMessage.h $
// $Author:: Cward                                                            $
//   $Date:: 2/15/06 7:50p                                                    $
//
// Description:
//	Definitions of custom logical port driver message types
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/LogPortMessage.h $
// 
// 2     2/15/06 7:50p Cward
// Added support for Registering Port Drivers, and Reconnecting Drivers
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
// 1     6/12/03 4:17p Khykin
// Initial checkin.
//
// 1     4/22/03 9:47a Khykin
// Kevins Working Checkin
//
// 15    3/18/03 6:52p Bmeinke
// Added message, defines and data types needed to perform 5-baud
// initialization
//
// 14    2/21/03 7:27p Bmeinke
// Added DCMD_LOGPORT_EST_COMMS devctl dcmd define
//
// 13    2/06/03 6:41p Derickso
// Chnaged initMssg from uint8_t[ 1] to uint8_t[ 128] in
// logPortEstablishComms_t message structure
//
// 12    2/06/03 10:28a Bmeinke
// Added more POSIX standard DCMD_* defines
// Removed unused message defintions
//
// 11    1/10/03 6:02p Bmeinke
// Added devctl() command and message for disabling the serial port
// transmitter for fast ISO initialization sequences
//
// 10    11/18/02 1:21p Bmeinke
// Added LOG_PORT_SET_REPLY_TIMEOUT devctl command ID
// Added logPortSetReplyTimeout message structure
//
// 9     10/24/02 8:06p Bmeinke
// Added the LOG_PORT_RESET_DRIVER devctl command
//
// 8     9/24/02 7:11p Bmeinke
// Added devctl messages to support per message protocol settings
//
// 7     8/22/02 12:14p Bmeinke
// Changed <inttypes.h> to <stdint.h>
// Added PORT_SET_NONE enum to the ProtocolFlags_t typedef
// Added structures for changing port protocol on a message-by-message
// basis
//
// 6     4/24/02 5:25a Bmeinke
// Added support for resetting client FIFOs via the LOG_PORT_RESET_CONN
// devctl message
//
// 5     4/15/02 1:49p Bmeinke
// Added ability to pass filter strings in subscription messages
// Moved definition of LogPortFilterByte_t data type to this file from
// LogPortFilterString.h
//
// 4     4/04/02 3:18a Bmeinke
// Added LogPortSubscriptionId_t typedef
// Replaced uint64_t with LogPortSubscriptionId_t where appropriate
//
// 3     2/01/02 9:47p Brian.meinke
// Added logPortNotifyMask_t enum to define the possible client
// notification masks
// Added modeMask and byteCount attributes to the subscription message
// stuct
//
// 2     11/30/01 3:01p Brian.meinke
// Added message identifiers and structures to handle subscription
// messages
//
// 1     11/29/01 3:36p Brian.meinke
// Logical port driver custom devctl message and data type definitions
//
//******************************************************************************

#ifndef LOG_PORT_MSSG_H_INCLUDED
#define LOG_PORT_MSSG_H_INCLUDED

#include <stdint.h>			// uint*_t
#include <devctl.h>			// __DIOT, __DION, etc defines
#include <sys/siginfo.h>	// struct sigevent

#include "SerialDefs.h"

/**
 * Logical port driver devctl() class
 */
#define		_CMD_IOCTL_LOGPORT	0x1000

/**
 * Base message identifier for custom logical port driver devctl messages
 * @since Nov 29, 2001
 */
#define		LOG_PORT_MSSG_BASE	0x00

/**
 * Message ID's for Logical Port Driver custom devctl messages
 * 	LOG_PORT_LOCK_PORT
 * 	LOG_PORT_UNLOCK_PORT
 * 	LOG_PORT_SET_PROTOCOL
 */
typedef enum
{
	/**
	 * Request an exclusiv on the port
	 */
	LOG_PORT_LOCK_PORT = LOG_PORT_MSSG_BASE,

	/**
	 * Unlock a previously locked port
	 */
	LOG_PORT_UNLOCK_PORT,

	/**
	 * Subscribe for logical port event notifications
	 */
	LOG_PORT_SUBSCRIBE,

	/**
	 * Unsubscribe for logical port event notifications
	 */
	LOG_PORT_UNSUBSCRIBE,

	/**
	 * Read the current port protocol
	 */
	LOG_PORT_GET_PROTOCOL,

	/**
	 * Change the port protocol
	 */
	LOG_PORT_SET_PROTOCOL,

	/**
	 * Reset a client's connection
	 */
	LOG_PORT_RESET_CONN,

	/**
	 * Reset the logical port driver
	 */
	LOG_PORT_RESET_DRIVER,

	/**
	 * Set a temporary reply timeout value (in nanoseconds)
	 */
	LOG_PORT_SET_REPLY_TIMEOUT,

	/**
	 * Perform a comm fast initialization sequence
	 */
	LOG_PORT_ESTABLISH_COMMS,

	/**
	 * Perform a comm slowfast initialization sequence
	 */
	LOG_PORT_LOW_SPEED_INIT,

	/**
	 * Reconnect to the underlying communication driver (devc, socket, etc)
	 */
	LOG_PORT_RECONNECT,

	/**
	 * Marker for derived classes to use for start of their custom devctl
	 * command id's
	 */
	LOG_PORT_DEVCTL_NEXT_AVAIL

} logPortMssgIds;

/**
 * Subscription notification modes. Used to filter when a client receives
 * notifications for subscribed-for transactions occur.
 */
typedef enum
{
	/**
	 * Never send notification (just a dummy place holder really)
	 */
	LOG_PORT_NOTIFY_NEVER 		= 0x00000000,

	/**
	 * Always send notification for subscribed events
	 */
	LOG_PORT_NOTIFY_ALWAYS		= 0x00000001,

	/**
	 * Only send notifications when client has port locked. Useful for receiving
	 * notifications only for expected port events
	 */
	LOG_PORT_NOTIFY_LOCKED		= 0x00000002,

} logPortNotifyMask_t;

/**
 * Flags used for setting port protocol via the Logical Port Driver's
 * custom LOG_PORT_SET_PROTOCOL devctl messages
 * 	PORT_SET_BAUD
 * 	PORT_SET_PARITY
 * 	PORT_SET_DATA_BITS
 * 	PORT_SET_STOP_BITS
 * 	PORT_SET_INTR
 * 	PORT_SET_BYTE_GAP
 * 	PORT_SET_MSSG_GAP
 *  PORT_SET_ALL
 */
typedef enum
{
	/**
	 * Flag indicating no protocol settings are valid
	 */
	PORT_SET_NONE		= 0x00,

	/**
	 * Flag indicating baud rate in set protocol message is valid
	 */
	PORT_SET_BAUD		= 0x01,

	/**
	 * Flag indicating parity in set protocol message is valid
	 */
	PORT_SET_PARITY		= 0x02,

	/**
	 * Flag indicating data bits in set protocol message is valid
	 */
	PORT_SET_DATA_BITS	= 0x04,

	/**
	 * Flag indicating stop bits in set protocol message is valid
	 */
	PORT_SET_STOP_BITS	= 0x08,

	/**
	 * Flag indicating interrupt mode in set protocol message is valid
	 */
	PORT_SET_INTR		= 0x10,

	/**
	 * Flag indicating inter-byte gap in set protocol message is valid
	 */
	PORT_SET_BYTE_GAP	= 0x20,

	/**
	 * Flag indicating inter-mssg gap in set protocol message is valid
	 */
	PORT_SET_MSSG_GAP	= 0x40,

	/**
	 * Flag indicating all data in set protocol message is valid
	 */
	PORT_SET_ALL		= 0xFF

} ProtocolFlags_t;

/**
 * Flags used to indicate which types of events a client would like to
 * subscribe for via the Logical Port Driver's custom LOG_PORT_SUBSCRIBE
 * devctl messages
 *  PORT_SUBSCRIBE_RX
 *  PORT_SUBSCRIBE_TX
 *  PORT_SUBSCRIBE_LOCK
 *  PORT_SUBSCRIBE_UNLOCK
 */
typedef enum
{
	/**
	 * Flag to indicate a client subscription for received data
	 */
	PORT_SUBSCRIBE_RX		= 0x00000001,

	/**
	 * Flag to indicate a client subscription for transmitted data
	 */
	PORT_SUBSCRIBE_TX		= 0x00000002,

	/**
	 * Flag to indicate a client subscription for port lock events
	 */
	PORT_SUBSCRIBE_LOCK		= 0x00000004,

	/**
	 * Flag to indicate a client subscription for port unlock events
	 */
	PORT_SUBSCRIBE_UNLOCK	= 0x00000008,

	/**
	 * Flag to indicate that the subscription message contains a
	 * filter string
	 */
	PORT_NOTIFY_FILTER		= 0x40000000,

	/**
	 * Flag to request notification if data is already in the port drivers FIFO
	 * (received and not yet read or waiting to be transmitted)
	 */
	PORT_NOTIFY_EXISTING	= 0x80000000,

	/**
	 * Flag to indicate a client subscription for all port events
	 */
	PORT_SUBSCRIBE_ALL		= 0x00FFFFFF

} PortSubscription_t;

/**
 * Data type used to send a port notification event to subscribed
 * client. Use the SIGEV_***_INIT macros to initialize the event
 *
 * @see sigevent
 */
typedef struct sigevent	NotifyEvent_t;

/**
 * Data type use top represent a single byte in a serial filter string
 */
typedef struct _LogPortFilterByte_t
{
	/**
	 * Filter byte value
	 */
	uint8_t		m_fbVal;

	/**
	 * Mask value to apply when comparing filter-byte value to
	 * literal values
	 */
	uint8_t		m_mask;

	/**
	 * Flag use to mark the filter-byte as a wildcard character
	 */
	bool		m_isWildCard;

} LogPortFilterByte_t;


/**
 * Data type used to represent a subscription ID for Logical Port data
 */
typedef uint64_t	LogPortSubscriptionId_t;

/**
 * Value used to indicate an invalid subscription handle
 */
#define LOG_PORT_INVALID_SUB	(LogPortSubscriptionId_t)-1

/**
 * Client message requesting exclusive lock on a logical port
 */
typedef struct _logPortLockPort_t
{
	/**
	 * Flags indicating blocking mode; possible values:
	 *  0: If unable to immediately lock the port, block until able to lock
	 *  O_NONBLOCK: if unable to immediately lock port, return immediately
	 */
	uint32_t	flags;

} logPortLockPort_t;

#define	LOG_PORT_FLTR_STRING_LEN	64
/**
 * Client message used to subscribe for logical port event notifications
 */
typedef struct _logPortSubscribe_t
{
	/**
	 * Bitwise flag indicating which types of logical port events a client wants
	 * to be notified about. Possible event types are given in the
	 * PortSubscription_t enumeration type
	 *
	 * @see PortSubscription_t
	 */
	uint32_t				flags;

	/**
	 * Mask bits to filter when a subscribed client wants to be notified of
	 * port events
	 *
	 * @see logPortNotifyMask_t
	 */
	uint32_t				modeMask;

	/**
	 * Value to indicate how many bytes should be buffered prior to sending
	 * the notification event
	 */
	uint32_t				byteCount;

	/**
	 * Event used to deliver the logical port event notification to the client
	 *
	 * @see NotifyEvent_t
	 */
	NotifyEvent_t			notifyEvent;

	/**
	 * Number of bytse in the filterString member
	 */
	uint32_t				filterByteCount;

	/**
	 * Optional filter string to use for filtering incoming data
	 * subscription notifications
	 */
	LogPortFilterByte_t		filterString[LOG_PORT_FLTR_STRING_LEN+1];

} logPortSubscribe_t;

/**
 * Client message used to subscribe for logical port event notifications
 */
typedef struct _logPortSubscribeReply_t
{
	/**
	 * Bitwise flag indicating which types of logical port events a client wants
	 * to be notified about. Possible event types are given in the
	 * PortSubscription_t enumeration type
	 *
	 * @see PortSubscription_t
	 */
	uint32_t				flags;

	/**
	 * Subscription handle returned to the client to identify his subscription
	 */
	LogPortSubscriptionId_t	handle;

} logPortSubscribeReply_t;

/**
 * Client message used to unsubscribe for logical port event notifications
 */
typedef struct _logPortUnsubscribe_t
{
	/**
	 * Bitwise flag indicating which types of logical port events a client wants
	 * to unsubscribe for. Possible event types are given in the
	 * PortSubscription_t enumeration type
	 *
	 * @see PortSubscription_t
	 */
	uint32_t				flags;

	/**
	 * Subscription handle returned to the client to identify his subscription
	 */
	LogPortSubscriptionId_t	handle;

} logPortUnsubscribe_t;

/**
 * Client message requesting a change in port protocol
 *
 * @see SerialProtocol_t
 */
typedef struct _logPortProtocol_t
{
	/**
	 * Bitwise flag indiciating which members of the portProtocol structure
	 * should be modified
	 */
	uint32_t			flags;

	/**
	 * New protocol to apply to the port
	 *
	 * @see SerialProtocol_t
	 */
	SerialProtocol_t	portProtocol;

} logPortProtocol_t;

/**
 * Message used to set a temporary reply timeout value
 */
typedef struct _logPortSetReplyTimeout_t
{
	/**
	 * Reply timeout in nanoseconds
	 */
	uint64_t	nsTimeout;

} logPortSetReplyTimeout_t;

/**
 * Message used to perform a fast module initialization
 */
typedef struct _logPortEstablishComms_t
{
	/**
	 * The amount of time to drive the comm signal low
	 */
	uint64_t	nsLowTime;
	/**
	 * The amount of time to drive the comm signal high
	 */
	uint64_t	nsHiTime;

	/**
	 * Length of the initMssg member
	 */
	uint32_t	mssgLen;

	/**
	 * Initialization message to send to the module after the init waveform is done
	 */
	uint8_t		initMssg[ 128];

} logPortEstablishComms_t;

/**
 * Low speed initialization support object.
 */
class CommInitByte
{
public:
	CommInitByte() : baud( 0), byteType( 0), timeOut( 0),
		literalVal( 0), refByte( -1), byteMask( 0), parity(NO_PARITY), dataBits(BITS8), 
        stopBits(STOP1), updatePortSettings(false)
	{
	}

	/**
	 * Baud rate needed for communication
	 */
	speed_t		baud;

	/**
	 * 0=Tx or 1=Rx
	 */
	uint8_t		byteType;

	/**
	 * ms
	 */
	uint32_t	timeOut;

	uint8_t		literalVal;

	/**
	 * -1 = literal value, >=0 means reference byte #
	 */
	int8_t		refByte;

	/**
	 * XOR value
	 */
	uint8_t		byteMask;

    Parity_t parity;

    DataBits_t dataBits;

    StopBits_t stopBits;

    bool updatePortSettings;
};

/**
 * Message used to perform a low-speed module initialization
 */
typedef struct
{
	uint8_t			byteCount;

	CommInitByte	mssg[ 1];

} CommInitMessage_t;


/**
 * Logical port driver custom devctl message structure
 */
typedef union
{
	/**
	 * Client message requesting a change in port lock
	 *
	 * @see logPortLockPort_t
	 * @see SerialProtocol_t
	 */
	logPortLockPort_t			lockPort;

	/**
	 * Client message used to subscribe for logical port event notifications
	 *
	 * @see logPortSubscribe_t
	 */
	logPortSubscribe_t			portSubscribe;

	/**
	 * Reply to client's subscription for logical port event notifications
	 *
	 * @see logPortSubscribe_t
	 */
	logPortSubscribeReply_t		portSubscribeReply;

	/**
	 * Client message used to unsubscribe for logical port event notifications
	 *
	 * @see logPortUnsubscribe_t
	 */
	logPortUnsubscribe_t		portUnsubscribe;

	/**
	 * Reply to client's unsubscription for logical port event notifications
	 *
	 * @see logPortUnsubscribe_t
	 */
	logPortUnsubscribe_t		portUnsubscribeReply;

	/**
	 * Client message requesting a change in port protocol
	 *
	 * @see logPortProtocol_t
	 * @see SerialProtocol_t
	 */
	SerialProtocol_t			portProtocol;

	/**
	 * Message used to set a temporary reply timeout value
	 */
	logPortSetReplyTimeout_t	replyTimeout;

	/**
	 * Message used to perform the sequence to establish communications
	 */
	logPortEstablishComms_t		establishComms;


} logPortMsg_t;


/**
 * devctl() DCMD used to lock the logical port
 * Input: logPortLockPort_t
 * Output: None
 */
#define DCMD_LOGPORT_LOCK		__DIOT (_CMD_IOCTL_LOGPORT, LOG_PORT_LOCK_PORT, logPortLockPort_t)

/**
 * devctl() DCMD used to lock the logical port
 * Input: None
 * Output: None
 */
#define DCMD_LOGPORT_UNLOCK		__DION (_CMD_IOCTL_LOGPORT, LOG_PORT_UNLOCK_PORT)

/**
 * devctl() DCMD used to subscriber for port events
 * Input: logPortSubscribe_t
 * Output: logPortSubscribeReply_t
 */
#define DCMD_LOGPORT_SUBSCR		__DIOTF(_CMD_IOCTL_LOGPORT, LOG_PORT_SUBSCRIBE, logPortSubscribeReply_t)

/**
 * devctl() DCMD used to un-subscriber for port events
 * Input: logPortUnsubscribe_t
 * Output: None
 */
#define DCMD_LOGPORT_UNSUBSCR	__DIOT (_CMD_IOCTL_LOGPORT, LOG_PORT_UNSUBSCRIBE, logPortUnsubscribe_t)

/**
 * devctl() DCMD used to read current port protocol
 * Input: None
 * Output: logPortProtocol_t
 */
#define DCMD_LOGPORT_GETPROT	__DIOF (_CMD_IOCTL_LOGPORT, LOG_PORT_GET_PROTOCOL, SerialProtocol_t)

/**
 * devctl() DCMD used to set a new port protocol
 * Input: logPortProtocol_t
 * Output: None
 */
#define DCMD_LOGPORT_SETPROT	__DIOT (_CMD_IOCTL_LOGPORT, LOG_PORT_SET_PROTOCOL, SerialProtocol_t)

/**
 * devctl() DCMD used to reset a connection to a logical port driver
 * Input: None
 * Output: None
 */
#define DCMD_LOGPORT_RST_CONN	__DION (_CMD_IOCTL_LOGPORT, LOG_PORT_RESET_CONN)

/**
 * devctl() DCMD used to reset a logical port driver
 * Input: None
 * Output: None
 */
#define DCMD_LOGPORT_RST_DRV	__DION (_CMD_IOCTL_LOGPORT, LOG_PORT_RESET_DRIVER)

/**
 * devctl() DCMD used to extablish a reply timeout
 * Input: logPortSetReplyTimeout_t
 * Output: None
 */
#define DCMD_LOGPORT_TIMEOUT	__DIOT (_CMD_IOCTL_LOGPORT, LOG_PORT_SET_REPLY_TIMEOUT, logPortSetReplyTimeout_t)

/**
 * devctl() DCMD used to perform an ISO fast initialization sequence
 * Input: logPortEstablishComms_t
 * Output: None
 */
#define DCMD_LOGPORT_EST_COMMS	__DIOT (_CMD_IOCTL_LOGPORT, LOG_PORT_ESTABLISH_COMMS, logPortEstablishComms_t)

/**
 * devctl() DCMD used to perform an ISO 5-baud initialization sequence
 * Input: logPortEstablishComms_t
 * Output: None
 */
#define DCMD_LOGPORT_5BAUD_INIT	__DIOT (_CMD_IOCTL_LOGPORT, LOG_PORT_LOW_SPEED_INIT, CommInitMessage_t)

/**
 * devctl() DCMD used to perform a driver reconnect (close port/re-open port)
 * Input: None
 * Output: None
 */
#define DCMD_LOGPORT_RECONNECT	__DION (_CMD_IOCTL_LOGPORT, LOG_PORT_RECONNECT)

#endif // LOG_PORT_MSSG_H_INCLUDED
