//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/NamedDataBroker/NamedDataBroker.cpp 5     10/26/06 1:11p Rwiersem $
//
// FILE DESCRIPTION:
//  The Named Data Broker is the broker for requests for data by processes.
//  It is responsible for receiving the messages and then forwarding them
//  to the appropriate process to respond to the requestor.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/NamedDataBroker/NamedDataBroker.cpp $
// 
// 5     10/26/06 1:11p Rwiersem
// Changes for the 06102701 core release:
// 
// Removed the call to LoadAdditionalConfigurationItems() in
// HandlePulse().
// This is now called from BepServer.
// 
// 4     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/12/06 1:05p Rwiersem
// Added LoadAdditionalConfigurationItems() and HandlePulse().  These
// methods are used to reload the configuration without restarting the
// process.
//
// 3     7/20/05 9:22p Cward
// Added support for new TaskMon
//
// 2     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 1     12/21/03 6:42p Cward
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/18/03 7:51a Khykin
// Updated with Flatrock changes.
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:01p Khykin
// Initial checkin.
//
// 1     4/22/03 9:49a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:08  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:15a Khykin
// Initial Checkin
//
// 1     1/18/03 8:10a Khykin
// Initial Checkin From DCTC
//
// 35    9/24/02 6:42p Tbochene
// Fixed bug in Command (put in try/catch) and updated Log() calls to take
// verbose mask
//
// 34    4/24/02 10:38p Skypig13
// Use getChild() instead of getChildren().getNode()
//
// 33    3/20/02 9:12a Dan.erickson
// LoadServerStatusTable must be called before Initialize.
//
// 32    2/25/02 8:54p Skypig13
// Updated Initialize()
//
// 31    2/19/02 4:37p Skypig13
// Update to match changes in BepServer
//
// 30    2/05/02 12:53a Skypig13
// Changes in BepServer and IBepCommunication as well as removing compiler
// warnings
//
// 29    1/04/02 1:03p Skypig13
// Updated Initialize to be compatible with new BepServer configuration
// file
//
// 28    12/10/01 4:13p Skypig13
// Changed response to all requests except for Read and Command to
// NotSupported
//
// 27    11/29/01 3:00p Skypig13
// Updated initialize() to save configuration file, handled tag
// not found in Read(), implemented Reset() to Initialize()
//
// 26    11/21/01 10:33a Skypig13
// Changed so that Subscribe(), UnSubscribe(), and Publish() should work
//
// 25    11/21/01 8:37a Skypig13
// Removed Terminate()
//
// 24    11/21/01 7:55a Skypig13
// Changed printf to Log
//
// 23    11/15/01 7:14p Skypig13
// Updated to use new structure inherited from BepServer
//
// 22    10/30/01 12:45p Kh
// Making change for the location of servers in VerifyServer.
//
// 21    10/26/01 9:38a Kh
// Changing the initialization of communiations to after the tag table is
// loaded.
//
// 20    10/25/01 11:25a Skypig13
// Updated diagnostics to be more gooder
//
// 19    10/17/01 9:56a Skypig13
// Changed prototype of Read() to include data reate
//
// 18    10/16/01 4:17p Kh
// Making change to the LoadServerStatusTable method.
//
// 17    10/16/01 12:14a Skypig13
// Comment in LoadServerStatusTable()
//
// 16    10/16/01 12:11a Skypig13
// Fixed initialization sequence and VerifyServer()
//
// 15    10/09/01 1:43p Skypig13
// Added BepComponent constructor to initializer list in constructors
//
// 14    10/08/01 11:48p Skypig13
// Initialize IBepCommunication before LoadTable()
//
// 13    10/02/01 3:39p Kh
// Adding retrys to the location of servers.
//
// 12    8/27/01 5:32p Skypig13
// Update to use BposIsTaskAlive() inside VerifyServer()
//
// 11    8/17/01 1:42p Skypig13
// Removed printf from method
//
// 9     8/02/01 4:59p Skypig13
// Weekly backup
//
// 7     7/24/01 11:29a Skypig13
// Latest changes
//
// 6     7/19/01 3:26p Skypig13
// Setup tests work
//
// 5     7/07/00 9:41a Skypig13
// Latest revision
//
// 4     6/13/01 7:08p Skypig13
// Added visitor pattern interface
//
// 3     6/05/01 9:03a Skypig13
// Latest revision
//
// 2     5/02/01 9:13p Skypig13
// Updating
//
// 1     4/10/01 10:46a Admin
//
// 1     4/09/01 1:00p Brian.meinke
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 1     2/22/01 5:33p Tim.bochenek
// Unit testing of Logger, Parser, and CmdLine Processor
//
// 1     01/03/01 8:00a Kevin.Hykin
// Initial Revision
//
//*************************************************************************
#include "Bepos.h"
#include "NamedDataBroker.h"

