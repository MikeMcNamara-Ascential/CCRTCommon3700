//******************************************************************************
// Description:
//	Logical serial port driver
//
//==============================================================================
//
//     Copyright  1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/Include/LogPortDefs.h $
// 
// 5     2/15/06 7:45p Cward
// Moved definition of Log Port Pulse Codes from Log Port Defs to Bep Defs
//
// 4     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 3     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:29p Cward
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
// 2     6/26/03 1:42p Khykin
// Updated with Brians latest changes.
//
// 3     6/18/03 7:30p Bmeinke
// AddBlockedReader() returns an int (similar to errno)
// If BlockedReaderList::Lock() fails, an error is printed to stderr
//
// 2     5/29/03 1:58p Bmeinke
// Attempt to create a subscription IDs 3 times (until a valid ID is
// created) in LogPortSubscription() constructors
// Streamlined the LogPortSubscription::CreateUniqueSubscriptionId()
// method
// Made m_subIdLock mutex static (was getting EINVAL errors when not
// static)
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 5     3/18/03 6:42p Bmeinke
// Fixed the CommGapTimer class
// Added BlockedWriterList and BlockedWriterList_t data types to manage
// lists of clients waiting towrite to a serial port
//
// 4     3/06/03 10:46p Bmeinke
// Moved SERIAL_DATA_READY and REGISTER_PULSE from RawCommPRoxy.h here to
// consolidate the pulse code enum values
//
// 3     2/06/03 10:07a Bmeinke
// Added BlockedReader_t data type to hold the reply ID and I/O read()
// message for a client blocked on a read() operation
// Changed BlockedReaderList_t to map a client OCB to a BlockedClient_t
// structure
//
// 2     1/18/03 1:44p Bmeinke
// Changed logPortIoOcb_t to IOFUNC_OCB_T
//
// 1     1/17/03 8:44p Bmeinke
// Data typed definitions for logical ports (adapted from LogPortDrv.h)
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _LOG_PORT_DEFS_H_INCLUDED
#define _LOG_PORT_DEFS_H_INCLUDED

#include <stdint.h>					//uint*_t
#include <pthread.h>				// pthread_mutex_t
#include <devctl.h>
#include <sys/types.h>				// off_t
#include <sys/dispatch.h>
#include <list>

#include "ResManagerBase.h"			// resMgrIoOcb_t
#include "LogPortMessage.h"			// Logical port driver custom message definitions
#include "LogPortFilterString.h"	// Filter stringd
#include "BepSync.h"				// BepCondVar
#include "CommFifo.h"				// CommFifo objects

/**
 * Logical port subscriber client attributes
 */
typedef struct _LogPortSubscription_t
{
	/**
	 * Number of bytes to be queued before the notification event
	 * should be sent to the subscriber
	 */
	uint32_t					count;

	/**
	 * Mask bits to filter when a subscribed client wants to be notified of
	 * port events
	 *
	 * @see logPortNotifyMask_t
	 */
	uint32_t					notifyModeMask;

	/**
	 * Flag used to indicate if this client should be notified of
	 * port events contained in the  mask attribute
	 */
	bool						shouldNotify;

	/**
	 * Event used to notify the client of port events
	 *
	 * @see NotifyEvent_t
	 */
	NotifyEvent_t				event;

	/**
	 * Connection to deliver the event notification on
	 */
	int							coId;

	/**
	 * Client's subscription ID
	 */
	LogPortSubscriptionId_t		id;

	/**
	 * List of filter strings that messages must pass through before
	 * a notification can be sent to the subscriber
	 */
	LogPortFilterStringList		filterList;

	/**
	 * Transmit buffer
	 */
	CommFifo					fifo;

} LogPortSubscription_t;

/**
 * Object used to manage a LogPortSubscription
 *
 * @author Brian Meinke
 * @version 1
 * @since April 4, 2002
 */
