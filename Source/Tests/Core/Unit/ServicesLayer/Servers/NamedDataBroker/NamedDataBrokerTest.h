#ifndef NamedDataBrokerTest_h
#define NamedDataBrokerTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class NamedDataBroker;

class NamedDataBrokerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(NamedDataBrokerTest);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST_SUITE_END();

public:
    NamedDataBrokerTest();
    ~NamedDataBrokerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the NamedDataBroker configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);

    /** The NamedDataBroker object under test */
    NamedDataBroker *ndb;

    /** The path to the test configuration files */
    string m_configPath;
};
#endif
