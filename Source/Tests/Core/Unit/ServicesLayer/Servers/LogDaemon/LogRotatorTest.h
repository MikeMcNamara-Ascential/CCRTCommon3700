#ifndef LOGROTATORTEST_H
#define LOGROTATORTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

// Forward declare of the class you're testing
// Make sure that this class declares this test
// class as a "friend" class
class LogRotator;

class LogRotatorTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( LogRotatorTest );
	CPPUNIT_TEST( TestConstructor );
	CPPUNIT_TEST( TestGetAdditionalLogFiles );
	CPPUNIT_TEST( TestRotateLogFiles );
	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Constructor, generally sets the private ivars
	 * to 0L
	 */
	LogRotatorTest();
	/**
	 * Destructor, delete's private ivars
	 */
	~LogRotatorTest();
	
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
	void TestConstructorDefaultPriority();
	void TestSetPeriod ();
	void TestGetAdditionalLogFiles ();
	void TestRotateLogFiles ();

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
