#include "MotorControllerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "Common/CommonMotorControllerTestSuite.h"
#include "Electric/ElectricMotorControllerTestSuite.h"
#include "Hydraulic/HydraulicMotorControllerTestSuite.h"

MotorControllerTestSuite::MotorControllerTestSuite()
{
	addTest(new CommonMotorControllerTestSuite());
    addTest(new ElectricMotorControllerTestSuite());
    addTest(new HydraulicMotorControllerTestSuite());
}

