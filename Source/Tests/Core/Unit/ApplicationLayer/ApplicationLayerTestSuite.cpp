#include "ApplicationLayerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "ModuleInterfaces/ModuleInterfacesTestSuite.h"
#include "ProtocolFilters/ProtocolFiltersTestSuite.h"
#include "TestComponents/TestCompTestSuite.h"
#include "VehicleTest/VehicleTestTestSuite.h"

ApplicationLayerTestSuite::ApplicationLayerTestSuite()
{
    addTest(new ModuleInterfacesTestSuite());
    addTest(new ProtocolFiltersTestSuite());
    addTest(new TestCompTestSuite());
    addTest(new VehicleTestTestSuite());
}


