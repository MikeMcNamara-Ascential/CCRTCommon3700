//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/NotificationList.h 5     10/30/07 3:46p Rwiersem $
//
// FILE DESCRIPTION:
//   The is the class for managing subscriber lists.  It provides all of the
//   functionality for maintaing and notifying subscribers in a list.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/NotificationList.h $
// 
// 5     10/30/07 3:46p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     7/20/05 8:50p Cward
// Updated Remove All and SendNotification 
//
// 3     9/01/04 9:33p Cward
// Allow specification of a reply timeout
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 4     10/24/03 11:54a Khykin
// Updated with latest mutex changes.
//
// 3     10/15/03 8:42a Khykin
// Updated with latest Korea changes.
//
// 2     10/10/03 9:30a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 3     8/12/03 2:50p Khykin
// Updated with new directory update changes.
//
// 2     8/06/03 4:11p Khykin
// Updated with the latest changes from Flatrock.
//
// 3     6/26/03 1:45p Khykin
// Added error checking when locking and unlocking mutexes.
//
// 2     6/20/03 8:50a Khykin
// Updated for Ford changes.
//
// 1     6/12/03 4:18p Khykin
// Initial checkin.
//
// 1     4/22/03 9:40a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.2  2003/02/25 14:34:54  tbochene
//    Potential bug fix in RemoveNotification() if last item is removed
//
//    Revision 1.1.1.1  2003/02/15 00:05:44  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:08a Khykin
// Initial Checkin
//
// 1     1/18/03 8:05a Khykin
// Initial Checkin From DCTC
//
// 37    11/19/02 9:09p Tbochene
// Removed SubscriptionManager as friend
//
// 36    11/19/02 9:06p Tbochene
// Added GetNumberOfSubscribers()
//
// 35    10/07/02 6:43a Khykin
// Updating for changes to the AbstractThread class.
//
// 34    9/12/02 5:47p Tbochene
// Updated to derive from ILogger
//
// 33    9/10/02 6:30p Tbochene
// Added new friend and conditioned prints
//
// 32    7/31/02 11:46a Skypig13
// Changes to to interface change, inherit from Debuggable
//
// 31    6/19/02 2:47p Skypig13
// Updated to include more prints and changed mutex protection to be more
// specific about how much to protect (less area protected == better)
//
// 30    5/30/02 5:03p Skypig13
// Latest stuff that compiles
//
// 29    4/25/02 4:41p Skypig13
// Updated with new changes
//    Revision 1.8  2002/04/23 21:34:44  erickson
//    improved robustness of constructor and AddNotification; added proper RemoveNotification() code for NotifyThisList() method; removed typecast  on IIPCCommObj for delete from SendNotification()
//
//    Revision 1.7  2002/04/18 20:08:48  erickson
//    fixed bug in RemoveFromList due to removing from a copy of the actual list
//
//    Revision 1.6  2002/04/17 21:58:18  erickson
//    added warning about locks
//
//    Revision 1.5  2002/04/17 14:21:53  erickson
//    modified Notify(tag, value, debug) to use Notify(node, debug)
//
//    Revision 1.4  2002/04/16 14:29:27  erickson
//    fixed comments on operators
//
//    Revision 1.3  2002/04/15 21:40:17  erickson
//    added operators for ==, <, > for NotificationListElement; added GetName to IPublish
//
//    Revision 1.2  2002/04/11 19:19:34  erickson
//    Updated ThreadPoolBase to properly use muticies and updated tests for INamedDataBroker.  Data server runs and SIGSEGV when the Subscription manager receives a subscription request and attemtps to create the first threads in the  Notification Thread Pool (first ContextAllocate()).
//
//    Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//    started
//
//
// 26    4/08/02 1:11p Skypig13
// Updated to use objects and strings as storage items
//
// 25    4/02/02 11:10p Skypig13
// Updated to be more genaric and to hold communication objects
//
// 23    3/12/02 12:01p Dan.erickson
// Modified XmlNode version of Notify() to send the string
// representation of the node as the value.
// Modified SendNotification() to parse the value parameter into
// a XmlNode representation.
//
// 22    2/20/02 2:26p Skypig13
// Changed semaphore to mutex and released when there is an error
// notifying befer RemoveNotification()
//
// 21    2/19/02 11:33p Bmeinke
// Changed the typedef vector< Process> List to typedef set< Process>
// NotifyProcList to avoid multiple subscriptions of a single process for
// the same data tag
// Numerous proitotype changes due to the aboce typedef change
// Optomizations when copying entire maps and lists: don't copy..just get
// a reference
//
// 20    2/05/02 12:47a Skypig13
// Updated to remove compiler warnings
//
// 19    1/16/02 4:28p Dan.erickson
// Modified Notify method to tak a pointer to an IBepCommunication object.
// Modified SendNotification method to use the IBepCommunication object if
// it was passed in.
//
// 18    11/29/01 5:54p Dan.erickson
// Modified SendNotification method to handle publishing both
// a tag and a XmlNode.
//
// 17    11/29/01 2:49p Dan.erickson
// Modified to allow notification of a node.
//
// 16    11/08/01 9:01a Kh
// When the data access mutex was changed to a semaphore changes were not
// made to allow the notification list to work.  A mutex, if you have it
// and try to relock it succeeds.  A semaphore would not let you perform
// the second lock and you would appear to hang.  Now it is better.
//
// 15    9/18/01 1:53p Skypig13
// Put debug configuration in IBepCommunication object that Notifies
// subscribers
//
// 14    9/18/01 7:30a Skypig13
// Update to use semaphores instead of mutexes to allow signals to
// interrupt
//
// 13    9/14/01 2:28p Skypig13
// Add debug parameter to all methods
//
// 11    8/28/01 3:51p Skypig13
// Updating to use BEP_STATUS_* and proper initializer for comm object in
// Notify() and NotifyList()
//
// 10    8/27/01 6:55p Kh
// Updating for 6.1 NTO, it doesent support typeid.
//
// 9     7/26/01 1:40p Kh
// Missed one printout.  Changing.
//
// 7     7/23/01 10:06p Kh
// Updating the notification list to use the IBepCommunication obj
//
// 6     7/16/01 1:05p Brian.meinke
// Replaced reference to Remove() method with RemoveNotification() in
// Notify() method
//
// 5     7/16/01 12:52p Brian.meinke
// Changed Add() method to AddNotification()
// Changed Remove() method to RemoveNotification()
//
// 4     6/01/00 8:03a Kh
// Adding return type to the remove method.
//
// 3     5/30/00 1:22p Kh
// Making thread safe.
//
// 2     5/24/00 3:16p Kh
// Adding methods for multiple lists.
//
// 1     5/23/00 11:30a Kh
// Notification List Template.  Class that is used to manage a list of
// clients for data.
//
//*************************************************************************
#ifndef NotificationList_h
#define NotificationList_h

