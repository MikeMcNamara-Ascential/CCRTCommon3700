#include <iostream>
#include "BogusLogFile.h"
#include "LogFileIteratorTest.h"
#include "LogFileIterator.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string>
#include <pthread.h>
#include <map>
#include "LogFileMap.h"

using namespace std;

LogFileIteratorTest::LogFileIteratorTest()
{
}

LogFileIteratorTest::~LogFileIteratorTest()
{
}

void LogFileIteratorTest::setUp()
{
}

void LogFileIteratorTest::tearDown()
{
}

/**
 * See that the constructor for LogFileIterator
 * stores the lock reference and creates begin/end
 * iterators on the given map.
 */
void LogFileIteratorTest::TestConstructor()
{
	BogusLogFile fileA("fileA.txt");
	BogusLogFile fileB("fileB.txt");
	string keyA("keyA");
	string keyB("keyB");

	LogFileMap_t myMap;
	myMap[ keyA ] = &fileA;
	myMap[ keyB ] = &fileB;
	
	LogFileMap_t::iterator first = myMap.begin();
	LogFileMap_t::iterator last = myMap.end();

	// create the iterator (locks the mutex)
	LogFileIterator* it = new LogFileIterator(&myMap);
	LogFileMap_t::iterator hisFirst = it->first;
	LogFileMap_t::iterator hisLast = it->last;
	// destroy the iterator (unlocks the mutex)
	delete(it);

	LogFile &refFirst = *first->second;
	LogFile &refHisFirst = *hisFirst->second;

	LogFile &refLast = *last->second;
	LogFile &refHisLast = *hisLast->second;

	CPPUNIT_ASSERT_EQUAL_MESSAGE("'first' iterator wrong", &refFirst, &refHisFirst);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("'last' iterator wrong", &refLast, &refHisLast);
}


/**
 * See that HasNext() faithfully reports availability of more items in
 * the iterator.
 */
void LogFileIteratorTest::TestHasNext()
{
	BogusLogFile fileA("fileA.txt");
	string keyA("keyA");

	LogFileMap_t myMap;
	myMap[ keyA ] = &fileA;
	
	
	// create the iterator (locks the mutex)
	LogFileIterator* it = new LogFileIterator(&myMap);

	// do some stuff, record results
	bool firstAttempt = it->HasNext();
	it->Next();
	bool secondAttempt = it->HasNext();

	// destroy the iterator (unlocks the mutex)
	delete(it);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("first HasNext() should be true",
			true, firstAttempt);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("second HasNext() should be false",
			false, secondAttempt);
}

/**
 * See that Next() correctly gets items from the iterator.
 */
void LogFileIteratorTest::TestNext()
{
	BogusLogFile fileA("fileA.txt");
	BogusLogFile fileB("fileB.txt");
	string keyA("keyA");
	string keyB("keyB");

	LogFileMap_t myMap;
	myMap[ keyA ] = &fileA;
	myMap[ keyB ] = &fileB;
	
	// create the iterator (locks the mutex)
	LogFileIterator* it = new LogFileIterator(&myMap);

	// do some stuff, record results
	LogFile* firstFile = it->Next();
	LogFile* secondFile = it->Next();

	// destroy the iterator (unlocks the mutex)
	delete(it);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("first file wrong",
			static_cast<LogFile*>(&fileA), firstFile);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("second file wrong",
			static_cast<LogFile*>(&fileB), secondFile);

}




/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
