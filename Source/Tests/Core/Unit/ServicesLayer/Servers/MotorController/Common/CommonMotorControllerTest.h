#ifndef COMMONMOTORCONTROLLERTEST_H
#define COMMONMOTORCONTROLLERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "MotorController.h"

using namespace std;

/**
 * Mock motor controller class that stubs out the pure virtual functions
 * in the MotorController base class.  This allows us to create an object for
 * testing the MotorController methods.
 */ 
class MockMotorController: public MotorController
{
public:
    MockMotorController() : MotorController()
    {
    }
    ~MockMotorController()
    {
    }
    virtual const INT32 Boost(void)
    {
        return(-1);
    }
    virtual const INT32 Speed(void)
    {
        return(-1);
    }
    virtual const INT32 Torque(void)
    {
        return(-1);
    }
    virtual const INT32 LossCompensation(void)
    {
        return(-1);
    }
    virtual const INT32 ApplyMotorLoad( float motorLoad, int masterRoll)
    {
        return(-1);
    }
};

class CommonMotorControllerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CommonMotorControllerTest);
	CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(TestSetupMotorLoading);
    CPPUNIT_TEST(TestSetupMotorLoading_MissingTags);
	CPPUNIT_TEST_SUITE_END();

public:
	CommonMotorControllerTest();
	~CommonMotorControllerTest();
	
	void setUp();
	void tearDown();

	/**
     * Test that will verify the CommonMotorController object is 
     * constructed properly.
     */ 
	void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the MotorController configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly loads the defaults when the tags are missing
     * from the configuration file.
     */
    void TestLoadAdditionalConfigurationItems_MissingTags(void);
    /**
     * Test that will verify the SetupMotorLoading() method properly reads
     * the Motor Loading configuration items from the configuration file.
     */ 
    void TestSetupMotorLoading(void);
    /**
     * Test that will verify the SetupMotorLoading() method properly loads the 
     * defaults when the tags are missing from the configuration file.
     */
    void TestSetupMotorLoading_MissingTags(void);

    /** Pointer to the motor controller object under test */
    MockMotorController *mmc;

    /** The path to the test configuration files */
    string m_configPath;
};

#endif
