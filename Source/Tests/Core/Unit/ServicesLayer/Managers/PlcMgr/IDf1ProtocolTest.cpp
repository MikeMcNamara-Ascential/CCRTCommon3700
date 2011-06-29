#include "IDf1ProtocolTest.h"
#include "IDf1Protocol.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
IDf1ProtocolTest::IDf1ProtocolTest()
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
        "/Core/Unit/ServicesLayer/Managers/PlcMgr/Configuration/";
}

//-----------------------------------------------------------------------------
IDf1ProtocolTest::~IDf1ProtocolTest()
{
}

//-----------------------------------------------------------------------------
void IDf1ProtocolTest::setUp()
{
    plc = new IDf1Protocol();
}

//-----------------------------------------------------------------------------
void IDf1ProtocolTest::tearDown()
{
    delete plc;
}

//-----------------------------------------------------------------------------
void IDf1ProtocolTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL(50,plc->m_df1BytesPerBlock);
    CPPUNIT_ASSERT_EQUAL((unsigned)0,plc->m_df1InitCount);
    CPPUNIT_ASSERT_EQUAL(-1,plc->m_df1ComDev);
    CPPUNIT_ASSERT_EQUAL((uint16_t)0,plc->m_df1TnsCounter);
    CPPUNIT_ASSERT_EQUAL((uint8_t)0,plc->m_df1SrcId);
    CPPUNIT_ASSERT(plc->m_terminate == false);
}

//-----------------------------------------------------------------------------
void IDf1ProtocolTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "IDf1ProtocolConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    plc->LoadAdditionalConfigurationItems(
        document->getChild("Setup/PlcBoards/MainDf1Plc"));

    CPPUNIT_ASSERT_EQUAL(40,plc->m_df1BytesPerBlock);
}

//-----------------------------------------------------------------------------
void IDf1ProtocolTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "IDf1ProtocolConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    plc->LoadAdditionalConfigurationItems(
        document->getChild("Setup/PlcBoards/MainDf1Plc"));

    CPPUNIT_ASSERT_EQUAL(50,plc->m_df1BytesPerBlock);
}
