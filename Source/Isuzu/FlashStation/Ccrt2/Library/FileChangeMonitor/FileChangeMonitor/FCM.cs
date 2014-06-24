using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
using LoggingInterface;

namespace FileChangeMonitor
{
    public class FCM
    {
        private string m_sourceDirectory;
        private string m_targetDirectory;
        private string m_tempDirectory;
        private int m_updateTime;
        private Logger m_logger;
        private volatile bool m_terminate;
        Thread m_fcmThread;
        public FCM(string source, string target, string temp, int updateTime, Logger logger)
        {
            m_sourceDirectory = source;
            m_targetDirectory = target;
            m_tempDirectory = temp;
            m_updateTime = updateTime;
            m_logger = logger;
            ThreadStart taskDelegate = null;
            taskDelegate = new ThreadStart(BuildRecordUpdateThread);
            m_fcmThread = new Thread(taskDelegate);
            m_terminate = false;
            m_fcmThread.Start();
        }
        public void StopFCMThread()
        {
            m_terminate = true;
        }
        public void BuildRecordUpdateThread()
        {
            while (!m_terminate)
            {
                try
                {
                    if (Directory.Exists(m_sourceDirectory) && Directory.Exists(m_targetDirectory) && Directory.Exists(m_tempDirectory))
                    {//both directories exist check num files
                        DirectoryInfo remoteInfo = new DirectoryInfo(m_sourceDirectory);
                        DirectoryInfo localInfo = new DirectoryInfo(m_targetDirectory);
                        List<FileInfo> remoteFilesInfo = new List<FileInfo>(remoteInfo.GetFiles().ToList());
                        List<FileInfo> localFilesInfo = new List<FileInfo>(localInfo.GetFiles().ToList());
                        if (remoteFilesInfo.Count() != localFilesInfo.Count())
                        {
                            m_logger.Log("INFO:    FCM::File Count Changed updating " + m_targetDirectory);
                            updateLocalBuildDirectory(remoteInfo, localInfo);
                        }
                        else
                        {//check file modification times if different update
                            //remoteFilesInfo.ToList().Sort(new CompareFileInfoEntries());
                            //localFilesInfo.ToList().Sort(new CompareFileInfoEntries());

                            for (int x = 0; x < remoteFilesInfo.Count(); x++)
                            {//check name and modification date
                                int index = 0;
                                for (index = 0; index < remoteFilesInfo.Count(); index++)
                                {//find corresponding index of name
                                    if (remoteFilesInfo[x].Name == localFilesInfo[index].Name)
                                    {
                                        //found
                                        break;
                                    }
                                }
                                if (index == remoteFilesInfo.Count())
                                {//not found
                                    m_logger.Log("INFO:    FCM::Name Difference updating " + m_targetDirectory);
                                    updateLocalBuildDirectory(remoteInfo, localInfo);
                                    //skip checking rest all will be updated
                                    break;
                                }
                                else if (remoteFilesInfo[x].LastWriteTime != localFilesInfo[index].LastWriteTime)
                                {
                                    m_logger.Log("INFO:    FCM::File Mod Time Changed " + m_targetDirectory);
                                    updateLocalBuildDirectory(remoteInfo, localInfo);
                                    //skip checking rest all will be updated
                                    break;
                                }
                            }
                        }

                    }
                    else
                    {
                        m_logger.Log("ERROR:    FCM::Source or Target directory does not exist source: " +
                            m_sourceDirectory + " Target: " + m_targetDirectory);
                        //m_terminate = true;
                    }
                }
                catch (Exception e)
                {//Error remote dir gone before transfer complete
                    m_logger.Log("ERROR:    FCM::Error durring File update Process " + e.ToString());
                }
                Thread.Sleep(m_updateTime);
            }
        }

        public class CompareFileInfoEntries : IComparer<FileInfo>
        {
            public int Compare(FileInfo f1, FileInfo f2)
            {
                return (string.Compare(f1.Name, f2.Name));
            }
        }  
       
        public void updateLocalBuildDirectory(DirectoryInfo remoteInfo, DirectoryInfo localInfo)
        {
            try
            {//delete current temp files
                DirectoryInfo tempInfo = new DirectoryInfo(m_tempDirectory);
                foreach (FileInfo fi in tempInfo.GetFiles())
                {
                    fi.Delete();
                }
                foreach (FileInfo fi in remoteInfo.GetFiles())
                {//copy to temp folder first in case remote directory goes down durring transfer
                    fi.CopyTo(Path.Combine(m_tempDirectory, fi.Name), true);
                }
                if (Directory.Exists(remoteInfo.ToString()))
                {//remote still active allow update                   
                    foreach (FileInfo fi in localInfo.GetFiles())
                    {
                        fi.Delete();
                    }
                    foreach (FileInfo fi in tempInfo.GetFiles())
                    {
                        fi.CopyTo(Path.Combine(localInfo.ToString(), fi.Name), true);
                    }
                }
                else
                {//Error remote dir gone before transfer complete
                    m_logger.Log("ERROR:    FCM::Error durring File update Process, check remote pc active");
                }
            }
            catch (Exception e)
            {//Error remote dir gone before transfer complete
                m_logger.Log("ERROR:    FCM::Error durring File update Process, check remote pc active " + e.ToString());
            }
        }
    }
}