#include <set>
#include <string>
#include <pthread.h>
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif
#include "BepDefs.h"
#include "ILogger.h"
#include "XmlNode.h"
#include "IPublish.h"
#include "BepException.h"
#include "INotification.h"
#include "IBepCommunication.h"

/**
 * Element type of the notification list
 *
 * Note:
 * This class is used similarly to a union to generically
 * create and safely access the elements of the list because
 * the list can contain dissimilar types in different implementations.
 *
 * @author Tim Bochenek
 * @author Carl Erickson
 * @version 1.1
 * @see NotificationList
 * @see NotificationListThreadPool
 * @since Version 1.0
 */
class NotificationListElement
{
    friend class NotificationListTest;
public:
    /**
     * Class constructor
     *
     * @param commObj Communication object to be referenced by element
     * @since Version 1.0
     */
    NotificationListElement(const IPublish* commObj) : m_commObj(commObj)
    {
        if(!m_commObj) throw BepException("NotificationListElement() was passed a NULL object");
    }
    /**
     * Name of process requesting to be notified by element
     *
     * @param name   Name of process
     * @since Version 1.0
     */
    NotificationListElement(const std::string& name) : m_commObj(NULL), m_name(name)
    {
        if(name.empty()) throw BepException("NotificationListElement() was passed an empty string");
    }
    /**
     * Get the pointer to object contained in element (if there is one)
     *
     * @return Pointer to communication object referenced by element
     *         (NULL if there is not one)
     * @since Version 1.0
     */
    const IPublish* GetPointer(void) const
    {
        return(m_commObj);
    }
    /**
     * Return name of process to notify stored by element
     *
     * @return Name of process to be notified
     *         (Empty string if not one)
     * @since Version 1.0
     */
    const std::string& GetName(void) const
    {
        return(m_name);
    }
    /**
     * Comparison operator (less than)
     *
     * @param elem   Element to compare (against this)
     * @return If this is less than parameter
     * @since Version 1.0
     */
    bool operator< (const NotificationListElement& elem) const
    {
        bool isLess = false;
        if(GetPointer() && elem.GetPointer() && (GetPointer() < elem.GetPointer()))
        {   // If both pointers are not null compare
            isLess = true;
        }
        else if(!GetName().empty() && !elem.GetName().empty() && (GetName() < elem.GetName()))
        {   // If both strings are not empty compare
            isLess = true;
        }
        else if(GetPointer() && !elem.GetPointer())
        {   // If I have an object and elem does not, I am less
            isLess = true;
        }
        else if(!GetName().empty() && elem.GetName().empty())
        {   // If I have an string and elem does not, I am less
            isLess = true;
        }
        return(isLess);
    }
    /**
     * Comparison operator (greater than)
     *
     * @param elem   Element to compare (against this)
     * @return If this is less than parameter
     * @since Version 1.0
     */
    bool operator> (const NotificationListElement& elem) const
    {
        bool isGreater = false;
        if(GetPointer() && elem.GetPointer() && (elem.GetPointer() < GetPointer()))
        {   // If both pointers are not null compare
            isGreater = true;
        }
        else if(!GetName().empty() && !elem.GetName().empty() && (elem.GetName() < GetName()))
        {   // If both strings are not empty compare
            isGreater = true;
        }
        else if(elem.GetPointer() && !GetPointer())
        {   // If I have an object and elem does not, I am less
            isGreater = true;
        }
        else if(!elem.GetName().empty() && GetName().empty())
        {   // If I have an string and elem does not, I am less
            isGreater = true;
        }
        return(isGreater);
    }
    /**
     * Equality operator
     *
     * @param str    String to compare (against this)
     * @return If this is less than parameter
     * @since Version 1.0
     */
    bool operator== (const std::string& str) const
    {
        return(GetName() == str);
    }
    /**
     * Equality operator with IPublish arg.
     *
     * @param ptr    Object to compare (against this)
     * @return If this is less than parameter
     * @since Version 1.0
     */
    bool operator== (const IPublish* ptr) const
    {
        return(GetPointer() == ptr);
    }
    /**
     * Equality operator with NLE arg.
     *
     * @param elem   Element to compare (against this)
     * @return If this is less than parameter
     * @since Version 1.0
     */
    bool operator== (const NotificationListElement& elem) const
    {
        return(!operator <(elem) && !operator>(elem));
    }
    /**
     * Closes the channel object (if exists) is connected to
     * @since Version 1.1
     */
    void Close(void) const
    {
        if(GetPointer()) GetPointer()->Close();
    }
protected:
    /**
     * Communication object to be used for publications
     * @since Version 1.0
     */
    const IPublish* m_commObj;
    /**
     * Name of communication cahnnel to create
     * @since Version 1.0
     */
    std::string m_name;
};

