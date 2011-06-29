#include "CommonMotorControllerTest.h"
#include "MotorController.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
CommonMotorControllerTest::CommonMotorControllerTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" << endl;
        exit(1);
    }

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += "/Core/Unit/ServicesLayer/Servers/MotorController/Common/Configuration/";
}

//-----------------------------------------------------------------------------
CommonMotorControllerTest::~CommonMotorControllerTest()
{
}

//-----------------------------------------------------------------------------
void CommonMotorControllerTest::setUp()
{
    mmc = new MockMotorController();
}

//-----------------------------------------------------------------------------
void CommonMotorControllerTest::tearDown()
{
    delete mmc;
}

//-----------------------------------------------------------------------------
void CommonMotorControllerTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL((INamedDataBroker*)NULL,mmc->m_dataComm);
    CPPUNIT_ASSERT_EQUAL((INamedDataBroker*)NULL,mmc->m_commandComm);
    CPPUNIT_ASSERT_EQUAL((IFaultServer*)NULL,mmc->m_faultComm);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-3.0,mmc->m_brakeDetectLimit,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0,mmc->m_brakeClearLimit,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,mmc->m_speedDeltaForBoost,0.1);
    CPPUNIT_ASSERT_EQUAL(100,mmc->m_updateRate);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,mmc->m_minimumSpeedForBoost,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_defaultAccelConst,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_accelerationLoadingConstant,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_defaultSpeedConst,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_speedLoadingConstant,0.1);
    CPPUNIT_ASSERT_EQUAL(true,mmc->m_motorLoadingEnabled);
    CPPUNIT_ASSERT_EQUAL(false,mmc->m_disableSpeedMatching);
    CPPUNIT_ASSERT_EQUAL(-1,mmc->m_speedFile);
    CPPUNIT_ASSERT_EQUAL(-1,mmc->m_accelFile);
    CPPUNIT_ASSERT_EQUAL(true,mmc->m_clutchOnZeroSpeed);
    CPPUNIT_ASSERT_EQUAL((HeartBeatThread*)NULL,mmc->m_heartBeatThread);
    CPPUNIT_ASSERT_EQUAL(string("LeftFront"),mmc->MotorDescriptions.at(0));
    CPPUNIT_ASSERT_EQUAL(string("RightFront"),mmc->MotorDescriptions.at(1));
    CPPUNIT_ASSERT_EQUAL(string("LeftRear"),mmc->MotorDescriptions.at(2));
    CPPUNIT_ASSERT_EQUAL(string("RightRear"),mmc->MotorDescriptions.at(3));
    CPPUNIT_ASSERT_EQUAL(string("LeftTandem"),mmc->MotorDescriptions.at(4));
    CPPUNIT_ASSERT_EQUAL(string("RightTandem"),mmc->MotorDescriptions.at(5));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_speeds.at(0),0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_speeds.at(1),0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_speeds.at(2),0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_speeds.at(3),0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_accelerations.at(0),0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_accelerations.at(1),0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_accelerations.at(2),0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_accelerations.at(3),0.1);
    CPPUNIT_ASSERT_EQUAL(string("Rear"),mmc->m_defaultDriveAxle);
}

//-----------------------------------------------------------------------------
void CommonMotorControllerTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "MotorControllerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    mmc->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-3.0,mmc->m_brakeDetectLimit,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0,mmc->m_brakeClearLimit,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,mmc->m_speedDeltaForBoost,0.1);
    CPPUNIT_ASSERT_EQUAL(false,mmc->m_reverseBoost);
    CPPUNIT_ASSERT_EQUAL(250,mmc->m_updateRate);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,mmc->m_minimumSpeedForBoost,0.1);
    CPPUNIT_ASSERT_EQUAL(string("/dev/Icm/Block/Speed1"),mmc->m_speedArray);
    CPPUNIT_ASSERT_EQUAL(string("/dev/Icm/Block/Acceleration1"),mmc->m_accelerationArray);
    CPPUNIT_ASSERT_EQUAL(true,mmc->m_disableSpeedMatching);
    CPPUNIT_ASSERT_EQUAL(document->getChild("Setup")->getChild("DataTags")->getChildren().getNode(0)->getValue(),
                         mmc->m_dataTags.getNode(0)->getValue());
    CPPUNIT_ASSERT_EQUAL(document->getChild("Setup")->getChild("Faults")->getChildren().getNode(0)->getValue(),
                         mmc->m_faults.getNode(0)->getValue());
    CPPUNIT_ASSERT_EQUAL(false,mmc->ClutchOnZeroSpeed());  
    CPPUNIT_ASSERT_EQUAL(string("Front"),mmc->m_defaultDriveAxle);
}

//-----------------------------------------------------------------------------
void CommonMotorControllerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "MotorControllerConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    mmc->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(string("/dev/Icm/Block/Speed"),mmc->m_speedArray);
    CPPUNIT_ASSERT_EQUAL(string("/dev/Icm/Block/Acceleration"),mmc->m_accelerationArray);
    CPPUNIT_ASSERT_EQUAL(false,mmc->m_disableSpeedMatching);
    CPPUNIT_ASSERT_EQUAL(true,mmc->ClutchOnZeroSpeed());        
    CPPUNIT_ASSERT_EQUAL(string("Rear"),mmc->m_defaultDriveAxle);
}

//-----------------------------------------------------------------------------
void CommonMotorControllerTest::TestSetupMotorLoading(void)
{
    XmlParser parser;
    string configFile = m_configPath + "MotorControllerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    mmc->SetupMotorLoading(document->getChild("Setup")->getChild("MotorParameters"));

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0,mmc->m_defaultAccelConst,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0,mmc->m_accelerationLoadingConstant,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,mmc->m_defaultSpeedConst,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,mmc->m_speedLoadingConstant,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,mmc->m_minSpeedForLoading,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(30.0,mmc->m_maxSpeedForLoading,0.1);
}

//-----------------------------------------------------------------------------
void CommonMotorControllerTest::TestSetupMotorLoading_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "MotorControllerConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    mmc->SetupMotorLoading(document->getChild("Setup")->getChild("MotorParameters"));

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_defaultAccelConst,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_defaultSpeedConst,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,mmc->m_minSpeedForLoading,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,mmc->m_maxSpeedForLoading,0.1);
}
