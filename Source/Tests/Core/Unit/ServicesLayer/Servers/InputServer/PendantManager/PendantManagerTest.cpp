#include "PendantManagerTest.h"
#include "PendantManager.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
PendantManagerTest::PendantManagerTest()
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
        "PendantManager/Configuration/";
}

//-----------------------------------------------------------------------------
PendantManagerTest::~PendantManagerTest()
{
}

//-----------------------------------------------------------------------------
void PendantManagerTest::setUp()
{
    pm = new PendantManager();
}

//-----------------------------------------------------------------------------
void PendantManagerTest::tearDown()
{
    delete pm;
}

//-----------------------------------------------------------------------------
void PendantManagerTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL('@',pm->m_currentCharacter);
    CPPUNIT_ASSERT_EQUAL(string(INPUT_SERVER_NORMAL_STATE),pm->m_currentState);
    CPPUNIT_ASSERT_EQUAL(0,pm->m_currentIndex);
    CPPUNIT_ASSERT_EQUAL(true,pm->m_abortButtonPressed);
    CPPUNIT_ASSERT_EQUAL(true,pm->m_publishKeyPress);
    CPPUNIT_ASSERT_EQUAL(string(INPUT_SERVER_NORMAL_STATE),pm->m_nextState);
    CPPUNIT_ASSERT_EQUAL(false,pm->m_stateChanged);
}

//-----------------------------------------------------------------------------
void PendantManagerTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "PendantManagerConfig.xml";

    // don't try to open the logical port
    pm->m_openLogicalPort = false;

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *configNode =
        document->getChild("Setup/InputDevices/Pendant");

    pm->LoadAdditionalConfigurationItems(configNode);

    // pick a value at random to make sure the configuration file was read in
    // correctly
    const XmlNode *normalNode = pm->m_pendantModes.getNode("Normal");
    CPPUNIT_ASSERT_EQUAL(string("FunctionKey2"),
                         normalNode->getChild("ValidKeyPad/KF2")->getValue());
}

