#ifndef TestDataExtractor_h
#define TestDataExtractor_h
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "XmlNode.h"
#include "XmlParser.h"
//-----------------------------------------------------------------------------

class TestDataExtractor
{
public:
    TestDataExtractor();
    ~TestDataExtractor();
    void ExtractTestResultData(const string &vin, vector<string> &testData, const XmlNode *dataFields);

private:
    BEP_STATUS_TYPE FindMatchingFiles(const string &vin, vector<string> &filenames);
    BEP_STATUS_TYPE GetTestResultData(const string &filename, vector<string> &testData, const XmlNode *dataFields);
    string m_testResultPath;
    string m_testType;
    XmlNode *m_testResults;
};

//-----------------------------------------------------------------------------
#endif //TestDataExtractor_h