class LogPortSubscription : public LogPortSubscription_t
{
public:
	/**
	 * Default constructor
	 */
	LogPortSubscription() : LogPortSubscription_t()
	{
		uint8_t attempts=0;

		do
		{
			id = CreateUniqueSubscriptionId();
			delay( 2);
		} while( (id == LOG_PORT_INVALID_SUB) && (attempts++ < 3));

		fifo.Reset();
		count = 0;
		notifyModeMask = LOG_PORT_NOTIFY_NEVER;
		shouldNotify = false;
		coId = -1;
	}

	/**
	 * Constructor with mask and byte count initializers
	 */
	LogPortSubscription( uint32_t byteCount) : LogPortSubscription_t()
	{
		uint8_t attempts=0;

		do
		{
			id = CreateUniqueSubscriptionId();
			delay( 2);
		} while( (id == LOG_PORT_INVALID_SUB) && (attempts++ < 3));

		fifo.Reset();
		count = byteCount;
		notifyModeMask = LOG_PORT_NOTIFY_NEVER;
		shouldNotify = false;
		coId = -1;
	}

	/**
	 * Constructor with byte count, notification event, mode and connection initializers
	 */
	LogPortSubscription(uint32_t byteCount, uint32_t modeMask, const NotifyEvent_t &clientEvent, int coid) :
		LogPortSubscription_t()
	{
		uint8_t attempts=0;

		do
		{
			id = CreateUniqueSubscriptionId();
			delay( 2);
		} while( (id == LOG_PORT_INVALID_SUB) && (attempts++ < 3));

		fifo.Reset();
		SetNotifyEvent( clientEvent, coid);

		count = byteCount;
		notifyModeMask = modeMask;
		shouldNotify = false;
	}

	/**
	 * Destructor
	 */
	~LogPortSubscription()
	{
	}

	/**
	 * Method used to initialize the event used to deliver notifications
	 * to the subscribed client
	 *
	 * @param clientEvent
	 *               Event used to deliver notifications to the subscribed client
	 * @param coid   Connection to which the event should be delivered when
	 *               notifying a subscriber
	 */
	void SetNotifyEvent( const NotifyEvent_t &clientEvent, int coid)
	{
		// Copy the event to be used to deliver the notification
		event = clientEvent;
		coId = coid;
		// Set flag to indicate that subscription is valid
		shouldNotify = true;
	}

	/**
	 * Method used to check if a subscription is valid
	 *
	 * @return true of the notification event has been initialized and the
	 *         notification should be delivered, false otherwise
	 */
	bool ShouldNotify() const
	{
		bool retVal = false;
		if( (shouldNotify == true) && (coId != -1))
		{
			retVal = true;
		}
		return( retVal);
	}

	/**
	 * Invalidates this subscription such that subsequent
	 * calls to ShouldNotify() return false
	 */
	void Invalidate()
	{
		memset( &event, 0, sizeof( event));
		coId = -1;
		shouldNotify = false;
	}

private:

	/**
	 * Creates a new and unique subscription ID
	 *
	 * @return Unique subscription ID
	 */
	LogPortSubscriptionId_t CreateUniqueSubscriptionId()
	{
		LogPortSubscriptionId_t		subId = LOG_PORT_INVALID_SUB;

		// Need to lock this operation to avoid duplicate subscription ID's
		if( (errno=m_subIdLock.Acquire()) == EOK)
		{
			// Use current time (with nanosec's) as subscription ID
			if( ClockTime( CLOCK_REALTIME, NULL, &subId) != -1)
			{
				// Wait here for a little bit so no sub ID's are the same
				delay( 5);
			}
			else
			{
				// Convert time to a 64-bit "subscription handle"
				subId = LOG_PORT_INVALID_SUB;
				perror( "\tINVALID subscription in CreateUniqueSubscriptionId");
			}
			m_subIdLock.Release();
		}
		else
		{
			// Convert time to a 64-bit "subscription handle"
			subId = LOG_PORT_INVALID_SUB;
			perror( "\tERROR locking subscription mutex INVALID subscription in CreateUniqueSubscriptionId");
		}
		return( subId);
	}


