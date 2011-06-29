//*************************************************************************
// Description:
//   Tests for the TaskMonTask class.
//
//===========================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/TaskMon/Common/TaskMonTaskTest.h $
// 
// 1     10/26/06 2:53p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================
#ifndef TaskMonTaskTest_h
#define TaskMonTaskTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

class TaskMonTask;

class TaskMonTaskTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TaskMonTaskTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestConstructor_Copy);
    CPPUNIT_TEST(TestConstructor_Params);
    CPPUNIT_TEST(TestGetThreadCount);
    CPPUNIT_TEST(TestSetThreadCount);
    CPPUNIT_TEST(TestGetOpenFds);
    CPPUNIT_TEST(TestSetOpenFds);
    CPPUNIT_TEST_SUITE_END();

public:
    TaskMonTaskTest();
    ~TaskMonTaskTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the TaskMonTask object is constructed properly.
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the TaskMonTask object is constructed properly
     * when passing in an object to copy.
     */ 
    void TestConstructor_Copy(void);
    /**
     * Test that will verify the TaskMonTask object is constructed properly
     * when passing in the parameters individually.
     */ 
    void TestConstructor_Params(void);
    /**
     * Test that will verify the GetThreadCount() method properly returns
     * the current thread count.
     */ 
    void TestGetThreadCount(void);
    /**
     * Test that will verify the SetThreadCount() method properly sets
     * the current thread count.
     */
    void TestSetThreadCount(void);
    /**
     * Test that will verify the GetOpenFds() method properly returns
     * the current open file descriptor count.
     */
    void TestGetOpenFds(void);
    /**
     * Test that will verify the SetOpenFds() method properly sets
     * the current open file descriptor count.
     */
    void TestSetOpenFds(void);

    /** The TaskMonTask object under test */
    TaskMonTask *tmt;
};
#endif
