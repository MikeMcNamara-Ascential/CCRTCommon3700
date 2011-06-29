//******************************************************************************
// Description:
//	This file implements the Task Monitor.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonOld/TaskMonOld.cpp $
// 
// 1     7/19/06 6:05p Cward
// Added old taskmon so that it can be built for the ghost image. The
// ghost image now has support for old and new task Monitors, with the
// ability to easily switch on reboot
//
// 2     6/24/04 1:17a Cward
// Removed pound if 0 blocks. Added new standard header block comments.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "TaskMonOld.h"

volatile bool	timeToStop = false;
string			nodeName;
uint32_t		loopPeriod = 100;
int				priority;
int 			rows, cols;
int				coId, chId;
bool			useColor = false, keepDead=false;
uint32_t		deadThresh;
uint32_t		aliveThresh;
volatile bool	winResize=false;
char			*ver = "Ver 1.10";

int main( int argc, char *argv[])
{
	ProcList_t		localCats;
	CatProcs_t		procNames;
	struct utsname	localNode;
	struct sigevent	sigev;
	struct _itimer	timerTime;
	timer_t			timerId;
	struct _pulse	pulse;
	sigset_t		origSigSet, winchSigSet, pendingSet;

	signal( SIGTERM, SignalHandler);
	signal( SIGINT, SignalHandler);
	signal( SIGQUIT, SignalHandler);

	// Get original signal mask
    sigprocmask( 0, NULL, &origSigSet);

	// Create a signal set with SIGQUIT blocked to use while logging
	sigemptyset( &winchSigSet);
    sigaddset( &winchSigSet, SIGWINCH);
	// block SIGWINCH...we will check for it manually
	sigprocmask( SIG_BLOCK, &winchSigSet, NULL);

	// use local node name as the default node to read from
	uname( &localNode);
	nodeName = string( localNode.nodename);

	// Read current priority
	priority = getprio( 0);

	// Parse command line
	ParseCommandLine(argc,argv);

	// Create a connection to receive timer pulses on
	chId = ChannelCreate( 0);
	coId = ConnectAttach( 0, 0, chId, _NTO_SIDE_CHANNEL, 0);
	SIGEV_PULSE_INIT( &sigev, coId, priority, 0, 0);

	// Calculate the number of loops needed for 2 seconds
	deadThresh = 2000 / loopPeriod;
	if( deadThresh == 0)	deadThresh = 10;
	aliveThresh = 2000 / loopPeriod;
	if( aliveThresh == 0)	aliveThresh = 10;

	// Initialize curses library
	CursesInit( false);

	// Setup a timer
	timerId = TimerCreate( CLOCK_REALTIME, &sigev);
	timerTime.nsec = 10LL;
	timerTime.interval_nsec = loopPeriod * 1000000LL;
	TimerSettime( timerId, 0, &timerTime, NULL);

	// What is the root directory where the names are listed
	const string	dirPath = string( "/net/") + nodeName + string("/dev/name/local");

	while( timeToStop == false)
	{
		if( MsgReceivePulse( chId, &pulse, sizeof( pulse), NULL)  == -1)
		{
			continue;
		}

		int key = CheckKeyPress();
		switch( key)
		{
		case 'k':	case 'K':
			keepDead = (keepDead== false);
			continue;
		case 'q':	case 'Q':
			timeToStop = true;
			continue;
		case 'r':	case 'R':
			localCats.clear();
			procNames.clear();
			break;

		}

		// Find out what directories exist under '/dev/name/local'
		//	and put those names into the localCats list
		ReadProcessNames( localCats, dirPath.c_str());

		// Now fill in the process name list
		GetProcessNames( localCats, procNames);

		// Update screen
		UpdateScreen( procNames);

		// Check if we received a SIGWINCH while updating
		sigpending( &pendingSet);
		if( sigismember( &pendingSet, SIGWINCH) == 1)
		{
			// block SIGWINCH...we will check for it manually
			sigprocmask( SIG_BLOCK, &winchSigSet, NULL);

			// Re-init our window to the new terminal size
			CursesClose( false);
			CursesInit( false);
		}
	}
	CursesClose( false);

	return( 0);
}

/**
 * Signal handler. Used to catch SIGINT, SIGQUIT and SIGKILL and perform a
 * graceful shutdown
 *
 * @param sigNo  Signal number being caught
 */
