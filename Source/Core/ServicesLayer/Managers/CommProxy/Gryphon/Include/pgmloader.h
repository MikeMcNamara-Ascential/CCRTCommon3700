// not used at this time

/* $Id: pgmloader.h,v 1.4 2000/03/28 18:00:51 stevelim Exp $ */

/* Program loader (SD_PGM) commands: */

#define CMD_PGM_DESC	    0x90    /* Describe a program to to uploaded    */
#define CMD_PGM_UPLOAD	    0x91    /* Upload a program to the Gryphon	    */
#define CMD_PGM_DELETE	    0x92    /* Delete an uploaded program   	    */
#define CMD_PGM_LIST	    0x93    /* Get a list of uploaded programs	    */
#define CMD_PGM_START	    0x94    /* Start an uploaded program    	    */
#define CMD_PGM_STOP	    0x95    /* Stop an uploaded program     	    */
#define CMD_PGM_STATUS	    0x96    /* Get the status of an uploaded program*/
#define CMD_PGM_OPTIONS	    0x97    /* Set the upload options               */
#define CMD_PGM_FILES	    0x98    /* Get a list of file names     	    */

#define PGM_CONV    	    1 	    /* Type of data conversion to perform   */
#define PGM_TYPE    	    2 	    /* Type of file			    */

#define PGM_BIN     	    11 	    /* Binary, no conversion		    */
#define PGM_ASCII   	    12 	    /* ASCII, convert CR LF to LF	    */
#define PGM_PGM     	    21 	    /* Executable			    */
#define PGM_DATA    	    22 	    /* Data				    */
