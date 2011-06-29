//******************************************************************************
//
// FILE:    ITestResultServer
//     $Archive::  $
//     $Author: Derickso $
//     $Date: 11/02/07 12:10p $
//
// Description:
//  Extend IInfoCommunication to support reporting of test results
//
//==============================================================================
//
//     Copyright © 2001 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Include/ITestResultServer.h $
// 
// 1     11/02/07 12:10p Derickso
// Added ReportDTC function that takes an XmlNode as an argument.
// 
// 5     10/30/07 3:45p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     10/26/06 9:19a Rwiersem
// Changes for the 06102701 core release:
// 
// Made WriteTestResult() virtual.
// 
// 3     9/05/06 4:02p Rwiersem
// Changes for the 06090601 core release:
// 
// - Made WriteTestResult virtual
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 2     10/24/03 11:48a Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 2     10/10/03 9:25a Khykin
// Updated with latest Flatrock changes.
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
// 1     6/12/03 4:15p Khykin
// Initial checkin.
//
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:40  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:06a Khykin
// Initial Checkin
//
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
//
// 17    8/23/02 8:20a Dan.erickson
// Added DTC reporting capabilities as well as allowing reporting of test
// failure codes to support DaimlerChrysler PFS reporting.
//
// 16    6/18/02 12:50p Dan.erickson
// Overloaded HandelRequest so Result attribute could be added
// to subscribe nodes.  This will keep the GUI updated properly.
//
// 15    6/07/02 12:00p Dan.erickson
// Added HandleRequest() method.
//
// 14    4/25/02 4:41p Skypig13
// Updated with new changes
//     Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//     started
//
//
// 12    3/20/02 11:04a Dan.erickson
// Moved definition of TEST_RESULT_SERVER_NAME to BepDefs.
//
// 11    3/12/02 11:55a Dan.erickson
// Restored SpecialProcessing method.
//
// 10    3/05/02 5:03p Dan.erickson
// Removed SpecialProcessing method.  No longer needed now that Xml nodes
// are better supported.
//
// 9     2/26/02 2:49p Dan.erickson
// Updated to take advantage of XmlNode type data.
//
// 8     2/18/02 7:27p Dan.erickson
// Updated after rework of BepServer.
//
// 7     2/18/02 10:33a Skypig13
// Thread Pool class for TestResultServer
//
// 6     12/03/01 12:23p Dan.erickson
// Added macro for TestResultServer name.
//
// 5     11/26/01 3:40p Dan.erickson
// Moved AddDetail and AddDetailUnits methods to IInfoCommunication.
//
// 4     11/14/01 3:49p Dan.erickson
// Added methods to get test result information.
// Debugged and tested.
//
// 3     11/09/01 2:15p Dan.erickson
// Added Initialize, Read, WriteTestResult, AddDetail, and
// AddDetailUnits methods.
//
// 2     11/06/01 10:28a Dan.erickson
// Removed all IInfoCommunication methods.
//
// 1     11/06/01 9:50a Dan.erickson
// Created.
//
//
//******************************************************************************

#ifndef ITestResultServer_h
#define ITestResultServer_h

#include "IInfoCommunication.h"
#include "BepComponent.h"
#include <list>

class ITestResultServerTest;

/**
 * Test Result detail data type
 */
typedef struct
{
    string  detailTag;
    string  detailValue;
    string  detailUnits;

} TestResultDetail_t;

/**
 * Object used to hold a list of Test Result details
 */
typedef std::list< TestResultDetail_t>      TestResultDetails_t;
typedef TestResultDetails_t::iterator       TestResultDetailsItr_t;
typedef TestResultDetails_t::const_iterator TestResultDetailsCItr_t;

/**
 * Object used to hold a list of Test Result details
 */
class TestResultDetails : public TestResultDetails_t
{
public:
    TestResultDetails();

    ~TestResultDetails();

    /**
     * Add a new Test Result detail to the list.
     *
     * @param tag   The detail name or tag.
     * @param value The detail value.
     * @param units The detail units. 
     */
    void AddDetail( const std::string &tag, const std::string &value, const std::string &units="");


    /**
     * Get the first detail from the list (without removing it)
     *
     * @param firstDetail
     *               Where to put the Test Result detail read from the list
     *
     * @return Iterator to be passed to subsequent calls to GetNextDetail()
     */
    TestResultDetailsCItr_t GetFirstDetail( TestResultDetail_t &firstDetail) const;

    /**
     * Get the next detail from the list (without removing it)
     *
     * @param nextDetail Where to put the next Test Result detail read from the list
     * @param itr        Iterator used to mark the last entry read from the list.
     *
     * @return Iterator to be passed to the next call to GetNextDetail()
     */
    TestResultDetailsCItr_t GetNextDetail( TestResultDetail_t &nextDetail, TestResultDetailsCItr_t itr) const;

};

/**
 * Class to handle the communications interface between the
 * TestResultServer and anything that wants to communicate
 * with it.
 *
 * @author Dan Erickson
 * @version Version 1.5
 * @since Version 1.0
 */
