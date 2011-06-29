//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/GeneralUtilities/GeneralUtilitiesTest.cpp 1     10/31/07 10:23a Rwiersem $
//
// FILE DESCRIPTION:
//  The GeneralUtilities test class.
//  
//===========================================================================
// Copyright (c) 2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/GeneralUtilities/GeneralUtilitiesTest.cpp $
// 
// 1     10/31/07 10:23a Rwiersem
// Changes for the 07103101 core release:
// 
// - Initial version.
//
//*************************************************************************
#include <string>
#include "GeneralUtilitiesTest.h"
#include "GeneralUtilities.h"

//-----------------------------------------------------------------------------
GeneralUtilitiesTest::GeneralUtilitiesTest(void)
{
}

//-----------------------------------------------------------------------------
GeneralUtilitiesTest::~GeneralUtilitiesTest(void)
{
}

//-----------------------------------------------------------------------------
void GeneralUtilitiesTest::setUp()
{
}

//-----------------------------------------------------------------------------
void GeneralUtilitiesTest::tearDown()
{
    // move to the next line for the next test name
    cout << endl;
}

//-----------------------------------------------------------------------------
void GeneralUtilitiesTest::ApplyForceMultiplierTest(void)
{
    cout << " ApplyForceMultiplierTest ";

    float result=0.0;

    // call the method under test
    result = ApplyForceMultiplier(100.0,2.0);

    // make sure the multiplier was applied correctly
    CPPUNIT_ASSERT_DOUBLES_EQUAL(200.0,result,0.01);
}

//-----------------------------------------------------------------------------
void GeneralUtilitiesTest::ApplyForceMultiplierTest_ZeroMultiplier(void)
{
    cout << " ApplyForceMultiplierTest_ZeroMultiplier ";

    float result=0.0;

    // call the method under test
    result = ApplyForceMultiplier(100.0,0.0);

    // make sure the force value was returned without applying the mulitplier
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0,result,0.01);
}



