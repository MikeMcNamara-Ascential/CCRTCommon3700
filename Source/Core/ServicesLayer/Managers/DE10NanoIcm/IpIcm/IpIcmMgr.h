//******************************************************************************
// Description:
//    Resource manager for Altera IP Icm card
//
//==============================================================================
//
// Copyright ? 1999 Burke E. Porter Machinery Co.
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interest thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _IP_ICM_MGR_H_
#define _IP_ICM_MGR_H_

class IpIcmMgr;

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

#include "ResManagerXml.h"      // I/O Manager base class

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

/*******************
 include files
 ******************/
#include <stdio.h>          // perror, printf
#include <stdarg.h>         // Variable argument stuff
#include <stdlib.h>         // rand
#include <string.h>         // memmove, strlen, etc
#include <time.h>           // POSIX Timers
#include <signal.h>         // POSIX signals
#include <semaphore.h>      // POSIX semaphores
#include <process.h>        // tfork
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <math.h>           // fabs()
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <vector>
#include <map>
#include <string>
#include <list>


//#include "IpIcm.h"              // Altera IP Icm object
#include "DE10Nano.h"              // SBS PCI40 IP Carrier Object

#include "tchar.h"              // Wide character wrappers
#include "Bepos.h"              // Standard BEP wrappers
#include "XmlParser.h"          // XML Parser definition
#include "XmlDefs.h"            // XML definitions
#include "XmlNode.h"            // XML Node definitions
#include "BepXmlUtils.h"        // XML Utilities
#include "BepException.h"       // Standard BEP exception class
#include "BepSync.h"            // BepMutex
#include "INamedDataBroker.h"   // System data read/write interface
#include "IcmDefs.h"            // My definitions
#include "IcmLogFile.h"         // Array data log file objects
#include "DataTagMap.h"         // Mapping tag name to system data tag
#include "ILogger.h"

/**
 * Calculate time delta between two clock readings (in ns) and correct for roll-over
 *
 * @param start Beginning time sample
 * @param end       Ending time sample
 *
 * @return Time delta (in ns) between start and end
 */
#define nsDelta( start, end)        ( (end >= start) ?  (end-start) : ( ((uint64_t)-1 - start) + end + 1) )

/**
 * Acceleration alarm states
 */
typedef enum
{
    /**
     * Alarm is off
     */
    ACCEL_ALARM_OFF = 0,
    /**
     * Waiting to turn Alarm off
     */
    ACCEL_ALARM_OFF_PENDING,
    /**
     * Waiting to turn Alarm on
     */
    ACCEL_ALARM_ON_PENDING,
    /**
     * Alarm is on
     */
    ACCEL_ALARM_ON
} AccelAlarmState_t;

/**
 * Timer class used for ICM data sampling
 *
 * @author Brian Meinke
 * @version 1
 * @since March 12, 2003
 */
class IcmTimer
{
public:
    /**
     * Constructor
     */
    IcmTimer() : m_timerId( -1)
    {
    }

    /**
     * Destructor
     */
    ~IcmTimer()
    {
    }

    /**
     * Start the timer with the specified delay/repeat to deliver the given event
     *
     * @param initDelay The amount of time for the timer to delay before firing
     * @param repeat    The repeat rate between firing consecutive timer events
     * @param event     Event to deliver when the timer expires
     */
    void Start( const uint64_t &initDelay, const uint64_t &repeat, struct sigevent *event)
    {
        if( m_timerId == -1)    m_timerId = TimerCreate( CLOCK_REALTIME, event);
        m_timerSpec.nsec = initDelay;
        m_timerSpec.interval_nsec = repeat;
        TimerSettime( m_timerId, 0, &m_timerSpec, NULL);
    }

    /**
     * Stops and destroys the timer
     */
    void Stop()
    {
        if( m_timerId != -1)
        {
            memset( &m_timerSpec, 0, sizeof( m_timerSpec));
            TimerSettime( m_timerId, 0, &m_timerSpec, NULL);
            TimerDestroy( m_timerId);
            m_timerId = -1;
        }
    }

protected:
    /**
     * Id of the timer
     */
    timer_t         m_timerId;

    /**
     * Timer control structure
     */
    struct _itimer  m_timerSpec;

    /**
     * Event structure controlling how timer behaves
     */
    struct sigevent m_timerEvent;

};

/**
 * On-shot timer class used to detect "no-interrupt" conditions
 *
 * @author Brian Meinke
 * @version 1
 * @since March 12, 2003
 */
class IsrWatchdogTimer : public IcmTimer
{
public:
    /**
     * Constructor
     */
    IsrWatchdogTimer() : IcmTimer()
    {
    }

