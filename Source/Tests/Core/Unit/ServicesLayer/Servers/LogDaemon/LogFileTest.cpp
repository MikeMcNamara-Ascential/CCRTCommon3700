#include "LogFileTest.h"
#include "LogFile.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

LogFileTest::LogFileTest()
	: baseTestLogName("testLog.tmp")
{
	origFtpRoot = getenv("FTP_ROOT");
}

LogFileTest::~LogFileTest()
{
}

void LogFileTest::setUp()
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

	// Delete all things in the log directory that start with our base log name
	string command = "rm -f " + testDirPath + "/Logs/" + baseTestLogName + "*";
	system(command.c_str());

	loggy = NULL;
}

void LogFileTest::tearDown()
{
// Delete all things in the log directory that start with our base log name
	string command = "rm -f " + testDirPath + "/Logs/" + baseTestLogName + "*";
	system(command.c_str());
}

/**
 * Test the basic functionality of the constructor
 */
void LogFileTest::TestConstructor()
{
	loggy = new LogFile("/bogusdir", "logfile.txt", 1, 5);

	/* tests contructor in normal situation */
	CPPUNIT_ASSERT_EQUAL_MESSAGE("LogDir not getting set properly",
		std::string("/bogusdir/"), loggy->logDir);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("fileName not getting set properly",
	std::string("/bogusdir/logfile.txt"), loggy->fileName);

	int type;
	pthread_mutexattr_gettype(&loggy->writeLockAtts, &type);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("mutex type not set properly", PTHREAD_MUTEX_RECURSIVE, type);

	delete loggy;

	loggy = new LogFile("/blah/blah", "////////////fileback.hjk", 1, 5);

	/* tests constructor with the wrong slashes */
	CPPUNIT_ASSERT_EQUAL_MESSAGE("LogDir not getting set properly",
		std::string("/blah/blah/"), loggy->logDir);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("fileName not getting set properly",
		std::string("/blah/blah/fileback.hjk"), loggy->fileName);

	delete loggy;
}

/**
 * See that RotateIfNeeded() does nothing to
 * the log file if no action is needed.  Ie,
 * max size not exceeded.
 */
void LogFileTest::TestRotateIfNeededNotNeeded() {

	/* give us a good logFile with a good file */
	loggy = new LogFile((testDirPath + "/Logs"), baseTestLogName, 1, 5);
	loggy->Open();
	loggy->RotateIfNeeded();

	/* ensure testLog.tmp is still here */
	struct stat fileBrowser;
	if ( stat((testDirPath + "/Logs/" + baseTestLogName).c_str(), &fileBrowser)) {
		CPPUNIT_FAIL(string("Could not find " + baseTestLogName + ": ")
			+ strerror(errno));
	}

	/* ensure testLog.tmp.0 is NOT here */
	if ( stat((testDirPath + "/Logs/" + baseTestLogName + ".0").c_str(), &fileBrowser)
		&& errno == ENOENT) {
		// good
	} else  {
		// bad tempLog was rotated!
		CPPUNIT_FAIL(baseTestLogName + ".0 was found, this is bad");
	}

	delete loggy;
}

/**
 * See that RotateIfNeeded() performs log rotation
 * if max size of target log file is exceeded.
 */
void LogFileTest::TestRotateIfNeededNeeded ()
{
	string logDir = testDirPath + "/Logs";
	/* open a log file and fill it so that it needs rotation */
	loggy = new LogFile(logDir, baseTestLogName, 1, 5);
	loggy->Open();

	for (int i=0; i<65536; i++)
	{
		loggy->WriteLine ("I am a happy line");
	}

	/* tell it to rotate */
	loggy->RotateIfNeeded();

	/* ensure testLog.tmp is still here and is empty */
	struct stat fileBrowser;
	if ( stat((logDir + "/" + baseTestLogName).c_str(), &fileBrowser)) {
		CPPUNIT_FAIL(string("Could not find testLog.tmp: ") + strerror(errno));
	}

	/* FIXME: Is this valid? Will all necessary filesystems have a zero-sized
	 * file return a size of zero? */
	CPPUNIT_ASSERT_EQUAL_MESSAGE(baseTestLogName + " size is not zero", 0,
		fileBrowser.st_size);

	/* Ensure that we can still write to the file even after being rotated, and
	 * that what we write gets to the file */
	loggy->WriteLine("Hello World!");
	loggy->Flush ();
	CPPUNIT_ASSERT_MESSAGE(baseTestLogName + " size is zero", 0 != loggy->Size());

	/* ensure testLog.tmp.0 is NOT here */
	if (stat((logDir + "/" + baseTestLogName + ".0").c_str(), &fileBrowser)) {
		CPPUNIT_FAIL(string("Could not find testLog.tmp.0: ") + strerror(errno));
	}

	delete loggy;
}

