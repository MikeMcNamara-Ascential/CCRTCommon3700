//******************************************************************************
// Description:
//       Runs all DCX specific tests
//
//==============================================================================
// Copyright (c) 2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/Tests/DCX/Unit/DCXUnitTestSuite.h $
// 
// 2     3/22/07 2:44p Dkrause
// Initial DCX Version
// 
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef DCXUNITTESTSUITE_H
#define DCXUNITTESTSUITE_H

#include <cppunit/TestSuite.h>

class DCXUnitTestSuite : public CppUnit::TestSuite
{
	public:
        DCXUnitTestSuite();
};

#endif

