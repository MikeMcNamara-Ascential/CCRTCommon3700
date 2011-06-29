//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/Include/SciProtocolFilter.h 2     10/25/06 3:31p Rwiersem $
//
// FILE DESCRIPTION:
//  SCI Protocol Filter for communicating with vehicle modules.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/Include/SciProtocolFilter.h $
// 
// 2     10/25/06 3:31p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a timer to tell when the last transmit message was sent.
// This
//       allows the keep alive message to be sent when needed instead of
// on a
//       specific time interval.
// 
// 1     4/28/04 11:06p Cward
// Added to core
// 
// 2     3/02/04 12:35p Derickso
// Updated to use BepMutex instead of BepSemaphore.
// 
// 1     1/27/04 9:31a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 3     1/20/04 6:41a Derickso
// Changed Semaphore to BepSemaphore.
// 
// 2     1/10/04 9:02a Derickso
// Latest updates from development testing.
// 
// 1     12/17/03 11:53a Derickso
// Created.  New SCI protocol filter files from Greg.
// 
//*************************************************************************
#ifndef SciProtocolFilter_h
#define SciProtocolFilter_h

#include "ProtocolFilter.h"

/**
 * SCI Protocol filter.
 * This class will send and receive SCI messages.
 * The ExtractModuleData() method has been instantiated to strip the header and
 * checksum from the module response.
 * 
 * @author Greg Pattison
 * @version Version 1.0
 * @see ProtocolFilter
 * @since September 24, 2003
 */
class SciProtocolFilter : public ProtocolFilter
{
public:
    /**
     * Data type definition for variable used to control
     * locking of the port while changing baud rate
     * @since Version 1.0
     */
    typedef enum
    {
        LockBeforeChange, UnlockAfterChange
    } LockControl;
    
    /**
     * Class Constructor
     * 
     * @param lastTxTime Variable used to hold the time of the last transmit operation. This is
     *                   designed to be a shared timestamp that all the protocol filters for
     *                   this module interface use so the module keep alive knows how long
     *                   it has been since the last string was sent to the module so as not to
     *                   send unnecessary keep alive messages
     * @param commsInUse Semaphore to allow only one thread in the process to utilize the comm port at a time.
     * @since Version 1.0
     */
    SciProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar = NULL, BepMutex *commsInUse = NULL);
    
    /**
     * Class destructor.
     * @since Version 1.0
     */
    virtual ~SciProtocolFilter();
    /**
     * Initialize the sci protocol filter.
     *
     * @param config The configuration to use for the channel initialization.
     * @return true if the initialization is successful.
     * @since Version 1.0
     */
    virtual bool Initialize(const XmlNode *config);
    /**
     * Get the Baud rate for the message based on the passed in message tag.
     * 
     * @param messageTag Message Tag to look up.
     * @param baudRate
     * 
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE GetBaudRate(const std::string &messageTag, speed_t &baudRate);
    /**
     * Send a message to a vehicle module.
     * 
     * @param message Message to be sent.
     * @return Status of the send.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(SerialString_t &message);
    /**
     * Send a message to vehicle module.
     * 
     * @param messageTag Name of message to be retrieved and sent.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag);
    /**
     * Send a message to a vehicle module.
     * 
     * @param messageTag Name of the message to be sent.
     * @param args       Extra data to be added to the message.
     * @return Status of the opeation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag, SerialArgs_t &args);
    /**
     * Send a message to the module and read the response.
     * 
     * @param messageTag Tag of message to send.
     * @param reply      Reply from the module.
     * @param args       Optional data to be written to the module.
     * 
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE GetModuleData(std::string messageTag, SerialString_t &reply, 
                                                SerialArgs_t *args = NULL);
    /**
     * Check the required transmit and receive baud rate
     * for the transmit message.  If the serial port does
     * not match, change the module baud rate and then
     * change the serial port baud rate.
     * 
     * @param messageTag Tag of message to send
     * 
     * @return operation status
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SciProtocolFilter::SetMessageSpeed(const std::string &messageTag);
    /**
     * Extract the data from the response.
     * 
     * @param moduleResponse
     *               Response from the module.
     * 
     * @return The data that was included in the response.
     * @since Version 1.0
     */
    const SerialString_t ExtractModuleData(SerialString_t &moduleResponse);
    
    /**
     * Return the baud rate for High Speed SCI
     * Messages
     * 
     * @return high speed baud rate
     */
    inline speed_t GetHighSpeedMessageBaud()
    {
        return(m_highSpeedBaudRate);
    };
    /**
     * Return the baud rate for Low Speed SCI
     * Messages
     * 
     * @return high speed baud rate
     */
    inline speed_t GetLowSpeedMessageBaud()
    {
        return(m_lowSpeedBaudRate);
    };
protected:
    /**
     * Switch the SCI module to low speed by sending the module
     * a command to switch to low speed comms.  The method checks
     * the module response to ensure it worked correctly.  This
     * method does not lock the serial port.
     * 
     * @return status of the operation
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SciProtocolFilter::SwitchModuleBaudRateToHighSpeed();
    /**
     * Switch the SCI module to high speed by sending the module
     * a command to switch to high speed comms.  The method checks
     * the module response to ensure it worked correctly. This method
     * does not lock the serial port.
     * 
     * @return status of the operation
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SciProtocolFilter::SwitchModuleBaudRateToLowSpeed();
    /**
     * Request to communicate at a baud rate
     * (Send request and change baud rate if necessary).  This
     * method does not lock the serial port.
     * 
     * @param newBaud New baud rate
     * @param tio     I/O Terminal settings read from the device.
     * 
     * @return BEP_STATUS_SUCCESS if baud rate changed or already at baud rate, else BEP_STATUS_SOFTWARE
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SwitchSerialPortBaudRate(const speed_t newBaud, struct termios *tio);
    /**
     * Set the baud rate for High Speed SCI
     * Messages
     * 
     * @return status of operation
     */
    inline void SetHighSpeedMessageBaud(const speed_t &baudRate)
    {
        m_highSpeedBaudRate = baudRate; 
    };
    /**
     * Set the baud rate for Low Speed SCI
     * Messages
     * 
     * @return status of operation
     */
    inline void SetLowSpeedMessageBaud(const speed_t &baudRate)
    {
        m_lowSpeedBaudRate = baudRate; 
    };
private:
    
    /**
     * Used to store the SCI communications high speed
     * baud rate value
     * @since Version 1.0
     */
    speed_t m_lowSpeedBaudRate;
    /**
     * Used to store the SCI communications low speed
     * baud rate
     * @since Version 1.0
     */
    speed_t m_highSpeedBaudRate;
};

#endif //SciProtocolFilter_h
