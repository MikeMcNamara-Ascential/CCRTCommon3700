#include "PromptServerTest.h"
#include "PromptServer.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

PromptServerTest::PromptServerTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" << endl;
        exit(1);
    }

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += "/Core/Unit/ServicesLayer/Servers/PromptServer/Configuration/";
}

PromptServerTest::~PromptServerTest()
{
}

void PromptServerTest::setUp()
{
    // register the name "core/NamedDataBroker" so the prompt server can
    // create a NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");
}

void PromptServerTest::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);
}

void PromptServerTest::TestConstructor()
{
    PromptServer promptServer;

    // check that the default values set in the intialize list are correct
    CPPUNIT_ASSERT_EQUAL(string(""),promptServer.m_lockProcess);
    CPPUNIT_ASSERT_EQUAL(MAX_PROMPT_BOXES,promptServer.m_numberOfPromptBoxes);
    CPPUNIT_ASSERT_EQUAL(MAX_PRIORITY,promptServer.m_maximumPriority);
    CPPUNIT_ASSERT_EQUAL(false,promptServer.m_serverLocked);
}

void PromptServerTest::TestLoadAdditionalConfigurationItems(void)
{
    PromptServer promptServer;
    XmlParser parser;
    string configFile = m_configPath + "PromptServerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    promptServer.LoadAdditionalConfigurationItems(document);

    // Make sure the prompt server configuration values are correctly
    // read in from the configuration file
    CPPUNIT_ASSERT_EQUAL(2,promptServer.m_numberOfPromptBoxes);
    CPPUNIT_ASSERT_EQUAL(2,promptServer.m_maximumPriority);
}

void PromptServerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    PromptServer promptServer;
    XmlParser parser;
    string configFile = m_configPath + "PromptServerConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    promptServer.LoadAdditionalConfigurationItems(document);

    // Make sure the configuration values are properly set to their default
    // when the tags are missing from the configuration file.
    CPPUNIT_ASSERT_EQUAL(MAX_PROMPT_BOXES,promptServer.m_numberOfPromptBoxes);
    CPPUNIT_ASSERT_EQUAL(MAX_PRIORITY,promptServer.m_maximumPriority);
}

void PromptServerTest::TestHandlePulse(void)
{
    PromptServer promptServer;
    string configFile = m_configPath + "PromptServerConfigReload.xml";

    // set a new configuration file so that we can make sure
    // it was reloaded
    promptServer.SetConfigFile(configFile);

    // send the reload configuration pulse
    promptServer.HandlePulse(SYSTEM_PULSE,RELOAD_PULSE);

    // make sure the new values were loaded
    CPPUNIT_ASSERT_EQUAL(1,promptServer.m_numberOfPromptBoxes);
    CPPUNIT_ASSERT_EQUAL(1,promptServer.m_maximumPriority);
}

