#include "BogusCoreLogFile.h"
#include "BogusLogFile.h"
#include <memory>
#include "LogFileMapTest.h"
#include "LogFileMap.h"
#include "LogFile.h"
#include <pthread.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace std;

LogFileMapTest::LogFileMapTest()
{
}

LogFileMapTest::~LogFileMapTest()
{
}

void LogFileMapTest::setUp()
{
	fileMap = new LogFileMap ();
}

void LogFileMapTest::tearDown()
{
	delete fileMap;
}

/** Make sure that the the mutex is initialized properly */
void LogFileMapTest::TestConstructor()
{
	int type;

	pthread_mutexattr_gettype(&fileMap->lockAtts, &type);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Mutex type not error check", PTHREAD_MUTEX_ERRORCHECK, type);
}

/* Make sure that put adds a file to the map properly */
void LogFileMapTest::TestPut()
{
	/* we use an auto_ptr to ensure that the memory is deallocated no matter
	 * how we get out of this function */
	auto_ptr <LogFile> loggy(new LogFile("/bogusdir/", "logfile.txt", 1, 5));

	/* make sure the file is not in the map initially */
	CPPUNIT_ASSERT_EQUAL_MESSAGE("'logfile.txt' entry in file map is non-null",
		static_cast<LogFile*>(NULL), fileMap->fileMap["logfile.txt"]);

	/* now add it and make sure it's there after we stick it there */
	fileMap->Put ("logfile.txt", loggy.get());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("'logfile.txt' entry in file map is null after"
		" its addition", loggy.get(), fileMap->fileMap["logfile.txt"]);
}

/* Ensure that get will retrieve files from the map */
void LogFileMapTest::TestGet ()
{
	/* first call get on a file not in the map and make sure it's not in the
	 * map */
	LogFile* nothere = fileMap->Get("logfile.txt");
	CPPUNIT_ASSERT_EQUAL_MESSAGE("expected file to not be present in the map, "
		"but non-null returned", static_cast<LogFile*>(NULL), nothere);

	auto_ptr <LogFile> loggy(new LogFile("/bogusdir/", "logfile.txt", 1, 5));
	fileMap->fileMap["logfile.txt"] = loggy.get();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("could not find logfile.txt in the map",
		loggy.get(), fileMap->Get ("logfile.txt"));
}

/* Ensure that we can grab the core log file from the map */
void LogFileMapTest::TestGetCoreLog ()
{
	BogusCoreLogFile core ("foo-core.1", "foo-core");

	fileMap->coreLog = &core;

	CPPUNIT_ASSERT_EQUAL_MESSAGE("core log not gotten right",
			static_cast<CoreLogFile*>(&core), fileMap->GetCoreLog());
}

/* See that we can set the core log properly */
void LogFileMapTest::TestSetCoreLog ()
{
	BogusCoreLogFile core ("foo-core.1", "foo-core");

	fileMap->SetCoreLog(&core);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("core log not set right",
			static_cast<CoreLogFile*>(&core), fileMap->coreLog);
}

/* See that we can grab an iterator with the map and lock and such properly set
 * */
void LogFileMapTest::TestIterator ()
{
	/* Make some files for the map */
	BogusLogFile fileA("fileA.txt");
	BogusLogFile fileB("fileB.txt");
	string keyA("keyA");
	string keyB("keyB");

	/* insert them */
	LogFileMap_t &myMap = fileMap->fileMap;
	myMap[ keyA ] = &fileA;
	myMap[ keyB ] = &fileB;

	/* grab an iterator */
	auto_ptr<LogFileIterator> it(fileMap->Iterator());

	/* grab some iterators directly from the map to compare with */
	LogFileMap_t::iterator first = myMap.begin ();
	LogFileMap_t::iterator last = myMap.end ();

	/* grab the locations in the map from the LogFileIterator */
	LogFileMap_t::iterator hisFirst = it->first;
	LogFileMap_t::iterator hisLast = it->last;
	
	/* now ensure that the files pointed by the iterators we got from the
	 * LogFileMap's LogfileIterator match those given by directly hacking up
	 * the map. */
	LogFile &refFirst = *first->second;
	LogFile &refHisFirst = *hisFirst->second;

	LogFile &refLast = *last->second;
	LogFile &refHisLast = *hisLast->second;

	CPPUNIT_ASSERT_EQUAL_MESSAGE("'first' iterator wrong", &refFirst, &refHisFirst);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("'last' iterator wrong", &refLast, &refHisLast);
}

/* make sure we can remove things from the map */
void LogFileMapTest::TestRemove ()
{
	/* Make some files for the map */
	BogusLogFile fileA("fileA.txt");
	BogusLogFile fileB("fileB.txt");
	string keyA("keyA");
	string keyB("keyB");

	/* insert them */
	LogFileMap_t &myMap = fileMap->fileMap;
	myMap[ keyA ] = &fileA;
	myMap[ keyB ] = &fileB;

	fileMap->Remove (keyA);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("keyA not removed",
			static_cast<LogFile*>(NULL), fileMap->Get("keyA"));
}

/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
