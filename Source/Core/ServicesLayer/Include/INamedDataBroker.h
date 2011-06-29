//******************************************************************************
//
//     $Header: /Ccrt/Source/Core/ServicesLayer/Include/INamedDataBroker.h 3     10/30/07 3:40p Rwiersem $
//
// Description:
//  Client interface to Named Data Broker.
//
//==============================================================================
//
//     Copyright © 2001 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/INamedDataBroker.h $
// 
// 3     10/30/07 3:40p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 2     8/06/03 3:50p Khykin
// Updated with changes from Flatrock.
//
// 1     6/12/03 4:13p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:38  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:05a Khykin
// Initial Checkin
//
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
//
// 37    11/20/02 7:00a Khykin
// Updated the constructor definition to take the size of the receive
// buffer and a timeout.
//
// 36    7/31/02 11:34a Skypig13
// Made IsDataOk() public
//
// 35    6/27/02 3:50p Skypig13
// Updated GetNext() methods
//
// 34    6/25/02 5:17p Skypig13
// Add GetMessageStatus()
//
// 33    6/19/02 2:27p Skypig13
// Updated SendToServer() method to take XmlNode pointer to reference
//
// 32    5/30/02 4:19p Skypig13
// Added Close() method
//
// 31    4/08/02 1:59p Skypig13
// Added XmlNode versions of Read(), Write(), etc.
//
// 30    4/02/02 11:06p Skypig13
// Updates for efficiency
//
// 29    2/26/02 2:04p Skypig13
// Updated prototype for state machine
//
// 28    2/12/02 5:17p Dan.erickson
// Updated to make better use of XmlNode Data.
//
// 27    2/05/02 12:38a Skypig13
// Restructured to be more properly OO and initilization cannot be done
// incorrectly
//
// 26    12/10/01 4:14p Skypig13
// Changed prototypes for state machine methods
//
// 25    11/29/01 2:58p Skypig13
// Add subscriber name and mutator methods
//
// 24    11/20/01 4:22p Skypig13
// Make StateMachine() public
//
// 23    11/01/01 9:48p Skypig13
// Changed prototype for FinalMethod()
//
// 22    11/01/01 6:00p Skypig13
// Added bad, test, and final data structures and chaged prototype
// for FinalMethod()
//
// 21    10/29/01 1:13p Skypig13
// Removed Initialize method and pre-processor compiler directive
// USE_COMM_MAP
//
// 20    10/23/01 12:52a Skypig13
// Optimize Communications
//
// 18    10/22/01 4:59p Kh
// Updating to move the alias numbering into the base class.
//
// 17    10/16/01 12:01a Skypig13
// Added value parameter to SendToNdb()
//
// 16    9/27/01 6:57a Kh
// Updating the StateMachine method.
//
// 15    9/26/01 1:30p Kh
// Updating for the ability to Write, Subscribe, Unsubscrbe, and Command.
//
// 14    8/17/01 1:42p Skypig13
// Added alias counter Ivar
//
// 12    7/27/01 4:26p Skypig13
// Weekly backup
//
// 11    7/24/01 11:29a Skypig13
// Latest changes
//
// 10    7/19/01 3:25p Skypig13
// Latest round of testing that passed
//
// 9     7/17/01 8:58a Skypig13
//
// 8     7/12/01 4:12p Skypig13
// Compilable using new base calss
//
// 7     7/07/00 2:21p Skypig13
// Beer truck prevention (may not compile, get last version if it does
// not)
//
//
//******************************************************************************
#ifndef INamedDataBroker_h
#define INamedDataBroker_h


#include <map>
#include <string>

#include "XmlNode.h"
#include "BepSync.h"
#include "BepXmlUtils.h"
#include "BepException.h"
#include "INamedDataBrokerBase.h"

class INamedDataBrokerTest; // Forward reference to testing class
/**
 * Interface class to the NamedDataBroker that will query the NamedDataBroker
 * for the Id of who will provide the requested data.  This class is used by
 * all clients that wish to get data in the system.
 *
 * @author Tim Bochenek
 * @version 2.8
 * @since Version 1.0
 */
