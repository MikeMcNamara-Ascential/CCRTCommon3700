#include "PendantManagerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "PendantManagerTest.h"

PendantManagerTestSuite::PendantManagerTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(PendantManagerTest::suite());
}

