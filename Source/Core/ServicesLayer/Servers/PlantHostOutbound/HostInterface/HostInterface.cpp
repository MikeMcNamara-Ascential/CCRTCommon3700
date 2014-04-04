//******************************************************************************
// Description:
//   HostInterface class to handle communications between the CCRT system and
//   a plant host system.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/PlantHostOutbound/HostInterface/HostInterface.cpp $
// 
// 1     11/01/07 3:28p Derickso
// Updated strategy for processing incoming test result files.
// 
// 1     5/01/07 1:02p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
// 
//******************************************************************************
// $NoKeywords: $
//=============================================================================
#include "HostInterface.h"
#include "GeneralUtilities.h"
#include <math.h>
#include <algorithm>
//-----------------------------------------------------------------------------

HostInterface::HostInterface() : BepServer(),
m_testResult(NULL), m_resultReadyToProcess(true), m_plantHostComm(NULL), m_ndb(NULL), m_keepAlive(NULL)
{	// Nothing special to do here
}

//-----------------------------------------------------------------------------
HostInterface::~HostInterface()
{	// Clear any XML maps to prevent memory leaks
	m_resultMap.clear(true);
	m_resultConversions.clear(true);
	// Get rid of all comm objects
	if(m_plantHostComm != NULL)	 delete m_plantHostComm;
	m_plantHostComm = NULL;
	if(m_ndb != NULL)  delete m_ndb;
	m_ndb = NULL;
	// Stop the keep alive thread
	if(m_keepAlive != NULL)	 delete m_keepAlive;
	m_keepAlive = NULL;
}

//-----------------------------------------------------------------------------
void HostInterface::Initialize(int argc, char *argv[])
{	// Parse the command line options for the configuration fie and other data
	ParseCommandLine(argc, argv);
	Initialize(GetConfigFile());
}

//-----------------------------------------------------------------------------
void HostInterface::Initialize(const string &fileName)
{
	const XmlNode *config = m_parser.ReturnXMLDocument(fileName);
	if(config != NULL)
	{
		Initialize(config->getChild("Setup/HostInterfaces")->getChild(HostInterfaceName()));
	}
}

//-----------------------------------------------------------------------------
void HostInterface::Initialize(const XmlNode *document)
{
	// Initialize the base class
	BepServer::Initialize(document);
	Log(LOG_FN_ENTRY, "HostInterface::Initialize() - Enter");
	// Create a new INamedDataBroker comm object
	// if an instance of namedDataBroker interface does not already exist, make one
	if(m_ndb == NULL)
	{
		m_ndb = new INamedDataBroker();
		Log(LOG_DEV_DATA, "Created new NamedDatBroker comm object");
	}
	// Load additional config data
	LoadAdditionalConfigurationItems(document);
	// Start the host system comm object
	if(m_plantHostComm == NULL)
	{
		m_plantHostComm = new SerialChannel();
		m_plantHostComm->Initialize(document->getChild("Setup/HostCommunication"));
		Log(LOG_DEV_DATA, "Created host comm object");
	}
	// Check if the keep alive thread should be started
	if( GetKeepAliveConfig() != NULL)
	{	// Get the machine number
		Log(LOG_DEV_DATA, "HostInterface::Initialize Configuring Keep alive thread");
		m_keepAlive = new HostInterfaceKeepAlive(this);
		m_keepAlive->Initialize(GetKeepAliveConfig());
		m_keepAlive->Resume();
	}
	else
	{
		Log(LOG_DEV_DATA, "HostInterface::Initialize - Not starting keep alive thread.");
	}
	m_reportedDtcs.clear();
	Log(LOG_FN_ENTRY, "HostInterface::Initialize() - Exit");
}

//-----------------------------------------------------------------------------
void HostInterface::Run(volatile bool *terminateFlag)/*=NULL*/
{
	bool notReady = false;
	bool ready = true;
	while(GetStatus() != BEP_TERMINATE)
	{	// Wait for a result to be written to us
		if(ResultReadyToProcess())
		{//Check list to determine if a test result is available note: popTestResult() deletes record from list  
			if((m_testResult = PopTestResult()) != NULL)
			{	// Indicate result is not ready to be processed
				ResultReadyToProcess(&notReady);
				// Process the new result
				ProcessTestResults();
				if(m_testResult != NULL)  delete m_testResult;
				m_testResult = NULL;
				ResultReadyToProcess(&ready);
			}
			else
			{	// Wait for a bit for the result to appear
				BposSleep(1000);
			}
		}
		else
		{	// Wait for a bit for the result to appear
			BposSleep(1000);
		}
	}
}

//-----------------------------------------------------------------------------
const string HostInterface::Write(const string &tag, const string &value)
{
	return BepServer::Write(tag, value);
}

#if 0
//-----------------------------------------------------------------------------
const string HostInterface::Write(const XmlNode *node)
{   
	string result(BEP_ERROR_RESPONSE);
	//static bool readyToProcess = true;
	Log(LOG_FN_ENTRY, "HostInterface::Write(%s) - Enter", node->getName().c_str());
	if(!node->getName().compare(TEST_RESULT_TAG))
	{	// This is the modified test result from PlantHostOutbound, store it and signal to process it
		XmlParser parser;
		//Add test result to list of test results to process
		PushTestResult(parser.ReturnXMLDocument(getenv("USR_ROOT") + node->getValue())->Copy());
		//m_testResult = parser.ReturnXMLDocument(getenv("USR_ROOT") + node->getValue())->Copy();
		//ResultReadyToProcess(&readyToProcess);
		result = BEP_SUCCESS_RESPONSE;
	}
	else
	{	// Let the base class handle this
		result = BepServer::Write(node);
	}
	Log(LOG_FN_ENTRY, "HostInterface::Write(%s) - Exit: %s", node->getName().c_str(), result.c_str());
	return result;
}

//-----------------------------------------------------------------------------
void HostInterface::PushTestResult(XmlNode *result)
{//Push Result onto list
	if((errno = m_testResultMutex.Acquire()) == EOK)
	{  //Check if list is empty 
		if(!m_testResultsToProcess.empty())
		{//Search list to see if result is already there
			vector<XmlNode*>::iterator iter;
			iter = find(m_testResultsToProcess.begin(), m_testResultsToProcess.end(), result);
			if(iter != m_testResultsToProcess.end())
			{
				Log(LOG_DEV_DATA, "Pushed testresult");
				m_testResultsToProcess.push_back(result);
			}
			else
			{
				Log(LOG_DEV_DATA, "Result Already Exists");
				if(result != NULL) delete result;
				result = NULL;
			}

		}
		else
		{
			Log(LOG_DEV_DATA, "Pushed testresult");
			m_testResultsToProcess.push_back(result);
		}
		m_testResultMutex.Release();
	}
	else
	{
		Log(LOG_ERRORS, "HostInterface: Could not Aquire mutex errno: %s", strerror(errno));
	}
}
#endif


//-----------------------------------------------------------------------------
const string HostInterface::Write(const XmlNode *node)
{   
	string result(BEP_ERROR_RESPONSE);
	//static bool readyToProcess = true;
	Log(LOG_FN_ENTRY, "HostInterface::Write(%s) - Enter", node->getName().c_str());
	if(!node->getName().compare(TEST_RESULT_TAG))
	{	// This is the modified test result from PlantHostOutbound, store it and signal to process it
		XmlParser parser;
        ResultContainer* resultObject = new ResultContainer();
        resultObject->resultFileName =  getenv("USR_ROOT") + node->getValue();
        resultObject->testResult = parser.ReturnXMLDocument(resultObject->resultFileName)->Copy();
		//Add test result to list of test results to process
		PushTestResult(resultObject);
		//m_testResult = parser.ReturnXMLDocument(getenv("USR_ROOT") + node->getValue())->Copy();
		//ResultReadyToProcess(&readyToProcess);
		result = BEP_SUCCESS_RESPONSE;
	}
	else
	{	// Let the base class handle this
		result = BepServer::Write(node);
	}
	Log(LOG_FN_ENTRY, "HostInterface::Write(%s) - Exit: %s", node->getName().c_str(), result.c_str());
	return result;
}

//-----------------------------------------------------------------------------
void HostInterface::PushTestResult(ResultContainer *result)//(XmlNode *result)
{//Push Result onto list
	if((errno = m_testResultMutex.Acquire()) == EOK)
	{  //Check if list is empty 
		if(!m_testResultsToProcess.empty())
		{//Search list to see if result is already there
            for(vector<ResultContainer*>::iterator iter = m_testResultsToProcess.begin(); iter != m_testResultsToProcess.end(); iter++)
            {
                if(!(*iter)->resultFileName.compare(result->resultFileName))
                {

                    Log(LOG_DEV_DATA, "Result Already Exists");
                    if(result != NULL) delete result;
                    result = NULL;
                    m_testResultMutex.Release();
                    return;
                }
            }
            m_testResultsToProcess.push_back(result);
            Log(LOG_DEV_DATA, "Pushed testresult");
			//if(iter != m_testResultsToProcess.end())
			//{
			//	Log(LOG_DEV_DATA, "Pushed testresult");
			//	m_testResultsToProcess.push_back(result);
			//}
			//else
			//{
		//		Log(LOG_DEV_DATA, "Result Already Exists");
		//		if(result != NULL) delete result;
		//		result = NULL;
		//	}

		}
		else
		{
			Log(LOG_DEV_DATA, "Pushed testresult");
			m_testResultsToProcess.push_back(result);
		}
		m_testResultMutex.Release();
	}
	else
	{
		Log(LOG_ERRORS, "HostInterface: Could not Aquire mutex errno: %s", strerror(errno));
	}
}

