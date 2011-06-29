#include "AvtCanProxyTest.h"
#include "AvtCanProxy.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
AvtCanProxyTest::AvtCanProxyTest()
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
        "/Core/Unit/ServicesLayer/Managers/CommProxy/AVT/CAN/Configuration/";
}

//-----------------------------------------------------------------------------
AvtCanProxyTest::~AvtCanProxyTest()
{
}

//-----------------------------------------------------------------------------
void AvtCanProxyTest::setUp()
{
    accp = new AvtCanProxy();
}

//-----------------------------------------------------------------------------
void AvtCanProxyTest::tearDown()
{
    delete accp;
}

//-----------------------------------------------------------------------------
void AvtCanProxyTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL((size_t)0,accp->m_avtBytesToCome);
    CPPUNIT_ASSERT_EQUAL(false,accp->m_waitingForAck);

    const uint8_t AvtAckString[] = AVT_ACK_STRING;
    const SerialString_t translatorAckString(AvtAckString);
    CPPUNIT_ASSERT(accp->m_translatorAckString[0] == translatorAckString[0]);
    CPPUNIT_ASSERT(accp->m_translatorAckString[1] == translatorAckString[1]);

    CPPUNIT_ASSERT_EQUAL(100,accp->m_translatorAckTimeout);
    CPPUNIT_ASSERT_EQUAL(-1,accp->m_avtAckChid);
    CPPUNIT_ASSERT_EQUAL(-1,accp->m_avtAckCoid);
    CPPUNIT_ASSERT(accp->m_clientBeingAcked == NULL);
    CPPUNIT_ASSERT_EQUAL((uint8_t)0x00,accp->m_avtVersion);
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x200,accp->m_avtTimeoutPulseVal);
    CPPUNIT_ASSERT_EQUAL(AvtCanProxy::AVT718_LONG_FORM,accp->m_avtMssgFormat);
    CPPUNIT_ASSERT_EQUAL(false,accp->m_enableMssgPadding);
    CPPUNIT_ASSERT_EQUAL(string(AVT_CAN_PROC_NAME),accp->GetProcessName());
}

//-----------------------------------------------------------------------------
void AvtCanProxyTest::TestReadPortConfig(void)
{
    XmlParser parser;
    string configFile = m_configPath + "AvtCanProxyConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *avtCanNode = document->getChild("Setup/LogicalPorts/AvtCan");

    accp->ReadPortConfig(avtCanNode);

    CPPUNIT_ASSERT_EQUAL(AvtCanProxy::AVT718_SHORT_FORM,accp->m_avtMssgFormat);
    CPPUNIT_ASSERT_EQUAL(true,accp->m_enableMssgPadding);
}

//-----------------------------------------------------------------------------
void AvtCanProxyTest::TestReadPortConfig_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "AvtCanProxyConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *avtCanNode = document->getChild("Setup/LogicalPorts/AvtCan");

    accp->ReadPortConfig(avtCanNode);

    CPPUNIT_ASSERT_EQUAL(AvtCanProxy::AVT718_LONG_FORM,accp->m_avtMssgFormat);
    CPPUNIT_ASSERT_EQUAL(false,accp->m_enableMssgPadding);
}
