#include "TransmissionTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "Common/CommonTransmissionTestSuite.h"

TransmissionTestSuite::TransmissionTestSuite()
{
    addTest(new CommonTransmissionTestSuite());
}


