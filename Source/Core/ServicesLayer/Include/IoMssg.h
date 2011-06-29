//******************************************************************************
// Description:
//    Common Packet Header definitions
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/IoMssg.h $
// 
// 3     10/30/07 3:42p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     7/20/05 8:47p Cward
// Fixed typo
//
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
//
// 2     6/28/04 8:30a Bmeinke
// Initialize m_speedRef1, m_speedRefIdx, m_selectMode, m_torqueCmd,
// m_maxPosTrq, m_maxNegTrq, m_spare1 and m_spare2 in the
// DriveCmdDataPacket constructor
//
// 1     6/11/04 4:15p Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:11a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 5     1/13/04 4:57p Bmeinke
// Removed all secondary speed reference items from
// Ab700sIOCmd_t/DriveCmdDataPacket
// Expanded the DriveModes enumeration type to include all possible
// commandable motor modes
//
// 4     1/09/04 1:49p Bmeinke
// Restructured almost entire class tree: the I/O messages we use are
// actually CommonPacket formatted packets. The restructuring takes
// advantage of the existing CommonPacket interface.
//
// 3     1/07/04 10:23a Bmeinke
// Added an IsEnabled() method to the IoMssg700s class
//
// 2     1/05/04 10:27p Bmeinke
// Added copy constructors
// Added a mutex to the IoMssg base class for thread safety
// Fixed the Enable()/Disable() methods (setting wron bit) of the
// IoMssg700s class
// Added QuickStop(),ClearQuickStop(), IsFaulted() and GetCommand()
// methods to the IoMssg700s class
// Fixed the speed and torque enum values in the DriveCmdMssg class
// Added an UnclearFault() method to the DriveCmdMssg class
//
// 1     12/23/03 8:15p Bmeinke
// Initial checkin
//
// 1     12/23/03 8:13p Bmeinke
// Initial checkin
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _IOMSSG_H_INCLUDED_
#define _IOMSSG_H_INCLUDED_
//==============================================================================

#include <sys/neutrino.h>
#include <pthread.h>
#include <stdint.h>
#include <list>

#include "ByteArray.h"
#include "MssgBase.h"
#include "CommonPacket.h"

/**
 * Forward declaration of classes
 */
class UdpIoMssg;
class DriveDataPacket;
class Ab700sUdpIoMssg;
class DriveCmdMssg;
class DriveCmdReply;

/*******************************************************************************
 *
 * See volume 2, section 3-5.1 of the Ethernet/IP spec
 * for a description of decoding UDP traffic.
 *
 *******************************************************************************/

#include <_pack1.h>

typedef union
{
    uint32_t    intVal;

    float       floatVal;

    uint16_t    shortVal[ 2];
} DataLinkVal_t;

typedef struct
{
    mutable uint16_t    m_timeStamp;

    mutable uint16_t    m_runIdleIndicator;

    uint16_t            m_pad1;

    uint16_t            m_command;

    uint16_t            m_pad2;

    uint32_t            m_reference;

    DataLinkVal_t       m_speedRef1;

    DataLinkVal_t       m_speedRefIdx;

    DataLinkVal_t       m_selectedMode;

    DataLinkVal_t       m_torqueCmd;

    DataLinkVal_t       m_maxPosTrq;

    DataLinkVal_t       m_maxNegTrq;

    DataLinkVal_t       m_spare1;

    DataLinkVal_t       m_spare2;

} Ab700sIOCmd_t;

typedef struct
{
    mutable uint16_t    m_timeStamp;

    mutable uint16_t    m_runIdleIndicator;

    uint16_t            m_pad1;

    uint16_t            m_status;

    uint16_t            m_pad2;

    uint32_t            m_reference;

    DataLinkVal_t       m_speed;

    DataLinkVal_t       m_torque;

    DataLinkVal_t       m_spare[ 6];

} Ab700sIOReply_t;


#include <_packpop.h>


namespace AB700s
{
    /**
     * Possible for the drive operational mode (Torque Select mode)k
     */
    enum DriveModes
    {
        /**
         * Free wheel
         */
        ZERO_TRQ_SELECT         = 0,

        /**
         * Apply needed torque to maintain commanded speed
         */
        SPEED_MODE_SELECT       = 1,

        /**
         * Apply full torque- not speed limited
         */
        TRQ_MODE_SELECT         = 2,

        /**
         * Apply full torque- speed limited to FwdSpeedLImit/RevSpeedLimit
         * parameters (param #31 and #30)
         */
        MIN_SPD_TRQ_SELECT      = 3,

        MAX_SPD_TRQ_SELECT      = 4,

        SUM_SPD_TRQ_SELECT      = 5,

