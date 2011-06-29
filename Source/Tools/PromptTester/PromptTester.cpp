//------------------------------------------------------------------------------
#include "PromptTester.h"
//------------------------------------------------------------------------------

PromptTester::PromptTester() : m_promptFile(NULL)
{
}

PromptTester::~PromptTester()
{
	m_promptFiles.clear(true);
}

void PromptTester::TestPromptFiles()
{   // Test each prompt file
	XmlNode *prompts;
	for (XmlNodeMapItr iter = m_promptFiles.begin(); iter != m_promptFiles.end(); iter++)
	{   // Load the next prompt file to be tested
		try
		{
			string pathName = getenv("USR_ROOT") + iter->second->getAttribute("Path")->getValue();
			string fileName = pathName + iter->second->getValue();
			printf("\n\nLoading file: %s\n", fileName.c_str());
			if (BEP_STATUS_SUCCESS == LoadPromptFile(fileName))
			{	
				try
				{   // Pull the prompt node from the config file
					prompts = const_cast<XmlNode *>(m_promptFile->getChild("Setup/Prompts"));
					if (CheckMasterPromptList())
					{
						ValidatePrompts(prompts);
					}
					else
					{   // Run through the prompts and display them on the screen
						TestPrompts(prompts);
					}
				}
				catch (XmlException &excpt)
				{
					printf("Error getting Prompt node from config file: %s\n", excpt.GetReason());
				}
			}
			else
			{
				printf("\n\n\tCould not test prompt file: %s\n", fileName.c_str());
			}
		}
		catch (XmlException &excpt)
		{
			printf("\n\n\tError loading data for prompt file: %s\n", excpt.GetReason());
		}
	}
}

void PromptTester::Initialize(const string &fileName)
{
	// Load the config file
	try
	{
		const XmlNode *config = m_parser.ReturnXMLDocument(fileName);
		// Get the list of prompt files
		try
		{
			m_promptFiles.DeepCopy(config->getChild("PromptFiles")->getChildren());
		}
		catch (XmlException &excpt)
		{
			printf("Error loading prompt file list: %s", excpt.GetReason());
			m_promptFiles.clear(true);
		}
		// Get the number of prompt boxes on the display
		try
		{
			m_numberOfPromptBoxes = BposReadInt(config->getChild("NumberOfPromptBoxes")->getValue().c_str());
		}
		catch (XmlException &excpt)
		{
			printf("Could not find prompt box count, using 1: %s\n", excpt.GetReason());
			m_numberOfPromptBoxes = 1;
		}
		// Get the time to keep each prompt displayed
		try
		{
			m_promptDisplayTime = BposReadInt(config->getChild("PromptDisplayTime")->getValue().c_str());
		}
		catch (XmlException &excpt)
		{
			printf("Could not find prompt display time, using 1 sec: %s\n", excpt.GetReason());
			m_promptDisplayTime = 1000;
		}
		// Determine if the prompts should be displayed or the prompts.properties file checked
		bool checkMasterList = false;
		try
		{
			string checkListValue = config->getChild("UseMasterPromptList")->getValue();
			checkMasterList = (!checkListValue.compare("Yes") || !checkListValue.compare("Y") ||
							   !checkListValue.compare("True") || !checkListValue.compare("T") ||
							   !checkListValue.compare("1"));
		}
		catch (XmlException &excpt)
		{
			printf("Could not find master list check option, displaying prompts to the screen: %s\n", excpt.GetReason());
			checkMasterList = false;
		}
		CheckMasterPromptList(&checkMasterList);
		// Determine if the master prompt list should be retrieved
		if (CheckMasterPromptList())
		{   
			try
			{
				// Get the file name for the master prompt list
				string filename = getenv("USR_ROOT") + config->getChild("MasterPromptList")->getValue();
				// Load the master prompt list
				LoadMasterPromptList(filename);
			}
			catch (XmlException &excpt)
			{
				printf("Error getting master promptlist: %s\n", excpt.GetReason());
				bool checkList = false;
				CheckMasterPromptList(&checkList);
			}
		}
		// Initialize the prompt server comm object
		m_promptServer.Initialize(PROMPT_SERVER_NAME, DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT, false);
	}
	catch (BepException &excpt)
	{
		printf("Error loading config file: %s\n", excpt.GetReason());
	}
}

