//******************************************************************************
//
// $Archive: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/ProtocolFilters/CAN/FordCanProtocolFilter.cpp $
// $Author: Mmcnamar $
//   $Date: 2/07/11 4:45p $
//
// Description:
//    Protocol filter for the Ford modules on CAN line
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/ProtocolFilters/CAN/FordCanProtocolFilter.cpp $
// 
// 1     2/07/11 4:45p Mmcnamar
// backup c drive source for cross compiler
// 
// 1     2/07/11 4:43p Mmcnamar
// backup c drive
// 
// 2     9/20/10 2:34p Mmcnamar
// Delay 150 ms after getting negative response from module in
// GetModuleData().
// 
// 1     5/10/10 2:09p Mmcnamar
// initial check-in for new core.
// 
// 9     3/17/10 12:06p Mmcnamar
// Updated GetModuleData() to prevent getting caught in an endless retry
// loop.
// 
// 8     11/14/08 3:22p Mmcnamar
// Added method SendMessage() that takes in arguments for variables in the
// config file message; Subtracting DataHeaderLength instead of
// DataStartIndex from response in ExtractModuleData(); Added methid
// GetModuleData() and updated to include retries for negative responses.
// 
// 7     8/03/06 1:18p Bmeinke
// If the AVTTxObjectNumber is not specified in the config file, do not
// re-throw an exception (does not play well with the base class)
// 
// 6     5/01/06 12:18p Bmeinke
// Added SendMessage(std::string) to comply with latest core
// 
// 5     2/24/06 1:18p Bmeinke
// In ExtractModuleData(): replaced GetDataStartIndex() with
// GetdataHeaderLength()
// 
// 4     2/21/06 4:22p Bmeinke
// reading the AVTTxObjectNumber from the config file back in (for U38x
// multi-frame support)
// 
// 3     6/30/05 8:29p Bmeinke
// Had to comment out the Initialize() section that reads the TX object
// number fro mthe config file (will investigate more later)
// 
// 2     6/10/05 4:13p Bmeinke
// Added supprt for new short transmit message format
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 1     9/30/04 2:42p Bmeinke
// Initial full core integration for Ford
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "FordCanProtocolFilter.h"

//FordCanProtocolFilter::FordCanProtocolFilter(BepMutex *commsInUse /*=NULL*/) 
//	: ProtocolFilter(commsInUse), m_avtTxObjNo( 0xFF), m_useTxObjNo( false)

//MAM 4/26/10 - For new core
FordCanProtocolFilter::FordCanProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar, BepMutex *commPortInUse /* =NULL*/)
: ProtocolFilter(lastTxTime, stopCommsBepCondVar, commPortInUse), m_avtTxObjNo( 0xFF), m_useTxObjNo( false)
{
    // Nothing
};

bool FordCanProtocolFilter::Initialize(const XmlNode *config)
{
    bool retVal = true;

    Log( LOG_FN_ENTRY, "Enter FordCanProtocolFilter::Initialize()\n");

    retVal = ProtocolFilter::Initialize(config);

    try
    {
        if ( NULL != config)
        {
            // Read which TX object to use for our messages
            const XmlNode *txObjNode = config->getChild( "Setup/AVTTxObjectNumber");
            m_avtTxObjNo = (uint8_t)atoi(txObjNode->getValue().c_str());
            if ( m_avtTxObjNo > 14)
            {
                retVal = false;
                throw( BepException( "Invalid transmit object number (%d)", m_avtTxObjNo));
            }
            //MAM 1/23/13 - If Tx object = 0 because were not using the AVT
            if ( 0 == m_avtTxObjNo)
            {
                m_useTxObjNo = false;
                Log( LOG_DEV_DATA, "NOT Using AVT Transmit Object for message transmission\n");
            }
            else
            {
                m_useTxObjNo = true;
                Log( LOG_DEV_DATA, "Using AVT Transmit Object $%02hhX for message transmission\n", m_avtTxObjNo);
            }
        }
        else
        {
            retVal = false;
            throw( XmlException( "NULL configuration node"));
        }
    }
    catch ( XmlException &err)
    {
        Log( LOG_ERRORS, "XmlException in FordCanProtocolFilter::Initialize: %s\n", err.GetReason());
        m_avtTxObjNo = 0xFF;
        m_useTxObjNo = false;               
        //throw;
    }
    catch ( BepException &err)
    {
        Log( LOG_ERRORS, "BepException in FordCanProtocolFilter::Initialize: %s\n", err.GetReason());
        m_avtTxObjNo = 0xFF;
        m_useTxObjNo = false;
        //throw;
    }
    Log( LOG_FN_ENTRY, "Exit FordCanProtocolFilter::Initialize()\n");

    return( retVal);
}

