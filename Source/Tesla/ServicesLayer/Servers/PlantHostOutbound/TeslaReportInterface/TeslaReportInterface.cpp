//******************************************************************************
// Description:
//   Host Interface base class to process a generic text result with STX and ETX, but no checksum
//   and send to a host system.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "TeslaReportInterface.h"
#include "GeneralUtilities.h"
#include <math.h>
#include <algorithm>
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
TeslaReportInterface::TeslaReportInterface() : HostInterface()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
TeslaReportInterface::~TeslaReportInterface()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE TeslaReportInterface::SendTestResultString(const string &resultString, 
                                                           SerialChannel &portComm, 
                                                           const INT32 &maxAttempts)
{
    INT32 currentAttempt = 1;                   // Keep track of attempts
    bool portLocked = false;                    // Port lock status
    string message(resultString);
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;  // Transmit status
    // Attempt to send the message
    do
    {   // Lock the port so only we have access to it
        portLocked = portComm.LockPort();
        // If the port is locked, transmit the message
        if (portLocked)
        {
            Log(LOG_FN_ENTRY, "Port was successfully locked by SendTestResultString()");
            // Transmit the message
            portComm.ResetConnection();
            status = portComm.Send(message);
            // Log the status
            Log(LOG_ERRORS, "Sent message to host: %s", ConvertStatusToResponse(status).c_str());
            // Unlock the port for this attempt
            portLocked = portComm.UnlockPort();
        }
        else
        {   // Log the error
            Log(LOG_ERRORS, "WARNING: Could not lock %s port.  Not transmitting results\n", 
                portComm.GetProcessName().c_str());
            BposSleep(100);       //keep from locking up if port is not released to use.
        }
        // Keep trying while attempts remaining and failed transmit
    } while ((BEP_STATUS_SUCCESS != status) && (currentAttempt++ <= maxAttempts));
    // Return the status
    return status;
}

//-----------------------------------------------------------------------------
void TeslaReportInterface::SendResultToHost(const string &result)
{
    Log(LOG_DEV_DATA, "TeslaReportInterface: Message to send to host system - %s", result.c_str());
    Log(LOG_DEV_DATA, "Adding STX and ETX to message\n");
    string stx = "\x002";
    string etx = "\x003";
    UINT16 checksum = CalculateCheckSum(result) & 0xFF;
    Log(LOG_DEV_DATA, "Calculated checksum is 0x%02X (%d)", checksum, checksum);
    string resultToHost = stx + result + (char)checksum + etx;
    // Transmit the result string to the host system
    BEP_STATUS_TYPE status = SendTestResultString(resultToHost, *(HostComm()), MaxMsgSendAttempts());
    Log(LOG_ERRORS, "Sent result to host system: %", ConvertStatusToResponse(status).c_str());
}

//-----------------------------------------------------------------------------
const string TeslaReportInterface::ProcessMapNode(const XmlNode *resultMapNode,
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
    if(type == "SimpleMath")
    {   // Determine the test result for this field
        result = GetMathResult(testResults, resultMapNode);
    }
    else if(type == "DetailAttribute")
    {
        result = GetDetailAttributeValue(testResults, resultMapNode);
    }
    else
    {   
        result = HostInterface::ProcessMapNode(resultMapNode, testResults, vehicleData, resultConversions, totalSerialNumbers);
    }
    return(result);
}

