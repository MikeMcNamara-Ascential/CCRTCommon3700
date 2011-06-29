//******************************************************************************
// Description:
//  Base class for all TaskMonTerm window and panel objects
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonPanel.cpp $
// 
// 2     4/13/06 3:59a Cward
// Update task Mon term for scolling 
//
// 3     4/05/06 11:16a Bmeinke
// Added a title color attribute to allow the title to be displayed in a
// color other than the default window color
// Added a GetBoxChars() method to allow derived classes to customize the
// border
//
// 2     4/04/06 4:55p Bmeinke
// m_rows and m_cols needto be decremented by 2 (not 1) to account for the
// top/bottom and the right/left borders
//
// 1     7/20/05 8:24p Cward
// Added new TaskMon
//
// 1     5/17/05 6:57p Bmeinke
// Core R05051101 release with TaskMon updates
//
// 1     4/25/05 4:11p Bmeinke
//
// 1     4/25/05 3:39p Bmeinke
// NewTaskMon task monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "TaskMonPanel.h"
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <stdarg.h>
#include <string.h>

#include <sys/utsname.h>

/**
 * Construct a new panel with 'rows' rows and 'columns' columnss with a top-left
 * screen corner of (top,left)
 *
 * @param title    Window title (default="Panel")
 * @param rows     Number of rows for this panel or -1 for full terminal height
 *                 (default=terminal row count)
 * @param columns  Number of columns for this panel or -1 for full terminal width
 *                 (default=terminal column count)
 * @param begin_y  Defines the top left corner of the window (in conjunction with 'left')
 *                 (default=0)
 * @param begin_x  Defines the top left corner of the window (in conjunction with 'top')
 *                 (default=0)
 * @param colorIdx Default color index used to draw this window
 */
TaskMonPanel::TaskMonPanel( const std::string &title, int rows, int columns, int begin_y, int begin_x, int colorIdx) :
	m_window( NULL), m_panel( NULL), m_top( begin_y), m_left( begin_x), m_rows( rows), m_cols( columns),
	m_title( title), m_useColor( true), m_useRealBox( false), m_isActive( false), m_windowColorIdx( colorIdx),
	m_hasBorder( true), m_titleColorIdx( colorIdx)
{
	// Create the window
	m_window = newwin( m_rows, m_cols, m_top, m_left);

	// Create the panel
	m_panel = new_panel( m_window);

	// Enable the reading of the F1-F12, arrow, etc keys
	keypad( m_window, TRUE);

	// Make getch() non-blocking
	nodelay( m_window, TRUE);

	// Hide the cursor
	curs_set(0);

	// OK to scroll
	scrollok( m_window, true);

	// Reset window coordinates relative to this window
	m_left = 0;
	m_top = 0;

	// If this terminal does not support colors
	m_useColor = (has_colors() == TRUE);

	// Adjust our dimensions for the box around this panel
	m_left++;	m_cols -= 2;
	m_top++;	m_rows -= 2;

	// Set the scrolling region to be the area insode the border
	wsetscrreg(m_window, m_top, m_rows);

	// Clear the window (with proper attributes)
	wbkgd( m_window, ' ' | COLOR_PAIR(GetWindowColor()));
	ColorOn(GetWindowColor());
	werase( m_window);
}

TaskMonPanel::~TaskMonPanel()
{
	if( m_panel != NULL)
	{
		del_panel( m_panel);
		m_panel = NULL;
	}

	if( m_window != NULL)
	{
		wclear( m_window);
		delwin( m_window);
		m_window = NULL;
	}
}

/**
 * Sets the title for the panel
 *
 * @param title
 */
void TaskMonPanel::SetTitle( const std::string &title)
{
	m_title = title;
}

/**
 * Tell the panel to use (enable=true) or simulate(enable=false)
 * the builtin curses box drawing routines. The local QNX console
 * does not draw the horizontal lines properly.
 *
 * @param enable true to use the builtin curses box drawing routines
 *               false to use a smaller horizontal dash
 */
void TaskMonPanel::UseRealBox( bool enable)
{
	m_useRealBox = enable;
}

/**
 * Method used to refresh/update the data in the panel
 */
void TaskMonPanel::Update()
{
	AttributeOn( COLOR_PAIR(GetWindowColor()));

	// Draw the box
	if( true == m_isActive)	AttributeOn( A_BOLD);
	Box();
	if( true == m_isActive)	AttributeOff( A_BOLD);

	if( false == m_title.empty())
	{
		std::string	title( m_title);
		title = " " + title + " ";

		AttributeOn( COLOR_PAIR(GetTitleColor()));
		if( true == m_isActive)	AttributeOn( A_REVERSE);
		Print( 0, CENTER_JUSTIFY, title);
		if( true == m_isActive)	AttributeOff( A_REVERSE);
		AttributeOn( COLOR_PAIR(GetWindowColor()));
	}
}