        /**
         * Apply full torque- speed limited to SpeedRef1
         */
        ABS_MIN_SPD_TRQ_SELECT  = 6
    };
};

/**
 * Base class for Ethernet/IP UDP I/O messages
 *
 * @author Brian Meinke
 * @version 1
 * @since December 20, 2003
 */

class UdpIoMssg : public CommonPacket
{
public:
    UdpIoMssg( uint32_t connectionId, CommonPacketConnectedDataItem* dataItem) : CommonPacket(),
        m_connData( dataItem)
    {
        m_seqItem.SetConnectionId( connectionId);
        m_seqItem.SetSequenceNo( 0);

        CommonPacket::AddItem( &m_seqItem);
        if( dataItem != NULL)
        {
            CommonPacket::AddItem( dataItem);
        }
    }

    /**
     * Class destructor.
     */ 
    virtual ~UdpIoMssg()
    {
        /**
         * We MUST call this here because when the base class
         * destrcutor runs and calls Clear(), this object no
         * longer exists and therefor the base class Clear()
         * code will run and try to free the memory for each
         * of the items in the item list. This is bas for us
         * because our entries in the item list ARE NOT
         * dynamically allocated.
         */
        Clear();
    }

    /**
     * Returns a reference to the CommonPacketSequencedDataItem
     * contained in our Common Packet
     *
     * @return A reference to the CommonPacketSequencedDataItem
     *         contained in our Common Packet
     */
    const CommonPacketSequencedDataItem& GetSeqItem() const
    {
        return( m_seqItem);
    }

    /**
     * Returns a reference to the CommonPacketSequencedDataItem
     * contained in our Common Packet
     *
     * @return A reference to the CommonPacketSequencedDataItem
     *         contained in our Common Packet
     */
    CommonPacketSequencedDataItem& GetSeqItem()
    {
        return( m_seqItem);
    }

    /**
     * Returns a reference to the CommonPacketConnectedDataItem
     * contained in our Common Packet
     *
     * @return A reference to the CommonPacketConnectedDataItem
     *         contained in our Common Packet
     */
    const CommonPacketConnectedDataItem* GetDataItem() const
    {
        return( m_connData);
    }

    /**
     * Returns a reference to the CommonPacketConnectedDataItem
     * contained in our Common Packet
     *
     * @return A reference to the CommonPacketConnectedDataItem
     *         contained in our Common Packet
     */
    CommonPacketConnectedDataItem* GetDataItem()
    {
        return( m_connData);
    }

    void SetSequenceNo( uint32_t seqNo)
    {
        m_seqItem.SetSequenceNo( seqNo);
    }
    uint32_t GetSequenceNo() const
    {
        return( m_seqItem.GetSequenceNo());
    }

    void SetConnectionId( uint32_t connId)
    {
        m_seqItem.SetConnectionId( connId);
    }
    uint32_t GetConnectionId() const
    {
        return( m_seqItem.GetConnectionId());
    }

    /**
     * Creates a new message object from the data contained in the given
     * raw byte array
     *
     * @param rawBuff Byte array continaing the raw byte format of the omessage object
     * @param buffSz  Number of bytes in the raw buffer
     *
     * @return Number of bytes used from the raw buffer to create the object
     */
    virtual int CreateFromBuff( const uint8_t *rawBuff, size_t buffSz)
    {
        uint16_t    itemCount;
        uint16_t    typeId;
        int         buffIdx=0;

        // Extract the item count
        buffIdx += GetRawItem( itemCount, &rawBuff[ buffIdx]);

        // Create 'itemCount' data items
        while( itemCount-- > 0)
        {
            // Read the type of the next data item. We do not want
            //  to increment buffIdx here because it will
            //  be incremented below when we call CreateFromBuff()
            GetRawItem( typeId, &rawBuff[ buffIdx]);
            switch( typeId)
            {
            case 0x00B1:
                buffIdx += m_connData->CreateFromBuff( &rawBuff[ buffIdx], buffSz-buffIdx+1);
                break;
            case 0x8002:
                buffIdx += m_seqItem.CreateFromBuff( &rawBuff[ buffIdx], buffSz-buffIdx+1);
                break;
            default:
                break;
            }
        }

        return( buffIdx);
    }



protected:

    /**
     * Clears the CommonPacketDataItems from our list and
     * frees the memory we allocated for each of them. We
     * override this because our CommonPacketDataItems are
     * not dynamically allocated, so we do not need to free
     * any memory
     */
    virtual void Clear()
    {
        GetItemList().clear();
    }

    CommonPacketSequencedDataItem   m_seqItem;

    CommonPacketConnectedDataItem   *m_connData;

};

