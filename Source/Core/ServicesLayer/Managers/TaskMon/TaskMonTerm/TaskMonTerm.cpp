//******************************************************************************
// Description:
//  Class used to manage the collection of panels and windows to display
//  data from TaskMon
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMonTerm/TaskMonTerm.cpp $
// 
// 4     10/26/06 11:02a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the number of thread and file descriptors associated with each
// task.
// 
// 3     4/13/06 3:59a Cward
// Update task Mon term for scolling 
//
// 3     4/05/06 11:19a Bmeinke
// Initialize the DEAD_TASKS color to white on red
//
// 2     3/20/06 12:00p Bmeinke
// Beautified
//
// 2     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 3     9/20/05 2:48p Bmeinke
// Do not reference the /net/ directory if no node name is specified on
// the command line
//
// 2     9/14/05 10:20a Bmeinke
// UpdateTaskList() now passes the task class to the TaskMonTask
// constructor
// Send TaskMon a 'refresh' message when the user presses the 'R' key
//
// 1     7/20/05 8:24p Cward
// Added new TaskMon
//
// 2     6/30/05 8:12p Bmeinke
// Changed the taskClass string for components from "Test Components" to
// "Component" to match what TaskMon uses
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

#include "TaskMonTerm.h"
#include "Bepos.h"
#include "BepDefs.h"
#include "XmlParser.h"
#include "ITaskMon.h"       // Toggle CPU/Memory monitoring

#include <stdlib.h>
#include <unistd.h>         // open()
#include <fcntl.h>          // O_RDONLY
#include <malloc.h>         // free()
#include <ctype.h>          // toupper()
#include <sys/utsname.h>    // uname()
#include <sys/dir.h>        // alphasort
#include <ncurses.h>

const char *taskClasses[] = { "Core", "Serial", "Component" };

int main( int argc, char *argv[])
{
    TaskMonTerm     taskTerm( argc, argv);
    int             retVal;

    // Read command line args
    retVal = taskTerm.Run();

    return( retVal);
}

/**
 * Constructor from command line args
 *
 * @param argc   Number of command line arguments
 * @param argv   List of command line arguments
 */
TaskMonTerm::TaskMonTerm( int argc, char *argv[]) :
m_keepDead( true), m_showCpu( true), m_nodeName( ""),
    m_loopTime( -1), m_priority( getprio(0)), m_configFile(""),
    m_useRealBox( false), m_displayMode(TASKMON_DISPLAY_CPU),
    m_activePanel(0), m_helpWindow( NULL)
{
#if 0
    // use local node name as the default node to read from
    struct utsname  localNode;
    uname( &localNode);
    m_nodeName = localNode.nodename;
#endif

    m_headerPanel = NULL;
    m_statusPanel = NULL;
    m_helpWindow = NULL;

    for( int ii=TASKMON_CLASS_CORE; ii<TASKMON_CLASS_END; ii++)
    {
        m_taskPanels[ii] = NULL;
    }

    // Initialize default colors
    m_termColors.resize( TASKSTATE_END+1);
    m_termColors[TASKSTATE_REG] = TermColor_t(COLOR_WHITE, COLOR_BLACK);
    m_termColors[TASKSTATE_RUN] = TermColor_t(COLOR_GREEN, COLOR_BLACK);
    m_termColors[TASKSTATE_STOP] = TermColor_t(COLOR_RED, COLOR_BLACK);
    m_termColors[TASKSTATE_UNREG] = TermColor_t(COLOR_WHITE, COLOR_RED);
    m_termColors[TASKSTATE_END] = TermColor_t(COLOR_BLACK, COLOR_WHITE);

    ParseCommandLine( argc, argv);
}

TaskMonTerm::~TaskMonTerm()
{
    // Cleanup our curses terminal interface
    CursesClose();
}

