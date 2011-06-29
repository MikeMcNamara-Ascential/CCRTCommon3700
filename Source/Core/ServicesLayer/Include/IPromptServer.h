//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/IPromptServer.h 5     10/30/07 3:43p Rwiersem $
//
// FILE DESCRIPTION:
//   This defines the interface for PromptServer object.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/IPromptServer.h $
// 
// 5     10/30/07 3:43p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     1/18/07 11:27a Rwiersem
// Changes for the 07011901 core release:
// 
// - Made DisplayPrompt(), ClearPromptBox(), and SetBackground color
// virtual for unit testing.
// 
// 3     9/05/06 4:01p Rwiersem
// Changes for the 06090601 core release:
// 
// - Made DisplayPrompt() virtual
// - Made RemovePrompt() virtual
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 11:44a Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 2     6/26/03 1:42p Khykin
// Updated with latest changes from Flatrock debug.
//
// 13    11/20/02 9:52a Derickso
// Removed parameters from LockPromptServer() and UnLockPromptServer()
// prototypes.
//
// 12    10/17/02 12:24p Derickso
// Changed default priority from MAX_PRIORITY/2 to MIN_PRIORITY.
//
// 11    10/03/02 4:16p Derickso
// Added moethods to support priority based prompting.
//
// 10    7/18/02 10:32a Dan.erickson
// Added methods to send lock and unlock message to the
// PromptServer.
//
// 9     4/25/02 4:41p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//    started
//
//
// 7     3/20/02 3:00p Dan.erickson
// Restored SpecialProcessing method but gave removed the body
// of this method.  Just do not want the IInfoCommunication
// version of this method getting called.
//
// 6     3/05/02 5:03p Dan.erickson
// Removed SpecialProcessing method.  No longer needed now that Xml nodes
// are better supported.
//
// 5     1/14/02 4:16p Dan.erickson
// Add ClearPromptBox and SetBackgroundColor methods.
// Added macros to support these methods.
//
// 4     11/30/01 9:23a Dan.erickson
// Changed server name from PromptServer to core/PromptServer
//
// 3     11/29/01 2:46p Dan.erickson
// Modified DisplayPrompt to take the prompt name instead of
// the prompt number.
//
// 2     11/26/01 3:35p Dan.erickson
// Debugged and tested.
//
// 1     11/26/01 11:37a Dan.erickson
// New file.
//
//
//*************************************************************************

#ifndef IPromptServer_h
#define IPromptServer_h

#include "IInfoCommunication.h"
#include <list>

/**
 * Name of the well-known prompt server
 * @since Version 1.0
 */
#define PROMPT_SERVER_NAME "core/PromptServer"
/**
 * Base tag to use to identify the prompt box.  The number must be appended
 * to identify a specific prompt box.
 * @since Version 1.0
 */
#define PROMPT_BOX         "PromptBox"
/**
 * Used as part of the tag for setting the background color of the prompt box.
 * @since Version 1.1
 */
#define BG_COLOR           "BGColor"
/**
 * Tag to use to indicate to the GUI to clear the prompt box.
 * @since Version 1.1
 */
#define CLEAR_PROMPT_BOX   "Blank"
/**
 * Color to use for the default background color.
 * @since Version 1.1
 */
#define DEFAULT_BG_COLOR   "White"
/**
 * Tags to write to the prompt server for locking and unlocking access.
 * @since Version 1.2
 */
#define LOCK_PROMPT_SERVER   "LockPromptServer"
#define UNLOCK_PROMPT_SERVER "UnlockPromptServer"
/**
 * Range for prompt priority.
 * @since Version 1.3
 */
#define MIN_PRIORITY    0
#define MAX_PRIORITY    9
/**
 * Values used to signal the prompt server to either remove or display a prompt tag.
 * @since Version 1.3
 */
#define REMOVE          "0"
#define DISPLAY         "1"

/**
 * Prompt detail data type
 */
typedef struct
{
    string  detailTag;
    string  detailValue;
} PromptDetail_t;

