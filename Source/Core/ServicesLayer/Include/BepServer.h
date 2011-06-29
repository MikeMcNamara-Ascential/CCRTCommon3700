//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/BepServer.h 8     10/30/07 3:30p Rwiersem $
//
// FILE DESCRIPTION:
//   Base server for all iterative servers.
//
//===========================================================================
// Copyright (c) 2001-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/BepServer.h $
// 
// 8     10/30/07 3:30p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 7     5/01/07 12:51p Rwiersem
// Changes for the 07050201 core release:
// 
// Moved GetDataTag() and ReadSubscribeData() from protected to public.
// 
// 6     2/22/07 10:59a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Updated and added Javadoc commments.
// 
// 5     1/18/07 11:21a Rwiersem
// Changes for the 07011901 core release:
// 
// - Added m_attr.
// 
// 4     10/26/06 9:11a Rwiersem
// Changes for the 06102701 core release:
// 
// Added Activate() and Deactivate().  These are used to keep the test
// components running when a new drive curve starts.
// 
// 3     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 2     4/29/05 8:00a Gswope
// Removed unused constructor: BepServer(argc,argv)
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
// 2     6/26/03 1:27p Khykin
// Updated with Tims latest changes.
//
// 39    6/04/03 11:15a Tbochene
// Added Heartbeat thread as friend
//
// 38    4/08/03 4:26p Tbochene
// Moved GetDataTag() method here
//
// 37    7/31/02 11:24a Skypig13
// Added WriteSubscribeData()
//
// 36    6/19/02 2:16p Skypig13
// Addd accessor methods for registered status
//
// 35    5/24/02 2:49p Kh
// Updating for use with the launcher.
//
// 34    3/21/02 2:01p Dan.erickson
// Made m_tagDataLock protected so derived objects can use the
// mutices also.
//
// 33    3/20/02 9:07a Dan.erickson
// Added Initialize() method that takes a string as its argument.
//
// 32    2/25/02 8:47p Skypig13
// Updated Initialize()
//
// 31    2/20/02 11:17a Dan.erickson
// Added UnsubscribeTags method.
//
// 30    2/12/02 5:13p Dan.erickson
// Updated prototypes.
// Added prototypes for XmlNode versions of methods.
//
// 29    2/05/02 12:27a Skypig13
// Restructured to not use multiple inheritance, but aggregate Thread Pool
// and IBepCommunication
//
// 27    1/10/02 11:53a Skypig13
// Updated to use vector of Notify threads to handle publications.
//
// 26    1/07/02 5:23p Dan.erickson
// Added SetConfiguredTags, LoadConfiguredTags, and
// KillOffThreadPool method prototypes.
//
// 25    1/04/02 11:10a Dan.erickson
// Added ReadSubscribeData method.
//
// 24    12/18/01 11:32a Kh
// Overloading the BepComponent Register method.
//
// 23    12/14/01 5:44p Kh
// Adding a Resigter method to deal with subscription issues.
//
// 22    12/13/01 1:05a Skypig13
// Moved body of Run() to cpp file
//
// 21    12/12/01 11:18p Skypig13
// Removed printf() from Run() method
//
// 20    12/12/01 11:10p Skypig13
// Update Run() method to provide better reliability during shutdown.
// Run() is base method that waits for signal to shut down.
//
// 19    11/30/01 1:22p Dan.erickson
// Modified to keep a list of tags that have been subscribed
// for as well as a list of tags the server supports.
//
// 18    11/20/01 2:57p Kh
// Adding Run method due to binding issues with the virtual methods.
//
// 17    11/20/01 10:05a Dan.erickson
// Removed duplicate virutal from ClearData method.
//
// 16    11/19/01 4:48p Dan.erickson
// Made ClearData method virtual.
//
// 15    11/15/01 4:18p Skypig13
// Really made ClearData virtual
//
// 14    10/25/01 10:28p Skypig13
// Made GetData() virtual
//
// 13    10/24/01 12:53p Dan.erickson
// Added mutex lock in read and write methods.
//
// 12    10/24/01 9:55a Skypig13
// Changed int to INT32
//
// 11    10/22/01 3:45p Dan.erickson
// Made m_data member protected.
//
// 10    10/17/01 9:55a Skypig13
// Changed prototype of Read() to include data reate
//
// 9     10/12/01 11:27a Dan.erickson
// Changed BepComponent inheritance to protected from public.
//
// 8     10/12/01 10:31a Dan.erickson
// Modified server to allow initialization with a tag list.
//
// 7     10/02/01 11:48p Skypig13
// Changed version number
//
// 6     9/27/01 3:47p Skypig13
// Derived from ThreadPoolBase and made Initialze() methods throw
// Exeptions out
//
// 5     9/14/01 1:57p Skypig13
// Added PUblish method and moved function bodies to .cpp file
//
// 4     9/04/01 9:07p Skypig13
// Changed response of Get/SetData()
//
// 3     8/30/01 4:13p Skypig13
// Update to use Notify thread
//
// 2     8/30/01 11:18a Skypig13
// Fix Notify method call on line 180 to use object name (duh!)
//
// 1     8/30/01 9:18a Skypig13
// New base server
//
//
//*************************************************************************
#ifndef BepServer_h
#define BepServer_h

