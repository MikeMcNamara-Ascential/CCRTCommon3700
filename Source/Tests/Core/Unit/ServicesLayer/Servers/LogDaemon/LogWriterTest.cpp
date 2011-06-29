#include "BogusLogFile.h"
#include "BogusLogWriter.h"
#include "BogusCoreLogFile.h"
#include "LogWriterTest.h"
#include "LogWriter.h"
#include "LogFileMap.h"
#include "LogMsgQueue.h"

#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

using namespace std;

LogWriterTest::LogWriterTest()
{
}

LogWriterTest::~LogWriterTest()
{
}

void LogWriterTest::setUp()
{
}

void LogWriterTest::tearDown()
{
	/* cleanup log files we may have created */
	system ("rm -f bogus1*");
}

/**
 * These tests trigger a bug in CPPUNIT when run with the other tests in this suite.
 * See the CVS log comment on version 1.5 of this file.
 */
void LogWriterTest::TestConstructor() {

	LogMsgQueue mqueue (42);
	LogFileMap fmap;

	LogWriter writer (&mqueue, &fmap);

	CPPUNIT_ASSERT_EQUAL_MESSAGE ("message queue not set properly", &mqueue,
			writer.queue);
	CPPUNIT_ASSERT_EQUAL_MESSAGE ("file map not set properly", &fmap,
			writer.fileMap);
}


/**
 * See that ProcessNormalMessage tries to write the correct
 * message to our bogus log file.
 */
void LogWriterTest::TestProcessNormalMessage() {

	LogFileMap logMap;
	BogusLogFile bogus1("bogus1");
	
	logMap.Put("bogus1", &bogus1);

	LogWriter logScribe(NULL, &logMap);
	LogMessage note(0, "now", "me", "Hello Universe", "bogus1");
	logScribe.ProcessNormalMessage(&note);

	char delim = note.GetDelimiter();
	string expected;

	expected += delim;
	expected += note.GetTimestamp();
	expected += " ";
	string who(note.GetWho());
	who.resize(30, ' ');
	expected += who;
	expected += " ";
	expected += note.GetMessage();
	expected += delim;

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect message written.",
			expected, bogus1.GetLine());
}

/**
 * See that TestActivateLogFile activates a log file, by
 * adding it to the map and making sure it's open.
 */
void LogWriterTest::TestActivateLogFile() {

	LogFileMap logMap;
	logMap.SetLogDir("./");

	LogMsgQueue Q(100);
	LogWriter logScribe(&Q, &logMap);

	LogMessage note (ROLL_ACTIVATE_SPECIAL_LOG, "now", "me", "5 5", "bogus1");

	int rv = logScribe.ActivateLogFile(&note);

	CPPUNIT_ASSERT_EQUAL_MESSAGE(string("activate 'bogus1' failed: ") +
			note.GetMessage(), 0, rv);
	CPPUNIT_ASSERT_MESSAGE(string("file 'bogus1' not added to map: ") +
			note.GetMessage(), NULL != logMap.Get("bogus1"));
}

/**
 * See that a given message is formatted in the proper way.
 */
void LogWriterTest::TestFormatMessage() {

	LogFileMap logMap;
	BogusLogFile bogus1("bogus1");
	logMap.Put("bogus1", &bogus1);

	LogWriter logScribe(NULL, &logMap);

	LogMessage note(0, "now", "me", "Hello Universe", "bogus1");
	char delim = note.GetDelimiter();
	string expected;

	expected += delim;
	expected += note.GetTimestamp();
	expected += " ";
	string who(note.GetWho());
	who.resize(30, ' ');
	expected += who;
	expected += " ";
	expected += note.GetMessage();
	expected += delim;

	string got;
	logScribe.FormatMessage(note, got);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect message formatted.",
			expected, got);
}

/**
 * See that the switch that routes the messages works properly.
 */
