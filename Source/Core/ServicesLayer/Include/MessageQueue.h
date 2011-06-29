//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Include/MessageQueue.h $
// $Author: Cward $
//   $Date: 12/21/03 6:27p $
//
// Description:
//	POSIX message queue wrapper
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
//     $Log: /Core/ServicesLayer/Include/MessageQueue.h $
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
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:43  tbochene
//
//     PooBag
//
// 
// 1     1/27/03 7:07a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:05a Khykin
// Initial Checkin From DCTC
// 
// 5     8/01/02 1:25p Bmeinke
// Made ~RawMessageQueue virtual to clean up compiler warnings
// 
// 4     3/22/02 1:40p Bmeinke
// Added and fixed javadoc comments
// Changed base Message Queue class name to RawMessageQueue and made
// MessageQueue a typedef of the base class
// Added a name argument to the Create() method
// Fixed bugs when creating a local BepMutex object: was passing pointer
// to global BepMutex instead of a reference
// Create() now throws an exception if the desied queue cannot be created
// QueueSend() now has a default value on priority argument of 0
// Added overloaded QueueSend() and QueueReceive() to allow
// reading/writing via uint8_t arrays
// In SetQueueName() add a '/' to the beginning of the queue name if it
// not already there
// 
// 3     2/01/02 9:49p Brian.meinke
// Changed from pthread_mutex_*() functions to BepMutex objects
// Cleaned up comments
// 
// 2     11/08/01 9:12p Brian.meinke
// Seperated Create and Open methods
// Unlink queue in Close if we created it
// Re-register for notification inside Receive
// 
// 1     11/07/01 3:10p Brian.meinke
// Data types and definitions for the serial comm server
// 
//******************************************************************************

#ifndef _MESSAGE_QUEUE_H_INCLUDED
#define _MESSAGE_QUEUE_H_INCLUDED

//Include files
#include <inttypes.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <string>

#include "BepDefs.h"
#include "BepSync.h"

using namespace std;

/**
 * Typedef mq_attr struct for convenience
 * 
 * @see struct mq_attr
 */
typedef struct mq_attr	mq_attr_t;

/**
 * Default mode for creating a message queue: R/W user,group,other
 */
#define DEF_CREATE_MODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/**
 * Default message queue message data type
 */
#define DEF_OPEN_MODE	O_RDWR

/**
 * Default message size for the message queue
 */
#define DEF_QUEUE_MSG_SZ	1024

/**
 * Default message layout for messages passing through the queue
 */
typedef struct
{
	/**
	 * Message data
	 */
	char msgBuf[ DEF_QUEUE_MSG_SZ];

} MssgQueueMsg_t;

/**
 * Object to encapsulate POSIX message queue functionality
 * 
 * @author Brian Meinke
 * @version 1.0
 * @see MssgQueueMsg_t
 */
template< class T>
class RawMessageQueue
{
public:
	typedef T	QueDataType_t;

	RawMessageQueue( const char *queueName="") : m_queueName( queueName), 
		m_queueId( -1), m_creator( false), m_wantsNotify( false)
	{
		if( m_queueName.length() > 0)
		{
			SetQueueName( queueName);
		}
		QueueInit();
	}

	RawMessageQueue( const string &queueName) : m_queueName( queueName), 
		m_queueId( -1), m_creator( false), m_wantsNotify( false)
	{
		if( m_queueName.length() > 0)
		{
			SetQueueName( queueName);
		}
		QueueInit();
	}

	virtual ~RawMessageQueue()
	{
		CloseQueue();
	}

