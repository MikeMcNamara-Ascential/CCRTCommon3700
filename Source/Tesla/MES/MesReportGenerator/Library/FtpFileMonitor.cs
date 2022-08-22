using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
using LoggingInterface;
using System.Net;
using System.Globalization;
using System.Text.RegularExpressions;
using AlexPilotti.FTPS.Client;
using AlexPilotti.FTPS.Common;

namespace FtpFileMonitorNamespace
{
    public class FtpFileMonitor
    {
        protected string m_remoteLocation;
        protected string m_localLocation;
        protected string m_tempLocation;
        protected int m_fileCheckInterval;
        protected string m_fileMask;
        protected Logger m_logger;
        protected volatile bool m_terminate;
        protected object m_terminateLock;
        Thread m_fileMonitorThread;
        protected int m_noESNFilesTransferedCount;
        public bool m_esnFault;

        protected string m_ftpUserLogin;
        protected string m_ftpUserPassword;
        protected string m_ftpServerIp;

        public object DirectoryLock;


        protected List<string> m_remoteLocations;
        protected List<string> m_ftpUserLogins;
        protected List<string> m_ftpUserPasswords;
        protected List<string> m_ftpServerIps;


        public FtpFileMonitor(string remoteLocation, string localLocation, string tempLocalLocation, int fileCheckInterval,
            string userLogin, string password, string ftpServerIp, string fileMask = "*", Logger logger = null)
        {
            DirectoryLock = new object();
            m_remoteLocation = "/home/" + userLogin + remoteLocation;
            m_localLocation = localLocation;
            m_tempLocation = tempLocalLocation;
            m_fileCheckInterval = fileCheckInterval;
            m_logger = logger;
            m_ftpUserLogin = userLogin;
            m_ftpUserPassword = password;
            m_ftpServerIp = ftpServerIp;
            m_terminate = false;
            m_terminateLock = new object();
            m_fileMask = fileMask;
            m_noESNFilesTransferedCount = 0;
            m_esnFault = false;
        }

        /// <summary>
        /// ftp muliple locations (upload only)
        /// </summary>
        /// <param name="remoteLocation"></param>
        /// <param name="localLocation"></param>
        /// <param name="tempLocalLocation"></param>
        /// <param name="fileCheckInterval"></param>
        /// <param name="userLogin"></param>
        /// <param name="password"></param>
        /// <param name="ftpServerIp"></param>
        /// <param name="logger"></param>
        public FtpFileMonitor(List<string> remoteLocations, string localLocation, List<string> userLogins, 
            List<string> passwords, List<string> ftpServerIps, string fileMask = "*", Logger logger = null)
        {
            DirectoryLock = new object();
            m_remoteLocations = new List<string>(remoteLocations);
            m_localLocation = localLocation;
            m_logger = logger;
            m_ftpUserLogins = new List<string>(userLogins);
            m_ftpUserPasswords = new List<string>(passwords);
            m_ftpServerIps = new List<string>(ftpServerIps);
            m_terminate = false;
            m_terminateLock = new object();
            m_fileMask = fileMask;
            m_noESNFilesTransferedCount = 0;
            m_esnFault = false;
        }

        public void StartFileMonitorThread()
        {
            ThreadStart taskDelegate = null;
            taskDelegate = new ThreadStart(FileChangeMonitorFtpLocation);
            m_fileMonitorThread = new Thread(taskDelegate);

            m_fileMonitorThread.Start();
        }
        public void StopFileMonitorThread()
        {
            lock (m_terminateLock)
            {
                m_terminate = true;
            }
        }
        public bool TerminateCalled()
        {
            lock (m_terminateLock)
            {
                return m_terminate;
            }
        }



