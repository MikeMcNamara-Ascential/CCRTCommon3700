#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "PromptTester.h"

#include "Bepos.h"			// BEP OS Wrapper functions
#include "BepDefs.h"		// BEP Standard Definitions
#include "tchar.h"			// XML definitions	
#include "XmlParser.h"		// XML Parser definition
#include "XmlDefs.h"		// XML definitions	
#include "XmlNode.h"		// XML Node definitions
#include "BepXmlUtils.h"	// XML Utilities


int main(int argc, char* argv[])
{
	char opt;
	string configFile;
	
	// Get all the arguments and process them
	cout << endl << endl;
	while( (opt=getopt( argc, argv, "f:")) != -1)
	{
		switch (opt)
		{
		case 'f':
			configFile = (string)optarg;
			cout << "Using config file: " << configFile << endl;
			break;
		default:
			break;
		}
	}
	// Make sure we have all the data we need
	if (!configFile.empty())
	{
		cout << endl << endl << "All data retrieved from the command line." << endl << endl;
		// Prepend the path to the config file
		configFile = getenv("USR_ROOT") + string("/") + configFile;
		// Create a new prompt tester and initialize the tester
		PromptTester promptTester;
		promptTester.Initialize(configFile);
		// Test the prompts
		cout << endl << endl << "Testing Prompt Files..." << endl;
		promptTester.TestPromptFiles();
		cout << endl << endl << "All prompt files tested!  Have a nice day!" << endl << endl << endl;
	}
	else
	{
		cout << endl << endl << "WARNING: No config file specified.  Not testing any prompt files!" << endl << endl << endl;
	}
	return 0;
}
