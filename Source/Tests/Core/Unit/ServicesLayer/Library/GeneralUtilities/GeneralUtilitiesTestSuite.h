//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/GeneralUtilities/GeneralUtilitiesTestSuite.h 1     10/31/07 10:24a Rwiersem $
//
// FILE DESCRIPTION:
//   This is the test suite for the GeneralUtilities tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/GeneralUtilities/GeneralUtilitiesTestSuite.h $
// 
// 1     10/31/07 10:24a Rwiersem
// Changes for the 07103101 core release:
// 
// - Initial version.
//
//*************************************************************************
#ifndef GeneralUtilitiesTestSuite_h
#define GeneralUtilitiesTestSuite_h

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>

class GeneralUtilitiesTestSuite : public CppUnit::TestSuite
{
    public:
        GeneralUtilitiesTestSuite();
};

#endif // GeneralUtilitiesTestSuite_h

