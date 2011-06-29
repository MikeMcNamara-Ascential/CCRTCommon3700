//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/Registrar.h 3     10/30/07 3:48p Rwiersem $
//
// FILE DESCRIPTION:
//      This is a class which handles communication with an active client.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/Registrar.h $
// 
// 3     10/30/07 3:48p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:20p Khykin
// Initial checkin.
//
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:45  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:09a Khykin
// Initial Checkin
//
// 1     1/18/03 8:06a Khykin
// Initial Checkin From DCTC
//
// 1     6/19/02 2:20p Skypig13
// New class for hadling Register requests for DataServer
//
//*************************************************************************
#ifndef Registrar_h
#define Registrar_h

#include <string>

#include "BepDefs.h"
#include "Debuggable.h"
#include "XmlNode.h"
#include "BepComponent.h"

/**
 * Class that will handle registering
 *
 * @author Tim Bochenek
 * @version 1.0
 * @since Version 1.0
 */
class Registrar : public Debuggable
{
public:
    /**
     * Class constructor
     * @since Version 1.0
     */
    Registrar(const bool debug);

    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~Registrar(void);

    /**
     * Handle the register request
     *
     * @param data      Xml node containing data.
     * @param component Component used to handle communications.
     * @param message   Message to write to the xml node.
     */
    virtual const INT32 HandleRequest(XmlNode* data, BepComponent* component, std::string& message);

};

#endif // Registrar_h
