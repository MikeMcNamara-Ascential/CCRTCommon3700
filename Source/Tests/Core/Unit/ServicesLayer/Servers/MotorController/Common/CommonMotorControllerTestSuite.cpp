#include "CommonMotorControllerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "CommonMotorControllerTest.h"

CommonMotorControllerTestSuite::CommonMotorControllerTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(CommonMotorControllerTest::suite());
}