//-----------------------------------------------------------------------------
string TeslaReportInterface::GetMathResult(const XmlNodeMap &testResultMap, const XmlNode *testResultStringField)
{
    string result;
    INT32 lengthDetail = 0;
    // Leave some room for the result, default to 32 bytes
    result.reserve(32);
    result.erase();
    bool excludeDetail = false;
    float scale, offset;
    // Get a map of the attributes
    const XmlNodeMap &attributes = testResultStringField->getAttributes();
    XmlNodeMapCItr attributeIter;
    // Get the field format from the node
    string format(attributes.getNode(fieldFormat)->getValue());
    string operandAName(attributes.getNode("DetailA")->getValue());
    string operandBName(attributes.getNode("DetailB")->getValue());
    Log(LOG_DETAILED_DATA, "GetMathResult() - Test Result Node: %s, Format: %s, DetailA Node: %s, DetailB Node: %s",
        testResultStringField->getValue().c_str(), format.c_str(), operandAName.c_str(), operandBName.c_str());
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
    // Get the type of data we are dealing with.  NOTE: default to string if not specified
    attributeIter = attributes.find(fieldDataType);
    string dataType = attributeIter != attributes.end() ? attributeIter->second->getValue() : typeString;
    // Get the type of math operation we are doing. NOTE: default to addition if not specified
    attributeIter = attributes.find("Operation");
    string operationType = attributeIter != attributes.end() ? attributeIter->second->getValue() : "addition";
    // Get the detail node from the test result node
    XmlNodeMapCItr testResultIter = testResultMap.find(testResultStringField->getValue());
    // note that the buffer below has to be bigger than any string field in use
    char buf[128];
    bool testDetailReported = false;
    
    if(testResultIter != testResultMap.end())
    {
        // Get the detail node and translate to appropraite type
        try
        {
            float value = 0.0;
            float operandA = atof(testResultIter->second->getChild(operandAName)->getValue().c_str());
            float operandB = atof(testResultIter->second->getChild(operandBName)->getValue().c_str());
            if (operationType == "addition")
            {
                value = operandA + operandB;
            }
            else if (operationType == "subtraction")
            {
                value = operandA - operandB;
            }
            else if (operationType == "multiplication")
            {
                value = operandA * operandB;
            }
            else if (operationType == "division")
            {
                value = operandA / operandB;
            }
            // store the data value to format
            string dataValue = CreateMessage(buf, sizeof(buf), "%f", value);
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
    {   // Test result was not reported, determine how to fill the field
        if(excludeDetail)
        {
            result.erase();
        }
        else if(!dataType.compare(typeInt) || !dataType.compare(typeFloat))
        {
            result = CreateMessage(buf, sizeof(buf), format.c_str(), 0.0);
        }
        else
        {
            result.erase();
        }
    }
    
    // Return the final result
    Log(LOG_FN_ENTRY, "GetMathResult() - Returning: %s", result.c_str());
    return(result);
}

//-----------------------------------------------------------------------------
string TeslaReportInterface::ProcessDtcNode(const XmlNode *dtcFormat, 
                                     const XmlNodeMap &testResults,
                                     const XmlNodeMap &vehicleData,
                                     const XmlNodeMap &resultConversions,
                                     INT32 &totalSerialNumbers)
{
    string result("");
    INT32 currentDtcsAdded = 0;
    // Determine the number of DTCs to report
    INT32 numDtcToReport = -1;   // Default to report all DTCs
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
        bool mapComponentName = false;  // Default to not map component names
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
        {   // Map the component name if required
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
                {   // Increment the number of DTCs added and process each format node
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
                        {   // If this is a component name type, add the name as the value of the node
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
                            else if(type == "DTCNumber")
                            {
                                char buf[32];
                                result += CreateMessage(buf, sizeof(buf),",DTC%1d:",currentDtcsAdded);
                                continue;
                            }
                            else if(type == "DTCStatus")
                            {
                                char buf[32];
                                result += CreateMessage(buf, sizeof(buf),"F:",currentDtcsAdded);
                                continue;
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
                {   //set value to blank
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
                    else if(type == "DTCNumber")
                    {
                        char buf[32];
                        result += CreateMessage(buf, sizeof(buf),",DTC%1d:",currentDtcsAdded);
                        continue;
                    }
                    else if(type == "DTCStatus")
                    {
                        char buf[32];
                        result += CreateMessage(buf, sizeof(buf),"Z",currentDtcsAdded);
                        continue;
                    }
                    else if(type == typeDtcAttribute)
                    {
                        continue;
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
string TeslaReportInterface::GetDetailAttributeValue(const XmlNodeMap &testResultMap, const XmlNode *testResultStringField)
{
    string result;
    result.reserve(32);
    result.erase();
	// Get a map of the attributes
	const XmlNodeMap &attributes = testResultStringField->getAttributes();
	XmlNodeMapCItr attributeIter;
    // Get the detail node name and attribute name from the node
	string detailNodeName(attributes.getNode(detailNode)->getValue());
    string attributeNodeName(attributes.getNode("AttributeNode")->getValue());
	Log(LOG_DETAILED_DATA, "GetDetailAttributeValue() - Test Result Node: %s, Detail Node: %s, Attribute Node: %s",
		testResultStringField->getValue().c_str(), detailNodeName.c_str(), attributeNodeName.c_str());
	// Get the detail node from the test result node
	XmlNodeMapCItr testResultIter = testResultMap.find(testResultStringField->getValue());

    try
    {
        // store the data value to format
        if(testResultIter != testResultMap.end())
        {
            result = testResultIter->second->getChild(detailNodeName)->getAttribute(attributeNodeName)->getValue();
        }
        else
        {
            Log(LOG_ERRORS, "Test result not reported: %s", testResultStringField->getValue().c_str());
            result.clear();
        }
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "Test result detail attribute not reported: %s", e.what());
        result.clear();
    }
    catch(...)
    {
        Log(LOG_ERRORS, "Unknown exception getting test result detail attribute");
        result.clear();
    }

	// Return the final result
	Log(LOG_FN_ENTRY, "GetDetailAttributeValue() - Returning: %s", result.c_str());
	return(result);
}


