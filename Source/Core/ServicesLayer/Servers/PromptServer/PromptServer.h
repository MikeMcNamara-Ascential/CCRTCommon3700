//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/PromptServer/PromptServer.h 4     10/31/07 10:07a Rwiersem $
//
// FILE DESCRIPTION:
//   This defines the PromptServer object.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/PromptServer/PromptServer.h $
// 
// 4     10/31/07 10:07a Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     10/26/06 1:19p Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 2     3/02/06 12:43a Cward
// Added ability to reload config file without rebooting
//
// 2     2/17/06 2:14p Rwiersem
// Updated comments.
//
// 1     12/21/03 6:42p Cward
//
// 3     10/24/03 1:47p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 2     10/15/03 8:52a Khykin
// Updated with latest Korea changes.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/07/03 8:05a Khykin
// Updated with latest changes from Flatrock.
//
// 2     6/26/03 3:55p Khykin
// Updated with Flatrocks latest changes.
//
// 15    3/27/03 11:34a Tbochene
// Added mutext ivar back in.
//
// 1     1/27/03 7:09a Khykin
// Initial Checkin
//
// 1     1/18/03 8:06a Khykin
// Initial Checkin From DCTC
//
// 13    11/20/02 9:55a Derickso
// Removed support for prompt file validation.  GUI will now handle prompt
// validation and verification.
//
// 11    10/07/02 12:58p Derickso
// Added mothods to support priority based prompting.
//
// 10    8/23/02 8:24a Dan.erickson
// Added GetSpecialInfo method.
//
// 9     7/18/02 10:31a Dan.erickson
// Added functionality to allow locking and unlocking access
// to the PromptServer.
//
// 8     4/25/02 8:18a Dan.erickson
// Added Read() method so clients can read data from the server.
//
// 7     3/20/02 3:09p Dan.erickson
// Removed obsolete Notify() and SpecialProcessing() methods.
//
// 6     3/05/02 5:03p Dan.erickson
// Removed SpecialProcessing method.  No longer needed now that Xml nodes
// are better supported.
//
// 5     2/18/02 1:11p Dan.erickson
// Removed prototype for Handle method.
//
// 4     2/12/02 5:14p Dan.erickson
// Updated after restructure of BepServer.
//
// 3     12/03/01 10:06a Dan.erickson
// Added Initialize(fileName) method, LoadPromptList, Write,
// ReloadConfiguration, ValidPrompt, SetPromptFileName and
// GetPromptFileName methods.
//
// 2     11/26/01 4:42p Dan.erickson
// Added Constructor and Destructor methods.
//
// 1     11/26/01 4:11p Dan.erickson
// PromptServer class.
//
//*************************************************************************

#ifndef PromptServer_h
#define PromptServer_h

#include "BepServer.h"
#include "IPromptServer.h"
#include "BepSync.h"
#include "IQnxDataServer.h"
#include <list>


/**
 * Maximum number of prompt boxes on the GUI
 * @since Version 1.4
 */
#define MAX_PROMPT_BOXES  5
/**
 * Queue to hold prompt nodes for display.
 * @since Version 1.3
 */
typedef list<XmlNode*> PriorityQueue;
typedef PriorityQueue::iterator PriorityQueueItr;

/**
 * Extends BepServer to handle processing of prompts.
 *
 * @author Dan Erickson
 * @version Version 1.4
 * @since Version 1.0
 */