NamedDataBroker::NamedDataBroker(void): BepServer()
{
}
NamedDataBroker::~NamedDataBroker(void)
{
    ClearData();
}
void NamedDataBroker::Initialize(const XmlNode *document)
{
    Log(LOG_FN_ENTRY, "NamedDataBroker Initializing\n");

    // load the additional configuration items needed by the named data broker
    LoadAdditionalConfigurationItems(document);

    BepServer::Initialize(document);    // LoadTable must be done before initialize

    // Tell TaskMon that we are running (NDB does not get registered)
    ReportTaskUp();

    Log(LOG_FN_ENTRY, "NamedDataBroker Initialized\n");
}

const INT32 NamedDataBroker::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 retVal = BEP_STATUS_FAILURE;

    // if this is a reload configuration pulse
    if((code == SYSTEM_PULSE) && (value == RELOAD_PULSE))
    {
        m_servers.clear(true);    // clear the old server list

        ReloadConfiguration();         // tell BepServer to reload the configuration

        retVal = BEP_STATUS_SUCCESS;
    }
    else                               // let the BepComponent handle the other pulses
    {
        retVal = BepComponent::HandlePulse(code, value);
    }

    return(retVal);
}

void NamedDataBroker::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    LoadServerStatusTable(document->getChild("Setup/Servers")->getChildren());
}

