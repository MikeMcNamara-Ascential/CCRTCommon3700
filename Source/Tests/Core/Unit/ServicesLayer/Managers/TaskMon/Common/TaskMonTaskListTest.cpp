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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/TaskMon/Common/TaskMonTaskListTest.cpp $
// 
// 1     10/26/06 2:53p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================
#include "TaskMonTaskListTest.h"
#include "TaskMonTaskList.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
TaskMonTaskListTest::TaskMonTaskListTest()
{
}

//-----------------------------------------------------------------------------
TaskMonTaskListTest::~TaskMonTaskListTest()
{
}

//-----------------------------------------------------------------------------
void TaskMonTaskListTest::setUp()
{
    tl = new TaskList();
}

//-----------------------------------------------------------------------------
void TaskMonTaskListTest::tearDown()
{
    delete tl;
}

//-----------------------------------------------------------------------------
void TaskMonTaskListTest::TestUpdate_Threads()
{
    bool status=false;
    TaskMonTask* tmt;

    // Add a new task
    tmt = tl->Add("TestTask",11,TASKSTATE_REG,TASKMON_CLASS_CORE,
                  TASKUPDATE_NONE);

    tmt->SetThreadCount(3);

    // Update the thread information for this task
    status = tl->Update(*tmt,TASKUPDATE_THREADS);

    // make sure the update was successful
    CPPUNIT_ASSERT_EQUAL(true,status);

    // find the task we added
    TaskListItr_t itr;
    if((itr=tl->find(tmt->GetName())) != tl->end())
    {
        TaskMonTask *curTask = itr->second;

        // make sure the thread count was updated correctly
        CPPUNIT_ASSERT_EQUAL((uint32_t)3,curTask->GetThreadCount());
    }
    else
    {
        CPPUNIT_FAIL("TaskMonTask not found in TestUpdate_Threads");
    }

    delete tmt;
}

//-----------------------------------------------------------------------------
void TaskMonTaskListTest::TestUpdate_FileDescriptors(void)
{
    bool status=false;
    TaskMonTask* tmt;

    // Add a new task
    tmt = tl->Add("TestTask2",12,TASKSTATE_RUN,TASKMON_CLASS_SERIAL,
                  TASKUPDATE_NONE);

    tmt->SetOpenFds(2);

    // Update the open file descriptors for this task
    status = tl->Update(*tmt,TASKUPDATE_FDS);

    // make sure the update was successful
    CPPUNIT_ASSERT_EQUAL(true,status);

    // find the task we added
    TaskListItr_t itr;
    if((itr=tl->find(tmt->GetName())) != tl->end())
    {
        TaskMonTask *curTask = itr->second;

        // make sure the open file descriptor count was updated correctly
        CPPUNIT_ASSERT_EQUAL((uint32_t)2,curTask->GetOpenFds());
    }
    else
    {
        CPPUNIT_FAIL("TaskMonTask not found in TestUpdate_FileDescriptors");
    }

    delete tmt;
}

//-----------------------------------------------------------------------------
void TaskMonTaskListTest::TestUpdate_TaskNotFound(void)
{
    bool status=true;
    TaskMonTask tmt;

    // try to update a task that isn't in the list
    status = tl->Update(tmt,TASKUPDATE_THREADS);

    // make sure the operation failed
    CPPUNIT_ASSERT_EQUAL(false,status);
}
