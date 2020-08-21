//******************************************************************************
// Description:
//  Base ProtocolFilter for communicating with vehicle modules.
//
//==============================================================================
// Copyright (c) 1999-2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/Include/ProtocolFilter.h $
// 
// 9     10/30/07 1:38p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 8     1/17/07 3:06p Rwiersem
// Changes for the 07011901 core release:
// 
// - Made the engine controller test class a friend.
// 
// 7     10/25/06 3:30p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a timer to tell when the last transmit message was sent.
// This
//       allows the keep alive message to be sent when needed instead of
// on a
//       specific time interval.
//     - Added OpenCom() and CloseCom()
// 
// 6     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 5     5/11/05 10:03p Cward
// Updated for 05051101 iteration
//
// 3     5/09/05 5:00p Gpattison
// Added support for validation of checksum and also addition
// array bounds checking to prevent faults on incomplete messages
//
// 4     6/09/04 11:01p Cward
// Removed Implementation from the file by refactoring. Updated the header
// comment block to the new standard.
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 1     7/08/03 1:05p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:28p Khykin
// Initial checkin
//    Revision 1.3  2003/05/20 16:13:38  tenharmsel
//    Fixed implementations of LockPort to return true, compiler was complaining
//
//    Revision 1.2  2003/05/08 20:25:25  tenharmsel
//    Added builds for the individual customer's modules, also added a VehicleTest makefile
//
//    Revision 1.1  2003/05/08 17:42:11  tenharmsel
//    Moved from the Include Directory to here
//
//    Revision 1.1.1.1  2003/05/07 15:13:34  tenharmsel
//    Bringing in the ProtocolFilter includes
//
//    Revision 1.1.1.1  2003/04/30 20:10:25  tenharmsel
//    Source code for CCRT Server
//
//
// 1     4/22/03 10:26a Khykin
// Initial Working Checkin.
//
// 1     4/11/03 7:33a Khykin
// Initial checkin.
//    Revision 1.2  2003/02/23 16:51:53  tbochene
//    New protocol filter stuff
//
//
// 4     2/10/03 2:37p Derickso
// Split functionality into seperate file.
//
// 2     1/30/03 8:40a Derickso
// Added constructor and destructor.
// Numerous updates from debugging and testing.
//
// 1     12/18/02 10:09a Derickso
// Created.  Not tested or compiled.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef ProtocolFilter_h
#define ProtocolFilter_h
//==============================================================================
#include <stdint.h>
#include <sys/neutrino.h>
#include "SerialChannel.h"
#include "IProtocolFilter.h"
#include "BepSync.h"

/**
 * Object used to hold the time of the last transmit operation
 */
typedef BepCondVar<uint64_t>    KeepAliveTimer_t;
/**
 * Object used to hold the value of the stopCommsFlag 
 */
typedef BepCondVar<bool>    StopCommsBepCondVar;

const string ChecksumCalculationAdd("Add");
const string ChecksumCalculationXor("Xor");

/**
 * Base protocol filter class.
 * This class will send messages to vehicle modules and receive the module response.
 * The ExtractModuleData() method must be instantiated for each different protocol.
 * The method ideally extracts only the data from the response.
 *
 * @author Dan Erickson
 * @author Ross Wiersma
 * @version Version 1.0
 * @see SerialChannel
 * @see ILogicalPort
 * @since December 17, 2002
 */
