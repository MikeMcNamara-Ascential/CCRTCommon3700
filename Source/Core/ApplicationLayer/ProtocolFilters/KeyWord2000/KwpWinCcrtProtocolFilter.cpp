//*************************************************************************
// FILE:
//    $Header: /Nissan/Source/Nissan/ApplicationLayer/ProtocolFilters/KwpBosch/KwpWinCcrtProtocolFilter.cpp 3     1/09/08 1:28p Mmcnamar $
//
// FILE DESCRIPTION:
//	Keyword 2000 protocol filter customized for Bosch module for Nissan
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
//    $Log: /Nissan/Source/Nissan/ApplicationLayer/ProtocolFilters/KwpBosch/KwpWinCcrtProtocolFilter.cpp $
//
// 3     1/09/08 1:28p Mmcnamar
// Updated for CAN ABS: Updated ExtractMessage( ); Overrode SendMessage()
// from ProtocolFilter.cpp to not send checksum for CAN msgs.
//
// 2     5/29/07 8:10a Gswope
// Fixed bug in constructor
//
// 1     2/09/07 1:50p Gswope
// Initial entry
//
// 1     2/23/05 8:26a Gswope
// new location, code as of 1/1/2005
//
// 1     11/22/04 11:09a Gswope
// Updates for sensor calibration.
//    Revision 1.2  2004/04/10 20:54:54  bereza
//    code complete. config complete. all uncompiled yet
//
//    Revision 1.1  2004/04/10 13:00:10  bereza
//    start of day 2 at the plant. Got the module config file done, maybe, except for interpretation. Starting on custom ProtocolFilter
//
//*************************************************************************

#include "KwpWinCcrtProtocolFilter.h"

KwpWinCcrtProtocolFilter::KwpWinCcrtProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar, BepMutex *commPortInUse /* =NULL*/) :
    ProtocolFilter(lastTxTime, stopCommsBepCondVar, commPortInUse)
{
    // NOthing special to do here
}

KwpWinCcrtProtocolFilter::~KwpWinCcrtProtocolFilter()
{
    // Clear out the buffer before we leave.
    Log(LOG_FN_ENTRY, "Resetting connection in ~KwpWinCcrtProtocolFilter()");
    //ResetConnection();
    //BposSleep(150);
    Log(LOG_FN_ENTRY, "~KwpWinCcrtProtocolFilter() complete\n");
}

bool KwpWinCcrtProtocolFilter::InitializeServer(const XmlNode *configNode)
{
    Log(LOG_FN_ENTRY, "KwpWinCcrtProtocolFilter::InitializeServer - Enter");

    std::string server = "";
    try
    {
        server = configNode->getChild("Setup/WinCcrtServer")->getValue();

    }
    catch(XmlException &err)
    {
        server = std::string("core/WinCcrt");
        Log(LOG_DEV_DATA, "\t defaulting to: %s\n", server.c_str());
    }
    Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::InitializeServer - Using the following server: %s\n", server.c_str());

    m_WinCcrtComm.Initialize(server, std::string("Client"));

    try
    {
        m_WinCcrtMsgNodes.DeepCopy(configNode->getChild("Setup/WinCcrtMsgTags")->getChildren());
    }
    catch(XmlException &err)
    {
        Log(LOG_DEV_DATA, "\t Cound not copy node for WinCcrtMsgTags. Broken interface with WinCcrt server\n", server.c_str());
    }



    Log(LOG_FN_ENTRY, "KwpWinCcrtProtocolFilter::InitializeServer - Exit");


    // Call the base class to complete the initialization
    return ProtocolFilter::Initialize(configNode);

}

const BEP_STATUS_TYPE KwpWinCcrtProtocolFilter::SendMessage(std::string messageTag)
{
    // Call the base class
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    std::string response;

    Log(LOG_DETAILED_DATA, "KwpWinCcrtProtocolFilter::SendMessage - Resetting The Connection\n");
    ResetConnection();
    Log(LOG_DETAILED_DATA, "KwpWinCcrtProtocolFilter::SendMessage - Reset The Connection\n");

    // default to SendMessage WinCcrt MsgType
    std::string msgType = "SendMessage";
    bool isMsgTagFound = false;

    for(XmlNodeMapCItr iter = m_WinCcrtMsgNodes.begin();
            (iter != m_WinCcrtMsgNodes.end()) && (BEP_STATUS_SUCCESS == status);
            iter++)
    {
        string itrMsgTag = iter->second->getAttribute("MessageTag")->getValue();

        if (itrMsgTag == messageTag)
        {
            msgType = iter->second->getAttribute("MessageTag")->getValue();
            isMsgTagFound = true;
            break;
        }
    }

    if (!isMsgTagFound)
    {
        Log(LOG_DEV_DATA, "WinCcrt Message tag was not found in config file. Defaulting to %s.",
            msgType.c_str());
        Log(LOG_DEV_DATA, "\tPossible options {SetAbsType,SendMessage,Connect,Disconnect}\n");
    }

    std::string winCcrtMsg = msgType + std::string(":") + messageTag;

    Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::SendMessage : %s\n", winCcrtMsg.c_str());
    // Send the message and return the result
    status = (BEP_STATUS_TYPE) m_WinCcrtComm.Write(std::string("WinMessage"), winCcrtMsg, response, true);
    Log(LOG_DEV_DATA, "m_WinCcrtComm.Write returned: %d. success is: %d   failure is: %d", status, BEP_STATUS_SUCCESS, BEP_STATUS_FAILURE);

    status = BEP_STATUS_SUCCESS;

    // Return the status
    return(status);
}