void SignalHandler( int sigNo)
{
	// If this is a SIGTERM signal (generic 'slay' command)
	if( (sigNo == SIGTERM) || (sigNo == SIGINT) || (sigNo == SIGQUIT))
	{
		timeToStop = true;
	}
}

/**
 * Signal handler. Used to catch SIGWINCH and re-reads the
 * terminal size
 *
 * @param sigNo  Signal number being caught
 */
void ResizeWindow( int sigNo)
{
	winResize = true;
}

/**
 * Parses command line arguments
 *
 * @param argc   Number of command line arguments to parse
 * @param argv   Array of command line arguments
 */
void ParseCommandLine( int argc, char **argv)
{
	char	optId;
	int		ii;

	// Disable "Invalid argument" message from getopt
	opterr = 0;

	// Start reparsing from the first argument
	optind = 1;

	// Get Command line options
	for( ii=1; ii<argc; ii++)
	{
		optId = getopt( argc, argv, "l:kn:p:");
		switch( optId)
		{
		case 'l':
			// Validate parameter value
			if( optarg != NULL)		loopPeriod = atol( optarg);
			// If invalid value, default to 1 second
			if( loopPeriod == 0)	loopPeriod = 100;
			break;
		case 'k':
			// Toggls the 'keep dead' option
			keepDead = true;
			break;
		case 'n':
			// Validate parameter value
			if( optarg != NULL)		nodeName = string( optarg);
			break;
		case 'p':
			// Validate parameter value
			if( optarg != NULL)
			{
				sched_param_t	schedParam;

				// Get current scheduling attributes
				if( SchedGet( 0, 0, &schedParam) != -1)
				{
					// Set new priority
					priority = atoi( optarg);
					schedParam.sched_priority = priority;
					if( SchedSet( 0, 0, SCHED_NOCHANGE, &schedParam) == -1)
					{
						perror( "Unable to set new priority");
						exit( 1);
					}
				}
				else
				{
					perror( "Unable to read current priority");;
					exit( 1);
				}
			}
			break;
		default:
			break;
		}
	}
}

/**
 * Fills in the localCats argument with process names read from /dev/name/local/<dirPath>
 *
 * @param localCats List of process names found under /dev/name/local/<dirPath>
 * @param dirPath   Path (relative to /dev/name/local) to read process names from
 */
void ReadProcessNames( ProcList_t &localCats, const char *dirPath)
{
	const string	nameDir = string( "/net/") + nodeName + string("/dev/name/local");
	DIR				*dir;
	struct dirent	*dirent;
	ProcListItr_t	itr;

	// Mark all current process as dead
	for( itr=localCats.begin(); itr!=localCats.end(); itr++)
	{
		ProcLife_t	&proc = itr->second;

		// If process was marked as dead last time
		if( proc.alive == false)
		{
			// If has not been dead for too long, mark as dead at one more sample
			if(proc.deadCount < (deadThresh+1))	proc.deadCount++;
			// Dead for too long...remove from list
			else if (keepDead == false)			localCats.erase( itr--);
		}

		// Mark process as died (will be changed if we find it below)
		proc.alive = false;
	}

	// Open the directory to read which files exist
	if( (dir=opendir( dirPath)) != NULL)
	{
		// Get the next files listing
		while( (dirent = readdir( dir)) != NULL)
		{
			// Check if the file is present in the list already
			string catName = string( dirent->d_name);
			if( (itr=localCats.find( catName)) != localCats.end())
			{
				ProcLife_t	&proc = itr->second;
				// Process was just restarted after being dead, so reset the alive count
				if( proc.deadCount > 0)	proc.aliveCount = 0;

				// Not dead anymore
				proc.deadCount = 0;
				proc.alive = true;

				// Already alive...just increment alive count
				if( proc.aliveCount < (aliveThresh+1))	proc.aliveCount++;
			}
			// New entry
			else
			{
				ProcLife_t	proc;
				proc.alive = true;
				proc.aliveCount = 1;
				proc.deadCount = 0;
				localCats.insert( ProcListVal_t( catName, proc));
			}
		}

		closedir( dir);
	}
}

/**
 * Reads all process name catrgories and the process names registered under
 * those categories in /dev/name/local
 *
 * @param localCats Name categories listed under /dev/name/local
 * @param procNames List of process names listed under each of the categories in /dev/name/local
 */
