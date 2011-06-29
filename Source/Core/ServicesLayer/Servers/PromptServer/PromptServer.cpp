//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/PromptServer/PromptServer.cpp 5     10/26/06 1:18p Rwiersem $
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/PromptServer/PromptServer.cpp $
// 
// 5     10/26/06 1:18p Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 4     3/02/06 12:43a Cward
// Added ability to reload config file without rebooting
//
// 2     2/17/06 2:14p Rwiersem
// Updated comments.
//
// 3     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 2     5/09/05 5:00p Gpattison
// Re-arranged the Initialize() method to call the base class initialize
// as soon as possible
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:42p Cward
//
// 3     10/24/03 1:47p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 3     10/15/03 8:52a Khykin
// Updated with latest Korea changes.
//
// 2     10/10/03 10:19a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/07/03 2:52p Khykin
// Updated with changes from Flatrock.
//
// 2     6/26/03 4:25p Khykin
// Updated with Flatrocks latest changes.
//
// 26    6/24/03 8:42a Tbochene
// Added data acces control for thread safety and changed
// printf() statements to Log () statements.
//
// 25    3/17/03 3:43p Tbochene
// Updated data access and bounds cheking the prompt box number
//
// 22    11/20/02 10:08a Derickso
// Removed validation of prompts.  Prompt validation is now handled by the
// GUI.
//
// 20    10/17/02 12:24p Derickso
// Changed default priority from MAX_PRIORITY/2 to MIN_PRIORITY.
//
// 19    10/11/02 9:58a Derickso
// Modified Priority based prompting to allow MIN_PRIORITY priority
// prompts to be overwritten without first being removed.
//
// 18    10/07/02 12:59p Derickso
// Added methods to support priority based prompting.
//
// 17    9/26/02 4:36p Tbochene
// Removed compiler warnings
//
// 16    8/23/02 10:29a Dan.erickson
// Added GetSpecialInfo method to return the Prompt attribute from a
// prompt node.
//
// 15    8/01/02 10:41a Skypig13
// Re-ordered initializer list in constructor to remove compiler warnings
//
// 14    7/18/02 10:31a Dan.erickson
// Added functionality to allow locking and unlocking access
// to the PromptServer.
//
// 13    5/06/02 3:52p Dan.erickson
// Corrected memory leak.
//
// 12    4/25/02 4:44p Skypig13
// Updated with new changes
//    Revision 1.1  2002/04/24 01:03:21  erickson
//    *** empty log message ***
//
//
// 10    3/20/02 3:08p Dan.erickson
// Removed obsolete Notify() and SpecialProcessing() methods.
//
// 9     3/05/02 5:06p Dan.erickson
// Updated to make better use of Xml nodes.
// Removed SpecialProcessing method.
//
// 8     3/05/02 10:36a Dan.erickson
// Made path of PromptFile based on USR_ROOT environment variable.
//
// 7     3/04/02 4:54p Dan.erickson
// Updated Initialize method to set m_comm prior to calling
// BepServer::Initialize().
//
// 6     2/12/02 5:14p Dan.erickson
// Updated after restructure of BepServer.
//
// 5     1/14/02 4:15p Dan.erickson
// Modified Write method to handle setting the background color
// of the prompt boxes.
// Modified LoadPromptFile to load the prompt tags into the
// publish tags list.
//
// 4     1/07/02 8:43a Dan.erickson
// Modified Initialize method to look for configuration data
// under the setup node.
//
// 3     12/03/01 9:59a Dan.erickson
// Added Initialize(fileName) method, LoadPromptList, Write,
// ReloadConfiguration, and ValidPrompt methods.
//
// 2     11/26/01 4:41p Dan.erickson
// Added Constructor and Destructor methods.
//
// 1     11/26/01 4:11p Dan.erickson
// PromptServer class.
//
//*************************************************************************

#include "PromptServer.h"
#include "PromptServerCommThreadPool.h"

PromptServer::PromptServer() : BepServer(), m_lockProcess(""), m_numberOfPromptBoxes(MAX_PROMPT_BOXES),
m_maximumPriority(MAX_PRIORITY), m_serverLocked(false), m_connectToQnxDataServer(false)
{
    pthread_mutexattr_t attr;

    memset( &attr, 0, sizeof( attr));
    memset( &m_queueDataLock, 0, sizeof( m_queueDataLock));

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_DISABLE);
    pthread_mutex_init(&m_queueDataLock, &attr);
}


PromptServer::~PromptServer()
{
    pthread_mutex_destroy(&m_queueDataLock);
    BepServer::Terminate();
}

