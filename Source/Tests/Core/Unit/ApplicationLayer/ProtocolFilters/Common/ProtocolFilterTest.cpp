//******************************************************************************
// Description:
//   Test class for testing the ProtocolFilter class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ProtocolFilters/Common/ProtocolFilterTest.cpp $
// 
// 2     2/22/07 11:25a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added TestCloseCom().
// 
// 1     10/26/06 1:45p Rwiersem
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
#include "MockProtocolFilter.h"
#include "ProtocolFilterTest.h"

//=============================================================================
ProtocolFilterTest::ProtocolFilterTest(void)
{
}

//=============================================================================
ProtocolFilterTest::~ProtocolFilterTest(void)
{
}

//=============================================================================
void ProtocolFilterTest::setUp()
{
    // create the object to use for testing
    pf = new MockProtocolFilter(messageTimer);
}

//=============================================================================
void ProtocolFilterTest::tearDown()
{
    delete pf;
}

//=============================================================================
void ProtocolFilterTest::ConstructorTest(void)
{
    CPPUNIT_ASSERT_EQUAL((UINT16)1,pf->m_numberOfRetries);
    CPPUNIT_ASSERT_EQUAL(false,pf->m_checksumValidationRequired);
    CPPUNIT_ASSERT(pf->m_commsInUse == NULL);
}

//=============================================================================
void ProtocolFilterTest::TestSetAndGetLastTxTime(void)
{
    pf->m_lastTxTime.SetValue((uint64_t)11);

    CPPUNIT_ASSERT_EQUAL((uint64_t)11,pf->m_lastTxTime.GetValue());
}

//=============================================================================
void ProtocolFilterTest::TestCloseCom(void)
{
    pf->CloseCom();

    // m_negativeResponseCodes should be cleared in CloseCom().  So, getting
    // the first element should throw an XmlException.
    CPPUNIT_ASSERT_THROW(pf->m_negativeResponseCodes.getNode(0),XmlException);
}
