//******************************************************************************
// Description:
//  Wrapper object for the POSIX mutex and semaphore synchronization objects
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
//===========================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/BepSync.h $
// 
// 6     10/30/07 3:32p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 5     2/15/06 7:40p Cward
// Corrected destructors for BepMutexes and BepCondVars.
//
// 4     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 1     12/21/03 6:26p Cward
//
// 3     10/24/03 11:14a Khykin
// Updated with mutex changes.
//
// 2     10/10/03 9:20a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 2     8/06/03 3:17p Khykin
// Updated with latest changes from Flatrock.
//
// 2     6/20/03 8:48a Khykin
// Latest changes.
//
// 2     5/29/03 4:38p Bmeinke
// Added #include <stdio.h> because perror() calls were added
//
// 3     5/27/03 7:35a Khykin
// Added error checking when initializing.
//
// 2     4/22/03 3:12p Khykin
// Updating for MotorController changes.
//
// 15    3/18/03 1:25a Tbochene
// Added SetValue() to BepCondVar class
//
// 14    12/16/02 10:38a Bmeinke
// Added #include <errno.h>
//
// 13    12/03/02 8:12p Bmeinke
// Added a Wait() method to the BepSync interface to allow a lock attempt
// to timeout
//
// 12    11/19/02 3:42p Bmeinke
// Fixed bug when setting mutex attributes to resursive
//
// 11    8/26/02 7:39p Bmeinke
// Changed BepMutex to be RECURSIVE
//
// 10    8/26/02 10:06a Bmeinke
// When initializing BepMutex, give mutex the RECURSIVE attribute
//
// 9     6/05/02 11:35p Bmeinke
// Added protected ivars 'sync_t m_syncObject' and 'sync_attr_t
// m_syncAttributes' to BepSync to be used by all derived classes
// Added new template parameter (bool shared=false) to BepCondVar to allow
// placement in shared memory and inter-process sharing
// Added CondVal_t typedef to BepCondVar (typedef T CondVar_t)
// Changed m_condMutex from pthread_mutex_t to sync_t due to library type
// collisions
//
// 8     4/04/02 3:05a Bmeinke
// Added TryAcauire() method
//
// 7     3/22/02 1:22p Bmeinke
// Corrections for BepCondVar class
//
// 6     2/27/02 1:53p Bmeinke
// Added a BepCondVar class to wrap pthread_cond* methods
//
// 5     2/05/02 7:44p Skypig13
// Updated virtual methods in BepSync to have bodies so it will compile
//
// 4     2/05/02 7:31p Bmeinke
// Added destructors for BepMutex and BepSem
//
// 3     2/05/02 7:17p Skypig13
// Added #include <semaphore.h>
//
// 1     1/15/02 10:53p Brian.meinke
// Wrapper classes for POSIX semaphore and mutex synchronization objects
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _BEP_SYNC_H_INCLUDED
#define _BEP_SYNC_H_INCLUDED
//==============================================================================

#include <semaphore.h>
#include <stdint.h>         // uint8_t, etc
#include <stdio.h>          // perror
#include <pthread.h>        // Base synchronization types
#include <errno.h>          // ETIMEDOUT
#include <string.h>         // memset()
#include <sys/neutrino.h>   // ClockGettime()

/**
 * Base class for synchronization data types
 *
 * @author Brian Meinke
 * @version 1
 * @since Jan 15, 2002
 */
class BepSync
{
public:
    /**
     * Creates a new synchronization object
     */
    BepSync() : m_shouldDestroy( true)
    {
        memset( &m_syncObject, 0, sizeof( m_syncObject));
        memset( &m_syncAttributes, 0, sizeof( m_syncAttributes));
    }

    /**
     * Creates a temporary copy of the given synchronization object. Most
     * useful inside methods to ensure the synchronization object is released
     * before leaving
     *
     * @param persistSync
     *               Synchronization object to be copied
     */
    BepSync( const BepSync &persistSync) : m_shouldDestroy( false)
    {
    }

    /**
     * Class destructor
     */
    virtual ~BepSync()
    {
#if 0
        Release();

        // If OK to destroy the underlying sync. object
        if( ShouldDestroy())
        {
            Destroy();
        }
#endif
    }

    /**
     * Acquire a lock on the synchronization object
     *
     * @return EOK if successful, other values indicate error
     */
    virtual int Acquire()
    {
        return(-1);
    }

    /**
     * Release a lock on a synchronization object
     *
     * @return EOK if successful, other values indicate error
     */
    virtual int Release()
    {
        return(-1);
    }

    /**
     * Attempt to lock the resource (don't wait if unavailable)
     *
     * @return EOK if successful, other values indicate error
     */
    virtual int TryAcquire()
    {
        return( -1);
    }

