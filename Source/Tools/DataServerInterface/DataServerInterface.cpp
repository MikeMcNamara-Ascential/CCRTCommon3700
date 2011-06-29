#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ds.h>
#include <errno.h>
#include <iostream.h>
#include <string>
#include "Bepos.h"
#include "BepDefs.h"
#include "XmlParser.h"		// XML Parser definition
#include "XmlDefs.h"		// XML definitions
#include "XmlNode.h"		// XML Node definitions
#include "BepXmlUtils.h"	// XML Utilities

//-----------------------------------------------------------------------------
void ReadDataServerVariable(const string &variable);
void WriteDataServerVariable(const string &variable, const string &value);
void CreateDataServerVariable(const string &variable, const string &value);
void DeleteDataServerVariable(const string &variable);
void CheckVariableList(const string &listLocation);
string GetDataServerFailureReason(const int &code);
void RunFromMenu(void);
void DisplayMenu(void);

//-----------------------------------------------------------------------------
ds_t dsId;

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    char opt;
    char *arg1, *arg2;
    string variable("");
    string value("");

    // Register with the data server
    if((dsId = ds_register()) != -1)
    {
        if(argc > 1)
        {
            // Parse each command line argument
            while((opt = getopt(argc, argv, "MmR:r:W:w:C:c:D:d:X:x:")) != -1)
            {   // parse out the variable and value
                if((opt == 'm') || (opt == 'M'))
                {
                    RunFromMenu();
                }
                else
                {
                    arg1 = strtok(optarg, ",");
                    arg2 = strtok(NULL, ",");
                    variable = (arg1 != NULL) ? string(arg1) : "";
                    value = (arg2 != NULL) ? string(arg2) : "";

                    // Process the next option
                    switch(opt)
                    {
                    case 'R':  // Read the value of the specified variable from the data server
                    case 'r':
                        ReadDataServerVariable(variable);
                        break;

                    case 'W':   // Write the specified value to the given tag
                    case 'w':
                        WriteDataServerVariable(variable, value);
                        break;

                    case 'C':   // Create a new data server variable and set the value
                    case 'c':
                        CreateDataServerVariable(variable, value);
                        break;

                    case 'D':   // Remove a variable from the data server
                    case 'd':
                        DeleteDataServerVariable(variable);
                        break;

                    case 'X':   // Check the data server variables using the attached variable list
                    case 'x':
                        CheckVariableList(variable);
                        break;

                    case 'M':   // Display the menu and keep running
                    case 'm':
                        RunFromMenu();
                        break;

                    default:
                        cout << "Unsupported option: " << optarg << endl;
                        break;
                    }
                }
            }
        }
        else
        {
            RunFromMenu();
        }
        // All done, disconnect from the data server
        ds_deregister(dsId);
    }
    else
    {
        cout << "Could not connect to data server: " << strerror(errno) << endl;
    }
    return 0;
}

//-----------------------------------------------------------------------------
void ReadDataServerVariable(const string &variable)
{
    int dsStatus = -1;
    char data[DEFAULT_BUFFER_SIZE];
    // Clear the data buffer
    memset(data, 0, sizeof(data));
    dsStatus = ds_get(dsId, variable.c_str(), &data[0], (size_t)sizeof(data));
    if(dsStatus != -1)
    {   // Display the data that was read
        cout <<  variable << " = " << data << endl;
    }
    else
    {   // Could not read data, print error message
        cout << "Could not read " << variable << ": " << GetDataServerFailureReason(errno) << endl;
    }
}

//-----------------------------------------------------------------------------
void WriteDataServerVariable(const string &variable, const string &value)
{
    int dsStatus = -1;
    dsStatus = ds_set(dsId, variable.c_str(), value.c_str(), value.length());
    if(dsStatus != -1)
    {   // Display the result
        cout << "Set " << variable << " = " << value << endl;
    }
    else
    {
        cout << "Could not set " << variable << " to " << value << ": " << GetDataServerFailureReason(errno) << endl;
    }
}

