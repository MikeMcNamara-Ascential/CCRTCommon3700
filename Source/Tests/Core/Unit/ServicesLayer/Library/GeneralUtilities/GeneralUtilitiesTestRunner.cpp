//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/GeneralUtilities/GeneralUtilitiesTestRunner.cpp 1     10/31/07 10:24a Rwiersem $
//
// FILE DESCRIPTION:
//   This is the test runner for the GeneralUtilities tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/GeneralUtilities/GeneralUtilitiesTestRunner.cpp $
// 
// 1     10/31/07 10:24a Rwiersem
// Changes for the 07103101 core release:
// 
// - Initial version.
//
//*************************************************************************
#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <iostream>
#include <string>
using namespace std;

#include "ProgressListener.h"

// Our directory's collector test fixture
#include "GeneralUtilitiesTestSuite.h"

int main( int argc, char* argv[])
{
    // Create our runner
    CppUnit::TextUi::TestRunner runner;

    // Make the test, the runner will clean it up for us
    runner.addTest( new GeneralUtilitiesTestSuite() );

    // Add the test progress listener
    ProgressListener progress;
    runner.eventManager().addListener(&progress);

    // Run the tests
    bool wasSuccessful = runner.run();

    // Return error code 1 if the one of test failed.
    return(wasSuccessful ? 0 : 1);
}

