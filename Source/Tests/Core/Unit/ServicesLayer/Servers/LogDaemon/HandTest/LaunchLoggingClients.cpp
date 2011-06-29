#include <string>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <errno.h>
#include <iostream.h>

#include <unistd.h>
#include <sys/syspage.h>

#include "IInterProcessCommunication.h"
#include "BepDefs.h"
#include "LogIfc.hpp"
#include "LogDefs.h"

#define LLC_DEBUG

extern void loggingLoop (string, int rate, int numOfMessages);
static std::string logMsg = "This is a default message from the LaunchLoggingClients executable.\n"

void main(int argc, char* argv[]) {
	if (argc < 5) {
		cout << "Usage: LaunchLoggingClients config_file num_of_clients message_rate num_of_messages [msg]" << endl;
		exit(1);
	}
	string confFileName(argv[1]);
	string numOfProcsStr(argv[2]);
	string rateStr(argv[3]);
	string numOfMessagesStr(argv[4]);
	
	int numOfProcs = atoi(numOfProcsStr.c_str());
	int rate = atoi(rateStr.c_str());
	int numOfMessages = atoi(numOfMessagesStr.c_str());

	if (argc > 5) {
		logMsg = argv[5];
	}
	
	if ((rate < 0) || (rate > 500)) {
		cout << "rate out of range 1 to 500" << endl;
		exit(1);
	}
	
	for(int i=0; i<numOfProcs; i++) {
#ifdef LLC_DEBUG		
		cout << "forking logging child " << i << endl;
#endif
		
		if (fork() == 0) {
			loggingLoop(confFileName, rate, numOfMessages);
			exit(0);
		}
	}
#ifdef LLC_DEBUG
	cout << "waiting on child loggers..." << endl;
#endif
	
	int status;
	for(int i=0; i<numOfProcs; i++) {
		wait(&status);
	}	
}

void loggingLoop(string fileName, int rate, int numOfMessages) {
	// doze delay in microseconds to achieve rate
	int doze;
	if (rate == 0) {
		doze = 0;
	} else {
		doze = (int) ((1.0 / rate) * 1000000.0);
	}	

#ifdef LLC_DEBUG
	cout << "dozing " << doze << " microseconds for rate " << rate << endl;
#endif

		// Create logger
	LogIfc logger;
	XmlParser parser;
	const XmlNode* confXml = parser.ReturnXMLDocument(fileName.c_str());

	logger.Initialize( confXml );


	// Begin logging loop
	double startTick = ClockCycles();
	for (int i = 0; i < numOfMessages; i++) {
		logger.Log(logMsg);
		if (doze != 0) {
			usleep(doze);
		}
	}
	double stopTick = ClockCycles();

	double total = (stopTick - startTick) / SYSPAGE_ENTRY(qtime)->cycles_per_sec;

	double realizedRate = (numOfMessages / total);
	
	cout << "Time: " << total <<  " Rate: " << realizedRate << endl;

}