void PromptServer::Initialize(const XmlNode *document)
{
    // Initialize the thread pool
    m_comm = new PromptServerCommThreadPool(this);
    // Use the base class to complete the initialization
    BepServer::Initialize(document);
    Log(LOG_FN_ENTRY, "PromptServer::Initialize(%s)", document->ToString().c_str());

    // load the additional configuration items needed by the prompt server
    LoadAdditionalConfigurationItems(document);
}

void PromptServer::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    // get the number of prompt boxes that are managed
    try
    {
        SetNumberOfPromptBoxes(BposReadInt(document->getChild("Setup/NumberOfPromptBoxes")->getValue().c_str()));
    }
    catch(BepException &e)
    {
        SetNumberOfPromptBoxes(MAX_PROMPT_BOXES);
    }
    // Get the number of priority queues for each prompt box
    try
    {
        SetMaximumPriority(BposReadInt(document->getChild("Setup/MaximumPriority")->getValue().c_str()));
    }
    catch(BepException &e)
    {
        SetMaximumPriority(MAX_PRIORITY);
    }
    // Determine if we need to connect to the QNX Data Server
    bool connectToServer = false;
    try
    {
        connectToServer = atob(document->getChild("Setup/ConnectToQnxDataServer")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Not connecting to QNX Data Server: %s", excpt.GetReason());
    }
    ConnectToQnxDataServer(&connectToServer);
    // Determine if we need to connect to the QNX Data Server
    if(ConnectToQnxDataServer())  m_qnxDataServer.ConnectToQnxDataServer();
}

const std::string PromptServer::Read(const XmlNode *node, const INT32 rate /*= 0*/)
{   // Make sure the node exists
    bool locked = false;
    std::string result("");
    if(FindTag(node->getName()))
    {
        try
        {   // Set the prompt tag to return
            int status;
            if((status = pthread_mutex_lock(&m_queueDataLock)) == EOK)
            {
                locked = true;
                result = m_data.getNode(node->getName())->getAttribute(BEP_PROMPT)->getValue();

                if(pthread_mutex_unlock(&m_queueDataLock) != EOK)
                    Log(LOG_ERRORS, "1 Failed to release tag data lock because %s\n", strerror(errno));
            }
            else Log(LOG_ERRORS, "1 Failed to acquire tag data lock because %d, %s\n", status, strerror(errno));
        }
        catch(...)
        {   // A prompt tag was not requested
            if(locked)
            {
                if(pthread_mutex_unlock(&m_queueDataLock) != EOK)
                    Log(LOG_ERRORS, "2 Failed to release tag data lock because %s\n", strerror(errno));
            }
            result = BepServer::Read(node, rate);
        }
    }
    return(result);
}

