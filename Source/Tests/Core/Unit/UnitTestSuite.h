//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/UnitTestSuite.h 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//   This is the Unit TestSuite that will combine all of the
// ApplicatonLayer and ServicesLayer system tests into a TestSuite.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/UnitTestSuite.h $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     1/06/05 3:56p Cward
// 
// 1     11/11/03 3:01p Khykin
// Initial checkin
//
//*************************************************************************
#ifndef UNITTESTSUITE_H
#define UNITTESTSUITE_H

#include <cppunit/TestSuite.h>

class UnitTestSuite : public CppUnit::TestSuite
{
	public:
        UnitTestSuite();
};

#endif

