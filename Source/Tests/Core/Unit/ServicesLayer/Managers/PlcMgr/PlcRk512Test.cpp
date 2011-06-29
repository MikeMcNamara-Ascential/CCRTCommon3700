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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/PlcMgr/PlcRk512Test.cpp $
// 
// 1     1/18/07 4:53p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "PlcRk512Test.h"
#include "PlcRk512.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
PlcRk512Test::PlcRk512Test()
{
}

//-----------------------------------------------------------------------------
PlcRk512Test::~PlcRk512Test()
{
}

//-----------------------------------------------------------------------------
void PlcRk512Test::setUp()
{
    plc = new PlcRk512();
}

//-----------------------------------------------------------------------------
void PlcRk512Test::tearDown()
{
    delete plc;
}

//-----------------------------------------------------------------------------
void PlcRk512Test::TestConstructor()
{
    CPPUNIT_ASSERT(plc->m_fromPlc == NULL);
    CPPUNIT_ASSERT(plc->m_toPlc == NULL);
    CPPUNIT_ASSERT_EQUAL(string(""),plc->m_rk512PortName);
    CPPUNIT_ASSERT_EQUAL((uint32_t)20,plc->m_commScanTime);
    CPPUNIT_ASSERT_EQUAL((uint32_t)16,plc->m_channelBits);
}