	/**
	 * Mutex used to guard the creation of unique subscription IDs.
	 * Declared in RawCommProxy.cpp
	 */
	static BepMutex	m_subIdLock;
};

/**
 * OCB structure used to for logical port I/O processing
 */
typedef struct _logPortIoOcb_t
{
	/**
	 * OCB used (and expected) by the base class I/O routines.
	 * !!!!! MUST BE FIRST !!!!!
	 */
	resMgrIoOcb_t				baseOcb;

	/**
	 * Mask bits to identify what events a client is subscribed for (RX, TX, etc)
	 *
	 * @see logPortNotifyMask_t
	 */
	uint32_t					subMask;

	/**
	 * Flag to indicate if this client has locked the port for exclusive use
	 */
	int							portLockCount;

	/**
	 * Client's subscription ID
	 */
	LogPortSubscriptionId_t		subscriptionId;

	/**
	 * Subscription attributes for RX data subscription
	 */
	LogPortSubscription			*rxSubscription;

	/**
	 * Subscription attributes for TX data subscription
	 */
	LogPortSubscription			*txSubscription;

	/**
	 * Subscription attributes for port lock events
	 */
	LogPortSubscription			*lockSubscription;

	/**
	 * Subscription attributes for port unlock events
	 */
	LogPortSubscription			*unlockSubscription;

	/**
	 * Mask bits to filter when a subscribed client wants to be notified of
	 * port events
	 *
	 * @see logPortNotifyMask_t
	 */
	long						notifyModeMask;

	/**
	 * Number of bytes the client wants buffered prior to being notified
	 */
	off_t						notifyByteCount;

	/**
	 * Object used to log serial communication
	 */
	ILogger						commLogger;

	/**
	 * Amount of time to wait (in nanoseconds) for a reply message before a
	 * timeout condition is raised. This is a temporary timeout value which
	 * will be reset after sending the first message after this value is
	 * specified
	 */
	uint64_t					replyTimeout;

	/**
	 * notification list used by iofunc_notify*()
	 */
    iofunc_notify_t 			notify[3];

} logPortIoOcb_t;


/**
 * Data type used to indicate communication direction of serial data
 */
typedef enum
{
	/**
	 * Value used to indicate communication to a vehicle
	 */
	ComDirTx,
	/**
	 * Value used to indicate communication from a vehicle
	 */
	ComDirRx,
	/**
	 * Value used to indicate a mesasge Acknowledgement
	 */
	ComDirBusAck,
	/**
	 * Value used to indicate an error response
	 */
	ComDirErr

} LogPortCommDir_t;


/**
 * Data type used to hold client subscription ID's and map them to event
 * notification structs
 * @since Nov 29, 2001
 */
typedef std::map< LogPortSubscriptionId_t, IOFUNC_OCB_T*>		PortSubscrList_t;
typedef PortSubscrList_t::iterator		   						PortSubscrListItr_t;
typedef PortSubscrList_t::const_iterator   						PortSubscrListCItr_t;
typedef PortSubscrList_t::value_type	   						PortSubscrListVal_t;

/**
 * Data type used to hold client subscription ID's and map them to event
 * notification structs. Extended from PortSubscrList_t to make adding and
 * removing subscribers easier
 *
 * @author Brian Meinke
 * @version 1
 * @see PortSubscrList_t
 * @since Nov 29,2001
 */
class PortSubscrList : public PortSubscrList_t
{
public:
	PortSubscrList() : PortSubscrList_t()
	{
	};

	~PortSubscrList()
	{
	};