int TaskMonTerm::Run()
{
    struct _pulse   pulse;
    int             rcvId;
    int             chId;
    TaskMonTimer    updateTimer;
    bool            stop = false;
    int             retVal = 0;

    // Read config file options
    Initialize();

    // Make sure the desired priority is valid
    if( (m_priority < sched_get_priority_min(sched_getscheduler(0))) ||
        (m_priority > sched_get_priority_max(sched_getscheduler(0))))
    {
        m_priority = getprio(0);
    }

    // Make sure the desired loop time is valid
    if( m_loopTime < 0) m_loopTime = 250;

    // Initialize the curses terminal interface
    CursesInit();

    // Create a channel and connection for the timer pulse
    BposCreateSelfAttachment( &chId, &m_timerCoid);

    // Start the update timer
    updateTimer.Initialize(m_timerCoid, SYSTEM_PULSE, TIMER_PULSE, m_priority);
//  updateTimer.Arm( (uint32_t)m_loopTime, (uint32_t)m_loopTime);
    updateTimer.Arm( mSEC_nSEC(m_loopTime), mSEC_nSEC(m_loopTime));

    while( false == stop)
    {
        // Wait for a pulse to come in
        rcvId = MsgReceivePulse( chId, &pulse, sizeof( pulse), NULL);

        // If we did not receive a pulse
        if( rcvId != 0)
        {
            // Reply to the sender that we do not support his request
            MsgReply( rcvId, ENOSYS, NULL, 0);
        }
        else
        {
            // If this was a timer pulse
            if( (SYSTEM_PULSE == pulse.code) && (TIMER_PULSE == pulse.value.sival_int))
            {
                // Update process stats
                UpdateTaskList();

                // Update the screen
                UpdateScreen();
            }
        }

        // Handle any keyboard input and exit if 'q' was pressed
        if( 'Q' == toupper( HandleKeyboard()) )
        {
            stop = true;
        }
    }

    updateTimer.Stop();

    if( -1 != m_timerCoid)  ConnectDetach( m_timerCoid);
    if( -1 != chId)         ChannelDestroy( chId);

    return( retVal);
}

/**
 * Returns what display mode we are operating in: No Data, CPU usage or
 * memory usage
 *
 * @return Display mode we are operating in: No Data, CPU usage or
 *         memory usage
 */
TaskMonDisplayMode_t TaskMonTerm::GetDisplayMode() const
{
    return( m_displayMode);
}

/**
 * Sets what display mode we are operating in: No Data, CPU usage or
 * memory usage
 *
 * @param displayMode
 *               What display mode we are operating in: No Data, CPU usage or
 *               memory usage
 */
void TaskMonTerm::SetDisplayMode( TaskMonDisplayMode_t displayMode)
{
    m_displayMode = displayMode;
}

/**
 * Parses command line arguments
 *
 * @param argc   Number of command line arguments to parse
 * @param argv   Array of command line arguments
 */
void TaskMonTerm::ParseCommandLine( int argc, char *argv[])
{
    char    optId;
    int     ii;

    // Disable "Invalid argument" message from getopt
    opterr = 0;

    // Start reparsing from the first argument
    optind = 1;

    // Get Command line options
    for( ii=1; ii<argc; ii++)
    {
        optId = getopt( argc, argv, "bcf:l:kn:p:");
        switch( optId)
        {
        case 'b':
            m_useRealBox = (m_useRealBox == false);
            break;
        case 'c':
            // Toggles the 'keep dead' option
            m_showCpu = (m_showCpu == false);
            break;
        case 'f':
            // Set the config file name
            if( optarg != NULL)     m_configFile = optarg;
            break;
        case 'l':
            // Validate parameter value
            if( optarg != NULL)     m_loopTime = atol( optarg);
            // If invalid value, default to 1 second
            if( m_loopTime == 0)    m_loopTime = 100;
            break;
        case 'k':
            // Toggles the 'keep dead' option
            m_keepDead = (m_keepDead == false);
            break;
        case 'n':
            // Validate parameter value
            if( optarg != NULL)     m_nodeName = std::string( optarg);
            break;
        case 'p':
            // Validate parameter value
            if( optarg != NULL)     m_priority = atoi( optarg);
            break;
        default:
            break;
        }
    }
}

