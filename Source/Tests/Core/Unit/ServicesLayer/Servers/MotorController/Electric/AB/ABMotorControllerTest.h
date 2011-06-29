#ifndef ABMOTORCONTROLLERTEST_H
#define ABMOTORCONTROLLERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

class ABMotorController;

class ABMotorControllerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ABMotorControllerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST_SUITE_END();

public:
    ABMotorControllerTest();
    ~ABMotorControllerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the ABMotorController object is 
     * constructed properly.
     */ 
    void TestConstructor(void);

    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the ABMotorController configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);

    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly sets the ABMotorController configuration items
     * to their default if the tag is missing from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);

    /** Pointer to the ab motor controller object under test */
    ABMotorController *amc;

    /** The path to the test configuration files */
    string m_configPath;
};

#endif
