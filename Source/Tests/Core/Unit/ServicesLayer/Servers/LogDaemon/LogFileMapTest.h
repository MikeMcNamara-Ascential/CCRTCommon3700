#ifndef LOGFILEMAPTEST_H
#define LOGFILEMAPTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

// Forward declare of the class you're testing
// Make sure that this class declares this test
// class as a "friend" class
class LogFileMap;

class LogFileMapTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( LogFileMapTest );
	CPPUNIT_TEST( TestConstructor );
	CPPUNIT_TEST( TestPut );
	CPPUNIT_TEST( TestGet );
	CPPUNIT_TEST( TestGetCoreLog );
	CPPUNIT_TEST( TestSetCoreLog );
	CPPUNIT_TEST( TestIterator );
	CPPUNIT_TEST( TestRemove );
	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Constructor, generally sets the private ivars
	 * to 0L
	 */
	LogFileMapTest();
	/**
	 * Destructor, delete's private ivars
	 */
	~LogFileMapTest();
	
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
	void TestPut();
	void TestGet();
	void TestGetCoreLog();
	void TestSetCoreLog();
	void TestIterator();
	void TestRemove();

private:
	// Objects used during testing
	LogFileMap *fileMap;
};
#endif
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
