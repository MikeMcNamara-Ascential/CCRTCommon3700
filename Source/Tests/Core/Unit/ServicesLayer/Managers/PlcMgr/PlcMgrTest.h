#ifndef PlcMgrTest_h
#define PlcMgrTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include "XmlParser.h"

using namespace std;
class PlcMgr;

class PlcMgrTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PlcMgrTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestHandlePulse_ReloadPulse);
    CPPUNIT_TEST_SUITE_END();

public:
    PlcMgrTest();
    ~PlcMgrTest();

    void setUp();
    void tearDown();

    /**
     * Test that will verify the PlcMgr object is constructed properly
     */
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the PlcMgr configuration items
     * from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the HandlePulse() method properly
     * processes the reload configuration pulse.
     */
    void TestHandlePulse_ReloadPulse(void);

    /** The PlcMgr object under test */
    PlcMgr *plc;

    /** The path to the test configuration files */
    string m_configPath;

    /** Task id for the mock named data broker */
    int m_ndbId;

    /** Xml parser used to parse the configuration files */
    XmlParser parser;
};
#endif
