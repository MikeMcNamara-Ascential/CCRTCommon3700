#include "CommonCommProxyTest.h"
#include "RawCommProxy.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
CommonCommProxyTest::CommonCommProxyTest()
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
        "/Core/Unit/ServicesLayer/Managers/CommProxy/Common/Configuration/";
}

//-----------------------------------------------------------------------------
CommonCommProxyTest::~CommonCommProxyTest()
{
}

//-----------------------------------------------------------------------------
void CommonCommProxyTest::setUp()
{
    rcp = new RawCommProxy();
}

//-----------------------------------------------------------------------------
void CommonCommProxyTest::tearDown()
{
    delete rcp;
}

//-----------------------------------------------------------------------------
void CommonCommProxyTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL(string(COMM_PROXY_DEV_PATH),rcp->m_pathName);
    CPPUNIT_ASSERT_EQUAL(string(""),rcp->m_parentName);
    CPPUNIT_ASSERT_EQUAL((uint32_t)0,rcp->m_portId);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1024,rcp->m_fifoSize);
    CPPUNIT_ASSERT(rcp->m_nameId == NULL);
    CPPUNIT_ASSERT_EQUAL(-1,rcp->m_readThreadId);
    CPPUNIT_ASSERT_EQUAL(-1,rcp->m_dataReadyChId);
    CPPUNIT_ASSERT_EQUAL(-1,rcp->m_comPortFd);
    CPPUNIT_ASSERT_EQUAL(-1,rcp->m_readThreadCoid);
    CPPUNIT_ASSERT_EQUAL(true,rcp->m_needToRegister);
    CPPUNIT_ASSERT_EQUAL(true,rcp->m_mssgGapExpired);
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,rcp->m_defaultReplyTimeout);
    CPPUNIT_ASSERT_EQUAL(-1,rcp->m_threadChId);
    CPPUNIT_ASSERT_EQUAL(-1,rcp->m_threadCoId);
    CPPUNIT_ASSERT_EQUAL(-1,rcp->m_threadTid);
    CPPUNIT_ASSERT_EQUAL(string(DEF_PROXY_PROC_NAME),rcp->GetProcessName());
    CPPUNIT_ASSERT_EQUAL((tcflag_t)(CLOCAL | CREAD),
                         rcp->m_configProtocol.m_tio.c_cflag);

    short lflagOptions = ICANON | ECHO | ECHOE | ISIG; 
    CPPUNIT_ASSERT_EQUAL(
        (long unsigned int)0,
        rcp->m_configProtocol.m_tio.c_lflag & lflagOptions);

    CPPUNIT_ASSERT_EQUAL((tcflag_t)(IGNPAR | IGNBRK),
                         rcp->m_configProtocol.m_tio.c_iflag);
    CPPUNIT_ASSERT_EQUAL((tcflag_t)0,rcp->m_configProtocol.m_tio.c_oflag);
    CPPUNIT_ASSERT_EQUAL((unsigned char)0,
                         rcp->m_configProtocol.m_tio.c_cc[VINTR]);
    CPPUNIT_ASSERT_EQUAL((unsigned char)0,
                         rcp->m_configProtocol.m_tio.c_cc[VQUIT]);
    CPPUNIT_ASSERT_EQUAL((unsigned char)0,
                         rcp->m_configProtocol.m_tio.c_cc[VSUSP]);
    CPPUNIT_ASSERT_EQUAL((unsigned char)1,
                         rcp->m_configProtocol.m_tio.c_cc[VMIN]);
    CPPUNIT_ASSERT_EQUAL((unsigned char)0,
                         rcp->m_configProtocol.m_tio.c_cc[VTIME]);
}

//-----------------------------------------------------------------------------
void CommonCommProxyTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "CommonCommProxyConfig.xml";

    rcp->m_pathName = "Scanner";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    rcp->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(string("/dev/ser2"),rcp->m_comPortName);
    CPPUNIT_ASSERT_EQUAL((uint64_t)100000000,rcp->m_defaultReplyTimeout);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1025,rcp->m_fifoSize);
    CPPUNIT_ASSERT_EQUAL(string("Scanner_BusComm1.log"),
                         rcp->m_generalCommLog.GetAdditionalLogFileName());
}

//-----------------------------------------------------------------------------
void CommonCommProxyTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "CommonCommProxyConfigMissingTags.xml";

    rcp->m_pathName = "Scanner";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    rcp->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL((uint32_t)1024,rcp->m_fifoSize);
}

//-----------------------------------------------------------------------------
void CommonCommProxyTest::TestHandlePulse(void)
{
    string configFile = m_configPath + "CommonCommProxyConfigReload.xml";

    rcp->m_pathName = "Scanner";

    // set a new configuration file so that we can make sure it was reloaded
    rcp->SetConfigFile(configFile);

    // send the reload configuration pulse
    rcp->HandlePulse(SYSTEM_PULSE,RELOAD_PULSE);

    CPPUNIT_ASSERT_EQUAL(string("/dev/ser1"),rcp->m_comPortName);
    CPPUNIT_ASSERT_EQUAL((uint64_t)200000000,rcp->m_defaultReplyTimeout);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1026,rcp->m_fifoSize);
    CPPUNIT_ASSERT_EQUAL(string("Scanner_BusComm2.log"),
                         rcp->m_generalCommLog.GetAdditionalLogFileName());
}
