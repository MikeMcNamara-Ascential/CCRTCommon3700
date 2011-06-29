//******************************************************************************
// Description:
//   Host Interface base lass to process a generic text result with no header or checksum
//   and send to a host system.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericTextInterface_h
#define GenericTextInterface_h
//-----------------------------------------------------------------------------
#include "HostInterface.h"
//-----------------------------------------------------------------------------


/**
 * Generic Text Interface class.
 * This class will process the test result data and create a generic string.
 * No special header will be added to the result string.  No trailer data or checksum will be
 * added to the result string.  The result string will be transmitted out the port.  No ACK
 * or response is expected from the host system.  If the message is successfully transmitted,
 * it will be assumed the host system received the message.
 * 
 * @author Dan Erickson
 * @see HostInterface
 * @since March 10, 2008
 */
class GenericTextInterface : public HostInterface
{
public:
    /**
     * Class constructor.
     */
    GenericTextInterface();
    /**
     * Class destructor.
     */
    virtual ~GenericTextInterface();

protected:
    /**
     * Send the test result string to the external host.
     *
     * @param resultString
     *                 Test result string to be sent.
     * @param portComm Port object to send the test result string.
     * @param maxAttempts Maximum number of times to try to successfully send
     *                    the message.
     *
     * @return Status of the message transmit operation.
     */
    BEP_STATUS_TYPE SendTestResultString(const string &resultString, 
                                         SerialChannel &portComm, 
                                         const INT32 &maxAttempts);
    /**
     * Transmit the result string to the plant host system system.
     *
     * @param result The result string to be sent.
     */
    virtual void SendResultToHost(const string &result);
};

//-----------------------------------------------------------------------------
#endif //GenericTextInterface_h
