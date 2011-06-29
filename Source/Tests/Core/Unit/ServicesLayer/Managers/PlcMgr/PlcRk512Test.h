//******************************************************************************
// Description:
//  The unit tests for the PlcRk512 class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/PlcMgr/PlcRk512Test.h $
// 
// 1     1/18/07 4:53p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef PlcRk512Test_h
#define PlcRk512Test_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class PlcRk512;

class PlcRk512Test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PlcRk512Test);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST_SUITE_END();

public:
    PlcRk512Test();
    ~PlcRk512Test();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the PlcRk512 object is constructed properly
     */ 
    void TestConstructor(void);

    /** The PlcRk512 object under test */
    PlcRk512 *plc;
};
#endif
