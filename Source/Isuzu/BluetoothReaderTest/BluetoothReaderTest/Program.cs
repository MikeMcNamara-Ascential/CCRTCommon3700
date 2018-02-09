using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
//using System.Drawing;
using System.IO.Ports;
using System.IO;
using System.Linq;
using System.Text;
using System.Timers;
using System.Threading.Tasks;
using System.Net.Sockets;
using Logger;
using QnxCcrtInterface;
using System.Net.NetworkInformation;
using System.Net;

namespace BluetoothReaderTest
{
    public class TestRun
    {
        static void Main()
        {
            BlueToothReader bluetoothReader = new BlueToothReader();
            bluetoothReader.Initialize();

            Console.ReadLine();
            bluetoothReader.Save_devInterface();
        }
    }

    // Need a means of selecting COM port at runtime, unless setting via the .sys file is acceptable
    // Add measurement refresh rate to the .sys file?
    public class BlueToothReader
    {
        SerialPort serialPort = new SerialPort();
        Byte[] buffer = new Byte[48];
        int sensorType = 0;
        string Ebuffer = "";
        bool connection = false;
        string measuredValue = "";  // set up as member variable
        string formattedMeasuredValue = "";
        string devInterface = "";
        System.Collections.ArrayList lstCOMPorts = new System.Collections.ArrayList();
        string portNumber;
        int Baud_Rate = 38400;

        Timer timer1 = new Timer(100);
        Timer timer2 = new Timer(100);
        Timer timer3 = new Timer(100);

        string QnxIpAddress;
        string WindowsIpAddress;
        int QnxCcrtPort;

        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// Thread to handle the connection to the QNX CCRT system.
        /// This will allow the connection to be attempted in the background.
        /// </summary>
        private System.Threading.Thread ConnectionThread { get; set; }

        /// <summary>
        /// Thread to use for creating the QNX CCRT Interface object.
        /// </summary>
        private System.Threading.Thread CreateQnxInterfaceThread { get; set; }

        /// <summary>
        /// Logger object to use for logging activities and events.
        /// </summary>
        private CcrtLogger Logger { get; set; }

        /// <summary>
        /// QNX CCRT system interface object
        /// </summary>
        private CcrtInterface QnxCcrt { get; set; }

        private bool m_previouslyConnected;


        public string Get_Measurement()
        {
            return measuredValue;
        }

        public void Initialize()
        {
            string errorMessage;

            timer1.Elapsed += new ElapsedEventHandler(timer1_Tick);
            timer2.Elapsed += new ElapsedEventHandler(timer2_Tick);
            timer3.Elapsed += new ElapsedEventHandler(timer3_Tick);

            Read_Options();


            // Create a new logging object
            Logger = new CcrtLogger("TestPath/test.log", "BluetoothReader", 10, 50000);
            // Create a new QNX interface
            CreateQnxInterfaceThread = new System.Threading.Thread(CreateQnxInterface);
            CreateQnxInterfaceThread.Start();

            if (serialPort.IsOpen)
            {
                serialPort.Close();
            }

            try
            {
                if (devInterface != "")
                {
                    serialPort.PortName = devInterface;
                    serialPort.BaudRate = Baud_Rate;
                    serialPort.Parity = Parity.None;
                    serialPort.DataBits = 8;
                    serialPort.StopBits = StopBits.One;
                    serialPort.ReadBufferSize = 4096;
                    serialPort.DtrEnable = false;
                    serialPort.Open();
                    serialPort.DataReceived += new SerialDataReceivedEventHandler(Data_Received);

                    
                    Send_Recognition();
                    Console.WriteLine("Reading data from " + devInterface + "\n");
                    Logger.Log("Reading data from " + devInterface);
                }
                else
                {
                    serialPort.PortName = SerialPort.GetPortNames()[0];
                    serialPort.BaudRate = Baud_Rate;
                    serialPort.Parity = Parity.None;
                    serialPort.DataBits = 8;
                    serialPort.StopBits = StopBits.One;
                    serialPort.ReadBufferSize = 4096;
                    serialPort.Open();
                    serialPort.DataReceived += new SerialDataReceivedEventHandler(Data_Received);

                    Console.WriteLine("Reading data from " + serialPort.PortName + "\n");
                    Logger.Log("Reading data from " + serialPort.PortName);
                }
                timer1.Start();
                

            }
            catch (Exception ex)
            {
                errorMessage = "Error 1: Can not open a devInterface!\n" +
                            "Close the program, connect the device and switch it on, then restart the program.\n";
                connection = false;
                Console.WriteLine(errorMessage + ex.Message + "\n");
                Logger.Log("Error: " + ex.Message);
            }
        }

