#include "ILogger.h"

#include <pthread.h>
#include <stdio.h>
#include <iostream.h>
#include <sys/syspage.h>


const char* filename = "IncrementConfig.xml";

int TEST_NUM = 0;

int main( int argc, char *argv[])
{

		// Get test number suffix
		if(argc < 2)
		{
				printf("Usage: IncrementTestNumber.qxe <test_num>\n");
				exit(1);
		}
		TEST_NUM = atoi(argv[1]);


		ILogger logger;
		const XmlNode* confDoc;
		try
		{
				// Load the config document
				XmlParser parser;
				confDoc = parser.ReturnXMLDocument(filename); 

				if (confDoc == 0) {
						cout << "BARF - null config doc" << endl;
						exit(1);
				}
				logger.Initialize( confDoc );
				cout << "-- IncrementTestNumber.cpp::logWorker: ILogger initialized." << endl;

		}
		catch(BepException &e)
		{
				cout << "Exception: " << e.what() << endl;
				exit(1);
		}

		logger.NewTestNumberLog(TEST_NUM);

}
