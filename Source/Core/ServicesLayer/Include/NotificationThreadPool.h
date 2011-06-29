//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/NotificationThreadPool.h 2     10/30/07 3:47p Rwiersem $
//
// FILE DESCRIPTION:
//   Thread pool for notifying subscribers of data that has changed
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/NotificationThreadPool.h $
// 
// 2     10/30/07 3:47p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 1     12/21/03 6:26p Cward
// 
// 3     10/24/03 11:55a Khykin
// Updated with latest mutex changes.
// 
// 3     10/15/03 8:43a Khykin
// Updated with latest Korea changes.
// 
// 2     10/10/03 9:30a Khykin
// Updated with latest Flatrock changes.
// 
// 1     10/07/03 11:37a Khykin
// Initial checkin
// 
// 2     8/06/03 4:12p Khykin
// Update with latest changes from Flatrock.
// 
// 2     6/26/03 1:49p Khykin
// Updated the notify method.
// 
// 1     6/12/03 4:18p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
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
// 12    11/19/02 9:09p Tbochene
// Removed SubscriptionManager as friend
// 
// 11    11/19/02 9:06p Tbochene
// Added GetNumberOfSubscribers()
// 
// 10    10/07/02 6:44a Khykin
// Adding the locking of data during notification.
// 
// 9     9/12/02 5:47p Tbochene
// Updated to derive from LogIfc
// 
// 8     9/10/02 6:34p Tbochene
// Added friend class for testing
// 
// 7     7/31/02 11:46a Skypig13
// Changes to to interface change, inherit from Debuggable
// 
// 6     5/30/02 5:04p Skypig13
// Latest stuff that compiles
//
//*************************************************************************

#ifndef NotificationThreadPool_h
#define NotificationThreadPool_h

#include <queue>

#include "IThread.h"
#include "INotification.h"
#include "AbstractThread.h"
#include "NotificationList.h"
#include "TemplateThreadPool.h"

/**
 * Thread class that will be used to notify subscribers of data change
 *
 * @author Tim Bochenek
 * @version 1.0
 * @since Version 1.0
 */
class NotifyThread : public AbstractThread
{
public:
    /**
     * Class constructor (default)
     * @since Version 1.0
     */
    NotifyThread(void): AbstractThread()
    {
    }
    /**
     * Initializing constructor
     * 
     * @param config Setup for object
     * @param pool   Thread pool that is managing the thread
     * @since Version 1.0
     */
    NotifyThread(const XmlNode* config, IThreadPool* pool = NULL) : AbstractThread(config, pool)
    {
        Log(LOG_FN_ENTRY, "NotifyThread::NotifyThread(%p, %p)\n", config, pool);
        if((m_thread = dynamic_cast<IThread*>(pool)) == NULL) Log(LOG_ERRORS, "Thread pool object %p is invalid\n", pool);
        Log(LOG_FN_ENTRY, "NotifyThread::NotifyThread(%p, %p) done\n", config, pool);
    }
    /**
     * Class destructor
     * @since Version 1.0
     */
    virtual ~NotifyThread(void)
    {
        Log(LOG_FN_ENTRY, "NotifyThread::~NotifyThread()\n");
    }
    /**
     * Funciton to be called when thread is created and blocked
     * @since Version 1.0
     */
    virtual void Block(void)
    {
        Log(LOG_FN_ENTRY, "NotifyThread::Block()\n");
        if(m_thread) m_thread->Block(NULL);
    }
    /**
     * Funciton to be called when Block() returns and should do some work
     * @since Version 1.0
     */
    virtual void Handle(void)
    {
        Log(LOG_FN_ENTRY, "NotifyThread::Handle()\n");
        if(m_thread) m_thread->Handle(NULL);
        Log(LOG_FN_ENTRY, "NotifyThread::Handle() done\n");
    }
private:
    /**
     * Thread pool that needs to be serviced
     * @since Version 1.0
     */
    IThread* m_thread;
};