void GetProcessNames( ProcList_t &localCats, CatProcs_t &procNames)
{
	ProcListItr_t	itr;
	CatProcsItr_t	catItr;
	string			namePath;

	if( localCats.empty())
	{
		procNames.clear();
	}
	else
	{
		for( itr=localCats.begin(); itr!=localCats.end(); itr++)
		{
			const string &category = itr->first;
			// Where to read process names from
			namePath = string( "/net/") + nodeName + string("/dev/name/local/") + category;

			// Have already read entries from this dir...just update what we have
			if( (catItr=procNames.find(category)) != procNames.end())
			{
				ProcList_t		&nameList = catItr->second;
				ReadProcessNames(nameList, namePath.c_str());
			}
			// Have not read this yet
			else
			{
				ProcList_t		nameList;
				ReadProcessNames(nameList, namePath.c_str());
				procNames.insert( CatProcsVal_t( category, nameList));
			}
		}
	}
}


#include <ncurses.h>
static WINDOW	*window;

/**
 * Initializes the curses library (terminal control)
 */
void CursesInit( bool resize)
{
	// Use all rows available.
	int c = tcgetsize(fileno(stdout), &rows, &cols);
	if( (c < 0) || (!(rows)) || (!(cols)) )
	{
		rows = 40;
		cols = 80;
	}

	if( resize == false)
	{
		// Initialize curses screen interface
		initscr();

		// Pass control characters (from keyboard) straight through
		cbreak();

		// Do not echo typed characters locally
		noecho();

		// Enable the reading of the F1-F12, arrow, etc keys
		keypad(stdscr, TRUE);

		// Control characters (CTRL-*) flush the keyboard buffer
		intrflush(stdscr, FALSE);

		// Create our output window (entire screen)
		window = newwin(rows,cols,0,0);

		// Make getch() non-blocking
		nodelay(window, TRUE);

		// If the terminal haas color capabilities
		if( has_colors() == TRUE)
		{
			// Initialize our colors
			start_color();
			init_pair( 1, COLOR_WHITE, COLOR_BLACK);
			init_pair( 2, COLOR_GREEN, COLOR_BLACK);
			init_pair( 3, COLOR_RED, COLOR_BLACK);
			useColor = true;
		}
	}
	else
	{
		// Set to WHITE on BLACK
		ColorOn( 1);

		wresize( window, rows, cols);
	}

	// Clear stdout
	wclear( stdscr);
	wrefresh( stdscr);

	// Erase all previous names listed
	wclear( window);
	wrefresh( window);

	// Turn the cursor off
	curs_set( 0);
}

/**
 * Cleanup the curses library when done
 */
void CursesClose( bool resize)
{
	// Turn off all attributes we may have set
	if( resize == false)	standend();

	// Free memory used for our window
	if( resize == false)	delwin( window);

	// Clear stdout
	wclear( stdscr);

	//Back to our original terminal mode
	if( resize == false)	endwin();
}

/**
 * Updates the screen with the currently running process lists
 *
 * @param procNames List of all running processes with a registered name under /dev/name/local
 */