    /**
     * Attempt to lock the resource, waiting up to nsTimeout nano-seconds.
     *
     * @param nsTimeout Number of nano-seconds to wait while trying to lock the resource. If
     *                  the timeout period expires, an error condition (ETIMEDOUT) is returned
     * @return EOK if successful, other values indicate error
     */
    virtual int Wait( const uint64_t &nsTimeout)
    {
        return( ETIMEDOUT);
    }

protected:

    /**
     * Destroys the base OS synchronization data
     */
    virtual void Destroy()
    {
    }

    /**
     * Returns the value of the m_souldDestroy flag
     *
     * @return The value of the m_souldDestroy flag
     */
    bool ShouldDestroy() const
    {
        return( m_shouldDestroy);
    }

    /**
     * Synchronization object
     */
    sync_t      m_syncObject;

    /**
     * Attributes of the synchronization object
     */
    sync_attr_t m_syncAttributes;

private:
    /**
     * Flag indicating if we should destroy the synchronization object
     * from the destructor
     */
    bool        m_shouldDestroy;
};

/**
 * Wrapper class for POSIX mutex object
 *
 * @author Brian Meinke
 * @version 1
 * @see BepSync
 * @see BepSem
 * @since Jan 15, 2002
 */
class BepMutex : public BepSync
{
public:
    /**
     * Creates a new mutex synchronization object
     */
    BepMutex() : BepSync()
    {
        if( (errno=pthread_mutexattr_init( &m_syncAttributes)) != EOK)
        {
            perror( "Error initializing attributes in BepMutex::BepMutex");
        }
        else if( (errno=pthread_mutexattr_settype( &m_syncAttributes, PTHREAD_MUTEX_RECURSIVE)) != EOK)
        {
            perror( "Error setting recursive type in BepMutex::BepMutex");
        }
        else if( (errno=pthread_mutexattr_setrecursive( &m_syncAttributes, PTHREAD_RECURSIVE_ENABLE)) != EOK)
        {
            perror( "Error setting recursive attributes in BepMutex::BepMutex");
        }
        else if( (errno=pthread_mutex_init( &m_syncObject, &m_syncAttributes)) != EOK)
        {
            perror( "Error initializing mutex in BepMutex::BepMutex");
        }
    }

    /**
     * Creates a temporary copy of the given synchronization object. Most
     * useful inside methods to ensure the synchronization object is released
     * before leaving
     *
     * @param persistMutex
     *               Synchronization object to be copied
     */
    BepMutex( const BepMutex &persistMutex) : BepSync( persistMutex)
    {
        m_syncObject = persistMutex.m_syncObject;
    }

    virtual ~BepMutex()
    {
        Release();

        // If OK to destroy the underlying sync. object
        if( ShouldDestroy())
        {
            Destroy();
        }
    }

    /**
     * Acquire a lock on the synchronization object
     *
     * @return EOK if successful, other values indicate error
     */
    virtual int Acquire()
    {
        return( pthread_mutex_lock( &m_syncObject));
    }

    /**
     * Release a lock on a synchronization object
     *
     * @return EOK if successful, other values indicate error
     */
    virtual int Release()
    {
        return( pthread_mutex_unlock( &m_syncObject));
    }

    /**
     * Attempt to lock the resource (don't wait if unavailable)
     *
     * @return EOK if successful, other values indicate error
     */
    virtual int TryAcquire()
    {
        return( pthread_mutex_trylock( &m_syncObject));
    }

    /**
     * Attempt to lock the resource, waiting up to nsTimeout nano-seconds.
     *
     * @param nsTimeout Number of nano-seconds to wait while trying to lock the resource. If
     *                  the timeout period expires, an error condition (ETIMEDOUT) is returned
     * @return EOK if successful, other values indicate error
     */
    virtual int Wait( const uint64_t &nsTimeout)
    {
        struct timespec timeOut;
        uint64_t    absTimeout;

        // Convert releative timeout into absolute timeout
        //////////////////////////////////////////////////

        // Get current clock time
        ClockTime( CLOCK_REALTIME, NULL, &absTimeout);
        // Add relative timeout to current time
        absTimeout += nsTimeout;
        // Convert to timespec struct
        nsec2timespec( &timeOut, absTimeout);
        // Wait for the resource to be available
        return( pthread_mutex_timedlock( &m_syncObject, &timeOut));
    }

protected:
    /**
     * Destroys the base OS synchronization data
     */
    virtual void Destroy()
    {
        pthread_mutex_destroy( &m_syncObject);
        pthread_mutexattr_destroy( &m_syncAttributes);
    }
};


/**
 * Wrapper class for POSIX semaphore object
 *
 * @author Brian Meinke
 * @version 1
 * @see BepSync
 * @see BepMutex
 * @since Jan 15, 2002
 */
