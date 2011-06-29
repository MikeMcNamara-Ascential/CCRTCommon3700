#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ModuleMessageTester.h"

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
	string path;
	bool multiFiles = false;
	
	// Get all the arguments and process them
	cout << endl << endl;
	while( (opt=getopt( argc, argv, "f:p:")) != -1)
	{
		switch (opt)
		{
		case 'f':
			configFile = (string)optarg;
			multiFiles = (configFile.find("*") != configFile.npos);
			if (multiFiles)
			{
				cout << "Analyzing config files matching pattern: " << configFile << endl;
			}
			else
			{
				cout << "Using config file: " << configFile << endl;
			}
			break;
		case 'p':
			path = (string)optarg;
			cout << "Using path:        " << path << endl;
		default:
			break;
		}
	}
	// Make sure we have all the data we need
	if (!configFile.empty())
	{	// Create a new message tester and initialize the tester
		ModuleMessageTester tester;
		// Test the message table
		cout << endl << "Testing Message Table..." << endl;
		tester.CheckModuleMessageTable(path, configFile, multiFiles);
		cout << endl << endl << "Module Message Table tested!  Have a nice day!" << endl << endl << endl;
		cout << "--------------------------------------------------------------------------------------" << endl;
	}
	else
	{
		cout << endl << endl << "WARNING: No config file specified.  Not testing module message table!" << endl << endl << endl;
	}
	return 0;
}