/**
 * Object used to hold a list of prompt details
 */
typedef std::list< PromptDetail_t>      PromptDetails_t;
typedef PromptDetails_t::iterator       PromptDetailsItr_t;
typedef PromptDetails_t::const_iterator PromptDetailsCItr_t;

/**
 * Object used to hold a list of prompt details
 */
class PromptDetails : public PromptDetails_t
{
public:
    PromptDetails();

    ~PromptDetails();

    /**
     * Add a new prompt detail to the list
     *
     * @param tag    The detail name or tag
     * @param value  The detail value
     */
    void AddDetail( const std::string &tag, const std::string &value);


    /**
     * Get the first detail from the list (without removing it)
     *
     * @param firstDetail
     *               Where to put the prompt detail read from the list
     *
     * @return Iterator to be passed to subsequent calls to GetNextDetail()
     */
    PromptDetailsCItr_t GetFirstDetail( PromptDetail_t &firstDetail) const;


    /**
     * Get the next detail from the list (without removing it)
     *
     * @param nextDetail Where to put the next prompt detail read from the list
     * @param itr        Iterator used to mark the last entry read from the list.
     *
     * @return Iterator to be passed to the next call to GetNextDetail()
     */
    PromptDetailsCItr_t GetNextDetail( PromptDetail_t &nextDetail, PromptDetailsCItr_t itr) const;

};
    
/**
 * Provides the communication interface for the prompt server.
 *
 * @author Dan Erickson
 * @version Version 1.3
 * @since Version 1.0
 */
