using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
//using LoggingInterface;
using System.Net;
using System.Globalization;
using System.Text.RegularExpressions;
using BomFileProcessor;

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
            m_remoteLocation = remoteLocation;
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
            string uri = "ftp://" + m_ftpServerIp + m_remoteLocation;
            string escapedUri = uri.Replace("../", "%2E%2E/");
            //m_logger.Log(" FileChangeMonitorFtpLocation() - Enter");

            while (!TerminateCalled())
            {
                try
                {
                    //m_logger.Log("Terminate not called for FTP File Monitor\n");
                    if (Directory.Exists(m_localLocation) && Directory.Exists(m_tempLocation))
                    {//both directories exist check num files
                        //m_logger.Log("FTP File Monitor directories exist\n");
                        FtpWebRequest reqFTP = (FtpWebRequest)FtpWebRequest.Create(new Uri(escapedUri));
                        reqFTP.Credentials = new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword);
                        reqFTP.Method = WebRequestMethods.Ftp.ListDirectoryDetails;
                        reqFTP.UseBinary = true;
                        reqFTP.UsePassive = false;
                        //Send request for directory listing, get response
                        FtpWebResponse respFTP = (FtpWebResponse)reqFTP.GetResponse();
                        Stream responseStream = respFTP.GetResponseStream();
                        StreamReader reader = new StreamReader(responseStream);
                        List<FtpFileInfo> ftpFileInfos = new List<FtpFileInfo>();
                        //Create list of files
                        //note format only verified for qnx response
                        //skip first line (num of files)
                        reader.ReadLine();
                        while (!reader.EndOfStream)
                        {
                            //m_logger.Log("Checking the file stream\n");
                            string file;
                            file = reader.ReadLine();
                            if (file[0] == '-')
                            {//regular file (omits directories)
                                FtpFileInfo fi = new FtpFileInfo(file);
                                if (FitsMask(fi.Name, m_fileMask))
                                {
                                    ftpFileInfos.Add(fi);
                                    m_logger.Log("INFO: Added " + fi.Name + " to the file list\n");
                                }
                            }
                        }


                        DirectoryInfo localInfo = new DirectoryInfo(m_localLocation);
                        if (ftpFileInfos.Count() > 0)
                        {
                            m_logger.Log("INFO:    Files to transfer found, updating " + m_localLocation);
                            UpdateLocalDirectoryRemoteFtpLocation(ftpFileInfos, localInfo);
                        }
                        /*else
                        {
                            m_logger.Log("No file found to transfer\n");
                        }*/
                    }
                    else
                    {
                        m_logger.Log("ERROR:    FileMonitor::Temp or Target directory does not exist temp: " +
                            m_tempLocation + " Target: " + m_localLocation);
                        //m_terminate = true;
                    }
                }
                catch (Exception e)
                {//Error remote dir gone before transfer complete
                    //m_logger.Log("ERROR:    FileMonitor::Error during File update Process " + e.ToString());
                }
                Thread.Sleep(m_fileCheckInterval);
            }
            //m_logger.Log("FileChangeMonitorFtpLocation() - Exit");
        }

        public List<string> UpdateLocalDirectoryRemoteFtpLocation(List<FtpFileInfo> remoteInfo, DirectoryInfo localInfo)
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
                    foreach (FtpFileInfo fi in remoteInfo)
                    {//copy to temp folder first in case remote directory goes down durring transfer
                        if (!TransferFileFromFtpLocation(fi.Name))
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
                            fi.CopyTo(Path.Combine(localInfo.ToString(), fi.Name), true);
                            DeleteFileFromFtpLocation(m_remoteLocation, fi.Name, currentFile == fileCount);
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

        public void DeleteFileFromFtpLocation(string location, string fileName, bool isFinal)
        {
            try
            {
                string uri = "ftp://" + m_ftpServerIp + location;
                string escapedUri = uri.Replace("../", "%2E%2E/");
                FtpWebRequest remFileReq = (FtpWebRequest)FtpWebRequest.Create(new Uri(escapedUri + fileName));
                remFileReq.Credentials = new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword);
                remFileReq.Method = WebRequestMethods.Ftp.DeleteFile;
                remFileReq.UseBinary = true;
                remFileReq.UsePassive = false;
                remFileReq.KeepAlive = !isFinal;
                FtpWebResponse remFileResp = (FtpWebResponse)remFileReq.GetResponse();
                remFileResp.Close();
            }
            catch (Exception ex)
            {
                Log("ERROR:  Could not delete remote copy of file " + fileName + ": " + ex.ToString());
            }
        }

        public bool TransferFileFromFtpLocation(string fileName)
        {
            Log("TransferFileFromFtpLocation - Enter\n");
            int buffLength = 2048;
            byte[] buff = new byte[buffLength];
            int contentLen;
            bool success = true;

            FileStream fs = null;
            FileInfo fileInf = new FileInfo(Path.Combine(m_tempLocation, fileName));
            string uri = "ftp://" + m_ftpServerIp + m_remoteLocation;
            string escapedUri = uri.Replace("../", "%2E%2E/");

            FtpWebRequest fileReq = (FtpWebRequest)FtpWebRequest.Create(new Uri(escapedUri + fileName));
            fileReq.Credentials = new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword);
            fileReq.Method = WebRequestMethods.Ftp.DownloadFile;
            fileReq.UseBinary = true;
            fileReq.UsePassive = false;
            fileReq.KeepAlive = true;

            try
            {
                Log("Trying to get files\n");
                fs = fileInf.Create();
                if (fs == null)
                {
                    Log("ERROR:  Could not create local copy of remote file");
                    success = false;
                }
                else
                {
                    Log("created a local copy of the file\n");
                    FtpWebResponse fileResp = (FtpWebResponse)fileReq.GetResponse();
                    Stream strm = fileResp.GetResponseStream();
                    BinaryReader fileReader = new BinaryReader(strm);


                    contentLen = fileReader.Read(buff, 0, buffLength);
                    Log("Read the file\n");
                    while (contentLen != 0)
                    {
                        Log("Writing content to the file\n");
                        fs.Write(buff, 0, contentLen);
                        contentLen = fileReader.Read(buff, 0, buffLength);
                    }
                    Log("INFO:  Retrieved file " + fileName);

                    fileReader.Close();
                    strm.Close();
                    fileResp.Close();
                }
            }
            catch (Exception ex)
            {
                Log("ERROR:  Could not create local copy of remote file " + fileName + ": " + ex.ToString());
                fs = null;
                success = false;
            }
            fs.Seek(0, SeekOrigin.Begin);
            fs.Close();
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

                    fileNumber++;
                    FileInfo fileInf = new FileInfo(fileName);
                    //ensure file is completely formed
                    FileStream fs = null;
                    try
                    {
                        fs = TryOpen(fileInf, 100, 250);
                        if (fs != null)
                        {
                            Log("INFO:  File Integrity Checked, beginning ftp process - test");

                            //Send file to server via FTP
                            string uri = "ftp://" + m_ftpServerIp + m_remoteLocation + "/" + fileInf.Name;
                            string escapedUri = uri.Replace("../", "%2E%2E/");
                            FtpWebRequest reqFTP;
                            
                            // Create FtpWebRequest object from the Uri provided
                            reqFTP = (FtpWebRequest)FtpWebRequest.Create(new Uri(escapedUri));
                            // Provide the WebPermission Credintials
                            reqFTP.Credentials = new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword);
                            // Specify the command to be executed.
                            reqFTP.Method = WebRequestMethods.Ftp.UploadFile;
                            // Specify the data transfer type.
                            reqFTP.UseBinary = true;

                            // Notify the server about the size of the uploaded file
                            reqFTP.ContentLength = fileInf.Length;
                            // By default KeepAlive is true, where the control connection is 
                            // not closed after a command is executed.
                            if (files.Count != fileNumber)
                            {
                                reqFTP.KeepAlive = true;
                            }
                            else
                            {
                                reqFTP.KeepAlive = false;
                            }
                            // The buffer size is set to 2kb
                            int buffLength = 2048;
                            byte[] buff = new byte[buffLength];
                            int contentLen;
                            Stream strm = null;
                            try
                            {
                                // Stream to which the file to be upload is written
                                Log("INFO:  Preparing to transfer file ");
                                Log("INFO:  Opening FTP Connection");
                                strm = reqFTP.GetRequestStream();

                                Log("INFO:  Reading content from file");
                                // Read from the file stream 2kb at a time
                                contentLen = fs.Read(buff, 0, buffLength);

                                // Till Stream content ends
                                Log("INFO:  File Transfer Started");
                                while (contentLen != 0)
                                {
                                    // Write Content from the file stream to the 
                                    // FTP Upload Stream
                                    strm.Write(buff, 0, contentLen);
                                    contentLen = fs.Read(buff, 0, buffLength);
                                }
                                Log("INFO:  File Transfer Complete");
                                Log("INFO:  File transfered successfully");

                            }
                            catch (Exception ex)
                            {
                                Log("ERROR:  Upload Error: " + ex.ToString());
                                success = false;
                                overAllSuccess = false;
                            }
                            if (strm != null)
                            {// Close the Request Stream
                                strm.Close();
                            }
                            fs.Seek(0, SeekOrigin.Begin);


                            if (success)
                            {
                                Log("INFO:  " + fileName + " transferred successfully");
                                //remove file from local directory
                                try
                                {
                                    fs.Close();
                                    fs = null;
                                    System.IO.File.Delete(fileName);
                                }
                                catch
                                {
                                    Log("ERROR:  Could not delete original file");
                                }
                            }
                            else
                            {
                                Log("ERROR:  " + fileName + " was not transfered successfully");
                                overAllSuccess = false;
                            }
                        }
                        else
                        {
                            Log("ERROR:  " + fileName + " File Integrity Check Failed");
                            overAllSuccess = false;
                        }
                    }
                    catch
                    {
                        Log("ERROR:  " + fileName + " Unknown Exception");
                        overAllSuccess = false;
                    }
                    if (fs != null)
                    {
                        fs.Close();
                    }
                }
            }
            return overAllSuccess;
        }

        public void ManageRemoteLocationFileCount(string remoteLocation, int maxFiles)
        {
            string uri = "ftp://" + m_ftpServerIp + remoteLocation;
            string escapedUri = uri.Replace("../", "%2E%2E/");
            string file;
            try
            {

                //check num files
                FtpWebRequest reqFTP = (FtpWebRequest)FtpWebRequest.Create(new Uri(escapedUri));
                reqFTP.Credentials = new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword);
                reqFTP.Method = WebRequestMethods.Ftp.ListDirectoryDetails;
                reqFTP.UseBinary = true;
                reqFTP.UsePassive = false;
                //Send request for directory listing, get response
                FtpWebResponse respFTP = (FtpWebResponse)reqFTP.GetResponse();
                Stream responseStream = respFTP.GetResponseStream();
                StreamReader reader = new StreamReader(responseStream);
                List<FtpFileInfo> ftpFileInfos = new List<FtpFileInfo>();
                //Create list of files
                //note format only verified for qnx response
                //skip first line (num of files)
                reader.ReadLine();
                while (!reader.EndOfStream)
                {
                    file = reader.ReadLine();
                    if (file[0] == '-')
                    {//regular file (omits directories)
                        FtpFileInfo fi = new FtpFileInfo(file);
                        if (FitsMask(fi.Name, m_fileMask))
                        {
                            ftpFileInfos.Add(fi);
                        }
                    }
                }
                if (ftpFileInfos.Count() > maxFiles)
                {//delete oldest files
                    int numFilesToDelete =ftpFileInfos.Count()- maxFiles;
                    int currentFile = 1;

                    List<FtpFileInfo> oldestFiles = ftpFileInfos.OrderBy(f => f.DateCreated).ToList();
                    oldestFiles = oldestFiles.GetRange(0,numFilesToDelete);
                    foreach (FtpFileInfo ftpFile in oldestFiles)
                    {
                        DeleteFileFromFtpLocation(remoteLocation, ftpFile.Name, currentFile == numFilesToDelete);
                        currentFile++;
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

                        fileNumber++;
                        FileInfo fileInf = new FileInfo(fileName);
                        //ensure file is completely formed
                        FileStream fs = null;
                        try
                        {
                            fs = TryOpen(fileInf, 100, 250);
                            if (fs != null)
                            {
                                //Log("INFO: Beginning ftp process");
                                //Send file to server via FTP
                                string uri = "ftp://" + m_ftpServerIps[currentLocal - 1] + m_remoteLocations[currentLocal - 1] + fileInf.Name;
                                string escapedUri = uri.Replace("../", "%2E%2E/"); 
                                FtpWebRequest reqFTP;
                                // Create FtpWebRequest object from the Uri provided
                                reqFTP = (FtpWebRequest)FtpWebRequest.Create(new Uri(escapedUri));
                                // Provide the WebPermission Credintials
                                reqFTP.Credentials = new NetworkCredential(m_ftpUserLogins[currentLocal - 1], m_ftpUserPasswords[currentLocal - 1]);
                                // Specify the command to be executed.
                                reqFTP.Method = WebRequestMethods.Ftp.UploadFile;
                                // Specify the data transfer type.
                                reqFTP.UseBinary = true;

                                // Notify the server about the size of the uploaded file
                                reqFTP.ContentLength = fileInf.Length;
                                // By default KeepAlive is true, where the control connection is 
                                // not closed after a command is executed.
                                if (files.Count != fileNumber)
                                {
                                    reqFTP.KeepAlive = true;
                                }
                                else
                                {
                                    reqFTP.KeepAlive = false;
                                }
                                // The buffer size is set to 2kb
                                int buffLength = 2048;
                                byte[] buff = new byte[buffLength];
                                int contentLen;
                                Stream strm = null;
                                try
                                {
                                    // Stream to which the file to be upload is written
                                    //Log("INFO:  Preparing to transfer file ");
                                    //Log("INFO:  Opening FTP Connection");
                                    try
                                    {
                                        strm = reqFTP.GetRequestStream();
                                    }
                                    catch (WebException web)
                                    {
                                        m_logger.Log("ERROR:  Upload Error: " + web.ToString());
                                        success = false;
                                        overallSuccess = false;
                                    }
                                    catch (ProtocolViolationException pro)
                                    {
                                        m_logger.Log("ERROR:  Upload Error: " + pro.ToString());
                                        success = false;
                                        overallSuccess = false;
                                    }
                                    catch (InvalidOperationException inv)
                                    {
                                        m_logger.Log("ERROR:  Upload Error: " + inv.ToString());
                                        success = false;
                                        overallSuccess = false;
                                    }
                                    catch (NullReferenceException nu)
                                    {
                                        m_logger.Log("ERROR:  Upload Error: " + nu.ToString());
                                        success = false;
                                        overallSuccess = false;
                                    }
                                    catch (ArgumentNullException ar)
                                    {
                                        m_logger.Log("ERROR:  Upload Error: " + ar.ToString());
                                        success = false;
                                        overallSuccess = false;
                                    }
                                    catch (Exception ex)
                                    {
                                        m_logger.Log("ERROR:  Upload Error: " + ex.ToString());
                                        success = false;
                                        overallSuccess = false;
                                    }

                                    // Read from the file stream 2kb at a time
                                    contentLen = fs.Read(buff, 0, buffLength);

                                    // Till Stream content ends
                                    while (contentLen != 0)
                                    {
                                        // Write Content from the file stream to the 
                                        // FTP Upload Stream
                                        strm.Write(buff, 0, contentLen);
                                        contentLen = fs.Read(buff, 0, buffLength);
                                    }
                                    //Log("INFO:  File Transfer Complete");
                                    //Log("INFO:  File transfered successfully");
                                }
                                catch (Exception ex)
                                {
                                    Log("ERROR:  Upload Error: " + ex.ToString());
                                    success = false;
                                    overallSuccess = false;
                                }

                                if (strm != null)
                                {// Close the Request Stream
                                    strm.Close();
                                }
                                fs.Seek(0, SeekOrigin.Begin);

                                //Log("Before checking success\n");
                                if (success)
                                {
                                    //Log("INFO:  " + fileName + " transfered successfully");
                                    //remove file from local directory
                                    fs.Close();
                                    try
                                    {
                                        
                                        fs = null;
                                        //if final transfer and successfull
                                        if (currentLocal == totalRemoteLocations && success)
                                        {
                                            System.IO.File.Delete(fileName);
                                        }
                                    }
                                    catch
                                    {
                                        Log("ERROR:  Could not delete original file");
                                        success = false;
                                        overallSuccess = false;
                                    }
                                }
                                else
                                {
                                    Log("ERROR:  " + fileName + " was not transfered successfully");
                                    success = false;
                                    overallSuccess = false;
                                }
                            }
                            else
                            {
                                Log("ERROR:  " + fileName + " File Integrity Check Failed");
                                success = false;
                                overallSuccess = false;
                            }
                        }
                        catch
                        {
                            Log("ERROR:  " + fileName + " Unknown Exception");
                            success = false;
                            overallSuccess = false;
                        }
                        if (fs != null)
                        {
                            fs.Close();
                        }
                    }
                }
                currentLocal++;

            }
            return overallSuccess;
        }

    }
}
