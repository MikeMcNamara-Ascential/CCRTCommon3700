#ifndef BALDORMOTORCONTROLLERTEST_H
#define BALDORMOTORCONTROLLERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

class BaldorMotorController;

class BaldorMotorControllerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BaldorMotorControllerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST_SUITE_END();

public:
    BaldorMotorControllerTest();
    ~BaldorMotorControllerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the BaldorMotorController object is 
     * constructed properly.
     */ 
    void TestConstructor(void);

    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the BaldorMotorController configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);

    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly sets the BaldorMotorController configuration items
     * to their default if the tag is missing from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);

    /** Pointer to the baldor motor controller object under test */
    BaldorMotorController *bmc;

    /** The path to the test configuration files */
    string m_configPath;
};

#endif
