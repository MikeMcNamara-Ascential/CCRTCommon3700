#include <stdio.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#include "IcmTerm.h"

const int		dataItems = 5;				   
				   
volatile bool	timeToStop = false;
const char 		*paths[] = {	"/dev/Icm/Block/PulseCount",
								"/dev/Icm/Block/Speed",
								"/dev/Icm/Block/Acceleration",
								"/dev/Icm/Block/Force",
								"/dev/Icm/Block/Loss" };
int				fd[ dataItems] = { -1, -1, -1, -1 };
float			data[ dataItems][ 6];
int				blockSize = 0;



void SignalHandler( int sigNo)
{
	const char *signOff = "SignalHandler\n";
	write( 1, signOff, strlen (signOff)+1);
	// If this is a SIGTERM signal (generic 'slay' command)
	if( (sigNo == SIGTERM) || (sigNo == SIGINT) || (sigNo == SIGQUIT))
	{
		const char *signOff = "Time to exit\n";
		
		write( 1, signOff, strlen(signOff)+1);
		timeToStop = true;
	}
}

int main( int argc, char *argv[])
{
	printf( "Enter main()\n");

	// Find all the PCI40 carriers and ICM IP modules
	Initialize();

	// Read data
	Run();

	// Cleanup
	Finalize();

	printf( "Exit main()\n");

	return( 0);
}

void Initialize()
{
	struct stat		fileStat;

	printf( "Enter Initialize()\n");
	
	signal( SIGTERM, SignalHandler);
	signal( SIGINT, SignalHandler);
	signal( SIGQUIT, SignalHandler);

	for( int ii=0; ii<dataItems; ii++)
	{
		if( (fd[ ii] = open( paths[ ii], O_RDONLY)) == -1)
		{
			printf( "open( %s) failed: %s\n", paths[ ii], strerror( errno));
			while( ii--)
			{
				close( fd[ ii]);
				fd[ ii] = -1;
			}
			break;
		}
		fstat( fd[ ii], &fileStat);
		if( fileStat.st_size > blockSize)	blockSize = fileStat.st_size;
	}

	blockSize /= sizeof( float);


	printf( "Exit Initialize()\n");
}

void Run()
{
	const int		loopTime = 100;

	for( int ii=0; ii<80; ii++)		printf( "\n");
	
	// Initialize our windowing stuff
	CursesInit();
	
	// Wait for interrupts to occurr
	while( timeToStop == false)
	{
		SampleData();

		UpdateScreen();
		
		delay( loopTime);

	}
}

void SampleData()
{
	int	ii;

	for( ii=0; ii<dataItems; ii++)
	{
		if( read( fd[ ii], data[ ii], blockSize*sizeof( float)) < 0)
		{
			fd[ ii] = open( paths[ ii], O_RDONLY);
		}
	}
}

void Finalize()
{
	CursesClose();
	
	printf( "Enter Finalize()\n");

	for( int ii=0; ii<dataItems; ii++)
	{
		if( fd[ ii] != -1)	close( fd[ ii]);
		fd[ ii] = -1;
	}
	printf( "Exit Finalize()\n");
}

#include <curses.h>

static WINDOW	*window;

void CursesInit()
{
	int c, rows, cols;
	/*
		Use all rows available.
		Last row might be used by terminal status line though. Can't help it...
	*/
	c = tcgetsize(fileno(stdout), &rows, &cols);
	if (c < 0 || !rows || !cols)
	{
		rows = 40;
		cols = 80;
	}
	
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	intrflush(stdscr, FALSE);

	window = newwin(rows,cols,0,0);
	nodelay(window, TRUE);

	wclear( stdscr);
	curs_set( 0);
}

void CursesClose()
{
	wclear( stdscr);
	delwin( window);
	endwin();
}

void UpdateScreen()
{
	static int	spinIdx=0;
	static char spinner[] = "/-\\|";
	int 		ii;
	char		lineBuff[ 1024];
	char		countBuff[ 128];
	char		*tab = "    ";

	sprintf( lineBuff, "%c %8.8s%s%8.8s%s%8.8s%s%8.8s%s%8.8s", spinner[ spinIdx++ %4], 
			 "Count", tab, "Speed", tab, "Accel", tab, "Force", tab, "Loss");
	mvwaddstr(window, 1, 5, lineBuff);
	
	std::string	dashes( strlen( lineBuff), '-');
	
	strcpy( lineBuff,  dashes.c_str());
	mvwaddstr(window, 2, 5, lineBuff);
	for( ii=0; ii<4; ii++)
	{
		itoa((uint32_t)data[ 0][ ii], countBuff, 10);
		sprintf( lineBuff, "  %8.8s%s%8.3f%s%8.3f%s%8.3f%s%8.3f\n",
				 countBuff, tab, data[ 1][ ii], tab, 
				 data[ 2][ ii], tab, data[ 3][ ii], tab, 
				 data[ 4][ ii]);
		mvwaddstr(window, ii+3, 5, lineBuff);
	}
	wrefresh( window);
	curs_set( 0);
}



/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
	
#ifdef __USAGE

Altera IP module ICM driver

    Notes:
    - Must be run as Super User.
    - $Revision: 1 $
    - $Date: 11/02/07 8:08a $

#endif

*******************************************************************************/

