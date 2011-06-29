#ifndef PendantManagerTest_h
#define PendantManagerTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class PendantManager;

class PendantManagerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PendantManagerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST_SUITE_END();

public:
    PendantManagerTest();
    ~PendantManagerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the PendantManager object is constructed 
     * properly.
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the PendantManager configuration items
     * from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems(void);

    /** The PendantManager object under test */
    PendantManager *pm;

    /** The path to the test configuration files */
    string m_configPath;
};
#endif
