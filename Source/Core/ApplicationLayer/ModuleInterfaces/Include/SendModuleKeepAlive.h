//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/SendModuleKeepAlive.h 1     1/08/08 8:56a Derickso $
//
// FILE DESCRIPTION:
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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/SendModuleKeepAlive.h $
// 
// 1     1/08/08 8:56a Derickso
// Updated comments to doxygen format.
// 
// 4     10/30/07 1:35p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     10/25/06 3:10p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a timer to tell when the last transmit message was sent.
// This
//       allows the keep alive message to be sent when needed instead of
// on a
//       specific time interval.
// 
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:32p Cward
//
// 1     10/24/03 3:16p Canyanwu
// Start of DCX working directory
//
// 4     10/20/03 9:31a Derickso
// Added logic to prevent thread from being deleted while the
// communication port is locked.
// Added a flag that the instantiateing object can use to determine if the
// thread has completed and exited.  These threads typically self-delete
// when commanded to exit.
//
// 3     9/22/03 4:42p Derickso
// Updated to use GenericTC instead of TestComponent
//
// 2     8/27/03 7:54a Derickso
// Latest version.
//
// 2     8/12/03 2:41p Khykin
// Updated with new directory update changes.
//    Revision 1.1  2003/05/27 13:29:15  tenharmsel
//    Moving customer specific module interfaces into a Customers directory
//
//    Revision 1.1.1.1  2003/05/07 15:11:59  tenharmsel
//    Bringing in the GenericModuleInterfaces
//
//    Revision 1.2  2003/02/27 04:31:10  bereza
//    added SwitchMonitor functionality to GenericModule and GenericTestCoponent
//
//    Revision 1.1  2003/02/26 20:55:04  bereza
//    added SendModuleKeepAlive class and methods to GenericModule
//
//
// 1     1/30/03 8:24a Derickso
// Created.
//
//*************************************************************************
#ifndef SendModuleKeepAlive_h
#define SendModuleKeepAlive_h
//*************************************************************************
#include "BackgroundMonitor.h"

/**
 * SendModuleKeepAlive.
 * This class uses the provided ProtocolFilter for sending the keep alive message to
 * the vehicle module periodically.  This class assumes that module communications have
 * already been established.
 *
 * @author Dan Erickson
 * @see BackgroundMonitor
 * @see ProtocolFilter
 * @since January 13, 2003
 */
class SendModuleKeepAlive : public BackgroundMonitor
{
    friend class SendModuleKeepAliveTest;

public:
    /**
     * Class Constructor
     *
     * @param vehicleComm
     *               Communication object to use.
     * @param keepAliveMessageTag
     *               Message tag for keep alive message.
     * @param keepAliveTime
     *               Time delay between keep alive messages (in ms).
     * @param responseRequired
     *               Flag to indicate if a response to the keep alive message will be received.
     */
    SendModuleKeepAlive(IProtocolFilter *vehicleComm,
                        const string keepAliveMessageTag,
                        const INT32 keepAliveTime,
                        const bool responseRequired);
    /**
     * Class destructor
     */
    virtual ~SendModuleKeepAlive(void);

protected:
    /**
     * Send the keep alive maessage to the vehicle.
     * <p><b>Description:</b><br>
     * The keep alive message will be sent to the module.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SendKeepAlive() </li>
     *      <li> GetKeepAliveMessageTag() </li>
     *      <li> IsResponseRequired() </li>
     *      </ul>
     *
     * @return Status of sending the keep alive message to the module.
     */
    virtual BEP_STATUS_TYPE MonitorData(void);
    /**
     * Transmit the keep alive message to the module.
     * <p><b>Description:</b><br>
     * The last message transmit time will be checked to determine if enough time has elapsed to require a keep alive
     * message to be sent to the module.  Typically, any message to the module will serve to keep the module in 
     * diagnostic mode.  If no message has been sent within the module timeout time, a keep alive message will be
     * transmitted to the module.
     * <p>
     * <b>Protocol Filter Functions Called:</b>
     *      <ul>
     *      <li> GetLastTxTime() </li>
     *      <li> GetModuleData() </li>
     *      <li> SendMessage() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetInterMessageGap() </li>
     *      </ul>
     *
     * @return Status of sending the keep alive message to the module.
     */
    const BEP_STATUS_TYPE SendKeepAlive(const string &keepAliveMessageTag,
                                        const bool &keepAliveResponseRequired);
    /**
     * Return the tag for the keep alive message.
     * <p><b>Description:</b><br>
     * The keep alive message tag will be returned to the calling function.
     * <p>
     * @return Tag for the keep alive message.
     */
    const string &GetKeepAliveMessageTag(void);
    /**
     * Indicate if a response will be sent from the module for the keep alive message.
     * <p><b>Description:</b><br>
     * The flag indicating if the module will send a response to the keep alive message will be returned to the 
     * calling function.
     * <p>
     * @return Flag indicating if the module will send a response to the keep alive message.
     */
    const bool &IsResponseRequired(void);

private:
    /**
     * Tag for the keep alive message.
     */
    std::string m_keepAliveMessageTag;
    /**
     * Flag to indicate if a response will be sent from the module.
     */
    bool m_responseRequired;
    /**
     * The required keep alive message time (ms).
     */
    const INT32 m_keepAliveTime;
};

#endif //SendModuleKeepAlive

