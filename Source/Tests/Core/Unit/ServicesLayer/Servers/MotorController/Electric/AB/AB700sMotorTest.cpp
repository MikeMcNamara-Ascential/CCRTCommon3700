#include "AB700sMotorTest.h"
#include "AB700sMotor.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
AB700sMotorTest::AB700sMotorTest()
{
    // make sure the TEST_DIR_PATH is valid
	if(getenv("TEST_DIR_PATH") == NULL)
	{
		cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" << endl;
		exit(1);
	}

    // set the path to the test configuration files
	m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += "/Core/Unit/ServicesLayer/Servers/MotorController/Electric/AB/Configuration/";
}

//-----------------------------------------------------------------------------
AB700sMotorTest::~AB700sMotorTest()
{
}

//-----------------------------------------------------------------------------
void AB700sMotorTest::setUp()
{
    am = new AB700sMotor();
}

//-----------------------------------------------------------------------------
void AB700sMotorTest::tearDown()
{
    delete am;
}

//-----------------------------------------------------------------------------
void AB700sMotorTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL((uint16_t)0,am->m_driveIndex);
    CPPUNIT_ASSERT_EQUAL((uint32_t)-1,am->m_sessionHandle);
    CPPUNIT_ASSERT(am->m_cipFwdOpenRsp == NULL);
    CPPUNIT_ASSERT_EQUAL(-1,am->m_sockFd);
    CPPUNIT_ASSERT_EQUAL(-1,am->m_readSock);
    CPPUNIT_ASSERT_EQUAL(-1,am->m_writeSock);
    CPPUNIT_ASSERT_EQUAL(-1,am->m_chId);
    CPPUNIT_ASSERT_EQUAL(-1,am->m_coId);
    CPPUNIT_ASSERT_EQUAL(false,am->m_hwDriveEnabled);
    CPPUNIT_ASSERT_EQUAL(false,am->m_enableDriveNeeded);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(180.0,am->m_maxTorque,0.1);
    CPPUNIT_ASSERT_EQUAL(string("0.0.0.0"),am->m_ipAddress);
    CPPUNIT_ASSERT_EQUAL((uint16_t)-1,am->m_tcpPort);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1000000,am->m_origRpi);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1000000,am->m_targRpi);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1780.0,am->m_baseSpeed,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0,am->m_maxPosTorque,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0,am->m_maxNegTorque,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(30.0,am->m_motorHP,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,am->m_ftLbsPerHp,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0,am->m_torqueSpeedLimit,0.1);
}

//-----------------------------------------------------------------------------
void AB700sMotorTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "AB700sMotorConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *params = 
        document->getChild("Setup")->getChild("Motors")->getChild("LeftFrontMotor");
    am->LoadAdditionalConfigurationItems(params);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1175.0,am->m_baseSpeed,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(31.0,am->m_motorHP,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5,am->m_ftLbsPerHp,0.1);
    CPPUNIT_ASSERT_EQUAL(string("192.168.10.0"),am->m_ipAddress);
    CPPUNIT_ASSERT_EQUAL((uint16_t)44818,am->m_tcpPort);
    CPPUNIT_ASSERT_EQUAL((uint32_t)500,am->m_origRpi);
}

//-----------------------------------------------------------------------------
void AB700sMotorTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "AB700sMotorConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *params = 
        document->getChild("Setup")->getChild("Motors")->getChild("LeftFrontMotor");
    am->LoadAdditionalConfigurationItems(params);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1780.0,am->m_baseSpeed,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(30.0,am->m_motorHP,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,am->m_ftLbsPerHp,0.1);
    CPPUNIT_ASSERT_EQUAL(string("0.0.0.0"),am->m_ipAddress);
    CPPUNIT_ASSERT_EQUAL((uint16_t)-1,am->m_tcpPort);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1000,am->m_origRpi);
}
