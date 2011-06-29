//******************************************************************************
// Description:
//  The test class for testing the HydraulicMotorController class.
//
//==============================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/MotorController/Hydraulic/HydraulicMotorControllerTest.h $
// 
// 2     1/18/07 5:01p Rwiersem
// Changes for the 07011901 core release:
//       
// - Added MockNamedDataBroker.
// - Added TestBoost_DisableVehiclePresent() and 
// TestBoost_EnableVehiclePresent().
// - Removed TestHandlePulse().
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef HydraulicMotorControllerTest_h
#define HydraulicMotorControllerTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class HydraulicMotorController;

class HydraulicMotorControllerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(HydraulicMotorControllerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(TestBoost_DisableVehiclePresent);
    CPPUNIT_TEST(TestBoost_EnableVehiclePresent);
    CPPUNIT_TEST_SUITE_END();

public:
    HydraulicMotorControllerTest();
    ~HydraulicMotorControllerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the HydraulicMotorController object is 
     * constructed properly.
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the HydraulicMotorController configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly sets the configuration items to the default
     * if the tags are missing from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);
    /**
     * Test that will verify Boost() turns off the boost bit if vehicle present
     * is lost and the parameter to check this condition is set.
     */ 
    void TestBoost_DisableVehiclePresent(void);
    /**
     * Test that will verify Boost() turns on the boost bit if vehicle present
     * is lost and the parameter to check this condition is not set.
     */ 
    void TestBoost_EnableVehiclePresent(void);

    /** HydraulicMotorController object to use for testing */
    HydraulicMotorController *hmc;

    /** The path to the test configuration files */
    string m_configPath;

    /** Task id for the mock named data broker */
    int m_ndbId;
};
#endif
