//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
// Interface class to the QNX Data Server
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
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
#ifndef IQnxDataServer_h
#define IQnxDataServer_h
//-----------------------------------------------------------------------------
#include <ds.h>
#include "BepDefs.h"
#include "XmlNode.h"

//-----------------------------------------------------------------------------
class IQnxDataServer
{
public:
    /** Class conctructor */
    IQnxDataServer();
    /** Class conctructor */
    virtual ~IQnxDataServer();
    /**
     * Connect to the QNX Data Server.
     * 
     * @return The status of connecting to the QNX Data Server.
     */
    BEP_STATUS_TYPE ConnectToQnxDataServer(void);
    /**
     * Disconnect the process from the QNX data server.
     * 
     * @return Status of disconnecting from the QNX data server.
     */
    BEP_STATUS_TYPE DisconnectFromQnxDataServer(void);
    /**
     * Update the specified data server variable with the provided value.
     * If the data server variable does not exist, it will be created.
     * 
     * @param data   Variable name and value to be updated.
     * 
     * @return Status of updating the data server variable.
     */
    BEP_STATUS_TYPE UpdateQnxDataServerVariable(const XmlNode *data);
    /**
     * Update the specified data server variable with the provided value.
     * If the data server variable does not exist, it will be created.
     * 
     * @param name   Variable to be updated.
     * @param value  Value to store in the specified variable.
     * 
     * @return Status of updating the data server variable.
     */
    BEP_STATUS_TYPE UpdateQnxDataServerVariable(const string &name, const string &value);
    /**
     * Read the current value of the specified data server variable.
     * 
     * @param name   Name of the variable to read.
     * 
     * @return Value of data server variable or "Unavailable" if the variable does not exist.
     */
    string ReadQnxDataServerVariable(const string &name);

private:
    /**
     * Create a new data server variable with the specified name.
     * 
     * @param name   Name of the data server variable to create.
     * 
     * @return Status of creating the data server variable.
     */
    BEP_STATUS_TYPE CreateDataServerVariable(const string &name);
    /**
     * Check if the process is connected to the QNX data server.
     * 
     * @return Flag indicating if the process has connected to the QNX data server.
     */
    bool ConnectedToDataServer(void);
    /**
     * File descriptor to the QNX data server.
     */
    ds_t m_qnxDataServer;
};

//-----------------------------------------------------------------------------
#endif //IQnxDataServer_h
