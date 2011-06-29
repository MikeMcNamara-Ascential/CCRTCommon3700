//******************************************************************************
// Description:
//   Test class for testing the SendModuleKeepAlive class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/SendModuleKeepAliveTest.cpp $
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
#include "SendModuleKeepAlive.h"
#include "SendModuleKeepAliveTest.h"
#include "KeywordProtocolFilter.h"

//=============================================================================
SendModuleKeepAliveTest::SendModuleKeepAliveTest(void)
{
}

//=============================================================================
SendModuleKeepAliveTest::~SendModuleKeepAliveTest(void)
{
}

//=============================================================================
void SendModuleKeepAliveTest::setUp()
{
    // create the object to use for testing
    KeepAliveTimer_t messageTimer;
    pf = new KeywordProtocolFilter(messageTimer);
    smka = new SendModuleKeepAlive(pf,"KeepAliveTag",2000,true);
}

//=============================================================================
void SendModuleKeepAliveTest::tearDown()
{
    // turn off the screen prints for destruction
    smka->m_enablePrintfs = false;

    delete smka;
    delete pf;
}

//=============================================================================
void SendModuleKeepAliveTest::ConstructorTest(void)
{
    CPPUNIT_ASSERT_EQUAL(string("KeepAliveTag"),smka->m_keepAliveMessageTag);
    CPPUNIT_ASSERT_EQUAL(true,smka->m_responseRequired);
    CPPUNIT_ASSERT_EQUAL((INT32)2000,smka->m_keepAliveTime);
}
