#include "ServicesLayerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "Library/LibraryTestSuite.h"
#include "Managers/ManagersTestSuite.h"
#include "Servers/ServersTestSuite.h"

ServicesLayerTestSuite::ServicesLayerTestSuite()
{
    addTest(new ManagersTestSuite());
	addTest(new ServersTestSuite());
	addTest(new LibraryTestSuite());
}

