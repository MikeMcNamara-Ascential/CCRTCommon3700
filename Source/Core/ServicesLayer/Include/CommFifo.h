//******************************************************************************
// Description:
//	Data type definitions for serial communication FIFOs
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Core/ServicesLayer/Include/CommFifo.h $
// 
// 3     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 11:33a Khykin
// Updated with mutex changes.
//
// 2     10/10/03 9:20a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:07p Khykin
// Initial checkin.
//
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
//
// 3     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:38  tbochene
//
//     PooBag
//
//
// 2     2/08/03 9:48a Khykin
// Updated for recursive locking by same thread is enabled.
//
// 1     1/27/03 7:04a Khykin
// Initial Checkin
//
// 5     1/17/03 8:44p Bmeinke
// Added #include <sys/syspage.h>
//
// 4     1/09/03 7:24p Bmeinke
// Added IsrPRint() protected method
//
// 3     9/06/02 6:23p Bmeinke
// Added Peek() method to read from fifo without removing the data
// Added IsrFifo::SetIsrMaskAttributes()
//
// 2     8/01/02 12:48p Bmeinke
// ~CommFifo is now virtual
// Removed compiler warnings
//
// 1     7/24/02 1:16p Bmeinke
// Was initially declared inside CommCard.h
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _COMM_FIFO_H_INCLUDED
#define _COMM_FIFO_H_INCLUDED

#include <stdint.h>			// uint8_t, etc
#include <sys/neutrino.h>	// intrspin_t
#include "SerialDefs.h"		// SerialProtocol_t

/**
 * FIFO implementation for use with communication data
 *
 * @author Brian Meinke
 * @version 1
 * @since March 21, 2002
 */
class CommFifo
{
public:
	CommFifo();

	CommFifo(uint32_t buffSize);

	virtual ~CommFifo();

	/**
	 * Retrieves the next byte from the buffer
	 *
	 * @param buffByte Location to put the next character from the buffer
	 * @return true if buffByte is valid, false if invalid (no more data)
	 */
	bool GetNextByte( uint8_t &buffByte);

	/**
	 * Retrieves the next byte from the buffer
	 *
	 * @param buff   Buffer to read FIFO contents into
	 * @param size   Maximum number of byte to read
	 * @return Number of bytes read from FIFO, or -1 if an error occurrs
	 */
	int GetBytes( uint8_t *buff, uint32_t size);

	/**
	 * Reads from the FIFO without removing data
	 *
	 * @param buff   Buffer to read FIFO contents into
	 * @param size   Maximum number of byte to read
	 * @return Number of bytes read from FIFO, or -1 if an error occurrs
	 */
	int Peek( uint8_t *buff, uint32_t size);

    /**
	 * Erase count bytes from the front of the FIFO
	 *
	 * @param count  The number of bytes to erase from the FIFO. Pass -1 to erase the entire
	 *               FIFO. Deafult = -1
	 *
	 * @return EOK if bytes were successfully erased
	 */
	int Erase( int count=-1);

    /**
	 * Adds more bytes to the buffer
	 *
	 * @param buff  Buffer of data to be added to the FIFO buffer
	 * @param size   Number of bytes to be added from the buff parameter
	 * @return true if data added, false if an error occurred
	 */
	bool AddBytes( const uint8_t *buff, uint32_t size);

	/**
	 * Resets all indices to 0 (effectively clearing the FIFO)
	 */
	void Reset();

	/**
	 * Returns the number of bytes in the buffer
	 *
	 * @return The number of bytes in the buffer
	 */
	uint32_t GetSize();

	/**
	 * Method used to initialize the protocol settings to be applied
	 * after all data is sent from the FIFO
	 *
	 * @param postProtocol
	 *               The protocol settings to be applied after all data is sent from
	 *               the FIFO
	 */
	void SetPostProtocol( const SerialProtocol_t &postProtocol);

	/**
	 * Returns the protocol settings to be applied after all data
	 * from the FIFO is transmitted
	 *
	 * @param postProtocol
	 *                  (output) Protocol settings to be applied after all data
	 *                  in FIFO is transmitted
	 * @param clearFlag Flag indicating whether or not to clear the 'protocol valid'
	 *                  flag before returning (default=true)
	 * @return true if valid port protocol data has been sepcified, false if
	 *         no port protocol data has been specified.
	 */
	bool GetPostProtocol( SerialProtocol_t &postProtocol, bool clearFlag=true);

	/**
	 * Locks the FIFO for exclusive access
	 *
	 * @return EOK if successfule, error code if an error occurrs
	 */
	virtual int Lock();

	/**
	 * Unlocks the FIFO
	 */
	virtual void Unlock();

protected:

	/**
	 * Locks the FIFO for exclusive access
	 *
	 * @return true if the FIFO was successfully locked
	 */
	virtual bool LockFifo();

	/**
	 * Unlocks the FIFO
	 */
	virtual void UnlockFifo();

	/**
	 * ISR safe mthod for printing character strings
	 *
	 * @param str    NULL terminated string to be printed
	 */
	void IsrPrint(char *str);

private:

	/**
	 * Synchronization object to guard against simultaneous buffer access
	 */
	mutable pthread_mutex_t		m_buffLock;

	/**
	 * Size of the FIFO buffer
	 */
	uint32_t				m_size;

	/**
	 * Index of next character to be read out of the buffer
	 */
	volatile uint32_t			m_head;

	/**
	 * Index of next character to be written into the buffer
	 */
	volatile uint32_t			m_tail;

	/**
	 * FIFO buffer
	 */
	volatile uint8_t			*m_buff;

	/**
	 * Protocol setting to apply after sending the data from the FIFO
	 */
	SerialProtocol_t			m_postProtocol;

	/**
	 * Flag indicating whether or not the m_postProtocol protocol
	 * settings should be applied after we send data from the FIFO
	 */
	bool						m_protocolValid;

};

/**
 * ISR safe implementation of the CommFifo class. ISR safety is
 * accomplished by using interrupt spin locks to lock the FIFO
 * buffer. These interrupt spin lockes require that any thread
 * using an IsrFifo object must have I/O privity. This is
 * accomplished by calling ThreadCtl( _NTO_TCTL_IO, 0) anywhere
 * in the thread before using the FIFO. The ThreadCtl() function
 * requires root priveleges to be successful.
 */
class IsrFifo : public CommFifo
{
public:
	IsrFifo();

	~IsrFifo();

	/**
	 * Initializes the irq id and ISR handle of this FIFO object.
	 * These are used to disable the appropriate interrupt while
	 * we have the FIFO locked so that the ISR is not invoked while
	 * we have the FIFO locked.
	 *
	 * @param irq    Interrupt number to be masked (in order to disable the
	 *               appropriate interrupt)
	 * @param irqId  Handle of the ISR being used to service the hardware interrupt
	 */
	void SetIsrMaskattributes( uint32_t irq, int irqId);

protected:
	/**
	 * Locks the FIFO for exclusive access
	 *
	 * @return true if the FIFO was successfully locked
	 */
	virtual bool LockFifo();

	/**
	 * Unlocks the FIFO
	 */
	virtual void UnlockFifo();

private:
	/**
	 * Synchronization object to guard against simultaneous buffer access
	 */
	mutable intrspin_t	m_isrLock;

	/**
	 * Interrupt vector number (to use with InterruptMask)
	 */
	uint32_t			m_irq;

	/**
	 * ISR handle (to use with InterruptMask)
	 */
	int					m_irqId;
};


#endif	// _COMM_FIFO_H_INCLUDED
