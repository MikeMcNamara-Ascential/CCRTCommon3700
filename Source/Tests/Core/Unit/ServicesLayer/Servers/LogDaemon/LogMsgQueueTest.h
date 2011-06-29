#ifndef LOGMSGQUEUETEST_H
#define LOGMSGQUEUETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>


using namespace std;

// Forward declare of the class you're testing
// Make sure that this class declares this test
// class as a "friend" class
class LogMsgQueue;

class LogMsgQueueTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( LogMsgQueueTest );
	
	CPPUNIT_TEST( TestLogMsgQueue_t );
	/* These two tests interact with other stuff somehow in a manner that
	 * causes deadlocks. We never figured out how. They execute fine in
	 * isolation but not together or with the other tests. I have them
	 * commented out so that work can proceed with the other tests */
	CPPUNIT_TEST( TestEnqueue_NotBlock );
	CPPUNIT_TEST( TestDequeue_Block );
	CPPUNIT_TEST( TestEnqueueDequeue );
	CPPUNIT_TEST( TestConstructor );

	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Constructor, generally sets the private ivars
	 * to 0L
	 */
	LogMsgQueueTest();
	/**
	 * Destructor, delete's private ivars
	 */
	~LogMsgQueueTest();
	
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
	void TestLogMsgQueue_t();
	void TestEnqueueDequeue();
	void TestDequeue_Block();
	void TestEnqueue_NotBlock();
	
	/** helpers for TestDequeue_block */
	static void *TestDequeue_Block_Producer (void *x);
	static void *TestDequeue_Block_Consumer (void *x);
	
	/** helpers for TestEnqueue_block */
	static void *TestEnqueue_Block_Producer (void *x);
	static void *TestEnqueue_Block_Checker (void *x);
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
