//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/SerialChannel.cpp 3     10/26/06 10:00a Rwiersem $
//
// FILE DESCRIPTION:
//   SerialChannel object that is used to command a serial motor.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/SerialChannel.cpp $
// 
// 3     10/26/06 10:00a Rwiersem
// Changes for the 06102701 core release:
// 
// Now call ClosePort() before trying to open a port.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:35a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:53p Khykin
// Initial checkin.
//
// 1     4/22/03 9:32a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:22a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.2  2003/02/20 15:43:49  tbochene
//    Better print control
//
//    Revision 1.1.1.1  2003/02/15 00:03:42  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:25a Khykin
// Initial Checkin
//
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
//
// 12    11/08/02 8:27a Tbochene
// Updated to use SerialString_t defined in SerialDefs.h instead of
// SERIAL_STRING.  Removed retry counter in Send() since WritePort() does
// this already (it was trying 9 times instead of the desired 3 times)
//
// 11    9/26/02 2:02p Tbochene
// Removed compiler warnings
//
// 10    9/16/02 11:13a Khykin
// Putting back the Send with a std::string message.  This is a very
// dangerous method due to the nature of what is being sent but it is
// still being used.
//
// 9     9/06/02 11:27a Khykin
// Updating fault handling.
//
// 8     8/23/02 7:57a Dan.erickson
// Changed printf statements to Log calls.
//
// 7     6/13/02 3:00p Dan.erickson
// Added GetRemainingData method.
//
// 6     5/28/02 3:30p Dan.erickson
// Base class is expecting unsigned char instead of char.
//
// 5     4/29/02 1:38p Kh
// Update due to VSS overwriting my file when trying to check out
//
// 4     4/29/02 12:40p Kh
// Updating the Get methods to return lengths.
//
// 3     4/10/02 4:54p Kh
// Updating descriptions.
//
// 2     4/10/02 3:52p Kh
// Making change for polling Get.  Return was not handled correctly.
//
// 1     4/03/02 1:25p Kh
// Initial Checkin.
//
//
//*************************************************************************

#include "SerialChannel.h"

