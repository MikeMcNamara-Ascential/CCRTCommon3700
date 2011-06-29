#include "SymbolScannerMgrTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "SymbolScannerMgrTest.h"

SymbolScannerMgrTestSuite::SymbolScannerMgrTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(SymbolScannerMgrTest::suite());
}