//-----------------------------------------------------------------------------
XmlNode* HostInterface::PopTestResult()
{//Get last test result added and remove from list
	XmlNode *testResultNode = NULL;
	if((errno = m_testResultMutex.Acquire()) == EOK)
	{
		Log(LOG_DEV_DATA, "Attempting to pop");
		if(!m_testResultsToProcess.empty())
		{
			Log(LOG_DEV_DATA, "Popping test result");
            if (m_testResultsToProcess.back()->testResult != NULL)
            {
                Log(LOG_DEV_DATA, "Copying test result");
                testResultNode = m_testResultsToProcess.back()->testResult->Copy();

                if(testResultNode == NULL)
                {
                    Log(LOG_DEV_DATA, "Value is null here");
                }
                else
                {
                    delete m_testResultsToProcess.back();
                }
                m_testResultsToProcess.pop_back();
            }
            else
            {
                Log(LOG_DEV_DATA, "Stored Result is null");
            }
		}
		m_testResultMutex.Release();
	}
	else
	{
		Log(LOG_ERRORS, "HostInterface: Could not Aquire mutex errno: %s", strerror(errno));
	}
	return testResultNode;
}

//-----------------------------------------------------------------------------
void HostInterface::LoadAdditionalConfigurationItems(const XmlNode *config)
{
	// Store the host system name
	string hostName(config->getName());
	HostSystemName(&hostName);
	Log(LOG_DEV_DATA, "Current process ID: %d", GetProcecssPid("core/"+HostSystemName()));
	// Store the index to the VIN
	INT32 vinIndex = -1;   // Default
	try
	{
		vinIndex = BposReadInt(config->getChild("Setup/VinIndex")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "HostInterface: Could not get VIN index, using %d: %s", vinIndex, excpt.GetReason());
	}
	VinIndex(&vinIndex);
	Log(LOG_DEV_DATA, "Stored VIN index: %d", VinIndex());
	// Store the maximum response time 
	INT32 responseTime = 3000;	// Default to 3 seconds
	try
	{
		responseTime = BposReadInt(config->getChild("Setup/MaxResponseTime")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "HostInterface: Could not get host response time, using %d: %s", responseTime, excpt.GetReason());
	}
	MaxHostResponseTime(&responseTime);
	Log(LOG_DEV_DATA, "Stored max host response time: %d ms", MaxHostResponseTime());
	// Store the maximum number of attempts for a successful message transmission
	INT32 maxAttempts = 3;	//Default to 3 attempts
	try
	{
		maxAttempts = BposReadInt(config->getChild("Setup/MaxAttempts")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "HostInterface: XmlException - Could not get max transmit attempts, using %d: %s", maxAttempts, excpt.GetReason());
	}
	MaxMsgSendAttempts(&maxAttempts);
	Log(LOG_DEV_DATA, "Stored max message send attempts: %d", MaxMsgSendAttempts());
	// Load the base result string map
	XmlParser parser;
	try
	{
		ResultMap(parser.ReturnXMLDocument(getenv("USR_ROOT") + config->getChild("Setup/ResultMap")->getValue()));
		Log(LOG_DEV_DATA, "Stored result map");
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "HostInterface: Could not load result base map: %s", excpt.GetReason());
	}
	catch(BepException &excpt)
	{
		Log(LOG_ERRORS, "HostInterface: Could not find result base map: %s", excpt.GetReason());
	}
	// Load the Parent DTC node format
	try
	{
		SetResultStrings(config->getChild("Setup/ParentNodeDtcFormat"), m_parentDtcNodeFormat);
		Log(LOG_DEV_DATA, "HostInterface: Stored parent DTC node format");
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "HostInterface: Not storing parent DTC node format - %s", excpt.GetReason());
		m_parentDtcNodeFormat.clear(true);
	}
	// Store the message ID to use when transmitting this message
	UINT8 msgId = STX;
	try
	{
		msgId = BposReadInt(config->getChild("Setup/MessageID")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "HostInterface: Could not get message id, using 0x%02X - %s", msgId, excpt.GetReason());
	}
	MessageId(&msgId);
	Log(LOG_DEV_DATA, "Stored message id: 0x%02X", MessageId());
	// Store the result conversion table
	try
	{
		SetResultStrings(config->getChild("Setup/TestResultConversion"), m_resultConversions);
		Log(LOG_DEV_DATA, "Stored test result conversions");
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "HostInterface: Could not load result conversion table - %s", excpt.GetReason());
	}
	// Store the component name mapping
	try
	{
		m_componentNameConversions.DeepCopy(config->getChild("Setup/ComponentNameMapping")->getChildren());
		Log(LOG_DEV_DATA, "Stored component name conversions");
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "LoadAdditionalConfigurationItems() - Not storing component name mappings: %s", excpt.GetReason());
		m_componentNameConversions.clear(true);
	}
	try
	{	// Get the setup data for the host keepalive
		SetKeepAliveConfig(config->getChild("Setup/KeepAliveConfig"));
	}
	catch(...)
	{
		Log(LOG_ERRORS, "WARNING: No keepalive comm not set");
		SetKeepAliveConfig(NULL);
	}

	// Determine if any test files should be archived
	bool archiveResults = false;
	try
	{
		archiveResults = atob(config->getChild("Setup/ArchiveTestData")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Not achiving test results: %s", excpt.GetReason());
	}
	TestDataFileArchiveEnabled(&archiveResults);
	if(archiveResults)
	{   // Save the map for the test data file naming
		try 
		{
            XmlParser nameParser;
            const XmlNode *testDataFileNamingMap = nameParser.ReturnXMLDocument(getenv("USR_ROOT") + config->getChild("Setup/ArchiveFilenameMap")->getValue());
            m_testDataFileNamingMap.DeepCopy(testDataFileNamingMap->getChildren());
			m_filesToArchive.DeepCopy(config->getChild("Setup/FilesToArchive")->getChildren());
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "No file naming map defined, not archiving: %s", excpt.GetReason());
			archiveResults = false;
			TestDataFileArchiveEnabled(&archiveResults);
		}
		catch(BepException &excpt)
		{
			Log(LOG_ERRORS, "No file naming map defined, not archiving: %s", excpt.GetReason());
			archiveResults = false;
			TestDataFileArchiveEnabled(&archiveResults);
		}
	}


	// Determine if perform abort check should be skipped
	bool skipPerformAbortCheck = false;
	try
	{
		skipPerformAbortCheck = atob(config->getChild("Setup/SkipPerformAbortCheck")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Not skipping perform abort check: %s", excpt.GetReason());
	}
	SkipPerformAbortCheck(&skipPerformAbortCheck);


	// Determine if in cycle results should be reported
	bool reportIntermediateResults = false;
	try
	{
		reportIntermediateResults = atob(config->getChild("Setup/ReportIntermediateResults")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Not reporting intermediate results: %s", excpt.GetReason());
	}
	ReportIntermediateResults(&reportIntermediateResults);

	// Store the current machine number Moved 04.24.2007 JPS store number before pfs header is set
	StoreMachineNumber();
}

//-----------------------------------------------------------------------------
void HostInterface::ParseCommandLine(int argc, char *argv[])
{
	Log(LOG_FN_ENTRY, "HostInterface::ParseCommandLine() - Enter");
	string options("f:der:v:P:p:");	 // Valid options
	string value;
	optind = 1;	  // Start parsing at the options.  Position 0 is the executable name
	// Process all arguments/command line options
	while(optind < argc)
	{	// Process the current option
		char optId = getopt(argc, argv, options.c_str());
		switch(optId)
		{
		case 'p':	// Path to use
			value = optarg;
			HostInterfaceName(&value);
			break;

		case 'P':
			value = (optarg != NULL) ? optarg : "core/PlantHostOutbound";
			break;

		case 'e':
			EnableLogger();
			SetLocalEcho(true);
			break;

		case 'f':
			if(optarg != NULL)	SetConfigFile(optarg);
			break;

		case 'r':
			if(optarg != NULL)	SetProcessName(optarg);
			break;

		case 'v':
			if(optarg != NULL)
			{
				char *tmp;
				UINT32  verbose;
				if( (tmp=strstr( optarg, "$")) != NULL)			sscanf( optarg, "$%X", &verbose);
				else if( (tmp=strstr( optarg, "0x")) != NULL)	sscanf( optarg, "0x%X", &verbose);
				else if( (tmp=strstr( optarg, "0X")) != NULL)	sscanf( optarg, "0X%X", &verbose);
				else											sscanf( optarg, "%X", &verbose);
				SetVerboseMask( verbose);
			}
			break;

		default:
			Log(LOG_ERRORS, "Unknown command line argument: %c", optId);
		}
	}
	Log(LOG_FN_ENTRY, "HostInterface::ParseCommandLine() - Exit");
}

//-----------------------------------------------------------------------------
void HostInterface::ProcessTestResults(void)
{  
	Log(LOG_FN_ENTRY, "HostInterface::ProcessTestResults() - Enter");
	// Ensure we have a vlid test result node
	if(m_testResult != NULL)
	{	// Log the test result data being used to generate the result string
		LogTestResultData(m_testResult);
		// Determine if the results should be processed
		if(ShouldProcessTestResults(m_testResult))
		{
			DoResultProcessing(m_testResult);
		}
	}
	else
	{
		Log(LOG_ERRORS, "Test result node is NULL, not processing results!");
	}
	Log(LOG_FN_ENTRY, "HostInterface::ProcessTestResults() - Exit");
}

//-----------------------------------------------------------------------------
void HostInterface::DoResultProcessing(const XmlNode *testResults)
{	// Store the VIN for reporting purposes if needed
	string vin("VinSourceNotValid");
	if(VinIndex() == -1)
	{	// Get the vin from the build data in the test results
		try
		{
			vin = testResults->getChild(VEHICLE_BUILD_TAG)->getChild(VIN_DATA_TAG)->getValue();
		}
		catch(XmlException &excpt)
		{
			vin = "VinNotInBuildData";
			Log(LOG_ERRORS, "VIN not included in vehicle build data, using %s - %s", vin.c_str(), excpt.GetReason());
		}
		MessageVin(&vin);
	}
	Log(LOG_DEV_DATA, "HostInterface::DoResultProcessing - Using vin: %s for message vin", vin.c_str());
	// Ensure overall result is pass or fail
	try
	{	// Get the overall test result
		string overallTestResult = 
		testResults->getChild(BEP_TEST_RESULT)->getChild(GetDataTag("OverallTest"))->getAttribute(BEP_RESULT)->getValue();
		if((BEP_PASS_RESPONSE != overallTestResult) && 
		   (BEP_ABORT_RESPONSE != overallTestResult) && 
		   (BEP_FAIL_RESPONSE != overallTestResult))
		{	// Result is something other than pass/fail/abort, set to fail
			XmlNode *overallResult = 
			const_cast<XmlNode *>(testResults->getChild(BEP_TEST_RESULT)->getChild(GetDataTag("OverallTest"))->getAttribute(BEP_RESULT));
			overallResult->setValue(BEP_FAIL_RESPONSE);
		}
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "HostInterface: XmlException setting overall test result to valid value - %s", excpt.GetReason());
	}
	// Generate the host result string
	string resultString(GenerateHostResultString(testResults));
	// If results should be sent to the host system, send the result string
	if(ShouldSendResultToHost())
	{	// Send the result string to the host system
		SendResultToHost(resultString);
		Log(LOG_DEV_DATA, "Host Interface: Sent result string to host system - %s", resultString.c_str());
	}
	else
	{
		Log(LOG_DEV_DATA, "HostInterface: Results not sent to the host system!");
	}

	if(TestDataFileArchiveEnabled())
	{
        // Build the file name
		char buff[256];
		INT32 junk = 0;
		string dataFileName(BuildTestResultString(testResults, m_testDataFileNamingMap, ResultConversions(), junk));
        Log(LOG_DEV_DATA, "HuntsvilleInterface::DoResultProcessing() The String as it is so far: %s", dataFileName.c_str());
		// Build the string for backing up all desired test data files
		string command("mkdir -m 777 TestResults/Archive; zip \"TestResults/Archive/" + dataFileName + "\" ");
		for(XmlNodeMapCItr iter = m_filesToArchive.begin(); iter != m_filesToArchive.end(); iter++)
		{   // Add the file names to the command
			string fileName = iter->second->getAttribute("Path")->getValue();
			XmlNodeMapCItr fileIter = iter->second->getAttributes().find("FileName");
			if(fileIter != iter->second->getAttributes().end())
			{
				fileName += fileIter->second->getValue();
			}
			else
			{
				string dataItem = testResults->getChild(iter->second->getAttribute("DataItem")->getValue())->getValue();
				INT32 startIndex = BposReadInt(iter->second->getAttribute("Offset")->getValue().c_str());
				INT32 length = BposReadInt(iter->second->getAttribute("Length")->getValue().c_str());
				fileName += CreateMessage(buff, sizeof(buff), 
										  iter->second->getAttribute("Format")->getValue().c_str(), 
										  ("\"" + dataItem.substr(startIndex, length) + "\"").c_str());
			}
			// Add the file to the command
			command += (fileName + " ");
		}
		// Run the command to compress all the files into one archive
		Log(LOG_DEV_DATA, "Running command to compress all test files: %s", command.c_str());
		if(system(command.c_str()) != -1)
		{  
            Log(LOG_DEV_DATA, "Successfully generated zip archive: %s", dataFileName.c_str());
		}
		else
		{
			Log(LOG_ERRORS, "Could not compress test data files, not archiving");
		}
	}
	else
	{
		Log(LOG_DEV_DATA, "Not archiving test data files");
	}
}