class ITestResultServer : public IInfoCommunication
{
    friend class ITestResultServerTest;
public:
    /**
     * Default constructor
     * @since Version 1.0
     */
    ITestResultServer(void);
    /**
     * Destructor
     * @since Version 1.0
     */
    virtual ~ITestResultServer(void);
    /**
     * Create a worker object that will get requests from
     * a channel already open and with message(s) waiting
     *
     * @param creator Object that opened channel to connect to
     * @param id     The id of open channel to talk on.
     * @since Version 1.1
     */
    void Initialize(IBepCommunication &creator, INT32 id = -1);
    /**
     * Setup the communication object.
     *
     * @param server  Name of the server to connect to.
     * @param type    Type of objec to create (Client/Server)
     * @param debug   Turn on/off diagnostic messages.
     * @param size    Buffer size
     * @param timeout Time allowed to perform commuinications.
     * @since Version 1.1
     */
    void Initialize(const std::string &server,
                    const std::string &type,
                    const bool debug    = false,
                    const INT32 size    = DEFAULT_BUFFER_SIZE,
                    const INT32 timeout = DEFAULT_TIMEOUT);
    /**
     * Set up the communincation object.
     *
     * @param config Configuration to use
     * @param id     Channel id to talk on.
     * @param rid    Channel to receive on.
     * @return
     * @since Version 1.1
     */
    void Initialize(const XmlNode *config, const INT32 id = -1, const INT32 rid = -1);
    /**
     * Set the result attribute for the given node to be written.
     *
     * @param result    Value to set for the test result.
     * @param existNode Existing XmlNode to add/update the result attribute for
     *
     * @return The given node (existNode) with the attribute added.
     * @since Version 1.0
     */
    XmlNode * SetResult(const std::string &result, XmlNode *existNode);
    /**
     * Set the test result code attribute for the given node to be written.
     *
     * @param code      Code associated with the test result.  Used mainly to specify a failure code.
     * @param existNode Existing XmlNode to add/update the result code attribute for
     *
     * @return The given node (existNode) with the attribute added.
     * @since Version 1.1
     */
    XmlNode * SetCode(const std::string &code, XmlNode* existNode);
    /**
     * Method to write a test result to the test result server.
     * The result and description will be added as attributes
     * if they are not already present.
     *
     * @param test   Name of the test.
     * @param result Test Result.
     * @param process Name of the test process that generated the result.
     * @param description
     *                Reason for the abort, fail, or pass.
     * @param message Message returned from server.
     * @param isFinal Is the message complete.
     * @param code    The failure code.
     * 
     * @return Status of the write operation.
     */
    virtual INT32 WriteTestResult(const std::string test, 
                                  const std::string result, 
                                  const std::string process,
                                  const std::string description, 
                                  std::string &message, 
                                  const bool isFinal = false,
                                  const std::string code = "0000");
    /**
     * Method to write a test result to the test result server with test result
     * details. The result, description and details will be added as attributes
     * if they are not already present.
     *
     * @param test    Name of the test.
     * @param result  Test Result.
     * @param process Name of the test process that generated the result.
     * @param description
     *                Reason for the abort, fail, or pass.
     * @param message Message returned from server.
     * @param details list of test result details
     * @param isFinal Is the message complete.
     * @param code
     *
     * @return Status of the write operation.
     * @since Version 1.1
     */
    virtual INT32 WriteTestResult(const std::string test, 
                                  const std::string result, 
                                  const std::string process,
                                  const std::string description, 
                                  std::string &message, 
                                  const TestResultDetails &details,
                                  const bool isFinal = false, 
                                  const std::string code = "0000");