        /// <summary>
        /// Monitor for files in source directory on ftp server
        /// if files are present, transfer to local temp directory
        /// copy into final target directory and delete remote files which 
        /// were transfered successfully
        /// </summary>
        public void FileChangeMonitorFtpLocation()
        {
            string uri = m_remoteLocation;
            string escapedUri = uri.Replace("../", "%2E%2E/");


            while (!TerminateCalled())
            {
                try
                {
                    if (Directory.Exists(m_localLocation) && Directory.Exists(m_tempLocation))
                    {//both directories exist check num files
                        using (FTPSClient client = new FTPSClient())
                        {
                            client.Connect(m_ftpServerIp, 2121, new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword),
                                           ESSLSupportMode.ClearText, null, null, 0, 0, 0, null, true, EDataConnectionMode.Active);
                            IList<string> fileList = new List<string>();
                            try
                            {
                                fileList = client.GetShortDirectoryList(escapedUri);
                            }
                            catch (FTPCommandException e)
                            {
                                if (e.Message != "No files found.")
                                    Log("ERROR:    FileMonitor::Error during File update Process " + e.ToString());
                            }
                            DirectoryInfo localInfo = new DirectoryInfo(m_localLocation);
                            if (fileList.Count() > 0)
                            {
                                Log("INFO:    Files to transfer found, updating " + m_localLocation);
                                UpdateLocalDirectoryRemoteFtpLocation(client, fileList, localInfo);
                                m_esnFault = false;
                                m_noESNFilesTransferedCount = 0;
                            }
                            else
                            {
                                m_noESNFilesTransferedCount++;
                                if (m_noESNFilesTransferedCount >= 90)
                                    m_esnFault = true;
                            }
                            client.Close();
                        }
                    }
                    else
                    {
                        Log("ERROR:    FileMonitor::Temp or Target directory does not exist temp: " +
                            m_tempLocation + " Target: " + m_localLocation);
                        //m_terminate = true;
                    }
                }
                catch (Exception ex)
                {//Error remote dir gone before transfer complete
                    Log("ERROR:    FileMonitor::Error during File update Process " + ex.ToString());
                }
                Thread.Sleep(m_fileCheckInterval);
            }
        }

        public List<string> UpdateLocalDirectoryRemoteFtpLocation(FTPSClient client, IList<string> remoteFiles, DirectoryInfo localInfo)
        {
            List<string> filesTransfered = new List<string>();

                try
                {//delete current temp files
                    DirectoryInfo tempInfo = new DirectoryInfo(m_tempLocation);
                    foreach (FileInfo fi in tempInfo.GetFiles())
                    {
                        if (FitsMask(fi.Name, m_fileMask))
                        {
                            fi.Delete();
                        }
                    }
                    //get all files
                    foreach (string fi in remoteFiles)
                    {//copy to temp folder first in case remote directory goes down during transfer
                        if (!TransferFileFromFtpLocation(client, fi.Substring(fi.LastIndexOf('/') + 1)))
                        {
                            break;
                        }
                    }

                    List<FileInfo> matchingFiles = new List<FileInfo>();
                    List<FileInfo> temp = tempInfo.GetFiles().ToList();
                    matchingFiles = temp.FindAll(x => FitsMask(x.Name, m_fileMask));
                    int fileCount = matchingFiles.Count();
                    int currentFile = 1;
                    //place temp files in final directory overwriting if neccessary
                    //lock to coordinate access to directory
                    lock (DirectoryLock)
                    {
                        foreach (FileInfo fi in matchingFiles)
                        {
                            Log("FTP File Info:");
                            Log("    Name: " + fi.Name.ToString());
                            Log("    Size: " + fi.Length.ToString());
                            fi.CopyTo(Path.Combine(localInfo.ToString(), fi.Name), true);
                            DeleteFileFromFtpLocation(client, m_remoteLocation, fi.Name, currentFile == fileCount);
                            currentFile++;
                        }
                    }
                }
                catch (Exception e)
                {//Error remote dir gone before transfer complete
                    Log("ERROR:    FileMonitor::Error during File update Process, check remote pc active " + e.ToString());
                }
            
            return filesTransfered;
        }

        public void DeleteFileFromFtpLocation(FTPSClient client, string location, string fileName, bool isFinal)
        {
            try
            {
                string uri = location + "/" + fileName;
                string escapedUri = uri.Replace("../", "%2E%2E/");
                client.DeleteFile(escapedUri);
            }
            catch (Exception ex)
            {
                Log("ERROR:  Could not delete remote copy of file " + fileName + ": " + ex.ToString());
            }
        }

