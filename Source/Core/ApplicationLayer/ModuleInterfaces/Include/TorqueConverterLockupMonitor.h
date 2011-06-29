//******************************************************************************
// Description:
//	Thread to monitor torque converter clutch lockup status from the
//  transmission control module.
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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/TorqueConverterLockupMonitor.h $
// 
// 1     1/10/08 11:29a Derickso
// Updated comments to doxygen format.
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
// 2     8/14/03 11:18a Derickso
// Split implementation out to cpp file.
//
// 1     5/27/03 3:31p Derickso
// Periodic backup.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef TorqueConverterLockupMonitor_h
#define TorqueConverterLockupMonitor_h
//==============================================================================

#include "BackgroundMonitor.h"

// Forward declarations
template <class ProtocolFilterType>
class GenericTransmissionModuleTemplate;

/**
 * Background monitor to look for the various torque converter clutch
 * lockup states.  Once all three states are seen, this thread will
 * stop running.  All results are stored in the module interface that
 * instantiated this thread.
 *
 * @author Dan Erickson
 * @see BackgroundMonitor
 * @see GenericTransmissionModuleTemplate
 */
template <class ProtocolFilterType>
class TorqueConverterLockupMonitor : public BackgroundMonitor
{
public:
	/**
	 * Class Constructor.<br>
     * The pointer to the module interface will be saved off into a member variable.
	 *
	 * @param module Vehicle Module Interface.
	 * @param filter Vehicle communication ProtocolFilter object.
	 * @param delay  Time delay (ms) between each read of the torque converter lockup statuses.
	 */
	TorqueConverterLockupMonitor(GenericTransmissionModuleTemplate<ProtocolFilterType> *module,
								 ProtocolFilterType *filter, const INT32 delay);
	/**
	 * Class destructor.
	 */
	virtual ~TorqueConverterLockupMonitor();

protected:
	/**
     * Monitor the torque converter lockup state.
     * <p><b>Description:</b><br>
     * The module interface will be called to monitor the torque converter lockup states.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> MonitorTorqueConverterLockupStatus() </li>
     *      </ul>
	 *
	 * @return Status of monitoring the module data.
	 */
	virtual BEP_STATUS_TYPE MonitorData(void);

private:
	/**
	 * Module interface object to use to read torque converter lockup states from the transmission control module.
	 */
	GenericTransmissionModuleTemplate<ProtocolFilterType> *m_module;
};

//*************************************************************************
#endif //TorqueConverterLockupMonitor_h
