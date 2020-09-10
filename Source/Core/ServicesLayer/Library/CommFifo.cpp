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
//     $Log: /Core/ServicesLayer/Library/CommFifo.cpp $
// 
// 2     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "CommFifo.h"

#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif

#include <pthread.h>		// pthread_mutex
#include <sys/syspage.h>	// SYSPAGE_ENTRY



CommFifo::CommFifo() : m_size(2048), m_protocolValid(false)
{
    m_head = m_tail = 0;

    m_buff = new uint8_t[ m_size];
    memset( (void*)m_buff, 0, m_size);
    // set up the mutex
    pthread_mutexattr_t attr;

    memset( &attr, 0, sizeof( attr));
    memset( &m_buffLock, 0, sizeof( m_buffLock));

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_ENABLE);
    pthread_mutex_init( &m_buffLock, &attr);
}

CommFifo::~CommFifo()
{
    pthread_mutex_destroy( &m_buffLock);
    delete [] m_buff;
}


bool CommFifo::GetNextByte( uint8_t &buffByte)
{
    bool retVal = false;

    if( LockFifo())
    {
        if( m_head != m_tail)
        {
            buffByte = m_buff[ m_head++];
            m_head %= m_size;
            retVal = true;
        }
        UnlockFifo();
    }

    return( retVal);
}

int CommFifo::GetBytes( uint8_t *buff, uint32_t size)
{
    uint32_t	retVal = 0;
    uint32_t	maxSz;

    if( LockFifo())
    {
        // Calculate number of bytes available in FIFO
        if (m_tail < m_head)
            maxSz = m_size + (m_tail - m_head);
        else
            maxSz = (m_tail - m_head);

        // Don't overflow caller's buffer
        if (size  < maxSz)
            maxSz = size;

        while( (m_head != m_tail) && (retVal < maxSz))
        {
            buff[ retVal++] = m_buff[ m_head++];
            m_head %= m_size;
        }
        UnlockFifo();
    }

    return( retVal);
}

int CommFifo::Peek( uint8_t *buff, uint32_t size)
{
    uint32_t	idx=m_head, retVal = 0;
    uint32_t	maxSz;

    if( LockFifo())
    {
        // Calculate number of bytes available in FIFO
        if (m_tail < m_head)
            maxSz = m_size + (m_tail - m_head);
        else
            maxSz = (m_tail - m_head);

        // Don't overflow caller's buffer
        if (size  < maxSz)
            maxSz = size;

        while( (idx != m_tail) && (retVal < maxSz))
        {
            buff[ retVal++] = m_buff[ idx++];
            idx %= m_size;
        }
        UnlockFifo();
    }

    return( retVal);
}

int CommFifo::Erase( int count /*=-1*/)
{
	uint32_t	retVal;
	uint32_t	idx = 0;

	if( (retVal=Lock()) == EOK)
	{
		if( count == -1)
		{
			Reset();
		}
		else
		{
			while( (m_head != m_tail) && (idx++ < (uint32_t)count))
			{
				m_head = (m_head + 1 ) % m_size;
			}
		}

		Unlock();
	}

	return( retVal);
}

bool CommFifo::AddBytes( const uint8_t *buff, uint32_t size)
{
    bool				retVal = false;
    register uint32_t	ii = 0;

    if( LockFifo())
    {
        for( ii=0; ii<size; ii++)
        {
            m_buff[ m_tail++] = buff[ ii];
            m_tail %= m_size;
        }

        retVal = true;
        UnlockFifo();
    }

    return( retVal);
}

void CommFifo::Reset()
{
    if( LockFifo())
    {
        memset( (void*)m_buff, 0, m_size);
        m_head = m_tail = 0;
        m_protocolValid = false;
        UnlockFifo();
    }
}

uint32_t CommFifo::GetSize()
{
    uint32_t	size = 0;

    if( LockFifo())
    {
        size = m_tail - m_head;
        if (m_tail < m_head)
            size = m_size + size;
        UnlockFifo();
    }

    return( size);
}


void CommFifo::SetPostProtocol( const SerialProtocol_t &postProtocol)
{
    if( LockFifo())
    {
        m_postProtocol = postProtocol;
        m_protocolValid = true;
        UnlockFifo();
    }
}

bool CommFifo::GetPostProtocol( SerialProtocol_t &postProtocol, bool clearFlag /*=true*/)
{
    bool retVal = m_protocolValid;

    if( m_protocolValid)
    {
        postProtocol = m_postProtocol;
        if (clearFlag)
            m_protocolValid = false;
    }

    return( retVal);
}

int CommFifo::Lock()
{
    return( pthread_mutex_lock( &m_buffLock));
}

void CommFifo::Unlock()
{
    pthread_mutex_unlock( &m_buffLock);
}


bool CommFifo::LockFifo()
{
    bool retVal = true;

    if( Lock() != EOK)
    {
        retVal = false;
    }

    return( retVal);
}

void CommFifo::UnlockFifo()
{
    Unlock();
}

void CommFifo::IsrPrint(char *str)
{
    void (*outchar)(struct syspage_entry *, char) = SYSPAGE_ENTRY(callout)->debug[0].display_char;

    while (*str)
        outchar(_syspage_ptr, *str++);
}


////////////////////////////////////////////////////////////////
//  IsrFifo
/////////////////////////////////////////////////////////////////


IsrFifo::IsrFifo() : CommFifo(),
    m_irq( (uint32_t)-1),m_irqId( -1)
{
    memset( &m_isrLock, 0, sizeof( m_isrLock));
}

IsrFifo::~IsrFifo()
{
}

void IsrFifo::SetIsrMaskattributes( uint32_t irq, int irqId)
{
    m_irq = irq;
    m_irqId = irqId;
}

bool IsrFifo::LockFifo()
{
    InterruptMask( m_irq, m_irqId);
    InterruptLock( &m_isrLock);
    return( true);
}

void IsrFifo::UnlockFifo()
{
    InterruptUnlock( &m_isrLock);
    InterruptUnmask( m_irq, m_irqId);
}

