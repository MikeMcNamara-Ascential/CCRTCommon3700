#include "LogDaemonTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "CoreLogFileTest.h"
#include "LogDaemonTest.h"
#include "LogFileIteratorTest.h"
#include "LogFileMapTest.h"
#include "LogFileTest.h"
#include "LogMessageTest.h"
#include "LogMsgQueueTest.h"
#include "LogRotatorTest.h"
#include "LogWriterTest.h"
#include "PrivilegedLoggerTest.h"

LogDaemonTestSuite::LogDaemonTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(CoreLogFileTest::suite());
	addTest(LogDaemonTest::suite());
	addTest(LogFileIteratorTest::suite());
	addTest(LogFileMapTest::suite());
	addTest(LogFileTest::suite());
	addTest(LogMessageTest::suite());
	addTest(LogMsgQueueTest::suite());
	addTest(LogRotatorTest::suite());
	addTest(LogWriterTest::suite());
	addTest(PrivilegedLoggerTest::suite());

}

/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */

