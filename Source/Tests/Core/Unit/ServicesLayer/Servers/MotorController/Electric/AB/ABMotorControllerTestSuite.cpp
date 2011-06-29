#include "ABMotorControllerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "AB700sMotorTest.h"
#include "ABMotorControllerTest.h"

ABMotorControllerTestSuite::ABMotorControllerTestSuite()
{
	// Add the inidividual test fixtures in this directory
    addTest(AB700sMotorTest::suite());
	addTest(ABMotorControllerTest::suite());
}

