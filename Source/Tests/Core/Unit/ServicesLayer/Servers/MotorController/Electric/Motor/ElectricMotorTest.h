#ifndef ELECTRICMOTORTEST_H
#define ELECTRICMOTORTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "Motor.h"

using namespace std;

/**
 * Mock motor class that stubs out the pure virtual functions
 * in the Motor base class.  This allows us to create an object for
 * testing the Motor methods.
 */ 
class MockMotor: public Motor
{
public:
    MockMotor() : Motor()
    {
    }

    ~MockMotor()
    {
    }

    virtual const INT32 UploadDriveParameters(const XmlNode *parameters)
    {
        return(-1);
    }

    virtual const INT32 SendCommand(const std::string& message)
    {
        return(-1);
    }

    virtual const INT32 SendVariableCommand(const std::string message, 
                                            const std::string tag, 
                                            const INT32 readResponseDelay = 0)
    {
        return(-1);
    }

    virtual const INT32 Enable(const bool enable)
    {
        return(-1);
    }

    virtual const char* GetTorqueCommand(void)
    {
        return(NULL);
    }

    virtual const char* GetSpeedCommand(void)
    {
        return(NULL);
    }
};

class ElectricMotorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ElectricMotorTest);
	CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_NegativeCommFaultLimit);
	CPPUNIT_TEST_SUITE_END();

public:
	ElectricMotorTest();
	~ElectricMotorTest();
	
	void setUp();
	void tearDown();

	/**
     * Test that will verify the ElectricMotor object is 
     * constructed properly.
     */ 
	void TestConstructor(void);

    /**
     * Test that will verify the additional configuration items needed
     * by the motor class are loaded properly.
     */ 
    void TestLoadAdditionalConfigurationItems(void);

    /**
     * Test that will verify the comm fault limit is set to 0 (disabled)
     * if a negative comm fault limit is specified in the configuration 
     * file.
     */ 
    void TestLoadAdditionalConfigurationItems_NegativeCommFaultLimit(void);

    /** Pointer to the motor object under test */
    MockMotor *mot;

    /** The path to the test configuration files */
    string m_configPath;
};

#endif
