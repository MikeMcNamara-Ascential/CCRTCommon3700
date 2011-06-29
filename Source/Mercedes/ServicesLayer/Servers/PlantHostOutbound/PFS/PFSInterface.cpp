//******************************************************************************
// Description:
//   PFSInterface class to handle communications between the DCX
//	 Plant Feedback System(PFS) and the Burke Porter system
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/PlantHostOutbound/PFS/PFSInterface.cpp $
// 
// 4     5/10/07 4:49p Derickso
// Updated to allow separate TPM message to be reported to PFS.
// 
// 3     4/04/07 11:06a Derickso
// Updated to have base class call LoadAdditionalConfigurationItems from
// Initialize().
// Removed overloaded Initialize functions and replaced with overloaded
// LoadAdditionalConfigurationItems.
// 
// 2     3/27/07 2:09p Derickso
// Added new Initialize functions.
// Corrected location of data items from config files.
// Corrected errors from testing and debug.
// 
// 1     3/23/07 3:05p Derickso
// Moved to PFS directory.
// 
// 2     3/23/07 3:04p Derickso
// Refactored to derive from HostInterface.
// 
// 1     3/23/07 12:30p Derickso
// Moved from Core to Dcx specific.
// 
// 1     11/10/06 4:26p Bbarrett
// Add sup[port for static tester.
// 
// 4     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration

// 3     11/24/04 1:29p Bmeinke
// Added GetPlantHostComm() method to get a reference to the SerialChannel
// used to communicate with the host system (needed for new QMSInterface
// object)
// Added GetPlantHost() method to get a reference to the PlantHostOutbound
// object that instantiated us (needed for new QMSInterface object)
//
// 2     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 1     7/21/04 10:18a Cward
//
// 3     5/25/04 4:34p Cward
// Updated logic to determine if results should be sent to PFS system
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     1/28/04 12:47p Cward
//
// 1     8/21/03 9:09a Derickso
// 05WK CCRT Project
//
// 1     7/09/03 2:31p Rwiersem
// Initial version.
//
// 1     7/03/03 5:20p Khykin
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
// 4     9/26/02 2:02p Tbochene
// Removed compiler warnings
//
// 3     8/23/02 10:19a Dan.erickson
// Added diagnostics.  Updated to ensure OverallTEst is Pass, Fail or
// Abort.
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

#include "PFSInterface.h"

//-----------------------------------------------------------------------------
PFSInterface::PFSInterface() : HostInterface(), m_addPfsHeader(true), m_reportTpmResults(false)
{   // NOthing special to do
}

//-----------------------------------------------------------------------------
PFSInterface::~PFSInterface()
{   // Clear all maps
    m_pfsDetailMap.clear(true);
    m_tpmResultMap.clear(true);
    m_tpmBodyStyles.clear(true);
}

//-----------------------------------------------------------------------------
void PFSInterface::DoResultProcessing(const XmlNode *testResults)
{   // Process the normal PFs result string
    MessageId(TestResultMessageId());
    HostInterface::DoResultProcessing(testResults);
    // Determine if the TPM result string should be generated
    if(ReportTpmResults())
    {
        try
        {
            string bodyStyle(testResults->getChild(VEHICLE_BUILD_TAG)->getChild(BODY_STYLE_TAG)->getValue());
            Log(LOG_DEV_DATA, "Checking if body style %s should be processed for TPM results", bodyStyle.c_str());
            if(ShouldGenerateTpmResultString(bodyStyle))
            {
                Log(LOG_DEV_DATA, "Generating TPM results for %s", bodyStyle.c_str());
                MessageId(TpmMessageId());
                DoTpmResultProcessing(testResults);
            }
            else
            {
                Log(LOG_DEV_DATA, "Not generating TPM result string for %s", bodyStyle.c_str());
            }
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "Body style not found in result data, not generating TPM result string: %s", excpt.GetReason());
        }
    }
    else
    {
        Log(LOG_DEV_DATA, "Not reporting TPM results to PFS");
    }
}

