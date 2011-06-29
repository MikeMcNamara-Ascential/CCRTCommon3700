#include <stdio.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <stdarg.h>
#include <sys/dispatch.h>

#include "Bepos.h"			// BEP OS Wrapper functions
#include "BepDefs.h"		// BEP Standard Definitions
#include "tchar.h"			// XML definitions
#include "XmlParser.h"		// XML Parser definition
#include "XmlDefs.h"		// XML definitions
#include "XmlNode.h"		// XML Node definitions
#include "BepXmlUtils.h"	// XML Utilities

#include "GenericModuleTemplate.cpp"
#include "KeywordProtocolFilter.h"

void CursesInit();
void CursesClose();
void UpdateScreen(const string &data, const SerialString_t &moduleResponse, const BEP_STATUS_TYPE &status);

string messageTag;
string messageType;
string configFile;
string dataType;
string protocol;
int    messageGap = 0;

int main(int argc, char* argv[])
{
	char opt;

	messageTag.erase();
	configFile.erase();
	messageType.erase();
	dataType.erase();
	protocol.erase();

	// Get all the arguments and process them
	cout << endl << endl;
	while( (opt=getopt( argc, argv, "m:d:l:f:t:")) != -1)
	{
		switch (opt)
		{
		case 'm':
			messageTag = (string)optarg;
			cout << "Sending message tag: " << messageTag << endl;
			break;

		case 'l':
			messageGap = atoi(optarg);
			if (!messageGap)
				cout << "Sending message to the module one time." << endl;
			else
				cout << "Waiting " << messageGap << "ms between messages" << endl;
			break;

		case 'f':
			configFile = (string)optarg;
			cout << "Using config file: " << configFile << endl;
			break;

		case 't':
			messageType = (string)optarg;
			cout << "Sending message type: " << messageType << endl;
			break;

		default:
			break;
		}
	}
	// Make sure we have all the data we need
	if (!configFile.empty() && !messageTag.empty())
	{
		cout << endl << endl << "All data retrieved from the command line." << endl << endl;
		// Get the Configuration file to use
		XmlParser parser;
		XmlNode *config = NULL;
		bool continueProgram = true;
		try
		{
			config = const_cast<XmlNode *>(parser.ReturnXMLDocument(getenv("USR_ROOT") + configFile));
			// Get the protocol from the config file
			protocol = config->getChild("Setup/CommunicationProtocol")->getValue();
		}
		catch (XmlException &excpt)
		{
			cout << endl << endl << "XmlExcpetion loading config file: " << excpt.GetReason() << endl << endl;
			continueProgram = false;
		}
		catch (BepException &excpt)
		{
			cout << endl << endl << "BepExcpetion loading config file: " << excpt.GetReason() << endl << endl;
			continueProgram = false;
		}
		if (continueProgram)
		{
			cout << "Communication Protocol: " << protocol << endl;
			// Create a new module interface based on the communication protocol.
			IGenericModule *module = NULL;
			if (!protocol.compare("KeywordProtocol2000"))
			{
				cout << endl << endl << "Creating module interface with DCX CAN KWP" << endl << endl;
//				module = new GenericModuleTemplate<DcxCANKeyword2000ProtocolFilter>();
				module = new GenericModuleTemplate<KeywordProtocolFilter>();
			}
			else
			{
				cout << endl << endl << "Unsupported communication protocol: " << protocol << endl << endl;
			}
			// Ensure we have a module interface to work with
			if (module != NULL)
			{
				cout << endl << endl << "Initializing module interface..." << endl << endl;
				module->Initialize(config);
				// Get the data type from the config file
				string replyInterpTag = config->getChild("SerialMessages/VehicleMessages")->getChild(messageTag)->getChild("ReplyInterpretation")->getValue();
				if (!replyInterpTag.empty())
				{   // Get the data type from the reply interpretation
					const XmlNode *replyNode = config->getChild("SerialMessages/ReplyInterpretations")->getChild(replyInterpTag)->getChildren().getNode(0);
					dataType = replyNode->getName();
					if (stricmp(dataType.c_str(), "Integer") == 0)
					{
						dataType = replyNode->getAttribute("type")->getValue() + " " + dataType;
					}
				}
				// Start reading data from the module
				BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
				string data("Data not read from module");
				char dataBuff[128];
				SerialString_t rawData;
				SerialArgs_t *args = NULL;
				// Setup the window after clearing it out
				for( int ii=0; ii<80; ii++)		printf( "\n");
				CursesInit();
				UpdateScreen(data, rawData, status);
				do
				{
					if (stricmp(dataType.c_str(), "string") == 0)
					{
						status = module->ReadModuleData(messageTag, data, args, &rawData);
					}
					else if ((stricmp(dataType.c_str(), "integer") == 0) ||
							 (stricmp(dataType.c_str(), "simple signed integer") == 0))
					{
						int iData = 0;
						status = module->ReadModuleData(messageTag, iData, args, &rawData);
						data = itoa(iData, dataBuff, 10);
					}
					else if (stricmp(dataType.c_str(), "unsigned integer") == 0)
					{
						unsigned short uData = 0x0000;
						status = module->ReadModuleData(messageTag, uData, args, &rawData);
						sprintf(dataBuff, "%04X", uData);
						data = dataBuff;
					}
					else if (stricmp(dataType.c_str(), "Float") == 0)
					{
						float fData = 0;
						status = module->ReadModuleData(messageTag, fData, args, &rawData);
						sprintf(dataBuff, "%.2f", fData);
						data = dataBuff;
					}
					else if (stricmp(dataType.c_str(), "Boolean") == 0)
					{
						bool bData = false;
						status = module->ReadModuleData(messageTag, bData, args, &rawData);
						data = bData ? "On" : "Off";
					}
					UpdateScreen(data, rawData, status);
					BposSleep(messageGap);
				} while (BEP_STATUS_SUCCESS == status);
				// Get rid of the module interface
				if (module != NULL) delete module;
				module = NULL;
				// Close out the window
				CursesClose();
			}
			else
			{
				cout << endl << "Module was not created!!" << endl << endl;
			}
		}
	}
	else
	{
		cout << endl << "All required data not supplied!" << endl;
	}
	cout << endl << endl << "Have a nice day!" << endl << endl;
	return 0;
}

