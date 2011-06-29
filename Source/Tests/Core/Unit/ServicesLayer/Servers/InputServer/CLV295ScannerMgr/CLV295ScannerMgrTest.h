#ifndef CLV295ScannerMgrTest_h
#define CLV295ScannerMgrTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class CLV295ScannerMgr;

class CLV295ScannerMgrTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CLV295ScannerMgrTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST_SUITE_END();

public:
    CLV295ScannerMgrTest();
    ~CLV295ScannerMgrTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the CLV295ScannerMgr object is constructed 
     * properly.
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the CLV295ScannerMgr configuration items
     * from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the items set in the 
     * LoadAdditionalConfigurationItems() method are set to the default
     * if the tag is missing in the configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);

    /** The CLV295ScannerMgr object under test */
    CLV295ScannerMgr *csm;

    /** The path to the test configuration files */
    string m_configPath;
};
#endif
