#include "ICMTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "IpIcm/IpIcmMgrTestSuite.h"

ICMTestSuite::ICMTestSuite()
{
    addTest(new IpIcmMgrTestSuite());
}