//-----------------------------------------------------------------------------
void CreateDataServerVariable(const string &variable, const string &value)
{
    int dsStatus = -1;
    dsStatus = ds_create(dsId, variable.c_str(), DS_PERM, NULL);
    if(dsStatus != -1)
    {   // Successfully created the variable in the data server, set a value if provided
        if(!value.empty())
        {
            dsStatus = ds_set(dsId, variable.c_str(), value.c_str(), value.length());
            if(dsStatus != -1)
            {
                cout << "Created " << variable << " = " << value << endl;
            }
            else
            {
                cout << "Could not set " << variable << " = " << value << ": " << GetDataServerFailureReason(errno) << endl;
            }
        }
        else
        {
            cout << "Created " << variable << endl;
        }
    }
    else
    {
        cout << "Could not create " << variable << ": " << GetDataServerFailureReason(errno) << endl;
    }
}

//-----------------------------------------------------------------------------
void DeleteDataServerVariable(const string &variable)
{
    int dsStatus = 0;
    dsStatus = ds_clear(dsId, variable.c_str());
    if(dsStatus != -1)
    {
        cout << "Removed " << variable << endl;
    }
    else
    {
        cout << "Could not remove " << variable << ": " << GetDataServerFailureReason(errno) << endl;
    }
}

//-----------------------------------------------------------------------------
void CheckVariableList(const string &listLocation)
{
    int dsStatus = -1;
    char data[STRING_RESERVE_SIZE];
    XmlParser parser;
    XmlNode *varList = const_cast<XmlNode *>(parser.ReturnXMLDocument(string(getenv("USR_ROOT")) + listLocation));
    for(XmlNodeMapCItr iter = varList->getChild("Setup/TagList/PublishTags")->getChildren().begin();
       iter != varList->getChild("Setup/TagList/PublishTags")->getChildren().end(); 
       iter++)
    {
        memset(data, 0, sizeof(data));
        dsStatus = ds_get(dsId, iter->second->getName().c_str(), &data[0], (size_t)sizeof(data));
        if(dsStatus != -1)
        {
            cout << iter->second->getName() << " = " << data << endl;
        }
        else
        {
            cout << "Could not read " << iter->second->getName() << ": " << GetDataServerFailureReason(errno) << endl;
        }
    }
}

//-----------------------------------------------------------------------------
void RunFromMenu(void)
{
    string option;
    string value;
    string variable;
    bool done = false;
    do
    {   // Display the menu
        DisplayMenu();
        cout << "Enter option: ";
        cin >> option;
        switch(option[0])
        {
        case 'R':
        case 'r':
            cout << endl << "Enter variable name: ";
            cin >> variable;
            ReadDataServerVariable(variable);
            break;

        case 'W':
        case 'w':
            cout << endl << "Enter variable name: ";
            cin >> variable;
            cout << "Enter value: ";
            cin >> value;
            WriteDataServerVariable(variable, value);
            break;

        case 'C':
        case 'c':
            cout << endl << "Enter new variable name: ";
            cin >> variable;
            cout << "Enter value: ";
            cin >> value;
            CreateDataServerVariable(variable, value);
            break;

        case 'D':
        case 'd':
            cout << endl << "Enter variable to delete: ";
            cin >> variable;
            DeleteDataServerVariable(variable);
            break;

        case 'X':
        case 'x':
            cout << endl << "Enter config file: ";
            cin >> variable;
            CheckVariableList(variable);
            break;

        case 'Q':
        case 'q':
            cout << endl << "Exiting" << endl << endl;
            done = true;
            break;

        default:
            cout << endl << "Unknown option " << option << " - Exiting" << endl << endl;
            done = true;
        }
    } while(!done);
}

//-----------------------------------------------------------------------------
string GetDataServerFailureReason(const int &code)
{
    string desc;
    switch(code)
    {
    case EBADF:
        desc = "Could not find data server";
        break;

    case EMSGSIZE:
        desc = "Buffer not big enough for variable value";
        break;

    case ESRCH:
        desc = "Variable does not exist in the data server";
        break;

    case ENOMEM:
        desc = "Not enough memory to store data in the data server";
        break;

    case EEXIST:
        desc = "Variable already defined in data server";
        break;

    default:
        desc = strerror(errno);
        break;
    }
    return desc;
}

//-----------------------------------------------------------------------------
void DisplayMenu(void)
{
    cout << endl << endl;
    cout << "R - Read data server variable value" << endl << endl;
    cout << "W - Write value to data server variable" << endl << endl;
    cout << "C - Create new data server variable" << endl << endl;
    cout << "D - Delete variable from data server" << endl << endl;
    cout << "X - Check variable list" << endl << endl;
    cout << "Q - Quit" << endl << endl;
}