/**
 * Common packet encapsulation of the UDP command we send to
 * the Allen Bradely drives
 *
 * @author Brian Meinke
 * @version 1
 * @since January 7, 2004
 */
class DriveCmdDataPacket : public CommonPacketConnectedDataItem, private Ab700sIOCmd_t
{
    friend class Ab700sUdpIoMssg;
    friend class DriveCmdMssg;

public:
    DriveCmdDataPacket() : CommonPacketConnectedDataItem()
    {
        memset( &m_lockAttr, 0, sizeof( m_lockAttr));
        memset( &m_lock, 0, sizeof( m_lock));
        pthread_mutexattr_init( &m_lockAttr);
        pthread_mutexattr_setrecursive( &m_lockAttr, PTHREAD_RECURSIVE_ENABLE);
        pthread_mutexattr_settype( &m_lockAttr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init( &m_lock, &m_lockAttr);

        m_timeStamp = 0;
        m_runIdleIndicator = 1;
        m_pad1 = 0;
        m_command = 0;
        m_pad2 = 0;
        m_reference = 0;
        m_speedRef1.intVal = 0;
        m_speedRefIdx.intVal = 0;
        m_selectedMode.intVal = 0;
        m_torqueCmd.intVal = 0;
        m_maxPosTrq.intVal = 0;
        m_maxNegTrq.intVal = 0;
        m_spare1.intVal = 0;
        m_spare2.intVal = 0;


    }

    virtual ~DriveCmdDataPacket()
    {
        pthread_mutex_destroy( &m_lock);
        pthread_mutexattr_destroy( &m_lockAttr);
    }

    /**
     * Sets the enable bit in the command word to the drive
     */
    void Enable()
    {
        if( Lock() == EOK)
        {
            m_command |= 0x0002;
            Unlock();
        }
    }
    /**
     * Clears the enable bit in the command word to the drive
     */
    void Disable()
    {
        if( Lock() == EOK)
        {
            m_command &= ~(0x0002);
            Unlock();
        }
    }
    /**
     * Checks if the  enable bit in the command word is set
     *
     * @return true if the enable bit is set
     */
    bool IsEnabled() const
    {
        bool status = false;
        if( Lock() == EOK)
        {
            if( m_command & 0x0002) status = true;
            Unlock();
        }
        return( status);
    }
    /**
     * Sets the stop in the command word to the drive
     */
    void QuickStop()
    {
        if( Lock() == EOK)
        {
            m_command |= 0x0010;
            Unlock();
        }
    }
    /**
     * Clears the stop in the command word to the drive
     */
    void ClearQuickStop()
    {
        if( Lock() == EOK)
        {
            m_command &= ~(0x0010);
            Unlock();
        }
    }

    /**
     * Returns the command/status word being sent to the drive
     *
     * @return The command/status word being sent to the drive
     */
    uint16_t GetStatus() const
    {
        return( m_command);
    }

    void ClearFault()
    {
        if( Lock() == EOK)
        {
            m_command |= 0x0008;
            Unlock();
        }
    }
    void UnclearFault()
    {
        if( Lock() == EOK)
        {
            m_command &=  ~(0x0008);
            Unlock();
        }
    }

    /**
     * Checks if the fault bit in the status word is set
     *
     * @return true if the fault bit is set
     */
    bool IsFaulted() const
    {
        bool retVal = true;
        if( Lock() == EOK)
        {
            if( m_command & 0x0080) retVal = true;
            else                        retVal = false;

            Unlock();
        }

        return( retVal);
    }

    void SetReference( uint32_t ref)
    {
        m_reference = ref;
    }
    uint32_t GetReference() const
    {
        return( m_reference);
    }

    void SetRunIndicator( uint16_t runInd)
    {
        m_runIdleIndicator = runInd;
    }
    uint16_t GetRunIndicator() const
    {
        return( m_runIdleIndicator);
    }

    void SetCommand( uint16_t cmd)
    {
        m_command = cmd;
    }
    uint16_t GetCommand() const
    {
        return( m_command);
    }

    void SetSpeedRef1( float speedRef1)
    {
        m_speedRef1.floatVal = speedRef1;
    }
    float GetSpeedRef1() const
    {
        return( m_speedRef1.floatVal);
    }

    void SelectSpeedRef( uint32_t speedRefIdx)
    {
        m_speedRefIdx.intVal = speedRefIdx;
    }
    uint32_t SelectedSpeedRef() const
    {
        return( m_speedRefIdx.intVal);
    }

    void SelectMode( AB700s::DriveModes newMode)
    {
        m_selectedMode.intVal = newMode;
    }
    AB700s::DriveModes SelectedMode() const
    {
        return( (AB700s::DriveModes)m_selectedMode.intVal);
    }

    void SetTorqueCmd( float torque)
    {
        m_torqueCmd.floatVal = torque;
    }
    float GetTorqueCmd() const
    {
        return( m_torqueCmd.floatVal);
    }

    void SetMaxPosTrq( float maxPosTrq)
    {
        m_maxPosTrq.floatVal = maxPosTrq;
    }
    float GetMaxPosTrq() const
    {
        return( m_maxPosTrq.floatVal);
    }

    void SetMaxNegTrq( float maxNegTrq)
    {
        m_maxNegTrq.floatVal = maxNegTrq;
    }
    float GetMaxNegTrq() const
    {
        return( m_maxNegTrq.floatVal);
    }

    /**
     * Copies the entire message into the given raw byte array
     *
     * @param rawBuff Byte array to copy the message into
     *
     * @return Number of bytes copied into the byte array
     */
    virtual int GetRawBytes(ByteArray_t &rawBuff) const
    {
        int bytesUsed = 0;

        if( Lock() == EOK)
        {
            m_timeStamp = (uint16_t)ClockCycles();

            bytesUsed += CommonPacketConnectedDataItem::GetRawBytes( rawBuff);
            bytesUsed += AddRawItem( m_timeStamp, rawBuff);
            bytesUsed += AddRawItem( m_runIdleIndicator, rawBuff);
            bytesUsed += AddRawItem( m_pad1, rawBuff);
            bytesUsed += AddRawItem( m_command, rawBuff);
            bytesUsed += AddRawItem( m_pad2, rawBuff);
            bytesUsed += AddRawItem( m_reference, rawBuff);
            // We need to short swap the 32-bit ints to/from the drive
            bytesUsed += AddRawItem( m_speedRef1.shortVal[ 0], rawBuff);
            bytesUsed += AddRawItem( m_speedRef1.shortVal[ 1], rawBuff);
            bytesUsed += AddRawItem( m_speedRefIdx.shortVal[ 0], rawBuff);
            bytesUsed += AddRawItem( m_speedRefIdx.shortVal[ 1], rawBuff);
            bytesUsed += AddRawItem( m_selectedMode.shortVal[ 0], rawBuff);
            bytesUsed += AddRawItem( m_selectedMode.shortVal[ 1], rawBuff);
            bytesUsed += AddRawItem( m_torqueCmd.shortVal[ 0], rawBuff);
            bytesUsed += AddRawItem( m_torqueCmd.shortVal[ 1], rawBuff);
            bytesUsed += AddRawItem( m_maxPosTrq.shortVal[ 0], rawBuff);
            bytesUsed += AddRawItem( m_maxPosTrq.shortVal[ 1], rawBuff);
            bytesUsed += AddRawItem( m_maxNegTrq.shortVal[ 0], rawBuff);
            bytesUsed += AddRawItem( m_maxNegTrq.shortVal[ 1], rawBuff);
            bytesUsed += AddRawItem( m_spare2.shortVal[ 0], rawBuff);
            bytesUsed += AddRawItem( m_spare2.shortVal[ 1], rawBuff);
            bytesUsed += AddRawItem( m_spare1.shortVal[ 0], rawBuff);
            bytesUsed += AddRawItem( m_spare1.shortVal[ 1], rawBuff);

            Unlock();
        }

        return( bytesUsed);
    }

    /**
     * Creates a new message object from the data contained in the given
     * raw byte array
     *
     * @param rawBuff Byte array continaing the raw byte format of the omessage object
     * @param buffSz  Number of bytes in the raw buffer
     *
     * @return Number of bytes used from the raw buffer to create the object
     */
    virtual int CreateFromBuff( const uint8_t *rawBuff, size_t buffSz)
    {
        int buffIdx = 0;

        if( Lock() == EOK)
        {
            buffIdx += CommonPacketConnectedDataItem::CreateFromBuff( rawBuff, buffSz);

            buffIdx += GetRawItem( m_timeStamp, &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_runIdleIndicator, &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_pad1, &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_command, &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_pad2, &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_reference, &rawBuff[ buffIdx]);

            // We need to short swap the 32-bit ints to/from the drive
            buffIdx += GetRawItem( m_speedRef1.shortVal[ 0], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_speedRef1.shortVal[ 1], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_speedRefIdx.shortVal[ 0], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_speedRefIdx.shortVal[ 1], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_selectedMode.shortVal[ 0], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_selectedMode.shortVal[ 1], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_torqueCmd.shortVal[ 0], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_torqueCmd.shortVal[ 1], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_maxPosTrq.shortVal[ 0], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_maxPosTrq.shortVal[ 1], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_maxNegTrq.shortVal[ 0], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_maxNegTrq.shortVal[ 1], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_spare2.shortVal[ 0], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_spare2.shortVal[ 1], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_spare1.shortVal[ 0], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_spare1.shortVal[ 1], &rawBuff[ buffIdx]);

            Unlock();
        }

        return( buffIdx);
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        if( Lock() == EOK)
        {
            CommonPacketConnectedDataItem::Print();
            printf( "         Run/Idle: $%4X\n", m_runIdleIndicator);
            printf( "          Command: $%04X\n", m_command);
            printf( "        Reference: $%08X\n", m_reference);
            printf( "Speed Reference 1: %+7.3f\n", m_speedRef1.floatVal);
            printf( "  Speed Ref Index: %d\n", m_speedRefIdx.intVal);
            printf( "    Selected Mode: %s\n", m_selectedMode.intVal == AB700s::SPEED_MODE_SELECT ? "Speed" : "Torque");
            printf( "   Torque Command: %+7.3f\n", m_torqueCmd.floatVal);
            printf( "      Max +Torque: %7.3f %%\n", m_maxPosTrq.floatVal*100);
            printf( "      Max -Torque: %7.3f %%\n", m_maxNegTrq.floatVal*100);

            Unlock();
        }
    }

    /**
     * Lock this message object for exclusive access
     *
     * @return EOK if successfully locked
     */
    inline int Lock() const
    {
        return( pthread_mutex_lock( &m_lock));
    }

    /**
     * Lock this message object for exclusive access
     *
     * @return EOK if successfully locked
     */
    inline int Lock()
    {
        return( pthread_mutex_lock( &m_lock));
    }

    /**
     * Unlock this message object after exclusive access
     *
     * @return EOK if successfully unlocked
     */
    inline int Unlock() const
    {
        return( pthread_mutex_unlock( &m_lock));
    }

    /**
     * Unlock this message object after exclusive access
     *
     * @return EOK if successfully unlocked
     */
    inline int Unlock()
    {
        return( pthread_mutex_unlock( &m_lock));
    }

protected:
    /**
     * Method used to get the length of data contained in the CommonPacketDataItem
     *
     * @return The length of data contained in the CommonPacketDataItem
     */
    virtual uint16_t CalcDataLength() const
    {
        uint16_t    len = 0;

#if 0
        len += sizeof( m_timeStamp);
        len += sizeof( m_runIdleIndicator);
        len += sizeof( m_pad1);
        len += sizeof( m_command);
        len += sizeof( m_pad2);
        len += sizeof( m_reference);
        len += 8*sizeof(m_dataLink[0].intVal);
#else
        len = sizeof( Ab700sIOCmd_t);
#endif
        return( len);
    }

    pthread_mutexattr_t             m_lockAttr;

    mutable pthread_mutex_t         m_lock;
};

/**
 * Common packet encapsulation of the UDP command we receive from
 * the Allen Bradely drives
 *
 * @author Brian Meinke
 * @version 1
 * @since January 7, 2004
 */
class DriveReplyDataPacket : public CommonPacketConnectedDataItem, private Ab700sIOReply_t
{
    friend class Ab700sUdpIoMssg;
    friend class DriveCmdReply;

public:
    DriveReplyDataPacket() : CommonPacketConnectedDataItem(), Ab700sIOReply_t()
    {
        memset( &m_lockAttr, 0, sizeof( m_lockAttr));
        memset( &m_lock, 0, sizeof( m_lock));
        pthread_mutexattr_init( &m_lockAttr);
        pthread_mutexattr_setrecursive( &m_lockAttr, PTHREAD_RECURSIVE_ENABLE);
        pthread_mutexattr_settype( &m_lockAttr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init( &m_lock, &m_lockAttr);

        m_runIdleIndicator = 1;
        m_pad1 = 0;
        m_status = 0;
        m_pad2 = 0;
        m_reference = 0;
        m_speed.floatVal = 0;
        m_torque.floatVal = 0;
        for( int ii=0; ii<6; ii++)
        {
            m_spare[ ii].intVal = 0;
        }
    }

    virtual ~DriveReplyDataPacket()
    {
        pthread_mutex_destroy( &m_lock);
        pthread_mutexattr_destroy( &m_lockAttr);
    }

    /**
     * Returns the current speed as reported from the drive
     *
     * @return The current speed as reported from the drive
     */
    float GetSpeed() const
    {
        return( m_speed.floatVal);
    }

    /**
     * Returns the current torque as reported from the drive
     *
     * @return The current torque as reported from the drive
     */
    float GetTorque() const
    {
        return( m_torque.floatVal);
    }

    /**
     * Checks if the  enable bit in the command word is set
     *
     * @return true if the enable bit is set
     */
    bool IsEnabled() const
    {
        bool status = false;
        if( Lock() == EOK)
        {
            if( m_status & 0x0002)  status = true;
            Unlock();
        }
        return( status);
    }

    /**
     * Returns the command/status word being sent to the drive
     *
     * @return The command/status word being sent to the drive
     */
    uint16_t GetStatus() const
    {
        return( m_status);
    }

    /**
     * Checks if the fault bit in the status word is set
     *
     * @return true if the fault bit is set
     */
    bool IsFaulted() const
    {
        bool retVal = true;
        if( Lock() == EOK)
        {
            if( m_status & 0x0080)  retVal = true;
            else                        retVal = false;

            Unlock();
        }

        return( retVal);
    }

    uint32_t GetReference() const
    {
        return( m_reference);
    }

    /**
     * Returns the
     */
    uint16_t GetRunIndicator() const
    {
        return( m_runIdleIndicator);
    }

    /**
     * Copies the entire message into the given raw byte array
     *
     * @param rawBuff Byte array to copy the message into
     *
     * @return Number of bytes copied into the byte array
     */
    virtual int GetRawBytes(ByteArray_t &rawBuff) const
    {
        int bytesUsed = 0;

        if( Lock() == EOK)
        {
            m_timeStamp = (uint16_t)ClockCycles();

            bytesUsed += CommonPacketConnectedDataItem::GetRawBytes( rawBuff);
            bytesUsed += AddRawItem( m_timeStamp, rawBuff);
            bytesUsed += AddRawItem( m_runIdleIndicator, rawBuff);
            bytesUsed += AddRawItem( m_pad1, rawBuff);
            bytesUsed += AddRawItem( m_status, rawBuff);
            bytesUsed += AddRawItem( m_pad2, rawBuff);
            bytesUsed += AddRawItem( m_reference, rawBuff);
            // We need to short swap the 32-bit ints to/from the drive
            bytesUsed += AddRawItem( m_speed.shortVal[0], rawBuff);
            bytesUsed += AddRawItem( m_speed.shortVal[1], rawBuff);
            bytesUsed += AddRawItem( m_torque.shortVal[ 0], rawBuff);
            bytesUsed += AddRawItem( m_torque.shortVal[ 1], rawBuff);
            for( int ii=0; ii<6; ii++)
            {
                bytesUsed += AddRawItem( m_spare[ ii].shortVal[ 0], rawBuff);
                bytesUsed += AddRawItem( m_spare[ ii].shortVal[ 1], rawBuff);
            }

            Unlock();
        }

        return( bytesUsed);
    }

    /**
     * Creates a new message object from the data contained in the given
     * raw byte array
     *
     * @param rawBuff Byte array continaing the raw byte format of the omessage object
     * @param buffSz  Number of bytes in the raw buffer
     *
     * @return Number of bytes used from the raw buffer to create the object
     */
    virtual int CreateFromBuff( const uint8_t *rawBuff, size_t buffSz)
    {
        int buffIdx = 0;

        if( Lock() == EOK)
        {
            buffIdx += CommonPacketConnectedDataItem::CreateFromBuff( rawBuff, buffSz);

            buffIdx += GetRawItem( m_timeStamp, &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_runIdleIndicator, &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_pad1, &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_status, &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_pad2, &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_reference, &rawBuff[ buffIdx]);
            // We need to short swap the 32-bit ints to/from the drive
            buffIdx += GetRawItem( m_speed.shortVal[ 0], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_speed.shortVal[ 1], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_torque.shortVal[ 0], &rawBuff[ buffIdx]);
            buffIdx += GetRawItem( m_torque.shortVal[ 1], &rawBuff[ buffIdx]);
            for( int ii=0; ii<6; ii++)
            {
                buffIdx += GetRawItem( m_spare[ ii].shortVal[ 0], &rawBuff[ buffIdx]);
                buffIdx += GetRawItem( m_spare[ ii].shortVal[ 1], &rawBuff[ buffIdx]);
            }

            Unlock();
        }

        return( buffIdx);
    }

    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        if( Lock() == EOK)
        {
            CommonPacketConnectedDataItem::Print();
            printf( "      Run/Idle: $%4X\n", m_runIdleIndicator);
            printf( "Command/Status: $%04X\n", m_status);
            printf( "     Reference: $%08X\n", m_reference);
            printf( "         Speed: %+7.3f\n", m_speed.floatVal);
            printf( "        Torque: %+7.3f\n", m_torque.floatVal);

            Unlock();
        }
    }