	/**
	 * Adds a new subscriber to the list
	 *
	 * @param subId  Client's subscription ID
	 * @param ioOcb  Client's connection ID containing his subscription attributes
	 *
	 * @return EOK if subscriber is successfully added
	 */
	int AddSubscriber( const LogPortSubscriptionId_t &subId, IOFUNC_OCB_T *ioOcb)
	{
		int retVal = EAGAIN;

		if( (retVal=Lock()) == EOK)
		{
		insert( PortSubscrListVal_t( subId, ioOcb));
			Unlock();
		}

		return( retVal);
	}

	/**
	 * Removes a subscriber from the subscription list
	 *
	 * @param subId  Subscription ID of the client to remove from the list
	 *
	 * @return EOK if subscriber is successfully removed
	 */
	int DelSubscriber( const LogPortSubscriptionId_t &subId)
	{
		int retVal = EAGAIN;

		if( (retVal=Lock()) == EOK)
		{
		erase( subId);
			Unlock();
		}

		return( retVal);
	};

	int Lock()
	{
		return( m_listLock.Acquire());
	}

	int Unlock()
	{
		return( m_listLock.Release());
	}

private:
	/**
	 * Mutex used to lock the list for exclusive access
	 */
	BepMutex	m_listLock;
};

#if 0
// moved to BepDefs.h 2/10/2006 ckw

/**
 * Pulse code identifiers for logical port events
 *  LOG_PORT_IRQ_PULSE_CODE
 *  LOG_PORT_NOTIFY_PULSE_CODE
 *  MSSG_GAP_TIMER
 *  BYTE_GAP_TIMER
 */
typedef enum
{
	/**
	 * Pulse code to be associated with serial interrupt events
	 */
	LOG_PORT_IRQ_PULSE_CODE		= 0x01,

	/**
	 * Pulse code used to tell the logical port driver to send event notifications
	 * to subscribed clients. The pulse value will be either PORT_SUBSCRIBE_RX to
	 * deliver notifications to clients subscribed for received data events or
	 * PORT_SUBSCRIBE_TX to deliver notifications to clients subscribed for transmit
	 * data events.
	 */
	LOG_PORT_NOTIFY_PULSE_CODE	= 0x02,

	/**
	 * Pulse code identifier for inter-message timing events
	 */
	MSSG_GAP_TIMER				= 0x03,

	/**
	 * Pulse code identifier for inter-byte timing events
	 */
	BYTE_GAP_TIMER				= 0x04,

	/**
	 * Pulse code returned from the serial driver to tell the reader thread that new
	 * data is available to read
	 */
	SERIAL_DATA_READY			= 0x05,

	/**
	 * Pulse code used by the serial server to tell us
	 * to register
	 */
	REGISTER_PULSE				= 0x06,

	/**
	 * Pulse code identifier for inter-message timing events
	 */
	REPLY_TIMEOUT_TIMER			= 0x07,

	NEXT_READ_CODE

} LogPortPulseCodes_t;

#endif
/**
 * Function used by the resource manager library for handling pulses
 * delivered to the resource manager's dispatch interface
 */
typedef int (*PulseHandler_t)(message_context_t*, int, unsigned, void*);

/**
 * Timer use for inter-message and inter-byte timing
 *
 * @author Brian Meinke
 * @version 1
 * @since Dec 2, 2001
 */
class CommGapTimer
{
public:
	CommGapTimer() : m_timerId( -1)
	{
	}

	~CommGapTimer()
	{
		Destroy();
	}

