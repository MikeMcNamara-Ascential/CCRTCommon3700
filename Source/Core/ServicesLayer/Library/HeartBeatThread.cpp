//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/HeartBeatThread.cpp 1     1/16/04 4:51p Cward $
//
// FILE DESCRIPTION:
//   Heartbeat thread class
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
//    $Log: /Core/ServicesLayer/Library/HeartBeatThread.cpp $
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//*************************************************************************

#include "HeartBeatThread.h"

HeartBeatThread::HeartBeatThread(void) : AtomicThread(PTHREAD_CREATE_DETACHED), ILogger(), m_hbComm(NULL)
{
}

HeartBeatThread::HeartBeatThread(const XmlNode *document, BepServer* server /*= NULL*/)
    : AtomicThread(PTHREAD_CREATE_DETACHED), ILogger(), m_hbComm(NULL), m_server(server)
{
    Initialize(document);
}

HeartBeatThread::~HeartBeatThread(void)
{
}

void HeartBeatThread::Initialize(const XmlNode *document, BepServer* server /*= NULL*/)
{
    if(document)
    {
        const XmlNode* setup = document->getChild("Setup/HeartBeatParameters/Setup");
        ILogger::Initialize(document->getChild("Setup/HeartBeatParameters"));
        m_heartbeatSampleDelay = BposReadInt(setup->getChild("HeartbeatSamplingInterval")->getValue().c_str());
        m_reflectedHbTag = setup->getChild("ReflectedHeartbeatTag")->getValue();
        m_HbTag = setup->getChild("HeartbeatTag")->getValue();
    }
    else Log(LOG_ERRORS, "Document passed in was NULL\n");
}

void* HeartBeatThread::Execute(void)
{	// Create the named data broker object
    try
    {
        if(m_hbComm == NULL) m_hbComm = new INamedDataBroker(GetLocalEcho() && GetLogStatus());

        while(GetStatus() != BEP_TERMINATE_STATUS)
        {
            UpdateHeartbeat();
            BposSleep(GetSamplingDelay());	// Wait for the next time we need to check status
        }
    }
    catch(BepException& Err)
    {
        Log("HeartbeatThread::Execute() failed: %s\n", Err.what());
    }
    catch(...)
    {
        Log("HeartbeatThread::Execute() failed for unknown reason\n");
    }
    return(NULL);
}

const UINT32 HeartBeatThread::GetSamplingDelay(void)
{
    return(m_heartbeatSampleDelay);
}

void HeartBeatThread::UpdateHeartbeat(void)
{	// Read reflected heartbeat tag and heartbeat tag
    const std::string& reflectedHb = m_server->ReadSubscribeData(m_reflectedHbTag);	
    std::string Hb = m_server->ReadSubscribeData(m_HbTag);

    if(reflectedHb == Hb)
    {	// PLC matched our bit
        Log(LOG_DEV_DATA, "Toggle required because PLC matched\n");

        Hb = (Hb == "1") ? "0" : "1";	// Toggle it

        if(m_hbComm)
        {
            std::string response;
            if(m_hbComm->Write(m_HbTag, Hb, response, true) == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA, "Wrote %s to %s\n", m_HbTag.c_str(), Hb.c_str());
            }
            else Log(LOG_ERRORS, "Failed to write %s to %s\n", m_HbTag.c_str(), Hb.c_str());
        }
        else Log(LOG_ERRORS, "Failed to write %s to %s because named data broker object was NULL\n");
    }
    else Log(LOG_ERRORS, "PLC has not matched\n");
}