    /**
     * Lock this message object for exclusive access
     *
     * @return EOK if successfully locked
     */
    inline int Lock() const
    {
        return( pthread_mutex_lock( &m_lock));
    }

    /**
     * Lock this message object for exclusive access
     *
     * @return EOK if successfully locked
     */
    inline int Lock()
    {
        return( pthread_mutex_lock( &m_lock));
    }

    /**
     * Unlock this message object after exclusive access
     *
     * @return EOK if successfully unlocked
     */
    inline int Unlock() const
    {
        return( pthread_mutex_unlock( &m_lock));
    }

    /**
     * Unlock this message object after exclusive access
     *
     * @return EOK if successfully unlocked
     */
    inline int Unlock()
    {
        return( pthread_mutex_unlock( &m_lock));
    }

protected:
    /**
     * Method used to get the length of data contained in the CommonPacketDataItem
     *
     * @return The length of data contained in the CommonPacketDataItem
     */
    virtual uint16_t CalcDataLength() const
    {
        uint16_t    len = 0;

        len += sizeof( m_timeStamp);
        len += sizeof( m_runIdleIndicator);
        len += sizeof( m_pad1);
        len += sizeof( m_status);
        len += sizeof( m_pad2);
        len += sizeof( m_reference);
        len += sizeof( m_speed.intVal);
        len += sizeof( m_torque.intVal);
        return( len);
    }

