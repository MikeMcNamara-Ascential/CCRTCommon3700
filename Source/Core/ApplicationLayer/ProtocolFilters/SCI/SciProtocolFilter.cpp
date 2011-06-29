//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/SCI/SciProtocolFilter.cpp 3     10/25/06 3:40p Rwiersem $
//
// FILE DESCRIPTION:
//  SCI Protocol Filter for communicating with vehicle modules.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/SCI/SciProtocolFilter.cpp $
// 
// 3     10/25/06 3:40p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a timer to tell when the last transmit message was sent.
// This
//       allows the keep alive message to be sent when needed instead of
// on a
//       specific time interval.
// 
// 2     5/25/04 3:21p Cward
// Add timeout handling in Send Message
//
// 1     4/28/04 10:58p Cward
//
// 1     1/27/04 9:31a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
//
// 3     1/20/04 6:43a Derickso
// Updated Semaphore to BepSemaphore.
//
// 2     1/10/04 9:02a Derickso
// Latest updates from development testing.
//
// 1     12/17/03 11:53a Derickso
// Created.  New SCI protocol filter files from Greg.
//
//*************************************************************************
#include "SciProtocolFilter.h"

SciProtocolFilter::SciProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar, BepMutex *commsInUse /*=NULL*/) : ProtocolFilter(lastTxTime, stopCommsBepCondVar, commsInUse)
{
};

SciProtocolFilter::~SciProtocolFilter()
{
};

bool SciProtocolFilter::Initialize(const XmlNode *config)
{   // Get the baud rates for high and low speed SCI communications
    try
    {   // Get the high speed baud rate for SCI communications
        SetHighSpeedMessageBaud(BposReadInt(config->getChild("SerialMessages/TransmitProtocols/HighSpeedTransmit/Baud")->getValue().c_str()));
    }
    catch (XmlException &err)
    {
        Log(LOG_ERRORS, "XmlException saving HighSpeedTransmit Baud - using 62500: %s\n", err.what());
        SetHighSpeedMessageBaud(62500);
    }
    try
    {   // Get the low speed baud rate for SCI communications
        SetLowSpeedMessageBaud(BposReadInt(config->getChild("SerialMessages/TransmitProtocols/LowSpeedTransmit/Baud")->getValue().c_str()));
    }
    catch (XmlException &err)
    {
        Log(LOG_ERRORS, "XmlException saving LowSpeedTransmit Baud - using 7612: %s\n", err.what());
        SetLowSpeedMessageBaud(7612);
    }
    // Call the base class to complete the initialization
    return ProtocolFilter::Initialize(config);
}

const BEP_STATUS_TYPE SciProtocolFilter::GetBaudRate(const std::string &messageTag, speed_t &baudRate)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    ComMessageMapItr_t  itr;
    if ((itr = m_mssgTags.find(messageTag)) != m_mssgTags.end())
    {   // Get baud rate for message
        ComMssgTableEntry &mssgEntry = itr->second;
        baudRate = mssgEntry.GetXmtProtocol().GetBaud();
        if(baudRate != m_highSpeedBaudRate)
        {
            baudRate = m_lowSpeedBaudRate;
        }
        status = BEP_STATUS_SUCCESS;
    }
    else status = BEP_STATUS_FAILURE;
    // return status of method
    return status;
}
const BEP_STATUS_TYPE SciProtocolFilter::SendMessage(SerialString_t &message)
{   // Clear the Fifos so bad data is not gathered
    Log(LOG_DETAILED_DATA, "Resetting The Connection\n");
    ResetConnection();
    Log(LOG_DETAILED_DATA, "Reset The Connection\n");
    sigevent event;                         // Event structure informing what to do if timed out
    event.sigev_notify = SIGEV_UNBLOCK;     // Unblock if time out
    uint64_t timeout = mSEC_nSEC(200);      // Set timeout value - 500ms
    TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_SEND | _NTO_TIMEOUT_REPLY | _NTO_TIMEOUT_RECEIVE, &event, &timeout, NULL);
    // Send the message and return the result
    PrintSerialString( "ProtocolFilter Sending", message);
    INT32 bytesSent = ILogicalPort::WritePort(message, GetNumberOfRetries());
    Log(LOG_DETAILED_DATA, "Sent the message\n");
    // Determine the result of the write
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if (bytesSent < 0)       status = BEP_STATUS_ERROR;
    else if (bytesSent == 0) status = BEP_STATUS_FAILURE;
    else if (bytesSent > 0)  status = BEP_STATUS_SUCCESS;
    // Return the status
    return status;
}