        /// <summary>
        /// Create a new interface to the QNX CCRT system.
        /// </summary>
        public void CreateQnxInterface()
        {   // Make sure this is a valid IP Address
            Int32 attempts = 3;
            Boolean goodIpAddress = false;
            // Set the default IP Address
            System.Threading.Thread.Sleep(2000);
            QnxCcrt = new CcrtInterface(IPAddress.Parse(WindowsIpAddress), QnxCcrtPort, Logger);
            timer3.Start();
            Logger.Log("Created new QNX CCRT Interface");
            do
            {
                try
                {
                    Ping ping = new Ping();
                    PingReply pingResult = ping.Send(IPAddress.Parse(QnxIpAddress));
                    if (pingResult.Status != IPStatus.Success)
                    {   // Could not detect QNX server
                        Logger.Log("Could not detect QNX server at " + QnxIpAddress);
                    }
                    else if (pingResult.Status == IPStatus.Success)
                    {   // Found a possible QNX server
                        Logger.Log("Detected QNX server at " + QnxIpAddress);
                        goodIpAddress = true;
                    }
                    else
                    {
                        Logger.Log("Could not detect QNX server at " + QnxIpAddress);
                    }
                }
                catch
                {
                    Logger.Log("No network connected!");
                }
            } while (!goodIpAddress && (attempts-- > 0));
            // Always start the connection status monitor thread
            // Make sure to kill off the existing connection thread
            if (ConnectionThread != null)
            {
                ConnectionThread.Abort();
            }
            // Create a new thread to attempt to connect to CCRT
            ConnectionThread = new System.Threading.Thread(QnxCcrtConnection);
            ConnectionThread.Start();
        }

        /// <summary>
        /// Attempt to connect to the QNX CCRT System.
        /// </summary>
        public void QnxCcrtConnection()
        {
            while (!QnxCcrt.QnxConnected) System.Threading.Thread.Sleep(1000);
            if (QnxCcrt.QnxConnected)
            {
                while (true)
                {
                    
                    if (QnxCcrt.QnxConnected && !m_previouslyConnected)
                    {
                        Logger.Log("Reconnected to QNX");
                    }
                    else if (!QnxCcrt.QnxConnected && m_previouslyConnected)
                    {
                        Logger.Log("Disconnected from QNX");
                    }
                    
                    System.Threading.Thread.Sleep(1000);
                }
            }
            m_previouslyConnected = QnxCcrt.QnxConnected;
        }

        private void Read_Options()
        {
            string optionPath;
            DirectoryInfo dir = new DirectoryInfo(".");
            string lineOfText, text1, text2, result;
            string[] astring = new string[12];

            text1 = dir.FullName;
            text2 = "devInterface.sys";
            optionPath = text1 + "\\" + text2;

            result = "";
            // Open file; catch and handle any errors
            try
            {
                StreamReader sr = new StreamReader(optionPath);
                lineOfText = sr.ReadLine();
                while (lineOfText != null)
                {
                    result += (lineOfText + ";");
                    lineOfText = sr.ReadLine();
                }
                sr.Close();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: " + ex.Message + "\n");
                Logger.Log("Error: " + ex.Message);
                throw;
            }
            finally
            {
                astring = result.Split(';');

                int number = astring.Length;
                if (number > 4)
                {
                    devInterface = astring[0]; //Comm port for the device
                    QnxIpAddress = astring[1]; //Qnx pc IP Address
                    QnxCcrtPort = int.Parse(astring[2]); //Port number in SerialServerConfig.xml
                    WindowsIpAddress = astring[3]; //Windows pc IP Address
                    Baud_Rate = int.Parse(astring[4]); //Device Serial Baud rate
                }
                else
                {
                }
            }
        }

        private void Data_Received(Object sender, SerialDataReceivedEventArgs e)
        {
            string errorMessage;

            try
            {
                UpdateData();
            }
            catch (Exception ex)
            {
                timer1.Enabled = false;
                errorMessage = "Error 12: Can not open a devInterface!\n" +
                               "Close the program, connect the device and switch it on, then restart the program.\n";
                connection = false;
                Console.WriteLine(errorMessage + ex.Message + "\n");
                Logger.Log("Error: " + ex.Message);
            }
        }
        
