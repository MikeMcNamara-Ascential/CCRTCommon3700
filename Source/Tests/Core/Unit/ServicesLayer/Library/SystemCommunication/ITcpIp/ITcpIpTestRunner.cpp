//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SystemCommunication/ITcpIp/ITcpIpTestRunner.cpp 1     7/20/05 8:14p Cward $
//
// FILE DESCRIPTION:
//   This is the TestRunner for the ITcpIp tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SystemCommunication/ITcpIp/ITcpIpTestRunner.cpp $
// 
// 1     7/20/05 8:14p Cward
// Directory structure change
// 
// 1     1/06/05 3:57p Cward
// 
//*************************************************************************
#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <iostream>
#include <string>
using namespace std;

// Our directory's collector test fixture
#include "ProgressListener.h"
#include "ITcpIpTestSuite.h"

int main( int argc, char* argv[])
{
    // Create our runner
    CppUnit::TextUi::TestRunner runner;

    // Make the test, the runner will clean it up for us
    runner.addTest( new ITcpIpTestSuite() );

    // Add the test progress listener
    ProgressListener progress;
    runner.eventManager().addListener(&progress);

    // Run the tests
    bool wasSuccessful = runner.run();

    // Return error code 1 if the one of test failed.
    return(wasSuccessful ? 0 : 1);
}

