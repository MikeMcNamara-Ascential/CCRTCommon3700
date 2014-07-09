using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
using LoggingInterface;
using System.Net;
using System.Globalization;

namespace FileMonitorNamespace
{
    public class FtpFileMonitor
    {
        protected string m_sourceLocation;
        protected string m_targetLocation;
        protected string m_tempLocation;
        protected int m_fileCheckInterval;
        protected Logger m_logger;
        protected volatile bool m_terminate;
        protected object m_terminateLock;
        Thread m_fileMonitorThread;

        protected string m_ftpUserLogin;
        protected string m_ftpUserPassword;
        protected string m_ftpServerIp;

        public object DirectoryLock;


        public FtpFileMonitor(string source, string target, string temp, int fileCheckInterval,string userLogin, string password, string ftpServerIp, Logger logger = null)
        {
            DirectoryLock = new object();
            m_sourceLocation = source;
            m_targetLocation = target;
            m_tempLocation = temp;
            m_fileCheckInterval = fileCheckInterval;
            m_logger = logger;
            m_ftpUserLogin = userLogin;
            m_ftpUserPassword = password;
            m_ftpServerIp = ftpServerIp;
            m_terminate = false;
            m_terminateLock = new object();
            ThreadStart taskDelegate = null;
            taskDelegate = new ThreadStart(FileChangeMonitorFtpLocation);
            m_fileMonitorThread = new Thread(taskDelegate);

            m_fileMonitorThread.Start();
        }

        public void StopFCMThread()
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
            string uri = "ftp://" + m_ftpServerIp + m_sourceLocation;
            string escapedUri = uri.Replace("../", "%2E%2E/");