const std::string NamedDataBroker::Read(const XmlNode *node, const INT32 rate /* = 0 */)
{
    Log(LOG_FN_ENTRY, "NamedDataBroker::Read(%s, %d)\n", node->ToString().c_str(), rate);
    std::string alias = BepServer::Read(node), server = BEP_UNAVAILABLE_RESPONSE;
    Log(LOG_DETAILED_DATA, "alias = %s\n", alias.c_str());
    if(m_comm->IsDataOk(alias))
    {   // Is alias ok
        try
        {   // Get server name
            server = m_servers.getNode(alias)->getValue();
            Log(LOG_DETAILED_DATA, "server = %s\n", server.c_str());
        }
        catch(...)
        {
            Log(LOG_ERRORS, "did not find alias %s\n", alias.c_str());
        }
    }
    else Log(LOG_ERRORS, "alias is whacked: %s\n", alias.c_str());
    Log(LOG_FN_ENTRY, "NamedDataBroker::Read(%s, %d) done\n", node->ToString().c_str(), rate);
    return(server);
}
const std::string NamedDataBroker::Write(const XmlNode *node)
{
    return(BEP_NO_IMPLEMENTED_RESPONSE);
}
const std::string NamedDataBroker::Command(const XmlNode *node)
{
    Log(LOG_FN_ENTRY, "NamedDataBroker::Command(%s)\n", node->ToString().c_str());
    std::string retVal = BEP_INVALID_RESPONSE, name, alias = BepServer::Read(node);
    if(m_comm->IsDataOk(alias))
    {
        try
        {
            XmlNode *attrib = const_cast<XmlNode *>(m_servers.getNode(alias)->getAttributes().getNode(BEP_STATUS));
            Log(LOG_DETAILED_DATA, "attrib = '%s'\n", attrib->ToString().c_str());
            if(node->getValue() == BEP_VERIFY)
            {   // If client requested server verification
                name = VerifyServer(node->getName());
                if(m_comm->IsDataOk(name))
                {   // If server name is not wacked
                    retVal = Read(node);
                    attrib->setValue(BEP_AVAILABLE_RESPONSE);
                }
                else retVal = name;
            }
            else if(node->getValue() == BEP_INVALIDATE)
            {   // If client requested server invalidation
                attrib->setValue(BEP_UNAVAILABLE_RESPONSE); // Set server as unavailable
                if(attrib->getValue() == BEP_UNAVAILABLE_RESPONSE)
                {   // If server was wacked
                    retVal = BEP_UNAVAILABLE_RESPONSE;
                }
            }
            else Log(LOG_ERRORS, "Unknown Ndb Command: %s\n", node->getValue().c_str());
        }
        catch(BepException& Err)
        {
            Log(LOG_ERRORS, "Error: %s\n", Err.what());
            retVal = BEP_INVALID_RESPONSE;
        }
        Log(LOG_FN_ENTRY, "NamedDataBroker::Command(%s, %s) returning: %s\n", node->getName().c_str(), node->getValue().c_str(), retVal.c_str());
    }
    else retVal = alias;
    return(retVal);
}
const std::string NamedDataBroker::Subscribe(const std::string &tag, const std::string &Process)
{
    return(BEP_NO_IMPLEMENTED_RESPONSE);
}
const std::string NamedDataBroker::Unsubscribe(const std::string &tag, const std::string &Process)
{
    return(BEP_NO_IMPLEMENTED_RESPONSE);
}
const std::string NamedDataBroker::Publish(const std::string &tag, const std::string &value)
{
    return(BEP_NO_IMPLEMENTED_RESPONSE);
}
void NamedDataBroker::Abort(void)
{
}
void NamedDataBroker::Reset(void)
{
    try
    {
        XmlParser parser;
        Initialize(parser.ReturnXMLDocument(GetConfigFile()));
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "NamedDataBroker::Reset Error: %s", e.what());
    }
}
void NamedDataBroker::LoadServerStatusTable(const XmlNodeMap &servers)
{   // the tag name and server registered task name
    Log(LOG_FN_ENTRY, "LoadServerStatusTable(%d)\n", servers.size());
    XmlString alias, name, status;
    XmlNode *newNode;
    for(UINT32 ii = 0; ii < servers.size(); ii++)
    {
        alias   = servers.getNode(ii)->getName();   // Get alias of server
        name    = servers.getNode(ii)->getValue();  // Get name of server
        status  = BEP_UNAVAILABLE_RESPONSE;         // invalidate the status for the verification
        for(INT32 jj = 0; (jj < 5) && (status != BEP_AVAILABLE_RESPONSE); jj++)
        {   // Try to connect to server 5 times before failing
            status = VerifyServer(name, true);
            newNode = new XmlNode(alias, name);
            newNode->addChild(BEP_STATUS, status, ATTRIBUTE_NODE);
            m_servers.addNode(newNode);
        }
        Log(LOG_FN_ENTRY, "server[%d] = %s\n", ii, m_servers.getNode(ii)->ToString().c_str());
    }
    Log(LOG_FN_ENTRY, "LoadServerStatusTable(%d) done\n", servers.size());
}
const std::string NamedDataBroker::VerifyServer(const std::string &tag, const bool IsServerName /* = false */)
{
    std::string status = BEP_UNAVAILABLE_RESPONSE, name = tag;
    Log(LOG_FN_ENTRY, "VerifyServer(%s) %d\n", tag.c_str(), m_servers.size());

    for(UINT32 ii = 0; ii < m_servers.size(); ii++)
    {
        Log(LOG_DETAILED_DATA, "server[%d] = %s\n", ii, m_servers.getNode(ii)->ToString().c_str());
    }

    XmlNode node(tag, "");
    if(!IsServerName) name = NamedDataBroker::Read(&node);  // Get process name
    Log(LOG_FN_ENTRY, "Verify server %s\n", name.c_str());
    if(m_comm->IsDataOk(name))
    {   // Is name ok
        if(BposIsTaskAlive(name.c_str()) == BEP_STATUS_SUCCESS)
        {   // Could we locate server?
            status = BEP_AVAILABLE_RESPONSE;
            Log(LOG_DETAILED_DATA, "Verified server %s done\n", name.c_str());
        }
        else Log(LOG_ERRORS, "Failed to verify server %s", name.c_str());
    }
    else Log(LOG_ERRORS, "Failed to find server for %s", name.c_str());
    Log(LOG_FN_ENTRY, "VerifyServer(%s) status: %s done\n", name.c_str(), status.c_str());
    return(status);
}

