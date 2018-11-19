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
#ifndef IsuzuPlantHostInbound_h
#define IsuzuPlantHostInbound_h
//-------------------------------------------------------------------------------------------------
#include "PlantHostInbound.h"
//-------------------------------------------------------------------------------------------------

#define SMALLTIRESIZE ".458"
#define NGASTIRESIZE  ".5"
#define LARGETIRESIZE ".520"

#define RET_ROLL_HI_TAG "RetRollHiPosition"
#define RET_ROLL_LO_TAG "RetRollLoPosition"

class IsuzuPlantHostInbound : public PlantHostInbound
{
public:
    IsuzuPlantHostInbound();
    virtual ~IsuzuPlantHostInbound();

protected:

    /**
     * Returns the build record for the requested vin.
     *
     * @param vin    Vin of the vehicle to be tested.
     * @param buildData Place to store the build data retrieved.
     * @param updateStatus
     *                  Flag to indicate if system status or prompts should be updated.
     *
     * @return Status of the operation (Success, Unavailable,...)
     */
    virtual const string LoadVehicleBuildFromFile(const string &vin, XmlNodeMap &buildData, const bool &updateStatus);

private:
    
    /**
     * Get a list of matching files.
     * The list of files matching the file pattern will be loaded and returned to the calling function.
     * 
     * @param pattern File name pattern to for finding potential build files.
     * @param matchingFiles
     *                List of files that match the given pattern.
     */
    void FindMatchingFiles(const string &pattern, vector<string> &matchingFiles);

    /**
     * Find the version of the file just previous to the specific version provided.
     * The specific file does not exist, so the previous version should be used in its place.
     * 
     * @param matchingFiles
     *               List of files that match except for the lot number.
     * @param specificFile
     *               specific vehicle file to find.
     * 
     * @return Previous version of the file that is on the system.
     */
    string FindPreviousVersionFile(vector<string> matchingFiles, const string &specificFile);

    bool CheckWheelSizes(string fileName);

    void SendWheelSizeToPLC(XmlNodeMap &buildData);
};
//-------------------------------------------------------------------------------------------------
#endif //IsuzuPlantHostInbound_h
