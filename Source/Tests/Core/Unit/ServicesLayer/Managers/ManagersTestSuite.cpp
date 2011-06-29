#include "ManagersTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "CommProxy/CommProxyTestSuite.h"
#include "ICM/ICMTestSuite.h"
#include "PlcMgr/PlcMgrTestSuite.h"
#include "SerialServer/SerialServerTestSuite.h"
#include "TaskMon/TaskMonTestSuite.h"

ManagersTestSuite::ManagersTestSuite()
{
    addTest(new CommProxyTestSuite());    
    addTest(new ICMTestSuite());    
    addTest(new PlcMgrTestSuite());
    addTest(new SerialServerTestSuite());
    addTest(new TaskMonTestSuite());
}