const SerialString_t KwpWinCcrtProtocolFilter::ExtractModuleData(SerialString_t &moduleResponse)
{
    string fullResponse;
    // Log the respnse received from the module
    char temp[256];
    for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
        fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
    Log(LOG_DETAILED_DATA, "Extracting data from response string: %s\n", fullResponse.c_str());
    // Look at the number of data bytes returned from the module
    UINT8 numberOfBytes = moduleResponse[5];
    // Module response starts immediately after the module response ID
    SerialString_t dataResponse;
    const SerialString_t::size_type dataStartIdx = GetDataStartIndex();
    const SerialString_t::size_type dataRespSz = numberOfBytes-GetDataHeaderLength();

    dataResponse.clear();

    if( moduleResponse.size() >= (dataStartIdx + dataRespSz) )
    {
        dataResponse = moduleResponse.substr(dataStartIdx, dataRespSz);

        // Log the data extracted from the module
        fullResponse.erase();
        for(UINT16 ii = 0; ii < dataResponse.length(); ii++)
            fullResponse += CreateMessage(temp, 256, "$%02X ", dataResponse[ii]);
        Log(LOG_DEV_DATA, "Extracted module data: %s\n", fullResponse.c_str());
        // Check if multiple messages are in the module
        Log(LOG_DETAILED_DATA, "Total Response Length: %d, Current Message Length: %d, Number of Bytes: %d\n",
            moduleResponse.length(), (5 + numberOfBytes + 1), numberOfBytes);
        if((UINT16)moduleResponse.length() > (5 + numberOfBytes + 1))
        {
            // Multiple messages in the response, update to point at next message
            moduleResponse = moduleResponse.substr(5 + numberOfBytes + 1);
            // Log the data remaining in the buffer
            fullResponse.erase();
            for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
                fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
            Log(LOG_DETAILED_DATA, "Module data remaining in buffer: %s\n", fullResponse.c_str());
        }
        else
        {
            // No more messages to be processed, clear the buffer
            moduleResponse.erase();
        }
    }
    else
    {
        for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
            fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
        Log( LOG_ERRORS, "Incomplete message in ExtractModuleData: <%s>\n", fullResponse.c_str());
        fullResponse.clear();
    }
    // Return the extracted data
    return dataResponse;
}

int KwpWinCcrtProtocolFilter::WaitForFullResponse( SerialString_t &response)
{
    int         retVal = 0;
    char messageChar[80];
    string message;
    string value;

    string winResponse = "ModuleResponse";

    // set up the receive timeout
    Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::WaitForFullResponse - Waiting for response from module\n");
    Log(LOG_DEV_DATA, "\t -- Module Response Delay: %d\n", GetResponseDelay());
    Log(LOG_DEV_DATA, "\t -- Response End Delay: %d\n", GetResponseEndDelay());

    retVal = m_WinCcrtComm.Read(std::string("ModuleResponse"), message, true);

    Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::WaitForFullResponse - message: %s  status: %d", message.c_str(), retVal);

    if(retVal == BEP_STATUS_SUCCESS)
    {
        char *stokPtr;
        char* delims = { "," };
        int cnt = 0;
        uint8_t tempHexVal=0x00;
        if (message.length() > 80) Log(LOG_DEV_DATA, "Message Response is too long. Increase size.\n", GetResponseEndDelay());

        
        strcpy(messageChar, message.c_str());

        if(BEP_STATUS_SUCCESS == m_WinCcrtComm.GetByTag("ModuleResponse", value, message))
        {
            // Parse 0x??... to response
            stokPtr = strtok(messageChar, delims);
            while(stokPtr != NULL)
            {
                

                Log(LOG_DEV_DATA, "sizeof a.%d  b.%d\n", (int)sizeof(&stokPtr), (int)strlen(stokPtr));
                // verify that the format of the string has two characters
                if(sizeof(&stokPtr) == 2)
                {
                    // add the tokenize string to the hex response list
                    tempHexVal = uint8_t(atoh(stokPtr));
                    Log(LOG_DEV_DATA, "stokPtr{%d}: %s   a.%d  b.%o  c.%x\n", cnt, stokPtr, tempHexVal, tempHexVal, tempHexVal);
                    response.append(&tempHexVal);
                }
                else
                {
                    Log(LOG_DEV_DATA, "Parsed data byte is incorrect size: %s expected length: 2", stokPtr);
                }

                // look for new byte of information to add
                stokPtr = strtok(NULL, delims);
                cnt++;
            }
        }
    }
    PrintSerialString("serial string created: ", response, LOG_DEV_DATA);
    Log(LOG_DEV_DATA, "Done waiting for response.  %d bytes received\n", message.length());
    return( retVal);
}