class ProtocolFilter : public IProtocolFilter, public SerialChannel
{
    friend class ProtocolFilterTest;
    friend class EngineControllerTest;

public:
    /**
     * Class Constructor
     *
     * @param lastTxTime Variable used to hold the time of the last transmit operation. This is
     *                   designed to be a shared timestamp that all the protocol filters for
     *                   this module interface use so the module keep alive knows how long
     *                   it has been since the last string was sent to the module so as not to
     *                   send unnecessary keep alive messages
     * @param commsInUse Semaphore to allow only one thread in the process to utilize the comm port at a time.
     * @since Version 1.0
     */
    ProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar = NULL, BepMutex *commsInUse = NULL);
    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~ProtocolFilter();
    /**
     * Initialize the serial channel.
     *
     * @param config The configuration to use for the channel initialization.
     * @return true if the initialization is successful.
     * @since Version 1.0
     */
    virtual bool Initialize(const XmlNode *config);
    /**
     * Opens the serial connection
     * 
     * @return true if the serial connection was opened successfully
     */
    virtual bool OpenCom();
    /**
     * Closes the serial connection
     */
    virtual void CloseCom();
    /**
     * Send a message to a vehicle module.
     *
     * @param message Message to be sent.
     * @return Status of the send.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(SerialString_t &message);
    /**
     * Send a message to vehicle module.
     *
     * @param messageTag Name of message to be retrieved and sent.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag);
    /**
     * Send a message to a vehicle module.
     *
     * @param messageTag Name of the message to be sent.
     * @param args       Extra data to be added to the message.
     * @return Status of the opeation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag, SerialArgs_t &args);
    /**
     * Broadcast a message on the vehicle bus.
     *
     * @param messageTag Tag for the message to be broadcast.
     *
     * @return Status of the broadcast.
     */
    virtual const BEP_STATUS_TYPE SendBusBroadcastMessage(string messageTag);
    /**
     * Read a module response to the message with the given tag.
     *
     * @param messageTag Tag of the message response to read.
     * @param reply      Reply from the module.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE GetResponse(const std::string &messageTag, SerialString_t &reply);
    /**
     * Read a module response.
     *
     * @param reply      Reply from the module.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE GetResponse(SerialString_t &reply);
    /**
     * Send a message to the module and read the response.
     *
     * @param messageTag Tag of message to send.
     * @param reply      Reply from the module.
     * @param args       Optional data to be written to the module.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE GetModuleData(std::string messageTag,
                                                SerialString_t &reply,
                                                SerialArgs_t *args = NULL);
	/**
     * Send a message to the module and read the response.
     *
     * @param messageTag Tag of message to send.
     * @param reply      Reply from the module.
     * @param args       Optional data to be written to the module.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE GetPGNModuleData(std::string messageTag,
                                                SerialString_t &reply,
                                                SerialArgs_t *args = NULL);
    /**
     * Wait for a message broadcast on the vehicle bus.
     *
     * @param messageTag Message to look for on the vehicle bus.
     * @param messageWaitTime
     *                   Amount of time to wait for the broadcast message.
     * @param busMssg    Location to store the broadcast message.
     *
     * @return Status of waiting for the broadcast message.
     */
    virtual const BEP_STATUS_TYPE GetBusBroadcastMessage(string messageTag,
                                                         const long messageWaitTime,
                                                         SerialString_t &busMssg);
    /**
     * Add the checksum to the message prior to transmitting the message to the module.
     *
     * @param message Message to be sent.
     * @since Version 1.0
     */
    virtual void AddChecksumToMessage(SerialString_t &message);
    /**
     * Calculate the checksum for the provided message.
     *
     * @param message Message to calculate the checksum for
     * @return Checksum of the provided message.
     * @since Version 1.0
     */
    virtual UINT8 ComputeCheckSum(const SerialString_t &message);
    /**
     * Subscribe for a message with the specified filter.
     *
     * @param messageTag Tag of the message to subscribe for.
     * @param event      event to wait on.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE FilterSubscribe(const std::string &messageTag, struct sigevent *event);
    /**
     * Upload a filter byte to filter for a message.
     *
     * @param filterVal Filter value to upload.
     * @param filterPosition
     *                  Position of filter byte in message.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE UploadFilterByte(UINT8 &filterVal, UINT32 &filterPosition);
    /**
     * Close the connection to the port.
     * @since Version 1.0
     */
    virtual void Close(void);

    /**
     * Extract the data from the response.
     *
     * @param moduleResponse
     *               Response from the module.
     * @return The data that was included in the response.
     * @since Version 1.0
     */
    virtual const SerialString_t ExtractModuleData(SerialString_t &moduleResponse) = 0;

    /**
     * Request to lock the port for exclusive use. The flags parameter
     * allows you to specify a blocking mode for this operation: If O_NONBLOCK
     * is specified and the port is already locked by another process, an error
     * is returned. If O_NONBLOCK is NOT specified, this operation will block
     * until the port can be locked.
     *
     * @param flags  Optional flags argument.
     * @return true if the port was successfully locked, false if an error
     *         occurred (use errno for more detailed error information)
     */
    virtual bool LockPort(int flags = 0);

    /**
     * Request to unlock a port. A successful call to LockPort() must have
     * been issued for this method to succeed.
     *
     * @return true if port was unlocked, false if an error occurred (use errno
     *         for more detailed error information)
     */
    virtual bool UnlockPort();

    /**
     * Fills in any wildcard characters in a transmit byte string with
     * serial byte values from the arguments provided.
     *
     * @param xmtMessage Message to replace wild card characters in.
     * @param args       Arguments to fill in the message.
     * @return True if the transmit interpretation bytes were filled in,
     *         false otherwise
     * @since Version 1.0
     */
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
    virtual bool GetXmtMssgString( const string &mssgTag, SerialString_t &mssgString,
                                   const SerialArgs_t &args);
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
    virtual bool GetXmtTemplate( const string &mssgTag, string &xmtTmpl);
    /**
     * Check if the message received from the module is valid.
     *
     * @param moduleResponse
     *               Response received from the module
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE CheckForValidResponse(const SerialString_t &moduleResponse);
    virtual const BEP_STATUS_TYPE CheckForValidUUDTResponse(const SerialString_t &moduleResponse);
    /**
     * Check if we received a negative response from the module.
     *
     * @param moduleResponse
     *               Response received from the module
     * @return Status of the operation
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE CheckForNegativeResponse(const SerialString_t &moduleResponse);
    /**
     * Delay for the specified keep alive time and then
     * send the keep alive message.
     *
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendKeepAlive(const std::string &keepAliveMessageTag, const bool &keepAliveResponseRequired);
    /**
     * Get the ASCII message based on the passed in message tag.
     *
     * @param messageTag Message Tag to look up.
     * @param message    ASCII message.
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE GetMessage(const std::string &messageTag, std::string &message);
    /**
     * Get the number of retries based on the passed in message tag.
     *
     * @param messageTag Message Tag to look up.
     * @param message    ASCII message.
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE GetResponseFailureRetryCount(const std::string &messageTag, INT32 &messageRetries);
    /**
     * Get the number of response pending reads based on the passed in message tag.
     *
     * @param messageTag Message Tag to look up.
     * @param message    ASCII message.
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE GetResponsePendingReads(const std::string &messageTag, INT32 &responsePendingReads);
    /**
     * Get the index in the response where the data actually begins.
     *
     * @return Offset into the response for the data.
     * @since Version 1.0
     */
    virtual const UINT16 &GetDataStartIndex(void);
    /**
     * Get the number of bytes that make up the response header.
     *
     * @return Number of header bytes.
     * @since Version 1.0
     */
    virtual const UINT16 &GetDataHeaderLength(void);
    /**
     * Get the number of bytes that make up the checksum.
     *
     * @return Number of bytes in the checksum.
     * @since Version 1.0
     */
    virtual const UINT16 &GetChecksumLength(void);
    /**
     * Get the number of times to resend a failed message before returning a failure.
     *
     * @return Number of attempts to successfully send a message.
     * @since Version 1.0
     */
    virtual const UINT16 &GetNumberOfRetries(void);
    /**
     * Get the message identifier that was sent to the module.
     *
     * @return Message Identifier.
     * @since Version 1.0
     */
    virtual const UINT16 &GetSentMessageIdentifier(void);
    /**
     * Get the message identifier index.
     *
     * @return Message Identifier.
     * @since Version 1.0
     */
    virtual const UINT16 &GetSentMessageIdentifierIndex(void);
    /**
     * Get the offset for a valid response.
     * For example, if command 0x21 is sent to the module, the module should respond
     * with command 0x61 if the offset is set to 0x40.
     *
     * @return The offset for a valid response.
     * @since Version 1.0
     */
    virtual const UINT16 &GetValidResponseOffset(void);
    /**
     * Get the value that indicates the module response is a negative response.
     *
     * @return The negative response value.
     * @since Version 1.0
     */
    virtual const UINT16 &GetNegativeResponseValue(void);
    /**
     * Get the index of the negative response code.
     *
     * @return Index of the negative response code.
     * @since Version 1.0
     */
    virtual const UINT16 &GetNegativeResponseCodeIndex(void);
    /**
     * Return the time to wait for the module to send a response.
     *
     * @return Time to delay in ms.
     * @since Version 1.0
     */
    virtual const UINT16 &GetResponseDelay(void);
    /**
     * Return the time to wait for the last byte of a response.
     *
     * @return Time to wait in ms.
     * @since Version 1.1
     */
     virtual const UINT16 &GetResponseEndDelay(void);
    /**
     * Get the reason for the specified negative response code.
     *
     * @param code   Negative response code from the module.
     * @return Text interpretation of the negative response code.
     * @since Version 1.0
     */
    virtual const std::string GetNegativeResponseReason(const UINT8 &code);
    /**
     * Return the member variable that indicates if the
     * Checking of the module response check sum is required
     *
     * @return true if validation of checksum is required, false if
     *         not required
     */
    virtual bool IsChecksumValidationRequired(void);
    /**
     * Check to ensure that checksum on module response
     * is valid.  No actual checksum validation done in
     * base class implementation.  If required by
     * specific protocol filter, it should be overloaded.
     *
     * @param moduleResponse
     *               entire module response from module
     *
     * @return true because no validation is actually done in base class
     */
    virtual bool IsChecksumValid(SerialString_t &moduleResponse);

    /**
     * Returns the timestamp of the last transmit operation
     * 
     * @return The timestamp of the last transmit operation
     */
    const uint64_t& GetLastTxTime() const;

    /**
     * Returns the timestamp of the last transmit operation
     * 
     * @return The timestamp of the last transmit operation
     */
    uint64_t GetLastTxTime();
    
    /**
     * Returns the value of m_stopCommsFlag
     * 
     * @return m_stopCommsFlag  bool to determine if we should continue sending vehicle messages
     */
    bool GetStopCommsFlag();

    /**
     * Update m_stopCommsFlag
     * 
     * @param stopCommsFlag bool to determine if we should continue sending vehicle messages
     */
    void SetStopCommsFlag( const bool &stopCommsFlag);
    
