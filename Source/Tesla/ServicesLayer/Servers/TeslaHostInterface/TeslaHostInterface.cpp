//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  The  Tesla Host Interface Server will interface to the Tesla DVT.
//  Messages will be received from the DVT and processed.  
//  A response will be generated to transmit back to the DVT.
//
//===========================================================================
// Copyright (c) 2017- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
// 
//*************************************************************************
#include "TeslaHostInterface.h"
#include <algorithm>


//-------------------------------------------------------------------------------------------------
TeslaHostInterface::TeslaHostInterface() : BepServer(), m_teslaComm(NULL)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
TeslaHostInterface::~TeslaHostInterface()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void TeslaHostInterface::Initialize(const string &fileName)
{
	BepServer::Initialize(fileName);
}

//-------------------------------------------------------------------------------------------------
void TeslaHostInterface::Initialize(const XmlNode *document)
{
	BepServer::Initialize(document);
	m_config = document->Copy();
	Log(LOG_FN_ENTRY, "TeslaHostInterface::Initialize() - Enter");
	// Setup the periodic messsage timer
	int interval = 1000;
	try
	{
		interval = BposReadInt(document->getChild("Setup/Parameters/PeriodicMessageInterval")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Periodic message interval not defined, using 1000ms - %s", excpt.GetReason());
	}
	SetupTimer(m_scheduledMessageTimer, interval, SCHEDULED_MSG_PULSE);
	// Setup the ICM data update timer
	interval = 500;
	try
	{
		interval = BposReadInt(document->getChild("Setup/Parameters/IcmUpdateInterval")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "ICM Update interval not defined, using 500ms - %s", excpt.GetReason());
	}
	SetupTimer(m_icmDataTimer, interval, ICM_DATA_PULSE);
	// Setup common messages
	m_periodicMsg = new TeslaDataMessage(8, PERIODIC_MSG);
	m_brakeResultMsg = new TeslaDataMessage(4, FORCE_MEASUREMENTS, BRAKE_FORCE_TEST);
	m_dragResultMsg = new TeslaDataMessage(4, FORCE_MEASUREMENTS, DRAG_FORCE_TEST);
	m_absResultMsg = new TeslaDataMessage(8, ABS_RESULTS);
	m_ackMsg.reserve(5);
    m_ackMsg.resize(5);
	m_ackMsg[0] = STX;
	m_ackMsg[1] = DvtACK;
	m_ackMsg[3] = ETX;
	Log(LOG_FN_ENTRY, "TeslaHostInterface::Initialize() - Exit");
}

//-------------------------------------------------------------------------------------------------
void TeslaHostInterface::Run()
{	// Wait for the serial channel to be created
	while(m_teslaComm == NULL)  BposSleep(1000);
	BposSleep(1000);   // Give the channel time to be opened and established
	// Loop until we are terminated
	SerialString_t message;
	while(BEP_TERMINATE != GetStatus())
	{	// Wait for a message from the DVT
		message.clear();
		INT32 byteCount = m_teslaComm->ReadPort(message, 1000, 50);
		// Clear the comm port for the next message
		m_teslaComm->ResetConnection();
		if(byteCount > 0)
		{	// Received a message, process it
			char buff[8];
			string strMessage;
			for(UINT16 index = 0; index < message.length(); index++)
			{
				strMessage += CreateMessage(buff, sizeof(buff), "%02X ", message[index]);
			}
			Log(LOG_DEV_DATA, "Received %d bytes from TESLA: %s", byteCount, strMessage.c_str());
			// Process request
			ProcessIncomingMessage(message);
		}
		else
		{
			Log(LOG_DETAILED_DATA, "Timeout waiting for a Tesla request!");
		}
	}
}

//-------------------------------------------------------------------------------------------------
const string TeslaHostInterface::Register(void)
{
	string result = BepServer::Register();
	Log(LOG_FN_ENTRY, "TeslaHostInterface::Register() - Enter");
	if(m_teslaComm == NULL)
	{
		m_teslaComm = new SerialChannel();
		m_teslaComm->Initialize(m_config->getChild("Setup/TeslaCommunications"));
	}
	m_icmData = new IcmDataSupplier(m_config->getChild("Setup/IcmData"), 4);
	Log(LOG_DEV_DATA, "Opened ICM data files: %s", ConvertStatusToResponse(m_icmData->OpenIcmDataFiles()).c_str());
	m_icmDataTimer.Start();
	m_scheduledMessageTimer.Start();
	Log(LOG_FN_ENTRY, "TeslaHostInterface::Register() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
const INT32 TeslaHostInterface::HandlePulse(const INT32 code, const INT32 value)
{
	INT32 retVal = BEP_STATUS_ERROR;
	Log(LOG_DETAILED_DATA, "TeslaHostInterface::HandlePulse(code: %d, value: %d)", code, value);
	switch(code)
	{
	case TESLA_HI_PULSE_CODE:
		switch(value)
		{
		case ICM_DATA_PULSE:
			float forces[4];
			float speeds[4];
			retVal = m_icmData->GetForces(forces);
			if(BEP_STATUS_SUCCESS == retVal)
			{
				retVal = m_icmData->GetWheelSpeeds(speeds);
				if(BEP_STATUS_SUCCESS == retVal)
				{
					m_periodicMsg->UpdateMessageData(0, speeds, 4);
					m_periodicMsg->UpdateMessageData(4, forces, 4);
				}
			}
			break;

		case SCHEDULED_MSG_PULSE:
			if((m_teslaComm != NULL) && (m_periodicMsg != NULL))
			{
				SerialString_t msg = m_periodicMsg->GetMessage();
				m_teslaComm->Send(msg);
			}
			break;

		default:
			Log(LOG_ERRORS, "Unhandled pulse value (%d)", value);
			break;
		}
		break;

	default:
		// Call the base class to handle the pulse
		retVal = BepServer::HandlePulse(code, value);
		break;
	}
	Log(LOG_DETAILED_DATA, "TeslaHostInterface::HandlePulse(code: %d, value: %d) - Exit", code, value);
	return retVal;
}

//-------------------------------------------------------------------------------------------------
void TeslaHostInterface::SetupTimer(BepTimer &timer, int interval, int pulseValue)
{
	timer.SetPulseCode(TESLA_HI_PULSE_CODE);
	timer.SetPulseValue(pulseValue);
	timer.Initialize(GetProcessName(), mSEC_nSEC(interval), mSEC_nSEC(interval));
	timer.Stop();
}

//-------------------------------------------------------------------------------------------------
void TeslaHostInterface::ProcessIncomingMessage(SerialString_t message)
{
	UINT8 ackType = DvtACK;
	bool sendAck = true;
	if(IsMessageValid(message))
	{
		switch(message[MSG_TYPE_BYTE])
		{
		case START_FORCE_TEST:
		case STOP_FORCE_TEST:
			{
				Log(LOG_DEV_DATA, "Processing Start/Stop force measurement");
				string markerName = "Unknown";
				bool setMarker = true;
				switch(message[MSG_SUBTYPE_BYTE])
				{
				case DRAG_FORCE_TEST:
					markerName = message[MSG_TYPE_BYTE] == START_FORCE_TEST ? DRAG_START : DRAG_END;
					break;

				case BRAKE_FORCE_TEST:
					markerName = message[MSG_TYPE_BYTE] == START_FORCE_TEST ? BRAKE_START : BRAKE_END;
					break;

				default:
					setMarker = false;
					Log(LOG_ERRORS, "Unknown force type: %02X", message[MSG_SUBTYPE_BYTE]);
					break;
				}
				if(setMarker)
				{
					DvtMarker *marker = new DvtMarker();
					marker->name = markerName;
					marker->index = m_icmData->TagArray(markerName);
					m_dvtMarkers.push_back(marker);
					Log(LOG_DEV_DATA, "Set marker %s at %d", markerName.c_str(), marker->index);
				}
			}
			break;

		case FORCE_MEASUREMENTS:
			{
				Log(LOG_DEV_DATA, "Processing force measurement message");
				string markerStartName = BRAKE_START;
				string markerStopName = BRAKE_END;
				TeslaDataMessage *dataMessage = m_brakeResultMsg;
				if(message[MSG_SUBTYPE_BYTE] == DRAG_FORCE_TEST)
				{
					markerStartName = DRAG_START;
					markerStopName = DRAG_END;
					dataMessage = m_dragResultMsg;
				}
				CalculateMeasuredForces(markerStartName, markerStopName, dataMessage);
				SerialString_t msg = dataMessage->GetMessage();
				m_teslaComm->Send(msg);
				sendAck = false;
			}
			break;

		case START_ABS_TEST:
		case STOP_ABS_TEST:
			{
				Log(LOG_DEV_DATA, "Process ABS Start/Stop message");
				DvtMarker *absMarker = new DvtMarker();
				absMarker->name = message[MSG_TYPE_BYTE] == START_ABS_TEST ? ABS_START : ABS_END;
				absMarker->index = m_icmData->TagArray(absMarker->name);
				m_dvtMarkers.push_back(absMarker);
				Log(LOG_DEV_DATA, "Set ABS marker %s at %d", absMarker->name.c_str(), absMarker->index);
			}
			break;

		case ABS_RESULTS:
			{
				Log(LOG_DEV_DATA, "Processing ABS Results message");
				CalculateAbsResults();
				SerialString_t msg = m_absResultMsg->GetMessage();
				m_teslaComm->Send(msg);
				sendAck = false;
			}
			break;

		case TEST_CYCLE_CONTROL:
			Log(LOG_DEV_DATA, "Processing test start/end message");
			if(message[MSG_SUBTYPE_BYTE] == START_TEST)
			{
				for(UINT8 index = 0; index < m_dvtMarkers.size(); index++)
				{
					delete m_dvtMarkers[index];
					m_dvtMarkers[index] = NULL;
				}
				m_dvtMarkers.clear();
				Log(LOG_DEV_DATA, "Test start - cleared DVT markers");
			}
			break;

		default:
			Log(LOG_ERRORS, "Unknown message type: %02X", message[MSG_TYPE_BYTE]);
			ackType = DvtNAK;
			break;
		}
	}
	else
	{
		Log(LOG_ERRORS, "Received invalid message");
		ackType = DvtNAK;
	}
	if(sendAck)  SendAckMsg(ackType, message[MSG_TYPE_BYTE]);
}

//-------------------------------------------------------------------------------------------------
bool TeslaHostInterface::IsMessageValid(SerialString_t message)
{
	return CalculateCheckSum(message) == message[message.size() - 1];
}

//-------------------------------------------------------------------------------------------------
UINT8 TeslaHostInterface::CalculateCheckSum(SerialString_t message)
{
	UINT8 chkSum = 0x00;
	for(UINT8 index = 0; index < message.size() - 1; index++)
	{
		chkSum += message[index];
	}
	return chkSum;
}

//-------------------------------------------------------------------------------------------------
void TeslaHostInterface::SendAckMsg(UINT8 ackType, UINT8 msgType)
{
	Log(LOG_FN_ENTRY, "TeslaHostInterface::SendAckMsg(axkType: %02X, msgType: %02X) - Enter", ackType, msgType);
	m_ackMsg[1] = ackType;
	m_ackMsg[2] = msgType;
	m_ackMsg[4] = CalculateCheckSum(m_ackMsg);
	m_teslaComm->Send(m_ackMsg);
	Log(LOG_FN_ENTRY, "TeslaHostInterface::SendAckMsg(axkType: %02X, msgType: %02X) - Exit", ackType, msgType);
}

//-------------------------------------------------------------------------------------------------
void TeslaHostInterface::CalculateMeasuredForces(string startTag, string endTag, TeslaDataMessage *resultMsg)
{
	Log(LOG_FN_ENTRY, "Calculating measured forces - startTag: %s, endTag: %s", startTag.c_str(), endTag.c_str());
	INT32 startIndex = FindDvtMarker(startTag);
	INT32 endIndex = FindDvtMarker(endTag);
	if((startIndex != -1) && (endIndex != -1))
	{
		IcmDataArray dataArray;
		if(BEP_STATUS_SUCCESS == m_icmData->ReadForceArray(startIndex, endIndex, dataArray))
		{   // Calculate the forces
			for(int roller = LFWHEEL; roller <= RRWHEEL; roller++)
			{
				float forceSum = 0.0;
				for(UINT8 index = 0; index < dataArray[roller].size(); index++)
				{
					forceSum += dataArray[roller][index];
				}
				float avgForce = forceSum / dataArray[roller].size();
				Log(LOG_DEV_DATA, "Calculated force for roller %d = %.2f", roller, avgForce);
				resultMsg->UpdateMessageData(roller, (INT16)(avgForce * 10.0));
			}
		}
		else
		{
			Log(LOG_ERRORS, "Failed to read force array");
		}
	}
	else
	{
		Log(LOG_ERRORS, "Could not find start or end index - start index: %d, end index: %s", startIndex, endIndex);
	}
}

//-------------------------------------------------------------------------------------------------
void TeslaHostInterface::CalculateAbsResults(void)
{
	Log(LOG_FN_ENTRY, "Calculating ABS forces");
	INT32 startIndex = FindDvtMarker(ABS_START);
	INT32 endIndex = FindDvtMarker(ABS_END);
	if((startIndex != -1) && (endIndex != -1))
	{
		IcmDataArray array;
		if(BEP_STATUS_SUCCESS == m_icmData->ReadForceArray(startIndex, endIndex, array))
		{
			for(int roller = LFWHEEL; roller <= RRWHEEL; roller++)
			{
				vector<float>::iterator arrayIter = min_element(array[roller].begin(), array[roller].end());
				float minVal = *arrayIter;
				arrayIter = max_element(array[roller].begin(), array[roller].end());
				float maxVal = *arrayIter;
				m_absResultMsg->UpdateMessageData(roller*2, (INT16)(minVal * 10.0));
				m_absResultMsg->UpdateMessageData((roller*2)+1, (INT16)(maxVal * 10.0));
				Log(LOG_DEV_DATA, "ABS data for roller %d - Min: %.2f, Max: %.2f", roller, minVal, maxVal);
			}
		}
		else
		{
			Log(LOG_ERRORS, "Failed to read force array");
		}
	}
	else
	{
		Log(LOG_ERRORS, "Could not find start or end index - start index: %d, end index: %s", startIndex, endIndex);
	}
}

//-------------------------------------------------------------------------------------------------
INT32 TeslaHostInterface::FindDvtMarker(string tag)
{
	INT32 tagIndex = -1;
	for(UINT8 index = 0; index < m_dvtMarkers.size(); index++)
	{
		if(m_dvtMarkers[index]->name == tag)
		{
			tagIndex = m_dvtMarkers[index]->index;
			break;
		}
	}
	return tagIndex;
}
