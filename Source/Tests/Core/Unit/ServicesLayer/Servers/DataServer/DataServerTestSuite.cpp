#include "DataServerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "DataServerTest.h"

DataServerTestSuite::DataServerTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(DataServerTest::suite());
}

