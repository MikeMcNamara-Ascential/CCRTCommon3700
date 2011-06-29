//******************************************************************************
// Description:
//    Plant host outbound test class.
//
//==============================================================================
//
//     Copyright (c) 2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/PlantHostOutboundTest.cpp $
// 
// 6     5/01/07 1:35p Rwiersem
// Changes for the 07050201 core release:
// 
// Updated to use the new HostInterface class.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "PlantHostOutboundTest.h"
#include "PlantHostOutbound.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
PlantHostOutboundTest::PlantHostOutboundTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory"
            << endl;
        exit(1);
    }

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += 
        "/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/Configuration/";

    // set the USR_ROOT environment variable so that we can find the
    // configuration files
    m_originalUsrRoot = getenv("USR_ROOT");
    m_newUsrRoot = getenv("TEST_DIR_PATH");
    m_newUsrRoot += "/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/";
}

//-----------------------------------------------------------------------------
PlantHostOutboundTest::~PlantHostOutboundTest()
{
}

//-----------------------------------------------------------------------------
void PlantHostOutboundTest::setUp()
{   // Create a new PHO pbject
    pho = new PlantHostOutbound();

    // set the new USR_ROOT environment variable
    setenv("USR_ROOT",m_newUsrRoot.c_str(),1);
}

//-----------------------------------------------------------------------------
void PlantHostOutboundTest::tearDown()
{   
    // restore the USR_ROOT environment variable
    setenv("USR_ROOT",m_originalUsrRoot.c_str(),1);

    delete pho;
    
}

//-----------------------------------------------------------------------------
void PlantHostOutboundTest::TestConstructor(void)
{
    CPPUNIT_ASSERT(pho->m_testResultNode == NULL);
    CPPUNIT_ASSERT_EQUAL(false,pho->m_reportAbortResults);
}

//-----------------------------------------------------------------------------
void PlantHostOutboundTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "PlantHostOutboundConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    pho->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(true,pho->m_reportAbortResults);
}

//-----------------------------------------------------------------------------
void 
PlantHostOutboundTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "PlantHostOutboundConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    pho->LoadAdditionalConfigurationItems(document);

    // The node map should be empty.  So, trying to get a node should throw
    // an XmlException
    CPPUNIT_ASSERT_EQUAL(false,pho->m_reportAbortResults);
}

//-----------------------------------------------------------------------------
void PlantHostOutboundTest::TestStoreHostInterfaceMap(void)
{   // Define a few nodes
    XmlNode hostMap("HostInterfaces", "");
    XmlNode interface1("Interface1", "");
    XmlNode interface2("Interface2", "");
    // Add the required attributes to the interface nodes
    interface1.addChild("Enabled", "0", ATTRIBUTE_NODE);
    interface1.addChild("Process", "$(USR_ROOT)/ServicesLayer/Bin/TestExe1", 
                        ATTRIBUTE_NODE);
    interface2.addChild("Enabled", "1", ATTRIBUTE_NODE);
    interface2.addChild("Process", "$(USR_ROOT)/ServicesLayer/Bin/TestExe2", 
                        ATTRIBUTE_NODE);
    // Add the two interfaces to the host map
    hostMap.addChild(interface1.Copy());
    hostMap.addChild(interface2.Copy());
    // Store the interfaces
    pho->StoreHostInterfaceMap(&hostMap);
    // Verify there is only one item stored
    CPPUNIT_ASSERT(1 == pho->m_hostInterfaceMap.size());
    CPPUNIT_ASSERT(pho->m_hostInterfaceMap.begin()->first == 
                   interface2.getName());
    // Verify the clear works correctly
    pho->ClearHostInterfaceMap();
    CPPUNIT_ASSERT(0 == pho->m_hostInterfaceMap.size());
    CPPUNIT_ASSERT(pho->m_hostInterfaceMap.begin() == 
                   pho->m_hostInterfaceMap.end());
}

//-----------------------------------------------------------------------------
void PlantHostOutboundTest::TestHandlePulse(void)
{
    string testResultHeaderCompareString;
    XmlParser parser;
    string configFile = m_configPath + "PlantHostOutboundConfigReload.xml";

    // set a new configuration file so that we can make sure it was reloaded
    pho->SetConfigFile(configFile);

    // send the reload configuration pulse
    pho->HandlePulse(SYSTEM_PULSE,RELOAD_PULSE);

    // Plant Host Outbound items
    CPPUNIT_ASSERT_EQUAL(false,pho->m_reportAbortResults);
}
