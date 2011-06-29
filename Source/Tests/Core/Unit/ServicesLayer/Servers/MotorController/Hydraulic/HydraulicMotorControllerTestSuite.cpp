#include "HydraulicMotorControllerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "HydraulicMotorControllerTest.h"

HydraulicMotorControllerTestSuite::HydraulicMotorControllerTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(HydraulicMotorControllerTest::suite());
}

