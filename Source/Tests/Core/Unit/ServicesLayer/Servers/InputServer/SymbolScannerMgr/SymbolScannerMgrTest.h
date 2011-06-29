//******************************************************************************
// Description:
//   Test class for testing the SymbolScannerMgr class.
//==============================================================================
// Copyright (c) 2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/InputServer/SymbolScannerMgr/SymbolScannerMgrTest.h $
// 
// 2     2/22/07 12:46p Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added EvaluateData_UseMaxVinLength() and EvaluateData_UseVinLength().
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef SymbolScannerMgrTest_h
#define SymbolScannerMgrTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class SymbolScannerMgr;

class SymbolScannerMgrTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SymbolScannerMgrTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(EvaluateData_UseMaxVinLength);
    CPPUNIT_TEST(EvaluateData_UseVinLength);
    CPPUNIT_TEST_SUITE_END();

public:
    SymbolScannerMgrTest();
    ~SymbolScannerMgrTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the SymbolScannerMgr object is constructed 
     * properly.
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the SymbolScannerMgr configuration items
     * from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the items set in the 
     * LoadAdditionalConfigurationItems() method are set to the default
     * if the tag is missing in the configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);
    /**
     * Test that will verify EvaluateData() works as expected when the VIN
     * string passed in should use the max vin length.
     */ 
    void EvaluateData_UseMaxVinLength(void);
    /**
     * Test that will verify EvaluateData() works as expected when the VIN
     * string passed in should use the vin length.
     */ 
    void EvaluateData_UseVinLength(void);

    /** The SymbolScannerMgr object under test */
    SymbolScannerMgr *ssm;

private:

    /** The path to the test configuration files */
    string m_configPath;

    /** Task id for the mock named data broker */
    int m_ndbId;
};
#endif
