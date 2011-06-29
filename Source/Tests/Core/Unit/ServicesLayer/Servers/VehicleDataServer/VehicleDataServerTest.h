//******************************************************************************
// Description:
//  The Test Class For Testing the Vehicle Data Server Class.
//
//==============================================================================
// Copyright (c) 2005 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/VehicleDataServer/VehicleDataServerTest.h $
// 
// 4     10/31/07 10:29a Rwiersem
// Changes for the 07103101 core release:
// 
// - Removed TestHandlePulse().
// 
// 3     10/26/06 3:17p Rwiersem
// Changes for the 06102701 core release:
// 
// Now register the name "core/NamedDataBroker".
// 
// 2     8/02/06 8:35p Cward
// Updated VehicleDataServer Tests to test for compatibility with older
// builds
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef VEHICLEDATASERVERTEST_H
#define VEHICLEDATASERVERTEST_H
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include "DebugLevel.h"
#include "VehicleDataServer.h"

using namespace std;

class VehicleDataServer;

class VehicleDataServerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(VehicleDataServerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(ReadTest);
    CPPUNIT_TEST(InitializationTest1);
    CPPUNIT_TEST(InitializationTest0);
    CPPUNIT_TEST(InitializationTest2);
    CPPUNIT_TEST(InitializationTest3);
    CPPUNIT_TEST(PublishTest);
    CPPUNIT_TEST(ReadTest2);
    CPPUNIT_TEST(ReadTest3);
    CPPUNIT_TEST(WheelbaseTest1);
    CPPUNIT_TEST(WheelbaseTest2);
    CPPUNIT_TEST_SUITE_END();

public:
    VehicleDataServerTest();
    ~VehicleDataServerTest();

    /**
     * Setup is run before each test, use it to
     * set up the member ivars for use during
     * the test
     */
    void setUp();

    /**
     * TearDown is called after each test, use it
     * if you need to clean things up after your
     * test
     */
    void tearDown();

    /**
     * Factored out helper to do debug printing
     */
    void PTHelp1(VehicleDataServer *vds);

    /**
     * Test that will verify the VehicleDataServer object is
     * constructed properly
     */
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the VehicleDataServer configuration items
     * from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the items set in the
     * LoadAdditionalConfiguraitonItems() method are set to the default
     * if the tag is missing in the configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);
    /**
     * Test that will verify the Read() method
     */
    void ReadTest(void);

    /**
     * Test to verify that initialization sets the correct values
     * based on the configuration file selected
     */
    void InitializationTest1(void);
    void InitializationTest0(void);
    void InitializationTest2(void);
    void InitializationTest3(void);

    /**
     * Test the logic imbedded in the Publish method, that handles
     * vin numbers and test in progress
     */
    void PublishTest(void);

    /**
     * more extensive test of the functionality of
     * the entire read, using the Korea build.
     */
    void ReadTest2(void);
    /**
     * more extensive test of the functionality of
     * the entire read method, using the HMMA build.
     */
    void ReadTest3(void);

    /**
     * test the scalar wheelbase decode (not used by any customer at this time)
     */
    void WheelbaseTest1(void);
    void WheelbaseTest2(void);

    /** The VehicleDataServer object under test */
    VehicleDataServer *vds;

    /** The path to the test configuration files */
    string m_configPath;
    /**
     * Variable that holds the current debug level
     */
    DebugLevel m_debug;

    /** Task id for the mock named data broker */
    int m_ndbId;
};
#endif