    pthread_mutexattr_t             m_lockAttr;

    mutable pthread_mutex_t         m_lock;
};

/**
 * Message wrapper object for communication with an Allen Bradley
 * 700s Drive
 *
 * @author Brian Meinke
 * @version 1
 * @since December 17, 2003
 */
class AB700sUdpIoMssg : public UdpIoMssg
{
public:
    AB700sUdpIoMssg( uint32_t connectionId, DriveCmdDataPacket &driveCmd) : UdpIoMssg(connectionId, &driveCmd)
    {
    }

    virtual ~AB700sUdpIoMssg()
    {
    }

    void SetSequenceNo( uint32_t seqNo)
    {
        m_seqItem.SetSequenceNo( seqNo);
    }
    uint32_t GetSequenceNo() const
    {
        return( m_seqItem.GetSequenceNo());
    }
};

/**
 * Message wrapper object for communication with an Allen Bradley
 * 700s Drive
 *
 * @author Brian Meinke
 * @version 1
 * @since December 17, 2003
 */
class AB700sUdpIoReply : public UdpIoMssg
{
public:
    AB700sUdpIoReply( DriveReplyDataPacket &driveStatus) : UdpIoMssg(0, &driveStatus)
    {
    }

    virtual ~AB700sUdpIoReply()
    {
    }
};

/**
 * Encapsulation of a PowerFlex 700s I/O broadcast message
 *
 * @author Brian Meinke
 * @version 1
 * @since December 22, 2003
 */
class DriveCmdMssg : public UdpIoMssg
{
public:
    DriveCmdMssg( uint32_t connectionId=0x00) : UdpIoMssg( connectionId, &m_driveCmd)
    {
        m_driveCmd.SetCommand( 0);
        SetSpeedRef1( 0.0);
        SelectSpeedRef( 1);
        SelectMode( AB700s::SPEED_MODE_SELECT);
        SetTrqCommand( 0.0);
        SetMaxPosTrq( 1.0);
        SetMaxNegTrq( -1.0);
    }