const BEP_STATUS_TYPE SciProtocolFilter::SendMessage(std::string messageTag)
{   // Call base class
    return ProtocolFilter::SendMessage(messageTag);
}

const BEP_STATUS_TYPE SciProtocolFilter::SendMessage(std::string messageTag, SerialArgs_t &args)
{   // Call base class
    return ProtocolFilter::SendMessage(messageTag, args);
}

const BEP_STATUS_TYPE SciProtocolFilter::GetModuleData(std::string messageTag, SerialString_t &reply,
                                                       SerialArgs_t *args)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    std::string asciiMessage;
    SerialString_t xmtMessage;
    UINT16 tries = GetNumberOfRetries() + 1;
    bool portLocked = false;
    if (GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
    {   // Convert the message to binary
        GetBinaryMssg(asciiMessage, xmtMessage);
            // Lock the port so only this thread uses it
            Log(LOG_DETAILED_DATA, "GetModuleData Locking Port\n");
            if ((portLocked = LockPort()) == true)
            {   // The port was locked
                Log(LOG_DETAILED_DATA, "GetModuleData Locked Port\n");
                // Check/Set module and serial port baud rate for message
                if(SetMessageSpeed(messageTag) == BEP_STATUS_SUCCESS)
                {
                    do
                    {   // Send the message to the module
                        if (args == NULL) status = SendMessage(messageTag);
                        else              status = SendMessage(messageTag, *args);
                        Log(LOG_DEV_DATA, "Sent message: %s to module - status: %s\n", messageTag.c_str(),
                            ConvertStatusToResponse(status).c_str());
                        // If response is expected, GetResponse
                        if(IsResponseExpected(messageTag))
                        {
                            // Set the message ID
                            SetSentMessageIdentifier(xmtMessage[GetDataStartIndex()]);
                            // Clear the response area
                            reply.erase();
                            // If send was successful, get the module response
                            if (BEP_STATUS_SUCCESS == status) status = GetResponse(messageTag, reply);
                            // If this is not a valid message, wait a bit before trying again
                            if (BEP_STATUS_SUCCESS != status) BposSleep(GetResponseDelay());
                        }
                        else
                        {
                            // If response NOT expected, exit do-while loop
                            status = BEP_STATUS_SUCCESS;
                        }
                    }
					while ((BEP_STATUS_SUCCESS != status) && (tries-- > 0) && !GetStopCommsFlag());
				}
                else
                {   // Message Comms Baud Rate not set correctly
                    Log(LOG_ERRORS, "Baud Rate Failure: %s\n", messageTag.c_str());
                    status = BEP_STATUS_FAILURE;
                }
            }
            else Log(LOG_ERRORS, "Could not lock the port for message: %s because: %s\n", messageTag.c_str(), strerror(errno));
            // Finished with the port for this attempt, unlock for others
            if (portLocked)
            {   // Unlock the port
                Log(LOG_DETAILED_DATA, "GetModuleData UnLocking Port\n");
                bool portUnlocked = false;
                UINT8 attempts = 5;
                while (((portUnlocked = UnlockPort()) == false) && attempts--)
                {
                    Log(LOG_ERRORS, "Error unlocking port - attempt: %d , Reason: %s\n", attempts, strerror(errno));
                    BposSleep(10);
                }
                if (!portUnlocked)  Log(LOG_ERRORS, "WARNING: Port was NOT UNLOCKED!!!!!!!!!!!!!!!\n");
                else                Log(LOG_DETAILED_DATA, "GetModuleData UnLocked Port\n");
            }

            if(IsResponseExpected(messageTag))
            {
                // Check if valid message was retrieved
                if ((status == BEP_STATUS_SUCCESS) && (reply.length() > 0))
                {   // Verify a valid response was received
                    status = CheckForValidResponse(reply);
                    // Extract the data from the response
                    if (BEP_STATUS_SUCCESS == status) reply = ExtractModuleData(reply);
                }
                else if (status != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_ERRORS, "Error getting data from module: Message - %s, status: %s\n",
                        messageTag.c_str(), ConvertStatusToResponse(status).c_str());
                }
                else if (reply.length() <= 0)
                {
                    Log(LOG_ERRORS, "Did not receive a valid response from the module\n");
                    status = BEP_STATUS_FAILURE;
                }
            }
            else
            {
                status = BEP_STATUS_SUCCESS;
            }
    }
    else
    {   // No message provided for this tag
        Log(LOG_ERRORS, "No message provided for tag: %s\n", messageTag.c_str());
        status = BEP_STATUS_SOFTWARE;
    }
    Log(LOG_DETAILED_DATA, "GetModuleData() returning - status: %s\n", ConvertStatusToResponse(status).c_str());
    return status;
}


