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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/PromptServer/MockPromptServer.h $
// 
// 1     1/18/07 4:49p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MockPromptServer_h
#define MockPromptServer_h

#include "IPromptServer.h"

class MockPromptServer : public IPromptServer
{
public:
    MockPromptServer();
    ~MockPromptServer();

    INT32 DisplayPrompt(const INT32 &promptBox,const std::string &prompt,
                        const PromptDetails &promptDetails,
                        std::string &response,
                        INT32 priorityLevel= MIN_PRIORITY);

    INT32 DisplayPrompt(const INT32 &promptBox,
                        const std::string &prompt,
                        std::string &response,
                        INT32 priorityLevel = MIN_PRIORITY);


    INT32 RemovePrompt(const INT32 &promptBox,
                       const std::string &prompt,
                       std::string &response,
                       INT32 priorityLevel = MIN_PRIORITY);

    INT32 ClearPromptBox(const INT32 &promptBox, std::string &response);

    INT32 SetBackgroundColor(const INT32 &promptBox, const std::string &color,
                             std::string &response);

    /** member variable to tell how many times DisplayPrompt() is called */
    int m_displayPromptCalled;

    /** vectors to store the variables passed to DisplayPrompt() */
    vector<INT32> m_displayPromptParamPromptBox;
    vector<std::string> m_displayPromptParamPrompt;
    vector<PromptDetails> m_displayPromptParamPromptDetails;
    vector<INT32> m_displayPromptParamPriorityLevel;

    /**
     * Vector of responses for DisplayPrompt().  These need to be set in the 
     * unit tests.
     */
    vector<std::string> m_displayPromptResponse;

    /**
     * Vector of return values for DisplayPrompt().  These need to be set in the 
     * unit tests.
     */ 
    vector<INT32> m_displayPromptReturnValue;

    /** member variable to tell how many times RemovePrompt() is called */
    int m_removePromptCalled;

    /** vectors to store the variables passed to RemovePrompt() */
    vector<INT32> m_removePromptParamPromptBox;
    vector<std::string> m_removePromptParamPrompt;
    vector<INT32> m_removePromptParamPriorityLevel;

    /**
     * Vector of responses for RemovePrompt().  These need to be set in the 
     * unit tests.
     */
    vector<std::string> m_removePromptResponse;

    /**
     * Vector of return values for RemovePrompt().  These need to be set in the 
     * unit tests.
     */ 
    vector<INT32> m_removePromptReturnValue;

    /** member variable to tell how many times ClearPromptBox() is called */
    int m_clearPromptBoxCalled;

    /** vector to store the variables passed to ClearPromptBox() */
    vector<INT32> m_clearPromptBoxParamPromptBox;

    /** flag to tell if ClearPromptBox() should throw an exeption */
    bool m_clearPromptBoxShouldThrow;

    /**
     * Vector of responses for ClearPromptBox().  These need to be set in the 
     * unit tests.
     */
    vector<std::string> m_clearPromptBoxResponse;

    /**
     * Vector of return values for ClearPromptBox().  These need to be set in 
     * the unit tests.
     */ 
    vector<INT32> m_clearPromptBoxReturnValue;

    /** member variable to tell how many times SetBackgroundColor() is called */
    int m_setBackgroundColorCalled;

    /** 
     * member variables to store the variables passed to SetBackgroundColor() 
     */
    INT32 m_setBackgroundColorParamPromptBox;
    std::string m_setBackgroundColorParamColor;

    /** 
     * Response to SetBackgroundColor().  This should be set in the unit tests.
     */ 
    std::string m_setBackgroundColorResponse;

    /** 
     * Return value of SetBackgroundColor().  This should be set in the unit 
     * tests.
     */
    INT32 m_setBackgroundColorReturnValue;
};

#endif
