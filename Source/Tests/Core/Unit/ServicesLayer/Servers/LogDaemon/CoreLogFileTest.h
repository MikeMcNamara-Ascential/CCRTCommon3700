#ifndef CORELOGFILETEST_H
#define CORELOGFILETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

// Forward declare of the class you're testing
// Make sure that this class declares this test
// class as a "friend" class
class CoreLogFile;

class CoreLogFileTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CoreLogFileTest );
	CPPUNIT_TEST( TestConstructor );
	CPPUNIT_TEST( TestIncrementFileName );
	CPPUNIT_TEST( TestRotateIfNeeded_startlog );
	CPPUNIT_TEST( TestRotateIfNeeded_notstartlog );
	CPPUNIT_TEST( TestMarkOldestFile_ignores );
	CPPUNIT_TEST( TestLimitCoreLog );
	CPPUNIT_TEST( TestMarkOldestFile );
	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Constructor, generally sets the private ivars
	 * to 0L
	 */
	CoreLogFileTest();
	/**
	 * Destructor, delete's private ivars
	 */
	~CoreLogFileTest();
	
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
	void TestIncrementFileName();
	void TestRotateIfNeeded();
	void TestRotateIfNeeded_startlog();
	void TestRotateIfNeeded_notstartlog();
	void TestLimitCoreLog();
	void TestMarkOldestFile();
	void TestMarkOldestFile_ignores();
	
private:
	// Objects used during testing
	string testDirPath;
	string origFtpRoot;
};
#endif
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
