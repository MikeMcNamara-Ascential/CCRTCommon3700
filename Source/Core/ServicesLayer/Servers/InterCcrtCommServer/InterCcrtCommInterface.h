//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/Core/ServicesLayer/Servers/InterCcrtCommServer/InterCcrtCommInterface.h 1     10/26/06 1:21p Bbarrett $
//
// FILE DESCRIPTION:
//   The InterCcrtCommInterface handles the data comms between ccrt and ccrt / ccts systems. 
// 
//   Build data will be sent when requested and saved and stored when received.
//  
//   Test result data will be sent when requested and saved and stored when received.  
//
//   Heartbeat messages will be sent when required.
//
//===========================================================================
// Copyright (c) 2006- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//===========================================================================
#ifndef InterCcrtCommInterface_h
#define InterCcrtCommInterface_h
//===========================================================================
#include "AtomicThread.h"
#include "BepServer.h"
#include "SerialChannel.h"
#include "InterCcrtCommServer.h"
#include "GeneralUtilities.h"
#include <time.h>
//===========================================================================

class InterCcrtCommServer;

/**
* Enum for the message type.
*/
enum MessageType
{
    BOM,
    RESULT,
    ACK,
    REQUEST,
    TYPE_HEARTBEAT,
    NOT_SUPPORTED
};

#define START_BOM  "BOM"
#define END_BOM  "ENDBOM"
#define START_RESULT  "RESULT"
#define END_RESULT  "ENDRESULT"
#define START_ACK  "ACK"
#define END_ACK  "ENDACK"
#define START_REQUEST  "REQUEST"
#define END_REQUEST  "ENDREQUEST"
#define HEARTBEAT  "HEARTBEAT"
#define MESSAGE_TYPE_NOT_SUPPORTED  "MESSAGE_TYPE_NOT_SUPPORTED"

#define MAIN_SEPARARTOR  ";"
#define DETAIL_SEPARARTOR  ":"

#define MESSAGE_MIN_NUM_STRINGS  3
#define RESULT_MIN_NUM_STRINGS 4
#define REQUEST_NUM_STRINGS  4
#define BOM_MIN_NUM_STRINGS  3
#define KEEP_ALIVE_TIME  250

#define NUMBER_OF_RESULT_DETAIL_FIELDS  6
#define NUMBER_OF_BOM_DETAIL_FIELDS  2

#define PART_ID_INDEX 1

//test result detail fields

#define TEST_RESULT_NAME_INDEX 0
#define TEST_RESULT_RESULT_INDEX 1
#define TEST_RESULT_MIN_INDEX 2
#define TEST_RESULT_MAX_INDEX 3
#define TEST_RESULT_VALUE_INDEX 4
#define TEST_RESULT_UNITS_INDEX 5

class CcrtProtocolMessage
{
public:
    CcrtProtocolMessage()
    {

    }
    ~CcrtProtocolMessage()
    {
        m_mainSeparatedValues.clear();
    }
    void SetMainSeparatedValues(string input)
    {//split input into strings separated by MAIN_SEPARARTOR
        SplitString(input, m_mainSeparatedValues, MAIN_SEPARARTOR);
    }
    vector<string> GetDetailSeparatedValues(string input)
    {//split input into strings separated by DETAIL_SEPARARTOR

        vector<string> details;
        SplitString(input, details, DETAIL_SEPARARTOR, false);
        return details;
    }
    MessageType m_messageType;
    vector<string> m_mainSeparatedValues;
    string m_partIdentifier;
};


class InterCcrtCommInterface : public AtomicThread
{
public:
    /**
     * Class Constructor.
     * The pointer to the server that created this object will be stored.
     * 
     * @param server Pointer to the server that created this object.
     */
    InterCcrtCommInterface(InterCcrtCommServer *server);
    /**
     * Class destructor.
     */
    virtual ~InterCcrtCommInterface();
    /**
     * Initializes the object.
     * The port to connect to will be stored as well as any other data needed to run properly
     * or properly setup the object.
     * 
     * @param config Configuration data to use to initialize the object.
     */
    void Initialize(const XmlNode *config);
    /**
     * Send a keep alive message to the endpoint.
     * If the state of the object is active, a keep alive message will be sent.
     * If the object is in an inactive state, no keep alive message will be sent.
     * 
     * @return Status of the keep alive message.
     *         If there is no response from the endpoint, the object will be set to an inactive state.
     */
    const BEP_STATUS_TYPE SendKeepAlive();
    const BEP_STATUS_TYPE SendBOM(string partID, string bom);
    const BEP_STATUS_TYPE SendTestResultRequest(string partID);
    const BEP_STATUS_TYPE BuildDataToString(XmlNode inputBuild, string &message);
const BEP_STATUS_TYPE SendMessage(string message);
    const bool& IsResultConnection(bool *isResultConnection = NULL);
    const bool& IsBomConnection(bool *isBomConnection = NULL);

protected:
    /**
     * Funciton to be executed when thread is run
     *
     * @return Status
     */
    virtual void * Execute(void);