/**
 * Draw a box around the current window
 */
void TaskMonPanel::Box()
{
	chtype	uLine, bLine;
	chtype	lLine, rLine;
	chtype	ulCorner, urCorner;
	chtype	blCorner, brCorner;

	// If this window has a border
	if( true == m_hasBorder)
	{
		GetBoxChars( uLine, bLine, lLine, rLine, ulCorner, urCorner, blCorner, brCorner);

		wborder( m_window, lLine, rLine, uLine, bLine,
				 ulCorner, urCorner, blCorner, brCorner);
	}
}

/**
 * Gets the characters to use for box drawing
 *
 * @param uLine    Upper edge vertical line character
 * @param bLine    Bottom edge vertical line character
 * @param lLine    Left edge horizontal line char
 * @param hLine    Right edge horizontal line char
 * @param ulCorner Upper left corner character
 * @param urCorner Upper right corner character
 * @param blCorner Bottom left corner character
 * @param brCorner Bottom right corner character
 */
void TaskMonPanel::GetBoxChars( chtype &uLine, chtype &bLine,
								chtype &lLine, chtype &rLine,
								chtype &ulCorner, chtype &urCorner,
								chtype &blCorner, chtype &brCorner) const
{
	uLine = bLine = ACS_HLINE;
	lLine = rLine = ACS_VLINE;
	ulCorner = ACS_ULCORNER;
	urCorner = ACS_URCORNER;
	blCorner = ACS_LLCORNER;
	brCorner = ACS_LRCORNER;

		// If we can't use colors
		if( false == m_useColor)
		{
			if(false == m_useRealBox)
			{
				// Use the ugly ASCII characters
			lLine = rLine = '|';
			uLine = bLine = '-';
				ulCorner = '+';	urCorner = '+';
				blCorner = '+';	brCorner = '+';
			}
		}
		/**
		 * The qansi family of terminals do not draw the ACS_HLINE nicely:
		 * 	it ends up being drawn with REVERSE and BLINK attributes. If
		 * 	we are not forcing the use of the real ACS_HLINE character,
		 * 	use a small dash character for our horizontal lines
		 */
		else if(false == m_useRealBox)
		{
		uLine = bLine = '-';
	}
}


/**
 * Sets the default color index for this window
 */
int TaskMonPanel::GetWindowColor() const
{
	return( m_windowColorIdx);
}

/**
 * Sets the default color index for this window
 *
 * @param colorIdx Color index to use for this window
 */
void TaskMonPanel::SetWindowColor( int colorIdx)
{
	if( GetWindowColor() != colorIdx)
	{
		m_windowColorIdx = colorIdx;
	}
}

/**
 * Check is this window has a border
 *
 * @return true if this window has a border, false if it does not
 */
bool TaskMonPanel::HasBorder() const
{
	return( m_hasBorder);
}

/**
 * Sets the default color index for this window's title
 */
int TaskMonPanel::GetTitleColor() const
{
	return( m_titleColorIdx);
}

/**
 * Sets the default color index for this window's title
 *
 * @param colorIdx Color index to use for this window's title
 */
void TaskMonPanel::SetTitleColor( int colorIdx)
{
	m_titleColorIdx = colorIdx;
}

/**
 * Turns on or off the windo border
 *
 * @param hasBorder true to draw the window with a border
 */
void TaskMonPanel::SetBorder( bool hasBorder)
{
	if( m_hasBorder != hasBorder)
	{
		m_hasBorder = hasBorder;

		// Adjust our dimensions for the box around this panel
		if( m_hasBorder)
		{
			m_left = 1;	m_cols--;
			m_top = 1;	m_rows--;
		}
		else
		{
			m_left = 0;	m_cols++;
			m_top = 0;	m_rows++;
		}
	}
}

/**
 * Print text starting at the current row and column
 *
 * @param fmt    printf style format string
 */
void TaskMonPanel::Print( const char *fmt, ...)
{
	va_list	args;
	char	junk[ m_cols+1];

	va_start( args, fmt);
	vsnprintf( junk, m_cols, fmt, args);
	va_end( args);

	std::string	text( junk);
	Print( text);
}

/**
 * Print text starting at the current row and column
 *
 * @param text   Text to be printed
 */
void TaskMonPanel::Print( const std::string &text)
{
	waddstr( m_window, text.c_str());
}

/**
 * Print text starting at the given row and column
 * with the given justification
 *
 * @param row    Row to start text at
 * @param justification
 *               How text should be justified
 * @param fmt    printf style format string
 *
 * @see TextJustify_t
 */
void TaskMonPanel::Print( int row, TextJustify_t justification, const char *fmt, ...)
{
	va_list	args;
	char	junk[ m_cols+1];

	va_start( args, fmt);
	vsnprintf( junk, m_cols, fmt, args);
	va_end( args);

	std::string	text( junk);
	Print( row, justification, text);
}

