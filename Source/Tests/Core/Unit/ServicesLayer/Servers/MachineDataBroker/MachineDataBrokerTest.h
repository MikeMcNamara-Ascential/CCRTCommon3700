#ifndef MACHINEDATABROKERTEST_H
#define MACHINEDATABROKERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

class MachineDataBroker;
class XmlParser;

class MachineDataBrokerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MachineDataBrokerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestSetMachineNumber_ConfigFile);
    CPPUNIT_TEST(TestSetMachineNumber_Environment);
    CPPUNIT_TEST(TestSetMachineNumber_MissingTag);
    CPPUNIT_TEST(TestSetupCycleTimer);
    CPPUNIT_TEST(TestSetupCycleTimer_MissingTag);
    CPPUNIT_TEST(TestSetupCycleTimer_ZeroRate);
    CPPUNIT_TEST(TestSetupPowerSupplyMonitoring);
    CPPUNIT_TEST(TestSetupPowerSupplyMonitoring_MissingTags);
    CPPUNIT_TEST(TestSetupPowerSupplyMonitoring_ZeroRate);
    CPPUNIT_TEST_SUITE_END();

public:
    MachineDataBrokerTest();
    ~MachineDataBrokerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the MachineDataBroker object is 
     * constructed properly
     */ 
    void TestConstructor(void);

    /**
     * Test that will verify the additional configuration items needed
     * by the machine data broker are set correctly.
     */ 
    void TestLoadAdditionalConfigurationItems(void);

    /**
     * Test that will verify the machine number is correctly set when
     * read in from the configuration file.
     */ 
    void TestSetMachineNumber_ConfigFile(void);

    /**
     * Test that will verify the machine number is correctly set when
     * read in from the environment variable
     */ 
    void TestSetMachineNumber_Environment(void);

    /**
     * Test that will verify the machine number is correctly set when
     * the tag is missing from the configuration file
     */ 
    void TestSetMachineNumber_MissingTag(void);

    /**
     * Test that will verify the cycle timer is setup correctly.
     */ 
    void TestSetupCycleTimer(void);

    /**
     * Test that will verify the cycle time is set to the default if
     * the tag is missing from the configuration file.
     */ 
    void TestSetupCycleTimer_MissingTag(void);

    /**
     * Test that will verify the cycle timer is not setup if the rate
     * is set to zero in the configuration file.
     */ 
    void TestSetupCycleTimer_ZeroRate(void);

    /**
     * Test that will verify the power supply monitoring is setup correctly.
     */ 
    void TestSetupPowerSupplyMonitoring(void);

    /**
     * Test that will verify the power supply monitoring is setup correctly
     * when the tags are missing from the configuration file
     */ 
    void TestSetupPowerSupplyMonitoring_MissingTags(void);

    /**
     * Test that will verify the power supply monitoring is not setup if
     * the rate is set to zero in the configuration file.
     */ 
    void TestSetupPowerSupplyMonitoring_ZeroRate(void);

    /** Machine data broker object to use for testing */
    MachineDataBroker *mdb;

    /** Xml parser used to read in the configuration files */
    XmlParser *parser;

    /** The path to the test configuration files */
    string m_configPath;
};
#endif