    ~DriveCmdMssg()
    {
    }

    void SetSpeedRef1( float speedRef)
    {
        m_driveCmd.SetSpeedRef1( speedRef);
    }
    float GetSpeedRef1() const
    {
        return( m_driveCmd.GetSpeedRef1());
    }

    void SelectSpeedRef( uint32_t speedRefIdx)
    {
        m_driveCmd.SelectSpeedRef( speedRefIdx);
    }
    uint32_t SelectedSpeedRef() const
    {
        return( m_driveCmd.SelectedSpeedRef());
    }

    void SelectMode( uint32_t driveMode)
    {
        m_driveCmd.SelectMode( (AB700s::DriveModes)driveMode);
    }
    uint32_t SelectedMode() const
    {
        return( (uint32_t)m_driveCmd.SelectedMode());
    }

    void SetTrqCommand( float trqCmd)
    {
        m_driveCmd.SetTorqueCmd( trqCmd);
    }
    float GetTrqCmd() const
    {
        return( m_driveCmd.GetTorqueCmd());
    }

    void SetMaxPosTrq( float trqLim)
    {
        m_driveCmd.SetMaxPosTrq( trqLim);
    }
    float GetMaxPosTrq() const
    {
        return( m_driveCmd.GetMaxPosTrq());
    }

