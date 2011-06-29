#ifndef LOGWRITERTEST_H
#define LOGWRITERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

// Forward declare of the class you're testing
// Make sure that this class declares this test
// class as a "friend" class
class LogWriter;

class LogWriterTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( LogWriterTest );
// These tests trigger a bug in CPPUNIT. See the comments on version 1.5 of this file.
	CPPUNIT_TEST( TestConstructor );
	//CPPUNIT_TEST( TestConstructorDefaultPriority );
	CPPUNIT_TEST( TestDeActivateLogFile );
	CPPUNIT_TEST( TestProcessNormalMessage );
	CPPUNIT_TEST( TestProcessUnknownCommand );
	CPPUNIT_TEST( TestIncrementCoreLogFile );
	CPPUNIT_TEST( TestFormatMessage );
	CPPUNIT_TEST( TestActivateLogFile );
	CPPUNIT_TEST( TestCheckMessageSwitch );
	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Constructor, generally sets the private ivars
	 * to 0L
	 */
	LogWriterTest();
	/**
	 * Destructor, delete's private ivars
	 */
	~LogWriterTest();
	
	/**
	 * Setup is run before each test, use it to
	 * set up the member ivars for use during
	 * the test
	 */
	void setUp();
	
	/**
	 * TearDown is called after each test, use it
	 * if you need to clean things up after your
	 * test
	 */
	void tearDown();

	// Put test methods below
	void TestConstructor();
	//void TestConstructorDefaultPriority();
	void TestActivateLogFile();
	void TestFormatMessage();
	void TestProcessNormalMessage();
	void TestProcessUnknownCommand();
	void TestIncrementCoreLogFile();
	void TestDeActivateLogFile();
	void TestCheckMessageSwitch();
	
private:
	// Objects used during testing
};
#endif
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
