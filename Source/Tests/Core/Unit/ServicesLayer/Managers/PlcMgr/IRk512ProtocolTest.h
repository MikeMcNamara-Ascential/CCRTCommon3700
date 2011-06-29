//******************************************************************************
// Description:
//  The unit tests for the IRk512Protocol class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/PlcMgr/IRk512ProtocolTest.h $
// 
// 1     1/18/07 4:52p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef IRk512ProtocolTest_h
#define IRk512ProtocolTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class IRk512Protocol;

class IRk512ProtocolTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(IRk512ProtocolTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST_SUITE_END();

public:
    IRk512ProtocolTest();
    ~IRk512ProtocolTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the IRk512Protocol object is constructed properly
     */ 
    void TestConstructor(void);

    /** The IRk512Protocol object under test */
    IRk512Protocol *plc;
};
#endif
