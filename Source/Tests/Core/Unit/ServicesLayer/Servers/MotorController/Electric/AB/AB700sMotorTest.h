#ifndef AB700SMOTORTEST_H
#define AB700SMOTORTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

class AB700sMotor;

class AB700sMotorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(AB700sMotorTest);
	CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
	CPPUNIT_TEST_SUITE_END();

public:
	AB700sMotorTest();
	~AB700sMotorTest();
	
	void setUp();
	void tearDown();

	/**
     * Test that will verify the AB700sMotor object is constructed properly.
     */ 
	void TestConstructor(void);

    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the AB700sMotor configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);

    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly loads the default AB700sMotor configuration items
     * if the tag is missing from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);

    /** Pointer to the ab700s motor controller object under test */
    AB700sMotor *am;

    /** The path to the test configuration files */
    string m_configPath;
};

#endif
