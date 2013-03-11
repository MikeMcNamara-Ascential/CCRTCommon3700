//******************************************************************************
//
// $Archive: /HMMA/Source/HMMA/ServicesLayer/Servers/PlantHostOutbound/ALCInterface.cpp $
// $Author: Gswope $
//   $Date: 3/09/06 3:16p $
//
// Description:
//    ALC Host Interface for HMMA
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /HMMA/Source/HMMA/ServicesLayer/Servers/PlantHostOutbound/ALCInterface.cpp $
// 
// 1     3/09/06 3:16p Gswope
// After changes to bring HMMA up to "current core"
// 
// 1     1/18/05 10:47a Gswope
// Initial Entry in this project in SourceSafe
// 
// 2     11/24/04 12:09p Bmeinke
// PFSInterface derived object for HMMA ALC host system
// 
// 1     11/19/04 3:10p Bmeinke
// 
//
//******************************************************************************


#include "ALCInterface.h"


//-----------------------------------------------------------------------------
ALCInterface::ALCInterface() : HostInterface()
{
}

//-----------------------------------------------------------------------------
ALCInterface::~ALCInterface()
{
}

//-----------------------------------------------------------------------------
void ALCInterface::LoadAdditionalConfigurationItems(const XmlNode *document)
{   // Begin by initializing the base class
    HostInterface::LoadAdditionalConfigurationItems(document);
    Log(LOG_FN_ENTRY, "ALCInterface::LoadAdditionalConfigurationItems() - begin");
    // Load the result string map
    XmlParser parser;
    try
    {
        DetailMap(parser.ReturnXMLDocument(getenv("USR_ROOT") + document->getChild("Setup/ALCDetailMap")->getValue()));
        Log(LOG_FN_ENTRY, "Detail Map loaded");
    }
    catch(XmlException &excpt)
    {
        Log(LOG_FN_ENTRY, "XmlException loading detail map: %s", excpt.GetReason());
    }
    catch(BepException &excpt)
    {
        Log(LOG_FN_ENTRY, "BepException loading detail map: %s", excpt.GetReason());
    }
    catch(...)
    {
        Log(LOG_FN_ENTRY, "Failed to load ALCDetailMap");
    }
    // Log the exit
    Log(LOG_FN_ENTRY, "ALCInterface::LoadAdditionalConfigurationItems() - complete");
}

//-----------------------------------------------------------------------------
inline const std::string ALCInterface::GetALCHeader(void)
{
    std::string noHeader("");
    return( noHeader);
}

//-----------------------------------------------------------------------------
void ALCInterface::UpdateALCHeader(const INT32 &data)
{
}

//-----------------------------------------------------------------------------
void ALCInterface::SetALCHeader(const XmlNode *header)
{
}

//-----------------------------------------------------------------------------
void ALCInterface::DoResultProcessing(const XmlNode *testResults)
{
    INT32 total = 0;
    PrepareTestResultValues(testResults, ResultMap(), ResultConversions(), total);
}

//-----------------------------------------------------------------------------
const XmlNodeMap& ALCInterface::DetailMap(const XmlNode *detailMap /*= NULL*/)
{
    if(detailMap != NULL)
    {
        m_alcDetailMap.DeepCopy(detailMap->getChildren());
        m_alcDetailMap.delNode("DetailSectionLength");
    }
    return m_alcDetailMap;
}

//-----------------------------------------------------------------------------
void ALCInterface::PrepareTestResultValues(const XmlNode *testResult,
                                                  const XmlNodeMap &resultStringMap,
                                                  const XmlNodeMap &resultConversions,
                                                  INT32 &totalSerialNumbers,
                                                  const bool &useVehicleBuild, /*= true*/
                                                  const bool &allowSpaceInResult) /*= true*/
{
    // Place the test results into a map so we can easily access the information
    const XmlNodeMap &testResultMap = testResult->getChild(BEP_TEST_RESULT)->getChildren();
    const XmlNodeMap *vehicleBuildMap;
    if(useVehicleBuild) vehicleBuildMap = &(testResult->getChild(VEHICLE_BUILD_TAG)->getChildren());
    string logMessage("");
    // Log the entry and clear the result 
    Log(LOG_DEV_DATA, "HostInterface::BuildTestResultString() - Enter");
    // Build the result string
    for(XmlNodeMapCItr iter = resultStringMap.begin(); iter != resultStringMap.end(); iter++)
    {   // Process the next field for the result string
        Log(LOG_DETAILED_DATA, "Preparing to Process field: %s", iter->second->ToString().c_str());
        try
        {
            ProcessMapNode(iter->second, testResultMap, *vehicleBuildMap, resultConversions, totalSerialNumbers);
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "Malformed test result node for map node: {%s}", iter->second->ToString().c_str());
        }
        Log(LOG_DETAILED_DATA, "Completed processing field: %s", iter->second->ToString().c_str());
    }
}

