#include "ElectricMotorControllerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "AB/ABMotorControllerTestSuite.h"
#include "Baldor/BaldorMotorControllerTestSuite.h"
#include "Motor/ElectricMotorTestSuite.h"

ElectricMotorControllerTestSuite::ElectricMotorControllerTestSuite()
{
    addTest(new ABMotorControllerTestSuite());
    addTest(new BaldorMotorControllerTestSuite());
    addTest(new ElectricMotorTestSuite());
}

