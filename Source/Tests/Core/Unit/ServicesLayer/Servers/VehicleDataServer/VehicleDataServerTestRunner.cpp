//******************************************************************************
// Description:
//   This is the TestRunner for the Vehicle Data Server tests.
//
//==============================================================================
// Copyright (c) 2005 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/VehicleDataServer/VehicleDataServerTestRunner.cpp $
// 
// 2     8/02/06 8:35p Cward
// Updated VehicleDataServer Tests to test for compatibility with older
// builds
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <iostream>
#include <string>

using namespace std;

#include "ProgressListener.h"

// Our directory's collector test fixture
#include "VehicleDataServerTestSuite.h"

int main( int argc, char* argv[])
{
    // Create our runner
    CppUnit::TextUi::TestRunner runner;

    // Make the test, the runner will clean it up for us
    runner.addTest( new VehicleDataServerTestSuite() );

    // Add the test progress listener
    ProgressListener progress;
    runner.eventManager().addListener(&progress);

    // Run the tests
    bool wasSuccessful = runner.run();

    // Return error code 1 if the one of test failed.
    return(wasSuccessful ? 0 : 1);
}

