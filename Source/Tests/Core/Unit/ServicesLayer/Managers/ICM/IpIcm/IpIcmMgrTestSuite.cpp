#include "IpIcmMgrTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "IpIcmMgrTest.h"

IpIcmMgrTestSuite::IpIcmMgrTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(IpIcmMgrTest::suite());
}

