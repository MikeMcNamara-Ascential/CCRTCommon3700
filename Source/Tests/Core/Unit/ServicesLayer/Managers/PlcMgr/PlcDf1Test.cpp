#include "PlcDf1Test.h"
#include "PlcDf1.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
PlcDf1Test::PlcDf1Test()
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
PlcDf1Test::~PlcDf1Test()
{
}

//-----------------------------------------------------------------------------
void PlcDf1Test::setUp()
{
    // register the name "core/NamedDataBroker" so plant host outbound can 
    // create an INamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    plc = new PlcDf1();
}

//-----------------------------------------------------------------------------
void PlcDf1Test::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    delete plc;
}

//-----------------------------------------------------------------------------
void PlcDf1Test::TestConstructor()
{
    CPPUNIT_ASSERT(plc->m_fromPlc == NULL);
    CPPUNIT_ASSERT(plc->m_toPlc == NULL);
    CPPUNIT_ASSERT_EQUAL(string(""),plc->m_df1PortName);
    CPPUNIT_ASSERT_EQUAL((uint32_t)20,plc->m_commScanTime);
    CPPUNIT_ASSERT_EQUAL((uint32_t)16,plc->m_channelBits);
}

//-----------------------------------------------------------------------------
void PlcDf1Test::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "PlcDf1Config.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    plc->LoadAdditionalConfigurationItems(
        document->getChild("Setup/PlcBoards/MainDf1Plc"));

    CPPUNIT_ASSERT_EQUAL(string("$N171"),plc->m_df1ThreadArg.readAddress);
    CPPUNIT_ASSERT_EQUAL(string("$N161"),plc->m_df1ThreadArg.writeAddress);
    CPPUNIT_ASSERT_EQUAL(string("PlcDf11"),plc->m_df1PortName);
    CPPUNIT_ASSERT_EQUAL((uint32_t)200,plc->m_commScanTime);
    CPPUNIT_ASSERT_EQUAL((uint8_t)0x02,plc->m_df1ThreadArg.df1SourceId);
    CPPUNIT_ASSERT_EQUAL((uint8_t)0x01,plc->m_df1ThreadArg.df1DestId);
}

//-----------------------------------------------------------------------------
void PlcDf1Test::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "PlcDf1ConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    plc->LoadAdditionalConfigurationItems(
        document->getChild("Setup/PlcBoards/MainDf1Plc"));

    CPPUNIT_ASSERT_EQUAL(string("PlcDf1"),plc->m_df1PortName);
    CPPUNIT_ASSERT_EQUAL((uint32_t)20,plc->m_commScanTime);
    CPPUNIT_ASSERT_EQUAL((uint8_t)0xFF,plc->m_df1ThreadArg.df1SourceId);
    CPPUNIT_ASSERT_EQUAL((uint8_t)0xFF,plc->m_df1ThreadArg.df1DestId);
}
