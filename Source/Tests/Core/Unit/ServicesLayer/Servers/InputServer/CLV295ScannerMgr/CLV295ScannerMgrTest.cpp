#include "CLV295ScannerMgrTest.h"
#include "CLV295ScannerMgr.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
CLV295ScannerMgrTest::CLV295ScannerMgrTest()
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
        "CLV295ScannerMgr/Configuration/";
}

//-----------------------------------------------------------------------------
CLV295ScannerMgrTest::~CLV295ScannerMgrTest()
{
}

//-----------------------------------------------------------------------------
void CLV295ScannerMgrTest::setUp()
{
    csm = new CLV295ScannerMgr();
}

//-----------------------------------------------------------------------------
void CLV295ScannerMgrTest::tearDown()
{
    delete csm;
}

//-----------------------------------------------------------------------------
void CLV295ScannerMgrTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL(17,csm->m_vinLength);
    CPPUNIT_ASSERT_EQUAL(true,csm->m_openLogicalPort);
}

//-----------------------------------------------------------------------------
void CLV295ScannerMgrTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "CLV295ScannerMgrConfig.xml";

    // don't try to open the logical port
    csm->m_openLogicalPort = false;

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *configNode = 
        document->getChild("Setup/InputDevices/CLV295Scanner");
    csm->LoadAdditionalConfigurationItems(configNode);

    CPPUNIT_ASSERT_EQUAL(4,csm->GetMaxAttempts());
    CPPUNIT_ASSERT_EQUAL(129,csm->GetMaxBufferSize());
    CPPUNIT_ASSERT_EQUAL(2000,csm->GetStackLightOnDuration());
    CPPUNIT_ASSERT_EQUAL(150,csm->GetStackLightTestDelay());
    CPPUNIT_ASSERT_EQUAL(101,csm->GetCommandDelay());
    CPPUNIT_ASSERT_EQUAL(1000,csm->GetDataDelay());
    CPPUNIT_ASSERT_EQUAL(31,csm->GetMaxDecodes());
    CPPUNIT_ASSERT_EQUAL(5,csm->GetReaderIDLength());
    CPPUNIT_ASSERT_EQUAL(5,csm->GetFISDecodeLength());
    CPPUNIT_ASSERT_EQUAL(101,csm->GetValidRecordAttempts());
    CPPUNIT_ASSERT_EQUAL(101,csm->GetValidRecordCheckDelay());
    CPPUNIT_ASSERT_EQUAL(18,csm->m_vinLength);
    CPPUNIT_ASSERT_EQUAL(string("2800000001"),
                         csm->m_clvCommands.getNode("NoRead")->getValue());
}

//-----------------------------------------------------------------------------
void 
CLV295ScannerMgrTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "CLV295ScannerMgrConfigMissingTags.xml";

    // don't try to open the logical port
    csm->m_openLogicalPort = false;

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *configNode =
        document->getChild("Setup/InputDevices/CLV295Scanner");
    csm->LoadAdditionalConfigurationItems(configNode);

    CPPUNIT_ASSERT_EQUAL(3,csm->GetMaxAttempts());
    CPPUNIT_ASSERT_EQUAL(128,csm->GetMaxBufferSize());
    CPPUNIT_ASSERT_EQUAL(500,csm->GetStackLightOnDuration());
    CPPUNIT_ASSERT_EQUAL(100,csm->GetStackLightTestDelay());
    CPPUNIT_ASSERT_EQUAL(100,csm->GetCommandDelay());
    CPPUNIT_ASSERT_EQUAL(10,csm->GetDataDelay());
    CPPUNIT_ASSERT_EQUAL(30,csm->GetMaxDecodes());
    CPPUNIT_ASSERT_EQUAL(4,csm->GetReaderIDLength());
    CPPUNIT_ASSERT_EQUAL(4,csm->GetFISDecodeLength());
    CPPUNIT_ASSERT_EQUAL(100,csm->GetValidRecordAttempts());
    CPPUNIT_ASSERT_EQUAL(100,csm->GetValidRecordCheckDelay());
    CPPUNIT_ASSERT_EQUAL(17,csm->m_vinLength);
}
