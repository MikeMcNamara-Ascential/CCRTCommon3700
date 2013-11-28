//*************************************************************************
// FILE DESCRIPTION:
//   Machine data broker that is responsible for maintaining and publishing
//   all of the information related to the machine.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef MazdaMachineDataBroker_h
#define MazdaMachineDataBroker_h
//-------------------------------------------------------------------------------------------------
#include "INamedDataBroker.h"
#include "MachineDataBroker.h"
//-------------------------------------------------------------------------------------------------
// the roller names, in order of the way they are provided by the ICM
const static string rollerName[] = {"LF", "RF", "LR", "RR"};

//-------------------------------------------------------------------------------------------------
class MazdaMachineDataBroker : public MachineDataBroker
{
public:
    MazdaMachineDataBroker();
    virtual ~MazdaMachineDataBroker();

    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
     *		   BEP_SUCCESS_RESPONSE == Registered
     */
    virtual const string Register(void);

    /**
     * Process the reload configuration pulse.  If any other pulses
     * are received, send them on to BepComponent.
     *
     * @param code   Pulse code
     * @param value  Pulse value
     * @return If pulse is handled.
     *         Handled     == BEP_STATUS_SUCCESS
     *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);

    /**
     * Update the ICM data.
     * The maximum roller speed will be found and made available to the system.
     * 
     * @return Status of updating the current maximum roller speed.
     */
    virtual INT32 IcmDataPulseHandler(void);

    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     */
    virtual const string Publish(const XmlNode *node);

private:
    /**
     * Get the current load cell readings from the PLC.  The raw values will be converted to usable values.
     * 
     * @return Status of reading and processing the raw load cell values.
     */
    const INT32 ProcessLoadCellReadings(void);

    /**
     * Setup the timer for the load cell value processing.
     * 
     * @param config Configuration data for the timer.
     */
    void SetupLoadCellTimer(const XmlNode *config);

    /**
     * Store the load cell conversion factors from the configuration data.
     * 
     * @param factors Load cell conversion factors.
     */
    void StoreLoadCellConversionFactors(const XmlNode *factors);

    /**
     * Store the parameters that will be needed to convert encoder pulses to distance.
     * 
     * @param params Distance calculation parameters.
     */
    void StoreDistanceParameters(const XmlNode *params);

    /**
     * Get/Set the encoder pulse count.
     * 
     * @param pulseCount Encoder pulse count.
     * 
     * @return Encoder pulse count.
     */
    const UINT16& EncoderPulseCount(const UINT16 *pulseCount = NULL);

    /**
     * Get/Set the roller diameter.
     * 
     * @param diameter Roller diameter.
     * 
     * @return Roller diameter.
     */
    const float RollerDiameter(const float *diameter = NULL);

#define LOAD_CELL_PULSE     20

    /** Conversion factors to apply to the raw load cell readings */
    typedef struct _loadCellStruct
    {
        float  conversionFactor;    // Conversion factor to apply to the raw load cell reading
        string rawSystemTag;        // Tag for reading the raw load cell value from the system
        string systemTag;           // Tag to use for supplying the converted load cell data to the system
		string displayTag;          // Tag to use for displaying the data on the screen
		float  displayConversion;   // Conversion factor for the displayed data
    }LoadCellConversionFactors;

    vector<LoadCellConversionFactors> m_loadCellConversionFactors;
    /** Timer to handle load cell readings */
    BepTimer m_loadCellTimer;
    /** System data object */
    INamedDataBroker *m_ndb;
    /** Initial distance for the test sequence */
    float m_initialDistanceReading[4];
    /** Encoder pulse count */
    UINT16 m_encoderPulseCount;
    /** Roller diameter */
    float m_rollerDiameter;
    /** Multiplier to use for max roller speed - this helps accomodate different tire sizes */
    XmlNodeMap m_maxRollerSpeedMultipliers;
    /** Current Speed multiplier in use */
    float m_currentSpeedMultiplier;
};
//-------------------------------------------------------------------------------------------------
#endif //MazdaMachineDataBroker_h
