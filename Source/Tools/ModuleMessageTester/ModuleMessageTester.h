#ifndef ModuleMessageTester_h
#define ModuleMessageTester_h
//------------------------------------------------------------------------------
#include <stdio.h>
#include "XmlNode.h"
#include "XmlParser.h"
//------------------------------------------------------------------------------

class ModuleMessageTester
{
public:
	ModuleMessageTester();
	virtual ~ModuleMessageTester();
	void CheckModuleMessageTable(const string &pathName, const string &fileName, const bool &multiFiles);
	
private:
	void FindMatchingFiles(const string &pathName, const string &fileName, vector<string> &matchingFiles);
	BEP_STATUS_TYPE LoadModuleMessageTable(const string &fileName);
	BEP_STATUS_TYPE CheckMessages(const XmlNodeMap *messageTable, 
								  const XmlNodeMap *messageSection, 
								  const string &sectionName,
								  const bool requiredSection);

	XmlNodeMap m_vehicleMessages;
	XmlNodeMap m_txMessages;
	XmlNodeMap m_txInterps;
	XmlNodeMap m_rxMessages;
	XmlNodeMap m_rxInterps;
};

//------------------------------------------------------------------------------
#endif //ModuleMessageTester_h
