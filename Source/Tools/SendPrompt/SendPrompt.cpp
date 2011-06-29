#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/dispatch.h>

#include "Bepos.h"			// BEP OS Wrapper functions
#include "BepDefs.h"		// BEP Standard Definitions
#include "tchar.h"			// XML definitions	
#include "XmlParser.h"		// XML Parser definition
#include "XmlDefs.h"		// XML definitions	
#include "XmlNode.h"		// XML Node definitions
#include "BepXmlUtils.h"	// XML Utilities
#include "IInterProcessCommunication.h"
#include "IPromptServer.h"

/**
 * Send a pulse message to a given process
 *
 * @param procId     Channel ID to which the pulse will be sent
 * @param pulseCode  Pulse code to be sent to the process
 * @param pulseValue Pulse value to be sent to the process
 */
void SendPulse( int procId, int pulseCode, int pulseValue);

/**
 * Read/write via PromptServer
 * 
 * @param tag    Data tag to read/write
 * @param val    Value to write
 */
void SendPromptServer( const string &promptBox, const string &prompt, 
					   const string &priority, const INT32 &operation);

bool	debug=false;

long	loopPeriod=0;

typedef enum
{
	SEND_NO_MSG = 0x00,
	SEND_PULSE_MSG = 0x01,
	SEND_XML_MSG = 0x02,
	SEND_PS_DSP = 0x04,
	SEND_PS_RMV = 0x08
} MsgSendType_t;

const INT32 removePrompt = 0;
const INT32 displayPrompt = 1;

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
	string			psTag, psVal(""), psPriority("0");

	// Don't report error for unknown option
	opterr = 0;

	// Parse command line arguments
	while( (opt=getopt( argc, argv, "b:d:l:p:r:")) != -1)
	{
		switch( opt)
		{
		case 'b':
			DebugOut( "PromptServer Tag = %s\n", optarg);
			if( (xmlArg=strchr( optarg, ',')) != NULL)
			{
				psVal = string( &xmlArg[ 1]);
				xmlArg[ 0] = 0;
			}
			psTag = string( optarg);
			DebugOut( "PromptServer Display Box=%s, Prompt=%s\n", 
					  psTag.c_str(), psVal.c_str());
			msgType |= SEND_PS_DSP;
			break;
		case 'r':
			DebugOut( "PromptServer Tag = %s\n", optarg);
			if( (xmlArg=strchr( optarg, ',')) != NULL)
			{
				psVal = string( &xmlArg[ 1]);
				xmlArg[ 0] = 0;
			}
			psTag = string( optarg);
			DebugOut( "PromptServer Remove Box=%s, Prompt=%s\n", 
					  psTag.c_str(), psVal.c_str());
			msgType |= SEND_PS_RMV;
			break;
		case 'd':
			DebugOut( "Debug = true\n");
			debug = true;
			break;
		case 'l':
			DebugOut( "Priority Level=%s\n", optarg);
			psPriority = string( optarg);
			break;
		case 'p':	case 'P':		// Send a pulse
			if( sscanf( optarg, "%d,%d", &pulseCode, &pulseVal) != 2)
			{
				pulseVal = 0;
			}
			DebugOut( "Pulse=%d, value=%d\n", pulseCode, pulseVal);
			msgType |= SEND_PULSE_MSG;
			break;
		default:
			break;
		}
	}

	if( msgType & SEND_PS_DSP)
	{
		SendPromptServer( psTag, psVal, psPriority, displayPrompt);
	}
	if( msgType & SEND_PS_RMV)
	{
		SendPromptServer( psTag, psVal, psPriority, removePrompt);
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
 * Read/write via PromptServer
 * 
 * @param tag    Data tag to read/write
 * @param val    Value to write
 */
void SendPromptServer( const string &tag, const string &prompt, const string &priority, const INT32 &operation)
{
	IPromptServer       promptServer;
	string				response, psVal;
 
	promptServer.Initialize("core/PromptServer", DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT, debug);

	do
	{
		if( prompt.empty() == false)
		{
			std::string promptBoxTag(PROMPT_BOX);
			INT32 promptBox = atoi((tag.substr(promptBoxTag.length(), 1)).c_str());
			INT32 pri = atoi(priority.c_str());
			if (operation == displayPrompt)
			{
				if (promptServer.DisplayPrompt(promptBox, prompt, response, pri) != -1)
				{
					printf( "Write reply: %s\n", response.c_str());
				}
			}
			else if (operation == removePrompt)
			{
				if (promptServer.RemovePrompt(promptBox, prompt, response, pri) != -1)
				{
					printf( "Write reply: %s\n", response.c_str());
				}
			}
			else printf("Unknown operation type - %d (DISPLAY:%d, REMOVE:%d)\n", 
						operation, displayPrompt, removePrompt);
		}
		else
		{
			if( promptServer.Read( tag, response, true) != -1)
			{
				promptServer.GetByTag(tag, psVal, response); 
				printf( "%s\n", psVal.c_str());
				DebugOut( "Extended Reply: %s\n",  response.c_str());
			}
		}
		delay( loopPeriod);

	} while( loopPeriod > 0);
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
*******************************************************************************/
#ifdef __USAGE

%C ([-btag,prompt,priority]) ([-rtag,prompt,priority]) [-p code[,value]] [-ln]
Requests the PromptServer to display a Prompt

    -l n     Specifies priority level of prompt
    -p	     Sends a pulse
      code   Pulse code to send to the given process
      value  Pulse value to send to the given process
    -b       Use PromptServer to display a prompt
      tag       Tag to read/write
      prompt    Prompt to display
      priority  Priority of the prompt
	-r       Use PromptServer to remove a prompt from display
      tag       Tag to read/write
      prompt    Prompt to remove
      priority  Priority of the prompt
    -d       Enable debug mode
#endif


