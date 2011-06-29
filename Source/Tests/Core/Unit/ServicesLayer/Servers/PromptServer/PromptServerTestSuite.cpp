#include "PromptServerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "PromptServerTest.h"

PromptServerTestSuite::PromptServerTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(PromptServerTest::suite());
}

