#include "VehicleDataBrokerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "VehicleDataBrokerTest.h"

VehicleDataBrokerTestSuite::VehicleDataBrokerTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(VehicleDataBrokerTest::suite());
}

