//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/Commander.cpp 1     1/16/04 4:51p Cward $
//
// FILE DESCRIPTION:
//		This is a class which handles subscriptions for an active client.
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
//    $Log: /Core/ServicesLayer/Library/Commander.cpp $
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//*************************************************************************
#include "Commander.h"

Commander::Commander(const bool debug) : Debuggable(debug)
{
}

Commander::~Commander(void)
{
}

const INT32 Commander::HandleRequest(XmlNode* data, BepComponent* component, std::string& message)
{
    const XmlNodeMap& kids = data->getChildren();
    for(XmlNodeMapCItr iter = kids.begin(); iter != kids.end(); ++iter)
    {
        XmlNode* node = const_cast<XmlNode*>(iter->second);
        node->setValue(component->Command(node));
    }
    data->writeNode(message);
    return(BEP_STATUS_SUCCESS);
}
