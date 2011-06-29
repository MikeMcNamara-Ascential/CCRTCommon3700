#include "TestResultServerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "TestResultServerTest.h"

TestResultServerTestSuite::TestResultServerTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(TestResultServerTest::suite());
}