/**
 * Class used by a server to receive messages on a communication
 * channel opened by an IBepCommunication object.
 *
 * @author Tim Bochenek
 * @version 1.1
 * @since Version 1.0
 */
template<class CommType>
class NotificationThreadPool : public TemplateThreadPool<NotifyThread>, public IThread, public INotification
{
public:
    friend class NotifyThread;              // You're my freind!
    friend class BepComponentTest;          // You're my freind!
    friend class SubscriptionManagerTest;   // You're my freind!
    /**
     * Class Constructor (default)
     * @since Version 1.0
     */
    NotificationThreadPool(void) : TemplateThreadPool<NotifyThread>(), IThread(), INotification(), m_cond(NULL)
    {
    }
    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~NotificationThreadPool(void)
    {
        while(!m_notifyData.empty())
        {
            delete m_notifyData.front();
            m_notifyData.pop();
        }
        if(m_cond != NULL)  delete m_cond;
        m_cond = NULL;
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
            Log(LOG_FN_ENTRY, "NotificationThreadPool::Initialize(%s)\n", config->ToString().c_str());

            // store the initial priority of the thread pool
            m_priority = BposGetPriority(0);

            pthread_mutexattr_t attr;
            
            memset( &attr, 0, sizeof( attr));
            memset( &m_mutex, 0, sizeof( m_mutex));
            
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK);
            pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_DISABLE);
            pthread_mutex_init(&m_mutex, &attr);    // Initialize mutex
            
            pthread_condattr_t cattr;
            
            memset( &cattr, 0, sizeof( cattr));
            
            m_cond = new pthread_cond_t;
            memset( m_cond, 0, sizeof( pthread_cond_t));
            
            pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_PRIVATE);
            pthread_cond_init(m_cond, &cattr);      // Initialize condition variable
            
            TemplateThreadPool<NotifyThread>::Initialize(config);   // Initialize base calss
            m_nList.Initialize(config);
            Log(LOG_FN_ENTRY, "NotificationThreadPool::Initialize(%s) done\n", config->ToString().c_str());
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
     * @param tag   The tag that has changed.
     * @param value The value of the tag that is being sent.
     */
    virtual void Notify(const std::string &tag, const std::string &value)
    {
        if((!tag.empty()) && (!value.empty()))
        {
            XmlNode *localNode = new XmlNode(tag, value);
            Log(LOG_FN_ENTRY, "NotificationThreadPool::Notify(%s, %s)\n", tag.c_str(), value.c_str());
            // lock during data access
            if((errno = pthread_mutex_lock(&m_mutex)) == EOK)
            {
                m_notifyData.push(localNode);       // push the local copy onto the list
                pthread_cond_signal(m_cond);       // Signal data ready to send
                pthread_mutex_unlock(&m_mutex);     // unlock when complete
        
            }
            else
            {
                Log(LOG_ERRORS, "Could not lock mutex in NotificationThreadPool::Notify( %s, %s) - %s\n", 
                    tag.c_str(), value.c_str(), strerror(errno));
            }
        }
    }
    /**
     * Method used when subscription data changes and the subscribers need to 
     * be notified.
     *
     * @param node The data that has changed.
     */
    virtual void Notify(const XmlNode *node)
    {
        if(node != NULL)
        {
            try
            {
                string nodeStr = node->ToString();
                Log(LOG_FN_ENTRY, "NotificationThreadPool::Notify(%s)\n", nodeStr.c_str());
                if ((errno = pthread_mutex_lock(&m_mutex)) == EOK)      // lock during data access
                {
                    m_notifyData.push(node->Copy());    // push the local copy onto the list
                    pthread_cond_signal(m_cond);        // Signal data ready to send
                    pthread_mutex_unlock(&m_mutex);     // unlock when complete
                }
                else
                {
                    Log(LOG_ERRORS, "Could not lock mutex in NotificationThreadPool::Notify(%s) - %s\n", 
                        nodeStr.c_str(), strerror(errno));
                }
            }
            catch(...)
            {
                Log(LOG_ERRORS, "MotificationThreadPool::Notify() mutex exception - %s\n", strerror(errno));
            }
        }
        Log(LOG_FN_ENTRY, "NotificationThreadPool::Notify(%s) Complete\n", node->getName().c_str());
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
     * Method called upon creation threads.
     * @param param
     * @since Version 1.0
     */
    virtual void Allocate(ThreadPoolInfo *param)
    {
    }
    /**
     * Dervied class method for performing the blocking operation.
     * @since Version 1.0
     */
    virtual void Block(ThreadPoolInfo* param)
    {
        try
        {
            // ensure that the thread returns to its initial priority upon being recycled
            struct sched_param  schedParam;
            SchedGet( 0, pthread_self(), &schedParam);
            schedParam.sched_priority = m_priority;
            SchedSet( 0, pthread_self(), SCHED_NOCHANGE, &schedParam);
            
            Log(LOG_FN_ENTRY, "NotificationThreadPool::Block(%d)\n", pthread_self());
            pthread_mutex_lock(&m_mutex);   // Will be unlocked by the Handle() method
            while(m_notifyData.empty())
            {
                pthread_cond_wait(m_cond, &m_mutex);
            }
            Log(LOG_FN_ENTRY, "NotificationThreadPool::Block(%d) done\n", pthread_self());
        }
        catch(BepException &Err)
        {
            pthread_mutex_unlock(&m_mutex);
            Log(LOG_ERRORS, "NotificationThreadPool::Block(%d) error: %s\n", pthread_self(), Err.what());
        }
        catch(...)
        {
            pthread_mutex_unlock(&m_mutex);
            Log(LOG_ERRORS, "NotificationThreadPool::Block(%d) error\n", pthread_self());
        }
    }
    /**
     * Derived class method for performing the work.
     *
     * @return Must return zero.
     * @since Version 1.0
     */
    virtual INT32 Handle(ThreadPoolInfo* param)
    {
        try
        {
            Log(LOG_FN_ENTRY, "NotificationThreadPool::Handle(%d)\n", pthread_self());
            XmlNode* theNode = NULL;
            if(m_notifyData.size())
            {
                theNode = m_notifyData.front();
                m_nList.Notify(theNode);
                m_notifyData.pop();
                if(theNode) delete theNode;
                theNode = NULL;
                pthread_mutex_unlock(&m_mutex);
            }
            else Log(LOG_FN_ENTRY, "No data in m_notifyData\n");
            Log(LOG_FN_ENTRY, "NotificationThreadPool::Handle(%d) done\n", pthread_self());
        }
        catch(BepException &Err)
        {
            pthread_mutex_unlock(&m_mutex);
            Log(LOG_ERRORS, "NotificationThreadPool::Handle(%d) error: %s\n", pthread_self(), Err.what());
        }
        catch(...)
        {
            pthread_mutex_unlock(&m_mutex);
            Log(LOG_ERRORS, "NotificationThreadPool::Handle(%d) error\n", pthread_self());
        }
        return(0);
    }
    /**
     * Method called upon deletion of threads.
     * @param param
     * @since Version 1.0
     */
    virtual void Free(ThreadPoolInfo *param)
    {
    }
    protected:
    /**
     * Subscriber notification list.
     * @since Version 1.0
     */
    NotificationList<CommType> m_nList;
    /**
     * Type for data structure to queue notification requests
     * @since Version 1.1
     */
    typedef queue<XmlNode*> NotificationQueue;

    /**
     * Queue of notification events
     * @since Version 1.0
     */
    NotificationQueue m_notifyData;
    /**
     * Mutex that controls access to the notify tag and value
     * @since Version 1.0
     */
    pthread_mutex_t m_mutex;
    /**
     * Condition variable that will be used to initiate publications
     * @since Version 1.0
     */
    pthread_cond_t *m_cond;
    
    /**
     * The priority of the thread pool when started.
     * @since Version 1.1
     */
    int m_priority;
};

#endif // NotificationThreadPool_h
