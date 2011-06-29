#ifndef LOGDAEMONTEST_H
#define LOGDAEMONTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

// Forward declare of the class you're testing
// Make sure that this class declares this test
// class as a "friend" class
class LogDaemon;

class LogDaemonTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( LogDaemonTest );
	CPPUNIT_TEST( TestConstructor );
	CPPUNIT_TEST( TestInitialize );
	CPPUNIT_TEST( TestInitialize_badVals );
	CPPUNIT_TEST( TestInitialize_missingVals );
	CPPUNIT_TEST( TestOut );
	CPPUNIT_TEST( TestRunInit );
	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Constructor, generally sets the private ivars
	 * to 0L
	 */
	LogDaemonTest();
	/**
	 * Destructor, delete's private ivars
	 */
	~LogDaemonTest();
	
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
	void TestInitialize ();
	void TestInitialize_badVals ();
	void TestInitialize_missingVals ();
	void TestOut ();
	void TestRunInit();
	
private:
	// Objects used during testing
	string origFtpRoot;
	string testDirPath;
};
#endif
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