const BEP_STATUS_TYPE FordCanProtocolFilter::SendMessage(std::string messageTag, SerialArgs_t &args)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t xmtMessage;
    // Get the message to be sent
    if (GetXmtMssgString(messageTag, xmtMessage, args) == true)
    {   // Send the message
        status = SendMessage(xmtMessage);
    }
    else
    {   // No message provided for this tag
        Log(LOG_ERRORS,"No message provided for tag: %s\n", messageTag.c_str());
        status = BEP_STATUS_SOFTWARE;
    }
    return(status);
}

const BEP_STATUS_TYPE FordCanProtocolFilter::SendMessage(SerialString_t &message)
{
    // If we have a valid TX object number from the config file
    if ( true == m_useTxObjNo)
    {
        // Add the TX object number on to the beginning of the AVT message
        message = SerialString_t( 1, m_avtTxObjNo) + message;

        Log( "Adding AVT TX Object $%02hhX to message\n", m_avtTxObjNo);
    }
    else
    {
        Log( "NOT Adding AVT TX Object $%02hhX to message\n", m_avtTxObjNo);
    }


    return( ProtocolFilter::SendMessage(message));
}

const BEP_STATUS_TYPE FordCanProtocolFilter::SendMessage(std::string messageTag)
{   
    return( ProtocolFilter::SendMessage(messageTag));
}

void FordCanProtocolFilter::AddChecksumToMessage(SerialString_t &message)
{
    // Nothing
}

const SerialString_t FordCanProtocolFilter::ExtractModuleData( SerialString_t &moduleResponse)
{
    std::string fullResponse;
    // Log the respnse received from the module
    char temp[256];
    for (UINT16 ii = 0; ii < moduleResponse.length(); ii++)
        fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
    Log("Extracting data from response string: %s\n", fullResponse.c_str());
    // Strip off the header and checksum
    //SerialString_t dataResponse = moduleResponse.substr(GetDataHeaderLength(),moduleResponse.length() - GetDataStartIndex());
    //MAM 10/28/08
    SerialString_t dataResponse = moduleResponse.substr(GetDataHeaderLength(),moduleResponse.length() - GetDataHeaderLength());
    // Log the data extracted from the module
    fullResponse.erase();
    for (UINT16 ii = 0; ii < dataResponse.length(); ii++)
        fullResponse += CreateMessage(temp, 256, "$%02X ", dataResponse[ii]);
    Log("ExtractModuleData() Extracted module data: %s\n", fullResponse.c_str());
    // Return the extracted data 
    return dataResponse;        
}

const SerialString_t FordCanProtocolFilter::ExtractModuleData2( SerialString_t &moduleResponse)
{
    std::string fullResponse;
    // Log the respnse received from the module
    char temp[256];
    for (UINT16 ii = 0; ii < moduleResponse.length(); ii++)
        fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
    Log("ExtractModuleData2() - Extracting data from response string: %s\n", fullResponse.c_str());
    // Strip off the header and checksum
    //SerialString_t dataResponse = moduleResponse.substr(GetDataHeaderLength(),moduleResponse.length() - GetDataHeaderLength());
    SerialString_t dataResponse = moduleResponse.substr(GetNegativeResponseCodeIndex()+1,moduleResponse.length() - GetDataHeaderLength());
    // Log the data extracted from the module
    fullResponse.erase();
    for (UINT16 ii = 0; ii < dataResponse.length(); ii++)
        fullResponse += CreateMessage(temp, 256, "$%02X ", dataResponse[ii]);
    Log("ExtractModuleData2() Extracted module data: %s\n", fullResponse.c_str());
    // Return the extracted data 
    return dataResponse;        
}

