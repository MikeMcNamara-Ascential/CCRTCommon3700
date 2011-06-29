#include "ILogger.h"

#include <pthread.h>
#include <stdio.h>
#include <iostream.h>
#include <sys/syspage.h>

const int LOG_LOOP_COUNT = 50000;
int WORKER_COUNT = 10;

void* logWorker(void* args);

const char* filename = "TestComponentConfigFile.xml";

int main( int argc, char *argv[])
{

	// Get conf file name
	if(argc < 2)
	{
		printf("Usage: Benchmark.qxe <worker_count>\n");
		exit(1);
		//filename = argv[1];
	}
	WORKER_COUNT = atoi(argv[1]);

	// Sample start time
	double startTick = ClockCycles();

	// Create and spin the worker threads
	printf("main: launching workers...\n");
	pthread_t workers[WORKER_COUNT];
	for(int i = 0; i < WORKER_COUNT; i++)
	{
		pthread_create(&workers[i], NULL, logWorker, NULL);
	}

	// Join all worker threads
	printf("main: joining workers...\n");
	for(int i = 0; i < WORKER_COUNT; i++)
	{
		pthread_join(workers[i], NULL);
	}

	// Sample stop time
	double stopTick = ClockCycles();

	// Report
	double total = (stopTick - startTick) / SYSPAGE_ENTRY(qtime)->cycles_per_sec;
	cout << "**** Results:\n";
	cout << WORKER_COUNT << " loggers doing " << LOG_LOOP_COUNT << " messages." << endl;
	//cout << "start =\t\t" << startTick << endl;
	//cout << "end =\t\t" << stopTick << endl;
	cout << "Time = " << total << " seconds" << endl;


}


/**
 * Init a new logger and begin logging loop
 */
void* logWorker(void*)
{
	ILogger logger;
	const XmlNode* confDoc;

	try
	{
		// Load the config document
		XmlParser parser;
		confDoc = parser.ReturnXMLDocument(filename);
		cout << "Config doc logging elem: "<< confDoc->getChild("Setup")->getChild("Logging")->getChild("AdditionalLogFile")->ToString().c_str() << endl;

		//AAAAAAAAAHHHHHHHHHHHHHHGGGGGGGGGGGGG ILogger logger;
		if (confDoc == 0) {
			cout << "BARF - null config doc" << endl;
			exit(1);
		}
		logger.Initialize( confDoc );
		cout << "-- Benchmark.cpp::logWorker: ILogger initialized." << endl;

	}
	catch(BepException &e)
	{
		cout << "Exception: " << e.what() << endl;
		exit(1);
	}

// 	logger.Log("Initializing The LossCompensationTC Component:\n%s\n",
//		confDoc->ToString().c_str());
	// name process
	// TO DO

	// Begin logging loop
	for(int i = 0; i < LOG_LOOP_COUNT; i++)
	{
		logger.Log(string("I, too, am indeed a lumberjack.  But not the kind known for sneakily wearing ladies' undergarments, that's gross.  Manliness is a key issue with all of us."));
	}

	cout << "-- Benchmark.cpp::logWorker: worker is done..." << endl;
    return(0);
}
