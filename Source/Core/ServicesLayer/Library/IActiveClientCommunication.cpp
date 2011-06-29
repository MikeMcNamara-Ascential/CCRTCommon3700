//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/IActiveClientCommunication.cpp 1     1/16/04 4:51p Cward $
//
// FILE DESCRIPTION:
//		This is a class which handles communication with an active client.
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Library/IActiveClientCommunication.cpp $
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//*************************************************************************

#include "IActiveClientCommunication.h"

#include <typeinfo>
#include <sys/syspage.h>
#include <sys/neutrino.h>
#include <sys/resource.h>

#include "BepTimer.h"
#include "XmlParser.h"

#include "Reader.h"
#include "Writer.h"
#include "Commander.h"
#include "Registrar.h"
#include "Subscriber.h"

#define PROFILING_ON (0)

	
IActiveClientCommunication::IActiveClientCommunication(void) : IBepCommunication()
{
}

IActiveClientCommunication::~IActiveClientCommunication(void)
{
}

INT32 IActiveClientCommunication::Publish(const XmlNode *node,
              std::string &response, const bool IsFinal /*= false*/)
{
    if(IsDebugOn())	printf("IActiveClientCommunication::Publish(%s)\n", node->ToString().c_str());
    XmlNode* responseNode = const_cast<XmlNode *>(m_parser.getDocElement());
    try
    {
        try
        {
            XmlNode* attribute = const_cast<XmlNode *>(responseNode->getAttributes().getNode(BEP_XML_MSG_TYPE));
            if(attribute->getValue() != BEP_PUBLISH)
            {
                attribute->setValue(BEP_PUBLISH);
            }
        }
        catch(BepException& BepErr)
        {
            responseNode->addChild(BEP_XML_MSG_TYPE, BEP_PUBLISH, ATTRIBUTE_NODE);
        }
        if(responseNode->getChildren().size() != 0)
        {
            XmlNodeMap& kids = const_cast<XmlNodeMap&>(responseNode->getChildren());
            kids.clear(true);
        }
        responseNode->addChild(node->Copy());
        if(IsFinal)
        {
            response.erase();						// Clear the string to be sent
            responseNode->writeNode(response);		// Set the data
            m_ISystemCommunication->Write(response);
        }
    }
    catch(BepException& BepErr)
    {
        if(IsDebugOn())
            printf("IActiveClientCommunication::Publish(%s) failed because %s\n",
                   node->ToString().c_str(), BepErr.what());
    }
    return(BEP_STATUS_SUCCESS);
}

const bool IActiveClientCommunication::SendResponse(XmlNode* doc, BepComponent* component)
{
    bool stat = false;

    std::string type, message;
    message.reserve(STRING_RESERVE_SIZE);

    if(IsDebugOn())	printf("IActiveClientCommunication::SendResponse(%s, %p)\n", doc->ToString().c_str(), component);
    try
    {
        type = GetRequestType(doc);	// Get request type

        if(type == BEP_READ)
        {
            Reader worker(doc, GetDataRate(), IsDebugOn());
            if(GetDataRate() > 0)
            {
#if (PROFILING_ON)
    #warning Profiling on
                uint64_t stamp1=0, stamp2=0;
                int slowDown = 0;
#endif
                for(;;)
                {
#if (PROFILING_ON)
                    stamp2 = ClockCycles();
                    if((slowDown++ % 50) == 0)
                        printf("cycle = %2.2f  id = %d\n",
                               1000.0 * ((double) (stamp2-stamp1)) / (double)SYSPAGE_ENTRY(qtime)->cycles_per_sec,
                               pthread_self());
                    stamp1 = stamp2;
#endif
                    worker.HandleRequest(doc, this, message);
                    IBepCommunication::SendResponse(message);
                }
            }
            else
            {
                worker.HandleRequest(doc, this, message);
                IBepCommunication::SendResponse(message);
            }
        }
        else if(type == BEP_SUBSCRIBE)
        {
            Subscriber worker(IsDebugOn());
            worker.HandleRequest(doc, this, message);
            IBepCommunication::SendResponse(message);
        }
        else if(type == BEP_COMMAND)
        {
            Commander worker(IsDebugOn());
            worker.HandleRequest(doc, component, message);
            IBepCommunication::SendResponse(message);
        }
        else if(type == BEP_WRITE)
        {
            Writer worker(IsDebugOn());
            worker.HandleRequest(doc, component, message);
            IBepCommunication::SendResponse(message);
        }
        else if(type == BEP_REGISTER)
        {
            Registrar worker(IsDebugOn());
            worker.HandleRequest(doc, component, message);
            IBepCommunication::SendResponse(message);
        }
        else printf("Unknown request type of %s in IActiveClientComunication::SendResponse()\n", type.c_str());
    }
    catch(BepException &BepErr)
    {
        if(IsDebugOn()) printf("Error in IActiveClientCommunication::SendResponse(%s): %s\n", message.c_str(), BepErr.what());
        throw;
    }
    catch(...)
    {
        if(IsDebugOn())	printf("Error in IActiveClientCommunication::SendResponse(%s)\n", message.c_str());
        throw;
    }
    return(stat);
}
inline void IActiveClientCommunication::SetClientClosed()
{
    ITcpIp* tcp = NULL;
    if(dynamic_cast<ITcpIp *>(m_ISystemCommunication) != NULL)
    {
        tcp = dynamic_cast<ITcpIp *>(m_ISystemCommunication);
        tcp->SetCloseOnExit(true);
    }
}

