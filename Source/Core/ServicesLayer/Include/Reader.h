//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/Reader.h 2     1/16/04 4:45p Cward $
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
//    $Log: /Core/ServicesLayer/Include/Reader.h $
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
// 6     9/10/02 10:14p Tbochene
//
// 5     7/31/02 11:49a Skypig13
// Added test class and improved SetRate() method
//
// 4     4/25/02 4:42p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//    started
//
//
// 3     4/08/02 1:41p Skypig13
// Added header
//
//*************************************************************************
#ifndef Reader_h
#define Reader_h

#include "XmlNode.h"
#include "IPublish.h"
#include "Debuggable.h"
//#include "IBepCommunication.h"
#include "IInterProcessCommunication.h"

class ReaderTest;
/**
 * Class that will read data at a specified rate
 *
 * @author Tim Bochenek
 * @author Carl Erickson
 * @version 1.0
 * @since Version 1.0
 */
class Reader : public Debuggable
{
public:
	friend class ReaderTest;
    /**
     * Class constructor
     * @since Version 1.0
     */
    Reader(const XmlNode* doc, const UINT32 rate, const bool debug);

    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~Reader(void);

    /**
     * Set data rate at which reader will read data
     *
     * @param rate   Data rate to read data (messages per second)
     * @since Version 1.0
     */
    void SetRate(const UINT32 rate);

	/**
	 * Retrieve the data rate
	 *
	 * @return Data rate
	 * @since Version 1.2
	 */
	const UINT32 GetRate(void);

    /**
     * Handle a read request
     *
     * @param data    Xml node containing data
     * @param comm    Communication object
     * @param message response message
     * @return Status of operation
     * @since Version 2.0
     */
    virtual const INT32 HandleRequest(XmlNode* data, IPublish* comm, std::string& message);

protected:
    /**
     * Delay for the appropriate amount of time for our data rate
     * @since Version 1.0
     */
    void Delay(void);

    /**
     * Worker that will get data for us
     * @since Version 1.0
     */
    IInterProcessCommunication m_ipc;
    /**
     * Rate at which the reader will get data (number of messages pre second)
     * @since Version 1.0
     */
    UINT32 m_rate;
    /**
     * message to use for communication
     * @since Version 1.0
     */
    std::string m_message;
};

#endif // Reader_h