template< int count=1>
class BepSem : public BepSync
{
public:
    /**
     * Creates a new mutex synchronization object
     */
    BepSem() : BepSync()
    {
        sem_init( &m_syncObject, 0, count);
    }

    virtual ~BepSem()
    {
        Release();

        // If OK to destroy the underlying sync. object
        if( ShouldDestroy())
        {
            Destroy();
        }
    }

    /**
     * Creates a temporary copy of the given synchronization object. Most
     * useful inside methods to ensure the synchronization object is released
     * before leaving.
     *
     * @param persistSem Synchronization object to be copied.
     */
    BepSem( const BepSem &persistSem) : BepSync( persistSem)
    {
        m_syncObject = persistSem.m_syncObject;
    }

    /**
     * Acquire a lock on the synchronization object
     *
     * @return EOK if successful, other values indicate error
     */
    virtual int Acquire()
    {
        int retVal = EOK;

        // If unable to lock the semaphore
        if( sem_wait( &m_syncObject) == -1)
        {
            // return err code
            retVal = errno;
        }
        return( retVal);
    }

    /**
     * Release a lock on a synchronization object
     *
     * @return EOK if successful, other values indicate error
     */
    virtual int Release()
    {
        int retVal = EOK;

        // If unable to release the semaphore
        if( sem_post( &m_syncObject) == -1)
        {
            // return err code
            retVal = errno;
        }
        return( retVal);
    }


    /**
     * Attempt to lock the resource (don't wait if unavailable)
     *
     * @return EOK if successful, other values indicate error
     */
    virtual int TryAcquire()
    {
        int retVal = EOK;

        if( sem_trywait( &m_syncObject) == -1)
        {
            retVal = errno;
        }

        return( retVal);
    }

    /**
     * Attempt to lock the resource, waiting up to nsTimeout nano-seconds.
     *
     * @param nsTimeout Number of nano-seconds to wait while trying to lock the resource. If
     *                  the timeout period expires, an error condition (ETIMEDOUT) is returned
     * @return EOK if successful, other values indicate error
     */
    virtual int Wait( const uint64_t &nsTimeout)
    {
        struct timespec timeOut;
        uint64_t    absTimeout;

        // Convert releative timeout into absolute timeout
        //////////////////////////////////////////////////

        // Get current clock time
        ClockTime( CLOCK_REALTIME, NULL, &absTimeout);
        // Add relative timeout to current time
        absTimeout += nsTimeout;
        // Convert to timespec struct
        nsec2timespec( &timeOut, absTimeout);
        // Wait for the resource to be available
        return( sem_timedwait( &m_syncObject, &timeOut));
    }

protected:

    /**
     * Destroys the base OS synchronization data
     */
    virtual void Destroy()
    {
        sem_destroy( &m_syncObject);
    }
};

/**
 * Condvar wrapper class
 *
 * @author Brian Meinke
 * @version 1
 * @since Frb 05, 2002
 */
template< class T, bool shared=false>
class BepCondVar : public BepSync
{
public:
    /**
     * Give the controlled data type an accessible type
     */
    typedef T       CondVal_t;

    /**
     * Default constructor
     */
    BepCondVar() : BepSync()
    {
        memset( &m_condMutexAttr, 0, sizeof( m_condMutexAttr));
        memset( &m_condMutex, 0, sizeof( m_condMutex));

        if( (errno=pthread_condattr_init( &m_syncAttributes)) != EOK)
        {
            perror( "Error initializing condvar attributes in BepCondVar::BepCondVar");
        }
        else
        {
            if( shared)     errno = pthread_condattr_setpshared( &m_syncAttributes, PTHREAD_PROCESS_SHARED);
            else            errno = pthread_condattr_setpshared( &m_syncAttributes, PTHREAD_PROCESS_PRIVATE);


            if( errno != EOK)
            {
                perror( "Error setting shared attributes in BepCondVar::BepCondVar");
            }
            else if( (errno=pthread_mutexattr_init( &m_condMutexAttr)) != EOK)
            {
                perror( "Error initializing mutex attributes in BepCondVar::BepCondVar");
            }
            else if( (errno=pthread_mutexattr_settype( &m_condMutexAttr, PTHREAD_MUTEX_ERRORCHECK)) != EOK)
            {
                perror( "Error initializing mutex PTHREAD_MUTEX_ERRORCHECK type in BepCondVar::BepCondVar");
            }
            else if( (errno=pthread_mutex_init( &m_condMutex, &m_condMutexAttr)) != EOK)
            {
                perror( "Error setting mutex in BepCondVar::BepCondVar");
            }
            else if( (errno=pthread_cond_init( &m_syncObject, &m_syncAttributes)) != EOK)
            {
                perror( "Error setting condvar in BepCondVar::BepCondVar");
            }
        }
    }