const SerialString_t SciProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{
    std::string fullResponse;
    // Log the respnse received from the module
    char temp[256];
    for (UINT16 ii = 0; ii < moduleResponse.length(); ii++)
        fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
    Log("ExtractModuleData() Extracted module data: %s\n", fullResponse.c_str());
    // Return the extracted data
    return moduleResponse;
}

const BEP_STATUS_TYPE SciProtocolFilter::SetMessageSpeed(const std::string &messageTag)
{
    speed_t baudRate = -1;  // Used to store the message tag's baud rate
    speed_t currentBaudIn = -1;     // Used to hold serial port in baud rate
    speed_t currentBaudOut = -1;        // Used to hold serial port out baud rate
    termios tio;    // used to access serial port information
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;  // Used to determine and return the method's status

    // Get the message's required baud rate
    status = GetBaudRate(messageTag,baudRate);
    Log(LOG_DEV_DATA, "Required baud rate: %d\n", baudRate);

    if(status == BEP_STATUS_SUCCESS)
    {   
        if(tcgetattr(m_inPortDrvFd, &tio) == EOK)
        {   // Read existing port protocol
            currentBaudIn =  tio.c_ispeed;
            currentBaudOut = tio.c_ospeed;
            Log(LOG_DEV_DATA, "Current baud rates: IN: %ld OUT: %ld\n", currentBaudIn, currentBaudOut);
            if((currentBaudIn != -1 && currentBaudOut != -1) &&
               ((currentBaudIn != baudRate) || (currentBaudOut != baudRate)))
            {   // Baud rate does not match, must change
                if(baudRate == GetHighSpeedMessageBaud())
                {   // Message is at high speed baud rate
                    // Switch module baud rate first
                    SwitchModuleBaudRateToHighSpeed();
                    // Always change the port, module may already be at low speed and module switch would have failed
                    status = SwitchSerialPortBaudRate(GetHighSpeedMessageBaud(), &tio);
                }
                else if(baudRate == GetLowSpeedMessageBaud())
                {   // Message is at low speed baud rate
                    SwitchModuleBaudRateToLowSpeed();
                    // Always change the port, module may already be at low speed and module switch would have failed
                    status = SwitchSerialPortBaudRate(GetLowSpeedMessageBaud(), &tio);
                }
                else
                {   // Unsupported baud rate for module
                    Log(LOG_ERRORS, "Unsupported baud rate for module %d\n", baudRate);
                        status = BEP_STATUS_SOFTWARE;
                }
            }
            else
            {   // Check if method baud rate is correct or if an error occured
                if(( currentBaudIn != -1) && (currentBaudOut != -1))
                {   // Baud rate is already set to correct value
                    status = BEP_STATUS_SUCCESS;
                    Log(LOG_DEV_DATA, "Baud rate is already %d\n", baudRate);
                }
                else
                {   // Error occured getting serial port's baud rate
                    Log(LOG_ERRORS, "Error getting current baud rates becase: %s\n", strerror(errno));
                }
            }
        }
        else
        {   // Failed to read serial port baud rate
            Log(LOG_ERRORS, "Failed to read initial port attributes because: %s\n", strerror(errno));
        }
    }
    else
    {   // Failed trying to get the message tag's baud rate
        Log(LOG_ERRORS, "SciProtocolFilter::SetMessageSpeed() failed to get message baud rate\n");
    }
    // return the status of the method
    return(status);
}

