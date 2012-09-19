//******************************************************************************
// Description:
//   Host Interface base class to handle communications between the CCRT system
//   and a plant host system.
//
//===========================================================================
// Copyright (c) 2007- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/PlantHostOutbound/Include/HostInterface.h $
// 
// 1     11/01/07 3:22p Derickso
// Made LoadAdditionalConfigurationItems virtual so child classes could
// initialize properly.
// Updated strategy for processing test results.
// 
// 2     10/31/07 10:04a Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 1     5/01/07 1:04p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef HostInterface_h
#define HostInterface_h
//==============================================================================
#include "BepServer.h"
#include "SerialChannel.h"
#include "PlantHostOutboundDefs.h"
#include "INamedDataBroker.h"
#include "HostInterfaceKeepAlive.h"

class HostInterfaceKeepAlive;

/**
 * Base class for plant host system interfaces.
 * This class will receive and process a test result and generate a result 
 * string to be transmitted to the plant host system.  Once the string is 
 * generated, the flag for sending results to the host will be checked.  If the 
 * flag indicates the results should be transitted, the generated result string 
 * will be transmitted to the host system.
 * 
 * @author Dan Erickson
 * @see PlantHostOutbound
 * @see BepServer
 */
class HostInterface : public BepServer
{
    friend class HostInterfaceTest;
public:
    /**
     * Class constructor.
     */
    HostInterface();
    /**
     * Class destructor.
     */
    virtual ~HostInterface();
    /**
     * Initialize the host interface using the supplied command line arguments.
     * The command line argumets will be parsed and used for initializing the 
     * host interface.
     * 
     * @param argc   Count of the command line arguments.
     * @param argv Command line arguments to parse and use for initializing the 
     *             host interface.
     */
    virtual void Initialize(int argc, char *argv[]);
    /**
     * Initialization method that retrieves the xml document.
     * 
     * @param fileName Configuration file which contains the object setup
     *                 information.
     */
    virtual void Initialize(const string &fileName);
    /**
     * Method for initialization that is passed a parsed configuration file.
     *
     * @param document Configuration data for setting up the interface.
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * Main Run method to control the process
     *
     * @param terminateFlag Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
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
    virtual const string Write(const string &tag, const string &value);
    /**
     * <b>Description:</b>
     * Request to write a data tag.
     *
     * @param node The data tag and new value to be written.
     * 
     * @return BEP_SUCCESS_RESPONSE if the new value was written successfully.
     *         BEP_ERROR_RESPONSE if the data tag was found, but the tag data
     *         mutex could not be locked.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was found and the tag 
     *         data mutex was locked, but the data could not be written.
     *         BEP_UNSUPPORTED_RESPONSE if the data tag could not be found.
     */
    virtual const string Write(const XmlNode *node);

protected:
    /**
     * Store additional items from the config file to complete the 
     * initialization of the host interface.
     * 
     * @param config Configuration data used to complete the initialization of 
     *               the host interface.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *config);
    /**
     * Parse the command line options.
     * Data from the command line options will be used for initializing the 
     * host interface object
     * 
     * @param argc   Number of command line arguments to process.
     * @param argv   Command line arguments to process.
     */
    void ParseCommandLine(int argc, char *argv[]);
    /**
     * Process the test results that were received.
     * NOTE:  The error code node should already be included in the results 
     *        from the PlantHostOutbound processing.
     */
    void ProcessTestResults(void);
    /**
     * Process the test result node, creating and sending the appropriate 
     * messages to plant host.
     * 
     * @param testResults pointer to the test result node copied from the file.
     */
    virtual void DoResultProcessing(const XmlNode *testResults);
    /**
     * Translate the test result from Xml to a string to send to broadcast.
     *
     * @param testResult Test results read from the test result file.
     * @param resultStringMap
     *                   Map specifying how to build the result string.
     * @param resultConversions
     *                   Map specifying test result conversions.  For example: 
     *                   Pass --> P.
     * @param useVehicleBuild
     *                   True - Use information from the vehicle build block of 
     *                          the test result.
     *                   False - Do not use information from the vehicle build 
     *                           block, or the test results do not contain a 
     *                           vehicle build block.
     * @param allowSpaceInResult
     *                   True - Allow spaces in the result fields.
     *                   False - Spaces in each field will be translated to 
     *                           underscores.
     * @param totalSerialNumbers
     *                   Count of serial numbers being reported in the test 
     *                   result string.
     * 
     * @return Result string to send to external host.
     */
    const string BuildTestResultString(const XmlNode *testResult,
                                       const XmlNodeMap &resultStringMap,
                                       const XmlNodeMap &resultConversions,
                                       INT32 &totalSerialNumbers,
                                       const bool &useVehicleBuild = true,
                                       const bool &allowSpaceInResult = true);
    /**
     * Process the node from the map specifying how to build the result string.
     *
     * @param resultMapNode Result string field to build.
     * @param testResults
     *                The reported test results.
     * @param vehicleData
     *                The vehicle build information from the tested vehicle.
     * @param resultConversions  The map specifying test result conversion. 
     * @param totalSerialNumbers
     *                The total number of Serial numbers reported in the test 
     *                results.
     * @return The data to place in the current result string field.
     */
    virtual const string ProcessMapNode(const XmlNode *resultMapNode,
                                const XmlNodeMap &testResults,
                                const XmlNodeMap &vehicleData,
                                const XmlNodeMap &resultConversions,
                                INT32 &totalSerialNumbers);
    /**
     * Process the DTC node from the map file.
     * The DTC node will be passed in from the test result file and iterated 
     * through.  The DTC format is contained in the map file and will be 
     * applied to all reported DTCs up to the maximum number of DTCs to be 
     * added to the string.  The string that is build will be returned to the 
     * calling function.
     * 
     * @param dtcFormat Format block to be used for creating the DTC result 
     *                  string.
     * @param testResults
     *                The reported test results.
     * @param vehicleData
     *                The vehicle build information from the tested vehicle.
     * @param resultConversions  The map specifying test result conversion. 
     * @param totalSerialNumbers
     *                The total number of Serial numbers reported in the test 
     *                results.
     * 
     * @return DTC string for reporting.
     */
    virtual string ProcessDtcNode(const XmlNode *dtcFormat, 
                                  const XmlNodeMap &testResults,
                                  const XmlNodeMap &vehicleData,
                                  const XmlNodeMap &resultConversions,
                                  INT32 &totalSerialNumbers);
    /**
     * Generate a parent node for the component that reported the DTC.
     * 
     * @param componentName
     *               Name of the component that reported the DTC.
     * @param testResults
     *               The reported test results.
     * @param vehicleData
     *               The vehicle build information from the tested vehicle.
     * @param resultConversions
     *               Result string conversion mapping.
     * @param totalSerialNumbers
     *               The total number of Serial numbers reported in the test results.
     * @param mapComponentName
     *               Flag indicating if the component name should be mapped to a different value.
     * 
     * @return Parent DTC node to be added to the test result string.
     */
    virtual string ProcessDtcParentNode(const string &componentName,
                                        const XmlNodeMap &testResults,
                                        const XmlNodeMap &vehicleData,
                                        const XmlNodeMap &resultConversions,
                                        INT32 &totalSerialNumbers,
                                        const bool &mapComponentName);
    /**
     * Map the provided component name to an alias.
     * The alias will be listed in the configuration file.
     * 
     * @param componentName Name of the component to be mapped.
     * 
     * @return Mapped name of the component.
     */
    string GetMappedComponentName(const string &componentName);
    /**
     * Converts a result string from Pass, Fail, Abort to a form acceptable by 
     * the plant host system.
     *
     * @param result Result to convert.
     * @param resultConversions
     *               Map of the valid conversions.
     * 
     * @return Plant host conversion value for Pass, Fail, or Abort
     */
    string ConvertResultString(const string &result, const XmlNodeMap &resultConversions);
    /**
     * Find the test result for the specified field.
     *
     * @param testResultMap
     *               The test results.
     * @param resultStringField
     *               The current field in the test result string being 
     *               processed.
     * @param resultConversions
     *               Test result conversion map.
     * 
     * @return The Pass/Fail/Abort result for the test.
     */
    string GetTestResult(const XmlNodeMap &testResultMap, 
                         const XmlNode *resultStringField,
                         const XmlNodeMap &resultConversions,
                         const bool *useConversion = NULL);

