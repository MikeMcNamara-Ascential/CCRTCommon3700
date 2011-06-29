//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/Commander.h 2     1/16/04 4:45p Cward $
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
//    $Log: /Core/ServicesLayer/Include/Commander.h $
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:07p Khykin
// Initial checkin.
//
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:35  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:03a Khykin
// Initial Checkin
//
// 1     1/18/03 8:01a Khykin
// Initial Checkin From DCTC
//
// 2     8/23/02 11:39a Kh
// Synchronizing with the pit machine.
//
// 1     4/08/02 1:49p Skypig13
// Class to handle commands for the data server
//
//*************************************************************************
#ifndef Commander_h
#define Commander_h

#include "Debuggable.h"
#include "BepComponent.h"
#include "XmlNode.h"

class CommanderTest;
/**
 * Class responsible for hadling command requests for data server
 *
 * @author Tim Bochenek
 * @author Carl Erickson
 * @version 1.1
 * @since Version 1.0
 */
class Commander : public Debuggable
{
public:
	friend class CommanderTest;
    /**
     * Class constructor (default)
     *
     * @param debug  Turn diagnostics on
     * @since Version 1.0
     */
    Commander(const bool debug);

    /**
     * Class destructor
     *
     * @since Version 1.1
     */
    virtual ~Commander(void);

    /**
     * Handle the command request
     *
     * @param data    Xml node containing data
     * @param component    Component to do writing for us
     * @param message response message
     * @return Status of operation
     * @since Version 2.0
     */
    virtual const INT32 HandleRequest(XmlNode* data, BepComponent* component, std::string& message);

};

#endif // Commander_h
