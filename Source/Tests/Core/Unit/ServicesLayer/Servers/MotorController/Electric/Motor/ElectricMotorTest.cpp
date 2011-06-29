#include "ElectricMotorTest.h"
#include "MotorController.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
ElectricMotorTest::ElectricMotorTest()
{
    // make sure the TEST_DIR_PATH is valid
	if(getenv("TEST_DIR_PATH") == NULL)
	{
		cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" << endl;
		exit(1);
	}

    // set the path to the test configuration files
	m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += "/Core/Unit/ServicesLayer/Servers/MotorController/Electric/Motor/Configuration/";
}

//-----------------------------------------------------------------------------
ElectricMotorTest::~ElectricMotorTest()
{
}

//-----------------------------------------------------------------------------
void ElectricMotorTest::setUp()
{
    mot = new MockMotor();
}

//-----------------------------------------------------------------------------
void ElectricMotorTest::tearDown()
{
    delete mot;
}

//-----------------------------------------------------------------------------
void ElectricMotorTest::TestConstructor()
{
    CPPUNIT_ASSERT_DOUBLES_EQUAL(28.30612546,mot->m_mphToRpm,0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.723957011,mot->m_torqueBitsPerPound,0.00001);
    CPPUNIT_ASSERT_EQUAL(2800,mot->m_maxRPM);
    CPPUNIT_ASSERT_EQUAL(100,mot->m_maxPositiveTorqueLimit);
    CPPUNIT_ASSERT_EQUAL(-100,mot->m_maxNegativeTorqueLimit);
    CPPUNIT_ASSERT(mot->m_iNDB == NULL);
    CPPUNIT_ASSERT_EQUAL(false,mot->m_commFaultStatus);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mot->m_commandValue,0.1);
    CPPUNIT_ASSERT_EQUAL((UINT32)0,mot->m_commandDelay);
    CPPUNIT_ASSERT_EQUAL(0,mot->m_itsAllGoodCounter);
    CPPUNIT_ASSERT_EQUAL(100,mot->m_threadLockupCountLimit);
}

//-----------------------------------------------------------------------------
void ElectricMotorTest::TestLoadAdditionalConfigurationItems()
{
    XmlParser parser;
    string configFile = m_configPath + "ElectricMotorConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *parameters = document->getChild("Setup")->getChild("Motors")->getChild("LeftFrontMotor");
    mot->LoadAdditionalConfigurationItems(parameters);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(28.12345,mot->m_mphToRpm,0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.12345,mot->m_torqueBitsPerPound,0.00001);
    CPPUNIT_ASSERT_EQUAL(2801,mot->m_maxRPM);
    CPPUNIT_ASSERT_EQUAL(101,mot->m_maxPositiveTorqueLimit);
    CPPUNIT_ASSERT_EQUAL(-1001,mot->m_maxNegativeTorqueLimit);
    CPPUNIT_ASSERT_EQUAL(11,mot->m_threadLockupCountLimit);
    CPPUNIT_ASSERT_EQUAL(1,mot->m_motorCommFaultLimit);
    CPPUNIT_ASSERT_EQUAL(string("LFRollMotorDriveCommFault"),mot->m_commFault);
}

//-----------------------------------------------------------------------------
void ElectricMotorTest::TestLoadAdditionalConfigurationItems_NegativeCommFaultLimit(void)
{
    XmlParser parser;
    string configFile = m_configPath + "ElectricMotorConfigNegCommFaultLimit.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *parameters = document->getChild("Setup")->getChild("Motors")->getChild("LeftFrontMotor");
    mot->LoadAdditionalConfigurationItems(parameters);

    CPPUNIT_ASSERT_EQUAL(0,mot->m_motorCommFaultLimit);
}