//-----------------------------------------------------------------------------
void PFSInterface::DoTpmResultProcessing(const XmlNode *testResults)
{   // Generate the TPM result string
    INT32 serialNumCount = 0;
    string tpmResult(BuildTestResultString(testResults, TpmResultMap(), ResultConversions(), serialNumCount, true, true));
    // If results should be sent to the host system, send the result string
    if(ShouldSendResultToHost())
    {   // Send the result string to the host system
        SendResultToHost(tpmResult);
        Log(LOG_DEV_DATA, "PFSInterface: Sent result string to host system - %s", tpmResult.c_str());
    }
    else
    {
        Log(LOG_DEV_DATA, "PFSInterface: Results not sent to the host system!");
    }
}

//-----------------------------------------------------------------------------
void PFSInterface::LoadAdditionalConfigurationItems(const XmlNode *document)
{   // Begin by initializing the base class
    HostInterface::LoadAdditionalConfigurationItems(document);
    // Set the Test Result message Id to be the default loaded by the base class
    TestResultMessageId(&(MessageId()));
    // Get result string data
    Log(LOG_FN_ENTRY, "PFSInterface::LoadAdditionalConfigurationItems() - begin");
    // Store the serial number index
    INT32 index = 5;
    try
    {
        index = BposReadInt(document->getChild("Setup/SerialNumberIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find serial number index, using %d: %s", index, excpt.GetReason());
    }
    SerialNumberIndex(&index);
    // Store the serial number field length
    INT32 length = 3;
    try
    {
        length = BposReadInt(document->getChild("Setup/SerialNumberLength")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find serial number length, using %d: %s", length, excpt.GetReason());
    }
    SerialNumberFieldLength(&length);
    // Load the result string map
    XmlParser parser;
    try
    {
        DetailMap(parser.ReturnXMLDocument(getenv("USR_ROOT") + document->getChild("Setup/PFSDetailMap")->getValue()));
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
        Log(LOG_FN_ENTRY, "Failed to load PFSDetailMap");
    }
    // Store the index of the detail section length field in the PFS header
    index = 11;
    try
    {
        index = BposReadInt(document->getChild("Setup/DetailSectionLengthFieldIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Detail Section Length index not defined, using %d: %s", index, excpt.GetReason());
    }
    DetailSectionFieldIndex(&index);
    // Store the length of the detail section length field of the PFS header
    length = 3;
    try
    {
        index = BposReadInt(document->getChild("Setup/DetailSectionFieldLength")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Detail Section Field Length not defined, using %d: %s", length, excpt.GetReason());
    }
    DetailSectionFieldLength(&length);
    // Check if header data should be added to the message
    string addPfsHeader("true");
    try
    {
        addPfsHeader = document->getChild("Setup/AddPfsHeader")->getValue();
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Defaulting to add PFS header to result string - %s", excpt.GetReason());
    }
    AddPfsHeader(&addPfsHeader);
    // Setup the PFS header information
    if(AddPfsHeader())
    {
        string doNotAddHeader("false");
        try
        {
            Log(LOG_FN_ENTRY, "Storing PFS Header information");
            SetPFSHeader(document->getChild("Setup/PFSHeader"));
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "No PFS header information provided, not adding PFS header to result strings - %s", excpt.GetReason());
            AddPfsHeader(&doNotAddHeader);
        }
        catch(...)
        {
            Log(LOG_FN_ENTRY, "PFSInterface::Initialize() - Unhandled exception");
            AddPfsHeader(&doNotAddHeader);
        }
    }
    // Check if a separate TPM message should be sent to PFS
    string reportTpm("false");
    try
    {
        reportTpm = document->getChild("Setup/SendTpmResultToHost")->getValue();
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Defaulting to not generating separate TPM result string: %s", excpt.GetReason());
    }
    ReportTpmResults(&reportTpm);
    if(ReportTpmResults())
    {   // Load the TPM result string map
        try
        {
            TpmResultMap(parser.ReturnXMLDocument(getenv("USR_ROOT") + document->getChild("Setup/TpmResultMap")->getValue()));
            Log(LOG_FN_ENTRY, "TPM Result Map Loaded.");
        }
        catch(XmlException &excpt)
        {
            Log(LOG_FN_ENTRY, "XmlException loading TPM map: %s", excpt.GetReason());
        }
        catch(BepException &excpt)
        {
            Log(LOG_FN_ENTRY, "BepException loading TPM map: %s", excpt.GetReason());
        }
        catch(...)
        {
            Log(LOG_FN_ENTRY, "Failed to load TpmResultMap");
        }
        // Store the TPM message ID
        UINT8 tpmMsgId = 0xF6;
        try
        {
            tpmMsgId = BposReadInt(document->getChild("Setup/TpmMessageID")->getValue().c_str());
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "Defaulting to %02X for TPM message ID: %s", excpt.GetReason());
        }
        TpmMessageId(&tpmMsgId);
        // Store the body styles to used for determining if TPM data should be reported
        try
        {
            StoreTpmBodyStyles(document->getChild("Setup/ReportTpmDataForBodyStyles"));
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "TPM Reporting Body Styles not loaded, no TPM results will be reported: %s", excpt.GetReason());
        }
    }
    // Log the exit
    Log(LOG_FN_ENTRY, "PFSInterface::LoadAdditionalConfigurationItems() - complete");
}

//-----------------------------------------------------------------------------
void PFSInterface::UpdatePFSHeader(const INT32 &data)
{   // Update the serial number length field
    char dataString[SerialNumberFieldLength()+1];
    char buff[SerialNumberFieldLength()+1];
    string format = string("%0") + itoa(SerialNumberFieldLength(), buff, 10) + "d";
    sprintf(dataString, format.c_str(), data*serialNumberLength);
    Log(LOG_DEV_DATA, "Updating PFS header [%s] with new serial number field length: %s", 
        m_pfsHeader.c_str(), dataString);
    m_pfsHeader.replace(SerialNumberIndex(), SerialNumberFieldLength(), dataString);
    // Update the Detail section length in the header
    if(SerialNumberFieldLength() != DetailSectionFieldLength())
    {
        format.replace(2, 1, itoa(DetailSectionFieldLength(), buff, 10));
    }
    char detailString[DetailSectionFieldLength()+1];
    sprintf(detailString, format.c_str(), DetailSectionLength());
    Log(LOG_DEV_DATA, "Updating PFS header [%s] with new detail section length: %s", 
        m_pfsHeader.c_str(), detailString);
    m_pfsHeader.replace(DetailSectionFieldIndex(), DetailSectionFieldLength(), detailString);
    Log(LOG_DEV_DATA, "Updated PFS header: %s", m_pfsHeader.c_str());
}

//-----------------------------------------------------------------------------
void PFSInterface::SetPFSHeader(const XmlNode *header)
{   // Build the header string
    m_pfsHeader = header->getChild("ProtocolIdentifier")->getValue();
    m_pfsHeader += header->getChild("MessageDescriptionSectionLength")->getValue();
    m_pfsHeader += header->getChild("TracebilitySectionLength")->getValue();
    m_pfsHeader += header->getChild("TestResultSectionLength")->getValue();
    m_pfsHeader += "xxx";   // Default place holder.  This will be updated in UpdatePFSHeader
    m_pfsHeader += header->getChild("RecordIdentifier")->getValue();
    m_pfsHeader += header->getChild("EquipmentIdentifier")->getValue();
    m_pfsHeader += GetMachineNumber();
}

//-----------------------------------------------------------------------------
inline const string PFSInterface::GetPFSHeader(void)
{
    return m_pfsHeader;
}

//-----------------------------------------------------------------------------
string PFSInterface::GenerateHostResultString(const XmlNode *testResults)
{
    string pfsResult, pfsDetail;
    pfsResult.reserve(1024);
    INT32 totalSerialNumbers = 0;
    // Generate the base and detail PFS strings
    pfsResult = BuildTestResultString(testResults, ResultMap(), ResultConversions(), totalSerialNumbers);
    pfsDetail = BuildTestResultString(testResults, DetailMap(), ResultConversions(), totalSerialNumbers);
    // Store the length of the detail section
    INT32 length = pfsDetail.length();
    DetailSectionLength(&length);
    pfsResult += pfsDetail;
    // Add the PFS header if required
    if(AddPfsHeader())
    {   // Update the header info and add to result string
        UpdatePFSHeader(totalSerialNumbers);
        pfsResult = GetPFSHeader() + pfsResult;
    }
    else
    {
        Log(LOG_DEV_DATA, "NOT adding PFS header information to the result string!");
    }
    // Return the result string
    return pfsResult;
}

//-----------------------------------------------------------------------------
inline const INT32& PFSInterface::SerialNumberIndex(const INT32 *index /*= NULL*/)
{
    if(index != NULL)  m_serialNumberIndex = *index;
    return m_serialNumberIndex;
}

//-----------------------------------------------------------------------------
inline const INT32& PFSInterface::SerialNumberFieldLength(const INT32 *length /*= NULL*/)
{
    if(length != NULL)  m_serialNumberLength = *length;
    return m_serialNumberLength;
}

//-----------------------------------------------------------------------------
inline const INT32& PFSInterface::DetailSectionFieldIndex(const INT32 *index /*= NULL*/)
{
    if(index != NULL)   m_detailSectionFieldIndex = *index;
    return m_detailSectionFieldIndex;
}

//-----------------------------------------------------------------------------
inline const INT32& PFSInterface::DetailSectionFieldLength(const INT32 *length /*= NULL*/)
{
    if(length != NULL)  m_detailSectionFieldLength = *length;
    return m_detailSectionFieldLength;
}

//-----------------------------------------------------------------------------
const INT32& PFSInterface::DetailSectionLength(const INT32 *sectionLength /*= NULL*/)
{
    if(sectionLength != NULL)   m_detailSectionLength = *sectionLength;
    return m_detailSectionLength;
}

//-----------------------------------------------------------------------------
const XmlNodeMap& PFSInterface::DetailMap(const XmlNode *detailMap /*= NULL*/)
{
    if(detailMap != NULL)
    {
        m_pfsDetailMap.DeepCopy(detailMap->getChildren());
        m_pfsDetailMap.delNode("DetailSectionLength");
    }
    return m_pfsDetailMap;
}

//-----------------------------------------------------------------------------
const XmlNodeMap& PFSInterface::TpmResultMap(const XmlNode *resultMap /*= NULL*/)
{
    if(resultMap != NULL)
    {
        m_tpmResultMap.DeepCopy(resultMap->getChildren());
    }
    return m_tpmResultMap;
}

//-----------------------------------------------------------------------------
inline const bool& PFSInterface::AddPfsHeader(const string *addPfsHeader/*= NULL*/)
{
    if(addPfsHeader != NULL)    m_addPfsHeader = atob(addPfsHeader->c_str());
    return m_addPfsHeader;
}

//-----------------------------------------------------------------------------
inline const bool& PFSInterface::ReportTpmResults(const string *reportTpmResult /*= NULL*/)
{
    if(reportTpmResult != NULL) m_reportTpmResults = atob(reportTpmResult->c_str());
    return m_reportTpmResults;
}

//-----------------------------------------------------------------------------
inline UINT8* PFSInterface::TpmMessageId(const UINT8 *messageId /*= NULL*/)
{
    if(messageId != NULL)  m_tpmMessageId = *messageId;
    return &m_tpmMessageId;
}

//-----------------------------------------------------------------------------
inline UINT8* PFSInterface::TestResultMessageId(const UINT8 *messageId /*= NULL*/)
{
    if(messageId != NULL)  m_testResultMessageId = *messageId;
    return &m_testResultMessageId;
}

//-----------------------------------------------------------------------------
inline void PFSInterface::StoreTpmBodyStyles(const XmlNode *tpmBodyStyles)
{   // Ensure the map is cleared prior to storing the new data
    m_tpmBodyStyles.clear(true);
    m_tpmBodyStyles.DeepCopy(tpmBodyStyles->getChildren());
}

//-----------------------------------------------------------------------------
const bool PFSInterface::ShouldGenerateTpmResultString(const string &bodyStyle)
{
    bool bodyStyleFound = false;
    for(XmlNodeMapItr iter = m_tpmBodyStyles.begin(); 
         (iter != m_tpmBodyStyles.end()) && !bodyStyleFound; 
         iter++)
    {
        bodyStyleFound = (bodyStyle.find(iter->second->getName()) != bodyStyle.npos);
        Log(LOG_DEV_DATA, "Checking for %s in %s - %s: find=%d, npos=%d",
            iter->second->getName().c_str(), bodyStyle.c_str(), bodyStyleFound ? "Found" : "Not Found",
            bodyStyle.find(iter->second->getName()), bodyStyle.npos);
    }
    return bodyStyleFound;
}
