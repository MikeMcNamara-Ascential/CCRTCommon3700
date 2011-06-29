//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/SystemScreenSaver/SystemScreenSaver.cpp 1     5/01/07 1:10p Rwiersem $
//
// FILE DESCRIPTION:
//   Special function screen saver.
//
//===========================================================================
// Copyright (c) 2005-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// Possible future enhancements:
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Set up for multiple serial communication ports. Currently we only support one
//  Write the IPC implementation
//  Design and integrate a logical method of processing multiple inputs together
//  Write a unit test.
//  Extend the timer to do a power-off/power-on function when time is ballanced
//  Consider writing a response processor.
//  Add a time delay method in the transmit messages, so we can send multiple.
//
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/SystemScreenSaver/SystemScreenSaver.cpp $
// 
// 1     5/01/07 1:10p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
//
//*************************************************************************

#include "SystemScreenSaver.h"

SystemScreenSaver::SystemScreenSaver():BepServer()
{
   m_triggers.clear();
   m_actions.clear();
   m_videoControl.clear();
   // in milliseconds
   m_videoTimerTime = 1000;
   m_currentMode = "Normal";
   m_modeAccumulator = 0;
   m_waitCount = 0;
   m_switchDelay = 300; // default to 5 minutes.
   m_okDelta = 3600; // default to an hour.
}

SystemScreenSaver::~SystemScreenSaver()
{
}

void SystemScreenSaver::Initialize(const std::string &fileName)
{
   BepServer::Initialize(fileName);
   // note that will call the Initialize below, after making it into a node.
}           

void SystemScreenSaver::Initialize(const XmlNode *document)
{
   // go do the parent initialization first (trust me)
   BepServer::Initialize(document);

   Log(LOG_FN_ENTRY, "SSS::Initialize() - Enter %s\n",document->ToString().c_str());
   ReadParameters(document);
   InitControls();

   m_updateVideoTimer.SetPulseCode( SYSTEM_PULSE+1);
   m_updateVideoTimer.SetPulseValue( TIMER_PULSE); 
   m_updateVideoTimer.Initialize(GetProcessName().c_str(), (m_videoTimerTime * 1000000), 
         (m_videoTimerTime * 1000000));
   m_updateVideoTimer.Stop();

   Log(LOG_FN_ENTRY,"SSS::Initialize() - Exit\n");
}

const std::string SystemScreenSaver::Register(void)
{
   // register the base class
   std::string status = BepServer::Register();
   // start the update timer
   m_updateVideoTimer.Start();

   return(status);
}