/**
 * Initializes our attribuutes
 */
void TaskMonTerm::Initialize()
{
    if( false == m_configFile.empty())
    {
        XmlParser       parser;
        const XmlNode   *setupNode;

        try
        {
            // Parse the config file
            setupNode = parser.ReturnXMLDocument( m_configFile)->getChild( "Setup");

            try
            {
                m_priority = BposReadInt( setupNode->getChild( "Priority")->getValue().c_str());
            }
            catch( XmlException &err)
            {
                printf( "XML Exception: %s\n", err.GetReason());
                exit( 1);
            }
            catch(...)
            {
            }

            try
            {
                if( m_loopTime < 0)
                {
                    m_loopTime = BposReadInt( setupNode->getChild( "LoopTime")->getValue().c_str());
                }
            }
            catch( XmlException &err)
            {
                printf( "XML Exception: %s\n", err.GetReason());
                exit( 1);
            }
            catch(...)
            {
            }

            try
            {
                const std::string &keepDead = setupNode->getChild( "ShowDeadProcesses")->getValue();
                m_keepDead = (stricmp( "Yes", keepDead.c_str()) == 0);
            }
            catch( XmlException &err)
            {
                printf( "XML Exception: %s\n", err.GetReason());
                exit( 1);
            }
            catch(...)
            {
            }

            try
            {
                const std::string &showCpu = setupNode->getChild( "ShowCpuUsage")->getValue();
                m_showCpu = (stricmp( "Yes", showCpu.c_str()) == 0);
            }
            catch( XmlException &err)
            {
                printf( "XML Exception: %s\n", err.GetReason());
                exit( 1);
            }
            catch(...)
            {
            }

            // Read the task state to color mapping
            InitializeColors( setupNode);
        }
        catch( XmlException &err)
        {
            printf( "XML Exception: %s\n", err.GetReason());
            exit( 1);
        }
        catch( ...)
        {
        }
    }
}

/**
 * Initializes our task state to color mappings
 *
 * @param setupNode Pointer to the config file <Setup> node
 */
void TaskMonTerm::InitializeColors( const XmlNode *setupNode)
{
    XmlNodeMapCItr  itr;
    int             colorIdx;

    if( setupNode != NULL)
    {
        try
        {
            // Get reference to the state to color mapping
            const XmlNodeMap &colors = setupNode->getChild( "Tasks/Colors")->getChildren();

            // Initialize our color pairs
            for( itr=colors.begin(); itr!=colors.end(); itr++)
            {
                TermColor_t color( COLOR_WHITE, COLOR_BLACK);

                // Convert task state name into a color index
                const XmlNode *colorNode = itr->second;
                if( colorNode->getName() == "Registered")       colorIdx = TASKSTATE_REG;
                else if( colorNode->getName() == "Running")     colorIdx = TASKSTATE_RUN;
                else if( colorNode->getName() == "Terminating") colorIdx = TASKSTATE_STOP;
                else if( colorNode->getName() == "Dead")        colorIdx = TASKSTATE_UNREG;
                else if( colorNode->getName() == "Unknown")     colorIdx = TASKSTATE_END;
                else                                            continue;

                // Convert colors from strings to color constants
                try
                {
                    const std::string &fg = colorNode->getAttribute( "foreground")->getValue();
                    if( fg == "black")          color.first = COLOR_BLACK;
                    else if( fg == "red")       color.first = COLOR_RED;
                    else if( fg == "green")     color.first = COLOR_GREEN;
                    else if( fg == "yellow")    color.first = COLOR_YELLOW;
                    else if( fg == "blue")      color.first = COLOR_BLUE;
                    else if( fg == "magenta")   color.first = COLOR_MAGENTA;
                    else if( fg == "cyan")      color.first = COLOR_CYAN;
                    else if( fg == "white")     color.first = COLOR_WHITE;

                    const std::string &bg = colorNode->getAttribute( "background")->getValue();
                    if( bg == "black")          color.second = COLOR_BLACK;
                    else if( bg == "red")       color.second = COLOR_RED;
                    else if( bg == "green")     color.second = COLOR_GREEN;
                    else if( bg == "yellow")    color.second = COLOR_YELLOW;
                    else if( bg == "blue")      color.second = COLOR_BLUE;
                    else if( bg == "magenta")   color.second = COLOR_MAGENTA;
                    else if( bg == "cyan")      color.second = COLOR_CYAN;
                    else if( bg == "white")     color.second = COLOR_WHITE;

                    m_termColors[ colorIdx] = color;
                }
                catch( ...)
                {
                }
            }
        }
        catch( ...)
        {
        }
    }
}

/**
 * Initializes the curses library (terminal control)
 */
void TaskMonTerm::CursesInit()
{
    const int rows = TaskMonPanel::GetTermRows();
    const int cols = TaskMonPanel::GetTermCols();
    const int top = 1;
    const int height = rows - top - 1;
    std::string nodeName( m_nodeName);
    int left;
    int width;

    // Save the original TTY settings
    //savetty();

    // Initialize curses screen interface
    initscr();

    // Pass control characters (from keyboard) straight through
    cbreak();

    // Do not echo typed characters locally
    noecho();

    // If the terminal haas color capabilities
    if( has_colors() == TRUE)
    {
        int     colorIdx;

        // Initialize our colors
        start_color();
        for( colorIdx=TASKSTATE_REG; colorIdx<=TASKSTATE_END; colorIdx++)
        {
            init_pair( colorIdx+1, m_termColors[colorIdx].first, m_termColors[colorIdx].second);
        }

        // Default color is white on black
        init_pair( DEFAULT_COLOR,   COLOR_WHITE,    COLOR_BLACK);
        init_pair( DEAD_TASKS,      COLOR_RED,      COLOR_WHITE);
        init_pair( CPU_YELLOW,      COLOR_WHITE,    COLOR_YELLOW);
        init_pair( CPU_MAGENTA,     COLOR_WHITE,    COLOR_MAGENTA);
        init_pair( CPU_RED,         COLOR_WHITE,    COLOR_RED);
        init_pair( HEADER_WINDOW,   COLOR_YELLOW,   COLOR_BLUE);
        init_pair( STATUS_WINDOW,   COLOR_WHITE,    COLOR_BLUE);
        init_pair( HILITE_KEY,      COLOR_RED,      COLOR_BLUE);
    }

    // use local node name as the default node to read from
    if( true == nodeName.empty())
    {
        struct utsname  localNode;
        uname( &localNode);
        nodeName = localNode.nodename;
    }
    m_headerPanel = new TaskMonHeaderPanel( nodeName, m_keepDead, m_loopTime);
    m_headerPanel->UseRealBox( m_useRealBox);

    m_statusPanel = new TaskMonStatusPanel;
    m_statusPanel->UseRealBox( m_useRealBox);

    int totalWidth = 0;
    int ii;
    for( ii=TASKMON_CLASS_CORE; ii<TASKMON_CLASS_END; ii++)
    {
        left = ii * cols / TASKMON_CLASS_END;
        width = cols / TASKMON_CLASS_END;

        // Calculate cumulative width of all panels so far
        totalWidth += width;

        // Adjust the last panel to extend to the right edge
        if( ((TASKMON_CLASS_END-1) == ii) && (totalWidth < cols))
        {
            left -= (cols - totalWidth) - 1;
            width += (cols - totalWidth);
        }

        m_taskPanels[ii] = new TaskMonTaskPanel( taskClasses[ii], height, width, top, left);
//      m_taskPanels[ii] = new TaskMonTaskPanel( taskClasses[ii], 10, width, top, left);
        m_taskPanels[ii]->UseRealBox( m_useRealBox);
    }

    // Set the first task panel active
    m_taskPanels[m_activePanel]->SetActive(true);

    // Clear stdout
    wclear( stdscr);
    wrefresh( stdscr);

    // Turn the cursor off
    curs_set( 0);
}

/**
 * Cleanup the curses library when done
 */
void TaskMonTerm::CursesClose()
{
    if( NULL != m_helpWindow)
    {
        delete m_helpWindow;
        m_helpWindow = NULL;
    }

    if( NULL != m_headerPanel)
    {
        delete m_headerPanel;
        m_headerPanel = NULL;
    }

    if( NULL != m_statusPanel)
    {
        delete m_statusPanel;
        m_statusPanel = NULL;
    }

    for( int ii=TASKMON_CLASS_CORE; ii<TASKMON_CLASS_END; ii++)
    {
        if( NULL != m_taskPanels[ ii])
        {
            delete m_taskPanels[ ii];
            m_taskPanels[ ii] = NULL;
        }
    }

    // Turn the cursor on
    curs_set( 1);

    // Turn off all attributes we may have set
    standout();

    // Turn on echoing of locally typed characters
    echo();

    endwin();

    // Restore the original TTY settings
//  resetty();
}

/**
 * Updates the screen with the currently running process lists
 */
void TaskMonTerm::UpdateTaskList()
{
    struct direct   **dirEnts;
    int             dirCount;
    uint32_t        ii, jj;
    TaskMonTask_t   newTask;
    char            dirName[_POSIX_PATH_MAX+1];

    // Read CPU load from /dev/TaskMon/TaskMon
    if( m_nodeName.empty() == false)
    {
    sprintf( dirName, "/net/%s/dev/TaskMon/", m_nodeName.c_str());
    }
    else
    {
        strcpy( dirName, "/dev/TaskMon/");
    }
    UpdateTaskStats( m_procTask, "TaskMon", dirName);

    // Get a listing of task names
    for( ii=0; ii<sizeof( taskClasses)/sizeof( taskClasses[0]); ii++)
    {
        if( m_nodeName.empty() == false)
        {
        sprintf( dirName, "/net/%s/dev/TaskMon/Tasks/%s/",
                 m_nodeName.c_str(), taskClasses[ii]);
        }
        else
        {
            sprintf( dirName, "/dev/TaskMon/Tasks/%s/", taskClasses[ii]);
        }

        // Read all entries under this CCRT name directory
        dirCount = scandir( dirName, &dirEnts, NULL, alphasort);
        // if any entries were found
        if( dirCount > 0)
        {
            for( jj=0; jj<(uint32_t)dirCount; jj++)
            {
                // Try to read task information for this task
                if( EOK == UpdateTaskStats( newTask, dirEnts[jj]->d_name, dirName))
                {
                    TaskMonTask realTask(dirEnts[jj]->d_name, newTask.m_taskPid,
                                         newTask.m_taskState, newTask.m_taskClass,
                                         newTask.m_cpuUsage, newTask.m_memoryUsage,
                                        newTask.m_threads, newTask.m_fds);
                    m_taskList[ ii].Add( realTask, TASKUPDATE_ALL);
                }
                else
                {
                    // Unable to read task stats...remove from list
                    m_taskList[ii].Remove( dirEnts[jj]->d_name);
                }

                free( dirEnts[jj]);
            }

            // Free the memory allocated by the library scandir() routine
            free( dirEnts);
        }
        // Mark all tasks as "unknown" if we cant read any info
        else
        {
            TaskListItr_t   itr;

            for( itr=m_taskList[ii].begin(); itr!=m_taskList[ii].end(); itr++)
            {
                TaskMonTask *task = itr->second;
                if( NULL != task)
                {
                    task->SetState( TASKSTATE_END);
                }
            }
        }
    }
}

/**
 * Updates the task statistics for the given task
 *
 * @param newTask  Task object to be updated
 * @param taskName Name of the task
 * @param taskDir  Directory under to which the task belongs
 *                 (i.e. /net/1/dev/TaskMon/Tasks/Core/)
 *
 * @return EOK if successful
 */
int TaskMonTerm::UpdateTaskStats( TaskMonTask_t &newTask, const std::string &taskName, const std::string &taskDir)
{
    TaskFdMapItr_t  itr;
    int             taskFd;
    std::string     taskPath( taskDir + taskName);
    int             retries = 3;
    int             retVal = ENODEV;

    // Do we already have a FD for this task?
    itr = m_taskFdMap.find( taskPath);
    // No fd yet...
    if( m_taskFdMap.end() == itr)
    {
        // Add the FD for this task
        taskFd = open( taskPath.c_str(), O_RDONLY);
        m_taskFdMap.insert( TaskFdMapVal_t(taskPath, taskFd));
    }
    // Should have an FD, but we don't
    else if( itr->second == -1)
    {
        // Get new FD
        taskFd = open( taskPath.c_str(), O_RDONLY);
        itr->second = taskFd;
    }
    else
    {
        // Get the FD for this task
        taskFd = itr->second;
    }

    // Default task state to "dead"
    newTask.m_taskState = TASKSTATE_UNREG;

    // If task FD is valid
    if( -1 != taskFd)
    {
        // Try to read task stats
        while( (retries-- > 0) && (retVal != EOK))
        {
            // Make sure we read all the information
            if( sizeof(newTask) != read( taskFd, &newTask, sizeof( newTask)))
            {
                // Bad FD
                if( errno != EOK)
                {
                    // Erase file descriptor for this task
                    m_taskFdMap.erase( taskPath);
                    // Close current FD
                    close( taskFd);
                    // Open new FD
                    taskFd = open( taskPath.c_str(), O_RDONLY);
                    // Add new FD mapping
                    m_taskFdMap.insert( TaskFdMapVal_t(taskPath, taskFd));
                }
            }
            else
            {
                retVal = EOK;
            }
        }
    }

    return( retVal);
}

/**
 * Updates the screen with the currently running process lists
 */
void TaskMonTerm::UpdateScreen()
{
    curs_set( 0);

    for( int ii=TASKMON_CLASS_CORE; ii<TASKMON_CLASS_END; ii++)
    {
        if( NULL != m_taskPanels[ ii])  m_taskPanels[ ii]->Update( m_taskList[ii], m_displayMode, m_keepDead);
    }

    if( NULL != m_headerPanel)  m_headerPanel->Update( m_procTask, m_keepDead);

    if( NULL != m_statusPanel)  m_statusPanel->Update( m_displayMode, m_keepDead);

    update_panels();

    doupdate();

    curs_set( 0);
}

/**
 * Checks to see if the user has pressed a key and processes the key press
 *
 * @param keyCode Key code for the key that was pressed
 *
 * @return true if a key was pressed, false if no key was pressed
 */
bool TaskMonTerm::CheckKeyPress( int &keyCode)
{
    int     ii;
    bool    retVal = false;

    if( (retVal == false) && (m_headerPanel != NULL))   retVal = m_headerPanel->CheckKeyPress( keyCode);
    if( (retVal == false) && (m_statusPanel != NULL))   retVal = m_statusPanel->CheckKeyPress( keyCode);
    if( (retVal == false) && (m_helpWindow != NULL))    retVal = m_helpWindow->CheckKeyPress( keyCode);

    for( ii=TASKMON_CLASS_CORE; ii<TASKMON_CLASS_END && retVal == false; ii++)
    {
        if( m_taskPanels[ ii] != NULL)
        {
            retVal = m_taskPanels[ ii]->CheckKeyPress( keyCode);
        }
    }

    return( retVal);
}

/**
 * Checks for and processes any key presses
 *
 * @return Which key was pressed or -1 if no keys were pressed
 */
int TaskMonTerm::HandleKeyboard()
{
    TaskListItr_t   taskItr;
    ITaskMon        *taskMon;
    int             keyCode = -1;
    int             classId;

    if( true == CheckKeyPress( keyCode))
    {
        // If the status window is active
        if( NULL != m_helpWindow)
        {
            delete m_helpWindow;
            m_helpWindow = NULL;
            keyCode = -1;
        }
        else
        {
            switch( keyCode)
            {
            case 0x09:  // TAB key
                m_taskPanels[m_activePanel]->SetActive(false);
                m_activePanel = (m_activePanel+1) % TASKMON_CLASS_END;
                m_taskPanels[m_activePanel]->SetActive(true);
                break;
            case KEY_DOWN:
                m_taskPanels[m_activePanel]->Scroll( 1);
                break;
            case KEY_UP:
                m_taskPanels[m_activePanel]->Scroll( -1);
                break;
            case 'k':   case 'K':
                m_keepDead = (m_keepDead == false);
                break;
            case 'q':   case 'Q':
                break;
            case 'r':   case 'R':
                    // Tell TaskMon to refresh his task list
                    taskMon = new ITaskMon;
                    taskMon->RefreshTaskList();
                    delete taskMon;
                    delay(500);

                    // Remove all our task entries (the listwill get refreshed when we
                    //  handle the next update pulse)
                for( classId=TASKMON_CLASS_CORE; classId<TASKMON_CLASS_END; classId++)
                {
                    for( taskItr=m_taskList[classId].begin(); taskItr!=m_taskList[classId].end(); taskItr++)
                    {
                        TaskMonTask *currTask = taskItr->second;
                        if( NULL != currTask)   delete currTask;
                    }
                        m_taskList[classId].clear();
                    }
                break;
            case 'c':   case 'C':
                if(TASKMON_DISPLAY_CPU != m_displayMode)
                {
                    m_displayMode = TASKMON_DISPLAY_CPU;
                }
                break;
            case 'm':   case 'M':
                if(TASKMON_DISPLAY_MEM != m_displayMode)
                {
                    m_displayMode = TASKMON_DISPLAY_MEM;
                }
                break;
            case 't':   case 'T':
                if(TASKMON_DISPLAY_THREADS != m_displayMode)
                {
                    m_displayMode = TASKMON_DISPLAY_THREADS;
                }
                break;
            case 'f':   case 'F':
                if(TASKMON_DISPLAY_FDS != m_displayMode)
                {
                    m_displayMode = TASKMON_DISPLAY_FDS;
                }
                break;
            case KEY_F(1):
                m_helpWindow = new TaskMonHelpWindow;
                m_helpWindow->UseRealBox( m_useRealBox);
                m_helpWindow->Update();
                break;
            case KEY_F(2):
                taskMon = new ITaskMon;
                taskMon->ToggleMemoryMonitoring();
                delete taskMon;
                break;
            default:
                keyCode = -1;
                break;
            }
        }
    }
    else
    {
        keyCode = -1;
    }

    // If a key was pressed
    if( -1 != keyCode)
    {
        // Send us a pulse to cause an immediate screen update
        MsgSendPulse( m_timerCoid, m_priority, SYSTEM_PULSE, TIMER_PULSE);
    }

    return( keyCode);
}

/**
 * Removes dead processes from the screen
 */
void TaskMonTerm::RemoveDeadProcesses()
{
    TaskListItr_t   itr;
    int             classId;

    for( classId=TASKMON_CLASS_CORE; classId<TASKMON_CLASS_END; classId++)
    {
        itr=m_taskList[classId].begin();
        while( itr != m_taskList[classId].end())
        {
            TaskMonTask *currTask = itr->second;
            if( (TASKSTATE_UNREG == currTask->GetState()) ||
                (TASKSTATE_END == currTask->GetState()))
            {
                itr = m_taskList[classId].erase( itr);
                delete currTask;
            }
            else
            {
                itr++;
            }
        }
    }
}

/*******************************************************************************
    Usage Block
    Information printed when a user types 'use xxxxx' at the QNX prompt
#ifdef __USAGE

Task Monitor Terminal
   Press the F1 key to see online help

%C [-b] [-c] [-f config_file] [-k] [-l period] [-n nodeName] [-p priority]

    -b    Draw boxes using the 'real' line drawing characters
    -c    Show CPU usage for each process
    -f    Read configuration from XML config file
    -k    Keep dead processes on the screen (use 'r' to refresh)
    -l    Loop at the specified period (in ms, default=100ms)
    -n    Node to monitor process on
    -p    Run at the specified priority

#endif
*******************************************************************************/

