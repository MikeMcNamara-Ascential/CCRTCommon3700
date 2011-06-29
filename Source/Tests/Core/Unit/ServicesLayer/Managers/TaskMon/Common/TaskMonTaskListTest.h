//*************************************************************************
// Description:
//   Tests for the TaskMonTaskList class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/TaskMon/Common/TaskMonTaskListTest.h $
// 
// 1     10/26/06 2:53p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================
#ifndef TaskMonTaskListTest_h
#define TaskMonTaskListTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

class TaskList;

class TaskMonTaskListTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TaskMonTaskListTest);
    CPPUNIT_TEST(TestUpdate_Threads);
    CPPUNIT_TEST(TestUpdate_FileDescriptors);
    CPPUNIT_TEST(TestUpdate_TaskNotFound);
    CPPUNIT_TEST_SUITE_END();

public:
    TaskMonTaskListTest();
    ~TaskMonTaskListTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the Update() method properly updates the current
     * thread count.
     */ 
    void TestUpdate_Threads(void);
    /**
     * Test that will verify the Update() method properly updates the current
     * open file descriptor count.
     */ 
    void TestUpdate_FileDescriptors(void);
    /**
     * Test that will verify the Update() method returns the correct value
     * when the task can not be found in the task list.
     */ 
    void TestUpdate_TaskNotFound(void);

    /** The TaskMonTaskList object under test */
    TaskList *tl;
};
#endif
