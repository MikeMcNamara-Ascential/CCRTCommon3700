#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <iostream>
#include <string>
using namespace std;

// Our directory's collector test fixture
#include "DcxServersTestSuite.h"
#include "ProgressListener.h"


int main( int argc, char* argv[])
{
	// Create our runner
	CppUnit::TextUi::TestRunner runner;

	// Make the test
	runner.addTest( new DcxServersTestSuite() );

	// Add the test progress listener
	//AOProgressListener progress;
    ProgressListener progress;
	runner.eventManager().addListener(&progress);

	// Run the tests
	bool wasSuccessful = runner.run();

	// Return error code 1 if the one of test failed.
	return wasSuccessful ? 0 : 1;
}
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
