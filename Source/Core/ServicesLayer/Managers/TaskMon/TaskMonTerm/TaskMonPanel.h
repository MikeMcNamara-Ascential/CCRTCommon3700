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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonPanel.h $
// 
// 3     10/30/07 4:07p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     4/13/06 3:59a Cward
// Update task Mon term for scolling 
//
// 2     4/05/06 11:16a Bmeinke
// Added a title color attribute to allow the title to be displayed in a
// color other than the default window color
// Added a GetBoxChars() method to allow derived classes to customize the
// border
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

#ifndef _TASKMONPANEL_H_INCLUDED_
#define _TASKMONPANEL_H_INCLUDED_

#include "TaskMonDefs.h"
#include "TaskMonTaskList.h"

#include <string>
#include <panel.h>
#include <curses.h>

/**
 * Base panel class
 */
class TaskMonPanel
{
public:
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
    TaskMonPanel( const std::string &title="Panel", int rows=TaskMonPanel::GetTermRows(),
                  int columns=TaskMonPanel::GetTermCols(), int begin_y=0, int begin_x=0,
                  int colorIdx=DEFAULT_COLOR);

    virtual ~TaskMonPanel();

    /**
     * Sets the title for the panel
     *
     * @param title
     */
    virtual void SetTitle( const std::string &title);

    /**
     * Tell the panel to use (enable=true) or simulate(enable=false)
     * the builtin curses box drawing routines. The local QNX console
     * does not draw the horizontal lines properly.
     *
     * @param enable true to use the builtin curses box drawing routines
     *               false to use a smaller horizontal dash
     */
    void UseRealBox( bool enable=true);

    /**
     * Method used to refresh/update the data in the panel
     */
    virtual void Update();

    /**
     * Checks to see if the user has pressed a key and processes the key press
     *
     * @param keyCode Key code for the key that was pressed
     *
     * @return true if a key was pressed, false if no key was pressed
     */
    bool CheckKeyPress( int &keyCode);

    /**
     * Activates (isActive=true) or de-activates (isActive=false) this panel
     *
     * @param isActive Flag indicating whether this panel is active (true) or not (false)
     */
    virtual void SetActive( bool isActive=true);

    /**
     * Scrolls the windows contents by the specified number of lines
     *
     * @param lines  Number of lines to scroll the windows by
     */
    virtual void Scroll( int lines=1);

    /**
     * Method used to read the number of rows in the current terminal
     *
     * @return The number of rows in the current terminal
     */
    static int GetTermRows();

    /**
     * Method used to read the number of columns in the current terminal
     *
     * @return The number of columns in the current terminal
     */
    static int GetTermCols();

protected:

    /**
     * Text justification values for the Print() method
     */
    enum TextJustify_t
    {
        LEFT_JUSTIFY,
        CENTER_JUSTIFY,
        RIGHT_JUSTIFY
    };

    /**
     * Sets the default color index for this window
     */
    int GetWindowColor() const;

    /**
     * Sets the default color index for this window
     *
     * @param colorIdx Color index to use for this window
     */
    void SetWindowColor( int colorIdx);

    /**
     * Check is this window has a border
     *
     * @return true if this window has a border, false if it does not
     */
    bool HasBorder() const;

    /**
     * Sets the default color index for this window's title
     */
    virtual int GetTitleColor() const;

    /**
     * Sets the default color index for this window's title
     *
     * @param colorIdx Color index to use for this window's title
     */
    virtual void SetTitleColor( int colorIdx);

    /**
     * Turns on or off the windo border
     *
     * @param hasBorder true to draw the window with a border
     */
    void SetBorder( bool hasBorder=true);

    /**
     * Turn on a color attribute for text printed on the screen
     *
     * @param idx    The color index to enable
     */
    void ColorOn( int idx);

    /**
     * Draw a box around the current window
     */
    virtual void Box();

    /**
     * Gets the characters to use for box drawing
     *
     * @param uLine    Upper edge vertical line character
     * @param bLine    Bottom edge vertical line character
     * @param lLine    Left edge horizontal line char
     * @param rLine    Right edge horizontal line char
     * @param ulCorner Upper left corner character
     * @param urCorner Upper right corner character
     * @param blCorner Bottom left corner character
     * @param brCorner Bottom right corner character
     */
    virtual void GetBoxChars( chtype &uLine, chtype &bLine,
                              chtype &lLine, chtype &rLine,
                              chtype &ulCorner, chtype &urCorner,
                              chtype &blCorner, chtype &brCorner) const;

    /**
     * Print text starting at the current row and column
     *
     * @param fmt    printf style format string
     */
    void Print( const char *fmt, ...);

    /**
     * Print text starting at the current row and column
     *
     * @param text   Text to be printed
     */
    void Print( const std::string &text);

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
    void Print( int row, TextJustify_t justification, const char *fmt, ...);

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
    void Print( int row, TextJustify_t justification, const std::string &text);

    /**
     * Print text starting at the given row and column
     * with the given justification
     *
     * @param row    Row to start text at
     * @param col    Column to start at
     * @param fmt    printf style format string
     */
    void Print( int row, int col, const char *fmt, ...);

    /**
     * Print text starting at the given row and column
     * with the given justification
     *
     * @param row    Row to start text at
     * @param col    Column to start at
     * @param text   Text to be printed
     */
    void Print( int row, int col, const std::string &text);

    /**
     * Turns the given attribute on
     *
     * @param attr   Attribute to be turned on
     */
    void AttributeOn( int attr);

    /**
     * Turns the given attribute off
     *
     * @param attr   Attribute to be turned off
     */
    void AttributeOff( int attr);

    /**
     * Clears fro mthe current row to the bottom of the window
     */
    void ClearToBottom();

    /**
     * Moves the cursor to the given row and column
     *
     * @param row    Row to move the cursor to
     * @param col    Column to move the cursor to
     */
    void MoveTo( int row, int col);

    /**
     * Gets the current cursor position
     *
     * @param row    Current row
     * @param col    Current column
     */
    void GetPosition( int &row, int &col);

    /**
     * Underlying window for this panel
     */
    WINDOW      *m_window;

    /**
     * Underlying panel object
     */
    PANEL       *m_panel;

    /**
     * Y coordinate of the top left corner of the panel
     */
    int         m_top;

    /**
     * X coordinate of the top left corner of the panel
     */
    int         m_left;

    /**
     * Number of rows for this panel (height)
     */
    int         m_rows;

    /**
     * Number of columns for this panel (width)
     */
    int         m_cols;

    /**
     * Title for this panel
     */
    std::string m_title;

    /**
     * Flag to indicate if we can use colors or not
     */
    bool        m_useColor;

    /**
     * Flag to indicate if we should use the real curses box drawing
     */
    bool        m_useRealBox;

    /**
     * Flag indicating whether this panel is active (true) or not (false)
     */
    bool        m_isActive;

    /**
     * Default color index for this window
     */
    int         m_windowColorIdx;

    /**
     * Flag indicating whether this panel should be drawn with a border
     */
    bool        m_hasBorder;

    /**
     * Color to draw the border in
     */
    int         m_titleColorIdx;
};

#endif // _TASKMONPANEL_H_INCLUDED_

