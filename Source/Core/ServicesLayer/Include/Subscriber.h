//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/Subscriber.h 2     1/16/04 4:45p Cward $
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
//    $Log: /Core/ServicesLayer/Include/Subscriber.h $
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
// 1     6/12/03 4:24p Khykin
// Initial checkin.
//
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:47  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:10a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 5     7/31/02 11:52a Skypig13
// Added test class
//
// 4     4/25/02 4:42p Skypig13
// Updated with new changes
//
// 3     4/08/02 1:44p Skypig13
// Added header
//
//*************************************************************************
#ifndef Subscriber_h
#define Subscriber_h

#include "IPublish.h"
#include "Debuggable.h"

class SubscriberTest;
/**
 * Class to manage subsribers
 *
 * @author Tim Bochenek
 * @author Carl Erickson
 * @version 1.0
 * @since Version 1.0
 */
class Subscriber : public Debuggable
{
public:
	friend class SubscriberTest;
    /**
     * Class constructor (default)
     * @since Version 1.0
     */
    Subscriber(void);
    /**
     * Class constructor (default)
     * @since Version 1.0
     */
    Subscriber(const bool debug);

    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~Subscriber(void);

    /**
     * Return initial response to client, then register this client with
     * the SubscriptionManager and return, as further handling of subscriptions
     * is up to the SubManager, not us.
     *
     * @param data    Xml node containing data
     * @param comm    Communication object
     * @param message response message
     * @return Status of operation
     * @since Version 2.0
     */
    virtual const INT32 HandleRequest(XmlNode* data, IPublish* comm, std::string& message);
};
#endif // Subscriber_h
