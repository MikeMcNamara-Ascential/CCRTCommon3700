//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/IFaultServer.h 4     10/30/07 3:38p Rwiersem $
//
// FILE DESCRIPTION:
//   This defines the interface for FaultServer object.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/IFaultServer.h $
// 
// 4     10/30/07 3:38p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     1/18/07 11:25a Rwiersem
// Changes for the 07011901 core release:
// 
// - Made SetFault() and ClearFault() virtual for unit testing.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 11:40a Khykin
// Removed unused semaphore.
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:12p Khykin
// Initial checkin.
//
// 2     5/19/03 11:21a Khykin
// Updating to make methods thread safe.
//
// 14    10/17/02 11:07a Derickso
// Added functionality to clear all faults.
//
// 13    4/25/02 4:40p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//    started
//
//
// 11    3/11/02 1:16p Dan.erickson
// Removed SpecialProcessing() method.
//
// 10    2/26/02 2:48p Dan.erickson
// Updated to take advantage of XmlNode type data.
//
// 9     1/15/02 4:01p Dan.erickson
// Added SubscribeAllFaults method.
// Overloaded BuildMessage to add MssgClass attribute to all
// messages.
//
// 8     12/03/01 11:38a Dan.erickson
// Changed name of server.
//
// 7     11/26/01 3:38p Dan.erickson
// Uncommented default values in Initialize prototype.
//
// 6     11/21/01 3:04p Dan.erickson
// Changed Severity level range to 0-9.
//
// 5     11/21/01 9:41a Dan.erickson
// Overloaded IBepCommunication::Read() to set attributes prior
// to calling IBepCommunication::Read() method.
//
// 4     11/20/01 5:37p Dan.erickson
// Expanded GetNextFault to return attributes as well.
//
// 3     11/19/01 3:57p Dan.erickson
// Added ALL_FAULTS definition.
// Added ProcessNameToTag, severityLevelToTag, GetAllFaults,
// GetNextFault, and SpecialProcessing methods.
//
// 2     11/16/01 4:34p Dan.erickson
// Testing.
//
// 1     11/15/01 4:06p Dan.erickson
// New file.
//
//*************************************************************************
#ifndef IFaultServer_h
#define IFaultServer_h

#include "IInfoCommunication.h"
#include "BepSemaphore.h"

/**
 * Value to indicate the server should set the fault.
 * @since Version 1.0
 */
#define SET                  "1"
/**
 * Value to indicate the server should clear a fault.
 * @since Version 1.0
 */
#define CLEAR                "0"
/**
 * Name of the fault server to connect to.
 * @since Version 1.0
 */
#define FAULT_SERVER_NAME    "core/FaultServer"
/**
 * Special tag to indicate the retrieval of all faults.
 * @since Version 1.0
 */
#define ALL_FAULTS           "AllFaults"
/**
 * Minimum severity level allowed.
 * @since Version 1.0
 */
#define MIN_SEVERITY         0
/**
 * Maximum severity level allowed.
 * @since Version 1.0
 */
#define MAX_SEVERITY         9

/**
 * Interface for communicating with the FaultServer.
 *
 * @author Dan Erickson
 * @version Version 1.3
 * @see IInfoCommunication, IBepCommunication
 * @since Version 1.0
 */
class IFaultServer : public IInfoCommunication
{
public:
    /**
     * Default Constructor
     * @since Version 1.0
     */
    IFaultServer();

    /**
     * Default Destructor
     * @since Version 1.0
     */
    virtual ~IFaultServer();

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
     * @return
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
     * @return
     * @since Version 1.0
     */
    void Initialize(const XmlNode *config, const INT32 id = -1, const INT32 rid = -1);

    /**
     * Set the severity of the fault.
     *
     * @param severity  Severity level.
     * @param existNode Existing XmlNode to add the severity attribute to
     *
     * @return The given node (existNode) with the attribute added.
     * @since Version 1.1
     */
    XmlNode * SetSeverity(const std::string &severity, XmlNode *existNode);

    /**
     * Set a fault.
     *
     * @param fault  Name of the fault.
     * @param description
     *                 Fault descrpition.
     * @param process Process that is reporting the fault.
     * @param response Response from set operation.
     * @param isFinal Is this the last fault to set.
     * @param severity Fault severity
     * @return Status of the SetFault operation.
     * @since Version 1.0
     */
    virtual INT32 SetFault(const std::string &fault, 
                           const std::string &description,
                           const std::string &process, std::string &response,
                           const bool isFinal = false, INT32 severity = -1);

    /**
     * Clear a fault that had been set.
     *
     * @param fault  Fault to clear.
     * @param response Response from the clear operation.
     * @param isFinal Is this the last fault to clear.
     * @return Result of the clear operation.
     * @since Version 1.0
     */
    virtual INT32 ClearFault(const std::string &fault, std::string &response, 
                             const bool isFinal = false);

    /**
     * Clear all faults from the fault server.
     *
     * @param response Response from the server.
     * @return Result of the clear operation.
     * @since Version 1.2
     */
    INT32 ClearAllFaults(std::string &response);

    /**
     * Get the default severity level currently in use.
     *
     * @return Default severity level in use.
     * @since Version 1.0
     */
    INT32 GetDefaultSeverityLevel(void);