const std::string PromptServer::Write(const XmlNode *node)
{
    std::string result = BEP_ERROR_RESPONSE, promptTag("");
    bool displayPrompt = false;
    Log(LOG_FN_ENTRY, "PromptServer::Write(%s)\n", node->ToString().c_str());
    // lock access to the prompt server data while updating
    try
    {
        // Check if this is a request to lock the prompt server
        if(node->getName() == LOCK_PROMPT_SERVER)
        {   // Lock the prompt server
            Log(LOG_DEV_DATA, "Attempting to lock prompt server\n");
            result = LockPromptServer(node->getAttribute(BEP_PROCESS)->getValue());
        }
        else if(node->getName() == UNLOCK_PROMPT_SERVER)
        {   // Unlock the prompt server
            Log(LOG_DEV_DATA, "Attempting to unlock prompt server\n");
            result = UnlockPromptServer(node->getAttribute(BEP_PROCESS)->getValue());
        }
        else
        {   // Handle the tag normally
            Log(LOG_DEV_DATA, "Processing a normal tag\n");
            if(IsPromptServerLocked() && (node->getAttribute(BEP_PROCESS)->getValue() == GetLockProcess()))
            {   // The prompt server is locked and the locking process is displaying a prompt
                displayPrompt = true;
            }
            else if(!IsPromptServerLocked())
            {   // The prompt server is not locked, allow prompt display
                displayPrompt = true;
            }
            else
            {   // Another process has the server locked
                displayPrompt = false;
            }
            // Determine if the prompt server should display the prompt
            if(displayPrompt)
            {   // Attempt to retrieve the prompt tag
                try
                {
                    promptTag = node->getAttribute(BEP_PROMPT)->getValue();
                }
                catch(XmlException &XmlErr)
                {
                    promptTag.erase();
                }
                // Check if this is a promt or background color
                if (!promptTag.empty())
                {   // Determine if this is a remove request
                    Log(LOG_DEV_DATA, "PromptServer: Received valid prompt - %s\n", node->ToString().c_str());
                    if (node->getValue() == REMOVE)
                    {   // Remove the prompt from the server
                        result = RemovePrompt(node);
                        // Check if any prompts are queued for display
                        DisplayPrompt();
                    }
                    else if ((node->getValue() == DISPLAY) &&
                             (node->getAttribute(BEP_PROMPT)->getValue() == CLEAR_PROMPT_BOX))
                    {   // Remove all queued prompts from the specified box
                        CleanPromptBox(GetPromptBoxNumber(node->getName()));
                        // No need to queue the prompt since the queues have been cleared.  Just let everyone know
                        result = BepServer::Write(node);
                    }
                    else if (node->getValue() == DISPLAY)
                    {   // Check if PromptServer is locked
                        if (IsPromptServerLocked())
                        {   // Server is locked, bypass the queue operations
                            result = BepServer::Write(node);
                        }
                        else
                        {   // Queue the prompt for display
                            result = QueuePrompt(node);
                            // Update the prompt displays
                            DisplayPrompt();
                        }
                    }
                }
                else
                {   // This is a valid publish tag
                    Log(LOG_DEV_DATA, "PromptServer: Received valid tag - %s\n", node->ToString().c_str());
                    result = BepServer::Write(node);
                }
            }
            else
            {   // Place a message in the log and set status to failure
                Log(LOG_ERRORS, "Cannot display %s, prompt server was locked by %s\n",
                    node->ToString().c_str(), GetLockProcess().c_str());
                result = BEP_FAILURE_RESPONSE;
            }
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Exceptional condition in PromptServer::Write(): %s\n", e.what());
    }

    return(result);
}

const std::string PromptServer::GetSpecialInfo(const std::string &tag, const std::string info)
{
    std::string promptValue = "Blank";
    Log(LOG_FN_ENTRY, "PromptServer::GetSpecialInfo(tag: %s, info: %s)\n", tag.c_str(), info.c_str());
    // Get the prompt attribute
    if(BEP_PROMPT == info)
    {   // Find the node
        Log(LOG_DEV_DATA, "Reading %s \n", tag.c_str());
        XmlNode readNode(tag, "");
        // Read the prompt
        promptValue = Read(&readNode);
        Log(LOG_DEV_DATA, "Prompt Value: %s\n", promptValue.c_str());
    }
    Log(LOG_FN_ENTRY, "PromptServer::GetSpecialInfo() complete, returning %s\n", promptValue.c_str());
    return(promptValue);
}

const std::string PromptServer::QueuePrompt(const XmlNode *promptNode)
{   // Get the prompt box and priority level from the node
    std::string returnStatus;
    Log(LOG_FN_ENTRY, "PromptServer::QueuePrompt() begin -- node: %s\n", promptNode->ToString().c_str());
    try
    {
        INT32 promptBox = GetPromptBoxNumber(promptNode->getName()) - 1;
        Log(LOG_DETAILED_DATA, "QueuePrompt in box# %d\n", promptBox);
        INT32 priority;// = atoi(promptNode->getAttribute(BEP_PRIORITY)->getValue().c_str());
        Log(LOG_DETAILED_DATA, "QueuePrompt: Getting prompt priority...\n");
        const XmlString prioStr = promptNode->getAttribute(BEP_PRIORITY)->getValue();

        if ( prioStr.empty() == false)
        {
            priority = atoi(prioStr.c_str());
        }
        else
        {
            priority = 0;
            Log(LOG_ERRORS, "\tEmpty prompt priority attribute in QueuePrompt...defaulting to priority %d\n", priority);
        }
        Log(LOG_DETAILED_DATA, "Prompt priority: %d\n", priority);
        // Range check the prompt box number
        Log(LOG_DETAILED_DATA, "Range checking the prompt box number...\n");
        if (promptBox < 0) promptBox = 0;
        else if (promptBox > (GetNumberOfPromptBoxes()-1))   promptBox = GetNumberOfPromptBoxes()-1;
        // Range check the priority level
        Log(LOG_DETAILED_DATA, "Range checking the priority level...\n");
        if (priority < 0)  priority = 0;
        else if (priority > GetMaximumPriority())  priority = GetMaximumPriority();
        // Attempt to queue the prompt
        Log(LOG_DEV_DATA, "Attempting to queue prompt -- promptBox: %d, priority: %d\n", promptBox, priority);
        if ((promptBox < 0) || (promptBox > (GetNumberOfPromptBoxes()-1)))
        {   // An invalid prompt box was specified
            Log(LOG_ERRORS, "Invalid prompt box specified - %d, not queuing prompt\n", GetPromptBoxNumber(promptNode->getName()));
            returnStatus = BEP_SOFTWAREFAIL_RESPONSE;
        }
        else
        {   // Lock the mutex while we mess with the priority queues
            int status;
            bool mutexLocked = false;
            try
            {
                Log(LOG_DETAILED_DATA, "Locking mutex so prompt can be added to queue...\n");
                if ((status = pthread_mutex_lock(&m_queueDataLock)) == EOK)
                {   // Flag that the mutex was locked
                    Log(LOG_DETAILED_DATA, "Mutex locked, adding prompt to the queue\n");
                    mutexLocked = true;
                    // Add the node to the appropriate queue
                    Log(LOG_DETAILED_DATA, "Adding prompt:%s to box:%d priority queue:%d\n",
                        promptNode->getAttribute(BEP_PROMPT)->getValue().c_str(), promptBox+1, priority);
                    m_promptBoxQueue[promptBox][priority].push_back(promptNode->Copy());
                    Log(LOG_DEV_DATA, "Added prompt:%s to box:%d priority queue:%d\n",
                        promptNode->getAttribute(BEP_PROMPT)->getValue().c_str(), promptBox+1, priority);
                    returnStatus = BEP_SUCCESS_RESPONSE;
                    // Unlock the mutex for others
                    if (pthread_mutex_unlock(&m_queueDataLock) != EOK)
                        Log(LOG_ERRORS, "Failed to release tag data lock in QueuePrompt because %s\n", strerror(errno));
                }
                else Log(LOG_ERRORS, "Failed to acquire tag data lock in QueuePrompt because %d, %s\n", status, strerror(errno));
            }
            catch (XmlException &err)
            {
                Log(LOG_ERRORS, "Error queuing prompt! -- XmlException thrown - %s\n", err.what());
                returnStatus = BEP_SOFTWAREFAIL_RESPONSE;
                // Unlock the mutex for others
                if (mutexLocked)
                {
                    if (pthread_mutex_unlock(&m_queueDataLock) != EOK)
                        Log(LOG_ERRORS, "Failed to release tag data lock in QueuePrompt because %s\n", strerror(errno));
                }
                else
                {
                    Log(LOG_ERRORS, "XmlException -- Mutex not locked - status: %s\n", strerror(errno));
                }
            }
            catch (BepException &err)
            {
                Log(LOG_ERRORS, "Error queuing prompt! -- BepException thrown - %s\n", err.what());
                returnStatus = BEP_SOFTWAREFAIL_RESPONSE;
                // Unlock the mutex for others
                if (mutexLocked)
                {
                    if (pthread_mutex_unlock(&m_queueDataLock) != EOK)
                        Log(LOG_ERRORS, "Failed to release tag data lock in QueuePrompt because %s\n", strerror(errno));
                }
                else
                {
                    Log(LOG_ERRORS, "BepException -- Mutex not locked - status: %s\n", strerror(errno));
                }
            }
            catch (...)
            {
                Log(LOG_ERRORS, "Error queuing prompt! -- Unknown exception thrown -- promptNode: %p, %s\n", promptNode, strerror(errno));
                returnStatus = BEP_SOFTWAREFAIL_RESPONSE;
                // Unlock the mutex for others
                if (mutexLocked)
                {
                    if (pthread_mutex_unlock(&m_queueDataLock) != EOK)
                        Log(LOG_ERRORS, "Failed to release tag data lock in QueuePrompt because %s\n", strerror(errno));
                }
                else
                {
                    Log(LOG_ERRORS, "Unknown Exception -- Mutex not locked - status: %s\n", strerror(errno));
                }
            }
        }
    }
    catch (BepException &err)
    {
        Log(LOG_ERRORS, "Error queuing prompt! - BepException thrown - %s\n", err.what());
        returnStatus = BEP_SOFTWAREFAIL_RESPONSE;
    }
    Log(LOG_FN_ENTRY, "PromptServer::QueuePrompt() Exit -- status: %s\n", returnStatus.c_str());
    return(returnStatus);
}

const std::string PromptServer::RemovePrompt(const XmlNode *promptNode)
{
    XmlNodeMapItr iter;
    std::string status = BEP_FAILURE_RESPONSE;
    bool removedNode = false;

    try
    {
        // Convert the prompt box and priority to int
        Log(LOG_DEV_DATA, "Removing Prompt: %s\n", promptNode->ToString().c_str());
        INT32 promptBox = GetPromptBoxNumber(promptNode->getName()) - 1;
        INT32 priority;// = atoi(promptNode->getAttribute(BEP_PRIORITY)->getValue().c_str());
        const XmlString prioStr = promptNode->getAttribute(BEP_PRIORITY)->getValue();

        /// Only convert the prompt priority if the priority attribute is not empty
        if( prioStr.empty() == false)
        {
            priority = atoi(prioStr.c_str());
        }
        else
        {
            priority = 0;
            Log( "\tEmpty prompt priority attribute in RemovePrompt()...defaulting to priority %d\n", priority);
        }

        // Range check the prompt box number
        if(promptBox < 0) promptBox = 0;
        else if(promptBox > (GetNumberOfPromptBoxes()-1))  promptBox = GetNumberOfPromptBoxes() - 1;
        // Get the prompt
        std::string prompt = promptNode->getAttribute(BEP_PROMPT)->getValue();
        // Check if the prompt is currently displayed
        int mutexStatus;
        bool promptFound = false;   // flag to indicate that the prompt was found
        if((mutexStatus = pthread_mutex_lock(&m_tagDataLock)) == EOK)
        {
            if((iter = m_data.find(promptNode->getName())) != m_data.end())
            {   // Check if our prompt is in the display
                try
                {
                    if((iter->second->getAttribute(BEP_PROMPT)->getValue() == prompt) &&
                       (iter->second->getValue() == DISPLAY))
                        promptFound = true;
                }
                // if the prompt did not exist
                catch(BepException &e)
                {
                    promptFound = false;
                }
            }
            // unlock the list before the publish
            if(pthread_mutex_unlock(&m_tagDataLock) != EOK)
                Log(LOG_ERRORS, "3 Failed to release tag data lock because %s\n", strerror(errno));
            // if the prompt was found, remove it
            if(promptFound)
            {   // Remove the prompt from the display
                XmlNode clearNode(promptNode->getName(), promptNode->getValue());
                clearNode.addChild(BEP_PROMPT, CLEAR_PROMPT_BOX, ATTRIBUTE_NODE);
                clearNode.addChild(BEP_DATA_TYPE, BEP_PROMPT, ATTRIBUTE_NODE);
                removedNode = BepServer::Write(&clearNode) == BEP_SUCCESS_RESPONSE;
                m_qnxDataServer.UpdateQnxDataServerVariable(promptNode->getName(), CLEAR_PROMPT_BOX);
                status = BEP_SUCCESS_RESPONSE;

                Log(LOG_DEV_DATA, "Removed prompt %s from GUI -- Box:%d, Priority:%d\n",  prompt.c_str(), promptBox, priority);
            }
        }
        else Log(LOG_ERRORS, "2 Failed to acquire tag data lock because %d, %s\n", mutexStatus, strerror(errno));

        // If the prompt has not ben successfully removed -- check queues
        if(!removedNode && !promptFound)
        {   // lock access to the priority queues
            if((mutexStatus = pthread_mutex_lock(&m_queueDataLock)) == EOK)
            {   // Find the prompt in the queue
                try
                {
                    for(PriorityQueueItr pIter = m_promptBoxQueue[promptBox][priority].begin();
                       pIter != m_promptBoxQueue[promptBox][priority].end() && !removedNode;
                       pIter++)
                    {   // Check if we found the node
                        if((*pIter)->getAttribute(BEP_PROMPT)->getValue() == prompt)
                        {   // Remove the node from the queue
                            XmlNode *delNode = *pIter;
                            m_promptBoxQueue[promptBox][priority].erase(pIter);
                            delete delNode;
                            delNode = NULL;
                            removedNode = true;
                            status = BEP_SUCCESS_RESPONSE;
                            Log(LOG_DEV_DATA, "Removed prompt %s from queue -- Box:%d, Priority:%d\n", prompt.c_str(), promptBox, priority);
                        }
                    }
                }
                catch(BepException &e)
                {
                    Log(LOG_ERRORS, "Unexpected Error Getting the PROMPT: %s\n", e.what());
                }
                // unlock the list before the publish
                if(pthread_mutex_unlock(&m_queueDataLock) != EOK)
                    Log(LOG_ERRORS, "Failed to release tag data lock in RemovePrompt:2 because %s\n", strerror(errno));
            }
            else Log(LOG_ERRORS, "2 Failed to acquire tag data lock in RemovePrompt:2 because %d, %s\n", mutexStatus, strerror(errno));
        }
        // Check if we did not find the prompt on the GUI or in the queue
        if(!removedNode)
        {
            Log(LOG_ERRORS, "Did not find prompt %s on the GUI or in the queue -- Box:%d, Priority:%d.  Must have already been removed\n",
                prompt.c_str(), promptBox, priority);
            status = BEP_SUCCESS_RESPONSE;
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "PromptServer::RemovePrompt() BepException: %s\n", e.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "PromptServer::RemovePrompt() Unknown Exception\n");
    }
    return(status);
}

void PromptServer::DisplayPrompt(void)
{   // Check each prompt box to determine if it is available for display
    Log(LOG_FN_ENTRY, "PromptServer::DisplayPrompt() -- Looking for a prompt to display\n");
    try
    {
        bool boxAvailable = false;
        char buff[10];
        Log(LOG_FN_ENTRY, "PromptServer::DisplayPrompt() -- Looking for a prompt to display\n");
        for(INT16 box = 0; box < GetNumberOfPromptBoxes(); box++)
        {   // Convert the box number to a tag
            std::string promptBox = std::string(PROMPT_BOX) + itoa(box+1, buff, 10);
            XmlNodeMapItr iter;
            // Default to box not available
            boxAvailable = false;

            int status = pthread_mutex_lock(&m_tagDataLock);
            if(status == EOK)
            {   // Look for the box
                if((iter = m_data.find(promptBox)) != m_data.end())
                {   // The box has a prompt -- OK if Blank
                    Log(LOG_DEV_DATA, "%s was recently displayed -- %s\n", promptBox.c_str(), iter->second->ToString().c_str());
                    std::string promptValue;
                    INT16 promptPriority = MIN_PRIORITY;   // Set to default priority
                    try
                    {   // Get the prompt value
                        promptValue = iter->second->getAttribute(BEP_PROMPT)->getValue();
                    }
                    catch(BepException &e)
                    {   // At startup, prompt node may not have the appropriate attributes
                        promptValue = iter->second->getValue();
                    }
                    try
                    {   // Get the priority of the prompt
                        promptPriority = atoi(iter->second->getAttribute(BEP_PRIORITY)->getValue().c_str());
                    }
                    catch(...)
                    {   // Set priority level to default
                        promptPriority = MIN_PRIORITY;
                    }
                    Log(LOG_DEV_DATA, "current prompt value:%s, priority:%d\n", promptValue.c_str(), promptPriority);
                    boxAvailable = (promptValue == CLEAR_PROMPT_BOX) || (promptPriority == MIN_PRIORITY);
                }
                else
                {   // Box was not displayed -- OK to use
                    boxAvailable = true;
                }
                if(pthread_mutex_unlock(&m_tagDataLock) != EOK)     Log(LOG_ERRORS, "4 Failed to release tag data lock because %s\n", strerror(errno));
            }
            else Log(LOG_ERRORS, "3 Failed to acquire tag data lock because %d, %s\n", status, strerror(errno));

            Log(LOG_DEV_DATA, "Prompt box %d is %s\n", box+1, boxAvailable ? "Available" : "NOT Available");
            // If the box is available, attempt to display a prompt
            if(boxAvailable && !IsPromptBoxQueueEmpty(box))
            {   // Look for prompts starting with the highest priority
                Log(LOG_DEV_DATA, "OK to display prompt.  Looking for prompt to display.\n");
                bool promptDisplayed = false;
                int status;
                if((status = pthread_mutex_lock(&m_queueDataLock)) == EOK)
                {
                    try
                    {
                        for(INT32 priority = GetMaximumPriority(); (priority >= MIN_PRIORITY) && !promptDisplayed; priority--)
                        {   // Make sure there is a prompt to display
                            Log(LOG_DEV_DATA, "Checking Box:%d Priority Queue:%d for a prompt to display\n", box+1, priority);
                            if(!m_promptBoxQueue[box][priority].empty())
                            {
                                const XmlNode *promptNode = m_promptBoxQueue[box][priority].front();
                                // Display the first prompt in the queue
                                Log(LOG_DEV_DATA, "Displaying prompt %s\n", m_promptBoxQueue[box][priority].front()->ToString().c_str());
                                BepServer::Write( promptNode);
                                m_qnxDataServer.UpdateQnxDataServerVariable(promptBox, 
                                                                            promptNode->getAttribute(BEP_PROMPT)->getValue());
                                // Remove the prompt from the queue and set prompt displayed flag
                                m_promptBoxQueue[box][priority].pop_front();
                                delete promptNode;
                                promptNode = NULL;
                                promptDisplayed = true;
                            }
                            else
                            {   // Priority queue is empty
                                Log(LOG_DEV_DATA, "Prompt Box:%d Priority queue:%d is empty - no prompts to display\n", box+1, priority);
                            }
                        }
                    }
                    catch(BepException &e)
                    {
                        Log(LOG_ERRORS, "Unexpected Error Displaying The Prompt: %s\n", e.what());
                    }

                    if(pthread_mutex_unlock(&m_queueDataLock) != EOK)
                        Log(LOG_ERRORS, "Failed to release tag data lock in DisplayPrompt() because %s\n", strerror(errno));
                }
                else Log(LOG_ERRORS, "Failed to acquire tag data lock in DisplayPrompt because %d, %s\n", status, strerror(errno));
            }
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "PromptServer::DisplayPrompt() BepException: %s\n", e.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "PromptServer::DisplayPrompt() Unknown Exception\n");
    }
}

inline void PromptServer::CleanPromptBox(const INT32 &promptBox)
{
    int status;

    if( promptBox < GetNumberOfPromptBoxes())
    {
        if((status = pthread_mutex_lock(&m_queueDataLock)) == EOK)
        {   // Clear the priority queues
            Log(LOG_DEV_DATA, "Clearing prompt box %d priority queues...\n", promptBox);
            for (INT16 priority = GetMaximumPriority(); priority >= MIN_PRIORITY; priority--)
            {   // Need to remove all nodes from the priority queues
                Log(LOG_DETAILED_DATA, "\tClearing priority queue %d...\n", priority);
                for (UINT16 index = 0; index < m_promptBoxQueue[promptBox-1][priority].size(); index++)
                {
                    const XmlNode *promptNode = m_promptBoxQueue[promptBox-1][priority].front();
                    Log(LOG_DETAILED_DATA, "\tRemoving %s from queue...\n", promptNode->getName().c_str());
                    // Pop the node off the front
                    m_promptBoxQueue[promptBox-1][priority].pop_front();
                    // Delete the node and set the pointer to NULL
                    delete promptNode;
                    promptNode = NULL;
                }
                Log(LOG_DETAILED_DATA, "Priority queue %d cleared!\n", priority);
            }
            Log(LOG_DEV_DATA, "Prompt box %d priority queues cleared!\n", promptBox);
            // Remove the prompt node from m_data
            char buf[10];
            XmlNodeMapItr iter;
            std::string promptBoxTag(std::string(PROMPT_BOX) + itoa(promptBox,buf,10));
            if ((status = pthread_mutex_lock(&m_tagDataLock)) == EOK)
            {
                if ((iter = m_data.find(promptBoxTag)) != m_data.end())
                {
                    Log(LOG_DETAILED_DATA, "\tDeleting node %s\n", promptBoxTag.c_str());
                    if( iter->second != NULL)
                    {
                        delete iter->second;           // Delete the memory associated with the node
                        iter->second = NULL;           // Point the pointer to NULL
                    }
                    else
                    {
                        Log( LOG_ERRORS, "\tNode %s already NULL !!!\n", promptBoxTag.c_str());
                    }
                    // Remove the node from the map
                    Log(LOG_DETAILED_DATA, "\tRemoving %s from m_data...\n", promptBoxTag.c_str());
                    m_data.delNode(promptBoxTag);
                }
                if(pthread_mutex_unlock(&m_tagDataLock) != EOK) Log(LOG_ERRORS, "Failed to release m_data lock because %s\n", strerror(errno));
            }
            else Log(LOG_ERRORS, "Failed to lock m_data access in CleanPromptBox() - %s\n", strerror(errno));
            Log(LOG_DETAILED_DATA, "Cleared all priority queues for prompt box %d\n", promptBox);
            if(pthread_mutex_unlock(&m_queueDataLock) != EOK)   Log(LOG_ERRORS, "5 Failed to release tag data lock because %s\n", strerror(errno));
        }
        else Log(LOG_ERRORS, "4 Failed to acquire tag data lock because %d, %s\n", status, strerror(errno));
    }
    else Log(LOG_ERRORS, "Invalid prompt box index in CleanPromptBox( %d): max=%d\n", promptBox, GetNumberOfPromptBoxes());
}

bool PromptServer::IsPromptBoxQueueEmpty(const INT32 &promptBox)
{
    bool queueEmpty = true;
    INT32 status;
    if((status = pthread_mutex_lock(&m_queueDataLock)) == EOK)
    {
        // Print a listing of prompts queued at each priority
        Log(LOG_DEV_DATA, "Priority queue for Prompt Box:%d --\n", promptBox+1);
        for(INT16 pri = GetMaximumPriority(); pri >= MIN_PRIORITY; pri--)
        {   // Display the priority header
            Log(LOG_DEV_DATA, "\tPriority: %d -- \n", pri);
            if(m_promptBoxQueue[promptBox][pri].empty())
            {   // Current queue is empty - no prompts
                Log(LOG_DEV_DATA, "\t\tQueue is EMPTY\n");
            }
            else
            {   // Current queue contains prompts for display
                for(PriorityQueueItr iter = m_promptBoxQueue[promptBox][pri].begin(); iter != m_promptBoxQueue[promptBox][pri].end(); iter++)
                {   // Print out all queued prompts
                    Log(LOG_DEV_DATA, "\t\tPrompt: %s\n", (*iter)->ToString().c_str());
                }
            }
        }
        // Check each priority queue to ensure it is empty
        INT16 priority = GetMaximumPriority();
        for(; (priority >= MIN_PRIORITY) && queueEmpty; priority--)
            queueEmpty = m_promptBoxQueue[promptBox][priority].empty();
        // Adjust the priority number to deal with the extra decrement
        priority++;
        Log(LOG_DEV_DATA, "Prompt Box %d queue %d is %s -- size:%d\n", promptBox+1, priority,
            queueEmpty ? "Empty" : "NOT Empty", m_promptBoxQueue[promptBox][priority].size());
        if(!queueEmpty) Log(LOG_DEV_DATA, "Found a prompt at priority %d -- prompt: %s\n", priority,
                            m_promptBoxQueue[promptBox][priority].front()->ToString().c_str());
        if(pthread_mutex_unlock(&m_queueDataLock) != EOK)
            Log(LOG_ERRORS, "Failed to release tag data lock IsPromptBoxQueueEmpty:1 because %s\n", strerror(errno));
    }
    else Log(LOG_ERRORS, "Failed to acquire tag data lock in IsPromptBoxQueueEmpty:1 because %d, %s\n", status, strerror(errno));
    // Return the status
    return(queueEmpty);
}

const std::string PromptServer::LockPromptServer(const std::string &processName)
{
    std::string result(BEP_ERROR_RESPONSE);
    // Check to see if server is lready locked
    if(IsPromptServerLocked())
    {   // Server is alerady locked
        Log(LOG_ERRORS, "WARNING: %s attempting to lock server.  Server previously locked by %s\n", processName.c_str(), GetLockProcess().c_str());
        result = BEP_FAILURE_RESPONSE;
    }
    else
    {   // Lock the prompt server if a name was provided
        if(processName != "")
        {   // Set the lock status to Lock
            SetLockStatus(true);
            // Set the process name of the locking process
            SetLockProcess(processName);
            result = BEP_SUCCESS_RESPONSE;
            Log(LOG_DEV_DATA, "PromptServer locked by %s\n", processName.c_str());
        }
        else
        {   // No proces name was provided, do not allow locking
            result = BEP_SOFTWAREFAIL_RESPONSE;
            Log(LOG_ERRORS, "PromptServer can not be locked without specifying a process name\n");
        }
    }
    // Return the result of the lock attempt
    return(result);
}

const std::string PromptServer::UnlockPromptServer(const std::string &processName)
{
    std::string result(BEP_ERROR_RESPONSE);
    if(IsPromptServerLocked() && (processName == GetLockProcess()))
    {   // The locking process has requested server to be unlocked
        SetLockStatus(false);
        SetLockProcess("");
        result = BEP_SUCCESS_RESPONSE;
        Log(LOG_DEV_DATA, "PromptServer unlocked by %s\n", processName.c_str());
    }
    else if(IsPromptServerLocked() && (processName != GetLockProcess()))
    {   // Another process attempted to unlock the prompt server
        result = BEP_FAILURE_RESPONSE;
        Log(LOG_ERRORS, "WARNING: %s attempting to unlock PromptServer.  Server previously locked by %s\n", processName.c_str(), GetLockProcess().c_str());
    }
    else if(!IsPromptServerLocked())
    {   // Server was not locked, do nothing
        result = BEP_SUCCESS_RESPONSE;
        Log(LOG_DEV_DATA, "PromptServer was not previously locked.\n");
    }
    // Return the status of the unlock attempt
    return(result);
}

//-------------------------------------------------------------------------------------------------
const bool& PromptServer::ConnectToQnxDataServer(const bool *connectToServer) /*= NULL*/
{
    if(connectToServer != NULL)  m_connectToQnxDataServer = *connectToServer;
    return m_connectToQnxDataServer;
}

