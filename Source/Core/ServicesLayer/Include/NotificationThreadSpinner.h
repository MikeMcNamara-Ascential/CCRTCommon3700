//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/NotificationThreadSpinner.h 5     10/30/07 3:47p Rwiersem $
//
// FILE DESCRIPTION:
//   Thread Spinner for notifying subscribers of data that has changed
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/NotificationThreadSpinner.h $
// 
// 5     10/30/07 3:47p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     5/08/06 7:19p Cward
// Changed printf statements to Log statements or throw exceptions
//
// 3     2/15/06 7:42p Cward
// Fixed minor bug in incrementing thread counts.
// In Notify(), minor change to sequence, to increment the thread count when
//  the NotificationData object is created.
// (it is decremented when that object is deleted.)
//
// 2     7/20/05 8:51p Cward
// Added friend clause to support Testing
//
// 1     12/21/03 6:27p Cward
//
// 1     11/04/03 10:36a Khykin
// Initial checkin.
//
//*************************************************************************

#ifndef NotificationThreadSpinner_h
#define NotificationThreadSpinner_h

#include "INotification.h"
#include "NotificationList.h"
#include "ILogger.h"
#include "BepSync.h"


// forward declaration of the notification thread
void* NotificationThread( void* arg);

/**
 * Notification data class used by the notification thread
 * to publish data.
 *
 * @author Kevin Hykin
 * @version Version 1.0
 */
class NotificationData
{
public:
    /**
     * Notification data constructor.
     */
    NotificationData() : m_parent(NULL), m_notificationData(NULL)
    {
    }

    /**
     * Notification data destructor.
     */
    virtual ~NotificationData()
    {
        if(m_notificationData != NULL)  delete m_notificationData;
        m_notificationData = NULL;
        // remove the thread from the parents outstanding threads
        if(m_parent != NULL)    m_parent->DecrementThreadCount();
    }

    /**
     * Get the data that the notification will be done with.
     *
     * @return The notification data.
     */
    const XmlNode *GetNotificationData(void)
    {
        // make a check to ensure that an empty notification is not sent
        if(m_notificationData == NULL)
            throw BepException("Error Notification Data Not Set!\n");

        return(m_notificationData);
    }

    /**
     * Set the data that the notifications will be done for.
     *
     * @param data   The data that the notifications will be done for.
     */
    void SetNotificationData(const XmlNode *data)
    {
        if(data != NULL)
            m_notificationData = data->Copy();
        else
            throw BepException("SetNotificationData() attempting to set null data\n");
    }

    /**
     * Set the reference to the process for which the notifications
     * will be done.
     *
     * @param parent The reference to the process for which the notifications will
     *               be done.
     */
    void SetParent(INotification *parent)
    {
        if(parent != NULL)
            m_parent = parent;
        else
            throw BepException("SetParent() attempting to set null parent\n");
    }

    /**
     * Notify the subscribers for the data.
     */
    void SendNotification(void)
    {
        try
        {
            if(m_parent != NULL)
                m_parent->SendNotification(GetNotificationData());
            else
                throw BepException("SendNotification(%s), no parent provided\n", GetNotificationData()->ToString().c_str());
        }
        catch(BepException &e)
        {
            printf("Error, SendNotification() failed: %s\n", e.what());
        }
    }

protected:
    /**
     * Reference to the parent class used for notification.
     */
    INotification *m_parent;
    /**
     * The data that has changed.
     */
    XmlNode *m_notificationData;
};

/**
 * Class used by a server to spin up a thread to notify subscribers
 * of data that changed.
 *
 *
 * @author Kevin Hykin
 * @version 1.0
 * @since Version 1.0
 */
