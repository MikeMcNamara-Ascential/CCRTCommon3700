using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using LoggingInterface;
using System.Threading;

namespace LoggerTest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            Setup();
        }

        public void Setup()
        {
            m_logger = new Logger(richTextBox1,"LogTest");

            for (int x = 0; x < 100; x++)
            {
                //start some logging threads
                Thread testThread = new Thread(delegate()
                {
                    LogThread(x);
                });
                testThread.Start();
            }
        }
        public void LogThread(int threadNum)
        {
            Random rnd = new Random();
            string message = "All work and no play makes bom something something " + threadNum.ToString() ;
            while (true)
            {
                m_logger.Log(message);
                Thread.Sleep(rnd.Next(5, 25));
            }
        }

        Logger m_logger;
    }
}
