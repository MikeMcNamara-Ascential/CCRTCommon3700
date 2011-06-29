#include "BrakeTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "Common/CommonBrakeTestSuite.h"

BrakeTestSuite::BrakeTestSuite()
{
    addTest(new CommonBrakeTestSuite());
}


