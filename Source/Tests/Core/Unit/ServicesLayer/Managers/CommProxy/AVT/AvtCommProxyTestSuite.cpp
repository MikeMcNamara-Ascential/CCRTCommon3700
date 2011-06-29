#include "AvtCommProxyTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "CAN/AvtCanProxyTestSuite.h"

AvtCommProxyTestSuite::AvtCommProxyTestSuite()
{
    addTest(new AvtCanProxyTestSuite());
}

