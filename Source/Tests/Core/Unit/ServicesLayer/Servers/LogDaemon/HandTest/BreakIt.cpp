#include "ILogger.h"
#include "IInterProcessCommunication.h"
#include "BepException.h"
#include "XmlParser.h"
#include "XmlNode.h"
#include "LogDefs.h"


#include <pthread.h>
#include <stdio.h>
#include <iostream.h>
#include <sys/syspage.h>
#include <unistd.h>

int INIT_LEN = 500;
int STEP_COUNT = 300;
int STEP_SIZE = 10;

const XmlNode* confDoc;

int old_main( int argc, char *argv[]);

int main( int argc, char *argv[])
{
	ILogger logger;
	try {
		XmlParser parser;
		const XmlNode* confXml = parser.ReturnXMLDocument("LoggingClients.xml");
		logger.Initialize( confXml );
	} catch (BepException &e) {
		cout << "Exception: " << e.what() << endl;
	}

	char buf[100];
	for (int i = 0; i < 20; i++) {
		sleep(2);
		sprintf(buf, "Message [%d] from BreakIt", i);
		logger.Log(buf);
	}


	//IInterProcessCommunication* ipc = NULL;
	//try {
		//ipc = new IInterProcessCommunication(DATA_LOGGER_NAME, "Client",
			//false, 4096, 100);
		//cout << "Connected." << endl;
		//ipc->Close();
		//cout << "Closed." << endl;
	//
	//} catch (BepException &bepex) {
		//cout << "Damn exception:" << bepex.Reason() << endl;
		//if (ipc == NULL) {
			//cout << "null" << endl;
		//} else {
			//cout << "NOT null" << endl;
		//}
	//}
}

int old_main( int argc, char *argv[]){
	// Get conf file name
	const char* filename = "TestComponentConfigFile.xml";

	try {
		// Load the config document
		XmlParser parser;
		confDoc = parser.ReturnXMLDocument(filename);
	} catch (BepException &e) {
		cout << "Exception: " << e.what() << endl;
	}

	ILogger logger;
	logger.SetAdditionalLogFileName( "Server/BreakIt" );
	logger.Initialize( confDoc );

	std::string msg = "";
	for (int i = 0; i < INIT_LEN; i++) {
		msg.append("a");
	}

	// Send log messages.  Each log message is STEP_SIZE chars longer than
	// the previous one.
	for (int i = 0; i < STEP_COUNT; i++) {
		cout << "Sending message size " << (INIT_LEN + (i*STEP_SIZE)) << endl;
		logger.Log(msg);

		// grow the message
		for (int j = 0; j < STEP_SIZE; j++) {
			msg.append("a");
		}
	}
    return(0);
}

/**
 * Init a new logger and begin logging loop
 */
//void* logWorker(void*) {
//	// Create logger
//	ILogger logger;
//	logger.Initialize( confDoc );
//	// name process
//	// TO DO

//	// Begin logging loop
//	for (int i = 0; i < LOG_LOOP_COUNT; i++) {
//		logger.Log("I, too, am indeed a lumberjack.  But not the kind known for sneakily wearing ladies' undergarments, that's gross.  Manliness is a key issue with all of us.");
//	}

//
//}
