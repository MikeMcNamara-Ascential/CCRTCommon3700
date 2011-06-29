#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/dispatch.h>
#include <list>
#include <utility>

#include "Bepos.h"			// BEP OS Wrapper functions
#include "BepDefs.h"		// BEP Standard Definitions
#include "tchar.h"			// XML definitions
#include "XmlParser.h"		// XML Parser definition
#include "XmlDefs.h"		// XML definitions
#include "XmlNode.h"		// XML Node definitions
#include "BepXmlUtils.h"	// XML Utilities
#include "IInterProcessCommunication.h"
#include "INamedDataBroker.h"

typedef std::pair<string, string>	TagValue_t;

typedef std::list< TagValue_t>		TagValueList_t;
typedef TagValueList_t::iterator	TagValueListItr_t;

/**
 * Send a pulse message to a given process
 *
 * @param procId     Channel ID to which the pulse will be sent
 * @param pulseCode  Pulse code to be sent to the process
 * @param pulseValue Pulse value to be sent to the process
 */
void SendPulse( int procId, int pulseCode, int pulseValue);

/**
 * Send an XML message to a given process
 *
 * @param srvrName Name of the server to send the message to
 * @param xmlMssgType
 *                 MssgType attribute value
 * @param xmlTag   XML node tag to send to the process
 * @param xmlVal   XML node value to send to the process
 */
void SendXml( const string &srvrName, const XML_Char *xmlMssgType, const XML_Char *xmlTag, const XML_Char *xmlVal);

/**
 * Read/write via named data broker
 *
 * @param tagList list of (tag,value) pairs to read/write via the NamedDataBroker
 */
void SendNdb( TagValueList_t &tagList);

/**
 * Read/write via named data broker
 *
 * @param tagList list of (tag,value) pairs to read/write via the NamedDataBroker
 * @param type   XML message type to send
 */
void SendNdb( TagValueList_t &tagList, const string &type);

bool	debug=false;

long	loopPeriod=0;

typedef enum
{
	SEND_NO_MSG = 0x00,
	SEND_PULSE_MSG = 0x01,
	SEND_XML_MSG = 0x02,
	SEND_NDB_MSG = 0x04
} MsgSendType_t;

int DebugOut( const char *fmt, ...)
{
	int retVal = 0;
	va_list	args;

	if( debug)
	{
		// format the input into a String
		va_start( args, fmt);

		// Limit the size of the message so that it fits into the buffer (else SIGSEGV)
		retVal = vprintf( fmt, args);

		// remove left overs
		va_end( args);
	}

	return( retVal);
}

int main( int argc, char **argv)
{
	char			opt;
	int				msgType=SEND_NO_MSG;
	int				pulseCode, pulseVal;
	int				procId=-1;
	XmlString		xmlTag, xmlVal(""), xmlMssgType( XML_T(BEP_COMMAND));
	string			temp;
	char			*xmlArg;
	string			srvrName;
	string			ndbTag, ndbVal("");
	bool			typeSpecified = false;
	TagValueList_t	tagList;
	TagValue_t		tagVal;

	// Don't report error for unknown option
	opterr = 0;

	// Parse command line arguments
	while( (opt=getopt( argc, argv, "b:dl:n:p:x:t:")) != -1)
	{
		switch( opt)
		{
		case 'b':
			DebugOut( "NDB Tag = %s\n", optarg);

			tagVal.second.clear();
			if( (xmlArg=strchr( optarg, ',')) != NULL)
			{
				tagVal.second = string( &xmlArg[ 1]);
				xmlArg[ 0] = 0;
			}
			tagVal.first = string( optarg);

			tagList.push_back( tagVal);
			DebugOut( "NDB Tag = %s, value=%s\n", tagVal.first.c_str(), tagVal.second.c_str());
			msgType |= SEND_NDB_MSG;
			break;
		case 'd':
			DebugOut( "Debug = true\n");
			debug = true;
			break;
		case 'l':
			DebugOut( "Loop milliseconds=%s\n", optarg);
			loopPeriod = atol( optarg);
			break;
		case 'n':	case 'N':		// Process name to send message to
			DebugOut( "Target process=%s\n", optarg);
			srvrName = optarg;
			procId = name_open( srvrName.c_str(), 0);
			break;
		case 'p':	case 'P':		// Send a pulse
			if( sscanf( optarg, "%d,%d", &pulseCode, &pulseVal) != 2)
			{
				pulseVal = 0;
			}
			DebugOut( "Pulse=%d, value=%d\n", pulseCode, pulseVal);
			msgType |= SEND_PULSE_MSG;
			break;
		case 'x':	case 'X':		// Send an xml message
			DebugOut( "XML Tag = %s\n", optarg);
			if( (xmlArg=strchr( optarg, ',')) != NULL)
			{
				temp = string( &xmlArg[ 1]);
				xmlVal = AsciiToXml( temp);
				xmlArg[ 0] = 0;
			}
			temp = string( optarg);
			xmlTag = AsciiToXml( temp);
			DebugOut( "XML Tag = %s, value=%s\n", xmlTag.c_str(), xmlVal.c_str());
			msgType |= SEND_XML_MSG;
			break;
		case 't':	case 'T':		// xml message MssgType
			DebugOut( "XML Type = %s\n", optarg);
			temp = string( optarg);
			xmlMssgType = AsciiToXml( temp);
			typeSpecified = true;
			break;
		default:
			break;
		}
	}

	if( msgType & SEND_NDB_MSG)
	{
		string mssgTypeString("");
		if( typeSpecified)
		{
			mssgTypeString = XmlToAscii( xmlMssgType);
		}

		SendNdb( tagList, mssgTypeString);
	}
	if( msgType & SEND_XML_MSG)
	{
		SendXml( srvrName, xmlMssgType.c_str(), xmlTag.c_str(), xmlVal.c_str());
	}
	if( msgType & SEND_PULSE_MSG)
	{
		SendPulse( procId, pulseCode, pulseVal);
	}
	return( 0);
}


