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
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif

#include "IQnxDataServer.h"

//-----------------------------------------------------------------------------
IQnxDataServer::IQnxDataServer() : m_qnxDataServer(-1)
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
IQnxDataServer::~IQnxDataServer()
{   // Make sure to disconnect from the data server
    if(ConnectedToDataServer())  DisconnectFromQnxDataServer();
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IQnxDataServer::ConnectToQnxDataServer(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Make sure we only connect once
    if(!ConnectedToDataServer())
    {   // Connect to the data server
        m_qnxDataServer = ds_register();
        status = (-1 != m_qnxDataServer) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
    }
    else
    {   // Already connected
        status = BEP_STATUS_SUCCESS;
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IQnxDataServer::DisconnectFromQnxDataServer(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Only disconnect if we have successfully connected
    if(ConnectedToDataServer())
    {
        status = (ds_deregister(m_qnxDataServer) == 0) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
    }
    else
    {   // Not connected, so return success
        status = BEP_STATUS_SUCCESS;
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IQnxDataServer::UpdateQnxDataServerVariable(const XmlNode *data)
{
    return UpdateQnxDataServerVariable(data->getName(), data->getValue());
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IQnxDataServer::UpdateQnxDataServerVariable(const string &name, const string &value)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Make sure we are connected to the data server
    if(!ConnectedToDataServer())  ConnectToQnxDataServer();
    int attempts = 2;
    // Update the data server variable
    do
    {
        switch(ds_set(m_qnxDataServer, name.c_str(), value.c_str(), value.length()))
        {
        case 0:    // Updated the data server variable
            status = BEP_STATUS_SUCCESS;
            break;

        case -1:   // Failed to update the variable, handle the problem
            switch(errno)
            {
            case EBADF:      // No connected to the data server, attmempt to connect
                status = (BEP_STATUS_SUCCESS == ConnectToQnxDataServer()) ? BEP_STATUS_FAILURE : BEP_STATUS_ABORT;
                break;

            case ENOMEM:     // Not enough memory to store the data
                status = BEP_STATUS_ABORT;
                break;

            case ESRCH:      // Variable does not exist, create and try again
                status = (BEP_STATUS_SUCCESS == CreateDataServerVariable(name)) ? BEP_STATUS_FAILURE : BEP_STATUS_ABORT;
                break;

            default:
                status = BEP_STATUS_SOFTWARE;
                break;
            }
            break;

        default:
            status = BEP_STATUS_SOFTWARE;
            break;
        }
    } while(attempts-- && (BEP_STATUS_SUCCESS != status) && (BEP_STATUS_ABORT != status));
    return status;
}

//-----------------------------------------------------------------------------
string IQnxDataServer::ReadQnxDataServerVariable(const string &name)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string value(BEP_ERROR_RESPONSE);
    int attempts = 2;
    int bufferSize = 256;
    // Make sure we are connected to the data server
    if(!ConnectedToDataServer())  ConnectToQnxDataServer();
    do
    {   // Attempt to read the variable
        char buff[bufferSize];
        switch(ds_get(m_qnxDataServer, name.c_str(), buff, bufferSize))
        {
        case -1:   // Could not read the variable from the data server
            switch(errno)
            {
            case EBADF:      // Not connected to the data server, connect and try again
                status = (BEP_STATUS_SUCCESS == ConnectToQnxDataServer()) ? BEP_STATUS_FAILURE : BEP_STATUS_ABORT;
                break;

            case EMSGSIZE:   // Data buffer not large enough, increase and try again
                bufferSize *= 2;
                status = BEP_STATUS_FAILURE;
                break;

            case ESRCH:      // Variable does not exist, bail out
                value = BEP_UNAVAILABLE_RESPONSE;
                status = BEP_STATUS_ABORT;
                break;

            default:
                value = BEP_UNAVAILABLE_RESPONSE;
                status = BEP_STATUS_ABORT;
            }
            break;

        default:   // Successfully retrieved the variable value
            value.assign(buff);
            status = BEP_STATUS_SUCCESS;
            break;
        }
    } while(attempts-- && (BEP_STATUS_SUCCESS != status) && (BEP_STATUS_ABORT != status));
    return value;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IQnxDataServer::CreateDataServerVariable(const string &name)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    int attempts = 2;
    // Make sure we are connected to the data server
    if(!ConnectedToDataServer())  ConnectToQnxDataServer();
    do
    {   // Create the variable
        switch(ds_create(m_qnxDataServer, name.c_str(), DS_PERM, NULL))
        {
        case -1:   // Failed to create the variable
            switch(errno)
            {
            case EBADF:      // Not connected to the data server, reconnect and try again
                status = (BEP_STATUS_SUCCESS == ConnectToQnxDataServer()) ? BEP_STATUS_FAILURE : BEP_STATUS_ABORT;
                break;

            case EEXIST:     // The variable already exists in the data server.
                status = BEP_STATUS_SUCCESS;
                break;

            case ENOMEM:     // Not enough memory to add the variable
                status = BEP_STATUS_ABORT;
                break;

            default:
                status = BEP_STATUS_ERROR;
                break;
            }
            break;

        default:   // Successfully created the variable
            status = BEP_STATUS_SUCCESS;
            break;
        }
    } while(attempts-- && (BEP_STATUS_SUCCESS != status) && (BEP_STATUS_ABORT != status));
    return status;
}

//-----------------------------------------------------------------------------
bool IQnxDataServer::ConnectedToDataServer(void)
{
    return m_qnxDataServer != -1;
}
