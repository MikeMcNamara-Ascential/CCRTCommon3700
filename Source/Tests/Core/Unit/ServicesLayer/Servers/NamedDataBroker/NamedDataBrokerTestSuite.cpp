#include "NamedDataBrokerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "NamedDataBrokerTest.h"

NamedDataBrokerTestSuite::NamedDataBrokerTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(NamedDataBrokerTest::suite());
}

