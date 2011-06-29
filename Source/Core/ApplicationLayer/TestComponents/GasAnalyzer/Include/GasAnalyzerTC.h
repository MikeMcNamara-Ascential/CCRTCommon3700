//******************************************************************************
// Description:
//  Gas Analyzer Test Component.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
//=============================================================================
#ifndef GasAnalyzer_h
#define GasAnalyzer_h
//-----------------------------------------------------------------------------
#include "GenericTCTemplate.cpp"

//-----------------------------------------------------------------------------
template <class AnalyzerType>
class GasAnalyzerTC : public GenericTCTemplate<AnalyzerType>
{
public:
    /** Class Constructor */
    GasAnalyzerTC();
    /** Class Destructor */
    virtual ~GasAnalyzerTC();
    /**
     * Sequence the next test step.
     * 
     * @param value  Test step value to be used during the test step.
     * 
     * @return Result of the test.
     */
    virtual const string CommandTestStep(const string &value);
    /**
     * Initialize the test component.
     * 
     * @param fileName File to use for initializing the test component.
     */
    virtual void Initialize(const string &fileName);
    /**
     * Initialize the test component.
     * 
     * @param config Configuration data to use for initializing the test component.
     */
    virtual void Initialize(const XmlNode *config);

protected:
    /**
     * Command the analyzer to enable measurement mode.
     * 
     * @return Result of enabling measurement mode.
     */
    string EnableMeasurementMode(void);
    /**
     * Place the analyzer into standby mode.
     * 
     * @return Result of placing the analyzer into standby mode.
     */
    string EnableStandbyMode(void);
    /**
     * Read the gas analyzer data.
     * 
     * @return Result of reading the gas analyzer data.
     */
    string GatherAnalyzerData(void);

private:
    /** Data items to gather from the gas analyzer */
    XmlNodeMap m_gasAnalyzerDataItems;
};

//-----------------------------------------------------------------------------
#endif //GasAnalyzer_h
