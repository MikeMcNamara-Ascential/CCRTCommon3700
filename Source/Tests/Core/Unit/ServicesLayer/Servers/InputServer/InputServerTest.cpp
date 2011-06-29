#include "InputServerTest.h"
#include "InputServer.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
InputServerTest::InputServerTest()
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
        "/Core/Unit/ServicesLayer/Servers/InputServer/Configuration/";
}

//-----------------------------------------------------------------------------
InputServerTest::~InputServerTest()
{
}

//-----------------------------------------------------------------------------
void InputServerTest::setUp()
{
    // register the name "core/NamedDataBroker" so the input server can
    // create a NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    is = new InputServer();
}

//-----------------------------------------------------------------------------
void InputServerTest::tearDown()
{
    delete is;

    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);
}

//-----------------------------------------------------------------------------
void InputServerTest::TestConstructor()
{
    CPPUNIT_ASSERT(is->m_ndb == NULL);
    CPPUNIT_ASSERT_EQUAL(true,is->m_launchInputDevices);
}

//-----------------------------------------------------------------------------
void InputServerTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "InputServerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    is->LoadAdditionalConfigurationItems(document);

    // pick a value at random to make sure the input devices were read from
    // the configuration file
    const XmlNode *rightMaple = 
        is->m_inputDeviceManagers.getNode("RightMapleKeypad");

    CPPUNIT_ASSERT_EQUAL(
        string("2"),
        rightMaple->getChild("Setup/DataTags/VINEntryRow")->getValue());

    CPPUNIT_ASSERT_EQUAL(string("core/WorkCellController"),
                         is->GetPlantHostInboundName());
    CPPUNIT_ASSERT_EQUAL(true,is->IsPlantHostInboundNameSpecified());
    CPPUNIT_ASSERT_EQUAL((UINT16)16,is->GetVinLength());

    // find the "LeftMapleKeypad" node in the map
    map<string, InputDevice_t*>::iterator itr;
    itr = is->m_inputDeviceMap.find("LeftMapleKeypad");
    InputDevice_t *inputDevice = itr->second;

    CPPUNIT_ASSERT_EQUAL(string("LeftMapleKeypad"),inputDevice->deviceName);

    string usrRoot = getenv("USR_ROOT");
    CPPUNIT_ASSERT_EQUAL(usrRoot + "/ServicesLayer/Bin/MapleKeypadManager.qxe",
                         inputDevice->processName);

    CPPUNIT_ASSERT_EQUAL(string("core/LeftMapleKeypad"),inputDevice->regName);
    CPPUNIT_ASSERT_EQUAL(-1,inputDevice->devicePid);
}

//-----------------------------------------------------------------------------
void InputServerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "InputServerConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    is->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(string(BEP_NONE),
                         is->GetPlantHostInboundName());
    CPPUNIT_ASSERT_EQUAL(false,is->IsPlantHostInboundNameSpecified());
    CPPUNIT_ASSERT_EQUAL((UINT16)17,is->GetVinLength());

    // "LeftMapleKeypad" node should not be in the map
    map<string, InputDevice_t*>::iterator itr;
    itr = is->m_inputDeviceMap.find("LeftMapleKeypad");

    // make sure the node wasn't found 
    CPPUNIT_ASSERT(is->m_inputDeviceMap.find("LeftMapleKeypad") == 
                   is->m_inputDeviceMap.end());
}

//-----------------------------------------------------------------------------
void InputServerTest::TestHandlePulse(void)
{
    string configFile = m_configPath + "InputServerConfigReload.xml";

    // don't try to launch the input devices
    is->m_launchInputDevices = false;

    // set a new configuration file so that we can make sure it was reloaded
    is->SetConfigFile(configFile);

    // send the reload configuration pulse
    is->HandlePulse(SYSTEM_PULSE,RELOAD_PULSE);

    try
    {
        // pick a value at random to make sure the input devices were read from
        // the configuration file
        const XmlNode *rightMaple = 
            is->m_inputDeviceManagers.getNode("RightMapleKeypad");
    
        CPPUNIT_ASSERT_EQUAL(
            string("3"),
            rightMaple->getChild("Setup/DataTags/VINEntryRow")->getValue());
    
        CPPUNIT_ASSERT_EQUAL(string("core/WorkCellController2"),
                             is->GetPlantHostInboundName());
        CPPUNIT_ASSERT_EQUAL(true,is->IsPlantHostInboundNameSpecified());
        CPPUNIT_ASSERT_EQUAL((UINT16)18,is->GetVinLength());    
    }
    catch(...)
    {
        CPPUNIT_FAIL("RightMapleKeypad not found in input device list");
    }
}
