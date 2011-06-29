#include "TestReportGenerator.h"
#include "GeneralUtilities.h"
#include <errno.h>
#include <time.h>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TestReportGenerator::TestReportGenerator(const int &numDays, const string &path, const string &fileName) : 
m_timeFrame(numDays * secondsInADay), m_path(path), m_configFileName(fileName), m_config(NULL)
{   // Nothing special to do
    cout << "Generating report for results in the last " << m_timeFrame << " seconds from files located in " << m_path << endl;
}

//-----------------------------------------------------------------------------
TestReportGenerator::~TestReportGenerator()
{   // Make sure our file list vector is cleared
    m_fileList.clear();
    // Close the files
    m_reportFile.close();
    m_dataFile.close();
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE TestReportGenerator::LoadTestResultFiles(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Open the directory to read results from
    DIR *directory = opendir(m_path.c_str());
    if(directory != NULL)
    {   // Opened directory, get all files created in the last x seconds
        cout << "Finding files in " << m_path << endl;
        FindFiles(directory);
        closedir(directory);
        status = BEP_STATUS_SUCCESS;
    }
    else
    {
        status = BEP_STATUS_FAILURE;
        cout << "Could not open " << m_path << ": " << strerror(errno) << endl;
    }
    // Return the status
    return status;
}

//-----------------------------------------------------------------------------
void TestReportGenerator::GenerateReports(void)
{
    string resultFileName(getenv("USR_ROOT"));
    resultFileName += (string("/FailureReport.") + GetTimeAndDate("%Y.%m.%d.txt"));
    string dataReportFileName(getenv("USR_ROOT"));
    dataReportFileName += (string("/DataReport.") + GetTimeAndDate("%Y.%m.%d.txt"));
    cout << "Logging report to: " << resultFileName << endl;
    cout << "Logging data to: " << dataReportFileName << endl;
    // Load the config file
    LoadDataReportConfig();
    // Open Result Files
    if(BEP_STATUS_SUCCESS == OpenResultFiles(resultFileName, dataReportFileName))
    {
        XmlParser parser;
        for(vector<string>::iterator iter = m_fileList.begin(); iter != m_fileList.end(); iter++)
        {   // Load the file
            try
            {
                const XmlNode *testResultData = parser.ReturnXMLDocument(*iter);
                const XmlNode *failureData    = testResultData->getChild("TestResult/AllFailures");
                const XmlNodeMap &resultData  = testResultData->getChild("TestResult")->getChildren();
                const XmlNodeMap &buildData   = testResultData->getChild("VehicleBuild")->getChildren();
                string vin(testResultData->getChild("VehicleBuild/VIN")->getValue());
                // Check if failures were reported
                if((failureData->getChildren().size() > 0) && m_reportFile.is_open())
                {   // Failure data reported, log it
                    cout << "Reporting failure data for: " << vin << endl;
                    ReportFailureData(failureData, vin);
                }
                // Check if DTCs were reported
                const XmlNode *dtcData = testResultData->getChild("TestResult/DTC");
                if((dtcData->getChildren().size() > 0) && m_reportFile.is_open())
                {   // Report the DTC data
                    cout << "Reporting DTC data for: " << vin << endl;
                    ReportDtcData(dtcData);
                }
                // Add data to data report file
                if((m_config != NULL) && m_dataFile.is_open())
                {
                    cout << "Reporting test data for: " << vin << endl;
                    ReportTestData(buildData, resultData);
                }
            }
            catch(XmlException &excpt)
            {
                cout << "Error opening file: " << *iter << endl;
            }
        }
    }
}

//-----------------------------------------------------------------------------
void TestReportGenerator::FindFiles(DIR *directory)
{
    struct dirent *currentFile = NULL;
    struct stat currentFileStats;
    time_t currentTime = time(NULL);
    // Read all the file entries
    while((currentFile = readdir(directory)) != NULL)
    {   // Get the stats on the current file
        string fileName(m_path);
        fileName += string(currentFile->d_name);
        if(stat(fileName.c_str(), &currentFileStats) != -1)
        {   // Ensure this is not a directory
            if(!S_ISDIR(currentFileStats.st_mode) && 
               fileName.compare(m_path+"TPMRetestData") && 
               fileName.compare(m_path+"TestResultIndex"))
            {   // Determine if the current file is a keeper
                if((difftime(currentTime, currentFileStats.st_mtime) < m_timeFrame) ||
                   (m_timeFrame < 0))
                {   // File falls into the correct time frame, add to list
                    m_fileList.push_back(fileName);
                    cout << "Adding to List: " << fileName << endl;
                }
            }
        }
        else
        {   // Could not get file stats for the file
            cout << "Error getting stats for " << fileName << ": " << strerror(errno) << endl;
        }
    }
    cout << endl;
}

//-----------------------------------------------------------------------------
void TestReportGenerator::ReportFailureData(const XmlNode *failureData, const string &vin)
{   // Add the header to the file entry
    m_reportFile << "======================================================================" << endl;
    m_reportFile << "VIN:  " << vin.c_str() << endl << endl << "Test Failures:" << endl;
    // Report each failure node
    const XmlNodeMap &failData = failureData->getChildren();
    for(XmlNodeMapCItr iter = failData.begin(); iter != failData.end(); iter++)
    {   // Ignore overall failure nodes
        string testName(iter->second->getName());
        if(testName.compare("TransmissionWaitForGearsAndLockup") || 
           (!testName.compare("TransmissionWaitForGearsAndLockup") && 
            iter->second->getAttribute(BEP_RESULT)->getValue().compare(BEP_TIMEOUT_RESPONSE)))
        {
            if(testName.find("OverallTest") == testName.npos)
            {   // Not an overall test failure, log it
                m_reportFile << endl << 
                    "\tTest:   " << iter->second->getName() << endl <<
                    "\tResult: " << iter->second->getAttribute(BEP_RESULT)->getValue() << endl <<
                    "\tDesc:   " << iter->second->getAttribute(BEP_DESCRIPTION)->getValue() << endl;
                // List any details
                if(iter->second->getChildren().size() > 0)
                {
                    const XmlNodeMap &detailData = iter->second->getChildren();
                    for(XmlNodeMapCItr detailIter = detailData.begin(); detailIter != detailData.end(); detailIter++)
                    {
                        m_reportFile << "\t\t" << detailIter->second->getName() << ": " <<
                            detailIter->second->getValue() << endl;
                    }
                }

            }
        }
    }
}

//-----------------------------------------------------------------------------
void TestReportGenerator::ReportDtcData(const XmlNode *failureData)
{   // Add the DTC header
    m_reportFile << endl << "\tModule Faults (DTC):" << endl; 
    // Print out the module information
    const XmlNodeMap &dtcNode = failureData->getChildren();
    for(XmlNodeMapCItr moduleIter = dtcNode.begin(); moduleIter != dtcNode.end(); moduleIter++)
    {   // Log the module name
        m_reportFile << "\t\tModule: " << moduleIter->second->getName() << endl;
        // Log each of the faults
        const XmlNodeMap &dtcData = moduleIter->second->getChildren();
        int dtc = 1;
        for(XmlNodeMapCItr dtcIter = dtcData.begin(); dtcIter != dtcData.end(); dtcIter++, dtc++)
        {
            m_reportFile << "\t\t\tDTC " << dtc << ": " << dtcIter->second->getAttribute(DTC_CODE)->getValue() <<
                " - " << dtcIter->second->getValue() << endl;
        }
    }
}

//-----------------------------------------------------------------------------
void TestReportGenerator::ReportTestData(const XmlNodeMap &buildData, const XmlNodeMap &resultData)
{
    m_dataFile << "======================================================================" << endl;
    m_dataFile << "VIN: " << buildData.getNode(VIN_DATA_TAG)->getValue() << "\t\tBuild Date: " << 
        buildData.getNode("BuildDate")->getValue() << endl;
    for(XmlNodeMapCItr iter = m_config->getChildren().begin();
         iter != m_config->getChildren().end();
         iter++)
    {   // Check if the item was tested
        XmlNodeMapCItr buildItem = buildData.find(iter->second->getName());
        if(buildItem != buildData.end())
        {   // Verify the option has the correct value
            if(!buildItem->second->getValue().compare(iter->second->getAttribute("value")->getValue()))
            {
                // Item was equipped, pull all the detail data
                for(XmlNodeMapCItr dataIter = iter->second->getChildren().begin();
                     dataIter != iter->second->getChildren().end();
                     dataIter++)
                {
                    XmlNodeMapCItr resultItem = resultData.find(dataIter->second->getName());
                    if(resultItem != resultData.end())
                    {
                        string dataValue;
                        try
                        {
                            dataValue = resultItem->second->getChild(dataIter->second->getValue())->getValue();
                        }
                        catch(XmlException &excpt)
                        {
                            dataValue = resultItem->second->getAttribute(BEP_RESULT)->getValue();
                        }
                        m_dataFile << "\t\t" << iter->second->getAttribute("value")->getValue() << " - " <<
                            dataIter->second->getValue() << ": " << dataValue << endl;
                    }
                    else
                    {
                        m_dataFile << "\t\t" << iter->second->getAttribute("value")->getValue() << " was not tested" <<
                            endl << "\t\tOverall Vehicle Test Result: " << 
                            resultData.getNode("OverallTest")->getAttribute(BEP_RESULT)->getValue() << endl;
                    }
                }
            }
            else
            {   // Not equipped with this specified option
                m_dataFile << "\t\t" << iter->second->getAttribute("value")->getValue() << " - " <<
                    buildItem->second->getValue() << endl;
            }
        }
    }
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE TestReportGenerator::OpenResultFiles(const string &reportFileName, const string &dataFileName)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    // Open the report file
    m_reportFile.open(reportFileName.c_str(), ios::out);
    if(m_reportFile.bad())
    {
        cout << "WARNING: Could not open report file [" << reportFileName << "]: " << strerror(errno) << endl;
        status = BEP_STATUS_FAILURE;
    }
    // Open the data file
    m_dataFile.open(dataFileName.c_str(), ios::out);
    if(m_dataFile.bad())
    {
        cout << "WARNING: Could not open data report file [" << dataFileName << "]: " << strerror(errno) << endl;
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
void TestReportGenerator::LoadDataReportConfig(void)
{
    if(m_config != NULL)  delete m_config;
    m_config = NULL;
    XmlParser parser;
    string fileName(getenv("USR_ROOT"));
    fileName += "/DataReportConfig.xml";
    try
    {
        m_config = parser.ReturnXMLDocument(fileName)->Copy();
    }
    catch(XmlException &excpt)
    {
        cout << "ERROR: could not load config file: " << fileName << " - " << excpt.Reason() << endl;
        m_config = NULL;
    }
}