class INamedDataBroker : protected INamedDataBrokerBase
{
public:
    friend class INamedDataBrokerTest;
    /**
     * Class constructor (default)
     * @since Version 1.0
     */
    INamedDataBroker(void);
    /**
     * Class constructor (initializing)
     *
     * @param debug   Flag to enable and disable debug information. true - enable
     * @param size    The size of the receive buffer to use.
     * @param timeout The amount of time to wait for the response.
     * @since Version 1.0
     */
    INamedDataBroker(const bool debug, const INT32 size = DEFAULT_BUFFER_SIZE,
                                   const INT32 timeout = DEFAULT_TIMEOUT);
    /**
     * Class destructor
     * @since Version 1.0
     */
    virtual ~INamedDataBroker(void);
    /**
     * Method to read data from a server
     *
     * @param tag      Tag to read
     * @param response Parameter to receive response
     * @param IsFinal  Is this the last request of this type?
     *                 (If true message will be built and sent to Server)
     * @return If operation succeded
     * @since Version 2.0
     */
    virtual INT32 Read(const std::string &tag, std::string &response,
                       const bool IsFinal = false);
    /**
     * Read information in the system based on the tag value
     * passed.
     *
     * @param node   Information to be read.
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
    virtual INT32 Read(const XmlNode *node, std::string &response,
                       const bool IsFinal = false);
    /**
     * Write data to a server
     *
     * @param tag      Tag of data to be written
     * @param value    Acutal data to be written
     * @param response Data returned by operation
     * @param IsFinal  If this tag is the las tag to be requested
     * @return Status of operation
     * @since Version 1.0
     */
    virtual INT32 Write(const std::string &tag, const std::string &value,
                        std::string &response, const bool IsFinal = false);
    /**
     * Write data to a server
     *
     * @param node   Data to be written.
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of operation
     * @since Version 1.0
     */
    virtual INT32 Write(const XmlNode *node, std::string &response, const bool IsFinal = false);
    /**
     * Command a value in the system.  This would be used in the case where one 
     * process in the system needs to inform another process of an event.  An 
     * example is when the user presses the enter key and the driver display is 
     * on the start selection a Start event should be sent to the System 
     * Controller.
     *
     * @param tag      Command to be executed.
     * @param value    The value of the command being executed.
     * @param response Data returned by operation.
     * @param IsFinal  If this tag is the las tag to be requested.
     * 
     * @return Status of operation
     */
    virtual INT32 Command(const std::string &tag, const std::string &value,
                          std::string &response, const bool IsFinal = false);
    /**
     * Command a value in the system.  This would be used in
     * the case where one process in the system needs to inform
     * another process of an event.  An example is when the
     * user presses the enter key and the driver display is on
     * the start selection a Start event should be sent to the
     * System Controller.
     *
     * @param node   Information to be commanded.
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of operation
     * @since Version 1.0
     */
    virtual INT32 Command(const XmlNode *node, std::string &response, const bool IsFinal = false);
    /**
     * Subscribe for notification of a specified tag
     *
     * @param tag    Tag to subcribe for notification
     * @param value    Server name that will be notified when data changes
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of subsribe operation
     * @since Version 1.0
     */
    virtual INT32 Subscribe(const std::string &tag, const std::string &value,
                            std::string &response, const bool IsFinal = false);
    /**
     * Subscribe for notification of a specified tag
     *
     * @param node   Data to be Subscribed for.
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of subsribe operation
     * @since Version 1.0
     */
    virtual INT32 Subscribe(const XmlNode *node, std::string &response, const bool IsFinal = false);
    /**
     * Remove from subscription list for notification of a specified tag
     *
     * @param tag    Tag to subcribe for notification
     * @param value    Server name that will be removed from subscription list
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of subsribe operation
     * @since Version 1.0
     */
    virtual INT32 Unsubscribe(const std::string &tag, const std::string &value,
                              std::string &response, const bool IsFinal = false);
    /**
     * Retrieves the next tag from the response
     *
     * @param tag     Data requested
     * @param value   Value of data
     * @param message Message being processed
     * @return Number of items left to process
     * @since Version 1.0
     */
    virtual const INT32 GetNext(std::string &tag, std::string &value, std::string &message);

    /**
     * Retrieves the next tag from the response.
     *
     * @param node    Node to populate.
     * @param message Message being processed.
     * 
     * @return Number of items left to process.
     */
    const INT32 GetNext(XmlNode &node, std::string &message);

