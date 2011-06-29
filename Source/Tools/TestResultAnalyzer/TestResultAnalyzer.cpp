#include "TestResultAnalyzer.h"
#include <dirent.h>
#include <string>
#include <sys/types.h>
#include <sys/dir.h>
#include <errno.h>
//------------------------------------------------------------------------------


TestResultAnalyzer::TestResultAnalyzer() : m_numberOfTests(0), m_passedTestCount(0), m_percentPass(0)
{
	m_testResultPath = (getenv("FTP_ROOT") + string("/TestResults/Vehicle/"));
}

TestResultAnalyzer::~TestResultAnalyzer()
{
	m_testResults.clear(true);
}

void TestResultAnalyzer::LoadTestResult(const string &vin)
{
	// Find all files that match what we are looking for
	vector<string> files;
	FindMatchingFiles(vin, files);
	string fileName;
	// Allow the operator an opportunity to select a file
	if (!files.empty())
	{
		if (files.size() > 1)
		{
			fileName = m_testResultPath + SelectTestResultFile(files);
		}
		else
		{
			fileName = m_testResultPath + files.front();
		}
	}
	// load the test result file
	XmlParser parser;
	const XmlNode *results = parser.ReturnXMLDocument(fileName);
	// Store the results
	m_testResults.clear(true);
	m_testResults.DeepCopy(results->getChild("TestResult")->getChildren());
}

void TestResultAnalyzer::FindMatchingFiles(const string &vin, vector<string> &fileNames)
{
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	if ((dir = opendir(m_testResultPath.c_str())) != NULL)
	{   // Directory is open, loop through and get all the files
		for (;;)
		{
			entry = readdir(dir);
			if (entry == NULL) 
			{
				break;
			}
			else
			{   // print the name of the file
				string file(entry->d_name);
				// determine if the file should be added to the list
				if (file.find(vin) != file.npos)
				{
					fileNames.push_back(file);
				}
			}
		}
		closedir(dir);
	}
	else
	{
		cout << "ERROR: Could not open: " << m_testResultPath << " -- " << strerror(errno) << endl;
		fileNames.clear();
	}
}

string TestResultAnalyzer::SelectTestResultFile(vector<string> &fileNames)
{
	// Print the list of files for the user
	int selectedFileNumber = 0;
	string selectedFileName;
	cout << "Please select a test result file from the following list: " << endl << endl;
	for (unsigned short index = 0; index < fileNames.size(); index++)
	{
		cout << "\t" << index+1 << ". " << fileNames[index] << endl;
	}
	cout << endl << "Enter number of file you wish to use: ";
	cin >> selectedFileNumber;
	selectedFileName = fileNames[selectedFileNumber-1];
	return selectedFileName;
}

void TestResultAnalyzer::GetTestResultStats(int &numberOfTests, int &passedTestCount, float &percentPass)
{   // First we need to perform a quick analysis
	AnalyzeResults();
	numberOfTests = GetNumberOfTestsPerformed();
	passedTestCount = GetNumberOfPassedTests();
	percentPass = CalculateTestPassPercent();
}
	
void TestResultAnalyzer::AnalyzeResults(void)
{
	m_passedTestCount = 0;
	m_numberOfTests = 0;
	for (XmlNodeMapItr iter = m_testResults.begin(); iter != m_testResults.end(); iter++)
	{   // Need to ignor AllFailures node and DTC node
		string testStepName = iter->second->getName();
		if (testStepName.compare("AllFailures") && 
			testStepName.compare("DTC") &&
			(testStepName.find("OverallTest") == testStepName.npos))
		{
			try
   			{	// Check the result.  If pass count it
				string result = iter->second->getAttribute("Result")->getValue();
				if (!result.compare("Pass")) m_passedTestCount++;
				// Increment the number of actual tests performed
				m_numberOfTests++;
			}
			catch (XmlException &excpt)
			{   // Nothing special to do.  This node was not a test result.
			}
		}
	}
}

inline const int& TestResultAnalyzer::GetNumberOfTestsPerformed(void)
{
	return m_numberOfTests;
}

inline const int& TestResultAnalyzer::GetNumberOfPassedTests(void)
{
	return m_passedTestCount;
}

const float& TestResultAnalyzer::CalculateTestPassPercent(void)
{
	m_percentPass = (((float)m_passedTestCount / (float)m_numberOfTests) * (float)100.0);
	return m_percentPass;
}
