#ifndef CommonCommProxyTest_h
#define CommonCommProxyTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class RawCommProxy;

class CommonCommProxyTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CommonCommProxyTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(TestHandlePulse);
    CPPUNIT_TEST_SUITE_END();

public:
    CommonCommProxyTest();
    ~CommonCommProxyTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the RawCommProxy object is constructed properly
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems() method 
     * properly reads the RawCommProxy configuration items from the 
     * configuration file.
     */
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the items set in the 
     * LoadAdditionalConfigurationItems() method are set to the default
     * if the tag is missing in the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems_MissingTags(void);
    /**
     * Test that will verify the HandlePulse() method properly
     * processes the reload configuration pulse.
     */
    void TestHandlePulse(void);

    /** The RawCommProxy object under test */
    RawCommProxy *rcp;

    /** The path to the test configuration files */
    string m_configPath;
};

#endif
