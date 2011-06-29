#include "ServersTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "DataServer/DataServerTestSuite.h"
#include "InputServer/InputServerTestSuite.h"
#include "LogDaemon/LogDaemonTestSuite.h"
#include "MachineDataBroker/MachineDataBrokerTestSuite.h"
#include "MotorController/MotorControllerTestSuite.h"
#include "NamedDataBroker/NamedDataBrokerTestSuite.h"
#include "PlantHostInbound/PlantHostInboundTestSuite.h"
#include "PlantHostOutbound/PlantHostOutboundTestSuite.h"
#include "PromptServer/PromptServerTestSuite.h"
#include "SystemMonitor/SystemMonitorTestSuite.h"
#include "SystemScreenSaver/SystemScreenSaverTestSuite.h"
#include "TestResultServer/TestResultServerTestSuite.h"
#include "VehicleDataBroker/VehicleDataBrokerTestSuite.h"
#include "VehicleDataServer/VehicleDataServerTestSuite.h"

ServersTestSuite::ServersTestSuite()
{
    addTest(new DataServerTestSuite());
    addTest(new InputServerTestSuite());
    addTest(new LogDaemonTestSuite());
    addTest(new MachineDataBrokerTestSuite());    
    addTest(new MotorControllerTestSuite());
    addTest(new NamedDataBrokerTestSuite());
    addTest(new PlantHostInboundTestSuite());
    addTest(new PlantHostOutboundTestSuite());
    addTest(new PromptServerTestSuite());
    addTest(new SystemMonitorTestSuite());
    addTest(new SystemScreenSaverTestSuite());
    addTest(new TestResultServerTestSuite());
    addTest(new VehicleDataBrokerTestSuite());
    addTest(new VehicleDataServerTestSuite());
}