const BEP_STATUS_TYPE FordCanProtocolFilter::GetModuleDataNew(std::string messageTag, SerialString_t &reply, SerialArgs_t *args)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    std::string asciiMessage;
    SerialString_t xmtMessage;
    //MAM 3/2/10 - Because tries was unsigned, when we subtracted 1 from 0 the value was not -1,
    //but FFFF, which ended up being 65535.
    //UINT16 tries = GetNumberOfRetries() + 1;
    //int  tries = 3;
    //int  tries = 1;
    //UINT16 retries = GetNumberOfRetries(); //MAM 10/29/08

    //MAM 12/22/14
    int retries = 3;

    //MAM 1/6/15
    bool responsePending = false;
    //INT32 retries = 0;
    //INT32 retries = 3;
    //use message specific retries if specified
    //GetResponseFailureRetryCount(messageTag, retries);
    //retries = (retries <= 0) ? GetNumberOfRetries() : retries;

    bool portLocked = false;
    if (GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
    {
        //MAM 10/29/08 - loop and redo if get negative response
        do
        {
            // Lock the port so only this thread uses it
            Log(LOG_DETAILED_DATA, "GetModuleData Locking Port\n");
            if ((portLocked = LockPort()) == true)
            {   // The port was locked
                Log(LOG_DETAILED_DATA, "GetModuleData Locked Port\n");
                //MAM 3/11/10 - Too many retries
                //do
                //{   // Send the message to the module

                //MAM 1/6/15  - don't resend message if watining for a response
                if (false == responsePending)
                {
                    if (args == NULL) status = SendMessage(messageTag);
                    else              status = SendMessage(messageTag, *args);
                    //Log(LOG_DEV_DATA, "tries = %d - Sent message: %s to module - status: %s\n", tries, messageTag.c_str(),
                    //    ConvertStatusToResponse(status).c_str());
                    Log(LOG_DEV_DATA, "(FordCanProtocolFilter) Sent message: %s to module - status: %s\n", messageTag.c_str(),
                        ConvertStatusToResponse(status).c_str());
                }
                else
                {
                    Log(LOG_DEV_DATA, "response pending for message: %s \n", messageTag.c_str());
                }

                // If response expected, call GetResponse(
                if (IsResponseExpected(messageTag))
                {   // Clear the response area                    
                    reply.erase();                    
                    // Set the message ID
                    // If send was successful, get the module response
                    Log(LOG_DEV_DATA, "FordCANProtocolFilter: Looking for response msg - status: %s \n", ConvertStatusToResponse(status).c_str());
                    //if (BEP_STATUS_SUCCESS == status) status = GetResponse(messageTag, reply);
                    if ((BEP_STATUS_SUCCESS == status) || (true == responsePending))
                    {
                        status = GetResponse(messageTag, reply);
                    }
                    // If this is not a valid message, wait a bit beofre trying again
                    if (BEP_STATUS_SUCCESS != status) BposSleep(GetResponseDelay());
                }
                else
                {
                    // If response NOT expected, exit do-while loop
                    status = BEP_STATUS_SUCCESS;
                }

                //}
                //while ((BEP_STATUS_SUCCESS != status) && (tries-- > 0));                
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
            // If a response is expected, verify that response is valid
            if (IsResponseExpected(messageTag))
            {
                // Check if valid message was retrieved
                if ((status == BEP_STATUS_SUCCESS) && (reply.length() > 0))
                {   // Verify a valid response was received
                    status = CheckForValidResponse(reply);
                    // Check for a negative response from the module
                    if (status != BEP_STATUS_SUCCESS) status = CheckForNegativeResponse(reply);
                    // Extract the data from the response if we got positive response
                    if (BEP_STATUS_SUCCESS == status)
                    {
                        reply = ExtractModuleData(reply);
                        retries = 0; //done if we got good data
                    }
                    else    //MAM 7/28/10 - If got negative response from module then delay
                    {
                        Log(LOG_ERRORS, "FordCANProtocolFilter: Got negative response from module - status: %s \n", ConvertStatusToResponse(status).c_str());                        

                        //MAM 11/3/10 - Wait 1500 ms if neg response in busy ($21) otherwise wait 150 ms
                        Log(LOG_ERRORS, "Negative reponse byte = %02X \n", reply[GetNegativeResponseCodeIndex()]);
                        //MAM 12/22/14 - MAM 12/22/14                        
                        //if ( (0x21 == reply[GetNegativeResponseCodeIndex()]) || (0x78 == reply[GetNegativeResponseCodeIndex()]) )
                        if (0x21 == reply[GetNegativeResponseCodeIndex()])
                        {
                            Log(LOG_ERRORS, "Delaying 1500 ms for negative rsponse (busy) \n");
                            responsePending = false;
                            BposSleep(1500);
                        }
                        else if (0x78 == reply[GetNegativeResponseCodeIndex()])
                        {
                            Log(LOG_ERRORS, "Response Pending \n");
                            //responsePending = true;
                            //MAM 1/9/15 - temporary fake pass until I have time to fix it
                            status = BEP_STATUS_SUCCESS;
                            BposSleep(1800);
                            retries = 0; //done
                        }
                        else
                        {
                            Log(LOG_ERRORS, "Delaying 150 ms for negative rsponse \n");
                            responsePending = false;
                            BposSleep(150);
                        }
                    }
                }
                else if (status != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_ERRORS, "retries = %d - Error getting data from module: Message - %s, status: %s\n",
                        retries, messageTag.c_str(), ConvertStatusToResponse(status).c_str());
                }
                else if (reply.length() <= 0)
                {
                    Log(LOG_ERRORS, "Did not receive a valid response from the module\n");
                    status = BEP_STATUS_FAILURE;
                }
            }
            else
            {
                // If response NOT expected, return Success
                status = BEP_STATUS_SUCCESS;
                retries = 0; //done if we don't expect a response
            }

            //MAM 3/1/10
            Log(LOG_DEV_DATA, "retries = %d \n", retries);
        }
        while (--retries > 0);  //MAM 3/11/10
        //while (retries-- > 0);
    }
    else
    {   // No message provided for this tag
        Log(LOG_ERRORS, "No message provided for tag: %s\n", messageTag.c_str());
        status = BEP_STATUS_SOFTWARE;
    }
    Log(LOG_DETAILED_DATA, "GetModuleData() returning - status: %s\n", ConvertStatusToResponse(status).c_str());
    return(status);
}