//-----------------------------------------------------------------------------
const string HostInterface::BuildTestResultString(const XmlNode *testResult,
												  const XmlNodeMap &resultStringMap,
												  const XmlNodeMap &resultConversions,
												  INT32 &totalSerialNumbers,
												  const bool &useVehicleBuild, /*= true*/
												  const bool &allowSpaceInResult) /*= true*/
{
	string testResultString, result;
	// Place the test results into a map so we can easily access the information
	const XmlNodeMap &testResultMap = testResult->getChild(BEP_TEST_RESULT)->getChildren();
	const XmlNodeMap *vehicleBuildMap;
	if(useVehicleBuild)	vehicleBuildMap = &(testResult->getChild(VEHICLE_BUILD_TAG)->getChildren());
	string logMessage("");
	// Log the entry and clear the result 
	Log(LOG_DEV_DATA, "HostInterface::BuildTestResultString() - Enter");
	m_reportedDtcs.clear();
	testResultString.erase();
	// Build the result string
	for(XmlNodeMapCItr iter = resultStringMap.begin(); iter != resultStringMap.end(); iter++)
	{	// Process the next field for the result string
		result.erase();
		m_allowedSpacePositions.clear();
		Log(LOG_DETAILED_DATA, "Preparing to Process field: %s", iter->second->ToString().c_str());
		try
		{
			result = ProcessMapNode(iter->second, testResultMap, *vehicleBuildMap, resultConversions, totalSerialNumbers);
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Malformed test result node for map node: {%s}", iter->second->ToString().c_str());
			Log(LOG_ERRORS, "Clearing result.");
			result.clear();
		}
		Log(LOG_DETAILED_DATA, "Completed processing field: %s", iter->second->ToString().c_str());
		if(!allowSpaceInResult)
		{	// Make sure we do not translate a space separator to underscore
			if(result.length() > 1)
			{	// Replace any spaces in the result string with underscores
				UINT32 index = 0;
				UINT32 allowedSpaceIndex = 0;
				while((index = result.find_first_of(' ', index)) != result.npos)
				{
					if(!m_allowedSpacePositions.empty())
					{
						if(m_allowedSpacePositions[allowedSpaceIndex] != index)
						{
							result[index] = '_';
						}
						else
						{//skip and increment space index
							index++;
							if(allowedSpaceIndex != m_allowedSpacePositions.size()-1)
							{
								allowedSpaceIndex++;
							}
						}
					}
					else result[index] = '_';
				}
			}
		}
		// Add the result to the test result string
		Log(LOG_DEV_DATA, "Adding Test Result: %s -- {%s}", iter->second->getValue().c_str(), result.c_str());
		testResultString += result;
	}
	// Log the test result string that was generated and return it
	Log(LOG_DEV_DATA, "Test Result string: %s", testResultString.c_str());
	return(testResultString);
}

