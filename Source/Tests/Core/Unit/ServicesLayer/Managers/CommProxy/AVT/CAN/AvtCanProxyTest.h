#ifndef AvtCanProxyTest_h
#define AvtCanProxyTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class AvtCanProxy;

class AvtCanProxyTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(AvtCanProxyTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestReadPortConfig);
    CPPUNIT_TEST(TestReadPortConfig_MissingTags);
    CPPUNIT_TEST_SUITE_END();

public:
    AvtCanProxyTest();
    ~AvtCanProxyTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the RawCommProxy object is constructed properly
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the ReadPortConfig() method properly reads the 
     * AvtCanProxy configuration items from the configuration file.
     */
    void TestReadPortConfig(void);
    /**
     * Test that will verify the ReadPortConfig() method properly sets the
     * configuration items to their default if the tag is missing from the
     * configuration file.
     */ 
    void TestReadPortConfig_MissingTags(void);

    /** The AvtCanCommProxy object under test */
    AvtCanProxy *accp;

    /** The path to the test configuration files */
    string m_configPath;
};

#endif
