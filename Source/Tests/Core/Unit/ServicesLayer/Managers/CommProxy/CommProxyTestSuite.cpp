#include "CommProxyTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "AVT/AvtCommProxyTestSuite.h"
#include "Common/CommonCommProxyTestSuite.h"

CommProxyTestSuite::CommProxyTestSuite()
{
    addTest(new AvtCommProxyTestSuite());
    addTest(new CommonCommProxyTestSuite());
}

