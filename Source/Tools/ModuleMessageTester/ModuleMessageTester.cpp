//------------------------------------------------------------------------------
#include "ModuleMessageTester.h"
//------------------------------------------------------------------------------

ModuleMessageTester::ModuleMessageTester()
{
}

ModuleMessageTester::~ModuleMessageTester()
{
	m_vehicleMessages.clear(true);
	m_txMessages.clear(true);
	m_txInterps.clear(true);
	m_rxMessages.clear(true);
	m_rxInterps.clear(true);
}

void ModuleMessageTester::CheckModuleMessageTable(const string &pathName, const string &fileName, const bool &multiFiles)
{
	vector<string> matchingFiles;
	if (multiFiles)
	{
		cout << "Finding matching files for " << fileName << " in " << pathName << endl;
	}
	else
	{
		matchingFiles.push_back(pathName + fileName);
	}
	for (UINT16 index = 0; index < matchingFiles.size(); index++)
	{	// Load the config file that was supplied
		cout << "Loading module message table from: \n\t" << matchingFiles[index];
		if (BEP_STATUS_SUCCESS == LoadModuleMessageTable(matchingFiles[index]))
		{   // Module message table was loaded, check the message sections
			// Check the transmit messages
			cout << "Checking TransmitMessages .......... ";
			cout << ((CheckMessages(&m_vehicleMessages, &m_txMessages, "TransmitMessage", true) == BEP_STATUS_SUCCESS) ? "OK" : "\nFailed\n") << endl;
			// Check the transmit interpretations
			cout << "Checking TransmitInterpretations ... ";
			cout << ((CheckMessages(&m_vehicleMessages, &m_txInterps, "TransmitInterpretation", false) == BEP_STATUS_SUCCESS) ? "OK" : "\nFailed\n") << endl;
			// Check the receive messages
			cout << "Checking ReceiveMessages ........... ";
			cout << ((CheckMessages(&m_vehicleMessages, &m_rxMessages, "ReplyMessage", true) == BEP_STATUS_SUCCESS) ? "OK" : "\nFailed\n") << endl;
			// Check the receive interpretations
			cout << "Checking ReceiveInterpretations .... ";
			cout << ((CheckMessages(&m_vehicleMessages, &m_rxInterps, "ReplyInterpretation", false) == BEP_STATUS_SUCCESS) ? "OK" : "\nFailed\n") << endl;
		}
		else
		{
			cout << "\t\tError loading module message table!" << endl << endl;
		}
	}
}

BEP_STATUS_TYPE ModuleMessageTester::LoadModuleMessageTable(const string &fileName)
{
	XmlParser parser;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	try
	{	// Load the config file
		const XmlNode* messageConfig = parser.ReturnXMLDocument(fileName)->getChild("SerialMessages");
		// Get the individual sections from the node
		m_vehicleMessages.DeepCopy(messageConfig->getChild("VehicleMessages")->getChildren());
		m_txMessages.DeepCopy(messageConfig->getChild("TransmitMessages")->getChildren());
		m_rxMessages.DeepCopy(messageConfig->getChild("ReplyMessages")->getChildren());
		m_txInterps.DeepCopy(messageConfig->getChild("TransmitInterpretations")->getChildren());
		m_rxInterps.DeepCopy(messageConfig->getChild("ReplyInterpretations")->getChildren());
		status = BEP_STATUS_SUCCESS;
		cout << " \n\tSuccess" << endl;
	}
	catch (XmlException &excpt)
	{
		cout << " \n\tFailed" << endl;
		cout << "\t\tXmlException loading message table: " << excpt.GetReason() << endl;
		status = BEP_STATUS_FAILURE;
	}
	catch (BepException &excpt)
	{
		cout << " Failed" << endl;
		cout << "\t\tBepException loading message table: " << excpt.GetReason() << endl;
		status = BEP_STATUS_FAILURE;
	}
	return status;
}
BEP_STATUS_TYPE ModuleMessageTester::CheckMessages(const XmlNodeMap *messageTable, 
												   const XmlNodeMap *messageSection, 
												   const string &sectionName,
												   const bool   requiredSection)
{
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
	// Check each entry in the table
	for (XmlNodeMapCItr iter = messageTable->begin(); iter != messageTable->end(); iter++)
	{  
        try
        {
            // Get the message tag to look for
            string tag(iter->second->getChild(sectionName)->getValue());
            if(!tag.empty())
            {   // Look for the entry in the specified section
                if(messageSection->find(tag) == messageSection->end())
                {   // Tag was specified, but no message entry
                    cout << endl << "\tERROR: Missing entry for " << tag << " in " << sectionName << " section!";
                    status = BEP_STATUS_FAILURE;
                }
            }
            else
            {   // No tag specified, check if a tag is required
                if(requiredSection)
                {   // Error, need a tag in this section
                    cout << endl << "\tERROR: No tag specified for message " << iter->second->getName() << ":" << sectionName;
                    status = BEP_STATUS_FAILURE;
                }
                else
                {   // Ok, just move on, do nothing special
                }
            }
        }
        catch(XmlException &excpt)
        {
            cout << endl << "\tERROR: XmlException checking " << iter->second->getName() << ":" << sectionName << ": " << excpt.GetReason() << endl;
            status = BEP_STATUS_FAILURE;
        }
	}
	return status;
}