    void SetMaxNegTrq( float trqLim)
    {
        m_driveCmd.SetMaxNegTrq( trqLim);
    }
    float GetMaxNegTrq() const
    {
        return( m_driveCmd.GetMaxNegTrq());
    }

    void ClearFault()
    {
        if( m_driveCmd.Lock() == EOK)
        {
            uint16_t cmd = m_driveCmd.GetCommand();
            m_driveCmd.SetCommand( cmd | 0x0008);
            m_driveCmd.Unlock();
        }
    }
    void UnclearFault()
    {
        if( m_driveCmd.Lock() == EOK)
        {
            uint16_t cmd = m_driveCmd.GetCommand();
            m_driveCmd.SetCommand( cmd & ~(0x0008));
            m_driveCmd.Unlock();
        }
    }
    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        if( m_driveCmd.Lock() == EOK)
        {
            m_seqItem.Print();
            m_driveCmd.Print();

            m_driveCmd.Unlock();
        }
    }

private:
    DriveCmdDataPacket  m_driveCmd;

};

class DriveCmdReply : public UdpIoMssg
{
public:
    DriveCmdReply( uint32_t connectionId=0x00) : UdpIoMssg( connectionId, &m_driveStatus)
    {
    }

    virtual ~DriveCmdReply()
    {
    }

