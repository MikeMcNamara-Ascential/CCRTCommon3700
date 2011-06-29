#include "PlantHostInboundTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "PlantHostInboundTest.h"

PlantHostInboundTestSuite::PlantHostInboundTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(PlantHostInboundTest::suite());
}