    /**
     * Find the test result code for the specified field.
     *
     * @param testResultMap
     *               The test results.
     * @param resultStringField
     *               The current field in the test result string being 
     *               processed.
     * 
     * @return The test result code for the test.
     */
    string GetTestResultCode(const XmlNodeMap &testResultMap, 
                         const XmlNode *resultStringField);

    string GetTestResultFromValue(string resultValue,
                                  string format,
                                    const XmlNodeMap &resultConversions,
                                    const bool *useConversion = NULL);

    /**
     * Get a value associated with a test result.
     *
     * @param testResultMap
     *                 Test result list.
     * @param testResultField
     *                 Specific test result we are looking for.
     * 
     * @return The test result value to add to the test result string.
     */
    string GetTestDetailValue(const XmlNodeMap &testResultMap, const XmlNode *testResultField);
    /**
     * Apply the specified data formatting to the passed in data.
     *
     * @param dataType Type of the data to format.  Valid data types are
     *                 typeInt, typeFloat, or typeString.
     * @param dataValue Data to format.
     * @param formatString String that specifies the "printf style" formatting.
     * @param fieldLength Field length specified in the configuration file
     * @param padChar Character to use for padding if necessary
     *
     * @return The resulting formatted data string.
     */
    virtual string ApplyDataFormatting(string dataType, 
                                       string dataValue,
                                       string formatString, 
                                       INT32 fieldLength,
                                       float scale,
                                       float offset,
                                       string padChar="0");
    /**
     * If necessary, pad or trim the passed in data value.
     *
     * @param dataType Type of the data to adjust.  Valid data types are
     *                 typeString, typeInt, or typeFloat.
     * @param dataValue Data to pad or trim if necessary.
     * @param length Field length
     * @param padChar Character to use for padding if necessary
     *
     * @return The resulting data string.
     */
    virtual string AdjustDataValueLength(string dataType, string dataValue, INT32 length, string padChar);
    /**
     * Get the format attribute from the specified node if it exists.  If the
     * attribute does not exist, default to an unformatted string (%s).
     *
     * @param resultMapNode Node that contains the format attribute
     *
     * @return the formattting string
     */
    virtual string GetFormatAttribute(const XmlNode* resultMapNode);
    /**
     * Get the data type attribute from the specified node if it exists.  If the
     * attribute does not exist, default to a string data type.
     *
     * @param resultMapNode Node that contains the data type attribute
     *
     * @return the data type
     */
    virtual string GetDataTypeAttribute(const XmlNode* resultMapNode);
    /**
     * Get the length attribute from the specified node if it exists.  If the
     * attribute does not exist, return 0 and use the length specified in
     * the format string.
     *
     * @param resultMapNode Node that contains the length attribute
     *
     * @return the length
     */
    virtual INT32 GetDataLengthAttribute(const XmlNode* resultMapNode);
    /**
     * Get the length attribute from the specified node if it exists.  If the
     * attribute does not exist, return 0 and use the length specified in
     * the format string.
     *
     * @param resultMapNode Node that contains the length attribute
     *
     * @return the length
     */
    virtual float GetScaleAttribute(const XmlNode* resultMapNode);
    /**
     * Get the length attribute from the specified node if it exists.  If the
     * attribute does not exist, return 0 and use the length specified in
     * the format string.
     *
     * @param resultMapNode Node that contains the length attribute
     *
     * @return the length
     */
    virtual float GetOffsetAttribute(const XmlNode* resultMapNode);
    /**
     * Get the length from the format string of an Integer or Float type.
     *
     * @param format Format string from configuration file
     *
     * @return length of value to report to host system
     */
    INT32 GetLengthFormatType(string format);
    /**
     * Set/Get the index of the VIN in the created result string.
     * In some instances, the VIN is used as part of the message header.
     * 
     * @param index  Index of the VIN in the created result string.  If this 
     *               parameter is NULL, only the current stored index will be 
     *               returned.
     * 
     * @return Index of the VIN in the created result string.
     */
    const INT32& VinIndex(INT32 *index = NULL);
    /**
     * Get/Set the VIN to use for the result string being generated for the 
     * plant host system.
     * 
     * @param vin    VIN to use for the plant host result string being generated.
     * 
     * @return VIN to use for the plant host result string being generated.
     */
    const string& MessageVin(string *vin = NULL);
    /**
     * Set/Get the maximum amount of time for the plant host system to respond.
     * 
     * @param maxResponseTime
     *               Maximum amount of time to wait for the plant host system 
     *               to respond.
     * 
     * @return Maximum amount of time to wait for the plant host system to 
     *         respond.
     */
    const INT32& MaxHostResponseTime(INT32 *maxResponseTime = NULL);
    /**
     * Set/Get the maximum amount of attempts to successfully transmit the test 
     * result string to the host system.
     * 
     * @param maxAttempts
     *               Maximum number of attempts to successfully transmit the 
     *               test result string to the host system.
     * 
     * @return Maximum number of attempts to successfully transmit the test 
     *         result string to the host system.
     */
    const INT32& MaxMsgSendAttempts(INT32 *maxAttempts = NULL);
    /**
     * Save/Retrieve the result base map.
     * If the parameter is NULL, just the base result string map will be 
     * returned.  Otherwise, the result base map will be stored.
     * 
     * @param baseMap Base result string map to use when building the result 
     *                string.
     * 
     * @return Base result string map to use when building the result string.
     */
    const XmlNodeMap& ResultMap(const XmlNode *baseMap = NULL);
    /**
     * Get the result conversion map.
     * 
     * @return Reference to the result conversion map.
     */
    const XmlNodeMap& ResultConversions(void);
    /**
     * Save the mapping of Pass/Fail/Abort to the format to use for test result 
     * reporting.
     *
     * @param resultStrings   Node containing the mapping.
     * @param resultStringMap Map to be populated.
     */
    void SetResultStrings(const XmlNode *resultStrings, XmlNodeMap &resultStringMap);
    /**
     * Get/Set the message Id to use for the transmitted message.
     * 
     * @param id Message Id used to identify the type of message to be sent to 
     *           the host system.
     * 
     * @return Message Id used to identify the type of message to be sent to the 
     *         host system.
     */
    const UINT8& MessageId(UINT8 *id = NULL);
    /**
     * Get/Set the host interface name being used for this host interface.
     * 
     * @param pathName Name to use for this host interface.
     * 
     * @return Name being used for this host interface.
     */
    const string& HostInterfaceName(const string *pathName = NULL);
    /**
     * Return a reference to the host communication object.
     * 
     * @return Reference to the comm object used to communicate with the host 
     *         system.
     */
    SerialChannel* HostComm(void);
    /**
     * Determine if the test results should be sent to the plant host system.
     *
     * @return True  - Send results to plant host system
     *         False - Do not send test results
     */
    const bool ShouldSendResultToHost(void);
    /**
     * Transmit the result string to the plant host system system.
     *
     * @param result The result string to be sent.
     */
    virtual void SendResultToHost(const string &result);
    /**
     * Send the test result string to the external host.
     *
     * @param resultString
     *                 Test result string to be sent.
     * @param portComm Port object to send the test result string.
     * @param ackChar  The acknowledgement character to look for.
     * @param maxAttempts Maximum number of times to try to successfully send 
     *                    the message.
     * @param hostResponseTime
     *                 Length of time (in ms) to wait for a response from the 
     *                 external host.
     * 
     * @return Status of the message transmit operation.
     */
    virtual const BEP_STATUS_TYPE SendTestResultString(string &resultString,
                                                       SerialChannel &portComm,
                                                       const unsigned char ackChar,
                                                       const INT32 &maxAttempts = 3,
                                                       const INT32 &hostResponseTime = 1000);
    /**
     * Determine if the test results should be processed and if a result string 
     * should be generated.  The test result data will be analyzed to determine 
     * if processing should continue.  If the VehicleBuild node is not in the 
     * test results, this is assumed to be a Loss Compensation result.  A 
     * result string should not be generated for the host system.  If the 
     * PerformAbort node is not set to Perform, there are no valid results to 
     * report to the host system.
     * 
     * @param results Test results to be analyzed.
     * 
     * @return Flag indicating if a result string should be generated.
     *         <ul>
     *         <li> True - Generate a result string to send to the host system. </li>
     *         <li> False - Skip generating a result string. </li>
     *         </ul>
     */
    virtual bool ShouldProcessTestResults(const XmlNode *results);
    /**
     * Determine if PFS Aborts should be sent to the plant host system.
     *
     * @return True  - Send results to plant host system
     *         False - Do not send test results
     */
    const bool ReportPfsAborts(void);
    /**
     * Generate the result string to be sent to the host system.
     * 
     * @param testResults
     *               Test result data to be used for generating the result 
     *               string for the host system.
     * 
     * @return Result string to be transmitted to the host system.
     */
    virtual string GenerateHostResultString(const XmlNode *testResults);
    /**
     * Get/Set the name of the host system to interface with.
     * 
     * @param name   Name of the host system to interface with.
     * 
     * @return Name of the host system to interface with.
     */
    const string& HostSystemName(string *name = NULL);
    /**
     * Return the current machine number.
     *
     * @return The number of this machine.
     */
    const string GetMachineNumber(void);
    /**
     * Get/Set the flag indicating if a new result is ready to process.
     * 
     * @param resultReady
     *               Flag indicating if a result is ready to process.
     * 
     * @return Flag indicating if a result is ready to process.
     */
    const bool& ResultReadyToProcess(const bool *resultReady = NULL);
    XmlNode* PopTestResult();
    void PushTestResult(XmlNode *result);
    /**
     * Store the configuration for the keep alive thread.
     *
     * @param  config  Configuration data for the keep alive thread.
     */
    void SetKeepAliveConfig(const XmlNode *config);
    /**
     * Get the keep alive configuration data for the keep alive thread.
     */
    const XmlNode* GetKeepAliveConfig(void);

