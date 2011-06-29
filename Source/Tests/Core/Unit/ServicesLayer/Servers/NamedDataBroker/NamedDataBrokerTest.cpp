#include "NamedDataBrokerTest.h"
#include "NamedDataBroker.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
NamedDataBrokerTest::NamedDataBrokerTest()
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
        "/Core/Unit/ServicesLayer/Servers/NamedDataBroker/Configuration/";
}

//-----------------------------------------------------------------------------
NamedDataBrokerTest::~NamedDataBrokerTest()
{
}

//-----------------------------------------------------------------------------
void NamedDataBrokerTest::setUp()
{
    ndb = new NamedDataBroker();
}

//-----------------------------------------------------------------------------
void NamedDataBrokerTest::tearDown()
{
    delete ndb;
}

//-----------------------------------------------------------------------------
void NamedDataBrokerTest::TestLoadAdditionalConfigurationItems(void)
{  
    XmlParser parser;
    string configFile = m_configPath + "NDBConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    ndb->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL((size_t)3,ndb->m_servers.size());
    CPPUNIT_ASSERT_EQUAL(string("FaultServer"),
                         ndb->m_servers.getNode(0)->getName());
    CPPUNIT_ASSERT_EQUAL(string("core/FaultServer"),
                         ndb->m_servers.getNode(0)->getValue());
    CPPUNIT_ASSERT_EQUAL(string("ICM"),
                         ndb->m_servers.getNode(1)->getName());
    CPPUNIT_ASSERT_EQUAL(string("mgr/ICM"),
                         ndb->m_servers.getNode(1)->getValue());
    CPPUNIT_ASSERT_EQUAL(string("InputServer"),
                         ndb->m_servers.getNode(2)->getName());
    CPPUNIT_ASSERT_EQUAL(string("core/InputServer"),
                         ndb->m_servers.getNode(2)->getValue());
}
