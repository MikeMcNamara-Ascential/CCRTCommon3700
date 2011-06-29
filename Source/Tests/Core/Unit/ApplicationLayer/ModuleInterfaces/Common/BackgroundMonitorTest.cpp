//******************************************************************************
// Description:
//   Test class for testing the BackgroundMonitor class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/BackgroundMonitorTest.cpp $
// 
// 1     10/26/06 1:40p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/resource.h>
#include <malloc.h>
#include "BackgroundMonitor.h"
#include "MockBackgroundMonitor.h"
#include "BackgroundMonitorTest.h"
#include "KeywordProtocolFilter.h"

//=============================================================================
BackgroundMonitorTest::BackgroundMonitorTest(void)
{
}

//=============================================================================
BackgroundMonitorTest::~BackgroundMonitorTest(void)
{
}

//=============================================================================
void BackgroundMonitorTest::setUp()
{
    // create the object to use for testing
    KeepAliveTimer_t messageTimer;
    pf = new KeywordProtocolFilter(messageTimer);
    mbm = new MockBackgroundMonitor(pf,2000);
}

//=============================================================================
void BackgroundMonitorTest::tearDown()
{
    // turn off the screen prints for destruction
    mbm->m_enablePrintfs = false;

    delete mbm;
    delete pf;
}

//=============================================================================
void BackgroundMonitorTest::ConstructorTest(void)
{
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,mbm->m_lastSendTime);
    CPPUNIT_ASSERT_EQUAL(false,mbm->m_threadJoined);
    CPPUNIT_ASSERT_EQUAL((INT32)2000,mbm->m_interMessageGap);
    CPPUNIT_ASSERT(mbm->m_vehicleComm == pf);
    CPPUNIT_ASSERT_EQUAL(true,mbm->m_enablePrintfs);
}

//=============================================================================
void BackgroundMonitorTest::SetInterMessageGapTest(void)
{
    mbm->SetInterMessageGap(3000);

    CPPUNIT_ASSERT_EQUAL((INT32)3000,mbm->m_interMessageGap);
}