    /**
     * Destructor
     */
    ~IsrWatchdogTimer()
    {
    }

    /**
     * Start the timer with the specified delay to deliver the given event
     *
     * @param initDelay The amount of time for the timer to delay before firing
     * @param event     Event to deliver when the timer expires
     */
    void Start( const uint64_t &initDelay, struct sigevent *event)
    {
        if( m_timerId == -1)    m_timerId = TimerCreate( CLOCK_REALTIME, event);
        m_timerSpec.nsec = initDelay;
        TimerSettime( m_timerId, 0, &m_timerSpec, NULL);
    }
};

/**
 * Object used to hold the attributes of an ICM sub channel (i.e.
 * speed, force, etc)
 *
 * @author Brian Meinek
 * @version 1.0
 * @since February 26, 2003
 */
class IcmSubChannel
{
public:
    /**
     * Default constructor
     *
     * @param name   Subchannel name (default = "")
     * @param mode   Subchannel accessibility: O_RDONLY or O_RDWR (default=O_RDONLY)
     */
    IcmSubChannel( char *name="", int mode=O_RDONLY) : m_name( name), m_mode( mode)
    {
    }

    /**
     * Check if this csub-channel is read only
     *
     * @return true if the sub-channel is read only, false if it is writeable
     */
    inline bool IsReadOnly() const
    {
        return( m_mode == O_RDONLY);
    }

    /**
     * Returns the name of the sub-channel object
     *
     * @return The name of the sub-channel object
     */
    const string &GetName() const
    {
        return( m_name);
    }

private:
    /**
     * Name of the sub-channel (i.e. Speed, Force, etc)
     */
    string  m_name;

    /**
     * Subchannel accessibility: O_RDONLY or O_RDWR (default=O_RDONLY)
     */
    int     m_mode;

    bool operator==( const IcmSubChannel &rhs)
    {
        return( (m_mode == rhs.m_mode) && (m_name == rhs.m_name));
    }
};

/**
 * Enumeration of ICM sub-channel indices
 */
typedef enum
{
    SPEED_K_INDEX   = 0,
    ACCEL_K_INDEX   = 1,
    FORCE_K_INDEX   = 2,
    DIST_INDEX      = 3,
    FREQ_INDEX      = 4,
    SPEED_INDEX     = 5,
    ACCEL_INDEX     = 6,
    FORCE_INDEX     = 7,
    LOSS_INDEX      = 8,
    COUNT_INDEX     = 9,
    ICM_SUBCHNL_CNT
} IcmSubChannelIdx_t;

/**
 * Enumeration of ICM block indices
 */
typedef enum
{
    SPEED_BLOCK_INDEX       = 0,
    ACCEL_BLOCK_INDEX       = 1,
    FORCE_BLOCK_INDEX       = 2,
    LOSS_BLOCK_INDEX        = 3,
    COUNT_BLOCK_INDEX       = 4,
    DRAG_BLOCK_INDEX        = 5,
    ICM_BLOCK_CNT
} IcmBlockIdx_t;

/**
 * Data type used to hold data values for each of the channels on an ICM
 */
typedef struct
{
    /**
     * Array of data values for each channel on an ICM
     */
    IcmChannelData_t        data[ MAX_CHANNELS];
} IcmDataSample_t;

/**
 * Data type used to hold the most recent data sample from the ICM
 */
typedef std::vector< IcmDataSample_t>       InstDataArray_t;
typedef InstDataArray_t::iterator           InstDataArrayItr_t;

typedef std::vector< std::string>           StringList_t;
typedef StringList_t::iterator              StringListItr_t;
typedef StringList_t::const_iterator        StringListCItr_t;

typedef std::vector< IcmSubChannel>         IcmSubChannelList_t;
typedef IcmSubChannelList_t::iterator       IcmSubChannelListItr_t;
typedef IcmSubChannelList_t::const_iterator IcmSubChannelListCItr_t;

typedef std::vector<DE10Nano*>                     IpIcmList_t;
typedef IpIcmList_t::iterator               IpIcmListItr_t;
typedef IpIcmList_t::const_iterator         IpIcmListCItr_t;

typedef std::map< string, uint32_t>         ChannelNameMap_t;
typedef ChannelNameMap_t::iterator          ChannelNameMapItr_t;
typedef ChannelNameMap_t::const_iterator    ChannelNameMapCItr_t;
typedef ChannelNameMap_t::value_type        ChannelNameMapVal_t;

