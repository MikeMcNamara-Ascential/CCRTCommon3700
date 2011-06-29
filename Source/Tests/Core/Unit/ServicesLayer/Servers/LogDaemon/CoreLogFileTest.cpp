#include "CoreLogFileTest.h"
#include "CoreLogFile.h"
#include "BogusCoreLogFile2.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

CoreLogFileTest::CoreLogFileTest()
{
	origFtpRoot = getenv("FTP_ROOT");
}

CoreLogFileTest::~CoreLogFileTest()
{
}

void CoreLogFileTest::setUp()
{
	// Set up the path's we'll neeed
	if(getenv("TEST_DIR_PATH") == NULL)
	{
		cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory, ie: ~/ccrtserver/Tests" << endl;
		exit(1);
	}

	string topTestDir = getenv("TEST_DIR_PATH");

	testDirPath = topTestDir + "/Core/Unit/ServicesLayer/Servers/LogDaemon";
	// Set the FTP_ROOT environment path
	setenv("FTP_ROOT", testDirPath.c_str(), 1);

	string command = "rm -f " + testDirPath + "/Logs/testLog*";
	system(command.c_str());
}

void CoreLogFileTest::tearDown()
{
	// Restore the original FTP_ROOT
	setenv("FTP_ROOT", origFtpRoot.c_str(), 1);

	string command = "rm -f " + testDirPath + "/Logs/testLog*";
	system(command.c_str());
}

/**
 * See that the constructor for CoreLogFile sets the ivars to the
 * expected default values.
 */
void CoreLogFileTest::TestConstructor()
{
	// create a core log file
	string logDir = testDirPath + "/Logs";
	CoreLogFile corey(logDir,"testLog", "testLog", 4, 4, 4, 4);

	// assert that everything is set correctly
	CPPUNIT_ASSERT_EQUAL_MESSAGE("basename set incorrectly", std::string("testLog"), corey.baseName);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("usingStartLog set incorrectly", true, corey.usingStartLog);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("capSize set incorrectly",static_cast<UINT32>(4), corey.capSize);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("maxTestLogFiles set incorrectly", static_cast<UINT32>(4), corey.maxTestLogFiles);
}

/**
 * See that the file name gets incremented properly.
 * See also that the new file is opened.
 * See also that LimitCoreLogs is called.
 */
void CoreLogFileTest::TestIncrementFileName()
{
	// create a core log file
	string logDir = testDirPath + "/Logs";
	BogusCoreLogFile2 corey(logDir, "testLog.", "testLog.");
	corey.IncrementFileName("tmp");

	// assert that everything is set correctly
	CPPUNIT_ASSERT_EQUAL_MESSAGE("filename set incorrectly",
		std::string(logDir + "/testLog.tmp"), corey.fileName);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("open not called", 1, corey.opencalled);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("close not called", 1, corey.closecalled);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("limitcorelog not called", 1, corey.lclcalled);
}

/**
 * See that MarkOldestFile marks the oldest CoreLogEntry in the given array for deletion.
 */
void CoreLogFileTest::TestMarkOldestFile()
{
	// create test core log file
	string logDir = testDirPath + "/Logs";
	CoreLogFile corey(logDir,"testLog.", "testLog.", 4, 4, 4, 4);

	time_t now(time(NULL));

	// create test CoreFileEntry data
	CoreFileEntry newest = {
			logDir + "/testLog.0", // name
			now, // time
			false
		};

	CoreFileEntry old = {
			logDir + "/testLog.1", // name
			now - 10, // time
			false
		};

	CoreFileEntry oldest = {
			logDir + "/testLog.2", // name
			now - 20, // time
			false
		};

	CoreFileEntry *entries[] = {
		&newest,
		&old,
		&oldest
	};

	corey.MarkOldestFile(entries, 3);

	CPPUNIT_ASSERT_MESSAGE("newest removed", newest.removed == false);
	CPPUNIT_ASSERT_MESSAGE("old removed", old.removed == false);
	CPPUNIT_ASSERT_MESSAGE("oldest not removed", oldest.removed == true);
}

/**
 * See that MarkOldestFile marks the oldest CoreLogEntry, ignoring any previously marked files.
 */
void CoreLogFileTest::TestMarkOldestFile_ignores()
{
	// create test core log file
	string logDir = testDirPath + "/Logs";
	CoreLogFile corey(logDir,"testLog.", "testLog.", 4, 4, 4, 4);

	time_t now(time(NULL));

	// create test CoreFileEntry data
	CoreFileEntry newest = {
			logDir + "/testLog.0", // name
			now, // time
			false
		};

	CoreFileEntry old = {
			logDir + "/testLog.1", // name
			now - 10, // time
			false
		};

	CoreFileEntry oldest = {
			logDir + "/testLog.2", // name
			now - 20, // time
			true
		};

	CoreFileEntry *entries[] = {
		&newest,
		&old,
		&oldest
	};

	corey.MarkOldestFile(entries, 3);

	CPPUNIT_ASSERT_MESSAGE("newest removed", newest.removed == false);
	CPPUNIT_ASSERT_MESSAGE("old not removed", old.removed == true);
	CPPUNIT_ASSERT_MESSAGE("oldest not removed", oldest.removed == true);
}

