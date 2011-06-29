//******************************************************************************
// Description:
//  The GenericTC test class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Common/GenericTcTest.h $
// 
// 2     5/01/07 1:18p Rwiersem
// Changes for the 07050201 core release:
// 
// Added GetNamedDataBrokerTest().
// 
// 1     10/26/06 1:48p Rwiersem
// Changes for the 06102701 core release:
// 
// Intial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericTcTest_h
#define GenericTcTest_h
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"
#include "XmlParser.h"

class GenericTC;

class GenericTcTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(GenericTcTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(SendSubtestResultWithDetailTest);
    CPPUNIT_TEST(TestInitialize_ConfigFile);
    CPPUNIT_TEST(TestInitialize_RegisteredName);
    CPPUNIT_TEST(TestInitialize_IPCNameAlias);
    CPPUNIT_TEST(TestInitialize_ObjectiveSize);
    CPPUNIT_TEST(TestInitialize_TestStepsSize);
    CPPUNIT_TEST(TestInitialize_ComponentName);
    CPPUNIT_TEST(TestInitialize_ParameterSize);
    CPPUNIT_TEST(TestInitialize_IPromptCreated);
    CPPUNIT_TEST(TestInitialize_IFaultCreated);
    CPPUNIT_TEST(TestInitialize_ITestResultCreated);
    CPPUNIT_TEST(TestInitialize_ParkBrakeDirections);
    CPPUNIT_TEST(TestShouldReloadConfigFile_ReloadSameConfigFile);
    CPPUNIT_TEST(TestShouldReloadConfigFile_ReloadNewConfigFile);
    CPPUNIT_TEST(GetNamedDataBrokerTest);
    CPPUNIT_TEST_SUITE_END();

public:
    GenericTcTest();
    virtual ~GenericTcTest();
    /**
     * Setup is run before each test, use it to set up the member ivars for use 
     * during the test
     */
    void setUp();
    /**
     * TearDown is called after each test, use it if you need to clean things up 
     * after your test
     */
    void tearDown();
    /**
     * Test that will verify that the object is correctly initialized on 
     * construction.
     */
    void ConstructorTest(void);
    /**
     * Test that will verify that SendSubtestResultWithDetail() works properly
     * for the successful case.
     */ 
    void SendSubtestResultWithDetailTest(void);

    /**
     * Tests that the config file name gets set properly
     */
    void TestInitialize_ConfigFile(void);
    
    /**
     * Tests that the component registers the correct IPC name
     */
    void TestInitialize_RegisteredName(void);
    
    /**
     * Tests that the component creates the correct IPC name alias
     */
    void TestInitialize_IPCNameAlias(void);
    
    /**
     * Tests that the list of objectives is the correct size
     */
    void TestInitialize_ObjectiveSize(void);
    
    /**
     * Tests that the list of test steps is the correct size
     */
    void TestInitialize_TestStepsSize(void);
    
    /**
     * Test that the component sets the correct name 
     * (different than the IPC name)
     */
    void TestInitialize_ComponentName(void);
    
    /**
     * Tests that the list of parameters is the correct size
     */
    void TestInitialize_ParameterSize(void);
    
    /**
     * Test that the component creates a INamedDataBroker interface
     */
    void TestInitialize_INDBCreated(void);
    
    /**
     * Test that the component creates a IPromptServer interface
     */
    void TestInitialize_IPromptCreated(void);
    
    /**
     * Test that the component creates a IFaultServer interface
     */
    void TestInitialize_IFaultCreated(void);
    
    /**
     * Test that the component creates a ITestResultServer interface
     */
    void TestInitialize_ITestResultCreated(void);

    /**
     * Test that the component correctly fills the list of valid park brake 
     * test directions
     */
    void TestInitialize_ParkBrakeDirections(void);

    /**
     * Test that the component recognizes that the config file does not need 
     * to be reloaded
     */
    void TestShouldReloadConfigFile_ReloadSameConfigFile(void);

    /**
     * Test that the component recognizes that the config file does need 
     * to be reloaded
     */
    void TestShouldReloadConfigFile_ReloadNewConfigFile(void);

    /**
     * Test that will verify GetNamedDataBroker() properly returns the named
     * data broker object member variable.
     */ 
    void GetNamedDataBrokerTest(void);

private:

    /* Initialize the GenericTC object */
    void InitializeObject(void);

    /** Generic test component under test */
    GenericTC* gtc;

    /** Task id for the mock motor controller */
    int m_motorControllerId;

    /** Task id for the mock named data broker */
    int m_ndbId;

    /** Task id for the mock prompt server */
    int m_promptServerId;

    /** Task id for the mock fault server */
    int m_faultServerId;

    /** Task id for the mock test result server */
    int m_testResultServerId;

    /** Task id for the test component */
    int m_componentId;

    /** Task id for the icm manager */
    int m_icmId;

    /** The directory to find the configuration files in */
    std::string m_configDir;

    /** The configuration file for the component to load */
    std::string m_configFile;

    /** The IPC channel name that the component should register */
    std::string m_regName;

    /** The IPC channel name link that the component should create */
    std::string m_regAlias;

    /** Parser used to parse the component config file */
    XmlParser m_xmlParser;
};
#endif //GenericTcTest_h
