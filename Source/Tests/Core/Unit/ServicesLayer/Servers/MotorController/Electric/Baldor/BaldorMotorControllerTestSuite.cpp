#include "BaldorMotorControllerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "BaldorMotorControllerTest.h"

BaldorMotorControllerTestSuite::BaldorMotorControllerTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(BaldorMotorControllerTest::suite());
}

