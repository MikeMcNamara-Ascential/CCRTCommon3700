#include <stdio.h>
#include <iostream>
#include "TestReportGenerator.h"

//-----------------------------------------------------------------------------
int main(void)
{   
    cout << "TestReport v2.0" << endl;
    // Get the number of days worth of vehcles to process
    int numDays = 1;
    cout << endl << "Enter number of days to report (-1 for all files): ";
    cin >> numDays;
#if 0
    // Get the path to the test result files
    cout << endl << "Enter directory for test results or Press Enter to accept default: ";
    char cPath[128];
    cin.getline(cPath, sizeof(cPath));
    cin.getline(cPath, sizeof(cPath));
    string path(&cPath[0]);
    if(path.empty())  
    {
        path = string(getenv("USR_ROOT")) + string("/TestResults/Vehicle/");
    }
    else
    {
        path += "/";
    }
#else
    string path(getenv("USR_ROOT"));
    path += string("/TestResults/Vehicle/");
    string configFile("TestReportDataConfig.xml");
#endif
    TestReportGenerator reporter(numDays, path, configFile);
    // Load the test result files.
    reporter.LoadTestResultFiles();
    // Generate the report
    reporter.GenerateReports();

    return 0;
}
