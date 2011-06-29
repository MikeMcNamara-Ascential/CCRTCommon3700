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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/LimpInDetailMonitor.h $
// 
// 1     11/21/07 8:17a Derickso
// Updated commets to doxygen format.
// 
// 1     10/30/07 1:33p Rwiersem
// Changes for the 07103101 core release:
// 
// - Initial version.
// 
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef LimpInDetailMonitor_h
#define LimpInDetailMonitor_h
//==============================================================================

#include "BackgroundMonitor.h"

// Forward declarations
template <class ProtocolFilterType>
class DcxAhstTransmissionModule;


/**
 * Limp-In detail status monitor.<br>
 * This thread runs in the background periodically checking the transmission
 * module for faults and checking the limp-in status.
 * If the module enters Limp-In mode or records faults, vehicle testing
 * will be aborted with the proper message displayed for the operator.
 *
 * @author Bryan Martin
 * @see GenericTransmissionModuleTemplate
 * @see IProtocolFilter
 * @since August 23, 2007
 */
template <class ProtocolFilterType>
class LimpInDetailMonitor : public BackgroundMonitor
{
public:
	/**
	 * Class Constructor.
	 *
	 * @param module Transmission module that started this thread.
	 * @param filter Communication object to use for monitoring the Limp-In status and Faults from the module.
	 * @param delay  Time in ms to wait between checks.
	 */
	LimpInDetailMonitor(DcxAhstTransmissionModule<ProtocolFilterType> *module,
						ProtocolFilterType *filter, 
                        const INT32 delay);
	/**
	 * Class destructor.
	 */
	virtual ~LimpInDetailMonitor();


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
	 * Module interface object to use to read limp-in and fault data from the transmission control module.
	 */
	DcxAhstTransmissionModule<ProtocolFilterType> *m_module;
};

//*************************************************************************
#endif //LimpInDetailMonitor_h