//-----------------------------------------------------------------------------
const string HostInterface::ProcessMapNode(const XmlNode *resultMapNode,
										   const XmlNodeMap &testResults,
										   const XmlNodeMap &vehicleData,
										   const XmlNodeMap &resultConversions,
										   INT32 &totalSerialNumbers)
{
	Log(LOG_DETAILED_DATA, "ProcessMapNode() - resultMapNode: %s", resultMapNode->ToString().c_str());
	string type, response, tag, result,format,dataType;
	INT32 length=0;
	float scale = 1;
	float offset = 0.0;

	// Get the field type
	type = resultMapNode->getAttributes().getNode(fieldType)->getValue();
	// Process the field
	if(type == BEP_RESULT)
	{	// Determine the test result for this field
		result = GetTestResult(testResults, resultMapNode, resultConversions);
	}
	else if((type == typeDetail) || (type == typeSerialNumber) || (type == typeDetailString))
	{	// Add the value to the result string
		result = GetTestDetailValue(testResults, resultMapNode);
		if((type == typeSerialNumber) && (result.length() == serialNumberLength))
		{	// Increment the total number of serial numbers reported
			totalSerialNumbers++;
		}
	}
	else if(type == BEP_TEST_RESULT_CODE)
	{	// Determine the test result for this field
		result = GetTestResultCode(testResults, resultMapNode);
	}
	else if(type == typeConditionalBlock)
	{	// Check if the result of the specified test against the desired result.  If it matches, process the block.
		string condition("equal");	// Default to equals
		XmlNodeMapCItr iter = resultMapNode->getAttributes().find("Condition");
		if(iter != resultMapNode->getAttributes().end())
		{
			condition = iter->second->getValue();
		}
		// Determine if this conditional block should be processed
		try
		{
			bool useConversionTable = false;
			string testResult = GetTestResult(testResults, resultMapNode->getAttribute("Test"), resultConversions, &useConversionTable);
			string conditionResult = resultMapNode->getAttribute("Result")->getValue();
			if(((stricmp(condition.c_str(), "equal") == 0) && !conditionResult.compare(testResult)) ||
			   ((stricmp(condition.c_str(), "notequal") == 0)  && conditionResult.compare(testResult)))
			{	// Result matches expected, process the data
				result.erase();
				const XmlNodeMap &conditionalResults = resultMapNode->getChildren();
				for(XmlNodeMapCItr iter = conditionalResults.begin(); iter != conditionalResults.end(); iter++)
				{
					result += ProcessMapNode(iter->second, testResults, vehicleData, resultConversions, totalSerialNumbers);
				}
			}
		}
		catch(XmlException &err)
		{
			Log(LOG_ERRORS, "Could not find attribute in result map: %s", err.GetReason());
		}
	}
	else if(type == typeVehicleBuild)
	{	// Get the data from the vehicle build map
		XmlNodeMapCItr buildIter;
		Log(LOG_DETAILED_DATA, "Looking for build item: %s\n", resultMapNode->getValue().c_str());
		if((buildIter = vehicleData.find(resultMapNode->getValue())) != vehicleData.end())
		{	// Get the value of the node
			result = buildIter->second->getValue();
			Log(LOG_DETAILED_DATA, "Found build item -- result: %s\n", result.c_str());
			if(resultMapNode->getValue() == driveCurve)
			{
				int dcNameStart = result.find_last_of("/")+1;
				result = result.substr(dcNameStart);
				Log(LOG_DETAILED_DATA, "Converted DriveCurve -- result: %s\n", result.c_str());
			}
		}
		else
		{	// Default to not equipped for the requested option
			result = noResult;
			Log(LOG_DETAILED_DATA, "Did not find build item -- result: %s\n", result.c_str());
		}
		// Check if this is a Yes/No value and convert appropriately
		if(result == yesResult || result == noResult) result = ConvertResultString(result, resultConversions);

		// get the formatting specified by the node and format the value
		format = GetFormatAttribute(resultMapNode);
		dataType = GetDataTypeAttribute(resultMapNode);
		length = GetDataLengthAttribute(resultMapNode);
		offset = GetOffsetAttribute(resultMapNode);
		result = ApplyDataFormatting(dataType,result,format,length,scale,offset);

		Log(LOG_DETAILED_DATA, "Reporting Vehicle Build Information: %s == %s", resultMapNode->getValue().c_str(), result.c_str());
	}
	else if(type == typeSeparator)
	{	// Add the separator
		if(resultMapNode->getValue() == "Blank")
		{
			result = std::string(" ");
		}
		else result	= resultMapNode->getValue();
	}
	else if(type == dateTimeNode)
	{	// See if there is a specific format defined
		try
		{	// Get the date/time format to apply to the data
			string format = resultMapNode->getAttribute(fieldFormat)->getValue();
			// Get current time from the system
			time_t systemTime = time(NULL);
			struct tm *currentTime = localtime(&systemTime);
			char timeBuffer[32];
			// Format the time
			strftime(timeBuffer, sizeof(timeBuffer), format.c_str(), currentTime);
			result = timeBuffer;
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "No specific data/time format specified, using default (MMDDYYYY/HHMM): %s", excpt.GetReason());
			result = GetTimeAndDate(resultMapNode->getValue());
		}
	}
	else if(type == typeValue)
	{	// Read the value through the NamedDataBroker
		if(("Date" == resultMapNode->getValue()) || ("Time" == resultMapNode->getValue()))
		{	// Get the date from the system
			result = GetTimeAndDate(resultMapNode->getValue());
		}
		else
		{	// Get the data through the NamedDataBroker
			if(m_ndb != NULL)
			{
				Log(LOG_DETAILED_DATA, "Getting data from NamedDataBroker: %s\n", resultMapNode->getValue().c_str());
				m_ndb->Read(resultMapNode->getValue(), response, true);
				m_ndb->GetNext(tag, result, response);
			}
		}
		// get the format specified by the node and format the value
		format = GetFormatAttribute(resultMapNode);
		dataType = GetDataTypeAttribute(resultMapNode);
		length = GetDataLengthAttribute(resultMapNode);
		scale = GetScaleAttribute(resultMapNode);
		offset = GetOffsetAttribute(resultMapNode);
		result = ApplyDataFormatting(dataType,result,format,length,scale,offset);
	}
	else if(type == typeDtc)
	{	// Process the DTCs
		if(testResults.find("DTC") != testResults.end())
		{
			result = ProcessDtcNode(resultMapNode, testResults, vehicleData, resultConversions, totalSerialNumbers); 
		}
		else
		{
			Log(LOG_DEV_DATA, "No Reported DTCs to add to result string!");
		}
	}
	else if(type == typeComponentName)
	{	// Get the component name from the format string.  This is typically dynamically inserted into the format string
		// in the ProcessDtcNode function.
		result = resultMapNode->getValue();
		// Get the formatters
		format = GetFormatAttribute(resultMapNode);
		dataType = GetDataTypeAttribute(resultMapNode);
		length = GetDataLengthAttribute(resultMapNode);
		result = ApplyDataFormatting(dataType,result,format,length,scale,offset);
	}
	else if(type == typeDtcAttribute)
	{	// Get the requested attribute from the DTC node.  
		// ASSUMPTION: only one DTC node in the testResults map.
		XmlNodeMapCItr dtcIter = testResults.getNode(0)->getAttributes().find(resultMapNode->getValue());
		result = (dtcIter != testResults.getNode(0)->getAttributes().end()) ? dtcIter->second->getValue() : "NotReported";
		// Format the result 
		format = GetFormatAttribute(resultMapNode);
		dataType = GetDataTypeAttribute(resultMapNode);
		length = GetDataLengthAttribute(resultMapNode);
		result = ApplyDataFormatting(dataType,result,format,length,scale,offset);
	}
	else if(type == typeDtcData)
	{	// Get the requested attribute from the DTC node.  
		// ASSUMPTION: only one DTC node in the testResults map.
		result = testResults.getNode(0)->getValue();
		// Format the result 
		format = GetFormatAttribute(resultMapNode);
		dataType = GetDataTypeAttribute(resultMapNode);
		length = GetDataLengthAttribute(resultMapNode);
		result = ApplyDataFormatting(dataType,result,format,length,scale,offset);
	}
	else if(type == typeXmlTag)
	{
		result = "<" + resultMapNode->getValue() + ">";
	}
	try
	{
		if(!result.empty() && (dataType != typeASCIIInt))
		{
			std::string locLengthStr = resultMapNode->getAttributes().getNode(fieldLength)->getValue();
			UINT16 locLength = abs(BposReadInt(locLengthStr.c_str()));
			result += std::string(locLength, ' ');
			result = result.substr(0,locLength);
		}
	}
	catch(...)
	{
		// result is left as is.
	}
	// Determine if a trailer needs to be added to the data 
	try
	{	// Check for the AddTrailer attribute to determine if a trailer should be placed on this data field
		char trailer = BposReadInt(resultMapNode->getAttribute("AddTrailer")->getValue().c_str());
		result += trailer;
	}
	catch(...)
	{	// Leave result as is.
	}
	return(result);
}