typedef struct
{
    float       rollerFactor[ MAX_CHANNELS];
    uint32_t    encoderPulse[ MAX_CHANNELS];
    float       rollDiameter[ MAX_CHANNELS];

} RollAttribs_t;




/**
 * Alter IP module ICM manager
 *
 * @author Brian Meinke
 * @version 1.0
 * @since February 26, 2003
 */
class IpIcmMgr : public ResManagerXml
{
    friend class IpIcmMgrTest;

public:

    /**
     * Default constructor
     */
    IpIcmMgr();

    /**
     * Constructor with command line arguments
     *
     * @param argc   Number of arguments in the argc array
     * @param argv   Array of command line options
     */
    IpIcmMgr( int argc, char *argv[]);

    /**
     * Destructor
     */
    ~IpIcmMgr();

    /**
     * Perform any server initialization
     *
     * @return true if successful
     *         false if error
     */
    virtual bool InitManager();

    /**
     * Performs any necessary manager cleanup (free memory, detach any
     * adopted path names, etc)
     */
    virtual void CleanupManager();

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         Success, Fail
     * @since Version 2.5
     */
    virtual const std::string Register(void);

    /**
     * Request to be inserted into the list to be notified
     * of a change in a data tag
     *
     * @param node     Data and Name for who is requesting the subscription information.
     * @return Status of operation
     *         BEP_SUCCESS_RESPONSE == Subscriber successfully added
     *         BEP_FAILURE_RESPONSE == Error adding subscriber
     * @since Version 2.0
     */
    virtual const std::string Subscribe(const XmlNode *node);

    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 2.1
     */
    virtual const std::string Publish(const XmlNode *node);

    /**
     * Request to Perform an action
     *
     * @param node    Action to be performed
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Command(const XmlNode *node);

    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     * @since Version 2.0
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate=0);

    /**
     * Request to write a data tag
     *
     * @param node  Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Write(const XmlNode *node);

    /**
     * Reloads the configuration file and resets the
     * base components objects.
     * @since Version 1.0
     */
    virtual void ReloadConfiguration(void);
	
	int ClearDistance();
	int StartICM();
    int TerminateICM();
	int StopICM();
	int SetNumChannels( int channels);
	int SetSampleRate( int rate);
	int ReadWheelValues( int channel, float &freq, int &dist, uint32_t &dir, uint32_t &time);
	int ReadChannelValues( int channel, uint32_t &time, uint32_t &count, float &freq);
	unsigned ReadChannelCount(int channel);
	unsigned ReadChannelTime();
	float ReadChannelFreq(int channel);
	unsigned ReadChannelDirection( int channel);
	void InitializeIpIcmData( void);

protected:

    /**
     * Handles I/O mesages
     *
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t IoMssgLoop();

    /**
     * method for component initialization.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Starts the pseudo-ISR sampling thread used to sample data from
     * the ICM IP modules
     */
    void AttachIsr();

    /**
     * Detaches our interupt handler and disables interrupts from the
     * ICM modules
     */
    void DetachIsr();

    /**
     * Fills in an XmlNode with the data value for a data item
     *
     * @param dataItem Properties for the data item to read
     * @param value    String to fill in with the value of the requested data item
     * @return 0 if valueNode is filled in
     *         -1 if unable to handle the request
     */
    virtual int ReadDataItem(const dataMapItem_t *dataItem, string &value);

    /**
     * Reads a data item's value from an XmlNode
     *
     * @param dataItem Properties for the data item to write to
     * @param value    String containing the value of the specified data item
     * @return 0 if new value was written
     *         -1 if unable to handle the request
     */
    virtual int WriteDataItem(const dataMapItem_t *dataItem, const string &value);

    /**
     * Method used to attach pulses to the resource manager's I/O
     * channel
     *
     * @return -1 on error, any other value indicates success
     */
    virtual int AttachIoPulses();

