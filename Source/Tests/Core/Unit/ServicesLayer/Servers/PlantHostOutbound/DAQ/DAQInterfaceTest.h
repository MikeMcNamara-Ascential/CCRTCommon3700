//******************************************************************************
// Description:
//    DAQ interface test class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/DAQ/DAQInterfaceTest.h $
// 
// 1     5/01/07 1:44p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef DAQInterfaceTest_h
#define DAQInterfaceTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class DAQInterface;

class DAQInterfaceTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DAQInterfaceTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(TestWriteFaultCode);
    CPPUNIT_TEST_SUITE_END();

public:
    DAQInterfaceTest();
    ~DAQInterfaceTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that verifies the DAQ Interface object is constructed
     * properly
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the DAQInterface configuration items
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
     * Verify that writing a fault code to the DAQ interface stores the fault 
     * and sets the flag indicating the fault is ready to be processed.
     */
    void TestWriteFaultCode(void);
    /** The DAQInterface object under test */
    DAQInterface *m_daq;
    /** The path to the test configuration files */
    string m_configPath;
    /** Used to store the original USR_ROOT environment variable */
    string m_originalUsrRoot;
    /** Used to store the new USR_ROOT environment variable */
    string m_newUsrRoot;
    /** Task id for the mock named data broker */
    int m_ndbId;
};
#endif