//-----------------------------------------------------------------------------
string HostInterface::ProcessDtcNode(const XmlNode *dtcFormat, 
									 const XmlNodeMap &testResults,
									 const XmlNodeMap &vehicleData,
									 const XmlNodeMap &resultConversions,
									 INT32 &totalSerialNumbers)
{
	string result("");
	INT32 currentDtcsAdded = 0;
	// Determine the number of DTCs to report
	INT32 numDtcToReport = -1;	 // Default to report all DTCs
	try
	{
		numDtcToReport = BposReadInt(dtcFormat->getAttribute("MaximumDtcsToReport")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Reporting all DTCs: %s", excpt.GetReason());
		numDtcToReport = -1;
	}
	// Ensure there are DTCs to report
	if(testResults.getNode("DTC")->getChildren().size() > 0)
	{
		bool mapComponentName = false;	// Default to not map component names
		// Determine if component names should be mapped
		try
		{
			mapComponentName = atob(dtcFormat->getAttribute("MapComponentNames")->getValue().c_str());
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Not mapping component names: %s", excpt.GetReason());
			mapComponentName = false;
		}
		string componentName = "";
		try
		{
			componentName = dtcFormat->getAttribute("Component")->getValue().c_str();
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Component Name not specified: %s", excpt.GetReason());
			componentName = "";
		}
		// Determine if a parent node should be reported for each DTC component
		bool reportParentDtcNode = false;
		try
		{
			reportParentDtcNode = atob(dtcFormat->getAttribute("ReportParentNodeDtc")->getValue().c_str());
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Not reporting DTC parent node: %s", excpt.GetReason());
			reportParentDtcNode = false;
		}

		// Process each component node
		const XmlNodeMap *componentNodes = &(testResults.getNode("DTC")->getChildren());
		Log(LOG_DEV_DATA, "Processing DTCs for %d modules", componentNodes->size());
		for(XmlNodeMapCItr compIter = componentNodes->begin(); compIter != componentNodes->end(); compIter++)
		{	// Map the component name if required
			string mappedComponentName = mapComponentName ? GetMappedComponentName(compIter->second->getName()) : compIter->second->getName();
			if(componentName == "" || componentName == compIter->second->getName())
			{
				// Process each component DTC
				const XmlNodeMap *componentDtc = &(compIter->second->getChildren());
				// Report the DTC Parent node if required
				if(reportParentDtcNode)
				{
					Log(LOG_DEV_DATA, "Reporting DTC parent node for %s", compIter->second->getName().c_str());
					result += ProcessDtcParentNode(compIter->second->getName(), testResults, vehicleData, 
												   resultConversions, totalSerialNumbers, mapComponentName);
				}
				// Process each DTC
				Log(LOG_DEV_DATA, "Processing %d DTCs for component %s", componentDtc->size(), mappedComponentName.c_str());
				for(XmlNodeMapCItr dtcIter = componentDtc->begin(); 
				   (dtcIter != componentDtc->end()) && ((numDtcToReport == -1) || (currentDtcsAdded < numDtcToReport));
				   dtcIter++)
				{	// Increment the number of DTCs added and process each format node
					currentDtcsAdded++;
					Log(LOG_DEV_DATA, "Adding DTC %d to result string: %s", currentDtcsAdded, dtcIter->second->getName().c_str());
					string dtcName(mappedComponentName + ":" + dtcIter->second->getName());
					if(find(m_reportedDtcs.begin(), m_reportedDtcs.end(), dtcName) == m_reportedDtcs.end()) 
					{
						m_reportedDtcs.push_back(dtcName);
						XmlNodeMap dtcNode;
						dtcNode.addNode(dtcIter->second);
						Log(LOG_DEV_DATA, "Processing DTC...");
						const XmlNodeMap &formatNodes = dtcFormat->getChildren();
						for(XmlNodeMapCItr formatIter = formatNodes.begin(); formatIter != formatNodes.end(); formatIter++)
						{	// If this is a component name type, add the name as the value of the node
							if(!formatIter->second->getAttribute(fieldType)->getValue().compare(typeComponentName))
							{
								XmlNode *temp = const_cast<XmlNode *>(formatIter->second);
								temp->setValue(mappedComponentName);
							}
							//if this was a space separator - store the result position to allow in final result string
							string type = formatIter->second->getAttributes().getNode(fieldType)->getValue();
							if(type == typeSeparator)
							{
								if(formatIter->second->getValue() == "Blank")
								{
									m_allowedSpacePositions.push_back(result.length());
								}
							}
							// Process this format node
							result += ProcessMapNode(formatIter->second, dtcNode, vehicleData, resultConversions, totalSerialNumbers);
						}
					}
					else
					{
						Log(LOG_DEV_DATA, "DTC %s has already been reported.", dtcName.c_str());
					}
				}
			}
		}
	}
	else
	{
		Log(LOG_DEV_DATA, "No reported DTCs to add to the result string!");
	}
	if(numDtcToReport != -1)
	{
		//determine if we need to fill in blank dtc data
		bool fillBlankDTCData;
		try
		{
			fillBlankDTCData = !(atob(dtcFormat->getAttribute(excludeNotTested)->getValue().c_str()));
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Not filling in blank DTC data: %s", excpt.GetReason());
			fillBlankDTCData = false;
		}
		if(fillBlankDTCData)
		{
			for(; currentDtcsAdded < numDtcToReport; currentDtcsAdded++)
			{
				Log(LOG_DEV_DATA, "Adding Blank DTC %d to result string", currentDtcsAdded+1);
				XmlNodeMap dtcNode;
				dtcNode.addNode(new XmlNode("Component", "HexDTC"));
				Log(LOG_DEV_DATA, "Processing DTC...");
				const XmlNodeMap &formatNodes = dtcFormat->getChildren();
				for(XmlNodeMapCItr formatIter = formatNodes.begin(); formatIter != formatNodes.end(); formatIter++)
				{	//set value to blank
					if(!formatIter->second->getAttribute(fieldType)->getValue().compare(typeComponentName))
					{
						XmlNode *temp = const_cast<XmlNode *>(formatIter->second);
						temp->setValue("xxx");
					}
					//if this was a space separator - store the result position to allow in final result string
					string type = formatIter->second->getAttributes().getNode(fieldType)->getValue();
					if(type == typeSeparator)
					{
						if(formatIter->second->getValue() == "Blank")
						{
							m_allowedSpacePositions.push_back(result.length());
						}
					}
					// Process this format node
					result += ProcessMapNode(formatIter->second, dtcNode, vehicleData, resultConversions, totalSerialNumbers);
				}
			}
		}
	}

	return result;
}

//-----------------------------------------------------------------------------
string HostInterface::ProcessDtcParentNode(const string &componentName,
										   const XmlNodeMap &testResults,
										   const XmlNodeMap &vehicleData,
										   const XmlNodeMap &resultConversions,
										   INT32 &totalSerialNumbers,
										   const bool &mapComponentName)
{
	string result("");
	string name(mapComponentName ? GetMappedComponentName(componentName) : componentName);
	// Use the parent DTC format to generate the result string
	for(XmlNodeMapItr iter = m_parentDtcNodeFormat.begin(); iter != m_parentDtcNodeFormat.end(); iter++)
	{	// If this is a component name field, make sure it is named properly
		if(!iter->second->getAttribute(fieldType)->getValue().compare(typeComponentName))
		{
			XmlNode *temp = const_cast<XmlNode *>(iter->second);
			temp->setValue(name);
		}
		result += ProcessMapNode(iter->second, testResults, vehicleData, resultConversions, totalSerialNumbers);
	}
	// Return the string
	return result;
}

//-----------------------------------------------------------------------------
string HostInterface::GetMappedComponentName(const string &componentName)
{
	XmlNodeMapItr iter = m_componentNameConversions.find(componentName);
	string name = (iter != m_componentNameConversions.end()) ? iter->second->getValue() : componentName;
	Log(LOG_DEV_DATA, "Mapped component name:  %s --> %s", componentName.c_str(), name.c_str());
	return name;
}

//-----------------------------------------------------------------------------
string HostInterface::ConvertResultString(const string &result, const XmlNodeMap &resultConversions)
{
	string reportResult = "Z";
	try
	{	// Get the conversion for the result string
		reportResult = resultConversions.getNode(result)->getValue();
	}
	catch(BepException &excpt)
	{
		Log(LOG_ERRORS, "ConvertResultString(%s) - %s\n", result.c_str(), excpt.what());
		reportResult = "Z";
		Log(LOG_ERRORS, "No conversion specified for %s.  Returning %s for report.\n", result.c_str(), reportResult.c_str());
	}
	return(reportResult);
}

//-----------------------------------------------------------------------------
string HostInterface::GetTestResult(const XmlNodeMap &testResultMap, 
									const XmlNode *resultStringField,
									const XmlNodeMap &resultConversions,
									const bool *useConversion) /*= NULL*/
{
	string result, resultValue;
	// Check to see if the test result was reported
	XmlNodeMapCItr testResultIter;
	bool convert = useConversion != NULL ? *useConversion : true;
	if((testResultIter = testResultMap.find(resultStringField->getValue())) != testResultMap.end())
	{	// Get the test result from the map
		resultValue = testResultIter->second->getAttributes().getNode(BEP_RESULT)->getValue();
	}
	else
	{	// This test was not performed on the current vehicle
		resultValue = "NotTested";
	}
	Log(LOG_DETAILED_DATA, "GetTestResult - %s = %s", resultStringField->getValue().c_str(), resultValue.c_str());
	if(convert)
	{
		result = ConvertResultString(resultValue, resultConversions);
		Log(LOG_DETAILED_DATA, "GetTestResult - %s = %s", resultStringField->getValue().c_str(), resultValue.c_str());
	}
	else
	{
		result = resultValue;
		Log(LOG_DETAILED_DATA, "NOt converting test result value for %s", resultStringField->getValue().c_str());
	}
	// Check if the result string needs to be formatted
	try
	{
		char buf[16];
		string newResult = CreateMessage(buf, sizeof(buf), 
										 resultStringField->getAttribute("Format")->getValue().c_str(), 
										 result.c_str());
		Log(LOG_DEV_DATA, "Formatted result from %s to %s", result.c_str(), newResult.c_str());
		result = newResult;
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "No format found for this node!");
	}

	return(result);
}

//-----------------------------------------------------------------------------
string HostInterface::GetTestResultCode(const XmlNodeMap &testResultMap, 
										const XmlNode *resultStringField) /*= NULL*/
{
	string result;
	// Check to see if the test result was reported
	XmlNodeMapCItr testResultIter;
	if((testResultIter = testResultMap.find(resultStringField->getValue())) != testResultMap.end())
	{	// Get the test result code
		result = testResultIter->second->getAttributes().getNode(BEP_TEST_RESULT_CODE)->getValue();
	}
	else
	{	// This test was not performed on the current vehicle
		result = "0000";
	}
	Log(LOG_DETAILED_DATA, "GetTestResultCode - %s = %s", resultStringField->getValue().c_str(), result.c_str());
	// Check if the result string needs to be formatted
	try
	{
		char buf[16];
		string newResult = CreateMessage(buf, sizeof(buf), 
										 resultStringField->getAttribute("Format")->getValue().c_str(), 
										 result.c_str());
		Log(LOG_DEV_DATA, "Formatted result code from %s to %s", result.c_str(), newResult.c_str());
		result = newResult;
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "No format found for this node!");
	}

	return(result);
}

//-----------------------------------------------------------------------------
string HostInterface::GetTestResultFromValue(string resultValue,
											 string format,
											 const XmlNodeMap &resultConversions,
											 const bool *useConversion)	/*= NULL*/
{
	string result;
	// Check to see if the test result was reported
	bool convert = useConversion != NULL ? *useConversion : true;
	Log(LOG_DETAILED_DATA, "GetTestResult - = %s", resultValue.c_str());
	if(convert)
	{
		result = ConvertResultString(resultValue, resultConversions);
		Log(LOG_DETAILED_DATA, "GetTestResult %s", resultValue.c_str());
	}
	else
	{
		result = resultValue;
		Log(LOG_DETAILED_DATA, "NOt converting test result value");
	}
	// Check if the result string needs to be formatted
	try
	{
		char buf[16];
		string newResult = CreateMessage(buf, sizeof(buf), 
										 format.c_str(), 
										 result.c_str());
		Log(LOG_DEV_DATA, "Formatted result from %s to %s", result.c_str(), newResult.c_str());
		result = newResult;
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "No format found for this node!");
	}

	return(result);
}

