//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/Writer.h 3     10/30/07 3:53p Rwiersem $
//
// FILE DESCRIPTION:
//      This is a class which handles subscriptions for an active client.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/Writer.h $
// 
// 3     10/30/07 3:53p Rwiersem
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
// 1     6/12/03 4:27p Khykin
// Initial checkin.
//
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:49  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:11a Khykin
// Initial Checkin
//
// 1     1/18/03 8:08a Khykin
// Initial Checkin From DCTC
//
// 2     7/31/02 11:55a Skypig13
// Added test class and Constructor/Destructor pair
//
// 1     4/08/02 1:47p Skypig13
// New class to handle write operations
//
//*************************************************************************
#ifndef Writer_h
#define Writer_h

#include "Debuggable.h"
#include "BepComponent.h"

class WriterTest;

/**
 * Class responsible for hadling write requests for data server
 *
 * @author Tim Bochenek
 * @author Carl Erickson
 * @version 1.0
 * @since Version 1.0
 */
class Writer : public Debuggable
{
public:
    friend class WriterTest;
    /**
     * Class constructor (default)
     *
     * @param debug  Turn diagnostics on
     * @since Version 1.0
     */
    Writer(const bool debug);

    /**
     * Class destructor
     *
     * @since Version 1.0
     */
    virtual ~Writer(void);

    /**
     * Handle the write request.
     *
     * @param data      Xml node containing data
     * @param component Component used to handle communications
     * @param message   The message to write to the XML node.
     */
    virtual const INT32 HandleRequest(XmlNode* data, BepComponent* component, std::string& message);

};

#endif // Writer_h
