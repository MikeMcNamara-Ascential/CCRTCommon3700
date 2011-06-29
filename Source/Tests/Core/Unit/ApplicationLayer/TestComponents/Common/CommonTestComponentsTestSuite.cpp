//******************************************************************************
// Description:
//   This is the test suite for the common test component tests.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Common/CommonTestComponentsTestSuite.cpp $
// 
// 2     1/18/07 4:07p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added GenericTcTemplateTest.
// 
// 1     10/26/06 1:48p Rwiersem
// Changes for the 06102701 core release:
// 
// Intial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "CommonTestComponentsTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "GenericTcTest.h"
#include "GenericTcTemplateTest.h"

CommonTestComponentsTestSuite::CommonTestComponentsTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(GenericTcTest::suite());
    addTest(GenericTcTemplateTest::suite());
}


