//******************************************************************************
// Description:
//  Base ProtocolFilter for communicating with vehicle modules.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CMake Core 062909/Ford/Source/Core/CORE/ApplicationLayer/ProtocolFilters/Include/IsoProtocolFilter.h $
// 
// 1     2/07/11 4:27p Mmcnamar
// 
// 1     2/07/11 4:01p Mmcnamar
// c drive backup
// 
// 1     5/10/10 2:00p Mmcnamar
// initial check-in for new core.
// 
// 5     10/25/06 3:28p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a timer to tell when the last transmit message was sent.
// This
//       allows the keep alive message to be sent when needed instead of
// on a
//       specific time interval.
// 
// 4     5/11/05 10:03p Cward
// Updated for 05051101 iteration
//
// 3     5/09/05 5:00p Gpattison
// Added support for validation of checksum and also addition
// array bounds checking to prevent faults on incomplete messages
//
// 3     7/21/04 10:42a Cward
// Added defaulted Bep Mutex parameter to the constructor
//
// 2     6/09/04 10:55p Cward
// Changed parameter to Extract Module Data method, to not be a const.
// Updated the header comment block to the new standard
//
// 1     12/21/03 6:33p Cward
//
// 1     7/08/03 1:05p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:28p Khykin
// Initial checkin
//    Revision 1.1  2003/05/08 17:42:11  tenharmsel
//    Moved from the Include Directory to here
//
//    Revision 1.1.1.1  2003/05/07 15:13:34  tenharmsel
//    Bringing in the ProtocolFilter includes
//
//    Revision 1.1  2003/03/06 22:50:22  bereza
//    first stab
//
//
// 2     1/30/03 8:38a Derickso
// Added constructor and destructor.
// Correctd error in ExtractModuleData().
//
// 1     12/18/02 10:10a Derickso
// Created.  Not tested or compiled.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef IsoProtocolFilter_h
#define IsoProtocolFilter_h
//==============================================================================
#include "ProtocolFilter.h"

/**
 * Iso Protocol 2000 filter.
 * This class will send and receive Iso 2000 messages.
 * The ExtractModuleData() method has been instantiated to strip the header and
 * checksum from the module response.
 *
 * @author Dan Erickson
 * @author Ross Wiersma
 * @version Version 1.0
 * @see ProtocolFilter
 * @since December 17, 2002
 */
class IsoProtocolFilter : public ProtocolFilter
{
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
    IsoProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar = NULL, BepMutex *commsInUse = NULL);
    /**
     * Class destructor.
     * @since Version 1.0
     */
    virtual ~IsoProtocolFilter();
    /**
     * Initialize the serial channel.
     *
     * @param config The configuration to use for the channel initialization.
     * @return true if the initialization is successful.
     */
    virtual bool Initialize(const XmlNode *config);
    /**
     * Send a message to a vehicle module.
     *
     * @param message Message to be sent.
     * @return Status of the send.
     */
    virtual const BEP_STATUS_TYPE SendMessage(SerialString_t &message);
    /**
     * Send a message to vehicle module.
     *
     * @param messageTag Name of message to be retrieved and sent.
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE SendMessage(string messageTag);
    /**
     * Send a message to a vehicle module.
     *
     * @param messageTag Name of the message to be sent.
     * @param args       Extra data to be added to the message.
     * @return Status of the opeation.
     */
    virtual const BEP_STATUS_TYPE SendMessage(string messageTag, SerialArgs_t &args);
    /**
     * Extract the data from the response.
     *
     * @param moduleResponse
     *               Response from the module.
     * @return The data that was included in the response.
     * @since Version 1.0
     */
    const SerialString_t ExtractModuleData(SerialString_t &moduleResponse);
    /**
     * Check to ensure that checksum on module response
     * is valid
     *
     * @param moduleResponse
     *               entire module response from module
     *
     * @return true if checksum is valid, false if invalid
     */
    virtual bool IsChecksumValid(SerialString_t &moduleResponse);
    /**
     * Get the initial message after the wake up sequence.
     * 
     * @param byteCount Number of bytes expected from the module.
     * @param initialTxByte
     *                  Initial byte to transmit to the module.
     * @param message   Message from the module.
     * 
     * @return Status of processing the initial message.
     */
    virtual BEP_STATUS_TYPE GetInitialMessage(INT32 byteCount, UINT8 initialTxByte, SerialString_t &message);
    /**
     * Overloaded function that only adds a checksum if required
     * 
     * @param message Message to which the checksum will be added
     */
    void AddChecksumToMessage(SerialString_t &message);
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
     * Check if the message received from the module is valid.
     *
     * @param moduleResponse
     *               Response received from the module
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE CheckForValidResponse(const SerialString_t &moduleResponse);

private:
    /**
     * Get/Set the character signifying the end of a module command.
     * 
     * @param character Command trailer character.
     * 
     * @return Command trailer character.
     */
    const UINT8& CommandTrailerCharacter(const UINT8 *character = NULL);

    /**
     * Get/Set the flag signifying whether a checksum is required
     * 
     * @param reqChecksum Flag indicating if a checksum is required for each message.
     * 
     * @return Flag indicating if a checksum is required for each message.
     */
    const bool& RequiresChecksum(const bool *reqChecksum = NULL);
    /**
     * Get/Set the flag signifying whether to echo data back to the module.
     * 
     * @param echoData Flag indicating if the data received from the module should be echoed back to the module.
     * 
     * @return Flag indicating if the data received from the module should be echoed back to the module.
     */
    const bool& EchoModuleData(const bool *echoData = NULL);
    /**
     * Get/Set the byte that contains the module sync data.
     * 
     * @param byte   Message byte that contains the message sync data.
     * 
     * @return Message byte that contains the message sync data.
     */
    const UINT8& SyncByte(const UINT8 *byte = NULL);
    /**
     * Character signifying the end of a command.
     */
    UINT8 m_commandTrailer;
    /**
     * Message byte that contains the sync data.
     */
    UINT8 m_syncByte;
    /**
     * Sync data to use for tracking messages with the module.
     */
    UINT8 m_syncData;
    /**
     * Boolean variable stating whether a message checksum is required.
     */
    bool m_requiresChecksum;
    /**
     * Flag indicating if the data received from the module should be echoed back to the module.
     */
    bool m_echoModuleData;
};

#endif //IsoProtocolFilter_h
