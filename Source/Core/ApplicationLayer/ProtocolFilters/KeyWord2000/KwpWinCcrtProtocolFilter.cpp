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

        for(XmlNodeMapCItr iter = m_WinCcrtMsgNodes.begin();
                (iter != m_WinCcrtMsgNodes.end());
                iter++)
        {
    
            string msgT = iter->second->getAttribute("MessageTag")->getValue();
            Log(LOG_DEV_DATA, "\tmessageTag: %s   msgT: %s\n", "MSG_TAG", msgT.c_str());
        }
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
            (iter != m_WinCcrtMsgNodes.end());
            iter++)
    {

        string msgT = iter->second->getAttribute("MessageTag")->getValue();
        Log(LOG_DEV_DATA, "\tmessageTag: %s   msgT: %s\n", messageTag.c_str(), msgT.c_str());

        if ( msgT == messageTag)
        {
            msgType = iter->second->getAttribute("MessageType")->getValue();
            isMsgTagFound = true;
            break;
        }
    }

    if (!isMsgTagFound)
    {
        Log(LOG_DEV_DATA, "WinCcrt Message tag was not found in config file. Defaulting to %s.", msgType.c_str());
        Log(LOG_DEV_DATA, "\tPossible options {SetAbsType,SendMessage,Connect,Disconnect}\n");
    }

    std::string winCcrtMsg = msgType + std::string(":") + messageTag + std::string(":");

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
    
    Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::ExtractModuleData - Enter");
    
    Log(LOG_DEV_DATA, "ExtractModuleData - Nothing is happening in this method.");
    
    Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::ExtractModuleData - Exit");

    // Return the extracted data
    return moduleResponse;
}

int KwpWinCcrtProtocolFilter::WaitForFullResponse(SerialString_t &response)
{
    int         retVal = 0;
    string message;
    string value;

    string winResponse = "ModuleResponse";

    // set up the receive timeout
    Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::WaitForFullResponse - Waiting for response from module\n");
    Log(LOG_DEV_DATA, "\t -- Module Response Delay: %d\n", GetResponseDelay());
    Log(LOG_DEV_DATA, "\t -- Response End Delay: %d\n", GetResponseEndDelay());

    retVal = m_WinCcrtComm.Read("ModuleResponse", message, true);

    Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::WaitForFullResponse - message: %s  status: %d", message.c_str(), retVal);
    

    response.clear();
    int bytesRead = 0;
    
    if(retVal == BEP_STATUS_SUCCESS)
    {
        char *stokPtr;
        char* delims = { "," };
        
        uint8_t tempHexVal = 0;

        //Log(LOG_DEV_DATA, "MesmessageChar has been copied.");

        if(BEP_STATUS_SUCCESS == m_WinCcrtComm.GetByTag("ModuleResponse", value, message))
        {

            Log(LOG_DEV_DATA, "value: %s    message: %s", value.c_str(), message.c_str());

            char * valueChar;
            valueChar = (char *)malloc(value.length());
            
            strcpy(valueChar, value.c_str());
            
            //Log(LOG_DEV_DATA, "valueChar: %s   value: %s", valueChar, value.c_str());

            // Parse 0x??... to response
            stokPtr = strtok(valueChar, delims);
            while(stokPtr != NULL)
            {
                
                //Log(LOG_DEV_DATA, "sizeof a.%d  b.%d\n", (int)sizeof(&stokPtr), (int)strlen(stokPtr));
                // verify that the format of the string has two characters
                if(strlen(stokPtr) == 2)
                {
                    // add the tokenize string to the hex response list
                    tempHexVal = uint8_t(atoh(stokPtr));
                    //Log(LOG_DEV_DATA, "stokPtr{%d}: %s   a.%d  b.%o  c.%x\n", bytesRead, stokPtr, tempHexVal, tempHexVal, tempHexVal);

                    response.push_back(tempHexVal);
                }
                else
                {
                    Log(LOG_DEV_DATA, "Parsed data byte (%s) is incorrect size: %d expected length: 2", stokPtr, strlen(stokPtr));
                    bytesRead--;
                }

                // look for new byte of information to add
                stokPtr = strtok(NULL, delims);
                bytesRead++;
            }
            
            free(valueChar);
        }
        else
        {
           Log(LOG_DEV_DATA, "Trouble getting data tag");
        }

        
    }

    Log(LOG_DEV_DATA, "Done waiting for response.  %d bytes received\n", message.length());
    
    PrintSerialString("serial string created", response, LOG_DEV_DATA);
    return(bytesRead);
}

