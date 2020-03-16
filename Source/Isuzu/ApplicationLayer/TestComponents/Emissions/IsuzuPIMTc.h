//******************************************************************************
// Description:
// Test steps specific to Isuzu Emissions testing.
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
#ifndef IsuzuPIMTc_h
#define IsuzuPIMTc_h
//-------------------------------------------------------------------------------------------------
#include "GenericEmissionsTCTemplate.cpp"


//-------------------------------------------------------------------------------------------------
template<class ModuleType>
class IsuzuPIMTc : public GenericEmissionsTCTemplate<ModuleType>
{
public:
    IsuzuPIMTc();
    virtual ~IsuzuPIMTc();
    /**
     * Execute the specified test step.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the
     * system for each test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     *
     * @param value  The value that was commanded by the test sequencer.
     *
     * @return The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);
    /**
     * Checks for a locked module before trying to clear faults
     * 
     * @return Result of clearing the faults from the module
     */
    virtual string ClearFaults(void);

    virtual string BrakeApplySensorLearn(void);

    virtual string UnlockPIM(void);



protected:



private:
};
//-------------------------------------------------------------------------------------------------
#endif //IsuzuPIMTc_h