const BEP_STATUS_TYPE SciProtocolFilter::SwitchModuleBaudRateToHighSpeed(void)
{
    SerialString_t response;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    // Send byte to module
    status = SendMessage("SwitchToHighSpeed");
    if(status == BEP_STATUS_SUCCESS)
    {   // Message sent, check response
        status = GetResponse("SwitchToHighSpeed",response);
        if(status != BEP_STATUS_SUCCESS)
        {               
            Log(LOG_ERRORS, "SciProtocolFilter::SwitchModuleBaudRateToHighSpeed() - Invalid Response = %X",
                atoh((char *)response.c_str()));
        }
    }
    // return status of method
    return(status);
}

const BEP_STATUS_TYPE SciProtocolFilter::SwitchModuleBaudRateToLowSpeed(void)
{
    SerialString_t response;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    // Send byte to module
    status = SendMessage("SwitchToLowSpeed");
    if(status == BEP_STATUS_SUCCESS)
    {   // Message sent, check response
        status = GetResponse("SwitchToLowSpeed",response);
        if(status != BEP_STATUS_SUCCESS)
        {               
            Log(LOG_ERRORS, "SciProtocolFilter::SwitchModuleBaudRateToLowSpeed() - Invalid Response = %X",
                atoh((char *)response.c_str()));
        }
    }
    // return status of method
    return(status);
}

const BEP_STATUS_TYPE SciProtocolFilter::SwitchSerialPortBaudRate(const speed_t newBaud, termios *tio)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SOFTWARE;
    
    if(cfsetispeed(tio, newBaud) == EOK)
    {   // Set input speed
        if(cfsetospeed(tio, newBaud) == EOK)
        {   // Set output speed
            if(tcsetattr(m_inPortDrvFd, TCSANOW, tio) == EOK)
            {   // Set new port protocol
                if(tcgetattr(m_inPortDrvFd, tio) == EOK)
                {   // Read new port protocol
                    if((cfgetispeed(tio) == newBaud) && (cfgetospeed(tio) == newBaud))
                    {   // If it was changed
                        status = BEP_STATUS_SUCCESS;
                        Log(LOG_DEV_DATA, "Successfully set new baud rate\n");
                    }
                    else Log(LOG_ERRORS, "Failed to set new baud rate ispeed = %u, ospeed = %u, newSpedd = %u\n", cfgetispeed(tio), cfgetospeed(tio), newBaud);
                }
                else Log(LOG_ERRORS, "Failed to read new port attributes because: %s\n", strerror(errno));
            }
            else Log(LOG_ERRORS, "Failed set port attributes because: %s\n", strerror(errno));
        }
        else Log(LOG_ERRORS, "Failed to set new baud outgoing baud rate in attributes because: %s\n", strerror(errno));
    }
    else Log(LOG_ERRORS, "Failed to set new baud incomming baud rate in attributes because: %s\n", strerror(errno));
    // return status of method
    return(status);
}