/**
 * See that WriteLine() writes a string to a log file
 */
void LogFileTest::TestWriteLine ()
{
	string logDir = testDirPath + "/Logs";

	loggy = new LogFile(logDir, baseTestLogName, 1, 5);
	loggy->Open ();

	CPPUNIT_ASSERT_EQUAL_MESSAGE("initial size not zero", 0L, loggy->Size());

	loggy->WriteLine("What is the meaning of to cheese?");
	loggy->Flush();
	size_t size = loggy->Size();
	CPPUNIT_ASSERT_MESSAGE("size did not increase after writing to log", 0 != loggy->Size());

	loggy->Cap ();

	loggy->WriteLine("What is the meaning of to cheese?");
	loggy->Flush();
    CPPUNIT_ASSERT_MESSAGE("size increased after writing to a capped log", size == (size_t)loggy->Size());

	delete loggy;
}

/**
 * See that Size() returns the correct size, in bytes, of the log file.
 */
void LogFileTest::TestSize ()
{
	string logDir = testDirPath + "/Logs";

	loggy = new LogFile(logDir, baseTestLogName, 1, 5);

	/* Output some data */
	loggy->Open();
	for (int i=0; i<256; i++)
	{
		loggy->WriteLine("I am the big monkey! The one true bovine Jeepster!");
	}
	loggy->Flush();
	loggy->Close();

	struct stat st;
	CPPUNIT_ASSERT_EQUAL_MESSAGE(string("stat() failed: ")+strerror(errno), 0,
			stat((logDir + "/" + baseTestLogName).c_str(), &st));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("size returned invalid value",
			static_cast<long>(st.st_size), loggy->Size());
	delete loggy;
}

/**
 * See that MaxSizeExceeded() returns true if the log file
 * is larger than is allowed, false if not.
 */
void LogFileTest::TestMaxSizeExceeded()
{
	string logDir = testDirPath + "/Logs";

	loggy = new LogFile(logDir, baseTestLogName, 1, 1);

	loggy->Open();

	CPPUNIT_ASSERT_MESSAGE("max size exceeded was true on a too small file",
			!loggy->MaxSizeExceeded());

	for (int i=0; i<131072; i++) {
		loggy->WriteLine("Hello world!");
	}

	loggy->Flush();

	CPPUNIT_ASSERT_MESSAGE("max size exceeded was false on a too big file",
			loggy->MaxSizeExceeded());
	delete loggy;
	loggy = NULL;
}


/**
 * See that Size() returns 0 and creates the file when run on a non-existant
 * file
 */
void LogFileTest::TestSize_NoFile()
{
	string logDir = testDirPath + "/Logs";

	LogFile loggy(logDir, "not_here", 5, 5);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Size wrong", 0L, loggy.Size());

	// ensure that the file was created
	struct stat myStat;
	CPPUNIT_ASSERT_MESSAGE("not_here not found",
		! stat((logDir + "/not_here").c_str(),&myStat));
}

/**
 * See that Rotate() renames all concerned files correctly,
 * not exceeding maximum file count.
 */
void LogFileTest::TestRotate()
{
	string logDir = testDirPath + "/Logs";

	LogFile loggy(logDir, "testrotate", 1, 3);

	// need to create a file (testlogs/testrotate)
	int fd;
	fd = open((logDir + "/testrotate").c_str(), O_CREAT); close(fd);
	fd = open((logDir + "/testrotate.0").c_str(), O_CREAT); close(fd);

	loggy.Rotate();

	struct stat myStat;
	// testlogs/testrotate.0 should exist!!
	CPPUNIT_ASSERT_MESSAGE("file .0 did not exist",
		! stat((logDir + "/testrotate.0").c_str(),&myStat));
	// testlogs/testrotate.1 should exist!!
	CPPUNIT_ASSERT_MESSAGE("file .1 did not exist",
		! stat((logDir + "/testrotate.1").c_str(),&myStat));
	// testlogs/testrotate.2 should not exist!!
	CPPUNIT_ASSERT_MESSAGE("file .2 did exist",
		stat((logDir + "/testrotate.2").c_str(),&myStat));

	// remove testlogs/testrotate*
	unlink((logDir + "/testrotate").c_str());
	unlink((logDir + "/testrotate.0").c_str());
	unlink((logDir + "/testrotate.1").c_str());
}

/**
 * See that Rotate() returns -1 if the target file is missing.
 */
// FIXME

/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
