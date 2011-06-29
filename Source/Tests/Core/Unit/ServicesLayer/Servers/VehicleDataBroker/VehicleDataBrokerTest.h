#ifndef VEHICLEDATABROKERTEST_H
#define VEHICLEDATABROKERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class VehicleDataBroker;
class MockNamedDataBroker;

class VehicleDataBrokerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(VehicleDataBrokerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_InvalidName);
    CPPUNIT_TEST(TestUpdatePLC);
    CPPUNIT_TEST(TestUpdatePLC_InvalidTag);
    CPPUNIT_TEST(TestUpdatePLC_Fail);
    CPPUNIT_TEST_SUITE_END();

public:
    VehicleDataBrokerTest();
    ~VehicleDataBrokerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the VehicleDataBroker object is 
     * constructed properly
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the VehicleDataBroker configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the items set in the 
     * LoadAdditionalConfigurationItems() method are set to the default
     * if the tag is missing in the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems_MissingTags(void);
    /**
     * Test that will verify the communication channel name is set to the
     * default if the name specified in the configuration file is invalid.
     */ 
    void TestLoadAdditionalConfigurationItems_InvalidName(void);
    /**
     * Test that will verify UpdatePLC() works properly in the successful case.
     */ 
    void TestUpdatePLC(void);
    /**
     * Test that will verify UpdatePLC() properly handles the case where a tag
     * specified to update is not in the vehicle build.
     */ 
    void TestUpdatePLC_InvalidTag(void);
    /**
     * Test that will verify UpdatePLC() properly handles the case where 
     * writing the PLC data fails.
     */ 
    void TestUpdatePLC_Fail(void);

    /** The Vehicle Data Broker object under test */
    VehicleDataBroker *vdb;

    /** The path to the test configuration files */
    string m_configPath;

    /** MockNamedDataBroker object to use for testing */
    MockNamedDataBroker* mndb;

    /** Task id for the mock named data broker */
    int m_ndbId;
};
#endif
