//******************************************************************************
// Description:
//	Thread to monitor throttle position sensor.
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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/ThrottlePositionMonitor.h $
// 
// 1     1/10/08 8:46a Derickso
// Updated comments to doxygen format.
// 
// 4     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 3     5/25/04 2:43p Cward
// Constructor updated to take Message Tag to be used.  Monitor Data
// function updated to send the message tag to the read Throttle Position
// function. member variable added.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:32p Cward
//
// 1     10/24/03 3:16p Canyanwu
// Start of DCX working directory
//
// 3     10/20/03 9:31a Derickso
// Added logic to prevent thread from being deleted while the
// communication port is locked.
// Added a flag that the instantiateing object can use to determine if the
// thread has completed and exited.  These threads typically self-delete
// when commanded to exit.
//
// 2     8/26/03 10:22a Derickso
// Updated for new naming scheme.
//
// 1     8/21/03 8:57a Derickso
// 05WK CCRT Project
//
// 5     8/20/03 6:41a Derickso
// Recombined cpp and h files.  Compiler does not support the
// implementation being seperate for template classes.
//
// 3     8/14/03 10:00a Derickso
// Split implementation out to cpp file.
//
// 2     6/09/03 8:01a Derickso
// Templatized the ThrottlePositionMonitor class.
//
// 1     5/27/03 3:31p Derickso
// Periodic backup.
//
// 1     4/25/03 11:12a Derickso
// Periodic backup.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef ThrottlePositionMonitor_h
#define ThrottlePositionMonitor_h
//==============================================================================

#include <string>
#include "BackgroundMonitor.h"

// Forward declaration for the compiler
template <class ProtocolFilterType>
class GenericEmissionsModuleTemplate;

/**
 * ThrottlePositionMonitor.
 * Thread to monitor the throttle position sensor.
 *
 * @author Dan Erickson
 * @see BackgroundMonitor
 * @see GenericEmissionsModuleTemplate
 */
template <class ProtocolFilterType>
class ThrottlePositionMonitor : public BackgroundMonitor
{
public:
	/**
	 * Class constructor.
	 * <br>The pointer to the emissions module will be saved off for later use.
     * <p>
	 * @param module     Pointer to the module that started this thread.
	 * @param filter     ProtocolFilter object to use for vehicle communications.
	 * @param messageTag Message tag to use to read the current throttle position.
	 * @param delay      Time to delay between throttle position sensor reads.
	 */
	ThrottlePositionMonitor(GenericEmissionsModuleTemplate<ProtocolFilterType> *module,
							ProtocolFilterType *filter, 
                            const string &messageTag,
							const INT32 delay);
	/**
	 * Class Destructor.
	 */
	virtual ~ThrottlePositionMonitor();

protected:
	/**
     * Monitor the throttle poisiton.
     * <p><b>Description:</b><br>
     * The module interface will be called to monitor the throttle position.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> ReadThrottlePosition() </li>
     *      </ul>
	 *
	 * @return Status of monitoring the throttle position sensor.
	 */
	virtual BEP_STATUS_TYPE MonitorData(void);

private:
	/**
	 * Pointer to module to use for reading the throttle position sensor.
	 */
	GenericEmissionsModuleTemplate<ProtocolFilterType> *m_module;
	/**
	 * Message tag to use to read the throttle position sensor.
	 */
	string m_messageTag;
};

//*************************************************************************
#endif //ThrottlePositionMonitor_h
