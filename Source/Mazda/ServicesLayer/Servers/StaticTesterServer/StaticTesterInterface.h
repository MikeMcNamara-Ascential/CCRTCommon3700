//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/StaticTesterServer/StaticTesterInterface.h 1     10/26/06 1:21p Bbarrett $
//
// FILE DESCRIPTION:
//   The StaticTesterInterface handles the data comms for the static tester.  
//   Build data will be retrieved for the static tester when requested.  Test
//   result data will also be saved and stored from the static tester.
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
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/StaticTesterServer/StaticTesterInterface.h $
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 2     10/16/06 3:08p Derickso
// Updated reconnect logic and added keep alive/heartbeat messages.
// 
// 1     8/30/06 4:42p Derickso
// New static tester server.
// 
//===========================================================================
#ifndef StaticTesterInterface_h
#define StaticTesterInterface_h
//===========================================================================
#include "AtomicThread.h"
#include "BepServer.h"
#include "SerialChannel.h"
#include "StaticTesterServer.h"
#include <time.h>
//===========================================================================

class StaticTesterServer;


class StaticTesterInterface : public AtomicThread
{
public:
    /**
     * Class Constructor.
     * The pointer to the server that created this object will be stored.
     * 
     * @param server Pointer to the server that created this object.
     */
    StaticTesterInterface(StaticTesterServer *server);
    /**
     * Class destructor.
     */
    virtual ~StaticTesterInterface();
    /**
     * Initializes the object.
     * The port to connect to will be stored as well as any other data needed to run properly
     * or properly setup the object.
     * 
     * @param config Configuration data to use to initialize the object.
     */
    void Initialize(const XmlNode *config);

protected:
    /**
     * Funciton to be executed when thread is run
     *
     * @return Status
     */
    virtual void * Execute(void);
    /**
     * Check the communication port for any message or request from the static tester.
     * The data will then be retrieved from the port and processed according to the type
     * of message it is.
     * If no communication is received from the static tester for a period of time, the
     * object will be set to an inactive state.
     * 
     * @return Status of the communication check.
     */
    const BEP_STATUS_TYPE GetStaticTesterRequest();
    /**
     * Check if comms with the static tester are still valid.
     * The current time will be compared against the time of the last comms with
     * the static tester.
     * 
     * @return Flag indicating if comms with the static tester have expired.
     *         True - comms have expired, status whould be set to inactive.
     *         False - comms are still valid.
     */
    const bool CheckCommsExpired(void);
    /**
     * Get vehicle build data for the supplied VIN.
     * The build data will then be sent back to the static tester.
     * 
     * @param vin    VIN to retrieve build data for.
     */
    void RequestBuildData(const string &vin);
    /**
     * Store the test results for use by the test components.
     * The test results will be parsed out of the string and placed in XML format.
     * The XML results will then be sent to the server for inclusion in a data file
     * for management.
     * 
     * @param vin    VIN of the test results.
     * @param testResults
     *               String from static tester containing test results.
     */
    void StoreTestResults(const string &vin, const string &testResults);
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
     * Send a keep alive message to the static tester.
     * If the state of the object is active, a keep alive message will be sent.
     * If the object is in an inactive state, no keep alive message will be sent.
     * 
     * @return Status of the keep alive message.
     *         If there is no response from the static tester, the object will be set to an inactive state.
     */
    const BEP_STATUS_TYPE SendKeepAlive();
    /**
     * Reconnect the port.
     * If the port is not active, the reconnect will not occur.
     * 
     * @return Status of the reconnect.
     */
    const BEP_STATUS_TYPE ReconnectPort();
    /**
     * Determine if it is time to attempt to reconnect to the static tester.
     * If the static tester becomes disconnected and will not reconnect, the software will back off and check periodically
     * for the static tester to come back online.
     * 
     * @return Flag indicating if it is time to attempt a reconnect to the static tester.
     *         True  - Attempt to reconnect to the static tester.
     *         False - It is not time to attempt to reconnect to the static tester.
     */
    const bool AttemptReconnect();
    /**
     * Set or Check the active flag for the static tester this object is attached to.
     * If the function argument is NULL, the current value of the active flag will be returned.
     * If the argument is supplied, the active state of the object will be updated.
     * 
     * @param active Flag indicating if the object has active comms with a static tester.
     * 
     * @return Flag indicating if the object has active comms with a static tester.
     */
    const bool& StaticTesterActive(const bool *active = NULL);
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
     * Store the time of the last comm to the static tester.
     */
    void SetLastCommTime(void);
    /**
     * Get the time of the last communication with the static tester.
     * 
     * @return Time of the last message between the CCRT system and the static tester.
     */
    const time_t& GetLastCommTime(void);
    /**
     * Store the time of the last reconnect attempt to the static tester.
     */
    void SetLastConnectAttemptTime(void);
    /**
     * Get the time of the last reconnect attempt to the static tester.
     * 
     * @return Time of the last reconnect attempt to the static tester.
     */
    const time_t& GetLastConnectAttemptTime(void);
    /**
     * Store or get the time span for comms remain valid.
     * If the argument is supplied, the valid comm time span will be set; otherwise,
     * the current comm time span will be returned.
     * 
     * @param validTime Allowed time span between good comms to the static tester.
     * 
     * @return Allowed time span between good comms
     */
    const INT32& CommValidTime(const INT32 *validTime = NULL);
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
     * Store/Retrieve the length of time (ms) to wait for the static tester to resond/request.
     * If the argument is provided, the data will be stored; otherwise, the current value will be returned.
     * 
     * @param responseTime
     *               Amount of time (ms) to wait for the static tester to response/request.
     * 
     * @return Amount of time (ms) to wait for the static tester to response/request.
     */
    const INT32& StaticTesterResponseTime(const INT32 *responseTime = NULL);
    /**
     * Store/Retrieve the delimiter for fields in the messages being exchanged between the static tester and CCRT.
     * If the function argument is NULL, the current value will be returned; otherwise the supplied value will also 
     * be stored.
     * 
     * @param delim  Characters to use as field delimiters.
     * 
     * @return Characters to use as field delimiters.
     */
    const string& FieldDelimiter(const string *delim = NULL);
    /**
     * Store/Retrieve the delimiter for tag/value pairs in the messages being exchanged between the static tester and CCRT.
     * If the function argument is NULL, the current value will be returned; otherwise the supplied value will also 
     * be stored.
     * 
     * @param delim  Characters to use as tag/value pair delimiters.
     * 
     * @return Characters to use as tag/value pair delimiters.
     */
    const string& TagValueDelim(const string *delim = NULL);
    /**
     * Translate the given tag into a known test result tag.
     * The static tester may not use the same test result tag names that the CCRT test components use or the same
     * detail tag names.  This function provides a way to map the static test result data tags to the tags the
     * CCRT system requires and uses.  The DataTags from the server config file will be used to provide the
     * translation.  If the tag is not specified in the DataTags node, the original tag from the static tester
     * will be used.
     *
     * @param tag  Static tester data tag to translate to CCRT equivalent.
     * 
     * @return  CCRT equivalent tag to the static tester tag provided.
     */
    const string TranslateTestResultTag(const char *tag);
    /**
     * Get/Set the use keep alive flag to indicate if keep alive messages should be sent to the static tester.
     * 
     * @param useKeepAlive
     *               Flag to indicate if keep alive messages should be sent to the static tester.
     *               If this parameter is NULL, only the currently stored value will be returned; otherwise, the supplied value will
     *               also be stored.
     * 
     * @return Flag indicating if keep alive messages should be sent to the static tester.
     */
    const bool& UseKeepAlive(bool *useKeepAlive = NULL);
    /**
     * Store or get the time span to wait between checks for tester connected.
     * If the argument is supplied, the valid comm time span will be set; otherwise,
     * the current comm time span will be returned.
     * 
     * @param validTime Time span between checks for disconnected static testers.
     * 
     * @return Allowed time span between checks for disconnected static testers.
     */
    const INT32& DisconnectedTesterCheckTime(INT32 *checkTime = NULL);
    /**
     * Flag indicating if the object has active communications from the static tester.
     */
    bool    m_isActive;
    /**
     * Flag to indicate if keep alive messages should be sent to the static tester.
     */
    bool    m_useKeepAlive;
    /**
     * Time delay for reconnecting the port.
     */
    INT32   m_reconnectDelay;
    /**
     * Number of times to attempt to reconnect the port before disabling the port.
     */
    INT32   m_reconnectAttempts;
    /**
     * Time span between reconnect attempts to the static tester.
     */
    INT32   m_disconnectedTesterCheckTimeSpan;
    /**
     * Time span between comms to static tester in order to remain active.
     */
    INT32   m_validCommTimeSpan;
    /**
     * Amount of time (ms) to wait for port to register.
     */
    INT32   m_portRegisterDelay;
    /**
     * Amount of time (ms) to wait for static tester to make a request.
     */
    INT32   m_staticTesterResponseTime;
    /**
     * Comm Port this thread will attach to.
     */
    string  m_commPort;
    /**
     * Field delimiter for fields in the strings being transfered between the static tester and CCRT.
     */
    string  m_fieldDelim;
    /**
     * Delimiter for tag value pairs in the strings being transfered between the static tester and CCRT.
     */
    string  m_tagValueDelim;
    /**
     * Time of the last communication with the static tester.
     */
    time_t  m_lastCommTime;
    /**
     * Time of the last attempt to connect to the static tester.
     */
    time_t  m_lastConnectAttemptTime;
    /**
     * Communication object to use for interfacing with the static tester.
     * 
     * @see SerialChannel
     */
    SerialChannel m_staticTesterComms;
    /**
     * Pointer to the server that created this object.
     * 
     * @see BepServer
     * @see StaticTesterServer
     */
    StaticTesterServer *m_server;
};

//==============================================================================
#endif  //StaticTesterInterface_h
