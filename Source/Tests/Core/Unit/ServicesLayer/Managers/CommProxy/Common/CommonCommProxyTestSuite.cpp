#include "CommonCommProxyTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "CommonCommProxyTest.h"

CommonCommProxyTestSuite::CommonCommProxyTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(CommonCommProxyTest::suite());
}

