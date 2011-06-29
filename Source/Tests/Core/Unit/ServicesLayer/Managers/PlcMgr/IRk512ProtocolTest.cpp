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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/PlcMgr/IRk512ProtocolTest.cpp $
// 
// 1     1/18/07 4:52p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "IRk512ProtocolTest.h"
#include "IRk512Protocol.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
IRk512ProtocolTest::IRk512ProtocolTest()
{
}

//-----------------------------------------------------------------------------
IRk512ProtocolTest::~IRk512ProtocolTest()
{
}

//-----------------------------------------------------------------------------
void IRk512ProtocolTest::setUp()
{
    plc = new IRk512Protocol();
}

//-----------------------------------------------------------------------------
void IRk512ProtocolTest::tearDown()
{
    delete plc;
}

//-----------------------------------------------------------------------------
void IRk512ProtocolTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL(128,plc->m_rk512BytesPerBlock);
    CPPUNIT_ASSERT_EQUAL((unsigned)0,plc->m_rk512InitCount);
    CPPUNIT_ASSERT_EQUAL(-1,plc->m_rk512ComDev);
    CPPUNIT_ASSERT_EQUAL((uint16_t)0,plc->m_rk512TnsCounter);
    CPPUNIT_ASSERT_EQUAL((uint8_t)0,plc->m_rk512SrcId);
}

