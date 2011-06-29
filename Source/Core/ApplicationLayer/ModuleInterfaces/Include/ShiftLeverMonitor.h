//******************************************************************************
// Description:
//	Thread to monitor shift lever position from the transmission control module.
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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/ShiftLeverMonitor.h $
// 
// 1     1/08/08 9:28a Derickso
// Updated comments to doxygen format.
// 
// 1     11/11/05 4:50p Cward
// Added New Shift Level Monitor
//
// 1     8/24/05 11:15a Derickso
// New monitor class.
//
// 1     3/24/05 5:09p Canyanwu
// PT & PM Platforms
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef ShiftLeverMonitor_h
#define ShiftLeverMonitor_h
//==============================================================================
#include "BackgroundMonitor.h"

// Forward declaration for the compiler
template <class ProtocolFilterType>
class GenericTransmissionModuleTemplate;

/**
 * Transmission ShiftLever Monitor.
 * This thread monitors the transmission and records all Shift Lever Positions observed
 * during the vehicle test.
 *
 * @author Greg Pattison
 * @see BackgroundMonitor
 * @see GenericTransmissionModule
 * @since January 13, 2005
 */
template <class ProtocolFilterType>
class ShiftLeverMonitor : public BackgroundMonitor
{
public:
    /**
     * Class Constructor.
     * Construct the ShiftLever monitor with the given module interface and Protocol
     * Filter object.
     *
     * @param module Vehicle Module Interface.
     * @param filter Vehicle communication ProtocolFilter object.
     * @param delay  Time delay between each read of the current shift lever position.
     */
    ShiftLeverMonitor(GenericTransmissionModuleTemplate<ProtocolFilterType> *module,
                      ProtocolFilterType *filter,
                      const INT32 delay);
    /**
     * Class destructor.
     */
    virtual ~ShiftLeverMonitor();

protected:
    /**
     * Monitor the transmission shift lever data.
     * <p><b>Description:</b><br>
     * The module interface will be used to monitor the transmission shift lever data.
     * <p>
     * <b>Transmission Module Interface Functions:</b>
     *      <ul>
     *      <li> MonitorTransmissionShiftLever() </li>
     *      </ul>
     *
     * @return Status of monitoring the module data.
     */
    virtual BEP_STATUS_TYPE MonitorData(void);

private:
    /**
     * Module interface object to use to monitor the shift lever position data from the transmission control module.
     */
    GenericTransmissionModuleTemplate<ProtocolFilterType> *m_module;
};

//*************************************************************************
#endif  //ShiftLeverMonitor_h