	/**
	 * Creates the message queue with the specified queue
	 * attributes. All parameters are optional. If failIfExists is
	 * false and the queue already exists, this method will
	 * attempt to open the queue
	 * 
	 * @param name   Name of the queue to open/create. Defaults to the name already
	 *               specified via the constructor
	 * @param createAlways
	 *               Flag to indicate if we want to ALWAYS create the queue. If
	 *               this flag is set to true and queue alreasdy exists, we will
	 *               try to destroy the queue before creating it.
	 * @param attr   Attributes to apply to the queue. Default=NULL (use OS defaults)
	 * @return Queue ID if successful, -1 on error (use errno for more error information)
	 * @see mq_attr_t
	 */
	int CreateQueue( const char *name=NULL, bool createAlways=true, mq_attr_t *attr=NULL)
	{
		int			openMode = DEF_OPEN_MODE | O_CREAT;
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			// If want to always create the queue
			if(  createAlways == true)
			{
				// Make it look like we are the creator of the queue
				m_creator = true;
				openMode |= O_EXCL;
			}

			// No multiple opens
			CloseQueue();
			
			// Check if new name specified
			if( name != NULL)
			{
				SetQueueName( name);
			}

			// If queue attributes specified
			if( attr != NULL)
			{
				// Use the specified queue attributes
				memmove( &m_queueAttrib, attr, sizeof( m_queueAttrib));
			}
			// Open the Queue
			m_queueId = mq_open( m_queueName.c_str(), openMode, DEF_CREATE_MODE, &m_queueAttrib);

			// If we were able to open the queue
			if( m_queueId != -1)
			{
				// Get queue attributes
				GetQueueAttr( m_queueAttrib);
				m_creator = true;
			}
			else
			{
				string errStr("Error creating queue '");
				errStr += m_queueName;
				errStr += "' in CreateQueue";
				ThrowOsException( errno, errStr);
			}
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in CreateQueue";
			ThrowOsException( queueErr, errStr);
		}

        return( m_queueId);
	}

	/**
	 * Opens an existing message queue with the specified open mode and
	 * name.
	 * 
	 * @param name     Name of the queue to open/create. Defaults to the name already
	 *                 specified via the constructor
	 * @param openMode Mode in which to open the queue. Default=O_RDWR (read/write)
	 * @param create   Flag to indicate if it is OK to create the queue if it does not
	 *                 exist. Default = false
	 * @return QueueI if successful, -1 on error (use errno for more error information)
	 * @see mq_attr_t
	 */
	int OpenQueue( const char *name=NULL, int openMode=DEF_OPEN_MODE, bool create=false)
	{
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			// No multiple opens
			CloseQueue();
	
			// Check if new name specified
			if( name != NULL)
			{
				SetQueueName( name);
			}
	
			m_creator = false;
			
			// If OK to create
			if( create)
			{
				CreateQueue( name, false, NULL);
			}
			else
			{
				// Open the Que
				m_queueId = mq_open( m_queueName.c_str(), DEF_OPEN_MODE, DEF_CREATE_MODE, NULL);
			}
	
			// If successful
			if( m_queueId != -1)
			{
				// Get queue attributes
				GetQueueAttr( m_queueAttrib);
			}
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in OpenQueue";
			ThrowOsException( queueErr, errStr);
		}

        return( m_queueId);
	}

	/**
	 * Closes the message queue. If we created the queue, this method will
	 * also unlink the queue.
	 */
	void CloseQueue()
	{
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			// If I created the queue
			if( m_creator)
			{
				// Destroy it
				mq_unlink( m_queueName.c_str());
			}

			// if queue ID is valid
			if( m_queueId != -1)
			{
				// Close the queue
				mq_close( m_queueId);
	
				// Invalidate data attributes
				m_queueId = -1;
				m_queueName = "";
				memset( &m_queueAttrib, 0, sizeof( m_queueAttrib));
				memset( &m_notifyEvent, 0, sizeof( m_notifyEvent));
				m_creator = false;
			}
			
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in CloseQueue";
			ThrowOsException( queueErr, errStr);
		}
	}

	/**
	 * Returns the name of the message queue
	 * 
	 * @return The name of the message queue
	 */
	const string& GetQueueName() const
	{
		return( m_queueName);
	}
    
	/**
	 * Request to be notified when there is a message in the queue
	 * 
	 * @param notifyStruct
	 *               Notification structure to use for notification
	 * @return 0 if successfully registered for notification, -1 if error (use
	 *         errno for more error information)
	 */
	int QueueNotify( const struct sigevent &notifyStruct)
	{
		int 		retVal = -1;
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			// Set flag
			m_wantsNotify = true;
			// Copy notification struct
			m_notifyEvent = notifyStruct;
			//memmove( &m_notifyEvent, &notifyStruct, sizeof( m_notifyEvent));
			retVal = mq_notify( m_queueId, &m_notifyEvent);
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in OpenQueue";
			ThrowOsException( queueErr, errStr);
		}

		return( retVal);
	}
    
	/**
	 * Request to cancel any pending queue notifications
	 * 
	 * @return 0 if successfully registered for notification, -1 if error (use
	 *         errno for more error information)
	 */
	void CancelNotify()
	{
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			// If currently waiting for notification
			if( m_wantsNotify)
			{
				// Tell queue manager to cancel our notification
				mq_notify( m_queueId, NULL);
			}
			m_wantsNotify = false;
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in OpenQueue";
			ThrowOsException( queueErr, errStr);
		}
	}

	/**
	 * Retrieves the queue attributes
	 * 
	 * @param queueAttribs
	 *               mq_attr_t object to fill in with the queue attributes
	 * @return 0 if successfully registered for notification, -1 if error (use
	 *         errno for more error information)
	 * @see mq_attr_t
	 */
	int GetQueueAttr( mq_attr_t &queueAttribs) const
	{
		int			retVal = -1;
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			retVal = mq_getattr( m_queueId, &queueAttribs);
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in OpenQueue";
			ThrowOsException( queueErr, errStr);
		}

		return( retVal);
	}

	/**
	 * Sets new attributes for the message queue
	 * 
	 * @param queueAttribs
	 *                  Attributes to apply to the message queue
	 * @param oldAttrib Optional mq_attr_t struct to receive the previous queue
	 *                  attributes
	 * @return 0 if successfully registered for notification, -1 if error (use
	 *         errno for more error information)
	 * @see mq_attr_t
	 */
	int SetQueueAttr( const mq_attr_t &queueAttribs, mq_attr_t *oldAttrib=NULL)
	{
		int			retVal = -1;
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			retVal = mq_setattr( m_queueId, &queueAttribs, oldAttrib);
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in OpenQueue";
			ThrowOsException( queueErr, errStr);
		}

		return( retVal);
	}

	/**
	 * Sends a message via the message queue
	 * 
	 * @param msg      Message to send to the queue
	 * @param priority Optional message priority
	 * @return 0 if successfully registered for notification, -1 if error (use
	 *         errno for more error information)
	 */
	int QueueSend( const T& msg, unsigned int priority=0)
	{
		return( QueueSend( msg, sizeof( msg), priority));
	}

	/**
	 * Sends a message via the message queue
	 * 
	 * @param msg      Message to send to the queue
	 * @param len      Number of bytes from the 'msg' object to send to the queue
	 * @param priority Optional message priority
	 * @return 0 if successfully registered for notification, -1 if error (use
	 *         errno for more error information)
	 */
	int QueueSend( const T& msg, size_t len, unsigned int priority=0)
	{
		int			retVal = -1;
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			retVal = mq_send( m_queueId, (const char*)&msg, len, priority);
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in OpenQueue";
			ThrowOsException( queueErr, errStr);
		}

		return( retVal);
	}

	/**
	 * Sends a message via the message queue
	 * 
	 * @param msg      Message to send to the queue
	 * @param len      Number of bytes from the 'msg' buffer to send to the queue
	 * @param priority Optional message priority
	 * @return 0 if successfully registered for notification, -1 if error (use
	 *         errno for more error information)
	 */
	int QueueSend( const char *msg, size_t len, unsigned int priority=0)
	{
		int			retVal = -1;
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			retVal = mq_send( m_queueId, msg, len, priority);
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in OpenQueue";
			ThrowOsException( queueErr, errStr);
		}

		return( retVal);
	}

	/**
	 * Sends a message via the message queue
	 * 
	 * @param msg      Message to send to the queue
	 * @param len      Number of bytes from the 'msg' buffer to send to the queue
	 * @param priority Optional message priority
	 * @return 0 if successfully registered for notification, -1 if error (use
	 *         errno for more error information)
	 */
	int QueueSend( const uint8_t *msg, size_t len, unsigned int priority=0)
	{
		return( QueueSend( (const char *)msg, len, priority));
	}

	/**
	 * Reads a message from the message queue
	 * 
	 * @param msg      Object to read the queue message into
	 * @param priority Optional int to receive the priority at which the received
	 *                 message was sent
	 * @return -1 if an error occurred or the number of bytes read from the queue
	 */
	int QueueReceive( T &msg, unsigned int *priority=NULL)
	{
		int			retVal = -1;
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			retVal = mq_receive( m_queueId, (char*)&msg, sizeof( msg), priority);

			// If we are registered for notifications
			if( m_wantsNotify)
			{
				// Re-register for notifications
				mq_notify( m_queueId, &m_notifyEvent);
			}
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in OpenQueue";
			ThrowOsException( queueErr, errStr);
		}
		
		return( retVal);
	}

	/**
	 * Checks if the queue is open
	 * 
	 * @return true if the queue is open, false otherwise
	 */
	bool IsOpen() const
	{
		return( m_queueId != -1);
	}

	/**
	 * Checks the number of messages currently waiting to be read from
	 * the queue
	 * 
	 * @return The number of messages currently waiting to be read from the 
	 *         queue or -1 on error (use errno for more error information)
	 */
	int	MssgsWaiting() const
	{
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			GetQueueAttr( m_queueAttrib);
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in OpenQueue";
			ThrowOsException( queueErr, errStr);
		}
		return( m_queueAttrib.mq_curmsgs);
	};


protected:
	
	/**
	 * Set or change the name of the message queue to the given name
	 * 
	 * @param name   Name of mesage queue to use
	 */
	void SetQueueName( const char *name)
	{
		int			queueErr;
		BepMutex	queueLock( m_queueLock);
		
		if( (queueErr=queueLock.Acquire()) == EOK)
		{
			// Only set/change name if not already open
			if( m_queueId == -1)
			{
				m_queueName = name;
			}

			if( m_queueName[ 0] != '/')
			{
				m_queueName = "/" + m_queueName;
			}
		}
		else
		{
			string errStr("Error locking queue '");
			errStr += m_queueName;
			errStr += "' in OpenQueue";
			ThrowOsException( queueErr, errStr);
		}
	}

	/**
	 * Set or change the name of the message queue to the given name
	 * 
	 * @param name   Name of mesage queue to use
	 */
	void SetQueueName( const string &name)
	{
		SetQueueName( name.c_str());
	}

	/**
	 * General message queue initialization.
	 */
	virtual void QueueInit()
	{
		memset( &m_queueAttrib, 0, sizeof( m_queueAttrib));
		memset( &m_notifyEvent, 0, sizeof( m_notifyEvent));
		m_queueAttrib.mq_msgsize = sizeof( T);
		m_queueAttrib.mq_maxmsg = 4096;
	}
private:

	/**
	 * Name of the message queue
	 */
	string					m_queueName;

	/**
	 * Message queue ID
	 */
	mqd_t	   				m_queueId;

	/**
	 * Message queue attributes
	 */
	mutable mq_attr_t 		m_queueAttrib;

	/**
	 * Flag to indicate whether or not we created the queue
	 */
	bool					m_creator;

	/**
	 * Event used to hold queue notification information
	 */
	struct sigevent			m_notifyEvent;

	/**
	 * Flag to indicate if we have registered for notifications
	 */
	bool					m_wantsNotify;

	/**
	 * Synchronization object used to protect against simultaneous changes
	 * to this queue object by two different threads
	 */
	mutable BepMutex		m_queueLock;
};


/**
 * Basic message queue object for character string messages
 */
typedef RawMessageQueue<MssgQueueMsg_t>		MessageQueue;

#endif // _MESSAGE_QUEUE_H_INCLUDED