protected:
    /**
     * Reads one character at a time from the logical port driver until
     * no more data is available (using a 100ms timeout)
     *
     * @param response Buffer to read the response into
     * @return number of characters received or -1 on error
     */
    virtual int WaitForFullResponse( SerialString_t &response);
    /**
     * Save the index into the module response where the data begins.
     *
     * @param index  Index of the start of data.
     * @since Version 1.0
     */
    virtual void SetDataStartIndex(const UINT16 index);
    /**
     * Save the index into the module response where the data begins.
     *
     * @param index  Index of the start of data.
     * @since Version 1.0
     */
    virtual void SetDataStartIndex(const std::string &index);
    /**
     * Save the number of bytes in the header.
     *
     * @param length Number of header bytes.
     * @since Version 1.0
     */
    virtual void SetDataHeaderLength(const UINT16 length);
    /**
     * Save the number of bytes in the header.
     *
     * @param length Number of header bytes.
     * @since Version 1.0
     */
    virtual void SetDataHeaderLength(const std::string &length);
    /**
     * Save the number of checksum bytes in the module response.
     *
     * @param length Number of checksum bytes.
     * @since Version 1.0
     */
    virtual void SetChecksumLength(const UINT16 length);
    /**
     * Save the number of checksum bytes in the module response.
     *
     * @param length Number of checksum bytes.
     * @since Version 1.0
     */
    virtual void SetChecksumLength(const std::string &length);
    /**
     * Save the number of attmepts to successfully send/receive a message.
     *
     * @param retries Number of attempts.
     * @since Version 1.0
     */
    virtual void SetNumberOfRetries(const UINT16 retries);
    /**
     * Save the number of attmepts to successfully send/receive a message.
     *
     * @param retries Number of attempts.
     * @since Version 1.0
     */
    virtual void SetNumberOfRetries(const std::string &retries);
    /**
     * Save the identifier for the message that was just sent to the module.
     *
     * @param identifier Message identifier.
     * @since Version 1.0
     */
    virtual void SetSentMessageIdentifier(const UINT16 identifier);
    /**
     * Save the identifier for the message that was just sent to the module.
     *
     * @param index The sent message indentifier index.
     */
    virtual void SetSentMessageIdentifierIndex(const UINT16 index);
    /**
     * Store the index of the message identifier in the module response.
     *
     * @param index  Index of the message identifier.
     * @since Version 1.0
     */
    virtual void SetSentMessageIdentifierIndex(const std::string &index);
    /**
     * Save the offset that indicates a valid response from the module.
     * This number is added to the message identifier and compared to the identifier
     * in the module response.
     *
     * @param offset Message identifier offset for a valid response.
     * @since Version 1.0
     */
    virtual void SetValidResponseOffset(const UINT16 offset);
    /**
     * Save the offset that indicates a valid response from the module.
     * This number is added to the message identifier and compared to the identifier
     * in the module response.
     *
     * @param offset Message identifier offset for a valid response.
     * @since Version 1.0
     */
    virtual void SetValidResponseOffset(const std::string &offset);
    /**
     * Save the value that indicates a negative response from the module.
     *
     * @param value  Negative response value.
     * @since Version 1.0
     */
    virtual void SetNegativeResponseValue(const UINT16 value);
    /**
     * Save the value that indicates a negative response from the module.
     *
     * @param value  Negative response value.
     * @since Version 1.0
     */
    virtual  void SetNegativeResponseValue(const std::string &value);
    /**
     * Store the index of the negative response code.
     *
     * @param index  Index of the negative response code.
     * @since Version 1.0
     */
    virtual  void SetNegativeResponseCodeIndex(const UINT16 index);
    /**
     * Store the index of the negative response code.
     *
     * @param index  Index of the negative response code.
     * @since Version 1.0
     */
    virtual  void SetNegativeResponseCodeIndex(const std::string &index);
    /**
     * Save the maximum time to wait for a response from the module.
     *
     * @param responseDelay  Maximum time to wait for a response(in ms).
     * @since Version 1.0
     */
    virtual  void SetResponseDelay(const UINT16 responseDelay);
    /**
     * Save the maximum time to wait for a response from the module.
     *
     * @param responseDelay  Maximum time to wait for a response(in ms).
     * @since Version 1.0
     */
    virtual  void SetResponseDelay(const std::string &responseDelay);
    /**
     * Save the time to wait for the last byte of a response.
     *
     * @param responseDelay Maximum time to wait for the last byte of a 
     *                      response(in ms).
     */
    virtual  void SetResponseEndDelay(const UINT16 responseDelay);
    /**
     * Save the negative response codes and the translation for each code.
     *
     * @param codes  The codes and translations to be stored.
     * @since Version 1.0
     */
    virtual  void StoreNegativeResponseCodes(const XmlNode *codes);

    /**
     * Update the timestamp of the last transmit operation
     * 
     * @param lastTxTime Timestamp of the last transmit operation
     */
    void SetLastTxTime( const uint64_t &lastTxTime);

    /**
     * Update the timestamp of the last transmit operation to the current
     * time
     */
    void SetLastTxTime();
    
    /**
     * Get/Set the method for calculating the message checksum.
     * 
     * @param method Method of calculating the message checksum.
     * 
     * @return Method of calculating the message checksum.
     */
    const string ChecksumCalculationMethod(const string *method = NULL);
    /**
     * Index into the module response where the data begins.
     * @since Version 1.0
     */
    UINT16 m_dataStartIndex;
    /**
     * Length of the header.
     * @since Version 1.0
     */
    UINT16 m_dataHeaderLength;
    /**
     * Length of the checksum.
     * @since Version 1.0
     */
    UINT16 m_checkSumLength;
    /**
     * Number of message retries
     * @since Version 1.0
     */
    UINT16 m_numberOfRetries;
    /**
     * Message indentifier for the sent message.
     * @since Version 1.0
     */
    UINT16 m_sentMessageIdentifier;
    /**
     * Index of the message identifier in the module response.
     * @since Version 1.0
     */
    UINT16 m_sentMessageIdentifierIndex;
    /**
     * The offset from the sent message identifier that indicates a valid
     * message.
     * @since Version 1.0
     */
    UINT16 m_validResponseOffset;
    /**
     * Value that determines a negative response.
     * @since Version 1.0
     */
    UINT16 m_negativeResponseValue;
    /**
     * Index into response of the reason for the negative response.
     * @since Version 1.0
     */
    UINT16 m_negativeResponseCodeIndex;
    /**
     * Time to wait for module to respond to a message.
     * @since Version 1.0
     */
    UINT16 m_moduleResponseDelay;
    /**
     * Time to wait for last byte of the response from the module
     * @since Version 1.1
     */
     UINT16 m_moduleResponseEndDelay;
    /**
     * Map of the Negative Response Codes the translation for each code.
     * @since Version 1.0
     */
    XmlNodeMap m_negativeResponseCodes;
    /**
     * Pointer to the comm port synchronization object.
     * @since Version 1.0
     */
    BepMutex *m_commsInUse;
    /**
     * flag used to determine if the checksum should be validated
     * or not for each module response
     */
    bool m_checksumValidationRequired;

private:
    /**
     * Variable used to hold the time of the last transmit operation. This is
     * designed to be a shared timestamp that all the protocol filters for
     * this module interface use so the module keep alive knows how long
     * it has been since the last string was sent to the module so as not to
     * send unnecessary keep alive messages
     */
    KeepAliveTimer_t &m_lastTxTime;
    /**
     * Variable used to hold the value of the stop comms flag. This is
     * designed to be a shared flag that all the protocol filters for
     * this module interface use so the module knows if it is time to stop
     * sending messages such as an abort situation
     */
    StopCommsBepCondVar *m_stopCommsBepCondVar;
    /** The method for calculating the checksum of the message */
    string m_checkSumCalculation;
};

#endif //ProtocolFilter_h

