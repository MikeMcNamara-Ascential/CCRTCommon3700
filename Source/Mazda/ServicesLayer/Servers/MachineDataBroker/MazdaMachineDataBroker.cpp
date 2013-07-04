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
#include "MazdaMachineDataBroker.h"

//-------------------------------------------------------------------------------------------------
MazdaMachineDataBroker::MazdaMachineDataBroker() : MachineDataBroker(), m_ndb(NULL)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MazdaMachineDataBroker::~MazdaMachineDataBroker()
{   // Stop the load cell timer
    m_loadCellTimer.Stop();
    // Get rid of the NamedDataBroker comm object
    if(m_ndb != NULL)  delete m_ndb;
    m_ndb = NULL;
    // Clear the speed multipliers
    m_maxRollerSpeedMultipliers.clear(true);
}

//-------------------------------------------------------------------------------------------------
void MazdaMachineDataBroker::Initialize(const XmlNode *document)
{   // Call the base class to begin the initialization sequence
    MachineDataBroker::Initialize(document);
    // Setup the timer for the load cells
    Log(LOG_FN_ENTRY, "MazdaMachineDataBroker::Initialize() - Enter");
    SetupLoadCellTimer(document->getChild("Setup/LoadCellData"));
    // Store the load cell conversion factors
    StoreLoadCellConversionFactors(document->getChild("Setup/LoadCellData/Conversions"));
    // Store the data parameters for distances
    StoreDistanceParameters(document->getChild("Setup/RollerDistanceParameters"));
    // Store the speed multipliers
    try
    {
        m_maxRollerSpeedMultipliers.DeepCopy(document->getChild("Setup/MaxRollerSpeedMultipliers")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No speed multipliers defined, using 1.0 for speed multiplier for all vehicles");
        m_maxRollerSpeedMultipliers.clear(true);
    }
    // Set the speed multiplier
    m_currentSpeedMultiplier = 1.0;
    Log(LOG_FN_ENTRY, "MazdaMachineDataBroker::Initialize() - Exit");
}

//-------------------------------------------------------------------------------------------------
const string MazdaMachineDataBroker::Register(void)
{   // Call the base class to begin the registration
    string result = MachineDataBroker::Register();
    // Create a new NamedDataBroker interface
    if(m_ndb == NULL)
        m_ndb = new INamedDataBroker();
    // Start the load cell timer
    Log(LOG_DEV_DATA, "Starting load cell update timer");
    m_loadCellTimer.Start();
    return result;
}

//-------------------------------------------------------------------------------------------------
const INT32 MazdaMachineDataBroker::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 status = BEP_STATUS_ERROR;
    switch(code)
    {
    case MACHINE_DATA_BROKER_PULSE_CODE:
        switch(value)
        {
        case LOAD_CELL_PULSE:
            status = ProcessLoadCellReadings();
            break;

        default:
            status = MachineDataBroker::HandlePulse(code, value);
            break;
        }
        break;

    default:
        status = BepServer::HandlePulse(code, value);
        break;
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
INT32 MazdaMachineDataBroker::IcmDataPulseHandler(void)
{   // Get the current wheel speeds
    float rollerSpeeds[RollerCount()];
    float maxRollerSpeed = 0.0;
    BEP_STATUS_TYPE status = m_icmData->GetWheelSpeeds(rollerSpeeds);
    // Log the roller speeds
    for(UINT8 index = 0; index < RollerCount(); index++)
    {
        Log(LOG_DEV_DATA, "Roller %d speed: %.2f", index, rollerSpeeds[index]);
        maxRollerSpeed = fabs(rollerSpeeds[index]) > fabs(maxRollerSpeed) ? rollerSpeeds[index] : maxRollerSpeed;
    }
    maxRollerSpeed *= (GetCurrentSpeedScalingFactor() * m_currentSpeedMultiplier);
    // Store the maximum roller speed for anyone who wants to read it
    char buff[32];
    string maxSpeed(CreateMessage(buff, sizeof(buff), "%.2f", maxRollerSpeed));
    Log(LOG_DEV_DATA, "Setting max roller speed to %s", maxSpeed.c_str());
    SetData(string("MaxRollerSpeed"), maxSpeed);

    // Get the current distances
    float currentDistance[RollerCount()];
    status = m_icmData->GetWheelDistances(currentDistance);
    if(BEP_STATUS_SUCCESS == status)
    {
        if(ReadSubscribeData(ROLLS_UP_DATA_TAG).compare("1"))
        {   // Rolls are down, set initial distance to current distance
            for(UINT8 index = LFWHEEL; index < RollerCount(); index++)
            {
                m_initialDistanceReading[index] = currentDistance[index];
            }
        }
        // Rolls are up, calculate the new distance
        for(UINT8 index = LFWHEEL; index < RollerCount(); index++)
        {   // Calculate the new distance
            float diff = currentDistance[index] - m_initialDistanceReading[index];
            // Convert the pulses to a distance reading
            float circumference = 3.1415927 * RollerDiameter();
            float revs = diff / (float)EncoderPulseCount();
            float distMeters = (revs * circumference) / 39.36996;
            // Write the data to the system
            char buff[64];
            Log(LOG_DEV_DATA, "Current Distance - %s: %.2f - {C: %.2f, Revs: %.2f, Diff: %.2f, CD: %.2f, ID: %.2f}", 
                rollerName[index].c_str(), distMeters, circumference, revs, diff, 
                currentDistance[index], m_initialDistanceReading[index]);
            SetData(rollerName[index] + "CurrentDistance", CreateMessage(buff, sizeof(buff), "%.2f", distMeters));
        }
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
const string MazdaMachineDataBroker::Publish(const XmlNode *node)
{   // Check if this is a new body style
    if(!node->getName().compare(BODY_STYLE_TAG))
    {   // Look up the new multiplier
        XmlNodeMapCItr iter = m_maxRollerSpeedMultipliers.find(node->getValue());
        if(iter != m_maxRollerSpeedMultipliers.end())
        {
            m_currentSpeedMultiplier = atof(iter->second->getValue().c_str());
        }
        else
        {
            Log(LOG_ERRORS, "Speed multiplier not defined for %s", node->getValue().c_str());
            m_currentSpeedMultiplier = 1.0;
        }
    }
    // Let the base class complete the Publish
    return MachineDataBroker::Publish(node);
}

//-------------------------------------------------------------------------------------------------
const INT32 MazdaMachineDataBroker::ProcessLoadCellReadings(void)
{
    INT32 status = BEP_STATUS_SUCCESS;
    string response;
    string value;
    char buff[32];
    // Read the raw data from the system
    for(vector<LoadCellConversionFactors>::iterator iter = m_loadCellConversionFactors.begin(); 
         iter < m_loadCellConversionFactors.end(); iter++)
    {
        if(BEP_STATUS_SUCCESS == m_ndb->Read(iter->rawSystemTag, response, true))
        {
            if(BEP_STATUS_SUCCESS == m_ndb->GetByTag(iter->rawSystemTag, value, response))
            {   // Convert the data item
				short tempData = atoi(value.c_str());
                float data = tempData * iter->conversionFactor;
                // Write the data so others can read it
                SetData(iter->systemTag, CreateMessage(buff, sizeof(buff), "%.2f", data));
                Log(LOG_DEV_DATA, "Load Cell Update: %s == %.2f [%s]", iter->systemTag.c_str(), data, value.c_str());
            }
            else
            {
                status = BEP_STATUS_FAILURE;
                Log(LOG_ERRORS, "Error getting data value from %s message", iter->rawSystemTag.c_str());
            }
        }
        else
        {
            status = BEP_STATUS_FAILURE;
            Log(LOG_ERRORS, "Could not read %s", iter->rawSystemTag.c_str());
        }
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
void MazdaMachineDataBroker::SetupLoadCellTimer(const XmlNode *config)
{
    UINT64 dataDelay = 0;
    try
    {
        dataDelay = BposReadInt(config->getChild("LoadCellUpdateDelay")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        dataDelay = 1000;
        Log(LOG_ERRORS, "Load cell update delay not found, defaulting to 1s - %s", excpt.GetReason());
    }
    // Setup the timer
    m_loadCellTimer.SetPulseCode(MACHINE_DATA_BROKER_PULSE_CODE);
    m_loadCellTimer.SetPulseValue(LOAD_CELL_PULSE);
    m_loadCellTimer.Initialize(GetProcessName(), mSEC_nSEC(dataDelay), mSEC_nSEC(dataDelay));
    m_loadCellTimer.Stop();
}

//-------------------------------------------------------------------------------------------------
void MazdaMachineDataBroker::StoreLoadCellConversionFactors(const XmlNode *factors)
{
    const XmlNodeMap &data = factors->getChildren();
    m_loadCellConversionFactors.reserve(data.size());
    m_loadCellConversionFactors.resize(data.size());
    UINT8 roller = 0;
    for(XmlNodeMapCItr iter = data.begin(); iter != data.end(); iter++)
    {
        m_loadCellConversionFactors[roller].conversionFactor = atof(iter->second->getValue().c_str());
        m_loadCellConversionFactors[roller].rawSystemTag = iter->second->getAttribute("RawData")->getValue();
        m_loadCellConversionFactors[roller].systemTag = iter->second->getAttribute("SystemTag")->getValue();
        roller++;
    }
}

//-------------------------------------------------------------------------------------------------
void MazdaMachineDataBroker::StoreDistanceParameters(const XmlNode *params)
{   // Set the pulse count
    UINT16 pulseCount = 0;
    try
    {
        pulseCount = BposReadInt(params->getChild("EncoderPulsesPerRevolution")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        pulseCount = 1250;
        Log(LOG_ERRORS, "Encoder pulse count not specified, using 1250 as default");
    }
    EncoderPulseCount(&pulseCount);
    // Set the roller diameter
    float diameter = 0.0;
    try
    {
        diameter = atof(params->getChild("RollerDiameter")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        diameter = 10.0;
        Log(LOG_ERRORS, "Roller diameter not specified, defaulting to 10");
    }
    RollerDiameter(&diameter);
}

//-------------------------------------------------------------------------------------------------
inline const UINT16& MazdaMachineDataBroker::EncoderPulseCount(const UINT16 *pulseCount /*= NULL*/)
{
    if(pulseCount != NULL)  m_encoderPulseCount = *pulseCount;
    return m_encoderPulseCount;
}

//-------------------------------------------------------------------------------------------------
inline const float MazdaMachineDataBroker::RollerDiameter(const float *diameter /*= NULL*/)
{
    if(diameter != NULL)  m_rollerDiameter = *diameter;
    return m_rollerDiameter;
}
