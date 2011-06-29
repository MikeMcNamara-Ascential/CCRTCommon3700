#include "MapleKeypadManagerTest.h"
#include "MapleKeypadManager.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
MapleKeypadManagerTest::MapleKeypadManagerTest()
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
    m_configPath += "/Core/Unit/ServicesLayer/Servers/InputServer/"
        "MapleKeypadManager/Configuration/";
}

//-----------------------------------------------------------------------------
MapleKeypadManagerTest::~MapleKeypadManagerTest()
{
}

//-----------------------------------------------------------------------------
void MapleKeypadManagerTest::setUp()
{
    mkm = new MapleKeypadManager();
}

//-----------------------------------------------------------------------------
void MapleKeypadManagerTest::tearDown()
{
    delete mkm;
}

//-----------------------------------------------------------------------------
void MapleKeypadManagerTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL(string(INPUT_SERVER_NORMAL_STATE),mkm->m_nextState);
    CPPUNIT_ASSERT_EQUAL(false,mkm->m_stateChanged);
    CPPUNIT_ASSERT_EQUAL(true,mkm->m_openLogicalPort);
}

//-----------------------------------------------------------------------------
void MapleKeypadManagerTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "MapleKeypadManagerConfig.xml";

    // don't try to open the logical port
    mkm->m_openLogicalPort = false;

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *configNode =
        document->getChild("Setup/InputDevices/LeftMapleKeypad");
    mkm->LoadAdditionalConfigurationItems(configNode);

    CPPUNIT_ASSERT_EQUAL(129,mkm->GetBufferSize());
    CPPUNIT_ASSERT_EQUAL(100,mkm->GetDataDelay());
    CPPUNIT_ASSERT_EQUAL(4,mkm->GetMaximumRetries());
}

//-----------------------------------------------------------------------------
void 
MapleKeypadManagerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = 
        m_configPath + "MapleKeypadManagerConfigMissingTags.xml";

    // don't try to open the logical port
    mkm->m_openLogicalPort = false;

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *configNode =
        document->getChild("Setup/InputDevices/LeftMapleKeypad");
    mkm->LoadAdditionalConfigurationItems(configNode);

    CPPUNIT_ASSERT_EQUAL(128,mkm->GetBufferSize());
    CPPUNIT_ASSERT_EQUAL(10,mkm->GetDataDelay());
    CPPUNIT_ASSERT_EQUAL(3,mkm->GetMaximumRetries());
}