    /**
     * Handles pulses delivered to our resource manager I/O channel
     *
     * @param pulse  Pulse received on the I/O channel
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t HandleResMgrPulse(io_pulse_t &pulse);

    /**
     * Message handler for _IO_READ I/O messages. Called by ResManagerBase::ioRead
     *
     * @param ctp    Dispatch Interface pointer
     * @param msg    Read message data
     * @param ioOcb  Control Block for this client
     * @return 0 if successful
     *         <0 if error occurs
     * @see ResManagerBase::ioRead
     */
    virtual int IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Message handler for _IO_WRITE I/O messages. Called ny ResManagerBase::ioWrite
     *
     * @param ctp    Dispatch Interface pointer
     * @param msg    Write message data
     * @param ioOcb  Control Block for this client
     * @return 0 if successful
     *         <0 if error occurs
     * @see ResManagerBase::ioWrite
     */
    virtual int IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handles the message that requests information about the
     * resource associated with the passed OCB. Called by ioStat
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Stat structure to return to the client
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManagerBase::ioStat
     */
    virtual int IoStat(resmgr_context_t *ctp, io_stat_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Initializes the named data broker interface object
     */
    void InitializeNdb();

    /**
    * Initialize the local named broker with an already created
    * named data broker
    * Implemented for use in testing
    */
    void InitializeNdb(INamedDataBroker *dataBroker);

    /**
     * Checks if the passed OCB refers to a path for an instantaneous data item
     *
     * @param ioOcb  OCB to be checked for being an instantaneous data item
     * @return true if the OCB refers to an instantaneous data item, false otherwise
     */
    bool IsInstantaneousDevice( resMgrIoOcb_t *ioOcb);

    /**
     * Checks if the passed OCB refers to a path for an array data item
     *
     * @param ioOcb  OCB to be checked for being an array data item
     * @return true if the OCB refers to an array data item, false otherwise
     */
    bool IsArrayDevice( resMgrIoOcb_t *ioOcb);

    /**
     * Checks if the passed OCB refers to a path for a block data item
     *
     * @param ioOcb  OCB to be checked for being a block data item
     * @return true if the OCB refers to a block data item, false otherwise
     */
    bool IsBlockDevice( resMgrIoOcb_t *ioOcb);

    /**
     * Opens an ascii data log file
     *
     * @param logStruct AsciiLogFile_t strusture describing the data log file to open
     */
    void OpenDataLog( AsciiLogFile_t &logStruct);

    /**
     * Creates the symbolic link required for the GUI to access a data log
     *
     * @param logStruct AsciiLogFile_t strusture describing the data log file to link
     */
    void UpdateLogLink( AsciiLogFile_t &logStruct);
	void LogChannelData(IcmChannelData_t *channelData);

	IpIcmData_t         m_ipIcmData;

    FILE *outFile;
    void Logo(unsigned int mask, const char *cpFmt, ...); 
    double m_disconnectInterval;
	
	
private:

    /**
     * Set cold default values for data members
     */
    void SetDefaults();

    /**
     * Method used to handle a command line argument. Called from
     * ParseCommandLine() for each comamnd line argument encountered.
     *
     * @param option The command line option (i.e. 'f' if -f specified on command line)
     * @param value  Value associated with the command line option being processed.
     *               May be NULL is no argument is associated with the command line
     *               option
     *
     * @return true if the argument was processed, false if not recognized
     */
    virtual bool HandleCommandLineArg( int option, const char *value);

    /**
     * Attaches the calling process as the IO manager of the given 'rootToAdopt'
     * path. The OS will send messages to our process when IO occurs within the
     * 'rootToAdopt' path. The OS will include a unit number in IO messages
     * pertaining to our path. This unit number is identical to the device number
     * returned by this function. The OS does not include the path in its IO
     * messages, so a process must keep track of device number/path relationships.
     *
     * @param rootToAdopt
     *               Path namespace to take over (i.e. MyResource would be mounted
     *               at /dev/MyResource)
     * @return 0 if successful, -1 if error
     */
    int PathAttach( const char *pathToAdopt);

    /**
     * Attaches path's in the process manager's namespace for instantaneous
     * data values for speed, force, acceleration, etc.
     *
     * @param rootToAdopt
     *               Path namespace to take over (i.e. MyResource would be mounted
     *               at /dev/MyResource)
     * @return 0 if successful, -1 if error
     */
    int AttachInstPaths( const char *rootToAdopt);

    /**
     * Attaches path's in the process manager's namespace for array
     * data values for speed, force, acceleration, etc.
     *
     * @param rootToAdopt
     *               Path namespace to take over (i.e. MyResource would be mounted
     *               at /dev/MyResource)
     * @return 0 if successful, -1 if error
     */
    int AttachArrayPaths( const char *rootToAdopt);

    /**
     * Attaches path's in the process manager's namespace for block
     * data values for speed, force, acceleration, etc.
     *
     * @param rootToAdopt
     *               Path namespace to take over (i.e. MyResource would be mounted
     *               at /dev/MyResource)
     * @return 0 if successful, -1 if error
     */
    int AttachBlockPaths( const char *rootToAdopt);

    /**
     * Copies instantaneous data values to the data arrays
     */
    void UpdateArrays();

    /**
     * Appends the latest data array samples to an ASCII text file
     */
    void LogAsciiData();

    /**
     * Appends the latest data point to a data log file
     *
     * @param dataLog   Data log object to log data to
     * @param dataArray Data array to log data from
     */
    void UpdateDataLog( IcmLogFile &dataLog, const float *dataArray);

    /**
     * Check if all wheels are at zerospeed
     *
     * @param zspd   Zerospeed flag: true if at zerospeed, false otherwise
     */
    void WriteZerospeed( bool zspd);

    /**
     * Publishes status of OverSpeed if it has changed
     *
     * @param overSpeed Overspeed flag: true if any wheel over the OverSpeed limit, false otherwise
     */
    void WriteOverspeed( bool overSpeed);

    /**
     * Writes the current wheel speeds to the PLC
     */
    void WritePlcWheelSpeeds();

    /**
     * Updates a channel loss (machine loss) in the ICM config file and
     * on the board and channel being passed in
     *
     * @param icmBoard   Index of the ICM board to update the machine loss on
     * @param channelIdx Channel to update the machine loss on
     * @param newLoss    New machine loss value
     */
    void WriteChannelLoss( uint32_t icmBoard, uint32_t channelIdx, float newLoss);

    /**
     * Checks to see if any acceleration value exceeds the accel limit. If any of the acceleration
     * values exceeds the accel limit, the PLC horn will be "beeped"
     */
    void CheckAccelLimit();

    /**
     * Determine what value to write to the accel alarm tag
     *
     * @param valToWrite The value to be written to the alarm output ("0" or "1")
     *
     * @return The value to be written to the alarm output ("0" or "1")
     */
    std::string& GetAccelAlarmValue( std::string& valToWrite);

    /**
     * Initialilzes an ICM board using the given board address
     */
    void InitializeBoards();

    /**
     * Caluclates the conversion factors used to convert ICM counts into speed,
     * force and acceleration values
     */
    void CalculateConversionFactors();

    /**
     * Attempts to autodetect ICM IP modules in the system
     *
     * @return Number of ICM IP modules detected
     */
    int DetectBoards();

    /**
     * Read general config data(for all ICM boards we manage)
     *
     * @param childNodes List of configuration nodes
     */
    void ReadGeneralConfig( const XmlNodeMap &childNodes);

    /**
     * Reads the data logging data from the given XML configuration node
     *
     * @param dataLogNode
     *               XML node whose children each contain a data log file name and link name
     */
    void ReadDataLogConfig( const XmlNode *dataLogNode);

    /**
     * Initializes the givern AsciiLogFile_t structure from the given
     * XML configuration node
     *
     * @param logStruct AsciiLogFile_t structure to be initialized
     * @param node      XML configuration node containing the log file name and link
     *                  name as attributes
     */
    void ReadDataLogConfig( AsciiLogFile_t &logStruct, const XmlNode *node);

    /**
     * Read configuration data for a given channel as specified by the
     * chnlTag parameter
     *
     * @param children List of setup data nodes (children of <SETUP>)
     * @param chnlTag  Tag of the channel to read config data for
     */
    void ReadChannelConfig( const XmlNodeMap &children, const char *chnlTag);

    /**
     * Reads the board index and channel index for a given ICM channel from a
     * configuration file XML node. The board and channel indices are stored
     * as attributes of a channel's XML configuration node.
     *
     * @return 0 if successful
     *         -1 if error
     */
    int LoadMachineLosses();

    /**
     * Reads the board index and channel index for a given ICM channel from a
     * configuration file XML node. The board and channel indices are stored
     * as attributes of a channel's XML configuration node.
     *
     * @param chnlNode Node to read data from
     * @param brdIdx   Board Index on which the channel resides
     * @param chnlIdx  Channel's index on the board it resides on.
     */
    void GetChannelIndex( const XmlNode* chnlNode, UINT32 &brdIdx, UINT32 &chnlIdx);

    /**
     * Reads the data tag names and properties
     *
     * @param setupNode Node to use as the root DATA_MAP node
     * @return 0 if successful
     *         -1 if error
     */
    virtual int ReadDataMap( const XmlElement *setupNode);

    /**
     * Reads the list of data tags we will use
     *
     * @param tagListNode
     *               Parent node of the data tag names
     */
    void ReadDataTagList( const XmlNode *tagListNode);

    /**
     * Method used to read instantaneous data when a client performs a
     * read() on any of our instantaneous data devices
     *
     * @param ctp    Dispatch Interface pointer
     * @param msg    Read message data
     * @param ioOcb  Control Block for this client
     * @return Number of bytes sent to the client
     */
    int ReadInstData( resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Method used to read array data when a client performs a
     * read() on any of our array data devices
     *
     * @param ctp    Dispatch Interface pointer
     * @param msg    Read message data
     * @param ioOcb  Control Block for this client
     * @return Number of bytes sent to the client
     */
    int ReadArrayData( resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Method used to read block type data when a client performs a
     * read() on any of our block data devices
     *
     * @param ctp    Dispatch Interface pointer
     * @param msg    Read message data
     * @param ioOcb  Control Block for this client
     * @return Number of bytes sent to the client
     */
    int ReadBlockData( resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Method used to write instantaneous data when a client performs a
     * write() on any of our instantaneous data devices
     *
     * @param ctp    Dispatch Interface pointer
     * @param msg    io_write message data
     * @param ioOcb  Control Block for this client
     * @return Number of bytes sent to the client
     */
    int WriteInstData( resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Method used to write block type data when a client performs a
     * write() on any of our block data devices
     *
     * @param ctp    Dispatch Interface pointer
     * @param msg    io_write message data
     * @param ioOcb  Control Block for this client
     * @return Number of bytes sent to the client
     */
    int WriteBlockData( resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Returns the number of instantaneous devices for which we have adopted paths
     *
     * @return The number of instantaneous devices for which we have adopted paths
     */
    int NumberOfInstantaneousDevices();

    /**
     * Returns the number of array devices for which we have adopted paths
     *
     * @return The number of array devices for which we have adopted paths
     */
    int NumberOfArrayDevices();

    /**
     * Returns the number of block devices for which we have adopted paths
     *
     * @return The number of block devices for which we have adopted paths
     */
    int NumberOfBlockDevices();

    /**
     * Calculates a unique base channel inode value based on a channel tag.
     * The base channel inode is common for all subchannels of a given
     * channel (i.e. BaseInode( LFSpeed) = BaseInode(LFForce)):
     *
     *      Channels Per Board=4, Sub Channel Count=10
     *
     *  Board Index     Channel Index   Base Inode Value
     *  ================================================
     *          0               0               0
     *          0               1               10
     *          0               2               20
     *          0               3               30
     *          1               0               40
     *          1               1               50
     *          1               2               60
     *          1               3               70
     *          2               0               80
     *          2               1               90
     *          2               2               100
     *          2               3               110
     *
     * @param channelTag Tag of the channel to calculate the base inode value
     *                   for (i.e. LF, RF)
     * @return Inode value for the given channel tag
     */
    ino_t CalculateBaseChnlInode( const dataTag_t &channelTag) const;

    /**
     * Encodes a (board,channel) pair into a unique index key
     *
     * @param brdIdx  Board index to be encoded
     * @param chnlIdx Channel index to be encoded
     * @return Unique index key encoding for a (board,channel) pair
     */
    uint32_t EncodeChnlIndexKey( uint16_t brdIdx, uint16_t chnlIdx);

    /**
     * Decodes unique index key into a (board,channel) pair
     *
     * @param key     Key to be decoded
     * @param brdIdx  Decoded board index (Output)
     * @param chnlIdx Decoded channel index (Output)
     */
    void DecodeChnlIndexKey( uint32_t key, uint16_t &brdIdx, uint16_t &chnlIdx);

    /**
     * Process new data from the ICM after an interrupt event
     */
    void ProcessIsr();

    /**
     * Reads the time, count and direction values for each ICM in the
     * icmModList
     *
     * @param icmModList List of ICM objects to read data from
     */
    void SampleData( IpIcmList_t &icmModList);

    /**
     * Reads the time, count and direction values from the given ICM
     *
     * @param ipIcm  ICM to read data from
     */
    void SampleData();

    /**
     * Process data after the ISR has signalled that more samples are
     * available
     *
     * @param icmModList list of pointers to IpIcm objects to read data from
     */
    void ProcessData( IpIcmList_t &icmModList);

    /**
     * Calculates instantaneous channel values (distance, speed, freq, etc) using adjacent data samples
     *
     * @param sample0 Sample just prior to sample0
     * @param sample1 Most recent data sample
     * @param ipIdx   Index of the IP module we are performing calculations on
     * @param chnlIdx The index of the channel we are calculating data for
     */
    void CalculateInstData( IcmChannelData_t *sample0, IcmChannelData_t *sample1, uint16_t ipIdx, uint16_t chnlIdx);

    /**
     * Calculates average values for wheel data
     *
     * @param avg       Sample to store the averaged data in
     * @param ipIdx     The index of the IP ICM we are averaging data from
     * @param chnlIdx   The channel on the ICM we are averaging data from
     * @param sampleIdx The lates sample index to use for calculating average values
     */
    void CalculateAvgData( IcmChannelData_t &avg, int ipIdx, int chnlIdx, int sampleIdx);


private:
    /**
     * PCI40 IP carrier card on which our IP ICM(s) are located
     */
    DE10Nano                   m_nano;

    /**
     * List of ICM IP modules detected
     */
    IpIcmList_t             m_icmIps;

    /**
     * List of channel names (i.e. LF, RF, etc)
     */
    StringList_t            m_channelNames;

    /**
     * list of subchannel attributes (name, read only, etc)
     */
    IcmSubChannelList_t     m_subChannels;

    /**
     * list of names to adopt for aour array data
     */
    StringList_t            m_dataArrayNames;

    /**
     * List of names to adopt for our block data
     */
    StringList_t            m_blockDataNames;

    /**
     * Instantaneous data values read from the ICM board(s)
     * (BoardIndex x ChannelIndex x SubchannelIndex)
     */
    InstDataArray_t         m_instData;

    /**
     * Array of distance data
     */
    float*                  m_distArray;

    /**
     * Array of speed data
     */
    float*                  m_speedArray;

    /**
     * Array of frequency data
     */
    float*                  m_freqArray;

    /**
     * Array of acceleration data
     */
    float*                  m_accelArray;

    /**
     * Array of force data
     */
    float*                  m_forceArray;

    /**
     * Maximum number of elements in the data arrays
     */
    uint32_t                m_arraySize;

    /**
     * Index at which the next data sample will be added to the data arrays
     */
    uint32_t                m_arrayIndex;

    /**
     * Index of the last sample saved to the ascii data log files
     */
    uint32_t                m_lastDataLogIndex;

    /**
     * Base inode number for the instantaneous paths
     */
    ino_t                   m_baseInstInode;

    /**
     * Base inode number for the array paths
     */
    ino_t                   m_baseArrayInode;

    /**
     * Base inode number for the block-data paths
     */
    ino_t                   m_baseBlockInode;

    /**
     * Mountpoint attributes for the array data devices
     */
    iofunc_mount_t          m_arrayMountPoint;

    /**
     * Mountpoint attributes for the block data devices
     */
    iofunc_mount_t          m_blockMountPoint;

    /**
     * The structures used to patch in our OCB alloc/free functions
     */
    vector< IOFUNC_ATTR_T*> m_ioFuncAttrs;

    /**
     * Device numbers used for mounting paths (/dev/Icm/LF, etc)
     */
    vector< int>            m_icmDevNos;

    /**
     * Device numbers used for mounting array paths (/dev/Icm/DistanceArray, etc)
     */
    vector< int>            m_arrayDevNos;

    /**
     * Device numbers used for mounting block paths (/dev/Icm/Block/Speed, etc)
     */
    vector< int>            m_blockDevNos;

    /**
     * List oif valid channels and their associated channel/board  indeces
     */
    HwDataMap               m_channelIndexes;

    /**
     * List of channel mappings (i.e. LF is mapped to board 2 channel 3).
     * The indices into this vector are:
     * LF Channel Mapping = 0
     * RF Channel Mapping = 1
     * LR Channel Mapping = 2
     * RR Channel Mapping = 3
     * LT Channel Mapping = 4
     * RT Channel Mapping = 5
     *
     * @see IcmChannelIdx_t
     */
    vector< uint32_t>       m_channelIdxMap;

    /**
     * Named Data Broker interface object used to read/write system data
     */
    INamedDataBroker        *m_dataBroker;

    /**
     * Flag to maintain zerospeed status
     */
    bool                    m_zeroSpeed;

    /**
     * Flag used to tell if we have published Zerospeed status
     * upon initialization
     */
    bool                    m_zspdPublished;

    /**
     * Log file structure for logging distance
     */
    IcmLogFile              m_distanceLog;

    /**
     * Log file structure for logging speed
     */
    IcmLogFile              m_speedLog;

    /**
     * Log file structure for logging brake force
     */
    IcmLogFile              m_brakeForceLog;

    /**
     * Log file structure for logging acceleration
     */
    IcmLogFile              m_accelLog;

    /**
     * Log file structure for logging frequency
     */
    IcmLogFile              m_freqLog;

    /**
     * Object used to insert a data tag into the ascii log file. This
     * is sent to us from a client in an XML message and is used to tag
     * certain data points in the arrays (like firing valves). In
     * response to the client's message, we return the array index to
     * which the tag name is assigned (the current array index).
     */
    IcmTagList              m_dataTagList;

    /**
     * Flag used to indicate if array logging is urrently active
     */
    bool                    m_arrayLogActive;

    /**
     * Mapping of ICM data tag names to system data tag names
     */
    DataTagMap              m_tagList;

    /**
     * Value for the zerospeed limit. Any wheel at a speed less than
     * this value is considered to be at zerospeed
     */
    float                   m_zeroSpeedVal;

    /**
     * Counter used to filter zerospeed to the PLC
     */
    uint32_t                m_zeroSpeedCount;

    /**
     * Filtered zerospeed value
     */
    bool                    m_filteredZerospeed;

    /**
     * Currently active value for the overspeed limit
     */
    float                   m_currentOverSpeedVal;

    /**
     * System wide default value for the overspeed limit
     */
    float                   m_defaultOverSpeedVal;

    /**
     * Flag to indicate if overspeed is currently active
     */
    bool                    m_overSpeedActive;

    /**
     * Mutex used to guard access to the instantaneous data item values
     */
    BepMutex                m_dataLock;

    /**
     * File name to store machine loss values in
     */
    std::string             m_lossFileName;

    /**
     * Priority to run our process at
     */
    int                     m_priority;

    /**
     * Number of samples to use for our moving average
     */
    uint16_t                m_sampleDelta;

    /**
     * Number of milliseconds between consecutive samples
     */
    uint32_t                m_samplePeriod;

    /**
     * FIFO to be filled in by the ISR with ICM data samples
     */
    IsrSampleFifo           m_isrFifo;

    /**
     * Constants used to convert frequencies into ICM data (speed, force, etc)
     */
    ConversionFactors_t     m_convFactors;

    /**
     * List of attributes for each roller, indexed by (board,wheel):
     * roll factor, encoder pulses and roll diameter
     */
    vector< RollAttribs_t>  m_rollAttribs;

    /**
     * Event used by the ISR thread to infomr us that new data is available
     */
    struct sigevent         m_newDataEvent;

    /**
     * ID returned from InterruptAttachEvent()
     */
    int                     m_irqId;

    /**
     * Timer used to monitor interrupt signal timeouts
     */
    IsrWatchdogTimer        m_isrWatchdog;

    /**
     * Event to be delivered when the ISR watchdog timer fires
     */
    struct sigevent         m_watchdogEvent;

    /**
     * Flag used to indicate whether we should wait for the Register message
     * before calling Register(), or if we should call it immediately
     */
    bool                    m_waitToRegister;

    /**
     * Timer used to sample ICM data if not using hardware interrupts
     */
    IcmTimer                m_pseudoIsrTimer;

    /**
     * Counter used to adjust the PLC speed publish rate.
     */
    uint8_t                 m_plcSpeedCount;

    /**
     * Flag that indicates if the encoders are single phase or
     * multiple phase.
     */
    bool                    m_encoderPhasing;

    /**
     * Flag that indicates if we are running a loss comp cycle. Needed so
     * we dont sound the accel alarm during a loss comp cycle.
     */
    bool                    m_lossComp;

    /**
     * Acceleration fault limit. If any of the acceleration values exceeds
     * this value, we will sound the PLC horn. Set to 0 to disable accel monitoring
     */
    float                   m_accelLimit;

    /**
     * Counter used to "beep" the accel fault horn
     */
    int                     m_accelAlarmTime;

    /**
     * Number of samples above accel limit before alarm is sounded
     */
    int                     m_accelAlarmFilterOnFilterCnt;

    /**
     * Number of samples to wait before silencing the accele alarm after
     * the acccel value is below the accel limit
     */
    int                     m_accelAlarmFilterOffFilterCnt;

    /**
     * Timestamp of the last active alarm 'on' beep
     */
    uint64_t                m_accelAlarmOnTime;

    /**
     * Timestamp of the last active alarm 'off' beep
     */
    uint64_t                m_accelAlarmOffTime;

    /**
     * State of the accel alarm
     */
    AccelAlarmState_t       m_accelAlarmState;

    /**
     * Current value of the accel alarm
     */
    std::string             m_alarmValue;

    /**
     * Data block used to hold the current drag forces. Used when caclulating
     * the "net" force.
     */
    float                   m_currDrags[ 6];

    /**
     * disabled (due to a licenseing problem)
     */
    bool                    m_disableSystem;
    bool                    m_reloadingConfigs;
	bool 					m_publish;
	bool 					m_sending;

	int m_portNum;
    int m_aliveCount;
	int m_zeroSamples[6];
};


#endif // _IP_ICM_MGR_H_