class IPromptServer : public IInfoCommunication
{
public:
    IPromptServer();
    /**
     * Destructor for the IPromptServer class.
     * @since Version 1.0
     */
    virtual ~IPromptServer();
    /**
     * Create a worker object that will get requests from
     * a channel already open and with message(s) waiting
     *
     * @param creator Object that opened channel to connect to
     * @param id     The id of open channel to talk on.
     * @since Version 1.0
     */
    void Initialize(IBepCommunication &creator, INT32 id = -1);
    /**
     * Setup the communication object.
     *
     * @param server Name of the server to connect to.
     * @param size   Buffer size for messages.
     * @param timeout Time allowed to perform commuinications.
     * @param debug  Turn on/off diagnostic messages.
     * @since Version 1.0
     */
    void Initialize(const std::string &server,
                    const INT32 size    = DEFAULT_BUFFER_SIZE,
                    const INT32 timeout = DEFAULT_TIMEOUT,
                    const bool debug    = false);
    /**
     * Set up the communincation object.
     *
     * @param config Configuration to use
     * @param id     Channel id to talk on.
     * @param rid    Channel to receive on.
     * @since Version 1.0
     */
    void Initialize(const XmlNode *config, const INT32 id = -1, const INT32 rid = -1);
    /**
     * Converts the prompt box number specified into a tag for the prompt box.
     *
     * @param promptBox Prompt Box number to use.
     * @return The tag for the specified prompt box number.
     * @since Version 1.0
     */
    std::string GetPromptBoxTag(const INT32 &promptBox);
    /**
     * Sends a message to display the specified prompt in the specified prompt box.
     * NOTE: Any details must be added before calling this method.
     * NOTE: The default priority level of the prompt will be MIN_PRIORITY.
     *
     * @param promptBox Prompt Box to display the prompt in.
     * @param prompt    Prompt to display.
     * @param response  Response from the write operation.
     * @param priorityLevel
     *                  Priority level of the prompt (0=Lowest, 9=Highest).
     * @return Status of the write operation.
     * @since Version 1.0
     */
    virtual INT32 DisplayPrompt(const INT32 &promptBox,
                        const std::string &prompt,
                        std::string &response,
                        INT32 priorityLevel = MIN_PRIORITY);
    /**
     * Sends a message to display the specified prompt in the specified prompt box
     * with the .specified details
     * NOTE: The default priority level of the prompt will be MIN_PRIORITY.
     *
     * @param promptBox Prompt Box to display the prompt in.
     * @param prompt    Prompt to display.
     * @param promptDetails
     *                  Details for the prompt
     * @param response  Response from the write operation.
     * @param priorityLevel
     *                  Priority level of the prompt (0=Lowest, 9=Highest).
     *
     * @return Status of the write operation.
     * @since Version 1.0
     */
    virtual INT32 DisplayPrompt(const INT32 &promptBox,
                                const std::string &prompt,
                                const PromptDetails &promptDetails,
                                std::string &response,
                                INT32 priorityLevel= MIN_PRIORITY);
    /**
     * Removes a prompt from either the display or from the buffer before it
     * has had a chance to be displayed.
     * NOTE: The default priority level of the prompt will be MIN_PRIORITY.
     *
     * @param promptBox Prompt Box the prmpt is waiting for.
     * @param prompt    Prompt tag to remove.
     * @param response  Response from the server.
     * @param priorityLevel
     *                  Priority level of the prompt (0=Lowest, 9=Highest).
     * @return Result of the operation.
     * @since Version 1.3
     */
    virtual INT32 RemovePrompt(const INT32 &promptBox,const std::string &prompt,
                               std::string &response,
                               INT32 priorityLevel = MIN_PRIORITY);
    /**
     * Method used to clear the prompt box of any prompts and return the background color
     * to the default color.  All buffered prompts will be cleared as well.
     *
     * @param promptBox Prompt box to clear.
     * @param response  Response from the write operation.
     * @return Result of the operation.
     * @since Version 1.1
     */
    virtual INT32 ClearPromptBox(const INT32 &promptBox, std::string &response);
    /**
     * Sets the background color for the prompt box.
     *
     * @param promptBox Prompt box to change background color in.
     * @param color     Color to set the background to.
     * @param response  The response from the server.
     * 
     * @return Status of the operation.
     */
    virtual INT32 SetBackgroundColor(const INT32 &promptBox,
                                     const std::string &color,
                                     std::string &response);
    /**
     * Sends a message to the prompt server to lock access so only the named process can
     * send messages to the prompt server and display prompts on the GUI.
     * All other processes will receive an error when attempting to display prompts.
     *
     * @return Status of the operation.
     * @since Version 1.2
     */
    INT32 LockPromptServer(void);
    /**
     * Unlocks the prompt server so other processes can display prompts on the GUI.
     * Only the process that locked the server can unlock the server.
     *
     * @return Status of the unlock operation.
     * @since Version 1.2
     */
    INT32 UnlockPromptServer(void);
    /**
     * Special processing to be done to each node.  Process and description 
     * attributes are added to each node.
     *
     * @param node   Node to add attributes to.
     * @param type   Type of operation we are performing.
     * @param server The server requesting the special processing.
     */
    virtual void SpecialProcessing(XmlNode *node, const std::string type, BepComponent *server);
    /**
     * Validate an incomming message and return if it is valid
     * or not.
     *
     * @param doc    Document to be validated
     * @return Status of operation
     * @since Version 1.0
     */
    virtual bool ValidateMessage(const XmlNode *doc);
    /**
     * Store the name of the process that created this object.
     *
     * @param processName
     *               Process name.
     * @since Version 1.2
     */
    void SetProcessName(const std::string &processName);
    /**
     * Get the name of the process that created this object.
     *
     * @return Process name.
     * @since Version 1.2
     */
    const std::string& GetProcessName(void);

protected:
    /**
     * Verifies that the attribute is valid
     *
     * @param attribute value of attribute
     * @return If valid
     * @since Version 1.0
     */
    virtual bool CheckAttribute(const XmlNode *attribute);
    /**
     * Convert the provided priority level to string representation.
     *
     * @param priority Priority level to be converted.
     * @return String representation of priority level.
     * @since Version 1.3
     */
    virtual std::string PriorityToString(INT32 priority);


private:
    /**
     * Name of the process that created this object.
     * @since Version 1.2
     */
    std::string m_processName;
};

#endif  //IPromptServer_h
