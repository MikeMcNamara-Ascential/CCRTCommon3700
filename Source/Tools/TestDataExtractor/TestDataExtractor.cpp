#include "TestDataExtractor.h"
#include <dirent.h>
#include <string>
#include <vector>
#include <errno.h>
#include <sys/types.h>
#include <sys/dir.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TestDataExtractor::TestDataExtractor() : m_testResults(NULL)
{   // Set the default test result path
    m_testResultPath = getenv("USR_ROOT") + string("/TestResults/Vehicle/");
}

//------------------------------------------------------------------------------
TestDataExtractor::~TestDataExtractor()
{   // Clear the test results
    if(m_testResults != NULL)  delete m_testResults;
    m_testResults = NULL;
}

//------------------------------------------------------------------------------
void TestDataExtractor::ExtractTestResultData(const string &vin, vector<string> &testData, const XmlNode *dataFields)
{  
    vector<string>fileNames;
    // Attempt to find any matching files
    if(BEP_STATUS_SUCCESS == FindMatchingFiles(vin, fileNames))
    {   // Extract data from each matching file
        cout << "Extracting test result data ";
        vector<string> data;
        for(UINT16 index = 0; index < fileNames.size(); index++)
        {
            data.clear();
            if(BEP_STATUS_SUCCESS == GetTestResultData(fileNames[index], data, dataFields))
            {   // Build the entry for the test data
                string dataString("");
                for(UINT16 entry = 0; entry < data.size(); entry++)
                {
                    dataString += (data[entry] + ",");
                }
                //cout << " --> " << dataString;
                testData.push_back(dataString);
            }
//            cout << ".";
        }
    }
    else
    {
        cout << "No files found matching vin: " << vin << endl;
    }
}

//------------------------------------------------------------------------------
BEP_STATUS_TYPE TestDataExtractor::FindMatchingFiles(const string &vin, vector<string> &filenames)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
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
                if ((file.find(vin) != file.npos) || vin.empty())
                {
                    filenames.push_back(file);
                }
            }
        }
        closedir(dir);
    }
    else
    {
        cout << "ERROR: Could not open: " << m_testResultPath << " -- " << strerror(errno) << endl;
        filenames.clear();
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//------------------------------------------------------------------------------
BEP_STATUS_TYPE TestDataExtractor::GetTestResultData(const string &filename, vector<string> &testData, 
                                                     const XmlNode *dataFields)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    XmlParser parser;
    XmlNode *testResults;
    try
    {   // Get the specified file
//        cout << endl << "Getting data from: " << filename;
        testResults = const_cast<XmlNode *>(parser.ReturnXMLDocument(m_testResultPath + filename));
        // Add the category to the data
        try
        {
            testData.push_back(testResults->getChild(dataFields->getChild("Category")->getValue())->getValue());
        }
        catch(XmlException &excpt)
        {
            try
            {
                testData.push_back(testResults->getChild(dataFields->getChild("AltCategory")->getValue())->getValue());
            }
            catch(XmlException &excpt)
            {   // Add the test number to the data
                testData.push_back("FileNumber." + filename.substr(filename.find_first_of(".")+1));
            }
        }
        // Get the data from the results -- only doing loss comp at this time
        for(XmlNodeMapCItr iter = dataFields->getChild("ResultFields")->getChildren().begin();
             iter != dataFields->getChild("ResultFields")->getChildren().end(); 
             iter++)
        {
            try
            {
                testData.push_back(testResults->getChild(iter->second->getValue())->getValue());
            }
            catch(XmlException &excpt)
            {   // Add a place holder so all data fields are the same length
                testData.push_back("-1.00");
            }
        }
    }
    catch(XmlException &excpt)
    {
        cout << "Error extracting data from " << filename << ": " << excpt.GetReason() << endl;
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

