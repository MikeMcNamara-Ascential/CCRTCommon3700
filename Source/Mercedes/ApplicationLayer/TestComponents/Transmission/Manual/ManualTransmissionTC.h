//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/TestComponents/Transmission/Include/ManualTransmissionTC.h 1     2/22/05 3:27p Derickso $
//
// FILE DESCRIPTION:
//  42RE Transmission Test Component.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Transmission/Include/ManualTransmissionTC.h $
// 
// 1     2/22/05 3:27p Derickso
// New transmission types.
// 
// 1     11/08/04 1:17p Mkelly
// 
// 1     11/01/04 3:26p Mkelly
// 
// 1     8/04/04 12:41p Derickso
// Common DCX trnasmission test component header files.
// 
// 2     1/29/04 8:53a Derickso
// Updated 2-3 shift test to latch data when an RPM drop is detected whlie
// the vehicle is in second gear and a steady throttle is applied.
// 
// 1     1/27/04 9:35a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 3     1/20/04 5:49a Derickso
// Moved loading of external fault list to GenericTC class.
// NOTE:  Faults node in test component config file is no longer
// supported.
// 
// 2     1/10/04 10:20a Derickso
// Latest updates from development testing.
// 
//*************************************************************************
#include "GenericTC.h"	// inherited from class
//*************************************************************************
#ifndef ManualTransmissionTC_H
#define ManualTransmissionTC_H

/**
 * Manual Transmission Test Component.  This
 * Class tests the Manual Transmission to ensure it works
 * properly as require by the Process Standard
 * 
 * @author Greg Pattison
 * @version Version 1.0
 * @see GenericTransmissionTCTemplate
 * @since September 10, 2003
 */
class ManualTransmissionTC : public GenericTC
{
public:
	/**
	* Class Constructor.
	* @since Version 1.0
	*/
	ManualTransmissionTC();
	/**
	 * Class Destructor
	 * @since Version 1.0
	 */
	virtual ~ManualTransmissionTC();

	//Overloaded initialization 
	void Initialize(const string &fileName);
	void Initialize(const XmlNode *config);

	/**
	 * This is the method that will handle the execution of each
	 * test step that has been specified by the system for each
	 * test objective that is passed to it.  This must be overloaded
	 * to execute the component tests.
	 *
	 * @param value This is the value that was commanded by
	 * the test sequencer.
	 * @return The status of the overall objective.
	 * @since Version 1.0
	 */
	virtual const string CommandTestStep(const string &value);

protected:

};
//-------------------------------------------------------------------------------------
#endif	// ManualTransmissionTC_H
