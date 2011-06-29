#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <iostream>
#include <string>
using namespace std;

// Our directory's collector test fixture
#include "ServicesLayerTestSuite.h"
#include "ProgressListener.h"

#if 0
// This is our progress listener
class AOProgressListener : public CppUnit::TextTestProgressListener
{
	void startTest(CppUnit::Test *test)
		{
			string name = test->getName();
			// cut off first 2 chars
			name.erase(0,2);
			// Get the test suite name
			int position = name.find('.');
			name.erase(position, name.size() - position);
			//name = name.left(name.find('.'));
			if(m_name != name) {
				if(m_name != "")
				{
					cout << endl;
				}

				cout << "Running: " << name << endl;
				m_name = name;
			}
		}
	private:
        string m_name;
};
#endif

int main( int argc, char* argv[])
{
	// Create our runner
	CppUnit::TextUi::TestRunner runner;

	// Make the test
	runner.addTest( new ServicesLayerTestSuite() );

	// Add the test progress listener
//	AOProgressListener progress;
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
