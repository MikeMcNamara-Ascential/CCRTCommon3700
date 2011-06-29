//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/SerialChannel.h 2     1/16/04 4:45p Cward $
//
// FILE DESCRIPTION:
//  SerialChannel object that is used for sending and receiving serial
//	information in the system.
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
//    $Log: /Core/ServicesLayer/Include/SerialChannel.h $
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
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//
// 9     3/07/03 6:56a Khykin
// Updating for latest changes to the SerialChannel
//
// 8     1/14/03 4:53p Derickso
// Latest version.
//
// 7     11/08/02 8:29a Tbochene
// Removed type definition of SERIAL_STRING and replaced with
// SerialString_t defined in SerialDefs.h
//
// 6     9/16/02 11:18a Khykin
// Updating to include Send method with std::string parameter
//
// 5     9/06/02 11:26a Khykin
// Updating to add fault information.
//
// 4     6/13/02 3:01p Dan.erickson
// Added GetRemainingData() method.
//
// 3     5/28/02 3:30p Dan.erickson
// Base class is expecting unsigned char instead of char.
//
// 2     4/29/02 12:40p Kh
// Updating the Get method to return the number of bytes read.
//
// 1     4/02/02 7:31a Kh
// Initial checkin.
//
//
//*************************************************************************
#ifndef SERIALCHANNEL_H
#define SERIALCHANNEL_H
//==============================================================================
#include <string>

#include "ILogicalPort.h"
#include "BepTimer.h"

#define NOTIFICATION_PULSE 	0x12	/* pulse to use to notify self that there is data to read */

/**
 * Serial communication channel interface.  This provides
 * the initialization of a serial channel and the methods
 * to communicate on it.
 *
 * @author Kevin Hykin
 * @version Version 1.2
 */
class SerialChannel : public ILogicalPort
{
public:
	/**
	 * Serial Channel constructor.
	 *
	 * @since Version 1.0
	 */
	SerialChannel(void);
	/**
	 * Serial Channel destructor.
	 *
	 * @since Version 1.0
	 */
	virtual ~SerialChannel(void);
	/**
	 * Initialize the serial channel.
	 *
	 * @param config The configuration to use for the channel initialization.
	 * @return true if the initialization is successful.
	 * @since Version 1.0
	 */
	virtual bool Initialize( const XmlNode* config);
	/**
	 * Opens a logical port for communication
	 *
	 * @param portName Name of the logical port to be opened
	 * @param mode     Mode in which to open the port: O_RDONLY, O_WRONLY or
	 *                 O_RDWR. The default is O_RDWR.
	 * @return true if successfully opened, false on error
	 */
	virtual bool OpenPort(const char *portName, int mode=O_RDWR);
	/**
	 * Send a message on the serial port.
	 *
	 * @param txBuff The information to send on the serial port.
	 * @param length The length of the message to send.
	 * @return The status of the operation.
	 * @since Version 1.0
	 */
	BEP_STATUS_TYPE Send(unsigned char *txBuff, int length);
	/**
	 * Send a message on the serial port.
	 *
	 * @param txBuff The data to send out the serial port.
	 * @return The status of the operation.
	 * @since Version 1.0
	 */
	BEP_STATUS_TYPE Send(SerialString_t& txBuff);
	/**
	 * Send a message on the serial port.
	 * WARNING:  If the sent message contains a null (0x00) character
	 *           DO NOT USE THIS METHOD.  The data sent will only include
	 *           up to the null.
	 *
	 * @param txBuff The data to send out the serial port.
	 * @return The status of the operation.
	 * @since Version 1.0
	 */
	BEP_STATUS_TYPE Send(std::string& txBuff);
	/**
	 * Get the data that was received.
	 *
	 * @param rxBuff The buffer to place the received information into.
	 * @param size   The length of the buffer that the information is to be
	 *               placed into.
	 * @return The number of bytes read or -1 if a error occurred.
	 * @since Version 1.0
	 */
	INT32 Get(unsigned char *rxBuff, int size);
	/**
	 * Get the data that was received.
	 * WARNING:  If the return message can contain a null (0x00) character
	 *           DO NOT USE THIS METHOD.  The data will only be valid to the
	 *           first null byte received!
	 *
	 * @param message The message that was received.
	 * @return The number of bytes read or -1 if a error occurred.
	 * @since Version 1.0
	 */
	INT32 Get(std::string &message);
	/**
	 * Get the remaining data from the serial port in case it was not completely received with the
	 * previous Get() method.
	 *
	 * @param buffer      Buffer to place the received charcters in.
	 * @param buffSize    Size of the buffer.
	 * @param byteCount   The number of bytes read from the serial port.
	 * @param maxAttempts Number of attempts to get data before stopping.
	 * @param dataDelay   Time to wait in ms between checks for data if no data was present on the previous ReadPort() call.
	 * @since Version 1.1
	 */
	void GetRemainingSerialData(unsigned char *buffer,
                                const INT32 &buffSize,
                                INT32 &byteCount,
                                const INT32 &maxAttempts = 3,
                                const INT32 &dataDelay = 50);
	/**
	 * Get the name of the serial channel.
	 *
	 * @return The name of the serial channel.
	 * @since Version 1.0
	 */
	const std::string GetName(void);

	/**
	 * Get the timeout time used to request data.
	 *
	 * @return The communication timeout time.
	 *         Note: the value is in mS.
	 * @since Version 1.2
	 */
	const UINT64 GetTimeoutTime(void);

	/**
	 * Set the timeout time used to request data.
	 *
	 * @param timeout The timeout time to update the communication timeout too.
	 *                Note: the value is in mS.
	 * @since Version 1.2
	 */
	void SetTimeoutTime(UINT64 timeout);


    /**
	 * Set the name of the serial channel
	 *
	 * @param name   The name that the channel should use.
	 * @since Version 1.0
	 */
	inline void SetName(const std::string &name);

private:
	/**
	 * Name of the communication channel that is used by the SerialChannel.
	 * @since Version 1.0
	 */
	string m_name;
	/**
	 * Notification event to use when there is data on the
	 * port.
	 * @since Version 1.0
	 */
	NotifyEvent_t m_event;
	/**
	 * The channel id to use for receiving information.
	 * @since Version 1.0
	 */
	int m_channelId;
	/**
	 * The connection id of the open channel.
	 * @since Version 1.0
	 */
	int m_connectionId;
	/**
	 * Pulse to user for communication.
	 * @since Version 1.0
	 */
	struct _pulse   m_pulse;
	/**
	 * The size of the buffer to use for communication.
	 * @since Version 1.0
	 */
	size_t  m_buffSize;
	/**
	 * The buffer to user for communication.
	 */
	char *m_commBuffer;
	/**
	 * BepTimer to use for providing communication timeouts.
	 * @since Version 1.0
	 */
	BepTimer m_timer;
	/**
	 * The timeout time to user for receiving communications.
	 * @since Version 1.0
	 */
	UINT64 m_timeoutTime;
	/**
	 * Number of times that an operation should be retried
	 * (e.g. Intitialize)
	 * @since Version 1.2
	 */
	UINT32 m_retryCount;
};

#endif // SERIALCHANNEL_H
