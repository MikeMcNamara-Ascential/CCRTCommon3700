//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Library/Registrar.cpp $
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
//     $Log: /Core/ServicesLayer/Library/Registrar.cpp $
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include "Registrar.h"



Registrar::Registrar(const bool debug) : Debuggable(debug)
{
}
/**
 * Class Destructor
 * @since Version 1.0
 */
Registrar::~Registrar(void)
{
}
/**
 * Handle the register request
 *
 * @param data   Xml node containing data
 * @param component Component used to handle communications
 * @since Version 1.0
 */
const INT32 Registrar::HandleRequest(XmlNode* data, BepComponent* component, std::string& message)
{
    const XmlNodeMap& kids = data->getChildren();
    for(XmlNodeMapCItr iter = kids.begin(); iter != kids.end(); ++iter)
    {
        XmlNode* node = const_cast<XmlNode*>(iter->second);
        node->setValue(component->Register());
    }
    data->writeNode(message);
    return(BEP_STATUS_SUCCESS);
}
