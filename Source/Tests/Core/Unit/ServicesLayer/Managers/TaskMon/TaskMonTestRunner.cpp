//*************************************************************************
// Description:
//   Test runner for the test suites in the TaskMon/Common directory.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/TaskMon/TaskMonTestRunner.cpp $
// 
// 1     10/26/06 2:52p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================
#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <iostream>
#include <string>
using namespace std;

// Our directory's collector test fixture
#include "TaskMonTestSuite.h"
#include "ProgressListener.h"


int main( int argc, char* argv[])
{
    // Create our runner
    CppUnit::TextUi::TestRunner runner;

    // Make the test
    runner.addTest( new TaskMonTestSuite() );

    // Add the test progress listener
    ProgressListener progress;
    runner.eventManager().addListener(&progress);

    // Run the tests
    bool wasSuccessful = runner.run();

    // Return error code 1 if the one of test failed.
    return wasSuccessful ? 0 : 1;
}