	/**
	 * Loads the comm timer with the specified timeout value. Timer notification
	 * is accompished via a pulse with the specified code and priority and is
	 * delivered to the dispatch context pointed to by dpp. A message_atatch()
	 * call must be made after this function is invoked to properly attach the
	 * pulse to the channel managed by the specified dispatch pointer. To start
	 * the timer, call Arm().
	 *
	 * @param coId      Connection to deliver the timer expiration pulse on
	 * @param pulseCode The pulse code to be delivered when the timer expires
	 * @param pulseVal  Value of the pulse to be sent when the timer expires
	 * @param prio      Priority to send the timer expiration pulse to
	 * @return -1 if an error occurred any other value indicates success
	 *         (use errno for more detailed error information)
	 * @see Arm
	 */
	int Initialize( int coId, uint8_t pulseCode=MSSG_GAP_TIMER, uint32_t pulseVal=0, int prio=SIGEV_PULSE_PRIO_INHERIT)
	{
		// Make sure not currently running
		Destroy();

		// Initialize the event to be delivered when the timer expires
		SIGEV_PULSE_INIT( &m_timerEvent, coId, prio, pulseCode, pulseVal);

		// Create the timer
		m_timerId = TimerCreate( CLOCK_REALTIME, &m_timerEvent);

		return( m_timerId);
	}

#if 0
	/**
	 * Starts the timer. A previously successfule call to Load() must be
	 * done prior to calling this method
	 *
	 * @param delay    Initial timer timeout values (in milliseconds)
	 * @param interval Timer repeat value (in milliseconds). default=0
	 *
	 * @return 0 if successful, -1 if an error occurred (use errno for more
	 *         detailed error information)
	 */
	int Arm( const uint32_t delay, const uint32_t interval=0)
	{
		int 			retVal = EBUSY;
		struct _itimer	timerTime;

		// Stop any current timer activity
		Stop();

		timerTime.nsec = mSEC_nSEC( delay);
		timerTime.interval_nsec = mSEC_nSEC( interval);

		// Start the timer
		retVal = TimerSettime( m_timerId, 0, &timerTime, NULL);

		return( retVal);
	}
#endif

	/**
	 * Starts the timer. A previously successfule call to Load() must be
	 * done prior to calling this method
	 *
	 * @param delay    Initial timer timeout values (in nanoseconds)
	 * @param interval Timer repeat value (in nanoseconds). default=0
	 * @return 0 if successful, -1 if an error occurred (use errno for more
	 *         detailed error information)
	 */
	int Arm( const uint64_t &delay, const uint64_t &interval=0)
	{
		int 			retVal = EBUSY;
		struct _itimer	timerTime;

		// Stop any current timer activity
		Stop();

		timerTime.nsec = delay;
		timerTime.interval_nsec = interval;

		// Start the timer
		retVal = TimerSettime( m_timerId, 0, &timerTime, NULL);
		if(retVal == -1)
			printf( "\tERROR starting the timer: %s\n", strerror( errno));

		return( retVal);
	}

	/**
	 * Stop the timer
	 */
	int Stop()
	{
		int retVal;
		struct _itimer	timerTime;
		memset( &timerTime, 0, sizeof( timerTime));

		// Stop the timer
		if( (retVal = TimerSettime( m_timerId, 0, &timerTime, NULL)) == -1)
		{
			printf( "\tERROR stopping timer: %s\n", strerror( errno));
		}

		return( retVal);
	}

	/**
	 * Stop and destroy the timer
	 */
	void Destroy()
	{
		// Stop and destroy the timer
		if( m_timerId != -1)
		{
			Stop();

			// Destroy the timer
			TimerDestroy(m_timerId);

			m_timerId = -1;
		}
	}

	/**
	 * Checks to see if the timer is currently armed (Active)
	 *
	 * @return true if timer is active, false if not active or unable to
	 *         determine if timer is active
	 */
	bool IsTimerArmed()
	{
		bool	retVal = false;
		struct _timer_info timerInfo;

		if( m_timerId != -1)
		{
			if( TimerInfo( 0, m_timerId, 0, &timerInfo) != -1)
			{
				retVal = ((timerInfo.flags & _NTO_TI_ACTIVE) == _NTO_TI_ACTIVE);
			}
		}
		return( retVal);
	}

private:

	/**
	 * Timer ID
	 */
	timer_t				m_timerId;

	/**
	 * Event used to deliver inter-mssg gap timing events
	 */
	NotifyEvent_t		m_timerEvent;

};

/**
 * Data type used to hold information about a client who is blocked
 * on a read operation
 */
