//******************************************************************************
// Description:
//    System screen saver test class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/SystemScreenSaver/SystemScreenSaverTest.h $
// 
// 1     5/01/07 1:48p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef SystemScreenSaverTest_h
#define SystemScreenSaverTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class SystemScreenSaver;

class SystemScreenSaverTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SystemScreenSaverTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(ReadParametersTest);
    CPPUNIT_TEST(DoActionTest_ActionFound);
    CPPUNIT_TEST(DoActionTest_ActionNotFound);
    CPPUNIT_TEST_SUITE_END();

public:
    SystemScreenSaverTest();
    ~SystemScreenSaverTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the SystemScreenSaver object is constructed 
     * properly.
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify ReadParameters() properly reads the configuration
     * items from the configuration file.
     */ 
    void ReadParametersTest(void);
    /**
     * Test that will verify DoAction() properly handles the case where the
     * action is found in the list.
     */ 
    void DoActionTest_ActionFound(void);
    /**
     * Test that will verify DoAction() properly handles the case where the
     * action is not found in the list.
     */ 
    void DoActionTest_ActionNotFound(void);

private:
    /** SystemScreenSaver object to use for testing */
    SystemScreenSaver *sss;

    /** The path to the test configuration files */
    string m_configPath;
};
#endif // SystemScreenSaver_h