    /**
     * Destructor
     */
    ~BepCondVar()
    {
        Release();

        // If OK to destroy the underlying sync. object
        if( ShouldDestroy())
        {
            Destroy();
        }
    }

    /**
     * Locks the mutex associated with the cond var
     *
     * @return EOK if successful, error code if an error occurred
     */
    int Acquire()
    {
        return( pthread_mutex_lock( &m_condMutex));
    }

    /**
     * Unlocks the mutex associated with the cond var
     *
     * @return EOK if successful, error code if an error occurred
     */
    int Release()
    {
        return( pthread_mutex_unlock( &m_condMutex));
    }

    /**
     * Attempt to lock the resource (don't wait if unavailable)
     *
     * @return EOK if successful, other values indicate error
     */
    virtual int TryAcquire()
    {
        return( pthread_mutex_trylock( &m_condMutex));
    }

    /**
     * Waits for the condition variable to have the value specified by waitVal.
     * When this value has been detected, the wait function returns WITHOUT
     * UNLOCKING the condvar mutex. You must call Lock() prior to calling
     * this method to ensure proper synchronization
     *
     * @param waitVal Value of the sleepon variable we want to wait for
     * @return EOK if successful, error code if an error occurred
     */
    int Wait( const CondVal_t& waitVal)
    {
        int retVal = EOK;

        // Wait for the correct state for the flag
        while( (m_value != waitVal) && (retVal == EOK) )
        {
            retVal = pthread_cond_wait( &m_syncObject, &m_condMutex);
        }

        return( retVal);
    }

    /**
     * Waits for the condition variable to have the value specified by waitVal.
     * When this value has been detected, the wait function returns WITHOUT
     * UNLOCKING the condvar mutex. You must call Lock() prior to calling
     * this method to ensure proper synchronization
     *
     * @param waitVal   Value of the sleepon variable we want to wait for
     * @param nsTimeout Amount of time in nano-seconds to wait for the condvar
     *                  before timing out
     * @return EOK if successful, ETIMEDOUT if timedout waiting or error code if
     *         an error occurred
     */
    int Wait( const CondVal_t& waitVal, uint64_t nsTimeout)
    {
        int retVal = EOK;
        uint64_t    timeOut = nsTimeout;

        // Wait for the correct state for the flag
        while( (m_value != waitVal) && (retVal == EOK) && (timeOut > 0))
        {
            if( (retVal=TimerTimeout_r( CLOCK_REALTIME, _NTO_TIMEOUT_CONDVAR, NULL, &timeOut, &timeOut)) == EOK)
            {
                retVal = pthread_cond_wait( &m_syncObject, &m_condMutex);
            }
            else if( retVal < 0)
            {
                retVal = -retVal;
            }
        }

        // If we timed out
        if( EOK != retVal)
        {
            // Make sure the cond var was not signalled as we timed out
            if( GetValue() == waitVal)
            {
                retVal = EOK;
            }
        }
        return( retVal);
    }

    /**
     * Signals the highest priority thread waiting on the condvar of a new
     * condvar value
     *
     * @param newValue New value to be signalled
     * @return EOK if successful, error code if an error occurred
     */
    int Signal( const CondVal_t& newValue)
    {
        m_value = newValue;
        return( pthread_cond_signal(&m_syncObject));
    }

    /**
     * Broadcasts a condvar value change to all threads waiting on the condvar
     *
     * @param newValue New value to be broadcast
     * @return EOK if successful, error code if an error occurred
     */
    int Broadcast( const CondVal_t& newValue)
    {
        m_value = newValue;
        return( pthread_cond_broadcast(&m_syncObject));
    }

    /**
     * Returns the value of the condvar value variable
     *
     * @return The value of the condvar value variable
     */
    const CondVal_t& GetValue() const
    {
        return( m_value);
    }

    /**
     * Returns the value of the condvar value variable
     *
     * @param value  Variable to receive the value of the condvar value variable
     */
    void GetValue( CondVal_t& value) const
    {
        value = m_value;
    }

    /**
     * Sets the value of the condvar value variable
     *
     * @param value  Variable to receive the value of the condvar value variable
     */
    void SetValue( const CondVal_t& value)
    {
        m_value = value;
    }

protected:

    /**
     * Destroys the base OS synchronization data
     */
    virtual void Destroy()
    {
        pthread_cond_destroy( &m_syncObject);
        pthread_condattr_destroy( &m_syncAttributes);
        pthread_mutex_destroy( &m_condMutex);
        pthread_mutexattr_destroy( &m_condMutexAttr);
    }

private:
    /**
     * Mutex to guard access to the cond var
     */
    sync_t      m_condMutex;

    /**
     * Attributes of the synchronization object
     */
    sync_attr_t m_condMutexAttr;

    /**
     * Value to associate with this cond var
     */
    CondVal_t   m_value;

};

#endif // _BEP_SYNC_H_INCLUDED