typedef struct
{
	/**
	 * Reply ID of the client; this is where we MsgReply() to when there
	 * is data ready for the client
	 */
	int 		replyId;

	/**
	 * The I/O message originally sent to use by the client when the read
	 * operation started
	 */
	io_read_t	readMsg;

	/**
	 * Clients OCB identifier
	 */
	const IOFUNC_OCB_T	*ocb;

} BlockedReader_t;

typedef std::map< const IOFUNC_OCB_T*, BlockedReader_t>		BlockedReaderList_t;
typedef BlockedReaderList_t::iterator						BlockedReaderListItr_t;
typedef BlockedReaderList_t::const_iterator					BlockedReaderListCItr_t;
typedef BlockedReaderList_t::value_type						BlockedReaderListVal_t;

class BlockedReaderList : public BlockedReaderList_t
{
public:
	BlockedReaderList() : BlockedReaderList_t()
	{
	}

	~BlockedReaderList()
	{
	}

	int Lock()
	{
		return( m_listLock.Acquire());
	}

	int Unlock()
	{
		return( m_listLock.Release());
	}

	/**
	 * Adds a new blocked reader to the list of blocked readers
	 *
	 * @param ocb     OCB used to uniquely identify a client connectoin
	 * @param rcvId   The channel on which the client is waiting for data on (and where we should reply
	 *                to when data is ready for the client)
	 * @param readMsg The read message received from the client
	 *
	 * @return EOK if client added to the list
	 */
	int AddBlockedReader( const IOFUNC_OCB_T* ocb, int rcvId, const io_read_t &readMsg)
	{
		int retVal;

		if( (retVal=Lock()) == EOK)
		{
			BlockedReader_t	client;
			client.replyId = rcvId;
			client.ocb = ocb;
			memcpy( &client.readMsg, &readMsg, sizeof( readMsg));

			insert( BlockedReaderListVal_t( ocb, client));

			Unlock();
		}
		else
		{
			errno = retVal;
			perror( "Failed to lock mutex in AddBlockedReader");
		}

		return( retVal);
	}

	/**
	 * Removes a client from the list of blocked readers
	 *
	 * @param ocb    OCB of the client to be removed from the blocked reader list
	 */
	void RemoveBlockedReader( const IOFUNC_OCB_T* ocb)
	{
		if( (errno=Lock()) == EOK)
		{
			erase( ocb);
			Unlock();
		}
		else
		{
			perror( "Failed to lock mutex in RemoveBlockedReader");
		}
	}

	/**
	 * Removes a client from the list of blocked readers based on a client's reply ID.
	 *
	 * @param replyId Connection ID on which a client is waiting for a reply message
	 */
	void RemoveBlockedReader( int replyId)
	{
		BlockedReaderListItr_t itr;
		if( (errno=Lock()) == EOK)
		{
			for( itr=begin(); itr!=end(); itr++)
			{
				if( itr->second.replyId == replyId)
				{
					itr = erase( itr);
				}
			}
			Unlock();
		}
		else
		{
			perror( "Failed to lock mutex in RemoveBlockedReader");
		}
	}

	/**
	 * Finds a matching BlockedReader_t for the client identified by the given OCB.
	 *
	 * @param ocb    OCB of the client whose reply ID we are looking for
	 * @param client Buffer where the attributes of the blocked client will be put if
	 *               we find a matching OCB in our list
	 *
	 * @return true if an OCB matching 'ocb' is found in our list, false if no
	 *         matching OCB was found
	 */
	bool FindBlockedReader( const IOFUNC_OCB_T *ocb, BlockedReader_t &client)
	{
		bool	retVal = false;
		BlockedReaderListItr_t	itr;

		if( (errno=Lock()) == EOK)
		{
			if( (itr=find( ocb)) != end())
			{
				client = itr->second;
				retVal = true;
			}

			Unlock();
		}
		else
		{
			perror( "Failed to lock mutex in FindBlockedReader");
		}
		return( retVal);
	}

private:
	BepMutex	m_listLock;
};


