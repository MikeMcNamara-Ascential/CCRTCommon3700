//*************************************************************************
// FILE DESCRIPTION:
//  The Test Class For BepServer.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/BepServerTest.cpp $
// 
// 1     10/26/06 2:08p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//*************************************************************************
#include <string>
#include "BepServerTest.h"
#include "BepServer.h"

BepServerTest::BepServerTest(void)
{
}

BepServerTest::~BepServerTest(void)
{
}

void BepServerTest::setUp()
{
}

void BepServerTest::tearDown()
{
}

void BepServerTest::TestRegister_NoSubscribeTags(void)
{
    BepServer bs;
    string retval;

    bs.SetStatus(BEP_TERMINATE_STATUS);

    retval = bs.Register();
   
    CPPUNIT_ASSERT_EQUAL(true,bs.IsRegistered());
    CPPUNIT_ASSERT_EQUAL(string(BEP_SUCCESS_RESPONSE),retval);
}

