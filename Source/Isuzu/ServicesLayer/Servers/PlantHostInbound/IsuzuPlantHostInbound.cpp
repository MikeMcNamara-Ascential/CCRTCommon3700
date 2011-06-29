//*************************************************************************
// FILE DESCRIPTION:
//  PlantHostInbound for Isuzu.  The scanned vehicle barcode will be used to 
//  select the correct vehicle build file.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "IsuzuPlantHostInbound.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <algorithm>
#include <vector>

//-------------------------------------------------------------------------------------------------
IsuzuPlantHostInbound::IsuzuPlantHostInbound() : PlantHostInbound()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
IsuzuPlantHostInbound::~IsuzuPlantHostInbound()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string IsuzuPlantHostInbound::LoadVehicleBuildFromFile(const string &vin, 
                                                             XmlNodeMap &buildData, 
                                                             const bool &updateStatus)
{   
    string fileName;
    string modelCode;
    string bookCode;
    string modelYear;
    string lotNumber;
    string response;
    string status = BEP_ERROR_RESPONSE;
    bool allIdData = false;
    // Get the Model Code, Book Code, Model year and lot number from the system
    if(m_broker->Read(GetDataTag("ModelCode"), response, true) == BEP_STATUS_SUCCESS)
    {
        m_broker->GetByTag(GetDataTag("ModelCode"), modelCode, response);
        // Read the book code from the system
        if(m_broker->Read(GetDataTag("BookCode"), response, true) == BEP_STATUS_SUCCESS)
        {
            m_broker->GetByTag(GetDataTag("BookCode"), bookCode, response);
            // Read the model year from the system
            if(m_broker->Read(GetDataTag("ModelYear"), response, true) == BEP_STATUS_SUCCESS)
            {
                m_broker->GetByTag(GetDataTag("ModelYear"), modelYear, response);
                // Get the lot number
                if(m_broker->Read(GetDataTag("LotNumber"), response, true) == BEP_STATUS_SUCCESS)
                {
                    m_broker->GetByTag(GetDataTag("LotNumber"), lotNumber, response);
                    if(lotNumber[0] == 'P')
                    {//normalize pilot lot number
                        lotNumber[0] = '0';
                    }
                    allIdData = true;
                }
                else
                {
                    Log(LOG_ERRORS, "Could not read system tag: %s", GetDataTag("LotNumber").c_str());
                }
            }
            else
            {
                Log(LOG_ERRORS, "Could not read system tag: %s", GetDataTag("ModelYear").c_str());
            }
        }
        else
        {
            Log(LOG_ERRORS, "Could not read system tag: %s", GetDataTag("BookCode").c_str());
        }
    }
    else
    {
        Log(LOG_ERRORS, "Could not read system tag: %s", GetDataTag("ModelCode").c_str());
    }
    // If all ID data has been retrieved, find the build data file
    if(allIdData)
    {
        XmlParser parser;
        string filePattern = string("Ps") + modelCode + string("_") + bookCode + string("_") + modelYear + string("_");
        string specificFile = filePattern + lotNumber + string(".xml");

        fileName = GetVehicleBuildFilePath() + specificFile;
        Log(LOG_DEV_DATA, "Looking for vehicle file: %s", fileName.c_str());
        // Check if the specific file exists
        if(access(fileName.c_str(), F_OK) != 0)
        {   // Specific file does not exist, find a previous version
            vector<string> fileList;
            Log(LOG_DEV_DATA, "Looking for files matching pattern: %s", filePattern.c_str());
            FindMatchingFiles(filePattern, fileList);
            if(fileList.size() > 0)
            {
                fileName = GetVehicleBuildFilePath() + FindPreviousVersionFile(fileList, specificFile);
            }
            else
            {
                Log(LOG_DEV_DATA, "No files match the pattern, cannot load vehicle data!");
                fileName.erase();
                status = BEP_UNAVAILABLE_RESPONSE;
                ClearVehicleBuild(buildData, true);
            }
        }
        // Only attempt to load the file if we found a matching file
        if(!fileName.empty())
        {   // Get the build data from the selected file
            Log(LOG_DEV_DATA,"Retrieving build record from %s", fileName.c_str());
            try
            {
                SetVehicleBuild(parser.ReturnXMLDocument(fileName)->getChild(VEHICLE_BUILD_TAG), buildData);

                AddVehicleBuildItem(string("VIN"), vin, buildData);
                AddVehicleBuildItem(GetDataTag("ModelCode"), modelCode, buildData);
                AddVehicleBuildItem(GetDataTag("BookCode"), bookCode, buildData);
                AddVehicleBuildItem(GetDataTag("ModelYear"), modelYear, buildData);
                AddVehicleBuildItem(GetDataTag("LotNumber"), lotNumber, buildData);
                status = BEP_SUCCESS_RESPONSE;
            }
            catch(BepException &excpt)
            {   // There was an error getting the build record
                status = BEP_UNAVAILABLE_RESPONSE;
                Log(LOG_ERRORS,"Error getting vehicle build from file - %s", excpt.GetReason());
                ClearVehicleBuild(buildData, true);
            }
        }
    }
    else
    {
        Log(LOG_ERRORS, "Could not get all vehicle ID data, not loading build data");
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
void IsuzuPlantHostInbound::FindMatchingFiles(const string &pattern, vector<string> &matchingFiles)
{   // Find the alternatives
    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(GetVehicleBuildFilePath().c_str())) != NULL)
    {   // Read all entries
        while((dirp = readdir(dp)) != NULL)
        {   // Make sure this is a file we can look at further
            string name = string(dirp->d_name);
            if(!name.compare(0, pattern.length(), pattern))
            {
                matchingFiles.push_back(name);
            }
        }
        closedir(dp);
    }
    else
    {   // Could not open directory
        Log(LOG_ERRORS, "Could not open directory to get listing");
    }
}

//-------------------------------------------------------------------------------------------------
string IsuzuPlantHostInbound::FindPreviousVersionFile(vector<string> matchingFiles, const string &specificFile)
{
    matchingFiles.push_back(specificFile);
    // Sort the vector
    sort(matchingFiles.begin(), matchingFiles.end());
    vector<string>::iterator foundFile = find(matchingFiles.begin(), matchingFiles.end(), specificFile) - 1;
    Log(LOG_DEV_DATA, "Selecting file: ", (*foundFile).c_str());;
    return *foundFile;
}
