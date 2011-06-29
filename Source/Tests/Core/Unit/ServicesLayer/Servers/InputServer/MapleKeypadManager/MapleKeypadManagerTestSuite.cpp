#include "MapleKeypadManagerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "MapleKeypadManagerTest.h"

MapleKeypadManagerTestSuite::MapleKeypadManagerTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(MapleKeypadManagerTest::suite());
}