//-----------------------------------------------------------------------------
string HostInterface::GetTestDetailValue(const XmlNodeMap &testResultMap, const XmlNode *testResultStringField)
{
	string result;
	INT32 lengthDetail = 0;
	// Leave some room for the result, default to 32 bytes
	result.reserve(32);
	result.erase();
	bool excludeDetail = false;
	bool serialNumberType = false;
	bool detailStringType = false;
	float scale, offset;
	// Get a map of the attributes
	const XmlNodeMap &attributes = testResultStringField->getAttributes();
	XmlNodeMapCItr attributeIter;
	// Get the field format from the node
	string format(attributes.getNode(fieldFormat)->getValue());
	string detailNodeName(attributes.getNode(detailNode)->getValue());
	Log(LOG_DETAILED_DATA, "GetTestDetailValue() - Test Result Node: %s, Format: %s, Detail Node: %s",
		testResultStringField->getValue().c_str(), format.c_str(), detailNodeName.c_str());
	// Attempt to find if this detail should be excluded from the results
	attributeIter = attributes.find(excludeNotTested);
	excludeDetail = (attributeIter != attributes.end()) ? !attributeIter->second->getValue().compare("Yes") : false;
	// Determine the field length
	attributeIter = attributes.find(fieldLength);
	lengthDetail = (attributeIter != attributes.end()) ? BposReadInt(attributeIter->second->getValue().c_str()) : 0;
	// Get the Scale
	attributeIter = attributes.find(fieldScale);
	scale = (attributeIter != attributes.end()) ? atof(attributeIter->second->getValue().c_str()) : 1;
	// Get the offset
	attributeIter = attributes.find(fieldOffset);
	offset = (attributeIter != attributes.end()) ? atof(attributeIter->second->getValue().c_str()) : 0;
	// Get the type of node we are dealing with
	attributeIter = attributes.find(fieldType);
	// Check what type of node we are delaing with
	serialNumberType = !attributeIter->second->getValue().compare(typeSerialNumber);
	detailStringType = !attributeIter->second->getValue().compare(typeDetailString);
	// Get the type of data we are dealing with.  NOTE: default to string if not specified
	attributeIter = attributes.find(fieldDataType);
	string dataType = attributeIter != attributes.end() ? attributeIter->second->getValue() : typeString;
	// Get the detail node from the test result node
	XmlNodeMapCItr testResultIter = testResultMap.find(testResultStringField->getValue());
	// note that the buffer below has to be bigger than any string field in use
	char buf[128];
	bool testDetailReported = false;
	if(dataType == BEP_RESULT)
	{
		string dataValue;
		if(testResultIter != testResultMap.end())
		{
			dataValue = testResultIter->second->getChild(detailNodeName)->getValue();
		}
		else
		{

			dataValue = "NotTested";
		}
		result = GetTestResultFromValue(dataValue, format, m_resultConversions);
	}
	else
	{
		if(testResultIter != testResultMap.end())
		{
			// Get the detail node and translate to appropraite type
			try
			{
				// store the data value to format
				string dataValue = testResultIter->second->getChild(detailNodeName)->getValue();
				Log(LOG_DEV_DATA, "Scale: %05.5f Offset: %05.5f Value: %s %05.5f Calculated value: %05.5f",scale, offset,
					dataValue.c_str(), atof(dataValue.c_str()), (((atof(dataValue.c_str())) * scale) + offset));
				result = ApplyDataFormatting(dataType,dataValue,format,lengthDetail,scale, offset);

				// if there were no errors formatting the result
				if(result != "")
				{
					testDetailReported = true;
				}
			}
			catch(XmlException &e)
			{
				Log(LOG_ERRORS, "Test result detail not reported: %s", e.what());
				result.clear();
			}
		}
		// Handle the result if it was not reported
		if(!testDetailReported)
		{	// Test result was not reported, determine how to fill the field
			if(excludeDetail)
			{
				result.erase();
			}
			else if(!dataType.compare(typeInt) || !dataType.compare(typeFloat))
			{
				result = CreateMessage(buf, sizeof(buf), format.c_str(), 0.0);
			}
			else if(!dataType.compare(typeString))
			{
				result = CreateMessage(buf, sizeof(buf), format.c_str(), "");
				if(lengthDetail > (INT32)result.length())
				{
					Log(LOG_DEV_DATA, "GetTestDetailValue(), data type string and result output string too small, padding with zeros");
					string pad;
					pad.assign(lengthDetail - result.length(), '0');
					result = pad + result;
				}
			}
			else
			{
				result.erase();
			}
		}
	}
	// Reutrn the final result
	Log(LOG_FN_ENTRY, "GetTestDetailValue() - Returning: %s", result.c_str());
	return(result);
}

//-----------------------------------------------------------------------------
string HostInterface::ApplyDataFormatting(string dataType, 
										  string dataValue, 
										  string formatString,
										  INT32 fieldLength,
										  float scale,
										  float offset,
										  string padChar)/*="0"*/
{
	string retVal;
	retVal.erase();
	char tempBuffer[128];
	// if a field length wasn't specified, try to get it from the format
	fieldLength = (fieldLength == 0) ? GetLengthFormatType(formatString) : fieldLength;
	// if we have a valid field length - negative is ok for strings
	if((fieldLength > 0) || (dataType == typeString))
	{
		if(dataType == typeString)	  // process string values
		{
			retVal = CreateMessage(tempBuffer,sizeof(tempBuffer),formatString.c_str(),dataValue.c_str());
		}
		else if(dataType == typeASCIIInt)
		{
			if((offset >= 0) && (offset < dataValue.length()))
			{//start string at offset
				if((fieldLength + offset) <= dataValue.length())
				{
					//get the characters specified by offset and fieldLength
					dataValue = dataValue.substr((INT32)offset,fieldLength);
					for(int i = 0; i < fieldLength; i++)
					{//convert each character to hex ascii
						retVal += CreateMessage(tempBuffer,sizeof(tempBuffer),formatString.c_str(),dataValue[i]);
					}
					//convert hex value #### to decimal and adjust field length
					UINT16 x = atoh(retVal.c_str());
					retVal = CreateMessage(tempBuffer,sizeof(tempBuffer), "%d", x);
					//double field length to adjust for ascii interp. size
					fieldLength = retVal.length();
				}
				else
				{
					Log(LOG_ERRORS, "Field length out of data range %s fieldlength: %d offset: %f dataValue: %s dataLength: %d",
						dataType.c_str(), fieldLength, offset, dataValue.c_str(), dataValue.length());
				}
			}
			else
			{
				Log(LOG_ERRORS, "Offset out of data range dataType: %s offset: %f dataValue: %s dataLength: %d",
					dataType.c_str(), offset, dataValue.c_str(), dataValue.length());
			}
		}
		// process integer and float values
		else if((dataType == typeInt) || (dataType == typeFloat))
		{
			retVal = ((dataType == typeInt) ?
					  CreateMessage(tempBuffer,sizeof(tempBuffer),formatString.c_str(),
									int((((atof(dataValue.c_str())) * scale) + offset))) :
					  CreateMessage(tempBuffer,sizeof(tempBuffer),formatString.c_str(),
									(((atof(dataValue.c_str())) * scale) + offset)));
		}
		else
		{
			Log(LOG_ERRORS, "Unknown data type %s passed to ApplyDataFormatting()",dataType.c_str());
			retVal.erase();
		}

		if(!retVal.empty())	   // if we have a valid result
		{
			// trim or pad the data value if necessary
			retVal = AdjustDataValueLength(dataType,retVal,fieldLength,padChar);
		}
	}
	else
	{
		Log(LOG_ERRORS,"No length specified in the format string or length attribute");
		retVal.erase();
	}
	// Return the formatted result
	return(retVal);
}

//------------------------------------------------------------------------------
string HostInterface::AdjustDataValueLength(string dataType, string dataValue, INT32 length, string padChar)
{
	bool valueAboveMax=false;
	// NOTE:  There is no special processing needed for strings.  The pad/trim
	//        will be done below
	if((dataType == typeInt) || (dataType == typeFloat))
	{
		if(dataType == typeInt)
		{
			// use the data length to see if the value is above the maximum
			if((INT32)dataValue.length() > length)
			{
				valueAboveMax = true;
			}
		}
		else	// float data type
		{
			// if the value is above the maximum allowable value, set the flag
			if(atof(dataValue.c_str()) > pow((float)10.0,length))
			{
				valueAboveMax = true;
			}
		}

		// if the result is greater than the highest possible value
		if(valueAboveMax == true)
		{
			Log(LOG_ERRORS,"Result string %s is greater than the "
				"specified field length %d.  Replacing with the largest"
				"possible field value",dataValue.c_str(),length);
			dataValue.erase();
			dataValue.assign(length, '9');
		}
	}
	else if((dataType == typeString) && (length < 0))
	{
		Log(LOG_DEV_DATA, "Received negative length %d, updating to %d", length, abs(length));
		length = abs(length);
		string tempData(dataValue.substr(dataValue.length() - length));
		Log(LOG_DEV_DATA, "Original data value: %s - New data value: %s", dataValue.c_str(), tempData.c_str());
		dataValue.erase();
		dataValue.resize(tempData.length());
		dataValue = tempData;
	}
	else if((dataType != typeString) && (dataType != typeASCIIInt))
	{
		Log(LOG_ERRORS,"Unknown data type %s passed to AdjustDataValueLength()", dataType.c_str());
		dataValue = "";
	}

	// pad the data value if necessary
	if(length > (INT32)dataValue.length())
	{
		Log(LOG_DEV_DATA, "Data string is smaller than field length Padding with %s",padChar.c_str());
		string pad;
		pad.assign(length - dataValue.length(), padChar[0]);
		dataValue = pad + dataValue;
	}
	// trim the data value if necessary
	else if((INT32)dataValue.length() > length)
	{
		Log(LOG_DEV_DATA, "Data string is larger than field length - field length: %d", length);
		dataValue.resize(length);
	}

	return(dataValue);
}

//------------------------------------------------------------------------------
string HostInterface::GetFormatAttribute(const XmlNode* resultMapNode)
{
	string format;

	try
	{	// Try to read the format field from the node
		format = resultMapNode->getAttribute(fieldFormat)->getValue();
	}
	catch(XmlException& ex)
	{	// the format attribute does not exist, default to a string
		format="%s";
	}

	return(format);
}

//------------------------------------------------------------------------------
string HostInterface::GetDataTypeAttribute(const XmlNode* resultMapNode)
{
	string dataType;

	try
	{	// try to read the data type from the node
		dataType = resultMapNode->getAttribute(fieldDataType)->getValue();
	}
	catch(XmlException& ex)
	{	// the data type attribute does not exist, default to a string
		dataType = typeString;
	}

	return(dataType);
}

//------------------------------------------------------------------------------
INT32 HostInterface::GetDataLengthAttribute(const XmlNode* resultMapNode)
{
	INT32 length;

	try
	{	// try to read the length from the node
		length = atoi(resultMapNode->getAttribute(fieldLength)->getValue().c_str());
	}
	catch(XmlException& ex)
	{	// the length attribute does not exist, default to 0
		length = 0;
	}

	return(length);
}

