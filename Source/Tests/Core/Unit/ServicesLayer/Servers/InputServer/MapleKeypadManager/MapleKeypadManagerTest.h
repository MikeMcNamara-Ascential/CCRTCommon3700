#ifndef MapleKeypadManagerTest_h
#define MapleKeypadManagerTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class MapleKeypadManager;

class MapleKeypadManagerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MapleKeypadManagerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST_SUITE_END();

public:
    MapleKeypadManagerTest();
    ~MapleKeypadManagerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the MapleKeypadManager object is constructed 
     * properly.
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the MapleKeypadManager configuration items
     * from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the items set in the 
     * LoadAdditionalConfigurationItems() method are set to the default
     * if the tag is missing in the configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);

    /** The MapleKeypadManager object under test */
    MapleKeypadManager *mkm;

    /** The path to the test configuration files */
    string m_configPath;
};
#endif