    /**
     * Current test result to process.
     */
    XmlNode *m_testResult;
    /**
     * Pointer to INamedDataBroker communication object.
     */
    INamedDataBroker *m_ndb;
	BepMutex m_testResultMutex;




private:
    /**
     * Log the data being used to build the result string for the host system.
     * 
     * @param data Data being used to generate the result string to be sent to 
     *             the host system.
     */
    void LogTestResultData(const XmlNode *data);
    /**
     * Store the current machine number.
     */
    void StoreMachineNumber(void);
    /**
     * Holds VIN from build record in test result.
     * Used for outgoing message if VIN offset in configuration file is set to 
     * -1.
     */
    string m_msgVin;
    /**
     * Start index of the Vin field in the result string.
     */
    INT32 m_vinIndex;
    /**
     * Maximum amount of time to wait for a response from the plant host system.
     */
    INT32 m_maxHostResponseTime;
    /**
     * Maximum number of attempts to send a test result string to PFS.
     */
    INT32 m_maxAttempts;
    /**
     * Message ID to place on the result sent to the WCC.
     */
    UINT8 m_msgId;
    /**
     * Name of the host system to interface with.
     * This will also be used to select the correct ExternalHostInterface node 
     * for configuration items.
     */
    string m_hostSystemName;
    /**
     * Name the host interface is known as in the CCRT system.
     */
    string m_hostInterfaceName;
    /**
     * The number of this machine.
     */
    string  m_machineNumber;
    /**
     * Flag indicating a test result has ben received and should be processed.
     */
    bool m_resultReadyToProcess;
    /**
     * Serial Communication object to use for communicaion with the plant host 
     * system.
     */
    SerialChannel  *m_plantHostComm;
        /**
     * Vector to hold the pointers to the test results to publish
     *
     * @see StaticTesterServer
     */
    vector<XmlNode *>  m_testResultsToProcess;
    /**
     * Map of the strings to convert test result to format acceptable by plant 
     * host system.
     */
    XmlNodeMap   m_resultConversions;
    /**
     * List of component names and how to translate them.
     */
    XmlNodeMap   m_componentNameConversions;
    /**
     * The base Test Result String record.  This information is the same 
     * regardless of the detail type the plant has selected.
     */
    XmlNodeMap   m_resultMap;
    /**
     * The format to use to generate the parent DTC node.
     */
    XmlNodeMap   m_parentDtcNodeFormat;
    /**
     * Keep alive.
     */
    HostInterfaceKeepAlive *m_keepAlive;
    /** 
     * Configuration data for the keep alive thread.
     */
    XmlNode *m_keepAliveConfig;
    /** 
     * list of position of spaces allowed in result string.
     */
    vector<UINT32>  m_allowedSpacePositions;
	/** List of DTCs that have already been reported **/
	vector<string>  m_reportedDtcs;
};

//-----------------------------------------------------------------------------
#endif //HostInterface_h