    /**
     * Set the default severity level to use.
     *
     * @param severityLevel
     *               Default severity level.
     * @since Version 1.0
     */
    void SetDefaultSeverityLevel(INT32 severityLevel);

    /**
     * Get all the faults that have been generated.
     *
     * @param response Response to the read operation.
     * @param isFinal Retrieve all faults now.
     * @return Status of the read operation.
     * @since Version 1.0
     */
    INT32 GetAllFaults(std::string &response, const bool isFinal = false);

    /**
     * Get all faults at the specified severity level.
     *
     * @param severityLevel
     *                 Severity level of the faults to retrieve.
     * @param response Response from the read operation.
     * @param isFinal Is this the final severity level to retrieve.
     * @return Status of the read operation.
     * @since Version 1.0
     */
    INT32 GetSeverityLevelFaults(INT32 severityLevel, std::string &response, const bool isFinal = false);

    /**
     * Get all faults generated by the specified process.
     *
     * @param processName
     *                 Name of the process that generated the faults.
     * @param response Response from the read operation.
     * @param isFinal Is this the last process name to get faults for.
     * @return Status of the read operation.
     * @since Version 1.0
     */
    INT32 GetProcessFaults(const std::string &processName, std::string &response, const bool isFinal = false);

    /**
     * Read information in the system based on the tag value
     * passed.
     *
     * @param node    Data to be read
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of operation
     *         Defined return types:
     *         All tags sent and received Ok                == BEP_STATUS_SUCCESS
     *         Tag was added to list (waiting for last tag) == 1
     *         Read operation failed to read all tags       == -1
     *         Read operation failed                        == -2
     * @since Version 1.0
     */
    virtual INT32 Read(const XmlNode *node, std::string &response, const bool IsFinal = false);

    /**
     * Subscribe for all faults that have the specified severity level.
     *
     * @param severityLevel
     *                 Severity level of the faults to subscribe for.
     * @param server Name of the server that will receive the data.
     * @param response Reponse from the subscribe operation.
     * @param isFinal Is this the final severity level to subscribe for.
     * @return Status of the subscribe operation.
     * @since Version 1.0
     */
    INT32 SubscribeSeverityLevelFaults(INT32 severityLevel, const std::string &server,
                                       std::string &response, const bool isFinal = false);

    /**
     * Subscribe for all faults generated by the specified process.
     *
     * @param processName
     *                 The process that generated the fault.
     * @param server Name of the server to receive the data.
     * @param response Response from the subscribe operation.
     * @param isFinal Is this the final process name to subscribe for.
     * @return Status of the subscribe operation.
     * @since Version 1.0
     */
    INT32 SubscribeProcessFaults(const std::string &processName, const std::string &server,
                                 std::string &response, const bool isFinal = false);

    /**
     * Subscribe for all faults that can be generated in the system.
     *
     * @param server Name of server to receive data.
     * @param response Response to the subscribe operation.
     * @param isFinal Is this the final item to subscribe for.
     * @return Status of the subscribe operation.
     * @since Version 1.1
     */
    INT32 SubscribeAllFaults(const std::string &server, std::string &response, const bool isFinal = false);

    /**
     * Build a message compliant with the Bep communication protocol
     *
     * @param function Function message will perform.
     * @param message  The location to store the new message.
     * 
     * @return Message that was built
     */
    void BuildMessage(const std::string function, std::string& message);

    /**
     * Validate an incomming message and return if it is valid
     * or not.
     *
     * @param doc    Document to be validated
     * @return Status of operation
     * @since Version 1.0
     */
    virtual bool ValidateMessage(const XmlNode* doc);

    /**
     * Retrieves the next tag from the response
     *
     * @param tag    Data requested
     * @param value  Value of data
     * @param description
     *                 The fault description.
     * @param process The process that reported the fault.
     * @param severity The severity level of the fault.
     * @param message Message being processed
     * @return Number of items left to process
     * @since Version 1.0
     */
    virtual const INT32 GetNextFault(std::string &tag, std::string &value, std::string &description,
                                     std::string &process, std::string &severity, std::string &message);

    /**
     * Converts all "/" to "_" in the process name so the
     * process name can be used as a tag.
     *
     * @param name   Process name to use as a tag.
     * @return The converted process name.
     * @since Version 1.0
     */
    std::string ProcessNameToTag(const std::string &name);

protected:
    /**
     * Verifies that the attribute is valid
     *
     * @param attribute Value of attribute
     * @return If valid
     * @since Version 1.0
     */
    virtual bool CheckAttribute(const XmlNode *attribute);

    /**
     * Convert the severity level to a form that can
     * be used as an XML tag.
     *
     * @param severityLevel
     *               Severity level.
     * @return The tag representation of the tag.
     * @since Version 1.0
     */
    std::string SeverityLevelToTag(INT32 severityLevel);

private:
    /**
     * Converts the severity level to a string.
     *
     * @param severity Severity level.
     * @return Character respresentation of the severity level.
     * @since Version 1.0
     */
    std::string SeverityToString(INT32 severity);

    /**
     * The default severity level to use.
     * @since Version 1.0
     */
    INT32 m_severityLevel;
};

#endif // IFaultServer_h
