#include "SerialServerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "SerialServerTest.h"

SerialServerTestSuite::SerialServerTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(SerialServerTest::suite());
}