void SystemScreenSaver::ReadParameters(const XmlNode *document)
{
   Log(LOG_FN_ENTRY,"SSS::ReadParameters() - Enter\n");

   m_triggers.clear();
   try
   {  // Set Up action for any trigger subscription tags
      Log(LOG_DEV_DATA, "Storing trigger subscription tags: \n");
      try
      {
         m_triggers.DeepCopy(document->getChild("Setup/TagList/SubscribeTags")->getChildren());
         Log(LOG_FN_ENTRY, "Stored all Subscribe Tag Triggers\n");
      }
      catch(BepException &Err)
      {
         Log(LOG_ERRORS, "Error copying trigger tags: %s\n", Err.what());
      }
      catch(...)
      {
         Log(LOG_ERRORS, "Error copying trigger tags: Unknkown Type\n");
      }
   }
   catch(...)
   {  // Do nothing.  Subscribe tags do not need to be declared
      Log(LOG_ERRORS, "No SubscribeTags node found\n");
   }

   m_actions.clear();
   try
   {  // Set Up action translations
      Log(LOG_DEV_DATA, "Storing action translations: \n");
      try
      {
         m_actions.DeepCopy(document->getChild("Setup/Control/Actions")->getChildren());
         Log(LOG_FN_ENTRY, "Stored all action translations\n");
      }
      catch(BepException &Err)
      {
         Log(LOG_ERRORS, "Error copying action translations: %s\n", Err.what());
      }
      catch(...)
      {
         Log(LOG_ERRORS, "Error copying action translations: Unknkown Type\n");
      }
   }
   catch(...)
   {  // Do nothing, no translations available
      Log(LOG_ERRORS, "No Control/Actions node found\n");
   }

   m_videoControl.clear();
   try
   {  // Set Up video controls
      Log(LOG_DEV_DATA, "Storing video controlls \n");
      try
      {
         m_videoControl.DeepCopy(document->getChild("Setup/VideoControl")->getChildren());
         Log(LOG_FN_ENTRY, "Stored all video controls\n");
      }
      catch(BepException &Err)
      {
         Log(LOG_ERRORS, "Error copying video controls: %s\n", Err.what());
      }
      catch(...)
      {
         Log(LOG_ERRORS, "Error copying video controls: Unknkown Type\n");
      }
   }
   catch(...)
   {  // Do nothing, no video available
      Log(LOG_ERRORS, "No VideoControl node found\n");
   }

   // read the mode switch time from the configuration file
   try
   {
      m_switchDelay = BposReadInt(document->getChild(
            "Setup/Control/Timer/ModeSwitch/ModeSwitchTime")->getValue().c_str());
   }
   catch(...)
   {
   }

   // read the acceptable mode switching delta time from the configuration file
   try
   {
      m_okDelta = BposReadInt(document->getChild(
            "Setup/Control/Timer/ModeSwitch/AcceptableDelta")->getValue().c_str());
   }
   catch(...)
   {
   }
}

void SystemScreenSaver::InitControls (void)
{
   // go through m_videoControls and set up the comm channels as required
   // currently we only support one serial channel
   // in the future, we should support more than one.
   Log(LOG_FN_ENTRY,"SSS::InitControls() - Enter.\n");
   for(XmlNodeMapItr vcitr = m_videoControl.begin(); vcitr != m_videoControl.end(); vcitr++)
   {
      if(vcitr->second->getChild("Active")->getValue()=="True")
      {
         Log(LOG_DEV_DATA,"Got a live one!\n");
         if(vcitr->second->getChild("CommunicationType")->getValue() == "Serial")
         {
            // note that we currently only support a single serial control channel
            // set up serial port
            Log(LOG_DEV_DATA,"Initializing Serial Port\n");
            m_videoControlComm.Initialize(vcitr->second);
            Log(LOG_DEV_DATA,"m_videoControlComm is initialized\n");
         }
         else if(vcitr->second->getChild("CommunicationType")->getValue() == "IPC")
         {
            // set up IPC functions
         }
      }
   }
}

void SystemScreenSaver::DoSerialControl(const XmlNode *controlNode, std::string doAction)
{
   bool ackReceived = false;           // not ack, but used to terminate loop
   bool portLocked = false;            // Port locked status
   INT32 attempt = 0, 
         maxAttempts = 3;                    //  for lock and Send only
   SerialString_t commandMessage;
   BEP_STATUS_TYPE transmitStatus;

   Log(LOG_FN_ENTRY,"SSS::DoSerialControl() - Enter\n");
   // build the string we want to send
   m_videoControlComm.GetBinaryMssg(
         controlNode->getChild("TransmitMessages")->
         getChild(doAction)->getValue(), commandMessage);
   // Attempt to transmit the message
   do
   {
      // Lock the port so no other comms interfere
      portLocked = m_videoControlComm.LockPort();
      if(portLocked)
      {
         Log(LOG_DEV_DATA,"Sending: %s\n",commandMessage.c_str());
         // Transmit the message. We are not currently processing responses
         transmitStatus = m_videoControlComm.Send(commandMessage);
         if(transmitStatus == BEP_STATUS_SUCCESS)
         {
            // if we have successfully transmitted, fake it out, so we will leave
            ackReceived = true;
            Log(LOG_DEV_DATA,"Serial command successfully sent\n");
         }
         else
         {
            Log(LOG_ERRORS,"Error transmitting message -- status: %s\n",
                  ConvertStatusToResponse(transmitStatus).c_str());
         }
         // Unlock the port since we have completed this attempt
         portLocked = m_videoControlComm.UnlockPort();
      }
      else
      {   // Log the error
         Log(LOG_ERRORS,"WARNING: Could not lock %s port.\n",
               m_videoControlComm.GetProcessName().c_str());
      }
      BposSleep(10); // delay between retries
   } while((!ackReceived) && (attempt++ <= maxAttempts));
   // Set the return status
   if(!ackReceived)
   {   // port failure, send a log message and fail.
      Log(LOG_ERRORS,"Unable to command monitor on %s port\n", 
            m_videoControlComm.GetProcessName().c_str());
   }
   else
   {   // data sent, set status to success
      Log(LOG_DEV_DATA,"Monitor control command on %s port successful\n", 
            m_videoControlComm.GetProcessName().c_str());
   } 
   Log(LOG_FN_ENTRY,"SSS::DoSerialControl() - Exit\n");
}

