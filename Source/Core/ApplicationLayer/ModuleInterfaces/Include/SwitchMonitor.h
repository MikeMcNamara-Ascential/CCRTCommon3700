//******************************************************************************
// Description:
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/SwitchMonitor.h $
// 
// 1     1/08/08 12:14p Derickso
// Updated comments to doxygen format.
// 
// 3     6/09/04 10:49p Cward
// Updated includes due to change in back ground monitor. Added new
// standard header comment block
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef SWITCHMONITOR_H
#define SWITCHMONITOR_H
//==============================================================================
#include "BackgroundMonitor.h"
#include "IGenericModule.h"
#include "IProtocolFilter.h"

/**
 * Switch Monitor.
 * This class will monitor switches in a module.
 * The thread will periodically call the instantiating objects
 * ReadSwitches() method to perform the desired work.
 *
 * @author Atomic Object
 * @author Dan Erickson
 * @see BackgroundMonitor
 * @see IGenericModule
 * @see IProtocolFilter
 * @since March 28, 2003
 */
class SwitchMonitor : public BackgroundMonitor
{
	friend class SwitchMonitorTest;
	
 public:
	/**
	 * Construct the switch monitor with given module, protocol filter and
	 * delay between calls.  The pointer to the intantiating module interface will be stored.
	 *
	 * @param module Module that instantiated the switch monitor.
	 * @param filter Communication object to use for reading the switches.
	 * @param delay  Time interval in ms between switch reads.
	 */
	SwitchMonitor(IGenericModule *module, IProtocolFilter *filter, const INT32 delay);
	/**
	 * Class Destructor.
	 */
	virtual ~SwitchMonitor();

protected:
	/**
     * Monitor the switch data from the module.
     * <p><b>Description:</b><br>
     * The instantiating module interface will be used to read and process the switch data from the module.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ReadSwitches() </li>
     *      </ul>
	 *
	 * @return Status of monitoring the switch data from the module.
	 */
	virtual BEP_STATUS_TYPE MonitorData(void);

 private:
	/**
	 * Pointer to the module interface that instantiated this object.
     * @see IGenericModule
	 */
	IGenericModule *m_module;
};

//=============================================================================
#endif

