//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/UnitTestSuite.cpp 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//   This is the Unit TestSuite that will excercise all of the
// Unit tests.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/UnitTestSuite.cpp $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
//
// 1     1/06/05 3:57p Cward
//
// 1     11/11/03 3:01p Khykin
// Initial checkin
//
//*************************************************************************
#include "UnitTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "ApplicationLayer/ApplicationLayerTestSuite.h"
#include "ServicesLayer/ServicesLayerTestSuite.h"

UnitTestSuite::UnitTestSuite()
{
    addTest(new ServicesLayerTestSuite());
    addTest(new ApplicationLayerTestSuite());
}


