//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//  Module Interface for the APG 3550 ABS.
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
//    $Log$
// 
//*************************************************************************
#ifndef MandoAbsTc_h
#define MandoAbsTc_h
//-----------------------------------------------------------------------------
#include "GenericABSTCTemplate.cpp"

//-----------------------------------------------------------------------------
template <class ModuleType>
class MandoAbsTc : public GenericABSTCTemplate<ModuleType>
{
public: 
    MandoAbsTc();
    virtual ~MandoAbsTc();
    /**
     * Sequence a test step.
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Test Step Functions Called:</b>
     *      <ul>
     *      <li> Setup() </li>
     *      <li> FinishUp() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      </ul>
     *
     * @param value    This is the value that was commanded by the test sequencer.
     * @return  The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

protected:
    // Used to skip test steps for ESP only vehicles and vehicles without HDC
    bool isEsp;
    bool isHdc;
    INT32 m_espStartIndex;
    INT32 m_espEndIndex;

    /** Reduction and recovery start and end indices for each wheel. */
    std::vector<ABSReductionRecoveryIndex>  m_espReduxRecovIndex;
    
    /**
     * Command the module to read ESP equipped byte from the ECU.
     * 
     * @return Result of commanding the module to read the ECU ID of the vehicle.
     */
    string CheckEspEquipped(void);

    /**
     * Command the module to check if the EOL test byte has been written.
     * 
     * @return Result of commanding the module to read the test byte.
     */
    string CheckEolTestByte(void);

    /**
     * Command the module to write the EOL test byte.
     * 
     * @return Result of commanding the module to write the test byte.
     */
    string WriteEolTestByte(void);

    /**
     * Command the module to copy the variant code if possible.
     * 
     * @return Result of commanding the module to copy the variant code.
     */
    string VariantCoding(void);

    /**
     * Command the module to calibrate the steering angle sensor if possible.
     * 
     * @return Result of commanding the module to calibrate the sas.
     */
    string SasCalibration(void);

    /**
     * Command the module to toggle the brake light.
     * 
     * @return If the module successfully toggeled the light.
     */
    string HdcLightTest(void);

    /**
     * Command the module to turn the rollers on one at a time, then read the wheel
     *   speeds and veryify the correct correlation of sensors and rollers.  Then
     *   the lows speed sensor test is performed
     * 
     * @return Result of proper correlation.
     */
    const std::string LfSensorTest(const std::string &value);

    /**
     * Command the module to turn the rollers on one at a time, then read the wheel
     *   speeds and veryify the correct correlation of sensors and rollers.  Then
     *   the lows speed sensor test is performed
     * 
     * @return Result of proper correlation.
     */
    const std::string LrSensorTest(const std::string &value);

    /**
     * Command the module to turn the rollers on one at a time, then read the wheel
     *   speeds and veryify the correct correlation of sensors and rollers.  Then
     *   the lows speed sensor test is performed
     * 
     * @return Result of proper correlation.
     */
    const std::string RfSensorTest(const std::string &value);

    /**
     * Command the module to turn the rollers on one at a time, then read the wheel
     *   speeds and veryify the correct correlation of sensors and rollers.  Then
     *   the lows speed sensor test is performed
     * 
     * @return Result of proper correlation.
     */
    const std::string RrSensorTest(const std::string &value);

    /**
     * Command the module to perform a high speed ESP test
     * 
     * @return Result of ESP valve firing test.
     */
    string EspValveFiringTest(void);


   /**
    * Analyze the ESP build force for the specified wheel and report the 
    * results.
    * 
    * @param roller The roller to analyze.
    * @param start  The start index for the build pulse.
    * @param end    The end index for the build pulse.
    * @return The result of the check.
    *         BEP_STATUS_SUCCESS or BEP_STATUS_FAILURE
    */
   virtual BEP_STATUS_TYPE AnalyzeESPBuildForces(INT32 roller, INT32 start, INT32 end);

   /**
    * Analyze the ESP reduction force for the specified wheel and report the 
    * results.
    * 
    * @param roller The roller to analyze.
    * @param start  The start index for the build pulse.
    * @param end    The end index for the build pulse.
    * @return The result of the check.
    *         BEP_STATUS_SUCCESS or BEP_STATUS_FAILURE
    */
   virtual BEP_STATUS_TYPE AnalyzeESPReductionForces(INT32 roller, INT32 start, INT32 end);

   virtual string PerformModuleLinkup(void);

   //virtual string EnterDiagnosticMode(void);

    string TcValveFiringTest(void);

    string EvaluateEsp(void);

    string WarningLight(void);

    void InitializeHook(const XmlNode *config);
};

//-----------------------------------------------------------------------------
#endif //MandoAbsTc_h