//------------------------------------------------------------------------------
float HostInterface::GetScaleAttribute(const XmlNode* resultMapNode)
{
	float scale;

	try
	{	// try to read the length from the node
		scale = atof(resultMapNode->getAttribute(fieldScale)->getValue().c_str());
	}
	catch(XmlException& ex)
	{	// the length attribute does not exist, default to 0
		scale = 1;
	}

	return(scale);
}

//------------------------------------------------------------------------------
float HostInterface::GetOffsetAttribute(const XmlNode* resultMapNode)
{
	float offset;

	try
	{	// try to read the length from the node
		offset = atof(resultMapNode->getAttribute(fieldOffset)->getValue().c_str());
	}
	catch(XmlException& ex)
	{	// the length attribute does not exist, default to 0
		offset = 0;
	}

	return(offset);
}

//-----------------------------------------------------------------------------
INT32 HostInterface::GetLengthFormatType(string format)
{
	INT32 length = 0;
	UINT16 percentPosition;
	UINT16 formatPosition;
	UINT16 lengthPosition;
	char formatChar;
	bool boolFoundFormatChar = false;

	// Determine the start of the format string and verify it is valid
	percentPosition = format.find("%");
	// Log debugging data
	Log(LOG_DETAILED_DATA, "GetLengthFormatType():  PercentPosition = %d\n", percentPosition);
	Log(LOG_DETAILED_DATA, "GetLengthFormatType():  Format string length = %d", format.length());
	// Check if there is a percent sign in the format string to verify it is a good string
	if(format.npos != percentPosition)
	{	// Use the formatPosition integer to locate a valid format type
		formatPosition = format.length() - 1;
		Log(LOG_DETAILED_DATA, "GetLengthFormatType(): Current Format Char Position = %d", formatPosition);
		// Find format character and ensure it is a valid one
		while((percentPosition != formatPosition) && (true != boolFoundFormatChar))
		{
			Log(LOG_DETAILED_DATA, "GetLengthFormatType(): While Loop, percentPosition=%d, "
				"formatPosition=%d, boolFoundFormatChar=%s, formatChar=%c", percentPosition, formatPosition,
				(boolFoundFormatChar ? "True" : "False"), format[formatPosition]);
			// Extract current character from string for checking
			formatChar = format[formatPosition];
			// Check character
			switch(formatChar)
			{
			case 'f':
				boolFoundFormatChar = true;
				break;
			case 'd':
				boolFoundFormatChar = true;
				break;
			case 'i':
				boolFoundFormatChar = true;
				break;
			case 'X':
				boolFoundFormatChar = true;
				break;
			case 's':
				boolFoundFormatChar = true;
				break;
			default:
				formatPosition -= 1;
				break;
			}
		}

		Log(LOG_DETAILED_DATA, "GetLengthFormatType(): Found Format Char = %s",(boolFoundFormatChar ? "True" : "False"));
		if(true == boolFoundFormatChar)
		{	// Check to see if a decimal point exists
			lengthPosition = (format.find(".") != format.npos) ? (format.find(".") - 1) : (formatPosition - 1);
			Log(LOG_DETAILED_DATA, "GetLengthFormatType(): Length Position = %d", lengthPosition);
			// Ensure information makes since
			if(lengthPosition >= percentPosition + 1)
			{
				if(format[percentPosition+1] == '-')
				{
					// extract length from format attribute
					length = atoi((format.substr(percentPosition+2,lengthPosition-(percentPosition+1))).c_str());
				}
				else
				{
					// extract length from format attribute
					length = atoi((format.substr(percentPosition+1,lengthPosition-percentPosition)).c_str());
				}

				if(length > 0)
				{
					Log(LOG_DETAILED_DATA, "GetLengthFormatType(): Valid length Attribute found - %d", length);
				}
				else
				{
					Log(LOG_ERRORS, "GetLengthFormatType(): WARNING, Length for current string %s, "
						"is less than or 0, length = %d", format.c_str(), length);
					Log(LOG_ERRORS, "GetLengthFormatType(): Setting length to zero.");
					length = 0;
				}
			}
			else
			{
				Log(LOG_ERRORS, "GetLengthFormatType(): WARNING, no length specified for Format string - %s", format.c_str());
				Log(LOG_ERRORS, "GetLengthFormatType(): Setting length to zero.");
				length = 0;
			}
		}
		else
		{
			Log(LOG_ERRORS, "GetLengthFormatType(): ERROR, Invalid format string, no format character found - %s",format.c_str());
			Log(LOG_ERRORS, "GetLengthFormatType(): Setting length to -1");
			length = -1;
		}
	}
	else
	{
		Log(LOG_ERRORS, "GetLengthFormatType(): ERROR, Invalid format string, no percent sign found - %s",format.c_str());
		Log(LOG_ERRORS, "GetLengthFormatType(): Setting length to -1");
		length = -1;
	}
	// return length
	return(length);
}

//-----------------------------------------------------------------------------
const INT32& HostInterface::VinIndex(INT32 *index) /*= NULL*/
{
	if(index != NULL)  m_vinIndex = *index;
	return m_vinIndex;
}

//-----------------------------------------------------------------------------
const string& HostInterface::MessageVin(string *vin) /*= NULL*/
{
	if(vin != NULL)	 m_msgVin = *vin;
	return m_msgVin;
}

//-----------------------------------------------------------------------------
const INT32& HostInterface::MaxHostResponseTime(INT32 *maxResponseTime)	/*= NULL*/
{
	if(maxResponseTime != NULL)	 m_maxHostResponseTime = *maxResponseTime;
	return m_maxHostResponseTime;
}

//-----------------------------------------------------------------------------
const INT32& HostInterface::MaxMsgSendAttempts(INT32 *maxAttempts) /*= NULL*/
{
	if(maxAttempts != NULL)	 m_maxAttempts = *maxAttempts;
	return m_maxAttempts;
}

//-----------------------------------------------------------------------------
const XmlNodeMap& HostInterface::ResultMap(const XmlNode *baseMap) /*= NULL*/
{
	if(baseMap != NULL)	 m_resultMap.DeepCopy(baseMap->getChildren());
	return m_resultMap;
}

//-----------------------------------------------------------------------------
const XmlNodeMap& HostInterface::ResultConversions(void)
{
	return m_resultConversions;
}

//-----------------------------------------------------------------------------
void HostInterface::SetResultStrings(const XmlNode *resultStrings, XmlNodeMap &resultStringMap)
{	// Make sure the node is not NULL
	if(resultStrings != NULL) resultStringMap.DeepCopy(resultStrings->getChildren());
	else resultStringMap.clear(true);
}

//-----------------------------------------------------------------------------
const UINT8& HostInterface::MessageId(UINT8 *id) /*= NULL*/
{
	if(id != NULL)	m_msgId = *id;
	return m_msgId;
}

//-----------------------------------------------------------------------------
const string& HostInterface::HostInterfaceName(const string *pathName) /*= NULL*/
{
	if(pathName != NULL)  m_hostInterfaceName = *pathName;
	return m_hostInterfaceName;
}

//-----------------------------------------------------------------------------
SerialChannel* HostInterface::HostComm(void)
{
	return m_plantHostComm;
}

//-----------------------------------------------------------------------------
const bool HostInterface::ShouldSendResultToHost(void)
{
	XmlParser parser;
	const XmlNode *configData = parser.ReturnXMLDocument(GetConfigFile());
	string sendResultNodeName("Setup/HostInterfaces/" + HostSystemName() + "/Setup/SendResultsToHost");
	string sendResult(configData->getChild(sendResultNodeName)->getValue());
	Log(LOG_DEV_DATA, "HostInterface: Config data send to host - %s", sendResult.c_str());
	bool sendToHost = !stricmp(sendResult.c_str(), sendResultsToHost.c_str());
	return(sendToHost);
}

