//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/GeneralUtilities/GeneralUtilitiesTestSuite.cpp 1     10/31/07 10:24a Rwiersem $
//
// FILE DESCRIPTION:
//   This is the GeneralUtilities test suite.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/GeneralUtilities/GeneralUtilitiesTestSuite.cpp $
// 
// 1     10/31/07 10:24a Rwiersem
// Changes for the 07103101 core release:
// 
// - Initial version.
//
//*************************************************************************
#include "GeneralUtilitiesTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "GeneralUtilitiesTest.h"

GeneralUtilitiesTestSuite::GeneralUtilitiesTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(GeneralUtilitiesTest::suite());
}


