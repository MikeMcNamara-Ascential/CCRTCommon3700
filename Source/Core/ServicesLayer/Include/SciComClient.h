//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Include/SciComClient.h $
// $Author:: Rwiersem                                                         $
//   $Date:: 10/30/07 3:49p                                                   $
//
// Description:
//  Data type definitions for SCI2 communication client
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/SciComClient.h $
// 
// 3     10/30/07 3:49p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
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
// 1     6/12/03 4:21p Khykin
// Initial checkin.
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//
// 6     2/21/03 7:28p Bmeinke
// Fixed TransactMessage()
//
// 5     2/06/03 10:27a Bmeinke
// Removed m_muxCtrlPath and m_muxCtrlFd (not needed due to use of
// SciCommProxy)
// Added m_moduleResponseDelay (based on Bill Bereza's code)
// Added Initialize() method (based on Bill Bereza's code)
// Added GetResponseDelay() and SetResponseDelay() (based on Bill Bereza's
// code)
// Removed overloaded LockPort() and UnlockPort() methods
// Removed LockMux() and UnlockMux() methods
// WaitForFullResponse() now uses new ILogicalPort::ReadPort() method
// using timeouts
//
// 4     1/16/03 6:50p Bmeinke
// Fixed GetXmtMssgString()
//
// 3     9/24/02 7:14p Bmeinke
// Added WaitForFullResponse() to read data from server byte-by-byte until
// no data left to read
// TransactMessage() calls WaitForFullResponse() instead of ReadPort()
//
// 2     8/22/02 12:22p Bmeinke
// Replaced the slew of SendMessage() methods which took additional
// parameters (arg0...) with a single SendMessage() which takes a
// SerialString_t of characters to use when patching in values for
// wildcard characters in a XMT message string
// Added TransactMessage() methods (needed to ensure port is locked in SCI
// mode for duration of send/receive transaction)
// Added GetXmtString() and GetXmtTemplate() methods (used when patching
// in values for wildcard characters in a XMT message string)
//
// 1     8/08/02 4:34p Bmeinke
// Initial checkin
//
//******************************************************************************

#ifndef _SCI_COM_CLIENT_H_INCLUDED_
#define _SCI_COM_CLIENT_H_INCLUDED_

#include <string.h>

#include "ILogicalPort.h"
#include "XmlNode.h"


/**
 * Object used to interface clients to the SCI2 communication
 * server in order to communicate on the SCI2 serial bus
 *
 * @author Brian Meinke
 * @version 1
 * @see ILogicalPort
 * @since March 13, 2002
 */
class SciComClient : public ILogicalPort
{
public:
    SciComClient();

    ~SciComClient();

    /**
    * Initialize the serial channel.
    *
    * @param config The configuration to use for the channel initialization.
    * @return true if the initialization is successful.
    * @since Version 1.0
    */
    virtual bool Initialize(const XmlNode *config);

    inline const uint16_t &GetResponseDelay(void);

    
    inline void SetResponseDelay(const uint16_t delay);

    inline void SetResponseDelay(const std::string &delay);

    /**
     * Requests to be notified when data is received on the port
     * that matches the receive string contained in the reply message
     * section of the config file under the node with a tag equal to
     * the rcvTag value.
     *
     * @param event  Event to be used by the driver to deliver the notification events
     * @param rcvTag Message tag being subscribed for
     * @return true if successfully registered for notifications, false if an
     *         error occurred (use errno for more detailed error information)
     * @see NotifyEvent_t
     */
    bool FilterSubscribe(const NotifyEvent_t &event, const string &rcvTag);

    /**
     * Send a transmit string and get a response
     *
     * @param sendTag  Tag of the message to transmit
     * @param rcvTag   Tag for the expected response string
     * @param response Actual response data
     * @return true if the transaction was successful (data sent and received),
     *         false othgerwise
     */
    bool TransactMessage( const string &sendTag, const string &rcvTag, SerialString_t &response);

    /**
     * Send a transmit string and get a response
     *
     * @param sendTag  Tag of the message to transmit
     * @param sendArgs Array of arguments to use when replacing any wildcard
     *                 characters in the transmit string
     * @param rcvTag   Tag for the expected response string
     * @param response Actual response data
     * @return true if the transaction was successful (data sent and received),
     *         false othgerwise
     */
    bool TransactMessage( const string &sendTag, const SerialString_t &sendArgs,
                          const string &rcvTag, SerialString_t &response);

