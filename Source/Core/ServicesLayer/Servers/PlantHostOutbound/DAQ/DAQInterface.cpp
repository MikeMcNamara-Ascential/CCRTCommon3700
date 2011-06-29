//******************************************************************************
// Description:
//
//
//===========================================================================
// Copyright (c) 2002-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/PlantHostOutbound/DAQ/DAQInterface.cpp $
// 
// 1     5/01/07 1:01p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
// 
// 5     2/22/07 11:16a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added a check for m_testResultNodeInUse in NeedToProcessTestResults().
// 
// 4     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/18/06 2:20p Rwiersem
// A SerialChannel object is now passed to the constructor.  This allows
// us to use a MockSerialChannel for testing.  Added
// LoadAdditionalConfigurationItems() which is used to reload the
// configuration without restarting the process.
//
// 3     3/30/06 1:41a Cward
// Updated for sending machine faults to the DAQ. PE #80
//
// 2     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 1     7/21/04 10:18a Cward
//
// 3     5/25/04 4:31p Cward
// Made sending of results to the Data Collector optional based on
// parameter.Updated handling of test results to be more robust
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     1/28/04 12:47p Cward
//
// 1     8/21/03 9:08a Derickso
// 05WK CCRT Project
//
// 1     7/08/03 1:00p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 2     6/24/03 7:59a Khykin
// Updated with Genes latest changes.
//
// 1     6/20/03 9:58a Khykin
// Initial checkin.
//
// 1     6/12/03 4:00p Khykin
// Initial checkin.
//
// 1     4/22/03 9:49a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//
// 5     10/22/02 3:55p Derickso
// Removed compile warnings.
//
// 4     10/07/02 1:09p Derickso
// Corrected setting of Machine number in SetResultStringHeader()
//
// 3     9/16/02 2:48p Derickso
// Added diagnostics to aid debugging.
//
// 2     8/14/02 4:34p Skypig13
// Removed compiler warnings
//
// 1     7/29/02 8:20a Dan.erickson
// Created.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "DAQInterface.h"

//-----------------------------------------------------------------------------
DAQInterface::DAQInterface() : HostInterface(), m_faultReadyToProcess(false)
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
DAQInterface::~DAQInterface()
{   // Clear all maps
    m_lossCompensationMap.clear(true);
}

