///*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/BepComponent.h 6     2/22/07 10:59a Rwiersem $
//
// FILE DESCRIPTION:
//   The is the base component for the RTOS system it will provide the base
//   functionality for the majority of the system.
//
//===========================================================================
// Copyright (c) 1999-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/BepComponent.h $
// 
// 6     2/22/07 10:59a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Updated and added Javadoc commments.
// 
// 5     10/26/06 9:08a Rwiersem
// Changes for the 06102701 core release:
// 
// Added Activate() and Deactivate().  These are used to keep the test 
// components running when a new drive curve starts.
// 
// 4     7/20/05 8:41p Cward
// Added support for new TaskMon
//
// 3     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 2     5/09/05 5:00p Gpattison
// Combined the GenericTC::Initialize(filename) with the BepComponent
// and added a member variable to store the parser which contains
// a copy of the config node pointer.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:05p Khykin
// Initial checkin.
//
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:34  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:03a Khykin
// Initial Checkin
//
// 1     1/18/03 8:01a Khykin
// Initial Checkin From DCTC
//
// 43    10/07/02 6:20a Khykin
// Updating for unblock strategy for the write method. This includes
// updating the Handle method to call the BposHandlePulse function if a
// puls is not handled.
//
// 42    7/31/02 11:18a Skypig13
// Updated status set on calls to Abort(), Terminate(), etc.  Removed
// Semaphore methods
//
// 41    5/30/02 5:03p Skypig13
// Latest stuff that compiles
//
// 40    4/25/02 4:39p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//    started
//
//
// 38    4/08/02 1:08p Skypig13
// Changed Notification List pointer to INotification pointer
//
// 37    4/02/02 11:02p Skypig13
// Changes for new Notification List
//
// 36    3/19/02 11:41a Kh
// Updating to use standard defines.
//
// 35    2/25/02 8:47p Skypig13
// Updated Initialize()
//
// 34    2/19/02 5:52p Skypig13
// Updated HandlePulse()
//
// 33    2/18/02 6:56p Skypig13
// Update Initialize()
//
// 32    2/12/02 5:13p Dan.erickson
// Created XmlNode versions of many methods.
//
// 31    2/05/02 12:24a Skypig13
// Restructured due to changes in BepServer
//
// 30    1/24/02 12:36p Kh
// Updating the HandlePulse method to deal with the standard pulse
//
// 29    12/18/01 11:32a Kh
// Adding a Register method for system initialization.
//
// 28    12/14/01 4:49p Kh
// Adding a Resigter method to deal with subscription issues.
//
// 27    12/06/01 5:06p Kh
// Adding an Initialize method that takes a config file.
//
// 26    11/27/01 10:07a Kh
// Adding the SemaphoreTryPost method.
//
// 25    11/20/01 11:09a Kh
// Adding semaphore create and destroy methods.
//
// 24    11/19/01 5:01p Kh
// Adding SemaphoreWait and SemaphorePost methods.
//
// 23    11/02/01 2:48p Dan.erickson
// Added SpecialProcessing method.
//
// 22    11/01/01 6:03p Dan.erickson
// Added GetSpecialInfo method to help various servers
// pass information around the system.
//
// 21    10/17/01 9:54a Skypig13
// Changed prototype of Read() to include data rate
//
// 20    10/08/01 2:46p Kh
// Updating the status of the process when Terminate is called.
//
// 19    9/13/01 10:07a Kh
// Adding the Publish method in the component to support
// IBepCommunication.
//
// 18    9/04/01 11:01a Skypig13
// Changed second parameter name from value to Process to clarify use of
// parameter in Subscribe() and Unsubscribe().
//
// 17    8/30/01 4:16p Skypig13
// Added HandlePulse()
//
// 16    8/15/01 9:20a Kh
// Updating Subscribe and Unsubscribe declarations.
//
// 15    7/23/01 10:07p Kh
// Updated the notification list ivar to refelect the new changes for
// Neutrino.
//
// 14    7/19/01 10:19a Kh
// Updating my spelling mistake which was so humbly pointed out by Tim.
//
// 13    7/19/01 10:12a Kh
// Updating the ReloadConfiguration method.
//
// 12    7/05/01 1:51p Kh
// Updating the command method.
//
// 11    6/27/01 8:36a Kh
// Integrating the Debuggable class.
//
// 10    6/13/01 5:42p Skypig13
// Added Read(), Write(), Command(), Subscribe(), and Unsubscribe()
//
// 9     6/12/00 8:36a Kh
// Update debug methods for common name usage.
//
// 8     6/01/00 3:52p Kh
// Adding debug methods and variables.
//
// 7     5/25/00 3:09p Kh
// Adding Typedefs.
//
// 6     5/23/00 3:23p Kh
// Changing the notification list functionality.
//
// 5     5/22/00 10:20a Kh
// Adding Reset, Abort, and Terminate methods.
//
// 4     5/17/00 9:32a Kh
// Updating Comments for CCDoc.
//
// 3     5/04/00 5:22p Kh
// Adding definitions to add and remove testing declarations.
//
// 2     4/26/01 10:36a Kh
// Finished Testing the component with the new tester.
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 1     12/07/00 1:00p Kevin.Hykin
// Initial Revision
//
//*************************************************************************
#ifndef BEPCOMPONENT_H
#define BEPCOMPONENT_H

