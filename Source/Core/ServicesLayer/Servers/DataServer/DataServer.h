//*************************************************************************
//
// Description:
//   Data server that will be the interface between the User Interface and Real-Time System
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/DataServer/DataServer.h $
// 
// 5     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 4     4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 3     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 2     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 1     12/21/03 6:42p Cward
//
// 2     10/24/03 1:32p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 2     10/15/03 8:49a Khykin
// Updated the return value of the publish method.
//
// 1     10/07/03 11:33a Khykin
// Initial checkin
//
// 1     7/08/03 12:58p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:08p Khykin
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
// 1     1/27/03 7:04a Khykin
// Initial Checkin
//
// 1     1/18/03 8:02a Khykin
// Initial Checkin From DCTC
//
// 16    6/19/02 2:21p Skypig13
// Added Register() method and configuration ivar for NotificationList
//
// 15    5/30/02 5:03p Skypig13
// Latest stuff that compiles
//
// 14    4/02/02 10:56p Skypig13
// New data server working
//
// 13    2/28/02 1:58p Skypig13
// Added Publish
//
// 12    2/26/02 2:06p Skypig13
// Change to inherit from BepComponent instead of BepServer
//
// 11    2/05/02 12:32a Skypig13
// Updated due changes in BepServer
//
// 10    12/13/01 10:07p Skypig13
// Removed overloaded Unsubscribe and Publish methods and added method to
// hadle subscribers
//
// 9     12/13/01 12:56a Skypig13
// Added Run() method
//
// 8     11/08/01 9:33p Skypig13
// Remove initialize method
//
// 7     10/29/01 1:26p Skypig13
// Removed m_INdbconfig
//
// 6     10/25/01 9:54p Skypig13
// Added StreamData() method and updated comments
//
// 5     10/17/01 9:57a Skypig13
// Changed prototype of Read() to include data reate
//
// 4     10/12/01 11:24a Skypig13
// Latest files for DataServer
//
// 3     10/05/01 4:33p Skypig13
// New Data Server header file
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef DataServer_h
#define DataServer_h

#include "BepServer.h"
#include "INamedDataBroker.h"

class DataServerTest;   // Forward declaration of testing class

/**
 * Class that provides interface between the GUI and data servers providing requested data
 *
 * @author Tim Bochenek
 * @version 2.2
 */
class DataServer : public BepServer
{
    friend class DataServerTest;    // Testing class
public:
    /**
     * Class constructor (default)
     * @since Version 1.0
     */
    DataServer(void);
    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~DataServer(void);
    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * Used to determine the action sent by a
     * CommandButton pressed on the GUI
     *
     * Added by Greg Pattison
     *
     * @param node   Action to be performed
     *
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since 05/05/2004
     */
	virtual const std::string Command(const XmlNode *node);
	/**
	 * Method used to terminate the component.  This performs
	 * any cleanup that the destructor would not and sets
	 * the state of the component to terminate.
	 * @since Version 1.0
	 */
	virtual void Terminate(void);
	/**
	 * Request to read a data tag
	 *
	 * @param node    Data to be read
	 * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
	 * @return Value of tag
	 * @since Version 2.0
	 */
	virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
	/**
     * Request to write a data tag
     *
     * @param node  Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Write(const XmlNode *node);
	/**
	 * Request to be inserted into the list to be notified
	 * of a change in a data tag
	 *
	 * @param node     Data and Name for who is requesting the subscription informaiton.
	 * @return Status of operation
	 *         BEP_SUCCESS_RESPONSE == Subscriber successfully added
	 *         BEP_FAILURE_RESPONSE == Error adding subscriber
	 * @since Version 2.0
	 */
	virtual const std::string Subscribe(const XmlNode *node);
	/**
	 * Method that is called to command the component to perform any
	 * system registration functions necessary.
	 *
	 * @return Status of the registration process.
	 *         Success, Fail
	 * @since Version 2.5
	 */
	virtual const std::string Register(void);
    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 2.1
     */
    virtual const std::string Publish(const XmlNode *node);
    /**
     * Main run method that subclasses can overload if different
     * behavior is desired.
     *
     * @param terminateFlag
     *               Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
	/**
	 * Signal handler fof SIGPIPE
	 *
	 * @param sig    Signal number
	 */
	static void Pipe(INT32 sig);
private:
	/**
	 * Communication configuration
	 */
	const XmlNode* m_commConfig;
	/**
	 * Pointer to NDB comm object.
	 */
	INamedDataBroker *m_ndb;
    /** List of ndb tags that can be commanded. */
    XmlNodeMap m_ndbCommandTags;
    /** List of NDB tags that can be written */
    XmlNodeMap m_ndbWriteTags;
};
#endif  //DataServer_h