/**
 * Data type used to hold information about a client who is blocked
 * on a write operation
 */
typedef struct
{
	/**
	 * Reply ID of the client; this is where we MsgReply() to when the
	 * client's write operation completes
	 */
	int 				replyId;

	/**
	 * The message data to be sent
	 */
	SerialString_t		writeMsg;

	/**
	 * Context structure
	 */
	resmgr_context_t	*ctp;

	/**
	 * Clients OCB identifier
	 */
	const IOFUNC_OCB_T	*ocb;

} BlockedWriter_t;

typedef std::list< BlockedWriter_t>				BlockedWriterList_t;
typedef BlockedWriterList_t::iterator			BlockedWriterListItr_t;
typedef BlockedWriterList_t::const_iterator		BlockedWriterListCItr_t;
typedef BlockedWriterList_t::value_type			BlockedWriterListVal_t;

class BlockedWriterList : public BlockedWriterList_t
{
public:
	BlockedWriterList() : BlockedWriterList_t()
	{
	}

	~BlockedWriterList()
	{
	}

	int Lock()
	{
		return( m_listLock.Acquire());
	}

	int Unlock()
	{
		return( m_listLock.Release());
	}

	/**
	 * Adds a new blocked writer to the list of blocked writers
	 *
	 * @param ctp    Context structure
	 * @param rcvId  The channel on which the client is waiting for data on (and where we should reply
	 *               to when data is ready for the client)
	 * @param mssg   The serial data to be sent
	 * @param ocb    Clients OCB identifier
	 */
	void AddBlockedWriter( resmgr_context_t *ctp, int rcvId, const SerialString_t &mssg, const IOFUNC_OCB_T *ocb)
	{
		if( Lock() == EOK)
		{
			BlockedWriter_t	client;

			client.replyId = rcvId;
			client.writeMsg = mssg;
			client.ctp = ctp;
			client.ocb = ocb;

			push_back( client);

			Unlock();
		}
	}

	/**
	 * Removes a client from the list of blocked writers based on a client's reply ID.
	 *
	 * @param replyId Connection ID on which a client is waiting for a reply message
	 */
	void RemoveBlockedWriter( int replyId)
	{
		BlockedWriterListItr_t itr;
		if( Lock() == EOK)
		{
			for( itr=begin(); itr!=end(); itr++)
			{
				BlockedWriter_t	&client = *itr;

				if( client.replyId == replyId)
				{
					itr = erase( itr);
				}
			}
			Unlock();
		}
	}

	/**
	 * Removes a client from the list of blocked writers based on a client's reply ID.
	 *
	 * @param ocb    OCB structure identifying the client to be removed
	 */
	void RemoveBlockedWriter( const IOFUNC_OCB_T *ocb)
	{
		BlockedWriterListItr_t itr;
		if( Lock() == EOK)
		{
			for( itr=begin(); itr!=end(); itr++)
			{
				BlockedWriter_t	&client = *itr;

				if( client.ocb == ocb)
				{
					itr = erase( itr);
				}
			}
			Unlock();
		}
	}

	/**
	 * Finds a matching BlockedWriter_t for the client identified by the given OCB.
	 *
	 * @param ocb    OCB of the client we are looking for
	 * @param client Buffer where the attributes of the blocked clients will be put if
	 *               we find a matching OCB in our list
	 *
	 * @return true if an OCB matching 'ocb' is found in our list, false if no
	 *         matching OCB was found
	 */
	bool FindBlockedWriter( const IOFUNC_OCB_T *ocb, BlockedWriter_t &client)
	{
		bool	retVal = false;
		BlockedWriterListItr_t	itr;

		if( Lock() == EOK)
		{
			for( itr=begin(); itr!=end(); itr++)
			{
				if( (const IOFUNC_OCB_T*)itr->ocb == ocb)
				{
					client = *itr;
					retVal = true;
					break;
				}
			}

			Unlock();
		}
		return( retVal);
	}

private:
	BepMutex	m_listLock;
};


