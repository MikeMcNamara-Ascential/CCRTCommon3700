//****************************************************************************
//
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/PlantHostInbound/WorkCellController/NispVehicles.h 1     6/08/07 10:24a Derickso $
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
//    $Log: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/PlantHostInbound/WorkCellController/NispVehicles.h $
// 
// 1     6/08/07 10:24a Derickso
// New class for determining of the vehicle is supported by NISP.
// 
//****************************************************************************
#ifndef NispVehicles_h
#define NispVehicles_h
//-----------------------------------------------------------------------------
#include "DataAnalysis.h"
#include "XmlNode.h"
#include "XmlDefs.h"
#include <vector>
#include <string>
//-----------------------------------------------------------------------------

/**
 * Convenience class for determining if a vehicle is supported by NiSP. <br>
 * Specific criteria will be stored and used later to determine if a vehicle is supported by the NiSP test
 * system.
 * 
 * @author Dan Erickson
 * @since June 05, 2007
 */
class NispVehicles
{
public:
    /**
     * Class constructor
     */
    NispVehicles();
    /**
     * Class destructor
     */
    ~NispVehicles();
    /**
     * Store the criteria to use to decide if the vehicle is a NiSP supported vehicle.
     * 
     * @param criteria Criteria to use to determine if the vehicle is supported by NiSP.
     */
    void StoreNispVehicleCriteria(const XmlNode *criteria);
    /**
     * Examine the provided vehicle build data using the preset criteria to determine if the vehicle should be tested
     * using the NiSP system or the CCRT system.
     * 
     * @param vehicleBuildData
     *               Vehicle Build data to analyze.
     * 
     * @return Flag indicating if the vehicle should be tested using NiSP.
     *         <ul>
     *         <li> True - The vehicle should be tested under the NiSP system. </li>
     *         <li> False - The vehicle should be tested using the CCRT system. </li>
     *         </ul>
     */
    const bool IsNispSupported(const XmlNodeMap *vehicleBuildData);

private:
    /**
     * Store/Retrieve the vehicle build data item to analyze for determining of the vehicle is supported by NiSP.
     * 
     * @param dataItem Vehicle build data item to use for determining if the vehicle is supported by NiSP.
     * 
     * @return Vehicle build data item to use for determining if the vehicle is supported by NiSP.
     */
    const string& VehicleBuildDataItem(const string *dataItem = NULL);
    /**
     * Get/Set flag indicating if the vehicle build data should be analyzed to determine if the vehicle should be
     * tested by the NiSP system or the CCRT system.
     * 
     * @param analyze Flag indicating if the vehicle build data should be analyzed.
     * 
     * @return Flag indicating if the vehicle build data should be analyzed.
     */
    const bool& AnalyzeBuildData(const bool *analyze = NULL);
    /**
     * Structure containing the criteria to apply to the vehicle build data item to determine if the vehicle is
     * supported by the NiSP test system.
     */
    typedef struct _criteria
    {
        INT32 startIndex;        // Start index of the item to be evaluated
        INT32 dataLength;        // Length of the data item to be evaluated
        Comparison_t condition;  // Logical condition to apply to the comparison value
        string value;            // Comparison value to apply to the vehicle build data
    } NispCriteria;
    /**
     * Vehicle build data item to be analyzed.
     */
    string m_dataItem;
    /**
     * All the criteria that must be satisfied in order to accurately determine the 
     * if the vehicle is supported ny NiSP.
     */
    vector<NispCriteria*> m_nispCriteria;
    /**
     * Convenience typedefs.
     */
    typedef vector<NispCriteria*>::iterator   NispCriteriaIter;
    typedef vector<NispCriteria*>::value_type NispCriteriaVal;
    /**
     * Flag indicating if vehicle build data should be analyzed to determine if NiSP should run the test sequence.
     */
    bool m_analyzeData;
};

//-----------------------------------------------------------------------------
#endif //NispVehicles_h
