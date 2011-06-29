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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/Include/KeywordProtocolFilter.h $
// 
// 5     10/25/06 3:30p Rwiersem
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
// 3     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     10/15/04 2:03p Bmeinke
// Override Initialize() to read the different negative response code
// values
// Override GetModuledata() so we can handle a "Busy, try again" response
// Added SetModuleBusyCode() and GetModuleBusyCode() methods
// Added m_moduleBusyCode data member
//
// 2     6/09/04 10:57p Cward
// Added default parameter to the constructor. Removed const from the
// parameter to the Extract Module Data method. Updated the header comment
// block to the new standard
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
//    Revision 1.1.1.1  2003/04/30 20:10:36  tenharmsel
//    Source code for CCRT Server
//
//
// 1     4/22/03 10:26a Khykin
// Initial Working Checkin.
//
// 1     4/11/03 7:33a Khykin
// Initial checkin.
//    Revision 1.1.1.1  2003/02/15 00:03:13  tbochene
//
//    PooBag
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
#ifndef KeywordProtocolFilter_h
#define KeywordProtocolFilter_h
//==============================================================================
#include "ProtocolFilter.h"

/**
 * Keyword Protocol 2000 filter.
 * This class will send and receive Keyword 2000 messages.
 * The ExtractModuleData() method has been instantiated to strip the header and
 * checksum from the module response.
 *
 * @author Dan Erickson
 * @author Ross Wiersma
 * @version Version 1.0
 * @see ProtocolFilter
 * @since December 17, 2002
 */
class KeywordProtocolFilter : public ProtocolFilter
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
    KeywordProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar = NULL, BepMutex *commsInUse = NULL);
    
    /**
     * Class destructor.
     * @since Version 1.0
     */
    virtual ~KeywordProtocolFilter();
    /**
     * Initialize the serial channel.
     *
     * @param config The configuration to use for the channel initialization.
     * @return true if the initialization is successful.
     * @since Version 1.0
     */
    virtual bool Initialize(const XmlNode *config);
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
     * Read a module response to the message with the given tag.
     *
     * @param messageTag Tag of the message response to read.
     * @param reply      Reply from the module.
     * @return Status of the operation.
     * @since Version 1.0
     */
    const BEP_STATUS_TYPE GetResponse(const std::string &messageTag, SerialString_t &reply);

    /**
     * Parse a received message and return the first full message that was
     * received.  Note:  This will remove the message from the moduleResponse
     * and leave only the remaining characters.
     *
     * @param moduleResponse Response from the module.
     * @return First message from the response.
     *         Note: If a valid message is not received, a blank string is returned
     */
    const SerialString_t ExtractMessage(SerialString_t &moduleResponse);
    /**
     * Send a message to the module and read the response.
     *
     * @param messageTag Tag of message to send.
     * @param reply      Reply from the module.
     * @param args       Optional data to be written to the module.
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE GetModuleData(std::string messageTag, SerialString_t &reply,
                                                SerialArgs_t *args = NULL);
    /**
     * Get the negative response code indicating the module is busy and to repeat the request
     *
     * @return Negative response code for module busy, repeat request
     */
    const UINT8& GetModuleBusyCode(void);
	/**
	 * Get the value read in from the config file, which indicates if RoutineNotComplete responses are allowed.
	 *
	 * @return allowRoutineNotComplete response allowance code
	 */
	const bool GetAllowRoutineNotComplete(void);
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
     * Check if the message received from the module is valid.
     *
     * @param moduleResponse
     *               Response received from the module
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE CheckForValidResponse(const SerialString_t &moduleResponse);

protected:
    /**
     * Set the negative response code indicating the module is busy and to repeat the request.
     *
     * @param  moduleBusyCode Negative response code for module busy, repeat request
     */
    void SetModuleBusyCode(const UINT8 &moduleBusyCode);
	/**
	 * Set the value read in from the config file, which indicates if RoutineNotComplete responses are allowed.
	 *
	 * @param  allowRoutineNotComplete response allowance code
	 */
	void SetAllowRoutineNotComplete(const bool allowRoutineNotComplete);

private:
    /**
     * Negative response code indicating the module is busy and to repeat the request.
     * @since Version 1.0
     */
    UINT8 m_moduleBusyCode;
	/**
	 * Boolean value read in from the config file, which indicates if RoutineNotComplete responses are allowed.
	 */
	bool m_allowRoutineNotComplete;
};

#endif //KeywordProtocolFilter_h
