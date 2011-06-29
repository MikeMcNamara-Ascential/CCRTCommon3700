#include "MachineDataBrokerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "MachineDataBrokerTest.h"

MachineDataBrokerTestSuite::MachineDataBrokerTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(MachineDataBrokerTest::suite());
}

