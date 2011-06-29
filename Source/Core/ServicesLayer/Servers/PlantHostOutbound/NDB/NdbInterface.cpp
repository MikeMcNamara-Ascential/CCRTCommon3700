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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/PlantHostOutbound/NDB/NdbInterface.cpp $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "NdbInterface.h"

//-----------------------------------------------------------------------------
NdbInterface::NdbInterface() : HostInterface()
{   // Nothing special to do here
}


//-----------------------------------------------------------------------------
void NdbInterface::Initialize(int argc, char *argv[])
{   // Parse the command line options for the configuration fie and other data
    return HostInterface::Initialize(argc,argv);
}

//-----------------------------------------------------------------------------
void NdbInterface::Initialize(const string &fileName)
{
    return HostInterface::Initialize(fileName);
}
//-----------------------------------------------------------------------------
void NdbInterface::Initialize(const XmlNode *document)
{
    // Initialize the base class
    BepServer::Initialize(document);
    Log(LOG_FN_ENTRY, "NdbInterface::Initialize() - Enter");
    // Create a new INamedDataBroker comm object
    // if an instance of namedDataBroker interface does not already exist, make one
    if(m_ndb == NULL)
    {
        m_ndb = new INamedDataBroker();
        Log(LOG_DEV_DATA, "Created new NamedDatBroker comm object");
    }
    // Load additional config data
    LoadAdditionalConfigurationItems(document);
    // host system serial channel m_plantHostComm removed - not needed for this interface
    Log(LOG_FN_ENTRY, "NdbInterface::Initialize() - Exit");
}

//-----------------------------------------------------------------------------
const string NdbInterface::ProcessMapNode(const XmlNode *resultMapNode,
                                           const XmlNodeMap &testResults,
                                           const XmlNodeMap &vehicleData,
                                           const XmlNodeMap &resultConversions,
                                           INT32 &totalSerialNumbers)
{
    string value(""), name, response, result(BEP_SUCCESS_RESPONSE);
    //name of ndb item that will be published
    name = resultMapNode->getName();
    value = HostInterface::ProcessMapNode(resultMapNode,
                                           testResults,
                                           vehicleData,
                                           resultConversions,
                                           totalSerialNumbers);
    Log("Writing tag %s with value: %s\n", name.c_str(), value.c_str());
    result = ConvertStatusToResponse(m_ndb->Write(name.c_str(), value.c_str(), response, true));
    Log("Result of NDB Write: %s\n", result.c_str());
    return value;
}

//-----------------------------------------------------------------------------
void NdbInterface::SendResultToHost(const string &result)
{//results already sent to NDB nothing to do here
}