//-----------------------------------------------------------------------------
const string ALCInterface::ProcessMapNode(const XmlNode *resultMapNode,
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
    {   // Determine the test result for this field
        result = GetTestResult(testResults, resultMapNode, resultConversions);
    }
    else if((type == typeDetail) || (type == typeSerialNumber) || (type == typeDetailString))
    {   // Add the value to the result string
        result = GetTestDetailValue(testResults, resultMapNode);
        if((type == typeSerialNumber) && (result.length() == serialNumberLength))
        {   // Increment the total number of serial numbers reported
            totalSerialNumbers++;
        }
    }
    else if(type == BEP_TEST_RESULT_CODE)
    {   // Not implemented
    }
    else if(type == typeConditionalBlock)
    {   // Check if the result of the specified test against the desired result.  If it matches, process the block.
        string condition("equal");  // Default to equals
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
            {   // Result matches expected, process the data
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
    {   // Get the data from the vehicle build map
        XmlNodeMapCItr buildIter;
        Log(LOG_DETAILED_DATA, "Looking for build item: %s\n", resultMapNode->getValue().c_str());
        if((buildIter = vehicleData.find(resultMapNode->getValue())) != vehicleData.end())
        {   // Get the value of the node
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
        {   // Default to not equipped for the requested option
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
    {   // Not implemented
    }
    else if(type == dateTimeNode)
    {   // See if there is a specific format defined
        try
        {   // Get the date/time format to apply to the data
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
    {   // Read the value through the NamedDataBroker
        if(("Date" == resultMapNode->getValue()) || ("Time" == resultMapNode->getValue()))
        {   // Get the date from the system
            result = GetTimeAndDate(resultMapNode->getValue());
        }
        else
        {   // Get the data through the NamedDataBroker
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
    {   // Not implemented
    }
    else if(type == typeComponentName)
    {   // Not implemented
    }
    else if(type == typeDtcAttribute)
    {   // Not implemented
    }
    else if(type == typeDtcData)
    {   // Not implemented
    }
    else if(type == typeXmlTag)
    {   // Not implemented
    }

    // Write the result to the PLC for the ALC to read it
    if(((type == typeDetail) || (type == typeSerialNumber) || (type == typeDetailString)) && 
       ((dataType == typeInt) || (dataType == typeFloat) || (dataType == typeASCIIInt)))
    {
        string plcTag;
        XmlNodeMapCItr iter = resultMapNode->getAttributes().find(plcDataTag);
        if(iter != resultMapNode->getAttributes().end())
        {
            plcTag = iter->second->getValue();
            Write(plcTag, result);
            Log(LOG_DEV_DATA, "Wrote PLC tag %s, %s\n", plcTag.c_str(), result.c_str());
        }
    }
    else if(type == BEP_RESULT)
    {
        string plcTag;
        XmlNodeMapCItr iter = resultMapNode->getAttributes().find(plcDataTag);
        if(iter != resultMapNode->getAttributes().end())
        {
            plcTag = iter->second->getValue();
            Write(plcTag, result);
            Log(LOG_DEV_DATA, "Wrote PLC tag %s, %s\n", plcTag.c_str(), result.c_str());
        }
    }
    else if(type == dateTimeNode)
    {
        string plcTag;
        XmlNodeMapCItr iter = resultMapNode->getAttributes().find(plcDataTag);
        if(iter != resultMapNode->getAttributes().end())
        {
            plcTag = iter->second->getValue();
            Write(plcTag, result);
            Log(LOG_DEV_DATA, "Wrote PLC tag %s:%s\n", plcTag.c_str(), result.c_str());
        }
    }
    else
    {
        Log(LOG_DEV_DATA, "Result type %s not implemented. Couldn't write %s:%s", type.c_str(), plcTag.c_str(), result.c_str());
    }
    return(result);
}