#include <list>
#include "BepComponent.h"
#include "IBepCommunicationThreadPool.h"

/**
 * BepServer base class provides base iterative server functionality.
 *
 * @author Tim Bochenek
 */
class BepServer : public BepComponent
{
public:
    friend class BepServerTest;
    friend class HeartBeatThread;   // Heartbeat thread needs to get protected data

    /**
     * <b>Description:</b>
     * Data type definition for a list of subscribers.
     */
    typedef list<std::string> SubscriptionList;
    /**
     * <b>Description:</b>
     * Data type definition of an iterator for a list of subscribers.
     */
    typedef SubscriptionList::iterator SubscriptionListIter;
    /**
     * <b>Description:</b>
     * Data type definition of a value in a list of subscribers.
     */
    typedef SubscriptionList::value_type SubscriptionListVal;
    /**
     * <b>Description:</b>
     * BepServer default constructor.
     */
    BepServer(void);
    /**
     * <b>Description:</b>
     * BepServer destructor.
     */
    virtual ~BepServer(void);
    /**
     * <b>Description:</b>
     * Method for initialization passing in the file name.
     *
     * @param fileName Name of the XML configuration file to be used to 
     *                 initialize.
     */
    virtual void Initialize(const std::string &fileName);
    /**
     * <b>Description:</b>
     * Method for initialization passing an XML node.
     *
     * <b>Configuration Items:</b>
     *     <ul>
     *     <li> Setup/Debug </li>
     *     <li> Setup/DataTags </li>
     *     </ul>
     * 
     * @param document Pointer to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * <b>Description:</b>
     * Reloads the configuration file and resets the base components objects.
     */
    virtual void ReloadConfiguration(void);
    /**
     * <b>Description:</b>
     * Method used to abort the current processing based on the needs of the 
     * component.
     */
    virtual void Abort(void);
    /**
     * <b>Description:</b>
     * Method used to set all of the objects in the component to their original 
     * states.  This would include all member variables such as buffers, lists, 
     * and stored data.
     */
    virtual void Reset(void);
    /**
     * <b>Description:</b>
     * Method used to terminate the component.  This performs any cleanup that 
     * the destructor would not and sets the state of the component to 
     * terminate.
     */
    virtual void Terminate(void);
    /**
     * <b>Description:</b>
     * Indicate that the component should re-activate itself. This should
     * happen at the start of a test when the component is needed.
     */
    virtual void Activate(void);
    /**
     * <b>Description:</b>
     * Indicate that the component should de-activate itself. This should
     * happen at the end of a test when the component is no longer needed
     */
    virtual void Deactivate(void);
    /**
     * <b>Description:</b>
     * Request to read a data tag.
     *
     * @param tag  Data tag to be read.
     * @param rate Rate the data should be provided (this is unused)
     * 
     * @return The value of the data tag if it is found.
     *         Otherwise, BEP_UNSUPPORTED_RESPONSE.
     */
    virtual const std::string Read(const std::string &tag, const INT32 rate = 0);
    /**
     * <b>Description:</b>
     * Request to read a data tag.
     *
     * @param node The XML node to be read.
     * @param rate Rate the data should be provided (this is unused)
     * 
     * @return The value of the data tag if it is found.
     *         Otherwise, BEP_UNSUPPORTED_RESPONSE.
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
    /**
     * <b>Description:</b>
     * Request to write a data tag.
     *
     * @param tag   The data tag to be written.
     * @param value The new value to be written.
     * 
     * @return BEP_SUCCESS_RESPONSE if the new value was written successfully.
     *         BEP_ERROR_RESPONSE if the data tag was found, but the tag data
     *         mutex could not be locked.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was found and the tag 
     *         data mutex was locked, but the data could not be written.
     *         BEP_UNSUPPORTED_RESPONSE if the data tag could not be found.
     */
    virtual const std::string Write(const std::string &tag, const std::string &value);
    /**
     * <b>Description:</b>
     * Request to write a data tag.
     *
     * @param node The XML node to be written.
     * 
     * @return BEP_SUCCESS_RESPONSE if the new value was written successfully.
     *         BEP_ERROR_RESPONSE if the data tag was found, but the tag data
     *         mutex could not be locked.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was found and the tag 
     *         data mutex was locked, but the data could not be written.
     *         BEP_UNSUPPORTED_RESPONSE if the data tag could not be found.
     */
    virtual const std::string Write(const XmlNode *node);
    /**
     * <b>Description:</b>
     * Request to perform an action.  This method simply calls 
     * BepComponent::Command().
     *
     * @param tag   The action to be performed.
     * @param value The value for the action to be performed.
     * 
     * @return BEP_SUCCESS_RESPONSE if the action is supported.
     *         BEP_UNSUPPORTED_RESPONSE if the action is unsupported.
     */
    virtual const std::string Command(const std::string &tag, const std::string &value);
    /**
     * <b>Description:</b>
     * Request to perform an action.  This method simply calls 
     * BepComponent::Command().
     *
     * @param node The action to be performed and its value.
     * 
     * @return BEP_SUCCESS_RESPONSE if the action is supported.
     *         BEP_UNSUPPORTED_RESPONSE if the action is unsupported.
     */
    virtual const std::string Command(const XmlNode *node);
    /**
     * <b>Description:</b>
     * Request to be inserted into the list to be notified of a change in a 
     * data tag.  If server has been initialized with tags, the requested tag 
     * will be compared to the list of tags before a subscribe will be allowed.
     *
     * @param tag     The subscription tag name.
     * @param Process The process name to notify when the data changes.
     * 
     * @return The current value of the data tag if it is found.
     *         BEP_UNSUPPORTED_RESPONSE if the tag is not found.
     *         An empty string if the server is not initialized with tags.
     */
    const std::string Subscribe(const std::string &tag, const std::string &Process);
    /**
     * <b>Description:</b>
     * Request to be inserted into the list to be notified of a change in a 
     * data tag.  If server has been initialized with tags, the requested tag 
     * will be compared to the list of tags before a subscribe will be allowed.
     *
     * @param node Node containing the subscription information.
     * 
     * @return The current value of the data tag if it is found.
     *         BEP_UNSUPPORTED_RESPONSE if the tag is not found.
     *         An empty string if the server is not initialized with tags.
     */
    const std::string Subscribe(const XmlNode *node);
    /**
     * <b>Description:</b>
     * Method that is called to command the component to perform any system 
     * registration functions necessary.
     *
     * @return BEP_SUCCESS_RESPONSE if the registration was successful
     *         BEP_FAILURE_RESPONSE if the registration was not successful
     */
    virtual const std::string Register(void);
    /**
     * <b>Description:</b>
     * Handle the specified pulse.  This method simply calls 
     * BepComponent::HandlePulse().
     *
     * @param code  The pulse code to process.
     * @param value The pulse value to process.
     * 
     * @return BEP_STATUS_SUCCESS if the pulse was successfully handled.
     *         BEP_STATUS_FAILURE if the pulse was not handled.
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);
    /**
     * <b>Description:</b>
     * Method to determine if the server was initialized with a list of tags.
     *
     * @return True if the server was initialized with a tag list.
     *         Otherwise, false.
     */
    bool UseConfiguredTags(void);
    /**
     * <b>Description:</b>
     * Main run method.  This method runs until the status is BEP_TERMINATE.
     *
     * @param terminateFlag Flag used to terminate the system.  (this is unused)
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
    /**
     * <b>Description:</b>
     * Get a system data tag from the data tag list.
     *
     * @param dataTag The data tag to look up.
     * 
     * @return The name for the data tag that was requested.  If the data tag
     *         is not found, the passed in name is returned.
     */
    const std::string GetDataTag(const std::string dataTag);
    /**
     * <b>Description:</b>
     * Request to read the value of a subscribed tag.
     *
     * @param tag  The data tag to read.
     * @param rate The rate at which to read the data. (this is unused)
     * 
     * @return The value of the tag if the data tag was succesfully read.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const std::string ReadSubscribeData(const std::string &tag, const INT32 rate=0);

protected:
    /**
     * <b>Description:</b>
     * Request to write the value of a subscribed.
     *
     * @param tag   The data tag to write.
     * @param value The new data tag value.
     * 
     * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const std::string WriteSubscribeData(const std::string &tag, const std::string &value);
    /**
     * <b>Description:</b>
     * Clears the subscribe data.
     */
    virtual void ClearData(void);
    /**
     * <b>Description:</b>
     * Returns the requested data item from the server.
     *
     * @param tag The data tag to read.
     * 
     * @return The value of the tag if the data tag was succesfully read.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const std::string GetData(const std::string &tag);
    /**
     * <b>Description:</b>
     * Writes the requested data item to the server.
     *
     * @param tag   The data tag to write.
     * @param value The new value of the data tag.
     * 
     * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const std::string SetData(const std::string &tag, const std::string &value);
    /**
     * <b>Description:</b>
     * Writes the requested data item to the server.
     *
     * @param node The data tag and value to write.
     * 
     * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const std::string SetData(const XmlNode *node);
    /**
     * <b>Description:</b>
     * Set the member variable that tells if the server was configured using a 
     * tag list.
     *
     * @param useConfiguredTags The flag that tells if the server was 
     *                          configured with a tag list.
     */
    void SetConfiguredTags(const bool useConfiguredTags);
    /**
     * <b>Description:</b>
     * Write the requested data item from the server.
     *
     * @param tag   The data tag to write.
     * @param value The new data tag value.
     * 
     * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const std::string Publish(const std::string &tag, const std::string &value);
    /**
     * <b>Description:</b>
     * Write the requested data item from the server.
     *
     * @param node  The data tag and value to write.
     * 
     * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const std::string Publish(const XmlNode *node);
    /**
     * <b>Description:</b>
     * Method which determines if the specified tag is available for use.
     * 
     * @param tag The data tag to look for in the list.
     * 
     * @return True if the tag is in the list.  Otherwise, false.
     */
    virtual bool FindTag(const std::string &tag);
    /**
     * <b>Description:</b>
     * Sets up the server's publish and subscribe data.
     *
     * <b>Configuration Items:</b>
     *     <ul>
     *     <li> Setup/TagList/PublishTags </li>
     *     <li> Setup/TagList/SubscribeTags </li>
     *     </ul>
     * 
     * @param setup The configuration information used to setup the server.
     */
    virtual void LoadConfiguredTags(const XmlNode *setup);
    /**
     * <b>Description:</b>
     * This method will unsubscribe for any subsribed tags.
     * 
     * @param clearList Flag indicating if the subscribe tag list should be 
     *                  cleared.
     */
    virtual void UnsubscribeTags(bool clearList=true);
    /**
     * <b>Description:</b>
     * Subscribe for all our subscribe tags and update their initial values.
     * 
     * @return BEP_STATUS_SUCCESS if the subsription was successful.
     *         Otherwise, BEP_STATUS_ERROR.
     */
    virtual INT32 SubscribeTags(void);
    /**
     * <b>Description:</b>
     * Checks to see if the server has already registered.
     *
     * @return True if the server has already been registered.  
     *         Otherwise, false.
     */
    const bool IsRegistered(void);
    /**
     * <b>Description:</b>
     * Sets registered status.
     *
     * @param registered Flag indicating if the server has been registered.
     */
    void SetRegistered(const bool registered);

protected:
    /** List containing data that was sent by clients. */
    XmlNodeMap m_data;

    /** List of tags the server supports. */
    XmlNodeMap m_publishTags;

    /** List of the servers subscribed tags. */
    SubscriptionList m_subscribeTags;

    /** Pointer to a communication object. */
    IBepCommunicationThreadPool *m_comm;

    /** Mutex lock variable for node map data. */
    mutable pthread_mutex_t m_tagDataLock;

    /** Mutex attributes for the lock variable for the node map data. */
    pthread_mutexattr_t m_attr;

    /** Data tags that are used to alias a name to actual data tag. */
    XmlNodeMap m_dataTags;

private:
    /** Flag to tell if the server was initialized with tags. */
    bool m_tags;

    /** Condition variable set to indicate the server should terminate. */
    pthread_cond_t m_endVar;

    /** Flag to indicate that the system was registered. */
    bool m_registered;
};
#endif  //BepServer_h

