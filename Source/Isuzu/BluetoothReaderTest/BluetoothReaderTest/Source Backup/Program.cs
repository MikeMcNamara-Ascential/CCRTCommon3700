using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.IO;
using System.Linq;
using System.Text;
using System.Timers;
using System.Threading.Tasks;

using System.Net.Sockets;

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
        string measuredValue = "";
        string devInterface = "";
        System.Collections.ArrayList lstCOMPorts = new System.Collections.ArrayList();
        string portNumber;
        int Baud_Rate = 38400;

        Timer timer1 = new Timer(100);
        Timer timer2 = new Timer(100);

        public string Get_Measurement()
        {
            return measuredValue;
        }

        public void Initialize()
        {
            string errorMessage;

            timer1.Elapsed += new ElapsedEventHandler(timer1_Tick);
            timer2.Elapsed += new ElapsedEventHandler(timer2_Tick);

            Read_Options();

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
                }
                timer1.Start();
            }
            catch (Exception ex)
            {
                errorMessage = "Error 1: Can not open a devInterface!\n" +
                            "Close the program, connect the device and switch it on, then restart the program.\n";
                connection = false;
                Console.WriteLine(errorMessage + ex.Message + "\n");
            }
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
                throw;
            }
            finally
            {
                astring = result.Split(';');

                int number = astring.Length;
                if (number > 0)
                {
                    devInterface = astring[0];
                }
                else
                {
                    devInterface = "COM1";
                }

                /*
                if (number < 8)
                {
                    devInterface = "COM1";
                    textSize = 18;
                    windowWidth = 354;
                    windowHeight = 180;
                    textWidth = 300;
                    windowPosX = 100;
                    windowPosY = 100;
                }
                else
                {
                    devInterface = astring[0];
                    textSize = int.Parse(astring[1]);       // unnecessary
                    windowWidth = int.Parse(astring[2]);    // unnecessary
                    windowHeight = int.Parse(astring[3]);   // unnecessary
                    textWidth = int.Parse(astring[4]);      // unnecessary
                    windowPosX = int.Parse(astring[5]);     // unnecessary
                    windowPosY = int.Parse(astring[6]);     // unnecessary
                }
                */
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

                    int testPos = text1.IndexOf(",", 1);    // minutes place always 0
                    if (testPos == -1)
                    {
                        text1 += ",0";
                    }
                    text1 += "°";

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

            /*
            p_Data = portNumber + "\n";
            if (p_Data[0] != 'C')
            {
                p_Data = devInterface + "\n";
            }
            */

            p_Data = devInterface + "\n" +
                     Baud_Rate + "\n";

            /*
            p_Data += textSize.ToString() + "\n" +
                      this.Width.ToString() + "\n" +
                      this.Height.ToString() + "\n" +
                      textBox1.Height.ToString() + "\n" +
                      this.Location.X.ToString() + "\n" +
                      this.Location.Y.ToString() + "\n" +
                      "38400\n";
            */

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
                               "Please check stick\n" +
                               ex.Message;
                connection = false;

                if (!serialPort.IsOpen)
                {
                    timer1.Stop();
                }

                Console.WriteLine(errorMessage + ex.Message + "\n");
            }
        }

        /*
        private void normalToolStripMenuItem_Click(object sender, EventArgs e)
        {
            bool timerSet = false;

            System.Threading.Thread.Sleep(750);
            if (timer1.Enabled)
            {
                timerSet = true;
                timer1.Enabled = false;
            }

            buffer[0] = 33;          //  !
            buffer[1] = 83;          //  S
            buffer[2] = 69;          //  E
            buffer[3] = 78;          //  N    
            buffer[4] = 83;          //  S
            buffer[5] = 32;          //  Space
            buffer[6] = 48;


            buffer[7] = 10;          //  LF
            buffer[8] = 13;          //  CR
            buffer[9] = 3;           //  
            Send();
            System.Threading.Thread.Sleep(1250);

            if (timerSet)
            {
                timer1.Enabled = true;
            }
        }
        */

        /*
        private void invertedToolStripMenuItem_Click(object sender, EventArgs e)
        {
            bool timerSet = false;

            if (timer1.Enabled)
            {
                timerSet = true;
                timer1.Enabled = false;
                System.Threading.Thread.Sleep(750);
            }

            buffer[0] = 33;          //  !
            buffer[1] = 83;          //  S
            buffer[2] = 69;          //  E
            buffer[3] = 78;          //  N    
            buffer[4] = 83;          //  S
            buffer[5] = 32;          //  Space
            buffer[6] = 49;


            buffer[7] = 10;          //  LF
            buffer[8] = 13;          //  CR
            buffer[9] = 3;           //  
            Send();

            System.Threading.Thread.Sleep(250);

            if (timerSet)
            {
                timer1.Enabled = true;
            }
        }
        */

        
    }
}
