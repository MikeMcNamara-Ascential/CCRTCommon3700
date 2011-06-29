//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Library/Reader.cpp $
// $Author: Cward $
// $Date: 1/16/04 4:51p $
//
// Description:
//		This is a class which handles communication with an active client.
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
//     $Log: /Core/ServicesLayer/Library/Reader.cpp $
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include "Reader.h"

#include "IBepCommunication.h"


/**
 * Class constructor
 * @since Version 1.0
 */
Reader::Reader(const XmlNode* doc, const UINT32 rate, const bool debug) : Debuggable(debug)
{
    SetRate(rate);
    doc->writeNode(m_message);
    IBepCommunication comm;
    comm.Initialize(NAMED_DATA_BROKER_NAME, std::string("Client"));
    std::string message, tag, name;
    comm.Read(doc->getChildren().getNode(0), message, true);
    comm.GetNext(tag, name, message);
    m_ipc.Initialize(name, std::string("Client"), IsDebugOn());
}
/**
 * Class Destructor
 * @since Version 1.0
 */
Reader::~Reader(void)
{
}
/**
 * Set data rate at which reader will read data
 *
 * @param rate   Data rate to read data (messages per second)
 * @since Version 1.0
 */
void Reader::SetRate(const UINT32 rate)
{
    if(rate > MAX_RATE)
    {
        m_rate = MAX_RATE;
        if(IsDebugOn())
        {
            printf("Reader::SetRate(%u)\n\tWarning can't set rate to greater than %u, will set to %u\n",
                   rate, MAX_RATE, MAX_RATE);
        }
    }
    else if(rate < MIN_RATE)
    {
        m_rate = MIN_RATE;
        if(IsDebugOn())
        {
            printf("Reader::SetRate(%u)\n\tWarning can't set rate to less than %u, will set to %u\n",
                   rate, MIN_RATE, MIN_RATE);
        }
    }
    else
    {
        m_rate = rate;
    }
}
/**
 * Retrieve the data rate
 *
 * @return Data rate
 * @since Version 1.2
 */
const UINT32 Reader::GetRate(void)
{
    return(m_rate);
}
/**
 * Handle a read request
 *
 * @param data    Xml node containing data
 * @param comm    Communication object
 * @param message response message
 * @return Status of operation
 * @since Version 2.0
 */
const INT32 Reader::HandleRequest(XmlNode* data, IPublish* comm, std::string& message)
{
    Delay();
    m_ipc.Write(m_message);
    return(m_ipc.Read(message));
}

/**
 * Delay for the appropriate amount of time for our data rate
 * @since Version 1.0
 */
void Reader::Delay(void)
{
    BposSleep(1000/m_rate);
}