    /**
     * Report a Diagnostic Trouble Code (DTC) that was detected in a vehicle module while
     * monitoring for faults.
     *
     * @param vehicleModule
     *                Vehicle module where the DTC was detected.
     * @param dtcName Name of the DTC.
     * @param dtcCode The code read from the module.
     * @param dtcDescription
     *                A description for the DTC.  For example, "Bad transmission range".
     * @return Result of the reporting operation.
     */
    INT32 ReportDTC(const std::string &vehicleModule,
                    const std::string &dtcName,
                    const std::string &dtcCode,
                    const std::string &dtcDescription);
    /**
     * Report a Diagnostic Trouble Code (DTC) that was detected in a vehicle module while
     * monitoring for faults.
     * 
     * @param vehicleModule
     *                Vehicle module where the DTC was detected.
     * @param dtcNode Fault node to report.
     * 
     * @return Result of the reporting operation.
     */
    INT32 ReportDTC(const std::string &vehicleModule, XmlNode *dtcNode);
    /**
     * Clear diagnostic trouble codes (DTC) from the report.
     *
     * @param vehicleModule
     *                Vehicle module to clear DTCs for.
     * @param dtcCode If specified, only this DTC will be cleared, otherwise, all DTCs for the specified module
     *                will be cleared.
     * @return Result of the clearing operation.
     */
    INT32 ClearDTC(const std::string &vehicleModule, const std::string &dtcCode = CLEAR_ALL_DTC);
    /**
     * Read a tag from the server.  Sets all the proper
     * attributes so the user does not have to.
     *
     * @param node   Tag to read from the server.
     * @param response Response from the server.
     * @param isFinal Is the list of tags to be read complete.
     * @return Status of the read operation.
     */
    INT32 Read(const XmlNode *node, std::string &response, const bool isFinal = false);
    /**
     * Get the result attribute from the node specified.
     *
     * @param node   Node to get result from.
     * @return Value of the result attribute.
     * @since Version 1.1
     */
    std::string GetResult(const XmlNode *node);
    /**
     * Get the test result code attribute from the node specified.
     *
     * @param node   Node to get result from.
     * @return Value of the test result code attribute.
     * @since Version 1.1
     */
    std::string GetCode(const XmlNode *node);
    /**
     * Get the next test result from the list of results.
     * NOTE: The node for the test result is removed from the
     * list and returned in testResult.
     *
     * @param testName Name of the test the result is for.
     * @param result Result of the test, Pass, Fail, Abort, etc.
     * @param testResult The test result node.
     * @return The number of results remaining in the list.
     * @since Version 1.0
     */
    INT32 GetNextTestResult(std::string &testName, std::string &result, XmlNode **testResult);
    /**
     * Get the description attribute from the test result.
     *
     * @param testName Name of the test result to get the description from.
     * @param testResult The test result node.
     * @return The description attribute for the test result.
     * @since Version 1.0
     */
    std::string GetTestDescription(const std::string &testName, XmlNode **testResult = NULL);
    /**
     * Get the process attribute from the test result.
     *
     * @param testName Name of the test result to get the process from.
     * @param testResult The test result node.
     * @return The process attribute for the test result.
     * @since Version 1.0
     */
    std::string GetTestProcess(const std::string &testName, XmlNode **testResult = NULL);
    /**
     * Get the result attribute from the test result.
     *
     * @param testName Name of the test result to get the result from.
     * @param testResult The test result node.
     * @return The result attribute for the test result.
     * @since Version 1.0
     */
    std::string GetTestResult(const std::string &testName, XmlNode **testResult = NULL);
    /**
     * Get the test result code attribute from the test result.
     *
     * @param testName Name of the test result to get the result from.
     * @param testResult The test result node.
     * @return The test result code attribute for the test result.
     * @since Version 1.0
     */
    std::string GetTestResultCode(const std::string &testName, XmlNode **testResult = NULL);
    /**
     * Get the next detail node from the test result node.
     * NOTE:  The detail node is removed from the list.
     *
     * @param testName Name of the test.
     * @param detailName Name of the detail node.
     * @param value  Value of the detail node.
     * @param resultDetail
     *                   The detail node.
     * @param testResult The test result.
     * @return The number of detail nodes remaining in the list.
     * @since Version 1.0
     */
    INT32 GetNextTestResultDetail(const std::string &testName,
                                  std::string &detailName,
                                  std::string &value,
                                  XmlNode **resultDetail,
                                  XmlNode **testResult = NULL);
    /**
     * Get the units for the detail value.
     *
     * @param testName Name of the test.
     * @param detailName Name of the detail.
     * @param resultDetail
     *                   The detail node.
     * @param testResult The test result.
     * @return The units associated with the detail value.
     * @since Version 1.0
     */
    std::string GetTestResultDetailUnits(const std::string &testName,
                                         const std::string detailName,
                                         const XmlNode *resultDetail = NULL,
                                         XmlNode **testResult = NULL);
    /**
     * Method that will actually handle the request from the
     * client.  This method calls the servers methods to perform
     * the requested operations.  This is the implementation of
     * the Visitor pattern in this class.
     *
     * @param message Message to populate with data that was sent back to client
     * @param server  Server that will fulfill the request of the client.
     * @param messagePopulated
     *                Flag to specify if the message being passed in has already been populated.
     *                TRUE - do not get message from the client.
     *                FALSE - get the message from the client.
     * @return Status of operation
     *         BEP_STATUS_ERROR    == operation failed
     *         BEP_STATUS_SUCCESS  == sucess
     * @since Version 1.4
     */
    const INT32 HandleRequest(std::string &message,
                              BepComponent *server = NULL,
                              const bool &messagePopulated = false);
    /**
     * Special processing to be done to each node.
     * Process and description attributes are added to each node.
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
    virtual bool ValidateMessage(const XmlNode* doc);

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
     * Finds the test result node specified.
     *
     * @param testName Name of the test result node to find.
     * @param resultNode The test result node specified by testName or NULL if not
     *                   found.
     * @return False if the test result node does not exist.
     * @since Version 1.1
     */
    bool FindResultNode(const std::string &testName, XmlNode **resultNode);
};

#endif // ITestResultServer_h

