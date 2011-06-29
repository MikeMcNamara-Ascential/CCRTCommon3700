#ifndef LOGFILETEST_H
#define LOGFILETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;

// Forward declare of the class you're testing
// Make sure that this class declares this test
// class as a "friend" class
class LogFile;

class LogFileTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( LogFileTest );
	CPPUNIT_TEST( TestConstructor );
	CPPUNIT_TEST( TestSize );
	CPPUNIT_TEST( TestRotateIfNeededNeeded );
	CPPUNIT_TEST( TestRotateIfNeededNotNeeded );
	CPPUNIT_TEST( TestWriteLine );
	CPPUNIT_TEST( TestMaxSizeExceeded );
	CPPUNIT_TEST( TestSize_NoFile );
	CPPUNIT_TEST( TestRotate );
	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Constructor, generally sets the private ivars
	 * to 0L
	 */
	LogFileTest();
	/**
	 * Destructor, delete's private ivars
	 */
	~LogFileTest();
	
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
	
	/**
	 * Test the constructor
	 */
	void TestConstructor();
	void TestSize ();
	void TestRotateIfNeededNotNeeded();
	void TestRotateIfNeededNeeded();
	void TestWriteLine();
	void TestMaxSizeExceeded();
	void TestSize_NoFile();
	void TestRotate();

private:
	// Objects used during testing
	LogFile* loggy;
	const std::string baseTestLogName;
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
