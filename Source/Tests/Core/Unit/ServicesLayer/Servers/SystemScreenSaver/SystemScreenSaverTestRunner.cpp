//******************************************************************************
// Description:
//    System screen saver test runner.
//
//==============================================================================
//
//     Copyright (c) 2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/SystemScreenSaver/SystemScreenSaverTestRunner.cpp $
// 
// 1     5/01/07 1:48p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
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
#include "SystemScreenSaverTestSuite.h"

int main( int argc, char* argv[])
{
    // Create our runner
    CppUnit::TextUi::TestRunner runner;

    // Make the test, the runner will clean it up for us
    runner.addTest( new SystemScreenSaverTestSuite() );

    // Add the test progress listener
    ProgressListener progress;
    runner.eventManager().addListener(&progress);

    // Run the tests
    bool wasSuccessful = runner.run();

    // Return error code 1 if the one of test failed.
    return wasSuccessful ? 0 : 1;
}