#include <string>
#include "Status.h"
#include "BepDefs.h"
#include "XmlNode.h"
#include "ILogger.h"
#include "SystemSetup.h"
#include "INotification.h"
#include "ITaskMon.h"

/**
 * BepComponent base class.  This class provides all system functionality 
 * needed by components.
 *
 * @author Kevin Hykin
 */
class BepComponent : public ILogger, public Status
{
    friend class BepComponentTest;

public:
    /**
     * <b>Description:</b>
     * BepComponent default constructor.
     */
    BepComponent(void);
    /**
     * <b>Description:</b>
     * BepComponent default destructor
     */
    virtual ~BepComponent(void);
    /**
     * <b>Description:</b>
     * Method for initialization passing in the file name.  It also sets the 
     * configuration file name and modification time.
     *
     * @param fileName Name of the XML configuration file to be used to 
     *                 initialize the object.
     */
    virtual void Initialize(const std::string &fileName);
    /**
     * <b>Description:</b>
     * Method for initialization passing in the top level configuration node.
     * It also calls ILogger::Initialize().
     *
     * <b>Configuration Items:</b>
     *     <ul>
     *     <li> Setup/Debug </li>
     *     <li> Setup/DebugLevel </li>
     *     <li> Setup/ProcessPriority </li>
     *     <li> Setup/TaskMon/TaskName </li>
     *     </ul>
     * 
     * @param document Pointer to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * <b>Description:</b>
     * Reloads the configuration file and re-initializes the object.
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
     * states.  This would includes all member variables such as buffers, 
     * lists, and stored data.
     */
    virtual void Reset(void);
    /**
     * <b>Description:</b>
     * Method used to terminate the component.  This performs any cleanup that 
     * the destructor would not and sets the state of the component to 
     * terminate.  It also tells TaskMon that the task is shutting down.
     */
    virtual void Terminate(void);
    /**
     * <b>Description:</b>
     * Indicate that the component should re-activate itself. This should
     * happen at the start of a test when the component is needed.  This method
     * also tells TaskMon that the task is running.
     */
    virtual void Activate(void);
    /**
     * <b>Description:</b>
     * Indicate that the component should de-activate itself. This should
     * happen at the end of a test when the component is no longer needed.
     * This method also tells TaskMon that the task is shutting down.
     */
    virtual void Deactivate(void);
    /**
     * <b>Description:</b>
     * Request to read a data tag based on a passed in string.  This method is 
     * unsupported by this class.
     *
     * @param tag  Data to be read.
     * @param rate Rate the data should be provided.
     * 
     * @return BEP_UNSUPPORTED_RESPONSE
     */
    virtual const std::string Read(const std::string &tag, const INT32 rate = 0);
    /**
     * <b>Description:</b>
     * Request to read a data tag based on a passed in XML node.  This method is
     * unsupported by this class.
     *
     * @param node Data to be read.
     * @param rate Rate the data should be provided.
     * 
     * @return BEP_UNSUPPORTED_RESPONSE
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
    /**
     * <b>Description:</b>
     * Request to write a data tag based on a passed in tag name and value.  
     * This method is unsupported by this class.
     *
     * @param tag   Tag to be written.
     * @param value Data to be written.
     * 
     * @return BEP_UNSUPPORTED_RESPONSE
     */
    virtual const std::string Write(const std::string &tag, const std::string &value);
    /**
     * <b>Description:</b>
     * Request to write a data tag based on a passed in XML node.  This method 
     * is unsupported by this class.
     *
     * @param node Data to be written.
     * 
     * @return BEP_UNSUPPORTED_RESPONSE
     */
    virtual const std::string Write(const XmlNode *node);
    /**
     * <b>Description:</b>
     * Request to perform an action based on a passed in tag name and value.
     *
     * <b>Supported Tag Names:</b>
     *     <ul>
     *     <li> BEP_ACTIVATE </li>
     *     <li> BEP_DEACTIVATE </li>
     *     <li> BEP_TERMINATE </li>
     *     </ul>
     * 
     * @param tag   Tag name of the action to be performed.
     * @param value Value of the data tag.
     * 
     * @return BEP_SUCCESS_RESPONSE if the action is supported.
     *         BEP_UNSUPPORTED_RESPONSE if the action is unsupported.
     */
    virtual const std::string Command(const std::string &tag, const std::string &value);
    /**
     * <b>Description:</b>
     * Request to perform an action based on a passed in XML node.
     *
     * <b>Supported Tag Names:</b>
     *     <ul>
     *     <li> BEP_ACTIVATE </li>
     *     <li> BEP_DEACTIVATE </li>
     *     <li> BEP_TERMINATE </li>
     *     </ul>
     * 
     * @param temp The action to be performed.
     * 
     * @return BEP_SUCCESS_RESPONSE if the action is supported.
     *         BEP_UNSUPPORTED_RESPONSE if the action is unsupported.
     */
    virtual const std::string Command(const XmlNode *temp);
    /**
     * <b>Description:</b>
     * Request to be inserted into the list to be notified of a change in a 
     * data tag.  This method is passed a string containing the data tag name
     * and a string containing the process to notify when the data changes.
     *
     * @param tag     The subscription tag name.
     * @param Process The process name to notify when the data changes.
     * 
     * @return The current value of the data tag if it is found.
     *         BEP_UNSUPPORTED_RESPONSE if the tag is not found.
     */
    virtual const std::string Subscribe(const std::string &tag, const std::string &Process);
    /**
     * <b>Description:</b>
     * Request to be inserted into the list to be notified of a change in a 
     * data tag.  This method is passed an XML node containing the data tag name
     * and the process to notify when the data changes.
     *
     * @param node Node containing the subscription information.
     * 
     * @return The current value of the data tag if it is found.
     *         BEP_UNSUPPORTED_RESPONSE if the tag is not found.
     */
    virtual const std::string Subscribe(const XmlNode *node);
    /**
     * <b>Description:</b>
     * Request to be removed from the list to be notified of a change in a data 
     * tag.  This method is passed a string continaing the data tag name and a
     * string containing the process name that was being notified when the data
     * changed.
     *
     * @param tag     The subscription tag name.
     * @param Process The process name that was being notified when the data 
     *                changed.
     * 
     * @return BEP_SUCCESS_RESPONSE if the subscriber was successfully removed.
     *         BEP_FAILURE_RESPONSE if there was an error removing subscriber.
     */
    virtual const std::string Unsubscribe(const std::string &tag, const std::string &Process);
    /**
     * <b>Description:</b>
     * Request to be removed from the list to be notified of a change in a data 
     * tag.  This method is passed an XML node continaing the data tag name and 
     * the process name that was being notified when the data changed.
     *
     * @param node Node containing the unsubcribe information
     * 
     * @return BEP_SUCCESS_RESPONSE if the subscriber was successfully removed.
     *         BEP_FAILURE_RESPONSE if there was an error removing subscriber.
     */
    virtual const std::string Unsubscribe(const XmlNode *node);
    /**
     * <b>Description:</b>
     * Handle the subscribe data for the process.  This method is unsupported 
     * by this class.
     *
     * @param tag   The subscription tag name.
     * @param value The new data value of the tag
     * 
     * @return BEP_UNSUPPORTED_RESPONSE
     */
    virtual const std::string Publish(const std::string &tag, const std::string &value);
    /**
     * <b>Description:</b>
     * Handle the subscribe data for the process.  This method is unsupported 
     * by this class.
     *
     * @param node The subscription tag name and data value.
     *  
     * @return BEP_UNSUPPORTED_RESPONSE
     */
    virtual const std::string Publish(const XmlNode *node);
    /**
     * <b>Description:</b>
     * Method that is called to command the component to perform any system 
     * registration functions necessary.  This method tells TaskMon that the
     * task is running.
     *
     * @return BEP_SUCCESS_RESPONSE
     */
    virtual const std::string Register(void);
    /**
     * <b>Description:</b>
     * Get any special info from the node.  This method is unsupported by this
     * class.
     *
     * @param tag  The tag name of the source node.
     * @param info The type of information to get.
     * 
     * @return An empty string ("").
     */
    virtual const std::string GetSpecialInfo(const std::string &tag, const std::string info);
    /**
     * <b>Description:</b>
     * Special processing to be done to each node.  Process and description 
     * attributes are added to each node.  This method is unsupported by this
     * class.
     *
     * @param node   The node that receives the new attributes.
     * @param type   The type of operation being performed.
     * @param server The server that sent the request.
     */
    virtual void SpecialProcessing(XmlNode *node, const std::string type, BepComponent *server);
    /**
     * <b>Description:</b>
     * Process the specified pulse.  If the pulse value should not be processed
     * by this class, it will be passed to the operating system.
     *
     * <b>Supported Pulse Code-Value:</b>
     *     <ul>
     *     <li> SYSTEM_PULSE-ABORT_PULSE </li>
     *     <li> SYSTEM_PULSE-RESET_PULSE </li>
     *     <li> SYSTEM_PULSE-RELOAD_PULSE </li>
     *     <li> SYSTEM_PULSE-TERMINATE_PULSE </li>
     *     <li> SYSTEM_PULSE-TASK_MON_PULSE </li>
     *     <li> SYSTEM_PULSE-ACTIVATE_PULSE </li>
     *     <li> SYSTEM_PULSE-DEACTIVATE_PULSE </li>
     *     </ul>
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
     * Returns the current configuration file name.
     *
     * @return The current configuration file name.
     */
    const std::string GetConfigFile(void);
    /**
     * <b>Description:</b>
     * Main run method that subclasses can overload if different behavior is 
     * desired.
     *
     * @param terminateFlag The flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
    
protected:
    /**
     * <b>Description:</b>
     * Sets the current configuration file name based on the passed in string.
     * 
     * @param configFile The configuration file to be used by the component.
     */
    void SetConfigFile(const std::string &configFile);
    /**
     * <b>Description:</b>
     * Sets the current configuration file name based on the passed in char
     * array.
     * 
     * @param configFile The configuration file to be used by the component.
     */
    void SetConfigFile(const char *configFile);
    /**
     * <b>Description:</b>
     * Returns the TaskMon class in which this process belongs.
     *
     * @return TASKMON_CLASS_CORE
     * @see TaskMonClass_t
     */
    virtual TaskMonClass_t GetTaskClass(void) const;
    /**
     * <b>Description:</b>
     * Creates a new ITaskMon object (if not already allocated) and sends a 
     * TaskMon register message.
     */
    virtual void RegisterWithTaskMon(void);
    /**
     * <b>Description:</b>
     * Tells TaskMon that we are up and running.
     */
    virtual void ReportTaskUp(void);
    /**
     * <b>Description:</b>
     * Tells TaskMon that we are about to terminate.
     */
    virtual void ReportTaskEnding(void);

    /**
     * <b>Description:</b>
     * Tells TaskMon that we are exiting.
     */
    virtual void ReportTaskDown(void);

    /** System setup object used to intall signal handlers */
    SystemSetup m_setup;

    /** Configuration file that is being used by the component. */
    std::string m_configFile;

    /** Pointer to notification list */
    INotification* m_nList;

    /**
     * Configuration for the notification list object.
     * NOTE:  This is done so that the creation to the list can be delayed 
     * until needed (if at all)
     */
    XmlNode* m_nListConfig;

    /** Parser for the configuration file. */
    XmlParser m_parser;

    /** TaskMon iterface object */
    ITaskMon *m_taskMon;

    /** Name to use when reporting to TaskMon */
    std::string m_taskMonTaskName;

    /** Task priority for this process */
    INT32 m_priority;

    /** Last modification time of our config file */
    time_t m_cfgModTime;
};

#endif  //BEPCOMPONENT_H