//-----------------------------------------------------------------------------
void HostInterface::SendResultToHost(const string &result)
{	// Get the VIN to include in the message
	string message;
	message.reserve(2048);
	if(VinIndex() < 0)
	{
		message = MessageVin();
		Log(LOG_DEV_DATA, "Using VIN from build data: %s", MessageVin().c_str());
	}
	else
	{	// Get the vin from the result string
		message = result.substr(VinIndex(), vinLength);
		Log(LOG_DEV_DATA, "Using VIN from message: %s", result.substr(VinIndex(), vinLength).c_str());
	}
	// Add the message to transmit
	message += result;
	// Add the message length to the message
	char msgLength[5];
	sprintf(msgLength, "%04d", message.length());
	message = msgLength + message;
	// Calculate and add the checksum to the message along with the wrapper
	UINT16 checksum = CalculateCheckSum(message) & 0x00FF;
	Log(LOG_DEV_DATA, "HostInterface: Message to send to host system - <%02X>%s<%02X><%02X><%02X>",
		MessageId(), message.c_str(), checksum, CR, LF);
	message = (char)MessageId() + message + (char)checksum + (char)CR + (char)LF;
	// Transmit the result string to the host system
	BEP_STATUS_TYPE status = SendTestResultString(message, *(HostComm()), ACK, MaxMsgSendAttempts(), MaxHostResponseTime());
	Log(LOG_ERRORS, "Send result to host system: %s", ConvertStatusToResponse(status).c_str());
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE HostInterface::SendTestResultString(string &resultString,
														  SerialChannel &portComm,
														  const unsigned char ackChar,
														  const INT32 &maxAttempts,	/*= 3*/
														  const INT32 &hostResponseTime) /*= 1000*/
{
	bool portLocked = false;				// Port locked status
	bool ackReceived = false;				// ACK received status
	bool nakReceived = false;				// NAK received status
	unsigned char hostAck = 0xFF;			// ACK from the external host
	INT32 byteCount = 0;					// Number of bytes recieved
	BEP_STATUS_TYPE transmitStatus = BEP_STATUS_ERROR;// Status of message transmission
	INT32 attempt = 1;						// Current attempt
	// Attempt to transmit the message
	do
	{	// Lock the port so no other comms interfere
		portLocked = portComm.LockPort();
		if(portLocked)
		{	// Transmit the message and wait for the host to respond
			portComm.ResetConnection();
			transmitStatus = portComm.Send(resultString);
			BposSleep(hostResponseTime);
			// Begin looking for an acknowledgement
			if(transmitStatus == BEP_STATUS_SUCCESS)
			{	// Get any response that was sent
				byteCount = 0;
				// keep total loop time true to parameter
				nakReceived = false;
				// Get the response from the host
				byteCount = portComm.Get(&hostAck, 1);
				Log(LOG_DEV_DATA, "Read 0x%02X from the %s port", hostAck, portComm.GetName().c_str());
				// Check for an ACK message
				if((byteCount > 0) && (hostAck == ackChar))
				{
					ackReceived = true;
				}
				else if((byteCount > 0) && (hostAck == NAK))
				{
					nakReceived = true;
				}
				Log(LOG_DEV_DATA, "Results sent, ackReceived: %s, nakReceived: %s", 
					(ackReceived ? "TRUE" : "FALSE"), (nakReceived ? "TRUE" : "FALSE"));
			}
			else
			{	// Error transmitting the result to the host
				Log(LOG_ERRORS, "Error transmitting message -- status: %s", ConvertStatusToResponse(transmitStatus).c_str());
			}
			// Unlock the port since we have completed this attempt
			portLocked = portComm.UnlockPort();
		}
		else
		{	// Log the error
			Log(LOG_ERRORS, "WARNING: Could not lock %s port.  Not transmitting results\n", portComm.GetProcessName().c_str());
			BposSleep(100);		  //keep ferom locking up if port is not released to use.
		}
	} while(!ackReceived && (attempt++ <= maxAttempts));
	// Set the return status
	if(!ackReceived)
	{	// No ack received, set fail status
		Log(LOG_ERRORS, "No ACK message from host on %s port\n", portComm.GetProcessName().c_str());
		transmitStatus = BEP_STATUS_FAILURE;
	}
	else
	{	// ACK message received, set status to success
		Log(LOG_DEV_DATA, "Message transmit to host on %s port successful\n", portComm.GetProcessName().c_str());
		transmitStatus = BEP_STATUS_SUCCESS;
	}
	// Return the status
	return(transmitStatus);
}

//-----------------------------------------------------------------------------
bool HostInterface::ShouldProcessTestResults(const XmlNode *results)
{	// If there is no vehicle build node, this is a Loss Compensation result, do not process
	bool processResults = true;
	bool isLossComp = false;
	bool isPfsAbort = false;
	try
	{
		isLossComp = !(results->getChildren().find(VEHICLE_BUILD_TAG) != results->getChildren().end());
		Log(LOG_DEV_DATA, "HostInterface: Is result from LossComp: %s", isLossComp ? "True" : "False");
		if(isLossComp)
		{
			processResults = false;
		}
        else if(!SkipPerformAbortCheck())
		{	// Check if the perform/abort flag is set to perform
			isPfsAbort = results->getChild(VEHICLE_BUILD_TAG)->getChild("PerformAbortTest")->getValue().compare("Perform");
			Log(LOG_DEV_DATA, "HostInterface: Is Abort from PFS: %s", isPfsAbort ? "True" : "False");
			Log(LOG_DEV_DATA, "HostInterface: Is Reporting PFS Aborts: %s", ReportPfsAborts() ? "True" : "False");
			if(isPfsAbort && !ReportPfsAborts())
			{
				processResults = false;
			}
		}
        try
        {//if intermediate results tag exists check for flag to report
          bool isIntermediateResult = results->getChild(BEP_TEST_RESULT)->getChildren().find(BEP_INTERMEDIATE_OVERALL_RESULT) != 
              results->getChild(BEP_TEST_RESULT)->getChildren().end();
          if (!ReportIntermediateResults() && isIntermediateResult)
          {
              Log(LOG_DETAILED_DATA, "Skipping process results Parameter ReportIntermediateResults: %s, Intermediate Overall result node Present: %s",
                      ReportIntermediateResults() ? "True" : "False",isIntermediateResult ?   "True" : "False");
              processResults = false;
          }
        }
        catch(XmlException &excpt)
        {
            Log(LOG_DEV_DATA, "HostInterface: IntermediateOverallResult tag not found, not checking if should report - %s",
                excpt.GetReason());
        }
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "HostInterface: XmlException determining if results should be processed, processing results - %s",
			excpt.GetReason());
		processResults = false;
	}
	Log(LOG_DEV_DATA, "HostInterface::ShouldProcessResults returning %s", processResults ? "True" : "False");
	return processResults;
}

//-----------------------------------------------------------------------------
const bool HostInterface::ReportPfsAborts(void)
{
	XmlParser parser;
	const XmlNode *configData = parser.ReturnXMLDocument(GetConfigFile());
	string reportResultNodeName("Setup/HostInterfaces/" + HostSystemName() + "/Setup/ReportPfsAborts");
	string reportResult;
	try
	{
		reportResult = configData->getChild(reportResultNodeName)->getValue();
	}
	catch(XmlException &excpt)
	{
		reportResult = "No";
	}
	Log(LOG_DEV_DATA, "HostInterface: Config data Report PFS Aborts - %s", reportResult.c_str());
	bool reportPfsAbort = !stricmp(reportResult.c_str(), "Yes");
	return(reportPfsAbort);
}

//-----------------------------------------------------------------------------
string HostInterface::GenerateHostResultString(const XmlNode *testResults)
{
	INT32 total = 0;
	return BuildTestResultString(testResults, ResultMap(), ResultConversions(), total);
}

//-----------------------------------------------------------------------------
const string& HostInterface::HostSystemName(string *name) /*= NULL*/
{
	if(name != NULL)  m_hostSystemName = *name;
	return m_hostSystemName;
}

//-----------------------------------------------------------------------------
const string HostInterface::GetMachineNumber(void)
{
	return m_machineNumber;
}
//-----------------------------------------------------------------------------
const bool& HostInterface::TestDataFileArchiveEnabled(const bool *archive /*= NULL*/)
{
	if(archive != NULL)  m_archiveResultFiles = *archive;
	return m_archiveResultFiles;
}

//-----------------------------------------------------------------------------
const bool& HostInterface::SkipPerformAbortCheck(const bool *skip /*= NULL*/)
{
	if(skip != NULL)  m_skipPerformAbortCheck = *skip;
	return m_skipPerformAbortCheck;
}

//-----------------------------------------------------------------------------
const bool& HostInterface::ReportIntermediateResults(const bool *reportIntermediateResults /*= NULL*/)
{
	if(reportIntermediateResults != NULL)  m_reportIntermediateResults = *reportIntermediateResults;
	return m_reportIntermediateResults;
}

//-----------------------------------------------------------------------------
void HostInterface::LogTestResultData(const XmlNode *data)
{
	// Log the data being used to create the result string
	try
	{
		Log(LOG_DEV_DATA, "Reported Vehicle Build Data:");
		for(XmlNodeMapCItr iter = data->getChild(VEHICLE_BUILD_TAG)->getChildren().begin(); 
		   iter != data->getChild(VEHICLE_BUILD_TAG)->getChildren().end(); 
		   iter++)
		{
			Log(LOG_DEV_DATA, "\t%s - %s", iter->second->getName().c_str(), iter->second->getValue().c_str());
		}
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Error logging vehicle build data from the test result: %s", excpt.GetReason());
	}
	try
	{
		Log(LOG_DEV_DATA, "Reported Test Result Data:");
		for(XmlNodeMapCItr iter = data->getChild(BEP_TEST_RESULT)->getChildren().begin(); 
		   iter != data->getChild(BEP_TEST_RESULT)->getChildren().end(); 
		   iter++)
		{
			Log(LOG_DEV_DATA, "\t%s", iter->second->ToString().c_str());
		}
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Error logging test result data from the test result: %s", excpt.GetReason());
	}
}

//-----------------------------------------------------------------------------
const bool& HostInterface::ResultReadyToProcess(const bool *resultReady) /*= NULL*/
{
	if(resultReady != NULL)	 m_resultReadyToProcess = *resultReady;
	return m_resultReadyToProcess;
}

//-----------------------------------------------------------------------------
void HostInterface::StoreMachineNumber(void)
{
	// Add the machine number to the header string
	Log(LOG_FN_ENTRY,"HostInterface::StoreMachineNumber() - Enter\n");
	string machineNumber, response, tag;
	if(m_ndb != NULL)
	{	// Get the machine number returned from our read
		m_ndb->Read(GetDataTag("MachineNumber"), response, true);
		Log(LOG_DEV_DATA, "Read %s - response: %s\n", GetDataTag("MachineNumber").c_str(), response.c_str());
		m_ndb->GetNext(tag, machineNumber, response);
		if(machineNumber == BEP_UNAVAILABLE_RESPONSE)
		{	// There was a problem with the read
			machineNumber = "0";
			Log(LOG_ERRORS, "Error reading machine number, using %s instead", machineNumber.c_str());
		}
	}
	else
	{	// No INamedDataBroker object, cannot get machine number from the system
		Log(LOG_ERRORS, "INamedDataBroker communication object does not exist, cannot read machine number from the system.");
		machineNumber = "0";
	}
	// Store the machine number to use
	m_machineNumber = machineNumber;
	Log(LOG_DEV_DATA, "MachineNumber: %s", machineNumber.c_str());
}

void HostInterface::SetKeepAliveConfig(const XmlNode *config)
{
	if(config != NULL) m_keepAliveConfig = config->Copy();
	else m_keepAliveConfig = NULL;
}

const XmlNode* HostInterface::GetKeepAliveConfig(void)
{
	return(m_keepAliveConfig);
}

