//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/Core/ServicesLayer/Servers/PlantHostOutbound/HostInterface/HostInterfaceKeepAlive.cpp 10/27/08 Jsemann $
//
// FILE DESCRIPTION:
// 	The HostInterfaceKeepAlive handles sending keep alive messages to the external External host systems.
//	This class will also proccess the response based on a configurable response string.
//
//===========================================================================
// Copyright (c) 2005- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
//*************************************************************************
#include "HostInterfaceKeepAlive.h"
#include "ITaskMon.h"
//-----------------------------------------------------------------------------

HostInterfaceKeepAlive::HostInterfaceKeepAlive(HostInterface *hostInterface) :
AtomicThread(), m_sequenceNumber(0x00), m_hostInterface(NULL),m_initialConnectionEstablished(false)
{   // Save the pointer to the host object
    m_hostInterface = hostInterface;
}

HostInterfaceKeepAlive::~HostInterfaceKeepAlive()
{   // Nothing special to do here
}

void HostInterfaceKeepAlive::Initialize(const XmlNode *document)
{
    // Get the time to wait between keep alive messages
    m_hostInterface->Log(LOG_ERRORS, "HostInterfaceKeepAlive::Initialize - getting KeepAliveTime");
    try
    {
        SetKeepAliveTime(BposReadInt(document->getChild("Setup/KeepAliveTime")->getValue().c_str()));
    }
    catch(XmlException &excpt)
    {
        m_hostInterface->Log(LOG_ERRORS, "KeepAliveTime not specified, using 30s: %s", excpt.GetReason());
        SetKeepAliveTime(30000);
    }
    // Get the reconnect time delay
    try
    {
        SetReconnectDelay(BposReadInt(document->getChild("Setup/ReconnectDelay")->getValue().c_str()));
    }
    catch(XmlException &excpt)
    {
        m_hostInterface->Log(LOG_ERRORS, "ReconncetDelay not specified, using 1s: %s", excpt.GetReason());
        SetReconnectDelay(1000);
    }
    // Get the keep alive max response time
    m_hostInterface->Log(LOG_ERRORS, "HostInterfaceKeepAlive::Initialize - getting MaxResponseTime");
    try
    {
        SetMaxResponseTime(BposReadInt(document->getChild("Setup/MaxResponseTime")->getValue().c_str()));
    }
    catch(XmlException &excpt)
    {
        m_hostInterface->Log(LOG_ERRORS, "MaxResponseTime not specified, using 5s: %s", excpt.GetReason());
        SetMaxResponseTime(5000);
    }
    // Get the minimum keep alive response length
    m_hostInterface->Log(LOG_ERRORS, "HostInterfaceKeepAlive::Initialize - getting MinResponseLength");
    try
    {
        SetMinResponseLength(BposReadInt(document->getChild("Setup/MinResponseLength")->getValue().c_str()));
    }
    catch(XmlException &excpt)
    {
        m_hostInterface->Log(LOG_ERRORS, "MinResponseLength not specified, using 7: %s", excpt.GetReason());
        SetMinResponseLength(7);
    }
    // Save the severity level map
    m_hostInterface->Log(LOG_ERRORS, "HostInterfaceKeepAlive::Initialize - getting SeverityLevelConversion");
    try
    {
        m_severityLevelConversion.DeepCopy(document->getChild("Setup/SeverityLevelConversion")->getChildren());
    }
    catch(XmlException &excpt)
    {
        m_hostInterface->Log(LOG_ERRORS, "SeverityLevelConversion not specified: %s", excpt.GetReason());
        m_severityLevelConversion.clear(true);
    }
    // Initialize the FaultServer interface object and host comm object
    m_hostInterface->Log(LOG_ERRORS, "HostInterfaceKeepAlive::Initialize - Initializing fault server comm.");
    m_faultServer.Initialize(FAULT_SERVER_NAME);
    try
    {   // Initialize the host Comms
        m_hostComm.Initialize(document);
    }
    catch(BepException &excpt)
    {   // Connection failed, keep trying until host is up
        m_hostInterface->Log(LOG_ERRORS, "Host Port not connected, will attempt later.");
    }
    try
    {
        SetKeepAliveRequestID(atoh(document->getChild("Setup/KeepAliveRequestID")->getValue().c_str()));
    }
    catch(XmlException &excpt)
    {
        m_hostInterface->Log(LOG_ERRORS, "KeepAliveRequestID not specified, using : 0xA3%s", excpt.GetReason());
        SetKeepAliveRequestID(0xA3);
    }
    try
    {
        SetKeepAliveResponseID(atoh(document->getChild("Setup/KeepAliveResponseID")->getValue().c_str()));
    }
    catch(XmlException &excpt)
    {
        m_hostInterface->Log(LOG_ERRORS, "KeepAliveResponseID not specified, using : 0x06 %s", excpt.GetReason());
        SetKeepAliveResponseID(0x06);
    }
    m_hostInterface->Log(LOG_ERRORS, "HostInterfaceKeepAlive::Initialize - complete");
}

