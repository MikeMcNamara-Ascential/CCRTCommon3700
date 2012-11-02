//****************************************************************************
//
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/PlantHostInbound/WorkCellController/NispVehicles.cpp 1     6/08/07 10:24a Derickso $
//
// DESCRIPTION:
//    Convenience class for determining if a vehicle is supported by NiSP.
//
//============================================================================
//
//    Copyright (c) 2007- Burke E. Porter Machinery Company
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//============================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/PlantHostInbound/WorkCellController/NispVehicles.cpp $
// 
// 1     6/08/07 10:24a Derickso
// New class for determining of the vehicle is supported by NISP.
// 
//****************************************************************************
#include "Bepos.h"
#include "NispVehicles.h"
//-----------------------------------------------------------------------------

NispVehicles::NispVehicles() : m_analyzeData(false)
{   // Nothing special to do
}

NispVehicles::~NispVehicles()
{   // Clear out the vector
    for(UINT16 index = 0; index < m_nispCriteria.size(); index++)
    {
        delete m_nispCriteria[index];
    }
    m_nispCriteria.clear();
}

void NispVehicles::StoreNispVehicleCriteria(const XmlNode *criteria)
{   // Get the data item to use
    XmlNodeMapCItr dataItem = criteria->getChildren().find("DataItem");
    bool analyzeBuildData = true;
    if(dataItem != criteria->getChildren().end())
    {   // Data item exists, save it and load the criteria
        string buildItem(dataItem->second->getValue());
        VehicleBuildDataItem(&buildItem);
        // Store the criteria
        XmlNodeMapCItr criteriaIter = criteria->getChildren().find("PositionCriteria");
        if(criteriaIter != criteria->getChildren().end())
        {   // Store the analysis criteria
            const XmlNodeMap &criteriaData = criteriaIter->second->getChildren();
            for(XmlNodeMapCItr iter = criteriaData.begin(); iter != criteriaData.end(); iter++)
            {   // Pull all the data from the current node
                NispCriteria *dataConditions = new NispCriteria();
                try
                {
                    string condition(iter->second->getAttribute("condition")->getValue());
                    if(stricmp(condition.c_str(),"equal") == 0)              dataConditions->condition = EQUAL;
                    else if(stricmp(condition.c_str(),"not equal") == 0)     dataConditions->condition = NOT_EQUAL;
                    else if(stricmp(condition.c_str(),"less") == 0)          dataConditions->condition = LESS;
                    else if(stricmp(condition.c_str(),"less equal") == 0)    dataConditions->condition = LESS_EQUAL;
                    else if(stricmp(condition.c_str(),"greater") == 0)       dataConditions->condition = GREATER;
                    else if(stricmp(condition.c_str(),"greater equal") == 0) dataConditions->condition = GREATER_EQUAL;
                    else                                                     dataConditions->condition = EQUAL;
                }
                catch(XmlException &excpt)
                {
                    dataConditions->condition = EQUAL;
                }
                try
                {
                    dataConditions->dataLength = BposReadInt(iter->second->getAttribute("length")->getValue().c_str());
                }
                catch(XmlException &excpt)
                {
                    dataConditions->dataLength = 1;
                }
                try
                {
                    dataConditions->startIndex = BposReadInt(iter->second->getAttribute("index")->getValue().c_str());
                }
                catch(XmlException &excpt)
                {
                    dataConditions->startIndex = 0;
                }
                try
                {
                    dataConditions->value = iter->second->getValue();
                }
                catch(XmlException &excpt)
                {
                    dataConditions->value.erase();
                }
                // Store the criteria
                m_nispCriteria.push_back(dataConditions);
            }
        }
        else
        {   // Do not allow build data to be analyzed - missing criteria for analysis
            analyzeBuildData = false;
        }
    }
    else
    {   // Do not allow build data to be analyzed - missing data item to analyze
        analyzeBuildData = false;
    }
    // Store the flag indicating if the data should be analyzed
    AnalyzeBuildData(&analyzeBuildData);
}

const bool NispVehicles::IsNispSupported(const XmlNodeMap *vehicleBuildData)
{
    bool criteriaMatch = true;
    if(AnalyzeBuildData())
    {   // Get the data item from the build data
        XmlNodeMapCItr iter = vehicleBuildData->find(VehicleBuildDataItem());
        if(iter != vehicleBuildData->end())
        {   // Found the data item, see if it meets the NiSP criteria
            DataAnalysis analyze;
            for(UINT16 index = 0; (index < m_nispCriteria.size()) && criteriaMatch; index++)
            {   // Pull out the substring from the data item
                string dataValue(iter->second->getValue().substr(m_nispCriteria[index]->startIndex, 
                                                                 m_nispCriteria[index]->dataLength));
                // Compare the data to our expected value
                criteriaMatch = analyze.CompareData(dataValue, m_nispCriteria[index]->value, m_nispCriteria[index]->condition);
            }
        }
        else
        {
            criteriaMatch = false;
        }
    }
    else
    {
        criteriaMatch = false;
    }
    // return the flag
    return criteriaMatch;
}

inline const string& NispVehicles::VehicleBuildDataItem(const string *dataItem /*= NULL*/)
{
    if(dataItem != NULL)  m_dataItem = *dataItem;
    return m_dataItem;
}

inline const bool& NispVehicles::AnalyzeBuildData(const bool *analyze /*= NULL*/)
{
    if(analyze != NULL)  m_analyzeData = *analyze;
    return m_analyzeData;
}
