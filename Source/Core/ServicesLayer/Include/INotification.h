//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/INotification.h 4     10/30/07 3:42p Rwiersem $
//
// FILE DESCRIPTION:
//   The is the class defining a classes ability to notify subscribers of chages.
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
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/INotification.h $
// 
// 4     10/30/07 3:42p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004. 
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 11:43a Khykin
// Updated with latest changes from Korea and Flatrock.
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
// 1     6/12/03 4:14p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:39  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:05a Khykin
// Initial Checkin
//
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
//
// 7     11/19/02 9:06p Tbochene
// Added GetNumberOfSubscribers()
//
// 6     7/31/02 11:35a Skypig13
// Added constructor and Destructor and updated prototype to not include a
// debug parameter
//
// 5     4/08/02 1:10p Skypig13
// Added Initialize() method
//
// 4     4/02/02 11:07p Skypig13
// Updates to go with new notification list
//
// 3     2/20/02 2:25p Skypig13
// Added header
//
//*************************************************************************
#ifndef INotification_h
#define INotification_h

#include <string>

#include "IPublish.h"
#include "XmlNode.h"

/**
 * Interface defining a classes ability to notify
 * subscribers of data change
 *
 * @author Tim Bochenek
 * @version 1.1
 * @see NotificationList
 * @see NotifyThreadPool
 * @since Version 1.0
 */
class INotification
{
public:
    /**
     * Class constructor
     * @since Version 1.1
     */
    INotification(void)
    {
    }
    /**
     * Class destructor
     * @since Version 1.1
     */
    virtual ~INotification(void)
    {
    }
    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document) = 0;
    /**
     * Add a process name to the notificaiton list of tag.
     *
     * @param tag    Tag subscriber (name) is interested in
     * @param name   The Process Name to be notified when tag changes
     */
    virtual void AddNotification(const std::string &tag, const std::string name) = 0;
    /**
     * Add a communication object to the notificaiton list of tag.
     *
     * @param tag     Tag subscriber (process communication object is connected to) is interested in
     * @param commObj The communication object used to publish tag and data
     */
    virtual void AddNotification(const std::string &tag, const IPublish* commObj) = 0;
    /**
     * Remove a process name from the notificaiton list of tag.
     *
     * @param tag    Tag no longer of interest to process name
     * @param name   The Process Name
     * @return The status of the removal.
     *         BEP_STATUS_SUCCESS removed
     *         BEP_STATUS_FAILURE not in list
     */
    virtual INT32 RemoveNotification(const std::string &tag, const std::string name) = 0;
    /**
     * Remove a communication object from the notificaiton list of tag.
     *
     * @param tag     Tag no longer of interest to commObj
     * @param commObj Communication object to remove from notification list of tag
     * @return The status of the removal.
     *         BEP_STATUS_SUCCESS removed
     *         BEP_STATUS_FAILURE not in list
     */
    virtual INT32 RemoveNotification(const std::string &tag, const IPublish* commObj) = 0;
    /**
     * method used when subscription data changes and
     * the subscribers need to be notified
     *
     * @param tag    the tag that has changed
     * @param value  The value of the tag that is being sent.
     */
    virtual void Notify(const std::string &tag, const std::string &value) = 0;
    /**
     * method used when subscription data changes and
     * the subscribers need to be notified
     *
     * @param node    the data that has changed
     */
    virtual void Notify(const XmlNode *node) = 0;
    /**
     * Remove all of the tags and subscribers from the
     * list.
     *
     */
    virtual void RemoveAll(void) = 0;
    /**
     * Get the number of subscribers for a tag
     *
     * @param tag    Tag to get information for
     * @return Number of subscribers for tag
     */
    virtual const INT32 GetNumberOfSubscribers(const std::string& tag) = 0;


    /**
     * Send notification to the subscribers for the passed in data.
     *
     * @param node   The data to publish.
     */
    virtual void SendNotification(const XmlNode *node)
    {
    }
    /**
     * Decrement the number of threads outstanding.
     *
     * @return The number of threads outstanding.
     */
    virtual INT32 DecrementThreadCount(void)
    {
        return 0;
    }

};

#endif // INotification_h