void* HostInterfaceKeepAlive::Execute(void)
{   // Keep sending keep alives until the end of time, or we get terminated
    INT32 keepAliveInterval = GetKeepAliveTime() / 100;
    INT32 keepAliveSendCountDown = keepAliveInterval;
    bool timeToSendKeepAlive = true;
    // Periodically send the keep alive message until we are told to die.
    while(m_hostInterface->GetStatus().compare(BEP_TERMINATE))
    {   // Check if it is time to send the keep alive message
        if(timeToSendKeepAlive)
        {   // Send the keep alive message
            SendKeepAliveMessage();
            timeToSendKeepAlive = false;
            keepAliveSendCountDown = keepAliveInterval;
        }
        else
        {   // Wait for a bit, and then decrement the count down
            BposSleep(100);
            keepAliveSendCountDown--;
            timeToSendKeepAlive = !keepAliveSendCountDown;
        }
    }
    return NULL;
}

void HostInterfaceKeepAlive::BuildKeepAliveMessage()
{
    // Build the machine number portion of the string
    string message;
    message = (char)GetKeepAliveRequestID() + (char)m_sequenceNumber;
    UINT16 checksum = CalculateCheckSum(message) & 0x00FF;
    m_hostInterface->Log(LOG_DEV_DATA, "HostInterfaceKeepAlive: Keep Alive Message - <$%02X><$%02X><$%02X><$%02X><$%02X>", 
               GetKeepAliveRequestID(), m_sequenceNumber, checksum, CR, LF);
    // Add the wrapper to the keep alive string
    m_keepAliveMessage = message + (char)checksum + (char)CR + (char)LF;
    //increment or reset sequence number
    m_sequenceNumber = m_sequenceNumber < 0xFF ? m_sequenceNumber++ : 0;
}

BEP_STATUS_TYPE HostInterfaceKeepAlive::SendKeepAliveMessage(void)
{
    BEP_STATUS_TYPE messageStatus = BEP_STATUS_ERROR;
    bool portLocked = false;
    m_hostInterface->Log(LOG_DEV_DATA, "Enter HostInterfaceKeepAlive: SendKeepAliveMessage");
    BuildKeepAliveMessage();
    do
    {   // Send the message to the host after clearing the buffer
        // Attempt to lock the port
        portLocked = m_hostComm.LockPort();
        m_hostInterface->Log(LOG_DEV_DATA, "Send Keep alive Port locked status : %s(%d)\n", (portLocked ? "TRUE" : "FALSE"), portLocked);
        if(portLocked)
        {   // Tell threads to ignore any incoming data
            m_hostComm.ResetConnection();
            messageStatus = m_hostComm.Send(m_keepAliveMessage);
            // If we successfully sent the message, get the response from the host
            if(BEP_STATUS_SUCCESS == messageStatus)
            {
                messageStatus = GetKeepAliveResponse();
                // If host has been shutdown, wait for 1 second before attempting to talk to it again
                if(BEP_STATUS_TIMEOUT == messageStatus && m_initialConnectionEstablished)
                {
                    m_hostComm.ReconnectDriver();
                    BposSleep(GetReconnectDelay());  // Wait for port driver to reconnect
                }
                else if(messageStatus == BEP_STATUS_SUCCESS && !m_initialConnectionEstablished)
                {
                     m_initialConnectionEstablished = true;
                }
            }
            else
            {
                m_hostInterface->Log(LOG_ERRORS, "HostInterfaceKeepAlive: Could not send keep alive message to host - status: %s", 
                           ConvertStatusToResponse(messageStatus).c_str());
            }

            // Unlock the port for others to use
            m_hostInterface->Log(LOG_DEV_DATA, "HostInterfaceKeepAlive: Unlocking comm port for others");
            portLocked = m_hostComm.UnlockPort();
        }
        else
        {
            m_hostInterface->Log(LOG_ERRORS, "HostInterfaceKeepAlive: Could not lock the comm port.  Not sending keep alive.");
            messageStatus = BEP_STATUS_TIMEOUT;
            BposSleep(100);
        }
        // Keep attempting to connect until connected or told to terminate
    } while((messageStatus == BEP_STATUS_TIMEOUT) && (m_hostInterface->GetStatus().compare(BEP_TERMINATE)));
    // Return the status of sending the message
    return messageStatus;
}

