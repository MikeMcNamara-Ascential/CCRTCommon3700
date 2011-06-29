#include "ToolsTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "ConfigChecker/ConfigCheckerTestSuite.h"

ToolsTestSuite::ToolsTestSuite()
{
	addTest(new ConfigCheckerTestSuite());
}

/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */

