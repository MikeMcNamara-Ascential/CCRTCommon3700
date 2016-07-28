//******************************************************************************
// Description:
//  ABS Test Component specialized for TRW CAN applications
//
//==============================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/TestComponents/Brake/TRW/CAN/FordTRWCANABSTC.h $
// 
// 1     2/07/11 4:48p Mmcnamar
// backup c drive source for cross compiler
// 
// 1     5/10/10 2:07p Mmcnamar
// initial check-in for new core.
// 
// 3     6/13/05 9:12a Bmeinke
// Inside the CloseRelay() method: changed GetInfo("CloseRelay") to
// GetInfo("RelayOn")
// Added test step methods for FrontInletsOn and RearInletsOn
// 
// 2     6/09/05 4:45p Bmeinke
// Removed the individual Reduction and Recovery methods and replaced them
// with generic versions
// Changes the names of the valve firing delay parameters
// 
// 1     5/04/05 12:36p Bmeinke
// TRW CAN ABS component files with overloaded valve firing sequence
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _FORDTRWCANABSTC_H_INCLUDED_
#define _FORDTRWCANABSTC_H_INCLUDED_

#include "FordABSTCTemplate.h"
#include "FordABSModuleTemplate.h"
#include "FordCanProtocolFilter.h"

/**
 * Typedef'd because I'm too lazy to have to keep typing 
 * FordABSTCTemplate<FordABSModuleTemplate<FordCanProtocolFilter>>
 */
typedef FordABSModuleTemplate<FordCanProtocolFilter>	CanAbsModule_t;
/**
 * Typedef'd because I'm too lazy to have to keep typing 
 * FordABSTCTemplate<FordABSModuleTemplate<FordCanProtocolFilter>>
 */
typedef FordABSTCTemplate<CanAbsModule_t>				FordCanAbsComp_t;


class FordTRWCANABSTC : public FordCanAbsComp_t
{
public:
	/**
	 * Constructor
	 */
	FordTRWCANABSTC();

	/**
	 * Destructor
	 */
	virtual ~FordTRWCANABSTC();

	/**
	 * This is the method that will handle the execution of each
	 * test step that has been specified by the system for each
	 * test objective that is passed to it.  This must be overloaded
	 * to execute the component tests.
	 *
	 * @param value This is the value that was commanded by
	 * the test sequencer.
	 * @return The status of the overall objective.
	 */
	virtual const string CommandTestStep(const string &value);
	
protected:

	/**
	 * Isolates all ABS valves prior to an ABS valve firing sequence
	 * 
	 * @return The test result
	 * @since 10 July 2003
	 */
	virtual string IsolateAll();

	/**
	 * Closes the ABS pump motor relay
	 * 
	 * @return Result of the operation
	 */
	virtual std::string CloseRelay();
	
	/**
	 * Perform a reduction sequence at the specified wheel.
	 *
	 * @param wheel      Wheel to perform reduction on.
	 * @param startIndex Reduction start index for the specified wheel.
	 * @param stopIndex  Reduction stop index for the specified wheel.
	 *
	 * @return Result of the reduction.
	 */
	virtual string Reduction(const string &wheel, INT32 &startIndex, INT32 &stopIndex);

	/**
	 * Perform the recovery sequence at the specified wheel.
	 *
	 * @param wheel      Which wheel to recover.
	 * @param startIndex Recovery start index.
	 * @param stopIndex  Recovery end index.
	 *
	 * @return Result of the test.
	 */
	virtual string Recovery(const string &wheel, INT32 &startIndex, INT32 &stopIndex);

	/**
	 * Open front ABS outlet valves
	 *
	 * @return Result of the test.
	 * @since Version 1.0
	 */
	virtual string FrontInletValvesOn();

	/**
	 * Open rear ABS outlet valves
	 *
	 * @return Result of the test.
	 * @since Version 1.0
	 */
	virtual string RearInletValvesOn();
	
	/**
	 * Open all ABS outlet valves
	 *
	 * @return Result of the test.
	 * @since Version 1.0
	 */
	virtual string AllOutletValvesOn();
	
	/**
	 * Closes all ABS outlet valves
	 *
	 * @return Result of the test.
	 * @since Version 1.0
	 */
	virtual string AllOutletValvesOff();
	
	/**
	 * Perform a high precision delay via TimerTimeout
	 * 
	 * @param delayMs Number of milliseconds to delay
	 */
	void Delay( int delayMs) const;
};


#endif // _FORDTRWCANABSTC_H_INCLUDED_

