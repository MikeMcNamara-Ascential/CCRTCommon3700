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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/TaskMon/Common/TaskMonTaskTest.cpp $
// 
// 1     10/26/06 2:53p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================
#include "TaskMonTaskTest.h"
#include "TaskMonTask.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
TaskMonTaskTest::TaskMonTaskTest()
{
}

//-----------------------------------------------------------------------------
TaskMonTaskTest::~TaskMonTaskTest()
{
}

//-----------------------------------------------------------------------------
void TaskMonTaskTest::setUp()
{
    tmt = new TaskMonTask();
}

//-----------------------------------------------------------------------------
void TaskMonTaskTest::tearDown()
{
    delete tmt;
}

//-----------------------------------------------------------------------------
void TaskMonTaskTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL(TASKSTATE_UNREG,tmt->m_taskState);
    CPPUNIT_ASSERT_EQUAL(-1,tmt->m_taskPid);
    CPPUNIT_ASSERT_EQUAL((double)0.0,tmt->m_cpuUsage);
    CPPUNIT_ASSERT_EQUAL(TASKMON_CLASS_END,tmt->m_taskClass);
    CPPUNIT_ASSERT_EQUAL((uint32_t)0,tmt->m_threads);
    CPPUNIT_ASSERT_EQUAL((uint32_t)0,tmt->m_fds);
    CPPUNIT_ASSERT_EQUAL(string(""),tmt->m_taskName);
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,tmt->m_lastSampleTime);
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,tmt->m_cpuTime);
    CPPUNIT_ASSERT_EQUAL(0,tmt->m_aliveCount);
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,tmt->m_memoryUsage);
    CPPUNIT_ASSERT_EQUAL(-1,tmt->m_pidFd);
    CPPUNIT_ASSERT_EQUAL(string(""),tmt->m_pidPath);
}

//-----------------------------------------------------------------------------
void TaskMonTaskTest::TestConstructor_Copy(void)
{
    TaskMonTask_t copy;

    // set the data in the task we are going to pass in
    copy.m_taskState = TASKSTATE_REG;
    copy.m_taskPid = 11;
    copy.m_cpuUsage = 25.1;
    copy.m_memoryUsage = (uint64_t)1000.0;
    copy.m_threads = 7;
    copy.m_fds = 8;
    copy.m_taskClass = TASKMON_CLASS_CORE;

    TaskMonTask* tmt1 = new TaskMonTask(copy);

    CPPUNIT_ASSERT_EQUAL(TASKSTATE_REG,tmt1->m_taskState);
    CPPUNIT_ASSERT_EQUAL(11,tmt1->m_taskPid);
    CPPUNIT_ASSERT_EQUAL((double)25.1,tmt1->m_cpuUsage);
    CPPUNIT_ASSERT_EQUAL((uint64_t)1000.0,tmt1->m_memoryUsage);
    CPPUNIT_ASSERT_EQUAL(TASKMON_CLASS_CORE,tmt1->m_taskClass);
    CPPUNIT_ASSERT_EQUAL((uint32_t)7,tmt1->m_threads);
    CPPUNIT_ASSERT_EQUAL((uint32_t)8,tmt1->m_fds);
    CPPUNIT_ASSERT_EQUAL(string(""),tmt1->m_taskName);
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,tmt1->m_lastSampleTime);
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,tmt1->m_cpuTime);
    CPPUNIT_ASSERT_EQUAL(0,tmt1->m_aliveCount);
    CPPUNIT_ASSERT_EQUAL(-1,tmt1->m_pidFd);
    CPPUNIT_ASSERT_EQUAL(string("/proc/11/as"),tmt1->m_pidPath);
}

//-----------------------------------------------------------------------------
void TaskMonTaskTest::TestConstructor_Params(void)
{
    TaskMonTask* tmt1 = new TaskMonTask("TestName",12,TASKSTATE_RUN,
                                        TASKMON_CLASS_SERIAL,65.0,1234,2,12);

    CPPUNIT_ASSERT_EQUAL(TASKSTATE_RUN,tmt1->m_taskState);
    CPPUNIT_ASSERT_EQUAL(12,tmt1->m_taskPid);
    CPPUNIT_ASSERT_EQUAL((double)65.0,tmt1->m_cpuUsage);
    CPPUNIT_ASSERT_EQUAL((uint64_t)1234,tmt1->m_memoryUsage);
    CPPUNIT_ASSERT_EQUAL((uint32_t)2,tmt1->m_threads);
    CPPUNIT_ASSERT_EQUAL((uint32_t)12,tmt1->m_fds);
    CPPUNIT_ASSERT_EQUAL(TASKMON_CLASS_SERIAL,tmt1->m_taskClass);
    CPPUNIT_ASSERT_EQUAL(string("TestName"),tmt1->m_taskName);
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,tmt1->m_lastSampleTime);
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,tmt1->m_cpuTime);
    CPPUNIT_ASSERT_EQUAL(0,tmt1->m_aliveCount);
    CPPUNIT_ASSERT_EQUAL(-1,tmt1->m_pidFd);
    CPPUNIT_ASSERT_EQUAL(string("/proc/12/as"),tmt1->m_pidPath);
}

//-----------------------------------------------------------------------------
void TaskMonTaskTest::TestGetThreadCount(void)
{
    tmt->m_threads = 10;

    CPPUNIT_ASSERT_EQUAL((uint32_t)10,tmt->GetThreadCount());
}

//-----------------------------------------------------------------------------
void TaskMonTaskTest::TestSetThreadCount(void)
{
    tmt->SetThreadCount(11);

    CPPUNIT_ASSERT_EQUAL((uint32_t)11,tmt->m_threads);
}

//-----------------------------------------------------------------------------
void TaskMonTaskTest::TestGetOpenFds(void)
{
    tmt->m_fds = 7;

    CPPUNIT_ASSERT_EQUAL((uint32_t)7,tmt->GetOpenFds());
}

//-----------------------------------------------------------------------------
void TaskMonTaskTest::TestSetOpenFds(void)
{
    tmt->SetOpenFds(8);

    CPPUNIT_ASSERT_EQUAL((uint32_t)8,tmt->m_fds);
}
