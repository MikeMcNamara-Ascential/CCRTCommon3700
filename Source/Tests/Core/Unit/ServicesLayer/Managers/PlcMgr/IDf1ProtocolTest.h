#ifndef IDf1ProtocolTest_h
#define IDf1ProtocolTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class IDf1Protocol;

class IDf1ProtocolTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(IDf1ProtocolTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST_SUITE_END();

public:
    IDf1ProtocolTest();
    ~IDf1ProtocolTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the IDf1Protocol object is constructed properly
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the IDf1Protocol configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the items set in the 
     * LoadAdditionalConfiguraitonItems() method are set to the default
     * if the tag is missing in the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems_MissingTags(void);

    /** The IDf1Protocol object under test */
    IDf1Protocol *plc;

    /** The path to the test configuration files */
    string m_configPath;
};
#endif
