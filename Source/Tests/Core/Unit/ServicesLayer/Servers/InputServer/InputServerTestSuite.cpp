#include "InputServerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "InputServerTest.h"
#include "CLV295ScannerMgr/CLV295ScannerMgrTest.h"
#include "MapleKeypadManager/MapleKeypadManagerTest.h"
#include "PendantManager/PendantManagerTest.h"
#include "SymbolScannerMgr/SymbolScannerMgrTest.h"

InputServerTestSuite::InputServerTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(InputServerTest::suite());
    addTest(CLV295ScannerMgrTest::suite());
    addTest(MapleKeypadManagerTest::suite());
    addTest(PendantManagerTest::suite());
    addTest(SymbolScannerMgrTest::suite());
}