/**
 * Define data type which describes a client waiting to lock the logical port
 */
typedef struct
{
	/**
	 * Client's OCB
	 */
	IOFUNC_OCB_T	*ocb;

	/**
	 * Connection to respond to when the port is successfully locked for the client
	 */
	int				replyId;

} LockWaitListEntry_t;

/**
 * Define data type to hold reply ID's of clients waiting to lock the port
 */
typedef std::list< LockWaitListEntry_t>		LockWaitList_t;
typedef LockWaitList_t::iterator			LockWaitListItr_t;
typedef LockWaitList_t::const_iterator		LockWaitListCItr_t;
typedef LockWaitList_t::value_type			LockWaitListVal_t;

class LockWaitList : public LockWaitList_t
{
public:
	LockWaitList() : LockWaitList_t()
	{
	}

	~LockWaitList()
	{
	};

	/**
	 * Adds a new client to the list
	 *
	 * @param ocb     Client OCB to be added to the list
	 * @param replyId Connection ID to respond to the client on
	 * @param head    Flag indicating if the client should be added at the head of
	 *                the list (true) or the end. flase by default
	 * @return Number of clients in the list (after the insertion)
	 */
	int AddClient( IOFUNC_OCB_T *ocb, int replyId, bool head=false) throw( BepException)
	{
		LockWaitListEntry_t	newEntry;
		int					retVal;

		if( (retVal=m_listLock.Acquire()) == EOK)
		{
			newEntry.ocb = ocb;
			newEntry.replyId = replyId;

			if( head)	LockWaitList_t::push_front( newEntry);
			else		LockWaitList_t::push_back( newEntry);

			retVal = LockWaitList_t::size();

			m_listLock.Release();
		}
		else
		{
			throw( BepException( "%s\n", strerror(retVal)));
		}
		return( retVal);
	}

	/**
	 * Pops the beginning client from the list (the list entry at the beginning
	 * of the list is removed)
	 *
	 * @param ocb     Returned OCB of the client from the head of the list
	 * @param replyId Returned connection ID to respond to the client at the head of the list on
	 * @return EOK if the client was successfully popped, error code otherwise
	 */
	int GetNextClient( IOFUNC_OCB_T* &ocb, int &replyId)
	{
		LockWaitListEntry_t	headEntry;
		int					retVal;

		if( (retVal=m_listLock.Acquire()) == EOK)
		{
			if( LockWaitList_t::size() > 0)
			{
				headEntry = LockWaitList_t::front();
				LockWaitList_t::pop_front();

				ocb = headEntry.ocb;
				replyId = headEntry.replyId;
			}
			else
			{
				retVal = ENODATA;
			}
			m_listLock.Release();
		}
		return( retVal);
	}

	/**
	 * Removes the entry nearest the end of the list with the given OCB value
	 *
	 * @param ocb    OCB of the client to be removed from the list
	 * @return true if the given client OCB was removed from the list
	 *         false if the OCB was not in the list or if an error occurred
	 */
	bool PopClient( IOFUNC_OCB_T* ocb)
	{
		LockWaitListItr_t	clientItr;
		bool				retVal = false;

		if( (retVal=m_listLock.Acquire()) == EOK)
		{
			// If any entries in the list
			if( LockWaitList_t::size() > 0)
			{
				// remove first entry fro client
				for( clientItr=begin(); clientItr!=end(); clientItr++)
				{
					LockWaitListEntry_t	&client = *clientItr;
					// If this is the client we are looking for
					if( client.ocb == ocb)
					{
						// Remove client from the list
						clientItr = erase( clientItr);
						retVal = true;
					}
				}
			}

			m_listLock.Release();
		}

		return( retVal);
	}

private:

	BepMutex	m_listLock;
};

#endif // _LOG_PORT_DEFS_H_INCLUDED