        public bool TransferFileFromFtpLocation(FTPSClient client, string fileName)
        {

            bool success = true;
            string uri = m_remoteLocation;
            string escapedUri = uri.Replace("../", "%2E%2E/");

            try
            {
                // Download a file
                ulong bytesCopied = client.GetFile(escapedUri + fileName, m_tempLocation + fileName);
                if (bytesCopied <= 0)
                {
                    success = false;
                    Log("ERROR:  Could not copy remote file " + fileName + "bytes copied:" + bytesCopied.ToString());
                }

            }
            catch (Exception ex)
            {
                success = false;
                Log("ERROR:  Could not copy remote file " + fileName + ": " + ex.ToString());
            }


            return success;
        }

        public class FtpFileInfo
        {
            public FtpFileInfo(string value)
            {
                var tokens = value.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                DateCreated = DateTime.ParseExact(tokens[4] + tokens[5]+tokens[6], "MMMddHH:mm", CultureInfo.InvariantCulture);
                Size = int.Parse(tokens[3]);
                Name = tokens[7];
            }
            public DateTime DateCreated;
            public int Size;
            public string Name;
        }

        public virtual void Log(string message)
        {
            m_logger.Log(message);

        }

        /// <summary>
        /// Tries to open a file, with a user defined number of attempt and Sleep delay between attempts.
        /// </summary>
        /// <param name="filePath">The full file path to be opened</param>
        /// <param name="maximumAttempts">The total number of attempts to make (multiply by attemptWaitMS for the maximum time the function with Try opening the file)</param>
        /// <param name="attemptWaitMS">The delay in Milliseconds between each attempt.</param>
        /// <returns>A valid FileStream object for the opened file, or null if the File could not be opened after the required attempts</returns>
        public FileStream TryOpen(FileInfo fileInfo, int maximumAttempts, int attemptWaitMS)
        {
            FileStream fs = null;
            int attempts = 0;

            // Loop allow multiple attempts
            while (!TerminateCalled())
            {
                try
                {
                    fs = fileInfo.OpenRead();
                    //If we get here, the File.Open succeeded, so break out of the loop and return the FileStream
                    break;
                }
                catch (IOException)
                {
                    // IOException is thrown if the file is in use by another process.
                    //Log("INFO:  Waiting for file Transfer completion # " + attempts.ToString());
                    // Check the numbere of attempts to ensure no infinite loop
                    attempts++;
                    if (attempts > maximumAttempts)
                    {
                        // Too many attempts,cannot Open File, break and return null 
                        fs = null;
                        break;
                    }
                    else
                    {
                        // Sleep before making another attempt
                        System.Threading.Thread.Sleep(attemptWaitMS);
                    }
                }
            }
            // Return the stream, may be valid or null
            return fs;
        }

        private bool FitsMask(string fileName, string fileMask)
        {
            string pattern =
                 '^' +
                 Regex.Escape(fileMask.Replace(".", "__DOT__")
                                 .Replace("*", "__STAR__")
                                 .Replace("?", "__QM__"))
                     .Replace("__DOT__", "[.]")
                     .Replace("__STAR__", ".*")
                     .Replace("__QM__", ".")
                 + '$';
            return new Regex(pattern, RegexOptions.IgnoreCase).IsMatch(fileName);
        }

