#include "LogMessageTest.h"
#include "LogMessage.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

LogMessageTest::LogMessageTest()
{
}

LogMessageTest::~LogMessageTest()
{
}

void LogMessageTest::setUp()
{
}

void LogMessageTest::tearDown()
{
}

/**
 * See that the constructor for LogMessage sets the ivars to the
 * expected default values.
 */
void LogMessageTest::TestConstructor()
{
	LogMessage message1;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("_command not initialized properly",
			static_cast<UINT32>(0), message1._command);

	LogMessage message2(42, "timestamp", "me", "message", "file");
	CPPUNIT_ASSERT_EQUAL_MESSAGE("_command not initialized properly",
			static_cast<UINT32>(42), message2._command);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("_timestamp not initialized properly", std::string("timestamp"),
			message2._timestamp);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("_who not initialized properly", std::string("me"), message2._who);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("_message not initialized properly", std::string("message"), message2._message);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("_file not initialized properly", std::string("file"), message2._file);
}

/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
