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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/TaskMon/TaskMonTestSuite.h $
// 
// 1     10/26/06 2:52p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================
#ifndef TaskMonTestSuite_h
#define TaskMonTestSuite_h

#include <cppunit/TestSuite.h>

class TaskMonTestSuite : public CppUnit::TestSuite
{
    public:
        TaskMonTestSuite();
};

#endif

