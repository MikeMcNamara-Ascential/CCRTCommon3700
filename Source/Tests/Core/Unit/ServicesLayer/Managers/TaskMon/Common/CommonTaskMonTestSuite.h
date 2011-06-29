//*************************************************************************
// Description:
//   Test suite for the tests in the TaskMon/Common directory.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/TaskMon/Common/CommonTaskMonTestSuite.h $
// 
// 1     10/26/06 2:53p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================
#ifndef CommonTaskMonTestSuite_h
#define CommonTaskMonTestSuite_h

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>
#include <string>

class CommonTaskMonTestSuite : public CppUnit::TestSuite
{
    public:
        CommonTaskMonTestSuite();
};

#endif