    /**
     * Retrieves the tag requested from the response
     *
     * @param tag     Data requested
     * @param value   Value of data
     * @param message Message being processed
     * @return Status of operation
     *         BEP_STATUS_SUCCESS == Tag requested is valid in response
     *         BEP_STATUS_ERROR   == Tag not in response
     *         BEP_STATUS_FAILURE == No tags in response
     * @since Version 1.5
     */
    virtual const INT32 GetByTag(const std::string tag, std::string &value, std::string &message);

    /**
     * Returns the status of data (from GetNext() for example)
     *
     * @return Status of data
     * @since Version 2.7
     */
    const std::string GetMessageStatus(void);

    /**
     * Remove from subscription list for notification of a specified tag
     *
     * @param node   Node to be Published.
     * @param response Data returned by operation
     * @param IsFinal If this tag is the las tag to be requested
     * @return Status of publish operation
     * @since Version 2.4
     */
    virtual INT32 Publish(const XmlNode *node, std::string &response, const bool IsFinal = false);

    /**
     * Resets Communication structure so a message can be resent.
     *
     * @since Version 1.7
     */
    void ResetCommunications(void);

    /**
     * Closes the channel object (if exists) is connected to
     * @since Version 2.6
     */
    virtual void Close(void) const;

    /**
     * Checks if data response was Ok
     *
     * @param data   Data to be evaluated
     * @return If data is not a failure mode
     * @since Version 2.8
     */
    bool IsDataOk(const XmlString &data);

protected:
    /**
     * The internal state machine
     *
     * @param function What function to execute
     * @param dataNode      Data to be processed
     * @param response Response to operation
     * @param IsFinal  If request is the last one
     * @return Status of operation
     * @since Version 2.2
     */
    INT32 StateMachine(const std::string function, const XmlNode *dataNode,
                       std::string &response, const bool IsFinal = false);
    /**
     * Method to attempt to reteive data from the server that supplies the 
     * data.  The method will start with TestData and iterate across all items 
     * in the list requesting data from the servers.  Upon successful retrieval 
     * of the data it will be put into FinalData, else it will be put into 
     * BadData to be verified or invalidated.
     *
     * @param tag       Data invloved in operation.
     * @param value     Value of data.
     * @param TestData  Container of data items invloved in operation(s).
     * @param FinalData Container for data items that passed.
     * 
     * @return Number of data items that failed (0 == All Passed)
     */
    const INT32 GetData(const std::string &tag, std::string &value,
                        XmlNodeMap &TestData, XmlNodeMap &FinalData);
    /**
     * Method called during start state of process
     *
     * @param function  Function to execute
     * @param TestData  Data to be sent to server
     * @param BadData   Data that failed to be sent (out parameter)
     * @param FinalData Data to be returned to the client
     * @return Next state
     * @since Version 1.0
     */
    virtual const ServerIdState StartMethod(const std::string &function,
                                            XmlNodeMap &TestData,
                                            XmlNodeMap &BadData,
                                            XmlNodeMap &FinalData);
    /**
     * Method called during PrimaryFunctionMethod state of process.
     *
     * @param function  Function to execute.
     * @param message   The message to send to the server.
     * @param TestData  Data to be sent to server.
     * @param BadData   Data that failed to be sent (out parameter).
     * @param FinalData Data to be returned to the client.
     * 
     * @return Next state
     */
    virtual const ServerIdState PrimaryFunctionMethod(const std::string function,
                                                      std::string& message,
                                                      XmlNodeMap &TestData,
                                                      XmlNodeMap &BadData,
                                                      XmlNodeMap &FinalData);
    /**
     * Method called during Verify state if process
     * (This is request to NamedDataBroker to validate id of server)
     *
     * @param function  Function to execute
     * @param TestData  Data to be sent to server
     * @param BadData   Data that failed to be sent (out parameter)
     * @param FinalData Data to be returned to the client
     * @return Next state
     * @since Version 1.0
     */
    virtual const ServerIdState VerifyMethod(const std::string &function,
                                             XmlNodeMap &TestData,
                                             XmlNodeMap &BadData,
                                             XmlNodeMap &FinalData);
    /**
     * Method called during SecondaryFunction state of process.
     *
     * @param function  Function to execute.
     * @param message   The message to send to the server.
     * @param TestData  Data to be sent to server.
     * @param BadData   Data that failed to be sent (out parameter).
     * @param FinalData Data to be returned to the client.
     * 
     * @return Next state
     */
    virtual const ServerIdState SecondaryFunctionMethod(const std::string function,
                                                      std::string& message,
                                                        XmlNodeMap &TestData,
                                                        XmlNodeMap &BadData,
                                                        XmlNodeMap &FinalData);
    /**
     * Method called during InValidate state of process
     * (This is used to request the NamedDataBroker to invalidate is of server)
     *
     * @param function  Function to execute
     * @param TestData  Data to be sent to server
     * @param BadData   Data that failed to be sent (out parameter)
     * @param FinalData Data to be returned to the client
     * @return Next state
     * @since Version 1.0
     */
    virtual const ServerIdState InValidateMethod(const std::string &function,
                                                 XmlNodeMap &TestData,
                                                 XmlNodeMap &BadData,
                                                 XmlNodeMap &FinalData);
    /**
     * Method called during Final state of process
     * (Method used to package data and send to client)
     *
     * @param function  Function being performed (Read, Write, etc.)
     * @param FinalData Location of final results
     * @param SendData  Data structure used to hold sent data
     * @param message   Message to be populated with results
     * @return Next state
     * @since Version 1.0
     */
    const virtual ServerIdState FinalMethod(const std::string &function,
                                            XmlNodeMap &FinalData,
                                            XmlNodeMap &SendData,
                                            std::string& message);
    /**
     * Sends data request to the server.
     *
     * @param function Function to execute (Read, Write, ...).
     * @param message  The message to send to the server.
     * @param dataNode Node containing the data to send to server.
     * @param table    Table to add results of operation.
     * @param IsFinal  If the request is the last to be sent.
     * 
     * @return Status of operation
     *         BEP_STATUS_SUCESS if operation performed successfully
     *         BEP_STATUS_ERROR if operation failed
     */
    const INT32 SendToServer(const std::string &function, std::string& message, XmlNode* &dataNode,
                             XmlNodeMap &table, const bool IsFinal = true);
    /**
     * Sends request to Named Data Broker to retreive, vaildate, or
     * invalidate the server id of the tag supplied
     *
     * @param function What function to perform (Read or Write)
     * @param tag      Tag to get server that supplies data
     * @param value    Value of command
     * @param message  Response from the NamedDataBroker
     * @param IsFinal  Last request (send to NDB)
     * @return Status of operation
     *         BEP_STATUS_SUCESS if operation performed successfully
     *         BEP_STATUS_ERROR if operation failed
     * @since Version 2.1
     */
    const INT32 SendToNdb(std::string function, const std::string &tag,
                          const std::string &value, std::string &message,
                          const bool IsFinal);
    /**
     * Get the name of the process subscribing for data
     *
     * @return Name of process that is subscribing for data
     * @since Version 2.5
     */
    const std::string GetSubscriberName(void);

