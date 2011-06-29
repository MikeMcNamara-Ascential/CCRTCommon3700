//******************************************************************************
// Description:
//    DAQ interface test runner.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/DAQ/DAQInterfaceTestRunner.cpp $
// 
// 1     5/01/07 1:44p Rwiersem
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
#include <iostream.h>
#include <string>

using namespace std;

#include "ProgressListener.h"
#include "DAQInterfaceTestSuite.h"

int main(int argc, char *argv[])
{   // Create the test runner and add it
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(new DAQInterfaceTestSuite());
    // Add the progress listener
    ProgressListener progress;
    runner.eventManager().addListener(&progress);
    // Run the tests
    bool wasSuccessful = runner.run();
    // Return error code 1 if any of the tests fails
    return wasSuccessful ? 0 : 1;
}
