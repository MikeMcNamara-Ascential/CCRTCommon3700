#ifndef PlantHostInboundTest_h
#define PlantHostInboundTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class PlantHostInbound;

class PlantHostInboundTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PlantHostInboundTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(TestLoadVehicleBuildFromFile);
    CPPUNIT_TEST(TestTranslateBuildRecord_PerformAbortTestWithChild);
    CPPUNIT_TEST(TestTranslateBuildRecord_PerformAbortTestNoChild);
    CPPUNIT_TEST(TestTranslateBuildRecord_WithTranslation);
    CPPUNIT_TEST(TestTranslateBuildRecord_TranslationMissing);
    CPPUNIT_TEST(TestTranslateBuildRecord_InvalidStartIndex);
    CPPUNIT_TEST(TestTranslateBuildRecord_ExceptionThrown);
    CPPUNIT_TEST(TestTranslateBuildRecordHook);
    CPPUNIT_TEST(GetAbortPrompt);
    CPPUNIT_TEST(GetAbortColor);
    CPPUNIT_TEST(GetBlockPrompt);
    CPPUNIT_TEST(GetShipPrompt);
    CPPUNIT_TEST(GetShipStatusColor);
    CPPUNIT_TEST_SUITE_END();

public:
    PlantHostInboundTest();
    ~PlantHostInboundTest();

    void setUp();
    void tearDown();

    /**
     * Test that verifies the plant host inbound object is constructed
     * properly
     */
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionalConfigurationItems()
     * method properly reads the PlantHostInbound configuration items
     * from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the LoadAdditionalConfigurationItems() method
     * properly loads the default values if the tags are missing in the
     * configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);
    /**
     * Test that will verify the HandlePulse() method properly processes the
     * reload configuration pulse.
     */
    void TestHandlePulse(void);
    /**
     * Test that will verify the LoadVehicleBuildFromFile() properly reads in
     * the specfied vehicle build record.
     */
    void TestLoadVehicleBuildFromFile(void);
    /**
     * Test that will verify TranslateBuildRecord() works as planned when the
     * tag is "PerformAbortTest" and a child value is specified.
     */ 
    void TestTranslateBuildRecord_PerformAbortTestWithChild(void);
    /**
     * Test that will verify TranslateBuildRecord() works as planned when the
     * tag is "PerformAbortTest" and no child value is specified.
     */
    void TestTranslateBuildRecord_PerformAbortTestNoChild(void);
    /**
     * Test that will verify TranslateBuildRecord() works as planned when the
     * tag is not "PerformAbortTest" and a translation value is specified.
     */
    void TestTranslateBuildRecord_WithTranslation(void);
    /**
     * Test that will verify TranslateBuildRecord() works as planned when the
     * tag is not "PerformAbortTest" and no translation value is specified.
     */ 
    void TestTranslateBuildRecord_TranslationMissing(void);
    /**
     * Test that will verify TranslateBuildRecord() works as planned when the
     * start index is invalid.
     */ 
    void TestTranslateBuildRecord_InvalidStartIndex(void);
    /**
     * Test that will verify TranslateBuildRecord() works as planned when an
     * exception is thrown.
     */ 
    void TestTranslateBuildRecord_ExceptionThrown(void);
    /**
     * Test that will verify TranslateBuildRecordHook() works as planned.
     */ 
    void TestTranslateBuildRecordHook(void);
    /**
     * Test that will verify GetAbortPrompt() properly returns the value of the
     * member variable that holds the abort prompt.
     */ 
    void GetAbortPrompt(void);
    /**
     * Test that will verify GetAbortColor() properly returns the value of the
     * member variable that holds the abort prompt color.
     */ 
    void GetAbortColor(void);
    /**
     * Test that will verify GetBlockPrompt() properly returns the value of the
     * member variable that holds the block prompt.
     */ 
    void GetBlockPrompt(void);
    /**
     * Test that will verify GetShipPrompt() properly returns the value of the
     * member variable that holds the ship prompt.
     */ 
    void GetShipPrompt(void);
    /**
     * Test that will verify GetShipStatusColor() properly returns the value of
     * the member variable that holds the ship status prompt color.
     */ 
    void GetShipStatusColor(void);

    /** The PlantHostInbound object under test */
    PlantHostInbound *phi;

    /** The path to the test configuration files */
    string m_configPath;

    /** The original FTP_ROOT.  This will be restored after we are done with
     * our tests. */
    string m_origFtpRoot;

    /** The FTP_ROOT to use during testing. */
    string m_newFtpRoot;
};
#endif
