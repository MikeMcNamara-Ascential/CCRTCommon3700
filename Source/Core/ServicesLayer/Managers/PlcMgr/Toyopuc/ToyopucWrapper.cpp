//*************************************************************************
// FILE:
//
//
// FILE DESCRIPTION:
//      Ethernet/IP Wrapper Class
//
//===========================================================================
// Copyright (c) 2007- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
// 
// 
//*************************************************************************
#include "ToyopucWrapper.h"

//#define TRACE_FILE_OUTPUT
//#define TRACE_OUTPUT

//---------------------------------------------------------------------------------------------------------------------
ToyopucWrapper::ToyopucWrapper(ILogger *logger)
{
    if(logger == NULL)
    {
        throw BepException("Null logger for Toyopuc wrapper");
    }
    m_logger = logger;
    m_terminate = false;
}

//---------------------------------------------------------------------------------------------------------------------
ToyopucWrapper::~ToyopucWrapper()  
{
    m_terminate = true;
}

//---------------------------------------------------------------------------------------------------------------------
bool ToyopucWrapper::Initialize()
{
    bool retStat = false;
    m_logger->Log(LOG_DEV_DATA, "ToyopucWrapper::Initialize() Enter\n");
    // startup the adapter
    if (clientStart())
    {
        pthread_attr_t attr;            // the attribute of the thread
        pthread_attr_init( &attr );
        pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
        if(pthread_create( &m_toyoThreadId, &attr, ToyopucWrapper::EipTask, this) != EOK)
        {
            m_toyoThreadId = -1;
            m_logger->Log(LOG_ERRORS, "Could Not Create Force Thread");
        }
        
        retStat = true;
        clientClaimAllHostIPAddress();
    }
    else
    {
        m_logger->Log(LOG_ERRORS, "Could not start the client!");
    }
    m_logger->Log(LOG_DEV_DATA, "ToyopucWrapper::Initialize() exit\n");
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool ToyopucWrapper::SendPlcData(const uint8_t *plcData, short byteCnt, const string &plcIpAddress, string plcTag)
{
    INT32 readId = 0;
    INT32 retStat = -1;
    m_logger->Log(LOG_DEV_DATA,"ToyopucWrapper::SendPlcData() enter\n");
    // build the network path
    string netPath = plcIpAddress + ",1,0";
	m_logger->Log(LOG_DEV_DATA,"ToyopucWrapper::SendPlcData - Setting network path to %s", netPath.c_str());
    char path[MAX_NETWORK_PATH_SIZE];
    EtIPObjectRequest req;
    memset(req.networkPath, 0, sizeof(req.networkPath));
    memset(req.requestTag, 0, sizeof(req.requestTag));
    memset(req.requestData, 0, sizeof(req.requestData));
    strcpy(path, netPath.c_str());
    for(UINT16 index = 0; index < netPath.length(); index++)
    {
        req.networkPath[index] = path[index];
    }
    req.bService = 0x4D;
    req.iClass = INVALID_CLASS;
    req.iInstance = INVALID_INSTANCE;
    req.iAttribute = INVALID_ATTRIBUTE;
    req.iMember = INVALID_MEMBER;   // req.iMember = 0;
    for(UINT16 index = 0; index < plcTag.length(); index++)
    {
        req.requestTag[index] = plcTag[index];
    }
    req.iTagSize = plcTag.length();
    req.requestData[0] = abINT;  // Sending 16 bit words;
    req.requestData[1] = 0x00;
    req.requestData[2] = (byteCnt/2) & 0x00FF;
    req.requestData[3] = ((byteCnt/2) & 0xFF00) >> 8;
    for (int i=0; i<byteCnt; i++)
    {
        req.requestData[i + 4] = plcData[i];
    }
    req.iDataSize = byteCnt + 4;
    req.lExplicitMessageTimeout = 0;
    req.lHostIPAddr = INADDR_NONE;

    // looks like everything above here is just for setting up the request.  Can probably replace most of this with pertinent info for the SerialChannel send
    // next line appears to be the sending of the request then you end up getting
    if((readId = clientSendUnconnectedRequest(&req)) < REQUEST_INDEX_BASE)
    {
        return false;
    }
    else
    {
        BposSleep(30);
        EtIPObjectResponse response;
		short attempts = 20;
		short msgDelay = 10;
		while(((retStat = clientGetUnconnectedResponse(readId, &response)) != 0) && attempts--)
		{
			BposSleep(msgDelay);
		}
        if(retStat == 0)
        {
            if(response.bGeneralStatus > 0)
            {
                m_logger->Log(LOG_ERRORS, "Request error, general status: 0x%02X\n", response.bGeneralStatus);
                if(response.iExtendedStatusDataSize) m_logger->Log(LOG_ERRORS, "Extended error code: 0x%X\n", response.iExtendedStatus);
                m_logger->Log(LOG_ERRORS, "Error description: %s\n", response.errorDescription);
                return false;
            }
        }
        else
        {
            m_logger->Log(LOG_ERRORS, "Error getting send status from the PLC: %d", retStat);
            return false;
        }
    }
    m_logger->Log(LOG_DEV_DATA,"ToyopucWrapper::SendPlcData() exit\n");
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
INT32 ToyopucWrapper::ReceivePlcData(uint8_t *plcData, const short &byteCnt, INT32 &bytesRead,
                                 const string &plcIpAddress, const string &plcTag)
{
    INT32 readId = 0;
    INT32 retStat = -1;
    m_logger->Log(LOG_DEV_DATA,"ToyopucWrapper::ReceivePlcData() enter\n");
    // build the network path
    string netPath = plcIpAddress + ",1,0";
    char path[MAX_NETWORK_PATH_SIZE];
    EtIPObjectRequest req;
    memset(req.networkPath, 0, sizeof(req.networkPath));
    memset(req.requestTag, 0, sizeof(req.requestTag));
    memset(req.requestData, 0, sizeof(req.requestData));
    strcpy(path, netPath.c_str());
    for(UINT16 index = 0; index < netPath.length(); index++)
    {
        req.networkPath[index] = path[index];
    }
    req.bService = 0x4C;
    req.iClass = INVALID_CLASS;
    req.iInstance = INVALID_INSTANCE;
    req.iAttribute = INVALID_ATTRIBUTE;
    req.iMember = INVALID_MEMBER;   // req.iMember = 0;
    for(UINT16 index = 0; index < plcTag.length(); index++)
    {
        req.requestTag[index] = plcTag[index];
    }
    req.iTagSize = plcTag.length();
    req.iDataSize = 2;
    req.lExplicitMessageTimeout = 0;
    req.lHostIPAddr = INADDR_NONE;
    req.requestData[0] = (byteCnt/2) & 0x00FF;
    req.requestData[1] = ((byteCnt/2) & 0xFF00) >> 8;
    if((readId = clientSendUnconnectedRequest(&req)) >= REQUEST_INDEX_BASE)
    {   // Successfully sent the request, wait for the response
        BposSleep(15);
        EtIPObjectResponse response;
		short attempts = 20;
		short msgDelay = 10;
		while(((retStat = clientGetUnconnectedResponse(readId, &response)) != 0) && attempts--)
		{
			BposSleep(msgDelay);
		}
        if(retStat == 0)
        {
            if(response.bGeneralStatus > 0)
            {
                m_logger->Log(LOG_ERRORS, "Request error, general status: 0x%02X\n", response.bGeneralStatus);
                if(response.iExtendedStatusDataSize) m_logger->Log(LOG_ERRORS, "Extended error code: 0x%X\n", response.iExtendedStatus);
                m_logger->Log(LOG_ERRORS, "Error description: %s\n", response.errorDescription);
                retStat = -1;
            }
            else
            {
                bytesRead = response.iDataSize;
                for (int ii=0; ii<byteCnt; ii++)
                {
                    plcData[ii] = response.responseData[ii+2];  // First two bytes are tag type, do not need this data.
                }
            }
        }
        else
        {
            m_logger->Log(LOG_ERRORS, "Error reading data from the PLC: %d", retStat);
        }
    }
    else
    {
        m_logger->Log(LOG_ERRORS, "Error sending read request to the PLC: %d", readId);
    }
    m_logger->Log(LOG_DEV_DATA,"ToyopucWrapper::ReceivePlcData() exit\n");
    return retStat;
}

//---------------------------------------------------------------------------------------------------------------------
void ToyopucWrapper::Close()
{
    // shutdown the adapter
    m_terminate = true;
    clientStop();
    // let the stack close any connections
    platformSleep(100);
}

//---------------------------------------------------------------------------------------------------------------------
void* ToyopucWrapper::EipTask(void *param)
{
    ToyopucWrapper *eip = (ToyopucWrapper*)param;
    while(!eip->m_terminate)
    {
        clientMainTask();
        BposSleep(2);
    }
}
