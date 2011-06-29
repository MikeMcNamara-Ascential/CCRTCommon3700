#include "SerialServerTest.h"
#include "SerialServer.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
SerialServerTest::SerialServerTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory"
            << endl;
        exit(1);
    }

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath +=
        "/Core/Unit/ServicesLayer/Managers/SerialServer/Configuration/";
}

//-----------------------------------------------------------------------------
SerialServerTest::~SerialServerTest()
{
}

//-----------------------------------------------------------------------------
void SerialServerTest::setUp()
{
    ss = new SerialServer();
}

//-----------------------------------------------------------------------------
void SerialServerTest::tearDown()
{
    delete ss;
}

//-----------------------------------------------------------------------------
void SerialServerTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL(true,ss->m_waitForRegister);
    CPPUNIT_ASSERT_EQUAL(string(SER_SRVR_NAME),ss->GetProcessName());
    CPPUNIT_ASSERT_EQUAL(true,ss->m_relaunchOnConfigReload);
}

//-----------------------------------------------------------------------------
void SerialServerTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SerialServerConfigPC.xml";

    try
    {
        const XmlNode *document = parser.ReturnXMLDocument(configFile);
        ss->LoadAdditionalConfigurationItems(document);
    }
    catch(XmlException ex)
    {
        CPPUNIT_FAIL("Couldn't parse XML document");
    }

    // find the PlcDf1 logical port in the map
    LogPortDrvMapItr_t itr;
    itr = ss->m_portDrvMap.find("PlcDf1");
    LogPortDrv_t *portDrv = itr->second;

    if(portDrv != NULL)    // make sure the data is valid
    {
        CPPUNIT_ASSERT_EQUAL(string("/dev/LogicalPorts/PlcDf1"),portDrv->devPath);

        string testDirPath = getenv("TEST_DIR_PATH");
        CPPUNIT_ASSERT_EQUAL(testDirPath + "/CMakeLists.txt",portDrv->drvName);

        CPPUNIT_ASSERT_EQUAL(-1,portDrv->drvPid);
        CPPUNIT_ASSERT_EQUAL(string("PlcDf1"),portDrv->portName);
        CPPUNIT_ASSERT_EQUAL(string("logport/PlcDf1"),portDrv->regName);
        CPPUNIT_ASSERT_EQUAL(true,portDrv->relaunch);
        CPPUNIT_ASSERT_EQUAL(true,portDrv->shouldReset);
    }
    else
    {
        CPPUNIT_FAIL("Could not find PlcDf1 in the logical port map");
    }
}

//-----------------------------------------------------------------------------
void SerialServerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SerialServerConfigPC.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    ss->LoadAdditionalConfigurationItems(document);

    // find the LfMotor logical port in the map
    LogPortDrvMapItr_t itr;
    itr = ss->m_portDrvMap.find("LfMotor");
    LogPortDrv_t *portDrv = itr->second;

    if(portDrv != NULL)    // make sure the data is valid
    {
        CPPUNIT_ASSERT_EQUAL(false,portDrv->relaunch);
        CPPUNIT_ASSERT_EQUAL(false,portDrv->shouldReset);
    }
    else
    {
        CPPUNIT_FAIL("Could not find LfMotor in the logical port map");
    }
}

//-----------------------------------------------------------------------------
void SerialServerTest::TestHandlePulse(void)
{
    string configFile = m_configPath + "SerialServerConfigPCReload.xml";

    // don't try to launch the logical ports
    ss->m_relaunchOnConfigReload = false;

    // set a new configuration file so that we can make sure it was reloaded
    ss->SetConfigFile(configFile);

    // send the reload configuration pulse
    ss->HandlePulse(SYSTEM_PULSE,RELOAD_PULSE);

    // find the PlcDf1 logical port in the map
    LogPortDrvMapItr_t itr;
    itr = ss->m_portDrvMap.find("PlcDf1");
    LogPortDrv_t *portDrv = itr->second;

    if(portDrv != NULL)    // make sure the data is valid
    {
        CPPUNIT_ASSERT_EQUAL(string("/dev/LogicalPorts/PlcDf1"),
                             portDrv->devPath);

        string testDirPath = getenv("TEST_DIR_PATH");
        CPPUNIT_ASSERT_EQUAL(testDirPath + "/CMakeLists.txt",portDrv->drvName);

        CPPUNIT_ASSERT_EQUAL(-1,portDrv->drvPid);
        CPPUNIT_ASSERT_EQUAL(string("PlcDf1"),portDrv->portName);
        CPPUNIT_ASSERT_EQUAL(string("logport/PlcDf1"),portDrv->regName);
        CPPUNIT_ASSERT_EQUAL(false,portDrv->relaunch);
        CPPUNIT_ASSERT_EQUAL(false,portDrv->shouldReset);
    }
    else
    {
        CPPUNIT_FAIL("Could not find PlcDf1 in the logical port map");
    }
}
