using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace Logger
{
    public class CcrtLogger
    {
        /* ============================================================================================================
         * Member Functions
         * ============================================================================================================
         */

        /// <summary>
        /// Create a new logger item.
        /// A new log file will be opened with the current date in the file name.
        /// </summary>
        /// <param name="logFilePath">Directory path for the log file.</param>
        /// <param name="processName">Name of the process that created this logger.</param>
        /// <param name="logBox">Text box to use for displaying logging information.</param>
        /// <param name="logFileCount">Maximum number of log files to keep.</param>
        /// <param name="maxLoggerWinSize">The maximum number of bytes that the Log Textbox will contain (50000 is good place to start if unknown)</param>
        public CcrtLogger(String logFilePath, String processName, TextBox logBox, Int32 logFileCount, Int32 maxLoggerWinSize)
        {
            InitializeLogger(logFilePath, processName, logBox, logFileCount, maxLoggerWinSize);
        }

        /// <summary>
        /// Create a new logger item.
        /// A new log file will be opened with the current date in the file name.
        /// </summary>
        /// <param name="logFilePath">Directory path for the log file.</param>
        /// <param name="processName">Name of the process that created this logger.</param>
        /// <param name="logBox">Text box to use for displaying logging information.</param>
        /// <param name="logFileCount">Maximum number of log files to keep.</param>
        public CcrtLogger(String logFilePath, String processName, TextBox logBox, Int32 logFileCount)
        {
            InitializeLogger(logFilePath, processName, logBox, logFileCount, 50000);
        }
        /// <summary>
        /// Create a new logger item.
        /// A new log file will be opened with the current date in the file name.
        /// </summary>
        /// <param name="logFilePath">Directory path for the log file.</param>
        /// <param name="processName">Name of the process that created this logger.</param>
        /// <param name="logBox">Text box to use for displaying logging information.</param>
        public CcrtLogger(String logFilePath, String processName, TextBox logBox)
        {   // Create a mutex to protect access to the log file

            InitializeLogger(logFilePath, processName, logBox, 10, 50000);

        }

        private void InitializeLogger(String logFilePath, String processName, TextBox logBox, Int32 logFileCount, Int32 maxLoggerWinSize)
        {
            // Create a mutex to protect access to the log file
            LogMutex = new Mutex();
            LogMessageQueue = new Queue<String>();
            // Save the log text box and logging ID info
            LogBox = logBox;
            m_maxTextBoxSize = maxLoggerWinSize;
            ProcessName = processName;
            if (logFilePath == null)
            {
                logFilePath = SelectLogFilePath();
            }
            else if (logFilePath.Length == 0)
            {
                logFilePath = SelectLogFilePath();
            }
            LogDirectory = logFilePath;
            LogFileCount = logFileCount;
            // Create or open the logging file
            LogFileName = CreateLogFileName();

            MessageLogger = new Thread(WriteLogMessagesToFile);
            MessageLogger.Start();
            // Indicate the start of the logging session
            Log("------------------------------------------------------------------------------------------");
            Log(ProcessName + " application started");
        }

        /// <summary>
        /// Close out the log file.
        /// </summary>
        public void CloseLogFile()
        {
            Log(ProcessName + " application closing");
            MessageLogger.Abort();
        }

        /// <summary>
        /// Create a log file name based on date and process name.
        /// </summary>
        /// <param name="processName">Name of the process that created the logger.</param>
        private String CreateLogFileName()
        {
            String fileName = ProcessName + ".Log." + DateTime.Now.ToString("yyyyMMdd") + ".txt";
            return LogDirectory + "\\" + fileName;
        }

        /// <summary>
        /// Display the log message in the window for the user.
        /// </summary>
        /// <param name="message">Log message to be displayed.</param>
        private void DisplayLogMessage(String message, Boolean invokeRequired = false)
        {
            try
            {
                if (LogBox.InvokeRequired || invokeRequired)
                {   // Create a new callback to prevent thread issues
                    SetTextCallback callback = new SetTextCallback(DisplayLogMessage);
                    LogBox.Invoke(callback, new object[] { message, false });
                }
                else
                {   // Just add the message to the text box
                    LogBox.AppendText(message + Environment.NewLine);
                    
                    if (LogBox.TextLength > m_maxTextBoxSize)
                    {
                        // Overwrite everything in the text box with about 10% (1 - .1 = .9) of the original data.
                        LogBox.Text = LogBox.Text.Substring(LogBox.Text.IndexOf("\r\n", Convert.ToInt32(m_maxTextBoxSize * 0.9)), 
                                            (LogBox.Text.Length - LogBox.Text.IndexOf("\r\n", Convert.ToInt32(m_maxTextBoxSize * 0.9))));
                        
                    }
                }
            }
            catch (InvalidOperationException)
            {   // Nothing to do, log box may not have been instantiated yet.
            }
        }

        /// <summary>
        /// Log the provided message to the log file and to the display.
        /// The message will be wrapped with the current date and time and then placed into the log file and the
        /// log text box on the display.  If this is the first log message of the day, the software will start using a 
        /// new log file.
        /// </summary>
        /// <param name="message">Message to be logged.</param>
        public void Log(String message, String processName = null)
        {   // Make sure we are the only thread accessing the log queue
            if ((LogMutex != null) && (LogMessageQueue != null))
            {
                try
                {
                    LogMutex.WaitOne();
                    String msg = "";

                    // Wrap the log message
                    if (message.Length > 0)
                    {
                        msg = DateTime.Now.ToString("MMM dd, yyyy HH:mm:ss.fff") + "\t" +
                                     (processName != null ? processName : ProcessName) + "\t" + message;
                    }

                    LogMessageQueue.Enqueue(msg);
                }
                catch (AbandonedMutexException excpt)
                {
                    DisplayLogMessage("Oops! - " + excpt, false);
                }
                catch (NullReferenceException nullExcpt)
                {
                    MessageBox.Show("Message: " + nullExcpt.Message + Environment.NewLine +
                                    "Source:  " + nullExcpt.Source + Environment.NewLine +
                                    "Stack:   " + nullExcpt.StackTrace, "Logger ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                finally
                {
                    LogMutex.ReleaseMutex();
                }
            }
            else
            {
                if (LogMutex == null)
                    MessageBox.Show("LogMutex is null", "Null Reference Detected", MessageBoxButtons.OK, MessageBoxIcon.Error);
                if (LogMessageQueue == null)
                    MessageBox.Show("LogMessageQueue is null", "Null Reference Detected", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Write the logged messages out to file.
        /// </summary>
        public void WriteLogMessagesToFile()
        {   // Wait for everything to stabilize
            Thread.Sleep(1000);
            while (true)
            {
                Queue<String> tempQ = new Queue<String>();
                try
                {   // Copy the queue contents to a temporary queue so we can quickly release the 
                    // main queue so as to not hold any other process up while we dump to file
                    LogMutex.WaitOne();
                    foreach (String msg in LogMessageQueue)
                    {
                        tempQ.Enqueue(msg);
                    }
                    LogMessageQueue.Clear();
                }
                catch (AbandonedMutexException excpt)
                {
                    DisplayLogMessage("Oops! - " + excpt, true);
                }
                finally
                {
                    LogMutex.ReleaseMutex();
                }
                // Make sure there is something to send to file
                if (tempQ.Count > 0)
                {   // Determine if we need to create a new log file name
                    String fileName = CreateLogFileName();
                    if (fileName != LogFileName)
                    {   // Update the log file name
                        String tempLogName = (LogFileName == null) ? fileName : LogFileName;
                        using (StreamWriter writer = new StreamWriter(tempLogName, true))
                        {
                            writer.WriteLine("Rolling over log file");
                        }
                        LogFileName = fileName;
                        // Maintain the log files
                        MaintainLogFiles();
                    }
                    try
                    {
                        // Write the log message to the file
                        using (StreamWriter writer = new StreamWriter(LogFileName, true))
                        {
                            foreach (String msg in tempQ)
                            {   // Write the message to the log file
                                writer.WriteLine(msg);
                                // Display the log message for the user
                                DisplayLogMessage(msg, true);
                            }
                        }
                    }
                    catch (DirectoryNotFoundException e)
                    { //Directory file was not found
                        DisplayLogMessage("Error::Logger - " + e.Message);
                    }
                }
                else
                {
                    Thread.Sleep(1000);
                }
            }
        }

        /// <summary>
        /// Get the list of log files currently in the system.
        /// </summary>
        /// <returns>List of current log files for this process.</returns>
        public List<FileInfo> LogFileListing()
        {   // Get a list of all log files currently in the system
            String filter = ProcessName + ".Log.*.txt";
            List<FileInfo> logFiles = new List<FileInfo>();
            if (LogDirectory != null)
            {
                DirectoryInfo dir = new DirectoryInfo(LogDirectory);
                foreach (FileInfo file in dir.GetFiles(filter))
                {
                    logFiles.Add(file);
                }
                // Make sure the list is sorted
                logFiles.Sort(OrderLogFilesByDate);
            }
            return logFiles;
        }

        /// <summary>
        /// Maintain the logfiles.  Only a specified number of log files will be kept in order 
        /// to prevent filling up the system.
        /// </summary>
        private void MaintainLogFiles()
        {   // Get the current list of log files
            List<FileInfo> fileList = LogFileListing();
            if (fileList.Count > LogFileCount)
            {   // more than enough log files, get rid of the older log files
                for (int index = LogFileCount; index < fileList.Count; index++)
                {
                    File.Delete(fileList.ToArray()[index].Name);
                    Log("Removed old log file: " + fileList.ToArray()[index].Name);
                    fileList.Remove(fileList.ToArray()[index]);
                }
            }
        }

        /// <summary>
        /// Compare two files by date to determine which file is newer.
        /// </summary>
        /// <param name="file1">File to be compared.</param>
        /// <param name="file2">File to be compared.</param>
        /// <returns>Ordering information:
        ///          -1: file2 is newer than file1.
        ///           0: file1 and file2 are the same age.
        ///           1: file1 is newer than file2.
        /// </returns>
        private int OrderLogFilesByDate(FileInfo file1, FileInfo file2)
        {
            int status = 0;   // Assume they are the same
            // Check if file1 is null
            if (file1 == null)
            {   // File1 is null, check if file2 is null
                if (file2 == null)
                {   // Both files are null so they are the equal.
                    status = 0;
                }
                else
                {   // file2 is not null, so it is newer
                    status = -1;
                }
            }
            else
            {   // file1 is not null, check if file2 is null
                if (file2 == null)
                {   // file2 is null, so file1 is newer
                    status = 1;
                }
                else
                {   // Two non-null files, compare creation dates
                    status = file1.CreationTime.CompareTo(file2.CreationTime);
                }
            }
            return status;
        }

        /// <summary>
        /// Allow the user to select the directory for the log files.
        /// </summary>
        /// <returns>Path for the log file directory.</returns>
        public String SelectLogFilePath()
        {
            String logDir = LogDirectory;
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select log file folder:";
            dlg.SelectedPath = LogDirectory;
            dlg.ShowNewFolderButton = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {   // User selected a new log file directory
                UpdateLogFilePath(dlg.SelectedPath);
                logDir = dlg.SelectedPath;
            }
            return logDir;
        }

        /// <summary>
        /// Update the number of log files to keep.
        /// </summary>
        /// <param name="maxLogFiles">Maximum number of log files to keep.</param>
        public void UpdateLogFileCount(Int32 maxLogFiles)
        {
            Log("Updating max log files from " + LogFileCount.ToString() + " to " + maxLogFiles.ToString());
            LogFileCount = maxLogFiles;
        }

        /// <summary>
        /// Set the new path of the log file.
        /// </summary>
        /// <param name="newPath">New log file directory path.</param>
        private void UpdateLogFilePath(String newPath)
        {
            String orgDir = LogDirectory;
            LogDirectory = newPath;
            Log("Updating log file path from " + orgDir + " to " + newPath);
        }

        /// <summary>
        /// Display the current log file.
        /// </summary>
        public void ViewCurrentLogFile()
        {
            LogViewerForm frm = new LogViewerForm(LogFileName);
            frm.Show();
        }

        /// <summary>
        /// View previous log files.
        /// Open a dialog for the user to select which log files to view.
        /// </summary>
        public void ViewPreviousLogFile()
        {   // Display a dialog to allow the user to select previous log files to display
            LogFileChooserDialog dlg = new LogFileChooserDialog(LogFileListing());
            DialogResult rslt = dlg.ShowDialog();
            if (rslt == DialogResult.OK)
            {   // Get the list of log files
                List<String> logFiles = dlg.SelectedLogFiles;
                List<LogViewerForm> logViewers = new List<LogViewerForm>();
                // Open each log file
                foreach (String log in logFiles)
                {
                    LogViewerForm viewer = new LogViewerForm(LogDirectory + "\\" + log);
                    viewer.Show();
                    logViewers.Add(new LogViewerForm(LogDirectory + "\\" + log));
                }
            }
        }



        /* ============================================================================================================
         * Delegates and Miscellaneous Items
         * ============================================================================================================
         */
        delegate void SetTextCallback(String text, Boolean invokeRequired);



        /* ============================================================================================================
         * Member Variables / Properties
         * ============================================================================================================
         */

        /// <summary>
        /// Log box to use for displaying logging messages.
        /// </summary>
        private TextBox m_logBox;

        private Int32 m_maxTextBoxSize;

        /// <summary>
        /// Get/Set the logging text box to be used for displaying logging messages.
        /// </summary>
        private TextBox LogBox
        {
            get { return m_logBox; }
            set { m_logBox = value; }
        }

        /// <summary>
        /// Number of log files to keep.
        /// </summary>
        private Int32 m_logFileCount;

        /// <summary>
        /// Get/Set the number of log files to keep.
        /// </summary>
        private Int32 LogFileCount
        {
            get { return m_logFileCount; }
            set { m_logFileCount = value; }
        }

        /// <summary>
        /// Name of the current log file.
        /// </summary>
        private String m_logFileName;

        /// <summary>
        /// Get/Set the name of the current log file.
        /// </summary>
        private String LogFileName
        {
            get { return m_logFileName; }
            set { m_logFileName = value; }
        }

        /// <summary>
        /// Path for the log file.
        /// </summary>
        private String m_logFilePath;

        /// <summary>
        /// Get/Set the directory for the log files.
        /// </summary>
        private String LogDirectory
        {
            get { return m_logFilePath; }
            set { m_logFilePath = value; }
        }

        /// <summary>
        /// Name of the process that created the logger.
        /// </summary>
        private String m_processName;

        /// <summary>
        /// Get/Set the name of the process that created the logger.
        /// </summary>
        private String ProcessName
        {
            get { return m_processName; }
            set { m_processName = value; }
        }

        /// <summary>
        /// Queue to use for placing messages into when a process calls the Log function.
        /// </summary>
        private Queue<String> LogMessageQueue { get; set; }

        /// <summary>
        /// Thread to use for writing the logged messages out to file.
        /// </summary>
        private Thread MessageLogger { get; set; }

        /// <summary>
        /// Mutex variable to make sure only one thread accesses the log file at a time.
        /// </summary>
        private static Mutex LogMutex { get; set; }
    }
}
