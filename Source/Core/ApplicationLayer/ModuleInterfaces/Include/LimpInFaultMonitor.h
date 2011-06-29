//******************************************************************************
// Description:
//	Thread to monitor Faults and Limp-In Status from the transmission control module.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/LimpInFaultMonitor.h $
// 
// 1     11/21/07 8:17a Derickso
// Updated commets to doxygen format.
// 
// 3     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:32p Cward
//
// 3     11/13/03 6:06p Derickso
// Latest updates from testing at JTE.
//
// 2     8/27/03 7:29a Derickso
// Corrected compiler errors.
// Compiles, not tested.
//
// 1     8/21/03 8:57a Derickso
// 05WK CCRT Project
//
// 4     8/20/03 6:41a Derickso
// Recombined cpp and h files.  Compiler does not support the
// implementation being seperate for template classes.
//
// 2     8/14/03 8:31a Derickso
// Split implementation out to cpp file.
//
// 1     5/27/03 3:31p Derickso
// Periodic backup.
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef LimpInFaultMonitor_h
#define LimpInFaultMonitor_h
//==============================================================================

#include "BackgroundMonitor.h"

// Forward declarations
template <class ProtocolFilterType>
class GenericTransmissionModuleTemplate;


/**
 * Limp-In/Fault status monitor.<br>
 * This thread runs in the background periodically checking the transmission
 * module for faults and checking the limp-in status.
 * If the module enters Limp-In mode or records faults, vehicle testing
 * will be aborted with the proper message displayed for the operator.
 *
 * @author Dan Erickson
 * @see GenericTransmissionModuleTemplate
 * @see AtomicThread
 * @see IProtocolFilter
 * @since May 26, 2003
 */
template <class ProtocolFilterType>
class LimpInFaultMonitor : public BackgroundMonitor
{
public:
	/**
	 * Class Constructor.
	 *
	 * @param module Transmission module that started this thread.
	 * @param filter Communication object to use for monitoring the Limp-In status
	 *               and Faults from the module.
	 * @param delay  Time in ms to wait between checks.
	 */
	LimpInFaultMonitor(GenericTransmissionModuleTemplate<ProtocolFilterType> *module,
					   ProtocolFilterType *filter, const INT32 delay);

	/**
	 * Class destructor.
	 */
	virtual ~LimpInFaultMonitor();


protected:
	/**
     * Monitor the data.
     * <p><b>Description:</b><br>
     * The limp-in status and fault data will be read from the module and analyzed to determine if a limp-in 
     * condition is present or if there are faults recorded in the module.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> MonitorLimpInDetailStatus() </li>
     *      </ul>
	 *
	 * @return Status of monitoring the module data.
	 */
	virtual BEP_STATUS_TYPE MonitorData(void);


private:
	/**
	 * Module interface object to use to read limp-in status and fault data from the transmission
	 * control module.
	 */
	GenericTransmissionModuleTemplate<ProtocolFilterType> *m_module;
};

//*************************************************************************
#endif //LimpInFaultMonitor_h