/**
 * Send a pulse message to a given process
 *
 * @param procId     Channel ID to which the pulse will be sent
 * @param pulseCode  Pulse code to be sent to the process
 * @param pulseValue Pulse value to be sent to the process
 */
void SendPulse( int procId, int pulseCode, int pulseValue)
{
	int	prio = sched_get_priority_max(sched_getscheduler( 0));

	DebugOut( "Sending pulse %d,%d to %d\n", pulseCode, pulseValue, procId);
	MsgSendPulse( procId, prio, pulseCode, pulseValue);
}

/**
 * Send an XML message to a given process
 *
 * @param srvrName Name of the server to send the message to
 * @param xmlMssgType
 *                 MssgType attribute value
 * @param xmlTag   XML node tag to send to the process
 * @param xmlVal   XML node value to send to the process
 */
void SendXml( const string &srvrName, const XML_Char *xmlMssgType, const XML_Char *xmlTag, const XML_Char *xmlVal)
{
	XmlElement					root( XML_T(BEP_XML_ROOT), XmlString(XML_T("")));
	XmlElement					*cmdNode;
	IInterProcessCommunication	ipcComm;
	string						ipcType( "Client");

	ipcComm.Initialize( srvrName, ipcType, debug, DEFAULT_BUFFER_SIZE, 60000);

	DebugOut( "Type=%s, Tag=%s, Val=%s\n", xmlMssgType, xmlTag, xmlVal);
	// Add msg type identifier attribute
	root.addChild( XML_T(BEP_XML_MSG_TYPE), xmlMssgType, ATTRIBUTE_NODE);

	// If tag is given
	if( xmlTag != NULL)
	{
		cmdNode = (XmlElement*)root.addChild( xmlTag, xmlVal, ELEMENT_NODE);
	}

	// Get the buffer as a string
	XmlString	cmdStr;
	root.writeNode( cmdStr);

	string ascii;
	DebugOut( "Sending %d XML Bytes: %s\n", cmdStr.length(), cmdStr.c_str());

	string response;
	do
	{
		ascii = XmlToAscii( cmdStr);
		ipcComm.Write( ascii);
		ipcComm.Read(response);
		DebugOut( "Received: %s\n", response.c_str());
		delay( loopPeriod);
	} while( loopPeriod > 0);

}

/**
 * Read/write via named data broker
 *
 * @param tagList list of (tag,value) pairs to read/write via the NamedDataBroker
 */
void SendNdb( TagValueList_t &tagList)
{
	INamedDataBroker	ndb( debug);
	TagValueListItr_t	itr;
	string				response, ndbVal;
	uint16_t			fieldWidth=0;
	bool				showTagName = tagList.size() > 1 ? true : false;

	// Determine the field width first
	for( itr=tagList.begin(); itr!=tagList.end(); itr++)
	{
		if( itr->first.size() > fieldWidth)	fieldWidth = itr->first.size();
	}
	fieldWidth++;

	do
	{
		// Loop through the list of tags
		for( itr=tagList.begin(); itr!=tagList.end(); itr++)
		{
			string tag(itr->first), val(itr->second);

			// If a value was given on the cmd line for this tag, write it
		if( val.empty() == false)
		{
			if( ndb.Write( tag, val, response, true) != -1)
			{
					printf( "%-*.*s= %s\n", fieldWidth, fieldWidth, tag.c_str(), val.c_str());
				}
			}
			// No value given on the cmd line means read it
		else
		{
			if( ndb.Read( tag, response, true) != -1)
			{
					ndb.GetByTag( tag, val, response);
					if( showTagName)	printf( "%-*.*s= ", fieldWidth, fieldWidth, tag.c_str());
					printf( "%s\n", val.c_str());
				DebugOut( "Extended Reply: %s\n",  response.c_str());
			}
		}
		}
		delay( loopPeriod);

		// If more than one tag specified, seperate the printed blocks with a blank line
		if(tagList.size() > 1)	printf( "\n");
	} while( loopPeriod > 0);
}

/**
 * Read/write via named data broker
 *
 * @param tagList list of (tag,value) pairs to read/write via the NamedDataBroker
 * @param type   XML message type to send
 */
void SendNdb( TagValueList_t &tagList, const string &type)
{
	INamedDataBroker	ndb(debug);
	TagValueListItr_t	itr;
	string				response, ndbVal;

	if( type == "Command")
	{
		do
		{
			for( itr=tagList.begin(); itr!=tagList.end(); itr++)
			{
				string &tag=itr->first, &val=itr->second;
			if( ndb.Command( tag, val, response) != -1)
			{
				printf( "Command reply: %s\n", response.c_str());
			}
			}
			delay( loopPeriod);

		} while( loopPeriod > 0);
	}
	else
	{
		SendNdb( tagList);
	}
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
*******************************************************************************/
#ifdef __USAGE

%C ([-n proc][-btag,val]) [-p code[,value]] [ -t type -x tag[,val]] [-ln]
Sends a command to the process name given on the command line

    -n proc  Process name (name_attach) to send command to
    -l n     Loop at 'n' millisecond intervals
    -p	     Sends a pulse
      code   Pulse code to send to the given process
      value  Pulse value to send to the given process
    -x       Sends an XML message
      tag    XML command tag to send to the given process
      val    XML Node Value to associate with the given XML tag
    -t type  Set MssgType attribute value to type
    -b       Use Named data broker to read/write
      tag    Tag to read/write
      val    For writes, value to write
    -d       Enable debug mode
#endif


