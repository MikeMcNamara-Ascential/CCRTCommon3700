#include "BaldorMotorControllerTest.h"
#include "BaldorMotorController.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
BaldorMotorControllerTest::BaldorMotorControllerTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" << endl;
        exit(1);
    }

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += "/Core/Unit/ServicesLayer/Servers/MotorController/Electric/Baldor/Configuration/";
}

//-----------------------------------------------------------------------------
BaldorMotorControllerTest::~BaldorMotorControllerTest()
{
}

//-----------------------------------------------------------------------------
void BaldorMotorControllerTest::setUp()
{
    bmc = new BaldorMotorController();
}

//-----------------------------------------------------------------------------
void BaldorMotorControllerTest::tearDown()
{
    delete bmc;
}

//-----------------------------------------------------------------------------
void BaldorMotorControllerTest::TestConstructor()
{
    CPPUNIT_ASSERT(bmc->m_motorConfig == NULL);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0,bmc->m_minBrakeAssistSpeed,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(200.0,bmc->m_BrakeAssistForce,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0,bmc->m_maxPositiveTorqueLimit,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-100.0,bmc->m_maxNegativeTorqueLimit,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0,bmc->m_torqueAssistMinMPH,0.1);
    CPPUNIT_ASSERT_EQUAL(false,bmc->m_boost);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.8,bmc->m_brakingFactor,0.1);
    CPPUNIT_ASSERT_EQUAL((UINT32)0,bmc->m_interMotorSpeedDelay);
    CPPUNIT_ASSERT_EQUAL((UINT32)2000,bmc->m_lossCompensationMotorDelay);
    CPPUNIT_ASSERT_EQUAL((UINT32)2000,bmc->m_lossCompensationSpeedCheckDelay);
    CPPUNIT_ASSERT_EQUAL((UINT32)0,bmc->m_lossCompensationSpeedCheckCount);
    CPPUNIT_ASSERT_EQUAL((UINT32)0,bmc->m_lossCompensationSpeedCheckCurrentCount);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,bmc->m_lcValidationSpeed,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0,bmc->m_torqueModeSpeedLimit,0.1);
    CPPUNIT_ASSERT_EQUAL(true,bmc->m_itsAllGood);
    CPPUNIT_ASSERT_EQUAL(false,bmc->m_enableStatusChecking);
    CPPUNIT_ASSERT_EQUAL(false,bmc->m_enableMotorCommFault);
    CPPUNIT_ASSERT_EQUAL(false,bmc->m_overrideMinParameter);
    CPPUNIT_ASSERT_EQUAL(false,bmc->m_enableQuickBrake);
    CPPUNIT_ASSERT_EQUAL(true,bmc->m_axleSpeedMatch);
}

//-----------------------------------------------------------------------------
void BaldorMotorControllerTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "BaldorMotorControllerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    bmc->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0,bmc->m_minBrakeAssistSpeed,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-200.0,bmc->m_BrakeAssistForce,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(11.0,bmc->m_torqueAssistMinMPH,0.1);
    CPPUNIT_ASSERT_EQUAL((UINT32)2001,bmc->m_lossCompensationMotorDelay);
    CPPUNIT_ASSERT_EQUAL((UINT32)3000,bmc->m_lossCompensationSpeedCheckDelay);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.1,bmc->m_lcValidationSpeed,0.1);
    CPPUNIT_ASSERT_EQUAL(
        string("/Configuration/Machine/LeftFrontBaldorDriveParameters-25M.xml"),
        bmc->m_driveParameters.getNode(0)->getValue());
    CPPUNIT_ASSERT_EQUAL(document->getChild("Setup")->getChild("Motors"),
                         bmc->m_motorConfig);

    const XmlNode* param = 
        document->getChild("Setup")->getChild("ElectricMotorParameters");

    CPPUNIT_ASSERT_EQUAL(
        string(BposGetEnv("FTP_ROOT")) + 
        param->getChild("ProtocolFile")->getValue(),
        bmc->m_serialMessages);
    CPPUNIT_ASSERT_EQUAL(true,bmc->m_enableQuickBrake);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(20.0,bmc->m_torqueModeSpeedLimit,0.1);
    CPPUNIT_ASSERT_EQUAL(true,bmc->m_enableStatusChecking);
    CPPUNIT_ASSERT_EQUAL(false,bmc->m_axleSpeedMatch);    
}

//-----------------------------------------------------------------------------
void BaldorMotorControllerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "BaldorMotorControllerConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    bmc->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(false,bmc->m_enableStatusChecking);
    CPPUNIT_ASSERT_EQUAL(true,bmc->m_axleSpeedMatch);    
}

