using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;

namespace BomFileProcessor
{
    public class Logger
    {
        /// <summary>
        /// Create a new logger and open the log file.
        /// </summary>
        public Logger(System.Windows.Forms.RichTextBox messageBox)
        {   // Open a new log file with the current date
            m_logMessageWindow = messageBox;
            CurrentLogFileName = BomFileProcessor.Properties.Resources.LogFileBase + DateTime.Today.Year + DateTime.Today.Month +
                                 DateTime.Today.Day + BomFileProcessor.Properties.Resources.LogFileExtension;
            // Maintain the log files
            Log(Environment.NewLine + Environment.NewLine + "------------------------------------------------------" +
                Environment.NewLine + "BOM file processor application started");
            MaintainLogFiles();
        }

        /// <summary>
        /// Write a message to the log file.  The message will be time stamped.
        /// </summary>
        /// <param name="message">Message to write to the log file.</param>
        public void Log(String message)
        {   // Write the log message to the log file
            lock (this)
            {
                using (StreamWriter writer = new StreamWriter(CurrentLogFileName, true))
                {
                    writer.WriteLine(DateTime.Now + "\t" + message);
                }
                // Display the message on the screen
                LogMessage(DateTime.Now + "\t" + message + Environment.NewLine);
            }
        }

        /// <summary>
        /// Scrolls the textbox to the end
        /// </summary>
        /// <param name="tb"></param>
        private void ScrollTextBoxEnd(System.Windows.Forms.RichTextBox tb)
        {
            const int WM_VSCROLL = 277;
            const int SB_BOTTOM = 7;

            IntPtr ptrWparam = new IntPtr(SB_BOTTOM);
            IntPtr ptrLparam = new IntPtr(0);
            SendMessage(tb.Handle, WM_VSCROLL, ptrWparam, ptrLparam);
        }

        [DllImport("User32.dll", CharSet = CharSet.Auto, EntryPoint = "SendMessage")]
        static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam,
        IntPtr lParam);

        /// <summary>
        /// Display a log message on the screen.
        /// </summary>
        /// <param name="message">Message to display on the screen.</param>
        delegate void LogMessageCallback(String message);
        public void LogMessage(String message)
        {
            try
            {
                if (m_logMessageWindow.InvokeRequired)
                {   // Create a new callback to prevent thread issues
                    LogMessageCallback callback = new LogMessageCallback(LogMessage);
                    m_logMessageWindow.Invoke(callback, new object[] { message });
                }
                else
                {   // Just add the message to the text box.
                    //                int insertLocation = m_logMessageWindow.Rtf.Length;
                    m_logMessageWindow.AppendText(message);
                    //scroll to bottom of window
                    ScrollTextBoxEnd(m_logMessageWindow);
                    /*
                                    String rtf = m_logMessageWindow.Rtf;
                                    m_logMessageWindow.Clear();
                                    // Change the color for all Error lines
                                    int errorIndex = rtf.IndexOf("INFO", insertLocation);
                                    if (errorIndex != -1)
                                    {   // Set error text to red
                                        rtf = rtf.Insert(errorIndex, "\\cf1");
                                        // Reset to black at end of line
                                        rtf = rtf.Insert(insertLocation + message.Length + 4, "\\cf4");
                                    }
                                    m_logMessageWindow.Rtf = rtf;
                    */
                }
            }
            catch (ObjectDisposedException)
            {
            }
        }

        /// <summary>
        /// Close the log file.
        /// </summary>
        public void CloseLogFile()
        {
            Log("Build item override application exiting");
        }

        /// <summary>
        /// Get the list of log files currently in the system.
        /// </summary>
        /// <returns></returns>
        public List<FileInfo> LogFileListing()
        {   // Get a list of all log files currently in the system
            String filter = BomFileProcessor.Properties.Resources.LogFileBase + "*" + BomFileProcessor.Properties.Resources.LogFileExtension;
            DirectoryInfo dir = new DirectoryInfo(Environment.CurrentDirectory);
            List<FileInfo> logFiles = new List<FileInfo>();
            foreach (FileInfo file in dir.GetFiles(filter))
            {
                logFiles.Add(file);
            }
            // Make sure the list is sorted
            logFiles.Sort(OrderLogFilesByDate);
            return logFiles;
        }

        /// <summary>
        /// Get/Set the name of the current log file being used.
        /// </summary>
        public String CurrentLogFileName
        {
            get { return m_logFileName; }
            set { m_logFileName = value; }
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
        /// Maintain the logfiles.  Only 10 log files will be kept in order to prevent filling up the system.
        /// </summary>
        private void MaintainLogFiles()
        {   // Get the current list of log files
            List<FileInfo> fileList = LogFileListing();
            if (fileList.Count > 10)
            {   // more than 10 log files, get rid of the older log files
                try
                {
                    for (int index = 10; index < fileList.Count; index++)
                    {
                        File.Delete(fileList.ToArray()[index].Name);
                        Log("Removed old log file: " + fileList.ToArray()[index].Name);
                        // No need to remove from the list since the list is temprary.
                        // Plus removing from the list changes the size of the list and leads to exceptions
                        // and other bad things.
                        //                    fileList.Remove(fileList.ToArray()[index]);
                    }
                }
                catch (IndexOutOfRangeException)
                {   // Nothing special to do
                }
            }
        }

        String m_logFileName;
        System.Windows.Forms.RichTextBox m_logMessageWindow;
    }
}
