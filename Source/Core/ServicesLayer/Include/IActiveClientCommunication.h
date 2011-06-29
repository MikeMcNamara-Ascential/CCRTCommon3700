//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/IActiveClientCommunication.h 2     1/16/04 4:45p Cward $
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
//    $Log: /Core/ServicesLayer/Include/IActiveClientCommunication.h $
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
// 1     6/12/03 4:09p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:37  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:04a Khykin
// Initial Checkin
//
// 1     1/18/03 8:02a Khykin
// Initial Checkin From DCTC
//
// 12    7/31/02 11:31a Skypig13
// Restored condition on print statement
//
// 11    6/19/02 2:23p Skypig13
// Added Register handling
//
// 10    4/25/02 4:40p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//    started
//
//
// 9     4/08/02 1:28p Skypig13
// Added new workers (Commander and Writer)
//
// 8     4/02/02 10:56p Skypig13
// Tested and working
//
// 7     3/07/02 6:51p Skypig13
// Update to new BepServer structure
//
// 6     2/28/02 3:01p Skypig13
// Fixed missing variable in InitializeServer()
//
// 5     2/28/02 2:01p Skypig13
// Update to make pulblications work
//
// 4     2/19/02 4:42p Skypig13
// Updates from testing
//
// 3     2/05/02 2:59p Skypig13
// Optimizing and removing unconditional set debug
//
// 2     2/05/02 1:42p Skypig13
// Removed warnings
//
// 1     2/05/02 12:33a Skypig13
// Classes needed to communicate with an active client (streaming reader
// or subscriber on a TCP/IP socket)
//
//*************************************************************************

#ifndef ActiveClientCommunication_h
#define ActiveClientCommunication_h

#include <typeinfo>
#include <string>
//#include <sys/syspage.h>
//#include <sys/neutrino.h>
//#include <sys/resource.h>

#include "XmlNode.h"
//#include "BepTimer.h"
//#include "XmlParser.h"
#include "IBepCommunication.h"

//#include "Reader.h"
//#include "Writer.h"
//#include "Commander.h"
//#include "Registrar.h"
//#include "Subscriber.h"

#define PROFILING_ON (0)

/**
 * Communication class customized for a Streaming/Subscribing Server
 *
 * @author Tim Bochenek
 * @version 1.0
 * @since Version 1.0
 */
class IActiveClientCommunication : public IBepCommunication
{
public:
	/**
	 * Class Constructor (default)
	 * @since Version 1.0
	 */
	IActiveClientCommunication(void);

	/**
	 * Class Destructor
	 * @since Version 1.0
	 */
	virtual ~IActiveClientCommunication(void);

	/**
	 * Remove from subscription list for notification of a specified tag
	 *
	 * @param node   Node to be Published.
	 * @param response Data returned by operation
	 * @param IsFinal If this tag is the las tag to be requested
	 * @return Status of publish operation
	 * @since Version 2.4
	 */
	virtual INT32 Publish(const XmlNode *node, std::string &response, const bool IsFinal = false);
    /**
     * Set the flag indicating that the client closed
     * the connection
     *
     * @param close  Whether to close the socket
     * @since Version 1.2
     */
    inline void SetClientClosed(void);

protected:
	/**
	 * Send response to request
	 *
	 * @param doc       Data to send
	 * @param component Component to use when handling request
	 * @return If operation succeded
	 * @since Version 1.0
	 */
	virtual const bool SendResponse(XmlNode* doc, BepComponent* component);
    bool m_clientClosed;

};

#endif // ActiveClientCommunication_h


