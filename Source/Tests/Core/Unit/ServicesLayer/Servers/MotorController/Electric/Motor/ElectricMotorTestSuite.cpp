#include "ElectricMotorTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "ElectricMotorTest.h"

ElectricMotorTestSuite::ElectricMotorTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(ElectricMotorTest::suite());
}

