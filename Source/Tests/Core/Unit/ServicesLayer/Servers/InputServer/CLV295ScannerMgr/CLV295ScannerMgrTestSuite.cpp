#include "CLV295ScannerMgrTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "CLV295ScannerMgrTest.h"

CLV295ScannerMgrTestSuite::CLV295ScannerMgrTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(CLV295ScannerMgrTest::suite());
}

