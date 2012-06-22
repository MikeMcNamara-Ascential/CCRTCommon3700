//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
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
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef PowerSteeringModule_h
#define PowerSteeringModule_h
//-----------------------------------------------------------------------------
#include "GenericModuleTemplate.cpp"
//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
class PowerSteeringModule : public GenericModuleTemplate<ProtocolFilterType> {
public:
    PowerSteeringModule();
    virtual ~PowerSteeringModule();
    virtual string ModuleName();
    virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);
    virtual bool Initialize(const XmlNode *configNode);
    virtual BEP_STATUS_TYPE ProgramVIN(string moduleVin = "");
protected:
    virtual const INT32& GetDTCCountIndex(void);
    virtual const INT32& GetBytesPerDTC(void);
    virtual const INT32& GetFirstDTCByteIndex(void);
    virtual const INT16& GetFirstFaultRegister(void);
    virtual const INT32& GetNumberOfFaultRegisters(void);
    const bool& FaultReadInProgress(void);
    const bool& GetStopNoDTCs(void);
    void SetReadFaultsInProgress(void);
    void SetReadFaultsComplete(void);
    XmlNodeMap m_faultByteCodes;
    XmlNode *m_moduleConfig;
private:
    void SetDTCCountIndex(const INT32 &index);
    void SetBytesPerDTC(const INT32 &bytesPerDTC);
    void SetFirstDTCByteIndex(const INT32 &firstDTCByteIndex);
    void SetFirstFaultRegister(const INT16 &firstFaultRegister);
    void SetNumberOfFaultRegisters(const INT32 &numberOfFaultRegisters);
    void SetStopNoDTCs(const string &stopDTCRead);
    INT32 m_dtcCountIndex;
    INT32 m_bytesPerDTC;
    INT32 m_firstDTCByteIndex;
    bool m_readingFaults;
    INT16 m_firstFaultRegister;
    INT32 m_numberOfFaultRegisters;
    bool m_stopNoDTCs;
};
//-----------------------------------------------------------------------------
#endif //PowerSteeringModule_h
