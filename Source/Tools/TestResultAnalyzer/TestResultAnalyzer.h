#ifndef TestResultAnalyzer_h
#define TestResultAnalyzer_h
//------------------------------------------------------------------------------
#include <stdio.h>
#include "XmlNode.h"
#include "XmlParser.h"
//------------------------------------------------------------------------------


class TestResultAnalyzer
{
public:
	TestResultAnalyzer();
	virtual ~TestResultAnalyzer();
	virtual void LoadTestResult(const string &vin);
	virtual void GetTestResultStats(int &numberOfTests, int &passedTestCount, float &percentPass);
	
protected:
	virtual void AnalyzeResults(void);
	virtual const int& GetNumberOfTestsPerformed(void);
	virtual const int& GetNumberOfPassedTests(void);
	virtual const float& CalculateTestPassPercent(void);
	virtual void FindMatchingFiles(const string &vin, vector<string> &fileNames);
	virtual string SelectTestResultFile(vector<string> &fileNames);

private:
	XmlNodeMap m_testResults;
	int m_numberOfTests;
	int m_passedTestCount;
	float m_percentPass;
	string m_testResultPath;
};

//------------------------------------------------------------------------------
#endif  //TestResultAnalyzer_h
