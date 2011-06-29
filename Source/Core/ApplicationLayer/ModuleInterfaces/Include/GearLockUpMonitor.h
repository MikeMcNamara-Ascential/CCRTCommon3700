//******************************************************************************
// Description:
//	Thread to monitor gears from the transmission control module.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/GearLockUpMonitor.h $
// 
// 1     11/01/07 1:37p Derickso
// Corrected header comments.
// 
// 1     10/05/07 7:10a Derickso
// Updated header comments to dixygen format.
// 
// 2     8/05/04 2:37a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 1     4/28/04 11:02p Cward
//
// 1     4/12/04 4:07p Derickso
// Combine the functionality of GearMonitor and
// TorqueConverterLockupMinitor.  This takes advantage of the situation
// where both pieces of data are available in the same message response
// from the module.
//
// 1     1/27/04 9:29a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
//
// 4     11/29/03 1:26p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
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
// 2     8/14/03 8:23a Derickso
// Split implementation out to cpp file.
//
// 1     5/27/03 3:31p Derickso
// Periodic backup.
//
// 1     4/25/03 11:11a Derickso
// Periodic backup.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GearLockUpMonitor_h
#define GearLockUpMonitor_h
//==============================================================================

#include "BackgroundMonitor.h"

// Forward declaration of the GenericTransmissionModuleTemplate class for the compiler
template <class ProtocolFilterType>
class GenericTransmissionModuleTemplate;

/**
 * Transmission Gear / Torque Converter Clutch Lockup Monitor.
 * This thread monitors the transmission and records all gears and
 * all torque converter clutch lockup states observed during the vehicle test.
 *
 * @author Dan Erickson
 * @see AtomicThread
 * @see GenericTransmissionModule
 * @since April 22, 2003
 */
template <class ProtocolFilterType>
class GearLockUpMonitor : public BackgroundMonitor
{
public:
	/**
	 * Class Constructor.
	 * Construct the gear / TCC Lockup monitor with the given module interface
	 * and Protocol Filter object.
	 *
	 * @param module Vehicle Module Interface.
	 * @param filter Vehicle communication ProtocolFilter object.
	 * @param delay  Time delay between each read of the current gear.
	 */
	GearLockUpMonitor(GenericTransmissionModuleTemplate<ProtocolFilterType> *module,
					  ProtocolFilterType *filter,
		              const INT32 delay);
	
	/**
	 * Class destructor.
	 */
	virtual ~GearLockUpMonitor();

protected:
	/**
     * Monitor module data.
     * <p><b>Description:</b><br>
     * The transmission gear and torque converter clutch lockup data will be read from the module.
     * the data will then be processed to determine if all transmission gears have been observed as well as all TCC
     * lock states.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTransmissionModuleTemplate::MonitorTransmissionData() </li>
     *      </ul>
	 *
     * @see GenericTransmissionModuleTemplate::MonitorTransmissionData
	 * @return Status of monitoring the module data.
	 */
	virtual BEP_STATUS_TYPE MonitorData(void);

private:
	/**
	 * Module interface object to use to read gears from the transmission control module.
	 */
	GenericTransmissionModuleTemplate<ProtocolFilterType> *m_module;
};

//*************************************************************************
#endif  //GearLockUpMonitor_h
