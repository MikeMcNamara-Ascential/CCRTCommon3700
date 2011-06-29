#include "TestCompTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "Brake/BrakeTestSuite.h"
#include "Common/CommonTestComponentsTestSuite.h"
#include "Emissions/EmissionsTestSuite.h"
#include "LossCompensation/LossCompensationTestSuite.h"
#include "Machine/MachineTestSuite.h"
#include "SpeedControl/SpeedControlTestSuite.h"
#include "Transmission/TransmissionTestSuite.h"

TestCompTestSuite::TestCompTestSuite()
{
    addTest(new BrakeTestSuite());
    addTest(new CommonTestComponentsTestSuite());
    addTest(new EmissionsTestSuite());
    addTest(new LossCompensationTestSuite());
    addTest(new MachineTestSuite());
    addTest(new SpeedControlTestSuite());
    addTest(new TransmissionTestSuite());
}