    /**
     * Send a transmit string and get a response
     *
     * @param sendMssg Message to send to the vehicle
     * @param response Actual response data
     * @return true if the transaction was successful (data sent and received),
     *         false othgerwise
     */
    bool TransactMessage( const SerialString_t &sendMssg, SerialString_t &response);

protected:

    /**
     * Sends the given serial string (array of unsigned chars) to the
     * destination port.
     * Throws BepException.
     *
     * @param sendMssg   Serial message to be sent
     * @param retryCount Number of times to re-send the message in case of
     *                   communication problems
     *
     * @return true if message sent properly, false if unable to send message
     */
    bool SendMessage( SerialString_t sendMssg, int retryCount=3);

    /**
     * Sends a formatted serial string (array of unsigned chars) to the
     * destination port. The string of bytes to be sent will be looked up
     * based on the tag given. The tag should represent a tag/message pair
     * from the configuration data with which this object is initialized.
     * Throws BepException.
     *
     * @param mssgTag    Tag of the message to be sent
     * @param retryCount Number of times to re-send the message in case of
     *                   communication problems
     * @param mssgArgs   Array of arguments to use when replacing any wildcard
     *                   characters in the transmit template for the given tranmit
     *                   tag
     *
     * @return true if message sent properly, false if unable to send message
     */
    bool SendMessage( const string &mssgTag, int retryCount=3,
                      SerialString_t mssgArgs = SerialString_t(0, 0));

    /**
     * Sends an array of bytes to the destination port and (optionally)
     * waits for a reply message from the vehicle
     *
     * @param sendBuff   Array of bytes to be sent
     * @param sendLen    Number of bytes to be sent from the sendBuff array
     * @param retryCount Number of times to re-send the message in case of
     *                   communication problems
     *
     * @return true if message sent properly, false if unable to send message
     */
    bool SendMessage( const uint8_t *sendBuff, uint8_t sendLen,
                      int retryCount=3);

    /**
     * Reads a vehicle response for the given messsage tag.
     *
     * @param mssgTag    The tag of the message to read a response for
     * @param respString The buffer into which the vehicle response will be read
     *
     * @return true if a valid response was received, false otherwise
     */
    bool GetResponse( const string &mssgTag, SerialString_t &respString);

    /**
     * Reads one character at a time from the logical port driver until no more 
     * data is available.
     *
     * @param response Buffer to read the response into.
     * @param timeout  Amount of time to wait for each data byte.
     * 
     * @return number of characters received or -1 on error
     */
    int WaitForFullResponse( SerialString_t &response, uint32_t timeout=5000);

    /**
     * Looks up and replaces any wildcard characters in the transmit
     * message string with the tag given by mssgTag. Wildcard characters
     * are replcaed using the bytes from the args string. The resulting
     * binary message string is placed in the mssgString parameter.
     *
     * @param mssgTag    Tag of the message template to use (as read from the
     *                   config fle)
     * @param mssgString The resulting fully formatted message string to be
     *                   transmitted to the
     *                   vehicle
     * @param args       String of bytes to use when replaing any replaceable
     *                   characters in the
     *                   message template
     * @return true if the resulting mssgString output is valid (the given tag
     *         was valid), false if the resulting mssgString is invalid
     */
    bool GetXmtMssgString( const string &mssgTag, SerialString_t &mssgString, const SerialString_t &args);

    /**
     * Finds the transmit template string for the given transmit
     * message tag
     *
     * @param mssgTag Tag of the transmit template string to lookup
     * @param xmtTmpl Transmit template string found for the given transmit message
     *                tag
     * @return true if the given tag is valid and a valid transmit template
     *         string was found, false otherwise
     */
    bool GetXmtTemplate( const string &mssgTag, string &xmtTmpl);

    /**
     * Checks if any response strings are defined for the given message tag
     *
     * @param mssgTag Message tag used to use when searching the message table for valid response string
     *                definitions
     * @return true if the given message tag has any response string defined
     *         false if no response strings are defined for the given message tag
     */
    bool NeedsResponse( const string &mssgTag);

private:

    /**
     * Time to wait for module to respond to a message.
     * @since Version 1.0
     */
    uint16_t            m_moduleResponseDelay;

};

#endif  // _SCI_COM_CLIENT_H_INCLUDED_