BEP_STATUS_TYPE HostInterfaceKeepAlive::GetKeepAliveResponse(void)
{
    SerialString_t keepAliveResponse;
    INT32 numBytesRead;
    BEP_STATUS_TYPE responseStatus = BEP_STATUS_ERROR;
    // Get the response from the ECC
    numBytesRead = m_hostComm.ReadPort(keepAliveResponse, GetMaxResponseTime(), m_hostComm.GetTimeoutTime());
    // Check the number of bytes read to ensure we have a good message
    if(numBytesRead >= GetMinResponseLength())
    {   // Good response, check for correct response ID
        if(keepAliveResponse[0] == GetKeepAliveResponseID())
        {   // Good response, check for error codes in message
            responseStatus = BEP_STATUS_SUCCESS;
        }
        else
        {
            m_hostInterface->Log(LOG_ERRORS, "HostInterfaceKeepAlive: Incorrect response ID received %02X expected %02X",
                       keepAliveResponse[0], GetKeepAliveResponseID());
            responseStatus = BEP_STATUS_FAILURE;
        }
    }
    else if(0 >= numBytesRead)
    {   // No data received, set as disconnected
        responseStatus = BEP_STATUS_TIMEOUT;
        m_hostInterface->Log(LOG_ERRORS, "Lost comms with host, attempt to reconnect.");
    }
    else
    {
        m_hostInterface->Log(LOG_ERRORS, "HostInterfaceKeepAlive: Keep alive response too short %d, expected at least %d",
                   numBytesRead, GetMinResponseLength());
        responseStatus = BEP_STATUS_FAILURE;
    }
    return responseStatus;
}

inline void HostInterfaceKeepAlive::SetMaxResponseTime(const INT32 &maxTime)
{
    m_maxResponseTime = maxTime;
}

inline const INT32& HostInterfaceKeepAlive::GetMaxResponseTime(void)
{
    return m_maxResponseTime;
}

inline void HostInterfaceKeepAlive::SetMinResponseLength(const INT32 &minResponseLength)
{
    m_minResponseLength = minResponseLength;
}

inline const INT32& HostInterfaceKeepAlive::GetMinResponseLength(void)
{
    return m_minResponseLength;
}

const INT32 HostInterfaceKeepAlive::GetSeverityLevel(const string &level)
{
    XmlNodeMapItr iter = m_severityLevelConversion.find(level);
    INT32 severity = MAX_SEVERITY;
    if(iter != m_severityLevelConversion.end())
    {
        severity = BposReadInt(iter->second->getValue().c_str());
    }
    else
    {
        m_hostInterface->Log(LOG_ERRORS, "Severity level conversion not specified for level %s, default to %d", level.c_str(), MAX_SEVERITY);
    }
    return severity;
}

inline void HostInterfaceKeepAlive::SetKeepAliveTime(const INT32 &keepAliveDelay)
{
    m_keepAliveTime = keepAliveDelay;
}

inline const INT32& HostInterfaceKeepAlive::GetKeepAliveTime(void)
{
    return m_keepAliveTime;
}

void HostInterfaceKeepAlive::SetReconnectDelay(const INT32 &delay)
{
    m_reconnectDelay = delay;
}

const INT32& HostInterfaceKeepAlive::GetReconnectDelay(void)
{
    return m_reconnectDelay;
}

const void HostInterfaceKeepAlive::SetKeepAliveRequestID(UINT8 responseID)
{
    m_keepAliveRequestID = responseID;
}

const void HostInterfaceKeepAlive::SetKeepAliveResponseID(UINT8 requestID)
{
    m_keepAliveResponseID = requestID;
}

const UINT8 HostInterfaceKeepAlive::GetKeepAliveRequestID(void)
{
    return m_keepAliveRequestID;
}

const UINT8 HostInterfaceKeepAlive::GetKeepAliveResponseID(void)
{
    return m_keepAliveResponseID;
}