        public void UpdateData()
        {
            string text1 = "";
            int c, d, i, x;
            string buffer = "";
            decimal temp1;
            double temp2;

            Ebuffer += serialPort.ReadExisting();

            try
            {
                if (Ebuffer.Length < 9)
                {
                    return;
                }
                if (Ebuffer.Length > 20)
                {
                    Ebuffer = "";
                }

                c = Ebuffer[0];

                if (c > 2)
                {
                    d = 0;
                    while (true)
                    {
                        c = Ebuffer[d];
                        if (c == 2)
                        {
                            x = Ebuffer.Length;
                            for (i = d; i < x; i++)
                            {
                                c = Ebuffer[i];
                                buffer += Ebuffer[i];
                            }
                            Ebuffer = "";

                            x = buffer.Length;
                            for (i = 0; i < x; i++)
                            {
                                c = buffer[i];
                                Ebuffer += buffer[i];
                            }
                            break;
                        }
                        d = d + 1;
                    }
                }

                c = Ebuffer[0];
                d = Ebuffer[1];
                if (c == 2 && d == 2 || d == 3 && !connection)  // connection test
                {
                    i = 2;
                    while (true)
                    {
                        c = Ebuffer[i];
                        if (c < 48)
                        {
                            break;
                        }
                        text1 += Ebuffer[i];
                        i++;
                    }

                    i = string.Compare(text1, "ROMESS");
                    if (i == 0)
                    {
                        connection = true;
                        timer2.Stop();
                        timer2.Enabled = false;
                    }

                    timer1.Enabled = true;
                    timer1.Start();
                    Ebuffer = "";
                    c = 0;
                    return;
                }

                c = Ebuffer[0];
                d = Ebuffer[1];
                if (c == 2 && d == 7)
                {
                    timer1.Stop();

                    i = 2;
                    c = Ebuffer[i];
                    while (c > 32)
                    {
                        c = Ebuffer[i];
                        text1 += (char)c;
                        i++;
                        c = Ebuffer[i];
                    }

                    temp1 = decimal.Parse(text1);
                    temp2 = (double)temp1 / 10;
                    i = (int)temp2;

                    temp1 = (decimal)i / 10;

                    text1 = "";

                    if (temp1 < 10)
                    {
                        text1 += " ";
                    }

                    text1 += temp1.ToString();

                    formattedMeasuredValue = text1;

                    //Removed extra values from the end of the data string
                    /*int testPos = text1.IndexOf(",", 1);    // minutes place always 0
                    if (testPos == -1)
                    {
                        text1 += ",0";
                    }    

                    text1 += "°";*/

                    Console.WriteLine(text1);       // Console test readout
                    measuredValue = text1;

                    c = Ebuffer[7];
                    sensorType = Ebuffer[6];

                    Ebuffer = "";
                    text1 = "";

                    timer1.Enabled = true;
                    timer1.Start();
                }

                if (c == 2 && d == 7 && !connection)
                {
                    Ebuffer = "";
                    text1 = "";
                }

                if (c == 2 && d == 8)
                {
                    text1 = "";
                    i = 2;
                    c = 33;
                    while (c > 32)
                    {
                        c = Ebuffer[i];
                        if (c > 31)
                        {
                            text1 += (char)c;
                        }
                        i++;
                    }
                    Ebuffer = "";
                }
            }
            catch (Exception ex)
            {
                timer1.Enabled = true;
                Ebuffer = "";
                Console.WriteLine("Error: " + ex.Message + "\n");
                Logger.Log("Error: " + ex.Message);
            }
        }

        private void Collect_MeasuredValue()
        {
            buffer[0] = 33;     // !
            buffer[1] = 77;     // M
            buffer[2] = 87;     // W
            buffer[3] = 69;     // E
            buffer[4] = 72;     // H
            buffer[5] = 10;     // LF
            buffer[6] = 13;     // CR
            buffer[7] = 0;      //
            Send();
        }

        private void timer1_Tick(object sender, ElapsedEventArgs e)
        {
            if (connection)
            {
                Collect_MeasuredValue();
            }
            else
            {
                Send_Recognition();
                Ebuffer = "";
            }
        }

        private void timer2_Tick(object sender, ElapsedEventArgs e)
        {
            if (!connection)
            {
                Send_Recognition();
            }
        }

        private void timer3_Tick(object sender, ElapsedEventArgs e)
        {
            QnxCcrt.Write("SteeringWheelAngle", formattedMeasuredValue);
        }

        public void Save_devInterface()
        {
            string p_Data = "";
            string text1 = "";
            string text2 = "";
            string dxs = Environment.CurrentDirectory;
            DirectoryInfo dir = new DirectoryInfo(".");

            text1 = dir.FullName;
            text2 = "devInterface.sys";
            dxs = text1 + "\\" + text2;

            p_Data = devInterface + "\n" +
                     QnxIpAddress + "\n" +
                     QnxCcrtPort + "\n" +
                     WindowsIpAddress + "\n" +
                     Baud_Rate + "\n";

            Encoding enc = Encoding.Default;
            FileStream fs = new FileStream(dxs, FileMode.Create);
            StreamWriter writer = new StreamWriter(fs, enc);
            writer.Write(p_Data, enc);
            writer.Close();
            fs.Close();
        }

        private void Send_Recognition()
        {
            timer2.Start();

            buffer[0] = 33;          //  !
            buffer[1] = 72;          //  H
            buffer[2] = 65;          //  A
            buffer[3] = 76;          //  L    
            buffer[4] = 79;          //  O 
            buffer[5] = 10;          //  LF
            buffer[6] = 13;          //  CR
            buffer[7] = 0;           //  
            Send();
        }

        private void Send()
        {
            string errorMessage;

            try
            {
                serialPort.WriteTimeout = 100;
                serialPort.ReadTimeout = 100;
                serialPort.Write(buffer, 0, buffer.Length);
            }
            catch (Exception ex)
            {
                timer2.Stop();
                errorMessage = "Error: 14\n" +
                               "Can not open the devInterface!\n" +
                               "Please use another ComPort\n" +
                               "Please check USB stick\n" +
                               ex.Message;
                connection = false;

                if (!serialPort.IsOpen)
                {
                    timer1.Stop();
                }

                Console.WriteLine(errorMessage + ex.Message + "\n");
                Logger.Log("Error: " + ex.Message);
            }
        }        
    }
}
