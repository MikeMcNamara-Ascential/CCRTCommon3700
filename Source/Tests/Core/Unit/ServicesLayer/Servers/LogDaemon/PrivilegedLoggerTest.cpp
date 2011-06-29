#include "BogusLogWriter.h"
#include "PrivilegedLoggerTest.h"
#include "PrivilegedLogger.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

PrivilegedLoggerTest::PrivilegedLoggerTest()
{
}

PrivilegedLoggerTest::~PrivilegedLoggerTest()
{
}

void PrivilegedLoggerTest::setUp()
{
}

void PrivilegedLoggerTest::tearDown()
{
}


/**
 * See that the constructor for PrivilegedLogger sets the ivars to the
 * expected default values.
 */
void PrivilegedLoggerTest::TestConstructor()
{
	PrivilegedLogger privLog;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("LogWriter not initialized to null", static_cast<LogWriter*>(NULL), privLog.logWriter);
}
	
/**
 * See that Priv Log calls the process normal message on its log writer correctly.
 */
void PrivilegedLoggerTest::TestPrivLog()
{
	PrivilegedLogger privLog;
	BogusLogWriter bogusScribbles(NULL, NULL);
	
	privLog.SetLogWriter(&bogusScribbles);
	privLog.PrivLog("I am a message full of cheese..mmm cheese");
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("LogWriter's processNormalMessage not called",
			static_cast<int>(BogusLogWriter::PROCESS_NORMAL_MESSAGE), bogusScribbles.getLastCalled());
}

/**
 * See that SetLogWriter sets the log right duh..
 */
void PrivilegedLoggerTest::TestSetLogWriter()
{
	PrivilegedLogger privLog;
	BogusLogWriter bogusScribbles(NULL, NULL);
	
	privLog.SetLogWriter(&bogusScribbles);
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Log Writer not set correctly",
			static_cast<LogWriter*>(&bogusScribbles), privLog.logWriter);
}

/**
 * See that SetLogWriter sets the log right duh..
 */
void PrivilegedLoggerTest::TestSetProcessName()
{
	PrivilegedLogger privLog;
	BogusLogWriter bogusScribbles(NULL, NULL);
	
	privLog.SetProcessName("Milli Vanilli");
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Log Writer not set correctly",
			std::string("Milli Vanilli"), privLog.m_processName );
}


/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
