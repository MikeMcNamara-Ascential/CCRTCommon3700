using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FileMonitorNamespace;
using System.IO;
using System.Windows.Forms;
using System.Net;

namespace TestFtpFcm
{
    public class ConsoleFileMonitor : FtpFileMonitor
    {
        public ConsoleFileMonitor(string source, string target, string temp, int fileCheckInterval,
            string userLogin, string password, string ftpServerIp)
            : base(source, target, temp, fileCheckInterval,
                userLogin, password, ftpServerIp)
        {
        }

        public override void Log(string message)
        {
            Console.WriteLine(message);
        }

    }
    class Program
    {
        ConsoleFileMonitor m_fileChangeMonitor;
        string m_targetPath;
        string m_tempPath;
        int m_fileCheckInterval;
       

        string m_sourcePath;
        string m_userLogin;
        string m_password;
        string m_ftpServerIp;

        static void Main(string[] args)
        {
            new Program().StartTests();
        }

        public void StartTests()
        {
            Setup();
            if (!NormalFileTransfer()) ExitSequence( false);
            else ExitSequence(true);
        }

        public void Setup()
        {
            string appDirectory = Path.GetDirectoryName(Application.ExecutablePath);
            m_targetPath = appDirectory+"\\Target";
            m_tempPath = appDirectory+"\\Temp";
            m_fileCheckInterval = 200;

            m_sourcePath = "//home/CCRT/Rewrite/TestResults/ftpOutBox/";
            m_userLogin = "burke";
            m_password = "porter";
            m_ftpServerIp = "192.168.1.1:2121";

            if (!Directory.Exists(m_tempPath))
            {
                Directory.CreateDirectory(m_tempPath);
            }
            if (!Directory.Exists(m_targetPath))
            {
                Directory.CreateDirectory(m_targetPath);
            }
            m_fileChangeMonitor = new ConsoleFileMonitor(m_sourcePath, m_targetPath, m_tempPath, m_fileCheckInterval,m_userLogin,m_password,m_ftpServerIp);
        }

        bool NormalFileTransfer()
        {
            //clear target directory
            DirectoryInfo targetInfo = new DirectoryInfo(m_targetPath);
            foreach (FileInfo fi in targetInfo.GetFiles())
            {
                fi.Delete();
            }
            //create files
            string appDirectory = Path.GetDirectoryName(Application.ExecutablePath);
            List<string> filesToUpload = new List<string>();
            for (int i = 0; i < 10; i++)
            {
                string fileName = appDirectory + "\\DummyFile"+i.ToString()+".txt";
                FileStream fs = new FileStream(fileName, FileMode.Create);
                fs.Seek(512*1024, SeekOrigin.Begin);
                fs.WriteByte(Convert.ToByte(i));
                fs.Close();
                filesToUpload.Add(fileName);
            }

            //upload files

            m_fileChangeMonitor.UploadToSource(filesToUpload);

            //ensure files were transfered to our target directory
            bool filesTransfered = false;
            while (!filesTransfered)
            {
                filesTransfered = (Directory.GetFiles(m_targetPath).Count() == filesToUpload.Count());
                System.Threading.Thread.Sleep(250);
            }


            return true;
        }

        void ExitSequence(bool isPass)
        {
            Console.WriteLine();
            m_fileChangeMonitor.StopFCMThread();
            if (isPass) Console.WriteLine("PASS");
            else Console.WriteLine("FAIL");
            Console.WriteLine();

            Console.WriteLine();
            Console.WriteLine("Press <ENTER> to terminate client.");
            Console.ReadLine();
        }

    }
}
