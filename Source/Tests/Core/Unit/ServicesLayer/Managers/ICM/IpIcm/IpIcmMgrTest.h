#ifndef IpIcmMgrTest_h
#define IpIcmMgrTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class IpIcmMgr;

class IpIcmMgrTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(IpIcmMgrTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestReloadConfiguration);
    CPPUNIT_TEST(TestReloadConfiguration_MissingTags);
    CPPUNIT_TEST_SUITE_END();

public:
    IpIcmMgrTest();
    ~IpIcmMgrTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the IpIcmMgr object is constructed properly
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the ReloadConfiguration() method 
     * properly reads the IpIcmMgr configuration items from the 
     * configuration file.
     */
    void TestReloadConfiguration(void);
    /**
     * Test that will verify the items set in the ReloadConfiguration()
     * method are set to the default if the tag is missing in the configuration 
     * file.
     */
    void TestReloadConfiguration_MissingTags(void);

    /** The IpIcmMgr object under test */
    IpIcmMgr *iim;

    /** The path to the test configuration files */
    string m_configPath;

    /** The original FTP_ROOT.  This will be restored after we are done with
     * our tests. */
    string m_origFtpRoot;

    /** The FTP_ROOT to use during testing. */
    string m_newFtpRoot;
};
#endif