// to be written at a later date
void SystemScreenSaver::DoIpcControl(const XmlNode *controlNode)
{
   // send Node
}

// this translates the action configured in the subscribe tag node
// to a video control type.
std::string SystemScreenSaver::DoAction(std::string thisAction)
{
   std::string retval;

   Log(LOG_FN_ENTRY,"SSS::DoAction - Enter {%s}\n", thisAction.c_str());
   try
   {
      XmlNodeMapItr actionitr = m_actions.find(thisAction);
      if(actionitr != m_actions.end())
      {
         Log(LOG_DEV_DATA,"Found it\n");
         try
         {
            retval = actionitr->second->getChild("Message")->getValue();
            m_currentMode = actionitr->second->getChild("ScreenStatus")->getValue();
            m_waitCount = 0; // we are doing something, wait before auto action
         }
         catch(...)
         {
            Log(LOG_DEV_DATA,"No message associated with this action\n");
         }
      }
      else
      {
         Log(LOG_DEV_DATA,"Not Found\n");
         retval.erase();
      }
   }
   catch(...)
   {
      Log(LOG_DEV_DATA,"Exception while finding - no action taken\n");
      retval.erase();
   }
   Log(LOG_FN_ENTRY,"SSS::DoAction - Exit {%s}\n", retval.c_str());

   return(retval);
}

// actually send out the controlling message.
void SystemScreenSaver::DoControl(std::string thisControl)
{
   Log(LOG_FN_ENTRY,"SSS::DoControl() - Enter {%s}\n",thisControl.c_str());
   if(!thisControl.empty())
   {
      for(XmlNodeMapItr controlitr = m_videoControl.begin(); controlitr != m_videoControl.end(); controlitr++)
      {
         try
         {
            if(controlitr->second->getChild("Active")->getValue() == "True")
            {
               if(controlitr->second->getChild("CommunicationType")->getValue() == "Serial")
               {
                  DoSerialControl(controlitr->second,thisControl);
               }
               if(controlitr->second->getChild("CommunicationType")->getValue() == "IPC")
               {
                  // to implement at a later date
               }
            }
         }
         catch(...)
         {
            // do nothing, but go on to the next one
         }
      }
   }
   else
   {
      Log(LOG_DEV_DATA,"SSS::DoControl received an empty Control\n");
   }
   Log(LOG_FN_ENTRY,"SSS::DoControl() - Exit\n");
}

// this calls the publish below
const std::string SystemScreenSaver::Publish(const std::string &tag, const std::string &value)
{
   XmlNode node(tag, value);
   return(Publish(&node));
}

