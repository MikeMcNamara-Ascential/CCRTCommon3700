#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <curses.h>
#include <limits.h>
#include <string.h>		// for strncpy()
#include <termios.h>	// for tcgetsize()

void ParseCommandLine(int argc, char *argv[]);
void CursesInit();
void UpdateScreen();
int ReadInputs();
int ReadOutputs();
int GetDataSize( const char *path);
void LineUp();
void LineDown();

uint8_t			writeInit = 0;
int				plcInFd=-1, plcOutFd=-1;
int				inCount = 100, outCount=100;
uint32_t		*plcInputs, *plcOutputs;
char			plcInPath[ _POSIX_PATH_MAX+1] ="/dev/PLC/Inputs";
char			plcOutPath[ _POSIX_PATH_MAX+1] ="/dev/PLC/Outputs";
static WINDOW	*window;

int             startLine, endLine;

int main( int argc, char *argv[])
{
	int 	ii;
	int		lastChar = ' ';

	ParseCommandLine(argc,argv);

	if( ((plcInFd = open( plcInPath, O_RDWR)) != -1) &&
		((plcOutFd = open( plcOutPath, O_RDONLY)) != -1))
	{

		inCount = GetDataSize( plcInPath);
		plcInputs = calloc( inCount, sizeof( uint32_t));
		
		outCount = GetDataSize( plcOutPath);
		plcOutputs = calloc( outCount, sizeof( uint32_t));

		if( writeInit != 0)
		{
			printf( "Writing input data...");	fflush( stdout);
			for( ii=0; ii<inCount; ii++)
			{
				plcInputs[ ii] = ii;
			}
			lseek( plcInFd, 0, SEEK_SET);
			ii = write( plcInFd, plcInputs, inCount*sizeof( uint32_t));
			printf( "%d bytes written\n", ii);
			delay( 500);
		}
		
		for( ii=0; ii<inCount; ii++)		plcInputs[ ii] = inCount-ii;
		for( ii=0; ii<outCount; ii++)		plcOutputs[ ii] = outCount-ii;

		CursesInit();
		
        while(lastChar != 'q')
		{
            if ((lastChar == 'd') || (lastChar == KEY_DOWN))       LineDown();
            else if (lastChar == 'u' || (lastChar == KEY_UP))    LineUp();

			UpdateScreen();
			// Wait for input to be available
			if( (lastChar=halfdelay( 1)) != ERR)
			{
				// Input is available...go read a character
				lastChar = wgetch(window);
			}
		}
		free( plcInputs);	free( plcOutputs);
		close( plcInFd);	close( plcOutFd);

		clear();
		refresh();
		endwin();
		curs_set( 1);
	}
	else
	{
		perror( "Error opening device(s)");
	}

	return( 0);
}

void ParseCommandLine(int argc, char *argv[])
{
	char optId;
	
	// Read port name from command line
	/////////////////////////////////////
	// Do not print errors for unknown arguments
	opterr = 0;
	// Start reparsing from the first argument
	optind = 1;

	while( optind < argc)
	{
		// Clear aoption argument string
		optarg = NULL;
		// Get next command line option
		optId = getopt( argc, argv, "i:o:w");
		switch( optId)
		{
		case 'i':		// path to inputs
			strncpy( plcInPath, optarg, _POSIX_PATH_MAX);;
			break;
		case 'o':		// path to outputs
			strncpy( plcOutPath, optarg, _POSIX_PATH_MAX);;
			break;
		case 'w':
			writeInit ^= 0x01;
			break;
		default:
			if( optId == '?')						optind++;
			else if(argv[ optind] == NULL)			optind++;
			else if(argv[optind][0] != '-')	   		optind++;
			else if((argv[ optind][0] == '-') && (strlen(argv[ optind]) == 1)) optind++;
			break;
		}
		optarg = NULL;
	}
}

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
	
    startLine = 0;
    endLine = startLine + rows;

	initscr();
	nonl();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	intrflush(stdscr, FALSE);

	window = newwin(rows,cols,0,0);
	nodelay(window, TRUE);
    keypad(window, TRUE);

	clear();
	curs_set( 0);
}

void UpdateScreen()
{
	static int	spinIdx=0;
	static char spinner[] = "/-\\|";
	int 		ii, maxIdx=inCount, ins, outs;
	char		lineBuff[ 1024];
	uint32_t	inVal, outVal;
    int         lastLine;

	ins = ReadInputs();
	outs = ReadOutputs();
	
	if( outCount > inCount)	maxIdx = outCount;

	sprintf( lineBuff, " %c     N16 %2d     N17 %2d   ", spinner[ spinIdx++ % 4], ins, outs);
	mvwaddstr(window, 0, 5, lineBuff);
	strcpy( lineBuff,  "------------------------");
	mvwaddstr(window, 1, 5, lineBuff);

    if (inCount > outCount)     lastLine = inCount;
    else                        lastLine = outCount;

	for( ii=startLine; ii<lastLine; ii++)
	{
		inVal = outVal = 0xFFFF;
		if( ii < inCount)	inVal = plcInputs[ ii];
		if( ii < outCount)	outVal = plcOutputs[ ii];

		sprintf( lineBuff, " %02d.    $%04X    $%04X", ii, inVal, outVal);
		mvwaddstr(window, ii+2-startLine, 5, lineBuff);
	}
	wrefresh( window);
	curs_set( 0);
}

int ReadInputs()
{
	int ii, jj;

	lseek( plcInFd, 0, SEEK_SET);
	ii = read( plcInFd, plcInputs, inCount*sizeof( uint32_t));
	if( ii < 0)	
	{
		ii = 0;
		close( plcInFd);
		plcInFd = open( plcInPath, O_RDWR);
	}
	
	if( ii != inCount*sizeof( uint32_t))
	{
		for( jj=ii; jj<inCount; jj++)
		{
			plcInputs[ jj] = 0xA5A5;
		}
	}

	return( ii / sizeof( uint32_t));
}

int ReadOutputs()
{
	int ii, jj;
	
	lseek( plcOutFd, 0, SEEK_SET);
	ii = read( plcOutFd, plcOutputs, outCount*sizeof( uint32_t));
	if( ii < 0)	
	{
		ii = 0;
		close( plcOutFd);
		plcOutFd = open( plcOutPath, O_RDONLY);
	}
	
	if( ii != outCount*sizeof( uint32_t))
	{
		for( jj=ii; jj<outCount; jj++)
		{
			plcOutputs[ jj] = 0x5A5A;
		}
	}
	
	return( ii / sizeof( uint32_t));
}

int GetDataSize( const char *path)
{
	int count = 50;
	struct stat	fStat;
	
	if( stat( path, &fStat) != -1)
	{
		count = fStat.st_size/sizeof( uint32_t);
	}

	return( count);
}

void LineUp()
{
    if (startLine > 0)
    {
        startLine--;
        endLine--;
    }
}

void LineDown()
{
    if (endLine <= inCount + 1)
    {
        startLine++;
        endLine++;
    }
}
