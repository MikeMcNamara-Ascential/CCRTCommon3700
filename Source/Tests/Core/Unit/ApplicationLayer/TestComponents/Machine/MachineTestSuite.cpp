#include "MachineTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "Common/CommonMachineTestSuite.h"

MachineTestSuite::MachineTestSuite()
{
    addTest(new CommonMachineTestSuite());
}