BEP_STATUS_TYPE PromptTester::LoadPromptFile(const string &fileName)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	try
	{   // Load the file	
		m_promptFile = const_cast<XmlNode *>(m_parser.ReturnXMLDocument(fileName));
		cout << "Loaded prompt file." << endl;
		status = BEP_STATUS_SUCCESS;
	}
	catch (XmlException &excpt)
	{
		printf("\n\n\tXmlExcpetion attempting to load %s: %s\n", fileName.c_str(), excpt.GetReason());
		status = BEP_STATUS_FAILURE;
	}
	catch (BepException &excpt)
	{
		printf("\n\n\tBepExcpetion attempting to load %s: %s\n", fileName.c_str(), excpt.GetReason());
		status = BEP_STATUS_FAILURE;
	}
	// Return the status
	return status;
}

void PromptTester::TestPrompts(const XmlNode *prompts)
{
	// Determine the wait time for each display
	INT32 promptDelay = m_promptDisplayTime / m_numberOfPromptBoxes;
	INT32 currentPromptBox = 1;
	string response;
	PromptDetails details;
	XmlNodeMap promptList;
	promptList.DeepCopy(prompts->getChildren());
	// Loop through and display the prompts in the list
	for (XmlNodeMapItr iter = promptList.begin(); iter != promptList.end(); iter++)
	{   // Clear the prompt box before displaying any prompts
		m_promptServer.ClearPromptBox(currentPromptBox, response);
		// Display the prompt
		try
		{
			cout << "Displaying Prompt for tag: " << iter->second->getName() << "\t\tin box: " << currentPromptBox << endl;
			m_promptServer.DisplayPrompt(currentPromptBox, 
										 iter->second->getValue(),
										 details,
										 response);
		}
		catch (XmlException &excpt)
		{
			cout << "\tERROR: Could not find Prompt value for: " << iter->second->getName() << endl;
		}
		// Wait before displaying the next prompt
		BposSleep(promptDelay);
		currentPromptBox++;
		if (currentPromptBox > m_numberOfPromptBoxes)  currentPromptBox = 1;
	}
	// Displayed all prompts, clean up our mess
	promptList.clear(true);
}

void PromptTester::ValidatePrompts(const XmlNode *prompts)
{   // place the prompt in a map for easy evaluation
	XmlNodeMap promptList;
	promptList.DeepCopy(prompts->getChildren());
	// Loop through the map and verify there is an entry in the master list
	for (XmlNodeMapItr iter = promptList.begin(); iter != promptList.end(); iter++)
	{   // Check the current prompt tag
		cout << "Prompt tag: " << iter->second->getValue() << " - ";
		if (m_masterPromptList.find(iter->second->getValue()) == m_masterPromptList.end())
		{
			cout << "\t\tNot Found!" << endl;
		}
		else
		{
			cout << "\t\tOK" << endl;
		}
	}
}

const bool& PromptTester::CheckMasterPromptList(const bool *checkMasterList)
{
	if (checkMasterList != NULL)  m_checkMasterPromptList = *checkMasterList;
	return m_checkMasterPromptList;
}

void PromptTester::LoadMasterPromptList(const string& filename)
{
	FILE *fd = NULL;
	// Open the master prompt file
	if ((fd = fopen(filename.c_str(), "r")) != NULL)
	{   // File open, load all entries into the map
		char buffer[100];
		while (fgets(buffer, 100, fd) != NULL)
		{   // Check if this is a valid prompt - NOTE: '#' represents a comment
			if (buffer[0] != '#')
			{   // Separate the prompt tag from the text
				string promptDef(buffer);
				UINT32 sep = promptDef.find_first_of("=");
				string tag = promptDef.substr(0, sep - 1); 
				string val = promptDef.substr(sep + 2);
				cout << "Adding prompt def: {" << tag << "}{" << val;
				m_masterPromptList.insert(MasterPromptListVal(tag, val));
			}
		}
		cout << "Loaded master prompt list - " << m_masterPromptList.size() << " prompt tags loaded." << endl;
		// Close the file
		fclose(fd);
	}
	else
	{
		printf("ERROR opening %s - %s\n", filename.c_str(), strerror(errno));
	}
}
