#ifndef PROMPTSERVERTEST_H
#define PROMPTSERVERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

class PromptServerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PromptServerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(TestHandlePulse);
    CPPUNIT_TEST_SUITE_END();

public:
    PromptServerTest();
    ~PromptServerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the PromptServer object is 
     * constructed properly
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the PromptServer configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the items set in the 
     * LoadAdditionalConfiguraitonItems() method are set to the default
     * if the tag is missing in the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems_MissingTags(void);
    /**
     * Test that will verify the HandlePulse() method properly
     * processes the reload configuration pulse.
     */ 
    void TestHandlePulse(void);

    /** The path to the test configuration files */
    string m_configPath;

    /** Task id for the mock named data broker */
    int m_ndbId;
};
#endif
