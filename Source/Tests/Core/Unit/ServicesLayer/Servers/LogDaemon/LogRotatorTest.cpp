#include <algorithm>
#include "BogusLogFile.h"
#include "BogusNdb.h"
#include "LogRotatorTest.h"
#include "LogRotator.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

LogRotatorTest::LogRotatorTest()
{
}

LogRotatorTest::~LogRotatorTest()
{
}

void LogRotatorTest::setUp()
{
}

void LogRotatorTest::tearDown()
{
}

/**
 * See that the constructor works:
 *  - sleepTime = 10
 *  - fileMap = fm*
 *  - threadPriority = priority
 */
void LogRotatorTest::TestConstructor ()
{
	LogFileMap map;
	LogRotator rot(&map);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("map wrong", &map, rot.logFileMap);

}


/**
 * See that...
 */
void LogRotatorTest::TestGetAdditionalLogFiles ()
{
	LogFileMap filemap;
	LogRotator rotor(&filemap);

	BogusLogFile file1("bogus01");
	BogusLogFile file2("bogus02");
	BogusLogFile file3("bogus03");

	filemap.Put("bogus01", &file1);
	filemap.Put("bogus02", &file2);
	filemap.Put("bogus03", &file3);

	std::vector<LogFile*> logfiles;

	rotor.GetAdditionalLogFiles(logfiles);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("could not find bogus01",
			static_cast<LogFile*>(&file1), *find(logfiles.begin(),
				logfiles.end(), &file1));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("could not find bogus02",
			static_cast<LogFile*>(&file2), *find(logfiles.begin(),
				logfiles.end(), &file2));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("could not find bogus03",
			static_cast<LogFile*>(&file3), *find(logfiles.begin(),
				logfiles.end(), &file3));
}

/**
 * See that...
 */
void LogRotatorTest::TestRotateLogFiles ()
{
#if 0
	LogFileMap filemap;
	LogRotator rotor(&filemap);
	BogusNdb ndb;

	BogusLogFile file1("bogus01");
	BogusLogFile file2("bogus02");
	BogusLogFile file3("bogus03");

	filemap.Put("bogus01", &file1);
	filemap.Put("bogus02", &file2);
	filemap.Put("bogus03", &file3);

	std::vector<LogFile*> logfiles;
	rotor.GetAdditionalLogFiles(logfiles);

	// need a namedDataBroker running before making this call

//	rotor.RotateLogFiles(logfiles);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("bogus01 was not rotated", true, file1.wasRotated());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("bogus02 was not rotated", true, file2.wasRotated());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("bogus03 was not rotated", true, file3.wasRotated());
#endif
}

/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
