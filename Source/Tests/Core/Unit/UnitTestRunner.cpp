//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/UnitTestRunner.cpp 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//   This is the TestRunner for all of the the Unit tests.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/UnitTestRunner.cpp $
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
#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <iostream>
#include <string>
using namespace std;

// Our directory's collector test fixture
#include "ProgressListener.h"
#include "UnitTestSuite.h"



int main( int argc, char* argv[])
{
	// Create our runner
	CppUnit::TextUi::TestRunner runner;

	// Make the test
	runner.addTest( new UnitTestSuite() );

	// Add the test progress listener
	ProgressListener progress;
	runner.eventManager().addListener(&progress);

	// Run the tests
	bool wasSuccessful = runner.run();

	// Return error code 1 if the one of test failed.
	return wasSuccessful ? 0 : 1;
}

