//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SoftwareId/SoftwareIdTestSuite.cpp 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//   This is the SoftwareId test that will excersize the Software Id class
//
//===========================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SoftwareId/SoftwareIdTestSuite.cpp $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     1/06/05 3:57p Cward
//
//*************************************************************************
#include "SoftwareIdTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "SoftwareIdTest.h"

SoftwareIdTestSuite::SoftwareIdTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(SoftwareIdTest::suite());
//	addTest(X2Test::suite());
}


