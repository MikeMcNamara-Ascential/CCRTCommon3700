#ifndef PlcDf1Test_h
#define PlcDf1Test_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class PlcDf1;

class PlcDf1Test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PlcDf1Test);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST_SUITE_END();

public:
    PlcDf1Test();
    ~PlcDf1Test();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the PlcDf1 object is constructed properly
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the PlcDf1 configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the items set in the 
     * LoadAdditionalConfiguraitonItems() method are set to the default
     * if the tag is missing in the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems_MissingTags(void);

    /** The PlcDf1 object under test */
    PlcDf1 *plc;

    /** The path to the test configuration files */
    string m_configPath;

    /** Task id for the mock named data broker */
    int m_ndbId;
};
#endif