const BEP_STATUS_TYPE FordCanProtocolFilter::GetModuleData(std::string messageTag, SerialString_t &reply, SerialArgs_t *args)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    std::string asciiMessage;
    SerialString_t xmtMessage;

    //int retries = 3;    
    int tries = 3;    
    bool responsePending = false;
    //INT32 responsePendingReads = 3;    
    INT32 responsePendingReads;
    bool portLocked = false;

    if (GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
    {
        do
        {
            // Lock the port so only this thread uses it
            Log(LOG_DETAILED_DATA, "GetModuleData Locking Port\n");
            if ((portLocked = LockPort()) == true)
            {   // The port was locked
                Log(LOG_DETAILED_DATA, "GetModuleData Locked Port\n");                

                // Send the message to the module
                if (args == NULL) status = SendMessage(messageTag);
                else              status = SendMessage(messageTag, *args);                
                Log(LOG_DEV_DATA, "(FordCanProtocolFilter) Sent message: %s to module - status: %s\n", messageTag.c_str(),
                    ConvertStatusToResponse(status).c_str());                

                bool getResponse = true;
                responsePendingReads = 3;

                do
                {
                    //if (IsResponseExpected(messageTag))
                    if ((IsResponseExpected(messageTag)) && (getResponse))
                    {   // Clear the response area                    
                        reply.erase();                    
                        // Set the message ID
                        // If send was successful, get the module response
                        Log(LOG_DEV_DATA, "FordCANProtocolFilter: Looking for response msg - status: %s \n", ConvertStatusToResponse(status).c_str());
                        //if (BEP_STATUS_SUCCESS == status) status = GetResponse(messageTag, reply);                      
                        if ((BEP_STATUS_SUCCESS == status) || (true == responsePending))
                        {
                            status = GetResponse(messageTag, reply);
                        }

                        // If this is not a valid message, wait a bit beofre trying again
                        //if (BEP_STATUS_SUCCESS != status) BposSleep(GetResponseDelay());
                    }
                    else
                    {
                        // If response NOT expected, exit do-while loop
                        status = BEP_STATUS_SUCCESS;
                    }                    

                    // If a response is expected
                    if (IsResponseExpected(messageTag))
                    // Make sure the response matches a filter before processing it
                    //if (IsResponseValid(messageTag, reply))
                    {
                        // Check if valid message was retrieved
                        if ((status == BEP_STATUS_SUCCESS) && (reply.length() > 0))
                        {
                            status = CheckForValidResponse(reply);
                        }

                        // Check for a negative response from the module
                        if ((status != BEP_STATUS_SUCCESS) && (reply.length() > 0))
                        {
                            status = CheckForNegativeResponse(reply);
                        }

                        //MAM 3/19/15 - If we have response pending followed by another repsonse concatenated together
                        if (status != BEP_STATUS_SUCCESS)
                        {
                            if ((0x78 == reply[GetNegativeResponseCodeIndex()]) && (reply.length() > GetNegativeResponseCodeIndex()+1) )
                            {
                                reply = ExtractModuleData2(reply);
                                status = CheckForValidResponse(reply);
                                // Check for a negative response from the module
                                if ((status != BEP_STATUS_SUCCESS) && (reply.length() > 0))
                                {
                                    status = CheckForNegativeResponse(reply);
                                }
                            }

                        }

                        //if ((status == BEP_STATUS_SUCCESS) && (reply.length() > 0))
                        if (status == BEP_STATUS_SUCCESS)
                        {
                            reply = ExtractModuleData(reply);
                        }
                        //if ((BEP_STATUS_FAILURE == status) || (responsePending && (BEP_STATUS_SUCCESS != status) && (reply.length() > 0)))
                        else
                        {
                            Log(LOG_ERRORS, "FordCANProtocolFilter: Got negative response from module - status: %s \n", ConvertStatusToResponse(status).c_str());                        
                            // Check what the negative response was
                            //UINT8 negativeResponseCode = reply[GetNegativeResponseCodeIndex()];                            
                            //MAM 11/3/10 - Wait 1500 ms if neg response in busy ($21) otherwise wait 150 ms
                            Log(LOG_ERRORS, "Negative reponse byte = %02X \n", reply[GetNegativeResponseCodeIndex()]);

                            if (0x21 == reply[GetNegativeResponseCodeIndex()])
                            {
                                Log(LOG_ERRORS, "Delaying 1500 ms for negative rsponse $21 (busy) \n");
                                //responsePending = false;
                                BposSleep(1500);
                            }
                            else if (0x78 == reply[GetNegativeResponseCodeIndex()])
                            {
                                // Check if more than one message was returned in GetResponse                                
                                //if (reply.length() > 0)
                                //{   // More than one response in the buffer, extract and evaluate
                                //    getResponse = false;
                                //}
                                //else
                                //{   // Only decrement response pending reads if we need to look for data in the buffer
                                responsePendingReads--;
                                getResponse = true;
                                //}
                                // Just need to look for the response again
                                status = BEP_STATUS_NA;
                                responsePending = true;
                                Log(LOG_DEV_DATA, "Handling response pending code - status: %s, responsePending: %s, getResponse: %s, responsePendingReads: %d",
                                    ConvertStatusToResponse(status).c_str(), responsePending ? "True" : "False", 
                                    getResponse ? "True" : "False", responsePendingReads);

                                if (getResponse)
                                {
                                    //Log(LOG_ERRORS, "Delaying 1800 ms for response pending $78 (busy) \n");
                                    //responsePending = false;
                                    //BposSleep(1800);
                                    Log(LOG_ERRORS, "Delaying 150 ms for response pending \n");                                   
                                    BposSleep(150);
                                }

                            }
                            else
                            {
                                Log(LOG_ERRORS, "Delaying 150 ms for negative rsponse \n");
                                //responsePending = false;
                                BposSleep(150);
                            }

                        }
                    }
                    else
                    {
                        // If response NOT expected, return Success
                        status = BEP_STATUS_SUCCESS;
                        //retries = 0; //done if we don't expect a response
                        tries = 0;
                    }

                    // If this is not a valid message, wait a bit before trying again
                    //if (BEP_STATUS_SUCCESS != status) BposSleep(GetResponseDelay());

                    // Keep checking until we get a good message or response pending reads run out
                    Log(LOG_DEV_DATA, "Determining if we need to loop for more data in buffer - status: %s, responsePendingReads: %d, responsePending: %s, GetStopCommFlag: %s",
                        ConvertStatusToResponse(status).c_str(), responsePendingReads, responsePending ? "True" : "False", 
                        GetStopCommsFlag() ? "True" : "False");

                } while ((BEP_STATUS_NA == status) && ((responsePendingReads > 0) && responsePending) && !GetStopCommsFlag());
                // Exit the critical section since we are done with the port for now
                //m_commsInUse->Release();
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


            //MAM 3/1/10
            Log(LOG_DEV_DATA, "tries = %d \n", tries);       
            //} while (--retries > 0);
            // Keep trying until a good response s received we run out of attempts or are signalled to stop sending messages
        } while ((BEP_STATUS_SUCCESS != status) && (--tries > 0) && !GetStopCommsFlag());

        // Check if valid message was retrieved
        if (GetStopCommsFlag())
        {
            Log(LOG_ERRORS, "Module Signaled to stop communications:StopCommsFlag - %s, Message - %s, status: %s\n", 
                GetStopCommsFlag() ? "true" : "false", messageTag.c_str(), ConvertStatusToResponse(status).c_str());
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
    {   // No message provided for this tag
        Log(LOG_ERRORS, "No message provided for tag: %s\n", messageTag.c_str());
        status = BEP_STATUS_SOFTWARE;
    }

    Log(LOG_DETAILED_DATA, "GetModuleData() returning - status: %s\n", ConvertStatusToResponse(status).c_str());
    return(status);
}

const BEP_STATUS_TYPE FordCanProtocolFilter::GetBusBroadcastMessage(string messageTag,
																   const long messageWaitTime,
																   SerialString_t &busMssg)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	INT32 replyBytes = 0;
	NotifyEvent_t   mssgEvent;
	struct timespec currentTime;
	struct timespec startTime;
	Log(LOG_FN_ENTRY, "Enter FordCanProtocolFilter::GetBusBroadcastMsg(%s)", messageTag.c_str());
	// Bogus event (because we are going to poll)
	SIGEV_NONE_INIT( &mssgEvent);
	// Subscribe for RX data that matches the pattern defined by the messageTag
	//  message under the <ReplyMessages> node
	ILogicalPort::FilterSubscribe(mssgEvent, messageTag);
	// Get the start time
	if(clock_gettime( CLOCK_REALTIME, &startTime) == -1)
	{
		throw BepException("Clock Gettime Error: %s", strerror(errno));
	}
	// Poll the comm proxy for the message
	bool validMessage = false;
	bool timeRemaining = true;
	double elapsedTime = 0;
	do
	{	// Read the port for the message
		ResetConnection();
		replyBytes = ReadPort(messageTag, busMssg);
		// Check the message
		if(replyBytes > 0)
		{
			string fullResponse = "";
			char temp[256];
			for(UINT16 ii = 0; ii < busMssg.length(); ii++)
				fullResponse += CreateMessage(temp, 256, "$%02X ", busMssg[ii]);
			Log( LOG_DEV_DATA, "GetBusBroadcastMessage Reply Received: <%s>\n", fullResponse.c_str());
			validMessage = IsResponseValid(messageTag, busMssg);
		}
		// Get the elapsed time
		clock_gettime( CLOCK_REALTIME, &currentTime);
		elapsedTime = (currentTime.tv_sec - startTime.tv_sec ) * 1000 + ( currentTime.tv_nsec - startTime.tv_nsec ) / 1000000;
		timeRemaining = messageWaitTime > elapsedTime;
		// Check if we should continue waiting for the message
	} while(!validMessage && timeRemaining);
	// Unsubscribe from the port
	PortUnsubscribe(PORT_SUBSCRIBE_RX);
	// Determine the status to return
	status = validMessage ? BEP_STATUS_SUCCESS : BEP_STATUS_TIMEOUT;
	Log(LOG_FN_ENTRY, "Exiting FordCanProtocolFilter::GetBusBroadcastMsg(%s) - status: %s", messageTag.c_str(), ConvertStatusToResponse(status).c_str());
	return status;
}