    /**
     * Set the name of the process subscribing for data
     *
     * @param name name of process that is subscribing for data
     * @since Version 2.5
     */
    void SetSubscriberName(const std::string &name);

protected:
    /**
     * Data type definition for map of communication objects
     * @since Version 2.4
     */
    typedef map< std::string, IBepCommunication*, less<std::string> > CommObjMap;
    /**
     * Data type definition for interator for map of communication objects
     * @since Version 2.4
     */
    typedef CommObjMap::iterator CommObjMapIter;
    /**
     * Data type definition for communication map element
     * @since Version 2.4
     */
    typedef CommObjMap::value_type CommObjMapValue;
    /**
     * Container of objects with open channels to servers
     * @since Version 2.4
     */
    CommObjMap m_CommObjs;
    /**
     * Container for data to be re-verified or invalidated
     * @since Version 2.0
     */
    XmlNodeMap  m_badData;
    /**
     * Container for data to be sent to the server
     * @since Version 2.0
     */
    XmlNodeMap  m_testData;
    /**
     * Container for data to be sent to the server
     * @since Version 2.0
     */
    XmlNodeMap  m_finalData;
    /**
     * Object pointer used to communicate between INdb and servers
     * @since Version 2.4
     */
    IBepCommunication *m_tempComm;
    /**
     * Name of process requesting information
     * @since Version 2.5
     */
    std::string m_name;

    /**
     * Mutex used to lock our StateMachine processing
     */
    pthread_mutexattr_t m_lockAttr;
    pthread_mutex_t     m_stateLock;
    //BepMutex  m_stateLock;
};
#endif // INamedDataBroker_h