    /**
     * Check the communication port for any message or request.
     * The data will then be retrieved from the port and processed according to the type
     * of message it is.
     * If no communication is received for a period of time, the
     * object will be set to an inactive state.
     * 
     * @return Status of the communication check.
     */
    const BEP_STATUS_TYPE WaitForIncomingData();

    /**
     * Check the communication port for any message or request.
     * The data will then be retrieved from the port and processed according to the type
     * of message it is.
     * If no communication is received for a period of time, the
     * object will be set to an inactive state.
     * 
     * @return Status of the communication check.
     */
    const BEP_STATUS_TYPE GetMessagesFromRaw(INT32 bytesRead, SerialString_t request, vector<string> &fullMessages);

    /**
     * Get vehicle build data for the supplied VIN.
     * The build data will then be sent back to the endpoint.
     * 
     * @param vin    VIN to retrieve build data for.
     */
    const BEP_STATUS_TYPE RequestBuildData(string &partId, string &message);
    /**
     * Store the test results for use by the test components.
     * The test results will be parsed out of the string and placed in XML format.
     * The XML results will then be sent to the server for inclusion in a data file
     * for management.
     * 
     * @param vin    VIN of the test results.
     * @param testResults
     *               String from endpoint containing test results.
     */
    //void StoreTestResults(const string &vin, const string &testResults);
    void StoreTestResults(string partId, vector<string> msgDetails);
    /**
     * Process the result data in the specified field.
     * The test result name, test result and any details will be extracted and placed
     * into an XmlNode and returned to the calling function.
     * 
     * @param resultField
     *                   Test result field to parse for test result data.
     * 
     * @return XmlNode containing the test result data that was parsed out of the result field.
     */
	XmlNode* ProcessResultField(string &resultField);

private:
    /**
     * Reconnect the port.
     * If the port is not active, the reconnect will not occur.
     * 
     * @return Status of the reconnect.
     */
    const BEP_STATUS_TYPE ReconnectPort();
    /**
     * Set or Check the active flag for the endpoint this object is attached to.
     * If the function argument is NULL, the current value of the active flag will be returned.
     * If the argument is supplied, the active state of the object will be updated.
     * 
     * @param active Flag indicating if the object has active comms with a endpoint.
     * 
     * @return Flag indicating if the object has active comms with a endpoint.
     */
    bool InterCcrtCommActive(const bool *active = NULL);
    /**
     * Store or get the amount of time to delay after reconnecting the port.
     * If the argument is NULL, the current time delay will be returned.
     * If the argument is specified, the reconnect delay will be updated.
     * 
     * @param reconnectDelay
     *               Amount of time to delay after reconnecting the port.
     * 
     * @return Amount of time to delay when reconnecting the port.
     */
    const INT32& ReconnectDelay(const INT32 *reconnectDelay = NULL);
    /**
     * Store or get the number of attempts to reconnect the port.
     * If the argument is NULL, the current attempts will be returned.
     * If the argument is specified, the attempts will be updated.
     * 
     * @param attempts
     *               Number of times to attempt to reconnect the port.
     * 
     * @return Number of times to attempt to reconnect the port.
     */
    const INT32& ReconnectAttempts(const INT32 *attempts = NULL);
    /**
     * Wait for the comm port to become active before attempting to connect.
     */
    void WaitForCommPort(void);
    /**
     * Store/Get the comm port this thread is connecting to.
     * If the parameter is NULL, the currently stored comm port will be retrieved; otherwise, the com port supplied
     * will be stored.
     * 
     * @param commPort Comm Port this thread is attached to.
     * 
     * @return Comm Port this thread is attached to.
     */
    const string& CommPortUsed(const string *commPort = NULL);
    /**
     * Store/Retrieve the amount of time to wait for the port driver to register.
     * If the argument to the function is not supplied, the port register time will be returned; 
     * otherwise, the new port register time will be stored.
     * 
     * @param registerDelay
     *               Amount of time (ms) to wait for the port driver to register.
     * 
     * @return Amount of time (ms) to wait for the port driver to register.
     */
    const INT32& PortRegisterDelay(const INT32 *registerDelay = NULL);
    /**
     * Store/Retrieve the length of time (ms) to wait for the endpoint to resond/request.
     * If the argument is provided, the data will be stored; otherwise, the current value will be returned.
     * 
     * @param responseTime
     *               Amount of time (ms) to wait for the endpoint to response/request.
     * 
     * @return Amount of time (ms) to wait for the endpoint to response/request.
     */
    const INT32& InterCcrtCommResponseTime(const INT32 *responseTime = NULL);
    /**
     * Translate the given tag into a known test result tag.
     * The endpoint may not use the same test result tag names that the CCRT test components use or the same
     * detail tag names.  This function provides a way to map the test result data tags to the tags the
     * CCRT system requires and uses.  The DataTags from the server config file will be used to provide the
     * translation.  If the tag is not specified in the DataTags node, the original tag from the endpoint
     * will be used.
     *
     * @param tag  endpoint data tag to translate to CCRT equivalent.
     * 
     * @return  CCRT equivalent tag to the endpoint tag provided.
     */
    const string TranslateTestResultTag(string tag);
    /**
     * Get/Set the use keep alive flag to indicate if keep alive messages should be sent to the endpoint.
     * 
     * @param useKeepAlive
     *               Flag to indicate if keep alive messages should be sent to the endpoint.
     *               If this parameter is NULL, only the currently stored value will be returned; otherwise, the supplied value will
     *               also be stored.
     * 
     * @return Flag indicating if keep alive messages should be sent to the endpoint.
     */
    const bool& UseKeepAlive(bool *useKeepAlive = NULL);



const BEP_STATUS_TYPE GetFullMessageFromString(string &testerRequest, string &returnMessage);
const BEP_STATUS_TYPE ProcessMessagesFromRaw(string testerRequest);
const BEP_STATUS_TYPE ConvertFullMessageToCcrtProtocolMessage(string fullMessage, CcrtProtocolMessage &ccrtMsg);
const BEP_STATUS_TYPE ProcessAck(CcrtProtocolMessage ccrtMsg);
const BEP_STATUS_TYPE ProcessHeartbeat(CcrtProtocolMessage ccrtMsg);
const BEP_STATUS_TYPE ProcessRequest(CcrtProtocolMessage ccrtMsg);
const BEP_STATUS_TYPE ProcessBom(CcrtProtocolMessage ccrtMsg);
const BEP_STATUS_TYPE ProcessResult(CcrtProtocolMessage ccrtMsg);
const BEP_STATUS_TYPE CreateAckMsg(MessageType msgType, string &message);
const BEP_STATUS_TYPE CreateRequestMsg(string partID, MessageType msgType, string &message);
const BEP_STATUS_TYPE CreateBomMsg(string partID, string &message);
const BEP_STATUS_TYPE CreateResultMsg(string partID, string &message);
const string GetMessageEndIDFromStartID(string startID);
size_t FindNextStartTag(string &testerRequest);
MessageType StringToMessageType(string startID);
string MessageTypeToString(MessageType msgType);
XmlNode GetResultDetailNode(vector<string> &resultItem);

