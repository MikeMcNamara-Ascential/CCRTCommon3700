//******************************************************************************
// Description:
// Test steps specific to Isuzu Mimamori Module testing.
//
//===========================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef MimamoriTc_h
#define MimamoriTc_h
//-------------------------------------------------------------------------------------------------
#include "GenericTCTemplate.cpp"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
class MimamoriTc : public GenericTCTemplate<ModuleType>
{
public:
	MimamoriTc();
	virtual ~MimamoriTc();



protected:
	/**
	 * Verify the diagnostic cable is connected.
	 * <p><b>Category:</b> Utility
	 * <p><b>Description:</b><br>
	 * Determine if the cable is connected.  If the cable is not connected, the operator will be prompted to connect
	 * the diagnostic cable.<br>
	 * If this function is run as a background test, it will first check to see
	 * if the cable is connected.  If not, it then checks to see if all the
	 * foreground tests are done.  It will continue to loop until either the
	 * foreground tests are complete, StatusCheck() fails, or the cable is
	 * connected.  If the foreground tests are complete and the cable is not
	 * connected, the function will attempt to lock the prompt server and will
	 * continue to loop as above.  Once the prompt server has been locked, the
	 * function acts as a foreground test would.
	 * <p>
	 *
	 * @return Flag indicating if the diagnostic cable is connected to the vehicle.
	 */
	virtual const bool CheckCableConnect(void);

	/**
	 * Check if the diagnostic cable is connected.
	 * <p><b>Category:</b> Utility
	 * <p><b>Description:</b><br>
	 * Check to see if the diagnostic cable is connected.  The subscribed data tag CBL_CONNECT_DATA_TAG will be checked
	 * to determine if the cable has been connected.
	 * <p>
	 *
	 * @return Flag indicating if the cable is connected.
	 */
	virtual const bool IsCableConnected(void);

    virtual const string CommandTestStep(const string &value);

    const string CheckDataRecordMemory();
 
    bool GetMimamoriData(const string &messageTag);

    const string CheckFuelEconomy();

    const string CheckEdrData();

    const string CheckTotalMileage();

    const string ClearHistory();

    string ClearFaults();

    string ReadFaults();


    bool m_dtcCleared;

    bool m_dataCleared;

};
//-------------------------------------------------------------------------------------------------
#endif //MimamoriTc_h
