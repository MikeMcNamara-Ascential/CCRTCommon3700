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
    // Log the exit
    Log(LOG_FN_ENTRY, "ALCInterface::LoadAdditionalConfigurationItems() - complete");
}

//-----------------------------------------------------------------------------
inline const std::string ALCInterface::GetPFSHeader(void)
{
	std::string noHeader("");
	return( noHeader);
}

//-----------------------------------------------------------------------------
void ALCInterface::UpdatePFSHeader(const INT32 &data)
{
}

//-----------------------------------------------------------------------------
void ALCInterface::SetPFSHeader(const XmlNode *header)
{
}

//-----------------------------------------------------------------------------
string ALCInterface::GenerateHostResultString(const XmlNode *testResults)
{
    string pfsResult;
    pfsResult.reserve(1024);
    INT32 totalSerialNumbers = 0;
    // Generate the base and detail PFS strings
    pfsResult = BuildTestResultString(testResults, ResultMap(), ResultConversions(), totalSerialNumbers);
    pfsResult += BuildTestResultString(testResults, DetailMap(), ResultConversions(), totalSerialNumbers);
    // Return the result string
    return pfsResult;
}

//-----------------------------------------------------------------------------
void ALCInterface::SendResultToHost(const string &result)
{   // Get the VIN to include in the message
    string message;
    message.reserve(1024);
    // Add the message length to the message
    char msgLength[5];
    sprintf(msgLength, "%04d", result.length());
    message = msgLength + result;
    // Calculate and add the checksum to the message along with the wrapper
    UINT16 checksum = CalculateCheckSum(message) & 0x00FF;
    Log(LOG_DEV_DATA, "HostInterface: Message to send to host system - <%02X>%s<%02X><%02X><%02X>",
        STX, message.c_str(), checksum, CR, LF);
    message = (char)STX + message + (char)checksum + (char)CR + (char)LF;
    // Transmit the result string to the host system
    BEP_STATUS_TYPE status = SendTestResultString(message, *(HostComm()), ACK, MaxMsgSendAttempts(), MaxHostResponseTime());
    Log(LOG_ERRORS, "Send result to host system: %", ConvertStatusToResponse(status).c_str());
}

//-----------------------------------------------------------------------------
const XmlNodeMap& ALCInterface::DetailMap(const XmlNode *detailMap /*= NULL*/)
{
    if(detailMap != NULL)
    {
        m_pfsDetailMap.DeepCopy(detailMap->getChildren());
        m_pfsDetailMap.delNode("DetailSectionLength");
    }
    return m_pfsDetailMap;
}
