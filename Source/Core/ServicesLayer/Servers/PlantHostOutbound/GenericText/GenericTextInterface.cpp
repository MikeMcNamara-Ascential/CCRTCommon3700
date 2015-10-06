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
#include "GenericTextInterface.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
GenericTextInterface::GenericTextInterface() : HostInterface()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
GenericTextInterface::~GenericTextInterface()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE GenericTextInterface::SendTestResultString(const string &resultString, 
                                                           SerialChannel &portComm, 
                                                           const INT32 &maxAttempts)
{
    INT32 currentAttempt = 1;                   // Keep track of attempts
    bool portLocked = false;                    // Port lock status
    string message(resultString);
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;  // Transmit status
    // Attempt to send the message
    do
    {   // Lock the port so only we have access to it
        portLocked = portComm.LockPort();
        // If the port is locked, transmit the message
        if(portLocked)
        {   // Transmit the message
            portComm.ResetConnection();
			// Make sure the port is connected
			portComm.ReconnectDriver();
            status = portComm.Send(message);
            // Log the status
            Log(LOG_ERRORS, "Sent message to host: %s", ConvertStatusToResponse(status).c_str());
            // Unlock the port for this attempt
            portLocked = portComm.UnlockPort();
        }
        else
        {   // Log the error
            Log(LOG_ERRORS, "WARNING: Could not lock %s port.  Not transmitting results\n", 
                portComm.GetProcessName().c_str());
            BposSleep(100);       //keep from locking up if port is not released to use.
        }
    // Keep trying while attempts remaining and failed transmit
    } while((BEP_STATUS_SUCCESS != status) && (currentAttempt++ <= maxAttempts));
    // Return the status
    return status;
}

//-----------------------------------------------------------------------------
void GenericTextInterface::SendResultToHost(const string &result)
{
    Log(LOG_DEV_DATA, "GenericTextInterface: Message to send to host system - %s", result.c_str());
    // Transmit the result string to the host system
    BEP_STATUS_TYPE status = SendTestResultString(result, *(HostComm()), MaxMsgSendAttempts());
    Log(LOG_ERRORS, "Sent result to host system: %", ConvertStatusToResponse(status).c_str());
}