const std::string SystemScreenSaver::Publish(const XmlNode *node)
{
   Log(LOG_FN_ENTRY, "SSS::Publish(%s) - Enter\n",
         node->ToString().c_str());

   if(!m_triggers.empty())
   {
      Log(LOG_DEV_DATA,"have a trigger list\n");
      XmlNodeMapItr trigitr = m_triggers.find(node->getName());
      if(trigitr != m_triggers.end())
      {
         Log(LOG_DEV_DATA,"found a node for this\n");
         // found a node with this name. need to find the associated value
         std::string locValue = std::string("Value") + node->getValue();
         const XmlNode *thisone = NULL;

         Log(LOG_DEV_DATA,"Looking for: %s\n",locValue.c_str());
         try
         {
            thisone = trigitr->second->getChild(locValue);
            Log(LOG_DEV_DATA,"Found it\n");
         }
         catch(...)
         {
            Log(LOG_DEV_DATA,"No Match\n");
            thisone = NULL;
         }
         // there isnt a matching value, try for default
         if(NULL==thisone)
         {
            try
            {
               thisone = trigitr->second->getChild("Default");
            }
            catch(...)
            {
               thisone = NULL;
            }
         }
         if(NULL != thisone)
         {
            // we found an action, either explicit or default
            Log(LOG_DEV_DATA,"going to do something!\n");
            DoControl(DoAction(thisone->getValue()));
         }
         else
         {
            Log(LOG_ERRORS, 
                  "Got publish node with an unexpected value, no default: %s\n",
                  node->ToString().c_str());
         }
      }
      else
      {
         Log(LOG_DEV_DATA,"Received publish of undesired data\n");
      }
   }
   else
   {
      Log(LOG_DEV_DATA,"No m_triggers map\n");
   }
   Log(LOG_FN_ENTRY, "SSS::Publish() - Exiting through BepServer\n");
   return(BepServer::Publish(node));
}

void SystemScreenSaver::TimingFunction(void)
{
   if(m_currentMode == "Normal")
   {
      m_modeAccumulator++;
   }
   else if(m_currentMode == "Inverted")
   {
      m_modeAccumulator--;
   }
   // clamp values
   if(m_modeAccumulator > ( 20 * m_okDelta)) m_modeAccumulator =  20 * m_okDelta;
   if(m_modeAccumulator < (-20 * m_okDelta)) m_modeAccumulator = -20 * m_okDelta;

   // check to see if it is ok to switch the screen !
   // this counter will reach zero if all conditions are correct
   INT64 Lcounter = 4;
   try
   {
      if(ReadSubscribeData("RollsDownElevUp") == "1") Lcounter--;
      if(ReadSubscribeData("RollsUpElevDown") == "0") Lcounter--;
      if(ReadSubscribeData("TestInProgress") == "0") Lcounter--;
      if(ReadSubscribeData("CableConnect") == "0") Lcounter--;
   }
   catch(...)
   {
      Log(LOG_ERRORS,"Something is seriously wrong with my subscriptions\n");
   }

   if(0 == Lcounter)
   {
      if(m_waitCount >= m_switchDelay)
      {
         if(m_modeAccumulator >= m_okDelta)
         {
            DoControl(DoAction("InvertedScreen"));
            m_waitCount = 0;
         }
         else if(m_modeAccumulator <= -(m_okDelta))
         {
            DoControl(DoAction("NormalScreen"));
            m_waitCount = 0;
         }
      }
      else m_waitCount++;
   }
   else
   {
      m_waitCount = 0;
   }
}

const INT32 SystemScreenSaver::HandlePulse(const INT32 code, const INT32 value)
{
   INT32 retVal = BEP_STATUS_FAILURE;

   if((SYSTEM_PULSE + 1) == code)
   {
      switch(value)
      {
      case TIMER_PULSE:
         try
         {
            TimingFunction();
            retVal = BEP_STATUS_SUCCESS;
         }
         catch(...)
         {
         }
         break;
      default:
         break;
      }
   }
   if(BEP_STATUS_SUCCESS != retVal)
   {
      retVal = BepServer::HandlePulse(code, value);
   }
   return( retVal);
}