void UpdateScreen( const CatProcs_t &procNames)
{
	char			nameBuff[ 1024];
	uint32_t		numToErase=0;
	uint32_t		jj, colWidth, nameCount=0;
	int				ii, colorIdx;
	time_t			time_of_day;
	CatProcsCItr_t	dirItr;
	ProcListCItr_t	nameItr;
	const string	dashes1(cols, '-');
	const string	dasehs2(cols, '=');
	const string	blanks( cols, ' ');
	const int 		headerLines = 2;

	// Set to WHITE on BLACK
	ColorOn( 1);

	// Determine the max number of process names over all categories
	for( dirItr=procNames.begin(); dirItr!=procNames.end(); dirItr++)
	{
		const ProcList_t &nameList = dirItr->second;
		if( nameList.size() > nameCount)	nameCount = nameList.size();
	}

	///////////////////////////
	// Output the heading line
	///////////////////////////
	// Erase all previous names listed

	const char *outStr = blanks.c_str();
	for( jj=0; jj<numToErase; jj++)
	{
		mvwaddstr(window, (headerLines+2)+jj, 0, outStr);
		//mvwprintw(window, (headerLines+2)+jj, 0, "%-*.*s    ", cols, cols, " ");
	}
	wrefresh( window);

	// Node name being monitored
	mvwaddstr(window, 0, 0, nodeName.c_str());

	// Time/Date
    time_of_day = time( NULL);
	ctime_r( &time_of_day, nameBuff);
	mvwaddstr(window, 0, cols-strlen(nameBuff+1), nameBuff);

	// Status
	sprintf( nameBuff, "Loop: %d\tKeepDead: %c\t", loopPeriod, keepDead == true ? 'Y' : 'N');
	mvwaddstr(window, 1, 0, nameBuff);
	mvwaddstr(window, 1, cols-strlen(ver), ver);

	// First dashed line
	mvwaddstr(window, headerLines, 0, dashes1.c_str());

	ColorOn( 1);
	numToErase = rows - (headerLines+2);

	// Update new process list
	if( procNames.size())
	{
		// 2nd dashed line
		mvwaddstr(window, headerLines+2, 0, dasehs2.c_str());

		// caluclate the max width of each column
		colWidth = cols/procNames.size()-1;

		for( ii=0, dirItr=procNames.begin(); dirItr!=procNames.end(); ii++, dirItr++)
		{
			const string &catName = dirItr->first;
			const ProcList_t &nameList = dirItr->second;

			ColorOn( 1);
			mvwprintw(window, headerLines+1, 1+ii*colWidth, "%-*.*s", colWidth, colWidth, catName.c_str());

			for( jj=0, nameItr=nameList.begin(); nameItr!=nameList.end(); jj++, nameItr++)
			{
				const string &procName = nameItr->first;
				const ProcLife_t &procLife = nameItr->second;

				nameBuff[ 0] = 0;

				// Decide which color to print the process name in:
				//	Alive for at least 10 samples = WHITE
				//	Alive for less than 10 samples = GREEN
				//	Dead for less than 10 samples = RED
				if( procLife.alive == false)
				{
					colorIdx = 3;
					strcpy( nameBuff, "-");
				}
				else
				{
					if( procLife.aliveCount >= aliveThresh)
					{
						nameBuff[ 0] = 0;
						colorIdx = 1;
					}

					if( procLife.aliveCount < aliveThresh)
					{
						strcpy( nameBuff, "+");
						colorIdx = 2;
					}
				}

				// Get process name to print
				if( nameItr != nameList.end())	strncat( nameBuff, procName.c_str(), colWidth-strlen( nameBuff));
				else							strncat( nameBuff, "    ", colWidth-strlen( nameBuff));

				ColorOn( colorIdx);
				mvwprintw(window, (headerLines+3)+jj, 1+ii*colWidth, "%-*.*s    ", colWidth-2, colWidth-2, nameBuff);
			}
		}
	}
	else if( numToErase)
	{
		// Erase the category line
		mvwprintw(window, headerLines+1, 0, "%-*.*s", cols, cols, " ");
		// Erase the 2nd dashed line
		mvwprintw(window, headerLines+2, 0, "%-*.*s", cols, cols, " ");
		// Erase all previous names listed
		for( jj=(headerLines+3); jj<(uint32_t)rows; jj++)	mvwprintw(window, jj, 0, "%-*.*s    ", cols, cols, " ");
	}
	numToErase = nameCount;

	// Back to WHITE on BLACK
	ColorOn( 1);
	wrefresh( window);
	curs_set( 0);
}

/**
 * Turn on a color attribute for text printed on the screen
 *
 * @param idx    The color index to enable
 */
void ColorOn( int idx)
{

	if( useColor)	wcolor_set(window, idx, NULL);
}

/**
 * Checks to see if the user has pressed a key and processes the key press
 *
 * @return The key that was pressed or -1 if no key has been pressed
 */
int CheckKeyPress()
{
	int key;

	if( (key=wgetch( window)) == ERR)
	{
		key = -1;
	}
	return( key);
}
/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
#ifdef __USAGE

Task Monitor Process
   Press 'k' to toggle the 'keep dead processes on screen' option
   Press 'r' to refresh the screen
   Press 'q' to quit

%C [-n nodeName] [-l period] [-p priority] [-k]

    -n    Node to monitor process on
    -l    Loop at the specified period (in ms, default=100ms)
    -p    Run at the specified priority
    -k    Keep dead processes on the screen (use 'r' to refresh)

#endif
*******************************************************************************/

