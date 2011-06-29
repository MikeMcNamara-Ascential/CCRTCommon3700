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
//    $Log: /CCRT-cmake/Source/Tests/DCX/Unit/DCXUnitTestRunner.cpp $
// 
// 2     3/22/07 2:44p Dkrause
// Initial DCX Version
// 
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
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
#include "DCXUnitTestSuite.h"



int main( int argc, char* argv[])
{
	// Create our runner
	CppUnit::TextUi::TestRunner runner;

	// Make the test
	runner.addTest( new DCXUnitTestSuite() );

	// Add the test progress listener
	ProgressListener progress;
	runner.eventManager().addListener(&progress);

	// Run the tests
	bool wasSuccessful = runner.run();

	// Return error code 1 if the one of test failed.
	return wasSuccessful ? 0 : 1;
}

