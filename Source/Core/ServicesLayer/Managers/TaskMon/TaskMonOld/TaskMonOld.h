
#ifndef _TASKMONOLD_H_INCLUDED_
#define _TASKMONOLD_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <signal.h>
#include <termios.h>
#include <sys/neutrino.h>
#include <sys/utsname.h>

#include <string>
#include <set>
#include <map>

using namespace std;

/**
 * Structure used to hold information about a process' existence
 */
typedef struct
{
	/**
	 * Is the process name still listed under /dev/name/local?
	 */
	bool		alive;

	/**
	 * Number of consecutive loops that the proicess name has been listed
	 * under /dev/name/local
	 */
	uint32_t	aliveCount;

	/**
	 * Number of consecutive loops that the proicess name has been absent
	 * from /dev/name/local
	 */
	uint32_t	deadCount;

} ProcLife_t;

/**
 * Data type used to hold lifetime attributes for processes with names
 * listed under /dev/name/local
 */
typedef map< string, ProcLife_t>		ProcList_t;
typedef ProcList_t::iterator			ProcListItr_t;
typedef ProcList_t::const_iterator		ProcListCItr_t;
typedef ProcList_t::value_type			ProcListVal_t;

/**
 * Categorized list of process names listed under /dev/name/local
 */
typedef map< string, ProcList_t>		CatProcs_t;
typedef CatProcs_t::iterator  			CatProcsItr_t;
typedef CatProcs_t::const_iterator		CatProcsCItr_t;
typedef CatProcs_t::value_type			CatProcsVal_t;

/**
 * Signal handler. Used to catch SIGINT, SIGQUIT and SIGKILL and perform a
 * graceful shutdown
 *
 * @param sigNo  Signal number being caught
 */
void SignalHandler( int sigNo);

/**
 * Signal handler. Used to catch SIGWINCH and re-reads the
 * terminal size
 *
 * @param sigNo  Signal number being caught
 */
void ResizeWindow( int sigNo);

/**
 * Parses command line arguments
 *
 * @param argc   Number of command line arguments to parse
 * @param argv   Array of command line arguments
 */
void ParseCommandLine( int argc, char **argv);

/**
 * Fills in the localCats argument with process names read from /dev/name/local/<dirPath>
 *
 * @param localCats List of process names found under /dev/name/local/<dirPath>
 * @param dirPath   Path (relative to /dev/name/local) to read process names from
 */
void ReadProcessNames( ProcList_t &localCats, const char *dirPath);

/**
 * Reads all process name catrgories and the process names registered under
 * those categories in /dev/name/local
 *
 * @param localCats Name categories listed under /dev/name/local
 * @param procNames List of process names listed under each of the categories in /dev/name/local
 */
void GetProcessNames( ProcList_t &localCats, CatProcs_t &procNames);

/**
 * Initializes the curses library (terminal control)
 */
void CursesInit( bool resize);

/**
 * Cleanup the curses library when done
 */
void CursesClose( bool resize);

/**
 * Updates the screen with the currently running process lists
 *
 * @param procNames List of all running processes with a registered name under /dev/name/local
 */
void UpdateScreen( const CatProcs_t &procNames);

/**
 * Turn on a color attribute for text printed on the screen
 *
 * @param idx    The color index to enable
 */
void ColorOn( int idx);

/**
 * Checks to see if the user has pressed a key and processes the key press
 *
 * @return The key that was pressed or -1 if no key has been pressed
 */
int CheckKeyPress();

#endif // _TASKMON_H_INCLUDED_