const BEP_STATUS_TYPE KwpWinCcrtProtocolFilter::GetResponse(const std::string &messageTag, SerialString_t &reply)
{
	BEP_STATUS_TYPE status = BEP_STATUS_NA;
	// Get the response from the module
	INT32 retry = 0;		// retry counter
	INT32 bytesRead = 0;	// the number of bytes read
    Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::GetRespons tk:gr1");
	do
	{	// read the data from the port
        Log(LOG_DEV_DATA, "KwpWinCcrtProtocolFilter::GetRespons tk:gr1 - wffr");
        bytesRead = WaitForFullResponse(reply);
		Log(LOG_DETAILED_DATA, "Received %d bytes for message %s\n", bytesRead, messageTag.c_str());
		// if data is read from the port, determine if it is done
		if(bytesRead > 0)
		{	// Log the response if enabled and flag set
			if(GetLogStatus() && (GetVerboseMask() & LOG_DEV_DATA))
			{
				std::string response;		// response
				char buffer[128];			// buffer for printing
				for(INT16 ii = 0; ii < bytesRead; ii++)
					response += CreateMessage(buffer, sizeof(buffer),"$%02X ", reply[ii]);
				Log(LOG_DETAILED_DATA, "Response bytes received: %d for message: %s\tResponse: %s\n\n",
					bytesRead, messageTag.c_str(), response.c_str());
			}
			// Validate the response against all filters
			if(IsResponseValid(messageTag, reply))
			{
				Log(LOG_DETAILED_DATA, "Full response received\n");
                m_WinCcrtComm.Clear();
                Log(LOG_DETAILED_DATA, "Cleared the B\n");
                // wait for ClearBuffer response
                int retVal = BEP_STATUS_NA;
                string message;
                SerialString_t response;
                int retries = 15;
                retVal = (BEP_STATUS_TYPE) m_WinCcrtComm.Write(std::string("ClearBuffer"), "FF", message, true);

                do
                {
                    retVal = m_WinCcrtComm.Read("ClearBuffer", message, true);
                    Log(LOG_DETAILED_DATA, "Waiting for ClearBuffer retVal: %d/%d   retries:%d   message: %s\n", retVal, BEP_STATUS_SUCCESS, retries, message.c_str());

                    if (message.find("ClearBuffer") > 0)
                    {
                        Log(LOG_DETAILED_DATA, "Valid response rx\n");
                        status = BEP_STATUS_SUCCESS;
                        break;
                    }
                    else
                    {
                       retVal = BEP_STATUS_NA;
                    }
                    retries--;

                    if (BEP_STATUS_NA == retVal)
                    {
                        delay(10);
                    }

                }while (retVal != BEP_STATUS_SUCCESS && retries > 0);


                Log(LOG_DETAILED_DATA, "Done with the response\n");
			}
			else
            {
                Log(LOG_ERRORS, "Incomplete response\n");
            }
				

			retry = 0;	// rest the retry counter, due data received
		}
		// else it is not looking good
		else
		{
			// if a problem detected reading the port
			if(bytesRead < 0)
			{	// Error reading data from the port
				status = BEP_STATUS_ERROR;
			}
			// else if the reply is empty and already performed another attempt
			else if(reply.empty() && retry)
			{	// No data sent from the module
				status = BEP_STATUS_FAILURE;
			}
			// else wait the full retry attempts to see the message received
			else
			{
				Log(LOG_DETAILED_DATA, "Waiting for full message\n");
			}
			retry++;	// increment the retry counter
		}
        if (status == BEP_STATUS_NA)
        {
            delay(30);
        }

		// while the result not detemined and retries remain
	}while((status == BEP_STATUS_NA) && (retry < 3) && !GetStopCommsFlag());

	// if a successful read never occurred, fail
	if(retry >= 3) status = BEP_STATUS_FAILURE;

	Log(LOG_FN_ENTRY, "Exit GetResponse: %d\n", status);

	// Return the status
	return(status);
}
