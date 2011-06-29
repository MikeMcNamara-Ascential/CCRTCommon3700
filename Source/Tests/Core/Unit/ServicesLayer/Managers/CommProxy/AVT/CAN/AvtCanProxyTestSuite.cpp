#include "AvtCanProxyTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "AvtCanProxyTest.h"

AvtCanProxyTestSuite::AvtCanProxyTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(AvtCanProxyTest::suite());
}