    bool IsFaulted() const
    {
        bool retVal = false;
        if( m_driveStatus.GetStatus() & 0x0080) retVal = true;
        return( retVal);
    }
    /**
     * Print out the contents of the message object
     */
    virtual void Print() const
    {
        if( m_driveStatus.Lock() == EOK)
        {
            m_seqItem.Print();
            m_driveStatus.Print();

            m_driveStatus.Unlock();
        }
    }

private:
    DriveReplyDataPacket    m_driveStatus;

};



#endif // _IOMSSG_H_INCLUDED_
/*

Byte                   Definition
=======================================================================
0x2A    02 00          item count

0x2C    02 80          sequenced address item (0x8002)
0x2E    08 00          length 8 32 bit values
0x30    11 00 a5 c6    connection ID (from connect)
0x34    98 14 00 00    sequence number inc's by one

0x37    b1 00          connected data item
0x3A    2e 00          length of data (# of bytes)
0x3C    98 14          ControlNet sequence number

                         To Drive                  From Drive
                       ------------------------------------------
0x3E    00 00            Run/idle indicator        Pad word
0x40    00 00            Pad word                  Pad word
0x42    8c 60            Logic command             Logic status

                         // 01 00 = enabled
                         // 02 00 = running
                         // 04 00 = command direction
                         // 08 00 = actual direction
                         // 10 00 = accelerating
                         // 20 00 = decelerating
                         // 40 00 = jogging
                         // 80 00 = faulted
                         // 00 01 = alarm
                         // 00 02 = flash mode
                         // 00 04 = ready to run
                         // 00 08 = at limit
                         // 00 10 = tack loss sw
                         // 00 20 = at zero speed
                         // 00 40 = at set speed
                         // 00 80 = reserved

0x44    00 00            Pad word                  Pad word
0x46    00 00            Reference (LSW)           Feedback (LSW)
0x48    00 00            Reference (MSW)           Feedback (MSW)
0x4A                     Datalink A1 in (LSW)      Datalink A1 out (LSW)
0x4C                     Datalink A1 in (MSW)      Datalink A1 out (MSW)
0x4E                     Datalink A2 in (LSW)      Datalink A2 out (LSW)
0x50                     Datalink A2 in (MSW)      Datalink A2 out (MSW)
0x52                     Datalink B1 in (LSW)      Datalink B1 out (LSW)
0x54                     Datalink B1 in (MSW)      Datalink B1 out (MSW)
0x56                     Datalink B2 in (LSW)      Datalink B2 out (LSW)
0x58                     Datalink B2 in (MSW)      Datalink B2 out (MSW)
0x5A                     Datalink C1 in (LSW)      Datalink C1 out (LSW)
0x5C                     Datalink C1 in (MSW)      Datalink C1 out (MSW)
0x5E                     Datalink C2 in (LSW)      Datalink C2 out (LSW)
0x60                     Datalink C2 in (MSW)      Datalink C2 out (MSW)
0x62                     Datalink D1 in (LSW)      Datalink D1 out (LSW)
0x64                     Datalink D1 in (MSW)      Datalink D1 out (MSW)
0x66                     Datalink D2 in (LSW)      Datalink D2 out (LSW)
0x68                     Datalink D2 in (MSW)      Datalink D2 out (MSW)

*/