/**
 * Print text starting at the given row and column
 * with the given justification
 *
 * @param row    Row to start text at
 * @param justification
 *               How text should be justified
 * @param text   Text to be printed
 *
 * @see TextJustify_t
 */
void TaskMonPanel::Print( int row, TextJustify_t justification, const std::string &text)
{
	int		len = text.size();
	int		column;

	switch( justification)
	{
	case CENTER_JUSTIFY:	column = (m_cols - len) / 2;	break;
	case RIGHT_JUSTIFY:		column = m_cols - len + 1;		break;
	default:
	case LEFT_JUSTIFY:		column = m_left + 0;			break;
	}
	Print( row, column, text);
}

/**
 * Print text starting at the given row and column
 * with the given justification
 *
 * @param row    Row to start text at
 * @param col    Column to start at
 * @param fmt    printf style format string
 */
void TaskMonPanel::Print( int row, int col, const char *fmt, ...)
{
	va_list	args;
	char	junk[ m_cols+1];

	va_start( args, fmt);
	vsnprintf( junk, m_cols, fmt, args);
	va_end( args);

	std::string	text( junk);
	Print( row, col, text);
}

/**
 * Print text starting at the given row and column
 * with the given justification
 *
 * @param row    Row to start text at
 * @param col    Column to start at
 * @param text   Text to be printed
 */
void TaskMonPanel::Print( int row, int col, const std::string &text)
{
	mvwaddstr( m_window, row, col, text.c_str());
}

/**
 * Turns the given attribute on
 *
 * @param attr   Attribute to be turned on
 */
void TaskMonPanel::AttributeOn( int attr)
{
	wattron( m_window, attr);
}

/**
 * Turns the given attribute off
 *
 * @param attr   Attribute to be turned off
 */
void TaskMonPanel::AttributeOff( int attr)
{
	wattroff( m_window, attr);
}

/**
 * Clears fro mthe current row to the bottom of the window
 */
void TaskMonPanel::ClearToBottom()
{
	wclrtobot( m_window);
}

/**
 * Moves the cursor to the given row and column
 *
 * @param row    Row to move the cursor to
 * @param col    Column to move the cursor to
 */
void TaskMonPanel::MoveTo( int row, int col)
{
	wmove( m_window, row, col);
}

/**
 * Gets the current cursor position
 *
 * @param row    Current row
 * @param col    Current column
 */
void TaskMonPanel::GetPosition( int &row, int &col)
{
	getyx( m_window, row, col);
}

/**
 * Checks to see if the user has pressed a key and processes the key press
 *
 * @param keyCode Key code for the key that was pressed
 *
 * @return true if a key was pressed, false if no key was pressed
 */
bool TaskMonPanel::CheckKeyPress( int &keyCode)
{
	bool keyWasPressed = false;

	keyCode = -1;
	if( (keyCode=wgetch(m_window)) != ERR)
	{
		keyWasPressed = true;
	}
	return( keyWasPressed);
}

/**
 * Turn on a color attribute for text printed on the screen
 *
 * @param idx    The color index to enable
 */
void TaskMonPanel::ColorOn( int idx)
{
	if( true == m_useColor)
	{
		// If this is already in attribute form
		if( idx >= 64)	AttributeOn( idx);
		// Convert color into an attribute
		else			AttributeOn( COLOR_PAIR(idx));
	}
}

/**
 * Activates (isActive=true) or de-activates (isActive=false) this panel
 *
 * @param isActive Flag indicating whether this panel is active (true) or not (false)
 */
void TaskMonPanel::SetActive( bool isActive)
{
	m_isActive = isActive;
}

/**
 * Scrolls the windows contents by the specified number of lines
 *
 * @param lines  Number of lines to scroll the windows by
 */
void TaskMonPanel::Scroll( int lines)
{
	wscrl( m_window, lines);
}

/**
 * Method used to read the number of rows in the current terminal
 *
 * @return The number of rows in the current terminal
 */
int TaskMonPanel::GetTermRows()
{
	int cc, rows, cols;

	// Use all rows available.
	cc = tcgetsize(fileno(stdout), &rows, &cols);
	if( (cc < 0) || (!(rows)) || (!(cols)) )
	{
		rows = 40;
		cols = 80;
	}

	return( rows);
}

/**
 * Method used to read the number of columns in the current terminal
 *
 * @return The number of columns in the current terminal
 */
int TaskMonPanel::GetTermCols()
{
	int cc, rows, cols;

	// Use all rows available.
	cc = tcgetsize(fileno(stdout), &rows, &cols);
	if( (cc < 0) || (!(rows)) || (!(cols)) )
	{
		rows = 40;
		cols = 80;
	}

	return( cols);
}
