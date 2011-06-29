#include "PlcMgrTest.h"
#include "PlcMgr.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
PlcMgrTest::PlcMgrTest()
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
PlcMgrTest::~PlcMgrTest()
{
}

//-----------------------------------------------------------------------------
void PlcMgrTest::setUp()
{
    // register the name "core/NamedDataBroker" so plant host outbound can
    // create an INamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    plc = new PlcMgr();
}

//-----------------------------------------------------------------------------
void PlcMgrTest::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    delete plc;
}

//-----------------------------------------------------------------------------
void PlcMgrTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL((unsigned long)0,plc->m_fromPlcCount);
    CPPUNIT_ASSERT_EQUAL((unsigned long)0,plc->m_toPlcCount);
    CPPUNIT_ASSERT_EQUAL(-1,plc->m_plcChnlFd);
    CPPUNIT_ASSERT_EQUAL(10,plc->m_scannerPrio);
    CPPUNIT_ASSERT(plc->m_threadPool == NULL);
    CPPUNIT_ASSERT_EQUAL(false,plc->m_autoDetect);
    CPPUNIT_ASSERT_EQUAL(-1,plc->m_scanTimerId);
    CPPUNIT_ASSERT_EQUAL(1,plc->m_toPlcInode);
    CPPUNIT_ASSERT_EQUAL(true,plc->m_waitForRegister);
    CPPUNIT_ASSERT_EQUAL(true,plc->m_needFirstPublish);
    CPPUNIT_ASSERT_EQUAL((UINT32)0,plc->GetVerboseMask());
    CPPUNIT_ASSERT_EQUAL((unsigned long)256,plc->m_channelCnt);
    CPPUNIT_ASSERT_EQUAL((long)CHANNEL_TIME,plc->m_convertTime);
    CPPUNIT_ASSERT(plc->m_fromPlc == NULL);
    CPPUNIT_ASSERT(plc->m_toPlc == NULL);
    CPPUNIT_ASSERT_EQUAL(-1,plc->m_outDevNo);
    CPPUNIT_ASSERT_EQUAL(-1,plc->m_inDevNo);
    CPPUNIT_ASSERT_EQUAL(false,plc->m_demoMode);
    CPPUNIT_ASSERT_EQUAL(10,plc->m_scannerPrio);
    CPPUNIT_ASSERT_EQUAL(-1,plc->m_writeChCoid);
}

//-----------------------------------------------------------------------------
void PlcMgrTest::TestLoadAdditionalConfigurationItems(void)
{
    string configFile = m_configPath + "PlcMgrConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    plc->Initialize(document);

    CPPUNIT_ASSERT_EQUAL(true,plc->m_needFirstPublish);

    PlcDiagMapItr_t diagItr;
    // pick an item at random
    diagItr = plc->m_diagList.find(100);
    PlcDiagItem &diagItem = diagItr->second;

    CPPUNIT_ASSERT_EQUAL(string("LRMachLossChangeOver5LBS"),diagItem.GetFaultTag());
    CPPUNIT_ASSERT_EQUAL((DiagLevel_t)2,diagItem.GetLevel());
    CPPUNIT_ASSERT_EQUAL((UINT32)1,diagItem.GetMin());
    CPPUNIT_ASSERT_EQUAL((UINT32)1,diagItem.GetMax());

    CPPUNIT_ASSERT_EQUAL((size_t)1,plc->m_plcBoards.size());
    CPPUNIT_ASSERT_EQUAL((unsigned long)32,plc->m_toPlcCount);
    CPPUNIT_ASSERT_EQUAL((unsigned long)32,plc->m_fromPlcCount);
    CPPUNIT_ASSERT_EQUAL((uint32_t)16,plc->m_plcBoards[0]->GetBitsPerChannel());
}

//-----------------------------------------------------------------------------
void PlcMgrTest::TestHandlePulse_ReloadPulse(void)
{
    string configFile = m_configPath + "PlcMgrConfig.xml";
    string reloadConfigFile = m_configPath + "PlcMgrConfigReload.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    plc->Initialize(document);

    // set a new configuration file so that we can make sure it was reloaded
    plc->SetConfigFile(reloadConfigFile);

    // send the reload configuration pulse
    plc->HandlePulse(SYSTEM_PULSE,RELOAD_PULSE);

    PlcDiagMapItr_t diagItr;
    // pick an item at random
    diagItr = plc->m_diagList.find(100);
    PlcDiagItem &diagItem = diagItr->second;

    CPPUNIT_ASSERT_EQUAL((DiagLevel_t)9,diagItem.GetLevel());
    CPPUNIT_ASSERT_EQUAL((UINT32)2,diagItem.GetMin());
    CPPUNIT_ASSERT_EQUAL((UINT32)2,diagItem.GetMax());
}
