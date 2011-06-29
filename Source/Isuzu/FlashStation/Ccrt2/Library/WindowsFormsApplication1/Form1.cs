using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SerialPortLibrary;
using System.Threading;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            m_serialPort = new CcrtSerialPort();
            m_serialPort.UpdatePortSettings("COM2", 9600, System.IO.Ports.Parity.None, 8, System.IO.Ports.StopBits.One, 500, 500, true);
            m_serialPort.OpenPort();
            m_serialPort.ShowPortDataForm();


            //start serial thread to allow barcode input
            ThreadStart taskDelegate = null;
            taskDelegate = new ThreadStart(GetSerialDeviceInput);
            Thread taskThread = new Thread(taskDelegate);
            taskThread.Start();
        }
        CcrtSerialPort m_serialPort;
        public void GetSerialDeviceInput()
        {

            while (m_serialPort.BytesRead == 0)
            {//wait for user input or scanned data
                Thread.Sleep(100);
            }
            byte[] receivedData = m_serialPort.RxData;
            string barcode = System.Text.Encoding.ASCII.GetString(receivedData);
            if (m_serialPort.BytesRead != 0)
            {//set barcode vlue
                UpdateLabelText(barcode);
            }
        }
        delegate void SetLabel1TextCallback(string text);
        public void UpdateLabelText(string text)
        {
            if (this.label1.InvokeRequired)
            {
                SetLabel1TextCallback d = new SetLabel1TextCallback(UpdateLabelText);
                this.Invoke(d, new object[] { text });
            }
            else label1.Text = text;
        }
    }
}
