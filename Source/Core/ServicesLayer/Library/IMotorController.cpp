//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/IMotorController.cpp 2     3/30/05 5:29p Cward $
//
// FILE DESCRIPTION:
//   Interface class used to control the motor controller
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Library/IMotorController.cpp $
// 
// 2     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//*************************************************************************

#include "IMotorController.h"

IMotorController::IMotorController(const bool debug /*= false*/) : IBepCommunication()
{
    IBepCommunication::Initialize(std::string(MOTOR_CONTROLLER_NAME), std::string("Client"), debug);
}

IMotorController::~IMotorController(void)
{
}

INT32 IMotorController::Read(const std::string &tag, const bool IsFinal /*= false*/)
{
    std::string response;
    const XmlNode node(tag, "");

    return(IBepCommunication::Read(&node, response, IsFinal));
}

INT32 IMotorController::Read(const XmlNode *node, const bool IsFinal /*= false*/)
{
    std::string response;

    return(IBepCommunication::Read(node, response, IsFinal));
}

INT32 IMotorController::Write(const std::string &tag, const std::string &value, const bool IsFinal /*= false*/)
{
    std::string response;
    const XmlNode node(tag, value);

    return(IBepCommunication::Write(&node, response, IsFinal));
}

INT32 IMotorController::Write(const XmlNode *node, const bool IsFinal /*= false*/)
{
    std::string response;

    return(IBepCommunication::Write(node, response, IsFinal));
}

const INT32 IMotorController::GetNext(std::string& tag, std::string& value)
{
    std::string message;

    return(IBepCommunication::GetNext(tag, value, message));
}

const INT32 IMotorController::GetNext(XmlNode &node)
{
    std::string message;

    return(IBepCommunication::GetNext(node, message));
}

