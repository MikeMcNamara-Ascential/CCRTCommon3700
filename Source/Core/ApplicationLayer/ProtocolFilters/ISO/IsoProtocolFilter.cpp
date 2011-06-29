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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/ISO/IsoProtocolFilter.cpp $
// 
// 5     10/25/06 3:33p Rwiersem
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
// 3     7/21/04 10:44a Cward
// Added Bep Mutex parameter to the constructor
//
// 2     6/09/04 11:03p Cward
// Removed const declaration for the parameter of the Extract Module Data
// Method. UPdated the header comment block to the new standard
//
// 1     12/21/03 6:33p Cward
//
// 1     7/08/03 1:05p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:28p Khykin
// Initial checkin
//    Revision 1.3  2003/06/18 13:11:48  tenharmsel
//    Syncing directory trees with the FordChicago system
//
//    Revision 1.2  2003/05/30 18:20:33  tenharmsel
//    Created FordIsoProtocolFilter
//
//    Revision 1.1.1.1  2003/05/07 15:14:21  tenharmsel
//    Bringing in the ISOKeyword ProtocolFilters
//
//    Revision 1.2  2003/03/14 20:09:10  tenharmsel
//    Removed one mismatched paren.
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
#include "IsoProtocolFilter.h"

//-----------------------------------------------------------------------------
IsoProtocolFilter::IsoProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar, BepMutex *commsInUse /*= NULL*/) : ProtocolFilter(lastTxTime, stopCommsBepCondVar, commsInUse)
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
IsoProtocolFilter::~IsoProtocolFilter()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
bool IsoProtocolFilter::Initialize(const XmlNode *config)
{
    UINT8 trailer = 0x00;
    try
    {
        trailer = atoh(config->getChild("Setup/MessageData/CommandTerminator")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find command trailer char - %s", excpt.GetReason());
        trailer = 0x00;
    }
    CommandTrailerCharacter(&trailer);
    bool requiresChecksum = true;
    try
    {
        requiresChecksum = atob(config->getChild("Setup/MessageData/RequiresChecksum")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find checksum required bool - %s", excpt.GetReason());
        requiresChecksum = true;
    }
    RequiresChecksum(&requiresChecksum);
    bool echoModuleData = false;
    try
    {
        echoModuleData = atob(config->getChild("Setup/MessageData/EchoModuleData")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find echo module data bool - %s", excpt.GetReason());
    }
    EchoModuleData(&echoModuleData);
    // Set the sync byte
    UINT8 syncByte = 0x00;
    try
    {
        syncByte = BposReadInt(config->getChild("Setup/MessageData/SyncByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Sync byte location not defined: %s", excpt.GetReason());
    }
    SyncByte(&syncByte);
    // Call base class to complete the initialization
    return ProtocolFilter::Initialize(config);
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE IsoProtocolFilter::SendMessage(SerialString_t &message)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    Log(LOG_FN_ENTRY, "IsoProtocolFilter::SendMessage(SerialString_t)");
    if(EchoModuleData())
    {
        // Clear the Fifos so bad data is not gathered
        Log(LOG_DETAILED_DATA, "Resetting The Connection\n");
        ResetConnection();
        Log(LOG_DETAILED_DATA, "Reset The Connection\n");
        // Update the sync byte - need to increment before sending
        message[SyncByte()] = ++m_syncData;
        // Send the message and return the result
        PrintSerialString( "ProtocolFilter Sending", message);
        // Write message
        INT32 bytesSent = 0, byteRead = 0;
        INT32 byteWritten = 0;
        UINT8 echoData = 0x00;
        // Send each byte except the trailing character that does not get echoed
        for(UINT16 index = 0; (index < message.length()) && (BEP_STATUS_SUCCESS == status); index++)
        {   // Write the current byte
            byteWritten = WritePort(&message[index], 1, 1);
            Log(LOG_DETAILED_DATA, "Sent %02X, waiting for echo", message[index]);
            bytesSent += byteWritten;
            // Wait for the module reflection and verify it is valid
            byteRead = ReadPort(&echoData, 1, 25, 0);
            Log(LOG_DETAILED_DATA, "Read %02X, validating data - written: %d, read: %d", echoData, byteWritten, byteRead);
            echoData = ~echoData;
            // Verify the echo is good
            status = ((byteWritten == byteRead) && (echoData == message[index])) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
        }
        // Send the trailer data
        byteWritten = WritePort(&(CommandTrailerCharacter()), 1, 1);

        SetLastTxTime();

        Log(LOG_DETAILED_DATA, "Sent the message - status: %s", ConvertStatusToResponse(status).c_str());
        // Determine the result of the write
        if(bytesSent < 0)       status = BEP_STATUS_ERROR;
        else if(bytesSent == 0) status = BEP_STATUS_FAILURE;
        else if(bytesSent > 0)  status = BEP_STATUS_SUCCESS;
    }
    else
    {   // Just call the base class to complete the message transmission
        status = ProtocolFilter::SendMessage(message);
    }
    return status;
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE IsoProtocolFilter::SendMessage(string messageTag)
{   // just call the base class
    return ProtocolFilter::SendMessage(messageTag);
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE IsoProtocolFilter::SendMessage(string messageTag, SerialArgs_t &args)
{   // Just call the base class
    return ProtocolFilter::SendMessage(messageTag, args);
}

//-----------------------------------------------------------------------------
const SerialString_t IsoProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{
    std::string fullResponse;
    bool validChecksum = true;
    SerialString_t dataResponse;
    // Log the respnse received from the module
    char temp[256];
    for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
    {
        fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
    }
    // Validate the Checksum if turned on by configuration file
    if(IsChecksumValidationRequired())
    {
        validChecksum = IsChecksumValid(moduleResponse);
    }

    if(validChecksum)
    {
    Log("Extracting data from response string: %s\n", fullResponse.c_str());

        if(moduleResponse.length() > GetDataStartIndex())
        {
    // Strip off the header and checksum
        dataResponse = moduleResponse.substr(GetDataStartIndex(),moduleResponse.length() - GetDataStartIndex());
    // Log the data extracted from the module
    fullResponse.erase();
            for(UINT16 ii = 0; ii < dataResponse.length(); ii++)
        fullResponse += CreateMessage(temp, 256, "$%02X ", dataResponse[ii]);
    Log("ExtractModuleData() Extracted module data: %s\n", fullResponse.c_str());
    }
    else
    {
            Log(LOG_ERRORS,"ERROR Module response length of %d is to short to be valid\n",
                moduleResponse.length());
        }
    }
    else
    {
        Log(LOG_ERRORS, "IsoProtocolFilter::ExtractModuleData - Invalid Checksum on message, not extracting response\n");
        dataResponse.clear();
    }
    // Return the extracted data
    return dataResponse;
}

//-----------------------------------------------------------------------------
bool IsoProtocolFilter::IsChecksumValid(SerialString_t &moduleResponse)
{
    UINT8 calculatedChecksum = 0x00;
    UINT8 responseChecksum = 0x00;
    responseChecksum = moduleResponse[(moduleResponse.length()-1)];
    for(UINT16 ii = 0; ii < moduleResponse.length() - 1; ii++)
    {
        calculatedChecksum += moduleResponse[ii];
    }
    // Compare the calculated vs module response checksum
    return(responseChecksum == calculatedChecksum);
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IsoProtocolFilter::GetInitialMessage(INT32 byteCount, UINT8 initialTxByte, SerialString_t &message)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    UINT8 echoData = 0x00;
    Log(LOG_DEV_DATA, "Sending %02X echo data to the module", initialTxByte);
    INT32 bytesWritten = WritePort(&initialTxByte, 1, 1);
    Log(LOG_DEV_DATA, "Sent %d bytes to the module", bytesWritten);
    for(INT32 index = 0; (index < byteCount) && (status == BEP_STATUS_SUCCESS); index++)
    {
        INT32 retVal = ReadPort(message, 25, 0);
        if(retVal > 0)
        {   // If this is the sync byte, save it.  Note we already recived  one byte, so sync byte will be one less
            if((SyncByte() - 1) == index)   m_syncData = message[index];
            echoData = ~message[index];
            Log(LOG_DEV_DATA, "Rx %02X, echoing %02X back to the module", message[index], echoData);
            // Echo the data back to the module if not trailer
            if((BEP_STATUS_SUCCESS == status) && (index < (byteCount - 1)))
            {
                bytesWritten = WritePort(&echoData, 1, 1);
                Log(LOG_DEV_DATA, "Sent %d bytes to the module", bytesWritten);
            }
            else if((message[index] == CommandTrailerCharacter()) && (index == byteCount-1))
            {
                Log(LOG_DEV_DATA, "End of ABS transmission");
            }
            else
            {
                Log(LOG_ERRORS, "No data from the ABS module.");
            }
        }
        else
        {
            Log(LOG_ERRORS, "No data from the ABS module - retVal: %d", retVal);
        }
    }
    return status;
}

//-----------------------------------------------------------------------------
inline const UINT8& IsoProtocolFilter::CommandTrailerCharacter(const UINT8 *character) /*= NULL*/
{
    if(character != NULL)  m_commandTrailer = *character;
    return m_commandTrailer;
}


//-----------------------------------------------------------------------------
inline const bool& IsoProtocolFilter::RequiresChecksum(const bool *reqChecksum) /*= NULL*/
{
    if(reqChecksum != NULL)  m_requiresChecksum = *reqChecksum;
    return m_requiresChecksum;
}


//-----------------------------------------------------------------------------
void IsoProtocolFilter::AddChecksumToMessage(SerialString_t &message)
{
    if(RequiresChecksum())
    {
        // call ProtocolFilter AddCheckSumMessage
        ProtocolFilter::AddChecksumToMessage(message);
    }
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE IsoProtocolFilter::GetResponse(const std::string &messageTag, SerialString_t &reply)
{
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    if(EchoModuleData())
    {   // Echo data back to the module
        UINT8 modByteCount = 0x00;
        SerialString_t absData;
        INT32 byteCount = ReadPort(&modByteCount, 1, 25, 0);
        if(byteCount > 0)
        {
            status = GetInitialMessage(modByteCount, ~modByteCount, absData);
        }
        // Add the incoming data to the reply message we have already received
        reply = modByteCount;
        reply += absData;
        Log(LOG_DEV_DATA, "Echoed module data - status: %s", ConvertStatusToResponse(status).c_str());
        PrintSerialString(messageTag.c_str(), reply);
    }
    else
    {   // Just call the base class to get the response
        status = ProtocolFilter::GetResponse(messageTag, reply);
    }
    return status;
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE IsoProtocolFilter::CheckForValidResponse(const SerialString_t &moduleResponse)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if(EchoModuleData())
    {
        status = BEP_STATUS_SUCCESS;
    }
    else
    {
        status = ProtocolFilter::CheckForValidResponse(moduleResponse);
    }
    return status;
}

//-----------------------------------------------------------------------------
inline const bool& IsoProtocolFilter::EchoModuleData(const bool *echoData) /*= NULL*/
{
    if(echoData != NULL)  m_echoModuleData = *echoData;
    return m_echoModuleData;
}

//-----------------------------------------------------------------------------
const UINT8& IsoProtocolFilter::SyncByte(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_syncByte = *byte;
    return m_syncByte;
}
