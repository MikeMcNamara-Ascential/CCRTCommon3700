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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/PlantHostOutboundTest.h $
// 
// 5     5/01/07 1:36p Rwiersem
// Changes for the 07050201 core release:
// 
// Updated to use the new HostInterface class.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef PlantHostOutboundTest_h
#define PlantHostOutboundTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class PlantHostOutbound;
class HostInterfaceProcessMap;

class PlantHostOutboundTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PlantHostOutboundTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(TestStoreHostInterfaceMap);
    CPPUNIT_TEST(TestHandlePulse);
    CPPUNIT_TEST_SUITE_END();

public:
    PlantHostOutboundTest();
    ~PlantHostOutboundTest();

    void setUp();
    void tearDown();

    /**
     * Test that verifies the plant host outbound object is constructed
     * properly
     */
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionalConfigurationItems()
     * method properly reads the PlantHostOutbound configuration items
     * from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the LoadAdditionalConfigurationItems() method
     * properly loads the default values if the tags are missing in the
     * configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);
    /**
     * Verify the host interface map is stored properly.
     */
    void TestStoreHostInterfaceMap(void);
    /**
     * Test that will verify the HandlePulse() method properly processes the
     * reload configuration pulse.
     */
    void TestHandlePulse(void);

    /** The PlantHostOutbound object under test */
    PlantHostOutbound *pho;

    /** The path to the test configuration files */
    string m_configPath;

    /** Used to store the original USR_ROOT environment variable */
    string m_originalUsrRoot;

    /** Used to store the new USR_ROOT environment variable */
    string m_newUsrRoot;
};
#endif