/**
 * See that LimitCoreLog removes the oldest log files in the directory. The number of files to
 * delete is dependant on the allowable number of files.
 */
void CoreLogFileTest::TestLimitCoreLog()
{
	// Create some test files with different dates
	int fileHandle;
	string logDir = testDirPath + "/Logs";
	unlink((logDir + "/testLog.3").c_str());
	fileHandle = open((logDir +"/testLog.3").c_str(), O_CREAT);
	sleep(1);
	close(fileHandle);
	unlink((logDir + "/testLog.2").c_str());
	fileHandle = open((logDir + "/testLog.2").c_str(), O_CREAT);
	sleep(1);
	close(fileHandle);
	unlink((logDir + "/testLog.1").c_str());
	fileHandle = open((logDir + "/testLog.1").c_str(), O_CREAT);
	sleep(1);
	close(fileHandle);
	unlink((logDir + "/testLog.0").c_str());
	fileHandle = open((logDir + "/testLog.0").c_str(), O_CREAT);
	close(fileHandle);

	CoreLogFile corey(logDir, "testLog.", "testLog.", 4, 4, 4, 3);
	corey.LimitCoreLog();

	struct stat st;
	if (stat((logDir + "/testLog.2").c_str(), &st) != 0)
		CPPUNIT_FAIL("testLog.2 not present");
	if (stat((logDir + "/testLog.1").c_str(), &st) != 0)
		CPPUNIT_FAIL("testLog.1 not present");
	if (stat((logDir + "/testLog.0").c_str(), &st) != 0)
		CPPUNIT_FAIL("testLog.0 not present");
	if (stat((logDir + "/testLog.3").c_str(), &st) == 0 || errno != ENOENT)
		CPPUNIT_FAIL("testLog.3 still present");

}


/**
 * See that RotateIfNeeded:
 *  - if in startlog mode it rotates the logs as they exceed their max size
 */
void CoreLogFileTest::TestRotateIfNeeded_startlog() {
	/* give us a good core logFile with a good file */
	string logDir = testDirPath + "/Logs";
	CoreLogFile *loggy = new CoreLogFile(logDir, "testLog.tmp", "testLog", 1, 3, 3, 3);
	loggy->Open();

	// fill a file with some stuff
	for (int i=0; i<65536; i++)
	{
		loggy->WriteLine ("I am a happy line");
	}

	loggy->RotateIfNeeded();

	/* ensure testLog.tmp is still here */
	struct stat fileBrowser;
	if ( stat((logDir + "/testLog.tmp").c_str(), &fileBrowser)) {
		CPPUNIT_FAIL(string("Could not find testLog.tmp: ") + strerror(errno));
	}

	/* ensure testLog.tmp.0 is here */
	if ( stat((logDir + "/testLog.tmp.0").c_str(), &fileBrowser) && errno == ENOENT) {
		// bad tempLog was not rotated!
		CPPUNIT_FAIL("testLog.tmp.0 was not found, this is bad");
	} else  {
		// good
	}

	delete loggy;

}

/**
 * See that RotateIfNeeded:
 *  - if not in startlog mode, the current testlog gets capped if max size is exceeded.
 */
void CoreLogFileTest::TestRotateIfNeeded_notstartlog() {
	/* give us a good core logFile with a good file */
	string logDir = testDirPath + "/Logs";
	CoreLogFile *loggy = new CoreLogFile(logDir, "testLog.tmp", "testLog", 1, 3, 1, 3);
	loggy->Open();

	loggy->usingStartLog = false;

	// fill a file with some stuff
	for (int i=0; i<65536; i++)
	{
		loggy->WriteLine ("I am a happy line");
	}

	loggy->RotateIfNeeded();

	/* ensure testLog.tmp is still here */
	struct stat fileBrowser;
	if ( stat((logDir + "/testLog.tmp").c_str(), &fileBrowser)) {
		CPPUNIT_FAIL(string("Could not find testLog.tmp: ") + strerror(errno));
	}

	/* ensure testLog.tmp.0 is not here */
	if ( stat((logDir + "/testLog.tmp.0").c_str(), &fileBrowser) && errno == ENOENT) {
		// good
	} else  {
		// bad tempLog was rotated!
		CPPUNIT_FAIL("testLog.tmp.0 was found, this is bad");
	}

	CPPUNIT_ASSERT_MESSAGE("not capped", loggy->isCapped);

	delete loggy;
}
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