SerialChannel::SerialChannel() : ILogicalPort(), m_channelId(-1),
m_connectionId(-1), m_commBuffer(NULL), m_retryCount(5)
{
}
SerialChannel::~SerialChannel()
{   // close the communication port if opened
    ClosePort();
    // if a channel was connected to
    if(m_connectionId != -1)    BposConnectDetach( m_connectionId);
    // if a channel id was obtained
    if(m_channelId != -1)       BposChannelDestroy( m_channelId);
    if(m_commBuffer != NULL)    delete m_commBuffer;
}
bool SerialChannel::Initialize(const XmlNode *config)
{
    Log(LOG_FN_ENTRY, "SerialChannel::Initialize()\n");
    // initialize the base class
    bool status = ILogicalPort::Initialize(config);
    // get the communication node
    const XmlNode *communication = config->getChild("Setup")->getChild("Communication");

    try
    {
        m_retryCount = atoi(communication->getChild("Retries")->getValue().c_str());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "Retry count not found using default value of %d\n", m_retryCount);
    }
    try
    {   // get the buffer size and allocate an appropriate size buffer
        m_buffSize = atoi(communication->getChild("BufferSize")->getValue().c_str());
    }
    catch(...)
    {   // default to 1024 characters
        m_buffSize = 1024;
    }
    Log(LOG_DEV_DATA, "BufferSize: %d\n", m_buffSize);
    if( NULL != m_commBuffer)   delete[] m_commBuffer;
    m_commBuffer = new char[m_buffSize + 1];

    // if notification desired when a certain number of bytes are received
    SetName(communication->getChild("Name")->getValue());
    // Note: the timeout time is in mS
    m_timeoutTime = atol(communication->getChild("Timeout")->getValue().c_str());
    // Make sure closed before re-opening
    ClosePort();
    // open the port
    if(OpenPort(GetName().c_str()) != true)
    {
        Log(LOG_ERRORS, "OpenPort() failed\n");
        throw BepException("Could Not Open Port %s", GetName().c_str());
    }

    Log(LOG_FN_ENTRY, "SerialChannel::Initialize() done\n");

    return(status);
}
bool SerialChannel::OpenPort(const char *portName, int mode /* = O_RDWR */)
{
    bool intitialized = false;      // Flag to indicate if initialzation succeded
    Log(LOG_FN_ENTRY, "SerialChannel::OpenPort(%s, %d)\n", portName, mode);
    for(UINT8 ii = 1; (ii <= m_retryCount) && !intitialized; ii++)
    {   // Set status to not initialized
        if((intitialized = ILogicalPort::OpenPort(portName, mode)) == false)
        {
            Log(LOG_DEV_DATA, "SerialChannel::OpenPort() failed %d time%s\n", ii, (ii == 1) ?  "": "s");
            BposSleep(100);
        }
    }
    Log(LOG_FN_ENTRY, "SerialChannel::OpenPort(%s, %d) %s [initialized = %d]\n", portName, mode, intitialized ? "Passed" : "Failed", intitialized);
    return(intitialized);
}
BEP_STATUS_TYPE SerialChannel::Send(unsigned char *txBuff, int length)
{   // write the data
    BEP_STATUS_TYPE overallStatus;
    int status, retry = 3;
    Log(LOG_FN_ENTRY, "SerialChannel::Send(%d)\n", length);

    if((GetVerboseMask() & LOG_DEV_DATA) && GetLocalEcho())
    {
        std::string message;
        char buffer[128];       // buffer to create messages with
        for(int ii = 0; ii < length; ii++)
        {
            message += CreateMessage(buffer, sizeof(buffer), "0x%02X, ", txBuff[ii]);
        }
        Log(LOG_DEV_DATA, "Sending: [%d bytes], %s\n", length, message.c_str());
    }
    status = WritePort(txBuff, length, retry);  // With retry
    // determine the overall status
    if(status == -1)    overallStatus = BEP_STATUS_FAILURE;
    else
    {
        overallStatus = BEP_STATUS_SUCCESS;
        Log(LOG_DEV_DATA, "Sent %d bytes.  Success!\n", status);
    }

    return(overallStatus);
}
BEP_STATUS_TYPE SerialChannel::Send(SerialString_t& txBuff)
{   // write the data
    if((GetVerboseMask() & LOG_DEV_DATA) && GetLocalEcho())
    {
        PrintSerialString("SerialChannel::Send", txBuff);
    }
    return(Send((unsigned char *) txBuff.c_str(), txBuff.size()));
}
BEP_STATUS_TYPE SerialChannel::Send(std::string &txBuff)
{   // write the data
    return(Send((unsigned char *) txBuff.c_str(), txBuff.size()));
}
INT32 SerialChannel::Get(unsigned char *rxBuff, int size)
{
    int commLength = 0; // the length of the data received

    // read the information from the channel, if timeout provided, set it up
    commLength = ReadPort(rxBuff, size, m_timeoutTime, 0);

    return(commLength);
}
INT32 SerialChannel::Get(std::string &message)
{   // get the information
    int commLength = Get((unsigned char *)m_commBuffer, m_buffSize);
    // if data received
    if(commLength > 0)  message = m_commBuffer;
    else                message = "";

    return(commLength);
}
void SerialChannel::GetRemainingSerialData(unsigned char *buffer, const INT32 &bufferSize, INT32 &byteCount, const INT32 &maxAttempts    /*= 3*/, const INT32 &dataDelay    /*= 50*/)
{
    unsigned char serialData[bufferSize];
    INT32 bytesRead = 0, attempts = 0;

    // Keep checking the port until our attempts are done or we exceed the buffer size
    do
    {
        // Get the next character from the serial port      
        memset(serialData, 0, bufferSize);
        // wait for data up to the dataDelay time but once all data is read return
        printf("Getting Remaining Serial Data delay: %d, maxAttempts: %d\n", dataDelay, maxAttempts);
        bytesRead = ReadPort(serialData, bufferSize, dataDelay, 0);
        // if No data received
        if(bytesRead < 1)
        {   // Decrement number of retries
            attempts++;             
        }
        // else record the data
        else
        {   // Reset number of attempts
            attempts = 0;
            for(INT32 index = 0; (index < bytesRead) && (byteCount < bufferSize); index++)
            {   // Add the data to the buffer
                buffer[byteCount++] = serialData[index];
            }
        }
    } while((attempts < maxAttempts) && (byteCount < bufferSize));
}


const std::string SerialChannel::GetName(void)
{
    return(m_name);
}


const UINT64 SerialChannel::GetTimeoutTime(void)
{
    return(m_timeoutTime);
}


void SerialChannel::SetTimeoutTime(UINT64 timeout)
{
    m_timeoutTime = timeout;
}


inline void SerialChannel::SetName(const std::string &name)
{
    if(!name.empty())  m_name = name;
}