    /**
     * Flag indicating if the object has active communications from the endpoint.
     */
    bool    m_isActive;
    /**
     * Flag to indicate if keep alive messages should be sent to the endpoint.
     */
    bool    m_useKeepAlive;
    bool    m_isResultConnection;
    bool    m_isBomConnection;
    /**
     * Time delay for reconnecting the port.
     */
    INT32   m_reconnectDelay;
    /**
     * Number of times to attempt to reconnect the port before disabling the port.
     */
    INT32   m_reconnectAttempts;
    /**
     * Amount of time (ms) to wait for port to register.
     */
    INT32   m_portRegisterDelay;
    /**
     * Amount of time (ms) to wait for endpoint to make a request.
     */
    INT32   m_interCcrtCommResponseTime;
    /**
     * Comm Port this thread will attach to.
     */
    string  m_commPort;
    /**
     * Communication object to use for interfacing with the end point.
     * 
     * @see SerialChannel
     */
    SerialChannel m_interCcrtComm;
    /**
     * Pointer to the server that created this object.
     * 
     * @see BepServer
     * @see InterCcrtCommServer
     */
    InterCcrtCommServer *m_server;
    /**
     * message data not used during port receive data event
     * 
     * @see BepServer
     * @see InterCcrtCommServer
     */
    string m_partialMessageBuffer;
    /**
     * Mutex used to control port access
     */
    BepMutex m_commPortMutex;
    /**
     * Mutex used to control access to flag indicating if 
     * communications port is active
     */
    BepMutex m_commsActiveMutex;
    /**
     * Timer for sending keep alive messages.
     */
    BepTimer m_keepAliveTimer;
};

//==============================================================================
#endif  //InterCcrtCommInterface_h
