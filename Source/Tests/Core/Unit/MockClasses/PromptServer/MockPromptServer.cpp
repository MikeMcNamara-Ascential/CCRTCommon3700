//******************************************************************************
// Description:
//  A mock prompt server class that is used in unit testing.
//
//==============================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/PromptServer/MockPromptServer.cpp $
// 
// 1     1/18/07 4:49p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "MockPromptServer.h"

//==============================================================================
MockPromptServer::MockPromptServer() : 
    m_displayPromptCalled(0), m_removePromptCalled(0),m_clearPromptBoxCalled(0),
    m_clearPromptBoxShouldThrow(false),m_setBackgroundColorCalled(0),
    m_setBackgroundColorResponse("Pass"),
    m_setBackgroundColorReturnValue(BEP_STATUS_SUCCESS)
{

}

//==============================================================================
MockPromptServer::~MockPromptServer()
{

}

//==============================================================================
INT32 MockPromptServer::DisplayPrompt(const INT32 &promptBox,
                                      const std::string &prompt,
                                      const PromptDetails &promptDetails,
                                      std::string &response,
                                      INT32 priorityLevel/*=MIN_PRIORITY*/)
{
    // store the passed in parameters
    m_displayPromptParamPromptBox.push_back(promptBox);
    m_displayPromptParamPrompt.push_back(prompt);
    m_displayPromptParamPromptDetails.push_back(promptDetails);
    m_displayPromptParamPriorityLevel.push_back(priorityLevel);

    // return the specified response
    response = m_displayPromptResponse[m_displayPromptCalled];

    // increment the method called counter and return the specified value
    return(m_displayPromptReturnValue[m_displayPromptCalled++]);
}

//==============================================================================
INT32 MockPromptServer::DisplayPrompt(const INT32 &promptBox,
                                      const std::string &prompt,
                                      std::string &response,
                                      INT32 priorityLevel/*=MIN_PRIORITY*/)
{
    // store the passed in parameters
    m_displayPromptParamPromptBox.push_back(promptBox);
    m_displayPromptParamPrompt.push_back(prompt);
    m_displayPromptParamPriorityLevel.push_back(priorityLevel);

    // return the specified response
    response = m_displayPromptResponse[m_displayPromptCalled];

    // increment the method called counter and return the specified value
    return(m_displayPromptReturnValue[m_displayPromptCalled++]);
}

//==============================================================================
INT32 MockPromptServer::RemovePrompt(const INT32 &promptBox,
                                     const std::string &prompt,
                                     std::string &response,
                                     INT32 priorityLevel/*=MIN_PRIORITY*/)
{
    // store the passed in parameters
    m_removePromptParamPromptBox.push_back(promptBox);
    m_removePromptParamPrompt.push_back(prompt);
    m_removePromptParamPriorityLevel.push_back(priorityLevel);

    // return the specified response
    response = m_removePromptResponse[m_removePromptCalled];

    // increment the method called counter and return the specified value
    return(m_removePromptReturnValue[m_removePromptCalled++]);
}

//==============================================================================
INT32 MockPromptServer::ClearPromptBox(const INT32 &promptBox, 
                                       std::string &response)
{
    // store the passed in parameters
    m_clearPromptBoxParamPromptBox.push_back(promptBox);

    // if an exception should be thrown
    if(m_clearPromptBoxShouldThrow == true)
    {
        throw BepException("ClearPromptBox exception");
    }
     
    // return the specified response
    response = m_clearPromptBoxResponse[m_clearPromptBoxCalled];

    // increment the method called counter and return the specified value
    return(m_clearPromptBoxReturnValue[m_clearPromptBoxCalled++]);
}

//==============================================================================
INT32 MockPromptServer::SetBackgroundColor(const INT32 &promptBox, 
                                           const std::string &color,
                                           std::string &response)
{
    // increment the method called counter
    m_setBackgroundColorCalled++;

    // store the passed in parameters
    m_setBackgroundColorParamPromptBox = promptBox;
    m_setBackgroundColorParamColor = color;

    // return the specified response
    response = m_setBackgroundColorResponse;

    // return the specified value
    return(m_setBackgroundColorReturnValue);
}