/**
* NotificationList base class.  This class provides all
* system functionality needed to manage lists of processes
* based on a common tag.
*
* @author Kevin Hykin
* @version 1.2
* @see INotification
* @see NotificationThreadPool
* @since Version 1.0
*/
template <class CommType>
class NotificationList : public INotification, public ILogger
{
    friend class NotificationListTest;      // You're my freind!
    friend class SubscriptionManagerTest;   // You're my freind!
public:
    /**
     * Class Constructor (default)
     * @since Version 1.0
     */
    NotificationList(void): INotification(), ILogger(), m_replyTimeout(0)
    {
        // Initialize the mutex
        pthread_mutexattr_t m_mutexAttr;

        memset( &m_mutexAttr, 0, sizeof( m_mutexAttr));
        memset( &m_mutex, 0, sizeof( m_mutex));

        pthread_mutexattr_init( &m_mutexAttr);
        pthread_mutexattr_settype( &m_mutexAttr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutexattr_setrecursive(&m_mutexAttr, PTHREAD_RECURSIVE_ENABLE);
        pthread_mutex_init(&m_mutex, &m_mutexAttr);
    }
    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~NotificationList(void)
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_mutexattr_destroy( &m_mutexAttr);
    }
    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document)
    {
        try
        {
            if(document != NULL)
            {
                ILogger::Initialize(document);
                Log(LOG_FN_ENTRY, "NotificationList::Initialize(%s)\n", document->ToString().c_str());
                // Get the response timeout to use
                try
                {
                    m_replyTimeout = BposReadInt(document->getChild("Setup/NotificationList/Setup/ResponseTimeout")->getValue().c_str());
                }
                catch (XmlException &excpt)
                {
                    Log(LOG_ERRORS, "Setup/NotificationList/Setup/ResponseTimeout not provided, using %d - %s",
                        DEFAULT_TIMEOUT, excpt.what());
                    m_replyTimeout = DEFAULT_TIMEOUT;
                }
            }
        }
        catch(BepException& Err)
        {
            Log(LOG_ERRORS, "NotificationList::Initialize(%s) failed because %s\n", document->ToString().c_str(), Err.what());
        }
        catch(...)
        {
            Log(LOG_ERRORS, "NotificationList::Initialize(%s) failed for an unknown reason\n", document->ToString().c_str());
        }
    }
    /**
     * Add a process name to the notificaiton list of tag.
     *
     * @param tag    Tag subscriber (name) is interested in
     * @param name   The Process Name to be notified when tag changes
     */
    virtual void AddNotification(const std::string &tag, const std::string name)
    {
        // ensure tag is valid
        if(tag.empty())
        {
            throw BepException("AddNotification with empty tag");
        }
        // ensure the name is valid
        else if(name.empty())
        {
            throw BepException("AddNotification with empty name");
        }
        NotificationListElement elem(name);
        AddToList(tag, elem);
    }
    /**
     * Add a communication object to the notificaiton list of tag.
     *
     * @param tag     Tag subscriber (process communication object is connected to) is interested in
     * @param commObj The communication object used to publish tag and data
     */
    virtual void AddNotification(const std::string &tag, const IPublish* commObj)
    {
        Log(LOG_FN_ENTRY, "NotificationList::AddNotification(%s, %p)\n", tag.c_str(), commObj);
        if(tag.empty())
        {
            throw BepException("AddNotification with empty tag");
        }
        else if (commObj == NULL)
        {
            throw BepException("AddNotification with NULL commObj");
        }
        NotificationListElement elem(commObj);
        AddToList(tag, elem);
        Log(LOG_FN_ENTRY, "NotificationList::AddNotification(%s, %p) done\n", tag.c_str(), commObj);
    }
    /**
     * Remove a process name from the notificaiton list of tag.
     *
     * @param tag    Tag no longer of interest to process name
     * @param name   The Process Name
     * @return The status of the removal.
     *         BEP_STATUS_SUCCESS removed
     *         BEP_STATUS_FAILURE not in list
     */
    virtual INT32 RemoveNotification(const std::string &tag, const std::string name)
    {
        INT32 status = BEP_STATUS_FAILURE;
        if(!tag.empty() && !name.empty())
        {
            status = RemoveFromList(tag, NotificationListElement(name));
        }
        return(status);
    }
    /**
     * Remove a communication object from the notificaiton list of tag.
     *
     * @param tag     Tag no longer of interest to commObj
     * @param commObj Communication object to remove from notification list of tag
     * @return The status of the removal.
     *         BEP_STATUS_SUCCESS removed
     *         BEP_STATUS_FAILURE not in list
     */
    virtual INT32 RemoveNotification(const std::string &tag, const IPublish* commObj)
    {  // verify the tag is in the subscriber list
        INT32 status = BEP_STATUS_FAILURE;
        if((!tag.empty()) && (commObj != NULL))
        {
            status = RemoveFromList(tag, NotificationListElement(commObj));
        }
        return(status);
    }
    /**
     * method used when subscription data changes and
     * the subscribers need to be notified
     *
     * @param tag    the tag that has changed
     * @param value  The value of the tag that is being sent.
     * @since Version 1.1
     */
    virtual void Notify(const std::string& tag, const std::string& value)
    {   // load a local copy of the list for notification
        if(!tag.empty() && !value.empty())
        {
            XmlNode node(tag, value);
            Notify(&node);
        }
        else
            Log(LOG_ERRORS, "Notify invoked with an empty tag or value: %s, %s\n", tag.c_str(), value.c_str());
    }
    /**
     * Method used when subscription data changes and
     * the subscribers need to be notified.
     *
     * @param node    the data that has changed
     * @since Version 1.1
     */
    virtual void Notify(const XmlNode *node)
    {   // load a local copy of the list for notification
        bool locked = false;
        INT32 mutexStatus;

        // if the node that was passed was not null
        if(node != NULL)
        {
            try
            {
                const XmlString nodeStr = node->ToString();
                Log(LOG_FN_ENTRY, "NotificationList::Notify(%s)\n", nodeStr.c_str());
                mutexStatus = pthread_mutex_lock(&m_mutex);
                if(mutexStatus == EOK)
                {
                    locked = true;
                    Log(LOG_FN_ENTRY, "NotificationList::Notify locked(%s)\n", nodeStr.c_str());

                    NotificationTableIter nIter;                // create an iterator
                    nIter = m_list.find(node->getName());       // locate the tag (if it exists)
                    NotifyList list;

                    if(nIter != m_list.end())
                    {   // if tag exists get the list
                        list = nIter->second;
                        if(!list.empty())
                        {
                            SendNotification(list, node);
                        }
                    }

                    if((mutexStatus = pthread_mutex_unlock(&m_mutex)) != EOK)
                        Log(LOG_ERRORS, "Unable to unlock the mutex in Notify(): %s, error: %d, %s\n",
                            nodeStr.c_str(), mutexStatus, strerror(errno));

                    Log(LOG_FN_ENTRY, "NotificationList::Notify unlocked(%s)\n", node->ToString().c_str());
                }
                else
                    Log(LOG_ERRORS, "Unable to lock the mutex in Notify(): %s, error: %d, %s\n",
                        nodeStr.c_str(), mutexStatus, strerror(errno));
            }
            catch(BepException &e)
            {
                Log(LOG_ERRORS, "NotificationList::Notify Exception VERY BAD: %s\n", e.what());
                if(locked)
                {
                    if((mutexStatus = pthread_mutex_unlock(&m_mutex)) != EOK)
                        Log(LOG_ERRORS, "Unable to unlock the mutex in Notify(): %s, error: %d, %s\n",
                            node->ToString().c_str(), mutexStatus, strerror(errno));
                }
                throw;
            }
            catch(...)
            {
                Log(LOG_ERRORS, "NotificationList::Notify Exception VERY BAD\n");
                if(locked)
                {
                    if((mutexStatus = pthread_mutex_unlock(&m_mutex)) != EOK)
                        Log(LOG_ERRORS, "Unable to unlock the mutex in Notify(): %s, error: %d, %s\n",
                            node->ToString().c_str(), mutexStatus, strerror(errno));
                }
                throw;
            }
        }
    }
    /**
     * Remove all of the tags and subscribers from the list.
     * @since Version 1.0
     */
   void RemoveAll(void)
   {
      std::set<IPublish *>   pointers;

      INT32 mutexStatus = pthread_mutex_lock(&m_mutex);
      if(mutexStatus == EOK)
      {
         Log(LOG_FN_ENTRY, "NotificationList::RemoveAll(%d)\n", m_list.size());
         for(NotificationTableIter iter = m_list.begin(); iter != m_list.end(); ++iter)
         {
            Log(LOG_FN_ENTRY, "Tag %s has %d subscribers\n", iter->first.c_str(), iter->second.size());
            for(NotifyListIter lIter = iter->second.begin(); lIter != iter->second.end(); ++lIter)
            {
               lIter->Close();    // Close communication channel
               // Keep track of the comm objects we need to delete
               if(lIter->GetPointer())
               {
                  pointers.insert(const_cast<IPublish *>(lIter->GetPointer()));
               }
            }
            iter->second.clear();
         }
         // Delete the pointers
         if(false == pointers.empty())
         {
            for(std::set<IPublish*>::iterator ptrItr = pointers.begin(); ptrItr!=pointers.end(); ptrItr++)
            {
               Log(LOG_FN_ENTRY, "deleting %p\n", *ptrItr);
//               Log(LOG_FN_ENTRY, "deleting %p\n", lIter->GetPointer());
               delete *ptrItr;
            }
         }
         m_list.clear();
        if((mutexStatus = pthread_mutex_unlock(&m_mutex)) != EOK)
            Log(LOG_ERRORS, "Unable to unlock the mutex in RemoveAll(): error: %d, %s\n",
                  mutexStatus, strerror(errno));
      }
      else
         Log(LOG_ERRORS, "Unable to lock the mutex in RemoveAll(): error: %d, %s\n",
               mutexStatus, strerror(errno));
   }

   /* this is the prevous version
    void RemoveAll(void)
    {
        INT32 mutexStatus = pthread_mutex_lock(&m_mutex);
        if(mutexStatus == EOK)
        {
            Log(LOG_FN_ENTRY, "NotificationList::RemoveAll(%d)\n", m_list.size());
            for(NotificationTableIter iter = m_list.begin(); iter != m_list.end(); ++iter)
            {
                Log(LOG_FN_ENTRY, "Tag %s has %d subscribers\n", iter->first.c_str(), iter->second.size());
                for(NotifyListIter lIter = iter->second.begin(); lIter != iter->second.end(); ++lIter)
                {
                    lIter->Close(); // Close communication channel
                    Log(LOG_FN_ENTRY, "deleting %p\n", lIter->GetPointer());
                    if(lIter->GetPointer()) delete lIter->GetPointer();    // Delete object
                }
                iter->second.clear();
            }
            m_list.clear();
            if((mutexStatus = pthread_mutex_unlock(&m_mutex)) != EOK)
                Log(LOG_ERRORS, "Unable to unlock the mutex in RemoveAll(): error: %d, %s\n",
                    mutexStatus, strerror(errno));
        }
        else
            Log(LOG_ERRORS, "Unable to lock the mutex in RemoveAll(): error: %d, %s\n",
                mutexStatus, strerror(errno));
    }
   */

    /**
     * Get the number of subscribers for a tag
     *
     * @param tag    Tag to get information for
     * @return Number of subscribers for tag
     */
    virtual const INT32 GetNumberOfSubscribers(const std::string& tag)
    {
        INT32 Size = -1;
        NotificationTableIter nIter = m_list.find(tag);     // locate the tag (if it exists)

        if(nIter != m_list.end())
        {
            Size = nIter->second.size();
        }
        return(Size);
    }
protected:
    /**
     * Notification vector list.
     */
    typedef std::set<NotificationListElement> NotifyList;
    /**
     * Process I.D. list iterator.
     */
    typedef NotifyList::iterator NotifyListIter;
    /**
     * Process I.D. list iterator.
     */
    typedef NotifyList::const_iterator NotifyListCIter;
    /**
     * Process I.D. list value type operator.
     */
    typedef NotifyList::value_type NotifyListVal;
    /**
     * Notification tag Map. List of all requestors
     * for notification based on a tag.
     */
    typedef map< std::string, NotifyList, less<std::string> > NotificationTable;
    /**
     * Subscriber map iterator.
     */
    typedef NotificationTable::iterator NotificationTableIter;
    /**
     * Subscriber map value type operator.
     */
    typedef NotificationTable::value_type NotificationTableVal;
    /**
     * Publish the changed data to all subscribers.
     *
     * @param nlist   List of Comm to notify.
     * @param node   Node with data to be published
     */
    virtual void SendNotification(const NotifyList& nlist, const XmlNode* node)
    {
        INT32 stat;
        std::string response;   // create a name
        IPublish *IPCCommObj = NULL, *commObj = NULL;

        Log(LOG_FN_ENTRY, "NotificationList::SendNotification(%s)\n", node->ToString().c_str());
        for(NotifyListCIter lIter = nlist.begin(); lIter != nlist.end(); lIter++)
        {   // walk through the list sending info
            try
            {   // init to talk to the next server
                commObj = const_cast<IPublish* >(lIter->GetPointer());
                Log(LOG_DETAILED_DATA, "commObj = %p, IPCCommObj = %p\n", commObj, IPCCommObj);
                if((commObj == NULL) && (IPCCommObj == NULL))
                {
                    Log(LOG_DETAILED_DATA, "Creating commObj\n");
                    IPCCommObj = new CommType();
                    Log(LOG_DETAILED_DATA, "IPCCommObj = %p\n", IPCCommObj);
                }
                // if a local comm object created for comm, initialize it
                if(IPCCommObj != NULL)
                {
                    Log(LOG_DETAILED_DATA, "Initializing commObj %s -- Name: %s, Type: %s\n",
                        lIter->GetName().c_str(), lIter->GetName().c_str(), "Client");
                    IPCCommObj->Initialize(lIter->GetName(), std::string("Client"), IsDebugOn(), DEFAULT_BUFFER_SIZE, m_replyTimeout);
                    Log(LOG_DEV_DATA, "Initialized commObj %s, id = %d, reply timeout = %d\n", lIter->GetName().c_str(),
                        ((CommType *)IPCCommObj)->GetId(), m_replyTimeout);
                    commObj = IPCCommObj;
                }
                else Log(LOG_DETAILED_DATA, "We have a comm object in the list\n");

                Log( LOG_DETAILED_DATA, "Calling commObj->Publish()\n");
                stat = commObj->Publish(node, response, true);  // Publish the data
                Log( LOG_DETAILED_DATA, "commObj->Publish() done, response=%s\n", response.c_str());
                if(stat == BEP_STATUS_SUCCESS)
                    Log(LOG_DEV_DATA, "Notified: %s message: %s\n", commObj->GetName().c_str(), response.c_str());
                else    // else log the error
                    Log(LOG_ERRORS, "THIS COULD BE BAD! Removing %s, stat: %d\n", node->ToString().c_str(), stat);
            }
            catch(BepException &BepErr)
            {
                Log(LOG_ERRORS, "Warning: Removing for tag %s: because %s\n", node->getName().c_str(), BepErr.what());
                stat = BEP_STATUS_ERROR;
            }
            catch (...)
            {
                Log(LOG_ERRORS, "SendNotification: Unknown exception\n");
                stat = BEP_STATUS_ERROR;
            }
            // if the publication was not successful, remove the subscriber
            if(stat != BEP_STATUS_SUCCESS)
            {
                if(IPCCommObj != NULL)  // if the NL is not managing communication objects remove the name
                    RemoveNotification(node->getName(), lIter->GetName());
                else                    // else remove the comm obj
                    RemoveNotification(node->getName(), commObj);
            }
        }
        // if the communication obj was locally created, delete it
        if(IPCCommObj != NULL)
        {
            // was
            // IBepCommunication* iBep = dynamic_cast<IBepCommunication*>(IPCCommObj);
//            CommType* iBep = dynamic_cast<CommType*>(IPCCommObj);
			CommType* iBep = (CommType*)IPCCommObj;
            if(iBep != NULL)    // if the communication object exists, delete it
            {
                Log(LOG_DETAILED_DATA, "Delete commObj %s, id = %d\n", iBep->GetName().c_str(), iBep->GetId());
                delete IPCCommObj;
                IPCCommObj = NULL;
            }
        }
        Log(LOG_FN_ENTRY, "NotificationList::SendNotification(%s) done\n", node->ToString().c_str());
    }
    /**
     * Send the tag and its value to all of the subscribers in the list.
     *
     * @param nlist  List of the subscribers to send the message to.
     * @param tag    The tag that was requested by all of the subscribers.
     * @param value  The value of the tag that was requested.
     */
    virtual void NotifyThisList(NotifyList &nlist, const std::string& tag, const std::string& value)
    {
        if(nlist.size() > 0)
        { // if there is actually items in the list
            std::string name, response, client("Client"); // create a name
            IPublish* commObj = NULL, *IPCCommObj = NULL;

            for(NotifyListIter lIter = nlist.begin(); lIter != nlist.end(); ++lIter)
            {    // walk through the list sending info
                try
                {   // init to talk to the next server
                    commObj = const_cast<IPublish*>(lIter->GetPointer());
                    Log(LOG_FN_ENTRY, "commObj = %p\n", commObj);
                    if((commObj == NULL) && (IPCCommObj == NULL))
                    {
                        Log(LOG_FN_ENTRY, "Creating commObj\n");
                        IPCCommObj = new CommType();
                    }
                    if(IPCCommObj != NULL)
                    {
                        Log(LOG_FN_ENTRY, "Initializing commObj\n");
                        IPCCommObj->Initialize(lIter->GetName(), std::string("Client"), IsDebugOn());
                        commObj = IPCCommObj;
                    }
                    else Log(LOG_FN_ENTRY, "We have a comm object in the list\n");

                    try
                    {
                        if(commObj->Publish(tag, value, response, true) != BEP_STATUS_SUCCESS)
                        {   // log the removal and remove it
                            Log(LOG_ERRORS, "Removed Comm: %s, Error: %s\n", commObj->GetName().c_str(), strerror(errno));
                            nlist.erase(lIter);     // remove the iterator if it does not exist
                            lIter--;                // point to the previous record in the list
                        }
                        else Log(LOG_FN_ENTRY, "Notified: %s\n", commObj->GetName().c_str());
                    }
                    catch(BepException &BepErr)
                    {
                        Log(LOG_ERRORS, "Error: Failure to Publish: %s\n", BepErr.what());
                        RemoveNotification(tag, commObj->GetName());
                    }
                    Log(LOG_FN_ENTRY, "List Complete\n");
                }
                catch(BepException &BepErr)
                {
                    Log(LOG_ERRORS, "Error: Notifying '%s' because %s\n", name.c_str(), BepErr.what());
                    if(IPCCommObj != NULL)
                    {
                        RemoveNotification(tag, lIter->GetName());
                    }
                    else
                    {
                        RemoveNotification(tag, commObj);
                    }
                }
            }
            if(IPCCommObj != NULL)
            {
                delete IPCCommObj;
                IPCCommObj = NULL;
            }
        }
    }
    /**
     * Build a list of subscribers that have subscribed for the tag that was passes.
     *
     * @param tag    The tag to lookup and retrieve the subscribers for.
     * @param nlist  The list to add the subscribers to.
     */
    virtual void CreateList(const std::string& tag, NotifyList& nlist)
    {
        NotificationTableIter nIter;    // create an iterator
        INT32 mutexStatus = pthread_mutex_lock(&m_mutex);
        if(mutexStatus == EOK)
        {
            nIter = m_list.find(tag);       // locate the tag (if it exists)
            if(nIter != m_list.end())
            {       // if tag exists
                NotifyList&         subList = nIter->second; // get the list
                NotifyListIter      sLIter = subList.begin(); // point to the beginning
                if(nlist.empty())   nlist = subList;       // if the list is empty copy current list to passed in list
                else
                {   // else make sure no duplicates
                    while(sLIter != subList.end())
                    {   // check if the entry appears in the list, if not, add it
                        if(nlist.find( *sLIter) != nlist.end())
                        {
                            Log(LOG_FN_ENTRY, "Adding: %s\n", tag.c_str());
                            nlist.insert( *sLIter);
                        }
                        sLIter++;   // walk through the list of subscribers
                    }
                }
            }
            if((mutexStatus = pthread_mutex_unlock(&m_mutex)) != EOK)
                Log(LOG_ERRORS, "Unable to unlock the mutex in CreateList(): %s, error: %d, %s\n",
                    tag.c_str(), mutexStatus, strerror(errno));
        }
        else
            Log(LOG_ERRORS, "Unable to lock the mutex in CreateList(): %s, error: %d, %s\n",
                tag.c_str(), mutexStatus, strerror(errno));
    }
    /**
     * Adds the notification list element to the notification list
     *
     * @param tag  The tag of the element to add to the list.
     * @param elem Element to add to list
     */
    void AddToList(const std::string& tag, NotificationListElement& elem)
    {
        Log(LOG_FN_ENTRY, "NotificationList::AddToList(%s, %p)\n", tag.c_str(), elem.GetPointer());
        NotificationTableIter   nIter;  // iterator for the notification list
        INT32 mutexStatus = pthread_mutex_lock(&m_mutex);
        if(mutexStatus == EOK)
        {
            nIter = m_list.find(tag);       // find the tag in the table
            Log(LOG_FN_ENTRY, "Adding elem for Tag: %s\n", tag.c_str());
            if(nIter == m_list.end())
            {               // if the tag was not in the table insert it
                // create a new vector and insert it into the map
                NotifyList nlist;
                nlist.insert(elem);
                m_list.insert(NotificationTableVal(tag, nlist));
                Log(LOG_FN_ENTRY, "Added new list and process for tag: %s\n", tag.c_str());
            }
            else
            {                                        // else add the chid to the list
                nIter->second.insert(elem);
                Log(LOG_FN_ENTRY, "Added process for tag: %s\n", tag.c_str());
            }

            if((mutexStatus = pthread_mutex_unlock(&m_mutex)) != EOK)
                Log(LOG_ERRORS, "Unable to unlock the mutex in AddToList(): %s, error: %d, %s\n",
                    tag.c_str(), mutexStatus, strerror(errno));
        }
        else
            Log(LOG_ERRORS, "Unable to lock the mutex in AddToList(): %s, error: %d, %s\n",
                tag.c_str(), mutexStatus, strerror(errno));

        Log(LOG_FN_ENTRY, "NotificationList::AddToList(%s, %p) done\n", tag.c_str(), elem.GetPointer());
    }
    /**
     * Removes the notification list element from the notification list
     *
     * @param tag   The tag of the element to remove from the list.
     * @param elem   Element to remove from list
         */
    const INT32 RemoveFromList(const std::string& tag, const NotificationListElement& elem)
    {
        INT32 status = BEP_STATUS_ERROR;
        INT32 mutexStatus;
        bool locked = false;


        try
        {
            NotificationTableIter   nIter;                      // iterator for the subscriber list
            mutexStatus = pthread_mutex_lock(&m_mutex);
            if(mutexStatus == EOK)
            {
                locked = true;
                nIter = m_list.find(tag);       // find the tag in the table
                Log(LOG_FN_ENTRY, "Removing Tag: %s\n", tag.c_str());
                if(nIter != m_list.end())
                {               // if the tag was in the list
                    NotifyList& nlist = nIter->second;              // get a reference to the list
                    NotifyListIter lIter;
                    // look through the list until the end or the element if found
                    for(lIter = nlist.begin(); lIter != nlist.end() ;)
                    {
                        if(!elem.GetName().empty()) Log(LOG_FN_ENTRY, "list loop %s\n", lIter->GetName().c_str());

                        // if the list contains an object pointer
                        if(elem.GetPointer() != NULL)
                        {
                            // if a match is found, remove the element
                            const IPublish* ptr = lIter->GetPointer();
                            if(ptr == elem.GetPointer())
                            {   // erase the element and assign iterator to the next element
                                lIter = nlist.erase(lIter);
                                Log(LOG_FN_ENTRY, "NotificationList Removed: %p for tag %s\n", ptr, tag.c_str());
                            }
                            // else increment the iterator
                            else
                            {
                                lIter++;
                            }
                        }
                        // else it must be an object
                        else
                        {   // if this is the object to remove
                            if(elem.GetName() == lIter->GetName())
                            {
                                // remove the element and assign the iterator to the next element
                                lIter = nlist.erase(lIter);
                                Log(LOG_FN_ENTRY, "NotificationList Removed: %s for tag %s\n", elem.GetName().c_str(), tag.c_str());
                            }
                            // else increment the iterator
                            else
                            {
                                lIter++;
                            }
                        }
                    }

                    // if the last subscriber was removed from the list, remove the tag
                    if(nlist.empty())
                    {
                        m_list.erase(nIter);
                    }
                    else
                    {
                        for(lIter = nlist.begin(); lIter != nlist.end(); lIter++)
                            Log(LOG_FN_ENTRY, "In The List: %s\n", lIter->GetName().c_str());
                    }
                }
                if((mutexStatus = pthread_mutex_unlock(&m_mutex)) != EOK)
                    Log(LOG_ERRORS, "Unable to unlock the mutex in RemoveFromList(): %s, error: %d, %s\n",
                        tag.c_str(), mutexStatus, strerror(errno));
            }
            else
                Log(LOG_ERRORS, "Unable to lock the mutex in RemoveFromList(): %s, error: %d, %s\n",
                    tag.c_str(), mutexStatus, strerror(errno));

        }
        catch(BepException &Err)
        {
            // if mutex locked, make sure it is unlocked
            if(locked)
            {
                if((mutexStatus = pthread_mutex_unlock(&m_mutex)) != EOK)
                    Log(LOG_ERRORS, "Unable to unlock the mutex in RemoveFromList(): %s, error: %d, %s\n",
                        tag.c_str(), mutexStatus, strerror(errno));
            }
            Log(LOG_FN_ENTRY, "BepException: %s\n", Err.what());
        }
        catch(...)
        {
            if(locked)
            {
                if((mutexStatus = pthread_mutex_unlock(&m_mutex)) != EOK)
                    Log(LOG_ERRORS, "Unable to unlock the mutex in RemoveFromList(): %s, error: %d, %s\n",
                        tag.c_str(), mutexStatus, strerror(errno));
            }
            Log(LOG_FN_ENTRY, "Unknown exception\n");
        }

        return(status);
    }
    /**
     * List of all requestors for notification.
     * @since Version 1.0
     */
    NotificationTable m_list;
    /**
     * Mutex used to lock the thread list for exclusive access
     * @since Version 1.2
     */
    pthread_mutex_t     m_mutex;

    /**
     * Attributes of the mutex
     */
    pthread_mutexattr_t m_mutexAttr;

private:
    /**
     * Amount of time to wait for a notified server to respond.
     */
    INT32  m_replyTimeout;
};

#endif  // NotificationList_h
