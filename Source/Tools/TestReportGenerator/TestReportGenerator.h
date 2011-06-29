#ifndef TestReportGenerator_h
#define TestReportGenerator_h
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "XmlNode.h"
#include "XmlParser.h"
//------------------------------------------------------------------------------
const int secondsInADay = 86400;  // 60 seconds/min * 60 min/hr * 24hr/day

class TestReportGenerator
{
public:
    TestReportGenerator(const int &numDays, const string &path, const string &configFile);
    ~TestReportGenerator();

    BEP_STATUS_TYPE LoadTestResultFiles(void);
    void GenerateReports(void);

private:
    void FindFiles(DIR *directory);
    void ReportFailureData(const XmlNode *failureData, const string &vin);
    void ReportDtcData(const XmlNode *failureData);
    void ReportTestData(const XmlNodeMap &buildData, const XmlNodeMap &resultData);
    BEP_STATUS_TYPE OpenResultFiles(const string &reportFileName, const string &dataFileName);
    void LoadDataReportConfig(void);
    
    int m_timeFrame;   // Grab all files in last x seconds
    string m_path;
    string m_configFileName;
    vector<string> m_fileList;
    ofstream m_reportFile;
    ofstream m_dataFile; 
    XmlNode *m_config;
};

//------------------------------------------------------------------------------
#endif  //TestReportGenerator_h