#include <curses.h>

static WINDOW	*window;

void CursesInit()
{
	int c, rows, cols;
	/*
		Use all rows available.
		Last row might be used by terminal status line though. Can't help it...
	*/
	c = tcgetsize(fileno(stdout), &rows, &cols);
	if (c < 0 || !rows || !cols)
	{
		rows = 40;
		cols = 80;
	}

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	intrflush(stdscr, FALSE);

	window = newwin(rows,cols,0,0);
	nodelay(window, TRUE);

	wclear( stdscr);
	curs_set( 0);
}

void CursesClose()
{
	wclear( stdscr);
	delwin( window);
	endwin();
}

void UpdateScreen(const string &data, const SerialString_t &moduleResponse, const BEP_STATUS_TYPE &status)
{
	static int	spinIdx=0;
	static char spinner[] = "/-\\|";
	char		lineBuff[ 1024];
	char    respBuff[16];
	int     lineNumber = 1;

	sprintf( lineBuff, "%c ", spinner[ spinIdx++ %4]);
	mvwaddstr(window, lineNumber++, 5, lineBuff);
	sprintf(lineBuff, "Config File:    %s\n", configFile.c_str());
	mvwaddstr(window, lineNumber++, 5, lineBuff);
	sprintf(lineBuff, "Message Delay:  %d\n", messageGap);
	mvwaddstr(window, lineNumber++, 5, lineBuff);
	sprintf(lineBuff, "Data Type:      %s\n", dataType.c_str());
	mvwaddstr(window, lineNumber++, 5, lineBuff);
	lineNumber++;  // Add an extra blank line
	sprintf(lineBuff, "Message Tag:    %s\n", messageTag.c_str());
	mvwaddstr(window, lineNumber++, 5, lineBuff);

	std::string	dashes( /*strlen( lineBuff)*/80, '-');
	strcpy( lineBuff,  dashes.c_str());
	mvwaddstr(window, lineNumber++, 5, lineBuff);
	lineNumber++;  // Add an extra blank line

	sprintf(lineBuff, "Module Data:      %s\n", data.c_str());
	mvwaddstr(window, lineNumber++, 5, lineBuff);
	lineNumber++;  // Add an extra blank line
	string displayResponse;
	for (unsigned short index = 0; index < moduleResponse.length(); index++)
	{
		displayResponse += CreateMessage(respBuff, sizeof(respBuff), "%02X ", moduleResponse[index]);
	}
	sprintf(lineBuff, "Module Response:  %s\n", displayResponse.c_str());
	mvwaddstr(window, lineNumber++, 5, lineBuff);
	lineNumber++;  // Add an extra blank line
	sprintf(lineBuff, "Message Status :  %s\n", ConvertStatusToResponse(status).c_str());
	mvwaddstr(window, lineNumber++, 5, lineBuff);

	wrefresh( window);
	curs_set( 0);
}

