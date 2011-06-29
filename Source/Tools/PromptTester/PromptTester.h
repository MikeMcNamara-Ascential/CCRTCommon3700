#ifndef PromptTester_h
#define PromptTester_h
//------------------------------------------------------------------------------
#include <stdio.h>
#include "XmlNode.h"
#include "XmlParser.h"
#include "IPromptServer.h"
//------------------------------------------------------------------------------

class PromptTester
{
public:
	typedef map<string, string>           MasterPromptList;
	typedef MasterPromptList::iterator    MasterPromptListItr;
	typedef MasterPromptList::value_type  MasterPromptListVal;

	PromptTester();
	virtual ~PromptTester();
	void TestPromptFiles(void);
	void Initialize(const string &fileName);

private:
	BEP_STATUS_TYPE LoadPromptFile(const string &fileName);
	void TestPrompts(const XmlNode *prompts);
	void ValidatePrompts(const XmlNode *prompts);
	const bool& CheckMasterPromptList(const bool *checkMasterList=NULL);
	void LoadMasterPromptList(const string& filename);

	XmlNode *m_promptFile;
	XmlNodeMap m_promptFiles;

	XmlParser m_parser;

	IPromptServer m_promptServer;

	MasterPromptList m_masterPromptList;

	INT32 m_numberOfPromptBoxes;
	INT32 m_promptDisplayTime;
	bool  m_checkMasterPromptList;
};

//------------------------------------------------------------------------------
#endif //PromptTester_h
