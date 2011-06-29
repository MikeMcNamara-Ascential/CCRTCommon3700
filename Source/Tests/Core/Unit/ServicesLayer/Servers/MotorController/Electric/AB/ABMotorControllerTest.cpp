#include "ABMotorControllerTest.h"
#include "ABMotorController.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
ABMotorControllerTest::ABMotorControllerTest()
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
ABMotorControllerTest::~ABMotorControllerTest()
{
}

//-----------------------------------------------------------------------------
void ABMotorControllerTest::setUp()
{
    amc = new ABMotorController();
}

//-----------------------------------------------------------------------------
void ABMotorControllerTest::tearDown()
{
    delete amc;
}

//-----------------------------------------------------------------------------
void ABMotorControllerTest::TestConstructor()
{
    CPPUNIT_ASSERT(amc->m_motorConfig == NULL);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0,amc->m_minBrakeAssistSpeed,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(200.0,amc->m_brakeAssistForce,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0,amc->m_maxPositiveTorqueLimit,0.1); 
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-100.0,amc->m_maxNegativeTorqueLimit,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0,amc->m_torqueAssistMinMPH,0.1); 
    CPPUNIT_ASSERT_EQUAL(false,amc->m_boost); 
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.8,amc->m_brakingFactor,0.1);
    CPPUNIT_ASSERT_EQUAL((int)LeftFront,amc->m_masterMotor); 
    CPPUNIT_ASSERT_EQUAL((UINT32)0,amc->m_interMotorSpeedDelay);
    CPPUNIT_ASSERT_EQUAL((UINT32)2000,amc->m_lossCompensationMotorDelay); 
    CPPUNIT_ASSERT_EQUAL((UINT32)2000,amc->m_lossCompensationSpeedCheckDelay);
    CPPUNIT_ASSERT_EQUAL((UINT32)0,amc->m_lossCompensationSpeedCheckCount); 
    CPPUNIT_ASSERT_EQUAL((UINT32)0,amc->m_lossCompensationSpeedCheckCurrentCount);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,amc->m_lcValidationSpeed,0.1); 
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0,amc->m_torqueModeSpeedLimit,0.1); 
    CPPUNIT_ASSERT_EQUAL(true,amc->m_itsAllGood);
    CPPUNIT_ASSERT_EQUAL(false,amc->m_enableStatusChecking); 
    CPPUNIT_ASSERT_EQUAL(false,amc->m_enableMotorCommFault); 
    CPPUNIT_ASSERT_EQUAL(false,amc->m_overrideMinParameter);
    CPPUNIT_ASSERT_EQUAL(false,amc->m_enableQuickBrake); 
    CPPUNIT_ASSERT_EQUAL(true,amc->m_axleSpeedMatch);
}

//-----------------------------------------------------------------------------
void ABMotorControllerTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "ABMotorControllerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    amc->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0,amc->m_minBrakeAssistSpeed,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-200.0,amc->m_brakeAssistForce,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(11.0,amc->m_torqueAssistMinMPH,0.1);
    CPPUNIT_ASSERT_EQUAL((UINT32)2001,amc->m_lossCompensationMotorDelay);
    CPPUNIT_ASSERT_EQUAL((UINT32)3000,amc->m_lossCompensationSpeedCheckDelay);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.1,amc->m_lcValidationSpeed,0.1);
    CPPUNIT_ASSERT_EQUAL(
        string("/Configuration/Machine/LeftFrontBaldorDriveParameters-25M.xml"),
        amc->m_driveParameters.getNode(0)->getValue());
    CPPUNIT_ASSERT_EQUAL(document->getChild("Setup")->getChild("Motors"),
                         amc->m_motorConfig);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.7,amc->m_brakingFactor,0.1);
    CPPUNIT_ASSERT_EQUAL(true,amc->m_enableQuickBrake);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(20.0,amc->m_torqueModeSpeedLimit,0.1);
    CPPUNIT_ASSERT_EQUAL(true,amc->m_enableStatusChecking);
    CPPUNIT_ASSERT_EQUAL(false,amc->m_axleSpeedMatch);    
}

//-----------------------------------------------------------------------------
void ABMotorControllerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "ABMotorControllerConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    amc->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(false,amc->m_enableStatusChecking);
    CPPUNIT_ASSERT_EQUAL(true,amc->m_axleSpeedMatch);    
}