class PromptServer : public BepServer
{
    friend class PromptServerTest;

public:
    /**
     * Class Constructor
     * @since Version 1.0
     */
    PromptServer();
    /**
     * Class Destructor.
     * @since Version 1.0
     */
    virtual ~PromptServer();
    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Prompt tag that is currently displayed for the requested prompt box
     * @since Version 1.0
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
    /**
     * Request to write a data tag
     *
     * @param node  Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 1.0
     */
    virtual const std::string Write(const XmlNode *node);
    /**
     * Get any special info from the node
     *
     * @param tag    Node to get information from.
     * @param info   Type of information to get.
     * @return Information that was part of the specified node.
     * @since Version 1.2
     */
    virtual const std::string GetSpecialInfo(const std::string &tag, const std::string info);
    /**
     * Convert the prompt box name to a number.
     *
     * @param promptBox Prompt box name
     * @return Prompt Box number.
     * @since Version 1.3
     */
    inline const INT32 GetPromptBoxNumber(const std::string &promptBox)
    {
        INT32       promptBoxNumber;
        std::string promptBoxTag(PROMPT_BOX);

        // If valid prompt box tag
        if( promptBox.length() > promptBoxTag.length())
        {
            Log( LOG_DEV_DATA, "\tGetPromptBoxNumber( %s): length=%d\n", promptBox.c_str(), promptBox.length());
            unsigned char boxNumber = promptBox.substr(promptBoxTag.length(), 1)[0];
            promptBoxNumber = boxNumber & 0x0F;
            Log( LOG_DEV_DATA, "\tGetPromptBoxNumber( %s): Using prompt box #%d\n", promptBox.c_str(), promptBoxNumber);
        }
        // Default to prompt box 1
        else
        {
            promptBoxNumber = 1;
            Log( "\tGetPromptBoxNumber( %s): Invalid prompt box identifier...defaulting to prompt box %d\n", promptBox.c_str(), promptBoxNumber);
        }

        return( promptBoxNumber);
    };
    /**
     * Returns the lock status of the prompt server.
     *
     * @return True - Prompt server has been locked.
     * @since Version 1.1
     */
    inline const bool& IsPromptServerLocked(void)
    {
        return m_serverLocked;
    };
    /**
     * Get the name of the process that locked access to the prompt server.
     *
     * @return Process name that locked access to the prompt server.
     * @since Version 1.1
     */
    inline const std::string& GetLockProcess(void)
    {
        return m_lockProcess;
    };
    /**
     * Get the number of supported prompt boxes.
     *
     * @return Number of supported prompt boxes.
     * @since Version 1.4
     */
    inline const INT32 &GetNumberOfPromptBoxes(void)
    {
        return m_numberOfPromptBoxes;
    };
    /**
     * Get the number of priority queues for each prompt box.
     *
     * @return Number of priority queues for each prompt box.
     * @since Version 1.4
     */
    inline const INT32 &GetMaximumPriority(void)
    {
        return m_maximumPriority;
    };

protected:
    /**
     * Load the additional configuration items needed by the prompt server.
     * The "standard" items will be handled by the call to
     * BepServer::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    /**
     * Place a prompt node in the appropriate prompt queue.
     *
     * @param promptNode Prompt node to add to queue for display.
     * @return Status of the queue operation.
     * @since Version 1.3
     */
    virtual const std::string QueuePrompt(const XmlNode *promptNode);
    /**
     * Remove a prompt from either the GUI display or the prompt queue.
     *
     * @param promptNode Prompt to be removed from display or prompt queue.
     * @return Status of the remove operation.
     * @since Version 1.3
     */
    virtual const std::string RemovePrompt(const XmlNode *promptNode);
    /**
     * Displays the next prompt based on the priority level.
     * @since Version 1.3
     */
    virtual void DisplayPrompt(void);
    /**
     * Removes all queued prompts from the priority queues of the specified prompt box.
     * The prompt box on the GUI will also be cleared.
     *
     * @param promptBox Prompt Box to clean out.
     * @since Version 1.3
     */
    virtual void CleanPromptBox(const INT32 &promptBox);
    /**
     * Returns true if all priority queues are empty for the specified prompt box.
     * NOTE: Calling methid should lock a mutex prior to calling
     * this method so no collisions happen with the priority queues.
     *
     * @param promptBox Prompt Box to check.
     * @return True -  All priority queues are empty.
     *         False - All priority queues are NOT empty.
     * @since Version 1.3
     */
    virtual bool IsPromptBoxQueueEmpty(const INT32 &promptBox);
    /**
     * Set the name of the process that has locked access to the prompt server.
     *
     * @param lockProcess
     *               Name of the locking process.
     * @since Version 1.1
     */
    inline void SetLockProcess(const std::string &lockProcess)
    {
        m_lockProcess = lockProcess;
    };
    /**
     * Set the lock status of the prompt server.
     *
     * @param serverLocked
     *               Serverock status
     * @since Version 1.1
     */
    inline void SetLockStatus(const bool &serverLocked)
    {
        m_serverLocked = serverLocked;
    };
    /**
     * Store the number of prompt boxes being used.
     *
     * @param numberOfPromptBoxes
     *               Number of prompt boxes to support.
     * @since Version 1.4
     */
    inline void SetNumberOfPromptBoxes(const INT32 &numberOfPromptBoxes)
    {
        if( numberOfPromptBoxes <= MAX_PROMPT_BOXES)
        {
            m_numberOfPromptBoxes = numberOfPromptBoxes;
        }
        else
        {
            m_numberOfPromptBoxes = MAX_PROMPT_BOXES;
            Log( LOG_ERRORS, "Attempt to set max prompt boxes too high (%d > %d)...defaulting to MAX_PROMPT_BOXES\n",
                 numberOfPromptBoxes, MAX_PROMPT_BOXES);
        }
    };
    /**
     * Store the number of priority queues supported for each prompt box.
     *
     * @param maximumPriority The maximum allowable priority.
     */
    inline void SetMaximumPriority(const INT32 &maximumPriority)
    {
        m_maximumPriority = maximumPriority;
    };

private:
    /**
     * Locks access to the prompt server.
     *
     * @param processName
     *               Name of the process locking access to the prompt server.
     * @return Status of the locking operation.
     * @since Version 1.1
     */
    const std::string LockPromptServer(const std::string &processName);
    /**
     * Unlocks access to the prompt server so all process can display prompts.
     *
     * @param processName
     *               Name of the process requesting the server to be unlocked.
     * @return Status of the unlock operation.
     * @since Version 1.1
     */
    const std::string UnlockPromptServer(const std::string &processName);
    /**
     * Get/Set the flag indicating of the QNX Data Server should be connected to.
     * 
     * @param connectToServer
     *               Flag indicating if the QNX data Server should be used.
     * 
     * @return Flag indicating if the QNX data Server should be used.
     */
    const bool& ConnectToQnxDataServer(const bool *connectToServer = NULL);
    /**
     * Priority queues for the prompt boxes.
     * @since Version 1.3
     */
    PriorityQueue m_promptBoxQueue[MAX_PROMPT_BOXES][MAX_PRIORITY+1];
    /**
     * Name of the process that has the prompt server locked.
     * @since Version 1.1
     */
    std::string m_lockProcess;
    /**
     * Number of prompt boxes allowed for display.
     * @since Version 1.4
     */
    INT32 m_numberOfPromptBoxes;
    /**
     * Number of priority queues allowed for each prompt box.
     * @since Version 1.4
     */
    INT32 m_maximumPriority;
    /**
     * Flag that indicates the lock status of the server.
     * True - prompt server has been locked.
     * @since Version 1.1
     */
    bool m_serverLocked;

    /**
     * Mutex lock variable for locking the prompt server queues.
     * @since Version 1.4
     */
    mutable pthread_mutex_t m_queueDataLock;
    /**
     * Flag to indicate if the server should connect to the QNX Data Server.
     */
    bool m_connectToQnxDataServer;
    /**
     * Interface to the QNX Data Server.
     */
    IQnxDataServer m_qnxDataServer;
};

#endif  //PromptServer_h
