#include "LogDaemonTest.h"
#include "LogDaemon.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

LogDaemonTest::LogDaemonTest()
{
	origFtpRoot = getenv("FTP_ROOT");
}

LogDaemonTest::~LogDaemonTest()
{
}

void LogDaemonTest::setUp()
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

	// Tell the log daemon's static log file object about the file to log to
	string logDaemonFilePath = testDirPath + "/Logs/LogDaemon.out";
	LogDaemon::auxLog.SetFileName(logDaemonFilePath);
}

void LogDaemonTest::tearDown()
{
	setenv("FTP_ROOT", origFtpRoot.c_str(), 1);
}

/**
 * See that the constructor for LogDaemon sets the ivars to the
 * expected default values.
 */
void LogDaemonTest::TestConstructor()
{
	// default constructor
	LogDaemon logger;

	// examine default values
	CPPUNIT_ASSERT_EQUAL_MESSAGE("useDebugging true", false, logger.useDebugging);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logBasePath wrong", string("/Logs"), logger.logBasePath);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileName wrong", string("StartLog"),
			logger.startFileName);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileMaxSize wrong", 5, logger.startFileMaxSize);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileMaxFiles wrong", 10, logger.startFileMaxFiles);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileName wrong", string("TestLog"),
			logger.stdFileName);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileMaxSize wrong", 10, logger.stdFileMaxSize);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileMaxFiles wrong", 100, logger.stdFileMaxFiles);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("receiverPriority wrong", 10, logger.receiverPriority);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logWriterPriority wrong", 20, logger.logWriterPriority);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logRotatorPriority wrong", 10, logger.logRotatorPriority);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logRotatorPeriod wrong", 10, logger.logRotatorPeriod);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("msgQueueSize wrong", 1000, logger.msgQueueSize);
    CPPUNIT_ASSERT_EQUAL(string(""),logger.m_configFile);
}

/**
 * See that Initialize replaces default ivar vals with parsed values from
 * XML config document.
 */
void LogDaemonTest::TestInitialize()
{
	// get config document
	XmlParser parser;
	const XmlNode* document =
		parser.ReturnXMLDocument(testDirPath + "/ConfigFiles/LogDaemonInit.xml");

	LogDaemon logger;
	// send in the xml
	logger.Initialize(document);

	// check new ivar vals
	CPPUNIT_ASSERT_EQUAL_MESSAGE("useDebugging false", true, logger.useDebugging);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logBasePath wrong", string("/Other"), logger.logBasePath);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileName wrong", string("FileOne"), logger.startFileName);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileMaxSize wrong", 6, logger.startFileMaxSize);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileMaxFiles wrong", 11, logger.startFileMaxFiles);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileName wrong", string("FileTwo"),
			logger.stdFileName);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileMaxSize wrong", 3, logger.stdFileMaxSize);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileMaxFiles wrong", 5, logger.stdFileMaxFiles);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("receiverPriority wrong", 33, logger.receiverPriority);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logWriterPriority wrong", 66, logger.logWriterPriority);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logRotatorPriority wrong", 44, logger.logRotatorPriority);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logRotatorPeriod wrong", 55, logger.logRotatorPeriod);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("msgQueueSize wrong", 77, logger.msgQueueSize);

}

/**
 * See that bad values specified for options in the config file
 * cause Initialize() to fall back on defaults.
 */
void LogDaemonTest::TestInitialize_badVals()
{
		// get config document
	XmlParser parser;
	const XmlNode* document =
		parser.ReturnXMLDocument(testDirPath + "/ConfigFiles/LogDaemonInit_someBad.xml");

	LogDaemon logger;
	// send in the xml
	logger.Initialize(document);

	// These vals should have parsed correctly
	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileName wrong", string("MyFile1"),
			logger.startFileName);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileName wrong", string("MyFile2"),
			logger.stdFileName);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("logBasePath wrong",
			string("/MyPath"), logger.logBasePath);

	// These vals should be defaulted
	CPPUNIT_ASSERT_EQUAL_MESSAGE("useDebugging true", false, logger.useDebugging);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileMaxSize wrong", 5,
		logger.startFileMaxSize);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileMaxFiles wrong", 10,
		logger.startFileMaxFiles);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileMaxSize wrong", 10,
		logger.stdFileMaxSize);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileMaxFiles wrong", 100,
		logger.stdFileMaxFiles);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("receiverPriority wrong", 10,
		logger.receiverPriority);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("logWriterPriority wrong", 20,
		logger.logWriterPriority);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("logRotatorPriority wrong", 10,
		logger.logRotatorPriority);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("logRotatorPeriod wrong", 10,
		logger.logRotatorPeriod);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("msgQueueSize wrong", 1000,
		logger.msgQueueSize);

}

/**
 * See that missing options in the config file cause
 * Initialize() to use default values.
 */
void LogDaemonTest::TestInitialize_missingVals()
{
	// get config document
	XmlParser parser;
	const XmlNode* document =
		parser.ReturnXMLDocument(testDirPath + "/ConfigFiles/LogDaemonInit_missing.xml");

	LogDaemon logger;
	// send in the xml
	logger.Initialize(document);

	// examine default values
	CPPUNIT_ASSERT_EQUAL_MESSAGE("useDebugging true", false, logger.useDebugging);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logBasePath wrong", string("/Logs"), logger.logBasePath);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileName wrong", string("StartLog"),
			logger.startFileName);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileMaxSize wrong", 5, logger.startFileMaxSize);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("startFileMaxFiles wrong", 10, logger.startFileMaxFiles);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileName wrong", string("TestLog"),
			logger.stdFileName);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileMaxSize wrong", 10, logger.stdFileMaxSize);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("stdFileMaxFiles wrong", 100, logger.stdFileMaxFiles);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("receiverPriority wrong", 10, logger.receiverPriority);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logWriterPriority wrong", 20, logger.logWriterPriority);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logRotatorPriority wrong", 10, logger.logRotatorPriority);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("logRotatorPeriod wrong", 10, logger.logRotatorPeriod);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("msgQueueSize wrong", 1000, logger.msgQueueSize);

}

/** Ensure that the out method writes strings to the file */
void LogDaemonTest::TestOut()
{
	LogDaemon::auxLog.Open();

	// Set up the
	string ftpRoot = getenv("FTP_ROOT");
	string filePath = ftpRoot + "/Logs/LogDaemon.out";
	struct stat st;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("auxLog not found or could not stat()",
		0, stat(filePath.c_str(),  &st));

	unsigned long initialsize = st.st_blocks;

	LogDaemon::out("Hello world!");

	CPPUNIT_ASSERT_EQUAL_MESSAGE("could not stat() auxLog",
		0, stat(filePath.c_str(),  &st));
	unsigned long newsize = st.st_blocks;

	CPPUNIT_ASSERT_MESSAGE("did not write to auxLog", newsize > initialsize);
}


/** See that Run() opens auxLog and adds it to the LogFileMap */
void LogDaemonTest::TestRunInit()
{
	// Create our logger
	LogDaemon logger;
	// Call run with the unit test flag set to true, so it won't actually
	// go into the main loop.
	logger.RunInit();
	CPPUNIT_ASSERT_MESSAGE("RunInit didn't successfully open the auxLog",
		logger.auxLog.fileStream != NULL);
	CPPUNIT_ASSERT_MESSAGE("RunInit didnt' add the auxLog to the file map",
		logger.fileMap->Get(logger.auxFileName) != NULL);
}
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
