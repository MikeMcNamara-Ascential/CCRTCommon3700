#include "MachineDataBrokerTest.h"
#include "MachineDataBroker.h"
#include "SoftwareId.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
MachineDataBrokerTest::MachineDataBrokerTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" << endl;
        exit(1);
    }

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += "/Core/Unit/ServicesLayer/Servers/MachineDataBroker/Configuration/";
}

//-----------------------------------------------------------------------------
MachineDataBrokerTest::~MachineDataBrokerTest()
{
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::setUp()
{
    mdb = new MachineDataBroker();
    parser = new XmlParser();
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::tearDown()
{
    delete parser;
    delete mdb;
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL(0,mdb->m_numberOfTestsSinceLossCompensation);
    CPPUNIT_ASSERT_EQUAL(false,mdb->m_cycleTimerStarted);
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestSetMachineNumber_ConfigFile(void)
{
    string configFile = m_configPath + "MachineDataBrokerConfig.xml";

    mdb->SetConfigFile(configFile);
    const XmlNode *document = parser->ReturnXMLDocument(configFile);

    // write the machine number from the configuration file
    mdb->Write(document->getChild("Setup/DataTags/MachineNumberTag")->getValue(),
              document->getChild("Setup/TagList/PublishTags/MachineNumber")->getValue());

    mdb->SetMachineNumber(document);

    // make sure the machine number value from the configuration file was not
    // overwritten
    CPPUNIT_ASSERT_EQUAL(2,atoi(mdb->Read(document->getChild("Setup/DataTags/MachineNumberTag")->getValue()).c_str()));
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestSetMachineNumber_Environment(void)
{
    string configFile = m_configPath + "MachineDataBrokerConfigEnv.xml";

    mdb->SetConfigFile(configFile);
    const XmlNode *document = parser->ReturnXMLDocument(configFile);

    setenv("MACHINE","3",1);    // set the machine environment variable to 3

    mdb->SetMachineNumber(document);

    // make sure the machine number environment variable was written correctly
    CPPUNIT_ASSERT_EQUAL(3,atoi(mdb->Read(document->getChild("Setup/DataTags/MachineNumberTag")->getValue()).c_str()));
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestSetMachineNumber_MissingTag(void)
{
    string configFile = m_configPath + "MachineDataBrokerConfigMissingTags.xml";

    mdb->SetConfigFile(configFile);
    const XmlNode *document = parser->ReturnXMLDocument(configFile);

    // write the machine number from the configuration file
    mdb->Write(document->getChild("Setup/DataTags/MachineNumberTag")->getValue(),
              document->getChild("Setup/TagList/PublishTags/MachineNumber")->getValue());

    mdb->SetMachineNumber(document);

    // make sure the machine number value from the configuration file was not
    // overwritten
    CPPUNIT_ASSERT_EQUAL(2,atoi(mdb->Read(document->getChild("Setup/DataTags/MachineNumberTag")->getValue()).c_str()));
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestSetupCycleTimer(void)
{
    string configFile = m_configPath + "MachineDataBrokerConfig.xml";

    mdb->SetConfigFile(configFile);
    const XmlNode *document = parser->ReturnXMLDocument(configFile);

    // initialize will call SetupCycleTimer()
    mdb->Initialize(document);

    // make sure the pulse code is set correctly
    CPPUNIT_ASSERT_EQUAL(MACHINE_DATA_BROKER_PULSE_CODE, 
                         mdb->m_cycleTimeTimer.GetPulseCode());
    // make sure the pulse value is set correctly
    CPPUNIT_ASSERT_EQUAL(CYCLE_TIMER_PULSE,mdb->m_cycleTimeTimer.GetPulseValue());
    // make sure the cycle time rate is set correctly
    CPPUNIT_ASSERT_EQUAL((UINT64)550000000,mdb->m_cycleTimeTimer.GetInitialDelay());
    // make sure the cycle timer was stopped
    CPPUNIT_ASSERT_EQUAL((UINT64)0,mdb->m_cycleTimeTimer.GetInterval());    
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestSetupCycleTimer_MissingTag(void)
{
    string configFile = m_configPath + "MachineDataBrokerConfigMissingTags.xml";

    mdb->SetConfigFile(configFile);
    const XmlNode *document = parser->ReturnXMLDocument(configFile);

    // initialize will call SetupCycleTimer()
    mdb->Initialize(document);

    // make sure the pulse code is set correctly
    CPPUNIT_ASSERT_EQUAL(MACHINE_DATA_BROKER_PULSE_CODE, 
                         mdb->m_cycleTimeTimer.GetPulseCode());
    // make sure the pulse value is set correctly
    CPPUNIT_ASSERT_EQUAL(CYCLE_TIMER_PULSE,mdb->m_cycleTimeTimer.GetPulseValue());
    // make sure the cycle time rate is set correctly
    CPPUNIT_ASSERT_EQUAL((UINT64)500000000,mdb->m_cycleTimeTimer.GetInitialDelay());
    // make sure the cycle timer was stopped
    CPPUNIT_ASSERT_EQUAL((UINT64)0,mdb->m_cycleTimeTimer.GetInterval());    
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestSetupCycleTimer_ZeroRate(void)
{
    string configFile = m_configPath + "MachineDataBrokerConfigZeroRate.xml";

    mdb->SetConfigFile(configFile);
    const XmlNode *document = parser->ReturnXMLDocument(configFile);

    // initialize will call SetupCycleTimer()
    mdb->Initialize(document);

    // make sure the timer id isn't set
    CPPUNIT_ASSERT_EQUAL((UINT32)-1,mdb->m_cycleTimeTimer.GetTimerId());
    // make sure the pulse code is set correctly
    CPPUNIT_ASSERT_EQUAL(SYSTEM_PULSE,mdb->m_cycleTimeTimer.GetPulseCode());
    // make sure the pulse value is set correctly
    CPPUNIT_ASSERT_EQUAL(TIMER_PULSE,mdb->m_cycleTimeTimer.GetPulseValue());
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestSetupPowerSupplyMonitoring(void)
{
    string configFile = m_configPath + "MachineDataBrokerConfig.xml";

    mdb->SetConfigFile(configFile);
    const XmlNode *document = parser->ReturnXMLDocument(configFile);

    // initialize will call SetupPowerSupplyMonitoring()
    mdb->Initialize(document);

    // make sure the pulse code is set correctly
    CPPUNIT_ASSERT_EQUAL(MACHINE_DATA_BROKER_PULSE_CODE, 
                         mdb->m_PowerSupplyTimer.GetPulseCode());
    // make sure the pulse value is set correctly
    CPPUNIT_ASSERT_EQUAL(POWER_SUPPLY_PULSE,mdb->m_PowerSupplyTimer.GetPulseValue());
    // make sure the power supply monitoring rate is set correctly
    CPPUNIT_ASSERT_EQUAL((UINT64)560000000,mdb->m_PowerSupplyTimer.GetInitialDelay());
    // make sure the power supply monitoring timer was stopped
    CPPUNIT_ASSERT_EQUAL((UINT64)0,mdb->m_PowerSupplyTimer.GetInterval());    
    // make sure the minimum value was set correctly
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.70,mdb->m_encoder5vMin,0.001);
    // make sure the maximum value was set correctly
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.20,mdb->m_encoder5vMax,0.001);    
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestSetupPowerSupplyMonitoring_MissingTags(void)
{
    string configFile = m_configPath + "MachineDataBrokerConfigMissingTags.xml";

    mdb->SetConfigFile(configFile);
    const XmlNode *document = parser->ReturnXMLDocument(configFile);

    // initialize will call SetupPowerSupplyMonitoring()
    mdb->Initialize(document);

    // make sure the pulse code is set correctly
    CPPUNIT_ASSERT_EQUAL(MACHINE_DATA_BROKER_PULSE_CODE, 
                         mdb->m_PowerSupplyTimer.GetPulseCode());
    // make sure the pulse value is set correctly
    CPPUNIT_ASSERT_EQUAL(POWER_SUPPLY_PULSE,mdb->m_PowerSupplyTimer.GetPulseValue());
    // make sure the power supply monitoring time rate is set correctly
    CPPUNIT_ASSERT_EQUAL((UINT64)500000000,mdb->m_PowerSupplyTimer.GetInitialDelay());
    // make sure the power supply monitoring timer was stopped
    CPPUNIT_ASSERT_EQUAL((UINT64)0,mdb->m_PowerSupplyTimer.GetInterval());    
    // make sure the minimum value was set correctly
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.75,mdb->m_encoder5vMin,0.001);
    // make sure the maximum value was set correctly
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.25,mdb->m_encoder5vMax,0.001);    
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestSetupPowerSupplyMonitoring_ZeroRate(void)
{
    string configFile = m_configPath + "MachineDataBrokerConfigZeroRate.xml";

    mdb->SetConfigFile(configFile);
    const XmlNode *document = parser->ReturnXMLDocument(configFile);

    // initialize will call SetupPowerSupplyMonitoring()
    mdb->Initialize(document);

    // make sure the timer id isn't set
    CPPUNIT_ASSERT_EQUAL((UINT32)-1,mdb->m_PowerSupplyTimer.GetTimerId());
    // make sure the pulse code is set correctly
    CPPUNIT_ASSERT_EQUAL(SYSTEM_PULSE,mdb->m_PowerSupplyTimer.GetPulseCode());
    // make sure the pulse value is set correctly
    CPPUNIT_ASSERT_EQUAL(TIMER_PULSE,mdb->m_PowerSupplyTimer.GetPulseValue());
}

//-----------------------------------------------------------------------------
void MachineDataBrokerTest::TestLoadAdditionalConfigurationItems(void)
{
    string configFile = m_configPath + "MachineDataBrokerConfig.xml";

    mdb->SetConfigFile(configFile);
    const XmlNode *document = parser->ReturnXMLDocument(configFile);

    // initialize will call LoadAdditionalConfigurationItems()
    mdb->Initialize(document);

    // make sure the loss compensation file was set correctly
    CPPUNIT_ASSERT_EQUAL(string("/Configuration/Machine/LossCompensationTimeDate.dat"),
                         mdb->m_dataFile);

    // build up the id string for comparison
    string custId = document->getChild("Setup")->getChild("CustId")->getValue();
    string custBld = document->getChild("Setup")->getChild("CustBld")->getValue();
    SoftwareId id(custId,custBld);
    string idString = id.GetPartialId("ProdID") + id.GetPartialId("Type") +
        id.GetPartialId("CoreBuild") + "-" + custId + "-" + custBld;
    // make sure the software id string is correct
    CPPUNIT_ASSERT_EQUAL(idString,mdb->Read(SOFTWARE_ID));
}