            while (!TerminateCalled())
            {
                try
                {
                    if (Directory.Exists(m_targetLocation) && Directory.Exists(m_tempLocation))
                    {//both directories exist check num files
                        FtpWebRequest reqFTP = (FtpWebRequest)FtpWebRequest.Create(new Uri(escapedUri));
                        reqFTP.Credentials = new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword);
                        reqFTP.Method = WebRequestMethods.Ftp.ListDirectoryDetails;
                        reqFTP.UseBinary = true;
                        reqFTP.UsePassive = true;
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
                            string file;
                            file = reader.ReadLine();
                            FtpFileInfo fi = new FtpFileInfo(file);
                            ftpFileInfos.Add(fi);
                        }


                        DirectoryInfo localInfo = new DirectoryInfo(m_targetLocation);
                        List<FileInfo> localFilesInfo = new List<FileInfo>(localInfo.GetFiles().ToList());
                        if (ftpFileInfos.Count() > 0)
                        {
                            Log("INFO:    Files to transfer found, updating " + m_targetLocation);
                            UpdateLocalDirectoryRemoteFtpLocation(ftpFileInfos, localInfo);
                        }
                    }
                    else
                    {
                        Log("ERROR:    FileMonitor::Temp or Target directory does not exist temp: " +
                            m_tempLocation + " Target: " + m_targetLocation);
                        //m_terminate = true;
                    }
                }
                catch (Exception e)
                {//Error remote dir gone before transfer complete
                    Log("ERROR:    FileMonitor::Error durring File update Process " + e.ToString());
                }
                Thread.Sleep(m_fileCheckInterval);
            }
        }

        public List<string> UpdateLocalDirectoryRemoteFtpLocation(List<FtpFileInfo> remoteInfo, DirectoryInfo localInfo)
        {
            List<string> filesTransfered = new List<string>();

                try
                {//delete current temp files
                    DirectoryInfo tempInfo = new DirectoryInfo(m_tempLocation);
                    foreach (FileInfo fi in tempInfo.GetFiles())
                    {
                        fi.Delete();
                    }
                    //get all files
                    foreach (FtpFileInfo fi in remoteInfo)
                    {//copy to temp folder first in case remote directory goes down durring transfer
                        if (!TransferFileFromFtpLocation(fi.Name))
                        {
                            break;
                        }
                    }
                    int fileCount = tempInfo.GetFiles().Count();
                    int currentFile = 1;
                    //place temp files in final directory overwriting if neccessary
                    //lock to coordinate access to directory
                    lock (DirectoryLock)
                    {
                            foreach (FileInfo fi in tempInfo.GetFiles())
                            {
                                fi.CopyTo(Path.Combine(localInfo.ToString(), fi.Name), true);
                                DeleteFileFromFtpLocation(fi.Name, currentFile == fileCount);
                                currentFile++;
                            }
                    }
                }
                catch (Exception e)
                {//Error remote dir gone before transfer complete
                    Log("ERROR:    FileMonitor::Error durring File update Process, check remote pc active " + e.ToString());
                }
            
            return filesTransfered;
        }

        public void DeleteFileFromFtpLocation(string fileName, bool isFinal)
        {
            try
            {
                string uri = "ftp://" + m_ftpServerIp + m_sourceLocation;
                string escapedUri = uri.Replace("../", "%2E%2E/");
                FtpWebRequest remFileReq = (FtpWebRequest)FtpWebRequest.Create(new Uri(escapedUri + fileName));
                remFileReq.Credentials = new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword);
                remFileReq.Method = WebRequestMethods.Ftp.DeleteFile;
                remFileReq.UseBinary = true;
                remFileReq.UsePassive = true;
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
            int buffLength = 2048;
            byte[] buff = new byte[buffLength];
            int contentLen;
            bool success = true;

            FileStream fs = null;
            FileInfo fileInf = new FileInfo(Path.Combine(m_tempLocation, fileName));
            string uri = "ftp://" + m_ftpServerIp + m_sourceLocation;
            string escapedUri = uri.Replace("../", "%2E%2E/");

            FtpWebRequest fileReq = (FtpWebRequest)FtpWebRequest.Create(new Uri(escapedUri + fileName));
            fileReq.Credentials = new NetworkCredential(m_ftpUserLogin, m_ftpUserPassword);
            fileReq.Method = WebRequestMethods.Ftp.DownloadFile;
            fileReq.UseBinary = true;
            fileReq.UsePassive = true;
            fileReq.KeepAlive = true;

            try
            {
                fs = fileInf.Create();
                if (fs == null)
                {
                    Log("ERROR:  Could not create local copy of remote file");
                    success = false;
                }
                else
                {
                    FtpWebResponse fileResp = (FtpWebResponse)fileReq.GetResponse();
                    Stream strm = fileResp.GetResponseStream();
                    BinaryReader fileReader = new BinaryReader(strm);


                    contentLen = fileReader.Read(buff, 0, buffLength);

                    while (contentLen != 0)
                    {
                        fs.Write(buff, 0, contentLen);
                        contentLen = fileReader.Read(buff, 0, buffLength);
                    }
                    //Log("INFO:  Retrieved file " + fileName);

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
                    Log("INFO:  Waiting for file Transfer completion # " + attempts.ToString());
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

        public void UploadToSource(List<string> files)
        {
            int fileNumber = 0;

            foreach (string fileName in files)
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
                        Log("INFO:  File Integrity Checked, beginning ftp process");

                        //Send file to server via FTP
                        string uri = "ftp://" + m_ftpServerIp + m_sourceLocation + "/" + fileInf.Name;
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
                            //Log("INFO:  Preparing to transfer file ");
                            //Log("INFO:  Opening FTP Connection");
                            strm = reqFTP.GetRequestStream();

                            //Log("INFO:  Reading content from file");
                            // Read from the file stream 2kb at a time
                            contentLen = fs.Read(buff, 0, buffLength);

                            // Till Stream content ends
                            //Log("INFO:  File Transfer Started");
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
                        }
                        if (strm != null)
                        {// Close the Request Stream
                            strm.Close();
                        }
                        fs.Seek(0, SeekOrigin.Begin);


                        if (success)
                        {
                            Log("INFO:  " + fileName + " transfered successfully");
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
                            Log("INFO:  " + fileName + " was not transfered successfully");
                        }
                    }
                    else
                    {
                        Log("ERROR:  "+fileName + " File Integrity Check Failed");
                    }
                }
                catch
                {
                    Log("ERROR:  " + fileName + " Unknown Exception");
                }
                if (fs != null)
                {
                    fs.Close();
                }
            }
        }


    }
}