        public bool UploadToSource(List<string> files)
        {
            int fileNumber = 0;
            bool overAllSuccess = true;
            foreach (string fileName in files)
            {
                if (overAllSuccess)
                {
                    bool success = true;
                    //Send file to server via FTP
                    string uri = m_remoteLocation + "/" + fileName.Substring(fileName.LastIndexOf('\\')+1);
                    string escapedUri = uri.Replace("../", "%2E%2E/");
                    fileNumber++;

                    using (FTPSClient client = new FTPSClient())
                    {
                        try
                        {   // Connect to the server with no encryption
                            client.Connect(m_ftpServerIp, 2121,new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword),
                                           ESSLSupportMode.ClearText, null, null, 0, 0, 0, null, true, EDataConnectionMode.Active);
                            
                            ulong bytesTransfered = client.PutFile(fileName, escapedUri);

                            if (bytesTransfered <= 0)
                            {
                                success = false;
                                Log("ERROR:  Could not upload file to remote location " + fileName + "bytes copied:" + bytesTransfered.ToString());
                            }
                            else
                            {
                                Log("INFO:  " + fileName.Substring(fileName.LastIndexOf('\\') + 1) + " transfered successfully");
                                //Delete the local file
                                try
                                {
                                    System.IO.File.Delete(fileName);
                                }
                                catch (Exception e)
                                {
                                    Log("ERROR:  Could not delete original file: " + e.ToString());
                                }
                            }

                            client.Close();
                        }
                        catch (Exception ex)
                        {
                            success = false;
                            Log("ERROR:  Could not upload file to remote location" + fileName + ": " + ex.ToString());
                        }
                    }

                    overAllSuccess = success;
                }
            }
            return overAllSuccess;
        }

        public void ManageRemoteLocationFileCount(string remoteLocation, int maxFiles)
        {
            string uri = m_remoteLocation;
            string escapedUri = uri.Replace("../", "%2E%2E/");
            string file;
            try
            {
                using (FTPSClient client = new FTPSClient())
                {
                    client.Connect(m_ftpServerIp, 2121, new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword),
                                    ESSLSupportMode.ClearText, null, null, 0, 0, 0, null, true, EDataConnectionMode.Active);
                    
                    IList<DirectoryListItem> fileList = client.GetDirectoryList(escapedUri);
                    client.Close();
                    if (fileList.Count > maxFiles)
                    {//Delete oldest file
                        int numFilesToDelete = fileList.Count() - maxFiles;
                        int currentFile = 1;
                        List<DirectoryListItem> oldestFiles = fileList.OrderBy(f => f.CreationTime).ToList();
                        oldestFiles = oldestFiles.GetRange(0, numFilesToDelete);
                        foreach(DirectoryListItem ftpFile in oldestFiles)
                        {
                            DeleteFileFromFtpLocation(client, remoteLocation, ftpFile.Name, currentFile == numFilesToDelete);
                            currentFile++;
                        }
                    }
                    
                    foreach (DirectoryListItem item in fileList)
                    {

                    }
                }
            }
            catch (Exception ex)
            {
                Log("ERROR:  File management error: " + ex.ToString());
            }
        }

        public bool UploadToSources(List<string> files)
        {
            int fileNumber = 0;
            bool overallSuccess = true;
            int totalRemoteLocations = m_remoteLocations.Count();
            int currentLocal = 1;
            foreach (string location in m_remoteLocations)
            {
                fileNumber = 0;
                foreach (string fileName in files)
                {
                    
                    if (overallSuccess)
                    {
                        bool success = true;
                        //Send file to server via FTP
                        string uri = m_remoteLocation + "/" + fileName;
                        string escapedUri = uri.Replace("../", "%2E%2E/");
                        fileNumber++;

                        using (FTPSClient client = new FTPSClient())
                        {
                            try
                            {   // Connect to the server with no encryption
                                client.Connect(m_ftpServerIp, 2121, new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword),
                                               ESSLSupportMode.ClearText, null, null, 0, 0, 0, null, true, EDataConnectionMode.Active);

                                ulong bytesTransfered = client.PutFile(m_tempLocation + fileName, escapedUri);

                                if (bytesTransfered <= 0)
                                {
                                    success = false;
                                    Log("ERROR:  Could not upload file to remote location " + fileName + "bytes copied:" + bytesTransfered.ToString());
                                }
                                else
                                {
                                    Log("INFO:  File transfered successfully");
                                    //Delete the local file
                                    try
                                    {
                                        System.IO.File.Delete(fileName);
                                    }
                                    catch (Exception e)
                                    {
                                        Log("ERROR:  Could not delete original file: " + e.ToString());
                                    }
                                }

                                client.Close();
                            }
                            catch (Exception ex)
                            {
                                success = false;
                                Log("ERROR:  Could not upload file to remote location" + fileName + ": " + ex.ToString());
                            }
                        }

                        overallSuccess = success;
                    }
                }
                currentLocal++;

            }
            return overallSuccess;
        }

    }
}
