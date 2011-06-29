//******************************************************************************
// Description:
//  The mock system monitor class.  This is used in unit testing since 
//  CheckTesting() is pure virtual in the base class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/SystemMonitor/MockSystemMonitor.cpp $
// 
// 1     1/18/07 4:50p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <iostream>
#include "MockSystemMonitor.h"

using namespace std;

//=============================================================================
MockSystemMonitor::MockSystemMonitor() :
    m_checkTestingCalled(0)
{
}

//=============================================================================
MockSystemMonitor::~MockSystemMonitor()
{

}

//=============================================================================
void MockSystemMonitor::CheckTesting(ControlData *ctrl)
{
    // store the parameter passed in
    m_checkTestingParamCtrl = ctrl;

    // increment the method called counter
    m_checkTestingCalled++;
}