void LogWriterTest::TestCheckMessageSwitch() {
	LogMsgQueue mqueue (42);
	LogFileMap fmap;
	BogusLogWriter writer (&mqueue, &fmap);

	LogMessage normallog(ROLL_LOG_STRING, "now", "me", "log", "bogus1");
	writer.RouteMessage (&normallog);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("unexpected message routing for normal log message",
			static_cast<int>(BogusLogWriter::NONE), writer.getLastCalled());

	LogMessage activate(ROLL_ACTIVATE_SPECIAL_LOG, "now", "me", "log", "bogus1");
	writer.RouteMessage (&activate);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("unexpected message routing for activate log file",
			static_cast<int>(BogusLogWriter::ACTIVATE_LOG_FILE), writer.getLastCalled());

	LogMessage deactivate(ROLL_DEACTIVATE_SPECIAL_LOG, "now", "me", "log", "bogus1");
	writer.RouteMessage (&deactivate);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("unexpected message routing for deactivate log file",
			static_cast<int>(BogusLogWriter::DEACTIVATE_LOG_FILE), writer.getLastCalled());

	LogMessage inccore(ROLL_NEW_TEST_NUMBER, "now", "me", "log", "bogus1");
	writer.RouteMessage (&inccore);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("unexpected message routing for increment core log file",
			static_cast<int>(BogusLogWriter::INCREMENT_CORE_LOG_FILE), writer.getLastCalled());

	LogMessage invalid(0xffffffff, "now", "me", "log", "bogus1");
	writer.RouteMessage (&invalid);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("unexpected message routing for invalid message",
			static_cast<int>(BogusLogWriter::PROCESS_UNKNOWN_COMMAND), writer.getLastCalled());
}

/**
 * See that a log file is removed from the map when
 * DeActivateLogFile is called.
 */
void LogWriterTest::TestDeActivateLogFile() {
	
	LogFileMap loggyMap;
	BogusLogFile *bogus1 = new BogusLogFile("bogus1");
	loggyMap.Put("bogus1", bogus1);

	LogWriter loggyScribbles(NULL, &loggyMap);
	LogMessage note(ROLL_DEACTIVATE_SPECIAL_LOG, "now", "me", "Kill my file", "bogus1");
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("bogus1 was not in the map", static_cast<LogFile*>(bogus1), loggyMap.Get("bogus1"));
	
	loggyScribbles.DeActivateLogFile(&note);
	
	if ( NULL != loggyMap.Get("bogus1")) {
		delete(bogus1);
		CPPUNIT_FAIL("bogus1 was in the map");
	}
}

/**
 * See that an unknown command spits out the correct message, with error text added.
 */
void LogWriterTest::TestProcessUnknownCommand() {
	LogFileMap loggyMap;
	BogusLogFile *bogus1 = new BogusLogFile("bogus1");
	loggyMap.Put("bogus1", bogus1);

	LogWriter loggyScribbles(NULL, &loggyMap);

	// giving it a bogus command number
	LogMessage note(99999, "now", "me", "Kill my file", "bogus1");

	loggyScribbles.ProcessUnknownCommand(&note);
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Message was not properly formatted with error text",
			std::string("Unrecognized command [99999] with message (Kill my file)"),
			note.GetMessage());
}

void LogWriterTest::TestIncrementCoreLogFile() {
	LogFileMap loggyMap;
	BogusCoreLogFile bogus1("bogus1", "bogus");

	loggyMap.SetCoreLog(&bogus1);

	CPPUNIT_ASSERT_MESSAGE("CORE FILE NULL", loggyMap.GetCoreLog() != NULL);

	LogWriter loggyScribbles(NULL, &loggyMap);
	LogMessage note(0, "now", "me", "bogus1", "bogus1");
	
	loggyScribbles.IncrementCoreLogFile( &note );
	/* make sure that the increment function has been called on CoreLog */
	CPPUNIT_ASSERT_EQUAL_MESSAGE("CoreLog filename not incremented", 1 ,  bogus1.incrementFileNameCalled);
}
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