template <class CommType>
class NotificationThreadSpinner : public INotification, public ILogger
{
    friend class NotificationThreadSpinnerTest;     // You're my freind!
public:
    /**
     * Class Constructor (default)
     * @since Version 1.0
     */
    NotificationThreadSpinner(void) : INotification(), ILogger(), m_threadCount(0)
    {
    }
    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~NotificationThreadSpinner(void)
    {
        // because the threads are directly tied to comm objects, we can wait for them
        // to complete because we know that they will time out.
        int timeRemaining = 50;
        while(GetThreadCount() && timeRemaining--)
        {
            Log(LOG_DEV_DATA, "Waiting for all notifications to complete: %d\n", GetThreadCount());
            BposSleep(100);
        }
    }
    /**
     * Method used to set up class
     *
     * @param config Configuration node used to set up class
     * @since Version 1.0
     */
    void Initialize(const XmlNode* config)
    {
        if(config != NULL)
        {
            // initialize the logger
            ILogger::Initialize(config);

            Log(LOG_FN_ENTRY, "NotificationThreadSpinner::Initialize(%s)\n", config->ToString().c_str());

            // initialize the notification list
            m_nList.Initialize(config);

            Log(LOG_FN_ENTRY, "NotificationThreadSpinner::Initialize(%s) done\n", config->ToString().c_str());
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
        // if the tag and name are valid, add it to the list
        if(!tag.empty() && !name.empty())
        {
            m_nList.AddNotification(tag, name);
        }
    }
    /**
     * Add a communication object to the notificaiton list of tag.
     *
     * @param tag     Tag subscriber (process communication object is connected to) is interested in
     * @param commObj The communication object used to publish tag and data
     */
    virtual void AddNotification(const std::string &tag, const IPublish* commObj)
    {
        if((!tag.empty()) && (commObj != NULL))
        {
            m_nList.AddNotification(tag, commObj);
        }
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

        if((!tag.empty()) && (!name.empty()))
        {
            status = m_nList.RemoveNotification(tag, name);
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
    {
        INT32 status = BEP_STATUS_FAILURE;

        if((!tag.empty()) && (commObj != NULL))
        {
            status = m_nList.RemoveNotification(tag, commObj);
        }

        return(status);
    }
    /**
     * Method used when subscription data changes and the subscribers need to 
     * be notified.
     *
     * @param tag    the tag that has changed
     * @param value  The value of the tag that is being sent.
     */
    virtual void Notify(const std::string &tag, const std::string &value)
    {
        if((!tag.empty()) && (!value.empty()))
        {
            // create an XML node for notification
            XmlNode localNode(tag, value);
            Notify(&localNode);
        }
    }
    /**
     * method used when subscription data changes and
     * the subscribers need to be notified
     *
     * @param node    the data that has changed
     */
    virtual void Notify(const XmlNode *node)
    {
        if(node != NULL)
        {
            Log(LOG_FN_ENTRY, "NotificationThreadSpinner::Notify(%s)\n", node->ToString().c_str());

            // create the notification data
            NotificationData *nData = new NotificationData();
            // increment the count, it will be decremented when nData is deleted
            IncrementThreadCount();
            nData->SetNotificationData(node);   // get a copy of the data to notify with
            nData->SetParent(this);                     // set the parent

            // at some future time we might want to limit the number based on the count and just allow X notifications at a time....
            // spin a detached thread for notification
            pthread_attr_t attr;
            pthread_attr_init( &attr );
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED );
            if(NULL != pthread_create( NULL, &attr, &NotificationThread, nData ))
            {
                Log(LOG_ERRORS, "Unable to launch thread for notification: %s\n", nData->GetNotificationData()->ToString().c_str());
                // delete this object, it is no longer needed
                delete nData;
            }
        }
    }
    /**
     * Remove all of the tags and subscribers from the
     * list.
     *
     */
    virtual void RemoveAll(void)
    {
        m_nList.RemoveAll();
    }

    /**
     * Method put in to satisfy the INotification interface.
     */
    void Start(void)
    {
    }

    /**
     * Decrement the number of threads outstanding.
     *
     * @return The number of threads outstanding (-1 on error).
     */
    virtual INT32 DecrementThreadCount(void)
    {
        INT32 count = -1;

        // lock the mutex for access
        if(m_threadCountMutex.Acquire() == EOK)
        {   // if there are outstanding threads
            if(m_threadCount)
            {   // decrement the count
                count = m_threadCount--;
            }
            // release the mutex
            m_threadCountMutex.Release();
        }
        else
        {
            Log(LOG_ERRORS, "Unable to DecrementThreadCount()\n");
        }

        return(count);
    }


protected:
    /**
     * Get the number of subscribers for a tag
     *
     * @param tag    Tag to get information for
     * @return Number of subscribers for tag
     */
    virtual const INT32 GetNumberOfSubscribers(const std::string& tag)
    {
        INT32 number = 0;

        if(!tag.empty())
        {
            number = m_nList.GetNumberOfSubscribers(tag);
        }

        return(number);
    }

    /**
     * Increment the number of threads outstanding.
     *
     * @return The number of threads outstanding (-1 on error).
     */
    virtual INT32 IncrementThreadCount(void)
    {
        INT32 count = -1;

        if(m_threadCountMutex.Acquire() == EOK)
        {
            count = m_threadCount++;
            // release the mutex
            m_threadCountMutex.Release();
        }
        else
        {
            Log(LOG_ERRORS, "Unable to IncrementThreadCount()\n");
        }

        return(count);
    }

    /**
     * Return the number of threads outstanding.
     *
     * @return The number of threads outstanding.
     */
    virtual INT32 GetThreadCount(void)
    {
        return(m_threadCount);
    }

    /**
     * Send notification to the subscribers for the passed in data.
     *
     * @param node   The data to publish.
     */
    virtual void SendNotification(const XmlNode *node)
    {
        Log(LOG_FN_ENTRY, "NotificationThreadSpinner::SendNotification(%s)\n", node->ToString().c_str());

        try
        {
            // notify the subscribers
            m_nList.Notify(node);
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "NotificationThreadSpinner::SendNotification Exception: %s\n", e.what());
        }

        Log(LOG_FN_ENTRY, "NotificationThreadSpinner::SendNotification(%s) done\n", node->ToString().c_str());
    }

protected:

    /**
     * Subscriber notification list.
     * @since Version 1.0
     */
    NotificationList<CommType> m_nList;

    /**
     * Mutex used to access the thread count ivar.
     */
    BepMutex    m_threadCountMutex;

    /**
     * The number of threads that are outstanding.
     */
    INT32       m_threadCount;
};

/**
 * Thread used to notify the subscribers of a tag that has been
 * updated.
 *
 * @param arg    The NotificationData class that is used to perform the work.
 *
 * @return The status of the operation.
 */
void* NotificationThread( void*  arg )
{
    NotificationData *data = (NotificationData *) arg;
    if(data != NULL)
    {
        // notify the list of the changed data
        data->SendNotification();
        // delete the data
        delete data;
    }
    return(0);
}



#endif // NotificationThreadSpinner_h
