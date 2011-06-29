#ifndef PRIVILEGEDLOGGERTEST_H
#define PRIVILEGEDLOGGERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

// Forward declare of the class you're testing
// Make sure that this class declares this test
// class as a "friend" class
class PrivilegedLogger;

class PrivilegedLoggerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( PrivilegedLoggerTest );
	CPPUNIT_TEST( TestConstructor );
	CPPUNIT_TEST( TestPrivLog );
	CPPUNIT_TEST( TestSetLogWriter );
	CPPUNIT_TEST( TestSetProcessName );
	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Constructor, generally sets the private ivars
	 * to 0L
	 */
	PrivilegedLoggerTest();
	/**
	 * Destructor, delete's private ivars
	 */
	~PrivilegedLoggerTest();
	
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
	void TestPrivLog();
	void TestSetLogWriter();
	void TestSetProcessName();
	
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
