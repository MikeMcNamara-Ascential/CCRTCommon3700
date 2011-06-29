#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <errno.h>
#include "TestDataExtractor.h"

int main(void)
{
    TestDataExtractor extractor;
    // Prompt the user for the configuration file
    string filename;
    cout << endl << endl << "Please enter the config file: ";
    cin >> filename;
    // Load the config file
    XmlParser parser;
    XmlNode *configFile = const_cast<XmlNode *>(parser.ReturnXMLDocument(filename));
    // Get the test data
    vector<string> testData;
    extractor.ExtractTestResultData(configFile->getChild(VIN_DATA_TAG)->getValue(), testData, 
                                    configFile->getChild("ExtractResults"));
    // Save the data to a file
    string fileName;
    cout << endl << endl << "Enter output data file name: ";
    cin >> fileName;
    FILE *dataFile;
    if((dataFile = fopen(fileName.c_str(), "a")) != NULL)
    {
        sort(testData.begin(), testData.end());
        for(UINT16 index = 0; index < testData.size(); index++)
        {
            fprintf(dataFile, "%s\n", testData[index].c_str());
        }
        fclose(dataFile);
    }
    else
    {
        cout << endl << "Could not open " << fileName << ": " << strerror(errno) << endl;
    }
    return 0;
}