//-----------------------------------------------------------------------------
void DAQInterface::LoadAdditionalConfigurationItems(const XmlNode *config)
{   // Begin by initializing the base map
    HostInterface::LoadAdditionalConfigurationItems(config);
    Log(LOG_FN_ENTRY, "DAQInterface::LoadAdditionalConfigurationItems() - Enter");
    // Store the length of the machine number field
    INT32 length = 4;
    try
    {
        length = BposReadInt(config->getChild("Setup/MachineNumberLength")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find length of machine number field, using %d: %s", length, excpt.GetReason());
    }
    MachineNumberLength(&length);
    // Store the measurement system in use
    string measurementSystem("E");
    try
    {
        measurementSystem = config->getChild("Setup/MeasurementSystem")->getValue();
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find measurement system, using %s: %s", measurementSystem.c_str(), excpt.GetReason());
    }
    MeasurementSystem(&measurementSystem);
    // Store the Loss Compensation result string map
    try
    {
        LossCompResultStringMap(config->getChild("Setup/LossCompensationResultMap"));
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not load loss compensation result map: %s", excpt.GetReason());
    }
    // Setup the various headers
    SetResultStringHeaders();
    Log(LOG_FN_ENTRY, "DAQInterface::LoadAdditionalConfigurationItems() - Exit");
}

//-----------------------------------------------------------------------------
void DAQInterface::Run(volatile bool *terminateFlag /*=NULL*/)
{
    bool notReady = false;
    bool ready = true;
    while(GetStatus() != BEP_TERMINATE)
    {   // Wait for a result or fault to be written to us
        if(ResultReadyToProcess())
        {//Check list to determine if a test result is available note: popTestResult() deletes record from list  
            if((m_testResult = PopTestResult()) != NULL)
            {   // Indicate result is not ready to be processed
                ResultReadyToProcess(&notReady);
                // Process the new result
            ProcessTestResults();
                if(m_testResult != NULL)  delete m_testResult;
                m_testResult = NULL;
                ResultReadyToProcess(&ready);
            }
            else
            {   // Wait for a bit for the result to appear
                BposSleep(1000);
            }
        }
        else if(FaultReadyToProcess())
        {   // Process the new fault code
            ProcessFaults();
            // Indicate faults are not ready to be processed
            FaultReadyToProcess(&notReady);
        }
        else
        {   // Wait for a bit for the result to appear
            BposSleep(1000);
        }
    }
}

//-----------------------------------------------------------------------------
const string DAQInterface::Write(const string &tag, const string &value)
{   // Nothing special, just call the base class
    return HostInterface::Write(tag, value);
}

//-----------------------------------------------------------------------------
const string DAQInterface::Write(const XmlNode *node)
{   // Determine what type of data this is and how it should be processed
    string result(BEP_ERROR_RESPONSE);
    static bool readyToProcess = true;
    Log(LOG_FN_ENTRY, "DAQInterface::Write(%s) - Enter", node->getName().c_str());
    if(!node->getName().compare(faultCodeTag))
    {   // Process a fault code from the PlantHostOutbound server
        INT32 faultCode = BposReadInt(node->getValue().c_str());
        FaultCode(&faultCode);
        FaultReadyToProcess(&readyToProcess);
        result = BEP_SUCCESS_RESPONSE;
    }
    else
    {   // Call the base class to handle everything else
        result = HostInterface::Write(node);
    }
    // Log the exit ad return the result
    Log(LOG_FN_ENTRY, "DAQInterface::Write(%s) - Exit", node->getName().c_str());
    return result;
}

//-----------------------------------------------------------------------------
bool DAQInterface::ShouldProcessTestResults(const XmlNode *results)
{
    Log(LOG_FN_ENTRY, "DAQInterface::ShouldProcessTestResults() - Enter");
    // Determine the type of result we are dealing with
    bool processResults = HostInterface::ShouldProcessTestResults(results);
    if(!processResults)
    {   // Check for a loss compensation result - Assumption: loss comp result has no vehicle build node
        processResults = results->getChildren().find(VEHICLE_BUILD_TAG) == results->getChildren().end();
    }
    Log(LOG_FN_ENTRY, "DAQInterface::ShouldProcessTestResults() - Exit: %s", processResults ? "True" : "False");
    return processResults;
}

//-----------------------------------------------------------------------------
string DAQInterface::GenerateHostResultString(const XmlNode *testResults)
{
    INT32 junk = 0;
    bool useVehicleBuild;
    string result;
    result.reserve(1024);
    const XmlNodeMap *resultStringMap = NULL;
    Log(LOG_FN_ENTRY, "DAQInterface::GenerateHostResultString() - Enter");
    // Determine what type of result string to process
    if(testResults->getChildren().find(VEHICLE_BUILD_TAG) != testResults->getChildren().end())
    {   // Normal test result, process using test result map
        result = GetTestResultHeader();
        resultStringMap = &(ResultMap());
        useVehicleBuild = true;
    }
    else
    {   // This is a loss comp result, use loss comp map
        result = GetLossCompensationHeader();
        resultStringMap = &(LossCompResultStringMap());
        useVehicleBuild = false;
    }
    result += BuildTestResultString(testResults, *resultStringMap, ResultConversions(), junk, useVehicleBuild, false);
    // Log the exit and return the result string
    Log(LOG_FN_ENTRY, "DAQInterface::GenerateHostResultString() - Exit");
    return result;
}

//-----------------------------------------------------------------------------
void DAQInterface::ProcessFaults(void)
{
    string daqResult;
    char formattedCode[6];
    bool sendResultsToDAQ = true;
    INT32 faultCode;
    Log(LOG_FN_ENTRY, "DAQInterface::ProcessFaults() - Enter");
    try
    {
       char locSign;
       daqResult = GetMachineFaultHeader();
       Log(LOG_DEV_DATA, "Adding Fault Header - %s", daqResult.c_str());
       // Pull data from global area and insert
       if(FaultCode() < 0)
       {
          locSign = '-';
          faultCode = 0 - FaultCode();
       }
       else
       {
          locSign = '+';
       }
       if(FaultCode() > 999) faultCode = 999;
       sprintf(formattedCode, "%03d%c ", faultCode, locSign);
       daqResult += formattedCode;
       sendResultsToDAQ = true;
    }
    catch(XmlException &excpt)
    {   // XmlException processing test results
       Log(LOG_ERRORS, "XmlException processing machine faults - %s", excpt.GetReason());
       Log(LOG_ERRORS, "Faults not sent to DataCollector!");
       sendResultsToDAQ = false;
    }
    catch(...)
    {   // Exception processing test results
       Log(LOG_ERRORS, "Unknown exception processing machine faults");
       Log(LOG_ERRORS, "Faults not sent to DataCollector!");
       sendResultsToDAQ = false;
    }
    // Send the result to the DataCollector
    if(sendResultsToDAQ)  SendResultToHost(daqResult);
}

//-----------------------------------------------------------------------------
void DAQInterface::SendResultToHost(const string &result)
{
    string message;    // Message to be transmitted
    // Calculate the check sum for the message
    UINT16 checksum = CalculateCheckSum(result) & 0x00FF;
    Log(LOG_DEV_DATA, "Message to send to DataCollector: <$%02X>%s<$%02X><$%02X><$%02X>",
        result[0], result.substr(1, result.length()-1).c_str(), checksum, CR, LF);
    // Add the message wrappers
    message = result + (char)checksum + (char)CR + (char)LF;
    Log(LOG_DEV_DATA, "Added trailer to message\n\tCheckSum: %02X\n\tCarriage Return: %02X\n\tLineFeed: %02X\n",
        checksum, CR, LF);
    // Transmit the message to the DataCollector
    BEP_STATUS_TYPE status = SendTestResultString(message, *(HostComm()), ACK, MaxMsgSendAttempts(), MaxHostResponseTime());
    Log(LOG_ERRORS, "Send results to Data Collector: %s", ConvertStatusToResponse(status).c_str());
}

//-----------------------------------------------------------------------------
inline const string DAQInterface::GetMachineFaultHeader(void)
{
   return m_machineFaultHeader;
}

//-----------------------------------------------------------------------------
inline const string DAQInterface::GetTestResultHeader(void)
{
   return m_testResultHeader;
}

//-----------------------------------------------------------------------------
inline const string DAQInterface::GetLossCompensationHeader(void)
{
   return m_lossCompensationHeader;
}

//-----------------------------------------------------------------------------
inline const INT32& DAQInterface::FaultCode(const INT32 *faultCode /*= NULL*/)
{
    if(faultCode != NULL)  m_faultCode = *faultCode;
    return m_faultCode;
}

//-----------------------------------------------------------------------------
inline const INT32& DAQInterface::MachineNumberLength(const INT32 *length /*= NULL*/)
{
    if(length != NULL)  m_machineNumberLength = *length;
    return m_machineNumberLength;
}

//-----------------------------------------------------------------------------
inline const XmlNodeMap& DAQInterface::LossCompResultStringMap(const XmlNode* mapList /*= NULL*/)
{
    if(mapList != NULL)
    {   // Get the loss compensation map
        XmlParser parser;
        m_lossCompensationMap.DeepCopy(parser.ReturnXMLDocument(getenv("USR_ROOT") + mapList->getValue())->getChildren());
    }
    return m_lossCompensationMap;
}

//-----------------------------------------------------------------------------
void DAQInterface::SetResultStringHeaders(void)
{   // Get the machine number data
    char machNum[MachineNumberLength()+1];
    sprintf(machNum, "%04d", atoi(GetMachineNumber().c_str()));
    // Set the test result header
    m_testResultHeader = (char)TEST_RESULT_STX + string(" ") + machNum + string(" ") + MeasurementSystem() + string(" ");
    // Set the calibrate result header
    m_lossCompensationHeader = (char)LOSS_COMPENSATION_STX + string(" ") + machNum + string(" ");
    // Set the fault string header
    m_machineFaultHeader = (char)MACHINE_FAULT_STX + string(" ") + machNum + string(" ");
}

//-----------------------------------------------------------------------------
const string& DAQInterface::MeasurementSystem(const string *measurementSystem /*= NULL*/)
{
    if(measurementSystem != NULL)  m_measurementSystem = *measurementSystem;
    return m_measurementSystem;
}

//-----------------------------------------------------------------------------
const bool& DAQInterface::FaultReadyToProcess(const bool *readyToProcess /*= NULL*/)
{
    if(readyToProcess != NULL)  m_faultReadyToProcess = *readyToProcess;
    return m_faultReadyToProcess;
}
